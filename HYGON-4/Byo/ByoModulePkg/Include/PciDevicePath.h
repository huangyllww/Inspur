/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __PCI_DEVICE_PATH_H__
#define __PCI_DEVICE_PATH_H__

#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>



#define DP_ROOT(root) \
  { \
    {\
        ACPI_DEVICE_PATH, ACPI_DP, { (UINT8) (sizeof(ACPI_HID_DEVICE_PATH)), (UINT8) \
        ((sizeof(ACPI_HID_DEVICE_PATH)) >> 8) }\
    }, \
    EISA_PNP_ID (0x0A03), root \
  }

#define DP_PCI(device, function) \
  { \
     {\
        HARDWARE_DEVICE_PATH, HW_PCI_DP, { (UINT8) (sizeof(PCI_DEVICE_PATH)), (UINT8) \
        ((sizeof(PCI_DEVICE_PATH)) >> 8) }\
     },\
     function, device \
  }

#define DP_END \
  { \
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, { END_DEVICE_PATH_LENGTH, 0 } \
  }

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridgeDevicePath;
  PCI_DEVICE_PATH           PciBridge1Path;
  EFI_DEVICE_PATH           EndDevicePath;
} BOARDCARD_PATH1;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridgeDevicePath;
  PCI_DEVICE_PATH           PciBridge1Path;
  PCI_DEVICE_PATH           PciBridge2Path;
  EFI_DEVICE_PATH           EndDevicePath;
} BOARDCARD_PATH2;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridgeDevicePath;
  PCI_DEVICE_PATH           PciBridge1Path;
  PCI_DEVICE_PATH           PciBridge2Path;
  PCI_DEVICE_PATH           PciBridge3Path;
  EFI_DEVICE_PATH           EndDevicePath;
} BOARDCARD_PATH3;


#endif

