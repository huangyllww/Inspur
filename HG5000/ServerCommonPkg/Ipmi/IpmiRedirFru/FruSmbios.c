/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#ifndef FRUMAXSTRING
#define FRUMAXSTRING 128
#endif

#include "IpmiRedirFru.h"

#include "Protocol\PlatHostInfoProtocol.h"
#include "Library\DevicePathLib.h" 
#include "Library\BaseMemoryLib.h"
#include "Protocol\DiskInfo.h"
#include "Include\IndustryStandard\Atapi.h"
#include "Library\PlatformCommLib.h" 
EFI_SM_FRU_REDIR_PROTOCOL   *mFruRedirProtocol;
EFI_SMBIOS_PROTOCOL         *mSmbiosProtocol;
extern UINT16               mDeviceID;
//EFI_GUID                    gEfiSmbiosProtocolGuid  = EFI_SMBIOS_PROTOCOL_GUID;

#define _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED   0x0400
#define ATAPI_DEVICE                            0x8000

UINT8
FruStrLen(
  IN CHAR8 *Str
  )
{
  UINT8  Length = 0;
  CHAR8  *Ptr;

  if (Str != NULL){
    Ptr = Str;
    while (*Ptr != '\0'){
      Length++;
      Ptr++;
    }
  }
  return Length;
}

void
GetStringFromFru (
  IN OUT  UINTN           *Offset,
  IN      UINT8           *TempPtr,
  IN OUT  UINT8           *StrPtr
  )
/*++

Routine Description:
  This routine attempts to get a string out of the FRU at the designated offset in the 
  buffer pointed to by TempPtr.  String type is ASCII

Arguments:
  Offset    - Offset of string in buffer pointed to by TempPtr, this is updated to the next
              offset.
  TempPtr   - Pointer to a buffer containing the FRU
  StrPtr    - the pointer to a buffer for retrive the string get from FRU.

Returns:
  STRING_REF with reference to string retrieved from the FRU or NULL if none is found.

--*/
{
  UINTN       Length;
  UINT8       *SrcStrPtr;

  Length     = 0x3F & TempPtr[*Offset];
  SrcStrPtr  = &TempPtr[*Offset + 1];

  ASSERT(Length < FRUMAXSTRING);

  if (Length > 0) {
    CopyMem(StrPtr, SrcStrPtr, Length);
    StrPtr[Length] = '\0';
  } else {
    StrPtr[0] = '\0';
  }
  *Offset = *Offset + Length + 1;
  return;
}

UINT8 *
GetFruInfoArea (
  IN  EFI_SM_FRU_REDIR_PROTOCOL *This,
  IN  UINTN                     Offset
  )
/*++

Routine Description:
  This routine gets the FRU info area specified by the offset and returns it in
  an allocated buffer.  It is the caller's responsibility to free the buffer.

Arguments:
  This      - SM Fru Redir protocol
  Offset    - Info Area starting offset in multiples of 8 bytes.

Returns:
  Buffer with FruInfo data or NULL if not found.

--*/
{
  EFI_STATUS  Status;
  UINT8       *TempPtr;
  UINTN       Length;

  TempPtr = NULL;

  Offset  = Offset * 8;
  if (Offset > 0) {
    //
    // Get Info area length, which is in multiples of 8 bytes
    //
    Length = 0;
    Status = EfiGetFruRedirData (This, 0, (Offset + 1), 1, (UINT8 *) &Length);
    if (Status == EFI_NOT_FOUND) {
      return NULL;
    }
    Length = Length * 8;

    if (Length > 0) {
      TempPtr = AllocateRuntimePool (Length);
      ASSERT (TempPtr != NULL);
      Status = EfiGetFruRedirData (This, 0, Offset, Length, TempPtr);
      if (Status == EFI_NOT_FOUND) {
        return NULL;
      }
    }
  }
  return TempPtr;
}

//
//
//
UINT8*
GetStructureByTypeNo(
  IN  UINT16    TypeNo
)
/*++

Routine Description:
  Type1,2,3 only has one instanse in SMBIOS talbes for each.

Arguments:
  TypeNo    - The number of SMBIOS TYPE

Returns:
  the pointer of SMBIOS TYPE structure.

--*/
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER      *Record;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = mSmbiosProtocol->GetNext (
                              mSmbiosProtocol, 
                              &SmbiosHandle, 
                              (EFI_SMBIOS_TYPE *)&TypeNo, 
                              &Record, 
                              NULL
                              );
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  return (UINT8*) Record;
}

