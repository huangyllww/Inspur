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
#include <Library/BaseCryptLib.h>
#include <Library/SetupUiLib.h>
#include <Library/HiiLib.h>
#include <Library/SystemPasswordLib.h>

#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Protocol/SystemPasswordProtocol.h>
#include <Protocol/ByoSmiFlashProtocol.h>

#define RECORDER_TIME    5
#define SYSTEM_PASSWORD_RECORDER_NAME   L"SR"

#pragma pack(1)
typedef struct _SYSTEM_PASSWORD_RECORDER {
  UINT8    AdmCount;
  UINT8    PopCount;
  UINT8    Admin[RECORDER_TIME][SYSTEM_PASSWORD_HASH_LENGTH];
  UINT8    PowerOn[RECORDER_TIME][SYSTEM_PASSWORD_HASH_LENGTH];
} SYSTEM_PASSWORD_RECORDER;
#pragma pack()

typedef struct {
  EFI_KEY EFIKey;
  CHAR16  Unicode;
  CHAR16  ShiftedUnicode;
  CHAR16  ScanCode;
} KEY_DESCRIPTOR;

STATIC CONST KEY_DESCRIPTOR  KeyDescriptor[] = {
  {EfiKeyC1,    'a',      'A',       0x1e },
  {EfiKeyB5,    'b',      'B',       0x30 },
  {EfiKeyB3,    'c',      'C',       0x2e },
  {EfiKeyC3,    'd',      'D',       0x20 },
  {EfiKeyD3,    'e',      'E',       0x12 },
  {EfiKeyC4,    'f',      'F',       0x21 },
  {EfiKeyC5,    'g',      'G',       0x22 },
  {EfiKeyC6,    'h',      'H',       0x23 },
  {EfiKeyD8,    'i',      'I',       0x17 },
  {EfiKeyC7,    'j',      'J',       0x24 },
  {EfiKeyC8,    'k',      'K',       0x25 },
  {EfiKeyC9,    'l',      'L',       0x26 },
  {EfiKeyB7,    'm',      'M',       0x32 },
  {EfiKeyB6,    'n',      'N',       0x31 },
  {EfiKeyD9,    'o',      'O',       0x18 },
  {EfiKeyD10,   'p',      'P',       0x19 },
  {EfiKeyD1,    'q',      'Q',       0x10 },
  {EfiKeyD4,    'r',      'R',       0x13 },
  {EfiKeyC2,    's',      'S',       0x1f },
  {EfiKeyD5,    't',      'T',       0x14 },
  {EfiKeyD7,    'u',      'U',       0x16 },
  {EfiKeyB4,    'v',      'V',       0x2f },
  {EfiKeyD2,    'w',      'W',       0x11 },
  {EfiKeyB2,    'x',      'X',       0x2d },
  {EfiKeyD6,    'y',      'Y',       0x15 },
  {EfiKeyB1,    'z',      'Z',       0x2c },
  {EfiKeyE1,    '1',      '!',       0x02 },
  {EfiKeyE2,    '2',      '@',       0x03 },
  {EfiKeyE3,    '3',      '#',       0x04 },
  {EfiKeyE4,    '4',      '$',       0x05 },
  {EfiKeyE5,    '5',      '%',       0x06 },
  {EfiKeyE6,    '6',      '^',       0x07 },
  {EfiKeyE7,    '7',      '&',       0x08 },
  {EfiKeyE8,    '8',      '*',       0x09 },
  {EfiKeyE9,    '9',      '(',       0x0a },
  {EfiKeyE10,   '0',      ')',       0x0b },
  {EfiKeyLShift,     0,     0,       0x2a },
  {EfiKeyCapsLock,  0,     0,       0x3a }
};

STATIC CONST CHAR16 gValidMarkChar16[] = {
  L'!', L'\"', L'#', L'$', L'%', L'&', L'\'', L'(', 
  L')', L'*',  L'+', L',', L'-', L'.', L'/',  L':', 
  L';', L'<',  L'=', L'>', L'?', L'@', L'[',  L'\\',
  L']', L'^',  L'_', L'{', L'|', L'}', L'~',  L' ',
  L'`'
};

