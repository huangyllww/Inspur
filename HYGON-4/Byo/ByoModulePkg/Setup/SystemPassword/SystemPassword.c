/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPassword.c

Abstract:
  Implementation of basic setup password function.

Revision History:

**/
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>

#include <SystemPasswordVariable.h>
#include <Library/SystemPasswordLib.h>
#include <Protocol/SystemPasswordProtocol.h>
#include <Protocol/ByoFormSetManager.h>
#include <Protocol/SetupItemUpdate.h>



VERIFY_MODE    gVerifyMode = VERIFY_MODE_MAX;
BOOLEAN    bPasswordChecked = FALSE;

UINT8      *mInitialPasswordHash = NULL;

BOOLEAN 
Leap_Year(UINT16 Year)
{
  return ((Year % 400 == 0) || ((Year % 4 == 0) && (Year % 100 != 0)))? 1 : 0;
}

UINT32 
Year_To_Day(UINT16 Year_Begin, UINT16 Year_End)
{
  UINT32 Sum_Days_Year = 0;
  UINT16 Year = Year_Begin;

  while(Year != Year_End) {
    Sum_Days_Year = Sum_Days_Year + 365 + Leap_Year(Year_Begin);
    Year++;
  }
  return Sum_Days_Year;
}


UINT32 Month_To_Day(PASSWOR_TIME *PassSetTime)
{
  UINT8  i;
  UINT32 Sum_Days_Month = 0;
  UINT8  Month[12] = {31,28,31,30,31,30,31,31,30,31,30,31}; 

  for(i = 1; i < PassSetTime->Month; i++){
    Sum_Days_Month = Sum_Days_Month + Month[i-1];
  }
  return (PassSetTime->Month > 2)? Sum_Days_Month + PassSetTime->Day + Leap_Year(PassSetTime->Year) : Sum_Days_Month + PassSetTime->Day;
}

UINT32
CalculateDayPeriod(PASSWOR_TIME *TimeBegin, PASSWOR_TIME *TimeEnd)
{
  return Year_To_Day(TimeBegin->Year, TimeEnd->Year) - Month_To_Day(TimeBegin) + Month_To_Day(TimeEnd);
}

void
CheckPwdValidDays (
  IN  PASSWORD_TYPE    Type,
  OUT VERIFY_RESULT    *Result
  )
{
  UINTN                         Size;
  EFI_TIME                      EfiTime;
  PASSWOR_TIME                  *PwdTime;
  SYSTEM_PASSWORD               SetupPassword;
  UINT32                        DayPeriod;
  UINT8                         PasswordNearExpirationDays = 0;

  Size = sizeof(SYSTEM_PASSWORD);
  gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &Size,
                  &SetupPassword
                  );
  if (SetupPassword.PasswordValidDays == 0) {
    return;
  }

  if (Type == PD_ADMIN) {
    PwdTime = &SetupPassword.AdmPwdTime;
  } else {
    PwdTime = &SetupPassword.PopPwdTime;
  }

  gRT->GetTime (&EfiTime, NULL);
  DayPeriod = CalculateDayPeriod(PwdTime, (PASSWOR_TIME*)&EfiTime);
  if (DayPeriod > SetupPassword.PasswordValidDays) {
    *Result = VERIFY_EXPIRE;
  } else if (DayPeriod == SetupPassword.PasswordValidDays) {
    if (PwdTime->Hour < EfiTime.Hour) {
      *Result = VERIFY_EXPIRE;
    } else if (PwdTime->Hour == EfiTime.Hour) {
      if (PwdTime->Minute < EfiTime.Minute) {
        *Result = VERIFY_EXPIRE;
      } else if (PwdTime->Minute == EfiTime.Minute) {
        if (PwdTime->Second < EfiTime.Second) {
          *Result = VERIFY_EXPIRE;
        }
      }
    }
  }
  if (PcdGet8(PcdPasswordReminderDays) == 0) {
    return;
  }
  if (DayPeriod <= SetupPassword.PasswordValidDays && *Result != VERIFY_EXPIRE) {
    PasswordNearExpirationDays = SetupPassword.PasswordValidDays - (UINT8)DayPeriod;
    if (PasswordNearExpirationDays <= PcdGet8(PcdPasswordReminderDays) && PasswordNearExpirationDays > 0) {
        *Result = VERIFY_EXPIRE_SOON;
    } else if (PasswordNearExpirationDays == 0) {
      if (PwdTime->Hour > EfiTime.Hour) {
        *Result = VERIFY_EXPIRE_SOON;
        PasswordNearExpirationDays = 1;
      }
    }
  }
  PcdSet8S(PcdPasswordNearExpirationDays, PasswordNearExpirationDays);
  return;
}

VOID
SetUnicodeMem (
  IN VOID   *Buffer,
  IN UINTN  Size,
  IN CHAR16 Value
  )
{
  CHAR16  *Ptr;

  Ptr = Buffer;
  while ((Size--)  != 0) {
    *(Ptr++) = Value;
  }
}



