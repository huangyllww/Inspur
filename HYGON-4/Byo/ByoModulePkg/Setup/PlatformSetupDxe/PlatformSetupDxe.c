/*++

Copyright (c) 2010 - 2018, Byosoft Corporation.<BR>
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
#include "SetupCallback.h"
#include <Library/UefiHiiServicesLib.h>
#include "FormsetConfiguration.h"
#include <Protocol/SystemPasswordProtocol.h>
#include <Protocol/SetupSaveNotify.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <ByoBootGroup.h>
#include <Library/PcdLib.h>
#include  "SystemPasswordVariable.h"
#include <Protocol/ByoSmiFlashLockProtocol.h>
#include <Guid/SystemMiscSetting.h>
#include <Protocol/UsbPolicy.h>
#include <Guid/PxeControlVariable.h>
#include <Guid/GlobalVariable.h>
#include <Library/ByoBootManagerLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/PrintLib.h>

STATIC SETUP_SAVE_NOTIFY_PROTOCOL  gSetupSaveNotify;
BOOLEAN        mNubLockEn;

STATIC EFI_USB_POLICY_PROTOCOL gUsbPolicy;

SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL mSetupItemNotify;

UINT8 mDefaultUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD,
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_ODD,
  BM_MENU_TYPE_UEFI_USB_DISK,
  BM_MENU_TYPE_UEFI_USB_ODD,
  BM_MENU_TYPE_UEFI_OTHERS,
  };

FORMSET_INFO    gSetupFormSets[] = {
//  {FORMSET_MAIN,       FORMSET_GUID_MAIN,       FormsetMainBin,     PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
  {FORMSET_ADVANCE,    FORMSET_GUID_ADVANCE,    FormsetAdvancedBin, PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
  {FORMSET_SECURITY,   FORMSET_GUID_SECURITY,   FormsetSecurityBin, PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
//  {FORMSET_POWER,      FORMSET_GUID_POWER,      FormsetPowerBin,    PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
  {FORMSET_BOOT,       FORMSET_GUID_BOOT,       FormsetBootBin,     PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
  {FORMSET_EXIT,       FORMSET_GUID_EXIT,       FormsetExitBin,     PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
};

FORM_CALLBACK_ITEM  gFormCallback[] = {
  {FORMSET_EXIT,      EXIT_KEY_SAVE,                  ExitFormCallback},
  {FORMSET_EXIT,      EXIT_KEY_DISCARD,               ExitFormCallback},
  {FORMSET_EXIT,      EXIT_KEY_DEFAULT,               ExitFormCallback},
  {FORMSET_EXIT,      EXIT_KEY_SAVE_EXIT,             ExitFormCallback},
  {FORMSET_EXIT,      EXIT_KEY_DISCARD_MODIFY,        ExitFormCallback},
  {FORMSET_EXIT,      EXIT_KEY_SAVE_EXIT_AND_RESET,   ExitFormCallback},
  {FORMSET_EXIT,      EXIT_KEY_DISCARD_EXIT_AND_RESET,ExitFormCallback},
  {FORMSET_EXIT,      KEY_BIOS_UPDATE,                ExitFormCallback},
  {FORMSET_EXIT,      KEY_BIOS_UPDATE_ALL,            ExitFormCallback},
  {FORMSET_EXIT,      KEY_BIOS_UPDATE_NETWORK,        ExitFormCallback},
  {FORMSET_EXIT,      KEY_BIOS_UPDATE_NETWORK_ALL,    ExitFormCallback},
  {FORMSET_EXIT,      KEY_SAVE_USER_DEFAULTS_VALUE,   ExitFormCallback},
  {FORMSET_EXIT,      KEY_RESTORE_USER_DEFAULTS_VALUE,ExitFormCallback},
  
  {FORMSET_SECURITY,  SEC_KEY_ADMIN_PD,               PasswordFormCallback},  
  {FORMSET_SECURITY,  SEC_KEY_POWER_ON_PD,            PasswordFormCallback},  
  {FORMSET_SECURITY,  SEC_KEY_CLEAR_USER_PD,          PasswordFormCallback},
  {FORMSET_ADVANCE,    KEY_USB_DEV_LIST,               UsbListCallback},
  {FORMSET_BOOT,      KEY_VALUE_CSM_CONFIGURATION,    BootFormCallback},
};

DYNAMIC_ITEM gDynamicSetupItem[] = {
  {FORMSET_EXIT,     ROOT_FORM_ID,    LABEL_CHANGE_LANGUAGE, STRING_TOKEN(STR_LANGUAGE_SELECT), STRING_TOKEN(STR_LANGUAGE_SELECT)},
  {FORMSET_EXIT,     ROOT_FORM_ID,    LABEL_CHOOSE_FOLDER,     STRING_TOKEN(STR_CHOOSE_FOLDER),     STRING_TOKEN(STR_CHOOSE_FOLDER)},
  {FORMSET_SECURITY, ROOT_FORM_ID,    SECURITY_DYNAMIC_LABEL,   STRING_TOKEN (FORMSET_TITLE_SECURITY),     STRING_TOKEN (FORMSET_TITLE_SECURITY)},
  {FORMSET_ADVANCE,  ROOT_FORM_ID,    LABEL_DYNAMIC_FORMSET,    STRING_TOKEN (STR_DYNAMIC_DEVICE),     STRING_TOKEN (STR_DYNAMIC_DEVICE)},
  {FORMSET_BOOT,     ROOT_FORM_ID,    LABEL_CHANGE_BOOT_ORDER,  STRING_TOKEN (STR_CHANGE_BOOT_ORDER),  STRING_TOKEN (STR_CHANGE_BOOT_ORDER_HELP)},
};

FORM_INIT_ITEM gFormInit[] = {
   {FORMSET_SECURITY, SecurityFormInit},
};

EFI_STATUS
EFIAPI
HiiExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *This,
  IN  CONST EFI_STRING    Request,
  OUT EFI_STRING    *Progress,
  OUT EFI_STRING    *Results
  )
{
  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;

  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
HiiRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *This,
  IN  CONST EFI_STRING    Configuration,
  OUT EFI_STRING    *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Configuration;

  return EFI_UNSUPPORTED;
}


EFI_STATUS
EFIAPI
FormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *This,
  IN EFI_BROWSER_ACTION    Action,
  IN EFI_QUESTION_ID    KeyValue,
  IN UINT8    Type,
  IN EFI_IFR_TYPE_VALUE    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST    *ActionRequest
  )
{
  EFI_STATUS Status;
  UINTN    Index;
  SETUP_FORMSET_INFO    *SetupFormSet;

  SetupFormSet =  BYO_FORMSET_INFO_FROM_THIS (This);
  DMSG((EFI_D_ERROR, "\n PlatformSetupDxe, FormCallback(), HiiHandle :0x%x, Class :%d. \n", SetupFormSet->HiiHandle, SetupFormSet->FormSetInfo.Class));
  
  Status = EFI_UNSUPPORTED;
  for (Index = 0; Index < (sizeof(gFormCallback) / sizeof(FORM_CALLBACK_ITEM)); Index++) {
    if ((gFormCallback[Index].Class == SetupFormSet->FormSetInfo.Class) &&
      ((gFormCallback[Index].Key == KeyValue) || (gFormCallback[Index].Key == KEY_UNASSIGN_GROUP))) {
      Status = gFormCallback[Index].Callback (This, Action, KeyValue, Type, Value, ActionRequest);
    }
  }
  return Status;
}

VOID
SetupLoadEventCallback (
  IN EFI_EVENT Event,
  IN VOID     *Context
  )
{
  UINTN    Index;

  //
  // FormSet Infomation.
  //
  for (Index = 0; Index < (sizeof(gSetupFormSets) / sizeof(FORMSET_INFO)); Index++) {
   mByoSetup->AddFormset(mByoSetup, (FORMSET_INFO *)&gSetupFormSets[Index]);
  }
  //
  // Dynamic Items.
  //
  for (Index = 0; Index < (sizeof(gDynamicSetupItem) / sizeof(DYNAMIC_ITEM)); Index++) {
   mByoSetup->AddDynamicItem(mByoSetup, (DYNAMIC_ITEM *)&gDynamicSetupItem[Index]);
  }

  gBS->CloseEvent(Event);
  
  return;
}

VOID
KeepPasswordStatus (
  VOID
);

EFI_STATUS
PlatformDiscardValue (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  KeepPasswordStatus ();
  return EFI_SUCCESS;
}

EFI_STATUS
PlatformLoadDefault (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  KeepPasswordStatus ();
  return EFI_SUCCESS;
}

EFI_STATUS
GetUserSaveNvData (
  IN OUT BYO_SETUP_USER_SAVE_DEFEAT *UserData
  )
{
  UINTN                           VariableSize;
  EFI_STATUS                      Status;

  VariableSize = sizeof(BYO_SETUP_USER_SAVE_DEFEAT);
  Status = gRT->GetVariable (
                    L"ByoUserDefault",
                    &gPlatformSetupVariableGuid,
                    NULL,
                    &VariableSize,
                    UserData
                    );
  if(EFI_ERROR(Status)){
    ZeroMem(UserData, sizeof(BYO_SETUP_USER_SAVE_DEFEAT));
  }
  return Status;
}

EFI_STATUS
GetCurrentLanguage (
  IN OUT BYO_SETUP_USER_SAVE_DEFEAT *UserData
  )
{
  EFI_STATUS                      Status;
  BYO_PLATFORM_LANGUAGE_PROTOCOL  *PlatLang;


  Status = gBS->LocateProtocol(&gByoPlatformLanguageProtocolGuid, NULL, (VOID**)&PlatLang);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  
  
  if(AsciiStrCmp(PlatLang->PlatformLang, "en-US") == 0){
    UserData->Language = 0x80;
  } else if(AsciiStrCmp(PlatLang->PlatformLang, "zh-Hans") == 0){
    UserData->Language = 0x81;
  } else {
    UserData->Language = 0x00;
  }
  DMSG((EFI_D_INFO, "(L%d) %a %X\n", __LINE__, PlatLang->PlatformLang, UserData->Language));

ProcExit:
  return Status;
}

EFI_STATUS
LoadUserVarToLanguage (
   IN BYO_SETUP_USER_SAVE_DEFEAT  *UserData
  )
{
  CHAR8                           *Lang;
  LANGUAGE_VALUE                  LangVal;
  EFI_GUID                        LangValueVariableGuid = LANGUAGE_VALUE_VARIABLE_GUID;
  EFI_STATUS                      Status = EFI_UNSUPPORTED;

  DMSG((EFI_D_ERROR, "CheckUserDefault(),\n"));

  if(UserData->Language) {
    if(UserData->Language == 0x80){
      Lang = "en-US";
      LangVal.Value = 0;
    } else if(UserData->Language == 0x81){
      Lang = "zh-Hans";
      LangVal.Value = 1;
    } else {
      Lang = "zh-Hans";
      LangVal.Value = 1;
    }
    LibGetSetPlatformLanguage(FALSE, &Lang);

    HiiSetBrowserData (
      &LangValueVariableGuid,
      L"LangValue",
      sizeof(LANGUAGE_VALUE),
      (UINT8*)&LangVal,
      NULL
      );

    Status = gRT->SetVariable (
                  L"PlatformLang",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  AsciiStrSize(Lang),
                  Lang
                  );
  }

  return Status;
}

/**
 Save user-set defaults.

  @param This            Points to the SETUP_SAVE_NOTIFY_PROTOCOL.

  @retval EFI_SUCCESS    Save defaults successfully.
  @retval other          Some error occurs when saving defaults.
**/
EFI_STATUS
PlatformSaveUserDefault (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  EFI_STATUS                                 Status;
  BOOLEAN                                    IsSucceed; 
  BYO_PLATFOMR_SETUP                         ByoPlatformSetup;
  UINTN                                      VariableSize;
  BOOT_TIMEOUT_DATA                          ByoBootTimeOut;
  BYO_SETUP_USER_SAVE_DEFEAT                 ByoUserDefeat;
  SYSTEM_PASSWORD                            SetupPassword;

  //
  //Save BYO PLATFOMR SETUP
  //
  VariableSize = sizeof(BYO_PLATFOMR_SETUP);
  IsSucceed = HiiGetBrowserData (
                &gPlatformSetupVariableGuid, 
                L"ByoSetup",
                VariableSize, 
                (UINT8*)&ByoPlatformSetup
                );
  if(IsSucceed) {
    Status = gRT->SetVariable (
                L"ByoSetupUserDefault",
                &gPlatformSetupVariableGuid,
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                VariableSize,
                &ByoPlatformSetup
                );
    ASSERT_EFI_ERROR (Status);
  }

  //
  //Save BYO_SETUP_USER_SAVE_DEFEAT
  //
  Status = GetCurrentLanguage(&ByoUserDefeat);
  if (EFI_ERROR (Status)) {
    DMSG((EFI_D_ERROR, "GetCurrentLanguage(), Status %r\n", Status));
  }

  VariableSize = sizeof(BOOT_TIMEOUT_DATA);
  IsSucceed = HiiGetBrowserData (
                &gEfiGlobalVariableGuid, 
                L"Timeout",
                VariableSize, 
                (UINT8*)&ByoBootTimeOut
                );
  if(IsSucceed && !EFI_ERROR (Status)) {
    ByoUserDefeat.Timeout = ByoBootTimeOut.Timeout;

    //
    //Save the current Settings to Variable "ByoUserDefault"
    //
    Status = gRT->SetVariable(
                L"ByoUserDefault", 
                &gPlatformSetupVariableGuid, 
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                sizeof(BYO_SETUP_USER_SAVE_DEFEAT), 
                &ByoUserDefeat
                ); 
    ASSERT_EFI_ERROR (Status);
    }

  //
  //Save the password related user default option values on the security page
  //
  IsSucceed = HiiGetBrowserData (
                &gEfiSystemPasswordVariableGuid, 
                SYSTEM_PASSWORD_NAME, 
                sizeof (SYSTEM_PASSWORD), 
                (UINT8*) &SetupPassword
                );
  if (IsSucceed) {
    Status = gRT->SetVariable (
                  USER_DEFEAT_SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  sizeof(SYSTEM_PASSWORD),
                  &SetupPassword
                  );
    ASSERT_EFI_ERROR (Status);
  }
  return EFI_SUCCESS;
}

