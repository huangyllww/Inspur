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
  Implement of platform setup protocol.

Revision History:


--*/


#include "PlatformSetupDxe.h"


SETUP_DATA                         gSetupDataTemp;
STATIC SETUP_SAVE_NOTIFY_PROTOCOL  gSetupSaveNotify;


FORMSET_INFO    gSetupFormSets[] = {
  {FORMSET_MAIN,       FORMSET_GUID_MAIN,       FormsetMainBin,     PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
  {FORMSET_ADVANCE,    FORMSET_GUID_ADVANCE,    FormsetAdvancedBin, PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
//  {FORMSET_DEVICE,     FORMSET_GUID_DEVICE,     FormsetDevicesBin,  PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
//  {FORMSET_POWER,      FORMSET_GUID_POWER,      FormsetPowerBin,    PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
  {FORMSET_SECURITY,   FORMSET_GUID_SECURITY,   FormsetSecurityBin, PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
  {FORMSET_EXIT,       FORMSET_GUID_EXIT,       FormsetExitBin,     PlatformSetupDxeStrings, {HiiExtractConfig, HiiRouteConfig, FormCallback}},
};

FORM_CALLBACK_ITEM	gFormCallback[] = {

  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBPORT0_VAL,     AdvancedFormCallback},
  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBPORT1_VAL,     AdvancedFormCallback},
  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBPORT2_VAL,     AdvancedFormCallback},
  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBPORT3_VAL,     AdvancedFormCallback},
  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBPORT4_VAL,     AdvancedFormCallback},
  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBPORT5_VAL,     AdvancedFormCallback},
  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBPORT6_VAL,     AdvancedFormCallback},
  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBPORT7_VAL,     AdvancedFormCallback},
  //
  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBCtrl0_VAL,     AdvancedFormCallback},
  {FORMSET_ADVANCE,     KEY_VALUE_ADVANCED_USBCtrl1_VAL,     AdvancedFormCallback},
  //

  {FORMSET_SECURITY,    SEC_KEY_ADMIN_PD,                PasswordFormCallback},
  {FORMSET_SECURITY,    SEC_KEY_POWER_ON_PD,             PasswordFormCallback},
  {FORMSET_SECURITY,    SEC_KEY_CLEAR_USER_PD,           PasswordFormCallback},

  {FORMSET_EXIT,        KEY_SAVE_AND_EXIT_VALUE  ,       ExitFormCallback},
  {FORMSET_EXIT,        KEY_SAVE_VALUE  ,                ExitFormCallback},
  {FORMSET_EXIT,        KEY_DISCARD_AND_EXIT_VALUE,      ExitFormCallback},
  {FORMSET_EXIT,        KEY_RESTORE_DEFAULTS_VALUE,      ExitFormCallback},
//{FORMSET_EXIT,        KEY_SAVE_USER_DEFAULTS_VALUE,    ExitFormCallback},
//{FORMSET_EXIT,        KEY_RESTORE_USER_DEFAULTS_VALUE, ExitFormCallback},

  {FORMSET_EXIT,        KEY_BIOS_UPDATE,                 ExitFormCallback},
  {FORMSET_EXIT,        KEY_SYS_RESET,                   ExitFormCallback},
  {FORMSET_EXIT,        KEY_SYS_SHUTDOWN,                ExitFormCallback},
};

DYNAMIC_ITEM gDynamicSetupItem[] = {
  {FORMSET_MAIN,    ROOT_FORM_ID,    LABEL_CHANGE_LANGUAGE, STRING_TOKEN(STR_LANGUAGE_SELECT), STRING_TOKEN(STR_LANGUAGE_SELECT)},
};

