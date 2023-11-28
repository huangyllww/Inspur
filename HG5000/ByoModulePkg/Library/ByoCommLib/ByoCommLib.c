/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include <Uefi.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Uefi/UefiSpec.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Atapi.h>
#include <IndustryStandard/Smbios.h>
#include <Uefi/UefiAcpiDataTable.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/ScsiPassThruExt.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/ScsiIo.h>
#include <Library/ByoCommLib.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <IndustryStandard/Acpi.h>
#include <ByoBootGroup.h>
#include <Guid/FileInfo.h>
#include <Guid/FileSystemInfo.h>
#include <Protocol/SimpleFileSystem.h>
#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/BootLogo.h>
#include <Protocol/ComponentName2.h>


#define PCI_DEV_MMBASE(Bus, Device, Function) \
    ( \
      (UINTN)PcdGet64(PcdPciExpressBaseAddress) + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) \
        (Function << 12) \
    )

UINT8 PcieRead8(UINTN PcieAddr)
{
  UINT8   Shift;
  UINT32  Data32;
  
  Data32 = MmioRead32(PcieAddr & ~(BIT0 + BIT1));
  Shift  = (UINT8)((PcieAddr & (BIT0 + BIT1))*8);
  
  return (UINT8)((Data32 >> Shift) & 0xFF);
}


// W = (d+2*m+3*(m+1)/5+y+y/4-y/100+y/400) mod 7
// 0 - Monday
UINT8 CaculateWeekDay(UINT16 y, UINT8 m, UINT8 d)
{
  UINT8 Weekday;
  
  if(m == 1 || m == 2){
    m += 12;
    y--;
  }
  Weekday = (d + 2*m + 3*(m + 1)/5 + y + y/4 - y/100 + y/400) % 7;
  return Weekday;
}  


BOOLEAN IsLeapYear(UINT16 Year)
{
  if (Year % 4 == 0) {
    if (Year % 100 == 0) {
      if (Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}



//------------------------------------ DEBUG -----------------------------------
VOID DumpMem32(const VOID *Base, UINTN Size)
{
#if !defined(MDEPKG_NDEBUG)
  UINTN  Index;
	UINTN  Addr;

  Addr = (UINTN)Base;
  Addr &= ~0x3;

  DEBUG((EFI_D_ERROR, "%a(%X,%X)\n", __FUNCTION__, Base, Size));
  for (Index=0; Index < Size; Index+=4) {
    if(Index%16==0){
      DEBUG((EFI_D_ERROR, "%08X: ", Addr+Index));
    }
    DEBUG((EFI_D_ERROR, "%08X ", MmioRead32(Addr+Index)));
    if((Index+4)%16==0){
      DEBUG((EFI_D_ERROR, "\n"));
    }
  }

  DEBUG((EFI_D_ERROR, "\n"));
#endif  
}


VOID DumpMem8(const VOID *Base, UINTN Size)
{
#if !defined(MDEPKG_NDEBUG)
  const UINT8  *Data8;
        UINTN  Index;

  DEBUG((EFI_D_ERROR, "%a(%X,%X)\n", __FUNCTION__, Base, Size));
  Data8 = (const UINT8*)Base;
  for(Index=0; Index<Size; Index++){
    DEBUG((EFI_D_ERROR, "%02X ", Data8[Index]));
    if(((Index+1)%16)==0){
      DEBUG((EFI_D_ERROR, "\n"));
    }
  }
  DEBUG((EFI_D_ERROR, "\n"));
#endif  
}


VOID DumpCmos()
{
#if !defined(MDEPKG_NDEBUG)
  UINTN  Index;

  DEBUG((EFI_D_ERROR, "%a()\n", __FUNCTION__));
  for(Index=0;Index<128;Index++){
    IoWrite8(0x70, (UINT8)Index);
    DEBUG((EFI_D_ERROR, "%02X ", IoRead8(0x71)));
    if((Index+1)%16==0){
      DEBUG((EFI_D_ERROR, "\n"));
    }
  } 
#endif  
}


VOID DumpPci(UINT8 Bus, UINT8 Dev, UINT8 Func)
{
#if !defined(MDEPKG_NDEBUG)
  UINTN   Index;
  UINTN   Base;

#if 0
  Base = PCI_DEV_MMBASE(Bus, Dev, Func);
  DEBUG((EFI_D_ERROR, "(%02X,%02X,%02X)\n", Bus, Dev, Func));
  for(Index=0;Index<256;Index+=4){
    DEBUG((EFI_D_ERROR, "%08X ", MmioRead32(Base+Index)));
    if((Index+4)%16==0){
      DEBUG((EFI_D_ERROR, "\n"));
    }
  }  
#else
	 Base = PCI_DEV_MMBASE(Bus, Dev, Func);
	 DEBUG((EFI_D_ERROR,"Device(%d, %d, %d):\n	 ",Bus, Dev, Func));
	 for(Index = 0; Index < 16; Index++) DEBUG((EFI_D_ERROR," %02X", Index));
	 DEBUG((EFI_D_ERROR,"\n  +"));
	 for(Index = 0; Index <= 3*16; Index++) DEBUG((EFI_D_ERROR,"-"));
	// DEBUG((EFI_D_ERROR, "\n"));
	// DEBUG((EFI_D_ERROR, "PciDev(%02X,%02X,%02X)\n", Bus, Dev, Func));
	 for(Index=0;Index<256;Index++){
	   if(Index%16==0) DEBUG((EFI_D_ERROR,"\n%02X|", Index/16));
	   DEBUG((EFI_D_ERROR, " %02X", MmioRead8(Base+Index)));
	   //if((Index+4)%16==0){
	   //  DEBUG((EFI_D_ERROR, "\n"));
	   //}
	 }	
	 DEBUG((EFI_D_ERROR,"\n"));
#endif
#endif
}

VOID DumpIo4(UINTN Base, UINT16 Size)
{
#if !defined(MDEPKG_NDEBUG)
  UINT16  Index;

  DEBUG((EFI_D_ERROR, "%a(%X,%X)\n", __FUNCTION__, Base, Size));
  for (Index=0; Index < Size; Index+=4) {
    DEBUG((EFI_D_ERROR, "%08X ", IoRead32(Base+Index)));
    if((Index+4)%16==0){
      DEBUG((EFI_D_ERROR, "\n"));
    }
  }
  DEBUG((EFI_D_ERROR, "\n"));
#endif  
}


VOID DumpAllPciIntLinePin()
{
#if !defined(MDEPKG_NDEBUG)
  UINTN   Base;
  UINT8   Bus, Dev, Func;
  UINT8   IntPin;

  for(Bus=0;Bus<64;Bus++){
    for(Dev=0;Dev<32;Dev++){
      for(Func=0;Func<8;Func++){
        Base = PCI_DEV_MMBASE(Bus, Dev, Func);
        if(MmioRead16(Base+0) == 0xFFFF){    // not present
          if(Func == 0){
            Func = 7;
          }
          continue;
        }
        IntPin = MmioRead8(Base+0x3D);
        if(IntPin != 0){
          DEBUG((EFI_D_ERROR, "(%02X,%02X,%02X) %02X(%c) -> %02X\n",
                               Bus, Dev, Func,
                               IntPin,
                               IntPin + 'A' - 1,
                               MmioRead8(Base+0x3C)
                               ));
        }
        if(Func == 0 && !(MmioRead8(Base + 0xE) & BIT7)){
          Func = 7;
        }
      }
    }
  }
#endif  
}


VOID DumpAllPci()
{
#if !defined(MDEPKG_NDEBUG)
  UINTN   Base;
  UINT8   Bus, Dev, Func;

  for(Bus=0;Bus<64;Bus++){                 // assume max bus number is 64
    for(Dev=0;Dev<32;Dev++){
      for(Func=0;Func<8;Func++){
        Base = PCI_DEV_MMBASE(Bus, Dev, Func);
        if(MmioRead16(Base+0) == 0xFFFF){    // not present
          if(Func == 0){
            Func = 7;
          }
          continue;
        }

        DumpPci(Bus, Dev, Func);

        if(Func == 0 && !(MmioRead8(Base + 0xE) & BIT7)){
          Func = 7;
        }
      }
    }
  }
  DEBUG((EFI_D_ERROR, "\n"));
#endif  
}


VOID DumpHob()
{
#if !defined(MDEPKG_NDEBUG)
  EFI_PEI_HOB_POINTERS  Hob;

  for (Hob.Raw = GetHobList(); !END_OF_HOB_LIST(Hob); Hob.Raw = GET_NEXT_HOB(Hob)) {
	  DEBUG((EFI_D_ERROR, "T:%d A:%X\n", GET_HOB_TYPE(Hob), Hob));
  }
#endif  
}


VOID 
WriteAsmedia (
    IN EFI_BOOT_SERVICES         *BS
  )
{
	EFI_STATUS		             Status;
	UINTN                        HandleCount;
	EFI_HANDLE 		             *HandleBuffer = NULL;
	EFI_HANDLE                   Handle;
	UINTN                        Index;
	EFI_PCI_IO_PROTOCOL          *ptPciIo;
	UINTN			             Segment, Bus, Dev, Func;
	UINT32                       Data32;
	UINTN                        Base;
	BOOLEAN                      IsSATA=FALSE;
	UINT32                       AsmediaAddr;	
	UINT16                       SerrEnable;
	UINT16                       RootCtrlEnable;
	UINT32                       CabliityAddr;
	UINT32                       CabliityEntry;
	UINT16                       AcsAddr;
	UINT16                       AcsDATA;
    UINT8                        IsBridgeDev;

	Status = BS->LocateHandleBuffer(
	                ByProtocol, 
	                &gEfiPciIoProtocolGuid, 
	                NULL, 
	                &HandleCount, 
	                &HandleBuffer
	                );
	if(EFI_ERROR(Status) || HandleCount == 0){
    DEBUG((EFI_D_ERROR, "(L%d) %r %d\n", __LINE__, Status, HandleCount));
    goto ProcExit;
  }

  
	for(Index=0; Index<HandleCount; Index++){
	  Handle = HandleBuffer[Index];
		Status = BS->HandleProtocol(
		                Handle,
		                &gEfiPciIoProtocolGuid, 
		                &ptPciIo
		                );
	
	    Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint8, 0x0E, 1, &IsBridgeDev);
	    IsBridgeDev &= ~BIT7; 
        if(IsBridgeDev == 1){ // only write bridge pcie spece
          //XCL + enable SERR for Ras
          Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint16, 0x3E, 1, &SerrEnable);
          SerrEnable |= BIT1; 
          Status = ptPciIo->Pci.Write(ptPciIo, EfiPciIoWidthUint16, 0x3E, 1, &SerrEnable);
       // }
          //Enable RootCtrl
          Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint16, 0x1C, 1, &RootCtrlEnable);
          RootCtrlEnable |= 7;  //BIT 0/1/2
          Status = ptPciIo->Pci.Write(ptPciIo, EfiPciIoWidthUint16, 0x1C, 1, &RootCtrlEnable);

		  //Enable ACS
		  Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint32, 0x100, 1, &CabliityAddr);
          CabliityEntry = CabliityAddr;
		  while((CabliityEntry&0xffff)!=0x000D){
		  	AcsAddr =(UINT16)(CabliityEntry>>20);
			if(AcsAddr==0){
              break;
			}
		    Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint32, (CabliityEntry>>20), 1, &CabliityEntry);
		  }
		  DEBUG((EFI_D_INFO,"CabliityEntry is %x\n",CabliityEntry));
		  DEBUG((EFI_D_INFO,"AcsAddr is %x\n",AcsAddr));
          if((CabliityEntry&0xffff)==0xD){
		    Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint16, AcsAddr + 6, 1, &AcsDATA);
		    AcsDATA |=0x7F;
		    Status = ptPciIo->Pci.Write(ptPciIo, EfiPciIoWidthUint16, AcsAddr + 6, 1, &AcsDATA);
          }
       } 
		Status = ptPciIo->GetLocation(ptPciIo, &Segment, &Bus, &Dev, &Func);
		Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &Data32);

		if(Data32 != 0x06221B21){	   // invalid vendor id.
			continue;
		}
		Base=PCI_DEV_MMBASE(Bus,Dev,Func);
		
		AsmediaAddr = MmioRead32(Base+0x24);
		DEBUG((EFI_D_INFO,"AsmediaAddr is %x\n",AsmediaAddr));
		if(!IsSATA){
			MmioWrite8(AsmediaAddr+0xf26,0x28); //Write data to F26 register sata
			MmioWrite8(AsmediaAddr+0xefe,0x0); //port 0
			MmioWrite8(AsmediaAddr+0xf26,0x28); //Write data to F26 register sata port0
			DEBUG((EFI_D_INFO,"AsmediaAddr data is %x\n",MmioRead8(AsmediaAddr+0xf26)));
			IsSATA=TRUE;
			continue;
		}
		MmioWrite8(AsmediaAddr+0xf26,0x08); //Write data to F26 register  M.2
		MmioWrite8(AsmediaAddr+0xefe,0x0); //M.2 pORT 0
		MmioWrite8(AsmediaAddr+0xf26,0x08); //Write data to F26 register M.2 PORT 0
		DEBUG((EFI_D_INFO,"AsmediaAddr data is %x\n",MmioRead8(AsmediaAddr+0xf26)));
	}
