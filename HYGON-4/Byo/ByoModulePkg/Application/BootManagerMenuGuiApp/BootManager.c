/** @file
  The boot manager reference implementation

  Copyright (c) 2004 - 2022, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/

#include "BootManager.h"

UINT16             mKeyInput;

//
// Boot video resolution and text mode.
//
UINT32    mBmBootHorizontalResolution    = 0;
UINT32    mBmBootVerticalResolution      = 0;
UINT32    mBmBootTextModeColumn          = 0;
UINT32    mBmBootTextModeRow             = 0;
//
// BIOS setup video resolution and text mode.
//
UINT32    mBmSetupTextModeColumn         = 0;
UINT32    mBmSetupTextModeRow            = 0;
UINT32    mBmSetupHorizontalResolution   = 0;
UINT32    mBmSetupVerticalResolution     = 0;

BOOLEAN   mBmModeInitialized             = FALSE;

UINT16    mExitAndBootOption             = 0;

HII_VENDOR_DEVICE_PATH  mBootManagerHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    EFI_CALLER_ID_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

BOOT_MANAGER_CALLBACK_DATA  gBootManagerPrivate = {
  BOOT_MANAGER_CALLBACK_DATA_SIGNATURE,
  NULL,
  NULL,
  {
    BootManagerExtractConfig,
    BootManagerRouteConfig,
    BootManagerCallback
  }
};

/**
  This function will change video resolution and text mode
  according to defined setup mode or defined boot mode

  @param  IsSetupMode   Indicate mode is changed to setup mode or boot mode.

  @retval  EFI_SUCCESS  Mode is changed successfully.
  @retval  Others             Mode failed to be changed.

**/
EFI_STATUS
BmSetConsoleMode (
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

  MaxGopMode  = 0;
  MaxTextMode = 0;

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
    // The required resolution and text mode is setup mode.
    //
    NewHorizontalResolution = mBmSetupHorizontalResolution;
    NewVerticalResolution   = mBmSetupVerticalResolution;
    NewColumns              = mBmSetupTextModeColumn;
    NewRows                 = mBmSetupTextModeRow;
  } else {
    //
    // The required resolution and text mode is boot mode.
    //
    NewHorizontalResolution = mBmBootHorizontalResolution;
    NewVerticalResolution   = mBmBootVerticalResolution;
    NewColumns              = mBmBootTextModeColumn;
    NewRows                 = mBmBootTextModeRow;
  }

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
                  Status = PcdSet32S (PcdConOutColumn, mBmSetupTextModeColumn);
                  ASSERT_EFI_ERROR (Status);
                  Status = PcdSet32S (PcdConOutRow, mBmSetupTextModeRow);
                  ASSERT_EFI_ERROR (Status);
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

  //
  // Set PCD to Inform GraphicsConsole to change video resolution.
  // Set PCD to Inform Consplitter to change text mode.
  //
  Status = PcdSet32S (PcdVideoHorizontalResolution, NewHorizontalResolution);
  ASSERT_EFI_ERROR (Status);
  Status = PcdSet32S (PcdVideoVerticalResolution, NewVerticalResolution);
  ASSERT_EFI_ERROR (Status);
  Status = PcdSet32S (PcdConOutColumn, NewColumns);
  ASSERT_EFI_ERROR (Status);
  Status = PcdSet32S (PcdConOutRow, NewRows);
  ASSERT_EFI_ERROR (Status);

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

  return EFI_SUCCESS;
}

