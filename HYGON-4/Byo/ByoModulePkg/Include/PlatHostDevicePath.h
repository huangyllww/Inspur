/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __PLAT_HOST_DEVICE_PATH_H__
#define __PLAT_HOST_DEVICE_PATH_H__

#include <Protocol/DevicePath.h>
#include <Library/DevicePathLib.h>



#define gPciRootBridge \
  { \
    {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
    ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 0 \
  }

#define gEndEntire \
  { \
    END_DEVICE_PATH_TYPE, END_ENTIRE_DEVICE_PATH_SUBTYPE, { END_DEVICE_PATH_LENGTH, 0} \
  }
  
#define gPciRootBridge0 \
  { \
    {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 0 \
  }

#define gPciRootBridge1 \
  { \
    {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
    ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 1 \
  }

#define gPciRootBridge2 \
  { \
    {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
    ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 2 \
  }

#define gPciRootBridge3 \
  { \
    {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
    ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 3 \
  }

#define gPciRootBridge4 \
  { \
    {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
    ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 4 \
  }

#define gPciRootBridge5 \
  { \
    {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 5 \
  }

#define gPciRootBridge6 \
  { \
    {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 6 \
  }

#define gPciRootBridge7 \
  { \
    {ACPI_DEVICE_PATH, ACPI_DP, {(UINT8) (sizeof (ACPI_HID_DEVICE_PATH)), (UINT8) \
      ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8)}}, EISA_PNP_ID (0x0A03), 7 \
  }

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           Pci0Device;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           Pci0Device;
  PCI_DEVICE_PATH           Pci1Device;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           Pci0Device;
  PCI_DEVICE_PATH           Pci1Device;
  PCI_DEVICE_PATH           Pci2Device;  
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH3;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           Pci0Device;
  PCI_DEVICE_PATH           Pci1Device;
  PCI_DEVICE_PATH           Pci2Device;
  PCI_DEVICE_PATH           Pci3Device;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH4;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           Pci0Device;
  PCI_DEVICE_PATH           Pci1Device;
  PCI_DEVICE_PATH           Pci2Device;
  PCI_DEVICE_PATH           Pci3Device;
  PCI_DEVICE_PATH           Pci4Device;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH5;

#endif

