/* $NoKeywords:$ */

/**
 * @file
 *
 * HSIO component definitions.
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

#ifndef _HSIO_H_
#define _HSIO_H_

#pragma pack (push, 1)

#define HSIO_MBOX_RETURN_NONPOSTED     0
#define HSIO_MBOX_RETURN_POSTED        1
#define HSIO_MBOX_RETURN_POSTED_DEFRD  3

#define HSIO_MSG_INIT_INIT                   0x0
#define HSIO_MSG_INIT_SETMACPCSPRESENT       0x1
#define HSIO_MSG_INIT_SCANCAPABILITIES       0x2
#define HSIO_MSG_INIT_LOADENGINECONFIG       (0x3 + (HSIO_MBOX_RETURN_POSTED_DEFRD << 8))
#define HSIO_MSG_INIT_CANNEDCONFIG           0x4
#define HSIO_MSG_INIT_CANNEDCONFIGANDGO      0x5
#define HSIO_MSG_INIT_CANNEDCONFIG_SRAM      0x6
#define HSIO_MSG_LISM_GO                     (0x7 + (HSIO_MBOX_RETURN_POSTED_DEFRD << 8))
#define HSIO_MSG_LISM_RESUME                 (0x8 + (HSIO_MBOX_RETURN_POSTED_DEFRD << 8))
#define HSIO_MSG_LISM_COLLECTRESULTS         0x9
#define HSIO_MSG_INIT_DMAENGINECONIFGTODRAM  0x14
#define HSIO_MSG_LISM_RELOAD                 45

// HPSM Messages
#define HSIO_MSG_HPSM_UPDATENOTIFICATION  0xF              /*!< notify HSIO of a change in a GPIO for hotplug */
#define HSIO_MSG_HPSM_ENGINEINITCOMPLETE  0x15             /*!< all engine init is complete ... set runtime flag */
#define HSIO_MSG_HPSM_RESUME              0x16             /*!< resume the current HPSM operation */
#define HSIO_MSG_HPSM_COLLECTRESULTS      0x17             /*!< collect response of the previous HPSM operation */

#define HSIO_MSG_PHYS_TARGETFREQUPDATE  0x10
#define HSIO_MSG_LOCK_SETLOCKSTATE      0x11
#define HSIO_MSG_LOCK_GETLOCKSTATE      0x12

#define HSIO_MSG_DBUG_GETVERSION  0x13
#define HSIO_MSG_DBUG_SETSCRATCH  0x22

#define HSIO_MSG_INIT_LOADHEAP          0x23               /*!< DMA a heap section from DRAM or SRAM into the HSIO table */
#define HSIO_MSG_INIT_LOADCAPABILITIES  0x24               /*!< DMA the capabilities from DRAM or SRAM into the HSIO table */

#define HSIO_MSG_RNTM_PCIERETRAIN               41
#define HSIO_MSG_RNTM_PCIEDISABLE               42
#define HSIO_MSG_RNTM_DMADELI                   43
#define HSIO_MSG_RNTM_CONFIGDONE                44
#define HSIO_MSG_RNTM_ENABLEFEATURE             52
#define HSIO_MSG_RNTM_PCIELINKMANAGEMENT        70
#define HSIO_MSG_RNTM_PCIELINKSPEEDLIMIT        71
#define HSIO_MSG_RNTM_PCIELINKTRAINING          72
#define HSIO_MSG_RNTM_ACDC_SWITCH               73
#define HSIO_MSG_RNTM_CHANGESPEEDPERPSPPPOLICY  74

/// Heap Type defines for HSIO_MSG_INIT_LOADHEAP
#define HSIO_HEAPTYPE_EMPTY    0
#define HSIO_HEAPTYPE_FABINIT  1
#define HSIO_HEAPTYPE_MACPCS   2
#define HSIO_HEAPTYPE_CFGPTR   3
#define HSIO_HEAPTYPE_CAPPTR   4
#define HSIO_HEAPTYPE_GPIOPTR  5
#define HSIO_HEAPTYPE_ANCDATA  6

