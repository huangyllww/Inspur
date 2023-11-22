/* $NoKeywords:$ */

/**
 * @file
 *
 * Get MMIO size for each Die
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Library
 *
 */
/*
 ******************************************************************************
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD.  All Rights Reserved.
 *
 * HYGON is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your HYGON
 * representative for a copy.
 *
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 *
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  HYGON DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 *
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  HYGON AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF HYGON AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall HYGON's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless HYGON and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of HYGON's proprietary rights in them.
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
#include "HGPI.h"
#include <Library/HygonBaseLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/FabricResourceManagerServicesProtocol.h>

#define FILECODE  LIBRARY_DXEFABRICRESOURCESIZEFOREACHRBLIB_DXEFABRICRESOURCESIZEFOREACHRBLIB_FILECODE

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
 * FabricGetResourceSizeForEachRb
 *
 * Try to get user's request about MMIO/IO size for each RB from NV variable
 *
 * @param[in, out]    MmioSizeForEachRb         User's request about Mmio size for each RB
 *
 * @retval            EFI_SUCCESS               Get user's request from NV variable successfully
 */
EFI_STATUS
FabricGetResourceSizeForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB *ResourceSizeForEachRb
  )
{
  UINTN       VariableSize;
  EFI_STATUS  Status;

  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_RB);
  Status = gRT->GetVariable (
                  L"ResourceSizeForEachRb",
                  &gHygonResourceSizeForEachRbGuid,
                  NULL,
                  &VariableSize,
                  (VOID *)ResourceSizeForEachRb
                  );

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricSetResourceSizeForEachRb
 *
 * Save user's request about MMIO/IO size for each Die to NV variable
 *
 * @param[in]    CurrentUsed        Current used Mmio size for each Die
 * @param[in]    UserRequest        User's request about Mmio size for each Die
 *
 * @retval       EFI_SUCCESS        Save user's request to NV variable successfully
 */
EFI_STATUS
FabricSetResourceSizeForEachRb (
  IN       FABRIC_RESOURCE_FOR_EACH_RB *CurrentUsed,
  IN       FABRIC_RESOURCE_FOR_EACH_RB *UserRequest
  )
{
  UINT8                        i;
  UINT8                        j;
  UINT8                        k;
  UINTN                        VariableSize;
  EFI_STATUS                   Status;
  FABRIC_RESOURCE_FOR_EACH_RB  CurrentRequest;

  // Calculate total size for each DIE
  for (i = 0; i < MAX_SOCKETS_SUPPORTED; i++) {
    for (j = 0; j < MAX_IOD_PER_SOCKET; j++) {
      for (k = 0; k < MAX_RBS_PER_IOD; k++) {
        CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j][k].Size = CurrentUsed->NonPrefetchableMmioSizeAbove4G[i][j][k].Size + UserRequest->NonPrefetchableMmioSizeAbove4G[i][j][k].Size;
        CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j][k].Size = CurrentUsed->NonPrefetchableMmioSizeBelow4G[i][j][k].Size + UserRequest->NonPrefetchableMmioSizeBelow4G[i][j][k].Size;
        CurrentRequest.PrefetchableMmioSizeAbove4G[i][j][k].Size    = CurrentUsed->PrefetchableMmioSizeAbove4G[i][j][k].Size + UserRequest->PrefetchableMmioSizeAbove4G[i][j][k].Size;
        CurrentRequest.PrefetchableMmioSizeBelow4G[i][j][k].Size    = CurrentUsed->PrefetchableMmioSizeBelow4G[i][j][k].Size + UserRequest->PrefetchableMmioSizeBelow4G[i][j][k].Size;

        if (0 == CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j][k].Size) {
		  CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment = 0;
        } else {	
          CurrentRequest.NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment = UserRequest->NonPrefetchableMmioSizeAbove4G[i][j][k].Alignment;
        }
		if (0 == CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j][k].Size ) {
          CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j][k].Alignment = 0;
		} else {
          CurrentRequest.NonPrefetchableMmioSizeBelow4G[i][j][k].Alignment = UserRequest->NonPrefetchableMmioSizeBelow4G[i][j][k].Alignment;
		}
		if (0 == CurrentRequest.PrefetchableMmioSizeAbove4G[i][j][k].Size ) {
          CurrentRequest.PrefetchableMmioSizeAbove4G[i][j][k].Alignment = 0;
		} else {
          CurrentRequest.PrefetchableMmioSizeAbove4G[i][j][k].Alignment    = UserRequest->PrefetchableMmioSizeAbove4G[i][j][k].Alignment;
		}
		if (0 == CurrentRequest.PrefetchableMmioSizeBelow4G[i][j][k].Size ) {
		  CurrentRequest.PrefetchableMmioSizeBelow4G[i][j][k].Alignment = 0;
		} else {
          CurrentRequest.PrefetchableMmioSizeBelow4G[i][j][k].Alignment    = UserRequest->PrefetchableMmioSizeBelow4G[i][j][k].Alignment;
		}
        CurrentRequest.IO[i][j][k].Size = CurrentUsed->IO[i][j][k].Size + UserRequest->IO[i][j][k].Size;
      }
    }
  }

  // Save user's request to NV
  VariableSize = sizeof (FABRIC_RESOURCE_FOR_EACH_RB);

  Status = gRT->SetVariable (
                  L"ResourceSizeForEachRb",
                  &gHygonResourceSizeForEachRbGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableSize,
                  (VOID *)&CurrentRequest
                  );
  IDS_HDT_CONSOLE (CPU_TRACE, "  Set DF Resource size per Rb to NV variable, Status: %r\n", Status);

  return Status;
}

/*---------------------------------------------------------------------------------------*/

/**
 * FabricClearResourceSizeForEachRb
 *
 * Clear user's request about MMIO/IO size for each Die to NV variable
 *
 *
 * @retval       EFI_SUCCESS        Clear NV variable
 */
EFI_STATUS
FabricClearResourceSizeForEachRb (
  )
{
  UINT8  Ignore;

  gRT->SetVariable (
         L"ResourceSizeForEachRb",
         &gHygonResourceSizeForEachRbGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
         0,
         (VOID *)&Ignore
         );
  IDS_HDT_CONSOLE (CPU_TRACE, "  Clear DF Resource size per RB to NV variable\n");

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
  UINTN       VariableSize;
  EFI_STATUS  Status;

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_IOD_PER_SOCKET * MAX_RBS_PER_IOD;
  Status = gRT->GetVariable (
                  L"ResourceDistribution",
                  &gHygonResourceDistributionGuid,
                  NULL,
                  &VariableSize,
                  (VOID *)ResourceDistribution
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
  UINTN       VariableSize;
  EFI_STATUS  Status;

  VariableSize = sizeof (BOOLEAN) * MAX_SOCKETS_SUPPORTED * MAX_IOD_PER_SOCKET * MAX_RBS_PER_IOD;
  Status = gRT->SetVariable (
                  L"ResourceDistribution",
                  &gHygonResourceDistributionGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
                  VariableSize,
                  (VOID *)ResourceDistribution
                  );

  return Status;
}
