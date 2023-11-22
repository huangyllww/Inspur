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
  PasswordFormCallback Setup Rountines

Revision History:


**/


#include <PlatformSetupDxe.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/SetupUiLib.h>
#include <Library/HiiLib.h>
#include <Library/SystemPasswordLib.h>

#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Protocol/SystemPasswordProtocol.h>

#define RECORDER_TIME    5
#define MIN_PASSWORD_COMPLEXITY    3
#define SYSTEM_PASSWORD_RECORDER_NAME   L"SR"

#pragma pack(1)
typedef struct _SYSTEM_PASSWORD_RECORDER {
  UINT8    AdmCount;
  UINT8    PopCount;
  UINT8    Admin[RECORDER_TIME][SYSTEM_PASSWORD_HASH_LENGTH];
  UINT8    PowerOn[RECORDER_TIME][SYSTEM_PASSWORD_HASH_LENGTH];
} SYSTEM_PASSWORD_RECORDER;
#pragma pack()

VOID HandleFatalError(EFI_HII_HANDLE HiiHandle)
{
  CHAR16   *Content;
  CHAR16   *Title;

  Title   = HiiGetString(HiiHandle, STRING_TOKEN(STR_ERROR), NULL);
  Content = HiiGetString(HiiHandle, STRING_TOKEN(STR_PASSWORD_FATAL_ERROR), NULL);
  SetOemDialogColor(DLG_OEM_COLOR_RED);
  UiConfirmDialog(DIALOG_NO_KEY, Title, NULL, TEXT_ALIGIN_CENTER, Content, NULL);
  FreePool(Title);
  FreePool(Content);
  gBS->RaiseTPL(TPL_HIGH_LEVEL);
  CpuDeadLoop();
}

VOID
ExpireWarningMessage (
  EFI_HII_HANDLE HiiHandle
  )
{
  CHAR16   *String;

  String = HiiGetString(HiiHandle, STRING_TOKEN(STR_OUT_TIME_PASSWORD_STRING), NULL);
  UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
  FreePool(String);
}

