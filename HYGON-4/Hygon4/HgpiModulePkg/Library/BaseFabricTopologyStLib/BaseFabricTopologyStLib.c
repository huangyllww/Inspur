/* $NoKeywords:$ */

/**
 * @file
 *
 * Fabric Topology Base Lib implementation for SAT
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 *
 */
/*
 ******************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/CcxBaseX86Lib.h>
#include <Library/FabricRegisterAccLib.h>
#include <FabricRegistersST.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_BASEFABRICTOPOLOGYSTLIB_BASEFABRICTOPOLOGYSTLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

CONST DEVICE_IDS ROMDATA  CsMap[] = {
  { 0x00000000, 0x00000000 },
  { 0x00000001, 0x00000001 }
};

CONST DEVICE_IDS ROMDATA  CcmMap[] = {
  { 0x00000004, 0x00000004 },
  { 0x00000005, 0x00000005 },
  { 0x00000006, 0x00000006 },
  { 0x00000007, 0x00000007 }
};

CONST DEVICE_IDS ROMDATA  IomsMap[] = {
  { 0x00000008, 0x00000008 },
  { 0x00000009, 0x00000009 },
  { 0x0000000A, 0x0000000A },
  { 0x0000000B, 0x0000000B }
};

CONST DEVICE_IDS ROMDATA  PieMap[] = {
  { 0x0000000C, 0x0000000C }
};

CONST HYGON_FABRIC_TOPOLOGY_DIE_DEVICE_MAP ROMDATA  StDeviceMap[] = {
  { Cs,   (UINTN)(sizeof (CsMap) / sizeof (CsMap[0])),     &CsMap[0]   },
  { Ccm,  (UINTN)(sizeof (CcmMap) / sizeof (CcmMap[0])),   &CcmMap[0]  },
  { Ioms, (UINTN)(sizeof (IomsMap) / sizeof (IomsMap[0])), &IomsMap[0] },
  { Pie,  (UINTN)(sizeof (PieMap) / sizeof (PieMap[0])),   &PieMap[0]  },
};

UINTN
FabricTopologyGetNumberOfSocketPresent (
  VOID
  )
{
  SYS_CFG_REGISTER  SysCfg;

  SysCfg.Value = FabricRegisterAccRead (0, 0, SYSCFG_FUNC, SYSCFG_REG, FABRIC_REG_ACC_BC);

  return (UINTN)LowBitSet32 (SysCfg.Field.SocketPresent + 1);
}

UINTN
FabricTopologyGetNumberOfSystemDies (
  VOID
  )
{
  SYS_COMP_COUNT_REGISTER  SystemCompCount;

  SystemCompCount.Value = FabricRegisterAccRead (0, 0, SYSCOMPCOUNT_FUNC, SYSCOMPCOUNT_REG, FABRIC_REG_ACC_BC);

  // Return IO die and Core die total number
  return (UINTN)SystemCompCount.Field.PIECount;
}

UINTN
FabricTopologyGetNumberOfSystemRootBridges (
  VOID
  )
{
  SYS_COMP_COUNT_REGISTER  SystemCompCount;

  SystemCompCount.Value = FabricRegisterAccRead (0, 0, SYSCOMPCOUNT_FUNC, SYSCOMPCOUNT_REG, FABRIC_REG_ACC_BC);

  return (UINTN)SystemCompCount.Field.IODIOSCount;
}

/*
 * Get physical IOD number On socket
 */
UINTN
FabricTopologyGetNumberOfPhysicalDiesOnSocket (
  IN       UINTN Socket
  )
{
  UINTN  PhysicalIodNumber = 0;
  UINT32 CpuModel;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    PhysicalIodNumber = 1;  //Satori
  }

  if (CpuModel == HYGON_GX_CPU) {
    if (IsEmeiPresent ()) {
      PhysicalIodNumber = FabricTopologyGetNumberOfLogicalDiesOnSocket (Socket) + 1;
    } else {
      PhysicalIodNumber = FabricTopologyGetNumberOfLogicalDiesOnSocket (Socket);
    }
  }
  return PhysicalIodNumber;
}

/*
 * Get physical IOD's Logical Die Id and die type
 */
