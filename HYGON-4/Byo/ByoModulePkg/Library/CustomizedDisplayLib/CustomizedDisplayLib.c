/** @file

  This library class defines a set of interfaces to customize Display module

Copyright (c) 2013 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/
#include "CustomizedDisplayLibInternal.h"

EFI_GUID          gCustomizedDisplayLibGuid = { 0x99fdc8fd, 0x849b, 0x4eba, { 0xad, 0x13, 0xfb, 0x96, 0x99, 0xc9, 0xa, 0x4d } };
EFI_SCREEN_DESCRIPTOR         gStatementDimensions;

EFI_HII_HANDLE    mCDLStringPackHandle;
UINT16            gClassOfVfr;                 // Formset class information
BOOLEAN           gLibIsFirstForm = TRUE;
BANNER_DATA       *gBannerData;

UINTN             gFooterHeight;

typedef enum {
  FirstPrint,
  DeletePassword,
  InputPassword,
  MoveCursor
} PASSWORD_DEAL_TYPE;

/**
+------------------------------------------------------------------------------+
|                           Byosoft ByoCore BIOS                               |
| Main | Advanced | Security | Boot | Exit |                                   |
+-------------------------------------------------------------+----------------+
|                                                             |  Help Message  |
| Item                         Info                           |----------------|
| Item                         Info                           |                |
| Item                         Info                           |                |
|                                                             |                |
|                                                             |                |
|                                                             |                |
|                                                             |                |
|                                                             |                |
+-------------------------------------------------------------+----------------+
|  F1  Help    v ^  Select Item    -/+   Change Value       F9  Setup Defaults  |
|  Esc Exit    < >  Select Menu    Enter Select Sub-Menu    F10 Save & Exit     |
|                  Copyright(C)2006-2021,Byosoft Co., Ltd                      |
+------------------------------------------------------------------------------+
**/

