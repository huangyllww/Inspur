/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BmcWdt.h

Abstract:
  Source file for the BMC watchdog timer.

Revision History:

**/



//
// Statements that include other files
//
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/EventGroup.h>


#include <IpmiDefinitions.h>
#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/BmcWdtProtocol.h>

#ifndef _EFI_BMCWATCHDOG_H_
#define _EFI_BMCWATCHDOG_H_

//
// BMC version instance data
//

typedef struct {
  UINTN                       Signature;
  EFI_BMC_WDT_PROTOCOL        Protocol;
  EFI_BMC_WDT_DATA            Data;
} EFI_BMC_WATCHDOG_INSTANCE;

#define SM_BMC_WATCHDOG_SIGNATURE SIGNATURE_32 ('b', 'm', 'w', 'd')

#define INSTANCE_FROM_EFI_BMC_WATCHDOG_THIS(a) CR(a, EFI_BMC_WATCHDOG_INSTANCE, Protocol, SM_BMC_WATCHDOG_SIGNATURE)

extern EFI_GUID gEfiSetupEnterGuid;
extern EFI_GUID gEfiBootMenuEnterGuid;
extern EFI_GUID gEfiFrb2WatchDogNotifyGuid;
#endif