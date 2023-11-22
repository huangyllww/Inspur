/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  CrbDxe.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include "ProjectDxe.h"
#include <IndustryStandard/Smbios.h>
#include <Library/SetupUiLib.h>
#include <SysMiscCfg.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/Smbios.h>
#include <IndustryStandard/Pci.h>
#include <ByoBootGroup.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Protocol/ByoStatusCodeLevelOverrideProtocol.h>
#include <BoardIdType.h>
#include <Library/SetupUiLib.h>
#include <Library/ByoCommLib.h>


VOID 
ProjectUpdateBootOption (
    VOID                          **BootOptions,
    UINTN                         *BootOptionCount
  );

VOID LegacyLocalHDDBootDisable();

EFI_HII_HANDLE      gHiiHandle;
CONST SETUP_DATA    *gSetupHob;
EFI_BOOT_MODE       gBootMode;


VOID BiosPostFinishToBmc()
{
  switch(PcdGet8(PcdBoardIdType)){
    case TKN_BOARD_ID_TYPE_NHVTB1:
//-   LibHygonSetGpio(mGPIO_DEF_PIN(0, 0, 137), mGPIO_DEF_SET(GPIO_FUNCTION_2, GPIO_OUTPUT_LOW, GPIO_PU_EN));
      break;
  }
}



VOID SetStatusCodeLevelValue()
{
  EFI_STATUS              Status;
  BYO_STATUS_CODE_LEVEL   *StatusCodeLevel;

  Status = gBS->LocateProtocol(&gByoStatusCodeLevelOverrideProtocol, NULL, (VOID**)&StatusCodeLevel);
  if(!EFI_ERROR(Status)){
    if(!gSetupHob->SysDebugMode){
      DEBUG((EFI_D_INFO, "MessageDisabled\n"));
      StatusCodeLevel->MessageDisabled = TRUE;
    }
  }
}


VOID
EFIAPI
ProjectOnReadyToBoot (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  BiosPostFinishToBmc();
  SetStatusCodeLevelValue();
  gBS->CloseEvent(Event);
}


VOID
EFIAPI
ProjectSetupEnterSetupCallback (
  IN EFI_EVENT    Event,
  IN VOID*        Context
  )
{
  EFI_STATUS            Status;
  VOID                  *Interface;

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);
  
  SetStatusCodeLevelValue();
}


STATIC
EFI_STATUS
AddSmbiosRecord (
  IN EFI_SMBIOS_PROTOCOL        *Smbios,
  OUT EFI_SMBIOS_HANDLE         *SmbiosHandle,
  IN EFI_SMBIOS_TABLE_HEADER    *Record
  )
{
  *SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  return Smbios->Add (
                   Smbios,
                   NULL,
                   SmbiosHandle,
                   Record
                   );
}




EFI_STATUS
EFIAPI
 ProjectStopBmcWatchDog (
    VOID
  )
{
  SignalProtocolEvent(gBS, &gEfiBootMenuEnterGuid, TRUE);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
 ProjectResetFrb2WatchDog (
    VOID
  )
{
  SignalProtocolEvent(gBS, &gEfiFrb2WatchDogResetNotifyGuid, TRUE);
  return EFI_SUCCESS;
}



STATIC UINT8 gLegacyBootGroupOrder[] = {
  BM_MENU_TYPE_LEGACY_HDD,
  BM_MENU_TYPE_LEGACY_PXE,
  BM_MENU_TYPE_LEGACY_ODD,
  BM_MENU_TYPE_LEGACY_USB_DISK,
  BM_MENU_TYPE_LEGACY_USB_ODD
};

STATIC UINT8 gUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD,
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_ODD,
  BM_MENU_TYPE_UEFI_USB_DISK,
  BM_MENU_TYPE_UEFI_USB_ODD,
  BM_MENU_TYPE_UEFI_OTHERS
};

