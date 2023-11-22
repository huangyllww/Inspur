/*++

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:
  Platform configuration setup.

Revision History:


--*/

#include "PlatformSetupDxe.h"
#include <Protocol/DiskInfo.h>
#include <IndustryStandard/Atapi.h>
#include "SetupItemId.h"
#include <SetupVariable.h>
#include <Library/PlatformCommLib.h>
#include <Library/PciLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciHostBridgeResourceAllocation.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/Smbios.h>
#include <Library/BiosIdLib.h>
#include <Protocol/SystemPasswordProtocol.h>
#include <Protocol/BlockIo.h>
#include <Protocol/UsbIo.h>
#include <Library/ByoCommLib.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <Library/DxeServicesLib.h>
#include <Library/PlatformLanguageLib.h>
#include <Library/IpmiBaseLib.h>
#include <HygonHsioInfo.h>
#include <Protocol/BlockIo.h>
#include <Pi/PiDxeCis.h>
#include <Protocol/LegacyBios.h>
#include <BoardIdType.h>
#include <Protocol/PlatHwInfoProtocol.h>
#include <Library/ByoHygonFabricLib.h>
#include <Protocol/HygonVerProtocol.h>


typedef struct {
  UINT8  L1dLineSize;
  UINT8  L1dLinePerTag;
  UINT8  L1dAssociativity;
  UINT8  L1dSizeKB;
} CPUID_80000005_ECX;

typedef struct {
  UINT8  L1iLineSize;
  UINT8  L1iLinePerTag;
  UINT8  L1iAssociativity;
  UINT8  L1iSizeKB;
} CPUID_80000005_EDX;

typedef struct {
  UINT32  L2LineSize:8;
  UINT32  L2LinePerTag:4;
  UINT32  L2Associativity:4;
  UINT32  L2SizeKB:16;
} CPUID_80000006_ECX;

typedef struct {
  UINT32  L3LineSize:8;
  UINT32  L3LinePerTag:4;
  UINT32  L3Associativity:4;
  UINT32  Reserved:2;
  UINT32  L3Size512KB:14;
} CPUID_80000006_EDX;


typedef struct {
  UINTN   CacheSizeKB;
  UINTN   CacheWays;
} SETUP_CPU_CACHE_INFO;

typedef struct {
  SETUP_VOLATILE_DATA           SetupVData;
  PLATFORM_COMM_INFO            *PlatCommInfo;
  HSIO_PCIE_CTX_INFO            *DxioCtxInfo;
  PLAT_HOST_INFO_PROTOCOL       *ptHostInfo;
  SETUP_CPU_CACHE_INFO          L1i;
  SETUP_CPU_CACHE_INFO          L1d;
  SETUP_CPU_CACHE_INFO          L2;
  SETUP_CPU_CACHE_INFO          L3; 
  UINTN                         CpuTdp;
} FORM_INIT_CTX;

STATIC UINT16 gAmdCpuL2L3Associativity[] = {
  0,  1,      2,   0, 
  4,  0,      8,   0, 
  16, 0,      32,  48, 
  64, 96,     128, 0xFF
};

FORM_INIT_CTX gFormInitCtx;

VOID
UpdatePspInfo (
  EFI_HII_HANDLE HiiHandle
  );

VOID UpdatePcieStatusList(EFI_HII_HANDLE HiiHandle);

#define _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED   0x0400
#define ATAPI_DEVICE                            0x8000

STATIC EFI_STRING_ID gSataStrList[] = {
  STRING_TOKEN(STR_SATA_DRIVE0_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE1_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE2_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE3_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE4_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE5_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE6_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE7_VALUE),
/*
  STRING_TOKEN(STR_SATA_DRIVE8_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE9_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE10_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE11_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE12_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE13_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE14_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE15_VALUE),

  STRING_TOKEN(STR_SATA_DRIVE16_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE17_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE18_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE19_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE20_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE21_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE22_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE23_VALUE),

  STRING_TOKEN(STR_SATA_DRIVE24_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE25_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE26_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE27_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE28_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE29_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE30_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE31_VALUE),
*/  
};


CHAR8 *gSataSpeedStr[] = {
  "NoLink",
  "1.5 Gb/s",
  "3 Gb/s",
  "6 Gb/s"
  };

VOID
InitString (
  EFI_HII_HANDLE    HiiHandle,
  EFI_STRING_ID     StrRef,
  CHAR16            *sFormat, ...
  )
{
  STATIC CHAR16 s[1024];
  VA_LIST  Marker;

  VA_START (Marker, sFormat);
  UnicodeVSPrint (s, sizeof (s),  sFormat, Marker);
  VA_END (Marker);

  HiiSetString (HiiHandle, StrRef, s, NULL);
}


EFI_STRING_ID
InitStringWithLang (
  EFI_HII_HANDLE    HiiHandle,
  EFI_STRING_ID     StrRef,
  CHAR8             *Lang,
  CHAR16            *sFormat, ...
  )
{
  STATIC CHAR16 s[1024];
  VA_LIST  Marker;

  VA_START (Marker, sFormat);
  UnicodeVSPrint (s, sizeof (s),  sFormat, Marker);
  VA_END (Marker);

  return HiiSetString (HiiHandle, StrRef, s, Lang);
}



EFI_STRING_ID
SetStringWithString (
  EFI_HII_HANDLE    HiiHandle,
  EFI_STRING_ID     StrId,
  EFI_STRING_ID     RefStrId
  )
{
  EFI_STRING    EnglishString;
  EFI_STRING    String;
  EFI_STRING_ID    NewStrId;
  CHAR8    *LanguageString;
  CHAR8    *LangCode;
  CHAR8    *Lang;


  if (HiiHandle == NULL || StrId == 0 || RefStrId == 0) {
    return 0;
  }

  LanguageString = HiiGetSupportedLanguages(HiiHandle);
  if (LanguageString == NULL) {
    DEBUG((EFI_D_ERROR, "NoSupportLang\n"));
    return 0;
  }

  Lang = AllocatePool(AsciiStrSize(LanguageString));
  if (Lang == NULL) {
    DEBUG((EFI_D_ERROR, "Alloc Err %a\n", LanguageString));
    return 0;
  }

  EnglishString = HiiGetString(HiiHandle, RefStrId, "en-US");
  if (EnglishString == NULL) {
    DEBUG((EFI_D_ERROR, "NoEnglish\n"));
    return 0;
  }

  DEBUG((EFI_D_INFO, "%s %a\n", EnglishString, LanguageString));

  NewStrId = 0;
  LangCode = LanguageString;
  while (*LangCode != 0) {
    GetNextLanguage (&LangCode, Lang);
    if (AsciiStriCmp (Lang, "uqi") == 0) {
      continue;
    }

    String = HiiGetString (HiiHandle, RefStrId, Lang);
    if (String != NULL) {
      NewStrId = HiiSetString (HiiHandle, StrId, String, Lang);
      FreePool (String);
    } else {
      NewStrId = HiiSetString (HiiHandle, StrId, EnglishString, Lang);
    }
  }

  FreePool (LanguageString);
  FreePool (Lang);
  FreePool (EnglishString);

  return StrId;
}





CHAR16 *
GetToken (
  IN  EFI_HII_HANDLE               HiiHandle,
  IN  EFI_STRING_ID                Token
  )
{
  EFI_STRING  String;

  if (HiiHandle == NULL) {
    return NULL;
  }

  String = HiiGetString (HiiHandle, Token, NULL);
  if (String == NULL) {
    String = AllocateCopyPool (sizeof (L"!"), L"!");
    ASSERT (String != NULL);
  }
  return (CHAR16 *) String;
}



CHAR16 *
GetTokenWithLang (
  IN  EFI_HII_HANDLE               HiiHandle,
  IN  EFI_STRING_ID                Token,
  IN  CHAR8                        *Lang
  )
{
  EFI_STRING  String;

  if (HiiHandle == NULL) {
    return NULL;
  }

  String = HiiGetString (HiiHandle, Token, Lang);
  if (String == NULL) {
    String = AllocateCopyPool (sizeof (L"!"), L"!");
    ASSERT (String != NULL);
  }
  return (CHAR16 *) String;
}



