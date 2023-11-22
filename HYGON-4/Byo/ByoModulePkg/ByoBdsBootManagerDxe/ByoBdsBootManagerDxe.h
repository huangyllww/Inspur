/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoBdsBootManagerDxe.h

Abstract:
  The platform installs gByoPlatformBootManagerProtocolGuid.

Revision History:

**/

#ifndef __PLATFORM_BOOT_MANAGER_DXE_H__
#define __PLATFORM_BOOT_MANAGER_DXE_H__

#include <ByoBootGroup.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/ByoBootManagerLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/BdsBootManagerProtocol.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#ifndef BYO_ONLY_UEFI
#include <Protocol/LegacyBootManager.h>
#endif
#include <SysMiscCfg.h>

EFI_STATUS
EFIAPI
ByoEfiBootManagerGetLoadOptions2 (
  OUT EFI_BOOT_MANAGER_LOAD_OPTION   **Option,
  OUT UINTN                          *OptionCount
);


#endif
