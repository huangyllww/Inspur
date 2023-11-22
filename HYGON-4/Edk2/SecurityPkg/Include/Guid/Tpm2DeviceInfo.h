/** @file
  GUID used to store TPM2 Device ManufacturerID and FirmwareVersion1 information


**/

#ifndef __TPM2_DEVICE_INFO_GUID_H__
#define __TPM2_DEVICE_INFO_GUID_H__

#define TPM2_DEVICE_INFO_GUID \
  { \
    0x7e5ade2e, 0x23cf, 0x4110, { 0xbd, 0x78, 0x35, 0xda, 0x60, 0xae, 0x1c, 0x7d } \
  }

typedef struct {
  UINT32  ManufactureId;
  UINT32  FirmwareVersion1;
  UINT32  FirmwareVersion2;
} TPM2_DEVICE_INFO;

#define TPM2_DEVICE_INFO_NAME  L"TPM2_DEVICE_INFO"

extern EFI_GUID gTpm2DeviceInfoGuid;

#endif
