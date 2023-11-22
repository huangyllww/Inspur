/* $NoKeywords:$ */

/**
 * @file
 *
 * CCX down core and SMT initialization
 *
 * This funtion reduces the number of threads in the system, if desired.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  CCX
 *
 */
/*
 ****************************************************************************
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

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * HYGON processors and its use is subject to the terms and conditions of the
 * HYGON Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an HYGON processor outside of
 * the applicable HYGON product specifications will void any HYGON warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and HYGON disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of HYGON processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "HYGON.h"
#include <Library/IdsLib.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Ppi/HygonCoreTopologyServicesPpi.h>
#include "Filecode.h"
#include "CcxDharmaDownCoreInit.h"
#include <CddRegistersDm.h>
#include <HygonFuse.h>
#include <Library/Iolib.h>
#include <Library/HygonSocBaseLib.h>

#define FILECODE  CCX_DHARMA_CCXDHARMAPEI_CCXDHARMADOWNCOREINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
typedef enum {
  CCX_DOWN_CORE_AUTO_HYEX = 0,   ///< Auto
  CCX_DOWN_CORE_1_0_0_0,        ///< 1 + 0 + 0 + 0 mode
  CCX_DOWN_CORE_1_1_0_0,        ///< 1 + 1 + 0 + 0 mode
  CCX_DOWN_CORE_2_0_0_0,        ///< 2 + 0 + 0 + 0 mode
  CCX_DOWN_CORE_3_0_0_0,        ///< 3 + 0 + 0 + 0 mode
  CCX_DOWN_CORE_2_2_0_0,        ///< 2 + 2 + 0 + 0 mode
  CCX_DOWN_CORE_4_0_0_0,        ///< 4 + 0 + 0 + 0 mode
  CCX_DOWN_CORE_3_3_0_0,        ///< 3 + 3 + 0 + 0 mode

  CCX_DOWN_CORE_1_1_1_0,        ///< 1 + 1 + 1 + 0 mode
  CCX_DOWN_CORE_1_1_1_1,        ///< 1 + 1 + 1 + 1 mode
  CCX_DOWN_CORE_2_2_2_0,        ///< 2 + 2 + 2 + 0 mode
  CCX_DOWN_CORE_2_2_2_2,        ///< 2 + 2 + 2 + 2 mode
  CCX_DOWN_CORE_3_3_3_0,        ///< 3 + 3 + 3 + 0 mode
  CCX_DOWN_CORE_3_3_3_3,        ///< 3 + 3 + 3 + 3 mode
  CCX_DOWN_CORE_4_4_0_0,        ///< 4 + 4 + 0 + 0 mode
  CCX_DOWN_CORE_4_4_4_0,        ///< 4 + 4 + 4 + 0 mode
  CCX_DOWN_CORE_MAX_HYEX         ///< Used for bounds checking
} CCX_DOWN_CORE_MODE_HYEX;

typedef enum {
  CCX_DOWN_CORE_AUTO_HYGX = 0,   ///< Auto
  CCX_DOWN_CORE_1_0,            ///< 1 + 0 mode
  CCX_DOWN_CORE_2_0,            ///< 2 + 0 mode
  CCX_DOWN_CORE_3_0,            ///< 3 + 0 mode
  CCX_DOWN_CORE_4_0,            ///< 4 + 0 mode
  CCX_DOWN_CORE_5_0,            ///< 5 + 0 mode
  CCX_DOWN_CORE_6_0,            ///< 6 + 0 mode
  CCX_DOWN_CORE_7_0,            ///< 7 + 0 mode
  CCX_DOWN_CORE_8_0,            ///< 8 + 0 mode
  CCX_DOWN_CORE_1_1,            ///< 1 + 1 mode
  CCX_DOWN_CORE_2_2,            ///< 2 + 2 mode
  CCX_DOWN_CORE_3_3,            ///< 3 + 3 mode
  CCX_DOWN_CORE_4_4,            ///< 4 + 4 mode
  CCX_DOWN_CORE_5_5,            ///< 5 + 5 mode
  CCX_DOWN_CORE_6_6,            ///< 6 + 6 mode
  CCX_DOWN_CORE_7_7,            ///< 7 + 7 mode
  CCX_DOWN_CORE_MAX_HYGX         ///< Used for bounds checking
} CCX_DOWN_CORE_MODE_HYGX;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
CcxDharmaDownCoreAfterSmuServicesCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
EFI_PEI_NOTIFY_DESCRIPTOR  mAfterSmuServicesPpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonNbioSmuServicesPpiGuid,
  CcxDharmaDownCoreAfterSmuServicesCallback
};

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxDownCoreInitHyEx
 *
 *  Description:
 *    This funtion initializes the DownCore feature for HyEx.
 *
 */
