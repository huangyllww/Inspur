/* $NoKeywords:$ */

/**
 * @file
 *
 * Platform PCIe Complex Definitions
 *
 * Contains HYGON HGPI core interface
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Include
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
 *
 ***************************************************************************/

#ifndef _HYGON_PCIE_COMPLEX_H_
#define _HYGON_PCIE_COMPLEX_H_

//
//
// HGPI Types and Definitions
//
//

/*----------------------------------------------------------------------------
 *   GNB PCIe configuration info
 *----------------------------------------------------------------------------
 */

#define DESCRIPTOR_TERMINATE_LIST  0x80000000ull
#define DESCRIPTOR_IGNORE          0x40000000ull

/// PCIe link initialization
typedef enum {
  HsioEndpointDetect = 0,                                    ///< Detect endpoint presence
  HsioEndpointNotPresent                                     ///< Endpoint not present (or connected). Used in case there is alternative way to determine
                                                             ///< if device present on board or in slot. For example GPIO can be used to determine device presence.
} HSIO_ENDPOINT_STATUS;

// --------------------------------------------------------------------------------------------------
// PHY Programming Data

typedef struct {
  IN       UINT16    ParamType;
  IN       UINT8     ParamValue;
} HSIO_PHY_PARAM;

typedef struct {
  IN       HSIO_PHY_PARAM    PhyParam[44];
} PHY_DATA;

#define PHY_PARAMS_START  , { {
#define PHY_PARAMS_END    } }
#define PHY_PARAM(x, y)  { x, y }

typedef enum {
  rxX_los_threshold = 1,
  rx_vref_ctrl,
  SSC_OFF_FRUG1,
  SSC_OFF_PHUG1,

  GEN1_txX_eq_pre = (1 << 6),
  GEN1_txX_eq_main,
  GEN1_txX_eq_post,
  GEN1_txX_iboost_lvl,
  GEN1_txX_vboost_en,
  GEN1_Unsupported1,
  GEN1_Unsupported2,
  GEN1_rxX_eq_att_lvl,
  GEN1_rxX_eq_vga1_gain,
  GEN1_rxX_eq_vga2_gain,
  GEN1_rxX_eq_ctle_pole,
  GEN1_rxX_eq_ctle_boost,
  GEN1_rxX_eq_dfe_tap1,
  GEN1_tx_vboost_lvl,
  GEN1_Unsupported3,

  GEN2_txX_eq_pre = (2 << 6),
  GEN2_txX_eq_main,
  GEN2_txX_eq_post,
  GEN2_txX_iboost_lvl,
  GEN2_txX_vboost_en,
  GEN2_Unsupported1,
  GEN2_Unsupported2,
  GEN2_rxX_eq_att_lvl,
  GEN2_rxX_eq_vga1_gain,
  GEN2_rxX_eq_vga2_gain,
  GEN2_rxX_eq_ctle_pole,
  GEN2_rxX_eq_ctle_boost,
  GEN2_rxX_eq_dfe_tap1,
  GEN2_tx_vboost_lvl,
  GEN2_Unsupported3,

  GEN3_txX_eq_pre = (3 << 6),
  GEN3_txX_eq_main,
  GEN3_txX_eq_post,
  GEN3_txX_iboost_lvl,
  GEN3_txX_vboost_en,
  GEN3_rxX_adapt_afe_en,
  GEN3_rxX_adapt_dfe_en,
  GEN3_rxX_eq_att_lvl,
  GEN3_rxX_eq_vga1_gain,
  GEN3_rxX_eq_vga2_gain,
  GEN3_rxX_eq_ctle_pole,
  GEN3_rxX_eq_ctle_boost,
  GEN3_rxX_eq_dfe_tap1,
  GEN3_tx_vboost_lvl,
  GEN3_OtherSide_txX_Preset,

  GEN4_txX_eq_pre = (4 << 6),
  GEN4_txX_eq_main,
  GEN4_txX_eq_post,
  GEN4_txX_iboost_lvl,
  GEN4_txX_vboost_en,
  GEN4_rxX_adapt_afe_en,
  GEN4_rxX_adapt_dfe_en,
  GEN4_rxX_eq_att_lvl,
  GEN4_rxX_eq_vga1_gain,
  GEN4_rxX_eq_vga2_gain,
  GEN4_rxX_eq_ctle_pole,
  GEN4_rxX_eq_ctle_boost,
  GEN4_rxX_eq_dfe_tap1,
  GEN4_tx_vboost_lvl,
  GEN4_OtherSide_txX_Preset
} HSIO_PHY_PARAM_TYPE;

// --------------------------------------------------------------------------------------------------
// PCIe specific data structures