/*
0000h or FFFFh = device does not report version
    F  15  Reserved
    F  14  Reserved for ATA/ATAPI-14
    F  13  Reserved for ATA/ATAPI-13
    F  12  Reserved for ATA/ATAPI-12
    F  11  Reserved for ATA/ATAPI-11
    F  10  Reserved for ATA/ATAPI-10
    F  9   Reserved for ATA/ATAPI-9
    F  8   Reserved for ATA/ATAPI-8
    F  7   1 = supports ATA/ATAPI-7
    F  6   1 = supports ATA/ATAPI-6
    F  5   1 = supports ATA/ATAPI-5
    F  4   1 = supports ATA/ATAPI-4
    F  3   Obsolete
    X  2   Obsolete
    X  1   Obsolete
    F  0   Reserved
*/
UINT8 GetAtaMajorVersion(UINT16 Major)
{
  UINT8   Ver = 0;
  UINT8   i;

  if(Major == 0x0000 || Major == 0xFFFF){
    return 0;
  }

  for(i = 14; i > 0; i--){
    if((Major >> i) & 1){
      Ver = i;
      break;
    }
  }

  return Ver;
}


/*
Word 217: Nominal media rotation rate
0000h         Rate not reported
0001h         Non-rotating media (e.g., solid state device)
0002h-0400h   Reserved
0401h-FFFEh   Nominal media rotation rate in rotations per minute (rpm) (e.g., 7200 rpm = 1C20h)
FFFFh         Reserved
*/
UINTN GetAtaMediaRotationRate(ATA_IDENTIFY_DATA *IdentifyData)
{
  UINT16  Rate;

  if(GetAtaMajorVersion(IdentifyData->major_version_no) < 8){
    return 0;
  }

  Rate = IdentifyData->nominal_media_rotation_rate;
  if(Rate == 1){
    return 1;
  } else if(Rate >= 0x401 && Rate <= 0xFFFE){
    return Rate;
  } else {
    return 0;
  }
}




STATIC
EFI_STATUS
GetSataDevName (
  EFI_HANDLE                      SataHandle,
  CHAR8                           *String,
  UINTN                           StrSize,
  UINT8                           *FwVer,
  UINT8                           *SnVer,
  UINTN                           *RotationRate,
  BOOLEAN                         *IsOdd
  )
{
  EFI_STATUS              Status;
  UINT32                  BufferSize;
  EFI_DISK_INFO_PROTOCOL  *DiskInfo;
  ATA_IDENTIFY_DATA       *IdentifyData = NULL;
  UINT64                  NumSectors = 0;
  UINT64                  DriveSizeInBytes = 0;
  UINT32                  DriveSizeInGB = 0;
//UINT32                  NumTenthsOfGB = 0;
  UINT64                  RemainderInBytes = 0;
  UINT8                   ModelNumber[40 + 1];

  DEBUG((EFI_D_INFO, "GetSataDevName\n"));

  Status = gBS->HandleProtocol(
                  SataHandle,
                  &gEfiDiskInfoProtocolGuid,
                  (VOID**)&DiskInfo
                  );
  ASSERT_EFI_ERROR(Status);

  String[0] = 0;

  IdentifyData = AllocatePool(sizeof(ATA_IDENTIFY_DATA));
  if(IdentifyData == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  BufferSize = sizeof(ATA_IDENTIFY_DATA);
  Status = DiskInfo->Identify (
                       DiskInfo,
                       IdentifyData,
                       &BufferSize
                       );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR,"Identify failed!\n"));
    goto ProcExit;;
  }

  CopyMem(ModelNumber, IdentifyData->ModelName, 40);
  SwapWordArray(ModelNumber, 40);
  ModelNumber[40] = 0;
  TrimStr8(ModelNumber);

  CopyMem(FwVer, IdentifyData->FirmwareVer, 8);
  SwapWordArray(FwVer, 8);
  FwVer[8] = 0;
  TrimStr8(FwVer);

  CopyMem(SnVer, IdentifyData->SerialNo, 20);
  SwapWordArray(SnVer, 20);
  SnVer[20] = 0;
  TrimStr8(SnVer);

  *RotationRate = GetAtaMediaRotationRate(IdentifyData);

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
    DriveSizeInGB = (UINT32)DivU64x64Remainder (DriveSizeInBytes, 1000000000, &RemainderInBytes);

    //
    // Convert the Remainder, which is in bytes, to number of tenths of a Decimal GB.
    //
//- NumTenthsOfGB = (UINT32) DivU64x64Remainder (RemainderInBytes, 100000000, NULL);
    AsciiSPrint(String, StrSize, "[%a] %a %dGB", ((*RotationRate == 1) ? "SSD" : "HDD"), ModelNumber, DriveSizeInGB & 0xFFFFFFFE);
    *IsOdd = FALSE;
  } else {
    AsciiSPrint(String, StrSize, "[ODD] %a", ModelNumber);
    *IsOdd = TRUE;
  }

ProcExit:
  if(IdentifyData != NULL){
    FreePool(IdentifyData);
  }
  return Status;
}

UINT8
GetPciRootUid (EFI_HANDLE Handle)
{
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;

  Status = gBS->HandleProtocol(Handle,
                               &gEfiDevicePathProtocolGuid,
                               &DevicePath);
  if(!EFI_ERROR(Status)){
    if(DevicePath->Type == 0x02 &&
       DevicePath->SubType == 0x01 &&
       ((ACPI_HID_DEVICE_PATH*)DevicePath)->HID == EISA_PNP_ID(0x0A03)){
          return (UINT8)((ACPI_HID_DEVICE_PATH*)DevicePath)->UID;
    }
  }
  return 0xff;
}



VOID
UpdateSataPortInfo (
  EFI_HII_HANDLE         HiiHandle
  )
{
  EFI_STATUS                      Status;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer;
  CHAR8                           DevString[128+1];
  CHAR8                           *SpeedString;
  UINT16                          ControllerNum;
  UINTN                           Index;
  UINTN                           PortIndex;
  UINTN                           StrIndex;
  UINT8                           FwVer[8 + 1];
  UINT8                           SnVer[20 + 1];
  UINTN                           RotationRate;
  BOOLEAN                         IsOdd;
  EFI_DEVICE_PATH_PROTOCOL        *Dp;
  EFI_HANDLE                      DeviceHandle;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  UINT32                          PciId;
  UINT32                          Data32;


  DEBUG((EFI_D_INFO, "UpdateSataPortInfo\n"));


  switch(PcdGet8(PcdBoardIdType)){
    case TKN_BOARD_ID_TYPE_NHVTB3:
      InitString(HiiHandle, STRING_TOKEN(STR_SATA_HOST0), L"%a", "MCIO_x8_C0P7_0");
      break;      
  }

  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  DEBUG((EFI_D_INFO, "DiskInfoCount:%d\n", HandleCount));

  for (Index = 0; Index < HandleCount; Index++) {

    DEBUG((EFI_D_INFO, "I:%d\n", Index));

    Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID**)&Dp);
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "NoDP\n"));
      continue;
    }
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DeviceHandle);
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "NoPciIo\n"));
      continue;
    }
    Status = gBS->HandleProtocol(DeviceHandle, &gEfiPciIoProtocolGuid, &PciIo);
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0, 1, &PciId);
    if(PciId != HG_AHCI_PCIID){
      DEBUG((EFI_D_INFO, "NoHygon\n"));
      continue;
    }

    ControllerNum = gFormInitCtx.ptHostInfo->GetSataHostIndex(HandleBuffer[Index]);
    DEBUG((EFI_D_INFO, "ControllerNum:%d\n", ControllerNum));    
    if(ControllerNum == 0xFFFF){
      continue;
    }
    if(ControllerNum == 0x8000){
      ControllerNum = 0;
    }

    PortIndex = gFormInitCtx.ptHostInfo->GetSataPortIndex(HandleBuffer[Index]);
    DEBUG((EFI_D_INFO, "PortIndex:%d\n", PortIndex));  
    
    PciIo->Mem.Read (
                 PciIo,
                 EfiPciIoWidthUint32,
                 5,
                 0x128 + PortIndex * 0x80,
                 1,
                 &Data32
                 );
    Data32 = (Data32>>4) & 0xF;
    if(Data32 < ARRAY_SIZE(gSataSpeedStr)){
      SpeedString = gSataSpeedStr[Data32];
    } else {
      SpeedString = "<OUT OF SPEC>";
    }

    Status = GetSataDevName(HandleBuffer[Index], DevString, sizeof(DevString), FwVer, SnVer, &RotationRate, &IsOdd);
    if(!EFI_ERROR(Status)){
      StrIndex = PortIndex;                  // ControllerNum*8 + PortIndex;
      ASSERT(StrIndex < ARRAY_SIZE(gSataStrList));
      if(StrIndex >= ARRAY_SIZE(gSataStrList)){
        continue;
      }

      if ((RotationRate > 1) && !IsOdd) {
        DEBUG ((EFI_D_INFO, "Port:%d %a FW:%a SN:%a RPM:%d %a\n", StrIndex, DevString, FwVer, SnVer, RotationRate, SpeedString));
        InitStringWithLang (
          HiiHandle,
          gSataStrList[StrIndex],
          "en-US",
          HiiGetString (HiiHandle, STRING_TOKEN (STR_SATA_DRIVER_CONTENT1), "en-US"),
          DevString,
          FwVer,
          SnVer,
          RotationRate,
          SpeedString
          );
        InitStringWithLang (
          HiiHandle,
          gSataStrList[StrIndex],
          "zh-Hans",
          HiiGetString (HiiHandle, STRING_TOKEN (STR_SATA_DRIVER_CONTENT1), "zh-Hans"),
          DevString,
          FwVer,
          SnVer,
          RotationRate,
          SpeedString
          );
      } else {
        DEBUG ((EFI_D_INFO, "Port:%d %a FW:%a SN:%a %a\n", StrIndex, DevString, FwVer, SnVer, SpeedString));
        InitStringWithLang (
          HiiHandle,
          gSataStrList[StrIndex],
          "en-US",
          HiiGetString (HiiHandle, STRING_TOKEN (STR_SATA_DRIVER_CONTENT2), "en-US"),
          DevString,
          FwVer,
          SnVer,
          SpeedString
          );
        InitStringWithLang (
          HiiHandle,
          gSataStrList[StrIndex],
          "zh-Hans",
          HiiGetString (HiiHandle, STRING_TOKEN (STR_SATA_DRIVER_CONTENT2), "zh-Hans"),
          DevString,
          FwVer,
          SnVer,
          SpeedString
          );
      }
    }
  }

