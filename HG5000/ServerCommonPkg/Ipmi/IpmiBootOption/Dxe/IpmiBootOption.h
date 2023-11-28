/** @file
#
#Copyright (c) 2017, Byosoft Corporation.<BR>
#All rights reserved.This software and associated documentation (if any)
#is furnished under a license and may only be used or copied in
#accordance with the terms of the license. Except as permitted by such
#license, no part of this software or documentation may be reproduced,
#stored in a retrieval system, or transmitted in any form or by any
#means without the express written consent of Byosoft Corporation.
#
#File Name:
#  IpmiBootOption.h
#
#Abstract:
#  ServerMgmt features via IPMI commands (IPMI2.0 Spc rev1.1 2013)
#
#Revision History:
#
#TIME:       2017-7-5
#$AUTHOR:    Phinux Qin
#$REVIEWERS:
#$SCOPE:     All BMC Platforms
#$TECHNICAL:
#
#
#$END--------------------------------------------------------------------
#
--*/

#ifndef _IPMI_BOOT_OPTIONS_H_
#define _IPMI_BOOT_OPTIONS_H_

#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiBootOptionProtocol.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Library/PlatformCommLib.h>

#pragma pack(1)
typedef struct {
  UINT16     BbsIndex;
} LEGACY_BOOT_OPTION_BBS_DATA;
#pragma pack()

/**
  Get Ipmi System Boot Option parameter5 and overrive local boot option

  @param This               Pointer to IPMI_BOOT_OPTIONS_PROTOCOL

  @retval EFI_SUCCESS        Reset successfully
  @retval EFI_DEVICE_ERROR   Failed to reset

**/
EFI_STATUS
EFIAPI
ProcessIpmiSystemBootOptionParameter5 (
  IN IPMI_BOOT_OPTIONS_PROTOCOL         *This
  );
/**

    GC_TODO: add routine description

    @param BootOptionParam5     - GC_TODO: add arg description
    @param BmcUsbPortsInfo      - GC_TODO: add arg description

    @retval EFI_INVALID_PARAMETER - GC_TODO: add retval description
    @retval EFI_NOT_FOUND         - GC_TODO: add retval description
    @retval Status                - GC_TODO: add retval description

**/
EFI_STATUS
OverrideBootVars (
  IN SYSTEM_BOOT_OPTION_PARAMETER5  *BootOptionParam5,
  IN UINT32                         BmcUsbPortsInfo
  );

/**

    GC_TODO: add routine description

**/
VOID
SetBootVarsFromLastPersistent (
  VOID
  );

/**

    GC_TODO: add routine description

**/
VOID
FreeIpmiBootOptionStruct (
  VOID
  );
#endif
