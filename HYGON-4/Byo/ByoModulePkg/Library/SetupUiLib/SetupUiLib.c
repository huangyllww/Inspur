/** @file
Copyright (c) 2019, Byosoft Software Corporation. All Rights Reserved.

You may not reproduce, distribute, publish, display, perform, modify, adapt,
transmit, broadcast, present, recite, release, license or otherwise exploit
any part of this publication in any form, by any means, without the prior
written permission of Byosoft Software Corporation.

File Name:
  SetupUiLib.c

Abstract:
  Setup Ui Library implementation.

Revision History:

**/

#include "SetupUiLibInternal.h"
#include <Library/UefiLib.h>
#include <Protocol/SystemPasswordProtocol.h>


EFI_GUID          gSetupUiLibGuid = { 0xeecdab61, 0x5444, 0x429d, { 0xa7, 0x89, 0x6, 0xf3, 0x40, 0x5a, 0xc5, 0xe4 } };
EFI_HII_HANDLE    mSetupUiStringHandle;

#define DIALOG_WIDTH_MIN    (44)
#define DIALOG_BLANK_LINE    (1)

BOOLEAN    bSetButtonTitle_YesNo = FALSE;
CHAR16    *gYesButtonTitle;
CHAR16    *gNoButtonTitle;
BOOLEAN    bSetButtonTitle_Warning = FALSE;
CHAR16    *gEnterButton;
BOOLEAN    bSetButtonTitle_Info = FALSE;
CHAR16    *gContinueButton;
DIALOG_TYPE    gLastDialogType = DIALOG_MAX;
BOOLEAN    mClearNoRepaintFlag;

//
// Current Boot video resolution and text mode.
//


/**
  Set Title of Button form Left to Right according Type of Dialog which will 
  be created by UiConfirmDialog(). Setting valid once.

**/
BOOLEAN
UiSetButton (
  IN DIALOG_TYPE    Type,
  IN  CHAR16    *String,
  ...
  ) 
{
  VA_LIST    Marker;
  CHAR16    *StringTemp;
  BOOLEAN    ReturnFlag;
  
  bSetButtonTitle_YesNo = FALSE;
  if (gYesButtonTitle) {
    FreePool (gYesButtonTitle );
    gYesButtonTitle = NULL;
  }
  if (gNoButtonTitle) {
    FreePool (gNoButtonTitle );
    gNoButtonTitle = NULL;
  }

  bSetButtonTitle_Warning = FALSE;
  if (gEnterButton) {
    FreePool (gEnterButton );
    gEnterButton = NULL;
  }

  bSetButtonTitle_Info = FALSE;
  if (gContinueButton) {
    FreePool (gContinueButton );
    gContinueButton = NULL;
  }
	
  ReturnFlag = TRUE;
  if (Type == DIALOG_YESNO) {
    StringTemp = String;
    VA_START (Marker, String);

    gYesButtonTitle = AllocateCopyPool(StrSize(StringTemp), StringTemp);
    ASSERT (NULL != gYesButtonTitle);

    StringTemp = VA_ARG (Marker, CHAR16*);
    gNoButtonTitle = AllocateCopyPool(StrSize(StringTemp), StringTemp);
    ASSERT (NULL != gNoButtonTitle);

    VA_END(Marker);
    bSetButtonTitle_YesNo = TRUE;
  } else   if (Type == DIALOG_WARNING) {
    gEnterButton = AllocateCopyPool(StrSize(String), String);
    ASSERT (NULL != gEnterButton);
    bSetButtonTitle_Warning = TRUE;
  } else   if (Type == DIALOG_INFO) {
    gContinueButton = AllocateCopyPool(StrSize(String), String);
    ASSERT (NULL != gContinueButton);
    bSetButtonTitle_Info = TRUE;
  } else {
    bSetButtonTitle_YesNo = FALSE;
    bSetButtonTitle_Warning = FALSE;
    bSetButtonTitle_Info = FALSE;
    ReturnFlag = FALSE;
  }

  return ReturnFlag;
}


#define EFI_HP_BACKGROUND_BLUE    EFI_BACKGROUND_CYAN


STATIC DLG_OEM_COLOR gOemColorIndex = DLG_OEM_COLOR_KEEP;

VOID SetOemDialogColor(DLG_OEM_COLOR Color)
{
  gOemColorIndex = Color;
}

VOID SetDlgCaption(CHAR16 *Caption)
{

}