/**
  Restores user-set defaults.

  @param This            Points to the SETUP_SAVE_NOTIFY_PROTOCOL.

  @retval EFI_SUCCESS    Restore defaults successfully.
  @retval other          Some error occurs when restoring defaults.
**/
EFI_STATUS
PlatformLoadUserDefault (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  EFI_STATUS                                 Status;
  BYO_PLATFOMR_SETUP                         ByoPlatformSetup;
  UINTN                                      VariableSize;
  BOOT_TIMEOUT_DATA                          ByoBootTimeOut;
  BYO_SETUP_USER_SAVE_DEFEAT                 ByoUserDefeat;
  SYSTEM_PASSWORD                            SetupPassword;

  VariableSize = sizeof(BYO_PLATFOMR_SETUP);
  Status = gRT->GetVariable (
                L"ByoSetupUserDefault",
                &gPlatformSetupVariableGuid,
                NULL,
                &VariableSize,
                &ByoPlatformSetup
                );
  if(!EFI_ERROR(Status)) {
    HiiSetBrowserData (
      &gPlatformSetupVariableGuid,
      L"ByoSetup",
      VariableSize,
      (UINT8 *) &ByoPlatformSetup,
      NULL
    );
  }

  VariableSize = sizeof(BYO_SETUP_USER_SAVE_DEFEAT);
  Status = gRT->GetVariable (
                  L"ByoUserDefault",
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &ByoUserDefeat
                  );
  DMSG ((EFI_D_ERROR, "PlatformLoadUserDefault(), Get User Defeats  :%r.\n", Status));  
  if (!EFI_ERROR(Status)) {
    //
    //Restores user's language Settings
    //
    Status = LoadUserVarToLanguage(&ByoUserDefeat);
    VariableSize = sizeof(BOOT_TIMEOUT_DATA);
    if(!EFI_ERROR(Status)) {
      ByoBootTimeOut.Timeout = ByoUserDefeat.Timeout;
      HiiSetBrowserData (
        &gEfiGlobalVariableGuid,
        L"Timeout",
        VariableSize,
        (UINT8 *) &ByoBootTimeOut,
        NULL
      );
      ASSERT_EFI_ERROR (Status);
    }
  }

  //
  //Restore the password related user default option values on the security page.
  //
  VariableSize = sizeof(SYSTEM_PASSWORD);  
  Status = gRT->GetVariable (
                  USER_DEFEAT_SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupPassword
                  );
  if (!EFI_ERROR (Status)) {
    HiiSetBrowserData (
      &gEfiSystemPasswordVariableGuid,
      SYSTEM_PASSWORD_NAME,
      sizeof (SYSTEM_PASSWORD),
      (UINT8 *)&SetupPassword,
      NULL
      );
  }
  return EFI_SUCCESS;
}