FORM_INIT_ITEM gFormInit[] = {
  {FORMSET_MAIN,    MainFormInit},
  {FORMSET_ADVANCE, AdvanceFormInit},
  //{FORMSET_DEVICE,  DeviceFormInit},  
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
  EFI_STATUS    Status;
  SETUP_FORMSET_INFO    *SetupFormSet;
  EFI_STRING    ConfigRequestHdr;
  EFI_STRING    ConfigRequest;
  BOOLEAN    AllocatedRequest;
  UINTN    Size;
  UINTN    BufferSize;
  VOID    *SystemConfigPtr;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  SetupFormSet = BYO_FORMSET_INFO_FROM_THIS (This);
  DMSG((EFI_D_ERROR, "\n PlatformSetupDxe, HiiExtractConfig(), HiiHandle :0x%x, Class :%d. \n", SetupFormSet->HiiHandle, SetupFormSet->FormSetInfo.Class));

  *Progress = Request;
  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &gPlatformSetupVariableGuid, PLATFORM_SETUP_VARIABLE_NAME)) {
    return EFI_NOT_FOUND;
  }

  ConfigRequestHdr = NULL;
  ConfigRequest = NULL;
  Size = 0;
  AllocatedRequest = FALSE;
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&gPlatformSetupVariableGuid, PLATFORM_SETUP_VARIABLE_NAME, SetupFormSet->DriverHandle);
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    BufferSize = sizeof (SETUP_DATA);
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
  }
  
  SystemConfigPtr = GetVariable(PLATFORM_SETUP_VARIABLE_NAME, &gPlatformSetupVariableGuid);
  if (SystemConfigPtr == NULL) {
    ZeroMem(&gSetupDataTemp, sizeof(SETUP_DATA));
  } else {
    CopyMem(&gSetupDataTemp, SystemConfigPtr, sizeof(SETUP_DATA));
    FreePool(SystemConfigPtr);
  }
  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                (UINT8*)&gSetupDataTemp,
                                sizeof (SETUP_DATA),
                                Results,
                                Progress
                                );
  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
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

  if (!HiiIsConfigHdrMatch (Configuration, &gPlatformSetupVariableGuid, PLATFORM_SETUP_VARIABLE_NAME)) {
    return EFI_NOT_FOUND;
  }

  *Progress = Configuration + StrLen (Configuration);

  return EFI_SUCCESS;
}



EFI_STATUS
FormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *This,
  IN EFI_BROWSER_ACTION                      Action,
  IN EFI_QUESTION_ID                         KeyValue,
  IN UINT8                                   Type,
  IN EFI_IFR_TYPE_VALUE                      *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS Status;
  UINTN    Index;
  SETUP_FORMSET_INFO    *SetupFormSet;

  SetupFormSet =  BYO_FORMSET_INFO_FROM_THIS (This);
//DEBUG((EFI_D_INFO, "Hii:%X, Class:%X\n", SetupFormSet->HiiHandle, SetupFormSet->FormSetInfo.Class));
  
  Status = EFI_SUCCESS;
  for (Index = 0; Index < (sizeof(gFormCallback) / sizeof(FORM_CALLBACK_ITEM)); Index++) {
    if ((gFormCallback[Index].Class == SetupFormSet->FormSetInfo.Class) &&
      ((gFormCallback[Index].Key == KeyValue) || (gFormCallback[Index].Key == KEY_UNASSIGN_GROUP))) {
      Status = gFormCallback[Index].Callback (This, Action, KeyValue, Type, Value, ActionRequest);
    }
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

STATIC EFI_STATUS KeyNotifyCallBack(EFI_KEY_DATA *KeyData)
{
  UINTN                      VariableSize;
  UINTN                      VariableSetupSize;
  SETUP_VOLATILE_DATA        SetupVData;
  SYSTEM_PASSWORD            SystemPassword;
  EFI_STRING    RequestString = NULL;

  VariableSize = sizeof(SetupVData);
  VariableSetupSize = sizeof(SystemPassword);

  HiiGetBrowserData (&gEfiSystemPasswordVariableGuid, SYSTEM_PASSWORD_NAME, VariableSetupSize, (UINT8 *)&SystemPassword);
  DEBUG((EFI_D_INFO,"Login type is %d\n",SystemPassword.EnteredType));
  if(LOGIN_USER_ADMIN != SystemPassword.EnteredType){ 
    return EFI_SUCCESS;
  }

  HiiGetBrowserData (&gPlatformSetupVariableGuid, SETUP_VOLATILE_VARIABLE_NAME, VariableSize, (UINT8 *)&SetupVData);
  
  SetupVData.AdvanceControl = ~SetupVData.AdvanceControl;
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_VOLATILE_DATA, AdvanceControl), sizeof(SetupVData.AdvanceControl));

  HiiSetBrowserData (&gPlatformSetupVariableGuid, SETUP_VOLATILE_VARIABLE_NAME, VariableSize, (UINT8 *)&SetupVData, RequestString);
  FreePool (RequestString);
  
  return EFI_SUCCESS;	
}

