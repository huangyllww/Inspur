/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Family 17h Satori register defination
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

#ifndef _FABRIC_REGISTERS_ST_H_
#define _FABRIC_REGISTERS_ST_H_
#include "HGPI.h"
#pragma pack (push, 1)

#define  SOCKET0_IODIE_DEVNUM_IN_GROUP_MODE      24
#define  SOCKET0_IODIE_DEVNUM_IN_FLAT_MODE       12
#define  SOCKET0_CDD0_DEVNUM_IN_FLAT_MODE_HYEX    13
#define  SOCKET0_CDD0_DEVNUM_IN_FLAT_MODE_HYGX    16

#define  CS0_FABRIC_ID            0
#define  CS1_FABRIC_ID            1
#define  CS2_FABRIC_ID            2
#define  CS3_FABRIC_ID            3
#define  CCM0_FABRIC_ID           4
#define  CCM1_FABRIC_ID           5
#define  CCM2_FABRIC_ID           6
#define  CCM3_FABRIC_ID           7
#define  IOMS0_FABRIC_ID          8
#define  PIE_FABRIC_ID_HYEX        9
#define  PIE_FABRIC_ID_HYGX        12

#define  CS0_INSTANCE_ID          0
#define  CS1_INSTANCE_ID          1
#define  CS2_INSTANCE_ID          2
#define  CS3_INSTANCE_ID          3
#define  CCM0_INSTANCE_ID         4
#define  CCM1_INSTANCE_ID         5
#define  CCM2_INSTANCE_ID         6
#define  CCM3_INSTANCE_ID         7
#define  IOMS0_INSTANCE_ID        8
#define  IOMS1_INSTANCE_ID        9
#define  IOMS2_INSTANCE_ID        10
#define  IOMS3_INSTANCE_ID        11
#define  PIE_INSTANCE_ID          12

#define  CAKE0_INSTANCE_ID        13
#define  CAKE1_INSTANCE_ID        14
#define  CAKE2_INSTANCE_ID        15
#define  CAKE3_INSTANCE_ID        16
#define  CAKE4_INSTANCE_ID        17
#define  CAKE5_INSTANCE_ID        18

//HyEx define
#define  CAKE6_INSTANCE_ID_HYEX   19
#define  CAKE7_INSTANCE_ID_HYEX   20
#define  CAKE8_INSTANCE_ID_HYEX   21
#define  CAKE9_INSTANCE_ID_HYEX   22
#define  CAKE10_INSTANCE_ID_HYEX  23
#define  CAKE11_INSTANCE_ID_HYEX  24
#define  CAKE12_INSTANCE_ID_HYEX  25
#define  CAKE13_INSTANCE_ID_HYEX  26
#define  TCDX0_INSTANCE_ID_HYEX   27
#define  TCDX1_INSTANCE_ID_HYEX   28
#define  TCDX2_INSTANCE_ID_HYEX   29
#define  TCDX3_INSTANCE_ID_HYEX   30
#define  TCDX4_INSTANCE_ID_HYEX   31
#define  TCDX5_INSTANCE_ID_HYEX   32
#define  TCDX6_INSTANCE_ID_HYEX   33

//HyGx define
#define  HMCD0_INSTANCE_ID_HYGX   19
#define  HMCD1_INSTANCE_ID_HYGX   20
#define  HMCD2_INSTANCE_ID_HYGX   21
#define  TCDX0_INSTANCE_ID_HYGX   22
#define  TCDX1_INSTANCE_ID_HYGX   23
#define  TCDX2_INSTANCE_ID_HYGX   24
#define  TCDX3_INSTANCE_ID_HYGX   25
#define  TCDX4_INSTANCE_ID_HYGX   26
#define  TCDX5_INSTANCE_ID_HYGX   27
#define  TCDX6_INSTANCE_ID_HYGX   28
#define  TCDX7_INSTANCE_ID_HYGX   29
#define  TCDX8_INSTANCE_ID_HYGX   30
#define  TCDX9_INSTANCE_ID_HYGX   31

//Fabirc ID shift
#define FABRIC_ID_SOCKET_SHIFT_HYEX          7
#define FABRIC_ID_SOCKET_SHIFT_HYGX          8
#define FABRIC_ID_SOCKET_SIZE_MASK          7

#define FABRIC_ID_DIE_SHIFT                 4
#define FABRIC_ID_DIE_SIZE_MASK_HYEX         7
#define FABRIC_ID_DIE_SIZE_MASK_HYGX         0xF

#define FABRIC_ID_COMPONENT_SHIFT           0
#define FABRIC_ID_COMPONENT_SIZE_MASK       0xF

#define FABRIC_ID_SOCKET_DIE_MASK_HYEX      ((FABRIC_ID_SOCKET_SIZE_MASK << FABRIC_ID_SOCKET_SHIFT_HYEX) || (FABRIC_ID_DIE_SIZE_MASK_HYEX << FABRIC_ID_DIE_SHIFT))
#define FABRIC_ID_SOCKET_DIE_MASK_HYGX      ((FABRIC_ID_SOCKET_SIZE_MASK << FABRIC_ID_SOCKET_SHIFT_HYGX) || (FABRIC_ID_DIE_SIZE_MASK_HYGX << FABRIC_ID_DIE_SHIFT))

//Die Id define
#define FABRIC_ID_IO_DIE_NUM    0
#define FABRIC_ID_CDD0_DIE_NUM  4
#define FABRIC_ID_CDD1_DIE_NUM  5
#define FABRIC_ID_CDD2_DIE_NUM  6
#define FABRIC_ID_CDD3_DIE_NUM  7

/* Fabric Block Instance Count Register */
#define FABBLKINSTCOUNT_FUNC  0x0
#define FABBLKINSTCOUNT_REG   0x040

/// Fabric Block Instance Count Register
typedef union {
  struct {
    ///< Bitfields of Fabric Block Instance Count Register
    UINT32    BlkInstCount : 8;      ///< Block Instance Count
    UINT32                 : 24;     ///< Reserved
  } Field;
  UINT32    Value;
} FABRIC_BLK_INST_COUNT_REGISTER;

/* Fabric Dram Addr Config 0 Register, only HyGx support */
#define DRAMADDRCONFIG0_FUNC      0x0
#define DRAMADDRCONFIG0_REG_HYGX   0x060

/// Fabric Dram Addr Config 0 Register
typedef union {
  struct {
    ///<
    UINT32    IntLvNumDies0  : 2;    ///< Dram pair 0 IntLvNumDies field
    UINT32    IntLvNumDies1  : 2;    ///< Dram pair 1 IntLvNumDies field
    UINT32    IntLvNumDies2  : 2;    ///< Dram pair 2 IntLvNumDies field
    UINT32    IntLvNumDies3  : 2;    ///< Dram pair 3 IntLvNumDies field
    UINT32    IntLvNumDies4  : 2;    ///< Dram pair 4 IntLvNumDies field
    UINT32    IntLvNumDies5  : 2;    ///< Dram pair 5 IntLvNumDies field
    UINT32    IntLvNumDies6  : 2;    ///< Dram pair 6 IntLvNumDies field
    UINT32    IntLvNumDies7  : 2;    ///< Dram pair 7 IntLvNumDies field
    UINT32    IntLvNumDies8  : 2;    ///< Dram pair 8 IntLvNumDies field
    UINT32    IntLvNumDies9  : 2;    ///< Dram pair 9 IntLvNumDies field
    UINT32    IntLvNumDies10 : 2;    ///< Dram pair 10 IntLvNumDies field
    UINT32    IntLvNumDies11 : 2;    ///< Dram pair 11 IntLvNumDies field
    UINT32    IntLvNumDies12 : 2;    ///< Dram pair 12 IntLvNumDies field
    UINT32    IntLvNumDies13 : 2;    ///< Dram pair 13 IntLvNumDies field
    UINT32    IntLvNumDies14 : 2;    ///< Dram pair 14 IntLvNumDies field
    UINT32    IntLvNumDies15 : 2;    ///< Dram pair 15 IntLvNumDies field
  } Field;
  UINT32    Value;
} FABRIC_DRAM_ADDR_CONFIG0_REGISTER_HYGX;

/* Fabric Dram Addr Config 1 Register, only HyGx support */
#define DRAMADDRCONFIG1_FUNC      0x0
#define DRAMADDRCONFIG1_REG_HYGX   0x064

