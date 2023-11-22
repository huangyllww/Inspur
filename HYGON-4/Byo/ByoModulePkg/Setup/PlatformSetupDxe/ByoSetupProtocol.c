/*++

Copyright (c) 2010 - 2018, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:
  Implement of platform setup protocol.

Revision History:


--*/

#include "PlatformSetupDxe.h"

//
//These three GUIDs are only used internally within the module to
//meet the requirements of Inspur
//
EFI_GUID gEfiFormsetGuidPlatformConfiguration = FORMSET_GUID_PLATFORM;
EFI_GUID gEfiFormsetGuidSocketConfiguration = SOCKET_FORMSET_GUID;
EFI_GUID gEfiFormsetGuidServerMgmt = SETUP_BMC_CFG_GUID;

EFI_BYO_PLATFORM_SETUP_PROTOCOL    *mByoSetup = NULL;

EFI_STATUS
InitializeByoMainFormset (
  EFI_BYO_PLATFORM_SETUP_PROTOCOL    *ByoSetup
  )
{
  EFI_STATUS Status;
  EFI_BYO_FORMSET_MANAGER_PROTOCOL    *FormsetManager = NULL;
  
  Status = gBS->LocateProtocol (
                  &gEfiByoFormsetManagerProtocolGuid,
                  NULL,
                  (VOID **) &FormsetManager
                  );
  DMSG((EFI_D_INFO, "InitializeByoMainFormset, Locate Formset Manager Protocol :%r.\n", Status));
  if (EFI_ERROR(Status) ) {  	
    return Status;
  }

  Status =FormsetManager->Insert (FormsetManager, &gEfiFormsetGuidMain);  
  DMSG((EFI_D_INFO, "InitializeByoMainFormset, Insert Formset Main :%r.\n", Status));

  Status =FormsetManager->Insert (FormsetManager, &gEfiFormsetGuidAdvance);
  DMSG((EFI_D_INFO, "InitializeByoMainFormset, Insert Formset Advanced :%r.\n", Status));

  Status =FormsetManager->Insert (FormsetManager, &gEfiFormsetGuidPlatformConfiguration);
  DMSG((EFI_D_INFO, "InitializeByoMainFormset, Insert Formset Platform Configuration :%r.\n", Status));

  Status =FormsetManager->Insert (FormsetManager, &gEfiFormsetGuidSocketConfiguration);
  DMSG((EFI_D_INFO, "InitializeByoMainFormset, Insert Formset Socket Configuration :%r.\n", Status));

  Status =FormsetManager->Insert (FormsetManager, &gEfiFormsetGuidServerMgmt);
  DMSG((EFI_D_INFO, "InitializeByoMainFormset, Insert Formset Server Mgmt :%r.\n", Status));

  Status =FormsetManager->Insert (FormsetManager, &gEfiFormsetGuidSecurity);
  DMSG((EFI_D_INFO, "InitializeByoMainFormset, Insert Formset Security :%r.\n", Status));
  
  Status =FormsetManager->Insert (FormsetManager, &gEfiFormsetGuidBoot);  
  DMSG((EFI_D_INFO, "InitializeByoMainFormset, Insert Formset Boot :%r.\n", Status));
	
  Status =FormsetManager->Insert (FormsetManager, &gEfiFormsetGuidExit);  
  DMSG((EFI_D_INFO, "InitializeByoMainFormset, Insert Formset Exit :%r.\n", Status));	

  //
  // Initialize Forms.
  //
  InitializeForm ();

  return EFI_SUCCESS;  
}