/// PCIe port misc extended controls
typedef struct  {
  IN      UINT8                     LinkComplianceMode     : 1;  ///< Force port into compliance mode (device will not be trained, port output compliance pattern)
  IN      UINT8                     LinkSafeMode           : 2;  /**< Safe mode PCIe capability. (Parameter may limit PCIe speed requested through HSIO_PORT_DATA::LinkSpeedCapability)
                                                                 *  @li @b 0 - port can advertize muximum supported capability
                                                                 *  @li @b 1 - port limit advertized capability and speed to PCIe Gen1
                                                                 */
  IN      UINT8                     SbLink                 : 1;  /**< PCIe link type
                                                                 *  @li @b 0 - General purpose port
                                                                 *  @li @b 1 - Port connected to SB
                                                                 */
  IN      UINT8                     ClkPmSupport           : 1;  /**< Clock Power Management Support
                                                                 *  @li @b 0 - Clock Power Management not configured
                                                                 *  @li @b 1 - Clock Power Management configured according to PCIe device capability
                                                                 */
  IN      UINT8                     ChannelType            : 2;  /**< Channel Type
                                                                 *  @li @b 0 - Channel Type Not Specified
                                                                 *  @li @b 1 - Channel Type Short Trace
                                                                 *  @li @b 2 - Channel Type Long Trace
                                                                 */
  IN      UINT8                     TurnOffUnusedLanes     : 1;  /**< Turn Off Unused Lanes
                                                                 *  @li @b 0 -
                                                                 *  @li @b 1 -
                                                                 */
} HSIO_PORT_MISC_CONTROL;

/// The IO APIC Interrupt Mapping Info
typedef struct {
  IN      UINT8                     GroupMap;               /**< Group mapping for slot or endpoint device (connected to PCIE port) interrupts .
                                                             *  @li <b>0</b> - IGNORE THIS STRUCTURE AND USE RECOMMENDED SETTINGS
                                                             *  @li <b>1</b> - mapped to Grp 0 (Interrupts 0..3   of IO APIC redirection table)
                                                             *  @li <b>2</b> - mapped to Grp 1 (Interrupts 4..7   of IO APIC redirection table)
                                                             *  @li ...
                                                             *  @li <b>8</b> - mapped to Grp 7 (Interrupts 28..31 of IO APIC redirection table)
                                                             */
  IN      UINT8                     Swizzle;                /**< Swizzle interrupt in the Group.
                                                             *  @li <b>0</b> - ABCD
                                                             *  @li <b>1</b> - BCDA
                                                             *  @li <b>2</b> - CDAB
                                                             *  @li <b>3</b> - DABC
                                                             */
  IN      UINT8                     BridgeInt;              /**< IOAPIC redirection table entry for PCIE bridge interrupt
                                                             *  @li <b>0</b>  - Entry 0  of IO APIC redirection table
                                                             *  @li <b>1</b>  - Entry 1  of IO APIC redirection table
                                                             *  @li ...
                                                             *  @li <b>31</b> - Entry 31 of IO APIC redirection table
                                                             */
} HSIO_APIC_DEVICE_INFO;

/// PCIe port configuration data
typedef struct  {
  IN       UINT16                  PortPresent         : 1;  ///< Enable PCIe port for initialization.
  IN       UINT16                  CxlMode             : 2;  ///< 0: CXL 1.1,  1: Cxl2.0, 3:PCIe  
  IN       UINT16                  Reserved1           : 4;  /// Unused bits
  IN       UINT16                  DeviceNumber        : 5;  /**< PCI Device number for port.
                                                               *   @li @b 0 - Native port device number
                                                               *   @li @b N - Port device number (See available configurations in BKDG
                                                               */
  IN       UINT16                  FunctionNumber      : 3;  ///< Reserved for future use
  IN       UINT16                  LinkSpeedCapability : 3;  /**< PCIe link speed/
                                                               *  @li @b 0 - Maximum supported by silicon
                                                               *  @li @b 1 - Gen1
                                                               *  @li @b 2 - Gen2
                                                               *  @li @b 3 - Gen3
                                                               */
  IN       UINT16                  AutoSpdChng         : 2;  /**< Upstread Auto Speed Change Allowed/
                                                               *  @li @b 0 - Use default implementation (Disabled for Gen1, Enabled for Gen2/3)
                                                               *  @li @b 1 - Always Disabled
                                                               *  @li @b 2 - Always Enabled
                                                               *  @li @b 3 - Reserved
                                                               */
  IN       UINT16                  EqPreset            : 4;  /**< Gen3 Equalization Preset */
  IN       UINT8                   LinkAspm            : 2;  /**< ASPM control. (see HgpiPcieLinkAspm for additional option to control ASPM)
                                                               *  @li @b 0 - Disabled
                                                               *  @li @b 1 - L0s only
                                                               *  @li @b 2 - L1 only
                                                               *  @li @b 3 - L0s and L1
                                                               */
  IN       UINT8                   LinkAspmL1_1        : 1;  /**< ASPM control. (see HgpiPcieLinkAspm for additional option to control ASPM)
                                                               *  @li @b 0 - Disabled
                                                               *  @li @b 1 - Enabled
                                                               */
  IN       UINT8                   LinkAspmL1_2        : 1;  /**< ASPM control. (see HgpiPcieLinkAspm for additional option to control ASPM)
                                                               *  @li @b 0 - Disabled
                                                               *  @li @b 1 - Enabled
                                                               */
  IN       UINT8                   ClkReq              : 4;  /**< ASPM Reserved Field
                                                               *  @li @b 0 - NONE
                                                               *  @li @b 1 - CLKREQ0 signal
                                                               *  @li @b 2 - CLKREQ1 signal
                                                               *  @li @b 3 - CLKREQ2 signal
                                                               *  @li @b 4 - CLKREQ3 signal
                                                               *  @li @b 5 - CLKREQG signal
                                                               */
  IN       UINT8                   LinkHotplug;              /**< Hotplug control.
                                                               *  @li @b 0 - Disabled
                                                               *  @li @b 1 - Basic
                                                               *  @li @b 2 - Server
                                                               *  @li @b 3 - Enhanced
                                                               */
  IN       UINT16                  SlotNum;                  ///< PHYSICAL_SLOT_NUM
  IN       HSIO_PORT_MISC_CONTROL  MiscControls;             ///< Misc extended controls
  IN       HSIO_APIC_DEVICE_INFO   ApicDeviceInfo;           ///< IOAPIC device programming info
  IN       HSIO_ENDPOINT_STATUS    EndpointStatus;           ///< PCIe endpoint (device connected to PCIe port) status
} HSIO_PORT_DATA;

