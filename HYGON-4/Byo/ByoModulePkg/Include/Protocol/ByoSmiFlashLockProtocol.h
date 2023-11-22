/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_SMIFLASH_LOCK_PROTOCOL_H__
#define __BYO_SMIFLASH_LOCK_PROTOCOL_H__

typedef struct _BYO_SMIFLASH_LOCK_PROTOCOL  BYO_SMIFLASH_LOCK_PROTOCOL;



#define BYO_SMIFLASH_STS_ALL_LOCK         BIT0 // this one is enable bit. If it is set, it means all tool locks.
#define BYO_SMIFLASH_STS_FLASH_LOCK       BIT1 // this one is for byoflash lock
#define BYO_SMIFLASH_STS_SMBIOS_LOCK      BIT2 // this one is for byodmi lock
#define BYO_SMIFLASH_STS_CFG_LOCK         BIT3 // this one is for byocfg lock

#define BYO_SMIFLASH_STS_MASK             (BYO_SMIFLASH_STS_ALL_LOCK|BYO_SMIFLASH_STS_SMBIOS_LOCK)

typedef
EFI_STATUS
(EFIAPI *BYO_SMIFLASH_GET_LOCK_STATUS)(
  IN  BYO_SMIFLASH_LOCK_PROTOCOL     *This,
  OUT UINTN                          *LockStatus
  );

typedef
EFI_STATUS
(EFIAPI *BYO_SMIFLASH_UNLOCK)(
  IN BYO_SMIFLASH_LOCK_PROTOCOL      *This,
  IN VOID                            *Buffer,
  IN UINTN                           BufferSize
  );

typedef
EFI_STATUS
(EFIAPI *BYO_SMIFLASH_LOCK)(
  IN BYO_SMIFLASH_LOCK_PROTOCOL      *This
  );

typedef
EFI_STATUS
(EFIAPI *BYO_SMIFLASH_SMBIOS_UPDATE_FILTER)(
  IN BYO_SMIFLASH_LOCK_PROTOCOL      *This,
  IN UINT8                           Type,
  IN UINT8                           Offset
  );



struct _BYO_SMIFLASH_LOCK_PROTOCOL {
  BYO_SMIFLASH_GET_LOCK_STATUS       GetLockStatus;
  BYO_SMIFLASH_UNLOCK                Unlock;
  BYO_SMIFLASH_LOCK                  Lock;
  BYO_SMIFLASH_SMBIOS_UPDATE_FILTER  SmbiosFilter;
};


extern EFI_GUID gByoSmiFlashLockProtocolGuid;

#endif

