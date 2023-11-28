/* $NoKeywords:$ */
/**
 * @file
 *
 * CCX down core and SMT initialization
 *
 * This funtion reduces the number of threads in the system, if desired.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CCX
 * @e \$Revision$   @e \$Date$
 *
 */
/*
 ****************************************************************************
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

/*****************************************************************************
 *
 * This software package can be used to enable the Overclocking of certain
 * AMD processors and its use is subject to the terms and conditions of the
 * AMD Overclocking Waiver. Enabling overclocking through use of the low-level
 * routines included in this package and operating an AMD processor outside of
 * the applicable AMD product specifications will void any AMD warranty and can
 * result in damage to the processor or the system into which the processor has
 * been integrated. The user of this software assumes, and AMD disclaims, all
 * risk, liability, costs and damages relating to or arising from the overclocking
 * of AMD processors.
 *
 ******************************************************************************
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "Porting.h"
#include "AMD.h"
#include <Library/IdsLib.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Ppi/AmdCoreTopologyServicesPpi.h>
#include "Filecode.h"
#include "CcxZenZpDownCoreInit.h"
#include <Library/Iolib.h>

#define FILECODE CCX_ZEN_CCXZENZPPEI_CCXZENZPDOWNCOREINIT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
typedef enum {
  CCX_DOWN_CORE_AUTO = 0,       ///< Auto
  CCX_DOWN_CORE_1_0,            ///< 1 + 0 mode
  CCX_DOWN_CORE_1_1,            ///< 1 + 1 mode
  CCX_DOWN_CORE_2_0,            ///< 2 + 0 mode
  CCX_DOWN_CORE_3_0,            ///< 3 + 0 mode
  CCX_DOWN_CORE_2_2,            ///< 2 + 2 mode
  CCX_DOWN_CORE_4_0,            ///< 4 + 0 mode
  CCX_DOWN_CORE_3_3,            ///< 3 + 3 mode
  CCX_DOWN_CORE_MAX             ///< Used for bounds checking
} CCX_DOWN_CORE_MODE;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
CcxZenZpDownCoreAfterSmuServicesCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
EFI_PEI_NOTIFY_DESCRIPTOR   mAfterSmuServicesPpiCallback = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioSmuServicesPpiGuid,
  CcxZenZpDownCoreAfterSmuServicesCallback
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
 *  FabricClockGatingInit
 *
 *  Description:
 *    This funtion initializes the DF clock gating feature.
 *
 */
VOID
CcxZenZpDownCoreInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  UINT32    RegEbx;
  UINT8     PwrResetCfg;
    
  if ((PcdGet8 (PcdAmdDownCoreMode) != 0) || (PcdGet8 (PcdAmdSmtMode) != 1)) {
    (**PeiServices).NotifyPpi (PeiServices, &mAfterSmuServicesPpiCallback);
  }
 
  AsmCpuid(0x8000001E, NULL, &RegEbx, NULL, NULL);  
  IDS_HDT_CONSOLE (CPU_TRACE, "  SMT mode %x  RegEbx %x\n",PcdGet8 (PcdAmdSmtMode),RegEbx);
  if ((PcdGet8 (PcdAmdSmtMode)== 1) && (((RegEbx >> 8) & 0xFF) == 0x0))
  {
      IDS_HDT_CONSOLE(CPU_TRACE, "Cold Reset to re-enable SMT.\n");  
      IoWrite8(0xCD6,0x10);// PwrResetCfg
      PwrResetCfg = IoRead8(0xCD7);
      PwrResetCfg = PwrResetCfg | BIT1;
      IoWrite8(0xCD7,PwrResetCfg);
      IoWrite8(0xCF9,0x02);// HARDSTARTSTATE
      IoWrite8(0xCF9,0x06);// HARDRESET
  }
}

