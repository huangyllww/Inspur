/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
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

#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/SerialPortLib.h>
#include <Library/DebugLib.h>
#include <Library/SuperIoLib.h>
#include <Ite8625Reg.h>



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


VOID 
HwmRegAndThenOr (
  IN  UINT8            Index,
  IN  UINT8            AndValue,
  IN  UINT8            OrValue
  )
{
  UINT8  Data8;

  IoWrite8(_PCD_VALUE_PcdHwmIoBaseAddress + 5, Index);
  Data8 = IoRead8(_PCD_VALUE_PcdHwmIoBaseAddress + 6);
  Data8 = (Data8 & AndValue) | OrValue;
  IoWrite8(_PCD_VALUE_PcdHwmIoBaseAddress + 6, Data8);
}

UINT8 ReadHwmReg(UINT8 Index)
{
  IoWrite8(_PCD_VALUE_PcdHwmIoBaseAddress + 5, Index);
  return IoRead8(_PCD_VALUE_PcdHwmIoBaseAddress + 6);
}

VOID WriteHwmReg(UINT8 Index, UINT8 Value)
{
  IoWrite8(_PCD_VALUE_PcdHwmIoBaseAddress + 5, Index);
  IoWrite8(_PCD_VALUE_PcdHwmIoBaseAddress + 6, Value);
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
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress, 0x01);
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress, 0x55);
#if _PCD_VALUE_PcdSioPortBaseAddress == SIO_CFG_PORT_2E
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress, 0x55);
#else
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress, 0xAA);
#endif
}

/**
  Program the SIO chip to exit the configure mode.
**/
VOID
ExitSioCfgMode (
  VOID
  )
{
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress, 0x02);
  IoWrite8 (_PCD_VALUE_PcdSioPortBaseAddress + 1, 0x02);
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


VOID EnableUart1(UINT16 IoBase, UINT8 Irq)
{
  EnterSioCfgMode();
  WriteSioReg(0x07, 0x01);
  WriteSioReg(0x30, 0x00); 
  if(IoBase){
    WriteSioReg(0x60, (UINT8)((IoBase >> 8) & 0xFF));
    WriteSioReg(0x61, (UINT8)(IoBase & 0xFF));
    WriteSioReg(0x70, Irq); 
    WriteSioReg(0xF0, 0x01);
    WriteSioReg(0x30, 0x01); 
  } else {
    WriteSioReg(0x60, 0x00);
    WriteSioReg(0x61, 0x00);
  }
 
  ExitSioCfgMode();
}


VOID EnableUart2(UINT16 IoBase, UINT8 Irq)
{
  EnterSioCfgMode();
  WriteSioReg(0x07, 0x02);
  WriteSioReg(0x30, 0x00); 
  if(IoBase){

    WriteSioReg(0x07, 0x07);
    WriteSioReg(0x26, 0x00);
    
    WriteSioReg(0x07, 0x02);
    
    WriteSioReg(0x60, (UINT8)((IoBase >> 8) & 0xFF));
    WriteSioReg(0x61, (UINT8)(IoBase & 0xFF));
    WriteSioReg(0x70, Irq); 
    WriteSioReg(0xF0, 0x01);
    WriteSioReg(0x30, 0x01); 
  } else {
    WriteSioReg(0x60, 0x00);
    WriteSioReg(0x61, 0x00);
  }
 
  ExitSioCfgMode();
}



VOID EnableLpt(UINT16 IoBase, BOOLEAN Port80En, UINT8 LptMode)
{
  UINT8  Mode;
  

  EnterSioCfgMode();
  WriteSioReg(0x07, 0x03);
  WriteSioReg(0x30, 0x00);

  if(IoBase){
    WriteSioReg(0x60, (UINT8)((IoBase >> 8) & 0xFF));
    WriteSioReg(0x61, (UINT8)(IoBase & 0xFF));
    WriteSioReg(0x70, 0x07);
    WriteSioReg(0x74, 0x03); 

    if(LptMode == LPT_MODE_ECP_EPP){
      Mode = 3;
    } else if(LptMode == LPT_MODE_ECP){
      Mode = 2;
    } else if(LptMode == LPT_MODE_EPP){
      Mode = 1;
    } else {
      Mode = 3;
    }
    SioRegAndThenOr(0xF0, (UINT8)~(BIT0|BIT1), Mode);
    WriteSioReg(0x30, 0x01);     
    
  } else {
    WriteSioReg(0x60, 0x00);
    WriteSioReg(0x61, 0x00);
  }

  SioRegAndThenOr(0xF0, (UINT8)~BIT3, Port80En?0:BIT3);
  
  ExitSioCfgMode();
}




VOID LibSetPowerLed(UINT8 Type)
{

}

