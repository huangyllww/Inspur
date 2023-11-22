/** @file

Copyright (c) 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  Redfish.c

Abstract:
  Binding the harddisk(SATA or NVME) according to the configuration set by users.
When this function has been opened, it can't boot from the unbound harddisk.

Revision History:

**/

#include "Setup.h"

#include <RedfishJsonStructure/Bios/v1_0_0/EfiBiosV1_0_0.h>
#include <RedfishJsonStructure/AttributeRegistry/v1_0_0/EfiAttributeRegistryV1_0_0.h>
#include <ByoBootGroup.h>
#include <SysMiscCfg.h>
#include <Library/ByoCommLib.h>

#define NUMBER_SETTING 0x100
#define TYPE_INT   "Integer"
#define TYPE_ENUM "Enumeration"

#define TRUE_STRING  "TRUE"
#define FALSE_STRING "FALSE"

#define RESET_BIOS_DEFAULT_SETTING "Reset Bios Default Setting"
#define LANGUAGE_SELECTION "Select Language"

#define SETTING_MODE_STANDARD       1
#define SETTING_MODE_STORAGE_NAME   2

CHAR8 *mPromptString = NULL;
CHAR16 mChineseString[]=L"Chinese";

extern CHAR16 *mUnknownString;
extern FORM_BROWSER_FORMSET  *mSystemLevelFormSet;
extern EFI_HII_HANDLE mSetupBrowserHandle;
extern EFI_GUID gByoSetupSearchFormGuid;

/**
  Load hii formset which would be displayed by the browser.

**/
VOID
LoadAllHiiFormset (
  VOID
  );

BOOLEAN
IsSupportedFormSet (
  FORM_BROWSER_FORMSET    *FormSet
  )
{
  BYO_BROWSER_FORMSET     *ByoFormSet;
  UINT8                   Index;
  LIST_ENTRY              *Link;

  //
  //  Look for the same node.
  //
  Link = GetFirstNode (&gByoFormsetManager->ByoFormSetList);
  while (!IsNull (&gByoFormsetManager->ByoFormSetList, Link)) {
    ByoFormSet = BYO_FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&gByoFormsetManager->ByoFormSetList, Link);
    if (CompareGuid(&ByoFormSet->Guid, &FormSet->Guid) && (CompareGuid(&gByoSetupSearchFormGuid, &FormSet->Guid) == FALSE)) {
      return TRUE;
    }
  }

  for (Index = 0; Index < FormSet->NumberOfClassGuid; Index ++) {
    if (CompareGuid(&(FormSet->ClassGuid[Index]), &gIfrByoRedfishUiPageGuid)) {
      return TRUE;
    }
  }

  return FALSE;
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
  );

/**
  Clean AltCfg string for current formset.

  @param  FormSet                Form data structure.

**/
VOID
CleanAltCfgForFormSet (
  IN FORM_BROWSER_FORMSET   *FormSet
  );

/**
  Get the ascii string based on the StringId and HII Package List Handle.

  @param  Token                  The String's ID.
  @param  HiiHandle              The package list in the HII database to search for
                                 the specified string.

  @return The output string.

**/
CHAR8 *
GetTokenEngCharString (
  IN  EFI_STRING_ID                Token,
  IN  EFI_HII_HANDLE               HiiHandle
  )
{
  EFI_STRING  String;
  UINTN       DestMax;
  UINTN       Index;
  CHAR8       *CharString;

  ASSERT (HiiHandle != NULL);
  ASSERT (Token != 0);

  String = HiiGetString (HiiHandle, Token, "en-US");
  if (String == NULL || String[0] == WIDE_CHAR) {
    if ((mPromptString != NULL) && (AsciiStrCmp (mPromptString, LANGUAGE_SELECTION) == 0)) {
      String = AllocateCopyPool (StrSize (mChineseString), mChineseString);
    } else {
      String = AllocateCopyPool (StrSize (mUnknownString), mUnknownString);
    }
    ASSERT (String != NULL);
  }

  for (Index = 0; String[Index] != '\0'; Index ++) {
    //
    // Don't support non English Char
    //
    if (String[Index] == GEOMETRICSHAPE_RIGHT_TRIANGLE) {
      String[Index] = L' ';
    }

    if (String[Index] >= 0x100) {
      String[Index] = '\0';
    }
  }

  DestMax = StrLen (String) + 1;
  CharString = AllocateZeroPool (DestMax);

  UnicodeStrToAsciiStrS (String, CharString, DestMax);
  FreePool (String);
  AsciiDeleteExtraSpaces (CharString);
  return CharString;
}

struct {
  BM_MENU_TYPE  BootGroupType;
  EFI_STRING_ID StringId;
  CHAR8         *StringBuffer;
} mBootGroupString [] = {
  {BM_MENU_TYPE_UEFI_HDD, STRING_TOKEN (STR_UEFI_HDD)},
  {BM_MENU_TYPE_UEFI_ODD, STRING_TOKEN (STR_UEFI_ODD)},
  {BM_MENU_TYPE_UEFI_USB_DISK, STRING_TOKEN (STR_UEFI_USB_DISK)},
  {BM_MENU_TYPE_UEFI_USB_ODD, STRING_TOKEN (STR_UEFI_USB_ODD)},
  {BM_MENU_TYPE_UEFI_PXE, STRING_TOKEN (STR_UEFI_PXE)},
  {BM_MENU_TYPE_UEFI_OTHERS, STRING_TOKEN (STR_UEFI_OTHERS)},
  {BM_MENU_TYPE_LEGACY_HDD, STRING_TOKEN (STR_LEGACY_HDD)},
  {BM_MENU_TYPE_LEGACY_ODD, STRING_TOKEN (STR_LEGACY_ODD)},
  {BM_MENU_TYPE_LEGACY_USB_DISK, STRING_TOKEN (STR_LEGACY_USB_DISK)},
  {BM_MENU_TYPE_LEGACY_USB_ODD, STRING_TOKEN (STR_LEGACY_USB_ODD)},
  {BM_MENU_TYPE_LEGACY_PXE, STRING_TOKEN (STR_LEGACY_PXE)},
  {BM_MENU_TYPE_LEGACY_OTHERS, STRING_TOKEN (STR_LEGACY_OTHERS)},
  {BM_MENU_TYPE_LEGACY_MAX, 0}
};

UINT8 gLegacyBootGroupOrder[] = {
  BM_MENU_TYPE_LEGACY_HDD,
  BM_MENU_TYPE_LEGACY_ODD,
  BM_MENU_TYPE_LEGACY_USB_DISK,
  BM_MENU_TYPE_LEGACY_USB_ODD,
  BM_MENU_TYPE_LEGACY_PXE,
  BM_MENU_TYPE_LEGACY_OTHERS
  };

UINT8 gUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD,
  BM_MENU_TYPE_UEFI_ODD,
  BM_MENU_TYPE_UEFI_USB_DISK,
  BM_MENU_TYPE_UEFI_USB_ODD,
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_OTHERS
  };

#define BOOT_GROUP_ORDER_COUNT sizeof (gUefiBootGroupOrder) / sizeof (UINT8)

CHAR8*
GetStringTokenId (
  UINT8 BootGroupType
  )
{
  for (UINTN Index = 0; mBootGroupString[Index].BootGroupType != BM_MENU_TYPE_LEGACY_MAX; Index ++) {
    if (mBootGroupString[Index].BootGroupType == BootGroupType) {
      return mBootGroupString[Index].StringBuffer;
    }
  }
  ASSERT (FALSE);
  return NULL;
}

