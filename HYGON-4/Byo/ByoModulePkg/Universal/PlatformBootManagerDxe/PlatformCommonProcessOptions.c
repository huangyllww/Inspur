/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformCommonProcessOptions.c

Abstract:
  Common functions for processing boot options in UEFI and Legacy mode.

Revision History:

**/

#include "PlatformBootManagerDxe.h"
#include <ByoDisabledGroupType.h>
#include <ByoBootGroup.h>

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/ByoCommLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>

#ifdef BYO_BOOT_MANAGER_LIB
#define DELETE_FLAG  (LoadOptionNumberMax + 1)
#endif

EFI_STATUS
EFIAPI
DuplicateBootOption (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION  *DstBootOption,
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION  *SrcBootOption
  )
{
  EFI_STATUS           Status;
  
  Status = EfiBootManagerInitializeLoadOption(
             DstBootOption,
             SrcBootOption->OptionNumber,
             SrcBootOption->OptionType,
             SrcBootOption->Attributes,
             SrcBootOption->Description,
             SrcBootOption->FilePath,
             SrcBootOption->OptionalData,
             SrcBootOption->OptionalDataSize
             );
  if (!EFI_ERROR(Status)) {
    CopyGuid (&DstBootOption->VendorGuid, &SrcBootOption->VendorGuid);
  }

  return Status;
}

BOOLEAN
IsCurrentTypeDisabled (
  DISABLED_GROUP_TYPE  *DisabledGroupType,
  UINT8                GroupType
  )
{
  UINT16    TypeVarOffset;

  if (!DisabledGroupType->IsByoCommonMaint) { // used in PlatformBootMaintUiDxe
    switch (GroupType) {
      case BM_MENU_TYPE_UEFI_HDD:
      case BM_MENU_TYPE_LEGACY_HDD:
        TypeVarOffset = DISABLE_HDD_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_UEFI_PXE:
      case BM_MENU_TYPE_LEGACY_PXE:
        TypeVarOffset = DISABLE_PXE_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_UEFI_ODD:
      case BM_MENU_TYPE_LEGACY_ODD:
        TypeVarOffset = DISABLE_ODD_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_UEFI_USB_DISK:
      case BM_MENU_TYPE_LEGACY_USB_DISK:
        TypeVarOffset = DISABLE_USB_DISK_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_UEFI_USB_ODD:
      case BM_MENU_TYPE_LEGACY_USB_ODD:
        TypeVarOffset = DISABLE_USB_ODD_VAR_OFFSET;
        break;
      default:
        return FALSE;
        break;
    }
  } else {
    switch (GroupType) {
      case BM_MENU_TYPE_UEFI_HDD:
        TypeVarOffset = DISABLE_HDD_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_LEGACY_HDD:
        TypeVarOffset = DISABLE_LEGACY_HDD_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_UEFI_PXE:
        TypeVarOffset = DISABLE_PXE_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_LEGACY_PXE:
        TypeVarOffset = DISABLE_LEGACY_PXE_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_UEFI_ODD:
        TypeVarOffset = DISABLE_ODD_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_LEGACY_ODD:
        TypeVarOffset = DISABLE_LEGACY_ODD_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_UEFI_USB_DISK:
        TypeVarOffset = DISABLE_USB_DISK_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_LEGACY_USB_DISK:
        TypeVarOffset = DISABLE_LEGACY_USB_DISK_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_UEFI_USB_ODD:
        TypeVarOffset = DISABLE_USB_ODD_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_LEGACY_USB_ODD:
        TypeVarOffset = DISABLE_LEGACY_USB_ODD_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_UEFI_OTHERS:
        TypeVarOffset = DISABLE_OTHERS_VAR_OFFSET;
        break;
      case BM_MENU_TYPE_LEGACY_OTHERS:
        TypeVarOffset = DISABLE_LEGACY_OTHERS_VAR_OFFSET;
        break;
      default:
        return FALSE;
        break;
    }
  }

  return *((UINT8 *)((UINTN)DisabledGroupType + TypeVarOffset));
}

