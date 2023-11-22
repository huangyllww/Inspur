/** @file

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

#include "SaveDevicePciConfig.h"


/*****************************************************************************
    PCIe Type 0/Type 1 Device has different PCI configuration space layout. 
  Registers at same offset could map to different meaning and functionality.

   PCIe Type 0 Device:
        Reg Offset  |  32bit   |   64bit
       -------------+----------+-------------                     
           0x10     |   BAR0   |   BAR0 Low
           0x14     |   BAR1   |   BAR0 Upper
           0x18     |   BAR2   |   BAR1 Low
           0x1C     |   BAR3   |   BAR1 Upper
           0x20     |   BAR4   |   BAR2 Low
           0x24     |   BAR5   |   BAR1
           0x04     |   Command Register


   PCIe Type 1 PCIe Bridge: 
         Reg Offset   |  32bit   |   64bit
       ---------------+----------+-------------                     
           0x10       |   BAR0   |   BAR0 Low
           0x14       |   BAR1   |   BAR0 High
           0x18       |   Primary Bus Number   
           0x19       |   Secondary Bus Number
           0x1A       |   Subordinate Bus Number
           0x1B       |   Secondary Latency Timer
           0x1C/0x1D  |   I/O Base/Limit
           0x1E       |   Secondary Status
           0x20/0x22  |   Memory Base/Limit
           0x24/0x26  |   Prefetchable Memory Base/Limit
           0x28/0x2C  |   Prefetchable Memory Base/Limit Upper
           0x04       |   Command Register

*******************************************************************************/
UINT8  mPciConfigRegisterOffset[] = {
        0x10 ,      // Reg Offset  0x10
        0x14 ,      // Reg Offset  0x14
        0x18 ,      // Reg Offset  0x18
        0x1C ,      // Reg Offset  0x1C
        0x20 ,      // Reg Offset  0x20
        0x24 ,      // Reg Offset  0x24
        0x28 ,      // Reg Offset  0x28
        0x04 ,      // Reg Offset  0x04
   };


/*
  This function convert given device path to its parent device path, by truncating off last non-end device path node
inside device path.
  Caution! 
    Input DevicePath will be changed in this function

*/
EFI_DEVICE_PATH_PROTOCOL *
GetParentDevicePath( 
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *CurNode = NULL;
  EFI_DEVICE_PATH_PROTOCOL    *NextNode = NULL;

  NextNode = DevicePath;
  while (!IsDevicePathEnd(NextNode)) {
    CurNode   = NextNode;
    NextNode = NextDevicePathNode (NextNode);
  }

  if (CurNode != NULL) {
    //
    // Truncate last non-End device path node
    CopyMem(CurNode, NextNode, sizeof(EFI_DEVICE_PATH_PROTOCOL));
  }

  return DevicePath;
}


/*
  This function locates Parent Device Handle

*/
EFI_HANDLE *
GetParentHandle (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
)
{
  EFI_STATUS               Status;
  EFI_HANDLE               Handle = NULL;
  EFI_DEVICE_PATH_PROTOCOL *TmpDevPath = NULL;
  EFI_DEVICE_PATH_PROTOCOL *OrgDevPath = NULL;

  OrgDevPath = DuplicateDevicePath (DevicePath);
  if (OrgDevPath == NULL) {
    return NULL;
  }

  if (IsDevicePathEnd(OrgDevPath)) {
    goto FUNC_EXIT;
  }

  TmpDevPath = GetParentDevicePath (OrgDevPath);

  //
  // Find Parent Handle which exactly-matches ParentDevicePath
  //
    Status = gBS->LocateDevicePath (&gEfiDevicePathProtocolGuid, &TmpDevPath, &Handle);
    if (EFI_ERROR(Status) || !IsDevicePathEnd(TmpDevPath)) {
      Handle = NULL;
    }

FUNC_EXIT:

  FreePool (OrgDevPath);
  return Handle;
}


