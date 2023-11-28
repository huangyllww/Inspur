/* $NoKeywords:$ */
/**
 * @file
 *
 * ApcbCustomizedDefinitions.h
 *
 * AGESA PSP Customization Block Data Intializer
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: AGESA
 * @e sub-project: ApcbGenerator
 * @e \$Revision: 192403 $ @e \$Date: 2012-12-12 15:46:25 -0600 (Wed, 12 Dec 2012) $
 *
 **/
/*****************************************************************************
*
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
* ***************************************************************************
*
*/
//
// OEM CUSTOMIZABLE DEFINITIONS. Any changes must be done here...
//

#ifndef _APCB_DEFAULTS_H_
#define _APCB_DEFAULTS_H_

#ifndef BLDCFG_FAMILY_ALL
  #define BLDCFG_FAMILY_ALL                         AMD_FAMILY_17_ALL
#endif
#ifndef BLDCFG_FAMILY_APCB
  #define BLDCFG_FAMILY_APCB                        AMD_F17_ALL_APCB
#endif

#ifndef BLDCFG_EXTERNAL_PARAMS_DEF_ENABLED
  #define BLDCFG_EXTERNAL_PARAMS_DEF_ENABLED        TRUE
#endif
#ifndef BLDCFG_EXTERNAL_PARAMS_ENABLED
  #define BLDCFG_EXTERNAL_PARAMS_ENABLED            FALSE
#endif
#ifndef BLDCFG_INTERNAL_PARAMS_DEF_ENABLED
  #define BLDCFG_INTERNAL_PARAMS_DEF_ENABLED        TRUE
#endif
#ifndef BLDCFG_INTERNAL_PARAMS_ENABLED
  #define BLDCFG_INTERNAL_PARAMS_ENABLED            FALSE
#endif


#ifndef BLDCFG_BOTTOM_IO
  #define BLDCFG_BOTTOM_IO                          0xE0
#endif
#ifndef BLDCFG_MEM_HOLE_REMAPPING
  #define BLDCFG_MEM_HOLE_REMAPPING                 TRUE
#endif
#ifndef BLDCFG_MEM_LIMIT_1TB
  #define BLDCFG_MEM_LIMIT_1TB                      TRUE
#endif
#ifndef BLDCFG_TIMING_MODE_SELECT
  #define BLDCFG_TIMING_MODE_SELECT                 TIMING_MODE_AUTO
#endif
#ifndef BLDCFG_MEMORY_CLOCK_SELECT
  #define BLDCFG_MEMORY_CLOCK_SELECT                DDR2400_FREQUENCY
#endif
#ifndef BLDCFG_MEMORY_ENABLE_CHIPSELECT_INTERLEAVING
  #define BLDCFG_MEMORY_ENABLE_CHIPSELECT_INTERLEAVING    FALSE
#endif
#ifndef BLDCFG_MEMORY_CHANNEL_INTERLEAVING
  #define BLDCFG_MEMORY_CHANNEL_INTERLEAVING        FALSE
#endif
#ifndef BLDCFG_ENABLE_ECC_FEATURE
  #define BLDCFG_ENABLE_ECC_FEATURE                 FALSE
#endif
#ifndef BLDCFG_MEMORY_POWER_DOWN
  #define BLDCFG_MEMORY_POWER_DOWN                  TRUE
#endif
#ifndef BLDCFG_PARITY_CONTROL
  #define BLDCFG_PARITY_CONTROL                     TRUE
#endif
#ifndef BLDCFG_BANK_SWIZZLE
  #define BLDCFG_BANK_SWIZZLE                       TRUE
#endif
#ifndef BLDCFG_ENABLE_MEM_CLR
  #define BLDCFG_ENABLE_MEM_CLR                     FALSE
#endif
#ifndef BLDCFG_ENABLE_MEMPSTATE
  #define BLDCFG_ENABLE_MEMPSTATE                   TRUE
#endif
#ifndef BLDCFG_UMA_MODE
  #define BLDCFG_UMA_MODE                           UMA_AUTO
#endif
#ifndef BLDCFG_UMA_SIZE
  #define BLDCFG_UMA_SIZE                           0
#endif
#ifndef BLDCFG_MEMORY_RESTORE_CONTROL
  #define BLDCFG_MEMORY_RESTORE_CONTROL             FALSE
