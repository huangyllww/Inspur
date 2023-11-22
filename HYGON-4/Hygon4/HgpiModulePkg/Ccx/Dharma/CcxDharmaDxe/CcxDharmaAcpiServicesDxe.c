/* $NoKeywords:$ */

/**
 * @file
 *
 * Gather Pstate Data Services.
 *
 * Contains code that Pstate Gather Data
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
#include <Library/BaseLib.h>
#include <Library/CcxBaseX86Lib.h>
#include <HGPI.h>
#include <Filecode.h>
#include <cpuRegisters.h>
#include <CcxRegistersDm.h>
#include <Library/HygonBaseLib.h>
#include <Library/HygonHeapLib.h>
#include <Library/CcxPstatesLib.h>
#include <Library/GnbLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Protocol/SocLogicalIdProtocol.h>
#include <Protocol/HygonAcpiCpuSsdtServicesProtocol.h>
#include <Protocol/HygonAcpiCratServicesProtocol.h>
#include <Protocol/HygonAcpiSratServicesProtocol.h>
#include <Protocol/HygonCoreTopologyProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>

#define FILECODE  CCX_DHARMA_CCXDHARMADXE_CCXDHARMAACPISERVICESDXE_FILECODE

/// TLB type
typedef enum {
  TLB_2M = 0,       ///< 0 - TLB 2M4M associativity
  TLB_4K,           ///< 1 - TLB 4K associativity
  TLB_1G,           ///< 2 - TLB 1G associativity
  TLB_TYPE_MAX,     ///< MaxValue
} TLB_TYPE;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
CcxDharmaPStateGatherData (
  IN       HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL    *This,
  OUT      HYGON_PSTATE_SYS_INFO                   **PstateSysInfoPtr
  );

HGPI_STATUS
CcxDharmaGetPstateTransLatency (
  OUT      UINT32                                 *TransitionLatency,
  IN       HYGON_CONFIG_PARAMS                      *StdHeader
  );

EFI_STATUS
EFIAPI
CcxDharmaGetCStateInfo (
  IN       HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL    *This,
  OUT      HYGON_CSTATE_INFO                       **CstateInfo
  );

UINT32
EFIAPI
CcxDharmaGetPsdDomain (
  IN       HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL    *This,
  IN       UINT8                                   LocalApicId
  );

EFI_STATUS
EFIAPI
CcxDharmaGetCratHsaProcEntry (
  IN       HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL    *This,
  IN       CRAT_HEADER                            *CratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  );

EFI_STATUS
EFIAPI
CcxDharmaGetCratCacheEntry (
  IN       HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL    *This,
  IN       CRAT_HEADER                            *CratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  );

EFI_STATUS
EFIAPI
CcxDharmaGetCratTlbEntry (
  IN       HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL    *This,
  IN       CRAT_HEADER                            *CratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  );

EFI_STATUS
EFIAPI
CcxDharmaCreateSratLapic (
  IN       HYGON_CCX_ACPI_SRAT_SERVICES_PROTOCOL    *This,
  IN       SRAT_HEADER                            *SratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  );

EFI_STATUS
EFIAPI
CcxDharmaCreateSratX2Apic (
  IN       HYGON_CCX_ACPI_SRAT_SERVICES_PROTOCOL    *This,
  IN       SRAT_HEADER                            *SratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  );

UINT32
STATIC
CcxDharmaCalcLocalApic (
  IN       UINTN    Socket,
  IN       UINTN    Cdd,
  IN       UINTN    Complex,
  IN       UINTN    Core,
  IN       UINTN    Thread
  );

UINT8 *
AddOneCratEntry (
  IN       CRAT_ENTRY_TYPE    CratEntryType,
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );

UINT8
GetCacheAssoc (
  IN       UINT16   RawAssoc
  );

UINT8
GetTlbSize (
  IN       TLB_TYPE   TLB_TYPE,
  IN       CRAT_TLB  *CratTlbEntry,
  IN       UINT16     RawAssocSize
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
STATIC HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL  mDharmaSsdtServicesProtocol = {
  0,
  CcxDharmaPStateGatherData,
  CcxDharmaGetCStateInfo,
  CcxDharmaGetPsdDomain
};

STATIC HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL  mDharmaCratServicesProtocol = {
  0,
  CcxDharmaGetCratHsaProcEntry,
  CcxDharmaGetCratCacheEntry,
  CcxDharmaGetCratTlbEntry
};

STATIC HYGON_CCX_ACPI_SRAT_SERVICES_PROTOCOL  mDharmaSratServicesProtocol = {
  1,
  CcxDharmaCreateSratLapic,
  CcxDharmaCreateSratX2Apic
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
CcxDharmaAcpiCpuSsdtServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  // Install ACPI CPU SSDT services protocol
  return gBS->InstallProtocolInterface (
                &ImageHandle,
                &gHygonAcpiCpuSsdtServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDharmaSsdtServicesProtocol
                );
}

EFI_STATUS
EFIAPI
CcxDharmaCratServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  // Install ACPI CPU CRAT services protocol
  return gBS->InstallProtocolInterface (
                &ImageHandle,
                &gHygonCcxAcpiCratServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDharmaCratServicesProtocol
                );
}

EFI_STATUS
EFIAPI
CcxDharmaSratServicesProtocolInstall (
  IN       EFI_HANDLE        ImageHandle,
  IN       EFI_SYSTEM_TABLE  *SystemTable
  )
{
  // Install ACPI CPU SRAT services protocol
  return gBS->InstallProtocolInterface (
                &ImageHandle,
                &gHygonCcxAcpiSratServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mDharmaSratServicesProtocol
                );
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  PStateGatherData
 *
 *  Description:
 *    This function will gather PState information from the MSRs and fill up the
 *    pStateBuf. This buffer will be used by the PState Leveling, and PState Table
 *    generation code later.
 *
 *  Parameters:
 *    @param[in]  This                                 A pointer to the HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL instance.
 *    @param[out] PstateSysInfoPtr                     Contains Pstate information for whole system
 *
 *    @retval     HGPI_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
EFIAPI
CcxDharmaPStateGatherData (
  IN       HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL    *This,
  OUT      HYGON_PSTATE_SYS_INFO                   **PstateSysInfoPtr
  )
{
  UINT32                                   LApicIdLoop;
  UINT32                                   TableSize;
  UINT32                                   PstateLoop;
  UINT32                                   TotalEnabledPStates;
  UINTN                                    NumberOfComplexes;
  UINTN                                    NumberOfCores;
  UINTN                                    NumberOfThreads;
  UINTN                                    ComplexeLoop;
  UINTN                                    CoreLoop;
  UINTN                                    ThreadLoop;
  UINTN                                    LogicalCoreNum;
  UINTN                                    MaxSwState;
  UINTN                                    NumberOfBoostPstate;
  UINTN                                    Frequency;
  UINTN                                    VoltageInuV;
  UINTN                                    PowerInmW;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfCdds;
  UINTN                                    CddsPresent;
  UINTN                                    SocketLoop;
  UINTN                                    CddLoop;
  UINTN                                    LogCddId;
  HYGON_PSTATE_SOCKET_INFO                 *PstateSocketInfo;
  S_PSTATE_VALUES                          *PstateStructure;
  ALLOCATE_HEAP_PARAMS                     AllocHeapParams;
  EFI_STATUS                               Status;
  HGPI_STATUS                              HgpiStatus;
  HYGON_CONFIG_PARAMS                      StdHeader;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopology;

  CcxGetPstateNumber (0, &NumberOfBoostPstate, &MaxSwState, &StdHeader);
  MaxSwState = MaxSwState - NumberOfBoostPstate;

  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, &FabricTopology);
  ASSERT (!EFI_ERROR (Status));

  Status = gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, &CoreTopology);
  ASSERT (!EFI_ERROR (Status));

  if (FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL) != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  // Create Heap and store p-state data for ACPI table
  TableSize = (UINT32)(sizeof (HYGON_PSTATE_SYS_INFO) + MultU64x64 ((MultU64x64 (MaxSwState, sizeof (S_PSTATE_VALUES)) + sizeof (HYGON_PSTATE_SOCKET_INFO)), NumberOfSockets));

  AllocHeapParams.RequestedBufferSize = TableSize;
  AllocHeapParams.BufferHandle = HYGON_PSTATE_DATA_BUFFER_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  HgpiStatus = HeapAllocateBuffer (&AllocHeapParams, &StdHeader);
  ASSERT (HgpiStatus == HGPI_SUCCESS);
  if (HgpiStatus == HGPI_SUCCESS) {
    *PstateSysInfoPtr = (HYGON_PSTATE_SYS_INFO *)AllocHeapParams.BufferPtr;
  } else {
    return EFI_OUT_OF_RESOURCES;
  }

  (*PstateSysInfoPtr)->TotalSocketInSystem = (UINT8)NumberOfSockets;
  (*PstateSysInfoPtr)->SizeOfBytes = TableSize;
  PstateSocketInfo = (*PstateSysInfoPtr)->PStateSocketStruct;

  for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) {
    // Calculate number of logical cores
    LogicalCoreNum = 0;
    LApicIdLoop    = 0;
    LogCddId = 0;
    if (FabricTopology->GetCddInfo (FabricTopology, SocketLoop, &NumberOfCdds, &CddsPresent) == EFI_SUCCESS) {
      for (CddLoop = 0; CddLoop < MAX_CDDS_PER_SOCKET; CddLoop++) {
        if (!IS_CDD_PRESENT (CddLoop, CddsPresent)) {
          continue;
        }

        if (CoreTopology->GetCoreTopologyOnCdd (
                            CoreTopology,
                            SocketLoop,
                            CddLoop,
                            &NumberOfComplexes,
                            &NumberOfCores,
                            &NumberOfThreads
                            ) == EFI_SUCCESS) {
          LogicalCoreNum += NumberOfComplexes * NumberOfCores * NumberOfThreads;
          for (ComplexeLoop = 0; ComplexeLoop < NumberOfComplexes; ComplexeLoop++) {
            for (CoreLoop = 0; CoreLoop < NumberOfCores; CoreLoop++) {
              for (ThreadLoop = 0; ThreadLoop < NumberOfThreads; ThreadLoop++) {
                PstateSocketInfo->LocalApicId[LApicIdLoop] = CcxDharmaCalcLocalApic (
                                                               SocketLoop,
                                                               LogCddId,
                                                               ComplexeLoop,
                                                               CoreLoop,
                                                               ThreadLoop
                                                               );
                LApicIdLoop++;
              }
            }
          }
        }

        LogCddId++;
      }
    }

    PstateSocketInfo->SocketNumber = (UINT8)SocketLoop;
    PstateSocketInfo->TotalLogicalCores = (UINT32)LogicalCoreNum;
    PstateSocketInfo->CreateAcpiTables  = TRUE;
    PstateSocketInfo->PStateMaxValue    = (UINT8)MaxSwState;

    // Get transition latency
    CcxDharmaGetPstateTransLatency (&(PstateSocketInfo->TransitionLatency), &StdHeader);

    // Get IsPsdDependent
    switch (PcdGet8 (PcdHygonHgpiPstatePolicy)) {
      case 0:
        PstateSocketInfo->IsPsdDependent = FALSE;
        break;
      case 1:
        PstateSocketInfo->IsPsdDependent = TRUE;
        break;
      case 2:
        PstateSocketInfo->IsPsdDependent = FALSE;
        break;
      default:
        ASSERT (FALSE);
        break;
    }

    PstateStructure     = PstateSocketInfo->PStateStruct;
    TotalEnabledPStates = 0;

    for (PstateLoop = 0; PstateLoop <= MaxSwState; PstateLoop++) {
      LibHygonMemFill (PstateStructure, 0, sizeof (S_PSTATE_VALUES), &StdHeader);

      if (CcxGetPstateInfo (0, (SwPstate0 + PstateLoop), &Frequency, &VoltageInuV, &PowerInmW, &StdHeader)) {
        PstateStructure->CoreFreq = (UINT32)Frequency;
        PstateStructure->Power    = (UINT32)PowerInmW;
        PstateStructure->SwPstateNumber = PstateLoop;
        PstateStructure->PStateEnable   = 1;
        PstateStructure++;
        TotalEnabledPStates++;
      }
    } // for (PstateLoop = 0; PstateLoop < MaxState; PstateLoop++)

    // Don't create ACPI Tables if there is one or less than one PState is enabled
    if (TotalEnabledPStates <= 1) {
      PstateSocketInfo->CreateAcpiTables = FALSE;
    }

    PstateSocketInfo = (HYGON_PSTATE_SOCKET_INFO *)((UINT8 *)PstateSocketInfo + sizeof (HYGON_PSTATE_SOCKET_INFO) + sizeof (S_PSTATE_VALUES) * MaxSwState);
  }

  return EFI_SUCCESS;
}

/*---------------------------------------------------------------------------------------*/

