

#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <AmdCpmDefine.h>
#include <Ppi/AmdCpmTablePpi/AmdCpmTablePpi.h>
#include <SetupVariable.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>
#include <PlatformDefinition.h>
#include <Library/FabricResourceManagerLib.h>
#include <Ppi/ByoEarlyGopInfoPpi.h>
#include <Library/MemoryAllocationLib.h>
#include <Token.h>



// DP:PciRoot(0x0)/Pci(0x1,0x5)/Pci(0x0,0x0)/Pci(0x0,0x0)
/*
Type:PCI   Express   Bus 00   Device 01   Function 05
Offset:0000 Width:01
94 1D 53 14 [07] 00 10 00 00 00 04 06 00 00 81 00
00 00 00 00 00 00 00 00 [00 04 05] 00 [11 11] 00 20       // 1000 - 1FFF
[00 EC 00 ED] F1 FF 01 00 FF FF FF FF 00 00 00 00         // EC000000 - ED0FFFFF
00 00 00 00 50 00 00 00 00 00 00 00 FF 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
01 58 03 C8 00 00 00 00 10 A0 42 01 22 80 00 00 
17 29 00 00 13 38 73 04 00 00 12 B0 00 00 5C 00 
00 00 40 01 00 00 01 00 00 00 00 00 BF 01 70 00 
06 00 00 00 0E 00 00 00 43 00 01 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
05 C0 80 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0D C8 00 00 94 1D 53 14 08 00 03 A8 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00

Type:PCI   Express   Bus 04   Device 00   Function 00
Offset:0000 Width:01
03 1A 50 11 [07] 00 10 00 04 00 04 06 00 00 01 00 
00 00 00 00 00 00 00 00 [04 05 05] 00 [11 11] 20 02     // 1000 - 1FFF
[00 EC 00 ED] F1 FF 01 00 FF FF FF FF 00 00 00 00       // EC000000 - ED0FFFFF
00 00 00 00 50 00 00 00 00 00 00 00 FF 01 00 00 
00 00 00 00 60 61 15 02 00 00 00 00 00 00 00 00 
05 78 80 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 11 78 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 01 80 C3 FF 08 00 00 00 
10 C0 72 00 21 80 00 00 10 29 19 00 12 EC 00 00 
00 00 12 10 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 1F 00 00 00 00 00 00 00 00 00 00 00 
02 00 01 00 00 00 00 00 00 00 00 00 00 00 00 00 
0D 00 00 00 03 1A 50 11 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 

Type:PCI   Bus 05   Device 00   Function 00
Offset:0000 Width:01
03 1A 00 20 [03] 00 10 02 41 00 00 03 00 00 00 00 
[00 00 00 EC] [00 00 00 ED] [01 10 00 00] 00 00 00 00        L:1000000  L:20000   L:80
00 00 00 00 00 00 00 00 00 00 00 00 03 1A 00 20 
00 00 00 00 40 00 00 00 00 00 00 00 FF 01 00 00 
01 50 C3 FF 00 00 00 00 00 00 00 00 00 00 00 00 
05 00 82 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
*/



