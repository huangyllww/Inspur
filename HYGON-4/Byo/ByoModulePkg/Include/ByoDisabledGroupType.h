/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_DISABLED_GROUP_TYPE_H__
#define __BYO_DISABLED_GROUP_TYPE_H__

#define BYO_DISABLED_GROUP_TYPE_VAR_NAME     L"DisabledGroupType"

#define DISABLED_GROUP_TYPE_GUID \
  { \
    0xccdbc411, 0x3b37, 0x4a26, { 0x98, 0x87, 0xd6, 0xf7, 0x78, 0x15, 0x4b, 0xd6 } \
  }

#define BYO_DISABLED_GROUP_TYPE_VAR_ATTR   (EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS)

//
// Every memory marks disabled GroupType(Hdd, ODD, USB_DISK, USB_ODD, PXE)
//
typedef struct {
  UINT8  DisableAll; // only used in PlatformBootMaintUiDxe: not including others.
  //
  // UEFI GroupType control
  //
  UINT8  DisableHDD;
  UINT8  DisableODD;
  UINT8  DisableUSBDisk;
  UINT8  DisableUSBODD;
  UINT8  DisablePXE;
  UINT8  DisableOthers;
  //
  // Legacy GroupType control
  //
  UINT8  DisableLegacyHDD;
  UINT8  DisableLegacyODD;
  UINT8  DisableLegacyUSBDisk;
  UINT8  DisableLegacyUSBODD;
  UINT8  DisableLegacyPXE;
  UINT8  DisableLegacyOthers;
  //
  // Mark if it's used in PlatformBootMaintUiDxe
  //
  BOOLEAN IsByoCommonMaint;
} DISABLED_GROUP_TYPE;

#define DISABLE_VAR_OFFSET(Field)           ((UINT16) ((UINTN) &(((DISABLED_GROUP_TYPE *) 0)->Field)))

#define DISABLE_ALL_VAR_OFFSET              DISABLE_VAR_OFFSET (DisableAll)
#define DISABLE_HDD_VAR_OFFSET              DISABLE_VAR_OFFSET (DisableHDD)
#define DISABLE_ODD_VAR_OFFSET              DISABLE_VAR_OFFSET (DisableODD)
#define DISABLE_USB_DISK_VAR_OFFSET         DISABLE_VAR_OFFSET (DisableUSBDisk)
#define DISABLE_USB_ODD_VAR_OFFSET          DISABLE_VAR_OFFSET (DisableUSBODD)
#define DISABLE_PXE_VAR_OFFSET              DISABLE_VAR_OFFSET (DisablePXE)
#define DISABLE_OTHERS_VAR_OFFSET           DISABLE_VAR_OFFSET (DisableOthers)

#define DISABLE_LEGACY_HDD_VAR_OFFSET       DISABLE_VAR_OFFSET (DisableLegacyHDD)
#define DISABLE_LEGACY_ODD_VAR_OFFSET       DISABLE_VAR_OFFSET (DisableLegacyODD)
#define DISABLE_LEGACY_USB_DISK_VAR_OFFSET  DISABLE_VAR_OFFSET (DisableLegacyUSBDisk)
#define DISABLE_LEGACY_USB_ODD_VAR_OFFSET   DISABLE_VAR_OFFSET (DisableLegacyUSBODD)
#define DISABLE_LEGACY_PXE_VAR_OFFSET       DISABLE_VAR_OFFSET (DisableLegacyPXE)
#define DISABLE_LEGACY_OTHERS_VAR_OFFSET    DISABLE_VAR_OFFSET (DisableLegacyOthers)

#endif /* __BYO_DISABLED_GROUP_TYPE_H__ */