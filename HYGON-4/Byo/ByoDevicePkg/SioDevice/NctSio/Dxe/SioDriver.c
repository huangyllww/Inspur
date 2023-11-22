
/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#include "SioDriver.h"
extern UINT8 mSerialPort1ConfigValue[5][4];
extern UINT16 mSioLptIoValueTable[4];
extern UINT8 mSioLptIrqValueTable[2];
extern UINT8 mSioLptDmaChannelValueTable[2];

extern ACPI_SIO_RESOURCES_IO_IRQ mCom1Resources;
extern ACPI_SIO_RESOURCES_IO_IRQ_DMA mLpt1Resources;

#define SB_TSI_BASE_ADDR        0x2A0
#define SMBUS_OFFSET_TSI_AGENT_ENABLE      0xD

//
// This driver is for ACPI(PNP0A03,0)/PCI(0x1f,0)
//
//
//  Sio Driver Global Variables
//
EFI_DRIVER_BINDING_PROTOCOL mSioDriver = {
  SioDriverSupported,
  SioDriverStart,
  SioDriverStop,
  1,
  NULL,
  NULL
};

//
// The list of the created SIO_DEV
//
LIST_ENTRY                  mSioDevPool = INITIALIZE_LIST_HEAD_VARIABLE (mSioDevPool);

//
// Template structure to create SIO_DEV
//
SIO_DEV                     mSioDevTemplate = {
  SIO_DEV_SIGNATURE,        // Signature
  NULL,                     // PciHandle
  {
    0x00000000,             // HID
    0x00000000              // UID
  },
  NULL,                     // Handle
  {                         // Sio Instance
    SioRegisterAccess,
    SioGetResources,
    SioSetResources,
    SioPossibleResources,
    SioModify
  },
  NULL,                     // DevicePath
  {
    NULL,                   // ForwardLink
    NULL,                   // BackLink
  }
};

//
// Template ACPI_HID_DEVICE_PATH structure to create device path
//
ACPI_HID_DEVICE_PATH        mAcpiNodeTemplate = {
  {
    ACPI_DEVICE_PATH,       // Type
    ACPI_DP,                // SubType
    {
      sizeof (ACPI_HID_DEVICE_PATH),  // Length[0]
      0                               // Length[1]
    }
  },
  0x00000000,               // HID
  0x00000000                // UID
};

VOID
SioUartInit()
{
  //MmioOr32(LPC_PCI_REG(0x44), 0x3F00);              // FCH::ITF::LPC::IOPortDecodeEn Bit8-13
  UINTN                      VarSize;
  SIO_SETUP_CONFIG           SioConfig;
  EFI_STATUS                 Status;

  VarSize = sizeof(SIO_SETUP_CONFIG);
  Status = gRT->GetVariable (
                  SIO_SETUP_VARIABLE_NAME,
                  &gSioSetupConfigGuid,
                  NULL,
                  &VarSize,
                  &SioConfig
                  );

  EnterSioCfgMode();
  //UART A 
  //SioRegAndThenOr(0x22, ~BIT4, BIT4); //not powerdown
  //SioRegAndThenOr(0x25, ~BIT4, 0x00); //tri state
  //SioRegAndThenOr(0x26, ~BIT1, 0x00); // legacy for irq enable

  WriteSioReg(R_SIO_LOGICAL_DEVICE, 0x02); //LDN2 UARTA
  WriteSioReg(0x2A, 0x00);
  WriteSioReg(R_SIO_BASE_ADDRESS_HIGH, 0x03); // decode 3F8
  WriteSioReg(R_SIO_BASE_ADDRESS_LOW, 0xF8);
  WriteSioReg(R_SIO_PRIMARY_INTERRUPT_SELECT, 0x04); //IRQ 5
  WriteSioReg(0xF0, 0x00); // CLK 1.8462
  WriteSioReg(R_SIO_ACTIVATE, SioConfig.UartAEn); // enable
  DEBUG((EFI_D_INFO, "ADDR H : %X L : %X \n", ReadSioReg(R_SIO_BASE_ADDRESS_HIGH), ReadSioReg(R_SIO_BASE_ADDRESS_LOW)));

  //UART B
  //SioRegAndThenOr(0x22, ~BIT5, BIT5); //not powerdown
  //SioRegAndThenOr(0x25, ~BIT5, 0x00); //tri state
  //SioRegAndThenOr(0x26, ~BIT1, 0x00); // legacy for irq enable

  WriteSioReg(R_SIO_LOGICAL_DEVICE, 0x03); //LDN3 UARTB
  WriteSioReg(0x2A, 0x00);
  WriteSioReg(R_SIO_BASE_ADDRESS_HIGH, 0x02); //decode  2E8
  WriteSioReg(R_SIO_BASE_ADDRESS_LOW, 0xf8);
  WriteSioReg(R_SIO_PRIMARY_INTERRUPT_SELECT, 0x3); // IRQ 6
  WriteSioReg(0xF0, 0x00);    //CLK 1.8462
  WriteSioReg(R_SIO_ACTIVATE, SioConfig.UartBEn); //enable
  DEBUG((EFI_D_INFO, "ADDR H : %X L : %X \n", ReadSioReg(R_SIO_BASE_ADDRESS_HIGH), ReadSioReg(R_SIO_BASE_ADDRESS_LOW)));
  ExitSioCfgMode();
}

