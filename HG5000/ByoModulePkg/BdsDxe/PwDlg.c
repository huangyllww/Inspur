/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PWDlg.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/
#include <Uefi.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleTextInEx.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Library/ByoCommLib.h>
#include <Library/PrintLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Guid/MdeModuleHii.h>



#define MESSAGE_NORMAL_ATTRIBUTE     (EFI_WHITE | EFI_BACKGROUND_BLUE)           
#define MESSAGE_FOCUS_ATTRIBUTE      (EFI_WHITE | EFI_BACKGROUND_BLACK) 

extern EFI_HII_HANDLE   gHiiHandle;


typedef struct {
  UINTN  InputX;
  UINTN  InputY;
  CHAR16 *Password;
} PASSWORD_INPUT_LINE_CTX;

UINTN
UefiLibGetStringWidth (
  IN  CHAR16               *String,
  IN  BOOLEAN              LimitLen,
  IN  UINTN                MaxWidth,
  OUT UINTN                *Offset
  );

VOID 
ShowSingleChar16 (
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL  *ConOut,
  CHAR16                           c
  )
{
  CHAR16  Buffer[2];
  ZeroMem(Buffer, sizeof(Buffer));
  Buffer[0] = c;
  ConOut->OutputString(ConOut, Buffer);  
}  



typedef struct {
  UINTN    Left;
  UINTN    Right;
  UINTN    Top;
  UINTN    Bottom;
} MY_DIALOG_RANGE;


EFI_STATUS 
BltSaveAndRetoreArea (
  IN VOID        *SystemTable,
  IN BOOLEAN     Save,
  IN UINTN       Start,
  IN UINTN       End,
  IN UINTN       Top,
  IN UINTN       Bottom
  )
{
  EFI_STATUS                      Status = EFI_SUCCESS;
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *ptGO;
  UINTN                           Width;
  UINTN                           Height;
  EFI_BOOT_SERVICES               *pBS;   
  EFI_SYSTEM_TABLE                *pST;
  UINTN                           Size;
  STATIC MY_DIALOG_RANGE          MyRange;
  STATIC EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *BltBuffer = NULL;  
  UINTN                           Columns;
  UINTN                           Rows;
  UINT8                           DeltaXPersent;
  UINTN                           Offset;


  DeltaXPersent = PcdGet8(PcdGraphicsConsoleDeltaXPersent);

  pST = (EFI_SYSTEM_TABLE*)SystemTable;
  ASSERT(pST->Hdr.Signature == EFI_SYSTEM_TABLE_SIGNATURE);
  pBS = pST->BootServices;
  
  ptGO = (EFI_GRAPHICS_OUTPUT_PROTOCOL*)LibGetGOP(pBS);
  if(ptGO == NULL){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }    

  if(DeltaXPersent){
    if(DeltaXPersent > 50){
      DeltaXPersent = 50;
    }
    Offset = ptGO->Mode->Info->HorizontalResolution * DeltaXPersent / 100;
  } else {
    Offset = 0;
  }

  if(Save){

    if(BltBuffer != NULL){
      pBS->FreePool(BltBuffer);
    }  

    pST->ConOut->QueryMode (
                     pST->ConOut,
                     pST->ConOut->Mode->Mode,
                     &Columns,
                     &Rows
                     );
    if(Top > 0){
      Top--;
    }
    if(Bottom+1 < Rows){
      Bottom++;
    }

    Width = Offset + (End - Start + 1) * EFI_GLYPH_WIDTH;
    Height = (Bottom - Top + 1) * EFI_GLYPH_HEIGHT;
    Size = Width * Height * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
    
    BltBuffer = AllocatePool(Size);
    if(EFI_ERROR(Status)){
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;		
    }	
  
    Status = ptGO->Blt(
                    ptGO,
                    BltBuffer,
                    EfiBltVideoToBltBuffer,
                    Start * EFI_GLYPH_WIDTH,
                    Top * EFI_GLYPH_HEIGHT,
                    0,
                    0,
                    Width,
                    Height,
                    Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)  
                    );  
    MyRange.Left = Start;
    MyRange.Right = End;
    MyRange.Top = Top;
    MyRange.Bottom = Bottom;
    
  } else {                                // restore
    
    if(BltBuffer == NULL){
      Status = EFI_INVALID_PARAMETER;
      goto ProcExit;	      
    }

    Start  = MyRange.Left;
    End    = MyRange.Right;
    Top    = MyRange.Top;
    Bottom = MyRange.Bottom;

    Width = Offset + (End - Start + 1) * EFI_GLYPH_WIDTH;
    Height = (Bottom - Top + 1) * EFI_GLYPH_HEIGHT;
    
    Status = ptGO->Blt(
                    ptGO,
                    BltBuffer,
                    EfiBltBufferToVideo,
                    0,
                    0,
                    Start * EFI_GLYPH_WIDTH,
                    Top * EFI_GLYPH_HEIGHT,
                    Width,
                    Height,
                    Width * sizeof(EFI_GRAPHICS_OUTPUT_BLT_PIXEL)
                    );
	
    FreePool(BltBuffer);
    BltBuffer = NULL;    
  }
	