/// Fabric Dram Addr Config 1 Register
typedef union {
  struct {
    ///<
    UINT32    IntLvNumDies16 : 2;     ///< Dram pair 16 IntLvNumDies field
    UINT32    IntLvNumDies17 : 2;     ///< Dram pair 17 IntLvNumDies field
    UINT32    IntLvNumDies18 : 2;     ///< Dram pair 18 IntLvNumDies field
    UINT32    IntLvNumDies19 : 2;     ///< Dram pair 19 IntLvNumDies field
    UINT32    IntLvNumDies20 : 2;     ///< Dram pair 20 IntLvNumDies field
    UINT32    IntLvNumDies21 : 2;     ///< Dram pair 21 IntLvNumDies field
    UINT32    IntLvNumDies22 : 2;     ///< Dram pair 22 IntLvNumDies field
    UINT32    IntLvNumDies23 : 2;     ///< Dram pair 23 IntLvNumDies field
    UINT32    IntLvNumDies24 : 2;     ///< Dram pair 24 IntLvNumDies field
    UINT32    IntLvNumDies25 : 2;     ///< Dram pair 25 IntLvNumDies field
    UINT32    IntLvNumDies26 : 2;     ///< Dram pair 26 IntLvNumDies field
    UINT32    IntLvNumDies27 : 2;     ///< Dram pair 27 IntLvNumDies field
    UINT32    IntLvNumDies28 : 2;     ///< Dram pair 28 IntLvNumDies field
    UINT32    IntLvNumDies29 : 2;     ///< Dram pair 29 IntLvNumDies field
    UINT32    IntLvNumDies30 : 2;     ///< Dram pair 30 IntLvNumDies field
    UINT32    IntLvNumDies31 : 2;     ///< Dram pair 31 IntLvNumDies field
  } Field;
  UINT32    Value;
} FABRIC_DRAM_ADDR_CONFIG1_REGISTER_HYGX;

/* VGA Enable Register */
#define VGA_EN_FUNC  0x0
#define VGA_EN_REG   0x080

/// VGA Enable Register
typedef union {
  struct {
    UINT32    VgaEn_VE          : 1;  ///< VGA Enable
    UINT32    VgaEn_NP          : 1;  ///< Non-Posted
    UINT32    VgaEn_CpuDis      : 1;  ///< CpuDis
    UINT32                      : 1;  ///< Reserved
    UINT32    VgaEn_DstFabricID : 11; ///< Destination Fabric ID, HyEx only 10 bits
    UINT32                      : 17; ///< Reserved
  } Field;
  UINT32    Value;
} VGA_EN_REGISTER;

/* Configuration Address Control Register */
#define CFGADDRESSCTRL_FUNC  0x0
#define CFGADDRESSCTRL_REG   0x084

/// Configuration Address Control Register
typedef union {
  struct {
    ///< Bitfields of Configuration Address Control Register
    UINT32    SecBusNum : 8;         ///< Secondary Bus Number
    UINT32              : 24;        ///< Reserved
  } Field;
  UINT32    Value;
} CFG_ADDRESS_CTRL_REGISTER;

/* Configuration Address Maps Register */
#define CFGADDRESSMAP_FUNC   0x0
#define CFGADDRESSMAP0_REG   0x0C0
#define CFGADDRESSMAP1_REG   0x0C4
#define CFGADDRESSMAP2_REG   0x0C8
#define CFGADDRESSMAP3_REG   0x0CC
#define CFGADDRESSMAP4_REG   0x0D0
#define CFGADDRESSMAP5_REG   0x0D4
#define CFGADDRESSMAP6_REG   0x0D8
#define CFGADDRESSMAP7_REG   0x0DC
#define CFGADDRESSMAP8_REG   0x0E0
#define CFGADDRESSMAP9_REG   0x0E4
#define CFGADDRESSMAP10_REG  0x0E8
#define CFGADDRESSMAP11_REG  0x0EC
#define CFGADDRESSMAP12_REG  0x0F0
#define CFGADDRESSMAP13_REG  0x0F4
#define CFGADDRESSMAP14_REG  0x0F8
#define CFGADDRESSMAP15_REG  0x0FC

/// Configuration Address Maps Register
typedef union {
  struct {
    ///< Bitfields of Configuration Address Maps Register
    UINT32    RE          : 1;       ///< Read Enable
    UINT32    WE          : 1;       ///< Write Enable
    UINT32                : 2;       ///< Reserved
    UINT32    DstFabricID : 11;      ///< Destination Fabric ID, HyEx only 10 bits
    UINT32                : 1;       ///< Reserved
    UINT32    BusNumBase  : 8;       ///< Bus Number Base
    UINT32    BusNumLimit : 8;       ///< Bus Number Limit
  } Field;
  UINT32    Value;
} CFG_ADDRESS_MAP_REGISTER;

/* DRAM Hole Control Register */
#define DRAMHOLECTRL_FUNC  0x0
#define DRAMHOLECTRL_REG   0x104

/// DRAM Hole Control Register
typedef union {
  struct {
    ///< Bitfields of DRAM Hole Control Register
    UINT32    DramHoleValid : 1;     ///< DRAM Hole Valid
    UINT32                  : 23;    ///< Reserved
    UINT32    DramHoleBase  : 8;     ///< DRAM Hole Base
  } Field;
  UINT32    Value;
} DRAM_HOLE_CONTROL_REGISTER;

/* DRAM Regions */
#define NUMBER_OF_DRAM_REGIONS       0x20
#define DRAM_REGION_REGISTER_OFFSET  0x8

/* DRAM Base Address Register */
#define DRAMBASEADDR0_FUNC  0x0
#define DRAMBASEADDR0_REG   0x110

#define DRAMBASEADDR1_FUNC  0x0
#define DRAMBASEADDR1_REG   0x118

/// DRAM Base Address Register
typedef union {
  struct {
    ///< Bitfields of DRAM Base Address Register
    UINT32    AddrRngVal      : 1;   ///< Address Range Valid
    UINT32    LgcyMmioHoleEn  : 1;   ///< Legacy MMIO Hole Enable
    UINT32    IntLvNumSockets : 2;   ///< Interleave Number Of Sockets
    UINT32    IntLvNumChan    : 4;   ///< Interleave Number Of Channels
    UINT32    IntLvAddrSel    : 3;   ///< Interleave Address Select
    UINT32    DstDcu          : 1;   ///< Dst is DCU
    UINT32    DramBaseAddr    : 20;  ///< DRAM Base Address
  } Field;
  UINT32    Value;
} DRAM_BASE_ADDRESS_REGISTER;

/* DRAM Limit Address Register */
#define DRAMLIMITADDR0_FUNC  0x0
#define DRAMLIMITADDR0_REG   0x114

#define DRAMLIMITADDR1_FUNC  0x0
#define DRAMLIMITADDR1_REG   0x11C

/// DRAM Limit Address Register
typedef union {
  struct {                          ///< Bitfields of DRAM Limit Address Register
    UINT32    DstFabricID   : 10;   ///< Destination Fabric ID
    UINT32    IntLvNumDies  : 2;    ///< Interleave Number Of Dies
    UINT32    DramLimitAddr : 20;   ///< DRAM Limit Address
  } HyExField;
  struct {
    ///< Bitfields of DRAM Limit Address Register
    UINT32    DstFabricID   : 11;    ///< Destination Fabric ID
    UINT32                  : 1;     ///< Reserved
    UINT32    DramLimitAddr : 20;    ///< DRAM Limit Address
  } HyGxField;
  UINT32    Value;
} DRAM_LIMIT_ADDRESS_REGISTER;

#define X86IO_BASE_ADDRESS_REG0   0x220         // IO base address register
#define X86IO_LIMIT_ADDRESS_REG0  0x224         // IO limit address register

/* DF Global Clock Gater Register */
#define DF_GLBL_CLK_GATER_FUNC  0x0
#define DF_GLBL_CLK_GATER_REG   0x3F0

/// DF Global Clock Gater Register
typedef union {
  struct {
    ///< Bitfields of DF Global Clock Gater Register
    UINT32    MGCGMode : 4;               ///< Mid-grain clock gating mode
    UINT32             : 28;              ///< Reserved
  } Field;
  UINT32    Value;
} DF_GLBL_CLK_GATER_REGISTER;

/* DF Global Control Register */
#define DF_GLBL_CTRL_FUNC  0x0
#define DF_GLBL_CTRL_REG   0x3F8

