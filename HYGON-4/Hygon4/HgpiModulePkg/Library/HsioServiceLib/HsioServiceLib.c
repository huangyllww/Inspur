/* $NoKeywords:$ */
/**
 * @file
 *
 * HSIO services
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
 *
 */
/*
*****************************************************************************
*
 * 
 * Copyright 2016 - 2022 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Filecode.h>
#include  <HGPI.h>
#include  <Gnb.h>
#include  <Filecode.h>
#include  <Library/GnbLib.h>
#include  <Library/IdsLib.h>
#include  <Library/HygonBaseLib.h>
#include  <Library/NbioHandleLib.h>
#include  <Library/NbioSmuLib.h>
#include  <Library/HygonPspMboxLib.h>

#define FILECODE LIBRARY_HSIOSERVICELIB_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * HSIO firmware request - Common function
 *
 *
 * @param[in]      NbioHandle      Pointer to GNB_HANDLE
 * @param[in]      HsioMBoxRequest HSIO Mailbox Request ID
 * @param[in, out] HsioMBoxArg     Psp Argument structure - [0] entry reserved for HSIO Mailbox Request ID
 * @param[in]      AccessFlags     Access Flag
 */

UINT32
HsioServiceRequestCore (
  IN       GNB_HANDLE               *NbioHandle,
  IN       UINT32                   HsioMBoxRequest,
  IN OUT   UINT32                   *HsioMBoxArg,
  IN       UINT32                   AccessFlags
  )
{
  UINT32            RetVal;
  *HsioMBoxArg = HsioMBoxRequest;

  RetVal = NbioPspServiceRequest (
    NbioHandle->Address.Address.Bus,
    NbioHandle->PhysicalDieId,
    MboxBiosCmdHsioRequest,
    HsioMBoxArg,
    AccessFlags
    );

  IDS_HDT_CONSOLE (
    GNB_TRACE,
    "Return Args = 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, ",
    HsioMBoxArg[0],
    HsioMBoxArg[1],
    HsioMBoxArg[2],
    HsioMBoxArg[3],
    HsioMBoxArg[4],
    HsioMBoxArg[5]
    );

  return RetVal;
}

/*----------------------------------------------------------------------------------------
/**
  Executes a specific HSIO service and returns the results (Generally intended for NBIO internal requests)

  Parameters:
  GNB_HANDLE
    GenHandle
  ServiceRequest
    The service request identifier
  InputParameters
    Pointer to an array of service request arguments (for CZ, UINT32[6]).  Input parameters are not validated by the driver prior to issuing the service request.
  ReturnValues
    Pointer to an array of service request response values (for CZ, UINT32[6])
**/
VOID
HsioServiceRequest (
  IN  GNB_HANDLE                  *GnbHandle,
  IN  UINT32                      ServiceRequest,
  IN  UINT32                      *InputParameters,
  OUT UINT32                      *ReturnValues
  )
{
  UINT32                          HsioMsgArg[6];
  UINT32                          HsioMsgResult;

  IDS_HDT_CONSOLE (MAIN_FLOW, "HsioServiceRequest Entry\n");

  LibHygonMemCopy (HsioMsgArg, InputParameters, 24, (HYGON_CONFIG_PARAMS *) NULL);
  HsioMsgResult = HsioServiceRequestCore (GnbHandle, ServiceRequest, HsioMsgArg, 0);
  if (HsioMsgResult == 1) {
    LibHygonMemCopy (ReturnValues, HsioMsgArg, 24, (HYGON_CONFIG_PARAMS *) NULL);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "HsioServiceRequest Exit SmuResult = %d\n", HsioMsgResult);
}