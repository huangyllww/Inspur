/* $NoKeywords:$ */
/**
 * @file
 *
 * HYGON Fabric ACPI HMAT.
 *
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Porting.h>
#include <HYGON.h>
#include <Library/BaseLib.h>
#include "Library/HygonBaseLib.h"
#include <FabricRegistersST.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/FabricRegisterAccLib.h>
#include <Protocol/HygonAcpiHmatServicesProtocol.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/HygonCxl20ServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/HygonCoreTopologyProtocol.h>
#include <Protocol/HygonSmbiosServicesProtocol.h>
#include <Protocol/HygonHpcbProtocol.h>
#include <Filecode.h>
#include "FabricAcpiDomainInfo.h"
#include "FabricAcpiTable.h"
#include <CxlCdat.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/PciNew.h>                           // byo231102 +
#include <Protocol/PciIo.h>
#include <Library/HygonCbsVariable.h>
#include <HPCB.h>
#include <Library/HpcbLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#define FILECODE FABRIC_FABRICDXE_FABRICACPIHMAT_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define ENTRY_BASE_UNIT_IN_PS                         1000
#define ENTRY_BASE_UNIT_IN_MB                         100
#define CXL_DEV_DEFAULT_LATENCY_PS                    200000
#define CXL_DEV_DEFAULT_BANDWIDTH_MB                  10000
#define MAX_REPORTED_DOMAINS                          32

// Sorted by Physical CDD ID
UINT16 TwoSocketUmcLatencyFourLinkForSingleSide[8][8] = {
             //S0D4,S0D5,S0D8,S0D9,S1D4,S1D5,S1D8,S1D9
/*S0D4*/      { 110, 170, 230, 175, 495, 440, 385, 440 },
/*S0D5*/      { 170, 110, 175, 230, 440, 385, 330, 385 },
/*S0D8*/      { 230, 175, 110, 170, 385, 330, 385, 440 },
/*S0D9*/      { 175, 230, 170, 110, 440, 385, 440, 495 },
/*S1D4*/      { 495, 440, 385, 440, 110, 170, 230, 175 },
/*S1D5*/      { 440, 385, 330, 385, 170, 110, 175, 230 },
/*S1D8*/      { 385, 330, 385, 440, 230, 175, 110, 170 },
/*S1D9*/      { 440, 385, 440, 495, 175, 230, 170, 110 }
};

// Sorted by Physical CDD ID
UINT16 TwoSocketCxlLatencyFourLinkForSingleSide[4][8] = {
             //S0D4,S0D5,S0D8,S0D9,S1D4,S1D5,S1D8,S1D9
/*S0D0*/      { 110, 165, 150, 100, 450, 335, 380, 430 },
/*S0D1*/      { 155, 100, 110, 165, 300, 250, 230, 280 },
/*S1D0*/      { 450, 330, 380, 430, 110, 165, 155, 100 },
/*S1D1*/      { 300, 250, 230, 280, 155, 100, 110, 165 },
};

/// Port Information Structure
typedef struct _HYGON_CXL_PORT_INFO_STRUCT {
  PCI_ADDR  EndPointBDF;                ///< Bus/Device/Function of Root Port in PCI_ADDR format
  UINT8     SocketId;                   ///< Socket ID for this port
  UINT8     LogicalDieId;
  UINT8     RbId;
  UINT8     Invalid;
} HYGON_CXL_PORT_INFO_STRUCT;
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
GetCcxAvailableDomainMap (
  IN  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology,
  IN  FABRIC_NUMA_SERVICES_PROTOCOL            *FabricNuma,
  OUT UINT64                                   *CcxAvailableDomainMap
  );

EFI_STATUS
GetCpuDramAvailableDomainMap (
  IN  FABRIC_NUMA_SERVICES_PROTOCOL            *FabricNuma,
  OUT UINT64                                   *DramAvailableDomainMap
  );

UINT32
GetDramPeakBandwidthInGroupInMB (
  IN HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL   *FabricTopology,
  IN DMI_INFO                                  *DmiInfo,
  IN DOMAIN_INFO                               *DomainInfo
  );

UINT32
GetGopBandwidthInMB (
  VOID
  );

BOOLEAN
CheckNumaOnSameSocket (
  IN  FABRIC_NUMA_SERVICES_PROTOCOL          *FabricNuma,
  IN  UINT32                                 NumaNode0,
  IN  UINT32                                 NumaNode1
  );

VOID
GetCxlLatencyBandwidth (
  IN  HYGON_NBIO_CXL20_SERVICES_PROTOCOL   *CxlServices,
  IN  CXL_DOMAIN_INFO                      *CxlDomainInfo,
  OUT UINT32                               *ReadLatency,
  OUT UINT32                               *WriteLatency,
  OUT UINT32                               *ReadBandwidth,
  OUT UINT32                               *WriteBandwidth
  );

/*----------------------------------------------------------------------------------------
 *                           G L O B A L   V A R I A B L E S
 *----------------------------------------------------------------------------------------
 */
UINT32 TargetDomainReadLatency[MAX_REPORTED_DOMAINS];
UINT32 TargetDomainWriteLatency[MAX_REPORTED_DOMAINS];
UINT32 TargetDomainReadBandwidth[MAX_REPORTED_DOMAINS];
UINT32 TargetDomainWriteBandwidth[MAX_REPORTED_DOMAINS];

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  FabricAcpiHmatInstall
 *
 *  Description:
 *    This function will create the contents of ACPI HMAT table
 *
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     Context    Pointer to the notification function's context.
 *
 *    @retval         EFI_SUCCESS                Services protocol installed
 *
 *---------------------------------------------------------------------------------------
 */
