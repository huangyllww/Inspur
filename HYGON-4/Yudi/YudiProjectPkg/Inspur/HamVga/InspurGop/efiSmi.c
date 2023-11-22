/*++

Copyright (c) 2004, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  efiSMI.c
    
Abstract:

  SMI Controller Driver.
  This driver is a sample implementation of the UGA Draw Protocol for the
  SMI family of PCI video controllers.  This driver is only
  usable in the EFI pre-boot environment.  This sample is intended to show
  how the UGA Draw Protocol is able to function.  The UGA I/O Protocol is not
  implemented in this sample.  A fully compliant EFI UGA driver requires both
  the UGA Draw and the UGA I/O Protocol.  Please refer to Microsoft's
  documentation on UGA for details on how to write a UGA driver that is able
  to function both in the EFI pre-boot environment and from the OS runtime.

Revision History:

--*/

//
// SMI Controller Driver
//

#include "efiSmi.h"

EFI_DRIVER_BINDING_PROTOCOL gSmiDriverBinding = {
  SmiControllerDriverSupported,
  SmiControllerDriverStart,
  SmiControllerDriverStop,
  0x10,
  NULL,
  NULL
};

//
// SMI Driver Entry point
//

EFI_DRIVER_ENTRY_POINT (SmiUgaDrawDriverEntryPoint)

EFI_STATUS
EFIAPI
SmiUgaDrawDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description:

Arguments:

Returns:

    None

--*/
// TODO:    ImageHandle - add argument and description to function comment
// TODO:    SystemTable - add argument and description to function comment
{
  return EfiLibInstallAllDriverProtocols (
          ImageHandle,
          SystemTable,
          &gSmiDriverBinding,
          ImageHandle,
          &gSmiComponentName,
          NULL,
          NULL
          );
}

EFI_STATUS
EFIAPI
SmiControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
/*++

Routine Description:

Arguments:

Returns:

    None

--*/
// TODO:    This - add argument and description to function comment
// TODO:    Controller - add argument and description to function comment
// TODO:    RemainingDevicePath - add argument and description to function comment
{
  EFI_STATUS          Status;
  EFI_PCI_IO_PROTOCOL *PciIo;
  PCI_TYPE00          Pci;

  //
  // Open the PCI I/O Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Read the PCI Configuration Header from the PCI Device
  //
  Status = PciIo->Pci.Read (
                        PciIo,
                        EfiPciIoWidthUint32,
                        0,
                        sizeof (Pci) / sizeof (UINT32),
                        &Pci
                        );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = EFI_UNSUPPORTED;
  //
  // See if the I/O enable is on.  Most systems only allow one VGA device to be turned on
  // at a time, so see if this is one that is turned on.
  //
  //  if (((Pci.Hdr.Command & 0x01) == 0x01)) {
  //
  // See if this is a SMI PCI controller
  //
  if (Pci.Hdr.VendorId == SILICON_MOTION_VENDOR_ID) {
    //
    // See if this is a SM750 or a SM718 PCI controller
    //
    if (Pci.Hdr.DeviceId == SMI_DEVICE_ID) {
      Status = EFI_SUCCESS;
    }

/*    if (Pci.Hdr.DeviceId == SMI_ALTERNATE_DEVICE_ID) {
      Status = EFI_SUCCESS;
    }
*/
  }

Done:
  //
  // Close the PCI I/O Protocol
  //
  gBS->CloseProtocol (
        Controller,
        &gEfiPciIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );

  return Status;
}

EFI_STATUS
EFIAPI
SmiControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
/*++

Routine Description:

Arguments:

Returns:

    None

--*/
// TODO:    This - add argument and description to function comment
// TODO:    Controller - add argument and description to function comment
// TODO:    RemainingDevicePath - add argument and description to function comment
{
  EFI_STATUS                      Status;
  SMI_PRIVATE_DATA  *Private;

  //
  // Allocate Private context data for UGA Draw inteface.
  //
  Private = NULL;
  Private = EfiLibAllocateZeroPool (sizeof (SMI_PRIVATE_DATA));
  if (Private == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto Error;
  }

  //
  // Set up context record
  //
  Private->Signature  = SMI_PRIVATE_DATA_SIGNATURE;
  Private->Handle     = Controller;

  //
  // Open PCI I/O Protocol
  //
  Status = gBS->OpenProtocol (
                  Private->Handle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &Private->PciIo,
                  This->DriverBindingHandle,
                  Private->Handle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  Status = Private->PciIo->Attributes (
                            Private->PciIo,
                            EfiPciIoAttributeOperationEnable,
                            EFI_PCI_DEVICE_ENABLE | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
                            NULL
                            );
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Start the UGA Draw software stack.
  //
  Status = SmiUgaDrawConstructor (Private);
  if (EFI_ERROR (Status)) {
    goto Error;
  }

  //
  // Publish the UGA Draw interface to the world
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Private->Handle,
                  &gEfiUgaDrawProtocolGuid,
                  &Private->UgaDraw,
                  NULL
                  );

Error:
  if (EFI_ERROR (Status)) {
    if (Private) {
      if (Private->PciIo) {
        Private->PciIo->Attributes (
                          Private->PciIo,
                          EfiPciIoAttributeOperationDisable,
                          EFI_PCI_DEVICE_ENABLE | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
                          NULL
                          );
      }
    }

    //
    // Close the PCI I/O Protocol
    //
    gBS->CloseProtocol (
          Private->Handle,
          &gEfiPciIoProtocolGuid,
          This->DriverBindingHandle,
          Private->Handle
          );
    if (Private) {
      gBS->FreePool (Private);
    }
  }

  return Status;
}

EFI_STATUS
EFIAPI
SmiControllerDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN UINTN                          NumberOfChildren,
  IN EFI_HANDLE                     *ChildHandleBuffer
  )
/*++

Routine Description:

Arguments:

Returns:

    None

--*/
// TODO:    This - add argument and description to function comment
// TODO:    Controller - add argument and description to function comment
// TODO:    NumberOfChildren - add argument and description to function comment
// TODO:    ChildHandleBuffer - add argument and description to function comment
// TODO:    EFI_SUCCESS - add return value to function comment
{
  EFI_UGA_DRAW_PROTOCOL           *UgaDraw;
  EFI_STATUS                      Status;
  SMI_PRIVATE_DATA  *Private;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUgaDrawProtocolGuid,
                  (VOID **) &UgaDraw,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    //
    // If the UGA Draw interface does not exist the driver is not started
    //
    return Status;
  }

  //
  // Get our private context information
  //
  Private = SMI_PRIVATE_DATA_FROM_UGA_DRAW_THIS (UgaDraw);

  //
  // Remove the UGA Draw interface from the system
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  Private->Handle,
                  &gEfiUgaDrawProtocolGuid,
                  &Private->UgaDraw,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Shutdown the hardware
  //
  SmiUgaDrawDestructor (Private);

  //
  // Free Buffers
  //
  if (Private->LineBuffer != NULL) {
    gBS->FreePool (Private->LineBuffer);
  }

  Private->PciIo->Attributes (
                    Private->PciIo,
                    EfiPciIoAttributeOperationDisable,
                    EFI_PCI_DEVICE_ENABLE | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
                    NULL
                    );

  //
  // Close the PCI I/O Protocol
  //
  gBS->CloseProtocol (
        Controller,
        &gEfiPciIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );

  //
  // Free our instance data
  //
  gBS->FreePool (Private);

  return EFI_SUCCESS;
}
