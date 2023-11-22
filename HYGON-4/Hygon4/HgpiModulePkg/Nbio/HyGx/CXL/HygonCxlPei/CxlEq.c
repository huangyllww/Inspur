/* $NoKeywords:$ */
/**
 * @file
 *
 * HygonCxl11Pei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: HGPI     
 * @e sub-project:  HygonCxlPei
 * @e \$Revision: 312065 $   @e \$Date: 2022-08-02 13:46:05 -0600 (Aug, 2 Tue 2022) $
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
#include <PiPei.h>
#include <Filecode.h>
#include <GnbHsio.h>
#include <Gnb.h>
#include <HygonCxlPei.h>
#include <HygonCxlRegOffset.h>
#include <HygonCxlRegStruct.h>
#include <GnbRegisters.h>
#include <FabricRegistersST.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/GnbPciLib.h>
#include <Library/GnbCxlLib.h>
#include <Library/GnbMemAccLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Ppi/NbioBaseServicesSTPpi.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Guid/GnbNbioBaseSTInfoHob.h>
#include <IndustryStandard/Cxl.h>
#include <IndustryStandard/Cxl20.h>          // byo230928 +

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define FILECODE        NBIO_CXL_HYGONCXLPEI_CXLEQ_FILECODE

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
 /*
  *  Cofigure CXL EQ before training
  *
  *
  *
  * @param[in]     
  * @param[out]    
  */
EFI_STATUS
CommonCxlEqCfgBeforeTraining (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  IDS_HDT_CONSOLE (MAIN_FLOW, "  CommonCxlEqCfgBeforeTraining Entry\n");

  IDS_HDT_CONSOLE (MAIN_FLOW, "  CommonCxlEqCfgBeforeTraining Exit\n");

  return EFI_SUCCESS;
}

 /*----------------------------------------------------------------------------------------*/
 /*
  *  Cofigure CXL EQ after training
  *
  *
  *
  * @param[in]     
  * @param[out]    
  */
EFI_STATUS
CommonCxlEqCfgAfterTraining (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  IDS_HDT_CONSOLE (MAIN_FLOW, "  CommonCxlEqCfgAfterTraining Entry\n");

  IDS_HDT_CONSOLE (MAIN_FLOW, "  CommonCxlEqCfgAfterTraining Exit\n");

  return EFI_SUCCESS;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Enable or disable CXL Read only registers writable
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this NBIO
 *             EnableFlag     TRUE : Enable;  FASLE : Disable
 *
 * @retval    
 */
 /*----------------------------------------------------------------------------------------*/

VOID
EnableCxlRoRegWritable (
  IN       GNB_HANDLE       *GnbHandle,
  IN       BOOLEAN          EnableFlag
  )
{
   CXL_MISC_CNTL0_REGISTER                     CxlMiscCtrl;
   MISC_CONTROL_1_OFF_REGISTER                 CxlMiscCtrl1;
   
   //Enable RO register RW 
   NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_CXL_MISC_CNTL0), &CxlMiscCtrl.Value, 0);
   NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_MISC_CONTROL_1_OFF), &CxlMiscCtrl1.Value, 0);
   
   if (EnableFlag) {
     IDS_HDT_CONSOLE (GNB_TRACE, "  Enable CXL RO Register Writable \n");
     CxlMiscCtrl.Field.APP_DBI_RO_WR_DISABLE = 0;   
     CxlMiscCtrl1.Field.DBI_RO_WR_EN = 1;
   } else {
     IDS_HDT_CONSOLE (GNB_TRACE, "  Disable CXL RO Register Writable \n");
     CxlMiscCtrl.Field.APP_DBI_RO_WR_DISABLE = 1;
     CxlMiscCtrl1.Field.DBI_RO_WR_EN = 0;
   }
   
   NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_CXL_MISC_CNTL0), &CxlMiscCtrl.Value, 0);
   NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, NBIO0_MISC_CONTROL_1_OFF), &CxlMiscCtrl1.Value, 0);
}