ProcExit:
  if (HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }
}



STATIC EFI_STRING_ID gMemoryStrList7000S0[] = {
  STRING_TOKEN(STR_MEM_SLOT0_VALUE),
  STRING_TOKEN(STR_MEM_SLOT1_VALUE),
  STRING_TOKEN(STR_MEM_SLOT2_VALUE),
  STRING_TOKEN(STR_MEM_SLOT3_VALUE),
  STRING_TOKEN(STR_MEM_SLOT4_VALUE),
  STRING_TOKEN(STR_MEM_SLOT5_VALUE),
  STRING_TOKEN(STR_MEM_SLOT6_VALUE),
  STRING_TOKEN(STR_MEM_SLOT7_VALUE),
  STRING_TOKEN(STR_MEM_SLOT8_VALUE),
  STRING_TOKEN(STR_MEM_SLOT9_VALUE),
  STRING_TOKEN(STR_MEM_SLOT10_VALUE),
  STRING_TOKEN(STR_MEM_SLOT11_VALUE),
  STRING_TOKEN(STR_MEM_SLOT12_VALUE),
  STRING_TOKEN(STR_MEM_SLOT13_VALUE),
  STRING_TOKEN(STR_MEM_SLOT14_VALUE),
  STRING_TOKEN(STR_MEM_SLOT15_VALUE),
  STRING_TOKEN(STR_MEM_SLOT16_VALUE),
  STRING_TOKEN(STR_MEM_SLOT17_VALUE),
  STRING_TOKEN(STR_MEM_SLOT18_VALUE),
  STRING_TOKEN(STR_MEM_SLOT19_VALUE),
  STRING_TOKEN(STR_MEM_SLOT20_VALUE),
  STRING_TOKEN(STR_MEM_SLOT21_VALUE),
  STRING_TOKEN(STR_MEM_SLOT22_VALUE),
  STRING_TOKEN(STR_MEM_SLOT23_VALUE),
  STRING_TOKEN(STR_MEM_SLOT24_VALUE),
  STRING_TOKEN(STR_MEM_SLOT25_VALUE),
  STRING_TOKEN(STR_MEM_SLOT26_VALUE),
  STRING_TOKEN(STR_MEM_SLOT27_VALUE),
  STRING_TOKEN(STR_MEM_SLOT28_VALUE),
  STRING_TOKEN(STR_MEM_SLOT29_VALUE),
  STRING_TOKEN(STR_MEM_SLOT30_VALUE),
  STRING_TOKEN(STR_MEM_SLOT31_VALUE),
};

STATIC EFI_STRING_ID gMemoryStrList7000S1[] = {
  STRING_TOKEN(STR_MEM_SLOT32_VALUE),
  STRING_TOKEN(STR_MEM_SLOT33_VALUE),
  STRING_TOKEN(STR_MEM_SLOT34_VALUE),
  STRING_TOKEN(STR_MEM_SLOT35_VALUE),
  STRING_TOKEN(STR_MEM_SLOT36_VALUE),
  STRING_TOKEN(STR_MEM_SLOT37_VALUE),
  STRING_TOKEN(STR_MEM_SLOT38_VALUE),
  STRING_TOKEN(STR_MEM_SLOT39_VALUE),
  STRING_TOKEN(STR_MEM_SLOT40_VALUE),
  STRING_TOKEN(STR_MEM_SLOT41_VALUE),
  STRING_TOKEN(STR_MEM_SLOT42_VALUE),
  STRING_TOKEN(STR_MEM_SLOT43_VALUE),
  STRING_TOKEN(STR_MEM_SLOT44_VALUE),
  STRING_TOKEN(STR_MEM_SLOT45_VALUE),
  STRING_TOKEN(STR_MEM_SLOT46_VALUE),
  STRING_TOKEN(STR_MEM_SLOT47_VALUE),
/*  
  STRING_TOKEN(STR_MEM_SLOT48_VALUE),
  STRING_TOKEN(STR_MEM_SLOT49_VALUE),
  STRING_TOKEN(STR_MEM_SLOT50_VALUE),
  STRING_TOKEN(STR_MEM_SLOT51_VALUE),
  STRING_TOKEN(STR_MEM_SLOT52_VALUE),
  STRING_TOKEN(STR_MEM_SLOT53_VALUE),
  STRING_TOKEN(STR_MEM_SLOT54_VALUE),
  STRING_TOKEN(STR_MEM_SLOT55_VALUE),
  STRING_TOKEN(STR_MEM_SLOT56_VALUE),
  STRING_TOKEN(STR_MEM_SLOT57_VALUE),
  STRING_TOKEN(STR_MEM_SLOT58_VALUE),
  STRING_TOKEN(STR_MEM_SLOT59_VALUE),
  STRING_TOKEN(STR_MEM_SLOT60_VALUE),
  STRING_TOKEN(STR_MEM_SLOT61_VALUE),
  STRING_TOKEN(STR_MEM_SLOT62_VALUE),
  STRING_TOKEN(STR_MEM_SLOT63_VALUE),
*/  
};

STATIC EFI_STRING_ID gMemoryPositionStrList7000S0[] = {
  STRING_TOKEN(STR_MEM_SLOT0_STRING),
  STRING_TOKEN(STR_MEM_SLOT1_STRING),
  STRING_TOKEN(STR_MEM_SLOT2_STRING),
  STRING_TOKEN(STR_MEM_SLOT3_STRING),
  STRING_TOKEN(STR_MEM_SLOT4_STRING),
  STRING_TOKEN(STR_MEM_SLOT5_STRING),
  STRING_TOKEN(STR_MEM_SLOT6_STRING),
  STRING_TOKEN(STR_MEM_SLOT7_STRING),
  STRING_TOKEN(STR_MEM_SLOT8_STRING),
  STRING_TOKEN(STR_MEM_SLOT9_STRING),
  STRING_TOKEN(STR_MEM_SLOT10_STRING),
  STRING_TOKEN(STR_MEM_SLOT11_STRING),
  STRING_TOKEN(STR_MEM_SLOT12_STRING),
  STRING_TOKEN(STR_MEM_SLOT13_STRING),
  STRING_TOKEN(STR_MEM_SLOT14_STRING),
  STRING_TOKEN(STR_MEM_SLOT15_STRING),
  STRING_TOKEN(STR_MEM_SLOT16_STRING),
  STRING_TOKEN(STR_MEM_SLOT17_STRING),
  STRING_TOKEN(STR_MEM_SLOT18_STRING),
  STRING_TOKEN(STR_MEM_SLOT19_STRING),
  STRING_TOKEN(STR_MEM_SLOT20_STRING),
  STRING_TOKEN(STR_MEM_SLOT21_STRING),
  STRING_TOKEN(STR_MEM_SLOT22_STRING),
  STRING_TOKEN(STR_MEM_SLOT23_STRING),
  STRING_TOKEN(STR_MEM_SLOT24_STRING),
  STRING_TOKEN(STR_MEM_SLOT25_STRING),
  STRING_TOKEN(STR_MEM_SLOT26_STRING),
  STRING_TOKEN(STR_MEM_SLOT27_STRING),
  STRING_TOKEN(STR_MEM_SLOT28_STRING),
  STRING_TOKEN(STR_MEM_SLOT29_STRING),
  STRING_TOKEN(STR_MEM_SLOT30_STRING),
  STRING_TOKEN(STR_MEM_SLOT31_STRING),
};

