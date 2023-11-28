//# @file/
//#
//# Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
//# All rights reserved.This software and associated documentation (if any)
//# is furnished under a license and may only be used or copied in
//# accordance with the terms of the license. Except as permitted by such
//# license, no part of this software or documentation may be reproduced,
//# stored in a retrieval system, or transmitted in any form or by any
//# means without the express written consent of Byosoft Corporation.
//#
//# File Name:
//#   TscTimerLib.c
//#
//# Abstract:
//#   CPU Timer File
//#
//# Revision History:
//#
//##
//**********************************************************************
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Fch.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>
#include <PlatformDefinition.h>



#define R_UART_RXBUF          0
#define R_UART_TXBUF          0
#define R_UART_BAUD_LOW       0
#define R_UART_BAUD_HIGH      1
#define R_UART_FCR            2
#define   B_UART_FCR_FIFOE    BIT0
#define   B_UART_FCR_FIFO64   BIT5
#define R_UART_LCR            3
#define   B_UART_LCR_DLAB     BIT7
#define R_UART_MCR            4
#define   B_UART_MCR_DTRC     BIT0
#define   B_UART_MCR_RTS      BIT1
#define R_UART_LSR            5
#define   B_UART_LSR_RXRDY    BIT0
#define   B_UART_LSR_TXRDY    BIT5
#define   B_UART_LSR_TEMT     BIT6
#define R_UART_MSR            6
#define   B_UART_MSR_CTS      BIT4
#define   B_UART_MSR_DSR      BIT5
#define   B_UART_MSR_RI       BIT6
#define   B_UART_MSR_DCD      BIT7

#define FCH_UART_DEBUG_SELECT   1
#define FCH_CHK_SERIAL_CABLE    1

#define FCH_UART_RXBUF          0
#define FCH_UART_TXBUF          0
#define FCH_UART_BAUD_LOW       0
#define FCH_UART_BAUD_HIGH      1
#define FCH_UART_FCR            2
#define   FCH_UART_FCR_FIFOE    BIT0
#define   FCH_UART_FCR_FIFO64   BIT5
#define FCH_UART_LCR            3
#define   FCH_UART_LCR_DLAB     BIT7
#define FCH_UART_MCR            4
#define   FCH_UART_MCR_DSR      BIT0
#define   FCH_UART_MCR_RTS      BIT1
#define FCH_UART_LSR            5
#define   FCH_UART_LSR_RXRDY    BIT0
#define   FCH_UART_LSR_TXRDY    BIT5
#define   FCH_UART_LSR_TEMT     BIT6
#define FCH_UART_MSR            6
#define   FCH_UART_MSR_CTS      BIT4
#define   FCH_UART_MSR_DSR      BIT5

#define FCH_SRR_OFFSET              0x88

#define FchSerialLineControl               0x3 
#define FchSerialFifoControl               0x7 

/**
  Read an 8-bit 16550 register.  If PcdSerialUseMmio is TRUE, then the value is read from 
  MMIO space.  If PcdSerialUseMmio is FALSE, then the value is read from I/O space.  The
  parameter Offset is added to the base address of the 16550 registers that is specified 
  by PcdSerialRegisterBase. 
  
  @param  Offset  The offset of the 16550 register to read.

  @return The value read from the 16550 register.

**/
UINT8
SocSerialPortReadRegister (
  UINTN  Offset
  )
{
//UART_0 0xFEDC_9xxx
//UART_1 0xFEDC_Axxx
//UART_2 0xFEDC_Exxx
//UART_3 0xFEDC_Fxxx
  return MmioRead8 ((UINTN)PcdGet64(PcdAmdIdsDebugPrintSerialPort) + Offset*4);
}


/**
  Write an 8-bit 16550 register.  If PcdSerialUseMmio is TRUE, then the value is written to
  MMIO space.  If PcdSerialUseMmio is FALSE, then the value is written to I/O space.  The
  parameter Offset is added to the base address of the 16550 registers that is specified 
  by PcdSerialRegisterBase. 
  
  @param  Offset  The offset of the 16550 register to write.
  @param  Value   The value to write to the 16550 register specified by Offset.

  @return The value written to the 16550 register.

**/

