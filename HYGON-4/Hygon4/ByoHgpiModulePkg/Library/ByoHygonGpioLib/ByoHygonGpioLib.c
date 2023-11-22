/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoHygonGpioLib.c

Abstract:
  Beep of platform.

Revision History:

**/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/SmnAccessLib.h>
#include <Fch.h>
#include <TSFch.h>
#include <Library/HygonSmnAddressLib.h>
#include <HygonCpmCommon.h>
#include <Library/HygonSocBaseLib.h>



VOID
LibCpmFchSetGpio (
  IN       UINT16                      Pin,
  IN       UINT16                      Value
  )
{
  HYGON_CPM_GPIO_SETTING     GpioPin;
  UINT8                      temp;

  GpioPin.Raw  = Value;

  if (Pin < 256) {
    temp = MmioRead8 ((UINT32)(ACPI_MMIO_BASE + GPIO_BANK0_BASE + (Pin << 2) + 2));
  } else {
    temp = MmioRead8 ((UINT32)(ACPI_MMIO_BASE + REMOTE_GPIO_BANK0_BASE + ((Pin-256) << 2) + 2));
  }

  if (GpioPin.Gpio.PresetEn) {
    temp = temp & 0xC7;
    if (GpioPin.Gpio.PullUpSel) {
      temp |= 0x08;
    }

    if (GpioPin.Gpio.PullUp) {
      temp |= 0x10;
    }

    if (GpioPin.Gpio.PullDown) {
      temp |= 0x20;
    }
  }

  if (GpioPin.Gpio.SetEnB == 0) {
    if (GpioPin.Gpio.OutEnB == 0) {
      temp |= 0x80;
      if (GpioPin.Gpio.Out) {
        temp |= 0x40;
      } else {
        temp &= 0xBF;
      }
    } else {
      temp &= 0x7F;
    }
  }

  if (Pin < 256) {
    MmioWrite8 ((UINT32)(ACPI_MMIO_BASE + GPIO_BANK0_BASE + (Pin << 2) + 2), temp);
  } else {
    MmioWrite8 ((UINT32)(ACPI_MMIO_BASE + REMOTE_GPIO_BANK0_BASE + ((Pin-256) << 2) + 2), temp);
  }

  if (GpioPin.Gpio.IoMuxEn) {
    MmioWrite8 ((UINT32)(ACPI_MMIO_BASE + IOMUX_BASE + ((Pin < 256) ? Pin : Pin -256 + 0xC0)), (UINT8)GpioPin.Gpio.IoMux);
  }
}


UINT8
EFIAPI
LibCpmSmnRead8 (
  IN       UINT16  Socket,
  IN       UINT32  SmnAddr,
  IN       UINT32  Offset
  )
{
  UINT8  Data;
  SmnRegisterReadBySocket ((UINT8)Socket, SmnAddr + Offset, AccessWidth8, &Data);
  return Data;
}


VOID
EFIAPI
LibCpmSmnWrite8 (
  IN       UINT16  Socket,
  IN       UINT32  SmnAddr,
  IN       UINT32  Offset,
  IN       UINT8   Data
  )
{
  SmnRegisterWriteBySocket ((UINT8)Socket, SmnAddr + Offset, AccessWidth8, &Data);
}