VOID
EFIAPI
InactiveDisabledOptions (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions,
  IN  CONST UINTN                        BootOptionsCount
  )
{
  EFI_STATUS                            Status;
  UINTN                                 Index;
  UINT8                                 GroupType;
  DISABLED_GROUP_TYPE                   DisabledGroupType;
  UINTN                                 Size;
  EFI_BOOT_MANAGER_LOAD_OPTION          *NvOptions;
  UINTN                                 NvOptionsCount;
  BOOLEAN                               OptionActive;
  UINT8                                 AllTypeDisabled;
  UINT8                                 TypeEnabled;

  Size = sizeof(DISABLED_GROUP_TYPE);
  Status = gRT->GetVariable (
                  BYO_DISABLED_GROUP_TYPE_VAR_NAME,
                  &gByoDisableGroupTypeGuid,
                  NULL,
                  &Size,
                  &DisabledGroupType
                  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "No need to inactive options %a\n", __FUNCTION__));
    return;
  }

  //
  // Update NV boot options
  //
  NvOptions = EfiBootManagerGetLoadOptions (&NvOptionsCount, LoadOptionTypeBoot);
  AllTypeDisabled = DisabledGroupType.DisableAll;
  for (Index = 0; Index < NvOptionsCount; Index++) {
    if (NvOptions[Index].OptionNumber == DELETE_FLAG) {
      continue;
    }

    #ifdef BYO_BOOT_MANAGER_LIB
      if ((DevicePathType (NvOptions[Index].FilePath) != BBS_DEVICE_PATH)
        || (DevicePathSubType (NvOptions[Index].FilePath) != BBS_BBS_DP)) {
      continue;
    }
    #endif
    OptionActive = (NvOptions[Index].Attributes & LOAD_OPTION_ACTIVE) != 0? TRUE: FALSE;
    #ifdef BYO_BOOT_MANAGER_LIB
      GroupType = GetBbsGroupType(LegacyBootOptionalDataGetPciClass(NvOptions[Index].OptionalData, NULL),
                                  (UINT8)((BBS_BBS_DEVICE_PATH*)NvOptions[Index].FilePath)->DeviceType);
    #else
      GroupType = (UINT8)GetEfiBootGroupType(NvOptions[Index].FilePath);
    #endif

    //
    // The PlatformBootMaintUiDxe doesn't supporting disabling BM_MENU_TYPE_UEFI_OTHERS and BM_MENU_TYPE_LEGACY_OTHERS
    //
    if (!DisabledGroupType.IsByoCommonMaint) {
      if ((GroupType == BM_MENU_TYPE_UEFI_OTHERS) || (GroupType == BM_MENU_TYPE_LEGACY_OTHERS)) {
        continue;
      }
    } else {
      #ifndef  BYO_BOOT_MANAGER_LIB
        if ((GroupType == BM_MENU_TYPE_UEFI_OTHERS) && (DevicePathType (NvOptions[Index].FilePath) == BBS_DEVICE_PATH)) {
          continue;
        }
      #endif
    }

    TypeEnabled = !IsCurrentTypeDisabled(&DisabledGroupType, GroupType);
    if (AllTypeDisabled) { // disable all GroupType except Others GroupType
        if ((OptionActive != TypeEnabled) || (OptionActive != !AllTypeDisabled)) {
          NvOptions[Index].Attributes &= (~LOAD_OPTION_ACTIVE);
          EfiBootManagerLoadOptionToVariable(&NvOptions[Index]);
        }
    } else {
        if ((OptionActive != TypeEnabled) && !TypeEnabled) { // disable current GroupType not all GroupType
          NvOptions[Index].Attributes &= (~LOAD_OPTION_ACTIVE);
          EfiBootManagerLoadOptionToVariable(&NvOptions[Index]);
        } else if ((OptionActive != TypeEnabled) && TypeEnabled) { // enable current GroupType here only in PlatformBootMaintUiDxe
          if (!DisabledGroupType.IsByoCommonMaint) {
            NvOptions[Index].Attributes |= LOAD_OPTION_ACTIVE;
            EfiBootManagerLoadOptionToVariable(&NvOptions[Index]);
          }
        }
      }
    }
  #ifdef BYO_BOOT_MANAGER_LIB
    ByoEfiBootManagerFreeLoadOptions(NvOptions, NvOptionsCount);
  #else
    EfiBootManagerFreeLoadOptions(NvOptions, NvOptionsCount);
  #endif

  //
  // Update enumerated boot options
  //
  for (Index = 0; Index < BootOptionsCount; Index++) {
    if (BootOptions[Index].OptionNumber == DELETE_FLAG) {
      continue;
    }
    #ifdef BYO_BOOT_MANAGER_LIB
      if ((DevicePathType (BootOptions[Index].FilePath) != BBS_DEVICE_PATH)
        || (DevicePathSubType (BootOptions[Index].FilePath) != BBS_BBS_DP)) {
        continue;
      }
    #endif
    if (DisabledGroupType.DisableAll != 0) {
      BootOptions[Index].Attributes &= (~LOAD_OPTION_ACTIVE); // Check and disable all options
      continue;
    }

    #ifdef BYO_BOOT_MANAGER_LIB
      GroupType = GetBbsGroupType(LegacyBootOptionalDataGetPciClass(BootOptions[Index].OptionalData, NULL),
                                  (UINT8)((BBS_BBS_DEVICE_PATH*)BootOptions[Index].FilePath)->DeviceType);
    #else
      GroupType = (UINT8)GetEfiBootGroupType(BootOptions[Index].FilePath);
    #endif
    TypeEnabled = !IsCurrentTypeDisabled(&DisabledGroupType, GroupType);
    if (!TypeEnabled) {
      BootOptions[Index].Attributes &= (~LOAD_OPTION_ACTIVE);
    }
  }
}