/**
  Draw a confirm pop up windows based on the Type, strings and Text Align. 
  SELECTION_TYPE will be returned.

**/
SELECTION_TYPE
EFIAPI
UiConfirmDialogExInternal (
  IN  UINT32           Attribute,
  IN DIALOG_TYPE       Type,
  IN CHAR16            *Title,     OPTIONAL
  OUT EFI_INPUT_KEY    *KeyValue,  OPTIONAL
  IN TEXT_ALIGIN       Align,
  IN  CHAR16           *String,
  IN VA_LIST           MoreParameters
  )
{
  VA_LIST    Marker;
  UINTN    Index;
  UINTN    Count;
  EFI_SCREEN_DESCRIPTOR    ScreenDimensions;
  UINTN    DimensionsWidth;
  UINTN    DimensionsHeight;
  UINTN    Start;
  UINTN    End;
  UINTN    Top;
  UINTN    Bottom;
  UINTN    YesStart;
  UINTN    NoStart;
  UINTN    EnterStart;
  UINTN    ContinueStart;  
  CHAR16    Character;
  CHAR16    *StringTemp;
  CHAR16    *StringNew;
  CHAR16    *SetupConfirmation;
  CHAR16    *EnterButton;
  CHAR16    *ContinueButton;
  CHAR16    *Buffer;
  EFI_INPUT_KEY    Key;
  CHAR16    ResponseArray[2];
  UINTN    ArrayIndex;
  USER_INPUT_TYPE    InputType;
  MOUSE_ACTION_INFO    Mouse;
  MOUSE_CURSOR_RANGE    RangeYes;
  MOUSE_CURSOR_RANGE    RangeNo;
  MOUSE_CURSOR_RANGE    RangeEnter;  
  UINTN    NumberOfStrings;
  UINTN    MaxWidth;
  UINTN    BasicColor;
  UINTN    FrameColor;
  UINTN    DisableColor;
  UINTN    SelectedColor;
  CHAR16    *mYesResponse;
  CHAR16    *mNoResponse;
  UINTN    PrintWidth;
  SELECTION_TYPE    ReturnFlag;
  UINTN    OriginalAttribute;
  BOOLEAN    OriginalCursorVisible;
  
  BOOLEAN    NotRepaint;
  static UINTN    CountNotRepaint = 0;
  static UINTN    LastNumberOfStrings = 0;
  static UINTN    LastMaxWidth = 0;
  BOOLEAN         KeyBefore;
  UINT16    HpDeltaX;
  UINTN    TopGap;
  CHAR16   **StringList;
  UINTN    CRNum;
  UINTN    CRIndex;
  EFI_STATUS                          Status;
  BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL *Display;
  BACKUP_RECOVER_DATA                 *DisplayData = NULL;

  ZeroMem(&RangeYes, sizeof(MOUSE_CURSOR_RANGE));
  ZeroMem(&RangeNo, sizeof(MOUSE_CURSOR_RANGE));
  ZeroMem(&RangeEnter, sizeof(MOUSE_CURSOR_RANGE));

  if (Type == DIALOG_BROWSER_YESNO) {
    Type = DIALOG_YESNO;
  }

  while(gST->ConIn->ReadKeyStroke(gST->ConIn,&Key) == EFI_SUCCESS){};   // flush keyboard buffer

  //
  // Catch screen dimension info.
  //
  gST->ConOut->QueryMode (
                   gST->ConOut,
                   gST->ConOut->Mode->Mode,
                   &ScreenDimensions.RightColumn,
                   &ScreenDimensions.BottomRow
                   );
  DimensionsWidth = ScreenDimensions.RightColumn ;
  DimensionsHeight = ScreenDimensions.BottomRow;

  OriginalAttribute  = gST->ConOut->Mode->Attribute;
  OriginalCursorVisible = gST->ConOut->Mode->CursorVisible;
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);


  //
  // Calculate string lines and Max length.
  // 
  if (NULL != Title) {
    SetupConfirmation = Title;
  } else {
    if (Type == DIALOG_WARNING) {
      SetupConfirmation = UiGetToken (STRING_TOKEN (STR_WARNING), mSetupUiStringHandle);
    } else if(Type == DIALOG_INFO) {
      SetupConfirmation = UiGetToken (STRING_TOKEN (STR_INFO), mSetupUiStringHandle);
    } else {
      SetupConfirmation = UiGetToken (STRING_TOKEN (STR_CONFIRMATION_DIALOG), mSetupUiStringHandle);
    }
  }
  if (Type == DIALOG_BOX) {
    SetupConfirmation = NULL;
  }
  
  //
  // Convert String to String List
  //
  Marker = MoreParameters;
  NumberOfStrings = 0;
  StringTemp = String;
  while (NULL != StringTemp) {
    CRNum = UiGetStringCRNum(StringTemp) + 1;
    NumberOfStrings += CRNum;
    StringTemp = VA_ARG (Marker, CHAR16*);
  }
  StringList = AllocateZeroPool ((NumberOfStrings) * sizeof (CHAR16*) + 1);

  Marker = MoreParameters;
  Index = 0;
  StringTemp = String;
  while (NULL != StringTemp) {
    CRNum = UiGetStringCRNum(StringTemp) + 1;
    for(CRIndex = 0; CRIndex < CRNum; Index++, CRIndex++){
      StringList[Index] = UiGetStringCR(StringTemp, CRIndex);
    }
    StringTemp = VA_ARG (Marker, CHAR16*);
  }  

  StringList[NumberOfStrings] = NULL;

  //
  // Get the max width of the string
  //
  MaxWidth = DIALOG_WIDTH_MIN;
  for (Index = 0; Index < NumberOfStrings; Index ++) {
    if (UiGetStringWidth(StringList[Index])/2 > MaxWidth) {
      MaxWidth = UiGetStringWidth(StringList[Index])/2;
    }
  }

  if (UiGetStringWidth(SetupConfirmation)/2 > MaxWidth) {
    MaxWidth = UiGetStringWidth(SetupConfirmation)/2;
  }    
  if (MaxWidth > DimensionsWidth - 10) {
    MaxWidth = DimensionsWidth - 10;
  }
  if (NumberOfStrings > DimensionsHeight - 12) {
    NumberOfStrings = DimensionsHeight - 12;
   }
  MaxWidth += MaxWidth%2;

  //
  // Check Dialog Type.
  //
  if (Type >= DIALOG_MAX ) {
    Type = DIALOG_YESNO;
    DMSG ((EFI_D_ERROR, "\n UiConfirmDialog, Input DIALOG_TYPE exceed DIALOG_MAX.\n"));
  }

  if (gBltBuffer) {
    FreePool (gBltBuffer);
    gBltBuffer = NULL;
  }

  //
  // Check Repaint Flag for DIALOG_NO_REPAINT.
  //
  if (mClearNoRepaintFlag) {
    mClearNoRepaintFlag = FALSE;
    CountNotRepaint = 0;
    LastNumberOfStrings = 0;
    LastMaxWidth = 0;	
  }
  NotRepaint = FALSE;
  if (Type == DIALOG_NO_REPAINT) {
    CountNotRepaint ++;
    if (Type == gLastDialogType &&LastNumberOfStrings == NumberOfStrings && LastMaxWidth == MaxWidth) {
      NotRepaint = TRUE;
    } else {
      CountNotRepaint = 0;
    }
    LastNumberOfStrings = NumberOfStrings;
    LastMaxWidth = MaxWidth;
  } else {
    CountNotRepaint = 0;
    LastNumberOfStrings = 0;
    LastMaxWidth = 0;  
  }
  gLastDialogType = Type;

  //
  // style of Color.
  //
  HpDeltaX = PcdGet8(PcdGraphicsConsoleDeltaXPersent);
  if (HpDeltaX) {
    if (Type == DIALOG_YESNO || Type == DIALOG_INFO || Type == DIALOG_NO_KEY || Type == DIALOG_WAIT_KEY||Type == DIALOG_NO_REPAINT \
    	|| Type == DIALOG_BOX) {
      BasicColor = EFI_WHITE | EFI_HP_BACKGROUND_BLUE;    
      FrameColor = EFI_WHITE |EFI_HP_BACKGROUND_BLUE;
      DisableColor = EFI_LIGHTGRAY |EFI_HP_BACKGROUND_BLUE;
      SelectedColor = EFI_WHITE |EFI_BACKGROUND_BLACK;
    }else if (Type == DIALOG_WARNING) {
      BasicColor = EFI_WHITE | EFI_BACKGROUND_RED;
      FrameColor = EFI_WHITE |EFI_BACKGROUND_RED;
      DisableColor = EFI_LIGHTGRAY |EFI_BACKGROUND_RED;
      SelectedColor = EFI_WHITE |EFI_BACKGROUND_BLACK;
    } else {
      BasicColor = EFI_WHITE | EFI_HP_BACKGROUND_BLUE;
      FrameColor = EFI_WHITE |EFI_HP_BACKGROUND_BLUE;
      DisableColor = EFI_LIGHTGRAY |EFI_HP_BACKGROUND_BLUE;
      SelectedColor = EFI_WHITE |EFI_BACKGROUND_BLACK;
    }
  } else {
    if (Type == DIALOG_YESNO || Type == DIALOG_INFO || Type == DIALOG_NO_KEY || Type == DIALOG_WAIT_KEY||Type == DIALOG_NO_REPAINT) {
      BasicColor = PcdGet8(PcdConfirmPopupStringColor);
      FrameColor = PcdGet8(PcdConfirmPopupFrameColor);
      DisableColor = PcdGet8(PcdConfirmPopupDisableColor);
      SelectedColor = PcdGet8(PcdConfirmPopupSelectColor);  
    }else if (Type == DIALOG_WARNING || Type == DIALOG_BOX) {
      BasicColor = PcdGet8(PcdWarnPopupStringColor);
      FrameColor = PcdGet8(PcdWarnPopupFrameColor);
      DisableColor = PcdGet8(PcdWarnPopupDisableColor);
      SelectedColor = PcdGet8(PcdWarnPopupSelectColor);
    } else {
      BasicColor = PcdGet8(PcdConfirmPopupStringColor);
      FrameColor = PcdGet8(PcdConfirmPopupFrameColor);
      DisableColor = PcdGet8(PcdConfirmPopupDisableColor);
      SelectedColor = PcdGet8(PcdConfirmPopupSelectColor);  
    }
  }

  if(gOemColorIndex == DLG_OEM_COLOR_RED){
    BasicColor = EFI_WHITE | EFI_BACKGROUND_RED;
    FrameColor = EFI_GREEN |EFI_BACKGROUND_RED;
    DisableColor = EFI_LIGHTGRAY |EFI_BACKGROUND_RED;
    SelectedColor = EFI_WHITE |EFI_BACKGROUND_CYAN;
    gOemColorIndex = DLG_OEM_COLOR_KEEP;
  }

  
  //
  // Catch String every time.
  //
  if (bSetButtonTitle_YesNo) {
    mYesResponse = gYesButtonTitle;
    mNoResponse  = gNoButtonTitle;    
  } else {
    mYesResponse  = UiGetToken (STRING_TOKEN (ARE_YOU_SURE_YES), mSetupUiStringHandle);
    mNoResponse  = UiGetToken (STRING_TOKEN (ARE_YOU_SURE_NO), mSetupUiStringHandle);
  }

  //
  //Calculate Dimension.
  //
  Start = (DimensionsWidth - MaxWidth - 2) / 2;
  End = Start + MaxWidth + 2;

  if(Align == TEXT_ALIGIN_CENTER_LOWER){
    Top = DimensionsHeight * 2 / 3;
  } else {
    Top = (DimensionsHeight - NumberOfStrings - 6) / 2;
  }
  if (Type == DIALOG_BOX) {
    Bottom = Top + NumberOfStrings + 1;
  } else if (Type != DIALOG_NO_KEY && Type != DIALOG_WAIT_KEY && Type != DIALOG_NO_REPAINT && Type != DIALOG_WAIT_ENTER_KEY) {
    Bottom = Top + NumberOfStrings + 4 + DIALOG_BLANK_LINE;
  } else {
    Bottom = Top + NumberOfStrings + 4;
  }

 YesStart = Start  + (MaxWidth * 1)/4 - UiGetStringWidth(mYesResponse)/4;
  if (YesStart <= Start  + MaxWidth/16) {
     YesStart = Start  + MaxWidth/16;
  }
 NoStart =  Start  + (MaxWidth * 3)/4 - UiGetStringWidth(mNoResponse)/4;
 if (NoStart <= Start + (MaxWidth*9)/16) {
    NoStart = Start  + (MaxWidth * 9)/16;
 }

  if (bSetButtonTitle_Warning) {
    EnterButton = gEnterButton;
  } else {
    EnterButton = UiGetToken (STRING_TOKEN (STR_ENTER_BUTTON), mSetupUiStringHandle);
  }
  EnterStart = Start + (MaxWidth - UiGetStringWidth(EnterButton)/2)/2 +1;
  if (EnterStart <= Start + MaxWidth/4) {
    EnterStart = Start + MaxWidth/4;
  }

  if (bSetButtonTitle_Info) {
    ContinueButton = gContinueButton;
  } else {
    ContinueButton = UiGetToken (STRING_TOKEN (STR_CONTINUE), mSetupUiStringHandle);
  }
  ContinueStart = Start + (MaxWidth - UiGetStringWidth(ContinueButton)/2)/2 +1;
  if (ContinueStart <= Start + MaxWidth/4) {
    ContinueStart = Start + MaxWidth/4;
  }

  //
  // set the gap from top to print content.
  //
  if (Type == DIALOG_BOX) {
    TopGap = 1;
  } else {
    TopGap = 3;
  }
  
  //
  //Only repaint the content string for DIALOG_NO_REPAINT and DIALOG_BOX.
  // 
  if (NotRepaint && CountNotRepaint > 0) {
    DMSG ((EFI_D_ERROR, "UiConfirmDialog, DIALOG_NO_REPAINT, Only Repaint Content,\n"));    
    for (Index = Top, Count = 0; Count < NumberOfStrings; Index++, Count++) {
      gST->ConOut->SetAttribute (gST->ConOut, BasicColor);  	
  
      StringNew = AllocateCopyPool(StrSize(StringList[Count]), StringList[Count]);
      if (StringNew == NULL) {
        continue;
      }
	
      PrintWidth = UiGetStringWidth(StringNew)/2;
      if (PrintWidth > MaxWidth) {
        PrintWidth = UiUpdateStringMaxWidth (StringNew, MaxWidth) / 2;
      }
  	
      if ((DIALOG_ATTRIBUTE_NO_REFRESH & Attribute) == 0) {
        UiClearLines (Start+1 +1 , End-1 -1, Index + TopGap, Index + TopGap, BasicColor);
      }
  	
      if (TEXT_ALIGIN_CENTER == Align || Align == TEXT_ALIGIN_CENTER_LOWER) {
        UiPrintStringAt (
          ((DimensionsWidth - PrintWidth) / 2) + 1,
          Index + TopGap,
          StringNew
          );
      }	else if (TEXT_ALIGIN_LEFT == Align) {
        UiPrintStringAt (
          Start + 2,
          Index + TopGap,
          StringNew
          );
      }	else if (TEXT_ALIGIN_RIGHT == Align) {
        UiPrintStringAt (
          End - PrintWidth,
          Index + TopGap,
          StringNew
          );
      }
  
      if (StringNew) {
        FreePool (StringNew);
        StringNew = NULL;
      }
      gST->ConOut->SetAttribute (gST->ConOut, FrameColor);
    } 

    ReturnFlag = TRUE;
    goto EXIT_CONFIRM_DIALOG;
  }
  
  //
  // Store the Popup background.
  // 
  if (DIALOG_ATTRIBUTE_RECOVER & Attribute) {
    Status = gBS->LocateProtocol(&gDisplayBackupRecoverProtocolGuid, NULL, (VOID**)&Display);
    if (!EFI_ERROR(Status)) {
      Status = Display->DispalyBackup(&DisplayData);
      if (EFI_ERROR(Status)) {
        DisplayData = NULL;
      }
    } else {
      Display = NULL;
    }
  }

  if (DisplayData == NULL) {
    UiSaveStoreScreenRange(gBS, TRUE, Start, End, Top, Bottom);
  }
  
  //
  // Draw Dialog.
  //
  UiClearLines (Start, End, Top, Bottom, BasicColor);
  
  gST->ConOut->SetAttribute (gST->ConOut, FrameColor);
  Buffer = NULL;
  Buffer = AllocateZeroPool (0x100);
  ASSERT(NULL != Buffer);
  Character = BOXDRAW_HORIZONTAL;
  for(Index = 0; Index + 2 < End - Start; Index++){
    Buffer[Index] = Character;
  }

  Character = BOXDRAW_DOWN_RIGHT;
  UiPrintCharAt (Start, Top, Character);
  UiPrintString (Buffer);

  Character = BOXDRAW_DOWN_LEFT;
  UiPrintChar (Character);
  Character = BOXDRAW_VERTICAL;
  UiPrintCharAt (Start, Top + 1, Character);
  UiPrintCharAt (End - 1, Top + 1, Character);

  //
  // Dialog Title.
  //
  if (Type != DIALOG_BOX) {
  gST->ConOut->SetAttribute (gST->ConOut, BasicColor);
  if (UiGetStringWidth(SetupConfirmation)/2 > MaxWidth) {
    StringTemp = NULL;
    StringTemp = AllocateCopyPool(StrSize(SetupConfirmation), SetupConfirmation);
    if (StringTemp ) {
      UiUpdateStringMaxWidth (StringTemp, MaxWidth);
      UiPrintStringAt (((DimensionsWidth - MaxWidth) / 2) + 1, Top + 1, StringTemp);
      FreePool (StringTemp);
      StringTemp = NULL;
    }
  } else {
    UiPrintStringAt(((DimensionsWidth - UiGetStringWidth (SetupConfirmation) / 2) / 2) + 1, Top + 1, SetupConfirmation);
  }
  if (NULL == Title) {
    FreePool (SetupConfirmation);
  }
  }

  gST->ConOut->SetAttribute (gST->ConOut, FrameColor);
  if (Type != DIALOG_BOX) {
    Character = BOXDRAW_VERTICAL_RIGHT;
  }else {
    Character = BOXDRAW_VERTICAL;
  }
  UiPrintCharAt (Start, Top + 2, Character);

  if (Type != DIALOG_BOX) {
    UiPrintString (Buffer);
    Character = BOXDRAW_VERTICAL_LEFT;
    UiPrintChar (Character);
  } else {
    Character = BOXDRAW_VERTICAL;	
    UiPrintCharAt (End - 1, Top + 2, Character);
  }

  if (Type != DIALOG_BOX) {
    Character = BOXDRAW_VERTICAL;
    UiPrintCharAt (Start, Top + 3, Character);
    UiPrintCharAt (End - 1, Top + 3, Character);
    UiPrintCharAt (Start, Top + 4, Character);
    UiPrintCharAt (End - 1, Top + 4, Character);
  }
  
  if (Align >= TEXT_ALIGIN_MAX) {
    Align = TEXT_ALIGIN_CENTER;
    DMSG ((EFI_D_ERROR, "\n UiConfirmDialog, Input TEXT_ALIGIN exceed TEXT_ALIGIN_MAX.\n"));
  }
  
  Character = BOXDRAW_VERTICAL;
  for (Index = Top, Count = 0; Count < NumberOfStrings; Index++, Count++) {
    gST->ConOut->SetAttribute (gST->ConOut, BasicColor);  	

    StringNew = AllocateCopyPool(StrSize(StringList[Count]), StringList[Count]);
    if (StringNew == NULL) {
      continue;
    }

    PrintWidth = UiGetStringWidth(StringNew)/2;
    if (PrintWidth > MaxWidth) {
      PrintWidth = UiUpdateStringMaxWidth (StringNew, MaxWidth) / 2;
    }
	
    if (TEXT_ALIGIN_CENTER == Align || Align == TEXT_ALIGIN_CENTER_LOWER) {
      UiPrintStringAt (
        ((DimensionsWidth - PrintWidth) / 2) + 1,
        Index + TopGap,
        StringNew
        );
    } else if (TEXT_ALIGIN_LEFT == Align) {
      UiPrintStringAt (
        Start + 2,
        Index + TopGap,
        StringNew
        );
    }	else if (TEXT_ALIGIN_RIGHT == Align) {
      UiPrintStringAt (
        End - PrintWidth,
        Index + TopGap,
        StringNew
        );
    }

    if (StringNew) {
      FreePool (StringNew);
      StringNew = NULL;
    }
    gST->ConOut->SetAttribute (gST->ConOut, FrameColor);
    UiPrintCharAt (Start, Index + TopGap, Character);
    UiPrintCharAt (End - 1, Index + TopGap, Character);
  }

  if (NULL != StringList[Count]) {
      UiPrintStringAt (
        Start + 2,
        Index + TopGap,
        L"..."
        );
  }

  //
  // Print Blank Line.
  //
  //if (Type != DIALOG_NO_KEY && Type != DIALOG_WAIT_KEY && Type != DIALOG_WAIT_ENTER_KEY && Type != DIALOG_NO_REPAINT) {
  if (Type != DIALOG_NO_KEY && Type != DIALOG_WAIT_KEY && Type != DIALOG_NO_REPAINT) {
    for (Count = 0; Count < DIALOG_BLANK_LINE; Index++, Count++) {
      gST->ConOut->SetAttribute (gST->ConOut, FrameColor);
      UiPrintCharAt (Start, Index + TopGap, Character);
      UiPrintCharAt (End - 1, Index + TopGap, Character);
    }
  }

  for (Index = 0; Index < NumberOfStrings; Index++){
    FreePool(StringList[Index]);
  }

  FreePool(StringList);

  if (Type == DIALOG_YESNO) {
  gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
  UiPrintStringAt(NoStart, Bottom - 1, L"[");
  gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
  PrintWidth = UiPrintStringAtWithWidth(NoStart + 1, Bottom - 1, mNoResponse, (MaxWidth*3)/8);
  gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
  UiPrintStringAt(NoStart + 1 + PrintWidth, Bottom - 1, L"]");
  SetRange (&RangeNo, NoStart + 1, PrintWidth, Bottom - 1, 0);

  gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
  UiPrintStringAt(YesStart, Bottom - 1, L"[");
  gST->ConOut->SetAttribute (gST->ConOut, SelectedColor);
  PrintWidth = UiPrintStringAtWithWidth (YesStart + 1, Bottom - 1, mYesResponse, (MaxWidth*3)/8);  
  gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
  UiPrintStringAt(YesStart + 1 + PrintWidth, Bottom - 1, L"]");
  SetRange (&RangeYes, YesStart + 1, PrintWidth, Bottom - 1, 0);
  }

  if (Type == DIALOG_WARNING) {
  gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
  UiPrintStringAt(EnterStart, Bottom - 1, L"[");
  gST->ConOut->SetAttribute (gST->ConOut, SelectedColor);
  PrintWidth = UiPrintStringAtWithWidth (EnterStart + 1, Bottom - 1, EnterButton, MaxWidth/2);
  gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
  UiPrintStringAt(EnterStart + 1 + PrintWidth, Bottom - 1, L"]");
  SetRange (&RangeEnter, EnterStart + 1, PrintWidth, Bottom - 1, 0);
  }

  if (Type == DIALOG_INFO) {
  gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
  UiPrintStringAt(ContinueStart, Bottom - 1, L"[");
  gST->ConOut->SetAttribute (gST->ConOut, SelectedColor);
  PrintWidth = UiPrintStringAtWithWidth(ContinueStart + 1, Bottom - 1, ContinueButton, MaxWidth/2);
  gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
  UiPrintStringAt(ContinueStart + 1 + PrintWidth, Bottom - 1, L"]");
  SetRange (&RangeEnter, ContinueStart + 1, PrintWidth, Bottom - 1, 0);
  }
  
  gST->ConOut->SetAttribute (gST->ConOut, FrameColor);
  Character = BOXDRAW_VERTICAL;
  UiPrintCharAt (Start, Bottom - 1, Character);
  UiPrintCharAt (End - 1, Bottom - 1, Character);

  Character = BOXDRAW_UP_RIGHT;
  UiPrintCharAt (Start, Bottom, Character);
  UiPrintString (Buffer);
  Character = BOXDRAW_UP_LEFT;
  UiPrintChar (Character);

  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  FreePool (Buffer);
  

  //
  //Confirm Dialog Process.
  //
  ReturnFlag = SELECTION_ESC;
  if (Type == DIALOG_YESNO) {
    ResponseArray[0] = L'Y';
    ResponseArray[1] = L'N';
    ArrayIndex = 0;

    KeyBefore = FALSE;
    
    do {

      InputType = WaitForUserInput(&Key, &Mouse);
//-   DEBUG((EFI_D_INFO, "IT:%d\n", InputType));

      if(!KeyBefore && (InputType == USER_INPUT_KEY || InputType == USER_INPUT_MOUSE)){
        KeyBefore = TRUE;
      }

      if(!KeyBefore && InputType == USER_INPUT_TIMEOUT){
        ReturnFlag = SELECTION_YES;
        goto EXIT_CONFIRM_DIALOG;
      }

      
      if (USER_INPUT_MOUSE == InputType) {
          if (IsMouseInRange (&RangeYes, &Mouse)) {
            if (MOUSE_LEFT_CLICK == Mouse.Action) {
              if (ArrayIndex == 1) {
                Key.UnicodeChar = CHAR_NULL;
                Key.ScanCode = SCAN_RIGHT;
                ArrayIndex = 1;
              } else {
                Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
                Key.ScanCode = SCAN_NULL;
                ArrayIndex = 0;
              }
            }
            if (MOUSE_LEFT_DOUBLE_CLICK == Mouse.Action) {
              Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
              Key.ScanCode = SCAN_NULL;
              ArrayIndex = 0;
            }
          }
          if (IsMouseInRange (&RangeNo, &Mouse)) {
            if (MOUSE_LEFT_CLICK == Mouse.Action) {
              if (ArrayIndex == 0) {
                Key.UnicodeChar = CHAR_NULL;
                Key.ScanCode = SCAN_RIGHT;
                ArrayIndex = 0;
              } else {
                Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
                Key.ScanCode = SCAN_NULL;
                ArrayIndex = 1;
              }
            }
            if (MOUSE_LEFT_DOUBLE_CLICK == Mouse.Action) {
              Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
              Key.ScanCode = SCAN_NULL;
              ArrayIndex = 1;
            }
          }
          if (MOUSE_RIGHT_CLICK == Mouse.Action) {
            Key.UnicodeChar = CHAR_NULL;
            Key.ScanCode = SCAN_ESC;
            ArrayIndex = 1;
          }
      }

      if(InputType == USER_INPUT_KEY || InputType == USER_INPUT_MOUSE){

        if (Key.UnicodeChar == CHAR_TAB) {
          Key.UnicodeChar = CHAR_NULL;
          Key.ScanCode = SCAN_LEFT;
        }
        
        switch (Key.UnicodeChar){
        case CHAR_NULL:
          switch (Key.ScanCode) {
          case SCAN_UP:
          case SCAN_LEFT:
            if(ArrayIndex == 1){
              ArrayIndex = 0;
              gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
              UiPrintStringAtWithWidth(NoStart + 1, Bottom - 1, mNoResponse, (MaxWidth*3)/8);

              gST->ConOut->SetAttribute (gST->ConOut, SelectedColor);
              UiPrintStringAtWithWidth(YesStart + 1, Bottom - 1, mYesResponse, (MaxWidth*3)/8);
            } else {
              ArrayIndex = 1;
              gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
              UiPrintStringAtWithWidth(YesStart + 1, Bottom - 1, mYesResponse, (MaxWidth*3)/8);

              gST->ConOut->SetAttribute (gST->ConOut, SelectedColor);
              UiPrintStringAtWithWidth(NoStart + 1, Bottom - 1, mNoResponse, (MaxWidth*3)/8);
            }
            break;

          case SCAN_DOWN:
          case SCAN_RIGHT:
            if(ArrayIndex == 0){
              ArrayIndex = 1;
              gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
              UiPrintStringAtWithWidth(YesStart + 1, Bottom - 1, mYesResponse, (MaxWidth*3)/8);

              gST->ConOut->SetAttribute (gST->ConOut, SelectedColor);
              UiPrintStringAtWithWidth(NoStart + 1, Bottom - 1, mNoResponse, (MaxWidth*3)/8);
            } else {
              ArrayIndex = 0;
              gST->ConOut->SetAttribute (gST->ConOut, DisableColor);
              UiPrintStringAtWithWidth(NoStart + 1, Bottom - 1, mNoResponse, (MaxWidth*3)/8);

              gST->ConOut->SetAttribute (gST->ConOut, SelectedColor);
              UiPrintStringAtWithWidth(YesStart + 1, Bottom - 1, mYesResponse, (MaxWidth*3)/8);
            }
            break;

          case SCAN_ESC:
            ReturnFlag = SELECTION_ESC;
            goto EXIT_CONFIRM_DIALOG;

          default:
            break;
          }
          break;

        case CHAR_CARRIAGE_RETURN:
          if((ResponseArray[ArrayIndex] | UPPER_LOWER_CASE_OFFSET) == (L'Y'| UPPER_LOWER_CASE_OFFSET)){
            gST->ConOut->SetCursorPosition (gST->ConOut, End-2, Bottom + 1);
            gST->ConOut->EnableCursor (gST->ConOut, FALSE);
            ReturnFlag = SELECTION_YES;
            goto EXIT_CONFIRM_DIALOG;
          }
          ReturnFlag = SELECTION_NO;
          goto EXIT_CONFIRM_DIALOG;

        case 'Y': 
        case 'y':
          gST->ConOut->SetCursorPosition (gST->ConOut, End-2, Bottom + 1);
          gST->ConOut->EnableCursor (gST->ConOut, FALSE);
          ReturnFlag = SELECTION_YES;
          goto EXIT_CONFIRM_DIALOG;

        case 'N': 
        case 'n':
          ReturnFlag = SELECTION_NO;
          goto EXIT_CONFIRM_DIALOG;

        default:
          break;
        }
      }
    } while (TRUE);
  }

  //
  //Warning Dialog Process.
  //
  ReturnFlag = SELECTION_ESC;
  if (Type == DIALOG_WARNING || Type == DIALOG_INFO) {
    do {
      InputType = WaitForUserInput(&Key, &Mouse);
      if (USER_INPUT_MOUSE == InputType) {
        if (IsMouseInRange (&RangeEnter, &Mouse)) {
          if (MOUSE_LEFT_CLICK == Mouse.Action || MOUSE_LEFT_DOUBLE_CLICK == Mouse.Action) {
            Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
            Key.ScanCode = SCAN_NULL;
          }
        }
        if (MOUSE_RIGHT_CLICK == Mouse.Action) {
          Key.UnicodeChar = CHAR_NULL;
          Key.ScanCode = SCAN_ESC;
        }
      }
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        gST->ConOut->SetCursorPosition (gST->ConOut, End-2, Bottom + 1);
        gST->ConOut->EnableCursor (gST->ConOut, FALSE);
        ReturnFlag = SELECTION_YES;
        goto EXIT_CONFIRM_DIALOG;
      } else if (Key.ScanCode == SCAN_ESC ) {
        gST->ConOut->SetCursorPosition (gST->ConOut, End-2, Bottom + 1);
        gST->ConOut->EnableCursor (gST->ConOut, FALSE);
        ReturnFlag = SELECTION_ESC;
        goto EXIT_CONFIRM_DIALOG;
      }

    } while (TRUE);
  }

  //
  //Wait key Dialog Process.
  //
  ReturnFlag = SELECTION_MAX;
  if (Type == DIALOG_WAIT_KEY) {
    InputType = WaitForUserInput(&Key, &Mouse);
    if (USER_INPUT_MOUSE == InputType) {
      if (MOUSE_RIGHT_CLICK == Mouse.Action) {
        Key.UnicodeChar = CHAR_NULL;
        Key.ScanCode = SCAN_ESC;
      }
    }
    if(KeyValue != NULL) {
      CopyMem(KeyValue, &Key, sizeof(EFI_INPUT_KEY));
    }
    gST->ConOut->SetCursorPosition (gST->ConOut, End-2, Bottom + 1);
    gST->ConOut->EnableCursor (gST->ConOut, FALSE);
    goto EXIT_CONFIRM_DIALOG;
  }

  //
  //Wait Enter key Dialog Process.
  //
  ReturnFlag = SELECTION_MAX;
  if (Type == DIALOG_WAIT_ENTER_KEY) {
    do{
      InputType = WaitForUserInput(&Key, &Mouse);
      if (USER_INPUT_MOUSE == InputType) {
        if (MOUSE_RIGHT_CLICK == Mouse.Action) {
          Key.UnicodeChar = CHAR_NULL;
          Key.ScanCode = SCAN_ESC;
        }
      }
    }while(Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

    if(KeyValue != NULL) {
      CopyMem(KeyValue, &Key, sizeof(EFI_INPUT_KEY));
    }

    ReturnFlag = SELECTION_ENTER;
    gST->ConOut->SetCursorPosition (gST->ConOut, End-2, Bottom + 1);
    gST->ConOut->EnableCursor (gST->ConOut, FALSE);
    goto EXIT_CONFIRM_DIALOG;
  }

  EXIT_CONFIRM_DIALOG:  

  if (!bSetButtonTitle_YesNo) {
    if (mYesResponse) { 
      FreePool (mYesResponse);
    }
    if (mNoResponse) { 
      FreePool (mNoResponse);
    }
  } else {
    if (gYesButtonTitle) {
      FreePool (gYesButtonTitle );
      gYesButtonTitle = NULL;
    }
    if (gNoButtonTitle) {
      FreePool (gNoButtonTitle );
      gNoButtonTitle = NULL;
    }
  }

  if (!bSetButtonTitle_Warning) {
    FreePool (EnterButton);
  } else {
    if (gEnterButton) {
      FreePool (gEnterButton );
      gEnterButton = NULL;
    }
  }

  if (!bSetButtonTitle_Info) {
    FreePool (ContinueButton);
  } else {
    if (gContinueButton) {
      FreePool (gContinueButton );
      gContinueButton = NULL;
    }
  }

  if (Type == DIALOG_BOX_DEAD_LOOP) {
    gBS->RaiseTPL(TPL_HIGH_LEVEL);
    CpuDeadLoop();
  }

  if(Type != DIALOG_NO_KEY && 
     !NotRepaint && 
     Type != DIALOG_BOX  && 
     Type != DIALOG_NO_REPAINT){

    if ((DIALOG_ATTRIBUTE_RECOVER & Attribute) && Display != NULL && DisplayData != NULL) {
      Display->DispalyRecover(DisplayData);
    } else {
      UiSaveStoreScreenRange(gBS, FALSE, 0, 0, 0, 0);
    }
  }
  
  bSetButtonTitle_YesNo = FALSE;
  bSetButtonTitle_Warning = FALSE;
  bSetButtonTitle_Info = FALSE;

  gST->ConOut->SetAttribute (gST->ConOut, OriginalAttribute);
  gST->ConOut->EnableCursor (gST->ConOut, OriginalCursorVisible);
  
  if (DisplayData != NULL) {
    FreePool(DisplayData);
  }

  return ReturnFlag;
}

