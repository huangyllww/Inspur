/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
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

#ifndef __EFI_IPMIBOOT_PROTOCOL_H__
#define __EFI_IPMIBOOT_PROTOCOL_H__

#define EFI_IPMI_BOOT_GUID \
  { \
    0x5c9b75ec, 0x8ec7, 0x45f2, 0x8f, 0x8f, 0xc1, 0xd8, 0x8f, 0x3b, 0x93, 0x45 } \
  }

//
// Parameter 5 data 2 Force Boot Device Selection
//
typedef enum {
  ForceBootNone = 0,
  ForceBootPxe,
  ForceBootHardDrive,
  ForceBootHardDriveSafeMode, // not supported
  ForceBootServicePartition,        // not supported
  ForceBootCdDvd,
  ForceBootBiosSetup,       // defined in IPMI2.0, reserved in IPMI 1.5
  ForceBootRemoteFloppy,
  ForceBootRemoteCdDvd,
  ForceBootEfiShell = 0x9,
  ForceBootRemoteHdd = 0xb,

  ForceBootFloppy = 0xf
} IPMI_FORCE_BOOT_DEVICE;

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
  UINT32                  BiosBootType:1;
  UINT32                  ReservedBits:22;
  UINT8                   ServicePartitionSelector;
} IPMI_FORCE_BOOT_OPTIONS;

typedef EFI_STATUS (*PROCESS_BOOT_FLAGS) ( IN OUT UINT8 *BootType);


typedef struct _IPMI_BOOT_PROTOCOL { 
  PROCESS_BOOT_FLAGS          ProcessBootFlags;
} IPMI_BOOT_PROTOCOL;

extern EFI_GUID gEfiIpmiBootGuid;

#endif
