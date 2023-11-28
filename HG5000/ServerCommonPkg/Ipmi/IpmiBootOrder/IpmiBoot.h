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
#include <Library/UefiDriverEntryPoint.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiBootOrder.h>


#define IPMI_STALL                       1000

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


#endif
