/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SuperIoLib.h

Abstract:

Revision History:

**/
#ifndef _ITE_SUPER_IO_LIB_H_
#define _ITE_SUPER_IO_LIB_H_

typedef struct {
  UINT8 Register;
  UINT8 DataMask;
  UINT8 Value;
} EFI_SIO_TABLE;

/**
  Program the SIO chip to enter the configure mode.
**/
VOID
EnterSioCfgMode (
  VOID
  );

/**
  Program the SIO chip to exit the configure mode.
**/
VOID
ExitSioCfgMode (
  VOID
  );


/**
  Perform a 8-bit I/O read from SIO register.

  @param[in]  Index  The register index.

  @return The value written to the register.
**/
UINT8
ReadSioReg (
  IN  UINT8            Index
  );

/**
  Perform a 8-bit I/O write to SIO register.

  @param[in]  Index  The register index.
  @param[in]  Data   The value to write to register.
**/
VOID
WriteSioReg (
  IN  UINT8            Index,
  IN  UINT8            Data
  );

UINT16
GetSioChipId(
   VOID
);

VOID
SioRegAndThenOr (
  IN  UINT8            Index,
  IN  UINT8            AndValue,
  IN  UINT8            OrValue
  );

VOID 
HwmRegAndThenOr (
  IN  UINT8            Index,
  IN  UINT8            AndValue,
  IN  UINT8            OrValue
  );

UINT8 ReadHwmReg(UINT8 Index);
VOID WriteHwmReg(UINT8 Index, UINT8 Value);

VOID 
HwmRegAndThenOrEx (
  IN  UINT8            Page,
  IN  UINT8            Index,
  IN  UINT8            AndValue,
  IN  UINT8            OrValue
  );

UINT8 ReadHwmRegEx(
  IN  UINT8            Page,
  IN  UINT8            Index
  );
VOID WriteHwmRegEx(
  IN  UINT8            Page,
  IN  UINT8            Index, 
  IN  UINT8            Value
  );
VOID EnableUart1(UINT16 IoBase, UINT8 Irq);
VOID EnableUart2(UINT16 IoBase, UINT8 Irq);
VOID EnableLpt(UINT16 IoBase, BOOLEAN Port80En, UINT8 LptMode);

#define LPT_MODE_ECP_EPP       0
#define LPT_MODE_ECP           1
#define LPT_MODE_EPP           2

#define LED_STATUS_ON       0
#define LED_STATUS_OFF      1
#define LED_STATUS_BLINK    2

#ifndef PCI_DEV_MMBASE
#define PCI_DEV_MMBASE(Bus, Device, Function) \
    ( \
      (UINTN)PcdGet64(PcdPciExpressBaseAddress) + (UINTN) (Bus << 20) + (UINTN) (Device << 15) + (UINTN) \
        (Function << 12) \
    )
#endif

#define LPC_PCI_REG(Reg)          (PCI_DEV_MMBASE(0, 17, 0)+Reg)

VOID LibSetPowerLed(UINT8 Type);

#endif