#endif
#ifndef BLDCFG_SAVE_MEMORY_CONTEXT_CONTROL
  #define BLDCFG_SAVE_MEMORY_CONTEXT_CONTROL        TRUE
#endif
#ifndef BLDCFG_IS_CAPSULE_MODE
  #define BLDCFG_IS_CAPSULE_MODE                    TRUE
#endif
#ifndef BLDCFG_FORCE_TRAIN_MODE
  #define BLDCFG_FORCE_TRAIN_MODE                   FORCE_TRAIN_AUTO
#endif
#ifndef BLDCFG_DIMM_TYPE_TECHNOLGY
  #define BLDCFG_DIMM_TYPE_TECHNOLGY                DDR4_TECHNOLOGY
#endif
#ifndef BLDCFG_AMP_MODE_ENABLE
  #define BLDCFG_AMP_MODE_ENABLE                    TRUE
#endif
#ifndef BLDCFG_DRAM_DOUBLE_REFRESH_RATE
  #define BLDCFG_DRAM_DOUBLE_REFRESH_RATE           FALSE
#endif
#ifndef BLDCFG_PMU_TRAINING_MODE
  #define BLDCFG_PMU_TRAINING_MODE                  PMU_TRAIN_1D
#endif
#ifndef BLDCFG_ECC_REDIRECTION
  #define BLDCFG_ECC_REDIRECTION                    FALSE
#endif
#ifndef BLDCFG_SCRUB_DRAM_RATE
  #define BLDCFG_SCRUB_DRAM_RATE                    0
#endif
#ifndef BLDCFG_SCRUB_L2_RATE
  #define BLDCFG_SCRUB_L2_RATE                      0
#endif
#ifndef BLDCFG_SCRUB_L3_RATE
  #define BLDCFG_SCRUB_L3_RATE                      0
#endif
#ifndef BLDCFG_SCRUB_IC_RATE
  #define BLDCFG_SCRUB_IC_RATE                      0
#endif
#ifndef BLDCFG_SCRUB_DC_RATE
  #define BLDCFG_SCRUB_DC_RATE                      0
#endif
#ifndef BLDCFG_ECC_SYNC_FLOOD
  #define BLDCFG_ECC_SYNC_FLOOD                     0
#endif
#ifndef BLDCFG_ECC_SYMBOL_SIZE
  #define BLDCFG_ECC_SYMBOL_SIZE                    0
#endif
#ifndef BLDCFG_DQS_TRAINING_CONTROL
  #define BLDCFG_DQS_TRAINING_CONTROL               TRUE
#endif
#ifndef BLDCFG_UMA_ABOVE_4GB
  #define BLDCFG_UMA_ABOVE_4GB                      TRUE
#endif
#ifndef BLDCFG_UMA_BUFFER_ALIGNMENT
  #define BLDCFG_UMA_BUFFER_ALIGNMENT               UMA_4MB_ALIGNED
#endif
#ifndef BLDCFG_MEMORY_ALL_CLOCKS_ON
  #define BLDCFG_MEMORY_ALL_CLOCKS_ON               TRUE
#endif
#ifndef BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT
  #define BLDCFG_MEMORY_BUS_FREQUENCY_LIMIT         DDR2400_FREQUENCY
#endif
#ifndef BLDCFG_POWER_DOWN_MODE
  #define BLDCFG_POWER_DOWN_MODE                    POWER_DOWN_BY_CHANNEL
#endif
#ifndef BLDCFG_IGNORE_SPD_CHECKSUM
  #define BLDCFG_IGNORE_SPD_CHECKSUM                TRUE
#endif
#ifndef BLDCFG_MEMORY_MODE_UNGANGED
  #define BLDCFG_MEMORY_MODE_UNGANGED               TRUE
#endif
#ifndef BLDCFG_MEMORY_QUAD_RANK_CAPABLE
  #define BLDCFG_MEMORY_QUAD_RANK_CAPABLE           TRUE
#endif
#ifndef BLDCFG_MEMORY_RDIMM_CAPABLE
  #define BLDCFG_MEMORY_RDIMM_CAPABLE               TRUE
#endif
#ifndef BLDCFG_MEMORY_LRDIMM_CAPABLE
  #define BLDCFG_MEMORY_LRDIMM_CAPABLE              TRUE
