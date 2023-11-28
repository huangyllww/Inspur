
#include "ProjectSmm.h"
#include <PlatHostDevicePath.h>


typedef struct {
  UINT8                     UsbPort;
  EFI_DEVICE_PATH_PROTOCOL  *HostDp;
  UINTN                     HostDpSize;
  UINT8                     ParentPortNumber;
  UINT8                     *Policy;
} USB_PORT_DP_INFO;


BOOLEAN IsUsbPortNeedDisable(EFI_DEVICE_PATH_PROTOCOL *Dp)
{
  return FALSE;
}



EFI_STATUS UsbFilterInit()
{
  return EFI_SUCCESS;
}



EFI_STATUS 
ByoLegacyUsbFilter (
  IN UINTN                             DevType,
  IN EFI_DEVICE_PATH_PROTOCOL          *DevPath
  )
{
  if(gSmmReadyToBoot){
    return EFI_SUCCESS;
  }

  DEBUG((EFI_D_INFO, "%a(%d)\n", __FUNCTION__, DevType));

  if(DevType == UDT_KEYBOARD){
    return EFI_SUCCESS;
  }

  if(IsUsbPortNeedDisable(DevPath)){
    return EFI_ACCESS_DENIED;
  } else {
    return EFI_SUCCESS;
  }
}


EFI_STATUS 
ByoLegacyUsbFilter2 (
  IN EFI_DEVICE_PATH_PROTOCOL          *DevPath
  )
{
  if(IsUsbPortNeedDisable(DevPath)){
    return EFI_ACCESS_DENIED;
  } else {
    return EFI_SUCCESS;
  }
}



