/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
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
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/EfiPostLoginProtocol.h>
#include <Library/SystemPasswordLib.h>
#include <Protocol/SystemPasswordProtocol.h>
#include <Library/HiiLib.h>
#include <Library/SetupUiLib.h>
#include <Library/ByoCommLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/UefiLib.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Library/PcdLib.h>
#include <Protocol/DisplayBackupRecovery.h>
#include <Protocol/ByoNetworkIdentifierProtocol.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/QrEncoderProtocol.h>
#include <DhcpGetNetworldCfg.h>

STATIC EFI_HII_HANDLE gHiiHandle = NULL;
static EFI_GUID  mSystemPasswordGuid = { 0xCB750F18, 0x3BAA, 0x4448, { 0x9B, 0x4E, 0x1F, 0x5A, 0x82, 0xD3, 0x0E, 0x5C } };
#define GET_NETWORK_IDENTIFIER_RETRY_MAX_COUNT   3
#define RESET_SYSTEMPASSWORD                     1
#define PASSWORD_FORGET_BOOT_SUPPORT             2
#define DIALOG_ATTRIBUTE                         BIT1
#define QR_SIDE_LENGTH                           200
#define RANDOM_NUMBER_SIZE_32                    32
#define RANDOM_NUMBER_SIZE  8
#define NARROW_CHAR         0xFFF0
#define WIDE_CHAR           0xFFF1

CHAR16                      mResetCode[RANDOM_NUMBER_SIZE];
CHAR16                      mRandomCode[RANDOM_NUMBER_SIZE_32 + 1];
UINT8                       mRandomUint8[RANDOM_NUMBER_SIZE_32];
UINT8                       mDealSystemPasswordFlag = 0;

UINTN
GetStringWidth (
  IN  CHAR16               *String,
  IN  BOOLEAN              LimitLen,
  IN  UINTN                MaxWidth,
  OUT UINTN                *Offset
  )
{
  UINTN Index;
  UINTN Count;
  UINTN IncrementValue;

  if (String == NULL) {
    return 0;
  }

  Index           = 0;
  Count           = 0;
  IncrementValue  = 1;

  do {
    for (;(String[Index] != NARROW_CHAR) && (String[Index] != WIDE_CHAR) && (String[Index] != 0); Index++) {
      Count = Count + IncrementValue;
      if (LimitLen && Count > MaxWidth) {
        break;
      }
    }
    if (String[Index] == 0) {
      break;
    }

    if (LimitLen && Count > MaxWidth) {
      *Offset = Index;
      break;
    }
    if (String[Index] == NARROW_CHAR) {
      Index++;
      IncrementValue = 1;
    } else {
      Index++;
      IncrementValue = 2;
    }
  } while (String[Index] != 0);

  return Count * sizeof (CHAR16);
}

//
//Use Sha256HashAll to generate hash for random code
//
VOID
EncodeRandomString (
  IN   UINT8    *RandomString,
  IN   UINTN    Length,
  OUT  UINT8    **Hash
  )
{
  UINTN    Len;
  UINT8   *Buf = NULL;
  UINT8    RandomStringArray[64];
  UINT8    HashArray[32];

  Len = RANDOM_NUMBER_SIZE;
  ZeroMem(RandomStringArray, sizeof(RandomStringArray));
  CopyMem(RandomStringArray, RandomString, Len);

  ZeroMem(HashArray, sizeof(HashArray));
  Sha256HashAll (RandomString, Length, HashArray);
  CopyMem(RandomStringArray + Len, HashArray, sizeof(HashArray));

  ZeroMem(HashArray, sizeof(HashArray));
  Sha256HashAll (RandomStringArray, Len + sizeof(HashArray), HashArray);

  Buf = NULL;
  Buf = AllocateZeroPool(RANDOM_NUMBER_SIZE + 1);
  ASSERT (Buf != NULL);
  CopyMem(Buf, HashArray, RANDOM_NUMBER_SIZE);

  *Hash = Buf;
}

VOID
GetRandomCode (
  VOID
  )
{
  UINT8       RandomBuffer[RANDOM_NUMBER_SIZE_32];
  UINT8       Index;
  UINT8       *PasswordHash;

  ZeroMem (RandomBuffer, sizeof (RandomBuffer));
  RandomSeed (NULL, 0);
  RandomBytes (RandomBuffer, RANDOM_NUMBER_SIZE_32);

  ZeroMem (mResetCode, sizeof (mResetCode));
  ZeroMem (mRandomCode, sizeof (mRandomCode));
  ZeroMem (mRandomUint8, sizeof (mRandomUint8));

  for (Index = 0; Index < RANDOM_NUMBER_SIZE_32; Index++) {
    mRandomUint8[Index] =(RandomBuffer [Index] % 26 + 'A');
    mRandomCode[Index] = (CHAR16) mRandomUint8[Index];
  }
  mRandomCode[8] = L'\0';

  EncodeRandomString (mRandomUint8, RANDOM_NUMBER_SIZE, &PasswordHash);
  for (Index = 0; Index < RANDOM_NUMBER_SIZE; Index++) {
    mResetCode[Index] =(CHAR16) (PasswordHash[Index] % 36 + ((PasswordHash[Index] % 36) > 9 ? '7' : '0'));
  }
  if (PasswordHash != NULL) {
    FreePool (PasswordHash);
  }
}


