/** @file

  Copyright (c) 2022, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

  Defines Hob extend GUID to record capsule info loaded from media for
  crisis recovery

  These are contracts between the recovery module and device recovery module
  that convey the name of a given recovery module type.

**/

#ifndef __CAPSULE_RECORD__H
#define __CAPSULE_RECORD__H

#include <Library/BiosIdLib.h>

#define MAX_CAPUSLE_NUMBER 1

typedef struct {
  EFI_PHYSICAL_ADDRESS    BaseAddress;
  UINT64                  Length;
  BIOS_ID_IMAGE           BiosIdImage;
  EFI_GUID                DeviceId;
  BOOLEAN                 Actived;
} CAPSULE_INFO;

typedef struct {
  CAPSULE_INFO        CapsuleInfo[8];
  UINT8               CapsuleCount;
} CAPSULE_RECORD;

extern EFI_GUID gRecoveryCapsuleRecordGuid;

#endif

