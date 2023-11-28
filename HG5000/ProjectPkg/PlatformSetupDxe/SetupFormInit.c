/*++

Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
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
#include <MemDmi.h>
#include <Library/ByoCommLib.h>
#include <Token.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <Library/DxeServicesLib.h>
#include <Protocol/ByoCustomPstatesProtocol.h>
#include <Protocol/DevicePathToText.h>
#include <Library/PlatformLanguageLib.h>
#include <Library/ByoUefiBootManagerLib.h>

VOID UpdatePcieStatusList(EFI_HII_HANDLE HiiHandle);

#define _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED   0x0400
#define ATAPI_DEVICE                            0x8000

#define MANUFACTURER_STRING_LENGTH              0x30

STATIC EFI_STRING_ID gSataStrList[] = {
  STRING_TOKEN(STR_SATA_DRIVE0_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE1_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE2_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE3_VALUE), 
  STRING_TOKEN(STR_SATA_DRIVE4_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE5_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE6_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE7_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE24_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE25_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE26_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE27_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE28_VALUE),
  STRING_TOKEN(STR_SATA_DRIVE29_VALUE)
};

#define SATA_STR_LIST_COUNT  (sizeof(gSataStrList)/sizeof(gSataStrList[0]))

CHAR8 *gSataSpeedStr[] = {
  "NoLink",
  "1.5 Gb/s", 
  "3 Gb/s", 
  "6 Gb/s",
  "N/A"
  };

UINT8                   gModelNumber[40 + 1];
UINT8                   gFwVer[8 + 1];
UINT8                   gSnVer[20 + 1];
CHAR8                   gRotationRateStr[16+1];
UINT32                  gDriveSizeInGB = 0;

  
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

VOID
AsciiToUnicode (
  IN      CHAR8     *AsciiString,
  IN OUT  CHAR16    *UnicodeString
  )
/*++

Routine Description:

  Converts an ascii string to unicode string.

Arguments:

  AsciiString - ASCII String to Convert
  UnicodeString - Converted Unicode String 

Returns:
  UnicodeString - Converted Unicode String 
--*/
{
  UINT8 Index;

  for (Index = 0; AsciiString[Index] != 0; Index++) {
    UnicodeString[Index] = (CHAR16) AsciiString[Index];
  }

  return ;
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
  UINTN                           StrSize
  )
{
  EFI_STATUS              Status;
  UINT32                  BufferSize;
  EFI_DISK_INFO_PROTOCOL  *DiskInfo;
  ATA_IDENTIFY_DATA       *IdentifyData = NULL;
  UINT64                  NumSectors = 0;  
  UINT64                  DriveSizeInBytes = 0;    
  UINT64                  RemainderInBytes = 0;  
  UINTN                   RotationRate;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
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

  CopyMem(gModelNumber, IdentifyData->ModelName, 40);
  SwapWordArray(gModelNumber, 40);
  gModelNumber[40] = 0;
  TrimStr8(gModelNumber);
  
  CopyMem(gFwVer, IdentifyData->FirmwareVer, 8);
  SwapWordArray(gFwVer, 8);
  gFwVer[8] = 0;
  TrimStr8(gFwVer);
  
  CopyMem(gSnVer, IdentifyData->SerialNo, 20);
  SwapWordArray(gSnVer, 20);  
  gSnVer[20] = 0;
  TrimStr8(gSnVer);  

  RotationRate = GetAtaMediaRotationRate(IdentifyData);
  if(RotationRate == 0){
    AsciiSPrint(gRotationRateStr, sizeof(gRotationRateStr), "N/A");
  } else if(RotationRate == 1){
    AsciiSPrint(gRotationRateStr, sizeof(gRotationRateStr), "0");
  } else {
    AsciiSPrint(gRotationRateStr, sizeof(gRotationRateStr), "%d", RotationRate);    
  }
  
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
    gDriveSizeInGB = (UINT32) DivU64x64Remainder (DriveSizeInBytes, 1000000000, &RemainderInBytes);  
    gDriveSizeInGB &=~1;
    //
    // Convert the Remainder, which is in bytes, to number of tenths of a Decimal GB.
    //
    //NumTenthsOfGB = (UINT32) DivU64x64Remainder (RemainderInBytes, 100000000, NULL); 
 
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
          return ((ACPI_HID_DEVICE_PATH*)DevicePath)->UID;
    }
  }
  return 0xff;
}


#define FCH_SATA_BAR5_REG128                 0x0128  // Port Serial ATA Status
#define FCH_SMN_SATA_CONTROL_BAR5            0x03101000ul



BOOLEAN IsHigonAhci(EFI_HANDLE SataHandle)
{
  EFI_STATUS                      Status;
  EFI_DEVICE_PATH_PROTOCOL        *Dp;
  EFI_HANDLE                      PciHandle;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  UINT32                          PciId;
  
  
  Status = gBS->HandleProtocol(SataHandle, &gEfiDevicePathProtocolGuid, &Dp);
  if(EFI_ERROR(Status)){
    return FALSE;
  }

  Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &PciHandle);
  if(EFI_ERROR(Status)){
    return FALSE;
  }

  Status = gBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, &PciIo);
  if(EFI_ERROR(Status)){
    return FALSE;
  }  

	PciIo->Pci.Read (
             PciIo,
             EfiPciIoWidthUint32,
             0,
             1,
             &PciId
             );
  if(PciId == 0x79011D94){
    DEBUG((EFI_D_INFO, "Higon AHCI\n"));
    return TRUE;
  }

  return FALSE;
}



