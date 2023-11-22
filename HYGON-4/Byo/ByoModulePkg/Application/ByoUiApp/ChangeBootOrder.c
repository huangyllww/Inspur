/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/
#include "ByoUi.h"

EFI_GUID *mIgnoreFormsetGuidList[] = {
  &gEfiFormsetGuidMain,
  &gEfiFormsetGuidAdvance,
  &gEfiFormsetGuidDevices,
  &gEfiFormsetGuidBoot,
  &gEfiFormsetGuidSecurity,
  &gEfiFormsetGuidPower,
  &gEfiFormsetGuidExit
};

EFI_GUID *mFormSetClassGuidList[] = {
  &gEfiIfrByoBootUiPageGuid,
  &gIfrByoSecurityUiPageGuid,
  &gIfrByoDevicesUiPageGuid
};

BOOLEAN
IsMatchClassGuid (
  EFI_GUID  *ClassGuid,
  UINT8     ClassGuidNum,
  EFI_GUID  *TargetGuid
)
{
  UINT8 Index;
  UINT8 Index2;

  //
  // if the page is hidden, skip it
  //
  for (Index = 0; Index < ClassGuidNum; Index ++) {
    if (CompareGuid (&gIfrByoHideUiPageGuid, ClassGuid+Index)) {
      return FALSE;
    }
  }

  //
  // if the target guid is not Setup class guid, 
  // search it in the class guid.
  //
  if (!CompareGuid (TargetGuid, &gEfiHiiPlatformSetupFormsetGuid)) {
    for (Index = 0; Index < ClassGuidNum; Index ++) {
      if (CompareGuid (TargetGuid, ClassGuid+Index)) {
        return TRUE;
      }
    }

    return FALSE;
  }

  //
  // if the target guid is Setup class guid, 
  // make sure it not have other class guid
  //
  for (Index = 0; Index < ClassGuidNum; Index ++) {
    for (Index2 = 0; Index2 < ARRAY_SIZE (mFormSetClassGuidList); Index2 ++) {
      if (CompareGuid (mFormSetClassGuidList[Index2], ClassGuid+Index)) {
        return FALSE;
      }
    }
  }

  //
  // Check whether it has Setup class guid
  //
  for (Index = 0; Index < ClassGuidNum; Index ++) {
    if (CompareGuid (TargetGuid, ClassGuid+Index)) {
      return TRUE;
    }
  }

  return FALSE;
}

