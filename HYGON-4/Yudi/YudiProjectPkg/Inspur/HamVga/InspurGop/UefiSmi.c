/*++

Copyright (c) 2007, Intelligraphics, Inc.                                                      

Module Name:

  UefiSmi.c
    
Abstract:

  Silicon Motion Controller Driver.
  This driver is a sample implementation of the Graphics Output Protocol for the
  SMI family of PCI video controllers.  This driver is only
  usable in the UEFI pre-boot environment.  This sample is intended to show
  how the Graphics Output Protocol is able to function.

Revision History:

--*/

//
// SMI Controller Driver
//

#include "UefiSmi.h"


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

//EFI_DRIVER_ENTRY_POINT (SmiGraphicsOutputDriverEntryPoint)

/*++

Routine Description:

  Entry point for the protocol driver

Arguments:

  ImageHandle - Firmware allocated handle for the UEFI image

  SystemTable - Pointer to the EFI System Table

Returns:

  EFI_SUCCESS or EFI_OUT_OF_RESOURCES

--*/
EFI_STATUS
EFIAPI
SmiGraphicsOutputDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  SMIDEBUG("SmiGraphicsOutputDriverEntryPoint +\n");
  SMIDEBUG("SmiGraphicsOutputDriverEntryPoint -\n");

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

/*++

Routine Description:

  Checks to see if driver specified by 'This' supports the device
  specified by 'Controller'.

Arguments:
  This - Pointer to the EFI_DIRVER_BINDING_PROTOCOL instance

  Controller - Handle of controller to test

  RemainingDevicePath - Pointer to remaining portion of device path

Returns:

    See the EFI 2.0 specification.

--*/
EFI_STATUS
EFIAPI
SmiControllerDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS          Status;
  EFI_PCI_IO_PROTOCOL *PciIo;
  PCI_TYPE00          Pci;
  EFI_DEV_PATH        *Node;

  //SMIDEBUG("SmiControllerDriverSupported +\n");
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
                        PCI_BAR_FRAMEBUFFER,
                        sizeof (Pci) / sizeof (UINT32),
                        &Pci
                        );
  if (EFI_ERROR (Status)) {
    goto Done;
  }

  Status = EFI_UNSUPPORTED;
  //
  // If there was a need to check for PCI I/O Enable, this is the 
  // place to do so.  We'll assume the  is the only card in the
  // system.
  //
  // See if this is a Silicon Motion PCI controller
  //
  #ifdef NSFPGA
      if (Pci.Hdr.VendorId == NS_PCI_VENDOR_ID) {
        //
        // See if this is an SMI PCI controller
        //
        if (Pci.Hdr.DeviceId == NS_DEVICE_ID_SM750){
          Status = EFI_SUCCESS;
		  SMIDEBUG("Smi VendorID = %x \n", Pci.Hdr.VendorId);
		  SMIDEBUG("Smi DeviceID = %x \n", Pci.Hdr.DeviceId);

          //
          // go further check RemainingDevicePath validation
          //
          if (RemainingDevicePath != NULL) {
            Node = (EFI_DEV_PATH *) RemainingDevicePath;
            //
            // Check if RemainingDevicePath is the End of Device Path Node, 
            // if yes, return EFI_SUCCESS
            //
            if (!IsDevicePathEnd (Node)) {
              //
              // If RemainingDevicePath isn't the End of Device Path Node,
              // check its validation
              //
              if (Node->DevPath.Type != ACPI_DEVICE_PATH ||
                  Node->DevPath.SubType != ACPI_ADR_DP ||
                  DevicePathNodeLength(&Node->DevPath) != sizeof(ACPI_ADR_DEVICE_PATH)) {
                Status = EFI_UNSUPPORTED;
              }
            }
          }
        }
      }
  #else
      if (Pci.Hdr.VendorId == SILICON_MOTION_VENDOR_ID) {
        //
        // See if this is an SMI PCI controller
        //
        if (Pci.Hdr.DeviceId == SMI_DEVICE_ID) {
          Status = EFI_SUCCESS;
          //
          // go further check RemainingDevicePath validation
          //
          if (RemainingDevicePath != NULL) {
            Node = (EFI_DEV_PATH *) RemainingDevicePath;
            //
            // Check if RemainingDevicePath is the End of Device Path Node, 
            // if yes, return EFI_SUCCESS
            //
            if (!IsDevicePathEnd (Node)) {
              //
              // If RemainingDevicePath isn't the End of Device Path Node,
              // check its validation
              //
              if (Node->DevPath.Type != ACPI_DEVICE_PATH ||
                  Node->DevPath.SubType != ACPI_ADR_DP ||
                  DevicePathNodeLength(&Node->DevPath) != sizeof(ACPI_ADR_DEVICE_PATH)) {
                Status = EFI_UNSUPPORTED;
              }
            }
          }
        }
      }
  #endif

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

  //SMIDEBUG("SmiControllerDriverSupported - Status:%r\n",Status);
  return Status;
}

