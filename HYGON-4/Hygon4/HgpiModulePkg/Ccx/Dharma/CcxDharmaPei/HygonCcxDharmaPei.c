/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CCX Dharma - Satori API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  CCX
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
/*++
Module Name:

  HygonCcxDharmaPei.c
  Init CCX interface

Abstract:
--*/

#include "PiPei.h"
#include <HGPI.h>
#include <CddRegistersDm.h>
#include "CcxDharmaCacheInit.h"
#include "CcxDharmaDownCoreInit.h"
#include "CcxDharmaBrandString.h"
#include <Library/CcxBaseX86Lib.h>
#include <Library/CcxRolesLib.h>
#include <Library/CcxDownCoreLib.h>
#include <Library/CcxResetTablesLib.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/IdsLib.h>
#include <Library/FchBaseLib.h>
#include <Library/HygonCapsuleLib.h>
#include <Library/OemHgpiCcxPlatformLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Ppi/HygonCcxPpi.h>
#include <Ppi/HygonCoreTopologyServicesPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <CcxRegistersDm.h>
#include "Filecode.h"
#include <cpuRegisters.h>

#define FILECODE  CCX_DHARMA_CCXDHARMAPEI_HYGONCCXDHARMAPEI_FILECODE

STATIC PEI_HYGON_CCX_INIT_COMPLETE_PPI  mCcxPeiInitCompletePpi = {
  HYGON_CCX_PPI_REVISION
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mCcxPeiInitCompletePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonCcxPeiInitCompletePpiGuid,
  &mCcxPeiInitCompletePpi
};

