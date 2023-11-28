/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric PIE RAS initialization.
 *
 * This funtion initializes the RAS features of PIE.
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
#include <Library/AmdIdsHookLib.h>
#include "Filecode.h"
#include "FabricPieRasInit.h"

#define FILECODE FABRIC_ZP_FABRICZPPEI_FABRICPIERASINIT_FILECODE

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
CONST UINTN ROMDATA ZpDfGlblCtrlInstanceIds[] = {
  ZP_CS0_INSTANCE_ID,
  ZP_CS1_INSTANCE_ID,
  ZP_CCM0_INSTANCE_ID,
  ZP_CCM1_INSTANCE_ID,
  ZP_IOMS0_INSTANCE_ID,
  ZP_PIE_INSTANCE_ID,
  ZP_CAKE0_INSTANCE_ID,
  ZP_CAKE1_INSTANCE_ID,
  ZP_CAKE2_INSTANCE_ID,
  ZP_CAKE3_INSTANCE_ID,
  ZP_CAKE4_INSTANCE_ID,
  ZP_CAKE5_INSTANCE_ID
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
 *  FabricPieRasInit
 *
 *  Description:
 *    This funtion initializes the RAS features of PIE.
 *
 */
VOID
FabricPieRasInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       AMD_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                           i;
  UINTN                           j;
  UINTN                           k;
  UINT32                          WdtCfgRequest;
  UINT32                          WdtCntSelRequest;
  HWA_STS_HI_REGISTER             HwaStsHi;
  HWA_STS_LOW_REGISTER            HwaStsLow;
  HWA_MASK_HI_REGISTER            HwaMaskHi;
  HWA_MASK_LOW_REGISTER           HwaMaskLow;
  DF_GLBL_CTRL_REGISTER           DfGlblCtrl;
  FABRIC_BLK_INST_COUNT_REGISTER  FabricBlkInstCount;

  IDS_HOOK (IDS_HOOK_DF_RAS_INIT, NULL, NULL);

  WdtCfgRequest = (UINT32) PcdGet8 (PcdAmdFabricWdtCfg);
  WdtCntSelRequest = (UINT32) PcdGet8 (PcdAmdFabricWdtCntSel);

  for (i = 0; i < FabricTopologyGetNumberOfProcessorsPresent (); i++) {
    for (j = 0; j < FabricTopologyGetNumberOfDiesOnSocket (i); j++) {
      FabricBlkInstCount.Value = FabricRegisterAccRead (i, j, FABBLKINSTCOUNT_FUNC, FABBLKINSTCOUNT_REG, FABRIC_REG_ACC_BC);
      for (k = 0; k < FabricBlkInstCount.Field.BlkInstCount; k++) {

        HwaStsLow.Value = FabricRegisterAccRead (i, j, HWA_STS_LOW_FUNC, HWA_STS_LOW_REG, k);
        if (HwaStsLow.Value != 0) {
          HwaMaskLow.Value = FabricRegisterAccRead (i, j, HWA_MASK_LOW_FUNC, HWA_MASK_LOW_REG, k);
          HwaStsLow.Value &= ~HwaMaskLow.Value;
          if (HwaStsLow.Value != 0) {
            IDS_HDT_CONSOLE (TOPO_TRACE, "  DF Hardware Assert Low: Socket %d, Die %d, InstanceID %d, Value %x\n", i, j, k, HwaStsLow.Value);
            HwaStsLow.Value = 0;
          }
          FabricRegisterAccWrite (i, j, HWA_STS_LOW_FUNC, HWA_STS_LOW_REG, k, HwaStsLow.Value, FALSE);
        }

        HwaStsHi.Value = FabricRegisterAccRead (i, j, HWA_STS_HI_FUNC, HWA_STS_HI_REG, k);
        if (HwaStsHi.Value != 0) {
          HwaMaskHi.Value = FabricRegisterAccRead (i, j, HWA_MASK_HI_FUNC, HWA_MASK_HI_REG, k);
          HwaStsHi.Value &= ~HwaMaskHi.Value;
          if (HwaStsHi.Value != 0) {
            IDS_HDT_CONSOLE (TOPO_TRACE, "  DF Hardware Assert High: Socket %d, Die %d, InstanceID %d, Value %x\n", i, j, k, HwaStsHi.Value);
            HwaStsHi.Value = 0;
          }
          FabricRegisterAccWrite (i, j, HWA_STS_HI_FUNC, HWA_STS_HI_REG, k, HwaStsHi.Value, FALSE);
        }
      }

      for (k = 0; k < (sizeof (ZpDfGlblCtrlInstanceIds) / sizeof (ZpDfGlblCtrlInstanceIds[0])); k++) {
        DfGlblCtrl.Value = FabricRegisterAccRead (i, j, DF_GLBL_CTRL_FUNC, DF_GLBL_CTRL_REG, ZpDfGlblCtrlInstanceIds[k]);
        if (WdtCfgRequest <= 3) {
          DfGlblCtrl.Field.WDTBaseSel = WdtCfgRequest;
        }
        if (WdtCntSelRequest <= 7) {
          DfGlblCtrl.Field.PIEWDTCntSel = WdtCntSelRequest;
          DfGlblCtrl.Field.IOMWDTCntSel = WdtCntSelRequest;
          DfGlblCtrl.Field.CCMWDTCntSel = WdtCntSelRequest;
        }
        DfGlblCtrl.Field.DisImmSyncFloodOnFatalErr = 0;
        IDS_HOOK (IDS_HOOK_DF_RAS_INIT2, NULL, (VOID *) &DfGlblCtrl);
        FabricRegisterAccWrite (i, j, DF_GLBL_CTRL_FUNC, DF_GLBL_CTRL_REG, ZpDfGlblCtrlInstanceIds[k], DfGlblCtrl.Value, FALSE);
      }
    }
  }
}