VOID
FatalErrorMessage (
  VOID
  )
{
  CHAR16   *Content;
  CHAR16   *Title;
  UINT16   Time;
  CHAR16   *Buffer;

  Content = NULL;
  Time    = PcdGet16 (PcdPasswordUnlockTime);
  if (Time == 0) {
    return;
  }
  Title   = HiiGetString(gHiiHandle, STRING_TOKEN(STR_ERROR), NULL);
  
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  if (Time != 0xFFFF) {
    Content = HiiGetString(gHiiHandle, STRING_TOKEN(STR_PASSWORD_ERROR_COUNTDOWN), NULL);
    while (TRUE) {
      Buffer = AllocatePool (StrSize (Content)+sizeof(UINT16)*2);
      UnicodeSPrint(Buffer, StrSize (Content)+sizeof(UINT16)*2, Content, Time);
      UiConfirmDialogEx(DIALOG_ATTRIBUTE_NO_REFRESH, DIALOG_NO_REPAINT, Title, NULL, TEXT_ALIGIN_CENTER, Buffer, NULL);
      gBS->Stall(1000 * 1000);
      FreePool (Buffer);
      if (Time == 0) {
        break;
      }
      Time--;
    }
    UiClearNoRepaintFlag(DIALOG_NO_REPAINT);
  } else {
    SetOemDialogColor(DLG_OEM_COLOR_RED);
    Content = HiiGetString(gHiiHandle, STRING_TOKEN(STR_PASSWORD_FATAL_ERROR), NULL);
    UiConfirmDialog(DIALOG_BOX_DEAD_LOOP, Title, NULL, TEXT_ALIGIN_CENTER, Content, NULL);
    gBS->RaiseTPL(TPL_HIGH_LEVEL);
    CpuDeadLoop();
  }

  gST->ConOut->ClearScreen(gST->ConOut);
  FreePool(Title);
  if (Content != NULL) FreePool(Content);
}

VOID
ExpireWarningMessage (
  VOID
  )
{
  CHAR16   *String;

  String = HiiGetString(gHiiHandle, STRING_TOKEN(STR_OUT_TIME_PASSWORD_STRING), NULL);
  UiConfirmDialog(DIALOG_WAIT_ENTER_KEY, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
  FreePool(String);
}

VOID
NearExpireWarningMessage (
  VOID
  )
{
  CHAR16   *String;
  CHAR16   *Buffer;


  String = HiiGetString(gHiiHandle, STRING_TOKEN(STR_PWD_NEAR_EXPIRATION_STR), NULL);
  Buffer = AllocatePool (StrSize (String)+sizeof(UINT16));
  UnicodeSPrint(Buffer, StrSize (String)+sizeof(UINT16), String, PcdGet8(PcdPasswordNearExpirationDays));

  UiConfirmDialog(DIALOG_WAIT_ENTER_KEY, NULL, NULL, TEXT_ALIGIN_CENTER, Buffer, NULL);
  FreePool (String);
  FreePool (Buffer);
}

EFI_STATUS
DrawInputResetCodeDialog(
  IN  EFI_SYSTEM_PASSWORD_PROTOCOL *SystemPassword,
  IN OUT CHAR16                      *StringPtr
  )
{
  CHAR16                               *Line = NULL;
  CHAR16                               *Line1 = NULL;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL      *ConOut;
  UINTN                                MaxLength;
  EFI_SIMPLE_TEXT_OUTPUT_MODE          SavedConsoleMode;
  UINTN                                Columns;
  UINTN                                Rows;
  UINTN                                Column;
  UINTN                                Row;
  UINTN                                AttribTable;
  BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL  *DisplayBackupRecover;
  BACKUP_RECOVER_DATA                  *Data = NULL;
  UINTN                                Index;
  EFI_STATUS                           Status;
  EFI_STRING                           InputResetCodeMessage;
  UINTN                                Length;

  InputResetCodeMessage = HiiGetString(gHiiHandle, STRING_TOKEN(STR_INPUT_TRSET_CODE_MSG),NULL);

  Status = gBS->LocateProtocol(&gDisplayBackupRecoverProtocolGuid, NULL, (VOID**)&DisplayBackupRecover);
  if (EFI_ERROR (Status)) {
    DisplayBackupRecover = NULL;
  } else {
    Status = DisplayBackupRecover->DispalyBackup(&Data);
  }

  AttribTable = (EFI_WHITE | EFI_BACKGROUND_BLUE) ;
  ConOut = gST->ConOut;
  CopyMem(&SavedConsoleMode, ConOut->Mode, sizeof(SavedConsoleMode));
  ConOut->QueryMode(ConOut, SavedConsoleMode.Mode, &Columns, &Rows);
  ConOut->EnableCursor(ConOut, FALSE);
  ConOut->SetAttribute(ConOut, AttribTable);

  MaxLength = MIN(31, Columns - 2);
  MaxLength = MAX(MIN(Columns-2, 31), MaxLength);
  Line = AllocateZeroPool((MaxLength + 2) * sizeof(CHAR16));
  Row    = (Rows - 6) / 2;
  Column = (Columns - (MaxLength + 2)) / 2;

  // first line
  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_DOWN_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_DOWN_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

  //tile
  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  Length = GetStringWidth(InputResetCodeMessage, FALSE, 0, NULL) / 2;   
  ConOut->SetCursorPosition(ConOut, Column + 1 + (MaxLength - Length) / 2, Row++);
  ConOut->OutputString(ConOut, InputResetCodeMessage);

// middle part
  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  for (Index = 0; Index < 3; Index++) {
    ConOut->SetCursorPosition(ConOut, Column, Row++);
    ConOut->OutputString(ConOut, Line);
  }

  //end part
  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_UP_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_UP_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  
  Line1 = AllocateZeroPool(27* sizeof(CHAR16));
  SetMem16(Line1, 27*2, L' ');
  Line1[24 + 2] = L'\0';
  ConOut->SetAttribute(ConOut, EFI_TEXT_ATTR (EFI_BLACK, EFI_BLACK));
  ConOut->SetCursorPosition(ConOut, Column +3, Row -2);
  ConOut->OutputString(ConOut, Line1);

  Status = SystemPassword->Input(Column +3, Row -2, StringPtr);

  if (DisplayBackupRecover != NULL && Data != NULL) {
    DisplayBackupRecover->DispalyRecover(Data);
    FreePool (Data);
    Data = NULL;
  }
  return Status;
}

VOID
ResetPasswordFunction(
  IN  EFI_SYSTEM_PASSWORD_PROTOCOL *SystemPassword,
  OUT BOOLEAN                       *bVerifyTrue
 )
{
  EFI_STRING                      ResetCodeMessage;
  EFI_STRING                      ResetCodeErrorMessage;
  EFI_STRING                      CleanPasswordStr;
  CHAR16                          *ResetCodeInputStringPtr;
  UINTN                           InputREsetCodeCount = 0;
  UINTN                           Index = 0;
  BOOLEAN                         bVerifyTFalseAndCountThree = FALSE;
  EFI_STATUS                      Status;
  EFI_STRING                      AllowBootCodeMessage;
  INTN                            RegionSize;
  QR_ENCODER_PROTOCOL             *QrEncoderProtocol;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GraphicsOutput;
  EFI_STRING                      QrPromptMessage;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL   *Bitmap = NULL;


  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  ASSERT(!EFI_ERROR(Status));

  RegionSize = QR_SIDE_LENGTH;
  ResetCodeInputStringPtr = AllocateZeroPool(sizeof(CHAR16) * (MAX_PASSWORD_LENGTH + 1));
  ResetCodeMessage = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWORD_RANDOM_CODE_MSG),NULL);
  ResetCodeErrorMessage = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWORD_CODE_ERROR_MSG),NULL);
  CleanPasswordStr = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWORD_CLEAR_MESSAGE),NULL);
  AllowBootCodeMessage = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWORD_ALLOW_BOOT_MSG),NULL);
  QrPromptMessage = HiiGetString(gHiiHandle,STRING_TOKEN(STR_QR_MSG),NULL);