VOID
DisableSioFdt()
{
  EnterSioCfgMode();

  WriteSioReg(R_SIO_LOGICAL_DEVICE, 0x00); //FDC
  WriteSioReg(R_SIO_ACTIVATE, 0x00); // disable
  
  ExitSioCfgMode();
}

VOID
DisableSioLpt()
{
  EnterSioCfgMode();

  WriteSioReg(R_SIO_LOGICAL_DEVICE, 0x01); //LPT
  WriteSioReg(R_SIO_ACTIVATE, 0x00); // disable
  
  ExitSioCfgMode();
}

VOID
SetTsiBaseAddr()
{
  EnterSioCfgMode();

  WriteSioReg(R_SIO_LOGICAL_DEVICE, 0x0B); //Hardware Monitor, Front Panel LED
  WriteSioReg(R_SIO_TSI_BASE_ADDR_MSB, 0x02); // tsi base address:0x2A0
  WriteSioReg(R_SIO_TSI_BASE_ADDR_LSB, 0xA0);
  ExitSioCfgMode();
  IoWrite8(SB_TSI_BASE_ADDR + SMBUS_OFFSET_TSI_AGENT_ENABLE, BIT0);
}

VOID
SioVariableInit()
{
  
  EFI_STATUS                 Status;
  UINTN                      VarSize;
  SIO_SETUP_CONFIG           SioConfig;

  VarSize = sizeof(SIO_SETUP_CONFIG);
  Status = gRT->GetVariable (
                  SIO_SETUP_VARIABLE_NAME,
                  &gSioSetupConfigGuid,
                  NULL,
                  &VarSize,
                  &SioConfig
                  );
  DEBUG((EFI_D_INFO, "GET SIO_SETUP_VARIABLE_NAME Status:%r \n", Status));
  if(Status == EFI_NOT_FOUND){
    ZeroMem(&SioConfig, sizeof(SIO_SETUP_CONFIG));
    Status = gRT->SetVariable (
                    SIO_SETUP_VARIABLE_NAME,
                    &gSioSetupConfigGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VarSize,
                    &SioConfig
                    );
      DEBUG((EFI_D_INFO, "Init Default Sio setup variable.\n"));
  }


}



