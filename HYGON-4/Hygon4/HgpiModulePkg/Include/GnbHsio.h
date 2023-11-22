/* $NoKeywords:$ */

/**
 * @file
 *
 * PCIe component definitions.
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

#ifndef _GNBHSIO_H_
#define _GNBHSIO_H_

#pragma pack (push, 1)

#include <HGPI.h>
#include <Gnb.h>

#define MAX_NUMBER_OF_COMPLEXES_HYEX            32
#define MAX_NUMBER_OF_COMPLEXES_HYGX            36

#define LANE_COUNT_PER_NBIO_EMEI               16
#define LANE_COUNT_PER_NBIO_DUJIANG            16
#define LANE_COUNT_PER_NBIO_SATORI             32

#define DESCRIPTOR_TERMINATE_GNB       0x40000000ull
#define DESCRIPTOR_TERMINATE_TOPOLOGY  0x20000000ull
#define DESCRIPTOR_ALLOCATED           0x10000000ull
#define DESCRIPTOR_PLATFORM            0x08000000ull
#define DESCRIPTOR_COMPLEX             0x04000000ull
#define DESCRIPTOR_SILICON             0x02000000ull
#define DESCRIPTOR_PCIE_WRAPPER        0x01000000ull
#define DESCRIPTOR_PCIE_ENGINE         0x00800000ull

#define DESCRIPTOR_ALL_WRAPPERS  (DESCRIPTOR_PCIE_WRAPPER)
#define DESCRIPTOR_ALL_ENGINES   (DESCRIPTOR_PCIE_ENGINE)

#define DESCRIPTOR_ALL_TYPES     (DESCRIPTOR_ALL_WRAPPERS | DESCRIPTOR_ALL_ENGINES | DESCRIPTOR_SILICON | DESCRIPTOR_PLATFORM)

#define UNUSED_LANE_ID  136

/// PCIe Link Training State
typedef enum {
  LinkStateResetAssert         =  0,       ///< Assert port GPIO reset
  LinkStateResetDuration       =  1,       ///< Timeout for reset duration
  LinkStateResetExit           =  2,       ///< Deassert port GPIO reset
  LinkTrainingResetEnter       =  3,       ///< Entry state before Port GPIO reset timeout
  LinkTrainingResetTimeout     =  4,       ///< Port GPIO reset timeout
  LinkStateReleaseTraining     =  5,       ///< Release link training
  LinkStateDetectPresence      =  6,       ///< Detect device presence
  LinkStateDetecting           =  7,       ///< Detect link training.
  LinkStateBrokenLane          =  8,       ///< Check and handle broken lane
  LinkStateGen2Fail            =  9,       ///< Check and handle device that fail training if GEN2 capability advertised
  LinkStateL0                  = 10,       ///< Device trained to L0
  LinkStateVcoNegotiation      = 11,       ///< Check VCO negotiation complete
  LinkStateRetrain             = 12,       ///< Force retrain link.
  LinkStateTrainingFail        = 13,       ///< Link training fail
  LinkStateTrainingSuccess     = 14,       ///< Link training success
  LinkStateGfxWorkaround       = 15,       ///< GFX workaround
  LinkStateCompliance          = 16,       ///< Link in compliance mode
  LinkStateDeviceNotPresent    = 17,       ///< Link is not connected
  LinkStateTrainingCompleted   = 18        ///< Link training completed
} PCIE_LINK_TRAINING_STATE;

typedef enum {
  DETECT_QUIET,               //0X00
  START_COMMON_MODE,          //0X01
  CHECK_COMMON_MODE,          //0X02
  RCVR_DETECT,                //0X03
  NO_RCVR_LOOP,               //0X04
  POLL_QUIET,                 //0X05
  POLL_ACTIVE,                //0X06
  POLL_COMPLIANCE,            //0X07
  POLL_CONFIG,                //0X08
  CONFIG_STEP1,               //0X09
  CONFIG_STEP3,               //0X0A
  CONFIG_STEP5,               //0X0B
  CONFIG_STEP2,               //0X0C
  CONFIG_STEP4,               //0X0D
  CONFIG_STEP6,               //0X0E
  CONFIG_IDLE,                //0X0F
  RCV_L0_AND_TX_L0,           //0X10
  RCV_L0_AND_TX_L0_IDLE,      //0X11
  RCV_L0_AND_TX_L0S,          //0X12
  RCV_L0_AND_TX_L0S_FTS,      //0X13
  RCV_L0S_AND_TX_L0,          //0X14
  RCV_L0S_AND_TX_L0_IDLE,     //0X15
  RCV_L0S_AND_TX_L0S,         //0X16
  RCV_L0S_AND_TX_L0S_FTS,     //0X17
  L1_ENTRY,                   //0X18
  L1_IDLE,                    //0X19
  L1_WAIT,                    //0X1A
  L1,                         //0X1B
  L23_STALL,                  //0X1C
  L23_ENTRY,                  //0X1D
  L23_IDLE,                   //0X1E
  L23_READY,                  //0X1F
  RECOVERY_LOCK,              //0X20
  RECOVERY_CONFIG,            //0X21
  RECOVERY_IDLE,              //0X22
  TRAINING_BIT,               //0X23
  RCVD_LOOPBACK,              //0X24
  RCVD_LOOPBACK_IDLE,         //0X25
  RCVD_RESET_IDLE,            //0X26
  RCVD_DISABLE_ENTRY,         //0X27
  RCVD_DISABLE_IDLE,          //0X28
  RCVD_DISABLE,               //0X29
  DETECT_IDLE,                //0X2A
  L23_WAIT,                   //0X2B
  RCV_L0S_SKP_AND_TX_L0,      //0X2C
  RCV_L0S_SKP_AND_TX_L0_IDLE, //0X2D
  RCV_L0S_SKP_AND_TX_L0S,     //0X2E
  RCV_L0S_SKP_AND_TX_L0S_FTS, //0X2F
  CONFIG_STEP2B,              //0X30
  RECOVERY_SPEED,             //0X31
  POLL_COMPLIANCE_IDLE,       //0X32
  RCVD_LOOPBACK_SPEED,        //0X33
  RECOVERY_EQ0,               //0X34
  RECOVERY_EQ1,               //0X35
  RECOVERY_EQ2,               //0X36
  RECOVERY_EQ3,               //0X37
  L1_1,                       //0X38
  L1_2_ENTRY,                 //0X39
  L1_2_IDLE,                  //0X3A
  L1_2_EXIT,                  //0X3B
  RESERVED_3C,                //0X3C
  RESERVED_3D,                //0X3D
  RESERVED_3E,                //0X3E
  ILLEGAL_STATE               //0X3F
} LTSSM_STATE;

typedef enum {
  PCIE_INIT,
  AFTER_RECONFIG,
  AFTER_TRAINING
} PCIE_TRAINING_PHASE;

#define INIT_STATUS_PCIE_PORT_GEN2_RECOVERY         0x00000001ull
#define INIT_STATUS_PCIE_PORT_BROKEN_LANE_RECOVERY  0x00000002ull
#define INIT_STATUS_PCIE_PORT_TRAINING_FAIL         0x00000004ull
#define INIT_STATUS_PCIE_TRAINING_SUCCESS           0x00000008ull
#define INIT_STATUS_PCIE_EP_NOT_PRESENT             0x00000010ull
#define INIT_STATUS_PCIE_PORT_IN_COMPLIANCE         0x00000020ull
#define INIT_STATUS_DDI_ACTIVE                      0x00000040ull
#define INIT_STATUS_ALLOCATED                       0x00000080ull
#define INIT_STATUS_PCIE_PORT_BROKEN_LANE_X1        0x00000100ull

// Get lowest PHY lane on engine
#define PcieLibGetLoPhyLane(Engine)  (Engine != NULL ? ((Engine->EngineData.StartLane > Engine->EngineData.EndLane) ? Engine->EngineData.EndLane : Engine->EngineData.StartLane) : 0)
// Get highest PHY lane on engine
#define PcieLibGetHiPhyLane(Engine)  (Engine != NULL ? ((Engine->EngineData.StartLane > Engine->EngineData.EndLane) ? Engine->EngineData.StartLane : Engine->EngineData.EndLane) : 0)
// Get number of lanes on wrapper
#define PcieLibWrapperNumberOfLanes(Wrapper)  (Wrapper != NULL ? ((UINT8)(Wrapper->EndPhyLane - Wrapper->StartPhyLane + 1)) : 0)
// Check if virtual descriptor
#define PcieLibIsEngineAllocated(Descriptor)  (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0) : FALSE)
// Check if it is last descriptor in list
#define PcieLibIsLastDescriptor(Descriptor)  (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_TERMINATE_LIST) != 0) : TRUE)
// Check if descriptor a PCIe engine
#define PcieLibIsPcieEngine(Descriptor)  (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_PCIE_ENGINE) != 0) : FALSE)
// Check if descriptor a DDI engine
#define PcieLibIsPcieWrapper(Descriptor)  (Descriptor != NULL ? ((Descriptor->Header.DescriptorFlags & DESCRIPTOR_PCIE_WRAPPER) != 0) : FALSE)
// Check if descriptor a PCIe wrapper
#define PcieLibGetNextDescriptor(Descriptor)  (Descriptor != NULL ? (((Descriptor->Header.DescriptorFlags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : ((Descriptor) + 1)) : NULL)

/// PCIe port State
typedef enum {
  UnhidePorts,                                          ///< Command to unhide port
  HidePorts,                                            ///< Command to hide unused ports
} PCIE_PORT_VISIBILITY;

/// Table Register Entry
typedef struct {
  UINT16    Reg;                                        ///< Address
  UINT32    Mask;                                       ///< Mask
  UINT32    Data;                                       ///< Data
} PCIE_PORT_REGISTER_ENTRY;

/// Table Register Entry
typedef struct {
  PCIE_PORT_REGISTER_ENTRY    *Table;                   ///< Table
  UINT32                      Length;                   ///< Length
} PCIE_PORT_REGISTER_TABLE_HEADER;

/// Table Register Entry
typedef struct {
  UINT32    Reg;                                        ///< Address
  UINT32    Mask;                                       ///< Mask
  UINT32    Data;                                       ///< Data
} PCIE_HOST_REGISTER_ENTRY;

/// Table Register Entry
typedef struct {
  PCIE_HOST_REGISTER_ENTRY    *Table;                   ///< Table
  UINT32                      Length;                   ///< Length
} PCIE_HOST_REGISTER_TABLE_HEADER;

/// Link ASPM info
typedef struct {
  PCI_ADDR          DownstreamPort;                     ///< PCI address of downstream port
  PCIE_ASPM_TYPE    DownstreamAspm;                     ///< Downstream Device Aspm
  PCI_ADDR          UpstreamPort;                       ///< PCI address of upstream port
  PCIE_ASPM_TYPE    UpstreamAspm;                       ///< Upstream Device Capability
  PCIE_ASPM_TYPE    RequestedAspm;                      ///< Requested ASPM
  BOOLEAN           BlackList;                          ///< Blacklist device
} PCIe_LINK_ASPM;

/// PCIe ASPM Latency Information
typedef struct {
  UINT8    MaxL0sExitLatency;                           ///< Max L0s exit latency in us
  UINT8    MaxL1ExitLatency;                            ///< Max L1 exit latency in us
} PCIe_ASPM_LATENCY_INFO;

/// PCI address association
typedef struct {
  UINT8    NewDeviceAddress;                             ///< New PCI address (Device,Fucntion)
  UINT8    NativeDeviceAddress;                          ///< Native PCI address (Device,Fucntion)
} PCI_ADDR_LIST;

typedef UINT16 PCIe_ENGINE_INIT_STATUS;

/// PCIe port configuration info
typedef struct {
  PCIe_PORT_DATA    PortData;                           ///< Port data
  UINT8             StartCoreLane;                      ///< Start Core Lane
  UINT8             EndCoreLane;                        ///< End Core lane
  UINT8             NativeDevNumber        : 5;         ///< Native PCI device number of the port
  UINT8             NativeFunNumber        : 3;         ///< Native PCI function number of the port
  UINT8             CoreId                 : 3;         ///< PCIe core ID
  UINT8             PortId                 : 5;         ///< MAC Port ID on wrapper
  PCI_ADDR          Address;                            ///< PCI address of the port
  UINT8             PcieBridgeId;                       ///< IOC PCIe bridge ID
  UINT8             GfxWrkRetryCount;                   ///< Number of retry for GFX workaround
  UINT8             LogicalBridgeId;                    ///< Logical Bridge ID
  UINT8             PowerOffUnusedLanesPLL : 1;         ///< Power Off unused lanes and PLL
  UINT8             PowerOffPhyLanesPllL1  : 1;         ///< Power Off Phy Lanes and Pll in L1
  UINT8             DynamicLanesPowerState : 6;         ///< Dynamic Lanes Power State for Linkwidth Switch
  UINT8             SlotPowerLimit;                     ///< Slot Power Limit
  UINT8             MaxPayloadSize;                     ///< Max_Payload_Size
  UINT8             L1ImmediateACK         : 1;         ///< L1 Immediate ACK
  UINT8             TXDeEmphasis           : 4;         ///< TX De-emphasis
  UINT8             TXMargin               : 3;         ///< TX Margin
  UINT8             ScramblerCntl          : 1;         ///< Scrambler control
  UINT8             EqSearchMode           : 2;         ///< Equalization Search Mode
  UINT8             DisGen3EQPhase         : 1;         ///< Disable Gen3 EQ Phase2/3
  UINT8             LinkAspmL1_1           : 1;         ///< Enable PM L1 SS L1.1
  UINT8             LinkAspmL1_2           : 1;         ///< Enable PM L1 SS L1.2
  UINT8             Reserved0              : 2;         ///< Reserved:[6:7]
  UINT8             ClkReq                 : 4;         ///< ClkReq:[0:3]
  UINT8             EqPreset               : 4;         ///< EqPreset:[4:7]
  struct {
    UINT8    NonPostedHeader;                           ///< Non-Posted Header
    UINT8    NonPostedData;                             ///< Non-Posted Data
    UINT8    PostedHeader;                              ///< Posted Header
    UINT8    PostedData;                                ///< Posted Data
    UINT8    CompletionHeader;                          ///< Completion Header
    UINT8    CompletionData;                            ///< Completion Data
  } FlowCntlCredits;
  struct {
    UINT8    SpcGen1 : 1;                               ///< SPC Mode 2P5GT
    UINT8    SpcGen2 : 1;                               ///< SPC Mode 5GT
    UINT8    SpcGen3 : 1;                               ///< SPC Mode 8GT
  } SpcMode;
  struct {
    UINT16    DsTxPreset     : 4;                       ///< Downstream Tx Preset
    UINT16    DsRxPresetHint : 3;                       ///< Downstream Rx Preset Hint
    UINT16    UsTxPreset     : 4;                       ///< Upstream Tx Preset
    UINT16    UsRxPresetHint : 3;                       ///< Upstream Rx Preset Hint
  } LaneEqualizationCntl;
  UINT8    LcFomTime;                                   ///< Figure of Merit time
  UINT8    PortReversal         : 1;                    ///< PortReversal
  UINT8    BypassGen3EQ         : 1;                    ///< BypassGen3EQ
  UINT8    Enable3x3Search      : 1;                    ///< Enable3x3Search
  UINT8    EnableSafeSearch     : 1;                    ///< EnableSafeSearch
  UINT8    SRIS_SRNS            : 1;                    ///< SRIS SRNS
  UINT8    SRIS_En              : 1;                    ///< SRIS Enable
  UINT8    SRIS_AutoDetectEn    : 1;                    ///< SRIS Auto Detect Enable
  UINT8    SRIS_LowerSKPSupport : 1;                    ///< SRIS Lower SKP Support
} PCIe_PORT_CONFIG;

/// Descriptor header
typedef struct {
  UINT32    DescriptorFlags;                            ///< Descriptor flags
  UINT16    Parent;                                     ///< Offset of parent descriptor
  UINT16    Peer;                                       ///< Offset of the peer descriptor
  UINT16    Child;                                      ///< Offset of the list of child descriptors
} PCIe_DESCRIPTOR_HEADER;

/// DDI (Digital Display Interface) configuration info
typedef struct {
  PCIe_DDI_DATA    DdiData;                             ///< DDI Data
  UINT8            DisplayPriorityIndex;                ///< Display priority index
  UINT8            ConnectorId;                         ///< Connector id determined by enumeration
  UINT8            DisplayDeviceId;                     ///< Display device id determined by enumeration
} PCIe_DDI_CONFIG;

/// Engine configuration data
typedef struct {
  PCIe_DESCRIPTOR_HEADER     Header;                    ///< Descriptor header
  PCIe_ENGINE_DATA           EngineData;                ///< Engine Data
  PCIe_ENGINE_INIT_STATUS    InitStatus;                ///< Initialization Status
  UINT8                      Scratch;                   ///< Scratch pad
  union {
    PCIe_PORT_CONFIG    Port;                           ///< PCIe port configuration data
    PCIe_DDI_CONFIG     Ddi;                            ///< DDI configuration data
  } Type;
} PCIe_ENGINE_CONFIG;

/// Wrapper configuration data
typedef struct {
  PCIe_DESCRIPTOR_HEADER    Header;                     ///< Descriptor Header
  UINT8                     WrapId;                     ///< Wrapper ID
  UINT8                     NumberOfPIFs;               ///< Number of PIFs on wrapper
  UINT8                     StartPhyLane;               ///< Start PHY Lane
  UINT8                     EndPhyLane;                 ///< End PHY Lane
  UINT8                     StartPcieCoreId : 4;        ///< Start PCIe Core ID
  UINT8                     EndPcieCoreId   : 4;        ///< End PCIe Core ID
  UINT8                     NumberOfLanes;              ///< Number of lanes
  struct {
    UINT8    PowerOffUnusedLanes     : 1;               ///< Power Off unused lanes
    UINT8    PowerOffUnusedPlls      : 1;               ///< Power Off unused Plls
    UINT8    ClkGating               : 1;               ///< TXCLK gating
    UINT8    LclkGating              : 1;               ///< LCLK gating
    UINT8    TxclkGatingPllPowerDown : 1;               ///< TXCLK clock gating PLL power down
    UINT8    PllOffInL1              : 1;               ///< PLL off in L1
    UINT8    AccessEncoding          : 1;               ///< Reg access encoding
  } Features;
  UINT8    MasterPll;                                   ///< Bitmap of master PLL
} PCIe_WRAPPER_CONFIG;

/// Silicon configuration data
typedef struct  {
  PCIe_DESCRIPTOR_HEADER    Header;                     ///< Descriptor Header
  UINT8                     SocketId;                   ///< Socket ID
  UINT8                     PhysicalDieId;              ///< Physical Die ID
  UINT8                     LogicalDieId;               ///< Logical Die ID
  UINT8                     GlobalIodId;                ///< Global IOD ID
  UINT8                     DieType;                    ///< Physical Die Type
  UINT8                     RbId;                       ///< NBIO Id
  UINT8                     InstanceId;                 ///< Logical Instance Identifier, 0:Socket0 DJ0 NBIO
  BOOLEAN                   IohubPresent;               ///< IO Hub (IOHC, IOMMU, IOAPIC) present on this NBIO
  PCI_ADDR                  Address;                    ///< PCI address of GNB host bridge
  UINT16                    StartLane;                  ///< Start Lane of this NBIO
  UINT16                    EndLane;                    ///< End Lane of this NBIO
  UINT16                    IodStartLane;               ///< Start Lane of this IOD
  UINT16                    IodEndLane;                 ///< End Lane of this IOD
  UINT16                    SocketStartLane;            ///< Start Lane of this socket
  UINT16                    SocketEndLane;              ///< End Lane of this socket
  BOOLEAN                   CxlPresent;                 ///< CXL root port present on this NBIO
  UINT8                     BusLimit;                   ///< Nbio Bus Limit
  UINT8                     Reserved[2];                ///< For alignment
} PCIe_SILICON_CONFIG;

typedef PCIe_SILICON_CONFIG GNB_HANDLE;

/// Complex configuration data
typedef struct {
  PCIe_DESCRIPTOR_HEADER    Header;                     ///< Descriptor Header
  UINT8                     NodeId;                     ///< Processor Node ID
  UINT8                     Reserved;                   ///< For alignment
} PCIe_COMPLEX_CONFIG;

/// PCIe platform configuration info
typedef struct {
  PCIe_DESCRIPTOR_HEADER    Header;                     ///< Descriptor Header
  PVOID                     StdHeader;                  ///< Standard configuration header
  UINT32                    LinkReceiverDetectionPooling; ///< Receiver pooling detection time in us.
  UINT32                    LinkL0Pooling;                        ///< Pooling for link to get to L0 in us
  UINT32                    LinkGpioResetAssertionTime;           ///< Gpio reset assertion time in us
  UINT32                    LinkResetToTrainingTime;              ///< Time duration between deassert GPIO reset and release training in us                                                      ///
  UINT8                     GfxCardWorkaround;                    ///< GFX Card Workaround
  UINT8                     PsppPolicy;                           ///< PSPP policy
  UINT8                     TrainingExitState;                    ///< State at which training should exit (see PCIE_LINK_TRAINING_STATE)
  UINT8                     TrainingAlgorithm;                    ///< Training algorithm (see PCIE_TRAINING_ALGORITHM)
  PCIe_COMPLEX_CONFIG       ComplexList[MAX_NUMBER_OF_COMPLEXES_HYGX]; ///< Complex
} PCIe_PLATFORM_CONFIG;

/// PCIe Engine Description

typedef struct {
  UINT32              Flags;                            /**< Descriptor flags
                                                         * @li @b Bit31 - last descriptor on wrapper
                                                         * @li @b Bit30 - Descriptor allocated for PCIe port or DDI
                                                         */
  PCIe_ENGINE_DATA    EngineData;                       ///< Engine Data
} PCIe_ENGINE_DESCRIPTOR;

#pragma pack (pop)

#endif
