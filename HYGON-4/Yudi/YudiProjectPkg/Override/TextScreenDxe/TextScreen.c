/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By:
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include <Library/HiiLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <ByoBootGroup.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <protocol/DiskInfo.h>
#include <IndustryStandard/Atapi.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/AtaPassThru.h>
#include <SysMiscCfg.h>
#include <Guid/MdeModuleHii.h>
#include <EfiServerManagement.h>
#include <Library/NetLib.h>
#include <Protocol/PlatHwInfoProtocol.h>
#include <Library/PlatformCommLib.h>
#include <Library/ProjectCommonLib.h>
#include <SetupVariable.h>


extern EFI_GUID gEfiIpmiTransportProtocolGuid;


EFI_HII_HANDLE    mHiiHandle = NULL;
EFI_GUID mTextScreenStringPackGuid = {
  0xDCEFF379, 0xD6E5, 0x41C9, { 0xBD, 0x83, 0xB1, 0xC4, 0x64, 0x51, 0xC1, 0x77 }
};

#define CHARACTER_NUMBER_FOR_VALUE  30

UINTN     MaxWidth  = 0;
UINTN     EraseLine = 0;

CONST SETUP_DATA  *mSetupHob;


UINTN
GetStringWidth (
  IN CHAR16               *String
  )
{
  UINTN Index;
  UINTN Count;
  UINTN IncrementValue;

  ASSERT (String != NULL);
  if (String == NULL) {
    return 0;
  }

  Index           = 0;
  Count           = 0;
  IncrementValue  = 1;

  do {
    //
    // Advance to the null-terminator or to the first width directive
    //
    for (;
         (String[Index] != NARROW_CHAR) && (String[Index] != WIDE_CHAR) && (String[Index] != 0);
         Index++, Count = Count + IncrementValue
        )
      ;

    //
    // We hit the null-terminator, we now have a count
    //
    if (String[Index] == 0) {
      break;
    }
    //
    // We encountered a narrow directive - strip it from the size calculation since it doesn't get printed
    // and also set the flag that determines what we increment by.(if narrow, increment by 1, if wide increment by 2)
    //
    if (String[Index] == NARROW_CHAR) {
      //
      // Skip to the next character
      //
      Index++;
      IncrementValue = 1;
    } else {
      //
      // Skip to the next character
      //
      Index++;
      IncrementValue = 2;
    }
  } while (String[Index] != 0);

  //
  // Increment by one to include the null-terminator in the size
  //
  Count++;

  return Count * sizeof (CHAR16);
}


VOID
TextScreenPrintString(
  IN CONST CHAR16  *Format,
  ...
  )
{
  VA_LIST    Marker;
  CHAR16    *Buffer = NULL;
  UINTN    BufferSize;
  UINTN    Return;
  CHAR16    *BlankBuffer = NULL;
  UINTN    Columns;
  UINTN    Rows;

  Columns = 100;
  Rows = 3;
  gST->ConOut->QueryMode(gST->ConOut, gST->ConOut->Mode->Mode, &Columns, &Rows);

  BufferSize = (Columns + 1) * sizeof (CHAR16);
  Buffer = (CHAR16 *) AllocateZeroPool(BufferSize);
  if(Buffer == NULL) {
    return;
  }	
  BlankBuffer = (CHAR16 *) AllocateZeroPool(BufferSize);
  if(BlankBuffer == NULL) {
    FreePool (Buffer);
    return;
  }
  SetMem16(BlankBuffer,Columns,L' ');
  BlankBuffer[Columns] = L'\0';

  if (EraseLine == Rows - 3) {  
    gBS->Stall (2000000);
    EraseLine = 0;  // back to top
    gST->ConOut->SetCursorPosition(gST->ConOut,0,EraseLine);
  }
	
  VA_START (Marker, Format);
  Return = UnicodeVSPrint (Buffer, BufferSize, Format, Marker);
  VA_END (Marker);
  if (Return > 0) {
    gST->ConOut->OutputString (gST->ConOut, BlankBuffer);
    gST->ConOut->SetCursorPosition(gST->ConOut,0,EraseLine);
    gST->ConOut->OutputString (gST->ConOut, Buffer);
    MaxWidth = MAX(GetStringWidth(Buffer), MaxWidth);
    DEBUG((EFI_D_ERROR, __FUNCTION__"(L.%d), EraseLine %02d :%s.\n", __LINE__, EraseLine, Buffer));
    EraseLine++;
  }
	
  FreePool (Buffer);
  FreePool (BlankBuffer);
  return;
}

