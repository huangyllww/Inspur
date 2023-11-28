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
 *
 ***************************************************************************/


#ifndef _AMD_CBS_FORM_ID_H_
#define _AMD_CBS_FORM_ID_H_

  #define CBS_CONFIGURATION_VARSTORE_ID                                           0x5000
// Section ID
  #define SETUP_CBS_ROOT_LABLE                                                    0x7000
  #define SETUP_ZEN_COMMON_OPTIONS_LABLE                                          0x7001
  #define SETUP_DF_COMMON_OPTIONS_LABLE                                           0x7002
  #define SETUP_UMC_COMMON_OPTIONS_LABLE                                          0x7003
  #define SETUP_NBIO_COMMON_OPTIONS_LABLE                                         0x7004
  #define SETUP_FCH_COMMON_OPTIONS_LABLE                                          0x7005
  #define SETUP_PROMONTORY_COMMON_OPTIONS_LABLE                                   0x7006
  #define SETUP_NTB_COMMON_OPTIONS_LABLE                                          0x7007
  #define SETUP_CUSTOM_CORE_PSTATES_LABLE                                         0x7009
  #define SETUP_CORE_THREAD_ENABLEMENT_LABLE                                      0x700A
  #define SETUP_PREFETCHER_SETTINGS_LABLE                                         0x700B
  #define SETUP_DECLINE_LABLE                                                     0x700C
  #define SETUP_ACCEPT_LABLE                                                      0x700D
  #define SETUP_DISAGREE_LABLE                                                    0x702E
  #define SETUP_AGREE_LABLE                                                       0x702F
  #define SETUP_DDR4_COMMON_OPTIONS_LABLE                                         0x7030
  #define SETUP_DRAM_MEMORY_MAPPING_LABLE                                         0x7031
  #define SETUP_MEMORY_MBIST_LABLE                                                0x7032
  #define SETUP_DRAM_TIMING_CONFIGURATION_LABLE                                   0x7033
  #define SETUP_DRAM_CONTROLLER_CONFIGURATION_LABLE                               0x7034
  #define SETUP_DATA_BUS_CONFIGURATION_LABLE                                      0x7035
  #define SETUP_COMMON_RAS_LABLE                                                  0x7036
  #define SETUP_SECURITY_LABLE                                                    0x7037
  #define SETUP_I_DECLINE_LABLE                                                   0x7038
  #define SETUP_I_ACCEPT_LABLE                                                    0x7039
  #define SETUP_DRAM_POWER_OPTIONS_LABLE                                          0x703A
  #define SETUP_ECC_CONFIGURATION_LABLE                                           0x703B
  #define SETUP_NB_CONFIGURATION_LABLE                                            0x703E
  #define SETUP_FAN_CONTROL_LABLE                                                 0x703F
  #define SETUP_HOT_PLUG_FLAGS_LABLE                                              0x7040
  #define SETUP_SATA_CONFIGURATION_OPTIONS_LABLE                                  0x7041
  #define SETUP_USB_CONFIGURATION_OPTIONS_LABLE                                   0x7042
  #define SETUP_SD_SECURE_DIGITAL_OPTIONS_LABLE                                   0x7043
  #define SETUP_AC_POWER_LOSS_OPTIONS_LABLE                                       0x7044
  #define SETUP_I2C_CONFIGURATION_OPTIONS_LABLE                                   0x7045
  #define SETUP_UART_CONFIGURATION_OPTIONS_LABLE                                  0x7046
  #define SETUP_ESPI_CONFIGURATION_OPTIONS_LABLE                                  0x7047
  #define SETUP_MCM_USB_ENABLE_LABLE                                              0x7048
  #define SETUP_PT_SATA_CONFIGURATION_OPTIONS_LABLE                               0x7049
  #define SETUP_PT_USB_CONFIGURATION_OPTIONS_LABLE                                0x704A
  #define SETUP_PROM4_USB_PORT_CONFIGURATION_OPTIONS_LABLE                        0x704B
  #define SETUP_PROM2_USB_PORT_CONFIGURATION_OPTIONS_LABLE                        0x704C
  #define SETUP_PROM1_USB_PORT_CONFIGURATION_OPTIONS_LABLE                        0x704D

// KEY function ID
  #define KEY_OC_MODE                                                             0x7008
  #define KEY_CUSTOM_PSTATE0                                                      0x700E
  #define KEY_PSTATE0_FID                                                         0x700F
  #define KEY_PSTATE0_DID                                                         0x7010
  #define KEY_PSTATE0_VID                                                         0x7011
  #define KEY_CUSTOM_PSTATE1                                                      0x7012
  #define KEY_PSTATE1_FID                                                         0x7013
  #define KEY_PSTATE1_DID                                                         0x7014
  #define KEY_PSTATE1_VID                                                         0x7015
  #define KEY_CUSTOM_PSTATE2                                                      0x7016
  #define KEY_PSTATE2_FID                                                         0x7017
  #define KEY_PSTATE2_DID                                                         0x7018
  #define KEY_PSTATE2_VID                                                         0x7019
  #define KEY_CUSTOM_PSTATE3                                                      0x701A
  #define KEY_PSTATE3_FID                                                         0x701B
  #define KEY_PSTATE3_DID                                                         0x701C
  #define KEY_PSTATE3_VID                                                         0x701D
  #define KEY_CUSTOM_PSTATE4                                                      0x701E
  #define KEY_PSTATE4_FID                                                         0x701F
  #define KEY_PSTATE4_DID                                                         0x7020
  #define KEY_PSTATE4_VID                                                         0x7021
  #define KEY_CUSTOM_PSTATE5                                                      0x7022
  #define KEY_PSTATE5_FID                                                         0x7023
  #define KEY_PSTATE5_DID                                                         0x7024
  #define KEY_PSTATE5_VID                                                         0x7025
  #define KEY_CUSTOM_PSTATE6                                                      0x7026
  #define KEY_PSTATE6_FID                                                         0x7027
  #define KEY_PSTATE6_DID                                                         0x7028
  #define KEY_PSTATE6_VID                                                         0x7029
  #define KEY_CUSTOM_PSTATE7                                                      0x702A
  #define KEY_PSTATE7_FID                                                         0x702B
  #define KEY_PSTATE7_DID                                                         0x702C
  #define KEY_PSTATE7_VID                                                         0x702D
  #define KEY_TSME                                                                0x703C
  #define KEY_DATA_SCRAMBLE                                                       0x703D

#endif // _AMD_CBS_FORM_ID_H_