ProcExit:
  return Status;	
}



VOID
ByoPrintPostString (
  IN UINTN     Row,
  IN UINTN     Column,
  IN CHAR16    *String,
  IN UINTN     Attribute
  )
{
  if(String == NULL){
    return;
  }

  if(Attribute != 0){
    gST->ConOut->SetAttribute (gST->ConOut, Attribute);
  }
  gST->ConOut->SetCursorPosition (gST->ConOut, Column, Row);
  gST->ConOut->OutputString (gST->ConOut, String);
}



EFI_STATUS
EFIAPI
ByoDrawPasswordDialog (
  IN OUT CHAR16                      *NewPassword,
  IN     UINTN                       PasswordSize,
  IN     BYO_DRAW_PASSWORD_CALLBACK  Callback,
  IN     CHAR16                      *Title,
  OUT    UINTN                       *Reason
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  EFI_SIMPLE_TEXT_OUTPUT_MODE       SavedConsoleMode;
  UINTN                             Columns;
  UINTN                             Rows;
  UINTN                             Column;
  UINTN                             Row;
  UINTN                             NumberOfLines;
  UINTN                             MaxLength;
  UINTN                             Length;
  CHAR16                            *Line = NULL;
  UINTN                             Index = 0;
  UINTN                             LineIndex;
  PASSWORD_INPUT_LINE_CTX           LineCtx[3];
  UINT16                            *NewPassword2;
  UINT16                            *CurPassword2;
  BOOLEAN                           InitFlag;
  UINTN                             AttribTable, AttribInput;
  BOOLEAN                           IsKeyValid;
  EFI_KEY_DATA                      KeyData;
  EFI_INPUT_KEY                     *Key; 
  UINTN                             EventIndex;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *TextInEx;
  CHAR16                            *EnterCurPwStr;
  CHAR16                            *EnterNewPwStr;
  CHAR16                            *CfmNewPwStr;
  CHAR16                            *InputPwStr;
  UINTN                             PmtLen1, PmtLen2, PmtLen3, PmtLen = 0;
  UINTN                             i;
  BOOLEAN                           VerifyOld;
  

  if(Reason!=NULL){*Reason = 0;}

  if(gHiiHandle == NULL){
    return EFI_NOT_READY;
  }

  Status = gBS->LocateProtocol(&gEfiSimpleTextInputExProtocolGuid, NULL, &TextInEx);
  if(EFI_ERROR(Status)){
    return Status;
  }

  EnterCurPwStr = HiiGetString(gHiiHandle, STRING_TOKEN(STR_ENTER_CUR_PW), NULL);
  EnterNewPwStr = HiiGetString(gHiiHandle, STRING_TOKEN(STR_ENTER_NEW_PW), NULL);
  CfmNewPwStr   = HiiGetString(gHiiHandle, STRING_TOKEN(STR_CFM_NEW_PW), NULL);
  InputPwStr    = HiiGetString(gHiiHandle, STRING_TOKEN(STR_INPUT_PW), NULL);  
  
  PmtLen1 = UefiLibGetStringWidth(EnterCurPwStr, FALSE, 0, NULL) / 2;
  PmtLen2 = UefiLibGetStringWidth(EnterNewPwStr, FALSE, 0, NULL) / 2;
  PmtLen3 = UefiLibGetStringWidth(CfmNewPwStr,   FALSE, 0, NULL) / 2;
  if(PmtLen < PmtLen1){PmtLen = PmtLen1;}
  if(PmtLen < PmtLen2){PmtLen = PmtLen2;}
  if(PmtLen < PmtLen3){PmtLen = PmtLen3;}
//DEBUG((EFI_D_INFO, "%d %d %d %d\n", PmtLen1, PmtLen2, PmtLen3, PmtLen));
  
// Init.
  ASSERT(NewPassword != NULL || PasswordSize >= 2);
  PasswordSize--;
  if(Title == NULL){Title = InputPwStr;}

  AttribTable = MESSAGE_NORMAL_ATTRIBUTE;
  AttribInput = MESSAGE_FOCUS_ATTRIBUTE;
  
  ConOut = gST->ConOut;
  CopyMem(&SavedConsoleMode, ConOut->Mode, sizeof(SavedConsoleMode));
  ConOut->QueryMode(ConOut, SavedConsoleMode.Mode, &Columns, &Rows);
  ConOut->EnableCursor(ConOut, TRUE);
  ConOut->SetAttribute(ConOut, AttribTable);

  if(Callback(NULL) == EFI_UNSUPPORTED){
    VerifyOld = TRUE;
  } else {
    VerifyOld = FALSE;
  }
    
  NumberOfLines = VerifyOld?7:6;
  MaxLength = MIN(PmtLen + 1 + PasswordSize + 2, Columns - 2);
  MaxLength = MAX(MIN(Columns-2, 40), MaxLength);
  Row    = (Rows - NumberOfLines) / 2;
  Column = (Columns - (MaxLength + 2)) / 2;
  Line = AllocateZeroPool((MaxLength + 2 + 1) * sizeof(CHAR16));      // '|', xxxx, '|', '\0' 
  ASSERT(Line != NULL);
  NewPassword2 = AllocatePool((PasswordSize+1)*sizeof(CHAR16));
  ASSERT(NewPassword2!=NULL);
  CurPassword2 = AllocatePool((PasswordSize+1)*sizeof(CHAR16));
  ASSERT(CurPassword2!=NULL);

  BltSaveAndRetoreArea(gST, TRUE, Column, Column+MaxLength+2, Row, Row + NumberOfLines);
    
// first line  
  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_DOWN_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_DOWN_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

// title  
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
  
  if(VerifyOld){
    SetMem16(Line, (MaxLength + 2) * 2, L' ');
    Line[0]             = BOXDRAW_VERTICAL;
    Line[MaxLength + 1] = BOXDRAW_VERTICAL;
    Line[MaxLength + 2] = L'\0';
    ConOut->SetCursorPosition(ConOut, Column, Row);
    ConOut->OutputString(ConOut, Line);
    i = 1 + PmtLen - PmtLen1;
    ConOut->SetCursorPosition(ConOut, Column+i, Row);
    ConOut->OutputString(ConOut, EnterCurPwStr);
    i += PmtLen1 + 1;
    ConOut->SetCursorPosition(ConOut, Column + i, Row);
    Line[0] = L'[';
    SetMem16(Line+1, PasswordSize * 2, L' ');
    Line[1+PasswordSize] = L']';
    Line[1+PasswordSize+1] = 0;
    ConOut->OutputString(ConOut, Line);		
    LineCtx[0].InputX = Column+i+1;
    LineCtx[0].InputY = Row;   
    LineCtx[0].Password = CurPassword2;
    LineIndex = 0;    
    Row++;    
   
  } else {
    LineCtx[0].Password = NULL;
    LineIndex = 1;
  }


  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  i = 1 + PmtLen - PmtLen2;
  ConOut->SetCursorPosition(ConOut, Column+i, Row);
  ConOut->OutputString(ConOut, EnterNewPwStr);
  i += PmtLen2 + 1;
  ConOut->SetCursorPosition(ConOut, Column + i, Row);
  Line[0] = L'[';
  SetMem16(Line+1, PasswordSize * 2, L' ');
  Line[1+PasswordSize] = L']';
  Line[1+PasswordSize+1] = 0;
  ConOut->OutputString(ConOut, Line);		
  LineCtx[1].InputX = Column+i+1;
  LineCtx[1].InputY = Row;   
  LineCtx[1].Password = NewPassword;
  Row++; 

  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  i = 1 + PmtLen - PmtLen3;
  ConOut->SetCursorPosition(ConOut, Column+i, Row);
  ConOut->OutputString(ConOut, CfmNewPwStr);
  i += PmtLen3 + 1;
  ConOut->SetCursorPosition(ConOut, Column + i, Row);
  Line[0] = L'[';
  SetMem16(Line+1, PasswordSize * 2, L' ');
  Line[1+PasswordSize] = L']';
  Line[1+PasswordSize+1] = 0;
  ConOut->OutputString(ConOut, Line);		
  LineCtx[2].InputX = Column+i+1;
  LineCtx[2].InputY = Row;   
  LineCtx[2].Password = NewPassword2;
  Row++; 

  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_UP_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_UP_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

  
// Read Key
  InitFlag = FALSE;
  ConOut->SetAttribute(ConOut, AttribInput);  
  while (TRUE) {
    if(!InitFlag){

      if(LineIndex && LineCtx[LineIndex-1].Password != NULL){
        Length = StrLen(LineCtx[LineIndex-1].Password);
        SetMem16(Line, Length*2, L'*');        
        SetMem16(Line+Length, (PasswordSize - Length) * 2, L' ');
        Line[PasswordSize] = 0;
        ConOut->SetAttribute(ConOut, AttribTable);          
        ConOut->SetCursorPosition(ConOut, LineCtx[LineIndex-1].InputX, LineCtx[LineIndex-1].InputY);    
        ConOut->OutputString(ConOut, Line);          
      }
    
      ConOut->SetAttribute(ConOut, AttribInput);
      SetMem16(Line, PasswordSize * sizeof(CHAR16), L' ');
      Line[PasswordSize] = 0;
      ConOut->SetCursorPosition(ConOut, LineCtx[LineIndex].InputX, LineCtx[LineIndex].InputY);    
      ConOut->OutputString(ConOut, Line);    
			
      Index = 0;
      ConOut->SetCursorPosition(ConOut, LineCtx[LineIndex].InputX, LineCtx[LineIndex].InputY);
      InitFlag = TRUE;      
    }    

  	gBS->WaitForEvent(1, &TextInEx->WaitForKeyEx, &EventIndex);
  	TextInEx->ReadKeyStrokeEx(TextInEx, &KeyData);
  
    Key = &KeyData.Key;			
    IsKeyValid = FALSE;
    if(Key->ScanCode == SCAN_ESC){
      IsKeyValid = TRUE;
      Status = EFI_ABORTED;
      if(Reason!=NULL){*Reason = PASSWD_REASON_USER_CANCEL;}
      break;
    } else if(Key->ScanCode == SCAN_NULL){
      if (Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
        IsKeyValid = TRUE;
        LineCtx[LineIndex].Password[Index] = 0;         // give a end flag.
        if(LineIndex == 0){
          if(Index == 0 || Callback(LineCtx[0].Password) != EFI_SUCCESS){
            Status = EFI_INVALID_PARAMETER;
            if(Reason!=NULL){*Reason = PASSWD_REASON_WRONG_OLD;}
            break;
          }
        }
        if(++LineIndex >= 3){
          if(StrCmp(LineCtx[1].Password, LineCtx[2].Password) != 0){
            Status = EFI_INVALID_PARAMETER;
            if(Reason!=NULL){*Reason = PASSWD_REASON_NOT_EQUAL;}
          }
          break;
        }
        InitFlag = FALSE;
        continue;
      } else if(Key->UnicodeChar == CHAR_BACKSPACE){
        if(Index){
          Index--;  
          LineCtx[LineIndex].Password[Index] = 0;
          ShowSingleChar16(ConOut, CHAR_BACKSPACE);
          IsKeyValid = TRUE;
        }  
      }else {
        if(Index < PasswordSize){
          LineCtx[LineIndex].Password[Index] = (CHAR8)Key->UnicodeChar;
          Index++;
          ShowSingleChar16(ConOut, L'*');
          IsKeyValid = TRUE;
        }  
      }
    }

  }                    
  
  FreePool(Line);
  ZeroMem(NewPassword2, PasswordSize * sizeof(CHAR16));
  FreePool(NewPassword2);
  ZeroMem(CurPassword2, PasswordSize * sizeof(CHAR16));
  FreePool(CurPassword2);
  FreePool(EnterCurPwStr);
  FreePool(EnterNewPwStr);
  FreePool(CfmNewPwStr);
  FreePool(InputPwStr);
  ConOut->EnableCursor(ConOut, SavedConsoleMode.CursorVisible);
  ConOut->SetCursorPosition(ConOut, SavedConsoleMode.CursorColumn, SavedConsoleMode.CursorRow);
  ConOut->SetAttribute(ConOut, SavedConsoleMode.Attribute);

  if(EFI_ERROR(Status)){
    ZeroMem(NewPassword, PasswordSize * sizeof(CHAR16));
  }

  BltSaveAndRetoreArea(gST, FALSE, 0, 0, 0, 0);
  
  return Status;  
}






