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

#ifndef _BYO_UI_H_
#define _BYO_UI_H_

#include <IndustryStandard/SmBios.h>

#include <Guid/MdeModuleHii.h>
#include <Guid/StatusCodeDataTypeId.h>

#include <Protocol/Smbios.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/LoadedImage.h>

#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiBootManagerLib.h>

#include <Protocol/BootLogo.h>
#include <Protocol/ByoFormSetManager.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Protocol/ByoFormBrowserEx.h>
#include <ByoPlatformSetupConfig.h>
#include "ChangeLanguage.h"


/**
  This function is the main entry of the UI entry.
  The function will present the main menu of the system UI.

  @param ConnectAllHappened Caller passes the value to UI to avoid unnecessary connect-all.

**/
VOID
EFIAPI
UiEntry (
  IN BOOLEAN                      ConnectAllHappened
  );

EFI_STATUS
AddAllFormset (
  VOID 
  );

VOID
RemoveHiiResource ( 
  VOID 
  );

EFI_STATUS
AddDynamicFormset (
  EFI_HII_HANDLE    HiiHandle,
  EFI_GUID          *FormSetGuid,
  EFI_GUID          *ClassGuid,
  EFI_FORM_ID       FormId,
  UINT16            Label
  );


extern EFI_BYO_PLATFORM_SETUP_PROTOCOL    *gByoSetup;
extern EFI_HII_CONFIG_ACCESS_PROTOCOL    gSetupFormSetConfig;


BOOLEAN
ExtractFormsetInfoFromHiiHandle (
  IN    EFI_HII_HANDLE    Handle,
  IN    EFI_GUID    *SetupClassGuid,
  OUT    EFI_STRING_ID    *FormSetTitle,
  OUT    EFI_STRING_ID    *FormSetHelp,
  OUT    EFI_GUID    **FormSetGuid,
  OUT    UINT16    *Class
  );

EFI_STATUS
AddBootOrderFormset (
  EFI_HII_HANDLE    HiiHandle,
  EFI_GUID    *FormSetGuid,
  EFI_FORM_ID     FormId
  );

EFI_STATUS
UpdateDynamicPages (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  SETUP_DYNAMIC_ITEM                     *SetupItem,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

/**
  Extract device path for given HII handle and class guid.

  @param Handle          The HII handle.

  @retval  NULL          Fail to get the device path string.
  @return  PathString    Get the device path string.

**/
CHAR16 *
ByoUiAppExtractDevicePathFromHiiHandle (
  IN      EFI_HII_HANDLE      Handle
  );
#endif