// get RandomCode
  GetRandomCode();
  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  Status = gBS->LocateProtocol (&gQrEncoderProtocolGuid, NULL, (VOID **)&QrEncoderProtocol);
  if (!EFI_ERROR (Status)) {
    Status = QrEncoderProtocol->GenerateQrCode (QrEncoderProtocol,
                                                mRandomUint8,
                                                RANDOM_NUMBER_SIZE_32,
                                                RegionSize,
                                                &Bitmap
                                                );
    if (!EFI_ERROR(Status) && Bitmap != NULL) {
      Status = GraphicsOutput->Blt (
                              GraphicsOutput,
                              Bitmap,
                              EfiBltBufferToVideo,
                              0,
                              0,
                              GraphicsOutput->Mode->Info->HorizontalResolution/2 -RegionSize/2,
                              GraphicsOutput->Mode->Info->VerticalResolution/4 -RegionSize/2,
                              RegionSize,
                              RegionSize,
                              0
                              );
      if (!EFI_ERROR (Status)) {
        Print (QrPromptMessage);
      }
    }
  }

  if (EFI_ERROR (Status) || Bitmap == NULL) {
    UiConfirmDialogEx (DIALOG_ATTRIBUTE, DIALOG_INFO, ResetCodeMessage, NULL, TEXT_ALIGIN_CENTER, mRandomCode, NULL);
  }

  do {
    Status = DrawInputResetCodeDialog(SystemPassword, ResetCodeInputStringPtr);
    if (EFI_ERROR(Status)) {
      continue;
    }
    InputREsetCodeCount++;
    //
    // Check reset code
    //
    if (StrLen (ResetCodeInputStringPtr) == RANDOM_NUMBER_SIZE) {
      for (Index = 0; Index < RANDOM_NUMBER_SIZE; Index++) {
        if (ResetCodeInputStringPtr[Index] != mResetCode[Index] ) {
          UiConfirmDialogEx (DIALOG_ATTRIBUTE, DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, ResetCodeErrorMessage, NULL);
          break;
        }
     }
    } else {
      UiConfirmDialogEx (DIALOG_ATTRIBUTE, DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, ResetCodeErrorMessage, NULL);
    }
    //
    //The reset code entered is correct
    //
    if (Index == RANDOM_NUMBER_SIZE) {
      *bVerifyTrue = TRUE;
      break;
    }
    //Enter the reset code three times or more
    if (InputREsetCodeCount >= 3) {
      bVerifyTFalseAndCountThree = TRUE;
      break;
    }
  } while (TRUE);

  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  //
  //mDealSystemPasswordFlag = PASSWORD_FORGET_BOOT_SUPPORT:Allow access to the system if users forget your password
  //mDealSystemPasswordFlag == RESET_SYSTEMPASSWORD :clear SystemPassword
  //
  if(*bVerifyTrue) {
    if (mDealSystemPasswordFlag == PASSWORD_FORGET_BOOT_SUPPORT) {
      UiConfirmDialog(DIALOG_INFO, ResetCodeMessage, NULL, TEXT_ALIGIN_CENTER, AllowBootCodeMessage, NULL);
    } else {
      SystemPassword->Clear(PD_MAX);
      UiConfirmDialog(DIALOG_INFO, ResetCodeMessage, NULL, TEXT_ALIGIN_CENTER, CleanPasswordStr, NULL);
    }
  }

  if (bVerifyTFalseAndCountThree) {
    FatalErrorMessage();
    SystemPassword->SetTimes (PD_MAX, VERIFY_ACTION_RESET);
  }

  FreePool (ResetCodeInputStringPtr);
  FreePool (ResetCodeMessage);
  FreePool (ResetCodeErrorMessage);
  FreePool (CleanPasswordStr);
  FreePool (AllowBootCodeMessage);
  FreePool (QrPromptMessage);
  if (Bitmap != NULL) {
    FreePool (Bitmap);
  }
}