EFI_STATUS 
GetSinglePciDevConfig(
  IN EFI_HANDLE DeviceHandle
  )
{
  EFI_STATUS                       Status;
  EFI_PCI_IO_PROTOCOL              *PciIo;
  UINTN                            Index;
  UINTN                            Segment, Bus, Dev, Func;
  PCI_DEV_CONFIG_INFO              *PciDevConfig;
  PCI_DEV_CONFIG_DATA              *ConfigData;
  UINT8                            HeaderType;
  UINT32                           PciRegVal;

  Status = gBS->HandleProtocol(
                  DeviceHandle,
                  &gEfiPciIoProtocolGuid, 
                  (VOID **)&PciIo
                  );
  if (EFI_ERROR(Status)){
    DEBUG ((EFI_D_ERROR,"  No pciio protocol allocated, skipped\n"));
    return Status;
  }

  PciDevConfig = AllocateZeroPool(sizeof(PCI_DEV_CONFIG_INFO));
  if (PciDevConfig == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ConfigData = &PciDevConfig->ConfigData;

  //
  // Save PCI Device (Bus Dev Func) info
  //
  PciIo->GetLocation(PciIo, &Segment, &Bus, &Dev, &Func);

  ConfigData->Bus = (UINT8)Bus;
  ConfigData->Dev = (UINT8)Dev;
  ConfigData->Func = (UINT8)Func;

  PciIo->Pci.Read(
               PciIo,
               EfiPciIoWidthUint8,
               PCI_HEADER_TYPE_OFFSET,
               1,
               &HeaderType
               );
  if ((HeaderType & HEADER_LAYOUT_CODE) == HEADER_TYPE_PCI_TO_PCI_BRIDGE ||
      (HeaderType & HEADER_LAYOUT_CODE) == HEADER_TYPE_CARDBUS_BRIDGE) {
    ConfigData->Type = PCIE_BRIDGE;
  } else {
    ConfigData->Type = PCIE_ENDPOINT;
  }

  DEBUG ((DEBUG_INFO, "  Save PciDev (%02X:%0X.%X) Type:%x\n", ConfigData->Bus, ConfigData->Dev, ConfigData->Func, ConfigData->Type));

  //
  // Save PCI Config Register Offset
  //
  for (Index = 0; Index < SAVE_REG_COUNT; Index ++) {
    ConfigData->PciRegOffset[Index] = mPciConfigRegisterOffset[Index];
  }

  //
  // Save Device/Bridge PCI Configuration Space Register Value
  //
  for (Index = 0; Index < SAVE_REG_COUNT; Index ++) {
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, ConfigData->PciRegOffset[Index], 1, &(PciRegVal));
    WriteUnaligned32(&ConfigData->PciRegVal[Index], PciRegVal);
    DEBUG ((DEBUG_INFO, "    Offset %02x : %x\n", ConfigData->PciRegOffset[Index], ReadUnaligned32(&ConfigData->PciRegVal[Index])));
  }
 
  //
  // Add Saved Device/Bridge PCI Configuration Space Context to PciDevice Config Database
  //
  InsertHeadList(&gPciDeviceConfigInfoList, &PciDevConfig->Link);

  return EFI_SUCCESS;
}

/**
  This is the save function for saving Pci device PCI Configuration Space
  as well as attached upstream PCI Bridge PCI Configuration Space

  @param[out] PcieInfo      The info this save function to save.
**/ 
EFI_STATUS
GetPciDevConfigInfo(
  PCI_DEVICE       *PcieDev
)
{
  EFI_STATUS                        Status;
  EFI_HANDLE                        DeviceHandle;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             Index;
  UINTN                             NumHandles;
  UINTN                             Segment, Bus, Dev, Func;
  EFI_DEVICE_PATH_PROTOCOL          *DevDp = NULL;
  EFI_PCI_IO_PROTOCOL               *PciIo;

  DEBUG ((DEBUG_INFO, "%a ---->\n", __FUNCTION__ ));

  DeviceHandle = NULL;
  NumHandles   = 0;
  HandleBuffer = NULL;

  //
  // Find Input PCI device(BDF)'s Handle
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &NumHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < NumHandles; Index ++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if (!EFI_ERROR (Status)) {
      PciIo->GetLocation(PciIo, &Segment, &Bus, &Dev, &Func);
      if (PcieDev->Bus == Bus && PcieDev->Dev == Dev && PcieDev->Func == Func) {
        DeviceHandle = HandleBuffer[Index];
        break;
      }
    }
  }

  if (DeviceHandle == NULL) {
    DEBUG ((DEBUG_INFO, " PciDev %02X:%0X.%X not found\n", PcieDev->Bus, PcieDev->Dev, PcieDev->Func));
  }

  //
  // Get PCI Configuration Space for this PCI Device & associated upstream PCI Bridges.
  // Logic first starts from PCI device, then climbs up to associated upstream PCI Bridges
  //
  while (DeviceHandle != NULL) {
    Status = gBS->HandleProtocol (DeviceHandle, &gEfiDevicePathProtocolGuid, (VOID **)&DevDp);
    if (EFI_ERROR(Status)) {
      DEBUG ((EFI_D_ERROR,"  Locate DevicePath fail\n"));
      break;
    }

    DEBUG ((DEBUG_INFO, "  "));
    ShowDevicePathDxe(gBS, DevDp);
 
    Status = GetSinglePciDevConfig(DeviceHandle);
    if (EFI_ERROR(Status)) {
      break;
    }
  
    //
    // Move to upstream Bridge Handle
    //
    DeviceHandle = GetParentHandle (DevDp);
  }


  FreePool(HandleBuffer);

  return Status;
}



