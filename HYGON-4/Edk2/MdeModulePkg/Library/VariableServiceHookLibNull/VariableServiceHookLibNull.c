/** @file
  Null Variable Service Hook Library instance.

  Copyright (c) 2022, Byosoft Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include <Uefi.h>
#include <Library/VariableServiceHookLib.h>

/**
  This API function is called before SetVariable for the additional operation.

  If this function returns error status, SetVariable will directly return this status.
  SetVariable will execute if this function returns EFI_SUCCESS.

  @param[in]  VariableName       Same as EFI_SET_VARIABLE.
  @param[in]  VendorGuid         Same as EFI_SET_VARIABLE.
  @param[in]  Attributes         Same as EFI_SET_VARIABLE.
  @param[in]  DataSize           Same as EFI_SET_VARIABLE.
  @param[in, out]  Data          Same as EFI_SET_VARIABLE.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval error status           The variable can't be updated.

**/
EFI_STATUS
EFIAPI
BeforeSetVariableHook (
  IN  CHAR16    *VariableName,
  IN  EFI_GUID  *VendorGuid,
  IN  UINT32    Attributes,
  IN  UINTN     DataSize,
  IN OUT  VOID  *Data
  )
{
  return RETURN_SUCCESS;
}

/**
  This API function is called after SetVariable for the additional operation.

  Its return status doesn't impact SetVariable return status.
  This function is called when SetVariable success.

  @param[in]  VariableName       Same as EFI_SET_VARIABLE.
  @param[in]  VendorGuid         Same as EFI_SET_VARIABLE.
  @param[in]  Attributes         Same as EFI_SET_VARIABLE.
  @param[in]  DataSize           Same as EFI_SET_VARIABLE.
  @param[in]  Data               Same as EFI_SET_VARIABLE.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval error status           The variable can't be updated.

**/
EFI_STATUS
EFIAPI
AfterSetVariableHook (
  IN  CHAR16    *VariableName,
  IN  EFI_GUID  *VendorGuid,
  IN  UINT32    Attributes,
  IN  UINTN     DataSize,
  IN  VOID      *Data
  )
{
  return RETURN_SUCCESS;
}

/**
  This API function is called before GetVariable for the additional operation.

  If this function returns EFI_SUCCESS or EFI_BUFFER_TOO_SMALL, GetVariable will directly return.
  GetVariable will exectue when this function returns other error status.

  @param[in]      VariableName       Same as EFI_GET_VARIABLE.
  @param[in]      VendorGuid         Same as EFI_GET_VARIABLE.
  @param[out]     Attributes         Same as EFI_GET_VARIABLE.
  @param[in,out]  DataSize           Same as EFI_GET_VARIABLE.
  @param[out]     Data               Same as EFI_GET_VARIABLE.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval error status           The variable can't be got.

**/
EFI_STATUS
EFIAPI
BeforeGetVariableHook (
  IN  CHAR16 *VariableName,
  IN  EFI_GUID *VendorGuid,
  OUT    UINT32 *Attributes, OPTIONAL
  IN OUT UINTN                     *DataSize,
  IN OUT VOID                      *Data        OPTIONAL
  )
{
  return RETURN_NOT_FOUND;
}

/**
  This API function is called after GetVariable for the additional operation.

  This function is called when GetVariable returns EFI_NOT_FOUND.

  @param[in]      VariableName       Same as EFI_GET_VARIABLE.
  @param[in]      VendorGuid         Same as EFI_GET_VARIABLE.
  @param[out]     Attributes         Same as EFI_GET_VARIABLE.
  @param[in,out]  DataSize           Same as EFI_GET_VARIABLE.
  @param[out]     Data               Same as EFI_GET_VARIABLE.

  @retval EFI_SUCCESS            The function completed successfully.
  @retval error status           The variable can't be got.

**/
EFI_STATUS
EFIAPI
AfterGetVariableHook (
  IN  CHAR16 *VariableName,
  IN  EFI_GUID *VendorGuid,
  OUT    UINT32 *Attributes, OPTIONAL
  IN OUT UINTN                     *DataSize,
  IN OUT VOID                      *Data        OPTIONAL
  )
{
  return RETURN_NOT_FOUND;
}
