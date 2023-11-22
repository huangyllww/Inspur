/** @file
Entry and initialization module for the browser.

Copyright (c) 2007 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "Setup.h"
#include <Library/ByoCommLib.h>
#include "SetupBrowser.h"

SETUP_DRIVER_PRIVATE_DATA  mPrivateData = {
  SETUP_DRIVER_SIGNATURE,
  NULL,
  {
    SendForm,
    BrowserCallback
  },
  {
    SetScope,
    RegisterHotKey,
    RegiserExitHandler,
    SaveReminder,
    RegisterHotkeyFunction
  },
  {
    BROWSER_EXTENSION2_VERSION_1_1,
    SetScope,
    RegisterHotKey,
    RegiserExitHandler,
    IsBrowserDataModified,
    ExecuteAction,
    {NULL,NULL},
    {NULL,NULL},
    IsResetRequired,
    RegisterHotkeyFunction
  },
  {
    IsBrowserDataModified,
    ExecuteAction,
    FakeOemPlatformReset,
    TriggerSetupNotify,
    ReterieveSetupOptions,
    ReterieveSetupOptionsName
  }
};

EFI_HII_DATABASE_PROTOCOL         *mHiiDatabase;
EFI_HII_CONFIG_ROUTING_PROTOCOL   *mHiiConfigRouting;
EFI_DEVICE_PATH_FROM_TEXT_PROTOCOL *mPathFromText;
EDKII_FORM_DISPLAY_ENGINE_PROTOCOL *mFormDisplay;
BYO_CUSTOM_STR_PROTOCOL            *mCustomString;

UINTN           gBrowserContextCount = 0;
LIST_ENTRY      gBrowserContextList = INITIALIZE_LIST_HEAD_VARIABLE (gBrowserContextList);
LIST_ENTRY      gBrowserFormSetList = INITIALIZE_LIST_HEAD_VARIABLE (gBrowserFormSetList);
LIST_ENTRY      gBrowserHotKeyList  = INITIALIZE_LIST_HEAD_VARIABLE (gBrowserHotKeyList);
LIST_ENTRY      gBrowserStorageList = INITIALIZE_LIST_HEAD_VARIABLE (gBrowserStorageList);
LIST_ENTRY      gBrowserSaveFailFormSetList = INITIALIZE_LIST_HEAD_VARIABLE (gBrowserSaveFailFormSetList);

BOOLEAN               mRunByoSetup  = FALSE;
BOOLEAN               mSystemSubmit = FALSE;
BOOLEAN               gResetRequired;
BOOLEAN               gExitRequired;
BOOLEAN               gFlagReconnect;
BOOLEAN               gCallbackReconnect;
BROWSER_SETTING_SCOPE gBrowserSettingScope = SystemLevel;
BOOLEAN               mBrowserScopeFirstSet = TRUE;
EXIT_HANDLER          ExitHandlerFunction = NULL;
FORM_BROWSER_FORMSET  *mSystemLevelFormSet;

//
// Browser Global Strings
//
CHAR16            *gEmptyString;
CHAR16            *mUnknownString = L"!";

extern EFI_GUID        mCurrentFormSetGuid;
extern EFI_HII_HANDLE  mCurrentHiiHandle;
extern UINT16          mCurrentFormId;
extern FORM_DISPLAY_ENGINE_FORM gDisplayFormData;

EFI_BYO_FORMSET_MANAGER_PROTOCOL    *gByoFormsetManager = NULL;

EFI_HII_HANDLE mSetupBrowserHandle = NULL;
BOOLEAN    mBeLoadAllHiiFormset = FALSE;
UINTN    mNumOfDisplayFormset = 0;
EFI_GUID *mDisplayFormsetGuidList = NULL;
EFI_EVENT     mTimerEvent;
static UINT8  mCount = 0;
BOOLEAN       mRecoveryDefault = FALSE;
extern UINT8                         SetupBrowserBin[];
UINTN                                mSearchCount = 0;
SEARCH_OPTION_DATA	                 *mSearchformData = NULL;
EFI_GUID                             gEfiSetupSearchGuid = SETUP_SEARCH_GUID;
EFI_GUID                             gByoSetupSearchFormGuid = FORMSET_GUID_SEARCH;
BOOLEAN                              mUpdateSearchTitle = FALSE;
#define                              MAXSUPPORTSEARCHCOUNT   1000
EFI_GUID                             gFileExplorerFormGuid =  {0xfe561596, 0xe6bf, 0x41a6, {0x83, 0x76, 0xc7, 0x2b, 0x71, 0x98, 0x74, 0xd0}};

EFI_GUID *mRootFormsetGuidList[] = {
  &gEfiFormsetGuidMain,
  &gEfiFormsetGuidAdvance,
  &gEfiFormsetGuidDevices,
  &gEfiFormsetGuidBoot,
  &gEfiFormsetGuidSecurity,
  &gEfiFormsetGuidPower,
  &gEfiFormsetGuidExit
};

SEARCH_PRIVATE_DATA gPrivateData = {
  SETUP_BROESER_SIGNATURE,
  NULL,
  NULL,
  {
    SearchOptionExtractConfig,
    SearchOptionRouteConfig,
    SearchOptionCallBack
  }
};

HII_VENDOR_DEVICE_PATH1 mSetupSearchVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    FORMSET_GUID_SEARCH
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

/**
  Converts the unicode character of the string from uppercase to lowercase.
  This is a internal function.

  @param[in out] ConfigString  String to be converted

**/
VOID
LibToLowerString (
  IN OUT CHAR16  *String
  )
{
  CHAR16      *TmpStr;

  for (TmpStr = String; *TmpStr != L'\0'; TmpStr++) {
    if (*TmpStr >= L'A' && *TmpStr <= L'Z') {
      *TmpStr = (CHAR16) (*TmpStr - L'A' + L'a');
    }
  }
}

/**
  Function to check if the first character of the InputString matches the first
  character of StringToMatch and if InputString starts with StringToMatch.

  @param[in] InputString     A pointer to a Null-terminated Unicode string.
  @param[in] StringToMatch   A pointer to a Null-terminated Unicode string to search for.
 
  @retval NULL              If StringToMatch doesn't match the first character of the InputString.
  @return others            If there is a match.
**/
CHAR16 *
StrStartsWithFirstChar(
  IN CONST CHAR16 *InputString,
  IN CONST CHAR16 *StringToMatch
  )
{
  if (InputString == NULL || StringToMatch == NULL) {
    return NULL;
  }

  CHAR16 InputFirstChar = InputString[0];
  UINTN MatchLength = StrLen(StringToMatch);

  if (StrLen(InputString) < MatchLength) {
    return NULL;
  }

  if (InputFirstChar != StringToMatch[0]) {
    return NULL;
  }

  if (StrnCmp(InputString, StringToMatch, MatchLength) == 0) {
    return (CHAR16 *)InputString;
  }

  return NULL;
}

/**
  Function to check if the first character of the InputString matches the first
  character of StringToMatch and if InputString starts with StringToMatch.

  @param[in] InputString     A pointer to a Null-terminated Unicode string.
  @param[in] StringToMatch   A pointer to a Null-terminated Unicode string to search for.
 
  @retval NULL              If StringToMatch doesn't match the end character of the InputString.
  @return others            If there is a match.
**/
CHAR16 *
StrEndsWithSubstring(
  IN CONST CHAR16 *InputString,
  IN CONST CHAR16 *Substring
  )
{
  if (InputString == NULL || Substring == NULL) {
    return NULL;
  }

  UINTN InputLength = StrLen(InputString);
  UINTN SubstringLength = StrLen(Substring);

  if (InputLength < SubstringLength) {
    return NULL;
  }

  CHAR16 LastChar = InputString[InputLength - 1];
  CHAR16 SubstringLastChar = Substring[SubstringLength - 1];

  if (LastChar != SubstringLastChar) {
    return NULL;
  }

  if (StrnCmp(&InputString[InputLength - SubstringLength], Substring, SubstringLength) == 0) {
    return (CHAR16 *)InputString;
  }

  return NULL;
}

/**
  This function allows a caller to search for Setup options based on the InputSearchString.And save the searched options to mSearchformData.

  @param[in]   InputSearchString              Search string.

  @retval EFI_SUCCESS                         Successful search.
  @retval EFI_OUT_OF_RESOURCES                Not enough memory to store the results.
  @retval EFI_NOT_FOUND                       The Setup option is not matched.

**/
EFI_STATUS
SearchFormAndGetFormId(
  IN CHAR16                      *InputSearchString
 ) 
{
  LIST_ENTRY                           *LinkFormSet;
  FORM_BROWSER_FORMSET                 *FormSet;
  CHAR16                               *TargetPromptString;
  LIST_ENTRY                           *LinkForm;
  FORM_BROWSER_FORM                    *NewForm;
  LIST_ENTRY                           *LinkQuestion;
  FORM_BROWSER_STATEMENT               *Question;
  SEARCH_SAVE_DATA                      SearchData;
  EFI_STATUS                            Status;
  CHAR16                                *StrResult = NULL;
  UINTN                                 GotoOptionCount = 0;
  UINT16                                GotoFormId[200];
  BOOLEAN                               IsSearchForm = FALSE;
  UINTN                                 Index = 0;
  EXPRESS_RESULT                        ExpressResult;
  BOOLEAN                               IsRootFormSet = FALSE;
  EFI_SYSTEM_PASSWORD_PROTOCOL          *SystemPassword = NULL;
  UINT8                                 EnterType = LOGIN_USER_ADMIN;

  Status = gBS->LocateProtocol (
                &gEfiSystemPasswordProtocolGuid,
                NULL,
                (VOID**)&SystemPassword
                );
  if (!EFI_ERROR (Status)) {
    EnterType = SystemPassword->GetEnteredType();
  }
  if (InputSearchString == NULL || StrCmp (InputSearchString, L"") == 0 ) {
    return EFI_INVALID_PARAMETER;
  }

  mSearchCount = 0;
  if (mSearchformData == NULL) {
    mSearchformData = AllocatePool (sizeof (SEARCH_OPTION_DATA) * MAXSUPPORTSEARCHCOUNT);
    if (mSearchformData == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }

  Status = HiiGetBrowserData(
                          &gEfiSetupSearchGuid, 
                          EFI_SEARCH_VARIABLE_NAME, 
                          sizeof(SEARCH_SAVE_DATA), 
                          (UINT8*)&SearchData
                          );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  LibToLowerString(InputSearchString);
  //
  //Iterate through gBrowserFormSetList to get the option that matches InputSearchString
  //
  LinkFormSet = GetFirstNode (&gBrowserFormSetList);
  while (!IsNull (&gBrowserFormSetList, LinkFormSet)) {
    FormSet = FORM_BROWSER_FORMSET_FROM_LINK (LinkFormSet);
    LinkFormSet = GetNextNode (&gBrowserFormSetList, LinkFormSet);
    if (EnterType == LOGIN_USER_POP) {
      for (Index = 0; Index < ARRAY_SIZE (mRootFormsetGuidList); Index ++) {
        if (CompareMem (&FormSet->Guid, mRootFormsetGuidList[Index], sizeof (EFI_GUID)) == 0) {
          IsRootFormSet = TRUE;
          break;
        }
      }
      if (IsRootFormSet == FALSE) {
        continue;
      }
      IsRootFormSet = FALSE;
    }

    //
    //Skip the search page and FileExplorer Form.
    //
    if (FormSet == NULL || CompareGuid (&FormSet->Guid, &gEfiFormsetGuidSearch) || CompareGuid (&FormSet->Guid, &gFileExplorerFormGuid)) {
      continue;
    }

    //
    //Search for the goto option in the root Form of the Formset, and if so, save the Formid.
    //
    GotoOptionCount = 0;
    LinkForm = GetFirstNode (&FormSet->FormListHead);
    NewForm = FORM_BROWSER_FORM_FROM_LINK (LinkForm);
    LinkQuestion = GetFirstNode (&NewForm->StatementListHead);
    while (!IsNull (&NewForm->StatementListHead, LinkQuestion)) {
      Question = FORM_BROWSER_STATEMENT_FROM_LINK (LinkQuestion);
      LinkQuestion = GetNextNode (&NewForm->StatementListHead, LinkQuestion);
      if(EvaluateExpressionList(Question->Expression, TRUE, FormSet, NewForm) != ExpressFalse) {
        continue;
      }
      if ((Question->Operand == EFI_IFR_STRING_REF1_OP) || (Question->Operand == EFI_IFR_STRING_REF2_OP) || (Question->Operand == EFI_IFR_QUESTION_REF3_OP)  || (Question->Operand == EFI_IFR_REF_OP) ) {
        GotoFormId[GotoOptionCount] = ((EFI_IFR_REF *) Question->OpCode)->FormId;
        GotoOptionCount ++;
      }
    }

    //
    //Iterate over the Statements in the Form
    //
    LinkForm = GetFirstNode (&FormSet->FormListHead);
    IsSearchForm = TRUE;
    while (!IsNull (&FormSet->FormListHead, LinkForm)) {
      NewForm = FORM_BROWSER_FORM_FROM_LINK (LinkForm);
      LinkForm = GetNextNode (&FormSet->FormListHead, LinkForm);
      if (NewForm == NULL) {
        continue;
      }

      //
      //If there is no create option goto to the Form in the root Form, the Form is not searched
      //
      for (Index = 0; Index < GotoOptionCount; Index++ ) {
        if (GotoFormId[Index] == NewForm->FormId) {
          IsSearchForm = TRUE;
          break;
        }
      }
      if (IsSearchForm == FALSE) {
        continue;
      }
      IsSearchForm = FALSE;
      //
      //Check the suppressif condition, Skip invalid forms.
      //
      if (EvaluateExpressionList(NewForm->SuppressExpression, TRUE, FormSet, NewForm)!= ExpressFalse) {
        continue;
      }

      LinkQuestion = GetFirstNode (&NewForm->StatementListHead);
      while (!IsNull (&NewForm->StatementListHead, LinkQuestion)) {
        Question = FORM_BROWSER_STATEMENT_FROM_LINK (LinkQuestion);
        LinkQuestion = GetNextNode (&NewForm->StatementListHead, LinkQuestion);
        //
        //Skip Subtitle OpCode
        //
        if (Question->Operand == EFI_IFR_SUBTITLE_OP) {
          continue;
        }
        if (Question->Prompt != 0) {
          TargetPromptString = HiiGetString (FormSet->HiiHandle, Question->Prompt, "en-US");
        } else {
          continue;
        }
        if (TargetPromptString == NULL) {
          continue;
        }
        //
        //Corresponding string matching was performed according to different search methods.
        //
        LibToLowerString(TargetPromptString);
        if (SearchData.MatchingAlgorithm == 0) {
          StrResult = StrStr(TargetPromptString, InputSearchString);
        } else if (SearchData.MatchingAlgorithm == 1) {
          StrResult = StrStartsWithFirstChar(TargetPromptString, InputSearchString);
        } else {
          StrResult = StrEndsWithSubstring(TargetPromptString, InputSearchString);
        }
        ExpressResult = EvaluateExpressionList(Question->Expression, TRUE, FormSet, NewForm);
        if (StrResult != NULL && ExpressResult != ExpressSuppress && ExpressResult != ExpressDisable && !CompareGuid (&FormSet->Guid, &gEfiFormsetGuidSearch)){
          mSearchformData[mSearchCount].FormId = NewForm->FormId;
          CopyGuid (&mSearchformData[mSearchCount].FormSetguid, &FormSet->Guid);
          mSearchformData[mSearchCount].HiiHandle = FormSet->HiiHandle;
          mSearchformData[mSearchCount].Operand = Question->Operand;
          mSearchformData[mSearchCount].Prompt = Question->Prompt;
          mSearchformData[mSearchCount].QuestionId = Question->QuestionId;
          mSearchformData[mSearchCount].ExpressResult = EvaluateExpressionList(Question->Expression, TRUE, FormSet, NewForm);
          mSearchCount ++;
        }
        FreePool (TargetPromptString);
      }
      if (EnterType == LOGIN_USER_POP) {
        break;
      }
    }
  }
  if (mSearchCount == 0) {
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

/**
  This function allows a caller to update the search page according to the search content(mSearchformData).

  @retval EFI_SUCCESS            Page updated successfully.
**/
EFI_STATUS
UpdateSearchForm(
  VOID
  )
{
  VOID                                  *StartOpCodeHandle;
  VOID                                  *EndOpCodeHandle;
  VOID                                  *StartOpCodeHandleText;
  VOID                                  *EndOpCodeHandleText;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_IFR_GUID_LABEL                    *EndLabel;
  UINTN                                 Index;
  CHAR16                                *TargetPromptString;
  CHAR16                                *TargetPromptStringChinese;
  EFI_STRING_ID                         TempPrompt;
  EFI_STATUS                            Status;
  EFI_IFR_GUID_LABEL                    *StartLabelText;
  EFI_IFR_GUID_LABEL                    *EndLabelText;
  BOOLEAN                               IsUpdateSubtitle = TRUE;
  CHAR16                                *UpdateSubtitleString;
  UINTN                                 UnoptionalIndex = 0;
  EFI_GUID                              *UnOptionalSearchformGuid;
  BOOLEAN                               CreatOpcode = TRUE;
  UINTN                                 IndexGuid = 0;
  EFI_STRING                            TitleString;
  BOOLEAN                               CreatOptionaltitle = TRUE;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_UPDATE_SEARCH_FORM;

  EndLabel               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_END;

  StartOpCodeHandleText = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandleText != NULL);

  EndOpCodeHandleText = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandleText != NULL);

  StartLabelText               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (StartOpCodeHandleText, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabelText->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabelText->Number       = LABEL_UPDATE_UNOPTIONAL_SEARCH_FORM;

  EndLabelText               = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (EndOpCodeHandleText, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabelText->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabelText->Number       = LABEL_UNOPTIONAL_END;

  UnOptionalSearchformGuid = AllocatePool (mSearchCount * sizeof (EFI_GUID ));
  HiiSetString(gPrivateData.HiiHandle, STRING_TOKEN(INPUT_SEARCH_NOT_OPTIONAL),L"", NULL);

  if (mSearchformData != NULL) {
    TitleString = HiiGetString (gPrivateData.HiiHandle, STRING_TOKEN (INPUT_SEARCH_RESULT_TUTLE3), NULL);
    HiiSetString (gPrivateData.HiiHandle, STRING_TOKEN (INPUT_SEARCH_RESULT_TUTLE1),TitleString, NULL);
    FreePool (TitleString);
    HiiCreateSubTitleOpCode (
        StartOpCodeHandle,
        STRING_TOKEN(INPUT_SEARCH_RESULT_TUTLE1),
        0,
        0,
        0
        );
    mUpdateSearchTitle = TRUE;
  }

  for (Index = 0; Index < mSearchCount; Index++ ) {
    if (mSearchformData == NULL) {
      break;
    }
    CreatOpcode = TRUE;
    //
    //Match the current language.
    //
    TempPrompt = mSearchformData[Index].Prompt;
    TargetPromptString = HiiGetString (mSearchformData[Index].HiiHandle, mSearchformData[Index].Prompt, "en-US");
    mSearchformData[Index].Prompt = 0;
    mSearchformData[Index].Prompt = HiiSetString(gPrivateData.HiiHandle, mSearchformData[Index].Prompt,TargetPromptString, "en-US");

    TargetPromptStringChinese = HiiGetString (mSearchformData[Index].HiiHandle, TempPrompt,  "zh-Hans");
    if (TargetPromptStringChinese != NULL) {
      mSearchformData[Index].Prompt = HiiSetString(gPrivateData.HiiHandle, mSearchformData[Index].Prompt, TargetPromptStringChinese,  "zh-Hans");
      FreePool (TargetPromptStringChinese);
    } else {
      mSearchformData[Index].Prompt = HiiSetString(gPrivateData.HiiHandle, mSearchformData[Index].Prompt, TargetPromptString, "zh-Hans");
    }
    FreePool (TargetPromptString);
    //
    //For text, grayscale, and QuestionId 0, go to the corresponding form.
    //
    if (mSearchformData[Index].Operand == EFI_IFR_TEXT_OP  || mSearchformData[Index].ExpressResult == ExpressGrayOut || mSearchformData[Index].QuestionId == 0) {
      //
      //Update Subtitle before optional and non-optional options.
      //
      if (IsUpdateSubtitle == TRUE) {
        UpdateSubtitleString = HiiGetString (gPrivateData.HiiHandle, STRING_TOKEN(INPUT_SEARCH_UPDATE), "en-US");
        HiiSetString(gPrivateData.HiiHandle, STRING_TOKEN(INPUT_SEARCH_NOT_OPTIONAL),UpdateSubtitleString, "en-US");
        FreePool (UpdateSubtitleString);

        UpdateSubtitleString = HiiGetString (gPrivateData.HiiHandle, STRING_TOKEN(INPUT_SEARCH_UPDATE), "zh-Hans");
        HiiSetString(gPrivateData.HiiHandle, STRING_TOKEN(INPUT_SEARCH_NOT_OPTIONAL),UpdateSubtitleString, "zh-Hans");
        FreePool (UpdateSubtitleString);
        IsUpdateSubtitle = FALSE;
      }
      //
      //When there are multiple Text, grayscale, and questionid zero options on the same page, just create a goto option to jump to the corresponding page.
      //
      for (IndexGuid = 0; IndexGuid < UnoptionalIndex; IndexGuid++) {
        if (CompareGuid (&UnOptionalSearchformGuid[IndexGuid], &mSearchformData[Index].FormSetguid)) {
          CreatOpcode = FALSE;
          break;
        }
      }
      if (CreatOpcode) {
        CopyGuid (&UnOptionalSearchformGuid[UnoptionalIndex], &mSearchformData[Index].FormSetguid);
        UnoptionalIndex++;

        HiiCreateGotoExOpCode (
          StartOpCodeHandleText,
          mSearchformData[Index].FormId,
          mSearchformData[Index].Prompt,
          STRING_TOKEN (STR_EMPTY),
          EFI_IFR_FLAG_CALLBACK,
          (EFI_QUESTION_ID)(0x8003 + Index),
          0,
          &mSearchformData[Index].FormSetguid,
          0
        );
      }
    } else {
      if (CreatOptionaltitle) {
        HiiCreateSubTitleOpCode (
          StartOpCodeHandle,
          STRING_TOKEN(STR_EMPTY),
          0,
          0,
          0
          );
        HiiCreateSubTitleOpCode (
          StartOpCodeHandle,
          STRING_TOKEN(INPUT_SEARCH_OPTIONAL),
          0,
          0,
          0
          );
        CreatOptionaltitle = FALSE;
      }

      HiiCreateGotoExOpCode (
        StartOpCodeHandle,
        mSearchformData[Index].FormId,
        mSearchformData[Index].Prompt,
        STRING_TOKEN (STR_EMPTY),
        EFI_IFR_FLAG_CALLBACK,
        (EFI_QUESTION_ID)(0x8003 + Index),
        mSearchformData[Index].QuestionId,
        &mSearchformData[Index].FormSetguid,
        0
      );
    }
  }

  Status = HiiUpdateForm (
                  gPrivateData.HiiHandle,
                  &gByoSetupSearchFormGuid,
                  SEARCH_OPTION_ID,
                  StartOpCodeHandle,
                  EndOpCodeHandle
                  );
  Status = HiiUpdateForm (
                  gPrivateData.HiiHandle,
                  &gByoSetupSearchFormGuid,
                  SEARCH_OPTION_ID,
                  StartOpCodeHandleText,
                  EndOpCodeHandleText
                  );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
  HiiFreeOpCodeHandle (StartOpCodeHandleText);
  HiiFreeOpCodeHandle (EndOpCodeHandleText);
  FreePool(UnOptionalSearchformGuid);
  if (mSearchformData != NULL) {
    FreePool(mSearchformData);
    mSearchformData = NULL;
  }

  return Status;
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
  @retval  EFI_INVALID_PARAMETER  Request is NULL, illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
SearchOptionExtractConfig (
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
SearchOptionRouteConfig (
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
SearchOptionCallBack (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  CHAR16                                  *InputString = NULL;
  EFI_STATUS                              Status;
  EFI_STRING                              String;
  SEARCH_SAVE_DATA                        SearchData;

  if ((This == NULL) || (Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    return EFI_SUCCESS;
  }

  String = HiiGetString (gPrivateData.HiiHandle, STRING_TOKEN (INPUT_SEARCH_RESULT_TUTLE2), NULL);
  if (Action == EFI_BROWSER_ACTION_FORM_OPEN && mUpdateSearchTitle) {
    HiiSetString (gPrivateData.HiiHandle, STRING_TOKEN (INPUT_SEARCH_RESULT_TUTLE1), String, NULL);
  }
  FreePool (String);

  //
  // Clear the search results when the default value is loaded.
  //
  if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    if (QuestionId == KEY_SEARCH_OPTION ) {
      Status = UpdateSearchForm ();
      HiiSetString(gPrivateData.HiiHandle, Value->string, L"", NULL);
    } else {
      return EFI_UNSUPPORTED;
    }
  }

  if (Action == EFI_BROWSER_ACTION_CHANGED) {
    switch (QuestionId) {
      case KEY_SEARCH_OPTION:
        InputString = HiiGetString(gPrivateData.HiiHandle, Value->string, NULL);
        Status = SearchFormAndGetFormId(InputString);
        if (Status != EFI_SUCCESS) {
          String = HiiGetString (gPrivateData.HiiHandle, STRING_TOKEN (SEARCH_ERROR_PROMPT), NULL);
          UiConfirmDialogEx (DIALOG_ATTRIBUTE_RECOVER, DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
          FreePool (String);
        }
        Status = UpdateSearchForm ();
        break;
      case KEY_SEARCH_ALGORITHM:
        HiiGetBrowserData (&gEfiSetupSearchGuid, EFI_SEARCH_VARIABLE_NAME, sizeof(SEARCH_SAVE_DATA), (UINT8 *)&SearchData);
        if (SearchData.SearchInputData[0] == 0) {
          break;
        }
        Status = SearchFormAndGetFormId((CHAR16 *)SearchData.SearchInputData);
        if (Status != EFI_SUCCESS) {
          String = HiiGetString (gPrivateData.HiiHandle, STRING_TOKEN (SEARCH_ERROR_PROMPT), NULL);
          UiConfirmDialogEx (DIALOG_ATTRIBUTE_RECOVER, DIALOG_INFO, NULL, NULL, TEXT_ALIGIN_CENTER, String, NULL);
          FreePool (String);
        }
        Status = UpdateSearchForm ();
        break;

      default:
        break;
    }
  }

  if (InputString != NULL) {
    FreePool (InputString);
  }
  return EFI_SUCCESS;
}
/**
  Create a menu with specified formset GUID and form ID, and add it as a child
  of the given parent menu.

  @param  HiiHandle              Hii handle related to this formset.
  @param  FormSetGuid            The Formset Guid of menu to be added.
  @param  FormId                 The Form ID of menu to be added.
  @param  QuestionId             The question id of this menu to be added.

  @return A pointer to the newly added menu or NULL if memory is insufficient.

**/
FORM_ENTRY_INFO *
UiAddMenuList (
  IN EFI_HII_HANDLE       HiiHandle,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId,
  IN UINT16               QuestionId
  )
{
  FORM_ENTRY_INFO  *MenuList;

  MenuList = AllocateZeroPool (sizeof (FORM_ENTRY_INFO));
  if (MenuList == NULL) {
    return NULL;
  }

  MenuList->Signature = FORM_ENTRY_INFO_SIGNATURE;

  MenuList->HiiHandle  = HiiHandle;
  CopyMem (&MenuList->FormSetGuid, FormSetGuid, sizeof (EFI_GUID));
  MenuList->FormId     = FormId;
  MenuList->QuestionId = QuestionId;

  //
  // If parent is not specified, it is the root Form of a Formset
  //
  InsertTailList (&mPrivateData.FormBrowserEx2.FormViewHistoryHead, &MenuList->Link);

  return MenuList;
}

/**
  Return the form id for the input hiihandle and formset.

  @param  HiiHandle              HiiHandle for FormSet.
  @param  FormSetGuid            The Formset GUID of the menu to search.

  @return First form's id for this form set.

**/
EFI_FORM_ID
GetFirstFormId (
  IN EFI_HII_HANDLE       HiiHandle,
  IN EFI_GUID             *FormSetGuid
  )
{
  LIST_ENTRY         *Link;
  FORM_BROWSER_FORM  *Form;

  Link = GetFirstNode (&gCurrentSelection->FormSet->FormListHead);
  Form = FORM_BROWSER_FORM_FROM_LINK (Link);

  return Form->FormId;
}

/**
  Search Menu with given FormSetGuid and FormId in all cached menu list.

  @param  HiiHandle              HiiHandle for FormSet.
  @param  FormSetGuid            The Formset GUID of the menu to search.
  @param  FormId                 The Form ID of menu to search.

  @return A pointer to menu found or NULL if not found.

**/
FORM_ENTRY_INFO *
UiFindMenuList (
  IN EFI_HII_HANDLE       HiiHandle,
  IN EFI_GUID             *FormSetGuid,
  IN UINT16               FormId
  )
{
  LIST_ENTRY         *Link;
  FORM_ENTRY_INFO    *MenuList;
  FORM_ENTRY_INFO    *RetMenu;
  EFI_FORM_ID        FirstFormId;

  RetMenu = NULL;

  Link = GetFirstNode (&mPrivateData.FormBrowserEx2.FormViewHistoryHead);
  while (!IsNull (&mPrivateData.FormBrowserEx2.FormViewHistoryHead, Link)) {
    MenuList = FORM_ENTRY_INFO_FROM_LINK (Link);
    Link = GetNextNode (&mPrivateData.FormBrowserEx2.FormViewHistoryHead, Link);

    //
    // If already find the menu, free the menus behind it.
    //
    if (RetMenu != NULL) {
      RemoveEntryList (&MenuList->Link);
      FreePool (MenuList);
      continue;
    }

    //
    // Find the same FromSet.
    //
    if (MenuList->HiiHandle == HiiHandle) {
      if (IsZeroGuid (&MenuList->FormSetGuid)) {
        //
        // FormSetGuid is not specified.
        //
        RetMenu = MenuList;
      } else if (CompareGuid (&MenuList->FormSetGuid, FormSetGuid)) {
        if (MenuList->FormId == FormId) {
          RetMenu = MenuList;
        } else if (FormId == 0 || MenuList->FormId == 0 ) {
          FirstFormId = GetFirstFormId (HiiHandle, FormSetGuid);
          if ((FormId == 0 && FirstFormId == MenuList->FormId) || (MenuList->FormId ==0 && FirstFormId == FormId)) {
            RetMenu = MenuList;
          }
        }
      }
    }
  }

  return RetMenu;
}

/**
  Find parent menu for current menu.

  @param  CurrentMenu    Current Menu
  @param  SettingLevel   Whether find parent menu in Form Level or Formset level.
                         In form level, just find the parent menu;
                         In formset level, find the parent menu which has different
                         formset guid value.

  @retval   The parent menu for current menu.
**/
FORM_ENTRY_INFO *
UiFindParentMenu (
  IN FORM_ENTRY_INFO          *CurrentMenu,
  IN BROWSER_SETTING_SCOPE    SettingLevel
  )
{
  FORM_ENTRY_INFO    *ParentMenu;
  LIST_ENTRY         *Link;

  ASSERT (SettingLevel == FormLevel || SettingLevel == FormSetLevel);

  if (CurrentMenu == NULL) {
    return NULL;
  }

  ParentMenu = NULL;
  Link       = &CurrentMenu->Link;

  while (Link->BackLink != &mPrivateData.FormBrowserEx2.FormViewHistoryHead) {
    ParentMenu = FORM_ENTRY_INFO_FROM_LINK (Link->BackLink);

    if (SettingLevel == FormLevel) {
      //
      // For FormLevel, just find the parent menu, return.
      //
      break;
    }

    if (!CompareGuid (&CurrentMenu->FormSetGuid, &ParentMenu->FormSetGuid)) {
      //
      // For SystemLevel, must find the menu which has different formset.
      //
      break;
    }

    Link = Link->BackLink;
  }

  //
  // Not find the parent menu, just return NULL.
  //
  if (Link->BackLink == &mPrivateData.FormBrowserEx2.FormViewHistoryHead) {
    return NULL;
  }

  return ParentMenu;
}

/**
  Free Menu list linked list.

  @param  MenuListHead    One Menu list point in the menu list.

**/
VOID
UiFreeMenuList (
  LIST_ENTRY   *MenuListHead
  )
{
  FORM_ENTRY_INFO    *MenuList;

  while (!IsListEmpty (MenuListHead)) {
    MenuList = FORM_ENTRY_INFO_FROM_LINK (MenuListHead->ForwardLink);
    RemoveEntryList (&MenuList->Link);

    FreePool (MenuList);
  }
}

/**
  Copy current Menu list to the new menu list.

  @param  NewMenuListHead        New create Menu list.
  @param  CurrentMenuListHead    Current Menu list.

**/
VOID
UiCopyMenuList (
  OUT LIST_ENTRY   *NewMenuListHead,
  IN  LIST_ENTRY   *CurrentMenuListHead
  )
{
  LIST_ENTRY         *Link;
  FORM_ENTRY_INFO    *MenuList;
  FORM_ENTRY_INFO    *NewMenuEntry;

  //
  // If new menu list not empty, free it first.
  //
  UiFreeMenuList (NewMenuListHead);

  Link = GetFirstNode (CurrentMenuListHead);
  while (!IsNull (CurrentMenuListHead, Link)) {
    MenuList = FORM_ENTRY_INFO_FROM_LINK (Link);
    Link = GetNextNode (CurrentMenuListHead, Link);

    NewMenuEntry = AllocateZeroPool (sizeof (FORM_ENTRY_INFO));
    ASSERT (NewMenuEntry != NULL);
    NewMenuEntry->Signature  = FORM_ENTRY_INFO_SIGNATURE;
    NewMenuEntry->HiiHandle  = MenuList->HiiHandle;
    CopyMem (&NewMenuEntry->FormSetGuid, &MenuList->FormSetGuid, sizeof (EFI_GUID));
    NewMenuEntry->FormId     = MenuList->FormId;
    NewMenuEntry->QuestionId = MenuList->QuestionId;

    InsertTailList (NewMenuListHead, &NewMenuEntry->Link);
  }
}


/**
  Load hii formset which would be displayed by the browser.

**/
VOID
LoadAllHiiFormset (
  VOID
  )
{
  FORM_BROWSER_FORMSET    *LocalFormSet;
  EFI_HII_HANDLE          *HiiHandles;
  UINTN                   Index;
  UINTN                   Index1;
  EFI_GUID                ZeroGuid;
  EFI_STATUS              Status;
  FORM_BROWSER_FORMSET    *OldFormset;
  BOOLEAN                 IsDisplayFormset;
  LIST_ENTRY              *Link;
  BYO_BROWSER_FORMSET     *MainFormSet;

  OldFormset = mSystemLevelFormSet;

  DMSG ((EFI_D_ERROR, "\n LoadAllHiiFormset, Begin, \n"));
  //
  // Get all the Hii handles
  //
  HiiHandles = HiiGetHiiHandles (NULL);
  ASSERT (HiiHandles != NULL);

  mNumOfDisplayFormset = 0;
  if (mDisplayFormsetGuidList != NULL){
    FreePool (mDisplayFormsetGuidList);
  }

  for (Index = 0; HiiHandles[Index] != NULL; Index++);
  mDisplayFormsetGuidList =  AllocateZeroPool(Index * sizeof (EFI_GUID));
  ASSERT(mDisplayFormsetGuidList != NULL);
  
  //
  // Add all known root formset Guid
  //
  for (Index = 0; Index < ARRAY_SIZE (mRootFormsetGuidList); Index ++) {
    FindGotoExOpcode(mRootFormsetGuidList[Index]);
  }
  //
  //Find GotoExOpcode.
  // 
  Link = GetFirstNode (&gByoFormsetManager->ByoFormSetList);
  while (!IsNull (&gByoFormsetManager->ByoFormSetList, Link)) {
    MainFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (Link);
    for (Index = 0; Index < ARRAY_SIZE (mRootFormsetGuidList); Index ++) {
      if (CompareMem (&MainFormSet->Guid, mRootFormsetGuidList[Index], sizeof (EFI_GUID)) == 0) {
        break;
      }
    }
    if (Index == ARRAY_SIZE (mRootFormsetGuidList)) {
      FindGotoExOpcode(&MainFormSet->Guid);
    }
    Link = GetNextNode (&gByoFormsetManager->ByoFormSetList, Link);
  }

  if (mNumOfDisplayFormset == 0){
    FreePool (HiiHandles);
    return ;
  }

  //
  // Search for formset of each class type
  //
  for (Index = 0; HiiHandles[Index] != NULL; Index++) {
    //
    // Check HiiHandles[Index] does exist in global maintain list.
    //
    DMSG ((EFI_D_ERROR, "LoadAllHiiFormset, HiiHandles :0x%x. \n", HiiHandles[Index]));
    if (IsHiiHandleInBrowserContext (HiiHandles[Index])) {
      continue;
    }
    if (GetFormSetFromHiiHandle (HiiHandles[Index]) != NULL) {
      continue;
    }

    //
    // Initialize FormSet Setting
    //
    LocalFormSet = AllocateZeroPool (sizeof (FORM_BROWSER_FORMSET));
    ASSERT (LocalFormSet != NULL);
    mSystemLevelFormSet = LocalFormSet;
    IsDisplayFormset = FALSE;

    ZeroMem (&ZeroGuid, sizeof (ZeroGuid));
    Status = InitializeFormSet (HiiHandles[Index], &ZeroGuid, LocalFormSet);
    
    //
    //Check if this formset needs to be displayed
    //
    for (Index1 = 0; Index1 < mNumOfDisplayFormset; Index1++) {
      if (CompareGuid(&mDisplayFormsetGuidList[Index1], &LocalFormSet->Guid)){
        IsDisplayFormset = TRUE;
        break;
      }
    }
    if (IsDisplayFormset == FALSE) {
      Status = EFI_NOT_FOUND;
    }

    if (EFI_ERROR (Status) || IsListEmpty (&LocalFormSet->FormListHead)) {
      DestroyFormSet (LocalFormSet);
      continue;
    }

    InitializeCurrentSetting (LocalFormSet);
    DMSG_HII(LocalFormSet->FormSetTitle, LocalFormSet->HiiHandle, L"LocalFormSet");

    //
    // Initialize Questions' Value
    //
    Status = LoadFormSetConfig (NULL, LocalFormSet);
    if (EFI_ERROR (Status)) {
      DestroyFormSet (LocalFormSet);
      continue;
    }


  }

  //
  // Free resources, and restore gOldFormSet and gClassOfVfr
  //
  FreePool (HiiHandles);
  
  mSystemLevelFormSet = OldFormset;
  mBeLoadAllHiiFormset = TRUE;

  DMSG ((EFI_D_ERROR, "LoadAllHiiFormset, End. \n"));  
}

 /**
  Find the GotoExOpcode contained in the formset.

  @param  EFI_GUID Current formset guid.
  
**/
VOID
FindGotoExOpcode (
EFI_GUID  *CurrentGuid
){
  EFI_HII_HANDLE          *HiiHandles;
  UINTN                   Index;
  EFI_STATUS              Status;
  FORM_BROWSER_FORMSET    *FormSet;
  UINT8                   *OpCodeData;
  UINTN                   OpCodeOffset = 0;
  UINTN                   OpCodeLength;
  UINT8                   Operand;
  EFI_GUID                NewGuid;

  CopyMem(&mDisplayFormsetGuidList[mNumOfDisplayFormset], CurrentGuid, sizeof (EFI_GUID));
  DMSG((EFI_D_INFO,"mDisplayFormsetGuidList[%d]:%g\n",mNumOfDisplayFormset,&mDisplayFormsetGuidList[mNumOfDisplayFormset]));
  mNumOfDisplayFormset++;

  HiiHandles = HiiGetHiiHandles (NULL);
  ASSERT (HiiHandles != NULL);

  for (Index = 0; HiiHandles[Index] != NULL; Index++) {
    FormSet = AllocateZeroPool (sizeof (FORM_BROWSER_FORMSET));
    ASSERT (FormSet != NULL);
    Status = GetIfrBinaryData (HiiHandles[Index], CurrentGuid, &FormSet->IfrBinaryLength, &FormSet->IfrBinaryData);
    if (EFI_ERROR(Status)) {
      if (FormSet->IfrBinaryData != NULL) {
        FreePool (FormSet->IfrBinaryData);
      }
      FreePool (FormSet);
      continue;
    }
    DMSG((EFI_D_INFO,"GUID:%g in HiiHandle[%d]:%r\n",CurrentGuid,Index,Status));

    //
    //Find and copy the FormsetGuid in GotoExOpcode.
    //
    while (OpCodeOffset < FormSet->IfrBinaryLength) {
      OpCodeData = FormSet->IfrBinaryData + OpCodeOffset;
      OpCodeLength = ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
      OpCodeOffset += OpCodeLength;
      Operand = ((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode;
  
      if (Operand == EFI_IFR_REF_OP && OpCodeLength >= sizeof (EFI_IFR_REF3)) {
        if ((OpCodeLength >= sizeof (EFI_IFR_REF4))){
          CopyMem(&NewGuid, &((EFI_IFR_REF4 *) OpCodeData)->FormSetId, sizeof (EFI_GUID));
        } else {
          CopyMem(&NewGuid, &((EFI_IFR_REF3 *) OpCodeData)->FormSetId, sizeof (EFI_GUID));
        }
        FindGotoExOpcode(&NewGuid);
      }
    }

    if (FormSet->IfrBinaryData != NULL) {
      FreePool (FormSet->IfrBinaryData);
    }
    FreePool (FormSet);
    break;
  }

  FreePool(HiiHandles);
}
/**
  Pop up the error info.

  @param      BrowserStatus    The input browser status.
  @param      HiiHandle        The Hiihandle for this opcode.
  @param      OpCode           The opcode use to get the erro info and timeout value.
  @param      ErrorString      Error string used by BROWSER_NO_SUBMIT_IF.

**/
UINT32
PopupErrorMessage (
  IN UINT32                BrowserStatus,
  IN EFI_HII_HANDLE        HiiHandle,
  IN EFI_IFR_OP_HEADER     *OpCode, OPTIONAL
  IN CHAR16                *ErrorString
  )
{
  FORM_DISPLAY_ENGINE_STATEMENT *Statement;
  USER_INPUT                    UserInputData;

  Statement = NULL;

  if (OpCode != NULL) {
    Statement = AllocateZeroPool (sizeof(FORM_DISPLAY_ENGINE_STATEMENT));
    ASSERT (Statement != NULL);
    Statement->OpCode = OpCode;
    gDisplayFormData.HighLightedStatement = Statement;
  }

  //
  // Used to compatible with old display engine.
  // New display engine not use this field.
  //
  gDisplayFormData.ErrorString   = ErrorString;
  gDisplayFormData.BrowserStatus = BrowserStatus;

  if (HiiHandle != NULL) {
    gDisplayFormData.HiiHandle     = HiiHandle;
  }

  mFormDisplay->FormDisplay (&gDisplayFormData, &UserInputData);

  gDisplayFormData.BrowserStatus = BROWSER_SUCCESS;
  gDisplayFormData.ErrorString   = NULL;

  if (OpCode != NULL) {
    FreePool (Statement);
  }

  return UserInputData.Action;
}

/**
  This is the routine which an external caller uses to direct the browser
  where to obtain it's information.


  @param This            The Form Browser protocol instance.
  @param Handles         A pointer to an array of Handles.  If HandleCount > 1 we
                         display a list of the formsets for the handles specified.
  @param HandleCount     The number of Handles specified in Handle.
  @param FormSetGuid     This field points to the EFI_GUID which must match the Guid
                         field in the EFI_IFR_FORM_SET op-code for the specified
                         forms-based package. If FormSetGuid is NULL, then this
                         function will display the first found forms package.
  @param FormId          This field specifies which EFI_IFR_FORM to render as the first
                         displayable page. If this field has a value of 0x0000, then
                         the forms browser will render the specified forms in their encoded order.
  @param ScreenDimensions Points to recommended form dimensions, including any non-content area, in
                          characters.
  @param ActionRequest   Points to the action recommended by the form.

  @retval  EFI_SUCCESS            The function completed successfully.
  @retval  EFI_INVALID_PARAMETER  One of the parameters has an invalid value.
  @retval  EFI_NOT_FOUND          No valid forms could be found to display.

**/
EFI_STATUS
EFIAPI
SendForm (
  IN  CONST EFI_FORM_BROWSER2_PROTOCOL *This,
  IN  EFI_HII_HANDLE                   *Handles,
  IN  UINTN                            HandleCount,
  IN  EFI_GUID                         *FormSetGuid, OPTIONAL
  IN  UINT16                           FormId, OPTIONAL
  IN  CONST EFI_SCREEN_DESCRIPTOR      *ScreenDimensions, OPTIONAL
  OUT EFI_BROWSER_ACTION_REQUEST       *ActionRequest  OPTIONAL
  )
{
  EFI_STATUS                    Status;
  UI_MENU_SELECTION             *Selection;
  UINTN                         Index;
  FORM_BROWSER_FORMSET          *FormSet;
  FORM_ENTRY_INFO               *MenuList;
  BOOLEAN                       RetVal;
  STATIC LIST_ENTRY             *ByoCurrentFormSetLink;

  //
  // If EDKII_FORM_DISPLAY_ENGINE_PROTOCOL not found, return EFI_UNSUPPORTED.
  //
  if (mFormDisplay == NULL) {
    DEBUG ((DEBUG_ERROR, "Fatal Error! EDKII_FORM_DISPLAY_ENGINE_PROTOCOL not found!"));
    return EFI_UNSUPPORTED;
  }

  DMSG ((EFI_D_ERROR, "\n SendForm, \n"));

  //
  // Save globals used by SendForm()
  //
  SaveBrowserContext ();
  ByoCurrentFormSetLink = gDisplayFormData.ByoCurrentFormSetLink;

  gFlagReconnect = FALSE;
  gResetRequired = FALSE;
  gExitRequired  = FALSE;
  gCallbackReconnect = FALSE;
  Status         = EFI_SUCCESS;
  gEmptyString   = L"";
  gDisplayFormData.ScreenDimensions = (EFI_SCREEN_DESCRIPTOR *) ScreenDimensions;
  gDisplayFormData.ByoCurrentFormSetLink = NULL;
	
  for (Index = 0; Index < HandleCount; Index++) {
    Selection = AllocateZeroPool (sizeof (UI_MENU_SELECTION));
    ASSERT (Selection != NULL);

    DMSG ((EFI_D_ERROR, "SendForm, HandleCount :%d, Index :%d.\n", HandleCount, Index));
    Selection->Handle = Handles[Index];
    if (FormSetGuid != NULL) {
      CopyMem (&Selection->FormSetGuid, FormSetGuid, sizeof (EFI_GUID));
      Selection->FormId = FormId;
    } else {
      CopyMem (&Selection->FormSetGuid, &gEfiHiiPlatformSetupFormsetGuid, sizeof (EFI_GUID));
    }

    do {
      DMSG ((EFI_D_ERROR, "\n SendForm, Create New Formset, \n"));
      FormSet = AllocateZeroPool (sizeof (FORM_BROWSER_FORMSET));
      ASSERT (FormSet != NULL);

      //
      // Validate the HiiHandle
      // if validate failed, find the first validate parent HiiHandle.
      //
      if (!ValidateHiiHandle(Selection->Handle)) {
        FindNextMenu (Selection, FormSetLevel);
      }

      //
      // Initialize internal data structures of FormSet
      //
      DMSG ((EFI_D_ERROR, "\n SendForm, InitializeFormSet, \n"));
      Status = InitializeFormSet (Selection->Handle, &Selection->FormSetGuid, FormSet);
      DMSG ((EFI_D_ERROR, "SendForm, InitializeFormSet :%r.\n", Status));
      if (EFI_ERROR (Status) || IsListEmpty (&FormSet->FormListHead)) {
        DestroyFormSet (FormSet);
        break;
      }
      Selection->FormSet = FormSet;
      mSystemLevelFormSet = FormSet;

      //
      // Display this formset
      //
      gCurrentSelection = Selection;
	  
      DMSG ((EFI_D_ERROR, "\n SendForm, SetupBrowser, \n"));
      Status = SetupBrowser (Selection);
      DMSG ((EFI_D_ERROR, "SendForm, SetupBrowser :%r.\n", Status));
	  
      gCurrentSelection = NULL;
      mSystemLevelFormSet = NULL;

      if (gFlagReconnect || gCallbackReconnect) {
        RetVal = ReconnectController (FormSet->DriverHandle);
        if (!RetVal) {
          PopupErrorMessage(BROWSER_RECONNECT_FAIL, NULL, NULL, NULL);
        }
        gFlagReconnect = FALSE;
        gCallbackReconnect = FALSE;
      }

      if (EFI_ERROR (Status)) {
        break;
      }	  
      DMSG ((EFI_D_ERROR, "SendForm, End, Selection->Action :0x%x\n", Selection->Action));
    } while (Selection->Action == UI_ACTION_REFRESH_FORMSET);

    FreePool (Selection);
  }

  if (ActionRequest != NULL) {
    *ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    if (gResetRequired) {
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_RESET;
    }
  }

  mFormDisplay->ExitDisplay();

  //
  // Clear the menu history data.
  //
  while (!IsListEmpty (&mPrivateData.FormBrowserEx2.FormViewHistoryHead)) {
    MenuList = FORM_ENTRY_INFO_FROM_LINK (mPrivateData.FormBrowserEx2.FormViewHistoryHead.ForwardLink);
    RemoveEntryList (&MenuList->Link);
    FreePool (MenuList);
  }

  //
  // Restore globals used by SendForm()
  //
  RestoreBrowserContext ();
  gDisplayFormData.ByoCurrentFormSetLink = ByoCurrentFormSetLink;

  return Status;
}

/**
  Get Formset info for ui showing.

**/
BOOLEAN
GetFormsetInfoFromGuid (
  IN    EFI_GUID     *ComparingGuid,
  OUT    EFI_HII_HANDLE      *FormsetHandle,
  OUT    EFI_STRING_ID       *FormSetTitle
  )
{
  EFI_HII_HANDLE               *HiiHandles;
  UINTN                        Index;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINTN                        BufferSize;
  UINT32                       Offset;
  UINT32                       Offset2;
  UINT32                       PackageListLength;
  EFI_HII_PACKAGE_HEADER       PackageHeader;
  UINT8                        *Package;
  UINT8                        *OpCodeData;
  EFI_STATUS                   Status;
  EFI_HII_HANDLE               HiiHandle;

  ASSERT (ComparingGuid != NULL);

  HiiHandle  = NULL;
  //
  // Get all the Hii handles
  //
  HiiHandles = HiiGetHiiHandles (NULL);
  ASSERT (HiiHandles != NULL);

  //
  // Search for formset of each class type
  //
  for (Index = 0; HiiHandles[Index] != NULL; Index++) {
    BufferSize = 0;
    HiiPackageList = NULL;
    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandles[Index], &BufferSize, HiiPackageList);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      HiiPackageList = AllocatePool (BufferSize);
      ASSERT (HiiPackageList != NULL);

      Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, HiiHandles[Index], &BufferSize, HiiPackageList);
    }
    if (EFI_ERROR (Status) || HiiPackageList == NULL) {
      return FALSE;
    }

    //
    // Get Form package from this HII package List
    //
    Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
    Offset2 = 0;
    CopyMem (&PackageListLength, &HiiPackageList->PackageLength, sizeof (UINT32));

    while (Offset < PackageListLength) {
      Package = ((UINT8 *) HiiPackageList) + Offset;
      CopyMem (&PackageHeader, Package, sizeof (EFI_HII_PACKAGE_HEADER));

      if (PackageHeader.Type == EFI_HII_PACKAGE_FORMS) {
        //
        // Search FormSet in this Form Package
        //
        Offset2 = sizeof (EFI_HII_PACKAGE_HEADER);
        while (Offset2 < PackageHeader.Length) {
          OpCodeData = Package + Offset2;

          if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_FORM_SET_OP) {
            //
            // Try to compare against formset GUID
            //
            if (CompareGuid (ComparingGuid, (EFI_GUID *)(OpCodeData + sizeof (EFI_IFR_OP_HEADER)))) {
              HiiHandle = HiiHandles[Index];
         *FormsetHandle = HiiHandles[Index];
    *FormSetTitle = (EFI_STRING_ID)((EFI_IFR_FORM_SET *) OpCodeData)->FormSetTitle;
              break;
            }
          }

          Offset2 += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
        }
      }
      if (HiiHandle != NULL) {
        break;
      }
      Offset += PackageHeader.Length;
    }

    FreePool (HiiPackageList);
    if (HiiHandle != NULL) {
      break;
    }
  }

  FreePool (HiiHandles);
  if (NULL == HiiHandle) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
Signal EnterSetupEvent to load all resource.

**/
VOID
SignalEnterSetupEvent (
  VOID
  )
{
  EFI_HANDLE    Handle = NULL;
  EFI_STATUS    Status = EFI_SUCCESS;
  static BOOLEAN    bSignal = FALSE;
  EFI_BYO_PLATFORM_SETUP_PROTOCOL    *ByoSetup = NULL;

  if (bSignal) {
    return;
  }
  Status = gBS->LocateProtocol (
                  &gEfiByoPlatformSetupGuid,
                  NULL,
                  (VOID**)&ByoSetup
                  );
  if (!EFI_ERROR(Status)) {
    DMSG((EFI_D_ERROR, "\n Enter Setup Event have been Signaled  by Platform.\n"));
    bSignal = TRUE;
    return;
  }

  DMSG((EFI_D_ERROR, "\n Signal Enter Setup Event, \n"));
   Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiSetupEnterGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  if (!EFI_ERROR(Status)) {
    bSignal = TRUE;  	
  }  
  return;
}

/**
  Inset  Byo Formset to ByoFormSetList.

**/
EFI_STATUS
InsertByoFormset (
  IN EFI_BYO_FORMSET_MANAGER_PROTOCOL    *This,
  IN EFI_GUID    *FormsetGuid
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  BYO_BROWSER_FORMSET    * ByoFormSet = NULL;
  LIST_ENTRY    *Link;
  EFI_HII_HANDLE  FormSetHandle;

  SignalEnterSetupEvent();

  //
  // Check Formset.
  //
  FormSetHandle = FormSetGuidToHiiHandle(FormsetGuid);
  if (NULL == FormSetHandle) {
    return EFI_NOT_FOUND;
  }
  DMSG ((EFI_D_ERROR, "\n InsertByoFormset, FormsetGuid :%g.\n", FormsetGuid));

  //
  //  Look for the same node.
  //
  Link = GetFirstNode (&This->ByoFormSetList);
  while (!IsNull (&This->ByoFormSetList, Link)) {
    ByoFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&This->ByoFormSetList, Link);
    if (CompareGuid(&ByoFormSet->Guid, FormsetGuid)) {
      DMSG((EFI_D_ERROR, "InsertByoFormset, The GUID have be registered.\n"));
      return EFI_INVALID_PARAMETER;
    }
  }
  //
  // Insert new node.
  //
  ByoFormSet = AllocateZeroPool (sizeof (BYO_BROWSER_FORMSET));
  ASSERT (ByoFormSet != NULL);
  ByoFormSet->Signature = BYO_FORM_BROWSER_FORMSET_SIGNATURE;
  CopyMem (&ByoFormSet->Guid, FormsetGuid, sizeof (EFI_GUID));
  ByoFormSet->HiiHandle = FormSetHandle;

  InsertTailList (&This->ByoFormSetList, &ByoFormSet->Link);

  if (mRunByoSetup) {
    //
    // In Setup Page, FormSetTitle needs to be prepared.
    //
    GetFormsetInfoFromGuid (
      &ByoFormSet->Guid,
      &ByoFormSet->HiiHandle,
      &ByoFormSet->FormSetTitle
      );
  }

  return Status;
}

/**
  Remove  Formset from ByoFormSetList.

**/
EFI_STATUS
RemoveByoFormset (
  IN EFI_BYO_FORMSET_MANAGER_PROTOCOL    *This,
  IN EFI_GUID    *FormsetGuid
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  LIST_ENTRY    *Link;
  BYO_BROWSER_FORMSET   *ByoFormSet = NULL;

  Link = GetFirstNode (&This->ByoFormSetList);
  while (!IsNull (&This->ByoFormSetList, Link)) {
    ByoFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&This->ByoFormSetList, Link);

    if (CompareGuid(&ByoFormSet->Guid, FormsetGuid)) {
      RemoveEntryList(&ByoFormSet->Link);
      FreePool(ByoFormSet);
    }
  }

  return Status;
}

/**
  Send a formset.
  IF FormsetGuid is NULL, show this first one.

**/
EFI_STATUS
RunByoFormset (
  IN EFI_BYO_FORMSET_MANAGER_PROTOCOL    *This,
  IN EFI_GUID    *FormsetGuid
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  LIST_ENTRY                    *Link;
  EFI_BROWSER_ACTION_REQUEST    ActionRequest = 0;
  BYO_BROWSER_FORMSET           *ByoFormSet = NULL;
  FORM_BROWSER_FORMSET          *FormSet;
  BOOLEAN                       bFound;
  EFI_SETUP_MOUSE_PROTOCOL      *SetupMouse;
  BROWSER_STORAGE               *BrowserStorage;

  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  DMSG ((EFI_D_ERROR, "\n RunByoFormset, \n"));
  //
  // Check ByoFormSetList.
  //
  if (IsListEmpty(&This->ByoFormSetList)) {
    return EFI_NOT_FOUND;
  }

  if (PcdGetBool (PcdShowSearchForm) && CompareGuid (FormsetGuid, &gEfiFormsetGuidMain)) {
    Status = InsertByoFormset(This, &gEfiFormsetGuidSearch);
    DMSG((EFI_D_INFO, "Insert Formset Search :%r.\n", Status));
 }

  mRunByoSetup = TRUE;
  //
  // Initialize all HiiHandle.
  //
  Link = GetFirstNode (&This->ByoFormSetList);
  while (!IsNull (&This->ByoFormSetList, Link)) {
    ByoFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&This->ByoFormSetList, Link);

    GetFormsetInfoFromGuid (
      &ByoFormSet->Guid,
      &ByoFormSet->HiiHandle,
      &ByoFormSet->FormSetTitle
      );
  }
  //
  // Look for runed Formset.
  //
  bFound = FALSE;
  if (NULL == FormsetGuid) {
    Link = GetFirstNode (&This->ByoFormSetList);
    if (!IsNull (&This->ByoFormSetList, Link)) {
      bFound = TRUE;
      ByoFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (Link);
    }
  } else {
    //
    //Look for Runed Guid.
    //
    Link = GetFirstNode (&This->ByoFormSetList);
    while (!IsNull (&This->ByoFormSetList, Link)) {
      ByoFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (Link);
      if (CompareGuid(&ByoFormSet->Guid, FormsetGuid)) {
        bFound = TRUE;
        break;
      }
      Link = GetNextNode (&This->ByoFormSetList, Link);
    }
    //
    // if not find Runed Guid, then run the first.
    //
    if (FALSE == bFound) {
      Link = GetFirstNode (&This->ByoFormSetList);
      if (!IsNull (&This->ByoFormSetList, Link)) {
        bFound = TRUE;
        ByoFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (Link);
      }
    }
  }

  //
  // Not Found Message.
  //
  if (FALSE == bFound) {
    return EFI_NOT_FOUND;
  }

  SetConsoleMode (TRUE);
  InvokeHookProtocol(gBS, &gAfterSetupSetConsoleModeHookGuid);

  //
  //Start Mouse driver.  
  //
  SetupMouse = NULL;
  Status = gBS->LocateProtocol (
                  &gEfiSetupMouseProtocolGuid,
                  NULL,
                  (VOID **)&SetupMouse
                  );
  DMSG ((EFI_D_ERROR, "RunByoFormset, Locate Setup Mouse :%r.\n", Status));
  if (!EFI_ERROR(Status)  && PcdGetBool(PcdTextSetupMouseEnable)) {
    DMSG ((EFI_D_ERROR, "RunByoFormset, Start Mouse,\n"));
    SetupMouse->Start ();
  }
  
  //
  // Prepare HII Browser Data
  //
  LoadAllHiiFormset ();

  DMSG ((EFI_D_ERROR, "RunByoFormset,  SendForm(), \n"));
  //
  // Send form.
  //
  Status = SendForm (
                 &mPrivateData.FormBrowser2,
                 &ByoFormSet->HiiHandle,
                 1,
                 &ByoFormSet->Guid,
                 0,
                 NULL,
                 &ActionRequest
                 );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "%a (L%d) SendForm %r\n", __FUNCTION__, __LINE__, Status));
  }

  //
  // After SendForm, Remove all Formset info.
  //
  Link = GetFirstNode (&This->ByoFormSetList);
  while (!IsNull (&This->ByoFormSetList, Link)) {
    ByoFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&This->ByoFormSetList, Link);

    RemoveEntryList(&ByoFormSet->Link);
    FreePool(ByoFormSet);
  }

  //
  // Free FormSet 
  //
  Link = GetFirstNode (&gBrowserFormSetList);
  while (!IsNull (&gBrowserFormSetList, Link)) {
    FormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&gBrowserFormSetList, Link);
    RemoveEntryList (&FormSet->Link);
    DestroyFormSet (FormSet);
  }

  //
  // Free BrowserStorage
  //
  Link  = GetFirstNode (&gBrowserStorageList);
  while (!IsNull (&gBrowserStorageList, Link)) {
    BrowserStorage = BROWSER_STORAGE_FROM_LINK (Link);
    Link = GetNextNode (&gBrowserStorageList, Link);

    if (BrowserStorage->Type == EFI_HII_VARSTORE_BUFFER || BrowserStorage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER) {
      if (BrowserStorage->ConfigRequest != NULL) {
        FreePool (BrowserStorage->ConfigRequest);
        BrowserStorage->ConfigRequest = NULL;
      }
      FreePool (BrowserStorage->Name);
      FreePool (BrowserStorage->Buffer);
      FreePool (BrowserStorage->EditBuffer);
      FreePool (BrowserStorage->DefaultBuffer);
    }
    BrowserStorage->SpareStrLen = 0;
    BrowserStorage->Initialized = FALSE;
    RemoveEntryList(&BrowserStorage->Link);
    FreePool (BrowserStorage);
  }

  //
  //Close Mouse driver.  
  //
  if (SetupMouse) {
    SetupMouse-> Close ();
    SetupMouse = NULL;
  }