/**
  This function defines Page Frame and Background. 
  
  Based on the above layout, it will be responsible for HeaderHeight, FooterHeight, 
  StatusBarHeight and Background. And, it will reserve Screen for Statement. 

  @param[in]  FormData             Form Data to be shown in Page.
  
  @return Status
**/
EFI_STATUS
EFIAPI
DisplayPageFrame (
  IN FORM_DISPLAY_ENGINE_FORM       *FormData
  )
{
  EFI_STATUS             Status;
  UINTN    LayoutStyle;
  BYO_BROWSER_FORMSET    *ByoFormSet;
  UINT16    LastClassOfVfr = FORMSET_CLASS_PLATFORM_SETUP;

  ASSERT (FormData != NULL);
  if (FormData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  DMSG ((EFI_D_ERROR, "\n DisplayPageFrame, Begin, \n"));
  //
  //Get layout style. 0, for main menu; 1, for sub menu.
  //
  LayoutStyle = 1;
  if (NULL != FormData->ByoCurrentFormSetLink) {
    ByoFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (FormData->ByoCurrentFormSetLink);
    if (ByoFormSet->HiiHandle == FormData->HiiHandle) {
      if (ByoFormSet->FirstFormId == FormData->FormId) {
        LayoutStyle = 0;
        gLastFormId = 0xFFFF;
      } 
    } 
  }
  FormData->ByoLayoutStyle = LayoutStyle;    
  
  Status = ScreenDimensionInfoValidate (FormData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  gClassOfVfr = FORMSET_CLASS_PLATFORM_SETUP;

  ProcessExternedOpcode(FormData);

  //
  // Calculate the ScreenForStatement.
  //
  if (gClassOfVfr == FORMSET_CLASS_PLATFORM_SETUP) {
    gStatementDimensions.BottomRow   = gScreenDimensions.BottomRow - STATUS_BAR_HEIGHT - gFooterHeight + 2;  
    gStatementDimensions.TopRow    = gScreenDimensions.TopRow + NONE_FRONT_PAGE_HEADER_HEIGHT + 1;
    gStatementDimensions.LeftColumn  = gScreenDimensions.LeftColumn + 1;
    gStatementDimensions.RightColumn = gScreenDimensions.RightColumn - 1;
  } else {
    gStatementDimensions.BottomRow   = gScreenDimensions.BottomRow - STATUS_BAR_HEIGHT - gFooterHeight;  
    gStatementDimensions.TopRow    = gScreenDimensions.TopRow + FRONT_PAGE_HEADER_HEIGHT;
    gStatementDimensions.LeftColumn  = gScreenDimensions.LeftColumn;
    gStatementDimensions.RightColumn = gScreenDimensions.RightColumn;
  }

  if ((gLibIsFirstForm) || ((FormData->Attribute & HII_DISPLAY_MODAL) != 0) ||(LastClassOfVfr != gClassOfVfr)) {
    //
    // Ensure we are in Text mode
    //
    if (gClassOfVfr == FORMSET_CLASS_PLATFORM_SETUP) {
      ClearLines (0, gScreenDimensions.RightColumn, 2, gScreenDimensions.BottomRow -3, EFI_BACKGROUND_LIGHTGRAY);
    } else {
      ClearLines (0, gScreenDimensions.RightColumn, 0, gScreenDimensions.BottomRow, KEYHELP_BACKGROUND); //  EFI_BACKGROUND_LIGHTGRAY
    }
  }

  //
  // Don't print frame for modal form.
  //
  if ((FormData->Attribute & HII_DISPLAY_MODAL) != 0) {
    return EFI_SUCCESS;
  }

  if (gClassOfVfr == FORMSET_CLASS_FRONT_PAGE) {
    PrintBannerInfo (FormData);
  }

  PrintFramework (FormData);

  UpdateStatusBar(NV_UPDATE_REQUIRED, FormData->SettingChangedFlag);

  if (gLibIsFirstForm) {
    gLibIsFirstForm = FALSE;
  }
  if (LastClassOfVfr != gClassOfVfr) {
    LastClassOfVfr = gClassOfVfr;
    gLibIsFirstForm = TRUE;	
  }

  DMSG ((EFI_D_ERROR, "DisplayPageFrame, End. \n"));
  return EFI_SUCCESS;
}

/**
  This function updates customized key panel's help information.
  The library will prepare those Strings for the basic key, ESC, Enter, Up/Down/Left/Right, +/-.
  and arrange them in Footer panel.
  
  @param[in]  FormData       Form Data to be shown in Page. FormData has the highlighted statement. 
  @param[in]  Statement      The statement current selected.
  @param[in]  Selected       Whether or not a tag be selected. TRUE means Enter has hit this question.
**/
VOID
EFIAPI
RefreshKeyHelp (
  IN FORM_DISPLAY_ENGINE_FORM      *FormData,
  IN FORM_DISPLAY_ENGINE_STATEMENT *Statement,
  IN  BOOLEAN                      Selected
  )
{
  UINTN                  SecCol;
  UINTN                  ThdCol;
  UINTN                  RightColumnOfHelp;
  UINTN                  TopRowOfHelp;
  UINTN                  BottomRowOfHelp;
  UINTN                  StartColumnOfHelp;
  EFI_IFR_NUMERIC        *NumericOp;
  EFI_IFR_DATE           *DateOp;
  EFI_IFR_TIME           *TimeOp;
  BOOLEAN                HexDisplay;
  UINTN                  ColumnWidth1;
  UINTN                  ColumnWidth2;
  UINTN                  ColumnWidth3;
  CHAR16                 *ColumnStr1;
  CHAR16                 *ColumnStr2;
  CHAR16                 *ColumnStr3;

  ASSERT (FormData != NULL);
  if (FormData == NULL) {
    return;
  }

  if ((FormData->Attribute & HII_DISPLAY_MODAL) != 0 || (gClassOfVfr != FORMSET_CLASS_FRONT_PAGE)) {
    return;
  }
  gST->ConOut->SetAttribute (gST->ConOut, KEYHELP_TEXT | KEYHELP_BACKGROUND);

  SecCol            = gScreenDimensions.LeftColumn + (gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn) / 3;
  ThdCol            = gScreenDimensions.LeftColumn + (gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn) / 3 * 2;

  //
  // + 2 means leave 1 space before the first hotkey info.
  //
  StartColumnOfHelp = gScreenDimensions.LeftColumn + 2;
  RightColumnOfHelp = gScreenDimensions.RightColumn - 1;
  TopRowOfHelp      = gScreenDimensions.BottomRow - STATUS_BAR_HEIGHT - gFooterHeight + 1;
  BottomRowOfHelp   = gScreenDimensions.BottomRow - STATUS_BAR_HEIGHT - 2;

  ColumnWidth1      = SecCol - StartColumnOfHelp;
  ColumnWidth2      = ThdCol - SecCol;
  ColumnWidth3      = RightColumnOfHelp - ThdCol;
  ColumnStr1        = gLibEmptyString;
  ColumnStr2        = gLibEmptyString;
  ColumnStr3        = gLibEmptyString;

  //
  // Clean the space at gScreenDimensions.LeftColumn + 1.
  //
  PrintStringAtWithWidth (StartColumnOfHelp - 1, BottomRowOfHelp, gLibEmptyString, 1);
  PrintStringAtWithWidth (StartColumnOfHelp - 1, TopRowOfHelp, gLibEmptyString, 1);

  if (Statement == NULL) {
    //
    // Print Key for Form without displayable statement.
    //
    PrintHotKeyHelpString (FormData, TRUE);
    PrintStringAtWithWidth (StartColumnOfHelp, BottomRowOfHelp, gLibEmptyString, ColumnWidth1);
    PrintStringAtWithWidth (SecCol, BottomRowOfHelp, gLibEmptyString, ColumnWidth2);
    PrintStringAtWithWidth (StartColumnOfHelp, TopRowOfHelp, gLibEmptyString, ColumnWidth1);
    if (gClassOfVfr == FORMSET_CLASS_PLATFORM_SETUP) {
      ColumnStr3 = gEscapeString;
    }
    PrintStringAtWithWidth (ThdCol, BottomRowOfHelp, ColumnStr3, ColumnWidth3);

    return;
  }

  HexDisplay = FALSE;
  NumericOp = NULL;
  DateOp    = NULL;
  TimeOp    = NULL;
  if (Statement->OpCode->OpCode == EFI_IFR_NUMERIC_OP) {
    NumericOp = (EFI_IFR_NUMERIC *) Statement->OpCode;
    HexDisplay = (NumericOp->Flags & EFI_IFR_DISPLAY_UINT_HEX) == EFI_IFR_DISPLAY_UINT_HEX;
  } else if (Statement->OpCode->OpCode == EFI_IFR_DATE_OP) {
    DateOp   = (EFI_IFR_DATE *) Statement->OpCode;
    HexDisplay = (DateOp->Flags & EFI_IFR_DISPLAY_UINT_HEX) == EFI_IFR_DISPLAY_UINT_HEX;
  } else if (Statement->OpCode->OpCode == EFI_IFR_TIME_OP) {
    TimeOp  = (EFI_IFR_TIME *) Statement->OpCode;
    HexDisplay = (TimeOp->Flags & EFI_IFR_DISPLAY_UINT_HEX) == EFI_IFR_DISPLAY_UINT_HEX;
  }  
  switch (Statement->OpCode->OpCode) {
  case EFI_IFR_ORDERED_LIST_OP:
  case EFI_IFR_ONE_OF_OP:
  case EFI_IFR_NUMERIC_OP:
  case EFI_IFR_TIME_OP:
  case EFI_IFR_DATE_OP:
    if (!Selected) {
      PrintHotKeyHelpString (FormData, TRUE);

      if (gClassOfVfr == FORMSET_CLASS_PLATFORM_SETUP) {
        ColumnStr3 = gEscapeString;
      }
      PrintStringAtWithWidth (ThdCol, BottomRowOfHelp, ColumnStr3, ColumnWidth3);

      if ((Statement->OpCode->OpCode == EFI_IFR_DATE_OP) ||
          (Statement->OpCode->OpCode == EFI_IFR_TIME_OP)) {
        PrintAt (
          ColumnWidth1, 
          StartColumnOfHelp,
          BottomRowOfHelp,
          L"%c%c%c%c%s",
          ARROW_UP,
          ARROW_DOWN,
          ARROW_RIGHT,
          ARROW_LEFT,
          gMoveHighlight
          );
        PrintStringAtWithWidth (SecCol, BottomRowOfHelp, gEnterString, ColumnWidth2);
        PrintStringAtWithWidth (StartColumnOfHelp, TopRowOfHelp, gAdjustNumber, ColumnWidth1);
      } else {
        PrintAt (ColumnWidth1, StartColumnOfHelp, BottomRowOfHelp, L"%c%c%s", ARROW_UP, ARROW_DOWN, gMoveHighlight);
        if (Statement->OpCode->OpCode == EFI_IFR_NUMERIC_OP && NumericOp != NULL && LibGetFieldFromNum(Statement->OpCode) != 0) {
          ColumnStr1 = gAdjustNumber;
        }
        PrintStringAtWithWidth (StartColumnOfHelp, TopRowOfHelp, ColumnStr1, ColumnWidth1);
        PrintStringAtWithWidth (SecCol, BottomRowOfHelp, gEnterString, ColumnWidth2);
      }
    } else {
      PrintHotKeyHelpString (FormData, FALSE);
      PrintStringAtWithWidth (SecCol, BottomRowOfHelp, gEnterCommitString, ColumnWidth2);

      //
      // If it is a selected numeric with manual input, display different message
      //
      if ((Statement->OpCode->OpCode == EFI_IFR_NUMERIC_OP) || 
          (Statement->OpCode->OpCode == EFI_IFR_DATE_OP) ||
          (Statement->OpCode->OpCode == EFI_IFR_TIME_OP)) {
        ColumnStr2 = HexDisplay ? gHexNumericInput : gDecNumericInput;
        PrintStringAtWithWidth (StartColumnOfHelp, BottomRowOfHelp, gLibEmptyString, ColumnWidth1);
      } else {
        PrintAt (ColumnWidth1, StartColumnOfHelp, BottomRowOfHelp, L"%c%c%s", ARROW_UP, ARROW_DOWN, gMoveHighlight);
      }

      if (Statement->OpCode->OpCode == EFI_IFR_ORDERED_LIST_OP) {
        ColumnStr1 = gPlusString;
        ColumnStr3 = gMinusString;
      }
      PrintStringAtWithWidth (StartColumnOfHelp, TopRowOfHelp, ColumnStr1, ColumnWidth1);
      PrintStringAtWithWidth (ThdCol, TopRowOfHelp, ColumnStr3, ColumnWidth3);
      PrintStringAtWithWidth (SecCol, TopRowOfHelp, ColumnStr2, ColumnWidth2);

      PrintStringAtWithWidth (ThdCol, BottomRowOfHelp, gEnterEscapeString, ColumnWidth3);
    }
    break;

  case EFI_IFR_CHECKBOX_OP:
    PrintHotKeyHelpString (FormData, TRUE);

    if (gClassOfVfr == FORMSET_CLASS_PLATFORM_SETUP) {
      ColumnStr3 = gEscapeString;
    }
    PrintStringAtWithWidth (ThdCol, BottomRowOfHelp, ColumnStr3, ColumnWidth3);

    PrintAt (ColumnWidth1, StartColumnOfHelp, BottomRowOfHelp, L"%c%c%s", ARROW_UP, ARROW_DOWN, gMoveHighlight);
    PrintStringAtWithWidth (SecCol, BottomRowOfHelp, gToggleCheckBox, ColumnWidth2);
    PrintStringAtWithWidth (StartColumnOfHelp, TopRowOfHelp, gLibEmptyString, ColumnWidth1);
    break;

  case EFI_IFR_REF_OP:
  case EFI_IFR_PASSWORD_OP:
  case EFI_IFR_STRING_OP:
  case EFI_IFR_TEXT_OP:
  case EFI_IFR_ACTION_OP:
  case EFI_IFR_RESET_BUTTON_OP:
  case EFI_IFR_SUBTITLE_OP:
     if (!Selected) {
      PrintHotKeyHelpString (FormData, TRUE);

      if (gClassOfVfr == FORMSET_CLASS_PLATFORM_SETUP) {
        ColumnStr3 = gEscapeString;
      }
      PrintStringAtWithWidth (ThdCol, BottomRowOfHelp, ColumnStr3, ColumnWidth3);

      PrintAt (ColumnWidth1, StartColumnOfHelp, BottomRowOfHelp, L"%c%c%s", ARROW_UP, ARROW_DOWN, gMoveHighlight);
      if (Statement->OpCode->OpCode != EFI_IFR_TEXT_OP && Statement->OpCode->OpCode != EFI_IFR_SUBTITLE_OP) {
        ColumnStr2 = gEnterString;
      }
      PrintStringAtWithWidth (SecCol, BottomRowOfHelp, ColumnStr2, ColumnWidth2);
      PrintStringAtWithWidth (StartColumnOfHelp, TopRowOfHelp, ColumnStr1, ColumnWidth1);
    } else {
      PrintHotKeyHelpString (FormData, FALSE);
      if (Statement->OpCode->OpCode != EFI_IFR_REF_OP) {
        ColumnStr2 = gEnterCommitString;
        ColumnStr3 = gEnterEscapeString;
      }
      PrintStringAtWithWidth (StartColumnOfHelp, TopRowOfHelp, ColumnStr1, ColumnWidth1);
      PrintStringAtWithWidth (StartColumnOfHelp, BottomRowOfHelp, ColumnStr1, ColumnWidth1);
      PrintStringAtWithWidth (SecCol, BottomRowOfHelp, ColumnStr2, ColumnWidth2);
      PrintStringAtWithWidth (ThdCol, BottomRowOfHelp, ColumnStr3, ColumnWidth3);
    }
    break;

  default:
    break;
  }  
}

/**
  Update status bar.

  This function updates the status bar on the bottom of menu screen. It just shows StatusBar. 
  Original logic in this function should be splitted out.

  @param[in]  MessageType            The type of message to be shown. InputError or Configuration Changed. 
  @param[in]  State                  Show or Clear Message.
**/
VOID
EFIAPI
UpdateStatusBar (
  IN  UINTN                  MessageType,
  IN  BOOLEAN                State
  )
{
  UINTN           Index;
  CHAR16          OptionWidth;

  OptionWidth = (CHAR16) ((gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn) / 3);

  switch (MessageType) {
  case INPUT_ERROR:
    if (gClassOfVfr != FORMSET_CLASS_FRONT_PAGE) {
      break;
    }
    if (State) {
      gST->ConOut->SetAttribute (gST->ConOut, ERROR_TEXT);
      PrintStringAt (
        gScreenDimensions.LeftColumn + OptionWidth,
        gScreenDimensions.BottomRow - 1,
        gInputErrorMessage
        );
    } else {
      gST->ConOut->SetAttribute (gST->ConOut, KEYHELP_BACKGROUND);
      for (Index = 0; Index < (LibGetStringWidth (gInputErrorMessage) - 2) / 2; Index++) {
        PrintStringAt (gScreenDimensions.LeftColumn + OptionWidth + Index, gScreenDimensions.BottomRow - 1, L"  ");
      }
    }
    break;

  case NV_UPDATE_REQUIRED:
    //
    // Global setting support. Show configuration change on every form.
    //
    if (gClassOfVfr != FORMSET_CLASS_FRONT_PAGE) {
      break;
    }
    if (State) {
      gST->ConOut->SetAttribute (gST->ConOut, INFO_TEXT);
      PrintStringAt (
        gScreenDimensions.LeftColumn + OptionWidth * 2,
        gScreenDimensions.BottomRow - 1,
        gNvUpdateMessage
        );
    } else {
      gST->ConOut->SetAttribute (gST->ConOut, KEYHELP_BACKGROUND);
      for (Index = 0; Index < (LibGetStringWidth (gNvUpdateMessage) - 2) / 2; Index++) {
        PrintStringAt (
          (gScreenDimensions.LeftColumn + OptionWidth * 2 + Index),
          gScreenDimensions.BottomRow - 1,
          L"  "
          );
      }
    }
    break;

  default:
    break;
  } 
}

/**
  Create popup window. It will replace CreateDialog(). 

  This function draws OEM/Vendor specific pop up windows.

  @param[out]  Key    User Input Key
  @param       ...    String to be shown in Popup. The variable argument list is terminated by a NULL.
  
**/
VOID
EFIAPI
CreateDialog (
  OUT EFI_INPUT_KEY  *Key,        OPTIONAL
  ...
  )
{
  VA_LIST       Marker;
  EFI_INPUT_KEY KeyValue;
  UINTN         LargestString;
  UINTN         LineNum;
  UINTN   Index;
  UINTN   Count;
  CHAR16  Character;
  UINTN   Start;
  UINTN   End;
  UINTN   Top;
  UINTN   Bottom;
  CHAR16  *String;
  UINTN   DimensionsWidth;
  UINTN   DimensionsHeight;
  UINTN   CurrentAttribute;
  BOOLEAN CursorVisible;

  USER_INPUT_TYPE    InputType;
  MOUSE_ACTION_INFO    Mouse;

  //
  // If screen dimension info is not ready, get it from console.
  //
  if (gScreenDimensions.RightColumn == 0 || gScreenDimensions.BottomRow == 0) {
    ZeroMem (&gScreenDimensions, sizeof (EFI_SCREEN_DESCRIPTOR));
    gST->ConOut->QueryMode (
                   gST->ConOut,
                   gST->ConOut->Mode->Mode,
                   &gScreenDimensions.RightColumn,
                   &gScreenDimensions.BottomRow
                   );
  }

  DimensionsWidth   = gScreenDimensions.RightColumn - gScreenDimensions.LeftColumn;
  DimensionsHeight  = gScreenDimensions.BottomRow - gScreenDimensions.TopRow;

  LargestString = 0;
  LineNum       = 0;
  VA_START (Marker, Key);
  while  ((String = VA_ARG (Marker, CHAR16 *)) != NULL) {
    LineNum ++;
    
    if ((LibGetStringWidth (String) / 2) > LargestString) {
      LargestString = (LibGetStringWidth (String) / 2);
    }
  } 
  VA_END (Marker);

  if ((LargestString + 2) > DimensionsWidth) {
    LargestString = DimensionsWidth - 2;
  }
  
  CurrentAttribute  = gST->ConOut->Mode->Attribute;
  CursorVisible     = gST->ConOut->Mode->CursorVisible;
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  gST->ConOut->SetAttribute (gST->ConOut, GetPopupColor ());

  //
  // Subtract the PopUp width from total Columns, allow for one space extra on
  // each end plus a border.
  //
  Start     = (DimensionsWidth - LargestString - 2) / 2 + gScreenDimensions.LeftColumn + 1;
  End       = Start + LargestString + 1;

  Top       = ((DimensionsHeight - LineNum - 2) / 2) + gScreenDimensions.TopRow - 1;
  Bottom    = Top + LineNum + 2;

  Character = BOXDRAW_DOWN_RIGHT;
  PrintCharAt (Start, Top, Character);
  Character = BOXDRAW_HORIZONTAL;
  for (Index = Start; Index + 2 < End; Index++) {
    PrintCharAt ((UINTN)-1, (UINTN)-1, Character);
  }

  Character = BOXDRAW_DOWN_LEFT;
  PrintCharAt ((UINTN)-1, (UINTN)-1, Character);
  Character = BOXDRAW_VERTICAL;

  Count = 0;
  VA_START (Marker, Key);
  for (Index = Top; Index + 2 < Bottom; Index++, Count++) {
    String = VA_ARG (Marker, CHAR16*);

    if (String[0] == CHAR_NULL) {
      //
      // Passing in a NULL results in a blank space
      //
      ClearLines (Start, End, Index + 1, Index + 1, GetPopupColor ());
    } else if (String[0] == L' ') {
      //
      // Passing in a space results in the assumption that this is where typing will occur
      //
      ClearLines (Start + 1, End - 1, Index + 1, Index + 1, POPUP_INVERSE_TEXT | POPUP_INVERSE_BACKGROUND);
      PrintStringAt (
        ((DimensionsWidth - LibGetStringWidth (String) / 2) / 2) + gScreenDimensions.LeftColumn + 1,
        Index + 1,
        String + 1
        );
    } else {
      //
      // This will clear the background of the line - we never know who might have been
      // here before us.  This differs from the next clear in that it used the non-reverse
      // video for normal printing.
      //
      ClearLines (Start, End, Index + 1, Index + 1, GetPopupColor ());
      PrintStringAt (
        ((DimensionsWidth - LibGetStringWidth (String) / 2) / 2) + gScreenDimensions.LeftColumn + 1,
        Index + 1,
        String
        );
    }

    gST->ConOut->SetAttribute (gST->ConOut, GetPopupColor ());
    PrintCharAt (Start, Index + 1, Character);
    PrintCharAt (End - 1, Index + 1, Character);
  }
  VA_END (Marker);

  Character = BOXDRAW_UP_RIGHT;
  PrintCharAt (Start, Bottom - 1, Character);
  Character = BOXDRAW_HORIZONTAL;
  for (Index = Start; Index + 2 < End; Index++) {
    PrintCharAt ((UINTN)-1, (UINTN) -1, Character);
  }

  Character = BOXDRAW_UP_LEFT;
  PrintCharAt ((UINTN)-1, (UINTN) -1, Character);

  if (Key != NULL) {
    InputType = WaitForUserInput(&KeyValue, &Mouse);
    if (USER_INPUT_MOUSE == InputType) {
      if (MOUSE_LEFT_CLICK == Mouse.Action || MOUSE_LEFT_DOUBLE_CLICK == Mouse.Action) {
        KeyValue.UnicodeChar = L'Y';
        KeyValue.ScanCode = SCAN_NULL;
      }
      if (MOUSE_RIGHT_CLICK == Mouse.Action) {
        KeyValue.UnicodeChar = L'N';
        KeyValue.ScanCode = SCAN_ESC;
      }   
    }


    CopyMem (Key, &KeyValue, sizeof (EFI_INPUT_KEY));
  }

  gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);
  gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
}

/**
  Confirm how to handle the changed data. 
  
  @return Action BROWSER_ACTION_SUBMIT, BROWSER_ACTION_DISCARD or other values.
**/
UINTN
EFIAPI
ConfirmDataChange (
  CHAR16    *Title
  )
{
  CHAR16    *ChangeTitle;
  SELECTION_TYPE ReturnFlag;
  
  if (NULL == Title) {
    ChangeTitle = gSaveChanges;
  } else {
    ChangeTitle = Title;
  }
  //
  // If NV flag is up, prompt user
  //
  ReturnFlag = UiConfirmDialog(DIALOG_YESNO, gStrUiSetupConfirmation, NULL, TEXT_ALIGIN_CENTER, gLibEmptyString, ChangeTitle, NULL);  
  if (ReturnFlag == SELECTION_YES) {
    return BROWSER_ACTION_SUBMIT;
  } else if (ReturnFlag == SELECTION_NO) {
    return BROWSER_ACTION_DISCARD;
  } else {
    return BROWSER_ACTION_NONE;
  }
}

/**
  OEM specifies whether Setup exits Page by ESC key.

  This function customized the behavior that whether Setup exits Page so that 
  system able to boot when configuration is not changed.

  @retval  TRUE     Exits FrontPage
  @retval  FALSE    Don't exit FrontPage.
**/
BOOLEAN
EFIAPI
FormExitPolicy (
  VOID
  )
{
  return gClassOfVfr == FORMSET_CLASS_FRONT_PAGE ? FALSE : TRUE;
}

/**
  Set Timeout value for a certain Form to get user response. 
  
  This function allows to set timeout value on a certain form if necessary.
  If timeout is not zero, the form will exit if user has no response in timeout. 
  
  @param[in]  FormData   Form Data to be shown in Page

  @return 0     No timeout for this form. 
  @return > 0   Timeout value in 100 ns units.
**/
UINT64
EFIAPI
FormExitTimeout (
  IN FORM_DISPLAY_ENGINE_FORM      *FormData
  )
{
  return 0;
}
//
// Print Functions
//
/**
  Prints a unicode string to the default console, at
  the supplied cursor position, using L"%s" format.

  @param  Column     The cursor position to print the string at. When it is -1, use current Position.
  @param  Row        The cursor position to print the string at. When it is -1, use current Position.
  @param  String     String pointer.

  @return Length of string printed to the console

**/
UINTN
EFIAPI
PrintStringAt (
  IN UINTN     Column,
  IN UINTN     Row,
  IN CHAR16    *String
  )
{
  return PrintAt (0, Column, Row, L"%s", String);
}

/**
  Prints a unicode string to the default console, at
  the supplied cursor position, using L"%s" format.

  @param  Column     The cursor position to print the string at. When it is -1, use current Position.
  @param  Row        The cursor position to print the string at. When it is -1, use current Position.
  @param  String     String pointer.
  @param  Width      Width for String.

  @return Length of string printed to the console

**/
UINTN
EFIAPI
PrintStringAtWithWidth (
  IN UINTN     Column,
  IN UINTN     Row,
  IN CHAR16    *String,
  IN UINTN     Width
  )
{
  return PrintAt (Width, Column, Row, L"%s", String);
}

/**
  Prints a character to the default console, at
  the supplied cursor position, using L"%c" format.

  @param  Column     The cursor position to print the string at. When it is -1, use current Position.
  @param  Row        The cursor position to print the string at. When it is -1, use current Position.
  @param  Character  Character to print.

  @return Length of string printed to the console.

**/
UINTN
EFIAPI
PrintCharAt (
  IN UINTN     Column,
  IN UINTN     Row,
  CHAR16       Character
  )
{
  return PrintAt (0, Column, Row, L"%c", Character);
}

/**
  Clear rectangle with specified text attribute.

  @param  LeftColumn     Left column of rectangle.
  @param  RightColumn    Right column of rectangle.
  @param  TopRow         Start row of rectangle.
  @param  BottomRow      End row of rectangle.
  @param  TextAttribute  The character foreground and background.

**/
VOID
EFIAPI
ClearLines (
  IN UINTN               LeftColumn,
  IN UINTN               RightColumn,
  IN UINTN               TopRow,
  IN UINTN               BottomRow,
  IN UINTN               TextAttribute
  )
{
  CHAR16  *Buffer;
  UINTN   Row;
  //
  // For now, allocate an arbitrarily long buffer
  //
  Buffer = AllocateZeroPool (0x10000);
  ASSERT (Buffer != NULL);

  //
  // Set foreground and background as defined
  //
  gST->ConOut->SetAttribute (gST->ConOut, TextAttribute | BYO_NOT_AUTO_FEED_ATTRIBUTE);

  //
  // Much faster to buffer the long string instead of print it a character at a time
  //
  LibSetUnicodeMem (Buffer, RightColumn - LeftColumn, L' ');

  //
  // Clear the desired area with the appropriate foreground/background
  //
  for (Row = TopRow; Row <= BottomRow; Row++) {
    PrintStringAt (LeftColumn, Row, Buffer);
  }

  gST->ConOut->SetAttribute (gST->ConOut, TextAttribute);
  gST->ConOut->SetCursorPosition (gST->ConOut, LeftColumn, TopRow);
  FreePool (Buffer);
}

//
// Color Setting Functions
//

/**
  Get OEM/Vendor specific popup attribute colors.

  @retval  Byte code color setting for popup color.
**/
UINT8
EFIAPI
GetPopupColor (
  VOID
  )
{
  return POPUP_TEXT | POPUP_BACKGROUND;
}

/**
  Get OEM/Vendor specific popup attribute colors.

  @retval  Byte code color setting for popup inverse color.
**/
UINT8
EFIAPI
GetPopupInverseColor (
  VOID
  )
{
  return POPUP_INVERSE_TEXT | POPUP_INVERSE_BACKGROUND;
}

/**
  Get OEM/Vendor specific PickList color attribute.

  @retval  Byte code color setting for pick list color.
**/
UINT8
EFIAPI
GetPickListColor (
  VOID
  )
{
  return PICKLIST_HIGHLIGHT_TEXT | PICKLIST_HIGHLIGHT_BACKGROUND;
}

/**
  Get OEM/Vendor specific arrow color attribute.

  @retval  Byte code color setting for arrow color.
**/
UINT8
EFIAPI
GetArrowColor (
  VOID
  )
{
  return ARROW_TEXT | ARROW_BACKGROUND;
}

/**
  Get OEM/Vendor specific info text color attribute.

  @retval  Byte code color setting for info text color.
**/
UINT8
EFIAPI
GetInfoTextColor (
  VOID
  )
{
  return INFO_TEXT | FIELD_BACKGROUND;
}

/**
  Get OEM/Vendor specific help text color attribute.

  @retval  Byte code color setting for help text color.
**/
UINT8
EFIAPI
GetHelpTextColor (
  VOID
  )
{
  return HELP_TEXT | FIELD_BACKGROUND;
}

/**
  Get OEM/Vendor specific grayed out text color attribute.

  @retval  Byte code color setting for grayed out text color.
**/
UINT8
EFIAPI
GetGrayedTextColor (
  VOID
  )
{
  return FIELD_TEXT_GRAYED | FIELD_BACKGROUND;
}

/**
  Get OEM/Vendor specific highlighted text color attribute.

  @retval  Byte code color setting for highlight text color.
**/
UINT8
EFIAPI
GetHighlightTextColor (
  VOID
  )
{
  return PcdGet8 (PcdBrowserFieldTextHighlightColor) | PcdGet8 (PcdBrowserFieldBackgroundHighlightColor);
}

/**
  Get OEM/Vendor specific field text color attribute.

  @retval  Byte code color setting for field text color.
**/
UINT8
EFIAPI
GetFieldTextColor (
  VOID
  )
{
  return PcdGet8 (PcdBrowserFieldTextColor) | FIELD_BACKGROUND;
}

/**
  Get OEM/Vendor specific subtitle text color attribute.

  @retval  Byte code color setting for subtitle text color.
**/
UINT8
EFIAPI
GetSubTitleTextColor (
  VOID
  )
{
  return PcdGet8 (PcdBrowserSubtitleTextColor) | FIELD_BACKGROUND;
}

/**
  Clear Screen to the initial state.
**/
VOID
EFIAPI 
ClearDisplayPage (
  VOID
  )
{
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
  gLibIsFirstForm = TRUE;
}

/**
  Constructor of Customized Display Library Instance.

  @param  ImageHandle   The firmware allocated handle for the EFI image.
  @param  SystemTable   A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The constructor always returns EFI_SUCCESS.

**/
EFI_STATUS
EFIAPI
CustomizedDisplayLibConstructor (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{
  InitializeLanguage(TRUE);

  mCDLStringPackHandle = HiiAddPackages (&gCustomizedDisplayLibGuid, ImageHandle, CustomizedDisplayLibStrings, NULL);
  ASSERT (mCDLStringPackHandle != NULL);

  InitializeLibStrings();

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
CustomizedDisplayLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{  
  HiiRemovePackages(mCDLStringPackHandle);
  
  FreeLibStrings ();

  return EFI_SUCCESS;
}

/**
  Create F1 help popup form.

**/
EFI_STATUS
CreateHelpDialog (
  OUT EFI_INPUT_KEY        *KeyValue
  )
{
  EFI_INPUT_KEY Key;
  EFI_STATUS    Status;
  BOOLEAN       SelectionComplete;
  UINTN         CurrentAttribute;
  CHAR16      Character;
  UINTN         Index;
  CHAR16      *Buffer;
  UINTN    StartColum;
  UINTN    StartColum1;
  UINTN    StartColum2;
  UINTN    StartColum3;
  UINTN    StartColum4;
  UINTN    StartRow;
  UINTN    StartRow1;
  UINTN    StartRow2;
  UINTN    StartRow3;
  UINTN    StartRow4;
  UINTN    StartRow5;
  UINTN    StartRow6;
  EFI_SCREEN_DESCRIPTOR  Screen;
  UINTN    HelpWidth;
  
  USER_INPUT_TYPE    InputType;
  MOUSE_ACTION_INFO    Mouse;
  MOUSE_CURSOR_RANGE    ContinueRange;
  SCREENSHOT_PROTOCOL   *ChooseFolderPath;
	
  ZeroMem (&Screen, sizeof (EFI_SCREEN_DESCRIPTOR));
  CopyMem (&Screen, &gScreenDimensions, sizeof (EFI_SCREEN_DESCRIPTOR));

  SelectionComplete = FALSE;

  Buffer = AllocateZeroPool (0x100);
  ASSERT (Buffer != NULL);

  CurrentAttribute  = gST->ConOut->Mode->Attribute;

  Character = BOXDRAW_HORIZONTAL;
  for(Index = 0; Index + 2 < 46; Index ++){
    Buffer[Index] = Character;
  }

  StartColum = (Screen.RightColumn - Screen.LeftColumn - 46) / 2;
  StartRow = (Screen.BottomRow - Screen.TopRow - 10) /2;
  HelpWidth = 42;

  //
  // width 46, height 10
  //
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  ClearLines (StartColum, StartColum + 46, StartRow, StartRow + 10, EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE);

  //
  //Line 1.
  //
  gST->ConOut->SetAttribute (gST->ConOut, EFI_GREEN| EFI_BACKGROUND_BLUE);
  Character = BOXDRAW_DOWN_RIGHT;
  PrintCharAt (StartColum, StartRow, Character);
  PrintStringAt ((UINTN) -1, (UINTN) -1, Buffer);
  Character = BOXDRAW_DOWN_LEFT;
  PrintCharAt ((UINTN) -1, (UINTN) -1, Character);

  //
  //Line 2.
  //
  Character = BOXDRAW_VERTICAL;
  PrintCharAt (StartColum, StartRow + 1, Character);
  PrintCharAt (StartColum + 45, StartRow +1, Character);
  
  gST->ConOut->SetAttribute (gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE);
  PrintStringAt(StartColum + (45 - StrLen (gByoGeneralHelp)) / 2, StartRow + 1, gByoGeneralHelp);

  //
  //Line 3.
  //
  gST->ConOut->SetAttribute (gST->ConOut, EFI_GREEN| EFI_BACKGROUND_BLUE);
  Character = BOXDRAW_VERTICAL_RIGHT;
  PrintCharAt (StartColum, StartRow + 2, Character);
  PrintStringAt ((UINTN) -1, (UINTN) -1, Buffer);
  Character = BOXDRAW_VERTICAL_LEFT;
  PrintCharAt (StartColum + 45, StartRow + 2 , Character);


  //
  // Middle line.
  //
  Character = BOXDRAW_VERTICAL;
  for (Index = StartRow +2; Index + 1 < StartRow + 10; Index++){
    PrintCharAt (StartColum, Index + 1, Character);
    PrintCharAt (StartColum + 45, Index + 1, Character);
  }

  //
  // Last line
  //
  Character = BOXDRAW_UP_RIGHT;
  PrintCharAt (StartColum, StartRow + 10, Character);
  PrintStringAt ((UINTN) -1, (UINTN) -1, Buffer);;
  Character = BOXDRAW_UP_LEFT;
  PrintCharAt ((UINTN) -1, (UINTN) -1, Character);

  //
  // Help Info.
  //
  StartRow1 = StartRow + 4;
  StartRow2 = StartRow + 5;
  StartRow3 = StartRow + 6;
  StartRow4 = StartRow + 7;
  StartRow6 = StartRow + 8;
  StartRow5 = StartRow + 9;
  StartColum1 = StartColum + 1;
  StartColum2 = StartColum + 1 + 5;
  StartColum3 = StartColum + (45 + 1)/2;
  StartColum4 = StartColum + (45 + 1)/2 + 6;
  gST->ConOut->SetAttribute (gST->ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE);
  PrintStringAt(StartColum1, StartRow1, gByoFunctionKeyOne);
  PrintStringAt(StartColum2, StartRow1, gByoFunctionKeyOneHelp);
  PrintStringAt(StartColum3, StartRow1, gByoKeyPlusAndMinus);
  PrintStringAt(StartColum4, StartRow1, gByoKeyPlusAndMinusHelp);

  if (PcdGetBool (PcdEnableSetupHotkeyF3F4)) {
    PrintStringAt(StartColum1, StartRow2, gByoFunctionKeyThree);
	PrintStringAt(StartColum2, StartRow2, gByoFunctionKeyNineHelp);
    PrintStringAt(StartColum3, StartRow2, gByoFunctionKeyEnter);
	PrintStringAt(StartColum4, StartRow2, gByoFunctionKeyEnterHelp);
  } else {
    PrintStringAt(StartColum1, StartRow2, gByoFunctionKeyNine);
	PrintStringAt(StartColum2, StartRow2, gByoFunctionKeyNineHelp);
    PrintStringAt(StartColum3, StartRow2, gByoFunctionKeyEnter);
	PrintStringAt(StartColum4, StartRow2, gByoFunctionKeyEnterHelp);
  }

  PrintAt(HelpWidth / 2, StartColum1, StartRow3, L"%c%c",ARROW_UP,ARROW_DOWN);
  PrintAt(HelpWidth / 2, StartColum2, StartRow3, gKeyUpDownHelp);
  PrintAt(HelpWidth / 2, StartColum3, StartRow3, L"%c%c",ARROW_LEFT,ARROW_RIGHT);
  PrintAt(0, StartColum4, StartRow3, gByoKeyLeftRightHelp);


  if (PcdGetBool (PcdEnableSetupHotkeyF3F4)) {
    PrintStringAt(StartColum1, StartRow4, gByoFunctionKeyFour);
	PrintStringAt(StartColum2, StartRow4, gByoFunctionKeyEnterTenHelp);
	PrintStringAt(StartColum3, StartRow4, gByoFunctionKeyEsc);
	PrintStringAt(StartColum4, StartRow4, gByoFunctionKeyEscHelp);
  } else {
    PrintStringAt(StartColum1, StartRow4, gByoFunctionKeyEnterTen);
	PrintStringAt(StartColum2, StartRow4, gByoFunctionKeyEnterTenHelp);
	PrintStringAt(StartColum3, StartRow4, gByoFunctionKeyEsc);
	PrintStringAt(StartColum4, StartRow4, gByoFunctionKeyEscHelp);
  }

  Status = gBS->LocateProtocol(&gScreenshotProtocolGuid, NULL, (VOID **)&ChooseFolderPath); 
  if (!EFI_ERROR (Status)) {
    PrintStringAt(StartColum1, StartRow6, gByoKeyCtrlAdds);
    PrintStringAt(StartColum2 + 2, StartRow6, gByoKeyCtrlAddsHelp);
  }
  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE | EFI_BACKGROUND_CYAN);
  PrintStringAt((StartColum + (45 - StrLen (gByoContinue)) / 2), StartRow5, gByoContinue);
  gST->ConOut->SetCursorPosition (gST->ConOut, StartColum +17, StartRow5);
  gBS->FreePool (Buffer);

  SetRange (&ContinueRange, StartColum + (45 - StrLen (gByoContinue)) / 2, StrLen (gByoContinue), StartRow5, 0);
  //
  // Take the first key typed and report it back?
  //
  if (KeyValue != NULL) {
    Status = WaitForKeyStroke (&Key);
    ASSERT_EFI_ERROR (Status);	
    CopyMem (KeyValue, &Key, sizeof (EFI_INPUT_KEY));
  } else {

    do {
      InputType = WaitForUserInput(&Key, &Mouse);
      if (USER_INPUT_MOUSE == InputType) {
        if (IsMouseInRange (&ContinueRange, &Mouse)) {
          if (MOUSE_LEFT_CLICK == Mouse.Action  || MOUSE_LEFT_DOUBLE_CLICK == Mouse.Action) {
            Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
            Key.ScanCode = SCAN_NULL;
          }
        }
        if (MOUSE_RIGHT_CLICK == Mouse.Action) {
          Key.UnicodeChar = CHAR_NULL;
          Key.ScanCode = SCAN_ESC;
        }
      }
	
      switch (Key.UnicodeChar) {
      case CHAR_NULL:
        switch (Key.ScanCode) {
        case SCAN_ESC:
          SelectionComplete = TRUE;
          gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);
          gST->ConOut->EnableCursor (gST->ConOut, FALSE);
          return EFI_SUCCESS;
        default:
          break;
        }
        break;

      case CHAR_CARRIAGE_RETURN:
        SelectionComplete = TRUE;
        gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);
        gST->ConOut->EnableCursor (gST->ConOut, FALSE);
        return EFI_SUCCESS;
        break;

      default:
        break;
      }      
    } while (!SelectionComplete);
  }

  gST->ConOut->SetAttribute (gST->ConOut, CurrentAttribute);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  return EFI_SUCCESS;
}