/// DF Global Control Register
typedef union {
  struct {
    ///< Bitfields of DF Global Control Register
    UINT32                              : 1;  ///< Reserved
    UINT32    GlobalCpuScratchEn        : 1;  ///< Global Cpu Scratch Enable
    UINT32    GlobalCC6SaveEn           : 1;  ///< Global CC6 save enable
    UINT32    DisImmSyncFloodOnFatalErr : 1;  ///< Disable immediate sync flood on fatal error
    UINT32    WDTBaseSel                : 2;  ///< Watchdog timer base select
    UINT32                              : 2;  ///< Reserved
    UINT32    PIEWDTCntSel              : 3;  ///< PIE watchdog timer count select
    UINT32                              : 1;  ///< Reserved
    UINT32    IOMWDTCntSel              : 3;  ///< IO master watchdog timer count select
    UINT32                              : 1;  ///< Reserved
    UINT32    CCMWDTCntSel              : 3;  ///< Core coherent master watchdog timer count select
    UINT32                              : 13; ///< Reserved
  } Field;
  UINT32    Value;
} DF_GLBL_CTRL_REGISTER;

/* System Configuration Register */
#define SYSCFG_FUNC  0x1

/// System Configuration 1 Register
#define SYSCFG1_REG  0x1A8
typedef union {
  struct {
    ///< Bitfields of System Configuration 1 Register
    UINT32    LocalDieType0  : 2;      ///< Die type for local die 0
    UINT32    LocalDieType1  : 2;      ///< Die type for local die 1
    UINT32    LocalDieType2  : 2;      ///< Die type for local die 2
    UINT32    LocalDieType3  : 2;      ///< Die type for local die 3
    UINT32    LocalDieType4  : 2;      ///< Die type for local die 4
    UINT32    LocalDieType5  : 2;      ///< Die type for local die 5
    UINT32    LocalDieType6  : 2;      ///< Die type for local die 6
    UINT32    LocalDieType7  : 2;      ///< Die type for local die 7
    UINT32    LocalDieType8  : 2;      ///< Die type for local die 8
    UINT32    LocalDieType9  : 2;      ///< Die type for local die 9
    UINT32    LocalDieType10 : 2;      ///< Die type for local die 10
    UINT32    LocalDieType11 : 2;      ///< Die type for local die 11
    UINT32    LocalDieType12 : 2;      ///< Die type for local die 12
    UINT32    LocalDieType13 : 2;      ///< Die type for local die 13
    UINT32    LocalDieType14 : 2;      ///< Die type for local die 14
    UINT32    LocalDieType15 : 2;      ///< Die type for local die 15
  } Field;
  UINT32    Value;
} SYS_CFG1_REGISTER;

/// System Configuration 2 Register
#define SYSCFG2_REG  0x1AC
typedef union {
  struct {
    ///< Bitfields of System Configuration 2 Register
    UINT32    LocalDiePresent : 16;    ///< Local Die Present
    UINT32    LocalDieIsMe    : 16;    ///< Internal die number
  } Field;
  UINT32    Value;
} SYS_CFG2_REGISTER;

/// System Configuration Register
#define SYSCFG_REG  0x200
typedef union {
  struct {
    ///< Bitfields of System Configuration Register
    UINT32    SocketPresent : 8;       ///< Socket Present
    UINT32                  : 8;       ///< Reserved
    UINT32    MyDieType     : 2;       ///< Die type
    UINT32                  : 2;       ///< Reserved
    UINT32    MyDieId       : 4;       ///< Die ID
    UINT32                  : 4;       ///< Reserved
    UINT32    MySocketId    : 3;       ///< Socket ID
    UINT32                  : 1;       ///< Reserved
  } Field;
  UINT32    Value;
} SYS_CFG_REGISTER;

/* System Component Count Register */
#define SYSCOMPCOUNT_FUNC  0x1
#define SYSCOMPCOUNT_REG   0x204

/// System Configuration Register
typedef union {
  struct {
    ///< Bitfields of System Configuration Register
    UINT32    PIECount    : 8;         ///< PIE Count
    UINT32    GCMCount    : 8;         ///< GCM Count
    UINT32    IOMCount    : 8;         ///< IOM Count
    UINT32    IODIOSCount : 8;         ///< IODIOS Count
  } Field;
  UINT32    Value;
} SYS_COMP_COUNT_REGISTER;

#define CS_CONFIG_A2_FUNC  0x2
#define CS_CONFIG_A2_REG   0x048

/* DRAM Contention Monitor Register */
#define DRAM_CONT_MON_FUNC  0x2
#define DRAM_CONT_MON_REG   0x068

/// DRAM Contention Monitor Register
typedef union {
  struct {
    ///< Bitfields of DRAM Contention Monitor Register
    UINT32    DramContentionMonEn  : 1;   ///< DRAM contention monitor enable
    UINT32    DramContentionThresh : 5;   ///< DRAM contention threshold
    UINT32                         : 2;   ///< Reserved
    UINT32    UmcRdSampleWin       : 4;   ///< UMC read sample window
    UINT32    SpecDramRdSampleWin  : 4;   ///< Speculative DRAM read sample window
    UINT32    UmcRdThresh          : 8;   ///< UMC read threshold
    UINT32                         : 8;   ///< Reserved
  } Field;
  UINT32    Value;
} DRAM_CONT_MON_REGISTER;

/* Speculative DRAM Read Monitor Register */
#define SPEC_DRAM_RD_MON_FUNC  0x2
#define SPEC_DRAM_RD_MON_REG   0x06C

/// Speculative DRAM Read Monitor Register
typedef union {
  struct {
    ///< Bitfields of Speculative DRAM Read Monitor Register
    UINT32    SpecDramRdMonEn            : 1; ///< Speculative DRAM read monitor enable
    UINT32    SpecDramRdContentionThresh : 5; ///< Speculative DRAM read contention threshold
    UINT32                               : 2; ///< Reserved
    UINT32    SpecDramRdMissThresh       : 8; ///< Speculative DRAM read miss threshold
    UINT32    UmcRdThreshLow             : 8; ///< UMC read threshold low
    UINT32    UmcRdThreshHi              : 8; ///< UMC read threshold high
  } Field;
  UINT32    Value;
} SPEC_DRAM_RD_MON_REGISTER;

#define MMIO_SPACE_FUNC             2          // DF Function number of MMIO Space
#define MMIO_BASE_ADDRESS_REG_0     0x240      // MMIO base address register
#define MMIO_LIMIT_ADDRESS_REG_0    0x244      // MMIO limit address register
#define MMIO_ADDRESS_CONTROL_REG_0  0x248      // MMIO control register

/* IOM Control Register */
#define IOM_CTRL_FUNC  0x3
#define IOM_CTRL_REG   0x04C

/// IOM Control Register
typedef union {
  struct {
    ///< Bitfields of IOM Control Register
    UINT32    EnIomActIntSysLmt      : 1;  ///< Enable limit on total number of active IOM
    UINT32    IomActIntSysLmt        : 2;  ///< Total number of active or outstanding IOM
    UINT32                           : 1;  ///< Reserved
    UINT32    FrcIomNoSdpRdRspRule   : 1;  ///< Disable SDP Read Response ordering
    UINT32    FrcIomSdpVc7ToFtiVc4   : 1;  ///< Force mapping
    UINT32    FrcIomSdpVc2To7BLZero  : 1;  ///< Force BlockLevel
    UINT32    FrcIomSdpVc2To7Ordered : 1;  ///< Treat commands as if PassPW = 0
    UINT32    FrcIomRspPPWPassPWOne  : 1;  ///< Force RspPWW and PPW bit to 1
    UINT32                           : 23; ///< Reserved
  } Field;
  UINT32    Value;
} IOM_CTRL_REGISTER;

/* CCM Configuration Register */
#define CCM_CFG_FUNC  0x3
#define CCM_CFG_REG   0x104

