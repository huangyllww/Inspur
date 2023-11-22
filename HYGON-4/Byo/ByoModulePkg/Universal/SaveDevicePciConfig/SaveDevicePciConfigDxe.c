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


EFI_RSC_HANDLER_PROTOCOL    *mRscHandlerProtocol = NULL;
EFI_HANDLE                  mHandle = NULL;
BOOLEAN                     mPciDeviceSavingDone = FALSE;

//
// List of PCI Devices registered through ByoSavePciDeviceSpace Protocol, with request to save/restore PCI space
//
LIST_ENTRY   gPciDeviceList = INITIALIZE_LIST_HEAD_VARIABLE (gPciDeviceList);

//
// List used to store PCI configuration space content for registered device and corresponding upstream PCI bridges
//
LIST_ENTRY   gPciDeviceConfigInfoList = INITIALIZE_LIST_HEAD_VARIABLE (gPciDeviceConfigInfoList);


BYO_SAVE_PCI_DEVICE_CONFIG_PROTOCOL mByoSavePciDeviceConfig = {
  RegisterPciDeviceToSave
};

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
) {
  PCI_DEVICE  *PciDevice;

  PciDevice = AllocateZeroPool(sizeof(PCI_DEVICE));
  if (PciDevice == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  PciDevice->Seg = Seg;
  PciDevice->Bus = Bus;
  PciDevice->Dev = Dev;
  PciDevice->Func = Func;

  InsertTailList(&gPciDeviceList, &PciDevice->Link);

  return EFI_SUCCESS;
}


/**
  Report status code listener of FPDT. This is used to collect performance data
  for OsLoaderLoadImageStart and OsLoaderStartImageStart in FPDT.

  @param[in]  CodeType            Indicates the type of status code being reported.
  @param[in]  Value               Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is used to
                                  classify the entity as well as an operation.
  @param[in]  Instance            The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param[in]  CallerId            This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different rules to
                                  different callers.
  @param[in]  Data                This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS             Status code is what we expected.
  @retval EFI_UNSUPPORTED         Status code not supported.

**/
EFI_STATUS
EFIAPI
SavePciDevConfigStatusCodeListener (
  IN EFI_STATUS_CODE_TYPE       CodeType,
  IN EFI_STATUS_CODE_VALUE      Value,
  IN UINT32                     Instance,
  IN EFI_GUID                   *CallerId,
  IN EFI_STATUS_CODE_DATA       *Data
  )
{
  LIST_ENTRY                       *Link;
  PCI_DEVICE                       *PciDevice;
  EFI_STATUS                        Status;
  
  //
  // Check whether status code is (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_READY_TO_BOOT_EVENT)
  //
  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) != EFI_PROGRESS_CODE ||
      Value != (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_READY_TO_BOOT_EVENT)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Return when more than one Boot retry happens
  //
  if (mPciDeviceSavingDone) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((DEBUG_INFO, "%a Start\n", __FUNCTION__ ));

  //
  // First, unregister listener to avoid collecting PCI device config more than once.
  //
  Status = mRscHandlerProtocol->Unregister (SavePciDevConfigStatusCodeListener);
  //
  // Dump PCI configuration space to **MEMORY** for PCI Device and related upstream PCI bridges
  //
  BASE_LIST_FOR_EACH(Link, &gPciDeviceList) {
    PciDevice = PCI_DEVICE_FROM_LINK(Link);
    GetPciDevConfigInfo(PciDevice);
  }
  
  //
  // Save dumped PCI Config Info to NVStorage
  //
  SavePciDevConfigInfoToNvram();

  mPciDeviceSavingDone = TRUE;

  return EFI_SUCCESS;
}



/**
  On ReadyToBoot, this Module saves PCI device configuration space content for S3 restoration
  Saving logic starts from one platform-predefined PCI Device, then goes upwards to its immediate upstream PCI-to-PCI Bridge,
  and finally stops when reaching Root Bridge at the top (The Root Bridge configuration space is also saved). 

  @param[in] ImageHandle       Image handle of this driver.
  @param[in] SystemTable       Global system service table.

  @retval EFI_SUCCESS           Initialization complete.
  @exception EFI_UNSUPPORTED       The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES  Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR      Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
SaveDevicePciConfigDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;
  
  DEBUG ((DEBUG_INFO, "%a Enter\n", __FUNCTION__ ));

  //
  // Get Report Status Code Handler Protocol.
  //
  Status = gBS->LocateProtocol (&gEfiRscHandlerProtocolGuid, NULL, (VOID **) &mRscHandlerProtocol);
  ASSERT_EFI_ERROR (Status);

  //
  // Use StatusCodeHandler to defer Saving PciDevice Configuration Space until **AFTER** ReadyToBoot.
  // As modules like HddPassword determine PCI device to save PCI config in ReadyToBoot Callback
  //
  Status = mRscHandlerProtocol->Register (SavePciDevConfigStatusCodeListener, TPL_HIGH_LEVEL);
  ASSERT_EFI_ERROR (Status);


  //
  // Install SavePciDeviceInfo Protocol to let platform register PCI Device to save 
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mHandle,
                  &gByoSavePciDeviceConfigProtocolGuid,
                  &mByoSavePciDeviceConfig,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}