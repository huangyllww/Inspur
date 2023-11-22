/* $NoKeywords:$ */
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
 ******************************************************************************
 */
#ifndef _CXLREGISTEROFFSET_H_
#define _CXLREGISTEROFFSET_H_

#define HYGON_CXL_VIDDID                                    0x14AE1D94
#define HYGON_CXL_RC_DEV                                    3
#define HYGON_CXL_RC_FUN                                    1

#define MAX_HDM_COUNT_PER_DEVICE                            2

#define NBIO0_CXL_BACKDOOR                                  0x780F000
#define NBIO0_CXL_CLK_CNTL                                  0x780F004
#define NBIO0_CXL_CLK_STATUS                                0x780F008
#define NBIO0_CXL_PM_CNTL                                   0x780F00C
#define NBIO0_CXL_PM_STATUS                                 0x780F010
#define NBIO0_CXL_MISC_CNTL0                                0x780F014
#define NBIO0_CXL_MISC_CNTL1                                0x780F018
#define NBIO0_CXL_MISC_STATUS0                              0x780F01C
#define NBIO0_CXL_MISC_STATUS1                              0x780F020
#define NBIO0_CXL_LC_CNTL                                   0x780F028
#define NBIO0_CXL_LC_STATUS0                                0x780F02C
#define NBIO0_CXL_LC_STATUS1                                0x780F030
#define NBIO0_CXL_LC_STATUS2                                0x780F034
#define NBIO0_CXL_LC_STATUS3                                0x780F038
#define NBIO0_CXL_LC_STATUS4                                0x780F03C
#define NBIO0_CXL_LC_STATUS5                                0x780F040
#define NBIO0_CXL_LC_STATUS6                                0x780F044
#define NBIO0_CXL_MST_CNTL0                                 0x780F04C
#define NBIO0_CXL_MST_CNTL1                                 0x780F050
#define NBIO0_CXL_MST_STATUS0                               0x780F054
#define NBIO0_CXL_SLV_CNTL0                                 0x780F058
#define NBIO0_CXL_SLV_CNTL1                                 0x780F05C
#define NBIO0_CXL_SLV_STATUS0                               0x780F060
#define NBIO0_CXL_TX_CNTL0                                  0x780F068
#define NBIO0_CXL_RX_CNTL0                                  0x780F074
#define NBIO0_CXL_RX_STATUS0                                0x780F07C
#define NBIO0_CXL_RSMU_BIOS_TIMER_CMD                       0x780F080
#define NBIO0_CXL_RSMU_BIOS_TIMER_CNTL                      0x780F084
#define NBIO0_CXL_RSMU_BIOS_TIMER_DEBUG                     0x780F088
#define NBIO0_CXL_PCIEP_HPGI_PRIVATE                        0x780F08C
#define NBIO0_CXL_PCIEP_HPGI                                0x780F090
#define NBIO0_CXL_PCIEP_HCNT_DESCRIPTOR                     0x780F094
#define NBIO0_CXL_SMU_HP_STATUS_UPDATE                      0x780F098
#define NBIO0_CXL_HP_SMU_COMMAND_UPDATE                     0x780F09C
#define NBIO0_CXL_SMU_HP_END_OF_INTERRUPT                   0x780F0A0
#define NBIO0_CXL_SMU_INT_PIN_SHARING_PORT_INDICATOR        0x780F0A4
#define NBIO0_CXL_LINK_MANAGEMENT_STATUS                    0x780F0A8
#define NBIO0_CXL_LINK_MANAGEMENT_MASK                      0x780F0AC
#define NBIO0_CXL_ERR_VDM_RCV_DATA                          0x780F0B4
#define NBIO0_CXL_PM_VDM_RCV_DATA0                          0x780F0B8
#define NBIO0_CXL_PM_VDM_RCV_DATA1                          0x780F0BC
#define NBIO0_CXL_PM_VDM_RCV_DATA2                          0x780F0C0
#define NBIO0_CXL_PM_VDM_SND_DATA0                          0x780F0C4
#define NBIO0_CXL_PM_VDM_SND_DATA1                          0x780F0C8
#define NBIO0_CXL_PM_VDM_SND_DATA2                          0x780F0CC
#define NBIO0_LCCNT_CONTROL                                 0x780F0D0
#define NBIO0_CXL_CFG_LNC_WINDOW_REGISTER                   0x780F0D4
#define NBIO0_LCCNT_QUAN_THRD                               0x780F0D8
#define NBIO0_LCCNT_WEIGHT                                  0x780F0DC
#define NBIO0_CX_LNC_TOTAL_WACC_REGISTER                    0x780F0E0
#define NBIO0_CXL_LNC_BW_WACC_REGISTER                      0x780F0E4
#define NBIO0_CXL_LNC_CMN_WACC_REGISTER                     0x780F0E8
#define NBIO0_CXL_BRIDGE_CONTROL                            0x780E000
#define NBIO0_REG_CTS_Reg_Ras_Info                          0x780E004
#define NBIO0_TYPE1_DEV_ID_VEND_ID_REG                      0x7800000
#define NBIO0_TYPE1_STATUS_COMMAND_REG                      0x7800004
#define NBIO0_TYPE1_CLASS_CODE_REV_ID_REG                   0x7800008
#define NBIO0_TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG   0x780000C
#define NBIO0_BAR0_REG                                      0x7800010
#define NBIO0_BAR1_REG                                      0x7800014
#define NBIO0_SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG     0x7800018
#define NBIO0_SEC_STAT_IO_LIMIT_IO_BASE_REG                 0x780001C
#define NBIO0_MEM_LIMIT_MEM_BASE_REG                        0x7800020
#define NBIO0_PREF_MEM_LIMIT_PREF_MEM_BASE_REG              0x7800024
#define NBIO0_PREF_BASE_UPPER_REG                           0x7800028
#define NBIO0_PREF_LIMIT_UPPER_REG                          0x780002C
#define NBIO0_IO_LIMIT_UPPER_IO_BASE_UPPER_REG              0x7800030
#define NBIO0_TYPE1_CAP_PTR_REG                             0x7800034
#define NBIO0_TYPE1_EXP_ROM_BASE_REG                        0x7800038
#define NBIO0_BRIDGE_CTRL_INT_PIN_INT_LINE_REG              0x780003C
#define NBIO0_SPCIE_CAP_HEADER_REG                          0x7800270
#define NBIO0_LINK_CONTROL3_REG                             0x7800274
#define NBIO0_LANE_ERR_STATUS_REG                           0x7800278
#define NBIO0_SPCIE_CAP_OFF_0CH_REG                         0x780027C
#define NBIO0_SPCIE_CAP_OFF_10H_REG                         0x7800280
#define NBIO0_SPCIE_CAP_OFF_14H_REG                         0x7800284
#define NBIO0_SPCIE_CAP_OFF_18H_REG                         0x7800288
#define NBIO0_SPCIE_CAP_OFF_1CH_REG                         0x780028C
#define NBIO0_SPCIE_CAP_OFF_20H_REG                         0x7800290
#define NBIO0_SPCIE_CAP_OFF_24H_REG                         0x7800294
#define NBIO0_SPCIE_CAP_OFF_28H_REG                         0x7800298
#define NBIO0_PL16G_EXT_CAP_HDR_REG                         0x7800420
#define NBIO0_PL16G_CAPABILITY_REG                          0x7800424
#define NBIO0_PL16G_CONTROL_REG                             0x7800428
#define NBIO0_PL16G_STATUS_REG                              0x780042C
#define NBIO0_PL16G_LC_DPAR_STATUS_REG                      0x7800430
#define NBIO0_PL16G_FIRST_RETIMER_DPAR_STATUS_REG           0x7800434
#define NBIO0_PL16G_SECOND_RETIMER_DPAR_STATUS_REG          0x7800438
#define NBIO0_PL16G_CAP_OFF_20H_REG                         0x7800440
#define NBIO0_PL16G_CAP_OFF_24H_REG                         0x7800444
#define NBIO0_PL16G_CAP_OFF_28H_REG                         0x7800448
#define NBIO0_PL16G_CAP_OFF_2CH_REG                         0x780044C
#define NBIO0_PL32G_EXT_CAP_HDR_REG                         0x78004A0
#define NBIO0_PL32G_CAPABILITY_REG                          0x78004A4
#define NBIO0_PL32G_CONTROL_REG                             0x78004A8
#define NBIO0_PL32G_STATUS_REG                              0x78004AC
#define NBIO0_PL32G_RCVD_MOD_TS_DATA1_REG                   0x78004B0
#define NBIO0_PL32G_RCVD_MOD_TS_DATA2_REG                   0x78004B4
#define NBIO0_PL32G_TX_MOD_TS_DATA1_REG                     0x78004B8
#define NBIO0_PL32G_TX_MOD_TS_DATA2_REG                     0x78004BC
#define NBIO0_PL32G_CAP_OFF_20H_REG                         0x78004C0
#define NBIO0_PL32G_CAP_OFF_24H_REG                         0x78004C4
#define NBIO0_PL32G_CAP_OFF_28H_REG                         0x78004C8
#define NBIO0_PL32G_CAP_OFF_2CH_REG                         0x78004CC
#define NBIO0_MARGIN_EXT_CAP_HDR_REG                        0x7800450
#define NBIO0_MARGIN_PORT_CAPABILITIES_STATUS_REG           0x7800454
#define NBIO0_MARGIN_LANE_CNTRL_STATUS0_REG                 0x7800458
#define NBIO0_MARGIN_LANE_CNTRL_STATUS1_REG                 0x780045C
#define NBIO0_MARGIN_LANE_CNTRL_STATUS2_REG                 0x7800460
#define NBIO0_MARGIN_LANE_CNTRL_STATUS3_REG                 0x7800464
#define NBIO0_MARGIN_LANE_CNTRL_STATUS4_REG                 0x7800468
#define NBIO0_MARGIN_LANE_CNTRL_STATUS5_REG                 0x780046C
#define NBIO0_MARGIN_LANE_CNTRL_STATUS6_REG                 0x7800470
#define NBIO0_MARGIN_LANE_CNTRL_STATUS7_REG                 0x7800474
#define NBIO0_MARGIN_LANE_CNTRL_STATUS8_REG                 0x7800478
#define NBIO0_MARGIN_LANE_CNTRL_STATUS9_REG                 0x780047C
#define NBIO0_MARGIN_LANE_CNTRL_STATUS10_REG                0x7800480
#define NBIO0_MARGIN_LANE_CNTRL_STATUS11_REG                0x7800484
#define NBIO0_MARGIN_LANE_CNTRL_STATUS12_REG                0x7800488
#define NBIO0_MARGIN_LANE_CNTRL_STATUS13_REG                0x780048C
#define NBIO0_MARGIN_LANE_CNTRL_STATUS14_REG                0x7800490
#define NBIO0_MARGIN_LANE_CNTRL_STATUS15_REG                0x7800494
#define NBIO0_RAS_DES_CAP_HEADER_REG                        0x78005B0
#define NBIO0_VENDOR_SPECIFIC_HEADER_REG                    0x78005B4
#define NBIO0_EVENT_COUNTER_CONTROL_REG                     0x78005B8
#define NBIO0_EVENT_COUNTER_DATA_REG                        0x78005BC
#define NBIO0_TIME_BASED_ANALYSIS_CONTROL_REG               0x78005C0
#define NBIO0_TIME_BASED_ANALYSIS_DATA_REG                  0x78005C4
#define NBIO0_TIME_BASED_ANALYSIS_DATA_63_32_REG            0x78005C8
#define NBIO0_EINJ_ENABLE_REG                               0x78005E0
#define NBIO0_EINJ0_CRC_REG                                 0x78005E4
#define NBIO0_EINJ1_SEQNUM_REG                              0x78005E8
#define NBIO0_EINJ2_DLLP_REG                                0x78005EC
#define NBIO0_EINJ3_SYMBOL_REG                              0x78005F0
#define NBIO0_EINJ4_FC_REG                                  0x78005F4
#define NBIO0_EINJ5_SP_TLP_REG                              0x78005F8
#define NBIO0_EINJ6_COMPARE_POINT_H0_REG                    0x78005FC
#define NBIO0_EINJ6_COMPARE_POINT_H1_REG                    0x7800600
#define NBIO0_EINJ6_COMPARE_POINT_H2_REG                    0x7800604
#define NBIO0_EINJ6_COMPARE_POINT_H3_REG                    0x7800608
#define NBIO0_EINJ6_COMPARE_VALUE_H0_REG                    0x780060C
#define NBIO0_EINJ6_COMPARE_VALUE_H1_REG                    0x7800610
#define NBIO0_EINJ6_COMPARE_VALUE_H2_REG                    0x7800614
#define NBIO0_EINJ6_COMPARE_VALUE_H3_REG                    0x7800618
#define NBIO0_EINJ6_CHANGE_POINT_H0_REG                     0x780061C
#define NBIO0_EINJ6_CHANGE_POINT_H1_REG                     0x7800620
#define NBIO0_EINJ6_CHANGE_POINT_H2_REG                     0x7800624
#define NBIO0_EINJ6_CHANGE_POINT_H3_REG                     0x7800628
#define NBIO0_EINJ6_CHANGE_VALUE_H0_REG                     0x780062C
#define NBIO0_EINJ6_CHANGE_VALUE_H1_REG                     0x7800630
#define NBIO0_EINJ6_CHANGE_VALUE_H2_REG                     0x7800634
#define NBIO0_EINJ6_CHANGE_VALUE_H3_REG                     0x7800638
#define NBIO0_EINJ6_TLP_REG                                 0x780063C
#define NBIO0_SD_CONTROL1_REG                               0x7800650
#define NBIO0_SD_CONTROL2_REG                               0x7800654
#define NBIO0_SD_STATUS_L1LANE_REG                          0x7800660
#define NBIO0_SD_STATUS_L1LTSSM_REG                         0x7800664
#define NBIO0_SD_STATUS_PM_REG                              0x7800668
#define NBIO0_SD_STATUS_L2_REG                              0x780066C
#define NBIO0_SD_STATUS_L3FC_REG                            0x7800670
#define NBIO0_SD_STATUS_L3_REG                              0x7800674
#define NBIO0_SD_EQ_CONTROL1_REG                            0x7800680
#define NBIO0_SD_EQ_CONTROL2_REG                            0x7800684
#define NBIO0_SD_EQ_CONTROL3_REG                            0x7800688
#define NBIO0_SD_EQ_STATUS1_REG                             0x7800690
#define NBIO0_SD_EQ_STATUS2_REG                             0x7800694
#define NBIO0_SD_EQ_STATUS3_REG                             0x7800698
#define NBIO0_SN_BASE                                       0x7800100
#define NBIO0_SER_NUM_REG_DW_1                              0x7800104
#define NBIO0_SER_NUM_REG_DW_2                              0x7800108
#define NBIO0_CXL_RCRB_PCIE_EXT_CAP_HDR_OFF                 0x7800500
#define NBIO0_CXL_RCRB_DVSEC_PORT_HDR_1_OFF                 0x7800504
#define NBIO0_CXL_RCRB_DVSEC_HDR_2_FLEXBUS_CAP_OFF          0x7800508
#define NBIO0_CXL_RCRB_FLEXBUS_CNTRL_STATUS_OFF             0x780050C
#define NBIO0_CXL_RCRB_FLEXBUS_RCVD_MOD_TS_PH1_OFF          0x7800510
#define NBIO0_CXL_2_0_EXT_DVSEC_CAP_HDR_OFF                 0x7800514
#define NBIO0_CXL_2_0_EXT_DVSEC_HDR_1_OFF                   0x7800518
#define NBIO0_CXL_2_0_EXT_DVSEC_HDR_2_OFF                   0x780051C
#define NBIO0_CXL_2_0_CTRL_ALT_BUS_BASE_LIMIT_OFF           0x7800520
#define NBIO0_CXL_2_0_ALT_MEM_BASE_LIMIT_OFF                0x7800524
#define NBIO0_CXL_2_0_ALT_PREFETCH_MEM_BASE_LIMIT_OFF       0x7800528
#define NBIO0_CXL_2_0_ALT_PREFETCH_MEM_BASE_HIGH_OFF        0x780052C
#define NBIO0_CXL_2_0_ALT_PREFETCH_MEM_LIMIT_HIGH_OFF       0x7800530
#define NBIO0_CXL_2_0_CXL_RCRB_BASE_OFF                     0x7800534
#define NBIO0_CXL_2_0_CXL_RCRB_BASE_HIGH_OFF                0x7800538
#define NBIO0_CXL_GPF_PORT_DVSEC_HDR_OFF                    0x7800560
#define NBIO0_CXL_GPF_PORT_DVSEC_HDR_1_OFF                  0x7800564
#define NBIO0_CXL_GPF_PORT_DVSEC_HDR_2_OFF                  0x7800568
#define NBIO0_CXL_GPF_PORT_PH1_CTRL_PH2_CTRL_OFF            0x780056C
#define NBIO0_CXL_REG_LOC_DVSEC_HDR_OFF                     0x780053C
#define NBIO0_CXL_REG_LOC_DVSEC_HDR_1_OFF                   0x7800540
#define NBIO0_CXL_REG_LOC_DVSEC_HDR_2_OFF                   0x7800544
#define NBIO0_CXL_REG_LOC_BLOCK_1_LOW_OFF                   0x7800548
#define NBIO0_CXL_REG_LOC_BLOCK_1_HIGH_OFF                  0x780054C
#define NBIO0_CXL_REG_LOC_BLOCK_2_LOW_OFF                   0x7800550
#define NBIO0_CXL_REG_LOC_BLOCK_2_HIGH_OFF                  0x7800554
#define NBIO0_CXL_REG_LOC_BLOCK_3_LOW_OFF                   0x7800558
#define NBIO0_CXL_REG_LOC_BLOCK_3_HIGH_OFF                  0x780055C
#define NBIO0_DPC_EXT_CAP_HEADER_OFF                        0x7800390
#define NBIO0_DPC_CAP_CONTROL_OFF                           0x7800394
#define NBIO0_DPC_STATUS_ERR_SOURCE_ID_OFF                  0x7800398
#define NBIO0_DPC_RP_PIO_STATUS_OFF                         0x780039C
#define NBIO0_DPC_RP_PIO_MASK_OFF                           0x78003A0
#define NBIO0_DPC_RP_PIO_SEVERITY_OFF                       0x78003A4
#define NBIO0_DPC_RP_PIO_SYSERROR_OFF                       0x78003A8
#define NBIO0_DPC_RP_PIO_EXCEPTION_OFF                      0x78003AC
#define NBIO0_DPC_RP_PIO_HEADER_LOG0_OFF                    0x78003B0
#define NBIO0_DPC_RP_PIO_HEADER_LOG1_OFF                    0x78003B4
#define NBIO0_DPC_RP_PIO_HEADER_LOG2_OFF                    0x78003B8
#define NBIO0_DPC_RP_PIO_HEADER_LOG3_OFF                    0x78003BC
#define NBIO0_DPC_RP_PIO_IMPSPEC_LOG_OFF                    0x78003C0
#define NBIO0_DPC_RP_PIO_PREFIX_LOG_OFF                     0x78003C4
#define NBIO0_CAP_ID_NXT_PTR_REG                            0x7800050
#define NBIO0_CON_STATUS_REG                                0x7800054
#define NBIO0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG    0x7800058
#define NBIO0_DEVICE_CAPABILITIES_REG                       0x780005C
#define NBIO0_DEVICE_CONTROL_DEVICE_STATUS                  0x7800060
#define NBIO0_LINK_CAPABILITIES_REG                         0x7800064
#define NBIO0_LINK_CONTROL_LINK_STATUS_REG                  0x7800068
#define NBIO0_SLOT_CAPABILITIES_REG                         0x780006C
#define NBIO0_SLOT_CONTROL_SLOT_STATUS                      0x7800070
#define NBIO0_ROOT_CONTROL_ROOT_CAPABILITIES_REG            0x7800074
#define NBIO0_ROOT_STATUS_REG                               0x7800078
#define NBIO0_DEVICE_CAPABILITIES2_REG                      0x780007C
#define NBIO0_DEVICE_CONTROL2_DEVICE_STATUS2_REG            0x7800080
#define NBIO0_LINK_CAPABILITIES2_REG                        0x7800084
#define NBIO0_LINK_CONTROL2_LINK_STATUS2_REG                0x7800088
#define NBIO0_AER_EXT_CAP_HDR_OFF                           0x7800150
#define NBIO0_UNCORR_ERR_STATUS_OFF                         0x7800154
#define NBIO0_UNCORR_ERR_MASK_OFF                           0x7800158
#define NBIO0_UNCORR_ERR_SEV_OFF                            0x780015C
#define NBIO0_CORR_ERR_STATUS_OFF                           0x7800160
#define NBIO0_CORR_ERR_MASK_OFF                             0x7800164
#define NBIO0_ADV_ERR_CAP_CTRL_OFF                          0x7800168
#define NBIO0_HDR_LOG_0_OFF                                 0x780016C
#define NBIO0_HDR_LOG_1_OFF                                 0x7800170
#define NBIO0_HDR_LOG_2_OFF                                 0x7800174
#define NBIO0_HDR_LOG_3_OFF                                 0x7800178
#define NBIO0_ROOT_ERR_CMD_OFF                              0x780017C
#define NBIO0_ROOT_ERR_STATUS_OFF                           0x7800180
#define NBIO0_ERR_SRC_ID_OFF                                0x7800184
#define NBIO0_TLP_PREFIX_LOG_1_OFF                          0x7800188
#define NBIO0_TLP_PREFIX_LOG_2_OFF                          0x780018C
#define NBIO0_TLP_PREFIX_LOG_3_OFF                          0x7800190
#define NBIO0_TLP_PREFIX_LOG_4_OFF                          0x7800194
#define NBIO0_TPH_EXT_CAP_HDR_REG                           0x7800230
#define NBIO0_TPH_REQ_CAP_REG_REG                           0x7800234
#define NBIO0_TPH_REQ_CONTROL_REG_REG                       0x7800238
#define NBIO0_PCI_MSI_CAP_ID_NEXT_CTRL_REG                  0x78000A0
#define NBIO0_MSI_CAP_OFF_04H_REG                           0x78000A4
#define NBIO0_MSI_CAP_OFF_08H_REG                           0x78000A8
#define NBIO0_MSI_CAP_OFF_0CH_REG                           0x78000AC
#define NBIO0_ATS_CAP_HDR_REG                               0x780023C
#define NBIO0_ATS_CAPABILITIES_CTRL_REG                     0x7800240
#define NBIO0_ACS_CAP_HDR_REG                               0x78002A0
#define NBIO0_ACS_CAPABILITIES_CTRL_REG                     0x78002A4
#define NBIO0_DATA_LINK_FEATURE_EXT_HDR_OFF                 0x7800410
#define NBIO0_DATA_LINK_FEATURE_CAP_OFF                     0x7800414
#define NBIO0_DATA_LINK_FEATURE_STATUS_OFF                  0x7800418
#define NBIO0_ACK_LATENCY_TIMER_OFF                         0x7800700
#define NBIO0_VENDOR_SPEC_DLLP_OFF                          0x7800704
#define NBIO0_PORT_FORCE_OFF                                0x7800708
#define NBIO0_ACK_F_ASPM_CTRL_OFF                           0x780070C
#define NBIO0_PORT_LINK_CTRL_OFF                            0x7800710
#define NBIO0_LANE_SKEW_OFF                                 0x7800714
#define NBIO0_TIMER_CTRL_MAX_FUNC_NUM_OFF                   0x7800718
#define NBIO0_SYMBOL_TIMER_FILTER_1_OFF                     0x780071C
#define NBIO0_FILTER_MASK_2_OFF                             0x7800720
#define NBIO0_PL_DEBUG0_OFF                                 0x7800728
#define NBIO0_PL_DEBUG1_OFF                                 0x780072C
#define NBIO0_TX_P_FC_CREDIT_STATUS_OFF                     0x7800730
#define NBIO0_TX_NP_FC_CREDIT_STATUS_OFF                    0x7800734
#define NBIO0_TX_CPL_FC_CREDIT_STATUS_OFF                   0x7800738
#define NBIO0_QUEUE_STATUS_OFF                              0x780073C
#define NBIO0_VC_TX_ARBI_1_OFF                              0x7800740
#define NBIO0_VC_TX_ARBI_2_OFF                              0x7800744
#define NBIO0_VC0_P_RX_Q_CTRL_OFF                           0x7800748
#define NBIO0_VC0_NP_RX_Q_CTRL_OFF                          0x780074C
#define NBIO0_VC0_CPL_RX_Q_CTRL_OFF                         0x7800750
#define NBIO0_GEN2_CTRL_OFF                                 0x780080C
#define NBIO0_PHY_STATUS_OFF                                0x7800810
#define NBIO0_PHY_CONTROL_OFF                               0x7800814
#define NBIO0_TRGT_MAP_CTRL_OFF                             0x780081C
#define NBIO0_CLOCK_GATING_CTRL_OFF                         0x780088C
#define NBIO0_GEN3_RELATED_OFF                              0x7800890
#define NBIO0_GEN3_EQ_CONTROL_OFF                           0x78008A8
#define NBIO0_GEN3_EQ_FB_MODE_DIR_CHANGE_OFF                0x78008AC
#define NBIO0_ORDER_RULE_CTRL_OFF                           0x78008B4
#define NBIO0_PIPE_LOOPBACK_CONTROL_OFF                     0x78008B8
#define NBIO0_MISC_CONTROL_1_OFF                            0x78008BC
#define NBIO0_MULTI_LANE_CONTROL_OFF                        0x78008C0
#define NBIO0_PHY_INTEROP_CTRL_OFF                          0x78008C4
#define NBIO0_TRGT_CPL_LUT_DELETE_ENTRY_OFF                 0x78008C8
#define NBIO0_PCIE_VERSION_NUMBER_OFF                       0x78008F8
#define NBIO0_PCIE_VERSION_TYPE_OFF                         0x78008FC
#define NBIO0_PL_APP_BUS_DEV_NUM_STATUS_OFF                 0x7800B10
#define NBIO0_PCIPM_TRAFFIC_CTRL_OFF                        0x7800B1C
#define NBIO0_PL_LTR_LATENCY_OFF                            0x7800B30
#define NBIO0_AUX_CLK_FREQ_OFF                              0x7800B40
#define NBIO0_POWERDOWN_CTRL_STATUS_OFF                     0x7800B48
#define NBIO0_PHY_INTEROP_CTRL_2_OFF                        0x7800B4C
#define NBIO0_GEN4_LANE_MARGINING_1_OFF                     0x7800B80
#define NBIO0_GEN4_LANE_MARGINING_2_OFF                     0x7800B84
#define NBIO0_GEN5_LANE_MARGINING_1_OFF                     0x7800B88
#define NBIO0_GEN5_LANE_MARGINING_2_OFF                     0x7800B8C
#define NBIO0_PIPE_RELATED_OFF                              0x7800B90
#define NBIO0_RX_SERIALIZATION_Q_CTRL_OFF                   0x7800C00
#define NBIO0_CXL_VLSM_CSR_REG_OFF                          0x7800C48
#define NBIO0_CXL_LRSM_CSR_REG_OFF                          0x7800C4C
#define NBIO0_CXL_LL_CTRL_CR_REG_OFF                        0x7800C50
#define NBIO0_CXL_FATAL_ERR_CR_REG_OFF                      0x7800C54
#define NBIO0_CXL_CACHE_MEM_LL_INJ_OFF                      0x7800C58
#define NBIO0_CACHE_MEM_RX_CRC_INJ_CTRL                     0x7800C98
#define NBIO0_CXL_IO_LL_INJ_OFF                             0x7800C5C
#define NBIO0_FLEXBUS_LOG_PHY_ERR_INJ_OFF                   0x7800C60
#define NBIO0_CXL_PORT_LOGIC_VIRAL_STS_OFF                  0x7800C64
#define NBIO0_DBI_FUNCTION_BANK_CTRL_REG_OFF                0x7800C7C
#define NBIO0_UTILITY_OFF                                   0x7800C80
#define NBIO0_PM_UTILITY_OFF                                0x7800C88

