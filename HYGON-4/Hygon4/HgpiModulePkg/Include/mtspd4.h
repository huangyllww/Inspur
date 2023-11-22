/* $NoKeywords:$ */

/**
 * @file
 *
 * mtspd4.h
 *
 * Technology SPD support for DDR4
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI
 * @e sub-project: (Mem/Tech/DDR4)
 *
 **/
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
  * ***************************************************************************
  *
 */

#ifndef _MTSPD4_H_
#define _MTSPD4_H_

/*----------------------------------------------------------------------------
 *   Mixed (DEFINITIONS AND MACROS / TYPEDEFS, STRUCTURES, ENUMS)
 *
 *----------------------------------------------------------------------------
 */

/*-----------------------------------------------------------------------------
 *                         DEFINITIONS AND MACROS
 *
 *-----------------------------------------------------------------------------
 */

/*===============================================================================
 *   JEDEC DDR 4
 *===============================================================================
 */

// -----------------------------------------------------------------------------------------------------
//
// Address map
//
//
// Block  Range    Description
// 0   0~127     Base Configuration and DRAM Parameters
// 1   128~191   Module Specific Parameters -- See annexes for details
// 192~255   Hybrid Memory Parameters -- See annexes for details
// 2   256~319   Reserved; must be coded as 0x00
// 320~383   Manufacturing Information
// 3   384~511   End User Programmable
//
// -----------------------------------------------------------------------------------------------------
//
//
/// DDR4 SPD BLOCK
typedef struct _DDR4_SPD_BLOCK {
  UINT8     Array[126]; ///< First 127 Bytes of an SPD Block
  UINT16    CRC;        ///< 16 Bit CRC for this block
} DDR4_SPD_BLOCK;

#define TCCD_S  4 ///< see Proposed DDR4 Full spec update(79-4A)

#define TRRD_dlr  4

#define TFAW_dlr  16
// -----------------------------------------------------------------------------------------------------
//
// Block 0: Base Configuration and DRAM Parameters
//
// Byte 0: Number of Serial PD Bytes Written / SPD Device Size / CRC Coverage 1, 2
#define SPD_BYTES_USED     0
#define BYTES_USED_SHIFT   0
#define BYTES_USED_MASK    0x0F
#define SPD_BYTES_TOTAL    0
#define BYTES_TOTAL_SHIFT  4
#define BYTES_TOTAL_MASK   0x07

// Byte 1: SPD Revision
#define SPD_SPD_REVISION  1

// Byte 2: Key Byte / DRAM Device Type
#define SPD_DRAM_DEVICE_TYPE  2
#define DEVICE_TYPE_DDR3      0x0B
#define DEVICE_TYPE_DDR4      0x0C

// Byte 3: Key Byte / Module Type
#define SPD_BASE_MODULE_TYPE     3
#define MODULE_TYPE_MASK         0x0F
#define MOD_TYPE_EXT             0x00
#define MOD_TYPE_RDIMM           0x01
#define MOD_TYPE_UDIMM           0x02
#define MOD_TYPE_SODIMM          0x03
#define MOD_TYPE_LRDIMM          0x04
#define MOD_TYPE_MINI_RDIMM      0x05
#define MOD_TYPE_MINI_UDIMM      0x06
#define MOD_TYPE_72b_SO_RDIMM    0x08
#define MOD_TYPE_72b_SO_UDIMM    0x09
#define MOD_TYPE_16b_SODIMM      0x0C
#define MOD_TYPE_32b_SODIMM      0x0D
#define MOD_TYPE_NO_BASE_MEMORY  0x0F
#define MOD_TYPE_RDIMM           0x01

#define MOD_TYPE_HYBRID       0x80
#define MOD_TYPE_HYBRID_TYPE  0x70
#define SPD_HYBRID_MEDIA      3
#define HYBRID_MEDIA_SHIFT    4
#define HYBRID_MEDIA_MASK     7
#define NVDIMM_HYBRID         1
#define SPD_HYBRID            3
#define HYBRID_SHIFT          7
#define HYBRID_MASK           1
#define HYBRID_TYPE           1