/**

  Check whether a reset is needed,if reset is needed, Popup a menu to notice user.

**/
VOID
BmSetupResetReminder (
  VOID
  )
{
  EFI_INPUT_KEY                 Key;
  CHAR16                        *StringBuffer1;
  CHAR16                        *StringBuffer2;
  EFI_STATUS                    Status;
  EDKII_FORM_BROWSER_EXTENSION2_PROTOCOL *FormBrowserEx2;

  //
  // Use BrowserEx2 protocol to check whether reset is required.
  //
  Status = gBS->LocateProtocol (&gEdkiiFormBrowserEx2ProtocolGuid, NULL, (VOID **) &FormBrowserEx2);
  //
  //check any reset required change is applied? if yes, reset system
  //
  if (!EFI_ERROR(Status) && FormBrowserEx2->IsResetRequired ()) {
    StringBuffer1 = AllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16));
    ASSERT (StringBuffer1 != NULL);
    StringBuffer2 = AllocateZeroPool (MAX_STRING_LEN * sizeof (CHAR16));
    ASSERT (StringBuffer2 != NULL);
    StringBuffer1 = HiiGetString(gBootManagerPrivate.HiiHandle, STRING_TOKEN(STR_RESET_APPLY_NOW), NULL);
    StringBuffer2 = HiiGetString(gBootManagerPrivate.HiiHandle, STRING_TOKEN(STR_ENTER_TO_RESET),  NULL); 
    //
    // Popup a menu to notice user
    //
    do {
      CreatePopUp (EFI_LIGHTGRAY | EFI_BACKGROUND_BLUE, &Key, StringBuffer1, StringBuffer2, NULL);
    } while (Key.UnicodeChar != CHAR_CARRIAGE_RETURN);

    FreePool (StringBuffer1);
    FreePool (StringBuffer2);

    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  }
}

/**
  This function converts an input device structure to a Unicode string.

  @param DevPath                  A pointer to the device path structure.

  @return A new allocated Unicode string that represents the device path.

**/
CHAR16 *
BmDevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  EFI_STATUS                       Status;
  CHAR16                           *ToText;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

  if (DevPath == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  (VOID **) &DevPathToText
                  );
  ASSERT_EFI_ERROR (Status);
  ToText = DevPathToText->ConvertDevicePathToText (
                            DevPath,
                            FALSE,
                            TRUE
                            );
  ASSERT (ToText != NULL);
  return ToText;
}

/**
  Check if it's a Device Path pointing to BootManagerMenu.

  @param  DevicePath     Input device path.

  @retval TRUE   The device path is BootManagerMenu File Device Path.
  @retval FALSE  The device path is NOT BootManagerMenu File Device Path.
**/
static
BOOLEAN
BmIsSetupUiAppPath (
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
)
{
  EFI_HANDLE                      FvHandle;
  VOID                            *NameGuid;
  EFI_STATUS                      Status;

  Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &DevicePath, &FvHandle);
  if (!EFI_ERROR (Status)) {
    NameGuid = EfiGetNameGuidFromFwVolDevicePathNode ((CONST MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) DevicePath);
    if (NameGuid != NULL) {
      return CompareGuid (NameGuid, PcdGetPtr (PcdBootManagerMenuFile));
    }
  }

  return FALSE;
}