STATIC EFI_STRING_ID gMemoryPositionStrList7000S1[] = {
  STRING_TOKEN(STR_MEM_SLOT32_STRING),
  STRING_TOKEN(STR_MEM_SLOT33_STRING),
  STRING_TOKEN(STR_MEM_SLOT34_STRING),
  STRING_TOKEN(STR_MEM_SLOT35_STRING),
  STRING_TOKEN(STR_MEM_SLOT36_STRING),
  STRING_TOKEN(STR_MEM_SLOT37_STRING),
  STRING_TOKEN(STR_MEM_SLOT38_STRING),
  STRING_TOKEN(STR_MEM_SLOT39_STRING),
  STRING_TOKEN(STR_MEM_SLOT40_STRING),
  STRING_TOKEN(STR_MEM_SLOT41_STRING),
  STRING_TOKEN(STR_MEM_SLOT42_STRING),
  STRING_TOKEN(STR_MEM_SLOT43_STRING),
  STRING_TOKEN(STR_MEM_SLOT44_STRING),
  STRING_TOKEN(STR_MEM_SLOT45_STRING),
  STRING_TOKEN(STR_MEM_SLOT46_STRING),
  STRING_TOKEN(STR_MEM_SLOT47_STRING),
/*  
  STRING_TOKEN(STR_MEM_SLOT48_STRING),
  STRING_TOKEN(STR_MEM_SLOT49_STRING),
  STRING_TOKEN(STR_MEM_SLOT50_STRING),
  STRING_TOKEN(STR_MEM_SLOT51_STRING),
  STRING_TOKEN(STR_MEM_SLOT52_STRING),
  STRING_TOKEN(STR_MEM_SLOT53_STRING),
  STRING_TOKEN(STR_MEM_SLOT54_STRING),
  STRING_TOKEN(STR_MEM_SLOT55_STRING),
  STRING_TOKEN(STR_MEM_SLOT56_STRING),
  STRING_TOKEN(STR_MEM_SLOT57_STRING),
  STRING_TOKEN(STR_MEM_SLOT58_STRING),
  STRING_TOKEN(STR_MEM_SLOT59_STRING),
  STRING_TOKEN(STR_MEM_SLOT60_STRING),
  STRING_TOKEN(STR_MEM_SLOT61_STRING),
  STRING_TOKEN(STR_MEM_SLOT62_STRING),
  STRING_TOKEN(STR_MEM_SLOT63_STRING),
*/  
};




CHAR8  gUnitStr[] = {"MB"};

UINT32 UpdateKBSizeUnit(UINT32 SizeKB, CHAR8 *UnitStr)
{
  UINT32  Size;

  if(SizeKB >= 1024 && (SizeKB % 1024) == 0){
    Size     = SizeKB / 1024;
    *UnitStr = 'M';
  } else {
    Size     = SizeKB;
    *UnitStr = 'K';
  }

  return Size;
}


EFI_STATUS
UpdateMemoryInfo (
  EFI_HII_HANDLE            HiiHandle,
  EFI_SMBIOS_PROTOCOL       *Smbios,  
  PLATFORM_COMM_INFO        *Info
  )
{
  EFI_STATUS                 Status;
  UINTN                      Index;
  EFI_STRING_ID              *MemStrList;
  EFI_STRING_ID              *MemPosStrList;  
  CHAR16                     *MemFmt[2];
  CHAR16                     *Buffer;
  UINTN                      BufferSize = 257*sizeof(CHAR16);
  PLAT_HW_INFO               *PlatHwInfo;  
  PLAT_DIMM_INFO             *DimmInfo;
  CHAR8                      UnitChar;
  UINTN                      Size;
  UINTN                      S0DimmCount = 0;
  UINTN                      S1DimmCount = 0;
  UINTN                      SocketDimmCount;
  UINTN                      i;

	
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  Status = gBS->LocateProtocol(&gPlatformHwInfoReadyProtocolGuid, NULL, (VOID**)&PlatHwInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  MemFmt[0] = GetTokenWithLang(HiiHandle, STRING_TOKEN(STR_MEM_INFO_FMT), "en-US");
  MemFmt[1] = GetTokenWithLang(HiiHandle, STRING_TOKEN(STR_MEM_INFO_FMT), "zh-Hans");  
  Buffer = AllocatePool(BufferSize);
  ASSERT(Buffer != NULL);

  for(Index=0; Index<PlatHwInfo->DimmInfoCount; Index++){

    for(i=0; i<PlatHwInfo->DimmInfoCount; i++){
      if(PlatHwInfo->DimmInfo[i].Index == Index){
        break;
      }
    }
    if(i >= PlatHwInfo->DimmInfoCount){
      continue;
    }
    DimmInfo = &PlatHwInfo->DimmInfo[i];

    DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), BankLocator :%a.\n", __LINE__, DimmInfo->BankLocator));
    if(DimmInfo->BankLocator[0] == 'P' && DimmInfo->BankLocator[1] == '1'){
      MemStrList = gMemoryStrList7000S1;
      MemPosStrList = gMemoryPositionStrList7000S1;
      if(S1DimmCount >= ARRAY_SIZE(gMemoryStrList7000S1)){
        break;
      }
      SocketDimmCount = S1DimmCount;
    } else {
      MemStrList = gMemoryStrList7000S0;
      MemPosStrList = gMemoryPositionStrList7000S0;
      if(S0DimmCount >= ARRAY_SIZE(gMemoryStrList7000S0)){
        break;
      }   
      SocketDimmCount = S0DimmCount;      
    }

    InitString(HiiHandle, MemPosStrList[SocketDimmCount], L"%a", DimmInfo->DeviceLocator);

    if(DimmInfo->Present && !DimmInfo->NoTrained){

      Size = LibUpdateMBSizeUnit(DimmInfo->DimmSizeMB, &UnitChar);
      UnicodeSPrint(
        Buffer,
        BufferSize,
        MemFmt[0],
        DimmInfo->Manufacturer,
        DimmInfo->PartNumber,
        Size,
        UnitChar,
        DimmInfo->Ranks,
        DimmInfo->DevWidth,
        DimmInfo->TypeStr,
        DimmInfo->Ecc ? "Yes" : "No",
        DimmInfo->SerialNumber
        );
      TrimStr16(Buffer);
      InitStringWithLang (
        HiiHandle,
        MemStrList[SocketDimmCount],
        "en-US",
        L"%s",
        Buffer
        );

      UnicodeSPrint(
        Buffer,
        BufferSize,
        MemFmt[1],
        DimmInfo->Manufacturer,
        DimmInfo->PartNumber,
        Size,
        UnitChar,
        DimmInfo->Ranks,
        DimmInfo->DevWidth,
        DimmInfo->TypeStr,
        DimmInfo->Ecc ? "Yes" : "No",
        DimmInfo->SerialNumber
        );
      TrimStr16(Buffer);
      InitStringWithLang (
        HiiHandle,
        MemStrList[SocketDimmCount],
        "zh-Hans",
        L"%s",
        Buffer
        );      

    }else if(DimmInfo->NoTrained) {
      SetStringWithString(HiiHandle, MemStrList[Index], STRING_TOKEN(STR_DIMM_PRESENT_NOT_TRAIN));
    }

    if(MemStrList == gMemoryStrList7000S0){
      S0DimmCount++;
    } else {
      S1DimmCount++;
    }
  }

  gFormInitCtx.SetupVData.Cpu0Dimms = (UINT8)S0DimmCount;
  gFormInitCtx.SetupVData.Cpu1Dimms = (UINT8)S1DimmCount;
  DEBUG((EFI_D_INFO, "S0DimmCount:%d, S1DimmCount:%d\n", S0DimmCount, S1DimmCount));  

  FreePool(MemFmt[0]);
  FreePool(MemFmt[1]);
  FreePool(Buffer);

  Size = LibUpdateMBSizeUnit(PlatHwInfo->DimmTotalSizeMB, &UnitChar);
  InitString(
    HiiHandle,
    STRING_TOKEN(STR_MEMORY_SIZE_VALUE),
    L"%d %cB",
    Size,
    UnitChar
    );

  InitString(
    HiiHandle,
    STRING_TOKEN(STR_MEMORY_SPEED_VALUE),
    L"%d MT/s",
    PlatHwInfo->MemSpeed
    );

  return Status;
}




UINT32
GetCacheSize (
  EFI_SMBIOS_PROTOCOL       *Smbios,
  EFI_SMBIOS_HANDLE         CacheHandle,
  CHAR8                     *Unit
)
{
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER  p;
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT32                    CacheSize = 0;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_CACHE_INFORMATION;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status) || SmbiosHandle == CacheHandle){
       p.Hdr = SmbiosHdr;
      break;
    }
  }
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }

  CacheSize = p.Type7->InstalledSize2;
  if(CacheSize & BIT31){
    CacheSize = CacheSize * 64;
  }

  if(CacheSize >= 1024 && (CacheSize % 1024) == 0){
    CacheSize = CacheSize / 1024;
    *Unit = 'M';
  } else {
    *Unit = 'K';
  }

