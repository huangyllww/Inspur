/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/HiiLib.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/GenericElog.h>
#include <Protocol/HiiConfigAccess.h>
#include <IpmiDefinitions.h>
#include <Library/IpmiBaseLib.h>
#include <BmcConfig.h>
#include <Library/SetupUiLib.h>


CHAR16 *gYesNoStrList[2] = {NULL};
CHAR16 *gClearElogTitile;
CHAR16 *gClearElogMsg;

extern EFI_HII_HANDLE           HiiHandle;
#define BLT_BACKGROUND_WHITE    EFI_BACKGROUND_LIGHTGRAY


UINTN
UefiLibGetStringWidth (
  IN  CHAR16               *String,
  IN  BOOLEAN              LimitLen,
  IN  UINTN                MaxWidth,
  OUT UINTN                *Offset
  );


VOID 
InitStr(EFI_HII_HANDLE  HiiHandle)
{
  EFI_STRING  Str;

  Str = HiiGetString(HiiHandle, STRING_TOKEN(STR_YES), NULL);
  if(gYesNoStrList[0] != NULL && StrCmp(gYesNoStrList[0], Str) == 0){
    goto ProcExit;
  }	

  DEBUG((EFI_D_INFO, "Update String\n"));

  if(gYesNoStrList[0] != NULL){
    FreePool(gYesNoStrList[0]);
    FreePool(gYesNoStrList[1]);
    FreePool(gClearElogTitile);
    FreePool(gClearElogMsg);
  }
  gYesNoStrList[0]  = HiiGetString(HiiHandle, STRING_TOKEN(STR_YES), NULL);
  gYesNoStrList[1]  = HiiGetString(HiiHandle, STRING_TOKEN(STR_NO), NULL);	  
  gClearElogTitile  = HiiGetString(HiiHandle, STRING_TOKEN(STR_CLEAR_ELOG_TITLE), NULL);
  gClearElogMsg     = HiiGetString(HiiHandle, STRING_TOKEN(STR_CLEAR_ELOG_CONFIRM), NULL);
  
ProcExit:
  if(Str != NULL){FreePool(Str);}
  return;	
}