STATIC VOID RegisterSetupHotKey()
{
  EFI_STATUS                         Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL	 *TextInEx;
  EFI_KEY_DATA                       KeyData;
  VOID                              *NotifyHandle;
  EDKII_FORM_BROWSER_EXTENSION2_PROTOCOL *FormBrowserEx2;

  Status = gBS->HandleProtocol(gST->ConsoleInHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID**)&TextInEx);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  Status = gBS->LocateProtocol (&gEdkiiFormBrowserEx2ProtocolGuid, NULL, (VOID **) &FormBrowserEx2);

  ZeroMem(&KeyData, sizeof(KeyData));
  KeyData.Key.ScanCode = SCAN_F11;
  FormBrowserEx2->RegisterHotKey (&KeyData.Key, BROWSER_ACTION_REFRESH, 0, L"Refresh HotKey");

  KeyData.Key.ScanCode            = SCAN_F11;
  KeyData.KeyState.KeyShiftState  = EFI_SHIFT_STATE_VALID | EFI_LEFT_CONTROL_PRESSED;
  Status = TextInEx->RegisterKeyNotify(TextInEx, &KeyData, KeyNotifyCallBack, &NotifyHandle);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "HotKey Reg line:%d :%r\n", __LINE__,Status));
  }
  KeyData.Key.ScanCode            = SCAN_F11;
  KeyData.KeyState.KeyShiftState  = EFI_SHIFT_STATE_VALID | EFI_RIGHT_CONTROL_PRESSED;
  Status = TextInEx->RegisterKeyNotify(TextInEx, &KeyData, KeyNotifyCallBack, &NotifyHandle);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "HotKey Reg line:%d :%r\n", __LINE__, Status));
  }

ProcExit:
  return;
}

VOID
SetupEnterCallback (
  IN EFI_EVENT Event,
  IN VOID     *Context
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINTN         Index;
  VOID          *Interface;
  EFI_BYO_FORM_BROWSER_EXTENSION_PROTOCOL *FormBrowserEx;


  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  for (Index = 0; Index < (sizeof(gSetupFormSets) / sizeof(FORMSET_INFO)); Index++) {
    Status = mByoSetup->AddFormset(mByoSetup, (FORMSET_INFO *)&gSetupFormSets[Index]);
  }
  for (Index = 0; Index < (sizeof(gDynamicSetupItem) / sizeof(DYNAMIC_ITEM)); Index++) {
    Status = mByoSetup->AddDynamicItem(mByoSetup, (DYNAMIC_ITEM *)&gDynamicSetupItem[Index]);
  }

  Status = gBS->LocateProtocol(&gEfiByoFormBrowserExProtocolGuid, NULL, &FormBrowserEx);
  if(!EFI_ERROR(Status)){
    FormBrowserEx->PlatformReset = PlatformReset;
  }
  // Register setup hotkey
  RegisterSetupHotKey ();
}