/*++

Routine Description:

  Dharma - Satori Driver Entry.  Initialize the core complex.

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
HygonCcxDharmaPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINTN                                   i;
  UINTN                                   j;
  UINTN                                   k;
  UINTN                                   m;
  UINTN                                   n;
  UINTN                                   NumberOfSockets;
  UINTN                                   NumberOfCdds;
  UINTN                                   CddsPresent;
  UINTN                                   CddId;
  UINTN                                   NumberOfComplexes;
  UINTN                                   NumberOfCores;
  UINTN                                   NumberOfThreads;
  UINTN                                   LogicalThread;
  UINTN                                   OpnThreadCount;
  UINT32                                  NumOfApLauntch;
  UINT32                                  BackupSyncFlag;
  UINT32                                  CpuModel;
  volatile UINT32                         *ApSyncFlag;
  volatile UINT32                         mApSyncFlag;
  BOOLEAN                                 IsS3;
  BOOLEAN                                 S3Support;
  EFI_STATUS                              Status;
  EFI_STATUS                              CalledStatus;
  HYGON_CONFIG_PARAMS                     StdHeader;
  PEI_HYGON_NBIO_SMU_SERVICES_PPI         *NbioSmuServices;
  HYGON_CORE_TOPOLOGY_SERVICES_PPI        *CoreTopologyServices;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServices;
  UINT8                                   SmtMode;
  THREAD_CONFIGURATION_STRUCT             ThreadConfiguration;

  HGPI_TESTPOINT (TpCcxPeiEntry, NULL);

  Status = EFI_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "  HygonCcxDharmaPeiInit Entry\n");

  if (CcxIsBsp (&StdHeader)) {
    if ((FchReadSleepType () == 0x3) ||  (HygonCapsuleGetStatus ())) {
      IsS3 = TRUE;
    } else {
      IsS3 = FALSE;
    }

    if (!IsS3) {
      CcxDharmaCacheInit (&StdHeader);
      IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaCacheInit Done\n");
    }

    CalledStatus = (*PeiServices)->LocatePpi (
                                     PeiServices,
                                     &gHygonCoreTopologyServicesPpiGuid,
                                     0,
                                     NULL,
                                     &CoreTopologyServices
                                     );
    ASSERT (CalledStatus == EFI_SUCCESS);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    CalledStatus = (*PeiServices)->LocatePpi (
                                     PeiServices,
                                     &gHygonFabricTopologyServicesPpiGuid,
                                     0,
                                     NULL,
                                     &FabricTopologyServices
                                     );
    ASSERT (CalledStatus == EFI_SUCCESS);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    FabricTopologyServices->GetSystemInfo (&NumberOfSockets, NULL, NULL);
    FabricTopologyServices->GetCddInfo (0, &NumberOfCdds, &CddsPresent);
    CddId = FabricTopologyGetFirstPhysCddIdOnSocket (0);
    // If total thread count is great or equal to 255, enable x2APIC mode, this must be done before CcxProgramTablesAtReset ()
    CoreTopologyServices->GetCoreTopologyOnCdd (
                            CoreTopologyServices,
                            0,
                            CddId,
                            &NumberOfComplexes,
                            &NumberOfCores,
                            &NumberOfThreads
                            );
    if (((NumberOfSockets * NumberOfCdds * NumberOfComplexes * NumberOfCores * NumberOfThreads) >= PcdGet8 (PcdMinCoresSwitchToX2ApicMode)) &&
        (!PcdGetBool (PcdX2ApicMode))) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Threads count above %d, PcdX2ApicMode is changed to TRUE\n", PcdGet8 (PcdMinCoresSwitchToX2ApicMode));
      PcdSetBoolS (PcdX2ApicMode, TRUE);
    } else {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Keep PcdX2ApicMode = %d \n", PcdGetBool (PcdX2ApicMode));
    }

    AsmMsrOr64 (MSR_APIC_BAR, BIT11);             // Enble BSP's XAPIC in the first step
    if (PcdGetBool (PcdX2ApicMode)) {
      AsmMsrOr64 (MSR_APIC_BAR, BIT11 | BIT10);   // Enble BSP's X2APIC in the second step
      ASSERT (PcdGetBool (PcdCfgIommuSupport));   // if X2APIC enable, IOMMU must be enable.
    }

    CcxDharmaDownCoreInit (PeiServices);
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaDownCoreInit Done\n");

    CcxProgramTablesAtReset ((IsS3 ? 3 : 0), &StdHeader);
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxProgramTablesAtReset Done\n");

    if (!IsS3) {
      CcxDharmaSetBrandString (&StdHeader);
      IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaSetBrandString Done\n");
    }

    if (IsS3) {
      HGPI_TESTPOINT (TpCcxPeiStartLaunchApsForS3, NULL);
      IDS_HDT_CONSOLE (CPU_TRACE, "    Launching APs in PEI for S3\n");

      mApSyncFlag = 1;
      SaveApSyncFlag ((UINT32)&mApSyncFlag, &BackupSyncFlag);
      ApSyncFlag = (volatile UINT32 *)&mApSyncFlag;

      IDS_HDT_CONSOLE (CPU_TRACE, "NumOfApLauntch Address: %X, %X\n", (UINT32)ApSyncFlag, (UINT32)&mApSyncFlag);

      for (i = 0; i < NumberOfSockets; i++) {
        FabricTopologyServices->GetCddInfo (i, &NumberOfCdds, &CddsPresent);
        for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
          if (!IS_CDD_PRESENT (j, CddsPresent)) {
            continue;
          }

          CoreTopologyServices->GetCoreTopologyOnCdd (
                                  CoreTopologyServices,
                                  i,
                                  j,
                                  &NumberOfComplexes,
                                  &NumberOfCores,
                                  &NumberOfThreads
                                  );
          for (k = 0; k < NumberOfComplexes; k++) {
            LogicalThread = 0;
            for (m = 0; m < NumberOfCores; m++) {
              for (n = 0; n < NumberOfThreads; n++) {
                if (!((i == 0) && (j == 0) && (k == 0) && (LogicalThread == 0))) {
                  IDS_HDT_CONSOLE (
                    CPU_TRACE,
                    "      Launch socket %X cdd %X complex %X core %X thread %X\n",
                    i,
                    j,
                    k,
                    m,
                    n
                    );
                  NumOfApLauntch = *ApSyncFlag;
                  CoreTopologyServices->LaunchThread (CoreTopologyServices, i, j, k, LogicalThread);
                  while(*ApSyncFlag != NumOfApLauntch+1) {
                      ;
                  }
                }

                LogicalThread++;
              }
            }
          }
        }
      }

      // MSRC001_1023[49, TwCfgCombineCr0Cd] = 1
      AsmMsrOr64 (0xC0011023, BIT49);
      RestoreContentApSyncFlag (BackupSyncFlag);
      HGPI_TESTPOINT (TpCcxPeiEndLaunchApsForS3, NULL);
    } else if(PcdGetBool (PcdHygonAcpiS3Support) == TRUE) {
      // Determine if S3 should be disabled
      S3Support = TRUE;

      CalledStatus = (**PeiServices).LocatePpi (
                                       PeiServices,
                                       &gHygonNbioSmuServicesPpiGuid,
                                       0,
                                       NULL,
                                       &NbioSmuServices
                                       );
      ASSERT (CalledStatus == EFI_SUCCESS);
      Status = (CalledStatus > Status) ? CalledStatus : Status;

      SmtMode = PcdGet8 (PcdHygonSmtMode);
      for (i = 0; i < NumberOfSockets; i++) {
        FabricTopologyServices->GetCddInfo (i, &NumberOfCdds, &CddsPresent);
        for (j = 0; j < MAX_CDDS_PER_SOCKET; j++) {
          if (!IS_CDD_PRESENT (j, CddsPresent)) {
            continue;
          }

          CoreTopologyServices->GetCoreTopologyOnCdd (
                                  CoreTopologyServices,
                                  i,
                                  j,
                                  &NumberOfComplexes,
                                  &NumberOfCores,
                                  &NumberOfThreads
                                  );
          
          CpuModel = GetHygonSocModel();
          if (CpuModel == HYGON_EX_CPU) {
            NbioSmuServices->SmuRegisterRead (
                             NbioSmuServices,
                             i,
                             CDD_SPACE (j, THREAD_CONFIGURATION_ADDRESS_HYEX),
                             (UINT32 *)&ThreadConfiguration
                             );
            OpnThreadCount = (UINTN)(((ThreadConfiguration.HyExField.SMTMode == 0) ? 1 : 0) + ((SmtMode == 1) ? 1 : 0));
          } else if (CpuModel == HYGON_GX_CPU) {
            NbioSmuServices->SmuRegisterRead (
                             NbioSmuServices,
                             i,
                             CDD_SPACE (j, THREAD_CONFIGURATION_ADDRESS_HYGX),
                             (UINT32 *)&ThreadConfiguration
                             );
            OpnThreadCount = (UINTN)(((ThreadConfiguration.HyGxField.SMTMode == 0) ? 1 : 0) + ((SmtMode == 1) ? 1 : 0));
          }

          //
          // todo Enabled core/ccx compare with OpnFuse
          //

          if (OpnThreadCount != NumberOfThreads) {
            IDS_HDT_CONSOLE (CPU_TRACE, "    Threads have been removed by software.  Disable S3\n");
            S3Support = FALSE;
          }
        }
      }

      if (!S3Support) {
        IDS_HDT_CONSOLE (CPU_TRACE, "    Setting PcdHygonAcpiS3Support to FALSE\n");
        PcdSetBoolS (PcdHygonAcpiS3Support, S3Support);
      }
    }

    CalledStatus = (**PeiServices).InstallPpi (PeiServices, &mCcxPeiInitCompletePpiList);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  HygonCcxDharmaPeiInit End\n");

  HGPI_TESTPOINT (TpCcxPeiExit, NULL);

  return (Status);
}