STATIC 
void
GetSataSpeedStr (
  EFI_HANDLE  SataHandle,
  UINTN       PortNum,
  CHAR8       **SpeedString
)
{
  PLATFORM_COMM_INFO              *Info;
  UINT8                           RootBridgeNum;
  UINT8                           DieBusNum;
  UINT32                          SATA_AHCI_P_SSTS;
  UINT8                           P_SCTL_SPD;
  UINT32                          Offset;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  UINT8                           P_SCTL_SPD1;
  UINT32                          SATA_AHCI_P_SSTS1;
  EFI_STATUS                      Status;
  EFI_DEVICE_PATH_PROTOCOL        *Dp;
  EFI_HANDLE                      PciHandle;
  
  Info = GetPlatformCommInfo();

  if(IsHigonAhci(SataHandle)){
    RootBridgeNum = GetPciRootUid(SataHandle);
    if(RootBridgeNum == 0xFF){
      goto ProcExit;
    }
    
    DieBusNum = Info->BusBase[RootBridgeNum];
    PciWrite32 (PCI_LIB_ADDRESS (DieBusNum, 0, 0, 0xB8), (UINT32)(FCH_SMN_SATA_CONTROL_BAR5 + FCH_SATA_BAR5_REG128 + PortNum * 0x80));
    SATA_AHCI_P_SSTS = PciRead32 (PCI_LIB_ADDRESS (DieBusNum, 0, 0, 0xBC));
    P_SCTL_SPD = (UINT8)((SATA_AHCI_P_SSTS>>4)&0xF);
    *SpeedString = gSataSpeedStr[P_SCTL_SPD];
    
  } else {

      Status = gBS->HandleProtocol(SataHandle, &gEfiDevicePathProtocolGuid, &Dp);
      if(EFI_ERROR(Status)){
        return ;
      }

      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &PciHandle);
      if(EFI_ERROR(Status)){
        return ;
      }

      Status = gBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, &PciIo);
        if(EFI_ERROR(Status)){
        return ;
      }  

    Offset = 0x100 + (UINT8)PortNum * 0x80 + 0x28;

    PciIo->Mem.Read (
               PciIo,
               EfiPciIoWidthUint32,
               0x5,
               (UINT64) Offset,
               1,
               &SATA_AHCI_P_SSTS1
               );
    P_SCTL_SPD1 = (UINT8)((SATA_AHCI_P_SSTS1>>4)&0xF);
    *SpeedString = gSataSpeedStr[P_SCTL_SPD1];
  }

ProcExit:
  return;
}

VOID
UpdateSataPortInfo (
  EFI_HII_HANDLE         HiiHandle
  )
{
  EFI_STATUS                      Status;
  UINTN                           HandleCount;
  EFI_HANDLE                      *HandleBuffer;
  CHAR8                           String[128+1];
  CHAR8                           *SpeedString;
  UINTN                           ControllerNum;
  UINTN                           Index;
  UINTN                           PortIndex;
  UINTN                           RealPortIndex;
  UINTN                           StrIndex;
  EFI_STRING                      USString;
  EFI_STRING                      HanString;


  UINTN                           SataCount = 0;
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));


  
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
    if(!LibGetSataPortInfo(gBS, HandleBuffer[Index], &ControllerNum, &PortIndex, &RealPortIndex) || ControllerNum == 0xFFFF){
      continue;
    }
    ControllerNum &= (UINTN)~BIT15;
    DEBUG((EFI_D_INFO, "ControllerNum:%d, RealPortIndex:%d\n", ControllerNum, RealPortIndex));

    GetSataSpeedStr(HandleBuffer[Index], RealPortIndex, &SpeedString);

    Status = GetSataDevName(HandleBuffer[Index], String, sizeof(String));
    if(!EFI_ERROR(Status)){
      if (ControllerNum < 2) { 
        StrIndex =  RealPortIndex;
		SataCount++;
      } else if (ControllerNum == 3){
        StrIndex = 8 + RealPortIndex;
      } else if (ControllerNum == 2){
        StrIndex = 10 + RealPortIndex;
      } else {
        StrIndex = SATA_STR_LIST_COUNT;
      }

      DEBUG((EFI_D_ERROR, __FUNCTION__"(), StrIndex :%d, String :%a.\n", StrIndex, String));

      if(StrIndex < SATA_STR_LIST_COUNT){
        if (gDriveSizeInGB == 0) {
          USString = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_OTHER_DEVICE_VALUE),"en-US"); 
          InitStringWithLang(
                             HiiHandle,
                             gSataStrList[StrIndex],
                             "en-US",
                             USString,
                             gModelNumber,
                             gFwVer,
                             gRotationRateStr,
                             SpeedString
                             );
        
          HanString = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_OTHER_DEVICE_VALUE),"zh-Hans"); 
          InitStringWithLang(
                             HiiHandle,
                             gSataStrList[StrIndex],
                             "zh-Hans",
                             HanString,
                             gModelNumber,
                             gFwVer,
                             gRotationRateStr,
                             SpeedString
                             );
          
        } else{
          if (AsciiStrCmp(gRotationRateStr, "0") == 0) {
           USString  = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_ATAPI_SSD_DEVICE_VALUE),"en-US"); 
           HanString = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_ATAPI_SSD_DEVICE_VALUE),"zh-Hans"); 
           InitStringWithLang(
                              HiiHandle,
                              gSataStrList[StrIndex],
                              "en-US",
                              USString,
                              L"SSD",
                              gModelNumber,
                              gDriveSizeInGB,
                              gFwVer,
                              SpeedString
                              );
           
           InitStringWithLang(
                              HiiHandle,
                              gSataStrList[StrIndex],
                              "zh-Hans",
                              HanString,
                              L"SSD",
                              gModelNumber,
                              gDriveSizeInGB,
                              gFwVer,
                              SpeedString
                              );
          } else {
           USString  = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_ATAPI_HDD_DEVICE_VALUE),"en-US"); 
           HanString = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_ATAPI_HDD_DEVICE_VALUE),"zh-Hans"); 
           InitStringWithLang(
                              HiiHandle,
                              gSataStrList[StrIndex],
                              "en-US",
                              USString,
                              L"HDD",
                              gModelNumber,
                              gDriveSizeInGB,
                              gFwVer,
                              gRotationRateStr,
                              SpeedString
                              );
           
           InitStringWithLang(
                              HiiHandle,
                              gSataStrList[StrIndex],
                              "zh-Hans",
                              HanString,
                              L"HDD",
                              gModelNumber,
                              gDriveSizeInGB,
                              gFwVer,
                              gRotationRateStr,
                              SpeedString
                              );
          }  
          gDriveSizeInGB = 0;
        }
		
		 if(StrIndex==12){
		 
			DEBUG((EFI_D_INFO,"StrIndex is %d\n",StrIndex));
            USString = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_ASMEDIA_DEVICE_VALUE),"en-US"); 
            InitStringWithLang(
                             HiiHandle,
                             gSataStrList[StrIndex],
                             "en-US",
                             USString,
                             gModelNumber,
                             gFwVer,
                             SpeedString
                             );
        
            HanString = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_ASMEDIA_DEVICE_VALUE),"zh-Hans"); 
            InitStringWithLang(
                             HiiHandle,
                             gSataStrList[StrIndex],
                             "zh-Hans",
                             HanString,
                             gModelNumber,
                             gFwVer,
                             SpeedString
                             );
		 }
		  if(StrIndex==8||StrIndex==9 ||StrIndex==10||StrIndex==11){
		  	
		  InitString(
					 HiiHandle,
					 gSataStrList[StrIndex],
					 L"%a",
					 gModelNumber
					 );

		  }
      }else{
        if(StrIndex == 14){
			
            USString = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_ASMEDIA_DEVICE_VALUE),"en-US"); 
            InitStringWithLang(
                             HiiHandle,
                             gSataStrList[13],
                             "en-US",
                             USString,
                             gModelNumber,
                             gFwVer,
                             SpeedString
                             );
        
            HanString = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_ASMEDIA_DEVICE_VALUE),"zh-Hans"); 
            InitStringWithLang(
                             HiiHandle,
                             gSataStrList[13],
                             "zh-Hans",
                             HanString,
                             gModelNumber,
                             gFwVer,
                             SpeedString
                             );
		}
	  }
    }
    
  }

  InitString(
             HiiHandle,
             STRING_TOKEN(STR_SATA_NUMBER_VALUE),
             L"%d",
             SataCount
            );
  
