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
#include <Token.h>

VOID
PlatformReset (
  VOID
  )
{
  EFI_STATUS         Status;
  UINTN              VariableSize;
  SETUP_DATA         *SetupHob;
  STATIC SETUP_DATA  SetupData;


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
    //DumpMem8(&SetupData, sizeof(SETUP_DATA));
    //DumpMem8(SetupHob,   sizeof(SETUP_DATA));    
    DEBUG((EFI_D_INFO, "SetupData changed!\n"));
//-    WriteCheckedCmosByte(SETUP_DATA_CHANGE_CMOS_OFFSET, 1);
  }

DoColdReset:
  //j_n Change to Warm reset.  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
  CpuDeadLoop();
}



