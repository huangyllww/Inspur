/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  FrontPage.h
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/
#ifndef _FRONT_PAGE_H_
#define _FRONT_PAGE_H_

#include <PiDxe.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/GlobalVariable.h>
#include <Guid/LegacyDevorder.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PcdLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/ByoUefiBootManagerLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SetupSaveNotify.h>
#include <Protocol/ByoFormSetManager.h>
#include <Protocol/DevicePath.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/ByoFormBrowserEx.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>
#include <ByoPlatformSetupConfig.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Library/LegacyBootOptionalDataLib.h>
#include "ChangeLanguage.h"
#include "DirectBoot.h"
#include <Library/ByoCommLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Token.h>

extern EFI_BYO_PLATFORM_SETUP_PROTOCOL   *gByoSetup;
extern EFI_HII_CONFIG_ACCESS_PROTOCOL    gSetupFormSetConfig;
  
CHAR16 *
UiDevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  );  

EFI_STATUS
BmInit (
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBm
  );

VOID
ByoFreeBMPackage (
  VOID
  );
  
#endif // _FRONT_PAGE_H_

