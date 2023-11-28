/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric Core Coherent Master initialization.
 *
 * This funtion initializes the CCM devices in the fabric.
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
#include "Porting.h"
#include "AMD.h"
#include <Library/IdsLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersZP.h>
#include <Library/FabricRegisterAccLib.h>
#include "Filecode.h"
#include "FabricCcmInit.h"

#define FILECODE FABRIC_ZP_FABRICZPPEI_FABRICCCMINIT_FILECODE

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
CONST UINTN ROMDATA ZpCcmInstanceIds[] = {
  ZP_CCM0_INSTANCE_ID,
  ZP_CCM1_INSTANCE_ID
};

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
 *  FabricCcmInit
 *
 *  Description:
 *    This funtion initializes the CCM devices in the fabric.
 *
 */
VOID
FabricCcmInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                                 i;
  UINTN                                 j;
  UINTN                                 k;
  EFI_STATUS                            CalledStatus;
  SOC_LOGICAL_ID                        LogicalId;
  CCM_CFG_REGISTER                      CcmCfg;
  CCM_CFG1_REGISTER                     CcmCfg1;
  MASTER_REQ_PRI_REGISTER               MasterReqPri;
  MASTER_REQ_CTRL_REGISTER              MasterReqCtrl;
  MASTER_LL_LP_REQ_PRI_THRESH_REGISTER  MasterLlLpReqPriThresh;

  for (i = 0; i < FabricTopologyGetNumberOfProcessorsPresent (); i++) {
    CalledStatus = SocLogicalIdPpi->GetLogicalIdOnGivenSocket (i, &LogicalId);
    ASSERT (CalledStatus == EFI_SUCCESS);
    for (j = 0; j < FabricTopologyGetNumberOfDiesOnSocket (i); j++) {
      MasterReqPri.Value = 0;
      MasterReqPri.Field.LPRdReqPri = 2;
      MasterReqPri.Field.UseSdpReqPri = 1;
      for (k = 0; k < (sizeof (ZpCcmInstanceIds) / sizeof (ZpCcmInstanceIds[0])); k++) {
        FabricRegisterAccWrite (i, j, MASTER_REQ_PRI_FUNC, MASTER_REQ_PRI_REG, ZpCcmInstanceIds[k], MasterReqPri.Value, FALSE);
      }

      MasterLlLpReqPriThresh.Value = 0;
      MasterLlLpReqPriThresh.Field.LPCumElvPriReqThr = 0x10;
      MasterLlLpReqPriThresh.Field.LPElvPriReqThr = 2;
      FabricRegisterAccWrite (i, j, MASTER_LL_LP_REQ_PRI_THRESH_FUNC, MASTER_LL_LP_REQ_PRI_THRESH_REG, FABRIC_REG_ACC_BC, MasterLlLpReqPriThresh.Value, FALSE);

      for (k = 0; k < (sizeof (ZpCcmInstanceIds) / sizeof (ZpCcmInstanceIds[0])); k++) {
        MasterReqCtrl.Value = FabricRegisterAccRead (i, j, MASTER_REQ_CTRL_FUNC, MASTER_REQ_CTRL_REG, ZpCcmInstanceIds[k]);
        MasterReqCtrl.Field.MstIoNpActReqLmt = 0;
        FabricRegisterAccWrite (i, j, MASTER_REQ_CTRL_FUNC, MASTER_REQ_CTRL_REG, ZpCcmInstanceIds[k], MasterReqCtrl.Value, FALSE);

        CcmCfg.Value = FabricRegisterAccRead (i, j, CCM_CFG_FUNC, CCM_CFG_REG, ZpCcmInstanceIds[k]);
        CcmCfg.Field.DisCcmRspDQByp = (((LogicalId.Family & AMD_FAMILY_17_ZP) != 0) && ((LogicalId.Revision & AMD_REV_F17_ZP_A0)) != 0) ? 1 : 0;
        CcmCfg.Field.DisProtErrRsp = 1;
        FabricRegisterAccWrite (i, j, CCM_CFG_FUNC, CCM_CFG_REG, ZpCcmInstanceIds[k], CcmCfg.Value, FALSE);

        CcmCfg1.Value = FabricRegisterAccRead (i, j, CCM_CFG1_FUNC, CCM_CFG1_REG, ZpCcmInstanceIds[k]);
        CcmCfg1.Field.EnExtPciGt8BTgtAbt = 1;
        FabricRegisterAccWrite (i, j, CCM_CFG1_FUNC, CCM_CFG1_REG, ZpCcmInstanceIds[k], CcmCfg1.Value, FALSE);
      }
    }
  }
}
