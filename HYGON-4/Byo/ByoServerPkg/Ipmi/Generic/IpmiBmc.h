/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _EFI_IPMI_BMC_H_
#define _EFI_IPMI_BMC_H_

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiRuntimeLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/IpmiBaseLib.h>

#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/Smbios.h>

#include <IndustryStandard/SmBios.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/AcpiAml.h>

#include "EfiServerManagement.h"
#include "IpmiBmcCommon.h"
#include "EfiSmStatusCodes.h"

extern EFI_IPMI_BMC_INSTANCE_DATA             *mIpmiInstance;

#define BMC_KCS_TIMEOUT            5   // [s] Single KSC request timeout

//
// IPMI Instance signature
//
#define SM_IPMI_BMC_SIGNATURE SIGNATURE_32 ('i', 'p', 'm', 'i')

#define INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS(a) \
  CR ( \
  a, \
  EFI_IPMI_BMC_INSTANCE_DATA, \
  IpmiTransport, \
  SM_IPMI_BMC_SIGNATURE \
  )

EFI_STATUS
GetDeviceId (
  IN      EFI_IPMI_BMC_INSTANCE_DATA   *mIpmiInstance,
  IN      EFI_STATUS_CODE_VALUE        StatusCodeValue[ ],
  IN OUT  UINT8                        *ErrorCount
  );

EFI_STATUS
GetSelfTest (
  IN      EFI_IPMI_BMC_INSTANCE_DATA  *mIpmiInstance,
  IN      EFI_STATUS_CODE_VALUE       StatusCodeValue[  ],
  IN OUT  UINT8                       *ErrorCount
  );

EFI_STATUS
EfiIpmiSendCommand (
  IN      EFI_IPMI_TRANSPORT           *This,
  IN      UINT8                        NetFunction,
  IN      UINT8                        Lun,
  IN      UINT8                        Command,
  IN      UINT8                        *CommandData,
  IN      UINTN                        CommandDataSize,
  IN OUT  UINT8                        *ResponseData,
  IN OUT  UINT8                        *ResponseDataSize
  );

EFI_STATUS
EfiIpmiBmcStatus (
  IN EFI_IPMI_TRANSPORT                *This,
  OUT EFI_BMC_STATUS                   *BmcStatus,
  OUT EFI_SM_COM_ADDRESS               *ComAddress
  );

EFI_STATUS
EFIAPI
EfiIpmiBtSendCommand (
  IN      EFI_IPMI_TRANSPORT           *This,
  IN      UINT8                        NetFunction,
  IN      UINT8                        Lun,
  IN      UINT8                        Command,
  IN      UINT8                        *CommandData,
  IN      UINTN                        CommandDataSize,
  IN OUT  UINT8                        *ResponseData,
  IN OUT  UINT8                        *ResponseDataSize
  );

#endif
