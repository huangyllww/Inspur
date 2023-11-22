/**
 * @file
 *
 * Fabric PCI Bus Map Reinitializes functions
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 *
 */
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 *
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 *
 ******************************************************************************
 */

#include "Porting.h"
#include "HGPI.h"
#include <Library/IdsLib.h>
#include <FabricRegistersST.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Filecode.h>
#include "FabricPciBusMapInit.h"

#define FILECODE  FABRIC_HYEX_FABRICPEI_FABRICPCIBUSMAP_FILECODE

STATIC CONST UINTN  IomsInstanceIds[] = {
  IOMS0_INSTANCE_ID,
  IOMS1_INSTANCE_ID,
  IOMS2_INSTANCE_ID,
  IOMS3_INSTANCE_ID
};

/**
 * FabricStSetCfgAddrMapReg
 *
 * Set ConfigAddress register according to input parameters
 *
 * @param[in]         TotalSocket                 System socket count
 * @param[in]         DstFabricId                 IOMS Fabric ID
 * @param[in]         BusNumBase                  Base of bus region
 * @param[in]         BusNumLimit                 Max bus region
 * @return            VOID
 */
VOID
FabricStSetCfgAddrMapReg (
  IN       UINT8  TotalSocket,
  IN       UINT16 DstFabricId,
  IN       UINT8  BusNumBase,
  IN       UINT8  BusNumLimit
  )
{
  UINTN                     CddsPresent, IodNumber;
  UINTN                     RegIndex, SocketIndex, DieIndex, CddIndex;
  CFG_ADDRESS_MAP_REGISTER  CfgAddrMap;

  ASSERT (BusNumLimit <= 0xFF);

  for (SocketIndex = 0; SocketIndex < TotalSocket; SocketIndex ++) {
    //
    // IOD DF
    //
    IodNumber = FabricTopologyGetNumberOfLogicalDiesOnSocket(SocketIndex);
    for (DieIndex = 0; DieIndex < IodNumber; DieIndex++) {
      for (RegIndex = 0; RegIndex < 16; RegIndex++) {
        
        CfgAddrMap.Value = FabricRegisterAccRead (
                             SocketIndex,
                             DieIndex,
                             CFGADDRESSMAP_FUNC,
                             (CFGADDRESSMAP0_REG + RegIndex * 4),
                             FABRIC_REG_ACC_BC
                           );
        if (CfgAddrMap.Field.DstFabricID != DstFabricId) {
          continue;
        }

        CfgAddrMap.Field.BusNumBase  = BusNumBase;
        CfgAddrMap.Field.BusNumLimit = BusNumLimit;

        FabricRegisterAccWrite (
          SocketIndex,
          DieIndex,
          CFGADDRESSMAP_FUNC,
          (CFGADDRESSMAP0_REG + RegIndex * 4),
          FABRIC_REG_ACC_BC,
          CfgAddrMap.Value,
          FALSE
        );
      }
    }

    //
    // CDD DF
    //
    CddsPresent = FabricTopologyGetCddsPresentOnSocket (SocketIndex);
    for (CddIndex = 0; CddIndex < MAX_CDDS_PER_SOCKET; CddIndex++) {
      if (!IS_CDD_PRESENT (CddIndex, CddsPresent)) {
        continue;
      }
      for (RegIndex = 0; RegIndex < 16; RegIndex++) {

        CfgAddrMap.Value = CddFabricRegisterAccRead (
                       SocketIndex,
                       CddIndex,
                       CFGADDRESSMAP_FUNC,
                       (CFGADDRESSMAP0_REG + (RegIndex * 4)),
                       FABRIC_REG_ACC_BC
                       );
        if (CfgAddrMap.Field.DstFabricID != DstFabricId) {
          continue;
        }

        CfgAddrMap.Field.BusNumBase  = BusNumBase;
        CfgAddrMap.Field.BusNumLimit = BusNumLimit;
        CddFabricRegisterAccWrite (
          SocketIndex,
          CddIndex,
          CFGADDRESSMAP_FUNC,
          (CFGADDRESSMAP0_REG + RegIndex * 4),
          FABRIC_REG_ACC_BC,
          CfgAddrMap.Value,
          FALSE
        );
      }
    }
  }
}

/**
 *
 *  FabricPciBusMapInit
 *
 *  Description:
 *    This funtion initializes the PCI Bus in the fabric.
 *
 *  @param[in]         VOID
 *  @return            VOID
 *
 */
VOID
FabricPciBusMapInit (
  VOID
  )
{
  UINT8                      i;
  UINT8                      j;
  UINT8                      k;
  UINT8                      *PciBusMaps = NULL;
  UINT16                     PciBusCount = 0;
  UINT8                      BusLength;
  UINT8                      BusNumBase = 0;
  UINT8                      BusNumLimite = 0;
  UINT16                     DstFabricId;
  UINT8                      SocketCount;
  UINT8                      LogicalDieCount;
  UINT8                      RbPerIOD;
  UINT8                      PciBusMapSize;
  CFG_ADDRESS_CTRL_REGISTER  CfgAddrCntl;

  PciBusMaps    = (UINT8 *)PcdGetPtr (PcdHygonFabricPciBusMap);
  PciBusMapSize = (UINT8)PcdGetSize (PcdHygonFabricPciBusMap);

    
  // Step 1: Check if the PcdHygonFabricPciBusMap is valid?
  for (i = 0; i < PciBusMapSize; i++) {
    BusLength = PciBusMaps[i];
    IDS_HDT_CONSOLE (CPU_TRACE, "    Logic Rb %d Bus Length: 0x%02x\n", i, BusLength);
    PciBusCount += BusLength;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "    PciBusCount: 0x%x\n", PciBusCount);
  if (PciBusCount != 0x100) {
    // PcdHygonFabricPciBusMap is invalid
    return;
  }

  SocketCount = (UINT8)FabricTopologyGetNumberOfSocketPresent ();
  for (i = 0; i < SocketCount; i++) {
    LogicalDieCount = (UINT8)FabricTopologyGetNumberOfLogicalDiesOnSocket(i);

    for (j = 0; j < LogicalDieCount; j++) {
      RbPerIOD = (UINT8)FabricTopologyGetNumberOfRootBridgesOnDie(i, j);

      for (k = 0; k < RbPerIOD; k++) {
        // Step 2: Build the bus range array for each logical nbio
        BusLength    = *(UINT8 *)PciBusMaps;
        BusNumLimite = BusNumBase + BusLength - 1;
        

        // Step 3: Set CFG_ADDRESS_MAP_REGISTER & CFG_ADDRESS_CTRL_REGISTER
        DstFabricId = (UINT16)FabricTopologyGetHostBridgeSystemFabricID (i, j, k);

        IDS_HDT_CONSOLE (CPU_TRACE, "    Socket %d Iod %d Rb %d Id %04X Bus Map: 0x%02x - 0x%02x\n", i, j, k, DstFabricId, BusNumBase, BusNumLimite);
        // Set CFG_ADDRESS_MAP_REGISTER
        FabricStSetCfgAddrMapReg (
          SocketCount,
          DstFabricId,
          BusNumBase,
          BusNumLimite
          );

        // Set CFG_ADDRESS_CTRL_REGISTER
        CfgAddrCntl.Field.SecBusNum = BusNumBase;
        FabricRegisterAccWrite (
          i,
          j,
          CFGADDRESSCTRL_FUNC,
          CFGADDRESSCTRL_REG,
          IomsInstanceIds[k],
          CfgAddrCntl.Value,
          FALSE
          );
        BusNumBase = BusNumBase + BusLength;
        PciBusMaps ++;
      }
    }
  }
}