EFI_STATUS
ByoDrawPostPasswordDialog (
  IN  CHAR16                     *TitleStr,
  IN  CHAR16                     *Prompt,
  OUT CHAR16                     *Password,
  IN  UINTN                      PasswordSize
  )
{
  EFI_STATUS                        Status = EFI_SUCCESS;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  UINTN                             Columns;
  UINTN                             Rows;
  UINTN                             Column;
  UINTN                             Row;
  UINTN                             NumberOfLines;
  UINTN                             MaxLength;
  UINTN                             Length;
  CHAR16                            *Line = NULL;
  UINTN                             Index = 0;
  EFI_KEY_DATA                      KeyData;
  EFI_INPUT_KEY                     *Key; 
  UINTN                             EventIndex;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *TextInEx;
  UINTN                             x, y;
  UINTN                             LineCount;
  BOOLEAN                           PasswordOK = FALSE;
  

  Status = gBS->LocateProtocol(&gEfiSimpleTextInputExProtocolGuid, NULL, &TextInEx);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  ASSERT(TitleStr != NULL && Prompt != NULL);

  ConOut = gST->ConOut;
  ConOut->QueryMode(ConOut, ConOut->Mode->Mode, &Columns, &Rows);
  ConOut->EnableCursor(ConOut, TRUE);
  ConOut->SetAttribute(ConOut, MESSAGE_NORMAL_ATTRIBUTE);

  ASSERT(PasswordSize >= 2);
  PasswordSize--;
  
  NumberOfLines = 6;
  MaxLength = PasswordSize + 10;
  Length = UefiLibGetStringWidth(TitleStr, FALSE, 0, NULL) / 2 + 2;
  if(Length > MaxLength){
    MaxLength = Length;
  }
  Length = UefiLibGetStringWidth(Prompt, FALSE, 0, NULL) / 2 + 2;
  if(Length > MaxLength){
    MaxLength = Length;
  }  
  
  Row    = (Rows - NumberOfLines) / 2;
  Column = (Columns - (MaxLength + 2)) / 2;
  Line = AllocateZeroPool((MaxLength + 2 + 1) * sizeof(CHAR16));      // '|', xxxx, '|', '\0' 
  ASSERT(Line != NULL);

  BltSaveAndRetoreArea(gST, TRUE, Column, Column+MaxLength+2, Row, Row + NumberOfLines);

  
// first line  
  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_DOWN_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_DOWN_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

// title  
  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  Length = UefiLibGetStringWidth(TitleStr, FALSE, 0, NULL) / 2;  
  ConOut->SetCursorPosition(ConOut, Column + 1 + (MaxLength - Length) / 2, Row++);
  ConOut->OutputString(ConOut, TitleStr);

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
  ConOut->SetCursorPosition(ConOut, Column, Row);
  ConOut->OutputString(ConOut, Line);
  ConOut->SetCursorPosition(ConOut, Column+2, Row++);
  ConOut->OutputString(ConOut, Prompt); 

  SetMem16(Line, (MaxLength + 2) * 2, L' ');
  Line[0]             = BOXDRAW_VERTICAL;
  Line[MaxLength + 1] = BOXDRAW_VERTICAL;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);
 
  x = Column;
  y = Row - 1;

  SetMem16(Line, (MaxLength + 2) * 2, BOXDRAW_HORIZONTAL);
  Line[0]             = BOXDRAW_UP_RIGHT;
  Line[MaxLength + 1] = BOXDRAW_UP_LEFT;
  Line[MaxLength + 2] = L'\0';
  ConOut->SetCursorPosition(ConOut, Column, Row++);
  ConOut->OutputString(ConOut, Line);

  while(gST->ConIn->ReadKeyStroke(gST->ConIn, &KeyData.Key) == EFI_SUCCESS){};

  LineCount = 0;
  Index = 0;
  ConOut->SetCursorPosition(ConOut, x+2, y);
  while (TRUE) {

  	gBS->WaitForEvent(1, &TextInEx->WaitForKeyEx, &EventIndex);
  	TextInEx->ReadKeyStrokeEx(TextInEx, &KeyData);

    Key = &KeyData.Key;			
    if(Key->ScanCode == SCAN_NULL){
      if (Key->UnicodeChar == CHAR_CARRIAGE_RETURN) {
        if(Index){
          Password[Index] = 0;         // give a end flag.
          PasswordOK = TRUE;
          break;
        } else {
          continue;
        }

      } else if(Key->UnicodeChar == CHAR_BACKSPACE){
        if(Index){
          Index--;  
          Password[Index] = 0;
          ShowSingleChar16(ConOut, CHAR_BACKSPACE);
        }  
      }else {
        if(Index < PasswordSize){
          Password[Index] = (CHAR8)Key->UnicodeChar;
          Index++;
          ShowSingleChar16(ConOut, L'*');
        }  
      }
    }else if(Key->ScanCode == SCAN_ESC){
      break;
    }
  }                    


  BltSaveAndRetoreArea(gST, FALSE, 0, 0, 0, 0);

ProcExit:
  if(Line != NULL){
    FreePool(Line);
  }
  if(PasswordOK){
    return EFI_SUCCESS;
  } else {
    return EFI_ABORTED;
  }
}







