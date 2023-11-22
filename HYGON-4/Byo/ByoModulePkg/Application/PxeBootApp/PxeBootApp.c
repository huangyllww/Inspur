/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadFile.h>
#include <Library/UefiLib.h>

#include <Library/ByoCommLib.h>
#include <Library/ByoBootManagerLib.h>

#ifndef BYO_ONLY_UEFI
#include <Protocol/LegacyBios.h>
VOID 
TryLegacyPxe (
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions,
  UINTN                         BootOptionCount
  )
{
  UINTN                         BootIndex;
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  EFI_STATUS                    Status;
  BBS_BBS_DEVICE_PATH           *BbsDp;

  Status = gBS->LocateProtocol(&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "No LegacyBios\n"));
    goto ProcExit;
  }

  for (BootIndex = 0; BootIndex < BootOptionCount; BootIndex ++) {
    if ((BootOptions[BootIndex].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      continue;
    }
    ShowDevicePathDxe(gBS, BootOptions[BootIndex].FilePath);
    BbsDp = (BBS_BBS_DEVICE_PATH*)BootOptions[BootIndex].FilePath;
    if(BbsDp->Header.Type == BBS_DEVICE_PATH && 
       BbsDp->Header.SubType == BBS_BBS_DP &&
       BbsDp->DeviceType == BBS_EMBED_NETWORK){
      DEBUG((EFI_D_INFO, "pxeboot %s\n", BootOptions[BootIndex].Description));      
      EfiBootManagerBoot(&BootOptions[BootIndex]);
      break;
    }
  }

ProcExit:
  return;
}
#endif

EFI_STATUS
EFIAPI
PxeBootAppEntry (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  UINTN                         Index;
  UINTN                         BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  UINT8                         RetryPxeBoot;

  RetryPxeBoot = PcdGet8(PcdRetryPxeBoot);
  if (RetryPxeBoot == 0) {
    PcdSet8S(PcdRetryPxeBoot, 1);
    PcdSet16S(PcdPxeRetrylimites, 0);
  }
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
  BootOptions = EfiBootManagerGetLoadOptions(&BootOptionCount, LoadOptionTypeBoot);
  gST->ConOut->ClearScreen (gST->ConOut);

  for (Index = 0; Index < BootOptionCount; Index++) {
    if ((BootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      continue;
    }
    if (GetEfiBootGroupType(BootOptions[Index].FilePath) == BM_MENU_TYPE_UEFI_PXE) {
      DEBUG ((EFI_D_INFO,"boot from native LAN BootOptions:%d,\n", Index));
      EfiBootManagerBoot (&BootOptions[Index]);
      break;
    }
  }

  PcdSet8S(PcdRetryPxeBoot, RetryPxeBoot);

  #ifndef BYO_ONLY_UEFI
  TryLegacyPxe(BootOptions, BootOptionCount);
  #endif

  EfiBootManagerFreeLoadOptions(BootOptions, BootOptionCount);

  return EFI_SUCCESS;
}
