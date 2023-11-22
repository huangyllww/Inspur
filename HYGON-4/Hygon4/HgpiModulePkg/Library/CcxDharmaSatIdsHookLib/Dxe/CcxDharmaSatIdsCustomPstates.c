/* $NoKeywords:$ */

/**
 * @file
 *
 * Implement Custom Core Pstates
 *
 * Contains code that Custom Core Pstates
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Library
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

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * HYGON processors and its use is subject to the terms and conditions of the
 * HYGON Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an HYGON processor outside of
 * the applicable HYGON product specifications will void any HYGON warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and HYGON disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of HYGON processors.
 *
 ******************************************************************************
 */

#include "PiDxe.h"
#include <Library/BaseLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CcxRolesLib.h>
#include <Library/CcxPstatesLib.h>
#include <Library/HygonPspHpobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CcxMpServicesLib.h>
#include <Library/CcxIdsCustomPstatesLib.h>
#include <Protocol/HygonCoreTopologyProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/HygonNbioSmuServicesProtocol.h>
#include "CcxRegistersDm.h"
#include "CcxDharmaSatIdsCustomPstates.h"
#include "IdsNvIdSAT.h"
#include "IdsNvDefSAT.h"
#include "Filecode.h"
#include <Protocol/ByoCustomPstatesProtocol.h>              // byo230831 +

#define FILECODE  LIBRARY_CCXDHARMASATIDSHOOKLIB_DXE_CCXDHARMASATIDSCUSTOMPSTATES_FILECODE

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*---------------------------------------------------------------------------------------*/

/**
 * CcxDharmaSatIdsDxeCustomPstates - routine to read Custom Pstates information from IDS
 *
 *
 * @param[in]  HookId       IDS Hook Id
 * @param[in]  *Handle      Unused
 * @param[in]  *Data        Unused
 *
 * @retval     IDS_HOOK_STATUS
 *
 */
IDS_HOOK_STATUS
CcxDharmaSatIdsDxeCustomPstates (
  IN       HOOK_ID HookId,
  IN       VOID *Handle,
  IN       VOID *Data
  )
{
  UINT8                   i;
  UINT64                  IdsNvValue;
  IDS_CUSTOM_CORE_PSTATE  CustomPstate[PS_MAX_REG - PS_MIN_REG + 1];
  BYO_CUSTOM_PSTATE_PROTOCOL  *ByoPState;                   // byo230831 +
  EFI_STATUS                  Status;                       // byo230831 +

// byo230831 + >>
  Status = gBS->LocateProtocol(&gByoCustomPStateProtocolGuid, NULL, (VOID**)&ByoPState);
  if(!EFI_ERROR(Status)){
    Status = ByoPState->UpdateTable(ByoPState, CustomPstate);
    if(!EFI_ERROR(Status)){
      CcxIdsCustomPstateSetMsrLib(CustomPstate);
      return IDS_HOOK_SUCCESS;
    }
  }
// byo230831 + <<  
  if ((!PcdGetBool (PcdOcDisable)) && (CcxIsBsp (NULL))) {
    IDS_HDT_CONSOLE (CPU_TRACE, "CcxDharmaSatIdsDxeCustomPstates Options\n");

    for (i = 0; i <= (PS_MAX_REG - PS_MIN_REG); i++) {
      CustomPstate[i].Custom = CCX_IDS_CORE_PSTATES_AUTO;
      IDS_NV_READ_SKIP (IDSNVID_CPU_PST_CUSTOM_P0 + i * 6, &IdsNvValue) {
        CustomPstate[i].Custom = (UINT8)IdsNvValue;
        if (CustomPstate[i].Custom == CCX_IDS_CORE_PSTATES_CUSTOM) {
          // Get FID
          IDS_NV_READ_SKIP (IDSNVID_CPU_PST0_FID + i * 6, &IdsNvValue) {
            CustomPstate[i].CoreFid = (UINT8)IdsNvValue;
          }
          // Get DID
          IDS_NV_READ_SKIP (IDSNVID_CPU_PST0_DID + i * 6, &IdsNvValue) {
            CustomPstate[i].CoreDid = (UINT8)IdsNvValue;
            if (CustomPstate[i].CoreDid > 0x1A) {
              CustomPstate[i].CoreDid &= 0xFE;
            }
          }
          // Get VID
          IDS_NV_READ_SKIP (IDSNVID_CPU_PST0_VID + i * 6, &IdsNvValue) {
            CustomPstate[i].CoreVid = (UINT8)IdsNvValue;
          }
        }
      }
    }

    CcxIdsCustomPstateSetMsrLib (CustomPstate);
  } // End of CcxIsBsp

  return IDS_HOOK_SUCCESS;
}
