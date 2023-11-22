/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HddSanitizationDxe.c

Abstract:
  Hdd Sanitization DXE driver.

Revision History:

**/




#include "HddSanitizationDxe.h"
#include "HddSanitizationCommon.h"
#include <Protocol/DisplayBackupRecovery.h>

EFI_GUID                         gHddSanitizeMethodCfgGuid = HDD_SANITIZE_METHOD_CONFIG_GUID;
LIST_ENTRY                       mHddSanitizationCfgFormList;
UINTN                            mNumberOfHddDevices = 0;
HDD_SANITIZE_DXE_PRIVATE_DATA    *gPrivate;
HDD_SECURITY_INFO                mHddInitialBrowserData;
EFI_EVENT                        HddSanitizeProgressEvent;
HDD_DLG_CTX                      gHddDlgCtx = {0, 0, NULL};

CHAR16                           *gSanitizeTitleStr;
CHAR16                           *gSanitizeWarnigStr;
CHAR16                           *gSanitizeNoPowerOffStr[2];
TIMER_CALLBACK_CTX               HddSanitizeTimeCtx;
EFI_EVENT                        HddSanitizeProgressEvent;

//
// This Function Construct full request string,extracts the current configuration from a block of bytes.
//
VOID
FormRequestConfig (
IN  CONST EFI_GUID              *Guid,
IN  CONST CHAR16                *VariableName,
IN  CONST UINTN                 BlockSize,
IN  CONST UINT8                 *BlockData,
IN  CONST EFI_STRING            Request,
OUT EFI_STRING                  *Progress,
OUT EFI_STRING                  *Results
 )
{
  EFI_STATUS                       Status;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;

  DEBUG((EFI_D_INFO, "%a %g\n", __FUNCTION__, Guid));
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;

  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr(Guid, VariableName, gPrivate->DriverHandle);
    Size = (StrLen(ConfigRequestHdr) + 32 + 1) * sizeof(CHAR16);
    ConfigRequest = AllocateZeroPool(Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint(ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, BlockSize);
    FreePool (ConfigRequestHdr);
  }

  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                BlockData,
                                BlockSize,
                                Results,
                                Progress
                                );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "BlockToConfig:%r\n", Status));
  }
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
}

//-----------------------------------------------------------------
EFI_STATUS
EFIAPI
FormExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINTN             BlockSize;
  UINT8             *BlockData;
  
  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  if (Request == NULL) {
    return EFI_NOT_FOUND;
  }

  if (HiiIsConfigHdrMatch(Request, &gHddSanitizeMethodCfgGuid, HDD_SANITIZE_METHOD_CONFIG_VAR_NAME)) {
    BlockSize = sizeof(HDD_SANITIZE_METHOD_CONFIG);
    BlockData = (UINT8*)&gPrivate->Current->HddSecInfo;
    FormRequestConfig(&gHddSanitizeMethodCfgGuid, HDD_SANITIZE_METHOD_CONFIG_VAR_NAME, BlockSize, BlockData, Request, Progress, Results);
  } else {
    DEBUG((EFI_D_ERROR, "HiiIsConfigHdrMatch error\n"));
    return EFI_NOT_FOUND;
  }

  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen(Request);
  }

  return Status;
}



