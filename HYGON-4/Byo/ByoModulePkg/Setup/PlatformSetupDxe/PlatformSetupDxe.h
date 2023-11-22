/*++

Copyright (c) 2010 - 2015, Byosoft Corporation.<BR>
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

#ifndef _PLAT_OVER_MNGR_H_
#define _PLAT_OVER_MNGR_H_

#include <PiDxe.h>
#include <SysMiscCfg.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/FormBrowserEx2.h>
#include <Protocol/DisplayProtocol.h>
#include <Protocol/ByoFormSetManager.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Protocol/ByoFormBrowserEx.h>
#include <Protocol/ByoPlatformLanguageProtocol.h>
#include <Protocol/SetupItemUpdateNotify.h>
#include <Guid/MdeModuleHii.h>
#include <Library/BaseLib.h>
#include <Library/HiiLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ByoCommLib.h>
#include <Library/PlatformLanguageLib.h>
#include "FormsetConfiguration.h"
#include "SetupFormInit.h"

//
//These three GUIDs are only used internally within the module to
//meet the requirements of Inspur
//
#define FORMSET_GUID_PLATFORM \
  { \
    0x6f624901, 0x99c5, 0x863f, { 0x78, 0xee, 0xab, 0x72, 0xa3, 0x6c, 0x46, 0xf8 } \
  }
#define SOCKET_FORMSET_GUID \
  { \
    0x516d5a04, 0xc0d5, 0x4657, { 0xb9, 0x8, 0xe4, 0xfb, 0x1d, 0x93, 0x5e, 0xf0 } \
  }
#define SETUP_BMC_CFG_GUID \
  { \
    0x8236697e, 0xf0f6, 0x405f, { 0x99, 0x13, 0xac, 0xbc, 0x50, 0xaa, 0x45, 0xd1 } \
  }

//
// uni string and Vfr Binary data.
//
extern UINT8  PlatformSetupDxeStrings[];
extern UINT8  FormsetAdvancedBin[];
extern UINT8  FormsetSecurityBin[];
extern UINT8  FormsetExitBin[];
extern UINT8  FormsetBootBin[];


/**
  Install Byo Setup Protocol interface.

**/
EFI_STATUS
InstallByoSetupProtocol (
  VOID
  );

extern EFI_BYO_PLATFORM_SETUP_PROTOCOL    *mByoSetup;

EFI_STATUS
EFIAPI
HiiExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *This,
  IN  CONST EFI_STRING    Request,
  OUT EFI_STRING    *Progress,
  OUT EFI_STRING    *Results
  );


EFI_STATUS
EFIAPI
HiiRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *This,
  IN  CONST EFI_STRING    Configuration,
  OUT EFI_STRING    *Progress
  );

EFI_STATUS
EFIAPI
FormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL    *This,
  IN EFI_BROWSER_ACTION    Action,
  IN EFI_QUESTION_ID    KeyValue,
  IN UINT8    Type,
  IN EFI_IFR_TYPE_VALUE    *Value,
  OUT EFI_BROWSER_ACTION_REQUEST    *ActionRequest
);

EFI_STATUS
InitializeForm (
  VOID 
  );

#endif