SELECTION_TYPE
EFIAPI
UiConfirmDialog (
  IN DIALOG_TYPE       Type,
  IN CHAR16            *Title,     OPTIONAL
  OUT EFI_INPUT_KEY    *KeyValue,  OPTIONAL
  IN TEXT_ALIGIN       Align,
  IN  CHAR16           *String,
  ...
  )
{
  SELECTION_TYPE  Ret;
  VA_LIST         Marker;

  VA_START (Marker, String);
  Ret = UiConfirmDialogExInternal(0, Type, Title, KeyValue, Align, String, Marker);
  VA_END(Marker);

  return Ret;
}

SELECTION_TYPE
EFIAPI
UiConfirmDialogEx (
  IN  UINT32            Attribute,
  IN  DIALOG_TYPE       Type,
  IN  CHAR16            *Title,     OPTIONAL
  OUT EFI_INPUT_KEY    *KeyValue,   OPTIONAL
  IN  TEXT_ALIGIN       Align,
  IN  CHAR16           *String,
  ...
  )
{
  SELECTION_TYPE  Ret;
  VA_LIST         Marker;

  VA_START (Marker, String);
  Ret = UiConfirmDialogExInternal(Attribute, Type, Title, KeyValue, Align, String, Marker);
  VA_END(Marker);

  return Ret;
}

