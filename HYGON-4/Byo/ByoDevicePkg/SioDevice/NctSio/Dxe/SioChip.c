
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

//
// COM 1 UART Controller
//
ACPI_SIO_RESOURCES_IO_IRQ      mCom1Resources = {
  {
    {ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR},
    0x3f8,
    8
  },
  {
    {ACPI_IRQ_NOFLAG_DESCRIPTOR},
    BIT4    // IRQ4
  },
  {
    ACPI_END_TAG_DESCRIPTOR,
    0
  }
};

//
// COM 2 UART Controller
//
ACPI_SIO_RESOURCES_IO_IRQ      mCom2Resources = {
  {
    {ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR},
    0x2f8,
    8
  },
  {
    {ACPI_IRQ_NOFLAG_DESCRIPTOR},
    BIT3    // IRQ3
  },
  {
    ACPI_END_TAG_DESCRIPTOR,
    0
  }
};

//
// LPT 1 Controller
//
ACPI_SIO_RESOURCES_IO_IRQ_DMA      mLpt1Resources = {
  {
    {ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR},
    0x378,
    8
  },
  {
    {ACPI_IRQ_NOFLAG_DESCRIPTOR},
    BIT7
  },
  {
    {ACPI_DMA_DESCRIPTOR},
    BIT3,
    BIT3
  },
  {
    ACPI_END_TAG_DESCRIPTOR,
    0
  }
};

//
// PS/2 Keyboard Controller
//
ACPI_SIO_RESOURCES_IO_IRQ      mKeyboardResources = {
  {
    {ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR},
    0x60,
    5
  },
  {
    {ACPI_IRQ_NOFLAG_DESCRIPTOR},
    BIT1
  },
  {
    ACPI_END_TAG_DESCRIPTOR,
    0
  }
};

//
// PS/2 Mouse Controller
//
ACPI_SIO_RESOURCES_IO_IRQ      mMouseResources = {
  {
    {ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR},
    0x60,
    5
  },
  {
    {ACPI_IRQ_NOFLAG_DESCRIPTOR},
    BIT12
  },
  {
    ACPI_END_TAG_DESCRIPTOR,
    0
  }
};
#pragma pack(1)
//
// Table of SIO Controllers
//
DEVICE_INFO    mDeviceInfo[] = {

// 0
  {
    {
      EISA_PNP_ID(0x501),
      0
    },
    SIO_UART_A_DEV_NUM,
    RESOURCE_IO | RESOURCE_IRQ,
    {(ACPI_SMALL_RESOURCE_HEADER *) &mCom1Resources},
    {(ACPI_SMALL_RESOURCE_HEADER *) &mCom1Resources}
  },  // COM 1 UART Controller
// 1
  {
    {
      EISA_PNP_ID(0x501),
      1
    },
    SIO_UART_B_DEV_NUM,
    RESOURCE_IO | RESOURCE_IRQ,
    {(ACPI_SMALL_RESOURCE_HEADER *) &mCom2Resources},
    {(ACPI_SMALL_RESOURCE_HEADER *) &mCom2Resources}
  },  // COM 2 UART Controller

// 2
  {
    {
      EISA_PNP_ID(0x401),
      0
    },
    SIO_PARALLEL_DEV_NUM,
    RESOURCE_IO | RESOURCE_IRQ | RESOURCE_DMA,
    {(ACPI_SMALL_RESOURCE_HEADER *) &mLpt1Resources},
    {(ACPI_SMALL_RESOURCE_HEADER *) &mLpt1Resources}
  },  // LPT 1 Controller

// 3
  {
    {
      EISA_PNP_ID(0x303),
      0
    },
    SIO_KBC_DEV_NUM,
    RESOURCE_IRQ,
    {(ACPI_SMALL_RESOURCE_HEADER *) &mKeyboardResources},
    {(ACPI_SMALL_RESOURCE_HEADER *) &mKeyboardResources}
  },  // PS/2 Keyboard Controller

// 4
  {
    {
      EISA_PNP_ID(0xF03),
      0
    },
    SIO_KBC_DEV_NUM,
    RESOURCE_IRQ,
    {(ACPI_SMALL_RESOURCE_HEADER *) &mMouseResources},
    {(ACPI_SMALL_RESOURCE_HEADER *) &mMouseResources}
  }   // PS/2 Mouse Controller

};