/// CCM Configuration Register
typedef union {
  struct {
    ///< Bitfields of CCM Configuration Register
    UINT32    DisAllMemCancel          : 1; ///< Disable MemCancel response.
    UINT32    DisRemoteMemCancel       : 1; ///< Disable MemCancel to remote die.
    UINT32    DisPrbReqByp             : 1; ///< Disable Probe Request bypass path.
    UINT32    DisAlignReqBufOk         : 1; ///< Disable aligning request buffer checks.
    UINT32    DisCcmReqQByp            : 1; ///< Disable Request queue (REQQ) bypass path
    UINT32    DisCcmRspDQByp           : 1; ///< Disable Read Response Bypass.
    UINT32    EnEarlyPrbRspDataReturn  : 1; ///< Enable return of Probe Response dirty data to SDP without waiting for all system responses to be accumulated.
    UINT32    RspQRdRspWaterMark       : 6; ///< Number of responses waiting to be scheduled to the data scheduler before the bypass path is disabled until the count falls below this threshold watermark.
    UINT32    FrcAnyVCPassPW           : 1; ///< Enable PassPW bit always set.
    UINT32    FrcAnyVCRspPassPW        : 1; ///< Enable Response PassPW always set
    UINT32    ForceCacheBlkRdZeroOrder : 1; ///< Force all cache block commands to fetch data in Zero word order.
    UINT32    DsPReqLmt                : 6; ///< Number of downstream Posted requests issued by CCM which may be outstanding on IO links.
    UINT32    DisSpecDramRead          : 1; ///< Disable Speculative Dram Reads to Remote Die.
    UINT32                             : 1; ///< Disable high priority for Fast TPR writes
    UINT32    ForceChgToXReturnDat     : 1; ///< Forces ChgToX to always return Data to CCX
    UINT32    ForceRdBlkLToC           : 1; ///< Force RdBlkL to be issued as RdBlk to FTI.
    UINT32    DisMstAbtRsp             : 1; ///< Disable Master Abort Response
    UINT32    DisPciCfgMstAbtRsp       : 1; ///< Disable PCI Config Master Abort Response
    UINT32    DisProtErrRsp            : 1; ///< Disable protocol violation on FTI Error Response
    UINT32    DisCcmRspDataByp         : 1; ///< Disable Response Data Bypass
    UINT32    EnCcmProbeCache          : 1; ///< Enable CCM probe cache function to get lower cache to cache transfer latency
    UINT32    EnCcmStash               : 1; ///< Enable CCM stashing function to handle stashing flow
  } Field;
  UINT32    Value;
} CCM_CFG_REGISTER;

/* CCM Configuration 1 Register */
#define CCM_CFG1_FUNC  0x3
#define CCM_CFG1_REG   0x108

/// CCM Configuration 1 Register
typedef union {
  struct {
    ///< Bitfields of CCM Configuration 1 Register
    UINT32    DisReqQLPOrLLPreEmptiveByp  : 1; ///< Disable Pre-emptive Bypass Path
    UINT32    DisReqQPickOutOfOrder       : 1; ///< Disable out-of-order request picking
    UINT32    DisRspQPickOutOfOrder       : 1; ///< Disable out-of-order response picking
    UINT32    DisPrbQPickOutOfOrder       : 1; ///< Disable out-of-order probe picking
    UINT32    ForceCohSzRdWr              : 1; ///< Force non-coherent commands as coherent versions
    UINT32                                : 1; ///< Reserved
    UINT32    EnExtPciGt8BTgtAbt          : 1; ///< Enable extended PCI accesses
    UINT32    DisReqQResAntiStrv          : 1; ///< Disable anti-starvation widget
    UINT32    DisFid0OrderMatch           : 1; ///< Treat all CS as separate targets
    UINT32    EnUnorderedWrSzFullZeroConv : 1; ///< Enable WrSizedFull with ReqBlockLevel = 0
    UINT32    PrbPickStallSdpEn           : 1; ///< PRBQ probe request picker is stalled
    UINT32    PrbPickStallFtiEn           : 1; ///< PRBQ probe response picker is stalled
    UINT32    DatPickStallSdpEn           : 1; ///< ORIGDQ picker is stalled
    UINT32    DatPickStallFtiEn           : 1; ///< RSPDQ picker is stalled
    UINT32    DatDeallocStallSdpEn        : 1; ///< ORIGDQ deallocation is stalled
    UINT32    DatDeallocStallFtiEn        : 1; ///< RSPDQ deallocation is stalled
    UINT32                                : 3; ///< Reserved
    UINT32    ForceRequestOrdering        : 1; ///< Force TgtStart/TgtDone ordering
    UINT32    AlarmClockCntSel            : 3; ///< CCM alarm clock count select
    UINT32    AlarmClockBaseSel           : 2; ///< CCM alarm clock timebase select
    UINT32    DisFullZeroConv             : 1; ///< Disable conversion
    UINT32    EnVicBlkClnDWaitData        : 1; ///< Enable Sdp VicBlkClnD request wait its related OrigData before issue to FTI.
    UINT32                                : 1; ///< Reserved
    UINT32    DisFtiParErr                : 1; ///< Disables parity error checking for FTI
    UINT32    DisRspdParErr               : 1; ///< Disables parity error checking for RSPD
    UINT32    DisSdpParErr                : 1; ///< Disables parity error checking for SPD
    UINT32    DisOrigdParErr              : 1; ///< Disables parity error checking for ORIGD
  } Field;
  UINT32    Value;
} CCM_CFG1_REGISTER;

/* CAKE Response Compression Control Register */
#define CAKE_RSP_COMP_CTRL_FUNC  0x3
#define CAKE_RSP_COMP_CTRL_REG   0x280

/* CAKE Request Compression Control Register */
#define CAKE_REQ_COMP_CTRL_FUNC  0x3
#define CAKE_REQ_COMP_CTRL_REG   0x284

/* CAKE System Configuration Register */
#define CAKE_SYS_CFG_FUNC  0x3
#define CAKE_SYS_CFG_REG   0x28C

/// CAKE System Configuration Register
typedef union {
  struct {
    ///< Bitfields of CAKE System Configuration Register
    UINT32    ConnDieId         : 4;        ///< Connected Die ID
    UINT32    ConnSktId         : 4;        ///< SktId connected to this CAKE (HMI or xHMI).
    UINT32    HmiMyType         : 2;        ///< Die Type of this cake's HMI link.
    UINT32    HmiConnType       : 2;        ///< HMI Connection Type
    UINT32    FtiToHmiDatBypEn  : 1;        ///< FTI to HMI Data Bypass Enable
    UINT32    HmiToFtiDatBypEn  : 1;        ///< HMI to FTI Data Bypass Enable
    UINT32    HmiPackFlushFull  : 1;        ///< HMI Pack Flush Full
    UINT32    CakeDisblTillRst  : 1;        ///< CAKE Disable Until Reset
    UINT32    PcsNopDis         : 1;        ///< Disable sending NOP to PCS
    UINT32    RouteChkDis       : 1;        ///< Force to disable routing check."
    UINT32    CakeHmi128        : 1;        ///< Fore the GOP bus only use 1:HMI128 link 0:256 link
    UINT32    LoneReqEn         : 1;        ///< "Enable LoneReq packet transfer. 1: Enable. 0: Disable.
    UINT32    ReqSrcSeqIdEnable : 1;        ///< Support SeqId in CAKE output FTI request
    UINT32    ReqTgtSeqIdEnable : 1;        ///< Support SeqId in CAKE input FTI request (the target ports).
    UINT32    SocketPresent     : 3;        ///< How many sockets are present. 0: 1 socket. 1: 2 sockets. ... 7: 8 sockets.
    UINT32    PrbMcastDieMask   : 4;        ///< Mcast probe McastMask[DieMask] change in CAKE.
    UINT32    SocketIdOffset    : 3;        ///< Only used for CPU-DCU connection.
  } Field;
  UINT32    Value;
} CAKE_SYS_CFG_REGISTER;

/* IOM Configuration 0 Register */
#define IOM_CFG_FUNC  0x3
#define IOM_CFG_REG   0x300