VOID
LibCpmSmnSetGpio (
  IN       UINT16                      Socket,
  IN       UINT16                      PhysicalDie,
  IN       UINT16                      Pin,
  IN       UINT16                      Value
  )
{
  HYGON_CPM_GPIO_SETTING     GpioPin;
  UINT32                     FchSmnGpioBase;
  UINT32                     FchSmnIomuxBase;
  UINT8                      temp;
  UINT32                     CpuModel;


  GpioPin.Raw       = Value;
  CpuModel          = GetHygonSocModel();

  if (CpuModel == HYGON_EX_CPU) {
    FchSmnGpioBase  = (Pin < 256) ? FCH_SMN_GPIO_BASE_HYEX : FCH_SMN_REMOTE_GPIO_BASE_HYEX;
    FchSmnIomuxBase = FCH_SMN_IOMUX_BASE_HYEX;
  } else if (CpuModel == HYGON_GX_CPU) {
    FchSmnGpioBase  = (Pin < 256) ? FCH_SMN_GPIO_BASE_HYGX : FCH_SMN_REMOTE_GPIO_BASE_HYGX;
    FchSmnIomuxBase = FCH_SMN_IOMUX_BASE_HYGX;
  }

  FchSmnGpioBase = IOD_SPACE(PhysicalDie, FchSmnGpioBase);
  temp = LibCpmSmnRead8(Socket, FchSmnGpioBase, (UINT32)(((Pin % 256) << 2) + 2));
  if (GpioPin.Gpio.PresetEn) {
    temp = temp & 0xC7;
    if (GpioPin.Gpio.PullUpSel) {
      temp |= 0x08;
    }

    if (GpioPin.Gpio.PullUp) {
      temp |= 0x10;
    }

    if (GpioPin.Gpio.PullDown) {
      temp |= 0x20;
    }
  }

  if (GpioPin.Gpio.SetEnB == 0) {
    if (GpioPin.Gpio.OutEnB == 0) {
      temp |= 0x80;
      if (GpioPin.Gpio.Out) {
        temp |= 0x40;
      } else {
        temp &= 0xBF;
      }
    } else {
      temp &= 0x7F;
    }
  }

  LibCpmSmnWrite8 (Socket, FchSmnGpioBase, (UINT32)(((Pin % 256) << 2) + 2), temp);

  if (GpioPin.Gpio.IoMuxEn) {
    FchSmnIomuxBase = IOD_SPACE(PhysicalDie, FchSmnIomuxBase);
    LibCpmSmnWrite8 (Socket, FchSmnIomuxBase, ((Pin < 256) ? Pin : (Pin-256 + 0xC0)), (UINT8)GpioPin.Gpio.IoMux);
  }
}




VOID
LibCpmSetGpio (
  IN       UINT16                      Pin,
  IN       UINT16                      Value
  )
{
  UINT16  Select;
  UINT16  PinNum;

  Select = Pin >> 9;
  PinNum = Pin & 0x1FF;

  switch (Select) {
    case 0:
      LibCpmFchSetGpio (PinNum, Value);
      break;
      
    default:
      LibCpmSmnSetGpio ((UINT16)((Pin & 0xE000) >> 13), (UINT16)((Pin & 0x1800) >> 11), PinNum, Value);
      break;
  }
}






UINT8
LibCpmFchGetGpio (
  IN       UINT16    Pin
  )
{
  UINT8                      Value;

  if (Pin < 256) {
    Value = MmioRead8 ((UINT32)(ACPI_MMIO_BASE + GPIO_BANK0_BASE + (Pin << 2) + 2)) & 0x01;
  } else {
    Value = MmioRead8 ((UINT32)(ACPI_MMIO_BASE + REMOTE_GPIO_BANK0_BASE + ((Pin-256) << 2) + 2)) & 0x01;
  }

  return Value;
}


UINT8
LibCpmSmnGetGpio (
  IN       UINT16                      Socket,
  IN       UINT16                      PhysicalDie,
  IN       UINT16                      Pin
  )
{
  UINT8                      Value;
  UINT32                     FchSmnGpioBase;
  UINT32                     CpuModel;

  Value             = 0;
  CpuModel          = GetHygonSocModel();

  if (CpuModel == HYGON_EX_CPU) {
    FchSmnGpioBase = (Pin < 256) ? FCH_SMN_GPIO_BASE_HYEX : FCH_SMN_REMOTE_GPIO_BASE_HYEX;
  } else if (CpuModel == HYGON_GX_CPU) {
    FchSmnGpioBase = (Pin < 256) ? FCH_SMN_GPIO_BASE_HYGX : FCH_SMN_REMOTE_GPIO_BASE_HYGX;
  }

  FchSmnGpioBase = IOD_SPACE(PhysicalDie, FchSmnGpioBase);
  Value = LibCpmSmnRead8(Socket, FchSmnGpioBase, (UINT32)(((Pin % 256) << 2) + 2)) & 0x01;
  return Value;
}


UINT8
LibCpmGetGpio (
  IN       UINT16   Pin
  )
{
  UINT16  Select;
  UINT16  PinNum;
  UINT8   Value;

  Select = Pin >> 9;
  PinNum = Pin & 0x1FF;
  
  switch (Select) {
    
    case 0:
      Value = LibCpmFchGetGpio(PinNum);
      break;
      
    default:
      Value = LibCpmSmnGetGpio((UINT16)((Pin & 0xE000) >> 13), (UINT16)((Pin & 0x1800) >> 11), PinNum);
      break;
  }

  return Value;
}