// Byte 4: SDRAM Density and Banks
#define SPD_CAPACITY           4
#define CAPACITY_MASK          0xF
#define SPD_BANK_ADDR_BITS     4
#define SRAM_CAPACITY_8G_BITS  5
#define SRAM_CAPACITY_4G_BITS  4
#define BANK_ADDR_BITS_SHIFT   4
#define BANK_ADDR_BITS_MASK    3
// #define GET_BANK_ADDR_BITS(byte) (((byte) >> 4) & 3)
#define SPD_BANK_GROUP_BITS    4
#define BANK_GROUP_BITS_SHIFT  6
#define BANK_GROUP_BITS_MASK   3

// Byte 5: SDRAM Addressing
#define SPD_ROWS_COLS        5           // [5:0] Rows and Columns
#define ROW_COL_MASK         0x3F        //
#define SPD_COL_ADDR_BITS    5           // [2:0] Column Address Bits
#define COL_ADDR_BITS_MASK   7
#define SPD_ROW_ADDR_BITS    5           // [5:3] Row Address Bits
#define ROW_ADDR_BITS_SHIFT  3
#define ROW_ADDR_BITS_MASK   7

// Byte 6: SDRAM Package Type
#define SPD_SIGNAL_LOADING      6
#define SPD_DIE_COUNT           6
#define SPD_DIE_COUNT_SHIFT     4
#define SPD_DIE_COUNT_MASK      0x7
#define SPD_PACKAGE_TYPE        6
#define SIGNAL_LOADING_MASK     3
#define MULTI_LOAD_STACK_TYPE   1
#define SINGLE_LOAD_STACK_TYPE  2

// Byte 7: SDRAM Optional Features
#define SPD_MAC        7
#define MAC_MASK       0xF
#define SPD_TMAW       7
#define MAW_SHIFT      4
#define MAW_MASK       3
#define Untested_MAC   0
#define Unlimited_MAC  8

// Byte 8: SDRAM Thermal and Refresh Options

// Byte 9: Other SDRAM Optional Features
#define SPD_PPR         9
#define DDR4_PPR_MSK    3
#define DDR4_PPR_SHIFT  6

// Byte 10: Reserved

// Byte 11: Module Nominal Voltage, VDD
#define SPD_DRAM_VDD  11                  // [0] 1.2V Operable
                                          // [1] 1.2V Endurant
                                          // [2] TBD1 Operable
                                          // [3] TBD1 Endurant
                                          // [4] TBD2 Operable
                                          // [5] TBD2 Endurant

// Byte 12: Module Organization
#define SPD_DEVICE_WIDTH   12             //
#define DEVICE_WIDTH_MASK  7              //
#define SPD_RANKS          12             //
#define RANKS_SHIFT        3              //
#define RANKS_MASK         7              //

// Byte 13: Module Memory Bus Width
#define SPD_BUS_WIDTH           13        //
#define BUSWIDTH_MASK           7         // [2:0] Primary Bus width
#define SPD_BUSWIDTH_EXT        13        //
#define SPD_BUSWIDTH_EXT_SHIFT  3
#define SPD_BUSWIDTH_EXT_MASK   3
#define BUSWIDTH_EXT_ECC        (1 << 3)

// Byte 14: Module Thermal Sensor
#define SPD_THERMAL_SENSOR    14
#define THERMAL_SENSOR_SHIFT  7           // [7] 1 = Thermal Sensor Present
#define THERMAL_SENSOR_MASK   1           //

// Byte 15: Extended module type
#define SPD_BASE_MODULETYPE_EXT  15       // Reserved, must be coded as 0000

// Byte 16: Reserved -- must be coded as 0x00

// Byte 17: Timebases
#define SPD_FINE_TIMEBASE    17           // [1:0] 0 = 1pS, all others reserved
#define FTB_MSK              3
#define SPD_MEDIUM_TIMEBASE  17           // [3:2] 0 = 125pS, all others reserved
#define MTB_SHIFT            2
#define MTB_MSK              3

// Byte 18: SDRAM Minimum Cycle Time (tCKAVGmin)
#define SPD_TCK  18                       //

// Byte 19: SDRAM Maximum Cycle Time (tCKAVGmax)
#define SPD_TCK_MAX  19                   //

// Byte 20: CAS Latencies Supported, First Byte
// Byte 21: CAS Latencies Supported, Second Byte
// Byte 22: CAS Latencies Supported, Third Byte
// Byte 23: CAS Latencies Supported, Fourth Byte
#define SPD_CAS_BYTE_0       20           //
#define SPD_CAS_BYTE_1       21           //
#define SPD_CAS_BYTE_2       22           //
#define SPD_CAS_BYTE_3       23           //
#define CAS_LAT_RANGE_SHIFT  7            // Bit 7 Determines High or Low Range
#define CAS_LAT_RANGE_MSK    1            // 0 = Low Range (CL7 - CL36)
                                          // 1 = High Range (CL23 - CL52)