#define HSIO_MBOX_RETVAL_NULL          0
#define HSIO_MBOX_RETVAL_TIMEOUT       1
#define HSIO_MBOX_RETVAL_ERROR         2
#define HSIO_MBOX_RETVAL_OK            3
#define HSIO_MBOX_RETVAL_UNKNOWN_LOCK  4
#define HSIO_MBOX_RETVAL_TRYAGAIN      5
#define HSIO_MBOX_RETVAL_MBOX_IDLE     6
#define HSIO_MBOX_RETVAL_MBOX_BUSY     7
#define HSIO_MBOX_RETVAL_MBOX_DONE     8

#define HSIO_MBOX_RETPARAMS_NONE       0
#define HSIO_MBOX_RETPARAMS_GENERIC    1
#define HSIO_MBOX_RETPARAMS_LISM       2
#define HSIO_MBOX_RETPARAMS_HPSM       3
#define HSIO_MBOX_RETPARAMS_REQ_RESET  4

#define HSIO_ARG_RNTM_ENABLEFEATURE_XHMIADAPTATION  00

#define HSIO_LinkInitState_init           0
#define HSIO_LinkInitState_disabled       1
#define HSIO_LinkInitState_scanned        2
#define HSIO_LinkInitState_canned         3
#define HSIO_LinkInitState_loaded         4
#define HSIO_LinkInitState_configured     5
#define HSIO_LinkInitState_earlyTraining  6
#define HSIO_LinkInitState_earlyTrained   7
#define HSIO_LinkInitState_vetting        8
#define HSIO_LinkInitState_getvet         9
#define HSIO_LinkInitState_noVetting      10
#define HSIO_LinkInitState_gpiosInit      11
#define HSIO_LinkInitState_NHPTraining    12
#define HSIO_LinkInitState_done           13
#define HSIO_LinkInitState_error          14
#define HSIO_LinkInitState_mapped         15

#define HSIO_EngineTrainingState_IdleUnconn      0
#define HSIO_EngineTrainingState_IdleConn        1
#define HSIO_EngineTrainingState_ResetRequested  2
#define HSIO_EngineTrainingState_Training        3
#define HSIO_EngineTrainingState_ActiveConn      4
#define HSIO_EngineTrainingState_ActiveUnconn    5
#define HSIO_EngineTrainingState_IdlePhyReset    7
#define HSIO_EngineTrainingState_Error           8
#define HSIO_EngineTrainingState_UNKNOWN         9

#define HSIO_freqUpdType_restoreOnly  0           /*!< use previously saved values and do not apply per-lane resets. */
#define HSIO_freqUpdType_all          1           /*!< update all engines of the given type */
#define HSIO_freqUpdType_single       2           /*!< update single engine of the given type */

#define DBUG_SETSCRATCHFLAG_SKIPPSPMESSAGES     13
#define DBUG_SETSCRATCHFLAG_RETURNAFTERMAPPING  14
#define DBUG_SETSCRATCHFLAG_RETURNAFTERCONFIG   15
#define DBUG_SETSCRATCHFLAG_V1ANCILLARYDATA     16
#define DBUG_SETSCRATCHFLAG_UNKNOWN             17

/// HSIO Return Parameters
typedef struct {
  UINT32    MP1Mbox_RetVal;              ///< Return value
  UINT32    RetParams_Type      : 8;     ///< Type
  UINT32    RetParams_NumParams : 8;     ///< NumParams
  UINT32    RetParams_Reserved  : 16;    ///< Reserved
  UINT32    RetParams_Param0;            ///< Param0
  UINT32    RetParams_Param1;            ///< Param1
  UINT32    RetParams_Param2;            ///< Param2
  UINT32    RetParams_Param3;            ///< Param3
} HSIO_RETURN_PARAMS;

/// HSIO LISM Return Paramaters
typedef struct {
  UINT32    CurrentState;                ///< Current State
  UINT32    RetParams_Param1;            ///< Param1
  UINT32    RetParams_Param2;            ///< Param2
  UINT32    RetParams_Param3;            ///< Param3
} LISM_RETURN_PARAMS;

