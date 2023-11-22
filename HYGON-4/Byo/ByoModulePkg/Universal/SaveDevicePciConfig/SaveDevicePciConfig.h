/** @file
  This is PCI platform initialization code.

  @copyright
  BYO CONFIDENTIAL
  Copyright 2004 - 2018 Byosoft Corporation. <BR>

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Byosoft Corporation or its suppliers or
  licensors. Title to the Material remains with Byosoft Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary    and
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
**/

#ifndef _SAVE_DEVICE_PCI_CONFIG_H_
#define _SAVE_DEVICE_PCI_CONFIG_H_

#include <PiDxe.h>
#include <IndustryStandard/Pci.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ByoCommLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <Protocol/ByoPciConfigSaveProtocol.h>

#define CONFIG_DATA_VAR_NAME   L"PciDevConfigData"

#define SAVE_REG_COUNT  8
#define PCIE_BRIDGE     1
#define PCIE_ENDPOINT   2


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

typedef struct {
  PCI_DEV_CONFIG_DATA  ConfigData;
  LIST_ENTRY           Link;
} PCI_DEV_CONFIG_INFO;

typedef struct {
  UINT16  Seg;
  UINT8   Bus;
  UINT8   Dev;
  UINT8   Func;
  LIST_ENTRY Link;
} PCI_DEVICE;


#define PCI_DEVICE_FROM_LINK(a) \
  BASE_CR(a, PCI_DEVICE, Link)


#define PCI_DEV_CONFIG_INFO_FROM_LINK(a) \
  BASE_CR(a, PCI_DEV_CONFIG_INFO, Link)

extern LIST_ENTRY   gPciDeviceList;
extern LIST_ENTRY   gPciDeviceConfigInfoList;


/**
  Platform use this function to register device that requires to save PCI configuration space

  @param[in] 
**/
EFI_STATUS
EFIAPI
RegisterPciDeviceToSave(
  IN UINT16   Seg,
  IN UINT8    Bus,
  IN UINT8    Dev,
  IN UINT8    Func
);

/**
  This is the save function for saving Pci device PCI Configuration Space
  as well as attached upstream PCI Bridge PCI Configuration Space

  @param[out] PcieInfo      The info this save function to save.
**/ 
EFI_STATUS
GetPciDevConfigInfo(
  PCI_DEVICE       *PcieDev
);


EFI_STATUS
SavePciDevConfigInfoToNvram (
  VOID
  );
#endif