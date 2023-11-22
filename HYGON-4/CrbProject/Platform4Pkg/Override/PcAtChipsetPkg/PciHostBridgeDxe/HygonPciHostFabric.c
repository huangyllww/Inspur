

#include "PciHostBridge.h"
#include <Library/BaseFabricTopologyLib.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <HYGON.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <GnbRegisters.h>
#include <Library/GnbPciAccLib.h>
#include <Library/ByoHygonCommLib.h>
#include <Library/GnbPciLib.h>



VOID
FabricStSetMmioReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  IodNumberPerSocket,
  IN       UINT8  MmioPairIndex,
  IN       UINT16 DstFabricID,
  IN       UINT64 BaseAddress,
  IN       UINT64 Length
  );

VOID
FabricStSetIoReg (
  IN       UINT8  TotalSocket,
  IN       UINT8  IodNumberPerSocket,
  IN       UINT8  RegIndex,
  IN       UINT16 DstFabricID,
  IN       UINT32 IoBase,
  IN       UINT32 IoSize
  );


VOID 
LibPciHostResourceSet (
    PLATFORM_COMM_INFO  *Info
  )
{
  UINT8  SocketNumber, IodsPerSocket, RbsPerIod;
  UINT8  i, j, k;
  UINT16 DstFabricID;
  UINT8  HostIndex = 0;


  Info->IoSize[0] += PLAT_PCI_IO_BASE;
  Info->IoBase[0]  = 0;
  
  SocketNumber  = (UINT8)FabricTopologyGetNumberOfSocketPresent ();
  IodsPerSocket = (UINT8)FabricTopologyGetNumberOfLogicalDiesOnSocket (0);
  RbsPerIod     = (UINT8)FabricTopologyGetNumberOfRootBridgesOnDie (0, 0);

  for (i = 0; i < SocketNumber; i++) {
    for (j = 0; j < IodsPerSocket; j++) {
      for (k = 0; k < RbsPerIod; k++) {
        DstFabricID = (UINT16)FabricTopologyGetHostBridgeSystemFabricID (i, j, k);
        FabricStSetMmioReg (
          SocketNumber,
          IodsPerSocket,
          ((i * IodsPerSocket * RbsPerIod + j * RbsPerIod + k) * 2 + 1),
          DstFabricID,
          Info->Mmio64Base[HostIndex],
          Info->Mmio64Size[HostIndex]
          ); 
        FabricStSetMmioReg (
          SocketNumber,
          IodsPerSocket,
          ((i * IodsPerSocket * RbsPerIod + j * RbsPerIod + k) * 2),
          DstFabricID,
          Info->MmioBase[HostIndex],
          Info->MmioSize[HostIndex]
          );
          
        FabricStSetIoReg (SocketNumber, IodsPerSocket, HostIndex, DstFabricID, Info->IoBase[HostIndex], Info->IoSize[HostIndex]);
        HostIndex++;
      }
    }
  }
}



EFI_STATUS 
LibSetHygonMiscBase (
    PLATFORM_COMM_INFO  *Info
  )
{
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *PcieServicesProtocol;
  PCIe_PLATFORM_CONFIG                   *Pcie;
  EFI_STATUS                             Status;
  GNB_HANDLE                             *GnbHandle;
  GNB_PCIE_INFORMATION_DATA_HOB          *PciePlatformConfigHobData;
  UINTN                                  Index = 0;
  UINT32                                 HygonCpuModel;
  UINT32                                 BaseAddr;
  PCI_ADDR                               IommuPciAddress;  
  UINT8                                  CapabilityOffset;
  
  DEBUG((EFI_D_INFO, "LibSetHygonMiscBase\n"));
  
  Status = gBS->LocateProtocol (
                &gHygonNbioPcieServicesProtocolGuid,
                NULL,
                &PcieServicesProtocol
                );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "Locate NbioPcie:%r\n", Status));
    return Status;
  }

  HygonCpuModel = GetHygonSocModel();
  PcieServicesProtocol->PcieGetTopology(PcieServicesProtocol, (UINT32**)&PciePlatformConfigHobData);
  Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);
  GnbHandle = NbioGetHandle (Pcie);
  
  while (GnbHandle != NULL) {
    if (!GnbHandle->IohubPresent) {
      GnbHandle = GnbGetNextHandle(GnbHandle);
      continue;
    }

    ASSERT(Index < Info->PciHostCount);

    BaseAddr = Info->NbApicBase[Index] | BIT0;
    if (HygonCpuModel == HYGON_EX_CPU) {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_BASE_ADDR_LOW_REG_HYEX), &BaseAddr, 0);
    } else {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_BASE_ADDR_LOW_REG_HYGX), &BaseAddr, 0);
    }

    BaseAddr = Info->NbioMmuBase[Index];
    IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
    IommuPciAddress.Address.Function = 0x2;
    DEBUG((EFI_D_INFO, "IOMMUBase[%d] (%x,%x,%x) %x\n", \
      Index, IommuPciAddress.Address.Bus, IommuPciAddress.Address.Device, IommuPciAddress.Address.Function, BaseAddr));
    GnbLibPciWrite (IommuPciAddress.AddressValue | 0x44, AccessS3SaveWidth32, &BaseAddr, NULL);

    if(HygonCpuModel == HYGON_GX_CPU){
      CapabilityOffset = GnbLibFindPciCapability(IommuPciAddress.AddressValue, VIOMMU_CAP_ID, NULL);
      if (CapabilityOffset) {
        BaseAddr = (UINT32)Info->vIOMMUBase[Index];
        IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
        IommuPciAddress.Address.Function = 0x2;
        GnbLibPciWrite (IommuPciAddress.AddressValue | 0xD0, AccessS3SaveWidth32, &BaseAddr, NULL);
        BaseAddr = (UINT32)(Info->vIOMMUBase[Index] >> 32);
        GnbLibPciWrite (IommuPciAddress.AddressValue | 0xD4, AccessS3SaveWidth32, &BaseAddr, NULL);

        BaseAddr = (UINT32)Info->vIOMMUCtrlBase[Index];
        IommuPciAddress = NbioGetHostPciAddress (GnbHandle);
        IommuPciAddress.Address.Function = 0x2;
        GnbLibPciWrite (IommuPciAddress.AddressValue | 0xD8, AccessS3SaveWidth32, &BaseAddr, NULL);
        BaseAddr = (UINT32)(Info->vIOMMUCtrlBase[Index] >> 32);
        GnbLibPciWrite (IommuPciAddress.AddressValue | 0xDC, AccessS3SaveWidth32, &BaseAddr, NULL);        
      }
    }
    
    Index++;

    GnbHandle = GnbGetNextHandle(GnbHandle);
  }

  return EFI_SUCCESS;
}