/**
 *  Family specific call to get Pstate Transition Latency.
 *
 *  Calculate TransitionLatency by Gaters On/Off Time value and pll value.
 *
 *  @param[out]    TransitionLatency                 The transition latency.
 *  @param[in]     StdHeader                         Header for library and services
 *
 *  @retval        HGPI_SUCCESS Always succeeds.
 */
HGPI_STATUS
CcxDharmaGetPstateTransLatency (
  OUT   UINT32                                 *TransitionLatency,
  IN       HYGON_CONFIG_PARAMS                      *StdHeader
  )
{
  *TransitionLatency = 0;

  return (HGPI_SUCCESS);
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  CcxDharmaGetCStateInfo
 *
 *  Description:
 *    This function will gather CState information
 *
 *  Parameters:
 *    @param[in]  This                                 A pointer to the HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL instance.
 *    @param[out] CstateInfo                           Contains Cstate information
 *
 *    @retval     HGPI_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
EFI_STATUS
EFIAPI
CcxDharmaGetCStateInfo (
  IN       HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL    *This,
  OUT   HYGON_CSTATE_INFO                          **CstateInfo
  )
{
  HGPI_STATUS           HgpiStatus;
  ALLOCATE_HEAP_PARAMS  AllocHeapParams;

  AllocHeapParams.RequestedBufferSize = sizeof (HYGON_CSTATE_INFO);
  AllocHeapParams.BufferHandle = HYGON_CSTATE_DATA_BUFFER_HANDLE;
  AllocHeapParams.Persist = HEAP_SYSTEM_MEM;
  HgpiStatus = HeapAllocateBuffer (&AllocHeapParams, NULL);
  ASSERT (HgpiStatus == HGPI_SUCCESS);
  if (HgpiStatus == HGPI_SUCCESS) {
    *CstateInfo = (HYGON_CSTATE_INFO *)AllocHeapParams.BufferPtr;
  } else {
    return EFI_OUT_OF_RESOURCES;
  }

  // Is Cstate enabled
  if ((PcdGet8 (PcdHygonCStateMode) == 0) ||
      (PcdGet16 (PcdHygonCStateIoBaseAddress) == 0)) {
    (*CstateInfo)->IsCstateEnabled = FALSE;
  } else {
    (*CstateInfo)->IsCstateEnabled = TRUE;
  }

  // Io Cstate address
  (*CstateInfo)->IoCstateAddr = (UINT32)PcdGet16 (PcdHygonCStateIoBaseAddress);

  // Is _CSD generated
  (*CstateInfo)->IsCsdGenerated = (BOOLEAN)(CcxGetThreadsPerCore () > 1);

  (*CstateInfo)->IsMonitorMwaitSupported = TRUE;

  return EFI_SUCCESS;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  CcxDharmaGetPsdDomain
 *
 *  Description:
 *    This function will return PSD domain for independency
 *
 *  Parameters:
 *    @param[in]  This                                 A pointer to the HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL instance.
 *    @param[in]  LocalApicId                          Local APIC ID
 *
 *    @retval     HGPI_STATUS
 *
 *---------------------------------------------------------------------------------------
 **/
UINT32
EFIAPI
CcxDharmaGetPsdDomain (
  IN       HYGON_ACPI_CPU_SSDT_SERVICES_PROTOCOL    *This,
  IN       UINT8                                   LocalApicId
  )
{
  UINT8  ThreadsPerCore;

  ThreadsPerCore = CcxGetThreadsPerCore ();
  ASSERT (ThreadsPerCore != 0);

  return ((UINT32)(LocalApicId / ThreadsPerCore));
}

/*----------------------------------------------------------------------------------------
 *                          HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL
 *----------------------------------------------------------------------------------------
 */

/**
 * This service retrieves CRAT information about the HSA.
 *
 * @param[in]      This                             A pointer to the
 *                                                  HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL instance.
 * @param[in]      CratHeaderStructPtr              CRAT table structure pointer
 * @param[in, out] TableEnd                         Point to the end of this table
 *
 * @retval EFI_SUCCESS                              The HSA processor information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER                    CratHsaProcInfo is NULL.
 *
 **/
EFI_STATUS
EFIAPI
CcxDharmaGetCratHsaProcEntry (
  IN       HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL    *This,
  IN       CRAT_HEADER                            *CratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  )
{
  UINTN                                    SocketLoop;
  UINTN                                    CddLoop;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfCdds;
  UINTN                                    CddsPresent;
  UINTN                                    LogCddId;
  UINTN                                    NumberOfComplexes;
  UINTN                                    NumberOfCores;
  UINTN                                    NumberOfThreads;
  UINT32                                   Domain;
  UINT32                                   PreDomain;
  EFI_STATUS                               CalledStatus;
  CRAT_HSA_PROCESSING_UNIT                 *CratHsaEntry;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopologyServices;
  FABRIC_NUMA_SERVICES_PROTOCOL            *FabricNuma;

  // Locate Fabric CRAT Services Protocol
  if (gBS->LocateProtocol (&gHygonFabricNumaServicesProtocolGuid, NULL, (VOID **)&FabricNuma) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  // Locate FabricTopologyServicesProtocol
  CalledStatus = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  ASSERT (!EFI_ERROR (CalledStatus));

  CalledStatus = gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, &CoreTopologyServices);
  ASSERT (!EFI_ERROR (CalledStatus));

  PreDomain    = 0;
  CratHsaEntry = NULL;
  if (FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL) == EFI_SUCCESS) {
    for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) {
      LogCddId = 0;
      if (FabricTopology->GetCddInfo (FabricTopology, SocketLoop, &NumberOfCdds, &CddsPresent) == EFI_SUCCESS) {
        for (CddLoop = 0; CddLoop < MAX_CDDS_PER_SOCKET; CddLoop++) {
          if (!IS_CDD_PRESENT (CddLoop, CddsPresent)) {
            continue;
          }

          if (FabricNuma->DomainXlat (FabricNuma, SocketLoop, CddLoop, &Domain) ==  EFI_SUCCESS) {
            if ((Domain != PreDomain) || (Domain == 0)) {
              CratHsaEntry = (CRAT_HSA_PROCESSING_UNIT *)AddOneCratEntry (
                                                           CRAT_HSA_PROC_UNIT_TYPE,
                                                           CratHeaderStructPtr,
                                                           TableEnd
                                                           );
              CratHsaEntry->Flags.Enabled    = 1;
              CratHsaEntry->Flags.CpuPresent = 1;
              CratHsaEntry->ProximityNode    = Domain;
              CratHsaEntry->ProcessorIdLow   = CcxDharmaCalcLocalApic (SocketLoop, LogCddId, 0, 0, 0);
              CratHsaEntry->NumCPUCores   = 0;
              CratHsaEntry->WaveFrontSize = 4;
              PreDomain = Domain;
            }

            CoreTopologyServices->GetCoreTopologyOnCdd (
                                    CoreTopologyServices,
                                    SocketLoop,
                                    CddLoop,
                                    &NumberOfComplexes,
                                    &NumberOfCores,
                                    &NumberOfThreads
                                    );
            CratHsaEntry->NumCPUCores += (UINT16)(NumberOfComplexes * NumberOfCores * NumberOfThreads);
          }

          LogCddId++;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 * This service retrieves information about the cache.
 *
 * @param[in]      This                             A pointer to the
 *                                                  HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL instance.
 * @param[in]      CratHeaderStructPtr              CRAT table structure pointer
 * @param[in, out] TableEnd                         Point to the end of this table
 *
 * @retval EFI_SUCCESS                              The cache information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER                    CratCacheInfo is NULL.
 *
 **/
EFI_STATUS
EFIAPI
CcxDharmaGetCratCacheEntry (
  IN       HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL    *This,
  IN       CRAT_HEADER                            *CratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  )
{
  UINT8                                    i;
  UINT32                                   ApicId;
  UINT8                                    SiblingMapMask;
  UINT32                                   NumOfThreadsSharing;
  UINT32                                   TotalThreads;
  UINTN                                    SocketLoop;
  UINTN                                    CddLoop;
  UINTN                                    CddsPresent;
  UINTN                                    LogCddId;
  UINTN                                    ComplexLoop;
  UINTN                                    CoreLoop;
  UINTN                                    ThreadsLoop;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfCdds;
  UINTN                                    NumberOfComplexes;
  UINTN                                    NumberOfCores;
  UINTN                                    NumberOfThreads;
  CPUID_DATA                               CpuId;
  EFI_STATUS                               CalledStatus;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopologyServices;
  CRAT_CACHE                               *CratCacheEntry;

  // Locate FabricTopologyServicesProtocol
  CalledStatus = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  ASSERT (!EFI_ERROR (CalledStatus));

  CalledStatus = gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, &CoreTopologyServices);
  ASSERT (!EFI_ERROR (CalledStatus));

  TotalThreads = 0;
  if (FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL) == EFI_SUCCESS) {
    for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) {
      LogCddId = 0;
      if (FabricTopology->GetCddInfo (FabricTopology, SocketLoop, &NumberOfCdds, &CddsPresent) == EFI_SUCCESS) {
        for (CddLoop = 0; CddLoop < MAX_CDDS_PER_SOCKET; CddLoop++) {
          if (!IS_CDD_PRESENT (CddLoop, CddsPresent)) {
            continue;
          }

          if (CoreTopologyServices->GetCoreTopologyOnCdd (
                                      CoreTopologyServices,
                                      SocketLoop,
                                      CddLoop,
                                      &NumberOfComplexes,
                                      &NumberOfCores,
                                      &NumberOfThreads
                                      ) == EFI_SUCCESS) {
            for (ComplexLoop = 0; ComplexLoop < NumberOfComplexes; ComplexLoop++) {
              for (CoreLoop = 0; CoreLoop < NumberOfCores; CoreLoop++) {
                for (ThreadsLoop = 0; ThreadsLoop < NumberOfThreads; ThreadsLoop++) {
                  ApicId = CcxDharmaCalcLocalApic (SocketLoop, LogCddId, ComplexLoop, CoreLoop, ThreadsLoop);

                  // L1 Data cache
                  AsmCpuidEx (0x8000001D, 0, &(CpuId.EAX_Reg), &(CpuId.EBX_Reg), &(CpuId.ECX_Reg), &(CpuId.EDX_Reg));
                  NumOfThreadsSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
                  if ((TotalThreads % NumOfThreadsSharing) == 0) {
                    CratCacheEntry = (CRAT_CACHE *)AddOneCratEntry (CRAT_CACHE_TYPE, CratHeaderStructPtr, TableEnd);
                    CratCacheEntry->Flags.Enabled   = 1;
                    CratCacheEntry->Flags.CpuCache  = 1;
                    CratCacheEntry->Flags.DataCache = 1;
                    CratCacheEntry->ProcessorIdLow  = ApicId;
                    SiblingMapMask = 1;
                    for (i = 1; i < NumOfThreadsSharing; i++) {
                      SiblingMapMask = (SiblingMapMask << 1) + 1;
                    }

                    i = (UINT8)(TotalThreads / 8);
                    CratCacheEntry->SiblingMap[i]   = SiblingMapMask << (TotalThreads % 8);
                    CratCacheEntry->CacheProperties = (((CpuId.EDX_Reg >> 1) & 1) == 0) ? 0 : 2;
                    AsmCpuid (
                      HYGON_CPUID_TLB_L1Cache,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratCacheEntry->CacheSize     = CpuId.ECX_Reg >> 24;
                    CratCacheEntry->CacheLevel    = L1_CACHE;
                    CratCacheEntry->LinesPerTag   = (CpuId.ECX_Reg >> 8) & 0xFF;
                    CratCacheEntry->CacheLineSize = CpuId.ECX_Reg & 0xFF;
                    CratCacheEntry->Associativity = (CpuId.ECX_Reg >> 16) & 0xFF;
                    CratCacheEntry->CacheLatency  = 1;
                  }

                  // L1 Instruction cache
                  AsmCpuidEx (0x8000001D, 1, &(CpuId.EAX_Reg), &(CpuId.EBX_Reg), &(CpuId.ECX_Reg), &(CpuId.EDX_Reg));
                  NumOfThreadsSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
                  if ((TotalThreads % NumOfThreadsSharing) == 0) {
                    CratCacheEntry = (CRAT_CACHE *)AddOneCratEntry (CRAT_CACHE_TYPE, CratHeaderStructPtr, TableEnd);
                    CratCacheEntry->Flags.Enabled  = 1;
                    CratCacheEntry->Flags.CpuCache = 1;
                    CratCacheEntry->Flags.InstructionCache = 1;
                    CratCacheEntry->ProcessorIdLow = ApicId;
                    SiblingMapMask = 1;
                    for (i = 1; i < NumOfThreadsSharing; i++) {
                      SiblingMapMask = (SiblingMapMask << 1) + 1;
                    }

                    i = (UINT8)(TotalThreads / 8);
                    CratCacheEntry->SiblingMap[i]   = SiblingMapMask << (TotalThreads % 8);
                    CratCacheEntry->CacheProperties = (((CpuId.EDX_Reg >> 1) & 1) == 0) ? 0 : 2;
                    AsmCpuid (
                      HYGON_CPUID_TLB_L1Cache,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratCacheEntry->CacheSize     = CpuId.EDX_Reg >> 24;
                    CratCacheEntry->CacheLevel    = L1_CACHE;
                    CratCacheEntry->LinesPerTag   = (CpuId.EDX_Reg >> 8) & 0xFF;
                    CratCacheEntry->CacheLineSize = CpuId.EDX_Reg & 0xFF;
                    CratCacheEntry->Associativity = (CpuId.EDX_Reg >> 16) & 0xFF;
                    CratCacheEntry->CacheLatency  = 1;
                  }

                  // L2 cache
                  AsmCpuidEx (0x8000001D, 2, &(CpuId.EAX_Reg), &(CpuId.EBX_Reg), &(CpuId.ECX_Reg), &(CpuId.EDX_Reg));
                  NumOfThreadsSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
                  if ((TotalThreads % NumOfThreadsSharing) == 0) {
                    CratCacheEntry = (CRAT_CACHE *)AddOneCratEntry (CRAT_CACHE_TYPE, CratHeaderStructPtr, TableEnd);
                    CratCacheEntry->Flags.Enabled   = 1;
                    CratCacheEntry->Flags.CpuCache  = 1;
                    CratCacheEntry->Flags.DataCache = 1;
                    CratCacheEntry->Flags.InstructionCache = 1;
                    CratCacheEntry->ProcessorIdLow = ApicId;
                    SiblingMapMask = 1;
                    for (i = 1; i < NumOfThreadsSharing; i++) {
                      SiblingMapMask = (SiblingMapMask << 1) + 1;
                    }

                    i = (UINT8)(TotalThreads / 8);
                    CratCacheEntry->SiblingMap[i]   = SiblingMapMask << (TotalThreads % 8);
                    CratCacheEntry->CacheProperties = (((CpuId.EDX_Reg >> 1) & 1) == 0) ? 0 : 2;
                    AsmCpuid (
                      HYGON_CPUID_L2L3Cache_L2TLB,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratCacheEntry->CacheSize     = CpuId.ECX_Reg >> 16;
                    CratCacheEntry->CacheLevel    = L2_CACHE;
                    CratCacheEntry->LinesPerTag   = (CpuId.ECX_Reg >> 8) & 0xF;
                    CratCacheEntry->CacheLineSize = CpuId.ECX_Reg & 0xFF;
                    CratCacheEntry->Associativity = GetCacheAssoc ((CpuId.ECX_Reg >> 12) & 0xF);
                    CratCacheEntry->CacheLatency  = 1;
                  }

                  // L3 cache
                  AsmCpuidEx (0x8000001D, 3, &(CpuId.EAX_Reg), &(CpuId.EBX_Reg), &(CpuId.ECX_Reg), &(CpuId.EDX_Reg));
                  NumOfThreadsSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
                  if ((TotalThreads % NumOfThreadsSharing) == 0) {
                    CratCacheEntry = (CRAT_CACHE *)AddOneCratEntry (CRAT_CACHE_TYPE, CratHeaderStructPtr, TableEnd);
                    CratCacheEntry->Flags.Enabled   = 1;
                    CratCacheEntry->Flags.CpuCache  = 1;
                    CratCacheEntry->Flags.DataCache = 1;
                    CratCacheEntry->Flags.InstructionCache = 1;
                    CratCacheEntry->ProcessorIdLow = ApicId;
                    SiblingMapMask = 1;
                    for (i = 1; i < NumOfThreadsSharing; i++) {
                      SiblingMapMask = (SiblingMapMask << 1) + 1;
                    }

                    i = (UINT8)(TotalThreads / 8);
                    CratCacheEntry->SiblingMap[i]   = SiblingMapMask << (TotalThreads % 8);
                    CratCacheEntry->CacheProperties = (((CpuId.EDX_Reg >> 1) & 1) == 0) ? 0 : 2;
                    AsmCpuid (
                      HYGON_CPUID_L2L3Cache_L2TLB,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratCacheEntry->CacheSize     = (CpuId.EDX_Reg >> 18) * 512;
                    CratCacheEntry->CacheLevel    = L3_CACHE;
                    CratCacheEntry->LinesPerTag   = (CpuId.EDX_Reg >> 8) & 0xF;
                    CratCacheEntry->CacheLineSize = CpuId.EDX_Reg & 0xFF;
                    CratCacheEntry->Associativity = GetCacheAssoc ((CpuId.EDX_Reg >> 12) & 0xF);
                    CratCacheEntry->CacheLatency  = 1;
                  }

                  TotalThreads++;
                }
              }
            }
          }

          LogCddId++;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 * This service retrieves information about the TLB.
 *
 * @param[in]      This                             A pointer to the
 *                                                  HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL instance.
 * @param[in]      CratHeaderStructPtr              CRAT table structure pointer
 * @param[in, out] TableEnd                         Point to the end of this table
 *
 * @retval EFI_SUCCESS                              The TLB information was successfully retrieved.
 * @retval EFI_INVALID_PARAMETER                    CratTlbInfo is NULL.
 *
 **/
EFI_STATUS
EFIAPI
CcxDharmaGetCratTlbEntry (
  IN       HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL    *This,
  IN       CRAT_HEADER                            *CratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  )
{
  UINT8                                    i;
  UINT32                                   ApicId;
  UINT8                                    SiblingMapMask;
  UINT32                                   NumOfThreadsSharing;
  UINT32                                   TotalThreads;
  UINTN                                    SocketLoop;
  UINTN                                    CddLoop;
  UINTN                                    CddsPresent;
  UINTN                                    LogCddId;
  UINTN                                    ComplexLoop;
  UINTN                                    CoreLoop;
  UINTN                                    ThreadsLoop;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfCdds;
  UINTN                                    NumberOfComplexes;
  UINTN                                    NumberOfCores;
  UINTN                                    NumberOfThreads;
  CPUID_DATA                               CpuId;
  EFI_STATUS                               CalledStatus;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopologyServices;
  CRAT_TLB                                 *CratTlbEntry;

  // Locate FabricTopologyServicesProtocol
  CalledStatus = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  ASSERT (!EFI_ERROR (CalledStatus));

  CalledStatus = gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, &CoreTopologyServices);
  ASSERT (!EFI_ERROR (CalledStatus));

  TotalThreads = 0;
  if (FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL) == EFI_SUCCESS) {
    for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) {
      LogCddId = 0;
      if (FabricTopology->GetCddInfo (FabricTopology, SocketLoop, &NumberOfCdds, &CddsPresent) == EFI_SUCCESS) {
        for (CddLoop = 0; CddLoop < MAX_CDDS_PER_SOCKET; CddLoop++) {
          if (!IS_CDD_PRESENT (CddLoop, CddsPresent)) {
            continue;
          }

          if (CoreTopologyServices->GetCoreTopologyOnCdd (
                                      CoreTopologyServices,
                                      SocketLoop,
                                      CddLoop,
                                      &NumberOfComplexes,
                                      &NumberOfCores,
                                      &NumberOfThreads
                                      ) == EFI_SUCCESS) {
            for (ComplexLoop = 0; ComplexLoop < NumberOfComplexes; ComplexLoop++) {
              for (CoreLoop = 0; CoreLoop < NumberOfCores; CoreLoop++) {
                for (ThreadsLoop = 0; ThreadsLoop < NumberOfThreads; ThreadsLoop++) {
                  ApicId = CcxDharmaCalcLocalApic (SocketLoop, LogCddId, ComplexLoop, CoreLoop, ThreadsLoop);

                  // L1 Data TLB
                  AsmCpuidEx (0x8000001D, 0, &(CpuId.EAX_Reg), &(CpuId.EBX_Reg), &(CpuId.ECX_Reg), &(CpuId.EDX_Reg));
                  NumOfThreadsSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
                  if ((TotalThreads % NumOfThreadsSharing) == 0) {
                    CratTlbEntry = (CRAT_TLB *)AddOneCratEntry (CRAT_TLB_TYPE, CratHeaderStructPtr, TableEnd);
                    CratTlbEntry->Flags.Enabled  = 1;
                    CratTlbEntry->Flags.DataTLB  = 1;
                    CratTlbEntry->Flags.CpuTLB   = 1;
                    CratTlbEntry->ProcessorIdLow = ApicId;
                    SiblingMapMask = 1;
                    for (i = 1; i < NumOfThreadsSharing; i++) {
                      SiblingMapMask = (SiblingMapMask << 1) + 1;
                    }

                    i = (UINT8)(TotalThreads / 8);
                    CratTlbEntry->SiblingMap[i] = SiblingMapMask << (TotalThreads % 8);
                    CratTlbEntry->TlbLevel = L1_CACHE;
                    AsmCpuid (
                      HYGON_CPUID_TLB_L1Cache,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratTlbEntry->DataTlbAssociativity2MB = CpuId.EAX_Reg >> 24;
                    CratTlbEntry->DataTlbSize2MB = GetTlbSize (TLB_2M, CratTlbEntry, ((CpuId.EAX_Reg >> 16) & 0xFF));
                    CratTlbEntry->DTLB4KAssoc    = CpuId.EBX_Reg >> 24;
                    CratTlbEntry->DTLB4KSize     = GetTlbSize (TLB_4K, CratTlbEntry, ((CpuId.EBX_Reg >> 16) & 0xFF));
                    AsmCpuid (
                      HYGON_CPUID_L1L2Tlb1GIdentifiers,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratTlbEntry->DTLB1GAssoc = CpuId.EAX_Reg >> 28;
                    CratTlbEntry->DTLB1GSize  = GetTlbSize (TLB_1G, CratTlbEntry, ((CpuId.EAX_Reg >> 16) & 0xFFF));
                  }

                  // L1 Instruction TLB
                  AsmCpuidEx (0x8000001D, 1, &(CpuId.EAX_Reg), &(CpuId.EBX_Reg), &(CpuId.ECX_Reg), &(CpuId.EDX_Reg));
                  NumOfThreadsSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
                  if ((TotalThreads % NumOfThreadsSharing) == 0) {
                    CratTlbEntry = (CRAT_TLB *)AddOneCratEntry (CRAT_TLB_TYPE, CratHeaderStructPtr, TableEnd);
                    CratTlbEntry->Flags.Enabled = 1;
                    CratTlbEntry->Flags.InstructionTLB = 1;
                    CratTlbEntry->Flags.CpuTLB   = 1;
                    CratTlbEntry->ProcessorIdLow = ApicId;
                    SiblingMapMask = 1;
                    for (i = 1; i < NumOfThreadsSharing; i++) {
                      SiblingMapMask = (SiblingMapMask << 1) + 1;
                    }

                    i = (UINT8)(TotalThreads / 8);
                    CratTlbEntry->SiblingMap[i] = SiblingMapMask << (TotalThreads % 8);
                    CratTlbEntry->TlbLevel = L1_CACHE;
                    AsmCpuid (
                      HYGON_CPUID_TLB_L1Cache,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratTlbEntry->InstructionTlbAssociativity2MB = (CpuId.EAX_Reg >> 8) & 0xFF;
                    CratTlbEntry->InstructionTlbSize2MB = GetTlbSize (TLB_2M, CratTlbEntry, (CpuId.EAX_Reg & 0xFF));
                    CratTlbEntry->ITLB4KAssoc = (CpuId.EBX_Reg >> 8) & 0xFF;
                    CratTlbEntry->ITLB4KSize  = GetTlbSize (TLB_4K, CratTlbEntry, (CpuId.EBX_Reg & 0xFF));
                    AsmCpuid (
                      HYGON_CPUID_L1L2Tlb1GIdentifiers,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratTlbEntry->ITLB1GAssoc = (CpuId.EAX_Reg >> 12) & 0xF;
                    CratTlbEntry->ITLB1GSize  = GetTlbSize (TLB_1G, CratTlbEntry, (CpuId.EAX_Reg & 0xFFF));
                  }

                  // L2 Data TLB
                  AsmCpuidEx (0x8000001D, 2, &(CpuId.EAX_Reg), &(CpuId.EBX_Reg), &(CpuId.ECX_Reg), &(CpuId.EDX_Reg));
                  NumOfThreadsSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
                  if ((TotalThreads % NumOfThreadsSharing) == 0) {
                    CratTlbEntry = (CRAT_TLB *)AddOneCratEntry (CRAT_TLB_TYPE, CratHeaderStructPtr, TableEnd);
                    CratTlbEntry->Flags.Enabled  = 1;
                    CratTlbEntry->Flags.DataTLB  = 1;
                    CratTlbEntry->Flags.CpuTLB   = 1;
                    CratTlbEntry->ProcessorIdLow = ApicId;
                    SiblingMapMask = 1;
                    for (i = 1; i < NumOfThreadsSharing; i++) {
                      SiblingMapMask = (SiblingMapMask << 1) + 1;
                    }

                    i = (UINT8)(TotalThreads / 8);
                    CratTlbEntry->SiblingMap[i] = SiblingMapMask << (TotalThreads % 8);
                    CratTlbEntry->TlbLevel = L2_CACHE;
                    AsmCpuid (
                      HYGON_CPUID_L2L3Cache_L2TLB,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratTlbEntry->DataTlbAssociativity2MB = CpuId.EAX_Reg >> 28;
                    CratTlbEntry->DataTlbSize2MB = GetTlbSize (TLB_2M, CratTlbEntry, ((CpuId.EAX_Reg >> 16) & 0xFFF));
                    CratTlbEntry->DTLB4KAssoc    = GetCacheAssoc (CpuId.EBX_Reg >> 28);
                    CratTlbEntry->DTLB4KSize     = GetTlbSize (TLB_4K, CratTlbEntry, ((CpuId.EBX_Reg >> 16) & 0xFFF));
                    AsmCpuid (
                      HYGON_CPUID_L1L2Tlb1GIdentifiers,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratTlbEntry->DTLB1GAssoc = GetCacheAssoc (CpuId.EBX_Reg >> 28);
                    CratTlbEntry->DTLB1GSize  = GetTlbSize (TLB_1G, CratTlbEntry, ((CpuId.EAX_Reg >> 16) & 0xFFF));
                  }

                  // L2 Instruction TLB
                  AsmCpuidEx (0x8000001D, 2, &(CpuId.EAX_Reg), &(CpuId.EBX_Reg), &(CpuId.ECX_Reg), &(CpuId.EDX_Reg));
                  NumOfThreadsSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
                  if ((TotalThreads % NumOfThreadsSharing) == 0) {
                    CratTlbEntry = (CRAT_TLB *)AddOneCratEntry (CRAT_TLB_TYPE, CratHeaderStructPtr, TableEnd);
                    CratTlbEntry->Flags.Enabled = 1;
                    CratTlbEntry->Flags.InstructionTLB = 1;
                    CratTlbEntry->Flags.CpuTLB   = 1;
                    CratTlbEntry->ProcessorIdLow = ApicId;
                    SiblingMapMask = 1;
                    for (i = 1; i < NumOfThreadsSharing; i++) {
                      SiblingMapMask = (SiblingMapMask << 1) + 1;
                    }

                    i = (UINT8)(TotalThreads / 8);
                    CratTlbEntry->SiblingMap[i] = SiblingMapMask << (TotalThreads % 8);
                    CratTlbEntry->TlbLevel = L2_CACHE;
                    AsmCpuid (
                      HYGON_CPUID_L2L3Cache_L2TLB,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratTlbEntry->InstructionTlbAssociativity2MB = GetCacheAssoc ((CpuId.EAX_Reg >> 12) & 0xF);
                    CratTlbEntry->InstructionTlbSize2MB = GetTlbSize (TLB_2M, CratTlbEntry, (CpuId.EAX_Reg & 0xFFF));
                    CratTlbEntry->ITLB4KAssoc = GetCacheAssoc ((CpuId.EBX_Reg >> 12) & 0xF);
                    CratTlbEntry->ITLB4KSize  = GetTlbSize (TLB_4K, CratTlbEntry, (CpuId.EBX_Reg & 0xFFF));
                    AsmCpuid (
                      HYGON_CPUID_L1L2Tlb1GIdentifiers,
                      &(CpuId.EAX_Reg),
                      &(CpuId.EBX_Reg),
                      &(CpuId.ECX_Reg),
                      &(CpuId.EDX_Reg)
                      );
                    CratTlbEntry->ITLB1GAssoc = GetCacheAssoc ((CpuId.EBX_Reg >> 12) & 0xF);
                    CratTlbEntry->ITLB1GSize  = GetTlbSize (TLB_1G, CratTlbEntry, (CpuId.EBX_Reg & 0xFFF));
                  }

                  // No L3 TLB

                  TotalThreads++;
                }
              }
            }
          }

          LogCddId++;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          HYGON_CCX_ACPI_SRAT_SERVICES_PROTOCOL
 *----------------------------------------------------------------------------------------
 */

/**
 * This service create SRAT Local APIC structure
 *
 * @param[in]      This                             A pointer to the
 *                                                  HYGON_CCX_ACPI_SRAT_SERVICES_PROTOCOL instance.
 * @param[in]      SratHeaderStructPtr              SRAT table structure pointer
 * @param[in, out] TableEnd                         Point to the end of this table
 *
 * @retval EFI_SUCCESS                              The LAPIC was successfully created.
 *
 **/
EFI_STATUS
EFIAPI
CcxDharmaCreateSratLapic (
  IN       HYGON_CCX_ACPI_SRAT_SERVICES_PROTOCOL    *This,
  IN       SRAT_HEADER                            *SratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  )
{
  UINT8                                    ApicId;
  UINTN                                    SocketLoop;
  UINTN                                    CddLoop;
  UINTN                                    CddsPresent;
  UINTN                                    LogCddId;
  UINTN                                    ComplexLoop;
  UINTN                                    CoreLoop;
  UINTN                                    ThreadsLoop;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfCdds;
  UINTN                                    NumberOfComplexes;
  UINTN                                    NumberOfCores;
  UINTN                                    NumberOfThreads;
  UINT32                                   Domain;
  SRAT_APIC                                *ApicEntry;
  EFI_STATUS                               CalledStatus;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopologyServices;
  FABRIC_NUMA_SERVICES_PROTOCOL            *FabricNuma;

  if (PcdGetBool (PcdX2ApicMode)) {
    return EFI_SUCCESS;
  }

  // Locate Fabric SRAT Services Protocol
  if (gBS->LocateProtocol (&gHygonFabricNumaServicesProtocolGuid, NULL, (VOID **)&FabricNuma) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  // Locate FabricTopologyServicesProtocol
  CalledStatus = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  ASSERT (!EFI_ERROR (CalledStatus));

  CalledStatus = gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, &CoreTopologyServices);
  ASSERT (!EFI_ERROR (CalledStatus));

  if (FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL) == EFI_SUCCESS) {
    for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) {
      LogCddId = 0;
      if (FabricTopology->GetCddInfo (FabricTopology, SocketLoop, &NumberOfCdds, &CddsPresent) == EFI_SUCCESS) {
        for (CddLoop = 0; CddLoop < MAX_CDDS_PER_SOCKET; CddLoop++) {
          if (!IS_CDD_PRESENT (CddLoop, CddsPresent)) {
            continue;
          }

          if (FabricNuma->DomainXlat (FabricNuma, SocketLoop, CddLoop, &Domain) ==  EFI_SUCCESS) {
            if (CoreTopologyServices->GetCoreTopologyOnCdd (
                                        CoreTopologyServices,
                                        SocketLoop,
                                        CddLoop,
                                        &NumberOfComplexes,
                                        &NumberOfCores,
                                        &NumberOfThreads
                                        ) == EFI_SUCCESS) {
              for (ComplexLoop = 0; ComplexLoop < NumberOfComplexes; ComplexLoop++) {
                for (CoreLoop = 0; CoreLoop < NumberOfCores; CoreLoop++) {
                  for (ThreadsLoop = 0; ThreadsLoop < NumberOfThreads; ThreadsLoop++) {
                    ApicEntry  = (SRAT_APIC *)*TableEnd;
                    *TableEnd += sizeof (SRAT_APIC);

                    ApicId = (UINT8)CcxDharmaCalcLocalApic (SocketLoop, LogCddId, ComplexLoop, CoreLoop, ThreadsLoop);

                    ApicEntry->Type   = SRAT_LOCAL_APIC_TYPE;
                    ApicEntry->Length = sizeof (SRAT_APIC);
                    ApicEntry->ProximityDomain_7_0     = (UINT8)Domain & 0xFF;
                    ApicEntry->ProximityDomain_31_8[0] = (UINT8)((Domain >> 8) & 0xFF);
                    ApicEntry->ProximityDomain_31_8[1] = (UINT8)((Domain >> 16) & 0xFF);
                    ApicEntry->ProximityDomain_31_8[2] = (UINT8)((Domain >> 24) & 0xFF);
                    ApicEntry->ApicId = ApicId;
                    ApicEntry->Flags.Enabled = 1;
                    ApicEntry->LocalSapicEid = 0;
                    ApicEntry->ClockDomain   = 0;
                  }
                }
              }
            }
          }

          LogCddId++;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
CcxDharmaCreateSratX2Apic (
  IN       HYGON_CCX_ACPI_SRAT_SERVICES_PROTOCOL    *This,
  IN       SRAT_HEADER                            *SratHeaderStructPtr,
  IN OUT   UINT8                                 **TableEnd
  )
{
  UINT32                                   x2ApicId;
  UINTN                                    SocketLoop;
  UINTN                                    CddLoop;
  UINTN                                    CddsPresent;
  UINTN                                    LogCddId;
  UINTN                                    ComplexLoop;
  UINTN                                    CoreLoop;
  UINTN                                    ThreadsLoop;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfCdds;
  UINTN                                    NumberOfComplexes;
  UINTN                                    NumberOfCores;
  UINTN                                    NumberOfThreads;
  UINT32                                   Domain;
  SRAT_x2APIC                              *X2ApicEntry;
  EFI_STATUS                               CalledStatus;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopologyServices;
  FABRIC_NUMA_SERVICES_PROTOCOL            *FabricNuma;

  if (!PcdGetBool (PcdX2ApicMode)) {
    return EFI_SUCCESS;
  }

  // Locate Fabric SRAT Services Protocol
  if (gBS->LocateProtocol (&gHygonFabricNumaServicesProtocolGuid, NULL, (VOID **)&FabricNuma) != EFI_SUCCESS) {
    return EFI_ABORTED;
  }

  // Locate FabricTopologyServicesProtocol
  CalledStatus = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **)&FabricTopology);
  ASSERT (!EFI_ERROR (CalledStatus));

  CalledStatus = gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, &CoreTopologyServices);
  ASSERT (!EFI_ERROR (CalledStatus));

  if (FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL) == EFI_SUCCESS) {
    for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) {
      LogCddId = 0;
      if (FabricTopology->GetCddInfo (FabricTopology, SocketLoop, &NumberOfCdds, &CddsPresent) == EFI_SUCCESS) {
        for (CddLoop = 0; CddLoop < MAX_CDDS_PER_SOCKET; CddLoop++) {
          if (!IS_CDD_PRESENT (CddLoop, CddsPresent)) {
            continue;
          }

          if (FabricNuma->DomainXlat (FabricNuma, SocketLoop, CddLoop, &Domain) ==  EFI_SUCCESS) {
            if (CoreTopologyServices->GetCoreTopologyOnCdd (
                                        CoreTopologyServices,
                                        SocketLoop,
                                        CddLoop,
                                        &NumberOfComplexes,
                                        &NumberOfCores,
                                        &NumberOfThreads
                                        ) == EFI_SUCCESS) {
              for (ComplexLoop = 0; ComplexLoop < NumberOfComplexes; ComplexLoop++) {
                for (CoreLoop = 0; CoreLoop < NumberOfCores; CoreLoop++) {
                  for (ThreadsLoop = 0; ThreadsLoop < NumberOfThreads; ThreadsLoop++) {
                    x2ApicId    = CcxDharmaCalcLocalApic (SocketLoop, LogCddId, ComplexLoop, CoreLoop, ThreadsLoop);
                    X2ApicEntry = (SRAT_x2APIC *)*TableEnd;
                    *TableEnd  += sizeof (SRAT_x2APIC);
                    X2ApicEntry->Type   = SRAT_LOCAL_X2_APIC_TYPE;
                    X2ApicEntry->Length = sizeof (SRAT_x2APIC);
                    X2ApicEntry->ProximityDomain = Domain;
                    X2ApicEntry->x2ApicId = x2ApicId;
                    X2ApicEntry->Flags.Enabled = 1;
                    X2ApicEntry->ClockDomain   = 0;
                  }
                }
              }
            }
          }

          LogCddId++;
        }
      }
    }
  }

  return EFI_SUCCESS;
}

UINT32
STATIC
CcxDharmaCalcLocalApic (
  IN       UINTN    Socket,
  IN       UINTN    Cdd,
  IN       UINTN    Complex,
  IN       UINTN    Core,
  IN       UINTN    Thread
  )
{
  UINT8           ThreadsPerCore;
  UINT8           NodesPerProcessor;
  UINTN           CddIndex;
  UINT32          LocalApicId;
  UINT32          CpuModel;

  LocalApicId       = 0;
  ThreadsPerCore    = CcxGetThreadsPerCore ();
  NodesPerProcessor = CcxGetNodesPerProcessor ();

  ASSERT (Socket < MAX_SOCKETS_SUPPORTED);
  ASSERT (Cdd < MAX_CDDS_PER_SOCKET);
  ASSERT (ThreadsPerCore != 0);
  ASSERT (ThreadsPerCore <= MAX_THREAD_PER_CORE);
  ASSERT (Thread < ThreadsPerCore);

  CddIndex = (Socket * (UINTN)NodesPerProcessor) + Cdd;

  CpuModel = GetHygonSocModel();
  switch (CpuModel) {
    case HYGON_EX_CPU:
      ASSERT (Complex < MAX_CCX_PER_CDD_HYEX);
      ASSERT (Core < MAX_CORE_PER_CCX_HYEX);
      if (ThreadsPerCore == 2) {
        return (UINT32)((CddIndex << 5) | (Complex << 3) | (Core << 1) | Thread);
      } else {
        return (UINT32)((CddIndex << 5) | (Complex << 3) | Core);
      }
      break;
      
    case HYGON_GX_CPU:
      // Shanghai
      ASSERT (Complex < MAX_CCX_PER_CDD_HYGX);
      ASSERT (Core < MAX_CORE_PER_CCX_HYGX);
      if (ThreadsPerCore == 2) {
        LocalApicId = (UINT32)((CddIndex << 5) | (Complex << 4) | (Core << 1) | Thread);
      } else {
        LocalApicId = (UINT32)((CddIndex << 5) | (Complex << 4) | Core);
      }
      break;
      
    case HYGON_HX_CPU:
      // Chengdu
      if (ThreadsPerCore == 4) {
        LocalApicId = (UINT32)((CddIndex << 6) | (Complex << 5) | (Core << 2) | Thread);
      } else if (ThreadsPerCore == 2) {
        LocalApicId = (UINT32)((CddIndex << 6) | (Complex << 5) | (Core << 1) | Thread);
      } else {
        LocalApicId = (UINT32)((CddIndex << 6) | (Complex << 5) | Core);
      }
      break;
    
    default:
      ASSERT(FALSE);
      break;
  }

  return LocalApicId;
}

/*---------------------------------------------------------------------------------------*/

/**
* This function will add one CRAT entry.
*
*    @param[in]      CratEntryType        CRAT entry type
*    @param[in]      CratHeaderStructPtr  CRAT header pointer
*    @param[in, out] TableEnd             The end of CRAT
*
*/
UINT8 *
AddOneCratEntry (
  IN       CRAT_ENTRY_TYPE    CratEntryType,
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  UINT8  *CurrentEntry;

  ASSERT (CratEntryType < CRAT_MAX_TYPE);

  CurrentEntry = *TableEnd;
  CratHeaderStructPtr->TotalEntries++;
  switch (CratEntryType) {
    case CRAT_HSA_PROC_UNIT_TYPE:
      *TableEnd += sizeof (CRAT_HSA_PROCESSING_UNIT);
      ((CRAT_HSA_PROCESSING_UNIT *)CurrentEntry)->Type   = (UINT8)CratEntryType;
      ((CRAT_HSA_PROCESSING_UNIT *)CurrentEntry)->Length = sizeof (CRAT_HSA_PROCESSING_UNIT);
      CratHeaderStructPtr->NumNodes++;
      break;
    case CRAT_CACHE_TYPE:
      *TableEnd += sizeof (CRAT_CACHE);
      ((CRAT_CACHE *)CurrentEntry)->Type   = (UINT8)CratEntryType;
      ((CRAT_CACHE *)CurrentEntry)->Length = sizeof (CRAT_CACHE);
      break;
    case CRAT_TLB_TYPE:
      *TableEnd += sizeof (CRAT_TLB);
      ((CRAT_TLB *)CurrentEntry)->Type   = (UINT8)CratEntryType;
      ((CRAT_TLB *)CurrentEntry)->Length = sizeof (CRAT_TLB);
      break;
    case CRAT_FPU_TYPE:
      *TableEnd += sizeof (CRAT_FPU);
      ((CRAT_FPU *)CurrentEntry)->Type   = (UINT8)CratEntryType;
      ((CRAT_FPU *)CurrentEntry)->Length = sizeof (CRAT_FPU);
      break;
    default:
      ASSERT (FALSE);
      break;
  }

  return CurrentEntry;
}

/*---------------------------------------------------------------------------------------*/

/**
* Return associativity
*
*    @param[in]      RawAssoc          Data which is got from CPUID
*
*/
UINT8
GetCacheAssoc (
  IN       UINT16   RawAssoc
  )
{
  UINT8  Associativity;

  Associativity = 0;

  switch (RawAssoc) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
      Associativity = (UINT8)RawAssoc;
      break;
    case 5:
      Associativity = 6;
      break;
    case 6:
      Associativity = 8;
      break;
    case 8:
      Associativity = 16;
      break;
    case 0xA:
      Associativity = 32;
      break;
    case 0xB:
      Associativity = 48;
      break;
    case 0xC:
      Associativity = 64;
      break;
    case 0xD:
      Associativity = 96;
      break;
    case 0xE:
      Associativity = 128;
      break;
    case 0xF:
      Associativity = 0xFF;
      break;
    default:
      ASSERT (FALSE);
      break;
  }

  return Associativity;
}

/*---------------------------------------------------------------------------------------*/

/**
* Return associativity
*
*    @param[in]      TLB_TYPE          2M4M, 4K or 1G
*    @param[in]      CratTlbEntry      Crat TLB entry
*    @param[in]      RawAssocSize      Value which is got from CPUID
*
*/
UINT8
GetTlbSize (
  IN       TLB_TYPE   TLB_TYPE,
  IN       CRAT_TLB  *CratTlbEntry,
  IN       UINT16     RawAssocSize
  )
{
  UINT8  TlbSize;

  if (RawAssocSize >= 256) {
    TlbSize = (UINT8)(RawAssocSize / 256);
    if (TLB_TYPE == TLB_2M) {
      CratTlbEntry->Flags.TLB2MBase256 = 1;
    }

    if (TLB_TYPE == TLB_4K) {
      CratTlbEntry->Flags.TLB4KBase256 = 1;
    }

    if (TLB_TYPE == TLB_1G) {
      CratTlbEntry->Flags.TLB1GBase256 = 1;
    }
  } else {
    TlbSize = (UINT8)(RawAssocSize);
  }

  return TlbSize;
}
