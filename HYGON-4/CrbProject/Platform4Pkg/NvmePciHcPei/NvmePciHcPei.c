/**@file
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

#include "NvmePciHcPei.h"

EDKII_NVM_EXPRESS_HOST_CONTROLLER_PPI  mNvmeHostControllerPpi = {
  GetNvmeHcMmioBar,
  GetNvmeHcDevicePath
};

EFI_PEI_PPI_DESCRIPTOR  mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEdkiiPeiNvmExpressHostControllerPpiGuid,
  &mNvmeHostControllerPpi
};

UINTN gTotalNvmeHcs;
UINTN gNvmeHcBar[MAX_NVME_HCS];
//
// Template for a NVM Express Host controller
//
NVME_HC_DEVICE_PATH  mNvmeHcDevicePathTemplate = {
  { 
    // PciRoot
    {
      ACPI_DEVICE_PATH, ACPI_DP, 
      { 
        (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)), 
        (UINT8) ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8) 
      }
    },
    EISA_PNP_ID (0x0A03), 
    0
  },
  {  // PciBridge
    {
      HARDWARE_DEVICE_PATH,
      HW_PCI_DP,
      {
        (UINT8) (sizeof (PCI_DEVICE_PATH)),
        (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8)
      }
    },
    1,
    1
  },
  {  // NvmeHcEp
    {
      HARDWARE_DEVICE_PATH,
      HW_PCI_DP,
      {
        (UINT8) (sizeof (PCI_DEVICE_PATH)),
        (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8)
      }
    },
    0,
    0
  },
  {  // End
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (sizeof (EFI_DEVICE_PATH_PROTOCOL)),
      (UINT8) ((sizeof (EFI_DEVICE_PATH_PROTOCOL)) >> 8)
    }
  }
};

EFI_STATUS
EFIAPI
VarLibGetVariable (
  IN     CHAR16                      *VariableName,
  IN     EFI_GUID                    *VendorGuid,
  OUT    UINT32                      *Attributes,    OPTIONAL
  IN OUT UINTN                       *DataSize,
  OUT    VOID                        *Data           OPTIONAL
  )
{
  EFI_STATUS                          Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI     *VariablePpi;

  //
  // Locate the variable PPI.
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariablePpi
             );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (VariablePpi != NULL) {
    Status = VariablePpi->GetVariable (
              VariablePpi,
              VariableName,
              VendorGuid,
              Attributes,
              DataSize,
              Data
              );
  } else {
    Status = EFI_UNSUPPORTED;
  }
  return Status;
}
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
  )
{
  if ((This == NULL) || (MmioBar == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if (ControllerId >= gTotalNvmeHcs) {
    return EFI_NOT_FOUND;
  }

  *MmioBar = gNvmeHcBar[ControllerId];

  return EFI_SUCCESS;
}


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
  )
{
  if ((This == NULL) || (DevicePath == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  //ToDo: We first assume There is only one System NVMe Controller with **FIXED** Bus Device Function
  //
  if (ControllerId >= gTotalNvmeHcs) {
    return EFI_NOT_FOUND;
  }

  *DevicePathLength = sizeof(NVME_HC_DEVICE_PATH);
  *DevicePath       = (EFI_DEVICE_PATH_PROTOCOL*)&mNvmeHcDevicePathTemplate;

  return EFI_SUCCESS;
}

/**
  Check whether the controller is a PCI NVM Express host controller.

  @param[in] PciHcBase    PCI address of a specified controller.

  @retval TRUE     The specified controller is an NVM Express host controller.
  @retval FALSE    The specified controller is not an NVM Express host controller.

**/
BOOLEAN
IsPciNvmeHc (
  IN UINTN    PciHcBase
  )
{
  UINT8    SubClass;
  UINT8    BaseClass;
  UINT8    ProgInt;

  ProgInt   = PciRead8 (PciHcBase + PCI_CLASSCODE_OFFSET);
  SubClass  = PciRead8 (PciHcBase + PCI_CLASSCODE_OFFSET + 1);
  BaseClass = PciRead8 (PciHcBase + PCI_CLASSCODE_OFFSET + 2);

  if ((BaseClass != PCI_CLASS_MASS_STORAGE) ||
      (SubClass  != PCI_CLASS_MASS_STORAGE_SOLID_STATE) ||
      (ProgInt   != PCI_IF_MASS_STORAGE_SOLID_STATE_ENTERPRISE_NVMHCI)) {
    return FALSE;
  }

  return TRUE;
}