// Byte 24: Minimum CAS Latency Time (tAAmin)
#define SPD_TAA  24                       //

// Byte 25: Minimum RAS to CAS Delay Time (tRCDmin)
#define SPD_TRCD  25                      //

// Byte 26: Minimum Row Precharge Delay Time (tRPmin)
#define SPD_TRP  26                       //

// Byte 27: Upper Nibbles for tRASmin and tRCmin
#define SPD_TRAS_UPPERNIBBLE  27          //
#define SPD_TRC_UPPERNIBBLE   27          //

// Byte 28: Minimum Active to Precharge Delay Time (tRASmin), Least Significant Byte
#define SPD_TRAS_LSB  28                  //

// Byte 29: Minimum Active to Active/Refresh Delay Time (tRCmin), Least Significant Byte
#define SPD_TRC_LSB  29                   //

// Byte 30: Minimum Refresh Recovery Delay Time (tRFC1min), LSB
// Byte 31: Minimum Refresh Recovery Delay Time (tRFC1min), MSB
#define SPD_TRFC1_LSB  30                 //
#define SPD_TRFC1_MSB  31                 //

// Byte 32: Minimum Refresh Recovery Delay Time (tRFC2min), LSB
// Byte 33: Minimum Refresh Recovery Delay Time (tRFC2min), MSB
#define SPD_TRFC2_LSB  32                 //
#define SPD_TRFC2_MSB  33                 //

// Byte 34: Minimum Refresh Recovery Delay Time (tRFC4min), LSB
// Byte 35: Minimum Refresh Recovery Delay Time (tRFC4min), MSB
#define SPD_TRFC4_LSB  34                 //
#define SPD_TRFC4_MSB  35                 //

// Byte 36: Minimum Four Activate Window Time (tFAWmin), Most Significant Nibble
#define SPD_TFAW_UPPERNIBBLE  36          //

// Byte 37: Minimum Four Activate Window Time (tFAWmin), Least Significant Byte
#define SPD_TFAW_LSB  37                  //

// Byte 38: Minimum Activate to Activate Delay Time (tRRD_Smin), different bank group
#define SPD_TRRD_S  38                    //

// Byte 39: Minimum Activate to Activate Delay Time (tRRD_Lmin), same bank group
#define SPD_TRRD_L  39                    //

// Byte 40: Minimum CAS to CAS Delay Time (tCCD_Lmin), same bank group
#define SPD_TCCD_L  40                    //

// Byte 41: Minimum Write Recovery Time (tWRmin)
#define SPD_TWR_UPPERNIBBLE  41           // tWR = BYTE 41[3:0] + BYTE 42[7:0]

// Byte 42: Minimum Write Recovery Time (tWRmin)
#define SPD_TWR_LSB  42                   //

// Byte 43: Minimum Write to Read Time (tWTR_Smin), different bank group
#define SPD_TWTR_S_UPPERNIBBLE  43        // tWTR_S = Byte 43[3:0] + Byte 44[7:0]

// Byte 44: Minimum Write to Read Time (tWTR_Smin), same bank group
#define SPD_TWTR_L_UPPERNIBBLE  43        // tWTR_L = Byte 43[7:4] + Byte 45[7:0]

// Byte 44: Minimum Write to Read Time (tWTR_Smin), different bank group
#define SPD_TWTR_S_LSB  44                // tWTR_S

// Byte 45: Minimum Write to Read Time (tWTR_Lmin), same bank group
#define SPD_TWTR_L_LSB  45                // tWTR_L

// Bytes 44~59: Reserved -- must be coded as 0x00

// Bytes 60~77: Connector to SDRAM Bit Mapping
#define SPD_DQ_MAPPING  60                //

// Bytes 78~116: Reserved -- must be coded as 0x00

// Byte 117: Fine Offset for Minimum CAS to CAS Delay Time (tCCD_Lmin), same bank group
#define SPD_TCCD_L_FTB  117                //

// Byte 118: Fine Offset for Minimum Activate to Activate Delay Time (tRRD_Lmin), same bank group
#define SPD_TRRD_L_FTB  118                //