ProcExit:

  if (HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }

}


STATIC EFI_STRING_ID gMemoryStrList[] = {
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
};

STATIC EFI_STRING_ID gCacheSizeList[] = {
  STRING_TOKEN(STR_PROCESSOR_CACHE_L1_VALUE),
  STRING_TOKEN(STR_PROCESSOR_CACHE_L2_VALUE),
  STRING_TOKEN(STR_PROCESSOR_CACHE_L3_VALUE),
  STRING_TOKEN(STR_PROCESSOR_CACHE_L1_VALUE_1), 
  STRING_TOKEN(STR_PROCESSOR_CACHE_L2_VALUE_1),
  STRING_TOKEN(STR_PROCESSOR_CACHE_L3_VALUE_1),
};

CHAR8  gUnitStr[] = {"MB"};

STATIC CHAR16 *gMemoryManufacturer[] = {
    L"Samsung",
    L"SK Hynix",
    L"Micron Technology",
    L"UniIC",
    L"AgigA Tech",
    L"三星",
    L"海力士",
    L"镁光",
    L"紫光",
    L"AgigA Tech"
};


UINT32 UpdateMBSizeUnit(UINT32 SizeMB, CHAR8 *UnitStr)
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


EFI_STATUS
UpdateMemoryInfo (
  EFI_HII_HANDLE            HiiHandle
  )
{
  EFI_STATUS                 Status = EFI_SUCCESS;
  EFI_SMBIOS_HANDLE          SmbiosHandle;
  EFI_SMBIOS_TYPE            SmbiosType;
  EFI_SMBIOS_TABLE_HEADER    *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER   p;
  UINT32                     SingleSize;
  UINTN                      Index;
  UINTN                      Index1;
  UINTN                      MaxMemorySlot;
  UINT32                     TotalMemSize = 0;
  UINTN                      MemSpeed = 0;
  EFI_SMBIOS_PROTOCOL        *Smbios;
  CHAR8                      *Manufacturer;
  CHAR8                      *OldManufacturer;
  CHAR16                     *UniMafc;
  UINT8                      DimmType;
  UINT8                      DevWidth; 
  CHAR8                      *Dw;  
  CHAR8                      *DimmTypeString[] = {"DDR2","DDR3","DDR4","DDR5","UnKnown"};
  UINT8                      TotalMemCount = 0;
  EFI_STRING                 USString;
  EFI_STRING                 HanString;
  EFI_STRING                 DimmNotTrainingStr;
  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if(EFI_ERROR(Status)){
    return Status;
  } 

  MaxMemorySlot = sizeof(gMemoryStrList)/sizeof(gMemoryStrList[0]);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  Index = 0;

  Manufacturer = AllocatePool(MANUFACTURER_STRING_LENGTH);
  ASSERT(Manufacturer!=NULL);
  UniMafc = AllocatePool(2*MANUFACTURER_STRING_LENGTH);
  ASSERT(UniMafc!=NULL);
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr = SmbiosHdr;

    OldManufacturer = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->Manufacturer);
    if (AsciiStrStr(OldManufacturer,"UniIC")!=NULL){
      AsciiStrCpy(Manufacturer,"UniIC");
    } else {
      AsciiStrCpy(Manufacturer,OldManufacturer);
    }
    DEBUG((EFI_D_INFO, "Mem[%d] Speed:%d(%d)\n", Index, p.Type17->Speed, p.Type17->ConfiguredMemoryClockSpeed));

    SingleSize = p.Type17->Size;
    if(SingleSize == 0 || SingleSize == 0xFFFF){
      SingleSize = 0;
    } else if(SingleSize == 0x7FFF){
      SingleSize = p.Type17->ExtendedSize & (~BIT31);   // MB
    } else {
      if(SingleSize & BIT15){                           // unit in KB.
        SingleSize = (SingleSize&(~BIT15)) >> 10;       // to MB
      }else{                                            // unit in MB.
      }
    }

    TotalMemSize += SingleSize;
    if(SingleSize>0x00){
      TotalMemCount += 1;
    }

    if(SingleSize && Index < MaxMemorySlot){
      if(MemSpeed == 0){
        MemSpeed = p.Type17->ConfiguredMemoryClockSpeed;
      }
      switch(p.Type17->MemoryType){
        case Ddr4MemType:
          DimmType = 2; //DDR4
          break;
        case Ddr3MemType:
          DimmType = 1; //DDR3
          break;
        case Ddr2MemType:
          DimmType = 0; //DDR2
          break;
        default:
          DimmType = 4;     //UnKnown
          break;
      }

      DevWidth = 0;
      Dw = LibSmbiosGetTypeEnd(p.Type17) - 4;
      DEBUG((EFI_D_INFO,"dw IS %a \n",Dw));
      if(Dw[0] == 'W' && Dw[1] >= '0' && Dw[1] <= '9' && Dw[2] >= '0' && Dw[2] <= '9'){
        DevWidth = (Dw[1] - '0') * 10 + (Dw[2] - '0');
      }else{
	  if(AsciiStrStr(OldManufacturer,"UniIC")!=NULL){ 
        DevWidth=PcdGet8(PcdUnilcRank);
      }
	  }
     USString = HiiGetString(HiiHandle,STRING_TOKEN(STR_MEM_SLOT_VALUE),"en-US"); 
     InitStringWithLang(
       HiiHandle,
       gMemoryStrList[Index],
       "en-US",
       USString,
       Manufacturer,
       UpdateMBSizeUnit(SingleSize, &gUnitStr[0]),
       gUnitStr,
       (p.Type17->Attributes)&0xF,
       DevWidth,
       DimmTypeString[DimmType],
       p.Type17->TotalWidth > p.Type17->DataWidth ? "Yes" : "No"
       );

     HanString = HiiGetString(HiiHandle,STRING_TOKEN(STR_MEM_SLOT_VALUE),"zh-Hans"); 
     AsciiStrToUnicodeStr(Manufacturer,UniMafc);
      for (Index1 = 0;Index1 < 5;Index1 ++) {
        if(StrStr(UniMafc,gMemoryManufacturer[Index1]) !=NULL) {
            StrCpy(UniMafc, gMemoryManufacturer[Index1 + 5]);
        }
      }

     InitStringWithLang(
       HiiHandle,
       gMemoryStrList[Index],
       "zh-Hans",
       HanString,
       UniMafc,
       UpdateMBSizeUnit(SingleSize, &gUnitStr[0]),
       gUnitStr,
       (p.Type17->Attributes)&0xF,
       DevWidth,
       DimmTypeString[DimmType],
       p.Type17->TotalWidth > p.Type17->DataWidth ? "Yes" : "No"
      );
    }

    if(p.Type17->TypeDetail.Reserved){ 
      DimmNotTrainingStr=HiiGetString(HiiHandle,STRING_TOKEN(STR_NOT_PASS_TRAINING),"en-US"); 
       InitStringWithLang(
        HiiHandle,
        gMemoryStrList[Index],
        "en-US",
        DimmNotTrainingStr
        );

       DimmNotTrainingStr=HiiGetString(HiiHandle,STRING_TOKEN(STR_NOT_PASS_TRAINING),"zh-Hans"); 
       InitStringWithLang(
        HiiHandle,
        gMemoryStrList[Index],
        "zh-Hans",
        DimmNotTrainingStr
        );
    }
    Index ++;
    ZeroMem(Manufacturer,MANUFACTURER_STRING_LENGTH);
    ZeroMem(UniMafc,2*MANUFACTURER_STRING_LENGTH);
  }

  InitString(
    HiiHandle,
    STRING_TOKEN(STR_MEMORY_COUNT_VALUE), 
    L"%d", 
    TotalMemCount
    );

  InitString(
    HiiHandle,
    STRING_TOKEN(STR_MEMORY_SIZE_VALUE), 
    L"%d %a", 
    UpdateMBSizeUnit(TotalMemSize, &gUnitStr[0]),
    gUnitStr
    );
  
  InitString(
    HiiHandle,
    STRING_TOKEN(STR_MEMORY_SPEED_VALUE), 
    L"%d MHz", 
    MemSpeed
    );

  FreePool(UniMafc);
  FreePool(Manufacturer);

  return Status;
}