// --------------------------------------------------------------------------------------------------
// EtherNet specific data structures

typedef struct  {
  IN       UINT32               PortNum           : 8;     ///< Port Number
  IN       UINT32               PlatConf          : 4;     /**< Platform Config
                                                            *  0 = Reserved
                                                            *  1 = 10G/1G BackPlane
                                                            *  2 = 2.5G BackPlane
                                                            *  3= Soldered down 1000Base-T
                                                            *  4 = Soldered down 1000Base-X
                                                            *  5 = Soldered down NBase-T
                                                            *  6 = Soldered down 10GBase-T
                                                            *  7 = Soldered down 10GBase-r
                                                            *  8 = SFP+ Connector
                                                            */
  IN       UINT32               Reserved1         : 4;     ///< Unused 12-15
  IN       UINT32               MdioId            : 5;     ///< MDIO ID when MDIO Side band is used
  IN       UINT32               Reserved2         : 2;     ///< Unused 21-22
  IN       UINT32               SuppSpeed         : 4;     /**< Supported Speeds by Platform
                                                            *  1 = 100M Supported
                                                            *  2 = 1G Supported
                                                            *  4 = 2.5G Supported
                                                            *  8 = 10G Supported
                                                            */
  IN       UINT32               Reserved3         : 1;     ///< Unused 27
  IN       UINT32               ConnType          : 3;     /**< Supported Speeds by Platform
                                                            *  0 = Port not Used
                                                            *  1 = SFP+ Connection I2C interface
                                                            *  2 = MDIO PHY
                                                            *  4 = Backplane Connection
                                                            */
  IN       UINT32               Reserved4         : 1;     ///< Unused 31
} ETH_PORT_PROPERTY0;

typedef struct  {
  IN       UINT32               MdioReset         : 2;     /**< MDIO Reset Type
                                                            *  0 = None
                                                            *  1 = I2C GPIO
                                                            *  2 = Integrated GPIO
                                                            *  3 = Reserved
                                                            */
  IN       UINT32               Reserved1         : 2;     ///< Unused 2-3
  IN       UINT32               MdioGpioResetNum  : 2;     ///< Integrated GPIO number for reset
  IN       UINT32               Reserved2         : 2;     ///< Unused 6-7
  IN       UINT32               SfpGpioAdd        : 3;     ///< Lower I2C address of GPIO Expander PCA9535
  IN       UINT32               Reserved3         : 1;     ///< Unused 11
  IN       UINT32               TxFault           : 4;     ///< TX FAULT
  IN       UINT32               Rs                : 4;     ///< RS Signal
  IN       UINT32               ModAbs            : 4;     ///< MOD_ABS signal
  IN       UINT32               RxLoss            : 4;     ///< Rx_LOS signal
  IN       UINT32               SfpGpioMask       : 4;     ///< SFP+ sideband signals that are not supported by platform
} ETH_PORT_PROPERTY3;

typedef struct  {
  IN       UINT32               SfpMux            : 3;     ///< Lower address of Mux PCA 9545
  IN       UINT32               Reserved1         : 1;     ///< Unused 3
  IN       UINT32               SfpBusSeg         : 3;     ///< SFP BUS Segment. Downstream channels of PCA9545
  IN       UINT32               Reserved2         : 1;     ///< Unused 7
  IN       UINT32               SfpMuxUpAdd       : 5;     ///< Upper address of Mux PCA 9545
  IN       UINT32               Reserved3         : 3;     ///< Unused 13-15
  IN       UINT32               RedriverAddress   : 7;     ///< Address of ReDriver
  IN       UINT32               RedriverInterface : 1;     ///< ReDriver Interface Descriptor
  IN       UINT32               RedriverLane      : 3;     ///< ReDriver Lane number
  IN       UINT32               Reserved4         : 1;     ///< Unused 27
  IN       UINT32               RedriverModel     : 3;     ///< ReDriver Model
  IN       UINT32               RedriverPresent   : 1;     ///< Redriver Present
} ETH_PORT_PROPERTY4;

