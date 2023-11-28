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

/** @file SerialPortLib.c
    This file contains SerialPortLib library instance

**/

//**********************************************************************
#include <Base.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/SerialPortLib.h>
#include <token.h>
#include <Fch.h>
#include <Library/TimerLib.h>
#include <Library/DebugLib.h>

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

/**
  ## Line Control Register (LCR) for the 16550 serial port.  This encodes data bits, parity, and stop bits.  
  #    BIT1..BIT0 - Data bits.  00b = 5 bits, 01b = 6 bits, 10b = 7 bits, 11b = 8 bits
  #    BIT2       - Stop Bits.  0 = 1 stop bit.  1 = 1.5 stop bits if 5 data bits selected, otherwise 2 stop bits.
  #    BIT5..BIT2 - Parity.  xx0b = No Parity, 001b = Odd Parity, 011b = Even Parity, 101b = Mark Parity, 111b=Stick Parity
  #    BIT7..BIT6 - Reserved.  Must be 0.
  #
  # Default is No Parity, 8 Data Bits, 1 Stop Bit.
  **/

#define FchSerialLineControl               0x3 
/**
   FIFO Control Register (FCR) for the 16550 serial port.   
  #    BIT0       - FIFO Enable.  0 = Disable FIFOs.  1 = Enable FIFOs.
  #    BIT1       - Clear receive FIFO.  1 = Clear FIFO.
  #    BIT2       - Clear transmit FIFO.  1 = Clear FIFO.
  #    BIT4..BIT3 - Reserved.  Must be 0.
  #    BIT5       - Enable 64-byte FIFO.  0 = Disable 64-byte FIFO.  1 = Enable 64-byte FIFO
  #    BIT7..BIT6 - Reserved.  Must be 0.
  #
  # Default is to enable and clear all FIFOs.
**/

#define FchSerialFifoControl               0x7 

/**
  ## If TRUE, then the 16550 serial port hardware flow control is enabled.
  #  If FALSE, then the 16550 serial port hardware flow control is disabled.  Default value.
**/
#define FchSerialUseHardwareFlowControl       FALSE 

/**
  ## If TRUE, then 16550 serial Tx operations will block if DSR is not asserted (no cable).
  #  If FALSE, then the 16550 serial Tx operations will not be blocked if DSR is not asserted. Default value.
  #  This define is ignored if FchSerialUseHardwareFlowControl is FALSE.
**/

#define FchSerialDetectCable       FALSE 



//UART_0 0xFEDC_9xxx
//UART_1 0xFEDC_Axxx
//UART_2 0xFEDC_Exxx
//UART_3 0xFEDC_Fxxx

UINTN
GetSbUartBase (VOID)
{
  return (UINTN)PcdGet64(PcdAmdIdsDebugPrintSerialPort);
}


