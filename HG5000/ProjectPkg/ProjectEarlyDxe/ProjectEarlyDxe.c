/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  CrbDxe.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ByoCommLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Pi/PiBootMode.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/BiosIdLib.h>
#include <Protocol/BiosIdInfoProtocol.h>
#include <Library/PrintLib.h>


BIOS_ID_INFO gPlatBiosIdInfo;


// sometimes, board id in bios id may not be same as the board name printed on the board, 
// so here give a convert table.
typedef struct {
  CHAR16 BoardId[7+1];
  CHAR16 OemId[3+1];
  CHAR8  BoardName[17];
} BIOS_ID_BOARD_ID;

static BIOS_ID_BOARD_ID gBiosIdBoardIdList[] = {
  {{L"T1DMHG_"}, {L"TF_"}, {"INSPUR-T1DMHG-E2"}},
};

STATIC EFI_STATUS GetPlatBiosIdInfo()
{
  EFI_STATUS            Status;
  BIOS_ID_IMAGE         BiosIdImage;
  UINTN                 Index;
  CHAR16                *p;
  CHAR8                 *s;
  UINT16                Ver;
  BIOS_ID_INFO          *BiosIdInfo;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  BiosIdInfo = &gPlatBiosIdInfo;
  ZeroMem(BiosIdInfo, sizeof(BIOS_ID_INFO));

  Status = GetBiosId(&BiosIdImage);
  ASSERT(!EFI_ERROR(Status));
  p = BiosIdImage.BiosIdString.VersionMajor;
  s = BiosIdInfo->BiosVer;
  s[0] = (UINT8)BiosIdImage.BiosIdString.BuildType;
  for(Index=0;Index<4;Index++){
    s[Index+1] = (CHAR8)p[Index];
  }
  s[Index+1] = 0;

  Ver = (UINT16)AsciiStrDecimalToUintn(s+1);
  BiosIdInfo->VerMajor = (UINT8)(Ver / 100);
  BiosIdInfo->VerMinor = (UINT8)(Ver % 100);

  AsciiSPrint (
    BiosIdInfo->BiosVer, 
    sizeof(BiosIdInfo->BiosVer), 
    "%a",
    (CHAR8*)PcdGetPtr(PcdSmbiosBiosVersion)
    );

  p = BiosIdImage.BiosIdString.TimeStamp;     // YYMMDDHHMM -> "2018-05-02"
  s = BiosIdInfo->BiosDate;
  s[0] = '2';
  s[1] = '0';  
  s[2] = (CHAR8)p[0];
  s[3] = (CHAR8)p[1];
  s[4] = '-'; 
  s[5] = (CHAR8)p[2];  
  s[6] = (CHAR8)p[3];   
  s[7] = '-'; 
  s[8] = (CHAR8)p[4];
  s[9] = (CHAR8)p[5]; 
  s[10] = 0;

  for(Index=0;Index<ARRAY_SIZE(gBiosIdBoardIdList);Index++){
    if(CompareMem(BiosIdImage.BiosIdString.BoardId, gBiosIdBoardIdList[Index].BoardId, sizeof(BiosIdImage.BiosIdString.BoardId)) == 0 &&
       CompareMem(BiosIdImage.BiosIdString.OemId, gBiosIdBoardIdList[Index].OemId, sizeof(BiosIdImage.BiosIdString.OemId)) == 0){
      AsciiStrCpy(BiosIdInfo->BoardId, gBiosIdBoardIdList[Index].BoardName);
      break;
    }
  }
  if(Index >= ARRAY_SIZE(gBiosIdBoardIdList)){
    p = BiosIdImage.BiosIdString.BoardId;
    s = BiosIdInfo->BoardId;  
    for(Index=0;Index<7;Index++){
      if(p[Index] == L'_'){
        break;
      }      
      s[Index] = (CHAR8)p[Index];
    }  
    s[Index] = 0;
  }

  DEBUG((EFI_D_INFO, "%a() %a %a %a %d.%d\n", __FUNCTION__, BiosIdInfo->BoardId, \
                        BiosIdInfo->BiosVer, BiosIdInfo->BiosDate, \
                        BiosIdInfo->VerMajor, BiosIdInfo->VerMinor));
  return Status;
}



EFI_STATUS
ProjectEarlyDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS       Status;

  Status = GetPlatBiosIdInfo();
  ASSERT(!EFI_ERROR(Status));

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gBiosIdInfoProtocolGuid, &gPlatBiosIdInfo,
                  NULL
                  );
  ASSERT(!EFI_ERROR(Status));  

  return EFI_SUCCESS;
}


