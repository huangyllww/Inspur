/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <Guid/MdeModuleHii.h>
#include <Guid/GlobalVariable.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/DevicePathToText.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/ByoUefiBootManagerLib.h>
#include <Protocol/ByoFormSetManager.h>
#include <ByoPlatformSetupConfig.h>
#include <Protocol/DisplayProtocol.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include "DirectBoot.h"
#include <Token.h>
#include "FrontPage.h"
#include <Guid/ByoSetupFormsetGuid.h>


VOID    *mStartOpCodeHandle;
VOID    *mEndOpCodeHandle;
EFI_IFR_GUID_LABEL    *mStartGuidLabel;
EFI_IFR_GUID_LABEL    *mEndGuidLabel;


VOID
RefreshLabel (	
  IN UINT16    StartLabelNum,
  IN UINT16    EndLabelNum
  )
{
  //
  //Remove Form Label.
  //
  if (mStartOpCodeHandle) {
    HiiFreeOpCodeHandle (mStartOpCodeHandle);
  }  
  if (mEndOpCodeHandle) {
    HiiFreeOpCodeHandle (mEndOpCodeHandle);
  }

  //
  //Create Form Label.
  //
  mStartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (mStartOpCodeHandle != NULL);

  mEndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (mEndOpCodeHandle != NULL);
  //
  // Create Hii Extend Label OpCode as the start opcode
  //
  mStartGuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (mStartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  mStartGuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  mStartGuidLabel->Number = StartLabelNum;

  //
  // Create Hii Extend Label OpCode as the end opcode
  //
  mEndGuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (mEndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  mEndGuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  mEndGuidLabel->Number = EndLabelNum;

  return;
}


/**
  Create direct boot item according to BootOrder Variable.

**/
EFI_STATUS
CreateDirectBootItem (
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  EFI_GUID    *FormsetGuid,
  IN  EFI_FORM_ID    FormId,
  IN  EFI_STRING_ID  HelpToken
  )
{
  EFI_STATUS    Status;
  UINTN    Index;
  EFI_BOOT_MANAGER_LOAD_OPTION    *BootOption;
  UINTN    BootOptionCount;  
  EFI_STRING_ID    PromptToken;

  DEBUG ((EFI_D_ERROR, "\n %a(), guid %g\n",__FUNCTION__,*FormsetGuid));
  if (HiiHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BootOption = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
  if(BootOption == NULL){
    return EFI_NOT_FOUND;
  }

  DEBUG ((EFI_D_ERROR, "%a(), BootOptionCount :%d.\n", __FUNCTION__,BootOptionCount));
  if (!BootOptionCount) {
    return EFI_NOT_READY;
  }
  RefreshLabel (LABEL_DIRECT_BOOT, LABEL_END);

  for (Index = 0; Index < BootOptionCount; Index++) {
    //
    // Don't display the hidden/inactive boot option.
    //
    if (((BootOption[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) || ((BootOption[Index].Attributes & LOAD_OPTION_ACTIVE) == 0)) {
      continue;
    }
    //
    // Set new Hii String to show the item.
    //
    PromptToken = HiiSetString (HiiHandle, 0, BootOption[Index].Description, NULL);
    if (PromptToken == 0) {
      continue;
    }

    DEBUG ((EFI_D_ERROR, "%a(), BootOption[%d] :%d-%s.\n",__FUNCTION__, Index, BootOption[Index].OptionNumber, BootOption[Index].Description));
    HiiCreateActionOpCode (
        mStartOpCodeHandle, 
        (EFI_QUESTION_ID) (LABEL_DIRECT_BOOT + BootOption[Index].OptionNumber), 
        PromptToken, 
        HelpToken, 
        EFI_IFR_FLAG_CALLBACK, 
        0
        );      
  }
  ByoEfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);

  Status = HiiUpdateForm (HiiHandle, FormsetGuid, FormId,mStartOpCodeHandle, mEndOpCodeHandle );
  DEBUG ((EFI_D_ERROR, "%a(), HiiUpdateForm :%r.\n",__FUNCTION__, Status));

  return Status;
}


/**
  Boot from selected item.

**/
VOID
DirectBootFromOptionNumber (
  IN   UINTN  OptionNumber
  )
{
  EFI_STATUS                            Status;
  EFI_STATUS                            ImageReturnStatus;
  EDKII_FORM_DISPLAY_ENGINE_PROTOCOL    *DisplayEngine = NULL;
  UINTN                                 Index;
  EFI_BOOT_MANAGER_LOAD_OPTION          *BootOption;
  UINTN                                 BootOptionCount;  

  Status = gBS->LocateProtocol (
                  &gEdkiiFormDisplayEngineProtocolGuid,
                  NULL,
                  (void **)&DisplayEngine
                  );
  DEBUG ((EFI_D_INFO, "\n %a(), Locate FormDisplay :%r. \n", __FUNCTION__,Status));  

  BootOption = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
  if(BootOption == NULL){
    return;
  }

  for (Index = 0; Index < BootOptionCount; Index++) {
    if (BootOption[Index].OptionNumber == OptionNumber) {
      DEBUG ((EFI_D_INFO, "%a(), BootOption[%d] :%d. \n", __FUNCTION__,Index, OptionNumber));
      //
      //Clear setup.
      //
      if (DisplayEngine) {
        DisplayEngine->ExitDisplay();
      }

      //
      //Efi Boot.
      //	  
      ByoEfiBootManagerBoot (&BootOption[Index]);
      break;
    }
  }

  ByoEfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);

ReEntry:
  BootOption = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
  if(BootOption == NULL){
    return;
  }
  for (Index = 0; Index < BootOptionCount; Index++) {
    if ((BootOption[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      DEBUG((EFI_D_INFO, "NotActived\n"));
      continue;
    }

    ByoEfiBootManagerBoot(&BootOption[Index]);
    DEBUG((EFI_D_INFO, "boot %s,%r\n", BootOption[Index].Description,BootOption[Index].Status));
    ImageReturnStatus = BootOption[Index].Status;

  }
  ByoEfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);
 
  if( ImageReturnStatus == EFI_SUCCESS){
    goto ReEntry;
  }

  return;
}

/**
  The Callback Function of Direct Boot.
  
**/
EFI_STATUS
DirectBootCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS   Status;
  LIST_ENTRY    *Link;
  SETUP_FORMSET_INFO    *SetupFormSet;
  SETUP_DYNAMIC_ITEM    *SetupItem;
  static BOOLEAN    bRepaintDirectBoot = FALSE;

  DEBUG ((EFI_D_ERROR, "%a, Action :%d, QuestionId :0x%x, bRepaintDirectBoot :%d. \n", __FUNCTION__,Action, QuestionId, bRepaintDirectBoot));
  SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
  if ((Action == EFI_BROWSER_ACTION_FORM_OPEN) && (SetupFormSet != NULL) && bRepaintDirectBoot) {
    //
    // Repaint Direct Boot Item when form opened.
    //
    SetupItem = NULL;
    Link = GetFirstNode (&gByoSetup->DynamicItemList);
    while (!IsNull (&gByoSetup->DynamicItemList, Link)) {
      SetupItem = BYO_SETUP_ITEM_FROM_LINK(Link);
      if (SetupItem->DynamicItem.RefreshLabel == LABEL_DIRECT_BOOT) {
        bRepaintDirectBoot = FALSE;
        Status = CreateDirectBootItem (
                                SetupFormSet->HiiHandle,
                                &SetupFormSet->FormSetInfo.FormSetGuid,
                                SetupItem->DynamicItem.FormId,
                                SetupItem->DynamicItem.Help
                                );
        if (EFI_ERROR(Status)) {
          bRepaintDirectBoot = TRUE;
        }
      }
      Link = GetNextNode (&gByoSetup->DynamicItemList, Link);
    }
  }
  if ((Action == EFI_BROWSER_ACTION_FORM_CLOSE) && (!bRepaintDirectBoot)) {
    bRepaintDirectBoot = TRUE;
  }
  
  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    //
    // Do nothing for UEFI OPEN/CLOSE Action
    //
    return EFI_SUCCESS;
  }
  DEBUG ((EFI_D_INFO, "\n %a, QuestionId :0x%x, OptionNumber :%d. \n", __FUNCTION__,QuestionId, QuestionId - LABEL_DIRECT_BOOT));
  
  DirectBootFromOptionNumber (QuestionId - LABEL_DIRECT_BOOT);

  return EFI_SUCCESS;
}