STATIC UINT64 gDlgShowTimeOut = 0; 

VOID SetDlgTimeOut(UINT8 TimeOutSeconds)
{
  if(TimeOutSeconds == 0){
    gDlgShowTimeOut = 0;
  } else {
    gDlgShowTimeOut = EFI_TIMER_PERIOD_SECONDS(TimeOutSeconds);
  }
}



/**
  Wait for user Input by Key or Mouse.

  @param Key         The key which is pressed by user.
  @param Mouse      The Mouse which is pressed by user.

  @retval USER_INPUT_TYPE  The Input type

**/
USER_INPUT_TYPE
WaitForUserInput (
  OUT  EFI_INPUT_KEY    *Key,
  OUT  MOUSE_ACTION_INFO *Mouse
  )
{
  EFI_STATUS       Status;
  UINTN            Index;
  EFI_EVENT        WaitList[8];
  EFI_SETUP_MOUSE_PROTOCOL    *SetupMouse;
  EFI_EVENT        TimerEvent = NULL;
  UINTN            EventCount = 0;
  UINTN            MouseEventIndex = 0xFF;
  UINTN            TimeEventIndex  = 0xFF;
  USER_INPUT_TYPE  InputType;
  

//DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (Key == NULL || Mouse == NULL) {
    return USER_INPUT_MAX;
  }

  WaitList[EventCount++] = gST->ConIn->WaitForKey;