/**
  Program the PCI configuration space of a PCI-PCI bridge to enable it.

  @param[in, out] HcInfo    A pointer to the NVME_HC_INFO structure.


/**
  Initialize NVM Express host controller.

  @retval EFI_SUCCESS    The function completes successfully.
  @retval others         The function fails.

**/
EFI_STATUS
NvmePciConfigSpaceProgram (
  VOID
  )
{
  EFI_STATUS                          Status;
  BYO_RESTORE_DEVICE_PCI_CONFIG_PPI   *RestorePciConfig;
  HDP_S3_DATA                         *HdpS3Data;
  HDP_PORT_INFO                       *PortInfo;
  UINTN                               VarDataSize;
  UINTN                               PciHcBase[MAX_NVME_HCS];
  UINTN                               NvmeHcBar[MAX_NVME_HCS];
  UINTN                               Index;

  //
  // Restore PCI Configuration Space for NVMe Controller and Upstream PCI Bridge
  //
  Status = PeiServicesLocatePpi (
              &gByoRestorePciDeviceConfigPpiGuid,
              0,
              NULL,
              (VOID **) &RestorePciConfig
              );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Fail to Allocate ByoRestorePciDeviceConfigPpi\n"));
    ASSERT(FALSE);
    return Status;
  }

  Status = RestorePciConfig->RestoreDevicePciConfig();
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "Fail to Restore Device PCI Configuration Space. HddPassword Variable May not exist\n"));
    return Status;
  }

  // 
  // Get BYO NVMe/HDD Password Info from Variable
  //
  VarDataSize = 0;
  Status = VarLibGetVariable (L"HddUnlockInfo", &gEfiHddPasswordSecurityVariableGuid, NULL, &VarDataSize, NULL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    DEBUG((DEBUG_INFO, "[%a] Fail to find Variable %s, it may not exist\n", __FUNCTION__, L"HddUnlockInfo"));
    return FALSE;
  }

  HdpS3Data = AllocatePages (EFI_SIZE_TO_PAGES (VarDataSize));
  ZeroMem(HdpS3Data, VarDataSize);
  if (HdpS3Data == NULL) {
    return FALSE;
  }

  Status = VarLibGetVariable (L"HddUnlockInfo", &gEfiHddPasswordSecurityVariableGuid, NULL, &VarDataSize, HdpS3Data);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "[%a] Fail to get Variable %s\n", __FUNCTION__, L"HddUnlockInfo"));
    return Status;
  }
  
  gTotalNvmeHcs = HdpS3Data->EnCount;

  /*
   In S3 Path, NVMe Controller and upstream Bridge PCI configuration space
   will be restored by calling RestorePciConfiguration, uppper in this function.

   In this Loop,we get all device info from Variable(L"HddUnlockInfo"),and select Nvme device.
   Here, we get NvmeBar value by retrieving from Pci Host controller Base
  */ 
 
  for (Index = 0; (Index < HdpS3Data->EnCount) && (Index < MAX_NVME_HCS); Index++) {
    PortInfo         = &HdpS3Data->Port[Index];
    PciHcBase[Index] = PCI_LIB_ADDRESS(PortInfo->Bus, PortInfo->Dev, PortInfo->Func, 0);

    if(IsPciNvmeHc(PciHcBase[Index]) == TRUE){
      NvmeHcBar[Index]   = PciRead32(PciHcBase[Index] + PCI_BASE_ADDRESSREG_OFFSET);
      gNvmeHcBar[Index]  = (NvmeHcBar[Index] & 0xFFFFFFF0);

      DEBUG((EFI_D_INFO, "[%a] Restored gNvmeHcBar:%x\n", __FUNCTION__, gNvmeHcBar[Index]));
      DumpPci32((VOID*)PciHcBase[Index], 256);
    }
  }

  FreePages (HdpS3Data, EFI_SIZE_TO_PAGES (VarDataSize));
  return Status;
}