/// IOM Configuration 0 Register
typedef union {
  struct {
    ///< Bitfields of IOM Configuration 0 Register
    UINT32    DisPairedCsMtch         : 1; ///< Disable ordering of paired CS
    UINT32    DisBusLock              : 1; ///< Disable blocking commands for BusLock
    UINT32    EnReqBusLock            : 1; ///< Block all NP commands
    UINT32    DisDbgPickStallFtiRsp   : 1; ///< Disable DebugStall pick for Source Done
    UINT32    DisDbgPickStallSdpWrRsp : 1; ///< Disable DebugStall pick for SDP write
    UINT32    DisDbgPickStallSdpRdRsp : 1; ///< Disable DebugStall pick for SDP Read Response
    UINT32    SrcDnCsWrSzAnyPPW       : 1; ///< Generate SrcDone for SDP request
    UINT32    SrcDnCsWrSzAnyBL        : 1; ///< Generate SrcDone regardless of BlockLevel
    UINT32    DisSrcDnOnVc2ToVc7      : 1; ///< Disable SrcDone for SDP from VC2 to VC7
    UINT32    AlarmClockBaseSel       : 2; ///< IOM ordering AlarmClock timebase select
    UINT32    AlarmClockCntSel        : 3; ///< IOM ordering AlarmClock Count select
    UINT32    IomHeartBeatEn          : 1; ///< Configure IOM heartbeat monitoring
    UINT32    LoopBackIosRsp          : 1; ///< Loop IOS reponse indication
    UINT32    ForceCohSzRdWr          : 1; ///< Force probes to be issued
    UINT32    DisIoChnWrOrd           : 1; ///< Disable IOS destination requests order
    UINT32    FrcIosRspPriZero        : 1; ///< Force IOS reposnse to be low priority
    UINT32    DisTgtStartShortCut     : 1; ///< Disable Target Start shortcut
    UINT32    LatMonFtiPrbRsp         : 1; ///< Force latency monitor to measure probe response
    UINT32    LatMonTgtSel            : 3; ///< Select performance Latency Monitor Target
    UINT32    DisIosRspBypass         : 1; ///< Disable IOS response bypass path
    UINT32    EnAutoClrRspWtPriorOp   : 1; ///< Enable automatic clearing of response wait code
    UINT32    DevMsgIoChnWrOnly       : 1; ///< Allow device message requests to set chain bit
    UINT32    DisBug1599Fix           : 1; ///< Disable the SATB0 fix for IOM does not set the wait code
    UINT32    FrcIomOrdReqBL1Or0AsBL2 : 1; ///< Force to order the Blocklevel or 1 requests like the Blocklevel2 requests out of the request queue
    UINT32    DisMrq0BypCntUId        : 1; ///< Disable the request bottom entry bypass count to throttle the requests as Blocklevel2
    UINT32    DisMrq0BypCntPrt        : 1; ///< Disable the request bottom entry bypass count to throttle the requests as Blocklevel3
    UINT32    FrcIomOrdReqAsPPWZero   : 1; ///< Force to order SDP_VC0/1 requests as if PPW=0 out of the request queue
  } Field;
  UINT32    Value;
} IOM_CFG_REGISTER;

/* Core Coherent Master Configuration Access Control */
#define CORE_MASTER_ACC_CTRL_FUNC  0x4
#define CORE_MASTER_ACC_CTRL_REG   0x44

/// Core Coherent Master Configuration Access Control
typedef union {
  struct {
    ///< Bitfields of Core Coherent Master Configuration Access Control
    UINT32    EnableCf8ExtCfg : 1;      ///< Enable PCI extended configuration register
    UINT32    DisPciCfgReg    : 1;      ///< Disable CFC/CF8 Accesses to IO space
    UINT32                    : 30;     ///< Reserved
  } Field;
  UINT32    Value;
} CORE_MASTER_ACC_CTRL_REGISTER;

/* Fabric Configuration Access Control */
#define FCAC_FUNC  0x4
#define FCAC_REG   0x40

/// Fabric Configuration Access Control
typedef union {
  struct {
    ///< Bitfields of Fabric Configuration Access Control
    UINT32    CfgRegInstAccEn      : 1;  ///< CfgRegInstAccEn
    UINT32    CfgRegInstAccRegLock : 1;  ///< ABL FW set this bit to 1 to lock this register after setting up address maps,
                                         ///< routing tables, flow control, and all the fabric per-instance registers.
    UINT32                         : 14; ///<
    UINT32    CfgRegInstID         : 8;  ///< CfgRegInstID
    UINT32    CfgRegDieID          : 8;  ///< CfgRegDieID
  } Field;
  UINT32    Value;
} FABRIC_CONFIG_ACC_CTRL_REGISTER;

/* Fabric Indirect Config Access Address 3 Register */
#define FICAA3_FUNC  0x4
#define FICAA3_REG   0x05C

/// Fabric Indirect Config Access Address 3 Register
typedef union {
  struct {
    ///< Bitfields of Fabric Indirect Config Access Address 3 Register
    UINT32    CfgRegInstAccEn   : 1; ///< CfgRegInstAccEn
    UINT32                      : 1; ///< Reserved
    UINT32    IndCfgAccRegNum   : 9; ///< IndCfgAccRegNum
    UINT32    IndCfgAccFuncNum  : 3; ///< IndCfgAccFuncNum
    UINT32    SixtyFourBitRegEn : 1; ///< SixtyFourBitRegEn
    UINT32                      : 1; ///< Reserved
    UINT32    CfgRegInstID      : 8; ///< CfgRegInstID
    UINT32    CfgRegDieID       : 8; ///< CfgRegDieID
  } Field;                           ///<
  UINT32    Value;                   ///<
} FABRIC_IND_CFG_ACCESS_ADDR_REGISTER;

/* Fabric Indirect Config Access Data Low 3 Register */
#define FICAD3_LO_FUNC  0x4
#define FICAD3_LO_REG   0x98

/* Fabric Indirect Config Access Data High 3 Register */
#define FICAD3_HI_FUNC  0x4
#define FICAD3_HI_REG   0x9C

/* Master Request Control Register */
#define MASTER_REQ_CTRL_FUNC  0x4
#define MASTER_REQ_CTRL_REG   0x0C0

/// Master Request Control Register
typedef union {
  struct {
    ///< Bitfields of Master Request Control Register
    UINT32    EnPerDramChActReqLmt : 1; ///< Enable bypass saturation count
    UINT32                         : 3; ///< Reserved
    UINT32    MstDramActReqLmt     : 8; ///< Master to DRAM Active Request Limit
    UINT32    MstIoNpActReqLmt     : 6; ///< Master to IO Non-Posted Active Request Limit
    UINT32                         : 2; ///< Reserved
    UINT32    MstReqThrottle       : 5; ///< Throttle rate of the Master components
    UINT32                         : 1; ///< Reserved
    UINT32    MstIoReqThrottle     : 5; ///< IO request throttle rate
    UINT32                         : 1; ///< Reserved
  } Field;
  UINT32    Value;
} MASTER_REQ_CTRL_REGISTER;

/* Master Request Priority Register */
#define MASTER_REQ_PRI_FUNC  0x4
#define MASTER_REQ_PRI_REG   0x0C4

/// Master Request Priority Register
typedef union {
  struct {
    ///< Bitfields of Master Request Priority Register
    UINT32    UseSdpReqPri : 1;        ///< Use SDP request priority
    UINT32                 : 3;        ///< Reserved
    UINT32    DefRdReqPri  : 4;        ///< Default read request priority
    UINT32    DefWrReqPri  : 4;        ///< Default write request priority
    UINT32    LLRdReqPri   : 4;        ///< Light load read request priority
    UINT32    LPRdReqPri   : 4;        ///< Lone potato read request priority
    UINT32                 : 12;       ///< Reserved
  } Field;
  UINT32    Value;
} MASTER_REQ_PRI_REGISTER;

/* Master Light Load And Lone Potato Request Priority Threshold Register */
#define MASTER_LL_LP_REQ_PRI_THRESH_FUNC  0x4
#define MASTER_LL_LP_REQ_PRI_THRESH_REG   0x0C8

/// Master Light Load And Lone Potato Request Priority Threshold Register
typedef union {
  struct {
    ///< Bitfields of Master Light Load And Lone Potato Request Priority Threshold Register
    UINT32    LLElvPriReqThr    : 4;   ///< Light load elevated priority request threshold
    UINT32                      : 4;   ///< Reserved
    UINT32    LLCumElvPriReqThr : 6;   ///< Light load cumulative elevated priority request threshold
    UINT32                      : 2;   ///< Reserved
    UINT32    LPElvPriReqThr    : 3;   ///< Lone potato elevated priority request threshold
    UINT32                      : 1;   ///< Reserved
    UINT32    LPCumElvPriReqThr : 5;   ///< Lone potato cumulative elevated priority request threshold
    UINT32                      : 7;   ///< Reserved
  } Field;
  UINT32    Value;
} MASTER_LL_LP_REQ_PRI_THRESH_REGISTER;

/* PIEControl */
#define PIE_CTRL_FUNC                  0x5
#define PIE_CTRL_REG                   0x108
#define REMOTE_INTR_MODE_FORCE_XAPIC   2
#define REMOTE_INTR_MODE_FORCE_X2APIC  3