//DEBUG((EFI_D_INFO, "%a(): 1 EventCount = %d\n", __FUNCTION__, EventCount));

  SetupMouse = NULL;
  Status = gBS->LocateProtocol (
                  &gEfiSetupMouseProtocolGuid,
                  NULL,
                  (VOID **) &SetupMouse
                  );
  if (SetupMouse && PcdGetBool(PcdTextSetupMouseEnable)) {
    MouseEventIndex = EventCount;
    WaitList[EventCount++] = SetupMouse->WaitForMouse;
//  DEBUG((EFI_D_INFO, "%a(): 2 EventCount = %d\n", __FUNCTION__, EventCount));
  }
  
  if(gDlgShowTimeOut){
    gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);
    gBS->SetTimer (TimerEvent, TimerRelative, gDlgShowTimeOut);
    TimeEventIndex = EventCount;
    WaitList[EventCount++] = TimerEvent;
//  DEBUG((EFI_D_INFO, "%a(): 3 EventCount = %d\n", __FUNCTION__, EventCount));
  }


  InputType = USER_INPUT_MAX;
  
  while (TRUE) {

    Status = gBS->WaitForEvent(EventCount, WaitList, &Index);
    if(EFI_ERROR(Status)) {
//    DEBUG((EFI_D_INFO, "%a(): Status = %r\n", __FUNCTION__, Status));
      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, Key);
      if (!EFI_ERROR (Status)) {
        while (gST->ConIn) {
          //
          // clean key buffer.
          //
          EFI_INPUT_KEY KeyTmp; 
          Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &KeyTmp);
          if (EFI_ERROR(Status)) {
            break;
          }
        }
        InputType = USER_INPUT_KEY;
        break;
      }
    }