EFI_STATUS
GetUserInputPassword (
  IN  BOOLEAN                       PopPassword,
  IN  EFI_SYSTEM_PASSWORD_PROTOCOL *SystemPassword,
  IN  CHAR16                       *ErrorStr,
  OUT CHAR16                       *Password,
  OUT BOOLEAN                      *LengthInvalid
  )
{
  EFI_STATUS                      Status; 
  EFI_STRING                      PasswordInfo0 = NULL; 
  EFI_STRING                      PasswordInfo1 = NULL; 
  EFI_STRING                      PasswordInfo2 = NULL; 
  CHAR16                          *TitleStr     = NULL;
  CHAR16                          *Prompt       = NULL;
  CHAR16                          *HelpStr      = NULL;
  CHAR16                          *ReminderStr  = NULL;
  CHAR16                          *InvalidLengthStr = NULL;

  PasswordInfo0 = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWD_INFO0),NULL);
  PasswordInfo1 = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWD_INFO1),NULL); 
  InvalidLengthStr = HiiGetString(gHiiHandle, STRING_TOKEN(STR_PASSWD_INVALID_LENGTH), NULL);
  if (ErrorStr != NULL) {
    if (*LengthInvalid) {
      UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER,DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, InvalidLengthStr, NULL);
    } else {
      UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER,DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, ErrorStr, NULL);
    }
  }

  if (PopPassword) {
    TitleStr = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWD_INFO2),NULL);
  } else {
    TitleStr = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWD_INFO3),NULL);
  }

  PcdSetBoolS (PcdAllowEmptySystemPassword, TRUE);
  ZeroMem(Password, sizeof(CHAR16) * SYSTEM_PASSWORD_LENGTH);
  Status = ShowPostPasswordDialog(TitleStr,
                                  Prompt,
                                  HelpStr,
                                  ReminderStr,
                                  NULL,
                                  NULL,
                                  Password,
                                  SYSTEM_PASSWORD_LENGTH
                                  );
  PcdSetBoolS (PcdAllowEmptySystemPassword, FALSE);

  if (FixedPcdGetBool(PcdGuiEnable)) {
      if (!((!EFI_ERROR(Status)) && Password[0] != 0)) {
        PasswordInfo2 = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWORD_NULL_INVALID),NULL);
        UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER,DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, PasswordInfo2, NULL);
        FreePool(PasswordInfo2);
      }
    }

  if (StrnLenS (Password, MAX_PASSWORD_LENGTH + 1) < MIN_PASSWORD_LENGTH) {
    *LengthInvalid = TRUE;
  } else {
    *LengthInvalid = FALSE;
  }
  FreePool(TitleStr);
  FreePool(InvalidLengthStr);
  FreePool(PasswordInfo0);
  FreePool(PasswordInfo1);
  gST->ConOut->ClearScreen (gST->ConOut);
  return Status;
}