EFI_STATUS  
GetPasswordKey (  
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL    *ConInEx,
  EFI_INPUT_KEY                        *Key,
  EFI_EVENT                            TimerEvent
  )
{
  EFI_STATUS      Status;
  EFI_KEY_DATA    KeyData;
  UINT32          KeyShiftState;
  UINTN           Index;
  EFI_EVENT       WaitEventList[2];
  BOOLEAN         CursorFlag = TRUE;

  
  WaitEventList[0] = ConInEx->WaitForKeyEx;
  WaitEventList[1] = TimerEvent; 

  while(1){ 
    Status = gBS->WaitForEvent(2, WaitEventList, &Index);
    if(Index == 0){
      break;
    } else if(Index == 1){
      gST->ConOut->EnableCursor(gST->ConOut, CursorFlag);
      CursorFlag = !CursorFlag;
    }
  }
  
  Status = ConInEx->ReadKeyStrokeEx(ConInEx, &KeyData);
  if(!EFI_ERROR(Status)){
    Key->ScanCode = KeyData.Key.ScanCode;
    Key->UnicodeChar = KeyData.Key.UnicodeChar;		

    KeyShiftState = KeyData.KeyState.KeyShiftState;
    if(KeyShiftState & EFI_SHIFT_STATE_VALID){
      if(KeyShiftState & (EFI_RIGHT_ALT_PRESSED | 
                        EFI_LEFT_ALT_PRESSED | 
                        EFI_RIGHT_CONTROL_PRESSED | 
                        EFI_LEFT_CONTROL_PRESSED)) {
        Key->ScanCode = SCAN_NULL;
        Key->UnicodeChar = CHAR_NULL;	
        Status = EFI_UNSUPPORTED;
      }                      
    }
  }
  
  return Status;
}

