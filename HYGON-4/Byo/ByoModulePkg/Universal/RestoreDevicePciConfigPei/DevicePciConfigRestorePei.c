/** @file

Copyright (c) 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiBoot.c

Abstract:
  Source file for the IPMI PEIM.

Revision History:

**/


#include "DevicePciConfigRestorePei.h"


static BYO_RESTORE_DEVICE_PCI_CONFIG_PPI  mByoRestoreDevicePciConfig = { RestoreDevicePciConfig };

static EFI_PEI_PPI_DESCRIPTOR       mPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gByoRestorePciDeviceConfigPpiGuid,
    &mByoRestoreDevicePciConfig
  }
};



VOID
DumpConfigData(
  PCI_DEV_CONFIG_DATA      *ConfigDataBuffer,
  UINTN                    ConfigCount
  )
{
  UINTN Index;
  UINTN RegIndex;

  //
  // Dump PCI Device Configuration Data
  //
  DEBUG((DEBUG_INFO, "======================= Dump Variable Config Data =======================\n"));
  DEBUG((DEBUG_INFO, "Seg Bus Fun Dev\\Reg: "));
  for (RegIndex = 0; RegIndex < SAVE_REG_COUNT; RegIndex ++) {
    DEBUG((DEBUG_INFO, "  0x%02x      ", ConfigDataBuffer[0].PciRegOffset[RegIndex]));
  }
  DEBUG((DEBUG_INFO, "\n"));

  for (Index = 0; Index < ConfigCount; Index++) {
    DEBUG((DEBUG_INFO, " %02X  %02X  %02X  %X      : ", ConfigDataBuffer[Index].Seg, ConfigDataBuffer[Index].Bus, ConfigDataBuffer[Index].Dev, ConfigDataBuffer[Index].Func));

    for (RegIndex = 0; RegIndex < SAVE_REG_COUNT; RegIndex ++) {
      DEBUG((DEBUG_INFO, " 0x%08x ", ReadUnaligned32(&ConfigDataBuffer[Index].PciRegVal[RegIndex])));
    }
    DEBUG((DEBUG_INFO, "\n"));
  }

  DEBUG((DEBUG_INFO, "========================================================================\n"));

}




VOID
DumpConfigDataFromPciDevice(
  PCI_DEV_CONFIG_DATA      *ConfigDataBuffer,
  UINTN                    ConfigCount
  )
{
  UINTN PciAddress;
  UINTN Index;
  UINTN RegIndex;

  //
  // Dump PCI Device Configuration Data
  //
  DEBUG((DEBUG_INFO, "======================= Restored PCI Device Config Data =======================\n"));
  DEBUG((DEBUG_INFO, "Seg Bus Fun Dev\\Reg: "));
  for (RegIndex = 0; RegIndex < SAVE_REG_COUNT; RegIndex ++) {
    DEBUG((DEBUG_INFO, "  0x%02x      ", ConfigDataBuffer[0].PciRegOffset[RegIndex]));
  }
  DEBUG((DEBUG_INFO, "\n"));

  for (Index = 0; Index < ConfigCount; Index++) {
    DEBUG((DEBUG_INFO, " %02X  %02X  %02X  %X      : ", ConfigDataBuffer[Index].Seg, ConfigDataBuffer[Index].Bus, ConfigDataBuffer[Index].Dev, ConfigDataBuffer[Index].Func));

    for (RegIndex = 0; RegIndex < SAVE_REG_COUNT; RegIndex ++) {
      PciAddress = PCI_LIB_ADDRESS(
                     ConfigDataBuffer[Index].Bus,
                     ConfigDataBuffer[Index].Dev,
                     ConfigDataBuffer[Index].Func,
                     ConfigDataBuffer[Index].PciRegOffset[RegIndex]
                     );

      DEBUG((DEBUG_INFO, " 0x%08x ", PciRead32(PciAddress)));
    }
    DEBUG((DEBUG_INFO, "\n"));
  }

  DEBUG((DEBUG_INFO, "===============================================================================\n"));

}