EFI_STATUS
GetPeimData (
    IN  EFI_GUID  *FileName,
    OUT VOID      **pFileData,
    OUT UINTN     *pFileSize
  )
{
  EFI_STATUS                  Status = EFI_NOT_FOUND;
  UINTN                       Instance;
  EFI_PEI_FV_HANDLE           VolumeHandle;
  BOOLEAN                     HasFound;
  EFI_PEI_FILE_HANDLE         FileHandle;
  VOID                        *SecData = NULL;
  UINT32                      SecDataSize;


  Instance = 0;
  HasFound = FALSE;
  while (1) {
    Status = PeiServicesFfsFindNextVolume(Instance, &VolumeHandle);
    if (EFI_ERROR (Status)) {
      break;
    }

    Status = PeiServicesFfsFindFileByName(FileName, VolumeHandle, &FileHandle);
    if(!EFI_ERROR(Status)){
      Status = PeiServicesFfsFindSectionData(EFI_SECTION_RAW, FileHandle, &SecData);
      ASSERT_EFI_ERROR(Status);
      HasFound = TRUE;
      break;
    }

    Instance++;
  }

  if(!HasFound){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  SecDataSize  = *(UINT32*)(&((EFI_COMMON_SECTION_HEADER*)((UINT8*)SecData - sizeof(EFI_COMMON_SECTION_HEADER)))->Size);
  SecDataSize &= 0xFFFFFF;
  SecDataSize -= sizeof(EFI_COMMON_SECTION_HEADER);

  *pFileData = SecData;
  *pFileSize = SecDataSize;

ProcExit:
  return Status;  
}








EFI_STATUS
AllocateMmio32 (
  IN  UINT8       Die,
  IN  UINT32      Length,
  IN  UINT32      Align,
  OUT UINTN       *pMmio
  )
{
  UINT64                    MmioBase;
  FABRIC_TARGET             MmioTarget;
  FABRIC_MMIO_ATTRIBUTE     Attributes;
  UINT64                    Len;
  EFI_STATUS                Status;

  
  Len = Length;
  MmioTarget.TgtType = TARGET_DIE;
  MmioTarget.SocketNum = Die / MAX_DIES_PER_SOCKET;
  MmioTarget.DieNum = Die % MAX_DIES_PER_SOCKET;
  Attributes.ReadEnable = 1;
  Attributes.WriteEnable = 1;
  Attributes.NonPosted = 0;
  Attributes.MmioType = MMIO_BELOW_4G;
  Status = FabricAllocateMmio(&MmioBase, &Len, Align, MmioTarget, &Attributes);
  if(!EFI_ERROR(Status)){
    *pMmio = (UINTN)MmioBase;
  }
  DEBUG((EFI_D_INFO, "%a %r %X\n", __FUNCTION__, Status, MmioBase));
  return Status;
}



EFI_STATUS
AllocateIo16 (
  IN  UINT8       Die,
  IN  UINT16      Length,
  OUT UINT16      *pIo
  )
{
  UINT32                    IoBase;
  FABRIC_TARGET             MmioTarget;
  UINT32                    IoLen;
  EFI_STATUS                Status;

  
  IoLen = Length;
  MmioTarget.TgtType = TARGET_DIE;
  MmioTarget.SocketNum = Die / MAX_DIES_PER_SOCKET;
  MmioTarget.DieNum = Die % MAX_DIES_PER_SOCKET;
  Status = FabricAllocateIo(&IoBase, &IoLen, MmioTarget);
  if(!EFI_ERROR(Status)){
    *pIo = (UINT16)IoBase;
  }
  DEBUG((EFI_D_INFO, "%a %r %X\n", __FUNCTION__, Status, IoBase));
  return Status;
}



VOID
DumpRootBridgeResource (
  PLATFORM_COMM_INFO  *Info
  )
{
  UINTN                HostIndex;
  UINTN                HostBase;
 

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  for(HostIndex=0; HostIndex<Info->PciHostCount; HostIndex++){ 
    HostBase = PCI_DEV_MMBASE(0, 0x18 + HostIndex, 0);  
    DumpMem32((VOID*)(HostBase+0x200), 256);
    DumpMem32((VOID*)(HostBase+0xC0), 64);
  }
}  


typedef struct {
  BYO_EARLY_GOP_INFO      Info;
  EFI_PEI_PPI_DESCRIPTOR  Desc;
} BYO_EARLY_GOP_INFO_PPI_CTX;



VOID
EFIAPI
PeiStall (
  IN  UINTN  Microseconds
  )
{
  MicroSecondDelay(Microseconds);
}



EFI_STATUS
EFIAPI
PlatEarlyGopPciInitEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  UINTN            Bar0;
  UINTN            Bar1;
  UINTN            Base;
  UINT16           MmioStart;
  UINT16           MmioEnd;
  UINT32           MmioRange;
  UINT16              IoBase;
  UINT16              IoRange;
  BYO_EARLY_GOP_INFO_PPI_CTX  *EarlyGopInfoCtx;
  BYO_EARLY_GOP_INFO  *Info;
  UINT8               *LogoData;
  UINTN               LogoDataSize;
  
  DEBUG ((EFI_D_INFO,"%a()\n",__FUNCTION__));
// Type =  Mem32; Base = 0xEC000000;	Length = 0x1100000;	Alignment = 0xFFFFFF
  Status = AllocateMmio32(0, 0x1100000, 0xFFFFFF, &Bar0);
  if(EFI_ERROR(Status)){
    return Status;
  }  

  Status = AllocateIo16(0, 0x1000, &IoBase);
  if(EFI_ERROR(Status)){
    return Status;
  }
  
  LogoData = (UINT8*)(UINTN)PcdGet32(PcdFlashNvLogoBase);
  LogoDataSize = PcdGet32(PcdFlashNvLogoSize);
  if(LogoData == NULL || LogoDataSize == 0 || (*(UINT64*)(LogoData) == 0xFFFFFFFFFFFFFFFF)){
    Status = GetPeimData(&gByoEarlyGopLogoFileGuid, &LogoData, &LogoDataSize);
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "logo file get error %r\n", Status));
      return Status;
    }   
  }
  Bar1 = Bar0 + 0x1000000;
  MmioStart = (UINT16)(Bar0 >> 20) << 4;
  MmioEnd = (UINT16)((Bar0 + 0x1100000 - 1) >> 20) << 4;
  MmioRange = MmioStart | (MmioEnd << 16);

  IoRange = ((IoBase >> 12) << 4) | ((IoBase >> 12) << 12);
  

  DEBUG((EFI_D_INFO, "(L%d) %X %X %X %X %X %X\n", __LINE__, Bar0, Bar1, MmioStart, MmioEnd, MmioRange, IoRange));

  Base = PCI_DEV_MMBASE(0, 1, 4);
  DEBUG((EFI_D_INFO, "(L%d) Base:%X\n", __LINE__, Base));
  DEBUG((EFI_D_INFO, "(L%d) %X\n", __LINE__, MmioRead32(Base)));   
  MmioWrite32(Base+0x18, 0x00040300);
  MmioWrite16(Base+0x1C, IoRange);
  MmioWrite32(Base+0x20, MmioRange);

  Base = PCI_DEV_MMBASE(3, 0, 0);
  DEBUG((EFI_D_INFO, "(L%d) Base:%X\n", __LINE__, Base));
  DEBUG((EFI_D_INFO, "(L%d) %X\n", __LINE__, MmioRead32(Base)));    
  MmioWrite32(Base+0x18, 0x00040403);
  MmioWrite16(Base+0x1C, IoRange);
  MmioWrite32(Base+0x20, MmioRange);

  Base = PCI_DEV_MMBASE(4, 0, 0);
  DEBUG((EFI_D_INFO, "(L%d) Base:%X\n", __LINE__, Base));
  DEBUG((EFI_D_INFO, "(L%d) %X\n", __LINE__, MmioRead32(Base)));    
  MmioWrite32(Base+0x10, Bar0);
  MmioWrite32(Base+0x14, Bar1);
  MmioWrite32(Base+0x18, IoBase);

  MmioWrite8(PCI_DEV_MMBASE(4, 0, 0)+4, 3);
  MmioWrite8(PCI_DEV_MMBASE(3, 0, 0)+4, 7);
  MmioWrite8(PCI_DEV_MMBASE(0, 1, 4)+4, 7);

