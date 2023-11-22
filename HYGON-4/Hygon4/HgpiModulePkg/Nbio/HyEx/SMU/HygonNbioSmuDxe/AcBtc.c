/* $NoKeywords:$ */

/**
 * @file
 *
 * AcBtc Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  AcBtc
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
#include <PiDxe.h>
#include <Filecode.h>
#include <HygonNbioSmuDxe.h>
#include <GnbRegisters.h>
#include <CddRegistersDm.h>
#include <Library/HygonBaseLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IdsLib.h>
#include <Protocol/MpService.h>
#include <Library/CcxPstatesLib.h>
#include <Library/SynchronizationLib.h>
#include <Library/NbioSmuLib.h>
#include <Library/GnbLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/CcxPstatesLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/BaseFabricTopologyLib.h>
#include "AcBtc.h"

#define FILECODE  NBIO_SMU_HYEX_HYGONNBIOSMUDXE_ACBTC_FILECODE

VOLATILE STATIC UINT64  ApReadyCount;
VOLATILE STATIC UINT64  ApTaskToRun;
VOLATILE STATIC UINT64  TscP0Count2mS;
STATIC EFI_EVENT        HygonBtcEvent;
VOID                    *WorkloadBuffer;

extern
VOID
ExecuteF18Btc (
  IN       UINT64  CalPgmIndex,
  IN       UINT64  Scratch    ///< Pointer to work buffer
  );

extern
VOID
ExecuteHalt (
  IN       UINT64  CalPgmIndex,
  IN       UINT64  Scratch    ///< Pointer to work buffer
  );

extern
VOID
SynchronizeAllApsF18 (
  IN       UINT64   IncrementAddress,
  IN       UINT64   MonitorAddress
  );

/* -----------------------------------------------------------------------------*/

/**
 *
 *  MiscBtcAps
 *
 *  Description:
 *    This routine stop all APs
 *
 */
VOID
RunBtcOnAps (
  )
{
  ACBTC_WORKLOAD_STRUCT  *LocalWorkload;
  CPUID_DATA             Cpuid;
  UINT32                 CcxNumber;
  UINT32                 Index;

  for (Index = 0; Index < 3; Index++) {
    SynchronizeAllApsF18 ((UINT64)&ApReadyCount, (UINT64)&ApTaskToRun);

    AsmCpuid (
      0x00000001,
      &(Cpuid.EAX_Reg),
      &(Cpuid.EBX_Reg),
      &(Cpuid.ECX_Reg),
      &(Cpuid.EDX_Reg)
      );
    LocalWorkload = WorkloadBuffer;
    CcxNumber = (Cpuid.EBX_Reg >> 27) & 0xF;
    while (CcxNumber != 0) {
      // Point to next workload buffer space
      LocalWorkload++;
      // 64 byte align the workload buffer space
      LocalWorkload = (VOID *)(((UINT64)LocalWorkload + 63) & 0xFFFFFFFFFFFFFFC0);
      CcxNumber--;
    }
    ((ACBTC_WORKLOAD_STRUCT *) LocalWorkload)->tscWaitCount = TscP0Count2mS;
    while (Index != ApTaskToRun) {}
    ExecuteF18Btc ((UINT64)ApTaskToRun, (UINT64)LocalWorkload);
  }
}


/* -----------------------------------------------------------------------------*/
/**
 *
 *  BeforeBtc
 *
 *  Description:
 *    This routine execute before BTC
 *
 */