#endif
#ifndef BLDCFG_MEMORY_UDIMM_CAPABLE
  #define BLDCFG_MEMORY_UDIMM_CAPABLE               TRUE
#endif
#ifndef BLDCFG_MEMORY_SODIMM_CAPABLE
  #define BLDCFG_MEMORY_SODIMM_CAPABLE              TRUE
#endif
#ifndef BLDCFG_DRAM_DOUBLE_REFRESH_RATE_ENABLE
  #define BLDCFG_DRAM_DOUBLE_REFRESH_RATE_ENABLE    FALSE
#endif
#ifndef BLDCFG_MEMORY_DDR4_CAPABLE
  #define BLDCFG_MEMORY_DDR4_CAPABLE                TRUE
#endif
#ifndef BLDCFG_MEMORY_DDR3_CAPABLE
  #define BLDCFG_MEMORY_DDR3_CAPABLE                FALSE
#endif
#ifndef BLDCFG_MEMORY_LPDDR3_CAPABLE
  #define BLDCFG_MEMORY_LPDDR3_CAPABLE              FALSE
#endif
#ifndef BLDCFG_ZQ_RESET_CONTROL_ENABLE
  #define BLDCFG_ZQ_RESET_CONTROL_ENABLE            TRUE
#endif
#ifndef BLDCFG_BANK_SWAP_GROUP_ENABLE
  #define BLDCFG_BANK_SWAP_GROUP_ENABLE             FALSE
#endif
#ifndef BLDCFG_ODT_CMD_THROT_ENABLE
  #define BLDCFG_ODT_CMD_THROT_ENABLE               FALSE
#endif
#ifndef BLDCFG_SW_CMD_THROT_ENABLE
  #define BLDCFG_SW_CMD_THROT_ENABLE                0
#endif
#ifndef BLDCFG_FORCE_POWER_DOWN_THROT_ENABLE
  #define BLDCFG_FORCE_POWER_DOWN_THROT_ENABLE      0
#endif
#ifndef BLDCFG_ODT_CMD_THROT_CYCLE
  #define BLDCFG_ODT_CMD_THROT_CYCLE                87
#endif
#ifndef BLDCFG_SW_CMD_THROT_CYCLE
  #define BLDCFG_SW_CMD_THROT_CYCLE                 0
#endif
#ifndef BLDCFG_DIMM_SENSOR_CONFIGURATION
  #define BLDCFG_DIMM_SENSOR_CONFIGURATION          0x408
#endif
#ifndef BLDCFG_DIMM_SENSOR_UPPER
  #define BLDCFG_DIMM_SENSOR_UPPER                  85
#endif
#ifndef BLDCFG_DIMM_SENSOR_LOWER
  #define BLDCFG_DIMM_SENSOR_LOWER                  10
#endif
#ifndef BLDCFG_DIMM_SENSOR_CRITICAL
  #define BLDCFG_DIMM_SENSOR_CRITICAL               95
#endif
#ifndef BLDCFG_DIMM_SENSOR_RESOLUTION
  #define BLDCFG_DIMM_SENSOR_RESOLUTION             1
#endif
#ifndef BLDCFG_AUTO_REF_FINE_GRAN_MODE
  #define BLDCFG_AUTO_REF_FINE_GRAN_MODE            0
#endif
#ifndef BLDCFG_ENABLE_MEMPSTATE
  #define BLDCFG_ENABLE_MEMPSTATE                   TRUE
#endif

#ifndef BLDCFG_MEM_MBIST_TEST_ENABLE
  #define BLDCFG_MEM_MBIST_TEST_ENABLE              FALSE
#endif

#ifndef BLDCFG_MEM_MBIST_SUBTEST_TYPE
  #define BLDCFG_MEM_MBIST_SUBTEST_TYPE             0
#endif

#ifndef BLDCFG_MEM_MBIST_AGGRESOR_ON
  #define BLDCFG_MEM_MBIST_AGGRESOR_ON              FALSE
#endif

#ifndef BLDCFG_MEM_MBIST_HALT_ON_ERROR
  #define BLDCFG_MEM_MBIST_HALT_ON_ERROR            TRUE
#endif