UINTN
EncodePsd (
  IN  UINT8    *Password,
  IN  UINTN    Length,
  OUT  UINT8    **Hash,
  IN  UINTN    Type    //Type :Ascii or Scancode
  )
{
  UINTN    Len;
  UINTN    i,j;
  UINTN    KeyIndex;
  UINTN    Count;
  UINT8   *Buf = NULL;
  UINT8    PasswordArray[64];
  UINT8    HashArray[32];
  BOOLEAN  ShiftPress;
  BOOLEAN  CaplockPress;
  
  Count        = 0;
  ShiftPress   = FALSE;
  CaplockPress = FALSE;
  Len = GetUniqueNumber(&Buf);
  ZeroMem(PasswordArray, sizeof(PasswordArray));
  CopyMem(PasswordArray, Buf, Len);
  FreePool(Buf);


  if(Type){
    //
    //convert Scancode to Ascii
    //
    for( i=0; i<Length; i++ ) { 
      j=i*2;
      if((Password[j] == 0x00)&&(Password[j+1] == 0x00)) break; 
      if('0'<=Password[i]&&Password[i]<='9')Password[i]=(UINT16)(Password[j]-'0');
      else if('a'<=Password[j]&&Password[j]<='f') Password[i]=(UINT16)(Password[j]-'a')+10;       
      else if('A'<=Password[j]&&Password[j]<='F') Password[i]=(UINT16)(Password[j]-'A')+10;         
      else return 0;
      
      if('0'<=Password[j+1]&&Password[j+1]<='9') Password[i]=Password[i]*0x10+(UINT16)(Password[j+1]-'0');         
        else if('a'<=Password[j+1]&&Password[j+1]<='f')Password[i]=Password[i]*0x10+(UINT16)(Password[j+1]-'a')+10;    
        else if('A'<=Password[j+1]&&Password[j+1]<='F') Password[i]=Password[i]*0x10+(UINT16)(Password[j+1]-'A')+10;       
        else return 0;      
    }

    for(i=0;i<(Length/2);i++) {
      if(Password[i] == 0x00) break;
        switch(Password[i]) {
          case 0x2a: 
            //
            //Lshift pressed
            //
            ShiftPress=ShiftPress?FALSE:TRUE;
            break;
          case 0x3a:
            //
            //Capslock pressed
            //
            CaplockPress=CaplockPress?FALSE:TRUE;
            break;
          default:
            for(KeyIndex =0 ;KeyIndex < (sizeof(KeyDescriptor)/sizeof(KEY_DESCRIPTOR));KeyIndex++ ) {
              if(Password[i] == KeyDescriptor[KeyIndex].ScanCode){
                if(ShiftPress){
                  Password[i-1] = (UINT8)KeyDescriptor[KeyIndex].ShiftedUnicode;
                  ShiftPress=ShiftPress?FALSE:TRUE;
                } else if(CaplockPress){
                  Password[i-1] = (UINT8)KeyDescriptor[KeyIndex].ShiftedUnicode;
                } else{
                  Password[i] = (UINT8)KeyDescriptor[KeyIndex].Unicode;                    
                }
                Count++;
                break;
              }
            }
            break;
        }
    }
    Length = Count;
  }

  ZeroMem(HashArray, sizeof(HashArray));
  Sha256Hash(Password, Length, HashArray, sizeof(HashArray));
  CopyMem(PasswordArray + Len, HashArray, sizeof(HashArray));

  ZeroMem(HashArray, sizeof(HashArray));
  Sha256Hash(PasswordArray, Len + sizeof(HashArray), HashArray, sizeof(HashArray));

  Buf = NULL;
  Buf = AllocateZeroPool(SYSTEM_PASSWORD_HASH_LENGTH + 1);
  ASSERT (Buf != NULL);
  CopyMem(Buf, HashArray, SYSTEM_PASSWORD_HASH_LENGTH);

  *Hash = Buf;
  return SYSTEM_PASSWORD_HASH_LENGTH;
}

EFI_STATUS
ReadSysPassword(
  IN  PASSWORD_TYPE    Type,
  OUT UINT8    **Password
  )
{
  EFI_STATUS    Status;
  UINTN             VariableSize;
  EFI_GUID        PasswordGuid = SYSTEM_PASSWORD_GUID;
  SYSTEM_PASSWORD  SetupPassword;
  UINT8             *PasswordBuf = NULL;

  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &PasswordGuid,
                  NULL,
                  &VariableSize,
                  &SetupPassword
                  );
  if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
  }

  switch (Type) {
  case PD_ADMIN:
    PasswordBuf = AllocateCopyPool (SYSTEM_PASSWORD_HASH_LENGTH, &SetupPassword.AdminHash);

    break;
  case PD_POWER_ON:
    PasswordBuf = AllocateCopyPool (SYSTEM_PASSWORD_HASH_LENGTH, &SetupPassword.PowerOnHash);
    break;
  default:
    return EFI_INVALID_PARAMETER;
  }

  *Password = PasswordBuf;
  return EFI_SUCCESS;
}

