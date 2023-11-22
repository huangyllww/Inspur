/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fabric PIE power management initialization.
 *
 * This funtion initializes fabric power management features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Fabric
 *
 */
/*
 ****************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Porting.h>
#include <HYGON.h>
#include <Library/IdsLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersST.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/HygonPspHpobLib.h>
#include <CcxRegistersDm.h>
#include <Filecode.h>
#include "FabricPiePwrMgmt.h"

#define FILECODE  FABRIC_HYEX_FABRICDXE_FABRICPIEPWRMGMT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/

/**
 *
 *  FabricPiePwrMgmtInit
 *
 *  Description:
 *    This funtion initializes fabric power management features.
 *
 */
VOID
FabricPiePwrMgmtInit (
  HYGON_SOC_LOGICAL_ID_PROTOCOL  *SocLogicalIdProtocol
  )
{
  UINTN                         i;
  UINTN                         j;
  UINTN                         SocketsCount;
  UINTN                         CddsCount;
  UINTN                         CddsPresent;
  BOOLEAN                       NvDimmIsPresentInSystem;
  HPOB_HEADER                   *Hpob;
  HPOBLIB_INFO                  *HpobInfo;
  CSTATE_CTRL_REGISTER          CstateCtrl;
  PSTATE_S3D3_CTRL_REGISTER     PstateS3D3Ctrl;
  MULTIDIE_HYST_CTRL_REGISTER   MultidieHystCtrl;
  HPOB_MEM_NVDIMM_INFO_STRUCT_HYEX   *HpobNvDimmInfo;
  LOAD_STEP_HYST_CTRL_REGISTER      LoadStepHystCtrl;

  Hpob = NULL;
  if (HygonPspGetHpobInfo (&HpobInfo) == EFI_SUCCESS) {
    if (HpobInfo->Supported) {
      Hpob = (HPOB_HEADER *)HpobInfo->HpobAddr;
    }
  }

  NvDimmIsPresentInSystem = FALSE;
  if (Hpob != NULL) {
    if (HygonPspGetHpobEntryInstance (
          HPOB_GROUP_MEM,
          HPOB_MEM_NVDIMM_INFO_TYPE,
          0,
          FALSE,
          (HPOB_TYPE_HEADER **) &HpobNvDimmInfo
          ) == EFI_SUCCESS) {
       NvDimmIsPresentInSystem = HpobNvDimmInfo->NvdimmPresentInSystem;
    }
  }

  SocketsCount = FabricTopologyGetNumberOfSocketPresent ();
  for (i = 0; i < SocketsCount; i++) {
    CddsCount   = FabricTopologyGetNumberOfCddsOnSocket (i);
    CddsPresent = FabricTopologyGetCddsPresentOnSocket (i);
    for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
      if (!IS_CDD_PRESENT (j, CddsPresent)) {
        continue;
      }

      IDS_HDT_CONSOLE (CPU_TRACE, "    Socket %d CDD %d PIE power management\n", i, j);
      CstateCtrl.Value = CddFabricRegisterAccRead (i, j, CSTATE_CTRL_FUNC, CSTATE_CTRL_REG, FABRIC_REG_ACC_BC);
      CstateCtrl.Field.DfCstateDisable = (PcdGet8 (PcdHygonDfCStateMode) == 0) ? 1 : 0;
      // If NVDIMMs are present in the system, act as PcdHygonDfCStateMode is 0
      if (NvDimmIsPresentInSystem) {
        CstateCtrl.Field.DfCstateDisable = 1;
      }

      CstateCtrl.Field.DfCstateClkPwrDnEn  = 1;
      CstateCtrl.Field.DfCstateSelfRefrEn  = 1;
      CstateCtrl.Field.DfCstateHmiPwrDnEn  = (CddsCount > 1) ? 1 : 0;
      CstateCtrl.Field.DfCstateXhmiPwrDnEn = (SocketsCount > 1) ? 1 : 0;
      IDS_HOOK (IDS_HOOK_PIE_PWR_MGMT_INIT, NULL, (VOID *)&CstateCtrl);
      CddFabricRegisterAccWrite (i, j, CSTATE_CTRL_FUNC, CSTATE_CTRL_REG, FABRIC_REG_ACC_BC, CstateCtrl.Value, TRUE);

      LoadStepHystCtrl.Value = CddFabricRegisterAccRead (
                                 i,
                                 j,
                                 LOAD_STEP_HYST_CTRL_FUNC,
                                 LOAD_STEP_HYST_CTRL_REG,
                                 FABRIC_REG_ACC_BC
                                 );
      LoadStepHystCtrl.Field.ClkLoadStepHyst = 0x20;
      LoadStepHystCtrl.Field.ClkLoadStepHystScale = 3;
      LoadStepHystCtrl.Field.HmiLoadStepHyst = (CddsCount > 1) ? 0x20 : 0;
      LoadStepHystCtrl.Field.HmiLoadStepHystScale  = (CddsCount > 1) ? 3 : 0;
      LoadStepHystCtrl.Field.PickLoadStepHyst      = 0x20;
      LoadStepHystCtrl.Field.PickLoadStepHystScale = 3;
      LoadStepHystCtrl.Field.PickLoadStepThrottle1 = 8;
      IDS_HOOK (IDS_HOOK_PIE_PWR_MGMT_INIT2, NULL, (VOID *)&LoadStepHystCtrl);
      CddFabricRegisterAccWrite (
        i,
        j,
        LOAD_STEP_HYST_CTRL_FUNC,
        LOAD_STEP_HYST_CTRL_REG,
        FABRIC_REG_ACC_BC,
        LoadStepHystCtrl.Value,
        TRUE
        );

      PstateS3D3Ctrl.Value = CddFabricRegisterAccRead (
                               i,
                               j,
                               PSTATE_S3D3_CTRL_FUNC,
                               PSTATE_S3D3_CTRL_REG,
                               FABRIC_REG_ACC_BC
                               );
      PstateS3D3Ctrl.Field.DfPstateChgDisableClkChg    = 1;
      PstateS3D3Ctrl.Field.DfPstateChgDisableQuiesce   = 1;
      PstateS3D3Ctrl.Field.DfPstateChgDisableMstateChg = 1;
      PstateS3D3Ctrl.Field.DfPstateChgDisableHmiChg    = 1;
      PstateS3D3Ctrl.Field.DfPstateChgDisableXhmiChg   = ((SocketsCount == 1) || NvDimmIsPresentInSystem) ? 1 : 0;
      PstateS3D3Ctrl.Field.NvdimmSelfRefrEn = NvDimmIsPresentInSystem ? 1 : 0;
      IDS_HOOK (IDS_HOOK_PIE_PWR_MGMT_INIT3, NULL, (VOID *)&PstateS3D3Ctrl);
      CddFabricRegisterAccWrite (
        i,
        j,
        PSTATE_S3D3_CTRL_FUNC,
        PSTATE_S3D3_CTRL_REG,
        FABRIC_REG_ACC_BC,
        PstateS3D3Ctrl.Value,
        TRUE
        );

      if (FabricTopologyGetNumberOfSystemDies () > 1) {
        MultidieHystCtrl.Value = CddFabricRegisterAccRead (
                                   i,
                                   j,
                                   MULTIDIE_HYST_CTRL_FUNC,
                                   MULTIDIE_HYST_CTRL_REG,
                                   FABRIC_REG_ACC_BC
                                   );
        MultidieHystCtrl.Field.HmiDisconHyst  = (CddsCount > 1) ? 0x46 : 0;
        MultidieHystCtrl.Field.XhmiDisconHyst = (SocketsCount > 1) ? 0x71 : 0;
        CddFabricRegisterAccWrite (
          i,
          j,
          MULTIDIE_HYST_CTRL_FUNC,
          MULTIDIE_HYST_CTRL_REG,
          FABRIC_REG_ACC_BC,
          MultidieHystCtrl.Value,
          TRUE
          );
      }
    }
  }
}