ProcExit:
	if(HandleBuffer!=NULL){BS->FreePool(HandleBuffer);}
}


VOID 
ListPci (
    IN EFI_BOOT_SERVICES         *BS
  )
{
#if !defined(MDEPKG_NDEBUG)
	EFI_STATUS		               Status;
	UINTN                        HandleCount;
	EFI_HANDLE 		               *HandleBuffer = NULL;
	EFI_HANDLE                   Handle;
	UINTN                        Index;
	EFI_PCI_IO_PROTOCOL          *ptPciIo;
	UINTN			                   Segment, Bus, Dev, Func;
  EFI_DEVICE_PATH_PROTOCOL     *Dp;
  UINT32                       Data32;
	UINT8                        ClassCode[3]; 
  UINT8                        Cmd;
  UINT64                       Attrib;


  DEBUG((EFI_D_INFO, "PciList:\n"));

	Status = BS->LocateHandleBuffer(
	                ByProtocol, 
	                &gEfiPciIoProtocolGuid, 
	                NULL, 
	                &HandleCount, 
	                &HandleBuffer
	                );
	if(EFI_ERROR(Status) || HandleCount == 0){
    DEBUG((EFI_D_ERROR, "(L%d) %r %d\n", __LINE__, Status, HandleCount));
    goto ProcExit;
  }

	for(Index=0; Index<HandleCount; Index++){
	  Handle = HandleBuffer[Index];
		Status = BS->HandleProtocol(
		                Handle,
		                &gEfiPciIoProtocolGuid, 
		                &ptPciIo
		                );
		
		Status = ptPciIo->GetLocation(ptPciIo, &Segment, &Bus, &Dev, &Func);
		Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &Data32);
		if((UINT16)Data32 == 0xFFFF){	   // invalid vendor id.
            DEBUG((EFI_D_ERROR, "(L%d) %r %X\n", __LINE__, Status, Data32));
			continue;
		}

    Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, &ClassCode[0]);
    Status = ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint8, PCI_COMMAND_OFFSET,   1, &Cmd);
    Status = ptPciIo->Attributes(ptPciIo, EfiPciIoAttributeOperationGet, 0, &Attrib);

    if(ClassCode[2] == 6 && ClassCode[1] == 0 && ClassCode[0] == 0){
      continue;
    }
    if(ClassCode[2] == 0x10 && ClassCode[1] == 0x80 && ClassCode[0] == 0){
      continue;
    }
    if(ClassCode[2] == 0x13 && ClassCode[1] == 0x00 && ClassCode[0] == 0){
      continue;
    }

		DEBUG((EFI_D_INFO, "(%02X,%02X,%02X) %08X (%02X,%02X,%02X) %02X %08lX ROM(%08X,%05X) ", Bus, Dev, Func, Data32, \
      ClassCode[2], ClassCode[1], ClassCode[0], Cmd, Attrib, ptPciIo->RomImage, ptPciIo->RomSize));

		Status = BS->HandleProtocol(
		                Handle,
		                &gEfiDevicePathProtocolGuid, 
		                &Dp
		                );
    if(!EFI_ERROR(Status)){
      ShowDevicePathDxe(BS, Dp);
    } else {
      DEBUG((EFI_D_INFO, "\n"));
    }
    
	}

ProcExit:
	if(HandleBuffer!=NULL){BS->FreePool(HandleBuffer);}
#endif  
}







//--------------------------------- CRC32 --------------------------------------
STATIC UINT32 gCrcTable[256] = {
  0x00000000, 
  0x77073096, 
  0xEE0E612C, 
  0x990951BA, 
  0x076DC419, 
  0x706AF48F, 
  0xE963A535, 
  0x9E6495A3, 
  0x0EDB8832, 
  0x79DCB8A4, 
  0xE0D5E91E, 
  0x97D2D988, 
  0x09B64C2B, 
  0x7EB17CBD, 
  0xE7B82D07, 
  0x90BF1D91, 
  0x1DB71064, 
  0x6AB020F2, 
  0xF3B97148, 
  0x84BE41DE, 
  0x1ADAD47D, 
  0x6DDDE4EB, 
  0xF4D4B551, 
  0x83D385C7, 
  0x136C9856, 
  0x646BA8C0, 
  0xFD62F97A, 
  0x8A65C9EC, 
  0x14015C4F, 
  0x63066CD9, 
  0xFA0F3D63, 
  0x8D080DF5, 
  0x3B6E20C8, 
  0x4C69105E, 
  0xD56041E4, 
  0xA2677172, 
  0x3C03E4D1, 
  0x4B04D447, 
  0xD20D85FD, 
  0xA50AB56B, 
  0x35B5A8FA, 
  0x42B2986C, 
  0xDBBBC9D6, 
  0xACBCF940, 
  0x32D86CE3, 
  0x45DF5C75, 
  0xDCD60DCF, 
  0xABD13D59, 
  0x26D930AC, 
  0x51DE003A, 
  0xC8D75180, 
  0xBFD06116, 
  0x21B4F4B5, 
  0x56B3C423, 
  0xCFBA9599, 
  0xB8BDA50F, 
  0x2802B89E, 
  0x5F058808, 
  0xC60CD9B2, 
  0xB10BE924, 
  0x2F6F7C87, 
  0x58684C11, 
  0xC1611DAB, 
  0xB6662D3D, 
  0x76DC4190, 
  0x01DB7106, 
  0x98D220BC, 
  0xEFD5102A, 
  0x71B18589, 
  0x06B6B51F, 
  0x9FBFE4A5, 
  0xE8B8D433, 
  0x7807C9A2, 
  0x0F00F934, 
  0x9609A88E, 
  0xE10E9818, 
  0x7F6A0DBB, 
  0x086D3D2D, 
  0x91646C97, 
  0xE6635C01, 
  0x6B6B51F4, 
  0x1C6C6162, 
  0x856530D8, 
  0xF262004E, 
  0x6C0695ED, 
  0x1B01A57B, 
  0x8208F4C1, 
  0xF50FC457, 
  0x65B0D9C6, 
  0x12B7E950, 
  0x8BBEB8EA, 
  0xFCB9887C, 
  0x62DD1DDF, 
  0x15DA2D49, 
  0x8CD37CF3, 
  0xFBD44C65, 
  0x4DB26158, 
  0x3AB551CE, 
  0xA3BC0074, 
  0xD4BB30E2, 
  0x4ADFA541, 
  0x3DD895D7, 
  0xA4D1C46D, 
  0xD3D6F4FB, 
  0x4369E96A, 
  0x346ED9FC, 
  0xAD678846, 
  0xDA60B8D0, 
  0x44042D73, 
  0x33031DE5, 
  0xAA0A4C5F, 
  0xDD0D7CC9, 
  0x5005713C, 
  0x270241AA, 
  0xBE0B1010, 
  0xC90C2086, 
  0x5768B525, 
  0x206F85B3, 
  0xB966D409, 
  0xCE61E49F, 
  0x5EDEF90E, 
  0x29D9C998, 
  0xB0D09822, 
  0xC7D7A8B4, 
  0x59B33D17, 
  0x2EB40D81, 
  0xB7BD5C3B, 
  0xC0BA6CAD, 
  0xEDB88320, 
  0x9ABFB3B6, 
  0x03B6E20C, 
  0x74B1D29A, 
  0xEAD54739, 
  0x9DD277AF, 
  0x04DB2615, 
  0x73DC1683, 
  0xE3630B12, 
  0x94643B84, 
  0x0D6D6A3E, 
  0x7A6A5AA8, 
  0xE40ECF0B, 
  0x9309FF9D, 
  0x0A00AE27, 
  0x7D079EB1, 
  0xF00F9344, 
  0x8708A3D2, 
  0x1E01F268, 
  0x6906C2FE, 
  0xF762575D, 
  0x806567CB, 
  0x196C3671, 
  0x6E6B06E7, 
  0xFED41B76, 
  0x89D32BE0, 
  0x10DA7A5A, 
  0x67DD4ACC, 
  0xF9B9DF6F, 
  0x8EBEEFF9, 
  0x17B7BE43, 
  0x60B08ED5, 
  0xD6D6A3E8, 
  0xA1D1937E, 
  0x38D8C2C4, 
  0x4FDFF252, 
  0xD1BB67F1, 
  0xA6BC5767, 
  0x3FB506DD, 
  0x48B2364B, 
  0xD80D2BDA, 
  0xAF0A1B4C, 
  0x36034AF6, 
  0x41047A60, 
  0xDF60EFC3, 
  0xA867DF55, 
  0x316E8EEF, 
  0x4669BE79, 
  0xCB61B38C, 
  0xBC66831A, 
  0x256FD2A0, 
  0x5268E236, 
  0xCC0C7795, 
  0xBB0B4703, 
  0x220216B9, 
  0x5505262F, 
  0xC5BA3BBE, 
  0xB2BD0B28, 
  0x2BB45A92, 
  0x5CB36A04, 
  0xC2D7FFA7, 
  0xB5D0CF31, 
  0x2CD99E8B, 
  0x5BDEAE1D, 
  0x9B64C2B0, 
  0xEC63F226, 
  0x756AA39C, 
  0x026D930A, 
  0x9C0906A9, 
  0xEB0E363F, 
  0x72076785, 
  0x05005713, 
  0x95BF4A82, 
  0xE2B87A14, 
  0x7BB12BAE, 
  0x0CB61B38, 
  0x92D28E9B, 
  0xE5D5BE0D, 
  0x7CDCEFB7, 
  0x0BDBDF21, 
  0x86D3D2D4, 
  0xF1D4E242, 
  0x68DDB3F8, 
  0x1FDA836E, 
  0x81BE16CD, 
  0xF6B9265B, 
  0x6FB077E1, 
  0x18B74777, 
  0x88085AE6, 
  0xFF0F6A70, 
  0x66063BCA, 
  0x11010B5C, 
  0x8F659EFF, 
  0xF862AE69, 
  0x616BFFD3, 
  0x166CCF45, 
  0xA00AE278, 
  0xD70DD2EE, 
  0x4E048354, 
  0x3903B3C2, 
  0xA7672661, 
  0xD06016F7, 
  0x4969474D, 
  0x3E6E77DB, 
  0xAED16A4A, 
  0xD9D65ADC, 
  0x40DF0B66, 
  0x37D83BF0, 
  0xA9BCAE53, 
  0xDEBB9EC5, 
  0x47B2CF7F, 
  0x30B5FFE9, 
  0xBDBDF21C, 
  0xCABAC28A, 
  0x53B39330, 
  0x24B4A3A6, 
  0xBAD03605, 
  0xCDD70693, 
  0x54DE5729, 
  0x23D967BF, 
  0xB3667A2E, 
  0xC4614AB8, 
  0x5D681B02, 
  0x2A6F2B94, 
  0xB40BBE37, 
  0xC30C8EA1, 
  0x5A05DF1B, 
  0x2D02EF8D
};


