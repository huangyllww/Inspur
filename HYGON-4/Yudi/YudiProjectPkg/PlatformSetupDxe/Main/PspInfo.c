/*++

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:
  Platform configuration setup.

Revision History:


--*/


#include "PlatformSetupDxe.h"
#include "SetupItemId.h"
#include <SetupVariable.h>
#include <Library/BaseLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>
#include <HygonPspDirectory.h>
#include <Library/HygonPspBaseLib.h>



STATIC PSP_DIRECTORY_ENTRY gPspDirL1[]={
  {PSP_FW_BOOT_LOADER,  0, 0},
  {SMU_OFFCHIP_FW,      0, 0},
};


VOID
UpdatePspInfo (
  EFI_HII_HANDLE HiiHandle
  )
{
  UINT8                   *p;
  UINT8                   TableIndex;
  UINT8                   EntryIndex;
  UINT32                  PspDirBase;
  UINT32                  BiosDirBase;
  PSP_DIRECTORY           *PspDir;


  if(!GetDirBase(&PspDirBase, &BiosDirBase)){
    return;
  }

  for (TableIndex = 0; TableIndex < ARRAY_SIZE(gPspDirL1); TableIndex++){
    PspDir = (PSP_DIRECTORY*)(UINTN)PspDirBase;
    for (EntryIndex = 0; EntryIndex < PspDir->Header.TotalEntries; EntryIndex++){
      if(PspDir->PspEntry[EntryIndex].Type == gPspDirL1[TableIndex].Type){
        gPspDirL1[TableIndex].Location = PspDir->PspEntry[EntryIndex].Location;
        break;
      }
    }    
  }

  p = (UINT8*)(gPspDirL1[0].Location + 0x60);
  InitString(HiiHandle, STRING_TOKEN(STR_PSP_BOOT_VER_VALUE), L"%d.%d.%d.%d", p[3], p[2], p[1], p[0]);

  p = (UINT8*)(gPspDirL1[1].Location + 0x60);
  InitString(HiiHandle, STRING_TOKEN(STR_SMU_FW_VER_VALUE_L1), L"%d.%d.%d.%d", p[3], p[2], p[1], p[0]);

}


