/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BootCallBack.c

Abstract:
  Exit Setup Rountines

Revision History:

$END--------------------------------------------------------------------

**/


#include <PlatformSetupDxe.h>
#include <Library/SetupUiLib.h>
#include <Library/ByoCommLib.h>
#include <Protocol/SetupItemUpdateNotify.h>



EFI_STATUS NotifySetupItemChanged(UINTN Item, UINTN NewStatus)
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
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      QuestionId,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
  EFI_STATUS   Status;


  DMSG((EFI_D_INFO, "BMCB A:%X Q:%X\n", Action, QuestionId));

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN || Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    Status = EFI_SUCCESS;
    goto ProcExit;
  }

  if (Value == NULL || ActionRequest == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  if (QuestionId == KEY_VALUE_BOOTTYPE_CONFIGURATION) {
    if(Action == EFI_BROWSER_ACTION_CHANGED){
      DMSG((EFI_D_INFO, "BootModeType-> %d\n", Value->u8));
      if (Value->u8 != BIOS_BOOT_UEFI_OS) {
        NotifySetupItemChanged(ITEM_CSM, STATUS_ENABLE);
      }
    }else if(Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD){
      return EFI_UNSUPPORTED;
    }
    return EFI_SUCCESS;
    
  } else if(QuestionId == KEY_VALUE_CSM_CONFIGURATION){
    if(Action == EFI_BROWSER_ACTION_CHANGED){
      DMSG((EFI_D_INFO, "Csm -> %d\n", Value->u8));
      if (Value->u8) {
        NotifySetupItemChanged(ITEM_CSM, STATUS_ENABLE);
      }
    }else if(Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD){
      return EFI_UNSUPPORTED;
    }
    return EFI_SUCCESS;
  }

  Status = EFI_UNSUPPORTED;

ProcExit:
  return Status;
}