/**
  Display error message for invalid password.

**/
VOID
PasswordInvalid (
  VOID
  )
{
  //
  // Invalid password, prompt error message
  //
  UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, L"", gPasswordInvalid, NULL);
}


/**
  Draw a pop up windows based on the dimension, number of lines and
  strings specified.

  @param RequestedWidth  The width of the pop-up.
  @param NumberOfLines   The number of lines.
  @param Marker          The variable argument list for the list of string to be printed.

**/
VOID
CreateSharedPopUp (
  IN  UINTN                       RequestedWidth,
  IN  UINTN                       NumberOfLines,
  IN  VA_LIST                     Marker
  )
{
  UINTN   Index;
  UINTN   Count;
  CHAR16  Character;
  UINTN   Start;
  UINTN   End;
  UINTN   Top;
  UINTN   Bottom;
  CHAR16  *String;
  UINTN   DimensionsWidth;
  UINTN   DimensionsHeight;

  String            = NULL;
  DimensionsWidth   = gStatementDimensions.RightColumn - gStatementDimensions.LeftColumn;
  DimensionsHeight  = gStatementDimensions.BottomRow - gStatementDimensions.TopRow;

  gST->ConOut->SetAttribute (gST->ConOut, GetPopupColor ());

  if ((RequestedWidth + 2) > DimensionsWidth) {
    RequestedWidth = DimensionsWidth - 2;
  }

  //
  // Subtract the PopUp width from total Columns, allow for one space extra on
  // each end plus a border.
  //
  Start     = (DimensionsWidth - RequestedWidth - 2) / 2 + gStatementDimensions.LeftColumn + 1;
  End       = Start + RequestedWidth + 1;

  Top       = ((DimensionsHeight - NumberOfLines - 2) / 2) + gStatementDimensions.TopRow - 1;
  Bottom    = Top + NumberOfLines + 2;

  Character = BOXDRAW_DOWN_RIGHT;
  PrintCharAt (Start, Top, Character);
  Character = BOXDRAW_HORIZONTAL;
  for (Index = Start; Index + 2 < End; Index++) {
    PrintCharAt ((UINTN)-1, (UINTN)-1, Character);
  }

  Character = BOXDRAW_DOWN_LEFT;
  PrintCharAt ((UINTN)-1, (UINTN)-1, Character);
  Character = BOXDRAW_VERTICAL;

  Count = 0;
  for (Index = Top; Index + 2 < Bottom; Index++, Count++) {
    String = VA_ARG (Marker, CHAR16*);

    //
    // This will clear the background of the line - we never know who might have been
    // here before us.  This differs from the next clear in that it used the non-reverse
    // video for normal printing.
    //
    if (LibGetStringWidth (String) / 2 > 1) {
      ClearLines (Start, End, Index + 1, Index + 1, GetPopupColor ());
    }

    //
    // Passing in a space results in the assumption that this is where typing will occur
    //
    if (String[0] == L' ') {
      ClearLines (Start + 1, End - 1, Index + 1, Index + 1, GetPopupInverseColor ());
    }

    //
    // Passing in a NULL results in a blank space
    //
    if (String[0] == CHAR_NULL) {
      ClearLines (Start, End, Index + 1, Index + 1, GetPopupColor ());
    }

    PrintStringAt (
      ((DimensionsWidth - LibGetStringWidth (String) / 2) / 2) + gStatementDimensions.LeftColumn + 1,
      Index + 1,
      String
      );
    gST->ConOut->SetAttribute (gST->ConOut, GetPopupColor ());
    PrintCharAt (Start, Index + 1, Character);
    PrintCharAt (End - 1, Index + 1, Character);
  }

  Character = BOXDRAW_UP_RIGHT;
  PrintCharAt (Start, Bottom - 1, Character);
  Character = BOXDRAW_HORIZONTAL;
  for (Index = Start; Index + 2 < End; Index++) {
    PrintCharAt ((UINTN)-1, (UINTN)-1, Character);
  }

  Character = BOXDRAW_UP_LEFT;
  PrintCharAt ((UINTN)-1, (UINTN)-1, Character);
}

