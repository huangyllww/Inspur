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
#include <Library/SystemPasswordLib.h>


EFI_STATUS HandleIfResetPasswordRequest()
{
  EFI_STATUS        Status;

  UINTN             VariableSize;
  SYSTEM_PASSWORD   SetupPassword;


  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = mSmmVariable->SmmGetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  (VOID **)&SetupPassword
                  );
  if(EFI_ERROR(Status)) {
    Status = EFI_NOT_FOUND;
  }
  SetupPassword.bHaveAdmin = FALSE;
  ZeroMem(SetupPassword.AdminHash, SYSTEM_PASSWORD_HASH_LENGTH);
  SetupPassword.bHavePowerOn = FALSE;
  ZeroMem(SetupPassword.PowerOnHash, SYSTEM_PASSWORD_HASH_LENGTH);
  SetupPassword.ChangePopByUser = 1;
  SetupPassword.VerifyTimes = 0;

  if (!SetupPassword.bHaveAdmin && !SetupPassword.bHavePowerOn) {
    SetupPassword.EnteredType = LOGIN_USER_ADMIN;
  }

  Status = mSmmVariable->SmmSetVariable (
                SYSTEM_PASSWORD_NAME,
                &gEfiSystemPasswordVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
                VariableSize,
                (VOID **)&SetupPassword
                );
  if(EFI_ERROR(Status)) {
    Status = EFI_ABORTED;
  } 
  
  return Status;
}