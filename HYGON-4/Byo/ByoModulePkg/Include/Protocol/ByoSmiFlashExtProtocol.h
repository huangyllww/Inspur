/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_SMIFLASH_EXT_PROTOCOL_H__
#define __BYO_SMIFLASH_EXT_PROTOCOL_H__

#include <ByoSmiFlashInfo.h>
#include <Protocol/NvMediaAccess.h>


#define BYO_SMIFLASH_EXT_PROTOCOL_GUID \
  { \
    0xabf7e520, 0xa85e, 0x4a3a, { 0xa4, 0xca, 0x8c, 0xbb, 0x64, 0xe9, 0x59, 0x74 } \
  }

typedef struct _BYO_SMIFLASH_EXT_PROTOCOL  BYO_SMIFLASH_EXT_PROTOCOL;

// return:
//   FALSE - this sub function is not my own.
//   TRUE  - this sub function is my own, return status maybe not SUCCESS, 
//           caller should use parameter "RetStatus" as sub function return value.
typedef
BOOLEAN
(EFIAPI *BYO_SMIFLASH_EXT_FUNCTION)(
  IN  SMI_INFO                     *SmiFlashInfo,
  IN  NV_MEDIA_ACCESS_PROTOCOL     *MediaAccess,
  OUT EFI_STATUS                   *RetStatus
  );

struct _BYO_SMIFLASH_EXT_PROTOCOL {
  BYO_SMIFLASH_EXT_FUNCTION       ByoSmiFlashExtFunc;
};


extern EFI_GUID gByoSmiFlashExtProtocolGuid;

#endif
