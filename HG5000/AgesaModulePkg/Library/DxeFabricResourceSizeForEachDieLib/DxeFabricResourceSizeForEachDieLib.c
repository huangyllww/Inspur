/* $NoKeywords:$ */
/**
 * @file
 *
 * Get MMIO size for each Die
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2008 - 2018 ADVANCED MICRO DEVICES, INC.  All Rights Reserved.
 *
 * AMD is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your AMD
 * representative for a copy.
 *
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 *
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  AMD DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 *
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  AMD AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF AMD AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall AMD's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless AMD and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of AMD's proprietary rights in them.
 *
 * EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
 * stated in the Software License Agreement.
 *******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Uefi.h"
#include "PiDxe.h"
#include "Filecode.h"
#include "AGESA.h"
#include <Library/AmdBaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>

#define FILECODE LIBRARY_DXEFABRICRESOURCESIZEFOREACHDIELIB_DXEFABRICRESOURCESIZEFOREACHDIELIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetResourceSizeForEachDie
 *
 * Try to get user's request about MMIO/IO size for each Die from NV variable
 *
 * @param[in, out]    MmioSizeForEachDie        User's request about Mmio size for each Die
 *
 * @retval            EFI_SUCCESS               Get user's request from NV variable successfully
 */
EFI_STATUS
FabricGetResourceSizeForEachDie (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *ResourceSizeForEachDie
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;

  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_DIE);
  Status = gRT->GetVariable (
                  L"ResourceSizeForEachDie",
                  &gAmdResourceSizeForEachDieGuid,
                  NULL,
                  &VariableSize,
                  (VOID *) ResourceSizeForEachDie
                  );

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricSetResourceSizeForEachDie
 *
 * Save user's request about MMIO/IO size for each Die to NV variable
 *
 * @param[in]    CurrentUsed        Current used Mmio size for each Die
 * @param[in]    UserRequest        User's request about Mmio size for each Die
 *
 * @retval       EFI_SUCCESS        Save user's request to NV variable successfully
 */
EFI_STATUS
FabricSetResourceSizeForEachDie (
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *CurrentUsed,
  IN       FABRIC_RESOURCE_FOR_EACH_DIE *UserRequest
  )
{
  UINT8  i;
  UINT8  j;
  UINTN  VariableSize;
  EFI_STATUS Status;
  FABRIC_RESOURCE_FOR_EACH_DIE CurrentRequest;

  // Calculate total size for each DIE
  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_DIES_PER_SOCKET; j++) {
      CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j].Size = CurrentUsed->NonPrefetchableMmioSizeAbove4G[i][j].Size + UserRequest->NonPrefetchableMmioSizeAbove4G[i][j].Size;
      CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j].Size = CurrentUsed->NonPrefetchableMmioSizeBelow4G[i][j].Size + UserRequest->NonPrefetchableMmioSizeBelow4G[i][j].Size;
      CurrentRequest.PrefetchableMmioSizeAbove4G[i][j].Size = CurrentUsed->PrefetchableMmioSizeAbove4G[i][j].Size + UserRequest->PrefetchableMmioSizeAbove4G[i][j].Size;
      CurrentRequest.PrefetchableMmioSizeBelow4G[i][j].Size = CurrentUsed->PrefetchableMmioSizeBelow4G[i][j].Size + UserRequest->PrefetchableMmioSizeBelow4G[i][j].Size;

      CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j].Alignment = UserRequest->NonPrefetchableMmioSizeAbove4G[i][j].Alignment;
      CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j].Alignment = UserRequest->NonPrefetchableMmioSizeBelow4G[i][j].Alignment;
      CurrentRequest.PrefetchableMmioSizeAbove4G[i][j].Alignment = UserRequest->PrefetchableMmioSizeAbove4G[i][j].Alignment;
      CurrentRequest.PrefetchableMmioSizeBelow4G[i][j].Alignment = UserRequest->PrefetchableMmioSizeBelow4G[i][j].Alignment;

      CurrentRequest.IO[i][j].Size = CurrentUsed->IO[i][j].Size + UserRequest->IO[i][j].Size;
    }
  }

  // Die0's 2nd MMIO
  CurrentRequest.Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment = UserRequest->Die0SecondNonPrefetchableMmioSizeBelow4G.Alignment;
  CurrentRequest.Die0SecondNonPrefetchableMmioSizeBelow4G.Size = CurrentUsed->Die0SecondNonPrefetchableMmioSizeBelow4G.Size + UserRequest->Die0SecondNonPrefetchableMmioSizeBelow4G.Size;
  CurrentRequest.Die0SecondPrefetchableMmioSizeBelow4G.Alignment = UserRequest->Die0SecondPrefetchableMmioSizeBelow4G.Alignment;
  CurrentRequest.Die0SecondPrefetchableMmioSizeBelow4G.Size = CurrentUsed->Die0SecondPrefetchableMmioSizeBelow4G.Size + UserRequest->Die0SecondPrefetchableMmioSizeBelow4G.Size;

  // Save user's request to NV
  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_DIE);

  Status = gRT->SetVariable (
                  L"ResourceSizeForEachDie",
                  &gAmdResourceSizeForEachDieGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableSize,
                  (VOID *) &CurrentRequest
                  );
  IDS_HDT_CONSOLE (CPU_TRACE, "  Set DF Resource size per DIE to NV variable, Status: %x\n", Status);

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricClearResourceSizeForEachDie
 *
 * Clear user's request about MMIO/IO size for each Die to NV variable
 *
 *
 * @retval       EFI_SUCCESS        Clear NV variable
 */
EFI_STATUS
FabricClearResourceSizeForEachDie (
  )
{
  UINT8  Ignore;

  gRT->SetVariable (
                  L"ResourceSizeForEachDie",
                  &gAmdResourceSizeForEachDieGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  0,
                  (VOID *) &Ignore
                  );
  IDS_HDT_CONSOLE (CPU_TRACE, "  Clear DF Resource size per DIE to NV variable\n");

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricGetResourceDistribution
 *
 * Get the information about which Die's MMIO should be put above Pcie Cfg
 *
 * @param[in, out]    ResourceDistribution       If it's above Pcie Cfg
 *
 */
EFI_STATUS
FabricGetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET;
  Status = gRT->GetVariable (
                  L"ResourceDistribution",
                  &gAmdResourceDistributionGuid,
                  NULL,
                  &VariableSize,
                  (VOID *) ResourceDistribution
                  );

  return Status;
}

/*---------------------------------------------------------------------------------------*/
/**
 * FabricSetResourceDistribution
 *
 * Set the information about which Die's MMIO should be put above Pcie Cfg
 *
 * @param[in, out]    ResourceDistribution       If it's above Pcie Cfg
 *
 */
EFI_STATUS
FabricSetResourceDistribution (
  IN       BOOLEAN   *ResourceDistribution
  )
{
  UINTN                             VariableSize;
  EFI_STATUS                        Status;

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_DIES_PER_SOCKET;
  Status = gRT->SetVariable (
                  L"ResourceDistribution",
                  &gAmdResourceDistributionGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableSize,
                  (VOID *) ResourceDistribution
                  );

  return Status;
}




