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

#ifndef _OS_WDT_POLICY_H_
#define _OS_WDT_POLICY_H_

typedef struct _EFI_OS_WDT_POLICY_PROTOCOL  EFI_OS_WDT_POLICY_PROTOCOL;

typedef struct {
  UINT8       OsWdtAction;
  UINTN       OsWdtTimeOut;
  BOOLEAN     OsWdtFlag;
} EFI_OS_WDT_POLICY_RECORD;

typedef
EFI_STATUS
(EFIAPI *EFI_OS_WDT_SET_POLICY) (
  IN EFI_OS_WDT_POLICY_PROTOCOL   *This,
  IN EFI_OS_WDT_POLICY_RECORD     *PolicyRecord
   );

typedef
EFI_STATUS
(EFIAPI *EFI_OS_WDT_GET_POLICY) (
  IN EFI_OS_WDT_POLICY_PROTOCOL     *This,
  IN OUT EFI_OS_WDT_POLICY_RECORD   *PolicyRecord
   );


struct _EFI_OS_WDT_POLICY_PROTOCOL {
  EFI_OS_WDT_SET_POLICY         OsWdtSetPolicy;
  EFI_OS_WDT_GET_POLICY         OsWdtGetPolicy;
};

extern EFI_GUID   gEfiOsWdtPolicyProtocolGuid;

#endif