//  DEBUG((EFI_D_INFO, "%a(): Index = %d\n", __FUNCTION__, Index));
    if (Index == MouseEventIndex && (SetupMouse != NULL)) {
      SetupMouse->GetData(Mouse);
      InputType = USER_INPUT_MOUSE;
      break;
      
    } else if(Index == 0) {
      Status = gST->ConIn->ReadKeyStroke (gST->ConIn, Key);
//    DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
      InputType = USER_INPUT_KEY;
      break;
      
    } else if(Index == TimeEventIndex){
      InputType = USER_INPUT_TIMEOUT;
      break;
    }
  }

  if(TimerEvent != NULL){
    gBS->CloseEvent(TimerEvent);
  }

  return InputType;
}

/**
  Wait for user Input by Key or Mouse.

  @param Key        The key which is pressed by user.
  @param Mouse      The Mouse which is pressed by user.

  @retval USER_INPUT_TYPE  The Input type

**/
USER_INPUT_TYPE
WaitForUserInputEx (
  OUT  EFI_KEY_DATA      *Key,
  OUT  MOUSE_ACTION_INFO *Mouse
  )
{
  EFI_STATUS       Status;
  UINTN            Index;
  EFI_EVENT        WaitList[8];
  EFI_SETUP_MOUSE_PROTOCOL    *SetupMouse;
  EFI_EVENT        TimerEvent = NULL;
  UINTN            EventCount = 0;
  UINTN            MouseEventIndex = 0xFF;
  UINTN            TimeEventIndex  = 0xFF;
  USER_INPUT_TYPE  InputType;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *TextInputEx = NULL;


  if (Key == NULL || Mouse == NULL) {
    return USER_INPUT_MAX;
  }

  Status = gBS->HandleProtocol(
                  gST->ConsoleInHandle,
                  &gEfiSimpleTextInputExProtocolGuid,
                  (VOID**)&TextInputEx
                  );
  if (EFI_ERROR(Status)) {
    TextInputEx = NULL;
  }
  WaitList[EventCount++] = TextInputEx == NULL ? gST->ConIn->WaitForKey : TextInputEx->WaitForKeyEx;

  SetupMouse = NULL;
  Status = gBS->LocateProtocol (
                  &gEfiSetupMouseProtocolGuid,
                  NULL,
                  (VOID **) &SetupMouse
                  );
  if (SetupMouse && PcdGetBool(PcdTextSetupMouseEnable)) {
    MouseEventIndex = EventCount;
    WaitList[EventCount++] = SetupMouse->WaitForMouse;
  }
  
  if(gDlgShowTimeOut){
    gBS->CreateEvent (EVT_TIMER, 0, NULL, NULL, &TimerEvent);
    gBS->SetTimer (TimerEvent, TimerRelative, gDlgShowTimeOut);
    TimeEventIndex = EventCount;
    WaitList[EventCount++] = TimerEvent;
  }

  InputType = USER_INPUT_MAX;
  
  while (TRUE) {

    Status = gBS->WaitForEvent(EventCount, WaitList, &Index);
    if(EFI_ERROR(Status)) {
      if (TextInputEx != NULL) {
        Status = TextInputEx->ReadKeyStrokeEx(TextInputEx, Key);
      } else {
        Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key->Key);
      }
      if (!EFI_ERROR (Status)) {
        while (gST->ConIn) {
          //
          // clean key buffer.
          //
          EFI_KEY_DATA KeyTmp;
          if (TextInputEx != NULL) {
            Status = TextInputEx->ReadKeyStrokeEx(TextInputEx, &KeyTmp);
          } else {
            Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &KeyTmp.Key);
          }
          if (EFI_ERROR(Status)) {
            break;
          }
        }
        InputType = USER_INPUT_KEY;
        break;
      }
    }
    if (Index == MouseEventIndex && (SetupMouse != NULL)) {
      SetupMouse->GetData(Mouse);
      InputType = USER_INPUT_MOUSE;
      break;
      
    } else if(Index == 0) {
      if (TextInputEx != NULL) {
        Status = TextInputEx->ReadKeyStrokeEx(TextInputEx, Key);
      } else {
        Status = gST->ConIn->ReadKeyStroke (gST->ConIn, &Key->Key);
      }
      InputType = USER_INPUT_KEY;
      break;
      
    } else if(Index == TimeEventIndex){
      InputType = USER_INPUT_TIMEOUT;
      break;
    }
  }

  if(TimerEvent != NULL){
    gBS->CloseEvent(TimerEvent);
  }

  return InputType;
}

/**
  Check whether mouse in the range.

  @param Range      The Range of a item.
  @param Mouse      The Mouse which is pressed by user.

  @retval BOOLEAN  The Input type

**/
BOOLEAN 
IsMouseInRange (
  IN  MOUSE_CURSOR_RANGE    *Range,
  IN  MOUSE_ACTION_INFO    *Mouse
)
{
  if (Range == NULL || Mouse == NULL) {
    return FALSE;
  }

  if (Range->StartX <= Mouse->Column && 
    Range->EndX >  Mouse->Column &&
    Range->StartY <= Mouse->Row &&
    Range->EndY >= Mouse->Row ) {

    return TRUE;
  }

  return FALSE;
}

