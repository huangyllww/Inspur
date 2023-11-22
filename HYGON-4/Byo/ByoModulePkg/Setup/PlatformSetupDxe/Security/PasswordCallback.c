/** @file

Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PasswordFormCallback.c

Abstract:
  PasswordFormCallback Setup Routines

Revision History:


**/


#include <PlatformSetupDxe.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SetupUiLib.h>
#include <Library/HiiLib.h>
#include <Library/SystemPasswordLib.h>
#include <Library/PcdLib.h>

#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Protocol/SystemPasswordProtocol.h>
#include <Protocol/ByoNetworkIdentifierProtocol.h>
#include <Protocol/DisplayBackupRecovery.h>
#include <DhcpGetNetworldCfg.h>

#define GET_NETWORK_IDENTIFIER_RETRY_MAX_COUNT   3
UINT8                          mGetNetworkIdCount = 0;

VOID
ExpireWarningMessage (
  EFI_HII_HANDLE HiiHandle
  )
{
  CHAR16   *String;

  String = HiiGetString(HiiHandle, STRING_TOKEN(STR_OUT_TIME_PASSWORD_STRING), NULL);
  UiConfirmDialogEx (2, DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
  FreePool(String);
}

VOID
KeepPasswordStatus (
  VOID
)
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  SYSTEM_PASSWORD               SetupPassword;
  SYSTEM_PASSWORD               PasswordVar;
  UINTN                         VariableSize;

  VariableSize = sizeof(SYSTEM_PASSWORD);  
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupPassword
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Get password from browser
    //
    HiiGetBrowserData (
      &gEfiSystemPasswordVariableGuid, 
      SYSTEM_PASSWORD_NAME, 
      sizeof (SYSTEM_PASSWORD), 
      (UINT8*) &PasswordVar
      );

    //
    // Restore password to browser.
    //
    PasswordVar.bHaveAdmin   = SetupPassword.bHaveAdmin;
    PasswordVar.bHavePowerOn = SetupPassword.bHavePowerOn;
    if (SetupPassword.bHaveAdmin || SetupPassword.bHavePowerOn) {
      PasswordVar.HashType     = SetupPassword.HashType;
    }
    if (SetupPassword.bHaveAdmin == 0) {
      PasswordVar.ChangePopByUser = SetupPassword.ChangePopByUser;
    }

    HiiSetBrowserData (
      &gEfiSystemPasswordVariableGuid,
      SYSTEM_PASSWORD_NAME,
      sizeof (SYSTEM_PASSWORD),
      (UINT8 *)&PasswordVar,
      NULL
      );
  }

}

VOID 
SetPasswordHashType (
  UINT8 HashType
)
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  SYSTEM_PASSWORD               SetupPassword;
  UINTN                         VariableSize;

  VariableSize = sizeof(SYSTEM_PASSWORD);  
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupPassword
                  );
  if (!EFI_ERROR (Status)) {
    SetupPassword.HashType = HashType;
    VariableSize = sizeof(SYSTEM_PASSWORD);
    Status = gRT->SetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  VariableSize,
                  &SetupPassword
                  );
    ASSERT_EFI_ERROR (Status);
  }

  return;
}