VOID
FabricTopologyGetPhysicalIodDieInfo (
  IN       UINTN PhysicalDieId,
  OUT      UINTN *LogicalDieId,
  OUT      UINTN *DieType
  )
{
  UINT32 CpuModel;

  if ((LogicalDieId == NULL) || (DieType == NULL)) {
    return;
  }

  if (PhysicalDieId >= FabricTopologyGetNumberOfPhysicalDiesOnSocket (0)) {
    return;
  }

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    *LogicalDieId = 0;
    *DieType = IOD_SATORI;
    return;
  }

  if (CpuModel == HYGON_GX_CPU) {
    if (IsEmeiPresent ()) {
      // System has DUJIANG and EMEI
      switch (PhysicalDieId) {
        case 0:
          *LogicalDieId = 0;
          *DieType = IOD_DUJIANG;
          break;
        case 1:
          *LogicalDieId = 0;
          *DieType = IOD_EMEI;
          break;
        case 2:
          *LogicalDieId = 1;
          *DieType = IOD_EMEI;
          break;
        case 3:
          *LogicalDieId = 1;
          *DieType = IOD_DUJIANG;
          break;
        default:
          *LogicalDieId = 0xFF;
          *DieType = IOD_NULL;
          break;
      }
    } else {
      // System only has DUJIANG
      *LogicalDieId = PhysicalDieId;
      *DieType = IOD_DUJIANG;
    }
  }
}

/*
 * Get logical IOD number On socket
 */
UINTN
FabricTopologyGetNumberOfLogicalDiesOnSocket (
  IN       UINTN Socket
  )
{
  SYS_CFG2_REGISTER  SysCfg2;
  INTN               BitIndex;
  INTN               LogicalIodCount = 0;

  SysCfg2.Value = FabricRegisterAccRead (Socket, 0, SYSCFG_FUNC, SYSCFG2_REG, FABRIC_REG_ACC_BC);

  for (BitIndex = 0; BitIndex < 4; BitIndex++) {
    // BIT[3:0] Logical IOD die present bits
    if ((SysCfg2.Field.LocalDiePresent & (1 << BitIndex)) != 0) {
      LogicalIodCount++;
    }
  }

  return LogicalIodCount;
}

/*
 * Only HyGx support, System has EMEI IOD ?
 * return: TRUE  : Yes
 *         FALSE : No
 */
BOOLEAN
IsEmeiPresent (
  VOID
  )
{
  UINT32                     RegEbx;
  UINT8                      PkgType;
  UINT32                     CpuModel;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    return FALSE;
  }

  AsmCpuid (0x80000001, NULL, &RegEbx, NULL, NULL);
  PkgType = (UINT8)(RegEbx >> 28) & 0xF;

  if ((PkgType == DM1_102) || (PkgType == SP6_204)) {
    // 2 or 4 Dujiang
    return FALSE;
  } else {
    return TRUE;
  }
}

/*
 * Get the Emei physical die Id by logical Die Id
 */
UINTN
FabricTopologyGetEmeiPhysicalDieId (
  IN       UINTN LogicalDieId
  )
{
  UINTN  PhysicalDieId = 0;

  PhysicalDieId = LogicalDieId + 1;

  return PhysicalDieId;
}

/*
 * Get the DJ physical die Id by logical Die Id
 */
UINTN
FabricTopologyGetDjPhysicalDieId (
  IN       UINTN LogicalDieId
  )
{
  UINTN  PhysicalDieId = 0;

  if (IsEmeiPresent ()) {
    if (LogicalDieId == 0) {
      PhysicalDieId = 0;
    }

    if (LogicalDieId == 1) {
      PhysicalDieId = 3;
    }
  } else {
    PhysicalDieId = LogicalDieId;
  }

  return PhysicalDieId;
}

/*
 * Get the IOHUB physical die Id by logical Die Id
 */
UINTN
FabricTopologyGetIohubPhysicalDieId (
  IN       UINTN LogicalDieId
  )
{
  UINTN  IohubPhysicalDieId;
  UINT32 CpuModel;

  CpuModel = GetHygonSocModel();
  if ((CpuModel == HYGON_GX_CPU) && IsEmeiPresent ()) {
    IohubPhysicalDieId = LogicalDieId + 1;
  } else {
    IohubPhysicalDieId = LogicalDieId;
  }

  return IohubPhysicalDieId;
}