//DumpPci(0, 1, 5);
//DumpPci(4, 0, 0);
//DumpPci(5, 0, 0);
//
//DumpIo(IoBase, 256);
//DumpMem32((VOID*)Bar0, 256);
//DumpMem32((VOID*)Bar1, 256);  
//
//Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
//DumpRootBridgeResource(Info);

  EarlyGopInfoCtx = (BYO_EARLY_GOP_INFO_PPI_CTX*)AllocateZeroPool(sizeof(BYO_EARLY_GOP_INFO_PPI_CTX));
  ASSERT_EFI_ERROR(EarlyGopInfoCtx != NULL);
  Info = &EarlyGopInfoCtx->Info;
  Info->Version = EARLY_GOP_INFO_VERSION1;
  Info->Bus     = 5;
  Info->Dev     = 0;  
  Info->Fun     = 0;
  Info->LogoDataAddr = (UINTN)LogoData;
  Info->LogoDataSize = (UINTN)LogoDataSize;
  Info->Stall = PeiStall;
  Info->PcieConfigBase       = (UINTN)PcdGet64(PcdPciExpressBaseAddress);
  Info->PciIoTranslation     = 0;
  Info->PciMmio32Translation = 0;

  EarlyGopInfoCtx->Desc.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  EarlyGopInfoCtx->Desc.Guid = &gByoEarlyGopInfoPpiGuid;
  EarlyGopInfoCtx->Desc.Ppi  = (VOID*)Info;
  Status = PeiServicesInstallPpi(&EarlyGopInfoCtx->Desc);
  ASSERT_EFI_ERROR(Status);

  return Status;  
}  


