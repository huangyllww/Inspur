/* $NoKeywords:$ */
/**
 * @file
 *
 * NbioRASControl - NBIO RAS Control
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioBasePei
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
#include <HYGON.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbHsio.h>
#include <GnbRegisters.h>
#include <Library/PeiServicesLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/GnbLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioHandleLib.h>

#define FILECODE  NBIO_NBIOBASE_HYGX_HYGONNBIOBASEPEI_NBIORASWORKAROUND_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------------------*/

/**
 * set nbif RAS leaf control
 *
 *
 *
 * @param[in]  GnbHandle             GNB Handle
 * @param[in]  RasLeafCtrlSmnAddr
 */
VOID
ConfigureRasLeafCtrl  (
  IN GNB_HANDLE                 *GnbHandle,
  IN UINT32                     RasLeafCtrlSmnAddr
  )
{
  RAS_LEAF_CTRL_STRUCT  RAS_LEAF_CTRL;

  NbioRegisterRead (GnbHandle, RAS_LEAF_CTRL_TYPE, NBIO_SPACE (GnbHandle, RasLeafCtrlSmnAddr), &RAS_LEAF_CTRL.Value, 0);
  RAS_LEAF_CTRL.Field.POISON_DET_EN = 1;
  RAS_LEAF_CTRL.Field.POISON_ERREVENT_EN = 1;
  RAS_LEAF_CTRL.Field.POISON_STALL_EN    = 1;
  RAS_LEAF_CTRL.Field.PARITY_DET_EN = 1;
  RAS_LEAF_CTRL.Field.PARITY_ERREVENT_EN = 1;
  RAS_LEAF_CTRL.Field.PARITY_STALL_EN    = 1;
  NbioRegisterWrite (GnbHandle, RAS_LEAF_CTRL_TYPE, NBIO_SPACE (GnbHandle, RasLeafCtrlSmnAddr), &RAS_LEAF_CTRL.Value, 0);
}

/*----------------------------------------------------------------------------------------*/

/**
 * set nbif SDP parity errors workaround
 *
 *
 *
 * @param[in]  GnbHandle      GNB Handle
 */
VOID
NbioNBIFParityErrorsWorkaroundST (
  IN GNB_HANDLE                 *GnbHandle
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "NbioNBIFParityErrorsWorkaroundST Enter\n");
  if ((GnbHandle->DieType == IOD_EMEI) && ((GnbHandle->RbId % 2) == 0)) {
    ConfigureRasLeafCtrl (GnbHandle, NBIF0_BIF_RAS_LEAF1_CTRL_ADDRESS_HYGX);
  }

  if (GnbHandle->DieType == IOD_DUJIANG) {
    ConfigureRasLeafCtrl (GnbHandle, NBIF2_BIF_RAS_LEAF1_CTRL_ADDRESS_HYGX);
    ConfigureRasLeafCtrl (GnbHandle, NBIF3_BIF_RAS_LEAF1_CTRL_ADDRESS_HYGX);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioNBIFParityErrorsWorkaroundST Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * set nbif poison data workaround
 *
 *
 *
 * @param[in]  GnbHandle      GNB Handle
 */
VOID
NbioNBIFPoisonDataWorkaroundST (
  IN GNB_HANDLE                 *GnbHandle
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "NbioNBIFPoisonDataWorkaroundST Enter\n");

  if ((GnbHandle->DieType == IOD_EMEI) && ((GnbHandle->RbId % 2) == 0)) {
    ConfigureRasLeafCtrl (GnbHandle, NBIF0_GDC_RAS_LEAF0_CTRL_ADDRESS_HYGX);
  }

  if (GnbHandle->DieType == IOD_DUJIANG) {
    ConfigureRasLeafCtrl (GnbHandle, NBIF2_GDC_RAS_LEAF0_CTRL_ADDRESS_HYGX);
    ConfigureRasLeafCtrl (GnbHandle, NBIF2_GDC_RAS_LEAF1_CTRL_ADDRESS_HYGX);
    ConfigureRasLeafCtrl (GnbHandle, NBIF3_GDC_RAS_LEAF0_CTRL_ADDRESS_HYGX);
    ConfigureRasLeafCtrl (GnbHandle, NBIF3_GDC_RAS_LEAF1_CTRL_ADDRESS_HYGX);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "NbioNBIFPoisonDataWorkaroundST Exit\n");
}
