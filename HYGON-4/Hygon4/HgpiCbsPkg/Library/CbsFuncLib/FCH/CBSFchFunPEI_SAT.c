/*****************************************************************************
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

#include "CBSFchFunPEI_SAT.h"

#ifdef HYGON_CBS_FCH_SAT
  PEI_CBS_FUNCTION_STRUCT  CbsFchPeiFunTableSat[] =
  {
    {
      F18_DM_RAW_ID,
      HYGON_RESERVE_ALL,
      CbsFchInitResetParamsSat,
      IDS_HOOK_FCH_INIT_RESET
    },
    {
      0,      // F18_DM_RAW_ID,
      0,      // HYGON_RESERVE_ALL,
      NULL,   // CBS_FUNC_DECLARE(FchFunc_Init_PEI,_SAT),
      0       // CBS_CALLOUT_ID
    }
  };

  VOID
  CbsFchInitResetParamsSat (
  IN      EFI_PEI_SERVICES    **PeiServices,
  IN OUT  VOID                *DataParams,
  IN HYGON_SYSTEM_CONFIGURATION *SystemConfiguration
  )
  {
    /*
    UINT32  SataEnable;

    // SataEnable: 0(disable)  1(enable)
    // Bit0~3 Socket 0 DJ0~3 SATA controller0
    // Bit4~31 Others socket DJ0~3 SATA controller0
    //
    SataEnable = PcdGet32 (PcdSataEnable);
    // SATA
    if (SystemConfiguration->CbsCmnFchSataEnable0 == 0) {
      SataEnable = SataEnable & 0xfffffffe;
    }

    PcdSet32S (PcdSataEnable, SataEnable);
    */
  }

#endif