/**
  Draw a pop up windows based on the dimension, number of lines and
  strings specified.

  @param RequestedWidth  The width of the pop-up.
  @param NumberOfLines   The number of lines.
  @param ...             A series of text strings that displayed in the pop-up.

**/
VOID
EFIAPI
CreateMultiStringPopUp (
  IN  UINTN                       RequestedWidth,
  IN  UINTN                       NumberOfLines,
  ...
  )
{
  VA_LIST Marker;

  VA_START (Marker, NumberOfLines);

  CreateSharedPopUp (RequestedWidth, NumberOfLines, Marker);

  VA_END (Marker);
}

/**
  Get maximum and minimum info from this opcode.

  @param  OpCode            Pointer to the current input opcode.
  @param  Minimum           The minimum size info for this opcode.
  @param  Maximum           The maximum size info for this opcode.

**/
VOID
GetFieldFromOp (
  IN   EFI_IFR_OP_HEADER       *OpCode,
  OUT  UINTN                   *Minimum,
  OUT  UINTN                   *Maximum
  )
{
  EFI_IFR_STRING    *StringOp;
  EFI_IFR_PASSWORD  *PasswordOp;
  if (OpCode->OpCode == EFI_IFR_STRING_OP) {
    StringOp = (EFI_IFR_STRING *) OpCode;
    *Minimum = StringOp->MinSize;
    *Maximum = StringOp->MaxSize;    
  } else if (OpCode->OpCode == EFI_IFR_PASSWORD_OP) {
    PasswordOp = (EFI_IFR_PASSWORD *) OpCode;
    *Minimum = PasswordOp->MinSize;
    *Maximum = PasswordOp->MaxSize;       
  } else {
    *Minimum = 0;
    *Maximum = 0;       
  }
}