typedef struct  {
  IN       UINT32               SlotMux            : 3;     ///< Lower address of Mux PCA 9545
  IN       UINT32               SlotBusSeg         : 3;     ///< Slot BUS Segment. Downstream channels of PCA9545
  IN       UINT32               SlotMuxUpAdd       : 5;     ///< Upper address of Mux PCA 9545
  IN       UINT32               InfoRomAdd         : 3;     ///< Lower I2C address of XGBE info ROM At24c08d
  IN       UINT32               InfoRomAddUP       : 5;     ///< Upper I2C address of XGBE info ROM At24c08d
  IN       UINT32               InfoRomLocation    : 1;     ///< XGBE info ROM At24c08d location,0=on board,1= field card
  IN       UINT32               Reserved           : 12;    ///< Unused
} ETH_PORT_PROPERTY5;

typedef struct  {
  IN       UINT32               TxEqPre           : 6;     ///< TX EQ PRE
  IN       UINT32               Reserved1         : 2;     ///< Unused 7-6
  IN       UINT32               TxEqMain          : 6;     ///< TX EQ MAIN
  IN       UINT32               Reserved2         : 2;     ///< Unused 15-14
  IN       UINT32               TxEqPost          : 6;     ///< TX EQ POST
  IN       UINT32               Reserved3         : 10;    ///< Unused 31-23
} ETH_PORT_TXEQ;

/// Ethernet PCIe port configuration data
typedef struct  {
  IN       ETH_PORT_PROPERTY0   EthPortProp0;             ///< XGBE_PORT_PROPERTY_0
  IN       ETH_PORT_PROPERTY3   EthPortProp3;             ///< XGBE_PORT_PROPERTY_3
  IN       ETH_PORT_PROPERTY4   EthPortProp4;             ///< XGBE_PORT_PROPERTY_4
  IN       ETH_PORT_PROPERTY5   EthPortProp5;             ///< XGBE_PORT_PROPERTY_5
  IN       UINT32               PadMux0;                  ///< PadMux0 Setting (8 bits)
  IN       UINT32               PadMux1;                  ///< PadMux1 Setting (8 bits)
  IN       UINT32               MacAddressLo;             ///< Lower 32 bits of MAC Address
  IN       UINT32               MacAddressHi;             ///< Upper 32 bits of MAC Address
  IN       ETH_PORT_TXEQ        EthPortTxEq;              ///< TX EQ Settings
} ETHERNET_PORT_DATA;

// --------------------------------------------------------------------------------------------------
// High level data structures for passing topology from platform to HGPI

/// Engine Configuration
typedef struct {
  IN       UINT8                EngineType;               /**< Engine type
                                                           *  @li @b 0 -  Ignore engine configuration
                                                           *  @li @b 1 -  PCIe port
                                                           *  @li @b 2 -  DDI
                                                           */
  IN       UINT8                HotPluggable  : 1;        /**<HotPluggable
                                                           *  @li @b 0 - Link is NOT Hot-Switchable
                                                           *  @li @b 1 - Link IS Hot-Switchable
                                                           */
  IN       UINT8                Reserved1     : 7;        /**< Unused field, leave as 0
                                                           */
  IN       UINT8                StartLane;                /**< Start Lane ID (in reversed configuration StartLane > EndLane)
                                                           * Refer to lane descriptions and supported configurations in BKDG
                                                           */
  IN       UINT8                EndLane;                  /**< End lane ID (in reversed configuration StartLane > EndLane)
                                                           * Refer to lane descriptions and supported configurations in BKDG
                                                           */
  IN       UINT8                GpioGroupId;              /**< Unique identifier for the GPIO or GPIO group associated with
                                                           * this engine.  GPIOs are used for hotplug notification and link
                                                           * type (e.g SATA Express or PCIe)
                                                           */
  IN       UINT8                Reserved2;                /**< Unused field, leave as 0
                                                           */
  IN       UINT8                Reserved3;                /**< Unused field, leave as 0
                                                           */
  IN       UINT8                Reserved4;                /**< Unused field, leave as 0
                                                           */
} HSIO_ENGINE_DATA;

/// DDI channel lane mapping
typedef struct {
  ///< Structure that discribe lane mapping
  IN      UINT8              Lane0   : 2;                  /**< Lane 0 mapping
                                                           *  @li @b 0 - Map to lane 0
                                                           *  @li @b 1 - Map to lane 1
                                                           *  @li @b 2 - Map to lane 2
                                                           *  @li @b 2 - Map to lane 3
                                                           */
  IN      UINT8              Lane1   : 2;                  ///< Lane 1 mapping (see "Lane 0 mapping")
  IN      UINT8              Lane2   : 2;                  ///< Lane 2 mapping (see "Lane 0 mapping")
  IN      UINT8              Lane3   : 2;                  ///< Lane 3 mapping (see "Lane 0 mapping")
} HSIO_CHANNEL_MAPPING;

