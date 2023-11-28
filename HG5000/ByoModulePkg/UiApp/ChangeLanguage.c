/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
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
#include <Guid/MdeModuleHii.h>
#include <Protocol/UnicodeCollation.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>
#include <Guid/GlobalVariable.h>
#include <Library/PlatformLanguageLib.h>
#include <ByoPlatformSetupConfig.h>
#include <Protocol/ByoFormSetManager.h>
#include "ChangeLanguage.h"
#include <Protocol/ByoPlatformLanguageProtocol.h>
#include <Library/ByoCommLib.h>
#include <Token.h>


VOID LibGetSetPlatformLanguage(BOOLEAN Get, CHAR8 **Lang);

EFI_STRING_ID    *gLanguageToken = NULL;
EFI_HII_HANDLE   gCurrentFormsetHiiHandle = NULL;

INTN
EFIAPI
StrCmpChar8 (
  IN      CONST CHAR8               *FirstString,
  IN      CONST CHAR8               *SecondString
  )
{

  while ((*FirstString != '\0') && (*FirstString == *SecondString)) {
    FirstString++;
    SecondString++;
  }

  return *FirstString - *SecondString;
}

EFI_GUID gLangValueVariableGuid = LANGUAGE_VALUE_VARIABLE_GUID;
/**
  Create language select item on a Formset.

  @param HiiHandle         The Hii Handle of Formset
  @param FormSetGuid    The Guid of Formset
  
**/
EFI_STATUS
UpdateLanguageSettingItem (
  EFI_HII_HANDLE    HiiHandle,
  EFI_GUID          *FormSetGuid,
  EFI_FORM_ID       FormId,
  EFI_STRING_ID     ItemPrompt,
  EFI_STRING_ID     ItemHelp
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
  BOOLEAN                     IsDefault;
  CHAR8                       *DefPlatformLang;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
  
  if (NULL == HiiHandle || NULL == FormSetGuid) {
    return EFI_INVALID_PARAMETER;
  }

  gCurrentFormsetHiiHandle = HiiHandle;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  OptionsOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (OptionsOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_CHANGE_LANGUAGE;
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_CHANGE_LANGUAGE_END;

  LanguageString = HiiGetSupportedLanguages (HiiHandle);
  ASSERT (LanguageString != NULL);
  DEBUG((EFI_D_INFO, "LanguageString:%a\n", LanguageString));

  Lang = AllocatePool(AsciiStrSize(LanguageString));
  ASSERT (Lang != NULL);

  DefPlatformLang = (CHAR8*)PcdGetPtr(PcdUefiVariableDefaultPlatformLang);

//CurrentLang = GetEfiGlobalVariable(L"PlatformLang");
  LibGetSetPlatformLanguage(TRUE, &CurrentLang);
  DEBUG((EFI_D_INFO, "CurrentLang:%a\n", CurrentLang));

  BestLanguage = GetBestLanguage (
                   LanguageString,
                   FALSE,
                   (CurrentLang != NULL) ? CurrentLang : "",
                   DefPlatformLang,
                   LanguageString,
                   NULL
                   );
  DEBUG((EFI_D_INFO, "BestLanguage:%a\n", BestLanguage));

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
  //Greate Best Language Option.
  //
  OptionCount = 0;
  LangCode = LanguageString;
  while (*LangCode != 0) {
    GetNextLanguage (&LangCode, Lang);	
    if (StrCmpChar8 (Lang, "uqi") == 0) {
      continue;
    }
    if (FirstFlag) {
      StringBuffer = HiiGetString (HiiHandle, LANGUAGE_NAME_STRING_ID, Lang);
      ASSERT (StringBuffer != NULL);
      gLanguageToken[OptionCount] = HiiSetString (HiiHandle, 0, StringBuffer, NULL);
      FreePool (StringBuffer);
    }

    if(AsciiStrCmp(Lang, DefPlatformLang) == 0){
      DEBUG((EFI_D_INFO, "Default:%d\n", OptionCount));
      IsDefault = TRUE;
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
    DEBUG((EFI_D_INFO, "OptionCount:%d, IsDefault:%d, Lang:%a\n", OptionCount, IsDefault, Lang));

    if (StrCmpChar8 (Lang, BestLanguage) == 0) {
      gRT->SetVariable(
               L"LangValue",
               &gLangValueVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS,
               sizeof(LANGUAGE_VALUE),
               &OptionCount
               );
      DEBUG((EFI_D_INFO, "LangValue:%d\n", OptionCount));
    }
    OptionCount++;
  }

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
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "HiiUpdateForm:%r\n", Status));
  }
  
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);

  return Status;
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
//EFI_STATUS                    Status;
  CHAR8                         *PlatformSupportedLanguages;
  CHAR8                         *BestLanguage;
  CHAR8                         *PlatLangCodes;


  DEBUG((EFI_D_INFO, "%a A:%X Q:%X\n", __FUNCTION__, Action, QuestionId));
  
  if (NULL == gCurrentFormsetHiiHandle) {
    return EFI_NOT_READY;
  }
  
  if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    if (QuestionId == KEY_CHANGE_LANGUAGE) {
      return EFI_UNSUPPORTED;
    }
  }

  if (Action != EFI_BROWSER_ACTION_CHANGING) {
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
        if (StrCmpChar8 (Lang, "uqi") == 0) {
         continue;
       }
        if (Index == Value->u8) {
          break;
        }
        Index++;
      }

      DEBUG((EFI_D_INFO, "Lang:%a\n", Lang));
      DEBUG((EFI_D_INFO, "LanguageString:%a\n", LanguageString));      

      PlatformSupportedLanguages = GetEfiGlobalVariable (L"PlatformLangCodes");
      if (PlatformSupportedLanguages == NULL) {
        PlatLangCodes = (CHAR8*)PcdGetPtr(PcdUefiVariableDefaultPlatformLangCodes);
        PlatformSupportedLanguages = AllocateCopyPool(AsciiStrSize(PlatLangCodes), PlatLangCodes);
        ASSERT (PlatformSupportedLanguages != NULL);
      }

      DEBUG((EFI_D_INFO, "PlatformLangCodes:%a\n", PlatformSupportedLanguages));
      BestLanguage = GetBestLanguage (
                       PlatformSupportedLanguages,
                       FALSE,
                       Lang,
                       NULL
                       );
      DEBUG((EFI_D_INFO, "GetBestLanguage:%a\n", BestLanguage));
      if (BestLanguage != NULL) {
//        Status = gRT->SetVariable (
//                        L"PlatformLang",
//                        &gEfiGlobalVariableGuid,
//                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
//                        AsciiStrSize (BestLanguage),
//                        BestLanguage
//                        );
        LibGetSetPlatformLanguage(FALSE, &BestLanguage);

//        DMSG ((EFI_D_ERROR, "SetLanguageCallback, SetVariable PlatformLang :%r. \n", Status));
//        ASSERT_EFI_ERROR(Status);
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



VOID SavePlatformLanguage()
{
  EFI_STATUS                      Status;
  BYO_PLATFORM_LANGUAGE_PROTOCOL  *PlatLang;

  Status = gBS->LocateProtocol(&gByoPlatformLanguageProtocolGuid, NULL, (VOID**)&PlatLang);
  if(EFI_ERROR(Status)){
    return;
  }

  DEBUG((EFI_D_INFO, "%a %a\n", __FUNCTION__, PlatLang->PlatformLang));
  
  Status = gRT->SetVariable (
                  L"PlatformLang",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  AsciiStrSize(PlatLang->PlatformLang),
                  PlatLang->PlatformLang
                  );  
  ASSERT(!EFI_ERROR(Status));
}


VOID PlatformLanguageLoadDefault()
{
  EFI_STATUS                      Status;
  BYO_PLATFORM_LANGUAGE_PROTOCOL  *PlatLang;
  CHAR8                           *DefLang;

  Status = gBS->LocateProtocol(&gByoPlatformLanguageProtocolGuid, NULL, (VOID**)&PlatLang);
  if(EFI_ERROR(Status)){
    return;
  }

  DefLang = (CHAR8*)PcdGetPtr(PcdUefiVariableDefaultPlatformLang);

  if(AsciiStrCmp(PlatLang->PlatformLang, DefLang) != 0){
    AsciiStrCpy(PlatLang->PlatformLang, DefLang);
    PlatLang->LangChanged = TRUE;
  }
}