// Return Choice Index
//   left : 0
//   rigth: 1
//   ESC  : 0xFF
//
// Color:
//   0 - B:White, F:Blue / default
//   1 - B:White, F:Red
UINTN
EFIAPI
DrawConfirmDialog (
  IN  UINTN          DialogColorIndex, 
  IN  CHAR16         **ChoiceStr,
  IN  CHAR16         *Title,
  ...
  )
{
  VA_LIST                          Args;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut;
  EFI_SIMPLE_TEXT_OUTPUT_MODE      SavedConsoleMode;
  UINTN                            Columns;
  UINTN                            Rows;
  UINTN                            Column;
  UINTN                            Row;
  UINTN                            NumberOfLines;
  UINTN                            MaxLength;
  CHAR16                           *String;
  UINTN                            Length;
  CHAR16                           *Line;
  CHAR16                           *TmpString;
  CHAR16                           *CStr[2];
  UINTN                            Choice1X=0, Choice2X=0, ChoiceY=0;
  UINTN                            EventIndex;
  EFI_INPUT_KEY                    Key;  
  UINTN                            ChoiceIndex;
  UINTN                            AttribComm, AttribFocus;
  UINTN                            CStr0Len, CStr1Len;
  

//DEBUG((EFI_D_ERROR, "%a()\n", __FUNCTION__));
  
  CStr[0]  = ChoiceStr[0];
  CStr[1]  = ChoiceStr[1];
  
  CStr0Len = StrLen(CStr[0]);    
  CStr1Len = StrLen(CStr[1]);
  ASSERT(CStr0Len < 20 && CStr0Len);
  ASSERT(CStr1Len < 20);     
  
  AttribComm = EFI_WHITE|EFI_BACKGROUND_BLUE;
  if(DialogColorIndex == 1){
    AttribComm = EFI_RED|BLT_BACKGROUND_WHITE;
  }
  AttribFocus = EFI_WHITE|EFI_BACKGROUND_BLACK;
  
  VA_START(Args, Title);
  MaxLength = 0;
  NumberOfLines = 0;
  while((String = VA_ARG (Args, CHAR16*)) != NULL) {
    MaxLength = MAX(MaxLength, UefiLibGetStringWidth(String, FALSE, 0, NULL) / 2);
    NumberOfLines++;
  }
  VA_END(Args);
  ASSERT(NumberOfLines != 0);
  ASSERT(MaxLength != 0);
  if(MaxLength < 40){MaxLength = 40;}

  ConOut = gST->ConOut;
  CopyMem(&SavedConsoleMode, ConOut->Mode, sizeof(SavedConsoleMode));
  ConOut->QueryMode(ConOut, SavedConsoleMode.Mode, &Columns, &Rows);
//DEBUG((EFI_D_ERROR, "(L%d) %d * %d\n", __LINE__, Columns, Rows));  
  ConOut->EnableCursor(ConOut, FALSE);
  ConOut->SetAttribute(ConOut, AttribComm);
  NumberOfLines = MIN(NumberOfLines, Rows - 7);
  MaxLength = MIN (MaxLength, Columns - 2);
  Row    = (Rows - (NumberOfLines + 7)) / 2;
  Column = (Columns - (MaxLength + 2)) / 2;
  Line = AllocateZeroPool((MaxLength + 3) * sizeof(CHAR16));
  ASSERT(Line != NULL);
//DEBUG((EFI_D_ERROR, "(L%d) %d %d %d %d\n", __LINE__, NumberOfLines, MaxLength, Row, Column));    

  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_DOWN_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_DOWN_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  Length = UefiLibGetStringWidth(Title, FALSE, 0, NULL) / 2;  
  ConOut->SetCursorPosition(ConOut, Column + 1 + (MaxLength - Length) / 2, Row++);
  ConOut->OutputString(ConOut, Title);

  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_VERTICAL_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);  
  
  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);  
  
  VA_START (Args, Title);
  while ((String = VA_ARG(Args, CHAR16 *)) != NULL && NumberOfLines > 0) {
    SetMem16(Line, (MaxLength + 2) * 2, L' ');
    Line[0]             = BOXDRAW_VERTICAL;
    Line[MaxLength + 1] = BOXDRAW_VERTICAL;
    Line[MaxLength + 2] = L'\0';
    ConOut->SetCursorPosition(ConOut, Column, Row);
    ConOut->OutputString(ConOut, Line);
    
    Length = UefiLibGetStringWidth(String, FALSE, 0, NULL) / 2;
    if (Length <= MaxLength) {
      ConOut->SetCursorPosition (ConOut, Column + 1 + (MaxLength - Length) / 2, Row++);
      ConOut->OutputString (ConOut, String);
    } else {
      UefiLibGetStringWidth (String, TRUE, MaxLength, &Length);
      TmpString = AllocateZeroPool ((Length + 1) * sizeof (CHAR16));
      ASSERT (TmpString != NULL);
      StrnCpy(TmpString, String, Length - 3);
      StrCat (TmpString, L"...");
      ConOut->SetCursorPosition (ConOut, Column + 1, Row++);
      ConOut->OutputString (ConOut, TmpString);
      FreePool(TmpString);
    }
    NumberOfLines--;
  }
  VA_END(Args);

  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);  
  
  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
//DEBUG((EFI_D_INFO, "(L%d) (%d,%d)\n", __LINE__, Column, Row));  
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  
  if(CStr1Len){
    Choice1X = Column + 1 + (MaxLength - StrLen(CStr[0]) - StrLen(CStr[1]) - 6)/3;
    Choice2X = Column + MaxLength - (Choice1X - Column) - StrLen(CStr[1]) - 2;
    ChoiceY  = Row++;
//  DEBUG((EFI_D_INFO, "(L%d) (%d,%d),(%d,%d) MaxL:%d X:%d\n", __LINE__, Choice1X, ChoiceY, Choice2X, ChoiceY, MaxLength, Column));
    ConOut->SetCursorPosition(ConOut, Choice1X, ChoiceY);
    ConOut->OutputString(ConOut, L"[");
    ConOut->SetAttribute(ConOut, AttribFocus);
    ConOut->OutputString(ConOut, CStr[0]);
    ConOut->SetAttribute(ConOut, AttribComm);  
    ConOut->OutputString(ConOut, L"]");
    ConOut->SetCursorPosition(ConOut, Choice2X, ChoiceY);
    ConOut->OutputString(ConOut, L"[");
    ConOut->OutputString(ConOut, CStr[1]);
    ConOut->OutputString(ConOut, L"]");    
    Choice1X++;
    Choice2X++;
  } else {
    String = CStr[0];
    Length = UefiLibGetStringWidth(String, FALSE, 0, NULL) / 2 + 2;
    ConOut->SetCursorPosition(ConOut, Column + 1 + (MaxLength - Length) / 2, Row++);
    ConOut->OutputString(ConOut, L"[");
    ConOut->SetAttribute(ConOut, AttribFocus);
    ConOut->OutputString(ConOut, CStr[0]);
    ConOut->SetAttribute(ConOut, AttribComm); 
    ConOut->OutputString(ConOut, L"]");     
  }  
 
  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_UP_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_UP_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);
  FreePool(Line);
  
  ChoiceIndex = 0;
  while (TRUE) {
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
    gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
    if(Key.ScanCode == SCAN_NULL && Key.UnicodeChar == CHAR_CARRIAGE_RETURN){
      break;
    } else if(CStr1Len &&
              (Key.ScanCode == SCAN_UP    || 
               Key.ScanCode == SCAN_DOWN  ||
               Key.ScanCode == SCAN_RIGHT || 
               Key.ScanCode == SCAN_LEFT)) {
      if(ChoiceIndex == 0){
        ChoiceIndex = 1;
      } else {
        ChoiceIndex = 0;
      }
      ConOut->SetCursorPosition(ConOut, Choice1X, ChoiceY);
      ConOut->SetAttribute(ConOut, ChoiceIndex==0?AttribFocus:AttribComm);
      ConOut->OutputString(ConOut, CStr[0]); 
      ConOut->SetCursorPosition(ConOut, Choice2X, ChoiceY);
      ConOut->SetAttribute(ConOut, ChoiceIndex==1?AttribFocus:AttribComm); 
      ConOut->OutputString(ConOut, CStr[1]);       
    } else if(Key.ScanCode == SCAN_ESC){
      ChoiceIndex = 0xFF;
      break;
    }    
  }  
  
  ConOut->EnableCursor(ConOut, SavedConsoleMode.CursorVisible);
  ConOut->SetCursorPosition(ConOut, SavedConsoleMode.CursorColumn, SavedConsoleMode.CursorRow);
  ConOut->SetAttribute(ConOut, SavedConsoleMode.Attribute);

  return ChoiceIndex;
}


