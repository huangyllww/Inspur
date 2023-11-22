/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PerformanceLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/DevicePathLib.h>
#include <Library/SetupUiLib.h>
#include <Guid/MdeModuleHii.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>
#include <Protocol/FileExplorer.h>
#include <Protocol/ScreenshotFolderPathProtocol.h>


#include <Guid/GlobalVariable.h>
#include <Library/PlatformLanguageLib.h>
#include <ByoPlatformSetupConfig.h>
#include <Protocol/ByoFormSetManager.h>
#include "ChangeLanguage.h"
#include <Protocol/ByoPlatformLanguageProtocol.h>


EFI_STRING_ID    *gLanguageToken = NULL;
EFI_HII_HANDLE   gCurrentFormsetHiiHandle = NULL;

UINTN                       mDefaultOptionCount = 0xFF;
UINTN                       mLanguageOptionCount = 0;

/**
  Create language select item on a Formset.

  @param HiiHandle         The Hii Handle of Formset
  @param FormSetGuid    The Guid of Formset
  
**/
EFI_STATUS
UpdateLanguageSettingItem (
  EFI_HII_HANDLE    HiiHandle,
  EFI_GUID    *FormSetGuid,
  EFI_FORM_ID     FormId,
  EFI_STRING_ID    ItemPrompt,
  EFI_STRING_ID    ItemHelp
  )
{
  EFI_STATUS                  Status;
  CHAR8                       *LanguageString;
  CHAR8                       *LangCode;
  CHAR8                       *Lang;
  CHAR8                       *CurrentLang;
  CHAR8                       *BestLanguage;
  UINTN                       OptionCount;
  CHAR16                      *StringBuffer;
  VOID                        *OptionsOpCodeHandle;
  VOID                        *StartOpCodeHandle;
  VOID                        *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL          *StartLabel;
  EFI_IFR_GUID_LABEL          *EndLabel;
  BOOLEAN                     FirstFlag;  
  EFI_GUID    LangValueVariableGuid = LANGUAGE_VALUE_VARIABLE_GUID;
  BOOLEAN                     IsDefault;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
  if (NULL == HiiHandle || NULL == FormSetGuid) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Init OpCode Handle and Allocate space for creation of UpdateData Buffer
  //
  gCurrentFormsetHiiHandle = HiiHandle;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  OptionsOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (OptionsOpCodeHandle != NULL);
  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_CHANGE_LANGUAGE;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_END;

  //
  // Collect the languages from what our current Language support is based on our VFR
  //
  LanguageString = HiiGetSupportedLanguages (HiiHandle);
  ASSERT (LanguageString != NULL);
  DMSG((EFI_D_ERROR, "\n UpdateLanguageSettingItem, LanguageString :%a.\n", LanguageString));

  //
  // Allocate working buffer for RFC 4646 language in supported LanguageString.
  //
  Lang = AllocatePool (AsciiStrSize (LanguageString));
  ASSERT (Lang != NULL);

//CurrentLang = GetEfiGlobalVariable(L"PlatformLang");
  LibGetSetPlatformLanguage(TRUE, &CurrentLang);
  DMSG((EFI_D_ERROR, "UpdateLanguageSettingItem, CurrentLang :%a.\n", CurrentLang));
  //
  // Select the best language in LanguageString as the default one.
  //
  BestLanguage = GetBestLanguage (
                   LanguageString,
                   FALSE,
                   (CurrentLang != NULL) ? CurrentLang : "",
                   (CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang),
                   LanguageString,
                   NULL
                   );
  DMSG((EFI_D_ERROR, "UpdateLanguageSettingItem, BestLanguage :%a.\n", BestLanguage));
  //
  // BestLanguage must be selected as it is the first language in LanguageString by default
  //
  ASSERT (BestLanguage != NULL);
  OptionCount = 0;
  LangCode    = LanguageString;
  FirstFlag   = FALSE;

 if (NULL == gLanguageToken) {
    while (*LangCode != 0) {
      GetNextLanguage (&LangCode, Lang);
      OptionCount ++;
    }
    gLanguageToken = AllocatePool (OptionCount * sizeof (EFI_STRING_ID));
    ASSERT (gLanguageToken != NULL);
    FirstFlag = TRUE; 
  }

  //
  //Create Best Language Option.
  //
  OptionCount = 0;
  LangCode = LanguageString;
  while (*LangCode != 0) {
    GetNextLanguage (&LangCode, Lang);	
    if (AsciiStrCmp (Lang, "uqi") == 0) {
      continue;
    }
    if (FirstFlag) {
      StringBuffer = HiiGetString (HiiHandle, LANGUAGE_NAME_STRING_ID, Lang);

      DMSG((EFI_D_ERROR, "UpdateLanguageSettingItem, Lang :%a-%s.\n", Lang, StringBuffer));
      ASSERT (StringBuffer != NULL);
      //
      // Save the string Id for each language
      //
      gLanguageToken[OptionCount] = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
      if (AsciiStrCmp (Lang, "zh-Hans") == 0) {
        HiiSetString (HiiHandle, gLanguageToken[OptionCount], L"Chinese", "en-US");
      }
      FreePool (StringBuffer);
    }

    if(AsciiStrCmp(Lang, (CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLang)) == 0){
      DEBUG((EFI_D_INFO, "Default:%d\n", OptionCount));
      IsDefault = TRUE;
      mDefaultOptionCount = OptionCount;
    } else {
      IsDefault = FALSE;
    }
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      gLanguageToken[OptionCount],
      IsDefault?EFI_IFR_OPTION_DEFAULT:0,
      EFI_IFR_NUMERIC_SIZE_1,
      (UINT8) OptionCount
      );

    if (AsciiStrCmp (Lang, BestLanguage) == 0) {
      gRT->SetVariable(
               L"LangValue",
               &LangValueVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS,
               sizeof(LANGUAGE_VALUE),
               &OptionCount
               );
      DEBUG((EFI_D_INFO, "LangValue:%d\n", OptionCount));
    }
    OptionCount++;
  }

  mLanguageOptionCount = OptionCount;

