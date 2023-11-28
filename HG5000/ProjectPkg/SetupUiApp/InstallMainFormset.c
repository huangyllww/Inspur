/** @file
  Byosoft Ui App module is driver for BDS phase.

 Copyright (c) 2010 - 2018, Byosoft Corporation.<BR>
 All rights reserved.This software and associated documentation (if any)
 is furnished under a license and may only be used or copied in
 accordance with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be reproduced,
 stored in a retrieval system, or transmitted in any form or by any
 means without the express written consent of Byosoft Corporation.

 File Name:

 Abstract:
    Byosoft Ui App module.

 Revision History:

**/

#include "FrontPage.h"


//
// module global data.
//
HII_VENDOR_DEVICE_PATH  gHiiByoDevicePath = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
          (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
        }
      },
      EFI_BYO_IFR_GUID,
    },
    0,
    0
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      END_DEVICE_PATH_LENGTH
    }
  }
};



EFI_STATUS
UiAppInstallFormset (
  SETUP_FORMSET_INFO    * SetupFormSet
  )
{
  EFI_STATUS    Status = EFI_SUCCESS;
  LIST_ENTRY    *Link;
  SETUP_DYNAMIC_ITEM    *SetupItem;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
  
  if (SetupFormSet == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  SetupFormSet->VendorDevicePath = AllocateCopyPool(sizeof (HII_VENDOR_DEVICE_PATH), &gHiiByoDevicePath);
  if (SetupFormSet->VendorDevicePath == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }  
  //
  // Use memory address as unique ID to distinguish from different device paths
  //
  SetupFormSet->VendorDevicePath->UniqueId = (UINTN)SetupFormSet->VendorDevicePath;
  
  //
  // Install Device Path Protocol and Config Access protocol to driver handle
  //
  CopyMem(&SetupFormSet->SetupConfig, &gSetupFormSetConfig, sizeof (EFI_HII_CONFIG_ACCESS_PROTOCOL));
  Status = gBS->InstallMultipleProtocolInterfaces (
                    &SetupFormSet->DriverHandle,
                    &gEfiDevicePathProtocolGuid,
                    SetupFormSet->VendorDevicePath,
                    &gEfiHiiConfigAccessProtocolGuid,
                    &SetupFormSet->SetupConfig,
                    NULL
                    );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "InstallFormset, Install ConfigAccess :%r\n", Status));
    return Status;  	
  }


// Publish All HII data.
// SC module may already install package with NULL driverHandle
// here we should remove it before.

  if(SetupFormSet->HiiHandle != NULL){
    HiiRemovePackages (SetupFormSet->HiiHandle);
    SetupFormSet->HiiHandle = NULL;  
  }
  
  SetupFormSet->HiiHandle = HiiAddPackages (
                              &SetupFormSet->FormSetInfo.FormSetGuid,
                              SetupFormSet->DriverHandle,
                              SetupFormSet->FormSetInfo.IfrPack,
                              SetupFormSet->FormSetInfo.StrPack,
                              NULL
                              );
  if (SetupFormSet->HiiHandle == NULL) {
    DEBUG((EFI_D_ERROR, "InstallFormset, HiiAddPackages error\n"));
    return EFI_INVALID_PARAMETER;
  }
	
  //
  //Add Dynamic Item  to Main formset.
  //  
  SetupItem = NULL;
  Link = GetFirstNode (&gByoSetup->DynamicItemList);
  while (!IsNull (&gByoSetup->DynamicItemList, Link)) {
    SetupItem = BYO_SETUP_ITEM_FROM_LINK (Link);
    if (SetupFormSet->FormSetInfo.Class == SetupItem->DynamicItem.Class) {
      //
      //Add change language function.
      //
      if (SetupItem->DynamicItem.RefreshLabel == LABEL_CHANGE_LANGUAGE) {    
        Status = UiAppUpdateLanguageSettingItem (
                   SetupFormSet->HiiHandle, 
                   &SetupFormSet->FormSetInfo.FormSetGuid, 
                   SetupItem->DynamicItem.FormId, 
                   SetupItem->DynamicItem.Prompt,
                   SetupItem->DynamicItem.Help
                   );
        SetupItem->Key = KEY_CHANGE_LANGUAGE;
        DEBUG((EFI_D_INFO, "InstallFormset, Update Change Language Item 0x%X: %r\n", SetupItem->DynamicItem.RefreshLabel, Status));
      }
#if SETUP_SINGLE_BOOT_OVERRIDE
      //
      //Add Direct Boot.
      //
      if (SetupItem->DynamicItem.RefreshLabel == LABEL_DIRECT_BOOT ) {
        Status = CreateDirectBootItem (
          SetupFormSet->HiiHandle,
          &SetupFormSet->FormSetInfo.FormSetGuid,
          SetupItem->DynamicItem.FormId,
          SetupItem->DynamicItem.Help
          );
        DEBUG((EFI_D_INFO, "InstallByoFormset, Add Direct Boot :0x%x-%r.\n", SetupItem->DynamicItem.RefreshLabel, Status));
      }
#endif
    }

    Link = GetNextNode (&gByoSetup->DynamicItemList, Link);
  }

  return Status;
}