UINT32
GetCacheSize (
  EFI_SMBIOS_PROTOCOL       *Smbios,
  EFI_SMBIOS_HANDLE         CacheHandle,
  EFI_HII_HANDLE            HiiHandle,
  UINTN                     Socket,
  UINTN                     CacheIndex
  )
{
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER  p;
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT32                    CacheSize = 0;
  EFI_STRING_ID             CacheStrID;
  UINT16                    Num1 = 0;
  UINT16                    Num2 = 0;

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
  
  if (Socket == 0) {
    CacheStrID = gCacheSizeList[CacheIndex-1];
  } else {
    CacheStrID = gCacheSizeList[CacheIndex-1+3];
  }
  
  if(CacheSize >= 1024 && (CacheSize % 1024) == 0){
    CacheSize = CacheSize / 1024;
    InitString (
    HiiHandle,
    CacheStrID,
    L"%d MB",
    CacheSize
    );
  DEBUG((EFI_D_INFO,"CPU%d Cache L%d:%d MB", Socket, CacheIndex, CacheSize));
  } else {
    Num1 = CacheSize / 1024;
    Num2 = CacheSize % 1024;
    Num2 = Num2 / 10;
    if(Num2>10){
      Num2 = Num2 / 10;
    }
    InitString (
    HiiHandle,
    CacheStrID,
    L"%d.%d MB",
    Num1,
    Num2
    );   
  DEBUG((EFI_D_INFO,"CPU%d Cache L%d:%d.%d MB", Socket, CacheIndex, Num1, Num2));
  } 

ProcExit:
  return CacheSize;
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
  SMBIOS_STRUCTURE_POINTER  p;
  EFI_STATUS                Status;
  CHAR8                     *CpuName;
  CHAR8                     *CpuNameNew = NULL;
  UINT16                    CurSpeed;
  UINT16                    CoreCount;
  UINT16                    ThreadCount;
  UINT32                    CpuId;
  EFI_SMBIOS_HANDLE         L1CacheHandle;
  EFI_SMBIOS_HANDLE         L2CacheHandle;
  EFI_SMBIOS_HANDLE         L3CacheHandle;
  PLAT_HOST_INFO_PROTOCOL   *ptHostInfo;  
  CHAR8                     *SocketName;
  UINT8                     CpuCount = 0;
  CHAR16                    *CpuNotInstall = NULL;
  CHAR16					*CpuNotInstallLang = NULL;

 
  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  ASSERT(!EFI_ERROR(Status));

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  
  CpuNotInstall = GetTokenWithLang (HiiHandle, STRING_TOKEN(STR_NOT_INSTALL_PROCESSOR), "en-US");	
  CpuNotInstallLang = GetTokenWithLang (HiiHandle, STRING_TOKEN(STR_NOT_INSTALL_PROCESSOR), "zh-Hans");
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
  

  p.Hdr         = SmbiosHdr;
  CpuName       = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->ProcessorVersion);
  CurSpeed      = p.Type4->CurrentSpeed;
  CoreCount     = p.Type4->CoreCount;
  ThreadCount   = p.Type4->ThreadCount;
  CpuId         = *(UINT32*)&(p.Type4->ProcessorId.Signature);
  L1CacheHandle = p.Type4->L1CacheHandle;
  L2CacheHandle = p.Type4->L2CacheHandle;
  L3CacheHandle = p.Type4->L3CacheHandle;
  if(CpuId != 0){
    CpuCount++;
  } 
     
  DEBUG((EFI_D_INFO, "CpuName:[%a]\n", CpuName));
  
  CpuNameNew = AllocatePool(AsciiStrSize(CpuName));
  ASSERT(CpuNameNew != NULL);
  AsciiStrCpy(CpuNameNew, CpuName);
  TrimStr8(CpuNameNew);

  SocketName = NULL;
  SocketName = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->Socket);

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_PROCESSOR_NUMBER_VALUE),
    L"%d",
    CpuCount
  );
  if (!AsciiStrCmp(SocketName, PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket0))) {
  	if(CpuId != 0){
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE),
      L"%a", 
      CpuNameNew
    );
	}else {
    InitStringWithLang (
      HiiHandle,
      STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE),
	  "en-US",
      L"%s", 
      CpuNotInstall
    );
	
    InitStringWithLang (
      HiiHandle,
      STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE),
      "zh-Hans",
      L"%s", 
      CpuNotInstallLang
    );	
    }
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_PROCESSOR_SPEED_VALUE),
    L"%d MHz",
    CurSpeed
  );

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_PROCESSOR_CORE_COUNT_VALUE),
    L"%d",
    CoreCount 
  );  

 InitString (
    HiiHandle,
    STRING_TOKEN(STR_PROCESSOR_THREAD_COUNT_VALUE),
    L"%d",
    ThreadCount
  );  

  if(ptHostInfo->CpuMcVer){
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_PROCESSOR_MICROCODE_VALUE),
      L"0x%X",
      ptHostInfo->CpuMcVer
    );
  }
  
  GetCacheSize (Smbios, L1CacheHandle, HiiHandle, 0, 1);
  GetCacheSize (Smbios, L2CacheHandle, HiiHandle, 0, 2);
  GetCacheSize (Smbios, L3CacheHandle, HiiHandle, 0, 3);
 
  }
  
  if (!AsciiStrCmp(SocketName, PcdGetPtr (PcdAmdSmbiosSocketDesignationSocket1))) {
    if (CpuId != 0){
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE_1),
      L"%a", 
      CpuNameNew
    );
	}else {
    InitStringWithLang (
      HiiHandle,
      STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE_1),
	  "en-US",
      L"%s", 
      CpuNotInstall
    );
	
    InitStringWithLang (
      HiiHandle,
      STRING_TOKEN(STR_PROCESSOR_VERSION_VALUE_1),
      "zh-Hans",
      L"%s", 
      CpuNotInstallLang
    );	
    }

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_PROCESSOR_SPEED_VALUE_1),
    L"%d MHz",
    CurSpeed
  );

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_PROCESSOR_CORE_COUNT_VALUE_1),
    L"%d",
    CoreCount
  ); 
  
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_PROCESSOR_THREAD_COUNT_VALUE_1),
    L"%d",
    ThreadCount
  );   

  if(ptHostInfo->CpuMcVer){
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_PROCESSOR_MICROCODE_VALUE_1),
      L"0x%X",
      ptHostInfo->CpuMcVer
    );
  } 
  
  GetCacheSize (Smbios, L1CacheHandle, HiiHandle, 1, 1);
  GetCacheSize (Smbios, L2CacheHandle, HiiHandle, 1, 2);
  GetCacheSize (Smbios, L3CacheHandle, HiiHandle, 1, 3);
  }
  
  if(CpuNameNew != NULL){
    FreePool(CpuNameNew);
   }
  }
}












