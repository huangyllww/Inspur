/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fabric IO Master/Slave initialization.
 *
 * This funtion initializes the IOMS devices in the fabric.
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
#include "Porting.h"
#include "HYGON.h"
#include <Library/IdsLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersST.h>
#include <Library/FabricRegisterAccLib.h>
#include "Filecode.h"
#include "FabricIomsInit.h"

#define FILECODE  FABRIC_HYEX_FABRICPEI_FABRICIOMSINIT_FILECODE

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
CONST UINTN ROMDATA  IomsInstanceIds[] = {
  IOMS0_INSTANCE_ID,
  IOMS1_INSTANCE_ID,
  IOMS2_INSTANCE_ID,
  IOMS3_INSTANCE_ID
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
 *  FabricIomsInit
 *
 *  Description:
 *    This funtion initializes the IOMS devices in the fabric.
 *
 */
VOID
FabricIomsInit (
  IN       CONST EFI_PEI_SERVICES       **PeiServices,
  IN       HYGON_PEI_SOC_LOGICAL_ID_PPI   *SocLogicalIdPpi
  )
{
  UINTN                     i;
  UINTN                     j;
  UINTN                     k;
  UINTN                     SocketCount;
  UINTN                     LogicalDieCount;
  UINTN                     RootBridgePerLogicalDie;
  IOM_CFG_REGISTER          IomCfg;
  IOM_CTRL_REGISTER         IomCtrl;
  MASTER_REQ_CTRL_REGISTER  MasterReqCtrl;

  SocketCount = FabricTopologyGetNumberOfSocketPresent ();
  for (i = 0; i < SocketCount; i++) {
    LogicalDieCount = FabricTopologyGetNumberOfLogicalDiesOnSocket (i);

    for (j = 0; j < LogicalDieCount; j++) {
      RootBridgePerLogicalDie = FabricTopologyGetNumberOfRootBridgesOnDie (i, j);

      for (k = 0; k < RootBridgePerLogicalDie; k++) {
        IDS_HDT_CONSOLE (CPU_TRACE, "    Socket %d IoDie %d Rb %d  set IOM_CTRL_REG  \n", i, j, k);
        IomCtrl.Value = FabricRegisterAccRead (i, j, IOM_CTRL_FUNC, IOM_CTRL_REG, IomsInstanceIds[k]);
        IomCtrl.Field.FrcIomSdpVc7ToFtiVc4 = 1;
        FabricRegisterAccWrite (i, j, IOM_CTRL_FUNC, IOM_CTRL_REG, IomsInstanceIds[k], IomCtrl.Value, FALSE);

        IDS_HDT_CONSOLE (CPU_TRACE, "    Socket %d IoDie %d Rb %d  set IOM_CFG_REG  \n", i, j, k);
        IomCfg.Value = FabricRegisterAccRead (i, j, IOM_CFG_FUNC, IOM_CFG_REG, IomsInstanceIds[k]);
        IomCfg.Field.DisSrcDnOnVc2ToVc7 = 1;
        FabricRegisterAccWrite (i, j, IOM_CFG_FUNC, IOM_CFG_REG, IomsInstanceIds[k], IomCfg.Value, FALSE);

        IDS_HDT_CONSOLE (CPU_TRACE, "    Socket %d IoDie %d Rb %d  set MASTER_REQ_CTRL_REG  \n", i, j, k);
        MasterReqCtrl.Value = FabricRegisterAccRead (i, j, MASTER_REQ_CTRL_FUNC, MASTER_REQ_CTRL_REG, IomsInstanceIds[k]);
        MasterReqCtrl.Field.MstIoNpActReqLmt = 0;
        FabricRegisterAccWrite (i, j, MASTER_REQ_CTRL_FUNC, MASTER_REQ_CTRL_REG, IomsInstanceIds[k], MasterReqCtrl.Value, FALSE);
      }
    }
  }
}