/// PIE Control Register
typedef union {
  struct {
    ///< Bitfields of Cstate Control Register
    UINT32    SMTEnable           : 1; ///< Reflects the SMT configuration status for the system
    UINT32    LintEn              : 1; ///< Enable conversion of ExtInt/NMI to LINT0/1.
    UINT32                        : 2;
    UINT32    CfgWrAckDly         : 6; ///< The number of cycles ACK (TgtDone) is delayed after config writes.
    UINT32                        : 2;
    UINT32    ArbIntPreferLclDie  : 4; ///< Specifies in 1/8 granularity the percentage of die level arbitrations which chooses the local die if possible.
    UINT32    PrqReqPickStallMask : 4; ///< This mask identifies which types of requests to stall when PRQ recieves a REQ Pick Stall.
    UINT32    ApicExtBrdCst       : 1; ///< Enable the extended APIC broadcast functionality.
    UINT32    ApicExtId           : 1; ///< Enable extended APIC ID size.
    UINT32    ApicExtSpur         : 1; ///< APIC extended spurious vector enable.
    UINT32    RmtIntrModeCtl      : 2; ///< Remote interrupts mode control, identified the type (xAPIC or x2APIC) of interrupts from IOM
    UINT32    GroupConfigEnable   : 1; ///< DF Grouping Config mode enable
    UINT32                        : 6; ///< Reserved
  } Field;
  UINT32    Value;
} PIE_CTRL_REGISTER;

/// SYS_CFG_CONVERT Register
#define SYS_CFG_CONVERT_FUNC       0x5
#define SYS_CFG_CONVERT_REG        0x180
typedef union {
  struct {
    ///< Bitfields of Background Register
    UINT32    LocalDiePresent : 16;    ///< Local Die Present
    UINT32    MyDieId         : 4;    
    UINT32    Reserved        : 12;   
  } Field;
  UINT32    Value;
} SYS_CFG_CONVERT_REGISTER;

/* Cstate Control Register */
#define CSTATE_CTRL_FUNC  0x5
#define CSTATE_CTRL_REG   0x300

/// Cstate Control Register
typedef union {
  struct {
    ///< Bitfields of Cstate Control Register
    UINT32    DfInCstate                  : 1; ///< DF in CState
    UINT32    AllClientsIdle              : 1; ///< All clients idle
    UINT32    AnyHmiDown                  : 1; ///< Any HMI down
    UINT32    DfRegsNotRestored           : 1; ///< DF registers not restored
    UINT32    PickLoadStepBusy            : 1; ///< Pick load step busy
    UINT32    DfCstateDisable             : 1; ///< DF CState disable
    UINT32    DfCstateEntryDisable        : 1; ///< DF CState entry disable
    UINT32    DfCstateAsyncWakeDisable    : 1; ///< DF CState asyncrhonous wake disable
    UINT32    DfCstateMmStutterOptEn      : 1; ///< DF CState MM stutter
    UINT32    DfCstateMmPwrGateOptEn      : 1; ///< DF CState MM power gate
    UINT32    DfCstateDceElptModeEn       : 1; ///< DF CState DCE eLPT mode
    UINT32    DfCstatePwrGateEn           : 1; ///< DF CState power gate
    UINT32    DfCstateClkPwrDnEn          : 1; ///< DF CState clock power down
    UINT32    DfCstateSelfRefrEn          : 1; ///< DF CState self refresh
    UINT32    DfCstateHmiPwrDnEn          : 1; ///< DF CState HMI power down
    UINT32    DfCstateXhmiPwrDnEn         : 1; ///< DF CState xHMI power down
    UINT32    PerDieDfCstateEn            : 1; ///< Per die DF CState
    UINT32    HmiLoadStepUseCstateHierLvl : 1; ///<
    UINT32    DfCstatePhysUpgradeDis      : 1;  ///<
    UINT32                                : 13; ///< Reserved
  } Field;
  UINT32    Value;
} CSTATE_CTRL_REGISTER;

/* Pstate S3/D3 Control Register */
#define PSTATE_S3D3_CTRL_FUNC  0x5
#define PSTATE_S3D3_CTRL_REG   0x304

/// Pstate S3/D3 Control Register
typedef union {
  struct {
    ///< Bitfields of Pstate S3/D3 Control Register
    UINT32    DfPstateChgBusy                    : 1; ///< DF Pstate change busy
    UINT32    DfS3EntryBusy                      : 1; ///< DF S3 entry busy
    UINT32    DfPstateChgReq                     : 1; ///< DF Pstate change request
    UINT32    ExpectDfPstateChg                  : 1; ///< Expect DF Pstate change
    UINT32    DfS3EntryReq                       : 1; ///< DF S3 entry request
    UINT32    NvdimmSelfRefrEn                   : 1; ///< NV DIMM self refresh enable
    UINT32    DfPstateChgDisableClkChg           : 1; ///< DF Pstate change disable clock change
    UINT32    DfPstateChgDisableQuiesce          : 1; ///< DF Pstate change disable quiesce
    UINT32    DfPstateChgDisableMstateChg        : 1; ///< DF Pstate change disable Mstate change
    UINT32    DfPstateChgDisableHmiChg           : 1; ///< DF Pstate change disable HMI change
    UINT32    DfPstateChgDisableXhmiChg          : 1; ///< DF Pstate change disable xHMI change
    UINT32    DfSaveStateStart                   : 1; ///< DF save state start
    UINT32    DfSaveStateDone                    : 1; ///< DF save state done
    UINT32    DfRestoreStateStart                : 1; ///< DF restore state start
    UINT32    DfRestoreStateDone                 : 1; ///< DF restore state done
    UINT32    RecalibrateE12gPhyPllOnDfPstateChg : 1; ///< Recal E12g phy PLL on DF Pstate change
    UINT32    DfPstateUmcSdpDisconDis            : 1; ///
    UINT32    DfPstateIgnoreUmcCompClkAck        : 1; ///
    UINT32                                       : 14; ///< Reserved
  } Field;
  UINT32    Value;
} PSTATE_S3D3_CTRL_REGISTER;

/* Multi-Die Hysteresis Control Register */
#define MULTIDIE_HYST_CTRL_FUNC  0x5
#define MULTIDIE_HYST_CTRL_REG   0x354

/// Multi-Die Hysteresis Control Register
typedef union {
  struct {
    ///< Bitfields of Multi-Die Hysteresis Control Register
    UINT32    HmiDisconHyst           : 8; ///< HMI disconnect hysteresis
    UINT32    XhmiDisconHyst          : 8; ///< xHMI disconnect hysteresis
    UINT32    PerDieDfCstateHyst      : 8; ///< Per die DF Cstate hysteresis
    UINT32    PerDieDfCstateHystScale : 5; ///< Per die DF Cstate hysteresis scale
    UINT32                            : 3; ///< Reserved
  } Field;
  UINT32    Value;
} MULTIDIE_HYST_CTRL_REGISTER;

/* Load Step Hysteresis Control Register */
#define LOAD_STEP_HYST_CTRL_FUNC  0x5
#define LOAD_STEP_HYST_CTRL_REG   0x358

/// Load Step Hysteresis Control Register
typedef union {
  struct {
    ///< Bitfields of Load Step Hysteresis Control Register
    UINT32    ClkLoadStepHyst       : 6; ///< Clock load step hysteresis
    UINT32    ClkLoadStepHystScale  : 2; ///< Clock load step hysteresis scale
    UINT32    HmiLoadStepHyst       : 6; ///< HMI load step hysteresis
    UINT32    HmiLoadStepHystScale  : 2; ///< HMI load step hysteresis scale
    UINT32    PickLoadStepHyst      : 6; ///< Pick load step hysteresis
    UINT32    PickLoadStepHystScale : 2; ///< Pick load step hysteresis scale
    UINT32    PickLoadStepThrottle0 : 4; ///< Pick load step throttle 0
    UINT32    PickLoadStepThrottle1 : 4; ///< Pick load step throttle 1
  } Field;
  UINT32    Value;
} LOAD_STEP_HYST_CTRL_REGISTER;

/* DRAM Scrubber Base Address Register */
#define DRAMSCRUBBASEADDR_FUNC  0x6
#define DRAMSCRUBBASEADDR_REG   0x048

/// DRAM Scrubber Base Address Register
typedef union {
  struct {
    ///< Bitfields of DRAM Scrubber Base Address Register
    UINT32    DramScrubEn       : 1;   ///< DRAM Scrub Enable
    UINT32                      : 3;   ///< Reserved
    UINT32    DramScrubBaseAddr : 28;  ///< DRAM Scrub Base Address
  } Fields;
  UINT32    Value;
} DRAM_SCRUB_BASE_ADDR_REGISTER;

/* DRAM Scrubber Limit Address Register */
#define DRAMSCRUBLIMITADDR_FUNC  0x6
#define DRAMSCRUBLIMITADDR_REG   0x04C