// Byte 119: Fine Offset for Minimum Activate to Activate Delay Time (tRRD_Smin), different bank group
#define SPD_TRRD_S_FTB  119                //

// Byte 120: Fine Offset for Minimum Activate to Activate/Refresh Delay Time (tRCmin)
#define SPD_TRC_FTB  120                   //

// Byte 121: Fine Offset for Minimum Row Precharge Delay Time (tRPmin)
#define SPD_TRP_FTB  121                   //

// Byte 122: Fine Offset for Minimum RAS to CAS Delay Time (tRCDmin)
#define SPD_TRCD_FTB  122                  //

// Byte 123: Fine Offset for Minimum CAS Latency Time (tAAmin)
#define SPD_TAA_FTB  123                   //

// Byte 124: Fine Offset for SDRAM Maximum Cycle Time (tCKAVGmax)
#define SPD_TCK_MAX_FTB  124               //

// Byte 125: Fine Offset for SDRAM Minimum Cycle Time (tCKAVGmin)
#define SPD_TCK_FTB  125                   //

// Byte 126: CRC for Base Configuration Section, Least Significant Byte
#define SPD_CRC_LSB  126                   //

// Byte 127: CRC for Base Configuration Section, Most Significant Byte
#define SPD_CRC_MSB  127                   //
//
// -----------------------------------------------------------------------------------------------------
// Module-Specific Section: Bytes 128~191 (0x080~0x0BF)
//
// This section contains SPD bytes which are specific to families DDR4 module families. Module
// Type Key Byte 3 is used as an index for the encoding of bytes 128~191. The content of bytes
// 128~191 are described in multiple annexes, one for each memory module family.
//
// -----------------------------------------------------------------------------------------------------
//
// DDR4 UDIMM Specific SPD Bytes
//
// Module Specific Bytes for Unbuffered Memory Module Types (Bytes 128~191, 0x080~0x0BF)
// This section defines the encoding of SPD bytes 128~191 when Memory Technology Key
// Byte 2 contains the value 0x0B and Module Type Key Byte 3 contains any of the following:
//
// - 0xH2, UDIMM
// - 0xH3, SO-DIMM
// - where H refers to the hybrid memory architecture, if any present on the module
//
// Byte 128: Raw Card Extension, Module Nominal Height
#define SPD_MODULE_HEIGHT     128          // [4:0] Module Nominal Height
#define SPD_RAW_CARD_REV_EXT  128          // [7:5] Raw Card Revision Extension
#define RAWCARDREVEXT_SHIFT   5            //
#define RAWCARDREVEXT_MASK    7            //

// Byte 129: Module Maximum Thickness
#define SPD_MAX_THICKNESS_FRONT  129
#define SPD_MAX_THICKNESS_BACK   129

// Byte 130: Reference Raw Card Used
#define SPD_RAWCARD        130             // [4:0] Reference Raw Card
#define RAWCARD_MASK       0x1F
#define RAWCARD_NONE       0x1F            // No Rawcard Reference used
#define SPD_RAWCARD_REV    130             // [6:5] Raw Card Revision
#define RAWCARDREV_SHIFT   5
#define RAWCARDREV_MASK    3
#define SPD_RAWCARD_EXT    130             // Raw Card Extension
#define RAWCARD_EXT_SHIFT  7               // 0 = R/C A through AL,
#define RAWCARD_EXT_MASK   1               // 1 = R/C AM through CB

// Byte 131: Address Mapping from Edge Connector to DRAM
#define SPD_ADDRMAP   131
#define ADDRMAP_MASK  1                    // [0] Rank 1 Mapping
                                           // 1 = Mirrored

// Bytes 132~191: Reserved -- Must be coded as 0x00

// -----------------------------------------------------------------------------------------------------
//
// DDR4 RDIMM-Specific SPD Bytes
//
// Module Specific Bytes for Registered Memory Module Types (Bytes 128~191, 0x080~0x0BF)
// This section defines the encoding of SPD bytes 128~191 when Memory Technology Key
// Byte 2 contains the value 0x0B and Module Type Key Byte 3 contains any of the following:
//
// - 0xH1, RDIMM
// - where H refers to the hybrid memory architecture, if any present on the module// Byte 128: Raw Card Extension, Module Nominal Height
//
// Byte 129: Module Maximum Thickness

// Byte 130: Reference Raw Card Used

