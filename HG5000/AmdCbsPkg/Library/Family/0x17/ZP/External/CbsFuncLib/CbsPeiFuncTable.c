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
 ******************************************************************************
 */

#include "CbsIDSCommonPei.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(IdsPeiFunTable,_Common)[];

#ifdef AMD_CBS_ZEN_ZP
#include "CBSZenFunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(ZenPeiFunTable,_ZP)[];
#endif
#ifdef AMD_CBS_DF_ZP
#include "CBSDfFunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(DfPeiFunTable,_ZP)[];
#endif
#ifdef AMD_CBS_UMC_ZP
#include "CBSUmcFunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(UmcPeiFunTable,_ZP)[];
#endif
#ifdef AMD_CBS_NBIO_ZP
#include "CBSNbioFunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(NbioPeiFunTable,_ZP)[];
#endif
#ifdef AMD_CBS_FCH_ZP
#include "CBSFchFunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(FchPeiFunTable,_ZP)[];
#endif
#ifdef AMD_CBS_PROMONTORY_ZP
#include "CBSPromontoryFunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(PromontoryPeiFunTable,_ZP)[];
#endif
#ifdef AMD_CBS_SP3_ZP
#include "CBSSp3FunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(Sp3PeiFunTable,_ZP)[];
#endif
#ifdef AMD_CBS_SP4_ZP
#include "CBSSp4FunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(Sp4PeiFunTable,_ZP)[];
#endif
#ifdef AMD_CBS_SMU_ZP
#include "CBSSmuFunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(SmuPeiFunTable,_ZP)[];
#endif
#ifdef AMD_CBS_SOC_ZP
#include "CBSSocFunPei_ZP.h"
extern PEI_CBS_FUNCTION_STRUCT CBS_FUNCTABLE_DECLARE(SocPeiFunTable,_ZP)[];
#endif

PEI_CBS_COMPONENT_STRUCT CbsCommonFuncTable[] =
{
  {0xFFFFFFFF,    AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(IdsPeiFunTable,_Common) },

#ifdef AMD_CBS_ZEN_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(ZenPeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(ZenPeiFunTable,_ZP) },
 #endif
#ifdef AMD_CBS_DF_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(DfPeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(DfPeiFunTable,_ZP) },
#endif
#ifdef AMD_CBS_UMC_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(UmcPeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(UmcPeiFunTable,_ZP) },
#endif
#ifdef AMD_CBS_NBIO_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(NbioPeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(NbioPeiFunTable,_ZP) },
#endif
#ifdef AMD_CBS_FCH_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(FchPeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(FchPeiFunTable,_ZP) },
#endif
#ifdef AMD_CBS_PROMONTORY_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(PromontoryPeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(PromontoryPeiFunTable,_ZP) },
#endif
#ifdef AMD_CBS_SP3_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(Sp3PeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(Sp3PeiFunTable,_ZP) },
#endif
#ifdef AMD_CBS_SP4_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(Sp4PeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(Sp4PeiFunTable,_ZP) },
#endif
#ifdef AMD_CBS_SMU_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SmuPeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SmuPeiFunTable,_ZP) },
#endif
#ifdef AMD_CBS_SOC_ZP
  {F17_ZP_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SocPeiFunTable,_ZP) },
  {F18_DN_RAW_ID, AMD_RESERVE_ALL, (PEI_CBS_FUNCTION_STRUCT*) CBS_FUNCTABLE_DECLARE(SocPeiFunTable,_ZP) },
#endif

  //
  // end of table.
  //
  {0, 0, NULL}
};