VOID
EFIAPI
FabricAcpiHmatInstall (
  IN       EFI_EVENT         Event,
  IN       VOID              *Context
  )
{
  EFI_STATUS                                     Status;
  FABRIC_NUMA_SERVICES_PROTOCOL                  *FabricNuma;
  HYGON_ACPI_HMAT_SERVICES_PROTOCOL              *HmatServices;
  HYGON_NBIO_CXL20_SERVICES_PROTOCOL             *CxlServices;
  HYGON_MEM_SMBIOS_SERVICES_PROTOCOL             *MemSmbios;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL        *FabricTopology;
  HMAT_MEM_PROXIMITY_DOMAIN_ATTRIBUTES_FLAGS     MemProxAttributes;
  HMAT_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO_FLAGS SysLocalInfoFlags;
  UINT32                                         NumOfDomain;
  UINT32                                         NumOfCxlDomain;
  DOMAIN_INFO                                    *DomainInfo;
  CXL_DOMAIN_INFO                                *CxlDomainInfo;
  UINT32                                         i;
  UINT32                                         j;
  UINT32                                         InitiatorDomain;
  UINT32                                         TargetDomain;
  UINT32                                         InitiatorDomainCount;
  UINT32                                         TargetDomainCount;
  UINT32                                         *InitiatorDomainList;
  UINT32                                         *TargetDomainList;
  UINT16                                         *Entries;
  UINT32                                         Bandwidth;
  UINT32                                         HalfxGMIBandwidth;
  UINT64                                         CcxAvailableDomainMap;
  UINT64                                         CpuDramAvailableDomainMap;
  DMI_INFO                                       *MemDmiInfo;
  UINT8                                          DataType[2];
  UINT32                                         EntryCount;
  UINT32                                         EntryIndex;
  BOOLEAN                                        IsSame;
  UINT32                                         SocketId;
  UINT32                                         DieId;

  IDS_HDT_CONSOLE (MAIN_FLOW, "%a - Entry\n", __FUNCTION__);

  // Locate required protocols
  Status = gBS->LocateProtocol (&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID **) &FabricTopology);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Not found gHygonFabricTopologyServicesProtocolGuid \n");
    return;
  }

  Status = gBS->LocateProtocol (&gHygonFabricNumaServicesProtocolGuid, NULL, (VOID **) &FabricNuma);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Not found gHygonFabricNumaServicesProtocolGuid \n");
    return;
  }

  Status = gBS->LocateProtocol (&gHygonAcpiHmatServicesProtocolGuid, NULL, (VOID **) &HmatServices);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Not found gHygonAcpiHmatServicesProtocolGuid \n");
    return;
  }

  Status = gBS->LocateProtocol (&gHygonNbioCxl20ServicesProtocolGuid, NULL, (VOID **) &CxlServices);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Not found gHygonNbioCxl20ServicesProtocolGuid \n");
    CxlServices = NULL;
  }

  Status = gBS->LocateProtocol (&gHygonMemSmbiosServicesProtocolGuid, NULL, (VOID **) &MemSmbios);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Not found gHygonMemSmbiosServicesProtocolGuid \n");
    return;
  }

  // Get required information
  Status = FabricNuma->GetDomainInfo (FabricNuma, &NumOfDomain, &DomainInfo, &NumOfCxlDomain, &CxlDomainInfo);
  if (Status != EFI_SUCCESS) {
    return;
  }

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (DMI_INFO), (VOID **) &MemDmiInfo);
  if (Status != EFI_SUCCESS) {
    return;
  }
  Status = MemSmbios->GetMemDmiInfo (MemSmbios, MemDmiInfo);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  MemSmbios->GetMemDmiInfo error (%r) \n", Status);
    gBS->FreePool (MemDmiInfo);
    return;
  }

  if (GetCcxAvailableDomainMap (FabricTopology, FabricNuma, &CcxAvailableDomainMap) != EFI_SUCCESS) {
    CcxAvailableDomainMap = (UINT64)-1;
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "  CcxAvailableDomainMap %x\n", CcxAvailableDomainMap);

  if (GetCpuDramAvailableDomainMap (FabricNuma, &CpuDramAvailableDomainMap) != EFI_SUCCESS) {
    CpuDramAvailableDomainMap = (UINT64)-1;
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "  CpuDramAvailableDomainMap %x\n", CpuDramAvailableDomainMap);

  HalfxGMIBandwidth = GetGopBandwidthInMB () / 2;
  IDS_HDT_CONSOLE (MAIN_FLOW, "  HalfxGMIBandwidth %d\n", HalfxGMIBandwidth);


  // Calculate DRAM and CXL domain count, and add Proximity Domain Attributes structures
  InitiatorDomainCount = 0;
  TargetDomainCount = 0;
  MemProxAttributes.Value = 0;

  //Normal NUMA node
  for (i = 0; i < NumOfDomain; i++) {
    MemProxAttributes.Fields.InitiatorProximityDomainValid = 0;

    if (CcxAvailableDomainMap & LShiftU64 (1, i)) {
      InitiatorDomainCount++;
      MemProxAttributes.Fields.InitiatorProximityDomainValid = 1;
    }

    if (CpuDramAvailableDomainMap & LShiftU64 (1, i)) {
      TargetDomainCount++;
      Status = HmatServices->AddMemoryProximityDomainAttributes (HmatServices, MemProxAttributes, i, i);
      ASSERT (Status == EFI_SUCCESS);
    }
  }

  //CXL NUMA node
  for (i = 0; i < NumOfCxlDomain; i++) {
    MemProxAttributes.Fields.InitiatorProximityDomainValid = 0;
    TargetDomainCount++;
    Status = HmatServices->AddMemoryProximityDomainAttributes (HmatServices, MemProxAttributes, (i + NumOfDomain), (i + NumOfDomain));
    ASSERT (Status == EFI_SUCCESS);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "  InitiatorDomainCount %d, TargetDomainCount %d \n", InitiatorDomainCount, TargetDomainCount);

  // Prepare the buffers
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (InitiatorDomainList[0]) * InitiatorDomainCount, (VOID **) &InitiatorDomainList);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  AllocatePool for InitiatorDomainList error (%r) \n", Status);
    InitiatorDomainList = NULL;
  }
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (TargetDomainList[0]) * TargetDomainCount, (VOID **) &TargetDomainList);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  AllocatePool for TargetDomainList error (%r) \n", Status);
    TargetDomainList = NULL;
  }
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (Entries[0]) * InitiatorDomainCount * TargetDomainCount, (VOID **) &Entries);
  if (Status != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  AllocatePool for Entries error (%r) \n", Status);
    Entries = NULL;
  }

  if (InitiatorDomainList != NULL && TargetDomainList != NULL && Entries != NULL) {
    // Prepare the Initiator Domain List and Target Domain List
    for (i = 0, j = 0; i < NumOfDomain; i++) {
      if (CcxAvailableDomainMap & LShiftU64 (1, i)) {
        InitiatorDomainList[j] = i;
        j++;
      }
    }
    ASSERT (j == InitiatorDomainCount);

    //NORMAL NUMA node
    j = 0;
    for (i = 0; i < NumOfDomain; i++) {
      if (CpuDramAvailableDomainMap & LShiftU64 (1, i)) {
        TargetDomainList[j] = i;
        j++;
      }
    }

    //CXL NUMA node
    for (i = 0; i < NumOfCxlDomain; i++) {
      TargetDomainList[j] = i + NumOfDomain;
      j++;
    }
    ASSERT (j == TargetDomainCount);

    // Calculate the target latency and bandwidth of each domain
    gBS->SetMem (TargetDomainReadLatency, sizeof (TargetDomainReadLatency), 0);
    gBS->SetMem (TargetDomainWriteLatency, sizeof (TargetDomainWriteLatency), 0);
    gBS->SetMem (TargetDomainReadBandwidth, sizeof (TargetDomainReadBandwidth), 0);
    gBS->SetMem (TargetDomainWriteBandwidth, sizeof (TargetDomainWriteBandwidth), 0);
    for (i = 0; i < TargetDomainCount; i++) {
      TargetDomain = TargetDomainList[i];
      if (TargetDomain < NumOfDomain) {
        //CPU DRAM NUMA
        TargetDomainReadBandwidth[i]  = GetDramPeakBandwidthInGroupInMB (FabricTopology, MemDmiInfo, &DomainInfo[TargetDomain]);
        TargetDomainReadBandwidth[i]  = TargetDomainReadBandwidth[i] * 3 / 4;   // 75% of peak DRAM b/w populated in target NUMA node
        TargetDomainWriteBandwidth[i] = TargetDomainReadBandwidth[i];           // Same bandwidth in read / write for DRAM
        TargetDomainReadLatency[i]    = 0;
        TargetDomainWriteLatency[i]   = 0;
      } else {
        //CXL NUMA
        if (CxlServices != NULL) {
          GetCxlLatencyBandwidth (
            CxlServices,
            &CxlDomainInfo[TargetDomain - NumOfDomain],
            &TargetDomainReadLatency[i],
            &TargetDomainWriteLatency[i],
            &TargetDomainReadBandwidth[i],
            &TargetDomainWriteBandwidth[i]
            );
          TargetDomainReadBandwidth[i]  = TargetDomainReadBandwidth[i]  / 2; // 50% of CXL_reported_bandwidth
          TargetDomainWriteBandwidth[i] = TargetDomainWriteBandwidth[i] / 2; // 50% of CXL_reported_bandwidth
        }
      }
    }


    // Prepare the latency table
    IsSame = TRUE;
    for (i = 0; i < TargetDomainCount; i++) {
      if (TargetDomainReadLatency[i] != TargetDomainWriteLatency[i]) {
        IsSame = FALSE;
        break;
      }
    }
    if (IsSame) {
      EntryCount = 1;
      DataType[0] = HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_ACCESS_LATENCY;
    } else {
      EntryCount = 2;
      DataType[0] = HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_READ_LATENCY;
      DataType[1] = HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_WRITE_LATENCY;
    }
    for (EntryIndex = 0; EntryIndex < EntryCount; EntryIndex++) {
      gBS->SetMem (Entries, sizeof (Entries[0]) * InitiatorDomainCount * TargetDomainCount, 0);
      for (i = 0; i < InitiatorDomainCount; i++) {
        for (j = 0; j < TargetDomainCount; j++) {
          InitiatorDomain = InitiatorDomainList[i];
          TargetDomain = TargetDomainList[j];
          if (TargetDomain < NumOfDomain) {
            //from CCX to CPU UMC, just include SOC internal latency
            Entries[i * TargetDomainCount + j] = TwoSocketUmcLatencyFourLinkForSingleSide[InitiatorDomain][TargetDomain];
          } else {
            //from CCX to CXL mem, include CXL device latency and SOC internal latency
            SocketId = (UINT32) LowBitSet32 (CxlDomainInfo[TargetDomain - NumOfDomain].SocketMap);
            DieId = (UINT32) LowBitSet32 (CxlDomainInfo[TargetDomain - NumOfDomain].DieMap);
            IDS_HDT_CONSOLE (MAIN_FLOW, "  Cxl NUMA node belong to socket %d die %d \n", SocketId, DieId);
            if (EntryIndex == 0) {
              Entries[i * TargetDomainCount + j] = (UINT16) (TargetDomainReadLatency[j] / ENTRY_BASE_UNIT_IN_PS);
            } else {
              Entries[i * TargetDomainCount + j] = (UINT16) (TargetDomainWriteLatency[j] / ENTRY_BASE_UNIT_IN_PS);
            }
            Entries[i * TargetDomainCount + j] += TwoSocketCxlLatencyFourLinkForSingleSide[SocketId * 2 + DieId][InitiatorDomain];
          }
        }
      }
      SysLocalInfoFlags.Value = 0;
      Status = HmatServices->AddSystemLocalityLatencyBandwidthInfo (
                              HmatServices,
                              SysLocalInfoFlags,
                              DataType[EntryIndex],
                              0,                         // MinTransferSize
                              InitiatorDomainCount,
                              TargetDomainCount,
                              ENTRY_BASE_UNIT_IN_PS,    // 1000 pecoseconds is 1 nanosecond
                              InitiatorDomainList,
                              TargetDomainList,
                              Entries
                              );
      ASSERT (Status == EFI_SUCCESS);
    }


    // Prepare the bandwidth table
    IsSame = TRUE;
    for (i = 0; i < TargetDomainCount; i++) {
      if (TargetDomainReadBandwidth[i] != TargetDomainWriteBandwidth[i]) {
        IsSame = FALSE;
        break;
      }
    }
    if (IsSame) {
      EntryCount = 1;
      DataType[0] = HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_ACCESS_BANDWIDTH;
    } else {
      EntryCount = 2;
      DataType[0] = HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_READ_BANDWIDTH;
      DataType[1] = HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_WRITE_BANDWIDTH;
    }
    for (EntryIndex = 0; EntryIndex < EntryCount; EntryIndex++) {
      gBS->SetMem (Entries, sizeof (Entries[0]) * InitiatorDomainCount * TargetDomainCount, 0);
      for (i = 0; i < InitiatorDomainCount; i++) {
        for (j = 0; j < TargetDomainCount; j++) {
          InitiatorDomain = InitiatorDomainList[i];
          TargetDomain = TargetDomainList[j];
          if (EntryIndex == 0) {
            Bandwidth = TargetDomainReadBandwidth[j];
          } else {
            Bandwidth = TargetDomainWriteBandwidth[j];
          }
          if (CheckNumaOnSameSocket (FabricNuma, InitiatorDomain, TargetDomain) == FALSE) {
            // In other socket
            Bandwidth = Bandwidth < HalfxGMIBandwidth ? Bandwidth : HalfxGMIBandwidth;
          }
          Entries[i * TargetDomainCount + j] = (UINT16) (Bandwidth / ENTRY_BASE_UNIT_IN_MB);
        }
      }

      SysLocalInfoFlags.Value = 0;
      Status = HmatServices->AddSystemLocalityLatencyBandwidthInfo (
                              HmatServices,
                              SysLocalInfoFlags,
                              DataType[EntryIndex],
                              0,                         // MinTransferSize
                              InitiatorDomainCount,
                              TargetDomainCount,
                              ENTRY_BASE_UNIT_IN_MB,
                              InitiatorDomainList,
                              TargetDomainList,
                              Entries
                              );
      ASSERT (Status == EFI_SUCCESS);
    }
  } //if (InitiatorDomainList != NULL && TargetDomainList != NULL && Entries != NULL) {

  // Free the buffers
  if (InitiatorDomainList) {
    gBS->FreePool (InitiatorDomainList);
  }
  if (TargetDomainList) {
    gBS->FreePool (TargetDomainList);
  }
  if (Entries) {
    gBS->FreePool (Entries);
  }
  gBS->FreePool (MemDmiInfo);

  IDS_HDT_CONSOLE (MAIN_FLOW, "%a - Exit\n", __FUNCTION__);
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  GetCcxAvailableDomainMap
 *
 *  Description:
 *    Return the domain bitmap that CCX is available in the domain
 *
 *  Parameters:
 *    @param[in]     FabricNuma              Pointer to the FABRIC_NUMA_SERVICES_PROTOCOL
 *    @param[out]    CcxAvailableDomainMap   The returned bitmap
 *
 *    @retval        EFI_SUCCESS             The bitmap is returned successfully
 *    @retval        EFI_INVALID_PARAMETER   Failed to get the bitmap
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
GetCcxAvailableDomainMap (
  IN  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology,
  IN  FABRIC_NUMA_SERVICES_PROTOCOL            *FabricNuma,
  OUT UINT64                                   *CcxAvailableDomainMap
  )
{
  UINTN                                    SocketId;
  UINTN                                    CddId;
  UINTN                                    NumberOfSockets;
  UINTN                                    NumberOfCdds;
  UINTN                                    CddsPresent; 
  UINTN                                    NumberOfComplexes;
  UINTN                                    NumberOfCores;
  UINTN                                    NumberOfThreads;
  UINT32                                   Domain;
  EFI_STATUS                               CalledStatus;
  HYGON_CORE_TOPOLOGY_SERVICES_PROTOCOL    *CoreTopologyServices;

  if (!FabricNuma || !CcxAvailableDomainMap) {
    return EFI_INVALID_PARAMETER;
  }

  *CcxAvailableDomainMap = 0;

  CalledStatus = gBS->LocateProtocol (&gHygonCoreTopologyServicesProtocolGuid, NULL, (VOID **)&CoreTopologyServices);
  if (CalledStatus != EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Not found gHygonCoreTopologyServicesProtocolGuid \n");
    return EFI_INVALID_PARAMETER;
  }

  CalledStatus = FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);
  if (CalledStatus != EFI_SUCCESS) {
    return EFI_INVALID_PARAMETER;
  }

  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    if (FabricTopology->GetCddInfo (FabricTopology, SocketId, &NumberOfCdds, &CddsPresent) == EFI_SUCCESS) {
      for (CddId = 0; CddId < MAX_CDDS_PER_SOCKET; CddId++) {
        if (!IS_CDD_PRESENT (CddId, CddsPresent)) {
          continue;
        }

        if (CoreTopologyServices->GetCoreTopologyOnCdd (CoreTopologyServices, SocketId, CddId, 
                  &NumberOfComplexes, &NumberOfCores, &NumberOfThreads) == EFI_SUCCESS) {
          if (FabricNuma->DomainXlat (FabricNuma, SocketId, CddId, &Domain) == EFI_SUCCESS) {
            if (NumberOfComplexes > 0) {
              *CcxAvailableDomainMap |= LShiftU64 (1, Domain);
            }
          }
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  GetCpuDramAvailableDomainMap
 *
 *  Description:
 *    Return the domain bitmap that CPU attached DRAM is available in the domain
 *
 *  Parameters:
 *    @param[in]     FabricNuma              Pointer to the FABRIC_NUMA_SERVICES_PROTOCOL
 *    @param[out]    DramAvailableDomainMap  The returned bitmap
 *
 *    @retval        EFI_SUCCESS             The bitmap is returned successfully
 *    @retval        EFI_INVALID_PARAMETER   Failed to get the bitmap
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
GetCpuDramAvailableDomainMap (
  IN  FABRIC_NUMA_SERVICES_PROTOCOL          *FabricNuma,
  OUT UINT64                                 *DramAvailableDomainMap
  )
{
  UINT32                                         i;
  UINT32                                         SocketId;
  UINT32                                         LogicalDieId;
  UINT32                                         CddId;
  UINT32                                         Domain;
  DRAM_BASE_ADDRESS_REGISTER                     DramBaseAddr;
  DRAM_LIMIT_ADDRESS_REGISTER                    DramLimitAddr;

  if (!FabricNuma || !DramAvailableDomainMap) {
    return EFI_INVALID_PARAMETER;
  }

  *DramAvailableDomainMap = 0;

  for (i = 0; i < NUMBER_OF_DRAM_REGIONS; i++) {
    DramBaseAddr.Value = FabricRegisterAccRead (0, 0, DRAMBASEADDR0_FUNC, (DRAMBASEADDR0_REG + (i * DRAM_REGION_REGISTER_OFFSET)), IOMS0_INSTANCE_ID);
    if (DramBaseAddr.Field.AddrRngVal == 1) {
      DramLimitAddr.Value = FabricRegisterAccRead (0, 0, DRAMLIMITADDR0_FUNC, (DRAMLIMITADDR0_REG + (i * DRAM_REGION_REGISTER_OFFSET)), IOMS0_INSTANCE_ID);
      FabricTopologyGetLocationFromFabricId (DramLimitAddr.HyGxField.DstFabricID, &SocketId, &LogicalDieId, NULL);
      if (LogicalDieId >= FABRIC_ID_CDD0_DIE_NUM) {
        CddId = (UINT32)GetCddPhysicalDieId (LogicalDieId) - FABRIC_ID_CDD0_DIE_NUM;
        if (FabricNuma->DomainXlat (FabricNuma, SocketId, CddId, &Domain) == EFI_SUCCESS) {
          *DramAvailableDomainMap |= LShiftU64 (1, Domain);
        }
      }
    }
  }

  return EFI_SUCCESS;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  GetDramFreq
 *
 *  Description:
 *    Get the Dram frequency of given socket number and channel number
 *
 *  Parameters:
 *    @param[in]     DmiInfo                 The DMI information buffer
 *    @param[in]     Socket                  The socket number
 *    @param[in]     Channel                 The channel number
 *
 *    @return The DRAM frequency
 *
 *---------------------------------------------------------------------------------------
 */
UINT32
GetDramFreq (
  IN DMI_INFO        *DmiInfo,
  IN UINT32          Socket,
  IN UINT32          Channel
  )
{
  UINT32 Dimm;
  UINT32 Freq;

  Freq = (UINT32) -1;
  for (Dimm = 0; Dimm < MAX_DIMMS_PER_CHANNEL; Dimm++) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Socket %d  Channel %d  Dimm %d : \n", Socket, Channel, Dimm);
    IDS_HDT_CONSOLE (MAIN_FLOW, "    MemorySize = 0x%X \n", DmiInfo->T17[Socket][Channel][Dimm].MemorySize);
    IDS_HDT_CONSOLE (MAIN_FLOW, "    ConfigSpeed = 0x%X \n", DmiInfo->T17[Socket][Channel][Dimm].ConfigSpeed);

    if (DmiInfo->T17[Socket][Channel][Dimm].MemorySize > 0) {
      if (DmiInfo->T17[Socket][Channel][Dimm].ConfigSpeed < Freq) {
        Freq = DmiInfo->T17[Socket][Channel][Dimm].ConfigSpeed;
      }
    }
  }
  if (Freq == (UINT32) -1) {
    Freq = 0;
  }

  Freq = Freq * 2; //double the rate for DDR
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Socket %d  Channel %d  Freq = %d\n", Socket, Channel, Freq);

  return Freq;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  GetDramPeakBandwidthInGroupInMB
 *
 *  Description:
 *    Get the peak Dram frequency of the group of given CsMap
 *
 *  Parameters:
 *    @param[in]     DmiInfo                 The DMI information buffer
 *    @param[in]     CsMap                   The CS bitmap
 *
 *    @return The DRAM frequency of the group
 *
 *---------------------------------------------------------------------------------------
 */
UINT32
GetDramPeakBandwidthInGroupInMB (
  IN HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL  *FabricTopology,
  IN DMI_INFO                                 *DmiInfo,
  IN DOMAIN_INFO                              *DomainInfo
  )
{
  UINTN                                     SocketId;
  UINTN                                     CddId;
  UINTN                                     LogicalCddId;
  UINTN                                     NumberOfCdds;
  UINTN                                     CddsPresent; 
  UINT32                                    ChannelId;
  UINT32                                    ChannelCount;
  UINT32                                    MinFreq;
  UINT32                                    Freq;
  UINTN                                     NumberOfSockets;
  EFI_STATUS                                CalledStatus;
  
  ChannelCount = 0;
  MinFreq = (UINT32) -1;

  CalledStatus = FabricTopology->GetSystemInfo (FabricTopology, &NumberOfSockets, NULL, NULL);
  if (CalledStatus != EFI_SUCCESS) {
    return EFI_INVALID_PARAMETER;
  }

  for (SocketId = 0; SocketId < NumberOfSockets; SocketId++) {
    LogicalCddId = 0;
    if (FabricTopology->GetCddInfo (FabricTopology, SocketId, &NumberOfCdds, &CddsPresent) == EFI_SUCCESS) {
      for (CddId = 0; CddId < MAX_CDDS_PER_SOCKET; CddId++) {
        if (!IS_CDD_PRESENT (CddId, CddsPresent)) {
          continue;
        }

        if ((SocketId == DomainInfo->Intlv.None.Socket) && (((1 << CddId) & DomainInfo->Intlv.None.CddMap) != 0)) {
          for (ChannelId = 0; ChannelId < MAX_CHANNELS_PER_CDD_HYGX; ChannelId++) {
            Freq = GetDramFreq (DmiInfo, (UINT32)SocketId, (UINT32)(LogicalCddId * MAX_CHANNELS_PER_CDD_HYGX + ChannelId));
            if (Freq) {
              ChannelCount++;
              if (Freq < MinFreq) {
                MinFreq = Freq;
              }
            }
          }
        }
        LogicalCddId++;
      }
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "  MinFreq %d  ChannelCount = %d\n\n", MinFreq, ChannelCount);

  // DRAM B/W = # of DRAM channels x min DDR x 8B.
  // E.g., for DDR5-4800 in 8-ch: DRAM B/W = 8 x 4.8 GT/s x 8B = 307.2 GB/s.
  return MinFreq * ChannelCount * 8;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  GetGopBandwidthInMB
 *
 *  Description:
 *    Get the Gop bandwidth
 *
 *    @return The Gop bandwidth in MB/s
 *
 *---------------------------------------------------------------------------------------
 */
UINT32
GetGopBandwidthInMB (
  VOID
  )
{
  EFI_STATUS                   Status = EFI_SUCCESS; 
  UINT32                       xHMIFreq = 0;
  CBS_CONFIG                   CbsSetupData;	
  UINTN                        CbsSetupVariableSize; 
  UINT64                       CbsMemberValue;
  UINT32                       CbsMemberSize;
  UINT32                       LinkCount;

  CbsSetupVariableSize = sizeof(CBS_CONFIG);
  Status = gRT->GetVariable (
                  L"HygonSetup",
                  &gCbsSystemConfigurationGuid,
                  NULL,
                  &CbsSetupVariableSize,
                  &CbsSetupData
                  );
  if (EFI_ERROR(Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  GetGopBandwidthInMB GetVariable error (%r), use default \n", Status);
    xHMIFreq = 28800;  //Default value
  } else {
    switch (CbsSetupData.CbsGnbxHmiPHYRateCFG) {
      case 0:
        xHMIFreq = 12800;
        break;
      case 1:
        xHMIFreq = 14400;
        break;
      case 2:
        xHMIFreq = 16000;
        break;
      case 3:
        xHMIFreq = 17600;
        break;
      case 4:
        xHMIFreq = 19200;
        break;
      case 5:
        xHMIFreq = 25600;
        break;
      case 6:
        xHMIFreq = 28800;
        break;
      case 7:
        xHMIFreq = 32000;
        break;
      case 255:
        xHMIFreq = 28800;
        break;
      default:
        xHMIFreq = 28800;
        break;
    }
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "  GOP Frequency = %d \n", xHMIFreq);

  // Xhmi Connect Type
  Status = HygonPspHpcbGetCbsCmnParameter (
    HPCB_TOKEN_CBS_CMN_XHMI_CONNECT_TYPE,
    &CbsMemberSize,
    &CbsMemberValue
  );
  if (EFI_ERROR(Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  GetGopBandwidthInMB Get XHMI_CONNECT_TYPE error (%r), use default \n", Status);
    LinkCount = 1;  //default value
  } else {
    LinkCount = (UINT32) CbsMemberValue;
  }
  IDS_HDT_CONSOLE (MAIN_FLOW, "  GOP Link count = %d \n", LinkCount);

  // Depends on Gop speed & # of links.
  // Gop B/W = link count * Gop speed x Gop lanes * DF efficiency / 8 (b to B)
  // E.g., for 32 Gbps GOP with four x16 links: GOP B/W = 4 x 32 Gbps * 16 * %75 / 8 = 192 GB/s..
  return (LinkCount * xHMIFreq * 16 * 3 / 4) / 8;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  Check NUMA node on same socket
 *
 *  Description:
 *
 *    @return TRUE Same socket
 *            FALSE Not Same socket
 *
 *---------------------------------------------------------------------------------------
 */
BOOLEAN
CheckNumaOnSameSocket (
  IN  FABRIC_NUMA_SERVICES_PROTOCOL          *FabricNuma,
  IN  UINT32                                 NumaNode0,
  IN  UINT32                                 NumaNode1
  )
{
  EFI_STATUS                                     Status;
  UINT32                                         NumOfDomain;
  UINT32                                         NumOfCxlDomain;
  DOMAIN_INFO                                    *DomainInfo;
  CXL_DOMAIN_INFO                                *CxlDomainInfo;
  
  Status = FabricNuma->GetDomainInfo (FabricNuma, &NumOfDomain, &DomainInfo, &NumOfCxlDomain, &CxlDomainInfo);
  if (Status != EFI_SUCCESS) {
    return TRUE;
  }

  if ((NumaNode0 < NumOfDomain) && (NumaNode1 < NumOfDomain)) {
    //Both Normal NUMA node
    if (DomainInfo[NumaNode0].Intlv.None.Socket == DomainInfo[NumaNode1].Intlv.None.Socket) {
      return TRUE;
    }
  }

  if ((NumaNode0 < NumOfDomain) && (NumaNode1 >= NumOfDomain)) {
    //Normal NUMA node and CXL NUMA node
    if (((1 << DomainInfo[NumaNode0].Intlv.None.Socket) & CxlDomainInfo[NumaNode1 - NumOfDomain].SocketMap) != 0) {
      return TRUE;
    }
  }

  if ((NumaNode0 >= NumOfDomain) && (NumaNode1 < NumOfDomain)) {
    //CXL NUMA node and Normal NUMA node
    if (((1 << DomainInfo[NumaNode1].Intlv.None.Socket) & CxlDomainInfo[NumaNode0 - NumOfDomain].SocketMap) != 0) {
      return TRUE;
    }
  }

  if ((NumaNode0 >= NumOfDomain) && (NumaNode1 >= NumOfDomain)) {
    //Both CXL NUMA node
    if ((CxlDomainInfo[NumaNode0 - NumOfDomain].SocketMap & CxlDomainInfo[NumaNode1 - NumOfDomain].SocketMap) != 0) {
      return TRUE;
    }
  }

  return FALSE;
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  GetCxlLatencyBandwidth
 *
 *  Description:
 *    Get the CXL latency and bandwidth in the given CsMap group with the contents in CDAT
 *
 *    @param[in]     CxlServices             The pointer to HYGON_NBIO_CXL20_SERVICES_PROTOCOL
 *    @param[in]     CsMap                   The CS bitmap
 *    @param[out]    ReadLatency             The pointer to the read latency value
 *    @param[out]    WriteLatency            The pointer to the write latency value
 *    @param[out]    ReadBandwidth           The pointer to the read bandwidth value
 *    @param[out]    WriteBandwidth          The pointer to the write bandwidth value
 *
 *---------------------------------------------------------------------------------------
 */
VOID
GetCxlLatencyBandwidth (
  IN  HYGON_NBIO_CXL20_SERVICES_PROTOCOL   *CxlServices,
  IN  CXL_DOMAIN_INFO                      *CxlDomainInfo,
  OUT UINT32                           *ReadLatency,
  OUT UINT32                           *WriteLatency,
  OUT UINT32                           *ReadBandwidth,
  OUT UINT32                           *WriteBandwidth
  )
{
  UINT32                              Index;
  UINT32                              CdatIndex;
  EFI_STATUS                          Status;
  CDAT_TABLE                          CdatTable;
  DSLBIS_CDAT                         Dslbis[MAX_DSLBIS_RECORDS_PER_CXL_DEVICE];
  UINT32                              *CdatReadLatency;
  UINT32                              *CdatWriteLatency;
  UINT32                              *CdatReadBandwidth;
  UINT32                              *CdatWriteBandwidth;
  UINT32                              CdatLatencyCount;
  UINT32                              CdatBandwidthCount;
  UINT32                              TempAccessLatency;
  UINT32                              TempReadLatency;
  UINT32                              TempWriteLatency;
  UINT32                              TempAccessBandwidth;
  UINT32                              TempReadBandwidth;
  UINT32                              TempWriteBandwidth;
  UINT64                              Value;
  UINT32                              CxlCount;
  PCI_ADDR                            EndPointBDF;
  UINTN                               HandleCount = 0;
  EFI_HANDLE                          *HandleBuffer = NULL;
  PCI_ADDR                            PciAddr; 
  UINTN                               Segment, Bus, Device, Function;
  UINT8                               ClassCode[3];
  EFI_PCI_IO_PROTOCOL                 *PciIo = NULL;
  HYGON_CXL_PORT_INFO_STRUCT          NbioPortInfo[16];
  UINT8                               SocketId, LogicalDieId, RbIndex;

  if (ReadLatency) {
    *ReadLatency = 0;
  }
  if (WriteLatency) {
    *WriteLatency = 0;
  }
  if (ReadBandwidth) {
    *ReadBandwidth = 0;
  }
  if (WriteBandwidth) {
    *WriteBandwidth = 0;
  }
  if (!CxlServices) {
    return;
  }

  // Scan CXL devices
  CxlCount = 0;
  gBS->SetMem (&NbioPortInfo[0], sizeof (NbioPortInfo), 0);
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiPciIoProtocolGuid, NULL, &HandleCount, &HandleBuffer);
  if(EFI_ERROR(Status)) {
    return;
  }
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
    if (EFI_ERROR(Status)) {
      continue;
    }
    
    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
    PciAddr.AddressValue = 0;
    PciAddr.Address.Segment = (UINT32) Segment;
    PciAddr.Address.Bus = (UINT32) Bus;
    PciAddr.Address.Device = (UINT32) Device;
    PciAddr.Address.Function = (UINT32) Function;

    Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 0x09, 3, &ClassCode[0]);
    if ((ClassCode[2] == PCI_CLASS_MEMORY_CONTROLLER) && (ClassCode[1] == PCI_CLASS_MEMORY_CXL)) {
      Status = CxlServices->GetCXL20PortRBLocation (CxlServices, (UINT8)PciAddr.Address.Bus, &SocketId, &LogicalDieId, &RbIndex);
      if (Status == EFI_SUCCESS) {
        NbioPortInfo[CxlCount].Invalid = 1;
        NbioPortInfo[CxlCount].SocketId = SocketId;
        NbioPortInfo[CxlCount].LogicalDieId = LogicalDieId;
        NbioPortInfo[CxlCount].RbId = RbIndex;
        NbioPortInfo[CxlCount].EndPointBDF.AddressValue = PciAddr.AddressValue;

        IDS_HDT_CONSOLE (MAIN_FLOW, "Found CXL device index %d addr [B%02X|D%02X|F%02X], on socket %d die %d NBIO %d \n", 
          CxlCount,
          PciAddr.Address.Bus, 
          PciAddr.Address.Device, 
          PciAddr.Address.Function, 
          SocketId, 
          LogicalDieId, 
          RbIndex);

        CxlCount++;
      }
    }
  }

  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (CdatReadLatency[0]) * CxlCount, (VOID **)&CdatReadLatency);
  if (Status != EFI_SUCCESS) {
    CdatReadLatency = NULL;
  }
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (CdatWriteLatency[0]) * CxlCount, (VOID **)&CdatWriteLatency);
  if (Status != EFI_SUCCESS) {
    CdatWriteLatency = NULL;
  }
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (CdatReadBandwidth[0]) * CxlCount, (VOID **)&CdatReadBandwidth);
  if (Status != EFI_SUCCESS) {
    CdatReadBandwidth = NULL;
  }
  Status = gBS->AllocatePool (EfiBootServicesData, sizeof (CdatWriteBandwidth[0]) * CxlCount, (VOID **)&CdatWriteBandwidth);
  if (Status != EFI_SUCCESS) {
    CdatWriteBandwidth = NULL;
  }

  CdatLatencyCount = 0;
  CdatBandwidthCount = 0;
  if (CdatReadLatency && CdatWriteLatency && CdatReadBandwidth && CdatWriteBandwidth) {
    for (Index = 0; Index < CxlCount; Index++) {
      if (NbioPortInfo[Index].Invalid != 1) {
        continue;
      }
      if ((((1 << NbioPortInfo[Index].SocketId) & CxlDomainInfo->SocketMap) == 0) ||
        (((1 << NbioPortInfo[Index].LogicalDieId) & CxlDomainInfo->DieMap) == 0)) {
        continue;
      }

      TempAccessLatency   = 0;
      TempReadLatency     = 0;
      TempWriteLatency    = 0;
      TempAccessBandwidth = 0;
      TempReadBandwidth   = 0;
      TempWriteBandwidth  = 0;

      EndPointBDF.AddressValue = NbioPortInfo[Index].EndPointBDF.AddressValue;
      IDS_HDT_CONSOLE (MAIN_FLOW, "CXL device %u in NUMA node %d \n", Index, CxlDomainInfo->CxlDomain);

      gBS->SetMem (&CdatTable, sizeof (CdatTable), 0);
      Status = CxlServices->CxlGetCdat (CxlServices, EndPointBDF.AddressValue, &CdatTable);
      if (Status == EFI_SUCCESS) {
        gBS->SetMem (&Dslbis, sizeof (Dslbis), 0);
        Status = CxlServices->CxlParseCdat (CxlServices, CdatTable, CdatTypeDslbis, Dslbis);
        if (Status == EFI_SUCCESS) {
          for (CdatIndex = 0; CdatIndex < MAX_DSLBIS_RECORDS_PER_CXL_DEVICE; CdatIndex++) {
            if (Dslbis[CdatIndex].Length == 0) {
              continue;
            }
            Value =  ((UINT64) Dslbis[CdatIndex].Entry[0]) * Dslbis[CdatIndex].EntryBaseUnit;
            switch (Dslbis[CdatIndex].DataType) {
            case HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_ACCESS_LATENCY:
              TempAccessLatency = (UINT32) Value;
              break;
            case HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_READ_LATENCY:
              TempReadLatency = (UINT32) Value;
              break;
            case HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_WRITE_LATENCY:
              TempWriteLatency = (UINT32) Value;
              break;
            case HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_ACCESS_BANDWIDTH:
              TempAccessBandwidth = (UINT32) Value;
              break;
            case HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_READ_BANDWIDTH:
              TempReadBandwidth = (UINT32) Value;
              break;
            case HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_DATA_TYPE_WRITE_BANDWIDTH:
              TempWriteBandwidth = (UINT32) Value;
              break;
            default:
              ASSERT (FALSE);
            }
          }
        }
        IDS_HDT_CONSOLE (MAIN_FLOW, "  CDAT DSLBIS\n");
        IDS_HDT_CONSOLE (MAIN_FLOW, "    AccessLatency   = %u\n", TempAccessLatency);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    ReadLatency     = %u\n", TempReadLatency);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    WriteLatency    = %u\n", TempWriteLatency);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    AccessBandwidth = %u\n", TempAccessBandwidth);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    ReadBandwidth   = %u\n", TempReadBandwidth);
        IDS_HDT_CONSOLE (MAIN_FLOW, "    WriteBandwidth  = %u\n", TempWriteBandwidth);
        if (CdatTable.Entries != NULL) {
          gBS->FreePool (CdatTable.Entries);
        }
      }

      if (TempAccessLatency == 0) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Use default access latency \n");
        TempAccessLatency = CXL_DEV_DEFAULT_LATENCY_PS;
      }
      // Use the access latency if no read latency or write latency
      // Save the latency for the CXL device
      if (TempReadLatency == 0) {
        TempReadLatency = TempAccessLatency;
      }
      if (TempWriteLatency == 0) {
        TempWriteLatency = TempAccessLatency;
      }
      CdatReadLatency[CdatLatencyCount] = TempReadLatency;
      CdatWriteLatency[CdatLatencyCount] = TempWriteLatency;
      IDS_HDT_CONSOLE (MAIN_FLOW, "  CdatReadLatency[%u]   = %u\n", CdatLatencyCount, CdatReadLatency[CdatLatencyCount]);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  CdatWriteLatency[%u]  = %u\n", CdatLatencyCount, CdatWriteLatency[CdatLatencyCount]);
      CdatLatencyCount++;

      if (TempAccessBandwidth == 0) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "  Use default bandwidth \n");
        TempAccessBandwidth = CXL_DEV_DEFAULT_BANDWIDTH_MB;
      }
      // Use the access bandwidth if no read bandwidth or write bandwidth
      // Save the bandwidth for the CXL device
      if (TempReadBandwidth == 0) {
        TempReadBandwidth = TempAccessBandwidth;
      }
      if (TempWriteBandwidth == 0) {
        TempWriteBandwidth = TempAccessBandwidth;
      }
      CdatReadBandwidth[CdatBandwidthCount] = TempReadBandwidth;
      CdatWriteBandwidth[CdatBandwidthCount] = TempWriteBandwidth;
      IDS_HDT_CONSOLE (MAIN_FLOW, "  CdatReadBandwidth[%u]   = %u\n", CdatBandwidthCount, CdatReadBandwidth[CdatBandwidthCount]);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  CdatWriteBandwidth[%u]  = %u\n", CdatBandwidthCount, CdatWriteBandwidth[CdatBandwidthCount]);
      CdatBandwidthCount++;
    }
  } //if (CdatReadLatency && CdatWriteLatency && CdatReadBandwidth && CdatWriteBandwidth)

  // Use the max latency
  TempReadLatency = 0;
  TempWriteLatency = 0;
  ASSERT (CdatLatencyCount != 0);
  if (CdatLatencyCount) {
    for (Index = 0; Index < CdatLatencyCount; Index++) {
      if (CdatReadLatency[Index] > TempReadLatency) {
        TempReadLatency = CdatReadLatency[Index];
      }
      if (CdatWriteLatency[Index] > TempWriteLatency) {
        TempWriteLatency = CdatWriteLatency[Index];
      }
    }
    ASSERT (TempReadLatency != 0);
    ASSERT (TempWriteLatency != 0);
    if (ReadLatency) {
      *ReadLatency = TempReadLatency;
    }
    if (WriteLatency) {
      *WriteLatency = TempWriteLatency;
    }
    IDS_HDT_CONSOLE (MAIN_FLOW, "Min Read Latency  = %d\n", TempReadLatency);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Min Write Latency = %d\n", TempWriteLatency);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Latency Count     = %d\n", CdatLatencyCount);
  }

  // min_reported_bw x # of devices.
  TempReadBandwidth = (UINT32) -1;
  TempWriteBandwidth = (UINT32) -1;
  ASSERT (CdatBandwidthCount != 0);
  if (CdatBandwidthCount) {
    for (Index = 0; Index < CdatBandwidthCount; Index++) {
      if (CdatReadBandwidth[Index] < TempReadBandwidth) {
        TempReadBandwidth = CdatReadBandwidth[Index];
      }
      if (CdatWriteBandwidth[Index] < TempWriteBandwidth) {
        TempWriteBandwidth = CdatWriteBandwidth[Index];
      }
    }
    ASSERT (TempReadBandwidth != (UINT32) -1);
    ASSERT (TempWriteBandwidth != (UINT32) -1);
    if (ReadBandwidth) {
      *ReadBandwidth = TempReadBandwidth * CdatBandwidthCount;
    }
    if (WriteBandwidth) {
      *WriteBandwidth = TempWriteBandwidth * CdatBandwidthCount;
    }
    IDS_HDT_CONSOLE (MAIN_FLOW, "Min Read Bandwidth  = %d\n", TempReadBandwidth);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Min Write Bandwidth = %d\n", TempWriteBandwidth);
    IDS_HDT_CONSOLE (MAIN_FLOW, "Bandwidth Count     = %d\n", CdatBandwidthCount);
  }

  if (CdatReadLatency) {
    gBS->FreePool (CdatReadLatency);
  }
  if (CdatWriteLatency) {
    gBS->FreePool (CdatWriteLatency);
  }
  if (CdatReadBandwidth) {
    gBS->FreePool (CdatReadBandwidth);
  }
  if (CdatWriteBandwidth) {
    gBS->FreePool (CdatWriteBandwidth);
  }
}