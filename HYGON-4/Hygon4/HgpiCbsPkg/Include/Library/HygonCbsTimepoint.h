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
 */

#ifndef _CBS_TIMEPOINT_H_
#define _CBS_TIMEPOINT_H_

typedef enum {
  // P E I    P H A S E
  CBS_INITRESET_BEFORE = 1,
  CBS_INITRESET_AFTER,
  CBS_INITEARLY_BEFORE,
  CBS_INITEARLY_AFTER,
  CBS_INITPOST_BEFORE,
  CBS_INITPOST_AFTER,
  CBS_S3LATERESTORE_BEFORE,
  CBS_S3LATERESTORE_AFTER,
  GNB_CALLOUT_PCIE_PLATFORM_CONFIG,
  GNB_CALLOUT_PCIE_PHY_CONFIG,
  GNB_CALLOUT_PCIE_EARLY_PORT_CONFIG,
  GNB_CALLOUT_GNB_PACKAGE_POWER_CONFIG,
  CBS_FCH_PEI_CALLOUT,
  GNB_CALLOUT_GNB_BEFORE_EARLY_INIT,
  GNB_CALLOUT_SMUBIOSTABLE_OVERRIDE,
  CBS_HGPI_GET_IDS_INIT_DATA,
  GNB_CALLOUT_BUILDOPTIONS_OVERRIDE,
  GNB_CALLOUT_SMUFEATURES_MASK_OVERRIDE,

  // D X E    P H A S E
  CBS_INITENV_BEFORE,
  CBS_INITENV_AFTER,
  CBS_INITMID_BEFORE,
  CBS_INITMID_AFTER,
  CBS_INITLATE_BEFORE,
  CBS_INITLATE_AFTER,
  GNB_CALLOUT_PPFUSE_OVERRIDE,
  GNB_CALLOUT_INTEGRATED_TABLE,
  GNB_CALLOUT_NB_POWER_GATE,
  GNB_CALLOUT_PCIE_POWER_GATE,
  GNB_CALLOUT_GMM_REGISTER_OVERRIDE,
  GNB_CALLOUT_GNB_PCIE_AER_CONFIG,
  CBS_FCH_DXE_CALLOUT,
  CBS_SMU_PORT80_LOGGING_BUFFER,
  GNB_CALLOUT_BEFORE_TXPRESET_LOADING
} CBS_TIMEPOINT;

#endif // _CBS_TIMEPOINT_H_
