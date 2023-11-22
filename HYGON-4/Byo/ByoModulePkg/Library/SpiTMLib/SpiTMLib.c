/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SpiTMLib.c

Abstract:
  I/O library for Phytium AARCH64 TCG MmioRead/MmioWrite(Implement use SpiDeviceRead/SpiDeviceWrite).
  Non I/O read and write access (memory map I/O read and write only).

Revision History:

**/


#include <Library/TMIoLib.h>
#include <Library/SpiDeviceLib.h>
#include <Library/DebugLib.h>

UINT8
EFIAPI
TrustedModuleIoRead8 (
  IN      UINTN                     Address
  )
{
  UINT8                             Value;

  SpiDeviceRead (Address, 1, &Value);

  return Value;
}

/**
  Writes an 8-bit MMIO register.

  Writes the 8-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 8-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT8
EFIAPI
TrustedModuleIoWrite8 (
  IN      UINTN                     Address,
  IN      UINT8                     Value
  )
{

  SpiDeviceWrite (Address, 1, &Value);

  return Value;
}

/**
  Reads a 16-bit MMIO register.

  Reads the 16-bit MMIO register specified by Address. The 16-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT16
EFIAPI
TrustedModuleIoRead16 (
  IN      UINTN                     Address
  )
{
  UINT16                            Value;

  ASSERT ((Address & 1) == 0);

  SpiDeviceRead (Address, 2, &Value);

  return Value;
}

/**
  Writes a 16-bit MMIO register.

  Writes the 16-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 16-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT16
EFIAPI
TrustedModuleIoWrite16 (
  IN      UINTN                     Address,
  IN      UINT16                    Value
  )
{

  ASSERT ((Address & 1) == 0);

  SpiDeviceWrite (Address, 2, &Value);

  return Value;
}

/**
  Reads a 32-bit MMIO register.

  Reads the 32-bit MMIO register specified by Address. The 32-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT32
EFIAPI
TrustedModuleIoRead32 (
  IN      UINTN                     Address
  )
{
  UINT32                            Value;

  ASSERT ((Address & 3) == 0);

  SpiDeviceRead (Address, 4, &Value);

  return Value;
}

/**
  Writes a 32-bit MMIO register.

  Writes the 32-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 32-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT32
EFIAPI
TrustedModuleIoWrite32 (
  IN      UINTN                     Address,
  IN      UINT32                    Value
  )
{

  ASSERT ((Address & 3) == 0);

  SpiDeviceWrite (Address, 4, &Value);

  return Value;
}

/**
  Reads a 64-bit MMIO register.

  Reads the 64-bit MMIO register specified by Address. The 64-bit read value is
  returned. This function must guarantee that all MMIO read and write
  operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to read.

  @return The value read.

**/
UINT64
EFIAPI
TrustedModuleIoRead64 (
  IN      UINTN                     Address
  )
{
  UINT64                            Value;

  ASSERT ((Address & 7) == 0);

  SpiDeviceRead (Address, 8, &Value);

  return Value;
}

/**
  Writes a 64-bit MMIO register.

  Writes the 64-bit MMIO register specified by Address with the value specified
  by Value and returns Value. This function must guarantee that all MMIO read
  and write operations are serialized.

  If 64-bit MMIO register operations are not supported, then ASSERT().

  @param  Address The MMIO register to write.
  @param  Value   The value to write to the MMIO register.

**/
UINT64
EFIAPI
TrustedModuleIoWrite64 (
  IN      UINTN                     Address,
  IN      UINT64                    Value
  )
{

  ASSERT ((Address & 7) == 0);

  SpiDeviceWrite (Address, 8, &Value);

  return Value;
}

