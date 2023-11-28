

#include "SpiCommon.h"
#include <Guid/EventGroup.h>


EFI_STATUS
EFIAPI
InitializeSpiDxe (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   VirtualAddressChangeEvent;  

 
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gSpiInstance.Handle,
                  &gEfiSpiProtocolGuid,
                  &gSpiInstance.SpiProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  SpiVirtualddressChangeEvent,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &VirtualAddressChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);  
  
  return Status;
}