/// HSIO Reset Request Return Paramaters
typedef struct {
  UINT32    resetAssertDeassert;         ///< 0=Assert, 1=De-Assert
  UINT32    GpioId;                      ///< GpioId
  UINT32    RetParams_Param2;            ///< Param2
  UINT32    RetParams_Param3;            ///< Param3
} RESET_REQUEST_PARAMS;

/// HSIO Link Capabilities
typedef struct {
  UINT32    portPresent           : 1;         ///< portPresent
  UINT32    earlyTrainLink        : 1;         ///< earlyTrainLink
  UINT32    linkComplianceMode    : 1;         ///< linkComplianceMode
  UINT32    linkReversal          : 1;         ///< linkReversal
  UINT32    maxLinkSpeedCap       : 3;         ///< maxLinkSpeedCap
  UINT32    endpointStatus        : 1;         ///< endpointStatus
  UINT32    linkHotplug           : 3;         ///< linkHotplug
  UINT32    portSize              : 5;         ///< portSize
  UINT32    maxLinkSpeedTrained   : 3;         ///< maxLinkSpeedTrained
  UINT32    enableTurnOffSettings : 1;         ///< enableTurnOffSettings
  UINT32    turnOffUnusedLanes    : 1;         ///< enableTurnOffSettings
  UINT32    ntbHotPlug            : 1;         ///< NTB Hotplug Support
  UINT32    CurrentSpeed          : 2;         ///< PSPP Current Speed
  UINT32    PSPPMode              : 3;         ///< PSPP Mode
  UINT32    partnerDeviceType     : 2;         ///< Partner Device Type
  UINT32    autoSpeedChangeCntrl  : 2;         ///< Auto Speed Change Control
  UINT32    capabilities0         : 1;         ///< capabilities0
  UINT32    eqSearchMode          : 2;         ///< eqSearchMode
  UINT32    eqSearchModeOride     : 1;         ///< eqSearchModeOride
  UINT32    capabilities1         : 29;        ///< capabilities1
} HSIO_LINK_CAP;

/// HSIO Link Descriptor Base
typedef struct {
  UINT32           ChannelType         : 8;    ///< ChannelType
  UINT32           ChannelDescID       : 8;    ///< ChannelDescID
  UINT32           AncillaryDataOffset : 16;   ///< Reserved3
  UINT32           DevFunNumber        : 32;   ///< DevFunNumber
  HSIO_LINK_CAP    Capabilities;               ///< Capabilities
  UINT32           MacID               : 8;    ///< MacID
  UINT32           MacPortID           : 8;    ///< MacPortID
  UINT32           StartCoreLane       : 8;    ///< StartCoreLane
  UINT32           EndCoreLane         : 8;    ///< EndCoreLane
  UINT32           PcsID               : 8;    ///< PcsID
  UINT32           Reserved1           : 24;   ///< Reserved1
} HSIO_LINK_DESC_BASE;

/// HSIO Link Descriptor Eth
typedef struct {
  UINT32           ChannelType         : 8;    ///< ChannelType
  UINT32           Reserved1           : 8;    ///< Reserved1
  UINT32           AncillaryDataOffset : 16;   ///< AncillaryDataOffset
  UINT32           DevFunNumber        : 32;   ///< DevFunNumber
  HSIO_LINK_CAP    Capabilities;               ///< Capabilities
  UINT32           Reserved2           : 32;   ///< Reserved2
  UINT32           Reserved3           : 32;   ///< Reserved3
} HSIO_LINK_DESC_ETH;

/// HSIO Link Descriptor PCIe
typedef struct {
  UINT32           ChannelType         : 8;    ///< ChannelType
  UINT32           ChannelDescID       : 8;    ///< ChannelDescID
  UINT32           AncillaryDataOffset : 16;   ///< Reserved3
  UINT32           DevFunNumber        : 32;   ///< DevFunNumber
  HSIO_LINK_CAP    Capabilities;               ///< Capabilities
  UINT32           MacID               : 8;    ///< MacID
  UINT32           MacPortID           : 8;    ///< MacPortID
  UINT32           StartCoreLane       : 8;    ///< StartCoreLane
  UINT32           EndCoreLane         : 8;    ///< EndCoreLane
  UINT32           PcsID               : 8;    ///< PcsID
  UINT32           Reserved1           : 24;   ///< Reserved1
} HSIO_LINK_DESC_PCIE;

