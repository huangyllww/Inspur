/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HpHotKey.c

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include "ProjectDxe.h"
#include <Protocol/SimpleTextOut.h>
#include <Library/UefiRuntimeServicesTableLib.h>



VOID
BootSetupHotKeyHandler (
    IN CONST EFI_KEY_DATA  *KeyData
  )
{
  UINTN                             Index;
  CHAR16                            *StrList[7];
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  UINTN                             Columns;
  UINTN                             Rows;
  UINTN                             x, y;
  UINTN                             Attribute;
  CHAR16                            *p;
  UINTN                             Size;
  SETUP_DATA                        SetupData;
  BOOLEAN                           NeedHotKeyPrompt;

  if (KeyData == NULL) {
    return;
  }

  Size = sizeof (SETUP_DATA);
  gRT->GetVariable (
         PLATFORM_SETUP_VARIABLE_NAME,
         &gPlatformSetupVariableGuid,
         NULL,
         &Size,
         &SetupData
         );
  NeedHotKeyPrompt = FALSE;	
  if(SetupData.QuietBootMode == 0 || SetupData.QuietBootMode == 1) {
    NeedHotKeyPrompt = TRUE;
  }  
  if (!NeedHotKeyPrompt && PcdGetBool (PcdForceCloseQuietBoot)) {
    NeedHotKeyPrompt = TRUE;
  }	
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), NeedHotKeyPrompt :%d.\n", __LINE__, NeedHotKeyPrompt));

  if(KeyData->Key.ScanCode == 0xFFFF){

    ConOut = gST->ConOut;
    ConOut->QueryMode(ConOut, ConOut->Mode->Mode, &Columns, &Rows);
    x = ConOut->Mode->CursorColumn;
    y = ConOut->Mode->CursorRow;
    Attribute = ConOut->Mode->Attribute;
    ConOut->SetCursorPosition(ConOut, 2, Rows-2);
    DEBUG((EFI_D_INFO, "%d x %d (%d, %d)\n", Columns, Rows, x, y));

    if((KeyData->Key.UnicodeChar == 0)&&NeedHotKeyPrompt){

      StrList[0] = HiiGetString(gHiiHandle, STRING_TOKEN(STR_HOTKEY_INFO), NULL);
      StrList[1] = HiiGetString(gHiiHandle, STRING_TOKEN(STR_HOTKEY_INFO2), NULL);
      StrList[2] = HiiGetString(gHiiHandle, STRING_TOKEN(STR_HOTKEY_INFO3), NULL);
      StrList[3] = HiiGetString(gHiiHandle, STRING_TOKEN(STR_HOTKEY_INFO4), NULL);
      StrList[4] = HiiGetString(gHiiHandle, STRING_TOKEN(STR_HOTKEY_INFO5), NULL);
      StrList[5] = HiiGetString(gHiiHandle, STRING_TOKEN(STR_HOTKEY_INFO6), NULL);
      StrList[6] = HiiGetString(gHiiHandle, STRING_TOKEN(STR_HOTKEY_INFO7), NULL);

      ConOut->SetAttribute(ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      ConOut->OutputString(ConOut, StrList[0]);
      ConOut->SetAttribute(ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
      ConOut->OutputString(ConOut, StrList[1]);
      ConOut->SetAttribute(ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      ConOut->OutputString(ConOut, StrList[2]);
      ConOut->SetAttribute(ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
      ConOut->OutputString(ConOut, StrList[3]);
      ConOut->SetAttribute(ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      ConOut->OutputString(ConOut, StrList[4]);
      ConOut->SetAttribute(ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
      ConOut->OutputString(ConOut, StrList[5]);
      ConOut->SetAttribute(ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
      ConOut->OutputString(ConOut, StrList[6]);
      for(Index=0;Index<ARRAY_SIZE(StrList);Index++){
        gBS->FreePool(StrList[Index]);
      }

    }else if(KeyData->Key.UnicodeChar == 1){
      Size = (Columns+1) * sizeof(CHAR16);
      p = AllocatePool(Size);
      ASSERT(p != NULL);
      SetMem16(p, Size, L' ');
      p[Columns] = 0;
      ConOut->OutputString(ConOut, p);
      FreePool(p);
    }
  }

  ConOut->SetAttribute(ConOut, Attribute);
  ConOut->SetCursorPosition(ConOut, x, y);
}



POST_HOT_KEY_CTX gPlatPostHotKeyCtx[] = {

  {
    {SCAN_NULL, CHAR_NULL},
    NULL,
    HOTKEY_ATTRIBUTE_NO_TIMEOUT_PROMPT,
    HOTKEY_BOOT_NONE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },

  {
    {SCAN_DELETE, CHAR_NULL},
    NULL,
    HOTKEY_ATTRIBUTE_OEM_DRAW,
    HOTKEY_BOOT_SETUP,
    NULL,
    PcdGetPtr (PcdBootManagerMenuFile),
    BootSetupHotKeyHandler,
    L"",                       // fill it later for multi-language support
    L"Setup"
  },

  {
    {SCAN_F11, CHAR_NULL},
    NULL,
    HOTKEY_ATTRIBUTE_NO_PROMPT,
    HOTKEY_BOOT_MENU,
    NULL,
    &gBootMenuFileGuid,
    NULL,
    L"",                      // fill it later for multi-language support
    L"BootMenu"
  },

  {
    {SCAN_F12, CHAR_NULL},
    NULL,
    HOTKEY_ATTRIBUTE_NO_PROMPT,
    HOTKEY_BOOT_PXE,
    NULL,
    &gPxeBootAppGuid,
    NULL,
    L"",                      // fill it later for multi-language support
    L"PxeMenu"
  },

  {
    {SCAN_NULL, CHAR_CARRIAGE_RETURN},
    NULL,
    0,
    HOTKEY_BOOT_PASS,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
  },

#if FixedPcdGetBool(PcdHotKeyF8ShellSupport)
  {
    {SCAN_F8, CHAR_NULL},
    NULL,
    HOTKEY_ATTRIBUTE_NO_PROMPT,
    HOTKEY_BOOT_OEM1,
    NULL,
    &gUefiShellFileGuid,
    NULL,
    L"",                      // fill it later for multi-language support
    L"Shell"
  },
#endif
    
};

UINTN gPlatPostHotKeyCtxCount = sizeof(gPlatPostHotKeyCtx)/sizeof(gPlatPostHotKeyCtx[0]);