UINT8
SmbiosCheckSum(
  IN UINT8    *ChecksumSrc,
  IN UINT8    length
  )
{
  UINT8     Checksum = 0;
  UINT8     i;

  for (i = 0; i < length; i++) {
    Checksum = Checksum + *ChecksumSrc++;
  }
  return (0 - Checksum);
}

VOID 
DynamicUpdateType(
  IN  UINT16      TypeNo,
  IN  UINTN       StringNo,
  IN  UINT8       *Data
  )
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER      *Record;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = mSmbiosProtocol->GetNext (
                              mSmbiosProtocol, 
                              &SmbiosHandle, 
                              (EFI_SMBIOS_TYPE *)&TypeNo, 
                              &Record, 
                              NULL
                              );
  if (EFI_ERROR(Status)) {
    return;
  }

 mSmbiosProtocol->UpdateString (
                    mSmbiosProtocol,
                    &SmbiosHandle,
                    &StringNo,
                    Data
                    );
  
}
#if 0
//Byosoft +
STATIC UINT16 SmbiosGetStrOffset(CONST SMBIOS_STRUCTURE *Hdr, UINT8 Index)
{
  CONST UINT8 *pData8;
  UINT8 i;

  if(Index == 0){return 0;}

  pData8	= (UINT8*)Hdr;
  pData8 += Hdr->Length;

  i = 1;
  while(i != Index){
    while(*pData8!=0) { pData8++; }
  
    if(pData8[1] == 0){	  // if next byte of a string end is NULL, type end.
      break;
    }

    pData8++;
    i++;
  }

  if(i == Index){
    return (UINT16)(pData8 - (UINT8*)Hdr);
  } else {
    return 0;
  }
}

STATIC
CHAR8 * 
SmbiosGetStringInTypeByIndex(
  SMBIOS_STRUCTURE_POINTER Hdr, 
  SMBIOS_TABLE_STRING 	 StrIndex
)
{
  CHAR8  *Str8;
  
  if(StrIndex == 0){
    return "";
  }

  Str8 = (CHAR8*)(Hdr.Raw + SmbiosGetStrOffset(Hdr.Hdr, StrIndex));
  return Str8;
}



EFI_STATUS
SendBiosInfo2BMC (
  IN EFI_SMBIOS_PROTOCOL  *Smbios,
  IN UINT8                *CommandData,
  IN UINT8                *ResponseData
)
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TYPE              SmbiosType;
  EFI_SMBIOS_TABLE_HEADER      *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER     p;
  UINT8                        ResponseSize;
  BIOS_VERSION_2_BMC           *BiosVer2Bmc;
  CHAR8                        *BiosDate;
  CHAR8                        *BiosVer;
  UINT8                        StrLength;
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr    = SmbiosHdr;
  BiosVer  = SmbiosGetStringInTypeByIndex(p, p.Type0->BiosVersion);
  BiosDate = SmbiosGetStringInTypeByIndex(p, p.Type0->BiosReleaseDate);

  BiosVer2Bmc = (BIOS_VERSION_2_BMC*)CommandData;
  BiosVer2Bmc->Function = SET_BIOS_VER;
  BiosVer2Bmc->BiosVerOffset = OFFSET_OF (BIOS_VERSION_2_BMC, Data);
  StrLength = (UINT8)AsciiStrSize(BiosVer);
  AsciiStrCpy (BiosVer2Bmc->Data, BiosVer);

  BiosVer2Bmc->BiosDateOffset = BiosVer2Bmc->BiosVerOffset + StrLength;
  AsciiStrCpy (&(BiosVer2Bmc->Data[StrLength]), BiosDate);

  StrLength += (UINT8)(sizeof (BIOS_VERSION_2_BMC) + AsciiStrSize(BiosDate));
  Status = EfiSendCommandToBMC (
             SM_BYOSOFT_NETFN_APP,
             SM_BYOSOFT_NETFN_SUB_FUN,
             CommandData,
             StrLength,
             ResponseData,
             &ResponseSize
             );

  return Status;
}





//Byosoft +

