/* $NoKeywords:$ */
/**
 * @file
 *
 * AmdNbioPcieZPPei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioPcieZPPei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
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
#include <PiPei.h>
#include <Filecode.h>
#include <GnbDxio.h>
#include <GnbRegistersZP.h>
//In AgesaPkg
#include <AmdPcieComplex.h>
#include <Ppi/NbioPcieComplexPpi.h>
#include <Ppi/AmdMemoryInfoHobPpi.h>

#include "AmdNbioPcieZPPei.h"


#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/NbioBaseServicesZPPpi.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>

#include <Library/AmdBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/IdsLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Library/DxioLibV1.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/AmdSocBaseLib.h>


#define FILECODE        NBIO_PCIE_AMDNBIOPCIEZPPEI_AMDNBIOPCIEZPPEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

STATIC EFI_PEI_NOTIFY_DESCRIPTOR   mDxioInitCallbackPpi[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH,
    &gAmdMemoryInfoHobPpiGuid,
    DxioInitializationCallbackPpi
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH,
    &gAmdNbioSmuServicesPpiGuid,
    DxioInitializationCallbackPpi
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gAmdNbioPcieComplexPpiGuid,
    DxioInitializationCallbackPpi
  }
};


EFI_STATUS
PcieGetTopology (
  IN       PEI_AMD_NBIO_PCIE_SERVICES_PPI  *This,
     OUT   GNB_PCIE_INFORMATION_DATA_HOB   **DebugOptions
  )
{
  EFI_STATUS                     Status;
  EFI_PEI_HOB_POINTERS           Hob;
  GNB_PCIE_INFORMATION_DATA_HOB  *GnbPcieInfoDataHob;

  GnbPcieInfoDataHob = NULL;

  Status = PeiServicesGetHobList (&Hob.Raw);

  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION &&
        CompareGuid ( &Hob.Guid->Name, &gGnbPcieHobInfoGuid)) {
      GnbPcieInfoDataHob = (GNB_PCIE_INFORMATION_DATA_HOB *) (Hob.Raw);
      Status = EFI_SUCCESS;
      break;
    }
    Hob.Raw = GET_NEXT_HOB (Hob);
  }
  *DebugOptions = GnbPcieInfoDataHob;

  return Status;
}

EFI_STATUS
PcieSetSpeed (
  IN  PEI_AMD_NBIO_PCIE_SERVICES_PPI  *This,
  IN  CONST EFI_PEI_SERVICES          **PeiServices,
  IN  UINT8                           PciDevice,
  IN  UINT8                           PciFunction,
  IN  UINT8                           TargetSpeed
  )
{
  EFI_STATUS                     Status;
  GNB_HANDLE                     *GnbHandle;
  PCIe_PLATFORM_CONFIG           *Pcie;
  GNB_PCIE_INFORMATION_DATA_HOB  *PciePlatformConfigHob;
  PEI_AMD_NBIO_SMU_SERVICES_PPI  *SmuServicesPpi;
  UINT32                         EngineId;
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;

  IDS_HDT_CONSOLE (MAIN_FLOW, "PcieSetSpeed Entry for Device %d Function %d to Gen%d\n", PciDevice, PciFunction, TargetSpeed);
  Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdNbioSmuServicesPpiGuid, 0, NULL, &SmuServicesPpi);

  PcieGetTopology (This, &PciePlatformConfigHob);
  Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);
  EngineId = 0xFF;

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if ((PcieEngine->Type.Port.PortData.DeviceNumber == PciDevice)
          && (PcieEngine->Type.Port.PortData.FunctionNumber == PciFunction)
          && ((PcieEngine->Header.DescriptorFlags & DESCRIPTOR_ALLOCATED) == DESCRIPTOR_ALLOCATED)) {
        EngineId = PcieEngine->Type.Port.PcieBridgeId;
        break;
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    if (EngineId != 0xFF) {
      break;
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

//  EngineId = 3; //Temporary for PT debug
  DxioSetLinkSpeedV1 (PeiServices, SmuServicesPpi, GnbHandle->InstanceId, EngineId, TargetSpeed);

  return Status;
}

EFI_STATUS
PcieConfigurationInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

STATIC PEI_AMD_NBIO_PCIE_SERVICES_PPI mNbioPcieServicePpi = {
  AMD_NBIO_PCIE_SERVICES_REVISION,  ///< revision
  PcieGetTopology,
  PcieSetSpeed
};

STATIC EFI_PEI_PPI_DESCRIPTOR mNbioPcieServicesPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gAmdNbioPcieServicesPpiGuid,
  &mNbioPcieServicePpi
};

EFI_STATUS
EFIAPI
AmdNbioPcieZPPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS       Status;

  AGESA_TESTPOINT (TpNbioPCIePeiEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioPcieZPPeiEntry Entry\n");

  // Create and Initialize Topology Structure (in HOB)
  PcieConfigurationInit (PeiServices);

  // Install PPI to provide Topology to other drivers
  Status = (**PeiServices).InstallPpi (PeiServices, &mNbioPcieServicesPpiList);

  // Install PPI to provide DPC status report service
  PcieDpcStatusServiceInit (PeiServices);

  // Publish callback for all services needed to initialize DXIO
  Status = (**PeiServices).NotifyPpi (PeiServices, mDxioInitCallbackPpi);
  IDS_HDT_CONSOLE (MAIN_FLOW, "AmdNbioPcieZPPeiEntry Exit\n");
  AGESA_TESTPOINT (TpNbioPCIePeiExit, NULL);
  return EFI_SUCCESS;
}

#define PcieConfigAttachChild(P, C)  (P)->Child = (UINT16) ((UINT8 *) C - (UINT8 *) P);
#define PcieConfigAttachParent(P, C) (C)->Parent = (UINT16) ((UINT8 *) C - (UINT8 *) P);
/*----------------------------------------------------------------------------------------*/
/**
 * Attach descriptors
 *
 *
 * @param[in]      Type       Descriptor type
 * @param[in,out]  Base       Base descriptor
 * @param[in,out]  New      New  descriptor
 */