EFI_STATUS 
EFIAPI
LibCalcCrc32 (
  IN  VOID    *Data,
  IN  UINTN   DataSize,
  OUT UINT32  *CrcOut
  )
{
  UINT32  Crc;
  UINTN   Index;
  UINT8   *Ptr;

  if (Data == NULL || DataSize == 0 || CrcOut == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Crc = 0xffffffff;
  for (Index = 0, Ptr = Data; Index < DataSize; Index++, Ptr++) {
    Crc = (Crc >> 8) ^ gCrcTable[(UINT8) Crc ^ *Ptr];
  }

  *CrcOut = Crc ^ 0xffffffff;
  return EFI_SUCCESS;
}


BOOLEAN LibVerifyDataCrc32(VOID *Data, UINTN DataSize, UINTN CrcOffset, UINT32 *CalcCrc32 OPTIONAL)
{
  UINT32      OldCrc32;
  UINT32      Crc32;
  EFI_STATUS  Status;
  BOOLEAN     rc;
  UINT32      *CrcPos;
  
  CrcPos   = (UINT32*)((UINT8*)Data+CrcOffset);
  OldCrc32 = *CrcPos;
  *CrcPos  = 0;
  rc       = FALSE;
  
  Status  = LibCalcCrc32(Data, DataSize, &Crc32);
  *CrcPos = OldCrc32;
  if(CalcCrc32 != NULL){
    *CalcCrc32 = Crc32;
  }
    
  if((!EFI_ERROR(Status)) && (Crc32==OldCrc32)){
    rc = TRUE;
  }

  return rc;
}











// I am a common library, I can not use Arch dependent library.
// So below gBS should be passed from caller.
VOID *LibGetGOP(EFI_BOOT_SERVICES *pBS)
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *ptGO;
  VOID                                  *Interface;
  EFI_STATUS                            Status;  
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 HandleCount;
  UINTN                                 Index;  

  HandleBuffer = NULL;
  ptGO         = NULL;
  
  Status = pBS->LocateHandleBuffer (
              ByProtocol,
              &gEfiGraphicsOutputProtocolGuid,
              NULL,
              &HandleCount,
              &HandleBuffer
              );
  if(EFI_ERROR(Status) || HandleCount==0){
    goto ProcExit;
  }  
              
  for(Index=0;Index<HandleCount;Index++){
    Status = pBS->HandleProtocol(
               HandleBuffer[Index], 
               &gEfiConsoleOutDeviceGuid, 
               &Interface
               );
    if(EFI_ERROR(Status)){
      continue;
    }
    Status = pBS->HandleProtocol(
               HandleBuffer[Index], 
               &gEfiGraphicsOutputProtocolGuid, 
               &ptGO
               );
    ASSERT(!EFI_ERROR(Status) && ptGO!=NULL);               
  }

ProcExit:
  pBS->FreePool(HandleBuffer);
  return (VOID*)ptGO;
}



EFI_STATUS BltSaveAndRetore(VOID *BootServices, BOOLEAN Save)
{
         EFI_STATUS                     Status = EFI_SUCCESS;
         EFI_GRAPHICS_OUTPUT_PROTOCOL   *ptGO;
  STATIC EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *gBltBuffer = NULL;
         UINTN                          Width;
         UINTN                          Height;
         EFI_BOOT_SERVICES              *pBS;      
  

  pBS = (EFI_BOOT_SERVICES*)BootServices;
  ASSERT(pBS->Hdr.Signature == EFI_BOOT_SERVICES_SIGNATURE);
  
  ptGO = (EFI_GRAPHICS_OUTPUT_PROTOCOL*)LibGetGOP(pBS);
  if(ptGO == NULL){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }    
  
  Width  = ptGO->Mode->Info->HorizontalResolution;
  Height = ptGO->Mode->Info->VerticalResolution;

  if(Save){
    if(gBltBuffer != NULL){
      pBS->FreePool(gBltBuffer);
      gBltBuffer = NULL;
    }  
    Status = pBS->AllocatePool(
                    EfiBootServicesData,
                    sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)*Width*Height,
                    &gBltBuffer
                    );
    if(EFI_ERROR(Status)){
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;		
    }	

    Status = ptGO->Blt(
                    ptGO,
                    gBltBuffer,
                    EfiBltVideoToBltBuffer,
                    0,
                    0,
                    0,
                    0,
                    Width,
                    Height,
                    Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)   // the number of bytes in a row of the BltBuffer.
                    );  
    DEBUG((EFI_D_INFO, "BltSave:%r\n", Status));    
  } else {
    
    if(gBltBuffer == NULL){
      Status = EFI_INVALID_PARAMETER;
      goto ProcExit;	      
    }
    Status = ptGO->Blt(
                    ptGO,
                    gBltBuffer,
                    EfiBltBufferToVideo,
                    0,
                    0,
                    0,
                    0,
                    Width,
                    Height,
                    Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                    );
    DEBUG((EFI_D_INFO, "BltRestore:%r\n", Status));
    pBS->FreePool(gBltBuffer);
    gBltBuffer = NULL;    
  }
	
ProcExit:
  return Status;	
}



VOID 
AcpiTableUpdateChksum (
  IN VOID *AcpiTable
  )
{
  UINTN  ChecksumOffset;
  UINT8  *Buffer;
  EFI_ACPI_COMMON_HEADER  *Hdr;

  Hdr = (EFI_ACPI_COMMON_HEADER*)AcpiTable;
  if(Hdr->Signature == EFI_ACPI_2_0_FIRMWARE_ACPI_CONTROL_STRUCTURE_SIGNATURE){
    return;
  }  

  Buffer = (UINT8*)Hdr;
  ChecksumOffset = OFFSET_OF(EFI_ACPI_DESCRIPTION_HEADER, Checksum);
  Buffer[ChecksumOffset] = 0;
  Buffer[ChecksumOffset] = CalculateCheckSum8(Buffer, Hdr->Length);
}



BOOLEAN
IsMyDevicePathEnd (
  IN EFI_DEVICE_PATH_PROTOCOL    *Dp
  )
{
  ASSERT (Dp != NULL);
  return (Dp->Type == END_DEVICE_PATH_TYPE && Dp->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE);
}


EFI_DEVICE_PATH_PROTOCOL *
MyNextDevicePathNode (
  IN CONST VOID  *Node
  )
{
  UINTN                     NodeSize;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
 

  ASSERT (Node != NULL);

  Dp = (EFI_DEVICE_PATH_PROTOCOL*)Node;
  NodeSize = (Dp->Length[1]<<8) + Dp->Length[0];
  
  return (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)(Node) + NodeSize);
}


UINTN
MyGetDevicePathSize (
  IN CONST EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  CONST EFI_DEVICE_PATH_PROTOCOL  *Start;
        UINTN                     Count = 0;
        UINTN                     NodeSize;

  if (DevicePath == NULL) {
    return 0;
  }

  //
  // Search for the end of the device path structure
  //
  Start = DevicePath;
  while (!(DevicePath->Type == END_DEVICE_PATH_TYPE && 
           DevicePath->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE)) {
    NodeSize = (DevicePath->Length[1]<<8) + DevicePath->Length[0];
    if(NodeSize < 4){
      return 0;
    }
    Count++;
    if(Count > 1000){
      return 0;
    }
    DevicePath = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)DevicePath + NodeSize);
  }

  return ((UINTN) DevicePath - (UINTN)Start) + (DevicePath->Length[1]<<8) + DevicePath->Length[0];
}



EFI_DEVICE_PATH_PROTOCOL *
MyDuplicateDevicePath(
  IN EFI_DEVICE_PATH_PROTOCOL    *DevicePath
  )
{
  UINTN                     Size;

  //
  // Compute the size
  //
  Size = MyGetDevicePathSize (DevicePath);
  if (Size == 0) {
    return NULL;
  }

  //
  // Allocate space for duplicate device path
  //
  return AllocateCopyPool (Size, DevicePath);
}




