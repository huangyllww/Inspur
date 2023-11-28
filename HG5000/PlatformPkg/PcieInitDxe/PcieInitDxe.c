

#include <Library/DebugLib.h>
#include <NbioRegisterTypes.h>
#include <AMD.h>
#include <GnbDxio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <PiDxe.h>
#include <Library/AmdBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/AmdNbioBaseServicesProtocol.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbNbioBaseZPInfoHob.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Protocol/PciIo.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <GnbRegistersZP.h>
#include <Library/NbioRegisterAccLib.h>



VOID
STATIC
PlatPcieSlotInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GNB_HANDLE            *NbioHandle;
  UINT32                Address;
  
  if (Engine->Type.Port.PortData.PortPresent){
    NbioHandle = (GNB_HANDLE*)PcieConfigGetParentSilicon (Engine);
    Address = MAKE_SBDFO(0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CAP_ADDRESS);

    NbioRegisterRMW (
       NbioHandle,
       TYPE_PCI,
       Address,
       (UINT32) ~(SLOT_CAP_PHYSICAL_SLOT_NUM_MASK),
       (UINT32) (Engine->Type.Port.PortData.SlotNum << SLOT_CAP_PHYSICAL_SLOT_NUM_OFFSET),
       0
       );
  }
}


EFI_STATUS 
PcieInitDxeEntry (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  PCIe_PLATFORM_CONFIG           *Pcie;
  EFI_PEI_HOB_POINTERS           GuidHob;


  DEBUG((EFI_D_INFO, "SetAgesaPcieSlotId\n"));

  GuidHob.Raw = GetFirstGuidHob(&gGnbPcieHobInfoGuid);
  if (GuidHob.Raw == NULL) {
    DEBUG((EFI_D_ERROR, "gGnbPcieHobInfoGuid not found\n"));
    return EFI_NOT_FOUND;
  }

  Pcie = (PCIe_PLATFORM_CONFIG*)GET_GUID_HOB_DATA(GuidHob);  
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PlatPcieSlotInitCallback,
    NULL,
    Pcie
    );
        
  return EFI_SUCCESS;  
}

