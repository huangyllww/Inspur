/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __LEGACY_BOOT_OPTIONAL_DATA_LIB_H__
#define __LEGACY_BOOT_OPTIONAL_DATA_LIB_H__

#include <Base.h>

UINT32 LegacyBootOptionalDataGetSize();
VOID   LegacyBootOptionalDataSetBbsIndex(VOID *OptionalData, UINT16 Index);
UINT16 LegacyBootOptionalDataGetBbsIndex(VOID *OptionalData);

VOID LegacyBootOptionalDataSetPciClass(VOID *OptionalData, UINT8 PciClass, UINT8 PciSubClass);
UINT8 LegacyBootOptionalDataGetPciClass(VOID *OptionalData, UINT8 *PciSubClass OPTIONAL);

/**
  Set UefiDevicePath to OptionData of legacy option.

  @param  OptionalData  OptionData of LoadOption;
  @param  OptionDataSize  Size of OptionData;
  @param  DevicePath    The real DevicePath of Legacy devices rather than BBS_DEVICE_PATH
 **/
VOID
EFIAPI
LegacyBootOptionalDataSetUefiDevicePath (
  IN OUT VOID                    **OptionalData,
  IN OUT UINT32                  *OptionDataSize,
  CONST EFI_DEVICE_PATH_PROTOCOL *DevicePath
  );

/**
  Get UefiDevicePath from OptionData of legacy option.

  @param  OptionalData    OptionData of LoadOption.
  @param  OptionDataSize  Size of OptionData;

  @return Pointer to device path constructed from OptionData of Legacy option
 **/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
LegacyBootOptionalDataGetUefiDevicePath (
  IN VOID                    *OptionalData,
  UINT32                     OptionDataSize
  );
#endif