VOID
ShowDevicePathDxe (
  IN EFI_BOOT_SERVICES         *BS,
  IN EFI_DEVICE_PATH_PROTOCOL  *ptDevPath
  )
{
  EFI_STATUS                               Status;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL         *ptDP2Txt;
  CHAR16                                   *Str;
  CHAR16                                   *Alloc = NULL;


  if(ptDevPath==NULL || BS == NULL){
    goto ProcExit;  
  }

  Status = BS->LocateProtocol(
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  &ptDP2Txt
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  
  if(ptDevPath->Type == END_DEVICE_PATH_TYPE &&
     ptDevPath->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE){
    Str = L"(End)";
  }else{
    Str = ptDP2Txt->ConvertDevicePathToText (
                      ptDevPath,
                      FALSE,
                      TRUE
                      );
    Alloc = Str;
  }
  
  if(Str!=NULL){
    DEBUG((EFI_D_INFO, "DP:%s\n", Str));
  }

ProcExit:
  if(Alloc!=NULL){BS->FreePool(Alloc);}
  return;
}





VOID
SwapWordArray (
  IN  UINT8   *Data,
  IN  UINTN   DataSize
  )
{
  UINTN  Index;
  UINT8  Data8;

  ASSERT(DataSize!=0 && Data!=NULL);

// here has a hidden bug:
// Data size may be odd number, so it will destroy data out of range.
  DataSize &= ~BIT0;          // Fix: make it be even.
  for(Index = 0; Index < DataSize; Index += 2){
    Data8         = Data[Index];
    Data[Index]   = Data[Index+1];
    Data[Index+1] = Data8;
  }

}



CHAR8 *TrimStr8(CHAR8 *Str)
{
  UINTN    StringLength;
  CHAR8    *NewString;
  BOOLEAN  HasLeading;
  UINTN    SrcIndex;
  UINTN    TarIndex;
  
  NewString = NULL;

  StringLength = AsciiStrLen(Str);
  if(StringLength==0){
    return Str; 
  }

  NewString = AllocatePool(StringLength + 1);
  if(NewString==NULL){
    return Str; 
  }

  HasLeading = TRUE;
  TarIndex   = 0;
  SrcIndex   = 0;
  for(;SrcIndex<StringLength;SrcIndex++){
    if(HasLeading){
      if(Str[SrcIndex] == ' '){
        continue;
      }else{
        HasLeading = FALSE;
      }
    }
    if(TarIndex>0){
      if((Str[SrcIndex-1]==Str[SrcIndex]) && (Str[SrcIndex]==' ')){
        continue;
      }
    }
    NewString[TarIndex++] = Str[SrcIndex];
  }

  if(TarIndex && NewString[TarIndex-1] == L' '){
    TarIndex--;
  }
  NewString[TarIndex] = 0;
  ASSERT(TarIndex<=StringLength);
  CopyMem(Str, NewString, (TarIndex+1)*sizeof(Str[0]));
  FreePool(NewString);
  return Str;
}



CHAR16 *TrimStr16(CHAR16 *Str)
{
  UINTN    StringSize;
  UINTN    StringLength;
  CHAR16   *NewString;
  BOOLEAN  HasLeading;
  UINTN    SrcIndex;
  UINTN    TarIndex;
  
  NewString = NULL;
  
  StringSize   = StrSize(Str);
  StringLength = StrLen(Str);
  if(StringLength==0){
    return Str; 
  }

  NewString = AllocatePool(StringSize);
  if(NewString==NULL){
    return Str; 
  }

	HasLeading = TRUE;
	TarIndex   = 0;
	SrcIndex   = 0;
	for(;SrcIndex<StringLength;SrcIndex++){
		if(HasLeading){
			if(Str[SrcIndex] == L' '){
				continue;
			}else{
				HasLeading = FALSE;
			}
		}
		if(TarIndex>0){
			if((Str[SrcIndex-1]==Str[SrcIndex]) && (Str[SrcIndex]==L' ')){
				continue;
			}
		}
		NewString[TarIndex++] = Str[SrcIndex];
	}

  if(TarIndex && NewString[TarIndex-1] == L' '){
    TarIndex--;
  }
  NewString[TarIndex] = 0;
  ASSERT(TarIndex<=StringLength);
  CopyMem(Str, NewString, (TarIndex+1)*sizeof(CHAR16));
  FreePool(NewString);
	
  return Str;
}




STATIC UINT8 gInvalidMacAddress1[6] = {0, 0, 0, 0, 0, 0};
STATIC UINT8 gInvalidMacAddress2[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};


// intel i350  (ethernet-controller-i350-datasheet P450)
// 0x5400 + 8*n   0x0040 + 8*n    RAL[0-15]     Receive Address Low (15:0) RW
// 0x5404 + 8*n   0x0044 + 8*n    RAH[0-15]     Receive Address High(15:0) RW
// 
// RAL [31:0]
// RAH [15:0]

VOID SwapLanMacAddress(UINT8 *MacAddr)
{
  UINT8  d[6];

  CopyMem(d, MacAddr, 6);
  MacAddr[0] = d[5];
  MacAddr[1] = d[4];  
  MacAddr[2] = d[3];
  MacAddr[3] = d[2];
  MacAddr[4] = d[1];  
  MacAddr[5] = d[0];  
}


EFI_STATUS
GetOnboardLanMacAddress (
  IN  EFI_BOOT_SERVICES         *pBS,
  IN  VOID                      *Dp,
  OUT UINT8                     MacAddr[6]
  )  
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
	EFI_PCI_IO_PROTOCOL       *PciIo; 
  UINT16                    PciId;
  UINT16                    DevId;
  UINT32                    Data32;
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  Data32 = 0;
  DevPath = (EFI_DEVICE_PATH_PROTOCOL*)Dp;

  ShowDevicePathDxe(pBS, DevPath);

  Status  = pBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &DevPath, &Handle);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  
  if(!IsMyDevicePathEnd(DevPath)){
    Status = EFI_NOT_FOUND;
    goto ProcExit;    
  }
  
  Status = pBS->HandleProtocol(
                  Handle,
                  &gEfiPciIoProtocolGuid, 
                  &PciIo
                  );
  ASSERT(!EFI_ERROR(Status));

  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 0, 1, &PciId);
  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, 2, 1, &DevId);
  DEBUG((EFI_D_INFO, "PciId:%X\n", PciId));  
  if(PciId == 0x10EC){
    Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_IO, NULL);
    Status = PciIo->Io.Read(PciIo, EfiPciIoWidthUint8, 0, 0, 6, MacAddr);
  } else if(PciId == 0x8086){
    Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
	if(DevId == 0x1557 || DevId == 0x10fb || DevId == 0x1528 || DevId==0x1563){
      Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x5400, 6, MacAddr);
	}else if(DevId == 0x1572){
	  Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e4440, 4, MacAddr);
      Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e44c0, 2, &MacAddr[4]);
    }else {
      Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x40, 6, MacAddr);
    }
  } else if(PciId == 0x14E4){ //Braodcom 
    DEBUG((EFI_D_INFO, "DevId:%X\n", DevId));  
    if(DevId == 0x168E){  //NetXtreme II  Gigabit Ethernet
       Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
       Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x8010, 4, &Data32);
       MacAddr[0] = (UINT8)(Data32>>8);
       MacAddr[1] = (UINT8)Data32;
       Data32 = 0;
       Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x8014, 4, &Data32);            
       MacAddr[2] = (UINT8)(Data32 >> 24);
       MacAddr[3] = (UINT8)(Data32>>16);
       MacAddr[4] = (UINT8)(Data32>>8);
       MacAddr[5] = (UINT8)Data32;
    }
  } else if(PciId == 0x8088){
     Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
     Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x16200, 6, MacAddr);
     SwapLanMacAddress(MacAddr);
  }else {
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  if(CompareMem(MacAddr, gInvalidMacAddress1, 6) == 0 ||
     CompareMem(MacAddr, gInvalidMacAddress2, 6) == 0){
    Status = EFI_UNSUPPORTED;
    goto ProcExit;    
  }  

ProcExit:
  DEBUG((EFI_D_INFO, "%r\n", Status));
  return Status;
}



EFI_STATUS
GetPciNicMac (
  EFI_PCI_IO_PROTOCOL    *PciIo,
  UINT8    *MacAddr
  )
{
  EFI_STATUS    Status;
  UINT16    VenderId, DeviceId;
  UINT8    ClassCode[3];
  UINT32    Data32 = 0;
  UINT8    CapabilityID = 0, CapabilityPtr = 0;
  UINT16    CapabilityEntry = 0;
  UINT16    GetMac = 0xf, Init = 0x4;

  if (PciIo == NULL ||MacAddr == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 9, 3, &ClassCode);
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), ClassCode :%d-%d-%d.\n", ClassCode[0], ClassCode[1], ClassCode[2]));
  if ( ClassCode[2] != PCI_CLASS_NETWORK) {
    return EFI_NOT_FOUND;
  }

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0, 1, &VenderId);
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), VenderId :0x%x.\n", VenderId));
  if (VenderId == 0x1D94) { // skip bridge + Base system peripherals + Encryption/Decryption controllers
    return EFI_NOT_FOUND;
  }
  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x02, 1, &DeviceId);
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), DeviceId :0x%x.\n", DeviceId));

  Status = EFI_SUCCESS;
  switch(VenderId) {
    case 0x10EC:
      Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_IO, NULL);
      Status = PciIo->Io.Read(PciIo, EfiPciIoWidthUint8, 0, 0, 6, MacAddr);
      break;

    case 0x8086:
      Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
      if(DeviceId == 0x1557 || DeviceId == 0x10fb || DeviceId == 0x1528 ||DeviceId == 0x1563){
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x5400, 6, MacAddr);
      }else if(DeviceId == 0x1572){
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e4440, 4, MacAddr);
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e44c0, 2, &MacAddr[4]);
      }else {
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x40, 6, MacAddr);
      }
      break;

    case 0x14E4:
      if(DeviceId == 0x168E){	//NetXtreme II	Gigabit Ethernet
        Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x8010, 4, &Data32);
        MacAddr[0] = (UINT8)(Data32>>8);
        MacAddr[1] = (UINT8)Data32;
        Data32 = 0;
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x8014, 4, &Data32);			
        MacAddr[2] = (UINT8)(Data32 >> 24);
        MacAddr[3] = (UINT8)(Data32>>16);
        MacAddr[4] = (UINT8)(Data32>>8);
        MacAddr[5] = (UINT8)Data32;
      }
      break;

    case 0x8088:
      Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
      Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x16200, 6, MacAddr);
      SwapLanMacAddress(MacAddr);
      break;

    case 0x15b3:
      Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);
      while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapabilityPtr, 1, &CapabilityEntry);
        CapabilityID = (UINT8) CapabilityEntry;
        DEBUG((EFI_D_INFO,"NETCARD CapabilityID = %0x\n",CapabilityID));
        if (0x9 == CapabilityID) {
          Status = PciIo->Pci.Write(PciIo, EfiPciIoWidthUint16, CapabilityPtr + 0x04, 1, &GetMac);
          Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, CapabilityPtr + 0x10, 4, &Data32);
          MacAddr[0] = (UINT8)(Data32>>8);
          MacAddr[1] = (UINT8)Data32;
          Data32 = 0;
          Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, CapabilityPtr + 0x14, 4, &Data32);			
          MacAddr[2] = (UINT8)(Data32 >> 24);
          MacAddr[3] = (UINT8)(Data32>>16);
          MacAddr[4] = (UINT8)(Data32>>8);
          MacAddr[5] = (UINT8)Data32;
          Status = PciIo->Pci.Write(PciIo, EfiPciIoWidthUint16, CapabilityPtr + 0x04, 1, &Init);
          break;
        }
        CapabilityPtr = (UINT8)(CapabilityEntry >> 8);
      }		
      break;

    default:
      Status = EFI_NOT_FOUND;
      break;
  }  

  DEBUG((EFI_D_ERROR, __FUNCTION__"(), MacAddr :%x-%x-%x-%x-%x-%x.\n", MacAddr[0],MacAddr[1],MacAddr[2],MacAddr[3],MacAddr[4],MacAddr[5]));
  return Status;  
}



BOOLEAN IsSataDp(VOID *DevicePath)
{
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  UINTN                     NodeSize;
  

  ASSERT(DevicePath != NULL);

  Dp = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;
  
  while(!(Dp->Type == END_DEVICE_PATH_TYPE &&
          Dp->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE)){
    if(Dp->Type == MESSAGING_DEVICE_PATH){
      if(Dp->SubType == MSG_SATA_DP){
        return TRUE;
      } else {
        return FALSE;
      }
    }

    NodeSize = (Dp->Length[1]<<8) + Dp->Length[0];
    Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)Dp + NodeSize);
  }

  return FALSE;
}


BOOLEAN IsUsbDp(VOID *DevicePath)
{
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  UINTN                     NodeSize;
  

  ASSERT(DevicePath != NULL);

  Dp = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;
  
  while(!(Dp->Type == END_DEVICE_PATH_TYPE &&
          Dp->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE)){
    if(Dp->Type == MESSAGING_DEVICE_PATH){
      if(Dp->SubType == MSG_USB_DP){
        return TRUE;
      } else {
        return FALSE;
      }
    }

    NodeSize = (Dp->Length[1]<<8) + Dp->Length[0];
    Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)Dp + NodeSize);
  }

  return FALSE;
}



UINT8 GetDeviceTypeFromDp(VOID *DevicePath)
{
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  UINTN                     NodeSize;
  

  ASSERT(DevicePath != NULL);

  Dp = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;
  
  while(!(Dp->Type == END_DEVICE_PATH_TYPE &&
          Dp->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE)){
    if(Dp->Type == MESSAGING_DEVICE_PATH){
      return Dp->SubType;
    }

    NodeSize = (Dp->Length[1]<<8) + Dp->Length[0];
    Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINT8*)Dp + NodeSize);
  }

  return 0xFF;
}


UINT32 GetPartitionNumberFromDp(VOID *DevicePath)
{
  UINTN                   NodeSize;
  HARDDRIVE_DEVICE_PATH   *HddDp;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;

  ASSERT(DevicePath != NULL);
  Dp = (EFI_DEVICE_PATH_PROTOCOL*)DevicePath;
  
  while(1){
    if(Dp->Type == END_DEVICE_PATH_TYPE && Dp->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE){
      break;
    }
    if(Dp->Type == MEDIA_DEVICE_PATH && Dp->SubType == MEDIA_HARDDRIVE_DP){
      HddDp = (HARDDRIVE_DEVICE_PATH*)Dp;
      return HddDp->PartitionNumber;
    }

    NodeSize = (Dp->Length[1]<<8) + Dp->Length[0];
    Dp = (EFI_DEVICE_PATH_PROTOCOL*)((UINTN)Dp + NodeSize);
  }

  return 0;
}




