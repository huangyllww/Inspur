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


//This file is auto generated, don't edit it manually

#ifndef _IDS_NV_DEF_ZP_H_
#define _IDS_NV_DEF_ZP_H_

///RedirectForReturnDis
///From a workaround for GCC/C000005 issue for XV Core on CZ A0, setting MSRC001_1029 Decode Configuration (DE_CFG) bit 14 [DecfgNoRdrctForReturns] to 1
typedef enum {
  IDSOPT_CMN_CPU_GEN_W_A05_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_CPU_GEN_W_A05_1 = 1,///<1
  IDSOPT_CMN_CPU_GEN_W_A05_0 = 0,///<0
} IDSOPT_CMN_CPU_GEN_W_A05;

///L2 TLB Associativity
///0 - L2 TLB ways [11:8] are fully associative.  1 - =L2 TLB ways [11:8] are 4K-only.
typedef enum {
  IDSOPT_CMN_CPU_L2_TLB_WAY_ALLOC_0 = 0,///<0
  IDSOPT_CMN_CPU_L2_TLB_WAY_ALLOC_1 = 1,///<1
  IDSOPT_CMN_CPU_L2_TLB_WAY_ALLOC_AUTO = 3,///<Auto
} IDSOPT_CMN_CPU_L2_TLB_WAY_ALLOC;

///Platform First Error Handling
///Enable/disable PFEH, cloak individual banks, and mask deferred error interrupts from each bank.
typedef enum {
  IDSOPT_CMN_CPU_PFEH_ENABLED = 1,///<Enabled
  IDSOPT_CMN_CPU_PFEH_DISABLED = 0,///<Disabled
  IDSOPT_CMN_CPU_PFEH_AUTO = 3,///<Auto
} IDSOPT_CMN_CPU_PFEH;

///Core Performance Boost
///Disable CPB
typedef enum {
  IDSOPT_CMN_CPU_CPB_DISABLED = 0,///<Disabled
  IDSOPT_CMN_CPU_CPB_AUTO = 1,///<Auto
} IDSOPT_CMN_CPU_CPB;

///Enable IBS
///Enables IBS through MSRC001_1005[42] and disables SpecLockMap through MSRC001_1020[54]
typedef enum {
  IDSOPT_CMN_CPU_EN_IBS_AUTO = 3,///<Auto
  IDSOPT_CMN_CPU_EN_IBS_ENABLED = 1,///<Enabled
  IDSOPT_CMN_CPU_EN_IBS_DISABLED = 0,///<Disabled
} IDSOPT_CMN_CPU_EN_IBS;

///Global C-state Control
///Controls IO based C-state generation and DF C-states.
typedef enum {
  IDSOPT_CMN_CPU_GLOBAL_CSTATE_CTRL_DISABLED = 0,///<Disabled
  IDSOPT_CMN_CPU_GLOBAL_CSTATE_CTRL_ENABLED = 1,///<Enabled
  IDSOPT_CMN_CPU_GLOBAL_CSTATE_CTRL_AUTO = 3,///<Auto
} IDSOPT_CMN_CPU_GLOBAL_CSTATE_CTRL;

///Opcache Control
///Enables or disables the Opcache
typedef enum {
  IDSOPT_CMN_CPU_OPCACHE_CTRL_DISABLED = 0,///<Disabled
  IDSOPT_CMN_CPU_OPCACHE_CTRL_ENABLED = 1,///<Enabled
  IDSOPT_CMN_CPU_OPCACHE_CTRL_AUTO = 3,///<Auto
} IDSOPT_CMN_CPU_OPCACHE_CTRL;

///OC Mode
///OC1 - 16 cores/3.6GHz on 1.3375V\nOC2 - 8 cores/3.7GHz on 1.369V\nOC3 - 4 cores/3.75GHz on 1.374V\nMax Stress - 16 cores/3.8GHz on 1.400V\n\nWARNING - DAMAGE CAUSED BY USE OF YOUR AMD PROCESSOR OUTSIDE OF SPECIFICATION OR IN EXCESS OF FACTORY SETTINGS ARE NOT COVERED UNDER YOUR AMD PRODUCT WARRANTY AND MAY NOT BE COVERED BY YOUR SYSTEM MANUFACTURER'S WARRANTY.\nOperating your AMD processor outside of specification or in excess of factory settings, including but not limited to overclocking, may damage or shorten the life of your processor or other system components, create system instabilities (e.g., data loss and corrupted images) and in extreme cases may result in total system failure. AMD does not provide support or service for issues or damages related to use of an AMD processor outside of processor specifications or in excess of factory settings.\n
typedef enum {
  IDSOPT_CMN_CPU_OC_MODE_NORMALOPERATION = 0,///<Normal Operation
  IDSOPT_CMN_CPU_OC_MODE_OC1 = 1,///<OC1
  IDSOPT_CMN_CPU_OC_MODE_OC2 = 2,///<OC2
  IDSOPT_CMN_CPU_OC_MODE_OC3 = 3,///<OC3
  IDSOPT_CMN_CPU_OC_MODE_MAXSTRESS = 4,///<Max Stress
} IDSOPT_CMN_CPU_OC_MODE;

///SEV-ES ASID Space Limit
///SEV VMs using ASIDs below the SEV-ES ASID Space Limit must enable the SEV-ES feature. The valid values for this field are from 0x1 (1) - 0x10 (16).
#define IDSOPT_CMN_CPU_SEV_ASID_SPACE_LIMIT_MIN 1 ///< Min of SEV-ES ASID Space Limit
#define IDSOPT_CMN_CPU_SEV_ASID_SPACE_LIMIT_MAX 16 ///< Max of SEV-ES ASID Space Limit

///Streaming Stores Control
///Enables or disables the streaming stores functionality
typedef enum {
  IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_DISABLED = 0,///<Disabled
  IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_ENABLED = 1,///<Enabled
  IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_AUTO = 3,///<Auto
} IDSOPT_CMN_CPU_STREAMING_STORES_CTRL;

///Custom Core Pstates Legal Disclaimer
///Legal Disclaimer
///Custom Core Pstates Legal Disclaimer 1
///Legal Disclaimer
///Custom Pstate0
///Disable - disable this Pstate\nCustom - customize this Pstate, applicable only if PcdOcDisable=FALSE\n\nWARNING - DAMAGE CAUSED BY USE OF YOUR AMD PROCESSOR OUTSIDE OF SPECIFICATION OR IN EXCESS OF FACTORY SETTINGS ARE NOT COVERED UNDER YOUR AMD PRODUCT WARRANTY AND MAY NOT BE COVERED BY YOUR SYSTEM MANUFACTURER'S WARRANTY.\nOperating your AMD processor outside of specification or in excess of factory settings, including but not limited to overclocking, may damage or shorten the life of your processor or other system components, create system instabilities (e.g., data loss and corrupted images) and in extreme cases may result in total system failure. AMD does not provide support or service for issues or damages related to use of an AMD processor outside of processor specifications or in excess of factory settings.\n
typedef enum {
  IDSOPT_CPU_PST_CUSTOM_P0_CUSTOM = 1,///<Custom
  IDSOPT_CPU_PST_CUSTOM_P0_AUTO = 2,///<Auto
} IDSOPT_CPU_PST_CUSTOM_P0;

///  Frequency (MHz)
///Current core frequency in MHz
#define IDSOPT_CPU_COF_P0_MIN 0 ///< Min of   Frequency (MHz)
#define IDSOPT_CPU_COF_P0_MAX 0xffffffff ///< Max of   Frequency (MHz)

///  Voltage (uV)
///Voltage in uV (1V = 1000 * 1000 uV)
#define IDSOPT_CPU_VOLTAGE_P0_MIN 0 ///< Min of   Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P0_MAX 0xffffffff ///< Max of   Voltage (uV)

///  Pstate0 FID
///Specifies the core frequency multiplier. COF = 200MHz * FID / DID
#define IDSOPT_CPU_PST0_FID_MIN 0x10 ///< Min of   Pstate0 FID
#define IDSOPT_CPU_PST0_FID_MAX 0xff ///< Max of   Pstate0 FID

///  Pstate0 DID
///Specifies the core frequency divisor (DID[0] should zero if DID[5:0]>1Ah).
#define IDSOPT_CPU_PST0_DID_MIN 0x8 ///< Min of   Pstate0 DID
#define IDSOPT_CPU_PST0_DID_MAX 0x30 ///< Max of   Pstate0 DID

///  Pstate0 VID
///Specifies the core voltage.
#define IDSOPT_CPU_PST0_VID_MIN 0 ///< Min of   Pstate0 VID
#define IDSOPT_CPU_PST0_VID_MAX 0xff ///< Max of   Pstate0 VID

///Custom Pstate1
///Disabled - disable this Pstate\nCustom - customize this Pstate, applicable only if PcdOcDisable=FALSE\n\nWARNING - DAMAGE CAUSED BY USE OF YOUR AMD PROCESSOR OUTSIDE OF SPECIFICATION OR IN EXCESS OF FACTORY SETTINGS ARE NOT COVERED UNDER YOUR AMD PRODUCT WARRANTY AND MAY NOT BE COVERED BY YOUR SYSTEM MANUFACTURER'S WARRANTY.\nOperating your AMD processor outside of specification or in excess of factory settings, including but not limited to overclocking, may damage or shorten the life of your processor or other system components, create system instabilities (e.g., data loss and corrupted images) and in extreme cases may result in total system failure. AMD does not provide support or service for issues or damages related to use of an AMD processor outside of processor specifications or in excess of factory settings.\n
typedef enum {
  IDSOPT_CPU_PST_CUSTOM_P1_DISABLED = 0,///<Disabled
  IDSOPT_CPU_PST_CUSTOM_P1_CUSTOM = 1,///<Custom
  IDSOPT_CPU_PST_CUSTOM_P1_AUTO = 2,///<Auto
} IDSOPT_CPU_PST_CUSTOM_P1;

///  Frequency (MHz)
#define IDSOPT_CPU_COF_P1_MIN 0 ///< Min of   Frequency (MHz)
#define IDSOPT_CPU_COF_P1_MAX 0xffffffff ///< Max of   Frequency (MHz)

///  Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P1_MIN 0 ///< Min of   Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P1_MAX 0xffffffff ///< Max of   Voltage (uV)

///  Pstate1 FID
///Specifies the core frequency multiplier. COF = 200MHz * FID / DID
#define IDSOPT_CPU_PST1_FID_MIN 0x10 ///< Min of   Pstate1 FID
#define IDSOPT_CPU_PST1_FID_MAX 0xff ///< Max of   Pstate1 FID

///  Pstate1 DID
///Specifies the core frequency divisor (DID[0] should zero if DID[5:0]>1Ah).
#define IDSOPT_CPU_PST1_DID_MIN 0x8 ///< Min of   Pstate1 DID
#define IDSOPT_CPU_PST1_DID_MAX 0x30 ///< Max of   Pstate1 DID

///  Pstate1 VID
///Specifies the core voltage.
#define IDSOPT_CPU_PST1_VID_MIN 0 ///< Min of   Pstate1 VID
#define IDSOPT_CPU_PST1_VID_MAX 0xff ///< Max of   Pstate1 VID

