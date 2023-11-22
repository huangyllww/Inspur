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

#ifndef _FRB_GUID_H_
#define _FRB_GUID_H_

//
// FRB Common Structures
//
typedef enum {
  EfiFrbReserved,
  EfiFrb2,
  EfiBiosPost,
  EfiOsBootWdt,
  EfiFrbReserved1,
  EfiFrbReserved2,
  EfiFrbReserved3,
  EfiFrbReserved4,
  EfiFrb1NotSupported,
  EfiFrb3NotSupported,
  EfiFrbReservedLast
} EFI_FRB_TYPE;

typedef enum {
  NoTimeoutInterrupt,
  SystemManagementInterrupt,
  DiagnosticInterrupt,
  MessagingInterrupt
} FRB_SYSTEM_TIMEOUT_INTERRUPT;

typedef enum {
  EfiNormalBoot,
  EfiHardReset,
  EfiPowerDown,
  EfiPowerCycle,
  EfiFrbSysReserved1,
  EfiFrbSysReserved2,
  EfiFrbSysReserved3,
  EfiFrbSysReserved4,
  EfiDiagnosticMode,
  EfiPlatformRecovery
} FRB_SYSTEM_BOOT_POLICY;

#endif
