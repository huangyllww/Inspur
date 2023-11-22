

#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>


STATIC EFI_GUID gDummyGuid = { 0xb3757eea, 0xcedf, 0x4e6f, { 0x8e, 0x4, 0xdc, 0x6d, 0x9f, 0xa7, 0x11, 0xcb } };

STATIC EFI_PEI_PPI_DESCRIPTOR  gPlatTpmDpxPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gDummyGuid,
  NULL
};


EFI_STATUS
EFIAPI
PlatPeiTpmDeviceSelectedCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_GUID    *TpmInstanceGuid = (EFI_GUID*)PcdGetPtr(PcdTpmInstanceGuid);
  EFI_GUID    *DpxGuid         = NULL;
  EFI_STATUS  Status;
  UINTN       Size;


  if(CompareGuid(TpmInstanceGuid, &gEfiTpmDeviceInstanceTpm20DtpmGuid) ||
     CompareGuid(TpmInstanceGuid, &gEfiTpmDeviceInstanceTpm20HgfTpmGuid)){
    DpxGuid = &gByoTpm20ModuleDpxGuid;
  } else if(CompareGuid(TpmInstanceGuid, &gEfiTpmDeviceInstanceTpm12Guid)){
    DpxGuid = &gByoTpm12ModuleDpxGuid;
  }else if(CompareGuid(TpmInstanceGuid, &gEfiTcmDeviceInstanceGuid)){
    DpxGuid = &gByoTcmModuleDpxGuid;
  } else {
    DpxGuid = &gDummyGuid;
  }

  gPlatTpmDpxPpiList.Guid = DpxGuid;
  Status = PeiServicesInstallPpi(&gPlatTpmDpxPpiList);
  ASSERT_EFI_ERROR(Status);

  Size   = sizeof(EFI_GUID);
  Status = PcdSetPtrS (PcdTpmTcmModuleDpxGuid, &Size, DpxGuid);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


