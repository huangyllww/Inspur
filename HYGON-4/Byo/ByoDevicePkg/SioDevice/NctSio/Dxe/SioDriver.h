
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#ifndef _SIO_DRIVER_H_
#define _SIO_DRIVER_H_

#include <PiDxe.h>
#include <IndustryStandard/Pci.h>
#include <Library/BaseLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/PciLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/S3IoLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/SuperIoLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/DriverBinding.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SuperIo.h>
#include "SioChip.h"
#include "SioHwm.h"
#include "SioService.h"
#include <ByoStatusCode.h>
#include <Guid/SioSetupDataStruct.h>


//
// SIO device private data structure
//
#define SIO_DEV_SIGNATURE SIGNATURE_32 ('_', 'S', 'I', 'O')

typedef struct _SIO_DEV {
  UINT32                   Signature;
  EFI_HANDLE               PciHandle;
  EFI_SIO_ACPI_DEVICE_ID   Device;
  EFI_HANDLE               Handle;
  EFI_SIO_PROTOCOL         Sio;
  EFI_DEVICE_PATH_PROTOCOL *DevicePath;
  LIST_ENTRY               Link;
} SIO_DEV;

#define SIO_DEV_FROM_THIS(a) CR (a, SIO_DEV, Sio, SIO_DEV_SIGNATURE)


//
// Prototypes for Driver model protocol interface
//

/**
  Test to see if this driver supports Controller Handle. 

  @param  This                Protocol instance pointer.
  @param  Controller          Handle of device to test
  @param  RemainingDevicePath Optional parameter use to pick a specific child
                              device to start.

  @retval EFI_SUCCESS         This driver supports this device
  @retval EFI_ALREADY_STARTED This driver is already running on this device
  @retval other               This driver does not support this device

**/
EFI_STATUS
EFIAPI
SioDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );


/**
  Start this driver on ControllerHandle.

  @param  This                 Protocol instance pointer.
  @param  Controller           Handle of device to bind driver to
  @param  RemainingDevicePath  Optional parameter use to pick a specific child
                               device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle
  @retval other                This driver does not support this device

**/
EFI_STATUS
EFIAPI
SioDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  );


/**
  Stop this driver on ControllerHandle.

  @param  This              Protocol instance pointer.
  @param  Controller        Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
SioDriverStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN  EFI_HANDLE                     Controller,
  IN  UINTN                          NumberOfChildren,
  IN  EFI_HANDLE                     *ChildHandleBuffer
  );

VOID
InitializeLpcSio ();

VOID
InitializeSioSerialPort1 ();

VOID
WriteRegisterAndSaveToScript (
  IN  UINT8   Index,
  IN  UINT8   Data
  );


#endif