BOOLEAN 
IsUefiAhciHddDp(
  IN  EFI_BOOT_SERVICES       *pBS,
  IN  VOID                    *DevicePath,
  OUT VOID                    **pDiskInfo  OPTIONAL
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *Dp;
  EFI_STATUS                    Status;
  EFI_HANDLE                    DevHandle;  
  EFI_SCSI_IO_PROTOCOL          *ScsiIo;  
  EFI_DISK_INFO_PROTOCOL        *DiskInfo;
  BOOLEAN                       Rc = FALSE;


  ASSERT(DevicePath != NULL);

  Dp = DevicePath;
  Status = pBS->LocateDevicePath(&gEfiDiskInfoProtocolGuid, &Dp, &DevHandle);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
    goto ProcExit;
  }

  Status = pBS->HandleProtocol (
                  DevHandle,
                  &gEfiScsiIoProtocolGuid,
                  (VOID**)&ScsiIo
                  );
  if(!EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "Ignore ScsiIo\n"));
    goto ProcExit;
  }
  
  Status = pBS->HandleProtocol (
                  DevHandle,
                  &gEfiDiskInfoProtocolGuid,
                  (VOID**)&DiskInfo
                  );
  ASSERT(!EFI_ERROR(Status));

  if(!CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid)){
    DEBUG((EFI_D_INFO, "Not Ahci\n"));
    goto ProcExit;
  }

  Rc = TRUE;
  if(pDiskInfo != NULL){
    *pDiskInfo = (VOID*)DiskInfo;
  }

ProcExit:
  return Rc;
}



VOID*
AllocateRunTimeZeroMemoryBelow4G (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer = NULL;

  Pages = EFI_SIZE_TO_PAGES(Size);
  Address = 0xffffffff;

  Status  = BS->AllocatePages (
                   AllocateMaxAddress,
                   EfiRuntimeServicesData,
                   Pages,
                   &Address
                   );
  if(EFI_ERROR(Status)){
    return NULL;
  }

  Buffer = (VOID*)(UINTN)Address;
  ZeroMem(Buffer, Size);

  return Buffer;
}



VOID*
AllocateAcpiNvsZeroMemoryBelow4G (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer;

  Pages = EFI_SIZE_TO_PAGES(Size);
  Address = 0xffffffff;

  Status  = BS->AllocatePages (
                   AllocateMaxAddress,
                   EfiACPIMemoryNVS,
                   Pages,
                   &Address
                   );
  ASSERT_EFI_ERROR(Status);
  if(EFI_ERROR(Status)){
    return NULL;
  }

  Buffer = (VOID*)(UINTN)Address;
  ZeroMem(Buffer, Size);

  return Buffer;
}



VOID*
AllocateReservedZeroMemoryBelow4G (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer = NULL;

  Pages = EFI_SIZE_TO_PAGES(Size);
  Address = 0xffffffff;

  Status  = BS->AllocatePages (
                   AllocateMaxAddress,
                   EfiReservedMemoryType,
                   Pages,
                   &Address
                   );
  if(EFI_ERROR(Status)){
    return NULL;
  }

  Buffer = (VOID*)(UINTN)Address;
  ZeroMem(Buffer, Size);

  return Buffer;
}


VOID*
AllocateReservedZeroMemoryBelow1M (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer = NULL;

  Pages = EFI_SIZE_TO_PAGES(Size);
  Address = 0xfffff;

  Status  = BS->AllocatePages (
                   AllocateMaxAddress,
                   EfiReservedMemoryType,
                   Pages,
                   &Address
                   );
  if(EFI_ERROR(Status)){
    return NULL;
  }

  Buffer = (VOID*)(UINTN)Address;
  ZeroMem(Buffer, Size);

  return Buffer;
}


VOID*
AllocateBsZeroMemoryBelow4G (
  IN EFI_BOOT_SERVICES  *BS,
  IN UINTN              Size
  )
{
  UINTN                 Pages;
  EFI_PHYSICAL_ADDRESS  Address;
  EFI_STATUS            Status;
  VOID*                 Buffer = NULL;

  Pages = EFI_SIZE_TO_PAGES(Size);
  Address = 0xffffffff;

  Status  = BS->AllocatePages (
                   AllocateMaxAddress,
                   EfiBootServicesData,
                   Pages,
                   &Address
                   );
  if(EFI_ERROR(Status)){
    return NULL;
  }

  Buffer = (VOID*)(UINTN)Address;
  ZeroMem(Buffer, Size);

  return Buffer;
}



EFI_STATUS InvokeHookProtocol(EFI_BOOT_SERVICES *BS, EFI_GUID *Protocol)
{
  UINTN                 HandleCount;
  EFI_HANDLE            *Handles = NULL;
  EFI_STATUS            Status;
  UINTN                 Index;
  EFI_MY_HOOK_PROTOCOL  MyHook;
  

  Status = BS->LocateHandleBuffer (
                 ByProtocol,
                 Protocol,
                 NULL,
                 &HandleCount,
                 &Handles
                 );
  if(EFI_ERROR(Status) || HandleCount == 0){
    goto ProcExit;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = BS->HandleProtocol (
                    Handles[Index],
                    Protocol,
                    (VOID**)&MyHook
                    );
    ASSERT(!EFI_ERROR(Status));
    MyHook();
  }

ProcExit:
  if(Handles != NULL){
    BS->FreePool(Handles);
  }
  return Status;
}



VOID
SignalProtocolEvent (
  EFI_BOOT_SERVICES *BS,
  EFI_GUID          *ProtocolGuid,
  BOOLEAN           NeedUnInstall
  )
{
  EFI_HANDLE     Handle;
  EFI_STATUS     Status;


  Handle = NULL;
  Status = BS->InstallProtocolInterface (
                  &Handle,
                  ProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  if(NeedUnInstall){
    Status = BS->UninstallProtocolInterface (
                    Handle,
                    ProtocolGuid,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status); 
  }
}



EFI_DEVICE_PATH_PROTOCOL *
GetPciParentDp (
    IN     EFI_BOOT_SERVICES         *BS,
    IN     EFI_DEVICE_PATH_PROTOCOL  *PciDp
  )
{
  UINTN                     DpSize;
  EFI_DEVICE_PATH_PROTOCOL  *ParentDp = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  EFI_DEVICE_PATH_PROTOCOL  *Temp;
  EFI_STATUS                Status;
  

//ShowDevicePathDxe(BS, PciDp);

  Dp   = PciDp;  
  Temp = PciDp;
  while(!IsMyDevicePathEnd(Dp)){
    Temp = Dp;
    Dp = MyNextDevicePathNode(Dp);
  }

  DpSize = (UINTN)Temp - (UINTN)PciDp + sizeof(EFI_DEVICE_PATH_PROTOCOL);
  Status = BS->AllocatePool(EfiBootServicesData, DpSize, (VOID**)&ParentDp);
  ASSERT(!EFI_ERROR(Status));
  if(EFI_ERROR(Status)){
    return NULL;
  }

  CopyMem(ParentDp, PciDp, DpSize - sizeof(EFI_DEVICE_PATH_PROTOCOL));
  CopyMem((UINT8*)ParentDp + DpSize - sizeof(EFI_DEVICE_PATH_PROTOCOL), Dp, sizeof(EFI_DEVICE_PATH_PROTOCOL));

//ShowDevicePathDxe(BS, ParentDp);

  return ParentDp;
}



EFI_HANDLE 
GetPciHandleFromBDF (
  EFI_BOOT_SERVICES         *BS,
  UINTN                     MyBus, 
  UINTN                     MyDev, 
  UINTN                     MyFunc
  )
{
  UINTN                 HandleCount;
  EFI_HANDLE            *Handles = NULL;
  EFI_STATUS            Status;
  UINTN                 Index;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINTN                 Bus, Dev, Func, Seg;
  EFI_HANDLE            PciHandle = NULL;
  

  Status = BS->LocateHandleBuffer (
                 ByProtocol,
                 &gEfiPciIoProtocolGuid,
                 NULL,
                 &HandleCount,
                 &Handles
                 );
  if(EFI_ERROR(Status) || HandleCount == 0){
    goto ProcExit;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = BS->HandleProtocol (
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID**)&PciIo
                    );
    PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);

    if(MyBus == Bus && MyDev == Dev && MyFunc == Func){
      PciHandle = Handles[Index];
      break;
    }
  }

ProcExit:
  if(Handles != NULL){
    BS->FreePool(Handles);
  }
  return PciHandle;
}


BOOLEAN 
GetPciClassCodeFromBDF(
  EFI_BOOT_SERVICES         *BS,
  UINTN                     MyBus, 
  UINTN                     MyDev, 
  UINTN                     MyFunc,
  UINT8                     ClassCode[3]
  )
{
  EFI_HANDLE            PciHandle;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  ;


  PciHandle = GetPciHandleFromBDF(BS, MyBus, MyDev, MyFunc);
  if(PciHandle == NULL){
    return FALSE;
  }

  BS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 9, 3, ClassCode);
  return TRUE;
}






EFI_STATUS 
GetObSataHostDp(
  EFI_BOOT_SERVICES        *BS, 
  UINTN                    HostIndex, 
  EFI_DEVICE_PATH_PROTOCOL **HostDp
  )
{
  EFI_STATUS                Status;
  PLAT_HOST_INFO_PROTOCOL   *ptHostInfo;  
  UINTN                     Index;
  UINTN                     SataHostIndex = 0;


  Status = BS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  if(EFI_ERROR(Status)){
    return EFI_NOT_FOUND;
  }

  for(Index=0;Index<ptHostInfo->HostCount;Index++){
    if(ptHostInfo->HostList[Index].HostType == PLATFORM_HOST_SATA){
      if(SataHostIndex == HostIndex){
        *HostDp = ptHostInfo->HostList[Index].Dp;
        return EFI_SUCCESS;
      }
      SataHostIndex++;
    }
  }

  return EFI_NOT_FOUND;  
}


EFI_STATUS InvokeHookProtocol2(EFI_BOOT_SERVICES *BS, EFI_GUID *Protocol, VOID *Param)
{
  UINTN                  HandleCount;
  EFI_HANDLE             *Handles = NULL;
  EFI_STATUS             Status;
  UINTN                  Index;
  EFI_MY_HOOK_PROTOCOL2  MyHook2;
  

  Status = BS->LocateHandleBuffer (
                 ByProtocol,
                 Protocol,
                 NULL,
                 &HandleCount,
                 &Handles
                 );
  if(EFI_ERROR(Status) || HandleCount == 0){
    goto ProcExit;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = BS->HandleProtocol (
                    Handles[Index],
                    Protocol,
                    (VOID**)&MyHook2
                    );
    ASSERT(!EFI_ERROR(Status));
    MyHook2(Param);
  }

ProcExit:
  if(Handles != NULL){
    BS->FreePool(Handles);
  }
  return Status;
}




VOID *GetAcpiTableBySign(UINT32 Sign, EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_ACPI_4_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp = NULL;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  UINTN                                         Index;
  UINT64                                        *XTableAddress;
  UINTN                                         TableCount;
  EFI_ACPI_DESCRIPTION_HEADER                   *Hdr;
  

  for (Index = 0; Index < SystemTable->NumberOfTableEntries; Index++) {
    if (CompareGuid (&gEfiAcpiTableGuid, &(SystemTable->ConfigurationTable[Index].VendorGuid))) {
      Rsdp = (EFI_ACPI_4_0_ROOT_SYSTEM_DESCRIPTION_POINTER*)SystemTable->ConfigurationTable[Index].VendorTable;
      break;
    }
  }
  if(Rsdp == NULL){
    return NULL;
  }

  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)Rsdp->XsdtAddress;
  TableCount = (Xsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT64);
  XTableAddress = (UINT64 *)(Xsdt + 1);
  for (Index = 0; Index < TableCount; Index++) {
    Hdr = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)XTableAddress[Index];
    if (Hdr->Signature == Sign){
      return (VOID*)Hdr;
    }
  }

  return NULL;
}




UINT8 Cmos70Read(UINT8 Address)
{
  IoWrite8(0x70, Address);
  return IoRead8(0x71);
}

VOID Cmos70Write(UINT8 Address, UINT8 Data)
{
  IoWrite8(0x70, Address);
  IoWrite8(0x71, Data);
}


VOID WriteCheckedCmosByte(UINT8 Offset, UINT8 Data8)
{
  UINT8  CmosData;

  ASSERT((Data8 & 0xF0) == 0);
  
  CmosData = Data8 & 0xF;
  CmosData = (((~CmosData) & 0xF) << 4)|CmosData;
  Cmos70Write(Offset, CmosData);
}