EFI_STATUS
GetAndSetSalt (
  EFI_SYSTEM_PASSWORD_PROTOCOL  *SystemPassword
  )
{
  EFI_STATUS                                 Status;
  NETWORK_IDENTIFIER_PROTOCOL                *NetworkIdentifierProtocol;
  UINT8                                      *NetWorkId = NULL;
  UINT8                                      NetWorkIdLen = 0x0;
  UINT8                                      GetNetworkIdCount = 0;
  EFI_STRING                                 ReSetTips = NULL;
  EFI_STRING                                 NetAuthPwStr = NULL;
  EFI_STRING                                 NetAuthPromptStr = NULL;
  PASSWORD_NETWORK_AUTH_CONFIG               PwdNetworkAuthInfo;
  UINTN                                      VariableSize;
  EFI_STRING                                 BootSupportTips;
  BOOLEAN                                    bVerifyStartUpCode = FALSE;

  VariableSize = sizeof (PASSWORD_NETWORK_AUTH_CONFIG);
  Status = gRT->GetVariable (
                  PASSWORD_NETWORK_AUTH_VAR_NAME,
                  &gByoPasswordNetworkAuthConfigGuid,
                  NULL,
                  &VariableSize,
                  &PwdNetworkAuthInfo
                  );
  if (EFI_ERROR (Status) || PwdNetworkAuthInfo.PwdNetworkAuthEnable != 0x1) {
    DEBUG ((DEBUG_ERROR, "Get PASSWORD_NETWORK_AUTH_VAR_NAME Failed!\n"));
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateProtocol (&gByoNetworkIdentifierProtocolGuid, NULL, (VOID **)&NetworkIdentifierProtocol);
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "Get NetworkIdentifier Protocol Failed!\n"));
    return EFI_NOT_FOUND;
  }

  NetWorkId = AllocateZeroPool (NETWORK_ID_MAX_SIZE);
  if (NetWorkId == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ReSetTips        = HiiGetString (gHiiHandle, STRING_TOKEN(STR_GETNETWORKID_MSG), NULL);
  NetAuthPwStr     = HiiGetString (gHiiHandle, STRING_TOKEN(STR_NETWORK_AUTH_RUNTIME), NULL);
  NetAuthPromptStr = HiiGetString (gHiiHandle, STRING_TOKEN(STR_NETWORK_AUTH_PROMPT), NULL);

  gST->ConOut->ClearScreen(gST->ConOut);
  UiConfirmDialog (DIALOG_NO_KEY, NetAuthPromptStr, NULL, TEXT_ALIGIN_CENTER, NetAuthPwStr, L"", NULL);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  do {
    if (GetNetworkIdCount < GET_NETWORK_IDENTIFIER_RETRY_MAX_COUNT) {
      NetWorkIdLen = (UINT8) NETWORK_ID_MAX_SIZE; 
      Status = NetworkIdentifierProtocol->GetNetworkId (NetWorkId, &NetWorkIdLen);
      if (!EFI_ERROR (Status)) {
        DumpMem8(NetWorkId, NetWorkIdLen);
        DEBUG ((DEBUG_ERROR, "Get Network Identifier Success!\n"));
        GetNetworkIdCount = 0;
        SystemPassword->SetSaltBuffer (NetWorkId, NetWorkIdLen);
        Status = EFI_SUCCESS;
        break;
      } else {
        DEBUG ((DEBUG_ERROR, "Get Network Identifier Failed!\n"));
        GetNetworkIdCount ++;
        if (GetNetworkIdCount < GET_NETWORK_IDENTIFIER_RETRY_MAX_COUNT) {
          continue;
        } else if (FixedPcdGetBool (PcdEnableBootWithoutSystemPassword)) {
          mDealSystemPasswordFlag = PASSWORD_FORGET_BOOT_SUPPORT;
          break;
        } else if (FixedPcdGetBool (PcdEnableSystemPasswordReset)) {
          mDealSystemPasswordFlag = RESET_SYSTEMPASSWORD;
          break;
        } else {
          UiConfirmDialog (DIALOG_BOX_DEAD_LOOP, NULL, NULL, TEXT_ALIGIN_CENTER, ReSetTips, NULL);
        }
      }
    }
  } while (1);

  gBS->Stall (2000000);
  gST->ConOut->ClearScreen (gST->ConOut);

  if ((mDealSystemPasswordFlag == PASSWORD_FORGET_BOOT_SUPPORT || mDealSystemPasswordFlag == RESET_SYSTEMPASSWORD) && Status != EFI_SUCCESS) {
    BootSupportTips  = HiiGetString (gHiiHandle, STRING_TOKEN(STR_GETNETWORKID_MSG2), NULL);
    UiConfirmDialogEx (DIALOG_ATTRIBUTE, DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, BootSupportTips, NULL);
    ResetPasswordFunction ( SystemPassword, &bVerifyStartUpCode);
    FreePool (BootSupportTips);
  }
  FreePool (ReSetTips);
  FreePool (NetAuthPwStr);
  FreePool (NetAuthPromptStr);
  if (NetWorkId != NULL) {
    FreePool (NetWorkId);
  }
  return Status;
}

