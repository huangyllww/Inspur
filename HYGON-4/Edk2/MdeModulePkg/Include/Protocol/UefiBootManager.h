/** @file

  Copyright (c) 2021, Byosoft Corporation. All rights reserved.

  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __UEFI_BOOT_MANAGER_PROTOCOL_H__
#define __UEFI_BOOT_MANAGER_PROTOCOL_H__

//
// Protocol interface structure
//
typedef struct _EDKII_UEFI_BOOT_MANAGER_PROTOCOL EDKII_UEFI_BOOT_MANAGER_PROTOCOL;

//
// Revision The revision to which the protocol interface adheres.
//          All future revisions must be backwards compatible.
//          If a future version is not back wards compatible it is not the same GUID.
//
#define EDKII_UEFI_BOOT_MANAGER_PROTOCOL_REVISION  0x00000001

/**
  The function boots a uefi boot option.
**/
typedef
VOID
(EFIAPI *EDKII_BOOT_MANAGER_UEFI_BOOT)(
  IN  VOID  *BootOption
  );

struct _EDKII_UEFI_BOOT_MANAGER_PROTOCOL {
  UINT64                          Revision;
  EDKII_BOOT_MANAGER_UEFI_BOOT    UefiBoot;
};

extern EFI_GUID  gEdkiiUefiBootManagerProtocolGuid;

#endif /* __UEFI_BOOT_MANAGER_PROTOCOL_H__ */
