/*++

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
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
#include <AmdPspDirectory.h>
#include <Library/AmdPspBaseLibV2.h>
#include <Token.h>


// Based on the suggestion from hygon, using below structure to record psp info
 
// PSP L1 directory - Fixed region
PSP_DIRECTORY_ENTRY gPspDirL1[]={
{PSP_FW_RECOVERY_BOOT_LOADER, 0, 0},	  //Type 0x03
{SMU_OFFCHIP_FW, 0, 0},                 //Type 0x08
{AGESA_BOOT_LOADER_0, 0, 0},            //Type 0x30
{PSP_DIR_LV2, 0, 0},                    //Type 0x40
};

// PSP L2 directory - Updatable region
PSP_DIRECTORY_ENTRY gPspDirL2[]={
{PSP_FW_BOOT_LOADER, 0, 0},             //Type 0x01
{SMU_OFFCHIP_FW, 0, 0},                 //Type 0x08
{AGESA_BOOT_LOADER_0, 0, 0},            //Type 0x30
};




VOID
UpdatePspInfo (
  EFI_HII_HANDLE HiiHandle
  )
{
  VOID                    *RomPtr;
  UINT8                   *VerPtr;
  UINT8                   TableCount;
  UINT8                   TableIndex;
  UINT8                   EntryIndex;
  UINT32                  PspDirBase;
  UINT32                  BiosDirBase;
  

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if(!GetDirBase(&PspDirBase, &BiosDirBase)){
    return;
  }

  RomPtr = (VOID*)(UINTN)PspDirBase;
  TableCount = sizeof(gPspDirL1)/sizeof(PSP_DIRECTORY_ENTRY);
  for (TableIndex = 0; TableIndex < TableCount; TableIndex++){
    for (EntryIndex = 0; EntryIndex < ((PSP_DIRECTORY*)RomPtr)->Header.TotalEntries; EntryIndex++){
      if(((PSP_DIRECTORY*)RomPtr)->PspEntry[EntryIndex].Type == gPspDirL1[TableIndex].Type){
        gPspDirL1[TableIndex].Location = ((PSP_DIRECTORY*)RomPtr)->PspEntry[EntryIndex].Location;
        DEBUG((EFI_D_ERROR,"gPspDirL1[%d].Location %X\n",TableIndex,gPspDirL1[TableIndex].Location));
        break;
      }
    }
  }

  RomPtr = (VOID*)(gPspDirL1[3].Location); 
  if(((PSP_DIRECTORY_HEADER*)RomPtr)->PspCookie != PSP_LV2_DIRECTORY_HEADER_SIGNATURE){
    DEBUG((EFI_D_INFO, "%a() L2 directory not found, RomPtr %X\n", __FUNCTION__,RomPtr));
    return ;//EFI_NOT_FOUND;
  }

  TableCount = sizeof(gPspDirL2)/sizeof(PSP_DIRECTORY_ENTRY);
  for (TableIndex = 0; TableIndex < TableCount; TableIndex++){
    for (EntryIndex = 0; EntryIndex < ((PSP_DIRECTORY*)RomPtr)->Header.TotalEntries; EntryIndex++){
      if (((PSP_DIRECTORY*)RomPtr)->PspEntry[EntryIndex].Type == gPspDirL2[TableIndex].Type){
        gPspDirL2[TableIndex].Location = ((PSP_DIRECTORY*)RomPtr)->PspEntry[EntryIndex].Location;
        DEBUG((EFI_D_ERROR,"gPspDirL2[%d].Location %X\n",TableIndex,gPspDirL2[TableIndex].Location));

        break;
      }
    }
  }

  //PSP Recovery BootLoader Version
  VerPtr = (UINT8 *)(gPspDirL1[0].Location + 0x60);
  InitString(HiiHandle, STRING_TOKEN(STR_PSP_REC_VER_VALUE), L"%d.%d.%d", *(VerPtr+3), *(VerPtr+2), *(VerPtr+1));

  //SMU FW Version
  VerPtr= (UINT8 *)(gPspDirL1[1].Location + 0x60);
  InitString(HiiHandle, STRING_TOKEN(STR_SMU_FW_VER_VALUE_L1), L"%d.%d.%d", *(VerPtr+2), *(VerPtr+1), *VerPtr);

  //ABL Version
  VerPtr = (UINT8 *)(gPspDirL1[2].Location + 0x60);
  InitString(HiiHandle, STRING_TOKEN(STR_ABL_VER_VALUE_L1), L"%d.%d.%d", *(VerPtr+3), *(VerPtr+2), *(VerPtr+1));

  //APCB Version
  VerPtr = (UINT8 *)(gPspDirL1[2].Location + 0xA0);
  InitString(HiiHandle, STRING_TOKEN(STR_APCB_VER_VALUE_L1), L"%02x%02x", *(VerPtr+1), *VerPtr);

  //APOB Version
  VerPtr = (UINT8 *)(gPspDirL1[2].Location + 0xA4);
  InitString(HiiHandle, STRING_TOKEN(STR_APOB_VER_VALUE_L1), L"%02x%02x", *(VerPtr+1), *VerPtr);

  //APPB Version
  VerPtr = (UINT8 *)(gPspDirL1[2].Location + 0xA8);
  InitString(HiiHandle, STRING_TOKEN(STR_APPB_VER_VALUE_L1), L"%02x%02x", *(VerPtr+1), *VerPtr);




  //PSP directory Level 2 Updatable region
  //PSP BootLoader Version
  VerPtr = (UINT8 *)(gPspDirL2[0].Location + 0x60);
  InitString(HiiHandle, STRING_TOKEN(STR_PSP_BOT_VER_VALUE), L"%d.%d.%d", *(VerPtr+3), *(VerPtr+2), *(VerPtr+1));

  //SMU FW Version
  VerPtr = (UINT8 *)(gPspDirL2[1].Location + 0x60);
  InitString(HiiHandle, STRING_TOKEN(STR_SMU_FW_VER_VALUE_L2), L"%d.%d.%d", *(VerPtr+2), *(VerPtr+1), *VerPtr);

  //ABL Version
  VerPtr = (UINT8 *)(gPspDirL2[2].Location + 0x60);
  InitString(HiiHandle, STRING_TOKEN(STR_ABL_VER_VALUE_L2), L"%d.%d.%d", *(VerPtr+3), *(VerPtr+2), *(VerPtr+1));

  //APCB Version
  VerPtr = (UINT8 *)(gPspDirL2[2].Location + 0xA0);
  InitString(HiiHandle, STRING_TOKEN(STR_APCB_VER_VALUE_L2), L"%02x%02x", *(VerPtr+1), *VerPtr);

  //APOB Version
  VerPtr= (UINT8 *)(gPspDirL2[2].Location + 0xA4);
  InitString(HiiHandle, STRING_TOKEN(STR_APOB_VER_VALUE_L2), L"%02x%02x", *(VerPtr+1), *VerPtr);

  //APPB Version
  VerPtr = (UINT8 *)(gPspDirL2[2].Location + 0xA8);
  InitString(HiiHandle, STRING_TOKEN(STR_APPB_VER_VALUE_L2), L"%02x%02x", *(VerPtr+1), *VerPtr);
}


