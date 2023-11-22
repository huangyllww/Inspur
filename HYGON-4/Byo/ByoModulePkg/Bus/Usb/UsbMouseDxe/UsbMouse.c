/** @file
  PS/2 Mouse driver. Routines that interacts with callers,
  conforming to EFI driver model.
  
Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "UsbMouse.h"

USB_MOUSE_COMMUNICATION_PROTOCOL   *mUsbMouseCommunicationInstance;

///
/// DriverBinding Protocol Instance
///
EFI_DRIVER_BINDING_PROTOCOL gUsbMouseDriverBinding = {
  UsbMouseDriverSupported,
  UsbMouseDriverStart,
  UsbMouseDriverStop,
  0xa,
  NULL,
  NULL
};

/**
  Get mouse packet . Only care first 3 bytes

  @param MouseDev  Pointer of Usb Mouse Private Data Structure 

  @retval EFI_NOT_READY  Mouse Device not ready to input data packet, or some error happened during getting the packet
  @retval EFI_SUCCESS    The data packet is gotten successfully.

**/
EFI_STATUS
USBMouseGetPacket (
   USB_MOUSE_DEV 	*MouseDev
   )
{
  BOOLEAN LButton   = FALSE;
  BOOLEAN RButton   = FALSE;
  BOOLEAN ValidData = FALSE;

  if(mUsbMouseCommunicationInstance == NULL || ((mUsbMouseCommunicationInstance->MouseDataStatus & 0x03) == 0x00)){
    return EFI_DEVICE_ERROR;
  }

  //
  // Decode the packet
  //
  if (mUsbMouseCommunicationInstance->MouseDataStatus == 0x01 || mUsbMouseCommunicationInstance->MouseDataStatus == 0x83) {
    RButton = mUsbMouseCommunicationInstance->CurrentMouseData0[0] & 0x02;
    LButton = mUsbMouseCommunicationInstance->CurrentMouseData0[0] & 0x01;
    //
    // Update mouse state
    //
    MouseDev->State.RelativeMovementX += (INT8)mUsbMouseCommunicationInstance->CurrentMouseData0[1];
    MouseDev->State.RelativeMovementY += (INT8)mUsbMouseCommunicationInstance->CurrentMouseData0[2];
    if (mUsbMouseCommunicationInstance->MouseDataStatus == 0x01 ){
      mUsbMouseCommunicationInstance->MouseDataStatus = 0x00; 
    } else {
      mUsbMouseCommunicationInstance->MouseDataStatus = 0x02;
    }
    ValidData = TRUE;
  } else if ((mUsbMouseCommunicationInstance->MouseDataStatus & 0x02) == 0x02) {
    RButton = mUsbMouseCommunicationInstance->CurrentMouseData1[0] & 0x02;
    LButton = mUsbMouseCommunicationInstance->CurrentMouseData1[0] & 0x01;
    //
    // Update mouse state
    //
    MouseDev->State.RelativeMovementX += (INT8)mUsbMouseCommunicationInstance->CurrentMouseData1[1];
    MouseDev->State.RelativeMovementY += (INT8)mUsbMouseCommunicationInstance->CurrentMouseData1[2];
    mUsbMouseCommunicationInstance->MouseDataStatus &= 0xFD;
    ValidData = TRUE;
  }
  if (ValidData) {
    MouseDev->State.RightButton = (UINT8) (RButton ? TRUE : FALSE);
    MouseDev->State.LeftButton  = (UINT8) (LButton ? TRUE : FALSE);
    MouseDev->StateChanged	     = TRUE;
  }

   return EFI_SUCCESS;

}

/**
  Uses USB I/O to check whether the device is a USB mouse device.

  @param  UsbIo    Pointer to a USB I/O protocol instance.

  @retval TRUE     Device is a USB mouse device.
  @retval FALSE    Device is a not USB mouse device.

**/
BOOLEAN
IsUsbMouse (
  IN  EFI_USB_IO_PROTOCOL     *UsbIo
  )
{
  EFI_STATUS                    Status;
  EFI_USB_INTERFACE_DESCRIPTOR  InterfaceDescriptor;

  //
  // Get the default interface descriptor
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (
                    UsbIo,
                    &InterfaceDescriptor
                    );

  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if ((InterfaceDescriptor.InterfaceClass == CLASS_HID) &&
      (InterfaceDescriptor.InterfaceSubClass == SUBCLASS_BOOT) &&
      (InterfaceDescriptor.InterfaceProtocol == PROTOCOL_MOUSE)
      ) {
    return TRUE;
  }

  return FALSE;
}

