/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformBootManagerDxe.h

Abstract:
  The platform installs gByoPlatformBootManagerProtocolGuid.

Revision History:

**/

#ifndef __PLATFORM_BOOT_MANAGER_DXE_H__
#define __PLATFORM_BOOT_MANAGER_DXE_H__

#ifdef BYO_BOOT_MANAGER_LIB
#include <Library/ByoBootManagerLib.h>
#include <Library/LegacyBootOptionalDataLib.h>
#else
#include <Library/UefiBootManagerLib.h>
#include <Protocol/PlatformBootManager.h>
#include <Library/ByoBootManagerLib.h>
#endif

typedef struct {
  CHAR16            *FilePathString;
  CHAR16            *Description;
} UEFI_OS_BOOT_FILE;

EFI_STATUS
EFIAPI
DefaultBootPolicy (
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  IN OUT UINT32                   *BootPolicyStatus
  );  

EFI_STATUS
EFIAPI
PlatformRefreshAllBootOptions (
  IN  CONST EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,
  IN  CONST UINTN                        BootOptionsCount,
  OUT       EFI_BOOT_MANAGER_LOAD_OPTION **UpdatedBootOptions,
  OUT       UINTN                        *UpdatedBootOptionsCount
  );

EFI_STATUS
EFIAPI
DuplicateBootOption (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION  *DstBootOption,
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION  *SrcBootOption
);

VOID
EFIAPI
InactiveDisabledOptions (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions,
  IN  CONST UINTN                        BootOptionsCount
  );
#endif