///Custom Pstate2
///Disabled - disable this Pstate\nCustom - customize this Pstate, applicable only if PcdOcDisable=FALSE\n\nWARNING - DAMAGE CAUSED BY USE OF YOUR AMD PROCESSOR OUTSIDE OF SPECIFICATION OR IN EXCESS OF FACTORY SETTINGS ARE NOT COVERED UNDER YOUR AMD PRODUCT WARRANTY AND MAY NOT BE COVERED BY YOUR SYSTEM MANUFACTURER'S WARRANTY.\nOperating your AMD processor outside of specification or in excess of factory settings, including but not limited to overclocking, may damage or shorten the life of your processor or other system components, create system instabilities (e.g., data loss and corrupted images) and in extreme cases may result in total system failure. AMD does not provide support or service for issues or damages related to use of an AMD processor outside of processor specifications or in excess of factory settings.\n
typedef enum {
  IDSOPT_CPU_PST_CUSTOM_P2_DISABLED = 0,///<Disabled
  IDSOPT_CPU_PST_CUSTOM_P2_CUSTOM = 1,///<Custom
  IDSOPT_CPU_PST_CUSTOM_P2_AUTO = 2,///<Auto
} IDSOPT_CPU_PST_CUSTOM_P2;

///  Frequency (MHz)
#define IDSOPT_CPU_COF_P2_MIN 0 ///< Min of   Frequency (MHz)
#define IDSOPT_CPU_COF_P2_MAX 0xffffffff ///< Max of   Frequency (MHz)

///  Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P2_MIN 0 ///< Min of   Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P2_MAX 0xffffffff ///< Max of   Voltage (uV)

///  Pstate2 FID
///Specifies the core frequency multiplier. COF = 200MHz * FID / DID
#define IDSOPT_CPU_PST2_FID_MIN 0x10 ///< Min of   Pstate2 FID
#define IDSOPT_CPU_PST2_FID_MAX 0xff ///< Max of   Pstate2 FID

///  Pstate2 DID
///Specifies the core frequency divisor (DID[0] should zero if DID[5:0]>1Ah).
#define IDSOPT_CPU_PST2_DID_MIN 0x8 ///< Min of   Pstate2 DID
#define IDSOPT_CPU_PST2_DID_MAX 0x30 ///< Max of   Pstate2 DID

///  Pstate2 VID
///Specifies the core voltage.
#define IDSOPT_CPU_PST2_VID_MIN 0 ///< Min of   Pstate2 VID
#define IDSOPT_CPU_PST2_VID_MAX 0xff ///< Max of   Pstate2 VID

///Custom Pstate3
///Disabled - disable this Pstate\nCustom - customize this Pstate, applicable only if PcdOcDisable=FALSE\n\nWARNING - DAMAGE CAUSED BY USE OF YOUR AMD PROCESSOR OUTSIDE OF SPECIFICATION OR IN EXCESS OF FACTORY SETTINGS ARE NOT COVERED UNDER YOUR AMD PRODUCT WARRANTY AND MAY NOT BE COVERED BY YOUR SYSTEM MANUFACTURER'S WARRANTY.\nOperating your AMD processor outside of specification or in excess of factory settings, including but not limited to overclocking, may damage or shorten the life of your processor or other system components, create system instabilities (e.g., data loss and corrupted images) and in extreme cases may result in total system failure. AMD does not provide support or service for issues or damages related to use of an AMD processor outside of processor specifications or in excess of factory settings.\n
typedef enum {
  IDSOPT_CPU_PST_CUSTOM_P3_DISABLED = 0,///<Disabled
  IDSOPT_CPU_PST_CUSTOM_P3_CUSTOM = 1,///<Custom
  IDSOPT_CPU_PST_CUSTOM_P3_AUTO = 2,///<Auto
} IDSOPT_CPU_PST_CUSTOM_P3;

///  Frequency (MHz)
#define IDSOPT_CPU_COF_P3_MIN 0 ///< Min of   Frequency (MHz)
#define IDSOPT_CPU_COF_P3_MAX 0xffffffff ///< Max of   Frequency (MHz)

///  Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P3_MIN 0 ///< Min of   Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P3_MAX 0xffffffff ///< Max of   Voltage (uV)

///  Pstate3 FID
///Specifies the core frequency multiplier. COF = 200MHz * FID / DID
#define IDSOPT_CPU_PST3_FID_MIN 0x10 ///< Min of   Pstate3 FID
#define IDSOPT_CPU_PST3_FID_MAX 0xff ///< Max of   Pstate3 FID

///  Pstate3 DID
///Specifies the core frequency divisor (DID[0] should zero if DID[5:0]>1Ah).
#define IDSOPT_CPU_PST3_DID_MIN 0x8 ///< Min of   Pstate3 DID
#define IDSOPT_CPU_PST3_DID_MAX 0x30 ///< Max of   Pstate3 DID

///  Pstate3 VID
///Specifies the core voltage.
#define IDSOPT_CPU_PST3_VID_MIN 0 ///< Min of   Pstate3 VID
#define IDSOPT_CPU_PST3_VID_MAX 0xff ///< Max of   Pstate3 VID

///Custom Pstate4
///Disabled - disable this Pstate\nCustom - customize this Pstate, applicable only if PcdOcDisable=FALSE\n\nWARNING - DAMAGE CAUSED BY USE OF YOUR AMD PROCESSOR OUTSIDE OF SPECIFICATION OR IN EXCESS OF FACTORY SETTINGS ARE NOT COVERED UNDER YOUR AMD PRODUCT WARRANTY AND MAY NOT BE COVERED BY YOUR SYSTEM MANUFACTURER'S WARRANTY.\nOperating your AMD processor outside of specification or in excess of factory settings, including but not limited to overclocking, may damage or shorten the life of your processor or other system components, create system instabilities (e.g., data loss and corrupted images) and in extreme cases may result in total system failure. AMD does not provide support or service for issues or damages related to use of an AMD processor outside of processor specifications or in excess of factory settings.\n
typedef enum {
  IDSOPT_CPU_PST_CUSTOM_P4_DISABLED = 0,///<Disabled
  IDSOPT_CPU_PST_CUSTOM_P4_CUSTOM = 1,///<Custom
  IDSOPT_CPU_PST_CUSTOM_P4_AUTO = 2,///<Auto
} IDSOPT_CPU_PST_CUSTOM_P4;

///  Frequency (MHz)
#define IDSOPT_CPU_COF_P4_MIN 0 ///< Min of   Frequency (MHz)
#define IDSOPT_CPU_COF_P4_MAX 0xffffffff ///< Max of   Frequency (MHz)

///  Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P4_MIN 0 ///< Min of   Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P4_MAX 0xffffffff ///< Max of   Voltage (uV)

///  Pstate4 FID
///Specifies the core frequency multiplier. COF = 200MHz * FID / DID
#define IDSOPT_CPU_PST4_FID_MIN 0x10 ///< Min of   Pstate4 FID
#define IDSOPT_CPU_PST4_FID_MAX 0xff ///< Max of   Pstate4 FID

///  Pstate4 DID
///Specifies the core frequency divisor (DID[0] should zero if DID[5:0]>1Ah).
#define IDSOPT_CPU_PST4_DID_MIN 0x8 ///< Min of   Pstate4 DID
#define IDSOPT_CPU_PST4_DID_MAX 0x30 ///< Max of   Pstate4 DID

///  Pstate4 VID
///Specifies the core voltage.
#define IDSOPT_CPU_PST4_VID_MIN 0 ///< Min of   Pstate4 VID
#define IDSOPT_CPU_PST4_VID_MAX 0xff ///< Max of   Pstate4 VID

///Custom Pstate5
///Disabled - disable this Pstate\nCustom - customize this Pstate, applicable only if PcdOcDisable=FALSE\n\nWARNING - DAMAGE CAUSED BY USE OF YOUR AMD PROCESSOR OUTSIDE OF SPECIFICATION OR IN EXCESS OF FACTORY SETTINGS ARE NOT COVERED UNDER YOUR AMD PRODUCT WARRANTY AND MAY NOT BE COVERED BY YOUR SYSTEM MANUFACTURER'S WARRANTY.\nOperating your AMD processor outside of specification or in excess of factory settings, including but not limited to overclocking, may damage or shorten the life of your processor or other system components, create system instabilities (e.g., data loss and corrupted images) and in extreme cases may result in total system failure. AMD does not provide support or service for issues or damages related to use of an AMD processor outside of processor specifications or in excess of factory settings.\n
typedef enum {
  IDSOPT_CPU_PST_CUSTOM_P5_DISABLED = 0,///<Disabled
  IDSOPT_CPU_PST_CUSTOM_P5_CUSTOM = 1,///<Custom
  IDSOPT_CPU_PST_CUSTOM_P5_AUTO = 2,///<Auto
} IDSOPT_CPU_PST_CUSTOM_P5;

///  Frequency (MHz)
#define IDSOPT_CPU_COF_P5_MIN 0 ///< Min of   Frequency (MHz)
#define IDSOPT_CPU_COF_P5_MAX 0xffffffff ///< Max of   Frequency (MHz)

///  Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P5_MIN 0 ///< Min of   Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P5_MAX 0xffffffff ///< Max of   Voltage (uV)

///  Pstate5 FID
///Specifies the core frequency multiplier. COF = 200MHz * FID / DID
#define IDSOPT_CPU_PST5_FID_MIN 0x10 ///< Min of   Pstate5 FID
#define IDSOPT_CPU_PST5_FID_MAX 0xff ///< Max of   Pstate5 FID

///  Pstate5 DID
///Specifies the core frequency divisor (DID[0] should zero if DID[5:0]>1Ah).
#define IDSOPT_CPU_PST5_DID_MIN 0x8 ///< Min of   Pstate5 DID
#define IDSOPT_CPU_PST5_DID_MAX 0x30 ///< Max of   Pstate5 DID

///  Pstate5 VID
///Specifies the core voltage.
#define IDSOPT_CPU_PST5_VID_MIN 0 ///< Min of   Pstate5 VID
#define IDSOPT_CPU_PST5_VID_MAX 0xff ///< Max of   Pstate5 VID

///Custom Pstate6
///Disabled - disable this Pstate\nCustom - customize this Pstate, applicable only if PcdOcDisable=FALSE\n\nWARNING - DAMAGE CAUSED BY USE OF YOUR AMD PROCESSOR OUTSIDE OF SPECIFICATION OR IN EXCESS OF FACTORY SETTINGS ARE NOT COVERED UNDER YOUR AMD PRODUCT WARRANTY AND MAY NOT BE COVERED BY YOUR SYSTEM MANUFACTURER'S WARRANTY.\nOperating your AMD processor outside of specification or in excess of factory settings, including but not limited to overclocking, may damage or shorten the life of your processor or other system components, create system instabilities (e.g., data loss and corrupted images) and in extreme cases may result in total system failure. AMD does not provide support or service for issues or damages related to use of an AMD processor outside of processor specifications or in excess of factory settings.\n
typedef enum {
  IDSOPT_CPU_PST_CUSTOM_P6_DISABLED = 0,///<Disabled
  IDSOPT_CPU_PST_CUSTOM_P6_CUSTOM = 1,///<Custom
  IDSOPT_CPU_PST_CUSTOM_P6_AUTO = 2,///<Auto
} IDSOPT_CPU_PST_CUSTOM_P6;

///  Frequency (MHz)
#define IDSOPT_CPU_COF_P6_MIN 0 ///< Min of   Frequency (MHz)
#define IDSOPT_CPU_COF_P6_MAX 0xffffffff ///< Max of   Frequency (MHz)