STATIC EFI_STRING_ID gObLanStrList[] = {
  STRING_TOKEN(STR_LAN_MAC_ADDR_VALUE),
#if TKN_ONBOARD_LAN_MAX_COUNT >= 2
  STRING_TOKEN(STR_LAN2_MAC_ADDR_VALUE),
#endif
};

EFI_STATUS
UpdateOnboardLanMac (
  EFI_HII_HANDLE HiiHandle
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  UINT8                     MacAddr[6];
  PLAT_HOST_INFO_PROTOCOL   *ptHostInfo;  
  UINTN                     Index;
  UINTN                     LanIndex = 0;
  UINTN                     LanCount;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  ASSERT(!EFI_ERROR(Status));

  LanCount = sizeof(gObLanStrList)/sizeof(gObLanStrList[0]);

  for(Index=0;Index<ptHostInfo->HostCount;Index++){
    if(ptHostInfo->HostList[Index].HostType == PLATFORM_HOST_LAN && LanIndex < LanCount){
      DevPath = ptHostInfo->HostList[Index].Dp;
      Status = GetOnboardLanMacAddress(gBS, DevPath, MacAddr);
      if(!EFI_ERROR(Status)){
        InitString(
          HiiHandle,
          gObLanStrList[LanIndex], 
          L"%02X-%02X-%02X-%02X-%02X-%02X", 
          MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]
          );
      }
      LanIndex++;
    }
  }

  return Status;  
}

VOID UpdateBmcVersion(
    EFI_HII_HANDLE	  HiiHandle
	)
{
  UINT32	  BmcVersion;
  UINT8 	  *p;

  BmcVersion = PcdGet32(PcdBmcVersion);
  if(BmcVersion == 0){
	  return;
  }
  
  p = (UINT8*)&BmcVersion;

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_BMC_FIRMWARE_VERSION_VALUE), 
    L"%d.%d.0", 
    p[2], p[1] 
    );
	  
}


VOID UpdateCpldVersion(
    EFI_HII_HANDLE	  HiiHandle
  )
{
  UINT32	  CpldVer;
  UINT8 	  *p;


  CpldVer = PcdGet32(PcdCpldVersion);
  if(CpldVer == 0xFFFFFFFF){
	  return;
  }

  p = (UINT8*)&CpldVer;

  InitString (
    HiiHandle,
    STRING_TOKEN(STR_CPLD_FIRMWARE_VERSION_VALUE), 
    L"%d.%d", 
    p[1], p[0]
    );
}




VOID
InitMain (
  EFI_HII_HANDLE HiiHandle
  )
{
  EFI_STATUS                Status;
  EFI_SMBIOS_PROTOCOL       *Smbios;
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
  UINT16                    UefiVer[3];


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if(EFI_ERROR(Status)){
    return;
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
  BoardId = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type2->Version);

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

#if _PCD_VALUE_PcdTestBiosVersion  
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_BIOS_RELEASE_VERSION_VALUE),
    L"%a(T%02d)",
    BiosVer,
    _PCD_VALUE_PcdTestBiosVersion
    );
#else
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_BIOS_RELEASE_VERSION_VALUE),
    L"%a",
    BiosVer
    );
