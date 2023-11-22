/* $NoKeywords:$ */
/**
 * @file
 *
 * HygonNbioPciePei Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioPciePei
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
#include <PiPei.h>
#include <Filecode.h>
#include <GnbHsio.h>
#include <GnbRegisters.h>
//In HgpiPkg
#include <HygonPcieComplex.h>
#include <Ppi/NbioPcieComplexPpi.h>
#include <Ppi/HygonMemoryInfoHobPpi.h>

#include "HygonNbioPciePei.h"


#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/NbioBaseServicesSTPpi.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>

#include <Library/HygonBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/IdsLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/HobLib.h>
#include <Hsio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/HygonSocBaseLib.h>
#include "PcieComplexData.h"


#define FILECODE        NBIO_PCIE_HYEX_HYGONNBIOPCIESTPEI_HYGONNBIOPCIESTPEI_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */
extern ST_COMPLEX_CONFIG             ComplexDataST;
extern ST_COMPLEX_CONFIG_MASTER_NBIO ComplexDataMasterST;
extern
VOID
HsioSetLinkSpeed (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       GNB_HANDLE                      *GnbHandle,
  IN       UINT32                          EngineId,
  IN       UINT32                          TargetSpeed
  );

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

STATIC EFI_PEI_NOTIFY_DESCRIPTOR   mHsioInitCallbackPpi[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH,
    &gHygonMemoryInfoHobPpiGuid,
    HsioInitializationCallbackPpi
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH,
    &gHygonNbioPcieTrainingDonePpiGuid,
    ClearPCIeTrainingErrorsCallback
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH,
    &gHygonNbioSmuServicesPpiGuid,
    HsioInitializationCallbackPpi
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_DISPATCH | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gHygonNbioPcieComplexPpiGuid,
    HsioInitializationCallbackPpi
  }
};

EFI_STATUS
PcieConfigurationInit (
  IN CONST EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
PcieGetTopology (
  IN       PEI_HYGON_NBIO_PCIE_SERVICES_PPI  *This,
  OUT   GNB_PCIE_INFORMATION_DATA_HOB   **DebugOptions
  );

EFI_STATUS
PcieSetSpeed (
  IN  PEI_HYGON_NBIO_PCIE_SERVICES_PPI  *This,
  IN  CONST EFI_PEI_SERVICES          **PeiServices,
  IN  UINT8                           PciDevice,
  IN  UINT8                           PciFunction,
  IN  UINT8                           TargetSpeed
  );

EFI_STATUS
HygonGetGnbhandleBySystemLogicalRbId (
  IN  PEI_HYGON_NBIO_PCIE_SERVICES_PPI       *This,
  IN  CONST EFI_PEI_SERVICES               **PeiServices,
  IN  UINTN                                SystemLogicalRbId,
  OUT GNB_HANDLE                           **MyHandle
  );

STATIC PEI_HYGON_NBIO_PCIE_SERVICES_PPI  mNbioPcieServicePpi = {
  HYGON_NBIO_PCIE_SERVICES_REVISION,  ///< revision
  PcieGetTopology,
  PcieSetSpeed,
  HygonGetGnbhandleBySystemLogicalRbId
};

STATIC EFI_PEI_PPI_DESCRIPTOR mNbioPcieServicesPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonNbioPcieServicesPpiGuid,
  &mNbioPcieServicePpi
};

EFI_STATUS
PcieGetTopology (
  IN       PEI_HYGON_NBIO_PCIE_SERVICES_PPI  *This,
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
  IN  PEI_HYGON_NBIO_PCIE_SERVICES_PPI  *This,
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
  PEI_HYGON_NBIO_SMU_SERVICES_PPI  *SmuServicesPpi;
  UINT32                         EngineId;
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;

  IDS_HDT_CONSOLE (MAIN_FLOW, "PcieSetSpeed Entry for Device %d Function %d to Gen%d\n", PciDevice, PciFunction, TargetSpeed);
  Status = (*PeiServices)->LocatePpi (PeiServices, &gHygonNbioSmuServicesPpiGuid, 0, NULL, &SmuServicesPpi);

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

  HsioSetLinkSpeed (PeiServices, GnbHandle, EngineId, TargetSpeed);

  return Status;
}

