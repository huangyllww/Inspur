/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
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
#include <SystemPasswordVariable.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/ByoSmiFlashProtocol.h>


VOID
BootSetupHotKeyHandler (
    IN CONST EFI_KEY_DATA  *KeyData
  )
{
  UINTN                             Index;
  CHAR16                            *StrList[5];
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  UINTN                             Columns;
  UINTN                             Rows;
  UINTN                             x, y;
  UINTN                             Attribute;
  CHAR16                            *p;
  UINTN                             Size;
  UINTN                             Size1;
  SETUP_DATA                        SetupData;
  EFI_STATUS                        Status = EFI_SUCCESS;
  

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if(KeyData->Key.ScanCode == 0xFFFF){

    ConOut = gST->ConOut;
    ConOut->QueryMode(ConOut, ConOut->Mode->Mode, &Columns, &Rows);
    x = ConOut->Mode->CursorColumn;
    y = ConOut->Mode->CursorRow;
    Attribute = ConOut->Mode->Attribute;
    ConOut->SetCursorPosition(ConOut, 2, Rows-3);
    DEBUG((EFI_D_INFO, "%d x %d (%d, %d)\n", Columns, Rows, x, y));

    if(KeyData->Key.UnicodeChar == 0){
      Size1 = sizeof (SETUP_DATA);
      Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size1,
                  &SetupData
                  ); 
    DEBUG((EFI_D_INFO,"Jim Setup.Quiteboot %d \n",SetupData.QuiteBoot));
    #if BC_TENCENT
     if(SetupData.QuiteBoot == 0){
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
     }
     #else
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
      #endif
      
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


VOID
HiddenSetupHotKeyHandler (
    IN CONST EFI_KEY_DATA  *KeyData
  )
{
  SYSTEM_PASSWORD                   PasswordVariable;
  UINTN                             VarSize;
  EFI_STATUS                        Status = EFI_SUCCESS;
  BYO_SMIFLASH_PROTOCOL             *ByoSmiFlash;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  //
  // Set Enter type.
  //
  VarSize = sizeof (SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VarSize,
                  &PasswordVariable
                  );
  if (!EFI_ERROR(Status)) {    
    PasswordVariable.HiddenEnteredType = 0x5A;
    Status = gRT->SetVariable (
                    SYSTEM_PASSWORD_NAME,
                    &gEfiSystemPasswordVariableGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VarSize,
                    &PasswordVariable
                    );
  
       gBS->LocateProtocol (
              &gByoSmiFlashProtocolGuid,
              NULL,
              (VOID**)&ByoSmiFlash
              );
       ByoSmiFlash->SaveSetupPassword(ByoSmiFlash);
  }

  if(KeyData->Key.ScanCode == SCAN_F2){
   DEBUG((EFI_D_INFO, "HiddenFlag:0x00 \n"));
  }else{
   DEBUG((EFI_D_INFO, "HiddenFlag:0x01 \n"));
  }
}


POST_HOT_KEY_CTX gPlatPostHotKeyCtx[] = {

  {
    {SCAN_DELETE, CHAR_NULL},
    NULL,
    HOTKEY_ATTRIBUTE_OEM_DRAW,
    HOTKEY_BOOT_SETUP,
    NULL,
    &gSetupFileGuid,
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
    HOTKEY_BOOT_OEM1,
    NULL,
    &gPxeBootAppGuid,
    NULL,
    L"",                      // fill it later for multi-language support
    L"PxeBoot"
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
};

UINTN gPlatPostHotKeyCtxCount = sizeof(gPlatPostHotKeyCtx)/sizeof(gPlatPostHotKeyCtx[0]);

