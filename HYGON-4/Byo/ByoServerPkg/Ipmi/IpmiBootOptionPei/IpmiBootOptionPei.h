/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiBootOptionPei.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _BDS_IPMI_H
#define _BDS_IPMI_H

#include <Base.h>
#include <PiPei.h>
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiBaseLib.h>
#include <Library/ByoCommLib.h>
#include <Library/TimerLib.h>
#include <Protocol/IpmiBootOrder.h>
#include <Ppi/ReadOnlyVariable2.h>


#define IPMI_STALL                       1000

#define IPMI_BUFFER_SIZE                 50

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

#endif