/**
  This function invokes Boot Manager. It then enumerate all boot options. If
  a boot option from the Boot Manager page is selected, Boot Manager will boot
  from this boot option.

**/
VOID
UpdateBootManager (
  VOID
  )
{
  UINTN                         Index;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption;
  UINTN                         BootOptionCount;
  EFI_STRING_ID                 Token;
  CHAR16                        *HelpString;
  EFI_STRING_ID                 HelpToken;
  UINT16                        *TempStr;
  EFI_HII_HANDLE                HiiHandle;
  UINTN                         TempSize;
  VOID                          *StartOpCodeHandle;
  VOID                          *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL            *StartLabel;
  EFI_IFR_GUID_LABEL            *EndLabel;
  BOOLEAN                       NeedEndOp;
  UINTN                         MaxLen;
  UINT16                        SetupUiKeyId;

  BootOption = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);

  HiiHandle = gBootManagerPrivate.HiiHandle;

  //
  // Allocate space for creation of UpdateData Buffer
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_BOOT_OPTION;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_BOOT_OPTION_END;
  mKeyInput = 0;
  NeedEndOp = FALSE;
  SetupUiKeyId = 0;
  for (Index = 0; Index < BootOptionCount; Index++) {
    //
    // At this stage we are creating a menu entry, thus the Keys are reproduceable
    //
    mKeyInput++;

    if (BmIsSetupUiAppPath(BootOption[Index].FilePath)) {
      SetupUiKeyId = mKeyInput;
      continue;
    }
    //
    // Don't display hidden and inactive boot options
    //
    if (((BootOption[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) || ((BootOption[Index].Attributes & LOAD_OPTION_ACTIVE) == 0)) {
      continue;
    }

    ASSERT (BootOption[Index].Description != NULL);

    Token = HiiSetString (HiiHandle, 0, BootOption[Index].Description, NULL);

    TempStr = BmDevicePathToStr (BootOption[Index].FilePath);
    TempSize = StrSize (TempStr);
    HelpString = AllocateZeroPool (TempSize + StrSize (L"Device Path : "));
    MaxLen = (TempSize + StrSize (L"Device Path : "))/sizeof(CHAR16);
    ASSERT (HelpString != NULL);
    StrCatS (HelpString, MaxLen, L"Device Path : ");
    StrCatS (HelpString, MaxLen, TempStr);

    HelpToken = HiiSetString (HiiHandle, 0, HelpString, NULL);
    FreePool (HelpString);
    FreePool (TempStr);

    HiiCreateActionOpCode (
      StartOpCodeHandle,
      mKeyInput,
      Token,
      HelpToken,
      EFI_IFR_FLAG_CALLBACK,
      0
      );
  }

  if (SetupUiKeyId > 0) {
    Token = HiiSetString (HiiHandle, 0, BootOption[SetupUiKeyId - 1].Description, NULL);
    TempStr = BmDevicePathToStr (BootOption[SetupUiKeyId - 1].FilePath);
    TempSize = StrSize (TempStr);
    HelpString = AllocateZeroPool (TempSize + StrSize (L"Device Path : "));
    MaxLen = (TempSize + StrSize (L"Device Path : "))/sizeof(CHAR16);
    ASSERT (HelpString != NULL);
    StrCatS (HelpString, MaxLen, L"Device Path : ");
    StrCatS (HelpString, MaxLen, TempStr);

    HelpToken = HiiSetString (HiiHandle, 0, HelpString, NULL);
    FreePool (HelpString);
    FreePool (TempStr);

    HiiCreateActionOpCode (
      StartOpCodeHandle,
      SetupUiKeyId,
      Token,
      HelpToken,
      EFI_IFR_FLAG_CALLBACK,
      0
      );
  }

  if (NeedEndOp) {
    HiiCreateEndOpCode (StartOpCodeHandle);
  }

  HiiUpdateForm (
    HiiHandle,
    &gBootMenuFileGuid,
    BOOT_MANAGER_FORM_ID,
    StartOpCodeHandle,
    EndOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  EfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);
}

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Request         A null-terminated Unicode string in <ConfigRequest> format.
  @param Progress        On return, points to a character in the Request string.
                         Points to the string's null terminator if request was successful.
                         Points to the most recent '&' before the first failing name/value
                         pair (or the beginning of the string if the failure is in the
                         first name/value pair) if the request was not successful.
  @param Results         A null-terminated Unicode string in <ConfigAltResp> format which
                         has all values filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval  EFI_SUCCESS            The Results is filled with the requested values.
  @retval  EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval  EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
BootManagerExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *Progress = Request;
  return EFI_NOT_FOUND;
}

