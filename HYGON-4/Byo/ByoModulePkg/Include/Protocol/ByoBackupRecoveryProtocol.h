/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_BACKUP_RECOVERY_PROTOCOL_H__
#define __BYO_BACKUP_RECOVERY_PROTOCOL_H__

typedef struct _BYO_BACKUP_RECOVERY_PROTOCOL  BYO_BACKUP_RECOVERY_PROTOCOL;

typedef
VOID
(EFIAPI *BYO_BACKUP_BIOS)(
  IN  BOOLEAN                        BackupToFlash
  );

typedef
VOID
(EFIAPI *BYO_RECOVERY_BIOS)(
  IN  BOOLEAN                        RecoveryFormFlash
  );

struct _BYO_BACKUP_RECOVERY_PROTOCOL {
  BYO_BACKUP_BIOS              BackupBios;
  BYO_RECOVERY_BIOS            RecoveryBios;
};

extern EFI_GUID gByoBackupRecoveryProtocolGuid;
#endif           // _BYO_BACKUP_RECOVERY_PROTOCOL