/**
  Input Password from KB.

**/
EFI_STATUS
InputPassword  (
  IN UINTN               Column,
  IN UINTN               Row,
  IN OUT CHAR16          *StringPtr
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINTN    CurrentCursor;
  BOOLEAN    CursorVisible;
  EFI_INPUT_KEY    Key;
  CHAR16    *BufferedString;
  CHAR16    *TempString;
  UINTN    Index;
  UINTN    Index2;
  UINTN    Count;
  UINTN    StringLen;
  UINTN    PreStringLen;
  UINTN    PasswordColor;
  CHAR16    KeyPad[2];
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *ConOut;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL  *ConInEx; 
  EFI_EVENT                          TimerEvent;


  ZeroMem(StringPtr, StrSize(StringPtr));
//DMSG ((EFI_D_ERROR, "\n InputPassword, StringPtr :%s.\n", StringPtr));
  
  BufferedString = AllocateZeroPool ((MAX_PASSWORD_LENGTH + 1) * sizeof (CHAR16));
  ASSERT (BufferedString);

  TempString = AllocateZeroPool ((MAX_PASSWORD_LENGTH + 1)* sizeof (CHAR16));
  ASSERT (TempString);

  PasswordColor = EFI_WHITE | EFI_BACKGROUND_BLACK;
  ConOut = gST->ConOut;

  CursorVisible = gST->ConOut->Mode->CursorVisible;
  gST->ConOut->SetCursorPosition (gST->ConOut, Column, Row);
  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  //
  // Clean more input before entering this.
  //
  while (gST->ConIn != NULL) {
    Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);    
    if (EFI_ERROR (Status)) {
       break;
    }
  }  
  
  Status = gBS->HandleProtocol(
                  gST->ConsoleInHandle, 
                  &gEfiSimpleTextInputExProtocolGuid,
                  (VOID**)&ConInEx
                  );
  if(EFI_ERROR(Status)){
    Status = gBS->LocateProtocol(&gEfiSimpleTextInputExProtocolGuid, NULL, (VOID**)&ConInEx);
    if(EFI_ERROR(Status)){
      return Status;
    }
  }

  Status = gBS->CreateEvent(EVT_TIMER, 0, NULL, NULL, &TimerEvent);
  if(EFI_ERROR (Status)){
    return Status;
  }
  gBS->SetTimer (
         TimerEvent,
         TimerPeriodic,
         10000*300
         );
  
  CurrentCursor = 0;
  PreStringLen = 0;
  do {
    Status = GetPasswordKey(ConInEx, &Key, TimerEvent);
    if (EFI_ERROR (Status)) {
      continue;
    }

    DMSG ((EFI_D_ERROR, "\n InputPassword, Key :%c-%d.\n", Key.UnicodeChar, Key.ScanCode));	
    gST->ConOut->SetAttribute (gST->ConOut, PasswordColor);
    switch (Key.UnicodeChar) {
    case CHAR_NULL:
      switch (Key.ScanCode) {
      case SCAN_LEFT:
        if (CurrentCursor > 0) {
          CurrentCursor--;
        }
        break;

      case SCAN_RIGHT:
        if (CurrentCursor < (StrLen (StringPtr))) {
          CurrentCursor++;
        }
        break;

      default:
        break;
      }
      break;

    case CHAR_CARRIAGE_RETURN:
      DMSG ((EFI_D_ERROR, "InputPassword, CHAR_CARRIAGE_RETURN, StringPtr :%s.\n", StringPtr));
      if (StrLen (StringPtr) >= sizeof (CHAR16)) {

        FreePool (TempString);
        FreePool (BufferedString);
        gST->ConOut->SetAttribute (gST->ConOut, PasswordColor);
        gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
        return EFI_SUCCESS;
      }
      if (!PcdGetBool (PcdAllowEmptySystemPassword) && *StringPtr == CHAR_NULL) {
        continue;
      }
	  
      FreePool (TempString);
      FreePool (BufferedString);
      gST->ConOut->SetAttribute (gST->ConOut, PasswordColor);
      gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
      return EFI_DEVICE_ERROR;
      break;

    case CHAR_BACKSPACE:
      DMSG ((EFI_D_ERROR, "InputPassword, CHAR_BACKSPACE, CurrentCursor :%d.\n", CurrentCursor));
      if (StringPtr[0] != CHAR_NULL && CurrentCursor != 0) {
        for (Index = 0; Index < CurrentCursor - 1; Index++) {
          TempString[Index] = StringPtr[Index];
        }
        Count = StrLen (StringPtr);
        if (Count >= CurrentCursor) {
          for (Index = CurrentCursor - 1, Index2 = CurrentCursor; Index2 < Count; Index++, Index2++) {
            TempString[Index] = StringPtr[Index2];
          }
          TempString[Index] = CHAR_NULL;
        }
        //
        // Effectively truncate string by 1 character
        //
        StrCpyS (StringPtr, Count + 1, TempString);
        CurrentCursor --;
      }

    default:
      //
      // Check invalid char.
      //
      if (!IsValidPdKey(&Key) && Key.UnicodeChar != CHAR_BACKSPACE) {
        break;
      }

      //
      // Add new char.
      //
      StringLen = StrLen (StringPtr);
      if (CHAR_BACKSPACE == Key.UnicodeChar) {
        if (StringPtr[0] == CHAR_NULL) {
          PreStringLen = 0;
          gST->ConOut->EnableCursor (gST->ConOut, FALSE);
          ConOut->SetCursorPosition(ConOut, Column, Row);
          ConOut->OutputString(ConOut, L" ");
          break;
        }
      } else if (StringPtr[0] == CHAR_NULL) {
        StrnCpyS (StringPtr, MAX_PASSWORD_LENGTH + 1, &Key.UnicodeChar, 1);
        StringLen = 1;
        CurrentCursor++;
      } else if (StringLen < MAX_PASSWORD_LENGTH) {
        KeyPad[0] = Key.UnicodeChar;
        KeyPad[1] = CHAR_NULL;
        Count = StringLen;
        if (CurrentCursor < Count) {
          for (Index = 0; Index < CurrentCursor; Index++) {
            TempString[Index] = StringPtr[Index];
          }
          TempString[Index] = CHAR_NULL;
          StrCatS (TempString, MAX_PASSWORD_LENGTH + 1, KeyPad);
          StrCatS (TempString, MAX_PASSWORD_LENGTH + 1, StringPtr + CurrentCursor);
          StrCpyS (StringPtr, MAX_PASSWORD_LENGTH + 1, TempString);
        } else {
          StrCatS (StringPtr, MAX_PASSWORD_LENGTH + 1, KeyPad);
        }
        CurrentCursor++;
        StringLen++;
      } else {
        DMSG ((EFI_D_ERROR, "InputPassword, Exceed MAX_PASSWORD_LENGTH.\n"));
        continue;
      }

      //
      // Print "*".
      //
      gST->ConOut->EnableCursor (gST->ConOut, FALSE);
      DMSG ((EFI_D_ERROR, "InputPassword, StringLen :%d.\n", StringLen));	  
      if (StringLen > 0 && StringLen > PreStringLen) {
        ConOut->SetCursorPosition(ConOut, Column + StringLen - 1, Row);
        ConOut->OutputString(ConOut, L"*");	  
        PreStringLen = StringLen;
      } else if (StringLen > 0 && StringLen < PreStringLen) {
        ConOut->SetCursorPosition(ConOut, Column + PreStringLen -1, Row);
        ConOut->OutputString(ConOut, L" ");
        PreStringLen = StringLen;	
      }
      break;
    }

    gST->ConOut->SetAttribute (gST->ConOut, PasswordColor);
    gST->ConOut->SetCursorPosition (gST->ConOut, Column + CurrentCursor, Row);
    gST->ConOut->EnableCursor (gST->ConOut, TRUE);
  } while (TRUE);

  gBS->CloseEvent(TimerEvent);

  return Status;
}