EFI_STATUS
CheckPopPassword (EFI_SYSTEM_PASSWORD_PROTOCOL  *SystemPassword)
{
  EFI_STATUS                      Status;
  CHAR16                          *Password = NULL;
  BOOLEAN                         bPassed;
  BOOLEAN                         bFirstChecked;
  EFI_STRING                      PasswordInvalid; 
  VERIFY_RESULT                   FatalError = VERIFY_PASS;
  PLAT_HOST_INFO_PROTOCOL         *PlatHost = NULL;
  EFI_HANDLE                      Handle;
  SYSTEM_PASSWORD                 PasswordVariable;
  UINTN                           VarSize;
  UINT8                           EnteredType = LOGIN_USER_ADMIN;
  UINT32                          SetupLock;
  BOOLEAN                         bVerifyTrue = FALSE;
  EFI_STRING                      PasswordEmpty; 
  BOOLEAN                         PasswordLengthInvalid = FALSE;

  VarSize = sizeof (SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VarSize,
                  &PasswordVariable
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }
  if (!PasswordVariable.RequirePopOnRestart) {
    return EFI_UNSUPPORTED;
  }
  if (mDealSystemPasswordFlag == PASSWORD_FORGET_BOOT_SUPPORT) {
    return EFI_SUCCESS;
  }


  if (SystemPassword->GetPopChecked()) {
    return EFI_SUCCESS;
  }
  if (!(SystemPassword->BeHave(PD_POWER_ON) || SystemPassword->BeHave(PD_ADMIN))) {
    return EFI_UNSUPPORTED;
  }


  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID**)&PlatHost);
  if(!EFI_ERROR(Status) && (PlatHost != NULL) && (PlatHost->StopWdg != NULL)) {
    PlatHost->StopWdg();
  } else {
    // install this to disable FRB2 watch dog
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiFrb2WatchDogNotifyGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }
  Status = GetAndSetSalt (SystemPassword);
  if ((mDealSystemPasswordFlag == PASSWORD_FORGET_BOOT_SUPPORT || mDealSystemPasswordFlag == RESET_SYSTEMPASSWORD) && Status != EFI_SUCCESS) {
    bVerifyTrue = TRUE;
    if (mDealSystemPasswordFlag == PASSWORD_FORGET_BOOT_SUPPORT) {
      EnteredType = LOGIN_USER_POP;
    }
  }

  PasswordInvalid = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWD_INVALID),NULL); 
  PasswordEmpty = HiiGetString (gHiiHandle, STRING_TOKEN(STR_PASSWORD_INPUT_EMPTY), NULL);

  Password = AllocateZeroPool ((MAX_PASSWORD_LENGTH + 1)* sizeof (CHAR16));
  ASSERT(Password != NULL);

  SystemPassword->SetTimes (PD_MAX, VERIFY_ACTION_RESET);
  SystemPassword->SetMode(VERIFY_MODE_INTO_POP);
  bPassed = FALSE;
  bFirstChecked = TRUE;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
  while ((!bPassed) && (!bVerifyTrue) ) {
    gST->ConOut->ClearScreen (gST->ConOut);
    gST->ConOut->EnableCursor (gST->ConOut, FALSE);
    if (bFirstChecked) {
      bFirstChecked = FALSE;
      GetUserInputPassword(TRUE, SystemPassword, NULL, Password, &PasswordLengthInvalid);
    } else if (*Password == 0) {
      PasswordLengthInvalid = FALSE;
      GetUserInputPassword(TRUE, SystemPassword, PasswordEmpty, Password, &PasswordLengthInvalid);
    } else {
      GetUserInputPassword(TRUE, SystemPassword, PasswordInvalid, Password, &PasswordLengthInvalid);
    }

    if (PasswordLengthInvalid) {
      continue;
    }

    if (SystemPassword->BeHave(PD_ADMIN)) {
      bPassed = SystemPassword->Verify(PD_ADMIN, Password, &FatalError);
      DEBUG((EFI_D_INFO, "Verify PD_ADMIN FatalError:%d, bPassed:%d\n", FatalError, bPassed));
      if (!bPassed) {
        if (SystemPassword->BeHave(PD_POWER_ON)) {
          SystemPassword->SetTimes (PD_POWER_ON, VERIFY_ACTION_SUB);
          bPassed = SystemPassword->Verify(PD_POWER_ON, Password, &FatalError);
          DEBUG((EFI_D_INFO, "Verify PD_POWER_ON FatalError:%d, bPassed:%d\n", FatalError, bPassed));
          if (bPassed) {
            EnteredType = LOGIN_USER_POP;
          }
        }
      } else {
        EnteredType = LOGIN_USER_ADMIN;
      }
    } else {
      bPassed = SystemPassword->Verify(PD_POWER_ON, Password, &FatalError);
      if (bPassed) {
        EnteredType = LOGIN_USER_ADMIN;
      }
    }

    if(FatalError == VERIFY_WRONG){
      if (FixedPcdGetBool (PcdEnableBootWithoutSystemPassword)) {
        mDealSystemPasswordFlag = PASSWORD_FORGET_BOOT_SUPPORT;
        ResetPasswordFunction( SystemPassword, &bVerifyTrue);
        EnteredType = LOGIN_USER_POP;
      } else if (FixedPcdGetBool (PcdEnableSystemPasswordReset)) {
        mDealSystemPasswordFlag = RESET_SYSTEMPASSWORD;
        ResetPasswordFunction( SystemPassword, &bVerifyTrue);
      } else {
        FatalErrorMessage();
        SystemPassword->SetTimes (PD_MAX, VERIFY_ACTION_RESET);
      }
    }
    if (FatalError == VERIFY_EXPIRE_SOON) {
      NearExpireWarningMessage();
    }
    if(FatalError == VERIFY_EXPIRE){
      ExpireWarningMessage();
    }
    
  }

  SystemPassword->SetTimes (PD_MAX, VERIFY_ACTION_RESET);
  VarSize = sizeof (SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VarSize,
                  &PasswordVariable
                  );
  if (!EFI_ERROR(Status)) {
    PasswordVariable.EnteredType = EnteredType;
    Status = gRT->SetVariable (
                    SYSTEM_PASSWORD_NAME,
                    &gEfiSystemPasswordVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VarSize,
                    &PasswordVariable
                    );
  }
  if (EnteredType == LOGIN_USER_POP){
    VarSize = sizeof(SetupLock);  
    Status = gRT->GetVariable (
                    FLASH_LOCK_STATUS_NAME,
                    &gEfiFlashLockStatusVariableGuid,
                    NULL,
                    &VarSize,
                    &SetupLock
                    );
    if (!EFI_ERROR(Status)){
      SetupLock = 1;
      Status = gRT->SetVariable (
                  FLASH_LOCK_STATUS_NAME,
                  &gEfiFlashLockStatusVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  VarSize,
                  &SetupLock
                  );
    }
  }
  FreePool(PasswordInvalid);
  FreePool(Password);
  FreePool (PasswordEmpty);
  return EFI_SUCCESS;
}