/*--
  Send IPMI command to clear SEL Log.
--*/
EFI_STATUS
ClearSEL (
  VOID
  )
{
  EFI_STATUS                Status;
  EFI_SM_ELOG_PROTOCOL      *ElogProtocol;
  DEBUG((EFI_D_ERROR,"%a\n", __FUNCTION__));

  Status = gBS->LocateProtocol (&gEfiGenericElogProtocolGuid, NULL, &ElogProtocol);
  if (!EFI_ERROR (Status)) {
    //
    // Clear SEL
    //
    Status = ElogProtocol->EraseEventlogData (ElogProtocol, EfiElogSmIPMI, NULL);
  }

  return Status;
}
VOID 
SetFanManual (VOID)
{
  EFI_STATUS      Status;
  UINT8           CmdDataBuff[10];
  UINT8           CmdDataSize;
  UINT8           ResponseData[10];
  UINT8           ResponseSize;

  CmdDataBuff[0]=0x0F;  
  CmdDataBuff[1]=0x02;  
  CmdDataBuff[2]=0x00;  
  CmdDataSize = 3; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "It finish set Manual mode.\n"));     
  //save to NV
  CmdDataBuff[0]=0x0F;  
  CmdDataBuff[1]=0x05;  

  //Get fan speed we want to set
  //
  CmdDataBuff[2]=0x64;  
  CmdDataSize = 3; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "It finish save to NV.\n"));  
  //Effective immediately 
  CmdDataBuff[0]=0x00;  
  CmdDataBuff[1]=0x06;  
  CmdDataBuff[2]=0x64; 
  CmdDataBuff[3]=0x64;
  CmdDataBuff[4]=0x64; 
  CmdDataBuff[5]=0x64;  
  CmdDataBuff[6]=0x64; 
  CmdDataBuff[7]=0x64;  
  CmdDataBuff[8]=0x64; 
  CmdDataBuff[9]=0x64;                                
  CmdDataSize = 10; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "It finish set Effective immediately.\n"));  
  return;                    
}

VOID 
SetFanSpeed (UINT8 Speed)
{
  EFI_STATUS      Status;
  UINT8           CmdDataBuff[10];
  UINT8           CmdDataSize;
  UINT8           ResponseData[10];
  UINT8           ResponseSize;

  CmdDataBuff[0]=0x0F;  
  CmdDataBuff[1]=0x02;  
  CmdDataBuff[2]=0x00;  
  CmdDataSize = 3; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "It finish set Manual mode.\n"));     
  //save to NV
  CmdDataBuff[0]=0x0F;  
  CmdDataBuff[1]=0x05;  
  //Get fan speed we want to set
  CmdDataBuff[2]=Speed; //0x64;  
  CmdDataSize = 3; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "It finish save to NV.\n"));  
  //Effective immediately 
  CmdDataBuff[0]=0x00;  
  CmdDataBuff[1]=0x06;  
  CmdDataBuff[2]=Speed; //0x64; 
  CmdDataBuff[3]=Speed;//0x64;
  CmdDataBuff[4]=Speed;//0x64; 
  CmdDataBuff[5]=Speed;//0x64;  
  CmdDataBuff[6]=Speed;//0x64; 
  CmdDataBuff[7]=Speed;//0x64;  
  CmdDataBuff[8]=Speed;//0x64; 
  CmdDataBuff[9]=Speed;//0x64;                                
  CmdDataSize = 10; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "It finish set Effective immediately.\n"));  
  return;                    
}

