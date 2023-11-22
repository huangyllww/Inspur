

#include <Uefi.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/DebugLib.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/HobLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioSmuLib.h>
#include <SocId.h>
#include <GnbRegisters.h>
#include <Library/HygonSmnAddressLib.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <Protocol/FabricTopologyServices.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <SmuBiosIf/Bios2SmuSmc.h>
#include <Library/PcieConfigLib.h>
#include <Library/ByoHygonCommLib.h>
#include <Library/ByoHygonFabricLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>




EFI_STATUS LibGetPxm(EFI_BOOT_SERVICES *BS, UINT32 *Pxm, UINTN *PxmCount)
{
  EFI_STATUS                             Status;
  HYGON_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;  
  UINTN                                  SocketLoop;
  UINTN                                  NumberOfDies;
  UINTN                                  NumberOfSockets;  
  FABRIC_NUMA_SERVICES_PROTOCOL          *FabricNuma;
  UINT32                                 Domain;
  UINTN                                  i = 0;
  UINTN                                  NumberOfCdds;
  UINTN                                  CddsPresent;
  UINTN                                  CddLoop;
  UINTN                                  MaxCount = *PxmCount;
  

  Status = BS->LocateProtocol(&gHygonFabricNumaServicesProtocolGuid, NULL, (VOID**)&FabricNuma);
  if(EFI_ERROR(Status)){
    return Status;
  }

  Status = BS->LocateProtocol(&gHygonFabricTopologyServicesProtocolGuid, NULL, (VOID**)&FabricTopology);
  if(EFI_ERROR(Status)){
    return Status;
  }

  Status = FabricTopology->GetSystemInfo(FabricTopology, &NumberOfSockets, NULL, NULL);
  if(EFI_ERROR(Status)){
    return Status;
  }

  for (SocketLoop = 0; SocketLoop < NumberOfSockets; SocketLoop++) {
    Status = FabricTopology->GetProcessorInfo(FabricTopology, SocketLoop, &NumberOfDies, NULL, NULL);
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }

    Status = FabricTopology->GetCddInfo(FabricTopology, SocketLoop, &NumberOfCdds, &CddsPresent);
    if(EFI_ERROR(Status)){
      goto ProcExit;
    }

    for (CddLoop = 0; CddLoop < MAX_CDDS_PER_SOCKET; CddLoop++) {
      if (!IS_CDD_PRESENT (CddLoop, CddsPresent)) {
        continue;
      }
      Status = FabricNuma->DomainXlat(FabricNuma, SocketLoop, CddLoop, &Domain);
      if(EFI_ERROR(Status)){
        goto ProcExit;
      }
      if(i >= MaxCount){
        Status = EFI_BUFFER_TOO_SMALL;
        goto ProcExit;
      }

      Pxm[i++] = Domain;

    }
  }

  *PxmCount = i;

ProcExit:
  return Status;
}



UINTN
LibGetCpuTdpValue (
    EFI_BOOT_SERVICES *BS
  )
{
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *NbioPcieSvr;
  GNB_PCIE_INFORMATION_DATA_HOB          *PcieInfoDataHob;
  EFI_STATUS                             Status;
  GNB_HANDLE                             *GnbHandle;
  UINT32                                 SmuArg[6];
  

  Status = BS->LocateProtocol(&gHygonNbioPcieServicesProtocolGuid, NULL, (VOID*)&NbioPcieSvr);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Get NbioPcieSvr:%r\n", Status));
    return 0;
  }
  
  Status = NbioPcieSvr->PcieGetTopology(NbioPcieSvr, (UINT32**)&PcieInfoDataHob);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "PcieGetTopology:%r\n", Status));    
    return 0;
  }
  
  GnbHandle = NbioGetHandle(&PcieInfoDataHob->PciePlatformConfigHob);
  NbioSmuServiceCommonInitArguments(SmuArg);
  Status = NbioSmuServiceRequest(NbioGetHostPciAddress(GnbHandle), 0, SMC_MSG_TdpTjmax, SmuArg, 0);
  DEBUG((EFI_D_INFO, "SmuArg[0]:%x\n", SmuArg[0]));
  return (SmuArg[0] >> 16);
}