#endif

  UefiVer[0] = (UINT16)((EFI_SPECIFICATION_VERSION >> 16) & 0xFFFF);
  UefiVer[1] = (UINT16)(EFI_SPECIFICATION_VERSION & 0xFFFF) / 10;
  UefiVer[2] = (UINT16)(EFI_SPECIFICATION_VERSION & 0xFFFF) % 10;
  if(UefiVer[2]){
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_HYGON_PI_VER_VALUE),
      L"PI 1.0.1.1",
      UefiVer[0], UefiVer[1], UefiVer[2]
      );
  } else {
    InitString (
      HiiHandle,
      STRING_TOKEN(STR_HYGON_PI_VER_VALUE),
      L"PI 1.0.1.1",
      UefiVer[0], UefiVer[1]
      );
  }

#if 0
  InitString (
    HiiHandle,
    STRING_TOKEN(STR_BIOS_VENDOR_VALUE), 
    L"%s", 
    gST->FirmwareVendor != NULL ? gST->FirmwareVendor : L"Byosoft"
    );
#endif

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

  UpdateMemoryInfo(HiiHandle);  
  UpdateCpuInfo(HiiHandle, Smbios);
  UpdateOnboardLanMac(HiiHandle); 
  UpdateBmcVersion(HiiHandle);
  UpdateCpldVersion(HiiHandle);
}


/*
CHAR8*
MyGetUsbDescription (
  IN EFI_HANDLE                Handle
  )
{
  EFI_STATUS                   Status;
  CHAR8                        *Description;
  UINT8                        ModelNumber[50];
  USB_BOOT_INQUIRY_DATA        UsbInquiryData;  
  UINT32                       BufferSize = 0;
  EFI_DISK_INFO_PROTOCOL       *DiskInfo;
  

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDiskInfoProtocolGuid,
                  &DiskInfo
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }             

  if(!CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoUsbInterfaceGuid)){
    return NULL;
  }		
	
  BufferSize = sizeof(USB_BOOT_INQUIRY_DATA);  
  Status = DiskInfo->Inquiry(
                     DiskInfo,
                     &UsbInquiryData,
                     &BufferSize
                     );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  
  ZeroMem (ModelNumber, sizeof (ModelNumber));
  CopyMem (ModelNumber, UsbInquiryData.VendorID, 8);
  ModelNumber[8] = ' ';

  CopyMem (&ModelNumber[9], UsbInquiryData.ProductID, 16);
  ModelNumber[25] = '\0';

  Description = AllocateZeroPool(64);
  AsciiSPrint (Description, 64, "%a", ModelNumber);
  TrimStr8(Description);
  
  return Description;
}
*/



CHAR8*
MyGetUsbDescription (
  IN EFI_USB_IO_PROTOCOL  *UsbIo
  )
{
  EFI_STATUS                   Status;
  CHAR16                       *Manufacturer = NULL;
  CHAR16                       *Product      = NULL;
  CHAR8                        *Description;
  EFI_USB_DEVICE_DESCRIPTOR    DevDesc;
  UINTN                        DescMaxSize;
  UINT16                       UsbLangId = 0x0409; // English
  BOOLEAN                      NeedFree1 = FALSE;
  BOOLEAN                      NeedFree2 = FALSE;
  

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));  

  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Status = UsbIo->UsbGetStringDescriptor (
                    UsbIo,
                    UsbLangId,
                    DevDesc.StrManufacturer,
                    &Manufacturer
                    );
  if (EFI_ERROR (Status)) {
    Manufacturer = L"";
  } else {
    NeedFree1 = TRUE;
  }

  Status = UsbIo->UsbGetStringDescriptor (
                    UsbIo,
                    UsbLangId,
                    DevDesc.StrProduct,
                    &Product
                    );
  if (EFI_ERROR (Status)) {
    Product = L"";
  } else {
    NeedFree2 = TRUE;
  }

  if (!NeedFree1 && !NeedFree2) {
    return NULL;
  }

  DescMaxSize = StrLen(Manufacturer) + 1 + StrLen(Product) + 1;
  Description = AllocateZeroPool(DescMaxSize);
  ASSERT (Description != NULL);
  AsciiSPrint(
    Description, 
    DescMaxSize, 
    "%s %s", 
    Manufacturer, 
    Product
    );
  
  TrimStr8(Description);
  if(NeedFree1)FreePool(Manufacturer);
  if(NeedFree2)FreePool(Product);
  return Description;
}









#define CLASS_HID           3
#define SUBCLASS_BOOT       1
#define PROTOCOL_KEYBOARD   1
#define PROTOCOL_MOUSE      2

EFI_STRING_ID gUsbStrList[] = {
  STRING_TOKEN(STR_USB_STORAGE),
  STRING_TOKEN(STR_USB_KEYBOARD),
  STRING_TOKEN(STR_USB_MOUSE)
};

EFI_STRING_ID gUsbPortStrList[] = {
  	STRING_TOKEN(STR_USB_FRONT_PORT1),
	STRING_TOKEN(STR_USB_FRONT_PORT2),
	STRING_TOKEN(STR_USB_BACK_PORT1),
	STRING_TOKEN(STR_USB_BACK_PORT2),
};