ProcExit:
  return CacheSize;
}








typedef struct {
  EFI_STRING_ID  Socket;
  EFI_STRING_ID  Voltage;
  EFI_STRING_ID  Version;
  EFI_STRING_ID  Speed;
  EFI_STRING_ID  MaxSpeed;
  EFI_STRING_ID  Tdp;
  EFI_STRING_ID  CoreCount;
  EFI_STRING_ID  EnCoreCount;
  EFI_STRING_ID  MicroCode;
  EFI_STRING_ID  Id;
  EFI_STRING_ID  L1d;
  EFI_STRING_ID  L1i;
  EFI_STRING_ID  L2;
  EFI_STRING_ID  L3;
  EFI_STRING_ID  Sn;
} CPU_INFO_STR_LIST;

CPU_INFO_STR_LIST gCpuInfoCpu0StrList = {
  STRING_TOKEN(STR_SOCKET_1),
  STRING_TOKEN(STR_PROCESSOR_VOLTAGE_VALUE),
  STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE),
  STRING_TOKEN(STR_PROCESSOR_SPEED_VALUE),
  STRING_TOKEN(STR_PROCESSOR_MAX_SPEED_VALUE),
  STRING_TOKEN(STR_PROCESSOR_TDP_VALUE),
  STRING_TOKEN(STR_PROCESSOR_CORE_COUNT_VALUE),
  STRING_TOKEN(STR_PROCESSOR_EN_CORE_COUNT_VALUE),  
  STRING_TOKEN(STR_PROCESSOR_MICROCODE_VALUE),
  STRING_TOKEN(STR_PROCESSOR_ID_VALUE),
  STRING_TOKEN(STR_PROCESSOR_DATA_CACHE_L1_VALUE),
  STRING_TOKEN(STR_PROCESSOR_INSTRUCTION_CACHE_L1_VALUE),
  STRING_TOKEN(STR_PROCESSOR_CACHE_L2_VALUE),
  STRING_TOKEN(STR_PROCESSOR_CACHE_L3_VALUE),
  STRING_TOKEN(STR_PROCESSOR_SN_VALUE)
};

CPU_INFO_STR_LIST gCpuInfoCpu1StrList = {
  STRING_TOKEN(STR_SOCKET_2),
  STRING_TOKEN(STR_PROCESSOR_VOLTAGE_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_SPEED_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_MAX_SPEED_VALUE),  
  STRING_TOKEN(STR_PROCESSOR_TDP_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_CORE_COUNT_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_EN_CORE_COUNT_VALUE_2),    
  STRING_TOKEN(STR_PROCESSOR_MICROCODE_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_ID_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_DATA_CACHE_L1_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_INSTRUCTION_CACHE_L1_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_CACHE_L2_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_CACHE_L3_VALUE_2),
  STRING_TOKEN(STR_PROCESSOR_SN_VALUE_2)  
};


VOID UpdateCpuString (
    EFI_HII_HANDLE            HiiHandle, 
    CPU_INFO_STR_LIST         *StrList,
    EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr
  )
{
  SMBIOS_STRUCTURE_POINTER  p;
  PROCESSOR_SIGNATURE       *CpuSign;
  UINT8                     CpuFamily, CpuModel;
  UINT8                     Volatage;
  BOOLEAN                   SmtEn = gFormInitCtx.PlatCommInfo->CpuSmtMode;
  

  p.Hdr   = SmbiosHdr;
  Volatage = (*(UINT8*)&(p.Type4->Voltage)) & 0x7F;
  CpuSign = (PROCESSOR_SIGNATURE*)&(p.Type4->ProcessorId.Signature);

  InitString (
    HiiHandle,
    StrList->Socket,
    L"%a",
    LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->Socket)
    );

  InitString (
    HiiHandle,
    StrList->Sn,
    L"%a",
    LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->SerialNumber)
    );

  InitString (
    HiiHandle,
    StrList->Voltage,
    L"%d.%dV",
    Volatage/10, Volatage%10
    );
  
  InitString (
    HiiHandle,
    StrList->Version,
    L"%a",
    LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->ProcessorVersion)
    );
  
  InitString (
    HiiHandle,
    StrList->Speed,
    L"%d MHz",
    p.Type4->CurrentSpeed
  );

  InitString (
    HiiHandle,
    StrList->MaxSpeed,
    L"%d MHz",
    p.Type4->MaxSpeed
  );

  if(gFormInitCtx.CpuTdp){
    InitString (
      HiiHandle,
      StrList->Tdp,
      L"%d W",
      gFormInitCtx.CpuTdp
      );
  }

  InitString (
    HiiHandle,
    StrList->CoreCount,
    L"%d Core(s) / %d Thread(s)",
    p.Type4->CoreCount2,
    SmtEn ? p.Type4->CoreCount2 * 2 : p.Type4->CoreCount2
    );

  InitString (
    HiiHandle,
    StrList->EnCoreCount,
    L"%d Core(s) / %d Thread(s)",
    p.Type4->EnabledCoreCount2,
    SmtEn ? p.Type4->EnabledCoreCount2 * 2 : p.Type4->EnabledCoreCount2
    );
  
  if(gFormInitCtx.ptHostInfo->CpuMcVer){
    InitString (
      HiiHandle,
      StrList->MicroCode,
      L"0x%X",
      gFormInitCtx.ptHostInfo->CpuMcVer
    );
  }

  if(CpuSign->ProcessorFamily < 0xF){
    CpuFamily = (UINT8)CpuSign->ProcessorFamily;
    CpuModel  = (UINT8)CpuSign->ProcessorModel;
  } else {
    CpuFamily = (UINT8)(CpuSign->ProcessorFamily + CpuSign->ProcessorXFamily);
    CpuModel  = (UINT8)(CpuSign->ProcessorModel + (CpuSign->ProcessorXModel << 4));
  }
  InitString (
    HiiHandle,
    StrList->Id,
    L"0x%X (Family %d, Model %d, Stepping %d)",
    *(UINT32*)CpuSign, CpuFamily, CpuModel, CpuSign->ProcessorSteppingId
    );

  if(gFormInitCtx.L1d.CacheWays == 0xFF){
    InitString (
      HiiHandle,
      StrList->L1d,
      L"%d KB / Full Way",
      gFormInitCtx.L1d.CacheSizeKB
      );    
  } else {
    InitString (
      HiiHandle,
      StrList->L1d,
      L"%d KB / %d-Way",
      gFormInitCtx.L1d.CacheSizeKB,
      gFormInitCtx.L1d.CacheWays
      );      
  }

  if(gFormInitCtx.L1i.CacheWays == 0xFF){
    InitString (
      HiiHandle,
      StrList->L1i,
      L"%d KB / Full Way",
      gFormInitCtx.L1i.CacheSizeKB
      );    
  } else {
    InitString (
      HiiHandle,
      StrList->L1i,
      L"%d KB / %d-Way",
      gFormInitCtx.L1i.CacheSizeKB,
      gFormInitCtx.L1i.CacheWays
      );      
  }

  if(gFormInitCtx.L2.CacheWays == 0xFF){
    InitString (
      HiiHandle,
      StrList->L2,
      L"%d KB / Full Way",
      gFormInitCtx.L2.CacheSizeKB
      );    
  } else {
    InitString (
      HiiHandle,
      StrList->L2,
      L"%d KB / %d-Way",
      gFormInitCtx.L2.CacheSizeKB,
      gFormInitCtx.L2.CacheWays
      );      
  }

  if(gFormInitCtx.L3.CacheWays == 0xFF){
    InitString (
      HiiHandle,
      StrList->L3,
      L"%d %a / Full Way",
      UpdateKBSizeUnit((UINT32)gFormInitCtx.L3.CacheSizeKB, gUnitStr),
      gUnitStr
      );    
  } else {
    InitString (
      HiiHandle,
      StrList->L3,
      L"%d %a / %d-Way",
      UpdateKBSizeUnit((UINT32)gFormInitCtx.L3.CacheSizeKB, gUnitStr),
      gUnitStr,
      gFormInitCtx.L3.CacheWays
      );      
  }
}


