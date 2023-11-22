/** @file
  Extension Form Browser Protocol provides the services that can be used to
  register the different hot keys for the standard Browser actions described in UEFI specification.

  Copyright (c) 2011 - 2022, Byosoft Corporation. All rights reserved.<BR>

  This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.


**/

#ifndef __BYO_FORM_BROWSER_EXTENSION_H__
#define __BYO_FORM_BROWSER_EXTENSION_H__

#include <Protocol/FormBrowserEx.h>
#include <Protocol/SetupSaveNotify.h>

#define BYO_FORM_BROWSER_EXTENSION_PROTOCOL_GUID  \
  { 0xcf49d6f3, 0x35bb, 0x488c, { 0x88, 0xab, 0x47, 0x0, 0x92, 0x2f, 0x3a, 0xc4 } }

typedef struct _EFI_BYO_FORM_BROWSER_EXTENSION_PROTOCOL   EFI_BYO_FORM_BROWSER_EXTENSION_PROTOCOL;

//
// Define Byo's Browser actions Which should begin from BIT10 to up,
// and Exclude BIT16 and BIT17.
//#define BROWSER_ACTION_NONE         BIT16
//#define BROWSER_ACTION_FORM_EXIT    BIT17
// 
#define BROWSER_ACTION_GOTO_BYO           BIT10
#define BROWSER_ACTION_SAVE_USER_DEFAULT  BIT13
#define BROWSER_ACTION_LOAD_USER_DEFAULT  BIT14
#define BROWSER_ACTION_REFRESH            BIT15
#define BROWSER_ACTION_DISPALY_HELP       BIT18
#define BROWSER_ACTION_EXIT_NO_RESET      BIT19

//
// Support Action for +/- X Key
//
#define BROWSER_ACTION_X_MOVE             BIT20
#define BROWSER_ACTION_MOVE_UP            BIT21
#define BROWSER_ACTION_MOVE_DOWN          BIT22

/**
  Check whether the browser data has been modified.

  @retval TRUE        Browser data is modified.
  @retval FALSE       No browser data is modified.

**/
typedef
BOOLEAN
(EFIAPI *IS_BROWSER_DATA_MODIFIED) (
  VOID
  );


/**
  Execute the action requested by the Action parameter.

  @param[in] Action     Execute the request action.
  @param[in] DefaultId  The default Id info when need to load default value.

  @retval EFI_SUCCESS              Execute the request action success.

**/
typedef
EFI_STATUS
(EFIAPI *EXECUTE_ACTION) (
  IN UINT32        Action,
  IN UINT16        DefaultId
  );

/**
  Oem Platform reset function.

**/
typedef
VOID
(EFIAPI *OEM_PLATFORM_RESET) (
  VOID
  );

/**
  Trigger notify protocol function.

  @param  SETUP_SAVE_NOTIFY_TYPE Notify type.

  @retval EFI_STATUS return status from the notify function.

**/
typedef
EFI_STATUS
(EFIAPI *SETUP_SAVE_NOTIFY) (
  IN SETUP_SAVE_NOTIFY_TYPE Type 
  );

/**
  Retrieve setup option value and attribute from Browser.

  @param  InKeyValueArray    Input setting to be applied.
  @param  OutKeyValueArray   Current setting from all installed HII packages.
  @param  OutAttributesArray Current setting attribute from all installed HII packages.

  @retval EFI_SUCCESS        Retrieve setup value and attribute from Browser
  @retval EFI_UNSUPPORTED    Don't support this service.

**/
typedef
EFI_STATUS
(EFIAPI *RETERIEVE_SETUP_OPTIONS) (
  IN  VOID *InKeyValueArray,
  OUT VOID **OutKeyValueArray,
  OUT VOID **OutAttributesArray
  );

struct _EFI_BYO_FORM_BROWSER_EXTENSION_PROTOCOL {
  IS_BROWSER_DATA_MODIFIED  IsBrowserDataModified;
  EXECUTE_ACTION            ExecuteAction;
  OEM_PLATFORM_RESET        PlatformReset;
  SETUP_SAVE_NOTIFY         SaveNotify;
  RETERIEVE_SETUP_OPTIONS   ReterieveSetupOptions;
  RETERIEVE_SETUP_OPTIONS   ReterieveSetupOptionsName;
};

extern EFI_GUID gEfiByoFormBrowserExProtocolGuid;



#endif

