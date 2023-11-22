/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __ENUM_LEGACY_BOOT_OPTIONS_HOOK_H__
#define __ENUM_LEGACY_BOOT_OPTIONS_HOOK_H__

#include <Protocol/LegacyBios.h>

#define ENUM_LEGACY_BOOT_OPTIONS_HOOK_SIGN     SIGNATURE_32('L', 'B', 'O', 'H')

typedef struct {
  UINT32                          Sign;
  BBS_TABLE                       *BbsTable;
  UINT16                          BbsCount;
  UINT16                          *BbsIndexList;
  UINT16                          *BbsIndexCount;
} ENUM_LEGACY_BOOT_OPTIONS_HOOK_PARAMETER;

extern EFI_GUID gEnumLegacyBootOptionsHookGuid;

#endif
