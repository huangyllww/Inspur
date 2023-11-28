/**
 * @file
 *
 * AMD Integrated Debug Debug_library Routines
 *
 * Contains the functions related to serial port output
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdIdsDebugPrintLib.h>
#include <AmdIdsDebugPrintLocal.h>
#include <Filecode.h>

#define FILECODE LIBRARY_AMDIDSDEBUGPRINTLIB_AMDIDSDPSERIAL_FILECODE

#define IDS_SERIAL_PORT (PcdGet64(PcdAmdIdsDebugPrintSerialPort))

#define IDS_SERIAL_PORT_LSR \
(IDS_SERIAL_PORT > 0xFFFF ? \
(IDS_SERIAL_PORT + 5*4) : \
(IDS_SERIAL_PORT + 5))

#define IDS_LSR_TRANSMIT_HOLDING_REGISTER_EMPTY_MASK BIT5

#define IDS_SERIAL_PORT_LSR_READ(Value) \
(IDS_SERIAL_PORT > 0xFFFF ? \
LibAmdMemRead (AccessWidth8, (UINT64) IDS_SERIAL_PORT_LSR, &Value, NULL) : \
LibAmdIoRead (AccessWidth8, (UINT16) IDS_SERIAL_PORT_LSR, &Value, NULL))

#define IDS_SERIAL_PORT_WRITE(ByteSended) \
(IDS_SERIAL_PORT > 0xFFFF ? \
LibAmdMemWrite (AccessWidth8, (UINT64) IDS_SERIAL_PORT, &ByteSended, NULL) : \
LibAmdIoWrite (AccessWidth8, (UINT16) IDS_SERIAL_PORT, &ByteSended, NULL))

#define IDS_SERIAL_PORT_MSR \
(IDS_SERIAL_PORT > 0xFFFF ? \
(IDS_SERIAL_PORT + 6*4) : \
(IDS_SERIAL_PORT + 6))

#define IDS_SERIAL_PORT_MSR_MASK     (BIT4 + BIT5)

#define IDS_SERIAL_PORT_MSR_READ(Value) \
(IDS_SERIAL_PORT > 0xFFFF ? \
LibAmdMemRead(AccessWidth8, (UINT64) IDS_SERIAL_PORT_MSR, &Value, NULL) : \
LibAmdIoRead(AccessWidth8, (UINT16) IDS_SERIAL_PORT_MSR, &Value, NULL))

/**
 *  Determine whether IDS console is enabled.
 *
 *
 *  @retval       TRUE    Alway return true
 *
 **/
BOOLEAN
AmdIdsDpSerialSupport (
  VOID
  )
{
  UINT8 Value;

  if (PcdGetBool (PcdAmdIdsDebugPrintSerialPortEnable)) {
    if (PcdGetBool (PcdAmdIdsDebugPrintSerialPortDetectCableConnection) ) {
      // Wait for both DSR and CTS to be set
      //   DSR is set if a cable is connected.
      //   CTS is set if it is ok to transmit data
      IDS_SERIAL_PORT_MSR_READ (Value);
      if ((Value == 0xFF) || ((Value & IDS_SERIAL_PORT_MSR_MASK) != IDS_SERIAL_PORT_MSR_MASK)) {
        return FALSE;
      }
    }
    return TRUE;
  }
  return FALSE;
}

VOID
AmdIdsDpSerialConstructor (
  VOID
  )
{
  return;
}

/**
 *  Get Serial customize Filter
 *
 *  @param[in,out] Filter    Filter do be filled
 *
 *  @retval       FALSE    Alway return FALSE
 *
 **/
BOOLEAN
AmdIdsDpSerialGetFilter (
  IN OUT   UINT64 *Filter
  )
{
  return FALSE;
}

/**
 *  Send byte to  Serial Port
 *
 *  Before use this routine, please make sure Serial Communications Chip have been initialed
 *
 *  @param[in] ByteSended   Byte to be sended
 *
 *  @retval       TRUE    Byte sended successfully
 *  @retval       FALSE   Byte sended failed
 *
 **/
BOOLEAN
AmdIdsDpSerialSendByte (
  IN      CHAR8   ByteSended
  )
{
  UINT32 RetryCount;
  UINT8 Value;

  // Wait until LSR.Bit5 (Transmitter holding register Empty)
  RetryCount = 2000;
  do {
    IDS_SERIAL_PORT_LSR_READ (Value);
    RetryCount--;
  } while (((Value & IDS_LSR_TRANSMIT_HOLDING_REGISTER_EMPTY_MASK) == 0) && (RetryCount > 0));

  if (RetryCount == 0) {
    // Time expired
    return FALSE;
  } else {
    IDS_SERIAL_PORT_WRITE (ByteSended);
    return TRUE;
  }
}


// winddy - >>
UINTN
EFIAPI
LegacyUartSerialPortWrite (
  IN UINT8     *Buffer,
  IN UINTN     NumberOfBytes
  );

/**
 *  Print formated string
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *
**/
VOID
AmdIdsDpSerialPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize
  )
{
#if _PCD_VALUE_PcdIdsLegacyUartPrintOut
  LegacyUartSerialPortWrite(Buffer, BufferSize);
#else
  BOOLEAN SendStatus;
  while (BufferSize--) {
    if (*Buffer == '\n') {
      SendStatus = AmdIdsDpSerialSendByte ('\r');
    }
    SendStatus = AmdIdsDpSerialSendByte (*Buffer);
    Buffer ++;
  }
#endif
}

// winddy - <<

CONST AMD_IDS_DEBUG_PRINT_INSTANCE   AmdIdsDebugPrintSerialInstance =
{
  AmdIdsDpSerialSupport,
  AmdIdsDpSerialConstructor,
  AmdIdsDpSerialGetFilter,
  AmdIdsDpSerialPrint
};



