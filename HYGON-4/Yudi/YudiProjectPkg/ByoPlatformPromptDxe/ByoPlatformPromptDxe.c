/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoPlatformPromptDxe.c

Abstract:
  Display prompt information: "Driver Loading ...".
  Common Start at gEventAfterConnectConsoleDeviceGuid, stop at gPlatAfterConsoleEndProtocolGuid.

  If in legacy boot mode, stop at gPlatAfterConnectSequenceProtocolGuid
  and start it at gBdsAllDriversConnectedProtocolGuid again.

  FullScreenLogo BIOS: If need display Logo, then delay it to gPlatAfterConsoleEndProtocolGuid(Common Stop, Logo Show 2 Second).

Revision History:

**/

#include <PiDxe.h>
#include <Protocol/GraphicsOutput.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/HobLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/BootLogoLib.h>
#include <SysMiscCfg.h>
#include <BoardIdType.h>
#include <SetupVariable.h>


EFI_EVENT                     gPromptEvent;
CHAR16                        *gPromptString;
UINTN                         gPromptStringLen;
EFI_HII_HANDLE                mHiiHandle = NULL;


EFI_STATUS
PrintPrompt (
  IN  UINT32      ScreenLine,
  IN  CHAR8       *Format,
  ...
  )
{
  EFI_STATUS                     Status;
  CHAR16                         Buffer[0x100];
  VA_LIST                        Marker;
  UINTN                          RightColumn;
  UINTN                          BottomRow;
  STATIC UINT32                  Row = 0;
  UINTN                          Index, IndexRow;
  UINTN                          PrintHeight;
  UINTN                          PrintX, PrintY;
  CHAR16                         Space[0x100];
  INT32                          SavedAttribute;
  INT32                          SavedCursorColumn;
  INT32                          SavedCursorRow;
  BOOLEAN                        SavedCursorVisible;

  //
  // Convert the DEBUG() message to a Unicode String
  //
  SetMem (&Buffer[0], sizeof (Buffer), 0x0);
  VA_START (Marker, Format);
  UnicodeVSPrintAsciiFormat (Buffer, sizeof(Buffer),  Format, Marker);
  VA_END (Marker);

  SavedAttribute     = gST->ConOut->Mode->Attribute;
  SavedCursorColumn  = gST->ConOut->Mode->CursorColumn;
  SavedCursorRow     = gST->ConOut->Mode->CursorRow;
  SavedCursorVisible = gST->ConOut->Mode->CursorVisible;

  gST->ConOut->SetAttribute (gST->ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);

  //
  // Print on right and bottom of screen.
  //
  RightColumn = 0;
  BottomRow = 0;
  Status= gST->ConOut->QueryMode (
                 gST->ConOut,
                 gST->ConOut->Mode->Mode,
                 &RightColumn,
                 &BottomRow
                 );
  if (EFI_ERROR(Status)) {
    Row = 0;
    return Status;
  }
  if (RightColumn == 0) {
    RightColumn = 100;
  }
  if (BottomRow == 0) {
    BottomRow = 30;
  }


  if (ScreenLine == 0) {
    Row = 0;
  } else if (ScreenLine == 0xFF) {
    Row++;
  } else {
    Row = ScreenLine;
  }

  PrintHeight = 1;
  if (Row >= PrintHeight) {
    Row = 0;
  }
  PrintX = 4;
  PrintY = BottomRow - 3;

  //
  //Clean all background.
  //
  for(Index = 0; Index < RightColumn - PrintX; Index++ ) {
    Space[Index] = L' ';
  }
  Space[Index] = CHAR_NULL;
  
  if (!Row) {
    for(IndexRow = 0; IndexRow < PrintHeight; IndexRow++ ) {
      gST->ConOut->SetCursorPosition(gST->ConOut, PrintX + gPromptStringLen, (PrintY + IndexRow));
      gST->ConOut->EnableCursor(gST->ConOut, FALSE);
      gST->ConOut->OutputString(gST->ConOut, Space);  //Output to all display devices.
    }
  }

  //
  //Print the Debug string.
  //
  if (Buffer[0] != CHAR_NULL) {
    gST->ConOut->SetCursorPosition(gST->ConOut, PrintX, (PrintY + Row));
    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    gST->ConOut->OutputString(gST->ConOut, Buffer);  //Output to all display devices.
  } else {
    gST->ConOut->SetCursorPosition(gST->ConOut, PrintX, (PrintY + IndexRow));
    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
    gST->ConOut->OutputString(gST->ConOut, Space);  //Output to all display devices.
  }

  // DEBUG((EFI_D_ERROR, "%a(), ScreenLine %d-%s.\n", __FUNCTION__, Row, Buffer));
  gST->ConOut->SetAttribute (gST->ConOut, SavedAttribute);
  gST->ConOut->SetCursorPosition(gST->ConOut, SavedCursorColumn, SavedCursorRow);
  gST->ConOut->EnableCursor(gST->ConOut, SavedCursorVisible);
  return EFI_SUCCESS;
}