/// Common Channel Mapping
typedef union {
  IN      UINT8                ChannelMappingValue;       ///< Raw lane mapping
  IN      HSIO_CHANNEL_MAPPING      ChannelMapping;       ///< Channel mapping
} HSIO_CONN_CHANNEL_MAPPING;

/// DDI Configuration data
typedef struct  {
  IN       UINT8                ConnectorType;            /**< Display Connector Type
                                                            *  @li @b 0 - DP
                                                            *  @li @b 1 - eDP
                                                            *  @li @b 2 - Single Link DVI-D
                                                            *  @li @b 3 - Dual  Link DVI-D (see @ref DualLinkDviDescription "Example Dual Link DVI connector description")
                                                            *  @li @b 4 - HDMI
                                                            *  @li @b 5 - DP-to-VGA
                                                            *  @li @b 6 - DP-to-LVDS
                                                            *  @li @b 7 - Hudson-2 NutMeg DP-to-VGA
                                                            *  @li @b 8 - Single Link DVI-I
                                                            *  @li @b 9 - Native CRT (Family 0x14)
                                                            *  @li @b 10 - Native LVDS (Family 0x14)
                                                            *  @li @b 11 - Auto detect LCD panel connector type. VBIOS is able to auto detect the LVDS connector type: native LVDS, eDP or DP-to-LVDS
                                                            *              The auto detection method only support panel with EDID.
                                                            */
  IN       UINT8                AuxIndex;                 /**< Indicates which AUX or DDC Line is used
                                                            *  @li @b 0 - AUX1
                                                            *  @li @b 1 - AUX2
                                                            *  @li @b 2 - AUX3
                                                            *  @li @b 3 - AUX4
                                                            *  @li @b 4 - AUX5
                                                            *  @li @b 5 - AUX6
                                                            */
  IN       UINT8                HdpIndex;                 /**< Indicates which HDP pin is used
                                                            *  @li @b 0 - HDP1
                                                            *  @li @b 1 - HDP2
                                                            *  @li @b 2 - HDP3
                                                            *  @li @b 3 - HDP4
                                                            *  @li @b 4 - HDP5
                                                            *  @li @b 5 - HDP6
                                                            */
  IN       HSIO_CONN_CHANNEL_MAPPING Mapping[2];          /**< Set specific mapping of lanes to connector pins
                                                            *  @li Mapping[0] define mapping for group of 4 lanes starting at PCIe_ENGINE_DATA.StartLane
                                                            *  @li Mapping[1] define mapping for group of 4 lanes ending at PCIe_ENGINE_DATA.EndLane (only applicable for Dual DDI link)
                                                            *  if Mapping[x] set to 0 than default mapping assumed
                                                            */
  IN       UINT8                LanePnInversionMask;      /**< Specifies whether to invert the state of P and N for each lane. Each bit represents a PCIe lane on the DDI port.
                                                            *  @li 0 - Do not invert (default)
                                                            *  @li 1 - Invert P and N on this lane
                                                            */
  IN       UINT8                Flags;                    /**< Capabilities flags
                                                            *  @li Flags bit[0] DDI_DATA_FLAGS_DP1_1_ONLY Selects downgrade PHY link to DP1.1
                                                            *  @li Flags bit[7:1] Reserved
                                                            */
} DDI_DATA;

/// PCIe port descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Bit31 - last descriptor in complex
                                                           */
  IN       HSIO_ENGINE_DATA     EngineData;               ///< Engine data
  IN       HSIO_PORT_DATA       Port;                     ///< PCIe port specific configuration info
  IN       ETHERNET_PORT_DATA   EtherNet;                 ///< Ancillary data for EtherNet
  IN       PHY_DATA             Phy;                      ///< Ancillary data for PHY programming customization
} HSIO_PORT_DESCRIPTOR;

/// DDI descriptor
typedef struct {
  IN       UINT32          Flags;                    /**< Descriptor flags
                                                           * @li @b Bit31 - last descriptor in complex
                                                           */
  IN       DDI_DATA        Ddi;                      ///< DDI port specific configuration info
} DDI_DESCRIPTOR;

/// PCIe Complex descriptor
typedef struct {
  IN       UINT32               Flags;                    /**< Descriptor flags
                                                           * @li @b Bit31 - last descriptor in topology
                                                           */
  IN       UINT32               SocketId;                 ///< Socket Id
  IN       HSIO_PORT_DESCRIPTOR *PciePortList;            ///< Pointer to array of PCIe port descriptors or NULL (Last element of array must be terminated with DESCRIPTOR_TERMINATE_LIST).
  IN       DDI_DESCRIPTOR       *DdiLinkList;             ///< Reserved for compatibility with FM15
  IN       VOID                 *Reserved2;               ///< Reserved for future use
  IN       UINT32               BmcLinkLocation;          ///< Identifies the socket/die location of a BMC link (Used by HGPI, input not required)
} HSIO_COMPLEX_DESCRIPTOR;