VOID
SetBootOrderOption (
  RedfishAttributeRegistry_V1_0_0_AttributeValue_Array_CS *OptionValue,
  UINT8                *BootGroupOrder
  )
{
  CHAR8 *StringBuffer;

  for (UINTN Index = 0; Index < BOOT_GROUP_ORDER_COUNT; Index ++) {
    OptionValue->Next = OptionValue + 1;
    OptionValue->ArrayValue = AllocateZeroPool (sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_CS));
    StringBuffer = GetStringTokenId (*BootGroupOrder);
    OptionValue->ArrayValue->ValueDisplayName = AllocateCopyPool (AsciiStrSize (StringBuffer), StringBuffer);
    OptionValue->ArrayValue->ValueName        = AllocateCopyPool (AsciiStrSize (StringBuffer), StringBuffer);
    OptionValue = OptionValue->Next;
    BootGroupOrder ++;
  }
  (OptionValue - 1)->Next = NULL;
}

VOID
SetBootOrderArray (
  RedfishCS_char_Array *ThisElement,
  UINT8                *BootGroupOrder
  )
{
  CHAR8 *StringBuffer;

  for (UINTN Index = 0; Index < BOOT_GROUP_ORDER_COUNT; Index ++) {
    ThisElement->Next = ThisElement + 1;
    StringBuffer = GetStringTokenId (*BootGroupOrder);
    ThisElement->ArrayValue = AllocateCopyPool (AsciiStrSize (StringBuffer), StringBuffer);
    ThisElement = ThisElement->Next;
    BootGroupOrder ++;
  }
  (ThisElement - 1)->Next = NULL;
}

VOID
AppendBootOrderSetting (
  RedfishCS_EmptyProp_KeyValue  *CurrentSetting,
  RedfishAttributeRegistry_V1_0_0_Attributes_Array_CS  *ValueAttribute,
  UINT8                *CurrentBootGroupOrder,
  UINT8                *DefaultBootGroupOrder,
  CHAR8                *PromptString,
  CHAR16               *StorageName
  )
{
  UINT16                  ArrayNumber;
  UINTN                   BufferIndex;
  RedfishAttributeRegistry_V1_0_0_AttributeValue_Array_CS *OptionValue;
  RedfishCS_char_Array *ThisElement;
  RedfishCS_char_Array *CurrentElement;

  CurrentSetting->NextKeyValuePtr = CurrentSetting + 1;
  CurrentSetting->KeyNamePtr = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
  CurrentSetting->Value = AllocateZeroPool (sizeof (RedfishCS_Vague));
  CurrentSetting->Value->DataType = RedfishCS_Vague_DataType_Array;
  ArrayNumber = BOOT_GROUP_ORDER_COUNT;
  CurrentSetting->Value->DataValue.ArrayPtr = AllocateZeroPool (sizeof (RedfishCS_char_Array) * ArrayNumber);
  SetBootOrderArray (CurrentSetting->Value->DataValue.ArrayPtr, CurrentBootGroupOrder);

  ValueAttribute->Next = ValueAttribute + 1;
  ValueAttribute->ArrayValue = AllocateZeroPool (sizeof (RedfishAttributeRegistry_V1_0_0_Attributes_CS));
  if (StorageName != NULL) {
    UINTN       DestMax;
    CHAR8       *CharString;

    //
    // Only record storage
    //
    DestMax = StrLen (StorageName) + 1;
    CharString = AllocateZeroPool (DestMax);
    UnicodeStrToAsciiStrS (StorageName, CharString, DestMax);
    ValueAttribute->ArrayValue->AttributeName = CharString;
    return;
  }

  ValueAttribute->ArrayValue->AttributeName = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
  ValueAttribute->ArrayValue->DisplayName  = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
  ValueAttribute->ArrayValue->CurrentValue = AllocateCopyPool (sizeof (RedfishCS_Vague), CurrentSetting->Value);
  ValueAttribute->ArrayValue->DefaultValue = AllocateCopyPool (sizeof (RedfishCS_Vague), CurrentSetting->Value);
  ValueAttribute->ArrayValue->HelpText     = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
  ValueAttribute->ArrayValue->MenuPath     = AllocateCopyPool (AsciiStrSize ("./"), "./");
  ValueAttribute->ArrayValue->DisplayOrder = AllocateZeroPool (sizeof (UINT64));
  *(ValueAttribute->ArrayValue->DisplayOrder) = 1;

  ValueAttribute->ArrayValue->CurrentValue->DataValue.ArrayPtr = AllocateZeroPool (sizeof (RedfishCS_char_Array) * ArrayNumber);
  ValueAttribute->ArrayValue->DefaultValue->DataValue.ArrayPtr = AllocateZeroPool (sizeof (RedfishCS_char_Array) * ArrayNumber);

  ValueAttribute->ArrayValue->Type  = AllocateCopyPool (AsciiStrSize (TYPE_ENUM), TYPE_ENUM);
  ValueAttribute->ArrayValue->Value = AllocateZeroPool (ArrayNumber * sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_Array_CS));
  SetBootOrderOption (ValueAttribute->ArrayValue->Value, DefaultBootGroupOrder);

  ThisElement    = ValueAttribute->ArrayValue->CurrentValue->DataValue.ArrayPtr;
  CurrentElement = CurrentSetting->Value->DataValue.ArrayPtr;
  for (BufferIndex = 0; BufferIndex < BOOT_GROUP_ORDER_COUNT; BufferIndex ++) {
    ThisElement->Next = ThisElement + 1;
    ThisElement->ArrayValue = AllocateCopyPool (AsciiStrSize (CurrentElement->ArrayValue), CurrentElement->ArrayValue);
    ThisElement = ThisElement->Next;
    CurrentElement = CurrentElement->Next;
  }
  (ThisElement - 1)->Next = NULL;

  ThisElement = ValueAttribute->ArrayValue->DefaultValue->DataValue.ArrayPtr;
  OptionValue = ValueAttribute->ArrayValue->Value;
  for (BufferIndex = 0; BufferIndex < BOOT_GROUP_ORDER_COUNT; BufferIndex ++) {
    ThisElement->Next = ThisElement + 1;
    ThisElement->ArrayValue = AllocateCopyPool (AsciiStrSize (OptionValue->ArrayValue->ValueName), OptionValue->ArrayValue->ValueName);
    ThisElement = ThisElement->Next;
    OptionValue = OptionValue->Next;
  }
  (ThisElement - 1)->Next = NULL;
}

VOID
AppendDefaultSetting (
  RedfishCS_EmptyProp_KeyValue  *CurrentSetting,
  RedfishAttributeRegistry_V1_0_0_Attributes_Array_CS  *ValueAttribute
  )
{
  CHAR8                   *PromptString;
  RedfishAttributeRegistry_V1_0_0_AttributeValue_Array_CS *OptionValue;

  PromptString = RESET_BIOS_DEFAULT_SETTING;
  CurrentSetting->NextKeyValuePtr = CurrentSetting + 1;
  CurrentSetting->KeyNamePtr = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
  CurrentSetting->Value = AllocateZeroPool (sizeof (RedfishCS_Vague));
  CurrentSetting->Value->DataType = RedfishCS_Vague_DataType_String;
  CurrentSetting->Value->DataValue.CharPtr = AllocateCopyPool (AsciiStrSize (FALSE_STRING), FALSE_STRING);

  ValueAttribute->Next = ValueAttribute + 1;
  ValueAttribute->ArrayValue = AllocateZeroPool (sizeof (RedfishAttributeRegistry_V1_0_0_Attributes_CS));
  ValueAttribute->ArrayValue->AttributeName = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
  ValueAttribute->ArrayValue->DisplayName  = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
  ValueAttribute->ArrayValue->CurrentValue = AllocateCopyPool (sizeof (RedfishCS_Vague), CurrentSetting->Value);
  ValueAttribute->ArrayValue->DefaultValue = AllocateCopyPool (sizeof (RedfishCS_Vague), CurrentSetting->Value);
  ValueAttribute->ArrayValue->HelpText     = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
  ValueAttribute->ArrayValue->MenuPath     = AllocateCopyPool (AsciiStrSize ("./"), "./");
  ValueAttribute->ArrayValue->DisplayOrder = AllocateZeroPool (sizeof (UINT64));
  *(ValueAttribute->ArrayValue->DisplayOrder) = 0;

  ValueAttribute->ArrayValue->Type = AllocateCopyPool (AsciiStrSize (TYPE_ENUM), TYPE_ENUM);
  ValueAttribute->ArrayValue->CurrentValue->DataValue.CharPtr = AllocateCopyPool (AsciiStrSize (FALSE_STRING), FALSE_STRING);
  ValueAttribute->ArrayValue->DefaultValue->DataValue.CharPtr = AllocateCopyPool (AsciiStrSize (FALSE_STRING), FALSE_STRING);
  ValueAttribute->ArrayValue->Value = AllocateZeroPool (2 * sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_Array_CS));
  OptionValue = ValueAttribute->ArrayValue->Value;
  OptionValue[0].Next = OptionValue + 1;
  OptionValue[0].ArrayValue = AllocateZeroPool (sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_CS));
  OptionValue[0].ArrayValue->ValueDisplayName = AllocateCopyPool (AsciiStrSize (TRUE_STRING), TRUE_STRING);
  OptionValue[0].ArrayValue->ValueName        = AllocateCopyPool (AsciiStrSize (TRUE_STRING), TRUE_STRING);

  OptionValue[1].Next = NULL;
  OptionValue[1].ArrayValue = AllocateZeroPool (sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_CS));
  OptionValue[1].ArrayValue->ValueDisplayName = AllocateCopyPool (AsciiStrSize (FALSE_STRING), FALSE_STRING);
  OptionValue[1].ArrayValue->ValueName        = AllocateCopyPool (AsciiStrSize (FALSE_STRING), FALSE_STRING);

  return;
}

