/** @file
  Definitions of functions for Driver Binding Protocol and Block I/O Protocol,
  and other internal definitions.

Copyright (c) 2007 - 2022, Byosoft Corporation. All rights reserved.<BR>

This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/

#ifndef _EFI_USBMASS_IMPL_H_
#define _EFI_USBMASS_IMPL_H_

typedef struct _USB_MASS_DEVICE USB_MASS_DEVICE;

#include "UsbMass.h"
#include "UsbMassBot.h"
#include "UsbMassCbi.h"
#include "UsbMassBoot.h"
#include "../UsbBus/UsbBus.h"
#include <IndustryStandard/Scsi.h>
#include <Protocol/PlatformUsbDeviceSupport.h>
#include <Protocol/BlockIo.h>

typedef struct _USB_INTERFACE     USB_INTERFACE;

enum {
    //
    // MassStorage raises TPL to TPL_NOTIFY to serialize all its operations
    // to protect shared data structures.
    //
    USB_MASS_TPL          = TPL_NOTIFY,

    USB_MASS_SIGNATURE    = SIGNATURE_32 ('U', 's', 'b', 'M'),
};

struct _USB_MASS_DEVICE {
    UINT32                    Signature;
    LIST_ENTRY                Link;
    EFI_USB_IO_PROTOCOL       *UsbIo;
    EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
    EFI_DEVICE_PATH_PROTOCOL  *UsbIoDevicePath;
    EFI_BLOCK_IO_PROTOCOL     BlockIo;
    EFI_BLOCK_IO_MEDIA        BlockIoMedia;
    BOOLEAN                   OpticalStorage;
    UINT8                     Lun;          // Logical Unit Number
    UINT8                     Pdt;          // Peripheral Device Type
    USB_MASS_TRANSPORT        *Transport;   // USB mass storage transport protocol
    UINT8                     VendorID[8];
    UINT8                     ProductID[16];
    VOID                      *Context;     // Opaque storage for mass transport
    BOOLEAN                   Cdb16Byte;
};

#define USB_MASS_DEVICE_FROM_BLOCKIO(a) \
        CR (a, USB_MASS_DEVICE, BlockIo, USB_MASS_SIGNATURE)

#define USB_MASS_DEVICE_FROM_LINK(a) \
        CR (a, USB_MASS_DEVICE, Link, USB_MASS_SIGNATURE)

EFI_STATUS
UsbConnectMassDriver (
    USB_INTERFACE        *UsbIf
)

/*++

Routine Description:

  Start the USB mass storage device on the controller. It will
  install a BLOCK_IO protocol on the device if everything is OK.

Arguments:

  This                - The USB mass storage driver binding.
  Controller          - The USB mass storage device to start on
  RemainingDevicePath - The remaining device path.

Returns:

  EFI_SUCCESS          - The driver has started on the device.
  EFI_OUT_OF_RESOURCES - Failed to allocate memory
  Others               - Failed to start the driver on the device.

--*/
;

EFI_STATUS
UsbDisconnectMassDriver(
    USB_INTERFACE        *UsbIf
)

/*++

Routine Description:

  Stop controlling the device.

Arguments:

  This              - The USB mass storage driver binding
  Controller        - The device controller controlled by the driver.
  NumberOfChildren  - The number of children of this device
  ChildHandleBuffer - The buffer of children handle.

Returns:

  EFI_SUCCESS - The driver stopped from controlling the device.
  Others      - Failed to stop the driver

--*/
;

EFI_STATUS
EFIAPI
UsbMassReset (
    IN EFI_BLOCK_IO_PROTOCOL    *This,
    IN BOOLEAN                  ExtendedVerification
);
#endif