VOID CcxDownCoreInitHyEx (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  IDS_HDT_CONSOLE (CPU_TRACE, "CcxDownCoreInitHyEx Entry \n");
  if ((PcdGet8 (PcdHygonDownCoreMode) != 0) || (PcdGet8 (PcdHygonSmtMode) != 1)) {
    (**PeiServices).NotifyPpi (PeiServices, &mAfterSmuServicesPpiCallback);
  }
}

/**
 *
 *  CcxDownCoreInitHyGx
 *
 *  Description:
 *    This funtion initializes the DownCore feature for HyGx.
 *
 */
VOID CcxDownCoreInitHyGx (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT32          RegEbx;
  UINT8           PwrResetCfg;
  UINT32          CurrentThreadNumber;
  UINT32          ExpectThreadNumber;
  BOOLEAN         NeedReconfigSmtMode;
  UINT8           SmtMode;

  IDS_HDT_CONSOLE (CPU_TRACE, "CcxDownCoreInitHyGx Entry \n");

  SmtMode = PcdGet8 (PcdHygonSmtMode);

  AsmCpuid (0x8000001E, NULL, &RegEbx, NULL, NULL);
  CurrentThreadNumber = ((RegEbx >> 8) & 0xFF) + 1;
  IDS_HDT_CONSOLE (CPU_TRACE, "Expect SMT mode = %x , current thread number = %x\n", SmtMode, CurrentThreadNumber);

  NeedReconfigSmtMode = FALSE;
  ExpectThreadNumber  = (UINT32)(1 << SmtMode);
  if (ExpectThreadNumber != CurrentThreadNumber) {
    if (PcdGet8 (PcdRunEnvironment) == 0) {
      NeedReconfigSmtMode = TRUE;
    } else {
      // For Presilicon ENV, BIOS use fuse define thread number, not change SMT mode
      PcdSet8S (PcdHygonSmtMode, (UINT8)(CurrentThreadNumber / 2));
      IDS_HDT_CONSOLE (CPU_TRACE, "Force SMT mode to %x \n", PcdGet8 (PcdHygonSmtMode));
    }
  }

  if ((PcdGet8 (PcdHygonDownCoreMode) != 0) || (NeedReconfigSmtMode == TRUE)) {
    (**PeiServices).NotifyPpi (PeiServices, &mAfterSmuServicesPpiCallback);
  }

  if (NeedReconfigSmtMode) {
    IDS_HDT_CONSOLE (CPU_TRACE, "Cold Reset to re-enable SMT.\n");
    IoWrite8 (0xCD6, 0x10);// PwrResetCfg
    PwrResetCfg = IoRead8 (0xCD7);
    PwrResetCfg = PwrResetCfg | BIT1;
    IoWrite8 (0xCD7, PwrResetCfg);
    IoWrite8 (0xCF9, 0x02);// HARDSTARTSTATE
    IoWrite8 (0xCF9, 0x06);// HARDRESET
  }
}

/**
 *
 *  CcxDharmaDownCoreOptionSet
 *
 *  Description:
 *    This funtion initializes the DownCore Option Paramter.
 *
 */
 
