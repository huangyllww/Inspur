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
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Protocol/CryptoLibDxeProtocol.h>
#include <Library/PeiServicesLib.h>


extern CRYPTO_LIB_PROTOCOL gCryptoLibProtocol;


STATIC EFI_PEI_PPI_DESCRIPTOR  gPpiList[] = {
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gCryptoLibPpiGuid,
    &gCryptoLibProtocol
  }    
};

EFI_STATUS
EFIAPI
CryptoLibPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS    Status;

  Status = PeiServicesInstallPpi(gPpiList);
  ASSERT_EFI_ERROR(Status);
  return EFI_SUCCESS;
}