/// DRAM Scrubber Limit Address Register
typedef union {
  struct {
    ///< Bitfields of DRAM Scrubber Limit Address Register
    UINT32    DramScrubRate      : 4;  ///< DRAM Scrub Rate
    UINT32    DramScrubLimitAddr : 28; ///< DRAM Scrub Limit Address
  } Fields;
  UINT32    Value;
} DRAM_SCRUB_LIMIT_ADDR_REGISTER;

/* DRAM Scrubber Address Low Register */
#define DRAMSCRUBADDRLO_FUNC  0x6
#define DRAMSCRUBADDRLO_REG   0x050

/// DRAM Scrubber Address Low Register
typedef union {
  struct {
    ///< Bitfields of DRAM Scrubber Address Low Register
    UINT32    MemInitEnable  : 1;      ///< MemInitEnable
    UINT32    MemInitDone    : 1;      ///< MemInitDone
    UINT32    DisMemInit     : 1;      ///< DisMemInit
    UINT32                   : 3;      ///< Reserved
    UINT32    SeqScrubAddrLo : 26;     ///< SeqScrubAddrLo
  } Fields;
  UINT32    Value;
} DRAM_SCRUB_ADDR_LO_REGISTER;

/* DRAM Scrubber Address High Register */
#define DRAMSCRUBADDRHI_FUNC  0x6
#define DRAMSCRUBADDRHI_REG   0x054

/// DRAM Scrubber Address High Register
typedef union {
  struct {
    ///< Bitfields of DRAM Scrubber Address High Register
    UINT32    SeqScrubAddrHi : 16;     ///< SeqScrubAddrHi
    UINT32                   : 16;     ///< Reserved
  } Fields;
  UINT32    Value;
} DRAM_SCRUB_ADDR_HI_REGISTER;

/* Redirect Scrubber Control Register */
#define REDIRECTSCRUBCTRL_FUNC  0x6
#define REDIRECTSCRUBCTRL_REG   0x058

/// DRAM Scrubber Address High Register
typedef union {
  struct {
    ///< Bitfields of DRAM Scrubber Address High Register
    UINT32    EnRedirScrub     : 1;    ///< Enable redirect scrub
    UINT32    RedirScrubReqLmt : 2;    ///< Redirect Scrub limits
    UINT32                     : 29;   ///< Reserved
  } Fields;
  UINT32    Value;
} REDIRECT_SCRUB_CTRL_REGISTER;

/* Trace Capture Buffer Control Register */
#define TCBCNTL_FUNC  0x6
#define TCBCNTL_REG   0x0DC

/// Trace Capture Buffer Control Register
typedef union {
  struct {
    ///< Bitfields of Trace Capture Buffer Control Register
    UINT32    TcbEn                  : 1; ///< Enable Trace Capture Buffer.
    UINT32    TraceToDramEn          : 1; ///< Enable Trace Capture Buffer Trace to DRAM mode.
    UINT32    AccTscMode             : 1; ///< Enable Accurate TSC Mode.
    UINT32    TraceInbDat            : 1; ///< This field determines which data channel is presented to Trace Capture Buffer for FrontEndD.
    UINT32    TraceInbPrb            : 1; ///< This field determines which data channel is presented to Trace Capture Buffer for FrontEndB.
    UINT32    TraceReqChanFullUnitId : 1; ///< This field determines whether to trace full UnitId or full Attr field for Request channel.
    UINT32    TraceDramAddrMapSel    : 5; ///< This field points to the AddrMap instance which is used to map the Trace Dram Address.
    UINT32    TscOverflowPktEn       : 1; ///< TSC overflow info packet
    UINT32    TscOverflowChanSel     : 2; ///< This field defines the Trace Capture Buffer Channel which is fed a TSC overflow in the event of an overflow.
    UINT32    TraceWbVcEn            : 1; ///< WriteBack VC trace control.
    UINT32    TraceQWSel             : 2; ///< Specify which part of the Trace Capture Buffer data value is stored with the data header.
    UINT32    TraceIosOutbDat        : 1; ///< Trace IOS Outbound Data channel
    UINT32    TraceIomOutbDat        : 1; ///< Trace IOM Outbound Data channel
    UINT32    TraceQosPri            : 2; ///< This field determines the priority with which the trace writes are issued over FTI.
    UINT32    TscAltReset            : 1; ///< TSC alternate reset value
    UINT32    TraceReqChanFullAddr   : 1; ///< Whether to trace full Addr or full ReqLen field for Request channel
    UINT32                           : 8; ///< Reserved
    UINT32    TcbCntlLock            : 1; ///< Trace Capture Buffer Control Register write lock.
  } Field;
  UINT32    Value;
} TCB_CNTL_REGISTER;

/* Hardware Assert Status Low Register */
#define HWA_STS_LOW_FUNC  0x6
#define HWA_STS_LOW_REG   0x1F0

/// Hardware Assert Status Low Register
typedef union {
  struct {
    ///< Bitfields of Hardware Assert Status Low Register
    UINT32    HWAssert0  : 1;          ///< Hardware Assert 0
    UINT32    HWAssert1  : 1;          ///< Hardware Assert 1
    UINT32    HWAssert2  : 1;          ///< Hardware Assert 2
    UINT32    HWAssert3  : 1;          ///< Hardware Assert 3
    UINT32    HWAssert4  : 1;          ///< Hardware Assert 4
    UINT32    HWAssert5  : 1;          ///< Hardware Assert 5
    UINT32    HWAssert6  : 1;          ///< Hardware Assert 6
    UINT32    HWAssert7  : 1;          ///< Hardware Assert 7
    UINT32    HWAssert8  : 1;          ///< Hardware Assert 8
    UINT32    HWAssert9  : 1;          ///< Hardware Assert 9
    UINT32    HWAssert10 : 1;          ///< Hardware Assert 10
    UINT32    HWAssert11 : 1;          ///< Hardware Assert 11
    UINT32    HWAssert12 : 1;          ///< Hardware Assert 12
    UINT32    HWAssert13 : 1;          ///< Hardware Assert 13
    UINT32    HWAssert14 : 1;          ///< Hardware Assert 14
    UINT32    HWAssert15 : 1;          ///< Hardware Assert 15
    UINT32    HWAssert16 : 1;          ///< Hardware Assert 16
    UINT32    HWAssert17 : 1;          ///< Hardware Assert 17
    UINT32    HWAssert18 : 1;          ///< Hardware Assert 18
    UINT32    HWAssert19 : 1;          ///< Hardware Assert 19
    UINT32    HWAssert20 : 1;          ///< Hardware Assert 20
    UINT32    HWAssert21 : 1;          ///< Hardware Assert 21
    UINT32    HWAssert22 : 1;          ///< Hardware Assert 22
    UINT32    HWAssert23 : 1;          ///< Hardware Assert 23
    UINT32    HWAssert24 : 1;          ///< Hardware Assert 24
    UINT32    HWAssert25 : 1;          ///< Hardware Assert 25
    UINT32    HWAssert26 : 1;          ///< Hardware Assert 26
    UINT32    HWAssert27 : 1;          ///< Hardware Assert 27
    UINT32    HWAssert28 : 1;          ///< Hardware Assert 28
    UINT32    HWAssert29 : 1;          ///< Hardware Assert 29
    UINT32    HWAssert30 : 1;          ///< Hardware Assert 30
    UINT32    HWAssert31 : 1;          ///< Hardware Assert 31
  } Field;
  UINT32    Value;
} HWA_STS_LOW_REGISTER;

/* Hardware Assert Status High Register */
#define HWA_STS_HI_FUNC  0x6
#define HWA_STS_HI_REG   0x1F4