EFI_STATUS
PlatformLoadDefault (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  EFI_STRING    RequestString = NULL;
  SETUP_DATA            DefaultSetupData;

  ZeroMem(&DefaultSetupData, sizeof (SETUP_DATA));
  HiiGetBrowserData(
      &gPlatformSetupVariableGuid, 
      PLATFORM_SETUP_VARIABLE_NAME, 
      sizeof(SETUP_DATA), 
      (UINT8*)&DefaultSetupData
      );

  DefaultSetupData.USBRearCtrl     = 1;
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, USBRearCtrl), sizeof(DefaultSetupData.USBRearCtrl));

  DefaultSetupData.USBFronCtrl     = 1;
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, USBFronCtrl), sizeof(DefaultSetupData.USBFronCtrl));

  DefaultSetupData.Die0XHCIPort[0] = 1;
  DefaultSetupData.Die0XHCIPort[1] = 1;
  DefaultSetupData.Die0XHCIPort[2] = 1;
  DefaultSetupData.Die0XHCIPort[3] = 1;
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, Die0XHCIPort), sizeof(DefaultSetupData.Die0XHCIPort));

  DefaultSetupData.Die1XHCIPort[0] = 1;
  DefaultSetupData.Die1XHCIPort[1] = 1;
  DefaultSetupData.Die1XHCIPort[2] = 1;
  DefaultSetupData.Die1XHCIPort[3] = 1;	
  RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, Die1XHCIPort), sizeof(DefaultSetupData.Die1XHCIPort));

  HiiSetBrowserData(
      &gPlatformSetupVariableGuid,
      PLATFORM_SETUP_VARIABLE_NAME,
      sizeof(SETUP_DATA), 
      (UINT8*)&DefaultSetupData, 
      RequestString
      );
  FreePool (RequestString);
  
  return EFI_SUCCESS;
}


STATIC SETUP_SAVE_NOTIFY_PROTOCOL  gSetupSaveNotify;



VOID PlatSetupCollectHiiResourceHook()
{
  EFI_STATUS            Status = EFI_SUCCESS;
  UINTN                 Index;
  LIST_ENTRY            *Link;
  EFI_BYO_PLATFORM_SETUP_PROTOCOL  *ByoSetup;
  SETUP_FORMSET_INFO               *SetupFormSet;


  ByoSetup = mByoSetup;
  ASSERT(ByoSetup != NULL);
  if(ByoSetup == NULL){
    return;
  }
  
  for (Index = 0; Index < (sizeof(gSetupFormSets) / sizeof(FORMSET_INFO)); Index++) {
    Status = ByoSetup->AddFormset(ByoSetup, (FORMSET_INFO *)&gSetupFormSets[Index]);
  }
  
  Link = GetFirstNode (&ByoSetup->MainFormsetList);
  while (!IsNull (&ByoSetup->MainFormsetList, Link)) {
    SetupFormSet = BYO_FORMSET_INFO_FROM_LINK(Link);

    if(SetupFormSet->HiiHandle == NULL){
      SetupFormSet->HiiHandle = HiiAddPackages (
                                  &SetupFormSet->FormSetInfo.FormSetGuid,
                                  SetupFormSet->DriverHandle,
                                  SetupFormSet->FormSetInfo.IfrPack,
                                  SetupFormSet->FormSetInfo.StrPack,
                                  NULL
                                  );
    }
    if (SetupFormSet->HiiHandle == NULL) {
      DEBUG((EFI_D_INFO, "[ERR] %a HiiAddPackages\n", __FUNCTION__));
    }

    Link = GetNextNode (&ByoSetup->MainFormsetList, Link);
  }  
}


EFI_STATUS
PlatformSetupEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  VOID             *Registration;
  EFI_STATUS       Status;


  InstallByoSetupProtocol();
  
  ZeroMem(&gSetupSaveNotify, sizeof(SETUP_SAVE_NOTIFY_PROTOCOL));
  gSetupSaveNotify.LoadDefault = PlatformLoadDefault;

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &gSetupSaveNotify.DriverHandle,
                  &gSetupSaveNotifyProtocolGuid, &gSetupSaveNotify,
                  &gPlatCollectHiiResourceHookGuid, PlatSetupCollectHiiResourceHook,
                  NULL
                  );

  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    SetupEnterCallback,
    NULL,
    &Registration
    );

  return EFI_SUCCESS;
}