// Byte 131: DIMM Module Attributes
#define SPD_NUM_REGISTERS    131           // Number of Registers
#define NUM_REGISTERS_MASK   3             // [1:0] # of Registers on RDIMM
#define NUM_REGISTERS_UNDEF  0             // Undefined number of registers
                                           // 1 = 1 Register
                                           // 2 = 2 Registers
                                           // 3 = 4 Registers
#define SPD_NUM_ROWS_DRAMS  131            // [3:2] # Number of Rows of DRAMs
#define NUMROWSDRAMS_MASK   3              // 0 = Undefined
#define NUMROWSDRAMS_SHIFT  2              // 1 = 1 Row of DRAMs
                                           // 2 = 2 Rows of DRAMs
                                           // 3 = 4 Rows of DRAMs
// Byte 132: RDIMM Thermal Heat Spreader Solution
#define SPD_HEAT_SPREADER   132            // Heat Spreader Solution
#define HEATSPREADER_MASK   1
#define HEATSPREADER_SHIFT  7

// Byte 133: Register Manufacturer ID Code, Least Significant Byte
#define SPD_REG_MFG_ID_LSB  133

// Byte 134: Register Manufacturer ID Code, Most Significant Byte
#define SPD_REG_MFG_ID_MSB  134

// Byte 135: Register Revision Number
#define SPD_REG_REVISION  135
// Byte 136: Address Mapping from Register to DRAM
#define SPD_REG_ADDRMAP  136
#define REGADDR_MASK     1                 // [0] Rank 1 Mapping
                                           // 1 = Mirrored

// Byte 137 Register output drive strength for control
#define SPD_OUTPUT_DRIVE_CTRL_R  137

// Byte 138 Register output drive strength for CK
#define SPD_OUTPUT_DRIVE_CK_R  138

// Bytes 137~191 Reserved, must be coded as 0x00

// -----------------------------------------------------------------------------------------------------
//
// DDR4 LRDIMM-Specific SPD Bytes
//
// Module Specific Bytes for Load Reduced Memory Module Types (Bytes 128~191, 0x080~0x0BF)
// This section defines the encoding of SPD bytes 128~191 when Memory Technology Key Byte 2
// contains the value 0x0B and Module Type Key Byte 3 contains any of the following:
//
// - 0xH4, LRDIMM
// - where H refers to the hybrid memory architecture, if any present on the module
//
//
// Byte 128: Raw Card Extension, Module Nominal Height
// Byte 129: Module Maximum Thickness
// Byte 130: Reference Raw Card Used
// Byte 131: DIMM Module Attributes
// Byte 132: LRDIMM Thermal Heat Spreader Solution
// Byte 133: Register and Data Buffer Manufacturer ID Code, Least Significant Byte
// Byte 134: Register and Data Buffer Manufacturer ID Code, Most Significant Byte
// Byte 135: Register Revision Number
// Byte 136: Address Mapping from Register to DRAM
// Byte 137: Register Output Drive Strength for Control and Command/Address
// Byte 138: Register Output Drive Strength for CK
// Byte 139: Data Buffer Revision Number
// Byte 140: DRAM VrefDQ for Package Rank 0
// Byte 141: DRAM VrefDQ for Package Rank 1
// Byte 142: DRAM VrefDQ for Package Rank 2
// Byte 143: DRAM VrefDQ for Package Rank 3
// Byte 144: Data Buffer VrefDQ for DRAM Interface
#define VREFDQ_F5BC6X  144

// Byte 145: Data Buffer MDQ Drive Strength and RTT for data rate < 1866
#define MDQ_ODT_1866  145

// Byte 146: Data Buffer MDQ Drive Strength and RTT for 1866 < data rate < 2400
#define MDQ_ODT_2400  146

// Byte 147: Data Buffer MDQ Drive Strength and RTT for 2400 < data rate < 3200
#define MDQ_ODT_3200  147

// Byte 148: DRAM Drive Strength (for data rates < 1866, 1866 < data rate < 2400, and
// Byte 2400: < data rate < 3200)
// Byte 149: DRAM ODT (RTT_WR, RTT_NOM) for data rate < 1866
#define RTT_WR_NOM_1866  149

// Byte 150: DRAM ODT (RTT_WR, RTT_NOM) for 1866 < data rate < 2400
#define RTT_WR_NOM_2400  150

