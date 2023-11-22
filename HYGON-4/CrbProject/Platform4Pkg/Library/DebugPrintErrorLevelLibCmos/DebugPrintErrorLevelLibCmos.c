/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By: 
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include <Uefi.h>
#include <Library/IoLib.h>

STATIC BOOLEAN gDebugPrintErrorLevelInit = FALSE;
STATIC UINT32  gDebugPrintErrorLevel;


STATIC UINT8 _Cmos70Read(UINT8 Address)
{
  UINT8  Port70;
  UINT8  Data8;

  Port70 = IoRead8(0x70);
  IoWrite8(0x70, Address);
  Data8 = IoRead8(0x71);
  IoWrite8(0x70, Port70); 

  return Data8;
}

STATIC VOID _Cmos70Write(UINT8 Address, UINT8 Data)
{
  UINT8  Port70;

  Port70 = IoRead8(0x70);
  IoWrite8(0x70, Address);
  IoWrite8(0x71, Data);
  IoWrite8(0x70, Port70);  
}


STATIC VOID _WriteCheckedCmosByte(UINT8 Offset, UINT8 Data8)
{
  UINT8  CmosData;
  
  CmosData = Data8 & 0xF;
  CmosData = (((~CmosData) & 0xF) << 4)|CmosData;
  _Cmos70Write(Offset, CmosData);
}

STATIC BOOLEAN _ReadCheckedCmosByte(UINT8 Offset, UINT8 *Data8)
{
  UINT8  CmosData;
  UINT8  Data1, Data2;

  CmosData = _Cmos70Read(Offset);
  Data1 = (CmosData & 0xF);
  Data2 = (CmosData >> 4) & 0xF;

  if(Data1 != ((~Data2) & 0xF)){
    return FALSE;
  } else {
    *Data8 = Data1;
    return TRUE;
  }
}


BOOLEAN
EFIAPI
SetDebugPrintErrorLevel (
  IN UINT32 ErrorLevel
  )
{
  UINT8  Data8;

  if(ErrorLevel == FixedPcdGet32(PcdDebugLevelOem0)){
    Data8 = 0;
  } else if(ErrorLevel == FixedPcdGet32(PcdDebugLevelOem1)){
    Data8 = 1;
  } else if(ErrorLevel == FixedPcdGet32(PcdDebugLevelOem2)){
    Data8 = 2;
  } else if(ErrorLevel == FixedPcdGet32(PcdDebugLevelOem3)){
    Data8 = 3;
  } else {
    Data8 = 0;
  }
  _WriteCheckedCmosByte(FixedPcdGet8(PcdDubugLevelCmosOffset), Data8);
  gDebugPrintErrorLevel     = ErrorLevel;
  gDebugPrintErrorLevelInit = TRUE;
  
  return EFI_SUCCESS;
}


UINT32
EFIAPI
GetDebugPrintErrorLevel (
  VOID
  )
{
  UINT32  Level;
  UINT8   Data8;

  if(gDebugPrintErrorLevelInit){
    return gDebugPrintErrorLevel;
  }
  
  if(!_ReadCheckedCmosByte(FixedPcdGet8(PcdDubugLevelCmosOffset), &Data8)){
    if(FixedPcdGetBool(PcdSetupSysDebugModeDefaultAll)){
      Level = FixedPcdGet32(PcdDebugLevelOem3);
    } else {
      Level = FixedPcdGet32(PcdDebugLevelOem0);
    }
  } else {
    switch (Data8) {
      case 0:
        Level = FixedPcdGet32(PcdDebugLevelOem0);
        break;
      case 1:
        Level = FixedPcdGet32(PcdDebugLevelOem1);
        break;
      case 2:
        Level = FixedPcdGet32(PcdDebugLevelOem2);
        break;
      case 3:
        Level = FixedPcdGet32(PcdDebugLevelOem3);
        break;
      default:
        Level = FixedPcdGet32(PcdDebugLevelOem0);
        break;
    }
  }
  gDebugPrintErrorLevel = Level;
  gDebugPrintErrorLevelInit = TRUE;

  return Level;
}



