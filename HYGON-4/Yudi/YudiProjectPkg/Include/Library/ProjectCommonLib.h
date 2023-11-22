
#ifndef __PROJECT_COMMON_LIB_H__
#define __PROJECT_COMMON_LIB_H__

#include <Uefi.h>
#include <IndustryStandard/SmBios.h>
#include <Protocol/PciIo.h>

#define  INSPUR_VENDOR_ID   0x1BD4
#define  INTEL_VENDOR_ID    0x8086
#define  WANGXUN_VENDOR_ID  0x8088

BOOLEAN
IsOsDiskByDescription (
  IN  CHAR16    *OsDescription
  );

/**
  Check PciIo and input boot option description, if sub-vendor-id is INSPUR_VENDOR_ID(0x1BD4) and
  description has vendor-id's vendor name, then replace it with "Inspur".

  @param  PciIo           PciIo of PCI network device.
  @param  DescriptionStr  Original boot option description.

  @return The string modified by this function or original description.

**/
CHAR16 *
EFIAPI
GetInspurNetworkCardName (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN CHAR16               *DescriptionStr
);


/**
  Get startlane from pci device slot number.

  @param  SlotNumber           Slot number of PCI device.

  @return The startlane number of the input pci device slot number.

**/
UINT16
EFIAPI
GetStartLanBySlotNumber (
  IN UINT16 SlotNumber
);

/**
  Get slot number of PCIe device.

  @param  BS           gBS.
  @param  DevicePath   DevicePath of the PCIe Device.

  @return The startlane number of the input pci device slot number.

**/
UINT16
EFIAPI
InspurHygonGetPciSlotNum (
  IN  EFI_BOOT_SERVICES          *BS,
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  );

/**
  Get dimm device type detail string from smbios info.

  @param  FormFactor   FormFactor field in SMBIOS_TABLE_TYPE17.
  @param  TypeDetail   TypeDetail field in SMBIOS_TABLE_TYPE17.

  @return The device type detail string of dimm.

**/
CHAR8*
EFIAPI
SmbiosGetDimmDeviceTypeStringInFormFactor (
  UINT8                      FormFactor,
  MEMORY_DEVICE_TYPE_DETAIL  TypeDetail
  );

EFI_STATUS
EFIAPI
InspurGetPciNicMac (
  IN  EFI_PCI_IO_PROTOCOL    *PciIo,
  IN  VOID                   *Dp,
  OUT UINT8                  *MacAddr
  );

UINT8
EFIAPI
InspurGetPciPortNum (
  IN  EFI_PCI_IO_PROTOCOL          *PciIo,
  IN  EFI_DEVICE_PATH_PROTOCOL     *FilePath
  );

BOOLEAN
EFIAPI
VerifyPasswordComplexity (
  CHAR16   *Password,
  BOOLEAN  Check
  );

VOID*
EFIAPI
GetSetupData2HobData (
  VOID
  );

CHAR16 *
EFIAPI
GetPciDeviceClassInfo (
  IN EFI_PCI_IO_PROTOCOL   *PciIo
  );

#endif