VOID
CcxDharmaDownCoreOptionSet(
  IN UINT8    DownCoreMode,
  IN UINTN    *DesiredCoreCount,
  IN UINTN    *DesiredComplexCount,
  IN UINT32   *CoreDisMask
)
{
  UINT32 CpuModel;
  
  CpuModel = GetHygonSocModel();
  IDS_HDT_CONSOLE (CPU_TRACE, "  CcxDharmaDownCoreOptionSet Entry, CpuModel = %d.\n", CpuModel);
  if (CpuModel == HYGON_EX_CPU) {
    ASSERT (DownCoreMode < CCX_DOWN_CORE_MAX_HYEX);
    switch (DownCoreMode) {
      case CCX_DOWN_CORE_1_0_0_0:
        IDS_HDT_CONSOLE (CPU_TRACE, "  Downcore mode (1+0) not supported.  No downcoring will take place.\n");
        // Make attainability an impossibility
        *DesiredCoreCount    = 0xFFFFFFFF;
        *DesiredComplexCount = 0xFFFFFFFF;
        break;
      case CCX_DOWN_CORE_2_0_0_0:
        *DesiredCoreCount    = 2;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FFFC;
        break;
      case CCX_DOWN_CORE_3_0_0_0:
        *DesiredCoreCount    = 3;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FFF8;
        break;
      case CCX_DOWN_CORE_4_0_0_0:
        *DesiredCoreCount    = 4;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FFF0;
        break;
      case CCX_DOWN_CORE_1_1_0_0:
        *DesiredCoreCount    = 1;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000FFEE;
        break;
      case CCX_DOWN_CORE_2_2_0_0:
        *DesiredCoreCount    = 2;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000FFCC;
        break;
      case CCX_DOWN_CORE_3_3_0_0:
        *DesiredCoreCount    = 3;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000FF88;
        break;

      case CCX_DOWN_CORE_1_1_1_0:
        *DesiredCoreCount    = 1;
        *DesiredComplexCount = 3;
        *CoreDisMask = 0x0000FEEE;
        break;
      case CCX_DOWN_CORE_1_1_1_1:
        *DesiredCoreCount    = 1;
        *DesiredComplexCount = 4;
        *CoreDisMask = 0x0000EEEE;
        break;
      case CCX_DOWN_CORE_2_2_2_0:
        *DesiredCoreCount    = 2;
        *DesiredComplexCount = 3;
        *CoreDisMask = 0x0000FCCC;
        break;
      case CCX_DOWN_CORE_2_2_2_2:
        *DesiredCoreCount    = 2;
        *DesiredComplexCount = 4;
        *CoreDisMask = 0x0000CCCC;
        break;
      case CCX_DOWN_CORE_3_3_3_0:
        *DesiredCoreCount    = 3;
        *DesiredComplexCount = 3;
        *CoreDisMask = 0x0000F888;
        break;
      case CCX_DOWN_CORE_3_3_3_3:
        *DesiredCoreCount    = 3;
        *DesiredComplexCount = 4;
        *CoreDisMask = 0x00008888;
        break;
      case CCX_DOWN_CORE_4_4_0_0:
        *DesiredCoreCount    = 4;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000FF00;
        break;
      case CCX_DOWN_CORE_4_4_4_0:
        *DesiredCoreCount    = 4;
        *DesiredComplexCount = 3;
        *CoreDisMask = 0x0000F000;
        break;

      default:
        // Make attainability an impossibility
        *DesiredCoreCount    = 0xFFFFFFFF;
        *DesiredComplexCount = 0xFFFFFFFF;
        break;
    }
  } else if (CpuModel == HYGON_GX_CPU) {
    ASSERT (DownCoreMode < CCX_DOWN_CORE_MAX_HYGX);
    switch (DownCoreMode) {
      case CCX_DOWN_CORE_1_0:
        IDS_HDT_CONSOLE (CPU_TRACE, "  Downcore mode (1+0) not supported.  No downcoring will take place.\n");
        // Make attainability an impossibility
        *DesiredCoreCount    = 0xFFFFFFFF;
        *DesiredComplexCount = 0xFFFFFFFF;
        break;
      case CCX_DOWN_CORE_2_0:
        *DesiredCoreCount    = 2;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FFFC;
        break;
      case CCX_DOWN_CORE_3_0:
        *DesiredCoreCount    = 3;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FFF8;
        break;
      case CCX_DOWN_CORE_4_0:
        *DesiredCoreCount    = 4;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FFF0;
        break;
      case CCX_DOWN_CORE_5_0:
        *DesiredCoreCount    = 5;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FFE0;
        break;
      case CCX_DOWN_CORE_6_0:
        *DesiredCoreCount    = 6;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FFC0;
        break;
      case CCX_DOWN_CORE_7_0:
        *DesiredCoreCount    = 7;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FF80;
        break;
      case CCX_DOWN_CORE_8_0:
        *DesiredCoreCount    = 8;
        *DesiredComplexCount = 1;
        *CoreDisMask = 0x0000FF00;
        break;

      case CCX_DOWN_CORE_1_1:
        *DesiredCoreCount    = 2;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000FEFE;
        break;
      case CCX_DOWN_CORE_2_2:
        *DesiredCoreCount    = 4;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000FCFC;
        break;
      case CCX_DOWN_CORE_3_3:
        *DesiredCoreCount    = 6;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000F8F8;
        break;
      case CCX_DOWN_CORE_4_4:
        *DesiredCoreCount    = 8;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000F0F0;
        break;
      case CCX_DOWN_CORE_5_5:
        *DesiredCoreCount    = 10;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000E0E0;
        break;
      case CCX_DOWN_CORE_6_6:
        *DesiredCoreCount    = 12;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x0000C0C0;
        break;
      case CCX_DOWN_CORE_7_7:
        *DesiredCoreCount    = 14;
        *DesiredComplexCount = 2;
        *CoreDisMask = 0x00008080;
        break;

      default:
        // Make attainability an impossibility
        *DesiredCoreCount    = 0xFFFFFFFF;
        *DesiredComplexCount = 0xFFFFFFFF;
        break;
    }
  }
}