VOID
UpdateCpuInfo (
  EFI_HII_HANDLE            HiiHandle,
  EFI_SMBIOS_PROTOCOL       *Smbios
  )
{
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  EFI_STATUS                Status;

//Socket 0
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  UpdateCpuString (
    HiiHandle, 
    &gCpuInfoCpu0StrList, 
    SmbiosHdr
    );
 
//Socket 1
  if(gFormInitCtx.SetupVData.CpuSockets > 1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(!EFI_ERROR(Status)){
      UpdateCpuString (
        HiiHandle, 
        &gCpuInfoCpu1StrList, 
        SmbiosHdr
        );
    }
  }
}




VOID
InitMain (
  EFI_HII_HANDLE            HiiHandle,
  EFI_SMBIOS_PROTOCOL       *Smbios  
  )
{
  EFI_STATUS                Status;
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER  p;
  CHAR8                     *BiosVer;
  CHAR8                     *BiosDate;
  CHAR8                     *BoardId;
  CHAR8                     *Manufacturer;
  CHAR8                     *SerialNumber;
  EFI_GUID                  *Uuid;
  CHAR8                     *AssetTag = NULL;
  UINT8                     HH = 0, MM = 0;
  UINT8                     ResponseBuff[16];
  UINT8                     ResponseSize;
  EFI_STRING_ID             BmcStatusStrId;
  HYGON_VER_PROTOCOL        *HygonVer;


  DEBUG((EFI_D_INFO, "InitMain\n"));

  ResponseSize = sizeof(ResponseBuff);
  ZeroMem(ResponseBuff,sizeof(ResponseBuff));

  Status = EfiSendCommandToBMC (
  	  	  	  EFI_SM_NETFN_APP,
  	  	  	  EFI_APP_GET_SELFTEST_RESULTS,
  	  	  	  NULL,
  			  0,
  			  (UINT8 *)&ResponseBuff,
  			  (UINT8 *)&ResponseSize
  			  );
  if(!EFI_ERROR(Status)){
    if (ResponseBuff[0] == 0x55 || ResponseBuff[0] == 0x56 ||(ResponseBuff[0] == 0x57 && ResponseBuff[1] == 0x04)) {
      BmcStatusStrId = STRING_TOKEN(STR_BMC_ACTIVE);
    } else {
      BmcStatusStrId = STRING_TOKEN(STR_BMC_DOWN);
    }
    SetStringWithString(HiiHandle, STRING_TOKEN(STR_BMC_FIRMWARE_STATUS_VALUE), BmcStatusStrId);
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  BiosVer = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosVersion);
  BiosDate = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosReleaseDate);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_SYSTEM_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  Manufacturer = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type1->Manufacturer);
  SerialNumber = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type1->SerialNumber);
  Uuid = &p.Type1->Uuid;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  BoardId = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type2->ProductName);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  if(!EFI_ERROR(Status)){
    p.Hdr = SmbiosHdr;
    AssetTag = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type3->AssetTag);
  }
  Status = GetBiosBuildTimeHHMM(&HH, &MM);
  ASSERT(!EFI_ERROR(Status));

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_BOARD_ID_VALUE),
    L"%a",
    BoardId
    );

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_BIOS_DATE_VALUE),
    L"%a %02d:%02d",
    BiosDate,
    HH, MM
    );

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_BIOS_RELEASE_VERSION_VALUE),
    L"%a",
    BiosVer
    );

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_UEFI_SPEC_VER_VALUE),
    L"UEFI %d.%d; PI %d.%d",
    EFI_SPECIFICATION_VERSION >> 16, EFI_SPECIFICATION_VERSION & 0xFFFF, 
    DXE_SPECIFICATION_MAJOR_REVISION, DXE_SPECIFICATION_MINOR_REVISION
    );

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_SMBIOS_VER_VALUE),
    L"%d.%d",
    Smbios->MajorVersion, Smbios->MinorVersion
    );

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_SYSTEM_MANUFACTURER_VALUE),
    L"%a",
    Manufacturer
    );

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_SYSTEM_SERIAL_NUMBER_VALUE),
    L"%a",
    SerialNumber
    );

  if(AssetTag != NULL){
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_ASSET_TAG_VALUE),
      L"%a",
      AssetTag
      );
  }

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_SYSTEM_UUID_VALUE),
    L"%g",
    Uuid
    );

  InitStringWithLang (
    HiiHandle,
    STRING_TOKEN (STR_DATE_HELP),
    "en-US",
    HiiGetString (HiiHandle, STRING_TOKEN (STR_DATE_HELP), "en-US"),
    _PCD_VALUE_PcdMinimalValidYear,
    _PCD_VALUE_PcdMaximalValidYear
    );
  InitStringWithLang (
    HiiHandle,
    STRING_TOKEN (STR_DATE_HELP),
    "zh-Hans",
    HiiGetString (HiiHandle, STRING_TOKEN (STR_DATE_HELP), "zh-Hans"),
    _PCD_VALUE_PcdMinimalValidYear,
    _PCD_VALUE_PcdMaximalValidYear
    );

  Status = gBS->LocateProtocol(&gHygonVersionStringProtocolGuid, NULL, (VOID**)&HygonVer);
  if(!EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "HygonVer:[%a] [%a]\n", HygonVer->Signature, HygonVer->VersionString));
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_AGESA_PI_VER_VALUE),
      L"%a",
      HygonVer->VersionString
      );
  }
}




VOID
UpdateNvmeInfo (
  EFI_HII_HANDLE         HiiHandle
  )
{
  EFI_STATUS                      Status;
  UINTN                           HandleCount;
  EFI_HANDLE                      *Handles;
  BYO_DISKINFO_PROTOCOL           *ByoDiskInfo;
  UINTN                           Index;
  VOID                            *StartOpCodeHandle;
  VOID                            *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL              *StartLabel;
  EFI_IFR_GUID_LABEL              *EndLabel;
  EFI_STRING_ID                   StrRef = 0;
  CHAR8                           Mn[41];
  CHAR8                           Sn[21];
  UINTN                           Size;
  UINT64                          DiskSize;
  CHAR8                           Buffer[256+1];
  UINT32                          DriveSizeInGB;
//UINT32                          NumTenthsOfGB;
  UINT64                          RemainderInBytes;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);
  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = NVME_DEV_LIST_LABEL;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = NVME_DEV_LIST_LABEL_END;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gByoDiskInfoProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gByoDiskInfoProtocolGuid,
                    (VOID **)&ByoDiskInfo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    if(ByoDiskInfo->DevType != BYO_DISK_INFO_DEV_NVME){
      continue;
    }

    Size = sizeof(Mn);
    Status = ByoDiskInfo->GetMn(ByoDiskInfo, Mn, &Size);
    Size = sizeof(Sn);
    Status = ByoDiskInfo->GetSn(ByoDiskInfo, Sn, &Size);
    Status = ByoDiskInfo->GetDiskSize(ByoDiskInfo, &DiskSize);

    DriveSizeInGB = (UINT32) DivU64x64Remainder(DiskSize, 1000000000, &RemainderInBytes);
//- NumTenthsOfGB = (UINT32) DivU64x64Remainder(RemainderInBytes, 100000000, NULL);
    AsciiSPrint(Buffer, sizeof(Buffer), "%a SN:%a Size:%dGB", Mn, Sn, DriveSizeInGB & 0xFFFFFFFE);

    StrRef = InitStringWithLang (
               HiiHandle,
               0,
               "en-US",
               L"%a",
               Buffer
               );

    StrRef = InitStringWithLang(
               HiiHandle,
               StrRef,
               "zh-Hans",
               L"%a",
               Buffer
               );

    HiiCreateTextOpCode (
      StartOpCodeHandle,
      StrRef,
      STRING_TOKEN (STR_EMPTY),
      STRING_TOKEN (STR_EMPTY)
      );

  }

  Status = HiiUpdateForm (HiiHandle, NULL, NVME_FORM_ID, StartOpCodeHandle, EndOpCodeHandle);

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  if (HandleCount != 0) {
    FreePool (Handles);
  }
}




STATIC
EFI_STATUS
FindPciDevIdName(
  UINT8          *FileData,
  UINTN          FileSize,
  UINT16         DevId,
  CHAR8          **Name
  )
{
  UINTN   Index = 0;
  UINTN   i;
  UINT16  Id = 0;
  CHAR8   *p;
  UINTN   n;


  while(Index < FileSize){

    i = Index;
    while(i+1 < FileSize){
      if(FileData[i] == 0xD && FileData[i+1] == 0xA){
        break;
      }
      i++;
    }
    if(FileData[i] == 0xD && i){
      i--;
    }

    if(Index + 7 < FileSize && FileData[Index] == 0x09 && LibIsHexChar(FileData[Index+1]) && LibIsHexChar(FileData[Index+2]) &&
      LibIsHexChar(FileData[Index+3]) && LibIsHexChar(FileData[Index+4]) && FileData[Index+5] == ' ' && FileData[Index+6] == ' '){
      LibChar2Hex2(&FileData[Index+1], &Id);
      if(Id == DevId){
        n = i - Index + 1 - 7;
        if(n > 128){
          n = 128;
        }
        p = AllocatePool(n+1);
        CopyMem(p, &FileData[Index+7], n);
        p[n] = 0;
        *Name = p;
        return EFI_SUCCESS;
      }
    }

    Index = i + 3;

  }

  return EFI_NOT_FOUND;
}


