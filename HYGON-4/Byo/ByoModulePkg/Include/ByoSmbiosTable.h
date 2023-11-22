/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_SMBIOS_TABLE_H__
#define __BYO_SMBIOS_TABLE_H__

#define SMBIOS_BYOTOOL_TYPE SMBIOS_OEM_BEGIN + 0x60 // 0x80 + 0x60 = 0xE0 (224)
#define SMBIOS_IFRDATA_TYPE SMBIOS_OEM_BEGIN + 0x61 // 0x80 + 0x61 = 0xE1 (225)
#define SMBIOS_DEFAULT_VARIABLE_HOB_DATA_TYPE SMBIOS_OEM_BEGIN + 0x62 // 0x80 + 0x62 = 0xE2 (226)

#define RESERVED_BYOTOOL_MEMORY_SIZE 0x2000
#define SMBIOS_BYOTOOL_TABLE_SIGNATURE SIGNATURE_32 ('S', 'B', 'T', 'T')
#define SMBIOS_IFRDATA_TABLE_SIGNATURE SIGNATURE_32 ('I', 'F', 'R', 'T')
#define SMBIOS_DEFAULT_VARIABLE_HOB_DATA_TABLE_SIGNATURE SIGNATURE_32 ('D', 'V', 'H', 'T')

//
// Only when SMBIOS_BYOTOOL_TABLE is updated, this version will be updated.
//
#define SMBIOS_BYOTOOL_TABLE_VERISON 0x01
//
// only version update when the interface between bios and tool is changed
//
#define BYOFLASH_VERSION  0x01
#define BYOCFG_VERSION    0x01
#define BYODMI_VERSION    0x01

#define BYO_VENDOR_TYPE       0x00
#define LENOVO_VENDOR_TYPE    0x01
#define HISILICON_VENDOR_TYPE 0x02
#define INSPUR_VENDOR_TYPE    0x03

#define USB_CONTROLLER_TYPE_PCI    0
#define USB_CONTROLLER_TYPE_MMIO   1

#pragma pack(1)

typedef struct {
  UINT8 TableVersion:4;
  UINT8 VendorType:4;
  UINT8 ByoFlashVersion;
  UINT8 ByoDmiVersion;
  UINT8 ByoCfgVersion;
} BYOTOOL_VERSION;

typedef struct {
  SMBIOS_STRUCTURE    Hdr;
  UINT32 Signature;
  BYOTOOL_VERSION Version;
  UINT32 SwSmiPort;
  UINT32 MemoryAddress;
  UINT32 MemorySize;
  UINT8  ZeroEnd[2];
} SMBIOS_BYOTOOL_TABLE;

typedef struct {
  SMBIOS_STRUCTURE    Hdr;
  UINT32 Signature;
  UINT64 IfrAddress;
  UINT64 IfrDataLength;
} SMBIOS_IFRDATA_TABLE;

typedef struct {
  SMBIOS_STRUCTURE    Hdr;
  UINT32 Signature;
  UINT64 HobDataAddress;
  UINT64 HobDataLength;
} SMBIOS_DEFAULT_VARIABLE_HOB_TABLE;

#define SMBIOS_USB_SUPPORT_DEVICE_TYPE               (SMBIOS_OEM_BEGIN + 0x7B) // 0x80 + 0x60 = 0xFB (251)
#define SMBIOS_USB_SUPPORT_DEVICE_TABLE_SIGNATURE    SIGNATURE_32 ('U', 'D', 'S', 'R')
#define SMBIOS_USB_SUPPORT_DEVICE_TABLE_VERISON_0001 0x0001
#define SMBIOS_USB_SUPPORT_DEVICE_TABLE_VERISON_0002 0x0002

typedef struct {
  UINT32  Support:1;                // 0 means unsupported, 1 means supported
  UINT32  StorageReadSupport:1;   
  UINT32  StorageWriteSupport:1;
  UINT32  StorageExecuteSupport:1;
  UINT32  UsbHostControllerType:4;
  UINT32  Reserved:24;
} USB_DEVICE_SUPPORT_PROPERTY;

typedef union  {
  struct {
    UINT32  Seg;
    UINT16  Bus;   
    UINT8   Dev;
    UINT8   Func;
  } PciController;
  UINT64  MmioBaseAddress;
} USB_DEVICE_CONTROL_INFO;

///
/// OEM USB Device Support (Type 251).
///
typedef struct {
  SMBIOS_STRUCTURE             Hdr;
  UINT32                       Signature;
  UINT16                       Revision;
  UINT16                       Priority;
  USB_DEVICE_CONTROL_INFO      ControllerInfo;
  UINT8                        PortAddressStr[16];
  UINT16                       IdVendor;
  UINT16                       IdProduct;
  UINT16                       DeviceClass;
  UINT8                        DeviceSubClass;
  UINT8                        DeviceProtocol;
  USB_DEVICE_SUPPORT_PROPERTY  SupportProperty;
  UINT8                        ZeroEnd[2];
} SMBIOS_USB_DEVICE_SUPPORT_TABLE_TYPE251;
#pragma pack()

#endif