//  if (CurrentLang != NULL) {
//    FreePool (CurrentLang);
//  }
  FreePool (BestLanguage);
  FreePool (Lang);
  FreePool (LanguageString);

  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    KEY_CHANGE_LANGUAGE,
    KEY_CHANGE_LANGUAGE,
    0,
    ItemPrompt,
    ItemHelp,
    EFI_IFR_FLAG_CALLBACK,
    EFI_IFR_NUMERIC_SIZE_1,
    OptionsOpCodeHandle,
    NULL
    );

  Status = HiiUpdateForm (
             HiiHandle,
             FormSetGuid,
             FormId,
             StartOpCodeHandle,
             EndOpCodeHandle
             );
  DMSG((EFI_D_ERROR, "UpdateLanguageSettingItem, HiiUpdateForm :%r.\n", Status));

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);

  return Status;
}

EFI_STATUS
UpdateChooseFolderPathSettingItem (
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  EFI_GUID          *FormSetGuid,
  IN  EFI_FORM_ID       FormId,
  IN  EFI_STRING_ID     ItemPrompt,
  IN  EFI_STRING_ID     ItemHelp
  )
{
  EFI_STATUS                  Status;
  VOID                        *ActionOpCodeHandle;
  VOID                        *StartOpCodeHandle;
  VOID                        *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL          *StartLabel;
  EFI_IFR_GUID_LABEL          *EndLabel;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (NULL == HiiHandle || NULL == FormSetGuid) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Init OpCode Handle and Allocate space for creation of UpdateData Buffer
  //
  gCurrentFormsetHiiHandle = HiiHandle;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  ActionOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (ActionOpCodeHandle != NULL);

  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_CHOOSE_FOLDER;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_END;

  //
  //add action for choose devicepath
  //
  HiiCreateActionOpCode (
    StartOpCodeHandle,
    KEY_CHOOSE_FOLDER,
    ItemPrompt,
    ItemHelp,
    EFI_IFR_FLAG_CALLBACK,
    0
    );

  Status = HiiUpdateForm (
             HiiHandle,
             FormSetGuid,
             FormId,
             StartOpCodeHandle,
             EndOpCodeHandle
             );
  DMSG((EFI_D_ERROR, "UpdateChooseFolderSettingItem, HiiUpdateForm :%r.\n", Status));

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
  HiiFreeOpCodeHandle (ActionOpCodeHandle);

  return Status;
}