/**
  The user Entry Point for module NCT6126. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
SioDriverEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{

  UINT16                  SioChipId;

  SioChipId = GetSioChipId();
  DEBUG((EFI_D_INFO, "%a ChipId %X\n", __FUNCTION__, SioChipId));
  if (SioChipId != NCT6776_CHIP_ID && SioChipId != NCT6126_CHIP_ID) {
    return EFI_UNSUPPORTED;
  }
  // Enable LPC 0x290 Decode for sio HWmonitor  
  MmioAndThenOr32(LPC_PCI_REG(0x90), 0, 0x290);
  MmioOr32(LPC_PCI_REG(0x48), BIT25);
  // uart resource
  MmioOr32(LPC_PCI_REG(0x44), BIT6|BIT7|BIT8|BIT9|BIT10|BIT11|BIT12|BIT13);
  SioUartInit();
  DisableSioFdt();
  DisableSioLpt();
  SioVariableInit();
  SioHwmDxeInit();
  SetTsiBaseAddr();
  EfiLibInstallDriverBinding (ImageHandle, SystemTable, &mSioDriver, ImageHandle);

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS                Status;
  PCI_TYPE00                Pci;
  UINTN                     Segment;
  UINTN                     Bus;
  UINTN                     Device;
  UINTN                     Function;
  PCI_DEVICE_PATH           *PciNode;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  ACPI_HID_DEVICE_PATH      *AcpiNode;
  EFI_DEVICE_PATH_PROTOCOL  *IsaBridgeDevicePath;


  //
  // Get the ISA bridge's Device Path and test it
  // the following code is specific
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&IsaBridgeDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = EFI_SUCCESS;
  AcpiNode =  (ACPI_HID_DEVICE_PATH *)IsaBridgeDevicePath;

  if (AcpiNode->Header.Type != ACPI_DEVICE_PATH ||
      AcpiNode->Header.SubType != ACPI_DP ||
      DevicePathNodeLength (&AcpiNode->Header) != sizeof(ACPI_HID_DEVICE_PATH) ||
      AcpiNode->HID != EISA_PNP_ID(0x0A03) ||
      AcpiNode->UID != 0 ) {

    Status = EFI_UNSUPPORTED;
  } else {
    //
    // Get the next node
    //
    IsaBridgeDevicePath = NextDevicePathNode (IsaBridgeDevicePath);
    PciNode  = (PCI_DEVICE_PATH *)IsaBridgeDevicePath;
    if (DevicePathType(PciNode) != HARDWARE_DEVICE_PATH || DevicePathSubType(PciNode) != HW_PCI_DP ||
        DevicePathNodeLength (&PciNode->Header) != sizeof (PCI_DEVICE_PATH) ||
        PciNode->Function != 0x03 || PciNode->Device != 0x14 ) { //BUGBUG, here should not use hardcode 0x3,0x14
      Status = EFI_UNSUPPORTED;
    }
  }


  gBS->CloseProtocol (
         Controller,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         Controller
         );

  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }


  //
  // Get PciIo protocol instance
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (!EFI_ERROR (Status)) {
    Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint32,
                          0,
                          sizeof (Pci) / sizeof (UINT32),
                          &Pci
                          );
    ASSERT_EFI_ERROR (Status);

    Status = EFI_UNSUPPORTED;

    if ((Pci.Hdr.Command & (EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE))
                        == (EFI_PCI_COMMAND_IO_SPACE | EFI_PCI_COMMAND_MEMORY_SPACE)
       ) {
      if (Pci.Hdr.ClassCode[2] == PCI_CLASS_BRIDGE) {
        //
        // See if this is a standard PCI to ISA Bridge from the Base Code and Class Code
        //
        if (Pci.Hdr.ClassCode[1] == PCI_CLASS_BRIDGE_ISA) {
          Status = EFI_SUCCESS;
        }

        //
        // See if this is an Intel PCI to ISA Bridge in Positive Decode Mode
        //
        if ((Pci.Hdr.ClassCode[1] == PCI_CLASS_BRIDGE_ISA_PDECODE) &&
            (Pci.Hdr.VendorId == 0x1D94)) { //BUGBUG, here should not use hardcode 0x1D94
          //
          // See if this is on Function #0 to avoid false positive on
          // PCI_CLASS_BRIDGE_OTHER that has the same value as
          // PCI_CLASS_BRIDGE_ISA_PDECODE
          //
          Status = PciIo->GetLocation (
                            PciIo,
                            &Segment,
                            &Bus,
                            &Device,
                            &Function
                            );
          if (!EFI_ERROR (Status)) {
            Status = EFI_SUCCESS;
          } else {
            Status = EFI_UNSUPPORTED;
          }
        }
      }
    }

    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
  }
  if (EFI_ERROR (Status) && (Status != EFI_ALREADY_STARTED)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Destroy the SIO controller handle.

  @param[in]  ChildHandle     The SIO controller handle.

  @retval EFI_SUCCESS         The SIO controller handle is destroyed successfully.
**/
EFI_STATUS
SioDestroyDevice (
  EFI_HANDLE                ChildHandle
  )
{
  EFI_STATUS                Status;
  SIO_DEV                   *SioDev;
  EFI_SIO_PROTOCOL          *Sio;
  EFI_PCI_IO_PROTOCOL       *PciIo;

  Status = gBS->HandleProtocol (
                  ChildHandle,
                  &gEfiSioProtocolGuid,
                  (VOID **) &Sio
                  );
  ASSERT_EFI_ERROR (Status);

  SioDev = SIO_DEV_FROM_THIS (Sio);

  Status = gBS->CloseProtocol (
                  SioDev->PciHandle,
                  &gEfiPciIoProtocolGuid,
                  mSioDriver.DriverBindingHandle,
                  ChildHandle
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->UninstallMultipleProtocolInterfaces (
                  ChildHandle,
                  &gEfiDevicePathProtocolGuid,
                  SioDev->DevicePath,
                  &gEfiSioProtocolGuid,
                  &SioDev->Sio,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    gBS->OpenProtocol (
           SioDev->PciHandle,
           &gEfiPciIoProtocolGuid,
           (VOID **) &PciIo,
           mSioDriver.DriverBindingHandle,
           ChildHandle,
           EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
           );
    return Status;
  }

  RemoveEntryList (&SioDev->Link);
  FreePool (SioDev->DevicePath);
  FreePool (SioDev);
  return EFI_SUCCESS;
}

/**
  Create the SIO controller handle.

  @param[in] Controller       The parent PCI controller handle.
  @param[in] Device           Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[in] ParentDevicePath The device path of the parent controller.
  @param[in] PciIo            The PciIo instance of the parent controller.
**/
VOID
SioCreateDevice (
  EFI_HANDLE                Controller,
  EFI_SIO_ACPI_DEVICE_ID    *Device,
  EFI_DEVICE_PATH_PROTOCOL  *ParentDevicePath,
  EFI_PCI_IO_PROTOCOL       *PciIo
  )
{
  EFI_STATUS                Status;
  SIO_DEV                   *SioDev;

  DeviceEnable (Device);
  SioDev = AllocateCopyPool (sizeof (SIO_DEV), &mSioDevTemplate);
  ASSERT (SioDev != NULL);
  InsertHeadList (&mSioDevPool, &SioDev->Link);

  SioDev->PciHandle       = Controller;

  CopyMem (&SioDev->Device, Device, sizeof (*Device));

  mAcpiNodeTemplate.HID = Device->HID;
  mAcpiNodeTemplate.UID = Device->UID;
  SioDev->DevicePath = AppendDevicePathNode (ParentDevicePath, (EFI_DEVICE_PATH_PROTOCOL *) &mAcpiNodeTemplate);
  ASSERT (SioDev->DevicePath != NULL);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &SioDev->Handle,
                  &gEfiSioProtocolGuid,        &SioDev->Sio,
                  &gEfiDevicePathProtocolGuid, SioDev->DevicePath,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  mSioDriver.DriverBindingHandle,
                  SioDev->Handle,
                  EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                  );
  ASSERT_EFI_ERROR (Status);
}

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
  )
{
  EFI_STATUS                          Status;
  EFI_PCI_IO_PROTOCOL                 *PciIo;
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL            *ParentDevicePath;
  EFI_SIO_ACPI_DEVICE_ID              *Devices;
  SIO_DEV                             *SioDev;
  UINTN                               Count;
  UINTN                               Index;
  ACPI_HID_DEVICE_PATH                *AcpiNode;
  BOOLEAN                             *HasCreated;
  BOOLEAN                             *RequestCreate;
  LIST_ENTRY                          *Node;

  
  //
  // Get the ISA bridge's Device Path
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &ParentDevicePath,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );
  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    return Status;
  }

  //
  // Get Pci IO
  //
  Status = gBS->OpenProtocol (
                  Controller,
                  &gEfiPciIoProtocolGuid,
                  (VOID **) &PciIo,
                  This->DriverBindingHandle,
                  Controller,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status) && Status != EFI_ALREADY_STARTED) {
    gBS->CloseProtocol (
           Controller,
           &gEfiDevicePathProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    return Status;
  }

  if ((RemainingDevicePath != NULL) && IsDevicePathEnd (RemainingDevicePath)) {
    return EFI_SUCCESS;
  }

  DeviceGetList (&Devices, &Count);
  if(Count == 0)
    return EFI_UNSUPPORTED;
  
  HasCreated    = AllocatePool (sizeof (BOOLEAN) * Count);
  RequestCreate = AllocatePool (sizeof (BOOLEAN) * Count);
  ASSERT ((HasCreated != NULL) && (RequestCreate != NULL));

  //
  // Assume no children has been created.
  // Assume the SIO interface hasn't been initialized.
  //
  ZeroMem (HasCreated, sizeof (BOOLEAN) * Count);

  if (Status == EFI_ALREADY_STARTED) {
    for (Node = GetFirstNode (&mSioDevPool);
         !IsNull (&mSioDevPool, Node);
         Node = GetNextNode (&mSioDevPool, Node)
        ) {
      SioDev = CR (Node, SIO_DEV, Link, SIO_DEV_SIGNATURE);
      Status = gBS->HandleProtocol (
                      SioDev->PciHandle,
                      &gEfiDevicePathProtocolGuid,
                      (VOID **) &DevicePath
                      );
      ASSERT_EFI_ERROR (Status);

      //
      // See if they are under the same PCI to ISA Bridge
      //
      if (CompareMem (DevicePath, ParentDevicePath, GetDevicePathSize (DevicePath)) == 0) {
        for (Index = 0; Index < Count; Index++) {
          if (CompareMem (&SioDev->Device, &Devices[Index], sizeof (EFI_SIO_ACPI_DEVICE_ID)) == 0) {
            HasCreated[Index] = TRUE;
            break;
          }
        }
      }
    }
  }

  AcpiNode = (ACPI_HID_DEVICE_PATH *) RemainingDevicePath;
  for (Index = 0; Index < Count; Index++) {
    if ((AcpiNode == NULL) ||
        ((AcpiNode->HID == Devices[Index].HID) && (AcpiNode->UID == Devices[Index].UID))
       ) {
      RequestCreate[Index] = TRUE;
    } else {
      RequestCreate[Index] = FALSE;
    }
  }

  for (Index = 0; Index < Count; Index++) {
    if ((RequestCreate[Index] && !HasCreated[Index])||(Devices[Index].HID == EISA_PNP_ID(0xF03))) {
      SioCreateDevice (Controller, &Devices[Index], ParentDevicePath, PciIo);
    }
  }

  FreePool (Devices);
  FreePool (HasCreated);
  FreePool (RequestCreate);

  return Status;
}


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
  )
{
  EFI_STATUS  Status;
  UINTN       Index;
  BOOLEAN     AllChildrenStopped;

  if (NumberOfChildren == 0) {
    gBS->CloseProtocol (
           Controller,
           &gEfiDevicePathProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    gBS->CloseProtocol (
           Controller,
           &gEfiPciIoProtocolGuid,
           This->DriverBindingHandle,
           Controller
           );
    return EFI_SUCCESS;
  }

  AllChildrenStopped = TRUE;
  for (Index = 0; Index < NumberOfChildren; Index++) {
    Status = SioDestroyDevice (ChildHandleBuffer[Index]);
    if (EFI_ERROR (Status)) {
      AllChildrenStopped = FALSE;
    }
  }

  if (AllChildrenStopped) {
    return EFI_SUCCESS;
  } else {
    return EFI_DEVICE_ERROR;
  }
}

