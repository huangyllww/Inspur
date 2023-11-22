/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __LEGACY_OPROM_INFO_H__
#define __LEGACY_OPROM_INFO_H__

typedef struct {
  UINT8  Bus;
  UINT8  Dev;
  UINT8  Func;
  UINT8  ClassCode[3];
  UINT8  *OpromData;
  UINTN  OpromDataSize;
} LEGACY_OPROM_INFO;


extern EFI_GUID gLegacyOptionRomInfoProtocolGuid;

#endif