/// Hardware Assert Status High Register
typedef union {
  struct {
    ///< Bitfields of Hardware Assert Status High Register
    UINT32    HWAssert0  : 1;          ///< Hardware Assert 0
    UINT32    HWAssert1  : 1;          ///< Hardware Assert 1
    UINT32    HWAssert2  : 1;          ///< Hardware Assert 2
    UINT32    HWAssert3  : 1;          ///< Hardware Assert 3
    UINT32    HWAssert4  : 1;          ///< Hardware Assert 4
    UINT32    HWAssert5  : 1;          ///< Hardware Assert 5
    UINT32    HWAssert6  : 1;          ///< Hardware Assert 6
    UINT32    HWAssert7  : 1;          ///< Hardware Assert 7
    UINT32    HWAssert8  : 1;          ///< Hardware Assert 8
    UINT32    HWAssert9  : 1;          ///< Hardware Assert 9
    UINT32    HWAssert10 : 1;          ///< Hardware Assert 10
    UINT32    HWAssert11 : 1;          ///< Hardware Assert 11
    UINT32    HWAssert12 : 1;          ///< Hardware Assert 12
    UINT32    HWAssert13 : 1;          ///< Hardware Assert 13
    UINT32    HWAssert14 : 1;          ///< Hardware Assert 14
    UINT32    HWAssert15 : 1;          ///< Hardware Assert 15
    UINT32    HWAssert16 : 1;          ///< Hardware Assert 16
    UINT32    HWAssert17 : 1;          ///< Hardware Assert 17
    UINT32    HWAssert18 : 1;          ///< Hardware Assert 18
    UINT32    HWAssert19 : 1;          ///< Hardware Assert 19
    UINT32    HWAssert20 : 1;          ///< Hardware Assert 20
    UINT32    HWAssert21 : 1;          ///< Hardware Assert 21
    UINT32    HWAssert22 : 1;          ///< Hardware Assert 22
    UINT32    HWAssert23 : 1;          ///< Hardware Assert 23
    UINT32    HWAssert24 : 1;          ///< Hardware Assert 24
    UINT32    HWAssert25 : 1;          ///< Hardware Assert 25
    UINT32    HWAssert26 : 1;          ///< Hardware Assert 26
    UINT32    HWAssert27 : 1;          ///< Hardware Assert 27
    UINT32    HWAssert28 : 1;          ///< Hardware Assert 28
    UINT32    HWAssert29 : 1;          ///< Hardware Assert 29
    UINT32    HWAssert30 : 1;          ///< Hardware Assert 30
    UINT32    HWAssert31 : 1;          ///< Hardware Assert 31
  } Field;
  UINT32    Value;
} HWA_STS_HI_REGISTER;

/* Hardware Assert Status Low Register */
#define HWA_MASK_LOW_FUNC  0x6
#define HWA_MASK_LOW_REG   0x1F8

/// Hardware Assert Mask Low Register
typedef union {
  struct {
    ///< Bitfields of Hardware Assert Mask Low Register
    UINT32    HWAssertMsk0  : 1;       ///< Hardware Assert Mask 0
    UINT32    HWAssertMsk1  : 1;       ///< Hardware Assert Mask 1
    UINT32    HWAssertMsk2  : 1;       ///< Hardware Assert Mask 2
    UINT32    HWAssertMsk3  : 1;       ///< Hardware Assert Mask 3
    UINT32    HWAssertMsk4  : 1;       ///< Hardware Assert Mask 4
    UINT32    HWAssertMsk5  : 1;       ///< Hardware Assert Mask 5
    UINT32    HWAssertMsk6  : 1;       ///< Hardware Assert Mask 6
    UINT32    HWAssertMsk7  : 1;       ///< Hardware Assert Mask 7
    UINT32    HWAssertMsk8  : 1;       ///< Hardware Assert Mask 8
    UINT32    HWAssertMsk9  : 1;       ///< Hardware Assert Mask 9
    UINT32    HWAssertMsk10 : 1;       ///< Hardware Assert Mask 10
    UINT32    HWAssertMsk11 : 1;       ///< Hardware Assert Mask 11
    UINT32    HWAssertMsk12 : 1;       ///< Hardware Assert Mask 12
    UINT32    HWAssertMsk13 : 1;       ///< Hardware Assert Mask 13
    UINT32    HWAssertMsk14 : 1;       ///< Hardware Assert Mask 14
    UINT32    HWAssertMsk15 : 1;       ///< Hardware Assert Mask 15
    UINT32    HWAssertMsk16 : 1;       ///< Hardware Assert Mask 16
    UINT32    HWAssertMsk17 : 1;       ///< Hardware Assert Mask 17
    UINT32    HWAssertMsk18 : 1;       ///< Hardware Assert Mask 18
    UINT32    HWAssertMsk19 : 1;       ///< Hardware Assert Mask 19
    UINT32    HWAssertMsk20 : 1;       ///< Hardware Assert Mask 20
    UINT32    HWAssertMsk21 : 1;       ///< Hardware Assert Mask 21
    UINT32    HWAssertMsk22 : 1;       ///< Hardware Assert Mask 22
    UINT32    HWAssertMsk23 : 1;       ///< Hardware Assert Mask 23
    UINT32    HWAssertMsk24 : 1;       ///< Hardware Assert Mask 24
    UINT32    HWAssertMsk25 : 1;       ///< Hardware Assert Mask 25
    UINT32    HWAssertMsk26 : 1;       ///< Hardware Assert Mask 26
    UINT32    HWAssertMsk27 : 1;       ///< Hardware Assert Mask 27
    UINT32    HWAssertMsk28 : 1;       ///< Hardware Assert Mask 28
    UINT32    HWAssertMsk29 : 1;       ///< Hardware Assert Mask 29
    UINT32    HWAssertMsk30 : 1;       ///< Hardware Assert Mask 30
    UINT32    HWAssertMsk31 : 1;       ///< Hardware Assert Mask 31
  } Field;
  UINT32    Value;
} HWA_MASK_LOW_REGISTER;

/* Hardware Assert Mask High Register */
#define HWA_MASK_HI_FUNC  0x6
#define HWA_MASK_HI_REG   0x1FC

/// Hardware Assert Mask High Register
typedef union {
  struct {
    ///< Bitfields of Hardware Assert Mask High Register
    UINT32    HWAssertMsk0  : 1;       ///< Hardware Assert Mask 0
    UINT32    HWAssertMsk1  : 1;       ///< Hardware Assert Mask 1
    UINT32    HWAssertMsk2  : 1;       ///< Hardware Assert Mask 2
    UINT32    HWAssertMsk3  : 1;       ///< Hardware Assert Mask 3
    UINT32    HWAssertMsk4  : 1;       ///< Hardware Assert Mask 4
    UINT32    HWAssertMsk5  : 1;       ///< Hardware Assert Mask 5
    UINT32    HWAssertMsk6  : 1;       ///< Hardware Assert Mask 6
    UINT32    HWAssertMsk7  : 1;       ///< Hardware Assert Mask 7
    UINT32    HWAssertMsk8  : 1;       ///< Hardware Assert Mask 8
    UINT32    HWAssertMsk9  : 1;       ///< Hardware Assert Mask 9
    UINT32    HWAssertMsk10 : 1;       ///< Hardware Assert Mask 10
    UINT32    HWAssertMsk11 : 1;       ///< Hardware Assert Mask 11
    UINT32    HWAssertMsk12 : 1;       ///< Hardware Assert Mask 12
    UINT32    HWAssertMsk13 : 1;       ///< Hardware Assert Mask 13
    UINT32    HWAssertMsk14 : 1;       ///< Hardware Assert Mask 14
    UINT32    HWAssertMsk15 : 1;       ///< Hardware Assert Mask 15
    UINT32    HWAssertMsk16 : 1;       ///< Hardware Assert Mask 16
    UINT32    HWAssertMsk17 : 1;       ///< Hardware Assert Mask 17
    UINT32    HWAssertMsk18 : 1;       ///< Hardware Assert Mask 18
    UINT32    HWAssertMsk19 : 1;       ///< Hardware Assert Mask 19
    UINT32    HWAssertMsk20 : 1;       ///< Hardware Assert Mask 20
    UINT32    HWAssertMsk21 : 1;       ///< Hardware Assert Mask 21
    UINT32    HWAssertMsk22 : 1;       ///< Hardware Assert Mask 22
    UINT32    HWAssertMsk23 : 1;       ///< Hardware Assert Mask 23
    UINT32    HWAssertMsk24 : 1;       ///< Hardware Assert Mask 24
    UINT32    HWAssertMsk25 : 1;       ///< Hardware Assert Mask 25
    UINT32    HWAssertMsk26 : 1;       ///< Hardware Assert Mask 26
    UINT32    HWAssertMsk27 : 1;       ///< Hardware Assert Mask 27
    UINT32    HWAssertMsk28 : 1;       ///< Hardware Assert Mask 28
    UINT32    HWAssertMsk29 : 1;       ///< Hardware Assert Mask 29
    UINT32    HWAssertMsk30 : 1;       ///< Hardware Assert Mask 30
    UINT32    HWAssertMsk31 : 1;       ///< Hardware Assert Mask 31
  } Field;
  UINT32    Value;
} HWA_MASK_HI_REGISTER;

#pragma pack (pop)
#endif /* _FABRIC_REGISTERS_ST_H_ */