VOID
EFIAPI
UpdateLockNumState (
  EFI_EVENT                               Event,
  VOID                                    *Context
  )
{
  EFI_STATUS                         Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL	 *TextInEx;
  EFI_KEY_TOGGLE_STATE               State;

  Status = gBS->HandleProtocol(gST->ConsoleInHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID**)&TextInEx);
  if(EFI_ERROR(Status)){
    return;
  }

  if (mNubLockEn) {
    State = EFI_TOGGLE_STATE_VALID;
  } else {
    State = EFI_NUM_LOCK_ACTIVE | EFI_TOGGLE_STATE_VALID;
  }
  TextInEx->SetState(TextInEx, &State);

  return;
}

VOID PlatformDxeReadyToBootHook()
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        Handle;
  UINTN                             VariableSize;
  UINT16                            BootCurrent;
  CHAR16                            BootOptionName[ARRAY_SIZE (L"Boot####")];
  EFI_BOOT_MANAGER_LOAD_OPTION      BmBootOption;
  STATIC BOOLEAN                    IsInstalled = FALSE;

  if (IsInstalled) {
    //
    // gNetworkStackFvDpxGuid has been installed before.
    //
    return;
  }

  VariableSize = sizeof(BootCurrent);
  Status = gRT->GetVariable (
                  EFI_BOOT_CURRENT_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &VariableSize,
                  &BootCurrent);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%a: failed to get %g:\"%s\": %r\n", gEfiCallerBaseName, __FUNCTION__, &gEfiGlobalVariableGuid, EFI_BOOT_CURRENT_VARIABLE_NAME, Status));
    return;
  }
  if (VariableSize != sizeof BootCurrent) {
    DEBUG ((DEBUG_ERROR, "%a:%a: got %Lu bytes for %g:\"%s\", expected %Lu\n", gEfiCallerBaseName, __FUNCTION__, (UINT64)VariableSize,
      &gEfiGlobalVariableGuid, EFI_BOOT_CURRENT_VARIABLE_NAME, (UINT64)sizeof BootCurrent));
    return;
  }

  //
  // Get the Boot#### option that the status code applies to.
  //
  UnicodeSPrint (BootOptionName, sizeof BootOptionName, L"Boot%04x", BootCurrent);
  Status = EfiBootManagerVariableToLoadOption (BootOptionName, &BmBootOption);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%a: EfiBootManagerVariableToLoadOption(\"%s\"): %r\n", gEfiCallerBaseName, __FUNCTION__, BootOptionName, Status));
    return;
  }

  //
  // Check whether it is network boot option
  //
  if (GetEfiBootGroupType (BmBootOption.FilePath) != BM_MENU_TYPE_UEFI_PXE) {
    EfiBootManagerFreeLoadOption (&BmBootOption);
    return;
  }
  EfiBootManagerFreeLoadOption (&BmBootOption);

  Status = gBS->InstallProtocolInterface (
        &Handle,
        &gNetworkStackFvDpxGuid,
        EFI_NATIVE_INTERFACE,
        NULL
        );
  if (!EFI_ERROR (Status)) {
    IsInstalled = TRUE;
  }
  return;
}