UINTN
FabricTopologyGetNumberOfCddsOnSocket (
  IN       UINTN Socket
  )
{
  SYS_CFG2_REGISTER  SysCfg2;
  INTN               BitIndex;
  INTN               CddCount = 0;

  SysCfg2.Value = FabricRegisterAccRead (Socket, 0, SYSCFG_FUNC, SYSCFG2_REG, FABRIC_REG_ACC_BC);

  for (BitIndex = 4; BitIndex < 12; BitIndex++) {
    // BIT[11:4] CDD die present bits
    if ((SysCfg2.Field.LocalDiePresent & (1 << BitIndex)) != 0) {
      CddCount++;
    }
  }

  return CddCount;
}

UINTN
FabricTopologyGetCddsPresentOnSocket (
  IN       UINTN Socket
  )
{
  SYS_CFG_CONVERT_REGISTER   SysCfgConvert;
  SYS_CFG2_REGISTER          SysCfg2;
  INTN                       CddPresent = 0;
  UINT32                     CpuModel;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    SysCfg2.Value = FabricRegisterAccRead (Socket, 0, SYSCFG_FUNC, SYSCFG2_REG, FABRIC_REG_ACC_BC);
    //BIT[11:4] CDD die present bits
    CddPresent = (SysCfg2.Field.LocalDiePresent >> 4) & 0xFF;
  } 
  
  if (CpuModel == HYGON_GX_CPU) {
    SysCfgConvert.Value = FabricRegisterAccRead (Socket, 0, SYS_CFG_CONVERT_FUNC, SYS_CFG_CONVERT_REG, FABRIC_REG_ACC_BC);
    // BIT[11:4] CDD die present bits
    CddPresent = (SysCfgConvert.Field.LocalDiePresent >> 4) & 0xFF;
  }
  return CddPresent;
}

UINTN
FabricTopologyGetFirstPhysCddIdOnSocket (
  IN       UINTN Socket
  )
{
  UINTN  CddsPresent;
  UINTN  CddIndex;

  CddsPresent = FabricTopologyGetCddsPresentOnSocket (Socket);
  for (CddIndex = 0; CddIndex < MAX_CDDS_PER_SOCKET; CddIndex++ ) {
    if (IS_CDD_PRESENT (CddIndex, CddsPresent)) {
      break;
    }
  }

  return CddIndex;
}

UINTN
FabricTopologyGetNumberOfRootBridgesOnSocket (
  IN       UINTN Socket
  )
{
  UINTN                    RbCount;
  SYS_COMP_COUNT_REGISTER  SystemCompCount;

  SystemCompCount.Value = FabricRegisterAccRead (Socket, 0, SYSCOMPCOUNT_FUNC, SYSCOMPCOUNT_REG, FABRIC_REG_ACC_BC);
  RbCount = (UINTN)DivU64x32 (SystemCompCount.Field.IODIOSCount, (UINT32)FabricTopologyGetNumberOfSocketPresent ());
  return (UINTN)RbCount;
}

/*
 * Get Iod Root bridge number
 * Socket : Socket Id
 * Die    : Logical Die Id
 */
UINTN
FabricTopologyGetNumberOfRootBridgesOnDie (
  IN       UINTN Socket,
  IN       UINTN LogicalDie
  )
{
  UINTN                    RbCount;
  SYS_COMP_COUNT_REGISTER  SystemCompCount;

  SystemCompCount.Value = FabricRegisterAccRead (Socket, 0, SYSCOMPCOUNT_FUNC, SYSCOMPCOUNT_REG, FABRIC_REG_ACC_BC);
  RbCount = (UINTN)DivU64x32 (SystemCompCount.Field.IODIOSCount, (UINT32)FabricTopologyGetNumberOfSocketPresent ());
  RbCount = (UINTN)DivU64x32 (RbCount, (UINT32)FabricTopologyGetNumberOfLogicalDiesOnSocket (Socket));

  return (UINTN)RbCount;
}

UINTN
FabricTopologyGetDieSystemOffset (
  IN       UINTN Socket,
  IN       UINTN LogicalDie
  )
{
  UINT32 CpuModel;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    return ((Socket << FABRIC_ID_SOCKET_SHIFT_HYEX) | (LogicalDie << FABRIC_ID_DIE_SHIFT));
  } else {
    return ((Socket << FABRIC_ID_SOCKET_SHIFT_HYGX) | (LogicalDie << FABRIC_ID_DIE_SHIFT));
  }
}