///  Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P6_MIN 0 ///< Min of   Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P6_MAX 0xffffffff ///< Max of   Voltage (uV)

///  Pstate6 FID
///Specifies the core frequency multiplier. COF = 200MHz * FID / DID
#define IDSOPT_CPU_PST6_FID_MIN 0x10 ///< Min of   Pstate6 FID
#define IDSOPT_CPU_PST6_FID_MAX 0xff ///< Max of   Pstate6 FID

///  Pstate6 DID
///Specifies the core frequency divisor (DID[0] should zero if DID[5:0]>1Ah).
#define IDSOPT_CPU_PST6_DID_MIN 0x8 ///< Min of   Pstate6 DID
#define IDSOPT_CPU_PST6_DID_MAX 0x30 ///< Max of   Pstate6 DID

///  Pstate6 VID
///Specifies the core voltage.
#define IDSOPT_CPU_PST6_VID_MIN 0 ///< Min of   Pstate6 VID
#define IDSOPT_CPU_PST6_VID_MAX 0xff ///< Max of   Pstate6 VID

///Custom Pstate7
///Disabled - disable this Pstate\nCustom - customize this Pstate, applicable only if PcdOcDisable=FALSE\n\nWARNING - DAMAGE CAUSED BY USE OF YOUR AMD PROCESSOR OUTSIDE OF SPECIFICATION OR IN EXCESS OF FACTORY SETTINGS ARE NOT COVERED UNDER YOUR AMD PRODUCT WARRANTY AND MAY NOT BE COVERED BY YOUR SYSTEM MANUFACTURER'S WARRANTY.\nOperating your AMD processor outside of specification or in excess of factory settings, including but not limited to overclocking, may damage or shorten the life of your processor or other system components, create system instabilities (e.g., data loss and corrupted images) and in extreme cases may result in total system failure. AMD does not provide support or service for issues or damages related to use of an AMD processor outside of processor specifications or in excess of factory settings.\n
typedef enum {
  IDSOPT_CPU_PST_CUSTOM_P7_DISABLED = 0,///<Disabled
  IDSOPT_CPU_PST_CUSTOM_P7_CUSTOM = 1,///<Custom
  IDSOPT_CPU_PST_CUSTOM_P7_AUTO = 2,///<Auto
} IDSOPT_CPU_PST_CUSTOM_P7;

///  Frequency (MHz)
#define IDSOPT_CPU_COF_P7_MIN 0 ///< Min of   Frequency (MHz)
#define IDSOPT_CPU_COF_P7_MAX 0xffffffff ///< Max of   Frequency (MHz)

///  Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P7_MIN 0 ///< Min of   Voltage (uV)
#define IDSOPT_CPU_VOLTAGE_P7_MAX 0xffffffff ///< Max of   Voltage (uV)

///  Pstate7 FID
///Specifies the core frequency multiplier. COF = 200MHz * FID / DID
#define IDSOPT_CPU_PST7_FID_MIN 0x10 ///< Min of   Pstate7 FID
#define IDSOPT_CPU_PST7_FID_MAX 0xff ///< Max of   Pstate7 FID

///  Pstate7 DID
///Specifies the core frequency divisor (DID[0] should zero if DID[5:0]>1Ah).
#define IDSOPT_CPU_PST7_DID_MIN 0x8 ///< Min of   Pstate7 DID
#define IDSOPT_CPU_PST7_DID_MAX 0x30 ///< Max of   Pstate7 DID

///  Pstate7 VID
///Specifies the core voltage.
#define IDSOPT_CPU_PST7_VID_MIN 0 ///< Min of   Pstate7 VID
#define IDSOPT_CPU_PST7_VID_MAX 0xff ///< Max of   Pstate7 VID

///Core/Thread Enablement Legal Disclaimer
///Legal Disclaimer
///Downcore control
///Sets the number of cores to be used. Once this option has been used to remove any cores, a POWER CYCLE is required in order for future selections to take effect.
typedef enum {
  IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_TWO11 = 2,///<TWO (1 + 1)
  IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_TWO20 = 3,///<TWO (2 + 0)
  IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_THREE30 = 4,///<THREE (3 + 0)
  IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_FOUR22 = 5,///<FOUR (2 + 2)
  IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_FOUR40 = 6,///<FOUR (4 + 0)
  IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_SIX33 = 7,///<SIX (3 + 3)
  IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_AUTO = 0,///<Auto
} IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL;

///L1 Stream HW Prefetcher
///Option to Enable | Disable L1 Stream HW Prefetcher
typedef enum {
  IDSOPT_CMN_CPU_L1_STREAM_HW_PREFETCHER_DISABLE = 0,///<Disable
  IDSOPT_CMN_CPU_L1_STREAM_HW_PREFETCHER_ENABLE = 1,///<Enable
  IDSOPT_CMN_CPU_L1_STREAM_HW_PREFETCHER_AUTO = 3,///<Auto
} IDSOPT_CMN_CPU_L1_STREAM_HW_PREFETCHER;

///L2 Stream HW Prefetcher
///Option to Enable | Disable L2 Stream HW Prefetcher
typedef enum {
  IDSOPT_CMN_CPU_L2_STREAM_HW_PREFETCHER_DISABLE = 0,///<Disable
  IDSOPT_CMN_CPU_L2_STREAM_HW_PREFETCHER_ENABLE = 1,///<Enable
  IDSOPT_CMN_CPU_L2_STREAM_HW_PREFETCHER_AUTO = 3,///<Auto
} IDSOPT_CMN_CPU_L2_STREAM_HW_PREFETCHER;

///DRAM scrub time
///Provide a value that is the number of hours to scrub memory.
typedef enum {
  IDSOPT_DF_CMN_DRAM_SCRUB_TIME_DISABLED = 0,///<Disabled
  IDSOPT_DF_CMN_DRAM_SCRUB_TIME_1HOUR = 1,///<1 hour
  IDSOPT_DF_CMN_DRAM_SCRUB_TIME_4HOURS = 2,///<4 hours
  IDSOPT_DF_CMN_DRAM_SCRUB_TIME_8HOURS = 3,///<8 hours
  IDSOPT_DF_CMN_DRAM_SCRUB_TIME_16HOURS = 4,///<16 hours
  IDSOPT_DF_CMN_DRAM_SCRUB_TIME_24HOURS = 5,///<24 hours
  IDSOPT_DF_CMN_DRAM_SCRUB_TIME_48HOURS = 6,///<48 hours
  IDSOPT_DF_CMN_DRAM_SCRUB_TIME_AUTO = 7,///<Auto
} IDSOPT_DF_CMN_DRAM_SCRUB_TIME;