EFI_STATUS
ErrorWarning (
  IN  PASSWORD_TYPE   Type,
  OUT VERIFY_RESULT   *Result
  )
{
  EFI_STATUS    Status;
  UINTN    VariableSize;
  EFI_GUID    PasswordGuid = SYSTEM_PASSWORD_GUID;
  SYSTEM_PASSWORD    SetupPassword;

  if (MAX_VERIFY_TIMES == 0) {
    return EFI_NOT_READY;
  }

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

  //
  // Check verify times.
  //
  if (VERIFY_MODE_INTO_POP == gVerifyMode ||
    VERIFY_MODE_INTO_SETUP == gVerifyMode ) {
    if (SetupPassword.VerifyTimes < MAX_VERIFY_TIMES || 
        (SetupPassword.bHaveAdmin && SetupPassword.bHavePowerOn && Type == PD_ADMIN)) {
      return EFI_NOT_READY;
    }
    SetupPassword.VerifyTimes = 0;
  } else if (VERIFY_MODE_SETUP == gVerifyMode) {
    if (PD_ADMIN == Type) {
      if (SetupPassword.VerifyTimesAdmin < MAX_VERIFY_TIMES ) {
        return EFI_NOT_READY;
      }
      SetupPassword.VerifyTimesAdmin = 0;
    } else if (PD_POWER_ON == Type) {
      if (SetupPassword.VerifyTimesPop < MAX_VERIFY_TIMES ) {
        return EFI_NOT_READY;
      }
      SetupPassword.VerifyTimesPop = 0;
    }
  } else if (VERIFY_MODE_HDP== gVerifyMode) {
    return EFI_NOT_READY;  // transfer error handing to HDP.
  } else {
    return EFI_NOT_READY;
  }


  //
  // 1. Clear VerifyTimes.
  //
  gRT->SetVariable (
                SYSTEM_PASSWORD_NAME,
                &gEfiSystemPasswordVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                VariableSize,
                &SetupPassword
                );

  //
  // 2. Warning dialog.
  //
  if (Result != NULL) {
    *Result = VERIFY_WRONG;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
ReadPassword(
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

EFI_STATUS
WritePassword(
  IN  PASSWORD_TYPE    Type,
  IN  CHAR16    *Password
  )
{
  EFI_STATUS    Status;
  UINTN    VariableSize;
  EFI_GUID    PasswordGuid = SYSTEM_PASSWORD_GUID;
  SYSTEM_PASSWORD    SetupPassword;
  UINTN    Len;
  UINT8    *Str;
  UINT8    *PasswordHash;
  EFI_TIME          EfiTime;
  UINTN    Count;

  Status = gRT->GetTime (&EfiTime, NULL);
  if (EFI_ERROR (Status)) {
    ZeroMem (&EfiTime, sizeof (EFI_TIME));
  }

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

  //
  // After initial password set in SystemPasswordInit(), if WritePassword() is called again,
  // and if password type is admin, check mInitialPasswordHash first and decide if need update password.
  //
  if (Type == PD_ADMIN && mInitialPasswordHash != NULL) {
    if (CompareMem (mInitialPasswordHash, &SetupPassword.AdminHash[0], SYSTEM_PASSWORD_HASH_LENGTH) != 0) {
      FreePool (mInitialPasswordHash);
      mInitialPasswordHash = NULL;
      return EFI_ALREADY_STARTED;
    } else {
      FreePool (mInitialPasswordHash);
      mInitialPasswordHash = NULL;
    }
  }

  switch (Type) {
    case PD_ADMIN:
      Len = Unicode2Ascii(Password, &Str);
      if (0 < Len ) {
        Count = EncodePassword(Str, Len, &PasswordHash,0);
        if (Count == 0) {
          return EFI_INVALID_PARAMETER;
        }
        CopyMem(&SetupPassword.AdminHash, PasswordHash, SYSTEM_PASSWORD_HASH_LENGTH);
        SetupPassword.bHaveAdmin = TRUE;
        CopyMem(&SetupPassword.AdmPwdTime, &EfiTime, sizeof (PASSWOR_TIME));
        FreePool (Str);
        FreePool (PasswordHash);
      } else {
        ZeroMem(&SetupPassword.AdminHash, SYSTEM_PASSWORD_HASH_LENGTH);
        SetupPassword.bHaveAdmin = FALSE;
      }
      break;

    case PD_POWER_ON:
      Len = Unicode2Ascii(Password, &Str);
      if (0 < Len ) {
        Count = EncodePassword(Str, Len, &PasswordHash,0);
        if (Count == 0) {
          return EFI_INVALID_PARAMETER;
        }
        CopyMem(&SetupPassword.PowerOnHash, PasswordHash, SYSTEM_PASSWORD_HASH_LENGTH);
        SetupPassword.bHavePowerOn = TRUE;
        CopyMem(&SetupPassword.PopPwdTime, &EfiTime, sizeof (PASSWOR_TIME));
        FreePool (Str);
        FreePool (PasswordHash);
      } else {
        ZeroMem(&SetupPassword.PowerOnHash, SYSTEM_PASSWORD_HASH_LENGTH);
        SetupPassword.bHavePowerOn = FALSE;
      }
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }

  SetupPassword.VerifyTimes = 0;

  Status = gRT->SetVariable (
                    SYSTEM_PASSWORD_NAME,
                    &gEfiSystemPasswordVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VariableSize,
                    &SetupPassword
                    );

  return Status;
}

EFI_STATUS
SetVerifyMode (
  IN VERIFY_MODE    Mode
  )
{
  if (VERIFY_MODE_MAX > Mode) {
    gVerifyMode = Mode;
  } else {
    gVerifyMode = VERIFY_MODE_MAX;
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

BOOLEAN
GetPopCheckedStatus (
  VOID
  )
{
  return bPasswordChecked;
}

EFI_STATUS
SetVerifyTimes(
  IN  PASSWORD_TYPE    Type,
  IN VERIFY_TIMES_ACTION    Action
  )
{
  EFI_STATUS    Status;
  UINTN    VariableSize;
  EFI_GUID    PasswordGuid = SYSTEM_PASSWORD_GUID;
  SYSTEM_PASSWORD    SetupPassword;

  if (MAX_VERIFY_TIMES == 0) {
    return EFI_NOT_READY;
  }

  if (VERIFY_MODE_MAX == gVerifyMode) {
    return EFI_NOT_READY;
  }

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

  switch (gVerifyMode) {
    case VERIFY_MODE_INTO_POP:
    case VERIFY_MODE_INTO_SETUP:
      switch (Action) {
        case VERIFY_ACTION_ADD:
          if (SetupPassword.VerifyTimes < (MAX_VERIFY_TIMES)) {
            SetupPassword.VerifyTimes += 1;
          } else {
            SetupPassword.VerifyTimes = 0;
          }
          break;

        case VERIFY_ACTION_SUB:
          if (SetupPassword.VerifyTimes > 0) {
           SetupPassword.VerifyTimes -= 1;
          } else {
           SetupPassword.VerifyTimes = 0;
          }
          break;

        case VERIFY_ACTION_RESET:
          SetupPassword.VerifyTimes = 0;
          break;

        default:
          return EFI_INVALID_PARAMETER;
      }
      break;
	  
    case VERIFY_MODE_HDP:
      //
      // Not change verify times when it used by HDP.
      //
      break;
	  
    case VERIFY_MODE_SETUP:

      switch (Action) {
        case VERIFY_ACTION_ADD:
          if (PD_ADMIN == Type) {
            if (SetupPassword.VerifyTimesAdmin < (MAX_VERIFY_TIMES)) {
              SetupPassword.VerifyTimesAdmin += 1;
            } else {
              SetupPassword.VerifyTimesAdmin = 0;
            }
          }
          if (PD_POWER_ON == Type) {
            if (SetupPassword.VerifyTimesPop < (MAX_VERIFY_TIMES)) {
              SetupPassword.VerifyTimesPop += 1;
            } else {
              SetupPassword.VerifyTimesPop = 0;
            }
          }
          break;

        case VERIFY_ACTION_SUB:
          if (PD_ADMIN == Type) {
            if (SetupPassword.VerifyTimesAdmin > 0) {
             SetupPassword.VerifyTimesAdmin -= 1;
            } else {
             SetupPassword.VerifyTimesAdmin = 0;
            }
          }
          if (PD_POWER_ON == Type) {
            if (SetupPassword.VerifyTimesPop > 0) {
             SetupPassword.VerifyTimesPop -= 1;
            } else {
             SetupPassword.VerifyTimesPop = 0;
            }
          }
          break;

        case VERIFY_ACTION_RESET:
          if (PD_ADMIN == Type) {
            SetupPassword.VerifyTimesAdmin = 0;
          }
          if (PD_POWER_ON == Type) {
            SetupPassword.VerifyTimesPop = 0;
          }
          break;

        default:
          return EFI_INVALID_PARAMETER;
      }

      break;

    default:
      return EFI_NOT_READY;
  }

  Status = gRT->SetVariable (
                    SYSTEM_PASSWORD_NAME,
                    &gEfiSystemPasswordVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VariableSize,
                    &SetupPassword
                    );

  return Status;
}


UINT8
GetVerifyTimes(
  IN  PASSWORD_TYPE    Type
  )
{
  EFI_STATUS    Status;
  UINTN    VariableSize;
  EFI_GUID    PasswordGuid = SYSTEM_PASSWORD_GUID;
  SYSTEM_PASSWORD    SetupPassword;
  UINT8    Times = (UINT8) -1;

  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &PasswordGuid,
                  NULL,
                  &VariableSize,
                  &SetupPassword
                  );
  if (!EFI_ERROR (Status)) {
    switch (gVerifyMode) {
      case VERIFY_MODE_INTO_POP:
      case VERIFY_MODE_INTO_SETUP:
        Times = SetupPassword.VerifyTimes;
        break;
  	  
      case VERIFY_MODE_HDP:
        Times = 0;
        break;
  	  
      case VERIFY_MODE_SETUP:
        if (PD_ADMIN == Type) {
          Times = SetupPassword.VerifyTimesAdmin;
        }
        if (PD_POWER_ON == Type) {
          Times = SetupPassword.VerifyTimesPop;
        }
        break;
  
      default:
        break;
    }
  }
  
  return Times;
}

BOOLEAN
BeHavePassword(
  IN  PASSWORD_TYPE    Type
  )
{
  EFI_STATUS    Status;
  UINT8    *PasswordBuf = NULL;
  UINTN    Index;
  BOOLEAN    bHave;

  bHave = FALSE;
  Status = ReadPassword(Type, &PasswordBuf);
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (0 != PasswordBuf[Index]) {
        bHave = TRUE;
        break;
        }
    }
  }

  if (NULL != PasswordBuf) FreePool(PasswordBuf);
  return bHave;
}


BOOLEAN
VerifyPassword(
  IN  PASSWORD_TYPE    Type,
  IN  CHAR16           *String,
  OUT VERIFY_RESULT    *Result
  )
{
  UINTN    Index;
  UINTN    Len;
  UINT8    *Str;
  UINT8    *InputHash;
  UINT8    *PasswordHash;
  UINTN    Count;
  EFI_STATUS Status;

  if (mInitialPasswordHash != NULL) {
    //
    // Initial password set phase 2
    //
    if ((PcdGetSize(PcdInitialSystemPassword)) > 2) {
      //
      //if pcd PcdInitialSystemPassword have Valid string.
      //
      WritePassword(PD_ADMIN, (CHAR16*)PcdGetPtr(PcdInitialSystemPassword));
    }
  }

  if (Result != NULL) {
    *Result = VERIFY_PASS;
  }

  Len = Unicode2Ascii(String, &Str);
  if (0 < Len) {
    Count = EncodePassword(Str, Len, &InputHash,0);
    if (Count == 0) {
      return FALSE;
    }
    FreePool (Str);

    Status = ReadPassword(Type, &PasswordHash);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (InputHash[Index] != PasswordHash[Index] ) {
        FreePool (PasswordHash);
        FreePool (InputHash);
        SetVerifyTimes(Type, VERIFY_ACTION_ADD);
        ErrorWarning (Type, Result);
        return FALSE;
      }
    }

    FreePool (InputHash);
    FreePool (PasswordHash);
  } else {

    Status = ReadPassword(Type, &PasswordHash);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }
    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (0 != PasswordHash[Index] ) {
        FreePool (PasswordHash);
        return FALSE;
      }
    }
  }

  SetVerifyTimes(Type, VERIFY_ACTION_RESET);
  bPasswordChecked = TRUE;

  if (Result != NULL) {
    CheckPwdValidDays (Type, Result);
  }

  return TRUE;
}