EFI_STATUS
SetupItemBootNotify(
  SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL    *This,
  UINTN                                Item,
  UINTN                                NewStatus
  )
{
  BYO_PLATFOMR_SETUP  ByoPlatformSetup;

  if(Item == ITEM_SECUREBOOT && NewStatus == STATUS_ENABLE){
    DEBUG((EFI_D_INFO, "secure enable, try to disable csm\n"));
    HiiGetBrowserData(
      &gPlatformSetupVariableGuid,
      EFI_BYO_SETUP_VARIABLE_NAME,
      sizeof(BYO_PLATFOMR_SETUP),
      (UINT8*)&ByoPlatformSetup
      );
    ByoPlatformSetup.BootModeType = BIOS_BOOT_UEFI_OS;
    ByoPlatformSetup.Csm          = 0;
    HiiSetBrowserData(
      &gPlatformSetupVariableGuid,
      EFI_BYO_SETUP_VARIABLE_NAME,
      sizeof(BYO_PLATFOMR_SETUP),
      (UINT8*)&ByoPlatformSetup,
      NULL
      );
  }

  return EFI_SUCCESS;
}

/**
  The driver Entry Point. The function will export a disk device class formset and
  its callback function to hii database.

  @param  ImageHandle    The firmware allocated handle for the EFI image.
  @param  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval other          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
PlatformSetupDxeInit (
  IN EFI_HANDLE    ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_EVENT           Event;
  VOID                *pSetupRegistration;
  BYO_PLATFOMR_SETUP  ByoPlatformSetup;
  UINTN               Size;
  UINT8               *UefiGroupOrder = NULL;
  UINT32              Data;
  SYSTEM_MISC_SETTING *SystemMiscSetting;
  UINTN               PcdSize;
  UINT8               VideoRomPolicy;
  UINT8               PxeRomPolicy;
  UINT8               StorageRomPolicy;
  UINT8               OtherRomPolicy;
  EFI_HANDLE          Handle = NULL;
  PXE_CONTROL_VARIABLE                PxeControl;

  Size = sizeof(BYO_PLATFOMR_SETUP);
  Status = gRT->GetVariable (
                  EFI_BYO_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &ByoPlatformSetup
                  );
  if(EFI_ERROR (Status)){
    mNubLockEn = TRUE;
    PcdSet32S(PcdSystemMiscConfig, PcdGet32(PcdSystemMiscConfig) | SYS_MISC_CFG_SHELL_EN);
    Status = gRT->SetVariable (
                  EFI_BYO_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Size,
                  &ByoPlatformSetup
                  );
    ASSERT_EFI_ERROR (Status);
  } else {
    PcdSet8S(PcdMaxVerifyTimes, ByoPlatformSetup.MaxVerifyTimes);
    PcdSet8S(PcdRecorderPasswordTime, ByoPlatformSetup.PasswordRepeatTimes);
    PcdSet16S(PcdPasswordUnlockTime, ByoPlatformSetup.UnlockTime);
    PcdSet16S(PcdLockScreenTime, ByoPlatformSetup.ScreenLockTime);
    PcdSet8S(PcdPasswordReminderDays, ByoPlatformSetup.PasswordReminderDays);
    if (ByoPlatformSetup.ShellEn) {
      PcdSet32S(PcdSystemMiscConfig, PcdGet32(PcdSystemMiscConfig) | SYS_MISC_CFG_SHELL_EN);
    } else {
      PcdSet32S(PcdSystemMiscConfig, PcdGet32(PcdSystemMiscConfig) & (~SYS_MISC_CFG_SHELL_EN));
    }
    mNubLockEn = ByoPlatformSetup.NumLockEn;
    PcdSet8S(PcdPasswordComplexity, ByoPlatformSetup.PasswordComplexity);
    PcdSetBoolS(PcdBiosIdVersionCheck, ByoPlatformSetup.FlashRollBackProtect);
    PcdSetBoolS (PcdDisplayMode, ByoPlatformSetup.DisplayMode);
  }

  //
  //  Synchronize the numlock state
  //
  EfiCreateProtocolNotifyEvent (
  &gByoAllDriversConnectedProtocolGuid,
  TPL_CALLBACK,
  UpdateLockNumState,
  NULL,
  &Event
  );

  Size = sizeof (Data);
  if(ByoPlatformSetup.RTVariablesProtect) {
    Status = gRT->GetVariable(
                      FLASH_LOCK_STATUS_NAME,
                      &gEfiFlashLockStatusVariableGuid,
                      NULL,
                      &Size,
                      &Data
                      );
    if (Status == EFI_NOT_FOUND) {
      Data = 0;
    }

    Data = Data | BYO_SMIFLASH_STS_CFG_LOCK;
    
    Status = gRT->SetVariable(
                      FLASH_LOCK_STATUS_NAME,
                      &gEfiFlashLockStatusVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      Size,
                      &Data
                      );
    ASSERT(!EFI_ERROR(Status));
  } else {
    Status = gRT->GetVariable(
                      FLASH_LOCK_STATUS_NAME,
                      &gEfiFlashLockStatusVariableGuid,
                      NULL,
                      &Size,
                      &Data
                      );
    if (Status == EFI_SUCCESS) {
      Data = Data & (~ BYO_SMIFLASH_STS_CFG_LOCK);
      Status = gRT->SetVariable(
                        FLASH_LOCK_STATUS_NAME,
                        &gEfiFlashLockStatusVariableGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        Size,
                        &Data
                        );
      ASSERT(!EFI_ERROR(Status));
    }
  }

  //
  // Set L"UefiBootGroupOrder" variable
  //
  Status = GetVariable2 (
              BYO_UEFI_BOOT_GROUP_VAR_NAME,
              &gByoGlobalVariableGuid,
              (void**)&UefiGroupOrder,
              &Size
              );
  if (EFI_ERROR(Status)) {
    Size = sizeof(mDefaultUefiBootGroupOrder);
    UefiGroupOrder = AllocateCopyPool(Size, mDefaultUefiBootGroupOrder);
    Status = gRT->SetVariable (
                    BYO_UEFI_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    Size,
                    UefiGroupOrder
                    );
    if (UefiGroupOrder != NULL) {
      FreePool(UefiGroupOrder);
    }
    ASSERT_EFI_ERROR (Status);
  }

  if(ByoPlatformSetup.PxeRetryEn){
    PcdSet8S(PcdRetryPxeBoot, TRUE);
    PcdSet16S(PcdPxeRetrylimites, ByoPlatformSetup.PxeRetryEn);    
  } else {
    PcdSet8S(PcdRetryPxeBoot, FALSE);
  }

  //
  // Setting pcds of Legacy or UEFI booting
  //
  if (PcdGetBool(PcdPlatSupportCSM)) {
    mSetupItemNotify.Notify = SetupItemBootNotify;
    gBS->InstallProtocolInterface (
                 &ImageHandle,
                 &gSetupItemUpdateNotifyProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &mSetupItemNotify);

    SystemMiscSetting = (SYSTEM_MISC_SETTING *) PcdGetPtr (PcdSystemMiscSetting);
    if (ByoPlatformSetup.Csm != 0) {
      PcdSet8S(PcdLegacyBiosSupport, ByoPlatformSetup.Csm);
      SystemMiscSetting->SysSetting.Bits.LegacyBiosSupport = ByoPlatformSetup.Csm;
      if (ByoPlatformSetup.OptionRomExecution == 0) { // Auto:according to BootModeType
        if (ByoPlatformSetup.BootModeType != BIOS_BOOT_LEGACY_OS) {
          VideoRomPolicy = ROM_POLICY_UEFI_FIRST;
          PxeRomPolicy = ROM_POLICY_UEFI_FIRST;
          StorageRomPolicy = ROM_POLICY_UEFI_FIRST;
          OtherRomPolicy = ROM_POLICY_UEFI_FIRST;
        } else {
          VideoRomPolicy = ROM_POLICY_LEGACY_FIRST;
          PxeRomPolicy = ROM_POLICY_LEGACY_FIRST;
          StorageRomPolicy = ROM_POLICY_LEGACY_FIRST;
          OtherRomPolicy = ROM_POLICY_LEGACY_FIRST;
        }
      } else { // Manual
        VideoRomPolicy = ByoPlatformSetup.VideoRomPolicy;
        PxeRomPolicy = ByoPlatformSetup.PxeRomPolicy;
        StorageRomPolicy = ByoPlatformSetup.StorageRomPolicy;
        OtherRomPolicy = ByoPlatformSetup.OtherRomPolicy;
      }
    } else {
      ByoPlatformSetup.BootModeType = BIOS_BOOT_UEFI_OS;
      VideoRomPolicy = ROM_POLICY_UEFI_FIRST;
      PxeRomPolicy = ROM_POLICY_UEFI_FIRST;
      StorageRomPolicy = ROM_POLICY_UEFI_FIRST;
      OtherRomPolicy = ROM_POLICY_UEFI_FIRST;
    }
    PcdSet8S(PcdBiosBootModeType, ByoPlatformSetup.BootModeType);
    PcdSet8S(PcdVideoOpRomLaunchPolicy, VideoRomPolicy);
    PcdSet8S(PcdPxeOpRomLaunchPolicy, PxeRomPolicy);
    PcdSet8S(PcdStorageOpRomLaunchPolicy, StorageRomPolicy);
    PcdSet8S(PcdOtherOpRomLaunchPolicy, OtherRomPolicy);

    SystemMiscSetting->PciSetting.Bits.VideoOpRomLaunchPolicy   = VideoRomPolicy;
    SystemMiscSetting->PciSetting.Bits.PxeOpRomLaunchPolicy     = PxeRomPolicy;
    SystemMiscSetting->PciSetting.Bits.StorageOpRomLaunchPolicy = StorageRomPolicy;
    SystemMiscSetting->PciSetting.Bits.OtherOpRomLaunchPolicy   = OtherRomPolicy;

    PcdSize = PcdGetSize (PcdSystemMiscSetting);
    PcdSetPtrS (PcdSystemMiscSetting, &PcdSize, (VOID *)SystemMiscSetting);

    if (ByoPlatformSetup.Csm != 0) {
      Status = gBS->InstallProtocolInterface (
                        &Handle,
                        &gByoCsmOnDpxGuid,
                        EFI_NATIVE_INTERFACE,
                        NULL
                        );

      //
      // set usb policy for legacy usb
      //
      ZeroMem(&gUsbPolicy, sizeof(gUsbPolicy));
      gUsbPolicy.Version                     = (UINT8)USB_POLICY_PROTOCOL_REVISION_2;
      gUsbPolicy.UsbMassStorageEmulationType = ATUO_TYPE;     // It seems spell error, but ...
      gUsbPolicy.UsbOperationMode            = HIGH_SPEED;
      gUsbPolicy.LegacyKBEnable              = LEGACY_KB_EN;
      gUsbPolicy.LegacyMSEnable              = LEGACY_MS_EN;//LEGACY_MS_DIS;
      gUsbPolicy.UsbMassSupport              = 1;
      gUsbPolicy.USBPeriodSupport            = LEGACY_PERIOD_UN_SUPP;
      gUsbPolicy.LegacyFreeSupport           = LEGACY_FREE_SUPP;
      gUsbPolicy.LegacyUsbEnable             = LEGACY_USB_EN;
      gUsbPolicy.XhcSupport                  = LEGACY_XHC_SUPP;
      gUsbPolicy.UsbTimeTue                  = LEGACY_USB_TIME_TUE_ENABLE;
      gUsbPolicy.InternelHubExist            = USB_NOT_HAVE_HUB_INTERNEL;
      gUsbPolicy.EnumWaitPortStableStall     = 100;
      gUsbPolicy.CodeBase                    = ICBD_CODE_BASE;
      Status = gBS->InstallMultipleProtocolInterfaces (
                  &gImageHandle,
                  &gUsbPolicyGuid, &gUsbPolicy,
                  NULL
                  );
    } else {
      Status = gBS->InstallProtocolInterface (
                        &Handle,
                        &gByoCsmOffDpxGuid,
                        EFI_NATIVE_INTERFACE,
                        NULL
                        );
    }
  }
  
  //
  // Create event to trigger enter setup.
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SetupLoadEventCallback,
                  NULL,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //
  pSetupRegistration = NULL;
  Status = gBS->RegisterProtocolNotify (
                  &gEfiSetupEnterGuid,
                  Event,
                  &pSetupRegistration
                  );

  ASSERT_EFI_ERROR (Status);

  //
  //Install Byo Platform Setup Protocol.
  //
  InstallByoSetupProtocol ();

  //Install setup notify function.
  //
  ZeroMem(&gSetupSaveNotify, sizeof(SETUP_SAVE_NOTIFY_PROTOCOL));
  gSetupSaveNotify.LoadDefault    = PlatformLoadDefault;
  gSetupSaveNotify.DiscardValue   = PlatformDiscardValue;
  gSetupSaveNotify.SaveUserDefault = PlatformSaveUserDefault;
  gSetupSaveNotify.LoadUserDefault = PlatformLoadUserDefault;
  Status = gBS->InstallProtocolInterface (
                  &gSetupSaveNotify.DriverHandle,
                  &gSetupSaveNotifyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gSetupSaveNotify
                  );
  //
  //Defer loading common network stack modules.
  //
  Size = sizeof(PXE_CONTROL_VARIABLE);
  Status = gRT->GetVariable (
                  PXE_CONTROL_VAR_NAME,
                  &gPxeControlVariableGuid,
                  NULL,
                  &Size,
                  &PxeControl
                  );
  if( (!EFI_ERROR (Status)) && (PxeControl.UefiNetworkStack) && (PcdGet8(PcdBiosBootModeType) != BIOS_BOOT_LEGACY_OS)){
    PcdSetBoolS (PcdAlwaysCreatNetOption, TRUE);
    Status = gBS->InstallProtocolInterface (
            &ImageHandle,
            &gEfiReadyToBootProtocolGuid,
            EFI_NATIVE_INTERFACE,
            (VOID*)(UINTN)PlatformDxeReadyToBootHook
            );
    DEBUG ((DEBUG_ERROR, "%a %d install gEfiReadyToBootProtocolGuid  Status is %r\n", __FUNCTION__, __LINE__, Status));
  }

  return EFI_SUCCESS;
}

/**
  Unload its installed protocol.

  @param[in]  ImageHandle       Handle that identifies the image to be unloaded.

  @retval EFI_SUCCESS           The image has been unloaded.
**/
EFI_STATUS
EFIAPI
PlatformSetupDxeUnload (
  IN EFI_HANDLE    ImageHandle
  )
{
  EFI_STATUS    Status;
  DMSG ((EFI_D_ERROR, "\n PlatformSetupDxeUnload(), \n"));  

  Status = EFI_SUCCESS;
  if (gSetupSaveNotify.DriverHandle != NULL) {
    Status = gBS->UninstallProtocolInterface (
                      gSetupSaveNotify.DriverHandle,
                      &gSetupSaveNotifyProtocolGuid,
                      &gSetupSaveNotify
                      );
    DMSG ((EFI_D_ERROR, "PlatformSetupDxeUnload(), Uninstall gSetupSaveNotify :%r.\n", Status));  
  }

  if (mByoSetup != NULL && mByoSetup->DriverHandle != NULL) {
    Status = gBS->UninstallProtocolInterface (
                      mByoSetup->DriverHandle,
                      &gEfiByoPlatformSetupGuid,
                      mByoSetup
                      );
    FreePool (mByoSetup);
    mByoSetup = NULL;
    DMSG ((EFI_D_ERROR, "PlatformSetupDxeUnload(), Uninstall mByoSetup :%r.\n", Status));  
  }

  return Status;
}