BOOLEAN
EFIAPI
IsFileSystemWritable (
  IN EFI_DEVICE_PATH_PROTOCOL  *FilePath
  )
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        FileHandle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL   *Fs;
  EFI_FILE_PROTOCOL                 *Root = NULL;
  EFI_FILE_PROTOCOL                 *File = NULL;
  CHAR16                            *StrFileSystemUnwritable;
  BOOLEAN                           IsQuit;

  Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &FilePath, &FileHandle);
  if (EFI_ERROR (Status)) {
    DMSG ((EFI_D_ERROR, "LocateDevicePath failed ,status is:%r\n", Status));
    IsQuit = FALSE;
    goto Exit;
  }

  Status = gBS->HandleProtocol (
                 FileHandle,
                 &gEfiSimpleFileSystemProtocolGuid,
                 (VOID**)&Fs
                 );
  ASSERT (!EFI_ERROR(Status));

  Status = Fs->OpenVolume (Fs, &Root);
  if (EFI_ERROR(Status)) {
    DMSG ((EFI_D_ERROR, "Fs->OpenVolume Error:%r\n", Status));
    IsQuit = FALSE;
    goto Exit;
  }

  //
  // Try opening a file for writing
  //
  Status = Root->Open(Root, &File, L"test.png", EFI_FILE_MODE_CREATE | EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE, 0);
  if (EFI_ERROR (Status)) {
    DMSG((EFI_D_ERROR, "FindWritableFs: Root->Open returned %r\n", Status));
    IsQuit = FALSE;
    goto Exit;
  }

  Root->Delete(File);
  IsQuit = TRUE;

Exit:
  if (IsQuit == FALSE) {
    StrFileSystemUnwritable = HiiGetString (gHandle,  STRING_TOKEN (STR_FOLDER_UNWRITEABLE), NULL);
    UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, StrFileSystemUnwritable, NULL);
  }
  return IsQuit;
}