EFI_STATUS
ClearPassword(
  IN  PASSWORD_TYPE    Type
  )
{
  EFI_STATUS    Status;
  UINTN             VariableSize;
  SYSTEM_PASSWORD  SetupPassword;

  bPasswordChecked = FALSE;

  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupPassword
                  );
  if (EFI_ERROR (Status)) {
      return EFI_NOT_FOUND;
  }

  switch (Type) {
  case PD_ADMIN:
    SetupPassword.bHaveAdmin = FALSE;
    ZeroMem(&SetupPassword.AdminHash, SYSTEM_PASSWORD_HASH_LENGTH);
    SetupPassword.ChangePopByUser = 1;
    break;
  case PD_POWER_ON:
    SetupPassword.bHavePowerOn = FALSE;
    ZeroMem(&SetupPassword.PowerOnHash, SYSTEM_PASSWORD_HASH_LENGTH);
//- SetupPassword.RequirePopOnRestart = 0;
    break;
  case PD_MAX:
    SetupPassword.bHaveAdmin = FALSE;
    ZeroMem(&SetupPassword.AdminHash, SYSTEM_PASSWORD_HASH_LENGTH);
    SetupPassword.bHavePowerOn = FALSE;
    ZeroMem(&SetupPassword.PowerOnHash, SYSTEM_PASSWORD_HASH_LENGTH);
//- SetupPassword.RequirePopOnRestart = 0;
    SetupPassword.ChangePopByUser = 1;
    break;
  default:
    return EFI_INVALID_PARAMETER;
  }

  SetupPassword.VerifyTimes = 0;
  if (PcdGet8 (PcdIsClearSystemPwd)) {
    SetupPassword.PasswordValidDays = 0;
    SetupPassword.RequirePopOnRestart = 0;
    SetupPassword.HashType = PASSWORD_HASH_SHA256;
  }

  if (!SetupPassword.bHaveAdmin && !SetupPassword.bHavePowerOn) {
    SetupPassword.EnteredType = LOGIN_USER_ADMIN;
  }

  Status = gRT->SetVariable (
                    SYSTEM_PASSWORD_NAME,
                    &gEfiSystemPasswordVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VariableSize,
                    &SetupPassword
                    );

  return Status;
}