VOID
SendCPUinfotoBMC( )
{
  EFI_STATUS                      Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  EFI_SMBIOS_TYPE                 SmbiosType;
  EFI_SMBIOS_TABLE_HEADER        *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER        p;
  UINT8                           Cpunum=0; //cpu number
  UINT16                          Cpucnum; //cpu core number
  UINT16                          CpuFreq;
  CHAR8                          *CpuName;
  UINT8                           commanddata[53]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  UINT8                           ResponseData[53];
  UINT8                           ResponseDataSize;
  UINT8                           i;
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  while(1){

    Status     = mSmbiosProtocol->GetNext(mSmbiosProtocol, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }

    p.Hdr   = SmbiosHdr;
    CpuName = SmbiosGetStringInTypeByIndex(p, p.Type4->ProcessorVersion);
    CpuFreq = p.Type4->CurrentSpeed;
    Cpucnum = p.Type4->CoreCount;
  
    commanddata[0] = 1;                              //parameter 1 = 1 means CPU info 
    commanddata[1] = Cpunum | BIT7;                  //Bit7=1 means valid, bit6~0 means CPU number       
    commanddata[2] = (UINT8)Cpucnum;                 //CPU core number
    commanddata[3] = (UINT8)(CpuFreq & 0xFF);        //low 8 bit  
    commanddata[4] = (UINT8)((CpuFreq >> 8) & 0xFF); //high 8 bit

    for (i=0;i<FruStrLen(CpuName);i++) {
      commanddata[5+i]=*(CpuName+i);
    }

    Status = EfiSendCommandToBMC (0x3E,              //OEM CMD to send CPU info to BMC
                                  0x7a,
                                  (UINT8 *) &commanddata,
                                  sizeof(commanddata),
                                  (UINT8 *) &ResponseData,
                                  (UINT8 *) &ResponseDataSize);
    Cpunum ++;
  }//loop for cpu number not 1
}