// end of setup is reset, so need to restore screen resolution.  
//SetConsoleMode (FALSE);

  DMSG ((EFI_D_ERROR, "RunByoFormset,  End, ActionRequest :0x%x.\n ", ActionRequest));
  if (EFI_BROWSER_ACTION_REQUEST_RESET == ActionRequest) {
    mPrivateData.ByoFormBrowserEx.PlatformReset();
  }

  mBeLoadAllHiiFormset = FALSE;
  mRunByoSetup = FALSE;

  return Status;
}

/**
  Look for Formset with given Guid in hii database.

**/
BOOLEAN
SetupCheckFormsetWithGuid (
  IN EFI_GUID    *FormsetGuid
  )
{
  if (NULL == FormsetGuid) {
    return FALSE;
  }

  if (NULL == FormSetGuidToHiiHandle(FormsetGuid)) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Install Byo Formset Manager Protocol.

**/
EFI_STATUS
InstallByoFormsetManagerProtocol (
  VOID
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_HANDLE    Handle = NULL;
  EFI_BYO_FORMSET_MANAGER_PROTOCOL    *FormsetManager = NULL;

  DMSG((EFI_D_ERROR, "InstallByoFormsetManagerProtocol(),\n"));
  Status = gBS->LocateProtocol (
                  &gEfiByoFormsetManagerProtocolGuid,
                  NULL,
                  (VOID**)&FormsetManager
                  );
  if ( ! EFI_ERROR(Status) ) {
    DMSG((EFI_D_ERROR, "InstallByoFormsetManagerProtocol(), Protocol Already Started.\n"));
    gByoFormsetManager = FormsetManager;
    return EFI_ALREADY_STARTED;
  }

  FormsetManager = AllocateZeroPool (sizeof(EFI_BYO_FORMSET_MANAGER_PROTOCOL));
  if (FormsetManager == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  InitializeListHead (&FormsetManager->ByoFormSetList);
  FormsetManager->Insert = InsertByoFormset;
  FormsetManager->Remove = RemoveByoFormset;
  FormsetManager->Run = RunByoFormset;
  FormsetManager->CheckFormset = SetupCheckFormsetWithGuid;

  Status = gBS->InstallProtocolInterface (
                 &Handle,
                 &gEfiByoFormsetManagerProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 FormsetManager
                 );

  gByoFormsetManager = FormsetManager;

  return Status;
}

STATIC CHAR16 mConfigHdrTemplate[] = L"GUID=00000000000000000000000000000000&NAME=0000&PATH=00";

/**
  Get or set data to the storage.

  @param  ResultsDataSize        The size of the buffer associated with ResultsData.
  @param  ResultsData            A string returned from an IFR browser or
                                 equivalent. The results string will have no
                                 routing information in them.
  @param  RetrieveData           A BOOLEAN field which allows an agent to retrieve
                                 (if RetrieveData = TRUE) data from the uncommitted
                                 browser state information or set (if RetrieveData
                                 = FALSE) data in the uncommitted browser state
                                 information.
  @param  Storage                The pointer to the storage.

  @retval EFI_SUCCESS            The results have been distributed or are awaiting
                                 distribution.

**/
EFI_STATUS
ProcessStorage (
  IN OUT UINTN                         *ResultsDataSize,
  IN OUT EFI_STRING                    *ResultsData,
  IN BOOLEAN                           RetrieveData,
  IN BROWSER_STORAGE                   *Storage
  )
{
  CHAR16                *ConfigResp;
  EFI_STATUS            Status;
  CHAR16                *StrPtr;
  UINTN                 BufferSize;
  UINTN                 TmpSize;
  UINTN                 MaxLen;
  EFI_STRING            ConfigHdr;
  
  DMSG ((EFI_D_ERROR, "ProcessStorage, RetrieveData :%d, Browser Storage :%d-%s. \n", RetrieveData, Storage->Type, Storage->Name));
  if (RetrieveData) {
    //
    // Generate <ConfigResp>
    //
    Status = StorageToConfigResp (Storage, &ConfigResp, Storage->ConfigRequest, TRUE);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Skip <ConfigHdr> and '&' to point to <ConfigBody> when first copy the configbody.
    // Also need to consider add "\0" at first time.
    //
    StrPtr = StrStr (ConfigResp, L"PATH");
    ASSERT (StrPtr != NULL);
    StrPtr = StrStr (StrPtr, L"&");
    if (StrPtr == NULL) {
      FreePool (ConfigResp);
      return EFI_NOT_FOUND;
    }
    StrPtr += 1;
    BufferSize = StrSize (StrPtr);

    //
    // Copy the data if the input buffer is bigger enough.
    //
    if (*ResultsDataSize >= BufferSize) {
      StrCpyS (*ResultsData, *ResultsDataSize / sizeof (CHAR16), StrPtr);
    }

    *ResultsDataSize = BufferSize;
    FreePool (ConfigResp);
  } else {
    //
    // SetBrowserData will be supported after initialize all hii formset.
    //
    if (!mBeLoadAllHiiFormset || Storage->ConfigRequest == NULL) {
      return EFI_UNSUPPORTED;
    }
    //
    // Prepare <ConfigResp>
    // 
    ConfigHdr = mConfigHdrTemplate;
    TmpSize = StrLen (*ResultsData);
    BufferSize = (TmpSize + StrLen (ConfigHdr) + 2) * sizeof (CHAR16);
    MaxLen = BufferSize / sizeof (CHAR16);
    ConfigResp = AllocateZeroPool (BufferSize);
    ASSERT (ConfigResp != NULL);

    StrCpyS (ConfigResp, MaxLen, ConfigHdr);
    StrCatS (ConfigResp, MaxLen, L"&");
    StrCatS (ConfigResp, MaxLen, *ResultsData);

    //
    // Update Browser uncommitted data
    //
    Status = ConfigRespToStorage (Storage, ConfigResp);
    FreePool (ConfigResp);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Update the ValueChanged status for all formset with the same variable,
  when HiiSetBrowserData() be invoked.

  @param  VariableGuid                Guid of the Variable.
  @param  VariableName              Name of the Variable.
**/
VOID 
UpdateValueChangedFlag (
  IN CONST EFI_GUID    *VariableGuid, 
  IN CONST CHAR16    *VariableName
  )
{
  LIST_ENTRY            *LinkFormset;      
  FORM_BROWSER_FORMSET    *LocalFormSet;
  LIST_ENTRY            *LinkStorage;
  FORMSET_STORAGE          *Storage;

  LIST_ENTRY    *LinkForm;
  LIST_ENTRY    *LinkQuestion;
  FORM_BROWSER_STATEMENT    *Question;
  FORM_BROWSER_FORM    *Form;

  if (VariableGuid == NULL || VariableName == NULL) {
    return;
  }
  DMSG ((EFI_D_ERROR, "\n UpdateValueChangedFlag, VariableName :%s.\n", VariableName));

  //
  //Look for all present Formset.
  //
  LinkFormset = GetFirstNode (&gBrowserFormSetList);
  while (!IsNull (&gBrowserFormSetList, LinkFormset)) {
    LocalFormSet = FORM_BROWSER_FORMSET_FROM_LINK (LinkFormset);
    LinkFormset  = GetNextNode (&gBrowserFormSetList, LinkFormset);
    if (!ValidateFormSet(LocalFormSet)) {
      continue;
    }
    DMSG ((EFI_D_ERROR, "\n    \n"));	
    DMSG_HII(LocalFormSet->FormSetTitle, LocalFormSet->HiiHandle, L"LocalFormSet");	
    //
    //Look for formset with the same variable.
    //		
    LinkStorage = GetFirstNode (&LocalFormSet->StorageListHead);
    while (!IsNull (&LocalFormSet->StorageListHead, LinkStorage)) {
      Storage = FORMSET_STORAGE_FROM_LINK (LinkStorage);
      if (CompareGuid (&Storage->BrowserStorage->Guid, (EFI_GUID *) VariableGuid) && (StrCmp (Storage->BrowserStorage->Name, (CHAR16 *) VariableName) == 0) ) {
        //
        // Get the FormSet with the same variable.
        //
        DMSG ((EFI_D_ERROR, "\n ++++ \n"));	
        DMSG_HII(LocalFormSet->FormSetTitle, LocalFormSet->HiiHandle, L"With same Variable");	
		
        //
        //Iterate over alll question in LocalFormset to Update Value change Flag.
        //
        LinkForm = GetFirstNode (&LocalFormSet->FormListHead);
        while (!IsNull (&LocalFormSet->FormListHead, LinkForm)) {
          Form = FORM_BROWSER_FORM_FROM_LINK (LinkForm);
          LinkForm = GetNextNode (&LocalFormSet->FormListHead, LinkForm);
      
          LinkQuestion = GetFirstNode (&Form->StatementListHead);
          while (!IsNull (&Form->StatementListHead, LinkQuestion)) {
            Question = FORM_BROWSER_STATEMENT_FROM_LINK (LinkQuestion);
            LinkQuestion = GetNextNode (&Form->StatementListHead, LinkQuestion);
            if (Question->Operand == EFI_IFR_PASSWORD_OP) {
              continue;
            }

            IsQuestionValueChanged(LocalFormSet, Form, Question, GetSetValueWithBothBuffer);
          }
        }	
		
        break;
      }// Get the FormSet with the same variable.
	  
      LinkStorage = GetNextNode (&LocalFormSet->StorageListHead, LinkStorage);
    }//Look for formset with the same variable.
  }//Look for all present Formset.
  
  return;
}


/**
  This routine called this service in the browser to retrieve or set certain uncommitted
  state information that resides in the open formsets.

  @param  This                   A pointer to the EFI_FORM_BROWSER2_PROTOCOL
                                 instance.
  @param  ResultsDataSize        A pointer to the size of the buffer associated
                                 with ResultsData.
  @param  ResultsData            A string returned from an IFR browser or
                                 equivalent. The results string will have no
                                 routing information in them.
  @param  RetrieveData           A BOOLEAN field which allows an agent to retrieve
                                 (if RetrieveData = TRUE) data from the uncommitted
                                 browser state information or set (if RetrieveData
                                 = FALSE) data in the uncommitted browser state
                                 information.
  @param  VariableGuid           An optional field to indicate the target variable
                                 GUID name to use.
  @param  VariableName           An optional field to indicate the target
                                 human-readable variable name.

  @retval EFI_SUCCESS            The results have been distributed or are awaiting
                                 distribution.
  @retval EFI_BUFFER_TOO_SMALL   The ResultsDataSize specified was too small to
                                 contain the results data.

**/
EFI_STATUS
EFIAPI
BrowserCallback (
  IN CONST EFI_FORM_BROWSER2_PROTOCOL  *This,
  IN OUT UINTN                         *ResultsDataSize,
  IN OUT EFI_STRING                    ResultsData,
  IN BOOLEAN                           RetrieveData,
  IN CONST EFI_GUID                    *VariableGuid, OPTIONAL
  IN CONST CHAR16                      *VariableName  OPTIONAL
  )
{
  EFI_STATUS            Status;
  LIST_ENTRY            *Link;
  BROWSER_STORAGE       *Storage;
  FORMSET_STORAGE       *FormsetStorage;
  UINTN                 TotalSize;
  BOOLEAN               Found;
  BROWSER_STORAGE       *SavedStorage = NULL;       // wz200116 +
  BROWSER_STORAGE       *GlobalStorage = NULL;

  if (ResultsDataSize == NULL || ResultsData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  DMSG ((EFI_D_ERROR, "\n BrowserCallback, RetrieveData :%d, VariableName :%s. \n", RetrieveData, VariableName));
  TotalSize = *ResultsDataSize;
  Storage   = NULL;
  Found     = FALSE;
  Status    = EFI_SUCCESS;
	
  if (VariableGuid != NULL) {
    //
    // Try to find target storage in the current formset.
    //
    Link = GetFirstNode (&gBrowserStorageList);
    while (!IsNull (&gBrowserStorageList, Link)) {
      Storage = BROWSER_STORAGE_FROM_LINK (Link);
      Link = GetNextNode (&gBrowserStorageList, Link);

      //DMSG ((EFI_D_ERROR, "BrowserCallback, Storage :%s, Storage->HiiHandle :0x%x. \n", Storage->Name, Storage->HiiHandle));
      //
      // Check the current storage.
      //
      if (!CompareGuid (&Storage->Guid, (EFI_GUID *) VariableGuid)) {
        continue;
      }

      if (Storage->Type == EFI_HII_VARSTORE_BUFFER ||
          Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER) {
        //
        // Buffer storage require both GUID and Name
        //
        if (VariableName == NULL) {
          return EFI_NOT_FOUND;
        }

        if (StrCmp (Storage->Name, (CHAR16 *) VariableName) != 0) {
          continue;
        }
        if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER) {
          //
          // Matched EFI Buffer Storage
          //
          GlobalStorage = Storage;
        }
      }

      if (Storage->Type == EFI_HII_VARSTORE_NAME_VALUE ||
          Storage->Type == EFI_HII_VARSTORE_BUFFER) {
        if (mSystemLevelFormSet == NULL || mSystemLevelFormSet->HiiHandle == NULL) {
          return EFI_NOT_FOUND;
        }

        if (Storage->HiiHandle != mSystemLevelFormSet->HiiHandle) {
          if(SavedStorage == NULL){     // wz200116 +
            SavedStorage = Storage;     // wz200116 +
          }                             // wz200116 +
          continue;
        } else {
          //
          // BufferStorage in the same HiiHandle, then Buffer Storage is matched.
          //
          SavedStorage = Storage;
          break;
        }
      }
    }

// wz200116 + >>
    if(SavedStorage != NULL || GlobalStorage != NULL){
      Storage = GlobalStorage;
      if (SavedStorage != NULL) {
        //
        // Buffer Storage is higher priority than EFI Buffer Storage
        //
        Storage = SavedStorage;
      }
      Status = ProcessStorage (&TotalSize, &ResultsData, RetrieveData, Storage);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER) {
        ConfigRequestAdjust (Storage, ResultsData, TRUE);
      }
      Found = TRUE;
    }
// wz200116 + <<
	

    if (!Found) {
      return EFI_NOT_FOUND;
    } else {
      if (RetrieveData == 0) {
        //
        //Synchronize Value Change Flag when HiiSetBrowserData() be invoked.      
        // 
        UpdateValueChangedFlag(VariableGuid, VariableName);
      }
    }
  } else {
    //
    // GUID/Name is not specified, take the first storage in FormSet
    //
    if (mSystemLevelFormSet == NULL) {
      return EFI_NOT_READY;
    }

    //
    // Generate <ConfigResp>
    //
    Link = GetFirstNode (&mSystemLevelFormSet->StorageListHead);
    if (IsNull (&mSystemLevelFormSet->StorageListHead, Link)) {
      return EFI_UNSUPPORTED;
    }

    FormsetStorage = FORMSET_STORAGE_FROM_LINK (Link);

    Status = ProcessStorage (&TotalSize, &ResultsData, RetrieveData, FormsetStorage->BrowserStorage);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  if (RetrieveData) {
    Status = TotalSize <= *ResultsDataSize ? EFI_SUCCESS : EFI_BUFFER_TOO_SMALL;
    *ResultsDataSize = TotalSize;
  }

  return Status;

}


/**
  Callback function for SimpleTextInEx protocol install events

  @param Event           the event that is signaled.
  @param Context         not used here.

**/
VOID
EFIAPI
FormDisplayCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  if (mFormDisplay != NULL) {
    return;
  }

  gBS->LocateProtocol (
                  &gEdkiiFormDisplayEngineProtocolGuid,
                  NULL,
                  (VOID **) &mFormDisplay
                  );
}

/**
  Callback function for CustomStr protocol install events

  @param Event           the event that is signaled.
  @param Context         not used here.

**/
VOID
EFIAPI
CustomStringCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  if (mCustomString != NULL) {
    return;
  }

  gBS->LocateProtocol (
                  &gByoCustomStrProtocolGuid,
                  NULL,
                  (VOID **) &mCustomString
                  );
}

/**
  Hotkey Callback of save screenshot.

  @param[in] KeyData     A pointer to a buffer that describes the keystroke
                         information for the hot key. Its type is EFI_KEY_DATA to
                         be supported by all ConsoleIn devices.
  @param[in] Context     Context of CallBack Function

  @retval EFI_SUCCESS            Hot key is registered or unregistered.
**/
EFI_STATUS
ScreenshotCallBack (
  IN EFI_KEY_DATA  KeyData,
  IN VOID          *Context
  )
{
  SCREENSHOT_PROTOCOL            *SaveScreenshot;
  EFI_STATUS                      Status;

  DMSG ((EFI_D_ERROR, "Successfully registered hotkey key(Ctrl + s)\n"));
  Status = gBS->LocateProtocol (&gScreenshotProtocolGuid, NULL, (VOID **)&SaveScreenshot);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = SaveScreenshot->SaveScreenshot (SaveScreenshot);
  return EFI_SUCCESS;
}

/**
  This function for HotkeyRegistration

  @param[in]  Event                 Event whose notification function is being invoked.
  @param[in]  Context               The pointer to the notification function's context,
                                    which is implementation-dependent.

**/
VOID
EFIAPI
HotkeyRegistrationFunction (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
  )
{
  EFI_STATUS                             Status;
  EFI_KEY_DATA                           ScreenshotKeyData;
  EDKII_FORM_BROWSER_EXTENSION_PROTOCOL  *FormBrowseExtensionProtocol;

  ScreenshotKeyData.Key.ScanCode = 0;
   ScreenshotKeyData.Key.UnicodeChar = 's';
  ScreenshotKeyData.KeyState.KeyShiftState = EFI_LEFT_CONTROL_PRESSED | EFI_SHIFT_STATE_VALID;

  Status = gBS->LocateProtocol (
                  &gEdkiiFormBrowserExProtocolGuid,
                  NULL,
                  (VOID **) &FormBrowseExtensionProtocol
                  );
  if (!EFI_ERROR (Status)) {
    Status = FormBrowseExtensionProtocol->RegisterHotKeyFunction (
                                              ScreenshotKeyData,
                                              BROWSER_ACTION_NONE,
                                              0,
                                              ScreenshotCallBack,
                                              0,
                                              NULL
                                              );
  }
}

/**
  This callback function shows dialog

  @param[in]  Event                 The event that is signaled.
  @param[in]  Context               not used here.

**/
VOID 
EFIAPI 
TimerInSecondsCallBack (
  EFI_EVENT           Event, 
  VOID                *Context
  )
{
  CHAR16      Str1[45];
  CHAR16      Str2[11];

  ZeroMem (Str1, sizeof (Str1));
  ZeroMem (Str2, sizeof (Str2));

  SetMem16 (Str2, sizeof (Str2), L' ');
  SetMem16 (Str2, sizeof (CHAR16)*(mCount % 10), L'.');
  Str2[10] = L'\0';
  mCount ++;
  if (mRecoveryDefault) {
  UnicodeSPrint (Str1, sizeof (Str1), L"%s%s",mCustomString->StrDefaultValueRecovery,Str2);
  } else {
  UnicodeSPrint (Str1, sizeof (Str1), L"%s%s",mCustomString->StrConfigurationSaving,Str2);
  }
  UiConfirmDialog (DIALOG_NO_REPAINT, mCustomString->gStrPromptMsg, NULL, TEXT_ALIGIN_CENTER, Str1, L"", NULL);

}

/**
  Initialize Setup Browser driver.

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval EFI_SUCCESS    The Setup Browser module is initialized correctly..
  @return Other value if failed to initialize the Setup Browser module.

**/
EFI_STATUS
EFIAPI
InitializeSetup (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                  Status;
  VOID                        *Registration;
  VOID                        *CustStrRegistration;
  EFI_EVENT                   Event;
  VOID                        *EventReg;
  UINTN                       VariableSize;
  SEARCH_SAVE_DATA            SearchData;

  //
  // Locate required Hii relative protocols
  //
  Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &mHiiDatabase
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
                  &gEfiHiiConfigRoutingProtocolGuid,
                  NULL,
                  (VOID **) &mHiiConfigRouting
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathFromTextProtocolGuid,
                  NULL,
                  (VOID **) &mPathFromText
                  );

  //
  // Install FormBrowser2 protocol
  //
  mPrivateData.Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiFormBrowser2ProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.FormBrowser2
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Install FormBrowserEx2 protocol
  //
  InitializeListHead (&mPrivateData.FormBrowserEx2.FormViewHistoryHead);
  InitializeListHead (&mPrivateData.FormBrowserEx2.OverrideQestListHead);
  mPrivateData.Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEdkiiFormBrowserEx2ProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.FormBrowserEx2
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEdkiiFormBrowserExProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.FormBrowserEx
                  );
  ASSERT_EFI_ERROR (Status);

  InitializeDisplayFormData ();

  Status = gBS->LocateProtocol (
                  &gEdkiiFormDisplayEngineProtocolGuid,
                  NULL,
                  (VOID **) &mFormDisplay
                  );

  if (EFI_ERROR (Status)) {
    EfiCreateProtocolNotifyEvent (
      &gEdkiiFormDisplayEngineProtocolGuid,
      TPL_CALLBACK,
      FormDisplayCallback,
      NULL,
      &Registration
      );
  }

  Status = gBS->LocateProtocol (
                  &gByoCustomStrProtocolGuid,
                  NULL,
                  (VOID **) &mCustomString
                  );

  if (EFI_ERROR (Status)) {
    EfiCreateProtocolNotifyEvent (
      &gByoCustomStrProtocolGuid,
      TPL_CALLBACK,
      CustomStringCallback,
      NULL,
      &CustStrRegistration
      );
  }
  //
  // Install Byo FormBrowserEx protocol
  //
  mPrivateData.Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &mPrivateData.Handle,
                  &gEfiByoFormBrowserExProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mPrivateData.ByoFormBrowserEx
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Install Byo Formset Manager Protocol.
  //
  InstallByoFormsetManagerProtocol();

  //
  // Register HotKey On Setup Enter callback
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  HotkeyRegistrationFunction,
                  NULL,
                  &Event
                  );
  if (!EFI_ERROR(Status)) {
    Status = gBS->RegisterProtocolNotify (
                    &gEfiSetupEnterGuid,
                    Event,
                    &EventReg
                    );
    ASSERT_EFI_ERROR (Status);
  }

  Status = gBS->CreateEvent(
    EVT_TIMER | EVT_NOTIFY_SIGNAL,
    TPL_NOTIFY,
    TimerInSecondsCallBack,
    NULL,
    &mTimerEvent
    );
  gPrivateData.DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gPrivateData.DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mSetupSearchVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &gPrivateData.ConfigAccess,
                  NULL
                  );
  gPrivateData.HiiHandle = HiiAddPackages (
                                  &gEfiFormsetGuidSearch,
                                  gPrivateData.DriverHandle,
                                  STRING_ARRAY_NAME,
                                  SetupBrowserBin,
                                  NULL
                                  );
  ASSERT (gPrivateData.HiiHandle != NULL);
  mSetupBrowserHandle = gPrivateData.HiiHandle;

  VariableSize = sizeof(SEARCH_SAVE_DATA);
  Status = gRT->GetVariable (
                    EFI_SEARCH_VARIABLE_NAME,
                    &gEfiSetupSearchGuid,
                    NULL,
                    &VariableSize,
                    &SearchData
                    );
  if (!EFI_ERROR (Status)) {
    ZeroMem (SearchData.SearchInputData, sizeof (SearchData.SearchInputData));
    Status = gRT->SetVariable (
                      EFI_SEARCH_VARIABLE_NAME,
                      &gEfiSetupSearchGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                      VariableSize,
                      &SearchData
                    );
  }

  return EFI_SUCCESS;
}


