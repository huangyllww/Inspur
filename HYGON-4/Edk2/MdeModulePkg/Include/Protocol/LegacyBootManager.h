/** @file

  Copyright (c) 2021, Byosoft Corporation. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __LEGACY_BOOT_MANAGER_PROTOCOL_H__
#define __LEGACY_BOOT_MANAGER_PROTOCOL_H__

#include <Library/DevicePathLib.h>

//
// Protocol interface structure
//
typedef struct _EDKII_LEGACY_BOOT_MANAGER_PROTOCOL EDKII_LEGACY_BOOT_MANAGER_PROTOCOL;

//
// Revision The revision to which the protocol interface adheres.
//          All future revisions must be backwards compatible.
//          If a future version is not back wards compatible it is not the same GUID.
//
#define EDKII_LEGACY_BOOT_MANAGER_PROTOCOL_REVISION  0x00000002

//
// Function Prototypes
//

/**
  The function boots a legacy boot option.
**/
typedef
VOID
(EFIAPI *EDKII_BOOT_MANAGER_LEGACY_BOOT)(
  IN  VOID  *BootOption
  );

/**
  The function enumerates all the legacy boot options, creates them and
  registers them in the BootOrder variable.
**/
typedef
VOID
(EFIAPI *EDKII_BOOT_MANAGER_REFRESH_LEGACY_BOOT_OPTION)(
  VOID
  );

/*
  This function is used to get Legacy Boot Group Type of boot options
*/
typedef
UINT8
(EFIAPI *GET_LEGACY_GROUP_TYPE_OF_OPTION)(
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath,
      UINT8                     *OptionalData
  );

struct _EDKII_LEGACY_BOOT_MANAGER_PROTOCOL {
  UINT64                                           Revision;
  EDKII_BOOT_MANAGER_REFRESH_LEGACY_BOOT_OPTION    RefreshLegacyBootOption;
  EDKII_BOOT_MANAGER_LEGACY_BOOT                   LegacyBoot;
  GET_LEGACY_GROUP_TYPE_OF_OPTION                  GetLegacyBootGroupType;
};

extern EFI_GUID  gEdkiiLegacyBootManagerProtocolGuid;

#endif /* __LEGACY_BOOT_MANAGER_PROTOCOL_H__ */