BOOLEAN ReadCheckedCmosByte(UINT8 Offset, UINT8 *Data8)
{
  UINT8  CmosData;
  UINT8  Data1, Data2;

  ASSERT(Data8 != NULL);

  CmosData = Cmos70Read(Offset);
  Data1 = (CmosData & 0xF);
  Data2 = (CmosData >> 4) & 0xF;

  if(Data1 & Data2){
    return FALSE;
  } else {
    *Data8 = Data1;
    return TRUE;
  }
}


UINT8 GetBbsGroupType(UINT8 PciClass, UINT8 BbsType)
{
  BM_MENU_TYPE   GroupType = BM_MENU_TYPE_MAX;
 
  switch (BbsType) {
  	case BBS_TYPE_FLOPPY:  //patch for some usb disk, it will initialed as floppy type.
    case BBS_TYPE_HARDDRIVE:
      if(PciClass == PCI_CLASS_SERIAL){
        GroupType = BM_MENU_TYPE_LEGACY_USK_DISK;
      } else {
        GroupType = BM_MENU_TYPE_LEGACY_HDD;
      }
      break;
      
    case BBS_TYPE_CDROM:
      if(PciClass == PCI_CLASS_SERIAL){
        GroupType = BM_MENU_TYPE_LEGACY_USB_ODD;
      } else {
        GroupType = BM_MENU_TYPE_LEGACY_ODD;         
      }
      break;
      
    case BBS_TYPE_EMBEDDED_NETWORK:
      GroupType = BM_MENU_TYPE_LEGACY_PXE;
      break;
  }

  return (UINT8)GroupType;
}



BOOLEAN 
IsFilePresentSize (
  EFI_BOOT_SERVICES  *BS,
  VOID               *pRootFile,
  CHAR16             *FilePathName,
  UINTN              *FileSize          OPTIONAL
  )
{
  EFI_STATUS          Status;
  EFI_FILE_PROTOCOL   *File     = NULL;  
  EFI_FILE_INFO       *FileInfo = NULL;
  UINTN               FileInfoSize;
  BOOLEAN             Rc = FALSE;
  EFI_FILE_PROTOCOL   *RootFile;


  RootFile = (EFI_FILE_PROTOCOL*)pRootFile;
  
  Status = RootFile->Open(
                       RootFile, 
                       &File, 
                       FilePathName, 
                       EFI_FILE_MODE_READ, 
                       0
                       );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "Open %s:%r\n", FilePathName, Status));   
    goto ProcExit;
  }    
 
  FileInfo = NULL;
  FileInfoSize = 0;
  Status = File->GetInfo (
                   File,
                   &gEfiFileInfoGuid,
                   &FileInfoSize,
                   FileInfo
                   );
  if(Status == EFI_BUFFER_TOO_SMALL){
    Status = BS->AllocatePool(EfiBootServicesData, FileInfoSize, &FileInfo);
    if(EFI_ERROR(Status)){
      Status = EFI_OUT_OF_RESOURCES;
    } else {
      Status = File->GetInfo (
                       File,
                       &gEfiFileInfoGuid,
                       &FileInfoSize,
                       FileInfo
                       );
      if(EFI_ERROR(Status)){
        DEBUG((EFI_D_ERROR, "GetInfo:%r\n", Status)); 
      }
    }
  }
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  

  if(FileInfo->Attribute & EFI_FILE_DIRECTORY){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  if(FileSize != NULL){
    *FileSize = (UINTN)FileInfo->FileSize;
  }
  
  Rc = TRUE;


ProcExit:
  if(FileInfo != NULL){
    BS->FreePool(FileInfo);
  } 
  if(File != NULL){
    File->Close(File);
  }   
  return Rc;
}


BOOLEAN 
IsFilePresent (
  EFI_BOOT_SERVICES  *BS,
  VOID               *pRootFile,
  CHAR16             *FilePathName
  )
{
  return IsFilePresentSize(BS, pRootFile, FilePathName, NULL);
}




BOOLEAN 
IsFsFolderPresent (
  EFI_BOOT_SERVICES  *BS,
  VOID               *pRootFile,
  CHAR16             *FolderName
  )
{
  EFI_STATUS          Status;
  EFI_FILE_PROTOCOL   *File     = NULL;  
  EFI_FILE_INFO       *FileInfo = NULL;
  UINTN               FileInfoSize;
  BOOLEAN             Rc = FALSE;
  EFI_FILE_PROTOCOL   *RootFile;


  RootFile = (EFI_FILE_PROTOCOL*)pRootFile;
  
  Status = RootFile->Open(
                       RootFile, 
                       &File, 
                       FolderName, 
                       EFI_FILE_MODE_READ, 
                       0
                       );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "Open %s:%r\n", FolderName, Status));   
    goto ProcExit;
  }    
 
  FileInfo = NULL;
  FileInfoSize = 0;
  Status = File->GetInfo (
                   File,
                   &gEfiFileInfoGuid,
                   &FileInfoSize,
                   FileInfo
                   );
  if(Status == EFI_BUFFER_TOO_SMALL){
    Status = BS->AllocatePool(EfiBootServicesData, FileInfoSize, &FileInfo);
    if(EFI_ERROR(Status)){
      Status = EFI_OUT_OF_RESOURCES;
    } else {
      Status = File->GetInfo (
                       File,
                       &gEfiFileInfoGuid,
                       &FileInfoSize,
                       FileInfo
                       );
      if(EFI_ERROR(Status)){
        DEBUG((EFI_D_ERROR, "GetInfo:%r\n", Status)); 
      }
    }
  }
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  

  if(!(FileInfo->Attribute & EFI_FILE_DIRECTORY)){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  Rc = TRUE;

ProcExit:
  if(FileInfo != NULL){
    BS->FreePool(FileInfo);
  } 
  if(File != NULL){
    File->Close(File);
  }   
  return Rc;
}




EFI_STATUS 
GetFsLabel (
  EFI_BOOT_SERVICES  *BS,
  VOID               *pRootFile,
  CHAR16             **pLabel
  )
{
  EFI_FILE_PROTOCOL       *RootFile;
  EFI_FILE_SYSTEM_INFO		*FsInfo = NULL;
  UINTN                   FsInfoSize = 0;
  EFI_STATUS              Status;
  UINTN                   LabelSize;


  RootFile = (EFI_FILE_PROTOCOL*)pRootFile;

  Status = RootFile->GetInfo(RootFile, &gEfiFileSystemInfoGuid, &FsInfoSize, FsInfo);
  if(Status != EFI_BUFFER_TOO_SMALL){
    return EFI_ABORTED;
  }

  Status = BS->AllocatePool(EfiBootServicesData, FsInfoSize, &FsInfo);
  if(EFI_ERROR(Status)){
    return EFI_OUT_OF_RESOURCES;
  }  
  
  Status = RootFile->GetInfo(RootFile, &gEfiFileSystemInfoGuid, &FsInfoSize, FsInfo);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  LabelSize = StrSize(FsInfo->VolumeLabel);
  Status = BS->AllocatePool(EfiBootServicesData, LabelSize, pLabel);
  if(EFI_ERROR(Status)){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }    
  StrCpy(*pLabel, FsInfo->VolumeLabel);

ProcExit:
  if(FsInfo != NULL){BS->FreePool(FsInfo);}
  return Status;
}






EFI_STATUS 
RootFileReadFile(   
  IN   EFI_BOOT_SERVICES     *BS,
  IN   EFI_FILE_PROTOCOL     *pRootFile,
  IN   CHAR16                *FileName,
  OUT  VOID                  **FileData,
  OUT  UINTN                 *FileSize
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_FILE_PROTOCOL               *ptFile = NULL;
  EFI_FILE_INFO                   *FileInfo = NULL;
  UINTN                           FileInfoSize;
  UINT8                           *Buffer = NULL;
  UINTN                           BufferSize = 0;
  EFI_FILE_PROTOCOL               *RootFile;


  if(pRootFile == NULL || FileName == NULL || FileData == NULL || FileSize == NULL){
    return EFI_INVALID_PARAMETER;
  }

  RootFile = (EFI_FILE_PROTOCOL*)pRootFile;

  Status = RootFile->Open (
             RootFile, 
             &ptFile, 
             FileName, 
             EFI_FILE_MODE_READ, 
             0
             );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "(L%d) open %s:%r\n", __LINE__, FileName, Status));
    goto ProcExit;
  }
  
  FileInfo = NULL;
  FileInfoSize = 0;
  Status = ptFile->GetInfo (
                     ptFile,
                     &gEfiFileInfoGuid,
                     &FileInfoSize,
                     FileInfo
                     );
  if(Status != EFI_BUFFER_TOO_SMALL){
    Status = EFI_ABORTED;
    goto ProcExit;    
  }
  Status = BS->AllocatePool(EfiBootServicesData, FileInfoSize, &FileInfo);
  if(!EFI_ERROR(Status)){
    Status = ptFile->GetInfo(
                       ptFile,
                       &gEfiFileInfoGuid,
                       &FileInfoSize,
                       FileInfo
                       );
  }  
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
	
	if(FileInfo->Attribute & EFI_FILE_DIRECTORY){
	  Status = EFI_INVALID_PARAMETER;
	  goto ProcExit;
	}

  BufferSize = (UINTN)FileInfo->FileSize;	
  if(BufferSize != 0){
    Status = BS->AllocatePool(EfiBootServicesData, BufferSize, &Buffer);
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }  
    Status = ptFile->Read(ptFile, &BufferSize, Buffer);
    if(EFI_ERROR(Status)){
      goto ProcExit;
    } 
  }
  
  *FileData = (VOID*)Buffer;
  *FileSize = BufferSize;
  Status = EFI_SUCCESS;

ProcExit:
  if(ptFile!=NULL){ptFile->Close(ptFile);}  
  if(FileInfo!=NULL){BS->FreePool(FileInfo);}
  if(EFI_ERROR(Status) && Buffer!=NULL){BS->FreePool(Buffer);}
  return Status;    
}





EFI_STATUS 
RootFileWriteFile(
  IN   VOID                  *pRootFile,
  IN   CHAR16                *FileName,
  IN   VOID                  *FileData,
  IN   UINTN                 FileSize
  )
{
  EFI_STATUS                      Status;
  EFI_FILE_PROTOCOL               *File = NULL;
  UINTN                           Index;
  UINTN                           Count;
  UINTN                           SendSize;
  EFI_FILE_PROTOCOL               *RootFile;  


  if(pRootFile == NULL || FileName == NULL || FileData == NULL){
    return EFI_INVALID_PARAMETER;
  }

  RootFile = (EFI_FILE_PROTOCOL*)pRootFile;

  Status = RootFile->Open (
                       RootFile, 
                       &File, 
                       FileName, 
                       EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 
                       EFI_FILE_ARCHIVE
                       );
  if(!EFI_ERROR(Status)){
    Status = File->Delete(File);
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_ERROR, "File Cannot Be Deleted:%r\n", Status));
      goto ProcExit;
    }
  }

  Status = RootFile->Open (
                       RootFile, 
                       &File, 
                       FileName, 
                       EFI_FILE_MODE_CREATE | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_READ, 
                       EFI_FILE_ARCHIVE
                       );
	if(EFI_ERROR(Status)){
    goto ProcExit;
  }  

  Count = FileSize/SIZE_4KB;
  for(Index=0;Index<Count;Index++){
    SendSize = SIZE_4KB;
    Status   = File->Write(
                       File,
                       &SendSize,
                       FileData
                       );
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }
    FileData = (VOID*)((UINTN)FileData + SIZE_4KB);
    DEBUG((EFI_D_INFO, "."));
  }
  Count = FileSize%SIZE_4KB;
  if(Count){
    SendSize = Count;
    Status   = File->Write(
                       File,
                       &SendSize,
                       FileData
                       );
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }
    DEBUG((EFI_D_INFO, "."));                        
  }

