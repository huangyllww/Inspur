/* $NoKeywords:$ */
/**
 * @file
 *
 * Implement Custom Core Pstates
 *
 * Contains code that Custom Core Pstates
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Library
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

#include "AMD.h"
#include "PiDxe.h"
#include <Library/BaseLib.h>
#include <Library/IdsLib.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/CcxPstatesLib.h>
#include <Library/AmdPspApobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CcxMpServicesLib.h>
#include <Library/CcxIdsCustomPstatesLib.h>
#include <Protocol/AmdCoreTopologyProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/AmdNbioSmuServicesProtocol.h>
#include "Filecode.h"

#define FILECODE LIBRARY_CCXIDSCUSTOMPSTATESZENLIB_CCXIDSCUSTOMPSTATESZENLIB_FILECODE


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

VOID
SwitchPstateOnAllCores (
  IN       CCX_SWITCH_PSTATE *SwitchPstate
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 * CcxZenZpIdsSetPstateMsrs - routine to update Pstate MSRs to specified values
 *
 *
 * @param[in]  *CustomPstates  Pointer to data for Custom Pstates
 *
 *
 */
VOID
CcxIdsCustomPstateSetMsrLib (
  IN       IDS_CUSTOM_CORE_PSTATE  *CustomPstates
  )
{
  UINT8                   i;
  UINT8                   CurPstate;
  UINT8                   PreVid;
  UINT8                   CurrentVid;
  UINT32                  MaxPstate;
  UINT32                  FreqAndVoltage;
  UINT32                  ApobInstanceId;
  UINT32                  PreFreq;
  UINT32                  CurrentFreq;
  UINTN                   SocketLoop;
  UINTN                   DieLoop;
  UINTN                   ComplexLoop;
  UINTN                   CoreLoop;
  UINTN                   NumberOfSockets;
  UINTN                   NumberOfSystemDies;
  UINTN                   NumberOfDies;
  UINTN                   NumberOfComplexes;
  UINTN                   NumberOfCores;
  UINTN                   SystemDieCount;
  BOOLEAN                 Modified;
  IDS_PSTATE_MSR          PstateMsr;
  IDS_PSTATE_CURLIM_MSR   PstateCurLim;
  CCX_SWITCH_PSTATE       SwitchPstate;
  DXE_AMD_NBIO_SMU_SERVICES_PROTOCOL            *NbioSmuServices;
  AMD_CORE_TOPOLOGY_SERVICES_PROTOCOL           *CoreTopologyServices;
  AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL         *FabricTopologyServices;
  APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE_STRUCT   ApobCcxLogToPhysMap;
  APOB_TYPE_HEADER                              *ApobEntry;

  SwitchPstate.WaitForCompletion = TRUE;

  // Check custom core Pstate settings
  PreFreq = 0xFFFFFFFF;
  PreVid = 0;
  for (i = 0; i <= (PS_MAX_REG - PS_MIN_REG); i++) {
    if (CustomPstates[i].Custom == CCX_IDS_CORE_PSTATES_DISABLED) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  P%X - Disable\n", i);
      break;
    } else if (CustomPstates[i].Custom == CCX_IDS_CORE_PSTATES_CUSTOM) {
      ASSERT (CustomPstates[i].CoreDid != 0);
      CurrentFreq = (UINT32) (DivU64x32 (MultU64x64 (200, CustomPstates[i].CoreFid), CustomPstates[i].CoreDid));
      CurrentVid = CustomPstates[i].CoreVid;
      IDS_HDT_CONSOLE (CPU_TRACE, "  P%X - Custom, Frequency: %dMHz  FID-0x%X DID-0x%X VID-0x%X\n", i, CurrentFreq, CustomPstates[i].CoreFid, CustomPstates[i].CoreDid, CustomPstates[i].CoreVid);
    } else {
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + i);
      if (PstateMsr.Field.PstateEn == 0) {
        break;
      }
      CurrentFreq = (UINT32) (DivU64x32 (MultU64x64 (200, PstateMsr.Field.CpuFid_7_0), (UINT32) PstateMsr.Field.CpuDfsId));
      CurrentVid = (UINT8) PstateMsr.Field.CpuVid;
      IDS_HDT_CONSOLE (CPU_TRACE, "  P%X - Auto,   Frequency: %dMHz  FID-0x%X DID-0x%X VID-0x%X\n", i, CurrentFreq, PstateMsr.Field.CpuFid_7_0, PstateMsr.Field.CpuDfsId, PstateMsr.Field.CpuVid);
    }

    if (PreFreq < CurrentFreq) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  ERROR: P%X frequency > P%X frequency\n", i, (i - 1));
      ASSERT (FALSE);
      return;
    }
    if (PreVid > CurrentVid) {
      IDS_HDT_CONSOLE (CPU_TRACE, "  ERROR: P%X VID < P%X VID\n", i, (i - 1));
      ASSERT (FALSE);
      return;
    }
    PreFreq = CurrentFreq;
    PreVid = CurrentVid;
  }

  gBS->LocateProtocol (&gAmdNbioSmuServicesProtocolGuid, NULL, &NbioSmuServices);
  gBS->LocateProtocol (&gAmdFabricTopologyServicesProtocolGuid, NULL, &FabricTopologyServices);
  gBS->LocateProtocol (&gAmdCoreTopologyServicesProtocolGuid, NULL, &CoreTopologyServices);

  MaxPstate = 0;
  Modified = FALSE;

  if (FabricTopologyServices->GetSystemInfo (FabricTopologyServices, &NumberOfSockets, &NumberOfSystemDies, NULL) != EFI_SUCCESS) {
    return;
  }

  // Get Current Pstate limit
  PstateCurLim.Value = AsmReadMsr64 (MSR_PSTATE_CURRENT_LIMIT);

  for (i = 0; i <= (PS_MAX_REG - PS_MIN_REG); i++) {
    // Disable
    if (CustomPstates[i].Custom == CCX_IDS_CORE_PSTATES_DISABLED) {
      // We can't disable P0
      ASSERT (i > 0);

      // if Pstate[i] is enabled, then disable it and all lower Pstates
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + i);
      if (PstateMsr.Field.PstateEn == 1) {
        Modified = TRUE;
        CurPstate = CcxGetCurrentPstate ();
        if (CurPstate >= i) {
          SwitchPstate.Pstate = i - 1;
          SwitchPstateOnAllCores (&SwitchPstate);
        }
        while (i <= (PS_MAX_REG - PS_MIN_REG)) {
          for (DieLoop = 0; DieLoop < NumberOfSystemDies; DieLoop++) {
            NbioSmuServices->AmdSmuRegisterRMW (NbioSmuServices, (UINT32) DieLoop, (0x5A824 + i * 8), 0x7FFFFFFF, 0);
          }
          i++;
        }
      }
      break;
    }

    // Custom
    if (CustomPstates[i].Custom == CCX_IDS_CORE_PSTATES_CUSTOM) {
      if (CustomPstates[i].CoreVid >= 0xF8) {
        PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + i);
        if (PstateMsr.Field.PstateEn == 1) {
          continue;
        } else {
          break;
        }
      }
      Modified = TRUE;
      CurPstate = CcxGetCurrentPstate ();
      if (CurPstate == i) {
        // Current Pstate == i, switch to above Pstate or PStateMaxVal
        if (i > 0) {
          SwitchPstate.Pstate = i - 1;
          SwitchPstateOnAllCores (&SwitchPstate);
        } else {
          SwitchPstate.Pstate = PstateCurLim.Field.PstateMaxVal;
          SwitchPstateOnAllCores (&SwitchPstate);
        }
      }
      // Customize this Pstate
      FreqAndVoltage = CustomPstates[i].CoreFid |
                       (CustomPstates[i].CoreDid << 8) |
                       (CustomPstates[i].CoreVid << 14);
      for (DieLoop = 0; DieLoop < NumberOfSystemDies; DieLoop++) {
        NbioSmuServices->AmdSmuRegisterRMW (NbioSmuServices, (UINT32) DieLoop, (0x5A820 + i * 8), 0xFFC00000, FreqAndVoltage);
        NbioSmuServices->AmdSmuRegisterRMW (NbioSmuServices, (UINT32) DieLoop, (0x5A824 + i * 8), 0x7FFFFFFF, 0x80000000);
      }

      // Switch back
      if (CurPstate == i) {
        SwitchPstate.Pstate = CurPstate;
        SwitchPstateOnAllCores (&SwitchPstate);
      }

      MaxPstate++;
    }

    // Auto
    if (CustomPstates[i].Custom == CCX_IDS_CORE_PSTATES_AUTO) {
      // If this Pstate is disabled, and user didn't customize it, just break
      PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + i);
      if (PstateMsr.Field.PstateEn == 0) {
        break;
      } else {
        MaxPstate++;
      }
    }
  }

  // If no registers were modified then return
  if (!Modified) {
    return;
  }

  // MaxPstate is 0 based
  MaxPstate--;
  ASSERT (MaxPstate < 8);

  //
  // Update PstateMaxVal and CurPstateLimit
  //

  SystemDieCount = 0;
  for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) {
    if (FabricTopologyServices->GetProcessorInfo (FabricTopologyServices, SocketLoop, &NumberOfDies, NULL, NULL, NULL) != EFI_SUCCESS) {
      return;
    }
    for (DieLoop = 0; DieLoop < NumberOfDies; DieLoop++) {
      SystemDieCount++;
      if (CoreTopologyServices->GetCoreTopologyOnDie (CoreTopologyServices, SocketLoop, DieLoop, &NumberOfComplexes, &NumberOfCores, NULL) != EFI_SUCCESS) {
        return;
      }

      ApobInstanceId = ((UINT32) SocketLoop << 8) | (UINT32) DieLoop;
      if (AmdPspGetApobEntryInstance (APOB_CCX, APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE, ApobInstanceId, FALSE, &ApobEntry) != EFI_SUCCESS) {
        return;
      }
      CopyMem (&ApobCcxLogToPhysMap, ApobEntry, sizeof (APOB_CCX_LOGICAL_TO_PHYSICAL_MAP_TYPE_STRUCT));

      for (ComplexLoop = 0; ComplexLoop < NumberOfComplexes; ComplexLoop++) {
        if (ApobCcxLogToPhysMap.ComplexMap[ComplexLoop].PhysComplexNumber == CCX_NOT_PRESENT) {
          return;
        }
        for (CoreLoop = 0; CoreLoop < NumberOfCores; CoreLoop++) {
          if (ApobCcxLogToPhysMap.ComplexMap[ComplexLoop].CoreInfo[CoreLoop].PhysCoreNumber  == CCX_NOT_PRESENT) {
            return;
          }
          NbioSmuServices->AmdSmuRegisterRMW (NbioSmuServices, (UINT32) (SystemDieCount - 1), 0x180010BC | (((UINT32) ApobCcxLogToPhysMap.ComplexMap[ComplexLoop].PhysComplexNumber) << 22) | (((UINT32) ApobCcxLogToPhysMap.ComplexMap[ComplexLoop].CoreInfo[CoreLoop].PhysCoreNumber) << 17), 0xFFFFFF88, ((MaxPstate << 4) | MaxPstate));
        }
      }
    }
  }

  return;
}


/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 * SwitchPstateOnAllCores - routine to switch Pstates on all cores.
 *
 *
 * @param[in]  *SwitchPstate  Pointer to struct with data for Pstate transition
 *
 *
 */
VOID
SwitchPstateOnAllCores (
  IN       CCX_SWITCH_PSTATE *SwitchPstate
  )
{
  SwitchPstate->WaitForCompletion = FALSE;
  CcxRunFunctionOnAps (ALL_THREADS, CcxSwitchPstate, SwitchPstate, NON_BLOCKING_MODE);
  SwitchPstate->WaitForCompletion = TRUE;
  CcxSwitchPstate (SwitchPstate);
}