//
// For Memory Info.
//
CHAR8     gUnitStr[]  = {"MB"};

UINT32 
UpdateMBSizeUnit (
  UINT32 SizeMB, 
  CHAR8 *UnitStr
  )
{
  UINT32  Size;

  if(SizeMB >= 1024 && (SizeMB % 1024) == 0){
    Size     = SizeMB / 1024;
    *UnitStr = 'G';
  } else {
    Size     = SizeMB;
    *UnitStr = 'M';
  }

  return Size;
}

typedef struct _MEM_BRAND_STR {
  CHAR8            *ManufacturerTag;
  EFI_STRING_ID    StringId;
  UINTN            CheckCount;
} MEM_BRAND_STR;

MEM_BRAND_STR    gMemBrand[] = {
  "Samsung", STRING_TOKEN(STR_MEM_BRAND_SAMSUNG), 0,
  "SK Hynix", STRING_TOKEN(STR_MEM_BRAND_HYNIX), 0,
  "Micron Technology", STRING_TOKEN(STR_MEM_BRAND_MICRON), 0,
  "UniIC", STRING_TOKEN(STR_MEM_BRAND_UNIIC), 0,
  "Ramaxel", STRING_TOKEN(STR_MEM_BRAND_RAMAXEL), 0,
  "Infineon (Siemens)", STRING_TOKEN(STR_MEM_BRAND_INFINEON), 0,
  "Transcend Information", STRING_TOKEN(STR_MEM_BRAND_TRANSCEND), 0,
  "Kingston", STRING_TOKEN(STR_MEM_BRAND_KINGSTON), 0,
  "Elpida", STRING_TOKEN(STR_MEM_BRAND_ELPIDA), 0,
  "Nanya Technology", STRING_TOKEN(STR_MEM_BRAND_NANYA), 0,
  "Kingmax Semiconductor", STRING_TOKEN(STR_MEM_BRAND_KINGMAX), 0,
  "A-DATA Technology", STRING_TOKEN(STR_MEM_BRAND_A_DATA), 0,
  "ASint Technology", STRING_TOKEN(STR_MEM_BRAND_ASINT), 0,
  "ChangXin Memory", STRING_TOKEN(STR_MEM_BRAND_CHANGXIN),
};

UINTN
CheckMemBrandStr (
  CHAR8    *ManufacturerStr
  )
{
  UINTN    Index, Count;

  if (NULL == ManufacturerStr) {
    return 0;
  }

  Count = 0;
  for (Index = 0; Index < (sizeof(gMemBrand) / sizeof(MEM_BRAND_STR)); Index++) {
    if (NULL == gMemBrand[Index].ManufacturerTag) {
      continue;
    }

    if(AsciiStrStr(ManufacturerStr, gMemBrand[Index].ManufacturerTag) !=NULL) {
      Count = gMemBrand[Index].CheckCount;
      gMemBrand[Index].CheckCount++;
    }
  }
  return Count;
}

