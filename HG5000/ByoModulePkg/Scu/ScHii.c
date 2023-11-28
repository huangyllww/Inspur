/** @file

Copyright (c) 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ScHii.c

Abstract:
  The common file is intended to push HII data into RT memory.
  It would be used on SCM or SCM test driver, so this file need HAL since
  it is only for UEFI purpose. Under OS, HII data has already been at RT
  memory by this file.

Revision History:

TIME:       2018-7-26
$AUTHOR:    Phinux Qin
$REVIEWERS:
$SCOPE:     All Byosoft IA32/X64 Platforms
$TECHNICAL:


T.O.D.O

$END-------------------------------------------------------------------------------

--*/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DxeServicesLib.h>
#include <Protocol/HiiDatabase.h>
#include <Library/HiiLib.h>
#include <Guid/MdeModuleHii.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/PrintLib.h>
#include <Library/ByoCommLib.h>
#include <Protocol/ByoPlatformSetupProtocol.h>


#pragma pack(1)

typedef struct {
  UINTN     PhysicalAddress;
  UINTN     Size;  
} SC_HII_VAR;

#pragma pack()


#define SC_VAR_NAME       L"ScHiiDb"


VOID
ScPushHiiDb (
    VOID
  )
{
  EFI_STATUS                     Status = EFI_SUCCESS;
  VOID                           *pBuff = NULL;
  UINTN                          BuffSize = 0;
  EFI_HII_DATABASE_PROTOCOL      *pHiiDatabaseProtocol= NULL;
  SC_HII_VAR                     ScHiiVar;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, &pHiiDatabaseProtocol);
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }

  Status = pHiiDatabaseProtocol->ExportPackageLists(pHiiDatabaseProtocol, NULL, &BuffSize, pBuff);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    pBuff = AllocateReservedZeroMemoryBelow4G(gBS, BuffSize);
    if (pBuff == NULL) {
      DEBUG((EFI_D_ERROR, "Out of resource\n"));
      goto ProcExit;
    }
    DEBUG((EFI_D_INFO, "Hii Data (%lX,%X)\n", (UINT64)(UINTN)pBuff, BuffSize));

    Status = pHiiDatabaseProtocol->ExportPackageLists(pHiiDatabaseProtocol, NULL, &BuffSize, pBuff);
    if (EFI_ERROR (Status)) {
       DEBUG ((EFI_D_ERROR, "Fail to get Hii data\n"));
       goto ProcExit;
    }

    ScHiiVar.PhysicalAddress = (UINTN)pBuff;
    ScHiiVar.Size = BuffSize;
    Status = gRT->SetVariable (
                    SC_VAR_NAME,
                    &gByoScVarGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    sizeof(SC_HII_VAR),
                    &ScHiiVar
                    );
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Fail to save Hii Data ACPINvs:%r\n", Status));
    }
  }
  
ProcExit:
  return;
}



VOID
EFIAPI 
ScOnReadyToBoot(
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__)); 
  gBS->CloseEvent(Event);

  InvokeHookProtocol(gBS, &gPlatCollectHiiResourceHookGuid);

  ScPushHiiDb();
}




/**
  Entry point of the system configuration or unit test DXE driver.

  @param ImageHandle    - ImageHandle
  @param SystemTable    - Pointer to System Table

  @return EFI_STATUS

**/
EFI_STATUS
EFIAPI
ScHiiEntry (
  IN    EFI_HANDLE                  ImageHandle,
  IN    EFI_SYSTEM_TABLE            *SystemTable
  )
{
  EFI_STATUS    Status;
  EFI_EVENT     Event; 


  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             ScOnReadyToBoot,
             NULL,
             &Event
             ); 
  ASSERT_EFI_ERROR(Status);

  return Status;
}



