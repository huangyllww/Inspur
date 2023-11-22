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

#ifndef __CUSTOMIZED_DISPLAY_LIB_H__
#define __CUSTOMIZED_DISPLAY_LIB_H__

#include <Library/SetupUiLib.h>
#include <Protocol/DisplayProtocol.h>
#include <Protocol/SetupMouseProtocol.h>

/**
+------------------------------------------------------------------------------+
|                                 Setup Page                                   |
+------------------------------------------------------------------------------+

Statement
Statement
Statement





+------------------------------------------------------------------------------+
|                                F9=Reset to Defaults        F10=Save          |
| ^"=Move Highlight          <Spacebar> Toggles Checkbox     Esc=Exit          |
+------------------------------------------------------------------------------+
  StatusBar
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
  );

/**
  Print Form layout, include title and help.

  @param[in]  FormData             Form Data to be shown in Page
**/
VOID
PrintFormLayout (
  IN FORM_DISPLAY_ENGINE_FORM       *FormData,
  IN UINTN    MiddleVerticalLineColumn
  );

/**
  Middle vertical line maybe covered by Pop-up Dialog.

  @param[in]  FormData             Form Data to be shown in Page
**/
VOID
RepaintMiddleLine (
  IN FORM_DISPLAY_ENGINE_FORM       *FormData,
  IN UINTN    MiddleVerticalLineColumn
  );

/**
  Clear Screen to the initial state.
**/
VOID
EFIAPI 
ClearDisplayPage (
  VOID
  );

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
  );

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
  );

/**
  Create popup window. 

  This function draws OEM/Vendor specific pop up windows.

  @param[out]  Key    User Input Key
  @param       ...    String to be shown in Popup. The variable argument list is terminated by a NULL.
  
**/
VOID
EFIAPI
CreateDialog (
  OUT EFI_INPUT_KEY  *Key,        OPTIONAL
  ...
  );

/**
  Confirm how to handle the changed data. 
  
  @return Action BROWSER_ACTION_SUBMIT, BROWSER_ACTION_DISCARD or other values.
**/
UINTN
EFIAPI
ConfirmDataChange (
  CHAR16    *Title
  );

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
  );

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
  );

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
  );


/**
  Prints a unicode string with the specified width to the default console, at
  the supplied cursor position, using L"%s" format.

  @param  Column     The cursor position to print the string at. When it is -1, use current Position.
  @param  Row        The cursor position to print the string at. When it is -1, use current Position.
  @param  String     String pointer.
  @param  Width      Width for String to be printed. If the print length of String < Width, 
                     Space char (L' ') will be used to append String. 

  @return Length of string printed to the console

**/
UINTN
EFIAPI
PrintStringAtWithWidth (
  IN UINTN     Column,
  IN UINTN     Row,
  IN CHAR16    *String,
  IN UINTN     Width
  );

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
  );

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
  );

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
  );

/**
  Get OEM/Vendor specific popup attribute colors.

  @retval  Byte code color setting for popup inverse color.
**/
UINT8
EFIAPI
GetPopupInverseColor (
  VOID
  );

/**
  Get OEM/Vendor specific PickList color attribute.

  @retval  Byte code color setting for pick list color.
**/
UINT8
EFIAPI
GetPickListColor (
  VOID
  );

/**
  Get OEM/Vendor specific arrow color attribute.

  @retval  Byte code color setting for arrow color.
**/
UINT8
EFIAPI
GetArrowColor (
  VOID
  );

/**
  Get OEM/Vendor specific info text color attribute.

  @retval  Byte code color setting for info text color.
**/
UINT8
EFIAPI
GetInfoTextColor (
  VOID
  );

/**
  Get OEM/Vendor specific help text color attribute.

  @retval  Byte code color setting for help text color.
**/
UINT8
EFIAPI
GetHelpTextColor (
  VOID
  );

/**
  Get OEM/Vendor specific grayed out text color attribute.

  @retval  Byte code color setting for grayed out text color.
**/
UINT8
EFIAPI
GetGrayedTextColor (
  VOID
  );

/**
  Get OEM/Vendor specific highlighted text color attribute.

  @retval  Byte code color setting for highlight text color.
**/
UINT8
EFIAPI
GetHighlightTextColor (
  VOID
  );

/**
  Get OEM/Vendor specific field text color attribute.

  @retval  Byte code color setting for field text color.
**/
UINT8
EFIAPI
GetFieldTextColor (
  VOID
  );

/**
  Get OEM/Vendor specific subtitle text color attribute.

  @retval  Byte code color setting for subtitle text color.
**/
UINT8
EFIAPI
GetSubTitleTextColor (
  VOID
  );
  
/**
  Check whether formset is in ByoFormSetList.

**/
BOOLEAN
IsByoFormset (
  IN  LIST_ENTRY    *ByoFormSetList,
  IN  EFI_HII_HANDLE    HiiHandle
  );

EFI_STATUS
CreateHelpDialog (
  OUT EFI_INPUT_KEY        *KeyValue
  );


#define FORMSET_CLASS_PLATFORM_SETUP         0x0001
#define FORMSET_CLASS_FRONT_PAGE             0x0002

extern UINT16            gClassOfVfr;

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
);

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
  );

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
);

BOOLEAN
MouseGetHotkey (
  IN MOUSE_ACTION_INFO    *Mouse,
  IN OUT EFI_INPUT_KEY    *Key
  );

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
  );

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
  );

//
// Common String.
//
extern CHAR16    *gStrUiSetupDefaultConfirmation; 
extern CHAR16    *gStrUiSetupConfirmation; 
extern CHAR16    *gStrUiDiscard;
extern CHAR16    *gStrUiDiscardExit;
extern CHAR16    *gStrUiLoadDefault;
extern CHAR16    *gStrUiSaving;
extern CHAR16    *gStrUiSavingExit;
extern CHAR16    *gStrUiSaveUserDefault;
extern CHAR16    *gStrUiLoadUserDefault;
extern CHAR16    *gStrUiLoadFormset;
extern CHAR16    *gConfirmSuccess;
extern CHAR16    *gStrDefaultValueRecovery;
extern CHAR16    *gStrConfigurationSaving;
extern CHAR16    *gStrPromptMsg;

#endif