VOID 
ShowMemoryInfo (
  EFI_SMBIOS_PROTOCOL *Smbios
  )
{
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_TYPE                   SmbiosType;
  EFI_SMBIOS_TABLE_HEADER           *SmbiosHdr;  
  UINTN                             DimmCount;
  EFI_STATUS                        Status;  
  UINT32                            SingleSize;
  CHAR8                             *DimmTypeString[] = {"DRAM", "RDRAM", "DDR", "DDR2", "DDR3","DDR4"};
  UINT8                             DimmType;
  UINT32                            TotalMemSize = 0;
  CHAR16                            *MemTotalStr;
  CHAR16                            *TotalSize;
  SMBIOS_STRUCTURE_POINTER          p;
  CHAR8                             *Manufacturer;
  UINT16                            ConfiguredMemSpeed;
  UINT16                            MaxMemSpeed;
  CHAR16                            *TotalCount;
  CHAR16                            *Type;
  CHAR16                            *CurrentFrequency;
  CHAR16                            *MaxFrequency;
  UINTN                             Index;
  CHAR16                            *UniMafc;
  CHAR8                             *MemFormFactor;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  MemTotalStr = HiiGetString(mHiiHandle, STRING_TOKEN(STR_SYS_MEMORY), NULL);
  TotalSize   = HiiGetString(mHiiHandle, STRING_TOKEN(STR_TOTAL_SIZE), NULL);
  TotalCount  = HiiGetString(mHiiHandle, STRING_TOKEN(STR_TOTAL_COUNT), NULL);
  Type        = HiiGetString(mHiiHandle, STRING_TOKEN(STR_TYPE), NULL);
  CurrentFrequency = HiiGetString(mHiiHandle, STRING_TOKEN(STR_MEM_CURRENT_FREQUENCY), NULL);
  MaxFrequency     = HiiGetString(mHiiHandle, STRING_TOKEN(STR_MEM_MAX_FREQUENCY), NULL);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  DimmCount    = 0;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if (EFI_ERROR(Status)) {
      break;
    }

    p.Hdr = SmbiosHdr;
    SingleSize = p.Type17->Size;
    if(SingleSize == 0 || SingleSize == 0xFFFF){
      SingleSize = 0;
    } else if(SingleSize == 0x7FFF){
      SingleSize = p.Type17->ExtendedSize & (~BIT31);   // MB
    } else {
      if(SingleSize & BIT15){                               // unit in KB.
        SingleSize = (SingleSize&(~BIT15)) >> 10;           // to MB
      }
    }

    if(SingleSize == 0){
      continue;
    }

    TotalMemSize += SingleSize; 
    ConfiguredMemSpeed = p.Type17->ConfiguredMemoryClockSpeed;
    MaxMemSpeed        = p.Type17->Speed;
    Manufacturer = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->Manufacturer);
    DimmType = p.Type17->MemoryType;
    switch (DimmType) {
      case MemoryTypeDram:
          DimmType = 0;
          break;
      case MemoryTypeRdram:
          DimmType = 1;
          break;
      case MemoryTypeDdr:
          DimmType = 2;
          break;
      case MemoryTypeDdr2:
          DimmType = 3;
          break;
      case MemoryTypeDdr3:
          DimmType = 4;
          break;
      case MemoryTypeDdr4:
          DimmType = 5;
          break;
    }       

    MemFormFactor = SmbiosGetDimmDeviceTypeStringInFormFactor (p.Type17->FormFactor, p.Type17->TypeDetail);
    DimmCount++;
		
    if (!CheckMemBrandStr (Manufacturer)) {
      for (Index = 0; Index < (sizeof(gMemBrand) / sizeof(MEM_BRAND_STR)); Index++) {
        if (AsciiStrStr(Manufacturer, gMemBrand[Index].ManufacturerTag) != NULL) {
          UniMafc = HiiGetString(mHiiHandle, STRING_TOKEN(gMemBrand[Index].StringId), NULL);
          TextScreenPrintString(L"%s: %s, %s: %a %a, %s: %dMHz, %s: %dMHz\n",MemTotalStr,UniMafc,Type,DimmTypeString[DimmType],MemFormFactor,CurrentFrequency,ConfiguredMemSpeed, MaxFrequency, MaxMemSpeed);
          FreePool(UniMafc);
        }
      }
    }
  }
  TextScreenPrintString(L"%s: %d%a, %s: %d\n", TotalSize, UpdateMBSizeUnit(TotalMemSize, &gUnitStr[0]), gUnitStr,TotalCount, DimmCount);

  FreePool(MemTotalStr);
  FreePool(TotalSize);
  FreePool(TotalCount);
  FreePool(Type);
  FreePool(CurrentFrequency);
  FreePool(MaxFrequency);
  if (MemFormFactor != NULL) {
    FreePool (MemFormFactor);
  }
}