/**
  Draw Popup Password Or String

  @param  X             String start coord x
  @param  Y             String start coord y
  @param  Width         Draw total width
  @param  Str           The displayed string
                        If the Str is empty, display *
  @param  StrLen        Displayed string len
  @param  StrColour     String colour
  @param  Cursor        TRUE   Display cursor
                        FALSE  No display cursor
  @param  CursorX       Cursor start coord x
  @param  CursorY       Cursor start coord y
  @param  CursorColour  Cursor colour

**/
static
EFI_STATUS
RenderString (
  IN PASSWORD_DEAL_TYPE  PasswordDealType,
  IN UINTN               X,
  IN UINTN               Y,
  IN UINTN               Width,
  IN CHAR16              *Str,
  IN UINTN               StrLen,
  IN UINTN               StrColour,
  IN BOOLEAN             Cursor,
  IN UINTN               CursorX,
  IN UINTN               CursorY,
  IN UINTN               CursorColour
  )
{
  CHAR16 *DisplayStr;
  UINTN  Index;

  if (Width == 0) {
    DEBUG((DEBUG_ERROR, "%a(%d) Parameter Error\n", __FILE__, __LINE__));
    return EFI_INVALID_PARAMETER;
  }

  if (StrLen > Width) {
    DEBUG((DEBUG_ERROR, "%a(%d) Parameter Error\n", __FILE__, __LINE__));
    return EFI_INVALID_PARAMETER;
  }

  DisplayStr = AllocatePool(sizeof(CHAR16) * (Width + 1));
  for (Index = 0; Index < StrLen; Index++) {
    DisplayStr[Index] = Str == NULL ? L'*' : Str[Index];
  }
  for (; Index < Width; Index++) {
    DisplayStr[Index] = L' ';
  }
  DisplayStr[Width] = 0;

  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    switch (PasswordDealType)
    {
    case FirstPrint:
      gST->ConOut->SetCursorPosition(gST->ConOut, X, Y);
      gST->ConOut->SetAttribute(gST->ConOut, StrColour);
      gST->ConOut->OutputString(gST->ConOut, DisplayStr);
      break;
    case InputPassword:
      gST->ConOut->SetAttribute(gST->ConOut, StrColour);
      if (Cursor < X+StrLen && Str != NULL) {
        gST->ConOut->SetCursorPosition(gST->ConOut, X, Y);
        gST->ConOut->OutputString(gST->ConOut, DisplayStr);
      } else {
        gST->ConOut->SetCursorPosition(gST->ConOut, X+StrLen-1, Y);
        gST->ConOut->OutputString(gST->ConOut, &DisplayStr[StrLen-1]);
      }
      break;
    case DeletePassword:
      gST->ConOut->SetAttribute(gST->ConOut, StrColour);
      if (Cursor < X+StrLen && Str != NULL) {
        gST->ConOut->SetCursorPosition(gST->ConOut, X, Y);
        gST->ConOut->OutputString(gST->ConOut, DisplayStr);
      } else {
        gST->ConOut->SetCursorPosition(gST->ConOut, X+StrLen, Y);
        gST->ConOut->OutputString(gST->ConOut, &DisplayStr[StrLen]);
      }
      break;
    default:
      break;
    }
 if (Cursor) {
    gST->ConOut->SetCursorPosition(gST->ConOut, CursorX, CursorY);
    gST->ConOut->SetAttribute(gST->ConOut, CursorColour);
    gST->ConOut->EnableCursor(gST->ConOut, TRUE);
  }

  FreePool(DisplayStr);
  return EFI_SUCCESS;
}

