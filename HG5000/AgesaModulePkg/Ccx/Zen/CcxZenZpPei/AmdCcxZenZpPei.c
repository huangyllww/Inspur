/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CCX Zen - Zeppelin API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
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

  AmdCcxZenZpPei.c
  Init CCX interface

Abstract:
--*/

#include "PiPei.h"
#include <AGESA.h>
#include "CcxZenZpCacheInit.h"
#include "CcxZenZpDownCoreInit.h"
#include "CcxZenZpBrandString.h"
#include <Library/CcxBaseX86Lib.h>
#include <Library/CcxRolesLib.h>
#include <Library/CcxDownCoreLib.h>
#include <Library/CcxResetTablesLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/IdsLib.h>
#include <Library/FchBaseLib.h>
#include <Library/AmdCapsuleLib.h>
#include <Library/OemAgesaCcxPlatformLib.h>
#include <Ppi/AmdCcxPpi.h>
#include <Ppi/AmdCoreTopologyServicesPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <CcxRegistersZp.h>
#include "Filecode.h"

#define FILECODE CCX_ZEN_CCXZENZPPEI_AMDCCXZENZPPEI_FILECODE

STATIC PEI_AMD_CCX_INIT_COMPLETE_PPI mCcxPeiInitCompletePpi = {
  AMD_CCX_PPI_REVISION
};

STATIC EFI_PEI_PPI_DESCRIPTOR mCcxPeiInitCompletePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdCcxPeiInitCompletePpiGuid,
  &mCcxPeiInitCompletePpi
};

#define ZEN_COMPLEX_MASK  0x0F
#define ZEN_COMPLEX_SIZE  4
#define ZP_COMPLEX_COUNT  2
UINT8  EnabledCoreCountOnComplex[16] =
{
  0x00,  //  0
  0x01,  //  1
  0x01,  //  2
  0x02,  //  3
  0x01,  //  4
  0x02,  //  5
  0x02,  //  6
  0x03,  //  7
  0x01,  //  8
  0x02,  //  9
  0x02,  // 10
  0x03,  // 11
  0x02,  // 12
  0x03,  // 13
  0x03,  // 14
  0x04,  // 15
};

