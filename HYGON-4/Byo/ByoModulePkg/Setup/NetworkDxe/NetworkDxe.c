/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  NetworkDxe.c

Abstract:

Revision History:

**/
#include <PiDxe.h>
#include <Guid/PxeControlVariable.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include <SysMiscCfg.h>
#include "NetworkDxe.h"



extern unsigned char NetworkConfigBin[];

HII_VENDOR_DEVICE_PATH gNetworkConfigIdHiiVendorDevicePath = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
          (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
        }
      },
      SETUP_NETWORK_CONFIGURATION_FORMSET_GUID,
    },
    0,
    (UINTN)&gNetworkConfigIdHiiVendorDevicePath
  },
  
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { 
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }  
};

EFI_STATUS
EFIAPI
NetworkDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  UINTN                       Size;
  EFI_STATUS                  Status;
  PXE_CONTROL_VARIABLE        PxeControl;
  UINT8                       NetControlType = 0;
  EFI_HII_HANDLE              NetworkConfigHiiHandle;
  EFI_HANDLE                  DeviceHandle = NULL;

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DeviceHandle,
                  &gEfiDevicePathProtocolGuid,
                  &gNetworkConfigIdHiiVendorDevicePath,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  NetworkConfigHiiHandle = HiiAddPackages (
                 &gByoSetupNetworkFormsetGuid,
                 DeviceHandle,
                 STRING_ARRAY_NAME,
                 NetworkConfigBin,
                 NULL
                 );
  ASSERT(NetworkConfigHiiHandle != NULL);

  Size = sizeof(PXE_CONTROL_VARIABLE);
  Status = gRT->GetVariable (
                  PXE_CONTROL_VAR_NAME,
                  &gPxeControlVariableGuid,
                  NULL,
                  &Size,
                  &PxeControl
                  );
  if (!EFI_ERROR (Status) && (PcdGet8(PcdBiosBootModeType) != BIOS_BOOT_LEGACY_OS)) {
    NetControlType |= ENABLE_SPEPARATE_CONTROL_NET_TYPE;
    if (PxeControl.UefiNetworkStack) {
      if (PxeControl.Ipv4Pxe) {
        NetControlType |= ENABLE_PXE_IPV4;
      }
      if (PxeControl.Ipv6Pxe) {
        NetControlType |= ENABLE_PXE_IPV6;
      }
      if (PxeControl.Ipv4Https) {
        NetControlType |= ENABLE_HTTP_IPV4;
      }
      if (PxeControl.Ipv6Https) {
        NetControlType |= ENABLE_HTTP_IPV6;
      }
    }
    PcdSet8S (PcdNetBootIp4Ip6, NetControlType);
  }

  Status = gBS->InstallProtocolInterface (
                &ImageHandle,
                &gByoSetupNetworkFormsetGuid,
                EFI_NATIVE_INTERFACE,
                NULL
                );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}