/**
  Read an 8-bit 16550 register.  If PcdSerialUseMmio is TRUE, then the value is read from 
  MMIO space.  If PcdSerialUseMmio is FALSE, then the value is read from I/O space.  The
  parameter Offset is added to the base address of the 16550 registers that is specified 
  by PcdSerialRegisterBase. 
  
  @param  Offset  The offset of the 16550 register to read.

  @return The value read from the 16550 register.

**/
UINT8
SerialPortReadRegister (
  UINTN  Offset
  )
{
  return MmioRead8 (GetSbUartBase()+ Offset*4);
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
SerialPortWriteRegister (
  UINTN  Offset,
  UINT8  Value
  )
{
  return MmioWrite8 (GetSbUartBase()+ Offset*4, Value);
}




/**
  Return whether the hardware flow control signal allows writing.

  @retval TRUE  The serial port is writable.
  @retval FALSE The serial port is not writable.
**/
BOOLEAN
SerialPortWritable (
  VOID
  )
{
  if (FchSerialUseHardwareFlowControl) {
    if (FchSerialDetectCable) {
      //
      // Wait for both DSR and CTS to be set
      //   DSR is set if a cable is connected.
      //   CTS is set if it is ok to transmit data
      //
      //   DSR  CTS  Description                               Action
      //   ===  ===  ========================================  ========
      //    0    0   No cable connected.                       Wait
      //    0    1   No cable connected.                       Wait
      //    1    0   Cable connected, but not clear to send.   Wait
      //    1    1   Cable connected, and clear to send.       Transmit
      //
      return (BOOLEAN) ((SerialPortReadRegister (FCH_UART_MSR) & (FCH_UART_MSR_DSR | FCH_UART_MSR_CTS)) == (FCH_UART_MSR_DSR | FCH_UART_MSR_CTS));
    } else {
      //
      // Wait for both DSR and CTS to be set OR for DSR to be clear.  
      //   DSR is set if a cable is connected.
      //   CTS is set if it is ok to transmit data
      //
      //   DSR  CTS  Description                               Action
      //   ===  ===  ========================================  ========
      //    0    0   No cable connected.                       Transmit
      //    0    1   No cable connected.                       Transmit
      //    1    0   Cable connected, but not clear to send.   Wait
      //    1    1   Cable connected, and clar to send.        Transmit
      //
      return (BOOLEAN) ((SerialPortReadRegister (FCH_UART_MSR) & (FCH_UART_MSR_DSR | FCH_UART_MSR_CTS)) != (FCH_UART_MSR_DSR));
    }
  }

  return TRUE;
}


/**
  Return TRUE if a cable is connected to the serial port. This function
  is used to avoid slowing down BIOS boot with serial logging while nothing
  is attached to the serial port. The typical terminal emulator+null modem
  cable setup will cause CTS or DSR or both to be asserted when the cable is
  connected and the terminal emulator has the serial port open. The function
  retruns TRUE if CTS, DSR, or both are high.

  @retval TRUE  The serial cable is connected.
  @retval FALSE The serial cable is not connected.
**/
STATIC BOOLEAN
SerialCableConnected (
  VOID
  )
{
  return (BOOLEAN) (SerialPortReadRegister (FCH_UART_MSR) & (FCH_UART_MSR_DSR | FCH_UART_MSR_CTS));
}


/**
  Initialize the serial device hardware.
  
  If no initialization is required, then return RETURN_SUCCESS.
  If the serial device was successfully initialized, then return RETURN_SUCCESS.
  If the serial device could not be initialized, then return RETURN_DEVICE_ERROR.
  
  @retval RETURN_SUCCESS        The serial device was initialized.
  @retval RETURN_DEVICE_ERROR   The serial device could not be initialized.

**/
RETURN_STATUS
EFIAPI
SerialPortInitialize (
  VOID
  )
{
//default use BAUD rate 115200
  MmioWrite8 (GetSbUartBase()+FCH_SRR_OFFSET, 0x01); //reset UART0
  MicroSecondDelay(100); // Delay for 100 us to avoid garbage in serial output.
  
  SerialPortWriteRegister (FCH_UART_LCR, FCH_UART_LCR_DLAB);
  SerialPortWriteRegister (FCH_UART_BAUD_HIGH, 00);
  SerialPortWriteRegister (FCH_UART_BAUD_LOW, 0x1a); //set 115200

  SerialPortWriteRegister (FCH_UART_LCR, (UINT8)(FchSerialLineControl & 0x3F));     // 8 bit

  SerialPortWriteRegister (FCH_UART_FCR, (UINT8)(FchSerialFifoControl & 0x27));

  SerialPortWriteRegister (FCH_UART_MCR, FCH_UART_MCR_DSR|FCH_UART_MCR_RTS);  // Data Terminal Ready, Request to Send


//UART0_CTS_L_UART2_RXD_EGPIO135   def:UART0_CTS_L
//UART0_RXD_EGPIO136               def:UART0_RXD
//UART0_RTS_L_UART2_TXD_EGPIO137   def:UART2_TXD
//UART0_TXD_EGPIO138               def:EGPIO138
//UART0_INTR_AGPIO139              def:UART0_INTR    

  MmioWrite8 (FCH_IOMUXx89_UART0_RTS_L_EGPIO137, 0x00);
  MmioWrite8 (FCH_IOMUXx8A_UART0_TXD_EGPIO138, 0x00);

//MmioWrite8 (FCH_IOMUXx8E_UART1_RTS_L_EGPIO142, 0x00);
//MmioWrite8 (FCH_IOMUXx8F_UART1_TXD_EGPIO143, 0x00);
   
  return RETURN_SUCCESS;
}



RETURN_STATUS
EFIAPI
SerialPortInitializeDxe (
  UINT32  SerialBaudRate
  )
{
  UINTN	  Divisor;


  Divisor = PcdGet32 (PcdSerialClockRate) / (SerialBaudRate * 16);
  if ((PcdGet32 (PcdSerialClockRate) % (SerialBaudRate * 16)) >= SerialBaudRate * 8) {
    Divisor++;
  }

  MmioWrite8 (GetSbUartBase()+FCH_SRR_OFFSET, 0x01); //reset UART0
  MicroSecondDelay(100); // Delay for 100 us to avoid garbage in serial output.
  
  SerialPortWriteRegister (FCH_UART_LCR, FCH_UART_LCR_DLAB);
  SerialPortWriteRegister (R_UART_BAUD_HIGH, (UINT8) (Divisor >> 8));
  SerialPortWriteRegister (R_UART_BAUD_LOW, (UINT8) (Divisor & 0xff));

  SerialPortWriteRegister (FCH_UART_LCR, (UINT8)(FchSerialLineControl & 0x3F));     // 8 bit

  SerialPortWriteRegister (FCH_UART_FCR, (UINT8)(FchSerialFifoControl & 0x27));

  SerialPortWriteRegister (FCH_UART_MCR, FCH_UART_MCR_DSR|FCH_UART_MCR_RTS);  // Data Terminal Ready, Request to Send


//UART0_CTS_L_UART2_RXD_EGPIO135   def:UART0_CTS_L
//UART0_RXD_EGPIO136               def:UART0_RXD
//UART0_RTS_L_UART2_TXD_EGPIO137   def:UART2_TXD
//UART0_TXD_EGPIO138               def:EGPIO138
//UART0_INTR_AGPIO139              def:UART0_INTR    

  MmioWrite8 (FCH_IOMUXx89_UART0_RTS_L_EGPIO137, 0x00);
  MmioWrite8 (FCH_IOMUXx8A_UART0_TXD_EGPIO138, 0x00);

//MmioWrite8 (FCH_IOMUXx8E_UART1_RTS_L_EGPIO142, 0x00);
//MmioWrite8 (FCH_IOMUXx8F_UART1_TXD_EGPIO143, 0x00);
   
  return RETURN_SUCCESS;
}






/**
  Write data from buffer to serial device. 
 
  Writes NumberOfBytes data bytes from Buffer to the serial device.  
  The number of bytes actually written to the serial device is returned.
  If the return value is less than NumberOfBytes, then the write operation failed.

  If Buffer is NULL, then ASSERT(). 

  If NumberOfBytes is zero, then return 0.

  @param  Buffer           Pointer to the data buffer to be written.
  @param  NumberOfBytes    Number of bytes to written to the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes written to the serial device.  
                           If this value is less than NumberOfBytes, then the read operation failed.

**/

UINTN
EFIAPI
SerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
)
{

  UINTN     Result;
  UINTN     Index, i;
  UINTN     FifoSize;
  UINT32    PostcodeAddr = 0xFED80E78;
  UINT32    PostcodeVal = 0;
  UINT8     code[12]={0};  
  UINT8     *PostcodeValPtr = 0; 
  UINT8     tempbyte =0;


#if (defined(FCH_CHK_SERIAL_CABLE) && (FCH_CHK_SERIAL_CABLE == 1)) 
  // Boot time reduction: skip serial output if no cable connected
  if (!SerialCableConnected()) return 0;
#endif
  
  //read postcode from FCH MMIO 0xFED80E78   
  PostcodeVal=*((UINT32 *)&PostcodeAddr);
  PostcodeValPtr= (UINT8 *)&PostcodeVal;
  PostcodeValPtr=PostcodeValPtr+3;
    
  if (NULL == Buffer) {
    return 0;
  }
 

  //Check is the Serial port init ok
  if ((SerialPortReadRegister (FCH_UART_LCR) & 0x3F) !=FchSerialLineControl) {
    SerialPortInitialize();                // re-init
  }
  
  if (NumberOfBytes == 0) {
    //
    // Flush the hardware
    //

    //
    // Wait for both the transmit FIFO and shift register empty.
    //
    while ((SerialPortReadRegister (FCH_UART_LSR) & FCH_UART_LSR_TEMT) == 0);

    while (!SerialPortWritable ());
    return 0;
  }

  //
  // Compute the maximum size of the Tx FIFO
  //
  FifoSize = 1;
  if ((FchSerialFifoControl & FCH_UART_FCR_FIFOE) != 0) {
    if ((FchSerialFifoControl & FCH_UART_FCR_FIFO64) == 0) {
      FifoSize = 16;
    } else {
      FifoSize = 64;
    }
  }
    
  Result = NumberOfBytes;
  while (NumberOfBytes != 0) {
    //
    // Wait for the serial port to be ready, to make sure both the transmit FIFO
    // and shift register empty.
    //
    while ((SerialPortReadRegister (FCH_UART_LSR) & FCH_UART_LSR_TEMT) == 0);

    //
    // Fill then entire Tx FIFO
    //
    //#ifndef PEI_BUILD
    //   { volatile UINTN __DeadLoopVar__ = 1; while (__DeadLoopVar__); }
    //#endif
    for (Index = 0; Index < FifoSize && NumberOfBytes != 0; Index++, NumberOfBytes--, Buffer++) {
      //
      // Wait for the hardware flow control signal
      //
      while (!SerialPortWritable ());
       
      //
      // Write byte to the transmit buffer.
      //
      SerialPortWriteRegister (FCH_UART_TXBUF, *Buffer);
    }
  }

  //Debug Postcode - print 32bit postcode value after each string  
  //Debug Postcode - can just change this to 0 if want to see regular output.
//  NumberOfBytes = 12;	//9 bytes for 32bit postcode + ASCII CR
  NumberOfBytes = 0;	//9 bytes for 32bit postcode + ASCII CR
  
  for (i=0;i<8;i=i+2)
  {
    tempbyte=*PostcodeValPtr;
    if((tempbyte & 0xF) < 0xA)
      code[i+1+2]=(tempbyte & 0xF) + 0x30;
    else
      code[i+1+2]=(tempbyte & 0xF) + 0x37;

    if(((tempbyte & 0xF0) >> 4) < 0xA)
      code[i+2]=((tempbyte & 0xF0) >> 4) + 0x30;
    else
      code[i+2]=((tempbyte & 0xF0) >> 4) + 0x37;
      PostcodeValPtr--;
  }

  code[0]=0x30;  //0
  code[1]=0x78; //x
  code[10]=0xD; //CR
  code[11]=0xA;//Newline  
  while (NumberOfBytes != 0) {
    //
    // Wait for the serial port to be ready, to make sure both the transmit FIFO
    // and shift register empty.
    //
    while ((SerialPortReadRegister (FCH_UART_LSR) & FCH_UART_LSR_TEMT) == 0);

    //
    // Fill then entire Tx FIFO
    //
    //#ifndef PEI_BUILD
    //   { volatile UINTN __DeadLoopVar__ = 1; while (__DeadLoopVar__); }
    //#endif
    for (Index = 0; Index < FifoSize && NumberOfBytes != 0; Index++, NumberOfBytes--) {
      //
      // Wait for the hardware flow control signal
      //
      // while (!SerialPortWritable ());
      while (!SerialPortWritable ());
        
      //
      // Write byte to the transmit buffer.
      //
      SerialPortWriteRegister (FCH_UART_TXBUF, code[Index]);
     }
  }
  //debug postcode end

  return Result;
}

