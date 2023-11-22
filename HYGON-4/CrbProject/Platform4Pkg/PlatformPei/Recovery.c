/** @file

Copyright (c) 2006 - 2011, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

Module Name:

  Recovery.c

Abstract:

  PEIM to provide the platform recovery functionality.

--*/


#include "PlatformPei.h"
#include <Ppi/BootInRecoveryMode.h>


static EFI_PEI_PPI_DESCRIPTOR mRecoveryPpiList[] = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiBootInRecoveryModePpiGuid,
  NULL
};


EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  IN EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (*PeiServices)->InstallPpi(PeiServices, &mRecoveryPpiList[0]);
  ASSERT_EFI_ERROR (Status);

  return Status;
}