//*************************************************************************//

EFI_SIO_TABLE mSioSerialPortConfigTable[] = {
  { R_SIO_BASE_ADDRESS_HIGH, 0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW, 0xff, 0x00 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x00 },
  { R_SIO_ACTIVATE, 0xff, 0x00 },
};

UINT8 mSerialPort1ConfigValue[5][4] = {
  { 0x00, 0x00, 0x00, 0x00 },
  { 0x03, 0xF8, 0x04, 0x01 },
  { 0x02, 0xF8, 0x03, 0x01 },
  { 0x03, 0xE8, 0x04, 0x01 },
  { 0x02, 0xE8, 0x03, 0x01 },
};

EFI_SIO_TABLE mSioParallelPortConfigTable[] = {
  { R_SIO_BASE_ADDRESS_HIGH, 0xff, 0x00 },
  { R_SIO_BASE_ADDRESS_LOW, 0xff, 0x00 },
  { R_SIO_PRIMARY_INTERRUPT_SELECT, 0xff, 0x00 },
  { R_SIO_DMA_CHANNEL_SELECT, 0xff,0x00 },
  { R_SIO_PARALLEL_PORT_SPECIAL, 0xff,0x00 },
  { R_SIO_ACTIVATE, 0xff, 0x00 },
};

UINT16 mSioLptIoValueTable[] = {
  // Disabled
  0x0000,
  // IO=378 (Default)
  0x0378,
  // IO=278
  0x0278,
  // IO=3BC
  0x03BC
};

UINT8 mSioLptModeValueTable[] = {
  // EPP & ECP (Default)
  0x0B,
  // ECP
  0x0A,
  // EPP
  0x09
};

UINT8 mSioLptDmaChannelValueTable[] = {
  // DMA 3 (Default)
  0x03,
  // DMA 1
  0x01
};

UINT8 mSioLptIrqValueTable[] = {
  // IRQ=7(Default)
  0x07,
  // IRQ=5
  0x05
};
#pragma pack()


//*************************************************************************//
/**
  Find the DEVICE_INFO for specified Device.

  @param[in] Device   Pointer to the EFI_SIO_ACPI_DEVICE_ID.

  @return DEVICE_INFO*  Pointer to the DEVICE_INFO.
**/
DEVICE_INFO *
DeviceSearch (
  IN EFI_SIO_ACPI_DEVICE_ID *Device
  )
{
  UINTN       Index;

  for (Index = 0; Index < sizeof (mDeviceInfo) / sizeof (mDeviceInfo[0]); Index++) {
    if (CompareMem (Device, &mDeviceInfo[Index].Device, sizeof (EFI_SIO_ACPI_DEVICE_ID)) == 0) {
      return &mDeviceInfo[Index];
    }
  }

  ASSERT (FALSE);
  return NULL;
}

/**
  Program the SIO chip to select the specified device.

  @param[in] Device    Pointer to EFI_SIO_ACPI_DEVICE_ID.

  @return              Pointer to DEVICE_INFO.
**/
DEVICE_INFO *
DeviceSelect (
  IN EFI_SIO_ACPI_DEVICE_ID *Device
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  WriteSioReg(R_SIO_LOGICAL_DEVICE, DeviceInfo->DeviceId);
  return DeviceInfo;
}

/**
  Program the SIO chip to set the IO base address of the specified device.

  @param[in] Device      Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[in] BaseAddress The base address to set.
**/
VOID
DeviceSetIo (
  IN EFI_SIO_ACPI_DEVICE_ID *Device,
  IN UINT16                 BaseAddress
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  if ((DeviceInfo->ResourceMask & RESOURCE_IO) == RESOURCE_IO) {
    WriteSioReg(R_SIO_BASE_ADDRESS_HIGH, (UINT8)(BaseAddress >> 8));
    WriteSioReg(R_SIO_BASE_ADDRESS_LOW,  (UINT8) BaseAddress);
  }
}

