/*++

Copyright (c) 2023, Byosoft Corporation.<BR>
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
#include <PlatformSetupDxe.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/FormBrowser2.h>
#include <Uefi/UefiInternalFormRepresentation.h>
#include <Library/UsbListLib.h>
#include <PlatformSetupDxe.h>


/**
  Device Formset callback.

**/

EFI_STATUS
EFIAPI
UsbListCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{


  if(Action == EFI_BROWSER_ACTION_FORM_OPEN){
    if(KeyValue == KEY_USB_DEV_LIST){
      UpdateUsbList (This, NULL, CHIPSET_SB_USB_FORM_ID);
      return EFI_SUCCESS;
    }
  }

  return EFI_UNSUPPORTED;
}

