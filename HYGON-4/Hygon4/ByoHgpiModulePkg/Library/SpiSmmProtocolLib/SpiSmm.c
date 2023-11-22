

#include "SpiCommon.h"
#include <Library/SmmServicesTableLib.h>


EFI_STATUS
EFIAPI
SpiSmmProtocolLibInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS     Status;
  VOID           *Interface;

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSpiProtocolGuid,
                    NULL,
                    (VOID**)&Interface
                    );
  if(!EFI_ERROR(Status)){
    return EFI_SUCCESS;
  }

  Status = gSmst->SmmInstallProtocolInterface (
                    &gSpiInstance.Handle,
                    &gEfiSmmSpiProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gSpiInstance.SpiProtocol
                    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}