VOID
SendDIMMinfotoBMC( )
{
  EFI_STATUS                      Status;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  EFI_SMBIOS_TYPE                 SmbiosType;
  EFI_SMBIOS_TABLE_HEADER        *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER        p;
  UINT8                           DIMMnum= 0;  //dimm number
  UINT8                           Dimmtype= 4; //4-DDR4
  UINT16                          DimmFreq;
  UINT16                          SingleSize;
  CHAR8                          *Manufacturer;
  CHAR8                          *PN;
  CHAR8                          *SN;
  CHAR8                          *DeviceLocator;
  CHAR8                          *BankLocator;
  UINT8                           commanddata[90]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  UINT8                           ResponseData[90];
  UINT8                           ResponseDataSize;
  UINT8                           i;
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_MEMORY_DEVICE;  //type17
  while(1) {
    Status = mSmbiosProtocol->GetNext(mSmbiosProtocol, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr = SmbiosHdr;
    DimmFreq = p.Type17->Speed;
    SingleSize = p.Type17->Size;

    Manufacturer = SmbiosGetStringInTypeByIndex(p, p.Type17->Manufacturer);
    PN = SmbiosGetStringInTypeByIndex(p, p.Type17->PartNumber);
    SN = SmbiosGetStringInTypeByIndex(p, p.Type17->SerialNumber);
    DeviceLocator = SmbiosGetStringInTypeByIndex(p, p.Type17->DeviceLocator); //DIMM 0/1    
    BankLocator = SmbiosGetStringInTypeByIndex(p, p.Type17->BankLocator); //P0/1 CHANNEL A/B/C/D/E/F/G/H

    commanddata[0] = 2;                          //parameter 2 means DIMM info 
    if (SingleSize != 0) {
    commanddata[1] = DIMMnum | BIT7;             //Bit7=1 means valid, bit6~0 means dimm number
    commanddata[5] = Dimmtype;
    }
    else {
    commanddata[1] = DIMMnum;                    //Bit7=0 means not present
    commanddata[5] = 0;                          //no dimm
    }
    commanddata[2] = *(BankLocator + 1);         //Socket;
    commanddata[3] = *(BankLocator + 11);        //Channel;
    commanddata[4] = *(DeviceLocator + 9);       //Dimm;
    commanddata[6] = (UINT8)(DimmFreq & 0xFF);        // low 8 bit  
    commanddata[7] = (UINT8)((DimmFreq >> 8) & 0xFF); //high 8 bit
    commanddata[8] = SingleSize/0x400;
    commanddata[9] = SingleSize%0x400;                // /1024 -> GB 

    for (i=0;i<FruStrLen(Manufacturer);i++) {
      commanddata[10+i]=*(Manufacturer+i);
    }

    for (i=0;i<FruStrLen(PN);i++) {
      commanddata[26+i]=*(PN+i);
    }

    for (i=0;i<FruStrLen(SN);i++) {
      commanddata[58+i]=*(SN+i);
    } 
    DIMMnum++;
    Status = EfiSendCommandToBMC (0x3E,                       //OEM CMD to send BIOS version and build date to BMC
                                0x7a,
                                (UINT8 *) &commanddata,
                                sizeof(commanddata),
                                (UINT8 *) &ResponseData,
                                (UINT8 *) &ResponseDataSize);
    //clear string 
    for (i=0;i<FruStrLen(Manufacturer);i++) {
      commanddata[10+i]=0;
    }

    for (i=0;i<FruStrLen(PN);i++) {
      commanddata[26+i]=0;
    }

    for (i=0;i<FruStrLen(SN);i++) {
      commanddata[58+i]=0;
    } 
  }
}

VOID
SendHDDinfotoBMC( )
{
  EFI_STATUS                      Status;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           Index;
  CHAR8                           *ModelName;
  CHAR8                           *FirmwareVer;
  CHAR8                           *SerialNo;
  CHAR8                           *Module;  
  UINT8                           commanddata[116]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
                                                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
                                                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
                                                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
                                                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,\
                                                    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  UINT8                           ResponseData[116];
  UINT32                          ResponseDataSize;  
  UINT64                          DriveSizeInBytes = 0;  
  UINT32                          DriveSizeInGB = 0;
  UINT32                          BufferSize;
  ATA_IDENTIFY_DATA               *IdentifyData = NULL;
  UINT64                          NumSectors = 0;
  UINT64                          RemainderInBytes = 0;
  UINT8                           i,j=0;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
  if(!EFI_ERROR(Status)){
    IdentifyData = AllocatePool(sizeof(ATA_IDENTIFY_DATA));
    if(IdentifyData == NULL) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;
    }

    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );
      ASSERT_EFI_ERROR(Status); 
 
      BufferSize = sizeof(ATA_IDENTIFY_DATA);
      Status = DiskInfo->Identify (
                       DiskInfo,
                       IdentifyData,
                       &BufferSize
                       );
      if (EFI_ERROR(Status)) {
        DEBUG((EFI_D_ERROR,"Identify failed!\n"));   
        continue;
      }

      ModelName = IdentifyData->ModelName;
      FirmwareVer = IdentifyData->FirmwareVer;
      ConvertIdentifyDataFormat(FirmwareVer, FruStrLen(FirmwareVer));      
      SerialNo = IdentifyData->SerialNo;
      ConvertIdentifyDataFormat(SerialNo, FruStrLen(SerialNo));  
      Module = "AHCI";

      if ((!(IdentifyData->config & ATAPI_DEVICE)) || (IdentifyData->config == 0x848A)) {
        if (IdentifyData->command_set_supported_83 & _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED) { 
          NumSectors = *(UINT64 *)&IdentifyData->maximum_lba_for_48bit_addressing; 
        } else {
          NumSectors = IdentifyData->user_addressable_sectors_lo + (IdentifyData->user_addressable_sectors_hi << 16) ; 
        }

        DriveSizeInBytes = MultU64x32(NumSectors, 512); 
        //
        // DriveSizeInGB is DriveSizeInBytes / 1 GB (1 Binary GB = 2^30 bytes)
        // DriveSizeInGB = (UINT32) Div64(DriveSizeInBytes, (1 << 30), &RemainderInBytes); 
        // Convert the Remainder, which is in bytes, to number of tenths of a Binary GB.
        // NumTenthsOfGB = GetNumTenthsOfGB(RemainderInBytes); 
        // DriveSizeInGB is DriveSizeInBytes / 1 GB (1 Decimal GB = 10^9 bytes)
        //
        DriveSizeInGB = (UINT32) DivU64x64Remainder (DriveSizeInBytes, 1000000000, &RemainderInBytes);  

        commanddata[0] = 4;                    //parameter 4 means hdd info 
        commanddata[1] = j | BIT7;             //Bit7=1 means valid, bit6~0 means HDD number 
        commanddata[2] = (UINT8)(DriveSizeInGB & 0xFF);
        commanddata[3] = (UINT8)((DriveSizeInGB >> 8) & 0xFF); 

        for (i=0;i<FruStrLen(Module);i++) {
          commanddata[4+i]=*(Module+i);
        }

        for (i=0;i<FruStrLen(ModelName);i++) {
          commanddata[20+i]=*(ModelName+i);
        }

        for (i=0;i<FruStrLen(SerialNo);i++) {
          commanddata[52+i]=*(SerialNo+i);
        }

//       for (i=0;i<FruStrLen(FirmwareVer);i++) {
        for (i=0;i<8;i++) {
          commanddata[84+i]=*(FirmwareVer+i);
        }

        Status = EfiSendCommandToBMC (0x3E,                       //OEM CMD to send BIOS version and build date to BMC
                                0x7a,
                                (UINT8 *) &commanddata,
                                sizeof(commanddata),
                                (UINT8 *) &ResponseData,
                                (UINT8 *) &ResponseDataSize);
        j++;
      }//ata dev
    } //for loop
  } //LocateHandleBuffer

  if(j==0) {  //no hdd device
    commanddata[0] = 4;                    //parameter 4 means hdd info 
    commanddata[1] = 0;             //Bit7=1 means valid, bit6~0 means HDD number 
    commanddata[2] = 0;
    commanddata[3] = 0; 
    Status = EfiSendCommandToBMC (0x3E,                       //OEM CMD to send BIOS version and build date to BMC
                              0x7a,
                              (UINT8 *) &commanddata,
                              sizeof(commanddata),
                              (UINT8 *) &ResponseData,
                              (UINT8 *) &ResponseDataSize);
  }
