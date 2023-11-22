/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON SOC BIST Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
 *
 */
/*
 ******************************************************************************
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

#include "Uefi.h"
#include <Library/BaseLib.h>
#include "HYGON.h"
#include "Filecode.h"
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PeiSocBistLib.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Ppi/HygonCoreTopologyServicesPpi.h>
#include <Ppi/HygonErrorLogPpi.h>

#define FILECODE  LIBRARY_PEISOCBISTLOGGINGLIB_SOCBISTLOGGINGLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
*/

/*---------------------------------------------------------------------------------------*/

/**
 *
 * Logs an error if BIST failures are detected
 *
 * @param[in]      PeiServices            Pointer to PEI services
 *
 */
VOID
LogBistStatus (
  IN       CONST EFI_PEI_SERVICES **PeiServices
  )
{
  UINTN                                   i;
  UINTN                                   j;
  UINTN                                   k;
  UINTN                                   NumberOfSockets;
  UINTN                                   NumberOfCdds;
  UINTN                                   CddsPresent;
  UINTN                                   NumberOfComplexes;
  UINTN                                   NumberOfCoresPerComplex;
  UINTN                                   NumberOfThreadsPerCore;
  UINT32                                  BistData;
  EFI_STATUS                              CalledStatus;
  HGPI_STATUS                             BistStatus;
  PEI_HYGON_ERROR_LOG_PPI                 *HygonErrorLog;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServices;
  HYGON_CORE_TOPOLOGY_SERVICES_PPI        *CoreTopologyServices;

  IDS_HDT_CONSOLE (MAIN_FLOW, "LogBistStatus Entry\n");
  CalledStatus = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gHygonFabricTopologyServicesPpiGuid,
                                   0,
                                   NULL,
                                   &FabricTopologyServices
                                   );

  ASSERT (CalledStatus == EFI_SUCCESS);

  CalledStatus = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gHygonCoreTopologyServicesPpiGuid,
                                   0,
                                   NULL,
                                   &CoreTopologyServices
                                   );

  ASSERT (CalledStatus == EFI_SUCCESS);

  CalledStatus = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gHygonErrorLogPpiGuid,
                                   0,
                                   NULL,
                                   &HygonErrorLog
                                   );

  ASSERT (CalledStatus == EFI_SUCCESS);

  FabricTopologyServices->GetSystemInfo (&NumberOfSockets, NULL, NULL);

  for (i = 0; i < NumberOfSockets; i++) {
    FabricTopologyServices->GetCddInfo (i, &NumberOfCdds, &CddsPresent);
    BistStatus = ReadNonCcxBistData (i, &BistData, PeiServices);

    if (BistStatus != HGPI_SUCCESS) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "***************************** !!Non-CCX BIST ERROR!! Socket %d *********************\n", i);

      HygonErrorLog->HygonErrorLogPei (
                       HygonErrorLog,
                       HGPI_ALERT,
                       CPU_EVENT_NON_CCX_BIST_FAILURE,
                       i,
                       0x00000000,
                       0x00000000,
                       BistData
                       );
    }

    for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
      if (!IS_CDD_PRESENT (j, CddsPresent)) {
        continue;
      }

      CoreTopologyServices->GetCoreTopologyOnCdd (CoreTopologyServices, i, j, &NumberOfComplexes, &NumberOfCoresPerComplex, &NumberOfThreadsPerCore);
      for (k = 0; k < NumberOfComplexes; k++) {
        // Read Bist
        BistStatus = ReadCcxBistData (i, j, k, PeiServices);

        // If there's a BIST failure, log it in error log
        if (BistStatus != HGPI_SUCCESS) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "***************************** !!CCX BIST ERROR!! Socket %d Die %d Complex %d *********************\n", i, j, k);

          HygonErrorLog->HygonErrorLogPei (
                           HygonErrorLog,
                           HGPI_ALERT,
                           CPU_EVENT_CCX_BIST_FAILURE,
                           i,
                           j,
                           k,
                           0x00000000
                           );
        }
      }
    }
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "LogBistStatus Exit\n");
}