EFI_STATUS
EFIAPI
FormRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if(HiiIsConfigHdrMatch(Configuration, &gHddSanitizeMethodCfgGuid, HDD_SANITIZE_METHOD_CONFIG_VAR_NAME)){
    *Progress = Configuration + StrLen(Configuration);
    return EFI_SUCCESS;
  }

  *Progress = Configuration;

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
FormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  HDD_SANITIZE_DEVICE_ENTRY       *HddSanDevEntry;
  EFI_STRING_ID                    StrId;
  NVME_DEVICE_INFO                 *NvmeInfo;
  HDD_SANITIZE_METHOD_CONFIG       HddSanInfo;
  ATA_DEVICE_INFO                  *AtaInfo;
  EFI_RESET_SYSTEM                 OldResetSystem;

  DEBUG((EFI_D_INFO, "%a() A:%d Q:%d T:%d\n", __FUNCTION__, Action, QuestionId, Type));

  if(!gHddDlgCtx.Init){
    gHddDlgCtx.HiiHandle = gPrivate->HiiHandle;
    gHddDlgCtx.Init = 1;
  }

  if (Action == EFI_BROWSER_ACTION_CHANGING && QuestionId >= KEY_HDD_DEVICE_ENTRY_BASE &&
      QuestionId <  KEY_HDD_DEVICE_ENTRY_BASE + mNumberOfHddDevices) {
    HddSanDevEntry = HddSanitizationGetConfigFormEntryByIndex(QuestionId - KEY_HDD_DEVICE_ENTRY_BASE);
    ASSERT (HddSanDevEntry != NULL);
    gHddDlgCtx.Current = HddSanDevEntry;
    DEBUG((EFI_D_INFO, "Current:%X\n", HddSanDevEntry));
    //
    // This data structure is used to track device state in HDD Password configure form
    // While it is useless after form exit. On exit, Data will be restored to avoid unnecessary
    //
    HiiGetBrowserData(
      &gHddSanitizeMethodCfgGuid,
      HDD_SANITIZE_METHOD_CONFIG_VAR_NAME,
      sizeof(HDD_SANITIZE_METHOD_CONFIG),
      (UINT8*)&mHddInitialBrowserData
      );
  }

  //
  // Restore initial HDD Configuration after exit HDD page to avoid unnecessary Saving Action in Browser
  //
  if ((Action == EFI_BROWSER_ACTION_FORM_CLOSE) && 
      (QuestionId == KEY_HDD_ATA_NAME || QuestionId == KEY_HDD_LOCK_NAME || QuestionId == KEY_HDD_NVME_NAME)) {

    HiiSetBrowserData(
      &gHddSanitizeMethodCfgGuid,
      HDD_SANITIZE_METHOD_CONFIG_VAR_NAME,
      sizeof(HDD_SANITIZE_METHOD_CONFIG),
      (UINT8*)&mHddInitialBrowserData,
      NULL
      );
    return EFI_SUCCESS;
  }

  if(Action == EFI_BROWSER_ACTION_RETRIEVE && gHddDlgCtx.Current != NULL) {
    if(QuestionId == KEY_HDD_ATA_NAME || QuestionId == KEY_HDD_LOCK_NAME || QuestionId == KEY_HDD_NVME_NAME){
      StrId = HiiSetString(gHddDlgCtx.HiiHandle, STRING_TOKEN(STR_HDD_NAME), gHddDlgCtx.Current->HddString, NULL);
      DEBUG((EFI_D_INFO, "StrId:%X %s\n", StrId, gHddDlgCtx.Current->HddString));
    }
    return EFI_SUCCESS;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
  }

  gHddDlgCtx.CurQid = QuestionId;

  //
  // Display special dialog title based on different HDD operation
  //
  switch (QuestionId) {
    case NVME_SANITIZE_START:
      //
      //Get the NVMe Sanitize operation type via Browser Data
      //
      HiiGetBrowserData(
        &gHddSanitizeMethodCfgGuid,
        HDD_SANITIZE_METHOD_CONFIG_VAR_NAME,
        sizeof(HDD_SANITIZE_METHOD_CONFIG),
        (UINT8*)&HddSanInfo
        );

      NvmeInfo = (NVME_DEVICE_INFO *)gHddDlgCtx.Current->DevInfo;
        //
        //During the sanitize progress,disable System reset function.
        //
        OldResetSystem   = gRT->ResetSystem;
        gRT->ResetSystem = ResetSystemNull;

        if (NvmeInfo->SanitizeCap != 0x0) {
          Status = NvmeDeviceSanitize (NvmeInfo, &(HddSanInfo.SanitizeMethod));
        } else {
          Status = NvmeBlockIoErase(NvmeInfo->BlockIo);
        }
        //
        //After Sanitize, restore the System reset function.
        //
        gRT->ResetSystem = OldResetSystem;
      break;

    case SATA_SANITIZE_START:
      //
      //Get the NVMe Sanitize operation type via Browser Data
      //
      HiiGetBrowserData(
        &gHddSanitizeMethodCfgGuid,
        HDD_SANITIZE_METHOD_CONFIG_VAR_NAME,
        sizeof(HDD_SANITIZE_METHOD_CONFIG),
        (UINT8*)&HddSanInfo
        );

      AtaInfo = (ATA_DEVICE_INFO *)gHddDlgCtx.Current->DevInfo;
        //
        //During the sanitize progress,disable System reset function.
        //
        OldResetSystem   = gRT->ResetSystem;
        gRT->ResetSystem = ResetSystemNull;

        if (AtaInfo->SanitizeSupported) {
          Status = SataDeviceSanitize(AtaInfo, &HddSanInfo.SanitizeMethod); 
        } else {
          Status = SataDevBlockIoErase(AtaInfo);
        }
        //
        //After Sanitize, restore the System reset function.
        //
        gRT->ResetSystem = OldResetSystem;
      break;
  }

  return Status;
}