/**
  Create a new string in HII Package List.

  @param  String                 The String to be added
  @param  HiiHandle              The package list in the HII database to insert the
                                 specified string.

  @return The output string.

**/
EFI_STRING_ID
NewString (
  IN  CHAR16                   *String,
  IN  EFI_HII_HANDLE           HiiHandle
  )
{
  EFI_STRING_ID  StringId;

  StringId = HiiSetString (HiiHandle, 0, String, NULL);
  ASSERT (StringId != 0);

  return StringId;
}


/**
  Delete a string from HII Package List.

  @param  StringId               Id of the string in HII database.
  @param  HiiHandle              The HII package list handle.

  @retval EFI_SUCCESS            The string was deleted successfully.

**/
EFI_STATUS
DeleteString (
  IN  EFI_STRING_ID            StringId,
  IN  EFI_HII_HANDLE           HiiHandle
  )
{
  CHAR16  NullChar;

  NullChar = CHAR_NULL;
  HiiSetString (HiiHandle, StringId, &NullChar, NULL);
  return EFI_SUCCESS;
}


/**
  Get the string based on the StringId and HII Package List Handle.

  @param  Token                  The String's ID.
  @param  HiiHandle              The package list in the HII database to search for
                                 the specified string.

  @return The output string.

**/
CHAR16 *
GetToken (
  IN  EFI_STRING_ID                Token,
  IN  EFI_HII_HANDLE               HiiHandle
  )
{
  EFI_STRING  String;

  if (HiiHandle == NULL) {
    return NULL;
  }

  String = HiiGetString (HiiHandle, Token, NULL);
  if (String == NULL) {
    String = HiiGetString (HiiHandle, Token, "en-US");
  }
  if (String == NULL) {
    String = AllocateCopyPool (StrSize (mUnknownString), mUnknownString);
    ASSERT (String != NULL);
  }
  return (CHAR16 *) String;
}


/**
  Allocate new memory and then copy the Unicode string Source to Destination.

  @param  Dest                   Location to copy string
  @param  Src                    String to copy

**/
VOID
NewStringCpy (
  IN OUT CHAR16       **Dest,
  IN CHAR16           *Src
  )
{
  if (*Dest != NULL) {
    FreePool (*Dest);
  }
  *Dest = AllocateCopyPool (StrSize (Src), Src);
  ASSERT (*Dest != NULL);
}


/**
  Allocate new memory and concatenate Source on the end of Destination.

  @param  Dest                   String to added to the end of.
  @param  Src                    String to concatenate.

**/
VOID
NewStringCat (
  IN OUT CHAR16       **Dest,
  IN CHAR16           *Src
  )
{
  CHAR16  *NewString;
  UINTN   MaxLen;

  if (*Dest == NULL) {
    NewStringCpy (Dest, Src);
    return;
  }

  MaxLen = ( StrSize (*Dest) + StrSize (Src) - 1) / sizeof (CHAR16);
  NewString = AllocateZeroPool (MaxLen * sizeof (CHAR16));
  ASSERT (NewString != NULL);

  StrCpyS (NewString, MaxLen, *Dest);
  StrCatS (NewString, MaxLen, Src);

  FreePool (*Dest);
  *Dest = NewString;
}

/**
  Get Value for given Name from a NameValue Storage.

  @param  Storage                The NameValue Storage.
  @param  Name                   The Name.
  @param  Value                  The returned Value.
  @param  GetValueFrom           Where to get source value, from EditValue or Value.

  @retval EFI_SUCCESS            Value found for given Name.
  @retval EFI_NOT_FOUND          No such Name found in NameValue storage.

**/
EFI_STATUS
GetValueByName (
  IN BROWSER_STORAGE             *Storage,
  IN CHAR16                      *Name,
  IN OUT CHAR16                  **Value,
  IN GET_SET_QUESTION_VALUE_WITH GetValueFrom
  )
{
  LIST_ENTRY              *Link;
  NAME_VALUE_NODE         *Node;

  if (GetValueFrom != GetSetValueWithEditBuffer && GetValueFrom != GetSetValueWithBuffer) {
    return EFI_INVALID_PARAMETER;
  }

  *Value = NULL;

  Link = GetFirstNode (&Storage->NameValueListHead);
  while (!IsNull (&Storage->NameValueListHead, Link)) {
    Node = NAME_VALUE_NODE_FROM_LINK (Link);

    if (StrCmp (Name, Node->Name) == 0) {
      if (GetValueFrom == GetSetValueWithEditBuffer) {
        NewStringCpy (Value, Node->EditValue);
      } else {
        NewStringCpy (Value, Node->Value);
      }
      return EFI_SUCCESS;
    }

    Link = GetNextNode (&Storage->NameValueListHead, Link);
  }

  return EFI_NOT_FOUND;
}


/**
  Set Value of given Name in a NameValue Storage.

  @param  Storage                The NameValue Storage.
  @param  Name                   The Name.
  @param  Value                  The Value to set.
  @param  SetValueTo             Whether update editValue or Value.
  @param  ReturnNode             The node use the input name.

  @retval EFI_SUCCESS            Value found for given Name.
  @retval EFI_NOT_FOUND          No such Name found in NameValue storage.

**/
EFI_STATUS
SetValueByName (
  IN  BROWSER_STORAGE             *Storage,
  IN  CHAR16                      *Name,
  IN  CHAR16                      *Value,
  IN  GET_SET_QUESTION_VALUE_WITH SetValueTo,
  OUT NAME_VALUE_NODE             **ReturnNode
  )
{
  LIST_ENTRY              *Link;
  NAME_VALUE_NODE         *Node;
  CHAR16                  *Buffer;

  if (SetValueTo != GetSetValueWithEditBuffer && SetValueTo != GetSetValueWithBuffer) {
    return EFI_INVALID_PARAMETER;
  }

  Link = GetFirstNode (&Storage->NameValueListHead);
  while (!IsNull (&Storage->NameValueListHead, Link)) {
    Node = NAME_VALUE_NODE_FROM_LINK (Link);

    if (StrCmp (Name, Node->Name) == 0) {
      if (SetValueTo == GetSetValueWithEditBuffer) {
        Buffer = Node->EditValue;
      } else {
        Buffer = Node->Value;
      }
      if (Buffer != NULL) {
        FreePool (Buffer);
      }
      Buffer = AllocateCopyPool (StrSize (Value), Value);
      ASSERT (Buffer != NULL);
      if (SetValueTo == GetSetValueWithEditBuffer) {
        Node->EditValue = Buffer;
      } else {
        Node->Value = Buffer;
      }

      if (ReturnNode != NULL) {
        *ReturnNode = Node;
      }

      return EFI_SUCCESS;
    }

    Link = GetNextNode (&Storage->NameValueListHead, Link);
  }

  return EFI_NOT_FOUND;
}


/**
  Convert setting of Buffer Storage or NameValue Storage to <ConfigResp>.

  @param  Storage                The Storage to be converted.
  @param  ConfigResp             The returned <ConfigResp>.
  @param  ConfigRequest          The ConfigRequest string.
  @param  GetEditBuf             Get the data from edit buffer or buffer.

  @retval EFI_SUCCESS            Convert success.
  @retval EFI_INVALID_PARAMETER  Incorrect storage type.

**/
EFI_STATUS
StorageToConfigResp (
  IN BROWSER_STORAGE         *Storage,
  IN CHAR16                  **ConfigResp,
  IN CHAR16                  *ConfigRequest,
  IN BOOLEAN                 GetEditBuf
  )
{
  EFI_STATUS              Status;
  EFI_STRING              Progress;
  LIST_ENTRY              *Link;
  NAME_VALUE_NODE         *Node;
  UINT8                   *SourceBuf;
  FORMSET_STORAGE         *FormsetStorage;

  Status = EFI_SUCCESS;
  DMSG ((EFI_D_ERROR, "StorageToConfigResp, GetEditBuf :%d, Storage :%d-%s. \n", GetEditBuf, Storage->Type, Storage->Name));
  switch (Storage->Type) {
  case EFI_HII_VARSTORE_BUFFER:
  case EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER:
    SourceBuf = GetEditBuf ? Storage->EditBuffer : Storage->Buffer;
    Status = mHiiConfigRouting->BlockToConfig (
                                  mHiiConfigRouting,
                                  ConfigRequest,
                                  SourceBuf,
                                  Storage->Size,
                                  ConfigResp,
                                  &Progress
                                  );
    DMSG ((EFI_D_ERROR, "StorageToConfigResp, BlockToConfig :%r. \n", Status));
    break;

  case EFI_HII_VARSTORE_NAME_VALUE:
    *ConfigResp = NULL;
    FormsetStorage = GetFstStgFromBrsStg(Storage);
    ASSERT (FormsetStorage != NULL);
    NewStringCat (ConfigResp, FormsetStorage->ConfigHdr);

    Link = GetFirstNode (&Storage->NameValueListHead);
    while (!IsNull (&Storage->NameValueListHead, Link)) {
      Node = NAME_VALUE_NODE_FROM_LINK (Link);

      if (StrStr (ConfigRequest, Node->Name) != NULL) {
        NewStringCat (ConfigResp, L"&");
        NewStringCat (ConfigResp, Node->Name);
        NewStringCat (ConfigResp, L"=");
        if (GetEditBuf) {
          NewStringCat (ConfigResp, Node->EditValue);
        } else {
          NewStringCat (ConfigResp, Node->Value);
        }
      }
      Link = GetNextNode (&Storage->NameValueListHead, Link);
    }
    break;

  case EFI_HII_VARSTORE_EFI_VARIABLE:
  default:
    Status = EFI_INVALID_PARAMETER;
    break;
  }

  return Status;
}


/**
  Convert <ConfigResp> to settings in Buffer Storage or NameValue Storage.

  @param  Storage                The Storage to receive the settings.
  @param  ConfigResp             The <ConfigResp> to be converted.

  @retval EFI_SUCCESS            Convert success.
  @retval EFI_INVALID_PARAMETER  Incorrect storage type.

**/
EFI_STATUS
ConfigRespToStorage (
  IN BROWSER_STORAGE         *Storage,
  IN CHAR16                  *ConfigResp
  )
{
  EFI_STATUS  Status;
  EFI_STRING  Progress;
  UINTN       BufferSize;
  CHAR16      *StrPtr;
  CHAR16      *Name;
  CHAR16      *Value;

  Status = EFI_SUCCESS;
  
  DMSG ((EFI_D_ERROR, "ConfigRespToStorage, Storage :%s. \n", Storage->Name));
  switch (Storage->Type) {
  case EFI_HII_VARSTORE_BUFFER:
  case EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER:
    BufferSize = Storage->Size;
    Status = mHiiConfigRouting->ConfigToBlock (
                                  mHiiConfigRouting,
                                  ConfigResp,
                                  Storage->EditBuffer,
                                  &BufferSize,
                                  &Progress
                                  );
    break;

  case EFI_HII_VARSTORE_NAME_VALUE:
    StrPtr = StrStr (ConfigResp, L"PATH");
    if (StrPtr == NULL) {
      break;
    }
    StrPtr = StrStr (ConfigResp, L"&");
    while (StrPtr != NULL) {
      //
      // Skip '&'
      //
      StrPtr = StrPtr + 1;
      Name = StrPtr;
      StrPtr = StrStr (StrPtr, L"=");
      if (StrPtr == NULL) {
        break;
      }
      *StrPtr = 0;

      //
      // Skip '='
      //
      StrPtr = StrPtr + 1;
      Value = StrPtr;
      StrPtr = StrStr (StrPtr, L"&");
      if (StrPtr != NULL) {
        *StrPtr = 0;
      }
      SetValueByName (Storage, Name, Value, GetSetValueWithEditBuffer, NULL);
    }
    break;

  case EFI_HII_VARSTORE_EFI_VARIABLE:
  default:
    Status = EFI_INVALID_PARAMETER;
    break;
  }

  return Status;
}

/**
  Get bit field value from the buffer and then set the value for the question.
  Note: Data type UINT32 can cover all the bit field value.

  @param  Question        The question refer to bit field.
  @param  Buffer          Point to the buffer which the question value get from.

**/
VOID
GetBitsQuestionValue (
  IN  FORM_BROWSER_STATEMENT *Question,
  IN  UINT8                  *Buffer
  )
{
  UINTN    StartBit;
  UINTN    EndBit;
  UINT32   RetVal;
  UINT32   BufferValue;

  StartBit = Question->BitVarOffset % 8;
  EndBit = StartBit + Question->BitStorageWidth - 1;

  CopyMem ((UINT8 *) &BufferValue, Buffer, Question->StorageWidth);

  RetVal = BitFieldRead32 (BufferValue, StartBit, EndBit);

  //
  // Set question value.
  // Note: Since Question with BufferValue (orderedlist, password, string)are not supported to refer bit field.
  // Only oneof/checkbox/oneof can support bit field.So we can copy the value to the Hiivalue of Question directly.
  //
  CopyMem ((UINT8 *) &Question->HiiValue.Value, (UINT8 *) &RetVal, Question->StorageWidth);
}

/**
  Set bit field value to the buffer.
  Note: Data type UINT32 can cover all the bit field value.

  @param  Question        The question refer to bit field.
  @param  Buffer          Point to the buffer which the question value set to.
  @param  Value           The bit field value need to set.

**/
VOID
SetBitsQuestionValue (
  IN     FORM_BROWSER_STATEMENT *Question,
  IN OUT UINT8                  *Buffer,
  IN     UINT32                 Value
  )
{
  UINT32   Operand;
  UINTN    StartBit;
  UINTN    EndBit;
  UINT32   RetVal;

  StartBit = Question->BitVarOffset % 8;
  EndBit = StartBit + Question->BitStorageWidth - 1;

  CopyMem ((UINT8*) &Operand, Buffer, Question->StorageWidth);

  RetVal = BitFieldWrite32 (Operand, StartBit, EndBit, Value);

  CopyMem (Buffer, (UINT8*) &RetVal, Question->StorageWidth);
}