UINT8
SocSerialPortWriteRegister (
  UINTN  Offset,
  UINT8  Value
  )
{
  return MmioWrite8 ((UINTN)PcdGet64(PcdAmdIdsDebugPrintSerialPort) + Offset*4, Value);
}





/**
  Initialize the serial device hardware.
  
  If no initialization is required, then return RETURN_SUCCESS.
  If the serial device was successfully initialized, then return RETURN_SUCCESS.
  If the serial device could not be initialized, then return RETURN_DEVICE_ERROR.
  
  @retval RETURN_SUCCESS        The serial device was initialized.
  @retval RETURN_DEVICE_ERROR   The serial device could not be initialized.

**/
EFI_STATUS
EFIAPI
SocSerialPortInitialize (
  VOID
  )
{
//default use BAUD rate 115200
  MmioWrite8 ((UINTN)PcdGet64(PcdAmdIdsDebugPrintSerialPort)+FCH_SRR_OFFSET, 0x01); //reset UART0
  MicroSecondDelay(100); // Delay for 100 us to avoid garbage in serial output.

#if _PCD_VALUE_PcdAmdIdsDebugPrintSerialPort == 0xfedc9000
  MmioOr32(SMBUS_PCI_REG(0xFC), BIT28);   // Uart0 ClockRate:1843200
#elif _PCD_VALUE_PcdAmdIdsDebugPrintSerialPort == 0xfedca000
  MmioOr32(SMBUS_PCI_REG(0xFC), BIT29);   // Uart1 ClockRate:1843200
#endif
  
  SocSerialPortWriteRegister (FCH_UART_LCR, FCH_UART_LCR_DLAB);
  SocSerialPortWriteRegister (FCH_UART_BAUD_HIGH, 00);
  SocSerialPortWriteRegister (FCH_UART_BAUD_LOW, 0x1a); //set 115200
  SocSerialPortWriteRegister (FCH_UART_LCR, (UINT8)(FchSerialLineControl & 0x3F));     // 8 bit
  SocSerialPortWriteRegister (FCH_UART_FCR, (UINT8)(FchSerialFifoControl & 0x27));
  SocSerialPortWriteRegister (FCH_UART_MCR, FCH_UART_MCR_DSR|FCH_UART_MCR_RTS);  // Data Terminal Ready, Request to Send
  
//UART0_CTS_L_UART2_RXD_EGPIO135   def:UART0_CTS_L
//UART0_RXD_EGPIO136               def:UART0_RXD
//UART0_RTS_L_UART2_TXD_EGPIO137   def:UART2_TXD
//UART0_TXD_EGPIO138               def:EGPIO138
//UART0_INTR_AGPIO139              def:UART0_INTR    

#if _PCD_VALUE_PcdAmdIdsDebugPrintSerialPort == 0xfedc9000
  MmioWrite8 (FCH_IOMUXx89_UART0_RTS_L_EGPIO137, 0x00);
  MmioWrite8 (FCH_IOMUXx8A_UART0_TXD_EGPIO138, 0x00);
#elif _PCD_VALUE_PcdAmdIdsDebugPrintSerialPort == 0xfedca000
  MmioWrite8 (FCH_IOMUXx8E_UART1_RTS_L_EGPIO142, 0x00);
  MmioWrite8 (FCH_IOMUXx8F_UART1_TXD_EGPIO143, 0x00);
#endif
 
   
// [0] 2e8-2ef, 0 - disable, 1 - enable
// [1] 2f8-2ff, 0 - disable, 1 - enable
// [2] 3e8-3ff, 0 - disable, 1 - enable
// [3] 3f8-3ff, 0 - disable, 1 - enable
// [09:08] which uart uses 2E8-2EF,  0 - UART0, 1 - UART1, 2 - UART2, 3 - UART3
// [11:10]                 2F8-2FF
// [13:12]                 3E8-3EF
// [15:14]                 3F8-3FF   


#if _PCD_VALUE_PcdAmdIdsDebugPrintSerialPort == 0xfedc9000
  MmioWrite16(FCH_AL2AHBx20_LEGACY_UART_IO_ENABLE, BIT3);
#elif _PCD_VALUE_PcdAmdIdsDebugPrintSerialPort == 0xfedca000
  MmioWrite16(FCH_AL2AHBx20_LEGACY_UART_IO_ENABLE, BIT3|BIT14);
#endif
  
  return RETURN_SUCCESS;
}