EFI_STATUS
InitializeForm (
  VOID 
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  SETUP_FORMSET_INFO    * SetupFormSet = NULL;
  LIST_ENTRY    *Link;
  UINTN    Index;

  //
  // Initialize Forms.
  //
  DMSG((EFI_D_ERROR, "\n InitializeForm, \n"));
  Link = GetFirstNode (&mByoSetup->MainFormsetList);
  while (!IsNull (&mByoSetup->MainFormsetList, Link)) {
    SetupFormSet = BYO_FORMSET_INFO_FROM_LINK (Link);
    Link = GetNextNode (&mByoSetup->MainFormsetList, Link);

    for (Index = 0; Index < (sizeof(gFormInit) / sizeof(FORM_INIT_ITEM)); Index++) {
      if (SetupFormSet->FormSetInfo.Class == gFormInit[Index].Class && SetupFormSet->HiiHandle != NULL) {
        Status = gFormInit[Index].FormInit(SetupFormSet->HiiHandle);
        if(EFI_ERROR(Status)){
          DEBUG((EFI_D_ERROR, "InitializeForm Form_%d:%r\n", SetupFormSet->FormSetInfo.Class, Status));
        }
      }
    }
  }
  DMSG((EFI_D_ERROR, "InitializeForm, End.\n"));  
  return Status;  
}