VOID SetDefaultBootOrderGroup()
{
  EFI_STATUS    Status;
  UINTN         DataSize;
  UINT8         *GroupOrder;


  DEBUG((EFI_D_INFO, "SetDefaultBootOrderGroup\n"));

  DataSize = 0;
  GroupOrder = NULL;
  Status = gRT->GetVariable (BYO_LEGACY_BOOT_GROUP_VAR_NAME, &gByoGlobalVariableGuid, NULL, &DataSize, GroupOrder);
  if (Status == EFI_NOT_FOUND || DataSize != sizeof(gLegacyBootGroupOrder)) {
    Status = gRT->SetVariable (
                    BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    BYO_BG_ORDER_VAR_ATTR,
                    sizeof(gLegacyBootGroupOrder),
                    gLegacyBootGroupOrder
                    );
    ASSERT(!EFI_ERROR(Status));
  }

  DataSize = 0;
  GroupOrder = NULL;
  Status = gRT->GetVariable (BYO_UEFI_BOOT_GROUP_VAR_NAME, &gByoGlobalVariableGuid, NULL, &DataSize, GroupOrder);
  if (Status == EFI_NOT_FOUND || DataSize != sizeof(gUefiBootGroupOrder)) {
    Status = gRT->SetVariable (
                    BYO_UEFI_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    BYO_BG_ORDER_VAR_ATTR,
                    sizeof(gUefiBootGroupOrder),
                    gUefiBootGroupOrder
                    );
    ASSERT(!EFI_ERROR(Status));
  }
}



void ProjectAfterConnectSequenceHook()
{
  SetDefaultBootOrderGroup();
}



BOOLEAN GetBmcVirtualUUID(EFI_GUID *Uuid)
{
  ZeroMem(Uuid, sizeof(EFI_GUID));
  return TRUE;
}

VOID 
UpdateSmbiosString (
  IN  EFI_SMBIOS_PROTOCOL    *Smbios,
  IN  EFI_SMBIOS_TYPE        Type,
  IN  UINTN                  Offset,
  IN  CHAR8                  *Str
  )
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER      *Record;
  UINTN                        StrNo;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->GetNext (
                    Smbios, 
                    &SmbiosHandle, 
                    &Type, 
                    &Record, 
                    NULL
                    );
  if (EFI_ERROR(Status)) {
    return;
  }

  StrNo = *((UINT8*)Record + Offset);

  Smbios->UpdateString (
          Smbios,
          &SmbiosHandle,
          &StrNo,
          Str
          );
  
}


VOID DoDmiVendorChange(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS              Status;
  CHAR8                   *DefVendorName  = (CHAR8*)PcdGetPtr(PcdDmiVendorChangeVendor);
  CHAR8                   *DefProductName = (CHAR8*)PcdGetPtr(PcdDmiVendorChangeProduct);
  EFI_SMBIOS_HANDLE       SmbiosHandle;
  EFI_SMBIOS_TABLE_HEADER *Record;
  SMBIOS_TABLE_TYPE1      *Type1;
  EFI_GUID                uuid;
  EFI_SMBIOS_TYPE         Type;

  if(GetBmcVirtualUUID(&uuid)){
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Type = EFI_SMBIOS_TYPE_SYSTEM_INFORMATION;
    Status = Smbios->GetNext (
                      Smbios, 
                      &SmbiosHandle, 
                      &Type, 
                      &Record, 
                      NULL
                      );  
    if(!EFI_ERROR(Status)){
      Type1 = (SMBIOS_TABLE_TYPE1*)Record;
      CopyMem(&Type1->Uuid, &uuid, sizeof(EFI_GUID));
    }
  }

  UpdateSmbiosString(Smbios, EFI_SMBIOS_TYPE_SYSTEM_INFORMATION, OFFSET_OF(SMBIOS_TABLE_TYPE1, Manufacturer), DefVendorName);
  UpdateSmbiosString(Smbios, EFI_SMBIOS_TYPE_SYSTEM_INFORMATION, OFFSET_OF(SMBIOS_TABLE_TYPE1, ProductName),  DefProductName);
  UpdateSmbiosString(Smbios, EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION, OFFSET_OF(SMBIOS_TABLE_TYPE2, Manufacturer), DefVendorName);
  UpdateSmbiosString(Smbios, EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION, OFFSET_OF(SMBIOS_TABLE_TYPE2, ProductName),  DefProductName);
  UpdateSmbiosString(Smbios, EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE, OFFSET_OF(SMBIOS_TABLE_TYPE3, Manufacturer),  DefVendorName);
  
}



BOOLEAN IsErrorDiagCouldShow()
{
  if(gBootMode == BOOT_IN_RECOVERY_MODE){
    return FALSE;
  } else {
    return TRUE;
  }
}