/**
  Test to see if this driver supports ControllerHandle. Any ControllerHandle
  than contains a IsaIo protocol can be supported.

  @param  This                Protocol instance pointer.
  @param  ControllerHandle    Handle of device to test
  @param  RemainingDevicePath Optional parameter use to pick a specific child
                              device to start.

  @retval EFI_SUCCESS         This driver supports this device
  @retval EFI_ALREADY_STARTED This driver is already running on this device
  @retval other               This driver does not support this device

**/
EFI_STATUS
EFIAPI
UsbMouseDriverSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS          Status;
  EFI_USB_IO_PROTOCOL *UsbIo;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbIoProtocolGuid,
                  (VOID **) &UsbIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Use the USB I/O Protocol interface to check whether Controller is
  // a mouse device that can be managed by this driver.
  //
  Status = EFI_SUCCESS;
  if (!IsUsbMouse (UsbIo)) {
    Status = EFI_UNSUPPORTED;
  }

  gBS->CloseProtocol (
        Controller,
        &gEfiUsbIoProtocolGuid,
        This->DriverBindingHandle,
        Controller
        );

  return Status;
}



EFI_STATUS
EFIAPI
MyMouseReset (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN BOOLEAN                        ExtendedVerification
  )
{
  return EFI_SUCCESS;
}

/**
  Start this driver on ControllerHandle by opening a IsaIo protocol, creating 
  Usb_MOUSE_ABSOLUTE_POINTER_DEV device and install gEfiAbsolutePointerProtocolGuid
  finally.

  @param  This                 Protocol instance pointer.
  @param  ControllerHandle     Handle of device to bind driver to
  @param  RemainingDevicePath  Optional parameter use to pick a specific child
                               device to start.

  @retval EFI_SUCCESS          This driver is added to ControllerHandle
  @retval EFI_ALREADY_STARTED  This driver is already running on ControllerHandle
  @retval other                This driver does not support this device

**/
EFI_STATUS
EFIAPI
UsbMouseDriverStart (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_USB_IO_PROTOCOL                 *UsbIo;
  USB_MOUSE_DEV                       *MouseDev;
  EFI_TPL                             OldTpl;
  EFI_DEVICE_PATH_PROTOCOL            *ParentDevicePath;

  OldTpl = gBS->RaiseTPL (TPL_CALLBACK);

  MouseDev    = NULL;
  
  //
  // Open USB I/O Protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiUsbIoProtocolGuid,
                  (VOID **) &UsbIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
    goto ErrorExit;
  }
  
  //
  // Open the device path protocol
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
    goto ErrorExit;
  }
  
  //
  // Report Status Code here since USB mouse will be detected next.
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_PRESENCE_DETECT),
    ParentDevicePath
    );

  //
  // Raise TPL to avoid keyboard operation impact
  //
