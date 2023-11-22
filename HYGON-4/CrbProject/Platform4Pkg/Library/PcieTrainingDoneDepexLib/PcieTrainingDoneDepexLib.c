/** @file
  Null Library instance to provide [Depex] for Tcg2ConfigPei module.

  Copyright (c) 2022, Byosoft Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>

/**
  This function just return success.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval error status           The variable can't be got.

**/
EFI_STATUS
EFIAPI
Tcg2ConfigPeiDepexLib (
  )
{
  return RETURN_SUCCESS;
}
