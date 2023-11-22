/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiBoot.h

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
#include <Library/IpmiBaseLib.h>
#include <Library/ByoCommLib.h>
#include <Library/HobLib.h>
#include <Protocol/IpmiBootOrder.h>
#include <ByoBootGroup.h>

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

EFI_STATUS 
EFIAPI
EfiBootManagerBootApp(
  IN EFI_GUID *AppGuid
  );

EFI_STATUS 
EFIAPI
EfiBootManagerBootHdd(
  VOID
  );

EFI_STATUS 
EFIAPI
EfiBootManagerBootUsb(
  VOID
  );

BM_MENU_TYPE
GetUefiBootGroupType(
  EFI_DEVICE_PATH_PROTOCOL *FilePath
  );
EFI_STATUS 
EFIAPI
BootManagerBootUefiGroup(
  BM_MENU_TYPE GroupType
  );

BM_MENU_TYPE
GetLegacyBootGroupType(
  EFI_DEVICE_PATH_PROTOCOL *FilePath,
  UINT8 *OptionalData
  );

EFI_STATUS 
EFIAPI
BootManagerBootLegacyGroup(
  BM_MENU_TYPE GroupType
  );

#endif