// Byte 151: DRAM ODT (RTT_WR, RTT_NOM) for 2400 < data rate < 3200
#define RTT_WR_NOM_3200  151

// Byte 152: DRAM ODT (RTT_PARK) for data rate < 1866
#define RTT_PARK_1866  152

// Byte 153: DRAM ODT (RTT_PARK) for 1866 < data rate < 2400
#define RTT_PARK_2400  153

// Byte 154: DRAM ODT (RTT_PARK) for 2400 < data rate < 3200
#define RTT_PARK_3200  154

// Byte 155~191:  Reserved -- must be coded as 0x00
//
//
//
// -----------------------------------------------------------------------------------------------------
// Hybrid Memory Architecture Specific Parameters: Bytes 192~255 (0x0C0~0x0FF)
// This section contains SPD bytes which are specific to hybrid memory module families. Module
// Type Key Byte 3 bits 7~4 are used as an index for the encoding of bytes 192~255. The content of
// bytes 192~255 are described in multiple annexes, one for each hybrid module family.
//
// -----------------------------------------------------------------------------------------------------
//
// DDR4 NVDIMM-Specific SPD Bytes
//
// Non-Volatile (NVDIMM-N) Hybrid Memory Parameters: Bytes 192~253 (0x0C0~0x1FD)
// This section defines the encoding of SPD bytes 192~255 when Module Type Key Byte 3 contains
// any of the following:
//
// - 0x9M, NVDIMM-N
// - where M refers to the base memory architecture
//
//
// Byte 192: (NVDIMM): Module Product Identifier, LSB
#define SPD_NVDIMM_MODULE_PRODUCT_ID_LSB  192

// Byte 193: (NVDIMM): Module Product Identifier, MSB
#define SPD_NVDIMM_MODULE_PRODUCT_ID_MSB  193

// Byte 194: (NVDIMM): Subsystem Controller Manufacturer Identifier, LSB
#define SPD_NVDIMM_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID_LSB  194

// Byte 195: (NVDIMM): Subsystem Controller Manufacturer Identifier, MSB
#define SPD_NVDIMM_SUBSYSTEM_CONTROLLER_MANUFACTURER_ID_MSB  195

// Byte 196: (NVDIMM): Subsystem Controller Identifier, LSB
#define SPD_NVDIMM_SUBSYSTEM_CONTROLLER_ID_LSB  196

// Byte 197: (NVDIMM): Subsystem Controller Identifier, MSB
#define SPD_NVDIMM_SUBSYSTEM_CONTROLLER_ID_MSB  197

// Byte 198: (NVDIMM): Subsystem Controller Revision Code
#define SPD_NVDIMM_SUBSYSTEM_CONTROLLER_REVISION_CODE  198

// Byte 201: (NVDIMM): Hybrid Module Media Types
#define SPD_NVDIMM_HYBRID_MODULE_MEDIA_TYPES_LSB  201
#define UNKNOWN_UNDEFINED_SHIFT                   0
#define UNKNOWN_UNDEFINED_MSK                     0x1
#define SDRAM_SHIFT                               1
#define SDRAM_MSK                                 0x2
#define NAND_SHIFT                                2
#define NAND_MSK                                  0x4

// Byte 204: (NVDIMM): Function 0 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_0_INTERFACE_DESCRIPTORS_LSB  204
#define FUNCTION_CLASS_LOW_SHIFT                         5
#define FUNCTION_CLASS_LOW_MSK                           0xe0

// Byte 205: (NVDIMM): Function 0 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_0_INTERFACE_DESCRIPTORS_MSB  205
#define FUNCTION_CLASS_HIGH_SHIFT                        0
#define FUNCTION_CLASS_HIGH_MSK                          3
#define FUNCTION_INTERFACE_IMPLEMENTED_SHIFT             7
#define FUNCTION_INTERFACE_IMPLEMENTED_MSK               0x80

// Byte 206: (NVDIMM): Function 1 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_1_INTERFACE_DESCRIPTORS_LSB  206

// Byte 207: (NVDIMM): Function 1 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_1_INTERFACE_DESCRIPTORS_MSB  207

// Byte 208: (NVDIMM): Function 2 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_2_INTERFACE_DESCRIPTORS_LSB  208

// Byte 209: (NVDIMM): Function 2 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_2_INTERFACE_DESCRIPTORS_MSB  209

// Byte 210: (NVDIMM): Function 3 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_3_INTERFACE_DESCRIPTORS_LSB  210