#if 0
/**
  One notified function to cleanup the allocated resources at the end of PEI.

  @param[in] PeiServices         Pointer to PEI Services Table.
  @param[in] NotifyDescriptor    Pointer to the descriptor for the Notification
                                 event that caused this function to execute.
  @param[in] Ppi                 Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS            The function completes successfully.

**/
EFI_STATUS
EFIAPI
NvmeHcEndOfPei (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  NVME_HC_PEI_PRIVATE_DATA    *Private;
  UINTN                       HcIndex;

  Private = NVME_HC_PEI_PRIVATE_DATA_FROM_THIS_NOTIFY (NotifyDescriptor);
  ASSERT (Private->TotalNvmeHcs <= MAX_NVME_HCS);

  for (HcIndex = 0; HcIndex < Private->TotalNvmeHcs; HcIndex++) {
    NvmeRestoreHcPciConfigSpace (&(Private->HcInfo[HcIndex]));
  }

  return EFI_SUCCESS;
}
#endif

/**
  One notified function at the installation of EDKII_PEI_STORAGE_SECURITY_CMD_PPI.
  It is to unlock OPAL password for S3.

  @param[in] PeiServices         Indirect reference to the PEI Services Table.
  @param[in] NotifyDescriptor    Address of the notification descriptor data structure.
  @param[in] Ppi                 Address of the PPI that was installed.

  @return Status of the notification.
          The status code returned from this function is ignored.

**/
EFI_STATUS
EFIAPI
PeiPostScriptTablePpiNotify (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDesc,
  IN VOID                         *Ppi
  )
{
  EFI_STATUS Status;

  DEBUG ((DEBUG_INFO, "%a entered at S3 resume!\n", __FUNCTION__));

  //
  // Restore NVME cont
  //
  Status = NvmePciConfigSpaceProgram ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "NvmePciConfigSpaceProgram fail with %r\n", Status));
    return Status;
  }

  Status = PeiServicesInstallPpi (&mPpiList);
  ASSERT_EFI_ERROR(Status);

  DEBUG ((DEBUG_INFO, "%a exit at S3 resume!\n", __FUNCTION__));

  return Status;
}



EFI_PEI_NOTIFY_DESCRIPTOR mPeiPostScriptTablePpiNotifyDesc = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiPostScriptTablePpiGuid,
  PeiPostScriptTablePpiNotify
};


/**
  The user code starts with this function.

  @param  FileHandle             Handle of the file being invoked.
  @param  PeiServices            Describes the list of possible PEI Services.

  @retval EFI_SUCCESS            The driver is successfully initialized.
  @retval Others                 Can't initialize the driver.

**/
EFI_STATUS
EFIAPI
NvmePciHcPeimEntry (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS       Status;
  EFI_BOOT_MODE    BootMode;

  Status = PeiServicesGetBootMode (&BootMode);
  if (EFI_ERROR (Status) || (BootMode != BOOT_ON_S3_RESUME)) {
    DEBUG ((DEBUG_ERROR, "%a: Fail to get the S3 boot mode.\n", __FUNCTION__));
    return Status;
  }

  DEBUG ((DEBUG_INFO, "%a: Enters in S3 path.\n", __FUNCTION__));

  Status = PeiServicesNotifyPpi (&mPeiPostScriptTablePpiNotifyDesc);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