EFI_STATUS
SetFolderPathCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId
  )
{
  EFI_STATUS                     Status;
  EFI_FILE_EXPLORER_PROTOCOL     *FileExplorer;
  EFI_DEVICE_PATH_PROTOCOL       *FileDp = NULL;
  SCREENSHOT_PROTOCOL            *ChooseFolderPath;

  if (NULL == gCurrentFormsetHiiHandle) {
    return EFI_NOT_READY;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  if (QuestionId != KEY_CHOOSE_FOLDER) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (&gEfiFileExplorerProtocolGuid, NULL, (void**)&FileExplorer);
  if(EFI_ERROR (Status)) {
    goto ProcExit;
  }

  Status = FileExplorer->ChooseFile (NULL, L"", IsFileSystemWritable, &FileDp);
  if(EFI_ERROR (Status)) {
    goto ProcExit;
  }

  Status = gBS->LocateProtocol(&gScreenshotProtocolGuid, NULL, (VOID **)&ChooseFolderPath); 
  if (!EFI_ERROR (Status)) {
    Status = ChooseFolderPath->SetFolder (
                                ChooseFolderPath,
                                FileDp
                                );
    DMSG((EFI_D_INFO, "Set Protocol Status is %r\n", Status));
  }

ProcExit:
  if (FileDp != NULL) {
    FreePool (FileDp);
  }

  DMSG((EFI_D_INFO, "%a %r\n", __FUNCTION__, Status));
  return EFI_SUCCESS;
}

/**
  The Callback Function of Language Select.
  
**/
EFI_STATUS
SetLanguageCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  CHAR8                         *LanguageString;
  CHAR8                         *LangCode;
  CHAR8                         *Lang;
  UINTN                         Index;
  EFI_STATUS                    Status;
  CHAR8                         *PlatformSupportedLanguages;
  CHAR8                         *BestLanguage;
  EFI_STRING                    StringBuffer;

  DMSG ((EFI_D_ERROR, "\n SetLanguageCallback, QuestionId :0x%x. \n", QuestionId));
  if (NULL == gCurrentFormsetHiiHandle) {
    return EFI_NOT_READY;
  }

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN && QuestionId == KEY_CHANGE_LANGUAGE) {
    //
    // Set Chinese as Chinese for English Language
    //
    for (Index = 0; Index < mLanguageOptionCount; Index ++) {
      StringBuffer = HiiGetString (gCurrentFormsetHiiHandle, gLanguageToken[Index], "en-US");
      if (StringBuffer == NULL) {
        continue;
      }
      if (StrCmp (StringBuffer, L"Chinese") == 0) {
        FreePool (StringBuffer);
        StringBuffer = HiiGetString (gCurrentFormsetHiiHandle, gLanguageToken[Index], "zh-Hans");
        if (StringBuffer == NULL) {
          break;
        }
        HiiSetString (gCurrentFormsetHiiHandle, gLanguageToken[Index], StringBuffer, "en-US");
        FreePool (StringBuffer);
        break;
      }
      FreePool (StringBuffer);
    }
    return EFI_SUCCESS;
  }

  if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    if (QuestionId == KEY_CHANGE_LANGUAGE) {
      //
      // Set the default language
      //
      if(PcdGetBool(PcdKeepCurrentLanagugeAsDefault)==TRUE) {
        return EFI_SUCCESS;
      }
      Value->u8 = (UINT8) mDefaultOptionCount;
    } else {
      return EFI_UNSUPPORTED;
    }
  } else if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_SUCCESS;
  }

  switch (QuestionId) {
    case KEY_CHANGE_LANGUAGE:
      //
      // Collect the languages from what our current Language support is based on our VFR
      //
      LanguageString = HiiGetSupportedLanguages (gCurrentFormsetHiiHandle);
      ASSERT (LanguageString != NULL);
      //
      // Allocate working buffer for RFC 4646 language in supported LanguageString.
      //	  
      Lang = AllocatePool (AsciiStrSize (LanguageString));
      ASSERT (Lang != NULL);
      Index = 0;
      LangCode = LanguageString;
      while (*LangCode != 0) {
        GetNextLanguage (&LangCode, Lang);
        if (AsciiStrCmp (Lang, "uqi") == 0) {
         continue;
       }
        if (Index == Value->u8) {
          break;
        }
        Index++;
      }

      PlatformSupportedLanguages = NULL;
      GetEfiGlobalVariable2 (L"PlatformLangCodes", (VOID**)&PlatformSupportedLanguages, NULL);
      if (PlatformSupportedLanguages == NULL) {
        PlatformSupportedLanguages = AllocateCopyPool (
                                       AsciiStrSize ((CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLangCodes)),
                                       (CHAR8 *) PcdGetPtr (PcdUefiVariableDefaultPlatformLangCodes)
                                       );
        ASSERT (PlatformSupportedLanguages != NULL);
      }
      //
      // Select the best language in platform supported Language.
      //
      BestLanguage = GetBestLanguage (
                       PlatformSupportedLanguages,
                       FALSE,
                       Lang,
                       NULL
                       );
      DMSG ((EFI_D_ERROR, "SetLanguageCallback, BestLanguage :%a. \n", BestLanguage));
      if (BestLanguage != NULL) {
        Status = gRT->SetVariable (
                        L"PlatformLang",
                        &gEfiGlobalVariableGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        AsciiStrSize (BestLanguage),
                        BestLanguage
                        );
        LibGetSetPlatformLanguage(FALSE, &BestLanguage);

        DMSG ((EFI_D_ERROR, "SetLanguageCallback, SetVariable PlatformLang :%r. \n", Status));
        ASSERT_EFI_ERROR(Status);
        FreePool (BestLanguage);
      } else {
        ASSERT (FALSE);
      }

      FreePool (PlatformSupportedLanguages);
      FreePool (Lang);
      FreePool (LanguageString);
      break;
	  
    default:
      break;
  }

  return EFI_SUCCESS;
}

