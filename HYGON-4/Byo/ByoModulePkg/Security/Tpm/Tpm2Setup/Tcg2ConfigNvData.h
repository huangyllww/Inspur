/** @file
  Header file for NV data structure definition.

Copyright (c) 2015 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __TCG2_CONFIG_NV_DATA_H__
#define __TCG2_CONFIG_NV_DATA_H__

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/Tcg2ConfigHii.h>
#include <IndustryStandard/TcgPhysicalPresence.h>

#define TCG2_CONFIGURATION_VARSTORE_ID  0x0001
#define TCG2_CONFIGURATION_INFO_VARSTORE_ID     0x0002
#define TCG2_VERSION_VARSTORE_ID        0x0003
#define TCG2_CONFIGURATION_FORM_ID      0x0001

#define KEY_TPM_DEVICE                                 0x2000
#define KEY_TPM2_OPERATION                             0x2001
#define KEY_TPM2_OPERATION_PARAMETER            0x2002
#define KEY_TPM2_PCR_BANKS_REQUEST_0            0x2003
#define KEY_TPM2_PCR_BANKS_REQUEST_1            0x2004
#define KEY_TPM2_PCR_BANKS_REQUEST_2            0x2005
#define KEY_TPM2_PCR_BANKS_REQUEST_3            0x2006
#define KEY_TPM2_PCR_BANKS_REQUEST_4            0x2007
#define KEY_TPM_DEVICE_INTERFACE                       0x2008
#define KEY_TCG2_PPI_VERSION                    0x2009
#define KEY_TPM2_ACPI_REVISION                  0x200A

#define TPM_DEVICE_NULL           0
#define TPM_DEVICE_1_2            1
#define TPM_DEVICE_2_0_DTPM       2
#define TPM_DEVICE_2_0_HG_FTPM    3
#define TCM_DEVICE_TCM            4
#define TPM_DEVICE_UNPRESENT      5
#define TPM_DEVICE_MIN            TPM_DEVICE_1_2
#define TPM_DEVICE_MAX            TCM_DEVICE_TCM
#define TPM_DEVICE_DEFAULT        TPM_DEVICE_2_0_DTPM

#define TPM2_ACPI_REVISION_3       3
#define TPM2_ACPI_REVISION_4       4

#define TPM_DEVICE_INTERFACE_TIS       0
#define TPM_DEVICE_INTERFACE_PTP_FIFO  1
#define TPM_DEVICE_INTERFACE_PTP_CRB   2
#define TPM_DEVICE_INTERFACE_MAX       TPM_DEVICE_INTERFACE_PTP_FIFO
#define TPM_DEVICE_INTERFACE_DEFAULT   TPM_DEVICE_INTERFACE_PTP_CRB

#define TCG2_PPI_VERSION_1_2                    0x322E31  // "1.2"
#define TCG2_PPI_VERSION_1_3                    0x332E31  // "1.3"

//
// Nv Data structure referenced by IFR, TPM device user desired
//
typedef struct {
  UINT8   TpmDevice;
  UINT8   TpmState;
  UINT8   PlatformHierarchy;
  UINT8   StorageHierarchy;
  UINT8   EndorsementHierarchy;
  UINT8   PHRandomization;
} TCG2_CONFIGURATION;

typedef struct {
  UINT64  PpiVersion;
  UINT8   Tpm2AcpiTableRev;
} TCG2_VERSION;

typedef struct {
  BOOLEAN  Sha1Supported;
  BOOLEAN  Sha256Supported;
  BOOLEAN  Sha384Supported;
  BOOLEAN  Sha512Supported;
  BOOLEAN  Sm3Supported;
  UINT8    TpmDeviceInterfaceAttempt;
  BOOLEAN  TpmDeviceInterfacePtpFifoSupported;
  BOOLEAN  TpmDeviceInterfacePtpCrbSupported;
  BOOLEAN  ChangeEPSSupported;
  BOOLEAN  IsHideAcpiOption;
} TCG2_CONFIGURATION_INFO;

//
// Variable saved for S3, TPM detected, only valid in S3 path.
// This variable is ReadOnly.
//
typedef struct {
  UINT8   TpmDeviceDetected;
} TCG2_DEVICE_DETECTION;


#define TPM_SELECT_DTPM             0
#define TPM_SELECT_FTPM             1
#define TPM_SELECT_NULL             0xFF

#define TCG2_STORAGE_NAME           L"TCG2_CONFIGURATION"
#define TCG2_STORAGE_INFO_NAME      L"TCG2_CONFIGURATION_INFO"
#define TCG2_DEVICE_DETECTION_NAME  L"TCG2_DEVICE_DETECTION"
#define TCG2_VERSION_NAME           L"TCG2_VERSION"

#endif
