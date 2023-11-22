/* $NoKeywords:$ */

/**
 * @file
 *
 * Low-level Fabric Topology Services base library
 *
 * Contains interface to the family specific fabric topology base library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
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
 **/

#ifndef _BASE_FABRIC_TOPOLOGY_LIB_H_
#define _BASE_FABRIC_TOPOLOGY_LIB_H_
#pragma pack (push, 1)

/// Fabric Device Types
typedef enum {
  Ccm,                         ///< Processor Family Specific Workarounds which are @b not practical using the other types.
  Gcm,                         ///< Processor Config Space registers via SMN.
  Ncs,                         ///< Processor Config Space registers via SMN.
  Ncm,                         ///< Processor Config Space registers via SMN.
  Pie,                         ///< Processor Config Space registers via SMN.
  Ioms,                        ///< Processor Config Space registers via SMN.
  Cs,                          ///< Processor Config Space registers via SMN.
  Tcdx,                        ///< Processor Config Space registers via SMN.
  Cake,                        ///< Processor Config Space registers via SMN.
  FabricDeviceTypeMax          ///< Not a valid entry type, use for limit checking.
} FABRIC_DEVICE_TYPE;

/// Device ID structure
typedef struct {
  UINTN    FabricID;          ///< Fabric ID
  UINTN    InstanceID;        ///< Instance ID
} DEVICE_IDS;

/// Processor neighbor information
typedef struct {
  UINTN    SocketNumber;      ///< Socket Number
} HYGON_FABRIC_TOPOLOGY_PROCESSOR_NEIGHBOR_INFO;

/// Fabric topology structure
typedef struct {
  FABRIC_DEVICE_TYPE    Type;         ///< Type
  UINTN                 Count;        ///< Count
  CONST DEVICE_IDS      *IDs;         ///< Device IDs
} HYGON_FABRIC_TOPOLOGY_DIE_DEVICE_MAP;

/// Fabric topology neighbor information structure
typedef struct {
  UINTN    SocketNumber;            ///< Socket Number
  UINTN    InternalDieNumber;       ///< Internal Die Number
  UINTN    HostCake;                ///< Host Cake
  UINTN    NeighborCake;            ///< Neighbor Cake
} HYGON_FABRIC_TOPOLOGY_DIE_NEIGHBOR_INFO;

UINTN
FabricTopologyGetNumberOfSocketPresent (
  VOID
  );

UINTN
FabricTopologyGetNumberOfSystemDies (
  VOID
  );

UINTN
FabricTopologyGetNumberOfSystemRootBridges (
  VOID
  );

UINTN
FabricTopologyGetNumberOfPhysicalDiesOnSocket (
  IN       UINTN Socket
  );

VOID
FabricTopologyGetPhysicalIodDieInfo (
  IN       UINTN PhysicalDieId,
  OUT      UINTN *LogicalDieId,
  OUT      UINTN *DieType
  );

UINTN
FabricTopologyGetNumberOfLogicalDiesOnSocket (
  IN       UINTN Socket
  );

BOOLEAN
IsEmeiPresent (
  VOID
  );

UINTN
FabricTopologyGetEmeiPhysicalDieId (
  IN       UINTN LogicalDieId
  );

UINTN
FabricTopologyGetDjPhysicalDieId (
  IN       UINTN LogicalDieId
  );

UINTN
FabricTopologyGetIohubPhysicalDieId (
  IN       UINTN LogicalDieId
  );

UINTN
FabricTopologyGetNumberOfCddsOnSocket (
  IN       UINTN Socket
  );

UINTN
FabricTopologyGetCddsPresentOnSocket (
  IN       UINTN Socket
  );

UINTN
FabricTopologyGetFirstPhysCddIdOnSocket (
  IN       UINTN Socket
  );
 
UINTN
FabricTopologyGetNumberOfRootBridgesOnSocket (
  IN       UINTN Socket
  );

UINTN
FabricTopologyGetNumberOfRootBridgesOnDie (
  IN       UINTN Socket,
  IN       UINTN LogicalDie
  );

UINTN
FabricTopologyGetDieSystemOffset (
  IN       UINTN Socket,
  IN       UINTN LogicalDie
  );

VOID
FabricTopologyGetLocationFromFabricId (
  IN       UINT32 FabricId,
  OUT      UINT32 *SocketId,
  OUT      UINT32 *DieId,
  OUT      UINT32 *NbioId    
  );
  
CONST
HYGON_FABRIC_TOPOLOGY_DIE_DEVICE_MAP *
FabricTopologyGetDeviceMapOnDie (
  IN       UINTN Socket,
  IN       UINTN LogicalDie
  );

UINTN
FabricTopologyGetHostBridgeSystemFabricID (
  IN       UINTN Socket,
  IN       UINTN LogicalDie,
  IN       UINTN NbioIndex
  );

UINTN
FabricTopologyGetHostBridgeBusBase (
  IN       UINTN Socket,
  IN       UINTN LogicalDie,
  IN       UINTN NbioIndex
  );

UINTN
FabricTopologyGetHostBridgeBusLimit (
  IN       UINTN Socket,
  IN       UINTN LogicalDie,
  IN       UINTN NbioIndex
  );

/*
 * Check Physical CDD is present ?
  1: Present
  0: Not Present
 */
#ifndef IS_CDD_PRESENT
  #define  IS_CDD_PRESENT(CDDINDEX, CDDSPRESENT)  (BOOLEAN)((((UINTN)1 << CDDINDEX) & CDDSPRESENT) != 0)
#endif

#pragma pack (pop)
#endif // _BASE_FABRIC_TOPOLOGY_LIB_H_