VOID
EFIAPI
EnterSetupCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID                             *Interface;
  EFI_STATUS                       Status;
  VOID                             *StartOpCodeHandle;
  VOID                             *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL               *StartLabel;
  EFI_IFR_GUID_LABEL               *EndLabel;
  UINTN                            FormIndex;
  HDD_SANITIZE_DEVICE_ENTRY        *HddSanDevEntry;
  LIST_ENTRY                       *Entry;
  EFI_FORM_ID                      FormId;
  HDD_PASSWORD_INFO                *HdpInfo  = NULL;
  NVME_DEVICE_INFO                 *NvmeInfo = NULL;
  ATA_DEVICE_INFO                  *AtaInfo  = NULL;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->CloseEvent(Event);

  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = HDD_DEVICE_ENTRY_LABEL;

  EndLabel                 = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode   = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number         = HDD_DEVICE_LABEL_END;

  FormIndex = 0;
  EFI_LIST_FOR_EACH (Entry, &mHddSanitizationCfgFormList) {
    HddSanDevEntry = BASE_CR (Entry, HDD_SANITIZE_DEVICE_ENTRY, Link);
    if ((*(UINT32 *)HddSanDevEntry->DevInfo) == NVME_DEVICE_INFO_SIGN) {
      NvmeInfo = (NVME_DEVICE_INFO *)HddSanDevEntry->DevInfo;
      HdpInfo  = &NvmeInfo->HdpInfo;
    } else if ((*(UINT32 *)HddSanDevEntry->DevInfo) == ATA_DEVICE_INFO_SIGN) {
      AtaInfo  = (ATA_DEVICE_INFO *)HddSanDevEntry->DevInfo;
      HdpInfo  = &AtaInfo->HdpInfo;
    } else {
      ASSERT(FALSE);
      return;
    }

    //If NVMe device doesn't support Sanitize, use block erase
    // If HDD Password is set, data sanitize is supported by HddPassword module.
    // If HDD Password is not set, data sanitize is supported by HddSanitize module
    DEBUG((EFI_D_INFO, "HddSanDevEntry->HddSecInfo.SanitizeCap: 0x%x \n HddSanDevEntry->HddSecInfo.Supported:0x%X\n", HddSanDevEntry->HddSecInfo.SanitizeCap, HddSanDevEntry->HddSecInfo.Supported));
    if (!HddSanDevEntry->HddSecInfo.SanitizeCap && !HddSanDevEntry->HddSecInfo.Supported) {
      continue;
    }

    if (HdpInfo->HddIndex >= 0x80) {
      if (HddSanDevEntry->HddSecInfo.Enabled) {
        FormId = FORMID_HDD_PASSWORD_ENABLED_FORM;
      } else {
        FormId = FORMID_NVME_DEVICE_FORM;
      }
    } else {
      if (HddSanDevEntry->HddSecInfo.Enabled) {
        FormId = FORMID_HDD_PASSWORD_ENABLED_FORM;
      } else {
        FormId = FORMID_ATA_DEVICE_FORM;
      }
    }

    HiiSetString(gPrivate->HiiHandle, HddSanDevEntry->TitleToken, HddSanDevEntry->HddString, NULL);
    DEBUG((EFI_D_INFO, "StrId:%X %s\n", HddSanDevEntry->TitleToken, HddSanDevEntry->HddString));
    HiiCreateGotoOpCode (
      StartOpCodeHandle,                                // Container for dynamic created opcodes
      FormId,                                           // Target Form ID
      HddSanDevEntry->TitleToken,                       // Prompt text
      HddSanDevEntry->TitleHelpToken,                   // Help text
      EFI_IFR_FLAG_CALLBACK,                            // Question flag
      (UINT16)(KEY_HDD_DEVICE_ENTRY_BASE + FormIndex)   // Question ID
      );

    FormIndex++;

    DEBUG((EFI_D_INFO, "FormIndex:%X %X %X\n", FormIndex, HddSanDevEntry->TitleToken, HddSanDevEntry->TitleHelpToken));

  }
  
  Status = HiiUpdateForm (
            gPrivate->HiiHandle,
            &gHddSanitizeMethodCfgGuid,
            FORMID_HDD_MAIN_FORM,
            StartOpCodeHandle,
            EndOpCodeHandle
            );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  //
  //Create event for show progress in Sanitize Progress.
  //
  Status = gBS->CreateEvent(
        EVT_TIMER | EVT_NOTIFY_SIGNAL,
        TPL_NOTIFY,
        EraseTimerInSecondsCallBack,
        &HddSanitizeTimeCtx,
        &HddSanitizeProgressEvent
        );

  gSanitizeTitleStr         = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_HDD_SANITIZE_OPERATION), NULL);
  gSanitizeWarnigStr        = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_SANITIZATION_WARNING), NULL);
  gSanitizeNoPowerOffStr[0] = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_SANITIZATION_WARNING_NO_POWEROFF1), NULL);
  gSanitizeNoPowerOffStr[1] = HiiGetString(gPrivate->HiiHandle, STRING_TOKEN(STR_SANITIZATION_WARNING_NO_POWEROFF2), NULL); 

}