#define NBIO0_PCIE_RCECEPA_ENH_CAP_LIST                     0xBC00158
#define NBIO0_PCIE_RCECEPA_ASSOCI_BITMAP                    0xBC0015C
#define NBIO0_PCIE_RCECEPA_ASSOCI_ADDRESS                   0xBC00160
#define NBIO0_RCEC_CFG                                      0xBC01000
#define NBIO0_RCEC_EXT_BDF_CHECK                            0xBC01004
#define NBIO0_RCEC_EXT_BDF_01                               0xBC01008
#define NBIO0_RCEC_EXT_BDF_23                               0xBC0100C

#define NBIO0_IOHC_SHADOW_CXL_PORT_CNTL_AND_ALT_BUS_RANGE   0x800400C
#define NBIO0_IOHC_SHADOW_CXL_ALT_MEM_BASE_LIMIT            0x8004010
#define NBIO0_IOHC_SHADOW_CXL_ALT_PREF_BASE_LIMIT_LOW       0x8004014
#define NBIO0_IOHC_SHADOW_CXL_ALT_PREF_BASE_HIGH            0x8004018
#define NBIO0_IOHC_SHADOW_CXL_ALT_PREF_LIMIT_HIGH           0x800401C
#define NBIO0_IOHC_SHADOW_CXL_RCRB_BASE_LOW                 0x8004020
#define NBIO0_IOHC_SHADOW_CXL_RCRB_BASE_HIGH                0x8004024

#define NBIO0_IOHC_SHADOW_CXL_MEM_BASE_LIMIT                0x8002020
#define NBIO0_IOHC_SHADOW_CXL_PREF_MEM_BASE_LIMIT           0x8002024
#define NBIO0_IOHC_SHADOW_CXL_PREF_MEM_BASE_UPPER           0x8002028
#define NBIO0_IOHC_SHADOW_CXL_PREF_MEM_LIMIT_UPPER          0x800202C
#endif