#ifndef BLDCFG_MEM_MBIST_CPU_VREF_RANGE
  #define BLDCFG_MEM_MBIST_CPU_VREF_RANGE           0
#endif

#ifndef BLDCFG_MEM_MBIST_DRAM_VREF_RANGE
  #define BLDCFG_MEM_MBIST_DRAM_VREF_RANGE          0
#endif

#ifndef BLDCFG_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT
  #define BLDCFG_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT 0
#endif

#ifndef BLDCFG_MEM_TSME_ENABLE
  #define BLDCFG_MEM_TSME_ENABLE                    FALSE
#endif

#ifndef BLDCFG_MEM_NVDIMM_POWER_SOURCE
  #define BLDCFG_MEM_NVDIMM_POWER_SOURCE            1
#endif

#ifndef BLDCFG_MEM_DATA_POISON
  #define BLDCFG_MEM_DATA_POISON                    TRUE
#endif

#ifndef BLDCFG_MEM_TEMP_CONTROLLED_REFRESH_EN
  #define BLDCFG_MEM_TEMP_CONTROLLED_REFRESH_EN     FALSE
#endif
#ifndef BLDCFG_MEM_DATA_SCRAMBLE
  #define BLDCFG_MEM_DATA_SCRAMBLE                  TRUE
#endif

#ifndef BLDCFG_MEM_PS_ERROR_HANDLING
  #define BLDCFG_MEM_PS_ERROR_HANDLING              0
#endif

#ifndef BLDCFG_MEM_CLK_MAP
  #define BLDCFG_MEM_CLK_MAP            { MEMCLK_DIS_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00)} // MemClkDisMap
#endif
#ifndef BLDCFG_CKE_TRI_MAP
  #define BLDCFG_CKE_TRI_MAP            { CKE_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0x05, 0x0A, 0xFF, 0xFF) }                           // CkeTriMap
#endif
#ifndef BLDCFG_ODT_TRI_MAP
  #define BLDCFG_ODT_TRI_MAP            { ODT_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x00, 0x00) }                           // OdtTriMap
#endif
#ifndef BLDCFG_CS_TRI_MAP
  #define BLDCFG_CS_TRI_MAP             { CS_TRI_MAP(ANY_SOCKET, ANY_CHANNEL, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00) }    // CsTriMap
#endif
#ifndef BLDCFG_NUM_DIMMS_SUPPORTED
  #define BLDCFG_NUM_DIMMS_SUPPORTED    { NUMBER_OF_DIMMS_SUPPORTED(ANY_SOCKET, ANY_CHANNEL, 2) }                                  // NumberDimmSupported
#endif
#ifndef BLDCFG_NUM_SOLDERED_DOWN_DIMM_SUPPORTED
  #define BLDCFG_NUM_SOLDERED_DOWN_DIMM_SUPPORTED      { 0, 0, 0, 0, 0, 0 }                                                        // NumberSolderedDownDimmsSupported
#endif
#ifndef BLDCFG_NUM_CS_SUPPORTED
  #define BLDCFG_NUM_CS_SUPPORTED       { 0, 0, 0, 0, 0, 0 }                                                                       // NumberChipSelectsSupported
#endif

#ifndef BLDCFG_NUM_CHNL_SUPPORTED
  #define BLDCFG_NUM_CHNL_SUPPORTED     { NUMBER_OF_CHANNELS_SUPPORTED(ANY_SOCKET, 2) }                                            // NumberChannelsSupported
#endif
#ifndef BLDCFG_BUS_SPEED_OVERRIDE
  #define BLDCFG_BUS_SPEED_OVERRIDE     { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }                                                  // OverrideBusSpeed
#endif
#ifndef BLDCFG_DRAM_TECH_OVERRIDE
  #define BLDCFG_DRAM_TECH_OVERRIDE     { 0, 0, 0, 0, 0, 0, 0, 0, 0 }                                                              // DramTechnology
#endif
#ifndef BLDCFG_SOLDRD_DOWN_DIMM_TYPE
  #define BLDCFG_SOLDRD_DOWN_DIMM_TYPE  { 0, 0, 0, 0, 0, 0 }                                                                       // SolderedDownSoDimmType
