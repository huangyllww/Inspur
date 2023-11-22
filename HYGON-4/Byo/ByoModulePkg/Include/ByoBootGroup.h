/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_BOOT_GROUP_H__
#define __BYO_BOOT_GROUP_H__

#define BYO_LEGACY_BOOT_GROUP_VAR_NAME     L"LegacyBootGroupOrder"
#define BYO_UEFI_BOOT_GROUP_VAR_NAME       L"UefiBootGroupOrder"
#define BYO_LEGACY_DEFAULT_BOOT_GROUP_VAR_NAME     L"LegacyDefaultBootGroupOrder"
#define BYO_UEFI_DEFAULT_BOOT_GROUP_VAR_NAME      L"UefiDefaultBootGroupOrder"

extern EFI_GUID gByoGlobalVariableGuid;

#define BYO_BG_ORDER_VAR_ATTR   (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

typedef enum {

  BM_MENU_TYPE_UEFI_MIN = 0,
  BM_MENU_TYPE_UEFI_HDD = BM_MENU_TYPE_UEFI_MIN,
  BM_MENU_TYPE_UEFI_ODD,
  BM_MENU_TYPE_UEFI_USB_DISK,
  BM_MENU_TYPE_UEFI_USB_ODD,
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_OTHERS,
  BM_MENU_TYPE_UEFI_MAX,

  BM_MENU_TYPE_LEGACY_MIN = 0x80,
  BM_MENU_TYPE_LEGACY_HDD = BM_MENU_TYPE_LEGACY_MIN,
  BM_MENU_TYPE_LEGACY_ODD,
  BM_MENU_TYPE_LEGACY_USB_DISK,
  BM_MENU_TYPE_LEGACY_USB_ODD,
  BM_MENU_TYPE_LEGACY_PXE,
  BM_MENU_TYPE_LEGACY_OTHERS,
  BM_MENU_TYPE_LEGACY_MAX,

  BM_MENU_TYPE_MAX = 0xFF
} BM_MENU_TYPE;

#endif