/**
  Get string or password input from user.

  @param  MenuOption        Pointer to the current input menu.
  @param  Prompt            The prompt string shown on popup window.
  @param  StringPtr         Old user input and destination for use input string.
  @param  CouldEmpty        Whether to allow the string to be empty.

  @retval EFI_SUCCESS       If string input is read successfully
  @retval EFI_DEVICE_ERROR  If operation fails

**/
EFI_STATUS
ReadString (
  IN     UI_MENU_OPTION              *MenuOption,
  IN     CHAR16                      *Prompt,
  IN OUT CHAR16                      *StringPtr,
  IN     BOOLEAN                     CouldEmpty
  )
{
  EFI_KEY_DATA            Key;
  CHAR16                  NullCharacter;
  UINTN                   ScreenSize;
  CHAR16                  Space[2];
  CHAR16                  KeyPad[2];
  CHAR16                  *TempString;
  CHAR16                  *BufferedString;
  UINTN                   Index;
  UINTN                   Index2;
  UINTN                   Count;
  UINTN                   Start;
  UINTN                   Top;
  UINTN                   DimensionsWidth;
  UINTN                   DimensionsHeight;
  UINTN                   CurrentCursor;
  BOOLEAN                 CursorVisible;
  UINTN                   Minimum;
  UINTN                   Maximum;
  FORM_DISPLAY_ENGINE_STATEMENT  *Question;
  BOOLEAN                 IsPassword;
  UINTN                   MaxLen;
  USER_INPUT_TYPE         InputType;
  MOUSE_ACTION_INFO       Mouse;
  UINTN                   StringWidth;
  EFI_STATUS              Status;
  BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL  *DisplayBackupRecover;
  BACKUP_RECOVER_DATA     *Data = NULL;
  BOOLEAN                 RenderFlag;
  PASSWORD_DEAL_TYPE      CurrentPasswordDealType;

  Status = gBS->LocateProtocol(&gDisplayBackupRecoverProtocolGuid, NULL, (VOID**)&DisplayBackupRecover);
  if (Status !=  EFI_SUCCESS) {
    DisplayBackupRecover = NULL;
  } else {
    Status = DisplayBackupRecover->DispalyBackup(&Data);
  }

  DimensionsWidth  = gStatementDimensions.RightColumn - gStatementDimensions.LeftColumn;
  DimensionsHeight = gStatementDimensions.BottomRow - gStatementDimensions.TopRow;

  NullCharacter    = CHAR_NULL;
  ScreenSize       = LibGetStringWidth (Prompt) / sizeof (CHAR16);
  Space[0]         = L' ';
  Space[1]         = CHAR_NULL;

  Question         = MenuOption->ThisTag;
  GetFieldFromOp(Question->OpCode, &Minimum, &Maximum);

  if (Question->OpCode->OpCode == EFI_IFR_PASSWORD_OP) {
    IsPassword = TRUE;
  } else {
    IsPassword = FALSE;
  }

  MaxLen = Maximum + 1;
  TempString = AllocateZeroPool (MaxLen * sizeof (CHAR16));
  ASSERT (TempString);

  if (ScreenSize < (Maximum + 1)) {
    ScreenSize = Maximum + 2;
  }

  if ((ScreenSize + 2) > DimensionsWidth) {
    ScreenSize = DimensionsWidth - 2;
  }
  DMSG ((EFI_D_ERROR, "\n ReadString, ScreenSize :%d. \n", ScreenSize));

  BufferedString = AllocateZeroPool (ScreenSize * 2);
  ASSERT (BufferedString);

  Start = (DimensionsWidth - ScreenSize - 2) / 2 + gStatementDimensions.LeftColumn + 1;
  Top   = ((DimensionsHeight - 6) / 2) + gStatementDimensions.TopRow - 1;

  //
  // Display prompt for string
  //
  CreateMultiStringPopUp (ScreenSize, 4, &NullCharacter, Prompt, Space, &NullCharacter);
  gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_BLACK, EFI_LIGHTGRAY));

  CursorVisible = gST->ConOut->Mode->CursorVisible;
  gST->ConOut->EnableCursor (gST->ConOut, TRUE);

  CurrentCursor = LibGetStringWidth (StringPtr) / 2 - 1;
  if (CurrentCursor != 0) {
    //
    // Show the string which has beed saved before.
    //
    LibSetUnicodeMem (BufferedString, ScreenSize - 1, L' ');
    PrintStringAt (Start + 1, Top + 3, BufferedString);

    if ((LibGetStringWidth (StringPtr) / 2) > (DimensionsWidth - 2)) {
      Index = (LibGetStringWidth (StringPtr) / 2) - DimensionsWidth + 2;
    } else {
      Index = 0;
    }

    if (IsPassword) {
      gST->ConOut->SetCursorPosition (gST->ConOut, Start + 1, Top + 3);
    }

    for (Count = 0; Index + 1 < LibGetStringWidth (StringPtr) / 2; Index++, Count++) {
      BufferedString[Count] = StringPtr[Index];

      if (IsPassword) {
        PrintCharAt ((UINTN)-1, (UINTN)-1, L'*');
      }
    }

    if (!IsPassword) {
      PrintStringAt (Start + 1, Top + 3, BufferedString);
    }
    
    gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
    gST->ConOut->SetCursorPosition (gST->ConOut, Start + LibGetStringWidth (StringPtr) / 2, Top + 3);
  }
  
  RenderFlag = FALSE;
  CurrentPasswordDealType = FirstPrint;
  do {
    InputType = WaitForUserInputEx(&Key, &Mouse);
    if (USER_INPUT_MOUSE == InputType) {
      if (MOUSE_LEFT_DOUBLE_CLICK == Mouse.Action) {
        Key.Key.UnicodeChar = CHAR_CARRIAGE_RETURN;
        Key.Key.ScanCode = SCAN_NULL;
      } else if (MOUSE_RIGHT_CLICK == Mouse.Action) {
        Key.Key.UnicodeChar = CHAR_NULL;
        Key.Key.ScanCode = SCAN_ESC;
      } else {
        continue;
      }
    }


    switch (Key.Key.UnicodeChar) {
    case CHAR_NULL:
      switch (Key.Key.ScanCode) {
      case SCAN_LEFT:
        if (CurrentCursor > 0) {
          CurrentCursor--;
          RenderFlag = TRUE;
          CurrentPasswordDealType = MoveCursor;
        }
        break;

      case SCAN_RIGHT:
        if (CurrentCursor < (LibGetStringWidth (StringPtr) / 2 - 1)) {
          CurrentCursor++;
          RenderFlag = TRUE;
          CurrentPasswordDealType = MoveCursor;
        }
        break;

      case SCAN_ESC:
        FreePool (TempString);
        FreePool (BufferedString);
        gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
        gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
        return EFI_DEVICE_ERROR;

       case SCAN_DELETE:
        for (Index = CurrentCursor; StringPtr[Index] != CHAR_NULL; Index++) {
          StringPtr[Index] = StringPtr[Index + 1];
          RenderFlag = TRUE;
          CurrentPasswordDealType = DeletePassword;
        }
        break;

      default:
        break;
      }

      break;

    case CHAR_CARRIAGE_RETURN:
      if (DisplayBackupRecover != NULL && Data != NULL) {
        DisplayBackupRecover->DispalyRecover(Data);
        FreePool (Data);
        Data = NULL;
      }
      StringWidth = LibGetStringWidth (StringPtr);
      if ((StringWidth >= ((Minimum + 1) * sizeof (CHAR16))) ||
          ((StringWidth == sizeof(CHAR16)) && CouldEmpty)) {

        FreePool (TempString);
        FreePool (BufferedString);
        gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
        gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
        return EFI_SUCCESS;
      } else {
        //
        // Simply create a popup to tell the user that they had typed in too few characters.
        // To save code space, we can then treat this as an error and return back to the menu.
        //
        UiConfirmDialogEx (DIALOG_ATTRIBUTE_RECOVER, DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, &NullCharacter, gShortestString, NULL);
			
        FreePool (TempString);
        FreePool (BufferedString);
        gST->ConOut->SetAttribute (gST->ConOut, EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK));
        gST->ConOut->EnableCursor (gST->ConOut, CursorVisible);
        return EFI_DEVICE_ERROR;
      }


    case CHAR_BACKSPACE:
      if (StringPtr[0] != CHAR_NULL && CurrentCursor != 0) {
        for (Index = 0; Index < CurrentCursor - 1; Index++) {
          TempString[Index] = StringPtr[Index];
        }
        Count = LibGetStringWidth (StringPtr) / 2 - 1;
        if (Count >= CurrentCursor) {
          for (Index = CurrentCursor - 1, Index2 = CurrentCursor; Index2 < Count; Index++, Index2++) {
            TempString[Index] = StringPtr[Index2];
          }
          TempString[Index] = CHAR_NULL;
        }
        //
        // Effectively truncate string by 1 character
        //
        StrCpyS (StringPtr, MaxLen, TempString);
        CurrentCursor --;
        RenderFlag = TRUE;
        CurrentPasswordDealType = DeletePassword;
      }

    default:
      //
      // Check Control
      //
      if ((Key.KeyState.KeyShiftState & EFI_LEFT_CONTROL_PRESSED) != 0 || 
          (Key.KeyState.KeyShiftState & EFI_RIGHT_CONTROL_PRESSED) != 0 ||
          (Key.KeyState.KeyShiftState & EFI_LEFT_ALT_PRESSED) != 0 ||
          (Key.KeyState.KeyShiftState & EFI_RIGHT_ALT_PRESSED) != 0) {
        break;
      }

      //
      // Check invalid char.
      //
      if (!IsValidPdKey(&Key.Key) && Key.Key.UnicodeChar != CHAR_BACKSPACE) {
        break;
      }

      //
      // If it is the beginning of the string, don't worry about checking maximum limits
      //
      if ((StringPtr[0] == CHAR_NULL) && (Key.Key.UnicodeChar != CHAR_BACKSPACE)) {
        StrnCpyS (StringPtr, MaxLen, &Key.Key.UnicodeChar, 1);
        CurrentCursor++;
        RenderFlag = TRUE;
        CurrentPasswordDealType = InputPassword;
      } else if ((LibGetStringWidth (StringPtr) < ((Maximum + 1) * sizeof (CHAR16))) && (Key.Key.UnicodeChar != CHAR_BACKSPACE)) {
        KeyPad[0] = Key.Key.UnicodeChar;
        KeyPad[1] = CHAR_NULL;
        Count = LibGetStringWidth (StringPtr) / 2 - 1;
        if (CurrentCursor < Count) {
          for (Index = 0; Index < CurrentCursor; Index++) {
            TempString[Index] = StringPtr[Index];
          }
		  TempString[Index] = CHAR_NULL;
          StrCatS (TempString, MaxLen, KeyPad);
          StrCatS (TempString, MaxLen, StringPtr + CurrentCursor);
          StrCpyS (StringPtr, MaxLen, TempString);
        } else {
          StrCatS (StringPtr, MaxLen, KeyPad);
        }
        CurrentCursor++;
        RenderFlag = TRUE;
        CurrentPasswordDealType = InputPassword;
      }

      //
      // If the width of the input string is now larger than the screen, we nee to
      // adjust the index to start printing portions of the string
      //
      if ((LibGetStringWidth (StringPtr) / 2) > (DimensionsWidth - 2)) {
        Index = (LibGetStringWidth (StringPtr) / 2) - DimensionsWidth + 2;
      } else {
        Index = 0;
      }

      for (Count = 0; Index + 1 < LibGetStringWidth (StringPtr) / 2; Index++, Count++) {
        BufferedString[Count] = StringPtr[Index];
      }
      break;
    }
    if (StrLen(StringPtr) == 1 && Key.Key.UnicodeChar != CHAR_BACKSPACE && Key.Key.UnicodeChar != CHAR_NULL) {
        CurrentPasswordDealType = FirstPrint;
      }
    if (RenderFlag == TRUE) {
      RenderFlag = FALSE;
      RenderString (CurrentPasswordDealType,
                    Start + 1, 
                    Top + 3, 
                    ScreenSize - 1, 
                    IsPassword ? NULL : StringPtr, 
                    StrLen(StringPtr),
                    EFI_TEXT_ATTR (EFI_BLACK, EFI_LIGHTGRAY),
                    TRUE,
                    Start + 1 + CurrentCursor,
                    Top + 3,
                    EFI_TEXT_ATTR (EFI_LIGHTGRAY, EFI_BLACK)
                    );
    }
  } while (TRUE);

}

