/** @file

Copyright (c) 2010 - 2015, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __SETUP_UI_LIB_H__
#define __SETUP_UI_LIB_H__

#include <Protocol/SetupMouseProtocol.h>

#pragma pack(1)

#define IFR_GUID_GRID_MODE_AUTO       (0)
#define IFR_GUID_GRID_MODE_ALIGN_LEFT (1)

typedef struct _BYO_IFR_GUID_GRID_START {
  EFI_IFR_OP_HEADER        Header;
  EFI_GUID                 Guid;

  UINT8                    Mode;
} BYO_IFR_GUID_GRID_START;
#pragma pack()

typedef enum {
  TEXT_ALIGIN_CENTER,
  TEXT_ALIGIN_LEFT,
  TEXT_ALIGIN_RIGHT,
  TEXT_ALIGIN_CENTER_LOWER,
  TEXT_ALIGIN_MAX
} TEXT_ALIGIN;

typedef enum {
  DIALOG_YESNO,
  DIALOG_WARNING,
  DIALOG_INFO,
  DIALOG_NO_KEY,
  DIALOG_WAIT_KEY,
  DIALOG_NO_REPAINT,
  DIALOG_WAIT_ENTER_KEY,
  DIALOG_BOX,
  DIALOG_BROWSER_YESNO, // ?
  DIALOG_BOX_DEAD_LOOP,
  DIALOG_MAX
} DIALOG_TYPE;

typedef enum {
  SELECTION_NO,
  SELECTION_YES,
  SELECTION_ESC,
  SELECTION_ENTER,
  SELECTION_MAX
} SELECTION_TYPE;

typedef enum {
  USER_INPUT_KEY,
  USER_INPUT_MOUSE,
  USER_INPUT_TIMEOUT,
  USER_INPUT_MAX
} USER_INPUT_TYPE;

typedef struct {
  UINTN    Left;
  UINTN    Right;
  UINTN    Top;
  UINTN    Bottom;
} DIALOG_RANGE;

/**
  Draw a confirm pop up windows based on the Type, strings and Text Align. 
  SELECTION_TYPE will be returned.

**/
SELECTION_TYPE
EFIAPI
UiConfirmDialog (
  IN DIALOG_TYPE       Type,
  IN CHAR16            *Title,
  OUT EFI_INPUT_KEY    *KeyValue  OPTIONAL,
  IN TEXT_ALIGIN       Align,
  IN  CHAR16           *String,
  ...                                           // end with "NULL"
  );

#define DIALOG_ATTRIBUTE_RECOVER   BIT1
#define DIALOG_ATTRIBUTE_NO_REFRESH BIT2   

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
  );

VOID SetDlgTimeOut(UINT8 TimeOutSeconds);
VOID SetDlgCaption(CHAR16 *Caption);


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
  Wait for user Input by Key or Mouse.

  @param Key         The key which is pressed by user.
  @param Mouse      The Mouse which is pressed by user.

  @retval USER_INPUT_TYPE  The Input type

**/

USER_INPUT_TYPE
WaitForUserInputEx (
  OUT  EFI_KEY_DATA      *Key,
  OUT  MOUSE_ACTION_INFO *Mouse
  );

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

/**
  Set Title of Button form Left to Right according Type of Dialog which will 
  be created by UiConfirmDialog(). Setting valid once.

**/
BOOLEAN
UiSetButton (
  IN DIALOG_TYPE    Type,
  IN  CHAR16    *String,
  ...
  );

/**
  Restore Background before creating it and Free memory.
  This service  is only for DIALOG_NO_KEY dialog now.

**/
BOOLEAN
UiClearDialog (
  IN DIALOG_TYPE    Type
  );

/**
  Not FreePool NULL;
  Set string point to NUll afte FreePool.

**/
VOID
UiSafeFree (
  CHAR16 **String
  );

/**
  Restore No Repaint flag after end a serial of No Repaint Dialog.
  This service  is only for DIALOG_NO_REPAINT and DIALOG_BOX dialog now.

**/
BOOLEAN
UiClearNoRepaintFlag (
  IN DIALOG_TYPE    Type
  );

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
  );

/**
  Refresh UiInitializeLibStrings HII String.

**/

VOID
PrintStringOnConsole (
  IN UINTN     Row,
  IN UINTN     Column,
  IN CHAR16    *String,
  IN UINTN     Attribute
  );

typedef enum {
  DLG_OEM_COLOR_KEEP = 0,  
  DLG_OEM_COLOR_RED,
} DLG_OEM_COLOR;

VOID SetOemDialogColor(DLG_OEM_COLOR Color);

EFI_STATUS 
UiSaveStoreScreenRange (
  IN VOID        *BootServices, 
  IN BOOLEAN     IsSave,
  IN UINTN       Start,
  IN UINTN       End,
  IN UINTN       Top,
  IN UINTN       Bottom
  );

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
  );

/**
  Register Windows

  @param[in] X            Windows Start Coord X
  @param[in] Y            Windows Start Coord Y
  @param[in] Width        Windows Width
  @param[in] Height       Windows Height
  @param[in] Top          Window On Top

  @retval NOT NULL        Register Windows Success
  @retval NULL            Register Windows Error

**/
EFI_HANDLE
RegisterWindows (
  IN  UINT32 X, 
  IN  UINT32 Y,
  IN  UINT32 Width,
  IN  UINT32 Height,
  IN  BOOLEAN Top
  );

/**
  UnRegister Windows

  @param[in] Windows      Windows Handle

  @retval EFI_SUCCESS     UnRegister Windows Success
  @retval Other           UnRegister Windows Error

**/
EFI_STATUS
UnRegisterWindows(
  IN  EFI_HANDLE Windows
  );

/**
  Add Dynamically Update Pop Control in Window

  @param[in] Windows      Windows Handle
  @param[in] Title        Windows Title

  @retval NOT NULL        Add ProgressBar Control in Window Success
  @retval NULL            Add ProgressBar Control in Window Success Error

**/
VOID*
AddDynamicallyUpdatePop(
  IN  EFI_HANDLE *Windows,
  IN  CHAR16     *Title   OPTIONAL
  );

/**
  Draw Dynamically Update Pop

  @param[in] Control      ProgressBar Control
  @param[in] Refresh      TRUE   Draw Window
                          FALSE  Draw ProgressBar Control
  @param[in] RateStr      The string below the progress bar

  @retval EFI_SUCCESS     Draw Success
  @retval Other           Draw Error

**/
EFI_STATUS
RefreshDynamicallyUpdatePop(
  IN  VOID     *Control, 
  IN  BOOLEAN  Refresh,
  IN  CHAR16   *RateStr
  );

/**
  Remove Dynamically Update Pop Control in Window

  @param[in] Control      Control in Window

  @retval EFI_SUCCESS     UnRegister Windows Success
  @retval Other           UnRegister Windows Error

**/
EFI_STATUS
DeleteDynamicallyUpdatePop (
  IN  VOID       *Control
  );

#endif