//
// For BMC Info.
//
VOID ShowBmcInfo(PLATFORM_COMM_INFO *Info)  //j_t PlatIpmiPeiCallBack
{
  CHAR16                            *BmcIpAddressStr;
  CHAR16                            *BmcVersionStr;
  CHAR16                            *InvalidStr;
  CHAR16                            Str[46];
  EFI_STATUS                        Status;
  UINT8                             *p;
  CHAR8                             *BmcVer;
	
  BmcIpAddressStr = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BMC_IP), NULL);
  BmcVersionStr   = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BMC_VERSION), NULL);
  InvalidStr      = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BMC_IP_INVALID), NULL);  

  BmcVer = PcdGetPtr(PcdBmcFwVerStr8);
  if(BmcVer[0] != 0){
    TextScreenPrintString(L"%s: %a\n", BmcVersionStr, BmcVer);
  }
  if(Info->BmcIpType == 4){
    p = Info->BmcIp.v4.Addr;
    UnicodeSPrint(Str, sizeof(Str), L"%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    TextScreenPrintString(L"%s: %s\n", BmcIpAddressStr, Str);
  } else if(Info->BmcIpType == 6){
    Status = NetLibIp6ToStr(&Info->BmcIp.v6, Str, sizeof(Str));
    if(!EFI_ERROR(Status)){
      TextScreenPrintString(L"%s: %s\n", BmcIpAddressStr, Str);
    }
  } else {
    TextScreenPrintString(L"%s: %s\n", BmcIpAddressStr, InvalidStr);
  }

  FreePool(BmcIpAddressStr);
  FreePool(BmcVersionStr);
  FreePool(InvalidStr);
}

VOID 
ShowBiosInfo (
  EFI_SMBIOS_PROTOCOL *Smbios
  )
{
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_TYPE                   SmbiosType; 
  EFI_STATUS                        Status;
  EFI_SMBIOS_TABLE_HEADER           *SmbiosHdr;
  CHAR8                             *BiosVer;
  CHAR8                             *BiosDate;
  CHAR16                            *BiosVersionStr;
  CHAR16                            *BiosReleaseDateStr;
  SMBIOS_STRUCTURE_POINTER          p;
  
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  BiosVersionStr = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BIOS_VERSION), NULL);
  BiosReleaseDateStr = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BIOS_RELEASE_DATE), NULL);
  
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  BiosVer = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosVersion);
  BiosDate = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosReleaseDate);

  TextScreenPrintString(L"%s: %a, %s: %a\n", BiosVersionStr, BiosVer, BiosReleaseDateStr, BiosDate);
  
  FreePool(BiosVersionStr);
  FreePool(BiosReleaseDateStr);
}


