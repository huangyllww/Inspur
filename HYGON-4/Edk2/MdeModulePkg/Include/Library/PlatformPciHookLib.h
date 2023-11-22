/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _PLATFORM_PCI_HOOK_LIB_H_
#define _PLATFORM_PCI_HOOK_LIB_H_

#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciIo.h>

typedef
BOOLEAN
(EFIAPI *PCI_DEVICE_SUPPORT)(
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL     *PciRootBridgeIo,
  IN  UINT8                               BridgeBus,
  IN  UINT8                               BridgeDevice,
  IN  UINT8                               BridgeFunc,
  IN  UINT8                               Bus,
  IN  UINT8                               Device,
  IN  UINT8                               Func
  );

typedef
BOOLEAN
(EFIAPI *PCI_DEVICE_SRIOV_SUPPORT)(
  IN  EFI_PCI_IO_PROTOCOL     *PciIo
  );

typedef
VOID
(EFIAPI *PCI_BUS_SCAN_HOOK_FUNC)(
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo,
  IN  UINT8                             Bus,
  IN  UINT8                             Device,
  IN  UINT8                             Func,
  IN  BOOLEAN                           IsRootBridge
  );

typedef
BOOLEAN
(EFIAPI *PCI_DEVICE_DEGRADE_HOOK_FUNC)(
  IN  PCI_TYPE00                        *Pci,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL   *PciRootBridgeIo,
  IN  BOOLEAN                           IsCsmOn,
  IN  UINT32                            RomSize,
  IN  UINT8                             Bus,
  IN  UINT8                             Device,
  IN  UINT8                             Func
  );

extern PCI_DEVICE_SUPPORT            gPciDeviceSupportFuncPtr;
extern PCI_DEVICE_SRIOV_SUPPORT      gPciDeviceSrIovSupportFuncPtr;
extern PCI_BUS_SCAN_HOOK_FUNC        gPciScanBusHookFuncPtr;
extern PCI_DEVICE_DEGRADE_HOOK_FUNC  gPciDeviceDegradeHookFuncPtr;

#endif
