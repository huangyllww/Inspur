/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PlatformRasCommon

Abstract:
  Source file for the RasElogEventLib.

Revision History:

**/
#include <PiDxe.h>
#include <Library/PciLib.h>
#include <AmdRas.h>

/**
    Counts set (populated) bits in word
    
    @param UINT16   Testword - word to count bits
    @retval UINT32  number of bits set in the input word
**/

UINT32
PopCount16(
  UINT16    Testword
  )
{
    UINT32  BitCount = 0;
    UINT32  Index;
    
    for(Index = 0; Index < 16; Index++){
        BitCount += (Testword >> Index)& 0x1;
    }
    return BitCount;
}

/**
    Read SMN register
 
    @param DieBus     - IOHC (Node) bus number
    @param SmnAddress  - Register SMN address
    @param Value       - Pointer to register value
 **/

VOID
SmnRead (
  IN UINT32 DieBus,
  IN UINT32 SmnAddress,
  IN UINT32 *Value
  )
{
    UINT32    RegIndex;
    UINTN     PciAddress;

    RegIndex = SmnAddress;
    PciAddress = (DieBus << 20) + 0x00B8;
    PciWrite32 (PciAddress, RegIndex);
    PciAddress = (DieBus << 20) + 0x00BC;
    *Value = PciRead32 (PciAddress);
}

/**
    Returns Ecc Symbol size currently programmed in UMC
 
    @param VOID
 
    @retval 0x08 - 8-Bit symbol, default
    @retval 0x04 - 4-Bit symbol
 **/

UINT32
GetSymbolSize(
        VOID)
{
    UINT8           DieCount;
    UINT32          DieBus;
    UINT32          Die;
    UMC_ECCCTRL_REG EccCtrl;
    UINT32          SymbolSize = 0x08;
    
    DieCount = PciRead8((0x18 << 15) + (0x01 << 12) + 0x204);
    
    for(Die=0; Die < DieCount; Die++){
        // Get DieBus number
        DieBus = PciRead8(((0x18 + Die)<< 15) + (0x0 << 12) + 0x84 );
        // Get EccCtrl register value
        SmnRead(DieBus, UMC0_CH_REG_BASE + UMC_ECC_CTRL, &EccCtrl.Value);
        if(EccCtrl.Value == 0){
            continue;
        }
        // Check if DIMM present and Ecc enabled
        if(EccCtrl.Field.RdEccEn == 1){
            if(EccCtrl.Field.EccSymbolSize == 0){
                SymbolSize = 0x04;
            }
            break;
        }
    }

    return SymbolSize;
}