STATIC EFI_STATUS FindPciIdName(UINT32 AddOnGfxId, CHAR8 **GfxName)
{
  EFI_STATUS  Status;
  UINTN       IdIndex;
  UINT8       *Data = NULL;
  UINTN       DataSize = 0;
  CHAR8       *p;
  CHAR8       *VendorName = NULL;
  UINT16      DevId;


  DevId = (UINT16)(AddOnGfxId >> 16);

  IdIndex = 0xFF;
  switch(AddOnGfxId & 0xFFFF){
    case 0x10DE:
      IdIndex = 0;
      VendorName = "Nvidia";
      break;
    case 0x1002:
      IdIndex = 1;
      VendorName = "Amd(ATI)";
      break;
  }

  if(IdIndex != 0xFF){
    Status = GetSectionFromAnyFv (
               &gByoPciIdDataFileGuid,
               EFI_SECTION_RAW,
               IdIndex,
               &Data,
               &DataSize
               );
    if(!EFI_ERROR(Status)){
      Status = FindPciDevIdName(Data, DataSize, DevId, GfxName);
      FreePool(Data);
      if(!EFI_ERROR(Status)){
        return EFI_SUCCESS;
      }
    }
  }

  p = AllocatePool(64);
  if(VendorName != NULL){
    AsciiSPrint(p, 64, "%a GFX(DEVID:%04X)", VendorName, DevId);
  } else {
    AsciiSPrint(p, 64, "Unknown GFX Card(ID:%08X)", AddOnGfxId);
  }

  *GfxName = p;
  return EFI_SUCCESS;
}


VOID
UpdateGfxInfo (
  EFI_HII_HANDLE         HiiHandle
  )
{
  PLAT_HOST_INFO_PROTOCOL   *ptHostInfo;
  EFI_STATUS                Status;
  CHAR8                     *GfxName = NULL;


  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  if(EFI_ERROR(Status) || ptHostInfo->AddOnGfxId == 0xFFFFFFFF){
    goto ProcExit;
  }

  Status = FindPciIdName(ptHostInfo->AddOnGfxId, &GfxName);
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_ADDON_GFX_NAME),
    L"%a",
    GfxName
    );

  FreePool(GfxName);

ProcExit:
  return;
}







STATIC EFI_GUID gSetupBmcCfgFileGuid =
  {0xCEDC062A, 0x84F6, 0x4ffb, {0x8E, 0xD5, 0xA4, 0x3D, 0x8E, 0xBD, 0x5B, 0x50}};

VOID FormInit()
{
  UINTN                         VariableSize;
  EFI_STATUS                    Status;
  VOID                          *Dummy;
  SETUP_VOLATILE_DATA           *p = &gFormInitCtx.SetupVData;
  EFI_HOB_GUID_TYPE             *GuidHob;
  CPUID_80000005_ECX            Ecx5;
  CPUID_80000005_EDX            Edx5;
  CPUID_80000006_ECX            Ecx6;
  CPUID_80000006_EDX            Edx6;  

  
  ZeroMem(&gFormInitCtx, sizeof(FORM_INIT_CTX));
  gFormInitCtx.PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  gFormInitCtx.CpuTdp       = LibGetCpuTdpValue(gBS); 

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &gFormInitCtx.ptHostInfo);
  ASSERT(!EFI_ERROR(Status));

  VariableSize = sizeof(SETUP_VOLATILE_DATA);
  Status = gRT->GetVariable (
                  SETUP_VOLATILE_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  p
                  );
  if(EFI_ERROR(Status)){
    VariableSize = sizeof(SETUP_VOLATILE_DATA);
    ZeroMem(p, VariableSize);
  }

  Status = gBS->LocateProtocol(&gSetupBmcCfgFileGuid, NULL, (VOID**)&Dummy);
  if(!EFI_ERROR(Status)){
    p->BmcPresent = 1;
  } else {
    p->BmcPresent = 0;
  }

  p->CpuSockets = gFormInitCtx.PlatCommInfo->CpuPhySockets;

  switch(PcdGet8(PcdBoardIdType)){
    case TKN_BOARD_ID_TYPE_NHVTB3:
      p->HygonSata0HwPresent[0] = 1;
      p->HygonSata0HwPresent[1] = 1;
      p->HygonSata0HwPresent[2] = 1;
      p->HygonSata0HwPresent[3] = 1;
      break;

    default:
      p->HygonSata0HwPresent[0] = 1;
      p->HygonSata0HwPresent[1] = 1;
      p->HygonSata0HwPresent[2] = 1;
      p->HygonSata0HwPresent[3] = 1;
      p->HygonSata0HwPresent[4] = 1;
      p->HygonSata0HwPresent[5] = 1;
      p->HygonSata0HwPresent[6] = 1;
      p->HygonSata0HwPresent[7] = 1;
      break;      
  }

  GuidHob = GetFirstGuidHob(&gHygonHsioInfoHobGuid);
  if (GuidHob != NULL) {
    gFormInitCtx.DxioCtxInfo = (HSIO_PCIE_CTX_INFO*)GET_GUID_HOB_DATA(GuidHob);
  }  

  AsmCpuid(0x80000005, NULL, NULL, (UINT32*)&Ecx5, (UINT32*)&Edx5);
  AsmCpuid(0x80000006, NULL, NULL, (UINT32*)&Ecx6, (UINT32*)&Edx6);
  gFormInitCtx.L1d.CacheSizeKB = Ecx5.L1dSizeKB;
  gFormInitCtx.L1d.CacheWays   = Ecx5.L1dAssociativity;
  gFormInitCtx.L1i.CacheSizeKB = Edx5.L1iSizeKB;
  gFormInitCtx.L1i.CacheWays   = Edx5.L1iAssociativity;
  gFormInitCtx.L2.CacheSizeKB  = Ecx6.L2SizeKB;
  gFormInitCtx.L2.CacheWays    = gAmdCpuL2L3Associativity[Ecx6.L2Associativity];
  gFormInitCtx.L3.CacheSizeKB  = Edx6.L3Size512KB << 9;
  gFormInitCtx.L3.CacheWays    = gAmdCpuL2L3Associativity[Edx6.L3Associativity];  
  
}



VOID FormInitEnd()
{
  EFI_STATUS       Status;

  Status = gRT->SetVariable (
                  SETUP_VOLATILE_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(SETUP_VOLATILE_DATA),
                  &gFormInitCtx.SetupVData
                  );
  DEBUG((EFI_D_INFO, "SetVar(%s):%r\n", SETUP_VOLATILE_VARIABLE_NAME, Status));
}



STATIC EFI_STRING_ID gPcieSlotStrList[] = {
  STRING_TOKEN(STR_PCIE_SLOT0),
  STRING_TOKEN(STR_PCIE_SLOT1),
  STRING_TOKEN(STR_PCIE_SLOT2),
  STRING_TOKEN(STR_PCIE_SLOT3),
  STRING_TOKEN(STR_PCIE_SLOT4),
  STRING_TOKEN(STR_PCIE_SLOT5),
  STRING_TOKEN(STR_PCIE_SLOT6),
  STRING_TOKEN(STR_PCIE_SLOT7),
  
  STRING_TOKEN(STR_PCIE_SLOT8),
  STRING_TOKEN(STR_PCIE_SLOT9),
  STRING_TOKEN(STR_PCIE_SLOT10),
  STRING_TOKEN(STR_PCIE_SLOT11),
  STRING_TOKEN(STR_PCIE_SLOT12),
  STRING_TOKEN(STR_PCIE_SLOT13),
  STRING_TOKEN(STR_PCIE_SLOT14),
  STRING_TOKEN(STR_PCIE_SLOT15),
  
  STRING_TOKEN(STR_PCIE_SLOT16),
  STRING_TOKEN(STR_PCIE_SLOT17),
  STRING_TOKEN(STR_PCIE_SLOT18),
  STRING_TOKEN(STR_PCIE_SLOT19),
  STRING_TOKEN(STR_PCIE_SLOT20),
  STRING_TOKEN(STR_PCIE_SLOT21),
  STRING_TOKEN(STR_PCIE_SLOT22),
  STRING_TOKEN(STR_PCIE_SLOT23),  
};