///Redirect scrubber control
///Control DF::RedirScrubCtrl[EnRedirScrub]
typedef enum {
  IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_DISABLED = 0,///<Disabled
  IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_ENABLED = 1,///<Enabled
  IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_REDIR_SCRUB_CTRL;

///Disable DF sync flood propagation
///Control DF::PIEConfig[DisSyncFloodProp]
typedef enum {
  IDSOPT_DF_CMN_SYNC_FLOOD_PROP_SYNCFLOODDISABLED = 0,///<Sync flood disabled
  IDSOPT_DF_CMN_SYNC_FLOOD_PROP_SYNCFLOODENABLED = 1,///<Sync flood enabled
  IDSOPT_DF_CMN_SYNC_FLOOD_PROP_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_SYNC_FLOOD_PROP;

///Freeze DF module queues on error
///Controls DF::PIEConfig[DisImmSyncFloodOnFatalError]\nDisabling this option sets DF:PIEConfig[DisImmSyncFloodOnFatalError]
typedef enum {
  IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_DISABLED = 0,///<Disabled
  IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_ENABLED = 1,///<Enabled
  IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR;

///GMI encryption control
///Control GMI link encryption
typedef enum {
  IDSOPT_DF_CMN_GMI_ENCRYPTION_DISABLED = 0,///<Disabled
  IDSOPT_DF_CMN_GMI_ENCRYPTION_ENABLED = 1,///<Enabled
  IDSOPT_DF_CMN_GMI_ENCRYPTION_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_GMI_ENCRYPTION;

///xGMI encryption control
///Control xGMI link encryption
typedef enum {
  IDSOPT_DF_CMN_X_GMI_ENCRYPTION_DISABLED = 0,///<Disabled
  IDSOPT_DF_CMN_X_GMI_ENCRYPTION_ENABLED = 1,///<Enabled
  IDSOPT_DF_CMN_X_GMI_ENCRYPTION_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_X_GMI_ENCRYPTION;

///CC6 memory region encryption
///Control whether or not the CC6 save/restore memory is encrypted
typedef enum {
  IDSOPT_DF_CMN_CC6_MEM_ENCRYPTION_DISABLED = 0,///<Disabled
  IDSOPT_DF_CMN_CC6_MEM_ENCRYPTION_ENABLED = 1,///<Enabled
  IDSOPT_DF_CMN_CC6_MEM_ENCRYPTION_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_CC6_MEM_ENCRYPTION;

///Location of private memory regions
///Controls whether or not the private memory regions (PSP, SMU and CC6) are at the top of DRAM or distributed. Note that distributed requires memory on all dies. Note that it will always be at the top of DRAM if some dies don't have memory regardless of this option's setting.
typedef enum {
  IDSOPT_DF_CMN_CC6_ALLOCATION_SCHEME_DISTRIBUTED = 0,///<Distributed
  IDSOPT_DF_CMN_CC6_ALLOCATION_SCHEME_CONSOLIDATED = 1,///<Consolidated
  IDSOPT_DF_CMN_CC6_ALLOCATION_SCHEME_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_CC6_ALLOCATION_SCHEME;

///System probe filter
///Controls whether or not the probe filter is enabled. Has no effect on parts where the probe filter is fuse disabled.
typedef enum {
  IDSOPT_DF_CMN_SYS_PROBE_FILTER_DISABLED = 0,///<Disabled
  IDSOPT_DF_CMN_SYS_PROBE_FILTER_ENABLED = 1,///<Enabled
  IDSOPT_DF_CMN_SYS_PROBE_FILTER_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_SYS_PROBE_FILTER;

///Memory interleaving
///Controls fabric level memory interleaving (AUTO, none, channel, die, socket). Note that channel, die, and socket has requirements on memory populations and it will be ignored if the memory doesn't support the selected option.
typedef enum {
  IDSOPT_DF_CMN_MEM_INTLV_NONE = 0,///<None
  IDSOPT_DF_CMN_MEM_INTLV_CHANNEL = 1,///<Channel
  IDSOPT_DF_CMN_MEM_INTLV_DIE = 2,///<Die
  IDSOPT_DF_CMN_MEM_INTLV_SOCKET = 3,///<Socket
  IDSOPT_DF_CMN_MEM_INTLV_AUTO = 7,///<Auto
} IDSOPT_DF_CMN_MEM_INTLV;

///Memory interleaving size
///Controls the memory interleaving size. The valid values are AUTO, 256 bytes, 512 bytes, 1 Kbytes or 2Kbytes. This determines the starting address of the interleave (bit 8, 9, 10 or 11).
typedef enum {
  IDSOPT_DF_CMN_MEM_INTLV_SIZE_256BYTES = 0,///<256 Bytes
  IDSOPT_DF_CMN_MEM_INTLV_SIZE_512BYTES = 1,///<512 Bytes
  IDSOPT_DF_CMN_MEM_INTLV_SIZE_1KB = 2,///<1 KB
  IDSOPT_DF_CMN_MEM_INTLV_SIZE_2KB = 3,///<2 KB
  IDSOPT_DF_CMN_MEM_INTLV_SIZE_AUTO = 7,///<Auto
} IDSOPT_DF_CMN_MEM_INTLV_SIZE;

///Channel interleaving hash
///Controls whether or not the address bits are hashed during channel interleave mode. This field should not be used unless the interleaving is set to channel and the interleaving size is 256 or 512 bytes.
typedef enum {
  IDSOPT_DF_CMN_CHNL_INTLV_HASH_DISABLED = 0,///<Disabled
  IDSOPT_DF_CMN_CHNL_INTLV_HASH_ENABLED = 1,///<Enabled
  IDSOPT_DF_CMN_CHNL_INTLV_HASH_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_CHNL_INTLV_HASH;

///Memory Clear
///When this feature is disabled, BIOS does not implement MemClear after memory training (only if non-ECC DIMMs are used).
typedef enum {
  IDSOPT_DF_CMN_MEM_CLEAR_ENABLED = 0,///<Enabled
  IDSOPT_DF_CMN_MEM_CLEAR_DISABLED = 1,///<Disabled
  IDSOPT_DF_CMN_MEM_CLEAR_AUTO = 3,///<Auto
} IDSOPT_DF_CMN_MEM_CLEAR;

///DRAM Timing Configuration Legal Disclaimer
///DRAM Timing Configuration Legal Disclaimer 1
///Overclock
///Memory Overclock Settings
typedef enum {
  IDSOPT_CMN_MEM_OVERCLOCK_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_OVERCLOCK_DDR4_ENABLED = 1,///<Enabled
} IDSOPT_CMN_MEM_OVERCLOCK_DDR4;

///Memory Clock Speed
///Set the memory clock frequency.
typedef enum {
  IDSOPT_CMN_MEM_SPEED_DDR4_667MHZ = 0x14,///<667MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_800MHZ = 0x18,///<800MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_933MHZ = 0x1C,///<933MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_1067MHZ = 0x20,///<1067MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_1200MHZ = 0x24,///<1200MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_SPEED_DDR4_1333MHZ = 0x28,///<1333MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_1467MHZ = 0x2C,///<1467MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_1600MHZ = 0x30,///<1600MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_333MHZ = 4,///<333MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_400MHZ = 6,///<400MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_533MHZ = 0xA,///<533MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_1050MHZ = 0x19,///<1050MHz
  IDSOPT_CMN_MEM_SPEED_DDR4_1066MHZ = 0x1A,///<1066MHz
} IDSOPT_CMN_MEM_SPEED_DDR4;

///Tcl
///Sets the tCL time.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0FHCLK = 0xF,///<0Fh Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_10HCLK = 0x10,///<10h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_11HCLK = 0x11,///<11h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_12HCLK = 0x12,///<12h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_13HCLK = 0x13,///<13h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_14HCLK = 0x14,///<14h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_15HCLK = 0x15,///<15h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_16HCLK = 0x16,///<16h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_17HCLK = 0x17,///<17h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_18HCLK = 0x18,///<18h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_19HCLK = 0x19,///<19h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1AHCLK = 0x1A,///<1Ah Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1BHCLK = 0x1B,///<1Bh Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1CHCLK = 0x1C,///<1Ch Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1DHCLK = 0x1D,///<1Dh Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1EHCLK = 0x1E,///<1Eh Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1FHCLK = 0x1F,///<1Fh Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_20HCLK = 0x20,///<20h Clk
  IDSOPT_CMN_MEM_TIMING_TCL_DDR4_21HCLK = 0x21,///<21h Clk
} IDSOPT_CMN_MEM_TIMING_TCL_DDR4;

///Trcdrd
///This sets the RAS# Active to CAS# read/write delay.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0FHCLK = 0xF,///<0Fh Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_10HCLK = 0x10,///<10h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_14HCLK = 0x14,///<14h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_15HCLK = 0x15,///<15h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_16HCLK = 0x16,///<16h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_17HCLK = 0x17,///<17h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_18HCLK = 0x18,///<18h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_19HCLK = 0x19,///<19h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_1AHCLK = 0x1A,///<1Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_11HCLK = 0x11,///<11h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_12HCLK = 0x12,///<12h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_13HCLK = 0x13,///<13h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_1BHCLK = 0x1B,///<1Bh Clk
} IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4;

///Trcdwr
///This sets the RAS# Active to CAS# read/write delay.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XACLK = 0xA,///<0xA Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XBCLK = 0xB,///<0xB Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XCCLK = 0xC,///<0xC Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XDCLK = 0xD,///<0xD Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XECLK = 0xE,///<0xE Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XFCLK = 0xF,///<0xF Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_10HCLK = 0x10,///<10h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_11HCLK = 0x11,///<11h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_12HCLK = 0x12,///<12h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_13HCLK = 0x13,///<13h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_14HCLK = 0x14,///<14h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_15HCLK = 0x15,///<15h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_16HCLK = 0x16,///<16h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_17HCLK = 0x17,///<17h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_18HCLK = 0x18,///<18h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_19HCLK = 0x19,///<19h Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_1AHCLK = 0x1A,///<1Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_1BHCLK = 0x1B,///<1Bh Clk
} IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4;

///Trp
///Specify the row precharge time.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0FHCLK = 0xF,///<0Fh Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_10HCLK = 0x10,///<10h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_11HCLK = 0x11,///<11h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_12HCLK = 0x12,///<12h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_13HCLK = 0x13,///<13h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_14HCLK = 0x14,///<14h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_15HCLK = 0x15,///<15h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_16HCLK = 0x16,///<16h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_17HCLK = 0x17,///<17h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_18HCLK = 0x18,///<18h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_19HCLK = 0x19,///<19h Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_1AHCLK = 0x1A,///<1Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRP_DDR4_1BHCLK = 0x1B,///<1Bh Clk
} IDSOPT_CMN_MEM_TIMING_TRP_DDR4;

///Tras
///Specify the min RAS# active time.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_15HCLK = 0x15,///<15h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_16HCLK = 0x16,///<16h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_17HCLK = 0x17,///<17h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_18HCLK = 0x18,///<18h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_19HCLK = 0x19,///<19h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1AHCLK = 0x1A,///<1Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1BHCLK = 0x1B,///<1Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1CHCLK = 0x1C,///<1Ch Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1DHCLK = 0x1D,///<1Dh Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1EHCLK = 0x1E,///<1Eh Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1FHCLK = 0x1F,///<1Fh Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_20HCLK = 0x20,///<20h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_21HCLK = 0x21,///<21h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_22HCLK = 0x22,///<22h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_23HCLK = 0x23,///<23h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_24HCLK = 0x24,///<24h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_25HCLK = 0x25,///<25h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_26HCLK = 0x26,///<26h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_27HCLK = 0x27,///<27h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_28HCLK = 0x28,///<28h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_29HCLK = 0x29,///<29h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2AHCLK = 0x2A,///<2Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2BHCLK = 0x2B,///<2Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2CHCLK = 0x2C,///<2Ch Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2DHCLK = 0x2D,///<2Dh Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2EHCLK = 0x2E,///<2Eh Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2FHCLK = 0x2F,///<2Fh Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_30HCLK = 0x30,///<30h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_31HCLK = 0x31,///<31h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_32HCLK = 0x32,///<32h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_33HCLK = 0x33,///<33h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_34HCLK = 0x34,///<34h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_35HCLK = 0x35,///<35h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_36HCLK = 0x36,///<36h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_37HCLK = 0x37,///<37h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_38HCLK = 0x38,///<38h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_39HCLK = 0x39,///<39h Clk
  IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_3AHCLK = 0x3A,///<3Ah Clk
} IDSOPT_CMN_MEM_TIMING_TRAS_DDR4;

///Trc Ctrl
///Specify Trc
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRC_CTRL_DDR4_AUTO = 0,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRC_CTRL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_TIMING_TRC_CTRL_DDR4;

///Trc
///Active to Active/Refresh Delay Time. Valid values 87h-1Dh.
#define IDSOPT_CMN_MEM_TIMING_TRC_DDR4_MIN 0x1d ///< Min of Trc
#define IDSOPT_CMN_MEM_TIMING_TRC_DDR4_MAX 0x87 ///< Max of Trc

///TrrdS
///Activate to Activate Delay Time, different bank group (tRRD_S)
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_0CHCLK = 0xC,///<0Ch Clk
} IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4;

///TrrdL
///Activate to Activate Delay Time, same bank group (tRRD_L)
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_0CHCLK = 0xC,///<0Ch Clk
} IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4;

///Tfaw Ctrl
///Specify Tfaw
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TFAW_CTRL_DDR4_AUTO = 0,///<Auto
  IDSOPT_CMN_MEM_TIMING_TFAW_CTRL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_TIMING_TFAW_CTRL_DDR4;

///Tfaw
///Four Activate Window Time. Valid values 36h-6h.
#define IDSOPT_CMN_MEM_TIMING_TFAW_DDR4_MIN 0x6 ///< Min of Tfaw
#define IDSOPT_CMN_MEM_TIMING_TFAW_DDR4_MAX 0x36 ///< Max of Tfaw

///TwtrS
///Minimum Write to Read Time, different bank group
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0EHCLK = 0xE,///<0Eh Clk
} IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4;

///TwtrL
///Minimum Write to Read Time, same bank group
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0EHCLK = 0xE,///<0Eh Clk
} IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4;

///Twr Ctrl
///Specify Twr
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TWR_CTRL_DDR4_AUTO = 0,///<Auto
  IDSOPT_CMN_MEM_TIMING_TWR_CTRL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_TIMING_TWR_CTRL_DDR4;

///Twr
///Minimum Write Recovery Time. Valid value 51h-Ah
#define IDSOPT_CMN_MEM_TIMING_TWR_DDR4_MIN 0xa ///< Min of Twr
#define IDSOPT_CMN_MEM_TIMING_TWR_DDR4_MAX 0x51 ///< Max of Twr

///Trcpage Ctrl
///Specify Trcpage
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4_AUTO = 0,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4;

///Trcpage
///SDRAM Optional Features (tMAW, MAC). Valid value 3FFh - 0h
#define IDSOPT_CMN_MEM_TIMING_TRCPAGE_DDR4_MIN 0 ///< Min of Trcpage
#define IDSOPT_CMN_MEM_TIMING_TRCPAGE_DDR4_MAX 0x3ff ///< Max of Trcpage

///TrdrdScL Ctrl
///Specify TrdrdScL
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4_AUTO = 0,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4;

///TrdrdScL
///CAS to CAS Delay Time, same bank group. Valid values Fh-1h
#define IDSOPT_CMN_MEM_TIMING_TRDRD_SC_L_DDR4_MIN 0x1 ///< Min of TrdrdScL
#define IDSOPT_CMN_MEM_TIMING_TRDRD_SC_L_DDR4_MAX 0xf ///< Max of TrdrdScL

///TwrwrScL Ctrl
///Specify TwrwrScL
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4_AUTO = 0,///<Auto
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4;

///TwrwrScL
///CAS to CAS Delay Time, same bank group. Valid values 3Fh-1h
#define IDSOPT_CMN_MEM_TIMING_TWRWR_SC_L_DDR4_MIN 0x1 ///< Min of TwrwrScL
#define IDSOPT_CMN_MEM_TIMING_TWRWR_SC_L_DDR4_MAX 0x3f ///< Max of TwrwrScL