BOOLEAN
CheckRepeatPsd(
  IN  PASSWORD_TYPE    Type,
  IN  CHAR16    *String
  )
{
  UINTN    Index;
  UINTN    Len;
  UINT8    *Str;
  UINT8    *InputHash;
  UINT8    *PasswordHash;

  Len = Unicode2Ascii(String, &Str);
  if (0 < Len) {
    EncodePsd(Str, Len, &InputHash,0);
    FreePool (Str);
    ReadSysPassword(Type, &PasswordHash);
    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (InputHash[Index] != PasswordHash[Index] ) {
        FreePool (PasswordHash);
        FreePool (InputHash);
        return FALSE;
      }
    }

    FreePool (InputHash);
    FreePool (PasswordHash);
  } else {
    ReadSysPassword(Type, &PasswordHash);
    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (0 != PasswordHash[Index] ) {
        FreePool (PasswordHash);
        return FALSE;
      }
    }
  }
  return TRUE;
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
        if(((Password[i] >= '!') && (Password[i] <= '/')) ||
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
  if((i == TRUE) && (j == TRUE) && (k == TRUE) && (n == TRUE)) {
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
  SYSTEM_PASSWORD           PasswordRecorder;
  UINTN                     Len;
  UINT8                     *Str = NULL;
  UINT8                     *PasswordHash = NULL;
  UINTN                     Index;
  UINT8                     ReCount = 0;
  BYO_SMIFLASH_PROTOCOL     *ByoSmiFlash;
  UINT8                     *PswRecordAddr;

  Len = Unicode2Ascii(Password, &Str);
  if (Len > 0) {
    EncodePassword(Str, Len, &PasswordHash, 0);
  } else {
    return 0;
  }

  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &PasswordRecorder
                  );

  switch (Type) {
    case SEC_KEY_ADMIN_PD:
      PswRecordAddr = PasswordRecorder.AdminRecords.FirstHashData;
      for (Index = 0; Index < 5; Index++) { 
        if (CompareMem (PswRecordAddr, PasswordHash, SYSTEM_PASSWORD_HASH_LENGTH) == 0)
        {
          FreePool (Str);
          FreePool (PasswordHash);
          return 1;
       }
        PswRecordAddr += SYSTEM_PASSWORD_HASH_LENGTH;
      }
    break;

  case SEC_KEY_POWER_ON_PD:
    PswRecordAddr = PasswordRecorder.PowOnRecords.FirstHashData;
    for (Index = 0; Index < 5; Index++) { 
      if (CompareMem (PswRecordAddr, PasswordHash, SYSTEM_PASSWORD_HASH_LENGTH) == 0)
      {
        FreePool (Str);
        FreePool (PasswordHash);
        return 1;
      }
      PswRecordAddr += SYSTEM_PASSWORD_HASH_LENGTH;
    }
    break;

  default:
     break;
  }

  if (Type == SEC_KEY_ADMIN_PD) {
      CopyMem(PasswordRecorder.AdminRecords.FirstHashData, PasswordRecorder.AdminRecords.SecondHashData, SYSTEM_PASSWORD_HASH_LENGTH);
      CopyMem(PasswordRecorder.AdminRecords.SecondHashData, PasswordRecorder.AdminRecords.ThirdHashData, SYSTEM_PASSWORD_HASH_LENGTH);
      CopyMem(PasswordRecorder.AdminRecords.ThirdHashData, PasswordRecorder.AdminRecords.FourthHashData, SYSTEM_PASSWORD_HASH_LENGTH);
      CopyMem(PasswordRecorder.AdminRecords.FourthHashData,PasswordRecorder.AdminRecords.FifthHashData,SYSTEM_PASSWORD_HASH_LENGTH);
      CopyMem(PasswordRecorder.AdminRecords.FifthHashData,PasswordHash,SYSTEM_PASSWORD_HASH_LENGTH);  
      FreePool (Str);
      FreePool (PasswordHash);
  } else {
      CopyMem(PasswordRecorder.PowOnRecords.FirstHashData, PasswordRecorder.PowOnRecords.SecondHashData, SYSTEM_PASSWORD_HASH_LENGTH);
      CopyMem(PasswordRecorder.PowOnRecords.SecondHashData, PasswordRecorder.PowOnRecords.ThirdHashData, SYSTEM_PASSWORD_HASH_LENGTH);
      CopyMem(PasswordRecorder.PowOnRecords.ThirdHashData, PasswordRecorder.PowOnRecords.FourthHashData, SYSTEM_PASSWORD_HASH_LENGTH);
      CopyMem(PasswordRecorder.PowOnRecords.FourthHashData,PasswordRecorder.PowOnRecords.FifthHashData,SYSTEM_PASSWORD_HASH_LENGTH);
      CopyMem(PasswordRecorder.PowOnRecords.FifthHashData,PasswordHash,SYSTEM_PASSWORD_HASH_LENGTH);  
      FreePool (Str);
      FreePool (PasswordHash);
  }

  Status = gRT->SetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  VariableSize,
                  &PasswordRecorder
                  );

       gBS->LocateProtocol (
              &gByoSmiFlashProtocolGuid,
              NULL,
              (VOID**)&ByoSmiFlash
              );
       ByoSmiFlash->SaveSetupPassword(ByoSmiFlash);
  return 0;
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
  UINT8                         EnterType;

  DEBUG((EFI_D_INFO, "\n PasswordFormCallback, Action :0x%X, KeyValue :0x%x, Type :0x%X.\n", Action, KeyValue, Type));
  if (Action != EFI_BROWSER_ACTION_CHANGING) {
    return EFI_SUCCESS;
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
  EnterType = SystemPassword->GetEnteredType();

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
          if (SystemPassword->Verify(PD_ADMIN, Password)) {
            PasswordState = BROWSER_STATE_SET_PASSWORD;
            Status = EFI_SUCCESS;
          } else {
            Status = EFI_NOT_READY;
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
              break;
            }
			if (CheckRepeatPsd(PD_POWER_ON, Password)) {
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_CONFIRM_RULE_ERROR), NULL);
              UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              break;
            }
            PwdState = CheckPasswordRecord (KeyValue, Password);
            if (PwdState) {
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_PASSWORD_TIMES), NULL);
              UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              break;
            }
            SystemPassword->Write(PD_ADMIN, Password);
            PasswordVar.bHaveAdmin = 1;
            *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
            SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
            String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_SET_PASSWORD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
          } else {
            SystemPassword->Clear(PD_ADMIN);
			SystemPassword->Clear(PD_POWER_ON);
            PasswordVar.bHaveAdmin = 0;
			PasswordVar.bHavePowerOn = 0;
            PasswordVar.ChangePopByUser = 1;
            PasswordVar.RequirePopOnRestart =0;
            PasswordVar.RebootTime=60;
            *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
            SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
            String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_CLEAR_PD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
          }
          PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
          break;

        default:
          PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
          break;
      }
      break;

    case SEC_KEY_POWER_ON_PD:
	 if (PasswordVar.bHaveAdmin == 0) {
		 String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_ADMIN_FIRST), NULL);
		 UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
		 PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
		 Status = EFI_NOT_AVAILABLE_YET;
		 break;
	  }		
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
          if (SystemPassword->Verify(PD_POWER_ON, Password)) {
            PasswordState = BROWSER_STATE_SET_PASSWORD;
            Status = EFI_SUCCESS;
          } else {
            Status = EFI_NOT_READY;
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
              break;
            }
            if (CheckRepeatPsd(PD_ADMIN, Password)) {
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_CONFIRM_RULE_ERROR), NULL);
              UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              break;
            }
            PwdState = CheckPasswordRecord (KeyValue, Password);
            if (PwdState) {
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_PASSWORD_TIMES), NULL);
              UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
              PasswordState = BROWSER_STATE_VALIDATE_PASSWORD;
              break;
            }
            SystemPassword->Write(PD_POWER_ON, Password);
            PasswordVar.bHavePowerOn = 1;
            *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
            SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
            String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_SET_PASSWORD), NULL);
            UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
          } else {
		  	if(EnterType == LOGIN_USER_POP){    //XCL+ USER can not delete user password
		      SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_CANNOT_DELETE), NULL);
              UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
			}else{
              SystemPassword->Clear(PD_POWER_ON);
              PasswordVar.bHavePowerOn = 0;
              if(!PasswordVar.bHaveAdmin) {
                PasswordVar.EnteredType = LOGIN_USER_ADMIN;
              }
              *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
              SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
              String = HiiGetString(SetupFormSet->HiiHandle, STRING_TOKEN(STR_CLEAR_PD), NULL);
              UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
			}
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
