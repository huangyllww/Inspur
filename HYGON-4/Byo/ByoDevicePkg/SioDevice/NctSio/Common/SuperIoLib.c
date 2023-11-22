
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/SerialPortLib.h>
#include <Library/DebugLib.h>
#include <Library/SuperIoLib.h>
#include <NctReg.h>



/**
  Perform a 8-bit I/O write to SIO register.

  @param[in]  Index  The register index.
  @param[in]  Data   The value to write to register.
**/
VOID
WriteSioReg (
  IN  UINT8            Index,
  IN  UINT8            Data
  )
{
  IoWrite8(_PCD_VALUE_PcdSioPortBaseAddress, Index);
  IoWrite8(_PCD_VALUE_PcdSioPortBaseAddress + 1, Data);
}

VOID
SioRegAndThenOr (
  IN  UINT8            Index,
  IN  UINT8            AndValue,
  IN  UINT8            OrValue
  )
{
  UINT8  Data8;

  IoWrite8(_PCD_VALUE_PcdSioPortBaseAddress, Index);
  Data8 = IoRead8(_PCD_VALUE_PcdSioPortBaseAddress + 1);
  Data8 = (Data8 & AndValue) | OrValue;
  IoWrite8(_PCD_VALUE_PcdSioPortBaseAddress + 1, Data8);
}



/**
  Perform a 8-bit I/O read from SIO register.

  @param[in]  Index  The register index.

  @return The value written to the register.
**/
UINT8
ReadSioReg (
  IN  UINT8            Index
  )
{
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress, Index);
  return IoRead8 (_PCD_VALUE_PcdSioPortBaseAddress + 1);
}

/**
  Program the SIO chip to enter the configure mode.
**/
VOID
EnterSioCfgMode (
  VOID
  )
{
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress, 0x87);
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress, 0x87);
}

VOID 
HwmRegAndThenOrEx (
  IN  UINT8            Page,
  IN  UINT8            Index,
  IN  UINT8            AndValue,
  IN  UINT8            OrValue
  )
{
  UINT8  Data8;
  IoWrite8(SIO_HW_INDEX_PORT, SIO_CFG_BANK_SEL);
  IoWrite8(SIO_HW_DATA_PORT , Page);
  IoWrite8(SIO_HW_INDEX_PORT , Index);
  Data8 = IoRead8(SIO_HW_DATA_PORT);
  Data8 = (Data8 & AndValue) | OrValue;
  IoWrite8(SIO_HW_DATA_PORT, Data8);
  
}

UINT8 ReadHwmRegEx(UINT8 Page,UINT8 Index)
{
  UINT8      Data8;
  IoWrite8(SIO_HW_INDEX_PORT, SIO_CFG_BANK_SEL);
  IoWrite8(SIO_HW_DATA_PORT, Page);
  IoWrite8(SIO_HW_INDEX_PORT, Index);
  Data8 = IoRead8(SIO_HW_DATA_PORT);
  return Data8;
}

VOID WriteHwmRegEx(UINT8 Page,UINT8 Index, UINT8 Value)
{
  IoWrite8(SIO_HW_INDEX_PORT, SIO_CFG_BANK_SEL);
  IoWrite8(SIO_HW_DATA_PORT, Page);
  IoWrite8(SIO_HW_INDEX_PORT, Index);
  IoWrite8(SIO_HW_DATA_PORT, Value);
}

/**
  Program the SIO chip to exit the configure mode.
**/
VOID
ExitSioCfgMode (
  VOID
  )
{
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress, 0xAA);
}


UINT16
GetSioChipId (
   VOID
){
  UINT16    DevId0;
  UINT16    DevId1;
  
  EnterSioCfgMode();
  DevId0 = ReadSioReg(R_DEVICE_ID0);
  DevId1 = ReadSioReg(R_DEVICE_ID1);
  ExitSioCfgMode();

  return (UINT16)(DevId0 << 8 | DevId1);   
}


