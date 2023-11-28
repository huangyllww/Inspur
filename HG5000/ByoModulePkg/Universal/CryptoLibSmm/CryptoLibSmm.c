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

#include <Uefi.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/CryptoLibDxeProtocol.h>



extern CRYPTO_LIB_PROTOCOL gCryptoLibProtocol;
  

EFI_STATUS
CryptoLibSmmEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;
  EFI_HANDLE    SmmHandle = NULL;
  
  Status = gSmst->SmmInstallProtocolInterface (
                    &SmmHandle,
                    &gCryptoLibSmmProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gCryptoLibProtocol
                    );
  ASSERT_EFI_ERROR(Status);
  return EFI_SUCCESS;
}


