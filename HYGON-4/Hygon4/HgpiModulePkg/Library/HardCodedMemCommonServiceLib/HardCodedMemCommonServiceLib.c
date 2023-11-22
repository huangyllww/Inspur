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
  * ***************************************************************************
  *
 */

#include "SocLogicalId.h"
#include "Library/HygonBaseLib.h"
#include "Ids.h"
#include "Library/IdsLib.h"
#include "Library/HygonErrorLogLib.h"
#include "Library/HygonHeapLib.h"
#include "Filecode.h"

#define FILECODE  LIBRARY_HARDCODEDMEMCOMMONSERVICELIB_HARDCODEDMEMCOMMONSERVICELIB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */
UINT32
GetPlatformNumberOfSockets (
  VOID
  )
{
  return 1;
}

BOOLEAN
GetSocketModuleOfNode (
  IN       UINT32    Node,
  OUT   UINT32    *Socket,
  OUT   UINT32    *Module,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  *Socket = 0;
  *Module = 0;

  return TRUE;
}

VOID
PutEventLog (
  IN       HGPI_STATUS EventClass,
  IN       UINT32 EventInfo,
  IN       UINT32 DataParam1,
  IN       UINT32 DataParam2,
  IN       UINT32 DataParam3,
  IN       UINT32 DataParam4,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
}

BOOLEAN
GetPciAddress (
  IN       HYGON_CONFIG_PARAMS *StdHeader,
  IN       UINT32            Socket,
  IN       UINT32            Module,
  OUT   PCI_ADDR          *PciAddress,
  OUT   HGPI_STATUS      *HgpiStatus
  )
{
  BOOLEAN  Result;

  Result = TRUE;
  // Always Succeeds.
  *HgpiStatus = HGPI_SUCCESS;

  PciAddress->AddressValue = MAKE_SBDFO (0, 0, 24, 0, 0);

  return Result;
}

BOOLEAN
IsProcessorPresent (
  IN       UINT32             Socket,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN  Result;

  Result = FALSE;

  if (Socket == 0) {
    Result = TRUE;
  }

  return Result;
}