/**
  Convert the buffer value to HiiValue.

  @param  Question               The question.
  @param  Value                  Unicode buffer save the question value.

  @retval  Status whether convert the value success.

**/
EFI_STATUS
BufferToValue (
  IN OUT FORM_BROWSER_STATEMENT           *Question,
  IN     CHAR16                           *Value
  )
{
  CHAR16                       *StringPtr;
  BOOLEAN                      IsBufferStorage;
  CHAR16                       *DstBuf;
  CHAR16                       TempChar;
  UINTN                        LengthStr;
  UINT8                        *Dst;
  CHAR16                       TemStr[5];
  UINTN                        Index;
  UINT8                        DigitUint8;
  BOOLEAN                      IsString;
  UINTN                        Length;
  EFI_STATUS                   Status;
  UINT8                        *Buffer;

  Buffer = NULL;

  IsString = (BOOLEAN) ((Question->HiiValue.Type == EFI_IFR_TYPE_STRING) ?  TRUE : FALSE);
  if (Question->Storage->Type == EFI_HII_VARSTORE_BUFFER || 
      Question->Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER) {
    IsBufferStorage = TRUE;
  } else {
    IsBufferStorage = FALSE;
  }

  //
  // Question Value is provided by Buffer Storage or NameValue Storage
  //
  if (Question->BufferValue != NULL) {
    //
    // This Question is password or orderedlist
    //
    Dst = Question->BufferValue;
  } else {
    //
    // Other type of Questions
    //
    if (Question->QuestionReferToBitField) {
      Buffer = (UINT8 *)AllocateZeroPool (Question->StorageWidth);
      if (Buffer == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      Dst = Buffer;
    } else {
      Dst = (UINT8 *) &Question->HiiValue.Value;
    }
  }

  //
  // Temp cut at the end of this section, end with '\0' or '&'.
  //
  StringPtr = Value;
  while (*StringPtr != L'\0' && *StringPtr != L'&') {
    StringPtr++;
  }
  TempChar = *StringPtr;
  *StringPtr = L'\0';

  LengthStr = StrLen (Value);

  //
  // Value points to a Unicode hexadecimal string, we need to convert the string to the value with CHAR16/UINT8...type.
  // When generating the Value string, we follow this rule: 1 byte -> 2 Unicode characters (for string: 2 byte(CHAR16) ->4 Unicode characters).
  // So the maximum value string length of a question is : Question->StorageWidth * 2.
  // If the value string length > Question->StorageWidth * 2, only set the string length as Question->StorageWidth * 2, then convert.
  //
  if (LengthStr > (UINTN) Question->StorageWidth * 2) {
    Length = (UINTN) Question->StorageWidth * 2;
  } else {
    Length = LengthStr;
  }

  Status    = EFI_SUCCESS;
  if (!IsBufferStorage && IsString) {
    //
    // Convert Config String to Unicode String, e.g "0041004200430044" => "ABCD"
    // Add string tail char L'\0' into Length
    //
    DstBuf = (CHAR16 *) Dst;
    ZeroMem (TemStr, sizeof (TemStr));
    for (Index = 0; Index < Length; Index += 4) {
      StrnCpyS (TemStr, sizeof (TemStr) / sizeof (CHAR16), Value + Index, 4);
      DstBuf[Index/4] = (CHAR16) StrHexToUint64 (TemStr);
    }
    //
    // Add tailing L'\0' character
    //
    DstBuf[Index/4] = L'\0';
  } else {
    ZeroMem (TemStr, sizeof (TemStr));
    for (Index = 0; Index < Length; Index ++) {
      TemStr[0] = Value[LengthStr - Index - 1];
      DigitUint8 = (UINT8) StrHexToUint64 (TemStr);
      if ((Index & 1) == 0) {
        Dst [Index/2] = DigitUint8;
      } else {
        Dst [Index/2] = (UINT8) ((DigitUint8 << 4) + Dst [Index/2]);
      }
    }
  }

  *StringPtr = TempChar;

  if (Buffer != NULL && Question->QuestionReferToBitField) {
    GetBitsQuestionValue (Question, Buffer);
    FreePool (Buffer);
  }

  return Status;
}

/**
  Get Question's current Value.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               Question to be initialized.
  @param  GetValueFrom           Where to get value, may from editbuffer, buffer or hii driver.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
GetQuestionValue (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN OUT FORM_BROWSER_STATEMENT       *Question,
  IN GET_SET_QUESTION_VALUE_WITH      GetValueFrom
  )
{
  EFI_STATUS          Status;
  BOOLEAN             Enabled;
  BOOLEAN             Pending;
  UINT8               *Dst;
  UINTN               StorageWidth;
  EFI_TIME            EfiTime;
  BROWSER_STORAGE     *Storage;
  FORMSET_STORAGE     *FormsetStorage;
  EFI_IFR_TYPE_VALUE  *QuestionValue;
  CHAR16              *ConfigRequest;
  CHAR16              *Progress;
  CHAR16              *Result;
  CHAR16              *Value;
  UINTN               Length;
  BOOLEAN             IsBufferStorage;
  UINTN               MaxLen;

  Status = EFI_SUCCESS;
  Value  = NULL;
  Result = NULL;

  //DMSG ((EFI_D_ERROR, "\n GetQuestionValue, GetValueFrom :%d. \n", GetValueFrom));
  //DMSG_HII(Question->Prompt, FormSet->HiiHandle, L"Prompt");
  if (GetValueFrom >= GetSetValueWithMax) {
    return EFI_INVALID_PARAMETER;
  }
  //
  // Question value is provided by an Expression, evaluate it
  //
  if (Question->ValueExpression != NULL) {
    Status = EvaluateExpression (FormSet, Form, Question->ValueExpression);
    //DMSG ((EFI_D_ERROR, "GetQuestionValue, ValueExpression :%r. \n", Status));
    if (!EFI_ERROR (Status)) {
      if (Question->ValueExpression->Result.Type == EFI_IFR_TYPE_BUFFER) {
        ASSERT (Question->HiiValue.Type == EFI_IFR_TYPE_BUFFER && Question->HiiValue.Buffer != NULL);
        if (Question->StorageWidth > Question->ValueExpression->Result.BufferLen) {
          CopyMem (Question->HiiValue.Buffer, Question->ValueExpression->Result.Buffer, Question->ValueExpression->Result.BufferLen);
          Question->HiiValue.BufferLen = Question->ValueExpression->Result.BufferLen;
        } else {
          CopyMem (Question->HiiValue.Buffer, Question->ValueExpression->Result.Buffer, Question->StorageWidth);
          Question->HiiValue.BufferLen = Question->StorageWidth;
        }
        FreePool (Question->ValueExpression->Result.Buffer);
      }
      Question->HiiValue.Type = Question->ValueExpression->Result.Type;
      CopyMem (&Question->HiiValue.Value, &Question->ValueExpression->Result.Value, sizeof (EFI_IFR_TYPE_VALUE));
    }
    return Status;
  }

  //
  // Get question value by read expression.
  //
  if (Question->ReadExpression != NULL && Form->FormType == STANDARD_MAP_FORM_TYPE) {
    Status = EvaluateExpression (FormSet, Form, Question->ReadExpression);
    //DMSG ((EFI_D_ERROR, "GetQuestionValue, ReadExpression :%r. \n", Status));
    if (!EFI_ERROR (Status) &&
      ((Question->ReadExpression->Result.Type < EFI_IFR_TYPE_OTHER) || (Question->ReadExpression->Result.Type == EFI_IFR_TYPE_BUFFER))) {
      //
      // Only update question value to the valid result.
      //
      if (Question->ReadExpression->Result.Type == EFI_IFR_TYPE_BUFFER) {
        ASSERT (Question->HiiValue.Type == EFI_IFR_TYPE_BUFFER && Question->HiiValue.Buffer != NULL);
        if (Question->StorageWidth > Question->ReadExpression->Result.BufferLen) {
          CopyMem (Question->HiiValue.Buffer, Question->ReadExpression->Result.Buffer, Question->ReadExpression->Result.BufferLen);
          Question->HiiValue.BufferLen = Question->ReadExpression->Result.BufferLen;
        } else {
          CopyMem (Question->HiiValue.Buffer, Question->ReadExpression->Result.Buffer, Question->StorageWidth);
          Question->HiiValue.BufferLen = Question->StorageWidth;
        }
        FreePool (Question->ReadExpression->Result.Buffer);
      }
      Question->HiiValue.Type = Question->ReadExpression->Result.Type;
      CopyMem (&Question->HiiValue.Value, &Question->ReadExpression->Result.Value, sizeof (EFI_IFR_TYPE_VALUE));
      return EFI_SUCCESS;
    }
  }

  //
  // Question value is provided by RTC
  //
  Storage = Question->Storage;
  QuestionValue = &Question->HiiValue.Value;
  if (Storage == NULL) {
    //
    // It's a Question without storage, or RTC date/time
    //
    if (Question->Operand == EFI_IFR_DATE_OP || Question->Operand == EFI_IFR_TIME_OP) {
      //
      // Date and time define the same Flags bit
      //
      switch (Question->Flags & EFI_QF_DATE_STORAGE) {
      case QF_DATE_STORAGE_TIME:
        Status = gRT->GetTime (&EfiTime, NULL);
        //DMSG ((EFI_D_ERROR, "GetQuestionValue, Get Time :%r. \n", Status));
        break;

      case QF_DATE_STORAGE_WAKEUP:
        Status = gRT->GetWakeupTime (&Enabled, &Pending, &EfiTime);
        break;

      case QF_DATE_STORAGE_NORMAL:
      default:
        //
        // For date/time without storage
        //
        return EFI_SUCCESS;
      }

      if (EFI_ERROR (Status)) {
        if (Question->Operand == EFI_IFR_DATE_OP){
          QuestionValue->date.Year  = 0xff;
          QuestionValue->date.Month = 0xff;
          QuestionValue->date.Day   = 0xff;
        } else {
          QuestionValue->time.Hour   = 0xff;
          QuestionValue->time.Minute = 0xff;
          QuestionValue->time.Second = 0xff;
        }
        return EFI_SUCCESS;
      }

      if (Question->Operand == EFI_IFR_DATE_OP) {
        QuestionValue->date.Year  = EfiTime.Year;
        QuestionValue->date.Month = EfiTime.Month;
        QuestionValue->date.Day   = EfiTime.Day;
      } else {
        QuestionValue->time.Hour   = EfiTime.Hour;
        QuestionValue->time.Minute = EfiTime.Minute;
        QuestionValue->time.Second = EfiTime.Second;
      }
    }

    return EFI_SUCCESS;
  }

  //
  // Question value is provided by EFI variable
  //
  StorageWidth = Question->StorageWidth;
  if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
    if (Question->BufferValue != NULL) {
      Dst = Question->BufferValue;
    } else {
      Dst = (UINT8 *) QuestionValue;
    }

    Status = gRT->GetVariable (
                     Question->VariableName,
                     &Storage->Guid,
                     NULL,
                     &StorageWidth,
                     Dst
                     );
    //DMSG ((EFI_D_ERROR, "GetQuestionValue, Get EFI Variable :%r. \n", Status));
    //
    // Always return success, even this EFI variable doesn't exist
    //
    return EFI_SUCCESS;
  }

  //
  // Question Value is provided by Buffer Storage or NameValue Storage
  //
  if (Question->BufferValue != NULL) {
    //
    // This Question is password or orderedlist
    //
    Dst = Question->BufferValue;
  } else {
    //
    // Other type of Questions
    //
    Dst = (UINT8 *) &Question->HiiValue.Value;
  }

  if (Storage->Type == EFI_HII_VARSTORE_BUFFER ||
      Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER) {
    IsBufferStorage = TRUE;
  } else {
    IsBufferStorage = FALSE;
  }
  //DMSG ((EFI_D_ERROR, "GetQuestionValue, IsBufferStorage :%d. \n", IsBufferStorage));
  if (GetValueFrom == GetSetValueWithEditBuffer || GetValueFrom == GetSetValueWithBuffer ) {
    if (IsBufferStorage) {
      if (GetValueFrom == GetSetValueWithEditBuffer) {
        //
        // Copy from storage Edit buffer
        // If the Question refer to bit filed, get the value in the related bit filed.
        //
        if (Question->QuestionReferToBitField) {
          GetBitsQuestionValue (Question, Storage->EditBuffer + Question->VarStoreInfo.VarOffset);
        } else {
        //DMSG ((EFI_D_ERROR, "GetQuestionValue, GetSetValueWithEditBuffer, \n"));
          CopyMem (Dst, Storage->EditBuffer + Question->VarStoreInfo.VarOffset, StorageWidth);
        }
      } else {
        //
        // Copy from storage Edit buffer
        // If the Question refer to bit filed, get the value in the related bit filed.
        //
        if (Question->QuestionReferToBitField) {
          GetBitsQuestionValue (Question, Storage->Buffer + Question->VarStoreInfo.VarOffset);
        } else {
        //DMSG ((EFI_D_ERROR, "GetQuestionValue, GetSetValueWithBuffer, \n"));
          CopyMem (Dst, Storage->Buffer + Question->VarStoreInfo.VarOffset, StorageWidth);
        }
      }
    } else {
      Value = NULL;
      Status = GetValueByName (Storage, Question->VariableName, &Value, GetValueFrom);
      //DMSG ((EFI_D_ERROR, "GetQuestionValue, GetValueByName :%r.\n", Status));
      if (EFI_ERROR (Status)) {
        return Status;
      }

      ASSERT (Value != NULL);
      Status = BufferToValue (Question, Value);
      FreePool (Value);
    }
  } else {
    FormsetStorage = GetFstStgFromVarId(FormSet, Question->VarStoreId);
    ASSERT (FormsetStorage != NULL);
    //
    // <ConfigRequest> ::= <ConfigHdr> + <BlockName> ||
    //                   <ConfigHdr> + "&" + <VariableName>
    //
    if (IsBufferStorage) {
      Length = StrLen (FormsetStorage->ConfigHdr);
      Length += StrLen (Question->BlockName);
    } else {
      Length = StrLen (FormsetStorage->ConfigHdr);
      Length += StrLen (Question->VariableName) + 1;
    }
    // Allocate buffer include '\0'
    MaxLen = Length + 1;
    ConfigRequest = AllocateZeroPool (MaxLen * sizeof (CHAR16));
    ASSERT (ConfigRequest != NULL);

    StrCpyS (ConfigRequest, MaxLen, FormsetStorage->ConfigHdr);
    if (IsBufferStorage) {
      StrCatS (ConfigRequest, MaxLen, Question->BlockName);
    } else {
      StrCatS (ConfigRequest, MaxLen, L"&");
      StrCatS (ConfigRequest, MaxLen, Question->VariableName);
    }

    //
    // Request current settings from Configuration Driver
    //
    Status = mHiiConfigRouting->ExtractConfig (
                                      mHiiConfigRouting,
                                      ConfigRequest,
                                      &Progress,
                                      &Result
                                      );
    //DMSG ((EFI_D_ERROR, "GetQuestionValue, ExtractConfig :%r. \n", Status));
    FreePool (ConfigRequest);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Skip <ConfigRequest>
    //
    if (IsBufferStorage) {
      Value = StrStr (Result, L"&VALUE");
      if (Value == NULL) {
        FreePool (Result);
        return EFI_NOT_FOUND;
      }
      //
      // Skip "&VALUE"
      //
      Value = Value + 6;
    } else {
      Value = Result + Length;
    }
    if (*Value != '=') {
      FreePool (Result);
      return EFI_NOT_FOUND;
    }
    //
    // Skip '=', point to value
    //
    Value = Value + 1;

    Status = BufferToValue (Question, Value);
    if (EFI_ERROR (Status)) {
      FreePool (Result);
      return Status;
    }

    //
    // Synchronize Edit Buffer
    //
    //DMSG ((EFI_D_ERROR, "GetQuestionValue, Synchronize Edit Buffer, \n"));
    if (IsBufferStorage) {
      CopyMem (Storage->EditBuffer + Question->VarStoreInfo.VarOffset, Dst, StorageWidth);
    } else {
      SetValueByName (Storage, Question->VariableName, Value, GetSetValueWithEditBuffer, NULL);
    }

    if (Result != NULL) {
      FreePool (Result);
    }
  }

  return Status;
}


/**
  Save Question Value to edit copy(cached) or Storage(uncached).

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               Pointer to the Question.
  @param  SetValueTo             Update the question value to editbuffer , buffer or hii driver.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
SetQuestionValue (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN OUT FORM_BROWSER_STATEMENT       *Question,
  IN GET_SET_QUESTION_VALUE_WITH      SetValueTo
  )
{
  EFI_STATUS          Status;
  BOOLEAN             Enabled;
  BOOLEAN             Pending;
  UINT8               *Src;
  EFI_TIME            EfiTime;
  UINTN               BufferLen;
  UINTN               StorageWidth;
  BROWSER_STORAGE     *Storage;
  FORMSET_STORAGE     *FormsetStorage;
  EFI_IFR_TYPE_VALUE  *QuestionValue;
  CHAR16              *ConfigResp;
  CHAR16              *Progress;
  CHAR16              *Value;
  UINTN               Length;
  BOOLEAN             IsBufferStorage;
  BOOLEAN             IsString;
  UINT8               *TemBuffer;
  CHAR16              *TemName;
  CHAR16              *TemString;
  UINTN               Index;
  NAME_VALUE_NODE     *Node;
  UINTN               MaxLen;

  Status = EFI_SUCCESS;
  Node   = NULL;

  //DMSG_HII(Question->Prompt, FormSet->HiiHandle, L"Prompt");
  if (SetValueTo >= GetSetValueWithMax) {
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // If Question value is provided by an Expression, then it is read only
  //
  if (Question->ValueExpression != NULL) {
    return Status;
  }

  //
  // Before set question value, evaluate its write expression.
  //
  if (Question->WriteExpression != NULL && Form->FormType == STANDARD_MAP_FORM_TYPE) {
    Status = EvaluateExpression (FormSet, Form, Question->WriteExpression);
    //DMSG ((EFI_D_ERROR, "SetQuestionValue, WriteExpression :%r. \n", Status));
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Question value is provided by RTC
  //
  Storage = Question->Storage;
  QuestionValue = &Question->HiiValue.Value;
  if (Storage == NULL) {
    //
    // It's a Question without storage, or RTC date/time
    //
    if (Question->Operand == EFI_IFR_DATE_OP || Question->Operand == EFI_IFR_TIME_OP) {
      //
      // Date and time define the same Flags bit
      //
      switch (Question->Flags & EFI_QF_DATE_STORAGE) {
      case QF_DATE_STORAGE_TIME:
        Status = gRT->GetTime (&EfiTime, NULL);
        break;

      case QF_DATE_STORAGE_WAKEUP:
        Status = gRT->GetWakeupTime (&Enabled, &Pending, &EfiTime);
        break;

      case QF_DATE_STORAGE_NORMAL:
      default:
        //
        // For date/time without storage
        //
        return EFI_SUCCESS;
      }

      if (EFI_ERROR (Status)) {
        return Status;
      }

      if (Question->Operand == EFI_IFR_DATE_OP) {
        EfiTime.Year  = QuestionValue->date.Year;
        EfiTime.Month = QuestionValue->date.Month;
        EfiTime.Day   = QuestionValue->date.Day;
      } else {
        EfiTime.Hour   = QuestionValue->time.Hour;
        EfiTime.Minute = QuestionValue->time.Minute;
        EfiTime.Second = QuestionValue->time.Second;
      }

      if ((Question->Flags & EFI_QF_DATE_STORAGE) == QF_DATE_STORAGE_TIME) {
        Status = gRT->SetTime (&EfiTime);
        //DMSG ((EFI_D_ERROR, "SetQuestionValue, SetTime :%r. \n", Status));
      } else {
        Status = gRT->SetWakeupTime (TRUE, &EfiTime);
      }
    }

    return Status;
  }

  //
  // Question value is provided by EFI variable
  //
  StorageWidth = Question->StorageWidth;
  if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
    if (Question->BufferValue != NULL) {
      Src = Question->BufferValue;
    } else {
      Src = (UINT8 *) QuestionValue;
    }

    Status = gRT->SetVariable (
                     Question->VariableName,
                     &Storage->Guid,
                     Storage->Attributes,
                     StorageWidth,
                     Src
                     );
    //DMSG ((EFI_D_ERROR, "SetQuestionValue, Set EFI Variable :%s-%r. \n", Question->VariableName, Status));
    return Status;
  }

  //
  // Question Value is provided by Buffer Storage or NameValue Storage
  //
  if (Question->BufferValue != NULL) {
    Src = Question->BufferValue;
  } else {
    Src = (UINT8 *) &Question->HiiValue.Value;
  }

  if (Storage->Type == EFI_HII_VARSTORE_BUFFER ||
      Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER) {
    IsBufferStorage = TRUE;
  } else {
    IsBufferStorage = FALSE;
  }
  IsString = (BOOLEAN) ((Question->HiiValue.Type == EFI_IFR_TYPE_STRING) ?  TRUE : FALSE);
  //DMSG ((EFI_D_ERROR, "SetQuestionValue, IsBufferStorage :%d, IsString :%d. \n", IsBufferStorage, IsString));
  if (SetValueTo == GetSetValueWithEditBuffer || SetValueTo == GetSetValueWithBuffer) {
    if (IsBufferStorage) {
      if (SetValueTo == GetSetValueWithEditBuffer) {
        //
        // Copy to storage edit buffer
        // If the Question refer to bit filed, copy the value in related bit filed to storage edit buffer.
        //
        if (Question->QuestionReferToBitField) {
          SetBitsQuestionValue (Question, Storage->EditBuffer + Question->VarStoreInfo.VarOffset, (UINT32)(*Src));
        } else {
        //DMSG ((EFI_D_ERROR, "SetQuestionValue, GetSetValueWithEditBuffer, \n"));
          CopyMem (Storage->EditBuffer + Question->VarStoreInfo.VarOffset, Src, StorageWidth);
        }
      } else if (SetValueTo == GetSetValueWithBuffer) {
        //
        // Copy to storage buffer
        // If the Question refer to bit filed, copy the value in related bit filed to storage buffer.
        //
        if (Question->QuestionReferToBitField) {
          SetBitsQuestionValue (Question, Storage->Buffer + Question->VarStoreInfo.VarOffset, (UINT32)(*Src));
        } else {
        //DMSG ((EFI_D_ERROR, "SetQuestionValue, GetSetValueWithBuffer, \n"));
          CopyMem (Storage->Buffer + Question->VarStoreInfo.VarOffset, Src, StorageWidth);
        }
      }
    } else {
      if (IsString) {
        //
        // Allocate enough string buffer.
        //
        Value = NULL;
        BufferLen = ((StrLen ((CHAR16 *) Src) * 4) + 1) * sizeof (CHAR16);
        Value = AllocateZeroPool (BufferLen);
        ASSERT (Value != NULL);
        //
        // Convert Unicode String to Config String, e.g. "ABCD" => "0041004200430044"
        //
        TemName = (CHAR16 *) Src;
        TemString = Value;
        for (; *TemName != L'\0'; TemName++) {
          UnicodeValueToStringS (
            TemString,
            BufferLen - ((UINTN)TemString - (UINTN)Value),
            PREFIX_ZERO | RADIX_HEX,
            *TemName,
            4
            );
          TemString += StrnLenS (TemString, (BufferLen - ((UINTN)TemString - (UINTN)Value)) / sizeof (CHAR16));
        }
      } else {
        BufferLen = StorageWidth * 2 + 1;
        Value = AllocateZeroPool (BufferLen * sizeof (CHAR16));
        ASSERT (Value != NULL);
        //
        // Convert Buffer to Hex String
        //
        TemBuffer = Src + StorageWidth - 1;
        TemString = Value;
        for (Index = 0; Index < StorageWidth; Index ++, TemBuffer --) {
          UnicodeValueToStringS (
            TemString,
            BufferLen * sizeof (CHAR16) - ((UINTN)TemString - (UINTN)Value),
            PREFIX_ZERO | RADIX_HEX,
            *TemBuffer,
            2
            );
          TemString += StrnLenS (TemString, BufferLen - ((UINTN)TemString - (UINTN)Value) / sizeof (CHAR16));
        }
      }

      Status = SetValueByName (Storage, Question->VariableName, Value, SetValueTo, &Node);
      //DMSG ((EFI_D_ERROR, "SetQuestionValue, SetValueByName :%r. \n", Status));
      FreePool (Value);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  } else if (SetValueTo == GetSetValueWithHiiDriver) {
    //
    // <ConfigResp> ::= <ConfigHdr> + <BlockName> + "&VALUE=" + "<HexCh>StorageWidth * 2" ||
    //                <ConfigHdr> + "&" + <VariableName> + "=" + "<string>"
    //
    if (IsBufferStorage) {
      Length = StrLen (Question->BlockName) + 7;
    } else {
      Length = StrLen (Question->VariableName) + 2;
    }
    if (!IsBufferStorage && IsString) {
      Length += (StrLen ((CHAR16 *) Src) * 4);
    } else {
      Length += (StorageWidth * 2);
    }
    FormsetStorage = GetFstStgFromVarId(FormSet, Question->VarStoreId);
    ASSERT (FormsetStorage != NULL);
    MaxLen = StrLen (FormsetStorage->ConfigHdr) + Length + 1;
    ConfigResp = AllocateZeroPool (MaxLen * sizeof (CHAR16));
    ASSERT (ConfigResp != NULL);

    StrCpyS (ConfigResp, MaxLen, FormsetStorage->ConfigHdr);
    if (IsBufferStorage) {
      StrCatS (ConfigResp, MaxLen, Question->BlockName);
      StrCatS (ConfigResp, MaxLen, L"&VALUE=");
    } else {
      StrCatS (ConfigResp, MaxLen, L"&");
      StrCatS (ConfigResp, MaxLen, Question->VariableName);
      StrCatS (ConfigResp, MaxLen, L"=");
    }

    Value = ConfigResp + StrLen (ConfigResp);

    if (!IsBufferStorage && IsString) {
      //
      // Convert Unicode String to Config String, e.g. "ABCD" => "0041004200430044"
      //
      TemName = (CHAR16 *) Src;
      TemString = Value;
      for (; *TemName != L'\0'; TemName++) {
        UnicodeValueToStringS (
          TemString,
          MaxLen * sizeof (CHAR16) - ((UINTN)TemString - (UINTN)ConfigResp),
          PREFIX_ZERO | RADIX_HEX,
          *TemName,
          4
          );
        TemString += StrnLenS (TemString, MaxLen - ((UINTN)TemString - (UINTN)ConfigResp) / sizeof (CHAR16));
      }
    } else {
      //
      // Convert Buffer to Hex String
      //
      TemBuffer = Src + StorageWidth - 1;
      TemString = Value;
      for (Index = 0; Index < StorageWidth; Index ++, TemBuffer --) {
        UnicodeValueToStringS (
          TemString,
          MaxLen * sizeof (CHAR16) - ((UINTN)TemString - (UINTN)ConfigResp),
          PREFIX_ZERO | RADIX_HEX,
          *TemBuffer,
          2
          );
        TemString += StrnLenS (TemString, MaxLen - ((UINTN)TemString - (UINTN)ConfigResp) / sizeof (CHAR16));
      }
    }

    //
    // Convert to lower char.
    //
    for (TemString = Value; *Value != L'\0'; Value++) {
      if (*Value >= L'A' && *Value <= L'Z') {
        *Value = (CHAR16) (*Value - L'A' + L'a');
      }
    }

    //
    // Submit Question Value to Configuration Driver
    //
    Status = mHiiConfigRouting->RouteConfig (
                                      mHiiConfigRouting,
                                      ConfigResp,
                                      &Progress
                                      );
    //DMSG ((EFI_D_ERROR, "SetQuestionValue, RouteConfig :%r. \n", Status));
    if (EFI_ERROR (Status)) {
      FreePool (ConfigResp);
      return Status;
    }
    FreePool (ConfigResp);

    //
    // Sync storage, from editbuffer to buffer.
    //
    //DMSG ((EFI_D_ERROR, "SetQuestionValue, Synchronize from editbuffer to buffer, \n"));
    CopyMem (Storage->Buffer + Question->VarStoreInfo.VarOffset, Src, StorageWidth);
  }

  return Status;
}


/**
  Perform nosubmitif check for a Form.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               The Question to be validated.
  @param  Type                   Validation type: NoSubmit

  @retval EFI_SUCCESS            Form validation pass.
  @retval other                  Form validation failed.

**/
EFI_STATUS
ValidateQuestion (
  IN  FORM_BROWSER_FORMSET            *FormSet,
  IN  FORM_BROWSER_FORM               *Form,
  IN  FORM_BROWSER_STATEMENT          *Question,
  IN  UINTN                           Type
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  LIST_ENTRY              *ListHead;
  FORM_EXPRESSION         *Expression;
  UINT32                  BrowserStatus;
  CHAR16                  *ErrorStr;

  BrowserStatus = BROWSER_SUCCESS;
  ErrorStr      = NULL;

  switch (Type) {
  case EFI_HII_EXPRESSION_INCONSISTENT_IF:
    ListHead = &Question->InconsistentListHead;
    break;

  case EFI_HII_EXPRESSION_WARNING_IF:
    ListHead = &Question->WarningListHead;
    break;

  case EFI_HII_EXPRESSION_NO_SUBMIT_IF:
    ListHead = &Question->NoSubmitListHead;
    break;

  default:
    ASSERT (FALSE);
    return EFI_UNSUPPORTED;
  }

  Link = GetFirstNode (ListHead);
  while (!IsNull (ListHead, Link)) {
    Expression = FORM_EXPRESSION_FROM_LINK (Link);

    //
    // Evaluate the expression
    //
    Status = EvaluateExpression (FormSet, Form, Expression);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if (IsTrue (&Expression->Result)) {
      switch (Type) {
      case EFI_HII_EXPRESSION_INCONSISTENT_IF:
        BrowserStatus = BROWSER_INCONSISTENT_IF;
        break;

      case EFI_HII_EXPRESSION_WARNING_IF:
        BrowserStatus = BROWSER_WARNING_IF;
        break;

      case EFI_HII_EXPRESSION_NO_SUBMIT_IF:
        BrowserStatus = BROWSER_NO_SUBMIT_IF;
        //
        // This code only used to compatible with old display engine,
        // New display engine will not use this field.
        //
        if (Expression->Error != 0) {
          ErrorStr = GetToken (Expression->Error, FormSet->HiiHandle);
        }
        break;

      default:
        ASSERT (FALSE);
        break;
      }

      if (!((Type == EFI_HII_EXPRESSION_NO_SUBMIT_IF) && mSystemSubmit)) {
        //
        // If in system submit process and for no_submit_if check, not popup this error message.
        // Will process this fail again later in not system submit process.
        //
        PopupErrorMessage(BrowserStatus, FormSet->HiiHandle, Expression->OpCode, ErrorStr);
      }

      if (ErrorStr != NULL) {
        FreePool (ErrorStr);
      }

      if (Type == EFI_HII_EXPRESSION_WARNING_IF) {
        return EFI_SUCCESS;
      } else {
        return EFI_NOT_READY;
      }
    }

    Link = GetNextNode (ListHead, Link);
  }

  return EFI_SUCCESS;
}

/**
  Perform question check.

  If one question has more than one check, process form high priority to low.
  Only one error info will be popup.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               The Question to be validated.

  @retval EFI_SUCCESS            Form validation pass.
  @retval other                  Form validation failed.

**/
EFI_STATUS
ValueChangedValidation (
  IN  FORM_BROWSER_FORMSET            *FormSet,
  IN  FORM_BROWSER_FORM               *Form,
  IN  FORM_BROWSER_STATEMENT          *Question
  )
{
  EFI_STATUS   Status;

  Status = EFI_SUCCESS;

  //
  // Do the inconsistentif check.
  //
  if (!IsListEmpty (&Question->InconsistentListHead)) {
    Status = ValidateQuestion (FormSet, Form, Question, EFI_HII_EXPRESSION_INCONSISTENT_IF);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  //
  // Do the warningif check.
  //
  if (!IsListEmpty (&Question->WarningListHead)) {
    Status = ValidateQuestion (FormSet, Form, Question, EFI_HII_EXPRESSION_WARNING_IF);
  }

  return Status;
}

/**
  Perform NoSubmit check for each Form in FormSet.

  @param  FormSet                FormSet data structure.
  @param  CurrentForm            Current input form data structure.
  @param  Statement              The statement for this check.

  @retval EFI_SUCCESS            Form validation pass.
  @retval other                  Form validation failed.

**/
EFI_STATUS
NoSubmitCheck (
  IN      FORM_BROWSER_FORMSET            *FormSet,
  IN OUT  FORM_BROWSER_FORM               **CurrentForm,
  OUT     FORM_BROWSER_STATEMENT          **Statement
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  FORM_BROWSER_STATEMENT  *Question;
  FORM_BROWSER_FORM       *Form;
  LIST_ENTRY              *LinkForm;

  LinkForm = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, LinkForm)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (LinkForm);
    LinkForm = GetNextNode (&FormSet->FormListHead, LinkForm);

    if (*CurrentForm != NULL && *CurrentForm != Form) {
      continue;
    }

    Link = GetFirstNode (&Form->StatementListHead);
    while (!IsNull (&Form->StatementListHead, Link)) {
      Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);
      Status = ValidateQuestion (FormSet, Form, Question, EFI_HII_EXPRESSION_NO_SUBMIT_IF);
      if (EFI_ERROR (Status)) {
        if (*CurrentForm == NULL) {
          *CurrentForm = Form;
        }
        if (Statement != NULL) {
          *Statement = Question;
        }
        return Status;
      }

      Link = GetNextNode (&Form->StatementListHead, Link);
    }
  }

  return EFI_SUCCESS;
}

/**
  Fill storage's edit copy with settings requested from Configuration Driver.

  @param  Storage                The storage which need to sync.
  @param  ConfigRequest          The config request string which used to sync storage.
  @param  SyncOrRestore          Sync the buffer to editbuffer or Restore  the
                                 editbuffer to buffer
                                 if TRUE, copy the editbuffer to the buffer.
                                 if FALSE, copy the buffer to the editbuffer.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
SynchronizeStorage (
  OUT BROWSER_STORAGE             *Storage,
  IN  CHAR16                      *ConfigRequest,
  IN  BOOLEAN                     SyncOrRestore
  )
{
  EFI_STATUS              Status;
  EFI_STRING              Progress;
  EFI_STRING              Result;
  UINTN                   BufferSize;
  LIST_ENTRY              *Link;
  NAME_VALUE_NODE         *Node;
  UINT8                   *Src;
  UINT8                   *Dst;

  Status = EFI_SUCCESS;
  Result = NULL;

  DMSG ((EFI_D_ERROR, "SynchronizeStorage, Storage :%s, SyncOrRestore :%d. \n", Storage->Name, SyncOrRestore));
  if (Storage->Type == EFI_HII_VARSTORE_BUFFER ||
      (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER)) {
    BufferSize = Storage->Size;

    if (SyncOrRestore) {
      Src = Storage->EditBuffer;
      Dst = Storage->Buffer;
    } else {
      Src = Storage->Buffer;
      Dst = Storage->EditBuffer;
    }

    if (ConfigRequest != NULL) {
      Status = mHiiConfigRouting->BlockToConfig(
                                    mHiiConfigRouting,
                                    ConfigRequest,
                                    Src,
                                    BufferSize,
                                    &Result,
                                    &Progress
                                    );
      if (EFI_ERROR (Status)) {
        return Status;
      }

      Status = mHiiConfigRouting->ConfigToBlock (
                                    mHiiConfigRouting,
                                    Result,
                                    Dst,
                                    &BufferSize,
                                    &Progress
                                    );
      if (Result != NULL) {
        FreePool (Result);
      }
    } else {
      CopyMem (Dst, Src, BufferSize);
    }
  } else if (Storage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    Link = GetFirstNode (&Storage->NameValueListHead);
    while (!IsNull (&Storage->NameValueListHead, Link)) {
      Node = NAME_VALUE_NODE_FROM_LINK (Link);

      if ((ConfigRequest != NULL && StrStr (ConfigRequest, Node->Name) != NULL) ||
          (ConfigRequest == NULL)) {
        if (SyncOrRestore) {
          NewStringCpy (&Node->Value, Node->EditValue);
        } else {
          NewStringCpy (&Node->EditValue, Node->Value);
        }
      }

      Link = GetNextNode (&Storage->NameValueListHead, Link);
    }
  }

  return Status;
}

/**
  When discard the question value, call the callback function with Changed type
  to inform the hii driver.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.

**/
VOID
SendDiscardInfoToDriver (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form
  )
{
  LIST_ENTRY                  *Link;
  FORM_BROWSER_STATEMENT      *Question;
  EFI_IFR_TYPE_VALUE          *TypeValue;
  EFI_BROWSER_ACTION_REQUEST  ActionRequest;

  if (FormSet->ConfigAccess == NULL) {
    return;
  }
  
  //DMSG ((EFI_D_ERROR, "\n SendDiscardInfoToDriver, \n"));
  Link = GetFirstNode (&Form->StatementListHead);
  while (!IsNull (&Form->StatementListHead, Link)) {
    Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);
    Link = GetNextNode (&Form->StatementListHead, Link);

    if (Question->Storage == NULL || Question->Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
      continue;
    }

    if ((Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) != EFI_IFR_FLAG_CALLBACK) {
      continue;
    }

    if (Question->Operand == EFI_IFR_PASSWORD_OP) {
      continue;
    }

    if (!Question->ValueChanged) {
      continue;
    }

    //
    // Restore the question value before call the CHANGED callback type.
    //
    GetQuestionValue (FormSet, Form, Question, GetSetValueWithEditBuffer);

    if (Question->Operand == EFI_IFR_STRING_OP){
      HiiSetString (FormSet->HiiHandle, Question->HiiValue.Value.string, (CHAR16*)Question->BufferValue, NULL);
    }

    if (Question->HiiValue.Type == EFI_IFR_TYPE_BUFFER) {
      TypeValue = (EFI_IFR_TYPE_VALUE *) Question->BufferValue;
    } else {
      TypeValue = &Question->HiiValue.Value;
    }

    ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    FormSet->ConfigAccess->Callback (
                             FormSet->ConfigAccess,
                             EFI_BROWSER_ACTION_CHANGED,
                             Question->QuestionId,
                             Question->HiiValue.Type,
                             TypeValue,
                             &ActionRequest
                             );
  }
}

/**
  When submit the question value, call the callback function with Submitted type
  to inform the hii driver.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.

**/
VOID
SubmitCallbackForForm (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form
  )
{
  LIST_ENTRY                  *Link;
  FORM_BROWSER_STATEMENT      *Question;
  EFI_IFR_TYPE_VALUE          *TypeValue;
  EFI_BROWSER_ACTION_REQUEST  ActionRequest;

  if (FormSet->ConfigAccess == NULL) {
    return;
  }

  Link = GetFirstNode (&Form->StatementListHead);
  while (!IsNull (&Form->StatementListHead, Link)) {
    Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);
    Link = GetNextNode (&Form->StatementListHead, Link);

    if (Question->Storage == NULL || Question->Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
      continue;
    }

    if ((Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) != EFI_IFR_FLAG_CALLBACK) {
       continue;
    }

    if (Question->Operand == EFI_IFR_PASSWORD_OP) {
       DMSG ((EFI_D_ERROR, "SubmitCallbackForForm, Only Return for Password. \n"));
       continue;
    }

    if (Question->HiiValue.Type == EFI_IFR_TYPE_BUFFER) {
      TypeValue = (EFI_IFR_TYPE_VALUE *) Question->BufferValue;
    } else {
      TypeValue = &Question->HiiValue.Value;
    }

    ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    FormSet->ConfigAccess->Callback (
                             FormSet->ConfigAccess,
                             EFI_BROWSER_ACTION_SUBMITTED,
                             Question->QuestionId,
                             Question->HiiValue.Type,
                             TypeValue,
                             &ActionRequest
                             );
  }
}

/**
  When value set Success, call the submit callback function.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.

**/
VOID
SubmitCallback (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form
  )
{
  FORM_BROWSER_FORM       *CurrentForm;
  LIST_ENTRY              *Link;

  if (Form != NULL) {
    SubmitCallbackForForm(FormSet, Form);
    return;
  }

  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    CurrentForm = FORM_BROWSER_FORM_FROM_LINK (Link);
    Link = GetNextNode (&FormSet->FormListHead, Link);

    SubmitCallbackForForm(FormSet, CurrentForm);
  }
}

/**
  Validate the HiiHandle.

  @param  HiiHandle              The input HiiHandle which need to validate.

  @retval TRUE                   The handle is validate.
  @retval FALSE                  The handle is invalidate.

**/
BOOLEAN
ValidateHiiHandle (
  EFI_HII_HANDLE          HiiHandle
  )
{
  EFI_HII_HANDLE          *HiiHandles;
  UINTN                   Index;
  BOOLEAN                 Find;

  if (HiiHandle == NULL) {
    return FALSE;
  }

  Find = FALSE;

  HiiHandles = HiiGetHiiHandles (NULL);
  ASSERT (HiiHandles != NULL);

  for (Index = 0; HiiHandles[Index] != NULL; Index++) {
    if (HiiHandles[Index] == HiiHandle) {
      Find = TRUE;
      break;
    }
  }

  FreePool (HiiHandles);

  return Find;
}

/**
  Validate the FormSet. If the formset is not validate, remove it from the list.

  @param  FormSet                The input FormSet which need to validate.

  @retval TRUE                   The handle is validate.
  @retval FALSE                  The handle is invalidate.

**/
BOOLEAN
ValidateFormSet (
  FORM_BROWSER_FORMSET    *FormSet
  )
{
  BOOLEAN  Find;

  ASSERT (FormSet != NULL);

  Find = ValidateHiiHandle(FormSet->HiiHandle);
  //
  // Should not remove the formset which is being used.
  //
  if (!Find && ((gCurrentSelection == NULL) || (FormSet != gCurrentSelection->FormSet))) {
    CleanBrowserStorage(FormSet);
    RemoveEntryList (&FormSet->Link);
    DestroyFormSet (FormSet);
  }
  
  //DMSG ((EFI_D_ERROR, "ValidateFormSet, Find :%d. \n", Find));
  return Find;
}
/**
  Check whether need to enable the reset flag in form level.
  Also clean all ValueChanged flag in question.

  @param  SetFlag                Whether need to set the Reset Flag.
  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.

**/
VOID
UpdateFlagForForm (
  IN BOOLEAN                          SetFlag,
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form
  )
{
  LIST_ENTRY              *Link;
  FORM_BROWSER_STATEMENT  *Question;
  BOOLEAN                 OldValue;

  Form->ValueChanged = FALSE;
  Link = GetFirstNode (&Form->StatementListHead);
  while (!IsNull (&Form->StatementListHead, Link)) {
    Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);
    Link = GetNextNode (&Form->StatementListHead, Link);

    if (!Question->ValueChanged) {
      continue;
    }

    OldValue = Question->ValueChanged;

    //
    // Compare the buffer and editbuffer data to see whether the data has been saved.
    //
    Question->ValueChanged = IsQuestionValueChanged(FormSet, Form, Question, GetSetValueWithBothBuffer);

    //
    // Only the changed data has been saved, then need to set the reset flag.
    //
    if (SetFlag && OldValue && !Question->ValueChanged) {
      if ((Question->QuestionFlags & EFI_IFR_FLAG_RESET_REQUIRED) != 0) {
        DMSG_HII(Question->Prompt, FormSet->HiiHandle, L"Set Reset Flag");
        gResetRequired = TRUE;
      }

      if ((Question->QuestionFlags & EFI_IFR_FLAG_RECONNECT_REQUIRED) != 0) {
        gFlagReconnect = TRUE;
      }
    } 
  }
}

/**
  Check whether need to enable the reset flag.
  Also clean ValueChanged flag for all statements.

  Form level or formset level, only one.

  @param  SetFlag                Whether need to set the Reset Flag.
  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.

**/
VOID
ValueChangeResetFlagUpdate (
  IN BOOLEAN                          SetFlag,
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form
  )
{
  FORM_BROWSER_FORM       *CurrentForm;
  LIST_ENTRY              *Link;

  DMSG_HII(FormSet->FormSetTitle, FormSet->HiiHandle, L"FormSetTitle");
  if (Form != NULL) {
    UpdateFlagForForm(SetFlag, FormSet, Form);
    return;
  }

  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    CurrentForm = FORM_BROWSER_FORM_FROM_LINK (Link);
    Link = GetNextNode (&FormSet->FormListHead, Link);

    UpdateFlagForForm(SetFlag, FormSet, CurrentForm);
  }
}