typedef struct {
  UINT32           ChannelType         : 8;    ///< ChannelType
  UINT32           ChannelDescID       : 8;    ///< ChannelDescID
  UINT32           AncillaryDataOffset : 16;   ///< Reserved3
  UINT32           DevFunNumber        : 32;   ///< DevFunNumber
  HSIO_LINK_CAP    Capabilities;               ///< Capabilities
  UINT32           MacID               : 8;    ///< MacID
  UINT32           MacPortID           : 8;    ///< MacPortID
  UINT32           StartCoreLane       : 8;    ///< StartCoreLane
  UINT32           EndCoreLane         : 8;    ///< EndCoreLane
  UINT32           PcsID               : 8;    ///< PcsID
  UINT32           linkTrainingState   : 8;    ///< Training Mode
  UINT32           WorkMode            : 1;    ///< 0: PCIE mode,  1:CXL mode
  UINT32           CxlMode             : 1;    ///< 0: CXL 1.1,  1: Cxl2.0
  UINT32           Reserved1           : 14;   ///< Reserved1
} HSIO_LINK_DESC_CXL;

typedef HSIO_LINK_DESC_BASE  HSIO_LINK_DESC_SATA;
typedef HSIO_LINK_DESC_BASE  HSIO_LINK_DESC_USB;
typedef HSIO_LINK_DESC_BASE  HSIO_LINK_DESC_DP;
typedef HSIO_LINK_DESC_BASE  HSIO_LINK_DESC_GOP;
typedef HSIO_LINK_DESC_BASE  HSIO_LINK_DESC_HMI;

/// HSIO Link Descriptor
typedef union {
  HSIO_LINK_DESC_BASE    Base;            ///< HSIO Link Descriptor Base
  HSIO_LINK_DESC_PCIE    Pcie;            ///< HSIO Link Descriptor PCIe
  HSIO_LINK_DESC_CXL     Cxl;             ///< HSIO Link Descriptor Cxl
  HSIO_LINK_DESC_SATA    Sata;            ///< HSIO Link Descriptor Sata
  HSIO_LINK_DESC_USB     Usb;             ///< HSIO Link Descriptor Usb
  HSIO_LINK_DESC_DP      Dp;              ///< HSIO Link Descriptor Dp
  HSIO_LINK_DESC_ETH     Eth;             ///< HSIO Link Descriptor Eth
  HSIO_LINK_DESC_GOP     Gop;             ///< HSIO Link Descriptor Gop
  HSIO_LINK_DESC_HMI     Hmi;             ///< HSIO Link Descriptor Hmi
} HSIO_LINK_DESC;

/// HSIO Engine Descriptor
typedef struct {
  UINT32            EngineType    : 8;         ///< EngineType
  UINT32            HotPluggable  : 1;         ///< HotPluggable
  UINT32            Reserved3     : 7;         ///< Reserved3
  UINT32            StartLane     : 8;         ///< StartLane
  UINT32            EndLane       : 8;         ///< EndLane
  UINT32            GpioGroupID   : 8;         ///< GpioGroupID
  UINT32            engineResetID : 8;         ///< engineResetID
  UINT32            Reserved2     : 16;        ///< Reserved2
  HSIO_LINK_DESC    LinkDesc;                  ///< LinkDesc
  UINT32            MacEnginePtr  : 16;        ///< MacEnginePtr
  UINT32            FirstLgdNum   : 8;         ///< FirstLgdNum
  UINT32            LastLgdNum    : 8;         ///< LastLgdNum
  UINT32            TrainingState : 4;         ///< TrainingState
  UINT32            Reserved1     : 28;        ///< Reserved1
} HSIO_ENGINE_DESCRIPTOR;

/// HSIO Platform Descriptor
typedef struct {
  UINT32    PlatformType  : 16;                 ///< PlatformType
  UINT32    Reserved4     : 16;                 ///< Reserved4
  UINT32    Reserved3     : 32;                 ///< Reserved3
  UINT32    Reserved2     : 32;                 ///< Reserved2
  UINT32    NumEngineDesc : 16;                 ///< NumEngineDesc
  UINT32    Reserved1     : 16;                 ///< Reserved1
} HSIO_PLATFORM_DESCRIPTOR;