/*
Get Socket, Die, NBIO Id from FabricId
*/
VOID
FabricTopologyGetLocationFromFabricId (
  IN       UINT32 FabricId,
  OUT      UINT32 *SocketId,
  OUT      UINT32 *DieId,
  OUT      UINT32 *NbioId  
  )
{
  UINT32    ComponentId;
  UINT32    CpuModel;
  UINT32    SocketShift;
  UINT32    DieMask;
  UINT32    LocalDieId;

  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    SocketShift = FABRIC_ID_SOCKET_SHIFT_HYEX;
    DieMask = FABRIC_ID_DIE_SIZE_MASK_HYEX;
  } else {
    SocketShift = FABRIC_ID_SOCKET_SHIFT_HYGX;
    DieMask = FABRIC_ID_DIE_SIZE_MASK_HYGX;
  }

  if (SocketId != NULL) {
    *SocketId = (FabricId >> SocketShift) & FABRIC_ID_SOCKET_SIZE_MASK;
  }

  LocalDieId = (FabricId >> FABRIC_ID_DIE_SHIFT) & DieMask;
  if (DieId != NULL) {
    *DieId = LocalDieId;
  }
  if ((NbioId != NULL) && (LocalDieId < FABRIC_ID_CDD0_DIE_NUM)) {
    ComponentId = (FabricId >> FABRIC_ID_COMPONENT_SHIFT) & FABRIC_ID_COMPONENT_SIZE_MASK;
    
    if ((ComponentId >= IOMS0_INSTANCE_ID) && (ComponentId <= IOMS3_INSTANCE_ID)) {
      *NbioId = ComponentId - IOMS0_INSTANCE_ID;
    } else if ((ComponentId >= CS0_INSTANCE_ID) && (ComponentId <= CS3_INSTANCE_ID)) {
      *NbioId = ComponentId - CS0_INSTANCE_ID;
    }
  }
}

CONST
HYGON_FABRIC_TOPOLOGY_DIE_DEVICE_MAP *
FabricTopologyGetDeviceMapOnDie (
  IN       UINTN Socket,
  IN       UINTN LogicalDie
  )
{
  return &StDeviceMap[0];
}

UINTN
FabricTopologyGetHostBridgeSystemFabricID (
  IN       UINTN Socket,
  IN       UINTN LogicalDie,
  IN       UINTN NbioIndex
  )
{
  return (FabricTopologyGetDieSystemOffset (Socket, LogicalDie) + IomsMap[NbioIndex].FabricID);
}

UINTN
FabricTopologyGetHostBridgeBusBase (
  IN       UINTN Socket,
  IN       UINTN LogicalDie,
  IN       UINTN NbioIndex
  )
{
  CFG_ADDRESS_CTRL_REGISTER  CfgAddrCtrl;

  CfgAddrCtrl.Value = FabricRegisterAccRead (Socket, LogicalDie, CFGADDRESSCTRL_FUNC, CFGADDRESSCTRL_REG, IomsMap[NbioIndex].InstanceID);
  return (UINTN)(CfgAddrCtrl.Field.SecBusNum);
}

UINTN
FabricTopologyGetHostBridgeBusLimit (
  IN       UINTN Socket,
  IN       UINTN LogicalDie,
  IN       UINTN NbioIndex
  )
{
  UINTN                     BusLimit;
  UINTN                     Register;
  CFG_ADDRESS_MAP_REGISTER  BusMap;
  UINTN                     TargetFabricId;

  TargetFabricId = FabricTopologyGetHostBridgeSystemFabricID (Socket, LogicalDie, NbioIndex);
  BusLimit = 0xFF;

  for (Register = CFGADDRESSMAP0_REG; Register <= CFGADDRESSMAP15_REG; Register += 4) {
    BusMap.Value = FabricRegisterAccRead (0, 0, CFGADDRESSMAP_FUNC, Register, FABRIC_REG_ACC_BC);
    if ((BusMap.Field.RE == 1) && (BusMap.Field.WE == 1) && (BusMap.Field.DstFabricID == (UINT32)TargetFabricId)) {
      BusLimit = (UINTN)BusMap.Field.BusNumLimit;
      break;
    }
  }

  return BusLimit;
}