/**
  Reads data from a serial device into a buffer.

  @param  Buffer           Pointer to the data buffer to store the data read from the serial device.
  @param  NumberOfBytes    Number of bytes to read from the serial device.

  @retval 0                NumberOfBytes is 0.
  @retval >0               The number of bytes read from the serial device.  
                           If this value is less than NumberOfBytes, then the read operation failed.

**/
UINTN
EFIAPI
SerialPortRead (
  OUT UINT8     *Buffer,
  IN  UINTN     NumberOfBytes
)
{
  UINTN  Result;
  UINT8  Mcr;
 
  if (NULL == Buffer) {
      return 0;
  }

  Mcr = (UINT8)(SerialPortReadRegister (FCH_UART_MCR) & ~FCH_UART_MCR_RTS);


  for (Result = 0; NumberOfBytes-- != 0; Result++, Buffer++) {
      //
      // Wait for the serial port to have some data.
      //
      while ((SerialPortReadRegister (FCH_UART_LSR) & FCH_UART_LSR_RXRDY) == 0) {
      
          if (FchSerialUseHardwareFlowControl) {
          //
          // Set RTS to let the peer send some data
          //
          SerialPortWriteRegister (FCH_UART_MCR, (UINT8)(Mcr | FCH_UART_MCR_RTS));
         }
      }
      
      if (FchSerialUseHardwareFlowControl) {
        //
        // Clear RTS to prevent peer from sending data
        //
        SerialPortWriteRegister (FCH_UART_MCR, Mcr);
      }
      
      //
      // Read byte from the receive buffer.
      //
      *Buffer = SerialPortReadRegister (FCH_UART_RXBUF);
  }

  return Result;
}


