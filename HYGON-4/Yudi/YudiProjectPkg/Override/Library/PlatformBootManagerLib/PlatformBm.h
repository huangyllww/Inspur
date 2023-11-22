/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformBm.h

Abstract:
  The platform support PlatformBootManagerLib.

Revision History:

**/

#ifndef _PLATFORM_BM_H_
#define _PLATFORM_BM_H_

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/PlatformBootManagerLib.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/ByoNetworkIdentifierProtocol.h>
#include <DhcpGetNetworldCfg.h>

#define SETUP_BOOT_STRING  L"Enter Setup"
#define SHELL_BOOT_STRING  L"Internal EDK Shell"

VOID
EFIAPI
InstallAdditionalOpRom (
  VOID
  );

#endif // _PLATFORM_BM_H_