EFI_STATUS
PromptEventNotify (
  IN  EFI_EVENT    Event,
  IN  VOID    *Context
  )
{
  STATIC UINTN  WarnCount = 0;
  UINTN         Mode;

  Mode = WarnCount % 3;
  // DEBUG((EFI_D_ERROR, "%a(), WarnCount :%d.\n", __FUNCTION__, WarnCount, gPromptString));

  switch (Mode) {
    case 0:
      PrintPrompt (0xFF, "%s .  ", gPromptString);
      break;

    case 1:
      PrintPrompt (0xFF, "%s .. ", gPromptString);
      break;

    case 2:
      PrintPrompt (0xFF, "%s ...", gPromptString);
      break;

    default:
      PrintPrompt (0xFF, "%s .  ", gPromptString);
      break;
  }

  WarnCount ++;
  return EFI_SUCCESS; 
}


EFI_EVENT
StartPrompt (
  )
{
  EFI_STATUS     Status;
  EFI_BOOT_MODE  BootMode;

  BootMode = GetBootModeHob();
  if (BootMode==BOOT_IN_RECOVERY_MODE) {
    return NULL;
  }
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));

  gPromptStringLen = 0;
  gPromptString = HiiGetString (mHiiHandle, STRING_TOKEN (STR_DEFAULT_PROMPT), NULL);
  gPromptStringLen = StrLen(gPromptString);
  // DEBUG((EFI_D_ERROR, "%a(), gPromptStringLen :0x%x.\n", __FUNCTION__, gPromptStringLen));

  //
  //Create 1s Timer event.
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  (EFI_EVENT_NOTIFY) PromptEventNotify,
                  NULL,
                  &gPromptEvent
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Status = gBS->SetTimer(gPromptEvent, TimerPeriodic, 1000 * 10000);
  if (EFI_ERROR (Status)) {
    gBS->CloseEvent(gPromptEvent);
    return NULL;
  }

  return gPromptEvent;
}


VOID
StopPrompt (
  )
{
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  gBS->CloseEvent (gPromptEvent);
  
  if (gPromptString) {
    FreePool (gPromptString);
  }
  gPromptStringLen = 0;
  PrintPrompt (0xFF, "", NULL);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);

  return;
}

VOID
EFIAPI
StartDriverLoadPromptEventNotify (
  EFI_EVENT                               Event,
  VOID                                    *Context
  )
{
  EFI_STATUS  Status;
  VOID        *Interface;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Start "Driver Loading ..."
  //
  StartPrompt ();

  return;	
}


VOID
EFIAPI
StartPromptAfterConnectConsoleDevice (
  EFI_EVENT                               Event,
  VOID                                    *Context
  )
{
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  //
  // Start "Driver Loading ..."
  //
  StartPrompt ();

  gBS->CloseEvent (Event);
}

VOID
EFIAPI
CommonStopPrompt (
  VOID
  )
{
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  StopPrompt();
}

EFI_STATUS
EFIAPI
ByoPlatformPromptDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  mHiiHandle = HiiAddPackages (
                 &gEfiCallerIdGuid,
                 ImageHandle,
                 ByoPlatformPromptDxeStrings,
                 NULL
                 );
  ASSERT (mHiiHandle != NULL);

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  StartPromptAfterConnectConsoleDevice, // Common Start
                  NULL,
                  &gEventAfterConnectConsoleDeviceGuid,
                  &Event
                  );

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gPlatAfterConsoleEndProtocolGuid, CommonStopPrompt,  // Common Stop
                  NULL
                  );

  if (PcdGet8 (PcdBiosBootModeType) != BIOS_BOOT_UEFI_OS) {
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &ImageHandle,
                    &gPlatAfterConnectSequenceProtocolGuid, StopPrompt, // Stop Prompt before run Legacy OptionRom
                    NULL
                    );

    {
      VOID  *StartDriverLoadPromptEventRegister;

      EfiCreateProtocolNotifyEvent (
        &gBdsAllDriversConnectedProtocolGuid,
        TPL_CALLBACK,
        StartDriverLoadPromptEventNotify,  // Restart Prompt after run Legacy OptionRom
        NULL,
        &StartDriverLoadPromptEventRegister
      );
    }
  }

  return Status;
}
