/**
 * @file
 *
 * SMU Features enablement
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 *
 */
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

#ifndef _SMU_FEATURES_ENABLE_H_
#define _SMU_FEATURES_ENABLE_H_

#include <HygonSmuBiosIf.h>

#define SMU_FEATURES_ENABLE_DEFAULT (1 << FEATURE_CCLK_CONTROLLER_BIT) | \
                                    (1 << FEATURE_FAN_CONTROLLER_BIT) | \
                                    (1 << FEATURE_DATA_CALCULATION_BIT) | \
                                    (1 << FEATURE_PPT_BIT) | \
                                    (1 << FEATURE_TDC_BIT) | \
                                    (1 << FEATURE_THERMAL_BIT) | \
                                    (1 << FEATURE_FIT_BIT) | \
                                    (0 << FEATURE_QOS_BIT) | \
                                    (0 << FEATURE_DF_CSTATES_BIT) | \
                                    (1 << FEATURE_PROCHOT_BIT) | \
                                    (0 << FEATURE_MCM_DATA_TRANSFER_BIT) | \
                                    (0 << FEATURE_DLWM_BIT) | \
                                    (1 << FEATURE_PC6_BIT) | \
                                    (1 << FEATURE_CSTATE_BOOST_BIT) | \
                                    (1 << FEATURE_VOLTAGE_CONTROLLER_BIT) | \
                                    (1 << FEATURE_HOT_PLUG_BIT) | \
                                    (0 << FEATURE_CC1_BIT) | \
                                    (0 << FEATURE_CC6_BIT)

#define SL2_SMU_FEATURES_ENABLE_DEFAULT (1 << FEATURE_CCLK_CONTROLLER_BIT) | \
                                        (1 << FEATURE_FAN_CONTROLLER_BIT) | \
                                        (1 << FEATURE_DATA_CALCULATION_BIT) | \
                                        (1 << FEATURE_PPT_BIT) | \
                                        (1 << FEATURE_TDC_BIT) | \
                                        (1 << FEATURE_THERMAL_BIT) | \
                                        (0 << FEATURE_FIT_BIT) | \
                                        (0 << FEATURE_QOS_BIT) | \
                                        (0 << FEATURE_DF_CSTATES_BIT) | \
                                        (1 << FEATURE_PROCHOT_BIT) | \
                                        (1 << FEATURE_MCM_DATA_TRANSFER_BIT) | \
                                        (1 << FEATURE_DLWM_BIT) | \
                                        (0 << FEATURE_PC6_BIT) | \
                                        (1 << FEATURE_CSTATE_BOOST_BIT) | \
                                        (1 << FEATURE_VOLTAGE_CONTROLLER_BIT) | \
                                        (1 << FEATURE_HOT_PLUG_BIT) | \
                                        (0 << FEATURE_CC1_BIT) | \
                                        (0 << FEATURE_CC6_BIT)


#define SL2R2_SMU_FEATURES_ENABLE_DEFAULT (1 << FEATURE_CCLK_CONTROLLER_BIT) | \
                                        (1 << FEATURE_FAN_CONTROLLER_BIT) | \
                                        (1 << FEATURE_DATA_CALCULATION_BIT) | \
                                        (1 << FEATURE_PPT_BIT) | \
                                        (1 << FEATURE_TDC_BIT) | \
                                        (1 << FEATURE_THERMAL_BIT) | \
                                        (1 << FEATURE_FIT_BIT) | \
                                        (0 << FEATURE_QOS_BIT) | \
                                        (0 << FEATURE_DF_CSTATES_BIT) | \
                                        (1 << FEATURE_PROCHOT_BIT) | \
                                        (1 << FEATURE_MCM_DATA_TRANSFER_BIT) | \
                                        (0 << FEATURE_DLWM_BIT) | \
                                        (0 << FEATURE_PC6_BIT) | \
                                        (1 << FEATURE_CSTATE_BOOST_BIT) | \
                                        (1 << FEATURE_VOLTAGE_CONTROLLER_BIT) | \
                                        (1 << FEATURE_HOT_PLUG_BIT) | \
                                        (0 << FEATURE_CC1_BIT) | \
                                        (0 << FEATURE_CC6_BIT)
#endif
