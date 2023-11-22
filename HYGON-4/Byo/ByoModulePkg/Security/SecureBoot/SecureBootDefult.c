/** @file

  Copyright (c) 2020, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

  File Name:
    SecureBootDefault.c

  Abstract:
    Enable secure boot by default.

  Revision History:

  TIME:
  $AUTHOR:
  $REVIEWERS:
  $SCOPE:
  $TECHNICAL:


  T.O.D.O

  $END

**/

#include "SecureBootDefult.h"
#include <Library/PcdLib.h>

EFI_STATUS
EFIAPI
SecureBootDefaultEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  UINTN                         VarSize;
  EFI_STATUS                    Status;
  UINT8                         SecureBootMode;

  UINT16                        ProvisionDefaultKey;

  VarSize = sizeof (UINT8);

  Status = gRT->GetVariable (
                  EFI_SECURE_BOOT_MODE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &VarSize,
                  &SecureBootMode
                  );
  DEBUG((DEBUG_INFO, "'SecureBoot' Var Value %x\n", SecureBootMode));

  ASSERT_EFI_ERROR(Status);
  
  // 
  // Determine whether the Default All Key is required by obtaining the value of PcdPlatformDefaultKey  
  //
  ProvisionDefaultKey = PcdGet16 (PcdPlatformDefaultKey);

  if ((!EFI_ERROR (Status)) && (SecureBootMode == 0)&&(ProvisionDefaultKey == 0x1)) {
    //
    // Force SecureBootMode Enable in case that SecureBoot is not turned on
    //
    AuthVarMfgReset ();
  }

  return EFI_SUCCESS;
}