VOID
UpdatePcieInfo (
  EFI_HII_HANDLE         HiiHandle
  )
{
  UINTN                         Index;
  UINT16                        SlotCount = 0;
  HSIO_PCIE_CTX                 *DxioPcieCtx;
  UINTN                         j = 0;
  

  if(gFormInitCtx.DxioCtxInfo == NULL){
    return;
  }

  DxioPcieCtx = gFormInitCtx.DxioCtxInfo->Ctx;
  for(Index=0;Index<gFormInitCtx.DxioCtxInfo->CtxCount;Index++){
    if(DxioPcieCtx[Index].EngineType == DXIO_DEV_TYPE_PCIE && 
       DxioPcieCtx[Index].BuildIn != PCIE_BUILD_IN_IGD){

      if(j < ARRAY_SIZE(gPcieSlotStrList)){
        InitString(
          HiiHandle,
          gPcieSlotStrList[j],
          L"%a",
          DxioPcieCtx[Index].Name
          );
        j++;
      }
       
      SlotCount++;
    }
  }

  DEBUG((EFI_D_INFO, "SlotCount:%d(%d)\n", SlotCount, j));
  gFormInitCtx.SetupVData.PcieSlotCount = SlotCount;
}


VOID UpdateLoginType(EFI_HII_HANDLE HiiHandle)
{
  EFI_SYSTEM_PASSWORD_PROTOCOL  *SystemPassword;
  UINT8                         UserType;
  EFI_STATUS                    Status;


  UserType = LOGIN_USER_ADMIN;
  Status = gBS->LocateProtocol (
                  &gEfiSystemPasswordProtocolGuid,
                  NULL,
                  (VOID**)&SystemPassword
                  );
  if(!EFI_ERROR(Status)){
    UserType = SystemPassword->GetEnteredType();
  }
  DEBUG((EFI_D_INFO, "UserType:%d\n", UserType));

  if(UserType == LOGIN_USER_POP){
    SetStringWithString(HiiHandle, STRING_TOKEN(STR_LOGIN_TYPE_VALUE), STRING_TOKEN(STR_LOGIN_TYPE_USER));
  } else {
    SetStringWithString(HiiHandle, STRING_TOKEN(STR_LOGIN_TYPE_VALUE), STRING_TOKEN(STR_LOGIN_TYPE_ADMIN));
  }
}


#define IS_NUM_CHAR(x)    ((x) >= '0' && (x) <= '9')

VOID UpdateCsmDate(EFI_HII_HANDLE HiiHandle)
{
  EFI_STATUS   Status;
  UINT8        *FileData = NULL;
  UINTN        FileSize;
  UINT8        *End;
  UINT8        *p;
  EFI_COMPATIBILITY16_TABLE  *C16Table;
  UINT32                     IbvPointer;
  CHAR8                      *IbvStr;
  UINTN                      Index;
  CHAR8                      DateStr[11];
  

  Status = GetSectionFromAnyFv (
             PcdGetPtr(PcdCsm16File),
             EFI_SECTION_RAW,
             0,
             &FileData,
             &FileSize
             );
  if(EFI_ERROR(Status)){
    return;
  }

  End = FileData + FileSize;
  for (p = FileData; p < End; p += 0x10) {
    if (*(UINT32*)p == EFI_COMPATIBILITY16_TABLE_SIGNATURE) {
      break;
    }
  }
  if(p >= End){
    DEBUG((EFI_D_ERROR, "sign not found\n"));
    goto ProcExit;
  }

  C16Table = (EFI_COMPATIBILITY16_TABLE*)p;
  IbvPointer = (((C16Table->IbvPointer >> 16) & 0xFFFF) << 4) + (C16Table->IbvPointer & 0xFFFF);
  if(IbvPointer >= 0x100000 || IbvPointer < 0xEC000){
    DEBUG((EFI_D_ERROR, "bad IbvPointer:%x\n", IbvPointer));
    goto ProcExit;
  }
  IbvStr = (CHAR8*)(End - (0x100000 - IbvPointer));

  Index = 0;
  while(1){
    if(IbvStr[Index] == 0){
      break;
    }
    Index++;
    if(Index >= 256){
      DEBUG((EFI_D_ERROR, "ibv string is too long\n"));
      goto ProcExit;
    }
  }

  p = AsciiStrStr(IbvStr, "/");
  if(p == NULL){
    DEBUG((EFI_D_ERROR, "\'/\' not found\n"));
    goto ProcExit;
  }
  CopyMem(DateStr, p-2, 10);
  DateStr[10] = 0;

// 10/15/2020
  if(!IS_NUM_CHAR(DateStr[0]) || !IS_NUM_CHAR(DateStr[1]) || DateStr[2] != '/' ||
     !IS_NUM_CHAR(DateStr[3]) || !IS_NUM_CHAR(DateStr[4]) || DateStr[5] != '/' ||
     !IS_NUM_CHAR(DateStr[6]) || !IS_NUM_CHAR(DateStr[7]) || 
     !IS_NUM_CHAR(DateStr[8]) || !IS_NUM_CHAR(DateStr[9])){
    DEBUG((EFI_D_ERROR, "invalid char\n"));
    goto ProcExit;
  }

  InitString(
    HiiHandle,
    STRING_TOKEN(STR_CSM_DATE_VALUE),
    L"%a",
    DateStr
    );

ProcExit:
  if(FileData != NULL){
    FreePool(FileData);
  }
  return;
}


extern const char SSL_version_str[];

VOID UpdateOpenSSLVer(EFI_HII_HANDLE HiiHandle)
{
  InitString(
    HiiHandle,
    STRING_TOKEN(STR_OPENSSL_VER_VALUE),
    L"%a",
    SSL_version_str
    );
}


VOID UpdateAcpiVer(EFI_HII_HANDLE HiiHandle)
{
  EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE  *Fadt;

  Fadt = (EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE*)GetAcpiTableBySign(EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE, gST);
  if(Fadt != NULL){
    if(Fadt->Header.Revision >= 5){
      InitString(
        HiiHandle,
        STRING_TOKEN(STR_INFO_ACPI_VER_VALUE),
        L"%d.%d",
        Fadt->Header.Revision,
        Fadt->MinorVersion
        );
    } else {
      InitString(
        HiiHandle,
        STRING_TOKEN(STR_INFO_ACPI_VER_VALUE),
        L"%d",
        Fadt->Header.Revision
        );
    }
  }
}


STATIC EFI_GUID gHddpasswordDxeModuleFileGuid = {0x2CC4C70B, 0x0312, 0x4cd7, {0xBF, 0xC2, 0xA7, 0xA8, 0x9C, 0x88, 0xC0, 0x8A}};

VOID UpdateModuleVer(EFI_HII_HANDLE HiiHandle)
{
  EFI_STATUS   Status;
  CHAR16       *ModuleVerStr = NULL;
  UINTN        ModuleVerStrSize;
  CHAR16       *p;

  Status = GetSectionFromAnyFv (
             &gHddpasswordDxeModuleFileGuid,
             EFI_SECTION_VERSION,
             0,
             &ModuleVerStr,
             &ModuleVerStrSize
             );
  if(!EFI_ERROR(Status)){
    p = ModuleVerStr;
    p = p + StrLen(p) + 1;
    InitString(
      HiiHandle,
      STRING_TOKEN(STR_HDDPASSWORD_MODULE_VER_VALUE),
      L"%s",
      p
      );    
    FreePool(ModuleVerStr);
  }
}




EFI_STATUS
MainFormInit (
  IN EFI_HII_HANDLE    HiiHandle
  )
{
  EFI_STATUS                Status;
  EFI_SMBIOS_PROTOCOL       *Smbios;  

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if(EFI_ERROR(Status)){
    return Status;
  }

  InitMain(HiiHandle, Smbios);
  UpdateCpuInfo(HiiHandle, Smbios);
  UpdatePspInfo(HiiHandle);  
  UpdateMemoryInfo(HiiHandle, Smbios, gFormInitCtx.PlatCommInfo);
  UpdateLoginType(HiiHandle);
  UpdateCsmDate(HiiHandle);
  UpdateOpenSSLVer(HiiHandle);
  UpdateAcpiVer(HiiHandle);
  UpdateModuleVer(HiiHandle);
  
  return EFI_SUCCESS;
}


EFI_STATUS
AdvanceFormInit (
  IN EFI_HII_HANDLE    HiiHandle
  )
{
  return EFI_SUCCESS;
}


EFI_STATUS
DeviceFormInit (
  IN EFI_HII_HANDLE    HiiHandle
  )
{
  UpdateSataPortInfo(HiiHandle);
  UpdateNvmeInfo(HiiHandle);
  UpdateGfxInfo(HiiHandle);
  UpdatePcieStatusList(HiiHandle);
  UpdatePcieInfo(HiiHandle);  
  return EFI_SUCCESS;
}