/**
  Set range of item.

  @param Range      The Range of a item.
  @param StartColum
  @param Length
  @param StartRow
  @param Height
  
  @retval EFI_STATUS

**/
EFI_STATUS 
SetRange (
  IN  MOUSE_CURSOR_RANGE    *Range,
  IN  UINTN    StartColum,
  IN  UINTN    Length,
  IN  UINTN    StartRow,
  IN  UINTN    Height 
)
{
  if (Range == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Range->StartX = StartColum;
  Range->EndX = StartColum + Length;

  Range->StartY = StartRow;
  Range->EndY = StartRow + Height;

  return EFI_SUCCESS;
}

/**
  Constructor of Customized Display Library Instance.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
SetupUiLibConstructor (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{

  InitializeLanguage(TRUE);

  mSetupUiStringHandle = HiiAddPackages (&gSetupUiLibGuid, ImageHandle, SetupUiLibStrings, NULL);
  ASSERT (mSetupUiStringHandle != NULL);
  DMSG ((EFI_D_ERROR, "SetupUiLibConstructor, mSetupUiStringHandle :0x%x. \n", mSetupUiStringHandle));

  return EFI_SUCCESS;
}

/**
  Destructor of Customized Display Library Instance.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The destructor completed successfully.
  @retval Other value   The destructor did not complete successfully.

**/
EFI_STATUS
EFIAPI
SetupUiLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{  
  DMSG ((EFI_D_ERROR, "SetupUiLibDestructor, mSetupUiStringHandle :0x%x. \n", mSetupUiStringHandle));

  HiiRemovePackages(mSetupUiStringHandle);

  if (gBltBuffer != NULL) {
    FreePool (gBltBuffer);
    gBltBuffer = NULL;
  }
  
  if (gYesButtonTitle) {
    FreePool (gYesButtonTitle );
    gYesButtonTitle = NULL;
  }

  if (gNoButtonTitle) {
    FreePool (gNoButtonTitle );
    gNoButtonTitle = NULL;
  }
  
  if (gEnterButton) {
    FreePool (gEnterButton );
    gEnterButton = NULL;
  }

  if (gContinueButton) {
    FreePool (gContinueButton );
    gContinueButton = NULL;
  }

  return EFI_SUCCESS;
}


/**
  Restore Background before creating it and Free memory.
  This service  is only for DIALOG_NO_KEY dialog now.

**/
BOOLEAN
UiClearDialog (
  IN DIALOG_TYPE    Type
  ) 
{
  BOOLEAN    ReturnFlag;
  
  ReturnFlag = FALSE;
  if (Type == gLastDialogType) {
    if ((gLastDialogType == DIALOG_NO_KEY || gLastDialogType == DIALOG_BOX) && gBltBuffer != NULL) {
      UiSaveStoreScreenRange(gBS, FALSE, 0, 0, 0, 0);
      ReturnFlag = TRUE;
    }
  }

  return ReturnFlag;
}


/**
  Restore No Repaint flag after end a serial of No Repaint Dailog.
  This service  is only for DIALOG_NO_REPAINT dialog now.

**/
BOOLEAN
UiClearNoRepaintFlag (
  IN DIALOG_TYPE    Type
  ) 
{
  BOOLEAN    ReturnFlag;
  
  ReturnFlag = FALSE;
  if (Type == DIALOG_NO_REPAINT || Type == DIALOG_BOX) {
    mClearNoRepaintFlag = TRUE;
    ReturnFlag = TRUE;
  } else {
    mClearNoRepaintFlag = FALSE;
  }

  return ReturnFlag;
}

/**
  This function will change video resolution and text mode
  according to defined setup mode or defined boot mode

  @param  IsSetupMode   Indicate mode is changed to setup mode or boot mode.

  @retval  EFI_SUCCESS  Mode is changed successfully.
  @retval  Others             Mode failed to be changed.

**/
EFI_STATUS
EFIAPI
SetConsoleMode (
  BOOLEAN  IsSetupMode
  )
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL          *GraphicsOutput;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL       *SimpleTextOut;
  UINTN                                 SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION  *Info;
  UINT32                                MaxGopMode;
  UINT32                                MaxTextMode;
  UINT32                                ModeNumber;
  UINT32                                NewHorizontalResolution;
  UINT32                                NewVerticalResolution;
  UINT32                                NewColumns;
  UINT32                                NewRows;
  UINTN                                 HandleCount;
  EFI_HANDLE                            *HandleBuffer;
  EFI_STATUS                            Status;
  UINTN                                 Index;
  UINTN                                 CurrentColumn;
  UINTN                                 CurrentRow;  
  UINTN                                 BootTextColumn;
  UINTN                                 BootTextRow;
  UINT8                                 DeltaXPersent;
  STATIC UINT32                         mBootHorizontalResolution = 0;
  STATIC UINT32                         mBootVerticalResolution = 0;
  STATIC UINT32                         mBootTextModeColumn = 0;
  STATIC UINT32                         mBootTextModeRow = 0;  


  MaxGopMode  = 0;
  MaxTextMode = 0;

  DMSG ((EFI_D_ERROR, "\n SetConsoleMode(), IsSetupMode :%d. \n", IsSetupMode));
  //
  // Get current video resolution and text mode 
  //
  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID**)&GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
  }

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiSimpleTextOutProtocolGuid,
                  (VOID**)&SimpleTextOut
                  );
  if (EFI_ERROR (Status)) {
    SimpleTextOut = NULL;
  }

  if ((GraphicsOutput == NULL) || (SimpleTextOut == NULL)) {
    return EFI_UNSUPPORTED;
  }

  if (IsSetupMode) {
    //
    // Save Current resolution and text mode is setup mode.
    //
    if (GraphicsOutput != NULL) {
      //
      // Get current video resolution and text mode.
      //
      if (mBootHorizontalResolution < GraphicsOutput->Mode->Info->HorizontalResolution) {
        mBootHorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
        mBootVerticalResolution   = GraphicsOutput->Mode->Info->VerticalResolution;
      }
    }

    if (SimpleTextOut != NULL) {
      Status = SimpleTextOut->QueryMode (
                                SimpleTextOut,
                                SimpleTextOut->Mode->Mode,
                                &BootTextColumn,
                                &BootTextRow
                                );
      if (mBootTextModeColumn < BootTextColumn) {
        mBootTextModeColumn = (UINT32)BootTextColumn;
        mBootTextModeRow    = (UINT32)BootTextRow;
      }
    }
    //
    // The required resolution and text mode is setup mode.
    //
    NewHorizontalResolution = PcdGet32 (PcdSetupVideoHorizontalResolution);
    NewVerticalResolution = PcdGet32 (PcdSetupVideoVerticalResolution);
    NewColumns = PcdGet32 (PcdSetupConOutColumn);
    NewRows = PcdGet32 (PcdSetupConOutRow);
    if(NewHorizontalResolution == 0 || NewVerticalResolution == 0){
      NewHorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
      NewVerticalResolution   = GraphicsOutput->Mode->Info->VerticalResolution;
    }
    if(NewColumns == 0 || NewRows == 0){
      NewColumns = NewHorizontalResolution/8;
      NewRows = NewVerticalResolution/19;
    }

    DeltaXPersent = PcdGet8(PcdGraphicsConsoleDeltaXPersent);
    if(DeltaXPersent){
      if(DeltaXPersent > 50){
        DeltaXPersent = 50;
      }
      NewColumns = NewColumns * (100 - DeltaXPersent) / 100;
    }
    DEBUG((EFI_D_INFO, "SetConsoleMode(), Setup Resolution :%d-%d, %d-%d. \n", NewHorizontalResolution, NewVerticalResolution, NewColumns, NewRows));
  } else {
    if (mBootHorizontalResolution == 0 ||mBootVerticalResolution == 0 ||mBootTextModeColumn == 0 ||mBootTextModeRow == 0) {
      DEBUG ((EFI_D_ERROR, "Resolution Error, Set SetConsoleMode() to TRUE to gather resolution data. \n"));
      return EFI_UNSUPPORTED;
    }
    //
    // The required resolution and text mode is boot mode.
    //
    NewHorizontalResolution = mBootHorizontalResolution;
    NewVerticalResolution = mBootVerticalResolution;
    NewColumns = mBootTextModeColumn;
    NewRows = mBootTextModeRow;
  }
  DEBUG((EFI_D_INFO, "SetConsoleMode(), Boot Resolution :%d-%d, %d-%d. \n", mBootHorizontalResolution, mBootVerticalResolution, mBootTextModeColumn, mBootTextModeRow));

  if (GraphicsOutput != NULL) {
    MaxGopMode  = GraphicsOutput->Mode->MaxMode;
  } 

  if (SimpleTextOut != NULL) {
    MaxTextMode = SimpleTextOut->Mode->MaxMode;
  }

  //
  // 1. If current video resolution is same with required video resolution,
  //    video resolution need not be changed.
  //    1.1. If current text mode is same with required text mode, text mode need not be changed.
  //    1.2. If current text mode is different from required text mode, text mode need be changed.
  // 2. If current video resolution is different from required video resolution, we need restart whole console drivers.
  //
  for (ModeNumber = 0; ModeNumber < MaxGopMode; ModeNumber++) {
    Status = GraphicsOutput->QueryMode (
                       GraphicsOutput,
                       ModeNumber,
                       &SizeOfInfo,
                       &Info
                       );
    if (!EFI_ERROR (Status)) {
      if ((Info->HorizontalResolution == NewHorizontalResolution) &&
          (Info->VerticalResolution == NewVerticalResolution)) {
        if ((GraphicsOutput->Mode->Info->HorizontalResolution == NewHorizontalResolution) &&
            (GraphicsOutput->Mode->Info->VerticalResolution == NewVerticalResolution)) {
          //
          // Current resolution is same with required resolution, check if text mode need be set
          //
          Status = SimpleTextOut->QueryMode (SimpleTextOut, SimpleTextOut->Mode->Mode, &CurrentColumn, &CurrentRow);
          ASSERT_EFI_ERROR (Status);
          if (CurrentColumn == NewColumns && CurrentRow == NewRows) {
            //
            // If current text mode is same with required text mode. Do nothing
            //
            FreePool (Info);
            return EFI_SUCCESS;
          } else {
            //
            // If current text mode is different from required text mode.  Set new video mode
            //
            for (Index = 0; Index < MaxTextMode; Index++) {
              Status = SimpleTextOut->QueryMode (SimpleTextOut, Index, &CurrentColumn, &CurrentRow);
              if (!EFI_ERROR(Status)) {
                if ((CurrentColumn == NewColumns) && (CurrentRow == NewRows)) {
                  //
                  // Required text mode is supported, set it.
                  //
                  Status = SimpleTextOut->SetMode (SimpleTextOut, Index);
                  ASSERT_EFI_ERROR (Status);
                  //
                  // Update text mode PCD.
                  //
                  PcdSet32S (PcdConOutColumn, PcdGet32 (PcdSetupConOutColumn));
                  PcdSet32S (PcdConOutRow, PcdGet32 (PcdSetupConOutRow));
                  FreePool (Info);
                  return EFI_SUCCESS;
                }
              }
            }
            if (Index == MaxTextMode) {
              //
              // If required text mode is not supported, return error.
              //
              FreePool (Info);
              return EFI_UNSUPPORTED;
            }
          }
        } else {
          //
          // If current video resolution is not same with the new one, set new video resolution.
          // In this case, the driver which produces simple text out need be restarted.
          //
          Status = GraphicsOutput->SetMode (GraphicsOutput, ModeNumber);
          if (!EFI_ERROR (Status)) {
            FreePool (Info);
            break;
          }
        }
      }
      FreePool (Info);
    }
  }

  if (ModeNumber == MaxGopMode) {
    //
    // If the resolution is not supported, return error.
    //
    return EFI_UNSUPPORTED;
  }

  DEBUG ((EFI_D_ERROR, " SetConsoleMode(), Set new resolution, \n" ));
  //
  // Set PCD to Inform GraphicsConsole to change video resolution.
  // Set PCD to Inform Consplitter to change text mode.
  //
  PcdSet32S (PcdVideoHorizontalResolution, NewHorizontalResolution);
  PcdSet32S (PcdVideoVerticalResolution, NewVerticalResolution);
  PcdSet32S (PcdConOutColumn, NewColumns);
  PcdSet32S (PcdConOutRow, NewRows);
  
  //
  // Video mode is changed, so restart graphics console driver and higher level driver.
  // Reconnect graphics console driver and higher level driver.
  // Locate all the handles with GOP protocol and reconnect it.
  //
  Status = gBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiSimpleTextOutProtocolGuid,
                   NULL,
                   &HandleCount,
                   &HandleBuffer
                   );
  if (!EFI_ERROR (Status)) {
    for (Index = 0; Index < HandleCount; Index++) {
      gBS->DisconnectController (HandleBuffer[Index], NULL, NULL);
    }
    for (Index = 0; Index < HandleCount; Index++) {
      gBS->ConnectController (HandleBuffer[Index], NULL, NULL, TRUE);
    }
    if (HandleBuffer != NULL) {
      FreePool (HandleBuffer);
    }
  }

  gST->ConOut->EnableCursor(gST->ConOut, FALSE);

  return EFI_SUCCESS;
}