EFI_STATUS
UiAppAddBaseFormset (
  VOID 
  )
{
  EFI_STATUS            Status = EFI_SUCCESS;
  LIST_ENTRY            *Link;
  SETUP_FORMSET_INFO    *SetupFormSet = NULL;
  
  Link = GetFirstNode (&gByoSetup->MainFormsetList);
  while (!IsNull (&gByoSetup->MainFormsetList, Link)) {
    SetupFormSet = BYO_FORMSET_INFO_FROM_LINK (Link);
    Status = UiAppInstallFormset(SetupFormSet);	
    Link = GetNextNode (&gByoSetup->MainFormsetList, Link);
  }
  return Status;
}


VOID
RemoveHiiResource ( 
  VOID 
  )
{
  LIST_ENTRY    *Link;
  SETUP_FORMSET_INFO    * SetupFormSet;
  SETUP_DYNAMIC_ITEM    *SetupItem;
  
  DMSG((EFI_D_ERROR, "\n RemoveHiiResource, \n"));
  SetupFormSet = NULL;
  Link = GetFirstNode (&gByoSetup->MainFormsetList);
  while (!IsNull (&gByoSetup->MainFormsetList, Link)) {
    SetupFormSet = BYO_FORMSET_INFO_FROM_LINK (Link);
    Link = GetNextNode (&gByoSetup->MainFormsetList, Link);

    DMSG((EFI_D_ERROR, "RemoveHiiResource, HiiHandle :0x%x, Class :%d. \n", SetupFormSet->HiiHandle, SetupFormSet->FormSetInfo.Class));

    if (SetupFormSet->DriverHandle != NULL) {
      gBS->UninstallMultipleProtocolInterfaces (
               SetupFormSet->DriverHandle,
               &gEfiDevicePathProtocolGuid,
               SetupFormSet->VendorDevicePath,
               &gEfiHiiConfigAccessProtocolGuid,
               &SetupFormSet->SetupConfig,
               NULL
               );
      SetupFormSet->DriverHandle = NULL;
    }

    if (SetupFormSet->VendorDevicePath != NULL) {
      FreePool (SetupFormSet->VendorDevicePath);
      SetupFormSet->VendorDevicePath = NULL;
    }

    if (SetupFormSet->HiiHandle != NULL) {
      HiiRemovePackages (SetupFormSet->HiiHandle);
      SetupFormSet->HiiHandle = NULL;
    }

    RemoveEntryList (&SetupFormSet->Link);
    FreePool (SetupFormSet);
  }


  DMSG((EFI_D_ERROR, "RemoveHiiResource, Remove Dynamic Item, \n"));
  SetupItem = NULL;
  Link = GetFirstNode (&gByoSetup->DynamicItemList);
  while (!IsNull (&gByoSetup->DynamicItemList, Link)) {
    SetupItem = BYO_SETUP_ITEM_FROM_LINK (Link);
    Link = GetNextNode (&gByoSetup->DynamicItemList, Link);

    RemoveEntryList (&SetupItem->Link);
    FreePool (SetupItem);
  }

  DMSG((EFI_D_ERROR, "RemoveHiiResource, End. \n"));
  return;
}