EFI_STATUS
EFIAPI
CcxZenZpDownCoreAfterSmuServicesCallback (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDesc,
  IN  VOID                            *InvokePpi
  )
{
  UINT8                                 DownCoreMode;
  UINT8                                 SmtMode;
  UINT32                                CoreDisFuse;
  UINTN                                 i;
  UINTN                                 j;
  UINTN                                 ComplexCount;
  UINTN                                 CoreCount;
  UINTN                                 ThreadCount;
  UINTN                                 DieCount;
  UINTN                                 SystemDieCount;
  UINTN                                 SocketCount;
  UINTN                                 DesiredCoreCount;
  UINTN                                 DesiredComplexCount;
  UINT32                                SystemDieLoop;
  UINT32                                CoreDisByFuseCount;
  UINT32                                CoreDisMask;
  BOOLEAN                               IssueReset;
  BOOLEAN                               Attainable;
  BOOLEAN                               Met;
  BOOLEAN                               AlreadyDownCored;
  EFI_STATUS                            Status;
  EFI_STATUS                            CalledStatus;
  PEI_AMD_NBIO_SMU_SERVICES_PPI         *NbioSmuServices;
  AMD_CORE_TOPOLOGY_SERVICES_PPI        *CoreTopologyServices;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServices;

  AGESA_TESTPOINT (TpCcxPeiDownCoreCallbackEntry, NULL);

  Status = EFI_SUCCESS;
  IssueReset = FALSE;
  DownCoreMode = PcdGet8 (PcdAmdDownCoreMode);
  SmtMode = PcdGet8 (PcdAmdSmtMode);
  ASSERT (DownCoreMode < CCX_DOWN_CORE_MAX);
  ASSERT (SmtMode <= 1);

  CalledStatus = (*PeiServices)->LocatePpi (PeiServices, &gAmdNbioSmuServicesPpiGuid, 0, NULL, &NbioSmuServices);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  CalledStatus = (*PeiServices)->LocatePpi (PeiServices, &gAmdFabricTopologyServicesPpiGuid, 0, NULL, &FabricTopologyServices);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  CalledStatus = (*PeiServices)->LocatePpi (PeiServices, &gAmdCoreTopologyServicesPpiGuid, 0, NULL, &CoreTopologyServices);
  Status = (CalledStatus > Status) ? CalledStatus : Status;

  FabricTopologyServices->GetSystemInfo (&SocketCount, &SystemDieCount, NULL);

  if (DownCoreMode != 0) {
    CoreDisMask = 0;
    switch (DownCoreMode) {
    case CCX_DOWN_CORE_1_0:
      IDS_HDT_CONSOLE (CPU_TRACE, "  Downcore mode (1+0) not supported.  No downcoring will take place.\n");
      // Make attainability an impossibility
      DesiredCoreCount = 0xFFFFFFFF;
      DesiredComplexCount = 0xFFFFFFFF;
      break;
    case CCX_DOWN_CORE_2_0:
      DesiredCoreCount = 2;
      DesiredComplexCount = 1;
      CoreDisMask = 0x000000FC;
      break;
    case CCX_DOWN_CORE_3_0:
      DesiredCoreCount = 3;
      DesiredComplexCount = 1;
      CoreDisMask = 0x000000F8;
      break;
    case CCX_DOWN_CORE_4_0:
      DesiredCoreCount = 4;
      DesiredComplexCount = 1;
      CoreDisMask = 0x000000F0;
      break;
    case CCX_DOWN_CORE_1_1:
      DesiredCoreCount = 1;
      DesiredComplexCount = 2;
      CoreDisMask = 0x000000EE;
      break;
    case CCX_DOWN_CORE_2_2:
      DesiredCoreCount = 2;
      DesiredComplexCount = 2;
      CoreDisMask = 0x000000CC;
      break;
    case CCX_DOWN_CORE_3_3:
      DesiredCoreCount = 3;
      DesiredComplexCount = 2;
      CoreDisMask = 0x00000088;
      break;
    default:
      // Make attainability an impossibility
      DesiredCoreCount = 0xFFFFFFFF;
      DesiredComplexCount = 0xFFFFFFFF;
      break;
    }

    Attainable = TRUE;
    Met = TRUE;
    AlreadyDownCored = FALSE;
    SystemDieLoop = 0;
    CoreDisByFuseCount = 0;
    for (i = 0; (i < SocketCount) && Attainable; i++) {
      FabricTopologyServices->GetProcessorInfo (i, &DieCount, NULL, NULL, NULL);
      for (j = 0; (j < DieCount) && Attainable; j++) {
        // Get CoreDis FUSE
        NbioSmuServices->SmuRegisterRead (NbioSmuServices, SystemDieLoop, 0x5D25C, &CoreDisFuse);
        CoreDisFuse &= 0xFF; // bits[7:0]
        while (CoreDisFuse != 0) {
          if ((CoreDisFuse & 1) == 1) {
            CoreDisByFuseCount++;
          }
          CoreDisFuse = CoreDisFuse >> 1;
        }

        // Get actually core count and complex count
        CoreTopologyServices->GetCoreTopologyOnDie (CoreTopologyServices, i, j, &ComplexCount, &CoreCount, &ThreadCount);
        IDS_HDT_CONSOLE (CPU_TRACE, "  CoreDisByFuseCount %X\n", CoreDisByFuseCount);
        IDS_HDT_CONSOLE (CPU_TRACE, "  ComplexCount %X; CoreCount %X\n", ComplexCount, CoreCount);

        // Check if already down cored
        if (((CoreCount * ComplexCount) + CoreDisByFuseCount) < 8) {
          AlreadyDownCored = TRUE;
          IDS_HDT_CONSOLE (CPU_TRACE, "  Already down cored, skip down core function\n");
        }
        if ((DesiredCoreCount > CoreCount) || (DesiredComplexCount > ComplexCount)) {
          Attainable = FALSE;
        }
        if ((DesiredCoreCount != CoreCount) || (DesiredComplexCount != ComplexCount)) {
          Met = FALSE;
        }

        SystemDieLoop++;
      }
    }


    if (Attainable && !Met && !AlreadyDownCored) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Set down core register %x\n", CoreDisMask);
      NbioSmuServices->SmuSetDownCoreRegister (NbioSmuServices, CoreDisMask);
      IssueReset = TRUE;
    }
  }

  if (SmtMode == 0) {
    CoreTopologyServices->GetCoreTopologyOnDie (CoreTopologyServices, 0, 0, &ComplexCount, &CoreCount, &ThreadCount);
    if (ThreadCount > 1) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  Disabling SMT\n");
      NbioSmuServices->SmuDisableSmt (NbioSmuServices);
      IssueReset = TRUE;
    }
  }

  if (IssueReset) {
    IDS_HDT_CONSOLE (CPU_TRACE, "  Issuing warm reset\n");
    (**PeiServices).ResetSystem (PeiServices);
  }

  AGESA_TESTPOINT (TpCcxPeiDownCoreCallbackExit, NULL);

  return Status;
}

