/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_HDD_PASSWORD_HOOK_PROTOCOL_H__
#define __BYO_HDD_PASSWORD_HOOK_PROTOCOL_H__


typedef struct _BYO_HDD_PASSWORD_HOOK_PROTOCOL  BYO_HDD_PASSWORD_HOOK_PROTOCOL;


typedef
EFI_STATUS
(EFIAPI *BUID_HDP_FROM_USER_INPUT) (
  IN BYO_HDD_PASSWORD_HOOK_PROTOCOL           *This,
  IN CHAR16                                   *PasswordStr,
  IN UINT8                                    *Sn,
  IN UINT8                                    *Password,
  IN UINT8                                    *PasswordData,
  IN UINT8                                    *NetworkId,
  IN UINT8                                    NetworkIdLen
  );

typedef
VOID
(EFIAPI *REPORT_HDD_PASSWORD) (
  IN BYO_HDD_PASSWORD_HOOK_PROTOCOL           *This,
  IN UINTN                                    HddIndex,
  IN UINT8                                    *Password
  );

#define BYO_HDD_PASSWORD_HOOK_PROTOCOL_REVISION_1_0  0x00010000

struct _BYO_HDD_PASSWORD_HOOK_PROTOCOL {
  ///
  /// Revision of the BYO_HDD_PASSWORD_HOOK_PROTOCOL. Consumer must check
  /// this protocol revision for compatibility concern.
  ///
  UINT32                              Revision;
  BUID_HDP_FROM_USER_INPUT            BuildHdpFromUserInput;
  REPORT_HDD_PASSWORD                 ReportHddPassword;
};

extern EFI_GUID gByoHddPasswordHookProtocolGuid;

#endif