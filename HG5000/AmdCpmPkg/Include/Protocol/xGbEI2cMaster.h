/*****************************************************************************
 *
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


#ifndef _XGBE_I2C_MASTER_H_
#define _XGBE_I2C_MASTER_H_

#include <Pi/PiI2c.h>


#define XGBE_I2C_MASTER_PROTOCOL_GUID {\
  0xF15E1432, 0x4B9B, 0x2521, { 0xCA, 0x59, 0xE7, 0x98, 0x37, 0xCA, 0x0F, 0x0A} \
}


typedef struct _XGBE_I2C_MASTER_PROTOCOL XGBE_I2C_MASTER_PROTOCOL;

/**
  Set the frequency for the I2cClock line.

  This routine must be called at or below TPL_NOTIFY.

  The software and controller do a best case effort of using the specified
  frequency for the I2cBus.  If the frequency does not match exactly then
  the I2cMaster protocol selects the next lower frequency to avoid
  exceeding the operating conditions for any of the I2cDevices on the bus.
  For example if 400 KHz was specified and the controller's divide network
  only supports 402 KHz or 398 KHz then the I2cMaster protocol selects 398
  KHz.  If there are not lower frequencies available, then return
  EFI_UNSUPPORTED.

  @param[in] This           Pointer to an EFI_I2C_MASTER_PROTOCOL structure
  @param[in] BusClockHertz  Pointer to the requested I2cBus clock frequency
                            in Hertz.  Upon return this value contains the
                            actual frequency in use by the I2cController.

  @retval EFI_SUCCESS           The bus frequency was set successfully.
  @retval EFI_ALREADY_STARTED   The controller is busy with another transaction.
  @retval EFI_INVALID_PARAMETER BusClockHertz is NULL
  @retval EFI_UNSUPPORTED       The controller does not support this frequency.

**/
typedef
EFI_STATUS
(EFIAPI *XGBE_I2C_MASTER_PROTOCOL_SET_BUS_FREQUENCY) (
  IN CONST XGBE_I2C_MASTER_PROTOCOL   *This,
  IN OUT UINTN                       *BusClockHertz
  );

/**
  Reset the I2cController and configure it for use

  This routine must be called at or below TPL_NOTIFY.

  The I2cController is reset.  The caller must call SetBusFrequench() after
  calling Reset().

  @param[in]     This       Pointer to an EFI_I2C_MASTER_PROTOCOL structure.

  @retval EFI_SUCCESS         The reset completed successfully.
  @retval EFI_ALREADY_STARTED The controller is busy with another transaction.
  @retval EFI_DEVICE_ERROR    The reset operation failed.

**/
typedef
EFI_STATUS
(EFIAPI *XGBE_I2C_MASTER_PROTOCOL_I2C_INIT) (
  IN CONST XGBE_I2C_MASTER_PROTOCOL *This
  );

/**
  Start an I2cTransaction on the host controller.

  This routine must be called at or below TPL_NOTIFY.  For synchronous
  requests this routine must be called at or below TPL_CALLBACK.

  This function initiates an I2cTransaction on the controller.  To
  enable proper error handling by the I2cProtocol stack, the I2cMaster
   protocol does not support queuing but instead only manages
  one I2cTransaction at a time.  This API requires that the I2cBus
  is in the correct configuration for the I2cTransaction.

  The transaction is performed by sending a start-bit and selecting the
  I2cDevice with the specified I2cSlave address and then performing
  the specified I2cOperations.  When multiple operations are requested
  they are separated with a repeated start bit and the slave address.
  The transaction is terminated with a stop bit.

  When Event is NULL, StartRequest operates synchronously and returns
  the I2cCompletion status as its return value.

  When Event is not NULL, StartRequest synchronously returns EFI_SUCCESS
  indicating that the I2cTransaction was started asynchronously.  The
  transaction status value is returned in the buffer pointed to by
  I2cStatus upon the completion of the I2cTransaction when I2cStatus
  is not NULL.  After the transaction status is returned the Event is
  signaled.

  Note: The typical consumer of this API is the I2cHost protocol.
  Extreme care must be taken by other consumers of this API to prevent
  confusing the third party I2cDrivers due to a state change at the
  I2cDevice which the third party I2cDrivers did not initiate. I2dPlatform
   specific code may use this API within these guidelines.

  @param[in] This           Pointer to an EFI_I2C_MASTER_PROTOCOL structure.
  @param[in] SlaveAddress   Address of the device on the I2cBus.  Set the
                            I2C_ADDRESSING_10_BIT when using 10-bit addresses,
                            clear this bit for 7-bit addressing.  Bits 0-6
                            are used for 7-bit I2cSlave addresses and bits
                            0-9 are used for 10-bit I2cSlave addresses.
  @param[in] RequestPacket  Pointer to an EFI_I2C_REQUEST_PACKET
                            structure describing the I2cTransaction.
  @param[in] Event          Event to signal for asynchronous transactions,
                            NULL for asynchronous transactions
  @param[out] I2cStatus     Optional buffer to receive the I2cTransaction
                            completion status

  @retval EFI_SUCCESS           The asynchronous transaction was successfully
                                started when Event is not NULL.
  @retval EFI_SUCCESS           The transaction completed successfully when
                                Event is NULL.
  @retval EFI_ALREADY_STARTED   The controller is busy with another transaction.
  @retval EFI_BAD_BUFFER_SIZE   The RequestPacket->LengthInBytes value is too
                                large.
  @retval EFI_DEVICE_ERROR      There was an I2cError (NACK) during the
                                transaction.
  @retval EFI_INVALID_PARAMETER RequestPacket is NULL
  @retval EFI_NOT_FOUND         Reserved bit set in the SlaveAddress parameter
  @retval EFI_NO_RESPONSE       The I2cDevice is not responding to the slave
                                address.  EFI_DEVICE_ERROR will be returned if
                                the controller cannot distinguish when the NACK
                                occurred.
  @retval EFI_OUT_OF_RESOURCES  Insufficient memory for I2cTransaction
  @retval EFI_UNSUPPORTED       The controller does not support the requested
                                transaction.

**/
typedef
EFI_STATUS
(EFIAPI *XGBE_I2C_MASTER_PROTOCOL_START_REQUEST) (
  IN CONST XGBE_I2C_MASTER_PROTOCOL *This,
  IN UINTN                         SlaveAddress,
  IN EFI_I2C_REQUEST_PACKET       *RequestPacket,
  IN EFI_EVENT                     Event      OPTIONAL,
  OUT EFI_STATUS                   *I2cStatus OPTIONAL
  );

///
/// I2cMaster mode protocol
///
/// This protocol manipulates the I2cHost controller to perform transactions as a
/// master on the I2cBus using the current state of any switches or multiplexers
/// in the I2cBus.
///
struct _XGBE_I2C_MASTER_PROTOCOL {
  ///
  /// The xGbE I2cController base addess.
  ///
  UINT32  BaseAddress;
  ///
  /// Set the clock frequency for the I2cBus.
  ///
  XGBE_I2C_MASTER_PROTOCOL_SET_BUS_FREQUENCY SetBusFrequency;
  ///
  /// Initiate I2cController when first time use.
  ///
  XGBE_I2C_MASTER_PROTOCOL_I2C_INIT I2cInit;
  ///
  /// Start an I2cTransaction in master mode on the host controller.
  ///
  XGBE_I2C_MASTER_PROTOCOL_START_REQUEST StartRequest;
};

extern EFI_GUID gxGbEI2cMasterProtocolGuid;

#endif  //  _XGBE_I2C_MASTER_H_
