/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen - Zeppelin API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
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
/*++
Module Name:

  CcxZenZpMicrocodePatch.c
  Load microcode

Abstract:
--*/

#include <Library/UefiLib.h>
#include <AGESA.h>
#include <Library/CcxMicrocodePatchLib.h>
#include <Library/BaseCoreLogicalIdLib.h>
#include <Library/AmdIdsHookLib.h>
#include "CcxZenZpMicrocodePatch.h"
#include "IdsLib.h"
#include "Filecode.h"
#include <IdsNvIdZP.h>

#define FILECODE CCX_ZEN_CCXZENZPDXE_CCXZENZPMICROCODEPATCH_FILECODE

BOOLEAN IdsOcEnabled;

STATIC CONST UINT16 ROMDATA CcxZpMicrocodeEquivalenceTable[] =
{
  0x8012, 0x8012,
  0x8011, 0x8011,
  0x8010, 0x8010,
  0x8000, 0x8000,
  ///HYGON Silicon
  0x9002, 0x9000, //HYGON_REV_F18_DN_A2
  0x9001, 0x9000, //HYGON_REV_F18_DN_A1
  0x9000, 0x9000, //HYGON_REV_F18_DN_A0
  0x9010, 0x9010, //HYGON_REV_F18_DN_B0
  0x9011, 0x9010, //HYGON_REV_F18_DN_B1
  0x9012, 0x9010, //HYGON_REV_F18_DN_B2 
  0x9020, 0x9010, //HYGON_REV_F18_DN_C0
};

/*++

Routine Description:

  Zen - Zeppelin Microcode Patch loading

Arguments:
 *  @param[in,out] UcodePatchAddr        - The selected UcodePatch address, return 0 if not found
 *  @param[in] StdHeader                 - Config handle for library and services.
Returns:

--*/
BOOLEAN
CcxZenZpMicrocodeInit (
  IN OUT   UINT64             *UcodePatchAddr,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  SOC_LOGICAL_ID LogicalId;
  BOOLEAN        Loaded = FALSE;    
  UINT64  IdsNvValue;
  
  IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_LOAD_UCODE_CONTROL, &IdsNvValue);
  if (IdsNvValue == 0)
  {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Disable Load CPU ucode feature.\n");
      return FALSE;
  }  else  {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Enable Load CPU ucode feature.\n");
  }

  BaseGetLogicalIdOnExecutingCore (&LogicalId);

  if ((LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_LTE_B1) != 0)) {
    AsmMsrOr64 (MSR_LS_CFG2, BIT7);
  }

  Loaded = LoadMicrocodePatch ((UINT16 *) &CcxZpMicrocodeEquivalenceTable[0],
                      (UINT8) (sizeof (CcxZpMicrocodeEquivalenceTable) / sizeof (CcxZpMicrocodeEquivalenceTable[0])),
                      UcodePatchAddr,
                      StdHeader);

  if ((LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_LTE_B1) != 0)) {
    AsmWbinvd ();
    AsmMsrAnd64 (MSR_LS_CFG2, ~((UINT64) BIT7));
  }

  // OpCache is controlled through 0xC001_100B[0] only with Rev B1 and ucode patch >= 0x08001118
  if ((LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_B1) != 0)) {
    if (AsmReadMsr64 (0x0000008B) >= 0x08001118) {
      IdsOcEnabled = TRUE;

      IDS_SKIP_HOOK (IDS_HOOK_CCX_SKIP_OC_ENABLE, NULL, NULL) {
        IdsOcEnabled = FALSE;
      }

      if (IdsOcEnabled) {
        // PLAT-16684
        AsmMsrOr64 (0xC001100B, BIT0);
      } else {
        AsmMsrAnd64 (0xC001100B, ~((UINT64) BIT0));
      }
    }
  }
  
  return Loaded;
}
