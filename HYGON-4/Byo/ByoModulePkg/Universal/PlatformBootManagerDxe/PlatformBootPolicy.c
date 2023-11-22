/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformBootPolicy.c

Abstract:
  Provides the platform specific actions to execute boot policy.

Revision History:

**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/ByoBootManagerLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>

#include <Protocol/ByoPlatformBootManager.h>
#include <Protocol/LoadedImage.h>

BOOLEAN
IsPxeBootOption (
  IN EFI_DEVICE_PATH_PROTOCOL         *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL     *TempDevicePath;

  if (DevicePath == NULL) {
    return FALSE;
  }

  TempDevicePath = DevicePath;
  while (!IsDevicePathEnd (TempDevicePath) &&
       ((DevicePathType (TempDevicePath) != MESSAGING_DEVICE_PATH) ||
        (DevicePathSubType (TempDevicePath) != MSG_MAC_ADDR_DP))
       ) {
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  if (!IsDevicePathEnd(TempDevicePath)) {
    return TRUE;
  }

  return FALSE;
}

EFI_STATUS
EFIAPI
DefaultBootPolicy (
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  IN OUT UINT32                   *BootPolicyStatus
  )
{
  UINT16                 PxeRetryTimes;
  *BootPolicyStatus = 0x0;

  if ((PcdGet8(PcdRetryPxeBoot) != 0) && !PcdGetBool (PcdIsPxeBootApp)) { // PxeBootApp:set PcdIsPxeBootApp to TRUE,no retrying here.
    PxeRetryTimes = PcdGet16(PcdPxeRetrylimites);
    if (IsPxeBootOption(BootOption->FilePath)) {
      *BootPolicyStatus = *BootPolicyStatus | BOOT_POLICY_PXE_RETRY |BOOT_POLICY_NEED_READY_TO_BOOT |BOOT_POLICY_NEED_POST_LOGIN;
      //
      // Enable BOOT_POLICY_PXE_RETRY_TIMES_ENABLE and set times
      //
      *BootPolicyStatus = *BootPolicyStatus | BOOT_POLICY_PXE_RETRY_TIMES_ENABLE;
      *BootPolicyStatus = *BootPolicyStatus | ((UINT32)PxeRetryTimes << 8);
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}