/**
  Process password op code.

  @param  MenuOption             The menu for current password op code.
  @param  UserInput              User input data.

  @retval EFI_SUCCESS            Question Option process success.
  @retval Other                  Question Option process fail.

**/
EFI_STATUS
PasswordProcess (
  IN  UI_MENU_OPTION              *MenuOption,
  OUT USER_INPUT                  *UserInput
  )
{
  CHAR16                          *StringPtr;
  CHAR16                          *TempString;
  UINTN                           Maximum;
  EFI_STATUS                      Status;
  EFI_IFR_PASSWORD                *PasswordInfo;
  FORM_DISPLAY_ENGINE_STATEMENT   *Question;
  BOOLEAN                         HasOldPassword = FALSE;
  

  Question     = MenuOption->ThisTag;
  PasswordInfo = (EFI_IFR_PASSWORD *) Question->OpCode;
  Maximum      = PasswordInfo->MaxSize;
  Status       = EFI_SUCCESS;

  StringPtr = AllocateZeroPool ((Maximum + 1) * sizeof (CHAR16));
  ASSERT (StringPtr);

  DMSG ((EFI_D_ERROR, "\n PasswordProcess, MenuOption :%s.\n", MenuOption->Description));	    
  //
  // Use a NULL password to test whether old password is required
  //
  *StringPtr = 0;
  Status = Question->PasswordCheck (NULL, Question, StringPtr);
  DMSG ((EFI_D_ERROR, "PasswordProcess, PasswordCheck :%r.\n", Status));
  if (Status == EFI_NOT_AVAILABLE_YET || Status == EFI_UNSUPPORTED) {
    //
    // Password can't be set now. 
    //
    if (Status == EFI_UNSUPPORTED) {
      UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, L"", gPasswordUnsupported, NULL);
    }
    FreePool (StringPtr);
    return EFI_SUCCESS;
  }

  if (EFI_ERROR (Status)) {
    HasOldPassword = TRUE;
    //
    // Old password exist, ask user for the old password
    //
    Status = ReadString (MenuOption, gPromptForPassword, StringPtr, FALSE);
    DMSG ((EFI_D_ERROR, "PasswordProcess, Old Exist, ReadString :%r-%s.\n", Status, StringPtr));
    if (EFI_ERROR (Status)) {
      FreePool (StringPtr);
      return Status;
    }

    //
    // Check user input old password
    //
    Status = Question->PasswordCheck (NULL, Question, StringPtr);
    DMSG ((EFI_D_ERROR, "PasswordProcess, input old, PasswordCheck :%r.\n", Status));
    if (EFI_ERROR (Status)) {
      if (Status == EFI_NOT_READY) {
        //
        // Typed in old password incorrect
        //
        PasswordInvalid ();
      } else {
        Status = EFI_SUCCESS;
      }

      FreePool (StringPtr);
      return Status;
    }
  }
  
  //
  // Ask for new password
  //
  ZeroMem (StringPtr, (Maximum + 1) * sizeof (CHAR16));
  Status = ReadString (MenuOption, gPromptForNewPassword, StringPtr, HasOldPassword);
  DMSG ((EFI_D_ERROR, "PasswordProcess, Ask for New, ReadString :%r-%s.\n", Status, StringPtr));
  if (EFI_ERROR (Status)) {
    //
    // Reset state machine for password
    //
    Question->PasswordCheck (NULL, Question, NULL);
    FreePool (StringPtr);
    return Status;
  }
  
  //
  // Confirm new password
  //
  TempString = AllocateZeroPool ((Maximum + 1) * sizeof (CHAR16));
  ASSERT (TempString);
  Status = ReadString (MenuOption, gConfirmPassword, TempString, HasOldPassword);
  DMSG ((EFI_D_ERROR, "PasswordProcess, Confirm New, ReadString :%r-%s.\n", Status, StringPtr));
  if (EFI_ERROR (Status)) {
    //
    // Reset state machine for password
    //
    Question->PasswordCheck (NULL, Question, NULL);
    FreePool (StringPtr);
    FreePool (TempString);
    return Status;
  }
  
  //
  // Compare two typed-in new passwords
  //
  if (StrCmp (StringPtr, TempString) == 0) {     
    UserInput->InputValue.Buffer = AllocateCopyPool (Question->CurrentValue.BufferLen, StringPtr);
    UserInput->InputValue.BufferLen = Question->CurrentValue.BufferLen;
    UserInput->InputValue.Type = Question->CurrentValue.Type;
    UserInput->InputValue.Value.string = HiiSetString(MenuOption->Handle, UserInput->InputValue.Value.string, StringPtr, NULL);

    Status = EFI_SUCCESS;
  } else {
    //
    // Reset state machine for password
    //
    Question->PasswordCheck (NULL, Question, NULL);
  
    //
    // Two password mismatch, prompt error message
    //
    UiConfirmDialog(DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, L"", gConfirmError, NULL);

    Status = EFI_INVALID_PARAMETER;
  }
  
  ZeroMem (TempString, (Maximum + 1) * sizeof (CHAR16));
  ZeroMem (StringPtr, (Maximum + 1) * sizeof (CHAR16));  
  FreePool (TempString);
  FreePool (StringPtr);

  DMSG ((EFI_D_ERROR, "PasswordProcess, End :%r.\n", Status));
  return Status;
}

