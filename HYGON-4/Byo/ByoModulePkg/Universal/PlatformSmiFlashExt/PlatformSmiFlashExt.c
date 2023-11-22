/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformSmiFlashExt.c

Abstract:
  Do some custom requirements when use byotool.

Revision History:

**/


#include <PiSmm.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <ByoSmiFlashInfo.h>
#include <Protocol/ByoSmiFlashExtProtocol.h>
#include <Protocol/SmmVariable.h>
#include <SystemPasswordVariable.h>
#include <Protocol/ByoSmiFlashLockProtocol.h>

typedef struct {
  EFI_GUID  *Guid;
  CHAR16    *Name;
  UINTN     BufferLength;
  UINT8     *Buffer;
  UINT32    Attributes;
} VARIABLE_DATA_BUFFER;

VARIABLE_DATA_BUFFER gReservedVariableList[] = {
  {&gEfiSystemPasswordVariableGuid, SYSTEM_PASSWORD_NAME, 0, NULL, 0},
  {NULL,NULL}
};

EFI_SMM_VARIABLE_PROTOCOL       *mSmmVariable = NULL;

BOOLEAN
IsProductionBoot (
  )
{
  //
  // Platform Implement it
  //
  return FALSE;
}

BOOLEAN
EFIAPI
SmiFlashExt (
  IN  SMI_INFO                     *SmiFlashInfo,
  IN  NV_MEDIA_ACCESS_PROTOCOL     *MediaAccess,
  OUT EFI_STATUS                   *RetStatus
  )
{

  UINTN        Index;
  EFI_STATUS   Status;

  if((SmiFlashInfo->SubFunction == SUBFUNCTION_QUERY_PASSWORD_SET)||
    (SmiFlashInfo->SubFunction == SUBFUNCTION_QUERY_PASSWORD)){
    //*RetStatus = EFI_NOT_FOUND;
    //return TRUE;   //skip verify password when use byotool
  }

  if (SmiFlashInfo->SubFunction == SUBFUNCTION_CHECK_BIOS_ID) {
    //*RetStatus = EFI_SUCCESS;
    //return TRUE;   //skip BiosId check when use ByoFlash update
  }

  if (SmiFlashInfo->SubFunction == SUBFUNCTION_CHECK_BIOS_LOCK) {
    if (IsProductionBoot ()) {
      //
      // Lock Smbios Status on production
      //
      *(UINTN*)SmiFlashInfo->Buffer = BYO_SMIFLASH_STS_SMBIOS_LOCK;
      *RetStatus = EFI_SUCCESS;
      return TRUE;
    }
  }

  if (SmiFlashInfo->SubFunction == SUBFUNCTION_IF_UNLOCK) {
    if (IsProductionBoot ()) {
      //
      // Don't support unlock on production
      //
      *RetStatus = EFI_UNSUPPORTED;
      return TRUE;
    }
  }

  if (SmiFlashInfo->SubFunction == SUBFUNCTION_DISABLE_USB_POWERBUTTON) {
    //
    // Read the reserved specific variable data before flash update
    //
    for (Index = 0; gReservedVariableList[Index].Guid != NULL; Index ++) {
      if (gReservedVariableList[Index].BufferLength > 0) {
        //
        // Has been cached.
        //
        continue;
      }
      Status = mSmmVariable->SmmGetVariable(
                        gReservedVariableList[Index].Name,
                        gReservedVariableList[Index].Guid,
                        &gReservedVariableList[Index].Attributes,
                        &gReservedVariableList[Index].BufferLength,
                        gReservedVariableList[Index].Buffer
                        );
      if (Status == EFI_BUFFER_TOO_SMALL) {
        //
        // Variable is cached here
        //
        gReservedVariableList[Index].Buffer = AllocatePool (gReservedVariableList[Index].BufferLength);
        Status = mSmmVariable->SmmGetVariable(
                          gReservedVariableList[Index].Name,
                          gReservedVariableList[Index].Guid,
                          &gReservedVariableList[Index].Attributes,
                          &gReservedVariableList[Index].BufferLength,
                          gReservedVariableList[Index].Buffer
                          );
      }
    }
  }

  if (SmiFlashInfo->SubFunction == SUBFUNCTION_ENABLE_USB_POWERBUTTON) {
    //
    // Set the reserved variable data if the specific variable can't be found after flash update
    //
    for (Index = 0; gReservedVariableList[Index].Guid != NULL; Index ++) {
      Status = mSmmVariable->SmmGetVariable(
                        gReservedVariableList[Index].Name,
                        gReservedVariableList[Index].Guid,
                        &gReservedVariableList[Index].Attributes,
                        &gReservedVariableList[Index].BufferLength,
                        gReservedVariableList[Index].Buffer
                        );
      if (Status == EFI_NOT_FOUND && gReservedVariableList[Index].BufferLength > 0) {
        //
        // Variable is cached here
        //
        Status = mSmmVariable->SmmSetVariable(
                          gReservedVariableList[Index].Name,
                          gReservedVariableList[Index].Guid,
                          gReservedVariableList[Index].Attributes,
                          gReservedVariableList[Index].BufferLength,
                          gReservedVariableList[Index].Buffer
                          );
      }
    }
  }

  return FALSE;

}

BYO_SMIFLASH_EXT_PROTOCOL gByoSmiflashExtProtocol = {
  SmiFlashExt
};

EFI_STATUS
ProjectSmmEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS                                Status;
  EFI_HANDLE                                SmmHandle = NULL; 

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = gSmst->SmmInstallProtocolInterface (
                    &SmmHandle,
                    &gByoSmiFlashExtProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gByoSmiflashExtProtocol
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID**)&mSmmVariable
                    );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}