//  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);

  //
  // Allocate private data
  //
  MouseDev = AllocateZeroPool (sizeof (USB_MOUSE_DEV));
  if (MouseDev == NULL) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }
  //
  // Setup the device instance
  //
  MouseDev->Signature       = USB_MOUSE_DEV_SIGNATURE;
  MouseDev->Handle          = Controller;
  MouseDev->SampleRate      = SampleRate20;
  MouseDev->Resolution      = MouseResolution4;
  MouseDev->Scaling         = Scaling1;
  MouseDev->DataPackageSize = 3;
  MouseDev->DevicePath      = ParentDevicePath;
  MouseDev->UsbIo           = UsbIo;

  //
  // Resolution = 4 counts/mm
  //
  MouseDev->Mode.ResolutionX                = 4;
  MouseDev->Mode.ResolutionY                = 4;
  MouseDev->Mode.LeftButton                 = TRUE;
  MouseDev->Mode.RightButton                = TRUE;

  MouseDev->SimplePointerProtocol.Reset     = MyMouseReset;
  MouseDev->SimplePointerProtocol.GetState  = MouseGetState;
  MouseDev->SimplePointerProtocol.Mode      = &(MouseDev->Mode);
  
  //
  // Report Status Code here since USB mouse will be detected next.
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_MOUSE | EFI_P_PC_PRESENCE_DETECT,
    ParentDevicePath
    );

  //
  // Setup the WaitForKey event
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_WAIT,
                  TPL_NOTIFY,
                  MouseWaitForInput,
                  MouseDev,
                  &((MouseDev->SimplePointerProtocol).WaitForInput)
                  );
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }
  
  //
  // Setup a periodic timer, used to poll mouse state
  //
  Status = gBS->CreateEvent (
                  EVT_TIMER | EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PollMouse,
                  MouseDev,
                  &MouseDev->TimerEvent
                  );
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }
  
  //
  // Start timer to poll mouse (100 samples per second)
  //
  Status = gBS->SetTimer (MouseDev->TimerEvent, TimerPeriodic, 100000);
  if (EFI_ERROR (Status)) {
    Status = EFI_OUT_OF_RESOURCES;
    goto ErrorExit;
  }

  Status = gBS->LocateProtocol (
                  &gEfiUsbMouseCommunicationProtocolGuid,
                  NULL,
                  &mUsbMouseCommunicationInstance
                  );
  if (EFI_ERROR (Status)) {
    mUsbMouseCommunicationInstance = NULL;
    DEBUG((EFI_D_ERROR, "MouseComm Protocol not found\n"));
    goto ErrorExit;
  } else {
    DEBUG((EFI_D_INFO,  "MouseComm Protocol found %X\n",mUsbMouseCommunicationInstance));
  }

  MouseDev->ControllerNameTable = NULL;
  AddUnicodeString2 (
    "eng",
    gUsbMouseComponentName.SupportedLanguages,
    &MouseDev->ControllerNameTable,
    L"Generic Usb Mouse",
    TRUE
    );
  AddUnicodeString2 (
    "en",
    gUsbMouseComponentName2.SupportedLanguages,
    &MouseDev->ControllerNameTable,
    L"Generic Usb Mouse",
    FALSE
    );


  //
  // Install protocol interfaces for the mouse device.
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Controller,
                  &gEfiSimplePointerProtocolGuid,
                  &MouseDev->SimplePointerProtocol,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    goto ErrorExit;
  }

  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    (EFI_PERIPHERAL_MOUSE | EFI_P_PC_ENABLE),
    MouseDev->DevicePath
    );

  gBS->RestoreTPL (OldTpl);

  return EFI_SUCCESS;

ErrorExit:

  if ((MouseDev != NULL) && (MouseDev->SimplePointerProtocol.WaitForInput != NULL)) {
    gBS->CloseEvent (MouseDev->SimplePointerProtocol.WaitForInput);
  }

  if ((MouseDev != NULL) && (MouseDev->TimerEvent != NULL)) {
    gBS->CloseEvent (MouseDev->TimerEvent);
  }

  if ((MouseDev != NULL) && (MouseDev->ControllerNameTable != NULL)) {
    FreeUnicodeStringTable (MouseDev->ControllerNameTable);
  }

  if (MouseDev != NULL) {
    FreePool (MouseDev);
  }

  gBS->CloseProtocol (
         Controller,
         &gEfiUsbIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  gBS->RestoreTPL (OldTpl);

  return Status;
}