EFI_STATUS
AddMainFormset (
  IN EFI_BYO_PLATFORM_SETUP_PROTOCOL    *This,
  FORMSET_INFO    *FormSetInfo
  )
{
  LIST_ENTRY    *Link;
  SETUP_FORMSET_INFO    * SetupFormSet;
  SETUP_FORMSET_INFO    *FormSetNode;

  if (FormSetInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  DMSG ((EFI_D_ERROR, "AddMainFormset, FormSetGuid :%g.\n", &FormSetInfo->FormSetGuid));
  
  //
  //Check Formset to avoid duplication.
  //
  Link = GetFirstNode (&This->MainFormsetList);
  while (!IsNull (&This->MainFormsetList, Link)) {
    SetupFormSet = BYO_FORMSET_INFO_FROM_LINK (Link);
    if (SetupFormSet->FormSetInfo.Class == FormSetInfo->Class) {
      DMSG ((EFI_D_ERROR, "AddMainFormset, Formset already be Added.\n"));
      return EFI_ALREADY_STARTED;
    }
    Link = GetNextNode (&This->MainFormsetList, Link);
  }
  
  //
  // Insert new node.
  //
  FormSetNode = AllocateZeroPool (sizeof (SETUP_FORMSET_INFO));
  if (FormSetNode == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  FormSetNode->Signature = BYO_FORMSET_LIST_SIGNATURE;

  FormSetNode->FormSetInfo.Class = FormSetInfo->Class;
  CopyGuid (&FormSetNode->FormSetInfo.FormSetGuid, &FormSetInfo->FormSetGuid);
  
  FormSetNode->FormSetInfo.IfrPack = FormSetInfo->IfrPack;
  FormSetNode->FormSetInfo.StrPack = FormSetInfo->StrPack;
  

  FormSetNode->FormSetInfo.FormSetConfig.ExtractConfig = FormSetInfo->FormSetConfig.ExtractConfig;
  FormSetNode->FormSetInfo.FormSetConfig.RouteConfig = FormSetInfo->FormSetConfig.RouteConfig;
  FormSetNode->FormSetInfo.FormSetConfig.Callback = FormSetInfo->FormSetConfig.Callback;

  
  InsertTailList (&This->MainFormsetList, &FormSetNode->Link);
  
  return EFI_SUCCESS;  
}

EFI_STATUS
AddSetupDynamicItem (
  IN EFI_BYO_PLATFORM_SETUP_PROTOCOL    *This,
  DYNAMIC_ITEM    *Item
  )
{
  SETUP_DYNAMIC_ITEM    *SetupItem ;

  if (Item == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  DMSG ((EFI_D_ERROR, "AddSetupDynamicItem, RefreshLabel :0x%x. \n", Item->RefreshLabel));

  //
  // Insert new node.
  //
  SetupItem = AllocateZeroPool (sizeof (SETUP_DYNAMIC_ITEM));
  if (SetupItem == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  SetupItem->Signature = BYO_SETUP_ITEM_SIGNATURE;

  SetupItem->DynamicItem.Class = Item->Class;
  SetupItem->DynamicItem.FormId = Item->FormId;
  SetupItem->DynamicItem.RefreshLabel = Item->RefreshLabel;  
  SetupItem->DynamicItem.Prompt = Item->Prompt;
  SetupItem->DynamicItem.Help = Item->Help;

  InsertTailList (&This->DynamicItemList, &SetupItem->Link);
  
  return EFI_SUCCESS;  
}


EFI_STATUS
RunByoSetup (
  EFI_BYO_PLATFORM_SETUP_PROTOCOL    *ByoSetup
  )
{

  EFI_STATUS Status;
  EFI_BYO_FORMSET_MANAGER_PROTOCOL    *FormsetManager = NULL;
  
  Status = gBS->LocateProtocol (
                  &gEfiByoFormsetManagerProtocolGuid,
                  NULL,
                  (VOID **) &FormsetManager
                  );
  DMSG((EFI_D_ERROR, "\n RunByoSetup, Locate Formset Manager Protocol :%r.\n", Status));
  if (! EFI_ERROR(Status) ) {
    FormsetManager->Run(FormsetManager, &gEfiFormsetGuidMain);
  }
  
  return Status;  
}




/**
  Install Byo Setup Protocol interface.

**/
EFI_STATUS
InstallByoSetupProtocol (
  VOID
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  EFI_BYO_PLATFORM_SETUP_PROTOCOL    *ByoSetup = NULL;

  DMSG((EFI_D_ERROR, "InstallByoSetupProtocol(),\n"));
  Status = gBS->LocateProtocol (
                  &gEfiByoPlatformSetupGuid,
                  NULL,
                  (VOID**)&ByoSetup
                  );
  if ( ! EFI_ERROR(Status) ) {
    DMSG((EFI_D_ERROR, "InstallByoSetupProtocol(), Byo Setup Already Installed.\n"));
    mByoSetup = ByoSetup;
    return EFI_ALREADY_STARTED;
  }

  ByoSetup = AllocateZeroPool (sizeof(EFI_BYO_PLATFORM_SETUP_PROTOCOL));
  if (ByoSetup == NULL) {
    DMSG((EFI_D_ERROR, "InstallByoSetupProtocol(), Memory Out of Resources.\n"));
    return EFI_OUT_OF_RESOURCES;
  }

  InitializeListHead (&ByoSetup->MainFormsetList);
  InitializeListHead (&ByoSetup->DynamicItemList);

  ByoSetup->AddFormset = AddMainFormset;
  ByoSetup->AddDynamicItem = AddSetupDynamicItem;
  ByoSetup->InitializeMainFormset = InitializeByoMainFormset;
  ByoSetup->Run = RunByoSetup;
  
  Status = gBS->InstallProtocolInterface (
                 &ByoSetup->DriverHandle,
                 &gEfiByoPlatformSetupGuid,
                 EFI_NATIVE_INTERFACE,
                 ByoSetup
                 );
  if (!EFI_ERROR(Status)) {
    mByoSetup = ByoSetup;
  }
  
  return Status;
}