BOOLEAN
IsIgnoreFormSetGuid (
  UINT8 *FormSetGuid
)
{
  UINTN Index;

  for (Index = 0; Index < ARRAY_SIZE (mIgnoreFormsetGuidList); Index ++) {
    if (CompareMem (FormSetGuid, mIgnoreFormsetGuidList[Index], sizeof (EFI_GUID)) == 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
  Check whether this HII package formset guid is same to input one.

  @retval   TRUE    The input HiiHandle matches the input FormsetGuid.
  @retval   FALSE   The input HiiHandle doesn't match the input FormsetGuid.

**/
BOOLEAN
MatchHiiHandleAndFormsetGuid (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  EFI_GUID                    *FormsetGuid
  )
{
  EFI_STATUS                  Status;
  EFI_IFR_FORM_SET            *Buffer;
  UINTN                       BufferSize;
  UINT8                       *Ptr;
  UINTN                       TempSize;
  BOOLEAN                     RetVal;

  Status = HiiGetFormSetFromHiiHandle(HiiHandle, &Buffer,&BufferSize);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  RetVal = FALSE;
  TempSize = 0;
  Ptr = (UINT8 *) Buffer;
  while(TempSize < BufferSize)  {
    TempSize += ((EFI_IFR_OP_HEADER *) Ptr)->Length;

    if (((EFI_IFR_OP_HEADER *) Ptr)->Length <= OFFSET_OF (EFI_IFR_FORM_SET, Flags)){
      Ptr += ((EFI_IFR_OP_HEADER *) Ptr)->Length;
      continue;
    }

    //
    // Check formset opcode
    //
    if (((EFI_IFR_OP_HEADER *) Ptr)->OpCode != EFI_IFR_FORM_SET_OP) {
      Ptr += ((EFI_IFR_OP_HEADER *) Ptr)->Length;
      continue;
    }

    RetVal = (CompareMem ((UINT8 *) &(((EFI_IFR_FORM_SET *)Ptr)->Guid), FormsetGuid, sizeof (EFI_GUID)) == 0);
    break;
  }

  FreePool (Buffer);

  return RetVal;
}

/**
  Check whether this driver need to be shown in the front page.

  @param    HiiHandle           The hii handle for the driver.
  @param    Guid                The special guid for the driver which is the target.
  @param    PromptId            Return the prompt string id.
  @param    HelpId              Return the help string id.
  @param    FormsetGuid         Return the formset guid info.

  @retval   EFI_SUCCESS         Search the driver success

**/
BOOLEAN
ExtractFormsetInfoByoClassGuid (
  IN  EFI_HII_HANDLE              HiiHandle,
  IN  EFI_GUID                    *Guid,
  OUT EFI_STRING_ID               *PromptId,
  OUT EFI_STRING_ID               *HelpId,
  OUT VOID                        *FormsetGuid, 
  IN OUT UINTN                    *FormSetOffset
  )
{
  EFI_STATUS                  Status;
  UINT8                       ClassGuidNum;
  EFI_GUID                    *ClassGuid;
  EFI_IFR_FORM_SET            *Buffer;
  UINTN                       BufferSize;
  UINT8                       *Ptr;
  UINTN                       TempSize;
  BOOLEAN                     RetVal;

  Status = HiiGetFormSetFromHiiHandle(HiiHandle, &Buffer,&BufferSize);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  RetVal = FALSE;
  TempSize = *FormSetOffset;
  Ptr = (UINT8 *) Buffer + TempSize;
  while(TempSize < BufferSize)  {
    TempSize += ((EFI_IFR_OP_HEADER *) Ptr)->Length;

    if (((EFI_IFR_OP_HEADER *) Ptr)->Length <= OFFSET_OF (EFI_IFR_FORM_SET, Flags)){
      Ptr += ((EFI_IFR_OP_HEADER *) Ptr)->Length;
      continue;
    }

    //
    // Check formset opcode
    //
    if (((EFI_IFR_OP_HEADER *) Ptr)->OpCode != EFI_IFR_FORM_SET_OP) {
      Ptr += ((EFI_IFR_OP_HEADER *) Ptr)->Length;
      continue;
    }

    //
    // Ignore Formset
    //
    if (IsIgnoreFormSetGuid ((UINT8 *) &(((EFI_IFR_FORM_SET *)Ptr)->Guid))) {
      Ptr += ((EFI_IFR_OP_HEADER *) Ptr)->Length;
      continue;
    }

    ClassGuidNum = (UINT8) (((EFI_IFR_FORM_SET *)Ptr)->Flags & 0x3);
    ClassGuid = (EFI_GUID *) (VOID *)(Ptr + sizeof (EFI_IFR_FORM_SET));
    if (IsMatchClassGuid (ClassGuid, ClassGuidNum, Guid)) {
      *PromptId = ((EFI_IFR_FORM_SET *)Ptr)->FormSetTitle;
      *HelpId = ((EFI_IFR_FORM_SET *)Ptr)->Help;
      CopyMem (FormsetGuid, &((EFI_IFR_FORM_SET *) Ptr)->Guid, sizeof (EFI_GUID));
      RetVal = TRUE;
    }
    break;
  }

  *FormSetOffset = TempSize;
  FreePool (Buffer);

  return RetVal;
}

/**
  Extract device path for given HII handle and class guid.

  @param Handle          The HII handle.

  @retval  NULL          Fail to get the device path string.
  @return  PathString    Get the device path string.

**/
CHAR16 *
ByoUiAppExtractDevicePathFromHiiHandle (
  IN      EFI_HII_HANDLE      Handle
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       DriverHandle;

  ASSERT (Handle != NULL);

  if (Handle == NULL) {
    return NULL;
  }

  Status = gHiiDatabase->GetPackageListHandle (gHiiDatabase, Handle, &DriverHandle);
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  //
  // Get device path string.
  //
  return ConvertDevicePathToText(DevicePathFromHandle (DriverHandle), FALSE, FALSE);
}

EFI_STATUS
AddDynamicFormset (
  EFI_HII_HANDLE    HiiHandle,
  EFI_GUID          *FormSetGuid,
  EFI_GUID          *ClassGuid,
  EFI_FORM_ID       FormId,
  UINT16            Label
  )
{
  EFI_STATUS    Status;
  UINTN    Index;
  EFI_HII_HANDLE    *HiiHandles;
  EFI_STRING_ID    FormSetTitle;
  EFI_STRING_ID    FormSetHelp;
  EFI_GUID    GotoFormSetGuid;
  VOID    *StartOpCodeHandle;
  VOID    *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL    *StartLabel;
  EFI_IFR_GUID_LABEL    *EndLabel;
  UINTN    Count;
  EFI_STRING_ID         Token;
  EFI_STRING_ID         TokenHelp;
  EFI_STRING String;
  CHAR16        *DevicePathStr;
  EFI_STRING_ID DevicePathId;
  CHAR8         *SupportedLanguages;
  CHAR8         *UpdatePageSupportedLanguages;
  CHAR8         *Supported;
  CHAR8         *Language;
  UINTN         FormsetOffset;

  HiiHandles = NULL;
  Status = EFI_NOT_FOUND;
  
  if (((HiiHandle == NULL) && (FormSetGuid == NULL)) || (ClassGuid == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Get all the Hii handles
  //
  HiiHandles = HiiGetHiiHandles (NULL);
  ASSERT (HiiHandles != NULL);
  if (HiiHandle == NULL) {
    //
    // Find HiiHandle to match the input FormSetGuid
    //
    for (Index = 0; HiiHandles[Index] != NULL; Index++) {
      if (MatchHiiHandleAndFormsetGuid (HiiHandles[Index], FormSetGuid)) {
        break;
      }
    }

    if (HiiHandles[Index] == NULL) {
      FreePool (HiiHandles);
      return EFI_NOT_FOUND;
    }

    HiiHandle = HiiHandles[Index];
  }

  UpdatePageSupportedLanguages = HiiGetSupportedLanguages (HiiHandle);

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
  //
  // According to the next show Form id(mNextShowFormId) to decide which form need to update.
  //
  StartLabel->Number       = Label;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_END;

  //
  // Search for formset of each class type
  //
  Count = 0;
  for (Index = 0; HiiHandles[Index] != NULL; Index++) {

    FormsetOffset = 0;
    while (ExtractFormsetInfoByoClassGuid (HiiHandles[Index], ClassGuid, &FormSetTitle, &FormSetHelp, &GotoFormSetGuid, &FormsetOffset)) {

      //
      // Get the device path for Hii handle
      //
      DevicePathStr = ByoUiAppExtractDevicePathFromHiiHandle(HiiHandles[Index]);
      DevicePathId  = 0;
      if (DevicePathStr != NULL){
        DevicePathId =  HiiSetString (HiiHandle, 0, DevicePathStr, NULL);
        FreePool(DevicePathStr);
      }

      //
      // Loop through each language that the string supports
      //
      Token = 0;
      TokenHelp = 0;
      SupportedLanguages = HiiGetSupportedLanguages (HiiHandle);

      for (Supported = SupportedLanguages; *Supported != '\0'; ) {
        //
        // Cache a pointer to the beginning of the current language in the list of languages
        //
        Language = Supported;

        //
        // Search for the next language separator and replace it with a Null-terminator
        //
        for (; *Supported != 0 && *Supported != ';'; Supported++);
        if (*Supported != 0) {
          *(Supported++) = '\0';
        }

        if ((IsLanguageSupported (UpdatePageSupportedLanguages, Language) != EFI_SUCCESS) || 
          AsciiStrnCmp (Language, UEFI_CONFIG_LANG, AsciiStrLen (UEFI_CONFIG_LANG)) == 0) {
          //
          // Skip unsupported languages
          // Skip string package used for keyword protocol.
          //
          continue;
        }

        //
        // Get/Set prompt and its help string
        //
        String = HiiGetString (HiiHandles[Index], FormSetTitle, Language);
        if (String == NULL) {
          String = HiiGetString (HiiHandles[Index], FormSetTitle, "en-US");
        }
        ASSERT (String != NULL);
        Token = HiiSetString (HiiHandle, Token, String, Language);
        FreePool (String);
        ASSERT (Token != 0);

        String = HiiGetString (HiiHandles[Index], FormSetHelp, Language);
        if (String == NULL) {
          String = HiiGetString (HiiHandles[Index], FormSetHelp, "en-US");
        }
        ASSERT (String != NULL);
        TokenHelp = HiiSetString (HiiHandle, TokenHelp, String, Language);
        FreePool (String);
        ASSERT (TokenHelp != 0);
      }

      FreePool (SupportedLanguages);

      HiiCreateGotoExOpCode (
            StartOpCodeHandle,
            0,
            Token,
            TokenHelp,
            Count == 0 ? EFI_IFR_FLAG_CALLBACK : 0,
            (EFI_QUESTION_ID) (Count + Label),
            0,
            &GotoFormSetGuid,
            DevicePathId
            );

      Count ++;
    }
  }

  if (Count) {
    Status = HiiUpdateForm (
                              HiiHandle,
                              FormSetGuid,
                              FormId,
                              StartOpCodeHandle,
                              EndOpCodeHandle
                              );
    DMSG ((EFI_D_ERROR, "AddDynamicFormset, Found %d FormSet :%r.\n", Count, Status));
  }

  //
  // Remove our packagelist from HII database.
  //
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);
  FreePool (HiiHandles);
  FreePool (UpdatePageSupportedLanguages);

  return Status;
}

EFI_STATUS
UpdateDynamicPages (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  SETUP_DYNAMIC_ITEM                     *SetupItem,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  SETUP_FORMSET_INFO    *SetupFormSet;
  EFI_STATUS    Status = EFI_SUCCESS;

  if (Action != EFI_BROWSER_ACTION_FORM_OPEN) {
    return EFI_UNSUPPORTED;
  }

  SetupFormSet = BYO_FORMSET_INFO_FROM_THIS (This);

  //
  //Add Third-part device Dynamic Formset.
  //
  if (SetupItem->DynamicItem.RefreshLabel == LABEL_DYNAMIC_FORMSET && QuestionId == LABEL_DYNAMIC_FORMSET) {
    Status = AddDynamicFormset (
                SetupFormSet->HiiHandle, 
                &SetupFormSet->FormSetInfo.FormSetGuid, 
                &gEfiHiiPlatformSetupFormsetGuid,
                SetupItem->DynamicItem.FormId, 
                SetupItem->DynamicItem.RefreshLabel
              );
    DMSG((EFI_D_ERROR, "InstallFormset, Add Third-part Dynamic FormSet :0x%x-%r.\n", SetupItem->DynamicItem.RefreshLabel, Status));
  }
  //
  //Add Third-part device Dynamic Formset.
  //
  if (SetupItem->DynamicItem.RefreshLabel == LABEL_PCIE_DEVICE_LIST && QuestionId == LABEL_PCIE_DEVICE_LIST) {
    Status = AddDynamicFormset (
                SetupFormSet->HiiHandle, 
                &SetupFormSet->FormSetInfo.FormSetGuid, 
                &gIfrByoDevicesUiPageGuid,
                SetupItem->DynamicItem.FormId, 
                SetupItem->DynamicItem.RefreshLabel
              );
    DMSG((EFI_D_ERROR, "InstallFormset, Add Devices Dynamic FormSet :0x%x-%r.\n", SetupItem->DynamicItem.RefreshLabel, Status));
  }
  //
  //Add Boot related pages
  //
  if (SetupItem->DynamicItem.RefreshLabel == LABEL_CHANGE_BOOT_ORDER && QuestionId == LABEL_CHANGE_BOOT_ORDER) {    
    Status = AddDynamicFormset (
        SetupFormSet->HiiHandle, 
        &SetupFormSet->FormSetInfo.FormSetGuid, 
        &gEfiIfrByoBootUiPageGuid,
        SetupItem->DynamicItem.FormId,
        SetupItem->DynamicItem.RefreshLabel
        );
    DMSG((EFI_D_ERROR, "InstallFormset, Add Boot Dynamic FormSet :0x%x-%r.\n", SetupItem->DynamicItem.RefreshLabel, Status));
  }

  //
  //Add Security related pages
  //
  if (SetupItem->DynamicItem.RefreshLabel == SECURITY_DYNAMIC_LABEL && QuestionId == SECURITY_DYNAMIC_LABEL) {
    Status = AddDynamicFormset (
        SetupFormSet->HiiHandle, 
        &SetupFormSet->FormSetInfo.FormSetGuid, 
        &gIfrByoSecurityUiPageGuid,
        SetupItem->DynamicItem.FormId,
        SetupItem->DynamicItem.RefreshLabel
        );
    DMSG((EFI_D_ERROR, "InstallFormset, Add Security Dynamic FormSet :0x%x-%r.\n", SetupItem->DynamicItem.RefreshLabel, Status));
  }

  return Status;
}