EFI_STATUS
GetUsbNameByDiskInfo (
    EFI_HANDLE       UsbIoHandle,
    CHAR8            **UsbName
  )
{
  EFI_DEVICE_PATH_PROTOCOL        *Dp;
  EFI_HANDLE                      UsbHandle;
  USB_BOOT_INQUIRY_DATA           UsbInquiryData;  
  UINT32                          BufferSize = 0;
  EFI_STATUS                      Status;
  EFI_DISK_INFO_PROTOCOL          *DiskInfo;
  CHAR8                           ModelNumber[32];
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;
  UINT64    DriveSizeInBytes, RemainderInBytes;  
  UINT32    DriveSizeInGB = 0;
  CHAR8    NewStr[64];
  UINT32    NumTenthsOfGB = 0;   
  UINT32    Point1 = 0;   

  Status = gBS->HandleProtocol (
                  UsbIoHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**)&Dp
                  );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }      
  
  Status = gBS->LocateDevicePath(&gEfiDiskInfoProtocolGuid, &Dp, &UsbHandle);
  if(EFI_ERROR (Status)){
    DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));    
    return Status;
  }

  Status = gBS->HandleProtocol (
                  UsbHandle,
                  &gEfiDiskInfoProtocolGuid,
                  (VOID**)&DiskInfo
                  );
    
  BufferSize = sizeof(USB_BOOT_INQUIRY_DATA);  
  Status = DiskInfo->Inquiry(
                     DiskInfo,
                     &UsbInquiryData,
                     &BufferSize
                     );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }
  
  ZeroMem(ModelNumber, sizeof(ModelNumber));
  CopyMem(ModelNumber, UsbInquiryData.VendorID, 8);
  ModelNumber[8] = ' ';
  CopyMem(&ModelNumber[9], UsbInquiryData.ProductID, 16);
  ModelNumber[25] = 0;
  TrimStr8(ModelNumber);
  //
  // Get Size.
  //
  Status = gBS->HandleProtocol (
                  UsbHandle,
                  &gEfiBlockIoProtocolGuid,
                  (VOID**)&BlockIo
                  );
  if (!EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), LastBlock :%d.\n", BlockIo->Media->LastBlock));
    DriveSizeInBytes = MultU64x32(BlockIo->Media->LastBlock, BlockIo->Media->BlockSize); 
	
    DriveSizeInGB = (UINT32) DivU64x64Remainder (DriveSizeInBytes, 1024*1024*1024, &RemainderInBytes);  
    NumTenthsOfGB = (UINT32) DivU64x64Remainder (RemainderInBytes, 1024*1024, NULL);
    if (NumTenthsOfGB > 99) {
      Point1 = NumTenthsOfGB % 100;
      NumTenthsOfGB = NumTenthsOfGB/100;
      if (Point1 > 50) {
        NumTenthsOfGB ++;
      }
    } else {
      NumTenthsOfGB = 0;
    }

    ZeroMem(NewStr, sizeof(NewStr));
    AsciiSPrint(NewStr, sizeof (NewStr), "%a %d.%dGB", ModelNumber, DriveSizeInGB, NumTenthsOfGB);
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), NewStr :%a.\n", NewStr));

    *UsbName = AllocateCopyPool(AsciiStrSize(NewStr), NewStr);
    return EFI_SUCCESS;
  }

  *UsbName = AllocateCopyPool(AsciiStrSize(ModelNumber), ModelNumber);
  return EFI_SUCCESS;
}



VOID
UpdateUsbPortInfo (
  EFI_HII_HANDLE         HiiHandle
  )
{
  EFI_STATUS                      Status;
  UINTN                           HandleCount;
  EFI_HANDLE                      *Handles;
  EFI_USB_IO_PROTOCOL             *UsbIo;
  UINTN                           Index;
  UINT8                           UsbType;
  UINT8                           UsbPortNum;
  CHAR8                           *Description;
  EFI_USB_INTERFACE_DESCRIPTOR    Interface;
  VOID                            *StartOpCodeHandle;
  VOID                            *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL              *StartLabel;
  EFI_IFR_GUID_LABEL              *EndLabel;
  EFI_STRING_ID                   StrRef = 0;
  CHAR16                          *UsbString;
  CHAR16                          *UsbPort;

  EFI_DEVICE_PATH_PROTOCOL        *Dp;
  USB_DEVICE_PATH				  *UsbDp;
  ACPI_HID_DEVICE_PATH			  *AcpiUsb;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);
  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = USB_DEV_LIST_LABEL;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = USB_DEV_LIST_LABEL_END;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiUsbIoProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiUsbIoProtocolGuid,
                    (VOID **) &UsbIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &Interface);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if(Interface.InterfaceClass == USB_MASS_STORE_CLASS){
      Status = GetUsbNameByDiskInfo(Handles[Index], &Description);
      if (EFI_ERROR (Status)) {
        continue;
      }      
    } else {
      Description = MyGetUsbDescription(UsbIo);
      if (Description == NULL) {
        continue;
      }
    }
    
    UsbType = 0xff;
    if (Interface.InterfaceClass == USB_MASS_STORE_CLASS) {
      UsbType = 0;
    } else if (Interface.InterfaceClass == CLASS_HID &&
              Interface.InterfaceSubClass == SUBCLASS_BOOT &&
              Interface.InterfaceProtocol == PROTOCOL_KEYBOARD) {
      UsbType = 1;
    } else if (Interface.InterfaceClass == CLASS_HID &&
              Interface.InterfaceSubClass == SUBCLASS_BOOT &&
              Interface.InterfaceProtocol == PROTOCOL_MOUSE) {
      UsbType = 2;
    }

    if(UsbType == 1 || UsbType == 2){
      if((AsciiStrStr(Description, "Virtual") != NULL)||(AsciiStrStr(Description, "Linux") != NULL)){
        DEBUG((EFI_D_INFO, "skip bmc usb kb&ms\n"));
        if (Description != NULL) {
          FreePool (Description);
        }        
        continue;
      }
    }

		
  Status = gBS->HandleProtocol (
                  Handles[Index],
                  &gEfiDevicePathProtocolGuid,
                  (VOID**)&Dp
                  );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
  }  
	if ((Dp->Type == ACPI_DEVICE_PATH) && (Dp->SubType == ACPI_DP)) {
			AcpiUsb = (ACPI_HID_DEVICE_PATH *) Dp;
			DEBUG((EFI_D_ERROR, "ACPI UID %x\n",AcpiUsb->UID));  

			while (!IsDevicePathEndType (Dp)) {
	  			if ((Dp->Type == MESSAGING_DEVICE_PATH) && (Dp->SubType == MSG_USB_DP)) {					
					 UsbDp = (USB_DEVICE_PATH *) Dp;
					 DEBUG((EFI_D_ERROR, "AcpiUsb->UID %x, UsbDp->ParentPortNumber %x\n",AcpiUsb->UID, UsbDp->ParentPortNumber));  
					if ((AcpiUsb->UID == 0)&&((UsbDp->ParentPortNumber == 1)||(UsbDp->ParentPortNumber == 5)))
						UsbPortNum = 0;
					if ((AcpiUsb->UID == 0)&&((UsbDp->ParentPortNumber == 0)||(UsbDp->ParentPortNumber == 4)))
						UsbPortNum = 1;
					if ((AcpiUsb->UID == 1)&&((UsbDp->ParentPortNumber == 1)||(UsbDp->ParentPortNumber == 3)))
						UsbPortNum = 3;
					if ((AcpiUsb->UID == 1)&&((UsbDp->ParentPortNumber == 0)||(UsbDp->ParentPortNumber == 4)))
						UsbPortNum = 2;
					break;
	  				}	
				Dp = NextDevicePathNode (Dp);
	  	}	
	}
	
    if (UsbType != 0xff) {
      UsbString = GetTokenWithLang (HiiHandle, gUsbStrList[UsbType], "en-US");
	  UsbPort = GetTokenWithLang (HiiHandle, gUsbPortStrList[UsbPortNum], "en-US");
      StrRef = InitStringWithLang (
                 HiiHandle,
                 0,
                 "en-US",
                 L"%s: %s  %a",   
	  			 UsbPort,
                 UsbString,
                 Description
                 );
      FreePool (UsbString);
      UsbString = GetTokenWithLang (HiiHandle, gUsbStrList[UsbType], "zh-Hans");
      UsbPort = GetTokenWithLang (HiiHandle, gUsbPortStrList[UsbPortNum], "zh-Hans");  
      StrRef = InitStringWithLang(
                 HiiHandle,
                 StrRef,
                 "zh-Hans",
                 L"%s: %s  %a",          
	  			 UsbPort,
                 UsbString,
                 Description
                 );
      FreePool (UsbString);
      HiiCreateTextOpCode (
        StartOpCodeHandle,
        StrRef,
        STRING_TOKEN (STR_EMPTY),
        STRING_TOKEN (STR_EMPTY)
        );
    }
    if (Description != NULL) {
      FreePool (Description);
    }
  }

  Status = HiiUpdateForm (HiiHandle, NULL, USB_PORT_FORM_ID, StartOpCodeHandle, EndOpCodeHandle);
  
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  if (HandleCount != 0) {
    FreePool (Handles);
  }

  return;
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
  EFI_STRING                      USStr;
  EFI_STRING                      HanStr;
  CHAR8                           Mn[41];
  CHAR8                           Sn[21];
  UINTN                           Size;
  UINT64                          DiskSize;
  UINT32                          DriveSizeInGB;
  UINT64                          RemainderInBytes;  
  EFI_STRING_ID                   StrRef = 0;

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
    DriveSizeInGB &=~1;
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), DriveSizeInGB :%d.\n", DriveSizeInGB));

    USStr = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_NVME_DEVICE_VALUE),"en-US"); 
    StrRef = InitStringWithLang(
          HiiHandle,
          0,
          "en-US",
          USStr,
          Mn,
          Sn,
          DriveSizeInGB
          );

    HanStr = HiiGetString(HiiHandle,STRING_TOKEN(STR_SATA_NVME_DEVICE_VALUE),"zh-Hans"); 
    InitStringWithLang(
          HiiHandle,
          StrRef,
          "zh-Hans",
          HanStr,
          Mn,
          Sn,
          DriveSizeInGB
          );
          
    HiiCreateTextOpCode (
      StartOpCodeHandle,
      StrRef,
      STRING_TOKEN (STR_EMPTY),
      STRING_TOKEN (STR_EMPTY)
      );
      
  }

  Status = HiiUpdateForm (HiiHandle, NULL, CHIPSET_SATA_FORM_ID, StartOpCodeHandle, EndOpCodeHandle);
  
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