ProcExit:  
  if(IdentifyData != NULL){
    FreePool(IdentifyData);}
  if (HandleBuffer!= NULL){
    FreePool(HandleBuffer);}
}

#endif

void
UpdateType1UUID ()
{
  EFI_STATUS                        Status;
  SMBIOS_TABLE_TYPE1                *TablePtr;
  EFI_IPMI_MSG_GET_DEVICE_GUID_RSP  ResponseData;  
  UINT8                             ResponseDataSize;

  ResponseDataSize = sizeof (ResponseData);
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_APP,
             EFI_APP_GET_SYSTEM_GUID,
             (UINT8 *) NULL,
             0,
             (UINT8 *) &ResponseData,
             (UINT8 *) &ResponseDataSize
             );
  if (EFI_ERROR (Status)) {
    return;
  }

  TablePtr = (SMBIOS_TABLE_TYPE1*)GetStructureByTypeNo (SMBIOSTYPE1);
  CopyMem(&TablePtr->Uuid, &ResponseData, ResponseDataSize);
}
/*
void
UpdateType39 (
  IN  EFI_SMBIOS_HANDLE    SmbiosHandle,
  IN  SMBIOS_TABLE_TYPE39  *TablePtr
)
{
  EFI_STATUS                        Status;
  UINT8                             *FruHdrPtr;
  UINT8                             FruHdrChksum;
  UINTN                             i;
  IPMI_FRU_COMMON_HEADER            FruCommonHeader; 
  UINTN                             Offset;
  UINT8                             *TempPtr;
  UINTN                             StringNo;
  UINT8                             TempStr[FRUMAXSTRING];
  
  Status = EfiGetFruRedirData (mFruRedirProtocol, 0, 0, sizeof (IPMI_FRU_COMMON_HEADER), (UINT8 *) &FruCommonHeader);
  if (Status == EFI_NOT_FOUND) {
    DEBUG ((EFI_D_ERROR, "Type39 FRU header not found.\n"));
    //
    //  The FRU information was not found, nothing need to do.
    //
    return;
  } else {
    //
    // Do a validity check on the FRU header, since it may be all 0xFF(s) if
    // there is no FRU programmed on the system.
    //
    FruHdrPtr = (UINT8 *) &FruCommonHeader;
    for (i = 0, FruHdrChksum = 0; i < sizeof (FruCommonHeader); i++) {
      FruHdrChksum = (UINT8) (FruHdrChksum +*FruHdrPtr++);
    }
    if (FruHdrChksum != 0) {
      DEBUG ((EFI_D_ERROR, "Type39 FRU header invalid.\n"));
      //
      //  The FRU information is bad so nothing need to do.
      //
      return;
    }
  }

  TempPtr = GetFruInfoArea (mFruRedirProtocol, FruCommonHeader.ProductInfoStartingOffset);
  if (TempPtr != NULL) {
    //
    // Get the following fields in the specified order.  DO NOT change this order unless the FRU file definition
    // changes.  Offset is initialized and then is incremented to the next field offset in GetStringFromFru.
    //
    // Product Manufacturer
    // Product Name
    // Product Model Number / System Part Number
    // Product Version
    // Product Serial Number
    // Product Asset Tag => Max power
    //
    Offset = PRODUCT_MFG_OFFSET;
    GetStringFromFru (&Offset, TempPtr, TempStr);    //MiscSystemManufacturer.SystemManufacturer
    if (FruStrLen(TempStr) != 0) {
      StringNo = 3;
      mSmbiosProtocol->UpdateString (
                         mSmbiosProtocol,
                         &SmbiosHandle,
                         &StringNo,
                         (CHAR8*)TempStr
                         );
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);    //MiscSystemManufacturer.SystemProductName
    if (FruStrLen(TempStr) != 0) {
      StringNo = 2;
      mSmbiosProtocol->UpdateString (
                         mSmbiosProtocol,
                         &SmbiosHandle,
                         &StringNo,
                         (CHAR8*)TempStr
                         );
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);    //***********************SystemPartNum
    if (FruStrLen(TempStr) != 0) {
      StringNo = 6;
      mSmbiosProtocol->UpdateString (
                         mSmbiosProtocol,
                         &SmbiosHandle,
                         &StringNo,
                         (CHAR8*)TempStr
                         );
    }
    
    GetStringFromFru (&Offset, TempPtr, TempStr);    //MiscSystemManufacturer.SystemVersion
    if (FruStrLen(TempStr) != 0) {
      StringNo = 7;
      mSmbiosProtocol->UpdateString (
                         mSmbiosProtocol,
                         &SmbiosHandle,
                         &StringNo,
                         (CHAR8*)TempStr
                         );

    }
  
    GetStringFromFru (&Offset, TempPtr, TempStr);    //MiscSystemManufacturer.SystemSerialNumber
    if (FruStrLen(TempStr) != 0) {
      StringNo = 4;
      mSmbiosProtocol->UpdateString (
                         mSmbiosProtocol,
                         &SmbiosHandle,
                         &StringNo,
                         (CHAR8*)TempStr
                         );

    }
  
    GetStringFromFru (&Offset, TempPtr, TempStr);    //***********************AssetTag
  
    FreePool (TempPtr);
  }
  
}

void
UpdateType39Infor ()
{
  EFI_STATUS                        Status;
  SMBIOS_TABLE_TYPE39               *TablePtr;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  UINT16                            TypeNo = 39;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = mSmbiosProtocol->GetNext (
                              mSmbiosProtocol, 
                              &SmbiosHandle, 
                              (EFI_SMBIOS_TYPE *)&TypeNo, 
                              (EFI_SMBIOS_TABLE_HEADER**)&TablePtr, 
                              NULL
                              );
  if (EFI_ERROR(Status)) {
    return;
  }
  mDeviceID = 1; // PSU1
  UpdateType39 (SmbiosHandle, TablePtr);

  Status = mSmbiosProtocol->GetNext (
                              mSmbiosProtocol, 
                              &SmbiosHandle, 
                              (EFI_SMBIOS_TYPE *)&TypeNo, 
                              (EFI_SMBIOS_TABLE_HEADER**)&TablePtr, 
                              NULL
                              );
  if (EFI_ERROR(Status)) {
    return;
  }
  mDeviceID = 2; //PSU2
  UpdateType39 (SmbiosHandle, TablePtr);
}

*/

