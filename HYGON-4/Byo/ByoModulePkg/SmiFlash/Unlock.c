/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "SmiFlash.h"
#include <Protocol/ByoSmiFlashLockProtocol.h>
#include <SystemPasswordVariable.h>

EFI_STATUS HandleIfUnlockRequest(ROM_HOLE_PARAMETER *Param)
{
  EFI_STATUS                   Status;
  BYO_SMIFLASH_LOCK_PROTOCOL   *SmiFlashLock;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = gSmst->SmmLocateProtocol (
                    &gByoSmiFlashLockProtocolGuid,
                    NULL,
                    (VOID**)&SmiFlashLock
                    );
  if(!EFI_ERROR(Status)){ 
    Status = SmiFlashLock->Unlock(SmiFlashLock, (VOID*)(UINTN)Param->Buffer, Param->BufferSize);
  } else {
    Status = UnLock();
  }

  return Status;
}


EFI_STATUS HandleIfLockRequest()
{
  EFI_STATUS                   Status;
  BYO_SMIFLASH_LOCK_PROTOCOL   *SmiFlashLock;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = gSmst->SmmLocateProtocol (
                    &gByoSmiFlashLockProtocolGuid,
                    NULL,
                    (VOID**)&SmiFlashLock
                    );
  if(!EFI_ERROR(Status)){ 
    Status = SmiFlashLock->Lock(SmiFlashLock);
  } else {
    Status = Lock();
  }

  return Status;
}

EFI_STATUS HandleIfGetLockStatus(UINTN *LockStatus)
{
  EFI_STATUS                   Status;
  BYO_SMIFLASH_LOCK_PROTOCOL   *SmiFlashLock;


  DEBUG((DEBUG_VERBOSE, "%a\n", __FUNCTION__));

  Status = gSmst->SmmLocateProtocol (
                    &gByoSmiFlashLockProtocolGuid,
                    NULL,
                    (VOID**)&SmiFlashLock
                    );
  if(!EFI_ERROR(Status)){ 
    Status = SmiFlashLock->GetLockStatus(SmiFlashLock, LockStatus);
  } else {
    Status = GetLockStatus(LockStatus);
  }

  return Status;
}

EFI_STATUS
EFIAPI
Lock(
  )
{
  UINT32 Attributes;
  UINTN Data;
  UINTN Size;
  Data = 1;
  Size = sizeof(UINT32);
  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
  return mSmmVariable->SmmSetVariable(FLASH_LOCK_STATUS_NAME,
                    &gEfiFlashLockStatusVariableGuid,
                    Attributes,
                    Size,
                    &Data
                    );

}

EFI_STATUS
EFIAPI
UnLock(
  )
{
  UINT32 Attributes;
  UINTN Data;
  UINTN Size;
  Data = 0;
  Size = sizeof(UINT32);
  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
  return mSmmVariable->SmmSetVariable(FLASH_LOCK_STATUS_NAME,
                    &gEfiFlashLockStatusVariableGuid,
                    Attributes,
                    Size,
                    &Data
                    );

}

EFI_STATUS
EFIAPI
GetLockStatus(
  UINTN *LockStatus
  )
{
  EFI_STATUS Status;
  UINTN Data;
  UINTN Size;
  UINT32 Attributes;
  Data = 0;
  Size = sizeof(UINT32);
  Attributes = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS;
  Status = mSmmVariable->SmmGetVariable(FLASH_LOCK_STATUS_NAME,
                    &gEfiFlashLockStatusVariableGuid,
                    NULL,
                    &Size,
                    &Data
                    );
  if (EFI_ERROR(Status)) {
    Data = 0;
    Size = sizeof(UINT32);
    Status = mSmmVariable->SmmSetVariable(FLASH_LOCK_STATUS_NAME,
                  &gEfiFlashLockStatusVariableGuid,
                  Attributes,
                  Size,
                  &Data
                  );
  } else {
    *LockStatus = Data;
  }

  return Status;
}
