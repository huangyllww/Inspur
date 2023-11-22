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

**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/PcdLib.h>
#include <Protocol/LegacyInterruptHandler.h>

#include <SysMiscCfg.h>

// {74C5076F-5D21-4c44-A9FD-4158E1F3BE0E}
static EFI_GUID gInt18Asm16File = 
{ 0x74c5076f, 0x5d21, 0x4c44, { 0xa9, 0xfd, 0x41, 0x58, 0xe1, 0xf3, 0xbe, 0xe } };



EFI_LEGACY_INTERRUPT_HANDLER_PROTOCOL gInterruptHandler = {
  0x18,
  NULL,
  0
};



EFI_STATUS
EFIAPI
InstallInt18 (
  IN    EFI_HANDLE        ImageHandle,
  IN    EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS     Status;

  if (PcdGet8(PcdBiosBootModeType) == BIOS_BOOT_UEFI_OS) {
    return EFI_SUCCESS;
  }

  Status = GetSectionFromFv (
             &gInt18Asm16File, 
             EFI_SECTION_RAW, 
             0, 
             &gInterruptHandler.Handler, 
             &gInterruptHandler.Length
             );
  ASSERT_EFI_ERROR(Status);
  ASSERT(*(UINT32*)gInterruptHandler.Handler == SIGNATURE_32('$', 'F', 'I', 'X'));

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiLegacyInterruptHandlerProtocolGuid, &gInterruptHandler,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);
 
  return EFI_SUCCESS;
}