VOID 
EFIAPI 
GenerateFruSmbiosType123DataNotified (
  IN EFI_EVENT Event, 
  IN VOID      *Context
  )
/*++

Routine Description:
  This routine is notified by protocol gEfiSmbiosProtocolGuid, and reads strings out of the FRU and populates them into the appropriate
  Smbios talbes.  

Arguments:
  Event                        - None
  Context                      - None
Returns:
  VOID

--*/
{
  EFI_STATUS                      Status;
  UINT8                           *FruHdrPtr;
  UINT8                           FruHdrChksum;
  IPMI_FRU_COMMON_HEADER          FruCommonHeader; 
  UINT8                           i;
  UINTN                           Offset;
  UINT8                           *TempPtr;
  UINT8                           TempStr[FRUMAXSTRING];
  UINT8                           *TablePtr;
  //VOID                            *Interface;
#if 0
  CHAR8                           *ProductName="THTF";
  UINT8                           *ResponseData;
  UINT8                           *CommandData;
#endif
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  

 /* Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_INFO,"Locate gBdsAllDriversConnectedProtocolGuid failed\n"));
    return;
  }*/

  gBS->CloseEvent(Event);
  SetMem((UINT8*)(&FruCommonHeader), sizeof(IPMI_FRU_COMMON_HEADER), 0);
  SetMem(TempStr, FRUMAXSTRING, 0);

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, &mSmbiosProtocol);
  ASSERT_EFI_ERROR(Status);