EFI_STATUS
EFIAPI
HddSanitizationDxeEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                       Status;
  HDD_SANITIZE_DXE_PRIVATE_DATA    *Private;
  VOID                             *Registration;
  EFI_BOOT_MODE                    BootMode;

  DEBUG((EFI_D_INFO, "%a() Enter\n", __FUNCTION__));
  BootMode = GetBootModeHob();
  if (BootMode == BOOT_IN_RECOVERY_MODE) {
    return EFI_UNSUPPORTED;
  }

  InitializeListHead(&mHddSanitizationCfgFormList);

  Private = AllocateZeroPool(sizeof(HDD_SANITIZE_DXE_PRIVATE_DATA));
  if (Private == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->Current                    = &Private->Dummy;
  Private->Signature                  = HDD_PASSWORD_DXE_PRIVATE_SIGNATURE;
  Private->ConfigAccess.ExtractConfig = FormExtractConfig;
  Private->ConfigAccess.RouteConfig   = FormRouteConfig;
  Private->ConfigAccess.Callback      = FormCallback;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Private->DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mHddSanitizationHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &Private->ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  Private->HiiHandle = HiiAddPackages (
                         &gHddSanitizeMethodCfgGuid,
                         Private->DriverHandle,
                         STRING_ARRAY_NAME,
                         HddSanitizationCfgBin,
                         NULL
                         );
  ASSERT(Private->HiiHandle != NULL);

  gPrivate = Private;

  Registration = NULL;
  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    EnterSetupCallBack,
    (VOID*)Private,
    &Registration
    );

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gAtaPassThruHookProtocolGuid, SataPassThruHookForHddSanitize,
                  &gNvmeSscpHookProtocolGuid, NvmeSscpHookForHddSanitize,
                  NULL
                  );

  return EFI_SUCCESS;
}