EFI_STATUS
CheckSetupPassword (EFI_SYSTEM_PASSWORD_PROTOCOL *SystemPassword) 
{
  EFI_STATUS                      Status;
  UINTN                           VarSize;
  CHAR16                          *Password;
  BOOLEAN                         bPassed;
  BOOLEAN                         bFirstChecked;
  SYSTEM_PASSWORD                 PasswordVariable;
  UINT8                           EnteredType;
  EFI_STRING                      PasswordInvalid = NULL; 
  VERIFY_RESULT                   FatalError = VERIFY_PASS;
  PLAT_HOST_INFO_PROTOCOL         *PlatHost = NULL;
  EFI_HANDLE                      Handle;
  UINT32                          SetupLock;
  BOOLEAN                         bVerifyTrue = FALSE;
  EFI_STRING                      PasswordEmpty;
  EFI_STRING                      ConfirmMessage;
  SELECTION_TYPE                  ReturnFlag;
  BOOLEAN                         PasswordLengthInvalid = FALSE;

  if (!SystemPassword->BeHave(PD_ADMIN) &&
      !SystemPassword->BeHave(PD_POWER_ON)) {
    return EFI_NOT_READY;
  }

  if (mDealSystemPasswordFlag == PASSWORD_FORGET_BOOT_SUPPORT) {
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID**)&PlatHost);
  if(!EFI_ERROR(Status) && (PlatHost != NULL) && (PlatHost->StopWdg != NULL)) {
    PlatHost->StopWdg();
  } else {
    // install this to disable FRB2 watch dog
    Handle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiFrb2WatchDogNotifyGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }

  PasswordInvalid = HiiGetString(gHiiHandle,STRING_TOKEN(STR_PASSWD_INVALID),NULL);       
  PasswordEmpty = HiiGetString (gHiiHandle, STRING_TOKEN (STR_PASSWORD_INPUT_EMPTY), NULL);
  ConfirmMessage = HiiGetString (gHiiHandle, STRING_TOKEN (STR_CONFIRM_INPUT_ENTER), NULL);
  	 
  EnteredType = LOGIN_USER_ADMIN;
  bPassed = FALSE;
  bFirstChecked = TRUE;
  SystemPassword->SetMode(VERIFY_MODE_INTO_SETUP);
  SystemPassword->SetTimes (PD_MAX, VERIFY_ACTION_RESET);

  Password = AllocateZeroPool ((MAX_PASSWORD_LENGTH + 1)* sizeof (CHAR16));
  ASSERT (Password != NULL);

  Status = GetAndSetSalt (SystemPassword);
  if ((mDealSystemPasswordFlag == PASSWORD_FORGET_BOOT_SUPPORT || mDealSystemPasswordFlag == RESET_SYSTEMPASSWORD) && Status != EFI_SUCCESS) {
    bVerifyTrue = TRUE;
    if (mDealSystemPasswordFlag == PASSWORD_FORGET_BOOT_SUPPORT) {
      EnteredType = LOGIN_USER_POP;
    }
  }

  while ((!bPassed) && (!bVerifyTrue) ) {
    gST->ConOut->ClearScreen (gST->ConOut);
    gST->ConOut->EnableCursor (gST->ConOut, FALSE);
    if (bFirstChecked) {
      bFirstChecked = FALSE;
      GetUserInputPassword(FALSE, SystemPassword, NULL, Password, &PasswordLengthInvalid);
    } else if (*Password == 0) {
      PasswordLengthInvalid = FALSE;
      GetUserInputPassword(FALSE, SystemPassword, PasswordEmpty, Password, &PasswordLengthInvalid);
    } else {
      GetUserInputPassword(FALSE, SystemPassword, PasswordInvalid, Password, &PasswordLengthInvalid);
    }

    if (*Password == 0) {
      if (SystemPassword->BeHave(PD_ADMIN) && !SystemPassword->BeHave(PD_POWER_ON) ) {
        ReturnFlag = UiConfirmDialogEx(DIALOG_ATTRIBUTE_RECOVER, DIALOG_YESNO, NULL, NULL, TEXT_ALIGIN_CENTER, ConfirmMessage, NULL);
        if (ReturnFlag == SELECTION_YES) {
          EnteredType = LOGIN_USER_POP;
          break;
        }
      }
    } else {

      if (PasswordLengthInvalid) {
        continue;
      }

      if (SystemPassword->BeHave(PD_ADMIN)) {
        bPassed = SystemPassword->Verify(PD_ADMIN, Password, &FatalError);
        if (!bPassed) {
          if (SystemPassword->BeHave(PD_POWER_ON)) {
            SystemPassword->SetTimes (PD_POWER_ON, VERIFY_ACTION_SUB);
            bPassed = SystemPassword->Verify(PD_POWER_ON, Password, &FatalError);          
            if (bPassed) {
              EnteredType = LOGIN_USER_POP;
            }
          }
        } else {
          EnteredType = LOGIN_USER_ADMIN;
        }
      } else {
        bPassed = SystemPassword->Verify(PD_POWER_ON, Password, &FatalError);
        if (bPassed) {
          EnteredType = LOGIN_USER_ADMIN;
        }
      }
    }

    if(FatalError == VERIFY_WRONG){
      if (FixedPcdGetBool (PcdEnableBootWithoutSystemPassword)) {
        mDealSystemPasswordFlag = PASSWORD_FORGET_BOOT_SUPPORT;
        ResetPasswordFunction( SystemPassword, &bVerifyTrue);
        EnteredType = LOGIN_USER_POP;
      } else if (FixedPcdGetBool (PcdEnableSystemPasswordReset)) {
        mDealSystemPasswordFlag = RESET_SYSTEMPASSWORD;
        ResetPasswordFunction( SystemPassword, &bVerifyTrue);
      } else {
        FatalErrorMessage();
        SystemPassword->SetTimes (PD_MAX, VERIFY_ACTION_RESET);
      }
    }
    if (FatalError == VERIFY_EXPIRE_SOON) {
      NearExpireWarningMessage();
    }
    if(FatalError == VERIFY_EXPIRE){
      ExpireWarningMessage();
    }
  }

  SystemPassword->SetTimes (PD_MAX, VERIFY_ACTION_RESET);

  VarSize = sizeof (SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VarSize,
                  &PasswordVariable
                  );
  if (!EFI_ERROR(Status)) {    
    PasswordVariable.EnteredType = EnteredType;
    Status = gRT->SetVariable (
                    SYSTEM_PASSWORD_NAME,
                    &gEfiSystemPasswordVariableGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VarSize,
                    &PasswordVariable
                    );
    if (EnteredType == LOGIN_USER_POP){
      VarSize = sizeof(SetupLock);  
      Status = gRT->GetVariable (
                      FLASH_LOCK_STATUS_NAME,
                      &gEfiFlashLockStatusVariableGuid,
                      NULL,
                      &VarSize,
                      &SetupLock
                      );
      if (!EFI_ERROR(Status)){
        SetupLock = 1;
        Status = gRT->SetVariable (
                    FLASH_LOCK_STATUS_NAME,
                    &gEfiFlashLockStatusVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VarSize,
                    &SetupLock
                    );
      }
    }
    
  }

  FreePool(Password);  
  FreePool(PasswordInvalid);
  FreePool (PasswordEmpty);
  FreePool (ConfirmMessage);
  return EFI_SUCCESS;
}