VOID 
ShowCpuInfo (
  EFI_SMBIOS_PROTOCOL *Smbios
  )
{
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_TYPE                   SmbiosType; 
  EFI_STATUS                        Status;
  EFI_SMBIOS_TABLE_HEADER           *SmbiosHdr;
  CHAR16                            *CpuCurrentFrequency;
  CHAR8                             *CpuName = NULL;
  CHAR8                             *SocketName = NULL;
  SMBIOS_STRUCTURE_POINTER          p;
  UINT32                            Cpuid;
  UINT16                            CpuSpeed;
  CHAR16                            *CPU;
  static UINT8    CpuIndex = 0;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  CpuCurrentFrequency = HiiGetString(mHiiHandle, STRING_TOKEN(STR_CPU_CURRENT_FREQUENCY), NULL);
  CPU = HiiGetString(mHiiHandle, STRING_TOKEN(STR_CPU), NULL);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr = SmbiosHdr;

    SocketName = NULL;
    SocketName = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->Socket);
        
    CpuName  = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->ProcessorVersion);
    CpuSpeed = p.Type4->CurrentSpeed;

    Cpuid = *(UINT32*)&(p.Type4->ProcessorId.Signature);
    if (Cpuid) {
      TextScreenPrintString(L"%s %d: %a, %s: %dMHz\n", CPU, CpuIndex, CpuName, CpuCurrentFrequency, CpuSpeed);
      CpuIndex++;
    }
  }

  FreePool(CpuCurrentFrequency);
  FreePool(CPU);
}