CHAR8 *mUefiBootGroupPrompt;
CHAR8 *mLegacyBootGroupPrompt;

UINT8 
ConvertBootGroupStringToValue (
  CHAR8 *GroupName
  )
{
  for (UINTN Index = 0; mBootGroupString[Index].BootGroupType != BM_MENU_TYPE_LEGACY_MAX; Index ++) {
    if (AsciiStrCmp (mBootGroupString[Index].StringBuffer, GroupName) == 0) {
      return mBootGroupString[Index].BootGroupType;
    }
  }
  ASSERT (FALSE);
  return 0;
}

// input 
// RedfishCS_EmptyProp_KeyValue *KeyValuePtr
// SetQuestionValue (FormSet, Form, Question, GetSetValueWithEditBuffer);
// SubmitForm (NULL, NULL, SystemLevel);
// 
//
// output 
// RedfishAttributeRegistry_V1_0_0_Attributes_Array_CS      *Attributes
// RedfishCS_EmptyProp_KeyValue *KeyValuePtr
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
ReterieveSetupOptionsCommon (
  IN  VOID *InKeyValueArray,
  OUT VOID **OutKeyValueArray,
  OUT VOID **OutAttributesArray,
  IN  UINT8  SettingMode
  )
{
  FORM_BROWSER_FORMSET    *FormSet;
  EFI_HII_HANDLE          *HiiHandle;
  LIST_ENTRY              *Link;
  LIST_ENTRY              *FormLink;
  LIST_ENTRY              *QuestionLink;
  LIST_ENTRY              *OptionLink;
  FORM_BROWSER_FORM       *Form;
  FORM_BROWSER_STATEMENT  *Question;
  CHAR8                   *PromptString;
  CHAR8                   *OptionString;
  CHAR8                   *CurrentOptionString;
  CHAR8                   *DefaultOptionString;
  CHAR8                   *TempString;
  CHAR8                   *MenuPath;
  CHAR8                   *FormSetTitle;
  CHAR8                   *FormTitle;
  QUESTION_OPTION         *Option;
  UINT64                  CurrentValue;
  UINT64                  UpdateValue;
  UINT64                  DefaultValue;
  UINTN                   ValueWidth;
  UINTN                   MenuPathLength;
  UINTN                   OptionIndexInForm;
  UINT16                  ValueOffset;
  EFI_HII_VALUE           DefaultHiiValue;
  INTN                    Result;
  LIST_ENTRY              *LinkStorage;
  FORMSET_STORAGE         *FormSetStorage;
  BROWSER_STORAGE         *Storage;
  EFI_STRING              Progress;
  UINTN                   BufferSize;
  BOOLEAN                 IsUpdateQuestion;
  EXPRESS_RESULT          ReturnVal;
  QUESTION_DEFAULT        *RefValue;
  LIST_ENTRY              HideFormList;
  BOOLEAN                 IsHideForm;
  STATIC BOOLEAN          RefreshFromSetting = FALSE;
  UI_MENU_SELECTION       *CurrentSelection;
  UI_MENU_SELECTION       Selection;
  FORM_BROWSER_FORMSET    *OldSystemLevelFormSet;
  UINTN                         NumberOfSetting;
  UINTN                         IndexOfSetting;
  UINTN                         OptionNumber;
  RedfishCS_EmptyProp_KeyValue  *KeyValue;
  RedfishCS_EmptyProp_KeyValue  *CurrentSettings;
  RedfishAttributeRegistry_V1_0_0_Attributes_Array_CS  *ValueAttributes;
  RedfishAttributeRegistry_V1_0_0_AttributeValue_Array_CS *OptionValue;
  RedfishCS_char_Array *ThisElement;
  RedfishCS_char_Array *CurrentElement;
  UINTN BootGroupOrderSize;
  UINT8 CurrentBootGroupOrder [BOOT_GROUP_ORDER_COUNT];
  UINT8 BootGroupOrder [BOOT_GROUP_ORDER_COUNT];
  UINT8 DefaultBootGroupOrder [BOOT_GROUP_ORDER_COUNT];
  UINT8 *DefaultBootGroupOrderPointer;
  UINT8 *CurrentBuffer = NULL;
  UINT8 *UpdateBuffer = NULL;
  UINTN BufferIndex;
  static BOOLEAN IsInitBootGroupString = FALSE;
  EFI_STATUS Status;

  if (IsInitBootGroupString == FALSE) {
    for (UINTN Index = 0; mBootGroupString[Index].BootGroupType != BM_MENU_TYPE_LEGACY_MAX; Index ++) {
      mBootGroupString[Index].StringBuffer = GetTokenEngCharString (mBootGroupString[Index].StringId, mSetupBrowserHandle);
    }
    mUefiBootGroupPrompt   = GetTokenEngCharString (STRING_TOKEN (STR_UEFI_BOOT_ORDER), mSetupBrowserHandle);
    mLegacyBootGroupPrompt = GetTokenEngCharString (STRING_TOKEN (STR_LEGACY_BOOT_ORDER), mSetupBrowserHandle);
    IsInitBootGroupString = TRUE;
  }

  IndexOfSetting  = 0;
  NumberOfSetting = NUMBER_SETTING;
  CurrentSettings  = AllocateZeroPool (NumberOfSetting * sizeof (RedfishCS_EmptyProp_KeyValue));
  ValueAttributes = AllocateZeroPool (NumberOfSetting * sizeof (RedfishAttributeRegistry_V1_0_0_Attributes_Array_CS));

  //
  // Init Local Variable
  //
  IsUpdateQuestion = FALSE;
  InitializeListHead (&HideFormList);

  //
  // Prepare all HII formset
  //
  LoadAllHiiFormset ();

  MenuPath     = NULL;
  FormSetTitle = NULL;
  FormTitle    = NULL;
  CurrentSelection = gCurrentSelection;
  OldSystemLevelFormSet = mSystemLevelFormSet;
  if (gCurrentSelection == NULL) {
    gCurrentSelection = &Selection;
  }

  if (mSystemLevelFormSet == NULL) {
    Link = GetFirstNode (&gBrowserFormSetList);
    while (!IsNull (&gBrowserFormSetList, Link)) {
      FormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
      Link = GetNextNode (&gBrowserFormSetList, Link);
      if (!ValidateFormSet(FormSet)) {
        continue;
      }
      mSystemLevelFormSet = FormSet;
      break;
    }
  }

  for (KeyValue = InKeyValueArray; KeyValue != NULL; KeyValue = KeyValue->NextKeyValuePtr) {
    if (SettingMode == SETTING_MODE_STANDARD && AsciiStrCmp (KeyValue->KeyNamePtr, RESET_BIOS_DEFAULT_SETTING) == 0) {
      if (AsciiStrCmp (KeyValue->Value->DataValue.CharPtr, TRUE_STRING) == 0) {
        //
        // Reset Default Setting, and ignore the input setting
        //
        InKeyValueArray = NULL;
        RefreshFromSetting = TRUE;
        ExtractDefault (NULL, NULL, 0, SystemLevel, GetDefaultForAll, NULL, FALSE, FALSE);
        SubmitForm (NULL, NULL, SystemLevel);
        break;
      }
    }

    if ((PcdGet8(PcdBiosBootModeType) != BIOS_BOOT_LEGACY_OS) && AsciiStrCmp (KeyValue->KeyNamePtr, mUefiBootGroupPrompt) == 0) {
      BootGroupOrderSize = BOOT_GROUP_ORDER_COUNT;
      Status = gRT->GetVariable(
                      BYO_UEFI_BOOT_GROUP_VAR_NAME,
                      &gByoGlobalVariableGuid,
                      NULL,
                      &BootGroupOrderSize,
                      CurrentBootGroupOrder
                      );
      if (!EFI_ERROR (Status)) {
        ThisElement = KeyValue->Value->DataValue.ArrayPtr;
        BufferIndex = 0;
        while (ThisElement != NULL) {
          BootGroupOrder[BufferIndex++] = ConvertBootGroupStringToValue (ThisElement->ArrayValue);
          ThisElement = ThisElement->Next;
        }
        ASSERT (BufferIndex == BOOT_GROUP_ORDER_COUNT);

        BootGroupOrderSize = BOOT_GROUP_ORDER_COUNT;
        Status = gRT->SetVariable (
                        BYO_UEFI_BOOT_GROUP_VAR_NAME,
                        &gByoGlobalVariableGuid,
                        BYO_BG_ORDER_VAR_ATTR,
                        BootGroupOrderSize,
                        BootGroupOrder
                        );
        ASSERT_EFI_ERROR (Status);

        if (CompareMem (CurrentBootGroupOrder, BootGroupOrder, BOOT_GROUP_ORDER_COUNT) != 0) {
          // IsUpdateQuestion = TRUE;
        }
      }
    }

    if ((PcdGet8(PcdBiosBootModeType) != BIOS_BOOT_UEFI_OS) && AsciiStrCmp (KeyValue->KeyNamePtr, mLegacyBootGroupPrompt) == 0) {
      BootGroupOrderSize = BOOT_GROUP_ORDER_COUNT;
      Status = gRT->GetVariable(
                      BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                      &gByoGlobalVariableGuid,
                      NULL,
                      &BootGroupOrderSize,
                      CurrentBootGroupOrder
                      );
      if (!EFI_ERROR (Status)) {
        ThisElement = KeyValue->Value->DataValue.ArrayPtr;
        BufferIndex = 0;
        while (ThisElement != NULL) {
          BootGroupOrder[BufferIndex++] = ConvertBootGroupStringToValue (ThisElement->ArrayValue);
          ThisElement = ThisElement->Next;
        }
        ASSERT (BufferIndex == BOOT_GROUP_ORDER_COUNT);

        BootGroupOrderSize = BOOT_GROUP_ORDER_COUNT;
        Status = gRT->SetVariable (
                        BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                        &gByoGlobalVariableGuid,
                        BYO_BG_ORDER_VAR_ATTR,
                        BootGroupOrderSize,
                        BootGroupOrder
                        );
        ASSERT_EFI_ERROR (Status);
      }
      if (CompareMem (CurrentBootGroupOrder, BootGroupOrder, BOOT_GROUP_ORDER_COUNT) != 0) {
        // IsUpdateQuestion = TRUE;
      }
    }
  }

  //
  // Formset
  //
  Link = GetFirstNode (&gBrowserFormSetList);
  while (!IsNull (&gBrowserFormSetList, Link)) {
    FormSet = FORM_BROWSER_FORMSET_FROM_LINK (Link);
    Link = GetNextNode (&gBrowserFormSetList, Link);
    if (!ValidateFormSet(FormSet)) {
      continue;
    }

    if (!IsSupportedFormSet (FormSet)) {
      continue;
    }

    //
    // Set current formset
    //
    mSystemLevelFormSet = FormSet;

    //
    // Update Setting in FormSet
    //
    if (RefreshFromSetting) {
      LoadFormSetConfig (NULL, FormSet);
    }

    HiiHandle = FormSet->HiiHandle;

    if (FormSetTitle != NULL) {
      FreePool (FormSetTitle);
    }
    FormSetTitle = GetTokenEngCharString (FormSet->FormSetTitle, HiiHandle);
    DEBUG ((DEBUG_INFO, "FormSet %a\n", FormSetTitle));

    //
    // Get Storage Default Value
    //
    LinkStorage = GetFirstNode (&FormSet->StorageListHead);
    while (!IsNull (&FormSet->StorageListHead, LinkStorage)) {
      FormSetStorage = FORMSET_STORAGE_FROM_LINK (LinkStorage);
      Storage        = FormSetStorage->BrowserStorage;

      if (Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER || Storage->Type == EFI_HII_VARSTORE_BUFFER) {
        ExtractAltCfgForFormSet (FormSet, EFI_HII_DEFAULT_CLASS_STANDARD, Storage);
        if (FormSetStorage->HasCallAltCfg) {
          BufferSize = Storage->Size;
          mHiiConfigRouting->ConfigToBlock (
                                  mHiiConfigRouting,
                                  FormSetStorage->ConfigAltResp,
                                  Storage->DefaultBuffer,
                                  &BufferSize,
                                  &Progress
                                  );
        }
      }

      LinkStorage = GetNextNode (&FormSet->StorageListHead, LinkStorage);
    }
    CleanAltCfgForFormSet (FormSet);

    //
    // Form
    //
    FormLink = GetFirstNode (&FormSet->FormListHead);
    while (!IsNull (&FormSet->FormListHead, FormLink)) {
      Form = FORM_BROWSER_FORM_FROM_LINK (FormLink);
      FormLink = GetNextNode (&FormSet->FormListHead, FormLink);

      if (Form->SuppressExpression != NULL) {
        if (EvaluateExpressionList(Form->SuppressExpression, TRUE, FormSet, Form) != ExpressFalse) {
          //
          // skip the suppressif form
          //
          continue;
        }
      }

      //
      // Check whether Form is invisible
      //
      IsHideForm = FALSE;
      if (!IsListEmpty (&HideFormList)) {
        //DMSG ((EFI_D_ERROR, "GetQuestionDefault, EFI_IFR_DEFAULT, \n"));
        QuestionLink = GetFirstNode (&HideFormList);
        while (!IsNull (&HideFormList, QuestionLink)) {
          RefValue = QUESTION_DEFAULT_FROM_LINK (QuestionLink);
          if (RefValue->Value.Value.ref.FormId == Form->FormId) {
            if ((CompareMem (&RefValue->Value.Value.ref.FormSetGuid, &gZeroGuid, sizeof (EFI_GUID)) == 0) ||
                (CompareMem (&RefValue->Value.Value.ref.FormSetGuid, &FormSet->Guid, sizeof (EFI_GUID)) == 0)) {
              //
              // Check Form ID and FormSet Guid
              //
              IsHideForm = TRUE;
              break;
            }
          }
          QuestionLink = GetNextNode (&HideFormList, QuestionLink);
        }
      }

      if (IsHideForm) {
        //
        // Skip this from
        //
        continue;
      }

      OptionIndexInForm = 0;
      if (FormTitle != NULL) {
        FreePool (FormTitle);
      }
      FormTitle = GetTokenEngCharString (Form->FormTitle, HiiHandle);

      if (MenuPath != NULL) {
        FreePool (MenuPath);
      }
      //
      // ./ FormSetTitle + / + FormTitle
      //
      MenuPathLength = 2 + AsciiStrSize (FormSetTitle) + AsciiStrSize (FormTitle);
      MenuPath = AllocateZeroPool (MenuPathLength);
      AsciiStrCatS (MenuPath, MenuPathLength, "./");
      AsciiStrCatS (MenuPath, MenuPathLength, FormSetTitle);
      AsciiStrCatS (MenuPath, MenuPathLength, "/");
      AsciiStrCatS (MenuPath, MenuPathLength, FormTitle);

      //
      // Question
      //
      QuestionLink = GetFirstNode (&Form->StatementListHead);
      while (!IsNull (&Form->StatementListHead, QuestionLink)) {
        Question = FORM_BROWSER_STATEMENT_FROM_LINK (QuestionLink);
        QuestionLink = GetNextNode (&Form->StatementListHead, QuestionLink);

        //
        // Skip the suppressif and disableif Goto opcode
        //
        if ((Question->Expression != NULL) && (Question->Operand == EFI_IFR_REF_OP)) {
          ReturnVal = EvaluateExpressionList(Question->Expression, TRUE, FormSet, Form);
          if (ReturnVal == ExpressDisable || ReturnVal == ExpressSuppress || ReturnVal == ExpressGrayOut) {
            //
            // Add this form into the ignore form list
            //
            RefValue = AllocateZeroPool (sizeof (QUESTION_DEFAULT));
            RefValue->Signature = QUESTION_DEFAULT_SIGNATURE;
            CopyMem (&RefValue->Value, &Question->HiiValue, sizeof (RefValue->Value));
            InsertTailList (&HideFormList, &RefValue->Link);
          }
        }

        //
        // Only handle EfiVarStore that maps to EFI variable
        //
        if (Question->Prompt != 0 && Question->Storage != NULL && (Question->Storage->Type == EFI_HII_VARSTORE_EFI_VARIABLE_BUFFER || Question->Storage->Type == EFI_HII_VARSTORE_BUFFER)) {
          if (Question->Operand == EFI_IFR_CHECKBOX_OP || Question->Operand == EFI_IFR_NUMERIC_OP || Question->Operand == EFI_IFR_ONE_OF_OP || Question->Operand == EFI_IFR_ORDERED_LIST_OP) {
            //
            // Skip the disabled options
            //
            ReturnVal = ExpressFalse;
            if (Question->Expression != NULL) {
              ReturnVal = EvaluateExpressionList(Question->Expression, TRUE, FormSet, Form);
              if (ReturnVal == ExpressDisable || ReturnVal == ExpressSuppress || ReturnVal == ExpressGrayOut) {
                continue;
              }
            }
            //
            // Question Value
            //
            PromptString = GetTokenEngCharString (Question->Prompt, HiiHandle);
            if (AsciiStrLen (PromptString) == 0 || (PromptString[0] == (CHAR8) mUnknownString[0])) {
              //
              // Skip the option without prompting string
              //
              continue;
            }
            mPromptString = PromptString;
            
            DMSG ((DEBUG_INFO, "  Question PromptString %a, Index is %d, and Opcode is %2x\n", PromptString, (UINT32)IndexOfSetting, Question->Operand));
            OptionString = NULL;
            ValueWidth   = 1;
            CurrentValue = 0;
            switch (Question->HiiValue.Type) {
            case EFI_IFR_TYPE_BOOLEAN:
              CurrentValue = (UINT64) Question->HiiValue.Value.b;
              ValueWidth = 1;
              break;
            case EFI_IFR_TYPE_NUM_SIZE_8:
              CurrentValue = (UINT64) Question->HiiValue.Value.u8;
              ValueWidth = 1;
              break;
            case EFI_IFR_TYPE_NUM_SIZE_16:
              CurrentValue = (UINT64) Question->HiiValue.Value.u16;
              ValueWidth = 2;
              break;
            case EFI_IFR_TYPE_NUM_SIZE_32:
              CurrentValue = (UINT64) Question->HiiValue.Value.u32;
              ValueWidth = 4;
              break;
            case EFI_IFR_TYPE_NUM_SIZE_64:
              CurrentValue = Question->HiiValue.Value.u64;
              ValueWidth = 8;
              break;
            case EFI_IFR_TYPE_BUFFER:
              CurrentBuffer = Question->BufferValue;
              UpdateBuffer  = AllocateZeroPool (sizeof(UINT64) * Question->MaxContainers);
              ValueWidth    = Question->StorageWidth / Question->MaxContainers;
              break;
            default:
              ASSERT (FALSE);
              break;
            }

            //
            // Check input question, if the value is different, then set question value.
            //
            for (KeyValue = InKeyValueArray; KeyValue != NULL; KeyValue = KeyValue->NextKeyValuePtr) {
              if (AsciiStrCmp (KeyValue->KeyNamePtr, PromptString) == 0) {
                UpdateValue = CurrentValue;
                switch (KeyValue->Value->DataType)
                {
                case RedfishCS_Vague_DataType_Array:
                  if (Question->Operand == EFI_IFR_ORDERED_LIST_OP) {
                    ThisElement = KeyValue->Value->DataValue.ArrayPtr;
                    BufferIndex = 0;
                    while (ThisElement != NULL) {
                      UpdateValue = (UINTN) -1;
                      OptionLink = GetFirstNode (&Question->OptionListHead);
                      while (!IsNull (&Question->OptionListHead, OptionLink)) {
                        Option = QUESTION_OPTION_FROM_LINK (OptionLink);
                        TempString = GetTokenEngCharString (Option->Text, HiiHandle);
                        if (AsciiStrCmp (ThisElement->ArrayValue, TempString) == 0) {
                          UpdateValue = Option->Value.Value.u64;
                          FreePool (TempString);
                          break;
                        }
                        FreePool (TempString);
                        OptionLink = GetNextNode (&Question->OptionListHead, OptionLink);
                      }
                      if (UpdateValue == -1) {
                        //
                        // No matched option is found. Still use original order.
                        //
                        break;
                      }
                      CopyMem (UpdateBuffer + BufferIndex * ValueWidth, &UpdateValue, ValueWidth);
                      ThisElement = ThisElement->Next;
                      BufferIndex ++;
                    }
                    if (BufferIndex != Question->MaxContainers) {
                      //
                      // Option doesn't match. Still use current setting
                      //
                      CopyMem (UpdateBuffer, CurrentBuffer, ValueWidth * Question->MaxContainers);
                      break;
                    }
                  }
                  UpdateValue = CurrentValue;
                  DMSG ((DEBUG_INFO, "  OrderList Question %a Value has been process. \n", PromptString));
                  break;
                case RedfishCS_Vague_DataType_String:
                  if (Question->Operand == EFI_IFR_ONE_OF_OP) {
                    OptionLink = GetFirstNode (&Question->OptionListHead);
                    while (!IsNull (&Question->OptionListHead, OptionLink)) {
                      Option = QUESTION_OPTION_FROM_LINK (OptionLink);
                      TempString = GetTokenEngCharString (Option->Text, HiiHandle);
                      if (AsciiStrCmp (KeyValue->Value->DataValue.CharPtr, TempString) == 0) {
                        UpdateValue = Option->Value.Value.u64;
                        FreePool (TempString);
                        break;
                      }
                      FreePool (TempString);
                      OptionLink = GetNextNode (&Question->OptionListHead, OptionLink);
                    }
                  } else if (Question->Operand == EFI_IFR_CHECKBOX_OP) {
                    if (AsciiStrCmp (KeyValue->Value->DataValue.CharPtr, TRUE_STRING) == 0) {
                      UpdateValue = 1;
                    } else if (AsciiStrCmp (KeyValue->Value->DataValue.CharPtr, FALSE_STRING) == 0) {
                      UpdateValue = 0;
                    }
                  }
                  DMSG ((DEBUG_INFO, "  Question Change Value is %a and Current Value is %a \n", KeyValue->Value->DataValue.CharPtr, OptionString));
                  break;
                case RedfishCS_Vague_DataType_Int64:
                  UpdateValue = *(KeyValue->Value->DataValue.Int64Ptr);
                  DMSG ((DEBUG_INFO, "  Question Change Value is %d and Current Value is %d \n", (UINT32) UpdateValue, (UINT32)CurrentValue));
                  break;
                case RedfishCS_Vague_DataType_Bool:
                  UpdateValue = (UINT8) *(KeyValue->Value->DataValue.BoolPtr);
                  break;
                default:
                  break;
                }

                if (UpdateValue != CurrentValue) {
                  IsUpdateQuestion = TRUE;
                  Question->HiiValue.Value.u64 = UpdateValue;
                  CurrentValue = UpdateValue;
                  Question->ValueChanged = TRUE;
                  gCurrentSelection->FormSet = FormSet;
                  gCurrentSelection->Form = Form;
                  if ((Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) == EFI_IFR_FLAG_CALLBACK) {
                    ProcessCallBackFunction(NULL, FormSet, Form, Question, EFI_BROWSER_ACTION_CHANGING, TRUE);
                    ProcessCallBackFunction(NULL, FormSet, Form, Question, EFI_BROWSER_ACTION_CHANGED, TRUE);
                  } else {
                    SetQuestionValue (FormSet, Form, Question, GetSetValueWithEditBuffer);
                  }
                  CurrentValue = Question->HiiValue.Value.u64;
                }
                if (Question->Operand == EFI_IFR_ORDERED_LIST_OP) {
                  if (CompareMem (UpdateBuffer, CurrentBuffer, ValueWidth * Question->MaxContainers) != 0) {
                    IsUpdateQuestion = TRUE;
                    Question->ValueChanged = TRUE;
                    gCurrentSelection->FormSet = FormSet;
                    gCurrentSelection->Form = Form;
                    CopyMem (CurrentBuffer, UpdateBuffer, ValueWidth * Question->MaxContainers);
                    if ((Question->QuestionFlags & EFI_IFR_FLAG_CALLBACK) == EFI_IFR_FLAG_CALLBACK) {
                      ProcessCallBackFunction(NULL, FormSet, Form, Question, EFI_BROWSER_ACTION_CHANGING, TRUE);
                      ProcessCallBackFunction(NULL, FormSet, Form, Question, EFI_BROWSER_ACTION_CHANGED, TRUE);
                    } else {
                      SetQuestionValue (FormSet, Form, Question, GetSetValueWithEditBuffer);
                    }
                    FreePool (UpdateBuffer);
                  }
                }
                break;
              }
            }

            //
            // Current Value = PromptString + OptionString or PromptString + CurrentValue
            //
            if (IndexOfSetting >= NumberOfSetting) {
              CurrentSettings  = ReallocatePool (NumberOfSetting * sizeof (CurrentSettings[0]), (NumberOfSetting + NUMBER_SETTING) * sizeof (CurrentSettings[0]), CurrentSettings);
              ValueAttributes = ReallocatePool (NumberOfSetting * sizeof (ValueAttributes[0]), (NumberOfSetting + NUMBER_SETTING) * sizeof (ValueAttributes[0]), ValueAttributes);
              NumberOfSetting = NumberOfSetting + NUMBER_SETTING;
            }
            CurrentSettings[IndexOfSetting].NextKeyValuePtr = CurrentSettings + IndexOfSetting + 1;
            CurrentSettings[IndexOfSetting].KeyNamePtr = PromptString;
            CurrentSettings[IndexOfSetting].Value = AllocateZeroPool (sizeof (RedfishCS_Vague));

            if (Question->Operand == EFI_IFR_ONE_OF_OP) {
              Option = ValueToOption (Question, &(Question->HiiValue));
              if (Option == NULL) {
                continue;
              }
              OptionString = GetTokenEngCharString (Option->Text, HiiHandle);
              CurrentSettings[IndexOfSetting].Value->DataType = RedfishCS_Vague_DataType_String;
              CurrentSettings[IndexOfSetting].Value->DataValue.CharPtr  = OptionString;
            } else if (Question->Operand == EFI_IFR_CHECKBOX_OP) {
              if (CurrentValue == 1) {
                OptionString = AllocateCopyPool (AsciiStrSize (TRUE_STRING), TRUE_STRING);
              } else {
                OptionString = AllocateCopyPool (AsciiStrSize (FALSE_STRING), FALSE_STRING);
              }
              CurrentSettings[IndexOfSetting].Value->DataType = RedfishCS_Vague_DataType_String;
              CurrentSettings[IndexOfSetting].Value->DataValue.CharPtr = OptionString;
            } else if (Question->Operand == EFI_IFR_NUMERIC_OP) {
              CurrentSettings[IndexOfSetting].Value->DataType = RedfishCS_Vague_DataType_Int64;
              CurrentSettings[IndexOfSetting].Value->DataValue.Int64Ptr = AllocateZeroPool (sizeof (UINT64));
              *(CurrentSettings[IndexOfSetting].Value->DataValue.Int64Ptr) = CurrentValue;
            } else if (Question->Operand == EFI_IFR_ORDERED_LIST_OP) {
              CurrentSettings[IndexOfSetting].Value->DataType = RedfishCS_Vague_DataType_Array;
              CurrentSettings[IndexOfSetting].Value->DataValue.ArrayPtr = AllocateZeroPool (sizeof (RedfishCS_char_Array) * Question->MaxContainers);
              ThisElement = CurrentSettings[IndexOfSetting].Value->DataValue.ArrayPtr;
              for (BufferIndex = 0; BufferIndex < Question->MaxContainers; BufferIndex ++) {
                TempString = NULL;
                OptionLink = GetFirstNode (&Question->OptionListHead);
                while (!IsNull (&Question->OptionListHead, OptionLink)) {
                  Option = QUESTION_OPTION_FROM_LINK (OptionLink);
                  if (CompareMem (CurrentBuffer + BufferIndex * ValueWidth, &(Option->Value.Value.u64), ValueWidth) == 0) {
                    TempString = GetTokenEngCharString (Option->Text, HiiHandle);
                    break;
                  }
                  OptionLink = GetNextNode (&Question->OptionListHead, OptionLink);
                }
                ASSERT (TempString != NULL);
                ThisElement->Next = ThisElement + 1;
                ThisElement->ArrayValue = TempString;
                ThisElement = ThisElement->Next;
              }
              (ThisElement - 1)->Next = NULL;
            } else {
              ASSERT (FALSE);
            }

            //
            // Question Attribute = Value Option + Default Value
            //
            ValueOffset  = Question->VarStoreInfo.VarOffset;
            DefaultValue = 0;
            CopyMem (&DefaultValue, Question->Storage->DefaultBuffer + ValueOffset, ValueWidth);
            DefaultOptionString = NULL;
            CurrentOptionString = OptionString;

            ValueAttributes[IndexOfSetting].Next = ValueAttributes + IndexOfSetting + 1;
            ValueAttributes[IndexOfSetting].ArrayValue = AllocateZeroPool (sizeof (RedfishAttributeRegistry_V1_0_0_Attributes_CS));
            if (SettingMode == SETTING_MODE_STORAGE_NAME) {
              UINTN       DestMax;
              CHAR8       *CharString;
              //
              // Only record storage name on this mode
              //
              DestMax = StrLen (Question->Storage->Name) + 1;
              CharString = AllocateZeroPool (DestMax);

              UnicodeStrToAsciiStrS (Question->Storage->Name, CharString, DestMax);
              ValueAttributes[IndexOfSetting].ArrayValue->AttributeName = CharString;
              //
              // Go to next option
              //
              IndexOfSetting ++;
              continue;
            }
            ValueAttributes[IndexOfSetting].ArrayValue->AttributeName = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
            ValueAttributes[IndexOfSetting].ArrayValue->DisplayName  = AllocateCopyPool (AsciiStrSize (PromptString), PromptString);
            ValueAttributes[IndexOfSetting].ArrayValue->CurrentValue = AllocateCopyPool (sizeof (RedfishCS_Vague), CurrentSettings[IndexOfSetting].Value);
            ValueAttributes[IndexOfSetting].ArrayValue->DefaultValue = AllocateCopyPool (sizeof (RedfishCS_Vague), CurrentSettings[IndexOfSetting].Value);
            ValueAttributes[IndexOfSetting].ArrayValue->HelpText     = GetTokenEngCharString (Question->Help, HiiHandle);
            ValueAttributes[IndexOfSetting].ArrayValue->MenuPath     = AllocateCopyPool (AsciiStrSize (MenuPath), MenuPath);
            ValueAttributes[IndexOfSetting].ArrayValue->DisplayOrder = AllocateZeroPool (sizeof (UINT64));
            *(ValueAttributes[IndexOfSetting].ArrayValue->DisplayOrder) = (UINT64) OptionIndexInForm++;
            if (ReturnVal == ExpressGrayOut) {
              ValueAttributes[IndexOfSetting].ArrayValue->GrayOut = AllocateZeroPool (sizeof (RedfishCS_bool));
              *(ValueAttributes[IndexOfSetting].ArrayValue->GrayOut) = RedfishCS_boolean_true;
            }
            if ((Question->QuestionFlags & EFI_IFR_FLAG_READ_ONLY) == EFI_IFR_FLAG_READ_ONLY) {
              ValueAttributes[IndexOfSetting].ArrayValue->ReadOnly = AllocateZeroPool (sizeof (RedfishCS_bool));
              *(ValueAttributes[IndexOfSetting].ArrayValue->ReadOnly) = RedfishCS_boolean_true;
            }

            if (Question->Operand == EFI_IFR_NUMERIC_OP) {
              ValueAttributes[IndexOfSetting].ArrayValue->Type = AllocateCopyPool (AsciiStrSize (TYPE_INT), TYPE_INT);
              ValueAttributes[IndexOfSetting].ArrayValue->LowerBound = AllocateZeroPool (sizeof (UINT64));
              ValueAttributes[IndexOfSetting].ArrayValue->UpperBound = AllocateZeroPool (sizeof (UINT64));
              ValueAttributes[IndexOfSetting].ArrayValue->ScalarIncrement = AllocateZeroPool (sizeof (UINT64));

              *(ValueAttributes[IndexOfSetting].ArrayValue->LowerBound) = Question->Minimum;
              *(ValueAttributes[IndexOfSetting].ArrayValue->UpperBound) = Question->Maximum;
              *(ValueAttributes[IndexOfSetting].ArrayValue->ScalarIncrement) = Question->Step;

              ValueAttributes[IndexOfSetting].ArrayValue->CurrentValue->DataValue.Int64Ptr    = AllocateZeroPool (sizeof (UINT64));
              *(ValueAttributes[IndexOfSetting].ArrayValue->CurrentValue->DataValue.Int64Ptr) = CurrentValue;
              ValueAttributes[IndexOfSetting].ArrayValue->DefaultValue->DataValue.Int64Ptr    = AllocateZeroPool (sizeof (UINT64));
              *(ValueAttributes[IndexOfSetting].ArrayValue->DefaultValue->DataValue.Int64Ptr) = DefaultValue;
            } else if (Question->Operand == EFI_IFR_CHECKBOX_OP) {
              if (DefaultValue == 1) {
                DefaultOptionString = AllocateCopyPool (AsciiStrSize (TRUE_STRING), TRUE_STRING);
              } else {
                DefaultOptionString = AllocateCopyPool (AsciiStrSize (FALSE_STRING), FALSE_STRING);
              }

              ValueAttributes[IndexOfSetting].ArrayValue->Type = AllocateCopyPool (AsciiStrSize (TYPE_ENUM), TYPE_ENUM);
              ValueAttributes[IndexOfSetting].ArrayValue->CurrentValue->DataValue.CharPtr = AllocateCopyPool (AsciiStrSize (CurrentOptionString), CurrentOptionString);
              ValueAttributes[IndexOfSetting].ArrayValue->DefaultValue->DataValue.CharPtr = DefaultOptionString;
              ValueAttributes[IndexOfSetting].ArrayValue->Value = AllocateZeroPool (2 * sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_Array_CS));
              OptionValue = ValueAttributes[IndexOfSetting].ArrayValue->Value;
              OptionValue[0].Next = OptionValue + 1;
              OptionValue[0].ArrayValue = AllocateZeroPool (sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_CS));
              OptionValue[0].ArrayValue->ValueDisplayName = AllocateCopyPool (AsciiStrSize (TRUE_STRING), TRUE_STRING);
              OptionValue[0].ArrayValue->ValueName        = AllocateCopyPool (AsciiStrSize (TRUE_STRING), TRUE_STRING);

              OptionValue[1].Next = NULL;
              OptionValue[1].ArrayValue = AllocateZeroPool (sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_CS));
              OptionValue[1].ArrayValue->ValueDisplayName = AllocateCopyPool (AsciiStrSize (FALSE_STRING), FALSE_STRING);
              OptionValue[1].ArrayValue->ValueName        = AllocateCopyPool (AsciiStrSize (FALSE_STRING), FALSE_STRING);
            } else if (Question->Operand == EFI_IFR_ONE_OF_OP || Question->Operand == EFI_IFR_ORDERED_LIST_OP) {
              CopyMem (&DefaultHiiValue, &(Question->HiiValue), sizeof (DefaultHiiValue));
              DefaultHiiValue.Value.u64 = DefaultValue;

              OptionNumber = 0;
              OptionLink = GetFirstNode (&Question->OptionListHead);
              while (!IsNull (&Question->OptionListHead, OptionLink)) {
                OptionLink = GetNextNode (&Question->OptionListHead, OptionLink);
                OptionNumber ++;
              }

              if (OptionNumber == 0) {
                //
                // Ignore the one of option question if no option
                //
                continue;
              }

              ValueAttributes[IndexOfSetting].ArrayValue->Value = AllocateZeroPool (OptionNumber * sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_Array_CS));
              OptionValue = ValueAttributes[IndexOfSetting].ArrayValue->Value;

              OptionLink = GetFirstNode (&Question->OptionListHead);
              while (!IsNull (&Question->OptionListHead, OptionLink)) {
                Option = QUESTION_OPTION_FROM_LINK (OptionLink);

                if (Question->Operand == EFI_IFR_ONE_OF_OP) {
                  if ((CompareHiiValue (&Option->Value, &DefaultHiiValue, &Result, NULL) == EFI_SUCCESS) && (Result == 0)) {
                    DefaultOptionString = GetTokenEngCharString (Option->Text, HiiHandle);
                  }
                }
                OptionString = GetTokenEngCharString (Option->Text, HiiHandle);
                OptionLink = GetNextNode (&Question->OptionListHead, OptionLink);

                OptionValue->Next = OptionValue + 1;
                OptionValue->ArrayValue = AllocateZeroPool (sizeof (RedfishAttributeRegistry_V1_0_0_AttributeValue_CS));
                OptionValue->ArrayValue->ValueDisplayName = OptionString;
                OptionValue->ArrayValue->ValueName        = AllocateCopyPool (AsciiStrSize (OptionString), OptionString);
                OptionValue++;
              }
              if (OptionValue != NULL) {
                (OptionValue-1)->Next = NULL;
              }

              ValueAttributes[IndexOfSetting].ArrayValue->Type = AllocateCopyPool (AsciiStrSize (TYPE_ENUM), TYPE_ENUM);
              
              if (Question->Operand == EFI_IFR_ONE_OF_OP) {
                if (DefaultOptionString == NULL) {
                  DefaultOptionString = AllocateCopyPool (AsciiStrSize (OptionString), OptionString);
                }
                ValueAttributes[IndexOfSetting].ArrayValue->CurrentValue->DataValue.CharPtr = AllocateCopyPool (AsciiStrSize (CurrentOptionString), CurrentOptionString);
                ValueAttributes[IndexOfSetting].ArrayValue->DefaultValue->DataValue.CharPtr = DefaultOptionString;
              }
              if (Question->Operand == EFI_IFR_ORDERED_LIST_OP) {
                ASSERT (OptionNumber == Question->MaxContainers);
                ValueAttributes[IndexOfSetting].ArrayValue->CurrentValue->DataValue.ArrayPtr = AllocateZeroPool (sizeof (RedfishCS_char_Array) * Question->MaxContainers);
                ValueAttributes[IndexOfSetting].ArrayValue->DefaultValue->DataValue.ArrayPtr = AllocateZeroPool (sizeof (RedfishCS_char_Array) * Question->MaxContainers);

                ThisElement    = ValueAttributes[IndexOfSetting].ArrayValue->CurrentValue->DataValue.ArrayPtr;
                CurrentElement = CurrentSettings[IndexOfSetting].Value->DataValue.ArrayPtr;
                for (BufferIndex = 0; BufferIndex < Question->MaxContainers; BufferIndex ++) {
                  ThisElement->Next = ThisElement + 1;
                  ThisElement->ArrayValue = AllocateCopyPool (AsciiStrSize (CurrentElement->ArrayValue), CurrentElement->ArrayValue);
                  ThisElement = ThisElement->Next;
                  CurrentElement = CurrentElement->Next;
                }
                (ThisElement - 1)->Next = NULL;

                ThisElement = ValueAttributes[IndexOfSetting].ArrayValue->DefaultValue->DataValue.ArrayPtr;
                OptionValue = ValueAttributes[IndexOfSetting].ArrayValue->Value;
                for (BufferIndex = 0; BufferIndex < Question->MaxContainers; BufferIndex ++) {
                  ThisElement->Next = ThisElement + 1;
                  ThisElement->ArrayValue = AllocateCopyPool (AsciiStrSize (OptionValue->ArrayValue->ValueName), OptionValue->ArrayValue->ValueName);
                  ThisElement = ThisElement->Next;
                  OptionValue = OptionValue->Next;
                }
                (ThisElement - 1)->Next = NULL;
              }
            } else {
              ASSERT (FALSE);
            }
            //
            // Go to next option
            //
            IndexOfSetting ++;
          }
        }
      }
    }
  }

  //
  // restore gCurrentSelection
  //
  gCurrentSelection = CurrentSelection;
  mSystemLevelFormSet = OldSystemLevelFormSet;

  if (FormSetTitle != NULL) {
    FreePool (FormSetTitle);
  }
  if (FormTitle != NULL) {
    FreePool (FormTitle);
  }
  if (MenuPath != NULL) {
    FreePool (MenuPath);
  }

  //
  // Free RefValue List
  //
  while (!IsListEmpty (&HideFormList)) {
    Link = GetFirstNode (&HideFormList);
    RefValue = QUESTION_DEFAULT_FROM_LINK (Link);
    RemoveEntryList (&RefValue->Link);

    FreePool (RefValue);
  }

  if (IndexOfSetting == 0) {
    //
    // Don't find any setup settings
    //
    FreePool (CurrentSettings);
    FreePool (ValueAttributes);
    *OutKeyValueArray     = NULL;
    *OutAttributesArray   = NULL;
    return EFI_NOT_FOUND;
  }

  //
  // Append Default Setting
  //
  if (SettingMode == SETTING_MODE_STANDARD) {
    AppendDefaultSetting (CurrentSettings + IndexOfSetting, ValueAttributes + IndexOfSetting);
    IndexOfSetting++;
  }

  //
  // Add UEFI/Legacy boot group order variable
  //
  BootGroupOrderSize = BOOT_GROUP_ORDER_COUNT;
  Status = gRT->GetVariable(
                  BYO_UEFI_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &BootGroupOrderSize,
                  BootGroupOrder
                  );
  if (!EFI_ERROR (Status) && (PcdGet8(PcdBiosBootModeType) != BIOS_BOOT_LEGACY_OS)) {
    DefaultBootGroupOrderPointer = gUefiBootGroupOrder;
    BootGroupOrderSize = BOOT_GROUP_ORDER_COUNT;
    Status = gRT->GetVariable(
                    BYO_UEFI_DEFAULT_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    NULL,
                    &BootGroupOrderSize,
                    DefaultBootGroupOrder
                    );
    if (!EFI_ERROR (Status)) {
      DefaultBootGroupOrderPointer = DefaultBootGroupOrder;
    }
    AppendBootOrderSetting (
      CurrentSettings + IndexOfSetting, 
      ValueAttributes + IndexOfSetting, 
      BootGroupOrder, 
      DefaultBootGroupOrderPointer, 
      mUefiBootGroupPrompt,
      SettingMode == SETTING_MODE_STORAGE_NAME ? BYO_UEFI_BOOT_GROUP_VAR_NAME:NULL
      );
    IndexOfSetting++;
  }

  BootGroupOrderSize = BOOT_GROUP_ORDER_COUNT;
  Status = gRT->GetVariable(
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &BootGroupOrderSize,
                  BootGroupOrder
                  );
  if (!EFI_ERROR (Status) && (PcdGet8(PcdBiosBootModeType) != BIOS_BOOT_UEFI_OS)) {
    DefaultBootGroupOrderPointer = gLegacyBootGroupOrder;
    BootGroupOrderSize = BOOT_GROUP_ORDER_COUNT;
    Status = gRT->GetVariable(
                    BYO_LEGACY_DEFAULT_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    NULL,
                    &BootGroupOrderSize,
                    DefaultBootGroupOrder
                    );
    if (!EFI_ERROR (Status)) {
      DefaultBootGroupOrderPointer = DefaultBootGroupOrder;
    }

    AppendBootOrderSetting (
      CurrentSettings + IndexOfSetting, 
      ValueAttributes + IndexOfSetting, 
      BootGroupOrder, 
      DefaultBootGroupOrderPointer, 
      mLegacyBootGroupPrompt,
      SettingMode == SETTING_MODE_STORAGE_NAME ? BYO_LEGACY_BOOT_GROUP_VAR_NAME:NULL
      );

    IndexOfSetting++;
  }

  //
  // Set the last option NULL as the terminator.
  //
  IndexOfSetting = IndexOfSetting - 1;
  CurrentSettings[IndexOfSetting].NextKeyValuePtr = NULL;
  ValueAttributes[IndexOfSetting].Next = NULL;

  //
  // After apply new setting, then retrieve new setting again
  //
  if (IsUpdateQuestion) {
    SubmitForm (NULL, NULL, SystemLevel);

    FreePool (CurrentSettings);
    FreePool (ValueAttributes);

    RefreshFromSetting = TRUE;

    ReterieveSetupOptionsCommon (
      NULL,
      (VOID **) &CurrentSettings,
      (VOID **) &ValueAttributes,
      SettingMode
      );
  }

  //
  // Return as the output 
  //
  *OutKeyValueArray   = (VOID *) CurrentSettings;
  *OutAttributesArray = (VOID *) ValueAttributes;

  return EFI_SUCCESS;
}

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
  return ReterieveSetupOptionsCommon (InKeyValueArray, OutKeyValueArray, OutAttributesArray, SETTING_MODE_STANDARD);
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
  return ReterieveSetupOptionsCommon (InKeyValueArray, OutKeyValueArray, OutStorageNameArray, SETTING_MODE_STORAGE_NAME);
}