/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  NoBootDeviceDxe.c

Abstract:
  Instant of gByoNoBootDeviceProtocolGuid.

Revision History:

**/

#include <Uefi.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <BoardIdType.h>
#include <SysMiscCfg.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LegacyBios.h>


#define SHELL_BOOT_STRING  L"Internal EDK Shell"

BOOLEAN mNoBootOptionCanRetry = TRUE;
BOOLEAN mIsNeedClear          = TRUE;
UINT8   mNoValibPromptIndex   = 1;
BOOLEAN mLegacyMode            = FALSE;

BOOLEAN
IsNetWorkBootOption (
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  ) 
{
  BBS_BBS_DEVICE_PATH           *BbsDp;

  if (!mLegacyMode) {
    if (StrStr (BootOption->Description, L"PXE") ||
        StrStr (BootOption->Description, L"HTTP")) { 
      return TRUE;
    }
  } else {
    BbsDp = (BBS_BBS_DEVICE_PATH*)BootOption->FilePath;
    if (BbsDp->Header.Type == BBS_DEVICE_PATH && 
        BbsDp->Header.SubType == BBS_BBS_DP &&
        BbsDp->DeviceType == BBS_EMBED_NETWORK) {
      return TRUE;
    }
  }
  
  DEBUG ((DEBUG_INFO, "IsNetWork: False\n"));
  return FALSE;
}



/**
  Attempt to boot each boot option in the BootOptions array.

  @param BootOptions       Input boot option array.
  @param BootOptionCount   Input boot option count.
  @param BootManagerMenu   Input boot manager menu.

  @retval TRUE  Successfully boot one of the boot options.
  @retval FALSE Failed boot any of the boot options.
**/
BOOLEAN
BootBootOptions (
  IN EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions,
  IN UINTN                           BootOptionCount,
  IN EFI_BOOT_MANAGER_LOAD_OPTION    *BootManagerMenu OPTIONAL
  )
{
  UINTN                              Index;
  BOOLEAN                            LastBootIsLegacy = FALSE;
  EFI_DEVICE_PATH_PROTOCOL           *Dp;

  DEBUG ((DEBUG_INFO, "BootOptionCount %d\n", BootOptionCount));

  //
  // Attempt boot each boot option
  //
  for (Index = 0; Index < BootOptionCount; Index++) {

    DEBUG ((DEBUG_INFO, "Loop Options Index %d : %s\n", Index, BootOptions[Index].Description));

    Dp = BootOptions[Index].FilePath;
    if(LastBootIsLegacy){
      if(DevicePathType(Dp) == BBS_DEVICE_PATH && DevicePathSubType(Dp) == BBS_BBS_DP){
        continue;
      }
    }

    //
    // According to EFI Specification, if a load option is not marked
    // as LOAD_OPTION_ACTIVE, the boot manager will not automatically
    // load the option.
    //
    if ((BootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      continue;
    }

    //
    // Boot#### load options with LOAD_OPTION_CATEGORY_APP are executables which are not
    // part of the normal boot processing. Boot options with reserved category values will be
    // ignored by the boot manager.
    //
    if ((BootOptions[Index].Attributes & LOAD_OPTION_CATEGORY) != LOAD_OPTION_CATEGORY_BOOT) {
      continue;
    }

    //
    // Skip Internal EDK Shell
    //
    if (StrCmp (BootOptions[Index].Description, SHELL_BOOT_STRING) == 0) {
      continue;
    }

    //
    // All the driver options should have been processed since
    // now boot will be performed.
    //
    DEBUG ((DEBUG_INFO, "Booting %s ...\n", BootOptions[Index].Description));
    
    if (!IsNetWorkBootOption(&BootOptions[Index])) {
      gST->ConOut->ClearScreen (gST->ConOut);
      Print (L"Booting %s ...\n", BootOptions[Index].Description);
      gBS->Stall (2 * 1000 * 1000);
    }

    EfiBootManagerBoot (&BootOptions[Index]);

    mNoBootOptionCanRetry = FALSE;
    mIsNeedClear          = TRUE;

    if(DevicePathType(Dp) == BBS_DEVICE_PATH && DevicePathSubType(Dp) == BBS_BBS_DP){
      LastBootIsLegacy = TRUE;
    }else {
      LastBootIsLegacy = FALSE;
    }

    //
    // If the boot via Boot#### returns with a status of EFI_SUCCESS, platform firmware
    // supports boot manager menu, and if firmware is configured to boot in an
    // interactive mode, the boot manager will stop processing the BootOrder variable and
    // present a boot manager menu to the user.
    //
    if ((BootManagerMenu != NULL) && (BootOptions[Index].Status == EFI_SUCCESS)) {
      EfiBootManagerBoot (BootManagerMenu);
      break;
    }
  }

  return (BOOLEAN) (Index < BootOptionCount);
}

VOID
EFIAPI
InspurNoBootDeviceHook (
  VOID
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION  *LoadOptions;
  UINTN                         LoadOptionCount;
  UINTN                         Index = 0;

  DEBUG ((DEBUG_INFO, "Start loop all boot options...\n"));

  for (;;) {
    DEBUG ((DEBUG_INFO, "Loop %d\n", Index++));
    LoadOptions = EfiBootManagerGetLoadOptions (&LoadOptionCount, LoadOptionTypeBoot);
    BootBootOptions (LoadOptions, LoadOptionCount, NULL);
    EfiBootManagerFreeLoadOptions (LoadOptions, LoadOptionCount);
    if (mNoBootOptionCanRetry) {
      if (mIsNeedClear) {
        gST->ConOut->ClearScreen (gST->ConOut);
        mIsNeedClear = FALSE;
      }
      switch (mNoValibPromptIndex) {
        case 1:
        gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
          Print (L"No Valid Boot Option, Still Retring.  \n");
          mNoValibPromptIndex++;
          gBS->Stall (2 * 1000 * 1000);
          break;
        case 2:
          gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
          Print (L"No Valid Boot Option, Still Retring.. \n");
          mNoValibPromptIndex++;
          gBS->Stall (2 * 1000 * 1000);
          break;
        case 3:
          gST->ConOut->SetCursorPosition(gST->ConOut, 0, 0);
          Print (L"No Valid Boot Option, Still Retring...\n");
          mNoValibPromptIndex = 1;
          gBS->Stall (2 * 1000 * 1000);
          break;
        default:
          break;
      }
    }
  }

}


EFI_STATUS
EFIAPI
NoBootDeviceDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status = EFI_SUCCESS;

  if (PcdGet8(PcdBiosBootModeType) == BIOS_BOOT_LEGACY_OS) {
    mLegacyMode = TRUE;
  } else {
    mLegacyMode = FALSE;
  }
  DEBUG ((DEBUG_INFO, "BootModeType:%x\n", PcdGet8(PcdBiosBootModeType)));

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gByoNoBootDeviceProtocolGuid, InspurNoBootDeviceHook,
                  NULL
                  );

  return Status;
}