/// Engine descriptor type
typedef enum {
  HsioUnusedEngine   = 0,                                 ///< Unused descriptor
  HsioPcieEngine     = 1,                                 ///< PCIe port
  HsioUSBEngine      = 2,                                 ///< USB port
  HsioSATAEngine     = 3,                                 ///< SATA
  HsioCxlEngine      = 4,                                 ///< CXL
  HsioDPEngine       = 8,                                 ///< Digital Display
  HsioEthernetEngine = 0x10,                              ///< Ethernet (GBe, XGBe)
  HsioGOPEngine      = 0x20,                              ///< GOP ///@todo
  HsioNTBDevice      = 0x60,                              ///< For NBIF NTB Enable - DO NOT USE FOR HSIO
  HsioHDaudioEngine,                                      ///< For NBIF HDaudtio Enable - DO NOT USE FOR HSIO
  HsioMaxPcieEngine                                       ///< Max engine type for boundary check.
} HSIO_ENGINE_TYPE;

/// PCIe link capability/speed
typedef enum  {
  HsioGenMaxSupported,                                    ///< Maximum supported
  HsioGen1 = 1,                                           ///< Gen1
  HsioGen2,                                               ///< Gen2
  HsioGen3,                                               ///< Gen3
  HsioGen4,                                               ///< Gen4
  HsioGen5,                                               ///< Gen5
  MaxHsioGen                                              ///< Max Gen for boundary check
} HSIO_LINK_SPEED_CAP;

/// CXL Mode
typedef enum  {
  CXL_1_1,                                                 ///< CXL 1.1
  CXL_2_0,                                                 ///< CXL 2.0
  CXL_PCIE                                                 ///< CXL PCIe
} HSIO_CXL_MODE;

/// PCIe link ASPM
typedef enum {
  HsioAspmDisabled,                                           ///< Disabled
  HsioAspmL0s,                                                ///< PCIe L0s link state
  HsioAspmL1,                                                 ///< PCIe L1 link state
  HsioAspmL0sL1,                                              ///< PCIe L0s & L1 link state
  HsioMaxAspm                                                 ///< Not valid value, used to verify input
} HSIO_ASPM_TYPE;

/// PCIe link hotplug support
typedef enum {
  HsioHotplugDisabled,                                        ///< Hotplug disable
  HsioHotplugBasic,                                           ///< Basic Hotplug
  HsioHotplugServerExpress,                                   ///< Server Hotplug Express Module
  HsioHotplugEnhanced,                                        ///< Enhanced
  HsioHotplugInboard,                                         ///< Inboard
  HsioHotplugServerEntSSD,                                    ///< Server Hotplug Enterprise SSD
  MaxHsioHotplug                                              ///< Not valid value, used to verify input
} HSIO_LINK_HOTPLUG_TYPE;

/// PCIe link initialization
typedef enum {
  HsioPortDisabled,                                           ///< Disable
  HsioPortEnabled                                             ///< Enable
} HSIO_PORT_ENABLE;

/// PCIe ClkPmSupport initialization
typedef enum {
  HsioClkPmSupportDisabled,                                   ///< Disable
  HsioClkPmSupportEnabled                                     ///< Enable
} HSIO_CLKPM_SUPPORT_ENABLE;

/// SATA ChannelType initialization
typedef enum {
  HsioSataChannelOther,                                       ///< Default Channel Type
  HsioSataChannelShort,                                       ///< Short Trace Channel Type
  HsioSataChannelLong                                         ///< Long Trace Channel Type
} HSIO_SATA_CHANNEL_TYPE;

/// CLKREQ signal Type initialization
typedef enum {
  HsioClkReqNone,                                             ///< Default None
  HsioClkReq0,                                                ///< CLKREQ0 signal
  HsioClkReq1,                                                ///< CLKREQ1 signal
  HsioClkReq2,                                                ///< CLKREQ2 signal
  HsioClkReq3,                                                ///< CLKREQ3 signal
  HsioClkReqG,                                                ///< CLKREQG signal
} HSIO_CLKREQ_TYPE;

/// Equalization Preset Search mode options
typedef enum {
  HsioEqPresetSearchDirectional,                              ///< Eq Preset Search Mode - Directional - do not use
  HsioEqPresetSearchExhaustive,                               ///< Eq Preset Search Mode - Exhaustive
  HsioEqPresetSearchUnsupported,                              ///< Eq Preset Search Mode - Unsupported - do not use
  HsioEqPresetSearchPreset,                                   ///< Eq Preset Search Mode - Preset
} HSIO_EQPRESET_TYPE;