EFI_STATUS
GetAndSetSalt (
  SETUP_FORMSET_INFO            *SetupFormSet,
  EFI_SYSTEM_PASSWORD_PROTOCOL  *SystemPassword
  )
{
  EFI_STATUS                            Status;
  NETWORK_IDENTIFIER_PROTOCOL           *NetworkIdentifierProtocol = NULL;
  UINT8                                 *NetWorkId = NULL;
  UINT8                                 NetWorkIdLen = 0x0;
  EFI_STRING                            ReSetTips = NULL;
  EFI_STRING                            NetAuthPwStr = NULL;
  EFI_STRING                            NetAuthPromptStr = NULL;
  PASSWORD_NETWORK_AUTH_CONFIG          PwdNetworkAuthInfo;
  UINTN                                 VariableSize;
  BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL   *BackupRecoverProtocol = NULL;
  BACKUP_RECOVER_DATA                   *BackupRestoreData = NULL;

  ReSetTips        = HiiGetString (SetupFormSet->HiiHandle, STRING_TOKEN(STR_GETNETWORKID_MSG), NULL);
  NetAuthPwStr     = HiiGetString (SetupFormSet->HiiHandle, STRING_TOKEN(STR_NETWORK_AUTH_RUNTIME), NULL);
  NetAuthPromptStr = HiiGetString (SetupFormSet->HiiHandle, STRING_TOKEN(STR_NETWORK_AUTH_PROMPT), NULL);
  if (mGetNetworkIdCount >= GET_NETWORK_IDENTIFIER_RETRY_MAX_COUNT) {
    Status = EFI_LOAD_ERROR;
    goto ProExit;
  }

  VariableSize = sizeof (PASSWORD_NETWORK_AUTH_CONFIG);
  Status = gRT->GetVariable (
                  PASSWORD_NETWORK_AUTH_VAR_NAME,
                  &gByoPasswordNetworkAuthConfigGuid,
                  NULL,
                  &VariableSize,
                  &PwdNetworkAuthInfo
                  );
  if (EFI_ERROR (Status) || PwdNetworkAuthInfo.PwdNetworkAuthEnable != 0x1) {
    DMSG ((DEBUG_ERROR, "Get PASSWORD_NETWORK_AUTH_VAR_NAME Failed!\n"));
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (&gByoNetworkIdentifierProtocolGuid, NULL, (VOID **)&NetworkIdentifierProtocol);
  if (EFI_ERROR(Status)) {
    DMSG ((DEBUG_ERROR, "Locate NetworkIdentifier Protocol Failed!\n"));
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (&gDisplayBackupRecoverProtocolGuid, NULL, (VOID **)&BackupRecoverProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = BackupRecoverProtocol->DispalyBackup(&BackupRestoreData);
  UiConfirmDialog (DIALOG_NO_KEY, NetAuthPromptStr, NULL, TEXT_ALIGIN_CENTER, NetAuthPwStr, L"", NULL);

  NetWorkId = AllocateZeroPool (NETWORK_ID_MAX_SIZE);
  if (NetWorkId == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  do {
    if (mGetNetworkIdCount < GET_NETWORK_IDENTIFIER_RETRY_MAX_COUNT) {
      NetWorkIdLen = (UINT8) NETWORK_ID_MAX_SIZE; 
      Status = NetworkIdentifierProtocol->GetNetworkId (NetWorkId, &NetWorkIdLen);

      if (!EFI_ERROR (Status)) {
        DMSG ((DEBUG_ERROR, "Get Network Identifier Success!\n"));
        mGetNetworkIdCount = 0;
        SystemPassword->SetSaltBuffer (NetWorkId, NetWorkIdLen);
        Status = EFI_SUCCESS;
        break;
      } else {
        DMSG ((DEBUG_ERROR, "Get Network Identifier Failed!\n"));
        mGetNetworkIdCount ++;
        if (mGetNetworkIdCount < GET_NETWORK_IDENTIFIER_RETRY_MAX_COUNT) {
          continue;
        } else {
          Status = EFI_LOAD_ERROR;
          break;
        }
      }
    }
  } while (1);

  gBS->Stall (2000000);
  BackupRecoverProtocol->DispalyRecover (BackupRestoreData);

ProExit:
  if (Status == EFI_LOAD_ERROR) {
    UiConfirmDialog (DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, ReSetTips, NULL);
  }

  FreePool (ReSetTips);
  FreePool (NetAuthPwStr);
  FreePool (NetAuthPromptStr);

  if (BackupRestoreData != NULL) {
    FreePool (BackupRestoreData);
  }

  if (NetWorkId != NULL) {
    FreePool (NetWorkId);
  }

  return Status;
}

EFI_STATUS
EFIAPI
PasswordFormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  EFI_SYSTEM_PASSWORD_PROTOCOL  *SystemPassword = NULL;
  SYSTEM_PASSWORD               PasswordVar;
  static UINTN                  PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
  CHAR16                        *Password = NULL;
  EFI_HII_HANDLE                HiiHandle = NULL;
  SETUP_FORMSET_INFO            *SetupFormSet;
  EFI_STRING                    String = NULL;
  UINT8                         PwdState;
  VERIFY_RESULT                 VerifyResult;
  BYO_PLATFOMR_SETUP            ByoPlatformSetup;

  DEBUG((EFI_D_INFO, "\n PasswordFormCallback, Action :0x%X, KeyValue :0x%x, Type :0x%X.\n", Action, KeyValue, Type));
  if (Action != EFI_BROWSER_ACTION_CHANGING) {
    return EFI_UNSUPPORTED;
  }

  //
  // Get system password protocol.
  //
  Status = gBS->LocateProtocol (
                  &gEfiSystemPasswordProtocolGuid,
                  NULL,
                  (VOID**)&SystemPassword
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  SystemPassword->SetMode(VERIFY_MODE_SETUP);

  //
  // Get password status.
  //
  if (!HiiGetBrowserData(&gEfiSystemPasswordVariableGuid, SYSTEM_PASSWORD_NAME, sizeof (SYSTEM_PASSWORD), (UINT8*) &PasswordVar)) {
    DEBUG((EFI_D_INFO, "PasswordFormCallback, System Password Not Found.\n"));
    return EFI_NOT_FOUND;
  }

  if (!HiiGetBrowserData(&gPlatformSetupVariableGuid, EFI_BYO_SETUP_VARIABLE_NAME, sizeof (BYO_PLATFOMR_SETUP), (UINT8*) &ByoPlatformSetup)) {
    DEBUG((EFI_D_INFO, "PasswordFormCallback, BYO SETUP VARIABLE Not Found.\n"));
    return EFI_NOT_FOUND;
  }
  //
  // Get Security form hii database handle.
  //
  SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
  HiiHandle = SetupFormSet->HiiHandle;
  ASSERT(HiiHandle != NULL);
  if (Type == EFI_IFR_TYPE_STRING && Value->string != 0) {
    //DEBUG((EFI_D_INFO, "Sting Value :%d. \n", Value->string));
    Password = HiiGetString(HiiHandle, Value->string, NULL);
  }

  if (NULL == Password && PasswordState == BROWSER_STATE_SET_PASSWORD) {
    PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
    return EFI_SUCCESS;
  }

  switch (KeyValue) {
    case SEC_KEY_ADMIN_PD:
      switch (PasswordState) {
        case BROWSER_STATE_VALIDATE_PASSWORD:
          if (Password != NULL) {
            if (StrLen (Password) == 0) {
              Status = GetAndSetSalt (SetupFormSet, SystemPassword);
              if (Status == EFI_LOAD_ERROR) {
                return EFI_NOT_AVAILABLE_YET;
              }
            }
          }
          if (!SystemPassword->BeHave(PD_ADMIN)) {
            PasswordState = BROWSER_STATE_SET_PASSWORD;
            return EFI_SUCCESS;
          }
          if (NULL ==Password) {
            PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
            return EFI_NOT_READY;
          }
          if (SystemPassword->Verify(PD_ADMIN, Password, &VerifyResult)) {
            PasswordState = BROWSER_STATE_SET_PASSWORD;
            Status = EFI_SUCCESS;
            if (VerifyResult == VERIFY_EXPIRE) {
              ExpireWarningMessage (HiiHandle);
            }
          } else {
            Status = EFI_NOT_READY;
          }
          break;

        case BROWSER_STATE_SET_PASSWORD:
          if (*Password != CHAR_NULL) {
            if (ByoPlatformSetup.PasswordComplexity && (!ByoVerifyPasswordComplexity(Password))) {
              *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
              SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_PD), NULL);
              UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              Status = EFI_ALREADY_STARTED;
              break;
            }
            //
            // Update HashType to Password Variable when set password
            //
            if (PasswordVar.bHaveAdmin == 0 && PasswordVar.bHavePowerOn == 0) {
              SetPasswordHashType (PasswordVar.HashType);
            }
            PwdState = ByoCheckPasswordRecord (KeyValue - SEC_KEY_ADMIN_PD, Password);
            if (PwdState) {
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_PASSWORD_TIMES), NULL);
              UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              Status = EFI_ALREADY_STARTED;
              break;
            }
            SystemPassword->Write(PD_ADMIN, Password);
            PasswordVar.bHaveAdmin = 1;
            *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
            SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
            String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_SET_PASSWORD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
            Status = EFI_ALREADY_STARTED;
          } else {
            SystemPassword->Clear(PD_ADMIN);
            PasswordVar.bHaveAdmin = 0;
            PasswordVar.ChangePopByUser = 1;
            *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
            SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
            String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_CLEAR_PD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
            Status = EFI_ALREADY_STARTED;
          }
          PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
          break;

        default:
          PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
          break;
      }
      break;

    case SEC_KEY_POWER_ON_PD:
      switch (PasswordState) {
        case BROWSER_STATE_VALIDATE_PASSWORD:
          if (StrLen (Password) == 0) {
            Status = GetAndSetSalt (SetupFormSet, SystemPassword);
            if (Status == EFI_LOAD_ERROR) {
              return EFI_NOT_AVAILABLE_YET;
            }
          }
          if (!SystemPassword->BeHave(PD_POWER_ON)) {
            PasswordState = BROWSER_STATE_SET_PASSWORD;
            return EFI_SUCCESS;
          }
          if (NULL ==Password) {
            PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
            return EFI_NOT_READY;
          }
          if (SystemPassword->Verify(PD_POWER_ON, Password, &VerifyResult)) {
            PasswordState = BROWSER_STATE_SET_PASSWORD;
            Status = EFI_SUCCESS;
            if (VerifyResult == VERIFY_EXPIRE) {
              ExpireWarningMessage (HiiHandle);
            }
          } else {
            Status = EFI_NOT_READY;
          }
          break;

        case BROWSER_STATE_SET_PASSWORD:
          if (*Password != CHAR_NULL) {
            if (ByoPlatformSetup.PasswordComplexity && (!ByoVerifyPasswordComplexity(Password))) {
              *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
              SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_PD), NULL);
              UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              Status = EFI_ALREADY_STARTED;
              break;
            }
            //
            // Update HashType to Password Variable when set password
            //
            if (PasswordVar.bHaveAdmin == 0 && PasswordVar.bHavePowerOn == 0) {
              SetPasswordHashType (PasswordVar.HashType);
            }
            PwdState = ByoCheckPasswordRecord (KeyValue - SEC_KEY_ADMIN_PD, Password);
            if (PwdState) {
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_PASSWORD_TIMES), NULL);
              UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              Status = EFI_ALREADY_STARTED;
              break;
            }
            SystemPassword->Write(PD_POWER_ON, Password);
            PasswordVar.bHavePowerOn = 1;
            *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
            SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
            String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_SET_PASSWORD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
            Status = EFI_ALREADY_STARTED;
          } else {
            SystemPassword->Clear(PD_POWER_ON);
            PasswordVar.bHavePowerOn = 0;
            *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
            SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
            String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_CLEAR_PD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
            Status = EFI_ALREADY_STARTED;
          }
          PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
          break;

        default:
          PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
          break;
      }
      break;

    case SEC_KEY_CLEAR_USER_PD:
      if (SystemPassword->BeHave(PD_POWER_ON)) {
        SystemPassword->Clear (PD_POWER_ON);
        PasswordVar.bHavePowerOn = 0;
        *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
        SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
        String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_CLEAR_PD), NULL);
        UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
        Status = EFI_ALREADY_STARTED;
      }
      break;

    default:
      break;
  }
  
  if(String != NULL){
    FreePool (String);
  }
  //
  // Restore password to browser.
  //
  HiiSetBrowserData (
    &gEfiSystemPasswordVariableGuid,
    SYSTEM_PASSWORD_NAME,
    sizeof (SYSTEM_PASSWORD),
    (UINT8 *)&PasswordVar,
    NULL
    );

  return Status;
}
