/* $NoKeywords:$ */
/**
 * @file
 *
 * Family specific PCIe configuration data
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <GnbHsio.h>
#include  <PcieComplexData.h>
#include  <Filecode.h>

#define FILECODE NBIO_NBIOBASE_HYEX_HYGONNBIOBASEPEI_PCIECOMPLEXDATA_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
//
// Complex configuration
//

ST_COMPLEX_CONFIG_MASTER_NBIO ComplexDataMasterST = {
  //Silicon
  {
    {//Header
      DESCRIPTOR_SILICON | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      0,
      0,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Silicon)
    },
    0,    // SocketId
    0,    // DieNumber
    0,    // RBIndex
    0,    // InstanceId
    0,    // Address
    GPP0_START_PHY_LANE,                           // StartLane
    GPP1_END_PHY_LANE - GPP0_START_PHY_LANE        // EndLane
  },
  //Gpp0 Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Silicon),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR7) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper)
    },
    GPP0_WRAP_ID,
    GPP0_NUMBER_OF_PIFs,
    GPP0_START_PHY_LANE,
    GPP0_END_PHY_LANE,
    GPP0_CORE_ID,                                   //StartPcieCoreId     [0:3]
    GPP0_CORE_ID,                                   //EndPcieCoreId       [4:7]
    GPP0_END_PHY_LANE - GPP0_START_PHY_LANE + 1,    //Number of Lanes
    {                                               //Feature
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      0,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      1,  //PllOffInL1
      1   //AccessEncoding
    },
    GNB_PCIE_MASTERPLL_A //MasterPll
  },
  //GPP1 Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Silicon),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp2Wrapper) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR15) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper)
    },
    GPP1_WRAP_ID,
    GPP1_NUMBER_OF_PIFs,
    GPP1_START_PHY_LANE,
    GPP1_END_PHY_LANE,
    GPP1_CORE_ID,                                 //StartPcieCoreId     [0:3]
    GPP1_CORE_ID,                                 //EndPcieCoreId       [4:7]
    GPP1_END_PHY_LANE - GPP1_START_PHY_LANE + 1,  //Number of Lanes
    {                                             //Feature
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      0,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      1,  //PllOffInL1
      1   //AccessEncoding
    },
    GNB_PCIE_MASTERPLL_A //MasterPll
  },
  //GPP2 Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp2Wrapper) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Silicon),
      0,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR17) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp2Wrapper)
    },
    GPP2_WRAP_ID,
    GPP2_NUMBER_OF_PIFs,
    GPP2_START_PHY_LANE,
    GPP2_END_PHY_LANE,
    GPP2_CORE_ID,                                 //StartPcieCoreId     [0:3]
    GPP2_CORE_ID,                                 //EndPcieCoreId       [4:7]
    GPP2_END_PHY_LANE - GPP2_START_PHY_LANE + 1,  //Number of Lanes
    {                                             //Feature
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      0,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      1,  //PllOffInL1
      1   //AccessEncoding
    },
    GNB_PCIE_MASTERPLL_A //MasterPll
  },
//------------------------------ GPP0 WRAPPER START-------------------------------------
  //Port PBR7
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR7) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR6) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR7),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR7_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR7_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR7_CORE_ID,         // CoreID                   [0:2]
        PBR7_PORT_ID,         // MacPortID                 [3:7]
        PBR7_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR7,                 // GfxWrkRetryCount
        PBR7_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR6
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR6) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR5) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR6),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR6_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR6_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR6_CORE_ID,         // CoreID                   [0:2]
        PBR6_PORT_ID,         // MacPortID                 [3:7]
        PBR6_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR6,                 // GfxWrkRetryCount
        PBR6_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR5
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR5) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR4) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR5),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR5_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR5_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR5_CORE_ID,         // CoreID                   [0:2]
        PBR5_PORT_ID,         // MacPortID                 [3:7]
        PBR5_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR5,                 // GfxWrkRetryCount
        PBR5_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR4
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR4) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR3) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR4),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR4_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR4_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR4_CORE_ID,         // CoreID                   [0:2]
        PBR4_PORT_ID,         // MacPortID                 [3:7]
        PBR4_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR4,                 // GfxWrkRetryCount
        PBR4_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR3
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR3) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR2) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR3),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR3_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR3_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR3_CORE_ID,         // CoreID                   [0:2]
        PBR3_PORT_ID,         // MacPortID                 [3:7]
        PBR3_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR3,                 // GfxWrkRetryCount
        PBR3_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR2
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR2) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR1) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR2),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR2_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR2_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR2_CORE_ID,         // CoreID                   [0:2]
        PBR2_PORT_ID,         // MacPortID                 [3:7]
        PBR2_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR2,                 // GfxWrkRetryCount
        PBR2_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR1
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR1) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR0) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR1),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR1_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR1_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR1_CORE_ID,         // CoreID                   [0:2]
        PBR1_PORT_ID,         // MacPortID                 [3:7]
        PBR1_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR1,                 // GfxWrkRetryCount
        PBR1_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR0
  {
    {
      DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR0) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR15) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR0),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID },                       // EngineData
    0,                                              // Initialization Status
    0xFF,                                           // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR0_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR0_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR0_CORE_ID,         // CoreID                   [0:2]
        PBR0_PORT_ID,         // MacPortID                 [3:7]
        PBR0_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR0,                 // GfxWrkRetryCount
        PBR0_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
//------------------------------ GPP0 WRAPPER END -------------------------------------
//------------------------------ GPP1 WRAPPER START----------------------------------
  //Port PBR15
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR15) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR14) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR15),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR15_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR15_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR15_CORE_ID,        // CoreID                   [0:2]
        PBR15_PORT_ID,        // MacPortID                 [3:7]
        PBR15_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR15,                // GfxWrkRetryCount
        PBR15_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR14
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR14) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR13) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR14),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR14_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR14_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR14_CORE_ID,        // CoreID                   [0:2]
        PBR14_PORT_ID,        // MacPortID                 [3:7]
        PBR14_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR14,                // GfxWrkRetryCount
        PBR14_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR13
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR13) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR12) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR13),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR13_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR13_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR13_CORE_ID,        // CoreID                   [0:2]
        PBR13_PORT_ID,        // MacPortID                 [3:7]
        PBR13_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR13,                // GfxWrkRetryCount
        PBR13_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR12
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR12) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR11) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR12),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR12_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR12_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR12_CORE_ID,        // CoreID                   [0:2]
        PBR12_PORT_ID,        // MacPortID                 [3:7]
        PBR12_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR12,                // GfxWrkRetryCount
        PBR12_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR11
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR11) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR10) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR11),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR11_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR11_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR11_CORE_ID,        // CoreID                   [0:2]
        PBR11_PORT_ID,        // MacPortID                 [3:7]
        PBR11_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR11,                // GfxWrkRetryCount
        PBR11_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR10
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR10) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR9) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR10),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR10_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR10_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR10_CORE_ID,        // CoreID                   [0:2]
        PBR10_PORT_ID,        // MacPortID                 [3:7]
        PBR10_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR10,                // GfxWrkRetryCount
        PBR10_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR9
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR9) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR8) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR9),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR9_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR9_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR9_CORE_ID,         // CoreID                   [0:2]
        PBR9_PORT_ID,         // MacPortID                 [3:7]
        PBR9_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR9,                 // GfxWrkRetryCount
        PBR9_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR8
  {
    {
      DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST,
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR8) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR17) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR8),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID },                         // EngineData
    0,                                                // Initialization Status
    0xFF,                                             // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR8_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR8_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR8_CORE_ID,         // CoreID                   [0:2]
        PBR8_PORT_ID,         // MacPortID                 [3:7]
        PBR8_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PcieBridgeId
        PBR8,                 // GfxWrkRetryCount
        PBR8_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //------------------------------ GPP2 WRAPPER START----------------------------------
    //Port PBR17
    {
      {
        DESCRIPTOR_PCIE_ENGINE,
        offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR17) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp2Wrapper),
        offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR16) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR17),
        0
      },
      { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
      0,                                                  // Initialization Status
      0xFF,                                               // Scratch
      {
        {
          {0},                  // Port Data  PCIe_PORT_DATA
          UNUSED_LANE_ID,       // Start Core Lane
          UNUSED_LANE_ID,       // End Core Lane
          PBR17_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
          PBR17_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
          PBR17_CORE_ID,        // CoreID                   [0:2]
          PBR17_PORT_ID,        // MacPortID                 [3:7]
          PBR17_PCI_ADDRESS,    // Address PCI_ADDRESS
          LinkStateResetExit,   // PcieBridgeId
          PBR17,                // GfxWrkRetryCount
          PBR17_UNIT_ID,        // LogicalBridgeId
          0,                    // Power Off Unused Lanes PLL     [0:0]
          1,                    // Power Off Unused Lanes PLL L1  [1:1]
          0,                    // DynamicLanesPowerState         [2:7]
          0,                    // Slot Power Limit
          5,                    // MAX_PAYLOAD
          0,                    // L1 Immediate ACK         [0:0]
          0,                    // TX De-emphasis           [1:4]
          0,                    // TX Margin                [5:7]
          0,                    // Scrambler control        [0:0]
          1,                    // Equalization Search Mode [1:2]
          0,                    // Disable Gen3 EQ Phase2/3 [3:3]
          0,                    // Enable PM L1 SS L1.1     [4:4]
          0,                    // Enable PM L1 SS L1.2     [5:5]
          0,                    // Reserved                 [6:7]
          0,                    // ClkReq                   [0:3]
          0,                    // Reserved                 [4:7]
          {0},                  // FlowCntlCredits
          {0,0,1},              // SpcMode
          {0},                  // LaneEqualizationCntl
          0,                    // Figure of Merit time
          0,                    // PortReversal             [0:0]
          0,                    // BypassGen3EQ             [1:1]
          1,                    // Enable3x3Search          [2:2]
          0,                    // EnableSafeSearch         [3:3]
          0,                    // SRIS SRNS                [4:4]
          0,                    // SRIS Enable              [5:5]
          0,                    // SRIS Auto Detect Enable  [6:6]
          0                     // SRIS Lower SKP Support   [7:7]
        },
      },
    },
    //Port PBR16
    {
      {
        DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
        offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, PortPBR16) - offsetof (ST_COMPLEX_CONFIG_MASTER_NBIO, Gpp2Wrapper),
        0,
        0
      },
      { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
      0,                                                  // Initialization Status
      0xFF,                                               // Scratch
      {
        {
          {0},                  // Port Data  PCIe_PORT_DATA
          UNUSED_LANE_ID,       // Start Core Lane
          UNUSED_LANE_ID,       // End Core Lane
          PBR16_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
          PBR16_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
          PBR16_CORE_ID,        // CoreID                   [0:2]
          PBR16_PORT_ID,        // MacPortID                [3:7]
          PBR16_PCI_ADDRESS,    // Address PCI_ADDRESS
          LinkStateResetExit,   // PcieBridgeId
          PBR16,                // GfxWrkRetryCount
          PBR16_UNIT_ID,        // LogicalBridgeId
          0,                    // Power Off Unused Lanes PLL     [0:0]
          1,                    // Power Off Unused Lanes PLL L1  [1:1]
          0,                    // DynamicLanesPowerState         [2:7]
          0,                    // Slot Power Limit
          5,                    // MAX_PAYLOAD
          0,                    // L1 Immediate ACK         [0:0]
          0,                    // TX De-emphasis           [1:4]
          0,                    // TX Margin                [5:7]
          0,                    // Scrambler control        [0:0]
          1,                    // Equalization Search Mode [1:2]
          0,                    // Disable Gen3 EQ Phase2/3 [3:3]
          0,                    // Enable PM L1 SS L1.1     [4:4]
          0,                    // Enable PM L1 SS L1.2     [5:5]
          0,                    // Reserved                 [6:7]
          0,                    // ClkReq                   [0:3]
          0,                    // Reserved                 [4:7]
          {0},                  // FlowCntlCredits
          {0,0,1},              // SpcMode
          {0},                  // LaneEqualizationCntl
          0,                    // Figure of Merit time
          0,                    // PortReversal             [0:0]
          0,                    // BypassGen3EQ             [1:1]
          1,                    // Enable3x3Search          [2:2]
          0,                    // EnableSafeSearch         [3:3]
          0,                    // SRIS SRNS                [4:4]
          0,                    // SRIS Enable              [5:5]
          0,                    // SRIS Auto Detect Enable  [6:6]
          0                     // SRIS Lower SKP Support   [7:7]
        },
      },
    },
};

ST_COMPLEX_CONFIG ComplexDataST = {
  //Silicon
  {
    {//Header
      DESCRIPTOR_SILICON | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      0,
      0,
      offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper) - offsetof (ST_COMPLEX_CONFIG, Silicon)
    },
    0,    // SocketId
    0,    // DieNumber
    0,    // RBIndex
    0,    // InstanceId
    0,    // Address
    GPP0_START_PHY_LANE,                           // StartLane
    GPP1_END_PHY_LANE - GPP0_START_PHY_LANE        // EndLane
  },
  //Gpp0 Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER,
      offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper) - offsetof (ST_COMPLEX_CONFIG, Silicon),
      offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR7) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper)
    },
    GPP0_WRAP_ID,
    GPP0_NUMBER_OF_PIFs,
    GPP0_START_PHY_LANE,
    GPP0_END_PHY_LANE,
    GPP0_CORE_ID,                                   //StartPcieCoreId     [0:3]
    GPP0_CORE_ID,                                   //EndPcieCoreId       [4:7]
    GPP0_END_PHY_LANE - GPP0_START_PHY_LANE + 1,    //Number of Lanes
    {                                               //Feature
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      0,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      1,  //PllOffInL1
      1   //AccessEncoding
    },
    GNB_PCIE_MASTERPLL_A //MasterPll
  },
  //GPP1 Wrapper
  {
    {
      DESCRIPTOR_PCIE_WRAPPER | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
      offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper) - offsetof (ST_COMPLEX_CONFIG, Silicon),
      0,
      offsetof (ST_COMPLEX_CONFIG, PortPBR15) - offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper)
    },
    GPP1_WRAP_ID,
    GPP1_NUMBER_OF_PIFs,
    GPP1_START_PHY_LANE,
    GPP1_END_PHY_LANE,
    GPP1_CORE_ID,                                 //StartPcieCoreId     [0:3]
    GPP1_CORE_ID,                                 //EndPcieCoreId       [4:7]
    GPP1_END_PHY_LANE - GPP1_START_PHY_LANE + 1,  //Number of Lanes
    {                                             //Feature
      1,  //PowerOffUnusedLanesEnabled,
      1,  //PowerOffUnusedPllsEnabled
      1,  //ClkGating
      0,  //LclkGating
      1,  //TxclkGatingPllPowerDown
      1,  //PllOffInL1
      1   //AccessEncoding
    },
    GNB_PCIE_MASTERPLL_A //MasterPll
  },
//------------------------------ GPP0 WRAPPER START-------------------------------------
  //Port PBR7
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR7) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR6) - offsetof (ST_COMPLEX_CONFIG, PortPBR7),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR7_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR7_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR7_CORE_ID,         // CoreID                   [0:3]
        PBR7_PORT_ID,         // PortID                   [4:7]
        PBR7_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR7,                 // GfxWrkRetryCount         [4:7]
        PBR7_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR6
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR6) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR5) - offsetof (ST_COMPLEX_CONFIG, PortPBR6),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR6_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR6_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR6_CORE_ID,         // CoreID                   [0:3]
        PBR6_PORT_ID,         // PortID                   [4:7]
        PBR6_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR6,                 // GfxWrkRetryCount         [4:7]
        PBR6_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR5
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR5) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR4) - offsetof (ST_COMPLEX_CONFIG, PortPBR5),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR5_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR5_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR5_CORE_ID,         // CoreID                   [0:3]
        PBR5_PORT_ID,         // PortID                   [4:7]
        PBR5_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR5,                 // GfxWrkRetryCount         [4:7]
        PBR5_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR4
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR4) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR3) - offsetof (ST_COMPLEX_CONFIG, PortPBR4),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR4_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR4_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR4_CORE_ID,         // CoreID                   [0:3]
        PBR4_PORT_ID,         // PortID                   [4:7]
        PBR4_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR4,                 // GfxWrkRetryCount         [4:7]
        PBR4_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR3
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR3) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR2) - offsetof (ST_COMPLEX_CONFIG, PortPBR3),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR3_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR3_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR3_CORE_ID,         // CoreID                   [0:3]
        PBR3_PORT_ID,         // PortID                   [4:7]
        PBR3_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR3,                 // GfxWrkRetryCount         [4:7]
        PBR3_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR2
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR2) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR1) - offsetof (ST_COMPLEX_CONFIG, PortPBR2),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR2_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR2_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR2_CORE_ID,         // CoreID                   [0:3]
        PBR2_PORT_ID,         // PortID                   [4:7]
        PBR2_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR2,                 // GfxWrkRetryCount         [4:7]
        PBR2_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR1
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR1) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR0) - offsetof (ST_COMPLEX_CONFIG, PortPBR1),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        4,                    // Start Core Lane
        7,                    // End Core Lane
        PBR1_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR1_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR1_CORE_ID,         // CoreID                   [0:3]
        PBR1_PORT_ID,         // PortID                   [4:7]
        PBR1_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR1,                 // GfxWrkRetryCount         [4:7]
        PBR1_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR0
  {
    {
      DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST,
      offsetof (ST_COMPLEX_CONFIG, PortPBR0) - offsetof (ST_COMPLEX_CONFIG, Gpp0Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR15) - offsetof (ST_COMPLEX_CONFIG, PortPBR0),
      0
    },
    { PciePortEngine, 0, 3 },                       // EngineData
    0,                                              // Initialization Status
    0xFF,                                           // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        0,                    // Start Core Lane
        3,                    // End Core Lane
        PBR0_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR0_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR0_CORE_ID,         // CoreID                   [0:3]
        PBR0_PORT_ID,         // PortID                   [4:7]
        PBR0_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR0,                 // GfxWrkRetryCount         [4:7]
        PBR0_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
//------------------------------ GPP0 WRAPPER END -------------------------------------
//------------------------------ GPP1 WRAPPER START----------------------------------
  //Port PBR15
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR15) - offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR14) - offsetof (ST_COMPLEX_CONFIG, PortPBR15),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR15_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR15_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR15_CORE_ID,        // CoreID                   [0:3]
        PBR15_PORT_ID,        // PortID                   [4:7]
        PBR15_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR15,                // GfxWrkRetryCount         [4:7]
        PBR15_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR14
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR14) - offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR13) - offsetof (ST_COMPLEX_CONFIG, PortPBR14),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR14_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR14_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR14_CORE_ID,        // CoreID                   [0:3]
        PBR14_PORT_ID,        // PortID                   [4:7]
        PBR14_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR14,                // GfxWrkRetryCount         [4:7]
        PBR14_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR13
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR13) - offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR12) - offsetof (ST_COMPLEX_CONFIG, PortPBR13),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR13_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR13_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR13_CORE_ID,        // CoreID                   [0:3]
        PBR13_PORT_ID,        // PortID                   [4:7]
        PBR13_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR13,                // GfxWrkRetryCount         [4:7]
        PBR13_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR12
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR12) - offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR11) - offsetof (ST_COMPLEX_CONFIG, PortPBR12),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR12_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR12_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR12_CORE_ID,        // CoreID                   [0:3]
        PBR12_PORT_ID,        // PortID                   [4:7]
        PBR12_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR12,                // GfxWrkRetryCount         [4:7]
        PBR12_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR11
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR11) - offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR10) - offsetof (ST_COMPLEX_CONFIG, PortPBR11),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR11_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR11_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR11_CORE_ID,        // CoreID                   [0:3]
        PBR11_PORT_ID,        // PortID                   [4:7]
        PBR11_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR11,                // GfxWrkRetryCount         [4:7]
        PBR11_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR10
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR10) - offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR9) - offsetof (ST_COMPLEX_CONFIG, PortPBR10),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        UNUSED_LANE_ID,       // Start Core Lane
        UNUSED_LANE_ID,       // End Core Lane
        PBR10_NATIVE_PCI_DEV, // NativeDevNumber          [0:4]
        PBR10_NATIVE_PCI_FUN, // NativeFunNumber          [5:7]
        PBR10_CORE_ID,        // CoreID                   [0:3]
        PBR10_PORT_ID,        // PortID                   [4:7]
        PBR10_PCI_ADDRESS,    // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR10,                // GfxWrkRetryCount         [4:7]
        PBR10_UNIT_ID,        // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR9
  {
    {
      DESCRIPTOR_PCIE_ENGINE,
      offsetof (ST_COMPLEX_CONFIG, PortPBR9) - offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper),
      offsetof (ST_COMPLEX_CONFIG, PortPBR8) - offsetof (ST_COMPLEX_CONFIG, PortPBR9),
      0
    },
    { PciePortEngine, UNUSED_LANE_ID, UNUSED_LANE_ID},  // EngineData
    0,                                                  // Initialization Status
    0xFF,                                               // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        4,                    // Start Core Lane
        7,                    // End Core Lane
        PBR9_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR9_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR9_CORE_ID,         // CoreID                   [0:3]
        PBR9_PORT_ID,         // PortID                   [4:7]
        PBR9_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR9,                 // GfxWrkRetryCount         [4:7]
        PBR9_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
  //Port PBR8
  {
    {
      DESCRIPTOR_PCIE_ENGINE | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY,
     offsetof (ST_COMPLEX_CONFIG, PortPBR8) - offsetof (ST_COMPLEX_CONFIG, Gpp1Wrapper),
      0,
      0
    },
    { PciePortEngine, 0, 3 },                         // EngineData
    0,                                                // Initialization Status
    0xFF,                                             // Scratch
    {
      {
        {0},                  // Port Data  PCIe_PORT_DATA
        0,                    // Start Core Lane
        3,                    // End Core Lane
        PBR8_NATIVE_PCI_DEV,  // NativeDevNumber          [0:4]
        PBR8_NATIVE_PCI_FUN,  // NativeFunNumber          [5:7]
        PBR8_CORE_ID,         // CoreID                   [0:3]
        PBR8_PORT_ID,         // PortID                   [4:7]
        PBR8_PCI_ADDRESS,     // Address PCI_ADDRESS
        LinkStateResetExit,   // PciBridgeId              [0:3]
        PBR8,                 // GfxWrkRetryCount         [4:7]
        PBR8_UNIT_ID,         // LogicalBridgeId
        0,                    // Power Off Unused Lanes PLL     [0:0]
        1,                    // Power Off Unused Lanes PLL L1  [1:1]
        0,                    // DynamicLanesPowerState         [2:7]
        0,                    // Slot Power Limit
        5,                    // MAX_PAYLOAD
        0,                    // L1 Immediate ACK         [0:0]
        0,                    // TX De-emphasis           [1:4]
        0,                    // TX Margin                [5:7]
        0,                    // Scrambler control        [0:0]
        1,                    // Equalization Search Mode [1:2]
        0,                    // Disable Gen3 EQ Phase2/3 [3:3]
        0,                    // Enable PM L1 SS L1.1     [4:4]
        0,                    // Enable PM L1 SS L1.2     [5:5]
        0,                    // Reserved                 [6:7]
        0,                    // ClkReq                   [0:3]
        0,                    // Reserved                 [4:7]
        {0},                  // FlowCntlCredits
        {0,0,1},              // SpcMode
        {0},                  // LaneEqualizationCntl
        0,                    // Figure of Merit time
        0,                    // PortReversal             [0:0]
        0,                    // BypassGen3EQ             [1:1]
        1,                    // Enable3x3Search          [2:2]
        0,                    // EnableSafeSearch         [3:3]
        0,                    // SRIS SRNS                [4:4]
        0,                    // SRIS Enable              [5:5]
        0,                    // SRIS Auto Detect Enable  [6:6]
        0                     // SRIS Lower SKP Support   [7:7]
      },
    },
  },
};