VOID ShowCmosBad()
{
  EFI_STATUS                        Status;
  EFI_EVENT                         TimerEvent = NULL;
  UINTN                             EventIndex;
  EFI_EVENT                         WaitList[2];
  EFI_INPUT_KEY                     Key;
  CHAR16                            *Title, *Content1, *Content2, *Reason;
  UINT8                             CmosBadReason;
  EFI_STRING_ID                     StrId;


  if(!IsErrorDiagCouldShow()){
    return;
  }

  CmosBadReason = PcdGet8(PcdCmosBadReason);
  if(CmosBadReason == CMOS_BAD_REASON_NO_VARIABLE){
    return;
  }
  
  if(CmosBadReason == CMOS_BAD_REASON_JUMPER){
    StrId = STRING_TOKEN(STR_CMOS_ERROR_REASON_JUMPER);
  } else if(CmosBadReason == CMOS_BAD_REASON_SW_SET){
    StrId = STRING_TOKEN(STR_CMOS_ERROR_REASON_SW_SET);
  } else if(CmosBadReason == CMOS_BAD_REASON_SW_UPDATE){
    StrId = STRING_TOKEN(STR_CMOS_ERROR_REASON_SW_UPDATE);
  } else {
    StrId = STRING_TOKEN(STR_CMOS_ERROR_REASON_UNKNOWN);
  }

  Title    = HiiGetString(gHiiHandle, STRING_TOKEN(STR_CMOS_ERROR_TITLE), NULL);
  Content1 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_CMOS_ERROR_CONTENT1), NULL);  
  Content2 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_CMOS_ERROR_CONTENT2), NULL); 
  Reason   = HiiGetString(gHiiHandle, StrId, NULL); 
  
  BltSaveAndRetore(gBS, TRUE);
  gST->ConOut->ClearScreen(gST->ConOut);  
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  
  UiConfirmDialog (
    DIALOG_BOX, 
    Title, 
    NULL, 
    TEXT_ALIGIN_LEFT,
    Content1,
    L"",
    Reason,
    L"",
    Content2,
    L"",    
    NULL
    );

  gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);
  gBS->SetTimer (TimerEvent, TimerRelative, EFI_TIMER_PERIOD_SECONDS(15));
  WaitList[0] = TimerEvent;
  WaitList[1] = gST->ConIn->WaitForKey;

  while (TRUE) {
  	Status = gBS->WaitForEvent(ARRAY_SIZE(WaitList), WaitList, &EventIndex);
    if(EFI_ERROR(Status)){
      break;
    }    
    
    if(EventIndex == 1) {
      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key); 
      if(Key.ScanCode == SCAN_NULL && Key.UnicodeChar == CHAR_CARRIAGE_RETURN){
        break;
      }
    } else if(EventIndex == 0){
      break;
    }
  }

  BltSaveAndRetore(gBS, FALSE);

  if(TimerEvent != NULL){
    gBS->CloseEvent(TimerEvent);
  }
  gBS->FreePool(Title);
  gBS->FreePool(Content1);  
  gBS->FreePool(Content2);
  gBS->FreePool(Reason);  
}


void ProjectAfterConnectAllDefConsoleHook()
{
  UINT8  IsCmosBad;
  UINT8  LogoState;

  IsCmosBad = PcdGet8(PcdIsPlatformCmosBad);
  if(IsCmosBad){
    ShowCmosBad();
  }

  if(1){                                                      // ShowOemLogoOnly
    LogoState = PcdGet8(PcdRecordUpdateLogoState);
    if(LogoState != 0x80){
      PcdSet32S(PcdSystemMiscConfig, PcdGet32(PcdSystemMiscConfig) | SYS_MISC_CFG_DIS_SHOW_LOGO);
    }
  }
}