VOID
EFIAPI
ClearLockStatus(
  IN EFI_EVENT    Event,
  IN VOID         *Context
)
{
  EFI_STATUS                    Status;
  EFI_SYSTEM_PASSWORD_PROTOCOL  *SystemPassword = NULL;
  UINTN                         VarSize;
  UINT32                        SetupLock;

  DEBUG ((EFI_D_INFO, "ClearLockStatus\n")); 

  Status = gBS->LocateProtocol (
                  &gEfiSystemPasswordProtocolGuid,
                  NULL,
                  (VOID**)&SystemPassword
                  );
  if (EFI_ERROR(Status)) {    
    return ;
  }

  if (!SystemPassword->BeHave(PD_ADMIN) && !SystemPassword->BeHave(PD_POWER_ON)){
    VarSize = sizeof(SetupLock);  
    Status = gRT->GetVariable (
                    FLASH_LOCK_STATUS_NAME,
                    &gEfiFlashLockStatusVariableGuid,
                    NULL,
                    &VarSize,
                    &SetupLock
                    );
    if (!EFI_ERROR(Status)){
      SetupLock = 0;
      Status = gRT->SetVariable (
                  FLASH_LOCK_STATUS_NAME,
                  &gEfiFlashLockStatusVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  VarSize,
                  &SetupLock
                  );
    }
  }
  return ;
}




EFI_STATUS
EFIAPI
CheckSysPd (
  BOOLEAN   BootSetup
  ) 
{
  EFI_STATUS                    Status;
  EFI_SYSTEM_PASSWORD_PROTOCOL  *SystemPassword = NULL;


  DEBUG ((EFI_D_INFO, "CheckSysPd(%d)\n", BootSetup)); 

  Status = gBS->LocateProtocol (
                  &gEfiSystemPasswordProtocolGuid,
                  NULL,
                  (VOID**)&SystemPassword
                  );
  if (EFI_ERROR(Status)) {    
    return Status;
  }
  
  if (BootSetup) {
    return CheckSetupPassword(SystemPassword);
  } else {
    return CheckPopPassword(SystemPassword);
  }
}


EFI_POST_LOGIN_PROTOCOL gPostLoginProtocol = {
  CheckSysPd
};

EFI_STATUS
EFIAPI
PostLoginInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;
  
  gHiiHandle = HiiAddPackages (
                 &mSystemPasswordGuid,
                 gImageHandle,
                 STRING_ARRAY_NAME,
                 NULL
                 );
  ASSERT (gHiiHandle != NULL);
  
  Status = gBS->InstallProtocolInterface (
                 &ImageHandle,
                 &gEfiPostLoginProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &gPostLoginProtocol
                 );
  ASSERT_EFI_ERROR(Status);

  Status = EfiCreateEventReadyToBootEx (
              TPL_CALLBACK,
              ClearLockStatus,
              NULL,
              &Event
              );
    ASSERT_EFI_ERROR (Status);

  return Status;
}