// ------------------------------------------------------------------------------
// SpeedSet Overrides
// ------------------------------------------------------------------------------

/// HSIO Speed Set Override Types
typedef enum {
  HSIO_OVRSSET_FLD_txX_eq_pre,
  HSIO_OVRSSET_FLD_txX_eq_main,
  HSIO_OVRSSET_FLD_txX_eq_post,
  HSIO_OVRSSET_FLD_txX_iboost_lvl,
  HSIO_OVRSSET_FLD_txX_vboost_en,
  HSIO_OVRSSET_FLD_rxX_adapt_afe_en,
  HSIO_OVRSSET_FLD_rxX_adapt_dfe_en,
  HSIO_OVRSSET_FLD_rxX_eq_att_lvl,
  HSIO_OVRSSET_FLD_rxX_eq_vga1_gain,
  HSIO_OVRSSET_FLD_rxX_eq_vga2_gain,
  HSIO_OVRSSET_FLD_rxX_eq_ctle_pole,
  HSIO_OVRSSET_FLD_rxX_eq_ctle_boost,
  HSIO_OVRSSET_FLD_rxX_eq_dfe_tap1,
  HSIO_OVRSSET_FLD_rxX_los_threshold,
  HSIO_OVRSSET_FLD_rx_vref_ctrl,
  HSIO_OVRSSET_FLD_tx_vboost_lvl,
  HSIO_OVRSSET_FLD_SSC_OFF_FRUG1,
  HSIO_OVRSSET_FLD_SSC_OFF_PHUG1,
  HSIO_OVRSSET_FLD_NUM_OVERRIDE_FIELDS
} ENUM_HSIO_OVRSSET_FIELDS;

/// structures for speedSet overrides
typedef struct {
  UINT32    overrideField : 8;           // which field is to be over-ridden
  UINT32    fieldGen      : 2;           // protocol generation i.e. Pcie Gen1/2/3
  UINT32    laneNumber    : 6;           // lane for over-ride
  UINT32    value         : 16;          // new value for over-ride
} HSIO_OVRSSET_ENTRY;

/// ------------------------------------------------------------------------------
/// HSIO ANCILLARY DATA SUBHEAP
/// ------------------------------------------------------------------------------

#define HSIO_ANCILLARYDATA_SUBHEAP_VERSION  1

/// HSIO Ancillary Data Subheap Types
typedef enum {
  HSIO_ANCDATA_SUBHEAPTYPE_XGBE    = 1,
  HSIO_ANCDATA_SUBHEAPTYPE_HIER    = 2,
  HSIO_ANCDATA_SUBHEAPTYPE_OVRRSET = 3
} HSIO_ANCDATA_SUBHEAPTYPE;

/// HSIO Ancillary Data Subheap Type Structure
typedef struct {
  UINT32    descriptorType : 8;
  UINT32    version        : 4;
  UINT32    reserved1      : 4;
  UINT32    numDWORDs      : 8;
  UINT32    reserved2      : 8;
} HSIO_ANCILLARYDATA_SUBHEAP;

// ------------------------------------------------------------------------------
// HSIO ANCILLARY DATA OVERALL STRUCTURE
// ------------------------------------------------------------------------------

#define HSIO_ANCILLARYDATA_VERSION  0

typedef struct {
  UINT32    descriptorType : 8;
  UINT32    version        : 4;
  UINT32    reserved1      : 4;
  UINT32    numDWords      : 8;
  UINT32    reserved2      : 8;
} HSIO_ANCILLARYDATA;

// ------------------------------------------------------------------------------
// HSIO HEAP DATA CONTAINER
// ------------------------------------------------------------------------------

#define HSIO_HEAPDATA_CONTAINER_VERSION  0

typedef struct {
  UINT32    heapType  : 8;
  UINT32    version   : 4;
  UINT32    reserved  : 4;
  UINT32    numDWords : 16;
} HSIO_HEAPDATA_CONTAINER;

#pragma pack (pop)

#endif