EFI_STATUS
SavePciDevConfigInfoToNvram (
  VOID
  )
{
  EFI_STATUS           Status;
  PCI_DEV_CONFIG_DATA  *ConfigDataBuffer;
  PCI_DEV_CONFIG_INFO  *PciConfigInfo;
  LIST_ENTRY           *Link;
  UINTN                ConfigCount = 0;
  UINTN                ConfigIndex;
  UINTN                RegIndex;
  UINTN                Index;
  BOOLEAN              Duplicate;

  DEBUG ((DEBUG_INFO, "%a Start\n", __FUNCTION__ ));
  ConfigCount = 0;
  BASE_LIST_FOR_EACH(Link, &gPciDeviceConfigInfoList) {
    ConfigCount++;
  }

  if (ConfigCount == 0) {
    return EFI_SUCCESS;
  }

  ConfigDataBuffer = AllocateZeroPool(sizeof(PCI_DEV_CONFIG_DATA) * ConfigCount);
  if (ConfigDataBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Dump All PCI Device Config Data from Configuration Info Link List(Storage in Memory)
  //
  DEBUG_CODE (
     DEBUG((DEBUG_INFO, "======================= Dump All PCI Device Config Data =======================\n"));
     DEBUG((DEBUG_INFO, "Seg Bus Fun Dev\\Reg: "));
     for (RegIndex = 0; RegIndex < SAVE_REG_COUNT; RegIndex ++) {
       DEBUG((DEBUG_INFO, "  0x%02x      ", mPciConfigRegisterOffset[RegIndex]));
     }
     DEBUG((DEBUG_INFO, "\n"));
   
     BASE_LIST_FOR_EACH(Link, &gPciDeviceConfigInfoList) {
       PciConfigInfo = PCI_DEV_CONFIG_INFO_FROM_LINK(Link);
       DEBUG((DEBUG_INFO, " %02X  %02X  %02X  %X      : ", PciConfigInfo->ConfigData.Seg, PciConfigInfo->ConfigData.Bus, PciConfigInfo->ConfigData.Dev, PciConfigInfo->ConfigData.Func));
       for (RegIndex = 0; RegIndex < SAVE_REG_COUNT; RegIndex ++) {
         DEBUG((DEBUG_INFO, " 0x%08x ", ReadUnaligned32(&PciConfigInfo->ConfigData.PciRegVal[RegIndex])));
       }
       DEBUG((DEBUG_INFO, "\n"));
     }
       DEBUG((DEBUG_INFO, "===============================================================================\n"));
     );

  //
  // Retrieve each PCI Device Configuration Data from Configuration Info Link List(Storage in Memory)
  //
  ConfigIndex = 0;
  BASE_LIST_FOR_EACH(Link, &gPciDeviceConfigInfoList) {
    PciConfigInfo = PCI_DEV_CONFIG_INFO_FROM_LINK(Link);
    //Filtering duplicate information
    Duplicate = FALSE;
    for (Index = 0; Index < ConfigIndex; Index++) {
      if((ConfigDataBuffer[Index].Bus == PciConfigInfo->ConfigData.Bus) && 
         (ConfigDataBuffer[Index].Dev == PciConfigInfo->ConfigData.Dev) && 
         (ConfigDataBuffer[Index].Func == PciConfigInfo->ConfigData.Func )) {
         Duplicate = TRUE;
         break;
      }
    }
    if(Duplicate == FALSE){
      CopyMem(&ConfigDataBuffer[ConfigIndex], &PciConfigInfo->ConfigData, sizeof(PCI_DEV_CONFIG_DATA));
      ConfigIndex++;
    }
  }

  //
  // Dump PCI Device Configuration Data
  //
  ConfigCount = ConfigIndex;
  DEBUG_CODE (
     DEBUG((DEBUG_INFO, "======================= Save PCI Device Config Data =======================\n"));
     DEBUG((DEBUG_INFO, "Seg Bus Fun Dev\\Reg: "));
     for (RegIndex = 0; RegIndex < SAVE_REG_COUNT; RegIndex ++) {
       DEBUG((DEBUG_INFO, "  0x%02x      ", mPciConfigRegisterOffset[RegIndex]));
     }
     DEBUG((DEBUG_INFO, "\n"));
   
     for (ConfigIndex = 0; ConfigIndex < ConfigCount; ConfigIndex++) {
       DEBUG((DEBUG_INFO, " %02X  %02X  %02X  %X      : ", ConfigDataBuffer[ConfigIndex].Seg, ConfigDataBuffer[ConfigIndex].Bus, ConfigDataBuffer[ConfigIndex].Dev, ConfigDataBuffer[ConfigIndex].Func));
       for (RegIndex = 0; RegIndex < SAVE_REG_COUNT; RegIndex ++) {
         DEBUG((DEBUG_INFO, " 0x%08x ", ReadUnaligned32(&ConfigDataBuffer[ConfigIndex].PciRegVal[RegIndex])));
       }
       DEBUG((DEBUG_INFO, "\n"));
     }
     DEBUG((DEBUG_INFO, "===============================================================================\n"));
  );

  //
  // Save PCI Device Configuration Data to NV Storage
  //
  Status = gRT->SetVariable (
                  CONFIG_DATA_VAR_NAME,
                  &gPciDevConfigDataVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof(PCI_DEV_CONFIG_DATA) * ConfigCount,
                  ConfigDataBuffer
                  );
  ASSERT_EFI_ERROR(Status);

  DEBUG((DEBUG_INFO, "[SaveDevicePciConfig] Save %s variable Size 0x%x\n", CONFIG_DATA_VAR_NAME, sizeof(PCI_DEV_CONFIG_DATA) * ConfigCount));

  return Status;

}