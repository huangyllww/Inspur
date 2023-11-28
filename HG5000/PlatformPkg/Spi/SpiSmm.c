

#include "SpiCommon.h"
#include <Library/SmmServicesTableLib.h>

EFI_STATUS
EFIAPI
InitializeSpiSmm (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS     Status;

/*  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gSpiInstance.Handle,
                  &gEfiSmmSpiProtocolGuid,
                  &gSpiInstance.SpiProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);
*/

  Status = gSmst->SmmInstallProtocolInterface (
                    &gSpiInstance.Handle,
                    &gEfiSmmSpiProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gSpiInstance.SpiProtocol
                    );
  ASSERT_EFI_ERROR (Status);

  return Status;
}