#endif
#ifndef BLDCFG_MEM_POWER_POLICY_OVERRIDE
  #define BLDCFG_MEM_POWER_POLICY_OVERRIDE  { 0, 0, 0, 0, 0, 0 }                                                                   // MemoryPowerPolicyOverride
#endif
#ifndef BLDCFG_NUM_MOTHERBOARD_LAYERS
  #define BLDCFG_NUM_MOTHERBOARD_LAYERS { 0, 0, 0, 0, 0, 0 }                                                                        // MotherboardLayers
#endif



#ifndef BLDCFG_MEMORY_QUADRANK_TYPE
  #define BLDCFG_MEMORY_QUADRANK_TYPE               QUADRANK_UNBUFFERED
#endif
#ifndef BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING
  #define BLDCFG_MEMORY_ENABLE_NODE_INTERLEAVING    FALSE
#endif

#ifndef BLDCFG_ONLINE_SPARE
  #define BLDCFG_ONLINE_SPARE                       FALSE
#endif
#ifndef BLDCFG_USE_BURST_MODE
  #define BLDCFG_USE_BURST_MODE                     FALSE
#endif
#ifndef BLDCFG_HEAP_DRAM_ADDRESS
  #define BLDCFG_HEAP_DRAM_ADDRESS                  0xB0000ul
#endif
#ifndef BLDCFG_1GB_ALIGN
  #define BLDCFG_1GB_ALIGN                          FALSE
#endif
#ifndef BLDCFG_UMA_ALIGNMENT
  #define BLDCFG_UMA_ALIGNMENT                      UMA_4MB_ALIGNED
#endif
#ifndef BLDCFG_DIMM_TYPE_PACKAGE
  #define BLDCFG_DIMM_TYPE_PACKAGE                  UDIMM_PACKAGE
#endif

//
// Please Review the below SMBUS Address, based off your schematics and comment out lines which doesnt belongs
// to the project for which this fie is being customized
//

#ifndef BLDCF_SPD_CH_A_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_A_DIMM0_ADDRESS      0xA0
#endif
#ifndef BLDCF_SPD_CH_A_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_A_DIMM1_ADDRESS      0xA4
#endif

#ifndef BLDCF_SPD_CH_B_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_B_DIMM0_ADDRESS      0xA2
#endif
#ifndef BLDCF_SPD_CH_B_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_B_DIMM1_ADDRESS      0xA6
#endif

#ifndef BLDCF_SPD_CH_C_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_C_DIMM0_ADDRESS      0xA8
#endif
#ifndef BLDCF_SPD_CH_C_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_C_DIMM1_ADDRESS      0xAC
#endif

#ifndef BLDCF_SPD_CH_D_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_D_DIMM0_ADDRESS      0xAA
#endif
#ifndef BLDCF_SPD_CH_D_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_D_DIMM1_ADDRESS      0xAE
#endif

#ifndef BLDCF_SPD_CH_E_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_E_DIMM0_ADDRESS      0xA0
#endif
#ifndef BLDCF_SPD_CH_E_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_E_DIMM1_ADDRESS      0xA4
#endif

#ifndef BLDCF_SPD_CH_F_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_F_DIMM0_ADDRESS      0xA2
#endif
#ifndef BLDCF_SPD_CH_F_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_F_DIMM1_ADDRESS      0xA6
#endif

#ifndef BLDCF_SPD_CH_G_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_G_DIMM0_ADDRESS      0xA8
#endif
#ifndef BLDCF_SPD_CH_G_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_G_DIMM1_ADDRESS      0xAC
#endif

#ifndef BLDCF_SPD_CH_H_DIMM0_ADDRESS
  #define BLDCF_SPD_CH_H_DIMM0_ADDRESS      0xAA
#endif
#ifndef BLDCF_SPD_CH_H_DIMM1_ADDRESS
  #define BLDCF_SPD_CH_H_DIMM1_ADDRESS      0xAE
#endif

#ifndef BLDCFG_MEM_LAYOOUT_TYPE
  #define BLDCFG_MEM_LAYOOUT_TYPE           MEMORY_LAYOUT_TYPE_DIMM
#endif


#ifndef BLDCFG_I2C_MUX_ADRRESS
  #define BLDCFG_I2C_MUX_ADRRESS            0x94
#endif
#ifndef BLDCF_I2C_CHANNEL_1
  #define BLDCF_I2C_CHANNEL_1               1
