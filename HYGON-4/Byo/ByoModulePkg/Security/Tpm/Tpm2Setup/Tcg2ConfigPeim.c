/** @file
  The module entry point for Tcg2 configuration module.

Copyright (c) 2015 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/


#include <PiPei.h>

#include <Guid/TpmInstance.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>

#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/TpmInitialized.h>
#include <Protocol/Tcg2Protocol.h>

#include "Tcg2ConfigNvData.h"
#include "Tcg2Internal.h"

TPM_INSTANCE_ID  mTpmInstanceId[] = TPM_INSTANCE_ID_LIST;

CONST EFI_PEI_PPI_DESCRIPTOR gTpmSelectedPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiTpmDeviceSelectedGuid,
  NULL
};

EFI_PEI_PPI_DESCRIPTOR  mTpmInitializationDonePpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiTpmInitializationDonePpiGuid,
  NULL
};

/**
  This routine check both SetupVariable and real TPM device, and return final TpmDevice configuration.

  @param  SetupTpmDevice  TpmDevice configuration in setup driver

  @return TpmDevice configuration
**/
UINT8
DetectTpmDevice (
  IN UINT8 SetupTpmDevice
  );

/**
  The entry point for Tcg2 configuration driver.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS            Convert variable to PCD successfully.
  @retval Others                 Fail to convert variable to PCD.
**/
EFI_STATUS
EFIAPI
Tcg2ConfigPeimEntryPoint (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINTN                           Size;
  EFI_STATUS                      Status;
  EFI_STATUS                      Status2;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *VariablePpi;
  TCG2_CONFIGURATION              Tcg2Configuration;
  UINTN                           Index;
  UINT8                           TpmDevice;

  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **) &VariablePpi);
  ASSERT_EFI_ERROR (Status);

  Size = sizeof(Tcg2Configuration);
  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          TCG2_STORAGE_NAME,
                          &gTcg2ConfigFormSetGuid,
                          NULL,
                          &Size,
                          &Tcg2Configuration
                          );
  if (EFI_ERROR (Status)) {
    //
    // Variable not ready, set default value
    //
    Tcg2Configuration.TpmDevice            = TPM_DEVICE_DEFAULT;
    Tcg2Configuration.TpmState             = 1;
    Tcg2Configuration.PlatformHierarchy    = 1;
    Tcg2Configuration.StorageHierarchy     = 1;
    Tcg2Configuration.EndorsementHierarchy = 1;
    Tcg2Configuration.PHRandomization      = 1;
  }

  //
  // Validation
  //
  if ((Tcg2Configuration.TpmDevice > TPM_DEVICE_MAX) || (Tcg2Configuration.TpmDevice < TPM_DEVICE_MIN)) {
    Tcg2Configuration.TpmDevice = TPM_DEVICE_DEFAULT;
  }

  //
  // Although we have SetupVariable info, we still need detect TPM device manually.
  //
  DEBUG ((EFI_D_INFO, "Tcg2Configuration.TpmDevice from Setup: %x\n", Tcg2Configuration.TpmDevice));

  if (PcdGetBool (PcdTpmAutoDetection)) {
    TpmDevice = DetectTpmDevice (Tcg2Configuration.TpmDevice);
    DEBUG ((EFI_D_INFO, "TpmDevice final: %x\n", TpmDevice));
    if (TpmDevice == TPM_DEVICE_NULL || TpmDevice == TCM_DEVICE_TCM) {
      PcdSetBoolS (PcdTpm2ChipPresent, FALSE);
    }
    if (TpmDevice != TPM_DEVICE_NULL && TpmDevice != TCM_DEVICE_TCM) {
      Tcg2Configuration.TpmDevice = TpmDevice;
      PcdSetBoolS (PcdTpm2ChipPresent, TRUE);
    }
  } else {
    TpmDevice = Tcg2Configuration.TpmDevice;
  }

  //
  // Convert variable to PCD.
  // This is work-around because there is no guarantee DynamicHiiPcd can return correct value in DXE phase.
  // Using DynamicPcd instead.
  //
  // NOTE: Tcg2Configuration variable contains the desired TpmDevice type,
  // while PcdTpmInstanceGuid PCD contains the real detected TpmDevice type
  //
  for (Index = 0; Index < sizeof(mTpmInstanceId)/sizeof(mTpmInstanceId[0]); Index++) {
    if (TpmDevice == mTpmInstanceId[Index].TpmDevice) {
      Size = sizeof(mTpmInstanceId[Index].TpmInstanceGuid);
      Status = PcdSetPtrS (PcdTpmInstanceGuid, &Size, &mTpmInstanceId[Index].TpmInstanceGuid);
      ASSERT_EFI_ERROR (Status);
      DEBUG ((EFI_D_INFO, "TpmDevice PCD: %g\n", &mTpmInstanceId[Index].TpmInstanceGuid));
      break;
    }
  }

  //
  // Update PcdTpmInstanceGuid PCD based on TCG2_STORAGE_NAME Variable
  //
  if (Tcg2Configuration.TpmState == 0) {
    Size = sizeof(gEfiTpmDeviceInstanceNoneGuid);
    Status = PcdSetPtrS (PcdTpmInstanceGuid, &Size, &gEfiTpmDeviceInstanceNoneGuid);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Set Hierarchy pcds based on TCG2_STORAGE_NAME Variable
  //
  if (Tcg2Configuration.PlatformHierarchy) {
    PcdSetBoolS (PcdTpm2HierarchyChangeAuthPlatform, TRUE);
  } else {
    PcdSetBoolS (PcdTpm2HierarchyChangeAuthPlatform, FALSE);
  }

  if (Tcg2Configuration.StorageHierarchy) {
    PcdSetBoolS (PcdTpm2HierarchyControlOwner, TRUE);
  } else {
    PcdSetBoolS (PcdTpm2HierarchyControlOwner, FALSE);
  }

  if (Tcg2Configuration.EndorsementHierarchy) {
    PcdSetBoolS (PcdTpm2HierarchyControlEndorsement, TRUE);
  } else {
    PcdSetBoolS (PcdTpm2HierarchyControlEndorsement, FALSE);
  }

  if (Tcg2Configuration.PHRandomization) {
    PcdSetBoolS (PcdTpm2HierarchyControlPlatform, TRUE);
  } else {
    PcdSetBoolS (PcdTpm2HierarchyControlPlatform, FALSE);
  }

  //
  // Selection done
  //
  Status = PeiServicesInstallPpi (&gTpmSelectedPpi);
  ASSERT_EFI_ERROR (Status);

  //
  // Even if no TPM is selected or detected, we still need install TpmInitializationDonePpi.
  // Because TcgPei or Tcg2Pei will not run, but we still need a way to notify other driver.
  // Other driver can know TPM initialization state by TpmInitializedPpi.
  //
  if (CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceNoneGuid)) {
    Status2 = PeiServicesInstallPpi (&mTpmInitializationDonePpiList);
    ASSERT_EFI_ERROR (Status2);
  }

  return Status;
}