EFI_STATUS
HygonGetGnbhandleBySystemLogicalRbId (
  IN  PEI_HYGON_NBIO_PCIE_SERVICES_PPI       *This,
  IN  CONST EFI_PEI_SERVICES                 **PeiServices,
  IN  UINTN                                  SystemLogicalRbId,
  OUT GNB_HANDLE                             **MyHandle
  )
{
  PCIe_PLATFORM_CONFIG           *Pcie;
  GNB_PCIE_INFORMATION_DATA_HOB  *PciePlatformConfigHobData;
  GNB_HANDLE                     *GnbHandle;
  UINTN                          SocketNumber;
  UINTN                          RbNumber;
  UINTN                          RbNumberPerIoDie;
  UINTN                          SocketId;
  UINTN                          SocketRbLogicalId;
  UINTN                          RbNumberOfPerSocket;

  SocketNumber     = FabricTopologyGetNumberOfSocketPresent ();
  RbNumber         = FabricTopologyGetNumberOfSystemRootBridges ();
  RbNumberPerIoDie = FabricTopologyGetNumberOfRootBridgesOnDie (0, 0);

  RbNumberOfPerSocket = RbNumber / SocketNumber;
  SocketId = SystemLogicalRbId / RbNumberOfPerSocket;
  SocketRbLogicalId = SystemLogicalRbId % RbNumberOfPerSocket;

  PcieGetTopology (This, &PciePlatformConfigHobData);
  Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    if (GnbHandle->SocketId == SocketId) {
      if (SocketRbLogicalId == (GnbHandle->LogicalDieId * RbNumberPerIoDie + GnbHandle->RbId)) {
        IDS_HDT_CONSOLE (
          GNB_TRACE,
          "Get IohubGnbhandle: Socket %d Die PhysicalDie %d LogicalDie %d Rb %d\n",
          GnbHandle->SocketId,
          GnbHandle->PhysicalDieId,
          GnbHandle->LogicalDieId,
          GnbHandle->RbId
          );
        *MyHandle = GnbHandle;
        return EFI_SUCCESS;
      }
    }

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
EFIAPI
HygonNbioPcieHyExPeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS       Status;

  HGPI_TESTPOINT (TpNbioPCIePeiEntry, NULL);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioPcieHyExPeiEntry Entry\n");

  // Create and Initialize Topology Structure (in HOB)
  PcieConfigurationInit (PeiServices);

  // Install PPI to provide Topology to other drivers
  Status = (**PeiServices).InstallPpi (PeiServices, &mNbioPcieServicesPpiList);

  // Install PPI to provide DPC status report service
  PcieDpcStatusServiceInit (PeiServices);

  // Publish callback for all services needed to initialize HSIO
  Status = (**PeiServices).NotifyPpi (PeiServices, mHsioInitCallbackPpi);
  IDS_HDT_CONSOLE (MAIN_FLOW, "HygonNbioPcieHyExPeiEntry Exit\n");
  HGPI_TESTPOINT (TpNbioPCIePeiExit, NULL);
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

UINT8 ROMDATA StartPhyLaneSL2 [] = {
  { 112 },  // NBIO0 PCIE0 Start Lane
  { 0   },  // NBIO0 PCIE1 Start Lane
  { 16  },  // NBIO1 PCIE0 Start Lane
  { 96  },  // NBIO1 PCIE1 Start Lane
  { 32  },  // NBIO2 PCIE0 Start Lane
  { 80  },  // NBIO2 PCIE1 Start Lane
  { 64  },  // NBIO3 PCIE0 Start Lane
  { 48  }   // NBIO3 PCIE1 Start Lane
};
/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to fixup Pcie Platform Configuration for package specific values
 *
 *
 *
 * @param[in]     Pcie    PCIe configuration info
 */
HGPI_STATUS
STATIC
PcieFixupPlatformConfigSL2 (
  IN      PCIe_WRAPPER_CONFIG   *Wrapper,
  IN      VOID                  *Buffer,
  IN      PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GNB_HANDLE            *GnbHandle;
  UINT8                 Index;

  GnbHandle = (GNB_HANDLE *) PcieConfigGetParentSilicon (Wrapper);
  if (Wrapper->WrapId != 2) {
    Index = (GnbHandle->RbId << 1) + Wrapper->WrapId;
    Wrapper->StartPhyLane = StartPhyLaneSL2[Index];
    Wrapper->EndPhyLane = Wrapper->StartPhyLane + 15;
  } else {
    Wrapper->StartPhyLane = 128;
    Wrapper->EndPhyLane = 129;
  }
  return HGPI_SUCCESS;
}
/*----------------------------------------------------------------------------------------*/
/*
 *  Routine to fixup Pcie Platform Configuration for package specific values
 *
 *
 *
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
PcieFixupPlatformConfig (
  IN     PCIe_PLATFORM_CONFIG                 *Pcie
  )
{
  HGPI_STATUS  HgpiStatus;

  HgpiStatus = PcieConfigRunProcForAllWrappers (DESCRIPTOR_ALL_WRAPPERS, PcieFixupPlatformConfigSL2, NULL, Pcie);
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
  EFI_STATUS                             Status;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI *FabricTopologyServicesPpi;
  PCIe_PLATFORM_CONFIG                   *Pcie;
  PCIe_SILICON_CONFIG                    *Silicon;
  GNB_PCIE_INFORMATION_DATA_HOB          *GnbPcieInfoDataHob;
  HYGON_FABRIC_TOPOLOGY_DIE_DEVICE_MAP   *FabricIdMap;
  UINTN                                  ComplexesDataLength;
  UINTN                                  ComplexIndex;
  VOID                                   *Buffer;
  UINTN                                  NumberOfProcessors;
  UINTN                                  TotalNumberOfDie;
  UINTN                                  TotalNumberOfPhysicalDie;
  UINTN                                  TotalNumberOfLogicalDie;
  UINTN                                  TotalNumberOfRootBridges;
  UINTN                                  SystemIdOffset;
  UINTN                                  SystemFabricID;
  UINTN                                  BusNumberBase;
  UINTN                                  BusNumberLimit;
  UINTN                                  SocketIndex;
  UINTN                                  DieIndex;
  UINTN                                  RootBridgeIndex;
    
  Status = EFI_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationInit Enter\n");
  // Get number of sockets from CPU topology PPI
  // Locate CPU topology PPI
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonFabricTopologyServicesPpiGuid,
                             0,
                             NULL,
                             &FabricTopologyServicesPpi
                             );
  FabricTopologyServicesPpi->GetSystemInfo (&NumberOfProcessors, &TotalNumberOfDie, &TotalNumberOfRootBridges);

  // Allocate PCIe Complex space (maximum possibly needed)
  if (TotalNumberOfRootBridges > NumberOfProcessors) {
      //1 socket has 2+ root bridges
      ComplexesDataLength = sizeof(ST_COMPLEX_CONFIG_MASTER_NBIO) * NumberOfProcessors +
                            sizeof(ST_COMPLEX_CONFIG) * (TotalNumberOfRootBridges - NumberOfProcessors);
  } else {
      //1 socket has 1 root bridges
      ComplexesDataLength = sizeof(ST_COMPLEX_CONFIG_MASTER_NBIO) * NumberOfProcessors;
  }

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
                                                  &TotalNumberOfPhysicalDie,
                                                  &TotalNumberOfLogicalDie,
                                                  &TotalNumberOfRootBridges
                                                  );
      for (DieIndex = 0; DieIndex < TotalNumberOfLogicalDie; DieIndex++) {
        IDS_HDT_CONSOLE (GNB_TRACE, "    Process Die %d\n", DieIndex);
        FabricTopologyServicesPpi->GetDieInfo (SocketIndex,
                                              DieIndex,
                                              &TotalNumberOfRootBridges,
                                              &SystemIdOffset,
                                              &FabricIdMap
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

          IDS_HDT_CONSOLE (GNB_TRACE, "        Bus Number 0x%x - 0x%x\n", BusNumberBase, BusNumberLimit);
          //Attach Complex to Silicon which will be created by PcieFmBuildComplexConfiguration
          PcieConfigAttachChild (&Pcie->ComplexList[ComplexIndex].Header, &((PCIe_SILICON_CONFIG *) Buffer)->Header);
          //Attach Complex to Pcie
          PcieConfigAttachParent (&Pcie->Header, &Pcie->ComplexList[ComplexIndex].Header);
          PcieConfigSetDescriptorFlags (&Pcie->ComplexList[ComplexIndex], DESCRIPTOR_COMPLEX | DESCRIPTOR_TERMINATE_LIST | DESCRIPTOR_TERMINATE_GNB | DESCRIPTOR_TERMINATE_TOPOLOGY);
          //Add copy from PcieComplex to Buffer using memcopy
          if (RootBridgeIndex == 0) {
            CopyMem(Buffer, (VOID *) &ComplexDataMasterST, sizeof (ST_COMPLEX_CONFIG_MASTER_NBIO));
          } else {
            CopyMem(Buffer, (VOID *) &ComplexDataST, sizeof (ST_COMPLEX_CONFIG));
          }
          Silicon = PcieConfigGetChildSilicon (&Pcie->ComplexList[ComplexIndex]);
          Silicon->SocketId = (UINT8) SocketIndex;
          Silicon->GlobalIodId = (UINT8) SocketIndex;
          Silicon->PhysicalDieId = (UINT8) DieIndex;
          Silicon->LogicalDieId = (UINT8) DieIndex;
          Silicon->DieType    = (UINT8)IOD_SATORI;
          Silicon->RbId = (UINT8) RootBridgeIndex;
          Silicon->InstanceId = (UINT8) ComplexIndex;
          Silicon->Address.Address.Bus = BusNumberBase;
          Silicon->BusLimit = (UINT8)BusNumberLimit;
          Silicon->IohubPresent = TRUE;
		  Silicon->CxlPresent = FALSE;
          
          IDS_HDT_CONSOLE (GNB_TRACE, "Silicon->Address = 0x%x\n", Silicon->Address.AddressValue);                                        
                      
          Silicon->SocketStartLane = 0;
          Silicon->SocketEndLane = 129;
          Silicon->IodStartLane = 0;
          Silicon->IodEndLane = 129;
          
          IDS_HDT_CONSOLE (GNB_TRACE, "Silicon->SocketStartLane = %d \n", Silicon->SocketStartLane);
          IDS_HDT_CONSOLE (GNB_TRACE, "Silicon->SocketEndLane = %d \n", Silicon->SocketEndLane);
          
          while (Silicon != NULL) {
            PcieConfigAttachParent (&Pcie->ComplexList[ComplexIndex].Header, &Silicon->Header);
            Silicon = (PCIe_SILICON_CONFIG *) PcieConfigGetNextTopologyDescriptor (Silicon, DESCRIPTOR_TERMINATE_TOPOLOGY);
          }

          if (ComplexIndex > 0) {
            PcieConfigAttachComplexes (&Pcie->ComplexList[ComplexIndex - 1], &Pcie->ComplexList[ComplexIndex]);
          }

          if (RootBridgeIndex == 0) {
            Buffer = (VOID *) ((UINT8 *) Buffer + sizeof (ST_COMPLEX_CONFIG_MASTER_NBIO));
          } else {
            Buffer = (VOID *) ((UINT8 *) Buffer + sizeof (ST_COMPLEX_CONFIG));
          }
          ComplexIndex++;
          Pcie->Header.Parent = (UINT16)((UINT32)Buffer - (UINT32)&Pcie->Header);
          IDS_HDT_CONSOLE (GNB_TRACE, "Pcie->Header.Parent = 0x%x\n", Pcie->Header.Parent);
        }
      }
    }
  }

  PcieFixupPlatformConfig (Pcie);
  PcieConfigDebugDump (Pcie);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigurationInit Exit [0x%x]\n", Status);
  return Status;
}