/**
  Stop this driver on ControllerHandle. Support stopping any child handles
  created by this driver.

  @param  This              Protocol instance pointer.
  @param  ControllerHandle  Handle of device to stop driver on
  @param  NumberOfChildren  Number of Handles in ChildHandleBuffer. If number of
                            children is zero stop the entire bus driver.
  @param  ChildHandleBuffer List of Child Handles to Stop.

  @retval EFI_SUCCESS       This driver is removed ControllerHandle
  @retval other             This driver was not removed from this device

**/
EFI_STATUS
EFIAPI
UsbMouseDriverStop (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN UINTN                          NumberOfChildren,
  IN EFI_HANDLE                     *ChildHandleBuffer
  )
{
  EFI_STATUS                  Status;
  EFI_SIMPLE_POINTER_PROTOCOL *SimplePointerProtocol;
  USB_MOUSE_DEV               *MouseDev;
//UINT8                       Data;

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  (VOID **) &SimplePointerProtocol,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }

  MouseDev = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL (SimplePointerProtocol);

  //
  // Report that the keyboard is being disabled
  //
  REPORT_STATUS_CODE_WITH_DEVICE_PATH (
    EFI_PROGRESS_CODE,
    EFI_PERIPHERAL_MOUSE | EFI_P_PC_DISABLE,
    MouseDev->DevicePath
    );

  Status = gBS->UninstallProtocolInterface (
                  Controller,
                  &gEfiSimplePointerProtocolGuid,
                  &MouseDev->SimplePointerProtocol
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  gBS->CloseProtocol (
         Controller,
         &gEfiUsbIoProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );
  
  //
  // Cancel mouse data polling timer, close timer event
  //
  gBS->SetTimer (MouseDev->TimerEvent, TimerCancel, 0);
  gBS->CloseEvent (MouseDev->TimerEvent);

  //
  // Since there will be no timer handler for mouse input any more,
  // exhaust input data just in case there is still mouse data left
  //
  Status = EFI_SUCCESS;

  gBS->CloseEvent (MouseDev->SimplePointerProtocol.WaitForInput);
  FreeUnicodeStringTable (MouseDev->ControllerNameTable);
  FreePool (MouseDev);

  return EFI_SUCCESS;
}

/**
  Get and Clear mouse status.
  
  @param This                 - Pointer of simple pointer Protocol.
  @param State                - Output buffer holding status.

  @retval EFI_INVALID_PARAMETER Output buffer is invalid.
  @retval EFI_NOT_READY         Mouse is not changed status yet.
  @retval EFI_SUCCESS           Mouse status is changed and get successful.
**/
EFI_STATUS
EFIAPI
MouseGetState (
  IN EFI_SIMPLE_POINTER_PROTOCOL    *This,
  IN OUT EFI_SIMPLE_POINTER_STATE   *State
  )
{
  USB_MOUSE_DEV *MouseDev;
  EFI_TPL       OldTpl;

  MouseDev = USB_MOUSE_DEV_FROM_MOUSE_PROTOCOL (This);

  if (State == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (!MouseDev->StateChanged) {
    return EFI_NOT_READY;
  }

  OldTpl = gBS->RaiseTPL (TPL_NOTIFY);
  CopyMem (State, &(MouseDev->State), sizeof (EFI_SIMPLE_POINTER_STATE));

  //
  // clear mouse state
  //
  MouseDev->State.RelativeMovementX = 0;
  MouseDev->State.RelativeMovementY = 0;
  MouseDev->State.RelativeMovementZ = 0;
  MouseDev->StateChanged            = FALSE;
  gBS->RestoreTPL (OldTpl);

  return EFI_SUCCESS;
}

/**

  Event notification function for SIMPLE_POINTER.WaitForInput event.
  Signal the event if there is input from mouse.

  @param Event    event object
  @param Context  event context

**/
VOID
EFIAPI
MouseWaitForInput (
  IN  EFI_EVENT               Event,
  IN  VOID                    *Context
  )
{
  USB_MOUSE_DEV *MouseDev;

  MouseDev = (USB_MOUSE_DEV *) Context;

  //
  // Someone is waiting on the mouse event, if there's
  // input from mouse, signal the event
  //
  if (MouseDev->StateChanged) {
    gBS->SignalEvent (Event);
  }

}

/**
  Event notification function for TimerEvent event.
  If mouse device is connected to system, try to get the mouse packet data.

  @param Event      -  TimerEvent in USB_MOUSE_DEV
  @param Context    -  Pointer to USB_MOUSE_DEV structure

**/
VOID
EFIAPI
PollMouse (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )

{
  USB_MOUSE_DEV *MouseDev;

  MouseDev = (USB_MOUSE_DEV *) Context;
  USBMouseGetPacket (MouseDev);

}

/**
  The user Entry Point for module UsbMouse. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.  
  @param[in] SystemTable    A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeUsbMouse(
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  //
  // Install driver model protocol(s).
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
             ImageHandle,
             SystemTable,
             &gUsbMouseDriverBinding,
             ImageHandle,
             &gUsbMouseComponentName,
             &gUsbMouseComponentName2
             );
  ASSERT_EFI_ERROR (Status);


  return Status;
}