UINT8
GetEnteredType (
  VOID
  )
{
  EFI_STATUS    Status;
  UINTN    VariableSize;
  SYSTEM_PASSWORD    SetupPassword;

  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupPassword
                  );
  if (!EFI_ERROR (Status)) {
    return SetupPassword.EnteredType;    
  } else {
    return LOGIN_USER_MAX;
  }
}

EFI_STATUS
SetSaltBuffer (
  IN UINT8    *SaltBuffer,
  IN UINTN    Length
  )
{
  EFI_STATUS              Status;
  UINTN                   Index;
  UINTN                   Size;
  SYSTEM_PASSWORD         PasswordVariable; 
  UINT8                   SaltHashArray[32];

  Size = sizeof (PasswordVariable);
  Status =  gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &Size,
                  &PasswordVariable
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  ZeroMem(PasswordVariable.Admin, sizeof (PasswordVariable.Admin));
  ZeroMem(PasswordVariable.PowerOn, sizeof(PasswordVariable.PowerOn));
  Sha256HashAll (SaltBuffer, Length, SaltHashArray);

  for (Index = 0; (Index < UNIQUE_SYSTEM_NUMBER_LENGTH) && (Index < Length); Index++) {
    PasswordVariable.Admin[Index] = (UINT16 ) (SaltHashArray[Index] / 2);
    PasswordVariable.PowerOn[Index] = (UINT16 ) (SaltHashArray[Index] - SaltHashArray[Index] / 2);
  }

  Status = gRT->SetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Size,
                  &PasswordVariable
                  );

  return Status;
}