///Trfc Ctrl
///Specify Trfc
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRFC_CTRL_DDR4_AUTO = 0,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRFC_CTRL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_TIMING_TRFC_CTRL_DDR4;

///Trfc
///Refresh Recovery Delay Time (tRFC1). Valid values 3DEh-3Ch
#define IDSOPT_CMN_MEM_TIMING_TRFC_DDR4_MIN 0x3c ///< Min of Trfc
#define IDSOPT_CMN_MEM_TIMING_TRFC_DDR4_MAX 0x3de ///< Max of Trfc

///Trfc2 Ctrl
///Specify Trfc2
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRFC2_CTRL_DDR4_AUTO = 0,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRFC2_CTRL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_TIMING_TRFC2_CTRL_DDR4;

///Trfc2
///Refresh Recovery Delay Time (tRFC2).  Valid values 3DEh-3Ch
#define IDSOPT_CMN_MEM_TIMING_TRFC2_DDR4_MIN 0x3c ///< Min of Trfc2
#define IDSOPT_CMN_MEM_TIMING_TRFC2_DDR4_MAX 0x3de ///< Max of Trfc2

///Trfc4 Ctrl
///Specify Trfc4
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRFC4_CTRL_DDR4_AUTO = 0,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRFC4_CTRL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_TIMING_TRFC4_CTRL_DDR4;

///Trfc4
///Refresh Recovery Delay Time (tRFC4). Valid values 3DEh-3Ch
#define IDSOPT_CMN_MEM_TIMING_TRFC4_DDR4_MIN 0x3c ///< Min of Trfc4
#define IDSOPT_CMN_MEM_TIMING_TRFC4_DDR4_MAX 0x3de ///< Max of Trfc4

///ProcODT
///Specifies the Processor ODT
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_HIGHIMPEDANCE = 0,///<High Impedance
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_480OHM = 1,///<480 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_240OHM = 2,///<240 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_160OHM = 3,///<160 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_120OHM = 8,///<120 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_96OHM = 9,///<96 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_80OHM = 10,///<80 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_686OHM = 11,///<68.6 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_60OHM = 0x18,///<60 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_533OHM = 0x19,///<53.3 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_48OHM = 0x1A,///<48 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_436OHM = 0x1B,///<43.6 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_40OHM = 0x38,///<40 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_369OHM = 0x39,///<36.9 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_343OHM = 0x3A,///<34.3 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_32OHM = 0x3B,///<32 ohm
  IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_30OHM = 0x3E,///<30 ohm
} IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4;

///Tcwl
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0FHCLK = 0xF,///<0Fh Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_10HCLK = 0x10,///<10h Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_11HCLK = 0x11,///<11h Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_12HCLK = 0x12,///<12h Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_13HCLK = 0x13,///<13h Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_14HCLK = 0x14,///<14h Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_15HCLK = 0x15,///<15h Clk
  IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_16HCLK = 0x16,///<16h Clk
} IDSOPT_CMN_MEM_TIMING_TCWL_DDR4;

///Trtp
///Specifies the read CAS# to precharge time.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_5CLK = 0x5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_6CLK = 0x6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_7CLK = 0x7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0EHCLK = 0xE,///<0Eh Clk
} IDSOPT_CMN_MEM_TIMING_TRTP_DDR4;

///Trdwr
///This sets the tWRTTO time.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0FHCLK = 0xF,///<0Fh Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_10HCLK = 0x10,///<10h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_11HCLK = 0x11,///<11h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_12HCLK = 0x12,///<12h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_13HCLK = 0x13,///<13h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_14HCLK = 0x14,///<14h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_15HCLK = 0x15,///<15h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_16HCLK = 0x16,///<16h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_17HCLK = 0x17,///<17h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_18HCLK = 0x18,///<18h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_19HCLK = 0x19,///<19h Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1AHCLK = 0x1A,///<1Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1BHCLK = 0x1B,///<1Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1CLK = 1,///<1 Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1CHCLK = 0x1C,///<1Ch Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1DHCLK = 0x1D,///<1Dh Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1EHCLK = 0x1E,///<1Eh Clk
  IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1FHCLK = 0x1F,///<1Fh Clk
} IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4;

///Twrrd
///Specify the write to read delay when accessing different DIMMs.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_1CLK = 1,///<1 Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0CH = 0xC,///<0Ch
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0DH = 0xD,///<0Dh
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0EH = 0xE,///<0Eh
  IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0FH = 0xF,///<0Fh
} IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4;

///TwrwrSc
///write to write timing same DIMM same chip select.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_1CLK = 1,///<1 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0FHCLK = 0xF,///<0Fh Clk
} IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4;

///TwrwrSd
///write to write timing same DIMM same chip select.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_1CLK = 1,///<1 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0FHCLK = 0xF,///<0Fh Clk
} IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4;

///TwrwrDd
///write to write timing same DIMM same chip select.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_1CLK = 1,///<1 Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0FHCLK = 0xF,///<0Fh Clk
} IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4;

///TrdrdSc
///write to write timing same DIMM same chip select.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_1CLK = 1,///<1 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4;

///TrdrdSd
///write to write timing same DIMM same chip select.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_1CLK = 1,///<1 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0FHCLK = 0xF,///<0Fh Clk
} IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4;

///TrdrdDd
///write to write timing same DIMM same chip select.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_1CLK = 1,///<1 Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0FHCLK = 0xF,///<0Fh Clk
} IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4;

///Tcke
///Specifies the CKE minimum high and low pulse width in memory clock cycles.
typedef enum {
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1CLK = 1,///<1 Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_2CLK = 2,///<2 Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_3CLK = 3,///<3 Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_4CLK = 4,///<4 Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_5CLK = 5,///<5 Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_6CLK = 6,///<6 Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_7CLK = 7,///<7 Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_8CLK = 8,///<8 Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_9CLK = 9,///<9 Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0AHCLK = 0xA,///<0Ah Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0BHCLK = 0xB,///<0Bh Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0CHCLK = 0xC,///<0Ch Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0DHCLK = 0xD,///<0Dh Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0EHCLK = 0xE,///<0Eh Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0FHCLK = 0xF,///<0Fh Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_10HCLK = 0x10,///<10h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_11HCLK = 0x11,///<11h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_12HCLK = 0x12,///<12h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_13HCLK = 0x13,///<13h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_14HCLK = 0x14,///<14h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_15HCLK = 0x15,///<15h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_16HCLK = 0x16,///<16h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_17HCLK = 0x17,///<17h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_18HCLK = 0x18,///<18h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_19HCLK = 0x19,///<19h Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1AHCLK = 0x1A,///<1Ah Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1BHCLK = 0x1B,///<1Bh Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1CHCLK = 0x1C,///<1Ch Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1DHCLK = 0x1D,///<1Dh Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1EHCLK = 0x1E,///<1Eh Clk
  IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1FHCLK = 0x1F,///<1Fh Clk
} IDSOPT_CMN_MEM_TIMING_TCKE_DDR4;

///Cmd2T
///Select between 1T and 2T mode on ADDR/CMD
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER2_T_MODE_DDR4_1T = 0,///<1T
  IDSOPT_CMN_MEM_CTRLLER2_T_MODE_DDR4_2T = 1,///<2T
  IDSOPT_CMN_MEM_CTRLLER2_T_MODE_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER2_T_MODE_DDR4;