VOID
PrintStringOnConsole (
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
ShowPostPasswordDialog (
  IN  CHAR16                     *TitleStr,
  IN  CHAR16                     *Prompt,      
  IN  CHAR16                     *HelpStr,      // password space
  IN  CHAR16                     *ReminderStr,  // last line
  IN  CHAR16                     *ErrorStr,     // error info
  IN  EFI_KEY_DATA               *KeyData,      // for F1 key
  OUT CHAR16                     *Password,
  IN  UINTN                      PasswordSize
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
  UINTN                                Index;
  EFI_STATUS                           Status;
  EFI_SYSTEM_PASSWORD_PROTOCOL         *SystemPassword;
  UINTN                                NumberOfStrings;
  UINTN                                CRNum;
  UINTN                                CRIndex;
  CHAR16                               *StringTemp;
  CHAR16                               **StringList;
  UINTN                                TitleLen;

  AttribTable = (EFI_WHITE | EFI_BACKGROUND_BLUE) ; 
  ConOut = gST->ConOut;
  CopyMem(&SavedConsoleMode, ConOut->Mode, sizeof(SavedConsoleMode));
  ConOut->QueryMode(ConOut, SavedConsoleMode.Mode, &Columns, &Rows);
  ConOut->EnableCursor(ConOut, FALSE);
  ConOut->SetAttribute(ConOut, AttribTable);

  TitleLen = UiGetStringWidth (TitleStr)/2;
  MaxLength = MAX (TitleLen, PasswordSize ) + 2 ;
  Line = AllocateZeroPool((MaxLength + 3) * sizeof(CHAR16));
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

  //
  // Convert String to String List
  //
  NumberOfStrings = 0;
  StringTemp = TitleStr;
  if (NULL != StringTemp) {
    CRNum = UiGetStringCRNum(StringTemp) + 1;
    NumberOfStrings += CRNum;
  }
  StringList = AllocateZeroPool ((NumberOfStrings) * sizeof (CHAR16*) + 1);

  Index = 0;
  StringTemp = TitleStr;
  if (NULL != StringTemp) {
    CRNum = UiGetStringCRNum(StringTemp) + 1;
    for(CRIndex = 0; CRIndex < CRNum; Index++, CRIndex++){
      StringList[Index] = UiGetStringCR(StringTemp, CRIndex);
    }
  }  

  for (Index = 0; Index < NumberOfStrings; Index++) {
    ConOut->SetCursorPosition(ConOut, Column + 3 , Row++);
    ConOut->OutputString(ConOut, StringList[Index]);
    SetMem16(Line, (MaxLength + 2) * 2, L' ');
    Line[0]             = BOXDRAW_VERTICAL;
    Line[MaxLength + 1] = BOXDRAW_VERTICAL;
    Line[MaxLength + 2] = L'\0';
    ConOut->SetCursorPosition(ConOut, Column, Row);
    ConOut->OutputString(ConOut, Line);
  }


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
  
  Line1 = AllocateZeroPool(PasswordSize * sizeof(CHAR16));
  SetMem16(Line1, PasswordSize * 2, L' ');
  Line1[PasswordSize] = L'\0';
  ConOut->SetAttribute(ConOut, EFI_TEXT_ATTR (EFI_BLACK, EFI_BLACK));
  ConOut->SetCursorPosition(ConOut, Column +3, Row -2);
  ConOut->OutputString(ConOut, Line1);

  Status = gBS->LocateProtocol(&gEfiSystemPasswordProtocolGuid, NULL, (VOID**)&SystemPassword);
  ASSERT_EFI_ERROR(Status);
  Status = SystemPassword->Input(Column +3, Row -2, Password);

  for (Index = 0; Index < NumberOfStrings; Index++){
    FreePool(StringList[Index]);
  }
  FreePool(StringList);

  return Status;
}

EFI_HANDLE
RegisterWindows (
  IN  UINT32 X, 
  IN  UINT32 Y,
  IN  UINT32 Width,
  IN  UINT32 Height,
  IN  BOOLEAN Top
  )
{
  return NULL;
}

EFI_STATUS
UnRegisterWindows(
  IN  EFI_HANDLE Windows
  )
{
  return EFI_UNSUPPORTED;
}

VOID*
AddDynamicallyUpdatePop(
  IN  EFI_HANDLE *Windows,
  IN  CHAR16     *Title   OPTIONAL
  )
{
  return NULL;
}

EFI_STATUS
RefreshDynamicallyUpdatePop(
  IN  VOID     *Control, 
  IN  BOOLEAN  Refresh,
  IN  CHAR16   *RateStr
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
DeleteDynamicallyUpdatePop (
  IN  VOID       *Control
  )
{
  return EFI_UNSUPPORTED;
}