STATIC EFI_STATUS AddSmbiosType39(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                         Status;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  SMBIOS_TABLE_TYPE39                *SmbiosRecord;
  CHAR8                              *DummyStr = " ";
  UINTN                              MyStrSize;
  CHAR8                              *p;
  UINTN                              Index;

  MyStrSize = AsciiStrSize(DummyStr);
  SmbiosRecord = AllocateZeroPool(sizeof(SMBIOS_TABLE_TYPE39) + 7 * MyStrSize + 1);
  ASSERT(SmbiosRecord != NULL);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY;
  SmbiosRecord->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE39);
  SmbiosRecord->Hdr.Handle = 0;
  SmbiosRecord->PowerUnitGroup = 1;
  SmbiosRecord->Location = 1;
  SmbiosRecord->DeviceName = 2;
  SmbiosRecord->Manufacturer = 3;
  SmbiosRecord->SerialNumber = 4;
  SmbiosRecord->AssetTagNumber = 5;
  SmbiosRecord->ModelPartNumber = 6;
  SmbiosRecord->RevisionLevel = 7;
  SmbiosRecord->MaxPowerCapacity = 0x8000;            // unknown
  SmbiosRecord->InputVoltageProbeHandle = 0xFFFF;     // none
  SmbiosRecord->CoolingDeviceHandle = 0xFFFF;         // none
  SmbiosRecord->InputCurrentProbeHandle = 0xFFFF;     // none
  SmbiosRecord->PowerSupplyCharacteristics.PowerSupplyPresent = 1;
  SmbiosRecord->PowerSupplyCharacteristics.InputVoltageRangeSwitch = 2;   // Unknown
  SmbiosRecord->PowerSupplyCharacteristics.PowerSupplyStatus = 3;         // OK
  SmbiosRecord->PowerSupplyCharacteristics.PowerSupplyType = 4;           // Switching

  p = (CHAR8*)(SmbiosRecord + 1);
  for(Index=1;Index<=7;Index++){
    AsciiStrCpyS(p, MyStrSize, DummyStr);
    p += MyStrSize;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord
                     );

  p = LibSmbiosGetStringInTypeByIndex(&SmbiosRecord->Hdr, SmbiosRecord->DeviceName);
  Index = AsciiStrLen(p);
  p[Index-1]++;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord
                     );

  FreePool(SmbiosRecord);
  return Status;
}




STATIC EFI_STATUS AddSmbiosType41(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                         Status = EFI_SUCCESS;
  SMBIOS_TABLE_TYPE41                *SmbiosRecord;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  UINTN                              Index;
  UINTN                              NameSize;
  EFI_DEVICE_PATH_PROTOCOL           *Dp;
  EFI_HANDLE                         DeviceHandle;
  BOOLEAN                            DevPresent;
  UINTN                              Seg, Bus, Dev, Fun;
  EFI_PCI_IO_PROTOCOL                *PciIo;
  PLATFORM_HOST_INFO                 *HostInfo;
  CHAR8                              *Name;
  UINT8                              DeviceType;
  UINT8                              LanInstance  = 0;
  UINT8                              SataInstance = 0;
  UINT8                              Instance;
  PLAT_HOST_INFO_PROTOCOL            *PlatHostInfo;


  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
  ASSERT(!EFI_ERROR(Status));  

  for(Index=0;Index<PlatHostInfo->HostCount;Index++){
    HostInfo = &PlatHostInfo->HostList[Index];

    if(HostInfo->HostType != PLATFORM_HOST_SATA && HostInfo->HostType != PLATFORM_HOST_LAN){
      continue;
    }

    Dp = HostInfo->Dp;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DeviceHandle);
    if(!EFI_ERROR(Status) && IsDevicePathEnd(Dp)){
      DevPresent = TRUE;
    } else {
      DevPresent = FALSE;
    }

    if(HostInfo->HostType == PLATFORM_HOST_SATA){
      Name = "Onboard - SATA";
      DeviceType = OnBoardDeviceExtendedTypeSATAController;
      SataInstance++;
      Instance = SataInstance;
    } else {
      Name = "Onboard - Lan";
      DeviceType = OnBoardDeviceExtendedTypeEthernet;
      LanInstance++;
      Instance = LanInstance;
    }

    NameSize = AsciiStrSize(Name);
    SmbiosRecord = AllocateZeroPool(sizeof(SMBIOS_TABLE_TYPE41) + NameSize + 1);
    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_ONBOARD_DEVICES_EXTENDED_INFORMATION;
    SmbiosRecord->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE41);
    SmbiosRecord->ReferenceDesignation = 1;
    SmbiosRecord->DeviceType = DeviceType;
    if(DevPresent){
      SmbiosRecord->DeviceType |= BIT7;
    }
    SmbiosRecord->DeviceTypeInstance = Instance;

    if(DevPresent){
      Status = gBS->HandleProtocol(DeviceHandle, &gEfiPciIoProtocolGuid, &PciIo);
      if(!EFI_ERROR(Status)){
        PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
      }
    } else {
      Seg = 0xFFFF;
      Bus = 0xFF;
      Dev = 0xFF;
      Fun = 0xFF;
    }

    SmbiosRecord->SegmentGroupNum = (UINT16)Seg;
    SmbiosRecord->BusNum          = (UINT8)Bus;
    SmbiosRecord->DevFuncNum      = (UINT8)(((Dev & 0x1F) << 3) | (Fun & 0x7));
    AsciiStrCpyS((CHAR8 *)(SmbiosRecord + 1), NameSize, Name);
    Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);
    FreePool(SmbiosRecord);

  }

  return Status;
}




