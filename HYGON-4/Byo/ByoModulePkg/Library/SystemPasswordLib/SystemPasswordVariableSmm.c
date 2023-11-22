/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPasswordVariableSmm.c

Abstract:
  System password library SMM version

Revision History:

**/

#include <Protocol/SmmVariable.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>

STATIC EFI_SMM_VARIABLE_PROTOCOL  *mSmmVariable = NULL;
extern UINT8                      gRecoederTime;

EFI_STATUS
EFIAPI
PasswordGetVariable (
  IN     CHAR16                      *VariableName,
  IN     EFI_GUID                    *VendorGuid,
  OUT    UINT32                      *Attributes,    OPTIONAL
  IN OUT UINTN                       *DataSize,
  OUT    VOID                        *Data           OPTIONAL
  )
{
  EFI_STATUS Status;

  //
  // Locate SmmVariableProtocol
  //
  if (mSmmVariable == NULL) {
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmVariableProtocolGuid,
                      NULL,
                      (VOID**)&mSmmVariable
                      );
    ASSERT_EFI_ERROR (Status);
  }
  
  return mSmmVariable->SmmGetVariable (
                VariableName,
                VendorGuid,
                Attributes,
                DataSize,
                Data
                );

}


EFI_STATUS
EFIAPI
PasswordSetVariable (
  IN     CHAR16                      *VariableName,
  IN     EFI_GUID                    *VendorGuid,
  IN     UINT32                      Attributes,
  IN     UINTN                       DataSize,
  OUT    VOID                        *Data
  )
{
  EFI_STATUS Status;

  //
  // Locate SmmVariableProtocol
  //
  if (mSmmVariable == NULL) {
    Status = gSmst->SmmLocateProtocol (
                      &gEfiSmmVariableProtocolGuid,
                      NULL,
                      (VOID**)&mSmmVariable
                      );
    ASSERT_EFI_ERROR (Status);
  }
  
  return mSmmVariable->SmmSetVariable (
            VariableName,
            VendorGuid,
            Attributes,
            DataSize,
            Data
            );

}

EFI_STATUS
EFIAPI
SmmPasswordEndOfDxeEventNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  gRecoederTime = PcdGet8(PcdRecorderPasswordTime);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PasswordCreatEvent (
  IN CONST VOID                   *NotifyContext
) {
  EFI_STATUS                     Status;
  VOID                          *Registration;

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmEndOfDxeProtocolGuid,
                    SmmPasswordEndOfDxeEventNotify,
                    &Registration
                    );
  return Status;
}