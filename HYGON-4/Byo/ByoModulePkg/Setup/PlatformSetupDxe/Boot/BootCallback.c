/*++

Copyright (c) 2010 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:
  Platform configuration setup.

Revision History:


--*/

#include "PlatformSetupDxe.h"

EFI_STATUS
NotifySetupItemChanged (
  UINTN Item,
  UINTN NewStatus
  )
{
  UINTN                                 HandleCount;
  EFI_HANDLE                            *Handles = NULL;
  EFI_STATUS                            Status = EFI_SUCCESS;
  UINTN                                 Index;
  SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL     *ItemNotify;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gSetupItemUpdateNotifyProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gSetupItemUpdateNotifyProtocolGuid,
                    (VOID**)&ItemNotify
                    );
    if(EFI_ERROR(Status)){
      continue;
    }

    Status = ItemNotify->Notify(ItemNotify, Item, NewStatus);
  }

  if(Handles != NULL){gBS->FreePool(Handles);}
  return Status;
}

EFI_STATUS
EFIAPI
BootFormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        KeyValue,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  BYO_PLATFOMR_SETUP    ByoPlatformSetup;

  if ((Action != EFI_BROWSER_ACTION_CHANGED) || (KeyValue != KEY_VALUE_CSM_CONFIGURATION)) {
    return EFI_UNSUPPORTED;
  }

  DEBUG((EFI_D_INFO, "CSM -> %d %a line=%d\n", Value->u8, __FUNCTION__, __LINE__));
  HiiGetBrowserData(
      &gPlatformSetupVariableGuid,
      EFI_BYO_SETUP_VARIABLE_NAME,
      sizeof(BYO_PLATFOMR_SETUP),
      (UINT8*)&ByoPlatformSetup
      );
  if (Value->u8) {
    ByoPlatformSetup.Csm = 1;
    NotifySetupItemChanged(ITEM_CSM, STATUS_ENABLE);
  } else {
    ByoPlatformSetup.Csm = 0;
    ByoPlatformSetup.BootModeType = BIOS_BOOT_UEFI_OS;
    NotifySetupItemChanged(ITEM_CSM, STATUS_DISABLE);
  }
  HiiSetBrowserData(
      &gPlatformSetupVariableGuid,
      EFI_BYO_SETUP_VARIABLE_NAME,
      sizeof(BYO_PLATFOMR_SETUP), 
      (UINT8*)&ByoPlatformSetup,
      NULL
      );

  return EFI_SUCCESS;
}