/*++

Routine Description:

  Zen - Zeppelin Driver Entry.  Initialize the core complex.

Arguments:

Returns:

  EFI_STATUS

--*/
EFI_STATUS
EFIAPI
AmdCcxZenZpPeiInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT8                                 OpnComplexCoreCount[ZP_COMPLEX_COUNT];
  UINTN                                 i;
  UINTN                                 j;
  UINTN                                 k;
  UINTN                                 m;
  UINTN                                 n;
  UINTN                                 NumberOfSockets;
  UINTN                                 NumberOfDies;
  UINTN                                 NumberOfComplexes;
  UINTN                                 NumberOfCores;
  UINTN                                 NumberOfThreads;
  UINTN                                 LogicalThread;
  UINTN                                 OpnThreadCount;
  UINTN                                 OpnComplexCount;
  UINTN                                 EnabledComplex;
  UINT32                                SystemDieLoop;
  UINT32                                OpnFuse;
  UINT32                                PackageType;
  UINT32                                NumOfApLauntch;
  UINT32                                BackupSyncFlag;
  volatile UINT32                       *ApSyncFlag;
  volatile UINT32                       mApSyncFlag;
  BOOLEAN                               IsS3;
  BOOLEAN                               S3Support;
  EFI_STATUS                            Status;
  EFI_STATUS                            CalledStatus;
  AMD_CONFIG_PARAMS                     StdHeader;
  PEI_AMD_NBIO_SMU_SERVICES_PPI         *NbioSmuServices;
  AMD_CORE_TOPOLOGY_SERVICES_PPI        *CoreTopologyServices;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServices;
  UINT8                                 SmtMode;

  AGESA_TESTPOINT (TpCcxPeiEntry, NULL);

  Status = EFI_SUCCESS;

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdCcxZenZpPeiInit Entry\n");

  if (CcxIsBsp (&StdHeader)) {
    if ((FchReadSleepType () == 0x3) ||  (AmdCapsuleGetStatus ()) ) {
      IsS3 = TRUE;
    } else {
      IsS3 = FALSE;
    }

    if (!IsS3) {
      CcxZenZpCacheInit (&StdHeader);
      IDS_HDT_CONSOLE (CPU_TRACE, "  CcxZenZpCacheInit Done\n");
    }

    CcxZenZpDownCoreInit (PeiServices);

    CcxProgramTablesAtReset ((IsS3 ? 3 : 0), &StdHeader);
    IDS_HDT_CONSOLE (CPU_TRACE, "  CcxProgramTablesAtReset Done\n");

    if (!IsS3) {
      CcxZenZpSetBrandString (&StdHeader);
      IDS_HDT_CONSOLE (CPU_TRACE, "  CcxZenZpSetBrandString Done\n");
    }

    CalledStatus = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gAmdCoreTopologyServicesPpiGuid,
                                   0,
                                   NULL,
                                   &CoreTopologyServices
                                   );
    ASSERT (CalledStatus == EFI_SUCCESS);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    CalledStatus = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gAmdFabricTopologyServicesPpiGuid,
                                   0,
                                   NULL,
                                   &FabricTopologyServices
                                   );
    ASSERT (CalledStatus == EFI_SUCCESS);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    FabricTopologyServices->GetSystemInfo (&NumberOfSockets, NULL, NULL);

    if (IsS3) {
      AGESA_TESTPOINT (TpCcxPeiStartLaunchApsForS3, NULL);
      IDS_HDT_CONSOLE (CPU_TRACE, "    Launching APs in PEI for S3\n");
      
      mApSyncFlag = 1;
      SaveApSyncFlag((UINT32)&mApSyncFlag, &BackupSyncFlag);
      ApSyncFlag = (volatile UINT32 *)&mApSyncFlag;
      
	  IDS_HDT_CONSOLE (CPU_TRACE, "NumOfApLauntch Address: %X, %X\n", (UINT32)ApSyncFlag, (UINT32)&mApSyncFlag);
      
      for (i = 0; i < NumberOfSockets; i++) {
        FabricTopologyServices->GetProcessorInfo (i, &NumberOfDies, NULL, NULL, NULL);
        for (j = 0; j < NumberOfDies; j++) {
          CoreTopologyServices->GetCoreTopologyOnDie (CoreTopologyServices, i, j, &NumberOfComplexes, &NumberOfCores, &NumberOfThreads);
          for (k = 0; k < NumberOfComplexes; k++) {
            LogicalThread = 0;
            for (m = 0; m < NumberOfCores; m++) {
              for (n = 0; n < NumberOfThreads; n++) {
                if (!((i == 0) && (j == 0) && (k == 0) && (LogicalThread == 0))) {

                  IDS_HDT_CONSOLE (CPU_TRACE, "      Launch socket %X die %X complex %X core %X thread %X\n", i, j, k, m, n);
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
      RestoreContentApSyncFlag(BackupSyncFlag);
      AGESA_TESTPOINT (TpCcxPeiEndLaunchApsForS3, NULL);
    } else {
      // Determine if S3 should be disabled
      S3Support = TRUE;

      CalledStatus = (**PeiServices).LocatePpi (
                      PeiServices,
                      &gAmdNbioSmuServicesPpiGuid,
                      0,
                      NULL,
                      &NbioSmuServices
                      );
      ASSERT (CalledStatus == EFI_SUCCESS);
      Status = (CalledStatus > Status) ? CalledStatus : Status;

      PackageType = CcxGetProcessorPackageType (&StdHeader);
      if (PackageType == (1 << ZP_SOCKET_SP4)) {
        NbioSmuServices->SmuRegisterRead (NbioSmuServices, 0, 0x5D258, &OpnFuse);
        if ((OpnFuse & BIT30) != 0) {
          PackageType = (1 << ZP_SOCKET_SP3r2);
        }
      }

      if ((PackageType == (1 << ZP_SOCKET_SP3)) ||
          (PackageType == (1 << ZP_SOCKET_SP4)) ||
          (PackageType == (1 << ZP_SOCKET_SP4r2)))  {
        IDS_HDT_CONSOLE (CPU_TRACE, "    The package type does not support S3\n");
        S3Support = FALSE;
      } else {
        SmtMode = PcdGet8 (PcdAmdSmtMode);
        SystemDieLoop = 0;
        for (i = 0; i < NumberOfSockets; i++) {
          FabricTopologyServices->GetProcessorInfo (i, &NumberOfDies, NULL, NULL, NULL);
          for (j = 0; j < NumberOfDies; j++) {
            CoreTopologyServices->GetCoreTopologyOnDie (CoreTopologyServices, i, j, &NumberOfComplexes, &NumberOfCores, &NumberOfThreads);
            NbioSmuServices->SmuRegisterRead (NbioSmuServices, SystemDieLoop, 0x5D25C, &OpnFuse);
            //OpnThreadCount = (UINTN) (((OpnFuse >> 8) & 1) + 1); // DM1-277 -
            OpnThreadCount = (UINTN) (((OpnFuse >> 8) & 1) + ((SmtMode == 1) ? 1 : 0)); // DM1-277 +

            OpnFuse = (~OpnFuse) & 0xFF;
            OpnComplexCount = 0;
            EnabledComplex = 0;
            for (k = 0; k < ZP_COMPLEX_COUNT; k++) {
              OpnComplexCoreCount[k] = EnabledCoreCountOnComplex[((OpnFuse >> (ZEN_COMPLEX_SIZE * k)) & ZEN_COMPLEX_MASK)];
              if (OpnComplexCoreCount[k] != 0) {
                OpnComplexCount++;
              }
            }

            ASSERT ((OpnComplexCount > 0) && (OpnComplexCount <= ZP_COMPLEX_COUNT));
            if (OpnComplexCount > 1) {
              ASSERT (OpnComplexCoreCount[0] == OpnComplexCoreCount[1]);
            }

            if ((OpnComplexCount != NumberOfComplexes) ||
                ((UINTN) OpnComplexCoreCount[EnabledComplex] != NumberOfCores) ||
                (OpnThreadCount != NumberOfThreads)) {
              IDS_HDT_CONSOLE (CPU_TRACE, "    Threads have been removed by software.  Disable S3\n");
              S3Support = FALSE;
            }
            SystemDieLoop++;
          }
        }
      }
      if (!S3Support) {
        IDS_HDT_CONSOLE (CPU_TRACE, "    Setting PcdAmdAcpiS3Support to FALSE\n");
        PcdSetBool (PcdAmdAcpiS3Support, S3Support);
      }
    }

    CalledStatus = (**PeiServices).InstallPpi (PeiServices, &mCcxPeiInitCompletePpiList);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  } else {
    Status = EFI_DEVICE_ERROR;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  AmdCcxZenZpPeiInit End\n");

  AGESA_TESTPOINT (TpCcxPeiExit, NULL);

  return (Status);
}