/**
  Polls a serial device to see if there is any data waiting to be read.

  Polls aserial device to see if there is any data waiting to be read.
  If there is data waiting to be read from the serial device, then TRUE is returned.
  If there is no data waiting to be read from the serial device, then FALSE is returned.

  @retval TRUE             Data is waiting to be read from the serial device.
  @retval FALSE            There is no data waiting to be read from the serial device.

**/
BOOLEAN
EFIAPI
SerialPortPoll (
  VOID
  )
{
  //
  // Read the serial port status
  //
  if ((SerialPortReadRegister (FCH_UART_LSR) & FCH_UART_LSR_RXRDY) != 0) {
      if (FchSerialUseHardwareFlowControl) {
      //
      // Clear RTS to prevent peer from sending data
      //
      SerialPortWriteRegister (FCH_UART_MCR, (UINT8)(SerialPortReadRegister (FCH_UART_MCR) & ~FCH_UART_MCR_RTS));
    }
    return TRUE;
  }    
  
    if (FchSerialUseHardwareFlowControl) {
    //
    // Set RTS to let the peer send some data
    //
    SerialPortWriteRegister (FCH_UART_MCR, (UINT8)(SerialPortReadRegister (FCH_UART_MCR) | FCH_UART_MCR_RTS));
  }
  
  return FALSE;
}