/**
  Base on the return Progress string to find the form.

  Base on the first return Offset/Width (Name) string to find the form
  which keep this string.

  @param  FormSet                FormSet data structure.
  @param  Storage                Storage which has this Progress string.
  @param  Progress               The Progress string which has the first fail string.
  @param  RetForm                The return form for this progress string.
  @param  RetQuestion            The return question for the error progress string.

  @retval TRUE                   Find the error form and statement for this error progress string.
  @retval FALSE                  Not find the error form.

**/
BOOLEAN
FindQuestionFromProgress (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN BROWSER_STORAGE                  *Storage,
  IN EFI_STRING                       Progress,
  OUT FORM_BROWSER_FORM               **RetForm,
  OUT FORM_BROWSER_STATEMENT          **RetQuestion
  )
{
  LIST_ENTRY                   *Link;
  LIST_ENTRY                   *LinkStorage;
  LIST_ENTRY                   *LinkStatement;
  FORM_BROWSER_CONFIG_REQUEST  *ConfigInfo;
  FORM_BROWSER_FORM            *Form;
  EFI_STRING                   EndStr;
  FORM_BROWSER_STATEMENT       *Statement;

  DMSG ((EFI_D_ERROR, "\n FindQuestionFromProgress, \n"));
  ASSERT ((*Progress == '&') || (*Progress == 'G'));

  ConfigInfo   = NULL;
  *RetForm     = NULL;
  *RetQuestion = NULL;

  //
  // Skip the first "&" or the ConfigHdr part.
  //
  if (*Progress == '&') {
    Progress++;
  } else {
    //
    // Prepare the "NAME" or "OFFSET=0x####&WIDTH=0x####" string.
    //
    if (Storage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
      //
      // For Name/Value type, Skip the ConfigHdr part.
      //
      EndStr = StrStr (Progress, L"PATH=");
      ASSERT (EndStr != NULL);
      while (*EndStr != '&') {
        EndStr++;
      }

      *EndStr = '\0';
    } else {
      //
      // For Buffer type, Skip the ConfigHdr part.
      //
      EndStr = StrStr (Progress, L"&OFFSET=");
      ASSERT (EndStr != NULL);
      *EndStr = '\0';
    }

    Progress = EndStr + 1;
  }

  //
  // Prepare the "NAME" or "OFFSET=0x####&WIDTH=0x####" string.
  //
  if (Storage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    //
    // For Name/Value type, the data is "&Fred=16&George=16&Ron=12" formset,
    // here, just keep the "Fred" string.
    //
    EndStr = StrStr (Progress, L"=");
    ASSERT (EndStr != NULL);
    *EndStr = '\0';
  } else {
    //
    // For Buffer type, the data is "OFFSET=0x####&WIDTH=0x####&VALUE=0x####",
    // here, just keep the "OFFSET=0x####&WIDTH=0x####" string.
    //
    EndStr = StrStr (Progress, L"&VALUE=");
    ASSERT (EndStr != NULL);
    *EndStr = '\0';
  }

  //
  // Search in the form list.
  //
  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);
    Link = GetNextNode (&FormSet->FormListHead, Link);

    //
    // Search in the ConfigRequest list in this form.
    //
    LinkStorage = GetFirstNode (&Form->ConfigRequestHead);
    while (!IsNull (&Form->ConfigRequestHead, LinkStorage)) {
      ConfigInfo = FORM_BROWSER_CONFIG_REQUEST_FROM_LINK (LinkStorage);
      LinkStorage = GetNextNode (&Form->ConfigRequestHead, LinkStorage);

      if (Storage != ConfigInfo->Storage) {
        continue;
      }

      if (StrStr (ConfigInfo->ConfigRequest, Progress) != NULL) {
        //
        // Find the OffsetWidth string in this form.
        //
        *RetForm = Form;
        break;
      }
    }

    if (*RetForm != NULL) {
      LinkStatement = GetFirstNode (&Form->StatementListHead);
      while (!IsNull (&Form->StatementListHead, LinkStatement)) {
        Statement = FORM_BROWSER_STATEMENT_FROM_LINK (LinkStatement);
        LinkStatement = GetNextNode (&Form->StatementListHead, LinkStatement);

        if (Statement->BlockName != NULL && StrStr (Statement->BlockName, Progress) != NULL) {
          *RetQuestion = Statement;
          break;
        }

        if (Statement->VariableName != NULL && StrStr (Statement->VariableName, Progress) != NULL) {
          *RetQuestion = Statement;
          break;
        }
      }
    }

    if (*RetForm != NULL) {
      break;
    }
  }

  //
  // restore the OffsetWidth string to the original format.
  //
  if (Storage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    *EndStr = '=';
  } else {
    *EndStr = '&';
  }

  return (BOOLEAN) (*RetForm != NULL);
}

/**
  Base on the return Progress string to get the SyncConfigRequest and RestoreConfigRequest
  for form and formset.

  @param  Storage                 Storage which has this Progress string.
  @param  ConfigRequest           The ConfigRequest string.
  @param  Progress                The Progress string which has the first fail string.
  @param  RestoreConfigRequest    Return the RestoreConfigRequest string.
  @param  SyncConfigRequest       Return the SyncConfigRequest string.

**/
VOID
GetSyncRestoreConfigRequest(
  IN  BROWSER_STORAGE   *Storage,
  IN  EFI_STRING        ConfigRequest,
  IN  EFI_STRING        Progress,
  OUT EFI_STRING        *RestoreConfigRequest,
  OUT EFI_STRING        *SyncConfigRequest
  )
{
  EFI_STRING    EndStr;
  EFI_STRING    ConfigHdrEndStr;
  EFI_STRING    ElementStr;
  UINTN         TotalSize;
  UINTN         RestoreEleSize;
  UINTN         SyncSize;

  DMSG ((EFI_D_ERROR, "\n GetSyncRestoreConfigRequest, \n"));
  ASSERT ((*Progress == L'&') || (*Progress == L'G'));
  //
  // If the Progress starts with ConfigHdr, means the failure is in the first name / value pair.
  // Need to restore all the fields in the ConfigRequest.
  //
  if (*Progress == L'G') {
    *RestoreConfigRequest = AllocateCopyPool (StrSize (ConfigRequest), ConfigRequest);
    ASSERT (*RestoreConfigRequest != NULL);
    return;
  }

  //
  // Find the first fail "NAME" or "OFFSET=0x####&WIDTH=0x####" string.
  //
  if (Storage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    //
    // For Name/Value type, the data is "&Fred=16&George=16&Ron=12" formset,
    // here, just keep the "Fred" string.
    //
    EndStr = StrStr (Progress, L"=");
    ASSERT (EndStr != NULL);
    *EndStr = L'\0';
    //
    // Find the ConfigHdr in ConfigRequest.
    //
    ConfigHdrEndStr = StrStr (ConfigRequest, L"PATH=");
    ASSERT (ConfigHdrEndStr != NULL);
    while (*ConfigHdrEndStr != L'&') {
      ConfigHdrEndStr++;
    }
  } else {
    //
    // For Buffer type, the data is "OFFSET=0x####&WIDTH=0x####&VALUE=0x####",
    // here, just keep the "OFFSET=0x####&WIDTH=0x####" string.
    //
    EndStr = StrStr (Progress, L"&VALUE=");
    ASSERT (EndStr != NULL);
    *EndStr = L'\0';
    //
    // Find the ConfigHdr in ConfigRequest.
    //
    ConfigHdrEndStr = StrStr (ConfigRequest, L"&OFFSET=");
  }
  //
  // Find the first fail pair in the ConfigRequest.
  //
  ElementStr = StrStr (ConfigRequest, Progress);
  ASSERT (ElementStr != NULL);
  //
  // To get the RestoreConfigRequest.
  //
  RestoreEleSize = StrSize (ElementStr);
  TotalSize = (ConfigHdrEndStr - ConfigRequest) * sizeof (CHAR16) + RestoreEleSize + sizeof (CHAR16);
  *RestoreConfigRequest = AllocateZeroPool (TotalSize);
  ASSERT (*RestoreConfigRequest != NULL);
  StrnCpyS (*RestoreConfigRequest, TotalSize / sizeof (CHAR16), ConfigRequest, ConfigHdrEndStr - ConfigRequest);
  StrCatS (*RestoreConfigRequest, TotalSize / sizeof (CHAR16), ElementStr);
  //
  // To get the SyncConfigRequest.
  //
  SyncSize = StrSize (ConfigRequest) - RestoreEleSize + sizeof (CHAR16);
  *SyncConfigRequest = AllocateZeroPool (SyncSize);
  ASSERT (*SyncConfigRequest != NULL);
  StrnCpyS (*SyncConfigRequest, SyncSize / sizeof (CHAR16), ConfigRequest, SyncSize / sizeof (CHAR16) - 1);

  //
  // restore the Progress string to the original format.
  //
  if (Storage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    *EndStr = L'=';
  } else {
    *EndStr = L'&';
  }
}

/**
  Popup an save error info and get user input.

  @param  TitleId                The form title id.
  @param  HiiHandle              The hii handle for this package.

  @retval UINT32                 The user select option for the save fail.
                                 BROWSER_ACTION_DISCARD or BROWSER_ACTION_JUMP_TO_FORMSET
**/
UINT32
ConfirmSaveFail (
  IN EFI_STRING_ID    TitleId,
  IN EFI_HII_HANDLE   HiiHandle
  )
{
  CHAR16                  *FormTitle;
  CHAR16                  *StringBuffer;
  UINT32                  RetVal;

  FormTitle = GetToken (TitleId, HiiHandle);

  StringBuffer = AllocateZeroPool (256 * sizeof (CHAR16));
  ASSERT (StringBuffer != NULL);

  UnicodeSPrint (
    StringBuffer,
    24 * sizeof (CHAR16) + StrSize (FormTitle),
    L"Submit Fail For Form: %s.",
    FormTitle
    );

  RetVal = PopupErrorMessage(BROWSER_SUBMIT_FAIL, NULL, NULL, StringBuffer);

  FreePool (StringBuffer);
  FreePool (FormTitle);

  return RetVal;
}

/**
  Popup an NO_SUBMIT_IF error info and get user input.

  @param  TitleId                The form title id.
  @param  HiiHandle              The hii handle for this package.

  @retval UINT32                 The user select option for the save fail.
                                 BROWSER_ACTION_DISCARD or BROWSER_ACTION_JUMP_TO_FORMSET
**/
UINT32
ConfirmNoSubmitFail (
  IN EFI_STRING_ID    TitleId,
  IN EFI_HII_HANDLE   HiiHandle
  )
{
  CHAR16                  *FormTitle;
  CHAR16                  *StringBuffer;
  UINT32                  RetVal;

  FormTitle = GetToken (TitleId, HiiHandle);

  StringBuffer = AllocateZeroPool (256 * sizeof (CHAR16));
  ASSERT (StringBuffer != NULL);

  UnicodeSPrint (
    StringBuffer,
    24 * sizeof (CHAR16) + StrSize (FormTitle),
    L"NO_SUBMIT_IF error For Form: %s.",
    FormTitle
    );

  RetVal = PopupErrorMessage(BROWSER_SUBMIT_FAIL_NO_SUBMIT_IF, NULL, NULL, StringBuffer);

  FreePool (StringBuffer);
  FreePool (FormTitle);

  return RetVal;
}

/**
  Discard data based on the input setting scope (Form, FormSet or System).

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  SettingScope           Setting Scope for Discard action.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_UNSUPPORTED        Unsupported SettingScope.

**/
EFI_STATUS
DiscardForm (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN BROWSER_SETTING_SCOPE            SettingScope
  )
{
  LIST_ENTRY                   *Link;
  FORMSET_STORAGE              *Storage;
  FORM_BROWSER_CONFIG_REQUEST  *ConfigInfo;
  FORM_BROWSER_FORMSET         *LocalFormSet;
  FORM_BROWSER_FORMSET         *OldFormSet;

  DMSG ((EFI_D_ERROR, "\n DiscardForm, SettingScope :%d. \n", SettingScope));
  //
  // Check the supported setting level.
  //
  if (SettingScope >= MaxLevel) {
    return EFI_UNSUPPORTED;
  }

  if (SettingScope == FormLevel && IsNvUpdateRequiredForForm (Form)) {
    DMSG ((EFI_D_ERROR, "DiscardForm, FormLevel, \n"));
    ConfigInfo = NULL;
    Link = GetFirstNode (&Form->ConfigRequestHead);
    while (!IsNull (&Form->ConfigRequestHead, Link)) {
      ConfigInfo = FORM_BROWSER_CONFIG_REQUEST_FROM_LINK (Link);
      Link = GetNextNode (&Form->ConfigRequestHead, Link);

      if (ConfigInfo->Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
        continue;
      }

      //
      // Skip if there is no RequestElement
      //
      if (ConfigInfo->ElementCount == 0) {
        continue;
      }

      //
      // Prepare <ConfigResp>
      //
      SynchronizeStorage(ConfigInfo->Storage, ConfigInfo->ConfigRequest, FALSE);

      //
      // Call callback with Changed type to inform the driver.
      //
      SendDiscardInfoToDriver (FormSet, Form);
    }

    ValueChangeResetFlagUpdate (FALSE, FormSet, Form);
  } else if (SettingScope == FormSetLevel && IsNvUpdateRequiredForFormSet (FormSet)) {

    DMSG ((EFI_D_ERROR, "\n DiscardForm, FormSetLevel, \n"));
    DMSG_HII(FormSet->FormSetTitle, FormSet->HiiHandle, L"FormSetTitle");
    //
    // Discard Buffer storage or Name/Value storage
    //
    Link = GetFirstNode (&FormSet->StorageListHead);
    while (!IsNull (&FormSet->StorageListHead, Link)) {
      Storage = FORMSET_STORAGE_FROM_LINK (Link);
      Link = GetNextNode (&FormSet->StorageListHead, Link);

      if (Storage->BrowserStorage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
        continue;
      }

      //
      // Skip if there is no RequestElement
      //
      if (Storage->ElementCount == 0) {
        continue;
      }

      SynchronizeStorage(Storage->BrowserStorage, Storage->ConfigRequest, FALSE);
    }

    Link = GetFirstNode (&FormSet->FormListHead);
    while (!IsNull (&FormSet->FormListHead, Link)) {
      Form = FORM_BROWSER_FORM_FROM_LINK (Link);
      Link = GetNextNode (&FormSet->FormListHead, Link);

      //
      // Call callback with Changed type to inform the driver.
      //
      SendDiscardInfoToDriver (FormSet, Form);
    }

    ValueChangeResetFlagUpdate(FALSE, FormSet, NULL);
  } else if (SettingScope == SystemLevel) {
    //
    // System Level Discard.
    //
    OldFormSet = mSystemLevelFormSet;
    DMSG ((EFI_D_ERROR, "\n DiscardForm, SystemLevel, \n"));

    //
    // Discard changed value for each FormSet in the maintain list.
    //
    Link = GetFirstNode (&gBrowserFormSetList);
    while (!IsNull (&gBrowserFormSetList, Link)) {
      LocalFormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
      Link = GetNextNode (&gBrowserFormSetList, Link);
      if (!ValidateFormSet(LocalFormSet)) {
        continue;
      }

      mSystemLevelFormSet = LocalFormSet;

      DiscardForm (LocalFormSet, NULL, FormSetLevel);
    }

    mSystemLevelFormSet = OldFormSet;
    TriggerSetupNotify(SetupSaveNotifyTypeDiscardVaule);
  }

  return EFI_SUCCESS;
}

/**
  Submit data for a form.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_UNSUPPORTED        Unsupported SettingScope.

**/
EFI_STATUS
SubmitForForm (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  EFI_STRING              ConfigResp;
  EFI_STRING              Progress;
  BROWSER_STORAGE         *Storage;
  FORM_BROWSER_CONFIG_REQUEST  *ConfigInfo;
  BOOLEAN                 SubmitFormFail;

  SubmitFormFail = FALSE;
  DMSG ((EFI_D_ERROR, "\n SubmitForForm, \n"));

  if (!IsNvUpdateRequiredForForm (Form)) {
    return EFI_SUCCESS;
  }

  Status = NoSubmitCheck (FormSet, &Form, NULL);
  DMSG ((EFI_D_ERROR, "SubmitForForm, NoSubmitCheck :%r. \n", Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Link = GetFirstNode (&Form->ConfigRequestHead);
  while (!IsNull (&Form->ConfigRequestHead, Link)) {
    ConfigInfo = FORM_BROWSER_CONFIG_REQUEST_FROM_LINK (Link);
    Link = GetNextNode (&Form->ConfigRequestHead, Link);

    Storage = ConfigInfo->Storage;
    if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
      continue;
    }

    //
    // Skip if there is no RequestElement
    //
    if (ConfigInfo->ElementCount == 0) {
      continue;
    }

    DMSG ((EFI_D_ERROR, "\n SubmitForForm, Storage :%d-%s. \n", Storage->Type, Storage->Name));
    //
    // 1. Prepare <ConfigResp>
    //
    Status = StorageToConfigResp (ConfigInfo->Storage, &ConfigResp, ConfigInfo->ConfigRequest, TRUE);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // 2. Set value to hii config routine protocol.
    //
    Status = mHiiConfigRouting->RouteConfig (
                                      mHiiConfigRouting,
                                      ConfigResp,
                                      &Progress
                                      );
    DMSG ((EFI_D_ERROR, "SubmitForForm, RouteConfig :%r. \n", Status));
    if (EFI_ERROR (Status)) {
      //
      // Submit fail, to get the RestoreConfigRequest and SyncConfigRequest.
      //
      SubmitFormFail = TRUE;
      GetSyncRestoreConfigRequest (ConfigInfo->Storage, ConfigInfo->ConfigRequest, Progress, &ConfigInfo->RestoreConfigRequest, &ConfigInfo->SyncConfigRequest);
      InsertTailList (&gBrowserSaveFailFormSetList, &ConfigInfo->SaveFailLink);
      FreePool (ConfigResp);
      continue;
    }

    FreePool (ConfigResp);
    //
    // 3. Config success, update storage shadow Buffer, only update the data belong to this form.
    //
    SynchronizeStorage (ConfigInfo->Storage, ConfigInfo->ConfigRequest, TRUE);
  }

  //
  // 4. Process the save failed storage.
  //
  if (!IsListEmpty (&gBrowserSaveFailFormSetList)) {
    DMSG ((EFI_D_ERROR, "SubmitForForm, Process Failed Storage, \n"));
    if (ConfirmSaveFail (Form->FormTitle, FormSet->HiiHandle) == BROWSER_ACTION_DISCARD) {
      Link = GetFirstNode (&gBrowserSaveFailFormSetList);
      while (!IsNull (&gBrowserSaveFailFormSetList, Link)) {
        ConfigInfo = FORM_BROWSER_CONFIG_REQUEST_FROM_SAVE_FAIL_LINK (Link);
        Link = GetNextNode (&gBrowserSaveFailFormSetList, Link);
        //
        // Process the submit fail question, base on the RestoreConfigRequest to restore the EditBuffer
        // base on the SyncConfigRequest to Sync the buffer.
        //
        SynchronizeStorage (ConfigInfo->Storage, ConfigInfo->RestoreConfigRequest, FALSE);
        FreePool (ConfigInfo->RestoreConfigRequest);
        ConfigInfo->RestoreConfigRequest = NULL;
        if (ConfigInfo->SyncConfigRequest != NULL) {
          SynchronizeStorage(ConfigInfo->Storage, ConfigInfo->SyncConfigRequest, TRUE);
          FreePool (ConfigInfo->SyncConfigRequest);
          ConfigInfo->SyncConfigRequest = NULL;
        }

        Status = EFI_SUCCESS;
      }
      SendDiscardInfoToDriver (FormSet,Form);
    } else {
      Status = EFI_UNSUPPORTED;
    }

    //
    // Free Form save fail list.
    //
    while (!IsListEmpty (&gBrowserSaveFailFormSetList)) {
      Link = GetFirstNode (&gBrowserSaveFailFormSetList);
      ConfigInfo = FORM_BROWSER_CONFIG_REQUEST_FROM_SAVE_FAIL_LINK (Link);
      RemoveEntryList (&ConfigInfo->SaveFailLink);
    }
  }

  //
  // 5. Update the NV flag.
  //
  DMSG ((EFI_D_ERROR, "SubmitForForm, ValueChangeResetFlagUpdate, \n"));
  ValueChangeResetFlagUpdate(TRUE, FormSet, Form);

  //
  // 6 Call callback with Submitted type to inform the driver.
  //
  if (!SubmitFormFail) {
    DMSG ((EFI_D_ERROR, "SubmitForForm, SubmitCallback, \n"));
    SubmitCallback (FormSet, Form);
  }

  DMSG ((EFI_D_ERROR, "SubmitForForm, End :%r. \n", Status));
  return Status;
}

/**
  Submit data for a formset.

  @param  FormSet                FormSet data structure.
  @param  SkipProcessFail        Whether skip to process the save failed storage.
                                 If submit formset is called when do system level save,
                                 set this value to true and process the failed formset
                                 together.
                                 if submit formset is called when do formset level save,
                                 set the value to false and process the failed storage
                                 right after process all storages for this formset.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_UNSUPPORTED        Unsupported SettingScope.

**/
EFI_STATUS
SubmitForFormSet (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN BOOLEAN                          SkipProcessFail
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  EFI_STRING              ConfigResp;
  EFI_STRING              Progress;
  BROWSER_STORAGE         *Storage;
  FORMSET_STORAGE         *FormSetStorage;
  FORM_BROWSER_FORM       *Form;
  BOOLEAN                 HasInserted;
  FORM_BROWSER_STATEMENT  *Question;
  BOOLEAN                 SubmitFormSetFail;
  BOOLEAN                 DiscardChange;

  HasInserted = FALSE;
  SubmitFormSetFail = FALSE;
  DiscardChange     = FALSE;

  DMSG ((EFI_D_ERROR, "\n SubmitForFormSet, \n"));
  DMSG_HII(FormSet->FormSetTitle, FormSet->HiiHandle, L"FormSetTitle");

  if (!IsNvUpdateRequiredForFormSet (FormSet)) {
    return EFI_SUCCESS;
  }

  Form = NULL;
  Status = NoSubmitCheck (FormSet, &Form, &Question);
  DMSG ((EFI_D_ERROR, "SubmitForFormSet, NoSubmitCheck :%r. \n", Status));
  if (EFI_ERROR (Status)) {
    if (SkipProcessFail) {
      //
      // Process NO_SUBMIT check first, so insert it at head.
      //
      FormSet->SaveFailForm = Form;
      FormSet->SaveFailStatement = Question;
      InsertHeadList (&gBrowserSaveFailFormSetList, &FormSet->SaveFailLink);
    }

    return Status;
  }

  Form = NULL;
  Question = NULL;
  //
  // Submit Buffer storage or Name/Value storage
  //
  Link = GetFirstNode (&FormSet->StorageListHead);
  while (!IsNull (&FormSet->StorageListHead, Link)) {
    FormSetStorage = FORMSET_STORAGE_FROM_LINK (Link);
    Storage        = FormSetStorage->BrowserStorage;
    Link = GetNextNode (&FormSet->StorageListHead, Link);

    if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
      continue;
    }

    //
    // Skip if there is no RequestElement
    //
    if (FormSetStorage->ElementCount == 0) {
      continue;
    }

    if (!FormSetStorage->BrowserStorage->IsSkipRequestString) {
      DMSG ((EFI_D_ERROR, "\n SubmitForFormSet, Storage :%d-%s. \n", Storage->Type, Storage->Name));
      //
      // 1. Prepare <ConfigResp>
      //
      Status = StorageToConfigResp (Storage, &ConfigResp, FormSetStorage->ConfigRequest, TRUE);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      //
      // 2. Send <ConfigResp> to Routine config Protocol.
      //
      DMSG ((EFI_D_ERROR, "SubmitForFormSet, RouteConfig, \n"));
      Status = mHiiConfigRouting->RouteConfig (
                                        mHiiConfigRouting,
                                        ConfigResp,
                                        &Progress
                                        );
      DMSG ((EFI_D_ERROR, "SubmitForFormSet, RouteConfig :%r. \n", Status));
      if (EFI_ERROR (Status)) {
        //
        // Submit fail, to get the RestoreConfigRequest and SyncConfigRequest.
        //
        SubmitFormSetFail = TRUE;
        GetSyncRestoreConfigRequest (FormSetStorage->BrowserStorage, FormSetStorage->ConfigRequest, Progress, &FormSetStorage->RestoreConfigRequest, &FormSetStorage->SyncConfigRequest);
        InsertTailList (&FormSet->SaveFailStorageListHead, &FormSetStorage->SaveFailLink);
        if (!HasInserted) {
          //
          // Call submit formset for system level, save the formset info
          // and process later.
          //
          FindQuestionFromProgress(FormSet, Storage, Progress, &Form, &Question);
          ASSERT (Form != NULL && Question != NULL);
          FormSet->SaveFailForm = Form;
          FormSet->SaveFailStatement = Question;
          if (SkipProcessFail) {
            InsertTailList (&gBrowserSaveFailFormSetList, &FormSet->SaveFailLink);
          }
          HasInserted = TRUE;
        }

        FreePool (ConfigResp);
        continue;
      }

      FreePool (ConfigResp);
    } else{
      Status = gRT->SetVariable (
                      Storage->Name,
                      &Storage->Guid,
                      Storage->Attributes,
                      Storage->Size,
                      Storage->EditBuffer
                      );
      ASSERT_EFI_ERROR (Status);
    }
    //
    // 3. Config success, update storage shadow Buffer
    //
    SynchronizeStorage (Storage, FormSetStorage->ConfigRequest, TRUE);
  }

  //
  // 4. Has save fail storage need to handle.
  //
  if (Form != NULL) {
    DMSG ((EFI_D_ERROR, "SubmitForFormSet, handle Failed Storage, \n"));
    if (!SkipProcessFail) {
      //
      // If not in system level, just handle the save failed storage here.
      //
      if (ConfirmSaveFail (Form->FormTitle, FormSet->HiiHandle) == BROWSER_ACTION_DISCARD) {
        DiscardChange = TRUE;
        Link = GetFirstNode (&FormSet->SaveFailStorageListHead);
        while (!IsNull (&FormSet->SaveFailStorageListHead, Link)) {
          FormSetStorage = FORMSET_STORAGE_FROM_SAVE_FAIL_LINK (Link);
          Storage        = FormSetStorage->BrowserStorage;
          Link = GetNextNode (&FormSet->SaveFailStorageListHead, Link);
          //
          // Process the submit fail question, base on the RestoreConfigRequest to restore the EditBuffer
          // base on the SyncConfigRequest to Sync the buffer.
          //
          SynchronizeStorage (FormSetStorage->BrowserStorage, FormSetStorage->RestoreConfigRequest, FALSE);
          FreePool (FormSetStorage->RestoreConfigRequest);
          FormSetStorage->RestoreConfigRequest = NULL;
          if (FormSetStorage->SyncConfigRequest != NULL) {
            SynchronizeStorage(FormSetStorage->BrowserStorage, FormSetStorage->SyncConfigRequest, TRUE);
            FreePool (FormSetStorage->SyncConfigRequest);
            FormSetStorage->SyncConfigRequest = NULL;
          }

          Status = EFI_SUCCESS;
        }
      } else {
        UiCopyMenuList(&mPrivateData.FormBrowserEx2.FormViewHistoryHead, &Form->FormViewListHead);

        gCurrentSelection->Action = UI_ACTION_REFRESH_FORMSET;
        gCurrentSelection->Handle = FormSet->HiiHandle;
        CopyGuid (&gCurrentSelection->FormSetGuid, &FormSet->Guid);
        gCurrentSelection->FormId = Form->FormId;
        gCurrentSelection->QuestionId = Question->QuestionId;

        Status = EFI_UNSUPPORTED;
      }

      //
      // Free FormSet save fail list.
      //
      while (!IsListEmpty (&FormSet->SaveFailStorageListHead)) {
        Link = GetFirstNode (&FormSet->SaveFailStorageListHead);
        FormSetStorage = FORMSET_STORAGE_FROM_SAVE_FAIL_LINK (Link);
        RemoveEntryList (&FormSetStorage->SaveFailLink);
      }
    } else {
      //
      // If in system level, just return error and handle the failed formset later.
      //
      Status = EFI_UNSUPPORTED;
    }
  }

  //
  // If user discard the change, send the discard info to driver.
  //
  if (DiscardChange) {
    Link = GetFirstNode (&FormSet->FormListHead);
    while (!IsNull (&FormSet->FormListHead, Link)) {
      Form = FORM_BROWSER_FORM_FROM_LINK (Link);
      Link = GetNextNode (&FormSet->FormListHead, Link);
      //
      // Call callback with Changed type to inform the driver.
      //
      SendDiscardInfoToDriver (FormSet, Form);
    }
  }

  //
  // 5. Update the NV flag.
  //
  ValueChangeResetFlagUpdate(TRUE, FormSet, NULL);

  //
  // 6. Call callback with Submitted type to inform the driver.
  //
  if (!SubmitFormSetFail) {
    SubmitCallback (FormSet, NULL);
  }

  DMSG ((EFI_D_ERROR, "SubmitForFormSet, End :%r. \n", Status));
  return Status;
}

/**
  Submit data for all formsets.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_UNSUPPORTED        Unsupported SettingScope.

**/
EFI_STATUS
SubmitForSystem (
  VOID
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  LIST_ENTRY              *FormLink;
  LIST_ENTRY              *StorageLink;
  FORMSET_STORAGE         *FormSetStorage;
  FORM_BROWSER_FORM       *Form;
  FORM_BROWSER_FORMSET    *LocalFormSet;
  UINT32                  UserSelection;
  FORM_BROWSER_STATEMENT  *Question;

  DMSG ((EFI_D_ERROR, "\n SubmitForSystem, \n"));
  mSystemSubmit = TRUE;
  Link = GetFirstNode (&gBrowserFormSetList);
  while (!IsNull (&gBrowserFormSetList, Link)) {
    LocalFormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&gBrowserFormSetList, Link);
    if (!ValidateFormSet(LocalFormSet)) {
      continue;
    }

    DMSG ((EFI_D_ERROR, "SubmitForSystem, SubmitForFormSet, \n"));
    #ifdef SETUP_DEBUG
    //
    //Show Status Info to user.
    //
    if (LocalFormSet->FormSetTitle) {
      CHAR16    *FormSetTitle;
      FormSetTitle = GetToken (LocalFormSet->FormSetTitle, LocalFormSet->HiiHandle);
      UiConfirmDialog(DIALOG_NO_REPAINT, mCustomString->StrLoadFormset, NULL, TEXT_ALIGIN_CENTER, gEmptyString, FormSetTitle, NULL);  
      if (FormSetTitle) {
        FreePool (FormSetTitle);
      }
    }
    #endif 

    Status = SubmitForFormSet (LocalFormSet, TRUE);

    DMSG ((EFI_D_ERROR, "SubmitForSystem, SubmitForFormSet :%r. \n", Status));
    if (EFI_ERROR (Status)) {
      continue;
    }
  }
  mSystemSubmit = FALSE;

  #ifdef SETUP_DEBUG 
  UiClearNoRepaintFlag(DIALOG_NO_REPAINT);
  #endif 
  
  Status = EFI_SUCCESS;

  //
  // Process the save failed formsets.
  //
  Link = GetFirstNode (&gBrowserSaveFailFormSetList);
  while (!IsNull (&gBrowserSaveFailFormSetList, Link)) {
    LocalFormSet = FORM_BROWSER_FORMSET_FROM_SAVE_FAIL_LINK (Link);
    Link = GetNextNode (&gBrowserSaveFailFormSetList, Link);

    if (!ValidateFormSet(LocalFormSet)) {
      continue;
    }

    DMSG ((EFI_D_ERROR, "\n SubmitForSystem, Process the save failed formsets, \n"));
    Form = LocalFormSet->SaveFailForm;
    Question= LocalFormSet->SaveFailStatement;

    //
    // Confirm with user, get user input.
    //
    if (IsListEmpty (&LocalFormSet->SaveFailStorageListHead)) {
      //
      // NULL for SaveFailStorageListHead means error caused by NO_SUBMIT_IF check.
      //
      UserSelection = ConfirmNoSubmitFail (Form->FormTitle, LocalFormSet->HiiHandle);
    } else {
      UserSelection = ConfirmSaveFail (Form->FormTitle, LocalFormSet->HiiHandle);
    }

    if (UserSelection == BROWSER_ACTION_DISCARD) {
      if (IsListEmpty (&LocalFormSet->SaveFailStorageListHead)) {
        StorageLink = GetFirstNode (&LocalFormSet->StorageListHead);
        while (!IsNull (&LocalFormSet->StorageListHead, StorageLink)) {
          FormSetStorage = FORMSET_STORAGE_FROM_LINK (StorageLink);
          StorageLink = GetNextNode (&LocalFormSet->StorageListHead, StorageLink);

          SynchronizeStorage(FormSetStorage->BrowserStorage, FormSetStorage->ConfigRequest, FALSE);
        }
      } else {
        StorageLink = GetFirstNode (&LocalFormSet->SaveFailStorageListHead);
        while (!IsNull (&LocalFormSet->SaveFailStorageListHead, StorageLink)) {
          FormSetStorage = FORMSET_STORAGE_FROM_SAVE_FAIL_LINK (StorageLink);
          StorageLink = GetNextNode (&LocalFormSet->SaveFailStorageListHead, StorageLink);
          //
          // Process the submit fail question, base on the RestoreConfigRequest to restore the EditBuffer
          // base on the SyncConfigRequest to Sync the buffer.
          //
          SynchronizeStorage (FormSetStorage->BrowserStorage, FormSetStorage->RestoreConfigRequest, FALSE);
          FreePool (FormSetStorage->RestoreConfigRequest);
          FormSetStorage->RestoreConfigRequest = NULL;
          if ( FormSetStorage->SyncConfigRequest != NULL) {
            SynchronizeStorage (FormSetStorage->BrowserStorage, FormSetStorage->SyncConfigRequest, TRUE);
            FreePool (FormSetStorage->SyncConfigRequest);
            FormSetStorage->SyncConfigRequest = NULL;
          }
        }
      }

      FormLink = GetFirstNode (&LocalFormSet->FormListHead);
      while (!IsNull (&LocalFormSet->FormListHead, FormLink)) {
        Form = FORM_BROWSER_FORM_FROM_LINK (FormLink);
        FormLink = GetNextNode (&LocalFormSet->FormListHead, FormLink);
        //
        // Call callback with Changed type to inform the driver.
        //
        SendDiscardInfoToDriver (LocalFormSet, Form);
      }

      if (IsHiiHandleInBrowserContext (LocalFormSet->HiiHandle)) {
        ValueChangeResetFlagUpdate(FALSE, LocalFormSet, NULL);
      }
    } else {
      if (IsListEmpty (&LocalFormSet->SaveFailStorageListHead)) {
        NoSubmitCheck (LocalFormSet, &Form, &Question);
      }

      UiCopyMenuList(&mPrivateData.FormBrowserEx2.FormViewHistoryHead, &Form->FormViewListHead);

      gCurrentSelection->Action = UI_ACTION_REFRESH_FORMSET;
      gCurrentSelection->Handle = LocalFormSet->HiiHandle;
      CopyGuid (&gCurrentSelection->FormSetGuid, &LocalFormSet->Guid);
      gCurrentSelection->FormId = Form->FormId;
      gCurrentSelection->QuestionId = Question->QuestionId;

      Status = EFI_UNSUPPORTED;
      break;
    }
  }

  //
  // Clean the list which will not process.
  //
  while (!IsListEmpty (&gBrowserSaveFailFormSetList)) {
    Link = GetFirstNode (&gBrowserSaveFailFormSetList);
    LocalFormSet = FORM_BROWSER_FORMSET_FROM_SAVE_FAIL_LINK (Link);
    RemoveEntryList (&LocalFormSet->SaveFailLink);

    while (!IsListEmpty (&LocalFormSet->SaveFailStorageListHead)) {
      StorageLink = GetFirstNode (&LocalFormSet->SaveFailStorageListHead);
      FormSetStorage = FORMSET_STORAGE_FROM_SAVE_FAIL_LINK (StorageLink);
      RemoveEntryList (&FormSetStorage->SaveFailLink);
    }
  }

  return Status;
}

