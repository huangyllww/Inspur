
#include "PlatformDxe.h"
#include <Library/PerformanceLib.h>
#include <Protocol/UsbPolicy.h>
#include <Protocol/Ps2Policy.h>
#include <Protocol/PciRootBridgeIo.h>
#include <AcpiRam.h>



STATIC EFI_USB_POLICY_PROTOCOL gUsbPolicy;

//EFI_PS2_POLICY_PROTOCOL mPs2PolicyData = {
//  EFI_KEYBOARD_NUMLOCK,
//  (EFI_PS2_INIT_HARDWARE) Ps2InitHardware
//};

EFI_STATUS MiscConfigDxe (VOID)
{
  EFI_STATUS  Status;
  
  ZeroMem(&gUsbPolicy, sizeof(gUsbPolicy));
  gUsbPolicy.Version                     = (UINT8)USB_POLICY_PROTOCOL_REVISION_2;
  gUsbPolicy.UsbMassStorageEmulationType = ATUO_TYPE;     // It seems spell error, but ...
  gUsbPolicy.UsbOperationMode            = HIGH_SPEED;
  gUsbPolicy.LegacyKBEnable              = LEGACY_KB_EN;
  gUsbPolicy.LegacyMSEnable              = LEGACY_MS_EN;//LEGACY_MS_DIS;
  gUsbPolicy.UsbMassSupport              = gSetupData->UsbMassSupport;  
  gUsbPolicy.USBPeriodSupport            = LEGACY_PERIOD_UN_SUPP;
  gUsbPolicy.LegacyFreeSupport           = LEGACY_FREE_SUPP;
//gUsbPolicy.LegacyUsbEnable             = gSetupData->LegacyUsbEnable?LEGACY_USB_EN:LEGACY_USB_DIS;
  gUsbPolicy.LegacyUsbEnable             = LEGACY_USB_EN;
  gUsbPolicy.XhcSupport                  = LEGACY_XHC_SUPP;
  gUsbPolicy.UsbTimeTue                  = LEGACY_USB_TIME_TUE_ENABLE;
  gUsbPolicy.InternelHubExist            = USB_NOT_HAVE_HUB_INTERNEL;
  gUsbPolicy.EnumWaitPortStableStall     = 100;
  gUsbPolicy.CodeBase                    = ICBD_CODE_BASE;
  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gImageHandle,
                  &gUsbPolicyGuid, &gUsbPolicy,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);  


//  mPs2PolicyData.KeyboardLight = 0;
//  if (gSetupData->Numlock)
//    mPs2PolicyData.KeyboardLight |= EFI_KEYBOARD_NUMLOCK;
//  Status = gBS->InstallProtocolInterface (
//                  &gImageHandle,
//                  &gEfiPs2PolicyProtocolGuid,
//                  EFI_NATIVE_INTERFACE,
//                  &mPs2PolicyData
//                  );
//  ASSERT_EFI_ERROR (Status);

  return Status;  
}