/*++

Routine Description:
  Starts the device specified by 'Controller' with the driver 
  specified by 'This'.

Arguments:
  This - Pointer to the EFI_DIRVER_BINDING_PROTOCOL instance

  Controller - Handle of controller to start

  RemainingDevicePath - Pointer to remaining portion of device path

Returns:

    EFI_SUCCESS, EFI_DEVICE_ERROR, or EFI_OUT_OF_RESOURCES

--*/
EFI_STATUS
EFIAPI
SmiControllerDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                      Status;
  SMI_PRIVATE_DATA  *Private;
  BOOLEAN                         PciAttributesSaved;
  EFI_DEVICE_PATH_PROTOCOL        *ParentDevicePath;
  ACPI_ADR_DEVICE_PATH            AcpiDeviceNode;
  UINT64                          Supports;

  SMIDEBUG("SmiControllerDriverStart +\n");
  DEBUG((EFI_D_INFO |EFI_D_ERROR, SMI_UEFI_DRV_VERSION));
  DEBUG((EFI_D_INFO |EFI_D_ERROR, "\n"));

  PciAttributesSaved = FALSE;
  //
  // Allocate Private context data for GOP inteface.
  //
  Private = NULL;
  Private = AllocateZeroPool (sizeof (SMI_PRIVATE_DATA));
  if (Private == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
	DEBUG ((EFI_D_ERROR,"line:%d,Status:%r\n",__LINE__,Status));
    goto Error;
  }

  //
  // Set up context record
  //
  Private->Signature  = SMI_PRIVATE_DATA_SIGNATURE;
  Private->Handle     = NULL;

  //
  // Open PCI I/O Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &Private->PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
	DEBUG ((EFI_D_ERROR,"line:%d,Status:%r\n",__LINE__,Status));
    goto Error;
  }

  //
  // Get supported PCI attributes
  //
  Status = Private->PciIo->Attributes (
                             Private->PciIo,
                             EfiPciIoAttributeOperationSupported,
                             0,
                             &Supports
                             );
  if (EFI_ERROR (Status)) {
	DEBUG ((EFI_D_ERROR,"line:%d,Status:%r\n",__LINE__,Status));
    goto Error;
  }

  Supports &= (EFI_PCI_IO_ATTRIBUTE_VGA_IO | EFI_PCI_IO_ATTRIBUTE_VGA_IO_16);
  if (Supports == 0 || Supports == (EFI_PCI_IO_ATTRIBUTE_VGA_IO | EFI_PCI_IO_ATTRIBUTE_VGA_IO_16)) {
    Status = EFI_UNSUPPORTED;
	SMIDEBUG ("line:%d,Status:%r\n",__LINE__,Status);
    //goto Error;
  }  

  //
  // Save original PCI attributes
  //
  Status = Private->PciIo->Attributes (
                    Private->PciIo,
                    EfiPciIoAttributeOperationGet,
                    0,
                    &Private->OriginalPciAttributes
                    );

  if (EFI_ERROR (Status)) {
	DEBUG ((EFI_D_ERROR,"line:%d,Status:%r\n",__LINE__,Status));
    goto Error;
  }
  PciAttributesSaved = TRUE;

  Status = Private->PciIo->Attributes (
                            Private->PciIo,
                            EfiPciIoAttributeOperationEnable,
							EFI_PCI_DEVICE_ENABLE,// | EFI_PCI_IO_ATTRIBUTE_VGA_MEMORY | EFI_PCI_IO_ATTRIBUTE_VGA_IO,
                            NULL
                            );
  if (EFI_ERROR (Status)) {
	DEBUG ((EFI_D_ERROR,"line:%d,Status:%r\n",__LINE__,Status));
    goto Error;
  }


  //
  // Get ParentDevicePath
  //
  Status = gBS->HandleProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath
                  );
  if (EFI_ERROR (Status)) {
	DEBUG ((EFI_D_ERROR,"line:%d,Status:%r\n",__LINE__,Status));
    goto Error;
  }

  SMIDEBUG("SmiControllerDriverStart Append Gop Device\n");

    //
    // Set Gop Device Path
    //
    if (RemainingDevicePath == NULL) {
      ZeroMem (&AcpiDeviceNode, sizeof (ACPI_ADR_DEVICE_PATH));
      AcpiDeviceNode.Header.Type = ACPI_DEVICE_PATH;
      AcpiDeviceNode.Header.SubType = ACPI_ADR_DP;
      AcpiDeviceNode.ADR = ACPI_DISPLAY_ADR (1, 0, 0, 1, 0, ACPI_ADR_DISPLAY_TYPE_VGA, 0, 0);
      SetDevicePathNodeLength (&AcpiDeviceNode.Header, sizeof (ACPI_ADR_DEVICE_PATH));

      Private->GopDevicePath = AppendDevicePathNode (
                                          ParentDevicePath,
                                          (EFI_DEVICE_PATH_PROTOCOL *) &AcpiDeviceNode
                                          );
    } else if (!IsDevicePathEnd (RemainingDevicePath)) {
      //
      // If RemainingDevicePath isn't the End of Device Path Node, 
      // only scan the specified device by RemainingDevicePath
      //
      Private->GopDevicePath = AppendDevicePathNode (ParentDevicePath, RemainingDevicePath);
    } else {
      //
      // If RemainingDevicePath is the End of Device Path Node, 
      // don't create child device and return EFI_SUCCESS
      //
      Private->GopDevicePath = NULL;
    }
      
    if (Private->GopDevicePath != NULL) {
      //
      // Creat child handle and device path protocol firstly
      //
      SMIDEBUG("Install GopDevicePath Handle+ %lx\n", Private->Handle);
      Private->Handle = NULL;
      Status = gBS->InstallMultipleProtocolInterfaces (
                      &Private->Handle,
                      &gEfiDevicePathProtocolGuid,
                      Private->GopDevicePath,
                      NULL
                      );
      SMIDEBUG("Install GopDevicePath Handle-:%lx,Status:%r\n", Private->Handle,Status);
    }
  

	if (Private->GopDevicePath == NULL) {
	//
	// If RemainingDevicePath is the End of Device Path Node, 
	// don't create child device and return EFI_SUCCESS
	//
	Status = EFI_SUCCESS;
	} else {
	//
    // Start the GOP software stack.
    //
    SMIDEBUG("SmiControllerDriverStart Start the GOP software stack\n");
    Status = SmiGraphicsOutputConstructor (Private);
    if (EFI_ERROR (Status)) {
	  DEBUG ((EFI_D_ERROR,"line:%d,Status:%r\n",__LINE__,Status));
      goto Error;
    }
	SMIDEBUG("Install GopProcotol Handle %lx\n", Controller);
    //
    // Publish the Graphics Output Protocol interface to the world
    //
    Status = gBS->InstallMultipleProtocolInterfaces (
	              &Controller,//&Private->Handle,
                  &gEfiGraphicsOutputProtocolGuid,
                  &Private->GraphicsOutput,
                  NULL
                  );
    SMIDEBUG("install GOP Status:%r,Handle:%x\n", Status,Controller);
	}

