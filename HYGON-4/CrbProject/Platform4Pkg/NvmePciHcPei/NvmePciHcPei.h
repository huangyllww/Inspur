/** @file
  This PEIM driver initialize Nvm Express host contoller and
  produce EdkiiPeiNvmExpressHostControllerPpi instance for other driver.

@copyright
  BYO CONFIDENTIAL
  Copyright 2019 Byosoft Corporation.

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Byosoft Corporation or its suppliers or
  licensors. Title to the Material remains with Byosoft Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary and
  confidential information of Byosoft Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Byosoft's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Byosoft in writing.

  Unless otherwise agreed by Byosoft in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Byosoft or
  Byosoft's suppliers or licensors in any way.

  This file contains a 'Sample Driver' and is licensed as such under the terms
  of your license agreement with Byosoft or your vendor. This file may be modified
  by the user, subject to the additional terms of the license agreement.

@par Specification Reference:
**/

#ifndef _NVME_PCI_HC_PEI_H_
#define _NVME_PCI_HC_PEI_H_

#include <PiPei.h>

#include <Ppi/MasterBootMode.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Ppi/NvmExpressHostController.h>

#include <IndustryStandard/Pci.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PciLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>

#include <AmdCpmDefine.h>
#include <Ppi/AmdCpmTablePpi/AmdCpmTablePpi.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <PciDevicePath.h>
#include <Ppi/ByoPciConfigRestore.h>
#include <HdpS3Info.h>
#include <Ppi/ReadOnlyVariable2.h>
//
// NVME HC PEI driver implementation related definitions
//
#define MAX_NVME_HCS                             8
#define ENDPOINT_PCI_BUS_NUMBER_START            0x10
#define PCI_BRIDGE_CONFIG_SPACE_STORE_SIZE       0x30

//
// PCIE configuration space offset for Pci-Pci Bridge
//
#define PCI_BRIDGE_IO_BASE                       0x1C
#define PCI_BRIDGE_IO_LIMIT                      0x1D
#define PCI_BRIDGE_MEMORY_BASE                   0x20
#define PCI_BRIDGE_MEMORY_LIMIT                  0x22
#define PCI_BRIDGE_PREFETCHABLE_MEMORY_BASE      0x24
#define PCI_BRIDGE_PREFETCHABLE_MEMORY_LIMIT     0x26
#define PCI_BRIDGE_PREFETCHABLE_BASE_UPPER32     0x28
#define PCI_BRIDGE_PREFETCHABLE_LIMIT_UPPER32    0x2C

#pragma pack(1)

//
// Device path for NVM Express host controller
//
typedef struct {
  ACPI_HID_DEVICE_PATH                     PciRoot;
  PCI_DEVICE_PATH                          PciBridge;
  PCI_DEVICE_PATH                          NvmeHcEp;
  EFI_DEVICE_PATH_PROTOCOL                 End;
} NVME_HC_DEVICE_PATH;

#pragma pack()


//
// EDKII_NVM_EXPRESS_HOST_CONTROLLER_PPI services
//

/**
  Get the MMIO base address of NVM Express host controller.

  @param[in]  This                 The PPI instance pointer.
  @param[in]  ControllerId         The ID of the NVM Express host controller.
  @param[out] MmioBar              The MMIO base address of the controller.

  @retval EFI_SUCCESS              The operation succeeds.
  @retval EFI_INVALID_PARAMETER    The parameters are invalid.
  @retval EFI_NOT_FOUND            The specified NVM Express host controller not
                                   found.

**/
EFI_STATUS
EFIAPI
GetNvmeHcMmioBar (
  IN  EDKII_NVM_EXPRESS_HOST_CONTROLLER_PPI    *This,
  IN  UINT8                                    ControllerId,
  OUT UINTN                                    *MmioBar
  );

/**
  Get the device path of NVM Express host controller.

  @param[in]  This                 The PPI instance pointer.
  @param[in]  ControllerId         The ID of the NVM Express host controller.
  @param[out] DevicePathLength     The length of the device path in bytes specified
                                   by DevicePath.
  @param[out] DevicePath           The device path of NVM Express host controller.
                                   This field re-uses EFI Device Path Protocol as
                                   defined by Section 10.2 EFI Device Path Protocol
                                   of UEFI 2.7 Specification.

  @retval EFI_SUCCESS              The operation succeeds.
  @retval EFI_INVALID_PARAMETER    The parameters are invalid.
  @retval EFI_NOT_FOUND            The specified NVM Express host controller not
                                   found.
  @retval EFI_OUT_OF_RESOURCES     The operation fails due to lack of resources.

**/
EFI_STATUS
EFIAPI
GetNvmeHcDevicePath (
  IN  EDKII_NVM_EXPRESS_HOST_CONTROLLER_PPI    *This,
  IN  UINT8                                    ControllerId,
  OUT UINTN                                    *DevicePathLength,
  OUT EFI_DEVICE_PATH_PROTOCOL                 **DevicePath
  );

#endif  // _NVME_PCI_HC_PEI_H_