/**
 *
 *  CcxDharmaDownCoreInit
 *
 *  Description:
 *    This funtion initializes the DownCore feature.
 *
 */
VOID
CcxDharmaDownCoreInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT32 CpuModel;
  
  CpuModel = GetHygonSocModel();
  if (CpuModel == HYGON_EX_CPU) {
    CcxDownCoreInitHyEx(PeiServices);
  } else if (CpuModel == HYGON_GX_CPU) {
    CcxDownCoreInitHyGx(PeiServices);
  }

}

EFI_STATUS
EFIAPI
CcxDharmaDownCoreAfterSmuServicesCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  UINT8                                   DownCoreMode;
  UINT8                                   SmtMode;
  UINT32                                  CoreDisFuse;
  UINTN                                   i;
  UINTN                                   j;
  UINTN                                   CddId;
  UINTN                                   ComplexCount;
  UINTN                                   CoreCount;
  UINTN                                   ThreadCount;
  UINTN                                   CddCount;
  UINTN                                   CddsPresent;
  UINTN                                   SystemDieCount;
  UINTN                                   SocketCount;
  UINTN                                   DesiredCoreCount;
  UINTN                                   DesiredComplexCount;
  UINT32                                  CoreDisByFuseCount;
  UINT32                                  CoreDisMask;
  UINT32                                  CpuModel;
  BOOLEAN                                 IssueReset;
  BOOLEAN                                 Attainable;
  BOOLEAN                                 Met;
  BOOLEAN                                 AlreadyDownCored;
  EFI_STATUS                              Status;
  EFI_STATUS                              CalledStatus;
  PEI_HYGON_NBIO_SMU_SERVICES_PPI         *NbioSmuServices;
  HYGON_CORE_TOPOLOGY_SERVICES_PPI        *CoreTopologyServices;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServices;

  HGPI_TESTPOINT (TpCcxPeiDownCoreCallbackEntry, NULL);

  Status       = EFI_SUCCESS;
  IssueReset   = FALSE;
  DownCoreMode = PcdGet8 (PcdHygonDownCoreMode);
  SmtMode      = PcdGet8 (PcdHygonSmtMode);
  ASSERT (SmtMode <= 1);

  CalledStatus = (*PeiServices)->LocatePpi (PeiServices, &gHygonNbioSmuServicesPpiGuid, 0, NULL, &NbioSmuServices);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  CalledStatus = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gHygonFabricTopologyServicesPpiGuid,
                                   0,
                                   NULL,
                                   &FabricTopologyServices
                                   );
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  CalledStatus = (*PeiServices)->LocatePpi (
                                   PeiServices,
                                   &gHygonCoreTopologyServicesPpiGuid,
                                   0,
                                   NULL,
                                   &CoreTopologyServices
                                   );
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  FabricTopologyServices->GetSystemInfo (&SocketCount, &SystemDieCount, NULL);

  if (DownCoreMode != 0) {
    CoreDisMask = 0;
    CcxDharmaDownCoreOptionSet(DownCoreMode, &DesiredCoreCount, &DesiredComplexCount, &CoreDisMask);

    Attainable = TRUE;
    Met = TRUE;
    AlreadyDownCored   = FALSE;
    CoreDisByFuseCount = 0;

    for (i = 0; (i < SocketCount) && Attainable; i++) {
      FabricTopologyServices->GetCddInfo (i, &CddCount, &CddsPresent);
      for (j = 0; (j < MAX_CDDS_PER_SOCKET) && Attainable; j++) {
        if (!IS_CDD_PRESENT (j, CddsPresent)) {
          continue;
        }
        
        CpuModel = GetHygonSocModel();
        if (CpuModel == HYGON_EX_CPU) {
          // Get CoreDis FUSE
          NbioSmuServices->SmuRegisterRead (NbioSmuServices, i,  CDD_SPACE(j, MP0_CORE_DISABLE_HYEX), &CoreDisFuse);
        } else if (CpuModel == HYGON_GX_CPU) {
          // Get CoreDis FUSE
          NbioSmuServices->SmuRegisterRead (NbioSmuServices, i, CDD_SPACE (j, MP0_CORE_DISABLE_HYGX), &CoreDisFuse);
        }
        
        CoreDisFuse &= 0xFFFF; // bits[15:0]
        while (CoreDisFuse != 0) {
          if ((CoreDisFuse & 1) == 1) {
            CoreDisByFuseCount++;
          }

          CoreDisFuse = CoreDisFuse >> 1;
        }

        // Get actually core count and complex count
        CoreTopologyServices->GetCoreTopologyOnCdd (
                                CoreTopologyServices,
                                i,
                                j,
                                &ComplexCount,
                                &CoreCount,
                                &ThreadCount
                                );
        IDS_HDT_CONSOLE (CPU_TRACE, "  CoreDisByFuseCount %X\n", CoreDisByFuseCount);
        IDS_HDT_CONSOLE (CPU_TRACE, "  ComplexCount %X; CoreCount %X\n", ComplexCount, CoreCount);

        // Check if already down cored
        if (((CoreCount * ComplexCount) + CoreDisByFuseCount) < 16) {
          AlreadyDownCored = TRUE;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Already down cored, skip down core function\n");
        }

        if ((DesiredCoreCount > CoreCount) || (DesiredComplexCount > ComplexCount)) {
          Attainable = FALSE;
        }

        if ((DesiredCoreCount != CoreCount) || (DesiredComplexCount != ComplexCount)) {
          Met = FALSE;
        }
      }
    }

    if (Attainable && !Met && !AlreadyDownCored) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Set down core register %x\n", CoreDisMask);
      NbioSmuServices->SmuSetDownCoreRegister (NbioSmuServices, CoreDisMask);
      IssueReset = TRUE;
    }
  }

  CddId = FabricTopologyGetFirstPhysCddIdOnSocket (0);
  CoreTopologyServices->GetCoreTopologyOnCdd (
                          CoreTopologyServices, 
                          0, 
                          CddId, 
                          &ComplexCount, 
                          &CoreCount, 
                          &ThreadCount
                          );

  if ((UINTN)(1 << SmtMode) != ThreadCount) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  SMU change SMT mode to %d \n", SmtMode);
    NbioSmuServices->SmuSetSmt (NbioSmuServices, SmtMode);
    IssueReset = TRUE;
  }

  if (IssueReset) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Issuing warm reset\n");
    (**PeiServices).ResetSystem (PeiServices);
  }

  HGPI_TESTPOINT (TpCcxPeiDownCoreCallbackExit, NULL);

  return Status;
}