#if 0 
	  //
	  // Open the Parent Handle for the child
	  //
	  Status = gBS->OpenProtocol (
	                  Controller,
	                  &gEfiPciIoProtocolGuid,
	                  (VOID **) &Private->PciIo,
	                  This->DriverBindingHandle,
	                  Private->Handle,
	                  EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
	                  );
	  if (EFI_ERROR (Status)) {
		DEBUG ((EFI_D_ERROR,"line:%d,Status:%r\n",__LINE__,Status));
	    goto Error;
 	  }
#endif
	  
Error:
  if (EFI_ERROR (Status)) {
    if (Private) {
      if (Private->PciIo) {
        if (PciAttributesSaved == TRUE) {
          //
          // Restore original PCI attributes
          //
          Private->PciIo->Attributes (
                          Private->PciIo,
                          EfiPciIoAttributeOperationSet,
                          Private->OriginalPciAttributes,
                          NULL
                          );
        }
        //
        // Close the PCI I/O Protocol
        //
        gBS->CloseProtocol (
              Controller,//Private->Handle,
              &gEfiPciIoProtocolGuid,
              This->DriverBindingHandle,
              Controller//Private->Handle
              );
      }

      //
      // Free our instance data
      //
      gBS->FreePool (Private);
      Private = NULL;
    }
  }

  SMIDEBUG("SmiControllerDriverStart - Status:%r\n",Status);
  return Status;
}