#endif
#ifndef BLDCF_I2C_CHANNEL_2
  #define BLDCF_I2C_CHANNEL_2               2
#endif
#ifndef BLDCF_I2C_CHANNEL_3
  #define BLDCF_I2C_CHANNEL_3               3
#endif
#ifndef BLDCF_I2C_CHANNEL_4
  #define BLDCF_I2C_CHANNEL_4               4
#endif

#ifndef BLDCFG_CCX_MIN_SEV_ASID
  #define BLDCFG_CCX_MIN_SEV_ASID             5
#endif

#ifndef BLDCFG_PSP_ENABLE_DEBUG_MODE
  #define BLDCFG_PSP_ENABLE_DEBUG_MODE        0
#endif

#ifndef BLDCFG_DF_GMI_ENCRYPT
  #define BLDCFG_DF_GMI_ENCRYPT               3
#endif
#ifndef BLDCFG_DF_XGMI_ENCRYPT
  #define BLDCFG_DF_XGMI_ENCRYPT              3
#endif
#ifndef BLDCFG_DF_SAVE_RESTORE_MEM_ENCRYPT
  #define BLDCFG_DF_SAVE_RESTORE_MEM_ENCRYPT  3
#endif
#ifndef BLDCFG_DF_SYS_STORAGE_AT_TOM
  #define BLDCFG_DF_SYS_STORAGE_AT_TOM        3
#endif
#ifndef BLDCFG_DF_PROBE_FILTER_ENABLE
  #define BLDCFG_DF_PROBE_FILTER_ENABLE       3
#endif
#ifndef BLDCFG_DF_BOTTOM_IO
  #define BLDCFG_DF_BOTTOM_IO                 0xE0
#endif
#ifndef BLDCFG_DF_MEM_INTERLEAVING
  #define BLDCFG_DF_MEM_INTERLEAVING          7
#endif
#ifndef BLDCFG_DF_DRAM_INTLV_SIZE
  #define BLDCFG_DF_DRAM_INTLV_SIZE           7
#endif
#ifndef BLDCFG_DF_CHAN_INTLV_HASH_EN
  #define BLDCFG_DF_CHAN_INTLV_HASH_EN        3
#endif
#ifndef BLDCFG_DF_PCI_MMIO_SIZE
  #define BLDCFG_DF_PCI_MMIO_SIZE             0x10000000
#endif
#ifndef BLDCFG_DF_CAKE_CRC_THRESH_PERF_BOUNDS
  #define BLDCFG_DF_CAKE_CRC_THRESH_PERF_BOUNDS  100  // 0.001%
#endif
#ifndef BLDCFG_DF_MEM_CLEAR
  #define BLDCFG_DF_MEM_CLEAR  3
#endif

//
// GNB Specific Definitions
//
#ifndef BLDCFG_BMC_SOCKET_INFO
  #define BLDCFG_BMC_SOCKET_INFO              0x0F
#endif
#ifndef BLDCFG_BMC_START_LANE
  #define BLDCFG_BMC_START_LANE               0x80
#endif
#ifndef BLDCFG_BMC_END_LANE
  #define BLDCFG_BMC_END_LANE                 0x80
#endif

#ifndef BLDCFG_BMC_BMC_DEVICE
  #define BLDCFG_BMC_BMC_DEVICE               0
#endif

#ifndef BLDCFG_BMC_FUNCTION
  #define BLDCFG_BMC_FUNCTION                 0
#endif
//
// UART Enable Disable through SOC UART/ LPC-SUPER IO
// 0 - LPC UART, 1 - SOC UART0
//
#ifndef BLDCFG_FCH_CONSOLE_OUT_ENABLE
  #define BLDCFG_FCH_CONSOLE_OUT_ENABLE         0
#endif

#ifndef BLDCFG_FCH_CONSOLE_OUT_SERIAL_PORT
  #define BLDCFG_FCH_CONSOLE_OUT_SERIAL_PORT    1
#endif


//#define BLDCFG_CONOUTCTRL_BREAKPOINT          FALSE
//#define BLDCFG_CONOUTCTRL_BREAKPOINT_ALLDIE   FALSE

#endif   //ifndef  _APCB_DEFAULTS_H_