/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  OemReset.c

Abstract:
  do platform reset.

Revision History:

**/

#include "PlatformSetupDxe.h"
#include <Library/ByoCommLib.h>
#include <Library/PlatformCommLib.h>


VOID
PlatformReset (
  VOID
  )
{
  EFI_STATUS         Status;
  UINTN              VariableSize;
  SETUP_DATA         *SetupHob;
  STATIC SETUP_DATA  SetupData;
  BOOLEAN            SetupChanged = FALSE;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  VariableSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupData
                  );
  ASSERT(!EFI_ERROR(Status));
  if (EFI_ERROR (Status)) {
    goto DoColdReset;
  }

  SetupHob = (SETUP_DATA*)GetSetupDataHobData();

  if (CompareMem(&SetupData, SetupHob, sizeof(SETUP_DATA))!=0) {
    DEBUG((EFI_D_INFO, "SetupData changed!\n"));
    SetupChanged = TRUE;
  }

  WriteCheckedCmosByte(FixedPcdGet8(PcdDubugLevelCmosOffset), SetupData.SysDebugMode);
  DEBUG((EFI_D_INFO, "Update SysDebugMode:%d\n", SetupData.SysDebugMode));

  if(SetupChanged){
//    if(SetupData.AfterPowerLoss != SetupHob->AfterPowerLoss){
//      PcdSet8S(PcdByoAfterPowerLoss, SetupData.AfterPowerLoss);
//    }
    InvokeHookProtocol(gBS, &gByoSetupOemresetDataChangedHookGuid);
  }

DoColdReset:
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  CpuDeadLoop();
}