BOOLEAN
RandomSeedGenerate (
  UINT16  *Buffer,
  UINTN   Number
  )
{
  UINTN    Index;

  for (Index = 0; Index < Number; Index++) {	
    if (Buffer[Index] != 0) {
      break;
    }
  }
  if (Index == Number) {
    //
    // Generate RandomSeed for Admin 
    //
    RandomSeed(NULL, 0);
    for (Index = 0; Index < Number; Index++) {	
      RandomBytes((UINT8 *)(Buffer + Index), sizeof (UINT16));
    }

    return TRUE;
  }

  return FALSE;
}

VOID
EFIAPI
ChangeDefaultPasswordHashType(
  EFI_EVENT                               Event,
  VOID                                    *Context
)
{
  EFI_STATUS                               Status = EFI_SUCCESS;
  SETUP_ITEM_UPDATE_PROTOCOL               *UpdateProtocol;
  SYSTEM_PASSWORD                          SetupPassword;
  UINTN                                    VariableSize;

  Status = gBS->LocateProtocol(&gSetupItemUpdateProtocolGuid, NULL, (VOID **)&UpdateProtocol);
  if (EFI_ERROR(Status)){
    return;
  }
  gBS->CloseEvent(Event);

  //
  // Get password variable.
  //
  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupPassword
                  );

  if (EFI_ERROR(Status)) {
    return;
  } else if( !SetupPassword.bHaveAdmin && !SetupPassword.bHavePowerOn ) {
    return;
  } else{
    UpdateProtocol->UpdateDefaultValue (SYSTEM_PASSWORD_NAME, &gEfiSystemPasswordVariableGuid, OFFSET_OF(SYSTEM_PASSWORD, HashType) , (UINT64)SetupPassword.HashType);
  }

  return;
}

//check integrity of password

