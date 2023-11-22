/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include "AtaAtapiPassThru.h"

EFI_STATUS
EFIAPI
InvokeHookProtocol2 (
  EFI_GUID  *Protocol,
  VOID      *Param
  )
{
  UINTN                  HandleCount;
  EFI_HANDLE             *Handles = NULL;
  EFI_STATUS             Status;
  UINTN                  Index;
  EFI_MY_HOOK_PROTOCOL2  MyHook2;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  Protocol,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    goto ProcExit;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    Protocol,
                    (VOID **)&MyHook2
                    );
    ASSERT (!EFI_ERROR (Status));
    MyHook2 (Param);
  }

ProcExit:
  if (Handles != NULL) {
    gBS->FreePool (Handles);
  }

  return Status;
}
