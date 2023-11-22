/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformBootManagerDxe.c

Abstract:
  The platform installs gByoPlatformBootManagerProtocolGuid.

Revision History:

**/

#include "PlatformBootManagerDxe.h"
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>                                  
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>                                                                                    
#include <Library/UefiLib.h>
#include <Protocol/ByoPlatformBootManager.h>
#include <Library/MemoryAllocationLib.h>

#include <SysMiscCfg.h>

BYO_PLATFORM_BOOT_MANAGER_PROTOCOL  mPlatformBootManager;

EFI_STATUS
EFIAPI
PlatformRefreshAllBootOptions2 (
  IN  CONST EFI_BOOT_MANAGER_LOAD_OPTION *BootOptions,
  IN  CONST UINTN                        BootOptionsCount,
  OUT       EFI_BOOT_MANAGER_LOAD_OPTION **UpdatedBootOptions,
  OUT       UINTN                        *UpdatedBootOptionsCount
  )
{
  EFI_STATUS                        Status;
  UINTN                             Index;

  *UpdatedBootOptions = (EFI_BOOT_MANAGER_LOAD_OPTION *)AllocateZeroPool(sizeof(EFI_BOOT_MANAGER_LOAD_OPTION) * BootOptionsCount);
  ASSERT(UpdatedBootOptions != NULL);

  *UpdatedBootOptionsCount = BootOptionsCount;
  for (Index = 0; Index < BootOptionsCount; Index++) {
    Status = DuplicateBootOption(*UpdatedBootOptions + Index, BootOptions + Index);
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_INFO, "[Bds] Failed to refresh boot options!\n"));
      ByoEfiBootManagerFreeLoadOptions (*UpdatedBootOptions, BootOptionsCount);
      return Status;
    }
  }

  //
  // Update the Attribute of options according to L"BootOptionDisable"
  //
  InactiveDisabledOptions(*UpdatedBootOptions, *UpdatedBootOptionsCount);

  return Status;
}

EFI_STATUS
EFIAPI
PlatformBootManagerDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS          Status;

  if (PcdGet8(PcdBiosBootModeType) != BIOS_BOOT_UEFI_OS) {
    mPlatformBootManager.RefreshAllBootOptions = PlatformRefreshAllBootOptions2;
  }

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gByoPlatformBootManagerProtocolGuid,
                  &mPlatformBootManager,
                  NULL
                  );

  return Status;
}