/**
  Main entry for this driver.

  @param ImageHandle     Image handle this driver.
  @param SystemTable     Pointer to SystemTable.

  @retval EFI_SUCCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
SystemPasswordInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_SYSTEM_PASSWORD_PROTOCOL    *SystemPassword = NULL;
  EFI_HANDLE    Handle = NULL;
  SYSTEM_PASSWORD    SetupPassword;
  UINT32   SetupLock;
  SYSTEM_PASSWORD_REQUIREMENTS    SystemPasswordRequirement;
  BYO_SYSTEM_PASSWORD_RECORDER    PasswordRecorder;
  UINTN    VariableSize;
  BOOLEAN  InitialSetPassword = FALSE;
  UINTN    Index;
  EFI_EVENT    Event;

  //
  // Check password variable.
  //
  VariableSize = sizeof(SYSTEM_PASSWORD);  
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupPassword
                  );
  DEBUG((EFI_D_INFO, "SystemPasswordInit(), GetVariable(%s):%r-%d\n", SYSTEM_PASSWORD_NAME, Status, VariableSize));	
  if (!EFI_ERROR(Status)) {
    if (PcdGet8 (PcdIsClearSystemPwd)) {
      ClearPassword(PD_MAX);

      VariableSize = sizeof(SYSTEM_PASSWORD);  
      Status = gRT->GetVariable (
                      SYSTEM_PASSWORD_NAME,
                      &gEfiSystemPasswordVariableGuid,
                      NULL,
                      &VariableSize,
                      &SetupPassword
                      );
      DEBUG((EFI_D_INFO, "[clear] GetVariable Again %r\n", Status));  
      
    }
    //
    //Check password status.
    //
    if (SetupPassword.bHaveAdmin != BeHavePassword(PD_ADMIN) ||
        SetupPassword.VerifyTimes > 0 ||
        SetupPassword.VerifyTimesAdmin > 0 ||
        SetupPassword.VerifyTimesPop > 0 ||
        SetupPassword.bHavePowerOn != BeHavePassword(PD_POWER_ON)
      ) {
      SetupPassword.bHaveAdmin = BeHavePassword(PD_ADMIN);
      SetupPassword.bHavePowerOn = BeHavePassword(PD_POWER_ON);
      SetupPassword.VerifyTimes = 0;
      SetupPassword.VerifyTimesAdmin = 0;
      SetupPassword.VerifyTimesPop = 0;
      SetupPassword.EnteredType = LOGIN_USER_ADMIN;

      VariableSize = sizeof(SYSTEM_PASSWORD);
      Status = gRT->SetVariable (
                    SYSTEM_PASSWORD_NAME,
                    &gEfiSystemPasswordVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VariableSize,
                    &SetupPassword
                    );
    }

    //
    // Make sure RandomSeed is created for Admin and PowerOn password
    //
    InitialSetPassword = RandomSeedGenerate (SetupPassword.Admin, ARRAY_SIZE (SetupPassword.Admin));
    RandomSeedGenerate (SetupPassword.PowerOn, ARRAY_SIZE (SetupPassword.PowerOn));
    if (PcdGet8 (PcdIsClearSystemPwd)) {
      InitialSetPassword = TRUE;
    }
    for (Index = 0; Index < SYSTEM_PASSWORD_HASH_LENGTH; Index++) {
      if (0 != SetupPassword.AdminHash[Index]) {
        break;
      }
    }
    if (Index >= SYSTEM_PASSWORD_HASH_LENGTH && (PcdGetSize(PcdInitialSystemPassword)) > 2) {
      InitialSetPassword = TRUE;
    }

    VariableSize = sizeof(SYSTEM_PASSWORD);
    Status = gRT->SetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  VariableSize,
                  &SetupPassword
                  );

    if (InitialSetPassword) {
      //
      // Initial password set phase 1
      //
      if ((PcdGetSize(PcdInitialSystemPassword)) > 2) {
        //
        //if pcd PcdInitialSystemPassword have Valid string.
        //
        WritePassword(PD_ADMIN,(CHAR16*)PcdGetPtr(PcdInitialSystemPassword));
        //
        // Read password hash for comparing after phase 1
        //
        ReadPassword (PD_ADMIN, &mInitialPasswordHash);
        //
        // Record Initial Password 
        //
        ByoCheckPasswordRecord(PD_ADMIN,(CHAR16*)PcdGetPtr(PcdInitialSystemPassword));
      }
    }

  } else if(Status == EFI_NOT_FOUND) {
    //
    // First in, we should set variable.
    //
    VariableSize = sizeof(SYSTEM_PASSWORD);
    ZeroMem(&SetupPassword, VariableSize);
	
    SetupPassword.RequirePopOnRestart = 1;
    SetupPassword.ChangePopByUser = 1;
    SetupPassword.EnteredType = LOGIN_USER_ADMIN;
    
    RandomSeedGenerate (SetupPassword.Admin, ARRAY_SIZE (SetupPassword.Admin));

    RandomSeedGenerate (SetupPassword.PowerOn, ARRAY_SIZE (SetupPassword.PowerOn));
		
    Status = gRT->SetVariable (
                    SYSTEM_PASSWORD_NAME,
                    &gEfiSystemPasswordVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VariableSize,
                    &SetupPassword
                    );
  }

  //
  // Check lock variable.
  //
  VariableSize = sizeof(SetupLock);  
  Status = gRT->GetVariable (
                  FLASH_LOCK_STATUS_NAME,
                  &gEfiFlashLockStatusVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupLock
                  );
  if (Status == EFI_NOT_FOUND){
    SetupLock = 0;
    Status = gRT->SetVariable (
                FLASH_LOCK_STATUS_NAME,
                &gEfiFlashLockStatusVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                VariableSize,
                &SetupLock
                );
  }

  //
  // Check password status variable.
  //
  VariableSize = sizeof(SYSTEM_PASSWORD_REQUIREMENTS);  
  Status = gRT->GetVariable (
                  PASSWORD_REQUIREMENTS,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &SystemPasswordRequirement
                  );
  if (Status == EFI_NOT_FOUND){
    SetMem(&SystemPasswordRequirement, sizeof(SystemPasswordRequirement), 0);
    SystemPasswordRequirement.PasswordLengthMin = FixedPcdGet8 (PcdSystemPasswordMinLength);
    SystemPasswordRequirement.PasswordLengthMax = FixedPcdGet8 (PcdSystemPasswordLength);
    SystemPasswordRequirement.RecoderTime = PcdGet8(PcdRecorderPasswordTime);
    SystemPasswordRequirement.RecoderTimeMax = FixedPcdGet8(PcdPasswordMaxRecorderTime);
    SystemPasswordRequirement.PcdSystemPasswordHashIteration = FixedPcdGetBool(PcdSystemPasswordHashIteration);
    SystemPasswordRequirement.AdminAndUserNotSame = FALSE;
    Status = gRT->SetVariable (
                PASSWORD_REQUIREMENTS,
                &gEfiSystemPasswordVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                VariableSize,
                &SystemPasswordRequirement
                );
  }

  VariableSize = sizeof(BYO_SYSTEM_PASSWORD_RECORDER);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_RECORDER_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  &PasswordRecorder
                  );
  if (Status == EFI_NOT_FOUND){
    ZeroMem(&PasswordRecorder, VariableSize);
    Status = gRT->SetVariable (
                  SYSTEM_PASSWORD_RECORDER_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  VariableSize,
                  &PasswordRecorder
                  );
  }
  
  


  //
  // Install password protocol.
  //
  SystemPassword = AllocateZeroPool(sizeof(EFI_SYSTEM_PASSWORD_PROTOCOL));
  if (SystemPassword == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SystemPassword->Read = ReadPassword;
  SystemPassword->Write = WritePassword;
  SystemPassword->BeHave = BeHavePassword;
  SystemPassword->Verify = VerifyPassword;
  SystemPassword->Clear = ClearPassword;
  SystemPassword->Input =  InputPassword;
  SystemPassword->SetMode = SetVerifyMode;
  SystemPassword->SetTimes = SetVerifyTimes;
  SystemPassword->GetTimes = GetVerifyTimes;
  SystemPassword->GetPopChecked = GetPopCheckedStatus;
  SystemPassword->GetEnteredType = GetEnteredType;
  SystemPassword->SetSaltBuffer = SetSaltBuffer;
  
  Status = gBS->InstallProtocolInterface (
                 &Handle,
                 &gEfiSystemPasswordProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 SystemPassword
                 );
  ASSERT_EFI_ERROR(Status);

  //
  // Check if the default hash type needs to be changed into memory.
  //
  EfiCreateProtocolNotifyEvent (
  &gSetupItemUpdateProtocolGuid,
  TPL_CALLBACK,
  ChangeDefaultPasswordHashType,
  NULL,
  &Event
  );

  return Status;
}