#if 0
  CommandData  = AllocatePool(BIOS_2_BMC_DATA_SIZE);
  ResponseData = AllocatePool(BIOS_2_BMC_DATA_SIZE);

  Status = SendBiosInfo2BMC(mSmbiosProtocol, CommandData, ResponseData);

  SendCPUinfotoBMC();
  SendDIMMinfotoBMC();
  SendHDDinfotoBMC();  

  FreePool(CommandData);
  FreePool(ResponseData);
#endif
  Status = EfiGetFruRedirData (mFruRedirProtocol, 0, 0, sizeof (IPMI_FRU_COMMON_HEADER), (UINT8 *) &FruCommonHeader);
  if (Status == EFI_NOT_FOUND) {
    DEBUG ((EFI_D_ERROR, "FRU header not found.\n"));
    //
    //  The FRU information was not found, nothing need to do.
    //
    return;
  } else {
    //
    // Do a validity check on the FRU header, since it may be all 0xFF(s) if
    // there is no FRU programmed on the system.
    //
    FruHdrPtr = (UINT8 *) &FruCommonHeader;
    for (i = 0, FruHdrChksum = 0; i < sizeof (FruCommonHeader); i++) {
      FruHdrChksum = (UINT8) (FruHdrChksum +*FruHdrPtr++);
    }

    if (FruHdrChksum != 0) {
      DEBUG ((EFI_D_ERROR, "FRU header invalid.\n"));
      //
      //  The FRU information is bad so nothing need to do.
      //
      return;
    }
  
  }
  //
  // SMBIOS Type 1, Product data
  //
  TempPtr = GetFruInfoArea (mFruRedirProtocol, FruCommonHeader.ProductInfoStartingOffset);
  if (TempPtr != NULL) {
    //
    // Get the following fields in the specified order.  DO NOT change this order unless the FRU file definition
    // changes.  Offset is initialized and then is incremented to the next field offset in GetStringFromFru.
    //
    // Product Manufacturer
    // Product Name
    // Product Model Number / System Part Number
    // Product Version
    // Product Serial Number
    // Product Asset Tag
    //
    Offset = PRODUCT_MFG_OFFSET;
    GetStringFromFru (&Offset, TempPtr, TempStr);    //MiscSystemManufacturer.SystemManufacturer
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING1, TempStr);
      DynamicUpdateType(SMBIOSTYPE3, STRING1, TempStr);
    DEBUG ((EFI_D_INFO, "[Stephen] TYPE1&3.MANUFACTURER: %a\n",TempStr));
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);    //MiscSystemManufacturer.SystemProductName
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING2, TempStr);
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);    //***********************SystemPartNum
    GetStringFromFru (&Offset, TempPtr, TempStr);    //MiscSystemManufacturer.SystemVersion
    
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING3, TempStr);
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);    //MiscSystemManufacturer.SystemSerialNumber
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING4, TempStr);
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);    //***********************AssetTag
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE3, STRING4, TempStr);   //NOTICE: this Asset Tag can be used by type 3 table
     // DynamicUpdateType(SMBIOSTYPE2, STRING5, TempStr);   //NOTICE: this Asset Tag can be used by type 2 table  hjm++
    }

	GetStringFromFru (&Offset, TempPtr, TempStr);    //SKUnumber
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE1, STRING5, TempStr);
      DEBUG ((EFI_D_INFO, "[Stephen] TYPE1.SKUNUMBER: %s\n",TempStr));  //hjm++  (Not be executed. TangYan's comment)
    }

    FreePool (TempPtr);
  }

  //
  // SMBIOS Type 2, Base Board data
  //
  TempPtr = GetFruInfoArea (mFruRedirProtocol, FruCommonHeader.BoardAreaStartingOffset);
  if (TempPtr != NULL) {
    //
    // Get the following fields in the specified order.  DO NOT change this order unless the FRU file definition
    // changes.  Offset is initialized and then is incremented to the next field offset in GetStringFromFru.
    //
    // Board Manufacturer
    // Board Product Name
    // Board Serial Number
    // Board Part Number
    // FRU Version Number
    //
    Offset  = BOARD_MFG_OFFSET;
    GetStringFromFru (&Offset, TempPtr, TempStr);  //MiscBaseBoardManufacturer.BaseBoardManufacturer
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING1, TempStr);
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);  //MiscBaseBoardManufacturer.BaseBoardProductName
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING2, TempStr);
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);  //MiscBaseBoardManufacturer.BaseBoardSerialNumber
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING4, TempStr);
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);  //MiscBaseBoardManufacturer.BaseBoardVersion
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING3, TempStr);
    }
    
	GetStringFromFru (&Offset, TempPtr, TempStr);  //AssetTag  //hjm++  (Null string. TangYan's comment)
    GetStringFromFru (&Offset, TempPtr, TempStr);  //Chassis Location  //hjm++  (Null string. TangYan's comment)
    
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE2, STRING6, TempStr);  //hjm++  
      DEBUG ((EFI_D_INFO, "[Stephen] TYPE2.Location in Chassis: %a\n",TempStr));  //Not be executed. TangYan's comment
    }
    // Not currently needed in SMBIOS, uncomment and copy to SMBIOS if needed.
    //GetStringFromFru (&Offset, TempPtr, TempStr);  //**************************FRU Version Number

    FreePool (TempPtr);
  }

  //
  // SMBIOS Type 3, Chassis data
  //
  TempPtr = GetFruInfoArea (mFruRedirProtocol, FruCommonHeader.ChassisInfoStartingOffset);
  if (TempPtr != NULL) {
    //special process:
    TablePtr = GetStructureByTypeNo(SMBIOSTYPE3);
    ASSERT (TablePtr != NULL);
    ((SMBIOS_TABLE_TYPE3*)TablePtr)->Type = TempPtr[CHASSIS_TYPE_OFFSET];
    //
    // Get the following fields in the specified order.  DO NOT change this order unless the FRU file definition
    // changes.  Offset is initialized and then is incremented to the next field offset in GetStringFromFru.
    //
    Offset  = CHASSIS_PART_NUMBER;
    GetStringFromFru (&Offset, TempPtr, TempStr);  //MiscChassisManufacturer.ChassisVersion
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE3, STRING2, TempStr);
    }

    GetStringFromFru (&Offset, TempPtr, TempStr);  //MiscChassisManufacturer.ChassisSerialNumber
    if (FruStrLen(TempStr) != 0) {
      DynamicUpdateType(SMBIOSTYPE3, STRING3, TempStr);
    }

    //GetStringFromFru (&Offset, TempPtr, TempStr);  //MiscChassisManufacturer.ChassisManufacturer  
    //if (FruStrLen(TempStr) != 0) {
    //  DynamicUpdateType(SMBIOSTYPE3, STRING1, TempStr);
    //}

    FreePool (TempPtr);
  }
  UpdateType1UUID ();
//  UpdateType39Infor ();
  return;
}

VOID
GenerateFruSmbiosData (
  IN EFI_SM_FRU_REDIR_PROTOCOL            *This
  )
/*++

Routine Description:
  This routine install a notify function listen to EfiSmbiosProtocol

Arguments:
  This                        - SM Fru Redir protocol

Returns:
  VOID

--*/
{
  VOID          *EventReg;
  
  mFruRedirProtocol = This;

  EfiCreateProtocolNotifyEvent (
    &gBdsAllDriversConnectedProtocolGuid,
    TPL_CALLBACK,
    GenerateFruSmbiosType123DataNotified,
    NULL,
    &EventReg
    ); 

}
