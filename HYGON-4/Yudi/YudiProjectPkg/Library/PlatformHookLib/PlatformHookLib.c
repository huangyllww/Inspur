/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By:
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include <Uefi.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/ByoHygonCommLib.h>


#define TKN_UART_SELECTION_CPU_UART0      0
#define TKN_UART_SELECTION_BMC_UART1      1
#define TKN_UART_SELECTION_CPU_UART1      2


EFI_STATUS
EFIAPI
SocSerialPortInitialize (
  UINT8  UartIndex
  );


#define SIO_Index_Port 0x2E
#define SIO_Data_Port  (SIO_Index_Port + 1)


EFI_STATUS Bmc2500Uart1Init()
{
  // ;Enter config mode by writting 0xA5 to index port twice
  IoWrite8 (SIO_Index_Port, 0xA5);
  IoWrite8 (SIO_Index_Port, 0xA5);

  //;Select LDN=2,uart1
  IoWrite8 (SIO_Index_Port, 0x07);
  IoWrite8 (SIO_Data_Port, 0x02);

  //;Set base address to 3F8h
  IoWrite8 (SIO_Index_Port, 0x60);
  IoWrite8 (SIO_Data_Port, 0x03);
  IoWrite8 (SIO_Index_Port, 0x61);
  IoWrite8 (SIO_Data_Port, 0xF8);

  //;Set IRQ=04h
  IoWrite8 (SIO_Index_Port, 0x70);
  IoWrite8 (SIO_Data_Port, 0x04);

  //;Enable UART1
  IoWrite8 (SIO_Index_Port, 0x30);
  IoWrite8 (SIO_Data_Port, 0x01);

  //;Exit config mode by writing 0xAA to Index Port.
  IoWrite8 (SIO_Index_Port, 0xAA);

  return EFI_SUCCESS;
}




RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  switch(FixedPcdGet8(PcdUartSelection)){
    case TKN_UART_SELECTION_CPU_UART0:
      return LibSocSerialPortInitialize(0);
      
    case TKN_UART_SELECTION_CPU_UART1:
      return LibSocSerialPortInitialize(1);

    case TKN_UART_SELECTION_BMC_UART1:
      LibEnableLpcSioUart0Decode();
      return Bmc2500Uart1Init();

    default:
      return EFI_UNSUPPORTED;
  }
}


