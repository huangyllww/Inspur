/** @file
  The module entry point for Tcg2 configuration module.

Copyright (c) 2021 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "Tcg2ConfigImpl.h"

/**
  Initialize default data when TPM2.0 is not present.

  @param[in]  PrivateData        Private Datastructure

  @retval EFI_OUT_OF_RESOURCES   Fail to execute entry point due to lack of resources.
  @retval EFI_SUCCESS            All the related protocols are installed on the driver.
  @retval Others                 Fail to install protocols as indicated.

**/
EFI_STATUS
EFIAPI
InitializeTcg2UnpresentDefault (
  IN TCG2_CONFIG_PRIVATE_DATA *PrivateData
  )
{
  EFI_STATUS                    Status;
  TCG2_CONFIGURATION            Tcg2Configuration;
  TCG2_DEVICE_DETECTION         Tcg2DeviceDetection;
  EDKII_VARIABLE_LOCK_PROTOCOL  *VariableLockProtocol;
  UINTN                         DataSize;

  DataSize = sizeof(Tcg2Configuration);
  Status = gRT->GetVariable (
                  TCG2_STORAGE_NAME,
                  &gTcg2ConfigFormSetGuid,
                  NULL,
                  &DataSize,
                  &Tcg2Configuration
                  );
  if (EFI_ERROR (Status)) {
    //
    // Variable not ready, set default value
    //
    Tcg2Configuration.TpmState = 1;
    Tcg2Configuration.PlatformHierarchy = 1;
    Tcg2Configuration.StorageHierarchy = 1;
    Tcg2Configuration.EndorsementHierarchy = 1;
    Tcg2Configuration.PHRandomization = 1;
  }

  //
  // TPM State Disable, set default value to TPM_DEVICE_NULL
  //
  Tcg2DeviceDetection.TpmDeviceDetected = TPM_DEVICE_NULL;
  Tcg2Configuration.TpmDevice = Tcg2DeviceDetection.TpmDeviceDetected;

  //
  // TPM not present, set default value to TPM_DEVICE_UNPRESENT
  //
  if (!PcdGetBool (PcdTpm2ChipPresent)) {
    Tcg2Configuration.TpmDevice = TPM_DEVICE_UNPRESENT;
  }

  PrivateData->TpmDeviceDetected = Tcg2Configuration.TpmDevice;

  //
  // Save to variable so platform driver can get it.
  //
  Status = gRT->SetVariable (
                  TCG2_DEVICE_DETECTION_NAME,
                  &gTcg2ConfigFormSetGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof(Tcg2DeviceDetection),
                  &Tcg2DeviceDetection
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tcg2ConfigDriver: Fail to set TCG2_DEVICE_DETECTION_NAME\n"));
    Status = gRT->SetVariable (
                    TCG2_DEVICE_DETECTION_NAME,\
                    &gTcg2ConfigFormSetGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    0,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Save to variable so platform driver can get it.
  //
  Status = gRT->SetVariable (
                  TCG2_STORAGE_NAME,
                  &gTcg2ConfigFormSetGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  sizeof(Tcg2Configuration),
                  &Tcg2Configuration
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Tcg2ConfigDriver: Fail to set TCG2_STORAGE_NAME\n"));
  }

  //
  // We should lock Tcg2DeviceDetection, because it contains information needed at S3.
  //
  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **)&VariableLockProtocol);
  if (!EFI_ERROR (Status)) {
    Status = VariableLockProtocol->RequestToLock (
                                     VariableLockProtocol,
                                     TCG2_DEVICE_DETECTION_NAME,
                                     &gTcg2ConfigFormSetGuid
                                     );
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Install Tcg2 configuration form
  //
  Status = InstallTcg2ConfigForm (PrivateData);
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  return EFI_SUCCESS;

ErrorExit:
  if (PrivateData != NULL) {
    UninstallTcg2ConfigForm (PrivateData);
  }

  return Status;
}