/**
  SmiControllerDriverStop

  TODO:    This - add argument and description to function comment
  TODO:    Controller - add argument and description to function comment
  TODO:    NumberOfChildren - add argument and description to function comment
  TODO:    ChildHandleBuffer - add argument and description to function comment
  TODO:    EFI_SUCCESS - add return value to function comment
**/
EFI_STATUS
EFIAPI
SmiControllerDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN UINTN                          NumberOfChildren,
  IN EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_GRAPHICS_OUTPUT_PROTOCOL    *GraphicsOutput;
  EFI_STATUS                      Status;
  SMI_PRIVATE_DATA  *Private;

  SMIDEBUG("SmiControllerDriverStop +\n");

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID **) &GraphicsOutput,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    //
    // If the Graphics Output Protocol interface does not exist the driver is not started
    //
    return Status;
  }

  //
  // Get our private context information
  //
  Private = SMI_PRIVATE_DATA_FROM_GRAPHICS_OUTPUT_THIS (GraphicsOutput);
  
  //
  // Remove the GOP(Graphics Output Protocol)  interface from the system
  //
  Status = gBS->UninstallMultipleProtocolInterfaces (
                  Controller,//Private->Handle,
                  &gEfiGraphicsOutputProtocolGuid,
                  &Private->GraphicsOutput,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Shutdown the hardware
  //
  SmiGraphicsOutputDestructor (Private);

  //
  // Free Buffers
  //
  if (Private->VirtualBuffer != NULL) {
    gBS->FreePool (Private->VirtualBuffer);
    Private->VirtualBuffer = NULL;
  }

  //
  // Restore original PCI attributes
  //
  Private->PciIo->Attributes (
                  Private->PciIo,
                  EfiPciIoAttributeOperationDisable,
                  //Private->OriginalPciAttributes,
                  EFI_PCI_DEVICE_ENABLE,
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
  Private = NULL;
  SMIDEBUG("SmiControllerDriverStop -\n");

  return EFI_SUCCESS;
}