BOOLEAN 
CheckPasswordMatch(CHAR16 *Password, UINT8 CheckType)
{
  UINTN   i;
  UINTN   Length;

  Length = StrLen(Password);

  switch(CheckType) {
    case 0:
      for(i = 0; i < Length; i++) {
        if((Password[i] >= '0') && (Password[i] <= '9')) {
          //DEBUG((EFI_D_INFO, "CheckType0: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    case 1:
      for(i = 0; i < Length; i++) {
        if((Password[i] >= 'A') && (Password[i] <= 'Z')) {
          //DEBUG((EFI_D_INFO, "CheckType1: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    case 2:
      for(i = 0; i < Length; i++) {
        if(((Password[i] >= ' ') && (Password[i] <= '/')) ||
           ((Password[i] >= ':') && (Password[i] <= '@')) ||
           ((Password[i] >= '[') && (Password[i] <= '`')) ||
           ((Password[i] >= '{') && (Password[i] <= '~'))) {
          //DEBUG((EFI_D_INFO, "CheckType2: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    case 3:
      for(i = 0; i < Length; i++) {
        if((Password[i] >= 'a') && (Password[i] <= 'z')) {
          //DEBUG((EFI_D_INFO, "CheckType3: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    default:
      return FALSE;
      break;
  }
  return FALSE;
}

BOOLEAN 
VerifyPasswordComplexity(CHAR16 *Password)
{
  UINT16 i;
  UINT16 j;
  UINT16 k;
  UINT16 n;

  i = CheckPasswordMatch(Password,0);
  j = CheckPasswordMatch(Password,1);
  k = CheckPasswordMatch(Password,2);
  n = CheckPasswordMatch(Password,3);
  
  if(i + j + k + n >= MIN_PASSWORD_COMPLEXITY) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  @return 1   Password is invalid.
  @retval 0   Password is valid.
**/
UINT8
CheckPasswordRecord (
  UINT16    Type,
  CHAR16    *Password
)
{
  EFI_STATUS                Status;
  UINTN                     VariableSize;
  SYSTEM_PASSWORD_RECORDER  PasswordRecorder;
  UINTN                     Len;
  UINT8                     *Str = NULL;
  UINT8                     *PasswordHash = NULL;
  UINTN                     Index;
  UINT8                     ReCount = 0;

  Len = Unicode2Ascii(Password, &Str);
  if (Len > 0) {
    EncodePassword(Str, Len, &PasswordHash, 0);
    if(Str != NULL){
      FreePool(Str);
    }
  } else {
    return 0;
  }


  VariableSize = sizeof(SYSTEM_PASSWORD_RECORDER);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_RECORDER_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &PasswordRecorder
                  );
  if (EFI_ERROR (Status)) {
    ZeroMem(&PasswordRecorder, VariableSize);
  }

  for (Index = 0; Index < RECORDER_TIME; Index ++) {
    if (Type == SEC_KEY_ADMIN_PD) {
      Str = PasswordRecorder.Admin[Index];
    } else {
      Str = PasswordRecorder.PowerOn[Index];
    }
    if (CompareMem(Str, PasswordHash, SYSTEM_PASSWORD_HASH_LENGTH) == 0) {
      break;
    }
  }

  if (Index < RECORDER_TIME) {
    return 1;
  }

  if (Type == SEC_KEY_ADMIN_PD) {
    ReCount = PasswordRecorder.AdmCount;
    if (ReCount == RECORDER_TIME) {
      for (Index = 0; Index < (RECORDER_TIME - 1); Index ++) {
        CopyMem(PasswordRecorder.Admin[Index], PasswordRecorder.Admin[Index + 1], SYSTEM_PASSWORD_HASH_LENGTH);
      }
      Str = PasswordRecorder.Admin[ReCount - 1];
    } else {
      Str = PasswordRecorder.Admin[ReCount];
      PasswordRecorder.AdmCount ++;
    }
  } else {
    ReCount = PasswordRecorder.PopCount;
    if (ReCount == RECORDER_TIME) {
      for (Index = 0; Index < (RECORDER_TIME - 1); Index ++) {
        CopyMem(PasswordRecorder.PowerOn[Index], PasswordRecorder.PowerOn[Index + 1], SYSTEM_PASSWORD_HASH_LENGTH);
      }
      Str = PasswordRecorder.PowerOn[ReCount - 1];
    } else {
      Str = PasswordRecorder.PowerOn[ReCount];
      PasswordRecorder.PopCount ++;
    }
  }
  CopyMem(Str, PasswordHash, SYSTEM_PASSWORD_HASH_LENGTH);

  Status = gRT->SetVariable (
                  SYSTEM_PASSWORD_RECORDER_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  VariableSize,
                  &PasswordRecorder
                  );
  if(PasswordHash != NULL){
    FreePool (PasswordHash);
  }
  return 0;
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
  //
  // Get Security form hii databas handle.
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
            if (VerifyResult == VERIFY_WRONG) {
              HandleFatalError(HiiHandle);
            }
          }
          break;

        case BROWSER_STATE_SET_PASSWORD:
          if (*Password != CHAR_NULL) {
            if (!VerifyPasswordComplexity(Password)) {
              *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
              SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_PD), NULL);
              UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              Status = EFI_ALREADY_STARTED;
              break;
            }
            PwdState = CheckPasswordRecord (KeyValue, Password);
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
            if (VerifyResult == VERIFY_WRONG) {
              HandleFatalError(HiiHandle);
            }
          }
          break;

        case BROWSER_STATE_SET_PASSWORD:
          if (*Password != CHAR_NULL) {
            if (!VerifyPasswordComplexity(Password)) {
              *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
              SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_ENTER_CORRECT_PD), NULL);
              UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              Status = EFI_ALREADY_STARTED;
              break;
            }
            PwdState = CheckPasswordRecord (KeyValue, Password);
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