/**
  Submit data based on the input Setting level (Form, FormSet or System).

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  SettingScope           Setting Scope for Submit action.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_UNSUPPORTED        Unsupported SettingScope.

**/
EFI_STATUS
SubmitForm (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN BROWSER_SETTING_SCOPE            SettingScope
  )
{
  EFI_STATUS              Status;

  DMSG ((EFI_D_ERROR, "\n SubmitForm, SettingScope :%d. \n", SettingScope));
  switch (SettingScope) {
  case FormLevel:
    Status = SubmitForForm(FormSet, Form);
    break;

  case FormSetLevel:
    Status = SubmitForFormSet (FormSet, FALSE);
    break;

  case SystemLevel:
    if (PcdGet8 (PcdDisplaySetupMessage) == 1) {
      Status = gBS->SetTimer(mTimerEvent, TimerPeriodic, 10000000);
      gST->ConOut->EnableCursor(gST->ConOut, FALSE);
      mRecoveryDefault = FALSE;
      TimerInSecondsCallBack (mTimerEvent, NULL);
    }
    Status = SubmitForSystem ();
    break;

  default:
    Status = EFI_UNSUPPORTED;
    break;
  }

  if (SettingScope == SystemLevel) {
    TriggerSetupNotify(SetupSaveNotifyTypeSaveValue);
    gBS->SetTimer(mTimerEvent, TimerCancel, 0);
    mCount = 0;
    UiClearNoRepaintFlag(DIALOG_NO_REPAINT);
  }
  return Status;
}

/**
  Converts the unicode character of the string from uppercase to lowercase.
  This is a internal function.

  @param ConfigString  String to be converted

**/
VOID
EFIAPI
HiiToLower (
  IN EFI_STRING  ConfigString
  )
{
  EFI_STRING  String;
  BOOLEAN     Lower;

  ASSERT (ConfigString != NULL);

  //
  // Convert all hex digits in range [A-F] in the configuration header to [a-f]
  //
  for (String = ConfigString, Lower = FALSE; *String != L'\0'; String++) {
    if (*String == L'=') {
      Lower = TRUE;
    } else if (*String == L'&') {
      Lower = FALSE;
    } else if (Lower && *String >= L'A' && *String <= L'F') {
      *String = (CHAR16) (*String - L'A' + L'a');
    }
  }
}

/**
  Find the point in the ConfigResp string for this question.

  @param  Question               The question.
  @param  ConfigResp             Get ConfigResp string.

  @retval  point to the offset where is for this question.

**/
CHAR16 *
GetOffsetFromConfigResp (
  IN FORM_BROWSER_STATEMENT           *Question,
  IN CHAR16                           *ConfigResp
  )
{
  CHAR16                       *RequestElement;
  CHAR16                       *BlockData;

  //
  // Type is EFI_HII_VARSTORE_NAME_VALUE.
  //
  if (Question->Storage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    RequestElement = StrStr (ConfigResp, Question->VariableName);
    if (RequestElement != NULL) {
      //
      // Skip the "VariableName=" field.
      //
      RequestElement += StrLen (Question->VariableName) + 1;
    }

    return RequestElement;
  }

  //
  // Type is EFI_HII_VARSTORE_EFI_VARIABLE or EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER
  //

  //
  // Convert all hex digits in ConfigResp to lower case before searching.
  //
  HiiToLower (ConfigResp);

  //
  // 1. Directly use Question->BlockName to find.
  //
  RequestElement = StrStr (ConfigResp, Question->BlockName);
  if (RequestElement != NULL) {
    //
    // Skip the "Question->BlockName&VALUE=" field.
    //
    RequestElement += StrLen (Question->BlockName) + StrLen (L"&VALUE=");
    return RequestElement;
  }

  //
  // 2. Change all hex digits in Question->BlockName to lower and compare again.
  //
  BlockData = AllocateCopyPool (StrSize(Question->BlockName), Question->BlockName);
  ASSERT (BlockData != NULL);
  HiiToLower (BlockData);
  RequestElement = StrStr (ConfigResp, BlockData);
  FreePool (BlockData);

  if (RequestElement != NULL) {
    //
    // Skip the "Question->BlockName&VALUE=" field.
    //
    RequestElement += StrLen (Question->BlockName) + StrLen (L"&VALUE=");
  }

  return RequestElement;
}

/**
  Get Question default value from AltCfg string.

  @param  FormSet                The form set.
  @param  Form                   The form
  @param  Question               The question.

  @retval EFI_SUCCESS            Question is reset to default value.

**/
EFI_STATUS
GetDefaultValueFromAltCfg (
  IN     FORM_BROWSER_FORMSET             *FormSet,
  IN     FORM_BROWSER_FORM                *Form,
  IN OUT FORM_BROWSER_STATEMENT           *Question
  )
{
  BROWSER_STORAGE              *Storage;
  FORMSET_STORAGE              *FormSetStorage;
  CHAR16                       *ConfigResp;
  CHAR16                       *Value;
  LIST_ENTRY                   *Link;
  FORM_BROWSER_CONFIG_REQUEST  *ConfigInfo;

  Storage = Question->Storage;
  if ((Storage == NULL) || (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) || Question->Storage->IsSkipRequestString) {
    return EFI_NOT_FOUND;
  }
  //DMSG ((EFI_D_ERROR, "\n GetDefaultValueFromAltCfg, \n"));
  //
  // Try to get AltCfg string from form. If not found it, then
  // try to get it from formset.
  //
  ConfigResp    = NULL;
  Link = GetFirstNode (&Form->ConfigRequestHead);
  while (!IsNull (&Form->ConfigRequestHead, Link)) {
    ConfigInfo = FORM_BROWSER_CONFIG_REQUEST_FROM_LINK (Link);
    Link = GetNextNode (&Form->ConfigRequestHead, Link);

    if (Storage == ConfigInfo->Storage) {
      ConfigResp = ConfigInfo->ConfigAltResp;
      break;
    }
  }

  if (ConfigResp == NULL) {
    Link = GetFirstNode (&FormSet->StorageListHead);
    while (!IsNull (&FormSet->StorageListHead, Link)) {
      FormSetStorage = FORMSET_STORAGE_FROM_LINK (Link);
      Link = GetNextNode (&FormSet->StorageListHead, Link);

      if (Storage == FormSetStorage->BrowserStorage) {
        ConfigResp = FormSetStorage->ConfigAltResp;
        break;
      }
    }
  }

  if (ConfigResp == NULL) {
    return EFI_NOT_FOUND;
  }

  Value = GetOffsetFromConfigResp (Question, ConfigResp);
  if (Value == NULL) {
    return EFI_NOT_FOUND;
  }

  return BufferToValue (Question, Value);
}

/**
  Get default Id value used for browser.

  @param  DefaultId              The default id value used by hii.

  @retval Browser used default value.

**/
INTN
GetDefaultIdForCallBack (
  UINTN DefaultId
  )
{
  if (DefaultId == EFI_HII_DEFAULT_CLASS_STANDARD) {
    return EFI_BROWSER_ACTION_DEFAULT_STANDARD;
  } else if (DefaultId == EFI_HII_DEFAULT_CLASS_MANUFACTURING) {
    return EFI_BROWSER_ACTION_DEFAULT_MANUFACTURING;
  } else if (DefaultId == EFI_HII_DEFAULT_CLASS_SAFE) {
    return EFI_BROWSER_ACTION_DEFAULT_SAFE;
  } else if (DefaultId >= EFI_HII_DEFAULT_CLASS_PLATFORM_BEGIN && DefaultId < EFI_HII_DEFAULT_CLASS_PLATFORM_BEGIN + 0x1000) {
    return EFI_BROWSER_ACTION_DEFAULT_PLATFORM + DefaultId - EFI_HII_DEFAULT_CLASS_PLATFORM_BEGIN;
  } else if (DefaultId >= EFI_HII_DEFAULT_CLASS_HARDWARE_BEGIN && DefaultId < EFI_HII_DEFAULT_CLASS_HARDWARE_BEGIN + 0x1000) {
    return EFI_BROWSER_ACTION_DEFAULT_HARDWARE + DefaultId - EFI_HII_DEFAULT_CLASS_HARDWARE_BEGIN;
  } else if (DefaultId >= EFI_HII_DEFAULT_CLASS_FIRMWARE_BEGIN && DefaultId < EFI_HII_DEFAULT_CLASS_FIRMWARE_BEGIN + 0x1000) {
    return EFI_BROWSER_ACTION_DEFAULT_FIRMWARE + DefaultId - EFI_HII_DEFAULT_CLASS_FIRMWARE_BEGIN;
  } else {
    return -1;
  }
}