/**
  Init serial port

**/
RETURN_STATUS
EFIAPI
PlatformHookSerialPortInitialize (
  VOID
  )
{
  SerialPortInitialize();
  
  return RETURN_SUCCESS;
}

/**
Sets the control bits on a serial device.
 
@param Control				  Sets the bits of Control that are settable.
 
@retval RETURN_SUCCESS		  The new control bits were set on the serial device.
@retval RETURN_UNSUPPORTED	  The serial device does not support this operation.
@retval RETURN_DEVICE_ERROR   The serial device is not functioning correctly.
 
 **/
RETURN_STATUS
EFIAPI
SerialPortSetControl (
  IN UINT32 Control
)
{
  UINTN SerialRegisterBase;
  UINT8 Mcr;

  //
  // First determine the parameter is invalid.
  //
  if ((Control & (~(EFI_SERIAL_REQUEST_TO_SEND | EFI_SERIAL_DATA_TERMINAL_READY |
    EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE))) != 0) {
    return RETURN_UNSUPPORTED;
  }

  SerialRegisterBase = GetSbUartBase ();
  if (SerialRegisterBase ==0) {
    return RETURN_UNSUPPORTED;
  }

//
// Read the Modem Control Register.
//
  Mcr = SerialPortReadRegister (R_UART_MCR);
  Mcr &= (~(B_UART_MCR_DTRC | B_UART_MCR_RTS));
  
  if ((Control & EFI_SERIAL_DATA_TERMINAL_READY) == EFI_SERIAL_DATA_TERMINAL_READY) {
    Mcr |= B_UART_MCR_DTRC;
  }
  
  if ((Control & EFI_SERIAL_REQUEST_TO_SEND) == EFI_SERIAL_REQUEST_TO_SEND) {
    Mcr |= B_UART_MCR_RTS;
  }
  
  //
  // Write the Modem Control Register.
  //
  SerialPortWriteRegister (R_UART_MCR, Mcr);
    DEBUG((DEBUG_INFO,"chuang0522 SerialPortSetControl \n")); 
    return RETURN_SUCCESS;
  }

