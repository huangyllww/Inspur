/** @file

Copyright (c) 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _DEVICE_PCI_RESTORE_H
#define _DEVICE_PCI_RESTORE_H

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ByoCommLib.h>
#include <Library/TimerLib.h>
#include <Library/PciLib.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/ByoPciConfigRestore.h>

#define CONFIG_DATA_VAR_NAME   L"PciDevConfigData"

#define SAVE_REG_COUNT 8

#pragma pack(1)

typedef struct {
  UINT16  Seg;
  UINT8   Bus;
  UINT8   Dev;
  UINT8   Func;
  UINT8   Type;
  UINT8   PciRegOffset[SAVE_REG_COUNT];
  UINT32  PciRegVal[SAVE_REG_COUNT];
} PCI_DEV_CONFIG_DATA;

#pragma pack()


EFI_STATUS
EFIAPI
RestoreDevicePciConfig (
  VOID
  );

#endif