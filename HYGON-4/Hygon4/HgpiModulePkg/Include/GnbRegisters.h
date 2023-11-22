/* $NoKeywords:$ */

/**
 * @file
 *
 * HGPI gnb file
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  GNB
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

#ifndef _GNB_REGISTERS_H_
#define _GNB_REGISTERS_H_

//HyEx SMN Aperture ID define
#define NBIO0_IOHC_APERTURE_ID_HYEX              0x13B
#define NBIO1_IOHC_APERTURE_ID_HYEX              0x13C
#define NBIO2_IOHC_APERTURE_ID_HYEX              0x13D
#define NBIO3_IOHC_APERTURE_ID_HYEX              0x13E

#define NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX        0x13F
#define NBIO1_L2IMU0_L2B_APERTURE_ID_HYEX        0x140
#define NBIO2_L2IMU0_L2B_APERTURE_ID_HYEX        0x141
#define NBIO3_L2IMU0_L2B_APERTURE_ID_HYEX        0x142

#define NBIO0_IOAPIC_APERTURE_ID_HYEX            0x143
#define NBIO1_IOAPIC_APERTURE_ID_HYEX            0x144
#define NBIO2_IOAPIC_APERTURE_ID_HYEX            0x145
#define NBIO3_IOAPIC_APERTURE_ID_HYEX            0x146

#define NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX       0x147
#define NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX       0x148
#define NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX       0x149
#define NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX       0x14A

#define NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX        0x157
#define NBIO1_L2IMU0_L2A_APERTURE_ID_HYEX        0x158
#define NBIO2_L2IMU0_L2A_APERTURE_ID_HYEX        0x159
#define NBIO3_L2IMU0_L2A_APERTURE_ID_HYEX        0x15A

#define NBIO0_L2IMU0_MMIO_APERTURE_ID_HYEX       0x024
#define NBIO1_L2IMU0_MMIO_APERTURE_ID_HYEX       0x025
#define NBIO2_L2IMU0_MMIO_APERTURE_ID_HYEX       0x026
#define NBIO3_L2IMU0_MMIO_APERTURE_ID_HYEX       0x027

#define NBIO0_PCIE0_APERTURE_ID_HYEX             0x111
#define NBIO1_PCIE0_APERTURE_ID_HYEX             0x112
#define NBIO2_PCIE0_APERTURE_ID_HYEX             0x113
#define NBIO3_PCIE0_APERTURE_ID_HYEX             0x114

#define NBIO0_PCIE1_APERTURE_ID_HYEX             0x115
#define NBIO1_PCIE1_APERTURE_ID_HYEX             0x116
#define NBIO2_PCIE1_APERTURE_ID_HYEX             0x117
#define NBIO3_PCIE1_APERTURE_ID_HYEX             0x118

#define NBIO0_PCIE2_APERTURE_ID_HYEX             0x179

#define NBIO0_NBIF0_APERTURE_ID_HYEX             0x101
#define NBIO1_NBIF0_APERTURE_ID_HYEX             0x102
#define NBIO2_NBIF0_APERTURE_ID_HYEX             0x103
#define NBIO3_NBIF0_APERTURE_ID_HYEX             0x104

#define NBIO0_NBIF1_APERTURE_ID_HYEX             0x105
#define NBIO1_NBIF1_APERTURE_ID_HYEX             0x106
#define NBIO2_NBIF1_APERTURE_ID_HYEX             0x107
#define NBIO3_NBIF1_APERTURE_ID_HYEX             0x108

#define NBIO0_NBIF2_APERTURE_ID_HYEX             0x109
#define NBIO1_NBIF2_APERTURE_ID_HYEX             0x10A
#define NBIO2_NBIF2_APERTURE_ID_HYEX             0x10B
#define NBIO3_NBIF2_APERTURE_ID_HYEX             0x10C

#define NBIO0_SYSHUB0_APERTURE_ID_HYEX           0x14
#define NBIO1_SYSHUB0_APERTURE_ID_HYEX           0x15
#define NBIO2_SYSHUB0_APERTURE_ID_HYEX           0x16
#define NBIO3_SYSHUB0_APERTURE_ID_HYEX           0x17
#define NBIO0_SYSHUB1_APERTURE_ID_HYEX           0x18
#define NBIO1_SYSHUB1_APERTURE_ID_HYEX           0x19
#define NBIO2_SYSHUB1_APERTURE_ID_HYEX           0x1A
#define NBIO3_SYSHUB1_APERTURE_ID_HYEX           0x1B
#define NBIO0_NTB_APERTURE_ID_HYEX               0x40
#define NBIO1_NTB_APERTURE_ID_HYEX               0x41
#define NBIO2_NTB_APERTURE_ID_HYEX               0x42
#define NBIO3_NTB_APERTURE_ID_HYEX               0x43
#define NBIO0_SST_FCH_APERTURE_ID_HYEX           0x171
#define NBIO1_SST_FCH_APERTURE_ID_HYEX           0x172
#define NBIO2_SST_FCH_APERTURE_ID_HYEX           0x173
#define NBIO3_SST_FCH_APERTURE_ID_HYEX           0x174

#define NBIO0_SST_NBIO_APERTURE_ID_HYEX          0x175
#define NBIO1_SST_NBIO_APERTURE_ID_HYEX          0x176
#define NBIO2_SST_NBIO_APERTURE_ID_HYEX          0x177
#define NBIO3_SST_NBIO_APERTURE_ID_HYEX          0x178

//HyGx SMN Aperture ID define
#define IOD0_SMN_BASE_HYEX                       0x00000000
#define IOD0_SMN_BASE_HYGX                       0x10000000
#define IOD1_SMN_BASE_HYGX                       0x20000000

#define NBIO0_IOHC_APERTURE_ID_HYGX              0x80
#define NBIO1_IOHC_APERTURE_ID_HYGX              0x81
#define NBIO2_IOHC_APERTURE_ID_HYGX              0x82
#define NBIO3_IOHC_APERTURE_ID_HYGX              0x83

#define NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX        0x84
#define NBIO1_L2IMU0_L2B_APERTURE_ID_HYGX        0x85
#define NBIO2_L2IMU0_L2B_APERTURE_ID_HYGX        0x86
#define NBIO3_L2IMU0_L2B_APERTURE_ID_HYGX        0x87

#define NBIO0_IOAPIC_APERTURE_ID_HYGX            0x88
#define NBIO1_IOAPIC_APERTURE_ID_HYGX            0x89
#define NBIO2_IOAPIC_APERTURE_ID_HYGX            0x8A
#define NBIO3_IOAPIC_APERTURE_ID_HYGX            0x8B

#define NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX       0x8C
#define NBIO0_L1IMU_PCIE1_APERTURE_ID_HYGX       0x90
#define NBIO0_L1IMU_NBIF0_APERTURE_ID_HYGX       0x94
#define NBIO0_L1IMU_IOAGR_APERTURE_ID_HYGX       0x98

#define NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX        0x9C
#define NBIO1_L2IMU0_L2A_APERTURE_ID_HYGX        0x9D
#define NBIO2_L2IMU0_L2A_APERTURE_ID_HYGX        0x9E
#define NBIO3_L2IMU0_L2A_APERTURE_ID_HYGX        0x9F

#define NBIO0_L2IMU0_MMIO_APERTURE_ID_HYGX       0x58
#define NBIO1_L2IMU0_MMIO_APERTURE_ID_HYGX       0x59
#define NBIO2_L2IMU0_MMIO_APERTURE_ID_HYGX       0x5A
#define NBIO3_L2IMU0_MMIO_APERTURE_ID_HYGX       0x5B

#define NBIO0_PCIE0_APERTURE_ID_HYGX             0x74
#define NBIO1_PCIE0_APERTURE_ID_HYGX             0x75
#define NBIO2_PCIE0_APERTURE_ID_HYGX             0x76
#define NBIO3_PCIE0_APERTURE_ID_HYGX             0x77

#define NBIO0_CXL_APERTURE_ID_HYGX               0x78
#define NBIO2_CXL_APERTURE_ID_HYGX               0x7A
#define NBIO0_PCIE2_APERTURE_ID_HYGX             0x7C

#define NBIO0_NBIF0_APERTURE_ID_HYGX             0x64
#define NBIO2_NBIF0_APERTURE_ID_HYGX             0x66
#define NBIO0_SYSHUB0_APERTURE_ID_HYGX           0x4C
#define NBIO2_SYSHUB0_APERTURE_ID_HYGX           0x4E
#define NBIO0_NBIF1_APERTURE_ID_HYGX             0x68
#define NBIO2_NBIF1_APERTURE_ID_HYGX             0x6A
#define NBIO0_NTB_APERTURE_ID_HYGX               0x60
#define NBIO2_NTB_APERTURE_ID_HYGX               0x62

#define NBIO0_NBIF2_APERTURE_ID_HYGX             0x6C
#define NBIO0_NBIF3_APERTURE_ID_HYGX             0x70
#define NBIO0_SYSHUB2_APERTURE_ID_HYGX           0x50
#define NBIO0_SYSHUB3_APERTURE_ID_HYGX           0x54

#define NBIO0_SST_NBIO_APERTURE_ID_HYGX          0xB0
#define NBIO1_SST_NBIO_APERTURE_ID_HYGX          0xB1
#define NBIO2_SST_NBIO_APERTURE_ID_HYGX          0xB2
#define NBIO3_SST_NBIO_APERTURE_ID_HYGX          0xB3

#define NBIO0_SST_FCH_APERTURE_ID_HYGX           0xB4
#define NBIO1_SST_FCH_APERTURE_ID_HYGX           0xB5
#define NBIO2_SST_FCH_APERTURE_ID_HYGX           0xB6
#define NBIO3_SST_FCH_APERTURE_ID_HYGX           0xB7
#define UNKNOW_APERTURE_ID                      0xFF
typedef enum {
  IOHC_REG_TYPE,
  IOAPIC_REG_TYPE,
  L1IMU_PCIE0_REG_TYPE,
  L1IMU_PCIE1_REG_TYPE,
  L1IMU_NBIF0_REG_TYPE,
  L1IMU_IOAGR_REG_TYPE,
  L2IMU0_L2A_REG_TYPE,
  L2IMU0_L2B_REG_TYPE,
  L2IMU0_MMIO_REG_TYPE,
  PCIE0_REG_TYPE,
  PCIE1_REG_TYPE,
  CXL_REG_TYPE,
  PCIE2_REG_TYPE,
  NBIF0_REG_TYPE,
  NBIF1_REG_TYPE,
  NBIF2_REG_TYPE,
  NBIF3_REG_TYPE,
  SYSHUB0_REG_TYPE,
  SYSHUB1_REG_TYPE,
  SYSHUB2_REG_TYPE,
  SYSHUB3_REG_TYPE,
  NTB_REG_TYPE,
  SST_NBIO_REG_TYPE,
  SST_FCH_REG_TYPE
} NBIO_REG_TYPE;

//HyEx and HyGx common logical bridge index
#define PCIE0_PORT0_LOGICALBRIDGE_INDEX         0x0
#define PCIE0_PORT1_LOGICALBRIDGE_INDEX         0x1
#define PCIE0_PORT2_LOGICALBRIDGE_INDEX         0x2
#define PCIE0_PORT3_LOGICALBRIDGE_INDEX         0x3
#define PCIE0_PORT4_LOGICALBRIDGE_INDEX         0x4
#define PCIE0_PORT5_LOGICALBRIDGE_INDEX         0x5
#define PCIE0_PORT6_LOGICALBRIDGE_INDEX         0x6
#define PCIE0_PORT7_LOGICALBRIDGE_INDEX         0x7
#define PCIE1_PORT0_LOGICALBRIDGE_INDEX         0x8

//HyEx Logical bridge index
#define PCIE1_PORT1_LOGICALBRIDGE_INDEX_HYEX     0x9
#define PCIE1_PORT2_LOGICALBRIDGE_INDEX_HYEX     0xA
#define PCIE1_PORT3_LOGICALBRIDGE_INDEX_HYEX     0xB
#define PCIE1_PORT4_LOGICALBRIDGE_INDEX_HYEX     0xC
#define PCIE1_PORT5_LOGICALBRIDGE_INDEX_HYEX     0xD
#define PCIE1_PORT6_LOGICALBRIDGE_INDEX_HYEX     0xE
#define PCIE1_PORT7_LOGICALBRIDGE_INDEX_HYEX     0xF
#define NBIF0_LOGICALBRIDGE_INDEX_HYEX           0x10
#define NBIF1_LOGICALBRIDGE_INDEX_HYEX           0x11
#define PCIE2_PORT0_LOGICALBRIDGE_INDEX_HYEX     0x12
#define PCIE2_PORT1_LOGICALBRIDGE_INDEX_HYEX     0x13

//HyGx Logical bridge index
#define NBIF0_LOGICALBRIDGE_INDEX_HYGX            0x9
#define NBIF2_LOGICALBRIDGE_INDEX_HYGX            0xA
#define NBIF3_LOGICALBRIDGE_INDEX_HYGX            0xB
#define PCIE2_PORT0_LOGICALBRIDGE_INDEX_HYGX      0xC
#define PCIE2_PORT1_LOGICALBRIDGE_INDEX_HYGX      0xD
#define PCIE2_PORT2_LOGICALBRIDGE_INDEX_HYGX      0xE
#define PCIE2_PORT3_LOGICALBRIDGE_INDEX_HYGX      0xF
#define PCIE2_PORT4_LOGICALBRIDGE_INDEX_HYGX      0x10
#define PCIE2_PORT5_LOGICALBRIDGE_INDEX_HYGX      0x11
#define PCIE2_PORT6_LOGICALBRIDGE_INDEX_HYGX      0x12
#define PCIE2_PORT7_LOGICALBRIDGE_INDEX_HYGX      0x13

#ifndef NBIO_SPACE
  #define  NBIO_SPACE(HANDLE, ADDRESS) \
             (UINT32)((HANDLE->DieType == IOD_SATORI) ? \
                      ((HANDLE->RbId << 20) + ADDRESS) : \
                      (IOD0_SMN_BASE_HYGX + (HANDLE->PhysicalDieId << 28) + (HANDLE->RbId << 20) + ADDRESS))
#endif

#ifndef PCIE_STRAP_SPACE
  #define  PCIE_STRAP_SPACE(HANDLE, ADDRESS) \
             (UINT32)((HANDLE->DieType == IOD_SATORI) ? \
                      ((HANDLE->RbId << 8) + ADDRESS) : \
                      (IOD0_SMN_BASE_HYGX + (HANDLE->PhysicalDieId << 28) + (HANDLE->RbId << 8) + ADDRESS))
#endif

#ifndef IOHC_BRIDGE_SPACE
  #define  IOHC_BRIDGE_SPACE(HANDLE, ENGINE, ADDRESS)   \
             (UINT32)((HANDLE->DieType == IOD_SATORI) ? \
                      (ADDRESS + (HANDLE->RbId << 20) + (ENGINE->Type.Port.LogicalBridgeId << 10)) : \
                      (IOD0_SMN_BASE_HYGX + (HANDLE->PhysicalDieId << 28) + ADDRESS + (HANDLE->RbId << 20) + (ENGINE->Type.Port.LogicalBridgeId << 10)))
#endif

#ifndef L1IMU_SPACE_HYEX
  #define  L1IMU_SPACE_HYEX(HANDLE, ADDRESS)   (ADDRESS + (HANDLE->RbId << 22))
#endif

#ifndef SDPMUX_SPACE_HYEX
  #define  SDPMUX_SPACE_HYEX(HANDLE, ADDRESS)   (HANDLE->RbId == 0? ADDRESS : (ADDRESS + 0x900000 + (HANDLE->RbId << 20)))
#endif

#define  TYPE_D0F0         0x1
#define  TYPE_D0F0x64      0x2
#define  TYPE_D0F0x98      0x3
#define  TYPE_D0F0xBC      0x4
#define  TYPE_PCI          TYPE_D0F0
#define  TYPE_SMN          TYPE_D0F0xBC
#define  TYPE_NBMISC       TYPE_SMN

// **** D0F0x04 Register Definition ****
// Address
#define NB_COMMAND_ADDRESS                                         0x4

// Type
#define NB_COMMAND_TYPE                                            TYPE_PCI
// Field Data
#define NB_COMMAND_IoAccessEn_OFFSET                               0
#define NB_COMMAND_IoAccessEn_WIDTH                                1
#define NB_COMMAND_IoAccessEn_MASK                                 0x1
#define NB_COMMAND_MemAccessEn_OFFSET                              1
#define NB_COMMAND_MemAccessEn_WIDTH                               1
#define NB_COMMAND_MemAccessEn_MASK                                0x2
#define NB_COMMAND_BusMasterEn_OFFSET                              2
#define NB_COMMAND_BusMasterEn_WIDTH                               1
#define NB_COMMAND_BusMasterEn_MASK                                0x4
#define NB_COMMAND_Reserved_31_3_OFFSET                            3
#define NB_COMMAND_Reserved_31_3_WIDTH                             29
#define NB_COMMAND_Reserved_31_3_MASK                              0xfffffff8

/// D0F0x04
typedef union {
  struct {                                                              ///<
    UINT32                                               IoAccessEn:1 ; ///<
    UINT32                                              MemAccessEn:1 ; ///<
    UINT32                                              BusMasterEn:1 ; ///<
    UINT32                                            Reserved_31_3:29; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<        
} NB_COMMAND_STRUCT;

// Address
#define NB_TOP_OF_DRAM_SLOT1_ADDRESS                               0x90

// Type
#define NB_TOP_OF_DRAM_SLOT1_TYPE                                  TYPE_PCI

#define NB_TOP_OF_DRAM_SLOT1_TOP_OF_DRAM_BIT_32_OFFSET             0
#define NB_TOP_OF_DRAM_SLOT1_TOP_OF_DRAM_BIT_32_WIDTH              1
#define NB_TOP_OF_DRAM_SLOT1_TOP_OF_DRAM_BIT_32_MASK               0x1
//Reserved 22:1
#define NB_TOP_OF_DRAM_SLOT1_TOP_OF_DRAM_OFFSET                    23
#define NB_TOP_OF_DRAM_SLOT1_TOP_OF_DRAM_WIDTH                     9
#define NB_TOP_OF_DRAM_SLOT1_TOP_OF_DRAM_MASK                      0xff800000L

/// D0F0_090
typedef union {
  struct {
    UINT32    TOP_OF_DRAM_BIT_32 : 1;                                  ///<
    UINT32    Reserved_22_1      : 22;                                 ///<
    UINT32    TOP_OF_DRAM        : 9;                                  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NB_TOP_OF_DRAM_SLOT1_STRUCT;

// **** D0F0xB8 Register Definition ****
// Address
#ifndef D0F0xB8_ADDRESS
  #define D0F0xB8_ADDRESS                                          0xB8
#endif
// Type
#define D0F0xB8_TYPE                                               TYPE_PCI
// Field Data
#define D0F0xB8_NbSmuIndAddr_OFFSET                                0
#define D0F0xB8_NbSmuIndAddr_WIDTH                                 32
#define D0F0xB8_NbSmuIndAddr_MASK                                  0xffffffff

/// D0F0xB8
typedef union {
  struct {                                                              ///<
    UINT32                                             NbSmuIndAddr:32; ///<
  } Field;                                                              ///<
  UINT32 Value;                                                         ///<
} D0F0xB8_STRUCT;

// PCIE STRAP ACCESS REGISTERS
#define NBIO0_PCIE0_STRAP_INDEX_ADDRESS  0x4A348
#define NBIO0_PCIE0_STRAP_DATA_ADDRESS   0x4A34C
#define NBIO0_PCIE1_STRAP_INDEX_ADDRESS  0x4A3C8
#define NBIO0_PCIE1_STRAP_DATA_ADDRESS   0x4A3CC
#define NBIO0_PCIE2_STRAP_INDEX_ADDRESS  0x4D748
#define NBIO0_PCIE2_STRAP_DATA_ADDRESS   0x4D74C

// Address
#define SDPMUX_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYEX                0x04400000
#define SDPMUX_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYGX                0x0B800000

#define SDPMUX_GLUE_CG_LCLK_CTRL_0_TYPE                        TYPE_SMN

#define NBMISC_4400_CG_OFF_HYSTERESIS_OFFSET                   4
#define NBMISC_4400_CG_OFF_HYSTERESIS_WIDTH                    8
#define NBMISC_4400_CG_OFF_HYSTERESIS_MASK                     0xff0
#define NBMISC_4400_SOFT_OVERRIDE_CLK9_OFFSET                  22
#define NBMISC_4400_SOFT_OVERRIDE_CLK9_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK9_MASK                    0x400000
#define NBMISC_4400_SOFT_OVERRIDE_CLK8_OFFSET                  23
#define NBMISC_4400_SOFT_OVERRIDE_CLK8_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK8_MASK                    0x800000
#define NBMISC_4400_SOFT_OVERRIDE_CLK7_OFFSET                  24
#define NBMISC_4400_SOFT_OVERRIDE_CLK7_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK7_MASK                    0x1000000
#define NBMISC_4400_SOFT_OVERRIDE_CLK6_OFFSET                  25
#define NBMISC_4400_SOFT_OVERRIDE_CLK6_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK6_MASK                    0x2000000
#define NBMISC_4400_SOFT_OVERRIDE_CLK5_OFFSET                  26
#define NBMISC_4400_SOFT_OVERRIDE_CLK5_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK5_MASK                    0x4000000
#define NBMISC_4400_SOFT_OVERRIDE_CLK4_OFFSET                  27
#define NBMISC_4400_SOFT_OVERRIDE_CLK4_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK4_MASK                    0x8000000
#define NBMISC_4400_SOFT_OVERRIDE_CLK3_OFFSET                  28
#define NBMISC_4400_SOFT_OVERRIDE_CLK3_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK3_MASK                    0x10000000
#define NBMISC_4400_SOFT_OVERRIDE_CLK2_OFFSET                  29
#define NBMISC_4400_SOFT_OVERRIDE_CLK2_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK2_MASK                    0x20000000
#define NBMISC_4400_SOFT_OVERRIDE_CLK1_OFFSET                  30
#define NBMISC_4400_SOFT_OVERRIDE_CLK1_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK1_MASK                    0x40000000
#define NBMISC_4400_SOFT_OVERRIDE_CLK0_OFFSET                  31
#define NBMISC_4400_SOFT_OVERRIDE_CLK0_WIDTH                   1
#define NBMISC_4400_SOFT_OVERRIDE_CLK0_MASK                    0x80000000L

// Address
#define SDPMUX_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYEX                0x04400004
#define SDPMUX_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYGX                0x0B800004

#define SDPMUX_GLUE_CG_LCLK_CTRL_1_TYPE                        TYPE_SMN

#define NBMISC_4404_SOFT_OVERRIDE_CLK9_OFFSET                  22
#define NBMISC_4404_SOFT_OVERRIDE_CLK9_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK9_MASK                    0x400000
#define NBMISC_4404_SOFT_OVERRIDE_CLK8_OFFSET                  23
#define NBMISC_4404_SOFT_OVERRIDE_CLK8_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK8_MASK                    0x800000
#define NBMISC_4404_SOFT_OVERRIDE_CLK7_OFFSET                  24
#define NBMISC_4404_SOFT_OVERRIDE_CLK7_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK7_MASK                    0x1000000
#define NBMISC_4404_SOFT_OVERRIDE_CLK6_OFFSET                  25
#define NBMISC_4404_SOFT_OVERRIDE_CLK6_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK6_MASK                    0x2000000
#define NBMISC_4404_SOFT_OVERRIDE_CLK5_OFFSET                  26
#define NBMISC_4404_SOFT_OVERRIDE_CLK5_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK5_MASK                    0x4000000
#define NBMISC_4404_SOFT_OVERRIDE_CLK4_OFFSET                  27
#define NBMISC_4404_SOFT_OVERRIDE_CLK4_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK4_MASK                    0x8000000
#define NBMISC_4404_SOFT_OVERRIDE_CLK3_OFFSET                  28
#define NBMISC_4404_SOFT_OVERRIDE_CLK3_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK3_MASK                    0x10000000
#define NBMISC_4404_SOFT_OVERRIDE_CLK2_OFFSET                  29
#define NBMISC_4404_SOFT_OVERRIDE_CLK2_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK2_MASK                    0x20000000
#define NBMISC_4404_SOFT_OVERRIDE_CLK1_OFFSET                  30
#define NBMISC_4404_SOFT_OVERRIDE_CLK1_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK1_MASK                    0x40000000
#define NBMISC_4404_SOFT_OVERRIDE_CLK0_OFFSET                  31
#define NBMISC_4404_SOFT_OVERRIDE_CLK0_WIDTH                   1
#define NBMISC_4404_SOFT_OVERRIDE_CLK0_MASK                    0x80000000L

#define NTBPCIE_LOCATION_ADDRESS_HYEX     0x0440000c
#define NTBPCIE_LOCATION_ADDRESS_HYGX     0x0B80000c

// Type
#define NTBPCIE_LOCATION_TYPE             TYPE_SDPMUX

#define NTBPCIE_LOCATION_Port_OFFSET     0
#define NTBPCIE_LOCATION_Port_WIDTH      16
#define NTBPCIE_LOCATION_Port_MASK       0xffff
#define NTBPCIE_LOCATION_Core_OFFSET     16
#define NTBPCIE_LOCATION_Core_WIDTH      16
#define NTBPCIE_LOCATION_Core_MASK       0xffff0000L

/// SDPMUX_0000000C
typedef union {
  struct {
    UINT32    Port : 16;                                               ///<
    UINT32    Core : 16;                                               ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NTBPCIE_LOCATION_STRUCT;

// Address
#define NTBPCIE_UNITID_ADDRESS_HYEX       0x04400010
#define NTBPCIE_UNITID_ADDRESS_HYGX       0x0B800010

// Type
#define NTBPCIE_UNITID_TYPE              TYPE_SMN

#define NTBPCIE_UnitID_OFFSET            0
#define NTBPCIE_UnitID_WIDTH             11
#define NTBPCIE_UnitID_MASK              0x7ff

/// SDPMUX_00000010
typedef union {
  struct {
    UINT32                              UnitID        : 11;    ///<HyEx only 9 bits
    UINT32                              Reserved_31_9 : 21;    ///<
  } Field;                                                     ///<
  UINT32    Value;                                             ///<
} NTBPCIE_UNITID_STRUCT;

// Register Name RCC_BIF_STRAP0

// Address
#define RCC_NBIF0_STRAP0_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x31400)
#define RCC_NBIF1_STRAP0_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x31400)
#define RCC_NBIF2_STRAP0_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x31400)

#define RCC_NBIF0_STRAP0_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x31400)
#define RCC_NBIF1_STRAP0_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x31400)
#define RCC_NBIF2_STRAP0_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x31400)
#define RCC_NBIF3_STRAP0_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x31400)

// Type
#define RCC_BIF_STRAP0_TYPE                                TYPE_SMN

#define RCC_BIF_STRAP0_GEN3_EN_PIN_OFFSET                   0
#define RCC_BIF_STRAP0_GEN3_EN_PIN_WIDTH                    1
#define RCC_BIF_STRAP0_GEN3_EN_PIN_MASK                     0x1
#define RCC_BIF_STRAP0_CLK_PM_EN_PIN_OFFSET                 1
#define RCC_BIF_STRAP0_CLK_PM_EN_PIN_WIDTH                  1
#define RCC_BIF_STRAP0_CLK_PM_EN_PIN_MASK                   0x2
#define RCC_BIF_STRAP0_VGA_DIS_PIN_OFFSET                   2
#define RCC_BIF_STRAP0_VGA_DIS_PIN_WIDTH                    1
#define RCC_BIF_STRAP0_VGA_DIS_PIN_MASK                     0x4
#define RCC_BIF_STRAP0_MEM_AP_SIZE_PIN_OFFSET               3
#define RCC_BIF_STRAP0_MEM_AP_SIZE_PIN_WIDTH                3
#define RCC_BIF_STRAP0_MEM_AP_SIZE_PIN_MASK                 0x38
#define RCC_BIF_STRAP0_BIOS_ROM_EN_PIN_OFFSET               6
#define RCC_BIF_STRAP0_BIOS_ROM_EN_PIN_WIDTH                1
#define RCC_BIF_STRAP0_BIOS_ROM_EN_PIN_MASK                 0x40
#define RCC_BIF_STRAP0_PX_CAPABLE_OFFSET                    7
#define RCC_BIF_STRAP0_PX_CAPABLE_WIDTH                     1
#define RCC_BIF_STRAP0_PX_CAPABLE_MASK                      0x80
#define RCC_BIF_STRAP0_BIF_KILL_GEN3_OFFSET                 8
#define RCC_BIF_STRAP0_BIF_KILL_GEN3_WIDTH                  1
#define RCC_BIF_STRAP0_BIF_KILL_GEN3_MASK                   0x100
#define RCC_BIF_STRAP0_MSI_FIRST_BE_FULL_PAYLOAD_EN_OFFSET  9
#define RCC_BIF_STRAP0_MSI_FIRST_BE_FULL_PAYLOAD_EN_WIDTH   1
#define RCC_BIF_STRAP0_MSI_FIRST_BE_FULL_PAYLOAD_EN_MASK    0x200
#define RCC_BIF_STRAP0_NBIF_IGNORE_ERR_INFLR_OFFSET         10
#define RCC_BIF_STRAP0_NBIF_IGNORE_ERR_INFLR_WIDTH          1
#define RCC_BIF_STRAP0_NBIF_IGNORE_ERR_INFLR_MASK           0x400
#define RCC_BIF_STRAP0_PME_SUPPORT_COMPLIANCE_EN_OFFSET     11
#define RCC_BIF_STRAP0_PME_SUPPORT_COMPLIANCE_EN_WIDTH      1
#define RCC_BIF_STRAP0_PME_SUPPORT_COMPLIANCE_EN_MASK       0x800
#define RCC_BIF_STRAP0_RX_IGNORE_EP_ERR_OFFSET              12
#define RCC_BIF_STRAP0_RX_IGNORE_EP_ERR_WIDTH               1
#define RCC_BIF_STRAP0_RX_IGNORE_EP_ERR_MASK                0x1000
#define RCC_BIF_STRAP0_RX_IGNORE_MSG_ERR_OFFSET             13
#define RCC_BIF_STRAP0_RX_IGNORE_MSG_ERR_WIDTH              1
#define RCC_BIF_STRAP0_RX_IGNORE_MSG_ERR_MASK               0x2000
#define RCC_BIF_STRAP0_RX_IGNORE_MAX_PAYLOAD_ERR_OFFSET     14
#define RCC_BIF_STRAP0_RX_IGNORE_MAX_PAYLOAD_ERR_WIDTH      1
#define RCC_BIF_STRAP0_RX_IGNORE_MAX_PAYLOAD_ERR_MASK       0x4000
#define RCC_BIF_STRAP0_RX_IGNORE_SHORTPREFIX_ERR_DN_OFFSET  15
#define RCC_BIF_STRAP0_RX_IGNORE_SHORTPREFIX_ERR_DN_WIDTH   1
#define RCC_BIF_STRAP0_RX_IGNORE_SHORTPREFIX_ERR_DN_MASK    0x8000
#define RCC_BIF_STRAP0_RX_IGNORE_TC_ERR_OFFSET              16
#define RCC_BIF_STRAP0_RX_IGNORE_TC_ERR_WIDTH               1
#define RCC_BIF_STRAP0_RX_IGNORE_TC_ERR_MASK                0x10000
#define RCC_BIF_STRAP0_RX_IGNORE_TC_ERR_DN_OFFSET           17
#define RCC_BIF_STRAP0_RX_IGNORE_TC_ERR_DN_WIDTH            1
#define RCC_BIF_STRAP0_RX_IGNORE_TC_ERR_DN_MASK             0x20000
#define RCC_BIF_STRAP0_DN_DEVNUM_OFFSET                     18
#define RCC_BIF_STRAP0_DN_DEVNUM_WIDTH                      5
#define RCC_BIF_STRAP0_DN_DEVNUM_MASK                       0x7c0000
#define RCC_BIF_STRAP0_DN_FUNCID_OFFSET                     23
#define RCC_BIF_STRAP0_DN_FUNCID_WIDTH                      3
#define RCC_BIF_STRAP0_DN_FUNCID_MASK                       0x3800000
#define RCC_BIF_STRAP0_QUICKSIM_START_OFFSET                26
#define RCC_BIF_STRAP0_QUICKSIM_START_WIDTH                 1
#define RCC_BIF_STRAP0_QUICKSIM_START_MASK                  0x4000000
#define RCC_BIF_STRAP0_NO_RO_ENABLED_P2P_PASSING_OFFSET     27
#define RCC_BIF_STRAP0_NO_RO_ENABLED_P2P_PASSING_WIDTH      1
#define RCC_BIF_STRAP0_NO_RO_ENABLED_P2P_PASSING_MASK       0x8000000
#define RCC_BIF_STRAP0_GPUIOV_SEC_LVL_OVRD_EN_OFFSET        28
#define RCC_BIF_STRAP0_GPUIOV_SEC_LVL_OVRD_EN_WIDTH         1
#define RCC_BIF_STRAP0_GPUIOV_SEC_LVL_OVRD_EN_MASK          0x10000000
#define RCC_BIF_STRAP0_CFG0_RD_VF_BUSNUM_CHK_EN_OFFSET      29
#define RCC_BIF_STRAP0_CFG0_RD_VF_BUSNUM_CHK_EN_WIDTH       1
#define RCC_BIF_STRAP0_CFG0_RD_VF_BUSNUM_CHK_EN_MASK        0x20000000
#define RCC_BIF_STRAP0_BIGAPU_MODE_OFFSET                   30
#define RCC_BIF_STRAP0_BIGAPU_MODE_WIDTH                    1
#define RCC_BIF_STRAP0_BIGAPU_MODE_MASK                     0x40000000
#define RCC_BIF_STRAP0_LINK_DOWN_RESET_EN_OFFSET            31
#define RCC_BIF_STRAP0_LINK_DOWN_RESET_EN_WIDTH             1
#define RCC_BIF_STRAP0_LINK_DOWN_RESET_EN_MASK              0x80000000L

/// RCC_BIF_STRAP0
typedef union {
  struct {
    UINT32    STRAP_GEN3_EN_PIN                  : 1;                  ///<
    UINT32    STRAP_CLK_PM_EN_PIN                : 1;                  ///<
    UINT32    STRAP_VGA_DIS_PIN                  : 1;                  ///<
    UINT32    STRAP_MEM_AP_SIZE_PIN              : 3;                  ///<
    UINT32    STRAP_BIOS_ROM_EN_PIN              : 1;                  ///<
    UINT32    STRAP_PX_CAPABLE                   : 1;                  ///<
    UINT32    STRAP_BIF_KILL_GEN3                : 1;                  ///<
    UINT32    STRAP_MSI_FIRST_BE_FULL_PAYLOAD_EN : 1;                  ///<
    UINT32    STRAP_NBIF_IGNORE_ERR_INFLR        : 1;                  ///<
    UINT32    STRAP_PME_SUPPORT_COMPLIANCE_EN    : 1;                  ///<
    UINT32    STRAP_RX_IGNORE_EP_ERR             : 1;                  ///<
    UINT32    STRAP_RX_IGNORE_MSG_ERR            : 1;                  ///<
    UINT32    STRAP_RX_IGNORE_MAX_PAYLOAD_ERR    : 1;                  ///<
    UINT32    STRAP_RX_IGNORE_SHORTPREFIX_ERR_DN : 1;                  ///<
    UINT32    STRAP_RX_IGNORE_TC_ERR             : 1;                  ///<
    UINT32    STRAP_RX_IGNORE_TC_ERR_DN          : 1;                  ///<
    UINT32    STRAP_DN_DEVNUM                    : 5;                  ///<
    UINT32    STRAP_DN_FUNCID                    : 3;                  ///<
    UINT32    STRAP_QUICKSIM_START               : 1;                  ///<
    UINT32    STRAP_NO_RO_ENABLED_P2P_PASSING    : 1;                  ///<
    UINT32    STRAP_GPUIOV_SEC_LVL_OVRD_EN       : 1;                  ///<
    UINT32    STRAP_CFG0_RD_VF_BUSNUM_CHK_EN     : 1;                  ///<
    UINT32    STRAP_BIGAPU_MODE                  : 1;                  ///<
    UINT32    STRAP_LINK_DOWN_RESET_EN           : 1;                  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} RCC_BIF_STRAP0_STRUCT;

// Register Name RCC_BIF_STRAP1

// Address
#define RCC_NBIF0_STRAP1_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x31404)
#define RCC_NBIF1_STRAP1_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x31404)
#define RCC_NBIF2_STRAP1_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x31404)

#define RCC_NBIF0_STRAP1_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x31404)
#define RCC_NBIF1_STRAP1_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x31404)
#define RCC_NBIF2_STRAP1_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x31404)
#define RCC_NBIF3_STRAP1_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x31404)

// Type
#define RCC_BIF_STRAP1_TYPE                                        TYPE_SMN

#define RCC_BIF_STRAP1_FUSESTRAP_VALID_OFFSET                            0
#define RCC_BIF_STRAP1_FUSESTRAP_VALID_WIDTH                             1
#define RCC_BIF_STRAP1_FUSESTRAP_VALID_MASK                            0x1
#define RCC_BIF_STRAP1_ROMSTRAP_VALID_OFFSET                             1
#define RCC_BIF_STRAP1_ROMSTRAP_VALID_WIDTH                              1
#define RCC_BIF_STRAP1_ROMSTRAP_VALID_MASK                             0x2
#define RCC_BIF_STRAP1_WRITE_DISABLE_OFFSET                              2
#define RCC_BIF_STRAP1_WRITE_DISABLE_WIDTH                               1
#define RCC_BIF_STRAP1_WRITE_DISABLE_MASK                              0x4
#define RCC_BIF_STRAP1_STRAP_ECRC_INTERMEDIATE_CHK_EN_OFFSET             3
#define RCC_BIF_STRAP1_STRAP_ECRC_INTERMEDIATE_CHK_EN_WIDTH              1
#define RCC_BIF_STRAP1_STRAP_ECRC_INTERMEDIATE_CHK_EN_MASK             0x8
#define RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_OFFSET                    4
#define RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_WIDTH                     1
#define RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_MASK                   0x10
#define RCC_BIF_STRAP1_STRAP_IGNORE_E2E_PREFIX_UR_SWUS_OFFSET            5
#define RCC_BIF_STRAP1_STRAP_IGNORE_E2E_PREFIX_UR_SWUS_WIDTH             3
#define RCC_BIF_STRAP1_STRAP_IGNORE_E2E_PREFIX_UR_SWUS_MASK           0xE0
#define RCC_BIF_STRAP1_STRAP_SWUS_APER_EN_OFFSET                         8
#define RCC_BIF_STRAP1_STRAP_SWUS_APER_EN_WIDTH                          1
#define RCC_BIF_STRAP1_STRAP_SWUS_APER_EN_MASK                       0x100
#define RCC_BIF_STRAP1_STRAP_SWUS_64BAR_EN_OFFSET                        9
#define RCC_BIF_STRAP1_STRAP_SWUS_64BAR_EN_WIDTH                         1
#define RCC_BIF_STRAP1_STRAP_SWUS_64BAR_EN_MASK                      0x200
#define RCC_BIF_STRAP1_STRAP_SWUS_AP_SIZE_OFFSET                        10
#define RCC_BIF_STRAP1_STRAP_SWUS_AP_SIZE_WIDTH                          2
#define RCC_BIF_STRAP1_STRAP_SWUS_AP_SIZE_MASK                       0xC00
#define RCC_BIF_STRAP1_STRAP_SWUS_APER_PREFETCHABLE_OFFSET              12
#define RCC_BIF_STRAP1_STRAP_SWUS_APER_PREFETCHABLE_WIDTH                1
#define RCC_BIF_STRAP1_STRAP_SWUS_APER_PREFETCHABLE_MASK            0x1000
#define RCC_BIF_STRAP1_STRAP_HWREV_LSB2_OFFSET                          13
#define RCC_BIF_STRAP1_STRAP_HWREV_LSB2_WIDTH                            2
#define RCC_BIF_STRAP1_STRAP_HWREV_LSB2_MASK                        0x6000
#define RCC_BIF_STRAP1_STRAP_SWREV_LSB2_OFFSET                          15
#define RCC_BIF_STRAP1_STRAP_SWREV_LSB2_WIDTH                            2
#define RCC_BIF_STRAP1_STRAP_SWREV_LSB2_MASK                       0x18000
#define RCC_BIF_STRAP1_STRAP_LINK_RST_CFG_ONLY_OFFSET                   17
#define RCC_BIF_STRAP1_STRAP_LINK_RST_CFG_ONLY_WIDTH                     1
#define RCC_BIF_STRAP1_STRAP_LINK_RST_CFG_ONLY_MASK                0x20000
#define RCC_BIF_STRAP1_STRAP_BIF_IOV_LKRST_DIS_OFFSET                   18
#define RCC_BIF_STRAP1_STRAP_BIF_IOV_LKRST_DIS_WIDTH                     1
#define RCC_BIF_STRAP1_STRAP_BIF_IOV_LKRST_DIS_MASK                0x40000
#define RCC_BIF_STRAP1_STRAP_GPUIOV_SEC_LVL_OVRD_VAL_OFFSET             19
#define RCC_BIF_STRAP1_STRAP_GPUIOV_SEC_LVL_OVRD_VAL_WIDTH               3
#define RCC_BIF_STRAP1_STRAP_GPUIOV_SEC_LVL_OVRD_VAL_MASK         0x380000
#define RCC_BIF_STRAP1_STRAP_BIF_PSN_UR_RPT_EN_OFFSET                   22
#define RCC_BIF_STRAP1_STRAP_BIF_PSN_UR_RPT_EN_WIDTH                     1
#define RCC_BIF_STRAP1_STRAP_BIF_PSN_UR_RPT_EN_MASK               0x400000
#define RCC_BIF_STRAP1_STRAP_BIF_XSTCLK_SWITCH_OVERRIDE_OFFSET          23
#define RCC_BIF_STRAP1_STRAP_BIF_XSTCLK_SWITCH_OVERRIDE_WIDTH            1
#define RCC_BIF_STRAP1_STRAP_BIF_XSTCLK_SWITCH_OVERRIDE_MASK      0x800000
#define RCC_BIF_STRAP1_STRAP_RP_BUSNUM_OFFSET                           24
#define RCC_BIF_STRAP1_STRAP_RP_BUSNUM_WIDTH                             8
#define RCC_BIF_STRAP1_STRAP_RP_BUSNUM_MASK                     0xFF000000

/// RCC_BIF_STRAP1
typedef union {
  struct {
    UINT32                                              FUSESTRAP_VALID:1 ; ///<
    UINT32                                               ROMSTRAP_VALID:1 ; ///<
    UINT32                                                WRITE_DISABLE:1 ; ///<
    UINT32                               STRAP_ECRC_INTERMEDIATE_CHK_EN:1 ; ///<
    UINT32                                      STRAP_TRUE_PM_STATUS_EN:1 ; ///<
    UINT32                              STRAP_IGNORE_E2E_PREFIX_UR_SWUS:3 ; ///<
    UINT32                                           STRAP_SWUS_APER_EN:1 ; ///<
    UINT32                                          STRAP_SWUS_64BAR_EN:1 ; ///<
    UINT32                                           STRAP_SWUS_AP_SIZE:2 ; ///<
    UINT32                                 STRAP_SWUS_APER_PREFETCHABLE:1 ; ///<
    UINT32                                             STRAP_HWREV_LSB2:2 ; ///<
    UINT32                                             STRAP_SWREV_LSB2:2 ; ///<
    UINT32                                      STRAP_LINK_RST_CFG_ONLY:1 ; ///<
    UINT32                                      STRAP_BIF_IOV_LKRST_DIS:1 ; ///<
    UINT32                                STRAP_GPUIOV_SEC_LVL_OVRD_VAL:3 ; ///<
    UINT32                                      STRAP_BIF_PSN_UR_RPT_EN:1 ; ///<
    UINT32                             STRAP_BIF_XSTCLK_SWITCH_OVERRIDE:1 ; ///<
    UINT32                                              STRAP_RP_BUSNUM:8 ; ///<
  } Field;                                                                  ///<
  UINT32 Value;                                                             ///<
} RCC_BIF_STRAP1_STRUCT;

// Register Name BIFC_MISC_CTRL0_

// Address
#define NBIF0_BIFC_MISC_CTRL0_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x3A010)
#define NBIF1_BIFC_MISC_CTRL0_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x3A010)
#define NBIF2_BIFC_MISC_CTRL0_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x3A010)

#define NBIF0_BIFC_MISC_CTRL0_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x3A010)
#define NBIF1_BIFC_MISC_CTRL0_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x3A010)
#define NBIF2_BIFC_MISC_CTRL0_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x3A010)
#define NBIF3_BIFC_MISC_CTRL0_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x3A010)

// Type
#define BIFC_MISC_CTRL0_TYPE                               TYPE_SMN

#define BIFC_MISC_CTRL0_VWIRE_TARG_UNITID_CHECK_EN_OFFSET  0
#define BIFC_MISC_CTRL0_VWIRE_TARG_UNITID_CHECK_EN_WIDTH   1
#define BIFC_MISC_CTRL0_VWIRE_TARG_UNITID_CHECK_EN_MASK    0x1
#define BIFC_MISC_CTRL0_VWIRE_SRC_UNITID_CHECK_EN_OFFSET   1
#define BIFC_MISC_CTRL0_VWIRE_SRC_UNITID_CHECK_EN_WIDTH    2
#define BIFC_MISC_CTRL0_VWIRE_SRC_UNITID_CHECK_EN_MASK     0x6
#define BIFC_MISC_CTRL0_Reserved_7_3_OFFSET                3
#define BIFC_MISC_CTRL0_Reserved_7_3_WIDTH                 5
#define BIFC_MISC_CTRL0_Reserved_7_3_MASK                  0xf8
#define BIFC_MISC_CTRL0_DMA_CHAIN_BREAK_IN_RCMODE_OFFSET   8
#define BIFC_MISC_CTRL0_DMA_CHAIN_BREAK_IN_RCMODE_WIDTH    1
#define BIFC_MISC_CTRL0_DMA_CHAIN_BREAK_IN_RCMODE_MASK     0x100
#define BIFC_MISC_CTRL0_HST_ARB_CHAIN_LOCK_OFFSET          9
#define BIFC_MISC_CTRL0_HST_ARB_CHAIN_LOCK_WIDTH           1
#define BIFC_MISC_CTRL0_HST_ARB_CHAIN_LOCK_MASK            0x200
#define BIFC_MISC_CTRL0_GSI_SST_ARB_CHAIN_LOCK_OFFSET      10
#define BIFC_MISC_CTRL0_GSI_SST_ARB_CHAIN_LOCK_WIDTH       1
#define BIFC_MISC_CTRL0_GSI_SST_ARB_CHAIN_LOCK_MASK        0x400
#define BIFC_MISC_CTRL0_Reserved_15_11_OFFSET              11
#define BIFC_MISC_CTRL0_Reserved_15_11_WIDTH               5
#define BIFC_MISC_CTRL0_Reserved_15_11_MASK                0xf800
#define BIFC_MISC_CTRL0_DMA_ATOMIC_LENGTH_CHK_DIS_OFFSET   16
#define BIFC_MISC_CTRL0_DMA_ATOMIC_LENGTH_CHK_DIS_WIDTH    1
#define BIFC_MISC_CTRL0_DMA_ATOMIC_LENGTH_CHK_DIS_MASK     0x10000
#define BIFC_MISC_CTRL0_DMA_ATOMIC_FAILED_STS_SEL_OFFSET   17
#define BIFC_MISC_CTRL0_DMA_ATOMIC_FAILED_STS_SEL_WIDTH    1
#define BIFC_MISC_CTRL0_DMA_ATOMIC_FAILED_STS_SEL_MASK     0x20000
#define BIFC_MISC_CTRL0_Reserved_23_18_OFFSET              18
#define BIFC_MISC_CTRL0_Reserved_23_18_WIDTH               6
#define BIFC_MISC_CTRL0_Reserved_23_18_MASK                0xfc0000
#define BIFC_MISC_CTRL0_PCIE_CAPABILITY_PROT_DIS_OFFSET    24
#define BIFC_MISC_CTRL0_PCIE_CAPABILITY_PROT_DIS_WIDTH     1
#define BIFC_MISC_CTRL0_PCIE_CAPABILITY_PROT_DIS_MASK      0x1000000
#define BIFC_MISC_CTRL0_VC7_DMA_IOCFG_DIS_OFFSET           25
#define BIFC_MISC_CTRL0_VC7_DMA_IOCFG_DIS_WIDTH            1
#define BIFC_MISC_CTRL0_VC7_DMA_IOCFG_DIS_MASK             0x2000000
#define BIFC_MISC_CTRL0_DMA_2ND_REQ_DIS_OFFSET             26
#define BIFC_MISC_CTRL0_DMA_2ND_REQ_DIS_WIDTH              1
#define BIFC_MISC_CTRL0_DMA_2ND_REQ_DIS_MASK               0x4000000
#define BIFC_MISC_CTRL0_PORT_DSTATE_BYPASS_MODE_OFFSET     27
#define BIFC_MISC_CTRL0_PORT_DSTATE_BYPASS_MODE_WIDTH      1
#define BIFC_MISC_CTRL0_PORT_DSTATE_BYPASS_MODE_MASK       0x8000000
#define BIFC_MISC_CTRL0_Reserved_30_28_OFFSET              28
#define BIFC_MISC_CTRL0_Reserved_30_28_WIDTH               3
#define BIFC_MISC_CTRL0_Reserved_30_28_MASK                0x70000000
#define BIFC_MISC_CTRL0_PCIESWUS_SELECTION_OFFSET          31
#define BIFC_MISC_CTRL0_PCIESWUS_SELECTION_WIDTH           1
#define BIFC_MISC_CTRL0_PCIESWUS_SELECTION_MASK            0x80000000L

/// NBIFMISC0_00000010
typedef union {
  struct {
    UINT32    VWIRE_TARG_UNITID_CHECK_EN : 1;                          ///<
    UINT32    VWIRE_SRC_UNITID_CHECK_EN  : 2;                          ///<
    UINT32    Reserved_7_3               : 5;                          ///<
    UINT32    DMA_CHAIN_BREAK_IN_RCMODE  : 1;                          ///<
    UINT32    HST_ARB_CHAIN_LOCK         : 1;                          ///<
    UINT32    GSI_SST_ARB_CHAIN_LOCK     : 1;                          ///<
    UINT32    Reserved_15_11             : 5;                          ///<
    UINT32    DMA_ATOMIC_LENGTH_CHK_DIS  : 1;                          ///<
    UINT32    DMA_ATOMIC_FAILED_STS_SEL  : 1;                          ///<
    UINT32    Reserved_23_18             : 6;                          ///<
    UINT32    PCIE_CAPABILITY_PROT_DIS   : 1;                          ///<
    UINT32    VC7_DMA_IOCFG_DIS          : 1;                          ///<
    UINT32    DMA_2ND_REQ_DIS            : 1;                          ///<
    UINT32    PORT_DSTATE_BYPASS_MODE    : 1;                          ///<
    UINT32    Reserved_30_28             : 3;                          ///<
    UINT32    PCIESWUS_SELECTION         : 1;                          ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} BIFC_MISC_CTRL0_STRUCT;

// Address
#define NBIF0_LINK_CNTL2_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x40094)
#define NBIF1_LINK_CNTL2_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x40094)
#define NBIF2_LINK_CNTL2_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x40094)

#define NBIF0_LINK_CNTL2_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x40094)
#define NBIF1_LINK_CNTL2_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x40094)
#define NBIF2_LINK_CNTL2_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x40094)
#define NBIF3_LINK_CNTL2_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x40094)

// Type
#define NBIF_LINK_CNTL2_TYPE                                TYPE_SMN

#define NBIF_LINK_CNTL2_TARGET_LINK_SPEED_OFFSET            0
#define NBIF_LINK_CNTL2_TARGET_LINK_SPEED_WIDTH             4
#define NBIF_LINK_CNTL2_TARGET_LINK_SPEED_MASK              0xf
#define NBIF_LINK_CNTL2_ENTER_COMPLIANCE_OFFSET             4
#define NBIF_LINK_CNTL2_ENTER_COMPLIANCE_WIDTH              1
#define NBIF_LINK_CNTL2_ENTER_COMPLIANCE_MASK               0x10
#define NBIF_LINK_CNTL2_HW_AUTONOMOUS_SPEED_DISABLE_OFFSET  5
#define NBIF_LINK_CNTL2_HW_AUTONOMOUS_SPEED_DISABLE_WIDTH   1
#define NBIF_LINK_CNTL2_HW_AUTONOMOUS_SPEED_DISABLE_MASK    0x20
#define NBIF_LINK_CNTL2_SELECTABLE_DEEMPHASIS_OFFSET        6
#define NBIF_LINK_CNTL2_SELECTABLE_DEEMPHASIS_WIDTH         1
#define NBIF_LINK_CNTL2_SELECTABLE_DEEMPHASIS_MASK          0x40
#define NBIF_LINK_CNTL2_XMIT_MARGIN_OFFSET                  7
#define NBIF_LINK_CNTL2_XMIT_MARGIN_WIDTH                   3
#define NBIF_LINK_CNTL2_XMIT_MARGIN_MASK                    0x380
#define NBIF_LINK_CNTL2_ENTER_MOD_COMPLIANCE_OFFSET         10
#define NBIF_LINK_CNTL2_ENTER_MOD_COMPLIANCE_WIDTH          1
#define NBIF_LINK_CNTL2_ENTER_MOD_COMPLIANCE_MASK           0x400
#define NBIF_LINK_CNTL2_COMPLIANCE_SOS_OFFSET               11
#define NBIF_LINK_CNTL2_COMPLIANCE_SOS_WIDTH                1
#define NBIF_LINK_CNTL2_COMPLIANCE_SOS_MASK                 0x800
#define NBIF_LINK_CNTL2_COMPLIANCE_DEEMPHASIS_OFFSET        12
#define NBIF_LINK_CNTL2_COMPLIANCE_DEEMPHASIS_WIDTH         4
#define NBIF_LINK_CNTL2_COMPLIANCE_DEEMPHASIS_MASK          0xf000

/// NBIF_LINK_CNTL2
typedef union {
  struct {
    UINT32                                       TARGET_LINK_SPEED:4 ; ///<
    UINT32                                        ENTER_COMPLIANCE:1 ; ///<
    UINT32                             HW_AUTONOMOUS_SPEED_DISABLE:1 ; ///<
    UINT32                                   SELECTABLE_DEEMPHASIS:1 ; ///<
    UINT32                                             XMIT_MARGIN:3 ; ///<
    UINT32                                    ENTER_MOD_COMPLIANCE:1 ; ///<
    UINT32                                          COMPLIANCE_SOS:1 ; ///<
    UINT32                                   COMPLIANCE_DEEMPHASIS:4 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} NBIF_LINK_CNTL2_STRUCT;

#define NBIF0_PCIE_VENDOR_SPECIFIC1_HYEX        (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x40108)
#define NBIF1_PCIE_VENDOR_SPECIFIC1_HYEX        (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x40108)
#define NBIF2_PCIE_VENDOR_SPECIFIC1_HYEX        (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x40108)

#define NBIF0_PCIE_VENDOR_SPECIFIC1_HYGX        (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x40108)
#define NBIF1_PCIE_VENDOR_SPECIFIC1_HYGX        (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x40108)
#define NBIF2_PCIE_VENDOR_SPECIFIC1_HYGX        (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x40108)
#define NBIF3_PCIE_VENDOR_SPECIFIC1_HYGX        (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x40108)

#define PCIE0_GPP0_LINK_CTL_STS_ADDRESS_HYEX    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x68)
#define PCIE1_GPP0_LINK_CTL_STS_ADDRESS_HYEX    (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x68)
#define PCIE2_GPP0_LINK_CTL_STS_ADDRESS_HYEX    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x68)

#define PCIE0_GPP0_LINK_CTL_STS_ADDRESS_HYGX    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x68)
#define PCIE2_GPP0_LINK_CTL_STS_ADDRESS_HYGX    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x68)

#define LINK_CTL_STS_DL_ACTIVE_OFFSET          29

#define PCIE0_GPP0_LINK_CTL3_ADDRESS_HYEX       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x274)
#define PCIE0_GPP0_LINK_CTL3_ADDRESS_HYGX       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x274)

// Address
#define SLOT_CAP_PCI_OFFSET                    0x6C

#define PCIE0_GPP0_SLOT_CAP_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x006C)
#define PCIE0_GPP1_SLOT_CAP_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x106C)
#define PCIE0_GPP2_SLOT_CAP_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x206C)
#define PCIE0_GPP3_SLOT_CAP_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x306C)
#define PCIE0_GPP4_SLOT_CAP_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x406C)
#define PCIE0_GPP5_SLOT_CAP_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x506C)
#define PCIE0_GPP6_SLOT_CAP_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x606C)
#define PCIE0_GPP7_SLOT_CAP_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x706C)

#define PCIE0_GPP0_SLOT_CAP_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x006C)
#define PCIE0_GPP1_SLOT_CAP_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x106C)
#define PCIE0_GPP2_SLOT_CAP_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x206C)
#define PCIE0_GPP3_SLOT_CAP_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x306C)
#define PCIE0_GPP4_SLOT_CAP_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x406C)
#define PCIE0_GPP5_SLOT_CAP_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x506C)
#define PCIE0_GPP6_SLOT_CAP_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x606C)
#define PCIE0_GPP7_SLOT_CAP_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x706C)

// Type
#define SLOT_CAP_TYPE                                         TYPE_SMN

#define SLOT_CAP_ATTN_BUTTON_PRESENT_OFFSET                   0
#define SLOT_CAP_ATTN_BUTTON_PRESENT_WIDTH                    1
#define SLOT_CAP_ATTN_BUTTON_PRESENT_MASK                     0x1
#define SLOT_CAP_PWR_CONTROLLER_PRESENT_OFFSET                1
#define SLOT_CAP_PWR_CONTROLLER_PRESENT_WIDTH                 1
#define SLOT_CAP_PWR_CONTROLLER_PRESENT_MASK                  0x2
#define SLOT_CAP_MRL_SENSOR_PRESENT_OFFSET                    2
#define SLOT_CAP_MRL_SENSOR_PRESENT_WIDTH                     1
#define SLOT_CAP_MRL_SENSOR_PRESENT_MASK                      0x4
#define SLOT_CAP_ATTN_INDICATOR_PRESENT_OFFSET                3
#define SLOT_CAP_ATTN_INDICATOR_PRESENT_WIDTH                 1
#define SLOT_CAP_ATTN_INDICATOR_PRESENT_MASK                  0x8
#define SLOT_CAP_PWR_INDICATOR_PRESENT_OFFSET                 4
#define SLOT_CAP_PWR_INDICATOR_PRESENT_WIDTH                  1
#define SLOT_CAP_PWR_INDICATOR_PRESENT_MASK                   0x10
#define SLOT_CAP_HOTPLUG_SURPRISE_OFFSET                      5
#define SLOT_CAP_HOTPLUG_SURPRISE_WIDTH                       1
#define SLOT_CAP_HOTPLUG_SURPRISE_MASK                        0x20
#define SLOT_CAP_HOTPLUG_CAPABLE_OFFSET                       6
#define SLOT_CAP_HOTPLUG_CAPABLE_WIDTH                        1
#define SLOT_CAP_HOTPLUG_CAPABLE_MASK                         0x40
#define SLOT_CAP_SLOT_PWR_LIMIT_VALUE_OFFSET                  7
#define SLOT_CAP_SLOT_PWR_LIMIT_VALUE_WIDTH                   8
#define SLOT_CAP_SLOT_PWR_LIMIT_VALUE_MASK                    0x7f80
#define SLOT_CAP_SLOT_PWR_LIMIT_SCALE_OFFSET                  15
#define SLOT_CAP_SLOT_PWR_LIMIT_SCALE_WIDTH                   2
#define SLOT_CAP_SLOT_PWR_LIMIT_SCALE_MASK                    0x18000
#define SLOT_CAP_ELECTROMECH_INTERLOCK_PRESENT_OFFSET         17
#define SLOT_CAP_ELECTROMECH_INTERLOCK_PRESENT_WIDTH          1
#define SLOT_CAP_ELECTROMECH_INTERLOCK_PRESENT_MASK           0x20000
#define SLOT_CAP_NO_COMMAND_COMPLETED_SUPPORTED_OFFSET        18
#define SLOT_CAP_NO_COMMAND_COMPLETED_SUPPORTED_WIDTH         1
#define SLOT_CAP_NO_COMMAND_COMPLETED_SUPPORTED_MASK          0x40000
#define SLOT_CAP_PHYSICAL_SLOT_NUM_OFFSET                     19
#define SLOT_CAP_PHYSICAL_SLOT_NUM_WIDTH                      13
#define SLOT_CAP_PHYSICAL_SLOT_NUM_MASK                       0xfff80000L

/// SLOT_CAP
typedef union {
  struct {
    UINT32                                     ATTN_BUTTON_PRESENT:1 ; ///<
    UINT32                                  PWR_CONTROLLER_PRESENT:1 ; ///<
    UINT32                                      MRL_SENSOR_PRESENT:1 ; ///<
    UINT32                                  ATTN_INDICATOR_PRESENT:1 ; ///<
    UINT32                                   PWR_INDICATOR_PRESENT:1 ; ///<
    UINT32                                        HOTPLUG_SURPRISE:1 ; ///<
    UINT32                                         HOTPLUG_CAPABLE:1 ; ///<
    UINT32                                    SLOT_PWR_LIMIT_VALUE:8 ; ///<
    UINT32                                    SLOT_PWR_LIMIT_SCALE:2 ; ///<
    UINT32                           ELECTROMECH_INTERLOCK_PRESENT:1 ; ///<
    UINT32                          NO_COMMAND_COMPLETED_SUPPORTED:1 ; ///<
    UINT32                                       PHYSICAL_SLOT_NUM:13; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} SLOT_CAP_STRUCT;

// SLOT_CNTL and SLOT_STATUS
// Address
#define SLOT_CNTL_PCI_OFFSET                     0x70

#define GPP0_SLOT_CNTL_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x0070)
#define GPP1_SLOT_CNTL_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x1070)
#define GPP2_SLOT_CNTL_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x2070)
#define GPP3_SLOT_CNTL_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x3070)
#define GPP4_SLOT_CNTL_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x4070)
#define GPP5_SLOT_CNTL_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x5070)
#define GPP6_SLOT_CNTL_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x6070)
#define GPP7_SLOT_CNTL_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x7070)

#define GPP0_SLOT_CNTL_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x0070)
#define GPP1_SLOT_CNTL_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x1070)
#define GPP2_SLOT_CNTL_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x2070)
#define GPP3_SLOT_CNTL_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x3070)
#define GPP4_SLOT_CNTL_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x4070)
#define GPP5_SLOT_CNTL_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x5070)
#define GPP6_SLOT_CNTL_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x6070)
#define GPP7_SLOT_CNTL_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x7070)

// DEVICE_CNTL2
#define DEVICE_CNTL2_PCI_OFFSET                      0x80
#define DEVICE_CNTL2_CPL_TIMEOUT_VALUE_OFFSET        0
#define DEVICE_CNTL2_CPL_TIMEOUT_VALUE_WIDTH         4
#define DEVICE_CNTL2_CPL_TIMEOUT_VALUE_MASK          0xF

// Type
#define SLOT_CNTL_TYPE                                         TYPE_SMN

#define SLOT_CNTL_ATTN_BUTTON_PRESSED_EN_OFFSET                0
#define SLOT_CNTL_ATTN_BUTTON_PRESSED_EN_WIDTH                 1
#define SLOT_CNTL_ATTN_BUTTON_PRESSED_EN_MASK                  0x1
#define SLOT_CNTL_PWR_FAULT_DETECTED_EN_OFFSET                 1
#define SLOT_CNTL_PWR_FAULT_DETECTED_EN_WIDTH                  1
#define SLOT_CNTL_PWR_FAULT_DETECTED_EN_MASK                   0x2
#define SLOT_CNTL_MRL_SENSOR_CHANGED_EN_OFFSET                 2
#define SLOT_CNTL_MRL_SENSOR_CHANGED_EN_WIDTH                  1
#define SLOT_CNTL_MRL_SENSOR_CHANGED_EN_MASK                   0x4
#define SLOT_CNTL_PRESENCE_DETECT_CHANGED_EN_OFFSET            3
#define SLOT_CNTL_PRESENCE_DETECT_CHANGED_EN_WIDTH             1
#define SLOT_CNTL_PRESENCE_DETECT_CHANGED_EN_MASK              0x8
#define SLOT_CNTL_COMMAND_COMPLETED_INTR_EN_OFFSET             4
#define SLOT_CNTL_COMMAND_COMPLETED_INTR_EN_WIDTH              1
#define SLOT_CNTL_COMMAND_COMPLETED_INTR_EN_MASK               0x10
#define SLOT_CNTL_HOTPLUG_INTR_EN_OFFSET                       5
#define SLOT_CNTL_HOTPLUG_INTR_EN_WIDTH                        1
#define SLOT_CNTL_HOTPLUG_INTR_EN_MASK                         0x20
#define SLOT_CNTL_ATTN_INDICATOR_CNTL_OFFSET                   6
#define SLOT_CNTL_ATTN_INDICATOR_CNTL_WIDTH                    2
#define SLOT_CNTL_ATTN_INDICATOR_CNTL_MASK                     0xc0
#define SLOT_CNTL_PWR_INDICATOR_CNTL_OFFSET                    8
#define SLOT_CNTL_PWR_INDICATOR_CNTL_WIDTH                     2
#define SLOT_CNTL_PWR_INDICATOR_CNTL_MASK                      0x300
#define SLOT_CNTL_PWR_CONTROLLER_CNTL_OFFSET                   10
#define SLOT_CNTL_PWR_CONTROLLER_CNTL_WIDTH                    1
#define SLOT_CNTL_PWR_CONTROLLER_CNTL_MASK                     0x400
#define SLOT_CNTL_ELECTROMECH_INTERLOCK_CNTL_OFFSET            11
#define SLOT_CNTL_ELECTROMECH_INTERLOCK_CNTL_WIDTH             1
#define SLOT_CNTL_ELECTROMECH_INTERLOCK_CNTL_MASK              0x800
#define SLOT_CNTL_DL_STATE_CHANGED_EN_OFFSET                   12
#define SLOT_CNTL_DL_STATE_CHANGED_EN_WIDTH                    1
#define SLOT_CNTL_DL_STATE_CHANGED_EN_MASK                     0x1000
//Reserved 3
#define SLOT_CNTL_ATTN_BUTTON_PRESSED_OFFSET                   16
#define SLOT_CNTL_ATTN_BUTTON_PRESSED_WIDTH                    1
#define SLOT_CNTL_ATTN_BUTTON_PRESSED_MASK                     0x10000
#define SLOT_CNTL_PWR_FAULT_DETECTED_OFFSET                    17
#define SLOT_CNTL_PWR_FAULT_DETECTED_WIDTH                     1
#define SLOT_CNTL_PWR_FAULT_DETECTED_MASK                      0x20000
#define SLOT_CNTL_MRL_SENSOR_CHANGED_OFFSET                    18
#define SLOT_CNTL_MRL_SENSOR_CHANGED_WIDTH                     1
#define SLOT_CNTL_MRL_SENSOR_CHANGED_MASK                      0x40000
#define SLOT_CNTL_PRESENCE_DETECT_CHANGED_OFFSET               19
#define SLOT_CNTL_PRESENCE_DETECT_CHANGED_WIDTH                1
#define SLOT_CNTL_PRESENCE_DETECT_CHANGED_MASK                 0x80000
#define SLOT_CNTL_COMMAND_COMPLETED_OFFSET                     20
#define SLOT_CNTL_COMMAND_COMPLETED_WIDTH                      1
#define SLOT_CNTL_COMMAND_COMPLETED_MASK                       0x100000
#define SLOT_CNTL_MRL_SENSOR_STATE_OFFSET                      21
#define SLOT_CNTL_MRL_SENSOR_STATE_WIDTH                       1
#define SLOT_CNTL_MRL_SENSOR_STATE_MASK                        0x200000
#define SLOT_CNTL_PRESENCE_DETECT_STATE_OFFSET                 22
#define SLOT_CNTL_PRESENCE_DETECT_STATE_WIDTH                  1
#define SLOT_CNTL_PRESENCE_DETECT_STATE_MASK                   0x400000
#define SLOT_CNTL_ELECTROMECH_INTERLOCK_STATUS_OFFSET          23
#define SLOT_CNTL_ELECTROMECH_INTERLOCK_STATUS_WIDTH           1
#define SLOT_CNTL_ELECTROMECH_INTERLOCK_STATUS_MASK            0x800000
#define SLOT_CNTL_DL_STATE_CHANGED_OFFSET                      24
#define SLOT_CNTL_DL_STATE_CHANGED_WIDTH                       1
#define SLOT_CNTL_DL_STATE_CHANGED_MASK                        0x1000000
//Reserved 7

/// SLOT_CNTL_STAT
typedef union {
  struct {
    UINT32    ATTN_BUTTON_PRESSED_EN       : 1;                        ///<
    UINT32    PWR_FAULT_DETECTED_EN        : 1;                        ///<
    UINT32    MRL_SENSOR_CHANGED_EN        : 1;                        ///<
    UINT32    PRESENCE_DETECT_CHANGED_EN   : 1;                        ///<
    UINT32    COMMAND_COMPLETED_INTR_EN    : 1;                        ///<
    UINT32    HOTPLUG_INTR_EN              : 1;                        ///<
    UINT32    ATTN_INDICATOR_CNTL          : 2;                        ///<
    UINT32    PWR_INDICATOR_CNTL           : 2;                        ///<
    UINT32    PWR_CONTROLLER_CNTL          : 1;                        ///<
    UINT32    ELECTROMECH_INTERLOCK_CNTL   : 1;                        ///<
    UINT32    DL_STATE_CHANGED_EN          : 1;                        ///<
    UINT32    AUTO_SLOT_PWR_LIMIT_DISABLE  : 1;                        ///<    
    UINT32    Reserved2                    : 2;                        ///<
    UINT32    ATTN_BUTTON_PRESSED          : 1;                        ///<
    UINT32    PWR_FAULT_DETECTED           : 1;                        ///<
    UINT32    MRL_SENSOR_CHANGED           : 1;                        ///<
    UINT32    PRESENCE_DETECT_CHANGED      : 1;                        ///<
    UINT32    COMMAND_COMPLETED            : 1;                        ///<
    UINT32    MRL_SENSOR_STATE             : 1;                        ///<
    UINT32    PRESENCE_DETECT_STATE        : 1;                        ///<
    UINT32    ELECTROMECH_INTERLOCK_STATUS : 1;                        ///<
    UINT32    DL_STATE_CHANGED             : 1;                        ///<
    UINT32    Reserved7                    : 7;                        ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} SLOT_CNTL_STRUCT;

// Register Name PCIE_STRAP_F0

// Address
#define PCIE0_STRAP_F0_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x802c0)
#define PCIE1_STRAP_F0_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x802c0)
#define PCIE2_STRAP_F0_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x802c0)

#define PCIE0_STRAP_F0_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x802c0)
#define PCIE2_STRAP_F0_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x802c0)

// Type
#define PCIE_STRAP_F0_TYPE                                              TYPE_SMN

#define PCIE_STRAP_F0_STRAP_F0_EN_OFFSET                                0
#define PCIE_STRAP_F0_STRAP_F0_EN_WIDTH                                 1
#define PCIE_STRAP_F0_STRAP_F0_EN_MASK                                  0x1
#define PCIE_STRAP_F0_STRAP_F0_LEGACY_DEVICE_TYPE_EN_OFFSET             1
#define PCIE_STRAP_F0_STRAP_F0_LEGACY_DEVICE_TYPE_EN_WIDTH              1
#define PCIE_STRAP_F0_STRAP_F0_LEGACY_DEVICE_TYPE_EN_MASK               0x2
#define PCIE_STRAP_F0_STRAP_F0_MSI_EN_OFFSET                            2
#define PCIE_STRAP_F0_STRAP_F0_MSI_EN_WIDTH                             1
#define PCIE_STRAP_F0_STRAP_F0_MSI_EN_MASK                              0x4
#define PCIE_STRAP_F0_STRAP_F0_VC_EN_OFFSET                             3
#define PCIE_STRAP_F0_STRAP_F0_VC_EN_WIDTH                              1
#define PCIE_STRAP_F0_STRAP_F0_VC_EN_MASK                               0x8
#define PCIE_STRAP_F0_STRAP_F0_DSN_EN_OFFSET                            4
#define PCIE_STRAP_F0_STRAP_F0_DSN_EN_WIDTH                             1
#define PCIE_STRAP_F0_STRAP_F0_DSN_EN_MASK                              0x10
#define PCIE_STRAP_F0_STRAP_F0_AER_EN_OFFSET                            5
#define PCIE_STRAP_F0_STRAP_F0_AER_EN_WIDTH                             1
#define PCIE_STRAP_F0_STRAP_F0_AER_EN_MASK                              0x20
#define PCIE_STRAP_F0_STRAP_F0_ACS_EN_OFFSET                            6
#define PCIE_STRAP_F0_STRAP_F0_ACS_EN_WIDTH                             1
#define PCIE_STRAP_F0_STRAP_F0_ACS_EN_MASK                              0x40
#define PCIE_STRAP_F0_STRAP_F0_BAR_EN_OFFSET                            7
#define PCIE_STRAP_F0_STRAP_F0_BAR_EN_WIDTH                             1
#define PCIE_STRAP_F0_STRAP_F0_BAR_EN_MASK                              0x80
#define PCIE_STRAP_F0_STRAP_F0_PWR_EN_OFFSET                            8
#define PCIE_STRAP_F0_STRAP_F0_PWR_EN_WIDTH                             1
#define PCIE_STRAP_F0_STRAP_F0_PWR_EN_MASK                              0x100
#define PCIE_STRAP_F0_STRAP_F0_DPA_EN_OFFSET                            9
#define PCIE_STRAP_F0_STRAP_F0_DPA_EN_WIDTH                             1
#define PCIE_STRAP_F0_STRAP_F0_DPA_EN_MASK                              0x200
#define PCIE_STRAP_F0_STRAP_F0_ATS_EN_OFFSET                            10
#define PCIE_STRAP_F0_STRAP_F0_ATS_EN_WIDTH                             1
#define PCIE_STRAP_F0_STRAP_F0_ATS_EN_MASK                              0x400
#define PCIE_STRAP_F0_STRAP_F0_PAGE_REQ_EN_OFFSET                       11
#define PCIE_STRAP_F0_STRAP_F0_PAGE_REQ_EN_WIDTH                        1
#define PCIE_STRAP_F0_STRAP_F0_PAGE_REQ_EN_MASK                         0x800
#define PCIE_STRAP_F0_STRAP_F0_PASID_EN_OFFSET                          12
#define PCIE_STRAP_F0_STRAP_F0_PASID_EN_WIDTH                           1
#define PCIE_STRAP_F0_STRAP_F0_PASID_EN_MASK                            0x1000
#define PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_OFFSET                     13
#define PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_WIDTH                      1
#define PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_MASK                       0x2000
#define PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_OFFSET                       14
#define PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_WIDTH                        1
#define PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_MASK                         0x4000
#define PCIE_STRAP_F0_STRAP_F0_CPL_ABORT_ERR_EN_OFFSET                  15
#define PCIE_STRAP_F0_STRAP_F0_CPL_ABORT_ERR_EN_WIDTH                   1
#define PCIE_STRAP_F0_STRAP_F0_CPL_ABORT_ERR_EN_MASK                    0x8000
#define PCIE_STRAP_F0_STRAP_F0_POISONED_ADVISORY_NONFATAL_OFFSET        16
#define PCIE_STRAP_F0_STRAP_F0_POISONED_ADVISORY_NONFATAL_WIDTH         1
#define PCIE_STRAP_F0_STRAP_F0_POISONED_ADVISORY_NONFATAL_MASK          0x10000
#define PCIE_STRAP_F0_STRAP_F0_MC_EN_OFFSET                             17
#define PCIE_STRAP_F0_STRAP_F0_MC_EN_WIDTH                              1
#define PCIE_STRAP_F0_STRAP_F0_MC_EN_MASK                               0x20000
#define PCIE_STRAP_F0_STRAP_F0_ATOMIC_EN_OFFSET                         18
#define PCIE_STRAP_F0_STRAP_F0_ATOMIC_EN_WIDTH                          1
#define PCIE_STRAP_F0_STRAP_F0_ATOMIC_EN_MASK                           0x40000
#define PCIE_STRAP_F0_STRAP_F0_ATOMIC_64BIT_EN_OFFSET                   19
#define PCIE_STRAP_F0_STRAP_F0_ATOMIC_64BIT_EN_WIDTH                    1
#define PCIE_STRAP_F0_STRAP_F0_ATOMIC_64BIT_EN_MASK                     0x80000
#define PCIE_STRAP_F0_STRAP_F0_ATOMIC_ROUTING_EN_OFFSET                 20
#define PCIE_STRAP_F0_STRAP_F0_ATOMIC_ROUTING_EN_WIDTH                  1
#define PCIE_STRAP_F0_STRAP_F0_ATOMIC_ROUTING_EN_MASK                   0x100000
#define PCIE_STRAP_F0_STRAP_F0_MSI_MULTI_CAP_OFFSET                     21
#define PCIE_STRAP_F0_STRAP_F0_MSI_MULTI_CAP_WIDTH                      3
#define PCIE_STRAP_F0_STRAP_F0_MSI_MULTI_CAP_MASK                       0xe00000
#define PCIE_STRAP_F0_STRAP_F0_VFn_MSI_MULTI_CAP_OFFSET                 24
#define PCIE_STRAP_F0_STRAP_F0_VFn_MSI_MULTI_CAP_WIDTH                  3
#define PCIE_STRAP_F0_STRAP_F0_VFn_MSI_MULTI_CAP_MASK                   0x7000000
#define PCIE_STRAP_F0_STRAP_F0_MSI_PERVECTOR_MASK_CAP_OFFSET            27
#define PCIE_STRAP_F0_STRAP_F0_MSI_PERVECTOR_MASK_CAP_WIDTH             1
#define PCIE_STRAP_F0_STRAP_F0_MSI_PERVECTOR_MASK_CAP_MASK              0x8000000
#define PCIE_STRAP_F0_STRAP_F0_NO_RO_ENABLED_P2P_PASSING_OFFSET         28
#define PCIE_STRAP_F0_STRAP_F0_NO_RO_ENABLED_P2P_PASSING_WIDTH          1
#define PCIE_STRAP_F0_STRAP_F0_NO_RO_ENABLED_P2P_PASSING_MASK           0x10000000
#define PCIE_STRAP_F0_STRAP_SWUS_ARI_EN_OFFSET                          29
#define PCIE_STRAP_F0_STRAP_SWUS_ARI_EN_WIDTH                           1
#define PCIE_STRAP_F0_STRAP_SWUS_ARI_EN_MASK                            0x20000000
#define PCIE_STRAP_F0_STRAP_F0_SRIOV_EN_OFFSET                          30
#define PCIE_STRAP_F0_STRAP_F0_SRIOV_EN_WIDTH                           1
#define PCIE_STRAP_F0_STRAP_F0_SRIOV_EN_MASK                            0x40000000
#define PCIE_STRAP_F0_STRAP_F0_MSI_MAP_EN_OFFSET                        31
#define PCIE_STRAP_F0_STRAP_F0_MSI_MAP_EN_WIDTH                         1
#define PCIE_STRAP_F0_STRAP_F0_MSI_MAP_EN_MASK                          0x80000000L

/// PCIE_STRAP_F0
typedef union {
  struct {
    UINT32                                             STRAP_F0_EN:1 ; ///<
    UINT32                          STRAP_F0_LEGACY_DEVICE_TYPE_EN:1 ; ///<
    UINT32                                         STRAP_F0_MSI_EN:1 ; ///<
    UINT32                                          STRAP_F0_VC_EN:1 ; ///<
    UINT32                                         STRAP_F0_DSN_EN:1 ; ///<
    UINT32                                         STRAP_F0_AER_EN:1 ; ///<
    UINT32                                         STRAP_F0_ACS_EN:1 ; ///<
    UINT32                                         STRAP_F0_BAR_EN:1 ; ///<
    UINT32                                         STRAP_F0_PWR_EN:1 ; ///<
    UINT32                                         STRAP_F0_DPA_EN:1 ; ///<
    UINT32                                         STRAP_F0_ATS_EN:1 ; ///<
    UINT32                                    STRAP_F0_PAGE_REQ_EN:1 ; ///<
    UINT32                                       STRAP_F0_PASID_EN:1 ; ///<
    UINT32                                  STRAP_F0_ECRC_CHECK_EN:1 ; ///<
    UINT32                                    STRAP_F0_ECRC_GEN_EN:1 ; ///<
    UINT32                               STRAP_F0_CPL_ABORT_ERR_EN:1 ; ///<
    UINT32                     STRAP_F0_POISONED_ADVISORY_NONFATAL:1 ; ///<
    UINT32                                          STRAP_F0_MC_EN:1 ; ///<
    UINT32                                      STRAP_F0_ATOMIC_EN:1 ; ///<
    UINT32                                STRAP_F0_ATOMIC_64BIT_EN:1 ; ///<
    UINT32                              STRAP_F0_ATOMIC_ROUTING_EN:1 ; ///<
    UINT32                                  STRAP_F0_MSI_MULTI_CAP:3 ; ///<
    UINT32                              STRAP_F0_VFn_MSI_MULTI_CAP:3 ; ///<
    UINT32                         STRAP_F0_MSI_PERVECTOR_MASK_CAP:1 ; ///<
    UINT32                      STRAP_F0_NO_RO_ENABLED_P2P_PASSING:1 ; ///<
    UINT32                                       STRAP_SWUS_ARI_EN:1 ; ///<
    UINT32                                       STRAP_F0_SRIOV_EN:1 ; ///<
    UINT32                                     STRAP_F0_MSI_MAP_EN:1 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_STRAP_F0_STRUCT;

// Register Name PCIE_L1_PM_SUB_CNTL

// Address
#define PCIE_L1_PM_SUB_CNTL_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x378)
#define PCIE_L1_PM_SUB_CNTL_ADDRESS_HYGX                      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x378)

// Type
#define PCIE_L1_PM_SUB_CNTL_TYPE                             TYPE_SMN

#define PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_2_EN_OFFSET            0
#define PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_2_EN_WIDTH             1
#define PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_2_EN_MASK              0x1
#define PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_1_EN_OFFSET            1
#define PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_1_EN_WIDTH             1
#define PCIE_L1_PM_SUB_CNTL_PCI_PM_L1_1_EN_MASK              0x2
#define PCIE_L1_PM_SUB_CNTL_ASPM_L1_2_EN_OFFSET              2
#define PCIE_L1_PM_SUB_CNTL_ASPM_L1_2_EN_WIDTH               1
#define PCIE_L1_PM_SUB_CNTL_ASPM_L1_2_EN_MASK                0x4
#define PCIE_L1_PM_SUB_CNTL_ASPM_L1_1_EN_OFFSET              3
#define PCIE_L1_PM_SUB_CNTL_ASPM_L1_1_EN_WIDTH               1
#define PCIE_L1_PM_SUB_CNTL_ASPM_L1_1_EN_MASK                0x8
#define PCIE_L1_PM_SUB_CNTL_LINK_ACTIVATION_INT_EN_OFFSET    4
#define PCIE_L1_PM_SUB_CNTL_LINK_ACTIVATION_INT_EN_WIDTH     1
#define PCIE_L1_PM_SUB_CNTL_LINK_ACTIVATION_INT_EN_MASK      0x10
#define PCIE_L1_PM_SUB_CNTL_LINK_ACTIVATION_CTRL_OFFSET      5
#define PCIE_L1_PM_SUB_CNTL_LINK_ACTIVATION_CTRL_WIDTH       1
#define PCIE_L1_PM_SUB_CNTL_LINK_ACTIVATION_CTRL_MASK        0x20
#define PCIE_L1_PM_SUB_CNTL_COMMON_MODE_RESTORE_TIME_OFFSET  8
#define PCIE_L1_PM_SUB_CNTL_COMMON_MODE_RESTORE_TIME_WIDTH   8
#define PCIE_L1_PM_SUB_CNTL_COMMON_MODE_RESTORE_TIME_MASK    0xff00
#define PCIE_L1_PM_SUB_CNTL_LTR_L1_2_THRESHOLD_VALUE_OFFSET  16
#define PCIE_L1_PM_SUB_CNTL_LTR_L1_2_THRESHOLD_VALUE_WIDTH   10
#define PCIE_L1_PM_SUB_CNTL_LTR_L1_2_THRESHOLD_VALUE_MASK    0x3ff0000
#define PCIE_L1_PM_SUB_CNTL_LTR_L1_2_THRESHOLD_SCALE_OFFSET  29
#define PCIE_L1_PM_SUB_CNTL_LTR_L1_2_THRESHOLD_SCALE_WIDTH   3
#define PCIE_L1_PM_SUB_CNTL_LTR_L1_2_THRESHOLD_SCALE_MASK    0xe0000000L

/// PCIERCCFG0F0_00000378
typedef union {
  struct {
    UINT32                                          PCI_PM_L1_2_EN:1 ; ///<
    UINT32                                          PCI_PM_L1_1_EN:1 ; ///<
    UINT32                                            ASPM_L1_2_EN:1 ; ///<
    UINT32                                            ASPM_L1_1_EN:1 ; ///<
    UINT32                                  LINK_ACTIVATION_INT_EN:1 ; ///<
    UINT32                                    LINK_ACTIVATION_CTRL:1 ; ///<
    UINT32                                            Reserved_7_6:2 ; ///<
    UINT32                                COMMON_MODE_RESTORE_TIME:8 ; ///<
    UINT32                                LTR_L1_2_THRESHOLD_VALUE:10; ///<
    UINT32                                          Reserved_28_26:3 ; ///<
    UINT32                                LTR_L1_2_THRESHOLD_SCALE:3 ; ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_L1_PM_SUB_CNTL_STRUCT;

// PCIE_LINK_CAP
// Address
#define PCIE0_LINK_CAP_SMN_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x64)
#define PCIE0_LINK_CAP_SMN_ADDRESS_HYGX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x64)
// PCIE_LINK_STATUS
// Address
#define PCIE0_LINK_STATUS_SMN_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x6A)
#define PCIE0_LINK_STATUS_SMN_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x6A)

// Register Name PCIE_HW_DEBUG
#define PCIE0_PCIE_HW_DEBUG_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80008)
#define PCIE1_PCIE_HW_DEBUG_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80008)
#define PCIE2_PCIE_HW_DEBUG_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80008)

#define PCIE0_PCIE_HW_DEBUG_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80008)
#define PCIE1_PCIE_HW_DEBUG_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYGX << 20) + 0x80008)
#define PCIE2_PCIE_HW_DEBUG_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80008)
// Register Name PCIEP_HW_DEBUG
// Address
#define PCIE0_PCIEP_HW_DEBUG_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40008)
#define PCIE1_PCIEP_HW_DEBUG_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40008)
#define PCIE2_PCIEP_HW_DEBUG_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40008)

#define PCIE0_PCIEP_HW_DEBUG_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40008)
#define PCIE2_PCIEP_HW_DEBUG_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40008)

// Type
#define PCIEP_HW_DEBUG_TYPE                        TYPE_SMN

#define PCIEP_HW_DEBUG_HW_00_DEBUG_OFFSET     0
#define PCIEP_HW_DEBUG_HW_00_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_00_DEBUG_MASK       0x1
#define PCIEP_HW_DEBUG_HW_01_DEBUG_OFFSET     1
#define PCIEP_HW_DEBUG_HW_01_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_01_DEBUG_MASK       0x2
#define PCIEP_HW_DEBUG_HW_02_DEBUG_OFFSET     2
#define PCIEP_HW_DEBUG_HW_02_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_02_DEBUG_MASK       0x4
#define PCIEP_HW_DEBUG_HW_03_DEBUG_OFFSET     3
#define PCIEP_HW_DEBUG_HW_03_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_03_DEBUG_MASK       0x8
#define PCIEP_HW_DEBUG_HW_04_DEBUG_OFFSET     4
#define PCIEP_HW_DEBUG_HW_04_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_04_DEBUG_MASK       0x10
#define PCIEP_HW_DEBUG_HW_05_DEBUG_OFFSET     5
#define PCIEP_HW_DEBUG_HW_05_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_05_DEBUG_MASK       0x20
#define PCIEP_HW_DEBUG_HW_06_DEBUG_OFFSET     6
#define PCIEP_HW_DEBUG_HW_06_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_06_DEBUG_MASK       0x40
#define PCIEP_HW_DEBUG_HW_07_DEBUG_OFFSET     7
#define PCIEP_HW_DEBUG_HW_07_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_07_DEBUG_MASK       0x80
#define PCIEP_HW_DEBUG_HW_08_DEBUG_OFFSET     8
#define PCIEP_HW_DEBUG_HW_08_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_08_DEBUG_MASK       0x100
#define PCIEP_HW_DEBUG_HW_09_DEBUG_OFFSET     9
#define PCIEP_HW_DEBUG_HW_09_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_09_DEBUG_MASK       0x200
#define PCIEP_HW_DEBUG_HW_10_DEBUG_OFFSET     10
#define PCIEP_HW_DEBUG_HW_10_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_10_DEBUG_MASK       0x400
#define PCIEP_HW_DEBUG_HW_11_DEBUG_OFFSET     11
#define PCIEP_HW_DEBUG_HW_11_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_11_DEBUG_MASK       0x800
#define PCIEP_HW_DEBUG_HW_12_DEBUG_OFFSET     12
#define PCIEP_HW_DEBUG_HW_12_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_12_DEBUG_MASK       0x1000
#define PCIEP_HW_DEBUG_HW_13_DEBUG_OFFSET     13
#define PCIEP_HW_DEBUG_HW_13_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_13_DEBUG_MASK       0x2000
#define PCIEP_HW_DEBUG_HW_14_DEBUG_OFFSET     14
#define PCIEP_HW_DEBUG_HW_14_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_14_DEBUG_MASK       0x4000
#define PCIEP_HW_DEBUG_HW_15_DEBUG_OFFSET     15
#define PCIEP_HW_DEBUG_HW_15_DEBUG_WIDTH      1
#define PCIEP_HW_DEBUG_HW_15_DEBUG_MASK       0x8000
#define PCIEP_HW_DEBUG_Reserved_31_16_OFFSET  16
#define PCIEP_HW_DEBUG_Reserved_31_16_WIDTH   16
#define PCIEP_HW_DEBUG_Reserved_31_16_MASK    0xffff0000L

/// PCIEPORT0F0_00000008
typedef union {
  struct {
    UINT32    HW_00_DEBUG    : 1;                                      ///<
    UINT32    HW_01_DEBUG    : 1;                                      ///<
    UINT32    HW_02_DEBUG    : 1;                                      ///<
    UINT32    HW_03_DEBUG    : 1;                                      ///<
    UINT32    HW_04_DEBUG    : 1;                                      ///<
    UINT32    HW_05_DEBUG    : 1;                                      ///<
    UINT32    HW_06_DEBUG    : 1;                                      ///<
    UINT32    HW_07_DEBUG    : 1;                                      ///<
    UINT32    HW_08_DEBUG    : 1;                                      ///<
    UINT32    HW_09_DEBUG    : 1;                                      ///<
    UINT32    HW_10_DEBUG    : 1;                                      ///<
    UINT32    HW_11_DEBUG    : 1;                                      ///<
    UINT32    HW_12_DEBUG    : 1;                                      ///<
    UINT32    HW_13_DEBUG    : 1;                                      ///<
    UINT32    HW_14_DEBUG    : 1;                                      ///<
    UINT32    HW_15_DEBUG    : 1;                                      ///<
    UINT32    Reserved_31_16 : 16;                                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIEP_HW_DEBUG_STRUCT;

// Register Name PCIEP_PORT_CNTL

// Address
#define PCIEP_PORT_CNTL_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40040)
#define PCIE0_GPP0_PORT_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40040)
#define PCIE0_GPP1_PORT_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x41040)
#define PCIE0_GPP2_PORT_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x42040)
#define PCIE0_GPP3_PORT_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x43040)
#define PCIE0_GPP4_PORT_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x44040)
#define PCIE0_GPP5_PORT_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x45040)
#define PCIE0_GPP6_PORT_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x46040)
#define PCIE0_GPP7_PORT_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x47040)

#define PCIEP_PORT_CNTL_ADDRESS_HYGX       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40040)
#define PCIE0_GPP0_PORT_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40040)
#define PCIE0_GPP1_PORT_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x41040)
#define PCIE0_GPP2_PORT_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x42040)
#define PCIE0_GPP3_PORT_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x43040)
#define PCIE0_GPP4_PORT_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x44040)
#define PCIE0_GPP5_PORT_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x45040)
#define PCIE0_GPP6_PORT_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x46040)
#define PCIE0_GPP7_PORT_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x47040)

// Type
#define PCIEP_PORT_CNTL_TYPE                                    TYPE_SMN

#define PCIEP_PORT_CNTL_SLV_PORT_REQ_EN_OFFSET                  0
#define PCIEP_PORT_CNTL_SLV_PORT_REQ_EN_WIDTH                   1
#define PCIEP_PORT_CNTL_SLV_PORT_REQ_EN_MASK                    0x1
#define PCIEP_PORT_CNTL_CI_SNOOP_OVERRIDE_OFFSET                1
#define PCIEP_PORT_CNTL_CI_SNOOP_OVERRIDE_WIDTH                 1
#define PCIEP_PORT_CNTL_CI_SNOOP_OVERRIDE_MASK                  0x2
#define PCIEP_PORT_CNTL_HOTPLUG_MSG_EN_OFFSET                   2
#define PCIEP_PORT_CNTL_HOTPLUG_MSG_EN_WIDTH                    1
#define PCIEP_PORT_CNTL_HOTPLUG_MSG_EN_MASK                     0x4
#define PCIEP_PORT_CNTL_NATIVE_PME_EN_OFFSET                    3
#define PCIEP_PORT_CNTL_NATIVE_PME_EN_WIDTH                     1
#define PCIEP_PORT_CNTL_NATIVE_PME_EN_MASK                      0x8
#define PCIEP_PORT_CNTL_PWR_FAULT_EN_OFFSET                     4
#define PCIEP_PORT_CNTL_PWR_FAULT_EN_WIDTH                      1
#define PCIEP_PORT_CNTL_PWR_FAULT_EN_MASK                       0x10
#define PCIEP_PORT_CNTL_PMI_BM_DIS_OFFSET                       5
#define PCIEP_PORT_CNTL_PMI_BM_DIS_WIDTH                        1
#define PCIEP_PORT_CNTL_PMI_BM_DIS_MASK                         0x20
#define PCIEP_PORT_CNTL_SEQNUM_DEBUG_MODE_OFFSET                6
#define PCIEP_PORT_CNTL_SEQNUM_DEBUG_MODE_WIDTH                 1
#define PCIEP_PORT_CNTL_SEQNUM_DEBUG_MODE_MASK                  0x40
#define PCIEP_PORT_CNTL_CI_SLV_CPL_STATIC_ALLOC_LIMIT_S_OFFSET  8
#define PCIEP_PORT_CNTL_CI_SLV_CPL_STATIC_ALLOC_LIMIT_S_WIDTH   8
#define PCIEP_PORT_CNTL_CI_SLV_CPL_STATIC_ALLOC_LIMIT_S_MASK    0xff00
#define PCIEP_PORT_CNTL_CI_MAX_CPL_PAYLOAD_SIZE_MODE_OFFSET     16
#define PCIEP_PORT_CNTL_CI_MAX_CPL_PAYLOAD_SIZE_MODE_WIDTH      2
#define PCIEP_PORT_CNTL_CI_MAX_CPL_PAYLOAD_SIZE_MODE_MASK       0x30000
#define PCIEP_PORT_CNTL_CI_PRIV_MAX_CPL_PAYLOAD_SIZE_OFFSET     18
#define PCIEP_PORT_CNTL_CI_PRIV_MAX_CPL_PAYLOAD_SIZE_WIDTH      3
#define PCIEP_PORT_CNTL_CI_PRIV_MAX_CPL_PAYLOAD_SIZE_MASK       0x1c0000
#define PCIEP_PORT_CNTL_CI_SLV_RSP_POISONED_UR_MODE_OFFSET      24
#define PCIEP_PORT_CNTL_CI_SLV_RSP_POISONED_UR_MODE_WIDTH       2
#define PCIEP_PORT_CNTL_CI_SLV_RSP_POISONED_UR_MODE_MASK        0x3000000

/// PCIEP_PORT_CNTL
typedef union {
  struct {
    UINT32    SLV_PORT_REQ_EN                 : 1;                     ///<
    UINT32    CI_SNOOP_OVERRIDE               : 1;                     ///<
    UINT32    HOTPLUG_MSG_EN                  : 1;                     ///<
    UINT32    NATIVE_PME_EN                   : 1;                     ///<
    UINT32    PWR_FAULT_EN                    : 1;                     ///<
    UINT32    PMI_BM_DIS                      : 1;                     ///<
    UINT32    SEQNUM_DEBUG_MODE               : 1;                     ///<
    UINT32    Reserved_7_7                    : 1;                     ///<
    UINT32    CI_SLV_CPL_STATIC_ALLOC_LIMIT_S : 8;                     ///<
    UINT32    CI_MAX_CPL_PAYLOAD_SIZE_MODE    : 2;                     ///<
    UINT32    CI_PRIV_MAX_CPL_PAYLOAD_SIZE    : 3;                     ///<
    UINT32    Reserved_23_21                  : 3;                     ///<
    UINT32    CI_SLV_RSP_POISONED_UR_MODE     : 2;                     ///<
    UINT32    Reserved_31_26                  : 6;                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIEP_PORT_CNTL_STRUCT;

// Register Name PCIE_TX_CNTL

// Address
#define PCIE_TX_CNTL_ADDRESS_HYEX                (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40080)
#define PCIE0_GPP0_TX_CNTL_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40080)
#define PCIE0_GPP1_TX_CNTL_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x41080)
#define PCIE0_GPP2_TX_CNTL_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x42080)
#define PCIE0_GPP3_TX_CNTL_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x43080)
#define PCIE0_GPP4_TX_CNTL_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x44080)
#define PCIE0_GPP5_TX_CNTL_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x45080)
#define PCIE0_GPP6_TX_CNTL_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x46080)
#define PCIE0_GPP7_TX_CNTL_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x47080)

#define PCIE_TX_CNTL_ADDRESS_HYGX                (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40080)
#define PCIE0_GPP0_TX_CNTL_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40080)
#define PCIE0_GPP1_TX_CNTL_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x41080)
#define PCIE0_GPP2_TX_CNTL_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x42080)
#define PCIE0_GPP3_TX_CNTL_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x43080)
#define PCIE0_GPP4_TX_CNTL_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x44080)
#define PCIE0_GPP5_TX_CNTL_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x45080)
#define PCIE0_GPP6_TX_CNTL_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x46080)
#define PCIE0_GPP7_TX_CNTL_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x47080)

// Type
#define PCIE_TX_CNTL_TYPE                             TYPE_SMN

#define PCIE_TX_CNTL_Reserved_9_0_OFFSET              0
#define PCIE_TX_CNTL_Reserved_9_0_WIDTH               10
#define PCIE_TX_CNTL_Reserved_9_0_MASK                0x3ff
#define PCIE_TX_CNTL_TX_SNR_OVERRIDE_OFFSET           10
#define PCIE_TX_CNTL_TX_SNR_OVERRIDE_WIDTH            2
#define PCIE_TX_CNTL_TX_SNR_OVERRIDE_MASK             0xc00
#define PCIE_TX_CNTL_TX_RO_OVERRIDE_OFFSET            12
#define PCIE_TX_CNTL_TX_RO_OVERRIDE_WIDTH             2
#define PCIE_TX_CNTL_TX_RO_OVERRIDE_MASK              0x3000
#define PCIE_TX_CNTL_TX_PACK_PACKET_DIS_OFFSET        14
#define PCIE_TX_CNTL_TX_PACK_PACKET_DIS_WIDTH         1
#define PCIE_TX_CNTL_TX_PACK_PACKET_DIS_MASK          0x4000
#define PCIE_TX_CNTL_TX_FLUSH_TLP_DIS_OFFSET          15
#define PCIE_TX_CNTL_TX_FLUSH_TLP_DIS_WIDTH           1
#define PCIE_TX_CNTL_TX_FLUSH_TLP_DIS_MASK            0x8000
#define PCIE_TX_CNTL_Reserved_19_16_OFFSET            16
#define PCIE_TX_CNTL_Reserved_19_16_WIDTH             4
#define PCIE_TX_CNTL_Reserved_19_16_MASK              0xf0000
#define PCIE_TX_CNTL_TX_CPL_PASS_P_OFFSET             20
#define PCIE_TX_CNTL_TX_CPL_PASS_P_WIDTH              1
#define PCIE_TX_CNTL_TX_CPL_PASS_P_MASK               0x100000
#define PCIE_TX_CNTL_TX_NP_PASS_P_OFFSET              21
#define PCIE_TX_CNTL_TX_NP_PASS_P_WIDTH               1
#define PCIE_TX_CNTL_TX_NP_PASS_P_MASK                0x200000
#define PCIE_TX_CNTL_TX_CLEAR_EXTRA_PM_REQS_OFFSET    22
#define PCIE_TX_CNTL_TX_CLEAR_EXTRA_PM_REQS_WIDTH     1
#define PCIE_TX_CNTL_TX_CLEAR_EXTRA_PM_REQS_MASK      0x400000
#define PCIE_TX_CNTL_TX_FC_UPDATE_TIMEOUT_DIS_OFFSET  23
#define PCIE_TX_CNTL_TX_FC_UPDATE_TIMEOUT_DIS_WIDTH   1
#define PCIE_TX_CNTL_TX_FC_UPDATE_TIMEOUT_DIS_MASK    0x800000
#define PCIE_TX_CNTL_Reserved_31_24_OFFSET            24
#define PCIE_TX_CNTL_Reserved_31_24_WIDTH             8
#define PCIE_TX_CNTL_Reserved_31_24_MASK              0xff000000L

/// PCIE_TX_CNTL
typedef union {
  struct {
    UINT32    Reserved_9_0             : 10;                           ///<
    UINT32    TX_SNR_OVERRIDE          : 2;                            ///<
    UINT32    TX_RO_OVERRIDE           : 2;                            ///<
    UINT32    TX_PACK_PACKET_DIS       : 1;                            ///<
    UINT32    TX_FLUSH_TLP_DIS         : 1;                            ///<
    UINT32    Reserved_19_16           : 4;                            ///<
    UINT32    TX_CPL_PASS_P            : 1;                            ///<
    UINT32    TX_NP_PASS_P             : 1;                            ///<
    UINT32    TX_CLEAR_EXTRA_PM_REQS   : 1;                            ///<
    UINT32    TX_FC_UPDATE_TIMEOUT_DIS : 1;                            ///<
    UINT32    Reserved_31_24           : 8;                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_TX_CNTL_STRUCT;

#define PCIE0_GPP0_ERR_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x401A8)
#define PCIE0_GPP0_ERR_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x401A8)

// Register Name PCIE_RX_CNTL

// Address
#define PCIE_RX_CNTL_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x401C0)
#define PCIE0_GPP0_RX_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x401C0)
#define PCIE0_GPP1_RX_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x411C0)
#define PCIE0_GPP2_RX_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x421C0)
#define PCIE0_GPP3_RX_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x431C0)
#define PCIE0_GPP4_RX_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x441C0)
#define PCIE0_GPP5_RX_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x451C0)
#define PCIE0_GPP6_RX_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x461C0)
#define PCIE0_GPP7_RX_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x471C0)

#define PCIE_RX_CNTL_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x401C0)
#define PCIE0_GPP0_RX_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x401C0)
#define PCIE0_GPP1_RX_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x411C0)
#define PCIE0_GPP2_RX_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x421C0)
#define PCIE0_GPP3_RX_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x431C0)
#define PCIE0_GPP4_RX_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x441C0)
#define PCIE0_GPP5_RX_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x451C0)
#define PCIE0_GPP6_RX_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x461C0)
#define PCIE0_GPP7_RX_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x471C0)

// Type
#define PCIE_RX_CNTL_TYPE                                               TYPE_SMN

#define PCIE_RX_CNTL_RX_IGNORE_IO_ERR_OFFSET                            0
#define PCIE_RX_CNTL_RX_IGNORE_IO_ERR_WIDTH                             1
#define PCIE_RX_CNTL_RX_IGNORE_IO_ERR_MASK                              0x1
#define PCIE_RX_CNTL_RX_IGNORE_BE_ERR_OFFSET                            1
#define PCIE_RX_CNTL_RX_IGNORE_BE_ERR_WIDTH                             1
#define PCIE_RX_CNTL_RX_IGNORE_BE_ERR_MASK                              0x2
#define PCIE_RX_CNTL_RX_IGNORE_MSG_ERR_OFFSET                           2
#define PCIE_RX_CNTL_RX_IGNORE_MSG_ERR_WIDTH                            1
#define PCIE_RX_CNTL_RX_IGNORE_MSG_ERR_MASK                             0x4
#define PCIE_RX_CNTL_RX_IGNORE_CRC_ERR_OFFSET                           3
#define PCIE_RX_CNTL_RX_IGNORE_CRC_ERR_WIDTH                            1
#define PCIE_RX_CNTL_RX_IGNORE_CRC_ERR_MASK                             0x8
#define PCIE_RX_CNTL_RX_IGNORE_CFG_ERR_OFFSET                           4
#define PCIE_RX_CNTL_RX_IGNORE_CFG_ERR_WIDTH                            1
#define PCIE_RX_CNTL_RX_IGNORE_CFG_ERR_MASK                             0x10
#define PCIE_RX_CNTL_RX_IGNORE_CPL_ERR_OFFSET                           5
#define PCIE_RX_CNTL_RX_IGNORE_CPL_ERR_WIDTH                            1
#define PCIE_RX_CNTL_RX_IGNORE_CPL_ERR_MASK                             0x20
#define PCIE_RX_CNTL_RX_IGNORE_EP_ERR_OFFSET                            6
#define PCIE_RX_CNTL_RX_IGNORE_EP_ERR_WIDTH                             1
#define PCIE_RX_CNTL_RX_IGNORE_EP_ERR_MASK                              0x40
#define PCIE_RX_CNTL_RX_IGNORE_LEN_MISMATCH_ERR_OFFSET                  7
#define PCIE_RX_CNTL_RX_IGNORE_LEN_MISMATCH_ERR_WIDTH                   1
#define PCIE_RX_CNTL_RX_IGNORE_LEN_MISMATCH_ERR_MASK                    0x80
#define PCIE_RX_CNTL_RX_IGNORE_MAX_PAYLOAD_ERR_OFFSET                   8
#define PCIE_RX_CNTL_RX_IGNORE_MAX_PAYLOAD_ERR_WIDTH                    1
#define PCIE_RX_CNTL_RX_IGNORE_MAX_PAYLOAD_ERR_MASK                     0x100
#define PCIE_RX_CNTL_RX_IGNORE_TC_ERR_OFFSET                            9
#define PCIE_RX_CNTL_RX_IGNORE_TC_ERR_WIDTH                             1
#define PCIE_RX_CNTL_RX_IGNORE_TC_ERR_MASK                              0x200
#define PCIE_RX_CNTL_RX_IGNORE_CFG_UR_OFFSET                            10
#define PCIE_RX_CNTL_RX_IGNORE_CFG_UR_WIDTH                             1
#define PCIE_RX_CNTL_RX_IGNORE_CFG_UR_MASK                              0x400
#define PCIE_RX_CNTL_RX_IGNORE_IO_UR_OFFSET                             11
#define PCIE_RX_CNTL_RX_IGNORE_IO_UR_WIDTH                              1
#define PCIE_RX_CNTL_RX_IGNORE_IO_UR_MASK                               0x800
#define PCIE_RX_CNTL_RX_IGNORE_AT_ERR_OFFSET                            12
#define PCIE_RX_CNTL_RX_IGNORE_AT_ERR_WIDTH                             1
#define PCIE_RX_CNTL_RX_IGNORE_AT_ERR_MASK                              0x1000
#define PCIE_RX_CNTL_RX_NAK_IF_FIFO_FULL_OFFSET                         13
#define PCIE_RX_CNTL_RX_NAK_IF_FIFO_FULL_WIDTH                          1
#define PCIE_RX_CNTL_RX_NAK_IF_FIFO_FULL_MASK                           0x2000
#define PCIE_RX_CNTL_RX_GEN_ONE_NAK_OFFSET                              14
#define PCIE_RX_CNTL_RX_GEN_ONE_NAK_WIDTH                               1
#define PCIE_RX_CNTL_RX_GEN_ONE_NAK_MASK                                0x4000
#define PCIE_RX_CNTL_RX_FC_INIT_FROM_REG_OFFSET                         15
#define PCIE_RX_CNTL_RX_FC_INIT_FROM_REG_WIDTH                          1
#define PCIE_RX_CNTL_RX_FC_INIT_FROM_REG_MASK                           0x8000
#define PCIE_RX_CNTL_RX_RCB_CPL_TIMEOUT_OFFSET                          16
#define PCIE_RX_CNTL_RX_RCB_CPL_TIMEOUT_WIDTH                           3
#define PCIE_RX_CNTL_RX_RCB_CPL_TIMEOUT_MASK                            0x70000
#define PCIE_RX_CNTL_RX_RCB_CPL_TIMEOUT_MODE_OFFSET                     19
#define PCIE_RX_CNTL_RX_RCB_CPL_TIMEOUT_MODE_WIDTH                      1
#define PCIE_RX_CNTL_RX_RCB_CPL_TIMEOUT_MODE_MASK                       0x80000
#define PCIE_RX_CNTL_RX_PCIE_CPL_TIMEOUT_DIS_OFFSET                     20
#define PCIE_RX_CNTL_RX_PCIE_CPL_TIMEOUT_DIS_WIDTH                      1
#define PCIE_RX_CNTL_RX_PCIE_CPL_TIMEOUT_DIS_MASK                       0x100000
#define PCIE_RX_CNTL_RX_IGNORE_SHORTPREFIX_ERR_OFFSET                   21
#define PCIE_RX_CNTL_RX_IGNORE_SHORTPREFIX_ERR_WIDTH                    1
#define PCIE_RX_CNTL_RX_IGNORE_SHORTPREFIX_ERR_MASK                     0x200000
#define PCIE_RX_CNTL_RX_IGNORE_MAXPREFIX_ERR_OFFSET                     22
#define PCIE_RX_CNTL_RX_IGNORE_MAXPREFIX_ERR_WIDTH                      1
#define PCIE_RX_CNTL_RX_IGNORE_MAXPREFIX_ERR_MASK                       0x400000
#define PCIE_RX_CNTL_RX_IGNORE_CPLPREFIX_ERR_OFFSET                     23
#define PCIE_RX_CNTL_RX_IGNORE_CPLPREFIX_ERR_WIDTH                      1
#define PCIE_RX_CNTL_RX_IGNORE_CPLPREFIX_ERR_MASK                       0x800000
#define PCIE_RX_CNTL_RX_IGNORE_INVALIDPASID_ERR_OFFSET                  24
#define PCIE_RX_CNTL_RX_IGNORE_INVALIDPASID_ERR_WIDTH                   1
#define PCIE_RX_CNTL_RX_IGNORE_INVALIDPASID_ERR_MASK                    0x1000000
#define PCIE_RX_CNTL_RX_IGNORE_NOT_PASID_UR_OFFSET                      25
#define PCIE_RX_CNTL_RX_IGNORE_NOT_PASID_UR_WIDTH                       1
#define PCIE_RX_CNTL_RX_IGNORE_NOT_PASID_UR_MASK                        0x2000000
#define PCIE_RX_CNTL_RX_TPH_DIS_OFFSET                                  26
#define PCIE_RX_CNTL_RX_TPH_DIS_WIDTH                                   1
#define PCIE_RX_CNTL_RX_TPH_DIS_MASK                                    0x4000000
#define PCIE_RX_CNTL_RX_RCB_FLR_TIMEOUT_DIS_OFFSET                      27
#define PCIE_RX_CNTL_RX_RCB_FLR_TIMEOUT_DIS_WIDTH                       1
#define PCIE_RX_CNTL_RX_RCB_FLR_TIMEOUT_DIS_MASK                        0x8000000
#define PCIE_RX_RCB_ST1_DAT_MEM_OVERFLOW_WATERMARK_OFFSET               28
#define PCIE_RX_RCB_ST1_DAT_MEM_OVERFLOW_WATERMARK_WIDTH                4
#define PCIE_RX_RCB_ST1_DAT_MEM_OVERFLOW_WATERMARK_MASK                 0xf0000000L

/// PCIE_RX_CNTL
typedef union {
  struct {
    UINT32    RX_IGNORE_IO_ERR                          : 1;                          ///<
    UINT32    RX_IGNORE_BE_ERR                          : 1;                          ///<
    UINT32    RX_IGNORE_MSG_ERR                         : 1;                          ///<
    UINT32    RX_IGNORE_CRC_ERR                         : 1;                          ///<
    UINT32    RX_IGNORE_CFG_ERR                         : 1;                          ///<
    UINT32    RX_IGNORE_CPL_ERR                         : 1;                          ///<
    UINT32    RX_IGNORE_EP_ERR                          : 1;                          ///<
    UINT32    RX_IGNORE_LEN_MISMATCH_ERR                : 1;                          ///<
    UINT32    RX_IGNORE_MAX_PAYLOAD_ERR                 : 1;                          ///<
    UINT32    RX_IGNORE_TC_ERR                          : 1;                          ///<
    UINT32    RX_IGNORE_CFG_UR                          : 1;                          ///<
    UINT32    RX_IGNORE_IO_UR                           : 1;                          ///<
    UINT32    RX_IGNORE_AT_ERR                          : 1;                          ///<
    UINT32    RX_NAK_IF_FIFO_FULL                       : 1;                          ///<
    UINT32    RX_GEN_ONE_NAK                            : 1;                          ///<
    UINT32    RX_FC_INIT_FROM_REG                       : 1;                          ///<
    UINT32    RX_RCB_CPL_TIMEOUT                        : 3;                          ///<
    UINT32    RX_RCB_CPL_TIMEOUT_MODE                   : 1;                          ///<
    UINT32    RX_PCIE_CPL_TIMEOUT_DIS                   : 1;                          ///<
    UINT32    RX_IGNORE_SHORTPREFIX_ERR                 : 1;                          ///<
    UINT32    RX_IGNORE_MAXPREFIX_ERR                   : 1;                          ///<
    UINT32    RX_IGNORE_CPLPREFIX_ERR                   : 1;                          ///<
    UINT32    RX_IGNORE_INVALIDPASID_ERR                : 1;                          ///<
    UINT32    RX_IGNORE_NOT_PASID_UR                    : 1;                          ///<
    UINT32    RX_TPH_DIS                                : 1;                          ///<
    UINT32    RX_RCB_FLR_TIMEOUT_DIS                    : 1;                          ///<
    UINT32    RX_RCB_ST1_DAT_MEM_OVERFLOW_WATERMARK     : 4;                          ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_RX_CNTL_STRUCT;

// Address
#define PCIE0_GPP0_LC_TRAINING_CNTL_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40284)
#define PCIE1_GPP0_LC_TRAINING_CNTL_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40284)
#define PCIE2_GPP0_LC_TRAINING_CNTL_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40284)

#define PCIE0_GPP0_LC_TRAINING_CNTL_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40284)
#define PCIE2_GPP0_LC_TRAINING_CNTL_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40284)
// Type
#define PCIE0_GPP0_LC_TRAINING_CNTL_TYPE                                 TYPE_SMN

#define PCIE0_GPP0_LC_TRAINING_CNTL_LC_DISABLE_TRAINING_BIT_ARCH_OFFSET  13
#define PCIE0_GPP0_LC_TRAINING_CNTL_LC_DISABLE_TRAINING_BIT_ARCH_WIDTH   1
#define PCIE0_GPP0_LC_TRAINING_CNTL_LC_DISABLE_TRAINING_BIT_ARCH_MASK    0x2000

// Register Name PCIE_LC_CNTL2

// Address
#define PCIE0_PCIE_LC_CNTL2_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x402c4)
#define PCIE1_PCIE_LC_CNTL2_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x402c4)
#define PCIE2_PCIE_LC_CNTL2_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x402c4)

#define PCIE0_PCIE_LC_CNTL2_ADDRESS_HYGX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x402c4)
#define PCIE2_PCIE_LC_CNTL2_ADDRESS_HYGX                    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x402c4)

// Type
#define PCIE_LC_CNTL2_TYPE                                        TYPE_SMN

#define PCIE_LC_CNTL2_LC_TIMED_OUT_STATE_OFFSET                   0
#define PCIE_LC_CNTL2_LC_TIMED_OUT_STATE_WIDTH                    6
#define PCIE_LC_CNTL2_LC_TIMED_OUT_STATE_MASK                     0x3f
#define PCIE_LC_CNTL2_LC_STATE_TIMED_OUT_OFFSET                   6
#define PCIE_LC_CNTL2_LC_STATE_TIMED_OUT_WIDTH                    1
#define PCIE_LC_CNTL2_LC_STATE_TIMED_OUT_MASK                     0x40
#define PCIE_LC_CNTL2_LC_LOOK_FOR_BW_REDUCTION_OFFSET             7
#define PCIE_LC_CNTL2_LC_LOOK_FOR_BW_REDUCTION_WIDTH              1
#define PCIE_LC_CNTL2_LC_LOOK_FOR_BW_REDUCTION_MASK               0x80
#define PCIE_LC_CNTL2_LC_MORE_TS2_EN_OFFSET                       8
#define PCIE_LC_CNTL2_LC_MORE_TS2_EN_WIDTH                        1
#define PCIE_LC_CNTL2_LC_MORE_TS2_EN_MASK                         0x100
#define PCIE_LC_CNTL2_LC_X12_NEGOTIATION_DIS_OFFSET               9
#define PCIE_LC_CNTL2_LC_X12_NEGOTIATION_DIS_WIDTH                1
#define PCIE_LC_CNTL2_LC_X12_NEGOTIATION_DIS_MASK                 0x200
#define PCIE_LC_CNTL2_LC_LINK_UP_REVERSAL_EN_OFFSET               10
#define PCIE_LC_CNTL2_LC_LINK_UP_REVERSAL_EN_WIDTH                1
#define PCIE_LC_CNTL2_LC_LINK_UP_REVERSAL_EN_MASK                 0x400
#define PCIE_LC_CNTL2_LC_ILLEGAL_STATE_OFFSET                     11
#define PCIE_LC_CNTL2_LC_ILLEGAL_STATE_WIDTH                      1
#define PCIE_LC_CNTL2_LC_ILLEGAL_STATE_MASK                       0x800
#define PCIE_LC_CNTL2_LC_ILLEGAL_STATE_RESTART_EN_OFFSET          12
#define PCIE_LC_CNTL2_LC_ILLEGAL_STATE_RESTART_EN_WIDTH           1
#define PCIE_LC_CNTL2_LC_ILLEGAL_STATE_RESTART_EN_MASK            0x1000
#define PCIE_LC_CNTL2_LC_WAIT_FOR_OTHER_LANES_MODE_OFFSET         13
#define PCIE_LC_CNTL2_LC_WAIT_FOR_OTHER_LANES_MODE_WIDTH          1
#define PCIE_LC_CNTL2_LC_WAIT_FOR_OTHER_LANES_MODE_MASK           0x2000
#define PCIE_LC_CNTL2_LC_ELEC_IDLE_MODE_OFFSET                    14
#define PCIE_LC_CNTL2_LC_ELEC_IDLE_MODE_WIDTH                     2
#define PCIE_LC_CNTL2_LC_ELEC_IDLE_MODE_MASK                      0xc000
#define PCIE_LC_CNTL2_LC_DISABLE_INFERRED_ELEC_IDLE_DET_OFFSET    16
#define PCIE_LC_CNTL2_LC_DISABLE_INFERRED_ELEC_IDLE_DET_WIDTH     1
#define PCIE_LC_CNTL2_LC_DISABLE_INFERRED_ELEC_IDLE_DET_MASK      0x10000
#define PCIE_LC_CNTL2_LC_ALLOW_PDWN_IN_L1_OFFSET                  17
#define PCIE_LC_CNTL2_LC_ALLOW_PDWN_IN_L1_WIDTH                   1
#define PCIE_LC_CNTL2_LC_ALLOW_PDWN_IN_L1_MASK                    0x20000
#define PCIE_LC_CNTL2_LC_ALLOW_PDWN_IN_L23_OFFSET                 18
#define PCIE_LC_CNTL2_LC_ALLOW_PDWN_IN_L23_WIDTH                  1
#define PCIE_LC_CNTL2_LC_ALLOW_PDWN_IN_L23_MASK                   0x40000
#define PCIE_LC_CNTL2_LC_DEASSERT_RX_EN_IN_L0S_OFFSET             19
#define PCIE_LC_CNTL2_LC_DEASSERT_RX_EN_IN_L0S_WIDTH              1
#define PCIE_LC_CNTL2_LC_DEASSERT_RX_EN_IN_L0S_MASK               0x80000
#define PCIE_LC_CNTL2_LC_BLOCK_EL_IDLE_IN_L0_OFFSET               20
#define PCIE_LC_CNTL2_LC_BLOCK_EL_IDLE_IN_L0_WIDTH                1
#define PCIE_LC_CNTL2_LC_BLOCK_EL_IDLE_IN_L0_MASK                 0x100000
#define PCIE_LC_CNTL2_LC_RCV_L0_TO_RCV_L0S_DIS_OFFSET             21
#define PCIE_LC_CNTL2_LC_RCV_L0_TO_RCV_L0S_DIS_WIDTH              1
#define PCIE_LC_CNTL2_LC_RCV_L0_TO_RCV_L0S_DIS_MASK               0x200000
#define PCIE_LC_CNTL2_LC_ASSERT_INACTIVE_DURING_HOLD_OFFSET       22
#define PCIE_LC_CNTL2_LC_ASSERT_INACTIVE_DURING_HOLD_WIDTH        1
#define PCIE_LC_CNTL2_LC_ASSERT_INACTIVE_DURING_HOLD_MASK         0x400000
#define PCIE_LC_CNTL2_LC_WAIT_FOR_LANES_IN_LW_NEG_OFFSET          23
#define PCIE_LC_CNTL2_LC_WAIT_FOR_LANES_IN_LW_NEG_WIDTH           2
#define PCIE_LC_CNTL2_LC_WAIT_FOR_LANES_IN_LW_NEG_MASK            0x1800000
#define PCIE_LC_CNTL2_LC_PWR_DOWN_NEG_OFF_LANES_OFFSET            25
#define PCIE_LC_CNTL2_LC_PWR_DOWN_NEG_OFF_LANES_WIDTH             1
#define PCIE_LC_CNTL2_LC_PWR_DOWN_NEG_OFF_LANES_MASK              0x2000000
#define PCIE_LC_CNTL2_LC_DISABLE_LOST_SYM_LOCK_ARCS_OFFSET        26
#define PCIE_LC_CNTL2_LC_DISABLE_LOST_SYM_LOCK_ARCS_WIDTH         1
#define PCIE_LC_CNTL2_LC_DISABLE_LOST_SYM_LOCK_ARCS_MASK          0x4000000
#define PCIE_LC_CNTL2_LC_LINK_BW_NOTIFICATION_DIS_OFFSET          27
#define PCIE_LC_CNTL2_LC_LINK_BW_NOTIFICATION_DIS_WIDTH           1
#define PCIE_LC_CNTL2_LC_LINK_BW_NOTIFICATION_DIS_MASK            0x8000000
#define PCIE_LC_CNTL2_LC_PMI_L1_WAIT_FOR_SLV_IDLE_OFFSET          28
#define PCIE_LC_CNTL2_LC_PMI_L1_WAIT_FOR_SLV_IDLE_WIDTH           1
#define PCIE_LC_CNTL2_LC_PMI_L1_WAIT_FOR_SLV_IDLE_MASK            0x10000000
#define PCIE_LC_CNTL2_LC_TEST_TIMER_SEL_OFFSET                    29
#define PCIE_LC_CNTL2_LC_TEST_TIMER_SEL_WIDTH                     2
#define PCIE_LC_CNTL2_LC_TEST_TIMER_SEL_MASK                      0x60000000
#define PCIE_LC_CNTL2_LC_ENABLE_INFERRED_ELEC_IDLE_FOR_PI_OFFSET  31
#define PCIE_LC_CNTL2_LC_ENABLE_INFERRED_ELEC_IDLE_FOR_PI_WIDTH   1
#define PCIE_LC_CNTL2_LC_ENABLE_INFERRED_ELEC_IDLE_FOR_PI_MASK    0x80000000L

/// PCIEPORT0F0_000002C4
typedef union {
  struct {
    UINT32    LC_TIMED_OUT_STATE                  : 6;                 ///<
    UINT32    LC_STATE_TIMED_OUT                  : 1;                 ///<
    UINT32    LC_LOOK_FOR_BW_REDUCTION            : 1;                 ///<
    UINT32    LC_MORE_TS2_EN                      : 1;                 ///<
    UINT32    LC_X12_NEGOTIATION_DIS              : 1;                 ///<
    UINT32    LC_LINK_UP_REVERSAL_EN              : 1;                 ///<
    UINT32    LC_ILLEGAL_STATE                    : 1;                 ///<
    UINT32    LC_ILLEGAL_STATE_RESTART_EN         : 1;                 ///<
    UINT32    LC_WAIT_FOR_OTHER_LANES_MODE        : 1;                 ///<
    UINT32    LC_ELEC_IDLE_MODE                   : 2;                 ///<
    UINT32    LC_DISABLE_INFERRED_ELEC_IDLE_DET   : 1;                 ///<
    UINT32    LC_ALLOW_PDWN_IN_L1                 : 1;                 ///<
    UINT32    LC_ALLOW_PDWN_IN_L23                : 1;                 ///<
    UINT32    LC_DEASSERT_RX_EN_IN_L0S            : 1;                 ///<
    UINT32    LC_BLOCK_EL_IDLE_IN_L0              : 1;                 ///<
    UINT32    LC_RCV_L0_TO_RCV_L0S_DIS            : 1;                 ///<
    UINT32    LC_ASSERT_INACTIVE_DURING_HOLD      : 1;                 ///<
    UINT32    LC_WAIT_FOR_LANES_IN_LW_NEG         : 2;                 ///<
    UINT32    LC_PWR_DOWN_NEG_OFF_LANES           : 1;                 ///<
    UINT32    LC_DISABLE_LOST_SYM_LOCK_ARCS       : 1;                 ///<
    UINT32    LC_LINK_BW_NOTIFICATION_DIS         : 1;                 ///<
    UINT32    LC_PMI_L1_WAIT_FOR_SLV_IDLE         : 1;                 ///<
    UINT32    LC_TEST_TIMER_SEL                   : 2;                 ///<
    UINT32    LC_ENABLE_INFERRED_ELEC_IDLE_FOR_PI : 1;                 ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_CNTL2_STRUCT;

// Register Name PCIE_LC_CNTL3

// Address
#define PCIE_LC_CNTL3_PCI_OFFSET  0x000002d4

#define PCIE0_GPP0_LC_CNTL3_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x402d4)
#define PCIE0_GPP1_LC_CNTL3_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x412d4)
#define PCIE0_GPP2_LC_CNTL3_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x422d4)
#define PCIE0_GPP3_LC_CNTL3_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x432d4)
#define PCIE0_GPP4_LC_CNTL3_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x442d4)
#define PCIE0_GPP5_LC_CNTL3_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x452d4)
#define PCIE0_GPP6_LC_CNTL3_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x462d4)
#define PCIE0_GPP7_LC_CNTL3_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x472d4)

#define PCIE0_GPP0_LC_CNTL3_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x402d4)
#define PCIE0_GPP1_LC_CNTL3_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x412d4)
#define PCIE0_GPP2_LC_CNTL3_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x422d4)
#define PCIE0_GPP3_LC_CNTL3_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x432d4)
#define PCIE0_GPP4_LC_CNTL3_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x442d4)
#define PCIE0_GPP5_LC_CNTL3_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x452d4)
#define PCIE0_GPP6_LC_CNTL3_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x462d4)
#define PCIE0_GPP7_LC_CNTL3_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x472d4)

// Type
#define PCIE_LC_CNTL3_TYPE                                              TYPE_SMN

#define PCIE_LC_CNTL3_LC_SELECT_DEEMPHASIS_OFFSET                        0
#define PCIE_LC_CNTL3_LC_SELECT_DEEMPHASIS_WIDTH                         1
#define PCIE_LC_CNTL3_LC_SELECT_DEEMPHASIS_MASK                          0x1
#define PCIE_LC_CNTL3_LC_SELECT_DEEMPHASIS_CNTL_OFFSET                   1
#define PCIE_LC_CNTL3_LC_SELECT_DEEMPHASIS_CNTL_WIDTH                    2
#define PCIE_LC_CNTL3_LC_SELECT_DEEMPHASIS_CNTL_MASK                     0x6
#define PCIE_LC_CNTL3_LC_RCVD_DEEMPHASIS_OFFSET                          3
#define PCIE_LC_CNTL3_LC_RCVD_DEEMPHASIS_WIDTH                           1
#define PCIE_LC_CNTL3_LC_RCVD_DEEMPHASIS_MASK                            0x8
#define PCIE_LC_CNTL3_LC_COMP_TO_DETECT_OFFSET                           4
#define PCIE_LC_CNTL3_LC_COMP_TO_DETECT_WIDTH                            1
#define PCIE_LC_CNTL3_LC_COMP_TO_DETECT_MASK                             0x10
#define PCIE_LC_CNTL3_LC_RESET_TSX_CNT_IN_RLOCK_EN_OFFSET                5
#define PCIE_LC_CNTL3_LC_RESET_TSX_CNT_IN_RLOCK_EN_WIDTH                 1
#define PCIE_LC_CNTL3_LC_RESET_TSX_CNT_IN_RLOCK_EN_MASK                  0x20
#define PCIE_LC_CNTL3_LC_AUTO_SPEED_CHANGE_ATTEMPTS_ALLOWED_OFFSET       6
#define PCIE_LC_CNTL3_LC_AUTO_SPEED_CHANGE_ATTEMPTS_ALLOWED_WIDTH        2
#define PCIE_LC_CNTL3_LC_AUTO_SPEED_CHANGE_ATTEMPTS_ALLOWED_MASK         0xc0
#define PCIE_LC_CNTL3_LC_AUTO_SPEED_CHANGE_ATTEMPT_FAILED_OFFSET         8
#define PCIE_LC_CNTL3_LC_AUTO_SPEED_CHANGE_ATTEMPT_FAILED_WIDTH          1
#define PCIE_LC_CNTL3_LC_AUTO_SPEED_CHANGE_ATTEMPT_FAILED_MASK           0x100
#define PCIE_LC_CNTL3_LC_CLR_FAILED_AUTO_SPD_CHANGE_CNT_OFFSET           9
#define PCIE_LC_CNTL3_LC_CLR_FAILED_AUTO_SPD_CHANGE_CNT_WIDTH            1
#define PCIE_LC_CNTL3_LC_CLR_FAILED_AUTO_SPD_CHANGE_CNT_MASK             0x200
#define PCIE_LC_CNTL3_LC_ENHANCED_HOT_PLUG_EN_OFFSET                     10
#define PCIE_LC_CNTL3_LC_ENHANCED_HOT_PLUG_EN_WIDTH                      1
#define PCIE_LC_CNTL3_LC_ENHANCED_HOT_PLUG_EN_MASK                       0x400
#define PCIE_LC_CNTL3_LC_RCVR_DET_EN_OVERRIDE_OFFSET                     11
#define PCIE_LC_CNTL3_LC_RCVR_DET_EN_OVERRIDE_WIDTH                      1
#define PCIE_LC_CNTL3_LC_RCVR_DET_EN_OVERRIDE_MASK                       0x800
#define PCIE_LC_CNTL3_LC_EHP_RX_PHY_CMD_OFFSET                           12
#define PCIE_LC_CNTL3_LC_EHP_RX_PHY_CMD_WIDTH                            2
#define PCIE_LC_CNTL3_LC_EHP_RX_PHY_CMD_MASK                             0x3000
#define PCIE_LC_CNTL3_LC_EHP_TX_PHY_CMD_OFFSET                           14
#define PCIE_LC_CNTL3_LC_EHP_TX_PHY_CMD_WIDTH                            2
#define PCIE_LC_CNTL3_LC_EHP_TX_PHY_CMD_MASK                             0xc000
#define PCIE_LC_CNTL3_LC_CHIP_BIF_USB_IDLE_EN_OFFSET                     16
#define PCIE_LC_CNTL3_LC_CHIP_BIF_USB_IDLE_EN_WIDTH                      1
#define PCIE_LC_CNTL3_LC_CHIP_BIF_USB_IDLE_EN_MASK                       0x10000
#define PCIE_LC_CNTL3_LC_L1_BLOCK_RECONFIG_EN_OFFSET                     17
#define PCIE_LC_CNTL3_LC_L1_BLOCK_RECONFIG_EN_WIDTH                      1
#define PCIE_LC_CNTL3_LC_L1_BLOCK_RECONFIG_EN_MASK                       0x20000
#define PCIE_LC_CNTL3_LC_AUTO_DISABLE_SPEED_SUPPORT_EN_OFFSET            18
#define PCIE_LC_CNTL3_LC_AUTO_DISABLE_SPEED_SUPPORT_EN_WIDTH             1
#define PCIE_LC_CNTL3_LC_AUTO_DISABLE_SPEED_SUPPORT_EN_MASK              0x40000
#define PCIE_LC_CNTL3_LC_AUTO_DISABLE_SPEED_SUPPORT_MAX_FAIL_SEL_OFFSET  19
#define PCIE_LC_CNTL3_LC_AUTO_DISABLE_SPEED_SUPPORT_MAX_FAIL_SEL_WIDTH   2
#define PCIE_LC_CNTL3_LC_AUTO_DISABLE_SPEED_SUPPORT_MAX_FAIL_SEL_MASK    0x180000
#define PCIE_LC_CNTL3_LC_FAST_L1_ENTRY_EXIT_EN_OFFSET                    21
#define PCIE_LC_CNTL3_LC_FAST_L1_ENTRY_EXIT_EN_WIDTH                     1
#define PCIE_LC_CNTL3_LC_FAST_L1_ENTRY_EXIT_EN_MASK                      0x200000
#define PCIE_LC_CNTL3_LC_RXPHYCMD_INACTIVE_EN_MODE_OFFSET                22
#define PCIE_LC_CNTL3_LC_RXPHYCMD_INACTIVE_EN_MODE_WIDTH                 1
#define PCIE_LC_CNTL3_LC_RXPHYCMD_INACTIVE_EN_MODE_MASK                  0x400000
#define PCIE_LC_CNTL3_LC_DSC_DONT_ENTER_L23_AFTER_PME_ACK_OFFSET         23
#define PCIE_LC_CNTL3_LC_DSC_DONT_ENTER_L23_AFTER_PME_ACK_WIDTH          1
#define PCIE_LC_CNTL3_LC_DSC_DONT_ENTER_L23_AFTER_PME_ACK_MASK           0x800000
#define PCIE_LC_CNTL3_LC_HW_VOLTAGE_IF_CONTROL_OFFSET                    24
#define PCIE_LC_CNTL3_LC_HW_VOLTAGE_IF_CONTROL_WIDTH                     2
#define PCIE_LC_CNTL3_LC_HW_VOLTAGE_IF_CONTROL_MASK                      0x3000000
#define PCIE_LC_CNTL3_LC_VOLTAGE_TIMER_SEL_OFFSET                        26
#define PCIE_LC_CNTL3_LC_VOLTAGE_TIMER_SEL_WIDTH                         4
#define PCIE_LC_CNTL3_LC_VOLTAGE_TIMER_SEL_MASK                          0x3c000000
#define PCIE_LC_CNTL3_LC_GO_TO_RECOVERY_OFFSET                           30
#define PCIE_LC_CNTL3_LC_GO_TO_RECOVERY_WIDTH                            1
#define PCIE_LC_CNTL3_LC_GO_TO_RECOVERY_MASK                             0x40000000
#define PCIE_LC_CNTL3_LC_N_EIE_SEL_OFFSET                                31
#define PCIE_LC_CNTL3_LC_N_EIE_SEL_WIDTH                                 1
#define PCIE_LC_CNTL3_LC_N_EIE_SEL_MASK                                  0x80000000L

/// PCIE_LC_CNTL3
typedef union {
  struct {
    UINT32    LC_SELECT_DEEMPHASIS                       : 1;          ///<
    UINT32    LC_SELECT_DEEMPHASIS_CNTL                  : 2;          ///<
    UINT32    LC_RCVD_DEEMPHASIS                         : 1;          ///<
    UINT32    LC_COMP_TO_DETECT                          : 1;          ///<
    UINT32    LC_RESET_TSX_CNT_IN_RLOCK_EN               : 1;          ///<
    UINT32    LC_AUTO_SPEED_CHANGE_ATTEMPTS_ALLOWED      : 2;          ///<
    UINT32    LC_AUTO_SPEED_CHANGE_ATTEMPT_FAILED        : 1;          ///<
    UINT32    LC_CLR_FAILED_AUTO_SPD_CHANGE_CNT          : 1;          ///<
    UINT32    LC_ENHANCED_HOT_PLUG_EN                    : 1;          ///<
    UINT32    LC_RCVR_DET_EN_OVERRIDE                    : 1;          ///<
    UINT32    LC_EHP_RX_PHY_CMD                          : 2;          ///<
    UINT32    LC_EHP_TX_PHY_CMD                          : 2;          ///<
    UINT32    LC_CHIP_BIF_USB_IDLE_EN                    : 1;          ///<
    UINT32    LC_L1_BLOCK_RECONFIG_EN                    : 1;          ///<
    UINT32    LC_AUTO_DISABLE_SPEED_SUPPORT_EN           : 1;          ///<
    UINT32    LC_AUTO_DISABLE_SPEED_SUPPORT_MAX_FAIL_SEL : 2;          ///<
    UINT32    LC_FAST_L1_ENTRY_EXIT_EN                   : 1;          ///<
    UINT32    LC_RXPHYCMD_INACTIVE_EN_MODE               : 1;          ///<
    UINT32    LC_DSC_DONT_ENTER_L23_AFTER_PME_ACK        : 1;          ///<
    UINT32    LC_HW_VOLTAGE_IF_CONTROL                   : 2;          ///<
    UINT32    LC_VOLTAGE_TIMER_SEL                       : 4;          ///<
    UINT32    LC_GO_TO_RECOVERY                          : 1;          ///<
    UINT32    LC_N_EIE_SEL                               : 1;          ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_CNTL3_STRUCT;

// Register Name PCIE_LC_FORCE_COEFF

// Address
#define PCIE0_LC_FORCE_COEFF_ADDRESS_HYEX                   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x402e0)
#define PCIE0_LC_FORCE_COEFF_ADDRESS_HYGX                   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x402e0)

// Type
#define PCIE_LC_FORCE_COEFF_TYPE                           TYPE_SMN

#define PCIE_LC_FORCE_COEFF_LC_FORCE_COEFF_OFFSET          0
#define PCIE_LC_FORCE_COEFF_LC_FORCE_COEFF_WIDTH           1
#define PCIE_LC_FORCE_COEFF_LC_FORCE_COEFF_MASK            0x1
#define PCIE_LC_FORCE_COEFF_LC_FORCE_PRE_CURSOR_OFFSET     1
#define PCIE_LC_FORCE_COEFF_LC_FORCE_PRE_CURSOR_WIDTH      6
#define PCIE_LC_FORCE_COEFF_LC_FORCE_PRE_CURSOR_MASK       0x7e
#define PCIE_LC_FORCE_COEFF_LC_FORCE_CURSOR_OFFSET         7
#define PCIE_LC_FORCE_COEFF_LC_FORCE_CURSOR_WIDTH          6
#define PCIE_LC_FORCE_COEFF_LC_FORCE_CURSOR_MASK           0x1f80
#define PCIE_LC_FORCE_COEFF_LC_FORCE_POST_CURSOR_OFFSET    13
#define PCIE_LC_FORCE_COEFF_LC_FORCE_POST_CURSOR_WIDTH     6
#define PCIE_LC_FORCE_COEFF_LC_FORCE_POST_CURSOR_MASK      0x7e000
#define PCIE_LC_FORCE_COEFF_LC_3X3_COEFF_SEARCH_EN_OFFSET  19
#define PCIE_LC_FORCE_COEFF_LC_3X3_COEFF_SEARCH_EN_WIDTH   1
#define PCIE_LC_FORCE_COEFF_LC_3X3_COEFF_SEARCH_EN_MASK    0x80000
#define PCIE_LC_FORCE_COEFF_LC_PRESET_10_EN_OFFSET         20
#define PCIE_LC_FORCE_COEFF_LC_PRESET_10_EN_WIDTH          1
#define PCIE_LC_FORCE_COEFF_LC_PRESET_10_EN_MASK           0x100000
#define PCIE_LC_FORCE_COEFF_Reserved_31_21_OFFSET          21
#define PCIE_LC_FORCE_COEFF_Reserved_31_21_WIDTH           11
#define PCIE_LC_FORCE_COEFF_Reserved_31_21_MASK            0xffe00000L

/// PCIEPORT0F0_000002E0
typedef union {
  struct {
    UINT32    LC_FORCE_COEFF         : 1;                              ///<
    UINT32    LC_FORCE_PRE_CURSOR    : 6;                              ///<
    UINT32    LC_FORCE_CURSOR        : 6;                              ///<
    UINT32    LC_FORCE_POST_CURSOR   : 6;                              ///<
    UINT32    LC_3X3_COEFF_SEARCH_EN : 1;                              ///<
    UINT32    LC_PRESET_10_EN        : 1;                              ///<
    UINT32    Reserved_31_21         : 11;                             ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_FORCE_COEFF_STRUCT;

// Register Name PCIE_LC_FORCE_COEFF2

// Address
#define PCIE0_LC_FORCE_COEFF2_ADDRESS_HYEX                           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x4037c)
#define PCIE1_LC_FORCE_COEFF2_ADDRESS_HYEX                           (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x4037c)
#define PCIE2_LC_FORCE_COEFF2_ADDRESS_HYEX                           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x4037c)

#define PCIE0_LC_FORCE_COEFF2_ADDRESS_HYGX                           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x4037c)
#define PCIE2_LC_FORCE_COEFF2_ADDRESS_HYGX                           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x4037c)
// Type
#define PCIE_LC_FORCE_COEFF2_TYPE                                    TYPE_SMN

#define PCIE_LC_FORCE_COEFF2_LC_FORCE_COEFF_OFFSET                   0
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_COEFF_WIDTH                    1
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_COEFF_MASK                     0x1
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_PRE_CURSOR_OFFSET              1
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_PRE_CURSOR_WIDTH               6
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_PRE_CURSOR_MASK                0x7e
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_CURSOR_OFFSET                  7
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_CURSOR_WIDTH                   6
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_CURSOR_MASK                    0x1f80
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_POST_CURSOR_OFFSET             13
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_POST_CURSOR_WIDTH              6
#define PCIE_LC_FORCE_COEFF2_LC_FORCE_POST_CURSOR_MASK               0x7e000
#define PCIE_LC_FORCE_COEFF2_LC_3X3_COEFF_SEARCH_EN_OFFSET           19
#define PCIE_LC_FORCE_COEFF2_LC_3X3_COEFF_SEARCH_EN_WIDTH            1
#define PCIE_LC_FORCE_COEFF2_LC_3X3_COEFF_SEARCH_EN_MASK             0x80000

/// PCIEPORT0F0_0000037C
typedef union {
  struct {
    UINT32                                     LC_FORCE_COEFF_16GT:1 ; ///<
    UINT32                                LC_FORCE_PRE_CURSOR_16GT:6 ; ///<
    UINT32                                    LC_FORCE_CURSOR_16GT:6 ; ///<
    UINT32                               LC_FORCE_POST_CURSOR_16GT:6 ; ///<
    UINT32                             LC_3X3_COEFF_SEARCH_EN_16GT:1 ; ///<
    UINT32                                          Reserved_31_20:12; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_LC_FORCE_COEFF2_STRUCT;

// Register Name PCIE_LC_FORCE_COEFF_32GT 

// Address
#define PCIE0_LC_FORCE_COEFF_32GT_ADDRESS_HYEX                            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x403a4)
#define PCIE1_LC_FORCE_COEFF_32GT_ADDRESS_HYEX                            (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x403a4)
#define PCIE2_LC_FORCE_COEFF_32GT_ADDRESS_HYEX                            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x403a4)

#define PCIE0_LC_FORCE_COEFF_32GT_ADDRESS_HYGX                            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x403a4)
#define PCIE2_LC_FORCE_COEFF_32GT_ADDRESS_HYGX                            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x403a4)

// Type
#define PCIE_LC_FORCE_COEFF_32GT_TYPE                                    TYPE_SMN

#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_COEFF_OFFSET                   0
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_COEFF_WIDTH                    1
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_COEFF_MASK                     0x1
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_PRE_CURSOR_OFFSET              1
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_PRE_CURSOR_WIDTH               6
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_PRE_CURSOR_MASK                0x7e
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_CURSOR_OFFSET                  7
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_CURSOR_WIDTH                   6
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_CURSOR_MASK                    0x1f80
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_POST_CURSOR_OFFSET             13
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_POST_CURSOR_WIDTH              6
#define PCIE_LC_FORCE_COEFF_32GT_LC_FORCE_POST_CURSOR_MASK               0x7e000

/// PCIEPORT0F0_000003a4
typedef union {
  struct {
    UINT32                                     LC_FORCE_COEFF_32GT:1 ; ///<
    UINT32                                LC_FORCE_PRE_CURSOR_32GT:6 ; ///<
    UINT32                                    LC_FORCE_CURSOR_32GT:6 ; ///<
    UINT32                               LC_FORCE_POST_CURSOR_32GT:6 ; ///<
    UINT32                                          Reserved_31_19:13; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_LC_FORCE_COEFF_32GT_STRUCT;

// Register Name PCIE_LC_FORCE_EQ_REQ_COEFF
#define PCIE0_LC_FORCE_EQ_REQ_COEFF_ADDRESS_HYEX                     (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x402e8)
#define PCIE1_LC_FORCE_EQ_REQ_COEFF_ADDRESS_HYEX                     (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x402e8)
#define PCIE2_LC_FORCE_EQ_REQ_COEFF_ADDRESS_HYEX                     (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x402e8)

#define PCIE0_LC_FORCE_EQ_REQ_COEFF_ADDRESS_HYGX                     (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x402e8)
#define PCIE2_LC_FORCE_EQ_REQ_COEFF_ADDRESS_HYGX                     (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x402e8)

typedef union {
  struct {
    UINT32                         LC_FORCE_COEFF_IN_EQ_REQ_PHASE:1 ; ///<
    UINT32                                LC_FORCE_PRE_CURSOR_REQ:6 ; ///<
    UINT32                                    LC_FORCE_CURSOR_REQ:6 ; ///<
    UINT32                               LC_FORCE_POST_CURSOR_REQ:6 ; ///<
    UINT32                                        LC_FS_OTHER_END:6 ; ///<
    UINT32                                        LC_LF_OTHER_END:6 ; ///<
    UINT32                                         Reserved_31_31:1 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_LC_FORCE_EQ_REQ_COEFF_STRUCT;

// Register Name PCIE_LC_FORCE_EQ_REQ_COEFF2
#define PCIE0_LC_FORCE_EQ_REQ_COEFF2_ADDRESS_HYEX                   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40380)
#define PCIE1_LC_FORCE_EQ_REQ_COEFF2_ADDRESS_HYEX                   (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40380)
#define PCIE2_LC_FORCE_EQ_REQ_COEFF2_ADDRESS_HYEX                   (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40380)

#define PCIE0_LC_FORCE_EQ_REQ_COEFF2_ADDRESS_HYGX                   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40380)
#define PCIE2_LC_FORCE_EQ_REQ_COEFF2_ADDRESS_HYGX                   (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40380)

typedef union {
  struct {
    UINT32                         LC_FORCE_COEFF_IN_EQ_REQ_PHASE_16GT:1 ; ///<
    UINT32                                LC_FORCE_PRE_CURSOR_REQ_16GT:6 ; ///<
    UINT32                                    LC_FORCE_CURSOR_REQ_16GT:6 ; ///<
    UINT32                               LC_FORCE_POST_CURSOR_REQ_16GT:6 ; ///<
    UINT32                                        LC_FS_OTHER_END_16GT:6 ; ///<
    UINT32                                        LC_LF_OTHER_END_16GT:6 ; ///<
    UINT32                                              Reserved_31_31:1 ; ///<
  } Field;                                                                 ///<
  UINT32 Value;                                                            ///<
} PCIE_LC_FORCE_EQ_REQ_COEFF2_STRUCT;

// Register Name PCIE_LC_FORCE_EQ_REQ_COEFF_32GT
#define PCIE0_LC_FORCE_EQ_REQ_COEFF_32GT_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40384)
#define PCIE1_LC_FORCE_EQ_REQ_COEFF_32GT_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40384)
#define PCIE2_LC_FORCE_EQ_REQ_COEFF_32GT_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40384)

#define PCIE0_LC_FORCE_EQ_REQ_COEFF_32GT_ADDRESS_HYGX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40384)
#define PCIE2_LC_FORCE_EQ_REQ_COEFF_32GT_ADDRESS_HYGX                    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40384)

typedef union {
  struct {
    UINT32                         LC_FORCE_COEFF_IN_EQ_REQ_PHASE_32GT:1 ; ///<
    UINT32                                LC_FORCE_PRE_CURSOR_REQ_32GT:6 ; ///<
    UINT32                                    LC_FORCE_CURSOR_REQ_32GT:6 ; ///<
    UINT32                               LC_FORCE_POST_CURSOR_REQ_32GT:6 ; ///<
    UINT32                                        LC_FS_OTHER_END_32GT:6 ; ///<
    UINT32                                        LC_LF_OTHER_END_32GT:6 ; ///<
    UINT32                                              Reserved_31_31:1 ; ///<
  } Field;                                                                 ///<
  UINT32 Value;                                                            ///<
} PCIE_LC_FORCE_EQ_REQ_COEFF_32GT_STRUCT;

// PCIE_LC_32GT_EQUALIZATION_CNTL
// Address
#define PCIE0_LC_32GT_EQUALIZATION_CNTL_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x403a0)
#define PCIE1_LC_32GT_EQUALIZATION_CNTL_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x403a0)
#define PCIE2_LC_32GT_EQUALIZATION_CNTL_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x403a0)

#define PCIE0_LC_32GT_EQUALIZATION_CNTL_ADDRESS_HYGX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x403a0)
#define PCIE2_LC_32GT_EQUALIZATION_CNTL_ADDRESS_HYGX                    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x403a0)

typedef union {
   struct {
     UINT32               LC_EQ_SEARCH_MODE_32GT:2;
     UINT32                    LC_BYPASS_EQ_32GT:1;
     UINT32             LC_BYPASS_EQ_PRESET_32GT:4;
     UINT32                      LC_REDO_EQ_32GT:1;
     UINT32              LC_USC_EQ_NOT_REQD_32GT:1;
     UINT32                 LC_USC_GO_TO_EQ_32GT:1;
     UINT32       LC_UNEXPECTED_COEFFS_RCVD_32GT:1;
     UINT32          LC_BYPASS_EQ_REQ_PHASE_32GT:1;
     UINT32 LC_FORCE_PRESET_IN_EQ_REQ_PHASE_32GT:1;
     UINT32           LC_FORCE_PRESET_VALUE_32GT:4;
     UINT32              LC_32GT_EQTS2_PRESET_EN:1;
     UINT32                 LC_32GT_EQTS2_PRESET:4;
     UINT32             LC_USE_EQTS2_PRESET_32GT:1;
     UINT32           LC_DSC_ACCEPT_32GT_EQ_REDO:1;
     UINT32            LC_USC_HW_32GT_EQ_REDO_EN:1;
     UINT32             LC_REGS_EQ_REQ_RCVD_32GT:1;
     UINT32          LC_3X3_COEFF_SEARCH_EN_32GT:1;
     UINT32                       RESERVED_31_27:5;
  } Field;                                                                    ///<
  UINT32    Value;                                                            ///<
} PCIE_LC_32GT_EQUALIZATION_CNTL_STRUCT;

// Register Name PCIE_LC_CNTL4

// Address
#define PCIE0_GPP0_PCIE_LC_CNTL4_ADDRESS_HYEX                  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x402d8)
#define PCIE1_GPP0_PCIE_LC_CNTL4_ADDRESS_HYEX                  (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x402d8)
#define PCIE2_GPP0_PCIE_LC_CNTL4_ADDRESS_HYEX                  (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x402d8)

#define PCIE0_GPP0_PCIE_LC_CNTL4_ADDRESS_HYGX                  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x402d8)
#define PCIE2_GPP0_PCIE_LC_CNTL4_ADDRESS_HYGX                  (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x402d8)

#define PCIE_LC_CNTL4_LC_TX_ENABLE_BEHAVIOUR_OFFSET           0
#define PCIE_LC_CNTL4_LC_TX_ENABLE_BEHAVIOUR_WIDTH            2
#define PCIE_LC_CNTL4_LC_TX_ENABLE_BEHAVIOUR_MASK             0x3
#define PCIE_LC_CNTL4_LC_DIS_CONTIG_END_SET_CHECK_OFFSET      2
#define PCIE_LC_CNTL4_LC_DIS_CONTIG_END_SET_CHECK_WIDTH       1
#define PCIE_LC_CNTL4_LC_DIS_CONTIG_END_SET_CHECK_MASK        0x4
#define PCIE_LC_CNTL4_LC_DIS_ASPM_L1_IN_SPEED_CHANGE_OFFSET   3
#define PCIE_LC_CNTL4_LC_DIS_ASPM_L1_IN_SPEED_CHANGE_WIDTH    1
#define PCIE_LC_CNTL4_LC_DIS_ASPM_L1_IN_SPEED_CHANGE_MASK     0x8
#define PCIE_LC_CNTL4_LC_BYPASS_EQ_OFFSET                     4
#define PCIE_LC_CNTL4_LC_BYPASS_EQ_WIDTH                      1
#define PCIE_LC_CNTL4_LC_BYPASS_EQ_MASK                       0x10
#define PCIE_LC_CNTL4_LC_REDO_EQ_OFFSET                       5
#define PCIE_LC_CNTL4_LC_REDO_EQ_WIDTH                        1
#define PCIE_LC_CNTL4_LC_REDO_EQ_MASK                         0x20
#define PCIE_LC_CNTL4_LC_EXTEND_EIEOS_OFFSET                  6
#define PCIE_LC_CNTL4_LC_EXTEND_EIEOS_WIDTH                   1
#define PCIE_LC_CNTL4_LC_EXTEND_EIEOS_MASK                    0x40
#define PCIE_LC_CNTL4_LC_IGNORE_PARITY_OFFSET                 7
#define PCIE_LC_CNTL4_LC_IGNORE_PARITY_WIDTH                  1
#define PCIE_LC_CNTL4_LC_IGNORE_PARITY_MASK                   0x80
#define PCIE_LC_CNTL4_LC_EQ_SEARCH_MODE_OFFSET                8
#define PCIE_LC_CNTL4_LC_EQ_SEARCH_MODE_WIDTH                 2
#define PCIE_LC_CNTL4_LC_EQ_SEARCH_MODE_MASK                  0x300
#define PCIE_LC_CNTL4_LC_DSC_CHECK_COEFFS_IN_RLOCK_OFFSET     10
#define PCIE_LC_CNTL4_LC_DSC_CHECK_COEFFS_IN_RLOCK_WIDTH      1
#define PCIE_LC_CNTL4_LC_DSC_CHECK_COEFFS_IN_RLOCK_MASK       0x400
#define PCIE_LC_CNTL4_LC_USC_EQ_NOT_REQD_OFFSET               11
#define PCIE_LC_CNTL4_LC_USC_EQ_NOT_REQD_WIDTH                1
#define PCIE_LC_CNTL4_LC_USC_EQ_NOT_REQD_MASK                 0x800
#define PCIE_LC_CNTL4_LC_USC_GO_TO_EQ_OFFSET                  12
#define PCIE_LC_CNTL4_LC_USC_GO_TO_EQ_WIDTH                   1
#define PCIE_LC_CNTL4_LC_USC_GO_TO_EQ_MASK                    0x1000
#define PCIE_LC_CNTL4_LC_SET_QUIESCE_OFFSET                   13
#define PCIE_LC_CNTL4_LC_SET_QUIESCE_WIDTH                    1
#define PCIE_LC_CNTL4_LC_SET_QUIESCE_MASK                     0x2000
#define PCIE_LC_CNTL4_LC_QUIESCE_RCVD_OFFSET                  14
#define PCIE_LC_CNTL4_LC_QUIESCE_RCVD_WIDTH                   1
#define PCIE_LC_CNTL4_LC_QUIESCE_RCVD_MASK                    0x4000
#define PCIE_LC_CNTL4_LC_UNEXPECTED_COEFFS_RCVD_OFFSET        15
#define PCIE_LC_CNTL4_LC_UNEXPECTED_COEFFS_RCVD_WIDTH         1
#define PCIE_LC_CNTL4_LC_UNEXPECTED_COEFFS_RCVD_MASK          0x8000
#define PCIE_LC_CNTL4_LC_BYPASS_EQ_REQ_PHASE_OFFSET           16
#define PCIE_LC_CNTL4_LC_BYPASS_EQ_REQ_PHASE_WIDTH            1
#define PCIE_LC_CNTL4_LC_BYPASS_EQ_REQ_PHASE_MASK             0x10000
#define PCIE_LC_CNTL4_LC_FORCE_PRESET_IN_EQ_REQ_PHASE_OFFSET  17
#define PCIE_LC_CNTL4_LC_FORCE_PRESET_IN_EQ_REQ_PHASE_WIDTH   1
#define PCIE_LC_CNTL4_LC_FORCE_PRESET_IN_EQ_REQ_PHASE_MASK    0x20000
#define PCIE_LC_CNTL4_LC_FORCE_PRESET_VALUE_OFFSET            18
#define PCIE_LC_CNTL4_LC_FORCE_PRESET_VALUE_WIDTH             4
#define PCIE_LC_CNTL4_LC_FORCE_PRESET_VALUE_MASK              0x3c0000
#define PCIE_LC_CNTL4_LC_USC_DELAY_DLLPS_OFFSET               22
#define PCIE_LC_CNTL4_LC_USC_DELAY_DLLPS_WIDTH                1
#define PCIE_LC_CNTL4_LC_USC_DELAY_DLLPS_MASK                 0x400000
#define PCIE_LC_CNTL4_LC_TX_SWING_OFFSET                      23
#define PCIE_LC_CNTL4_LC_TX_SWING_WIDTH                       1
#define PCIE_LC_CNTL4_LC_TX_SWING_MASK                        0x800000
#define PCIE_LC_CNTL4_LC_EQ_WAIT_FOR_EVAL_DONE_OFFSET         24
#define PCIE_LC_CNTL4_LC_EQ_WAIT_FOR_EVAL_DONE_WIDTH          1
#define PCIE_LC_CNTL4_LC_EQ_WAIT_FOR_EVAL_DONE_MASK           0x1000000
#define PCIE_LC_CNTL4_LC_8GT_SKIP_ORDER_EN_OFFSET             25
#define PCIE_LC_CNTL4_LC_8GT_SKIP_ORDER_EN_WIDTH              1
#define PCIE_LC_CNTL4_LC_8GT_SKIP_ORDER_EN_MASK               0x2000000
#define PCIE_LC_CNTL4_LC_WAIT_FOR_MORE_TS_IN_RLOCK_OFFSET     26
#define PCIE_LC_CNTL4_LC_WAIT_FOR_MORE_TS_IN_RLOCK_WIDTH      6
#define PCIE_LC_CNTL4_LC_WAIT_FOR_MORE_TS_IN_RLOCK_MASK       0xfc000000L

/// PCIEPORT0F0_000002D8
typedef union {
  struct {
    UINT32    LC_TX_ENABLE_BEHAVIOUR          : 2;                     ///<
    UINT32    LC_DIS_CONTIG_END_SET_CHECK     : 1;                     ///<
    UINT32    LC_DIS_ASPM_L1_IN_SPEED_CHANGE  : 1;                     ///<
    UINT32    LC_BYPASS_EQ                    : 1;                     ///<
    UINT32    LC_REDO_EQ                      : 1;                     ///<
    UINT32    LC_EXTEND_EIEOS                 : 1;                     ///<
    UINT32    LC_IGNORE_PARITY                : 1;                     ///<
    UINT32    LC_EQ_SEARCH_MODE               : 2;                     ///<
    UINT32    LC_DSC_CHECK_COEFFS_IN_RLOCK    : 1;                     ///<
    UINT32    LC_USC_EQ_NOT_REQD              : 1;                     ///<
    UINT32    LC_USC_GO_TO_EQ                 : 1;                     ///<
    UINT32    LC_SET_QUIESCE                  : 1;                     ///<
    UINT32    LC_QUIESCE_RCVD                 : 1;                     ///<
    UINT32    LC_UNEXPECTED_COEFFS_RCVD       : 1;                     ///<
    UINT32    LC_BYPASS_EQ_REQ_PHASE          : 1;                     ///<
    UINT32    LC_FORCE_PRESET_IN_EQ_REQ_PHASE : 1;                     ///<
    UINT32    LC_FORCE_PRESET_VALUE           : 4;                     ///<
    UINT32    LC_USC_DELAY_DLLPS              : 1;                     ///<
    UINT32    LC_TX_SWING                     : 1;                     ///<
    UINT32    LC_EQ_WAIT_FOR_EVAL_DONE        : 1;                     ///<
    UINT32    LC_8GT_SKIP_ORDER_EN            : 1;                     ///<
    UINT32    LC_WAIT_FOR_MORE_TS_IN_RLOCK    : 6;                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_CNTL4_STRUCT;

// Register Name LINK_CAP2

// Address
#define PCIE0_LINK_CAP2_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x02084)
#define PCIE1_LINK_CAP2_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x02084)
#define PCIE2_LINK_CAP2_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x02084)

#define PCIE0_LINK_CAP2_ADDRESS_HYGX                      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x02084)
#define PCIE2_LINK_CAP2_ADDRESS_HYGX                      (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x02084)

// Type
#define PCIE_LINK_CAP2_TYPE                              TYPE_SMN

#define PCIE_LINK_CAP2_SUPPORTED_LINK_SPEED_OFFSET       1
#define PCIE_LINK_CAP2_SUPPORTED_LINK_SPEED_WIDTH        7
#define PCIE_LINK_CAP2_SUPPORTED_LINK_SPEED_MASK         0xfe
#define PCIE_LINK_CAP2_CROSSLINK_SUPPORTED_OFFSET        8
#define PCIE_LINK_CAP2_CROSSLINK_SUPPORTED_WIDTH         1
#define PCIE_LINK_CAP2_CROSSLINK_SUPPORTED_MASK          0x100
#define PCIE_LINK_CAP2_LOWER_SKP_OS_GEN_SUPPORT_OFFSET   9
#define PCIE_LINK_CAP2_LOWER_SKP_OS_GEN_SUPPORT_WIDTH    7
#define PCIE_LINK_CAP2_LOWER_SKP_OS_GEN_SUPPORT_MASK     0xfe00
#define PCIE_LINK_CAP2_LOWER_SKP_OS_RCV_SUPPORT_OFFSET   16
#define PCIE_LINK_CAP2_LOWER_SKP_OS_RCV_SUPPORT_WIDTH    7
#define PCIE_LINK_CAP2_LOWER_SKP_OS_RCV_SUPPORT_MASK     0x7f0000
#define PCIE_LINK_CAP2_RTM1_PRESENCE_DET_SUPPORT_OFFSET  23
#define PCIE_LINK_CAP2_RTM1_PRESENCE_DET_SUPPORT_WIDTH   1
#define PCIE_LINK_CAP2_RTM1_PRESENCE_DET_SUPPORT_MASK    0x800000
#define PCIE_LINK_CAP2_RTM2_PRESENCE_DET_SUPPORT_OFFSET  24
#define PCIE_LINK_CAP2_RTM2_PRESENCE_DET_SUPPORT_WIDTH   1
#define PCIE_LINK_CAP2_RTM2_PRESENCE_DET_SUPPORT_MASK    0x1000000
#define PCIE_LINK_CAP2_DRS_SUPPORTED_OFFSET              31
#define PCIE_LINK_CAP2_DRS_SUPPORTED_WIDTH               1
#define PCIE_LINK_CAP2_DRS_SUPPORTED_MASK                0x80000000L

/// PCIERCCFG0F2_00000084
typedef union {
  struct {
    UINT32    Reserved_0_0              : 1;                           ///<
    UINT32    SUPPORTED_LINK_SPEED      : 7;                           ///<
    UINT32    CROSSLINK_SUPPORTED       : 1;                           ///<
    UINT32    LOWER_SKP_OS_GEN_SUPPORT  : 7;                           ///<
    UINT32    LOWER_SKP_OS_RCV_SUPPORT  : 7;                           ///<
    UINT32    RTM1_PRESENCE_DET_SUPPORT : 1;                           ///<
    UINT32    RTM2_PRESENCE_DET_SUPPORT : 1;                           ///<
    UINT32    Reserved_30_25            : 6;                           ///<
    UINT32    DRS_SUPPORTED             : 1;                           ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LINK_CAP2_STRUCT;

// Register Name PCIE_LC_CNTL

// Address
#define PCIE0_LC_CNTL_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40280)
#define PCIE1_LC_CNTL_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40280)
#define PCIE2_LC_CNTL_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40280)

#define PCIE0_LC_CNTL_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40280)
#define PCIE2_LC_CNTL_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40280)

// Type
#define PCIE_LC_CNTL_TYPE                               TYPE_SMN

#define PCIE_LC_CNTL_RESERVED_OFFSET                    0
#define PCIE_LC_CNTL_RESERVED_WIDTH                     1
#define PCIE_LC_CNTL_RESERVED_MASK                      0x1
#define PCIE_LC_CNTL_LC_DONT_ENTER_L23_IN_D0_OFFSET     1
#define PCIE_LC_CNTL_LC_DONT_ENTER_L23_IN_D0_WIDTH      1
#define PCIE_LC_CNTL_LC_DONT_ENTER_L23_IN_D0_MASK       0x2
#define PCIE_LC_CNTL_LC_RESET_L_IDLE_COUNT_EN_OFFSET    2
#define PCIE_LC_CNTL_LC_RESET_L_IDLE_COUNT_EN_WIDTH     1
#define PCIE_LC_CNTL_LC_RESET_L_IDLE_COUNT_EN_MASK      0x4
#define PCIE_LC_CNTL_LC_RESET_LINK_OFFSET               3
#define PCIE_LC_CNTL_LC_RESET_LINK_WIDTH                1
#define PCIE_LC_CNTL_LC_RESET_LINK_MASK                 0x8
#define PCIE_LC_CNTL_LC_16X_CLEAR_TX_PIPE_OFFSET        4
#define PCIE_LC_CNTL_LC_16X_CLEAR_TX_PIPE_WIDTH         4
#define PCIE_LC_CNTL_LC_16X_CLEAR_TX_PIPE_MASK          0xf0
#define PCIE_LC_CNTL_LC_L0S_INACTIVITY_OFFSET           8
#define PCIE_LC_CNTL_LC_L0S_INACTIVITY_WIDTH            4
#define PCIE_LC_CNTL_LC_L0S_INACTIVITY_MASK             0xf00
#define PCIE_LC_CNTL_LC_L1_INACTIVITY_OFFSET            12
#define PCIE_LC_CNTL_LC_L1_INACTIVITY_WIDTH             4
#define PCIE_LC_CNTL_LC_L1_INACTIVITY_MASK              0xf000
#define PCIE_LC_CNTL_LC_PMI_TO_L1_DIS_OFFSET            16
#define PCIE_LC_CNTL_LC_PMI_TO_L1_DIS_WIDTH             1
#define PCIE_LC_CNTL_LC_PMI_TO_L1_DIS_MASK              0x10000
#define PCIE_LC_CNTL_LC_INC_N_FTS_EN_OFFSET             17
#define PCIE_LC_CNTL_LC_INC_N_FTS_EN_WIDTH              1
#define PCIE_LC_CNTL_LC_INC_N_FTS_EN_MASK               0x20000
#define PCIE_LC_CNTL_LC_LOOK_FOR_IDLE_IN_L1L23_OFFSET   18
#define PCIE_LC_CNTL_LC_LOOK_FOR_IDLE_IN_L1L23_WIDTH    2
#define PCIE_LC_CNTL_LC_LOOK_FOR_IDLE_IN_L1L23_MASK     0xc0000
#define PCIE_LC_CNTL_LC_FACTOR_IN_EXT_SYNC_OFFSET       20
#define PCIE_LC_CNTL_LC_FACTOR_IN_EXT_SYNC_WIDTH        1
#define PCIE_LC_CNTL_LC_FACTOR_IN_EXT_SYNC_MASK         0x100000
#define PCIE_LC_CNTL_LC_WAIT_FOR_PM_ACK_DIS_OFFSET      21
#define PCIE_LC_CNTL_LC_WAIT_FOR_PM_ACK_DIS_WIDTH       1
#define PCIE_LC_CNTL_LC_WAIT_FOR_PM_ACK_DIS_MASK        0x200000
#define PCIE_LC_CNTL_LC_WAKE_FROM_L23_OFFSET            22
#define PCIE_LC_CNTL_LC_WAKE_FROM_L23_WIDTH             1
#define PCIE_LC_CNTL_LC_WAKE_FROM_L23_MASK              0x400000
#define PCIE_LC_CNTL_LC_L1_IMMEDIATE_ACK_OFFSET         23
#define PCIE_LC_CNTL_LC_L1_IMMEDIATE_ACK_WIDTH          1
#define PCIE_LC_CNTL_LC_L1_IMMEDIATE_ACK_MASK           0x800000
#define PCIE_LC_CNTL_LC_ASPM_TO_L1_DIS_OFFSET           24
#define PCIE_LC_CNTL_LC_ASPM_TO_L1_DIS_WIDTH            1
#define PCIE_LC_CNTL_LC_ASPM_TO_L1_DIS_MASK             0x1000000
#define PCIE_LC_CNTL_LC_DELAY_COUNT_OFFSET              25
#define PCIE_LC_CNTL_LC_DELAY_COUNT_WIDTH               2
#define PCIE_LC_CNTL_LC_DELAY_COUNT_MASK                0x6000000
#define PCIE_LC_CNTL_LC_DELAY_L0S_EXIT_OFFSET           27
#define PCIE_LC_CNTL_LC_DELAY_L0S_EXIT_WIDTH            1
#define PCIE_LC_CNTL_LC_DELAY_L0S_EXIT_MASK             0x8000000
#define PCIE_LC_CNTL_LC_DELAY_L1_EXIT_OFFSET            28
#define PCIE_LC_CNTL_LC_DELAY_L1_EXIT_WIDTH             1
#define PCIE_LC_CNTL_LC_DELAY_L1_EXIT_MASK              0x10000000
#define PCIE_LC_CNTL_LC_EXTEND_WAIT_FOR_EL_IDLE_OFFSET  29
#define PCIE_LC_CNTL_LC_EXTEND_WAIT_FOR_EL_IDLE_WIDTH   1
#define PCIE_LC_CNTL_LC_EXTEND_WAIT_FOR_EL_IDLE_MASK    0x20000000
#define PCIE_LC_CNTL_LC_ESCAPE_L1L23_EN_OFFSET          30
#define PCIE_LC_CNTL_LC_ESCAPE_L1L23_EN_WIDTH           1
#define PCIE_LC_CNTL_LC_ESCAPE_L1L23_EN_MASK            0x40000000
#define PCIE_LC_CNTL_LC_GATE_RCVR_IDLE_OFFSET           31
#define PCIE_LC_CNTL_LC_GATE_RCVR_IDLE_WIDTH            1
#define PCIE_LC_CNTL_LC_GATE_RCVR_IDLE_MASK             0x80000000L

/// PCIEPORT0F0_00000280
typedef union {
  struct {
    UINT32    RESERVED                   : 1;                          ///<
    UINT32    LC_DONT_ENTER_L23_IN_D0    : 1;                          ///<
    UINT32    LC_RESET_L_IDLE_COUNT_EN   : 1;                          ///<
    UINT32    LC_RESET_LINK              : 1;                          ///<
    UINT32    LC_16X_CLEAR_TX_PIPE       : 4;                          ///<
    UINT32    LC_L0S_INACTIVITY          : 4;                          ///<
    UINT32    LC_L1_INACTIVITY           : 4;                          ///<
    UINT32    LC_PMI_TO_L1_DIS           : 1;                          ///<
    UINT32    LC_INC_N_FTS_EN            : 1;                          ///<
    UINT32    LC_LOOK_FOR_IDLE_IN_L1L23  : 2;                          ///<
    UINT32    LC_FACTOR_IN_EXT_SYNC      : 1;                          ///<
    UINT32    LC_WAIT_FOR_PM_ACK_DIS     : 1;                          ///<
    UINT32    LC_WAKE_FROM_L23           : 1;                          ///<
    UINT32    LC_L1_IMMEDIATE_ACK        : 1;                          ///<
    UINT32    LC_ASPM_TO_L1_DIS          : 1;                          ///<
    UINT32    LC_DELAY_COUNT             : 2;                          ///<
    UINT32    LC_DELAY_L0S_EXIT          : 1;                          ///<
    UINT32    LC_DELAY_L1_EXIT           : 1;                          ///<
    UINT32    LC_EXTEND_WAIT_FOR_EL_IDLE : 1;                          ///<
    UINT32    LC_ESCAPE_L1L23_EN         : 1;                          ///<
    UINT32    LC_GATE_RCVR_IDLE          : 1;                          ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_CNTL_STRUCT;

// Address
#define PCIE0_LC_LINK_WIDTH_CNTL_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40288)
#define PCIE1_LC_LINK_WIDTH_CNTL_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40288)
#define PCIE2_LC_LINK_WIDTH_CNTL_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40288)

#define PCIE0_LC_LINK_WIDTH_CNTL_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40288)
#define PCIE2_LC_LINK_WIDTH_CNTL_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40288)

// Type
#define PCIE_LC_LINK_WIDTH_CNTL_TYPE                                    TYPE_SMN

#define PCIE_LC_LINK_WIDTH_CNTL_LC_LINK_WIDTH_OFFSET                     0
#define PCIE_LC_LINK_WIDTH_CNTL_LC_LINK_WIDTH_WIDTH                      3
#define PCIE_LC_LINK_WIDTH_CNTL_LC_LINK_WIDTH_MASK                       0x7
#define PCIE_LC_LINK_WIDTH_CNTL_Reserved_3_3_OFFSET                      3
#define PCIE_LC_LINK_WIDTH_CNTL_Reserved_3_3_WIDTH                       1
#define PCIE_LC_LINK_WIDTH_CNTL_Reserved_3_3_MASK                        0x8
#define PCIE_LC_LINK_WIDTH_CNTL_LC_LINK_WIDTH_RD_OFFSET                  4
#define PCIE_LC_LINK_WIDTH_CNTL_LC_LINK_WIDTH_RD_WIDTH                   3
#define PCIE_LC_LINK_WIDTH_CNTL_LC_LINK_WIDTH_RD_MASK                    0x70
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RECONFIG_ARC_MISSING_ESCAPE_OFFSET    7
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RECONFIG_ARC_MISSING_ESCAPE_WIDTH     1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RECONFIG_ARC_MISSING_ESCAPE_MASK      0x80
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RECONFIG_NOW_OFFSET                   8
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RECONFIG_NOW_WIDTH                    1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RECONFIG_NOW_MASK                     0x100
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RENEGOTIATION_SUPPORT_OFFSET          9
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RENEGOTIATION_SUPPORT_WIDTH           1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RENEGOTIATION_SUPPORT_MASK            0x200
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RENEGOTIATE_EN_OFFSET                 10
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RENEGOTIATE_EN_WIDTH                  1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RENEGOTIATE_EN_MASK                   0x400
#define PCIE_LC_LINK_WIDTH_CNTL_LC_SHORT_RECONFIG_EN_OFFSET              11
#define PCIE_LC_LINK_WIDTH_CNTL_LC_SHORT_RECONFIG_EN_WIDTH               1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_SHORT_RECONFIG_EN_MASK                0x800
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCONFIGURE_SUPPORT_OFFSET            12
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCONFIGURE_SUPPORT_WIDTH             1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCONFIGURE_SUPPORT_MASK              0x1000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCONFIGURE_DIS_OFFSET                13
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCONFIGURE_DIS_WIDTH                 1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCONFIGURE_DIS_MASK                  0x2000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCFG_WAIT_FOR_RCVR_DIS_OFFSET        14
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCFG_WAIT_FOR_RCVR_DIS_WIDTH         1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCFG_WAIT_FOR_RCVR_DIS_MASK          0x4000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCFG_TIMER_SEL_OFFSET                15
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCFG_TIMER_SEL_WIDTH                 1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCFG_TIMER_SEL_MASK                  0x8000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DEASSERT_TX_PDNB_OFFSET               16
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DEASSERT_TX_PDNB_WIDTH                1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DEASSERT_TX_PDNB_MASK                 0x10000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_L1_RECONFIG_EN_OFFSET                 17
#define PCIE_LC_LINK_WIDTH_CNTL_LC_L1_RECONFIG_EN_WIDTH                  1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_L1_RECONFIG_EN_MASK                   0x20000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DYNLINK_MST_EN_OFFSET                 18
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DYNLINK_MST_EN_WIDTH                  1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DYNLINK_MST_EN_MASK                   0x40000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DUAL_END_RECONFIG_EN_OFFSET           19
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DUAL_END_RECONFIG_EN_WIDTH            1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DUAL_END_RECONFIG_EN_MASK             0x80000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCONFIGURE_CAPABLE_OFFSET            20
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCONFIGURE_CAPABLE_WIDTH             1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_UPCONFIGURE_CAPABLE_MASK              0x100000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DYN_LANES_PWR_STATE_OFFSET            21
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DYN_LANES_PWR_STATE_WIDTH             2
#define PCIE_LC_LINK_WIDTH_CNTL_LC_DYN_LANES_PWR_STATE_MASK              0x600000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_EQ_REVERSAL_LOGIC_EN_OFFSET           23
#define PCIE_LC_LINK_WIDTH_CNTL_LC_EQ_REVERSAL_LOGIC_EN_WIDTH            1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_EQ_REVERSAL_LOGIC_EN_MASK             0x800000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_MULT_REVERSE_ATTEMP_EN_OFFSET         24
#define PCIE_LC_LINK_WIDTH_CNTL_LC_MULT_REVERSE_ATTEMP_EN_WIDTH          1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_MULT_REVERSE_ATTEMP_EN_MASK           0x1000000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RESET_TSX_CNT_IN_RCONFIG_EN_OFFSET    25
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RESET_TSX_CNT_IN_RCONFIG_EN_WIDTH     1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_RESET_TSX_CNT_IN_RCONFIG_EN_MASK      0x2000000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_WAIT_FOR_L_IDLE_IN_R_IDLE_OFFSET      26
#define PCIE_LC_LINK_WIDTH_CNTL_LC_WAIT_FOR_L_IDLE_IN_R_IDLE_WIDTH       1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_WAIT_FOR_L_IDLE_IN_R_IDLE_MASK        0x4000000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_WAIT_FOR_NON_EI_ON_RXL0S_EXIT_OFFSET  27
#define PCIE_LC_LINK_WIDTH_CNTL_LC_WAIT_FOR_NON_EI_ON_RXL0S_EXIT_WIDTH   1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_WAIT_FOR_NON_EI_ON_RXL0S_EXIT_MASK    0x8000000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_HOLD_EI_FOR_RSPEED_CMD_CHANGE_OFFSET  28
#define PCIE_LC_LINK_WIDTH_CNTL_LC_HOLD_EI_FOR_RSPEED_CMD_CHANGE_WIDTH   1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_HOLD_EI_FOR_RSPEED_CMD_CHANGE_MASK    0x10000000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_BYPASS_RXL0S_ON_SHORT_EI_OFFSET       29
#define PCIE_LC_LINK_WIDTH_CNTL_LC_BYPASS_RXL0S_ON_SHORT_EI_WIDTH        1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_BYPASS_RXL0S_ON_SHORT_EI_MASK         0x20000000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_TURN_OFF_UNUSED_LANES_OFFSET          30
#define PCIE_LC_LINK_WIDTH_CNTL_LC_TURN_OFF_UNUSED_LANES_WIDTH           1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_TURN_OFF_UNUSED_LANES_MASK            0x40000000
#define PCIE_LC_LINK_WIDTH_CNTL_LC_BYPASS_RXSTANDBY_STATUS_OFFSET        31
#define PCIE_LC_LINK_WIDTH_CNTL_LC_BYPASS_RXSTANDBY_STATUS_WIDTH         1
#define PCIE_LC_LINK_WIDTH_CNTL_LC_BYPASS_RXSTANDBY_STATUS_MASK          0x80000000L

/// PCIE_LC_LINK_WIDTH_CNTL
typedef union {
  struct {
    UINT32    LC_LINK_WIDTH                    : 3;                    ///<
    UINT32    Reserved_3_3                     : 1;                    ///<
    UINT32    LC_LINK_WIDTH_RD                 : 3;                    ///<
    UINT32    LC_RECONFIG_ARC_MISSING_ESCAPE   : 1;                    ///<
    UINT32    LC_RECONFIG_NOW                  : 1;                    ///<
    UINT32    LC_RENEGOTIATION_SUPPORT         : 1;                    ///<
    UINT32    LC_RENEGOTIATE_EN                : 1;                    ///<
    UINT32    LC_SHORT_RECONFIG_EN             : 1;                    ///<
    UINT32    LC_UPCONFIGURE_SUPPORT           : 1;                    ///<
    UINT32    LC_UPCONFIGURE_DIS               : 1;                    ///<
    UINT32    LC_UPCFG_WAIT_FOR_RCVR_DIS       : 1;                    ///<
    UINT32    LC_UPCFG_TIMER_SEL               : 1;                    ///<
    UINT32    LC_DEASSERT_TX_PDNB              : 1;                    ///<
    UINT32    LC_L1_RECONFIG_EN                : 1;                    ///<
    UINT32    LC_DYNLINK_MST_EN                : 1;                    ///<
    UINT32    LC_DUAL_END_RECONFIG_EN          : 1;                    ///<
    UINT32    LC_UPCONFIGURE_CAPABLE           : 1;                    ///<
    UINT32    LC_DYN_LANES_PWR_STATE           : 2;                    ///<
    UINT32    LC_EQ_REVERSAL_LOGIC_EN          : 1;                    ///<
    UINT32    LC_MULT_REVERSE_ATTEMP_EN        : 1;                    ///<
    UINT32    LC_RESET_TSX_CNT_IN_RCONFIG_EN   : 1;                    ///<
    UINT32    LC_WAIT_FOR_L_IDLE_IN_R_IDLE     : 1;                    ///<
    UINT32    LC_WAIT_FOR_NON_EI_ON_RXL0S_EXIT : 1;                    ///<
    UINT32    LC_HOLD_EI_FOR_RSPEED_CMD_CHANGE : 1;                    ///<
    UINT32    LC_BYPASS_RXL0S_ON_SHORT_EI      : 1;                    ///<
    UINT32    LC_TURN_OFF_UNUSED_LANES         : 1;                    ///<
    UINT32    LC_BYPASS_RXSTANDBY_STATUS       : 1;                    ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_LINK_WIDTH_CNTL_STRUCT;

// Address
#define PCIE0_GPP0_LC_CNTL6_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x402ec)
#define PCIE1_GPP0_LC_CNTL6_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x402ec)
#define PCIE2_GPP0_LC_CNTL6_ADDRESS_HYEX  (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x402ec)

#define PCIE0_GPP0_LC_CNTL6_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x402ec)
#define PCIE2_GPP0_LC_CNTL6_ADDRESS_HYGX  (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x402ec)
// Type
#define PCIE_LC_CNTL6_TYPE                                    TYPE_SMN

#define PCIE_LC_CNTL6_LC_SPC_MODE_2P5GT_OFFSET                0
#define PCIE_LC_CNTL6_LC_SPC_MODE_2P5GT_WIDTH                 1
#define PCIE_LC_CNTL6_LC_SPC_MODE_2P5GT_MASK                  0x1
#define PCIE_LC_CNTL6_Reserved_1_1_OFFSET                     1
#define PCIE_LC_CNTL6_Reserved_1_1_WIDTH                      1
#define PCIE_LC_CNTL6_Reserved_1_1_MASK                       0x2
#define PCIE_LC_CNTL6_LC_SPC_MODE_5GT_OFFSET                  2
#define PCIE_LC_CNTL6_LC_SPC_MODE_5GT_WIDTH                   1
#define PCIE_LC_CNTL6_LC_SPC_MODE_5GT_MASK                    0x4
#define PCIE_LC_CNTL6_Reserved_3_3_OFFSET                     3
#define PCIE_LC_CNTL6_Reserved_3_3_WIDTH                      1
#define PCIE_LC_CNTL6_Reserved_3_3_MASK                       0x8
#define PCIE_LC_CNTL6_LC_SPC_MODE_8GT_OFFSET                  4
#define PCIE_LC_CNTL6_LC_SPC_MODE_8GT_WIDTH                   1
#define PCIE_LC_CNTL6_LC_SPC_MODE_8GT_MASK                    0x10
#define PCIE_LC_CNTL6_LC_WAIT_FOR_EIEOS_IN_RLOCK_OFFSET       5
#define PCIE_LC_CNTL6_LC_WAIT_FOR_EIEOS_IN_RLOCK_WIDTH        1
#define PCIE_LC_CNTL6_LC_WAIT_FOR_EIEOS_IN_RLOCK_MASK         0x20
#define PCIE_LC_CNTL6_LC_DYNAMIC_INACTIVE_TS_SELECT_OFFSET    6
#define PCIE_LC_CNTL6_LC_DYNAMIC_INACTIVE_TS_SELECT_WIDTH     2
#define PCIE_LC_CNTL6_LC_DYNAMIC_INACTIVE_TS_SELECT_MASK      0xc0
#define PCIE_LC_CNTL6_LC_SRIS_EN_OFFSET                       8
#define PCIE_LC_CNTL6_LC_SRIS_EN_WIDTH                        1
#define PCIE_LC_CNTL6_LC_SRIS_EN_MASK                         0x100
#define PCIE_LC_CNTL6_LC_SRNS_SKIP_IN_SRIS_OFFSET             9
#define PCIE_LC_CNTL6_LC_SRNS_SKIP_IN_SRIS_WIDTH              4
#define PCIE_LC_CNTL6_LC_SRNS_SKIP_IN_SRIS_MASK               0x1e00
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_EN_OFFSET            13
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_EN_WIDTH             1
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_EN_MASK              0x2000
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_FACTOR_OFFSET        14
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_FACTOR_WIDTH         2
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_FACTOR_MASK          0xc000
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_MODE_OFFSET          16
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_MODE_WIDTH           2
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_MODE_MASK            0x30000
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_OUT_OF_RANGE_OFFSET  18
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_OUT_OF_RANGE_WIDTH   1
#define PCIE_LC_CNTL6_LC_SRIS_AUTODETECT_OUT_OF_RANGE_MASK    0x40000
#define PCIE_LC_CNTL6_LC_DEFER_SKIP_FOR_EIEOS_EN_OFFSET       19
#define PCIE_LC_CNTL6_LC_DEFER_SKIP_FOR_EIEOS_EN_WIDTH        1
#define PCIE_LC_CNTL6_LC_DEFER_SKIP_FOR_EIEOS_EN_MASK         0x80000
#define PCIE_LC_CNTL6_LC_SEND_EIEOS_IN_RCFG_OFFSET            20
#define PCIE_LC_CNTL6_LC_SEND_EIEOS_IN_RCFG_WIDTH             1
#define PCIE_LC_CNTL6_LC_SEND_EIEOS_IN_RCFG_MASK              0x100000
#define PCIE_LC_CNTL6_LC_L1_POWERDOWN_OFFSET                  21
#define PCIE_LC_CNTL6_LC_L1_POWERDOWN_WIDTH                   1
#define PCIE_LC_CNTL6_LC_L1_POWERDOWN_MASK                    0x200000
#define PCIE_LC_CNTL6_LC_P2_ENTRY_OFFSET                      22
#define PCIE_LC_CNTL6_LC_P2_ENTRY_WIDTH                       1
#define PCIE_LC_CNTL6_LC_P2_ENTRY_MASK                        0x400000
#define PCIE_LC_CNTL6_LC_RXRECOVER_EN_OFFSET                  23
#define PCIE_LC_CNTL6_LC_RXRECOVER_EN_WIDTH                   1
#define PCIE_LC_CNTL6_LC_RXRECOVER_EN_MASK                    0x800000
#define PCIE_LC_CNTL6_LC_RXRECOVER_TIMEOUT_OFFSET             24
#define PCIE_LC_CNTL6_LC_RXRECOVER_TIMEOUT_WIDTH              7
#define PCIE_LC_CNTL6_LC_RXRECOVER_TIMEOUT_MASK               0x7f000000
#define PCIE_LC_CNTL6_LC_RX_L0S_STANDBY_EN_OFFSET             31
#define PCIE_LC_CNTL6_LC_RX_L0S_STANDBY_EN_WIDTH              1
#define PCIE_LC_CNTL6_LC_RX_L0S_STANDBY_EN_MASK               0x80000000L

/// PCIE_LC_CNTL6
typedef union {
  struct {
    UINT32    LC_SPC_MODE_2P5GT               : 1;                     ///<
    UINT32    Reserved_1_1                    : 1;                     ///<
    UINT32    LC_SPC_MODE_5GT                 : 1;                     ///<
    UINT32    Reserved_3_3                    : 1;                     ///<
    UINT32    LC_SPC_MODE_8GT                 : 1;                     ///<
    UINT32    LC_WAIT_FOR_EIEOS_IN_RLOCK      : 1;                     ///<
    UINT32    LC_DYNAMIC_INACTIVE_TS_SELECT   : 2;                     ///<
    UINT32    LC_SRIS_EN                      : 1;                     ///<
    UINT32    LC_SRNS_SKIP_IN_SRIS            : 4;                     ///<
    UINT32    LC_SRIS_AUTODETECT_EN           : 1;                     ///<
    UINT32    LC_SRIS_AUTODETECT_FACTOR       : 2;                     ///<
    UINT32    LC_SRIS_AUTODETECT_MODE         : 2;                     ///<
    UINT32    LC_SRIS_AUTODETECT_OUT_OF_RANGE : 1;                     ///<
    UINT32    LC_DEFER_SKIP_FOR_EIEOS_EN      : 1;                     ///<
    UINT32    LC_SEND_EIEOS_IN_RCFG           : 1;                     ///<
    UINT32    LC_L1_POWERDOWN                 : 1;                     ///<
    UINT32    LC_P2_ENTRY                     : 1;                     ///<
    UINT32    LC_RXRECOVER_EN                 : 1;                     ///<
    UINT32    LC_RXRECOVER_TIMEOUT            : 7;                     ///<
    UINT32    LC_RX_L0S_STANDBY_EN            : 1;                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_CNTL6_STRUCT;

// Register Name PCIE_LC_CNTL7

// Address
#define PCIE0_LC_CNTL7_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x402f0)
#define PCIE1_LC_CNTL7_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x402f0)
#define PCIE2_LC_CNTL7_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x402f0)

#define PCIE0_LC_CNTL7_ADDRESS_HYGX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x402f0)
#define PCIE2_LC_CNTL7_ADDRESS_HYGX             (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x402f0)

// Type
#define PCIE_LC_CNTL7_TYPE                                      TYPE_SMN

#define PCIE_LC_CNTL7_LC_EXPECTED_TS2_CFG_COMPLETE_OFFSET       0
#define PCIE_LC_CNTL7_LC_EXPECTED_TS2_CFG_COMPLETE_WIDTH        1
#define PCIE_LC_CNTL7_LC_EXPECTED_TS2_CFG_COMPLETE_MASK         0x1
#define PCIE_LC_CNTL7_LC_IGNORE_NON_CONTIG_SETS_IN_RCFG_OFFSET  1
#define PCIE_LC_CNTL7_LC_IGNORE_NON_CONTIG_SETS_IN_RCFG_WIDTH   1
#define PCIE_LC_CNTL7_LC_IGNORE_NON_CONTIG_SETS_IN_RCFG_MASK    0x2
#define PCIE_LC_CNTL7_LC_ROBUST_TRAINING_BIT_CHK_EN_OFFSET      2
#define PCIE_LC_CNTL7_LC_ROBUST_TRAINING_BIT_CHK_EN_WIDTH       1
#define PCIE_LC_CNTL7_LC_ROBUST_TRAINING_BIT_CHK_EN_MASK        0x4
#define PCIE_LC_CNTL7_LC_RESET_TS_COUNT_ON_EI_OFFSET            3
#define PCIE_LC_CNTL7_LC_RESET_TS_COUNT_ON_EI_WIDTH             1
#define PCIE_LC_CNTL7_LC_RESET_TS_COUNT_ON_EI_MASK              0x8
#define PCIE_LC_CNTL7_LC_NBIF_ASPM_INPUT_EN_OFFSET              4
#define PCIE_LC_CNTL7_LC_NBIF_ASPM_INPUT_EN_WIDTH               1
#define PCIE_LC_CNTL7_LC_NBIF_ASPM_INPUT_EN_MASK                0x10
#define PCIE_LC_CNTL7_LC_EVER_IDLE_TO_RLOCK_OFFSET              8
#define PCIE_LC_CNTL7_LC_EVER_IDLE_TO_RLOCK_WIDTH               1
#define PCIE_LC_CNTL7_LC_EVER_IDLE_TO_RLOCK_MASK                0x100
#define PCIE_LC_CNTL7_LC_RXEQEVAL_AFTER_TIMEOUT_EN_OFFSET       9
#define PCIE_LC_CNTL7_LC_RXEQEVAL_AFTER_TIMEOUT_EN_WIDTH        1
#define PCIE_LC_CNTL7_LC_RXEQEVAL_AFTER_TIMEOUT_EN_MASK         0x200
#define PCIE_LC_CNTL7_LC_ESM_WAIT_FOR_PLL_INIT_DONE_L1_OFFSET   12
#define PCIE_LC_CNTL7_LC_ESM_WAIT_FOR_PLL_INIT_DONE_L1_WIDTH    1
#define PCIE_LC_CNTL7_LC_ESM_WAIT_FOR_PLL_INIT_DONE_L1_MASK     0x1000
#define PCIE_LC_CNTL7_LC_SCHEDULED_RXEQEVAL_INTERVAL_OFFSET     13
#define PCIE_LC_CNTL7_LC_SCHEDULED_RXEQEVAL_INTERVAL_WIDTH      8
#define PCIE_LC_CNTL7_LC_SCHEDULED_RXEQEVAL_INTERVAL_MASK       0x1fe000
#define PCIE_LC_CNTL7_LC_SCHEDULED_RXEQEVAL_MODE_OFFSET         21
#define PCIE_LC_CNTL7_LC_SCHEDULED_RXEQEVAL_MODE_WIDTH          1
#define PCIE_LC_CNTL7_LC_SCHEDULED_RXEQEVAL_MODE_MASK           0x200000
#define PCIE_LC_CNTL7_LC_SCHEDULED_RXEQEVAL_UPCONFIG_EN_OFFSET  22
#define PCIE_LC_CNTL7_LC_SCHEDULED_RXEQEVAL_UPCONFIG_EN_WIDTH   1
#define PCIE_LC_CNTL7_LC_SCHEDULED_RXEQEVAL_UPCONFIG_EN_MASK    0x400000
#define PCIE_LC_CNTL7_LC_LINK_MANAGEMENT_EN_OFFSET              23
#define PCIE_LC_CNTL7_LC_LINK_MANAGEMENT_EN_WIDTH               1
#define PCIE_LC_CNTL7_LC_LINK_MANAGEMENT_EN_MASK                0x800000
#define PCIE_LC_CNTL7_LC_FOM_TIME_OFFSET                        24
#define PCIE_LC_CNTL7_LC_FOM_TIME_WIDTH                         2
#define PCIE_LC_CNTL7_LC_FOM_TIME_MASK                          0x3000000
#define PCIE_LC_CNTL7_LC_SAFE_EQ_SEARCH_OFFSET                  26
#define PCIE_LC_CNTL7_LC_SAFE_EQ_SEARCH_WIDTH                   1
#define PCIE_LC_CNTL7_LC_SAFE_EQ_SEARCH_MASK                    0x4000000
#define PCIE_LC_CNTL7_LC_ESM_PLL_INIT_STATE_OFFSET              27
#define PCIE_LC_CNTL7_LC_ESM_PLL_INIT_STATE_WIDTH               1
#define PCIE_LC_CNTL7_LC_ESM_PLL_INIT_STATE_MASK                0x8000000
#define PCIE_LC_CNTL7_LC_ESM_PLL_INIT_DONE_OFFSET               28
#define PCIE_LC_CNTL7_LC_ESM_PLL_INIT_DONE_WIDTH                1
#define PCIE_LC_CNTL7_LC_ESM_PLL_INIT_DONE_MASK                 0x10000000
#define PCIE_LC_CNTL7_LC_ESM_REDO_INIT_OFFSET                   29
#define PCIE_LC_CNTL7_LC_ESM_REDO_INIT_WIDTH                    1
#define PCIE_LC_CNTL7_LC_ESM_REDO_INIT_MASK                     0x20000000
#define PCIE_LC_CNTL7_LC_MULTIPORT_ESM_OFFSET                   30
#define PCIE_LC_CNTL7_LC_MULTIPORT_ESM_WIDTH                    1
#define PCIE_LC_CNTL7_LC_MULTIPORT_ESM_MASK                     0x40000000
#define PCIE_LC_CNTL7_LC_CONSECUTIVE_EIOS_RESET_EN_OFFSET       31
#define PCIE_LC_CNTL7_LC_CONSECUTIVE_EIOS_RESET_EN_WIDTH        1
#define PCIE_LC_CNTL7_LC_CONSECUTIVE_EIOS_RESET_EN_MASK         0x80000000L

/// PCIEPORT0F0_000002F0
typedef union {
  struct {
    UINT32    LC_EXPECTED_TS2_CFG_COMPLETE      : 1;                   ///<
    UINT32    LC_IGNORE_NON_CONTIG_SETS_IN_RCFG : 1;                   ///<
    UINT32    LC_ROBUST_TRAINING_BIT_CHK_EN     : 1;                   ///<
    UINT32    LC_RESET_TS_COUNT_ON_EI           : 1;                   ///<
    UINT32    LC_NBIF_ASPM_INPUT_EN             : 1;                   ///<
    UINT32    Reserved_7_5                      : 3;                   ///<
    UINT32    LC_EVER_IDLE_TO_RLOCK             : 1;                   ///<
    UINT32    LC_RXEQEVAL_AFTER_TIMEOUT_EN      : 1;                   ///<
    UINT32    Reserved_11_10                    : 2;                   ///<
    UINT32    LC_ESM_WAIT_FOR_PLL_INIT_DONE_L1  : 1;                   ///<
    UINT32    LC_SCHEDULED_RXEQEVAL_INTERVAL    : 8;                   ///<
    UINT32    LC_SCHEDULED_RXEQEVAL_MODE        : 1;                   ///<
    UINT32    LC_SCHEDULED_RXEQEVAL_UPCONFIG_EN : 1;                   ///<
    UINT32    LC_LINK_MANAGEMENT_EN             : 1;                   ///<
    UINT32    LC_FOM_TIME                       : 2;                   ///<
    UINT32    LC_SAFE_EQ_SEARCH                 : 1;                   ///<
    UINT32    LC_ESM_PLL_INIT_STATE             : 1;                   ///<
    UINT32    LC_ESM_PLL_INIT_DONE              : 1;                   ///<
    UINT32    LC_ESM_REDO_INIT                  : 1;                   ///<
    UINT32    LC_MULTIPORT_ESM                  : 1;                   ///<
    UINT32    LC_CONSECUTIVE_EIOS_RESET_EN      : 1;                   ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_CNTL7_STRUCT;

// svd_group: NBIO_IOHC_aliasSMN
// Address
#define NB_IOC_CNTL_ADDRESS_HYEX                             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10000)
#define NB_IOC_CNTL_ADDRESS_HYGX                             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10000)
#define NB_CNTL_DISABLE_HALF_NBIO_FOR_DEINTERLEAVING_OFFSET  31
#define NB_CNTL_DISABLE_HALF_NBIO_FOR_DEINTERLEAVING_WIDTH   1
#define NB_CNTL_DISABLE_HALF_NBIO_FOR_DEINTERLEAVING_MASK    0x80000000
/// NBMISC_0000
typedef union {
  struct {
		UINT32                EMEI_AND_DJ_SIDEBAND_ENABLE : 1;
		UINT32                                   reserved : 1;
		UINT32        EMEI_CAN_SEND_ERR_EVENT_THROUGH_SMN : 1;
		UINT32                                            : 4;
		UINT32                             HWINIT_WR_LOCK : 1;
		UINT32                                            : 2;
		UINT32     EMEI_AND_DJ_COWORK_FOR_HMIPCS_BACKDOOR : 1;
		UINT32              EMEI_AND_DJ_COWORK_FOR_HMIPCS : 1;
		UINT32                   PKG_TYPE_ENABLE_BACKDOOR : 1;
		UINT32                PKG_TYPE_EMEI_AND_DJ_COWORK : 1;
		UINT32     PKG_TYPE_EMEI_USE_RAW_BRDG_CFG_SETTING : 1;
		UINT32                                            : 1;
		UINT32             PKG_TYPE_DJ_TO_EMEI_SMN_DIE_ID : 4;
		UINT32             PKG_TYPE_EMEI_TO_DJ_SMN_DIE_ID : 4;
		UINT32              ENABLE_FORWARD_MPx_CFG_TO_SDF : 1;
		UINT32               ENABLE_FORWARD_MPx_IO_TO_SDF : 1;
		UINT32             ENABLE_FORWARD_MPx_MMIO_TO_SDF : 1;
		UINT32                                            : 2;
		UINT32      DISABLE_NBIO_1_2_3_FOR_DEINTERLEAVING : 1;
		UINT32    IGNORE_LCLK_CTRL_FROM_IOAGR_and_TWO_SST : 1;
		UINT32        DISABLE_NBIO_1_3_FOR_DEINTERLEAVING : 1;
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_NB_IOC_CNTL_STRUCT;

#define NB_IOC_DEBUG_ADDRESS_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10004)
#define NB_IOC_DEBUG_ADDRESS_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10004)

// Type
#define NB_IOC_DEBUG_TYPE                     TYPE_NBMISC

#define NBMISC_0004_NB_IOC_DEBUG_RW_OFFSET  0
#define NBMISC_0004_NB_IOC_DEBUG_RW_WIDTH   16
#define NBMISC_0004_NB_IOC_DEBUG_RW_MASK    0xffff

/// NBMISC_0004
typedef union {
  struct {
    UINT32    NB_IOC_DEBUG_RW : 16;                                    ///<
    UINT32    Reserved_31_16  : 16;                                    ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_0004_STRUCT;

// Address
#define IOHC_P2P_CNTL_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10030)
#define IOHC_P2P_CNTL_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10030)

// Type
#define IOHC_P2P_CNTL_TYPE                  TYPE_SMN

#define IOHC_P2P_CNTL_DlDownResetEn_OFFSET  11
#define IOHC_P2P_CNTL_DlDownResetEn_WIDTH   1
#define IOHC_P2P_CNTL_DlDownResetEn_MASK    0x800

/// IOHCMISC_00000030
typedef union {
  struct {
    UINT32    Reserved_10_0  : 11;                                     ///<
    UINT32    DlDownResetEn  : 1;                                      ///<
    UINT32    Reserved_31_12 : 20;                                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOHC_P2P_CNTL_STRUCT;

// Register Name NB_PCI_CTRL

// Address
#define NB_PCI_CTRL_ADDRESS_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x0004C)
#define NB_PCI_CTRL_ADDRESS_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x0004C)

// Type
#define NB_PCI_CTRL_TYPE                   TYPE_SMN

#define NB_PCI_CTRL_Reserved_3_0_OFFSET    0
#define NB_PCI_CTRL_Reserved_3_0_WIDTH     4
#define NB_PCI_CTRL_Reserved_3_0_MASK      0xf
#define NB_PCI_CTRL_PMEDis_OFFSET          4
#define NB_PCI_CTRL_PMEDis_WIDTH           1
#define NB_PCI_CTRL_PMEDis_MASK            0x10
#define NB_PCI_CTRL_SErrDis_OFFSET         5
#define NB_PCI_CTRL_SErrDis_WIDTH          1
#define NB_PCI_CTRL_SErrDis_MASK           0x20
#define NB_PCI_CTRL_Reserved_22_6_OFFSET   6
#define NB_PCI_CTRL_Reserved_22_6_WIDTH    17
#define NB_PCI_CTRL_Reserved_22_6_MASK     0x7fffc0
#define NB_PCI_CTRL_MMIOEnable_OFFSET      23
#define NB_PCI_CTRL_MMIOEnable_WIDTH       1
#define NB_PCI_CTRL_MMIOEnable_MASK        0x800000
#define NB_PCI_CTRL_Reserved_25_24_OFFSET  24
#define NB_PCI_CTRL_Reserved_25_24_WIDTH   2
#define NB_PCI_CTRL_Reserved_25_24_MASK    0x3000000
#define NB_PCI_CTRL_HPDis_OFFSET           26
#define NB_PCI_CTRL_HPDis_WIDTH            1
#define NB_PCI_CTRL_HPDis_MASK             0x4000000
#define NB_PCI_CTRL_Reserved_31_27_OFFSET  27
#define NB_PCI_CTRL_Reserved_31_27_WIDTH   5
#define NB_PCI_CTRL_Reserved_31_27_MASK    0xf8000000L

/// NB_PCI_CTRL
typedef union {
  struct {
    UINT32    Reserved_3_0   : 4;                                      ///<
    UINT32    PMEDis         : 1;                                      ///<
    UINT32    SErrDis        : 1;                                      ///<
    UINT32    Reserved_22_6  : 17;                                     ///<
    UINT32    MMIOEnable     : 1;                                      ///<
    UINT32    Reserved_25_24 : 2;                                      ///<
    UINT32    HPDis          : 1;                                      ///<
    UINT32    Reserved_31_27 : 5;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NB_PCI_CTRL_STRUCT;

// Device REMAP Address
#define NB_PROG_DEVICE_REMAP_HYEX                (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x100C0)
#define NB_PROG_DEVICE_REMAP_N18_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10E80)
#define NB_PROG_DEVICE_REMAP_N19_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10E84)

#define NB_PROG_DEVICE_REMAP_HYGX                (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x100C0)
#define NB_PROG_DEVICE_REMAP_N18_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10E80)
#define NB_PROG_DEVICE_REMAP_N19_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10E84)

#define NB_PROG_DEVICE_REMAP_DevFnMap_OFFSET    0
#define NB_PROG_DEVICE_REMAP_DevFnMap_WIDTH     8
#define NB_PROG_DEVICE_REMAP_DevFnMap_MASK      0xff

typedef union {
  struct {
    UINT32    DevFnMap : 8;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NB_PROG_DEVICE_REMAP_STRUCT;

// Register Name PCIE_VDM_NODE_CTRL4

// Address
#define PCIE_VDM_NODE0_CTRL4_ADDRESS_HYEX                     (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10190)
#define PCIE_VDM_NODE0_CTRL4_ADDRESS_HYGX                     (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10190)

#ifndef IOHC_INTERRUPT_EOI_HYGX
  #define IOHC_INTERRUPT_EOI_HYEX                             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10120)
  #define IOHC_INTERRUPT_EOI_HYGX                             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10120)
#endif

#define IOHC_PIN_CNTL_HYEX                                    (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10128)
#define IOHC_PIN_CNTL_HYGX                                    (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10128)

#define IOHC_INTR_CNTL_HYEX                                   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1012c)
#define IOHC_INTR_CNTL_HYGX                                   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1012c)

#define IOHC_NMI_CHANGE_BROADCAST_DEST_IN_X2APIC_OFFSET      31
#define IOHC_NMI_CHANGE_BROADCAST_DEST_IN_X2APIC_WIDTH       1
#define IOHC_NMI_CHANGE_BROADCAST_DEST_IN_X2APIC_MASK        0x80000000

// Type
#define PCIE_VDM_NODE_CTRL4_TYPE                            TYPE_SMN

#define PCIE_VDM_NODE_CTRL4_BUS_RANGE_BASE_OFFSET   0
#define PCIE_VDM_NODE_CTRL4_BUS_RANGE_BASE_WIDTH    8
#define PCIE_VDM_NODE_CTRL4_BUS_RANGE_BASE_MASK     0xff
#define PCIE_VDM_NODE_CTRL4_BUS_RANGE_LIMIT_OFFSET  8
#define PCIE_VDM_NODE_CTRL4_BUS_RANGE_LIMIT_WIDTH   8
#define PCIE_VDM_NODE_CTRL4_BUS_RANGE_LIMIT_MASK    0xff00
#define PCIE_VDM_NODE_CTRL4_Reserved_30_16_OFFSET   16
#define PCIE_VDM_NODE_CTRL4_Reserved_30_16_WIDTH    15
#define PCIE_VDM_NODE_CTRL4_Reserved_30_16_MASK     0x7fff0000
#define PCIE_VDM_NODE_CTRL4_NODE_PRESENT_OFFSET     31
#define PCIE_VDM_NODE_CTRL4_NODE_PRESENT_WIDTH      1
#define PCIE_VDM_NODE_CTRL4_NODE_PRESENT_MASK       0x80000000L

/// PCIE_VDM_NODE_CTRL4
typedef union {
  struct {
    UINT32    BUS_RANGE_BASE  : 8;                                     ///<
    UINT32    BUS_RANGE_LIMIT : 8;                                     ///<
    UINT32    Reserved_30_16  : 15;                                    ///<
    UINT32    NODE_PRESENT    : 1;                                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_VDM_NODE_CTRL4_STRUCT;

// Register Name PCIE_CNTL

// Address
#define PCIE0_CNTL_ADDRESS_HYEX                (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80040)
#define PCIE1_CNTL_ADDRESS_HYEX                (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80040)
#define PCIE2_CNTL_ADDRESS_HYEX                (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80040)

#define PCIE0_CNTL_ADDRESS_HYGX                (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80040)
#define PCIE2_CNTL_ADDRESS_HYGX                (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80040)

// Type
#define PCIE_CNTL_TYPE                                 TYPE_SMN

#define PCIE_CNTL_HWINIT_WR_LOCK_OFFSET                0
#define PCIE_CNTL_HWINIT_WR_LOCK_WIDTH                 1
#define PCIE_CNTL_HWINIT_WR_LOCK_MASK                  0x1
#define PCIE_CNTL_LC_HOT_PLUG_DELAY_SEL_OFFSET         1
#define PCIE_CNTL_LC_HOT_PLUG_DELAY_SEL_WIDTH          3
#define PCIE_CNTL_LC_HOT_PLUG_DELAY_SEL_MASK           0xe
#define PCIE_CNTL_Reserved_6_4_OFFSET                  4
#define PCIE_CNTL_Reserved_6_4_WIDTH                   3
#define PCIE_CNTL_Reserved_6_4_MASK                    0x70
#define PCIE_CNTL_UR_ERR_REPORT_DIS_OFFSET             7
#define PCIE_CNTL_UR_ERR_REPORT_DIS_WIDTH              1
#define PCIE_CNTL_UR_ERR_REPORT_DIS_MASK               0x80
#define PCIE_CNTL_PCIE_MALFORM_ATOMIC_OPS_OFFSET       8
#define PCIE_CNTL_PCIE_MALFORM_ATOMIC_OPS_WIDTH        1
#define PCIE_CNTL_PCIE_MALFORM_ATOMIC_OPS_MASK         0x100
#define PCIE_CNTL_PCIE_HT_NP_MEM_WRITE_OFFSET          9
#define PCIE_CNTL_PCIE_HT_NP_MEM_WRITE_WIDTH           1
#define PCIE_CNTL_PCIE_HT_NP_MEM_WRITE_MASK            0x200
#define PCIE_CNTL_RX_SB_ADJ_PAYLOAD_SIZE_OFFSET        10
#define PCIE_CNTL_RX_SB_ADJ_PAYLOAD_SIZE_WIDTH         3
#define PCIE_CNTL_RX_SB_ADJ_PAYLOAD_SIZE_MASK          0x1c00
#define PCIE_CNTL_Reserved_14_13_OFFSET                13
#define PCIE_CNTL_Reserved_14_13_WIDTH                 2
#define PCIE_CNTL_Reserved_14_13_MASK                  0x6000
#define PCIE_CNTL_RX_RCB_ATS_UC_DIS_OFFSET             15
#define PCIE_CNTL_RX_RCB_ATS_UC_DIS_WIDTH              1
#define PCIE_CNTL_RX_RCB_ATS_UC_DIS_MASK               0x8000
#define PCIE_CNTL_RX_RCB_REORDER_EN_OFFSET             16
#define PCIE_CNTL_RX_RCB_REORDER_EN_WIDTH              1
#define PCIE_CNTL_RX_RCB_REORDER_EN_MASK               0x10000
#define PCIE_CNTL_RX_RCB_INVALID_SIZE_DIS_OFFSET       17
#define PCIE_CNTL_RX_RCB_INVALID_SIZE_DIS_WIDTH        1
#define PCIE_CNTL_RX_RCB_INVALID_SIZE_DIS_MASK         0x20000
#define PCIE_CNTL_RX_RCB_UNEXP_CPL_DIS_OFFSET          18
#define PCIE_CNTL_RX_RCB_UNEXP_CPL_DIS_WIDTH           1
#define PCIE_CNTL_RX_RCB_UNEXP_CPL_DIS_MASK            0x40000
#define PCIE_CNTL_RX_RCB_CPL_TIMEOUT_TEST_MODE_OFFSET  19
#define PCIE_CNTL_RX_RCB_CPL_TIMEOUT_TEST_MODE_WIDTH   1
#define PCIE_CNTL_RX_RCB_CPL_TIMEOUT_TEST_MODE_MASK    0x80000
#define PCIE_CNTL_RX_RCB_WRONG_PREFIX_DIS_OFFSET       20
#define PCIE_CNTL_RX_RCB_WRONG_PREFIX_DIS_WIDTH        1
#define PCIE_CNTL_RX_RCB_WRONG_PREFIX_DIS_MASK         0x100000
#define PCIE_CNTL_RX_RCB_WRONG_ATTR_DIS_OFFSET         21
#define PCIE_CNTL_RX_RCB_WRONG_ATTR_DIS_WIDTH          1
#define PCIE_CNTL_RX_RCB_WRONG_ATTR_DIS_MASK           0x200000
#define PCIE_CNTL_RX_RCB_WRONG_FUNCNUM_DIS_OFFSET      22
#define PCIE_CNTL_RX_RCB_WRONG_FUNCNUM_DIS_WIDTH       1
#define PCIE_CNTL_RX_RCB_WRONG_FUNCNUM_DIS_MASK        0x400000
#define PCIE_CNTL_RX_ATS_TRAN_CPL_SPLIT_DIS_OFFSET     23
#define PCIE_CNTL_RX_ATS_TRAN_CPL_SPLIT_DIS_WIDTH      1
#define PCIE_CNTL_RX_ATS_TRAN_CPL_SPLIT_DIS_MASK       0x800000
#define PCIE_CNTL_TX_CPL_DEBUG_OFFSET                  24
#define PCIE_CNTL_TX_CPL_DEBUG_WIDTH                   6
#define PCIE_CNTL_TX_CPL_DEBUG_MASK                    0x3f000000
#define PCIE_CNTL_RX_IGNORE_LTR_MSG_UR_OFFSET          30
#define PCIE_CNTL_RX_IGNORE_LTR_MSG_UR_WIDTH           1
#define PCIE_CNTL_RX_IGNORE_LTR_MSG_UR_MASK            0x40000000
#define PCIE_CNTL_RX_CPL_POSTED_REQ_ORD_EN_OFFSET      31
#define PCIE_CNTL_RX_CPL_POSTED_REQ_ORD_EN_WIDTH       1
#define PCIE_CNTL_RX_CPL_POSTED_REQ_ORD_EN_MASK        0x80000000L

/// PCIE_CNTL
typedef union {
  struct {
    UINT32    HWINIT_WR_LOCK               : 1;                        ///<
    UINT32    LC_HOT_PLUG_DELAY_SEL        : 3;                        ///<
    UINT32    Reserved_6_4                 : 3;                        ///<
    UINT32    UR_ERR_REPORT_DIS            : 1;                        ///<
    UINT32    PCIE_MALFORM_ATOMIC_OPS      : 1;                        ///<
    UINT32    PCIE_HT_NP_MEM_WRITE         : 1;                        ///<
    UINT32    RX_SB_ADJ_PAYLOAD_SIZE       : 3;                        ///<
    UINT32    Reserved_14_13               : 2;                        ///<
    UINT32    RX_RCB_ATS_UC_DIS            : 1;                        ///<
    UINT32    RX_RCB_REORDER_EN            : 1;                        ///<
    UINT32    RX_RCB_INVALID_SIZE_DIS      : 1;                        ///<
    UINT32    RX_RCB_UNEXP_CPL_DIS         : 1;                        ///<
    UINT32    RX_RCB_CPL_TIMEOUT_TEST_MODE : 1;                        ///<
    UINT32    RX_RCB_WRONG_PREFIX_DIS      : 1;                        ///<
    UINT32    RX_RCB_WRONG_ATTR_DIS        : 1;                        ///<
    UINT32    RX_RCB_WRONG_FUNCNUM_DIS     : 1;                        ///<
    UINT32    RX_ATS_TRAN_CPL_SPLIT_DIS    : 1;                        ///<
    UINT32    TX_CPL_DEBUG                 : 6;                        ///<
    UINT32    RX_IGNORE_LTR_MSG_UR         : 1;                        ///<
    UINT32    RX_CPL_POSTED_REQ_ORD_EN     : 1;                        ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_CNTL_STRUCT;

// Register Name PCIE_CNTL2

// Address
#define PCIE0_CNTL2_ADDRESS_HYEX                (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80070)
#define PCIE1_CNTL2_ADDRESS_HYEX                (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80070)
#define PCIE2_CNTL2_ADDRESS_HYEX                (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80070)

#define PCIE0_CNTL2_ADDRESS_HYGX                (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80070)
#define PCIE2_CNTL2_ADDRESS_HYGX                (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80070)
// Address
#define SMU_PCIE0_FENCED2_REG_HYEX              (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80604)
#define SMU_PCIE1_FENCED2_REG_HYEX              (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80604)
#define SMU_PCIE2_FENCED2_REG_HYEX              (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80604)

#define SMU_PCIE0_FENCED2_REG_HYGX              (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80604)
#define SMU_PCIE2_FENCED2_REG_HYGX              (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80604)
// Address
#define PCIE0_GPP0_UNCORR_ERR_MASK_HYEX         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00158)
#define PCIE1_GPP0_UNCORR_ERR_MASK_HYEX         (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00158)
#define PCIE2_GPP0_UNCORR_ERR_MASK_HYEX         (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00158)

#define PCIE0_GPP0_UNCORR_ERR_MASK_HYGX         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00158)
#define PCIE2_GPP0_UNCORR_ERR_MASK_HYGX         (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00158)
// Address
#define PCIE0_GPP0_UNCORR_ERR_SEVERITY_HYEX     (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x0015c)
#define PCIE1_GPP0_UNCORR_ERR_SEVERITY_HYEX     (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x0015c)
#define PCIE2_GPP0_UNCORR_ERR_SEVERITY_HYEX     (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x0015c)

#define PCIE0_GPP0_UNCORR_ERR_SEVERITY_HYGX     (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x0015c)
#define PCIE2_GPP0_UNCORR_ERR_SEVERITY_HYGX     (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x0015c)
// Address
#define PCIE0_GPP0_CORR_ERR_MASK_HYEX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00164)
#define PCIE1_GPP0_CORR_ERR_MASK_HYEX           (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00164)
#define PCIE2_GPP0_CORR_ERR_MASK_HYEX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00164)

#define PCIE0_GPP0_CORR_ERR_MASK_HYGX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00164)
#define PCIE2_GPP0_CORR_ERR_MASK_HYGX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00164)

#define PCIE0_TX_ACK_LATENCY_LIMIT_HYEX         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40098)
#define PCIE1_TX_ACK_LATENCY_LIMIT_HYEX         (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40098)
#define PCIE2_TX_ACK_LATENCY_LIMIT_HYEX         (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40098)

#define PCIE0_TX_ACK_LATENCY_LIMIT_HYGX         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40098)
#define PCIE2_TX_ACK_LATENCY_LIMIT_HYGX         (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40098)

#define TX_ACK_LATENCY_ACCELERATE_OFFSET       31
#define TX_ACK_LATENCY_ACCELERATE_WIDTH        1
#define TX_ACK_LATENCY_ACCELERATE_MASK         0x1

// Address
#define PCIE0_DEVICE_CNTL2_ADDRESS_HYEX                        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00080)
#define PCIE0_DEVICE_CNTL2_ADDRESS_HYGX                        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00080)
#define DEVICE_CNTL2_TEN_BIT_TAG_REQUESTER_ENABLE_OFFSET       12
#define DEVICE_CNTL2_TEN_BIT_TAG_REQUESTER_ENABLE_WIDTH        1
#define DEVICE_CNTL2_TEN_BIT_TAG_REQUESTER_ENABLE_MASK         0x1000

// Type
#define PCIE_CNTL2_TYPE                                 TYPE_SMN

#define PCIE_CNTL2_TX_ARB_ROUND_ROBIN_EN_OFFSET         0
#define PCIE_CNTL2_TX_ARB_ROUND_ROBIN_EN_WIDTH          1
#define PCIE_CNTL2_TX_ARB_ROUND_ROBIN_EN_MASK           0x1
#define PCIE_CNTL2_TX_ARB_SLV_LIMIT_OFFSET              1
#define PCIE_CNTL2_TX_ARB_SLV_LIMIT_WIDTH               5
#define PCIE_CNTL2_TX_ARB_SLV_LIMIT_MASK                0x3e
#define PCIE_CNTL2_TX_ARB_MST_LIMIT_OFFSET              6
#define PCIE_CNTL2_TX_ARB_MST_LIMIT_WIDTH               5
#define PCIE_CNTL2_TX_ARB_MST_LIMIT_MASK                0x7c0
#define PCIE_CNTL2_TX_BLOCK_TLP_ON_PM_DIS_OFFSET        11
#define PCIE_CNTL2_TX_BLOCK_TLP_ON_PM_DIS_WIDTH         1
#define PCIE_CNTL2_TX_BLOCK_TLP_ON_PM_DIS_MASK          0x800
#define PCIE_CNTL2_TX_NP_MEM_WRITE_SWP_ENCODING_OFFSET  12
#define PCIE_CNTL2_TX_NP_MEM_WRITE_SWP_ENCODING_WIDTH   1
#define PCIE_CNTL2_TX_NP_MEM_WRITE_SWP_ENCODING_MASK    0x1000
#define PCIE_CNTL2_TX_ATOMIC_OPS_DISABLE_OFFSET         13
#define PCIE_CNTL2_TX_ATOMIC_OPS_DISABLE_WIDTH          1
#define PCIE_CNTL2_TX_ATOMIC_OPS_DISABLE_MASK           0x2000
#define PCIE_CNTL2_TX_ATOMIC_ORDERING_DIS_OFFSET        14
#define PCIE_CNTL2_TX_ATOMIC_ORDERING_DIS_WIDTH         1
#define PCIE_CNTL2_TX_ATOMIC_ORDERING_DIS_MASK          0x4000
#define PCIE_CNTL2_Reserved_15_15_OFFSET                15
#define PCIE_CNTL2_Reserved_15_15_WIDTH                 1
#define PCIE_CNTL2_Reserved_15_15_MASK                  0x8000
#define PCIE_CNTL2_SLV_MEM_LS_EN_OFFSET                 16
#define PCIE_CNTL2_SLV_MEM_LS_EN_WIDTH                  1
#define PCIE_CNTL2_SLV_MEM_LS_EN_MASK                   0x10000
#define PCIE_CNTL2_SLV_MEM_AGGRESSIVE_LS_EN_OFFSET      17
#define PCIE_CNTL2_SLV_MEM_AGGRESSIVE_LS_EN_WIDTH       1
#define PCIE_CNTL2_SLV_MEM_AGGRESSIVE_LS_EN_MASK        0x20000
#define PCIE_CNTL2_MST_MEM_LS_EN_OFFSET                 18
#define PCIE_CNTL2_MST_MEM_LS_EN_WIDTH                  1
#define PCIE_CNTL2_MST_MEM_LS_EN_MASK                   0x40000
#define PCIE_CNTL2_REPLAY_MEM_LS_EN_OFFSET              19
#define PCIE_CNTL2_REPLAY_MEM_LS_EN_WIDTH               1
#define PCIE_CNTL2_REPLAY_MEM_LS_EN_MASK                0x80000
#define PCIE_CNTL2_SLV_MEM_SD_EN_OFFSET                 20
#define PCIE_CNTL2_SLV_MEM_SD_EN_WIDTH                  1
#define PCIE_CNTL2_SLV_MEM_SD_EN_MASK                   0x100000
#define PCIE_CNTL2_SLV_MEM_AGGRESSIVE_SD_EN_OFFSET      21
#define PCIE_CNTL2_SLV_MEM_AGGRESSIVE_SD_EN_WIDTH       1
#define PCIE_CNTL2_SLV_MEM_AGGRESSIVE_SD_EN_MASK        0x200000
#define PCIE_CNTL2_MST_MEM_SD_EN_OFFSET                 22
#define PCIE_CNTL2_MST_MEM_SD_EN_WIDTH                  1
#define PCIE_CNTL2_MST_MEM_SD_EN_MASK                   0x400000
#define PCIE_CNTL2_REPLAY_MEM_SD_EN_OFFSET              23
#define PCIE_CNTL2_REPLAY_MEM_SD_EN_WIDTH               1
#define PCIE_CNTL2_REPLAY_MEM_SD_EN_MASK                0x800000
#define PCIE_CNTL2_RX_NP_MEM_WRITE_ENCODING_OFFSET      24
#define PCIE_CNTL2_RX_NP_MEM_WRITE_ENCODING_WIDTH       5
#define PCIE_CNTL2_RX_NP_MEM_WRITE_ENCODING_MASK        0x1f000000
#define PCIE_CNTL2_SLV_MEM_DS_EN_OFFSET                 29
#define PCIE_CNTL2_SLV_MEM_DS_EN_WIDTH                  1
#define PCIE_CNTL2_SLV_MEM_DS_EN_MASK                   0x20000000
#define PCIE_CNTL2_MST_MEM_DS_EN_OFFSET                 30
#define PCIE_CNTL2_MST_MEM_DS_EN_WIDTH                  1
#define PCIE_CNTL2_MST_MEM_DS_EN_MASK                   0x40000000
#define PCIE_CNTL2_REPLAY_MEM_DS_EN_OFFSET              31
#define PCIE_CNTL2_REPLAY_MEM_DS_EN_WIDTH               1
#define PCIE_CNTL2_REPLAY_MEM_DS_EN_MASK                0x80000000L

/// PCIECORE0_00000070
typedef union {
  struct {
    UINT32    TX_ARB_ROUND_ROBIN_EN        : 1;                        ///<
    UINT32    TX_ARB_SLV_LIMIT             : 5;                        ///<
    UINT32    TX_ARB_MST_LIMIT             : 5;                        ///<
    UINT32    TX_BLOCK_TLP_ON_PM_DIS       : 1;                        ///<
    UINT32    TX_NP_MEM_WRITE_SWP_ENCODING : 1;                        ///<
    UINT32    TX_ATOMIC_OPS_DISABLE        : 1;                        ///<
    UINT32    TX_ATOMIC_ORDERING_DIS       : 1;                        ///<
    UINT32    Reserved_15_15               : 1;                        ///<
    UINT32    SLV_MEM_LS_EN                : 1;                        ///<
    UINT32    SLV_MEM_AGGRESSIVE_LS_EN     : 1;                        ///<
    UINT32    MST_MEM_LS_EN                : 1;                        ///<
    UINT32    REPLAY_MEM_LS_EN             : 1;                        ///<
    UINT32    SLV_MEM_SD_EN                : 1;                        ///<
    UINT32    SLV_MEM_AGGRESSIVE_SD_EN     : 1;                        ///<
    UINT32    MST_MEM_SD_EN                : 1;                        ///<
    UINT32    REPLAY_MEM_SD_EN             : 1;                        ///<
    UINT32    RX_NP_MEM_WRITE_ENCODING     : 5;                        ///<
    UINT32    SLV_MEM_DS_EN                : 1;                        ///<
    UINT32    MST_MEM_DS_EN                : 1;                        ///<
    UINT32    REPLAY_MEM_DS_EN             : 1;                        ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_CNTL2_STRUCT;

#define PCIE0_SELECT_BDF_ID_HYEX                       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x804e4)
#define PCIE1_SELECT_BDF_ID_HYEX                       (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x804e4)
#define PCIE2_SELECT_BDF_ID_HYEX                       (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x804e4)

#define PCIE0_SELECT_BDF_ID_HYGX                       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x804e4)
#define PCIE2_SELECT_BDF_ID_HYGX                       (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x804e4)

// Register Name PCIE_P_CNTL

// Address
#define PCIE0_P_CNTL_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80100)
#define PCIE1_P_CNTL_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80100)
#define PCIE2_P_CNTL_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80100)

#define PCIE0_P_CNTL_ADDRESS_HYGX                      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80100)
#define PCIE2_P_CNTL_ADDRESS_HYGX                      (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80100)

// Type
#define PCIE_P_CNTL_TYPE                                          TYPE_SMN

#define PCIE_P_CNTL_P_PWRDN_EN_OFFSET                             0
#define PCIE_P_CNTL_P_PWRDN_EN_WIDTH                              1
#define PCIE_P_CNTL_P_PWRDN_EN_MASK                               0x1
#define PCIE_P_CNTL_P_SYMALIGN_MODE_OFFSET                        1
#define PCIE_P_CNTL_P_SYMALIGN_MODE_WIDTH                         1
#define PCIE_P_CNTL_P_SYMALIGN_MODE_MASK                          0x2
#define PCIE_P_CNTL_P_SYMALIGN_HW_DEBUG_OFFSET                    2
#define PCIE_P_CNTL_P_SYMALIGN_HW_DEBUG_WIDTH                     1
#define PCIE_P_CNTL_P_SYMALIGN_HW_DEBUG_MASK                      0x4
#define PCIE_P_CNTL_P_ELASTDESKEW_HW_DEBUG_OFFSET                 3
#define PCIE_P_CNTL_P_ELASTDESKEW_HW_DEBUG_WIDTH                  1
#define PCIE_P_CNTL_P_ELASTDESKEW_HW_DEBUG_MASK                   0x8
#define PCIE_P_CNTL_P_IGNORE_CRC_ERR_OFFSET                       4
#define PCIE_P_CNTL_P_IGNORE_CRC_ERR_WIDTH                        1
#define PCIE_P_CNTL_P_IGNORE_CRC_ERR_MASK                         0x10
#define PCIE_P_CNTL_P_IGNORE_LEN_ERR_OFFSET                       5
#define PCIE_P_CNTL_P_IGNORE_LEN_ERR_WIDTH                        1
#define PCIE_P_CNTL_P_IGNORE_LEN_ERR_MASK                         0x20
#define PCIE_P_CNTL_P_IGNORE_EDB_ERR_OFFSET                       6
#define PCIE_P_CNTL_P_IGNORE_EDB_ERR_WIDTH                        1
#define PCIE_P_CNTL_P_IGNORE_EDB_ERR_MASK                         0x40
#define PCIE_P_CNTL_P_IGNORE_IDL_ERR_OFFSET                       7
#define PCIE_P_CNTL_P_IGNORE_IDL_ERR_WIDTH                        1
#define PCIE_P_CNTL_P_IGNORE_IDL_ERR_MASK                         0x80
#define PCIE_P_CNTL_P_IGNORE_TOK_ERR_OFFSET                       8
#define PCIE_P_CNTL_P_IGNORE_TOK_ERR_WIDTH                        1
#define PCIE_P_CNTL_P_IGNORE_TOK_ERR_MASK                         0x100
#define PCIE_P_CNTL_DESKEW_EXTEND_RXVALID_AFTER_EIOS_OFFSET       9
#define PCIE_P_CNTL_DESKEW_EXTEND_RXVALID_AFTER_EIOS_WIDTH        1
#define PCIE_P_CNTL_DESKEW_EXTEND_RXVALID_AFTER_EIOS_MASK         0x200
#define PCIE_P_CNTL_Reserved_11_10_OFFSET                         10
#define PCIE_P_CNTL_Reserved_11_10_WIDTH                          2
#define PCIE_P_CNTL_Reserved_11_10_MASK                           0xc00
#define PCIE_P_CNTL_P_BLK_LOCK_MODE_OFFSET                        12
#define PCIE_P_CNTL_P_BLK_LOCK_MODE_WIDTH                         1
#define PCIE_P_CNTL_P_BLK_LOCK_MODE_MASK                          0x1000
#define PCIE_P_CNTL_P_ALWAYS_USE_FAST_TXCLK_OFFSET                13
#define PCIE_P_CNTL_P_ALWAYS_USE_FAST_TXCLK_WIDTH                 1
#define PCIE_P_CNTL_P_ALWAYS_USE_FAST_TXCLK_MASK                  0x2000
#define PCIE_P_CNTL_P_ELEC_IDLE_MODE_OFFSET                       14
#define PCIE_P_CNTL_P_ELEC_IDLE_MODE_WIDTH                        2
#define PCIE_P_CNTL_P_ELEC_IDLE_MODE_MASK                         0xc000
#define PCIE_P_CNTL_DLP_IGNORE_IN_L1_EN_OFFSET                    16
#define PCIE_P_CNTL_DLP_IGNORE_IN_L1_EN_WIDTH                     1
#define PCIE_P_CNTL_DLP_IGNORE_IN_L1_EN_MASK                      0x10000
#define PCIE_P_CNTL_ASSERT_DVALID_ON_EI_TRANS_OFFSET              17
#define PCIE_P_CNTL_ASSERT_DVALID_ON_EI_TRANS_WIDTH               1
#define PCIE_P_CNTL_ASSERT_DVALID_ON_EI_TRANS_MASK                0x20000
#define PCIE_P_CNTL_Reserved_19_18_OFFSET                         18
#define PCIE_P_CNTL_Reserved_19_18_WIDTH                          2
#define PCIE_P_CNTL_Reserved_19_18_MASK                           0xc0000
#define PCIE_P_CNTL_P_IGNORE_TOKEN_ERR_ALL_OFFSET                 20
#define PCIE_P_CNTL_P_IGNORE_TOKEN_ERR_ALL_WIDTH                  1
#define PCIE_P_CNTL_P_IGNORE_TOKEN_ERR_ALL_MASK                   0x100000
#define PCIE_P_CNTL_RXP_DISABLE_EXTEND_DATAVLD_64_OFFSET          21
#define PCIE_P_CNTL_RXP_DISABLE_EXTEND_DATAVLD_64_WIDTH           1
#define PCIE_P_CNTL_RXP_DISABLE_EXTEND_DATAVLD_64_MASK            0x200000
#define PCIE_P_CNTL_RXP_DISABLE_GOTO_END_EDS_OFFSET               22
#define PCIE_P_CNTL_RXP_DISABLE_GOTO_END_EDS_WIDTH                1
#define PCIE_P_CNTL_RXP_DISABLE_GOTO_END_EDS_MASK                 0x400000
#define PCIE_P_CNTL_RXP_X4_CHANGE_PATH_OFFSET                     23
#define PCIE_P_CNTL_RXP_X4_CHANGE_PATH_WIDTH                      1
#define PCIE_P_CNTL_RXP_X4_CHANGE_PATH_MASK                       0x800000
#define PCIE_P_CNTL_P_IGNORE_TOK_ERR_BD_OFFSET                    24
#define PCIE_P_CNTL_P_IGNORE_TOK_ERR_BD_WIDTH                     1
#define PCIE_P_CNTL_P_IGNORE_TOK_ERR_BD_MASK                      0x1000000
#define PCIE_P_CNTL_RXP_GOTO_RECOVERY_GEN12_OFFSET                25
#define PCIE_P_CNTL_RXP_GOTO_RECOVERY_GEN12_WIDTH                 1
#define PCIE_P_CNTL_RXP_GOTO_RECOVERY_GEN12_MASK                  0x2000000
#define PCIE_P_CNTL_Reserved_31_26_OFFSET                         26
#define PCIE_P_CNTL_Reserved_31_26_WIDTH                          6
#define PCIE_P_CNTL_Reserved_31_26_MASK                           0xfc000000

/// PCIECORE0_00000100
typedef union {
  struct {
    UINT32    P_PWRDN_EN                            : 1;                       ///<
    UINT32    P_SYMALIGN_MODE                       : 1;                       ///<
    UINT32    P_SYMALIGN_HW_DEBUG                   : 1;                       ///<
    UINT32    P_ELASTDESKEW_HW_DEBUG                : 1;                       ///<
    UINT32    P_IGNORE_CRC_ERR                      : 1;                       ///<
    UINT32    P_IGNORE_LEN_ERR                      : 1;                       ///<
    UINT32    P_IGNORE_EDB_ERR                      : 1;                       ///<
    UINT32    P_IGNORE_IDL_ERR                      : 1;                       ///<
    UINT32    P_IGNORE_TOK_ERR                      : 1;                       ///<
    UINT32    DESKEW_EXTEND_RXVALID_AFTER_EIOS      : 1;                       ///<
    UINT32    Reserved_11_10                        : 2;                       ///<
    UINT32    P_BLK_LOCK_MODE                       : 1;                       ///<
    UINT32    P_ALWAYS_USE_FAST_TXCLK               : 1;                       ///<
    UINT32    P_ELEC_IDLE_MODE                      : 2;                       ///<
    UINT32    DLP_IGNORE_IN_L1_EN                   : 1;                       ///<
    UINT32    ASSERT_DVALID_ON_EI_TRANS             : 1;                       ///<
    UINT32    Reserved_19_18                        : 2;                       ///<
    UINT32    P_IGNORE_TOKEN_ERR_ALL                : 1;                       ///<
    UINT32    RXP_DISABLE_EXTEND_DATAVLD_64         : 1;                       ///<
    UINT32    RXP_DISABLE_GOTO_END_EDS              : 1;                       ///<
    UINT32    RXP_X4_CHANGE_PATH                    : 1;                       ///<
    UINT32    P_IGNORE_TOK_ERR_BD                   : 1;                       ///<
    UINT32    RXP_GOTO_RECOVERY_GEN12               : 1;                       ///<
    UINT32    Reserved_31_26                        : 6;                       ///<
  } Field;                                                                     ///<
  UINT32    Value;                                                             ///<
} PCIE_P_CNTL_STRUCT;

// Address
#define PCIE0_STRAP_MISC_ADDRESS_HYEX      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80300)
#define PCIE1_STRAP_MISC_ADDRESS_HYEX      (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80300)
#define PCIE2_STRAP_MISC_ADDRESS_HYEX      (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80300)

#define PCIE0_STRAP_MISC_ADDRESS_HYGX      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80300)
#define PCIE2_STRAP_MISC_ADDRESS_HYGX      (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80300)

#define STRAP_DLF_EN_OFFSET            0
#define STRAP_DLF_EN_WIDTH             1
#define STRAP_DLF_EN_MASK              0x1
#define STRAP_16G_EN_OFFSET            1
#define STRAP_16G_EN_WIDTH             1
#define STRAP_16G_EN_MASK              0x2
#define STRAP_32G_EN_OFFSET            2
#define STRAP_32G_EN_WIDTH             1
#define STRAP_32G_EN_MASK              0x4
#define STRAP_MARGINING_EN_OFFSET      3
#define STRAP_MARGINING_EN_WIDTH       1
#define STRAP_MARGINING_EN_MASK        0x8
#define STRAP_TL_ALT_BUF_EN_OFFSET     4
#define STRAP_TL_ALT_BUF_EN_WIDTH      1
#define STRAP_TL_ALT_BUF_EN_MASK       0x10
#define STRAP_PRECODING_EN_OFFSET      5
#define STRAP_PRECODING_EN_WIDTH       1
#define STRAP_PRECODING_EN_MASK        0x20
#define STRAP_BYPASS_SCRAMBLER_OFFSET  6
#define STRAP_BYPASS_SCRAMBLER_WIDTH   1
#define STRAP_BYPASS_SCRAMBLER_MASK    0x40
#define STRAP_CLK_PM_EN_OFFSET         24
#define STRAP_CLK_PM_EN_WIDTH          1
#define STRAP_CLK_PM_EN_MASK           0x1000000
#define STRAP_ECN1P1_EN_OFFSET         25
#define STRAP_ECN1P1_EN_WIDTH          1
#define STRAP_ECN1P1_EN_MASK           0x2000000
#define STRAP_EXT_VC_COUNT_OFFSET      26
#define STRAP_EXT_VC_COUNT_WIDTH       1
#define STRAP_EXT_VC_COUNT_MASK        0x4000000
#define STRAP_REVERSE_ALL_OFFSET       28
#define STRAP_REVERSE_ALL_WIDTH        1
#define STRAP_REVERSE_ALL_MASK         0x10000000
#define STRAP_MST_ADR64_EN_OFFSET      29
#define STRAP_MST_ADR64_EN_WIDTH       1
#define STRAP_MST_ADR64_EN_MASK        0x20000000
#define STRAP_FLR_EN_OFFSET            30
#define STRAP_FLR_EN_WIDTH             1
#define STRAP_FLR_EN_MASK              0x40000000
#define STRAP_INTERNAL_ERR_EN_OFFSET   31
#define STRAP_INTERNAL_ERR_EN_WIDTH    1
#define STRAP_INTERNAL_ERR_EN_MASK     0x80000000
/// PCIE_STRAP_MISC
typedef union {
  struct {
    UINT32    STRAP_DLF_EN           : 1;                      ///<
    UINT32    STRAP_16G_EN           : 1;                      ///<
    UINT32    STRAP_32G_EN           : 1;                      ///<
    UINT32    STRAP_MARGINING_EN     : 1;                      ///<
    UINT32    STRAP_TL_ALT_BUF_EN    : 1;                      ///<
    UINT32    STRAP_PRECODING_EN     : 1;                      ///<
    UINT32    STRAP_BYPASS_SCRAMBLER : 1;                      ///<
    UINT32    Reserved_23_7          : 17;                     ///<
    UINT32    STRAP_CLK_PM_EN        : 1;                      ///<
    UINT32    STRAP_ECN1P1_EN        : 1;                      ///<
    UINT32    STRAP_EXT_VC_COUNT     : 1;                      ///<
    UINT32    Reserved_27_27         : 1;                      ///<
    UINT32    STRAP_REVERSE_ALL      : 1;                      ///<
    UINT32    STRAP_MST_ADR64_EN     : 1;                      ///<
    UINT32    STRAP_FLR_EN           : 1;                      ///<
    UINT32    STRAP_INTERNAL_ERR_EN  : 1;                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_STRAP_MISC;

// Address
#define PCIE0_STRAP_MISC2_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80304)
#define PCIE1_STRAP_MISC2_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80304)
#define PCIE2_STRAP_MISC2_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80304)

#define PCIE0_STRAP_MISC2_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80304)
#define PCIE2_STRAP_MISC2_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80304)

#define STRAP_LINK_NOTIFICATION_CAP_EN_OFFSET  0
#define STRAP_LINK_NOTIFICATION_CAP_EN_WIDTH   1
#define STRAP_LINK_NOTIFICATION_CAP_EN_MASK    0x1
#define STRAP_GEN2_COMPLIANCE_OFFSET           1
#define STRAP_GEN2_COMPLIANCE_WIDTH            1
#define STRAP_GEN2_COMPLIANCE_MASK             0x2
#define STRAP_MSTCPL_TIMEOUT_EN_OFFSET         2
#define STRAP_MSTCPL_TIMEOUT_EN_WIDTH          1
#define STRAP_MSTCPL_TIMEOUT_EN_MASK           0x4
#define STRAP_GEN3_COMPLIANCE_OFFSET           3
#define STRAP_GEN3_COMPLIANCE_WIDTH            1
#define STRAP_GEN3_COMPLIANCE_MASK             0x8
#define STRAP_TPH_SUPPORTED_OFFSET             4
#define STRAP_TPH_SUPPORTED_WIDTH              1
#define STRAP_TPH_SUPPORTED_MASK               0x10
#define STRAP_GEN4_COMPLIANCE_OFFSET           5
#define STRAP_GEN4_COMPLIANCE_WIDTH            1
#define STRAP_GEN4_COMPLIANCE_MASK             0x20
#define STRAP_GEN5_COMPLIANCE_OFFSET           6
#define STRAP_GEN5_COMPLIANCE_WIDTH            1
#define STRAP_GEN5_COMPLIANCE_MASK             0x40
#define STRAP_BYPASS_EQ_EN_OFFSET              7
#define STRAP_BYPASS_EQ_EN_WIDTH               1
#define STRAP_BYPASS_EQ_EN_MASK                0x80

/// PCIE_STRAP_MISC
typedef union {
  struct {
    UINT32    STRAP_LINK_BW_NOTIFICATION_CAP_EN : 1;           ///<
    UINT32    STRAP_GEN2_COMPLIANCE             : 1;           ///<
    UINT32    STRAP_MSTCPL_TIMEOUT_EN           : 1;           ///<
    UINT32    STRAP_GEN3_COMPLIANCE             : 1;           ///<
    UINT32    STRAP_TPH_SUPPORTED               : 1;           ///<
    UINT32    STRAP_GEN4_COMPLIANCE             : 1;           ///<
    UINT32    STRAP_GEN5_COMPLIANCE             : 1;           ///<
    UINT32    STRAP_BYPASS_EQ_EN                : 1;           ///<
    UINT32    Reserved                          : 24;          ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_STRAP_MISC2;

// Address
#define NBIF0_PCIEP_STRAP_MISC_ADDRESS_HYEX                    (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x235CC)
#define NBIF1_PCIEP_STRAP_MISC_ADDRESS_HYEX                    (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x235CC)
#define NBIF2_PCIEP_STRAP_MISC_ADDRESS_HYEX                    (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x235CC)

#define NBIF0_PCIEP_STRAP_MISC_ADDRESS_HYGX                    (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x235CC)
#define NBIF1_PCIEP_STRAP_MISC_ADDRESS_HYGX                    (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x235CC)
#define NBIF2_PCIEP_STRAP_MISC_ADDRESS_HYGX                    (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x235CC)
#define NBIF3_PCIEP_STRAP_MISC_ADDRESS_HYGX                    (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x235CC)
// Type
#define NBIF_PCIEP_STRAP_MISC_TYPE                            TYPE_SMN

#define NBIF_PCIEP_STRAP_MISC_STRAP_MULTI_FUNC_EN_OFFSET      10
#define NBIF_PCIEP_STRAP_MISC_STRAP_MULTI_FUNC_EN_WIDTH       1
#define NBIF_PCIEP_STRAP_MISC_STRAP_MULTI_FUNC_EN_MASK        0x400

/// NBIFRCC0_000035CC
typedef union {
  struct {
    UINT32    Reserved_9_0        : 10;                                ///<
    UINT32    STRAP_MULTI_FUNC_EN : 1;                                 ///<
    UINT32    Reserved31_11       : 21;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIF_PCIEP_STRAP_MISC_STRUCT;

// NBIF1~3

// Address
#define NBIF0RCC_DEV0_PORT_STRAP6_ADDRESS_HYEX         (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x31018)
#define NBIF1RCC_DEV0_PORT_STRAP6_ADDRESS_HYEX         (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x31018)
#define NBIF2RCC_DEV0_PORT_STRAP6_ADDRESS_HYEX         (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x31018)

#define NBIF0RCC_DEV0_PORT_STRAP6_ADDRESS_HYGX         (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x31018)
#define NBIF1RCC_DEV0_PORT_STRAP6_ADDRESS_HYGX         (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x31018)
#define NBIF2RCC_DEV0_PORT_STRAP6_ADDRESS_HYGX         (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x31018)
#define NBIF3RCC_DEV0_PORT_STRAP6_ADDRESS_HYGX         (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x31018)

// Type
#define RCC_DEV0_PORT_STRAP6_TYPE                     TYPE_SMN

#define STRAP_CFG_CRS_EN_DEV0_OFFSET                  0
#define STRAP_CFG_CRS_EN_DEV0_WIDTH                   1
#define STRAP_CFG_CRS_EN_DEV0_MASK                    0x1
#define STRAP_SMN_ERR_STATUS_MASK_EN_DNS_DEV0_OFFSET  1
#define STRAP_SMN_ERR_STATUS_MASK_EN_DNS_DEV0_WIDTH   1
#define STRAP_SMN_ERR_STATUS_MASK_EN_DNS_DEV0_MASK    0x2

/// RCC_DEV0_PORT_STRAP6
typedef union {
  struct {
    UINT32    STRAP_CFG_CRS_EN_DEV0                 : 1;               ///<
    UINT32    STRAP_SMN_ERR_STATUS_MASK_EN_DNS_DEV0 : 1;               ///<
    UINT32    Reserved31_2                          : 30;              ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} RCC_DEV0_PORT_STRAP6_STRUCT;

// Address
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x3A024)
#define NBIF1_BIFC_DMA_ATTR_OVERRIDE_F2_F3_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x3A024)
#define NBIF2_BIFC_DMA_ATTR_OVERRIDE_F2_F3_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x3A024)

#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x3A024)
#define NBIF1_BIFC_DMA_ATTR_OVERRIDE_F2_F3_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x3A024)
#define NBIF2_BIFC_DMA_ATTR_OVERRIDE_F2_F3_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x3A024)
#define NBIF3_BIFC_DMA_ATTR_OVERRIDE_F2_F3_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x3A024)

// Type
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TYPE                         TYPE_SMN

#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_P_F2_OFFSET   0
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_P_F2_WIDTH    2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_P_F2_MASK     0x3
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_NP_F2_OFFSET  2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_NP_F2_WIDTH   2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_NP_F2_MASK    0xc
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F2_OFFSET    6
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F2_WIDTH     2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F2_MASK      0xc0
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_NP_F2_OFFSET   8
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_NP_F2_WIDTH    2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_NP_F2_MASK     0x300
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_P_F2_OFFSET   10
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_P_F2_WIDTH    2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_P_F2_MASK     0xc00
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_NP_F2_OFFSET  12
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_NP_F2_WIDTH   2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_NP_F2_MASK    0x3000
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_P_F3_OFFSET   16
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_P_F3_WIDTH    2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_P_F3_MASK     0x30000
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_NP_F3_OFFSET  18
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_NP_F3_WIDTH   2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_IDO_OVERIDE_NP_F3_MASK    0xc0000
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F3_OFFSET    22
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F3_WIDTH     2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F3_MASK      0xc00000
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_NP_F3_OFFSET   24
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_NP_F3_WIDTH    2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_NP_F3_MASK     0x3000000
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_P_F3_OFFSET   26
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_P_F3_WIDTH    2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_P_F3_MASK     0xc000000
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_NP_F3_OFFSET  28
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_NP_F3_WIDTH   2
#define NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_SNR_OVERIDE_NP_F3_MASK    0x30000000
/// NBIFMISC0_00000024
typedef union {
  struct {
    UINT32    TX_IDO_OVERIDE_P_F2  : 2;                                ///<
    UINT32    TX_IDO_OVERIDE_NP_F2 : 2;                                ///<
    UINT32    reserved_5_4         : 2;                                ///<
    UINT32    TX_RO_OVERIDE_P_F2   : 2;                                ///<
    UINT32    TX_RO_OVERIDE_NP_F2  : 2;                                ///<
    UINT32    TX_SNR_OVERIDE_P_F2  : 2;                                ///<
    UINT32    TX_SNR_OVERIDE_NP_F2 : 2;                                ///<
    UINT32    reserved_15_14       : 2;                                ///<
    UINT32    TX_IDO_OVERIDE_P_F3  : 2;                                ///<
    UINT32    TX_IDO_OVERIDE_NP_F3 : 2;                                ///<
    UINT32    reserved_21_20       : 2;                                ///<
    UINT32    TX_RO_OVERIDE_P_F3   : 2;                                ///<
    UINT32    TX_RO_OVERIDE_NP_F3  : 2;                                ///<
    UINT32    TX_SNR_OVERIDE_P_F3  : 2;                                ///<
    UINT32    TX_SNR_OVERIDE_NP_F3 : 2;                                ///<
    UINT32    Reserved_31_30       : 2;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_STRUCT;

// Address
#define NBIFMISC0_INTR_LINE_POLARITY_ADDRESS_HYEX     (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x3A004)
#define NBIFMISC1_INTR_LINE_POLARITY_ADDRESS_HYEX     (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x3A004)
#define NBIFMISC2_INTR_LINE_POLARITY_ADDRESS_HYEX     (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x3A004)

#define NBIFMISC0_INTR_LINE_POLARITY_ADDRESS_HYGX     (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x3A004)
#define NBIFMISC1_INTR_LINE_POLARITY_ADDRESS_HYGX     (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x3A004)
#define NBIFMISC2_INTR_LINE_POLARITY_ADDRESS_HYGX     (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x3A004)
#define NBIFMISC3_INTR_LINE_POLARITY_ADDRESS_HYGX     (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x3A004)
// Type
#define NBIFMISC_INTR_LINE_POLARITY_TYPE             TYPE_SMN

#define NBIFMISC_00000004_INTR_LINE_POLARITY_OFFSET  0
#define NBIFMISC_00000004_INTR_LINE_POLARITY_WIDTH   8
#define NBIFMISC_00000004_INTR_LINE_POLARITY_MASK    0xff

/// NBIFMISC0_00000004
typedef union {
  struct {
    UINT32    INTR_LINE_POLARITY : 8;                                  ///<
    UINT32    Reserved_31_8      : 24;                                 ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIFMISC_00000004_STRUCT;

// Address
#define NBIFMISC0_INTR_LINE_ENABLE_ADDRESS_HYEX    (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x3A008)
#define NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYEX    (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x3A008)
#define NBIFMISC2_INTR_LINE_ENABLE_ADDRESS_HYEX    (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x3A008)

#define NBIFMISC0_INTR_LINE_ENABLE_ADDRESS_HYGX    (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x3A008)
#define NBIFMISC1_INTR_LINE_ENABLE_ADDRESS_HYGX    (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x3A008)
#define NBIFMISC2_INTR_LINE_ENABLE_ADDRESS_HYGX    (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x3A008)
#define NBIFMISC3_INTR_LINE_ENABLE_ADDRESS_HYGX    (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x3A008)
// Type
#define NBIFMISC_INTR_LINE_ENABLE_TYPE             TYPE_SMN

#define NBIFMISC_00000008_INTR_LINE_ENABLE_OFFSET  0
#define NBIFMISC_00000008_INTR_LINE_ENABLE_WIDTH   8
#define NBIFMISC_00000008_INTR_LINE_ENABLE_MASK    0xff

/// NBIFMISC0_00000008
typedef union {
  struct {
    UINT32    INTR_LINE_ENABLE : 8;                                    ///<
    UINT32    Reserved_31_8    : 24;                                   ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIFMISC_00000008_STRUCT;

// Address
#define SB_LOCATION_ADDRESS_HYEX                   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1007C)
#define SB_LOCATION_ADDRESS_HYGX                   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1007C)

// Type
#define SB_LOCATION_TYPE                          TYPE_SMN

#define NBMISC_SB_LOCATION_SBlocated_Port_OFFSET  0
#define NBMISC_SB_LOCATION_SBlocated_Port_WIDTH   16
#define NBMISC_SB_LOCATION_SBlocated_Port_MASK    0xffff
#define NBMISC_SB_LOCATION_SBlocated_Core_OFFSET  16
#define NBMISC_SB_LOCATION_SBlocated_Core_WIDTH   16
#define NBMISC_SB_LOCATION_SBlocated_Core_MASK    0xffff0000L

/// NBMISC_007C
typedef union {
  struct {
    UINT32    SBlocated_Port : 16;                                     ///<
    UINT32    SBlocated_Core : 16;                                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_SB_LOCATION_STRUCT;

// svd_group: NBIO_IOHC_aliasSMN

// Address
#define CFG_IOHC_PCI_ADDRESS_HYEX                  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10034)
#define CFG_IOHC_PCI_ADDRESS_HYGX                  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10034)

// Type
#define CFG_IOHC_PCI_TYPE                              TYPE_SMN

#define CFG_IOHC_PCI_CFG_IOHC_PCI_Dev0Fn2RegEn_OFFSET  0
#define CFG_IOHC_PCI_CFG_IOHC_PCI_Dev0Fn2RegEn_WIDTH   1
#define CFG_IOHC_PCI_CFG_IOHC_PCI_Dev0Fn2RegEn_MASK    0x1
#define CFG_IOHC_PCI_IOMMU_DIS_OFFSET                  31
#define CFG_IOHC_PCI_IOMMU_DIS_WIDTH                   1
#define CFG_IOHC_PCI_IOMMU_DIS_MASK                    0x80000000L

/// SMN_00000034
typedef union {
  struct {
    UINT32    CFG_IOHC_PCI_Dev0Fn2RegEn : 1;                           ///<
    UINT32    Reserved_30_1             : 30;                          ///<
    UINT32    IOMMU_DIS                 : 1;                           ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} CFG_IOHC_PCI_STRUCT;

// Address
#define NB_MMIOBASE_ADDRESS_HYEX                   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1005C)
#define NB_MMIOBASE_ADDRESS_HYGX                   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1005C)

// Type
#define NB_MMIOBASE_TYPE                          TYPE_SMN

#define NB_MMIOBASE_OFFSET                        0
#define NB_MMIOBASE_WIDTH                         32
#define NB_MMIOBASE_MASK                          0xFFFFFFFF

/// NBMISC_0064
typedef union {
  struct {
    UINT32    MMIOBASE : 32;                                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NB_MMIOBASE_STRUCT;

// Address
#define NB_MMIOLIMIT_ADDRESS_HYEX                 (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10060)
#define NB_MMIOLIMIT_ADDRESS_HYGX                 (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10060)

// Type
#define NB_MMIOLIMIT_TYPE                         TYPE_SMN

#define NB_MMIOLIMIT_OFFSET                       0
#define NB_MMIOLIMIT_WIDTH                        32
#define NB_MMIOLIMIT_MASK                         0xFFFFFFFF

/// NBMISC_0064
typedef union {
  struct {
    UINT32    MMIOLIMIT : 32;                                           ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NB_MMIOLIMIT_STRUCT;

// Address
#define NB_LOWER_TOP_OF_DRAM2_ADDRESS_HYEX        (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10064)
#define NB_LOWER_TOP_OF_DRAM2_ADDRESS_HYGX        (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10064)

// Type
#define NB_LOWER_TOP_OF_DRAM2_TYPE               TYPE_SMN

#define NBMISC_0064_ENABLE_OFFSET                0
#define NBMISC_0064_ENABLE_WIDTH                 1
#define NBMISC_0064_ENABLE_MASK                  0x1
// Reserved_22_1:22
#define NBMISC_0064_LOWER_TOM2_OFFSET            23
#define NBMISC_0064_LOWER_TOM2_WIDTH             9
#define NBMISC_0064_LOWER_TOM2_MASK              0xff800000L

/// NBMISC_0064
typedef union {
  struct {
    UINT32    ENABLE        : 1;                                       ///<
    UINT32    Reserved_22_1 : 22;                                      ///<
    UINT32    LOWER_TOM2    : 9;                                       ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_0064_STRUCT;

// Address
#define NB_UPPER_TOP_OF_DRAM2_ADDRESS_HYEX       (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10068)
#define NB_UPPER_TOP_OF_DRAM2_ADDRESS_HYGX       (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10068)

// Type
#define NB_UPPER_TOP_OF_DRAM2_TYPE              TYPE_SMN

#define NBMISC_0068_UPPER_TOM2_OFFSET           0
#define NBMISC_0068_UPPER_TOM2_WIDTH            9
#define NBMISC_0068_UPPER_TOM2_MASK             0x1ff

/// NBMISC_0068
typedef union {
  struct {
    UINT32    UPPER_TOM2    : 9;                                       ///<
    UINT32    Reserved_32_8 : 23;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_0068_STRUCT;

// Address
#define NB_LOWER_DRAM2_BASE_ADDRESS_HYEX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1006c)
#define NB_LOWER_DRAM2_BASE_ADDRESS_HYGX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1006c)

// Type
#define NB_LOWER_DRAM2_BASE_TYPE                TYPE_SMN

#define NBMISC_006C_LOWER_DRAM2_BASE_OFFSET     23
#define NBMISC_006C_LOWER_DRAM2_BASE_WIDTH      9
#define NBMISC_006C_LOWER_DRAM2_BASE_MASK       0xff800000L

/// NBMISC_006C
typedef union {
  struct {
    UINT32    Reserved_22_0    : 23;                                   ///<
    UINT32    LOWER_DRAM2_BASE : 9;                                    ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_006C_STRUCT;

// Address
#define NB_UPPER_DRAM2_BASE_ADDRESS_HYEX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10070)
#define NB_UPPER_DRAM2_BASE_ADDRESS_HYGX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10070)

// Type
#define NB_UPPER_DRAM2_BASE_TYPE                TYPE_SMN

#define NBMISC_0070_UPPER_DRAM2_BASE_OFFSET     0
#define NBMISC_0070_UPPER_DRAM2_BASE_WIDTH      9
#define NBMISC_0070_UPPER_DRAM2_BASE_MASK       0x1ff

/// NBMISC_0070
typedef union {
  struct {
    UINT32    UPPER_DRAM2_BASE : 9;                                    ///<
    UINT32    Reserved_31_9    : 23;                                   ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_0070_STRUCT;

// Address
#define IOHC_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYEX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10088)
#define IOHC_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYGX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10088)

// Type
#define IOHC_GLUE_CG_LCLK_CTRL_0_TYPE          TYPE_SMN

#define NBMISC_0088_CG_OFF_HYSTERESIS_OFFSET   4
#define NBMISC_0088_CG_OFF_HYSTERESIS_WIDTH    8
#define NBMISC_0088_CG_OFF_HYSTERESIS_MASK     0xff0
#define NBMISC_0088_SOFT_OVERRIDE_CLK9_OFFSET  22
#define NBMISC_0088_SOFT_OVERRIDE_CLK9_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK9_MASK    0x400000
#define NBMISC_0088_SOFT_OVERRIDE_CLK8_OFFSET  23
#define NBMISC_0088_SOFT_OVERRIDE_CLK8_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK8_MASK    0x800000
#define NBMISC_0088_SOFT_OVERRIDE_CLK7_OFFSET  24
#define NBMISC_0088_SOFT_OVERRIDE_CLK7_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK7_MASK    0x1000000
#define NBMISC_0088_SOFT_OVERRIDE_CLK6_OFFSET  25
#define NBMISC_0088_SOFT_OVERRIDE_CLK6_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK6_MASK    0x2000000
#define NBMISC_0088_SOFT_OVERRIDE_CLK5_OFFSET  26
#define NBMISC_0088_SOFT_OVERRIDE_CLK5_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK5_MASK    0x4000000
#define NBMISC_0088_SOFT_OVERRIDE_CLK4_OFFSET  27
#define NBMISC_0088_SOFT_OVERRIDE_CLK4_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK4_MASK    0x8000000
#define NBMISC_0088_SOFT_OVERRIDE_CLK3_OFFSET  28
#define NBMISC_0088_SOFT_OVERRIDE_CLK3_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK3_MASK    0x10000000
#define NBMISC_0088_SOFT_OVERRIDE_CLK2_OFFSET  29
#define NBMISC_0088_SOFT_OVERRIDE_CLK2_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK2_MASK    0x20000000
#define NBMISC_0088_SOFT_OVERRIDE_CLK1_OFFSET  30
#define NBMISC_0088_SOFT_OVERRIDE_CLK1_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK1_MASK    0x40000000
#define NBMISC_0088_SOFT_OVERRIDE_CLK0_OFFSET  31
#define NBMISC_0088_SOFT_OVERRIDE_CLK0_WIDTH   1
#define NBMISC_0088_SOFT_OVERRIDE_CLK0_MASK    0x80000000L

/// NBMISC_0088
typedef union {
  struct {
    UINT32    Reserved_3_0       : 4;                                  ///<
    UINT32    CG_OFF_HYSTERESIS  : 8;                                  ///<
    UINT32    Reserved_21_12     : 10;                                 ///<
    UINT32    SOFT_OVERRIDE_CLK9 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK8 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK7 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK6 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK5 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK4 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK3 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK2 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK1 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK0 : 1;                                  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_0088_STRUCT;

// Address
#define IOHC_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYEX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1008c)
#define IOHC_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYGX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1008c)

// Type
#define IOHC_GLUE_CG_LCLK_CTRL_1_TYPE          TYPE_SMN

#define NBMISC_008C_SOFT_OVERRIDE_CLK9_OFFSET  22
#define NBMISC_008C_SOFT_OVERRIDE_CLK9_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK9_MASK    0x400000
#define NBMISC_008C_SOFT_OVERRIDE_CLK8_OFFSET  23
#define NBMISC_008C_SOFT_OVERRIDE_CLK8_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK8_MASK    0x800000
#define NBMISC_008C_SOFT_OVERRIDE_CLK7_OFFSET  24
#define NBMISC_008C_SOFT_OVERRIDE_CLK7_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK7_MASK    0x1000000
#define NBMISC_008C_SOFT_OVERRIDE_CLK6_OFFSET  25
#define NBMISC_008C_SOFT_OVERRIDE_CLK6_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK6_MASK    0x2000000
#define NBMISC_008C_SOFT_OVERRIDE_CLK5_OFFSET  26
#define NBMISC_008C_SOFT_OVERRIDE_CLK5_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK5_MASK    0x4000000
#define NBMISC_008C_SOFT_OVERRIDE_CLK4_OFFSET  27
#define NBMISC_008C_SOFT_OVERRIDE_CLK4_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK4_MASK    0x8000000
#define NBMISC_008C_SOFT_OVERRIDE_CLK3_OFFSET  28
#define NBMISC_008C_SOFT_OVERRIDE_CLK3_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK3_MASK    0x10000000
#define NBMISC_008C_SOFT_OVERRIDE_CLK2_OFFSET  29
#define NBMISC_008C_SOFT_OVERRIDE_CLK2_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK2_MASK    0x20000000
#define NBMISC_008C_SOFT_OVERRIDE_CLK1_OFFSET  30
#define NBMISC_008C_SOFT_OVERRIDE_CLK1_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK1_MASK    0x40000000
#define NBMISC_008C_SOFT_OVERRIDE_CLK0_OFFSET  31
#define NBMISC_008C_SOFT_OVERRIDE_CLK0_WIDTH   1
#define NBMISC_008C_SOFT_OVERRIDE_CLK0_MASK    0x80000000L

/// NBMISC_008C
typedef union {
  struct {
    UINT32    Reserved_21_0      : 22;                                 ///<
    UINT32    SOFT_OVERRIDE_CLK9 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK8 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK7 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK6 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK5 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK4 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK3 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK2 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK1 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK0 : 1;                                  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_008C_STRUCT;

// Address
#define IOHC_GLUE_CG_LCLK_CTRL_2_ADDRESS_HYEX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10090)
#define IOHC_GLUE_CG_LCLK_CTRL_2_ADDRESS_HYGX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10090)

// Type
#define IOHC_GLUE_CG_LCLK_CTRL_2_TYPE          TYPE_SMN

#define NBMISC_0090_SOFT_OVERRIDE_CLK9_OFFSET  22
#define NBMISC_0090_SOFT_OVERRIDE_CLK9_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK9_MASK    0x400000
#define NBMISC_0090_SOFT_OVERRIDE_CLK8_OFFSET  23
#define NBMISC_0090_SOFT_OVERRIDE_CLK8_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK8_MASK    0x800000
#define NBMISC_0090_SOFT_OVERRIDE_CLK7_OFFSET  24
#define NBMISC_0090_SOFT_OVERRIDE_CLK7_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK7_MASK    0x1000000
#define NBMISC_0090_SOFT_OVERRIDE_CLK6_OFFSET  25
#define NBMISC_0090_SOFT_OVERRIDE_CLK6_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK6_MASK    0x2000000
#define NBMISC_0090_SOFT_OVERRIDE_CLK5_OFFSET  26
#define NBMISC_0090_SOFT_OVERRIDE_CLK5_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK5_MASK    0x4000000
#define NBMISC_0090_SOFT_OVERRIDE_CLK4_OFFSET  27
#define NBMISC_0090_SOFT_OVERRIDE_CLK4_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK4_MASK    0x8000000
#define NBMISC_0090_SOFT_OVERRIDE_CLK3_OFFSET  28
#define NBMISC_0090_SOFT_OVERRIDE_CLK3_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK3_MASK    0x10000000
#define NBMISC_0090_SOFT_OVERRIDE_CLK2_OFFSET  29
#define NBMISC_0090_SOFT_OVERRIDE_CLK2_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK2_MASK    0x20000000
#define NBMISC_0090_SOFT_OVERRIDE_CLK1_OFFSET  30
#define NBMISC_0090_SOFT_OVERRIDE_CLK1_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK1_MASK    0x40000000
#define NBMISC_0090_SOFT_OVERRIDE_CLK0_OFFSET  31
#define NBMISC_0090_SOFT_OVERRIDE_CLK0_WIDTH   1
#define NBMISC_0090_SOFT_OVERRIDE_CLK0_MASK    0x80000000L

/// NBMISC_0090
typedef union {
  struct {
    UINT32    Reserved_21_0      : 22;                                 ///<
    UINT32    SOFT_OVERRIDE_CLK9 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK8 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK7 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK6 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK5 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK4 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK3 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK2 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK1 : 1;                                  ///<
    UINT32    SOFT_OVERRIDE_CLK0 : 1;                                  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_0090_STRUCT;

// Address
#define IOAGR_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYEX                 0x15B00000
#define IOAGR_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYGX                 0x0A000000

// Type
#define IOAGR_GLUE_CG_LCLK_CTRL_0_TYPE                         TYPE_NBMISC

#define NBMISC_0000_CG_OFF_HYSTERESIS_OFFSET                   4
#define NBMISC_0000_CG_OFF_HYSTERESIS_WIDTH                    8
#define NBMISC_0000_CG_OFF_HYSTERESIS_MASK                     0xff0
#define NBMISC_0000_SOFT_OVERRIDE_CLK9_OFFSET                  22
#define NBMISC_0000_SOFT_OVERRIDE_CLK9_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK9_MASK                    0x400000
#define NBMISC_0000_SOFT_OVERRIDE_CLK8_OFFSET                  23
#define NBMISC_0000_SOFT_OVERRIDE_CLK8_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK8_MASK                    0x800000
#define NBMISC_0000_SOFT_OVERRIDE_CLK7_OFFSET                  24
#define NBMISC_0000_SOFT_OVERRIDE_CLK7_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK7_MASK                    0x1000000
#define NBMISC_0000_SOFT_OVERRIDE_CLK6_OFFSET                  25
#define NBMISC_0000_SOFT_OVERRIDE_CLK6_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK6_MASK                    0x2000000
#define NBMISC_0000_SOFT_OVERRIDE_CLK5_OFFSET                  26
#define NBMISC_0000_SOFT_OVERRIDE_CLK5_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK5_MASK                    0x4000000
#define NBMISC_0000_SOFT_OVERRIDE_CLK4_OFFSET                  27
#define NBMISC_0000_SOFT_OVERRIDE_CLK4_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK4_MASK                    0x8000000
#define NBMISC_0000_SOFT_OVERRIDE_CLK3_OFFSET                  28
#define NBMISC_0000_SOFT_OVERRIDE_CLK3_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK3_MASK                    0x10000000
#define NBMISC_0000_SOFT_OVERRIDE_CLK2_OFFSET                  29
#define NBMISC_0000_SOFT_OVERRIDE_CLK2_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK2_MASK                    0x20000000
#define NBMISC_0000_SOFT_OVERRIDE_CLK1_OFFSET                  30
#define NBMISC_0000_SOFT_OVERRIDE_CLK1_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK1_MASK                    0x40000000
#define NBMISC_0000_SOFT_OVERRIDE_CLK0_OFFSET                  31
#define NBMISC_0000_SOFT_OVERRIDE_CLK0_WIDTH                   1
#define NBMISC_0000_SOFT_OVERRIDE_CLK0_MASK                    0x80000000L

// Address
#define IOAGR_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYEX                 0x15B00004
#define IOAGR_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYGX                 0x0A000004

// Type
#define IOAGR_GLUE_CG_LCLK_CTRL_1_TYPE                         TYPE_NBMISC

#define NBMISC_0004_SOFT_OVERRIDE_CLK9_OFFSET                  22
#define NBMISC_0004_SOFT_OVERRIDE_CLK9_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK9_MASK                    0x400000
#define NBMISC_0004_SOFT_OVERRIDE_CLK8_OFFSET                  23
#define NBMISC_0004_SOFT_OVERRIDE_CLK8_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK8_MASK                    0x800000
#define NBMISC_0004_SOFT_OVERRIDE_CLK7_OFFSET                  24
#define NBMISC_0004_SOFT_OVERRIDE_CLK7_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK7_MASK                    0x1000000
#define NBMISC_0004_SOFT_OVERRIDE_CLK6_OFFSET                  25
#define NBMISC_0004_SOFT_OVERRIDE_CLK6_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK6_MASK                    0x2000000
#define NBMISC_0004_SOFT_OVERRIDE_CLK5_OFFSET                  26
#define NBMISC_0004_SOFT_OVERRIDE_CLK5_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK5_MASK                    0x4000000
#define NBMISC_0004_SOFT_OVERRIDE_CLK4_OFFSET                  27
#define NBMISC_0004_SOFT_OVERRIDE_CLK4_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK4_MASK                    0x8000000
#define NBMISC_0004_SOFT_OVERRIDE_CLK3_OFFSET                  28
#define NBMISC_0004_SOFT_OVERRIDE_CLK3_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK3_MASK                    0x10000000
#define NBMISC_0004_SOFT_OVERRIDE_CLK2_OFFSET                  29
#define NBMISC_0004_SOFT_OVERRIDE_CLK2_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK2_MASK                    0x20000000
#define NBMISC_0004_SOFT_OVERRIDE_CLK1_OFFSET                  30
#define NBMISC_0004_SOFT_OVERRIDE_CLK1_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK1_MASK                    0x40000000
#define NBMISC_0004_SOFT_OVERRIDE_CLK0_OFFSET                  31
#define NBMISC_0004_SOFT_OVERRIDE_CLK0_WIDTH                   1
#define NBMISC_0004_SOFT_OVERRIDE_CLK0_MASK                    0x80000000L

// Address
#define IOAPIC_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYEX                0x14300100
#define IOAPIC_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYGX                0x08800100

// Type
#define IOAPIC_GLUE_CG_LCLK_CTRL_0_TYPE                        TYPE_NBMISC

#define NBMISC_0100_CG_OFF_HYSTERESIS_OFFSET                   4
#define NBMISC_0100_CG_OFF_HYSTERESIS_WIDTH                    8
#define NBMISC_0100_CG_OFF_HYSTERESIS_MASK                     0xff0
#define NBMISC_0100_SOFT_OVERRIDE_CLK9_OFFSET                  22
#define NBMISC_0100_SOFT_OVERRIDE_CLK9_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK9_MASK                    0x400000
#define NBMISC_0100_SOFT_OVERRIDE_CLK8_OFFSET                  23
#define NBMISC_0100_SOFT_OVERRIDE_CLK8_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK8_MASK                    0x800000
#define NBMISC_0100_SOFT_OVERRIDE_CLK7_OFFSET                  24
#define NBMISC_0100_SOFT_OVERRIDE_CLK7_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK7_MASK                    0x1000000
#define NBMISC_0100_SOFT_OVERRIDE_CLK6_OFFSET                  25
#define NBMISC_0100_SOFT_OVERRIDE_CLK6_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK6_MASK                    0x2000000
#define NBMISC_0100_SOFT_OVERRIDE_CLK5_OFFSET                  26
#define NBMISC_0100_SOFT_OVERRIDE_CLK5_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK5_MASK                    0x4000000
#define NBMISC_0100_SOFT_OVERRIDE_CLK4_OFFSET                  27
#define NBMISC_0100_SOFT_OVERRIDE_CLK4_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK4_MASK                    0x8000000
#define NBMISC_0100_SOFT_OVERRIDE_CLK3_OFFSET                  28
#define NBMISC_0100_SOFT_OVERRIDE_CLK3_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK3_MASK                    0x10000000
#define NBMISC_0100_SOFT_OVERRIDE_CLK2_OFFSET                  29
#define NBMISC_0100_SOFT_OVERRIDE_CLK2_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK2_MASK                    0x20000000
#define NBMISC_0100_SOFT_OVERRIDE_CLK1_OFFSET                  30
#define NBMISC_0100_SOFT_OVERRIDE_CLK1_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK1_MASK                    0x40000000
#define NBMISC_0100_SOFT_OVERRIDE_CLK0_OFFSET                  31
#define NBMISC_0100_SOFT_OVERRIDE_CLK0_WIDTH                   1
#define NBMISC_0100_SOFT_OVERRIDE_CLK0_MASK                    0x80000000L

#define IOC_FEATURE_CNTL_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10118)
#define IOC_FEATURE_CNTL_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10118)

#define IOC_SDP_PORT_CNTL_ADDRESS_HYGX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10344)

// Type
#define IOC_FEATURE_CNTL_TYPE                   TYPE_NBMISC

#define NBMISC_0118_HpPmpme_DevID_En_OFFSET   0
#define NBMISC_0118_HpPmpme_DevID_En_WIDTH    1
#define NBMISC_0118_HpPmpme_DevID_En_MASK     0x1
#define NBMISC_0118_P2P_mode_OFFSET           1
#define NBMISC_0118_P2P_mode_WIDTH            2
#define NBMISC_0118_P2P_mode_MASK             0x6
#define NBMISC_0118_IOHC_ARCH_MODE_OFFSET     3
#define NBMISC_0118_IOHC_ARCH_MODE_WIDTH      1
#define NBMISC_0118_IOHC_ARCH_MODE_MASK       0x8
#define NBMISC_0118_IOC_ARI_SUPPORTED_OFFSET  22
#define NBMISC_0118_IOC_ARI_SUPPORTED_WIDTH   1
#define NBMISC_0118_IOC_ARI_SUPPORTED_MASK    0x400000
#define NBMISC_0118_IOHC_dGPU_MODE_OFFSET     28
#define NBMISC_0118_IOHC_dGPU_MODE_WIDTH      1
#define NBMISC_0118_IOHC_dGPU_MODE_MASK       0x10000000L
#define NBMISC_0118_MISC_FEATURE_CNTL_OFFSET  29
#define NBMISC_0118_MISC_FEATURE_CNTL_WIDTH   3
#define NBMISC_0118_MISC_FEATURE_CNTL_MASK    0xe0000000L

/// NBMISC_0118
typedef union {
  struct {
    UINT32    HpPmpme_DevID_En  : 1;                                   ///<
    UINT32    P2P_mode          : 2;                                   ///<
    UINT32    IOHC_ARCH_MODE    : 1;                                   ///<
    UINT32    Reserved_21_4     : 18;                                  ///<
    UINT32    IOC_ARI_SUPPORTED : 1;                                   ///<
    UINT32    Reserved_27_23    : 5;                                   ///<
    UINT32    IOHC_dGPU_MODE    : 1;                                   ///<
    UINT32    MISC_FEATURE_CNTL : 3;                                   ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_0118_STRUCT;

// Address
#define NB_TOP_OF_DRAM3_ADDRESS_HYEX     (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10138)
#define NB_TOP_OF_DRAM3_ADDRESS_HYGX     (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10138)

// Type
#define NB_TOP_OF_DRAM3_TYPE            TYPE_SMN

#define NBMISC_0138_TOM3_LIMIT_OFFSET   0
#define NBMISC_0138_TOM3_LIMIT_WIDTH    30
#define NBMISC_0138_TOM3_LIMIT_MASK     0x3fffffff
// Reserved_30_30
#define NBMISC_0138_TOM3_ENABLE_OFFSET  31
#define NBMISC_0138_TOM3_ENABLE_WIDTH   1
#define NBMISC_0138_TOM3_ENABLE_MASK    0x80000000L

/// NBMISC_0138
typedef union {
  struct {
    UINT32    TOM3_LIMIT     : 30;                                     ///< physical memory BIT[51:22]
    UINT32    Reserved_30_30 : 1;                                      ///<
    UINT32    TOM3_ENABLE    : 1;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_0138_STRUCT;

// Address
#define NB_TOP_OF_DRAM3_EXT_ADDRESS_HYEX    (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1013C)
#define NB_TOP_OF_DRAM3_EXT_ADDRESS_HYGX    (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1013C)

// Type
#define NB_TOP_OF_DRAM3_EXT_TYPE           TYPE_SMN
#define NBMISC_013C_TOM3_EXT_BASE_OFFSET   0
#define NBMISC_013C_TOM3_EXT_BASE_WIDTH    3
#define NBMISC_013C_TOM3_EXT_BASE_MASK     0x00000007
#define NBMISC_013C_TOM3_EXT_LIMIT_OFFSET  16
#define NBMISC_013C_TOM3_EXT_LIMIT_WIDTH   3
#define NBMISC_013C_TOM3_EXT_LIMIT_MASK    0x00070000

/// NBMISC_013C
typedef union {
  struct {
    UINT32    TOM3_EXT_BASE  : 3;                                      ///< physical memory BIT[54:52]
    UINT32    Reserved_3_15  : 13;                                     ///<
    UINT32    TOM3_EXT_LIMIT : 3;                                      ///< physical memory BIT[54:52]
    UINT32    Reserved_19_31 : 13;                                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_013C_STRUCT;

// Address
#define PCIE_VDM_CNTL2_ADDRESS_HYEX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10230)
#define PCIE_VDM_CNTL2_ADDRESS_HYGX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10230)

// Type
#define PCIE_VDM_CNTL2_TYPE                TYPE_SMN

#define NBMISC_0230_VdmP2pMode_OFFSET      0
#define NBMISC_0230_VdmP2pMode_WIDTH       2
#define NBMISC_0230_VdmP2pMode_MASK        0x3
// Reserved 3:2
#define NBMISC_0230_MCTPT2SMUEn_OFFSET     4
#define NBMISC_0230_MCTPT2SMUEn_WIDTH      1
#define NBMISC_0230_MCTPT2SMUEn_MASK       0x10
#define NBMISC_0230_HYGONVDM2SMUEn_OFFSET  5
#define NBMISC_0230_HYGONVDM2SMUEn_WIDTH   1
#define NBMISC_0230_HYGONVDM2SMUEn_MASK    0x20
#define NBMISC_0230_OtherVDM2SMUEn_OFFSET  6
#define NBMISC_0230_OtherVDM2SMUEn_WIDTH   1
#define NBMISC_0230_OtherVDM2SMUEn_MASK    0x40
// Reserved 14:7
#define NBMISC_0230_MCTPMasterValid_OFFSET  15
#define NBMISC_0230_MCTPMasterValid_WIDTH   1
#define NBMISC_0230_MCTPMasterValid_MASK    0x8000
#define NBMISC_0230_MCTPMasterID_OFFSET     16
#define NBMISC_0230_MCTPMasterID_WIDTH      16
#define NBMISC_0230_MCTPMasterID_MASK       0xffff0000L

/// NBMISC_0230
typedef union {
  struct {
    UINT32    VdmP2pMode      : 2;                                     ///<
    UINT32    Reserved_3_2    : 2;                                     ///<
    UINT32    MCTPT2SMUEn     : 1;                                     ///<
    UINT32    HYGONVDM2SMUEn  : 1;                                     ///<
    UINT32    OtherVDM2SMUEn  : 1;                                     ///<
    UINT32    Reserved_14_7   : 8;                                     ///<
    UINT32    MCTPMasterValid : 1;                                     ///<
    UINT32    MCTPMasterID    : 16;                                    ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBMISC_0230_STRUCT;

// Address
#define IOHC_SION_S0_Client0_Req_BurstTarget_Lower_ADDRESS_HYEX        (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14400)
#define IOHC_SION_S0_Client0_Req_BurstTarget_Lower_ADDRESS_HYGX        (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14400)

// Type
#define IOHC_SION_S0_Client0_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4400_IOHC_SION_S0_Client0_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4400_IOHC_SION_S0_Client0_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4400_IOHC_SION_S0_Client0_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4400
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client0_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4400_STRUCT;

// Address
#define IOHC_SION_S0_Client0_Req_BurstTarget_Upper_ADDRESS_HYEX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14404)
#define IOHC_SION_S0_Client0_Req_BurstTarget_Upper_ADDRESS_HYGX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14404)

// Type
#define IOHC_SION_S0_Client0_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4404_IOHC_SION_S0_Client0_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4404_IOHC_SION_S0_Client0_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4404_IOHC_SION_S0_Client0_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4404
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client0_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4404_STRUCT;

// Address
#define IOHC_SION_S0_Client0_Req_TimeSlot_Lower_ADDRESS_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14408)
#define IOHC_SION_S0_Client0_Req_TimeSlot_Lower_ADDRESS_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14408)

// Type
#define IOHC_SION_S0_Client0_Req_TimeSlot_Lower_TYPE                    TYPE_SMN

#define NBIOIOHC_4408_IOHC_SION_S0_Client0_Req_TimeSlot_Lower_OFFSET    0
#define NBIOIOHC_4408_IOHC_SION_S0_Client0_Req_TimeSlot_Lower_WIDTH     32
#define NBIOIOHC_4408_IOHC_SION_S0_Client0_Req_TimeSlot_Lower_MASK      0xFFFFFFFFF

/// NBIOIOHC_4408
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client0_Req_TimeSlot_Lower : 32;            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4408_STRUCT;

// Address
#define IOHC_SION_S0_Client0_Req_TimeSlot_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1440C)
#define IOHC_SION_S0_Client0_Req_TimeSlot_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1440C)

// Type
#define IOHC_SION_S0_Client0_Req_TimeSlot_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_440C_IOHC_SION_S0_Client0_Req_TimeSlot_Upper_OFFSET  0
#define NBIOIOHC_440C_IOHC_SION_S0_Client0_Req_TimeSlot_Upper_WIDTH   32
#define NBIOIOHC_440C_IOHC_SION_S0_Client0_Req_TimeSlot_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_440C
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client0_Req_TimeSlot_Upper : 32;            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_440C_STRUCT;

// Address
#define IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_ADDRESS_HYEX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14410)
#define IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_ADDRESS_HYGX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14410)

// Type
#define IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4410_IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4410_IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4410_IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4410
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower : 32;       ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4410_STRUCT;

// Address
#define IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_ADDRESS_HYEX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14414)
#define IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_ADDRESS_HYGX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14414)

// Type
#define IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4414_IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4414_IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4414_IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4414
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4414_STRUCT;

// Address
#define IOHC_SION_S0_Client1_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14804)
#define IOHC_SION_S0_Client1_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14804)

// Type
#define IOHC_SION_S0_Client1_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4804_IOHC_SION_S0_Client1_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4804_IOHC_SION_S0_Client1_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4804_IOHC_SION_S0_Client1_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4804
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client1_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4804_STRUCT;

// Address
#define IOHC_SION_S0_Client1_Req_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14808)
#define IOHC_SION_S0_Client1_Req_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14808)
// Type
#define IOHC_SION_S0_Client1_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4808_IOHC_SION_S0_Client1_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4808_IOHC_SION_S0_Client1_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4808_IOHC_SION_S0_Client1_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4808
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client1_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4808_STRUCT;

// Address
#define IOHC_SION_S0_Client1_Req_TimeSlot_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1480C)
#define IOHC_SION_S0_Client1_Req_TimeSlot_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1480C)

// Type
#define IOHC_SION_S0_Client1_Req_TimeSlot_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_480C_IOHC_SION_S0_Client1_Req_TimeSlot_Lower_OFFSET  0
#define NBIOIOHC_480C_IOHC_SION_S0_Client1_Req_TimeSlot_Lower_WIDTH   32
#define NBIOIOHC_480C_IOHC_SION_S0_Client1_Req_TimeSlot_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_480C
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client1_Req_TimeSlot_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_480C_STRUCT;

// Address
#define IOHC_SION_S0_Client1_Req_TimeSlot_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14810)
#define IOHC_SION_S0_Client1_Req_TimeSlot_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14810)
// Type
#define IOHC_SION_S0_Client1_Req_TimeSlot_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4810_IOHC_SION_S0_Client1_Req_TimeSlot_Upper_OFFSET  0
#define NBIOIOHC_4810_IOHC_SION_S0_Client1_Req_TimeSlot_Upper_WIDTH   32
#define NBIOIOHC_4810_IOHC_SION_S0_Client1_Req_TimeSlot_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4810
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client1_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4810_STRUCT;

// Address
#define IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14814)
#define IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14814)
// Type
#define IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4814_IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4814_IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4814_IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4814
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4814_STRUCT;

// Address
#define IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14818)
#define IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14818)
// Type
#define IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4818_IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4818_IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4818_IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4818
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper : 32;       ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4818_STRUCT;

// Address
#define IOHC_SION_S0_Client2_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C08)
#define IOHC_SION_S0_Client2_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C08)
// Type
#define IOHC_SION_S0_Client2_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4C08_IOHC_SION_S0_Client2_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4C08_IOHC_SION_S0_Client2_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4C08_IOHC_SION_S0_Client2_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C08
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client2_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C08_STRUCT;

// Address
#define IOHC_SION_S0_Client2_Req_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C0C)
#define IOHC_SION_S0_Client2_Req_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C0C)
// Type
#define IOHC_SION_S0_Client2_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4C0C_IOHC_SION_S0_Client2_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4C0C_IOHC_SION_S0_Client2_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4C0C_IOHC_SION_S0_Client2_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C0C
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client2_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C0C_STRUCT;

// Address
#define IOHC_SION_S0_Client2_Req_TimeSlot_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C10)
#define IOHC_SION_S0_Client2_Req_TimeSlot_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C10)
// Type
#define IOHC_SION_S0_Client2_Req_TimeSlot_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4C10_IOHC_SION_S0_Client2_Req_TimeSlot_Lower_OFFSET  0
#define NBIOIOHC_4C10_IOHC_SION_S0_Client2_Req_TimeSlot_Lower_WIDTH   32
#define NBIOIOHC_4C10_IOHC_SION_S0_Client2_Req_TimeSlot_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C10
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client2_Req_TimeSlot_Lower : 32;            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C10_STRUCT;

// Address
#define IOHC_SION_S0_Client2_Req_TimeSlot_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C14)
#define IOHC_SION_S0_Client2_Req_TimeSlot_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C14)
// Type
#define IOHC_SION_S0_Client2_Req_TimeSlot_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4C14_IOHC_SION_S0_Client2_Req_TimeSlot_Upper_OFFSET  0
#define NBIOIOHC_4C14_IOHC_SION_S0_Client2_Req_TimeSlot_Upper_WIDTH   32
#define NBIOIOHC_4C14_IOHC_SION_S0_Client2_Req_TimeSlot_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C14
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client2_Req_TimeSlot_Upper : 32;            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C14_STRUCT;

// Address
#define IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_ADDRESS_HYEX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C18)
#define IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_ADDRESS_HYGX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C18)
// Type
#define IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4C18_IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4C18_IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4C18_IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C18
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C18_STRUCT;

// Address
#define IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C1C)
#define IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C1C)
// Type
#define IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4C1C_IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4C1C_IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4C1C_IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C1C
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C1C_STRUCT;

// Address
#define IOHC_SION_S0_Client3_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1500C)
#define IOHC_SION_S0_Client3_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1500C)
// Type
#define IOHC_SION_S0_Client3_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_500C_IOHC_SION_S0_Client3_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_500C_IOHC_SION_S0_Client3_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_500C_IOHC_SION_S0_Client3_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_500C
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client3_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_500C_STRUCT;

// Address
#define IOHC_SION_S0_Client3_Req_BurstTarget_Upper_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15010)
#define IOHC_SION_S0_Client3_Req_BurstTarget_Upper_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15010)
// Type
#define IOHC_SION_S0_Client3_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5010_IOHC_SION_S0_Client3_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5010_IOHC_SION_S0_Client3_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5010_IOHC_SION_S0_Client3_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5010
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client3_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5010_STRUCT;

// Address
#define IOHC_SION_S0_Client3_Req_TimeSlot_Lower_ADDRESS_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15014)
#define IOHC_SION_S0_Client3_Req_TimeSlot_Lower_ADDRESS_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15014)
// Type
#define IOHC_SION_S0_Client3_Req_TimeSlot_Lower_TYPE                   TYPE_SMN

#define NBIOIOHC_5014_IOHC_SION_S0_Client3_Req_TimeSlot_Lower_OFFSET   0
#define NBIOIOHC_5014_IOHC_SION_S0_Client3_Req_TimeSlot_Lower_WIDTH    32
#define NBIOIOHC_5014_IOHC_SION_S0_Client3_Req_TimeSlot_Lower_MASK     0xFFFFFFFFF

/// NBIOIOHC_5014
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client3_Req_TimeSlot_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5014_STRUCT;

// Address
#define IOHC_SION_S0_Client3_Req_TimeSlot_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15018)
#define IOHC_SION_S0_Client3_Req_TimeSlot_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15018)
// Type
#define IOHC_SION_S0_Client3_Req_TimeSlot_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5018_IOHC_SION_S0_Client3_Req_TimeSlot_Upper_OFFSET  0
#define NBIOIOHC_5018_IOHC_SION_S0_Client3_Req_TimeSlot_Upper_WIDTH   32
#define NBIOIOHC_5018_IOHC_SION_S0_Client3_Req_TimeSlot_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5018
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client3_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5018_STRUCT;

// Address
#define IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_ADDRESS_HYEX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1501C)
#define IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_ADDRESS_HYGX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1501C)
// Type
#define IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_TYPE                 TYPE_SMN

#define NBIOIOHC_501C_IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_501C_IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_501C_IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_501C
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_501C_STRUCT;

// Address
#define IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15020)
#define IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15020)
// Type
#define IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5020_IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5020_IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5020_IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5020
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5020_STRUCT;

// Address
#define IOHC_SION_S0_Client4_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15410)
#define IOHC_SION_S0_Client4_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15410)
// Type
#define IOHC_SION_S0_Client4_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5410_IOHC_SION_S0_Client4_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_5410_IOHC_SION_S0_Client4_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_5410_IOHC_SION_S0_Client4_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5410
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client4_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5410_STRUCT;

// Address
#define IOHC_SION_S0_Client4_Req_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15414)
#define IOHC_SION_S0_Client4_Req_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15414)
// Type
#define IOHC_SION_S0_Client4_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5414_IOHC_SION_S0_Client4_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5414_IOHC_SION_S0_Client4_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5414_IOHC_SION_S0_Client4_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5414
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client4_Req_BurstTarget_Upper : 32;          ///<
  } Field;                                                              ///<
  UINT32    Value;                                                      ///<
} NBIOIOHC_5414_STRUCT;

// Address
#define IOHC_SION_S0_Client4_Req_TimeSlot_Lower_ADDRESS_HYEX             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15418)
#define IOHC_SION_S0_Client4_Req_TimeSlot_Lower_ADDRESS_HYGX             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15418)
// Type
#define IOHC_SION_S0_Client4_Req_TimeSlot_Lower_TYPE                    TYPE_SMN

#define NBIOIOHC_5418_IOHC_SION_S0_Client4_Req_TimeSlot_Lower_OFFSET    0
#define NBIOIOHC_5418_IOHC_SION_S0_Client4_Req_TimeSlot_Lower_WIDTH     32
#define NBIOIOHC_5418_IOHC_SION_S0_Client4_Req_TimeSlot_Lower_MASK      0xFFFFFFFFF

/// NBIOIOHC_5418
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client4_Req_TimeSlot_Lower : 32;            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5418_STRUCT;

// Address
#define IOHC_SION_S0_Client4_Req_TimeSlot_Upper_ADDRESS_HYEX             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1541C)
#define IOHC_SION_S0_Client4_Req_TimeSlot_Upper_ADDRESS_HYGX             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1541C)
// Type
#define IOHC_SION_S0_Client4_Req_TimeSlot_Upper_TYPE                    TYPE_SMN

#define NBIOIOHC_541C_IOHC_SION_S0_Client4_Req_TimeSlot_Upper_OFFSET    0
#define NBIOIOHC_541C_IOHC_SION_S0_Client4_Req_TimeSlot_Upper_WIDTH     32
#define NBIOIOHC_541C_IOHC_SION_S0_Client4_Req_TimeSlot_Upper_MASK      0xFFFFFFFFF

/// NBIOIOHC_541C
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client4_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_541C_STRUCT;

// Address
#define IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15420)
#define IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15420)
// Type
#define IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5420_IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_5420_IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_5420_IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5420
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5420_STRUCT;

// Address
#define IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15424)
#define IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15424)
// Type
#define IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5424_IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5424_IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5424_IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5424
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5424_STRUCT;

// Address
#define IOHC_SION_S0_Client5_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15814)
#define IOHC_SION_S0_Client5_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15814)
// Type
#define IOHC_SION_S0_Client5_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5814_IOHC_SION_S0_Client5_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_5814_IOHC_SION_S0_Client5_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_5814_IOHC_SION_S0_Client5_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5814
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client5_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5814_STRUCT;

// Address
#define IOHC_SION_S0_Client5_Req_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15818)
#define IOHC_SION_S0_Client5_Req_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15818)
// Type
#define IOHC_SION_S0_Client5_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5818_IOHC_SION_S0_Client5_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5818_IOHC_SION_S0_Client5_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5818_IOHC_SION_S0_Client5_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5818
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client5_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5818_STRUCT;

// Address
#define IOHC_SION_S0_Client5_Req_TimeSlot_Lower_ADDRESS_HYEX             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1581C)
#define IOHC_SION_S0_Client5_Req_TimeSlot_Lower_ADDRESS_HYGX             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1581C)
// Type
#define IOHC_SION_S0_Client5_Req_TimeSlot_Lower_TYPE                    TYPE_SMN

#define NBIOIOHC_581C_IOHC_SION_S0_Client5_Req_TimeSlot_Lower_OFFSET    0
#define NBIOIOHC_581C_IOHC_SION_S0_Client5_Req_TimeSlot_Lower_WIDTH     32
#define NBIOIOHC_581C_IOHC_SION_S0_Client5_Req_TimeSlot_Lower_MASK      0xFFFFFFFFF

/// NBIOIOHC_581C
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client5_Req_TimeSlot_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_581C_STRUCT;

// Address
#define IOHC_SION_S0_Client5_Req_TimeSlot_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15820)
#define IOHC_SION_S0_Client5_Req_TimeSlot_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15820)
// Type
#define IOHC_SION_S0_Client5_Req_TimeSlot_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5820_IOHC_SION_S0_Client5_Req_TimeSlot_Upper_OFFSET  0
#define NBIOIOHC_5820_IOHC_SION_S0_Client5_Req_TimeSlot_Upper_WIDTH   32
#define NBIOIOHC_5820_IOHC_SION_S0_Client5_Req_TimeSlot_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5820
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client5_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5820_STRUCT;

// Address
#define IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15824)
#define IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15824)
// Type
#define IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5824_IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_5824_IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_5824_IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5824
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5824_STRUCT;

// Address
#define IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15828)
#define IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15828)
// Type
#define IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5828_IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5828_IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5828_IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5828
typedef union {
  struct {
    UINT32    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5828_STRUCT;

// Address
#define IOHC_SION_S1_Client0_Req_BurstTarget_Lower_ADDRESS_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14430)
#define IOHC_SION_S1_Client0_Req_BurstTarget_Lower_ADDRESS_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14430)
// Type
#define IOHC_SION_S1_Client0_Req_BurstTarget_Lower_TYPE                   TYPE_SMN

#define NBIOIOHC_4430_IOHC_SION_S1_Client0_Req_BurstTarget_Lower_OFFSET   0
#define NBIOIOHC_4430_IOHC_SION_S1_Client0_Req_BurstTarget_Lower_WIDTH    32
#define NBIOIOHC_4430_IOHC_SION_S1_Client0_Req_BurstTarget_Lower_MASK     0xFFFFFFFFF

/// NBIOIOHC_4430
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client0_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4430_STRUCT;

// Address
#define IOHC_SION_S1_Client0_Req_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14434)
#define IOHC_SION_S1_Client0_Req_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14434)

// Type
#define IOHC_SION_S1_Client0_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4434_IOHC_SION_S1_Client0_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4434_IOHC_SION_S1_Client0_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4434_IOHC_SION_S1_Client0_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4434
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client0_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4434_STRUCT;

// Address
#define IOHC_SION_S1_Client0_Req_TimeSlot_Lower_ADDRESS_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14438)
#define IOHC_SION_S1_Client0_Req_TimeSlot_Lower_ADDRESS_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14438)
// Type
#define IOHC_SION_S1_Client0_Req_TimeSlot_Lower_TYPE                   TYPE_SMN

#define NBIOIOHC_4438_IOHC_SION_S1_Client0_Req_TimeSlot_Lower_OFFSET  0
#define NBIOIOHC_4438_IOHC_SION_S1_Client0_Req_TimeSlot_Lower_WIDTH   32
#define NBIOIOHC_4438_IOHC_SION_S1_Client0_Req_TimeSlot_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4438
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client0_Req_TimeSlot_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4438_STRUCT;

// Address
#define IOHC_SION_S1_Client0_Req_TimeSlot_Upper_ADDRESS_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1443C)
#define IOHC_SION_S1_Client0_Req_TimeSlot_Upper_ADDRESS_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1443C)
// Type
#define IOHC_SION_S1_Client0_Req_TimeSlot_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_443C_IOHC_SION_S1_Client0_Req_TimeSlot_Upper_OFFSET  0
#define NBIOIOHC_443C_IOHC_SION_S1_Client0_Req_TimeSlot_Upper_WIDTH   32
#define NBIOIOHC_443C_IOHC_SION_S1_Client0_Req_TimeSlot_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_443C
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client0_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_443C_STRUCT;

// Address
#define IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_ADDRESS_HYEX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14440)
#define IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_ADDRESS_HYGX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14440)
// Type
#define IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4440_IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4440_IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4440_IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4440
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4440_STRUCT;

// Address
#define IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14444)
#define IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14444)
// Type
#define IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4444_IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4444_IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4444_IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4444
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4444_STRUCT;

// Address
#define IOHC_SION_S1_Client1_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14834)
#define IOHC_SION_S1_Client1_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14834)
// Type
#define IOHC_SION_S1_Client1_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4834_IOHC_SION_S1_Client1_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4834_IOHC_SION_S1_Client1_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4834_IOHC_SION_S1_Client1_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4834
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client1_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4834_STRUCT;

// Address
#define IOHC_SION_S1_Client1_Req_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14838)
#define IOHC_SION_S1_Client1_Req_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14838)
// Type
#define IOHC_SION_S1_Client1_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4838_IOHC_SION_S1_Client1_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4838_IOHC_SION_S1_Client1_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4838_IOHC_SION_S1_Client1_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4838
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client1_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4838_STRUCT;

// Address
#define IOHC_SION_S1_Client1_Req_TimeSlot_Lower_ADDRESS_HYEX              (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1483C)
#define IOHC_SION_S1_Client1_Req_TimeSlot_Lower_ADDRESS_HYGX              (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1483C)
// Type
#define IOHC_SION_S1_Client1_Req_TimeSlot_Lower_TYPE                     TYPE_SMN

#define NBIOIOHC_483C_IOHC_SION_S1_Client1_Req_TimeSlot_Lower_OFFSET     0
#define NBIOIOHC_483C_IOHC_SION_S1_Client1_Req_TimeSlot_Lower_WIDTH      32
#define NBIOIOHC_483C_IOHC_SION_S1_Client1_Req_TimeSlot_Lower_MASK       0xFFFFFFFFF

/// NBIOIOHC_483C
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client1_Req_TimeSlot_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_483C_STRUCT;

// Address
#define IOHC_SION_S1_Client1_Req_TimeSlot_Upper_ADDRESS_HYEX              (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14840)
#define IOHC_SION_S1_Client1_Req_TimeSlot_Upper_ADDRESS_HYGX              (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14840)
// Type
#define IOHC_SION_S1_Client1_Req_TimeSlot_Upper_TYPE                     TYPE_SMN

#define NBIOIOHC_4840_IOHC_SION_S1_Client1_Req_TimeSlot_Upper_OFFSET     0
#define NBIOIOHC_4840_IOHC_SION_S1_Client1_Req_TimeSlot_Upper_WIDTH      32
#define NBIOIOHC_4840_IOHC_SION_S1_Client1_Req_TimeSlot_Upper_MASK       0xFFFFFFFFF

/// NBIOIOHC_4840
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client1_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4840_STRUCT;

// Address
#define IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_ADDRESS_HYEX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14844)
#define IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_ADDRESS_HYGX         (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14844)
// Type
#define IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4844_IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4844_IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4844_IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4844
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4844_STRUCT;

// Address
#define IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14848)
#define IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14848)
// Type
#define IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4848_IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4848_IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4848_IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4848
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4848_STRUCT;

// Address
#define IOHC_SION_S1_Client2_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C38)
#define IOHC_SION_S1_Client2_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C38)
// Type
#define IOHC_SION_S1_Client2_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4C38_IOHC_SION_S1_Client2_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4C38_IOHC_SION_S1_Client2_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4C38_IOHC_SION_S1_Client2_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C38
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client2_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C38_STRUCT;

// Address
#define IOHC_SION_S1_Client2_Req_BurstTarget_Upper_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C3C)
#define IOHC_SION_S1_Client2_Req_BurstTarget_Upper_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C3C)
// Type
#define IOHC_SION_S1_Client2_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4C3C_IOHC_SION_S1_Client2_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4C3C_IOHC_SION_S1_Client2_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4C3C_IOHC_SION_S1_Client2_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C3C
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client2_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C3C_STRUCT;

// Address
#define IOHC_SION_S1_Client2_Req_TimeSlot_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C40)
#define IOHC_SION_S1_Client2_Req_TimeSlot_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C40)
// Type
#define IOHC_SION_S1_Client2_Req_TimeSlot_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4C40_IOHC_SION_S1_Client2_Req_TimeSlot_Lower_OFFSET  0
#define NBIOIOHC_4C40_IOHC_SION_S1_Client2_Req_TimeSlot_Lower_WIDTH   32
#define NBIOIOHC_4C40_IOHC_SION_S1_Client2_Req_TimeSlot_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C40
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client2_Req_TimeSlot_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C40_STRUCT;

// Address
#define IOHC_SION_S1_Client2_Req_TimeSlot_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C44)
#define IOHC_SION_S1_Client2_Req_TimeSlot_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C44)
// Type
#define IOHC_SION_S1_Client2_Req_TimeSlot_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4C44_IOHC_SION_S1_Client2_Req_TimeSlot_Upper_OFFSET  0
#define NBIOIOHC_4C44_IOHC_SION_S1_Client2_Req_TimeSlot_Upper_WIDTH   32
#define NBIOIOHC_4C44_IOHC_SION_S1_Client2_Req_TimeSlot_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C44
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client2_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C44_STRUCT;

// Address
#define IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C48)
#define IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C48)
// Type
#define IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_4C48_IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_4C48_IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_4C48_IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C48
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C48_STRUCT;

// Address
#define IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x14C4C)
#define IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x14C4C)
// Type
#define IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_4C4C_IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_4C4C_IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_4C4C_IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_4C4C
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_4C4C_STRUCT;

// Address
#define IOHC_SION_S1_Client3_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1503C)
#define IOHC_SION_S1_Client3_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1503C)
// Type
#define IOHC_SION_S1_Client3_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_503C_IOHC_SION_S1_Client3_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_503C_IOHC_SION_S1_Client3_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_503C_IOHC_SION_S1_Client3_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_503C
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client3_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_503C_STRUCT;

// Address
#define IOHC_SION_S1_Client3_Req_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15040)
#define IOHC_SION_S1_Client3_Req_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15040)
// Type
#define IOHC_SION_S1_Client3_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5040_IOHC_SION_S1_Client3_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5040_IOHC_SION_S1_Client3_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5040_IOHC_SION_S1_Client3_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5040
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client3_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5040_STRUCT;

// Address
#define IOHC_SION_S1_Client3_Req_TimeSlot_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15044)
#define IOHC_SION_S1_Client3_Req_TimeSlot_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15044)
// Type
#define IOHC_SION_S1_Client3_Req_TimeSlot_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5044_IOHC_SION_S1_Client3_Req_TimeSlot_Lower_OFFSET  0
#define NBIOIOHC_5044_IOHC_SION_S1_Client3_Req_TimeSlot_Lower_WIDTH   32
#define NBIOIOHC_5044_IOHC_SION_S1_Client3_Req_TimeSlot_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5044
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client3_Req_TimeSlot_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5044_STRUCT;

// Address
#define IOHC_SION_S1_Client3_Req_TimeSlot_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15048)
#define IOHC_SION_S1_Client3_Req_TimeSlot_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15048)
// Type
#define IOHC_SION_S1_Client3_Req_TimeSlot_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5048_IOHC_SION_S1_Client3_Req_TimeSlot_Upper_OFFSET  0
#define NBIOIOHC_5048_IOHC_SION_S1_Client3_Req_TimeSlot_Upper_WIDTH   32
#define NBIOIOHC_5048_IOHC_SION_S1_Client3_Req_TimeSlot_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5048
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client3_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5048_STRUCT;

// Address
#define IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1504C)
#define IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1504C)
// Type
#define IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_504C_IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_504C_IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_504C_IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_504C
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_504C_STRUCT;

// Address
#define IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15050)
#define IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15050)
// Type
#define IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5050_IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5050_IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5050_IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5050
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5050_STRUCT;

// Address
#define IOHC_SION_S1_Client4_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15440)
#define IOHC_SION_S1_Client4_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15440)
// Type
#define IOHC_SION_S1_Client4_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5440_IOHC_SION_S1_Client4_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_5440_IOHC_SION_S1_Client4_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_5440_IOHC_SION_S1_Client4_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5440
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client4_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5440_STRUCT;

// Address
#define IOHC_SION_S1_Client4_Req_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15444)
#define IOHC_SION_S1_Client4_Req_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15444)
// Type
#define IOHC_SION_S1_Client4_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5444_IOHC_SION_S1_Client4_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5444_IOHC_SION_S1_Client4_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5444_IOHC_SION_S1_Client4_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5444
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client4_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5444_STRUCT;

// Address
#define IOHC_SION_S1_Client4_Req_TimeSlot_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15448)
#define IOHC_SION_S1_Client4_Req_TimeSlot_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15448)
// Type
#define IOHC_SION_S1_Client4_Req_TimeSlot_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5448_IOHC_SION_S1_Client4_Req_TimeSlot_Lower_OFFSET  0
#define NBIOIOHC_5448_IOHC_SION_S1_Client4_Req_TimeSlot_Lower_WIDTH   32
#define NBIOIOHC_5448_IOHC_SION_S1_Client4_Req_TimeSlot_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5448
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client4_Req_TimeSlot_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5448_STRUCT;

// Address
#define IOHC_SION_S1_Client4_Req_TimeSlot_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1544C)
#define IOHC_SION_S1_Client4_Req_TimeSlot_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1544C)
// Type
#define IOHC_SION_S1_Client4_Req_TimeSlot_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_544C_IOHC_SION_S1_Client4_Req_TimeSlot_Upper_OFFSET  0
#define NBIOIOHC_544C_IOHC_SION_S1_Client4_Req_TimeSlot_Upper_WIDTH   32
#define NBIOIOHC_544C_IOHC_SION_S1_Client4_Req_TimeSlot_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_544C
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client4_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_544C_STRUCT;

// Address
#define IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15450)
#define IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15450)
// Type
#define IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5450_IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_5450_IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_5450_IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5450
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5450_STRUCT;

// Address
#define IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15454)
#define IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15454)
// Type
#define IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5454_IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5454_IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5454_IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5454
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5454_STRUCT;

// Address
#define IOHC_SION_S1_Client5_Req_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15844)
#define IOHC_SION_S1_Client5_Req_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15844)
// Type
#define IOHC_SION_S1_Client5_Req_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5844_IOHC_SION_S1_Client5_Req_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_5844_IOHC_SION_S1_Client5_Req_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_5844_IOHC_SION_S1_Client5_Req_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5844
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client5_Req_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5844_STRUCT;

// Address
#define IOHC_SION_S1_Client5_Req_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15848)
#define IOHC_SION_S1_Client5_Req_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15848)
// Type
#define IOHC_SION_S1_Client5_Req_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5848_IOHC_SION_S1_Client5_Req_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5848_IOHC_SION_S1_Client5_Req_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5848_IOHC_SION_S1_Client5_Req_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5848
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client5_Req_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5848_STRUCT;

// Address
#define IOHC_SION_S1_Client5_Req_TimeSlot_Lower_ADDRESS_HYEX             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x1584C)
#define IOHC_SION_S1_Client5_Req_TimeSlot_Lower_ADDRESS_HYGX             (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1584C)
// Type
#define IOHC_SION_S1_Client5_Req_TimeSlot_Lower_TYPE                    TYPE_SMN

#define NBIOIOHC_584C_IOHC_SION_S1_Client5_Req_TimeSlot_Lower_OFFSET    0
#define NBIOIOHC_584C_IOHC_SION_S1_Client5_Req_TimeSlot_Lower_WIDTH     32
#define NBIOIOHC_584C_IOHC_SION_S1_Client5_Req_TimeSlot_Lower_MASK      0xFFFFFFFFF

/// NBIOIOHC_584C
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client5_Req_TimeSlot_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_584C_STRUCT;

// Address
#define IOHC_SION_S1_Client5_Req_TimeSlot_Upper_ADDRESS_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15850)
#define IOHC_SION_S1_Client5_Req_TimeSlot_Upper_ADDRESS_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15850)
// Type
#define IOHC_SION_S1_Client5_Req_TimeSlot_Upper_TYPE                   TYPE_SMN

#define NBIOIOHC_5850_IOHC_SION_S1_Client5_Req_TimeSlot_Upper_OFFSET   0
#define NBIOIOHC_5850_IOHC_SION_S1_Client5_Req_TimeSlot_Upper_WIDTH    32
#define NBIOIOHC_5850_IOHC_SION_S1_Client5_Req_TimeSlot_Upper_MASK     0xFFFFFFFFF

/// NBIOIOHC_5850
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client5_Req_TimeSlot_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5850_STRUCT;

// Address
#define IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15854)
#define IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15854)
// Type
#define IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_TYPE                  TYPE_SMN

#define NBIOIOHC_5854_IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_OFFSET  0
#define NBIOIOHC_5854_IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_WIDTH   32
#define NBIOIOHC_5854_IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_MASK    0xFFFFFFFFF

/// NBIOIOHC_5854
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5854_STRUCT;

// Address
#define IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x15858)
#define IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x15858)
// Type
#define IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_TYPE                  TYPE_SMN

#define NBIOIOHC_5858_IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_OFFSET  0
#define NBIOIOHC_5858_IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_WIDTH   32
#define NBIOIOHC_5858_IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_MASK    0xFFFFFFFFF

/// NBIOIOHC_5858
typedef union {
  struct {
    UINT32    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper : 32;         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOIOHC_5858_STRUCT;

// Address
#define PARITY_CONTROL_0_ADDRESS_HYEX              (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20000)
#define PARITY_CONTROL_0_ADDRESS_HYGX              (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20000)
// Type
#define PARITY_CONTROL_0_TYPE                     TYPE_SMN

#define NBRASCFG_0000_ParityCorrThreshold_OFFSET  0
#define NBRASCFG_0000_ParityCorrThreshold_WIDTH   16
#define NBRASCFG_0000_ParityCorrThreshold_MASK    0xffff
#define NBRASCFG_0000_ParityUCPThreshold_OFFSET   16
#define NBRASCFG_0000_ParityUCPThreshold_WIDTH    16
#define NBRASCFG_0000_ParityUCPThreshold_MASK     0xffff0000L

/// NBRASCFG_0000
typedef union {
  struct {
    UINT32    ParityCorrThreshold : 16;                                ///<
    UINT32    ParityUCPThreshold  : 16;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBRASCFG_0000_STRUCT;

// Address
#define PARITY_CONTROL_1_ADDRESS_HYEX                   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20004)
#define PARITY_CONTROL_1_ADDRESS_HYGX                   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20004)
// Type
#define PARITY_CONTROL_1_TYPE                          TYPE_SMN

#define NBRASCFG_0004_ParityErrGenGroupSel_OFFSET      0
#define NBRASCFG_0004_ParityErrGenGroupSel_WIDTH       8
#define NBRASCFG_0004_ParityErrGenGroupSel_MASK        0xff
#define NBRASCFG_0004_ParityErrGenGroupTypeSel_OFFSET  8
#define NBRASCFG_0004_ParityErrGenGroupTypeSel_WIDTH   1
#define NBRASCFG_0004_ParityErrGenGroupTypeSel_MASK    0x100
#define NBRASCFG_0004_Reserved_10_9_OFFSET             9
#define NBRASCFG_0004_Reserved_10_9_WIDTH              2
#define NBRASCFG_0004_Reserved_10_9_MASK               0x600
#define NBRASCFG_0004_ParityErrGenIdSel_OFFSET         11
#define NBRASCFG_0004_ParityErrGenIdSel_WIDTH          5
#define NBRASCFG_0004_ParityErrGenIdSel_MASK           0xf800
#define NBRASCFG_0004_ParityErrGenCmd_OFFSET           16
#define NBRASCFG_0004_ParityErrGenCmd_WIDTH            4
#define NBRASCFG_0004_ParityErrGenCmd_MASK             0xf0000
#define NBRASCFG_0004_Reserved_29_20_OFFSET            20
#define NBRASCFG_0004_Reserved_29_20_WIDTH             10
#define NBRASCFG_0004_Reserved_29_20_MASK              0x3ff00000
#define NBRASCFG_0004_ParityErrGenTrigger_OFFSET       30
#define NBRASCFG_0004_ParityErrGenTrigger_WIDTH        1
#define NBRASCFG_0004_ParityErrGenTrigger_MASK         0x40000000
#define NBRASCFG_0004_ParityErrGenInjectAllow_OFFSET   31
#define NBRASCFG_0004_ParityErrGenInjectAllow_WIDTH    1
#define NBRASCFG_0004_ParityErrGenInjectAllow_MASK     0x80000000L

/// NBRASCFG_0004
typedef union {
  struct {
    UINT32    ParityErrGenGroupSel     : 8;                            ///<
    UINT32    ParityErrGenGroupTypeSel : 1;                            ///<
    UINT32    Reserved_10_9            : 2;                            ///<
    UINT32    ParityErrGenIdSel        : 5;                            ///<
    UINT32    ParityErrGenCmd          : 4;                            ///<
    UINT32    Reserved_29_20           : 10;                           ///<
    UINT32    ParityErrGenTrigger      : 1;                            ///<
    UINT32    ParityErrGenInjectAllow  : 1;                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBRASCFG_0004_STRUCT;

// Address
#define PARITY_SEVERITY_CONTROL_UNCORR_0_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20008)
#define PARITY_SEVERITY_CONTROL_UNCORR_0_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20008)
// Type
#define PARITY_SEVERITY_CONTROL_UNCORR_0_TYPE         TYPE_SMN

#define NBRASCFG_0008_ParityErrSevUnCorrGrp0_OFFSET   0
#define NBRASCFG_0008_ParityErrSevUnCorrGrp0_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp0_MASK     0x3
#define NBRASCFG_0008_ParityErrSevUnCorrGrp1_OFFSET   2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp1_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp1_MASK     0xc
#define NBRASCFG_0008_ParityErrSevUnCorrGrp2_OFFSET   4
#define NBRASCFG_0008_ParityErrSevUnCorrGrp2_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp2_MASK     0x30
#define NBRASCFG_0008_ParityErrSevUnCorrGrp3_OFFSET   6
#define NBRASCFG_0008_ParityErrSevUnCorrGrp3_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp3_MASK     0xc0
#define NBRASCFG_0008_ParityErrSevUnCorrGrp4_OFFSET   8
#define NBRASCFG_0008_ParityErrSevUnCorrGrp4_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp4_MASK     0x300
#define NBRASCFG_0008_ParityErrSevUnCorrGrp5_OFFSET   10
#define NBRASCFG_0008_ParityErrSevUnCorrGrp5_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp5_MASK     0xc00
#define NBRASCFG_0008_ParityErrSevUnCorrGrp6_OFFSET   12
#define NBRASCFG_0008_ParityErrSevUnCorrGrp6_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp6_MASK     0x3000
#define NBRASCFG_0008_ParityErrSevUnCorrGrp7_OFFSET   14
#define NBRASCFG_0008_ParityErrSevUnCorrGrp7_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp7_MASK     0xc000
#define NBRASCFG_0008_ParityErrSevUnCorrGrp8_OFFSET   16
#define NBRASCFG_0008_ParityErrSevUnCorrGrp8_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp8_MASK     0x30000
#define NBRASCFG_0008_ParityErrSevUnCorrGrp9_OFFSET   18
#define NBRASCFG_0008_ParityErrSevUnCorrGrp9_WIDTH    2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp9_MASK     0xc0000
#define NBRASCFG_0008_ParityErrSevUnCorrGrp10_OFFSET  20
#define NBRASCFG_0008_ParityErrSevUnCorrGrp10_WIDTH   2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp10_MASK    0x300000
#define NBRASCFG_0008_ParityErrSevUnCorrGrp11_OFFSET  22
#define NBRASCFG_0008_ParityErrSevUnCorrGrp11_WIDTH   2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp11_MASK    0xc00000
#define NBRASCFG_0008_ParityErrSevUnCorrGrp12_OFFSET  24
#define NBRASCFG_0008_ParityErrSevUnCorrGrp12_WIDTH   2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp12_MASK    0x3000000
#define NBRASCFG_0008_ParityErrSevUnCorrGrp13_OFFSET  26
#define NBRASCFG_0008_ParityErrSevUnCorrGrp13_WIDTH   2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp13_MASK    0xc000000
#define NBRASCFG_0008_ParityErrSevUnCorrGrp14_OFFSET  28
#define NBRASCFG_0008_ParityErrSevUnCorrGrp14_WIDTH   2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp14_MASK    0x30000000
#define NBRASCFG_0008_ParityErrSevUnCorrGrp15_OFFSET  30
#define NBRASCFG_0008_ParityErrSevUnCorrGrp15_WIDTH   2
#define NBRASCFG_0008_ParityErrSevUnCorrGrp15_MASK    0xc0000000

/// NBRASCFG_0008
typedef union {
  struct {
    UINT32    ParityErrSevUnCorrGrp0  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp1  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp2  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp3  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp4  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp5  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp6  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp7  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp8  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp9  : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp10 : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp11 : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp12 : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp13 : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp14 : 2;                             ///<
    UINT32    ParityErrSevUnCorrGrp15 : 2;                             ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBRASCFG_0008_STRUCT;

// Address
#define PARITY_SEVERITY_CONTROL_UNCORR_1_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x2000c)
#define PARITY_SEVERITY_CONTROL_UNCORR_1_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x2000c)
// Type
#define PARITY_SEVERITY_CONTROL_UNCORR_1_TYPE         TYPE_SMN

#define NBRASCFG_000C_ParityErrSevUnCorrGrp16_OFFSET  0
#define NBRASCFG_000C_ParityErrSevUnCorrGrp16_WIDTH   2
#define NBRASCFG_000C_ParityErrSevUnCorrGrp16_MASK    0x3
#define NBRASCFG_000C_ParityErrSevUnCorrGrp17_OFFSET  2
#define NBRASCFG_000C_ParityErrSevUnCorrGrp17_WIDTH   2
#define NBRASCFG_000C_ParityErrSevUnCorrGrp17_MASK    0xc
#define NBRASCFG_000C_ParityErrSevUnCorrGrp18_OFFSET  4
#define NBRASCFG_000C_ParityErrSevUnCorrGrp18_WIDTH   2
#define NBRASCFG_000C_ParityErrSevUnCorrGrp18_MASK    0x30
#define NBRASCFG_000C_ParityErrSevUnCorrGrp19_OFFSET  6
#define NBRASCFG_000C_ParityErrSevUnCorrGrp19_WIDTH   2
#define NBRASCFG_000C_ParityErrSevUnCorrGrp19_MASK    0xC0
#define NBRASCFG_000C_ParityErrSevUnCorrGrp20_OFFSET  8
#define NBRASCFG_000C_ParityErrSevUnCorrGrp20_WIDTH   2
#define NBRASCFG_000C_ParityErrSevUnCorrGrp20_MASK    0x300

/// NBRASCFG_000C
typedef union {
  struct {
    UINT32    ParityErrSevUnCorrGrp16 : 2;                              ///<
    UINT32    ParityErrSevUnCorrGrp17 : 2;                              ///<
    UINT32    ParityErrSevUnCorrGrp18 : 2;                              ///<
    UINT32    ParityErrSevUnCorrGrp19 : 2;                              ///< Only HyGx support
    UINT32    ParityErrSevUnCorrGrp20 : 2;                              ///< Only HyGx support
    UINT32    Reserved_31_10          : 22;                             ///<
  } Field;                                                              ///<
  UINT32    Value;                                                      ///<
} NBRASCFG_000C_STRUCT;

// Address
#define PARITY_SEVERITY_CONTROL_CORR_0_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20010)
#define PARITY_SEVERITY_CONTROL_CORR_0_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20010)
// Type
#define PARITY_SEVERITY_CONTROL_CORR_0_TYPE         TYPE_SMN

#define NBRASCFG_0010_ParityErrSevCorrGrp0_OFFSET   0
#define NBRASCFG_0010_ParityErrSevCorrGrp0_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp0_MASK     0x3
#define NBRASCFG_0010_ParityErrSevCorrGrp1_OFFSET   2
#define NBRASCFG_0010_ParityErrSevCorrGrp1_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp1_MASK     0xc
#define NBRASCFG_0010_ParityErrSevCorrGrp2_OFFSET   4
#define NBRASCFG_0010_ParityErrSevCorrGrp2_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp2_MASK     0x30
#define NBRASCFG_0010_ParityErrSevCorrGrp3_OFFSET   6
#define NBRASCFG_0010_ParityErrSevCorrGrp3_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp3_MASK     0xc0
#define NBRASCFG_0010_ParityErrSevCorrGrp4_OFFSET   8
#define NBRASCFG_0010_ParityErrSevCorrGrp4_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp4_MASK     0x300
#define NBRASCFG_0010_ParityErrSevCorrGrp5_OFFSET   10
#define NBRASCFG_0010_ParityErrSevCorrGrp5_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp5_MASK     0xc00
#define NBRASCFG_0010_ParityErrSevCorrGrp6_OFFSET   12
#define NBRASCFG_0010_ParityErrSevCorrGrp6_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp6_MASK     0x3000
#define NBRASCFG_0010_ParityErrSevCorrGrp7_OFFSET   14
#define NBRASCFG_0010_ParityErrSevCorrGrp7_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp7_MASK     0xc000
#define NBRASCFG_0010_ParityErrSevCorrGrp8_OFFSET   16
#define NBRASCFG_0010_ParityErrSevCorrGrp8_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp8_MASK     0x30000
#define NBRASCFG_0010_ParityErrSevCorrGrp9_OFFSET   18
#define NBRASCFG_0010_ParityErrSevCorrGrp9_WIDTH    2
#define NBRASCFG_0010_ParityErrSevCorrGrp9_MASK     0xc0000
#define NBRASCFG_0010_ParityErrSevCorrGrp10_OFFSET  20
#define NBRASCFG_0010_ParityErrSevCorrGrp10_WIDTH   2
#define NBRASCFG_0010_ParityErrSevCorrGrp10_MASK    0x300000
#define NBRASCFG_0010_ParityErrSevCorrGrp11_OFFSET  22
#define NBRASCFG_0010_ParityErrSevCorrGrp11_WIDTH   2
#define NBRASCFG_0010_ParityErrSevCorrGrp11_MASK    0xc00000
#define NBRASCFG_0010_ParityErrSevCorrGrp12_OFFSET  24
#define NBRASCFG_0010_ParityErrSevCorrGrp12_WIDTH   2
#define NBRASCFG_0010_ParityErrSevCorrGrp12_MASK    0x3000000
#define NBRASCFG_0010_ParityErrSevCorrGrp13_OFFSET  26
#define NBRASCFG_0010_ParityErrSevCorrGrp13_WIDTH   2
#define NBRASCFG_0010_ParityErrSevCorrGrp13_MASK    0xc000000
#define NBRASCFG_0010_ParityErrSevCorrGrp14_OFFSET  28
#define NBRASCFG_0010_ParityErrSevCorrGrp14_WIDTH   2
#define NBRASCFG_0010_ParityErrSevCorrGrp14_MASK    0x30000000
#define NBRASCFG_0010_ParityErrSevCorrGrp15_OFFSET  30
#define NBRASCFG_0010_ParityErrSevCorrGrp15_WIDTH   2
#define NBRASCFG_0010_ParityErrSevCorrGrp15_MASK    0xc0000000L

/// NBRASCFG_0010
typedef union {
  struct {
    UINT32    ParityErrSevCorrGrp0  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp1  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp2  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp3  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp4  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp5  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp6  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp7  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp8  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp9  : 2;                               ///<
    UINT32    ParityErrSevCorrGrp10 : 2;                               ///<
    UINT32    ParityErrSevCorrGrp11 : 2;                               ///<
    UINT32    ParityErrSevCorrGrp12 : 2;                               ///<
    UINT32    ParityErrSevCorrGrp13 : 2;                               ///<
    UINT32    ParityErrSevCorrGrp14 : 2;                               ///<
    UINT32    ParityErrSevCorrGrp15 : 2;                               ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBRASCFG_0010_STRUCT;

// Address
#define PARITY_SEVERITY_CONTROL_CORR_1_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20014)
#define PARITY_SEVERITY_CONTROL_CORR_1_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20014)
// Type
#define PARITY_SEVERITY_CONTROL_CORR_1_TYPE         TYPE_SMN

#define NBRASCFG_0014_ParityErrSevCorrGrp16_OFFSET  0
#define NBRASCFG_0014_ParityErrSevCorrGrp16_WIDTH   2
#define NBRASCFG_0014_ParityErrSevCorrGrp16_MASK    0x3
#define NBRASCFG_0014_ParityErrSevCorrGrp17_OFFSET  2
#define NBRASCFG_0014_ParityErrSevCorrGrp17_WIDTH   2
#define NBRASCFG_0014_ParityErrSevCorrGrp17_MASK    0xc
#define NBRASCFG_0014_ParityErrSevCorrGrp18_OFFSET  4
#define NBRASCFG_0014_ParityErrSevCorrGrp18_WIDTH   2
#define NBRASCFG_0014_ParityErrSevCorrGrp18_MASK    0x30
#define NBRASCFG_0014_ParityErrSevCorrGrp19_OFFSET  6
#define NBRASCFG_0014_ParityErrSevCorrGrp19_WIDTH   2
#define NBRASCFG_0014_ParityErrSevCorrGrp19_MASK    0xC0
#define NBRASCFG_0014_ParityErrSevCorrGrp20_OFFSET  8
#define NBRASCFG_0014_ParityErrSevCorrGrp20_WIDTH   2
#define NBRASCFG_0014_ParityErrSevCorrGrp20_MASK    0x300

/// NBRASCFG_0014
typedef union {
  struct {
    UINT32    ParityErrSevCorrGrp16 : 2;                               ///<
    UINT32    ParityErrSevCorrGrp17 : 2;                               ///<
    UINT32    ParityErrSevCorrGrp18 : 2;                               ///<
    UINT32    ParityErrSevCorrGrp19 : 2;                               ///< Only HyGx support
    UINT32    ParityErrSevCorrGrp20 : 2;                               ///< Only HyGx support
    UINT32    Reserved_31_10        : 22;                              ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBRASCFG_0014_STRUCT;

// Address
#define PARITY_SEVERITY_CONTROL_UCP_0_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20018)
#define PARITY_SEVERITY_CONTROL_UCP_0_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20018)
// Type
#define PARITY_SEVERITY_CONTROL_UCP_0_TYPE         TYPE_SMN

#define NBRASCFG_0018_ParityErrSevUCPGrp0_OFFSET   0
#define NBRASCFG_0018_ParityErrSevUCPGrp0_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp0_MASK     0x3
#define NBRASCFG_0018_ParityErrSevUCPGrp1_OFFSET   2
#define NBRASCFG_0018_ParityErrSevUCPGrp1_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp1_MASK     0xc
#define NBRASCFG_0018_ParityErrSevUCPGrp2_OFFSET   4
#define NBRASCFG_0018_ParityErrSevUCPGrp2_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp2_MASK     0x30
#define NBRASCFG_0018_ParityErrSevUCPGrp3_OFFSET   6
#define NBRASCFG_0018_ParityErrSevUCPGrp3_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp3_MASK     0xc0
#define NBRASCFG_0018_ParityErrSevUCPGrp4_OFFSET   8
#define NBRASCFG_0018_ParityErrSevUCPGrp4_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp4_MASK     0x300
#define NBRASCFG_0018_ParityErrSevUCPGrp5_OFFSET   10
#define NBRASCFG_0018_ParityErrSevUCPGrp5_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp5_MASK     0xc00
#define NBRASCFG_0018_ParityErrSevUCPGrp6_OFFSET   12
#define NBRASCFG_0018_ParityErrSevUCPGrp6_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp6_MASK     0x3000
#define NBRASCFG_0018_ParityErrSevUCPGrp7_OFFSET   14
#define NBRASCFG_0018_ParityErrSevUCPGrp7_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp7_MASK     0xc000
#define NBRASCFG_0018_ParityErrSevUCPGrp8_OFFSET   16
#define NBRASCFG_0018_ParityErrSevUCPGrp8_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp8_MASK     0x30000
#define NBRASCFG_0018_ParityErrSevUCPGrp9_OFFSET   18
#define NBRASCFG_0018_ParityErrSevUCPGrp9_WIDTH    2
#define NBRASCFG_0018_ParityErrSevUCPGrp9_MASK     0xc0000
#define NBRASCFG_0018_ParityErrSevUCPGrp10_OFFSET  20
#define NBRASCFG_0018_ParityErrSevUCPGrp10_WIDTH   2
#define NBRASCFG_0018_ParityErrSevUCPGrp10_MASK    0x300000
#define NBRASCFG_0018_ParityErrSevUCPGrp11_OFFSET  22
#define NBRASCFG_0018_ParityErrSevUCPGrp11_WIDTH   2
#define NBRASCFG_0018_ParityErrSevUCPGrp11_MASK    0xc00000
#define NBRASCFG_0018_ParityErrSevUCPGrp12_OFFSET  24
#define NBRASCFG_0018_ParityErrSevUCPGrp12_WIDTH   2
#define NBRASCFG_0018_ParityErrSevUCPGrp12_MASK    0x3000000
#define NBRASCFG_0018_ParityErrSevUCPGrp13_OFFSET  26
#define NBRASCFG_0018_ParityErrSevUCPGrp13_WIDTH   2
#define NBRASCFG_0018_ParityErrSevUCPGrp13_MASK    0xC000000
#define NBRASCFG_0018_ParityErrSevUCPGrp14_OFFSET  28
#define NBRASCFG_0018_ParityErrSevUCPGrp14_WIDTH   2
#define NBRASCFG_0018_ParityErrSevUCPGrp14_MASK    0x30000000

/// NBRASCFG_0018
typedef union {
  struct {
    UINT32    ParityErrSevUCPGrp0  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp1  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp2  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp3  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp4  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp5  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp6  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp7  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp8  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp9  : 2;                                ///<
    UINT32    ParityErrSevUCPGrp10 : 2;                                ///<
    UINT32    ParityErrSevUCPGrp11 : 2;                                ///<
    UINT32    ParityErrSevUCPGrp12 : 2;                                ///<
    UINT32    ParityErrSevUCPGrp13 : 2;                                ///< Only HyGx support
    UINT32    ParityErrSevUCPGrp14 : 2;                                ///< Only HyGx support
    UINT32    reserved_31_30       : 2;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBRASCFG_0018_STRUCT;

// Address
#define ErrEvent_ACTION_CONTROL_ADDRESS_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20168)
#define ErrEvent_ACTION_CONTROL_ADDRESS_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20168)
// Type
#define ErrEvent_ACTION_CONTROL_TYPE                  TYPE_SMN

#define ErrEvent_ACTION_CONTROL_APML_ERR_En_OFFSET    0
#define ErrEvent_ACTION_CONTROL_APML_ERR_En_WIDTH     1
#define ErrEvent_ACTION_CONTROL_APML_ERR_En_MASK      0x1
#define ErrEvent_ACTION_CONTROL_IntrGenSel_OFFSET     1
#define ErrEvent_ACTION_CONTROL_IntrGenSel_WIDTH      2
#define ErrEvent_ACTION_CONTROL_IntrGenSel_MASK       0x6
#define ErrEvent_ACTION_CONTROL_LinkDis_En_OFFSET     3
#define ErrEvent_ACTION_CONTROL_LinkDis_En_WIDTH      1
#define ErrEvent_ACTION_CONTROL_LinkDis_En_MASK       0x8
#define ErrEvent_ACTION_CONTROL_SyncFlood_En_OFFSET   4
#define ErrEvent_ACTION_CONTROL_SyncFlood_En_WIDTH    1
#define ErrEvent_ACTION_CONTROL_SyncFlood_En_MASK     0x10
#define ErrEvent_ACTION_CONTROL_Reserved_31_5_OFFSET  5
#define ErrEvent_ACTION_CONTROL_Reserved_31_5_WIDTH   27
#define ErrEvent_ACTION_CONTROL_Reserved_31_5_MASK    0xffffffe0L

/// IOHCRAS_00000168
typedef union {
  struct {
    UINT32    APML_ERR_En   : 1;                                       ///<
    UINT32    IntrGenSel    : 2;                                       ///<
    UINT32    LinkDis_En    : 1;                                       ///<
    UINT32    SyncFlood_En  : 1;                                       ///<
    UINT32    Reserved_31_5 : 27;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} ErrEvent_ACTION_CONTROL_STRUCT;

#define MISC_SEVERITY_CONTROL_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20158)
#define ParitySerr_ACTION_CONTROL_ADDRESS_HYEX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x2016c)
#define ParityFatal_ACTION_CONTROL_ADDRESS_HYEX     (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20170)
#define ParityNonFatal_ACTION_CONTROL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20174)
#define ParityCorr_ACTION_CONTROL_ADDRESS_HYEX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20178)
#define EGRESS_POISON_SEVERITY_LO_ADDRESS_HYEX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20830)
#define EGRESS_POISON_SEVERITY_HI_ADDRESS_HYEX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20834)

#define MISC_SEVERITY_CONTROL_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20158)
#define ParitySerr_ACTION_CONTROL_ADDRESS_HYGX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x2016c)
#define ParityFatal_ACTION_CONTROL_ADDRESS_HYGX     (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20170)
#define ParityNonFatal_ACTION_CONTROL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20174)
#define ParityCorr_ACTION_CONTROL_ADDRESS_HYGX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20178)
#define EGRESS_POISON_SEVERITY_LO_ADDRESS_HYGX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20830)
#define EGRESS_POISON_SEVERITY_HI_ADDRESS_HYGX      (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20834)

// Address
#define POISON_ACTION_CONTROL_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x20814)
#define POISON_ACTION_CONTROL_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x20814)
// Type
#define POISON_ACTION_CONTROL_TYPE                              TYPE_SMN

#define POISON_ACTION_CONTROL_IntPoisonAPMLErrEn_OFFSET         0
#define POISON_ACTION_CONTROL_IntPoisonAPMLErrEn_WIDTH          1
#define POISON_ACTION_CONTROL_IntPoisonAPMLErrEn_MASK           0x1
#define POISON_ACTION_CONTROL_IntPoisonIntrGenSel_OFFSET        1
#define POISON_ACTION_CONTROL_IntPoisonIntrGenSel_WIDTH         2
#define POISON_ACTION_CONTROL_IntPoisonIntrGenSel_MASK          0x6
#define POISON_ACTION_CONTROL_IntPoisonLinkDisEn_OFFSET         3
#define POISON_ACTION_CONTROL_IntPoisonLinkDisEn_WIDTH          1
#define POISON_ACTION_CONTROL_IntPoisonLinkDisEn_MASK           0x8
#define POISON_ACTION_CONTROL_IntPoisonSyncFloodEn_OFFSET       4
#define POISON_ACTION_CONTROL_IntPoisonSyncFloodEn_WIDTH        1
#define POISON_ACTION_CONTROL_IntPoisonSyncFloodEn_MASK         0x10
#define POISON_ACTION_CONTROL_Reserved_7_5_OFFSET               5
#define POISON_ACTION_CONTROL_Reserved_7_5_WIDTH                3
#define POISON_ACTION_CONTROL_Reserved_7_5_MASK                 0xE0
#define POISON_ACTION_CONTROL_EgressPoisonLSAPMLErrEn_OFFSET    8
#define POISON_ACTION_CONTROL_EgressPoisonLSAPMLErrEn_WIDTH     1
#define POISON_ACTION_CONTROL_EgressPoisonLSAPMLErrEn_MASK      0x100
#define POISON_ACTION_CONTROL_EgressPoisonLSIntrGenSel_OFFSET   9
#define POISON_ACTION_CONTROL_EgressPoisonLSIntrGenSel_WIDTH    2
#define POISON_ACTION_CONTROL_EgressPoisonLSIntrGenSel_MASK     0x600
#define POISON_ACTION_CONTROL_EgressPoisonLSLinkDisEn_OFFSET    11
#define POISON_ACTION_CONTROL_EgressPoisonLSLinkDisEn_WIDTH     1
#define POISON_ACTION_CONTROL_EgressPoisonLSLinkDisEn_MASK      0x800
#define POISON_ACTION_CONTROL_EgressPoisonLSSyncFloodEn_OFFSET  12
#define POISON_ACTION_CONTROL_EgressPoisonLSSyncFloodEn_WIDTH   1
#define POISON_ACTION_CONTROL_EgressPoisonLSSyncFloodEn_MASK    0x1000
#define POISON_ACTION_CONTROL_Reserved_15_13_OFFSET             13
#define POISON_ACTION_CONTROL_Reserved_15_13_WIDTH              3
#define POISON_ACTION_CONTROL_Reserved_15_13_MASK               0xE000
#define POISON_ACTION_CONTROL_EgressPoisonHSAPMLErrEn_OFFSET    16
#define POISON_ACTION_CONTROL_EgressPoisonHSAPMLErrEn_WIDTH     1
#define POISON_ACTION_CONTROL_EgressPoisonHSAPMLErrEn_MASK      0x10000
#define POISON_ACTION_CONTROL_EgressPoisonHSIntrGenSel_OFFSET   17
#define POISON_ACTION_CONTROL_EgressPoisonHSIntrGenSel_WIDTH    2
#define POISON_ACTION_CONTROL_EgressPoisonHSIntrGenSel_MASK     0x60000
#define POISON_ACTION_CONTROL_EgressPoisonHSLinkDisEn_OFFSET    19
#define POISON_ACTION_CONTROL_EgressPoisonHSLinkDisEn_WIDTH     1
#define POISON_ACTION_CONTROL_EgressPoisonHSLinkDisEn_MASK      0x80000
#define POISON_ACTION_CONTROL_EgressPoisonHSSyncFloodEn_OFFSET  20
#define POISON_ACTION_CONTROL_EgressPoisonHSSyncFloodEn_WIDTH   1
#define POISON_ACTION_CONTROL_EgressPoisonHSSyncFloodEn_MASK    0x100000
#define POISON_ACTION_CONTROL_Reserved_31_21_OFFSET             21
#define POISON_ACTION_CONTROL_Reserved_31_21_WIDTH              11
#define POISON_ACTION_CONTROL_Reserved_31_21_MASK               0xFFE00000

/// POISON_ACTION_CONTROL Register
typedef union {
  struct {
    UINT32    IntPoisonAPMLErrEn        : 1;                           ///<
    UINT32    IntPoisonIntrGenSel       : 2;                           ///<
    UINT32    IntPoisonLinkDisEn        : 1;                           ///<
    UINT32    IntPoisonSyncFloodEn      : 1;                           ///<
    UINT32    Reserved_7_5              : 3;                           ///<
    UINT32    EgressPoisonLSAPMLErrEn   : 1;                           ///<
    UINT32    EgressPoisonLSIntrGenSel  : 2;                           ///<
    UINT32    EgressPoisonLSLinkDisEn   : 1;                           ///<
    UINT32    EgressPoisonLSSyncFloodEn : 1;                           ///<
    UINT32    Reserved_15_13            : 3;                           ///<
    UINT32    EgressPoisonHSAPMLErrEn   : 1;                           ///<
    UINT32    EgressPoisonHSIntrGenSel  : 2;                           ///<
    UINT32    EgressPoisonHSLinkDisEn   : 1;                           ///<
    UINT32    EgressPoisonHSSyncFloodEn : 1;                           ///<
    UINT32    Reserved_31_21            : 11;                          ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} POISON_ACTION_CONTROL_STRUCT;

// IOHUB::IOMMUL2::IOMMU_CAP_BASE_LO_aliasSMN;
// Address
#define IOMMU_CAP_BASE_LO_ADDRESS_HYEX                (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x00044)
#define IOMMU_CAP_BASE_HI_ADDRESS_HYEX                (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x00048)
#define IOMMU_CAP_VF_BASE_LO_ADDRESS_HYEX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x000D0)
#define IOMMU_CAP_VF_BASE_HI_ADDRESS_HYEX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x000D4)
#define IOMMU_CAP_VFCNTL_BASE_LO_ADDRESS_HYEX         (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x000D8)
#define IOMMU_CAP_VFCNTL_BASE_HI_ADDRESS_HYEX         (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x000DC)

#define IOMMU_CAP_BASE_LO_ADDRESS_HYGX                (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x00044)
#define IOMMU_CAP_BASE_HI_ADDRESS_HYGX                (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x00048)
#define IOMMU_CAP_VF_BASE_LO_ADDRESS_HYGX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x000D0)
#define IOMMU_CAP_VF_BASE_HI_ADDRESS_HYGX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x000D4)
#define IOMMU_CAP_VFCNTL_BASE_LO_ADDRESS_HYGX         (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x000D8)
#define IOMMU_CAP_VFCNTL_BASE_HI_ADDRESS_HYGX         (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x000DC)

// Type
#define IOMMU_CAP_BASE_LO_TYPE                       TYPE_SMN

#define IOMMU_CAP_BASE_LO_IOMMU_ENABLE_OFFSET        0
#define IOMMU_CAP_BASE_LO_IOMMU_ENABLE_WIDTH         1
#define IOMMU_CAP_BASE_LO_IOMMU_ENABLE_MASK          0x1
#define IOMMU_CAP_BASE_LO_IOMMU_BASE_ADDR_LO_OFFSET  19
#define IOMMU_CAP_BASE_LO_IOMMU_BASE_ADDR_LO_WIDTH   13
#define IOMMU_CAP_BASE_LO_IOMMU_BASE_ADDR_LO_MASK    0xfff80000L

/// SMN_00000044
typedef union {
  struct {
    UINT32    IOMMU_ENABLE       : 1;                                  ///<
    UINT32    Reserved_18_1      : 18;                                 ///<
    UINT32    IOMMU_BASE_ADDR_LO : 13;                                 ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOMMU_CAP_BASE_LO_STRUCT;

// Register Name IOMMU_CONTROL_W

// Address
#define IOMMU_CONTROL_W_ADDRESS_HYEX                 (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x0007c)
#define IOMMU_CONTROL_W_ADDRESS_HYGX                 (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x0007c)
// Type
#define IOMMU_CONTROL_W_TYPE                        TYPE_SMN

#define IOMMU_CONTROL_W_INTERRUPT_PIN_W_OFFSET      0
#define IOMMU_CONTROL_W_INTERRUPT_PIN_W_WIDTH       3
#define IOMMU_CONTROL_W_INTERRUPT_PIN_W_MASK        0x7
#define IOMMU_CONTROL_W_Reserved_3_3_OFFSET         3
#define IOMMU_CONTROL_W_Reserved_3_3_WIDTH          1
#define IOMMU_CONTROL_W_Reserved_3_3_MASK           0x8
#define IOMMU_CONTROL_W_MINOR_REV_ID_W_OFFSET       4
#define IOMMU_CONTROL_W_MINOR_REV_ID_W_WIDTH        4
#define IOMMU_CONTROL_W_MINOR_REV_ID_W_MASK         0xf0
#define IOMMU_CONTROL_W_IO_TLBSUP_W_OFFSET          8
#define IOMMU_CONTROL_W_IO_TLBSUP_W_WIDTH           1
#define IOMMU_CONTROL_W_IO_TLBSUP_W_MASK            0x100
#define IOMMU_CONTROL_W_EFR_SUP_W_OFFSET            9
#define IOMMU_CONTROL_W_EFR_SUP_W_WIDTH             1
#define IOMMU_CONTROL_W_EFR_SUP_W_MASK              0x200
#define IOMMU_CONTROL_W_MSI_MULT_MESS_CAP_W_OFFSET  10
#define IOMMU_CONTROL_W_MSI_MULT_MESS_CAP_W_WIDTH   3
#define IOMMU_CONTROL_W_MSI_MULT_MESS_CAP_W_MASK    0x1c00
#define IOMMU_CONTROL_W_IOMMU_CAP_EXT_W_OFFSET      13
#define IOMMU_CONTROL_W_IOMMU_CAP_EXT_W_WIDTH       1
#define IOMMU_CONTROL_W_IOMMU_CAP_EXT_W_MASK        0x2000
#define IOMMU_CONTROL_W_Reserved_31_14_OFFSET       14
#define IOMMU_CONTROL_W_Reserved_31_14_WIDTH        18
#define IOMMU_CONTROL_W_Reserved_31_14_MASK         0xffffc000L

/// IOMMU_CONTROL_W
typedef union {
  struct {
    UINT32    INTERRUPT_PIN_W     : 3;                                 ///<
    UINT32    Reserved_3_3        : 1;                                 ///<
    UINT32    MINOR_REV_ID_W      : 4;                                 ///<
    UINT32    IO_TLBSUP_W         : 1;                                 ///<
    UINT32    EFR_SUP_W           : 1;                                 ///<
    UINT32    MSI_MULT_MESS_CAP_W : 3;                                 ///<
    UINT32    IOMMU_CAP_EXT_W     : 1;                                 ///<
    UINT32    Reserved_31_14      : 18;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOMMU_CONTROL_W_STRUCT;

// svd_group: IOHC_instINTSBDEVINDCFG0_aliasSMN
// Group Base = (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x30000

// Register Name IOHC_Bridge_CNTL
//HyEx PCIE0 port A~H
#define NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x31004)
#define NB_DEVINDCFG1_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x31404)
#define NB_DEVINDCFG2_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x31804)
#define NB_DEVINDCFG3_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x31C04)
#define NB_DEVINDCFG4_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x32004)
#define NB_DEVINDCFG5_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x32404)
#define NB_DEVINDCFG6_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x32804)
#define NB_DEVINDCFG7_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x32C04)
//HyEx PCIE1 port A~H
#define NB_DEVINDCFG8_IOHC_Bridge_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x33004)
#define NB_DEVINDCFG9_IOHC_Bridge_CNTL_ADDRESS_HYEX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x33404)
#define NB_DEVINDCFG10_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x33804)
#define NB_DEVINDCFG11_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x33C04)
#define NB_DEVINDCFG12_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x34004)
#define NB_DEVINDCFG13_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x34404)
#define NB_DEVINDCFG14_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x34804)
#define NB_DEVINDCFG15_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x34C04)
//HyEx PCIE2 ports
#define NB_DEVINDCFG16_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x35004)
#define NB_DEVINDCFG17_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x35404)
// HyEx NBIF0
#define NB_DEVINDCFG18_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x38004)
// HyEx NBIF1
#define NB_DEVINDCFG19_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x39004)
#define NB_DEVINDCFG20_IOHC_Bridge_CNTL_ADDRESS_HYEX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x3C004)  

// HyGx PCIE Core 0 Port A~H
#define NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x31004)
#define NB_DEVINDCFG1_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x31404)
#define NB_DEVINDCFG2_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x31804)
#define NB_DEVINDCFG3_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x31C04)
#define NB_DEVINDCFG4_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x32004)
#define NB_DEVINDCFG5_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x32404)
#define NB_DEVINDCFG6_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x32804)
#define NB_DEVINDCFG7_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x32C04)
// HyGx PCIE Core 1 Port A~H
#define NB_DEVINDCFG8_IOHC_Bridge_CNTL_ADDRESS_HYGX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x33004)
#define NB_DEVINDCFG9_IOHC_Bridge_CNTL_ADDRESS_HYGX   (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x33404)
#define NB_DEVINDCFG10_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x33804)
#define NB_DEVINDCFG11_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x33C04)
#define NB_DEVINDCFG12_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x34004)
#define NB_DEVINDCFG13_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x34404)
#define NB_DEVINDCFG14_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x34804)
#define NB_DEVINDCFG15_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x34C04)
// HyGx PCIE Core 2 Port A~H
#define NB_DEVINDCFG16_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x35004)
#define NB_DEVINDCFG17_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x35404)
#define NB_DEVINDCFG18_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x35804)
#define NB_DEVINDCFG19_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x35C04)
#define NB_DEVINDCFG20_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x36004)
#define NB_DEVINDCFG21_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x36404)
#define NB_DEVINDCFG22_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x36804)
#define NB_DEVINDCFG23_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x36C04)
// HyGx NBIF0
#define NB_DEVINDCFG24_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x38004)
// HyGx NBIF2
#define NB_DEVINDCFG25_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x39004)
// HyGx NBIF3
#define NB_DEVINDCFG26_IOHC_Bridge_CNTL_ADDRESS_HYGX  (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x39404)

// DEVINDCFG Type
#define NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE      TYPE_SMN

#define IOHC_BRIDGE_CNTL_BridgeDis_OFFSET       0
#define IOHC_BRIDGE_CNTL_BridgeDis_WIDTH        1
#define IOHC_BRIDGE_CNTL_BridgeDis_MASK         0x1
#define IOHC_BRIDGE_CNTL_BusMasterDis_OFFSET    1
#define IOHC_BRIDGE_CNTL_BusMasterDis_WIDTH     1
#define IOHC_BRIDGE_CNTL_BusMasterDis_MASK      0x2
#define IOHC_BRIDGE_CNTL_CfgDis_OFFSET          2
#define IOHC_BRIDGE_CNTL_CfgDis_WIDTH           1
#define IOHC_BRIDGE_CNTL_CfgDis_MASK            0x4
#define IOHC_BRIDGE_CNTL_P2pDis_OFFSET          3
#define IOHC_BRIDGE_CNTL_P2pDis_WIDTH           1
#define IOHC_BRIDGE_CNTL_P2pDis_MASK            0x8
#define IOHC_BRIDGE_CNTL_VDMDis_OFFSET          5
#define IOHC_BRIDGE_CNTL_VDMDis_WIDTH           1
#define IOHC_BRIDGE_CNTL_VDMDis_MASK            0x20
#define IOHC_BRIDGE_CNTL_MaskUR_Enable_OFFSET   6
#define IOHC_BRIDGE_CNTL_MaskUR_Enable_WIDTH    1
#define IOHC_BRIDGE_CNTL_MaskUR_Enable_MASK     0x40
#define IOHC_BRIDGE_CNTL_PassPWDis_OFFSET       7
#define IOHC_BRIDGE_CNTL_PassPWDis_WIDTH        1
#define IOHC_BRIDGE_CNTL_PassPWDis_MASK         0x80
#define IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET      8
#define IOHC_BRIDGE_CNTL_NoSnoopDis_WIDTH       1
#define IOHC_BRIDGE_CNTL_NoSnoopDis_MASK        0x100
#define IOHC_BRIDGE_CNTL_ForceRspPassPw_OFFSET  9
#define IOHC_BRIDGE_CNTL_ForceRspPassPw_WIDTH   1
#define IOHC_BRIDGE_CNTL_ForceRspPassPw_MASK    0x200
#define IOHC_BRIDGE_CNTL_IDOMode_OFFSET         10
#define IOHC_BRIDGE_CNTL_IDOMode_WIDTH          2
#define IOHC_BRIDGE_CNTL_IDOMode_MASK           0xc00
#define IOHC_BRIDGE_CNTL_ExtDevPlug_OFFSET      16
#define IOHC_BRIDGE_CNTL_ExtDevPlug_WIDTH       1
#define IOHC_BRIDGE_CNTL_ExtDevPlug_MASK        0x10000
#define IOHC_BRIDGE_CNTL_ExtDevCrsEn_OFFSET     17
#define IOHC_BRIDGE_CNTL_ExtDevCrsEn_WIDTH      1
#define IOHC_BRIDGE_CNTL_ExtDevCrsEn_MASK       0x20000
#define IOHC_BRIDGE_CNTL_CrsEnable_OFFSET       18
#define IOHC_BRIDGE_CNTL_CrsEnable_WIDTH        1
#define IOHC_BRIDGE_CNTL_CrsEnable_MASK         0x40000
#define IOHC_BRIDGE_CNTL_APIC_Enable_OFFSET     23
#define IOHC_BRIDGE_CNTL_APIC_Enable_WIDTH      1
#define IOHC_BRIDGE_CNTL_APIC_Enable_MASK       0x800000
#define IOHC_BRIDGE_CNTL_APIC_Range_OFFSET      24
#define IOHC_BRIDGE_CNTL_APIC_Range_WIDTH       8
#define IOHC_BRIDGE_CNTL_APIC_Range_MASK        0xff000000L

typedef union {
  struct {
    UINT32    BridgeDis      : 1;                                      ///<
    UINT32    BusMasterDis   : 1;                                      ///<
    UINT32    CfgDis         : 1;                                      ///<
    UINT32    P2pDis         : 1;                                      ///<
    UINT32                   : 1;                                      ///<
    UINT32    VDMDis         : 1;                                      ///<
    UINT32    MaskUR_Enable  : 1;                                      ///<
    UINT32    PassPWDis      : 1;                                      ///<
    UINT32    NoSnoopDis     : 1;                                      ///<
    UINT32    ForceRspPassPw : 1;                                      ///<
    UINT32    IDOMode        : 2;                                      ///<
    UINT32                   : 4;                                      ///<
    UINT32    ExtDevPlug     : 1;                                      ///<
    UINT32    ExtDevCrsEn    : 1;                                      ///<
    UINT32    CrsEnable      : 1;                                      ///<
    UINT32                   : 4;                                      ///<
    UINT32    APIC_Enable    : 1;                                      ///<
    UINT32    APIC_Range     : 8;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOHC_Bridge_CNTL_STRUCT;

#define IOHC_FASTREG_APERTURE_ADDRESS_HYEX                     (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x07000)
#define IOHC_FASTREG_APERTURE_ADDRESS_HYGX                     (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x07000)
typedef union {
  struct {
    UINT32                               FASTREG_APERTURE_ID : 12;
    UINT32                                                   : 4;
    UINT32                                   FASTREG_NODE_ID : 4;
    UINT32                                                   : 11;
    UINT32                               FASTREG_TRAN_POSTED : 1;
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} IOHC_FASTREG_APERTURE_STRUCT;

#define IOHC_FASTREG_BASE_ADDR_LO_ADDRESS_HYEX                 (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10300)
#define IOHC_FASTREG_BASE_ADDR_LO_ADDRESS_HYGX                 (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10300)
typedef union {
  struct {
    UINT32                                   FASTREG_MMIO_EN : 1;
    UINT32                                 FASTREG_MMIO_LOCK : 1;
    UINT32                                                   : 18;
    UINT32                              FASTREG_BASE_ADDR_LO : 12;
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} IOHC_FASTREG_BASE_ADDR_LO_STRUCT;

#define IOHC_FASTREG_BASE_ADDR_HI_ADDRESS_HYEX                 (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10304)
#define IOHC_FASTREG_BASE_ADDR_HI_ADDRESS_HYGX                 (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10304)
typedef union {
  struct {
    UINT32                              FASTREG_BASE_ADDR_HI : 16;
    UINT32                                                   : 16;
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} IOHC_FASTREG_BASE_ADDR_HI_STRUCT;

// IOHUB::IOMMUL2::IOMMU_CAP_MISC_aliasSMN
// Address
#define IOMMU_CAP_MISC_ADDRESS_HYEX                            (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x00050)
#define IOMMU_CAP_MISC_ADDRESS_HYGX                            (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x00050)

// Type  
#define IOMMU_CAP_MISC_TYPE                      TYPE_SMN

#define IOMMU_CAP_MISC_IOMMU_MSI_NUM_OFFSET      0
#define IOMMU_CAP_MISC_IOMMU_MSI_NUM_WIDTH       5
#define IOMMU_CAP_MISC_IOMMU_MSI_NUM_MASK        0x1f
#define IOMMU_CAP_MISC_IOMMU_GVA_SIZE_OFFSET     5
#define IOMMU_CAP_MISC_IOMMU_GVA_SIZE_WIDTH      3
#define IOMMU_CAP_MISC_IOMMU_GVA_SIZE_MASK       0xe0
#define IOMMU_CAP_MISC_IOMMU_PA_SIZE_OFFSET      8
#define IOMMU_CAP_MISC_IOMMU_PA_SIZE_WIDTH       7
#define IOMMU_CAP_MISC_IOMMU_PA_SIZE_MASK        0x7f00
#define IOMMU_CAP_MISC_IOMMU_VA_SIZE_OFFSET      15
#define IOMMU_CAP_MISC_IOMMU_VA_SIZE_WIDTH       7
#define IOMMU_CAP_MISC_IOMMU_VA_SIZE_MASK        0x3f8000
#define IOMMU_CAP_MISC_IOMMU_HT_ATS_RESV_OFFSET  22
#define IOMMU_CAP_MISC_IOMMU_HT_ATS_RESV_WIDTH   1
#define IOMMU_CAP_MISC_IOMMU_HT_ATS_RESV_MASK    0x400000
#define IOMMU_CAP_MISC_Reserved1_OFFSET          23
#define IOMMU_CAP_MISC_Reserved1_WIDTH           4
#define IOMMU_CAP_MISC_Reserved1_MASK            0x7800000
#define IOMMU_CAP_MISC_IOMMU_MSI_NUM_PPR_OFFSET  27
#define IOMMU_CAP_MISC_IOMMU_MSI_NUM_PPR_WIDTH   5
#define IOMMU_CAP_MISC_IOMMU_MSI_NUM_PPR_MASK    0xf8000000L

/// SMN_00000050
typedef union {
  struct {
    UINT32    IOMMU_MSI_NUM     : 5;                                   ///<
    UINT32    IOMMU_GVA_SIZE    : 3;                                   ///<
    UINT32    IOMMU_PA_SIZE     : 7;                                   ///<
    UINT32    IOMMU_VA_SIZE     : 7;                                   ///<
    UINT32    IOMMU_HT_ATS_RESV : 1;                                   ///<
    UINT32    Reserved1         : 4;                                   ///<
    UINT32    IOMMU_MSI_NUM_PPR : 5;                                   ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOMMU_CAP_MISC_STRUCT;

// IOHUB::IOMMUL2INDX::L2_DEBUG_3_aliasSMN;
// Address
#define L2_DEBUG_3_ADDRESS_HYEX                             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x0111c) 
#define L2_DEBUG_3_ADDRESS_HYGX                             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x0111c)                                            // L2_MISC_CNTRL_3_ADDRESS

// Type
#define L2_DEBUG_3_TYPE                                    TYPE_SMN

#define L2_DEBUG_3_REG_ats_iw_OFFSET                       0
#define L2_DEBUG_3_REG_ats_iw_WIDTH                        1
#define L2_DEBUG_3_REG_ats_iw_MASK                         0x1
#define L2_DEBUG_3_REG_mask_l2_nw_OFFSET                   1
#define L2_DEBUG_3_REG_mask_l2_nw_WIDTH                    1
#define L2_DEBUG_3_REG_mask_l2_nw_MASK                     0x2
#define L2_DEBUG_3_REG_atomic_filter_en_OFFSET             2
#define L2_DEBUG_3_REG_atomic_filter_en_WIDTH              1
#define L2_DEBUG_3_REG_atomic_filter_en_MASK               0x4
#define L2_DEBUG_3_REG_l1wq_id_el_en_OFFSET                3
#define L2_DEBUG_3_REG_l1wq_id_el_en_WIDTH                 1
#define L2_DEBUG_3_REG_l1wq_id_el_en_MASK                  0x8
#define L2_DEBUG_3_REG_PPR_StrictOrder_En_OFFSET           4
#define L2_DEBUG_3_REG_PPR_StrictOrder_En_WIDTH            1
#define L2_DEBUG_3_REG_PPR_StrictOrder_En_MASK             0x10
#define L2_DEBUG_3_REG_vIOMMU_IntFilter_En_OFFSET          5
#define L2_DEBUG_3_REG_vIOMMU_IntFilter_En_WIDTH           1
#define L2_DEBUG_3_REG_vIOMMU_IntFilter_En_MASK            0x20
#define L2_DEBUG_3_REG_pagemode0_fix_dis_OFFSET            6
#define L2_DEBUG_3_REG_pagemode0_fix_dis_WIDTH             1
#define L2_DEBUG_3_REG_pagemode0_fix_dis_MASK              0x40
#define L2_DEBUG_3_REG_LogATS_iriw_zero_OFFSET             7
#define L2_DEBUG_3_REG_LogATS_iriw_zero_WIDTH              1
#define L2_DEBUG_3_REG_LogATS_iriw_zero_MASK               0x80
#define L2_DEBUG_3_REG_PTC_Update_AddrTransReq_OFFSET      8
#define L2_DEBUG_3_REG_PTC_Update_AddrTransReq_WIDTH       1
#define L2_DEBUG_3_REG_PTC_Update_AddrTransReq_MASK        0x100
#define L2_DEBUG_3_REG_RstPtrs_on_BaseHiAcc_Dis_OFFSET     9
#define L2_DEBUG_3_REG_RstPtrs_on_BaseHiAcc_Dis_WIDTH      1
#define L2_DEBUG_3_REG_RstPtrs_on_BaseHiAcc_Dis_MASK       0x200
#define L2_DEBUG_3_REG_RstPtrs_on_BaseLoAcc_Dis_OFFSET     10
#define L2_DEBUG_3_REG_RstPtrs_on_BaseLoAcc_Dis_WIDTH      1
#define L2_DEBUG_3_REG_RstPtrs_on_BaseLoAcc_Dis_MASK       0x400
#define L2_DEBUG_3_REG_RstPtrs_on_LenAccs_En_OFFSET        11
#define L2_DEBUG_3_REG_RstPtrs_on_LenAccs_En_WIDTH         1
#define L2_DEBUG_3_REG_RstPtrs_on_LenAccs_En_MASK          0x800
#define L2_DEBUG_3_REG_DTEResvBitChkDis_OFFSET             12
#define L2_DEBUG_3_REG_DTEResvBitChkDis_WIDTH              1
#define L2_DEBUG_3_REG_DTEResvBitChkDis_MASK               0x1000
#define L2_DEBUG_3_REG_RstGVAPtrs_on_BaseHiAcc_Dis_OFFSET  13
#define L2_DEBUG_3_REG_RstGVAPtrs_on_BaseHiAcc_Dis_WIDTH   1
#define L2_DEBUG_3_REG_RstGVAPtrs_on_BaseHiAcc_Dis_MASK    0x2000
#define L2_DEBUG_3_REG_RstGVAPtrs_on_BaseLoAcc_Dis_OFFSET  14
#define L2_DEBUG_3_REG_RstGVAPtrs_on_BaseLoAcc_Dis_WIDTH   1
#define L2_DEBUG_3_REG_RstGVAPtrs_on_BaseLoAcc_Dis_MASK    0x4000
#define L2_DEBUG_3_REG_RstGVAPtrs_on_LenAccs_En_OFFSET     15
#define L2_DEBUG_3_REG_RstGVAPtrs_on_LenAccs_En_WIDTH      1
#define L2_DEBUG_3_REG_RstGVAPtrs_on_LenAccs_En_MASK       0x8000
#define L2_DEBUG_3_RESERVED_OFFSET                         16
#define L2_DEBUG_3_RESERVED_WIDTH                          15
#define L2_DEBUG_3_RESERVED_MASK                           0x7fff0000
#define L2_DEBUG_3_REG_gmc_iommu_dis_OFFSET                31
#define L2_DEBUG_3_REG_gmc_iommu_dis_WIDTH                 1
#define L2_DEBUG_3_REG_gmc_iommu_dis_MASK                  0x80000000L

/// SMN_0000011C
typedef union {
  struct {
    UINT32    REG_ats_iw                      : 1;                     ///<
    UINT32    REG_mask_l2_nw                  : 1;                     ///<
    UINT32    REG_atomic_filter_en            : 1;                     ///<
    UINT32    REG_l1wq_id_el_en               : 1;                     ///<
    UINT32    REG_PPR_StrictOrder_En          : 1;                     ///< Only HyGx support
    UINT32    REG_vIOMMU_IntFilter_En         : 1;                     ///< Only HyGx support
    UINT32    REG_pagemode0_fix_dis           : 1;                     ///<
    UINT32    REG_LogATS_iriw_zero            : 1;                     ///<
    UINT32    REG_PTC_Update_AddrTransReq     : 1;                     ///<
    UINT32    REG_RstPtrs_on_BaseHiAcc_Dis    : 1;                     ///<
    UINT32    REG_RstPtrs_on_BaseLoAcc_Dis    : 1;                     ///<
    UINT32    REG_RstPtrs_on_LenAccs_En       : 1;                     ///<
    UINT32    REG_DTEResvBitChkDis            : 1;                     ///<
    UINT32    REG_RstGVAPtrs_on_BaseHiAcc_Dis : 1;                     ///<
    UINT32    REG_RstGVAPtrs_on_BaseLoAcc_Dis : 1;                     ///<
    UINT32    REG_RstGVAPtrs_on_LenAccs_En    : 1;                     ///<
    UINT32    RESERVED                        : 15;                    ///<
    UINT32    REG_gmc_iommu_dis               : 1;                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2_DEBUG_3_STRUCT;

// Address
#define L2_SB_LOCATION_ADDRESS_HYEX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x0112c)
#define L2_SB_LOCATION_ADDRESS_HYGX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x0112c)

// Type
#define L2_SB_LOCATION_TYPE                    TYPE_SMN

#define IOMMUL2INDX_12C_SBlocated_Port_OFFSET  0
#define IOMMUL2INDX_12C_SBlocated_Port_WIDTH   16
#define IOMMUL2INDX_12C_SBlocated_Port_MASK    0xffff
#define IOMMUL2INDX_12C_SBlocated_Core_OFFSET  16
#define IOMMUL2INDX_12C_SBlocated_Core_WIDTH   16
#define IOMMUL2INDX_12C_SBlocated_Core_MASK    0xffff0000L

/// IOMMUL2INDX_12C
typedef union {
  struct {
    UINT32    SBlocated_Port : 16;                                     ///<
    UINT32    SBlocated_Core : 16;                                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOMMUL2INDX_12C_STRUCT;

// IOHUB::IOMMUL2INDX::L2_CONTROL_5_aliasSMN
// Address
#define L2_CONTROL_5_ADDRESS_HYEX                      (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x01130)
#define L2_CONTROL_5_ADDRESS_HYGX                      (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x01130)

// Type
#define L2_CONTROL_5_TYPE                             TYPE_SMN

#define L2_CONTROL_5_QueueArbFBPri_OFFSET             0
#define L2_CONTROL_5_QueueArbFBPri_WIDTH              1
#define L2_CONTROL_5_QueueArbFBPri_MASK               0x1
#define L2_CONTROL_5_FC1Dis_OFFSET                    2
#define L2_CONTROL_5_FC1Dis_WIDTH                     1
#define L2_CONTROL_5_FC1Dis_MASK                      0x4
#define L2_CONTROL_5_DTCUpdateVOneIVZero_OFFSET       3
#define L2_CONTROL_5_DTCUpdateVOneIVZero_WIDTH        1
#define L2_CONTROL_5_DTCUpdateVOneIVZero_MASK         0x8
#define L2_CONTROL_5_DTCUpdateVZeroIVOne_OFFSET       4
#define L2_CONTROL_5_DTCUpdateVZeroIVOne_WIDTH        1
#define L2_CONTROL_5_DTCUpdateVZeroIVOne_MASK         0x10
#define L2_CONTROL_5_FC3Dis_OFFSET                    6
#define L2_CONTROL_5_FC3Dis_WIDTH                     1
#define L2_CONTROL_5_FC3Dis_MASK                      0x40
#define L2_CONTROL_5_Reserved_10_7_OFFSET             7
#define L2_CONTROL_5_Reserved_10_7_WIDTH              4
#define L2_CONTROL_5_Reserved_10_7_MASK               0x780
#define L2_CONTROL_5_ForceTWonVC7_OFFSET              11
#define L2_CONTROL_5_ForceTWonVC7_WIDTH               1
#define L2_CONTROL_5_ForceTWonVC7_MASK                0x800
#define L2_CONTROL_5_GST_partial_ptc_cntrl_OFFSET     12
#define L2_CONTROL_5_GST_partial_ptc_cntrl_WIDTH      7
#define L2_CONTROL_5_GST_partial_ptc_cntrl_MASK       0x7f000
#define L2_CONTROL_5_PCTRL_hysteresis_OFFSET          19
#define L2_CONTROL_5_PCTRL_hysteresis_WIDTH           6
#define L2_CONTROL_5_PCTRL_hysteresis_MASK            0x1f80000
#define L2_CONTROL_5_DTCUpdatePri_OFFSET              25
#define L2_CONTROL_5_DTCUpdatePri_WIDTH               1
#define L2_CONTROL_5_DTCUpdatePri_MASK                0x2000000
#define L2_CONTROL_5_L2B_L2A_v1_trans_credits_OFFSET  26
#define L2_CONTROL_5_L2B_L2A_v1_trans_credits_WIDTH   6
#define L2_CONTROL_5_L2B_L2A_v1_trans_credits_MASK    0xfc000000

/// SMN_00000130
typedef union {
  struct {
    UINT32    QueueArbFBPri            : 1;                            ///<
    UINT32    Reserved_1_1             : 1;                            ///<
    UINT32    FC1Dis                   : 1;                            ///<
    UINT32    DTCUpdateVOneIVZero      : 1;                            ///<
    UINT32    DTCUpdateVZeroIVOne      : 1;                            ///<
    UINT32    Reserved_5_5             : 1;                            ///<
    UINT32    FC3Dis                   : 1;                            ///<
    UINT32    Reserved_10_7            : 4;                            ///<
    UINT32    ForceTWonVC7             : 1;                            ///< Only HyGx support
    UINT32    GST_partial_ptc_cntrl    : 7;                            ///<
    UINT32    PCTRL_hysteresis         : 6;                            ///<
    UINT32    DTCUpdatePri             : 1;                            ///<
    UINT32    L2B_L2A_v1_trans_credits : 6;                            ///< Only HyGx support
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2_CONTROL_5_STRUCT;

// IOHUB::IOMMUL2INDX::L2B_UPDATE_FILTER_CNTL_aliasSMN;
// Address
#define L2B_UPDATE_FILTER_CNTL_ADDRESS_HYEX                        (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x0114c)
#define L2B_UPDATE_FILTER_CNTL_ADDRESS_HYGX                        (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x0114c)

// Type
#define L2B_UPDATE_FILTER_CNTL_TYPE                               TYPE_SMN

#define L2B_UPDATE_FILTER_CNTL_L2b_Update_Filter_Bypass_OFFSET     0
#define L2B_UPDATE_FILTER_CNTL_L2b_Update_Filter_Bypass_WIDTH      1
#define L2B_UPDATE_FILTER_CNTL_L2b_Update_Filter_Bypass_MASK       0x1
#define L2B_UPDATE_FILTER_CNTL_L2b_Update_Filter_RdLatency_OFFSET  1
#define L2B_UPDATE_FILTER_CNTL_L2b_Update_Filter_RdLatency_WIDTH   4
#define L2B_UPDATE_FILTER_CNTL_L2b_Update_Filter_RdLatency_MASK    0x1e

/// SMN_0000014C
typedef union {
  struct {
    UINT32    L2b_Update_Filter_Bypass    : 1;                         ///<
    UINT32    L2b_Update_Filter_RdLatency : 4;                         ///<
    UINT32    Reserved_31_5               : 27;                        ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2B_UPDATE_FILTER_CNTL_STRUCT;

// IOHUB::IOMMUL2INDX::L2_TW_CONTROL_aliasSMN
// Address
#define L2_TW_CONTROL_ADDRESS_HYEX                 (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x01150)
#define L2_TW_CONTROL_ADDRESS_HYGX                 (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x01150)

// Type
#define L2_TW_CONTROL_TYPE                         TYPE_SMN

#define L2_TW_CONTROL_RESERVED_OFFSET              0
#define L2_TW_CONTROL_RESERVED_WIDTH               6
#define L2_TW_CONTROL_RESERVED_MASK                0x3f
#define L2_TW_CONTROL_TWForceCoherent_OFFSET       6
#define L2_TW_CONTROL_TWForceCoherent_WIDTH        1
#define L2_TW_CONTROL_TWForceCoherent_MASK         0x40
#define L2_TW_CONTROL_TWPrefetchEn_OFFSET          8
#define L2_TW_CONTROL_TWPrefetchEn_WIDTH           1
#define L2_TW_CONTROL_TWPrefetchEn_MASK            0x100
#define L2_TW_CONTROL_TWPrefetchOnly4KDis_OFFSET   9
#define L2_TW_CONTROL_TWPrefetchOnly4KDis_WIDTH    1
#define L2_TW_CONTROL_TWPrefetchOnly4KDis_MASK     0x200
#define L2_TW_CONTROL_TWPTEOnUntransExcl_OFFSET    10
#define L2_TW_CONTROL_TWPTEOnUntransExcl_WIDTH     1
#define L2_TW_CONTROL_TWPTEOnUntransExcl_MASK      0x400
#define L2_TW_CONTROL_TWPTEOnAddrTransExcl_OFFSET  11
#define L2_TW_CONTROL_TWPTEOnAddrTransExcl_WIDTH   1
#define L2_TW_CONTROL_TWPTEOnAddrTransExcl_MASK    0x800
#define L2_TW_CONTROL_TWPrefetchRange_OFFSET       12
#define L2_TW_CONTROL_TWPrefetchRange_WIDTH        3
#define L2_TW_CONTROL_TWPrefetchRange_MASK         0x7000
#define L2_TW_CONTROL_TWFilter_Dis_OFFSET          16
#define L2_TW_CONTROL_TWFilter_Dis_WIDTH           1
#define L2_TW_CONTROL_TWFilter_Dis_MASK            0x10000
#define L2_TW_CONTROL_TWFilter_64B_Dis_OFFSET      17
#define L2_TW_CONTROL_TWFilter_64B_Dis_WIDTH       1
#define L2_TW_CONTROL_TWFilter_64B_Dis_MASK        0x20000
#define L2_TW_CONTROL_TWContWalkOnPErrDis_OFFSET   18
#define L2_TW_CONTROL_TWContWalkOnPErrDis_WIDTH    1
#define L2_TW_CONTROL_TWContWalkOnPErrDis_MASK     0x40000
#define L2_TW_CONTROL_TWSetAccessBit_Dis_OFFSET    19
#define L2_TW_CONTROL_TWSetAccessBit_Dis_WIDTH     1
#define L2_TW_CONTROL_TWSetAccessBit_Dis_MASK      0x80000
#define L2_TW_CONTROL_TWClearAPBit_Dis_OFFSET      20
#define L2_TW_CONTROL_TWClearAPBit_Dis_WIDTH       1
#define L2_TW_CONTROL_TWClearAPBit_Dis_MASK        0x100000
#define L2_TW_CONTROL_TWGuestPrefetchEn_OFFSET     21
#define L2_TW_CONTROL_TWGuestPrefetchEn_WIDTH      1
#define L2_TW_CONTROL_TWGuestPrefetchEn_MASK       0x200000
#define L2_TW_CONTROL_TWGuestPrefetchRange_OFFSET  22
#define L2_TW_CONTROL_TWGuestPrefetchRange_WIDTH   3
#define L2_TW_CONTROL_TWGuestPrefetchRange_MASK    0x1C00000
#define L2_TW_CONTROL_TWCacheNestedPTE_OFFSET      25
#define L2_TW_CONTROL_TWCacheNestedPTE_WIDTH       1
#define L2_TW_CONTROL_TWCacheNestedPTE_MASK        0x2000000

/// SMN_00000150
typedef union {
  struct {
    UINT32    RESERVED             : 6;                                ///<
    UINT32    TWForceCoherent      : 1;                                ///<
    UINT32    Reserved_7_7         : 1;                                ///<
    UINT32    TWPrefetchEn         : 1;                                ///<
    UINT32    TWPrefetchOnly4KDis  : 1;                                ///<
    UINT32    TWPTEOnUntransExcl   : 1;                                ///<
    UINT32    TWPTEOnAddrTransExcl : 1;                                ///<
    UINT32    TWPrefetchRange      : 3;                                ///<
    UINT32    Reserved_15_15       : 1;                                ///<
    UINT32    TWFilter_Dis         : 1;                                ///<
    UINT32    TWFilter_64B_Dis     : 1;                                ///<
    UINT32    TWContWalkOnPErrDis  : 1;                                ///<
    UINT32    TWSetAccessBit_Dis   : 1;                                ///<
    UINT32    TWClearAPBit_Dis     : 1;                                ///<
    UINT32    TWGuestPrefetchEn    : 1;                                ///<
    UINT32    TWGuestPrefetchRange : 3;                                ///<
    UINT32    TWCacheNestedPTE     : 1;                                ///< Only HyGx support
    UINT32    Reserved_31_26       : 6;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2_TW_CONTROL_STRUCT;

// IOHUB::IOMMUL2INDX::L2_CP_CONTROL_aliasSMN
// Address
#define L2_CP_CONTROL_ADDRESS_HYEX           (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x01158)
#define L2_CP_CONTROL_ADDRESS_HYGX           (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x01158)

// Type
#define L2_CP_CONTROL_TYPE                  TYPE_SMN

#define L2_CP_CONTROL_CPPrefetchDis_OFFSET  0
#define L2_CP_CONTROL_CPPrefetchDis_WIDTH   1
#define L2_CP_CONTROL_CPPrefetchDis_MASK    0x1
#define L2_CP_CONTROL_CPFlushOnWait_OFFSET  1
#define L2_CP_CONTROL_CPFlushOnWait_WIDTH   1
#define L2_CP_CONTROL_CPFlushOnWait_MASK    0x2
#define L2_CP_CONTROL_CPFlushOnInv_OFFSET   2
#define L2_CP_CONTROL_CPFlushOnInv_WIDTH    1
#define L2_CP_CONTROL_CPFlushOnInv_MASK     0x4
#define L2_CP_CONTROL_CPRdDelay_OFFSET      16
#define L2_CP_CONTROL_CPRdDelay_WIDTH       16
#define L2_CP_CONTROL_CPRdDelay_MASK        0xffff0000L

/// SMN_00000158
typedef union {
  struct {
    UINT32    CPPrefetchDis : 1;                                       ///<
    UINT32    CPFlushOnWait : 1;                                       ///<
    UINT32    CPFlushOnInv  : 1;                                       ///<
    UINT32    Reserved_15_3 : 13;                                      ///<
    UINT32    CPRdDelay     : 16;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2_CP_CONTROL_STRUCT;

// IOHUB::IOMMUL2INDX::L2_ERR_RULE_CONTROL_0_aliasSMN
// Address
#define L2_ERR_RULE_CONTROL_0_ADDRESS_HYEX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x01200)
#define L2_ERR_RULE_CONTROL_0_ADDRESS_HYGX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x01200)

// Type
#define L2_ERR_RULE_CONTROL_0_TYPE                    TYPE_SMN

#define L2_ERR_RULE_CONTROL_0_ERRRuleLock0_OFFSET     0
#define L2_ERR_RULE_CONTROL_0_ERRRuleLock0_WIDTH      1
#define L2_ERR_RULE_CONTROL_0_ERRRuleLock0_MASK       0x1
#define L2_ERR_RULE_CONTROL_0_ERRRuleDisable0_OFFSET  4
#define L2_ERR_RULE_CONTROL_0_ERRRuleDisable0_WIDTH   28
#define L2_ERR_RULE_CONTROL_0_ERRRuleDisable0_MASK    0xfffffff0L

/// SMN_00000200
typedef union {
  struct {
    UINT32    ERRRuleLock0    : 1;                                     ///<
    UINT32    Reserved_3_1    : 3;                                     ///<
    UINT32    ERRRuleDisable0 : 28;                                    ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2_ERR_RULE_CONTROL_0_STRUCT;

// IOHUB::IOMMUL2INDX::L2_L2B_CK_GATE_CONTROL_aliasSMN
// Address
#define L2_L2B_CK_GATE_CONTROL_ADDRESS_HYEX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x01240)
#define L2_L2B_CK_GATE_CONTROL_ADDRESS_HYGX             (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x01240)

// Type
#define L2_L2B_CK_GATE_CONTROL_TYPE                            TYPE_SMN

#define L2_L2B_CK_GATE_CONTROL_CKGateL2BRegsDisable_OFFSET     0
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BRegsDisable_WIDTH      1
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BRegsDisable_MASK       0x1
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BDynamicDisable_OFFSET  1
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BDynamicDisable_WIDTH   1
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BDynamicDisable_MASK    0x2
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BMiscDisable_OFFSET     2
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BMiscDisable_WIDTH      1
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BMiscDisable_MASK       0x4
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BCacheDisable_OFFSET    3
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BCacheDisable_WIDTH     1
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BCacheDisable_MASK      0x8
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BLength_OFFSET          4
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BLength_WIDTH           2
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BLength_MASK            0x30
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BStop_OFFSET            6
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BStop_WIDTH             2
#define L2_L2B_CK_GATE_CONTROL_CKGateL2BStop_MASK              0xc0
#define L2_L2B_CK_GATE_CONTROL_Reserved_31_8_OFFSET            8
#define L2_L2B_CK_GATE_CONTROL_Reserved_31_8_WIDTH             24
#define L2_L2B_CK_GATE_CONTROL_Reserved_31_8_MASK              0xffffff00L

/// SMN_00000240
typedef union {
  struct {
    UINT32    CKGateL2BRegsDisable    : 1;                             ///<
    UINT32    CKGateL2BDynamicDisable : 1;                             ///<
    UINT32    CKGateL2BMiscDisable    : 1;                             ///<
    UINT32    CKGateL2BCacheDisable   : 1;                             ///<
    UINT32    CKGateL2BLength         : 2;                             ///<
    UINT32    CKGateL2BStop           : 2;                             ///<
    UINT32    Reserved_31_8           : 24;                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2_L2B_CK_GATE_CONTROL_STRUCT;

// IOHUB::IOMMUL2INDX::L2_L2B_PGSIZE_CONTROL_aliasSMN
// Address
#define L2_PDC_CONTROL_ADDRESS_HYEX                       (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x01140)
#define L2_L2B_PGSIZE_CONTROL_ADDRESS_HYEX                (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x01250)
#define L2B_SDP_PARITY_ERROR_EN_ADDRESS_HYEX              (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x01288)

#define L2_PDC_CONTROL_ADDRESS_HYGX                       (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x01140)
#define L2_L2B_PGSIZE_CONTROL_ADDRESS_HYGX                (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x01250)
#define L2B_SDP_PARITY_ERROR_EN_ADDRESS_HYGX              (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x01288)

// Type
#define L2_L2B_PGSIZE_CONTROL_TYPE                       TYPE_SMN

#define L2_L2B_PGSIZE_CONTROL_L2BREG_GST_PGSIZE_OFFSET   0
#define L2_L2B_PGSIZE_CONTROL_L2BREG_GST_PGSIZE_WIDTH    7
#define L2_L2B_PGSIZE_CONTROL_L2BREG_GST_PGSIZE_MASK     0x7f
#define L2_L2B_PGSIZE_CONTROL_L2BREG_HOST_PGSIZE_OFFSET  8
#define L2_L2B_PGSIZE_CONTROL_L2BREG_HOST_PGSIZE_WIDTH   7
#define L2_L2B_PGSIZE_CONTROL_L2BREG_HOST_PGSIZE_MASK    0x7f00

/// SMN_00000250
typedef union {
  struct {
    UINT32    L2BREG_GST_PGSIZE  : 7;                                  ///<
    UINT32    Reserved_7_7       : 1;                                  ///<
    UINT32    L2BREG_HOST_PGSIZE : 7;                                  ///<
    UINT32    Reserved_31_15     : 17;                                 ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2_L2B_PGSIZE_CONTROL_STRUCT;

// svd_group: NBIO_IOHUB_IOAPIC_aliasSMN
// Address
#define IOAPIC_FEATURES_ENABLE_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00000)
#define IOAPIC_BASE_ADDR_HI_REG_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x102F4)
#define IOAPIC_BASE_ADDR_LOW_REG_HYEX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x102F0)
#define IOAPIC_ID_REG_HYEX                      0x02801000

#define IOAPIC_FEATURES_ENABLE_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00000)
#define IOAPIC_BASE_ADDR_HI_REG_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x102F4)
#define IOAPIC_BASE_ADDR_LOW_REG_HYGX           (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x102F0)
#define IOAPIC_ID_REG_HYGX                      0x05C01000

//SMN Registers
#define MP0_BASE_ADDR_HI_REG_HYEX               (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x102E4)
#define MP0_BASE_ADDR_LOW_REG_HYEX              (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x102E0)

#define MP0_BASE_ADDR_HI_REG_HYGX               (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x102E4)
#define MP0_BASE_ADDR_LOW_REG_HYGX              (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x102E0)

#define MP8_BASE_ADDR_HI_REG_HYGX               (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10328)
#define MP8_BASE_ADDR_LOW_REG_HYGX              (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x1032C)

// Type
#define FEATURES_ENABLE_TYPE                                   TYPE_SMN

#define NBIOAPICCFG_000_Ioapic_id_ext_en_OFFSET                2
#define NBIOAPICCFG_000_Ioapic_id_ext_en_WIDTH                 1
#define NBIOAPICCFG_000_Ioapic_id_ext_en_MASK                  0x4
#define NBIOAPICCFG_000_Ioapic_sb_feature_en_OFFSET            4
#define NBIOAPICCFG_000_Ioapic_sb_feature_en_WIDTH             1
#define NBIOAPICCFG_000_Ioapic_sb_feature_en_MASK              0x10
#define NBIOAPICCFG_000_Ioapic_secondary_en_OFFSET             5
#define NBIOAPICCFG_000_Ioapic_secondary_en_WIDTH              1
#define NBIOAPICCFG_000_Ioapic_secondary_en_MASK               0x20
#define NBIOAPICCFG_000_Ioapic_processor_mode_OFFSET           8
#define NBIOAPICCFG_000_Ioapic_processor_mode_WIDTH            1
#define NBIOAPICCFG_000_Ioapic_processor_mode_MASK             0x100
#define NBIOAPICCFG_000_INTx_LevelOnlyMode_OFFSET              9
#define NBIOAPICCFG_000_INTx_LevelOnlyMode_WIDTH               1
#define NBIOAPICCFG_000_INTx_LevelOnlyMode_MASK                0x200
#define NBIOAPICCFG_000_RCEC_Intr_map_OFFSET                   26
#define NBIOAPICCFG_000_RCEC_Intr_map_WIDTH                    5
#define NBIOAPICCFG_000_RCEC_Intr_map_MASK                     0x7c000000
#define NBIOAPICCFG_000_Linkdown_clear_int_assert_mode_OFFSET  31
#define NBIOAPICCFG_000_Linkdown_clear_int_assert_mode_WIDTH   1
#define NBIOAPICCFG_000_Linkdown_clear_int_assert_mode_MASK    0x80000000
/// NBIOAPICCFG_000
typedef union {
  struct {
    UINT32    Reserved_1_0                   : 2;                      ///<
    UINT32    Ioapic_id_ext_en               : 1;                      ///<
    UINT32    Reserved_3_3                   : 1;                      ///<
    UINT32    Ioapic_sb_feature_en           : 1;                      ///<
    UINT32    Ioapic_secondary_en            : 1;                      ///<
    UINT32    Reserved_7_6                   : 2;                      ///<
    UINT32    Ioapic_processor_mode          : 1;                      ///<
    UINT32    INTx_LevelOnlyMode             : 1;                      ///<
    UINT32    Reserved_25_10                 : 16;                     ///<
    UINT32    RCEC_Intr_map                  : 5;                      ///< Only HyGx support
    UINT32    Linkdown_clear_int_assert_mode : 1;                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIOAPICCFG_000_STRUCT;

// Address
#define IOAPIC_BR0_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00040)
#define IOAPIC_BR1_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00044)
#define IOAPIC_BR2_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00048)
#define IOAPIC_BR3_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x0004c)
#define IOAPIC_BR4_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00050)
#define IOAPIC_BR5_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00054)
#define IOAPIC_BR6_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00058)
#define IOAPIC_BR7_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x0005c)
#define IOAPIC_BR8_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00060)
#define IOAPIC_BR9_INTERRUPT_ROUTING_ADDRESS_HYEX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00064)
#define IOAPIC_BR10_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00068)
#define IOAPIC_BR11_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x0006c)
#define IOAPIC_BR12_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00070)
#define IOAPIC_BR13_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00074)
#define IOAPIC_BR14_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00078)
#define IOAPIC_BR15_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x0007c)
#define IOAPIC_BR16_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00080)
#define IOAPIC_BR17_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00084)
#define IOAPIC_BR18_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x00088)
#define IOAPIC_BR19_INTERRUPT_ROUTING_ADDRESS_HYEX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYEX << 20) + 0x0008C)

#define IOAPIC_BR0_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00040)
#define IOAPIC_BR1_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00044)
#define IOAPIC_BR2_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00048)
#define IOAPIC_BR3_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x0004c)
#define IOAPIC_BR4_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00050)
#define IOAPIC_BR5_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00054)
#define IOAPIC_BR6_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00058)
#define IOAPIC_BR7_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x0005c)
#define IOAPIC_BR8_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00060)
#define IOAPIC_BR9_INTERRUPT_ROUTING_ADDRESS_HYGX      (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00064)
#define IOAPIC_BR10_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00068)
#define IOAPIC_BR11_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x0006c)
#define IOAPIC_BR12_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00070)
#define IOAPIC_BR13_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00074)
#define IOAPIC_BR14_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00078)
#define IOAPIC_BR15_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x0007c)
#define IOAPIC_BR16_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00080)
#define IOAPIC_BR17_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00084)
#define IOAPIC_BR18_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00088)
#define IOAPIC_BR19_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x0008C)
#define IOAPIC_BR20_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00090)
#define IOAPIC_BR21_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00094)
#define IOAPIC_BR22_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x00098)
#define IOAPIC_BR23_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x0009C)
#define IOAPIC_BR24_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x000A0)
#define IOAPIC_BR25_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x000A4)
#define IOAPIC_BR26_INTERRUPT_ROUTING_ADDRESS_HYGX     (UINT32)((NBIO0_IOAPIC_APERTURE_ID_HYGX << 20) + 0x000A8)

// Type
#define IOAPIC_BR_INTERRUPT_ROUTING_TYPE              TYPE_SMN

#define IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET    0
#define IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_WIDTH     3
#define IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK      0x7
#define IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET    4
#define IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_WIDTH     2
#define IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK      0x30
#define IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET    16
#define IOAPIC_BR_INTERRUPT_Br_int_Intr_map_WIDTH     5
#define IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK      0x1f0000

/// IOAPIC_BR_INTERRUPT_ROUTING
typedef union {
  struct {
    UINT32    Br_ext_Intr_grp : 3;                                    ///<
    UINT32    Reserved0       : 1;                                    ///<
    UINT32    Br_ext_Intr_swz : 2;                                    ///<
    UINT32    Reserved1       : 10;                                   ///<
    UINT32    Br_int_Intr_map : 5;                                    ///<
    UINT32    Reserved2       : 11;                                   ///<
  } Field;                                                            ///<
  UINT32    Value;                                                    ///<
} IOAPIC_BR_INTERRUPT_ROUTING_STRUCT;


// Register Name L1_MISC_CNTRL_1
// Address
// Address
#define L1_MISC_CNTRL_1_PCIE0_ADDRESS_HYEX                      (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX << 20) + 0x0001c)
#define L1_MISC_CNTRL_1_PCIE1_ADDRESS_HYEX                      (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX << 20) + 0x0001c)
#define L1_MISC_CNTRL_1_NBIF0_ADDRESS_HYEX                      (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX << 20) + 0x0001c)
#define L1_MISC_CNTRL_1_IOAGR_ADDRESS_HYEX                      (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX << 20) + 0x0001c)

#define L1_MISC_CNTRL_1_PCIE0_ADDRESS_HYGX                      (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX << 20) + 0x0001c)
#define L1_MISC_CNTRL_1_PCIE1_ADDRESS_HYGX                      (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYGX << 20) + 0x0001c)
#define L1_MISC_CNTRL_1_NBIF0_ADDRESS_HYGX                      (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYGX << 20) + 0x0001c)
#define L1_MISC_CNTRL_1_IOAGR_ADDRESS_HYGX                      (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYGX << 20) + 0x0001c)

// Type
#define L1_MISC_CNTRL_1_TYPE                                   TYPE_SMN

#define L1_MISC_CNTRL_1_REG_force_OrderStreamID_func_OFFSET    0
#define L1_MISC_CNTRL_1_REG_force_OrderStreamID_func_WIDTH     1
#define L1_MISC_CNTRL_1_REG_force_OrderStreamID_func_MASK      0x1
#define L1_MISC_CNTRL_1_REG_ForceBypass_OFFSET                 1
#define L1_MISC_CNTRL_1_REG_ForceBypass_WIDTH                  1
#define L1_MISC_CNTRL_1_REG_ForceBypass_MASK                   0x2
#define L1_MISC_CNTRL_1_REG_cmdwait_wait_emptwq_OFFSET         2
#define L1_MISC_CNTRL_1_REG_cmdwait_wait_emptwq_WIDTH          1
#define L1_MISC_CNTRL_1_REG_cmdwait_wait_emptwq_MASK           0x4
#define L1_MISC_CNTRL_1_Reserved_3_3_OFFSET                    3
#define L1_MISC_CNTRL_1_Reserved_3_3_WIDTH                     1
#define L1_MISC_CNTRL_1_Reserved_3_3_MASK                      0x8
#define L1_MISC_CNTRL_1_REG_inv_wait_emptwq_OFFSET             4
#define L1_MISC_CNTRL_1_REG_inv_wait_emptwq_WIDTH              1
#define L1_MISC_CNTRL_1_REG_inv_wait_emptwq_MASK               0x10
#define L1_MISC_CNTRL_1_Reserved_5_5_OFFSET                    5
#define L1_MISC_CNTRL_1_Reserved_5_5_WIDTH                     1
#define L1_MISC_CNTRL_1_Reserved_5_5_MASK                      0x20
#define L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_OFFSET    6
#define L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_WIDTH     1
#define L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_MASK      0x40
#define L1_MISC_CNTRL_1_REG_L2toL1_rsp_ir_iw_set_OFFSET        7
#define L1_MISC_CNTRL_1_REG_L2toL1_rsp_ir_iw_set_WIDTH         1
#define L1_MISC_CNTRL_1_REG_L2toL1_rsp_ir_iw_set_MASK          0x80
#define L1_MISC_CNTRL_1_Reserved_8_8_OFFSET                    8
#define L1_MISC_CNTRL_1_Reserved_8_8_WIDTH                     1
#define L1_MISC_CNTRL_1_Reserved_8_8_MASK                      0x100
#define L1_MISC_CNTRL_1_REG_L2toL1_credit_ctrl_OFFSET          9
#define L1_MISC_CNTRL_1_REG_L2toL1_credit_ctrl_WIDTH           1
#define L1_MISC_CNTRL_1_REG_L2toL1_credit_ctrl_MASK            0x200
#define L1_MISC_CNTRL_1_REG_eco_allow_update_dis_OFFSET        10
#define L1_MISC_CNTRL_1_REG_eco_allow_update_dis_WIDTH         1
#define L1_MISC_CNTRL_1_REG_eco_allow_update_dis_MASK          0x400
#define L1_MISC_CNTRL_1_Reserved_11_11_OFFSET                  11
#define L1_MISC_CNTRL_1_Reserved_11_11_WIDTH                   1
#define L1_MISC_CNTRL_1_Reserved_11_11_MASK                    0x800
#define L1_MISC_CNTRL_1_REG_Truncate_ATS_Rsp_Over4K_En_OFFSET  12
#define L1_MISC_CNTRL_1_REG_Truncate_ATS_Rsp_Over4K_En_WIDTH   1
#define L1_MISC_CNTRL_1_REG_Truncate_ATS_Rsp_Over4K_En_MASK    0x1000
#define L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_OFFSET        13
#define L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_WIDTH         1
#define L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_MASK          0x2000
#define L1_MISC_CNTRL_1_REG_ATS_no_phys_overlap_En_OFFSET      14
#define L1_MISC_CNTRL_1_REG_ATS_no_phys_overlap_En_WIDTH       1
#define L1_MISC_CNTRL_1_REG_ATS_no_phys_overlap_En_MASK        0x4000
#define L1_MISC_CNTRL_1_REG_guest_addr_mask_en_OFFSET          15
#define L1_MISC_CNTRL_1_REG_guest_addr_mask_en_WIDTH           1
#define L1_MISC_CNTRL_1_REG_guest_addr_mask_en_MASK            0x8000
#define L1_MISC_CNTRL_1_REG_aerlog_on_abrt_OFFSET              16
#define L1_MISC_CNTRL_1_REG_aerlog_on_abrt_WIDTH               1
#define L1_MISC_CNTRL_1_REG_aerlog_on_abrt_MASK                0x10000
#define L1_MISC_CNTRL_1_REG_enable_nw_OFFSET                   17
#define L1_MISC_CNTRL_1_REG_enable_nw_WIDTH                    1
#define L1_MISC_CNTRL_1_REG_enable_nw_MASK                     0x20000
#define L1_MISC_CNTRL_1_REG_force_pasid_vld_OFFSET             18
#define L1_MISC_CNTRL_1_REG_force_pasid_vld_WIDTH              1
#define L1_MISC_CNTRL_1_REG_force_pasid_vld_MASK               0x40000
#define L1_MISC_CNTRL_1_Reserved_19_19_OFFSET                  19
#define L1_MISC_CNTRL_1_Reserved_19_19_WIDTH                   1
#define L1_MISC_CNTRL_1_Reserved_19_19_MASK                    0x80000
#define L1_MISC_CNTRL_1_REG_force_tlpprefix_EX_OFFSET          20
#define L1_MISC_CNTRL_1_REG_force_tlpprefix_EX_WIDTH           1
#define L1_MISC_CNTRL_1_REG_force_tlpprefix_EX_MASK            0x100000
#define L1_MISC_CNTRL_1_REG_force_tlpprefix_PMR_OFFSET         21
#define L1_MISC_CNTRL_1_REG_force_tlpprefix_PMR_WIDTH          1
#define L1_MISC_CNTRL_1_REG_force_tlpprefix_PMR_MASK           0x200000
#define L1_MISC_CNTRL_1_REG_ignore_iwir_for_s_bit_OFFSET       22
#define L1_MISC_CNTRL_1_REG_ignore_iwir_for_s_bit_WIDTH        1
#define L1_MISC_CNTRL_1_REG_ignore_iwir_for_s_bit_MASK         0x400000
#define L1_MISC_CNTRL_1_REG_force_ats_len_OFFSET               23
#define L1_MISC_CNTRL_1_REG_force_ats_len_WIDTH                4
#define L1_MISC_CNTRL_1_REG_force_ats_len_MASK                 0x7800000
#define L1_MISC_CNTRL_1_RESERVED_OFFSET                        27
#define L1_MISC_CNTRL_1_RESERVED_WIDTH                         5
#define L1_MISC_CNTRL_1_RESERVED_MASK                          0xf8000000L

/// L1_MISC_CNTRL_1
typedef union {
  struct {
    UINT32    REG_force_OrderStreamID_func   : 1;                      ///<
    UINT32    REG_ForceBypass                : 1;                      ///<
    UINT32    REG_cmdwait_wait_emptwq        : 1;                      ///<
    UINT32    Reserved_3_3                   : 1;                      ///<
    UINT32    REG_inv_wait_emptwq            : 1;                      ///<
    UINT32    Reserved_5_5                   : 1;                      ///<
    UINT32    REG_disble_eco_invaldation_0   : 1;                      ///<
    UINT32    REG_L2toL1_rsp_ir_iw_set       : 1;                      ///<
    UINT32    Reserved_8_8                   : 1;                      ///<
    UINT32    REG_L2toL1_credit_ctrl         : 1;                      ///<
    UINT32    REG_eco_allow_update_dis       : 1;                      ///<
    UINT32    Reserved_11_11                 : 1;                      ///<
    UINT32    REG_Truncate_ATS_Rsp_Over4K_En : 1;                      ///<
    UINT32    REG_Block_On_4K_wait_dis       : 1;                      ///<
    UINT32    REG_ATS_no_phys_overlap_En     : 1;                      ///<
    UINT32    REG_guest_addr_mask_en         : 1;                      ///<
    UINT32    REG_aerlog_on_abrt             : 1;                      ///<
    UINT32    REG_enable_nw                  : 1;                      ///<
    UINT32    REG_force_pasid_vld            : 1;                      ///<
    UINT32    Reserved_19_19                 : 1;                      ///<
    UINT32    REG_force_tlpprefix_EX         : 1;                      ///<
    UINT32    REG_force_tlpprefix_PMR        : 1;                      ///<
    UINT32    REG_ignore_iwir_for_s_bit      : 1;                      ///<
    UINT32    REG_force_ats_len              : 4;                      ///<
    UINT32    RESERVED                       : 5;                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L1_MISC_CNTRL_1_STRUCT;

// Address
#define L1_CNTRL_0_PCIE0_ADDRESS_HYEX               (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX << 20) + 0x00030)
#define L1_CNTRL_0_PCIE1_ADDRESS_HYEX               (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX << 20) + 0x00030)
#define L1_CNTRL_0_NBIF0_ADDRESS_HYEX               (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX << 20) + 0x00030)
#define L1_CNTRL_0_IOAGR_ADDRESS_HYEX               (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX << 20) + 0x00030)

#define L1_CNTRL_0_PCIE0_ADDRESS_HYGX               (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX << 20) + 0x00030)
#define L1_CNTRL_0_PCIE1_ADDRESS_HYGX               (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYGX << 20) + 0x00030)
#define L1_CNTRL_0_NBIF0_ADDRESS_HYGX               (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYGX << 20) + 0x00030)
#define L1_CNTRL_0_IOAGR_ADDRESS_HYGX               (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYGX << 20) + 0x00030)

// Type
#define L1_CNTRL_0_TYPE                             TYPE_SMN

#define L1_CNTRL_0_Unfilter_dis_OFFSET              0
#define L1_CNTRL_0_Unfilter_dis_WIDTH               1
#define L1_CNTRL_0_Unfilter_dis_MASK                0x1
#define L1_CNTRL_0_Fragment_dis_OFFSET              1
#define L1_CNTRL_0_Fragment_dis_WIDTH               1
#define L1_CNTRL_0_Fragment_dis_MASK                0x2
#define L1_CNTRL_0_CacheIR_only_OFFSET              2
#define L1_CNTRL_0_CacheIR_only_WIDTH               1
#define L1_CNTRL_0_CacheIR_only_MASK                0x4
#define L1_CNTRL_0_CacheIW_only_OFFSET              3
#define L1_CNTRL_0_CacheIW_only_WIDTH               1
#define L1_CNTRL_0_CacheIW_only_MASK                0x8
#define L1_CNTRL_0_Reserved0_OFFSET                 4
#define L1_CNTRL_0_Reserved0_WIDTH                  1
#define L1_CNTRL_0_Reserved0_MASK                   0x10
#define L1_CNTRL_0_RESERVED_OFFSET                  5
#define L1_CNTRL_0_RESERVED_WIDTH                   1
#define L1_CNTRL_0_RESERVED_MASK                    0x20
#define L1_CNTRL_0_Reserved_7_6_OFFSET              6
#define L1_CNTRL_0_Reserved_7_6_WIDTH               2
#define L1_CNTRL_0_Reserved_7_6_MASK                0xc0
#define L1_CNTRL_0_L2Credits_OFFSET                 8
#define L1_CNTRL_0_L2Credits_WIDTH                  6
#define L1_CNTRL_0_L2Credits_MASK                   0x3f00
#define L1_CNTRL_0_Reserved1_OFFSET                 14
#define L1_CNTRL_0_Reserved1_WIDTH                  6
#define L1_CNTRL_0_Reserved1_MASK                   0xfc000
#define L1_CNTRL_0_L1Banks_OFFSET                   20
#define L1_CNTRL_0_L1Banks_WIDTH                    2
#define L1_CNTRL_0_L1Banks_MASK                     0x300000
#define L1_CNTRL_0_Reserved_23_22_OFFSET            22
#define L1_CNTRL_0_Reserved_23_22_WIDTH             2
#define L1_CNTRL_0_Reserved_23_22_MASK              0xc00000
#define L1_CNTRL_0_L1Entries_OFFSET                 24
#define L1_CNTRL_0_L1Entries_WIDTH                  4
#define L1_CNTRL_0_L1Entries_MASK                   0xf000000
#define L1_CNTRL_0_L1ErrEventDetectDis_OFFSET       28
#define L1_CNTRL_0_L1ErrEventDetectDis_WIDTH        1
#define L1_CNTRL_0_L1ErrEventDetectDis_MASK         0x10000000
#define L1_CNTRL_0_L1ForceHostRspPassPWHigh_OFFSET  29
#define L1_CNTRL_0_L1ForceHostRspPassPWHigh_WIDTH   2
#define L1_CNTRL_0_L1ForceHostRspPassPWHigh_MASK    0x60000000
#define L1_CNTRL_0_L1InterruptHalfDwDis_OFFSET      31
#define L1_CNTRL_0_L1InterruptHalfDwDis_WIDTH       1
#define L1_CNTRL_0_L1InterruptHalfDwDis_MASK        0x80000000L

/// L1_CNTRL_0
typedef union {
  struct {
    UINT32    Unfilter_dis             : 1;                            ///<
    UINT32    Fragment_dis             : 1;                            ///<
    UINT32    CacheIR_only             : 1;                            ///<
    UINT32    CacheIW_only             : 1;                            ///<
    UINT32    Reserved0                : 1;                            ///<
    UINT32    RESERVED                 : 1;                            ///<
    UINT32    Reserved_7_6             : 2;                            ///<
    UINT32    L2Credits                : 6;                            ///<
    UINT32    Reserved1                : 6;                            ///<
    UINT32    L1Banks                  : 2;                            ///<
    UINT32    Reserved_23_22           : 2;                            ///<
    UINT32    L1Entries                : 4;                            ///<
    UINT32    L1ErrEventDetectDis      : 1;                            ///<
    UINT32    L1ForceHostRspPassPWHigh : 2;                            ///<
    UINT32    L1InterruptHalfDwDis     : 1;                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L1_CNTRL_0_STRUCT;

#define L1_CNTRL_1_PCIE0_ADDRESS_HYEX          (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX << 20) + 0x00034)
#define L1_CNTRL_1_PCIE1_ADDRESS_HYEX          (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX << 20) + 0x00034)
#define L1_CNTRL_1_NBIF0_ADDRESS_HYEX          (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX << 20) + 0x00034)
#define L1_CNTRL_1_IOAGR_ADDRESS_HYEX          (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX << 20) + 0x00034)

#define L1_CNTRL_1_PCIE0_ADDRESS_HYGX          (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX << 20) + 0x00034)
#define L1_CNTRL_1_PCIE1_ADDRESS_HYGX          (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYGX << 20) + 0x00034)
#define L1_CNTRL_1_NBIF0_ADDRESS_HYGX          (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYGX << 20) + 0x00034)
#define L1_CNTRL_1_IOAGR_ADDRESS_HYGX          (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYGX << 20) + 0x00034)

// Address
#define L1_CNTRL_2_PCIE0_ADDRESS_HYEX          (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX << 20) + 0x00038)
#define L1_CNTRL_2_PCIE1_ADDRESS_HYEX          (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX << 20) + 0x00038)
#define L1_CNTRL_2_NBIF0_ADDRESS_HYEX          (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX << 20) + 0x00038)
#define L1_CNTRL_2_IOAGR_ADDRESS_HYEX          (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX << 20) + 0x00038)

#define L1_CNTRL_2_PCIE0_ADDRESS_HYGX          (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX << 20) + 0x00038)
#define L1_CNTRL_2_PCIE1_ADDRESS_HYGX          (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYGX << 20) + 0x00038)
#define L1_CNTRL_2_NBIF0_ADDRESS_HYGX          (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYGX << 20) + 0x00038)
#define L1_CNTRL_2_IOAGR_ADDRESS_HYGX          (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYGX << 20) + 0x00038)

// Type
#define L1_CNTRL_2_TYPE                                   TYPE_SMN

#define L1_CNTRL_2_L1Disable_OFFSET                       0
#define L1_CNTRL_2_L1Disable_WIDTH                        1
#define L1_CNTRL_2_L1Disable_MASK                         0x1
#define L1_CNTRL_2_MSI_to_HT_remap_dis_OFFSET             1
#define L1_CNTRL_2_MSI_to_HT_remap_dis_WIDTH              1
#define L1_CNTRL_2_MSI_to_HT_remap_dis_MASK               0x2
#define L1_CNTRL_2_L1_abrt_ats_dis_OFFSET                 2
#define L1_CNTRL_2_L1_abrt_ats_dis_WIDTH                  1
#define L1_CNTRL_2_L1_abrt_ats_dis_MASK                   0x4
#define L1_CNTRL_2_L1ATSDataErrorSignalEn_OFFSET          3
#define L1_CNTRL_2_L1ATSDataErrorSignalEn_WIDTH           1
#define L1_CNTRL_2_L1ATSDataErrorSignalEn_MASK            0x8
#define L1_CNTRL_2_RESERVED_OFFSET                        4
#define L1_CNTRL_2_RESERVED_WIDTH                         20
#define L1_CNTRL_2_RESERVED_MASK                          0xfffff0
#define L1_CNTRL_2_CPD_RESP_MODE_OFFSET                   24
#define L1_CNTRL_2_CPD_RESP_MODE_WIDTH                    3
#define L1_CNTRL_2_CPD_RESP_MODE_MASK                     0x7000000
#define L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_OFFSET  27
#define L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_WIDTH   1
#define L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_MASK    0x8000000
#define L1_CNTRL_2_L1ConsumedDataErrorSignalEn_OFFSET     28
#define L1_CNTRL_2_L1ConsumedDataErrorSignalEn_WIDTH      1
#define L1_CNTRL_2_L1ConsumedDataErrorSignalEn_MASK       0x10000000
#define L1_CNTRL_2_L1SDPParityEn_OFFSET                   29
#define L1_CNTRL_2_L1SDPParityEn_WIDTH                    1
#define L1_CNTRL_2_L1SDPParityEn_MASK                     0x20000000
#define L1_CNTRL_2_FlushVC_HRT1_Inv_OFFSET                30
#define L1_CNTRL_2_FlushVC_HRT1_Inv_WIDTH                 1
#define L1_CNTRL_2_FlushVC_HRT1_Inv_MASK                  0x40000000
#define L1_CNTRL_2_FlushVC_HRT1_IntInv_OFFSET             31
#define L1_CNTRL_2_FlushVC_HRT1_IntInv_WIDTH              1
#define L1_CNTRL_2_FlushVC_HRT1_IntInv_MASK               0x80000000L

/// L1_CNTRL_2
typedef union {
  struct {
    UINT32    L1Disable                      : 1;                      ///<
    UINT32    MSI_to_HT_remap_dis            : 1;                      ///<
    UINT32    L1_abrt_ats_dis                : 1;                      ///<
    UINT32    L1ATSDataErrorSignalEn         : 1;                      ///<
    UINT32    RESERVED                       : 20;                     ///<
    UINT32    CPD_RESP_MODE                  : 3;                      ///<
    UINT32    L1NonConsumedDataErrorSignalEn : 1;                      ///<
    UINT32    L1ConsumedDataErrorSignalEn    : 1;                      ///<
    UINT32    L1SDPParityEn                  : 1;                      ///<
    UINT32    FlushVC_HRT1_Inv               : 1;                      ///<
    UINT32    FlushVC_HRT1_IntInv            : 1;                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L1_CNTRL_2_STRUCT;

//IOAGR
#define IOAGR_NBIF2_A2S_CNTL_SW0_HYGX                   (UINT32)((NBIO0_SYSHUB2_APERTURE_ID_HYGX << 20) + 0x03ae8)
#define IOAGR_NBIF2_A2S_CNTL_SW1_HYGX                   (UINT32)((NBIO0_SYSHUB2_APERTURE_ID_HYGX << 20) + 0x03aec)
#define IOAGR_NBIF2_A2S_CNTL_SW2_HYGX                   (UINT32)((NBIO0_SYSHUB2_APERTURE_ID_HYGX << 20) + 0x03af0)
#define IOAGR_NBIF2_A2S_CNTL_SW3_HYGX                   (UINT32)((NBIO0_SYSHUB2_APERTURE_ID_HYGX << 20) + 0x03af4)
#define IOAGR_NBIF2_A2S_CNTL_SW4_HYGX                   (UINT32)((NBIO0_SYSHUB2_APERTURE_ID_HYGX << 20) + 0x03af8)

#define IOAGR_NBIF3_A2S_CNTL_SW0_HYGX                   (UINT32)((NBIO0_SYSHUB3_APERTURE_ID_HYGX << 20) + 0x03af0)
#define IOAGR_NBIF3_A2S_CNTL_SW1_HYGX                   (UINT32)((NBIO0_SYSHUB3_APERTURE_ID_HYGX << 20) + 0x03af4)
#define IOAGR_NBIF3_A2S_CNTL_SW2_HYGX                   (UINT32)((NBIO0_SYSHUB3_APERTURE_ID_HYGX << 20) + 0x03af8)
#define IOAGR_NBIF3_A2S_CNTL_SW3_HYGX                   (UINT32)((NBIO0_SYSHUB3_APERTURE_ID_HYGX << 20) + 0x03afc)
#define IOAGR_NBIF3_A2S_CNTL_SW4_HYGX                   (UINT32)((NBIO0_SYSHUB3_APERTURE_ID_HYGX << 20) + 0x03b00)

// Address
#define L1_CNTRL_4_PCIE0_ADDRESS_HYEX              (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX << 20) + 0x000c8)
#define L1_CNTRL_4_PCIE1_ADDRESS_HYEX              (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX << 20) + 0x000c8)
#define L1_CNTRL_4_NBIF0_ADDRESS_HYEX              (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX << 20) + 0x000c8)
#define L1_CNTRL_4_IOAGR_ADDRESS_HYEX              (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX << 20) + 0x000c8)

#define L1_CNTRL_4_PCIE0_ADDRESS_HYGX              (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX << 20) + 0x000c8)
#define L1_CNTRL_4_PCIE1_ADDRESS_HYGX              (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYGX << 20) + 0x000c8)
#define L1_CNTRL_4_NBIF0_ADDRESS_HYGX              (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYGX << 20) + 0x000c8)
#define L1_CNTRL_4_IOAGR_ADDRESS_HYGX              (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYGX << 20) + 0x000c8)

// Type
#define L1_CNTRL_4_TYPE                            TYPE_SMN

#define L1_CNTRL_4_ATS_multiple_resp_en_OFFSET     0
#define L1_CNTRL_4_ATS_multiple_resp_en_WIDTH      1
#define L1_CNTRL_4_ATS_multiple_resp_en_MASK       0x1
#define L1_CNTRL_4_Timeout_pulse_ext_En_OFFSET     2
#define L1_CNTRL_4_Timeout_pulse_ext_En_WIDTH      1
#define L1_CNTRL_4_Timeout_pulse_ext_En_MASK       0x4
#define L1_CNTRL_4_RESERVED_OFFSET                 4
#define L1_CNTRL_4_RESERVED_WIDTH                  19
#define L1_CNTRL_4_RESERVED_MASK                   0x7ffff0
#define L1_CNTRL_4_AtsRsp_send_mem_type_en_OFFSET  23
#define L1_CNTRL_4_AtsRsp_send_mem_type_en_WIDTH   1
#define L1_CNTRL_4_AtsRsp_send_mem_type_en_MASK    0x800000
#define L1_CNTRL_4_IntGfx_UnitID_Val_OFFSET        24
#define L1_CNTRL_4_IntGfx_UnitID_Val_WIDTH         7
#define L1_CNTRL_4_IntGfx_UnitID_Val_MASK          0x7f000000

/// IOMMUL1IOAGR_0C8
typedef union {
  struct {
    UINT32    ATS_multiple_resp_en    : 1;                             ///<
    UINT32    Reserved_1_1            : 1;                             ///<
    UINT32    Timeout_pulse_ext_En    : 1;                             ///<
    UINT32    Reserved_3_3            : 1;                             ///<
    UINT32    RESERVED                : 19;                            ///<
    UINT32    AtsRsp_send_mem_type_en : 1;                             ///<
    UINT32    IntGfx_UnitID_Val       : 7;                             ///<
    UINT32    Reserved_31_31          : 1;                             ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L1_CNTRL_4_STRUCT;

// Register Name L1_FEATURE_CNTRL

// Address
#define L1_FEATURE_CNTRL_PCIE0_ADDRESS_HYEX           (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX << 20) + 0x0009C)
#define L1_FEATURE_CNTRL_PCIE1_ADDRESS_HYEX           (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX << 20) + 0x0009C)
#define L1_FEATURE_CNTRL_NBIF0_ADDRESS_HYEX           (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX << 20) + 0x0009C)
#define L1_FEATURE_CNTRL_IOAGR_ADDRESS_HYEX           (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX << 20) + 0x0009C)

#define L1_FEATURE_CNTRL_PCIE0_ADDRESS_HYGX           (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX << 20) + 0x0009C)
#define L1_FEATURE_CNTRL_PCIE1_ADDRESS_HYGX           (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYGX << 20) + 0x0009C)
#define L1_FEATURE_CNTRL_NBIF0_ADDRESS_HYGX           (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYGX << 20) + 0x0009C)
#define L1_FEATURE_CNTRL_IOAGR_ADDRESS_HYGX           (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYGX << 20) + 0x0009C)

// Type
#define L1_FEATURE_CNTRL_TYPE                        TYPE_SMN

#define L1_FEATURE_CNTRL_Rsv_sticky_bits0_OFFSET     0
#define L1_FEATURE_CNTRL_Rsv_sticky_bits0_WIDTH      1
#define L1_FEATURE_CNTRL_Rsv_sticky_bits0_MASK       0x1
#define L1_FEATURE_CNTRL_PMR_lock_bit_OFFSET         1
#define L1_FEATURE_CNTRL_PMR_lock_bit_WIDTH          1
#define L1_FEATURE_CNTRL_PMR_lock_bit_MASK           0x2
#define L1_FEATURE_CNTRL_EXE_lock_bit_OFFSET         2
#define L1_FEATURE_CNTRL_EXE_lock_bit_WIDTH          1
#define L1_FEATURE_CNTRL_EXE_lock_bit_MASK           0x4
#define L1_FEATURE_CNTRL_Rsv_sticky_bits7to3_OFFSET  3
#define L1_FEATURE_CNTRL_Rsv_sticky_bits7to3_WIDTH   5
#define L1_FEATURE_CNTRL_Rsv_sticky_bits7to3_MASK    0xf8
#define L1_FEATURE_CNTRL_Reserved_31_8_OFFSET        8
#define L1_FEATURE_CNTRL_Reserved_31_8_WIDTH         24
#define L1_FEATURE_CNTRL_Reserved_31_8_MASK          0xffffff00L

/// L1_FEATURE_CNTRL
typedef union {
  struct {
    UINT32    Rsv_sticky_bits0    : 1;                                 ///<
    UINT32    PMR_lock_bit        : 1;                                 ///<
    UINT32    EXE_lock_bit        : 1;                                 ///<
    UINT32    Rsv_sticky_bits7to3 : 5;                                 ///<
    UINT32    Reserved_31_8       : 24;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L1_FEATURE_CNTRL_STRUCT;

// Address
#define L1_CLKCNTRL_0_PCIE0_ADDRESS_HYEX            (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX << 20) + 0x000cc)
#define L1_CLKCNTRL_0_PCIE1_ADDRESS_HYEX            (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX << 20) + 0x000cc)
#define L1_CLKCNTRL_0_NBIF0_ADDRESS_HYEX            (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX << 20) + 0x000cc)
#define L1_CLKCNTRL_0_IOAGR_ADDRESS_HYEX            (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX << 20) + 0x000cc)

#define L1_CLKCNTRL_0_PCIE0_ADDRESS_HYGX            (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX << 20) + 0x000cc)
#define L1_CLKCNTRL_0_PCIE1_ADDRESS_HYGX            (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYGX << 20) + 0x000cc)
#define L1_CLKCNTRL_0_NBIF0_ADDRESS_HYGX            (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYGX << 20) + 0x000cc)
#define L1_CLKCNTRL_0_IOAGR_ADDRESS_HYGX            (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYGX << 20) + 0x000cc)

// Type
#define L1_CLKCNTRL_0_TYPE                          TYPE_SMN

#define L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET      4
#define L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_WIDTH       1
#define L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK        0x10
#define L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET    5
#define L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_WIDTH     1
#define L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK      0x20
#define L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET    6
#define L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_WIDTH     1
#define L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK      0x40
#define L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET     8
#define L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_WIDTH      1
#define L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK       0x100
#define L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET   9
#define L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_WIDTH    1
#define L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK     0x200
#define L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET      10
#define L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_WIDTH       1
#define L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK        0x400
#define L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET  11
#define L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_WIDTH   1
#define L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK    0x800
#define L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET   12
#define L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_WIDTH    1
#define L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK     0x1000
#define L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET  13
#define L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_WIDTH   1
#define L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK    0x2000
#define L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET  14
#define L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_WIDTH   8
#define L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK    0x3fc000
#define L1_CLKCNTRL_0_reserved_OFFSET               22
#define L1_CLKCNTRL_0_reserved_WIDTH                9
#define L1_CLKCNTRL_0_reserved_MASK                 0x7fc00000
#define L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET       31
#define L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_WIDTH        1
#define L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK         0x80000000L

/// L1_CLK_CNTROL0
typedef union {
  struct {
    UINT32    Reserved_3_0          : 4;                               ///<
    UINT32    L1_DMA_CLKGATE_EN     : 1;                               ///<
    UINT32    L1_CACHE_CLKGATE_EN   : 1;                               ///<
    UINT32    L1_CPSLV_CLKGATE_EN   : 1;                               ///<
    UINT32    Reserved_7_7          : 1;                               ///<
    UINT32    L1_PERF_CLKGATE_EN    : 1;                               ///<
    UINT32    L1_MEMORY_CLKGATE_EN  : 1;                               ///<
    UINT32    L1_REG_CLKGATE_EN     : 1;                               ///<
    UINT32    L1_HOSTREQ_CLKGATE_EN : 1;                               ///<
    UINT32    L1_DMARSP_CLKGATE_EN  : 1;                               ///<
    UINT32    L1_HOSTRSP_CLKGATE_EN : 1;                               ///<
    UINT32    L1_CLKGATE_HYSTERESIS : 8;                               ///<
    UINT32    reserved              : 9;                               ///<
    UINT32    L1_L2_CLKGATE_EN      : 1;                               ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L1_CLKCNTRL_0_STRUCT;

//
// Register Name L1_FEATURE_SUP_CNTRL
//
#define L1_FEATURE_SUP_CNTRL_ADDRESS_HYEX              (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX << 20) + 0x000dc)
#define L1_FEATURE_SUP_CNTRL_ADDRESS_HYGX              (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX << 20) + 0x000dc)
#define L1_FEATURE_SUP_CNTRL_L1_XT_SUP_W_OFFSET       5
#define L1_FEATURE_SUP_CNTRL_L1_XT_SUP_W_WIDTH        1
#define L1_FEATURE_SUP_CNTRL_L1_XT_SUP_W_MASK         0x20

/// Bitfield Description :
#define L1_FEATURE_SUP_CNTRL_Reserved_31_6_OFFSET     6
#define L1_FEATURE_SUP_CNTRL_Reserved_31_6_WIDTH      26
#define L1_FEATURE_SUP_CNTRL_Reserved_31_6_MASK       0xffffffc0L

typedef union {
  struct {
    UINT32    L1_EFR_SUP    : 1;                                       ///<
    UINT32    L1_PPR_SUP    : 1;                                       ///<
    UINT32    L1_DTE_seg_W  : 2;                                       ///<
    UINT32    L1_GT_SUP_W   : 1;                                       ///<
    UINT32    L1_XT_SUP_W   : 1;                                       ///<
    UINT32    Reserved_31_6 : 26;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L1_FEATURE_SUP_CNTRL_STRUCT;
// IOHUB::IOMMUL1::L1_CNTRL_4_instIOMMUL1INT0_aliasSMN  NBIF
// Address
#define L1_SB_LOCATION_PCIE0_ADDRESS_HYEX           (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYEX << 20) + 0x00024)
#define L1_SB_LOCATION_PCIE1_ADDRESS_HYEX           (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYEX << 20) + 0x00024)
#define L1_SB_LOCATION_NBIF0_ADDRESS_HYEX           (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYEX << 20) + 0x00024)
#define L1_SB_LOCATION_IOAGR_ADDRESS_HYEX           (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYEX << 20) + 0x00024)

#define L1_SB_LOCATION_PCIE0_ADDRESS_HYGX           (UINT32)((NBIO0_L1IMU_PCIE0_APERTURE_ID_HYGX << 20) + 0x00024)
#define L1_SB_LOCATION_PCIE1_ADDRESS_HYGX           (UINT32)((NBIO0_L1IMU_PCIE1_APERTURE_ID_HYGX << 20) + 0x00024)
#define L1_SB_LOCATION_NBIF0_ADDRESS_HYGX           (UINT32)((NBIO0_L1IMU_NBIF0_APERTURE_ID_HYGX << 20) + 0x00024)
#define L1_SB_LOCATION_IOAGR_ADDRESS_HYGX           (UINT32)((NBIO0_L1IMU_IOAGR_APERTURE_ID_HYGX << 20) + 0x00024)

// Type
#define L1_SB_LOCATION_TYPE                        TYPE_SMN

#define L1_SB_LOCATION_SBlocated_Port_OFFSET       0
#define L1_SB_LOCATION_SBlocated_Port_WIDTH        16
#define L1_SB_LOCATION_SBlocated_Port_MASK         0xffff
#define L1_SB_LOCATION_SBlocated_Core_OFFSET       16
#define L1_SB_LOCATION_SBlocated_Core_WIDTH        16
#define L1_SB_LOCATION_SBlocated_Core_MASK         0xffff0000L

/// L1_SB_LOCATION
typedef union {
  struct {
    UINT32    SBlocated_Port : 16;                                     ///<
    UINT32    SBlocated_Core : 16;                                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L1_SB_LOCATION_STRUCT;

// IOHUB::IOMMUL2A::L2A_UPDATE_FILTER_CNTL_aliasSMN;
// Address
#define L2_DTC_CONTROL_ADDRESS_HYEX                       (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX << 20) + 0x00040)
#define L2_ITC_CONTROL_ADDRESS_HYEX                       (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX << 20) + 0x00050)
#define L2_PTC_A_CONTROL_ADDRESS_HYEX                     (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX << 20) + 0x00060)
#define L2A_UPDATE_FILTER_CNTL_ADDRESS_HYEX               (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX << 20) + 0x00088)

#define L2_DTC_CONTROL_ADDRESS_HYGX                       (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX << 20) + 0x00040)
#define L2_ITC_CONTROL_ADDRESS_HYGX                       (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX << 20) + 0x00050)
#define L2_PTC_A_CONTROL_ADDRESS_HYGX                     (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX << 20) + 0x00060)
#define L2A_UPDATE_FILTER_CNTL_ADDRESS_HYGX               (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX << 20) + 0x00088)

// Type
#define L2A_UPDATE_FILTER_CNTL_TYPE                      TYPE_SMN

#define SMN_15700088_L2a_Update_Filter_Bypass_OFFSET     0
#define SMN_15700088_L2a_Update_Filter_Bypass_WIDTH      1
#define SMN_15700088_L2a_Update_Filter_Bypass_MASK       0x1
#define SMN_15700088_L2a_Update_Filter_RdLatency_OFFSET  1
#define SMN_15700088_L2a_Update_Filter_RdLatency_WIDTH   4
#define SMN_15700088_L2a_Update_Filter_RdLatency_MASK    0x1e

/// SMN_15700088
typedef union {
  struct {
    UINT32    L2a_Update_Filter_Bypass    : 1;                         ///<
    UINT32    L2a_Update_Filter_RdLatency : 4;                         ///<
    UINT32    Reserved_31_5               : 27;                        ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} SMN_15700088_STRUCT;

// IOHUB::IOMMUL2A::L2_ERR_RULE_CONTROL_3_aliasSMN
// Address
#define L2_ERR_RULE_CONTROL_3_ADDRESS_HYEX             (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX << 20) + 0x000c0)
#define L2_ERR_RULE_CONTROL_3_ADDRESS_HYGX             (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX << 20) + 0x000c0)

// Type
#define L2_ERR_RULE_CONTROL_3_TYPE                    TYPE_SMN

#define L2_ERR_RULE_CONTROL_3_ERRRuleLock1_OFFSET     0
#define L2_ERR_RULE_CONTROL_3_ERRRuleLock1_WIDTH      1
#define L2_ERR_RULE_CONTROL_3_ERRRuleLock1_MASK       0x1
#define L2_ERR_RULE_CONTROL_3_ERRRuleDisable3_OFFSET  4
#define L2_ERR_RULE_CONTROL_3_ERRRuleDisable3_WIDTH   28
#define L2_ERR_RULE_CONTROL_3_ERRRuleDisable3_MASK    0xfffffff0L

/// SMN_157000C0
typedef union {
  struct {
    UINT32    ERRRuleLock1    : 1;                                     ///<
    UINT32    Reserved_3_1    : 3;                                     ///<
    UINT32    ERRRuleDisable3 : 28;                                    ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2_ERR_RULE_CONTROL_3_STRUCT;

// IOHUB::IOMMUL2A::L2_L2A_CK_GATE_CONTROL_aliasSMN
// Address
#define L2_L2A_CK_GATE_CONTROL_ADDRESS_HYEX           (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX << 20) + 0x000cc)
#define L2_L2A_CK_GATE_CONTROL_ADDRESS_HYGX           (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX << 20) + 0x000cc)

// Type
#define L2_L2A_CK_GATE_CONTROL_TYPE                  TYPE_SMN

#define IOMMUL2A_0CC_CKGateL2ARegsDisable_OFFSET     0
#define IOMMUL2A_0CC_CKGateL2ARegsDisable_WIDTH      1
#define IOMMUL2A_0CC_CKGateL2ARegsDisable_MASK       0x1
#define IOMMUL2A_0CC_CKGateL2ADynamicDisable_OFFSET  1
#define IOMMUL2A_0CC_CKGateL2ADynamicDisable_WIDTH   1
#define IOMMUL2A_0CC_CKGateL2ADynamicDisable_MASK    0x2
#define IOMMUL2A_0CC_CKGateL2ACacheDisable_OFFSET    2
#define IOMMUL2A_0CC_CKGateL2ACacheDisable_WIDTH     1
#define IOMMUL2A_0CC_CKGateL2ACacheDisable_MASK      0x4
#define IOMMUL2A_0CC_CKGateL2ASpare_OFFSET           3
#define IOMMUL2A_0CC_CKGateL2ASpare_WIDTH            1
#define IOMMUL2A_0CC_CKGateL2ASpare_MASK             0x8
#define IOMMUL2A_0CC_CKGateL2ALength_OFFSET          4
#define IOMMUL2A_0CC_CKGateL2ALength_WIDTH           2
#define IOMMUL2A_0CC_CKGateL2ALength_MASK            0x30
#define IOMMUL2A_0CC_CKGateL2AStop_OFFSET            6
#define IOMMUL2A_0CC_CKGateL2AStop_WIDTH             2
#define IOMMUL2A_0CC_CKGateL2AStop_MASK              0xc0
#define IOMMUL2A_0CC_Reserved_31_8_OFFSET            8
#define IOMMUL2A_0CC_Reserved_31_8_WIDTH             24
#define IOMMUL2A_0CC_Reserved_31_8_MASK              0xffffff00L

/// IOMMUL2A_0CC
typedef union {
  struct {
    UINT32    CKGateL2ARegsDisable    : 1;                             ///<
    UINT32    CKGateL2ADynamicDisable : 1;                             ///<
    UINT32    CKGateL2ACacheDisable   : 1;                             ///<
    UINT32    CKGateL2ASpare          : 1;                             ///<
    UINT32    CKGateL2ALength         : 2;                             ///<
    UINT32    CKGateL2AStop           : 2;                             ///<
    UINT32    Reserved_31_8           : 24;                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOMMUL2A_0CC_STRUCT;

// IOHUB::IOMMUL2A::L2_L2A_PGSIZE_CONTROL_aliasSMN;
// Address
#define L2_L2A_PGSIZE_CONTROL_ADDRESS_HYEX                (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX << 20) + 0x000d0)
#define L2_L2A_PGSIZE_CONTROL_ADDRESS_HYGX                (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX << 20) + 0x000d0)

// Type
#define L2_L2A_PGSIZE_CONTROL_TYPE                       TYPE_SMN

#define L2_L2A_PGSIZE_CONTROL_L2AREG_GST_PGSIZE_OFFSET   0
#define L2_L2A_PGSIZE_CONTROL_L2AREG_GST_PGSIZE_WIDTH    7
#define L2_L2A_PGSIZE_CONTROL_L2AREG_GST_PGSIZE_MASK     0x7f
#define L2_L2A_PGSIZE_CONTROL_L2AREG_HOST_PGSIZE_OFFSET  8
#define L2_L2A_PGSIZE_CONTROL_L2AREG_HOST_PGSIZE_WIDTH   7
#define L2_L2A_PGSIZE_CONTROL_L2AREG_HOST_PGSIZE_MASK    0x7f00

/// SMN_157000D0
typedef union {
  struct {
    UINT32    L2AREG_GST_PGSIZE  : 7;                                  ///<
    UINT32    Reserved_7_7       : 1;                                  ///<
    UINT32    L2AREG_HOST_PGSIZE : 7;                                  ///<
    UINT32    Reserved_31_15     : 17;                                 ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} L2_L2A_PGSIZE_CONTROL_STRUCT;

// Address
#define SST_CLOCK_CTRL_SST0_ADDRESS_HYEX                            (UINT32)((NBIO0_SST_FCH_APERTURE_ID_HYEX << 20) + 0x00004)
#define SST_CLOCK_CTRL_SST1_ADDRESS_HYEX                            (UINT32)((NBIO0_SST_NBIO_APERTURE_ID_HYEX << 20) + 0x00004)

#define SST_CLOCK_CTRL_SST0_ADDRESS_HYGX                            (UINT32)((NBIO0_SST_NBIO_APERTURE_ID_HYGX << 20) + 0x00004)
#define SST_CLOCK_CTRL_SST1_ADDRESS_HYGX                            (UINT32)((NBIO0_SST_FCH_APERTURE_ID_HYGX << 20) + 0x00004)

// Type
#define TYPE_SST0  TYPE_SMN
#define TYPE_SST1  TYPE_SMN

#define SST_CLOCK_CTRL_TXCLKGATEEn_OFFSET                               0
#define SST_CLOCK_CTRL_TXCLKGATEEn_WIDTH                                1
#define SST_CLOCK_CTRL_TXCLKGATEEn_MASK                                 0x1
#define SST_CLOCK_CTRL_Reserved1_OFFSET                                 1
#define SST_CLOCK_CTRL_Reserved1_WIDTH                                  7
#define SST_CLOCK_CTRL_Reserved1_MASK                                   0xfe
#define SST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET                           8
#define SST_CLOCK_CTRL_PCTRL_IDLE_TIME_WIDTH                            8
#define SST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK                             0xff00
#define SST_CLOCK_CTRL_RXCLKGATEEn_OFFSET                               16
#define SST_CLOCK_CTRL_RXCLKGATEEn_WIDTH                                1
#define SST_CLOCK_CTRL_RXCLKGATEEn_MASK                                 0x10000
#define SST_CLOCK_CTRL_Reserved0_OFFSET                                 17
#define SST_CLOCK_CTRL_Reserved0_WIDTH                                  4
#define SST_CLOCK_CTRL_Reserved0_MASK                                   0x1e0000
#define SST_CLOCK_CTRL_RX_IDLE_IGONRE_RX_FIFO_EMPTY_OFFSET              21
#define SST_CLOCK_CTRL_RX_IDLE_IGONRE_RX_FIFO_EMPTY_WIDTH               1
#define SST_CLOCK_CTRL_RX_IDLE_IGONRE_RX_FIFO_EMPTY_MASK                0x200000
#define SST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_OFFSET        22
#define SST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_WIDTH         1
#define SST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_MASK          0x400000
#define SST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_OFFSET                    23
#define SST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_WIDTH                     1
#define SST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_MASK                      0x800000
#define SST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_OFFSET                        24
#define SST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_WIDTH                         8
#define SST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_MASK                          0xff000000

/// SST_CLOCK_CTRL_STRUCT
typedef union {
  struct {
    UINT32    TXCLKGATEEn                           : 1;                                     ///<
    UINT32    Reserved1                             : 7;                                     ///<
    UINT32    PCTRL_IDLE_TIME                       : 8;                                     ///<
    UINT32    RXCLKGATEEn                           : 1;                                     ///<
    UINT32    Reserved0                             : 4;                                     ///<
    UINT32    RX_IDLE_IGONRE_RX_FIFO_EMPTY          : 1;                                     ///< Only HyGx support
    UINT32    PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE    : 1;                                     ///< Only HyGx support
    UINT32    PCTRL_USE_RX_IDLE_TIME                : 1;                                     ///< Only HyGx support
    UINT32    PCTRL_RX_IDLE_TIME                    : 8;                                     ///< Only HyGx support
  } Field;                                                                                   ///<
  UINT32    Value;                                                                           ///<
} SST_CLOCK_CTRL_STRUCT;

// Address
#define FST_CLOCK_CTRL_FST0_ADDRESS_HYGX                                0x0C000004

// Type
#define FST_CLOCK_CTRL_FST0_TYPE                                        TYPE_SMN

#define FST_CLOCK_CTRL_TXCLKGATEEn_OFFSET                               0
#define FST_CLOCK_CTRL_TXCLKGATEEn_WIDTH                                1
#define FST_CLOCK_CTRL_TXCLKGATEEn_MASK                                 0x1
#define FST_CLOCK_CTRL_Reserved1_OFFSET                                 1
#define FST_CLOCK_CTRL_Reserved1_WIDTH                                  7
#define FST_CLOCK_CTRL_Reserved1_MASK                                   0xfe
#define FST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET                           8
#define FST_CLOCK_CTRL_PCTRL_IDLE_TIME_WIDTH                            8
#define FST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK                             0xff00
#define FST_CLOCK_CTRL_RXCLKGATEEn_OFFSET                               16
#define FST_CLOCK_CTRL_RXCLKGATEEn_WIDTH                                1
#define FST_CLOCK_CTRL_RXCLKGATEEn_MASK                                 0x10000
#define FST_CLOCK_CTRL_Reserved0_OFFSET                                 17
#define FST_CLOCK_CTRL_Reserved0_WIDTH                                  5
#define FST_CLOCK_CTRL_Reserved0_MASK                                   0x3e0000
#define FST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_OFFSET        22
#define FST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_WIDTH         1
#define FST_CLOCK_CTRL_PCTRL_JUMP_RX_DISC_FROM_RX_CHKIDLE_MASK          0x400000
#define FST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_OFFSET                    23
#define FST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_WIDTH                     1
#define FST_CLOCK_CTRL_PCTRL_USE_RX_IDLE_TIME_MASK                      0x800000
#define FST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_OFFSET                        24
#define FST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_WIDTH                         8
#define FST_CLOCK_CTRL_PCTRL_RX_IDLE_TIME_MASK                          0xff000000

// Register Name SION_WRAPPER_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK
// Address
#define SION_WRAPPER_404_SST0_ADDRESS_HYEX                    (UINT32)((NBIO0_SST_FCH_APERTURE_ID_HYEX << 20) + 0x00404)
#define SION_WRAPPER_404_SST1_ADDRESS_HYEX                    (UINT32)((NBIO0_SST_NBIO_APERTURE_ID_HYEX << 20) + 0x00404) 

#define SION_WRAPPER_404_SST0_ADDRESS_HYGX                    (UINT32)((NBIO0_SST_NBIO_APERTURE_ID_HYGX << 20) + 0x00404)
#define SION_WRAPPER_404_SST1_ADDRESS_HYGX                    (UINT32)((NBIO0_SST_FCH_APERTURE_ID_HYGX << 20) + 0x00404)

// Type
#define SION_WRAPPER_404_SST0_TYPE                           TYPE_SMN
#define SION_WRAPPER_404_SST1_TYPE                           TYPE_SMN

#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK0_OFFSET  0
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK0_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK0_MASK    0x1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK1_OFFSET  1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK1_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK1_MASK    0x2
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK2_OFFSET  2
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK2_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK2_MASK    0x4
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK3_OFFSET  3
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK3_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK3_MASK    0x8
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_OFFSET  4
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_MASK    0x10
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK5_OFFSET  5
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK5_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK5_MASK    0x20
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK6_OFFSET  6
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK6_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK6_MASK    0x40
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK7_OFFSET  7
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK7_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK7_MASK    0x80
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK8_OFFSET  8
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK8_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK8_MASK    0x100
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK9_OFFSET  9
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK9_WIDTH   1
#define SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK9_MASK    0x200
#define SION_WRAPPER_404_Reserved1_OFFSET                                         10
#define SION_WRAPPER_404_Reserved1_WIDTH                                          6
#define SION_WRAPPER_404_Reserved1_MASK                                           0xfc00
#define SION_WRAPPER_404_Reserved_25_16_OFFSET                                    16
#define SION_WRAPPER_404_Reserved_25_16_WIDTH                                     10
#define SION_WRAPPER_404_Reserved_25_16_MASK                                      0x3ff0000
#define SION_WRAPPER_404_Reserved0_OFFSET                                         26
#define SION_WRAPPER_404_Reserved0_WIDTH                                          6
#define SION_WRAPPER_404_Reserved0_MASK                                           0xfc000000L

/// SST0_00000404
typedef union {
  struct {
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK0 : 1;  ///<
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK1 : 1;  ///<
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK2 : 1;  ///<
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK3 : 1;  ///<
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4 : 1;  ///<
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK5 : 1;  ///<
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK6 : 1;  ///<
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK7 : 1;  ///<
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK8 : 1;  ///<
    UINT32    CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK9 : 1;  ///<
    UINT32    Reserved1                                        : 6;  ///<
    UINT32    Reserved_25_16                                   : 10; ///<
    UINT32    Reserved0                                        : 6;  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} SION_WRAPPER_404_STRUCT;

// rsmu_fusestrap_pcie0_define
#define STRAP_BIF_AER_EN_ADDR                             0xFFFE0002
#define STRAP_BIF_DPC_EN_ADDR                             0xFFFE000F
#define STRAP_BIF_ALWAYS_USE_FAST_TXCLK_ADDR              0xFFFE0030
#define STRAP_BIF_ACS_EN_ADDR                             0xFFFE0068
#define STRAP_BIF_ACS_SOURCE_VALIDATION_ADDR              0xFFFE0069
#define STRAP_BIF_ACS_TRANSLATION_BLOCKING_ADDR           0xFFFE006A
#define STRAP_BIF_ACS_DIRECT_TRANSLATED_P2P_ADDR          0xFFFE006B
#define STRAP_BIF_ACS_P2P_COMPLETION_REDIRECT_ADDR        0xFFFE006C
#define STRAP_BIF_ACS_P2P_REQUEST_REDIRECT_ADDR           0xFFFE006D
#define STRAP_BIF_ACS_UPSTREAM_FORWARDING_ADDR            0xFFFE006E
#define STRAP_BIF_SDP_UNIT_ID_ADDR                        0xFFFE006F
#define STRAP_BIF_SUBSYS_VEN_ID_ADDR                      0xFFFE0082
#define STRAP_BIF_SUBSYS_ID_ADDR                          0xFFFE0083
#define STRAP_BIF_LINK_CONFIG_ADDR                        0xFFFE0084
#define STRAP_CHIP_BIF_MODE_ADDR                          0xFFFE0086
#define STRAP_BIF_PCIE_LANE_EQ_DN_PORT_RX_PRESET_ADDR     0xFFFE0088
#define STRAP_BIF_PCIE_LANE_EQ_UP_PORT_RX_PRESET_ADDR     0xFFFE0089
#define STRAP_BIF_PCIE_LANE_EQ_DN_PORT_TX_PRESET_ADDR     0xFFFE008A
#define STRAP_BIF_PCIE_LANE_EQ_UP_PORT_TX_PRESET_ADDR     0xFFFE008B
#define STRAP_BIF_MAX_PAYLOAD_SUPPORT_A_ADDR              0xFFFE00DB
#define STRAP_BIF_MAX_PAYLOAD_SUPPORT_B_ADDR              0xFFFE012C
#define STRAP_BIF_MAX_PAYLOAD_SUPPORT_C_ADDR              0xFFFE017D
#define STRAP_BIF_MAX_PAYLOAD_SUPPORT_D_ADDR              0xFFFE01CE
#define STRAP_BIF_MAX_PAYLOAD_SUPPORT_E_ADDR              0xFFFE021F
#define STRAP_BIF_MAX_PAYLOAD_SUPPORT_F_ADDR              0xFFFE0270
#define STRAP_BIF_MAX_PAYLOAD_SUPPORT_G_ADDR              0xFFFE02C1
#define STRAP_BIF_MAX_PAYLOAD_SUPPORT_H_ADDR              0xFFFE0312
#define STRAP_BIF_PORT_OFFSET_A_ADDR                      0xFFFE0335
#define STRAP_BIF_PORT_OFFSET_B_ADDR                      0xFFFE0336
#define STRAP_BIF_PORT_OFFSET_C_ADDR                      0xFFFE0337
#define STRAP_BIF_PORT_OFFSET_D_ADDR                      0xFFFE0338
#define STRAP_BIF_PORT_OFFSET_E_ADDR                      0xFFFE0339
#define STRAP_BIF_PORT_OFFSET_F_ADDR                      0xFFFE033A
#define STRAP_BIF_PORT_OFFSET_G_ADDR                      0xFFFE033B
#define STRAP_BIF_PORT_OFFSET_H_ADDR                      0xFFFE033C
#define STRAP_BIF_LC_PORT_ORDER_EN_ADDR                   0xFFFE033D
#define STRAP_BIF_DLF_EN_ADDR                             0xFFFE033E
#define STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_A_ADDR  0xFFFE0353
#define STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_A_ADDR  0xFFFE0354
#define STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_B_ADDR  0xFFFE0363
#define STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_B_ADDR  0xFFFE0364
#define STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_C_ADDR  0xFFFE0373
#define STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_C_ADDR  0xFFFE0374
#define STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_D_ADDR  0xFFFE0383
#define STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_D_ADDR  0xFFFE0384
#define STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_E_ADDR  0xFFFE0393
#define STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_E_ADDR  0xFFFE0394
#define STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_F_ADDR  0xFFFE03A3
#define STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_F_ADDR  0xFFFE03A4
#define STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_G_ADDR  0xFFFE03B3
#define STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_G_ADDR  0xFFFE03B4
#define STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_H_ADDR  0xFFFE03C3
#define STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_H_ADDR  0xFFFE03C4
#define STRAP_BIF_LINK_ACTIVATION_SUPPORTED_A_ADDR        0xFFFE0355
#define STRAP_BIF_LOCAL_DLF_SUPPORTED_A_ADDR              0xFFFE0356
#define STRAP_BIF_DLF_EXCHANGE_EN_A_ADDR                  0xFFFE0357
#define STRAP_BIF_DLF_EXCHANGE_EN_B_ADDR                  0xFFFE0367
#define STRAP_BIF_DLF_EXCHANGE_EN_C_ADDR                  0xFFFE0377
#define STRAP_BIF_DLF_EXCHANGE_EN_D_ADDR                  0xFFFE0387
#define STRAP_BIF_DLF_EXCHANGE_EN_E_ADDR                  0xFFFE0397
#define STRAP_BIF_DLF_EXCHANGE_EN_F_ADDR                  0xFFFE03A7
#define STRAP_BIF_DLF_EXCHANGE_EN_G_ADDR                  0xFFFE03B7
#define STRAP_BIF_DLF_EXCHANGE_EN_H_ADDR                  0xFFFE03C7
#define STRAP_BIF_LANE_EQUALIZATION_CNTL_DSP_16GT_TX_PRESET_ADDR 0xFFFE0347
#define STRAP_BIF_LANE_EQUALIZATION_CNTL_USP_16GT_TX_PRESET_ADDR 0xFFFE0348
#define STRAP_BIF_LANE_EQUALIZATION_CNTL_DSP_32GT_TX_PRESET_ADDR 0xFFFE03CC
#define STRAP_BIF_LANE_EQUALIZATION_CNTL_USP_32GT_TX_PRESET_ADDR 0xFFFE03CD

//HyEx NBIF Device Control Register
#define NBIF0_NTB_STRAP0_ADDRESS_HYEX                              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34200)
#define NBIF0_FUNC2_STRAP0_ADDRESS_HYEX                            (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34400)
#define NBIF0_USB_STRAP0_ADDRESS_HYEX                              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34600)

#define NBIF1_FUNC1_STRAP0_ADDRESS_HYEX                            (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34200)
#define NBIF1_SATA_STRAP0_ADDRESS_HYEX                             (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34400)
#define NBIF1_SATA_STRAP3_ADDRESS_HYEX                             (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x3440C)
#define NBIF1_SATA_STRAP13_ADDRESS_HYEX                            (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34434)
#define NBIF1_HDAUDIO_STRAP0_ADDRESS_HYEX                          (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34600)
#define NBIF1_XGBE0_STRAP0_ADDRESS_HYEX                            (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34800)
#define NBIF1_XGBE1_STRAP0_ADDRESS_HYEX                            (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34A00)
#define NBIF1_XGBE2_STRAP0_ADDRESS_HYEX                            (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34C00)
#define NBIF1_XGBE3_STRAP0_ADDRESS_HYEX                            (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34E00)

#define NBIF2_NTB_STRAP0_ADDRESS_HYEX                              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34200)

//HyGx NBIF Device Control Register
//Address
#define NBIF0_NTB_STRAP0_ADDRESS_HYGX                              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34200)

//PSPCCP NTBCCP ASP
#define NBIF0_FUNC2_STRAP0_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34400)
#define NBIF0_FUNC3_STRAP0_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34600)
#define NBIF2_ASP_STRAP0_ADDRESS_HYGX                              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34600)
#define NBIF2_ASP_STRAP3_ADDRESS_HYGX                              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x3460C)

#define NBIF2_FUNC4_STRAP0_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34800)
#define NBIF3_FUNC4_STRAP0_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34800)

#define NBIF2_FUNC4_STRAP2_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34808)
#define NBIF3_FUNC4_STRAP2_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34808)

#define NBIF2_FUNC4_STRAP6_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34818)
#define NBIF3_FUNC4_STRAP6_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34818)

#define NBIF2_USB_STRAP0_ADDRESS_HYGX                              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34200)
#define NBIF3_USB_STRAP0_ADDRESS_HYGX                              (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34200)

#define NBIF2_SATA_STRAP0_ADDRESS_HYGX                             (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34400)
#define NBIF3_SATA_STRAP0_ADDRESS_HYGX                             (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34400)
#define NBIF2_SATA_STRAP3_ADDRESS_HYGX                             (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x3440C)
#define NBIF3_SATA_STRAP3_ADDRESS_HYGX                             (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x3440C)
#define NBIF2_SATA_STRAP13_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34434)
#define NBIF3_SATA_STRAP13_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34434)

#define NBIF3_HDAUDIO_STRAP0_ADDRESS_HYGX                          (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34600)
#define NBIF3_HDAUDIO_STRAP3_ADDRESS_HYGX                          (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x3460C)

#define NBIF2_XGBE0_STRAP0_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34800)
#define NBIF2_XGBE1_STRAP0_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34A00)
#define NBIF2_XGBE2_STRAP0_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34C00)
#define NBIF2_XGBE3_STRAP0_ADDRESS_HYGX                            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34E00)

// Type
#define NBIF_STRAP0_TYPE                                 TYPE_SMN

#define NBIFSTRAP0_DEVICE_ID_DEV0_F0_OFFSET              0
#define NBIFSTRAP0_DEVICE_ID_DEV0_F0_WIDTH               16
#define NBIFSTRAP0_DEVICE_ID_DEV0_F0_MASK                0xffff
#define NBIFSTRAP0_MAJOR_REV_ID_DEV0_F0_OFFSET           16
#define NBIFSTRAP0_MAJOR_REV_ID_DEV0_F0_WIDTH            4
#define NBIFSTRAP0_MAJOR_REV_ID_DEV0_F0_MASK             0xf0000
#define NBIFSTRAP0_MINOR_REV_ID_DEV0_F0_OFFSET           20
#define NBIFSTRAP0_MINOR_REV_ID_DEV0_F0_WIDTH            4
#define NBIFSTRAP0_MINOR_REV_ID_DEV0_F0_MASK             0xf00000
#define NBIFSTRAP0_ATI_REV_ID_DEV0_F0_OFFSET             24
#define NBIFSTRAP0_ATI_REV_ID_DEV0_F0_WIDTH              4
#define NBIFSTRAP0_ATI_REV_ID_DEV0_F0_MASK               0xf000000
#define NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET                28
#define NBIFSTRAP0_FUNC_EN_DEV0_F0_WIDTH                 1
#define NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK                  0x10000000
#define NBIFSTRAP0_LEGACY_DEVICE_TYPE_EN_DEV0_F0_OFFSET  29
#define NBIFSTRAP0_LEGACY_DEVICE_TYPE_EN_DEV0_F0_WIDTH   1
#define NBIFSTRAP0_LEGACY_DEVICE_TYPE_EN_DEV0_F0_MASK    0x20000000
#define NBIFSTRAP0_D1_SUPPORT_DEV0_F0_OFFSET             30
#define NBIFSTRAP0_D1_SUPPORT_DEV0_F0_WIDTH              1
#define NBIFSTRAP0_D1_SUPPORT_DEV0_F0_MASK               0x40000000
#define NBIFSTRAP0_D2_SUPPORT_DEV0_F0_OFFSET             31
#define NBIFSTRAP0_D2_SUPPORT_DEV0_F0_WIDTH              1
#define NBIFSTRAP0_D2_SUPPORT_DEV0_F0_MASK               0x80000000

/// NBIFSTRAP0
typedef union {
  struct {
    UINT32    STRAP_DEVICE_ID_DEV0_F0             : 16;                ///<
    UINT32    STRAP_MAJOR_REV_ID_DEV0_F0          : 4;                 ///<
    UINT32    STRAP_MINOR_REV_ID_DEV0_F0          : 4;                 ///<
    UINT32    STRAP_ATI_REV_ID_DEV0_F0            : 4;                 ///<
    UINT32    STRAP_FUNC_EN_DEV0_F0               : 1;                 ///<
    UINT32    STRAP_LEGACY_DEVICE_TYPE_EN_DEV0_F0 : 1;                 ///<
    UINT32    STRAP_D1_SUPPORT_DEV0_F0            : 1;                 ///<
    UINT32    STRAP_D2_SUPPORT_DEV0_F0            : 1;                 ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIFSTRAP0_STRUCT;

/// NBIFSTRAP2
typedef union {
  struct {
    UINT32                                   STRAP_SRIOV_EN_DEV0_F4 : 1;
    UINT32                            STRAP_SRIOV_TOTAL_VFS_DEV0_F4 : 7;
    UINT32                              STRAP_NO_SOFT_RESET_DEV0_F4 : 1;
    UINT32                              STRAP_RESIZE_BAR_EN_DEV0_F4 : 1;
    UINT32                                                          : 5;
    UINT32                     STRAP_MSI_PERVECTOR_MASK_CAP_DEV0_F4 : 1;
    UINT32                                                          : 1;
    UINT32                                     STRAP_AER_EN_DEV0_F4 : 1;
    UINT32                                     STRAP_ACS_EN_DEV0_F4 : 1;
    UINT32                                                          : 1;
    UINT32                           STRAP_CPL_ABORT_ERR_EN_DEV0_F4 : 1;
    UINT32                                     STRAP_DPA_EN_DEV0_F4 : 1;
    UINT32                                                          : 1;
    UINT32                                      STRAP_VC_EN_DEV0_F4 : 1;
    UINT32                              STRAP_MSI_MULTI_CAP_DEV0_F4 : 3;
    UINT32                                                          : 5;
  } Field;                                                         ///<
  UINT32 Value;                                                    ///<
} NBIF_FUN4_STRAP2_STRUCT;

/// NBIFSTRAP6
typedef union {
  struct {
    UINT32                                 STRAP_APER0_EN_DEV0_F4 : 1;
    UINT32                    STRAP_APER0_PREFETCHABLE_EN_DEV0_F4 : 1;
    UINT32                                                        : 6;
    UINT32                                 STRAP_APER1_EN_DEV0_F4 : 1;
    UINT32                    STRAP_APER1_PREFETCHABLE_EN_DEV0_F4 : 1;
    UINT32                                                        : 6;
    UINT32                                 STRAP_APER2_EN_DEV0_F4 : 1;
    UINT32                    STRAP_APER2_PREFETCHABLE_EN_DEV0_F4 : 1;
    UINT32                                                        : 14;
  } Field;                                                         ///<
  UINT32 Value;                                                    ///<
} NBIF_FUN4_STRAP6_STRUCT;

// Address
#define PCIE_CONFIG_CNTL_PCIECORE0_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80044)
#define PCIE_CONFIG_CNTL_PCIECORE1_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80044)
#define PCIE_CONFIG_CNTL_PCIECORE2_ADDRESS_HYEX                    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80044)

#define PCIE_CONFIG_CNTL_PCIECORE0_ADDRESS_HYGX                    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80044)
#define PCIE_CONFIG_CNTL_PCIECORE2_ADDRESS_HYGX                    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80044)

// Type
#define PCIE_CONFIG_CNTL_TYPE                                      TYPE_SMN

#define PCIE_CONFIG_CNTL_DYN_CLK_LATENCY_OFFSET                     0
#define PCIE_CONFIG_CNTL_DYN_CLK_LATENCY_WIDTH                      4
#define PCIE_CONFIG_CNTL_DYN_CLK_LATENCY_MASK                       0xf
#define PCIE_CONFIG_CNTL_Reserved_7_4_OFFSET                        4
#define PCIE_CONFIG_CNTL_Reserved_7_4_WIDTH                         4
#define PCIE_CONFIG_CNTL_Reserved_7_4_MASK                          0xf0
#define PCIE_CONFIG_CNTL_CI_SWUS_MAX_PAYLOAD_SIZE_MODE_OFFSET       8
#define PCIE_CONFIG_CNTL_CI_SWUS_MAX_PAYLOAD_SIZE_MODE_WIDTH        1
#define PCIE_CONFIG_CNTL_CI_SWUS_MAX_PAYLOAD_SIZE_MODE_MASK         0x100
#define PCIE_CONFIG_CNTL_CI_SWUS_PRIV_MAX_PAYLOAD_SIZE_OFFSET       9
#define PCIE_CONFIG_CNTL_CI_SWUS_PRIV_MAX_PAYLOAD_SIZE_WIDTH        2
#define PCIE_CONFIG_CNTL_CI_SWUS_PRIV_MAX_PAYLOAD_SIZE_MASK         0x600
#define PCIE_CONFIG_CNTL_CI_10BIT_TAG_EN_OVERRIDE_OFFSET            11
#define PCIE_CONFIG_CNTL_CI_10BIT_TAG_EN_OVERRIDE_WIDTH             2
#define PCIE_CONFIG_CNTL_CI_10BIT_TAG_EN_OVERRIDE_MASK              0x1800
#define PCIE_CONFIG_CNTL_CI_SWUS_10BIT_TAG_EN_OVERRIDE_OFFSET       13
#define PCIE_CONFIG_CNTL_CI_SWUS_10BIT_TAG_EN_OVERRIDE_WIDTH        2
#define PCIE_CONFIG_CNTL_CI_SWUS_10BIT_TAG_EN_OVERRIDE_MASK         0x6000
#define PCIE_CONFIG_CNTL_CI_MAX_PAYLOAD_SIZE_MODE_OFFSET            16
#define PCIE_CONFIG_CNTL_CI_MAX_PAYLOAD_SIZE_MODE_WIDTH             1
#define PCIE_CONFIG_CNTL_CI_MAX_PAYLOAD_SIZE_MODE_MASK              0x10000
#define PCIE_CONFIG_CNTL_CI_PRIV_MAX_PAYLOAD_SIZE_OFFSET            17
#define PCIE_CONFIG_CNTL_CI_PRIV_MAX_PAYLOAD_SIZE_WIDTH             3
#define PCIE_CONFIG_CNTL_CI_PRIV_MAX_PAYLOAD_SIZE_MASK              0xe0000
#define PCIE_CONFIG_CNTL_CI_MAX_READ_REQUEST_SIZE_MODE_OFFSET       20
#define PCIE_CONFIG_CNTL_CI_MAX_READ_REQUEST_SIZE_MODE_WIDTH        1
#define PCIE_CONFIG_CNTL_CI_MAX_READ_REQUEST_SIZE_MODE_MASK         0x100000
#define PCIE_CONFIG_CNTL_CI_PRIV_MAX_READ_REQUEST_SIZE_OFFSET       21
#define PCIE_CONFIG_CNTL_CI_PRIV_MAX_READ_REQUEST_SIZE_WIDTH        3
#define PCIE_CONFIG_CNTL_CI_PRIV_MAX_READ_REQUEST_SIZE_MASK         0xe00000
#define PCIE_CONFIG_CNTL_CI_MAX_READ_SAFE_MODE_OFFSET               24
#define PCIE_CONFIG_CNTL_CI_MAX_READ_SAFE_MODE_WIDTH                1
#define PCIE_CONFIG_CNTL_CI_MAX_READ_SAFE_MODE_MASK                 0x1000000
#define PCIE_CONFIG_CNTL_CI_EXTENDED_TAG_EN_OVERRIDE_OFFSET         25
#define PCIE_CONFIG_CNTL_CI_EXTENDED_TAG_EN_OVERRIDE_WIDTH          2
#define PCIE_CONFIG_CNTL_CI_EXTENDED_TAG_EN_OVERRIDE_MASK           0x6000000
#define PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_MODE_OFFSET  27
#define PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_MODE_WIDTH   1
#define PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_MODE_MASK    0x8000000
#define PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_PRIV_OFFSET  28
#define PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_PRIV_WIDTH   2
#define PCIE_CONFIG_CNTL_CI_SWUS_MAX_READ_REQUEST_SIZE_PRIV_MASK    0x30000000
#define PCIE_CONFIG_CNTL_CI_SWUS_EXTENDED_TAG_EN_OVERRIDE_OFFSET    31
#define PCIE_CONFIG_CNTL_CI_SWUS_EXTENDED_TAG_EN_OVERRIDE_WIDTH     2
#define PCIE_CONFIG_CNTL_CI_SWUS_EXTENDED_TAG_EN_OVERRIDE_MASK      0xc0000000

/// PCIE_CONFIG_CNTL
typedef union {
  struct {
    UINT32    DYN_CLK_LATENCY                    : 4;                  ///<
    UINT32    Reserved_7_4                       : 4;                  ///<
    UINT32    CI_SWUS_MAX_PAYLOAD_SIZE_MODE      : 1;                  ///< 8
    UINT32    CI_SWUS_PRIV_MAX_PAYLOAD_SIZE      : 2;                  ///< 9-10
    UINT32    CI_10BIT_TAG_EN_OVERRIDE           : 2;                  ///< 11-12
    UINT32    CI_SWUS_10BIT_TAG_EN_OVERRIDE      : 2;                  ///< 13-14
    UINT32    Reserved_15_15                     : 1;                  ///< 15
    UINT32    CI_MAX_PAYLOAD_SIZE_MODE           : 1;                  ///< 16
    UINT32    CI_PRIV_MAX_PAYLOAD_SIZE           : 3;                  ///< 17-19
    UINT32    CI_MAX_READ_REQUEST_SIZE_MODE      : 1;                  ///< 20
    UINT32    CI_PRIV_MAX_READ_REQUEST_SIZE      : 3;                  ///< 21-23
    UINT32    CI_MAX_READ_SAFE_MODE              : 1;                  ///< 24
    UINT32    CI_EXTENDED_TAG_EN_OVERRIDE        : 2;                  ///< 25-26
    UINT32    CI_SWUS_MAX_READ_REQUEST_SIZE_MODE : 1;                  ///< 27
    UINT32    CI_SWUS_MAX_READ_REQUEST_SIZE_PRIV : 2;                  ///< 28-29
    UINT32    CI_SWUS_EXTENDED_TAG_EN_OVERRIDE   : 2;                  ///< 30-31
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_CONFIG_CNTL_STRUCT;

// Address
#define PCIE_DEBUG_CNTL_PCIECORE0_ADDRESS_HYGX                     (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80048)
#define PCIE_DEBUG_CNTL_PCIECORE2_ADDRESS_HYGX                     (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80048)
                    
// Type
#define PCIE_DEBUG_CNTL_TYPE                                       TYPE_SMN

#define PCIE_DEBUG_CNTL_DEBUG_PORT_EN_OFFSET                       0
#define PCIE_DEBUG_CNTL_DEBUG_PORT_EN_WIDTH                        8
#define PCIE_DEBUG_CNTL_DEBUG_PORT_EN_MASK                         0xff
#define PCIE_DEBUG_CNTL_DEBUG_SELECT_OFFSET                        8
#define PCIE_DEBUG_CNTL_DEBUG_SELECT_WIDTH                         1
#define PCIE_DEBUG_CNTL_DEBUG_SELECT_MASK                          0x100
#define PCIE_DEBUG_CNTL_PCIE_DEBUG_LOGIC_DISABLE_OFFSET            9
#define PCIE_DEBUG_CNTL_PCIE_DEBUG_LOGIC_DISABLE_WIDTH             1
#define PCIE_DEBUG_CNTL_PCIE_DEBUG_LOGIC_DISABLE_MASK              0x200
#define PCIE_DEBUG_CNTL_Reserved_15_10_OFFSET                      10
#define PCIE_DEBUG_CNTL_Reserved_15_10_WIDTH                       6
#define PCIE_DEBUG_CNTL_Reserved_15_10_MASK                        0xfc00
#define PCIE_DEBUG_CNTL_DEBUG_LANE_EN_OFFSET                       16
#define PCIE_DEBUG_CNTL_DEBUG_LANE_EN_WIDTH                        16
#define PCIE_DEBUG_CNTL_DEBUG_LANE_EN_MASK                         0xffff0000L

// Address
#define CXL_CLK_CNTL_PCIECORE1_ADDRESS                      (UINT32)((NBIO0_CXL_APERTURE_ID_HYGX << 20) + 0x0F004)

// Type
#define CXL_CLK_CNTL_PCIECORE1_TYPE                         TYPE_SMN

#define CXL_CLK_CNTL_LCLK_GATE_ENABLE_OFFSET                0
#define CXL_CLK_CNTL_LCLK_GATE_ENABLE_WIDTH                 1
#define CXL_CLK_CNTL_LCLK_GATE_ENABLE_MASK                  0x1
#define CXL_CLK_CNTL_PCLK_GATE_ENABLE_OFFSET                1
#define CXL_CLK_CNTL_PCLK_GATE_ENABLE_WIDTH                 1
#define CXL_CLK_CNTL_PCLK_GATE_ENABLE_MASK                  0x2
#define CXL_CLK_CNTL_FCLK_GATE_ENABLE_OFFSET                2
#define CXL_CLK_CNTL_FCLK_GATE_ENABLE_WIDTH                 1
#define CXL_CLK_CNTL_FCLK_GATE_ENABLE_MASK                  0x4
#define CXL_CLK_CNTL_Reserved_3_OFFSET                      3
#define CXL_CLK_CNTL_Reserved_3_WIDTH                       1
#define CXL_CLK_CNTL_Reserved_3_MASK                        0x8
#define CXL_CLK_CNTL_RADM_CLK_GATE_ENABLE_OFFSET            4
#define CXL_CLK_CNTL_RADM_CLK_GATE_ENABLE_WIDTH             1
#define CXL_CLK_CNTL_RADM_CLK_GATE_ENABLE_MASK              0x10
#define CXL_CLK_CNTL_Reserved_29_5_OFFSET                   5
#define CXL_CLK_CNTL_Reserved_29_5_WIDTH                    25
#define CXL_CLK_CNTL_Reserved_29_5_MASK                     0x3fffffe0
#define CXL_CLK_CNTL_CXL_LINKDOWN_PCS_RST_EN_OFFSET         30
#define CXL_CLK_CNTL_CXL_LINKDOWN_PCS_RST_EN_WIDTH          1
#define CXL_CLK_CNTL_CXL_LINKDOWN_PCS_RST_EN_MASK           0x40000000
#define CXL_CLK_CNTL_MAXPCLK_SEL_EN_OFFSET                  31
#define CXL_CLK_CNTL_MAXPCLK_SEL_EN_WIDTH                   1
#define CXL_CLK_CNTL_MAXPCLK_SEL_EN_MASK                    0x80000000L

// Address
#define SWRST_COMMAND_STATUS_PCIECORE0_ADDRESS_HYEX         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80400)
#define SWRST_COMMAND_STATUS_PCIECORE1_ADDRESS_HYEX         (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80400)
#define SWRST_COMMAND_STATUS_PCIECORE2_ADDRESS_HYEX         (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80400)

#define SWRST_COMMAND_STATUS_PCIECORE0_ADDRESS_HYGX         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80400)
#define SWRST_COMMAND_STATUS_PCIECORE2_ADDRESS_HYGX         (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80400)

#define SWRST_COMMAND_STATUS_RECONFIGURE_OFFSET            0
#define SWRST_COMMAND_STATUS_RECONFIGURE_WIDTH             1
#define SWRST_COMMAND_STATUS_RECONFIGURE_MASK              0x1
#define SWRST_COMMAND_STATUS_RESET_COMPLETE_OFFSET         16
#define SWRST_COMMAND_STATUS_RESET_COMPLETE_WIDTH          1
#define SWRST_COMMAND_STATUS_RESET_COMPLETE_MASK           0x10000

#define SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80404)
#define SWRST_GENERAL_CONTROL_PCIECORE1_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80404)
#define SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80404)

#define SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80404)
#define SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80404)

#define SWRST_GENERAL_CONTROL_RECONFIGURE_EN_OFFSET        0
#define SWRST_GENERAL_CONTROL_RECONFIGURE_EN_WIDTH         1
#define SWRST_GENERAL_CONTROL_RECONFIGURE_EN_MASK          0x1
#define SWRST_GENERAL_CONTROL_CONFIG_XFER_MODE_OFFSET      12
#define SWRST_GENERAL_CONTROL_CONFIG_XFER_MODE_WIDTH       1
#define SWRST_GENERAL_CONTROL_CONFIG_XFER_MODE_MASK        0x1000
#define SWRST_GENERAL_CONTROL_BYPASS_PCS_HOLD_OFFSET       17
#define SWRST_GENERAL_CONTROL_BYPASS_PCS_HOLD_WIDTH        1
#define SWRST_GENERAL_CONTROL_BYPASS_PCS_HOLD_MASK         0x20000

#define SWRST_CONTROL_6_PCIECORE0_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80428)
#define SWRST_CONTROL_6_PCIECORE1_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80428)
#define SWRST_CONTROL_6_PCIECORE2_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80428)

#define SWRST_CONTROL_6_PCIECORE0_ADDRESS_HYGX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80428)
#define SWRST_CONTROL_6_PCIECORE2_ADDRESS_HYGX             (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80428)

// Address
#define CPM_CONTROL_PCIECORE0_ADDRESS_HYEX                 (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80460)
#define CPM_CONTROL_PCIECORE1_ADDRESS_HYEX                 (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80460)
#define CPM_CONTROL_PCIECORE2_ADDRESS_HYEX                 (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80460)

#define CPM_CONTROL_PCIECORE0_ADDRESS_HYGX                 (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80460)
#define CPM_CONTROL_PCIECORE2_ADDRESS_HYGX                 (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80460)

// Type
#define CPM_CONTROL_TYPE                                TYPE_SMN

#define CPM_CONTROL_LCLK_DYN_GATE_ENABLE_OFFSET         0
#define CPM_CONTROL_LCLK_DYN_GATE_ENABLE_WIDTH          1
#define CPM_CONTROL_LCLK_DYN_GATE_ENABLE_MASK           0x1
#define CPM_CONTROL_TXCLK_DYN_GATE_ENABLE_OFFSET        1
#define CPM_CONTROL_TXCLK_DYN_GATE_ENABLE_WIDTH         1
#define CPM_CONTROL_TXCLK_DYN_GATE_ENABLE_MASK          0x2
#define CPM_CONTROL_TXCLK_PERM_GATE_ENABLE_OFFSET       2
#define CPM_CONTROL_TXCLK_PERM_GATE_ENABLE_WIDTH        1
#define CPM_CONTROL_TXCLK_PERM_GATE_ENABLE_MASK         0x4
#define CPM_CONTROL_Reserved_4_3_OFFSET                 3
#define CPM_CONTROL_Reserved_4_3_WIDTH                  2
#define CPM_CONTROL_Reserved_4_3_MASK                   0x18
#define CPM_CONTROL_TXCLK_LCNT_GATE_ENABLE_OFFSET       5
#define CPM_CONTROL_TXCLK_LCNT_GATE_ENABLE_WIDTH        1
#define CPM_CONTROL_TXCLK_LCNT_GATE_ENABLE_MASK         0x20
#define CPM_CONTROL_TXCLK_REGS_GATE_ENABLE_OFFSET       6
#define CPM_CONTROL_TXCLK_REGS_GATE_ENABLE_WIDTH        1
#define CPM_CONTROL_TXCLK_REGS_GATE_ENABLE_MASK         0x40
#define CPM_CONTROL_TXCLK_PRBS_GATE_ENABLE_OFFSET       7
#define CPM_CONTROL_TXCLK_PRBS_GATE_ENABLE_WIDTH        1
#define CPM_CONTROL_TXCLK_PRBS_GATE_ENABLE_MASK         0x80
#define CPM_CONTROL_REFCLK_REGS_GATE_ENABLE_OFFSET      8
#define CPM_CONTROL_REFCLK_REGS_GATE_ENABLE_WIDTH       1
#define CPM_CONTROL_REFCLK_REGS_GATE_ENABLE_MASK        0x100
#define CPM_CONTROL_LCLK_DYN_GATE_LATENCY_OFFSET        9
#define CPM_CONTROL_LCLK_DYN_GATE_LATENCY_WIDTH         1
#define CPM_CONTROL_LCLK_DYN_GATE_LATENCY_MASK          0x200
#define CPM_CONTROL_TXCLK_DYN_GATE_LATENCY_OFFSET       10
#define CPM_CONTROL_TXCLK_DYN_GATE_LATENCY_WIDTH        1
#define CPM_CONTROL_TXCLK_DYN_GATE_LATENCY_MASK         0x400
#define CPM_CONTROL_TXCLK_PERM_GATE_LATENCY_OFFSET      11
#define CPM_CONTROL_TXCLK_PERM_GATE_LATENCY_WIDTH       1
#define CPM_CONTROL_TXCLK_PERM_GATE_LATENCY_MASK        0x800
#define CPM_CONTROL_TXCLK_REGS_GATE_LATENCY_OFFSET      12
#define CPM_CONTROL_TXCLK_REGS_GATE_LATENCY_WIDTH       1
#define CPM_CONTROL_TXCLK_REGS_GATE_LATENCY_MASK        0x1000
#define CPM_CONTROL_REFCLK_REGS_GATE_LATENCY_OFFSET     13
#define CPM_CONTROL_REFCLK_REGS_GATE_LATENCY_WIDTH      1
#define CPM_CONTROL_REFCLK_REGS_GATE_LATENCY_MASK       0x2000
#define CPM_CONTROL_LCLK_GATE_TXCLK_FREE_OFFSET         14
#define CPM_CONTROL_LCLK_GATE_TXCLK_FREE_WIDTH          1
#define CPM_CONTROL_LCLK_GATE_TXCLK_FREE_MASK           0x4000
#define CPM_CONTROL_RCVR_DET_CLK_ENABLE_OFFSET          15
#define CPM_CONTROL_RCVR_DET_CLK_ENABLE_WIDTH           1
#define CPM_CONTROL_RCVR_DET_CLK_ENABLE_MASK            0x8000
#define CPM_CONTROL_Reserved_16_OFFSET                  16
#define CPM_CONTROL_Reserved_16_WIDTH                   1
#define CPM_CONTROL_Reserved_16_MASK                    0x10000
#define CPM_CONTROL_FAST_TXCLK_LATENCY_OFFSET           17
#define CPM_CONTROL_FAST_TXCLK_LATENCY_WIDTH            3
#define CPM_CONTROL_FAST_TXCLK_LATENCY_MASK             0xe0000
#define CPM_CONTROL_Reserved_21_20_OFFSET               20
#define CPM_CONTROL_Reserved_21_20_WIDTH                2
#define CPM_CONTROL_Reserved_21_20_MASK                 0x300000
#define CPM_CONTROL_REFCLK_XSTCLK_ENABLE_OFFSET         22
#define CPM_CONTROL_REFCLK_XSTCLK_ENABLE_WIDTH          1
#define CPM_CONTROL_REFCLK_XSTCLK_ENABLE_MASK           0x400000
#define CPM_CONTROL_REFCLK_XSTCLK_LATENCY_OFFSET        23
#define CPM_CONTROL_REFCLK_XSTCLK_LATENCY_WIDTH         1
#define CPM_CONTROL_REFCLK_XSTCLK_LATENCY_MASK          0x800000
#define CPM_CONTROL_CLKREQb_UNGATE_TXCLK_ENABLE_OFFSET  24
#define CPM_CONTROL_CLKREQb_UNGATE_TXCLK_ENABLE_WIDTH   1
#define CPM_CONTROL_CLKREQb_UNGATE_TXCLK_ENABLE_MASK    0x1000000
#define CPM_CONTROL_TXCLK_RXP_GATE_ENABLE_OFFSET        25
#define CPM_CONTROL_TXCLK_RXP_GATE_ENABLE_WIDTH         2
#define CPM_CONTROL_TXCLK_RXP_GATE_ENABLE_MASK          0x6000000
#define CPM_CONTROL_TXCLK_PI_GATE_ENABLE_OFFSET         27
#define CPM_CONTROL_TXCLK_PI_GATE_ENABLE_WIDTH          2
#define CPM_CONTROL_TXCLK_PI_GATE_ENABLE_MASK           0x18000000
#define CPM_CONTROL_SPARE_REGS_OFFSET                   29
#define CPM_CONTROL_SPARE_REGS_WIDTH                    3
#define CPM_CONTROL_SPARE_REGS_MASK                     0xe0000000L

/// CPM_CONTROL
typedef union {
  struct {
    UINT32    LCLK_DYN_GATE_ENABLE        : 1;                         ///<
    UINT32    TXCLK_DYN_GATE_ENABLE       : 1;                         ///<
    UINT32    TXCLK_PERM_GATE_ENABLE      : 1;                         ///<
    UINT32    Reserved_4_3                : 2;                         ///<
    UINT32    TXCLK_LCNT_GATE_ENABLE      : 1;                         ///<
    UINT32    TXCLK_REGS_GATE_ENABLE      : 1;                         ///<
    UINT32    TXCLK_PRBS_GATE_ENABLE      : 1;                         ///<
    UINT32    REFCLK_REGS_GATE_ENABLE     : 1;                         ///<
    UINT32    LCLK_DYN_GATE_LATENCY       : 1;                         ///<
    UINT32    TXCLK_DYN_GATE_LATENCY      : 1;                         ///<
    UINT32    TXCLK_PERM_GATE_LATENCY     : 1;                         ///<
    UINT32    TXCLK_REGS_GATE_LATENCY     : 1;                         ///<
    UINT32    REFCLK_REGS_GATE_LATENCY    : 1;                         ///<
    UINT32    LCLK_GATE_TXCLK_FREE        : 1;                         ///<
    UINT32    RCVR_DET_CLK_ENABLE         : 1;                         ///<
    UINT32    Reserved_16                 : 1;                         ///<
    UINT32    FAST_TXCLK_LATENCY          : 3;                         ///<
    UINT32    Reserved_21_20              : 2;                         ///<
    UINT32    REFCLK_XSTCLK_ENABLE        : 1;                         ///<
    UINT32    REFCLK_XSTCLK_LATENCY       : 1;                         ///<
    UINT32    CLKREQb_UNGATE_TXCLK_ENABLE : 1;                         ///<
    UINT32    TXCLK_RXP_GATE_ENABLE       : 2;                         ///< Only HyGx support
    UINT32    TXCLK_PI_GATE_ENABLE        : 2;                         ///< Only HyGx support
    UINT32    SPARE_REGS                  : 3;                         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} CPM_CONTROL_STRUCT;

// Address
#define LC_CPM_CONTROL_0_PCIECORE0_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x804CC)
#define LC_CPM_CONTROL_0_PCIECORE2_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x804CC)     

// Type
#define LC_CPM_CONTROL_0_TYPE                                 TYPE_SMN

#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_A_GATE_ENABLE_OFFSET           0
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_A_GATE_ENABLE_WIDTH            1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_A_GATE_ENABLE_MASK             0x1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_B_GATE_ENABLE_OFFSET           1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_B_GATE_ENABLE_WIDTH            1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_B_GATE_ENABLE_MASK             0x2
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_C_GATE_ENABLE_OFFSET           2
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_C_GATE_ENABLE_WIDTH            1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_C_GATE_ENABLE_MASK             0x4
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_D_GATE_ENABLE_OFFSET           3
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_D_GATE_ENABLE_WIDTH            1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_D_GATE_ENABLE_MASK             0x8
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_E_GATE_ENABLE_OFFSET           4
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_E_GATE_ENABLE_WIDTH            1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_E_GATE_ENABLE_MASK             0x10
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_F_GATE_ENABLE_OFFSET           5
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_F_GATE_ENABLE_WIDTH            1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_F_GATE_ENABLE_MASK             0x20
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_G_GATE_ENABLE_OFFSET           6
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_G_GATE_ENABLE_WIDTH            1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_G_GATE_ENABLE_MASK             0x40
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_H_GATE_ENABLE_OFFSET           7
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_H_GATE_ENABLE_WIDTH            1
#define LC_CPM_CONTROL_0_TXCLK_DYN_PORT_H_GATE_ENABLE_MASK             0x80
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_A_GATE_ENABLE_OFFSET        8
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_A_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_A_GATE_ENABLE_MASK          0x100
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_B_GATE_ENABLE_OFFSET        9
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_B_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_B_GATE_ENABLE_MASK          0x200
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_C_GATE_ENABLE_OFFSET        10
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_C_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_C_GATE_ENABLE_MASK          0x400
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_D_GATE_ENABLE_OFFSET        11
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_D_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_D_GATE_ENABLE_MASK          0x800
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_E_GATE_ENABLE_OFFSET        12
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_E_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_E_GATE_ENABLE_MASK          0x1000
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_F_GATE_ENABLE_OFFSET        13
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_F_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_F_GATE_ENABLE_MASK          0x2000
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_G_GATE_ENABLE_OFFSET        14
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_G_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_G_GATE_ENABLE_MASK          0x4000
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_H_GATE_ENABLE_OFFSET        15
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_H_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_L0_PORT_H_GATE_ENABLE_MASK          0x8000
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_A_GATE_ENABLE_OFFSET        16
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_A_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_A_GATE_ENABLE_MASK          0x10000
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_B_GATE_ENABLE_OFFSET        17
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_B_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_B_GATE_ENABLE_MASK          0x20000
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_C_GATE_ENABLE_OFFSET        18
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_C_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_C_GATE_ENABLE_MASK          0x40000
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_D_GATE_ENABLE_OFFSET        19
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_D_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_D_GATE_ENABLE_MASK          0x80000
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_E_GATE_ENABLE_OFFSET        20
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_E_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_E_GATE_ENABLE_MASK          0x100000
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_F_GATE_ENABLE_OFFSET        21
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_F_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_F_GATE_ENABLE_MASK          0x200000
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_G_GATE_ENABLE_OFFSET        22
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_G_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_G_GATE_ENABLE_MASK          0x400000
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_H_GATE_ENABLE_OFFSET        23
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_H_GATE_ENABLE_WIDTH         1
#define LC_CPM_CONTROL_0_TXCLK_DYN_TR_PORT_H_GATE_ENABLE_MASK          0x800000
#define LC_CPM_CONTROL_0_Reserved_31_24_OFFSET                         24
#define LC_CPM_CONTROL_0_Reserved_31_24_WIDTH                          8
#define LC_CPM_CONTROL_0_Reserved_31_24_MASK                           0xFF000000L

// Address
#define CPM_CONTROL_2_PCIECORE0_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x804E0)
#define CPM_CONTROL_2_PCIECORE2_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x804E0)              

// Type
#define CPM_CONTROL_2_TYPE                              TYPE_SMN

#define CPM_CONTROL_2_L1_PWR_GATE_ENABLE_OFFSET                     0
#define CPM_CONTROL_2_L1_PWR_GATE_ENABLE_WIDTH                      1
#define CPM_CONTROL_2_L1_PWR_GATE_ENABLE_MASK                       0x1
#define CPM_CONTROL_2_L1_1_PWR_GATE_ENABLE_OFFSET                   1
#define CPM_CONTROL_2_L1_1_PWR_GATE_ENABLE_WIDTH                    1
#define CPM_CONTROL_2_L1_1_PWR_GATE_ENABLE_MASK                     0x2
#define CPM_CONTROL_2_L1_2_PWR_GATE_ENABLE_OFFSET                   2
#define CPM_CONTROL_2_L1_2_PWR_GATE_ENABLE_WIDTH                    1
#define CPM_CONTROL_2_L1_2_PWR_GATE_ENABLE_MASK                     0x4
#define CPM_CONTROL_2_REFCLKREQ_REFCLKACK_LOOPBACK_ENABLE_OFFSET    3
#define CPM_CONTROL_2_REFCLKREQ_REFCLKACK_LOOPBACK_ENABLE_WIDTH     1
#define CPM_CONTROL_2_REFCLKREQ_REFCLKACK_LOOPBACK_ENABLE_MASK      0x8
#define CPM_CONTROL_2_IGNORE_REGS_IDLE_IN_PG_OFFSET                 4
#define CPM_CONTROL_2_IGNORE_REGS_IDLE_IN_PG_WIDTH                  1
#define CPM_CONTROL_2_IGNORE_REGS_IDLE_IN_PG_MASK                   0x10
#define CPM_CONTROL_2_Reserved_14_5_OFFSET                          5
#define CPM_CONTROL_2_Reserved_14_5_WIDTH                           10
#define CPM_CONTROL_2_Reserved_14_5_MASK                            0x7FE0
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ALL_OFFSET                    15
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ALL_WIDTH                     1
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ALL_MASK                      0x8000
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_A_OFFSET               16
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_A_WIDTH                1
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_A_MASK                 0x10000
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_B_OFFSET               17
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_B_WIDTH                1
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_B_MASK                 0x20000
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_C_OFFSET               18
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_C_WIDTH                1
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_C_MASK                 0x40000
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_D_OFFSET               19
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_D_WIDTH                1
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_D_MASK                 0x80000
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_E_OFFSET               20
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_E_WIDTH                1
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_E_MASK                 0x100000
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_F_OFFSET               21
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_F_WIDTH                1
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_F_MASK                 0x200000
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_G_OFFSET               22
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_G_WIDTH                1
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_G_MASK                 0x400000
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_H_OFFSET               23
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_H_WIDTH                1
#define CPM_CONTROL_2_TXCLK_LCLK_GATE_ENABLE_H_MASK                 0x800000
#define CPM_CONTROL_2_Reserved_24_OFFSET                            24
#define CPM_CONTROL_2_Reserved_24_WIDTH                             1
#define CPM_CONTROL_2_Reserved_24_MASK                              0x1000000
#define CPM_CONTROL_2_LCLK_GATE_ALLOW_IN_L1_OFFSET                  25
#define CPM_CONTROL_2_LCLK_GATE_ALLOW_IN_L1_WIDTH                   1
#define CPM_CONTROL_2_LCLK_GATE_ALLOW_IN_L1_MASK                    0x2000000
#define CPM_CONTROL_2_PG_EARLY_WAKE_ENABLE_OFFSET                   26
#define CPM_CONTROL_2_PG_EARLY_WAKE_ENABLE_WIDTH                    1
#define CPM_CONTROL_2_PG_EARLY_WAKE_ENABLE_MASK                     0x4000000
#define CPM_CONTROL_2_PCIE_CORE_IDLE_OFFSET                         27
#define CPM_CONTROL_2_PCIE_CORE_IDLE_WIDTH                          1
#define CPM_CONTROL_2_PCIE_CORE_IDLE_MASK                           0x8000000
#define CPM_CONTROL_2_PCIE_LINK_IDLE_OFFSET                         28
#define CPM_CONTROL_2_PCIE_LINK_IDLE_WIDTH                          1
#define CPM_CONTROL_2_PCIE_LINK_IDLE_MASK                           0x10000000
#define CPM_CONTROL_2_PCIE_BUFFER_EMPTY_OFFSET                      29
#define CPM_CONTROL_2_PCIE_BUFFER_EMPTY_WIDTH                       1
#define CPM_CONTROL_2_PCIE_BUFFER_EMPTY_MASK                        0x20000000
#define CPM_CONTROL_2_REGS_IDLE_TO_PG_LATENCY_OFFSET                30
#define CPM_CONTROL_2_REGS_IDLE_TO_PG_LATENCY_WIDTH                 2
#define CPM_CONTROL_2_REGS_IDLE_TO_PG_LATENCY_MASK                  0xC0000000L  

// Address
#define PCIE_SDP_CTRL_PCIECORE0_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x8018C)
#define PCIE_SDP_CTRL_PCIECORE1_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x8018C)
#define PCIE_SDP_CTRL_PCIECORE2_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x8018C)

#define PCIE_SDP_CTRL_PCIECORE0_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x8018C)
#define PCIE_SDP_CTRL_PCIECORE2_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x8018C)

// Type
#define PCIE_SDP_CTRL_TYPE                                                     TYPE_SMN

#define PCIE_SDP_CTRL_SDP_UNIT_ID_OFFSET                                        0
#define PCIE_SDP_CTRL_SDP_UNIT_ID_WIDTH                                         4
#define PCIE_SDP_CTRL_SDP_UNIT_ID_MASK                                          0xf
#define PCIE_SDP_CTRL_CI_SLV_REQR_FULL_DISCONNECT_EN_OFFSET                     4
#define PCIE_SDP_CTRL_CI_SLV_REQR_FULL_DISCONNECT_EN_WIDTH                      1
#define PCIE_SDP_CTRL_CI_SLV_REQR_FULL_DISCONNECT_EN_MASK                       0x10
#define PCIE_SDP_CTRL_CI_SLV_REQR_PART_DISCONNECT_EN_OFFSET                     5
#define PCIE_SDP_CTRL_CI_SLV_REQR_PART_DISCONNECT_EN_WIDTH                      1
#define PCIE_SDP_CTRL_CI_SLV_REQR_PART_DISCONNECT_EN_MASK                       0x20
#define PCIE_SDP_CTRL_CI_MSTSDP_CLKGATE_ONESIDED_ENABLE_OFFSET                  6
#define PCIE_SDP_CTRL_CI_MSTSDP_CLKGATE_ONESIDED_ENABLE_WIDTH                   1
#define PCIE_SDP_CTRL_CI_MSTSDP_CLKGATE_ONESIDED_ENABLE_MASK                    0x40
#define PCIE_SDP_CTRL_TX_RC_TPH_PRIV_DIS_OFFSET                                 7
#define PCIE_SDP_CTRL_TX_RC_TPH_PRIV_DIS_WIDTH                                  1
#define PCIE_SDP_CTRL_TX_RC_TPH_PRIV_DIS_MASK                                   0x80
#define PCIE_SDP_CTRL_TX_SWUS_TPH_PRIV_DIS_OFFSET                               8
#define PCIE_SDP_CTRL_TX_SWUS_TPH_PRIV_DIS_WIDTH                                1
#define PCIE_SDP_CTRL_TX_SWUS_TPH_PRIV_DIS_MASK                                 0x100
#define PCIE_SDP_CTRL_CI_SLAVE_TAG_STEALING_DIS_OFFSET                          9
#define PCIE_SDP_CTRL_CI_SLAVE_TAG_STEALING_DIS_WIDTH                           1
#define PCIE_SDP_CTRL_CI_SLAVE_TAG_STEALING_DIS_MASK                            0x200
#define PCIE_SDP_CTRL_SLAVE_PREFIX_PRELOAD_DIS_OFFSET                           10
#define PCIE_SDP_CTRL_SLAVE_PREFIX_PRELOAD_DIS_WIDTH                            1
#define PCIE_SDP_CTRL_SLAVE_PREFIX_PRELOAD_DIS_MASK                             0x400
#define PCIE_SDP_CTRL_CI_DISABLE_LTR_DROPPING_OFFSET                            11
#define PCIE_SDP_CTRL_CI_DISABLE_LTR_DROPPING_WIDTH                             1
#define PCIE_SDP_CTRL_CI_DISABLE_LTR_DROPPING_MASK                              0x800
#define PCIE_SDP_CTRL_RX_SWUS_SIDEBAND_CPLHDR_DIS_OFFSET                        12
#define PCIE_SDP_CTRL_RX_SWUS_SIDEBAND_CPLHDR_DIS_WIDTH                         1
#define PCIE_SDP_CTRL_RX_SWUS_SIDEBAND_CPLHDR_DIS_MASK                          0x1000
#define PCIE_SDP_CTRL_CI_MST_MEMR_RD_NONCONT_BE_EN_OFFSET                       13
#define PCIE_SDP_CTRL_CI_MST_MEMR_RD_NONCONT_BE_EN_WIDTH                        1
#define PCIE_SDP_CTRL_CI_MST_MEMR_RD_NONCONT_BE_EN_MASK                         0x2000
#define PCIE_SDP_CTRL_CI_MSTSDP_DISCONNECT_RSP_ON_PARTIAL_OFFSET                14
#define PCIE_SDP_CTRL_CI_MSTSDP_DISCONNECT_RSP_ON_PARTIAL_WIDTH                 1
#define PCIE_SDP_CTRL_CI_MSTSDP_DISCONNECT_RSP_ON_PARTIAL_MASK                  0x4000
#define PCIE_SDP_CTRL_CI_SWUS_RCVD_ERR_HANDLING_DIS_OFFSET                      15
#define PCIE_SDP_CTRL_CI_SWUS_RCVD_ERR_HANDLING_DIS_WIDTH                       1
#define PCIE_SDP_CTRL_CI_SWUS_RCVD_ERR_HANDLING_DIS_MASK                        0x8000
#define PCIE_SDP_CTRL_EARLY_HW_WAKE_UP_EN_OFFSET                                16
#define PCIE_SDP_CTRL_EARLY_HW_WAKE_UP_EN_WIDTH                                 1
#define PCIE_SDP_CTRL_EARLY_HW_WAKE_UP_EN_MASK                                  0x10000
#define PCIE_SDP_CTRL_SLV_SDP_DISCONNECT_WHEN_IN_L1_EN_OFFSET                   17
#define PCIE_SDP_CTRL_SLV_SDP_DISCONNECT_WHEN_IN_L1_EN_WIDTH                    1
#define PCIE_SDP_CTRL_SLV_SDP_DISCONNECT_WHEN_IN_L1_EN_MASK                     0x20000
#define PCIE_SDP_CTRL_BLOCK_SLV_SDP_DISCONNECT_WHEN_EARLY_HW_WAKE_UP_EN_OFFSET  18
#define PCIE_SDP_CTRL_BLOCK_SLV_SDP_DISCONNECT_WHEN_EARLY_HW_WAKE_UP_EN_WIDTH   1
#define PCIE_SDP_CTRL_BLOCK_SLV_SDP_DISCONNECT_WHEN_EARLY_HW_WAKE_UP_EN_MASK    0x40000
#define PCIE_SDP_CTRL_TX_ENCMSG_USE_SDP_EP_DIS_OFFSET                           19
#define PCIE_SDP_CTRL_TX_ENCMSG_USE_SDP_EP_DIS_WIDTH                            1
#define PCIE_SDP_CTRL_TX_ENCMSG_USE_SDP_EP_DIS_MASK                             0x80000
#define PCIE_SDP_CTRL_TX_IGNORE_POISON_BIT_EN_OFFSET                            20
#define PCIE_SDP_CTRL_TX_IGNORE_POISON_BIT_EN_WIDTH                             1
#define PCIE_SDP_CTRL_TX_IGNORE_POISON_BIT_EN_MASK                              0x100000
#define PCIE_SDP_CTRL_TX_RBUF_WRITE_2HDR_DIS_OFFSET                             21
#define PCIE_SDP_CTRL_TX_RBUF_WRITE_2HDR_DIS_WIDTH                              1
#define PCIE_SDP_CTRL_TX_RBUF_WRITE_2HDR_DIS_MASK                               0x200000
#define PCIE_SDP_CTRL_TX_RBUF_READ_2HDR_DIS_OFFSET                              22
#define PCIE_SDP_CTRL_TX_RBUF_READ_2HDR_DIS_WIDTH                               1
#define PCIE_SDP_CTRL_TX_RBUF_READ_2HDR_DIS_MASK                                0x400000
#define PCIE_SDP_CTRL_TX_RBUF_END_TLP2_DIS_OFFSET                               23
#define PCIE_SDP_CTRL_TX_RBUF_END_TLP2_DIS_WIDTH                                1
#define PCIE_SDP_CTRL_TX_RBUF_END_TLP2_DIS_MASK                                 0x800000
#define PCIE_SDP_CTRL_TX_MULTICYCLE_DLLP_DIS_OFFSET                             24
#define PCIE_SDP_CTRL_TX_MULTICYCLE_DLLP_DIS_WIDTH                              1
#define PCIE_SDP_CTRL_TX_MULTICYCLE_DLLP_DIS_MASK                               0x1000000
#define PCIE_SDP_CTRL_CI_VIRTUAL_WIRE_MODE_OFFSET                               25
#define PCIE_SDP_CTRL_CI_VIRTUAL_WIRE_MODE_WIDTH                                1
#define PCIE_SDP_CTRL_CI_VIRTUAL_WIRE_MODE_MASK                                 0x2000000
#define PCIE_SDP_CTRL_SDP_UNIT_ID_LOWER_OFFSET                                  26
#define PCIE_SDP_CTRL_SDP_UNIT_ID_LOWER_WIDTH                                   3
#define PCIE_SDP_CTRL_SDP_UNIT_ID_LOWER_MASK                                    0x1c000000
/// PCIE_SDP_CTRL
typedef union {
  struct {
    UINT32    SDP_UNIT_ID                                       : 4;  ///<
    UINT32    CI_SLV_REQR_FULL_DISCONNECT_EN                    : 1;  ///<
    UINT32    CI_SLV_REQR_PART_DISCONNECT_EN                    : 1;  ///<
    UINT32    CI_MSTSDP_CLKGATE_ONESIDED_ENABLE                 : 1;  ///<
    UINT32    TX_RC_TPH_PRIV_DIS                                : 1;  ///<
    UINT32    TX_SWUS_TPH_PRIV_DIS                              : 1;  ///<
    UINT32    CI_SLAVE_TAG_STEALING_DIS                         : 1;  ///<
    UINT32    SLAVE_PREFIX_PRELOAD_DIS                          : 1;  ///<
    UINT32    CI_DISABLE_LTR_DROPPING                           : 1;  ///<
    UINT32    RX_SWUS_SIDEBAND_CPLHDR_DIS                       : 1;  ///<
    UINT32    CI_MST_MEMR_RD_NONCONT_BE_EN                      : 1;  ///<
    UINT32    CI_MSTSDP_DISCONNECT_RSP_ON_PARTIAL               : 1;  ///<
    UINT32    CI_SWUS_RCVD_ERR_HANDLING_DIS                     : 1;  ///<
    UINT32    EARLY_HW_WAKE_UP_EN                               : 1;  ///<
    UINT32    SLV_SDP_DISCONNECT_WHEN_IN_L1_EN                  : 1;  ///<
    UINT32    BLOCK_SLV_SDP_DISCONNECT_WHEN_EARLY_HW_WAKE_UP_EN : 1;  ///<
    UINT32    TX_ENCMSG_USE_SDP_EP_DIS                          : 1;  ///<
    UINT32    TX_IGNORE_POISON_BIT_EN                           : 1;  ///<
    UINT32    TX_RBUF_WRITE_2HDR_DIS                            : 1;  ///<
    UINT32    TX_RBUF_READ_2HDR_DIS                             : 1;  ///<
    UINT32    TX_RBUF_END_TLP2_DIS                              : 1;  ///<
    UINT32    TX_MULTICYCLE_DLLP_DIS                            : 1;  ///<
    UINT32    CI_VIRTUAL_WIRE_MODE                              : 1;  ///<
    UINT32    SDP_UNIT_ID_LOWER                                 : 3;  ///<
    UINT32    Reserved_31_29                                    : 3;  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_SDP_CTRL_STRUCT;

// Address
#define PCIE_FC_P_ADDRESS_HYEX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40180)
#define PCIE_FC_P_ADDRESS_HYGX        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40180)

// Type
#define PCIE_FC_P_TYPE               TYPE_SMN

#define PCIE_FC_P_PD_CREDITS_OFFSET  0
#define PCIE_FC_P_PD_CREDITS_WIDTH   12
#define PCIE_FC_P_PD_CREDITS_MASK    0xfff
#define PCIE_FC_P_PH_CREDITS_OFFSET  16
#define PCIE_FC_P_PH_CREDITS_WIDTH   12
#define PCIE_FC_P_PH_CREDITS_MASK    0xfff0000

/// PCIEPORT0F0_00000180
typedef union {
  struct {
    UINT32    PD_CREDITS     : 12;                                     ///<
    UINT32    Reserved_15_12 : 4;                                      ///<
    UINT32    PH_CREDITS     : 12;                                     ///<
    UINT32    Reserved_31_28 : 4;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_FC_P_STRUCT;

// Address
#define PCIE_FC_NP_ADDRESS_HYEX         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40184)
#define PCIE_FC_NP_ADDRESS_HYGX         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40184)

// Type
#define PCIE_FC_NP_TYPE                TYPE_SMN

#define PCIE_FC_NP_NPD_CREDITS_OFFSET  0
#define PCIE_FC_NP_NPD_CREDITS_WIDTH   12
#define PCIE_FC_NP_NPD_CREDITS_MASK    0xfff
#define PCIE_FC_NP_NPH_CREDITS_OFFSET  16
#define PCIE_FC_NP_NPH_CREDITS_WIDTH   12
#define PCIE_FC_NP_NPH_CREDITS_MASK    0xfff0000

/// PCIEPORT0F0_00000184
typedef union {
  struct {
    UINT32    NPD_CREDITS    : 12;                                     ///<
    UINT32    Reserved_15_12 : 4;                                      ///<
    UINT32    NPH_CREDITS    : 12;                                     ///<
    UINT32    Reserved_31_28 : 4;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_FC_NP_STRUCT;

// Address
#define PCIE_FC_CPL_ADDRESS_HYEX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40188)
#define PCIE_FC_CPL_ADDRESS_HYGX          (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40188)

// Type
#define PCIE_FC_CPL_TYPE                 TYPE_SMN

#define PCIE_FC_CPL_CPLD_CREDITS_OFFSET  0
#define PCIE_FC_CPL_CPLD_CREDITS_WIDTH   12
#define PCIE_FC_CPL_CPLD_CREDITS_MASK    0xfff
#define PCIE_FC_CPL_CPLH_CREDITS_OFFSET  16
#define PCIE_FC_CPL_CPLH_CREDITS_WIDTH   12
#define PCIE_FC_CPL_CPLH_CREDITS_MASK    0xfff0000

/// PCIEPORT0F0_00000188
typedef union {
  struct {
    UINT32    CPLD_CREDITS   : 12;                                     ///<
    UINT32    Reserved_15_12 : 4;                                      ///<
    UINT32    CPLH_CREDITS   : 12;                                     ///<
    UINT32    Reserved_31_28 : 4;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_FC_CPL_STRUCT;

// Address
#define PCIE0_LANE_EQUALIZATION_CNTL_ADDRESS_HYEX      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x0027C)
#define PCIE0_LANE_EQUALIZATION_CNTL_ADDRESS_HYGX      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x0027C)

// Type
#define PCIE_LANE_EQUALIZATION_CNTL_TYPE                                   TYPE_SMN

#define PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_TX_PRESET_OFFSET       0
#define PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_TX_PRESET_WIDTH        4
#define PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_TX_PRESET_MASK         0xf
#define PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_RX_PRESET_HINT_OFFSET  4
#define PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_RX_PRESET_HINT_WIDTH   3
#define PCIE_LANE_EQUALIZATION_CNTL_DOWNSTREAM_PORT_RX_PRESET_HINT_MASK    0x70
#define PCIE_LANE_EQUALIZATION_CNTL_Reserved_7_7_OFFSET                    7
#define PCIE_LANE_EQUALIZATION_CNTL_Reserved_7_7_WIDTH                     1
#define PCIE_LANE_EQUALIZATION_CNTL_Reserved_7_7_MASK                      0x80
#define PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_TX_PRESET_OFFSET         8
#define PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_TX_PRESET_WIDTH          4
#define PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_TX_PRESET_MASK           0xf00
#define PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_RX_PRESET_HINT_OFFSET    12
#define PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_RX_PRESET_HINT_WIDTH     3
#define PCIE_LANE_EQUALIZATION_CNTL_UPSTREAM_PORT_RX_PRESET_HINT_MASK      0x7000
#define PCIE_LANE_EQUALIZATION_CNTL_Reserved_15_15_OFFSET                  15
#define PCIE_LANE_EQUALIZATION_CNTL_Reserved_15_15_WIDTH                   1
#define PCIE_LANE_EQUALIZATION_CNTL_Reserved_15_15_MASK                    0x8000

/// PCIE_LANE_EQUALIZATION_CNTL
typedef union {
  struct {
    UINT16    DOWNSTREAM_PORT_TX_PRESET      : 4;                      ///<
    UINT16    DOWNSTREAM_PORT_RX_PRESET_HINT : 3;                      ///<
    UINT16    Reserved_7_7                   : 1;                      ///<
    UINT16    UPSTREAM_PORT_TX_PRESET        : 4;                      ///<
    UINT16    UPSTREAM_PORT_RX_PRESET_HINT   : 3;                      ///<
    UINT16    Reserved_15_15                 : 1;                      ///<
  } Field;                                                             ///<
  UINT16    Value;                                                     ///<
} PCIE_LANE_EQUALIZATION_CNTL_STRUCT;

// Address
#define PCIE0_NBIO_CLKREQb_MAP_CNTL_ADDRESS_HYEX              (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80190)
#define PCIE1_NBIO_CLKREQb_MAP_CNTL_ADDRESS_HYEX              (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80190)

#define PCIE0_NBIO_CLKREQb_MAP_CNTL_ADDRESS_HYGX              (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80190)

// Type
#define NBIO_CLKREQb_MAP_CNTL_TYPE                           TYPE_SMN

#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_0_MAP_OFFSET      0
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_0_MAP_WIDTH       5
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_0_MAP_MASK        0x1f
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_1_MAP_OFFSET      5
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_1_MAP_WIDTH       5
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_1_MAP_MASK        0x3e0
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_2_MAP_OFFSET      10
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_2_MAP_WIDTH       5
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_2_MAP_MASK        0x7c00
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_3_MAP_OFFSET      15
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_3_MAP_WIDTH       5
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_3_MAP_MASK        0xf8000
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_4_MAP_OFFSET      20
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_4_MAP_WIDTH       5
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_4_MAP_MASK        0x1f00000
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_CNTL_MASK_OFFSET  31
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_CNTL_MASK_WIDTH   1
#define NBIO_CLKREQb_MAP_CNTL_PCIE_CLKREQB_CNTL_MASK_MASK    0x80000000

/// PCIECORE0_00000190
typedef union {
  struct {
    UINT32    PCIE_CLKREQB_0_MAP     : 5;                              ///<
    UINT32    PCIE_CLKREQB_1_MAP     : 5;                              ///<
    UINT32    PCIE_CLKREQB_2_MAP     : 5;                              ///<
    UINT32    PCIE_CLKREQB_3_MAP     : 5;                              ///<
    UINT32    PCIE_CLKREQB_4_MAP     : 5;                              ///<
    UINT32    Reserved_30_25         : 6;                              ///<
    UINT32    PCIE_CLKREQB_CNTL_MASK : 1;                              ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIO_CLKREQb_MAP_CNTL_STRUCT;

// Address
#define NBIF0_DN_PCIE_CNTL_SMN_ADDRESS_HYEX       (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x2358c)
#define NBIF1_DN_PCIE_CNTL_SMN_ADDRESS_HYEX       (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x2358c)
#define NBIF2_DN_PCIE_CNTL_SMN_ADDRESS_HYEX       (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x2358c)

#define NBIF0_DN_PCIE_CNTL_SMN_ADDRESS_HYGX       (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x2358c)
#define NBIF1_DN_PCIE_CNTL_SMN_ADDRESS_HYGX       (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x2358c)
#define NBIF2_DN_PCIE_CNTL_SMN_ADDRESS_HYGX       (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x2358c)
#define NBIF3_DN_PCIE_CNTL_SMN_ADDRESS_HYGX       (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x2358c)
// Type
#define DN_PCIE_CNTL_TYPE                         TYPE_SMN

#define DN_PCIE_CNTL_HWINIT_WR_LOCK_OFFSET        0
#define DN_PCIE_CNTL_HWINIT_WR_LOCK_WIDTH         1
#define DN_PCIE_CNTL_HWINIT_WR_LOCK_MASK          0x1
#define DN_PCIE_CNTL_Reserved_6_1_OFFSET          1
#define DN_PCIE_CNTL_Reserved_6_1_WIDTH           6
#define DN_PCIE_CNTL_Reserved_6_1_MASK            0x7e
#define DN_PCIE_CNTL_UR_ERR_REPORT_DIS_DN_OFFSET  7
#define DN_PCIE_CNTL_UR_ERR_REPORT_DIS_DN_WIDTH   1
#define DN_PCIE_CNTL_UR_ERR_REPORT_DIS_DN_MASK    0x80
#define DN_PCIE_CNTL_Reserved_29_8_OFFSET         8
#define DN_PCIE_CNTL_Reserved_29_8_WIDTH          22
#define DN_PCIE_CNTL_Reserved_29_8_MASK           0x3fffff00
#define DN_PCIE_CNTL_RX_IGNORE_LTR_MSG_UR_OFFSET  30
#define DN_PCIE_CNTL_RX_IGNORE_LTR_MSG_UR_WIDTH   1
#define DN_PCIE_CNTL_RX_IGNORE_LTR_MSG_UR_MASK    0x40000000
#define DN_PCIE_CNTL_Reserved_31_31_OFFSET        31
#define DN_PCIE_CNTL_Reserved_31_31_WIDTH         1
#define DN_PCIE_CNTL_Reserved_31_31_MASK          0x80000000L

/// NBIFRCC0_0000358C
typedef union {
  struct {
    UINT32    HWINIT_WR_LOCK       : 1;                                ///<
    UINT32    Reserved_6_1         : 6;                                ///<
    UINT32    UR_ERR_REPORT_DIS_DN : 1;                                ///<
    UINT32    Reserved_29_8        : 22;                               ///<
    UINT32    RX_IGNORE_LTR_MSG_UR : 1;                                ///<
    UINT32    Reserved_31_31       : 1;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} DN_PCIE_CNTL_STRUCT;

// Address
#define IOMMU_MMIO_CONTROL0_W_HYEX         (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x00080)
#define IOMMUL2_MMIO_CONTROL0_W_HYEX       (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX << 20) + 0x04330)
#define IOMMU_MMIO_CNTRL_1_HYEX            (UINT32)((NBIO0_L2IMU0_MMIO_APERTURE_ID_HYEX << 20) + 0x0001C)
#define IOMMUL2_MMIO_CNTRL_1_HYEX          (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYEX << 20) + 0x0401C)

#define IOMMU_MMIO_CONTROL0_W_HYGX         (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x00080)
#define IOMMUL2_MMIO_CONTROL0_W_HYGX       (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX << 20) + 0x04330)
#define IOMMU_MMIO_CNTRL_1_HYGX            (UINT32)((NBIO0_L2IMU0_MMIO_APERTURE_ID_HYGX << 20) + 0x0001C)
#define IOMMUL2_MMIO_CNTRL_1_HYGX          (UINT32)((NBIO0_L2IMU0_L2A_APERTURE_ID_HYGX << 20) + 0x0401C)

// Address
#define NBIF0_MGCG_CTRL_ADDRESS_HYEX       (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x3A030)
#define NBIF1_MGCG_CTRL_ADDRESS_HYEX       (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x3A030)
#define NBIF2_MGCG_CTRL_ADDRESS_HYEX       (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x3A030)

#define NBIF0_MGCG_CTRL_ADDRESS_HYGX       (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x3A030)
#define NBIF1_MGCG_CTRL_ADDRESS_HYGX       (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x3A030)
#define NBIF2_MGCG_CTRL_ADDRESS_HYGX       (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x3A030)
#define NBIF3_MGCG_CTRL_ADDRESS_HYGX       (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x3A030)

// Register Name RCC_BIF_STRAP0
// Address
#define NBIF0_SMN_VWR_VCHG_DIS_CTRL_1_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x3A058)
#define NBIF1_SMN_VWR_VCHG_DIS_CTRL_1_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x3A058)
#define NBIF2_SMN_VWR_VCHG_DIS_CTRL_1_ADDRESS_HYEX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x3A058)

#define NBIF0_SMN_VWR_VCHG_DIS_CTRL_1_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x3A058)
#define NBIF1_SMN_VWR_VCHG_DIS_CTRL_1_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x3A058)
#define NBIF2_SMN_VWR_VCHG_DIS_CTRL_1_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x3A044)
#define NBIF3_SMN_VWR_VCHG_DIS_CTRL_1_ADDRESS_HYGX   (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x3A044)

// Address
#define SYSHUBMM0_NGDC_MGCG_CTRL_ADDRESS_HYEX        0x01403b80
#define SYSHUBMM1_NGDC_MGCG_CTRL_ADDRESS_HYEX        0x01803b80

#define SYSHUBMM0_NGDC_MGCG_CTRL_ADDRESS_HYGX        0x04C03b80
#define SYSHUBMM2_NGDC_MGCG_CTRL_ADDRESS_HYGX        0x05003b80
#define SYSHUBMM3_NGDC_MGCG_CTRL_ADDRESS_HYGX        0x05403b80
// Type
#define MGCG_CTRL_TYPE              TYPE_SMN

#define MGCG_EN_OFFSET              0
#define MGCG_EN_WIDTH               1
#define MGCG_EN_MASK                0x1
#define MGCG_MODE_OFFSET            1
#define MGCG_MODE_WIDTH             1
#define MGCG_MODE_MASK              0x2
#define MGCG_HYSTERESIS_OFFSET      2
#define MGCG_HYSTERESIS_WIDTH       8
#define MGCG_HYSTERESIS_MASK        0x3fc
#define MGCG_Reserved_31_10_OFFSET  10
#define MGCG_Reserved_31_10_WIDTH   22
#define MGCG_Reserved_31_10_MASK    0xfffffc00L

typedef union {
  struct {
    UINT32    MGCG_EN         : 1;                                     ///<
    UINT32    MGCG_MODE       : 1;                                     ///<
    UINT32    MGCG_HYSTERESIS : 8;                                     ///<
    UINT32    Reserved_31_10  : 22;                                    ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} MGCG_CTRL_STRUCT;

// Register Name RCC_DEV0_EPF3_STRAP3

// Address
#define RCC_DEV0_EPF3_STRAP3_ADDRESS_HYEX           (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x3460C)
#define RCC_DEV0_EPF3_STRAP3_ADDRESS_HYGX           (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x3460C)

// Type
#define RCC_DEV0_EPF3_STRAP3_TYPE                  TYPE_SMN

#define RCC_DEV0_EPF3_STRAP3_STRAP_POISONED_ADVISORY_NONFATAL_DEV0_F3_OFFSET  0
#define RCC_DEV0_EPF3_STRAP3_STRAP_POISONED_ADVISORY_NONFATAL_DEV0_F3_WIDTH   1
#define RCC_DEV0_EPF3_STRAP3_STRAP_POISONED_ADVISORY_NONFATAL_DEV0_F3_MASK    0x1
#define RCC_DEV0_EPF3_STRAP3_STRAP_PWR_EN_DEV0_F3_OFFSET                      1
#define RCC_DEV0_EPF3_STRAP3_STRAP_PWR_EN_DEV0_F3_WIDTH                       1
#define RCC_DEV0_EPF3_STRAP3_STRAP_PWR_EN_DEV0_F3_MASK                        0x2
#define RCC_DEV0_EPF3_STRAP3_STRAP_SUBSYS_ID_DEV0_F3_OFFSET                   2
#define RCC_DEV0_EPF3_STRAP3_STRAP_SUBSYS_ID_DEV0_F3_WIDTH                    16
#define RCC_DEV0_EPF3_STRAP3_STRAP_SUBSYS_ID_DEV0_F3_MASK                     0x3fffc
#define RCC_DEV0_EPF3_STRAP3_STRAP_MSI_EN_DEV0_F3_OFFSET                      18
#define RCC_DEV0_EPF3_STRAP3_STRAP_MSI_EN_DEV0_F3_WIDTH                       1
#define RCC_DEV0_EPF3_STRAP3_STRAP_MSI_EN_DEV0_F3_MASK                        0x40000
#define RCC_DEV0_EPF3_STRAP3_STRAP_MSI_CLR_PENDING_EN_DEV0_F3_OFFSET          19
#define RCC_DEV0_EPF3_STRAP3_STRAP_MSI_CLR_PENDING_EN_DEV0_F3_WIDTH           1
#define RCC_DEV0_EPF3_STRAP3_STRAP_MSI_CLR_PENDING_EN_DEV0_F3_MASK            0x80000
#define RCC_DEV0_EPF3_STRAP3_STRAP_MSIX_EN_DEV0_F3_OFFSET                     20
#define RCC_DEV0_EPF3_STRAP3_STRAP_MSIX_EN_DEV0_F3_WIDTH                      1
#define RCC_DEV0_EPF3_STRAP3_STRAP_MSIX_EN_DEV0_F3_MASK                       0x100000
#define RCC_DEV0_EPF3_STRAP3_Reserved_23_21_OFFSET                            21
#define RCC_DEV0_EPF3_STRAP3_Reserved_23_21_WIDTH                             3
#define RCC_DEV0_EPF3_STRAP3_Reserved_23_21_MASK                              0xe00000
#define RCC_DEV0_EPF3_STRAP3_STRAP_PMC_DSI_DEV0_F3_OFFSET                     24
#define RCC_DEV0_EPF3_STRAP3_STRAP_PMC_DSI_DEV0_F3_WIDTH                      1
#define RCC_DEV0_EPF3_STRAP3_STRAP_PMC_DSI_DEV0_F3_MASK                       0x1000000
#define RCC_DEV0_EPF3_STRAP3_STRAP_VENDOR_ID_BIT_DEV0_F3_OFFSET               25
#define RCC_DEV0_EPF3_STRAP3_STRAP_VENDOR_ID_BIT_DEV0_F3_WIDTH                1
#define RCC_DEV0_EPF3_STRAP3_STRAP_VENDOR_ID_BIT_DEV0_F3_MASK                 0x2000000
#define RCC_DEV0_EPF3_STRAP3_STRAP_ALL_MSI_EVENT_SUPPORT_EN_DEV0_F3_OFFSET    26
#define RCC_DEV0_EPF3_STRAP3_STRAP_ALL_MSI_EVENT_SUPPORT_EN_DEV0_F3_WIDTH     1
#define RCC_DEV0_EPF3_STRAP3_STRAP_ALL_MSI_EVENT_SUPPORT_EN_DEV0_F3_MASK      0x4000000
#define RCC_DEV0_EPF3_STRAP3_STRAP_SMN_ERR_STATUS_MASK_EN_EP_DEV0_F3_OFFSET   27
#define RCC_DEV0_EPF3_STRAP3_STRAP_SMN_ERR_STATUS_MASK_EN_EP_DEV0_F3_WIDTH    1
#define RCC_DEV0_EPF3_STRAP3_STRAP_SMN_ERR_STATUS_MASK_EN_EP_DEV0_F3_MASK     0x8000000
#define RCC_DEV0_EPF3_STRAP3_Reserved_31_28_OFFSET                            28
#define RCC_DEV0_EPF3_STRAP3_Reserved_31_28_WIDTH                             4
#define RCC_DEV0_EPF3_STRAP3_Reserved_31_28_MASK                              0xf0000000L

/// RCC_DEV0_EPF3_STRAP3
typedef union {
  struct {
    UINT32    STRAP_POISONED_ADVISORY_NONFATAL_DEV0_F3 : 1;            ///<
    UINT32    STRAP_PWR_EN_DEV0_F3                     : 1;            ///<
    UINT32    STRAP_SUBSYS_ID_DEV0_F3                  : 16;           ///<
    UINT32    STRAP_MSI_EN_DEV0_F3                     : 1;            ///<
    UINT32    STRAP_MSI_CLR_PENDING_EN_DEV0_F3         : 1;            ///<
    UINT32    STRAP_MSIX_EN_DEV0_F3                    : 1;            ///<
    UINT32    Reserved_23_21                           : 3;            ///<
    UINT32    STRAP_PMC_DSI_DEV0_F3                    : 1;            ///<
    UINT32    STRAP_VENDOR_ID_BIT_DEV0_F3              : 1;            ///<
    UINT32    STRAP_ALL_MSI_EVENT_SUPPORT_EN_DEV0_F3   : 1;            ///<
    UINT32    STRAP_SMN_ERR_STATUS_MASK_EN_EP_DEV0_F3  : 1;            ///<
    UINT32    Reserved_31_28                           : 4;            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} RCC_DEV0_EPF3_STRAP3_STRUCT;

// **** DxF0x58 Register Definition ****
// Address
#define DxF0x58_PCI_OFFSET              0x58

// Field Data
#define DxF0x58_CapID_OFFSET            0
#define DxF0x58_CapID_WIDTH             8
#define DxF0x58_CapID_MASK              0xff
#define DxF0x58_NextPtr_OFFSET          8
#define DxF0x58_NextPtr_WIDTH           8
#define DxF0x58_NextPtr_MASK            0xff00
#define DxF0x58_Version_OFFSET          16
#define DxF0x58_Version_WIDTH           4
#define DxF0x58_Version_MASK            0xf0000
#define DxF0x58_DeviceType_OFFSET       20
#define DxF0x58_DeviceType_WIDTH        4
#define DxF0x58_DeviceType_MASK         0xf00000
#define DxF0x58_SlotImplemented_OFFSET  24
#define DxF0x58_SlotImplemented_WIDTH   1
#define DxF0x58_SlotImplemented_MASK    0x1000000
#define DxF0x58_IntMessageNum_OFFSET    25
#define DxF0x58_IntMessageNum_WIDTH     5
#define DxF0x58_IntMessageNum_MASK      0x3e000000
#define DxF0x58_Reserved_31_30_OFFSET   30
#define DxF0x58_Reserved_31_30_WIDTH    2
#define DxF0x58_Reserved_31_30_MASK     0xc0000000

/// DxF0x58
typedef union {
  struct {
    ///<
    UINT32    CapID           : 8;                                      ///<
    UINT32    NextPtr         : 8;                                      ///<
    UINT32    Version         : 4;                                      ///<
    UINT32    DeviceType      : 4;                                      ///<
    UINT32    SlotImplemented : 1;                                      ///<
    UINT32    IntMessageNum   : 5;                                      ///<
    UINT32    Reserved_31_30  : 2;                                      ///<
  } Field;                                                              ///<
  UINT32    Value;                                                      ///<
} DxF0x58_STRUCT;

// Address
#define NTB_CTRL_ADDRESS_HYGX               0x06000200

// Type
#define NTB_CTRL_TYPE                      TYPE_SMN

#define NTB_CTRL_CLKOffHyst_OFFSET         0
#define NTB_CTRL_CLKOffHyst_WIDTH          16
#define NTB_CTRL_CLKOffHyst_MASK           0xffff
#define NTB_CTRL_CLK_GATE_EN_OFFSET        16
#define NTB_CTRL_CLK_GATE_EN_WIDTH         1
#define NTB_CTRL_CLK_GATE_EN_MASK          0x10000
#define NTB_CTRL_PMMReg_AccessPath_OFFSET  17
#define NTB_CTRL_PMMReg_AccessPath_WIDTH   1
#define NTB_CTRL_PMMReg_AccessPath_MASK    0x20000
#define NTB_CTRL_SMMReg_AccessPath_OFFSET  18
#define NTB_CTRL_SMMReg_AccessPath_WIDTH   1
#define NTB_CTRL_SMMReg_AccessPath_MASK    0x40000
#define NTB_CTRL_SMN_ERR_TYPE_OFFSET       19
#define NTB_CTRL_SMN_ERR_TYPE_WIDTH        1
#define NTB_CTRL_SMN_ERR_TYPE_MASK         0x80000
#define NTB_CTRL_SMM_Reg_Ctrl_OFFSET       20
#define NTB_CTRL_SMM_Reg_Ctrl_WIDTH        1
#define NTB_CTRL_SMM_Reg_Ctrl_MASK         0x100000
#define NTB_CTRL_PMM_Reg_Ctrl_OFFSET       21
#define NTB_CTRL_PMM_Reg_Ctrl_WIDTH        1
#define NTB_CTRL_PMM_Reg_Ctrl_MASK         0x200000
#define NTB_CTRL_RESERVED_OFFSET           22
#define NTB_CTRL_RESERVED_WIDTH            10
#define NTB_CTRL_RESERVED_MASK             0xffc00000L

/// NTB_00000200
typedef union {
  struct {
    UINT32    CLKOffHyst        : 16;                                  ///<
    UINT32    CLK_GATE_EN       : 1;                                   ///<
    UINT32    PMMReg_AccessPath : 1;                                   ///<
    UINT32    SMMReg_AccessPath : 1;                                   ///<
    UINT32    SMN_ERR_TYPE      : 1;                                   ///<
    UINT32    SMM_Reg_Ctrl      : 1;                                   ///<
    UINT32    PMM_Reg_Ctrl      : 1;                                   ///<
    UINT32    RESERVED          : 10;                                  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NTB_CTRL_STRUCT;

// Address
#define NBIF0_HMI_WRR_WEIGHT_HYEX           (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x23844)
#define NBIF1_HMI_WRR_WEIGHT_HYEX           (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x23844)
#define NBIF2_HMI_WRR_WEIGHT_HYEX           (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x23844)

#define NBIF0_HMI_WRR_WEIGHT_HYGX           (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x23844)
#define NBIF1_HMI_WRR_WEIGHT_HYGX           (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x23844)
#define NBIF2_HMI_WRR_WEIGHT_HYGX           (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x23844)
#define NBIF3_HMI_WRR_WEIGHT_HYGX           (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x23844)
// Type
#define NBIF_HMI_WRR_WEIGHT_TYPE            TYPE_SMN

#define SMN_HMI_REQ_REALTIME_WEIGHT_OFFSET  0
#define SMN_HMI_REQ_REALTIME_WEIGHT_WIDTH   8
#define SMN_HMI_REQ_REALTIME_WEIGHT_MASK    0xff
#define SMN_HMI_REQ_NORM_P_WEIGHT_OFFSET    8
#define SMN_HMI_REQ_NORM_P_WEIGHT_WIDTH     8
#define SMN_HMI_REQ_NORM_P_WEIGHT_MASK      0xff00
#define SMN_HMI_REQ_NORM_NP_WEIGHT_OFFSET   16
#define SMN_HMI_REQ_NORM_NP_WEIGHT_WIDTH    8
#define SMN_HMI_REQ_NORM_NP_WEIGHT_MASK     0xff0000
#define SMN_Reserved_31_24_OFFSET           24
#define SMN_Reserved_31_24_WIDTH            8
#define SMN_Reserved_31_24_MASK             0xff000000L

/// NBIFRCC0_00003990
typedef union {
  struct {
    UINT32    HMI_REQ_REALTIME_WEIGHT : 8;                             ///<
    UINT32    HMI_REQ_NORM_P_WEIGHT   : 8;                             ///<
    UINT32    HMI_REQ_NORM_NP_WEIGHT  : 8;                             ///<
    UINT32    Reserved_31_24          : 8;                             ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NBIF0_HMI_WRR_WEIGHT_STRUCT;

#define PCIE_TX_ACK_LATENCY_LIMIT_HYGX                         (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40098)
#define TX_ACK_LATENCY_ACCELERATE_OFFSET  31
#define TX_ACK_LATENCY_ACCELERATE_WIDTH   1
#define TX_ACK_LATENCY_ACCELERATE_MASK    0x1

// Register Name PCIEP_STRAP_MISC
// Address
#define PCIEP_STRAP_MISC_instPCIE0_link_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40304)
#define PCIEP_STRAP_MISC_instPCIE1_link_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40304)
#define PCIEP_STRAP_MISC_instPCIE2_link_ADDRESS_HYEX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40304)

#define PCIEP_STRAP_MISC_instPCIE0_link_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40304)
#define PCIEP_STRAP_MISC_instPCIE2_link_ADDRESS_HYGX           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40304)

// Type
#define PCIEP_STRAP_MISC_TYPE                                 TYPE_SMN

#define PCIEP_STRAP_MISC_STRAP_REVERSE_LANES_OFFSET           0
#define PCIEP_STRAP_MISC_STRAP_REVERSE_LANES_WIDTH            1
#define PCIEP_STRAP_MISC_STRAP_REVERSE_LANES_MASK             0x1
#define PCIEP_STRAP_MISC_STRAP_E2E_PREFIX_EN_OFFSET           1
#define PCIEP_STRAP_MISC_STRAP_E2E_PREFIX_EN_WIDTH            1
#define PCIEP_STRAP_MISC_STRAP_E2E_PREFIX_EN_MASK             0x2
#define PCIEP_STRAP_MISC_STRAP_EXTENDED_FMT_SUPPORTED_OFFSET  2
#define PCIEP_STRAP_MISC_STRAP_EXTENDED_FMT_SUPPORTED_WIDTH   1
#define PCIEP_STRAP_MISC_STRAP_EXTENDED_FMT_SUPPORTED_MASK    0x4
#define PCIEP_STRAP_MISC_STRAP_OBFF_SUPPORTED_OFFSET          3
#define PCIEP_STRAP_MISC_STRAP_OBFF_SUPPORTED_WIDTH           2
#define PCIEP_STRAP_MISC_STRAP_OBFF_SUPPORTED_MASK            0x18
#define PCIEP_STRAP_MISC_STRAP_LTR_SUPPORTED_OFFSET           5
#define PCIEP_STRAP_MISC_STRAP_LTR_SUPPORTED_WIDTH            1
#define PCIEP_STRAP_MISC_STRAP_LTR_SUPPORTED_MASK             0x20
#define PCIEP_STRAP_MISC_Reserved_31_6_OFFSET                 6
#define PCIEP_STRAP_MISC_Reserved_31_6_WIDTH                  26
#define PCIEP_STRAP_MISC_Reserved_31_6_MASK                   0xffffffc0L

/// PCIEPORT0F0_00000304
typedef union {
  struct {
    UINT32    STRAP_REVERSE_LANES          : 1;                        ///<
    UINT32    STRAP_E2E_PREFIX_EN          : 1;                        ///<
    UINT32    STRAP_EXTENDED_FMT_SUPPORTED : 1;                        ///<
    UINT32    STRAP_OBFF_SUPPORTED         : 2;                        ///<
    UINT32    STRAP_LTR_SUPPORTED          : 1;                        ///<
    UINT32    Reserved_31_6                : 26;                       ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIEP_STRAP_MISC_STRUCT;

// Register Name RAS_LEAF_CTRL
#define NBIF0_BIF_RAS_LEAF0_CTRL_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x39000)
#define NBIF0_BIF_RAS_LEAF1_CTRL_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x39004)
#define NBIF0_BIF_RAS_LEAF2_CTRL_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x39008)

#define NBIF1_BIF_RAS_LEAF0_CTRL_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x39000)
#define NBIF1_BIF_RAS_LEAF1_CTRL_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x39004)
#define NBIF1_BIF_RAS_LEAF2_CTRL_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x39008)

#define NBIF2_BIF_RAS_LEAF0_CTRL_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x39000)
#define NBIF2_BIF_RAS_LEAF1_CTRL_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x39004)
#define NBIF2_BIF_RAS_LEAF2_CTRL_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x39008)

#define NBIF0_BIF_RAS_LEAF0_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x39000)
#define NBIF0_BIF_RAS_LEAF1_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x39004)
#define NBIF0_BIF_RAS_LEAF2_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x39008)

#define NBIF1_BIF_RAS_LEAF0_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x39000)
#define NBIF1_BIF_RAS_LEAF1_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x39004)
#define NBIF1_BIF_RAS_LEAF2_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x39008)

#define NBIF2_BIF_RAS_LEAF0_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x39000)
#define NBIF2_BIF_RAS_LEAF1_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x39004)
#define NBIF2_BIF_RAS_LEAF2_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x39008)

#define NBIF3_BIF_RAS_LEAF0_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x39000)
#define NBIF3_BIF_RAS_LEAF1_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x39004)
#define NBIF3_BIF_RAS_LEAF2_CTRL_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x39008)

// HyEx Address
#define NBIF0_GDC_RAS_LEAF0_CTRL_ADDRESS_HYEX   0x0141F800
#define NBIF0_GDC_RAS_LEAF1_CTRL_ADDRESS_HYEX   0x0141F804
#define NBIF0_GDC_RAS_LEAF2_CTRL_ADDRESS_HYEX   0x0141F808
#define NBIF0_GDC_RAS_LEAF3_CTRL_ADDRESS_HYEX   0x0141F80C
#define NBIF0_GDC_RAS_LEAF4_CTRL_ADDRESS_HYEX   0x0141F810
#define NBIF0_GDC_RAS_LEAF5_CTRL_ADDRESS_HYEX   0x0141F814

#define NBIF1_GDC_RAS_LEAF0_CTRL_ADDRESS_HYEX   0x0181F800
#define NBIF1_GDC_RAS_LEAF1_CTRL_ADDRESS_HYEX   0x0181F804
#define NBIF1_GDC_RAS_LEAF2_CTRL_ADDRESS_HYEX   0x0181F808
#define NBIF1_GDC_RAS_LEAF3_CTRL_ADDRESS_HYEX   0x0181F80C
#define NBIF1_GDC_RAS_LEAF4_CTRL_ADDRESS_HYEX   0x0181F810
#define NBIF1_GDC_RAS_LEAF5_CTRL_ADDRESS_HYEX   0x0181F814

// HyGx Address
#define NBIF0_GDC_RAS_LEAF0_CTRL_ADDRESS_HYGX   0x04C1F800
#define NBIF0_GDC_RAS_LEAF1_CTRL_ADDRESS_HYGX   0x04C1F804
#define NBIF0_GDC_RAS_LEAF2_CTRL_ADDRESS_HYGX   0x04C1F808
#define NBIF0_GDC_RAS_LEAF3_CTRL_ADDRESS_HYGX   0x04C1F80C
#define NBIF0_GDC_RAS_LEAF4_CTRL_ADDRESS_HYGX   0x04C1F810
#define NBIF0_GDC_RAS_LEAF5_CTRL_ADDRESS_HYGX   0x04C1F814

#define NBIF2_GDC_RAS_LEAF0_CTRL_ADDRESS_HYGX   0x0501F800
#define NBIF2_GDC_RAS_LEAF1_CTRL_ADDRESS_HYGX   0x0501F804
#define NBIF2_GDC_RAS_LEAF2_CTRL_ADDRESS_HYGX   0x0501F808
#define NBIF2_GDC_RAS_LEAF3_CTRL_ADDRESS_HYGX   0x0501F80C
#define NBIF2_GDC_RAS_LEAF4_CTRL_ADDRESS_HYGX   0x0501F810
#define NBIF2_GDC_RAS_LEAF5_CTRL_ADDRESS_HYGX   0x0501F814

#define NBIF3_GDC_RAS_LEAF0_CTRL_ADDRESS_HYGX   0x0541F800
#define NBIF3_GDC_RAS_LEAF1_CTRL_ADDRESS_HYGX   0x0541F804
#define NBIF3_GDC_RAS_LEAF2_CTRL_ADDRESS_HYGX   0x0541F808
#define NBIF3_GDC_RAS_LEAF3_CTRL_ADDRESS_HYGX   0x0541F80C
#define NBIF3_GDC_RAS_LEAF4_CTRL_ADDRESS_HYGX   0x0541F810
#define NBIF3_GDC_RAS_LEAF5_CTRL_ADDRESS_HYGX   0x0541F814
// Type
#define RAS_LEAF_CTRL_TYPE                       TYPE_SMN

#define RAS_LEAF_CTRL_POISON_DET_EN_OFFSET       0
#define RAS_LEAF_CTRL_POISON_DET_EN_WIDTH        1
#define RAS_LEAF_CTRL_POISON_DET_EN_MASK         0x1
#define RAS_LEAF_CTRL_POISON_ERREVENT_EN_OFFSET  1
#define RAS_LEAF_CTRL_POISON_ERREVENT_EN_WIDTH   1
#define RAS_LEAF_CTRL_POISON_ERREVENT_EN_MASK    0x2
#define RAS_LEAF_CTRL_POISON_STALL_EN_OFFSET     2
#define RAS_LEAF_CTRL_POISON_STALL_EN_WIDTH      1
#define RAS_LEAF_CTRL_POISON_STALL_EN_MASK       0x4
#define RAS_LEAF_CTRL_Reserved_3_3_OFFSET        3
#define RAS_LEAF_CTRL_Reserved_3_3_WIDTH         1
#define RAS_LEAF_CTRL_Reserved_3_3_MASK          0x8
#define RAS_LEAF_CTRL_PARITY_DET_EN_OFFSET       4
#define RAS_LEAF_CTRL_PARITY_DET_EN_WIDTH        1
#define RAS_LEAF_CTRL_PARITY_DET_EN_MASK         0x10
#define RAS_LEAF_CTRL_PARITY_ERREVENT_EN_OFFSET  5
#define RAS_LEAF_CTRL_PARITY_ERREVENT_EN_WIDTH   1
#define RAS_LEAF_CTRL_PARITY_ERREVENT_EN_MASK    0x20
#define RAS_LEAF_CTRL_PARITY_STALL_EN_OFFSET     6
#define RAS_LEAF_CTRL_PARITY_STALL_EN_WIDTH      1
#define RAS_LEAF_CTRL_PARITY_STALL_EN_MASK       0x40
#define RAS_LEAF_CTRL_Reserved_15_7_OFFSET       7
#define RAS_LEAF_CTRL_Reserved_15_7_WIDTH        9
#define RAS_LEAF_CTRL_Reserved_15_7_MASK         0xff80
#define RAS_LEAF_CTRL_ERR_EVENT_RECV_OFFSET      16
#define RAS_LEAF_CTRL_ERR_EVENT_RECV_WIDTH       1
#define RAS_LEAF_CTRL_ERR_EVENT_RECV_MASK        0x10000
#define RAS_LEAF_CTRL_LINK_DIS_RECV_OFFSET       17
#define RAS_LEAF_CTRL_LINK_DIS_RECV_WIDTH        1
#define RAS_LEAF_CTRL_LINK_DIS_RECV_MASK         0x20000
#define RAS_LEAF_CTRL_POISON_ERR_DET_OFFSET      18
#define RAS_LEAF_CTRL_POISON_ERR_DET_WIDTH       1
#define RAS_LEAF_CTRL_POISON_ERR_DET_MASK        0x40000
#define RAS_LEAF_CTRL_PARITY_ERR_DET_OFFSET      19
#define RAS_LEAF_CTRL_PARITY_ERR_DET_WIDTH       1
#define RAS_LEAF_CTRL_PARITY_ERR_DET_MASK        0x80000
#define RAS_LEAF_CTRL_ERR_EVENT_SENT_OFFSET      20
#define RAS_LEAF_CTRL_ERR_EVENT_SENT_WIDTH       1
#define RAS_LEAF_CTRL_ERR_EVENT_SENT_MASK        0x100000
#define RAS_LEAF_CTRL_EGRESS_STALLED_OFFSET      21
#define RAS_LEAF_CTRL_EGRESS_STALLED_WIDTH       1
#define RAS_LEAF_CTRL_EGRESS_STALLED_MASK        0x200000
#define RAS_LEAF_CTRL_Reserved_31_22_OFFSET      22
#define RAS_LEAF_CTRL_Reserved_31_22_WIDTH       10
#define RAS_LEAF_CTRL_Reserved_31_22_MASK        0xffc00000L

/// NBIFGDCRAS0_00000014
typedef union {
  struct {
    UINT32    POISON_DET_EN      : 1;                                  ///<
    UINT32    POISON_ERREVENT_EN : 1;                                  ///<
    UINT32    POISON_STALL_EN    : 1;                                  ///<
    UINT32    Reserved_3_3       : 1;                                  ///<
    UINT32    PARITY_DET_EN      : 1;                                  ///<
    UINT32    PARITY_ERREVENT_EN : 1;                                  ///<
    UINT32    PARITY_STALL_EN    : 1;                                  ///<
    UINT32    Reserved_15_7      : 9;                                  ///<
    UINT32    ERR_EVENT_RECV     : 1;                                  ///<
    UINT32    LINK_DIS_RECV      : 1;                                  ///<
    UINT32    POISON_ERR_DET     : 1;                                  ///<
    UINT32    PARITY_ERR_DET     : 1;                                  ///<
    UINT32    ERR_EVENT_SENT     : 1;                                  ///<
    UINT32    EGRESS_STALLED     : 1;                                  ///<
    UINT32    Reserved_31_22     : 10;                                 ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} RAS_LEAF_CTRL_STRUCT;

typedef union {
  struct {
    UINT32        STRAP_SRIOV_VF_DEVICE_ID_DEV0_F4 : 16;
    UINT32 STRAP_SRIOV_SUPPORTED_PAGE_SIZE_DEV0_F4 : 16;
  } Field;
  UINT32 Value;
} NBIF_FUN4_STRAP1_STRUCT;

typedef union {
  struct {
    UINT32  : 20;
    UINT32  STRAP_ATOMIC_64BIT_EN_DEV0_F4 : 1;
    UINT32        STRAP_ATOMIC_EN_DEV0_F4 : 1;
    UINT32           STRAP_FLR_EN_DEV0_F4 : 1;
    UINT32      STRAP_PME_SUPPORT_DEV0_F4 : 5;
    UINT32    STRAP_INTERRUPT_PIN_DEV0_F4 : 3;
    UINT32   STRAP_AUXPWR_SUPPORT_DEV0_F4 : 1;
  } Field;
  UINT32 Value;
} RCC_DEV0_EPFx_STRAP4_STRUCT;

// Register Name RCC_DEV0_EPF#_STRAP2
//HyEx NBIF Address
#define NBIF0_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34008)
#define NBIF0_RCC_DEV0_EPF1_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34208)
#define NBIF0_RCC_DEV0_EPF2_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34408)
#define NBIF0_RCC_DEV0_EPF3_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34608)
#define NBIF0_RCC_DEV0_EPF4_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34808)
#define NBIF0_RCC_DEV0_EPF5_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34A08)
#define NBIF0_RCC_DEV0_EPF6_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34C08)
#define NBIF0_RCC_DEV0_EPF7_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34E08)

#define NBIF1_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34008)
#define NBIF1_RCC_DEV0_EPF1_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34208)
#define NBIF1_RCC_DEV0_EPF2_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34408)
#define NBIF1_RCC_DEV0_EPF3_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34608)
#define NBIF1_RCC_DEV0_EPF4_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34808)
#define NBIF1_RCC_DEV0_EPF5_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34A08)
#define NBIF1_RCC_DEV0_EPF6_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34C08)
#define NBIF1_RCC_DEV0_EPF7_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34E08)

#define NBIF2_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34008)
#define NBIF2_RCC_DEV0_EPF1_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34208)
#define NBIF2_RCC_DEV0_EPF2_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34408)
#define NBIF2_RCC_DEV0_EPF3_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34608)
#define NBIF2_RCC_DEV0_EPF4_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34808)
#define NBIF2_RCC_DEV0_EPF5_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34A08)
#define NBIF2_RCC_DEV0_EPF6_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34C08)
#define NBIF2_RCC_DEV0_EPF7_STRAP2_ADDRESS_HYEX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34E08)

#define RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET_HYEX                         16
#define RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_WIDTH_HYEX                          1
#define RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK_HYEX                           0x10000
#define RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET_HYEX                         17
#define RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_WIDTH_HYEX                          1
#define RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK_HYEX                           0x20000

#define RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET                   20
#define RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_WIDTH                    1
#define RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK                     0x100000
#define RCC_DEV0_EPFx_STRAP2_STRAP_DPA_EN_DEV0_F0_OFFSET                             21
#define RCC_DEV0_EPFx_STRAP2_STRAP_DPA_EN_DEV0_F0_WIDTH                              1
#define RCC_DEV0_EPFx_STRAP2_STRAP_DPA_EN_DEV0_F0_MASK                               0x200000
#define RCC_DEV0_EPFx_STRAP2_STRAP_DSN_EN_DEV0_F0_OFFSET                             22
#define RCC_DEV0_EPFx_STRAP2_STRAP_DSN_EN_DEV0_F0_WIDTH                              1
#define RCC_DEV0_EPFx_STRAP2_STRAP_DSN_EN_DEV0_F0_MASK                               0x400000
#define RCC_DEV0_EPFx_STRAP2_STRAP_VC_EN_DEV0_F0_OFFSET                              23
#define RCC_DEV0_EPFx_STRAP2_STRAP_VC_EN_DEV0_F0_WIDTH                               1
#define RCC_DEV0_EPFx_STRAP2_STRAP_VC_EN_DEV0_F0_MASK                                0x800000
#define RCC_DEV0_EPFx_STRAP2_STRAP_MSI_MULTI_CAP_DEV0_F0_OFFSET                      24
#define RCC_DEV0_EPFx_STRAP2_STRAP_MSI_MULTI_CAP_DEV0_F0_WIDTH                       3
#define RCC_DEV0_EPFx_STRAP2_STRAP_MSI_MULTI_CAP_DEV0_F0_MASK                        0x7000000
#define RCC_DEV0_EPFx_STRAP2_STRAP_PAGE_REQ_EN_DEV0_F0_OFFSET                        27
#define RCC_DEV0_EPFx_STRAP2_STRAP_PAGE_REQ_EN_DEV0_F0_WIDTH                         1
#define RCC_DEV0_EPFx_STRAP2_STRAP_PAGE_REQ_EN_DEV0_F0_MASK                          0x8000000
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_EN_DEV0_F0_OFFSET                           28
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_EN_DEV0_F0_WIDTH                            1
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_EN_DEV0_F0_MASK                             0x10000000
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_EXE_PERMISSION_SUPPORTED_DEV0_F0_OFFSET     29
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_EXE_PERMISSION_SUPPORTED_DEV0_F0_WIDTH      1
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_EXE_PERMISSION_SUPPORTED_DEV0_F0_MASK       0x20000000
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_GLOBAL_INVALIDATE_SUPPORTED_DEV0_F0_OFFSET  30
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_GLOBAL_INVALIDATE_SUPPORTED_DEV0_F0_WIDTH   1
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_GLOBAL_INVALIDATE_SUPPORTED_DEV0_F0_MASK    0x40000000
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_PRIV_MODE_SUPPORTED_DEV0_F0_OFFSET          31
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_PRIV_MODE_SUPPORTED_DEV0_F0_WIDTH           1
#define RCC_DEV0_EPFx_STRAP2_STRAP_PASID_PRIV_MODE_SUPPORTED_DEV0_F0_MASK            0x80000000L

/// NBIFSTRAP0_00000008
typedef union {
  struct {
    UINT32                                  STRAP_SRIOV_EN_DEV0_F0:1 ; ///<
    UINT32                           STRAP_SRIOV_TOTAL_VFS_DEV0_F0:5 ; ///<
    UINT32                                 STRAP_64BAR_DIS_DEV0_F0:1 ; ///<
    UINT32                             STRAP_NO_SOFT_RESET_DEV0_F0:1 ; ///<
    UINT32                             STRAP_RESIZE_BAR_EN_DEV0_F0:1 ; ///<
    UINT32                           STRAP_MAX_PASID_WIDTH_DEV0_F0:5 ; ///<
    UINT32                    STRAP_MSI_PERVECTOR_MASK_CAP_DEV0_F0:1 ; ///<
    UINT32                                    STRAP_ARI_EN_DEV0_F0:1 ; ///<
    UINT32                                    STRAP_AER_EN_DEV0_F0:1 ; ///<
    UINT32                                    STRAP_ACS_EN_DEV0_F0:1 ; ///<
    UINT32                                    STRAP_ATS_EN_DEV0_F0:1 ; ///<
    UINT32                                          Reserved_19_19:1 ; ///<
    UINT32                          STRAP_CPL_ABORT_ERR_EN_DEV0_F0:1 ; ///<
    UINT32                                    STRAP_DPA_EN_DEV0_F0:1 ; ///<
    UINT32                                    STRAP_DSN_EN_DEV0_F0:1 ; ///<
    UINT32                                     STRAP_VC_EN_DEV0_F0:1 ; ///<
    UINT32                             STRAP_MSI_MULTI_CAP_DEV0_F0:3 ; ///<
    UINT32                               STRAP_PAGE_REQ_EN_DEV0_F0:1 ; ///<
    UINT32                                  STRAP_PASID_EN_DEV0_F0:1 ; ///<
    UINT32            STRAP_PASID_EXE_PERMISSION_SUPPORTED_DEV0_F0:1 ; ///<
    UINT32         STRAP_PASID_GLOBAL_INVALIDATE_SUPPORTED_DEV0_F0:1 ; ///<
    UINT32                 STRAP_PASID_PRIV_MODE_SUPPORTED_DEV0_F0:1 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} RCC_DEV0_EPFx_STRAP2_STRUCT_HYEX;

//HyGx NBIF Address
#define NBIF0_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34008)
#define NBIF0_RCC_DEV0_EPF1_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34208)
#define NBIF0_RCC_DEV0_EPF2_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34408)
#define NBIF0_RCC_DEV0_EPF3_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34608)
#define NBIF0_RCC_DEV0_EPF4_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34808)
#define NBIF0_RCC_DEV0_EPF5_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34A08)
#define NBIF0_RCC_DEV0_EPF6_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34C08)
#define NBIF0_RCC_DEV0_EPF7_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34E08)

#define NBIF1_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34008)
#define NBIF1_RCC_DEV0_EPF1_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34208)
#define NBIF1_RCC_DEV0_EPF2_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34408)
#define NBIF1_RCC_DEV0_EPF3_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34608)
#define NBIF1_RCC_DEV0_EPF4_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34808)
#define NBIF1_RCC_DEV0_EPF5_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34A08)
#define NBIF1_RCC_DEV0_EPF6_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34C08)
#define NBIF1_RCC_DEV0_EPF7_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34E08)

#define NBIF2_RCC_DEV0_EPF0_STRAP1_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34004)
#define NBIF2_RCC_DEV0_EPF1_STRAP1_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34204)
#define NBIF2_RCC_DEV0_EPF2_STRAP1_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34404)
#define NBIF2_RCC_DEV0_EPF3_STRAP1_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34604)
#define NBIF2_RCC_DEV0_EPF4_STRAP1_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34804)
#define NBIF2_RCC_DEV0_EPF5_STRAP1_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34A04)
#define NBIF2_RCC_DEV0_EPF6_STRAP1_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34C04)
#define NBIF2_RCC_DEV0_EPF7_STRAP1_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34E04)

#define NBIF2_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34008)
#define NBIF2_RCC_DEV0_EPF1_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34208)
#define NBIF2_RCC_DEV0_EPF2_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34408)
#define NBIF2_RCC_DEV0_EPF3_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34608)
#define NBIF2_RCC_DEV0_EPF4_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34808)
#define NBIF2_RCC_DEV0_EPF5_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34A08)
#define NBIF2_RCC_DEV0_EPF6_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34C08)
#define NBIF2_RCC_DEV0_EPF7_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34E08)

#define NBIF2_EPF4_PCIE_VENDOR_SPECIFIC1_HYGX    (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x44108)

#define NBIF2_RCC_DEV0_EPF0_STRAP4_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34010)
#define NBIF2_RCC_DEV0_EPF1_STRAP4_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34210)
#define NBIF2_RCC_DEV0_EPF2_STRAP4_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34410)
#define NBIF2_RCC_DEV0_EPF3_STRAP4_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34610)
#define NBIF2_RCC_DEV0_EPF4_STRAP4_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34810)
#define NBIF2_RCC_DEV0_EPF5_STRAP4_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34A10)
#define NBIF2_RCC_DEV0_EPF6_STRAP4_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34C10)
#define NBIF2_RCC_DEV0_EPF7_STRAP4_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34E10)

#define NBIF3_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34008)
#define NBIF3_RCC_DEV0_EPF1_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34208)
#define NBIF3_RCC_DEV0_EPF2_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34408)
#define NBIF3_RCC_DEV0_EPF3_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34608)
#define NBIF3_RCC_DEV0_EPF4_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34808)
#define NBIF3_RCC_DEV0_EPF5_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34A08)
#define NBIF3_RCC_DEV0_EPF6_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34C08)
#define NBIF3_RCC_DEV0_EPF7_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x34E08)


#define NBIF0_RCC_DEV0_EPF2_STRAP6_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34418)
#define NBIF1_RCC_DEV0_EPF2_STRAP6_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34418)

#define NBIF0_RCC_DEV0_EPF3_STRAP6_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34618)
#define NBIF1_RCC_DEV0_EPF3_STRAP6_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34618)

#define RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET_HYGX                         17
#define RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_WIDTH_HYGX                          1
#define RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK_HYGX                           0x20000
#define RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET_HYGX                         18
#define RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_WIDTH_HYGX                          1
#define RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK_HYGX                           0x40000

/// NBIFSTRAP0_00000008
typedef union {
  struct {
    UINT32    Reserved_7_0                                    : 8;     ///<
    UINT32    STRAP_NO_SOFT_RESET_DEV0_F0                     : 1;     ///<
    UINT32    STRAP_RESIZE_BAR_EN_DEV0_F0                     : 1;     ///<
    UINT32    STRAP_MAX_PASID_WIDTH_DEV0_F0                   : 5;     ///<
    UINT32    STRAP_MSI_PERVECTOR_MASK_CAP_DEV0_F0            : 1;     ///<
    UINT32    STRAP_ARI_EN_DEV0_F0                            : 1;     ///<
    UINT32    STRAP_AER_EN_DEV0_F0                            : 1;     ///<
    UINT32    STRAP_ACS_EN_DEV0_F0                            : 1;     ///<
    UINT32    STRAP_ATS_EN_DEV0_F0                            : 1;     ///<
    UINT32    STRAP_CPL_ABORT_ERR_EN_DEV0_F0                  : 1;     ///<
    UINT32    STRAP_DPA_EN_DEV0_F0                            : 1;     ///<
    UINT32    STRAP_DSN_EN_DEV0_F0                            : 1;     ///<
    UINT32    STRAP_VC_EN_DEV0_F0                             : 1;     ///<
    UINT32    STRAP_MSI_MULTI_CAP_DEV0_F0                     : 3;     ///<
    UINT32    STRAP_PAGE_REQ_EN_DEV0_F0                       : 1;     ///<
    UINT32    STRAP_PASID_EN_DEV0_F0                          : 1;     ///<
    UINT32    STRAP_PASID_EXE_PERMISSION_SUPPORTED_DEV0_F0    : 1;     ///<
    UINT32    STRAP_PASID_GLOBAL_INVALIDATE_SUPPORTED_DEV0_F0 : 1;     ///<
    UINT32    STRAP_PASID_PRIV_MODE_SUPPORTED_DEV0_F0         : 1;     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} RCC_DEV0_EPFx_STRAP2_STRUCT_HYGX;

// Register Name RCC_DEV0_PORT_STRAP0
// Address
#define NBIF0_RCC_DEV0_PORT_STRAP0_ADDRESS_HYEX      (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x31000)
#define NBIF1_RCC_DEV0_PORT_STRAP0_ADDRESS_HYEX      (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x31000)
#define NBIF2_RCC_DEV0_PORT_STRAP0_ADDRESS_HYEX      (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x31000)

#define NBIF0_RCC_DEV0_PORT_STRAP0_ADDRESS_HYGX      (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x31000)
#define NBIF1_RCC_DEV0_PORT_STRAP0_ADDRESS_HYGX      (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x31000)
#define NBIF2_RCC_DEV0_PORT_STRAP0_ADDRESS_HYGX      (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x31000)
#define NBIF3_RCC_DEV0_PORT_STRAP0_ADDRESS_HYGX      (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x31000)

// Type
#define RCC_DEV0_PORT_STRAP0_TYPE                                       TYPE_SMN

#define RCC_DEV0_PORT_STRAP0_Reserved_0_0_OFFSET                        0
#define RCC_DEV0_PORT_STRAP0_Reserved_0_0_WIDTH                         1
#define RCC_DEV0_PORT_STRAP0_Reserved_0_0_MASK                          0x1
#define RCC_DEV0_PORT_STRAP0_STRAP_ARI_EN_DN_DEV0_OFFSET                1
#define RCC_DEV0_PORT_STRAP0_STRAP_ARI_EN_DN_DEV0_WIDTH                 1
#define RCC_DEV0_PORT_STRAP0_STRAP_ARI_EN_DN_DEV0_MASK                  0x2
#define RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET                2
#define RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_WIDTH                 1
#define RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK                  0x4
#define RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET                3
#define RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_WIDTH                 1
#define RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK                  0x8
#define RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET      4
#define RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_WIDTH       1
#define RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK        0x10
#define RCC_DEV0_PORT_STRAP0_STRAP_DEVICE_ID_DN_DEV0_OFFSET             5
#define RCC_DEV0_PORT_STRAP0_STRAP_DEVICE_ID_DN_DEV0_WIDTH              16
#define RCC_DEV0_PORT_STRAP0_STRAP_DEVICE_ID_DN_DEV0_MASK               0x1fffe0
#define RCC_DEV0_PORT_STRAP0_STRAP_INTERRUPT_PIN_DN_DEV0_OFFSET         21
#define RCC_DEV0_PORT_STRAP0_STRAP_INTERRUPT_PIN_DN_DEV0_WIDTH          3
#define RCC_DEV0_PORT_STRAP0_STRAP_INTERRUPT_PIN_DN_DEV0_MASK           0xe00000
#define RCC_DEV0_PORT_STRAP0_STRAP_IGNORE_E2E_PREFIX_UR_DN_DEV0_OFFSET  24
#define RCC_DEV0_PORT_STRAP0_STRAP_IGNORE_E2E_PREFIX_UR_DN_DEV0_WIDTH   1
#define RCC_DEV0_PORT_STRAP0_STRAP_IGNORE_E2E_PREFIX_UR_DN_DEV0_MASK    0x1000000
#define RCC_DEV0_PORT_STRAP0_STRAP_MAX_PAYLOAD_SUPPORT_DN_DEV0_OFFSET   25
#define RCC_DEV0_PORT_STRAP0_STRAP_MAX_PAYLOAD_SUPPORT_DN_DEV0_WIDTH    3
#define RCC_DEV0_PORT_STRAP0_STRAP_MAX_PAYLOAD_SUPPORT_DN_DEV0_MASK     0xe000000
#define RCC_DEV0_PORT_STRAP0_STRAP_MAX_LINK_WIDTH_SUPPORT_DEV0_OFFSET   28
#define RCC_DEV0_PORT_STRAP0_STRAP_MAX_LINK_WIDTH_SUPPORT_DEV0_WIDTH    3
#define RCC_DEV0_PORT_STRAP0_STRAP_MAX_LINK_WIDTH_SUPPORT_DEV0_MASK     0x70000000
#define RCC_DEV0_PORT_STRAP0_Reserved_31_31_OFFSET                      31
#define RCC_DEV0_PORT_STRAP0_Reserved_31_31_WIDTH                       1
#define RCC_DEV0_PORT_STRAP0_Reserved_31_31_MASK                        0x80000000L

/// NBIFRCCSTRAP0_00000000
typedef union {
  struct {
    UINT32    Reserved_0_0                       : 1;                  ///<
    UINT32    STRAP_ARI_EN_DN_DEV0               : 1;                  ///<
    UINT32    STRAP_ACS_EN_DN_DEV0               : 1;                  ///<
    UINT32    STRAP_AER_EN_DN_DEV0               : 1;                  ///<
    UINT32    STRAP_CPL_ABORT_ERR_EN_DN_DEV0     : 1;                  ///<
    UINT32    STRAP_DEVICE_ID_DN_DEV0            : 16;                 ///<
    UINT32    STRAP_INTERRUPT_PIN_DN_DEV0        : 3;                  ///<
    UINT32    STRAP_IGNORE_E2E_PREFIX_UR_DN_DEV0 : 1;                  ///<
    UINT32    STRAP_MAX_PAYLOAD_SUPPORT_DN_DEV0  : 3;                  ///<
    UINT32    STRAP_MAX_LINK_WIDTH_SUPPORT_DEV0  : 3;                  ///<
    UINT32    Reserved_31_31                     : 1;                  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} RCC_DEV0_PORT_STRAP0_STRUCT;

//HyEx SSID Registers
#define PCICFG_IOMMU_SSID_ADDRESS_HYEX              (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYEX << 20) + 0x00078)

#define PCICFG_NBIF0RC_SSID_ADDRESS_HYEX            (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x31004)
#define PCICFG_NBIF0_SSID_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x4004C)
#define PCICFG_NBIF0_NTB_SSID_ADDRESS_HYEX          (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x4104C)
#define PCICFG_NBIF0_FUNC2_SSID_ADDRESS_HYEX        (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x4204C)
#define PCICFG_NBIF0_EPFN_CFG_ADDRESS_HYEX          (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x43000)

#define PCICFG_NBIF1RC_SSID_ADDRESS_HYEX            (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x31004)
#define PCICFG_NBIF1_SSID_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x4004C)
#define PCICFG_NBIF1_FUNC1_SSID_ADDRESS_HYEX        (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x4104C)
#define PCICFG_NBIF1_SATA_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x42000)
#define PCICFG_NBIF1_XGBE0_SSID_ADDRESS_HYEX        (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x4404C)
#define PCICFG_NBIF1_XGBE1_SSID_ADDRESS_HYEX        (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x4504C)
#define PCICFG_NBIF1_XGBE2_SSID_ADDRESS_HYEX        (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x4604C)
#define PCICFG_NBIF1_XGBE3_SSID_ADDRESS_HYEX        (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x4704C)

#define PCICFG_NBIF2RC_SSID_ADDRESS_HYEX            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x31004)
#define PCICFG_NBIF2_SSID_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x4004C)
#define PCICFG_NBIF2_NTB_SSID_ADDRESS_HYEX          (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x4104C)

#define PCICFG_NBADAPTER_SSID_ADDRESS_HYEX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x00050)

//HyGx SSID Registers
// IOMMU
#define PCICFG_IOMMU_SSID_ADDRESS_HYGX              (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x00078)

// NBIF Root Bridge Functions
#define PCICFG_NBIF0RC_SSID_ADDRESS_HYGX            (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x31004)
#define PCICFG_NBIF2RC_SSID_ADDRESS_HYGX            (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x31004)
#define PCICFG_NBIF3RC_SSID_ADDRESS_HYGX            (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x31004)

// NBIF Dummy Functions
#define PCICFG_NBIF0_SSID_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x4004C)
#define PCICFG_NBIF2_SSID_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x4004C)
#define PCICFG_NBIF3_SSID_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x4004C)

// NTB
#define PCICFG_NBIF0_NTB_SSID_ADDRESS_HYGX          (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x4104C)

// NTBCCP/PSPCCP
#define PCICFG_NBIF0_FUNC2_SSID_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x4204C)
#define PCICFG_NBIF0_FUNC3_SSID_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x4304C)
#define PCICFG_NBIF3_FUNC4_SSID_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x4404C)

#define PCICFG_NBIF2_USB_ADAPTER_ID_W_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x41000)
#define PCICFG_NBIF3_USB_ADAPTER_ID_W_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x41000)

// SATA
#define PCICFG_NBIF2_SATA_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x42000)
#define PCICFG_NBIF3_SATA_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x42000)

// XGBE
#define PCICFG_NBIF2_XGBE0_SSID_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x4404C)
#define PCICFG_NBIF2_XGBE1_SSID_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x4504C)
#define PCICFG_NBIF2_XGBE2_SSID_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x4604C)
#define PCICFG_NBIF2_XGBE3_SSID_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x4704C)

// IOHC
#define PCICFG_NBADAPTER_SSID_ADDRESS_HYGX          (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x00050)

#define PCICFG_SUBSYSTEM_VENDOR_ID_OFFSET          0
#define PCICFG_SUBSYSTEM_VENDOR_ID_WIDTH           16
#define PCICFG_SUBSYSTEM_VENDOR_ID_MASK            0xffff
#define PCICFG_SUBSYSTEM_ID_OFFSET                 16
#define PCICFG_SUBSYSTEM_ID_WIDTH                  16
#define PCICFG_SUBSYSTEM_ID_MASK                   0xffff0000L

/// PCICFG SUBSYSTEM DEFINITION
typedef union {
  struct {
    UINT32    SUBSYSTEM_VENDOR_ID : 16;                                ///<
    UINT32    SUBSYSTEM_ID        : 16;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCICFG_STRUCT;

// Address
#define NBIF0_RCC_DEV0_PORT_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x31008)
#define NBIF1_RCC_DEV0_PORT_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x31008)
#define NBIF2_RCC_DEV0_PORT_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x31008)
#define NBIF3_RCC_DEV0_PORT_STRAP2_ADDRESS_HYGX  (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x31008)

#define NBIF1RCCFG_COMMAND_ADDRESS_HYGX          (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x00004)
#define NBIF1RCCFG_SUB_BUS_ADDRESS_HYGX          (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x00018)
#define NBIF1RCCFG_BASELIMIT_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x00020)
#define NBIF1EPF3CFG_VENDOR_ID_ADDRESS_HYGX      (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x43000)
#define NBIF1EPF3CFG_COMMAND_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x43004)
#define NBIF1EPF3CFG_BASE_ADDR_1_ADDRESS_HYGX    (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x43010)

// Audio
#define NBIF3RCCFG_VENDOR_ID_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x00000)

#define NBIF3EPF3CFG_VENDOR_ID_ADDRESS_HYGX      (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x43000)
#define NBIF3EPF3CFG_COMMAND_ADDRESS_HYGX        (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x43004)
#define NBIF3EPF3CFG_BAR0_ADDRESS_HYGX           (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x43010)
#define NBIF3EPF3CFG_ADAPTER_ID_ADDRESS_HYGX     (UINT32)((NBIO0_NBIF3_APERTURE_ID_HYGX << 20) + 0x4304C)

// Type
#define RCC_DEV0_PORT_STRAP2_TYPE           TYPE_SMN

#define STRAP_ECRC_CHECK_EN_DEV0_OFFSET     4
#define STRAP_ECRC_CHECK_EN_DEV0_WIDTH      1
#define STRAP_ECRC_CHECK_EN_DEV0_MASK       0x10
#define STRAP_ECRC_GEN_EN_DEV0_OFFSET       5
#define STRAP_ECRC_GEN_EN_DEV0_WIDTH        1
#define STRAP_ECRC_GEN_EN_DEV0_MASK         0x20

// PCIE_DPC_STATUS
// Address
#define PCIE0_DPC_STATUS_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00398)
#define PCIE0_DPC_STATUS_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00398)

// Type
#define L2_TW_CONTROL_TYPE                                   TYPE_SMN

#define PCIE_DPC_STATUS_DPC_TRIGGER_STATUS_OFFSET            0
#define PCIE_DPC_STATUS_DPC_TRIGGER_STATUS_WIDTH             1
#define PCIE_DPC_STATUS_DPC_TRIGGER_STATUS_MASK              0x1
#define PCIE_DPC_STATUS_DPC_TRIGGER_REASON_OFFSET            1
#define PCIE_DPC_STATUS_DPC_TRIGGER_REASON_WIDTH             2
#define PCIE_DPC_STATUS_DPC_TRIGGER_REASON_MASK              0x6
#define PCIE_DPC_STATUS_DPC_INTERRUPT_STATUS_OFFSET          3
#define PCIE_DPC_STATUS_DPC_INTERRUPT_STATUS_WIDTH           1
#define PCIE_DPC_STATUS_DPC_INTERRUPT_STATUS_MASK            0x8
#define PCIE_DPC_STATUS_DPC_RP_BUSY_OFFSET                   4
#define PCIE_DPC_STATUS_DPC_RP_BUSY_WIDTH                    1
#define PCIE_DPC_STATUS_DPC_RP_BUSY_MASK                     0x10
#define PCIE_DPC_STATUS_DPC_TRIGGER_REASON_EXTENSION_OFFSET  5
#define PCIE_DPC_STATUS_DPC_TRIGGER_REASON_EXTENSION_WIDTH   2
#define PCIE_DPC_STATUS_DPC_TRIGGER_REASON_EXTENSION_MASK    0x60
#define PCIE_DPC_STATUS_Reserved_7_7_OFFSET                  7
#define PCIE_DPC_STATUS_Reserved_7_7_WIDTH                   1
#define PCIE_DPC_STATUS_Reserved_7_7_MASK                    0x80
#define PCIE_DPC_STATUS_RP_PIO_FIRST_ERROR_POINTER_OFFSET    8
#define PCIE_DPC_STATUS_RP_PIO_FIRST_ERROR_POINTER_WIDTH     5
#define PCIE_DPC_STATUS_RP_PIO_FIRST_ERROR_POINTER_MASK      0x1F00
#define PCIE_DPC_STATUS_Reserved_13_31_OFFSET                13
#define PCIE_DPC_STATUS_Reserved_13_31_WIDTH                 19
#define PCIE_DPC_STATUS_Reserved_13_31_MASK                  0xFFFFE000

/// PCIE_DPC_STATUS
typedef union {
  struct {
    UINT32    DPC_TRIGGER_STATUS           : 1;                        ///<
    UINT32    DPC_TRIGGER_REASON           : 2;                        ///<
    UINT32    DPC_INTERRUPT_STATUS         : 1;                        ///<
    UINT32    DPC_RP_BUSY                  : 1;                        ///<
    UINT32    DPC_TRIGGER_REASON_EXTENSION : 2;                        ///<
    UINT32    Reserved_7_7                 : 1;                        ///<
    UINT32    RP_PIO_FIRST_ERROR_POINTER   : 5;                        ///<
    UINT32    Reserved_13_31               : 19;                       ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_DPC_STATUS_STRUCT;

// Register Name IOHC_PCIE_CRS_Count
// Address
#define IOHC_PCIE_CRS_Count_ADDRESS_HYEX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10028)
#define IOHC_PCIE_CRS_Count_ADDRESS_HYGX            (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10028)

// Type
#define IOHC_PCIE_CRS_Count_TYPE                   TYPE_SMN

#define IOHC_PCIE_CRS_Count_CrsDelayCount_OFFSET   0
#define IOHC_PCIE_CRS_Count_CrsDelayCount_WIDTH    16
#define IOHC_PCIE_CRS_Count_CrsDelayCount_MASK     0xffff
#define IOHC_PCIE_CRS_Count_CrsLimitCount_OFFSET   16
#define IOHC_PCIE_CRS_Count_CrsLimitCount_WIDTH    12
#define IOHC_PCIE_CRS_Count_CrsLimitCount_MASK     0xfff0000
#define IOHC_PCIE_CRS_Count_Reserved_31_28_OFFSET  28
#define IOHC_PCIE_CRS_Count_Reserved_31_28_WIDTH   4
#define IOHC_PCIE_CRS_Count_Reserved_31_28_MASK    0xf0000000L

/// IOHCMISC_00000028
typedef union {
  struct {
    UINT32    CrsDelayCount  : 16;                                     ///<
    UINT32    CrsLimitCount  : 12;                                     ///<
    UINT32    Reserved_31_28 : 4;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOHC_PCIE_CRS_Count_STRUCT;

// Type
#define NB_BUS_NUM_CNTL_TYPE                       TYPE_SMN

// Address
#define NB_BUS_NUM_CNTL_ADDRESS_HYEX                (UINT32)((NBIO0_IOHC_APERTURE_ID_HYEX << 20) + 0x10044)
#define NB_BUS_NUM_CNTL_ADDRESS_HYGX                (UINT32)((NBIO0_IOHC_APERTURE_ID_HYGX << 20) + 0x10044)
/// IOHCMISC_00000044
typedef union {
  struct {
    UINT32    NB_BUS_NUM      : 8;                                 ///<
    UINT32    NB_BUS_LAT_Mode : 1;                                 ///<
    UINT32    Reserved_31_9   : 23;                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} NB_BUS_NUM_CNTL_STRUCT;

// Register Name IOMMU_MMIO_CONTROL1_W
// Address
#define IOMMU_MMIO_CONTROL1_W_ADDRESS_HYEX           (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x00084)
#define IOMMU_MMIO_CONTROL1_W_ADDRESS_HYGX           (UINT32)((NBIO0_L2IMU0_L2B_APERTURE_ID_HYGX << 20) + 0x00084)
// Type
#define IOMMU_MMIO_CONTROL1_W_TYPE                  TYPE_SMN

#define IOMMU_MMIO_CONTROL1_W_MARCnum_SUP_W_OFFSET  11
#define IOMMU_MMIO_CONTROL1_W_MARCnum_SUP_W_WIDTH   2
#define IOMMU_MMIO_CONTROL1_W_MARCnum_SUP_W_MASK    0x1800

/// IOMMU_MMIO_CONTROL1_W
typedef union {
  struct {
    UINT32    PAS_MAX_W                : 4;                     ///<
    UINT32    Reserved_5_4             : 2;                     ///<
    UINT32    DTE_seg_W                : 2;                     ///<
    UINT32    PPR_OVERFLOW_EARLY_SUP_W : 1;                     ///<
    UINT32    PPR_AUTORESP_SUP_W       : 1;                     ///<
    UINT32    BLOCK_STOPMARK_SUP_W     : 1;                     ///<
    UINT32    MARCnum_SUP_W            : 2;                     ///<
    UINT32    SNOOP_ATTRS_SUP_W        : 1;                     ///<
    UINT32    GIo_SUP_W                : 1;                     ///<
    UINT32    HA_SUP_W                 : 1;                     ///<
    UINT32    EPH_SUP_W                : 1;                     ///<
    UINT32    ATTRFW_SUP_W             : 1;                     ///<
    UINT32    V2_HD_DIS_SUP_W          : 1;                     ///<
    UINT32    InvIotlbTypeSup_W        : 1;                     ///<
    UINT32    HD_SUP_W                 : 1;                     ///<
    UINT32    VIOMMU_SUP_W             : 1;                     ///< Only HyGx support
    UINT32    VMGUARDIO_SUP_W          : 1;                     ///< Only HyGx support
    UINT32    VMTABLESIZE_W            : 4;                     ///< Only HyGx support
    UINT32    V2_HA_DIS_SUP_W          : 1;                     ///<
    UINT32    Reserved_31_28           : 4;                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOMMU_MMIO_CONTROL1_W_STRUCT;

// Register Name IOMMU_MMIO_EFR_0
// Address
#define IOMMU_MMIO_EFR_0_ADDRESS_HYEX                                   (UINT32)((NBIO0_L2IMU0_MMIO_APERTURE_ID_HYEX << 20) + 0x00030)
#define IOMMU_MMIO_EFR_0_ADDRESS_HYGX                                   (UINT32)((NBIO0_L2IMU0_MMIO_APERTURE_ID_HYGX << 20) + 0x00030)
#define IOMMU_MMIO_EFR_0_XT_SUP_MASK                                   4
#define IOMMU_MMIO_EFR_0_XT_SUP_OFFSET                                 2

// Register Name IOMMU_MMIO_CNTRL_1_HYGX
// Address
#define IOMMU_MMIO_CNTRL_1_ADDRESS_HYEX                                 (UINT32)((NBIO0_L2IMU0_MMIO_APERTURE_ID_HYEX << 20) + 0x0001C)
#define IOMMU_MMIO_CNTRL_1_ADDRESS_HYGX                                 (UINT32)((NBIO0_L2IMU0_MMIO_APERTURE_ID_HYGX << 20) + 0x0001C)
#define IOMMU_MMIO_CNTRL_1_XT_EN_MASK                                  0x40000
#define IOMMU_MMIO_CNTRL_1_XT_EN_OFFSET                                18
#define IOMMU_MMIO_CNTRL_1_IMU_XTInt_EN_MASK                           0x80000
#define IOMMU_MMIO_CNTRL_1_IMU_XTInt_EN_OFFSET                         19

// Register Name IOMMU_PRIVATE_CNTRL
// Address
#define IOMMU_MMIO_PRIVATE_CNTRL_ADDRESS_HYEX                           (UINT32)((NBIO0_L2IMU0_MMIO_APERTURE_ID_HYEX << 20) + 0x00400)
#define IOMMU_MMIO_PRIVATE_CNTRL_ADDRESS_HYGX                           (UINT32)((NBIO0_L2IMU0_MMIO_APERTURE_ID_HYGX << 20) + 0x00400)
#define IOMMU_MMIO_SPECIAL_INT_CHANGE_BROADCAST_DEST_IN_X2APIC_MASK    0x1
#define IOMMU_MMIO_SPECIAL_INT_CHANGE_BROADCAST_DEST_IN_X2APIC_OFFSET  0

// Register Name: IOMMU_MMIO_CONTROL0_W_HYGX
#define PCICFG_IOMMU_MMIO_CONTROL0_W_OFFSET                            0x80UL

typedef union {
  struct {
    UINT32    PREF_SUP_W     : 1;                                      ///<
    UINT32    PPR_SUP_W      : 1;                                      ///<
    UINT32    XT_SUP_W       : 1;                                      ///<
    UINT32    NX_SUP_W       : 1;                                      ///<
    UINT32    GT_SUP_W       : 1;                                      ///<
    UINT32    Reserved_5_5   : 1;                                      ///<
    UINT32    IA_SUP_W       : 1;                                      ///<
    UINT32    GA_SUP_W       : 1;                                      ///<
    UINT32    HE_SUP_W       : 1;                                      ///<
    UINT32    PC_SUP_W       : 1;                                      ///<
    UINT32    HATS_W         : 2;                                      ///<
    UINT32    US_SUP_W       : 1;                                      ///<
    UINT32    Reserved_20_13 : 8;                                      ///<
    UINT32    GAM_SUP_W      : 3;                                      ///<
    UINT32    PPRF_W         : 2;                                      ///<
    UINT32    Reserved_27_26 : 2;                                      ///<
    UINT32    EVENTF_W       : 2;                                      ///<
    UINT32    GLX_SUP_W      : 2;                                      ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} IOMMU_MMIO_CONTROL0_W_STRUCT;

#define RSMU_VF_ENABLE_NBIF2_ADDRESS_HYGX                           0x01074198

typedef union {
  struct {
    UINT32                                         RSMU_VF_ENABLE : 1;
    UINT32                                                        : 31;
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} RSMU_VF_ENABLE_NBIF2_STRUCT;

/// LINK_CNTL_32GT
 // Address
#define PCIE0_LINK_CNTL_32GT_ADDRESS_HYEX                           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x4A8)
#define PCIE1_LINK_CNTL_32GT_ADDRESS_HYEX                           (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x4A8)
#define PCIE2_LINK_CNTL_32GT_ADDRESS_HYEX                           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x4A8)

#define PCIE0_LINK_CNTL_32GT_ADDRESS_HYGX                           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x4A8)
#define PCIE2_LINK_CNTL_32GT_ADDRESS_HYGX                           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x4A8)

#define EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE_OFFSET         0
#define EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE_WIDTH          1
#define EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE_MASK           0x1
#define NO_EQUALIZATION_NEEDED_DISABLE_OFFSET                      1
#define NO_EQUALIZATION_NEEDED_DISABLE_WIDTH                       1
#define NO_EQUALIZATION_NEEDED_DISABLE_MASK                        0x2

typedef union {
   struct {
     UINT32      EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE       :1;     ///<
     UINT32      NO_EQUALIZATION_NEEDED_DISABLE                    :1;     ///<
     UINT32      RESERVED0                                         :30;    ///<
  } Field;
  UINT32    Value;
} LINK_CNTL_32GT_STRUCT;

#define PCIE0_PCIE_LC_CNTL12_ADDRESS_HYEX                           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40394)
#define PCIE1_PCIE_LC_CNTL12_ADDRESS_HYEX                           (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40394)
#define PCIE2_PCIE_LC_CNTL12_ADDRESS_HYEX                           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40394)

#define PCIE0_PCIE_LC_CNTL12_ADDRESS_HYGX                           (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40394)
#define PCIE2_PCIE_LC_CNTL12_ADDRESS_HYGX                           (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40394)

#define LC_TRANSMITTER_PRECODE_REQUEST_32GT_OFFSET                 9
#define LC_TRANSMITTER_PRECODE_REQUEST_32GT_WIDTH                  1
#define LC_TRANSMITTER_PRECODE_REQUEST_32GT_MASK                   0x200

typedef union {
  struct {
    UINT32      RESET_DIRECT_32GT_SPEED_CHANGE_ATTEMPT_FAILED               :1;     ///<
    UINT32      CLEAR_DIRECT_TO_32GT_AFTER_SPEED_CHANGE_ATTEMPT_FAILED      :1;     ///<
    UINT32      LC_AUTO_DIRECT_SPEED_CHANGE_TO_32GT_ATTEMPTS_ALLOWED        :2;     ///<
    UINT32      LC_DIRECT_32GT_SPEED_CHANGE_ATTEMPT_FAILED                  :1;     ///<
    UINT32      LC_REDO_EQ_AFTER_DIRECT_32GT_FAIL                           :1;     ///<
    UINT32      RESERVED0                                                   :1;     ///<
    UINT32      LC_ALWAYS_USEREGS_IN_NONEEDED                               :1;     ///<
    UINT32      RESERVED1                                                   :1;     ///<
    UINT32      LC_TRANSMITTER_PRECODE_REQUEST_32GT                         :1;     ///<
    UINT32      LC_OVERRODE_TX_PRECODE_REQUEST_32GT                         :1;     ///<
    UINT32      LC_OVERRODE_TX_PRECODE_REQUEST_EN_32GT                      :1;     ///<
    UINT32      RESERVED2                                                   :4;     ///<
    UINT32      LC_OVERRODE_PRESET_32GT                                     :1;     ///<
    UINT32      RESERVED3                                                   :8;     ///<
    UINT32      LC_EN_TX_PRECODE_BY_RCV_ANY                                 :1;     ///<
    UINT32      RESERVED4                                                   :2;     ///<
    UINT32      LC_SRNS_SKIP_IN_SRIS_HIGH                                   :4;     ///<
  } Field;                                                                          ///<
  UINT32    Value;                                                                  ///<
} PCIE_LC_CNTL12_STRUCT;

// PCIE_LC_CNTL8
// Address
#define PCIE0_PCIE_LC_CNTL8_ADDRESS_HYEX                            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40374)   
#define PCIE1_PCIE_LC_CNTL8_ADDRESS_HYEX                            (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40374)   
#define PCIE2_PCIE_LC_CNTL8_ADDRESS_HYEX                            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40374)   

#define PCIE0_PCIE_LC_CNTL8_ADDRESS_HYGX                            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40374)     
#define PCIE2_PCIE_LC_CNTL8_ADDRESS_HYGX                            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40374) 

#define LC_SPC_MODE_16GT_OFFSET                                    30
#define LC_SPC_MODE_16GT_WIDTH                                     2
#define LC_SPC_MODE_16GT_MASK                                      0xC0000000
#define LC_SPC_MODE_32GT_OFFSET                                    24
#define LC_SPC_MODE_32GT_WIDTH                                     2
#define LC_SPC_MODE_32GT_MASK                                      0x3000000

typedef union {
   struct {
     UINT32      LC_EQ_SEARCH_MODE_16GT                            :2;      ///<
     UINT32      LC_BYPASS_EQ_16GT                                 :1;      ///<
     UINT32      LC_BYPASS_EQ_PRESET_16GT                          :4;      ///<
     UINT32      LC_REDO_EQ_16GT                                   :1;      ///<
     UINT32      LC_USC_EQ_NOT_REQD_16GT                           :1;      ///<
     UINT32      LC_USC_GO_TO_EQ_16GT                              :1;      ///<
     UINT32      LC_UNEXPECTED_COEFFS_RCVD_16GT                    :1;      ///<
     UINT32      LC_BYPASS_EQ_REQ_PHASE_16GT                       :1;      ///<
     UINT32      LC_FORCE_PRESET_IN_EQ_REQ_PHASE_16GT              :1;      ///<
     UINT32      LC_FORCE_PRESET_VALUE_16GT                        :4;      ///<
     UINT32      LC_EQTS2_PRESET_EN                                :1;      ///<
     UINT32      LC_EQTS2_PRESET                                   :4;      ///<
     UINT32      LC_USE_EQTS2_PRESET                               :1;      ///<
     UINT32      RESERVED0                                         :1;      ///<
     UINT32      LC_SPC_MODE_32GT                                  :2;      ///<
     UINT32      RESERVED1                                         :2;      ///<
     UINT32      LC_8GT_EQ_REDO_EN                                 :1;      ///<
     UINT32      RESERVED2                                         :1;      ///<
     UINT32      LC_SPC_MODE_16GT                                  :2;      ///<
  } Field;                                                                  ///<
  UINT32    Value;                                                          ///<
} PCIE_LC_CNTL8_STRUCT;


// Register Name PCIE_LC_SPEED_CNTL

// Address
#define PCIE0_LC_SPEED_CNTL_ADDRESS_HYEX                                 (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40290) 
#define PCIE1_LC_SPEED_CNTL_ADDRESS_HYEX                                 (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40290) 
#define PCIE2_LC_SPEED_CNTL_ADDRESS_HYEX                                 (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40290) 

#define PCIE0_LC_SPEED_CNTL_ADDRESS_HYGX                                 (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40290)  
#define PCIE2_LC_SPEED_CNTL_ADDRESS_HYGX                                 (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40290)  

// Field Definitions
#define PCIE_LC_SPEED_CNTL_LC_GEN2_EN_STRAP_OFFSET                      0
#define PCIE_LC_SPEED_CNTL_LC_GEN2_EN_STRAP_WIDTH                       1
#define PCIE_LC_SPEED_CNTL_LC_GEN2_EN_STRAP_MASK                        0x1
#define PCIE_LC_SPEED_CNTL_LC_GEN3_EN_STRAP_OFFSET                      1
#define PCIE_LC_SPEED_CNTL_LC_GEN3_EN_STRAP_WIDTH                       1
#define PCIE_LC_SPEED_CNTL_LC_GEN3_EN_STRAP_MASK                        0x2
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_EN_OFFSET      2
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_EN_WIDTH       1
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_EN_MASK        0x4
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_OFFSET         3
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_WIDTH          2
#define PCIE_LC_SPEED_CNTL_LC_TARGET_LINK_SPEED_OVERRIDE_MASK           0x18
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_SW_SPEED_CHANGE_OFFSET           5
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_SW_SPEED_CHANGE_WIDTH            1
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_SW_SPEED_CHANGE_MASK             0x20
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_SW_SPEED_CHANGE_OFFSET          6
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_SW_SPEED_CHANGE_WIDTH           1
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_SW_SPEED_CHANGE_MASK            0x40
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_HW_SPEED_CHANGE_OFFSET           7
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_HW_SPEED_CHANGE_WIDTH            1
#define PCIE_LC_SPEED_CNTL_LC_FORCE_EN_HW_SPEED_CHANGE_MASK             0x80
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_HW_SPEED_CHANGE_OFFSET          8
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_HW_SPEED_CHANGE_WIDTH           1
#define PCIE_LC_SPEED_CNTL_LC_FORCE_DIS_HW_SPEED_CHANGE_MASK            0x100
#define PCIE_LC_SPEED_CNTL_LC_INITIATE_LINK_SPEED_CHANGE_OFFSET         9
#define PCIE_LC_SPEED_CNTL_LC_INITIATE_LINK_SPEED_CHANGE_WIDTH          1
#define PCIE_LC_SPEED_CNTL_LC_INITIATE_LINK_SPEED_CHANGE_MASK           0x200
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPTS_ALLOWED_OFFSET      10
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPTS_ALLOWED_WIDTH       2
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPTS_ALLOWED_MASK        0xc00
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPT_FAILED_OFFSET        12
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPT_FAILED_WIDTH         1
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_ATTEMPT_FAILED_MASK          0x1000
#define PCIE_LC_SPEED_CNTL_LC_CURRENT_DATA_RATE_OFFSET                  13
#define PCIE_LC_SPEED_CNTL_LC_CURRENT_DATA_RATE_WIDTH                   2
#define PCIE_LC_SPEED_CNTL_LC_CURRENT_DATA_RATE_MASK                    0x6000
#define PCIE_LC_SPEED_CNTL_LC_DONT_CLR_TARGET_SPD_CHANGE_STATUS_OFFSET  15
#define PCIE_LC_SPEED_CNTL_LC_DONT_CLR_TARGET_SPD_CHANGE_STATUS_WIDTH   1
#define PCIE_LC_SPEED_CNTL_LC_DONT_CLR_TARGET_SPD_CHANGE_STATUS_MASK    0x8000
#define PCIE_LC_SPEED_CNTL_LC_CLR_FAILED_SPD_CHANGE_CNT_OFFSET          16
#define PCIE_LC_SPEED_CNTL_LC_CLR_FAILED_SPD_CHANGE_CNT_WIDTH           1
#define PCIE_LC_SPEED_CNTL_LC_CLR_FAILED_SPD_CHANGE_CNT_MASK            0x10000
#define PCIE_LC_SPEED_CNTL_LC_1_OR_MORE_TS2_SPEED_ARC_EN_OFFSET         17
#define PCIE_LC_SPEED_CNTL_LC_1_OR_MORE_TS2_SPEED_ARC_EN_WIDTH          1
#define PCIE_LC_SPEED_CNTL_LC_1_OR_MORE_TS2_SPEED_ARC_EN_MASK           0x20000
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN2_OFFSET          18
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN2_WIDTH           1
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN2_MASK            0x40000
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN2_OFFSET           19
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN2_WIDTH            1
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN2_MASK             0x80000
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN3_OFFSET          20
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN3_WIDTH           1
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_EVER_SENT_GEN3_MASK            0x100000
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN3_OFFSET           21
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN3_WIDTH            1
#define PCIE_LC_SPEED_CNTL_LC_OTHER_SIDE_SUPPORTS_GEN3_MASK             0x200000
#define PCIE_LC_SPEED_CNTL_LC_AUTO_RECOVERY_DIS_OFFSET                  22
#define PCIE_LC_SPEED_CNTL_LC_AUTO_RECOVERY_DIS_WIDTH                   1
#define PCIE_LC_SPEED_CNTL_LC_AUTO_RECOVERY_DIS_MASK                    0x400000
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_STATUS_OFFSET                23
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_STATUS_WIDTH                 1
#define PCIE_LC_SPEED_CNTL_LC_SPEED_CHANGE_STATUS_MASK                  0x800000
#define PCIE_LC_SPEED_CNTL_LC_DATA_RATE_ADVERTISED_OFFSET               24
#define PCIE_LC_SPEED_CNTL_LC_DATA_RATE_ADVERTISED_WIDTH                2
#define PCIE_LC_SPEED_CNTL_LC_DATA_RATE_ADVERTISED_MASK                 0x3000000
#define PCIE_LC_SPEED_CNTL_LC_CHECK_DATA_RATE_OFFSET                    26
#define PCIE_LC_SPEED_CNTL_LC_CHECK_DATA_RATE_WIDTH                     1
#define PCIE_LC_SPEED_CNTL_LC_CHECK_DATA_RATE_MASK                      0x4000000
#define PCIE_LC_SPEED_CNTL_LC_MULT_UPSTREAM_AUTO_SPD_CHNG_EN_OFFSET     27
#define PCIE_LC_SPEED_CNTL_LC_MULT_UPSTREAM_AUTO_SPD_CHNG_EN_WIDTH      1
#define PCIE_LC_SPEED_CNTL_LC_MULT_UPSTREAM_AUTO_SPD_CHNG_EN_MASK       0x8000000
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L0s_EN_OFFSET           28
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L0s_EN_WIDTH            1
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L0s_EN_MASK             0x10000000
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L1_EN_OFFSET            29
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L1_EN_WIDTH             1
#define PCIE_LC_SPEED_CNTL_LC_INIT_SPEED_NEG_IN_L1_EN_MASK              0x20000000
#define PCIE_LC_SPEED_CNTL_LC_DONT_CHECK_EQTS_IN_RCFG_OFFSET            30
#define PCIE_LC_SPEED_CNTL_LC_DONT_CHECK_EQTS_IN_RCFG_WIDTH             1
#define PCIE_LC_SPEED_CNTL_LC_DONT_CHECK_EQTS_IN_RCFG_MASK              0x40000000
#define PCIE_LC_SPEED_CNTL_LC_DELAY_COEFF_UPDATE_DIS_OFFSET             31
#define PCIE_LC_SPEED_CNTL_LC_DELAY_COEFF_UPDATE_DIS_WIDTH              1
#define PCIE_LC_SPEED_CNTL_LC_DELAY_COEFF_UPDATE_DIS_MASK               0x80000000L

/// PCIE_LC_SPEED_CNTL
typedef union {
  struct {
    UINT32    LC_GEN2_EN_STRAP                     : 1;                ///<
    UINT32    LC_GEN3_EN_STRAP                     : 1;                ///<
    UINT32    LC_GEN4_EN_STRAP                     : 1;                ///<
    UINT32    LC_GEN5_EN_STRAP                     : 1;                ///<
    UINT32    Reserved                             : 1;                ///<
    UINT32    LC_FORCE_EN_SW_SPEED_CHANGE          : 1;                ///<
    UINT32    LC_FORCE_DIS_SW_SPEED_CHANGE         : 1;                ///<
    UINT32    LC_FORCE_EN_HW_SPEED_CHANGE          : 1;                ///<
    UINT32    LC_FORCE_DIS_HW_SPEED_CHANGE         : 1;                ///<
    UINT32    LC_INITIATE_LINK_SPEED_CHANGE        : 1;                ///<
    UINT32    LC_SPEED_CHANGE_ATTEMPTS_ALLOWED     : 2;                ///<
    UINT32    LC_SPEED_CHANGE_ATTEMPT_FAILED       : 1;                ///<
    UINT32    LC_CURRENT_DATA_RATE                 : 2;                ///<
    UINT32    LC_DONT_CLR_TARGET_SPD_CHANGE_STATUS : 1;                ///<
    UINT32    LC_CLR_FAILED_SPD_CHANGE_CNT         : 1;                ///<
    UINT32    LC_1_OR_MORE_TS2_SPEED_ARC_EN        : 1;                ///<
    UINT32    LC_OTHER_SIDE_EVER_SENT_GEN2         : 1;                ///<
    UINT32    LC_OTHER_SIDE_SUPPORTS_GEN2          : 1;                ///<
    UINT32    LC_OTHER_SIDE_EVER_SENT_GEN3         : 1;                ///<
    UINT32    LC_OTHER_SIDE_SUPPORTS_GEN3          : 1;                ///<
    UINT32    LC_AUTO_RECOVERY_DIS                 : 1;                ///<
    UINT32    LC_SPEED_CHANGE_STATUS               : 1;                ///<
    UINT32    LC_DATA_RATE_ADVERTISED              : 2;                ///<
    UINT32    LC_CHECK_DATA_RATE                   : 1;                ///<
    UINT32    LC_MULT_UPSTREAM_AUTO_SPD_CHNG_EN    : 1;                ///<
    UINT32    LC_INIT_SPEED_NEG_IN_L0s_EN          : 1;                ///<
    UINT32    LC_INIT_SPEED_NEG_IN_L1_EN           : 1;                ///<
    UINT32    LC_DONT_CHECK_EQTS_IN_RCFG           : 1;                ///<
    UINT32    LC_DELAY_COEFF_UPDATE_DIS            : 1;                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_SPEED_CNTL_STRUCT;

// Address
#define PCIE0_LC_SPEED_STATUS_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40398) 
#define PCIE1_LC_SPEED_STATUS_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40398) 
#define PCIE2_LC_SPEED_STATUS_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40398) 

#define PCIE0_LC_SPEED_STATUS_ADDRESS_HYGX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40398) 
#define PCIE2_LC_SPEED_STATUS_ADDRESS_HYGX             (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40398) 
/// PCIE_LC_SPEED_STATUS
typedef union {
  struct {
    UINT32    LC_CURRENT_DATA_RATE_SHADOW         : 3;                 ///<
    UINT32    Reserved                            : 1;                 ///<
    UINT32    LC_DATA_RATE_ADVERTISED_SHADOW      : 3;                 ///<
    UINT32    Reserved0                           : 9;                 ///<
    UINT32    LC_OTHER_SIDE_EVER_SENT_GEN2_SHADOW : 1;                 ///<
    UINT32    LC_OTHER_SIDE_SUPPORTS_GEN2_SHADOW  : 1;                 ///<
    UINT32    LC_OTHER_SIDE_EVER_SENT_GEN3_SHADOW : 1;                 ///<
    UINT32    LC_OTHER_SIDE_SUPPORTS_GEN3_SHADOW  : 1;                 ///<
    UINT32    LC_OTHER_SIDE_EVER_SENT_GEN4_SHADOW : 1;                 ///<
    UINT32    LC_OTHER_SIDE_SUPPORTS_GEN4_SHADOW  : 1;                 ///<
    UINT32    LC_OTHER_SIDE_EVER_SENT_GEN5        : 1;                 ///<
    UINT32    LC_OTHER_SIDE_SUPPORTS_GEN5         : 1;                 ///<
    UINT32    Reserved1                           : 8;                 ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_SPEED_STATUS_STRUCT;

// Address
#define PCIE0_LC_STATE0_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40294)
#define PCIE1_LC_STATE0_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x40294)
#define PCIE2_LC_STATE0_ADDRESS_HYEX             (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x40294)

#define PCIE0_LC_STATE0_ADDRESS_HYGX             (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40294)
#define PCIE2_LC_STATE0_ADDRESS_HYGX             (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x40294)

#define PCIE_LC_STATE0_LC_CURRENT_STATE_OFFSET  0
#define PCIE_LC_STATE0_LC_CURRENT_STATE_WIDTH   6
#define PCIE_LC_STATE0_LC_CURRENT_STATE_MASK    0x3f
#define PCIE_LC_STATE0_Reserved0_OFFSET         6
#define PCIE_LC_STATE0_Reserved0_WIDTH          2
#define PCIE_LC_STATE0_Reserved0_MASK           0xc0
#define PCIE_LC_STATE0_LC_PREV_STATE1_OFFSET    8
#define PCIE_LC_STATE0_LC_PREV_STATE1_WIDTH     6
#define PCIE_LC_STATE0_LC_PREV_STATE1_MASK      0x3f00
#define PCIE_LC_STATE0_Reserved1_OFFSET         14
#define PCIE_LC_STATE0_Reserved1_WIDTH          2
#define PCIE_LC_STATE0_Reserved1_MASK           0xc000
#define PCIE_LC_STATE0_LC_PREV_STATE2_OFFSET    16
#define PCIE_LC_STATE0_LC_PREV_STATE2_WIDTH     6
#define PCIE_LC_STATE0_LC_PREV_STATE2_MASK      0x3f0000
#define PCIE_LC_STATE0_Reserved2_OFFSET         22
#define PCIE_LC_STATE0_Reserved2_WIDTH          2
#define PCIE_LC_STATE0_Reserved2_MASK           0xc00000
#define PCIE_LC_STATE0_LC_PREV_STATE3_OFFSET    24
#define PCIE_LC_STATE0_LC_PREV_STATE3_WIDTH     6
#define PCIE_LC_STATE0_LC_PREV_STATE3_MASK      0x3f000000
#define PCIE_LC_STATE0_Reserved3_OFFSET         30
#define PCIE_LC_STATE0_Reserved3_WIDTH          2
#define PCIE_LC_STATE0_Reserved3_MASK           0xc0000000

#define PCIE0_LC_CNTL_10_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x4038C) 
#define PCIE1_LC_CNTL_10_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x4038C) 
#define PCIE2_LC_CNTL_10_ADDRESS_HYEX            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x4038C) 

#define PCIE0_LC_CNTL_10_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x4038C) 
#define PCIE2_LC_CNTL_10_ADDRESS_HYGX            (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x4038C) 

typedef union {
    struct {
        UINT32                              LC_LINK_DOWN_SPD_CHG_EN:1;   
        UINT32                    LC_CLR_DELAY_DLLP_WHEN_NO_AUTO_EQ:1;    
        UINT32                    LC_MULT_AUTO_SPD_CHG_ON_LAST_RATE:1;  
        UINT32               LC_CLEAR_RXSTANDBY_ON_RATE_UPDATE_ONLY:1;
        UINT32        LC_POWERDOWN_P0_WAIT_FOR_REFCLKACK_ON_L1_EXIT:1;
        UINT32                                          RESERVE_5_7:3;
        UINT32                     LC_TARGET_LINK_SPEED_OVERRIDE_EN:1;
        UINT32                        LC_TARGET_LINK_SPEED_OVERRIDE:3;
        UINT32                                        RESERVE_11_15:4;
        UINT32                       LC_CLEAR_REVERSE_ATTEMPT_IN_L0:1;
        UINT32                                     LC_LOCK_REVERSAL:1;
        UINT32                                           RESERVE_18:1;
        UINT32                          LC_WAIT_FOR_LANES_IN_CONFIG:1;
        UINT32                        LC_REQ_COEFFS_FOR_TXMARGIN_EN:1;
        UINT32                         LC_AUTO_REJECT_AFTER_TIMEOUT:1; 
        UINT32                                         LC_ESM_RATES:2;
        UINT32                                    LC_ESM_ENTRY_MODE:1;  
        UINT32                                        RESERVE_25_29:5;
        UINT32                         LC_OTHER_SIDE_EVER_SENT_GEN4:1;  
        UINT32                          LC_OTHER_SIDE_SUPPORTS_GEN4:1;
    }Field;
    UINT32 Value;
}PCIE_LC_CNTL_10;

/// PCIE_LC_STATE0
typedef union {
  struct {
    UINT32    LC_CURRENT_STATE : 6;                                    ///<
    UINT32    Reserved0        : 2;                                    ///<
    UINT32    LC_PREV_STATE1   : 6;                                    ///<
    UINT32    Reserved1        : 2;                                    ///<
    UINT32    LC_PREV_STATE2   : 6;                                    ///<
    UINT32    Reserved2        : 2;                                    ///<
    UINT32    LC_PREV_STATE3   : 6;                                    ///<
    UINT32    Reserved3        : 2;                                    ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_STATE0_STRUCT;
// Address
#define PCIE0_SWRST_CONTROL_6_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x80428) 
#define PCIE1_SWRST_CONTROL_6_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x80428) 
#define PCIE2_SWRST_CONTROL_6_ADDRESS_HYEX               (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x80428) 

#define PCIE0_SWRST_CONTROL_6_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x80428) 
#define PCIE2_SWRST_CONTROL_6_ADDRESS_HYGX               (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x80428) 

#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_A_OFFSET  0
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_A_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_A_MASK    0x1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_B_OFFSET  1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_B_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_B_MASK    0x2
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_C_OFFSET  2
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_C_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_C_MASK    0x4
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_D_OFFSET  3
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_D_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_D_MASK    0x8
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_E_OFFSET  4
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_E_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_E_MASK    0x10
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_F_OFFSET  5
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_F_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_F_MASK    0x20
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_G_OFFSET  6
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_G_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_G_MASK    0x40
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_H_OFFSET  7
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_H_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_H_MASK    0x80
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_I_OFFSET  8
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_I_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_I_MASK    0x100
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_J_OFFSET  9
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_J_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_J_MASK    0x200
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_K_OFFSET  10
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_K_WIDTH   1
#define PCIE_SWRST_CNTL6_STRUCT_HOLD_TRAINING_K_MASK    0x400

/// PCIE_SWRST_CNTL6_STRUCT
typedef union {
  struct {
    UINT32    HOLD_TRAINING_A : 1;                                     ///<
    UINT32    HOLD_TRAINING_B : 1;                                     ///<
    UINT32    HOLD_TRAINING_C : 1;                                     ///<
    UINT32    HOLD_TRAINING_D : 1;                                     ///<
    UINT32    HOLD_TRAINING_E : 1;                                     ///<
    UINT32    HOLD_TRAINING_F : 1;                                     ///<
    UINT32    HOLD_TRAINING_G : 1;                                     ///<
    UINT32    HOLD_TRAINING_H : 1;                                     ///<
    UINT32    HOLD_TRAINING_I : 1;                                     ///<
    UINT32    HOLD_TRAINING_J : 1;                                     ///<
    UINT32    HOLD_TRAINING_K : 1;                                     ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_SWRST_CNTL6_STRUCT;

// Register Name PCIE_LC_CNTL5

// Address
#define PCIE0_LC_CNTL5_ADDRESS_HYEX                              (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x402DC) 
#define PCIE1_LC_CNTL5_ADDRESS_HYEX                              (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x402DC) 
#define PCIE2_LC_CNTL5_ADDRESS_HYEX                              (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x402DC) 

#define PCIE0_LC_CNTL5_ADDRESS_HYGX                              (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x402DC) 
#define PCIE2_LC_CNTL5_ADDRESS_HYGX                              (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x402DC) 

// Type
#define PCIE_LC_CNTL5_TYPE                                       TYPE_SMN

#define PCIE_LC_CNTL5_LC_EQ_FS_0_OFFSET                          0
#define PCIE_LC_CNTL5_LC_EQ_FS_0_WIDTH                           6
#define PCIE_LC_CNTL5_LC_EQ_FS_0_MASK                            0x3f
#define PCIE_LC_CNTL5_LC_EQ_FS_8_OFFSET                          6
#define PCIE_LC_CNTL5_LC_EQ_FS_8_WIDTH                           6
#define PCIE_LC_CNTL5_LC_EQ_FS_8_MASK                            0xfc0
#define PCIE_LC_CNTL5_LC_EQ_LF_0_OFFSET                          12
#define PCIE_LC_CNTL5_LC_EQ_LF_0_WIDTH                           6
#define PCIE_LC_CNTL5_LC_EQ_LF_0_MASK                            0x3f000
#define PCIE_LC_CNTL5_LC_EQ_LF_8_OFFSET                          18
#define PCIE_LC_CNTL5_LC_EQ_LF_8_WIDTH                           6
#define PCIE_LC_CNTL5_LC_EQ_LF_8_MASK                            0xfc0000
#define PCIE_LC_CNTL5_LC_DSC_EQ_FS_LF_INVALID_TO_PRESETS_OFFSET  24
#define PCIE_LC_CNTL5_LC_DSC_EQ_FS_LF_INVALID_TO_PRESETS_WIDTH   1
#define PCIE_LC_CNTL5_LC_DSC_EQ_FS_LF_INVALID_TO_PRESETS_MASK    0x1000000
#define PCIE_LC_CNTL5_LC_TX_SWING_OVERRIDE_OFFSET                25
#define PCIE_LC_CNTL5_LC_TX_SWING_OVERRIDE_WIDTH                 1
#define PCIE_LC_CNTL5_LC_TX_SWING_OVERRIDE_MASK                  0x2000000
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_OFFSET               26
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_WIDTH                1
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_MASK                 0x4000000
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_TEST_OFFSET          27
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_TEST_WIDTH           1
#define PCIE_LC_CNTL5_LC_ACCEPT_ALL_PRESETS_TEST_MASK            0x8000000
#define PCIE_LC_CNTL5_LC_WAIT_IN_DETECT_OFFSET                   28
#define PCIE_LC_CNTL5_LC_WAIT_IN_DETECT_WIDTH                    1
#define PCIE_LC_CNTL5_LC_WAIT_IN_DETECT_MASK                     0x10000000
#define PCIE_LC_CNTL5_LC_HOLD_TRAINING_MODE_OFFSET               29
#define PCIE_LC_CNTL5_LC_HOLD_TRAINING_MODE_WIDTH                3
#define PCIE_LC_CNTL5_LC_HOLD_TRAINING_MODE_MASK                 0xe0000000L

/// PCIE_LC_CNTL5_STRUCT
typedef union {
  struct {
    UINT32    LC_EQ_FS_0                         : 6;                  ///<
    UINT32    LC_EQ_FS_8                         : 6;                  ///<
    UINT32    LC_EQ_LF_0                         : 6;                  ///<
    UINT32    LC_EQ_LF_8                         : 6;                  ///<
    UINT32    LC_DSC_EQ_FS_LF_INVALID_TO_PRESETS : 1;                  ///<
    UINT32    LC_TX_SWING_OVERRIDE               : 1;                  ///<
    UINT32    LC_ACCEPT_ALL_PRESETS              : 1;                  ///<
    UINT32    LC_ACCEPT_ALL_PRESETS_TEST         : 1;                  ///<
    UINT32    LC_WAIT_IN_DETECT                  : 1;                  ///<
    UINT32    LC_HOLD_TRAINING_MODE              : 3;                  ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LC_CNTL5_STRUCT;

/// PCIE_LINK_CAP
#define RC_PCIE_LINK_CAP_OFFSET    0x64  
typedef union {
  struct {
    UINT32                                              LINK_SPEED:4 ;
    UINT32                                              LINK_WIDTH:6 ;
    UINT32                                              PM_SUPPORT:2 ;
    UINT32                                        L0S_EXIT_LATENCY:3 ;
    UINT32                                         L1_EXIT_LATENCY:3 ;
    UINT32                                  CLOCK_POWER_MANAGEMENT:1 ;
    UINT32                             SURPRISE_DOWN_ERR_ERPORTING:1 ;
    UINT32                             DL_ACTIVE_ERPORTING_CAPABLE:1 ;
    UINT32                                 LINK_BW_NOTFICATION_CAP:1 ;
    UINT32                              ASM_OPTIONALITY_COMPLIANCE:1 ;
    UINT32                                             RESERVER_23:1 ;
    UINT32                                             PORT_NUMBER:8 ;
  } Field;                                                             
  UINT32 Value;                                                        
} PCIE_LINK_CAP_STRUCT;

#define SMN_PCS20_0x12E0b0c8_ADDRESS_HYEX                   0x12E0b0c8
#define SMN_PCS21_0x12F0b0c8_ADDRESS_HYEX                   0x12F0b0c8

// Address
#define RC_PCIE_LINK_STATUS_OFFSET                         0x68
#define PCIE0_LINK_STATUS_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00068) 
#define PCIE1_LINK_STATUS_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00068) 
#define PCIE2_LINK_STATUS_ADDRESS_HYEX                      (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00068) 

#define PCIE0_LINK_STATUS_ADDRESS_HYGX                      (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00068) 
#define PCIE2_LINK_STATUS_ADDRESS_HYGX                      (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00068) 

// Type
#define PCIE_LINK_STATUS_TYPE                              TYPE_SMN

#define PCIE_LINK_STATUS_CURRENT_LINK_SPEED_OFFSET         16
#define PCIE_LINK_STATUS_CURRENT_LINK_SPEED_WIDTH          4
#define PCIE_LINK_STATUS_CURRENT_LINK_SPEED_MASK           0xf0000
#define PCIE_LINK_STATUS_NEGOTIATED_LINK_WIDTH_OFFSET      20
#define PCIE_LINK_STATUS_NEGOTIATED_LINK_WIDTH_WIDTH       6
#define PCIE_LINK_STATUS_NEGOTIATED_LINK_WIDTH_MASK        0x3f00000
#define PCIE_LINK_STATUS_LINK_TRAINING_OFFSET              27
#define PCIE_LINK_STATUS_LINK_TRAINING_WIDTH               1
#define PCIE_LINK_STATUS_LINK_TRAINING_MASK                0x8000000
#define PCIE_LINK_STATUS_SLOT_CLOCK_CFG_OFFSET             28
#define PCIE_LINK_STATUS_SLOT_CLOCK_CFG_WIDTH              1
#define PCIE_LINK_STATUS_SLOT_CLOCK_CFG_MASK               0x10000000
#define PCIE_LINK_STATUS_DL_ACTIVE_OFFSET                  29
#define PCIE_LINK_STATUS_DL_ACTIVE_WIDTH                   1
#define PCIE_LINK_STATUS_DL_ACTIVE_MASK                    0x20000000
#define PCIE_LINK_STATUS_LINK_BW_MANAGEMENT_STATUS_OFFSET  30
#define PCIE_LINK_STATUS_LINK_BW_MANAGEMENT_STATUS_WIDTH   1
#define PCIE_LINK_STATUS_LINK_BW_MANAGEMENT_STATUS_MASK    0x40000000
#define PCIE_LINK_STATUS_LINK_AUTONOMOUS_BW_STATUS_OFFSET  31
#define PCIE_LINK_STATUS_LINK_AUTONOMOUS_BW_STATUS_WIDTH   1
#define PCIE_LINK_STATUS_LINK_AUTONOMOUS_BW_STATUS_MASK    0x80000000

/// PCIE_LINK_STATUS

typedef union {
  struct {
    UINT32                                                reserved:16; ///<
    UINT32                                      CURRENT_LINK_SPEED:4 ; ///<
    UINT32                                   NEGOTIATED_LINK_WIDTH:6 ; ///<
    UINT32                                             reserved_10:1 ; ///<
    UINT32                                           LINK_TRAINING:1 ; ///<
    UINT32                                          SLOT_CLOCK_CFG:1 ; ///<
    UINT32                                               DL_ACTIVE:1 ; ///<
    UINT32                               LINK_BW_MANAGEMENT_STATUS:1 ; ///<
    UINT32                               LINK_AUTONOMOUS_BW_STATUS:1 ; ///<
  } Field;                                                             ///<
  UINT32 Value;                                                        ///<
} PCIE_LINK_STATUS_STRUCT;
 
/// PCIE_LINK_CNTL
#define RC_PCIE_LINK_CNTL_OFFSET                                  0x68  
#define PCIE0_LINK_CNTL_ADDRESS_HYGX                               (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00068) 
#define PCIE2_LINK_CNTL_ADDRESS_HYGX                               (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00068) 

typedef union {
  struct {
    UINT32                                              PM_CONTROL:2 ;
    UINT32                                              RESERVED_2:1 ;
    UINT32                                       READ_CPL_BOUNDARY:1 ;
    UINT32                                                LINK_DIS:1 ;
    UINT32                                            RETRAIN_LINK:1 ;
    UINT32                                        COMMON_CLOCK_CFG:1 ;
    UINT32                                           EXTENDED_SYNC:1 ;
    UINT32                               CLOCK_POWER_MANAGEMENT_EN:1 ;
    UINT32                             HW_AUTONOMOUS_WIDTH_DISABLE:1 ;
    UINT32                               LINK_BW_MANAGEMENT_INT_EN:1 ;
    UINT32                                LINK_AUTONMOUS_BW_INT_EN:1 ;
    UINT32                                          RESERVED_12_13:2 ;
    UINT32                                   DRS_SIGNALING_CONTROL:2 ;
    UINT32                                                reserved:16;
  } Field;                                                             
  UINT32 Value;                                                        
} PCIE_LINK_CNTL_STRUCT;

/// PCIE_LINK_CNTL3
#define RC_PCIE_LINK_CNTL3_OFFSET                      0x274
#define PCIE0_LINK_CNTL3_ADDRESS_HYEX                   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00274) 
#define PCIE1_LINK_CNTL3_ADDRESS_HYEX                   (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00274) 
#define PCIE2_LINK_CNTL3_ADDRESS_HYEX                   (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00274) 

#define PCIE0_LINK_CNTL3_ADDRESS_HYGX                   (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00274) 
#define PCIE2_LINK_CNTL3_ADDRESS_HYGX                   (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00274) 

typedef union {
  struct {
    UINT32                                    PERFORM_EQUALIZATION:1 ;
    UINT32                            LINK_EQUALIZATION_REQ_INT_EN:1 ;
    UINT32                                            RESERVED_2_8:7 ;
    UINT32                                ENABLE_LOWER_SKIP_OS_GEN:7 ;
    UINT32                                                reserved:16;
  } Field;                                                             
  UINT32 Value;                                                        
} PCIE_LINK_CNTL3_STRUCT;

/// PCIE_LANE_EQUALIZATION_CNTL Gen3
#define PCIE_LANE_EQUALIZATION_CNTL_OFFSET               0x27C

typedef union {
  struct {
    UINT16    DOWNSTREAM_PORT_TX_PRESET         : 4;                        ///<
    UINT16    DOWNSTREAM_PORT_RX_PRESET_HINT    : 3;                        ///<
    UINT16    Reserved_0                        : 1;                        ///<
    UINT16    UPSTREAM_PORT_TX_PRESET           : 4;                        ///<
    UINT16    UPSTREAM_PORT_RX_PRESET_HINT      : 3;                        ///<
    UINT16    Reserved_1                        : 1;                        ///<
  } Field;                                                                  ///<
  UINT16    Value;                                                          ///<
} PCIE_LANE_EQ_CNTL_STRUCT;

/// PCIE_LANE_EQUALIZATION_CNTL_16GT Gen4
#define PCIE_LANE_EQUALIZATION_CNTL_16GT_OFFSET          0x440
#define PCIE0_LANE_EQUALIZATION_CNTL_16GT_ADDRESS_HYEX    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00440) 
#define PCIE1_LANE_EQUALIZATION_CNTL_16GT_ADDRESS_HYEX    (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00440) 
#define PCIE2_LANE_EQUALIZATION_CNTL_16GT_ADDRESS_HYEX    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00440) 

#define PCIE0_LANE_EQUALIZATION_CNTL_16GT_ADDRESS_HYGX    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00440) 
#define PCIE2_LANE_EQUALIZATION_CNTL_16GT_ADDRESS_HYGX    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00440)

typedef union {
  struct {
    UINT8    DOWNSTREAM_PORT_TX_PRESET          : 4;                        ///<
    UINT8    UPSTREAM_PORT_TX_PRESET            : 4;                        ///< 
  } Field;                                                                  ///<
  UINT8    Value;                                                           ///<
} PCIE_LANE_EQ_CNTL_16GT_STRUCT;

///RC_PCIE_DEVICE_STATUS
#define RC_PCIE_DEVICE_CNTL_AND_STATUS_OFFSET       0x60
#define PCIE0_DEVICE_CNTL_AND_STATUS_ADDRESS_HYEX    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00060) 
#define PCIE1_DEVICE_CNTL_AND_STATUS_ADDRESS_HYEX    (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00060) 
#define PCIE2_DEVICE_CNTL_AND_STATUS_ADDRESS_HYEX    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00060) 
#define PCIE0_DEVICE_CNTL_AND_STATUS_ADDRESS_HYGX    (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00060) 
#define PCIE2_DEVICE_CNTL_AND_STATUS_ADDRESS_HYGX    (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00060)

///RC_PCIE_UNCORR_ERR_STATUS
#define RC_PCIE_UNCORR_ERR_STATUS_OFFSET        0x154
#define PCIE0_UNCORR_ERR_STATUS_ADDRESS_HYEX     (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00154) 
#define PCIE1_UNCORR_ERR_STATUS_ADDRESS_HYEX     (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00154) 
#define PCIE2_UNCORR_ERR_STATUS_ADDRESS_HYEX     (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00154) 
#define PCIE0_UNCORR_ERR_STATUS_ADDRESS_HYGX     (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00154) 
#define PCIE2_UNCORR_ERR_STATUS_ADDRESS_HYGX     (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00154)

///RC_PCIE_CORR_ERR_STATUS
#define RC_PCIE_CORR_ERR_STATUS_OFFSET          0x160
#define PCIE0_CORR_ERR_STATUS_ADDRESS_HYEX       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00160) 
#define PCIE1_CORR_ERR_STATUS_ADDRESS_HYEX       (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00160) 
#define PCIE2_CORR_ERR_STATUS_ADDRESS_HYEX       (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00160) 
#define PCIE0_CORR_ERR_STATUS_ADDRESS_HYGX       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00160) 
#define PCIE2_CORR_ERR_STATUS_ADDRESS_HYGX       (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00160)

///RC_PCIE_ROOT_ERR_STATUS
#define RC_PCIE_ROOT_ERR_STATUS_OFFSET          0x180
#define PCIE0_ROOT_ERR_STATUS_ADDRESS_HYEX       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00180) 
#define PCIE1_ROOT_ERR_STATUS_ADDRESS_HYEX       (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00180) 
#define PCIE2_ROOT_ERR_STATUS_ADDRESS_HYEX       (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00180)
#define PCIE0_ROOT_ERR_STATUS_ADDRESS_HYGX       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00180) 
#define PCIE2_ROOT_ERR_STATUS_ADDRESS_HYGX       (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00180)

///RC_PCIE_LANE_ERR_STATUS
#define RC_PCIE_LANE_ERR_STATUS_OFFSET          0x278
#define PCIE0_LANE_ERR_STATUS_ADDRESS_HYEX       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00278) 
#define PCIE2_LANE_ERR_STATUS_ADDRESS_HYEX       (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00278)
#define PCIE0_LANE_ERR_STATUS_ADDRESS_HYGX       (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00278) 
#define PCIE2_LANE_ERR_STATUS_ADDRESS_HYGX       (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00278)

/// PCIE_LINK_CNTL2
// Type
#define PCIE_LINK_CNTL2_TYPE                                TYPE_SMN

#define PCIE_LINK_CNTL2_TARGET_LINK_SPEED_OFFSET            0
#define PCIE_LINK_CNTL2_TARGET_LINK_SPEED_WIDTH             4
#define PCIE_LINK_CNTL2_TARGET_LINK_SPEED_MASK              0xf
#define PCIE_LINK_CNTL2_ENTER_COMPLIANCE_OFFSET             4
#define PCIE_LINK_CNTL2_ENTER_COMPLIANCE_WIDTH              1
#define PCIE_LINK_CNTL2_ENTER_COMPLIANCE_MASK               0x10
#define PCIE_LINK_CNTL2_HW_AUTONOMOUS_SPEED_DISABLE_OFFSET  5
#define PCIE_LINK_CNTL2_HW_AUTONOMOUS_SPEED_DISABLE_WIDTH   1
#define PCIE_LINK_CNTL2_HW_AUTONOMOUS_SPEED_DISABLE_MASK    0x20
#define PCIE_LINK_CNTL2_SELECTABLE_DEEMPHASIS_OFFSET        6
#define PCIE_LINK_CNTL2_SELECTABLE_DEEMPHASIS_WIDTH         1
#define PCIE_LINK_CNTL2_SELECTABLE_DEEMPHASIS_MASK          0x40
#define PCIE_LINK_CNTL2_XMIT_MARGIN_OFFSET                  7
#define PCIE_LINK_CNTL2_XMIT_MARGIN_WIDTH                   3
#define PCIE_LINK_CNTL2_XMIT_MARGIN_MASK                    0x380
#define PCIE_LINK_CNTL2_ENTER_MOD_COMPLIANCE_OFFSET         10
#define PCIE_LINK_CNTL2_ENTER_MOD_COMPLIANCE_WIDTH          1
#define PCIE_LINK_CNTL2_ENTER_MOD_COMPLIANCE_MASK           0x400
#define PCIE_LINK_CNTL2_COMPLIANCE_SOS_OFFSET               11
#define PCIE_LINK_CNTL2_COMPLIANCE_SOS_WIDTH                1
#define PCIE_LINK_CNTL2_COMPLIANCE_SOS_MASK                 0x800
#define PCIE_LINK_CNTL2_COMPLIANCE_DEEMPHASIS_OFFSET        12
#define PCIE_LINK_CNTL2_COMPLIANCE_DEEMPHASIS_WIDTH         4
#define PCIE_LINK_CNTL2_COMPLIANCE_DEEMPHASIS_MASK          0xf000

// Address
#define RC_PCIE_LINK_CNTL2_PCI_OFFSET                       0x88 

#define PCIE0_LINK_CNTL2_ADDRESS_HYEX                        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00088)
#define PCIE1_LINK_CNTL2_ADDRESS_HYEX                        (UINT32)((NBIO0_PCIE1_APERTURE_ID_HYEX << 20) + 0x00088)
#define PCIE2_LINK_CNTL2_ADDRESS_HYEX                        (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYEX << 20) + 0x00088)

#define PCIE0_LINK_CNTL2_ADDRESS_HYGX                        (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00088)
#define PCIE2_LINK_CNTL2_ADDRESS_HYGX                        (UINT32)((NBIO0_PCIE2_APERTURE_ID_HYGX << 20) + 0x00088)

typedef union {
  struct {
    UINT32    TARGET_LINK_SPEED           : 4;                         ///<
    UINT32    ENTER_COMPLIANCE            : 1;                         ///<
    UINT32    HW_AUTONOMOUS_SPEED_DISABLE : 1;                         ///<
    UINT32    SELECTABLE_DEEMPHASIS       : 1;                         ///<
    UINT32    XMIT_MARGIN                 : 3;                         ///<
    UINT32    ENTER_MOD_COMPLIANCE        : 1;                         ///<
    UINT32    COMPLIANCE_SOS              : 1;                         ///<
    UINT32    COMPLIANCE_DEEMPHASIS       : 4;                         ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} PCIE_LINK_CNTL2_STRUCT;

#define LANE_ANA_RX_MISC_OVRD_4                              0x11DD
#define LANE_ANA_RX_MISC_OVRD_7                              0x11EC

#define LANE_ANA_RX_AFE_OVRD_2                               0x11DE
#define LANE_ANA_RX_AFE_OVRD_3                               0x11DF
#define LANE_ANA_RX_AFE_OVRD_4                               0x11EC
#define LANE_ANA_RX_AFE_OVRD_5                               0x11ED

#define LANE_ANA_RX_CAL_MUXA                                 0x11D2

#define PCIE_RC_REGS_SMN_BASE_HYEX                           (UINT32)(NBIO0_PCIE0_APERTURE_ID_HYEX << 20)
#define PCIE_PORT_REGS_SMN_BASE_HYEX                         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x40000)
#define SMN_PCIE0_SUB_BUS_NUMBER_LATENCY_HYEX                (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYEX << 20) + 0x00018)

#define PCIE_RC_REGS_SMN_BASE_HYGX                           (UINT32)(NBIO0_PCIE0_APERTURE_ID_HYGX << 20)
#define PCIE_PORT_REGS_SMN_BASE_HYGX                         (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x40000)
#define SMN_PCIE0_SUB_BUS_NUMBER_LATENCY_HYGX                (UINT32)((NBIO0_PCIE0_APERTURE_ID_HYGX << 20) + 0x00018)

// HyGx NTB Registers
//Address
#define NBIF0RCC_DEV0_EPF1_STRAP6_ADDRESS_HYGX               (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34218)
#define NBIF1RCC_DEV0_EPF1_STRAP6_ADDRESS_HYGX               (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34218)
#define NBIF2RCC_DEV0_EPF1_STRAP6_ADDRESS_HYGX               (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34218)

#define NBIF0RCC_DEV0_EPF1_STRAP2_ADDRESS_HYGX               (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34208)
#define NBIF1RCC_DEV0_EPF1_STRAP2_ADDRESS_HYGX               (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34208)
#define NBIF2RCC_DEV0_EPF1_STRAP2_ADDRESS_HYGX               (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34208)

#define NBIF0RCC_DEV0_EPF1_STRAP10_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34228)
#define NBIF1RCC_DEV0_EPF1_STRAP10_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34228)
#define NBIF2RCC_DEV0_EPF1_STRAP10_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34228)

#define NBIF0RCC_DEV0_EPF1_STRAP11_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x3422C)
#define NBIF1RCC_DEV0_EPF1_STRAP11_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x3422C)
#define NBIF2RCC_DEV0_EPF1_STRAP11_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x3422C)

#define NBIF0RCC_DEV0_EPF1_STRAP12_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x34230)
#define NBIF1RCC_DEV0_EPF1_STRAP12_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x34230)
#define NBIF2RCC_DEV0_EPF1_STRAP12_ADDRESS_HYGX              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x34230)

#define NBIF0D00F01PCIE_BAR1_CNTL_ADDRSS_HYGX                (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x41208)
#define NBIF1D00F01PCIE_BAR1_CNTL_ADDRSS_HYGX                (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x41208)
#define NBIF2D00F01PCIE_BAR1_CNTL_ADDRSS_HYGX                (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x41208)
#define NBIF0D00F01PCIE_BAR2_CNTL_ADDRSS_HYGX                (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x41210)
#define NBIF1D00F01PCIE_BAR2_CNTL_ADDRSS_HYGX                (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x41210)
#define NBIF2D00F01PCIE_BAR2_CNTL_ADDRSS_HYGX                (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x41210)
#define NBIF0D00F01PCIE_BAR3_CNTL_ADDRSS_HYGX                (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYGX << 20) + 0x41218)
#define NBIF1D00F01PCIE_BAR3_CNTL_ADDRSS_HYGX                (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYGX << 20) + 0x41218)
#define NBIF2D00F01PCIE_BAR3_CNTL_ADDRSS_HYGX                (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYGX << 20) + 0x41218)

#define STRAP_RESIZE_BAR_EN_DEV0_F1_HYGX_OFFSET             9
#define STRAP_RESIZE_BAR_EN_DEV0_F1_HYGX_WIDTH              1
#define STRAP_RESIZE_BAR_EN_DEV0_F1_HYGX_MASK               0x200

// HyEx NTB Registers
// Address
#define NBIF0RCC_DEV0_EPF1_STRAP2_ADDRESS_HYEX               (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34208)
#define NBIF1RCC_DEV0_EPF1_STRAP2_ADDRESS_HYEX               (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34208)
#define NBIF2RCC_DEV0_EPF1_STRAP2_ADDRESS_HYEX               (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34208)

#define STRAP_RESIZE_BAR_EN_DEV0_F1_OFFSET                  8
#define STRAP_RESIZE_BAR_EN_DEV0_F1_WIDTH                   1
#define STRAP_RESIZE_BAR_EN_DEV0_F1_MASK                    0x100

// Address
#define NBIF0RCC_DEV0_EPF1_STRAP6_ADDRESS_HYEX               (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34218)
#define NBIF1RCC_DEV0_EPF1_STRAP6_ADDRESS_HYEX               (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34218)
#define NBIF2RCC_DEV0_EPF1_STRAP6_ADDRESS_HYEX               (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34218)

#define STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_OFFSET          17
#define STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_WIDTH           1
#define STRAP_APER2_PREFETCHABLE_EN_DEV0_F1_MASK            0x20000
#define STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_OFFSET          25
#define STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_WIDTH           1
#define STRAP_APER3_PREFETCHABLE_EN_DEV0_F1_MASK            0x2000000

// Address
#define NBIF0RCC_DEV0_EPF1_STRAP10_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34228)
#define NBIF1RCC_DEV0_EPF1_STRAP10_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34228)
#define NBIF2RCC_DEV0_EPF1_STRAP10_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34228)

#define STRAP_APER1_RESIZE_EN_DEV0_F1_OFFSET                0
#define STRAP_APER1_RESIZE_EN_DEV0_F1_WIDTH                 1
#define STRAP_APER1_RESIZE_EN_DEV0_F1_MASK                  0x1
#define STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_OFFSET           1
#define STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_WIDTH            20
#define STRAP_APER1_RESIZE_SUPPORT_DEV0_F1_MASK             0x1FFFFFFE

// Address
#define NBIF0RCC_DEV0_EPF1_STRAP11_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x3422C)
#define NBIF1RCC_DEV0_EPF1_STRAP11_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x3422C)
#define NBIF2RCC_DEV0_EPF1_STRAP11_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x3422C)

#define STRAP_APER2_RESIZE_EN_DEV0_F1_OFFSET                0
#define STRAP_APER2_RESIZE_EN_DEV0_F1_WIDTH                 1
#define STRAP_APER2_RESIZE_EN_DEV0_F1_MASK                  0x1
#define STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_OFFSET           1
#define STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_WIDTH            20
#define STRAP_APER2_RESIZE_SUPPORT_DEV0_F1_MASK             0x1FFFFFFE

// Address
#define NBIF0RCC_DEV0_EPF1_STRAP12_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x34230)
#define NBIF1RCC_DEV0_EPF1_STRAP12_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF1_APERTURE_ID_HYEX << 20) + 0x34230)
#define NBIF2RCC_DEV0_EPF1_STRAP12_ADDRESS_HYEX              (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x34230)

#define STRAP_APER3_RESIZE_EN_DEV0_F1_OFFSET                0
#define STRAP_APER3_RESIZE_EN_DEV0_F1_WIDTH                 1
#define STRAP_APER3_RESIZE_EN_DEV0_F1_MASK                  0x1
#define STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_OFFSET           1
#define STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_WIDTH            20
#define STRAP_APER3_RESIZE_SUPPORT_DEV0_F1_MASK             0x1FFFFFFE

//Address
#define NBIF0D00F01PCIE_BAR1_CNTL_ADDRSS_HYEX                (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x41208)
#define NBIF2D00F01PCIE_BAR1_CNTL_ADDRSS_HYEX                (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x41208)
#define NBIF0D00F01PCIE_BAR2_CNTL_ADDRSS_HYEX                (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x41210)
#define NBIF2D00F01PCIE_BAR2_CNTL_ADDRSS_HYEX                (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x41210)
#define NBIF0D00F01PCIE_BAR3_CNTL_ADDRSS_HYEX                (UINT32)((NBIO0_NBIF0_APERTURE_ID_HYEX << 20) + 0x41218)
#define NBIF2D00F01PCIE_BAR3_CNTL_ADDRSS_HYEX                (UINT32)((NBIO0_NBIF2_APERTURE_ID_HYEX << 20) + 0x41218)

#define BAR_SIZE_OFFSET                                     8
#define BAR_SIZE_WIDTH                                      5
#define BAR_SIZE_MASK                                       0x1F00

//Address
#define NTB_CTRL_ADDRESS_HYEX                                0x4000200
#define PMM_REG_CTRL_OFFSET                                 21
#define PMM_REG_CTRL_WIDTH                                  1
#define PMM_REG_CTRL_MASK                                   0x200000
#define SMM_REG_CTRL_OFFSET                                 20
#define SMM_REG_CTRL_WIDTH                                  1
#define SMM_REG_CTRL_MASK                                   0x100000

//Address
#define NTB_PBAR1LMT_ADDRESS_HYEX                            0x4000414
#define NTB_PBAR1LMT_ADDRESS_HYGX                            0x6000414


#define NTB_PBAR1LMT_OFFSET                                 12
#define NTB_PBAR1LMT_WIDTH                                  20
#define NTB_PBAR1LMT_MASK                                   0xFFFFF000

//Address
#define NTB_PBAR23LMT_LO_ADDRESS_HYEX                        0x4000418
#define NTB_PBAR23LMT_LO_ADDRESS_HYGX                        0x6000418

#define NTB_PBAR23LMT_LO_OFFSET                             12
#define NTB_PBAR23LMT_LO_WIDTH                              20
#define NTB_PBAR23LMT_LO_MASK                               0xFFFFF000

//Address
#define NTB_PBAR23LMT_HI_ADDRESS_HYEX                        0x400041C
#define NTB_PBAR23LMT_HI_ADDRESS_HYGX                        0x600041C

#define NTB_PBAR23LMT_HI_OFFSET                             0
#define NTB_PBAR23LMT_HI_WIDTH                              32
#define NTB_PBAR23LMT_HI_MASK                               0xFFFFFFFF

//Address
#define NTB_PBAR45LMT_LO_ADDRESS_HYEX                        0x4000420
#define NTB_PBAR45LMT_LO_ADDRESS_HYGX                        0x6000420

#define NTB_PBAR45LMT_LO_OFFSET                             12
#define NTB_PBAR45LMT_LO_WIDTH                              20
#define NTB_PBAR45LMT_LO_MASK                               0xFFFFF000

//Address
#define NTB_PBAR45LMT_HI_ADDRESS_HYEX                        0x4000424
#define NTB_PBAR45LMT_HI_ADDRESS_HYGX                        0x6000424

#define NTB_PBAR45LMT_HI_OFFSET                             0
#define NTB_PBAR45LMT_HI_WIDTH                              32
#define NTB_PBAR45LMT_HI_MASK                               0xFFFFFFFF

//Address
#define NTB_SBAR1LMT_ADDRESS_HYEX                            0x4000814
#define NTB_SBAR1LMT_ADDRESS_HYGX                            0x6000814

#define NTB_SBAR1LMT_OFFSET                                 12
#define NTB_SBAR1LMT_WIDTH                                  20
#define NTB_SBAR1LMT_MASK                                   0xFFFFF000

//Address
#define NTB_SBAR23LMT_LO_ADDRESS_HYEX                        0x4000818
#define NTB_SBAR23LMT_LO_ADDRESS_HYGX                        0x6000818

#define NTB_SBAR23LMT_LO_OFFSET                             12
#define NTB_SBAR23LMT_LO_WIDTH                              20
#define NTB_SBAR23LMT_LO_MASK                               0xFFFFF000

//Address
#define NTB_SBAR23LMT_HI_ADDRESS_HYEX                        0x400081C
#define NTB_SBAR23LMT_HI_ADDRESS_HYGX                        0x600081C

#define NTB_SBAR23LMT_HI_OFFSET                             0
#define NTB_SBAR23LMT_HI_WIDTH                              32
#define NTB_SBAR23LMT_HI_MASK                               0xFFFFFFFF

//Address
#define NTB_SBAR45LMT_LO_ADDRESS_HYEX                        0x4000820
#define NTB_SBAR45LMT_LO_ADDRESS_HYGX                        0x6000820

#define NTB_SBAR45LMT_LO_OFFSET                             12
#define NTB_SBAR45LMT_LO_WIDTH                              20
#define NTB_SBAR45LMT_LO_MASK                               0xFFFFF000

//Address
#define NTB_SBAR45LMT_HI_ADDRESS_HYEX                        0x4000824
#define NTB_SBAR45LMT_HI_ADDRESS_HYGX                        0x6000824

#define NTB_SBAR45LMT_HI_OFFSET                             0
#define NTB_SBAR45LMT_HI_WIDTH                              32
#define NTB_SBAR45LMT_HI_MASK                               0xFFFFFFFF

#define FST_LINK_CONTROL_HYGX                                0xC000030

#endif
