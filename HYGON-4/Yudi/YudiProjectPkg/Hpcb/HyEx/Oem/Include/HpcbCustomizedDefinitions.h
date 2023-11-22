/* $NoKeywords:$ */

/**
 * @file
 *
 * HpcbCustomizedDefinitions.h
 *
 * HGPI PSP Customization Block Data Intializer
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI
 * @e sub-project: HpcbGenerator
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
//
// OEM CUSTOMIZABLE DEFINITIONS. Any changes must be done here...
//

#ifndef _HPCB_CUSTOMIZED_DEFINITIONS_H_
#define _HPCB_CUSTOMIZED_DEFINITIONS_H_
#define MCT_BOTTOM_IO_VALUE 0x80
#define DF_PCI_MMIO_SIZE    0x10000000
#define HygonHBLDebug_SUPPORT 1
#define HygonHBLDebug_Select  1
#define BLDCFG_FAMILY_ALL   HYGON_FAMILY_17_ALL
#define BLDCFG_FAMILY_HPCB  HYGON_F17_ALL_HPCB

#define BLDCFG_EXTERNAL_PARAMS_DEF_ENABLED  TRUE
#define BLDCFG_EXTERNAL_PARAMS_ENABLED      FALSE
#define BLDCFG_INTERNAL_PARAMS_DEF_ENABLED  TRUE
#define BLDCFG_INTERNAL_PARAMS_ENABLED      FALSE

#define BLDCFG_BOTTOM_IO                              MCT_BOTTOM_IO_VALUE
#define BLDCFG_MEM_HOLE_REMAPPING                     TRUE
#define BLDCFG_MEM_LIMIT_1TB                          TRUE
#define BLDCFG_TIMING_MODE_SELECT                     TIMING_MODE_AUTO
#define BLDCFG_MEMORY_CLOCK_SELECT                    DDR3200_FREQUENCY
#define BLDCFG_MEMORY_ENABLE_CHIPSELECT_INTERLEAVING  TRUE
#define BLDCFG_MEMORY_CHANNEL_INTERLEAVING            FALSE
#define BLDCFG_ENABLE_ECC_FEATURE                     TRUE 
#define BLDCFG_MEMORY_POWER_DOWN                      FALSE
#define BLDCFG_PARITY_CONTROL                         FALSE
#define BLDCFG_BANK_SWIZZLE                           TRUE
#define BLDCFG_ENABLE_MEM_CLR                         FALSE
#define BLDCFG_UMA_MODE                               UMA_AUTO
#define BLDCFG_UMA_SIZE                               0
#define BLDCFG_MEMORY_RESTORE_CONTROL                 FALSE
#define BLDCFG_SAVE_MEMORY_CONTEXT_CONTROL            TRUE
#define BLDCFG_IS_CAPSULE_MODE                        TRUE
#define BLDCFG_FORCE_TRAIN_MODE                       FORCE_TRAIN_AUTO
#define BLDCFG_DIMM_TYPE_TECHNOLGY                    DDR5_TECHNOLOGY
#define BLDCFG_AMP_MODE_ENABLE                        TRUE
#define BLDCFG_DRAM_DOUBLE_REFRESH_RATE               FALSE
#define BLDCFG_PMU_TRAINING_MODE                      PMU_TRAIN_1D_2D
#define BLDCFG_ECC_REDIRECTION                        FALSE
#define BLDCFG_SCRUB_DRAM_RATE                        0
#define BLDCFG_SCRUB_L2_RATE                          0
#define BLDCFG_SCRUB_L3_RATE                          0
#define BLDCFG_SCRUB_IC_RATE                          0
#define BLDCFG_SCRUB_DC_RATE                          0
#define BLDCFG_ECC_SYNC_FLOOD                         0
#define BLDCFG_ECC_SYMBOL_SIZE                        1
#define BLDCFG_DQS_TRAINING_CONTROL                   TRUE
#define BLDCFG_UMA_ABOVE_4GB                          TRUE
#define BLDCFG_UMA_BUFFER_ALIGNMENT                   UMA_4MB_ALIGNED
#define BLDCFG_MEMORY_ALL_CLOCKS_ON                   TRUE
#define BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT             DDR4800_FREQUENCY
#define BLDCFG_POWER_DOWN_MODE                        POWER_DOWN_BY_CHANNEL
#define BLDCFG_IGNORE_SPD_CHECKSUM                    TRUE
#define BLDCFG_MEMORY_MODE_UNGANGED                   TRUE
#define BLDCFG_MEMORY_QUAD_RANK_CAPABLE               TRUE
#define BLDCFG_MEMORY_RDIMM_CAPABLE                   TRUE
#define BLDCFG_MEMORY_LRDIMM_CAPABLE                  TRUE
#define BLDCFG_MEMORY_UDIMM_CAPABLE                   TRUE
#define BLDCFG_MEMORY_SODIMM_CAPABLE                  TRUE
#define BLDCFG_DRAM_DOUBLE_REFRESH_RATE_ENABLE        FALSE
#define BLDCFG_MEMORY_DDR5_CAPABLE                    TRUE
#define BLDCFG_ZQ_RESET_CONTROL_ENABLE                TRUE
#define BLDCFG_BANK_SWAP_GROUP_ENABLE                 TRUE
#define BLDCFG_ODT_CMD_THROT_ENABLE                   TRUE
#define BLDCFG_SW_CMD_THROT_ENABLE                    0
#define BLDCFG_FORCE_POWER_DOWN_THROT_ENABLE          0
#define BLDCFG_ODT_CMD_THROT_CYCLE                    87
#define BLDCFG_SW_CMD_THROT_CYCLE                     0
#define BLDCFG_DIMM_SENSOR_CONFIGURATION              0x408
#define BLDCFG_DIMM_SENSOR_UPPER                      80
#define BLDCFG_DIMM_SENSOR_LOWER                      10
#define BLDCFG_DIMM_SENSOR_CRITICAL                   95
#define BLDCFG_DIMM_SENSOR_RESOLUTION                 1
#define BLDCFG_AUTO_REF_FINE_GRAN_MODE                0
#define BLDCFG_ENABLE_MEMPSTATE                       TRUE
#define BLDCFG_ENABLE_DDRROUTEBALANCEDTEE             FALSE
#define BLDCFG_MEM_NVDIMM_POWER_SOURCE                1
#define BLDCFG_MEM_DATA_POISON                        FALSE
#define BLDCFG_MEM_DATA_SCRAMBLE                      TRUE
//
// MBIST Items
// MBIST SubTestType: 0-Basic Test, 1 - Single Chipselect, 2- Multi Chipselect
// 3 - AddressLine Test, 4- All Test
//
#define BLDCFG_MEM_MBIST_TEST_ENABLE      TRUE
#define BLDCFG_MEM_MBIST_SUBTEST_TYPE     0
#define BLDCFG_MEM_MBIST_AGGRESOR_ON      FALSE
#define BLDCFG_MEM_MBIST_HALT_ON_ERROR    TRUE
#define BLDCFG_MEM_MBIST_CPU_VREF_RANGE   0
#define BLDCFG_MEM_MBIST_DRAM_VREF_RANGE  0
// #define BLDCFG_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT 0
#define BLDCFG_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT 0

#define BLDCFG_MEM_I2C_I3C_VOLTAGE_LEVEL 2    // 0:for I3C 1.1V; 1: for I2C/I3C 1.8V; 2: for I2C 3.3V

#define BLDCFG_MEM_TSME_ENABLE                     FALSE
#define BLDCFG_MEM_TEMP_CONTROLLED_REFRESH_EN      FALSE
#define BLDCFG_MEM_PS_ERROR_HANDLING               0
#define BLDCFG_MEM_CLK_MAP  { MEMCLK_DIS_MAP (ANY_SOCKET, ANY_CHANNEL, 0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00) }         // MemClkDisMap
#define BLDCFG_CKE_TRI_MAP  { CKE_TRI_MAP (ANY_SOCKET, ANY_CHANNEL, 0x00, 0x01, 0x02, 0x03) }                                    // CkeTriMap
#define BLDCFG_ODT_TRI_MAP  { ODT_TRI_MAP (ANY_SOCKET, ANY_CHANNEL, 0x00, 0x01, 0x02, 0x03) }                                    // OdtTriMap
#define BLDCFG_CS_TRI_MAP   { CS_TRI_MAP (ANY_SOCKET, ANY_CHANNEL, 0x00, 0x01, 0x02, 0x03, 0x00, 0x00, 0x00, 0x00) }             // CsTriMap
#define BLDCFG_NUM_DIMMS_SUPPORTED               { NUMBER_OF_DIMMS_SUPPORTED (ANY_SOCKET, ANY_CHANNEL, 1) }                      // NumberDimmSupported
#define BLDCFG_NUM_SOLDERED_DOWN_DIMM_SUPPORTED  { 0, 0, 0, 0, 0, 0 }                                                            // NumberSolderedDownDimmsSupported
#define BLDCFG_NUM_CS_SUPPORTED                  { 0, 0, 0, 0, 0, 0 }                                                            // NumberChipSelectsSupported

#define BLDCFG_NUM_CHNL_SUPPORTED         { NUMBER_OF_CHANNELS_SUPPORTED(ANY_SOCKET, 16) }                                            // NumberChannelsSupported
#define BLDCFG_BUS_SPEED_OVERRIDE         { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }                                              // OverrideBusSpeed
#define BLDCFG_DRAM_TECH_OVERRIDE         { 0, 0, 0, 0, 0, 0, 0, 0, 0 }                                                          // DramTechnology
#define BLDCFG_SOLDRD_DOWN_DIMM_TYPE      { 0, 0, 0, 0, 0, 0 }                                                                   // SolderedDownSoDimmType
#define BLDCFG_MEM_POWER_POLICY_OVERRIDE  { 0, 0, 0, 0, 0, 0 }                                                                   // MemoryPowerPolicyOverride
#define BLDCFG_NUM_MOTHERBOARD_LAYERS     { 0, 0, 0, 0, 0, 0 }                                                                   // MotherboardLayers

#define BLDCFG_MEMORY_QUADRANK_TYPE             QUADRANK_UNBUFFERED
#define BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING  FALSE

#define BLDCFG_ONLINE_SPARE       FALSE
#define BLDCFG_USE_BURST_MODE     FALSE
#define BLDCFG_HEAP_DRAM_ADDRESS  0xB0000ul
#define BLDCFG_1GB_ALIGN          FALSE
#define BLDCFG_UMA_ALIGNMENT      UMA_4MB_ALIGNED
#define BLDCFG_DIMM_TYPE_PACKAGE  UDIMM_PACKAGE

//
// Please Review the below SMBUS Address, based off your schematics and comment out lines which doesnt belongs
// to the project for which this fie is being customized
//

#define BLDCF_SPD_CH_A_DIMM0_ADDRESS      0x50
#define BLDCF_SPD_CH_A_DIMM1_ADDRESS      0x51

#define BLDCF_SPD_CH_B_DIMM0_ADDRESS      0x52
#define BLDCF_SPD_CH_B_DIMM1_ADDRESS      0x53

#define BLDCF_SPD_CH_C_DIMM0_ADDRESS      0x54
#define BLDCF_SPD_CH_C_DIMM1_ADDRESS      0x55

#define BLDCF_SPD_CH_D_DIMM0_ADDRESS      0x50
#define BLDCF_SPD_CH_D_DIMM1_ADDRESS      0x51

#define BLDCF_SPD_CH_E_DIMM0_ADDRESS      0x52
#define BLDCF_SPD_CH_E_DIMM1_ADDRESS      0x53

#define BLDCF_SPD_CH_F_DIMM0_ADDRESS      0x54
#define BLDCF_SPD_CH_F_DIMM1_ADDRESS      0x55

#define BLDCF_SPD_CH_G_DIMM0_ADDRESS      0x50
#define BLDCF_SPD_CH_G_DIMM1_ADDRESS      0x51

#define BLDCF_SPD_CH_H_DIMM0_ADDRESS      0x52
#define BLDCF_SPD_CH_H_DIMM1_ADDRESS      0x53

#define BLDCF_SPD_CH_I_DIMM0_ADDRESS      0x54
#define BLDCF_SPD_CH_I_DIMM1_ADDRESS      0x55

#define BLDCF_SPD_CH_J_DIMM0_ADDRESS      0x50
#define BLDCF_SPD_CH_J_DIMM1_ADDRESS      0x51

#define BLDCF_SPD_CH_K_DIMM0_ADDRESS      0x52
#define BLDCF_SPD_CH_K_DIMM1_ADDRESS      0x53

#define BLDCF_SPD_CH_L_DIMM0_ADDRESS      0x54
#define BLDCF_SPD_CH_L_DIMM1_ADDRESS      0x55

//#define BLDCFG_I2C_MUX_ADDRESS            0x94
#define BLDCFG_I2C_MUX_ADDRESS            0xE0
#define BLDCF_I2C_CHANNEL_1               1
#define BLDCF_I2C_CHANNEL_2               2
#define BLDCF_I2C_CHANNEL_3               3
#define BLDCF_I2C_CHANNEL_4               4

#define BLDCFG_CCX_MIN_CSV_ASID                1
#define BLDCFG_PSP_ENABLE_DEBUG_MODE           0
#define BLDCFG_DF_HMI_ENCRYPT                  3
#define BLDCFG_DF_XHMI_ENCRYPT                 3
#define BLDCFG_DF_SAVE_RESTORE_MEM_ENCRYPT     3
#define BLDCFG_DF_SYS_STORAGE_AT_TOM           3
#define BLDCFG_DF_PROBE_FILTER_ENABLE          3
#define BLDCFG_DF_BOTTOM_IO                    MCT_BOTTOM_IO_VALUE
#define BLDCFG_DF_MEM_INTERLEAVING             7
#define BLDCFG_DF_DRAM_INTLV_SIZE              7
#define BLDCFG_DF_CHAN_INTLV_HASH_EN           3
#define BLDCFG_DF_PCI_MMIO_SIZE                DF_PCI_MMIO_SIZE
#define BLDCFG_DF_CAKE_CRC_THRESH_PERF_BOUNDS  100  // 0.001%
#define BLDCFG_DF_MEM_CLEAR_DISABLE            3

//
// GNB Specific Configurations
//
#define BLDCFG_BMC_SOCKET_INFO  0x0F
#define BLDCFG_BMC_START_LANE   0x80
#define BLDCFG_BMC_END_LANE     0x80
#define BLDCFG_BMC_BMC_DEVICE   0
#define BLDCFG_BMC_FUNCTION     0
//
// UART Enable Disable through SOC UART/ LPC-SUPER IO
// 1 - SOC UART0 2 - SOC UART1
//
#define BLDCFG_FCH_CONSOLE_OUT_ENABLE       HygonHBLDebug_SUPPORT
#define BLDCFG_FCH_CONSOLE_OUT_SERIAL_PORT  HygonHBLDebug_Select

// #define BLDCFG_CONOUTCTRL_BREAKPOINT          FALSE
// #define BLDCFG_CONOUTCTRL_BREAKPOINT_ALLDIE   FALSE

#define BLDCFG_SOCKET_NUMBER 2               //SOCKET NUMBER is 1/2/4/8
#define BLDCFG_SOCKET_XHMI_CONNECT_TYPE 4    //XHMI_CONNECT TYPE

#endif //ifndef  _HPCB_CUSTOMIZED_DEFINITIONS_H_