ProcExit:
  if(File!=NULL){File->Close(File);}  
  return Status;   
}



EFI_STATUS 
RootFileRmFile (
  IN   VOID                  *pRootFile,
  IN   CHAR16                *FileName
  )
{
  EFI_STATUS                      Status;
  EFI_FILE_PROTOCOL               *File = NULL;
  EFI_FILE_PROTOCOL               *RootFile;  


  if(pRootFile == NULL || FileName == NULL){
    return EFI_INVALID_PARAMETER;
  }

  RootFile = (EFI_FILE_PROTOCOL*)pRootFile;

  Status = RootFile->Open (
                       RootFile, 
                       &File, 
                       FileName, 
                       EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 
                       EFI_FILE_ARCHIVE
                       );
  if(!EFI_ERROR(Status)){
    Status = File->Delete(File);
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_ERROR, "File %s Cannot Be Deleted:%r\n", FileName, Status));
      goto ProcExit;
    }
    File = NULL;
  }

  Status = EFI_SUCCESS;

ProcExit:
  if(File!=NULL){File->Close(File);}  
  return Status;   
}


 
EFI_STATUS 
LibGetFileInfo (
IN  EFI_BOOT_SERVICES     *BS,
IN  VOID                  *ptFile,
OUT VOID                  **Info
)
{
  UINTN             BufferSize;
  EFI_FILE_INFO     *FileInfo;
  EFI_STATUS        Status;
  EFI_FILE_PROTOCOL	*File;


  if(ptFile == NULL || Info == NULL){
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;    
  }

  File = (EFI_FILE_PROTOCOL*)ptFile;
  BufferSize = 0;
  FileInfo   = NULL;	
  Status = File->GetInfo (
                  File,
                  &gEfiFileInfoGuid,
                  &BufferSize,
                  FileInfo
                  );
  if(Status == EFI_BUFFER_TOO_SMALL){
    Status = BS->AllocatePool(EfiBootServicesData, BufferSize, &FileInfo);
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }
  } else {
    Status = EFI_ABORTED;
    goto ProcExit;
  }
  Status = File->GetInfo (
                  File,
                  &gEfiFileInfoGuid,
                  &BufferSize,
                  FileInfo
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;    
  }

  *Info = (VOID*)FileInfo;

ProcExit:
  return Status;
}






STATIC UINT16 SmbiosGetStrOffset(CONST SMBIOS_STRUCTURE *Hdr, UINT8 StrId)
{
  CONST UINT8 *pData8;
  UINT8       i;

  if(StrId == 0){
    return 0;
  }

  pData8  = (UINT8*)Hdr;
  pData8 += Hdr->Length;

  i = 1;
  while(i != StrId){
    while(*pData8!=0){pData8++;}

    if(pData8[1] == 0){     // if next byte of a string end is NULL, type end.
      break;
    }

    pData8++;
    i++;
  }
  if(i == StrId){
    return (UINT16)(pData8 - (UINT8*)Hdr);
  } else {
    return 0;
  }
}


STATIC CHAR8 gDummyStr[] = {" "};

CHAR8 *
LibSmbiosGetStringInTypeByIndex (
  VOID       *SmbiosHdr,
  UINT8      StrId
  )
{
  SMBIOS_STRUCTURE_POINTER  p;
  UINT16                    Offset;
  
  if(StrId == 0){
    return gDummyStr;
  }

  p.Hdr = (SMBIOS_STRUCTURE*)SmbiosHdr;
  Offset = SmbiosGetStrOffset(p.Hdr, StrId);
  if(Offset == 0){
    return gDummyStr;
  } else {
    return (CHAR8*)(p.Raw + Offset);
  }
}


CHAR8 *
LibSmbiosGetTypeEnd (
  VOID       *SmbiosHdr
  )
{
  SMBIOS_STRUCTURE   *Hdr;
  UINT8              *pData8;

  
  Hdr = (SMBIOS_STRUCTURE*)SmbiosHdr;
  pData8  = (UINT8*)Hdr;
  pData8 += Hdr->Length;

  while(1){
    while(*pData8!=0){pData8++;}
    if(pData8[1] == 0){
      break;
    }
    pData8++;
  }
  
  return pData8+1;
}



EFI_STATUS
LibReadFileFromFv(
  IN  EFI_BOOT_SERVICES *BS,
  IN  CONST EFI_GUID    *NameGuid,
  IN  UINT8             SectionType,
  IN  UINTN             SectionInstance,
  OUT VOID              **FileData,
  OUT UINTN             *FileDataSize
  )
{
  UINTN                         FvHandleCount = 0;
  EFI_HANDLE                    *FvHandleBuffer = NULL;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv2 = NULL;
	UINT32                        AuthenticationStatus;
  EFI_FV_FILETYPE               FoundType;
  UINT32                        FvStatus;
  UINT32                        Attributes;
  VOID                          *MyFileData;
  UINTN                         MyFileDataSize;
  EFI_STATUS                    Status;
  UINTN                         Index;


  Status = BS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &FvHandleCount,
                  &FvHandleBuffer
                  );
  if(EFI_ERROR(Status) || FvHandleCount==0){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  for (Index = 0; Index < FvHandleCount; Index++) {
    Status = BS->HandleProtocol (
                    FvHandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID**)&Fv2
                    );
    ASSERT(!EFI_ERROR(Status));
    Status = Fv2->ReadFile (
                  Fv2,
                  NameGuid,
                  NULL,
                  &MyFileDataSize,
                  &FoundType,
                  &Attributes,
                  &AuthenticationStatus
                  );
    if (!EFI_ERROR(Status)) {
      break;
    }
  }
  if(Index >= FvHandleCount){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  MyFileData     = NULL;  //!!! NULL means memory allocated by callee.
  MyFileDataSize = 0;
  Status = Fv2->ReadSection (
                    Fv2,
                    NameGuid,
                    SectionType,
                    SectionInstance,
                    &MyFileData,
                    &MyFileDataSize,
                    &FvStatus
                    );
  if (!EFI_ERROR(Status)) {
    *FileData     = MyFileData;
    *FileDataSize = MyFileDataSize;
  }

ProcExit:
  if(FvHandleBuffer != NULL){
    BS->FreePool(FvHandleBuffer);
  }
  return Status;
}



BOOLEAN
IsFilePresentInFv (
  IN  EFI_BOOT_SERVICES *BS,
  IN  CONST EFI_GUID    *FileGuid
  )
{
  UINTN                         FvHandleCount = 0;
  EFI_HANDLE                    *FvHandleBuffer = NULL;
  EFI_FIRMWARE_VOLUME2_PROTOCOL *Fv2 = NULL;
	UINT32                        AuthenticationStatus;
  EFI_FV_FILETYPE               FoundType;
  UINT32                        Attributes;
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINTN                         FileDataSize;


  Status = BS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &FvHandleCount,
                  &FvHandleBuffer
                  );
  if(EFI_ERROR(Status) || FvHandleCount==0){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  for (Index = 0; Index < FvHandleCount; Index++) {
    Status = BS->HandleProtocol (
                    FvHandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID**)&Fv2
                    );
    ASSERT(!EFI_ERROR(Status));
    Status = Fv2->ReadFile (
                    Fv2,
                    FileGuid,
                    NULL,
                    &FileDataSize,
                    &FoundType,
                    &Attributes,
                    &AuthenticationStatus
                    );
    if (!EFI_ERROR(Status)) {
      break;
    }
  }
  if(Index >= FvHandleCount){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

ProcExit:
  if(FvHandleBuffer != NULL){
    BS->FreePool(FvHandleBuffer);
  }
  if(!EFI_ERROR(Status)){
    return TRUE;
  } else {
    return FALSE;
  }
}


BOOLEAN IsAllDataFF(VOID *DataIn, UINTN DataSize)
{
  UINTN  Index;
  UINT8  *Data8;


  if(DataSize == 0){
    return FALSE;
  }

  Data8 = (UINT8*)DataIn;

  for(Index=0;Index<DataSize;Index++){
    if(Data8[Index] != 0xFF){
      break;
    }
  }

  if(Index >= DataSize){
    return TRUE;
  } else {
    return FALSE;
  }
}


BOOLEAN LibIsHexChar(CHAR8 c)
{
  if(c >= 'a' && c <= 'f'){
    return TRUE;
  } else if(c >= 'A' && c <= 'F'){
    return TRUE;
  } else if(c >= '0' && c <= '9'){
    return TRUE;
  } else {
    return FALSE;
  }
}

BOOLEAN LibChar2Hex(CHAR8 *s, UINT8 *d)
{
  UINT8 d1, d2;

  d1 = s[0];
  
  if(d1 >= 'a' && d1 <= 'f'){
    d1 = (d1 - 'a') + 0xA;
  } else if(d1 >= 'A' && d1 <= 'F'){
    d1 = (d1 - 'A') + 0xA;
  } else if(d1 >= '0' && d1 <= '9'){
    d1 = d1 - '0';
  } else {
    return FALSE;
  }

  d2 = s[1];
  
  if(d2 >= 'a' && d2 <= 'f'){
    d2 = (d2 - 'a') + 0xA;
  } else if(d2 >= 'A' && d2 <= 'F'){
    d2 = (d2 - 'A') + 0xA;
  } else if(d2 >= '0' && d2 <= '9'){
    d2 = d2 - '0';
  } else {
    return FALSE;
  }

  *d = (d1 << 4) | d2;
  return TRUE;
}


BOOLEAN LibChar2Hex4(CHAR8 *s, UINT32 *d)
{
  UINT32  Data32;
  UINT8   *p;

  p = (UINT8*)&Data32;

  if(!LibChar2Hex(s, p+3) || !LibChar2Hex(s+2, p+2) || !LibChar2Hex(s+4, p+1) || !LibChar2Hex(s+6, p)){
    return FALSE;
  }

  *d = Data32;
  return TRUE;
}


BOOLEAN LibChar2Hex2(CHAR8 *s, UINT16 *d)
{
  UINT16  Data16;
  UINT8   *p;

  p = (UINT8*)&Data16;

  if(!LibChar2Hex(s, p+1) || !LibChar2Hex(s+2, p)){
    return FALSE;
  }

  *d = Data16;
  return TRUE;
}



BOOLEAN LibChar2Dec(CHAR8 *s, UINT8 *d)
{
  UINT8 d1, d2;

  d1 = s[0];
  if(d1 >= '0' && d1 <= '9'){
    d1 = d1 - '0';
  } else {
    return FALSE;
  }

  d2 = s[1];
  if(d2 >= '0' && d2 <= '9'){
    d2 = d2 - '0';
  } else {
    return FALSE;
  }

  *d = (d1 * 10) + d2;
  return TRUE;
}


BOOLEAN LibChar2Dec2(CHAR8 *s, UINT16 *d)
{
  UINT8  Data8[2];


  if(!LibChar2Dec(s, Data8) || !LibChar2Dec(s+2, Data8+1)){
    return FALSE;
  }

  *d = Data8[0] * 100 + Data8[1];
  return TRUE;
}


BOOLEAN LibChar2Dec4(CHAR8 *s, UINT32 *d)
{
  UINT16  Data16[2];


  if(!LibChar2Dec2(s, Data16) || !LibChar2Dec2(s+4, Data16+1)){
    return FALSE;
  }

  *d = Data16[0] * 10000 + Data16[1];
  return TRUE;
}



static void SwapDataInUuid(EFI_GUID *Uuid)
{
  UINT8  *p;

  p = Uuid->Data4;

  *(UINT16*)&p[0] = SwapBytes16(*(UINT16*)&p[0]);
  *(UINT16*)&p[2] = SwapBytes16(*(UINT16*)&p[2]);
  *(UINT32*)&p[4] = SwapBytes32(*(UINT32*)&p[4]);   
}