VOID
UpdateSetupVData (
    VOID
  )
{
  EFI_STATUS                    Status;
  SETUP_VOLATILE_DATA           SetupVData;
  UINTN                         VariableSize;
  BYO_SYS_CFG_STS_DATA          *SysCfg;
  VOID                          *Dummy;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  SysCfg = (BYO_SYS_CFG_STS_DATA*)GetByoSysAndStsHobData();

  VariableSize = sizeof(SetupVData);
  Status = gRT->GetVariable (
                  SETUP_VOLATILE_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupVData
                  );
  if(EFI_ERROR(Status)){
    VariableSize = sizeof(SetupVData);
    ZeroMem(&SetupVData, VariableSize);
  }

  Status = gBS->LocateProtocol(&gSetupBmcCfgFileGuid, NULL, (VOID**)&Dummy);
  if(!EFI_ERROR(Status)){
    SetupVData.BmcPresent = 1;
  } else {
    SetupVData.BmcPresent = 0;
  }

  SetupVData.Riser1SlotCount = SysCfg->Riser1SlotCount;
  SetupVData.Riser2SlotCount = SysCfg->Riser2SlotCount;  
  SetupVData.Riser3SlotCount = SysCfg->Riser3SlotCount;

  Status = gRT->SetVariable (
                  SETUP_VOLATILE_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  VariableSize,
                  &SetupVData
                  );
  DEBUG((EFI_D_INFO, "SetVar(%s):%r\n", SETUP_VOLATILE_VARIABLE_NAME, Status));
}


VOID
UpdatePspInfo (
  EFI_HII_HANDLE HiiHandle
  );

EFI_STATUS
MainFormInit (
  IN EFI_HII_HANDLE    HiiHandle
  )
{
  InitMain(HiiHandle);
  UpdateSetupVData();
  UpdatePcieStatusList(HiiHandle);
  UpdatePspInfo (HiiHandle);

  return EFI_SUCCESS;
}

STATIC EFI_STRING_ID gCpuSpeedList[] = {
  STRING_TOKEN(STR_SPEED1_VALUE),  
  STRING_TOKEN(STR_SPEED2_VALUE),
  STRING_TOKEN(STR_SPEED3_VALUE),    
};
 
EFI_STATUS
AdvanceFormInit (
  IN EFI_HII_HANDLE    HiiHandle
  )
{
  BYO_CUSTOM_PSTATE_PROTOCOL  *ByoPState;
  EFI_STATUS                  Status;
  UINTN                       Index;
  

  Status = gBS->LocateProtocol(&gByoCustomPStateProtocolGuid, NULL, (VOID**)&ByoPState);
  if(!EFI_ERROR(Status) && ByoPState->CpuPxSpeedCount >= ARRAY_SIZE(gCpuSpeedList)){
    for(Index=0;Index<ARRAY_SIZE(gCpuSpeedList);Index++){
      InitString(
        HiiHandle,
        gCpuSpeedList[Index], 
        L"%d MHz", 
        ByoPState->CpuPxSpeed[Index]
        );
    }
  }

  UpdateSataPortInfo(HiiHandle);
  UpdateUsbPortInfo(HiiHandle);
  UpdateNvmeInfo(HiiHandle);
  UpdateGfxInfo(HiiHandle);
  return EFI_SUCCESS;  
}


EFI_STATUS
DeviceFormInit (
  IN EFI_HII_HANDLE    HiiHandle
  )
{
  UpdateSataPortInfo(HiiHandle);
  UpdateUsbPortInfo(HiiHandle);
  UpdateNvmeInfo(HiiHandle);
  UpdateGfxInfo(HiiHandle);
  //UpdatePcieStatusList(HiiHandle);
  return EFI_SUCCESS;  
}