VOID 
ShowOsDiskInfo (
 VOID 
 )
{
  EFI_STATUS                      Status;
  UINTN                           Index;
  EFI_BDS_BOOT_MANAGER_PROTOCOL   *BdsBootManager;
  BOOLEAN                         OsHDDExist;

  OsHDDExist = FALSE;
  Status = gBS->LocateProtocol (&gEfiBootManagerProtocolGuid,
                                NULL,
                                (VOID **)&BdsBootManager);
  if (EFI_ERROR (Status)) {
    return ;
  }
	
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), BootOptionsCount :%d.\n", __LINE__, BdsBootManager->BootOptionsCount));
  for (Index = 0; Index < BdsBootManager->BootOptionsCount ; Index++) {
    if (((BdsBootManager->BootOptions[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) || ((BdsBootManager->BootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0)) {
      continue;
    }
    if (BdsBootManager->BootOptions[Index].GroupType != BM_MENU_TYPE_UEFI_HDD) {
      continue;
    }
    if (!IsOsDiskByDescription(BdsBootManager->BootOptions[Index].Description)) { 
      continue;
    }
    OsHDDExist = TRUE;
    TextScreenPrintString(L"%s\n", BdsBootManager->BootOptions[Index].Description);
  }
  if (OsHDDExist) {
    gBS->Stall(2000000);
  }
}


// SMART RETURN STATUS
//   Features = DAh
//   LBA_Mid  = 4Fh
//   LBA_High = C2h
//   Command  = B0h
// If the device has not detected a threshold exceeded condit ion, the device sets 
// the LBA Mid register to 4Fh and the LBA High register to C2h. If the device has 
// detected a threshold exceeded condition, the device sets the LBA Mid register 
// to F4h and the LBA High register to 2Ch. 

EFI_STATUS 
CheckSmart (
  EFI_ATA_PASS_THRU_PROTOCOL      *AtaPassThru,
  UINT16                          Port,
  UINT16                          PortMp,
  EFI_HANDLE                      Handle,
  UINT16                          HostIndex,
  BOOLEAN                         *HasError
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;
  EFI_ATA_STATUS_BLOCK              *Asb;

  if (AtaPassThru == NULL || Handle == NULL || HasError == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *HasError = FALSE;
  Asb = (EFI_ATA_STATUS_BLOCK*)AllocateAlignedPages (
                                 EFI_SIZE_TO_PAGES (sizeof(EFI_ATA_STATUS_BLOCK)),
                                 AtaPassThru->Mode->IoAlign
                                 );
  if (Asb == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (Asb, sizeof (EFI_ATA_STATUS_BLOCK));
  ZeroMem (&Acb, sizeof (EFI_ATA_COMMAND_BLOCK));
  Acb.AtaCommand      = ATA_CMD_SMART;
  Acb.AtaFeatures     = ATA_SMART_RETURN_STATUS;
  Acb.AtaCylinderLow  = ATA_CONSTANT_4F;
  Acb.AtaCylinderHigh = ATA_CONSTANT_C2;

  ZeroMem(&Packet, sizeof(Packet));
  Packet.Protocol = EFI_ATA_PASS_THRU_PROTOCOL_ATA_NON_DATA;
  Packet.Length   = EFI_ATA_PASS_THRU_LENGTH_NO_DATA_TRANSFER;
  Packet.Asb      = Asb;
  Packet.Acb      = &Acb;
  Packet.Timeout  = EFI_TIMER_PERIOD_SECONDS(3);

  Status = AtaPassThru->PassThru(
                          AtaPassThru,
                          Port,
                          PortMp,
                          &Packet,
                          NULL
                          );
  DEBUG ((EFI_D_INFO, __FUNCTION__"(), %d SATA(%X,%X), Status :%r %X-%X.\n", HostIndex, Port, PortMp, Status, Acb.AtaCylinderLow, Acb.AtaCylinderHigh));
  if(!EFI_ERROR(Status)){
    if (Acb.AtaCylinderLow == 0xf4 && Acb.AtaCylinderHigh == 0x2c) {
      if(HostIndex == 0x8000){
        TextScreenPrintString(L"SATA%d S.M.A.R.T ERROR\n", Port);
      } else {
        TextScreenPrintString(L"SATA%d-%d S.M.A.R.T ERROR\n", HostIndex, Port);
      }
      Status = EFI_DEVICE_ERROR;
      *HasError = TRUE;
    }
  }

  FreeAlignedPages(Asb, EFI_SIZE_TO_PAGES(sizeof(EFI_ATA_STATUS_BLOCK)));
  return Status;
}


BOOLEAN 
ShowHddSmartStatus (
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer = NULL;
  UINTN                           Index;
  UINT16                          SataPort;
  UINT16                          SataPortMp; 
  EFI_ATA_PASS_THRU_PROTOCOL      *AtaPassThru;
  PLAT_HOST_INFO_PROTOCOL         *ptHostInfo;
  UINT16                          HostIndex;
  BOOLEAN                         ThisHasError;
  BOOLEAN                         HasError = FALSE;

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiAtaPassThruProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), AtaPassThru HandleCount :%d.\n", __LINE__, HandleCount));

  for (Index = 0; Index < HandleCount; Index++) {
    HostIndex = ptHostInfo->GetSataHostIndex(HandleBuffer[Index]);
    if(HostIndex == 0xFFFF){
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiAtaPassThruProtocolGuid,
                    &AtaPassThru
                    );

    SataPort = 0xFFFF;
    while (TRUE) {
      Status = AtaPassThru->GetNextPort(AtaPassThru, &SataPort);
      if (EFI_ERROR (Status)) {
        break;
      }
      SataPortMp = 0xFFFF;
      while (TRUE) {
        Status = AtaPassThru->GetNextDevice(AtaPassThru, SataPort, &SataPortMp);
        if (EFI_ERROR (Status)) {
          break;
        }

        CheckSmart(AtaPassThru, SataPort, SataPortMp, HandleBuffer[Index], HostIndex, &ThisHasError);
        HasError |= ThisHasError;
      }
    }
  }

ProcExit:
  if (HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }
  return HasError;
}


VOID
ShowBaseBoardInfo (
  EFI_SMBIOS_PROTOCOL *Smbios
  )
{
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  EFI_SMBIOS_TYPE                   SmbiosType; 
  EFI_STATUS                        Status;
  EFI_SMBIOS_TABLE_HEADER           *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER          p;
  CHAR8                             *OemBoardPnString;
  CHAR8	                            *OemProductString;
  CHAR16                            *ProductName = NULL;
  CHAR16                            *BoardPn = NULL;
  UINTN                             Size;
  SETUP_DATA                        SetupData;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  Size = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  ); 

  ProductName = HiiGetString(mHiiHandle, STRING_TOKEN(STR_PRODUCT_NAME), NULL);
  BoardPn     = HiiGetString(mHiiHandle, STRING_TOKEN(STR_BOARD_PN), NULL);
  
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr = SmbiosHdr;
    OemProductString = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type1->ProductName);
    OemBoardPnString = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type2->Version);
  }
  
  if(OemProductString != NULL && OemBoardPnString != NULL){
    if(SetupData.HideBrandLogo == 0){
      TextScreenPrintString(L"%s: %a, %s: %a\n", ProductName, OemProductString, BoardPn, OemBoardPnString);
    }else{
      TextScreenPrintString(L"%s: TBD, %s: TBD\n", ProductName, BoardPn);
    }
  }
  
  FreePool(ProductName);
  FreePool(BoardPn);
}