STATIC
UINT32
LibRbEnumDs (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  OUT    UINT8                      *DsBus
)
{
  UINT32                            OldValue, NewValue;
  UINT8                             PrimaryBus, SecondaryBus, SubBusNum;
  UINT32                            SmnAddress;
  
  SmnAddress = ConvertPciePortAddress2 (
                 SMN_PCIE0_SUB_BUS_NUMBER_LATENCY_HYGX,
                 GnbHandle,
                 Engine
                 );
  
  //save old sub bus config
  NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &OldValue, 0);
  //allocate bus for DS
  PrimaryBus   = (UINT8)GnbHandle->Address.Address.Bus;
  SecondaryBus = (UINT8)(GnbHandle->Address.Address.Bus + 1);
  *DsBus       = SecondaryBus;
  SubBusNum    = (UINT8)(GnbHandle->Address.Address.Bus + 1);
  NewValue = 0;
  NewValue |= PrimaryBus;
  NewValue |= (SecondaryBus << 8);
  NewValue |= (SubBusNum << 16);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &NewValue, 0);

  return OldValue;
}


STATIC
VOID
LibRbUnEnumDs (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_ENGINE_CONFIG         *Engine,
  IN     UINT32                     OldRegValue
  )
{
  UINT32                            SmnAddress;
  
  SmnAddress = ConvertPciePortAddress2 (
                 SMN_PCIE0_SUB_BUS_NUMBER_LATENCY_HYGX,
                 GnbHandle,
                 Engine
                 );
  NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &OldRegValue, 0);
}


VOID
LibGetPcieSubDeviceInfo (
  IN     VOID               *pGnbHandle,
  IN     VOID               *pEngine,
  OUT    BYO_PEI_PCIE_INFO  *PcieInfo
  )
{
  GNB_HANDLE           *GnbHandle = (GNB_HANDLE*)pGnbHandle;
  PCIe_ENGINE_CONFIG   *Engine    = (PCIe_ENGINE_CONFIG*)pEngine;
  UINT32               OldValue;
  UINT8                Bus;
  UINTN                PciMmBase;
  UINT32               Data32;
  

  OldValue = LibRbEnumDs(GnbHandle, Engine, &Bus);
  PciMmBase = PCI_DEV_MMBASE(Bus, 0, 0);

  PcieInfo->PortBus = (UINT8)GnbHandle->Address.Address.Bus;
  PcieInfo->PortDev = Engine->Type.Port.PortData.DeviceNumber;
  PcieInfo->PortFun = Engine->Type.Port.PortData.FunctionNumber; 

  PcieInfo->PciId = MmioRead32(PciMmBase);
  Data32 = MmioRead32(PciMmBase + 8);
  CopyMem(PcieInfo->ClassCode, ((UINT8*)&Data32)+1, 3);

  LibRbUnEnumDs(GnbHandle, Engine, OldValue);  
}




VOID LibGetSysPci64Range(UINT64 *Base, UINT64 *Size)
{
  UINT64   TOM2;
  UINT64   MmioBaseAddr;
  UINT64   MmioLimitAbove4G;
  

  TOM2 = AsmReadMsr64 (0xC001001D);

  MmioBaseAddr = (TOM2 > 0x100000000) ? TOM2 : 0x100000000; // Check if TOM2 > 4G

  if (!PcdGetBool (PcdHygonHgpiSmee)) {    //SMEE disable
    MmioLimitAbove4G = LShiftU64(1, PcdGet8 (PcdHygonAbove4GMmioLimitBit));
  } else {                                
    if (PcdGet8 (PcdHygonAbove4GMmioLimitBit) > 45) {  //SMEE enable MMIO address is 45bit or below 45bit
      MmioLimitAbove4G = (UINT64) 1 << 45; 
    } else {
      MmioLimitAbove4G = (UINT64) 1 << PcdGet8 (PcdHygonAbove4GMmioLimitBit);
    }
  }
  MmioLimitAbove4G -= 0x300000000; //MMIO limit - 12G

  if (PcdGet64 (PcdHygonMmioAbove4GLimit) < MmioLimitAbove4G) {
    MmioLimitAbove4G = (PcdGet64 (PcdHygonMmioAbove4GLimit) + 1) & 0xFFFFFFFFFFFF0000;
  }

  *Base = MmioBaseAddr;
  *Size = MmioLimitAbove4G - MmioBaseAddr;
  
}