/**
  This function processes the results of changes in configuration.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Configuration   A null-terminated Unicode string in <ConfigResp> format.
  @param Progress        A pointer to a string filled in with the offset of the most
                         recent '&' before the first failing name/value pair (or the
                         beginning of the string if the failure is in the first
                         name/value pair) or the terminating NULL if all was successful.

  @retval  EFI_SUCCESS            The Results is processed successfully.
  @retval  EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
BootManagerRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;

  return EFI_NOT_FOUND;
}

/**
  Initial the boot mode related parameters.

**/
VOID
BmInitialBootModeInfo (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_GRAPHICS_OUTPUT_PROTOCOL       *GraphicsOutput;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL    *SimpleTextOut;
  UINTN                              BootTextColumn;
  UINTN                              BootTextRow;

  if (mBmModeInitialized) {
    return;
  }

  //
  // After the console is ready, get current video resolution
  // and text mode before launching setup at first time.
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

  if (GraphicsOutput != NULL) {
    //
    // Get current video resolution and text mode.
    //
    mBmBootHorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
    mBmBootVerticalResolution   = GraphicsOutput->Mode->Info->VerticalResolution;
    mBmSetupHorizontalResolution = GraphicsOutput->Mode->Info->HorizontalResolution;
    mBmSetupVerticalResolution   = GraphicsOutput->Mode->Info->VerticalResolution;
  } else {
    mBmSetupHorizontalResolution = PcdGet32 (PcdSetupVideoHorizontalResolution);
    mBmSetupVerticalResolution   = PcdGet32 (PcdSetupVideoVerticalResolution);
  }

  if (SimpleTextOut != NULL) {
    Status = SimpleTextOut->QueryMode (
                              SimpleTextOut,
                              SimpleTextOut->Mode->Mode,
                              &BootTextColumn,
                              &BootTextRow
                              );
    mBmBootTextModeColumn = (UINT32)BootTextColumn;
    mBmBootTextModeRow    = (UINT32)BootTextRow;
    mBmSetupTextModeColumn = (UINT32)BootTextColumn;
    mBmSetupTextModeRow    = (UINT32)BootTextRow;
  } else {
    mBmSetupTextModeColumn = PcdGet32 (PcdSetupConOutColumn);
    mBmSetupTextModeRow    = PcdGet32 (PcdSetupConOutRow);
  }

  mBmModeInitialized           = TRUE;
}

static
BOOLEAN
IsFilePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath,
  IN  EFI_GUID                     *FileGuid
)
{
  EFI_HANDLE                      FvHandle;
  VOID                            *NameGuid;
  EFI_STATUS                      Status;

  Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &DevicePath, &FvHandle);
  if (!EFI_ERROR (Status)) {
    NameGuid = EfiGetNameGuidFromFwVolDevicePathNode ((CONST MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) DevicePath);
    if (NameGuid != NULL) {
      return CompareGuid (NameGuid, FileGuid);
    }
  }

  return FALSE;
}

/**
  This call back function is registered with Boot Manager formset.
  When user selects a boot option, this call back function will
  be triggered. The boot option is saved for later processing.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action          Specifies the type of action taken by the browser.
  @param QuestionId      A unique value which is sent to the original exporting driver
                         so that it can identify the type of data to expect.
  @param Type            The type of value for the question.
  @param Value           A pointer to the data being sent to the original exporting driver.
  @param ActionRequest   On return, points to the action requested by the callback function.

  @retval  EFI_SUCCESS           The callback successfully handled the action.
  @retval  EFI_INVALID_PARAMETER The setup browser call this function with invalid parameters.

**/
EFI_STATUS
EFIAPI
BootManagerCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION *BootOption;
  UINTN                        BootOptionCount;
  EFI_STATUS                            Status;
  BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL   *DisplayBackupRecover = NULL;
  BACKUP_RECOVER_DATA                   *Data = NULL;
  EDKII_FORM_BROWSER_EXTENSION2_PROTOCOL *BrowserExtension;

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN) {
    //
    //Means enter the boot manager form.
    //Update the boot manage page,because the boot option may changed.
    //
    if (QuestionId == 0x1212){
      UpdateBootManager();
    }
    return EFI_SUCCESS;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    //
    // Do nothing for other UEFI Action. Only do call back when data is changed.
    //
    return EFI_UNSUPPORTED;
  }

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  BootOption = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);

  //
  //check any reset required change is applied? if yes, reset system
  //
  BmSetupResetReminder ();

  //
  // parse the selected option
  //
  BmInitialBootModeInfo ();

  if (IsFilePath(BootOption[QuestionId - 1].FilePath, &gUefiShellFileGuid)) {
    Status = gBS->LocateProtocol(&gDisplayBackupRecoverProtocolGuid, NULL, (VOID**)&DisplayBackupRecover);
    if (!EFI_ERROR (Status)) {
      Status = DisplayBackupRecover->DispalyBackup(&Data);
    }

    EfiBootManagerBoot (&BootOption[QuestionId - 1]);

    if (DisplayBackupRecover != NULL && Data != NULL) {
      DisplayBackupRecover->DispalyRecover(Data);
      FreePool (Data);
      Data = NULL;
    }
  } else if (BmIsSetupUiAppPath(BootOption[QuestionId - 1].FilePath)) {
    Status = gBS->LocateProtocol (
                    &gEdkiiFormBrowserEx2ProtocolGuid,
                    NULL,
                    (void**)&BrowserExtension
                    );
    if (!EFI_ERROR(Status)) {
      BrowserExtension->ExecuteAction(BROWSER_ACTION_EXIT, 0);  
    }
    mExitAndBootOption = QuestionId - 1;
  } else {
    EfiBootManagerBoot (&BootOption[QuestionId - 1]);
  }

  EfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);

  return EFI_SUCCESS;
}