// Byte 211: (NVDIMM): Function 3 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_3_INTERFACE_DESCRIPTORS_MSB  211

// Byte 212: (NVDIMM): Function 4 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_4_INTERFACE_DESCRIPTORS_LSB  212

// Byte 213: (NVDIMM): Function 4 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_4_INTERFACE_DESCRIPTORS_MSB  213

// Byte 214: (NVDIMM): Function 5 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_5_INTERFACE_DESCRIPTORS_LSB  214

// Byte 215: (NVDIMM): Function 5 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_5_INTERFACE_DESCRIPTORS_MSB  215

// Byte 216: (NVDIMM): Function 6 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_6_INTERFACE_DESCRIPTORS_LSB  216

// Byte 217: (NVDIMM): Function 6 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_6_INTERFACE_DESCRIPTORS_MSB  217

// Byte 218: (NVDIMM): Function 7 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_7_INTERFACE_DESCRIPTORS_LSB  218

// Byte 219: (NVDIMM): Function 7 Interface Descriptors
#define SPD_NVDIMM_FUNCTION_7_INTERFACE_DESCRIPTORS_MSB  219
// <--- lqs debug add smbios 3.2 support
// Byte 254: CRC for SPD Block 1, Least Significant Byte 1
#define SPD_CRC_BLOCK1_LSB  254            //

// Byte 255: CRC for SPD Block 1, Most Significant Byte
#define SPD_CRC_BLOCK1_MSB  255            //

// -----------------------------------------------------------------------------------------------------
//
// DDR4 Module Supplier Data: Bytes 320~383 (0x140~0x17F)
//
// -----------------------------------------------------------------------------------------------------
//
// Byte 320: Module Manufacturer ID Code, Least Significant Byte
#define SPD_MODULE_MANUFACTURER_ID_CODE_LSB  320
//
// Byte 321: Module Manufacturer ID Code, Most Significant Byte
#define SPD_MODULE_MANUFACTURER_ID_CODE_MSB  321

// Byte 322: Module Manufacturing Location
#define SPD_MODULE_MANUFACTURING_LOCATION  322

// Bytes 323~324: Module Manufacturing Date
#define SPD_MODULE_MANUFACTURING_DATE  323              // Year, Week, Coded in BCD
#define SPD_MODULE_MANUFACTURING_YEAR  323              // Year, Coded in BCD
#define SPD_MODULE_MANUFACTURING_WEEK  324              // Week, Coded in BCD

// Bytes 325~328: Module Serial Number
#define SPD_MODULE_SERIAL_NUMBER      325
#define SPD_MODULE_SERIAL_NUMBER_LEN  (328 - 325 + 1)               // 4 Bytes

// Bytes 329~348: Module Part Number
#define SPD_MODULE_PART_NUMBER      329
#define SPD_MODULE_PART_NUMBER_LEN  (348 - 329 + 1)                 // 20 Bytes

// Byte 349: Module Revision Code
#define SPD_MODULE_REVISION_CODE  349

// Byte 350: DRAM Manufacturers ID Code, Least Significant Byte
#define SPD_DRAM_MANUFACTURERS_ID_CODE_LSB  350

// Byte 351: DRAM Manufacturers ID Code, Most Significant Byte
#define SPD_DRAM_MANUFACTURERS_ID_CODE_MSB  351

// Byte 352: DRAM Stepping
#define SPD_DRAM_STEPPING  352

// Bytes: 353~381: Module Manufacturer Specific
#define SPD_MODULE_MANUFACTURER_SPECIFIC      353
#define SPD_MODULE_MANUFACTURER_SPECIFIC_LEN  (381 - 353 + 1)

// Bytes: 382~383 Reserved, must be coded as 0x00
#define SPD_RESERVED_382     382
#define SPD_RESERVED_383     383
#define SPD_BYTE_OFFSET_508  508
#define SPD_BYTE_OFFSET_509  509
#define SPD_BYTE_OFFSET_510  510
#define SPD_BYTE_OFFSET_511  511

// DDR3 Definitions for Reference
//
// -----------------------------------------------------------------------------------------------------
#define OPEN_PAGE                        0
#define ENERGY_SOURCE_POLICY             0x14
#define DEVICE_MANAGED_POLICY_SUPPORTED  0x1
#define HOST_MANAGED_POLICY_SUPPORTED    0x2

