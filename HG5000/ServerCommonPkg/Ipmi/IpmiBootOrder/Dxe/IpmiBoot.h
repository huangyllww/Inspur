/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _BDS_IPMI_H
#define _BDS_IPMI_H

#include "PiDxe.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>

#include "EfiServerManagement.h"

#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiBootOrder.h>
#include <Guid/LegacyDevOrder.h>
#include <Guid/GlobalVariable.h>
#include <Guid/DebugMask.h>


#define IPMI_STALL                       1000

#define BOOT_INFO_HANDLED_BY_BIOS        (1 << 0)  // BIT0
#define BOOT_INFO_HANDLED_BY_OS_LOADER   (1 << 1)  // BIT1
#define BOOT_INFO_HANDLED_BY_OS          (1 << 2)  // BIT2
#define BOOT_INFO_HANDLED_BY_SMS         (1 << 3)  // BIT3
#define BOOT_INFO_HANDLED_BY_OEM         (1 << 4)  // BIT4

#define VAR_FLAG  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE

//
// Get Boot Options parameter types
//
typedef enum {
  ParmTypeSetInProgress = 0,
  ParmTypeServicePartitionSelector,
  ParmTypeServicePartitionScan,
  ParmTypeBootFlagValid,
  ParmTypeBootInfoAck,
  ParmTypeBootFlags,
  ParmTypeBootInitiatorInfo,
  ParmTypeBootInitiatorMailbox,
  ParmTypeMax
} BOOT_OPTION_PARAMETER_TYPE;

//
// Parameter 5 data 2 Force Boot Device Selection
//
typedef enum {
  ForceBootNone = 0,
  ForceBootPxe,
  ForceBootLegacyHardDrive,
  ForceBootLegacyHardDriveSafeMode, // not supported
  ForceBootServicePartition,        // not supported
  ForceBootLegacyCdDvd,
  ForceBootBiosSetup,       // defined in IPMI2.0, reserved in IPMI 1.5
                            // Arbitrary gap for additions to IPMI
  ForceBootEfiShell = 0x9,	// EPSD defined (non-standard use, formerly == 0x6)

  ForceBootLegacyFloppy = 0xf
} IPMI_FORCE_BOOT_DEVICE;


#pragma pack(1)
typedef union {
  UINT8 AsUint8;
  struct {
    UINT8 BlankVideo : 1;
    UINT8 WriteProtectDiskette : 1;
    UINT8 LockNmiButton : 1;
    UINT8 LockSleepButton : 1;
    UINT8 LockResetButton : 1;
    UINT8 LockPowerButton : 1;
    UINT8 LockFrontPanel : 1;
    UINT8 ForceSecureMode : 1;
  } s;
} SECURE_MODE_OPTIONS;

typedef struct {
  SECURE_MODE_OPTIONS Options;
} SET_SECURE_MODE_OPTIONS;

#pragma pack()
//
// IPMI force boot option 
//
typedef struct {
  SECURE_MODE_OPTIONS     SecureMode;
  IPMI_FORCE_BOOT_DEVICE  BootDevice;
  UINT32                  LockKeyboard:1;
  UINT32                  CmosClear:1;
  UINT32                  UserPasswordBypass:1;
  UINT32                  ConsoleRedirection:2;
  UINT32                  BiosVerbosity:2;
  UINT32                  PersistentOptions:1;
  UINT32                  BootValid:1;
  UINT8                   ServicePartitionSelector;
} IPMI_FORCE_BOOT_OPTIONS;

BOOLEAN
PlatformUpdateSmBootOrder (
  VOID
  );

extern 
BOOLEAN
IsInternalShell(
  IN  EFI_DEVICE_PATH_PROTOCOL          *FilePath
  );

EFI_STATUS
ProcessIpmiBootFlags (
  VOID
  );

EFI_STATUS
ProcessIpmiF2BootOrderTable (
  VOID
  );

EFI_STATUS
ProcessIpmiBootOrderTable (
  VOID
  );

EFI_STATUS
IpmiSetBootOptions (
  IN  BOOT_OPTION_PARAMETER_TYPE       ParameterType,
  OUT BOOT_OPTIONS_PARAMETERS          *RequestParameters
  );

EFI_STATUS
IpmiGetBootOptions (
  IN  BOOT_OPTION_PARAMETER_TYPE      ParameterType,
  OUT BOOT_OPTIONS_PARAMETERS         *ResponseParameters
  );

EFI_STATUS
IpmiWaitSetInProgressClear (
  VOID
  );
  
EFI_STATUS
FindEfiBootVariableByType(
  UINT16                      DeviceType,
  UINTN                       *BootOrder
  );

EFI_STATUS
GetForceBootOptions(
  OUT IPMI_FORCE_BOOT_OPTIONS *IpmiForceBootOptions
  );

VOID
UpdateBootOrderWithBootNext (
  VOID
  );

#endif