static
VOID
EFIAPI
OemPlatformReset (VOID) {

}

/**

  Install Boot Manager Menu driver.

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval  EFI_SUCCESS  Install Boot manager menu success.
  @retval  Other        Return error status.

**/
EFI_STATUS
EFIAPI
BootManagerMenuEntry (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_BYO_FORMSET_MANAGER_PROTOCOL *FormsetManager;
  OEM_PLATFORM_RESET               PlatformReset = NULL;
  EFI_BYO_FORM_BROWSER_EXTENSION_PROTOCOL *ByoFormBrowserExtProtocol;
  EFI_BOOT_MANAGER_LOAD_OPTION            *BootOption;
  UINTN                                   BootOptionCount;

  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
  SignalProtocolEvent(gBS, &gEfiBootMenuEnterGuid, TRUE);

  //
  // Install Device Path Protocol and Config Access protocol to driver handle
  //
  gBootManagerPrivate.DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gBootManagerPrivate.DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mBootManagerHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &gBootManagerPrivate.ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data
  //
  gBootManagerPrivate.HiiHandle = HiiAddPackages (
                                    &gBootMenuFileGuid,
                                    gBootManagerPrivate.DriverHandle,
                                    BootManagerVfrBin,
                                    BootManagerMenuAppStrings,
                                    NULL
                                    );
  ASSERT (gBootManagerPrivate.HiiHandle != NULL);

  Status = gBS->LocateProtocol (
                  &gEfiByoFormBrowserExProtocolGuid,
                  NULL,
                  (VOID**)&ByoFormBrowserExtProtocol
                  );
  if (!EFI_ERROR(Status)) {
    PlatformReset = ByoFormBrowserExtProtocol->PlatformReset;
    ByoFormBrowserExtProtocol->PlatformReset = OemPlatformReset;
  }

  Status = gBS->LocateProtocol (
                  &gEfiByoFormsetManagerProtocolGuid,
                  NULL,
                  (VOID **) &FormsetManager
                  );

  if (!EFI_ERROR(Status) ) {
    FormsetManager->Insert (FormsetManager, &gBootMenuFileGuid);
    FormsetManager->Run (FormsetManager, &gBootMenuFileGuid);
  }

  Status = gBS->UninstallMultipleProtocolInterfaces (
                  gBootManagerPrivate.DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mBootManagerHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &gBootManagerPrivate.ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  HiiRemovePackages (gBootManagerPrivate.HiiHandle);

  FormsetManager->Remove (FormsetManager, &gBootMenuFileGuid);

  if (ByoFormBrowserExtProtocol != NULL) {
    ByoFormBrowserExtProtocol->PlatformReset = PlatformReset;
  }

  if(!EFI_ERROR(Status)) {
    BootOption = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
    EfiBootManagerBoot (&BootOption[mExitAndBootOption]);
    EfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);
  }

  return EFI_SUCCESS;
}