/// SOCKET 
typedef enum  {
  CpmSocket0,
  CpmSocket1,
  CpmSocket2,
  CpmSocket3,
} CPM_SOCKET_TYPE;

/// CDD 
typedef enum  {
  CpmCdd0,
  CpmCdd1,
  CpmCdd2,
  CpmCdd3,
  CpmCdd4,
  CpmCdd5,
  CpmCdd6,
  CpmCdd7,
} CPM_CDD_TYPE;

/// UMC Channel
typedef enum  {
  CpmUmcChannel0,
  CpmUmcChannel1,
  CpmUmcChannel2,
} CPM_UMC_TYPE;

/// DIMM 
typedef enum  {
  CpmUmcDimm0,
  CpmUmcDimm1,
} CPM_DIMM_TYPE;

// Macro for statically initializing various structures
#define  HSIO_ENGINE_DATA_INITIALIZER(mType, mStartLane, mEndLane, mHotplug, mGpioGroupId)  { mType, mHotplug, 0, \
                                                                                              mStartLane, mEndLane, \
                                                                                              mGpioGroupId, 0, 0, 0 }

#define  HSIO_PORT_DATA_INITIALIZER_PCIE(mPortPresent, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, \
                                         mAspm, mAspmL1_1, mAspmL1_2, mClkPmSupport, mSlotNum) \
  { mPortPresent, 3, 0, mDevAddress, mDevFunction, mMaxLinkSpeed, 0, 0, mAspm, mAspmL1_1, \
    mAspmL1_2, 0, mHotplug, mSlotNum, { 0, mMaxLinkCap, 0, mClkPmSupport, 0, 1 }, { 0, 0, 0 }, \
    HsioEndpointDetect }, \
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { 0, 0, 0, 0, 0 }, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } }
#define  HSIO_PORT_DATA_INITIALIZER_CXL(mPortPresent, CxlMode, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, \
                                         mAspm, mAspmL1_1, mAspmL1_2, mClkPmSupport, mSlotNum) \
  { mPortPresent, CxlMode, 0, mDevAddress, mDevFunction, mMaxLinkSpeed, 0, 0, mAspm, mAspmL1_1, \
    mAspmL1_2, 0, mHotplug, mSlotNum, { 0, mMaxLinkCap, 0, mClkPmSupport, 0, 1 }, { 0, 0, 0 }, \
    HsioEndpointDetect }, \
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { 0, 0, 0, 0, 0 }, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } }

#define  HSIO_PORT_DATA_INITIALIZER_PCIE_V2(mPortPresent, \
                                            mDevAddress, \
                                            mDevFunction, \
                                            mHotplug, \
                                            mMaxLinkSpeed, \
                                            mMaxLinkCap, \
                                            mAspm, \
                                            mAspmL1_1, \
                                            mAspmL1_2,  \
                                            mClkPmSupport, \
                                            mClkReq, \
                                            mSlotNum) \
  { mPortPresent, 3, 0, mDevAddress, mDevFunction, mMaxLinkSpeed, 0, 0, mAspm, mAspmL1_1, \
    mAspmL1_2, mClkReq, mHotplug, mSlotNum, { 0, mMaxLinkCap, 0, mClkPmSupport, 0, 1 }, { 0, 0, 0 }, \
    HsioEndpointDetect }, \
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { 0, 0, 0, 0, 0 }, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } }

#define  HSIO_PORT_DATA_INITIALIZER_PCIE_V3(mPortPresent, \
                                            mDevAddress, \
                                            mDevFunction, \
                                            mHotplug, \
                                            mMaxLinkSpeed, \
                                            mMaxLinkCap, \
                                            mAspm, \
                                            mAspmL1_1, \
                                            mAspmL1_2,  \
                                            mClkPmSupport, \
                                            mClkReq, \
                                            mAutoSpdChange, \
                                            mSlotNum) \
  { mPortPresent, 3, 0, mDevAddress, mDevFunction, mMaxLinkSpeed, mAutoSpdChange, 0, mAspm, mAspmL1_1, \
    mAspmL1_2, mClkReq, mHotplug, mSlotNum, { 0, mMaxLinkCap, 0, mClkPmSupport, 0, 1 }, { 0, 0, 0 }, \
    HsioEndpointDetect }, \
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { 0, 0, 0, 0, 0 }, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } }

#define  HSIO_PORT_DATA_INITIALIZER_PCIE_V4(mPortPresent, \
                                            mDevAddress, \
                                            mDevFunction, \
                                            mHotplug, \
                                            mMaxLinkSpeed, \
                                            mMaxLinkCap, \
                                            mAspm, \
                                            mAspmL1_1, \
                                            mAspmL1_2,  \
                                            mClkPmSupport, \
                                            mEqPreset, \
                                            mSlotNum) \
  { mPortPresent, 3, 0, mDevAddress, mDevFunction, mMaxLinkSpeed, 0, mEqPreset, mAspm, mAspmL1_1, \
    mAspmL1_2, 0, mHotplug, mSlotNum, { 0, mMaxLinkCap, 0, mClkPmSupport, 0, 1 }, { 0, 0, 0 }, \
    HsioEndpointDetect }, \
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { 0, 0, 0, 0, 0 }, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } }

#define  HSIO_PORT_DATA_INITIALIZER_CHIP(mPortPresent, mDevAddress, mDevFunction, mHotplug, mMaxLinkSpeed, mMaxLinkCap, \
                                         mAspm, mAspmL1_1, mAspmL1_2, mClkPmSupport) \
  { mPortPresent, 3, 0, mDevAddress, mDevFunction, mMaxLinkSpeed, 0, 0, mAspm, mAspmL1_1, \
    mAspmL1_2, 0, mHotplug, 0, { 0, mMaxLinkCap, 1, mClkPmSupport, 0, 0 }, { 0, 0, 0 }, \
    HsioEndpointDetect }, \
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { 0, 0, 0, 0, 0 }, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } }

#define  HSIO_PORT_DATA_INITIALIZER_SATA(mPortPresent)  { mPortPresent, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                                                          { 0, 0, 0, 0, 0, 0 }, { 0, 0, 0 }, 0 }, \
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { 0, 0, 0, 0, 0 }, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } }

#define  HSIO_PORT_DATA_INITIALIZER_SATA_V2(mPortPresent, mChannelType)  { mPortPresent, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, \
                                                                           0, { 0, 0, 0, 0, mChannelType, 0 }, \
                                                                           { 0, 0, 0 }, 0 }, \
    { { 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }, \
    { 0, 0, 0, 0, 0 }, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 } }

#define  HSIO_PORT_DATA_INITIALIZER_ENET(mPortPresent, \
                                         mPortNum, \
                                         mPlatConf, \
                                         mMdioId, \
                                         mSuppSpeed, \
                                         mConnType, \
                                         mMdioReset, \
                                         mMdioGpioResetNum, \
                                         mSfpGpioAdd, \
                                         mTxFault, \
                                         mRs, \
                                         mModAbs, \
                                         mRxLoss, \
                                         mSfpGpioMask, \
                                         mSfpMux, \
                                         mSfpBusSeg, \
                                         mSfpMuxUpAdd, \
                                         mRedriverAddress, \
                                         mRedriverInterface, \
                                         mRedriverLane, \
                                         mRedriverModel, \
                                         mRedriverPresent, \
                                         mSlotMux, \
                                         mSlotBusSeg, \
                                         mSlotMuxUpAdd, \
                                         mInfoRomAdd, \
                                         mInfoRomAddUP, \
                                         InfoRomLocation, \
                                         mPadMux0, \
                                         mPadMux1, \
                                         mMacAddressLo, \
                                         mMacAddressHi, \
                                         mTxEqPre, \
                                         mTxEqMain, \
                                         mTxEqPost) \
  { mPortPresent, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0 }, { 0, 0, 0 }, 0 }, \
  { { mPortNum, mPlatConf, 0, mMdioId, 0, mSuppSpeed, 0, mConnType, 0 }, \
    { mMdioReset, 0, mMdioGpioResetNum, 0, mSfpGpioAdd, 0, mTxFault, mRs, mModAbs, mRxLoss, mSfpGpioMask }, \
    { mSfpMux, 0, mSfpBusSeg, 0, mSfpMuxUpAdd, 0, mRedriverAddress, mRedriverInterface, mRedriverLane, 0, \
      mRedriverModel, mRedriverPresent }, \
    { mSlotMux, mSlotBusSeg, mSlotMuxUpAdd, mInfoRomAdd, mInfoRomAddUP, InfoRomLocation, 0 }, mPadMux0, mPadMux1, \
      mMacAddressLo, mMacAddressHi, \
    { mTxEqPre, 0, mTxEqMain, 0, mTxEqPost, 0 } }

#define PCIE_EQ(Flag, Type, Socket, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, ...) \
  {Flag, Type, Socket, 0, 0, VidDid, SubSysVidDid, StartLane, EndLane, PerPort, { __VA_ARGS__ }, 0, {0, 0, 0}, 0, {0, 0, 0}}

// Iod must be set 0 for HyEx, USB is on NBIO0/2
// Nbio must be set 0 for HyGx  
#define USB_EQ(Flag, Type, Socket, Iod, Nbio, UsbPort, ...) \
  {Flag, Type, Socket, Iod, Nbio, 0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, UsbPort, { __VA_ARGS__ }, 0, {0, 0, 0}}

#define SATA_EQ(Flag, Type, Socket, Iod, Nbio, SataPort, ...) \
  {Flag, Type, Socket, Iod, Nbio, 0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 0, {0, 0, 0}, SataPort, { __VA_ARGS__ }}

#define DIMM_MAP(Socket, Cdd, UMC, DMIM, LogicChannel, PostFix) {Socket, Cdd, UMC, DMIM, LogicChannel, PostFix}
#endif // _HGPI_H_
