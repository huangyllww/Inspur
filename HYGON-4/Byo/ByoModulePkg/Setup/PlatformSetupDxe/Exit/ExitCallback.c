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
EFIAPI
ExitFormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        KeyValue,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                             Status;
  EDKII_FORM_DISPLAY_ENGINE_PROTOCOL     *FormDisplay;
  EDKII_FORM_BROWSER_EXTENSION2_PROTOCOL *BrowserExtension;
  BOOLEAN                                UpdateNvVariableEnableValue;


  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    return EFI_UNSUPPORTED;
  }

  Status = gBS->LocateProtocol (
                  &gEdkiiFormDisplayEngineProtocolGuid,
                  NULL,
                  (void **) &FormDisplay
                  );
  if ( EFI_ERROR(Status) ) {
    return EFI_SUCCESS;
  }
  
  Status = gBS->LocateProtocol (
                  &gEdkiiFormBrowserEx2ProtocolGuid,
                  NULL,
                  (void**)&BrowserExtension
                  );
  if (!EFI_ERROR(Status)) {
    switch (KeyValue) {
      case EXIT_KEY_SAVE_EXIT_AND_RESET:
        Status = BrowserExtension->ExecuteAction(BROWSER_ACTION_SUBMIT | BROWSER_ACTION_EXIT, 0);  
        break;

      case EXIT_KEY_SAVE:	  	
        Status = BrowserExtension->ExecuteAction(BROWSER_ACTION_SUBMIT, 0);  
        break;

      case EXIT_KEY_DISCARD_EXIT_AND_RESET:
        Status = BrowserExtension->ExecuteAction(BROWSER_ACTION_DISCARD | BROWSER_ACTION_EXIT, 0);
        break;

      case EXIT_KEY_SAVE_EXIT:
        Status = BrowserExtension->ExecuteAction(BROWSER_ACTION_SUBMIT | BROWSER_ACTION_EXIT_NO_RESET, 0);
        break;

      case EXIT_KEY_DISCARD:
        Status = BrowserExtension->ExecuteAction(BROWSER_ACTION_DISCARD | BROWSER_ACTION_EXIT_NO_RESET, 0);
        break;

      case EXIT_KEY_DISCARD_MODIFY:
        Status = BrowserExtension->ExecuteAction (BROWSER_ACTION_DISCARD, 0);
        break;

      case EXIT_KEY_DEFAULT:      
        Status = BrowserExtension->ExecuteAction(BROWSER_ACTION_DEFAULT, 0);
        break;

      case KEY_BIOS_UPDATE:      
        InvokeHookProtocol(gBS, &gByoSetupBiosUpdateProtocolGuid);
        break;

      case KEY_BIOS_UPDATE_ALL:
        UpdateNvVariableEnableValue = PcdGetBool (PcdUpdateNvVariableEnable);
        PcdSetBoolS (PcdUpdateNvVariableEnable, TRUE);
        InvokeHookProtocol(gBS, &gByoSetupBiosUpdateAllProtocolGuid);
        PcdSetBoolS (PcdUpdateNvVariableEnable, UpdateNvVariableEnableValue);
        break;

      case KEY_BIOS_UPDATE_NETWORK:
        InvokeHookProtocol(gBS,&gByoSetupBiosUpdateNetworkProtocolGuid);
        break;

      case KEY_BIOS_UPDATE_NETWORK_ALL:
        UpdateNvVariableEnableValue = PcdGetBool (PcdUpdateNvVariableEnable);
        PcdSetBoolS (PcdUpdateNvVariableEnable, TRUE);
        InvokeHookProtocol(gBS,&gByoSetupBiosUpdateAllNetworkProtocolGuid);
        PcdSetBoolS (PcdUpdateNvVariableEnable, UpdateNvVariableEnableValue);
        break;
      case KEY_SAVE_USER_DEFAULTS_VALUE:
        Status = BrowserExtension->ExecuteAction(BROWSER_ACTION_SAVE_USER_DEFAULT, 0);
        break;
    
      case KEY_RESTORE_USER_DEFAULTS_VALUE:
        Status = BrowserExtension->ExecuteAction(BROWSER_ACTION_LOAD_USER_DEFAULT, 0);
        break;
      default:
        break;
    }
  }

  return Status;
}


