/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 *
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header doess *NOT* give you permission to use the Materials
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
 *
 ******************************************************************************
 */

#ifndef __I3C_MASTER_PROTOCOL_H__
#define __I3C_MASTER_PROTOCOL_H__

#include <PiI3c.h>

#define EFI_DXE_I3C_MASTER_PROTOCOL_GUID \
  {0xdb9b6d74, 0xe766, 0x475c, {0x94, 0x4c, 0x33, 0x91, 0x37, 0x2b, 0xa1, 0x5c}}
  
typedef struct _EFI_DXE_I3C_MASTER_PROTOCOL EFI_DXE_I3C_MASTER_PROTOCOL;

/**
  Set the frequency for the I3C clock line.
  
  @param This                   Pointer to an EFI_DXE_I3C_MASTER_PROTOCOL structure.
  @param BusClockHertz          Pointer to the requested I3C bus clock frequency in Hertz.
                                Upon return this value contains the actual frequency 
                                in use by the I3C controller.

  @retval EFI_SUCCESS           The bus frequency was set successfully.
  @retval EFI_INVALID_PARAMETER BusClockHertz is NULL
  @retval EFI_UNSUPPORTED       The controller does not support this frequency.
   
**/
typedef
EFI_STATUS
(EFIAPI *EFI_DXE_I3C_MASTER_PROTOCOL_SET_BUS_FREQUENCY) (
  IN EFI_DXE_I3C_MASTER_PROTOCOL    *This,
  IN UINTN                          *BusClockHertz
  );


/**
  Reset the I3C controller and configure it for use.
  
  @param  This                  Pointer to an EFI_DXE_I3C_MASTER_PROTOCOL structure.

  @retval EFI_SUCCESS           The reset completed successfully.
  @retval EFI_DEVICE_ERROR      The reset operation failed.
   
**/
typedef
EFI_STATUS
(EFIAPI *EFI_DXE_I3C_MASTER_PROTOCOL_RESET) (
  IN CONST EFI_DXE_I3C_MASTER_PROTOCOL  *This
  );

/**
  Start an I3C transaction on the host controller.
  
  @param  This                   Pointer to an EFI_DXE_I3C_MASTER_PROTOCOL structure.
  @param  SlaveAddress           Address of the device on the I3C bus.
                                 Set the I3C_ADDRESSING_10_BIT when using 10-bit addresses, 
                                 clear this bit for 7-bit addressing.
                                 Bits 0-6 are used for 7-bit I3C slave addresses and 
                                 bits 0-9 are used for 10-bit I3C slave addresses.
  @param  RequestPacket          Pointer to an EFI_I3C_REQUEST_PACKET structure describing the I3C transaction.

  @retval EFI_SUCCESS	           The transaction completed successfully.
  @retval EFI_BAD_BUFFER_SIZE	   The RequestPacket->LengthInBytes value is too large.
  @retval EFI_DEVICE_ERROR	     There was an I3C error (NACK) during the transaction.
  @retval EFI_INVALID_PARAMETER	 RequestPacket is NULL
  @retval EFI_NO_RESPONSE	       The I3C device is not responding to the slave address.
                                 EFI_DEVICE_ERROR will be returned if the controller cannot distinguish when the NACK occurred.
  @retval EFI_NOT_FOUND	         Reserved bit set in the SlaveAddress parameter
  @retval EFI_OUT_OF_RESOURCES	 Insufficient memory for I3C transaction
  @retval EFI_UNSUPPORTED	       The controller does not support the requested transaction.
   
**/
typedef
EFI_STATUS
(EFIAPI *EFI_DXE_I3C_MASTER_PROTOCOL_START_REQUEST) (
  IN CONST EFI_DXE_I3C_MASTER_PROTOCOL      *This,
  IN UINTN                                  SlaveAddress,
  IN EFI_I3C_REQUEST_PACKET                 *RequestPacket
  );

///
/// This PROTOCOL manipulates the I3C host controller to perform transactions as a master on the I3C bus
/// using the current state of any switches or multiplexers in the I3C bus.
///
struct _EFI_DXE_I3C_MASTER_PROTOCOL {
  EFI_DXE_I3C_MASTER_PROTOCOL_SET_BUS_FREQUENCY   SetBusFrequency;
  EFI_DXE_I3C_MASTER_PROTOCOL_RESET               Reset;
  EFI_DXE_I3C_MASTER_PROTOCOL_START_REQUEST       StartRequest;
  CONST EFI_I3C_CONTROLLER_CAPABILITIES           *I3cControllerCapabilities;
  EFI_GUID                                        Identifier;
};

extern EFI_GUID gEfiI3cMasterProtocolGuid;

#endif