VOID DisplayTextMessage (VOID)
{
  EFI_STATUS                Status;  
  EFI_SMBIOS_PROTOCOL       *Smbios;
  PLAT_HW_INFO              *PlatHwInfo; 
  UINT32                    SystemMiscCfg;
  BOOLEAN                   HasError = FALSE;
  CHAR16                    *Str;
  PLAT_HOST_INFO_PROTOCOL   *PlatHostInfo;
  UINTN                     EventIndex;
  EFI_INPUT_KEY             Key;
  UINT16                    BdsWaitTime;
  PLATFORM_COMM_INFO        *Info;  

  BOOLEAN                   NeedShowPostInfo;
  UINT16                    WaitSeconds;
  EFI_STRING                WaitSecondsPrompt;
  CHAR16                    *Prompt;
  EFI_STATUS                EventStatus;
  EFI_EVENT                 WaitList[2];


  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), PcdPostTextScreenEnable :%d.\n", __LINE__, PcdGetBool(PcdPostTextScreenEnable)));
  if(!PcdGetBool(PcdPostTextScreenEnable)){
    return;
  }
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);

  Status = gBS->LocateProtocol (
                  &gEfiSmbiosProtocolGuid,
                  NULL,
                  (VOID**)&Smbios
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
	
  Status = gBS->LocateProtocol(&gPlatformHwInfoReadyProtocolGuid, NULL, (VOID**)&PlatHwInfo);
  if (EFI_ERROR (Status)) {
    return;
  }

  NeedShowPostInfo = FALSE;	
  if(mSetupHob->QuietBootMode == 2 || mSetupHob->QuietBootMode == 3) {
    NeedShowPostInfo = TRUE;
  }  
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), NeedShowPostInfo :%d.\n", __LINE__, NeedShowPostInfo));

  if (!NeedShowPostInfo) {
    if (mSetupHob->WaitToShowPostInfo != 0) {
      while (gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) == EFI_SUCCESS) {};   // flush keyboard buffer
      WaitSeconds = mSetupHob->WaitToShowPostInfo;
      DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), WaitSeconds :%d.\n", __LINE__, WaitSeconds));

      WaitSecondsPrompt = HiiGetString(mHiiHandle, STRING_TOKEN(STR_WAIT_F2_TO_CLOSE_QUIET_BOOT), NULL);
      gST->ConOut->ClearScreen (gST->ConOut);
      while (TRUE) {
        Prompt = AllocateZeroPool (StrSize (WaitSecondsPrompt) + sizeof(UINT16)*2);
        UnicodeSPrint(Prompt, StrSize (WaitSecondsPrompt) + sizeof(UINT16)*2, WaitSecondsPrompt, WaitSeconds);
        gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
        gST->ConOut->OutputString (gST->ConOut, Prompt);
        FreePool (Prompt);

        if (WaitSeconds == 0) {
          gBS->Stall (100 * 1000); // delay 0.1s to show 0 second info
          break;
        }

        gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &WaitList[0]);
        gBS->SetTimer (WaitList[0], TimerRelative, 1 * 10 * 1000 * 1000);
        WaitList[1] = gST->ConIn->WaitForKey;
        while (TRUE) {
          EventStatus = gBS->WaitForEvent(ARRAY_SIZE(WaitList), WaitList, &EventIndex);
          if (!EFI_ERROR(EventStatus)) {
            if (EventIndex == 0) {
              break;
            } else if (EventIndex == 1) {
              EventStatus = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
              if (Key.ScanCode == SCAN_F2) {
                NeedShowPostInfo = TRUE;
                break;
              } else {
              }
            }
          }
        }

        if (NeedShowPostInfo) {
          break;
        }

        WaitSeconds--;
      }
      gST->ConOut->ClearScreen (gST->ConOut);
      gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
      FreePool (WaitSecondsPrompt);
    }
  }

  //
  // Show All Message.
  //
  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  ShowBaseBoardInfo(Smbios);
  ShowBiosInfo(Smbios);
  ShowBmcInfo(Info);
  ShowCpuInfo(Smbios);
  ShowMemoryInfo(Smbios);
  //j_t ShowOnBoardLanMac();
  //j_t ShowUsbInfo();
  ShowOsDiskInfo();

  //
  //show HDD S.M.A.R.T status
  //
  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), SystemMiscCfg :0x%x.\n", __LINE__, SystemMiscCfg));
  if(SystemMiscCfg & SYS_MISC_CFG_HDD_SMART_CHECK){
    HasError = ShowHddSmartStatus();
  }
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), HasError :%d.\n", __LINE__, HasError));
	
  if(HasError){
    Str = HiiGetString(mHiiHandle, STRING_TOKEN(STR_KEY_CONTINUE), NULL);
    TextScreenPrintString(L"\n%s\n", Str);
    FreePool(Str);

    Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
    if(!EFI_ERROR(Status)){
      PlatHostInfo->StopWdg();
    }
    
    while (TRUE) {
      Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
      if(EFI_ERROR(Status)){
        break;
      }
      Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
      if(Key.ScanCode == SCAN_NULL && Key.UnicodeChar == CHAR_CARRIAGE_RETURN){
        break;
      }
    }
  } else {
    BdsWaitTime = PcdGet16(PcdPlatformBootTimeOut);
    if(BdsWaitTime < 1){
      gBS->Stall(1000000);
    }
  }

  return;
}