VOID
STATIC
PcieConfigAttachDescriptors (
  IN       UINT32                         Type,
  IN OUT   PCIe_DESCRIPTOR_HEADER         *Base,
  IN OUT   PCIe_DESCRIPTOR_HEADER         *New
  )
{
  PCIe_DESCRIPTOR_HEADER  *Left;
  PCIe_DESCRIPTOR_HEADER  *Right;

  Left = PcieConfigGetPeer (DESCRIPTOR_TERMINATE_GNB, PcieConfigGetChild (Type, Base));
  ASSERT (Left != NULL);
  Right = PcieConfigGetChild (Type, New);
  Left->Peer = (UINT16) ((UINT8 *) Right - (UINT8 *) Left);
  PcieConfigResetDescriptorFlags (Left, DESCRIPTOR_TERMINATE_TOPOLOGY);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Attach configurations of two GNB to each other.
 *
 * Function will link all data structure to linked lists
 *
 * @param[in,out]  Base       Base complex descriptor
 * @param[in,out]  New        New complex descriptor
 */
VOID
STATIC
PcieConfigAttachComplexes (
  IN OUT   PCIe_COMPLEX_CONFIG            *Base,
  IN OUT   PCIe_COMPLEX_CONFIG            *New
  )
{
  // Connect Complex
  Base->Header.Peer = (UINT16) ((UINT8 *) New - (UINT8 *) Base);
  PcieConfigResetDescriptorFlags (Base, DESCRIPTOR_TERMINATE_TOPOLOGY);
  // Connect Silicon
  PcieConfigAttachDescriptors (DESCRIPTOR_SILICON, &Base->Header, &New->Header);
  // Connect Wrappers
  PcieConfigAttachDescriptors (DESCRIPTOR_PCIE_WRAPPER, &Base->Header, &New->Header);
  // Connect Engines
  PcieConfigAttachDescriptors (DESCRIPTOR_PCIE_ENGINE, &Base->Header, &New->Header);
}

/*----------------------------------------------------------------------------------------*/
/**
 * Create internal PCIe configuration topology
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @retval     EFI_STATUS
 */

EFI_STATUS
PcieConfigurationInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS            Status;
  AMD_PEI_FABRIC_TOPOLOGY_SERVICES_PPI *FabricTopologyServicesPpi;
  PEI_AMD_NBIO_BASE_SERVICES_PPI  *NbioBaseServicesPpi;
  PCIe_PLATFORM_CONFIG            *Pcie;
  PCIe_SILICON_CONFIG             *Silicon;
  GNB_PCIE_INFORMATION_DATA_HOB   *GnbPcieInfoDataHob;
  AMD_FABRIC_TOPOLOGY_PROCESSOR_NEIGHBOR_INFO   *NeighborInfo;
  AMD_FABRIC_TOPOLOGY_DIE_DEVICE_MAP      *FabricIdMap;
  UINTN                           ComplexesDataLength;
  UINTN                           ComplexIndex;
  VOID                            *Buffer;
  UINTN                           NumberOfProcessors;
  UINTN                           TotalNumberOfDie;
  UINTN                           TotalNumberOfRootBridges;
  UINTN                           NumberOfNeighbors;
  UINTN                           SystemIdOffset;
  UINTN                           SystemFabricID;
  UINTN                           BusNumberBase;
  UINTN                           BusNumberLimit;
  UINTN                           SocketIndex;
  UINTN                           DieIndex;
  UINTN                           RootBridgeIndex;
    
  Status = EFI_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationInit Enter\n");
  // Get number of sockets from CPU topology PPI
  // Locate CPU topology PPI
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdFabricTopologyServicesPpiGuid,
                             0,
                             NULL,
                             &FabricTopologyServicesPpi
                             );
  FabricTopologyServicesPpi->GetSystemInfo (&NumberOfProcessors, &TotalNumberOfDie, &TotalNumberOfRootBridges);

  // Locate NBIO APU info PPI
  // Get PCIe Complex info
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gAmdNbioBaseServicesZpPpiGuid,
                             0,
                             NULL,
                             &NbioBaseServicesPpi
                             );

  // Allocate PCIe Complex space
  ComplexesDataLength = NbioBaseServicesPpi->PcieComplexSize * TotalNumberOfRootBridges;

  Status = PeiServicesCreateHob (
    EFI_HOB_TYPE_GUID_EXTENSION,
    sizeof (GNB_PCIE_INFORMATION_DATA_HOB) + (UINT16) ComplexesDataLength,
    &GnbPcieInfoDataHob
    );

  GnbPcieInfoDataHob->EfiHobGuidType.Name = gGnbPcieHobInfoGuid;
  Pcie = &(GnbPcieInfoDataHob->PciePlatformConfigHob);
  ZeroMem (Pcie, sizeof (GNB_PCIE_INFORMATION_DATA_HOB));

  // Build PCIe Complex
  ComplexIndex = 0;
  if (Pcie != NULL) {
    PcieConfigAttachChild (&Pcie->Header, &Pcie->ComplexList[ComplexIndex].Header);
    PcieConfigSetDescriptorFlags (Pcie, DESCRIPTOR_PLATFORM | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_TOPOLOGY);
    Buffer = (UINT8 *) (Pcie) + sizeof (PCIe_PLATFORM_CONFIG);
    for (SocketIndex = 0; SocketIndex < NumberOfProcessors; SocketIndex++) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Process Socket %d\n", SocketIndex);
      FabricTopologyServicesPpi->GetProcessorInfo (SocketIndex,
                                                  &TotalNumberOfDie,
                                                  &TotalNumberOfRootBridges,
                                                  &NumberOfNeighbors,
                                                  &NeighborInfo);
      for (DieIndex = 0; DieIndex < TotalNumberOfDie; DieIndex++) {
        IDS_HDT_CONSOLE (GNB_TRACE, "    Process Die %d\n", DieIndex);
        FabricTopologyServicesPpi->GetDieInfo (SocketIndex,
                                              DieIndex,
                                              &TotalNumberOfRootBridges,
                                              &SystemIdOffset,
                                              &FabricIdMap,
                                              &NumberOfNeighbors,
                                              (AMD_FABRIC_TOPOLOGY_DIE_NEIGHBOR_INFO**)&NeighborInfo
                                              );
        for (RootBridgeIndex = 0; RootBridgeIndex < TotalNumberOfRootBridges; RootBridgeIndex++) {
          IDS_HDT_CONSOLE (GNB_TRACE, "      Process  RootBridge %d\n", RootBridgeIndex);
          FabricTopologyServicesPpi->GetRootBridgeInfo (SocketIndex,
                                                DieIndex,
                                                RootBridgeIndex,
                                                &SystemFabricID,
                                                &BusNumberBase,
                                                &BusNumberLimit
                                                );

          IDS_HDT_CONSOLE (GNB_TRACE, "        Bus Number 0x%x\n", BusNumberBase);
          //Attach Complex to Silicon which will be created by PcieFmBuildComplexConfiguration
          PcieConfigAttachChild (&Pcie->ComplexList[ComplexIndex].Header, &((PCIe_SILICON_CONFIG *) Buffer)->Header);
          //Attach Complex to Pcie
          PcieConfigAttachParent (&Pcie->Header, &Pcie->ComplexList[ComplexIndex].Header);
          PcieConfigSetDescriptorFlags (&Pcie->ComplexList[ComplexIndex], DESCRIPTOR_COMPLEX | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY);
          //Add copy from PcieComplex to Buffer using memcopy
          CopyMem (Buffer, NbioBaseServicesPpi->PcieComplexData, NbioBaseServicesPpi->PcieComplexSize);
          Silicon = PcieConfigGetChildSilicon (&Pcie->ComplexList[ComplexIndex]);
          Silicon->SocketId = (UINT8) SocketIndex;
          Silicon->DieNumber = (UINT8) DieIndex;
          Silicon->RBIndex = (UINT8) RootBridgeIndex;
          Silicon->InstanceId = (UINT8) ComplexIndex;
          Silicon->Address.Address.Bus = BusNumberBase;
          
          IDS_HDT_CONSOLE (GNB_TRACE, "Silicon->Address = 0x%x\n", Silicon->Address.AddressValue);
          
                      
          Silicon->StartLane += (UINT16) ((Silicon->InstanceId - (SocketIndex * TotalNumberOfDie)) * 32);
          Silicon->EndLane += (UINT16) ((Silicon->InstanceId - (SocketIndex * TotalNumberOfDie)) * 32);
          IDS_HDT_CONSOLE (GNB_TRACE, "Silicon->StartLane = %d \n", Silicon->StartLane);
          IDS_HDT_CONSOLE (GNB_TRACE, "Silicon->EndLane = %d \n", Silicon->EndLane);
          
          while (Silicon != NULL) {
            PcieConfigAttachParent (&Pcie->ComplexList[ComplexIndex].Header, &Silicon->Header);
            Silicon = (PCIe_SILICON_CONFIG *) PcieConfigGetNextTopologyDescriptor (Silicon, DESCRIPTOR_TERMINATE_TOPOLOGY);
          }

          if (ComplexIndex > 0) {
            PcieConfigAttachComplexes (&Pcie->ComplexList[ComplexIndex - 1], &Pcie->ComplexList[ComplexIndex]);
          }
          Buffer = (VOID *) ((UINT8 *) Buffer + NbioBaseServicesPpi->PcieComplexSize);
          ComplexIndex++;
        }
      }
    }
  }

  PcieConfigDebugDump (Pcie);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationInit Exit [0x%x]\n", Status);
  return Status;
}
