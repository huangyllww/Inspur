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

#ifndef _HYGON_CBS_FORM_ID_H_
#define _HYGON_CBS_FORM_ID_H_

#define CBS_CONFIGURATION_VARSTORE_ID  0x5000
// Section ID
#define SETUP_CBS_ROOT_LABEL                              0x7000
#define SETUP_CORE_COMMON_OPTIONS_LABEL                   0x7001
#define SETUP_DF_COMMON_OPTIONS_LABEL                     0x7002
#define SETUP_UMC_COMMON_OPTIONS_LABEL                    0x7003
#define SETUP_NBIO_COMMON_OPTIONS_LABEL                   0x7004
#define SETUP_FCH_COMMON_OPTIONS_LABEL                    0x7005
#define SETUP_CXL_COMMON_OPTIONS_LABEL                    0x7006
#define SETUP_NTB_COMMON_OPTIONS_LABEL                    0x7007
#define SETUP_RAS_COMMON_OPTIONS_LABEL                    0x7008
#define SETUP_CUSTOM_CORE_PSTATES_LABEL                   0x7009
#define SETUP_CORE_THREAD_ENABLEMENT_LABEL                0x700A
#define SETUP_PREFETCHER_SETTINGS_LABEL                   0x700B
#define SETUP_DECLINE_LABEL                               0x700C
#define SETUP_ACCEPT_LABEL                                0x700D
#define SETUP_DISAGREE_LABEL                              0x700E
#define SETUP_AGREE_LABEL                                 0x700F
#define SETUP_DRAM_MEMORY_MAPPING_LABEL                   0x7031
#define SETUP_MEMORY_MBIST_LABEL                          0x7032
#define SETUP_DRAM_TIMING_CONFIGURATION_LABEL             0x7033
#define SETUP_DRAM_CONTROLLER_CONFIGURATION_LABEL         0x7034
#define SETUP_DATA_BUS_CONFIGURATION_LABEL                0x7035
#define SETUP_SECURITY_LABEL                              0x7037
#define SETUP_I_DECLINE_LABEL                             0x7038
#define SETUP_I_ACCEPT_LABEL                              0x7039
#define SETUP_DRAM_POWER_OPTIONS_LABEL                    0x703A
#define SETUP_ECC_CONFIGURATION_LABEL                     0x703B
#define SETUP_FAN_CONTROL_LABEL                           0x703F
#define SETUP_HOT_PLUG_FLAGS_LABEL                        0x7040
#define SETUP_SATA_CONFIGURATION_OPTIONS_LABEL            0x7041
#define SETUP_USB_CONFIGURATION_OPTIONS_LABEL             0x7042
#define SETUP_SD_SECURE_DIGITAL_OPTIONS_LABEL             0x7043
#define SETUP_AC_POWER_LOSS_OPTIONS_LABEL                 0x7044
#define SETUP_I2C_CONFIGURATION_OPTIONS_LABEL             0x7045
#define SETUP_UART_CONFIGURATION_OPTIONS_LABEL            0x7046
#define SETUP_ESPI_CONFIGURATION_OPTIONS_LABEL            0x7047
#define SETUP_MCM_USB_ENABLE_LABEL                        0x7048
#define SETUP_PT_SATA_CONFIGURATION_OPTIONS_LABEL         0x7049
#define SETUP_PT_USB_CONFIGURATION_OPTIONS_LABEL          0x704A
#define SETUP_PROM4_USB_PORT_CONFIGURATION_OPTIONS_LABEL  0x704B
#define SETUP_PROM2_USB_PORT_CONFIGURATION_OPTIONS_LABEL  0x704C
#define SETUP_PROM1_USB_PORT_CONFIGURATION_OPTIONS_LABEL  0x704D
#define SETUP_DEBUG_LEVEL_CONFIGURATION_OPTIONS_LABEL     0x704E
#define SETUP_VALIDATION_COMMON_OPTIONS_LABEL             0x704F
#define SETUP_SPI_CONFIGURATION_OPTIONS_LABEL             0x7050

// KEY function ID
#define KEY_OC_MODE         0x7060
#define KEY_CUSTOM_PSTATE0  0x7061
#define KEY_PSTATE0_FID     0x7062
#define KEY_PSTATE0_DID     0x7063
#define KEY_PSTATE0_VID     0x7064
#define KEY_CUSTOM_PSTATE1  0x7065
#define KEY_PSTATE1_FID     0x7066
#define KEY_PSTATE1_DID     0x7067
#define KEY_PSTATE1_VID     0x7068
#define KEY_CUSTOM_PSTATE2  0x7069
#define KEY_PSTATE2_FID     0x706A
#define KEY_PSTATE2_DID     0x706B
#define KEY_PSTATE2_VID     0x706C
#define KEY_TSME            0x7071
#define KEY_DATA_SCRAMBLE   0x7072

// New Added ID
#define SETUP_DDR_RAS_SUPPORT_LABEL                         0x7100
#define SETUP_DF_RAS_SUPPORT_LABEL                          0x7101
#define SETUP_NBIO_RAS_SUPPORT_LABEL                        0x7102
#define SETUP_COMMON_DDR5_RAS_LABEL                         0x7103
#define SETUP_DDR5_COMMON_OPTIONS_LABEL                     0x7104
#define SETUP_SATA_RAS_SUPPORT_LABEL                        0x7106
#define SETUP_VALIDATION_FORCE_VOLTAGE_AND_FREQUENCE_LABEL  0x7108

#endif // _HYGON_CBS_FORM_ID_H_