/**
  Return data element in an Array by its Index.

  @param  Array                  The data array.
  @param  Type                   Type of the data in this array.
  @param  Index                  Zero based index for data in this array.

  @retval Value                  The data to be returned

**/
UINT64
GetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index
  )
{
  UINT64 Data;

  ASSERT (Array != NULL);

  Data = 0;
  switch (Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    Data = (UINT64) *(((UINT8 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    Data = (UINT64) *(((UINT16 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    Data = (UINT64) *(((UINT32 *) Array) + Index);
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    Data = (UINT64) *(((UINT64 *) Array) + Index);
    break;

  default:
    break;
  }

  return Data;
}


/**
  Set value of a data element in an Array by its Index.

  @param  Array                  The data array.
  @param  Type                   Type of the data in this array.
  @param  Index                  Zero based index for data in this array.
  @param  Value                  The value to be set.

**/
VOID
SetArrayData (
  IN VOID                     *Array,
  IN UINT8                    Type,
  IN UINTN                    Index,
  IN UINT64                   Value
  )
{

  ASSERT (Array != NULL);

  switch (Type) {
  case EFI_IFR_TYPE_NUM_SIZE_8:
    *(((UINT8 *) Array) + Index) = (UINT8) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_16:
    *(((UINT16 *) Array) + Index) = (UINT16) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_32:
    *(((UINT32 *) Array) + Index) = (UINT32) Value;
    break;

  case EFI_IFR_TYPE_NUM_SIZE_64:
    *(((UINT64 *) Array) + Index) = (UINT64) Value;
    break;

  default:
    break;
  }
}

/**
  Search an Option of a Question by its value.

  @param  Question               The Question
  @param  OptionValue            Value for Option to be searched.

  @retval Pointer                Pointer to the found Option.
  @retval NULL                   Option not found.

**/
QUESTION_OPTION *
ValueToOption (
  IN FORM_BROWSER_STATEMENT   *Question,
  IN EFI_HII_VALUE            *OptionValue
  )
{
  LIST_ENTRY       *Link;
  QUESTION_OPTION  *Option;
  INTN             Result;

  Link = GetFirstNode (&Question->OptionListHead);
  while (!IsNull (&Question->OptionListHead, Link)) {
    Option = QUESTION_OPTION_FROM_LINK (Link);
    if ((CompareHiiValue (&Option->Value, OptionValue, &Result, NULL) == EFI_SUCCESS) && (Result == 0)) {
      //
      // Check the suppressif condition, only a valid option can be return.
      //
      if ((Option->SuppressExpression == NULL) ||
          ((EvaluateExpressionList(Option->SuppressExpression, FALSE, NULL, NULL) == ExpressFalse))) {
        return Option;
      }
    }

    Link = GetNextNode (&Question->OptionListHead, Link);
  }

  return NULL;
}

/**
  Reset Question to its default value.

  @param  FormSet                The form set.
  @param  Form                   The form.
  @param  Question               The question.
  @param  DefaultId              The Class of the default.

  @retval EFI_SUCCESS            Question is reset to default value.

**/
EFI_STATUS
GetQuestionDefault (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN FORM_BROWSER_STATEMENT           *Question,
  IN UINT16                           DefaultId
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  QUESTION_DEFAULT        *Default;
  QUESTION_OPTION         *Option;
  EFI_HII_VALUE           *HiiValue;
  UINT8                   Index;
  EFI_STRING              StrValue;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  EFI_BROWSER_ACTION_REQUEST      ActionRequest;
  INTN                            Action;
  CHAR16                          *NewString;
  EFI_IFR_TYPE_VALUE              *TypeValue;
  UINT16                          OriginalDefaultId;
  FORMSET_DEFAULTSTORE            *DefaultStore;
  LIST_ENTRY                      *DefaultLink;

  Status   = EFI_NOT_FOUND;
  StrValue = NULL;
  OriginalDefaultId  = DefaultId;
  DefaultLink        = GetFirstNode (&FormSet->DefaultStoreListHead);

  //
  // Statement don't have storage, skip them
  //
  if (Question->QuestionId == 0) {
    return Status;
  }

  // DMSG ((EFI_D_ERROR, "\n GetQuestionDefault, DefaultId :0x%x. \n", DefaultId));
  //DMSG_HII(Question->Prompt, FormSet->HiiHandle, L"Prompt");
  //
  // There are Five ways to specify default value for a Question:
  //  1, use call back function (highest priority)
  //  2, use ExtractConfig function
  //  3, use nested EFI_IFR_DEFAULT
  //  4, set flags of EFI_ONE_OF_OPTION (provide Standard and Manufacturing default)
  //  5, set flags of EFI_IFR_CHECKBOX (provide Standard and Manufacturing default) (lowest priority)
  //
ReGetDefault:
  HiiValue = &Question->HiiValue;
  TypeValue = &HiiValue->Value;
  if (HiiValue->Type == EFI_IFR_TYPE_BUFFER) {
    //
    // For orderedlist, need to pass the BufferValue to Callback function.
    //
    TypeValue = (EFI_IFR_TYPE_VALUE *) Question->BufferValue;
  }

  //
  // Get Question default value from call back function.
  //
  ConfigAccess = FormSet->ConfigAccess;
  Action = GetDefaultIdForCallBack (DefaultId);
  if ((Action > 0) && ((Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) != 0) && (ConfigAccess != NULL)) {
    ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
    Status = ConfigAccess->Callback (
                             ConfigAccess,
                             Action,
                             Question->QuestionId,
                             HiiValue->Type,
                             TypeValue,
                             &ActionRequest
                             );
    //DMSG ((EFI_D_ERROR, "GetQuestionDefault, Callback :%r. \n", Status));
    if (!EFI_ERROR (Status)) {
      if (HiiValue->Type == EFI_IFR_TYPE_STRING) {
        NewString = GetToken (Question->HiiValue.Value.string, FormSet->HiiHandle);
        ASSERT (NewString != NULL);

        ASSERT (StrLen (NewString) * sizeof (CHAR16) <= Question->StorageWidth);
        if (StrLen (NewString) * sizeof (CHAR16) <= Question->StorageWidth) {
          ZeroMem (Question->BufferValue, Question->StorageWidth);
          CopyMem (Question->BufferValue, NewString, StrSize (NewString));
        } else {
          CopyMem (Question->BufferValue, NewString, Question->StorageWidth);
        }

        FreePool (NewString);
      }
      return Status;
    }
  }

  //
  // Get default value from altcfg string.
  //
  if (ConfigAccess != NULL) {
    Status = GetDefaultValueFromAltCfg(FormSet, Form, Question);
    //DMSG ((EFI_D_ERROR, "GetQuestionDefault, GetDefaultValueFromAltCfg :%r. \n", Status));
    if (!EFI_ERROR (Status)) {
        return Status;
    }
  }

  //
  // EFI_IFR_DEFAULT has highest priority
  //
  if (!IsListEmpty (&Question->DefaultListHead)) {
    //DMSG ((EFI_D_ERROR, "GetQuestionDefault, EFI_IFR_DEFAULT, \n"));
    Link = GetFirstNode (&Question->DefaultListHead);
    while (!IsNull (&Question->DefaultListHead, Link)) {
      Default = QUESTION_DEFAULT_FROM_LINK (Link);

      if (Default->DefaultId == DefaultId) {
        if (Default->ValueExpression != NULL) {
          //
          // Default is provided by an Expression, evaluate it
          //
          Status = EvaluateExpression (FormSet, Form, Default->ValueExpression);
          if (EFI_ERROR (Status)) {
            return Status;
          }

          if (Default->ValueExpression->Result.Type == EFI_IFR_TYPE_BUFFER) {
            ASSERT (HiiValue->Type == EFI_IFR_TYPE_BUFFER && Question->BufferValue != NULL);
            if (Question->StorageWidth > Default->ValueExpression->Result.BufferLen) {
              CopyMem (Question->HiiValue.Buffer, Default->ValueExpression->Result.Buffer, Default->ValueExpression->Result.BufferLen);
              Question->HiiValue.BufferLen = Default->ValueExpression->Result.BufferLen;
            } else {
              CopyMem (Question->HiiValue.Buffer, Default->ValueExpression->Result.Buffer, Question->StorageWidth);
              Question->HiiValue.BufferLen = Question->StorageWidth;
            }
            FreePool (Default->ValueExpression->Result.Buffer);
          }
          HiiValue->Type = Default->ValueExpression->Result.Type;
          CopyMem (&HiiValue->Value, &Default->ValueExpression->Result.Value, sizeof (EFI_IFR_TYPE_VALUE));
        } else {
          //
          // Default value is embedded in EFI_IFR_DEFAULT
          //
          if (Default->Value.Type == EFI_IFR_TYPE_BUFFER) {
            ASSERT (HiiValue->Buffer != NULL);
            CopyMem (HiiValue->Buffer, Default->Value.Buffer, Default->Value.BufferLen);
          } else {
            CopyMem (HiiValue, &Default->Value, sizeof (EFI_HII_VALUE));
          }
        }

        if (HiiValue->Type == EFI_IFR_TYPE_STRING) {
          StrValue = HiiGetString (FormSet->HiiHandle, HiiValue->Value.string, NULL);
          if (StrValue == NULL) {
            return EFI_NOT_FOUND;
          }
          if (Question->StorageWidth > StrSize (StrValue)) {
            ZeroMem (Question->BufferValue, Question->StorageWidth);
            CopyMem (Question->BufferValue, StrValue, StrSize (StrValue));
          } else {
            CopyMem (Question->BufferValue, StrValue, Question->StorageWidth);
          }
        }

        return EFI_SUCCESS;
      }

      Link = GetNextNode (&Question->DefaultListHead, Link);
    }
  }

  //
  // EFI_ONE_OF_OPTION
  //
  if ((Question->Operand == EFI_IFR_ONE_OF_OP) && !IsListEmpty (&Question->OptionListHead)) {
    if (DefaultId <= EFI_HII_DEFAULT_CLASS_MANUFACTURING)  {
      //
      // OneOfOption could only provide Standard and Manufacturing default
      //
      //DMSG ((EFI_D_ERROR, "GetQuestionDefault, EFI_IFR_ONE_OF_OP, \n"));
      Link = GetFirstNode (&Question->OptionListHead);
      while (!IsNull (&Question->OptionListHead, Link)) {
        Option = QUESTION_OPTION_FROM_LINK (Link);
        Link = GetNextNode (&Question->OptionListHead, Link);

        if ((Option->SuppressExpression != NULL) &&
            EvaluateExpressionList(Option->SuppressExpression, FALSE, NULL, NULL) != ExpressFalse) {
          continue;
        }

        if (((DefaultId == EFI_HII_DEFAULT_CLASS_STANDARD) && ((Option->Flags & EFI_IFR_OPTION_DEFAULT) != 0)) ||
            ((DefaultId == EFI_HII_DEFAULT_CLASS_MANUFACTURING) && ((Option->Flags & EFI_IFR_OPTION_DEFAULT_MFG) != 0))
           ) {
          CopyMem (HiiValue, &Option->Value, sizeof (EFI_HII_VALUE));

          return EFI_SUCCESS;
        }
      }
    }
  }

  //
  // EFI_IFR_CHECKBOX - lowest priority
  //
  if (Question->Operand == EFI_IFR_CHECKBOX_OP) {
    if (DefaultId <= EFI_HII_DEFAULT_CLASS_MANUFACTURING)  {
      //
      // Checkbox could only provide Standard and Manufacturing default
      //
      //DMSG ((EFI_D_ERROR, "GetQuestionDefault, EFI_IFR_CHECKBOX_OP, \n"));
      if (((DefaultId == EFI_HII_DEFAULT_CLASS_STANDARD) && ((Question->Flags & EFI_IFR_CHECKBOX_DEFAULT) != 0)) ||
          ((DefaultId == EFI_HII_DEFAULT_CLASS_MANUFACTURING) && ((Question->Flags & EFI_IFR_CHECKBOX_DEFAULT_MFG) != 0))
         ) {
        HiiValue->Value.b = TRUE;
      }

      return EFI_SUCCESS;
    }
  }

  //
  // For question without default value for current default Id, we try to re-get the default value form other default id in the DefaultStoreList.
  // If get, will exit the function, if not, will choose next default id in the DefaultStoreList.
  // The default id in DefaultStoreList are in ascending order to make sure choose the smallest default id every time.
  //
  while (!IsNull(&FormSet->DefaultStoreListHead, DefaultLink)) {
    DefaultStore = FORMSET_DEFAULTSTORE_FROM_LINK(DefaultLink);
    DefaultLink = GetNextNode (&FormSet->DefaultStoreListHead,DefaultLink);
    DefaultId = DefaultStore->DefaultId;
    if (DefaultId == OriginalDefaultId) {
      continue;
    }
    goto ReGetDefault;
  }

  //
  // For Questions without default value for all the default id in the DefaultStoreList.
  //
  //DMSG ((EFI_D_ERROR, "GetQuestionDefault, without default, Operand :0x%x. \n", Question->Operand));
  Status = EFI_NOT_FOUND;
  switch (Question->Operand) {
  case EFI_IFR_CHECKBOX_OP:
    HiiValue->Value.b = FALSE;
    Status = EFI_SUCCESS;
    break;

  case EFI_IFR_NUMERIC_OP:
    //
    // Take minimum value as numeric default value
    //
    if ((Question->Flags & EFI_IFR_DISPLAY) == 0) {
      //
      // In EFI_IFR_DISPLAY_INT_DEC type, should check value with int* type.
      //
      switch (Question->Flags & EFI_IFR_NUMERIC_SIZE) {
      case EFI_IFR_NUMERIC_SIZE_1:
        if (((INT8) HiiValue->Value.u8 < (INT8) Question->Minimum) || ((INT8) HiiValue->Value.u8 > (INT8) Question->Maximum)) {
          HiiValue->Value.u8 = (UINT8) Question->Minimum;
          Status = EFI_SUCCESS;
        }
        break;
      case EFI_IFR_NUMERIC_SIZE_2:
        if (((INT16) HiiValue->Value.u16 < (INT16) Question->Minimum) || ((INT16) HiiValue->Value.u16 > (INT16) Question->Maximum)) {
          HiiValue->Value.u16 = (UINT16) Question->Minimum;
          Status = EFI_SUCCESS;
        }
        break;
      case EFI_IFR_NUMERIC_SIZE_4:
        if (((INT32) HiiValue->Value.u32 < (INT32) Question->Minimum) || ((INT32) HiiValue->Value.u32 > (INT32) Question->Maximum)) {
          HiiValue->Value.u32 = (UINT32) Question->Minimum;
          Status = EFI_SUCCESS;
        }
        break;
      case EFI_IFR_NUMERIC_SIZE_8:
        if (((INT64) HiiValue->Value.u64 < (INT64) Question->Minimum) || ((INT64) HiiValue->Value.u64 > (INT64) Question->Maximum)) {
          HiiValue->Value.u64 = Question->Minimum;
          Status = EFI_SUCCESS;
        }
        break;
      default:
        break;
      }
    } else {
      if ((HiiValue->Value.u64 < Question->Minimum) || (HiiValue->Value.u64 > Question->Maximum)) {
        HiiValue->Value.u64 = Question->Minimum;
        Status = EFI_SUCCESS;
      }
    }
    break;

  case EFI_IFR_ONE_OF_OP:
    //
    // Take first oneof option as oneof's default value
    //
    if (ValueToOption (Question, HiiValue) == NULL) {
      Link = GetFirstNode (&Question->OptionListHead);
      while (!IsNull (&Question->OptionListHead, Link)) {
        Option = QUESTION_OPTION_FROM_LINK (Link);
        Link = GetNextNode (&Question->OptionListHead, Link);

        if ((Option->SuppressExpression != NULL) &&
            EvaluateExpressionList(Option->SuppressExpression, FALSE, NULL, NULL) != ExpressFalse) {
          continue;
        }

        CopyMem (HiiValue, &Option->Value, sizeof (EFI_HII_VALUE));
        Status = EFI_SUCCESS;
        break;
      }
    }
    break;

  case EFI_IFR_ORDERED_LIST_OP:
    //
    // Take option sequence in IFR as ordered list's default value
    //
    Index = 0;
    Link = GetFirstNode (&Question->OptionListHead);
    while (!IsNull (&Question->OptionListHead, Link)) {
      Status = EFI_SUCCESS;
      Option = QUESTION_OPTION_FROM_LINK (Link);
      Link = GetNextNode (&Question->OptionListHead, Link);

      if ((Option->SuppressExpression != NULL) &&
          EvaluateExpressionList(Option->SuppressExpression, FALSE, NULL, NULL) != ExpressFalse) {
        continue;
      }

      SetArrayData (Question->BufferValue, Question->ValueType, Index, Option->Value.Value.u64);

      Index++;
      if (Index >= Question->MaxContainers) {
        break;
      }
    }
    break;

  default:
    break;
  }

  return Status;
}

/**
  Get AltCfg string for current form.

  @param  FormSet                Form data structure.
  @param  Form                   Form data structure.
  @param  DefaultId              The Class of the default.
  @param  BrowserStorage         The input request storage for the questions.

**/
VOID
ExtractAltCfgForForm (
  IN FORM_BROWSER_FORMSET   *FormSet,
  IN FORM_BROWSER_FORM      *Form,
  IN UINT16                 DefaultId,
  IN BROWSER_STORAGE        *BrowserStorage
  )
{
  EFI_STATUS                   Status;
  LIST_ENTRY                   *Link;
  CHAR16                       *ConfigResp;
  CHAR16                       *Progress;
  CHAR16                       *Result;
  BROWSER_STORAGE              *Storage;
  FORM_BROWSER_CONFIG_REQUEST  *ConfigInfo;
  FORMSET_STORAGE              *FormSetStorage;

  //DMSG ((EFI_D_ERROR, "\n ExtractAltCfgForForm,  \n"));
  //DMSG_HII(Form->FormTitle, FormSet->HiiHandle, L"FormTitle");
  //
  // Check whether has get AltCfg string for this formset.
  // If yes, no need to get AltCfg for form.
  //
  Link = GetFirstNode (&FormSet->StorageListHead);
  while (!IsNull (&FormSet->StorageListHead, Link)) {
    FormSetStorage = FORMSET_STORAGE_FROM_LINK (Link);
    Storage        = FormSetStorage->BrowserStorage;
    Link = GetNextNode (&FormSet->StorageListHead, Link);
    if (BrowserStorage != NULL && BrowserStorage != Storage) {
      continue;
    }

    if (Storage->Type != EFI_HII_VARSTORE_EFI_VARIABLE &&
        FormSetStorage->ElementCount != 0 &&
        FormSetStorage->HasCallAltCfg) {
      return;
    }
  }

  //
  // Get AltCfg string for each form.
  //
  Link = GetFirstNode (&Form->ConfigRequestHead);
  while (!IsNull (&Form->ConfigRequestHead, Link)) {
    ConfigInfo = FORM_BROWSER_CONFIG_REQUEST_FROM_LINK (Link);
    Link = GetNextNode (&Form->ConfigRequestHead, Link);

    Storage = ConfigInfo->Storage;
    if (BrowserStorage != NULL && BrowserStorage != Storage) {
      continue;
    }

    if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
      continue;
    }

    //
    // 1. Skip if there is no RequestElement
    //
    if (ConfigInfo->ElementCount == 0) {
      continue;
    }

    //
    // 2. Get value through hii config routine protocol.
    //
    Status = mHiiConfigRouting->ExtractConfig (
                                      mHiiConfigRouting,
                                      ConfigInfo->ConfigRequest,
                                      &Progress,
                                      &Result
                                      );
    //DMSG ((EFI_D_ERROR, "ExtractAltCfgForForm, ExtractConfig :%r. \n", Status));
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // 3. Call ConfigRouting GetAltCfg(ConfigRoute, <ConfigResponse>, Guid, Name, DevicePath, AltCfgId, AltCfgResp)
    //    Get the default configuration string according to the default ID.
    //
    Status = mHiiConfigRouting->GetAltConfig (
                                  mHiiConfigRouting,
                                  Result,
                                  &Storage->Guid,
                                  Storage->Name,
                                  NULL,
                                  &DefaultId,  // it can be NULL to get the current setting.
                                  &ConfigResp
                                );
    FreePool (Result);
    //DMSG ((EFI_D_ERROR, "ExtractAltCfgForForm, GetAltConfig :%r. \n", Status));
    if (EFI_ERROR (Status)) {
      continue;
    }

    ConfigInfo->ConfigAltResp = ConfigResp;
  }
}

/**
  Clean AltCfg string for current form.

  @param  Form                   Form data structure.

**/
VOID
CleanAltCfgForForm (
  IN FORM_BROWSER_FORM   *Form
  )
{
  LIST_ENTRY              *Link;
  FORM_BROWSER_CONFIG_REQUEST  *ConfigInfo;

  Link = GetFirstNode (&Form->ConfigRequestHead);
  while (!IsNull (&Form->ConfigRequestHead, Link)) {
    ConfigInfo = FORM_BROWSER_CONFIG_REQUEST_FROM_LINK (Link);
    Link = GetNextNode (&Form->ConfigRequestHead, Link);

    if (ConfigInfo->ConfigAltResp != NULL) {
      FreePool (ConfigInfo->ConfigAltResp);
      ConfigInfo->ConfigAltResp = NULL;
    }
  }
}

/**
  Get AltCfg string for current formset.

  @param  FormSet                Form data structure.
  @param  DefaultId              The Class of the default.
  @param  BrowserStorage         The input request storage for the questions.

**/
VOID
ExtractAltCfgForFormSet (
  IN FORM_BROWSER_FORMSET   *FormSet,
  IN UINT16                 DefaultId,
  IN BROWSER_STORAGE        *BrowserStorage
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *Link;
  CHAR16                  *ConfigResp;
  CHAR16                  *Progress;
  CHAR16                  *Result;
  BROWSER_STORAGE         *Storage;
  FORMSET_STORAGE         *FormSetStorage;

  //DMSG ((EFI_D_ERROR, "\n ExtractAltCfgForFormSet, \n"));
  //DMSG_HII(FormSet->FormSetTitle, FormSet->HiiHandle, L"FormSetTitle");
  Link = GetFirstNode (&FormSet->StorageListHead);
  while (!IsNull (&FormSet->StorageListHead, Link)) {
    FormSetStorage = FORMSET_STORAGE_FROM_LINK (Link);
    Storage        = FormSetStorage->BrowserStorage;
    Link = GetNextNode (&FormSet->StorageListHead, Link);

    if (BrowserStorage != NULL && BrowserStorage != Storage) {
      continue;
    }

    if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE) {
      continue;
    }

    //
    // 1. Skip if there is no RequestElement
    //
    if (FormSetStorage->ElementCount == 0) {
      continue;
    }

    FormSetStorage->HasCallAltCfg = TRUE;

    //
    // 2. Get value through hii config routine protocol.
    //
    Status = mHiiConfigRouting->ExtractConfig (
                                      mHiiConfigRouting,
                                      FormSetStorage->ConfigRequest,
                                      &Progress,
                                      &Result
                                      );
    //DMSG ((EFI_D_ERROR, "ExtractAltCfgForFormSet, ExtractConfig :%r. \n", Status));
    if (EFI_ERROR (Status)) {
      continue;
    }

    //
    // 3. Call ConfigRouting GetAltCfg(ConfigRoute, <ConfigResponse>, Guid, Name, DevicePath, AltCfgId, AltCfgResp)
    //    Get the default configuration string according to the default ID.
    //
    Status = mHiiConfigRouting->GetAltConfig (
                                  mHiiConfigRouting,
                                  Result,
                                  &Storage->Guid,
                                  Storage->Name,
                                  NULL,
                                  &DefaultId,  // it can be NULL to get the current setting.
                                  &ConfigResp
                                );

    FreePool (Result);
    //DMSG ((EFI_D_ERROR, "ExtractAltCfgForFormSet, GetAltConfig :%r. \n", Status));
    if (EFI_ERROR (Status)) {
      continue;
    }

    FormSetStorage->ConfigAltResp = ConfigResp;
  }

}

/**
  Clean AltCfg string for current formset.

  @param  FormSet                Form data structure.

**/
VOID
CleanAltCfgForFormSet (
  IN FORM_BROWSER_FORMSET   *FormSet
  )
{
  LIST_ENTRY              *Link;
  FORMSET_STORAGE         *FormSetStorage;

  Link = GetFirstNode (&FormSet->StorageListHead);
  while (!IsNull (&FormSet->StorageListHead, Link)) {
    FormSetStorage = FORMSET_STORAGE_FROM_LINK (Link);
    Link = GetNextNode (&FormSet->StorageListHead, Link);

    if (FormSetStorage->ConfigAltResp != NULL) {
      FreePool (FormSetStorage->ConfigAltResp);
      FormSetStorage->ConfigAltResp = NULL;
    }

    FormSetStorage->HasCallAltCfg = FALSE;
  }
}

/**
  Reset Questions to their initial value or default value in a Form, Formset or System.

  GetDefaultValueScope parameter decides which questions will reset
  to its default value.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  DefaultId              The Class of the default.
  @param  SettingScope           Setting Scope for Default action.
  @param  GetDefaultValueScope   Get default value scope.
  @param  Storage                Get default value only for this storage.
  @param  RetrieveValueFirst     Whether call the retrieve call back to
                                 get the initial value before get default
                                 value.
  @param  SkipGetAltCfg          Whether skip the get altcfg string process.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_UNSUPPORTED        Unsupported SettingScope.

**/
EFI_STATUS
ExtractDefault (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN UINT16                           DefaultId,
  IN BROWSER_SETTING_SCOPE            SettingScope,
  IN BROWSER_GET_DEFAULT_VALUE        GetDefaultValueScope,
  IN BROWSER_STORAGE                  *Storage OPTIONAL,
  IN BOOLEAN                          RetrieveValueFirst,
  IN BOOLEAN                          SkipGetAltCfg
  )
{
  EFI_STATUS              Status;
  LIST_ENTRY              *FormLink;
  LIST_ENTRY              *Link;
  FORM_BROWSER_STATEMENT  *Question;
  FORM_BROWSER_FORMSET    *LocalFormSet;
  FORM_BROWSER_FORMSET    *OldFormSet;

  Status = EFI_SUCCESS;

  //
  // Check the supported setting level.
  //
  if (SettingScope >= MaxLevel || GetDefaultValueScope >= GetDefaultForMax) {
    return EFI_UNSUPPORTED;
  }

  if (GetDefaultValueScope == GetDefaultForStorage && Storage == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (SettingScope == FormLevel) {
    //DMSG_HII(Form->FormTitle, FormSet->HiiHandle, L"FormLevel");
    //
    // Prepare the AltCfg String for form.
    //
    if (!SkipGetAltCfg && (GetDefaultValueScope != GetDefaultForNoStorage)) {
      ExtractAltCfgForForm (FormSet, Form, DefaultId, Storage);
    }

    //
    // Extract Form default
    //
    Link = GetFirstNode (&Form->StatementListHead);
    while (!IsNull (&Form->StatementListHead, Link)) {
      Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);
      Link = GetNextNode (&Form->StatementListHead, Link);

      //
      // If get default value only for this storage, check the storage first.
      //
      if ((GetDefaultValueScope == GetDefaultForStorage) && (Question->Storage != Storage)) {
        continue;
      }

      //
      // If get default value only for no storage question, just skip the question which has storage.
      //
      if ((GetDefaultValueScope == GetDefaultForNoStorage) && (Question->Storage != NULL)) {
        continue;
      }

      //
      // If Question is disabled, don't reset it to default
      //
      if (Question->Expression != NULL) {
        if (EvaluateExpressionList(Question->Expression, TRUE, FormSet, Form) == ExpressDisable) {
          continue;
        }
      }

      if (RetrieveValueFirst) {
        //
        // Call the Retrieve call back to get the initial question value.
        //
        Status = ProcessRetrieveForQuestion(FormSet->ConfigAccess, Question, FormSet);
      }

      //
      // If not request to get the initial value or get initial value fail, then get default value.
      //
      if (!RetrieveValueFirst || EFI_ERROR (Status)) {
        Status = GetQuestionDefault (FormSet, Form, Question, DefaultId);
        if (EFI_ERROR (Status)) {
          continue;
        }
      }

      //
      // Synchronize Buffer storage's Edit buffer
      //
      if ((Question->Storage != NULL) &&
          (Question->Storage->Type != EFI_HII_VARSTORE_EFI_VARIABLE)) {
        SetQuestionValue (FormSet, Form, Question, GetSetValueWithEditBuffer);
      }
    }

    //
    // Clean the AltCfg String.
    //
    if (!SkipGetAltCfg && (GetDefaultValueScope != GetDefaultForNoStorage)) {
      CleanAltCfgForForm(Form);
    }
  } else if (SettingScope == FormSetLevel) {

    //DMSG ((EFI_D_ERROR, "\n ExtractDefault, FormSetLevel, \n"));	
    //DMSG_HII(FormSet->FormSetTitle, FormSet->HiiHandle, L"FormSetTitle");
    //
    // Prepare the AltCfg String for formset.
    //
    if (!SkipGetAltCfg && (GetDefaultValueScope != GetDefaultForNoStorage)) {
      //DMSG ((EFI_D_ERROR, "ExtractDefault, ExtractAltCfgForFormSet, \n"));
      ExtractAltCfgForFormSet (FormSet, DefaultId, Storage);
    }

    FormLink = GetFirstNode (&FormSet->FormListHead);
    while (!IsNull (&FormSet->FormListHead, FormLink)) {
      Form = FORM_BROWSER_FORM_FROM_LINK (FormLink);
      ExtractDefault (FormSet, Form, DefaultId, FormLevel, GetDefaultValueScope, Storage, RetrieveValueFirst, SkipGetAltCfg);
      FormLink = GetNextNode (&FormSet->FormListHead, FormLink);
    }

    //
    // Clean the AltCfg String.
    //
    if (!SkipGetAltCfg && (GetDefaultValueScope != GetDefaultForNoStorage)) {
      CleanAltCfgForFormSet (FormSet);
    }
	
  } else if (SettingScope == SystemLevel) {

    //DMSG ((EFI_D_ERROR, "\n ExtractDefault, Begin, GetDefaultValueScope :%d. \n", GetDefaultValueScope));
    //
    // Preload all Hii formset.
    // 
    // LoadAllHiiFormset();	
    if (PcdGet8 (PcdDisplaySetupMessage) == 1) {
      Status = gBS->SetTimer(mTimerEvent, TimerPeriodic, 10000000);
      mRecoveryDefault = TRUE;
      TimerInSecondsCallBack (mTimerEvent, NULL);
    }
    OldFormSet = mSystemLevelFormSet;	
    //
    // Set Default Value for each FormSet in the maintain list.
    //
    Link = GetFirstNode (&gBrowserFormSetList);
    while (!IsNull (&gBrowserFormSetList, Link)) {
      LocalFormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
      Link = GetNextNode (&gBrowserFormSetList, Link);
      if (!ValidateFormSet(LocalFormSet)) {
        continue;
      }

      mSystemLevelFormSet = LocalFormSet;

      #ifdef SETUP_DEBUG	  
      //
      //Show Status Info to user.
      //
      if (LocalFormSet->FormSetTitle) {
        CHAR16    *FormSetTitle;
        FormSetTitle = GetToken (LocalFormSet->FormSetTitle, LocalFormSet->HiiHandle);
        UiConfirmDialog(DIALOG_NO_REPAINT, mCustomString->StrLoadFormset, NULL, TEXT_ALIGIN_CENTER, gEmptyString, FormSetTitle, NULL);
        if (FormSetTitle) {
          FreePool (FormSetTitle);
        }
      }
      #endif
	  
      ExtractDefault (LocalFormSet, NULL, DefaultId, FormSetLevel, GetDefaultValueScope, Storage, RetrieveValueFirst, SkipGetAltCfg);
    }

    mSystemLevelFormSet = OldFormSet;

    TriggerSetupNotify(SetupSaveNotifyTypeLoadDefault);
    #ifdef SETUP_DEBUG	  
    UiClearNoRepaintFlag(DIALOG_NO_REPAINT);
    #endif
    //DMSG ((EFI_D_ERROR, "ExtractDefault, End. \n"));
    gBS->SetTimer(mTimerEvent, TimerCancel, 0);
    mCount = 0;
    UiClearNoRepaintFlag(DIALOG_NO_REPAINT);
  }

  return EFI_SUCCESS;
}


/**
  Validate whether this question's value has changed.

  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.
  @param  Question               Question to be initialized.
  @param  GetValueFrom           Where to get value, may from editbuffer, buffer or hii driver.

  @retval TRUE                   Question's value has changed.
  @retval FALSE                  Question's value has not changed

**/
BOOLEAN
IsQuestionValueChanged (
  IN FORM_BROWSER_FORMSET             *FormSet,
  IN FORM_BROWSER_FORM                *Form,
  IN OUT FORM_BROWSER_STATEMENT       *Question,
  IN GET_SET_QUESTION_VALUE_WITH      GetValueFrom
  )
{
  EFI_HII_VALUE    BackUpValue;
  CHAR8            *BackUpBuffer;
  EFI_HII_VALUE    BackUpValue2;
  CHAR8            *BackUpBuffer2;
  EFI_STATUS       Status;
  BOOLEAN          ValueChanged;
  UINTN            BufferWidth;

  //
  // For question without storage, always mark it as data not changed.
  //
  if (Question->Storage == NULL && Question->Operand != EFI_IFR_TIME_OP && Question->Operand != EFI_IFR_DATE_OP) {
    return FALSE;
  }

  BackUpBuffer = NULL;
  BackUpBuffer2 = NULL;
  ValueChanged = FALSE;

  switch (Question->Operand) {
    case EFI_IFR_ORDERED_LIST_OP:
      BufferWidth  = Question->StorageWidth;
      BackUpBuffer = AllocateCopyPool (BufferWidth, Question->BufferValue);
      ASSERT (BackUpBuffer != NULL);
      break;

    case EFI_IFR_STRING_OP:
    case EFI_IFR_PASSWORD_OP:
      BufferWidth  = (UINTN) Question->Maximum * sizeof (CHAR16);
      BackUpBuffer = AllocateCopyPool (BufferWidth, Question->BufferValue);
      ASSERT (BackUpBuffer != NULL);
      break;

    default:
      BufferWidth = 0;
      break;
  }
  CopyMem (&BackUpValue, &Question->HiiValue, sizeof (EFI_HII_VALUE));

  if (GetValueFrom == GetSetValueWithBothBuffer) {
    Status = GetQuestionValue (FormSet, Form, Question, GetSetValueWithEditBuffer);
    ASSERT_EFI_ERROR(Status);

    switch (Question->Operand) {
      case EFI_IFR_ORDERED_LIST_OP:
        BufferWidth  = Question->StorageWidth;
        BackUpBuffer2 = AllocateCopyPool (BufferWidth, Question->BufferValue);
        ASSERT (BackUpBuffer2 != NULL);
        break;

      case EFI_IFR_STRING_OP:
      case EFI_IFR_PASSWORD_OP:
        BufferWidth  = (UINTN) Question->Maximum * sizeof (CHAR16);
        BackUpBuffer2 = AllocateCopyPool (BufferWidth, Question->BufferValue);
        ASSERT (BackUpBuffer2 != NULL);
        break;

      default:
        BufferWidth = 0;
        break;
    }
    CopyMem (&BackUpValue2, &Question->HiiValue, sizeof (EFI_HII_VALUE));

    Status = GetQuestionValue (FormSet, Form, Question, GetSetValueWithBuffer);
    ASSERT_EFI_ERROR(Status);

    if (CompareMem (&BackUpValue2, &Question->HiiValue, sizeof (EFI_HII_VALUE)) != 0 ||
        CompareMem (BackUpBuffer2, Question->BufferValue, BufferWidth) != 0) {
      ValueChanged = TRUE;
      //DMSG_HII(Question->Prompt, FormSet->HiiHandle, L"Value Changed");
    }
  } else {
    Status = GetQuestionValue (FormSet, Form, Question, GetValueFrom);
    ASSERT_EFI_ERROR(Status);

    if (CompareMem (&BackUpValue, &Question->HiiValue, sizeof (EFI_HII_VALUE)) != 0 ||
        CompareMem (BackUpBuffer, Question->BufferValue, BufferWidth) != 0) {
      ValueChanged = TRUE;
      //DMSG_HII(Question->Prompt, FormSet->HiiHandle, L"Value Changed");
    }
  }

  CopyMem (&Question->HiiValue, &BackUpValue, sizeof (EFI_HII_VALUE));
  if (BackUpBuffer != NULL) {
    CopyMem (Question->BufferValue, BackUpBuffer, BufferWidth);
    FreePool (BackUpBuffer);
  }

  if (BackUpBuffer2 != NULL) {
    FreePool (BackUpBuffer2);
  }
  Question->ValueChanged = ValueChanged;

  return ValueChanged;
}

/**
  Initialize Question's Edit copy from Storage.

  @param  Selection              Selection contains the information about
                                 the Selection, form and formset to be displayed.
                                 Selection action may be updated in retrieve callback.
                                 If Selection is NULL, only initialize Question value.
  @param  FormSet                FormSet data structure.
  @param  Form                   Form data structure.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
LoadFormConfig (
  IN OUT UI_MENU_SELECTION    *Selection,
  IN FORM_BROWSER_FORMSET     *FormSet,
  IN FORM_BROWSER_FORM        *Form
  )
{
  EFI_STATUS                  Status;
  LIST_ENTRY                  *Link;
  FORM_BROWSER_STATEMENT      *Question;

  Link = GetFirstNode (&Form->StatementListHead);
  while (!IsNull (&Form->StatementListHead, Link)) {
    Question = FORM_BROWSER_STATEMENT_FROM_LINK (Link);
    //DMSG_HII(Question->Prompt, FormSet->HiiHandle, L"Question");	
    //
    // Initialize local copy of Value for each Question
    //
    if (Question->Operand == EFI_IFR_PASSWORD_OP && (Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK)== 0) {
      Status = GetQuestionValue (FormSet, Form, Question, GetSetValueWithHiiDriver);
    } else {
      Status = GetQuestionValue (FormSet, Form, Question, GetSetValueWithEditBuffer);
    }
    if (EFI_ERROR (Status)) {
      return Status;
    }

    if ((Question->Operand == EFI_IFR_STRING_OP) || (Question->Operand == EFI_IFR_PASSWORD_OP)) {
      HiiSetString (FormSet->HiiHandle, Question->HiiValue.Value.string, (CHAR16*)Question->BufferValue, NULL);
    }

    Link = GetNextNode (&Form->StatementListHead, Link);
  }

  return EFI_SUCCESS;
}

/**
  Initialize Question's Edit copy from Storage for the whole Formset.

  @param  Selection              Selection contains the information about
                                 the Selection, form and formset to be displayed.
                                 Selection action may be updated in retrieve callback.
                                 If Selection is NULL, only initialize Question value.
  @param  FormSet                FormSet data structure.

  @retval EFI_SUCCESS            The function completed successfully.

**/
EFI_STATUS
LoadFormSetConfig (
  IN OUT UI_MENU_SELECTION    *Selection,
  IN     FORM_BROWSER_FORMSET *FormSet
  )
{
  EFI_STATUS            Status;
  LIST_ENTRY            *Link;
  FORM_BROWSER_FORM     *Form;

  DMSG ((EFI_D_ERROR, "\n LoadFormSetConfig, Begin, \n"));
  DMSG_HII(FormSet->FormSetTitle, FormSet->HiiHandle, L"FormSetTitle");

  Link = GetFirstNode (&FormSet->FormListHead);
  while (!IsNull (&FormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);

    //
    // Initialize local copy of Value for each Form
    //    
    //DMSG ((EFI_D_ERROR, "\n LoadFormSetConfig, Form, \n"));
    //DMSG_HII(Form->FormTitle, FormSet->HiiHandle, L"FormTitle");
    Status = LoadFormConfig (Selection, FormSet, Form);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Link = GetNextNode (&FormSet->FormListHead, Link);
  }

  //
  // Finished question initialization.
  //
  FormSet->QuestionInited = TRUE;
  
  DMSG ((EFI_D_ERROR, "LoadFormSetConfig, End. \n"));
  return EFI_SUCCESS;
}

/**
  Remove the Request element from the Config Request.

  @param  Storage                Pointer to the browser storage.
  @param  RequestElement         The pointer to the Request element.

**/
VOID
RemoveElement (
  IN OUT BROWSER_STORAGE      *Storage,
  IN     CHAR16               *RequestElement
  )
{
  CHAR16   *NewStr;
  CHAR16   *DestStr;

  ASSERT (Storage->ConfigRequest != NULL && RequestElement != NULL);

  NewStr = StrStr (Storage->ConfigRequest, RequestElement);

  if (NewStr == NULL) {
    return;
  }

  //
  // Remove this element from this ConfigRequest.
  //
  DestStr = NewStr;
  NewStr += StrLen (RequestElement);
  CopyMem (DestStr, NewStr, StrSize (NewStr));

  Storage->SpareStrLen += StrLen (RequestElement);
}

/**
  Adjust config request in storage, remove the request elements existed in the input ConfigRequest.

  @param  Storage                Pointer to the formset storage.
  @param  ConfigRequest          The pointer to the Request element.

**/
VOID
RemoveConfigRequest (
  FORMSET_STORAGE   *Storage,
  CHAR16            *ConfigRequest
  )
{
  CHAR16       *RequestElement;
  CHAR16       *NextRequestElement;
  CHAR16       *SearchKey;


  //
  // No request element in it, just return.
  //
  if (ConfigRequest == NULL) {
    return;
  }

  if (Storage->BrowserStorage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    //
    // "&Name1&Name2" section for EFI_HII_VARSTORE_NAME_VALUE storage
    //
    SearchKey = L"&";
  } else {
    //
    // "&OFFSET=####&WIDTH=####" section for EFI_HII_VARSTORE_BUFFER storage
    //
    SearchKey = L"&OFFSET";
  }

  //
  // Find SearchKey storage
  //
  if (Storage->BrowserStorage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    RequestElement = StrStr (ConfigRequest, L"PATH");
    ASSERT (RequestElement != NULL);
    RequestElement = StrStr (RequestElement, SearchKey);
  } else {
    RequestElement = StrStr (ConfigRequest, SearchKey);
  }

  while (RequestElement != NULL) {
    //
    // +1 to avoid find header itself.
    //
    NextRequestElement = StrStr (RequestElement + 1, SearchKey);

    //
    // The last Request element in configRequest string.
    //
    if (NextRequestElement != NULL) {
      //
      // Replace "&" with '\0'.
      //
      *NextRequestElement = L'\0';
    }

    RemoveElement (Storage->BrowserStorage, RequestElement);

    if (NextRequestElement != NULL) {
      //
      // Restore '&' with '\0' for later used.
      //
      *NextRequestElement = L'&';
    }

    RequestElement = NextRequestElement;
  }

  //
  // If no request element remain, just remove the ConfigRequest string.
  //
  if (StrCmp (Storage->BrowserStorage->ConfigRequest, Storage->ConfigHdr) == 0) {
    FreePool (Storage->BrowserStorage->ConfigRequest);
    Storage->BrowserStorage->ConfigRequest = NULL;
    Storage->BrowserStorage->SpareStrLen   = 0;
  }
}

/**
  Base on the current formset info, clean the ConfigRequest string in browser storage.

  @param  FormSet                Pointer of the FormSet

**/
VOID
CleanBrowserStorage (
  IN OUT FORM_BROWSER_FORMSET  *FormSet
  )
{
  //
  // Do nothing.
  //
}

/**
  Check whether current element in the ConfigRequest string.

  @param  BrowserStorage                Storage which includes ConfigRequest.
  @param  RequestElement                New element need to check.

  @retval TRUE        The Element is in the ConfigRequest string.
  @retval FALSE       The Element not in the configRequest String.

**/
BOOLEAN
ElementValidation (
  BROWSER_STORAGE   *BrowserStorage,
  CHAR16            *RequestElement
  )
{
  return StrStr (BrowserStorage->ConfigRequest, RequestElement) != NULL ? TRUE : FALSE;
}

/**
  Append the Request element to the Config Request.

  @param  ConfigRequest          Current ConfigRequest info.
  @param  SpareStrLen            Current remain free buffer for config request.
  @param  RequestElement         New Request element.

**/
VOID
AppendConfigRequest (
  IN OUT CHAR16               **ConfigRequest,
  IN OUT UINTN                *SpareStrLen,
  IN     CHAR16               *RequestElement
  )
{
  CHAR16   *NewStr;
  UINTN    StringSize;
  UINTN    StrLength;
  UINTN    MaxLen;

  StrLength = StrLen (RequestElement);
  StringSize = (*ConfigRequest != NULL) ? StrSize (*ConfigRequest) : sizeof (CHAR16);
  MaxLen = StringSize / sizeof (CHAR16) + *SpareStrLen;

  //
  // Append <RequestElement> to <ConfigRequest>
  //
  if (StrLength > *SpareStrLen) {
    //
    // Old String buffer is not sufficient for RequestElement, allocate a new one
    //
    MaxLen = StringSize / sizeof (CHAR16) + CONFIG_REQUEST_STRING_INCREMENTAL;
    NewStr = AllocateZeroPool (MaxLen * sizeof (CHAR16));
    ASSERT (NewStr != NULL);

    if (*ConfigRequest != NULL) {
      CopyMem (NewStr, *ConfigRequest, StringSize);
      FreePool (*ConfigRequest);
    }
    *ConfigRequest = NewStr;
    *SpareStrLen   = CONFIG_REQUEST_STRING_INCREMENTAL;
  }

  StrCatS (*ConfigRequest, MaxLen, RequestElement);
  *SpareStrLen -= StrLength;
}

/**
  Adjust the config request info, remove the request elements which already in AllConfigRequest string.

  @param  Storage                Form set Storage.
  @param  Request                The input request string.
  @param  RespString             Whether the input is ConfigRequest or ConfigResp format.

  @retval TRUE                   Has element not covered by current used elements, need to continue to call ExtractConfig
  @retval FALSE                  All elements covered by current used elements.

**/
BOOLEAN
ConfigRequestAdjust (
  IN  BROWSER_STORAGE         *Storage,
  IN  CHAR16                  *Request,
  IN  BOOLEAN                 RespString
  )
{
  CHAR16       *RequestElement;
  CHAR16       *NextRequestElement;
  CHAR16       *NextElementBakup;
  CHAR16       *SearchKey;
  CHAR16       *ValueKey;
  BOOLEAN      RetVal;
  CHAR16       *ConfigRequest;

  RetVal         = FALSE;
  NextElementBakup = NULL;
  ValueKey         = NULL;

  if (Request != NULL) {
    ConfigRequest = Request;
  } else {
    ConfigRequest = Storage->ConfigRequest;
  }

  if (Storage->ConfigRequest == NULL) {
    Storage->ConfigRequest = AllocateCopyPool (StrSize (ConfigRequest), ConfigRequest);
    return TRUE;
  }

  if (Storage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    //
    // "&Name1&Name2" section for EFI_HII_VARSTORE_NAME_VALUE storage
    //
    SearchKey = L"&";
  } else {
    //
    // "&OFFSET=####&WIDTH=####" section for EFI_HII_VARSTORE_BUFFER storage
    //
    SearchKey = L"&OFFSET";
    ValueKey  = L"&VALUE";
  }

  //
  // Find SearchKey storage
  //
  if (Storage->Type == EFI_HII_VARSTORE_NAME_VALUE) {
    RequestElement = StrStr (ConfigRequest, L"PATH");
    ASSERT (RequestElement != NULL);
    RequestElement = StrStr (RequestElement, SearchKey);
  } else {
    RequestElement = StrStr (ConfigRequest, SearchKey);
  }

  while (RequestElement != NULL) {

    //
    // +1 to avoid find header itself.
    //
    NextRequestElement = StrStr (RequestElement + 1, SearchKey);

    //
    // The last Request element in configRequest string.
    //
    if (NextRequestElement != NULL) {
      if (RespString && (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER)) {
        NextElementBakup = NextRequestElement;
        NextRequestElement = StrStr (RequestElement, ValueKey);
        ASSERT (NextRequestElement != NULL);
      }
      //
      // Replace "&" with '\0'.
      //
      *NextRequestElement = L'\0';
    } else {
      if (RespString && (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER)) {
        NextElementBakup = NextRequestElement;
        NextRequestElement = StrStr (RequestElement, ValueKey);
        ASSERT (NextRequestElement != NULL);
        //
        // Replace "&" with '\0'.
        //
        *NextRequestElement = L'\0';
      }
    }

    if (!ElementValidation (Storage, RequestElement)) {
      //
      // Add this element to the Storage->BrowserStorage->AllRequestElement.
      //
      AppendConfigRequest(&Storage->ConfigRequest, &Storage->SpareStrLen, RequestElement);
      RetVal = TRUE;
    }

    if (NextRequestElement != NULL) {
      //
      // Restore '&' with '\0' for later used.
      //
      *NextRequestElement = L'&';
    }

    if (RespString && (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER)) {
      RequestElement = NextElementBakup;
    } else {
      RequestElement = NextRequestElement;
    }
  }

  return RetVal;
}

/**
  Fill storage's edit copy with settings requested from Configuration Driver.

  @param  FormSet                FormSet data structure.
  @param  Storage                Buffer Storage.

**/
VOID
LoadStorage (
  IN FORM_BROWSER_FORMSET    *FormSet,
  IN FORMSET_STORAGE         *Storage
  )
{
  EFI_STATUS  Status;
  EFI_STRING  Progress;
  EFI_STRING  Result;
  CHAR16      *StrPtr;
  EFI_STRING  ConfigRequest;
  UINTN       StrLen;

  ConfigRequest = NULL;
  
  DMSG ((EFI_D_ERROR, "\n LoadStorage, VarStoreId :0x%x, Storage :%d-%s.\n", Storage->VarStoreId, Storage->BrowserStorage->Type, Storage->BrowserStorage->Name));
  switch (Storage->BrowserStorage->Type) {
    case EFI_HII_VARSTORE_EFI_VARIABLE:
      return;

    case EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER:
      if (Storage->BrowserStorage->ConfigRequest != NULL) {
        DMSG ((EFI_D_ERROR, "LoadStorage, ConfigRequest is not NULL, \n"));
        if (!Storage->BrowserStorage->IsSkipRequestString) {
          ConfigRequestAdjust(Storage->BrowserStorage, Storage->ConfigRequest, FALSE);
        }
        return;
      }
      break;

    case EFI_HII_VARSTORE_BUFFER:
    case EFI_HII_VARSTORE_NAME_VALUE:
      //
      // Skip if there is no RequestElement.
      //
      if (Storage->ElementCount == 0) {
        return;
      }

      //
      // Just update the ConfigRequest, if storage already initialized.
      //
      if (Storage->BrowserStorage->Initialized) {
        ConfigRequestAdjust(Storage->BrowserStorage, Storage->ConfigRequest, FALSE);
        return;
      }

      Storage->BrowserStorage->Initialized = TRUE;
      break;

    default:
      return;
  }

  if (Storage->BrowserStorage->Type != EFI_HII_VARSTORE_NAME_VALUE) {
    //
    // Create the config request string to get all fields for this storage.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWW"followed by a Null-terminator
    //
    StrLen = StrSize (Storage->ConfigHdr) + 20 * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (StrLen);
    ASSERT (ConfigRequest != NULL);
    UnicodeSPrint (
               ConfigRequest,
               StrLen,
               L"%s&OFFSET=0&WIDTH=%04x",
               Storage->ConfigHdr,
               Storage->BrowserStorage->Size);
  } else {
    ConfigRequest = Storage->ConfigRequest;
  }

  //
  // Request current settings from Configuration Driver
  //
  Status = mHiiConfigRouting->ExtractConfig (
                                    mHiiConfigRouting,
                                    ConfigRequest,
                                    &Progress,
                                    &Result
                                    );
  DMSG ((EFI_D_ERROR, "LoadStorage, ExtractConfig :%r. \n", Status));
  //
  // If get value fail, extract default from IFR binary
  //
  if (EFI_ERROR (Status)) {
    ExtractDefault (FormSet, NULL, EFI_HII_DEFAULT_CLASS_STANDARD, FormSetLevel, GetDefaultForStorage, Storage->BrowserStorage, TRUE, TRUE);
  } else {
    //
    // Convert Result from <ConfigAltResp> to <ConfigResp>
    //
    StrPtr = StrStr (Result, L"&GUID=");
    if (StrPtr != NULL) {
      *StrPtr = L'\0';
    }

    Status = ConfigRespToStorage (Storage->BrowserStorage, Result);
    FreePool (Result);
  }

  Storage->BrowserStorage->ConfigRequest = AllocateCopyPool (StrSize (Storage->ConfigRequest), Storage->ConfigRequest);

  //
  // Input NULL for ConfigRequest field means sync all fields from editbuffer to buffer.
  //
  DMSG ((EFI_D_ERROR, "LoadStorage, sync all fields from editbuffer to buffer, \n"));
  SynchronizeStorage(Storage->BrowserStorage, NULL, TRUE);

  if (Storage->BrowserStorage->Type != EFI_HII_VARSTORE_NAME_VALUE) {
    if (ConfigRequest != NULL) {
      FreePool (ConfigRequest);
    }
  }
}

/**
  Get Value changed status from old question.

  @param  NewFormSet                FormSet data structure.
  @param  OldQuestion               Old question which has value changed.

**/
VOID
SyncStatusForQuestion (
  IN OUT FORM_BROWSER_FORMSET             *NewFormSet,
  IN     FORM_BROWSER_STATEMENT           *OldQuestion
  )
{
  LIST_ENTRY                  *Link;
  LIST_ENTRY                  *QuestionLink;
  FORM_BROWSER_FORM           *Form;
  FORM_BROWSER_STATEMENT      *Question;

  //
  // For each form in one formset.
  //
  Link = GetFirstNode (&NewFormSet->FormListHead);
  while (!IsNull (&NewFormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);
    Link = GetNextNode (&NewFormSet->FormListHead, Link);

    //
    // for each question in one form.
    //
    QuestionLink = GetFirstNode (&Form->StatementListHead);
    while (!IsNull (&Form->StatementListHead, QuestionLink)) {
      Question = FORM_BROWSER_STATEMENT_FROM_LINK (QuestionLink);
      QuestionLink = GetNextNode (&Form->StatementListHead, QuestionLink);

      if (Question->QuestionId == OldQuestion->QuestionId) {
        Question->ValueChanged = TRUE;
        return;
      }
    }
  }
}

/**
  Get Value changed status from old from.

  @param  NewFormSet                FormSet data structure.
  @param  OldQuestion               Old form which has value changed.

**/
VOID
SyncStatusForForm (
  IN OUT FORM_BROWSER_FORMSET             *NewFormSet,
  IN     FORM_BROWSER_FORM                *Form
  )
{
  LIST_ENTRY                  *Link;
  FORM_BROWSER_FORM           *NewForm;

  //
  // For each form in one formset.
  //
  Link = GetFirstNode (&NewFormSet->FormListHead);
  while (!IsNull (&NewFormSet->FormListHead, Link)) {
    NewForm = FORM_BROWSER_FORM_FROM_LINK (Link);
    Link = GetNextNode (&NewFormSet->FormListHead, Link);

    if (NewForm->FormId == Form->FormId) {
      NewForm->ValueChanged = TRUE;
      return;
    }
  }
}

/**
  Get Value changed status from old formset.

  @param  NewFormSet                FormSet data structure.
  @param  OldFormSet                FormSet data structure.

**/
VOID
SyncStatusForFormSet (
  IN OUT FORM_BROWSER_FORMSET             *NewFormSet,
  IN     FORM_BROWSER_FORMSET             *OldFormSet
  )
{
  LIST_ENTRY                  *Link;
  LIST_ENTRY                  *QuestionLink;
  FORM_BROWSER_FORM           *Form;
  FORM_BROWSER_STATEMENT      *Question;

  DMSG ((EFI_D_ERROR, "\n SyncStatusForFormSet, \n"));
  //
  // For each form in one formset.
  //
  Link = GetFirstNode (&OldFormSet->FormListHead);
  while (!IsNull (&OldFormSet->FormListHead, Link)) {
    Form = FORM_BROWSER_FORM_FROM_LINK (Link);
    Link = GetNextNode (&OldFormSet->FormListHead, Link);

    if (Form->ValueChanged) {
      SyncStatusForForm(NewFormSet, Form);
    }

    //
    // for each question in one form.
    //
    QuestionLink = GetFirstNode (&Form->StatementListHead);
    while (!IsNull (&Form->StatementListHead, QuestionLink)) {
      Question = FORM_BROWSER_STATEMENT_FROM_LINK (QuestionLink);
      QuestionLink = GetNextNode (&Form->StatementListHead, QuestionLink);

      if (!Question->ValueChanged) {
        continue;
      }

      //
      // Find the same question in new formset and update the value changed flag.
      //
      SyncStatusForQuestion (NewFormSet, Question);
    }
  }
}

/**
  Get current setting of Questions.

  @param  FormSet                FormSet data structure.

**/
VOID
InitializeCurrentSetting (
  IN OUT FORM_BROWSER_FORMSET             *FormSet
  )
{
  LIST_ENTRY              *Link;
  FORMSET_STORAGE         *Storage;
  FORM_BROWSER_FORMSET    *OldFormSet;

  DMSG ((EFI_D_ERROR, "\n InitializeCurrentSetting, Begin, \n"));
  DMSG_HII(FormSet->FormSetTitle, FormSet->HiiHandle, L"FormSetTitle");
  //
  // Try to find pre FormSet in the maintain backup list.
  // If old formset != NULL, destroy this formset. Add new formset to gBrowserFormSetList.
  //
  OldFormSet = GetFormSetFromHiiHandle (FormSet->HiiHandle);
  if (OldFormSet != NULL) {
    DMSG ((EFI_D_ERROR, "InitializeCurrentSetting, Remove Old Formset,\n"));
    SyncStatusForFormSet (FormSet, OldFormSet);
    RemoveEntryList (&OldFormSet->Link);
    DestroyFormSet (OldFormSet);
  }
  InsertTailList (&gBrowserFormSetList, &FormSet->Link);
  DMSG ((EFI_D_ERROR, "InitializeCurrentSetting, Insert to gBrowserFormSetList.\n"));

  //
  // Extract default from IFR binary for no storage questions.
  //
  DMSG ((EFI_D_ERROR, "InitializeCurrentSetting, ExtractDefault,\n"));
  ExtractDefault (FormSet, NULL, EFI_HII_DEFAULT_CLASS_STANDARD, FormSetLevel, GetDefaultForNoStorage, NULL, TRUE, FALSE);

  //
  // Request current settings from Configuration Driver
  //
  DMSG ((EFI_D_ERROR, "InitializeCurrentSetting, LoadStorage,\n"));
  Link = GetFirstNode (&FormSet->StorageListHead);
  while (!IsNull (&FormSet->StorageListHead, Link)) {
    Storage = FORMSET_STORAGE_FROM_LINK (Link);

    LoadStorage (FormSet, Storage);

    Link = GetNextNode (&FormSet->StorageListHead, Link);
  }
  
  DMSG ((EFI_D_ERROR, "InitializeCurrentSetting, End. \n"));
}


/**
  Fetch the Ifr binary data of a FormSet.

  @param  Handle                 PackageList Handle
  @param  FormSetGuid            On input, GUID or class GUID of a formset. If not
                                 specified (NULL or zero GUID), take the first
                                 FormSet with class GUID EFI_HII_PLATFORM_SETUP_FORMSET_GUID
                                 found in package list.
                                 On output, GUID of the formset found(if not NULL).
  @param  BinaryLength           The length of the FormSet IFR binary.
  @param  BinaryData             The buffer designed to receive the FormSet.

  @retval EFI_SUCCESS            Buffer filled with the requested FormSet.
                                 BufferLength was updated.
  @retval EFI_INVALID_PARAMETER  The handle is unknown.
  @retval EFI_NOT_FOUND          A form or FormSet on the requested handle cannot
                                 be found with the requested FormId.

**/
EFI_STATUS
GetIfrBinaryData (
  IN  EFI_HII_HANDLE   Handle,
  IN OUT EFI_GUID      *FormSetGuid,
  OUT UINTN            *BinaryLength,
  OUT UINT8            **BinaryData
  )
{
  EFI_STATUS                   Status;
  EFI_HII_PACKAGE_LIST_HEADER  *HiiPackageList;
  UINTN                        BufferSize;
  UINT8                        *Package;
  UINT8                        *OpCodeData;
  UINT32                       Offset;
  UINT32                       Offset2;
  UINT32                       PackageListLength;
  EFI_HII_PACKAGE_HEADER       PackageHeader;
  UINT8                        Index;
  UINT8                        NumberOfClassGuid;
  BOOLEAN                      ClassGuidMatch;
  EFI_GUID                     *ClassGuid;
  EFI_GUID                     *ComparingGuid;

  OpCodeData = NULL;
  Package = NULL;
  ZeroMem (&PackageHeader, sizeof (EFI_HII_PACKAGE_HEADER));

  //
  // if FormSetGuid is NULL or zero GUID, return first Setup FormSet in the package list
  //
  if (FormSetGuid == NULL) {
    ComparingGuid = &gZeroGuid;
  } else {
    ComparingGuid = FormSetGuid;
  }

  //
  // Get HII PackageList
  //
  BufferSize = 0;
  HiiPackageList = NULL;
  Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, Handle, &BufferSize, HiiPackageList);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    HiiPackageList = AllocatePool (BufferSize);
    ASSERT (HiiPackageList != NULL);

    Status = mHiiDatabase->ExportPackageLists (mHiiDatabase, Handle, &BufferSize, HiiPackageList);
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ASSERT (HiiPackageList != NULL);

  //
  // Get Form package from this HII package List
  //
  Offset = sizeof (EFI_HII_PACKAGE_LIST_HEADER);
  Offset2 = 0;
  CopyMem (&PackageListLength, &HiiPackageList->PackageLength, sizeof (UINT32));

  ClassGuidMatch = FALSE;
  while (Offset < PackageListLength) {
    Package = ((UINT8 *) HiiPackageList) + Offset;
    CopyMem (&PackageHeader, Package, sizeof (EFI_HII_PACKAGE_HEADER));

    if (PackageHeader.Type == EFI_HII_PACKAGE_FORMS) {
      //
      // Search FormSet in this Form Package
      //
      Offset2 = sizeof (EFI_HII_PACKAGE_HEADER);
      while (Offset2 < PackageHeader.Length) {
        OpCodeData = Package + Offset2;

        if (((EFI_IFR_OP_HEADER *) OpCodeData)->OpCode == EFI_IFR_FORM_SET_OP) {
          //
          // Try to compare against formset GUID
          //
          if (IsZeroGuid (FormSetGuid) ||
              CompareGuid (ComparingGuid, (EFI_GUID *)(OpCodeData + sizeof (EFI_IFR_OP_HEADER)))) {
            break;
          }

          if (((EFI_IFR_OP_HEADER *) OpCodeData)->Length > OFFSET_OF (EFI_IFR_FORM_SET, Flags)) {
            //
            // Try to compare against formset class GUID
            //
            NumberOfClassGuid = (UINT8) (((EFI_IFR_FORM_SET *) OpCodeData)->Flags & 0x3);
            ClassGuid         = (EFI_GUID *) (OpCodeData + sizeof (EFI_IFR_FORM_SET));
            for (Index = 0; Index < NumberOfClassGuid; Index++) {
              if (CompareGuid (ComparingGuid, ClassGuid + Index)) {
                ClassGuidMatch = TRUE;
                break;
              }
            }
            if (ClassGuidMatch) {
              break;
            }
          } else if (ComparingGuid == &gEfiHiiPlatformSetupFormsetGuid) {
            ClassGuidMatch = TRUE;
            break;
          }
        }

        Offset2 += ((EFI_IFR_OP_HEADER *) OpCodeData)->Length;
      }

      if (Offset2 < PackageHeader.Length) {
        //
        // Target formset found
        //
        break;
      }
    }

    Offset += PackageHeader.Length;
  }

  if (Offset >= PackageListLength) {
    //
    // Form package not found in this Package List
    //
    FreePool (HiiPackageList);
    return EFI_NOT_FOUND;
  }

  if (FormSetGuid != NULL) {
    //
    // Return the FormSet GUID
    //
    CopyMem (FormSetGuid, &((EFI_IFR_FORM_SET *) OpCodeData)->Guid, sizeof (EFI_GUID));
  }

  //
  // To determine the length of a whole FormSet IFR binary, one have to parse all the Opcodes
  // in this FormSet; So, here just simply copy the data from start of a FormSet to the end
  // of the Form Package.
  //
  *BinaryLength = PackageHeader.Length - Offset2;
  *BinaryData = AllocateCopyPool (*BinaryLength, OpCodeData);

  FreePool (HiiPackageList);

  if (*BinaryData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}


/**
  Initialize the internal data structure of a FormSet.

  @param  Handle                 PackageList Handle
  @param  FormSetGuid            On input, GUID or class GUID of a formset. If not
                                 specified (NULL or zero GUID), take the first
                                 FormSet with class GUID EFI_HII_PLATFORM_SETUP_FORMSET_GUID
                                 found in package list.
                                 On output, GUID of the formset found(if not NULL).
  @param  FormSet                FormSet data structure.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_NOT_FOUND          The specified FormSet could not be found.

**/
EFI_STATUS
InitializeFormSet (
  IN  EFI_HII_HANDLE                   Handle,
  IN OUT EFI_GUID                      *FormSetGuid,
  OUT FORM_BROWSER_FORMSET             *FormSet
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                DriverHandle;

  DMSG ((EFI_D_ERROR, "\n InitializeFormSet, \n"));
  Status = GetIfrBinaryData (Handle, FormSetGuid, &FormSet->IfrBinaryLength, &FormSet->IfrBinaryData);
  DMSG ((EFI_D_ERROR, "InitializeFormSet, GetIfrBinaryData :%r.\n", Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  FormSet->Signature = FORM_BROWSER_FORMSET_SIGNATURE;
  FormSet->HiiHandle = Handle;
  CopyMem (&FormSet->Guid, FormSetGuid, sizeof (EFI_GUID));
  FormSet->QuestionInited = FALSE;

  //
  // Retrieve ConfigAccess Protocol associated with this HiiPackageList
  //
  Status = mHiiDatabase->GetPackageListHandle (mHiiDatabase, Handle, &DriverHandle);
  DMSG ((EFI_D_ERROR, "InitializeFormSet, GetPackageListHandle :%r.\n", Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }
  FormSet->DriverHandle = DriverHandle;
  Status = gBS->HandleProtocol (
                  DriverHandle,
                  &gEfiHiiConfigAccessProtocolGuid,
                  (VOID **) &FormSet->ConfigAccess
                  );
  DMSG ((EFI_D_ERROR, "InitializeFormSet, Get ConfigAccess :%r.\n", Status));
  if (EFI_ERROR (Status)) {
    //
    // Configuration Driver don't attach ConfigAccess protocol to its HII package
    // list, then there will be no configuration action required
    //
    FormSet->ConfigAccess = NULL;
  }

  //
  // Parse the IFR binary OpCodes
  //
  Status = ParseOpCodes (FormSet);
  DMSG ((EFI_D_ERROR, "InitializeFormSet, ParseOpCodes :%r.\n", Status));

  return Status;
}


/**
  Save globals used by previous call to SendForm(). SendForm() may be called from
  HiiConfigAccess.Callback(), this will cause SendForm() be retried.
  So, save globals of previous call to SendForm() and restore them upon exit.

**/
VOID
SaveBrowserContext (
  VOID
  )
{
  BROWSER_CONTEXT      *Context;
  FORM_ENTRY_INFO      *MenuList;
  FORM_BROWSER_FORMSET *FormSet;

  gBrowserContextCount++;
  if (gBrowserContextCount == 1) {
    //
    // This is not reentry of SendForm(), no context to save
    //
    return;
  }
  DMSG ((EFI_D_ERROR, "\n SaveBrowserContext, \n"));

  Context = AllocatePool (sizeof (BROWSER_CONTEXT));
  ASSERT (Context != NULL);

  Context->Signature = BROWSER_CONTEXT_SIGNATURE;

  //
  // Save FormBrowser context
  //
  Context->Selection            = gCurrentSelection;
  Context->ResetRequired        = gResetRequired;
  Context->FlagReconnect        = gFlagReconnect;
  Context->CallbackReconnect    = gCallbackReconnect;
  Context->ExitRequired         = gExitRequired;
  Context->HiiHandle            = mCurrentHiiHandle;
  Context->FormId               = mCurrentFormId;
  CopyGuid (&Context->FormSetGuid, &mCurrentFormSetGuid);
  Context->SystemLevelFormSet   = mSystemLevelFormSet;
  Context->CurFakeQestId        = mCurFakeQestId;
  Context->HiiPackageListUpdated = mHiiPackageListUpdated;
  Context->FinishRetrieveCall   = mFinishRetrieveCall;

  //
  // Save the menu history data.
  //
  InitializeListHead(&Context->FormHistoryList);
  while (!IsListEmpty (&mPrivateData.FormBrowserEx2.FormViewHistoryHead)) {
    MenuList = FORM_ENTRY_INFO_FROM_LINK (mPrivateData.FormBrowserEx2.FormViewHistoryHead.ForwardLink);
    RemoveEntryList (&MenuList->Link);

    InsertTailList(&Context->FormHistoryList, &MenuList->Link);
  }

  //
  // Save formset list.
  //
  InitializeListHead(&Context->FormSetList);
  while (!IsListEmpty (&gBrowserFormSetList)) {
    FormSet = FORM_BROWSER_FORMSET_FROM_LINK (gBrowserFormSetList.ForwardLink);
    RemoveEntryList (&FormSet->Link);

    InsertTailList(&Context->FormSetList, &FormSet->Link);
  }

  //
  // Insert to FormBrowser context list
  //
  InsertHeadList (&gBrowserContextList, &Context->Link);
}


/**
  Restore globals used by previous call to SendForm().

**/
VOID
RestoreBrowserContext (
  VOID
  )
{
  LIST_ENTRY       *Link;
  BROWSER_CONTEXT  *Context;
  FORM_ENTRY_INFO      *MenuList;
  FORM_BROWSER_FORMSET *FormSet;

  DMSG ((EFI_D_ERROR, "\n RestoreBrowserContext, \n"));
  ASSERT (gBrowserContextCount != 0);
  gBrowserContextCount--;
  if (gBrowserContextCount == 0) {
    //
    // This is not reentry of SendForm(), no context to restore
    //
    return;
  }
  DMSG ((EFI_D_ERROR, "\n RestoreBrowserContext, \n"));
  ASSERT (!IsListEmpty (&gBrowserContextList));

  Link = GetFirstNode (&gBrowserContextList);
  Context = BROWSER_CONTEXT_FROM_LINK (Link);

  //
  // Restore FormBrowser context
  //
  gCurrentSelection     = Context->Selection;
  gResetRequired        = Context->ResetRequired;
  gFlagReconnect        = Context->FlagReconnect;
  gCallbackReconnect    = Context->CallbackReconnect;
  gExitRequired         = Context->ExitRequired;
  mCurrentHiiHandle     = Context->HiiHandle;
  mCurrentFormId        = Context->FormId;
  CopyGuid (&mCurrentFormSetGuid, &Context->FormSetGuid);
  mSystemLevelFormSet   = Context->SystemLevelFormSet;
  mCurFakeQestId        = Context->CurFakeQestId;
  mHiiPackageListUpdated = Context->HiiPackageListUpdated;
  mFinishRetrieveCall   = Context->FinishRetrieveCall;

  //
  // Restore the menu history data.
  //
  while (!IsListEmpty (&Context->FormHistoryList)) {
    MenuList = FORM_ENTRY_INFO_FROM_LINK (Context->FormHistoryList.ForwardLink);
    RemoveEntryList (&MenuList->Link);

    InsertTailList(&mPrivateData.FormBrowserEx2.FormViewHistoryHead, &MenuList->Link);
  }

  //
  // Restore the Formset data.
  //
  while (!IsListEmpty (&Context->FormSetList)) {
    FormSet = FORM_BROWSER_FORMSET_FROM_LINK (Context->FormSetList.ForwardLink);
    RemoveEntryList (&FormSet->Link);

    InsertTailList(&gBrowserFormSetList, &FormSet->Link);
  }

  //
  // Remove from FormBrowser context list
  //
  RemoveEntryList (&Context->Link);
  gBS->FreePool (Context);
}

/**
  Find the matched FormSet context in the backup maintain list based on HiiHandle.

  @param Handle  The Hii Handle.

  @return the found FormSet context. If no found, NULL will return.

**/
FORM_BROWSER_FORMSET *
GetFormSetFromHiiHandle (
  EFI_HII_HANDLE Handle
  )
{
  LIST_ENTRY           *Link;
  FORM_BROWSER_FORMSET *FormSet;

  Link = GetFirstNode (&gBrowserFormSetList);
  while (!IsNull (&gBrowserFormSetList, Link)) {
    FormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&gBrowserFormSetList, Link);
    if (!ValidateFormSet(FormSet)) {
      continue;
    }
    if (FormSet->HiiHandle == Handle) {
      //DMSG ((EFI_D_ERROR, "GetFormSetFromHiiHandle, Get Handle :0x%x. \n", FormSet->HiiHandle));
      return FormSet;
    }
  }

  return NULL;
}

/**
  Check whether the input HII handle is the FormSet that is being used.

  @param Handle  The Hii Handle.

  @retval TRUE   HII handle is being used.
  @retval FALSE  HII handle is not being used.

**/
BOOLEAN
IsHiiHandleInBrowserContext (
  EFI_HII_HANDLE Handle
  )
{
  LIST_ENTRY       *Link;
  BROWSER_CONTEXT  *Context;

  //
  // HiiHandle is Current FormSet.
  //
  if (mCurrentHiiHandle == Handle) {
    DMSG ((EFI_D_ERROR, "IsHiiHandleInBrowserContext, Handle is Current FormSet. \n"));
    return TRUE;
  }

  //
  // Check whether HiiHandle is in BrowserContext.
  //
  Link = GetFirstNode (&gBrowserContextList);
  while (!IsNull (&gBrowserContextList, Link)) {
    Context = BROWSER_CONTEXT_FROM_LINK (Link);
    if (Context->HiiHandle == Handle) {
      //
      // HiiHandle is in BrowserContext
      //
      DMSG ((EFI_D_ERROR, "IsHiiHandleInBrowserContext, Handle is In BrowserContext. \n"));
      return TRUE;
    }
    Link = GetNextNode (&gBrowserContextList, Link);
  }

  return FALSE;
}

/**
  Perform Password check.
  Password may be encrypted by driver that requires the specific check.

  @param  Form             Form where Password Statement is in.
  @param  Statement        Password statement
  @param  PasswordString   Password string to be checked. It may be NULL.
                           NULL means to restore password.
                           "" string can be used to checked whether old password does exist.

  @return Status     Status of Password check.
**/
EFI_STATUS
EFIAPI
PasswordCheck (
  IN FORM_DISPLAY_ENGINE_FORM      *Form,
  IN FORM_DISPLAY_ENGINE_STATEMENT *Statement,
  IN EFI_STRING                    PasswordString  OPTIONAL
  )
{
  EFI_STATUS                      Status;
  EFI_HII_CONFIG_ACCESS_PROTOCOL  *ConfigAccess;
  EFI_BROWSER_ACTION_REQUEST      ActionRequest;
  EFI_IFR_TYPE_VALUE              IfrTypeValue;
  FORM_BROWSER_STATEMENT          *Question;

  ConfigAccess = gCurrentSelection->FormSet->ConfigAccess;
  Question = GetBrowserStatement(Statement);
  ASSERT (Question != NULL);

  if ((Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) == EFI_IFR_FLAG_CALLBACK) {
    if (ConfigAccess == NULL) {
      return EFI_UNSUPPORTED;
    }
  } else {
    //
    // If a password doesn't have the CALLBACK flag, browser will not handle it.
    //
    return EFI_UNSUPPORTED;
  }

  DMSG ((EFI_D_ERROR, "\n PasswordCheck, PasswordString :%s.\n", PasswordString));	  
  //
  // Prepare password string in HII database
  //
  if (PasswordString != NULL) {
    IfrTypeValue.string = NewString (PasswordString, gCurrentSelection->FormSet->HiiHandle);
  } else {
    IfrTypeValue.string = 0;
  }

  //
  // Send password to Configuration Driver for validation
  //
  ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;
  Status = ConfigAccess->Callback (
                           ConfigAccess,
                           EFI_BROWSER_ACTION_CHANGING,
                           Question->QuestionId,
                           Question->HiiValue.Type,
                           &IfrTypeValue,
                           &ActionRequest
                           );

    DMSG ((EFI_D_ERROR, "PasswordCheck, ActionRequest :%d. \n", ActionRequest));  	

  //
  // Remove password string from HII database
  //
  if (PasswordString != NULL) {
    DeleteString (IfrTypeValue.string, gCurrentSelection->FormSet->HiiHandle);
  }

  if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
    gResetRequired = TRUE;
    DMSG ((EFI_D_ERROR, "PasswordCheck, Set Reset Flag. \n"));  	
  }
  return Status;
}

/**
  Find the registered HotKey based on KeyData.

  @param[in] KeyData     A pointer to a buffer that describes the keystroke
                         information for the hot key.

  @return The registered HotKey context. If no found, NULL will return.
**/
BROWSER_HOT_KEY *
GetHotKeyFromRegisterList (
  IN EFI_KEY_DATA KeyData
  )
{
  LIST_ENTRY       *Link;
  BROWSER_HOT_KEY  *HotKey;

  DMSG ((EFI_D_ERROR, "\n GetHotKeyFromRegisterList, SetupBrowser,\n"));
  Link = GetFirstNode (&gBrowserHotKeyList);
  while (!IsNull (&gBrowserHotKeyList, Link)) {
    HotKey = BROWSER_HOT_KEY_FROM_LINK (Link);

    if (HotKey->KeyData.Key.ScanCode == KeyData.Key.ScanCode &&
        HotKey->KeyData.Key.UnicodeChar == KeyData.Key.UnicodeChar) {

      if (HotKey->KeyData.KeyState.KeyShiftState == KeyData.KeyState.KeyShiftState) {
        DMSG ((EFI_D_ERROR, "GetHotKeyFromRegisterList, Find Node Success\n"));
        return HotKey;
      }
    }

    Link = GetNextNode (&gBrowserHotKeyList, Link);
  }

  return NULL;
}

/**
  Configure what scope the hot key will impact.
  All hot keys have the same scope. The mixed hot keys with the different level are not supported.
  If no scope is set, the default scope will be FormSet level.
  After all registered hot keys are removed, previous Scope can reset to another level.

  @param[in] Scope               Scope level to be set.

  @retval EFI_SUCCESS            Scope is set correctly.
  @retval EFI_INVALID_PARAMETER  Scope is not the valid value specified in BROWSER_SETTING_SCOPE.
  @retval EFI_UNSUPPORTED         Scope level is different from current one that the registered hot keys have.

**/
EFI_STATUS
EFIAPI
SetScope (
  IN BROWSER_SETTING_SCOPE Scope
  )
{
  if (Scope >= MaxLevel) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // When no hot key registered in system or on the first setting,
  // Scope can be set.
  //
  if (mBrowserScopeFirstSet || IsListEmpty (&gBrowserHotKeyList)) {
    gBrowserSettingScope  = Scope;
    mBrowserScopeFirstSet = FALSE;
  } else if (Scope != gBrowserSettingScope) {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Register the hot key with its browser action, or unregistered the hot key.
  If the action value is zero, the hot key will be unregistered if it has been registered.

  @param[in] KeyData     A pointer to a buffer that describes the keystroke
                         information for the hot key. Its type is EFI_KEY_DATA to
                         be supported by all ConsoleIn devices.
  @param[in] Action      Action value that describes what action will be triggered when the hot key is pressed.
  @param[in] DefaultId   Specifies the type of defaults to retrieve, which is only for DEFAULT action.
  @param[in] Fun         CallBack Function
  @param[in] Context     Context of CallBack Function
  @param[in] HelpString  Help string that describes the hot key information.
                         Its value may be NULL for the unregistered hot key.

  @retval EFI_SUCCESS            Hot key is registered or unregistered.
  @retval EFI_INVALID_PARAMETER  KeyData is NULL.
**/
EFI_STATUS
EFIAPI
RegisterHotkeyFunction(
  IN EFI_KEY_DATA                    KeyData,
  IN UINT32                          Action,
  IN UINT16                          DefaultId,
  IN BROWSER_HOT_KEY_CALL_BACK       Fun,
  IN VOID                            *Context   OPTIONAL,
  IN EFI_STRING                      HelpString OPTIONAL
  )
{
  BROWSER_HOT_KEY      *HotKey;

  DMSG ((EFI_D_ERROR, "\n RegisterHotkeyFunction, Action :0x%x, DefaultId :0x%x, ScanCode :0x%x UnicodeChar :0x%x KeyShiftState :0x%x KeyToggleState :0x%x\n", 
        Action, 
        DefaultId,
        KeyData.Key.ScanCode,
        KeyData.Key.UnicodeChar,
        KeyData.KeyState.KeyShiftState,
        KeyData.KeyState.KeyToggleState
        ));
  //
  // Check whether the input KeyData is in BrowserHotKeyList.
  //
  HotKey = GetHotKeyFromRegisterList (KeyData);

  //
  // Unregister HotKey
  //
  if (Action == BROWSER_ACTION_UNREGISTER) {
    if (HotKey != NULL) {
      //
      // The registered HotKey is found.
      // Remove it from List, and free its resource.
      //
      RemoveEntryList (&HotKey->Link);
      if (HotKey->HelpString != NULL) {
        FreePool (HotKey->HelpString);
      }
      FreePool (HotKey);
      DMSG ((EFI_D_ERROR, "\n UnRegisterHotkeyFunction Success."));
      return EFI_SUCCESS;
    } else {
      //
      // The registered HotKey is not found.
      //
      return EFI_NOT_FOUND;
    }
  }

  if (HotKey != NULL) {
    return EFI_ALREADY_STARTED;
  }

  //
  // Create new Key, and add it into List.
  //
  HotKey = AllocateZeroPool (sizeof (BROWSER_HOT_KEY));
  ASSERT (HotKey != NULL);
  HotKey->Signature  = BROWSER_HOT_KEY_SIGNATURE;
  HotKey->Action     = Action;
  HotKey->CallBack   = Fun;
  HotKey->Context    = Context;
  HotKey->DefaultId  = DefaultId;
  HotKey->HelpString = NULL;
  CopyMem(&HotKey->KeyData, &KeyData, sizeof(EFI_KEY_DATA));
  if (HelpString != NULL) {
    HotKey->HelpString = AllocateCopyPool (StrSize (HelpString), HelpString);
  }

  DMSG ((EFI_D_ERROR, "\n RegisterHotkeyFunction Success."));
  InsertTailList (&gBrowserHotKeyList, &HotKey->Link);
  return EFI_SUCCESS;
}

/**
  Register the hot key with its browser action, or unregistered the hot key.
  Only support hot key that is not printable character (control key, function key, etc.).
  If the action value is zero, the hot key will be unregistered if it has been registered.
  If the same hot key has been registered, the new action and help string will override the previous ones.

  @param[in] KeyData     A pointer to a buffer that describes the keystroke
                         information for the hot key. Its type is EFI_INPUT_KEY to
                         be supported by all ConsoleIn devices.
  @param[in] Action      Action value that describes what action will be triggered when the hot key is pressed.
  @param[in] DefaultId   Specifies the type of defaults to retrieve, which is only for DEFAULT action.
  @param[in] HelpString  Help string that describes the hot key information.
                         Its value may be NULL for the unregistered hot key.

  @retval EFI_SUCCESS            Hot key is registered or unregistered.
  @retval EFI_INVALID_PARAMETER  KeyData is NULL or HelpString is NULL on register.
  @retval EFI_NOT_FOUND          KeyData is not found to be unregistered.
  @retval EFI_UNSUPPORTED        Key represents a printable character. It is conflicted with Browser.
  @retval EFI_ALREADY_STARTED    Key already been registered for one hot key.
**/
EFI_STATUS
EFIAPI
RegisterHotKey ( 
  IN EFI_INPUT_KEY *KeyData,
  IN UINT32        Action,
  IN UINT16        DefaultId,
  IN EFI_STRING    HelpString OPTIONAL
  )
{
  EFI_KEY_DATA Key;
  Key.KeyState.KeyShiftState  = 0x00;
  Key.KeyState.KeyToggleState = 0x00;
  CopyMem(&Key.Key, KeyData, sizeof(EFI_INPUT_KEY));

  DMSG ((EFI_D_ERROR, "\n RegisterHotKey..."));
  return RegisterHotkeyFunction(Key, Action, DefaultId, NULL, NULL, HelpString);
}

/**
  Register Exit handler function.
  When more than one handler function is registered, the latter one will override the previous one.
  When NULL handler is specified, the previous Exit handler will be unregistered.

  @param[in] Handler      Pointer to handler function.

**/
VOID
EFIAPI
RegiserExitHandler (
  IN EXIT_HANDLER Handler
  )
{
  ExitHandlerFunction = Handler;
  return;
}

/**
  Check whether the browser data has been modified.

  @retval TRUE        Browser data is modified.
  @retval FALSE       No browser data is modified.

**/
BOOLEAN
EFIAPI
IsBrowserDataModified (
  VOID
  )
{
  LIST_ENTRY              *Link;
  FORM_BROWSER_FORMSET    *FormSet;

  switch (gBrowserSettingScope) {
    case FormLevel:
      if (gCurrentSelection == NULL) {
        return FALSE;
      }
      return IsNvUpdateRequiredForForm (gCurrentSelection->Form);

    case FormSetLevel:
      if (gCurrentSelection == NULL) {
        return FALSE;
      }
      return IsNvUpdateRequiredForFormSet (gCurrentSelection->FormSet);

    case SystemLevel:
      Link = GetFirstNode (&gBrowserFormSetList);
      while (!IsNull (&gBrowserFormSetList, Link)) {
        FormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
        Link = GetNextNode (&gBrowserFormSetList, Link);
        if (!ValidateFormSet(FormSet)) {
          continue;
        }

        if (IsNvUpdateRequiredForFormSet (FormSet)) {
          return TRUE;
        }
      }
      return FALSE;

    default:
      return FALSE;
  }
}

/**
  Execute the action requested by the Action parameter.

  @param[in] Action     Execute the request action.
  @param[in] DefaultId  The default Id info when need to load default value. Only used when Action is BROWSER_ACTION_DEFAULT.

  @retval EFI_SUCCESS              Execute the request action success.
  @retval EFI_INVALID_PARAMETER    The input action value is invalid.

**/
EFI_STATUS
EFIAPI
ExecuteAction (
  IN UINT32        Action,
  IN UINT16        DefaultId
  )
{
  EFI_STATUS              Status;
  FORM_BROWSER_FORMSET    *FormSet;
  FORM_BROWSER_FORM       *Form;
  CHAR16   *CfmStr;
  BOOLEAN  ResetFlagRestore;
  CHAR16                   *ResetString = NULL;

  if (gBrowserSettingScope < SystemLevel && gCurrentSelection == NULL) {
    return EFI_NOT_READY;
  }

  Status  = EFI_SUCCESS;
  FormSet = NULL;
  Form    = NULL;
  ResetFlagRestore = gResetRequired;
  if (gBrowserSettingScope < SystemLevel) {
    FormSet = gCurrentSelection->FormSet;
    Form    = gCurrentSelection->Form;
  }
  DMSG ((EFI_D_ERROR, "ExecuteAction(), Action :0x%x. \n", Action));

  //
  // Normal EXIT means EXIT and RESET
  //
  if ((Action & BROWSER_ACTION_EXIT) == BROWSER_ACTION_EXIT) {
    Action = Action | BROWSER_ACTION_RESET;
  }
  
  //
  // EXTI without RESET means EXIT only
  //
  if ((Action & BROWSER_ACTION_EXIT_NO_RESET) == BROWSER_ACTION_EXIT_NO_RESET) {
    Action = Action | BROWSER_ACTION_EXIT;
  }

  //
  // Set Reset Flag.
  //
  if ((Action & BROWSER_ACTION_RESET) != 0) {
    gResetRequired = TRUE;
    gExitRequired = TRUE;
    DMSG ((EFI_D_ERROR, "ExecuteAction, Set Reset Flag. \n"));
  }
  
  //
  // Set Exit Flag.
  //
  if ((Action & BROWSER_ACTION_EXIT) != 0) {
    gExitRequired = TRUE;
    DMSG ((EFI_D_ERROR, "ExecuteAction, Set Exit Flag. \n"));
  }

  //
  // Discard action.
  //
  if ((Action & BROWSER_ACTION_DISCARD) != 0) { 
    if ((Action & BROWSER_ACTION_RESET) != 0) {
      ResetString =  HiiGetString (gPrivateData.HiiHandle, STRING_TOKEN (STR_DISCARD_EXIT_AND_RESET), NULL);
      CfmStr = ResetString;
    } else if(Action & BROWSER_ACTION_EXIT) {
      CfmStr = mCustomString->StrDiscardExit;
    } else {
      CfmStr = mCustomString->StrDiscard;
    }
	
    if (SELECTION_YES == UiConfirmDialog(DIALOG_BROWSER_YESNO, mCustomString->StrSetupConfirmation, NULL, TEXT_ALIGIN_CENTER, gEmptyString, CfmStr, NULL)) {
      if (ResetString != NULL) {
        FreePool (ResetString);
      }
      Status = DiscardForm (FormSet, Form, gBrowserSettingScope);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    } else {
      gExitRequired = FALSE;
      gResetRequired = ResetFlagRestore;
      Status = EFI_NOT_READY;
      return Status;
    }
  }
  //
  // Default action.
  //
  if ((Action & BROWSER_ACTION_DEFAULT) != 0) {
    if (SELECTION_YES == UiConfirmDialog(DIALOG_BROWSER_YESNO, mCustomString->StrSetupDefaultConfirmation, NULL, TEXT_ALIGIN_CENTER, gEmptyString, mCustomString->StrLoadDefault, NULL)) {

      Status = ExtractDefault (FormSet, Form, DefaultId, gBrowserSettingScope, GetDefaultForAll, NULL, FALSE, FALSE);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      UpdateStatementStatus (FormSet, Form, gBrowserSettingScope);
    } else {
      Status = EFI_NOT_READY;
      return Status;
    }
  }

  //
  // Submit action.
  //
  if ((Action & BROWSER_ACTION_SUBMIT) != 0) {
    if ((Action & BROWSER_ACTION_RESET ) != 0) {
      ResetString =  HiiGetString (gPrivateData.HiiHandle, STRING_TOKEN (STR_SAVE_EXIT_AND_RESET), NULL);
      CfmStr = ResetString;
    } else if(Action & BROWSER_ACTION_EXIT) {
      CfmStr = mCustomString->StrSavingExit;
    } else {
      CfmStr = mCustomString->StrSaving;
    }
    if (SELECTION_YES == UiConfirmDialog(DIALOG_BROWSER_YESNO, mCustomString->StrSetupConfirmation, NULL, TEXT_ALIGIN_CENTER, gEmptyString, CfmStr, NULL)) {
      if (ResetString != NULL) {
        FreePool (ResetString);
      }
      if ((Action & BROWSER_ACTION_EXIT) != 0) {
        gST->ConOut->SetAttribute (gST->ConOut,EFI_BLACK | EFI_BACKGROUND_BLACK);  //Set the screen black 
        gST->ConOut->ClearScreen(gST->ConOut);
      }
      Status = SubmitForm (FormSet, Form, gBrowserSettingScope);
      if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_ERROR, "%a (L%d) SubmitForm %r\n", __FUNCTION__, __LINE__, Status));
        return Status;
      }
    } else {
      gExitRequired = FALSE;
      gResetRequired = ResetFlagRestore;
      Status = EFI_NOT_READY;
      return Status;
    }
  }

  if ((Action & BROWSER_ACTION_SAVE_USER_DEFAULT) != 0) {
    if (SELECTION_YES == UiConfirmDialog(DIALOG_BROWSER_YESNO, mCustomString->StrSetupDefaultConfirmation, NULL, TEXT_ALIGIN_CENTER, gEmptyString, mCustomString->StrSaveUserDefault, NULL)) {
      TriggerSetupNotify(SetupSaveNotifyTypeSaveUserDefault);
      Status = EFI_SUCCESS;
    } else {
      Status = EFI_NOT_READY;
    }
    return Status;
  }

  if ((Action & BROWSER_ACTION_LOAD_USER_DEFAULT) != 0) {
    if (SELECTION_YES == UiConfirmDialog(DIALOG_BROWSER_YESNO, mCustomString->StrSetupDefaultConfirmation, NULL, TEXT_ALIGIN_CENTER, gEmptyString, mCustomString->StrLoadUserDefault, NULL)) {
      LoadAllHiiFormset ();
      TriggerSetupNotify(SetupSaveNotifyTypeLoadUserDefault);
      Status = EFI_SUCCESS;
    } else {
      Status = EFI_NOT_READY;
    }
    return Status;
  }

  //
  // Execute the exit Hook Function.
  //
  if ((Action & BROWSER_ACTION_EXIT) != 0) {
    DiscardForm (FormSet, Form, gBrowserSettingScope);
    if (gBrowserSettingScope == SystemLevel) {
      if (ExitHandlerFunction != NULL) {
        ExitHandlerFunction ();
      }
    }
  }

  return Status;
}

/**
  Create reminder to let user to choose save or discard the changed browser data.
  Caller can use it to actively check the changed browser data.

  @retval BROWSER_NO_CHANGES       No browser data is changed.
  @retval BROWSER_SAVE_CHANGES     The changed browser data is saved.
  @retval BROWSER_DISCARD_CHANGES  The changed browser data is discard.
  @retval BROWSER_KEEP_CURRENT     Browser keep current changes.

**/
UINT32
EFIAPI
SaveReminder (
  VOID
  )
{
  LIST_ENTRY              *Link;
  FORM_BROWSER_FORMSET    *FormSet;
  BOOLEAN                 IsDataChanged;
  UINT32                  DataSavedAction;
  UINT32                  ConfirmRet;

  DMSG ((EFI_D_ERROR, "\n SaveReminder, \n"));
  
  DataSavedAction  = BROWSER_NO_CHANGES;
  IsDataChanged    = FALSE;
  Link = GetFirstNode (&gBrowserFormSetList);
  while (!IsNull (&gBrowserFormSetList, Link)) {
    FormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&gBrowserFormSetList, Link);
    if (!ValidateFormSet(FormSet)) {
      continue;
    }
    if (IsNvUpdateRequiredForFormSet (FormSet)) {
      IsDataChanged = TRUE;
      break;
    }
  }

  //
  // No data is changed. No save is required.
  //
  if (!IsDataChanged) {
    return DataSavedAction;
  }

  //
  // If data is changed, prompt user to save or discard it.
  //
  do {
    ConfirmRet = (UINT32) mFormDisplay->ConfirmDataChange(NULL);

    if (ConfirmRet == BROWSER_ACTION_SUBMIT) {
      SubmitForm (NULL, NULL, SystemLevel);
      DataSavedAction = BROWSER_SAVE_CHANGES;
      break;
    } else if (ConfirmRet == BROWSER_ACTION_DISCARD) {
      DiscardForm (NULL, NULL, SystemLevel);
      DataSavedAction = BROWSER_DISCARD_CHANGES;
      break;
    } else if (ConfirmRet == BROWSER_ACTION_NONE) {
      DataSavedAction = BROWSER_KEEP_CURRENT;
      break;
    }
  } while (1);

  return DataSavedAction;
}

/**
  Check whether the Reset Required for the browser

  @retval TRUE      Browser required to reset after exit.
  @retval FALSE     Browser not need to reset after exit.

**/
BOOLEAN
EFIAPI
IsResetRequired (
  VOID
  )
{
  return gResetRequired;
}

/**
  Trigger notify protocol function.

  @param  SETUP_SAVE_NOTIFY_TYPE Notify type.

  @retval EFI_STATUS return status from the notify function.

**/
EFI_STATUS
EFIAPI
TriggerSetupNotify ( SETUP_SAVE_NOTIFY_TYPE Type )
{
  EFI_STATUS    Status;
  EFI_HANDLE    *HandleBuffer;
  EFI_HANDLE    Handle;
  UINTN    HandleCount;
  UINTN    Index;
  SETUP_SAVE_NOTIFY_PROTOCOL    *ptSetupSaveNotify;

  
  DEBUG((EFI_D_INFO, "%a(%d)\n", __FUNCTION__, Type));

  HandleBuffer = NULL;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gSetupSaveNotifyProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  DEBUG((EFI_D_INFO, "Handles:%d\n", HandleCount));

  for(Index=0; Index<HandleCount; Index++){
    Handle = HandleBuffer[Index];
    Status = gBS->HandleProtocol(
                    Handle,
                    &gSetupSaveNotifyProtocolGuid,
                    ( void**)&ptSetupSaveNotify);
    ASSERT(!EFI_ERROR(Status));

    DEBUG((EFI_D_INFO, "SetupSaveNotify:%X, Handle:%X\n", ptSetupSaveNotify, Handle));

    switch(Type){
      case SetupSaveNotifyTypeSaveValue:
        if (NULL != ptSetupSaveNotify->SaveValue) { 	
          Status = ptSetupSaveNotify->SaveValue(ptSetupSaveNotify);
        }
        break;

      case SetupSaveNotifyTypeDiscardVaule:
        if (NULL != ptSetupSaveNotify->DiscardValue) { 		  	
          Status = ptSetupSaveNotify->DiscardValue(ptSetupSaveNotify);
        }
        break;
      case SetupSaveNotifyTypeLoadDefault:
        if (NULL != ptSetupSaveNotify->LoadDefault) { 		  	
          Status = ptSetupSaveNotify->LoadDefault(ptSetupSaveNotify);
        }
        break;
      case SetupSaveNotifyTypeSaveUserDefault:
        if (NULL != ptSetupSaveNotify->SaveUserDefault) { 		  	
          Status = ptSetupSaveNotify->SaveUserDefault(ptSetupSaveNotify);
        }
        break;
      case SetupSaveNotifyTypeLoadUserDefault:
        if (NULL != ptSetupSaveNotify->LoadUserDefault) { 		  	
          Status = ptSetupSaveNotify->LoadUserDefault(ptSetupSaveNotify);
        }
        break;

      default:
        break;
    }
  }

ProcExit:
  if(HandleBuffer!=NULL){
    FreePool(HandleBuffer);
  }

  DEBUG((EFI_D_INFO, "%a Exit\n", __FUNCTION__));  
  return Status;
}


/**
  This is only fake function that will be made in platform setup.
**/
VOID
EFIAPI
FakeOemPlatformReset (VOID)
{
  DMSG((EFI_D_ERROR, "FakeOemPlatformReset(), This is a EfiResetCold, please make new function in platform setup.\n"));
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  return;
}

#ifndef ENABLE_RETERIEVE_SETTING
/**
  Retrieve setup option value and attribute from Browser.

  @param  InKeyValueArray    Input setting to be applied.
  @param  OutKeyValueArray   Current setting from all installed HII packages.
  @param  OutAttributesArray Current setting attribute from all installed HII packages.

  @retval EFI_SUCCESS        Retrieve setup value and attribute from Browser
  @retval EFI_UNSUPPORTED    Don't support this service.

**/
EFI_STATUS
EFIAPI
ReterieveSetupOptions (
  IN  VOID *InKeyValueArray,
  OUT VOID **OutKeyValueArray,
  OUT VOID **OutAttributesArray
  )
{
  return EFI_UNSUPPORTED;
}

/**
  Retrieve setup option value and storage name from Browser.

  @param  InKeyValueArray     Input setting to be applied.
  @param  OutKeyValueArray    Current setting from all installed HII packages.
  @param  OutStorageNameArray Current setting storage name from all installed HII packages.

  @retval EFI_SUCCESS        Retrieve setup value and attribute from Browser
  @retval EFI_UNSUPPORTED    Don't support this service.

**/
EFI_STATUS
EFIAPI
ReterieveSetupOptionsName (
  IN  VOID *InKeyValueArray,
  OUT VOID **OutKeyValueArray,
  OUT VOID **OutStorageNameArray
  )
{
  return EFI_UNSUPPORTED;
}
#endif