/**
  Program the SIO chip to set the IRQ of the specified device.

  @param[in] Device      Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[in] Irq         The IRQ to set.
**/
VOID
DeviceSetIrq (
  IN EFI_SIO_ACPI_DEVICE_ID *Device,
  IN UINT8                  Irq
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  if ((DeviceInfo->ResourceMask & RESOURCE_IRQ) == RESOURCE_IRQ) {
    WriteSioReg(R_SIO_PRIMARY_INTERRUPT_SELECT, Irq);
  }
}

/**
  Program the SIO chip to set the DMA of the specified device.

  @param[in] Device      Pointer to EFI_SIO_ACPI_DEVICE_ID.
  @param[in] Dma         The DMA to set.
**/
VOID
DeviceSetDma (
  IN EFI_SIO_ACPI_DEVICE_ID *Device,
  IN UINT8                  Dma
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  if ((DeviceInfo->ResourceMask & RESOURCE_DMA) == RESOURCE_DMA) {
    WriteSioReg(R_SIO_DMA_CHANNEL_SELECT, Dma);
  }
}

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
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  *Resources = DeviceInfo->Resources;

  return EFI_SUCCESS;
}

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
  )
{
  DEVICE_INFO                                 *DeviceInfo;
  ACPI_RESOURCE_HEADER_PTR                    ResourcePtr;
  ACPI_RESOURCE_HEADER_PTR                    ResourcePtr2;
  EFI_ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR  *FixedIo;
  EFI_ACPI_IRQ_DESCRIPTOR                     *Irq;
  EFI_ACPI_DMA_DESCRIPTOR                     *Dma;

  ResourcePtr = Resources;
  DeviceInfo  = DeviceSearch (Device);

  EnterSioCfgMode();
  DeviceSelect (Device);
  while (ResourcePtr.SmallHeader->Byte != ACPI_END_TAG_DESCRIPTOR) {

    switch (ResourcePtr.SmallHeader->Byte) {
      case ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR:
        FixedIo = (EFI_ACPI_FIXED_LOCATION_IO_PORT_DESCRIPTOR *) ResourcePtr.SmallHeader;
        DeviceSetIo (Device, FixedIo->BaseAddress);
        break;

      case ACPI_IRQ_NOFLAG_DESCRIPTOR:
      case ACPI_IRQ_DESCRIPTOR:
        Irq = (EFI_ACPI_IRQ_DESCRIPTOR *) ResourcePtr.SmallHeader;
        DeviceSetIrq (Device, (UINT8) LowBitSet32 ((UINT32) Irq->Mask));
        break;

      case ACPI_DMA_DESCRIPTOR:
        Dma = (EFI_ACPI_DMA_DESCRIPTOR *) ResourcePtr.SmallHeader;
        DeviceSetDma (Device, (UINT8) LowBitSet32 ((UINT32) Dma->ChannelMask));
        break;
    }

    //
    // Update the current resource
    //
    ResourcePtr2 = DeviceInfo->Resources;
    while (ResourcePtr2.SmallHeader->Byte != ACPI_END_TAG_DESCRIPTOR) {
      if (ResourcePtr2.SmallHeader->Byte == ResourcePtr.SmallHeader->Byte) {
        if (ResourcePtr2.SmallHeader->Bits.Type == 0) {
          CopyMem (
            ResourcePtr2.SmallHeader,
            ResourcePtr.SmallHeader,
            ResourcePtr.SmallHeader->Bits.Length + sizeof (*ResourcePtr.SmallHeader)
            );
        } else {
          CopyMem (
            ResourcePtr2.LargeHeader,
            ResourcePtr.LargeHeader,
            ResourcePtr.LargeHeader->Length + sizeof (*ResourcePtr.LargeHeader)
            );
        }
      }

      if (ResourcePtr2.SmallHeader->Bits.Type == 0) {
        ResourcePtr2.SmallHeader = (ACPI_SMALL_RESOURCE_HEADER *) ((UINT8 *) ResourcePtr2.SmallHeader
                                + ResourcePtr2.SmallHeader->Bits.Length
                                + sizeof (*ResourcePtr2.SmallHeader));
      } else {
        ResourcePtr2.LargeHeader = (ACPI_LARGE_RESOURCE_HEADER *) ((UINT8 *) ResourcePtr2.LargeHeader
                                + ResourcePtr2.LargeHeader->Length
                                + sizeof (*ResourcePtr2.LargeHeader));
      }

      break;
    }
    //
    // Should find a match
    //
    ASSERT (ResourcePtr2.SmallHeader->Byte != ACPI_END_TAG_DESCRIPTOR);


    if (ResourcePtr.SmallHeader->Bits.Type == 0) {
      ResourcePtr.SmallHeader = (ACPI_SMALL_RESOURCE_HEADER *) ((UINT8 *) ResourcePtr.SmallHeader
                              + ResourcePtr.SmallHeader->Bits.Length
                              + sizeof (*ResourcePtr.SmallHeader));
    } else {
      ResourcePtr.LargeHeader = (ACPI_LARGE_RESOURCE_HEADER *) ((UINT8 *) ResourcePtr.LargeHeader
                              + ResourcePtr.LargeHeader->Length
                              + sizeof (*ResourcePtr.LargeHeader));
    }
  }

  ExitSioCfgMode();

  return EFI_SUCCESS;
}

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
  )
{
  DEVICE_INFO               *DeviceInfo;

  DeviceInfo = DeviceSearch (Device);

  *Resources = DeviceInfo->PossibleResources;

  return EFI_SUCCESS;
}
/**
  Program the SIO chip to enable the specified device using the default resource.

  @param[in] Device          Pointer to EFI_SIO_ACPI_DEVICE_ID.
**/
VOID
DeviceEnable (
  IN EFI_SIO_ACPI_DEVICE_ID   *Device
  )
{
  ACPI_RESOURCE_HEADER_PTR    Resources;

  DeviceGetResources (Device, &Resources);
  DeviceSetResources (Device, Resources);

  EnterSioCfgMode();
  DeviceSelect (Device);
  WriteSioReg (R_SIO_ACTIVATE, 1);
  ExitSioCfgMode();
}

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
  )
{
  EFI_SIO_ACPI_DEVICE_ID     *LocalDevices;
  UINTN                      LocalCount;
  UINTN                      Index;
  EFI_STATUS                 Status;
  UINTN                      VarSize;
  SIO_SETUP_CONFIG           SioConfig;

  VarSize = sizeof(SIO_SETUP_CONFIG);
  ZeroMem(&SioConfig, VarSize);
  Status = gRT->GetVariable (
                  SIO_SETUP_VARIABLE_NAME,
                  &gSioSetupConfigGuid,
                  NULL,
                  &VarSize,
                  &SioConfig
                  );
  //
  // Allocate enough memory for simplicity
  //
  
  LocalDevices = AllocatePool (sizeof (EFI_SIO_ACPI_DEVICE_ID) * 5);
  LocalCount   = 0;
  for(Index=0; Index< sizeof (mDeviceInfo) / sizeof (mDeviceInfo[0]) ; Index++){
    if(mDeviceInfo[Index].DeviceId == SIO_UART_A_DEV_NUM && (!SioConfig.UartAEn)){
       continue;
    }
    if(mDeviceInfo[Index].DeviceId == SIO_UART_B_DEV_NUM && (!SioConfig.UartBEn)){
       continue;
    }
    CopyMem (&LocalDevices[LocalCount], &mDeviceInfo[Index].Device, sizeof (EFI_SIO_ACPI_DEVICE_ID));
    LocalCount ++;
  }

  *Devices = LocalDevices;
  *Count   = LocalCount;
  
}

