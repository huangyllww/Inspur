/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PeiUefiVariableDefaultHobLib.c

Abstract:
  Implementation of managing the UEFI Variable default data HOB in PEI.

Revision History:

**/

#include "UefiVariableDefaultHob.h"

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/UefiVariableDefaultHobLib.h>
#include <Library/UefiVariableHelperLib.h>
#include <Ppi/ReadOnlyVariable2.h>


/**
  Find variable from default variable HOB.

  @param[in]  VariableName      A Null-terminated string that is the name of the vendor's
                                variable.
  @param[in]  VendorGuid        A unique identifier for the vendor.
  @param[out] AuthFlag          Pointer to output Authenticated variable flag.

  @return Pointer to variable header, NULL if not found.

**/
AUTHENTICATED_VARIABLE_HEADER *
EFIAPI
FindVariableFromHob (
  IN CHAR16                     *VariableName,
  IN EFI_GUID                   *VendorGuid,
  OUT BOOLEAN                   *AuthFlag
  )
{
  EFI_HOB_GUID_TYPE             *GuidHob;
  VARIABLE_STORE_HEADER         *VariableStoreHeader;
  AUTHENTICATED_VARIABLE_HEADER *StartPtr;
  AUTHENTICATED_VARIABLE_HEADER *EndPtr;
  AUTHENTICATED_VARIABLE_HEADER *CurrPtr;
  VOID                          *Point;

  VariableStoreHeader = NULL;

  GuidHob = GetFirstGuidHob (&gEfiAuthenticatedVariableGuid);
  if (GuidHob != NULL) {
    VariableStoreHeader = (VARIABLE_STORE_HEADER *) GET_GUID_HOB_DATA (GuidHob);
    *AuthFlag = TRUE;
  } else {
    GuidHob = GetFirstGuidHob (&gEfiVariableGuid);
    if (GuidHob != NULL) {
      VariableStoreHeader = (VARIABLE_STORE_HEADER *) GET_GUID_HOB_DATA (GuidHob);
      *AuthFlag = FALSE;
    }
  }
  ASSERT (VariableStoreHeader != NULL);
  if (VariableStoreHeader == NULL) {
    return NULL;
  }

  StartPtr = GetStartPointer (VariableStoreHeader);
  EndPtr   = GetEndPointer (VariableStoreHeader);
  for ( CurrPtr = StartPtr
      ; (CurrPtr < EndPtr) && IsValidVariableHeader (CurrPtr)
      ; CurrPtr = GetNextVariablePtr (CurrPtr, *AuthFlag)
      ) {
    if (CurrPtr->State == VAR_ADDED) {
      if (CompareGuid (VendorGuid, GetVendorGuidPtr (CurrPtr, *AuthFlag))) {
        Point = (VOID *) GetVariableNamePtr (CurrPtr, *AuthFlag);

        ASSERT (NameSizeOfVariable (CurrPtr, *AuthFlag) != 0);
        if (CompareMem (VariableName, Point, NameSizeOfVariable (CurrPtr, *AuthFlag)) == 0) {
          return CurrPtr;
        }
      }
    }
  }

  return NULL;
}

/**
  Get variable from default variable HOB.

  @param[in]      VariableName  A Null-terminated string that is the name of the vendor's
                                variable.
  @param[in]      VendorGuid    A unique identifier for the vendor.
  @param[out]     Attributes    If not NULL, a pointer to the memory location to return the
                                attributes bitmask for the variable.
  @param[in, out] DataSize      On input, the size in bytes of the return Data buffer.
                                On output the size of data returned in Data.
  @param[out]     Data          The buffer to return the contents of the variable.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_NOT_FOUND         The variable was not found.
  @retval EFI_BUFFER_TOO_SMALL  The DataSize is too small for the result.
  @retval EFI_INVALID_PARAMETER VariableName is NULL.
  @retval EFI_INVALID_PARAMETER VendorGuid is NULL.
  @retval EFI_INVALID_PARAMETER DataSize is NULL.
  @retval EFI_INVALID_PARAMETER The DataSize is not too small and Data is NULL.

**/
EFI_STATUS
EFIAPI
GetVariableFromHob (
  IN     CHAR16                 *VariableName,
  IN     EFI_GUID               *VendorGuid,
  OUT    UINT32                 *Attributes, OPTIONAL
  IN OUT UINTN                  *DataSize,
  OUT    VOID                   *Data
  )
{
  BOOLEAN                       AuthFlag;
  AUTHENTICATED_VARIABLE_HEADER *Variable;
  UINTN                         VarDataSize;

  if ((VariableName == NULL) || (VendorGuid == NULL) || (DataSize == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Variable = FindVariableFromHob (VariableName, VendorGuid, &AuthFlag);
  if (Variable == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Get data size
  //
  VarDataSize = DataSizeOfVariable (Variable, AuthFlag);
  if (*DataSize >= VarDataSize) {
    if (Data == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    CopyMem (Data, GetVariableDataPtr (Variable, AuthFlag), VarDataSize);

    if (Attributes != NULL) {
      *Attributes = Variable->Attributes;
    }

    *DataSize = VarDataSize;
    return EFI_SUCCESS;
  } else {
    *DataSize = VarDataSize;
    return EFI_BUFFER_TOO_SMALL;
  }
}

/**
  Set variable to default variable HOB.

  @param[in] VariableName       A Null-terminated string that is the name of the vendor's
                                variable.
  @param[in] VendorGuid         A unique identifier for the vendor.
  @param[in] Attributes         If not NULL, a pointer to the memory location to set the
                                attributes bitmask for the variable.
  @param[in] DataSize           On input, the size in bytes of the return Data buffer.
                                On output the size of data returned in Data.
  @param[in] Data               The buffer to return the contents of the variable.

  @retval EFI_SUCCESS           The function completed successfully.
  @retval EFI_NOT_FOUND         The variable was not found.
  @retval EFI_INVALID_PARAMETER VariableName is NULL.
  @retval EFI_INVALID_PARAMETER VendorGuid is NULL.
  @retval EFI_INVALID_PARAMETER Attributes is not NULL, but attributes value is 0.
  @retval EFI_INVALID_PARAMETER DataSize is not equal to the variable data size.
  @retval EFI_INVALID_PARAMETER The DataSize is equal to the variable data size, but Data is NULL.

**/
EFI_STATUS
EFIAPI
SetVariableToHob (
  IN CHAR16                     *VariableName,
  IN EFI_GUID                   *VendorGuid,
  IN UINT32                     *Attributes, OPTIONAL
  IN UINTN                      DataSize,
  IN VOID                       *Data
  )
{
  BOOLEAN                       AuthFlag;
  AUTHENTICATED_VARIABLE_HEADER *Variable;
  UINTN                         VarDataSize;

  if (VariableName == NULL || VariableName[0] == 0 || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Variable = FindVariableFromHob (VariableName, VendorGuid, &AuthFlag);
  if (Variable == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // Get data size
  //
  VarDataSize = DataSizeOfVariable (Variable, AuthFlag);
  if (DataSize == VarDataSize) {
    if (Data == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    if (Attributes != NULL) {
      if (*Attributes == 0) {
        return EFI_INVALID_PARAMETER;
      }
      Variable->Attributes = *Attributes;
    }

    CopyMem (GetVariableDataPtr (Variable, AuthFlag), Data, VarDataSize);

    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}


