/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Fabric ready to boot tasks
 *
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
#include <HYGON.h>
#include <Filecode.h>
#include <Library/IdsLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include <FabricRegistersST.h>
#include "FabricReadyToBoot.h"
#include <Library/UefiBootServicesTableLib.h>         // byo230831 +

#define FILECODE  FABRIC_HYEX_FABRICDXE_FABRICREADYTOBOOT_FILECODE

/* -----------------------------------------------------------------------------*/

/**
 *
 *  FabricReadyToBoot
 *
 *  Description:
 *    This function disables PCI extended configuration register access
 *
 */
VOID
EFIAPI
FabricReadyToBoot (
  IN       EFI_EVENT Event,
  IN       VOID      *Context
  )
{
  UINTN                          i;
  UINTN                          j;
  UINTN                          k;
  UINTN                          CddsPresent;
  CORE_MASTER_ACC_CTRL_REGISTER  CoreMasterAccCtrl;

  gBS->CloseEvent(Event);                             // byo230831 +

  HGPI_TESTPOINT (TpDfReadyToBootEntry, NULL);

  IDS_HDT_CONSOLE (CPU_TRACE, "FabricReadyToBoot: Disable extended configuration access\n");

  for (i = 0; i < FabricTopologyGetNumberOfSocketPresent (); i++) {
    for (j = 0; j < FabricTopologyGetNumberOfLogicalDiesOnSocket (i); j++) {
      CoreMasterAccCtrl.Value = FabricRegisterAccRead (
                                  i,
                                  j,
                                  CORE_MASTER_ACC_CTRL_FUNC,
                                  CORE_MASTER_ACC_CTRL_REG,
                                  FABRIC_REG_ACC_BC
                                  );
      CoreMasterAccCtrl.Field.EnableCf8ExtCfg = 0;
      FabricRegisterAccWrite (
        i,
        j,
        CORE_MASTER_ACC_CTRL_FUNC,
        CORE_MASTER_ACC_CTRL_REG,
        FABRIC_REG_ACC_BC,
        CoreMasterAccCtrl.Value,
        TRUE
        );
    }

    CddsPresent = FabricTopologyGetCddsPresentOnSocket (i);
    for (k = 0; k < MAX_CDDS_PER_SOCKET; k++) {
      if (!IS_CDD_PRESENT (k, CddsPresent)) {
        continue;
      }

      CoreMasterAccCtrl.Value = CddFabricRegisterAccRead (
                                  i,
                                  k,
                                  CORE_MASTER_ACC_CTRL_FUNC,
                                  CORE_MASTER_ACC_CTRL_REG,
                                  FABRIC_REG_ACC_BC
                                  );
      CoreMasterAccCtrl.Field.EnableCf8ExtCfg = 0;
      CddFabricRegisterAccWrite (
        i,
        k,
        CORE_MASTER_ACC_CTRL_FUNC,
        CORE_MASTER_ACC_CTRL_REG,
        FABRIC_REG_ACC_BC,
        CoreMasterAccCtrl.Value,
        TRUE
        );
    }
  }

  HGPI_TESTPOINT (ToDfReadyToBootExit, NULL);
}
