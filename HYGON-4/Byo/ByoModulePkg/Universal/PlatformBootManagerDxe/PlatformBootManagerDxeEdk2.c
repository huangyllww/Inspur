/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformBootManagerDxeEdk2.c

Abstract:
  The platform installs gEdkiiPlatformBootManagerProtocolGuid.

Revision History:

**/

#include "PlatformBootManagerDxe.h"
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>

#include <SysMiscCfg.h>

EDKII_PLATFORM_BOOT_MANAGER_PROTOCOL mEdkiiPlatformBootManager;

EFI_STATUS
EFIAPI
EdkiiPlatformBootManagerDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS          Status;

  mEdkiiPlatformBootManager.BootPolicy = DefaultBootPolicy;
  mEdkiiPlatformBootManager.RefreshAllBootOptions = PlatformRefreshAllBootOptions;
  mEdkiiPlatformBootManager.GetUefiBootGroupType = (GET_UEFI_GROUP_TYPE_OF_OPTION)GetEfiBootGroupType;
  mEdkiiPlatformBootManager.Revision = EDKII_PLATFORM_BOOT_MANAGER_PROTOCOL_REVISION;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEdkiiPlatformBootManagerProtocolGuid,&mEdkiiPlatformBootManager,
                  NULL
                  );

  return Status;
}