/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPassword.c

Abstract:
  Implementation of basic setup password function.

Revision History:

**/
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/EfiPostLoginProtocol.h>
#include <Library/SystemPasswordLib.h>


EFI_POST_LOGIN_PROTOCOL gPostLoginProtocol = {
  CheckSysPd
};

EFI_STATUS
EFIAPI
PostLoginInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS  Status;
  
  Status = gBS->InstallProtocolInterface (
                 &ImageHandle,
                 &gEfiPostLoginProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &gPostLoginProtocol
                 );
  ASSERT_EFI_ERROR(Status);

  return Status;
}


