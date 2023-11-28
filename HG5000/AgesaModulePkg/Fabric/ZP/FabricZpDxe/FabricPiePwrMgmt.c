/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric PIE power management initialization.
 *
 * This funtion initializes fabric power management features.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ****************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Porting.h>
#include <AMD.h>
#include <Library/IdsLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersZP.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/AmdPspApobLib.h>
#include <CcxRegistersZp.h>
#include <Filecode.h>
#include "FabricPiePwrMgmt.h"

#define FILECODE FABRIC_ZP_FABRICZPDXE_FABRICPIEPWRMGMT_FILECODE

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
  AMD_SOC_LOGICAL_ID_PROTOCOL  *SocLogicalIdProtocol
  )
{
  UINTN                                            i;
  UINTN                                            j;
  BOOLEAN                                          NvDimmIsPresentInSystem;
  EFI_STATUS                                       CalledStatus;
  APOB_HEADER                                      *Apob;
  APOBLIB_INFO                                     *ApobInfo;
  SOC_LOGICAL_ID                                   LogicalId;
  CSTATE_CTRL_REGISTER                             CstateCtrl;
  PSTATE_S3D3_CTRL_REGISTER                        PstateS3D3Ctrl;
  MULTIDIE_HYST_CTRL_REGISTER                      MultidieHystCtrl;
  APOB_MEM_NVDIMM_INFO_STRUCT                      *ApobNvDimmInfo;
  LOAD_STEP_HYST_CTRL_REGISTER                     LoadStepHystCtrl;

  Apob = NULL;
  if (AmdPspGetApobInfo (&ApobInfo) == EFI_SUCCESS) {
    if (ApobInfo->Supported) {
      Apob = (APOB_HEADER *) ApobInfo->ApobAddr;
    }
  }

  NvDimmIsPresentInSystem = FALSE;
  if (Apob != NULL) {
    if (Apob->Version >= 9) {
      if (AmdPspGetApobEntryInstance (APOB_MEM, APOB_MEM_NVDIMM_INFO_TYPE, 0, FALSE, (APOB_TYPE_HEADER **) &ApobNvDimmInfo) == EFI_SUCCESS) {
        NvDimmIsPresentInSystem = ApobNvDimmInfo->NvdimmPresentInSystem;
      }
    }
  }

  for (i = 0; i < FabricTopologyGetNumberOfProcessorsPresent (); i++) {
    CalledStatus = SocLogicalIdProtocol->GetLogicalIdOnGivenSocket (SocLogicalIdProtocol, i, &LogicalId);
    ASSERT (CalledStatus == EFI_SUCCESS);
    for (j = 0; j < FabricTopologyGetNumberOfDiesOnSocket (i); j++) {
      CstateCtrl.Value = FabricRegisterAccRead (i, j, CSTATE_CTRL_FUNC, CSTATE_CTRL_REG, FABRIC_REG_ACC_BC);
      CstateCtrl.Field.DfCstateDisable = (PcdGet8 (PcdAmdCStateMode) == 1) ? 0 : 1;
      // If it's ZP, Rev Ax and package is SP3, or if NVDIMMs are present in the system, act as PcdAmdCStateMode is 0
      if (NvDimmIsPresentInSystem || ((LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_Ax) != 0) && (LibAmdGetPackageType (NULL) == (1 << ZP_SOCKET_SP3)))) {
        CstateCtrl.Field.DfCstateDisable = 1;
      }
      CstateCtrl.Field.DfCstateClkPwrDnEn = 1;
      if ((FabricTopologyGetNumberOfDiesOnSocket(i) > 1) && (LogicalId.Family == AMD_FAMILY_17_ZP) && ((LogicalId.Revision & AMD_REV_F17_ZP_LT_B2) != 0)) {
        CstateCtrl.Field.DfCstateClkPwrDnEn = 0;
      }
      CstateCtrl.Field.DfCstateSelfRefrEn = 1;
      CstateCtrl.Field.DfCstateGmiPwrDnEn = FabricTopologyGetNumberOfDiesOnSocket (i) > 1 ? 1 : 0;
      CstateCtrl.Field.DfCstateXgmiPwrDnEn = FabricTopologyGetNumberOfProcessorsPresent () > 1 ? 1 : 0;
      IDS_HOOK (IDS_HOOK_PIE_PWR_MGMT_INIT, NULL, (VOID *) &CstateCtrl);
      FabricRegisterAccWrite (i, j, CSTATE_CTRL_FUNC, CSTATE_CTRL_REG, FABRIC_REG_ACC_BC, CstateCtrl.Value, TRUE);

      LoadStepHystCtrl.Value = FabricRegisterAccRead (i, j, LOAD_STEP_HYST_CTRL_FUNC, LOAD_STEP_HYST_CTRL_REG, FABRIC_REG_ACC_BC);
      LoadStepHystCtrl.Field.ClkLoadStepHyst = 0x20;
      LoadStepHystCtrl.Field.ClkLoadStepHystScale = 3;
      LoadStepHystCtrl.Field.GmiLoadStepHyst = FabricTopologyGetNumberOfDiesOnSocket (i) > 1 ? 0x20 : 0;
      LoadStepHystCtrl.Field.GmiLoadStepHystScale = FabricTopologyGetNumberOfDiesOnSocket (i) > 1 ? 3 : 0;
      LoadStepHystCtrl.Field.PickLoadStepHyst = 0x20;
      LoadStepHystCtrl.Field.PickLoadStepHystScale = 3;
      LoadStepHystCtrl.Field.PickLoadStepThrottle1 = 8;
      IDS_HOOK (IDS_HOOK_PIE_PWR_MGMT_INIT2, NULL, (VOID *) &LoadStepHystCtrl);
      FabricRegisterAccWrite (i, j, LOAD_STEP_HYST_CTRL_FUNC, LOAD_STEP_HYST_CTRL_REG, FABRIC_REG_ACC_BC, LoadStepHystCtrl.Value, TRUE);

      PstateS3D3Ctrl.Value = FabricRegisterAccRead (i, j, PSTATE_S3D3_CTRL_FUNC, PSTATE_S3D3_CTRL_REG, FABRIC_REG_ACC_BC);
      PstateS3D3Ctrl.Field.DfPstateChgDisableClkChg = 1;
      PstateS3D3Ctrl.Field.DfPstateChgDisableQuiesce = 1;
      PstateS3D3Ctrl.Field.DfPstateChgDisableMstateChg = 1;
      PstateS3D3Ctrl.Field.DfPstateChgDisableGmiChg = 1;
      PstateS3D3Ctrl.Field.DfPstateChgDisableXgmiChg = ((FabricTopologyGetNumberOfProcessorsPresent () == 1) || NvDimmIsPresentInSystem) ? 1 : 0;
      PstateS3D3Ctrl.Field.NvdimmSelfRefrEn = NvDimmIsPresentInSystem ? 1 : 0;
      IDS_HOOK (IDS_HOOK_PIE_PWR_MGMT_INIT3, NULL, (VOID *) &PstateS3D3Ctrl);
      FabricRegisterAccWrite (i, j, PSTATE_S3D3_CTRL_FUNC, PSTATE_S3D3_CTRL_REG, FABRIC_REG_ACC_BC, PstateS3D3Ctrl.Value, TRUE);

      if (FabricTopologyGetNumberOfSystemDies () > 1) {
        MultidieHystCtrl.Value = FabricRegisterAccRead (i, j, MULTIDIE_HYST_CTRL_FUNC, MULTIDIE_HYST_CTRL_REG, FABRIC_REG_ACC_BC);
        MultidieHystCtrl.Field.GmiDisconHyst = FabricTopologyGetNumberOfDiesOnSocket (i) > 1 ? 0x46 : 0;
        MultidieHystCtrl.Field.XgmiDisconHyst = FabricTopologyGetNumberOfProcessorsPresent () > 1 ? 0x71 : 0;
        FabricRegisterAccWrite (i, j, MULTIDIE_HYST_CTRL_FUNC, MULTIDIE_HYST_CTRL_REG, FABRIC_REG_ACC_BC, MultidieHystCtrl.Value, TRUE);
      }
    }
  }
}