/**
Retrieve the status of the control bits on a serial device.
 
@param Control				  A pointer to return the current control signals from the serial device.
 
@retval RETURN_SUCCESS		  The control bits were read from the serial device.
@retval RETURN_UNSUPPORTED	  The serial device does not support this operation.
@retval RETURN_DEVICE_ERROR   The serial device is not functioning correctly.
**/
RETURN_STATUS
EFIAPI
SerialPortGetControl (
  OUT UINT32 *Control
  )
{
  UINTN SerialRegisterBase;
  UINT8 Msr;
  UINT8 Mcr;
  UINT8 Lsr;
  
  SerialRegisterBase = GetSbUartBase ();
  if (SerialRegisterBase ==0) {
    return RETURN_UNSUPPORTED;
  }
  
  *Control = 0;
  
  //
  // Read the Modem Status Register.
  //
  Msr = SerialPortReadRegister (R_UART_MSR);
  
  if ((Msr & B_UART_MSR_CTS) == B_UART_MSR_CTS) {
    *Control |= EFI_SERIAL_CLEAR_TO_SEND;
  }
  
  if ((Msr & B_UART_MSR_DSR) == B_UART_MSR_DSR) {
    *Control |= EFI_SERIAL_DATA_SET_READY;
  }
  
  if ((Msr & B_UART_MSR_RI) == B_UART_MSR_RI) {
    *Control |= EFI_SERIAL_RING_INDICATE;
  }
  
  if ((Msr & B_UART_MSR_DCD) == B_UART_MSR_DCD) {
    *Control |= EFI_SERIAL_CARRIER_DETECT;
  }
  
  //
  // Read the Modem Control Register.
  //
  Mcr = SerialPortReadRegister (R_UART_MCR);
  
  if ((Mcr & B_UART_MCR_DTRC) == B_UART_MCR_DTRC) {
    *Control |= EFI_SERIAL_DATA_TERMINAL_READY;
  }
  
  if ((Mcr & B_UART_MCR_RTS) == B_UART_MCR_RTS) {
    *Control |= EFI_SERIAL_REQUEST_TO_SEND;
  }
  
  if (PcdGetBool (PcdSerialUseHardwareFlowControl)) {
    *Control |= EFI_SERIAL_HARDWARE_FLOW_CONTROL_ENABLE;
  }
  
  //
  // Read the Line Status Register.
  //
  Lsr = SerialPortReadRegister (R_UART_LSR);
  
  if ((Lsr & (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) == (B_UART_LSR_TEMT | B_UART_LSR_TXRDY)) {
    *Control |= EFI_SERIAL_OUTPUT_BUFFER_EMPTY;
  }
  
  if ((Lsr & B_UART_LSR_RXRDY) == 0) {
    *Control |= EFI_SERIAL_INPUT_BUFFER_EMPTY;
  }

  return RETURN_SUCCESS;
}

/**
Sets the baud rate, receive FIFO depth, transmit/receice time out, parity,
data bits, and stop bits on a serial device.
 
@param BaudRate   The requested baud rate. A BaudRate value of 0 will use the
                  device's default interface speed.
                  On output, the value actually set.
@param ReveiveFifoDepth   The requested depth of the FIFO on the receive side of the
                  serial interface. A ReceiveFifoDepth value of 0 will use
                  the device's default FIFO depth.
                  On output, the value actually set.
@param Timeout    The requested time out for a single character in microseconds.
                  This timeout applies to both the transmit and receive side of the
                  interface. A Timeout value of 0 will use the device's default time
                  out value.
                  On output, the value actually set.
@param Parity     The type of parity to use on this serial device. A Parity value of
                  DefaultParity will use the device's default parity value.
                  On output, the value actually set.
@param DataBits   The number of data bits to use on the serial device. A DataBits
                  vaule of 0 will use the device's default data bit setting.
                  On output, the value actually set.
@param StopBits   The number of stop bits to use on this serial device. A StopBits
                  value of DefaultStopBits will use the device's default number of
                  stop bits.
                  On output, the value actually set.
 
@retval RETURN_SUCCESS			  The new attributes were set on the serial device.
@retval RETURN_UNSUPPORTED		  The serial device does not support this operation.
@retval RETURN_INVALID_PARAMETER  One or more of the attributes has an unsupported value.
@retval RETURN_DEVICE_ERROR 	  The serial device is not functioning correctly.
**/
RETURN_STATUS
EFIAPI
SerialPortSetAttributes (
  IN OUT UINT64			  *BaudRate,
  IN OUT UINT32			  *ReceiveFifoDepth,
  IN OUT UINT32			  *Timeout,
  IN OUT EFI_PARITY_TYPE	  *Parity,
  IN OUT UINT8			  *DataBits,
  IN OUT EFI_STOP_BITS_TYPE *StopBits
  )
{
  UINTN	  SerialRegisterBase;
  UINT32	  SerialBaudRate;
  UINTN	  Divisor;
  UINT8	  Lcr;
  UINT8	  LcrData;
  UINT8	  LcrParity;
  UINT8	  LcrStop;
  
  SerialRegisterBase = GetSbUartBase ();
  if (SerialRegisterBase ==0) {
    return RETURN_UNSUPPORTED;
  }

  //
  // Check for default settings and fill in actual values.
  //
  if (*BaudRate == 0) {
    *BaudRate = PcdGet32 (PcdSerialBaudRate);
  }
  SerialBaudRate = (UINT32) *BaudRate;
  
  if (*DataBits == 0) {
    LcrData = (UINT8) (PcdGet8 (PcdSerialLineControl) & 0x3);
    *DataBits = LcrData + 5;
  } else {
    if ((*DataBits < 5) || (*DataBits > 8)) {
      return RETURN_INVALID_PARAMETER;
    }
    //
    // Map 5..8 to 0..3
    //
    LcrData = (UINT8) (*DataBits - (UINT8) 5);
  }

  if (*Parity == DefaultParity) {
    LcrParity = (UINT8) ((PcdGet8 (PcdSerialLineControl) >> 3) & 0x7);
    switch (LcrParity) {
      case 0:
        *Parity = NoParity;
        break;

      case 3:
        *Parity = EvenParity;
        break;
      
      case 1:
        *Parity = OddParity;
        break;

      case 7:
        *Parity = SpaceParity;
        break;

      case 5:
        *Parity = MarkParity;
        break;

      default:
        break;
    }
  } else {
    switch (*Parity) {
      case NoParity:
        LcrParity = 0;
        break;
      
      case EvenParity:
        LcrParity = 3;
        break;
      
      case OddParity:
        LcrParity = 1;
        break;
      
      case SpaceParity:
        LcrParity = 7;
        break;
      
      case MarkParity:
        LcrParity = 5;
        break;
      
      default:
        return RETURN_INVALID_PARAMETER;
    }
  }

  if (*StopBits == DefaultStopBits) {
    LcrStop = (UINT8) ((PcdGet8 (PcdSerialLineControl) >> 2) & 0x1);
    switch (LcrStop) {
      case 0:
        *StopBits = OneStopBit;
        break;
   
      case 1:
        if (*DataBits == 5) {
      	*StopBits = OneFiveStopBits;
        } else {
      	*StopBits = TwoStopBits;
        }
        break;
   
      default:
        break;
    }
  } else {
    switch (*StopBits) {
      case OneStopBit:
      LcrStop = 0;
      break;
    
      case OneFiveStopBits:
      case TwoStopBits:
      LcrStop = 1;
      break;
    
      default:
      return RETURN_INVALID_PARAMETER;
    }
  }

  //
  // Calculate divisor for baud generator
  // Ref_Clk_Rate / Baud_Rate / 16
  //

  Divisor = PcdGet32 (PcdSerialClockRate) / (SerialBaudRate * 16);
  if ((PcdGet32 (PcdSerialClockRate) % (SerialBaudRate * 16)) >= SerialBaudRate * 8) {
    Divisor++;
  }


  //
  // Configure baud rate
  //
  MicroSecondDelay(1000000);
  SerialPortWriteRegister (R_UART_LCR, B_UART_LCR_DLAB);
  SerialPortWriteRegister (R_UART_BAUD_HIGH, (UINT8) (Divisor >> 8));
  SerialPortWriteRegister (R_UART_BAUD_LOW, (UINT8) (Divisor & 0xff));

  //
  // Clear DLAB and configure Data Bits, Parity, and Stop Bits.
  // Strip reserved bits from line control value
  //
  Lcr = (UINT8) ((LcrParity << 3) | (LcrStop << 2) | LcrData);
  SerialPortWriteRegister (R_UART_LCR, (UINT8) (Lcr & 0x3F));

  return RETURN_SUCCESS;
}


