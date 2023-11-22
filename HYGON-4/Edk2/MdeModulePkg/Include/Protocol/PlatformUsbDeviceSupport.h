/** @file
  Platform USB Device Support Protocol

Copyright (c) 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _USB_DEVICE_SUPPORT_PROTOCOL_H_
#define _USB_DEVICE_SUPPORT_PROTOCOL_H_

typedef struct _PLATFORM_USB_DEVICE_SUPPORT_PROTOCOL PLATFORM_USB_DEVICE_SUPPORT_PROTOCOL;

#define PLATFORM_USB_DEVICE_SUPPORT_PROTOCOL_GUID  {\
  0x8d8b41f3, 0xd147, 0x444d, { 0xb7, 0xcf, 0xf8, 0x8b, 0x23, 0x3e, 0x40, 0x4 } \
};
// {8D8B41F3-D147-444D-B7CF-F88B233E4004}

#define  PLATFORM_USB_DEVICE_SUPPORT_TABLE_SIGNATURE  SIGNATURE_32 ('U', 'D', 'S', 'T')
#define  PLATFORM_USB_DEVICE_SUPPORT_TABLE_REVISION   0x0002

typedef union  {
  struct {
    UINT32    Support               : 1; // 0 means unspported, 1 means supported
    UINT32    StorageReadSupport    : 1;
    UINT32    StorageWriteSupport   : 1;
    UINT32    StorageExecuteSupport : 1;
    UINT32    UsbHostControllerType : 4; // 0 means Pci,1 means MMIO
    UINT32    Reserved              : 24;
  } Bits;
  UINT32    Uint32;
} USB_SUPPORT_PROPERITY;

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;     // NULL means all port
  UINT16                      Priority;
  UINT16                      IdVendor;        // 0xFFFF means skip check
  UINT16                      IdProduct;       // 0xFFFF means skip check
  UINT16                      DeviceClass;     // 0xFFFF means skip check
  UINT8                       DeviceSubClass;  // 0xFF means skip check
  UINT8                       DeviceProtocol;  // 0xFF means skip check
  USB_SUPPORT_PROPERITY       SupportProperty;
} USB_DEVICE_SUPPORT_RULE;

typedef struct {
  UINT32    Signature;
  UINT32    Revision;
  UINT32    RuleCount;
} PLATFORM_USB_DEVICE_SUPPORT_TABLE_HEADER;

typedef struct {
  PLATFORM_USB_DEVICE_SUPPORT_TABLE_HEADER    Header;
  USB_DEVICE_SUPPORT_RULE                     Rules[1];
  //
  // Append (Header.RuleCount - 1) USB_DEVICE_SUPPORT_RULE
  //
} PLATFORM_USB_DEVICE_SUPPORT_TABLE;

extern EFI_GUID  gPlatformUsbDeviceSupportProtocolGuid;

#endif