///Gear Down Mode
typedef enum {
  IDSOPT_CMN_MEM_GEAR_DOWN_MODE_DDR4_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_GEAR_DOWN_MODE_DDR4_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_GEAR_DOWN_MODE_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_GEAR_DOWN_MODE_DDR4;

///Power Down Enable
///Enable or disable DDR power down mode
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4;

///Data Bus Configuration User Controls
///Specify the mode for drive strength to Auto or Manual
typedef enum {
  IDSOPT_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4_MANUAL = 1,///<Manual
} IDSOPT_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4;

///RttNom
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RTT_NOMDISABLE = 0,///<Rtt_Nom Disable
  IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ4 = 1,///<RZQ/4
  IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ2 = 2,///<RZQ/2
  IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ6 = 3,///<RZQ/6
  IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ1 = 4,///<RZQ/1
  IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ5 = 5,///<RZQ/5
  IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ3 = 6,///<RZQ/3
  IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ7 = 7,///<RZQ/7
  IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4;

///RttWr
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_DYNAMICODTOFF = 0,///<Dynamic ODT Off
  IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_RZQ2 = 1,///<RZQ/2
  IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_RZQ1 = 2,///<RZQ/1
  IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_HIZ = 3,///<Hi-Z
  IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_RZQ3 = 4,///<RZQ/3
  IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4;

///RttPark
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RTT_PARKDISABLE = 0,///<Rtt_PARK Disable
  IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ4 = 1,///<RZQ/4
  IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ2 = 2,///<RZQ/2
  IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ6 = 3,///<RZQ/6
  IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ1 = 4,///<RZQ/1
  IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ5 = 5,///<RZQ/5
  IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ3 = 6,///<RZQ/3
  IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ7 = 7,///<RZQ/7
  IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4;

///Data Poisoning
/// Enable/disable data poisoning: UMC_CH::EccCtrl[UcFatalEn] UMC_CH::EccCtrl[WrEccEn] Should be enabled/disabled together.
typedef enum {
  IDSOPT_CMN_MEM_DATA_POISONING_DDR4_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_DATA_POISONING_DDR4_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_DATA_POISONING_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_DATA_POISONING_DDR4;

///DRAM ECC Symbol Size
///DRAM ECC Symbol Size (x4/x8) - UMC_CH::EccCtrl[EccSymbolSize]
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4_X4 = 0,///<x4
  IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4_X8 = 1,///<x8
  IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4;

///DRAM ECC Enable
///Use this option to enable / disable DRAM ECC. Auto will set ECC to enable.
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4;

///TSME
///Transparent SME: AddrTweakEn = 1; ForceEncrEn =1; DataEncrEn = 0
typedef enum {
  IDSOPT_CMN_MEM_TSME_DDR4_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_TSME_DDR4_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_TSME_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_TSME_DDR4;

///Data Scramble
///Data scrambling: DataScrambleEn
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4_ENABLED = 0x1,///<Enabled
  IDSOPT_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4_DISABLED = 0x0,///<Disabled
  IDSOPT_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4;

///Chipselect Interleaving
///Interleave memory blocks across the DRAM chip selects for node 0.
typedef enum {
  IDSOPT_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4;

///BankGroupSwap
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4;

///BankGroupSwapAlt
typedef enum {
  IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4;

///Address Hash Bank
///Enable or disable bank address hashing
typedef enum {
  IDSOPT_CMN_MEM_ADDRESS_HASH_BANK_DDR4_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_ADDRESS_HASH_BANK_DDR4_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_ADDRESS_HASH_BANK_DDR4_AUTO = 0xFF,///<Auto
} IDSOPT_CMN_MEM_ADDRESS_HASH_BANK_DDR4;

///Address Hash CS
///Enable or disable CS address hashing
typedef enum {
  IDSOPT_CMN_MEM_ADDRESS_HASH_CS_DDR4_AUTO = 0xFF,///<Auto
  IDSOPT_CMN_MEM_ADDRESS_HASH_CS_DDR4_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_ADDRESS_HASH_CS_DDR4_DISABLED = 0,///<Disabled
} IDSOPT_CMN_MEM_ADDRESS_HASH_CS_DDR4;

///MBIST Enable
///Enable or disable Memory MBIST
typedef enum {
  IDSOPT_CMN_MEM_MBIST_EN_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_MBIST_EN_ENABLED = 1,///<Enabled
} IDSOPT_CMN_MEM_MBIST_EN;

///MBIST Test Mode
///Select MBIST Test Mode -Interface Mode (Tests Single and Multiple CS transactions and Basic Connectivity) or Data Eye Mode (Measures Voltage vs. Timing)
typedef enum {
  IDSOPT_CMN_MEM_MBIST_TESTMODE_INTERFACEMODE = 0,///<Interface Mode
  IDSOPT_CMN_MEM_MBIST_TESTMODE_DATAEYEMODE = 1,///<Data Eye Mode
} IDSOPT_CMN_MEM_MBIST_TESTMODE;

///MBIST Aggressors
///Enable or disable MBIST Aggressor test
typedef enum {
  IDSOPT_CMN_MEM_MBIST_AGGRESSORS_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_MBIST_AGGRESSORS_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_MBIST_AGGRESSORS_AUTO = 0xff,///<Auto
} IDSOPT_CMN_MEM_MBIST_AGGRESSORS;

///MBIST Per Bit Slave Die Reporting
///Reports 2D Data Eye Results in ABL Log for each DQ, Chipselect, and Channel
typedef enum {
  IDSOPT_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT_DISABLED = 0,///<Disabled
  IDSOPT_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT_ENABLED = 1,///<Enabled
  IDSOPT_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT_AUTO = 0xff,///<Auto
} IDSOPT_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT;

///System Configuration
///Warning: Select System Configuration may cause the system to hang, as some System Configuration may not be supported by your OPN.\n
typedef enum {
  IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_65WPORCONFIGURATION = 1,///<65W POR Configuration
  IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_45WPORCONFIGURATION = 2,///<45W POR Configuration
  IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_35WPORCONFIGURATION = 3,///<35W POR Configuration
  IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_AUTO = 0xf,///<Auto
} IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG;

///NBIO Internal Poison Consumption
///NBIO Internal Poison Consumption
typedef enum {
  IDSOPT_DBG_POISON_CONSUMPTION_ENABLED = 1,///<Enabled
  IDSOPT_DBG_POISON_CONSUMPTION_DISABLED = 0,///<Disabled
  IDSOPT_DBG_POISON_CONSUMPTION_AUTO = 0xF,///<Auto
} IDSOPT_DBG_POISON_CONSUMPTION;

///NBIO RAS Control
///NBIO RAS Control
typedef enum {
  IDSOPT_DBG_RAS_CONTROL_ENABLED = 1,///<Enabled
  IDSOPT_DBG_RAS_CONTROL_DISABLED = 0,///<Disabled
  IDSOPT_DBG_RAS_CONTROL_AUTO = 0xF,///<Auto
} IDSOPT_DBG_RAS_CONTROL;

///Determinism Slider
///Auto = Use default performance determinism settings\nPower\nPerformance
typedef enum {
  IDSOPT_CMN_DETERMINISM_SLIDER_AUTO = 0xF,///<Auto
  IDSOPT_CMN_DETERMINISM_SLIDER_POWER = 0,///<Power
  IDSOPT_CMN_DETERMINISM_SLIDER_PERFORMANCE = 1,///<Performance
} IDSOPT_CMN_DETERMINISM_SLIDER;

///cTDP Control
///Auto = Use the fused cTDP\nManual = User can set customized cTDP
typedef enum {
  IDSOPT_CMNC_TDP_CTL_MANUAL = 1,///<Manual
  IDSOPT_CMNC_TDP_CTL_AUTO = 0,///<Auto
} IDSOPT_CMNC_TDP_CTL;

///cTDP
///cTDP [W] 0 = Invalid value.
#define IDSOPT_CMNC_TDP_LIMIT_MIN 0 ///< Min of cTDP
#define IDSOPT_CMNC_TDP_LIMIT_MAX 0xffffffff ///< Max of cTDP

///Efficiency Optimized Mode
typedef enum {
  IDSOPT_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE_DISABLED = 0,///<Disabled
  IDSOPT_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE_ENABLED = 1,///<Enabled
  IDSOPT_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE_AUTO = 0xF,///<Auto
} IDSOPT_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE;

///PSI
///Disable PSI
typedef enum {
  IDSOPT_CMN_NBIO_PSI_DISABLE_DISABLE = 1,///<Disable
  IDSOPT_CMN_NBIO_PSI_DISABLE_AUTO = 0xf,///<Auto
} IDSOPT_CMN_NBIO_PSI_DISABLE;

///ACS Enable
typedef enum {
  IDSOPT_DBG_GNB_DBG_ACS_ENABLE_ENABLE = 1,///<Enable
  IDSOPT_DBG_GNB_DBG_ACS_ENABLE_DISABLED = 0,///<Disabled
  IDSOPT_DBG_GNB_DBG_ACS_ENABLE_AUTO = 0xF,///<Auto
} IDSOPT_DBG_GNB_DBG_ACS_ENABLE;

///PCIe ARI Support
///Enables Alternative Routing-ID Interpretation
typedef enum {
  IDSOPT_GNB_DBG_PCIE_ARI_SUPPORT_DISABLE = 0,///<Disable
  IDSOPT_GNB_DBG_PCIE_ARI_SUPPORT_ENABLE = 1,///<Enable
  IDSOPT_GNB_DBG_PCIE_ARI_SUPPORT_AUTO = 0xf,///<Auto
} IDSOPT_GNB_DBG_PCIE_ARI_SUPPORT;

///CLDO_VDDP Control
///Manual = User can set customized CLDO_VDDP voltage
typedef enum {
  IDSOPT_CMN_CLDO_VDDP_CTL_AUTO = 0,///<Auto
  IDSOPT_CMN_CLDO_VDDP_CTL_MANUAL = 1,///<Manual
} IDSOPT_CMN_CLDO_VDDP_CTL;

///CLDO_VDDP voltage
///Warning: user must manually cold reset the system so that the CLDOs get re-latched otherwise the voltage change will not take into effect.\n\nCLDO_VDDP voltage [mV] 0 = Invalid value.
#define IDSOPT_CMN_CLDOVDD_PVOLTAGE_MIN 0 ///< Min of CLDO_VDDP voltage
#define IDSOPT_CMN_CLDOVDD_PVOLTAGE_MAX 0xffffffff ///< Max of CLDO_VDDP voltage

///HD Audio Enable
typedef enum {
  IDSOPT_CMN_GNB_HD_AUDIO_EN_ENABLE = 1,///<Enable
  IDSOPT_CMN_GNB_HD_AUDIO_EN_DISABLED = 0,///<Disabled
  IDSOPT_CMN_GNB_HD_AUDIO_EN_AUTO = 0xF,///<Auto
} IDSOPT_CMN_GNB_HD_AUDIO_EN;

///Block PCIe Loopback
///Block PCIe loopback mode for hot plug slots
typedef enum {
  IDSOPT_CFG_PCIE_LOOPBACK_MODE_DISABLE = 0,///<Disable
  IDSOPT_CFG_PCIE_LOOPBACK_MODE_ENABLE = 1,///<Enable
  IDSOPT_CFG_PCIE_LOOPBACK_MODE_AUTO = 2,///<Auto
} IDSOPT_CFG_PCIE_LOOPBACK_MODE;

///CRS Delay
///CRS delay for hot plug ports
#define IDSOPT_CFG_PCIE_CRS_DELAY_MIN 0 ///< Min of CRS Delay
#define IDSOPT_CFG_PCIE_CRS_DELAY_MAX 0xffff ///< Max of CRS Delay

///CRS Limit
///CRS limit for hot plug ports
#define IDSOPT_CFG_PCIE_CRS_LIMIT_MIN 0 ///< Min of CRS Limit
#define IDSOPT_CFG_PCIE_CRS_LIMIT_MAX 0xffff ///< Max of CRS Limit

///IOMMU
///Enable/Disable IOMMU
typedef enum {
  IDSOPT_CMN_GNB_NB_IOMMU_DISABLED = 0,///<Disabled
  IDSOPT_CMN_GNB_NB_IOMMU_ENABLED = 1,///<Enabled
  IDSOPT_CMN_GNB_NB_IOMMU_AUTO = 0xf,///<Auto
} IDSOPT_CMN_GNB_NB_IOMMU;

///Concurrent Training
///Enable/Disable Concurrent Training
typedef enum {
  IDSOPT_PCIE_SYNC_RESET_FALSE = 0,///<False
  IDSOPT_PCIE_SYNC_RESET_TRUE = 1,///<True
  IDSOPT_PCIE_SYNC_RESET_AUTO = 0xf,///<Auto
} IDSOPT_PCIE_SYNC_RESET;

///Fan Control
///Auto = Use the default fan controller settings\nManual = User can set customized fan controller settings
typedef enum {
  IDSOPT_DBG_FAN_CTL_MANUAL = 1,///<Manual
  IDSOPT_DBG_FAN_CTL_AUTO = 0,///<Auto
} IDSOPT_DBG_FAN_CTL;

///Force PWM Control
///Unforce = Do not force the fan PWM\nForce = Force the fan PWM to the use specified value
typedef enum {
  IDSOPT_DBG_FORCE_PWM_CTL_FORCE = 1,///<Force
  IDSOPT_DBG_FORCE_PWM_CTL_UNFORCE = 0,///<Unforce
} IDSOPT_DBG_FORCE_PWM_CTL;

///Force PWM
///Specify the PWM to force the fan to [0-100]
#define IDSOPT_DBG_FORCE_PWM_MIN 0 ///< Min of Force PWM
#define IDSOPT_DBG_FORCE_PWM_MAX 100 ///< Max of Force PWM

///Fan Table Control
///Auto = Use the default fan table\nManual = User can set customized fan table
typedef enum {
  IDSOPT_DBG_FAN_TABLE_CTL_MANUAL = 1,///<Manual
  IDSOPT_DBG_FAN_TABLE_CTL_AUTO = 0,///<Auto
} IDSOPT_DBG_FAN_TABLE_CTL;

///Low Temperature
///Low Temperature ['C]
#define IDSOPT_DBG_FAN_TABLE_TEMP_LOW_MIN 0 ///< Min of Low Temperature
#define IDSOPT_DBG_FAN_TABLE_TEMP_LOW_MAX 0xff ///< Max of Low Temperature

///Medium Temperature
///Medium Temperature ['C]
#define IDSOPT_DBG_FAN_TABLE_TEMP_MED_MIN 0 ///< Min of Medium Temperature
#define IDSOPT_DBG_FAN_TABLE_TEMP_MED_MAX 0xff ///< Max of Medium Temperature

///High Temperature
///High Temperature ['C]
#define IDSOPT_DBG_FAN_TABLE_TEMP_HIGH_MIN 0 ///< Min of High Temperature
#define IDSOPT_DBG_FAN_TABLE_TEMP_HIGH_MAX 0xff ///< Max of High Temperature

///Critical Temperature
///Critical Temperature ['C]
#define IDSOPT_DBG_FAN_TABLE_TEMP_CRITICAL_MIN 0 ///< Min of Critical Temperature
#define IDSOPT_DBG_FAN_TABLE_TEMP_CRITICAL_MAX 0xff ///< Max of Critical Temperature

///Low Pwm
///Low Pwm [0-100]
#define IDSOPT_DBG_FAN_TABLE_T_PWM_LOW_MIN 0 ///< Min of Low Pwm
#define IDSOPT_DBG_FAN_TABLE_T_PWM_LOW_MAX 100 ///< Max of Low Pwm

///Medium Pwm
///Medium Pwm [0-100]
#define IDSOPT_DBG_FAN_TABLE_PWM_MED_MIN 0 ///< Min of Medium Pwm
#define IDSOPT_DBG_FAN_TABLE_PWM_MED_MAX 100 ///< Max of Medium Pwm

///High Pwm
///High Pwm [0-100]
#define IDSOPT_DBG_FAN_TABLE_PWM_HIGH_MIN 0 ///< Min of High Pwm
#define IDSOPT_DBG_FAN_TABLE_PWM_HIGH_MAX 100 ///< Max of High Pwm

///Temperature Hysteresis
///Temperature Hysteresis ['C]
#define IDSOPT_DBG_FAN_TABLE_HYST_MIN 0 ///< Min of Temperature Hysteresis
#define IDSOPT_DBG_FAN_TABLE_HYST_MAX 0xff ///< Max of Temperature Hysteresis

///Pwm Frequency
///0 = 25kHz\n1 = 100Hz
typedef enum {
  IDSOPT_DBG_FAN_TABLE_PWM_FREQ_100HZ = 1,///<100Hz
  IDSOPT_DBG_FAN_TABLE_PWM_FREQ_25KHZ = 0,///<25kHz
} IDSOPT_DBG_FAN_TABLE_PWM_FREQ;

///Fan Polarity
///0 = negative\n1 = positive
typedef enum {
  IDSOPT_DBG_FAN_POLARITY_NEGATIVE = 0,///<Negative
  IDSOPT_DBG_FAN_POLARITY_POSITIVE = 1,///<Positive
} IDSOPT_DBG_FAN_POLARITY;

///Ignore sideband
///Disable sideband
typedef enum {
  IDSOPT_CMN_DISABLE_SIDEBAND_DISABLED = 0,///<Disabled
  IDSOPT_CMN_DISABLE_SIDEBAND_ENABLED = 1,///<Enabled
  IDSOPT_CMN_DISABLE_SIDEBAND_AUTO = 0xF,///<Auto
} IDSOPT_CMN_DISABLE_SIDEBAND;

///Disable L1 w/a
///Disable L1 w/a
typedef enum {
  IDSOPT_CMN_DISABLE_L1WA_DISABLED = 0,///<Disabled
  IDSOPT_CMN_DISABLE_L1WA_ENABLED = 1,///<Enabled
  IDSOPT_CMN_DISABLE_L1WA_AUTO = 0xF,///<Auto
} IDSOPT_CMN_DISABLE_L1WA;

///Disable BridgeDis
///No BridgeDis update based on sideband
typedef enum {
  IDSOPT_CMN_DISABLE_BRIDGE_DIS_DISABLED = 0,///<Disabled
  IDSOPT_CMN_DISABLE_BRIDGE_DIS_ENABLED = 1,///<Enabled
  IDSOPT_CMN_DISABLE_BRIDGE_DIS_AUTO = 0xF,///<Auto
} IDSOPT_CMN_DISABLE_BRIDGE_DIS;

///Disable irq polling
///HPC holds root port IRQ until DL_Active is set
typedef enum {
  IDSOPT_CMN_DISABLE_IRQ_POLL_DISABLED = 0,///<Disabled
  IDSOPT_CMN_DISABLE_IRQ_POLL_ENABLED = 1,///<Enabled
  IDSOPT_CMN_DISABLE_IRQ_POLL_AUTO = 0xF,///<Auto
} IDSOPT_CMN_DISABLE_IRQ_POLL;

///IRQ sets BridgeDis
///Register control of BridgeDis only follows DL_Active
typedef enum {
  IDSOPT_CMN_IRQ_SETS_BRIDGE_DIS_DISABLED = 0,///<Disabled
  IDSOPT_CMN_IRQ_SETS_BRIDGE_DIS_ENABLED = 1,///<Enabled
  IDSOPT_CMN_IRQ_SETS_BRIDGE_DIS_AUTO = 0xF,///<Auto
} IDSOPT_CMN_IRQ_SETS_BRIDGE_DIS;

///PCIE Hotplug support
typedef enum {
  IDSOPT_PCIE_HOTPLUG_SUPPORT_DISABLED = 0,///<Disabled
  IDSOPT_PCIE_HOTPLUG_SUPPORT_ENABLED = 1,///<Enabled
  IDSOPT_PCIE_HOTPLUG_SUPPORT_AUTO = 0xF,///<Auto
} IDSOPT_IDSOPT_PCIE_HOTPLUG_SUPPORT;

///SATA Controller
///Disable or enable OnChip SATA controller
typedef enum {
  IDSOPT_CMN_FCH_SATA_ENABLE_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_SATA_ENABLE_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_SATA_ENABLE_AUTO = 0xf,///<Auto
} IDSOPT_CMN_FCH_SATA_ENABLE;

///SATA Mode
///Select OnChip SATA Type
typedef enum {
  IDSOPT_CMN_FCH_SATA_CLASS_AHCI = 2,///<AHCI
  IDSOPT_CMN_FCH_SATA_CLASS_AHCIASID0X7904 = 5,///<AHCI as ID 0x7904
  IDSOPT_CMN_FCH_SATA_CLASS_AUTO = 0xf,///<Auto
  IDSOPT_CMN_FCH_SATA_CLASS_RAID = 0x1,///<RAID
} IDSOPT_CMN_FCH_SATA_CLASS;

///Sata RAS Support
///Disable or enable Sata RAS Support
typedef enum {
  IDSOPT_CMN_FCH_SATA_RAS_SUPPORT_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_SATA_RAS_SUPPORT_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_SATA_RAS_SUPPORT_AUTO = 0xf,///<Auto
} IDSOPT_CMN_FCH_SATA_RAS_SUPPORT;

///Sata Disabled AHCI Prefetch Function
///Disable or enable Sata Disabled AHCI Prefetch Function
typedef enum {
  IDSOPT_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION_AUTO = 0xf,///<Auto
} IDSOPT_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION;

///Aggresive SATA Device Sleep Port 0
typedef enum {
  IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0_DISABLED = 0,///<Disabled
  IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0_ENABLED = 1,///<Enabled
  IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0;

///DevSleep0 Port Number
///DEVSLP port 0
#define IDSOPT_DBG_FCH_SATA_DEV_SLP_PORT0_NUM_MIN 0 ///< Min of DevSleep0 Port Number
#define IDSOPT_DBG_FCH_SATA_DEV_SLP_PORT0_NUM_MAX 7 ///< Max of DevSleep0 Port Number

///Aggresive SATA Device Sleep Port 1
typedef enum {
  IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1_DISABLED = 0,///<Disabled
  IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1_ENABLED = 1,///<Enabled
  IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1;

///DevSleep1 Port Number
///DEVSLP port 1
#define IDSOPT_DBG_FCH_SATA_DEV_SLP_PORT1_NUM_MIN 0 ///< Min of DevSleep1 Port Number
#define IDSOPT_DBG_FCH_SATA_DEV_SLP_PORT1_NUM_MAX 7 ///< Max of DevSleep1 Port Number

///XHCI controller enable
///Enable or disable USB3 controller.
typedef enum {
  IDSOPT_CMN_FCH_USB_XHC_I0_ENABLE_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_USB_XHC_I0_ENABLE_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_USB_XHC_I0_ENABLE_AUTO = 0xf,///<Auto
} IDSOPT_CMN_FCH_USB_XHC_I0_ENABLE;

///XHCI Controller1 enable (Die1)
///Enable or disable USB3 controller.
typedef enum {
  IDSOPT_CMN_FCH_USB_XHC_I1_ENABLE_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_USB_XHC_I1_ENABLE_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_USB_XHC_I1_ENABLE_AUTO = 0xf,///<Auto
} IDSOPT_CMN_FCH_USB_XHC_I1_ENABLE;

///XHCI2 enable (MCM1/Die0)
///Enable or disable USB3 controller.
typedef enum {
  IDSOPT_CMN_FCH_USB_XHC_I2_ENABLE_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_USB_XHC_I2_ENABLE_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_USB_XHC_I2_ENABLE_AUTO = 0xf,///<Auto
} IDSOPT_CMN_FCH_USB_XHC_I2_ENABLE;

///XHCI3 enable (MCM1/Die1)
///Enable or disable USB3 controller.
typedef enum {
  IDSOPT_CMN_FCH_USB_XHC_I3_ENABLE_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_USB_XHC_I3_ENABLE_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_USB_XHC_I3_ENABLE_AUTO = 0xf,///<Auto
} IDSOPT_CMN_FCH_USB_XHC_I3_ENABLE;

///SD Configuration Mode
///Select SD Mode
typedef enum {
  IDSOPT_CMN_FCH_SD_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_SD_CONFIG_VER20 = 1,///<Ver2.0
  IDSOPT_CMN_FCH_SD_CONFIG_SDDUMP = 6,///<SdDump
  IDSOPT_CMN_FCH_SD_CONFIG_AUTOVERSION20LOWSPEED = 0xf,///<Auto (Version 2.0 + Low Speed)
} IDSOPT_CMN_FCH_SD_CONFIG;

///I2C 0 Enable
typedef enum {
  IDSOPT_CMN_FCH_I2_C0_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_I2_C0_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_I2_C0_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_I2_C0_CONFIG;

///I2C 1 Enable
typedef enum {
  IDSOPT_CMN_FCH_I2_C1_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_I2_C1_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_I2_C1_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_I2_C1_CONFIG;

///I2C 2 Enable
typedef enum {
  IDSOPT_CMN_FCH_I2_C2_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_I2_C2_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_I2_C2_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_I2_C2_CONFIG;

///I2C 3 Enable
typedef enum {
  IDSOPT_CMN_FCH_I2_C3_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_I2_C3_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_I2_C3_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_I2_C3_CONFIG;

///I2C 4 Enable
typedef enum {
  IDSOPT_CMN_FCH_I2_C4_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_I2_C4_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_I2_C4_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_I2_C4_CONFIG;

///I2C 5 Enable
typedef enum {
  IDSOPT_CMN_FCH_I2_C5_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_I2_C5_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_I2_C5_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_I2_C5_CONFIG;

///Uart 0 Enable
///Uart 0 has no HW FC if Uart 2 is enabled
typedef enum {
  IDSOPT_CMN_FCH_UART0_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_UART0_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_UART0_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_UART0_CONFIG;

///Uart 0 Legacy Options
typedef enum {
  IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_0X2E8 = 1,///<0x2E8
  IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_0X2F8 = 2,///<0x2F8
  IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_0X3E8 = 3,///<0x3E8
  IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_0X3F8 = 4,///<0x3F8
} IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG;

///Uart 1 Enable
///Uart 1 has no HW FC if Uart 3 is enabled
typedef enum {
  IDSOPT_CMN_FCH_UART1_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_UART1_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_UART1_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_UART1_CONFIG;

///Uart 1 Legacy Options
typedef enum {
  IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_0X2E8 = 1,///<0x2E8
  IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_0X2F8 = 2,///<0x2F8
  IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_0X3E8 = 3,///<0x3E8
  IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_0X3F8 = 4,///<0x3F8
} IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG;

///Uart 2 Enable (no HW FC)
typedef enum {
  IDSOPT_CMN_FCH_UART2_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_UART2_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_UART2_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_UART2_CONFIG;

///Uart 2 Legacy Options
typedef enum {
  IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_0X2E8 = 1,///<0x2E8
  IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_0X2F8 = 2,///<0x2F8
  IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_0X3E8 = 3,///<0x3E8
  IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_0X3F8 = 4,///<0x3F8
} IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG;

///Uart 3 Enable (no HW FC)
typedef enum {
  IDSOPT_CMN_FCH_UART3_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_UART3_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_UART3_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_UART3_CONFIG;

///Uart 3 Legacy Options
typedef enum {
  IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_0X2E8 = 1,///<0x2E8
  IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_0X2F8 = 2,///<0x2F8
  IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_0X3E8 = 3,///<0x3E8
  IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_0X3F8 = 4,///<0x3F8
} IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG;

///ESPI Enable
typedef enum {
  IDSOPT_CMN_FCH_ESPI_CONFIG_DISABLED = 0,///<Disabled
  IDSOPT_CMN_FCH_ESPI_CONFIG_ENABLED = 1,///<Enabled
  IDSOPT_CMN_FCH_ESPI_CONFIG_AUTO = 0xf,///<AUTO
} IDSOPT_CMN_FCH_ESPI_CONFIG;

///PT SATA Port Enable
typedef enum {
  IDSOPT_CMN_PT_SATA_PORT_ENABLE_DISABLED = 0,///<Disabled
  IDSOPT_CMN_PT_SATA_PORT_ENABLE_ENABLED = 1,///<Enabled
  IDSOPT_CMN_PT_SATA_PORT_ENABLE_AUTO = 0xf,///<Auto
} IDSOPT_CMN_PT_SATA_PORT_ENABLE;

///PT SATA Mode
///Select Promontory SATA Type
typedef enum {
  IDSOPT_CMN_PT_SATA_CLASS_AHCI = 0,///<AHCI
  IDSOPT_CMN_PT_SATA_CLASS_RAID = 1,///<RAID
  IDSOPT_CMN_PT_SATA_CLASS_AUTO = 0xf,///<Auto
} IDSOPT_CMN_PT_SATA_CLASS;

///PT Aggresive SATA Device Sleep Port 0
typedef enum {
  IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0;

///PT Aggresive SATA Device Sleep Port 1
typedef enum {
  IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1;

///PT XHCI GEN1
typedef enum {
  IDSOPT_DBG_PT_XHCI_GEN1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_XHCI_GEN1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_XHCI_GEN1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_XHCI_GEN1;

///PT XHCI GEN2
typedef enum {
  IDSOPT_DBG_PT_XHCI_GEN2_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_XHCI_GEN2_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_XHCI_GEN2_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_XHCI_GEN2;

///PT USB Equalization4
typedef enum {
  IDSOPT_DBG_PT_USB_EQUALIZATION4_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB_EQUALIZATION4_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB_EQUALIZATION4_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB_EQUALIZATION4;

///PT USB Redriver
typedef enum {
  IDSOPT_DBG_PT_USB_REDRIVER_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB_REDRIVER_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB_REDRIVER_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB_REDRIVER;

///PT USB31 PORT0
typedef enum {
  IDSOPT_DBG_PT_USB31_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB31_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB31_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB31_P0;

///PT USB31 PORT1
typedef enum {
  IDSOPT_DBG_PT_USB31_P1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB31_P1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB31_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB31_P1;

///PT USB30 PORT0
typedef enum {
  IDSOPT_DBG_PT_USB30_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB30_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB30_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB30_P0;

///PT USB30 PORT1
typedef enum {
  IDSOPT_DBG_PT_USB30_P1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB30_P1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB30_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB30_P1;

///PT USB30 PORT2
typedef enum {
  IDSOPT_DBG_PT_USB30_P2_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB30_P2_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB30_P2_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB30_P2;

///PT USB30 PORT3
typedef enum {
  IDSOPT_DBG_PT_USB30_P3_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB30_P3_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB30_P3_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB30_P3;

///PT USB30 PORT4
typedef enum {
  IDSOPT_DBG_PT_USB30_P4_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB30_P4_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB30_P4_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB30_P4;

///PT USB30 PORT5
typedef enum {
  IDSOPT_DBG_PT_USB30_P5_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB30_P5_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB30_P5_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB30_P5;

///PT USB20 PORT0
typedef enum {
  IDSOPT_DBG_PT_USB20_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB20_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB20_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB20_P0;

///PT USB20 PORT1
typedef enum {
  IDSOPT_DBG_PT_USB20_P1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB20_P1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB20_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB20_P1;

///PT USB20 PORT2
typedef enum {
  IDSOPT_DBG_PT_USB20_P2_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB20_P2_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB20_P2_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB20_P2;

///PT USB20 PORT3
typedef enum {
  IDSOPT_DBG_PT_USB20_P3_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB20_P3_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB20_P3_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB20_P3;

///PT USB20 PORT4
typedef enum {
  IDSOPT_DBG_PT_USB20_P4_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB20_P4_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB20_P4_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB20_P4;

///PT USB20 PORT5
typedef enum {
  IDSOPT_DBG_PT_USB20_P5_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_USB20_P5_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_USB20_P5_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_USB20_P5;

///PT USB31 PORT0
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB31_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB31_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB31_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB31_P0;

///PT USB31 PORT1
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB31_P1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB31_P1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB31_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB31_P1;

///PT USB30 PORT0
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB30_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB30_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB30_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB30_P0;

///PT USB30 PORT1
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB30_P1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB30_P1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB30_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB30_P1;

///PT USB20 PORT0
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB20_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB20_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB20_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB20_P0;

///PT USB20 PORT1
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB20_P1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB20_P1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB20_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB20_P1;

///PT USB20 PORT2
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB20_P2_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB20_P2_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB20_P2_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB20_P2;

///PT USB20 PORT3
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB20_P3_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB20_P3_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB20_P3_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB20_P3;

///PT USB20 PORT4
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB20_P4_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB20_P4_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB20_P4_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB20_P4;

///PT USB20 PORT5
typedef enum {
  IDSOPT_DBG_PT_PROM2_USB20_P5_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM2_USB20_P5_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM2_USB20_P5_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM2_USB20_P5;

///PT USB31 PORT0
typedef enum {
  IDSOPT_DBG_PT_PROM1_USB31_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM1_USB31_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM1_USB31_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM1_USB31_P0;

///PT USB31 PORT1
typedef enum {
  IDSOPT_DBG_PT_PROM1_USB31_P1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM1_USB31_P1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM1_USB31_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM1_USB31_P1;

///PT USB30 PORT0
typedef enum {
  IDSOPT_DBG_PT_PROM1_USB30_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM1_USB30_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM1_USB30_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM1_USB30_P0;

///PT USB20 PORT0
typedef enum {
  IDSOPT_DBG_PT_PROM1_USB20_P0_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM1_USB20_P0_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM1_USB20_P0_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM1_USB20_P0;

///PT USB20 PORT1
typedef enum {
  IDSOPT_DBG_PT_PROM1_USB20_P1_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM1_USB20_P1_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM1_USB20_P1_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM1_USB20_P1;

///PT USB20 PORT2
typedef enum {
  IDSOPT_DBG_PT_PROM1_USB20_P2_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM1_USB20_P2_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM1_USB20_P2_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM1_USB20_P2;

///PT USB20 PORT3
typedef enum {
  IDSOPT_DBG_PT_PROM1_USB20_P3_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM1_USB20_P3_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM1_USB20_P3_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM1_USB20_P3;

///PT USB20 PORT4
typedef enum {
  IDSOPT_DBG_PT_PROM1_USB20_P4_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM1_USB20_P4_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM1_USB20_P4_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM1_USB20_P4;

///PT USB20 PORT5
typedef enum {
  IDSOPT_DBG_PT_PROM1_USB20_P5_DISABLE = 0,///<Disable
  IDSOPT_DBG_PT_PROM1_USB20_P5_ENABLE = 1,///<Enable
  IDSOPT_DBG_PT_PROM1_USB20_P5_AUTO = 0xF,///<Auto
} IDSOPT_DBG_PT_PROM1_USB20_P5;

///NTB Enable
///Enable NTB
typedef enum {
  IDSOPT_DBG_S_P3_NTB_ENABLE_AUTO = 0,///<Auto
  IDSOPT_DBG_S_P3_NTB_ENABLE_ENABLE = 1,///<Enable
} IDSOPT_DBG_S_P3_NTB_ENABLE;

///NTB Location
typedef enum {
  IDSOPT_DBG_S_P3_NTB_LOCATION_AUTO = 0xFF,///<Auto
  IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET0DIE0 = 0,///<Socket0-Die0
  IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET0DIE1 = 1,///<Socket0-Die1
  IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET0DIE2 = 2,///<Socket0-Die2
  IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET0DIE3 = 3,///<Socket0-Die3
  IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET1DIE0 = 4,///<Socket1-Die0
  IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET1DIE1 = 5,///<Socket1-Die1
  IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET1DIE2 = 6,///<Socket1-Die2
  IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET1DIE3 = 7,///<Socket1-Die3
} IDSOPT_DBG_S_P3_NTB_LOCATION;

///NTB active on PCIeCore
///NTB enable on PCIe Core
typedef enum {
  IDSOPT_DBG_S_P3_NTB_PC_IE_CORE_AUTO = 0xf,///<Auto
  IDSOPT_DBG_S_P3_NTB_PC_IE_CORE_CORE0 = 0,///<Core0
  IDSOPT_DBG_S_P3_NTB_PC_IE_CORE_CORE1 = 16,///<Core1
} IDSOPT_DBG_S_P3_NTB_PC_IE_CORE;

///NTB Mode
///Select NTB Mode (Core 0, Port 0)
typedef enum {
  IDSOPT_DBG_S_P3_NTB_MODE_NTBDISABLED = 0,///<NTB Disabled
  IDSOPT_DBG_S_P3_NTB_MODE_NTBPRIMARY = 1,///<NTB Primary
  IDSOPT_DBG_S_P3_NTB_MODE_NTBSECONDARY = 2,///<NTB Secondary
  IDSOPT_DBG_S_P3_NTB_MODE_NTBRANDOM = 3,///<NTB Random
  IDSOPT_DBG_S_P3_NTB_MODE_AUTO = 0xf,///<Auto
} IDSOPT_DBG_S_P3_NTB_MODE;

///  Link Speed
///Select Link Speed for NTB Mode (Core 0, Port 0)
typedef enum {
  IDSOPT_DBG_S_P3_NTB_LINK_SPEED_MAXSPEED = 0,///<Max Speed
  IDSOPT_DBG_S_P3_NTB_LINK_SPEED_GEN1 = 1,///<Gen 1
  IDSOPT_DBG_S_P3_NTB_LINK_SPEED_GEN2 = 2,///<Gen 2
  IDSOPT_DBG_S_P3_NTB_LINK_SPEED_GEN3 = 3,///<Gen 3
  IDSOPT_DBG_S_P3_NTB_LINK_SPEED_AUTO = 0xf,///<Auto
} IDSOPT_DBG_S_P3_NTB_LINK_SPEED;


#endif //_IDS_NV_DEF_ZP_H_
