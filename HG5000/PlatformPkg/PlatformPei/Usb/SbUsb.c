/*++

Module Name:
  
  SbUsb.c

Abstract:

  Ehci PPI Init

--*/
#include "UsbController.h"
#include <PlatformDefinition.h>
#include <IndustryStandard/Pci.h>


STATIC
EFI_STATUS
EnableEhciController (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN USB_CONTROLLER_INFO      *HostInfo,
  IN UINT8                    Index
  )
{
  UINTN  Bar0;

  if (Index >= HostInfo->EhciControllersNum) {
    return EFI_INVALID_PARAMETER;
  }

  Bar0 = HostInfo->EhciPciAddr[Index] + PCI_BASE_ADDRESSREG_OFFSET;

  MmioWrite32(Bar0, HostInfo->EhciMemBase[Index]);
  MmioOr16(HostInfo->EhciPciAddr[Index] + PCI_COMMAND_OFFSET, BIT2 | BIT1);

  DEBUG((EFI_D_INFO, "EHCI[%d] BAR0(%08X) = %08X\n", Index, Bar0, MmioRead32(Bar0)));

  return EFI_SUCCESS;
}


STATIC
EFI_STATUS
EnableXhciController (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN USB_CONTROLLER_INFO      *HostInfo,
  IN UINT8                    Index
  )
{
  UINTN Bar0;
  
  if (Index >= HostInfo->XhciControllersNum) {
    return EFI_INVALID_PARAMETER;
  }

  Bar0 = HostInfo->XhciPciAddr[Index] + PCI_BASE_ADDRESSREG_OFFSET;

  MmioWrite32(Bar0, HostInfo->XhciMemBase[Index]);
  MmioOr16(HostInfo->XhciPciAddr[Index] + PCI_COMMAND_OFFSET, BIT2 | BIT1);

  DEBUG((EFI_D_INFO, "XHCI[%d] BAR0(%08X) = %08X\n", Index, Bar0, MmioRead32(Bar0)));

  return EFI_SUCCESS;
}



EFI_STATUS
InitUsbControl (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  USB_CONTROLLER_INFO           *PeiUsbControllerInfo
  )
{
  EFI_STATUS            Status;
  UINTN                 Index;


  DEBUG ((EFI_D_INFO, "InitUsbControl()\n"));
  
  Status = EFI_SUCCESS;

  for(Index = 0; Index < PeiUsbControllerInfo->EhciControllersNum; Index ++){
    Status = EnableEhciController(PeiServices, PeiUsbControllerInfo, (UINT8)Index);
    ASSERT_EFI_ERROR (Status);
  }
  for(Index = 0; Index < PeiUsbControllerInfo->XhciControllersNum; Index ++){
    Status = EnableXhciController(PeiServices, PeiUsbControllerInfo, (UINT8)Index);
    ASSERT_EFI_ERROR (Status);
  }

  DEBUG ((EFI_D_INFO, "InitUsbControl() End\n"));

  return Status;

}