VOID 
SetFanSilent (VOID)
{
  EFI_STATUS      Status;
  UINT8           CmdDataBuff[10]={0,0,0,0,0,0,0,0,0,0};
  UINT8           CmdDataSize;
  UINT8           ResponseData[10];
  UINT8           ResponseSize;

  CmdDataBuff[0]=0x0F;  
  CmdDataBuff[1]=0x02;  
  CmdDataBuff[2]=0x01;  
  CmdDataSize = 3; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "It finish set SILENT mode.\n")); 
  return;
}

VOID 
SetFanBalance (VOID)
{
  EFI_STATUS      Status;
  UINT8           CmdDataBuff[10];
  UINT8           CmdDataSize;
  UINT8           ResponseData[10];
  UINT8           ResponseSize;

  CmdDataBuff[0]=0x0F;  
  CmdDataBuff[1]=0x02;  
  CmdDataBuff[2]=0x02;  
  CmdDataSize = 3; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "It is finish set BALANCE mode.\n")); 
  return;
}

VOID 
SetFanPerformance (VOID)
{
  EFI_STATUS      Status;
  UINT8           CmdDataBuff[10];
  UINT8           CmdDataSize;
  UINT8           ResponseData[10];
  UINT8           ResponseSize;

  CmdDataBuff[0]=0x0F;  
  CmdDataBuff[1]=0x02;  
  CmdDataBuff[2]=0x03;  
  CmdDataSize = 3; 
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_GET_FUN_SPEED,
             CmdDataBuff,
             CmdDataSize,
             ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "It finish set PERFORMANCE mode.\n")); 
  return;
}
EFI_STATUS
UpdateBmcSelForm (
  IN  EFI_HII_HANDLE    HiiHandle
  );

EFI_STATUS
ClearBmcSelForm (
  IN  EFI_HII_HANDLE    HiiHandle
  );

EFI_STATUS
EFIAPI
ServerMgmtFormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
  EFI_STATUS        Status;

  CHAR16    *WaitSel;
  DEBUG((EFI_D_ERROR,"ServerMgmtFormCallback(), Action :%x. KeyValue = %d\n", Action,KeyValue));
  
  InitStr(HiiHandle);   

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN && KeyValue == KEY_OPEN_SEL_FORM) {
    WaitSel = HiiGetString(HiiHandle, STRING_TOKEN(STR_WAIT_BMC_SEL), NULL);

    CreatePopUp (EFI_BLUE | EFI_BACKGROUND_LIGHTGRAY, NULL, L"", WaitSel,  L" ", NULL);

    UpdateBmcSelForm (HiiHandle);

    if (WaitSel) FreePool (WaitSel);
    return EFI_INVALID_PARAMETER;
  }
  if (Action == EFI_BROWSER_ACTION_FORM_CLOSE && KeyValue == KEY_OPEN_SEL_FORM) {
    ClearBmcSelForm (HiiHandle);
    return EFI_INVALID_PARAMETER;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGING) {
    //
    // Do nothing for UEFI OPEN/CLOSE Action
    //
    return EFI_INVALID_PARAMETER;
  }
  Status = EFI_SUCCESS;
  switch (KeyValue) {
    case KEY_SERVER_MGMT_CLEAR_SEL:
      if(SELECTION_YES == UiConfirmDialog(DIALOG_YESNO, gClearElogTitile, NULL, TEXT_ALIGIN_CENTER, gClearElogMsg, NULL)){    //YES
        ClearSEL ();
      }
      break;

    case KEY_BMC_FAN_PROFILE: {
      if (Value->u8 == 0) {
        SetFanManual ();
      } 
      else if (Value->u8 == 1) {
        SetFanSilent(); 
      }
      else if (Value->u8 == 2) {
        SetFanBalance();
      } 
      else {
        SetFanPerformance();
      }
      }
      break;

    case KEY_BMC_FAN_SPEED_CONTROL:
      if (Value->u8 == 30) {
        SetFanSpeed(0x1e);
      }
      else if (Value->u8 == 50) {
        SetFanSpeed(0x32);      
      }
      else if (Value->u8 == 80)
      {
        SetFanSpeed(0x50);
      }
      else { 
        SetFanSpeed(0x64); 
      }
      break;

    default:
      Status = EFI_INVALID_PARAMETER;
      break;
  }

  return Status;
}