BOOLEAN LibChar2Guid(CHAR8 *s, EFI_GUID *Guid)
{
  if(AsciiStrLen(s) >= 36 && s[8] == '-' && s[13] == '-' && s[18] == '-' && s[23] == '-' &&
    LibChar2Hex4(s, &Guid->Data1) && LibChar2Hex2(s+9, &Guid->Data2) && 
    LibChar2Hex2(s+14, &Guid->Data3) && LibChar2Hex2(s+19, (UINT16*)Guid->Data4) && 
    LibChar2Hex2(s+24, (UINT16*)(Guid->Data4+2)) && LibChar2Hex4(s+28, (UINT32*)(Guid->Data4+4))){
    SwapDataInUuid(Guid);
    return TRUE;
  }

  return FALSE;
}



VOID SetBootLogoInvalid(EFI_BOOT_SERVICES *BS)
{
  EFI_STATUS               Status;
  EFI_BOOT_LOGO_PROTOCOL   *BootLogo;
  
  Status = BS->LocateProtocol(&gEfiBootLogoProtocolGuid, NULL, (VOID**)&BootLogo);
  if (!EFI_ERROR(Status)) {
    Status = BootLogo->SetBootLogo (BootLogo, NULL, 0, 0, 0, 0);
  }
}


UINT8 GetPciSlotNum(
  IN  EFI_BOOT_SERVICES *BS,
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                DriverHandle;
  EFI_PCI_IO_PROTOCOL       *PciIo = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *BridgeDp;
  VOID                      *DpTemp;
  UINT8                     ClassCode[3];
  UINT8                     SlotNum = 0xFF;
  UINT8                     CapabilityPtr = 0;
  UINT8                     CapabilityID = 0;
  UINT16                    CapabilityEntry = 0;
  UINT32                    SlotCap = 0;
  BOOLEAN                   BridgeDevice = FALSE;
  UINT8                     SmilelineType;

  BridgeDp = DevicePath;
  while(!BridgeDevice && !(BridgeDp->Type == END_DEVICE_PATH_TYPE &&
        BridgeDp->SubType == END_ENTIRE_DEVICE_PATH_SUBTYPE)) {
    DpTemp = (VOID*)(UINTN)BridgeDp;
    //ShowDevicePathDxe(BS, BridgeDp);
    Status = BS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &(EFI_DEVICE_PATH_PROTOCOL*)DpTemp,
                  &DriverHandle
                  );
    if (!EFI_ERROR (Status)) {
      Status = BS->HandleProtocol(
                    DriverHandle,
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );
      if (!EFI_ERROR (Status)) {
        PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, ClassCode);
        if(ClassCode[2] == 6) {
          BridgeDevice = TRUE;
        }
      }
    }
    BridgeDp = GetPciParentDp(BS, BridgeDp);
  }
  
  if(BridgeDevice == FALSE){
    DEBUG((EFI_D_ERROR,"Something wrong with this device path!!\n"));
    return 0xFF;
  }
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);

  while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapabilityPtr, 1, &CapabilityEntry);
    CapabilityID = (UINT8) CapabilityEntry;

    if ((UINT8)EFI_PCI_CAPABILITY_ID_PCIEXP == CapabilityID) {
      PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, CapabilityPtr + 0x14, 1, &SlotCap);
      SlotNum = (UINT8)((SlotCap & 0xfff80000) >> 19);
      break;
    }
      CapabilityPtr = (UINT8)(CapabilityEntry >> 8);
  }

  SmilelineType = PcdGet8(PcdSmileline);

  if(SmilelineType==3){
	if(8<=SlotNum && SlotNum<=10){
	  SlotNum = 11-SlotNum;
	}else{
      SlotNum = 0;
	}
  }else if(SmilelineType==0){
    if(10<=SlotNum && SlotNum<=12){
	  SlotNum = 13-SlotNum;
	}else{
      SlotNum = 0;
	}
  }else if(SmilelineType==2){
	if(9<=SlotNum && SlotNum<=11){
	  SlotNum = 12-SlotNum;
	}else if(SlotNum==6){
      SlotNum = 0;
	}else{
      SlotNum = 4;
	}
  }

  return SlotNum;
}


CHAR16
LibUnicodeToUpper (
  IN      CHAR16                    Chr
  )
{
  return (Chr >= L'a' && Chr <= L'z') ? Chr - (L'a' - L'A') : Chr;
}

INTN
LibStrinCmp (
  IN CHAR16   *String,
  IN CHAR16   *String2,
  IN UINTN    Length
  )
{
  if (Length == 0) {
    return 0;
  }

  while ((*String != L'\0') && (*String2 != L'\0') &&
         (LibUnicodeToUpper (*String) == LibUnicodeToUpper (*String2)) && (Length > 1)) {
    String++;
    String2++;
    Length--;
  }

  return LibUnicodeToUpper (*String) - LibUnicodeToUpper (*String2);
}


INTN
LibStriCmp (
  IN CHAR16   *String,
  IN CHAR16   *String2
  )
{
  while ((*String != L'\0') &&
         (LibUnicodeToUpper (*String) == LibUnicodeToUpper (*String2))) {
    String++;
    String2++;
  }

  return LibUnicodeToUpper (*String) - LibUnicodeToUpper (*String2);
}


VOID *GetByoSysAndStsHobData(VOID)
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *Data;

  GuidHob.Raw = GetFirstGuidHob(&gByoSysCfgAndStsHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  Data = (VOID *)(GuidHob.Guid + 1);

  return Data;
}




CHAR16 * 
LibOpalDriverGetDriverDeviceName (
    EFI_BOOT_SERVICES   *BS,
    EFI_HANDLE          DiskHandle
  )
{
  EFI_HANDLE                    *NvmeBuffer = NULL;
  UINTN                         Nvmes;
  EFI_STATUS                    Status;
  CHAR16                        *DevName = NULL;
  CHAR16                        *Str = NULL;
  EFI_HANDLE*                   Cn2Buffer = NULL;
  UINTN                         Cn2s;
  EFI_COMPONENT_NAME2_PROTOCOL* Cn2;
  UINTN                         Index1;
  UINTN                         Index2;


  Status = BS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiNvmExpressPassThruProtocolGuid,
                  NULL,
                  &Nvmes,
                  &NvmeBuffer
                  );
  if (EFI_ERROR(Status) || Nvmes == 0) {
    goto ProcExit;
  }

  Status = BS->LocateHandleBuffer(
                  ByProtocol,
                  &gEfiComponentName2ProtocolGuid,
                  NULL,
                  &Cn2s,
                  &Cn2Buffer
                  );
  if (EFI_ERROR(Status) || Cn2s == 0) {
    goto ProcExit;
  }

  for (Index1 = 0; Index1 < Cn2s; Index1++) {

    Status = BS->HandleProtocol(
                 Cn2Buffer[Index1],
                 &gEfiComponentName2ProtocolGuid,
                 (VOID**)&Cn2
                 );
    
    for (Index2 = 0; Index2 < Nvmes; Index2++) {
      Status = Cn2->GetControllerName(
                      Cn2,
                      NvmeBuffer[Index2],
                      DiskHandle,
                      Cn2->SupportedLanguages,
                      &DevName
                      );
      if (!EFI_ERROR(Status) && DevName != NULL) {
        Status = BS->AllocatePool(EfiBootServicesData, StrSize(DevName), (VOID**)&Str);
        ASSERT(!EFI_ERROR(Status));
        StrCpy(Str, DevName);
        TrimStr16(Str);        
        goto ProcExit;
      }
    }
  }

ProcExit:
  if(NvmeBuffer != NULL){BS->FreePool(NvmeBuffer);}
  if(Cn2Buffer != NULL){BS->FreePool(Cn2Buffer);}  
  return Str;
}





BOOLEAN
LibContainEfiImage (
  IN VOID            *RomImage,
  IN UINT64          RomSize
  )
{
  PCI_EXPANSION_ROM_HEADER  *RomHeader;
  PCI_DATA_STRUCTURE        *RomPcir;
  BOOLEAN                   FirstCheck;

  FirstCheck = TRUE;
  RomHeader  = RomImage;

  while ((UINT8 *) RomHeader < (UINT8 *) RomImage + RomSize) {
    if (RomHeader->Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE) {
      if (FirstCheck) {
        return FALSE;
      } else {
        RomHeader = (PCI_EXPANSION_ROM_HEADER *) ((UINT8 *) RomHeader + 512);
        continue;
      }
    }

    FirstCheck = FALSE;
    RomPcir    = (PCI_DATA_STRUCTURE *) ((UINT8 *) RomHeader + RomHeader->PcirOffset);

    if (RomPcir->CodeType == PCI_CODE_TYPE_EFI_IMAGE) {
      return TRUE;
    }

    RomHeader = (PCI_EXPANSION_ROM_HEADER *) ((UINT8 *) RomHeader + RomPcir->Length * 512);
  }

  return FALSE;
}



BOOLEAN
LibContainLegacyImage (
  IN VOID            *RomImage,
  IN UINT64          RomSize
  )
{
  PCI_EXPANSION_ROM_HEADER  *RomHeader;
  PCI_DATA_STRUCTURE        *RomPcir;
  UINT8                     Indicator;

  Indicator = 0;
  RomHeader = RomImage;
  if (RomHeader == NULL) {
    return FALSE;
  }

  do {
    if (RomHeader->Signature != PCI_EXPANSION_ROM_HEADER_SIGNATURE) {
      RomHeader = (PCI_EXPANSION_ROM_HEADER *) ((UINT8 *) RomHeader + 512);
      continue;
    }

    //
    // The PCI Data Structure must be DWORD aligned. 
    //
    if (RomHeader->PcirOffset == 0 ||
        (RomHeader->PcirOffset & 3) != 0 ||
        (UINT8 *) RomHeader + RomHeader->PcirOffset + sizeof (PCI_DATA_STRUCTURE) > (UINT8 *) RomImage + RomSize) {
      break;
    }

    RomPcir = (PCI_DATA_STRUCTURE *) ((UINT8 *) RomHeader + RomHeader->PcirOffset);
    if (RomPcir->Signature != PCI_DATA_STRUCTURE_SIGNATURE) {
      break;
    }

    if (RomPcir->CodeType == PCI_CODE_TYPE_PCAT_IMAGE) {
      return TRUE;
    }

    Indicator = RomPcir->Indicator;
    RomHeader = (PCI_EXPANSION_ROM_HEADER *) ((UINT8 *) RomHeader + RomPcir->ImageLength * 512);
  } while (((UINT8 *) RomHeader < (UINT8 *) RomImage + RomSize) && ((Indicator & 0x80) == 0x00));

  return FALSE;
}





BOOLEAN
LibGetSataPortInfo (
  IN  EFI_BOOT_SERVICES         *BS,
  IN  EFI_HANDLE                Handle,
  OUT UINTN                     *HostIndex,
  OUT UINTN                     *PortIndex,
  OUT UINTN                     *PhysicPortIndex OPTIONAL
  )
{
  EFI_STATUS                    Status;
  UINT16                        AtaIndex;
  UINT16                        PlatSataHostIndex = 0;
  PLAT_HOST_INFO_PROTOCOL       *PlatHostInfo;
  BOOLEAN                       rc = FALSE;


  Status = BS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "PlatHostInfo not found\n"));
    goto ProcExit;
  }

  AtaIndex = PlatHostInfo->GetSataPortIndex(Handle, PhysicPortIndex);
  if(AtaIndex == 0xFFFF){
    DEBUG((EFI_D_INFO, "GetSataPortIndex Error\n"));
    goto ProcExit;
  }  

  PlatSataHostIndex = PlatHostInfo->GetSataHostIndex(Handle);   

  if(PlatSataHostIndex == 0xFFFF || PlatSataHostIndex == 0x8000){
    *HostIndex = PlatSataHostIndex;
  } else {
    *HostIndex = PlatSataHostIndex;
  }
  *PortIndex = AtaIndex;
  rc = TRUE;

ProcExit:
  return rc;
}