#define HWREV         0x4
#define SPECREV       0x6
#define SLOT0_FWREV0  0x7
#define SLOT0_FWREV1  0x8
#define SLOT1_FWREV0  0x9
#define SLOT1_FWREV1  0xa

#define CSAVE_TRIGGER_SUPPORT  0x16
#define SAVE_N                 0x1

#define CSAVE_TIMEOUT0  0x18
#define CSAVE_TIMEOUT1  0x19

#define PAGE_SWITCH_LATENCY0  0x1a

#define PAGE_SWITCH_LATENCY1  0x1b
#define UNIT_IN_SECOND        0x80
#define LATENCY1_MASK         0x7f

#define RESTORE_TIMEOUT0  0x1c
#define RESTORE_TIMEOUT1  0x1d

#define ERASE_TIMEOUT0  0x1e
#define ERASE_TIMEOUT1  0x1f

#define ARM_TIMEOUT0       0x20
#define ARM_TIMEOUT1       0x21
#define ABORT_CMD_TIMEOUT  0x24

#define NVDIMM_MGT_CMD0                 0x40
#define RST_CTRL                        0x1
#define CL_ALL_CMD_STAT                 0x2
#define CL_SAVE_STAT                    0x4
#define CL_RESTORE_STAT                 0x8
#define CL_ERASE_STAT                   0x10
#define CL_ARM_STAT                     0x20
#define CL_SET_EVENT_NOTIFICATION_STAT  0x40
#define CL_SET_ES_STAT                  0x80
#define CL_ALL                          0xfe

#define NVDIMM_FUNC_CMD0       0x43
#define START_FACTORY_DEFAULT  0x1
#define START_RESTORE          0x4
#define START_ERASE            0x8
#define ABORT_CURRENT_OP       0x10

#define ARM_CMD     0x45
#define ARM_SAVE_N  0x1

#define SET_ES_POLICY_CMD      0x49
#define DEVICE_MANAGED_POLICY  0x1
#define HOST_MANAGED_POLICY    0x2

#define NVDIMM_READY  0x60

#define NVDIMM_CMD_STATUS0           0x61
#define CONTROLLER_BUSY              0x1
#define FACTORY_DEFAULT_IN_PROGRESS  0x2
#define SAVE_IN_PROGRESS             0x4
#define RESTORE_IN_PROGRESS          0x8
#define ERASE_IN_PROGRESS            0x10
#define ABORT_IN_PROGRESS            0x20
#define ARM_IN_PROGRESS              0x40
#define FIRMWARE_OPS_IN_PROGRESS     0x80

#define SAVE_STATUS0  0x64
#define SAVE_SUCCESS  0x1
#define SAVE_ERROR    0x2

#define RESTORE_STATUS0  0x66
#define RESTORE_SUCCESS  0x1
#define RESTORE_ERROR    0x2
#define ABORT_SUCCESS    0x10
#define ABORT_ERROR      0x20

#define ERASE_STATUS0  0x68
#define ERASE_SUCCESS  0x1
#define ERASE_ERROR    0x2

#define ARM_STATUS0   0x6a
#define ARM_SUCCESS   0x1
#define ARM_ERROR     0x2
#define SAVE_N_ARMED  0x4

#define FACTORY_DEFAULT_STATUS0  0x6C
#define FACTORY_DEFAULT_SUCCESS  0x1
#define FACTORY_DEFAULT_ERROR    0x2

#define SET_ES_POLICY_STATUS           0x70
#define SET_ES_POLICY_SUCCESS          0x1
#define SET_ES_POLICY_ERROR            0x2
#define DEVICE_MANAGED_POLICY_ENABLED  0x4

#define CSAVE_INFO0     0x80
#define NVM_DATA_VALID  0x1
#define SAVE_N_INFO0    0x4

#define CSAVE_FAIL_INFO0             0x84
#define CSAVE_FAIL_INFO1             0x85
#define MODULE_HEALTH                0xa0
#define MODULE_HEALTH_STATUS0        0xa1
#define MODULE_HEALTH_STATUS1        0xa2
#define NOT_ENOUGH_ENERGY_FOR_CSAVE  1

/*----------------------------------------------------------------------------
 *                         TYPEDEFS, STRUCTURES, ENUMS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           FUNCTIONS PROTOTYPE
 *
 *----------------------------------------------------------------------------
 */

#endif /* _MTSPD4_H_ */
