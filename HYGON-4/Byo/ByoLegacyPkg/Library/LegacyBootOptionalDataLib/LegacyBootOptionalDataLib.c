/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>

#pragma pack(1)
typedef struct {
  UINT32     Tag;
  UINT16     BbsIndex;
  UINT8      PciClass;
  UINT8      PciSubClass;
} LEGACY_BOOT_OPTION_BBS_DATA;
#pragma pack()

#pragma pack(1)
typedef struct {
  UINT32     Reserve[2];
} LEGACY_OPTION_DATA_RESERVE;
#pragma pack()

#define LEGACY_BOOT_OPTION_BBS_DATA_TAG    SIGNATURE_32('B', 'Y', 'L', 'B')



UINT32 LegacyBootOptionalDataGetSize()
{
  return sizeof(LEGACY_BOOT_OPTION_BBS_DATA);
}

VOID LegacyBootOptionalDataSetBbsIndex(VOID *OptionalData, UINT16 Index)
{
  LEGACY_BOOT_OPTION_BBS_DATA  *LegacyData;
  
  ASSERT(OptionalData != NULL);
  
  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA*)OptionalData;
  LegacyData->BbsIndex = Index;
  LegacyData->Tag = LEGACY_BOOT_OPTION_BBS_DATA_TAG;
}

UINT16 LegacyBootOptionalDataGetBbsIndex(VOID *OptionalData)
{
  LEGACY_BOOT_OPTION_BBS_DATA  *LegacyData;
  
  ASSERT(OptionalData != NULL);
  
  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA*)OptionalData;
  ASSERT(LegacyData->Tag == LEGACY_BOOT_OPTION_BBS_DATA_TAG);
  
  return LegacyData->BbsIndex;  
}


VOID LegacyBootOptionalDataSetPciClass(VOID *OptionalData, UINT8 PciClass, UINT8 PciSubClass)
{
  LEGACY_BOOT_OPTION_BBS_DATA  *LegacyData;
  
  ASSERT(OptionalData != NULL);
  
  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA*)OptionalData;
  LegacyData->PciClass = PciClass;
  LegacyData->PciSubClass = PciSubClass; 
}

UINT8 LegacyBootOptionalDataGetPciClass(VOID *OptionalData, UINT8 *PciSubClass OPTIONAL)
{
  LEGACY_BOOT_OPTION_BBS_DATA  *LegacyData;
  
  ASSERT(OptionalData != NULL);
  
  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA*)OptionalData;
  ASSERT(LegacyData->Tag == LEGACY_BOOT_OPTION_BBS_DATA_TAG);

  if(PciSubClass != NULL){
    *PciSubClass = LegacyData->PciSubClass;
  }
  
  return LegacyData->PciClass; 
}

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
  )
{
  LEGACY_BOOT_OPTION_BBS_DATA  **LegacyData;
  UINT32                       Size;

  ASSERT(OptionalData != NULL);

  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA **)OptionalData;
  ASSERT((*LegacyData)->Tag == LEGACY_BOOT_OPTION_BBS_DATA_TAG);
  if (DevicePath == NULL) {
    return;
  }

  //
  // When set DevicePath, we must reserve 8 bytes(LEGACY_OPTION_DATA_RESERVE) in front of DevicePath.
  //
  Size = (UINT32)GetDevicePathSize(DevicePath) + sizeof(LEGACY_OPTION_DATA_RESERVE);
  *LegacyData = ReallocatePool(*OptionDataSize, *OptionDataSize + Size, *LegacyData);
  ASSERT(*LegacyData != NULL);

  CopyMem((UINT8 *)*LegacyData + *OptionDataSize + sizeof(LEGACY_OPTION_DATA_RESERVE),
          DevicePath, GetDevicePathSize(DevicePath));
  *OptionDataSize = *OptionDataSize + Size;
}

/**
  Get UefiDevicePath from OptionData of legacy option.

  @param  OptionalData  OptionData of LoadOption.

  @return Pointer to device path constructed from OptionData of Legacy option
 **/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
LegacyBootOptionalDataGetUefiDevicePath (
  IN VOID                    *OptionalData,
  UINT32                     OptionDataSize
  )
{
  LEGACY_BOOT_OPTION_BBS_DATA  *LegacyData;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;
  UINT32                       Size;

  ASSERT(OptionalData != NULL);

  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA*)OptionalData;
  ASSERT(LegacyData->Tag == LEGACY_BOOT_OPTION_BBS_DATA_TAG);

  Size = OptionDataSize - sizeof(LEGACY_BOOT_OPTION_BBS_DATA);
  if (Size == 0) {
    DEBUG((EFI_D_INFO, "No DevicePath appended on this Legacy option %a\n", __FUNCTION__));
    return NULL;
  }

  DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)AllocateCopyPool(Size - sizeof(LEGACY_OPTION_DATA_RESERVE),
    (UINT8 *)OptionalData + sizeof(LEGACY_BOOT_OPTION_BBS_DATA) + sizeof(LEGACY_OPTION_DATA_RESERVE));
  if (DevicePath == NULL) {
    return NULL;
  }

  return DevicePath;
}