VOID
CleanScreenTextEvent (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  UINTN    Row;
  CHAR16   Space[0x101];
  UINTN    Index;

  gBS->CloseEvent (Event);

  if(!PcdGetBool(PcdPostTextScreenEnable)){
    return;
  }

  for(Index = 0; Index < MaxWidth; Index++ ) {
    Space[Index] = L' ';
  }
  Space[Index] = CHAR_NULL;

  for (Row = 0; Row < EraseLine; Row++){
    gST->ConOut->SetCursorPosition(gST->ConOut, 0, Row);
    gST->ConOut->OutputString(gST->ConOut, Space);
  }
  gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
}


EFI_STATUS
EFIAPI
TextScreenEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS   Status;
  EFI_EVENT    Event;


  mSetupHob = GetSetupDataHobData();
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), QuietBootMode :%d. \n", __LINE__, mSetupHob->QuietBootMode));
		
  mHiiHandle = HiiAddPackages (
            &mTextScreenStringPackGuid,
            ImageHandle,
            TextScreenStrings,
            NULL
            );
  ASSERT(mHiiHandle != NULL);
  
  Status = gBS->InstallMultipleProtocolInterfaces(
              &ImageHandle,
              &gByoBdsWaitHookProtocolGuid, DisplayTextMessage,
              NULL
              );
  DEBUG((EFI_D_INFO,"Install BdsWaitHookProtocol %r\n",Status));

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             CleanScreenTextEvent,
             NULL,
             &Event
             );
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), Craete CleanScreenText :%r.\n", Status));

  return Status;
}


