
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#ifndef _SIO_H_
#define _SIO_H_

#define PLATFORM_ID_SHIFT 50
#define PLATFORM_ID_MASK  7   // Bits 52:50

#include <NctReg.h>


#pragma pack(1)

typedef struct {
  EFI_ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR  Io;
  EFI_ACPI_IRQ_NOFLAG_DESCRIPTOR              Irq;
  EFI_ACPI_END_TAG_DESCRIPTOR                 End;
} ACPI_SIO_RESOURCES_IO_IRQ;

typedef struct {
  EFI_ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR  Io;
  EFI_ACPI_IRQ_NOFLAG_DESCRIPTOR              Irq;
  EFI_ACPI_DMA_DESCRIPTOR                     Dma;
  EFI_ACPI_END_TAG_DESCRIPTOR                 End;
} ACPI_SIO_RESOURCES_IO_IRQ_DMA;

#pragma pack()

typedef struct {
  UINT32                      HID;
  UINT32                      UID;
} EFI_SIO_ACPI_DEVICE_ID;

typedef struct {
  EFI_SIO_ACPI_DEVICE_ID      Device;
  UINT8                       DeviceId;
  UINT8                       ResourceMask;
  ACPI_RESOURCE_HEADER_PTR    Resources;
  ACPI_RESOURCE_HEADER_PTR    PossibleResources;
} DEVICE_INFO;


/**
  Initialize the SIO chip for S3.
**/
VOID
SioInitForS3 (
  VOID
  );

/**
  Return the supported devices.

  @param[out] Devices         Pointer to pointer of EFI_SIO_ACPI_DEVICE_ID.
                              Caller is responsible to free the buffer.
  @param[out] Count           Pointer to UINTN holding the device count.
**/
VOID
DeviceGetList (
  OUT EFI_SIO_ACPI_DEVICE_ID **Devices,
  OUT UINTN                  *Count
  );

/**
  Program the SIO chip to enable the specified device using the default resource.

  @param[in] Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
**/
VOID
DeviceEnable (
  IN EFI_SIO_ACPI_DEVICE_ID   *Device
  );

/**
  Get the possible ACPI resources for specified device.

  @param[in]  Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[out] Resources       Pointer to ACPI_RESOURCE_HEADER_PTR.

  @retval EFI_SUCCESS         The resources are returned successfully.
**/
EFI_STATUS
DevicePossibleResources (
  IN  EFI_SIO_ACPI_DEVICE_ID   *Device,
  OUT ACPI_RESOURCE_HEADER_PTR *Resources
  );

/**
  Set the ACPI resources for specified device.

  The SIO chip is programmed to use the new resources and the
  resources setting are saved. The function assumes the resources
  are valid.

  @param[in] Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[in] Resources       ACPI_RESOURCE_HEADER_PTR.

  @retval EFI_SUCCESS        The resources are set successfully.
**/
EFI_STATUS
DeviceSetResources (
  IN EFI_SIO_ACPI_DEVICE_ID   *Device,
  IN ACPI_RESOURCE_HEADER_PTR Resources
  );

/**
  Get the ACPI resources for specified device.

  @param[in]  Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[out] Resources       Pointer to ACPI_RESOURCE_HEADER_PTR.

  @retval EFI_SUCCESS         The resources are returned successfully.
**/
EFI_STATUS
DeviceGetResources (
  IN  EFI_SIO_ACPI_DEVICE_ID   *Device,
  OUT ACPI_RESOURCE_HEADER_PTR *Resources
  );

VOID
WriteHWRegister (
  IN  UINT8            Index,
  IN  UINT8            Data
  );

UINT8
ReadHWRegister (
  IN  UINT8            Index
  );

VOID
AndOrHWRegister (
  IN  UINT8            Index,
  IN  UINT8            AndData,
  IN  UINT8            OrData
  );


//
// Prototypes for the sio internal function
//
//
// Internal function
//
VOID
SioInitForS3 (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  GC_TODO: add return values

--*/
;

#endif