EFI_STATUS
EFIAPI
StartBtc (
  )
{
  EFI_STATUS                Status;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Create a event so StartupAllAPs executes in non-blocking mode.
  Status = gBS->CreateEvent (
                  0x00000000,
                  0,
                  NULL,
                  NULL,
                  &HygonBtcEvent
                  );
  if (EFI_ERROR (Status)) {
    return HGPI_ERROR;
  }

  Status = MpServices->StartupAllAPs (
                         MpServices,
                         (EFI_AP_PROCEDURE)RunBtcOnAps,
                         FALSE,
                         HygonBtcEvent,
                         0,
                         NULL,
                         NULL
                         );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
 *---------------------------------------------------------------------------------------
 *  CallbackAcBtc
 *
 *  Description:
 *     notification event handler after MpService Protocol ready
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
CallbackAcBtc (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  EFI_STATUS                            Status;
  PCIe_PLATFORM_CONFIG                  *PcieFromHob;
  PCIe_PLATFORM_CONFIG                  *Pcie;
  GNB_HANDLE                            *NbioHandle;
  UINT32                                SmuArg[6];
  DXE_HYGON_NBIO_SMU_SERVICES_PROTOCOL  *NbioSmuServiceProtocol;
  UINT32                                AcBtcIndex;
  EFI_MP_SERVICES_PROTOCOL              *MpService;
  UINT32                                SmuResult;
  UINT32                                WorkloadBufferSize;
  UINT64                                WorkloadBufferAligned;
  ACBTC_WORKLOAD_STRUCT                 *BufferPtr;
  UINT8                                 Index;
  UINT32                                NumberOfComplexes;
  UINT32                                NumberOfThreads;
  UINT32                                TotalThreads;
  UINTN                                 Frequency;
  UINTN                                 VoltageInuV;
  UINTN                                 PowerInmW;
  THREAD_ENABLE_STRUCT                  ThreadEnable;
  THREAD_CONFIGURATION_STRUCT           ThreadConfiguration;
  UINT64                                ActiveThreadMap;
  UINT8                                 CoreThreads;
  UINT8                                 ThreadBit;
  UINTN                                 CddsPresent;
  UINTN                                 CddIndex;
  UINTN                                 CddId;

  IDS_HDT_CONSOLE (MAIN_FLOW, "Callback Ac Btc First notify\n");

  // Frequency      The P-State's frequency in MegaHertz
  if (TRUE == CcxGetPstateInfo (0, 0, &Frequency, &VoltageInuV, &PowerInmW, (HYGON_CONFIG_PARAMS *)NULL)) {
    TscP0Count2mS = Frequency * 2000;
  }

  //
  // Add more check to locate protocol after got event, because
  // the library will signal this event immediately once it is register
  // just in case it is already installed.
  //
  Status = gBS->LocateProtocol (
                  &gEfiMpServiceProtocolGuid,
                  NULL,
                  &MpService
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "Callback Ac Btc Start\n");

  SmuDxeInitialize (&PcieFromHob);
  Pcie = PcieFromHob;

  Status = gBS->LocateProtocol (
                  &gHygonNbioSmuServicesProtocolGuid,
                  NULL,
                  &NbioSmuServiceProtocol
                  );
  IDS_HDT_CONSOLE (MAIN_FLOW, "Status = 0x%x\n", Status);

  NumberOfComplexes = 0;
  NumberOfThreads   = 0;
  TotalThreads = 0;
  NbioHandle   = NbioGetHandle (Pcie);
  while (NbioHandle != NULL) {
    CddsPresent = FabricTopologyGetCddsPresentOnSocket (NbioHandle->SocketId);
    for (CddIndex = 0; CddIndex < MAX_CDDS_PER_SOCKET; CddIndex++) {
      if (!IS_CDD_PRESENT (CddIndex, CddsPresent)) {
        continue;
      }

      NbioRegisterRead (
        NbioHandle,
        THREAD_CONFIGURATION_TYPE,
        (UINT32)CDD_SPACE (CddIndex, THREAD_CONFIGURATION_ADDRESS_HYEX),
        &ThreadConfiguration,
        0
        );

      NumberOfComplexes += ThreadConfiguration.HyExField.ComplexCount + 1;
      NumberOfThreads    = (ThreadConfiguration.HyExField.SMTMode == 0) ? 2 : 1;
      TotalThreads      += (ThreadConfiguration.HyExField.ComplexCount + 1) * (ThreadConfiguration.HyExField.CoreCount + 1) * NumberOfThreads;
    }
    IDS_HDT_CONSOLE (MAIN_FLOW, "TotalThreads = 0x%x\n", TotalThreads);
    NbioHandle = GnbGetNextSocketHandle (NbioHandle);
  }
 
  if (PcdGet8 (PcdAcBtc)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "AcBtc \n");
    /// Need to allocate workload buffer - minimum size is 0x931 qwords or 18824 bytes
    //
    // Step14. Allocate DRAM
    //
    WorkloadBufferSize = (sizeof (ACBTC_WORKLOAD_STRUCT) + 64) * NumberOfComplexes;
    Status = gBS->AllocatePool (
                    EfiRuntimeServicesData,
                    WorkloadBufferSize,
                    &WorkloadBuffer
                    );
    IDS_HDT_CONSOLE (MAIN_FLOW, "Allocate Pool Status = 0x%x\n", Status);
    WorkloadBufferAligned = (UINT64)WorkloadBuffer;
    WorkloadBufferAligned = (WorkloadBufferAligned + 63) & 0xFFFFFFFFFFFFFFC0;
    WorkloadBuffer = (VOID *)WorkloadBufferAligned;

    ApReadyCount = 0;
    ApTaskToRun  = 0xFF;
    IDS_HDT_CONSOLE (MAIN_FLOW, "Before ApReadyCount = 0x%x\n", ApReadyCount);
    // Step1: Start all APs running.  The first thing they should do is a call to a MONITOR/MWWAIT function. On return from the function they will execute the workload function.
    StartBtc ();

    for (AcBtcIndex = 0; AcBtcIndex < 3; AcBtcIndex++) {
      // Initialize the ApReadyCount.

      // Wait for all APs to reach MWAIT...
      // while (ApReadyCount < NumberOfAPsToTest) {}
      while (ApReadyCount < (TotalThreads - 1)) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "ApReadyCount = 0x%x\n", ApReadyCount);
      }

      IDS_HDT_CONSOLE (MAIN_FLOW, "ApReadyCount = 0x%x\n", ApReadyCount);

      // Renitialize the ApReadyCount because APs will return to increment it after workload execution
      ApReadyCount = 0;
      IDS_HDT_CONSOLE (MAIN_FLOW, "ApReadyCount = 0x%x\n", ApReadyCount);

      // Initialize SynchLine for all buffers
      ActiveThreadMap = 0xBEEFBEEFBEEFBEEFull;
      NbioHandle = NbioGetHandle (Pcie);
      CddId = FabricTopologyGetFirstPhysCddIdOnSocket (NbioHandle->SocketId);
      NbioRegisterRead (
        NbioHandle,
        THREAD_ENABLE_TYPE,
        (UINT32)CDD_SPACE (CddId, THREAD_ENABLE_ADDRESS_HYEX),
        &ThreadEnable,
        0
        );

      CoreThreads = (UINT8)(ThreadEnable.Field.ThreadEn & 0xFF);
      if (CoreThreads != 0) {
        ActiveThreadMap = 0xBEEFBEEFBEEFBEEFull;
        ThreadBit = 0x80;
        while ((ThreadBit & CoreThreads) == 0) {
          ActiveThreadMap = RShiftU64 (ActiveThreadMap, 8);
          ThreadBit = ThreadBit >> 1;
        }
      }

      BufferPtr = (ACBTC_WORKLOAD_STRUCT *)WorkloadBuffer;
      for (Index = 0; Index < NumberOfComplexes; Index++) {
        BufferPtr->SynchLine = ActiveThreadMap;
        BufferPtr++;
        BufferPtr = (VOID *)(((UINT64)BufferPtr + 63) & 0xFFFFFFFFFFFFFFC0);
      }

      // Call BIOSSMC_MSG_AcBtcStartCal [ARG = i]
      NbioHandle = NbioGetHandle (Pcie);
      NbioSmuServiceCommonInitArguments (SmuArg);
      SmuArg[0] = AcBtcIndex;
      SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_AcBtcStartCal, SmuArg, 0);

      // Write to monitored address to start APs executing workload.
      // Assembly code on APs will exit MWAIT when this write occurs
      ApTaskToRun = AcBtcIndex;

      // Execute workload code on BSP
      ExecuteF18Btc ((UINT64)AcBtcIndex, (UINT64)WorkloadBuffer);

      NbioSmuServiceCommonInitArguments (SmuArg);
      SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_AcBtcStopCal, SmuArg, 0);
    }

    NbioHandle = NbioGetHandle (Pcie);
    NbioSmuServiceCommonInitArguments (SmuArg);
    SmuResult = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_AcBtcEndCal, SmuArg, 0);
    //
    // Step 5. Release all others AP.
    // AfterBtc();  ////EGH I am not sure what this is supposed to  - probably same as CZ - transition to Pstate 0 and IdleAllAps
  }

  if (PcdGet8 (PcdDcBtc)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "DcBtc \n");

    NbioHandle = NbioGetHandle (Pcie);
    NbioSmuServiceCommonInitArguments (SmuArg);
    Status = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_DcBtc, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
  }

  if ((PcdGet32 (PcdSmuFeatureControlDefines) & (FEATURE_CC1_MASK + FEATURE_CC6_MASK)) != 0) {
    NbioHandle = NbioGetHandle (Pcie);
    while (NbioHandle != NULL) {
      //
      // Update PcdSmuFeatureControlDefines value from CBS.
      //
      SmuArg[0] = PcdGet32 (PcdSmuFeatureControlDefines) & (FEATURE_CC1_MASK + FEATURE_CC6_MASK);
      Status    = NbioSmuServiceRequest (NbioGetHostPciAddress (NbioHandle), 0, SMC_MSG_EnableSmuFeatures, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
      IDS_HDT_CONSOLE (MAIN_FLOW, "AllSmuFeatureControlDefines = 0x%x\n", SmuArg[0]);
      IDS_HDT_CONSOLE (MAIN_FLOW, "Status = 0x%x\n", Status);

      NbioHandle = GnbGetNextSocketHandle (NbioHandle);
    }
  }

  // Close event, so it will not be invoked again.
  //
  gBS->CloseEvent (Event);

  IDS_HDT_CONSOLE (MAIN_FLOW, "Callback Ac Btc End\n");
}