VOID
ProjectAfterConnectAllCallback (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
)
{
  EFI_STATUS             Status;
  EFI_SMBIOS_PROTOCOL    *Smbios;
  VOID                   *Interface;


  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);
  DEBUG((EFI_D_INFO, "ProjectAfterConnectAllCallback\n"));

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (!EFI_ERROR(Status)) {
    Status = AddSmbiosType41(Smbios);
    ASSERT(!EFI_ERROR(Status));
    Status = AddSmbiosType39(Smbios);
    ASSERT(!EFI_ERROR(Status));
    if(0){                                                    // DmiVendorChange
      DoDmiVendorChange(Smbios);
    }
  }

  LegacyLocalHDDBootDisable();  
}



VOID ProjectBdsEnterHook()
{
  if(gByoSharedSmmData->NeedSysReset){
    DEBUG((EFI_D_INFO, "reset...\n"));
    gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
  }
}



EFI_STATUS
ProjectDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_EVENT                       Event;
  PLAT_HOST_INFO_PROTOCOL         *PlatHostInfo;
  VOID                            *Registration;
  UINTN                           Index;
  PLATFORM_COMM_INFO              *Info;


  gSetupHob = GetSetupDataHobData();
  gBootMode = GetBootModeHob();
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), gBootMode :%d.\n", __LINE__, gBootMode));
	
  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
  ASSERT(!EFI_ERROR(Status));

  gHiiHandle = HiiAddPackages (
                 &gEfiCallerIdGuid,
                 gImageHandle,
                 STRING_ARRAY_NAME,
                 NULL
                 );
  ASSERT(gHiiHandle != NULL);

  PlatHostInfo->SetupAddOnHandle = gHiiHandle;
  PlatHostInfo->SetupTitleId = STRING_TOKEN(STR_SETUP_TITLE);

  PlatHostInfo->HotKey = gPlatPostHotKeyCtx;
  PlatHostInfo->HotKeyCount = gPlatPostHotKeyCtxCount;
  if(0){                                                      //j_t PostHotKeyDis
    for(Index=0;Index<PlatHostInfo->HotKeyCount;Index++){
      PlatHostInfo->HotKey[Index].Attribute |= HOTKEY_ATTRIBUTE_DISABLED;
    }
    PcdSet16S(PcdPlatformBootTimeOut, 0);
  }

  PlatHostInfo->SataHostCount = 0;
  for(Index=0;Index<PlatHostInfo->HostCount;Index++){
    if(PlatHostInfo->HostList[Index].HostType == PLATFORM_HOST_SATA){
      PlatHostInfo->SataHostCount++;
    }
  }

  PlatHostInfo->StopWdg          = ProjectStopBmcWatchDog;
  PlatHostInfo->ResetFrb2Wdg     = ProjectResetFrb2WatchDog;
  PlatHostInfo->UpdateBootOption = ProjectUpdateBootOption;  

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             ProjectOnReadyToBoot,
             NULL,
             &Event
             );
  ASSERT_EFI_ERROR(Status);

  EfiCreateProtocolNotifyEvent (
    &gBdsAllDriversConnectedProtocolGuid,
    TPL_CALLBACK,
    ProjectAfterConnectAllCallback,
    NULL,
    &Registration
    );

  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    ProjectSetupEnterSetupCallback,
    NULL,
    &Registration
    );

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gPlatAfterConnectSequenceProtocolGuid, ProjectAfterConnectSequenceHook,
                  &gBdsAfterConnectAllDefConsoleProtocolGuid, ProjectAfterConnectAllDefConsoleHook,
                  &gByoBdsEnterHookGuid, ProjectBdsEnterHook,
                  NULL
                  );

  return EFI_SUCCESS;
}


