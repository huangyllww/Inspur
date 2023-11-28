/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Fabric - Zeppelin API, and related functions.
 *
 * Contains code that initializes the data fabric
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Fabric
 * @e \$Revision$   @e \$Date$
 *
 */
/*****************************************************************************
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
/*++
Module Name:

  AmdFabricZpPei.c
  Init DF interface

Abstract:
--*/

#include <PiPei.h>
#include "AGESA.h"
#include <Filecode.h>
#include <AmdSoc.h>
#include "FabricPstateServicesPei.h"
#include <Library/PeiFabricTopologyServicesLib.h>
#include "FabricDfClkGatingInit.h"
#include "FabricCsInit.h"
#include "FabricCcmInit.h"
#include "FabricIomsInit.h"
#include "FabricPieRasInit.h"
#include <Library/AmdIdsHookLib.h>
#include "Library/IdsLib.h"
#include <Ppi/SocLogicalIdPpi.h>
#include "FabricScrubInit.h"

#define FILECODE FABRIC_ZP_FABRICZPPEI_AMDFABRICZPPEI_FILECODE


/*++

Routine Description:

  Fabric - Zeppelin Driver Entry.  Initialize the core complex.

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdFabricZpPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                      Status;
  EFI_STATUS                      CalledStatus;
  AMD_PEI_SOC_LOGICAL_ID_PPI      *SocLogicalIdPpi;

  AGESA_TESTPOINT (TpDfPeiEntry, NULL);

  Status = EFI_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdFabricZpPeiInit Entry\n");

  CalledStatus = (*PeiServices)->LocatePpi (
                                 PeiServices,
                                 &gAmdSocLogicalIdPpiGuid,
                                 0,
                                 NULL,
                                 &SocLogicalIdPpi
                                 );

  ASSERT (CalledStatus == EFI_SUCCESS);

  // Publish PEI topology services
  CalledStatus = FabricTopologyServicePpiInstall (PeiServices);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  // Publish PEI P-state services
  CalledStatus = FabricPstateServicePpiInstall (PeiServices);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  IDS_HDT_CONSOLE (CPU_TRACE, "    Clock Gating Init\n");
  FabricClockGatingInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    CS Init\n");
  FabricCsInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    CCM Init\n");
  FabricCcmInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    IOMS Init\n");
  FabricIomsInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    PIE RAS Init\n");
  FabricPieRasInit (PeiServices, SocLogicalIdPpi);

  IDS_HDT_CONSOLE (CPU_TRACE, "    Scrub Init\n");
  FabricScrubInit (PeiServices, SocLogicalIdPpi);

  IDS_HOOK (IDS_HOOK_FABRIC_PEI_INIT_END, NULL, NULL);

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdFabricZpPeiInit End\n");

  AGESA_TESTPOINT (TpDfPeiExit, NULL);

  return (Status);
}