/**
  Returns the value of a variable.

  @param[in]       VariableName  A Null-terminated string that is the name of the vendor's
                                 variable.
  @param[in]       VendorGuid    A unique identifier for the vendor.
  @param[out]      Attributes    If not NULL, a pointer to the memory location to return the
                                 attributes bitmask for the variable.
  @param[in, out]  DataSize      On input, the size in bytes of the return Data buffer.
                                 On output the size of data returned in Data.
  @param[out]      Data          The buffer to return the contents of the variable. May be NULL
                                 with a zero DataSize in order to determine the size buffer needed.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval EFI_NOT_FOUND          The variable was not found.
  @retval EFI_BUFFER_TOO_SMALL   The DataSize is too small for the result.
  @retval EFI_INVALID_PARAMETER  VariableName is NULL.
  @retval EFI_INVALID_PARAMETER  VendorGuid is NULL.
  @retval EFI_INVALID_PARAMETER  DataSize is NULL.
  @retval EFI_INVALID_PARAMETER  The DataSize is not too small and Data is NULL.
  @retval EFI_DEVICE_ERROR       The variable could not be retrieved due to a hardware error.
  @retval EFI_SECURITY_VIOLATION The variable could not be retrieved due to an authentication failure.
  @retval EFI_UNSUPPORTED        This function is not implemented by this instance of the LibraryClass

**/
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


EFI_STATUS
EFIAPI
RestoreDevicePciConfig (
  VOID
  )
{
  EFI_STATUS               Status;
  UINTN                    VarDataSize;
  PCI_DEV_CONFIG_DATA      *PciConfigData;
  UINTN                    Index;
  UINTN                    RegIdx;
  UINTN                    PciAddress;
  UINT32                   PciRegVal;

  VarDataSize = 0;
  PciConfigData = NULL;

  //
  // Get PCI Configuration Space Setting from Variable
  //
  Status = VarLibGetVariable (CONFIG_DATA_VAR_NAME, &gPciDevConfigDataVariableGuid, NULL, &VarDataSize, NULL);
  if (Status != EFI_BUFFER_TOO_SMALL) {
    DEBUG((DEBUG_INFO, "[PciConfigRestorePei] Fail to find Variable %s, it may not exist\n", CONFIG_DATA_VAR_NAME));
    return Status;
  }

  PciConfigData = AllocateZeroPool(VarDataSize);
  if (PciConfigData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = VarLibGetVariable (
              CONFIG_DATA_VAR_NAME,
              &gPciDevConfigDataVariableGuid,
              NULL,
              &VarDataSize,
              PciConfigData
              );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_INFO, "[PciConfigRestorePei] Fail to get Variable\n"));
    return Status;
  }

  DEBUG((DEBUG_INFO, "[PciConfigRestorePei] %s variable Size 0x%x\n", CONFIG_DATA_VAR_NAME, VarDataSize));

  DumpConfigData(PciConfigData, VarDataSize / sizeof(PCI_DEV_CONFIG_DATA));

  //
  // Restore PCI Configuration Space
  //
  for (Index = 0; Index < VarDataSize / sizeof(PCI_DEV_CONFIG_DATA); Index++) {
    DEBUG((DEBUG_INFO, "[PciConfigRestorePei] Restore PciDev %02X:%0X.%X \n", PciConfigData[Index].Bus, PciConfigData[Index].Dev, PciConfigData[Index].Func));

    for (RegIdx = 0; RegIdx < SAVE_REG_COUNT; RegIdx ++) {
      PciAddress = PCI_LIB_ADDRESS(
                     PciConfigData[Index].Bus,
                     PciConfigData[Index].Dev,
                     PciConfigData[Index].Func,
                     PciConfigData[Index].PciRegOffset[RegIdx]
                     );

      //
      // Restore PCI Configuration Register only when needed
      //
      PciRegVal = PciRead32(PciAddress);
      if (PciRegVal != ReadUnaligned32(&PciConfigData[Index].PciRegVal[RegIdx])) {
        PciWrite32(
          PciAddress,
          ReadUnaligned32(&PciConfigData[Index].PciRegVal[RegIdx])
          );
      }
    }
  }

  //
  // For Debug, Dump PCI Configuration Space after Restore
  //
  DumpConfigDataFromPciDevice(PciConfigData, VarDataSize / sizeof(PCI_DEV_CONFIG_DATA));

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
DevicePciConfigRestoreEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = PeiServicesInstallPpi (&mPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  return Status;
}