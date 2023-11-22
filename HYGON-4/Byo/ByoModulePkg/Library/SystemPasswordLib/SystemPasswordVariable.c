/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPasswordVariable.c

Abstract:
  System password variable.

Revision History:

**/

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>

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
  return gRT->GetVariable (
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
  return gRT->SetVariable (
                VariableName,
                VendorGuid,
                Attributes,
                DataSize,
                Data
                );
}

VOID
EFIAPI
PasswordLibEndOfDxe (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  gRecoederTime = PcdGet8(PcdRecorderPasswordTime);
}

EFI_STATUS
EFIAPI
PasswordCreatEvent (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_EVENT                      PasswordEndOfDxeEvent      = NULL;

  Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                PasswordLibEndOfDxe,
                NULL,
                &gEfiEndOfDxeEventGroupGuid,
                &PasswordEndOfDxeEvent
                );
  return Status;
}