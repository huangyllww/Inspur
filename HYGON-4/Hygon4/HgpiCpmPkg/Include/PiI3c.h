/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 ******************************************************************************
 */

#ifndef __PI_I3C_H__
#define __PI_I3C_H__

/// I3C read operation when set
#define I3C_FLAG_READ               0x00000001

/// SMBus process call operation
#define I3C_FLAG_SMBUS_PROCESS_CALL 0x00040000

/// SMBus use packet error code (PEC)
///   Note that the I2C master protocol may clear the I2C_FLAG_SMBUS_PEC bit
///   to indicate that the PEC value was checked by the hardware and is
///   not appended to the returned read data.
///
#define I3C_FLAG_SMBUS_PEC          0x00080000

/// I3C controller capabilities
///
/// The EFI_I3C_CONTROLLER_CAPABILITIES specifies the capabilities of the
/// I3C host controller.  The StructureSizeInBytes enables variations of
/// this structure to be identified if there is need to extend this
/// structure in the future.
///
typedef struct {
  ///
  /// Length of this data structure in bytes
  ///
  UINT32 StructureSizeInBytes;

  ///
  /// The maximum number of bytes the I3C host controller is able to
  /// receive from the I3C bus.
  ///
  UINT32 MaximumReceiveBytes;

  ///
  /// The maximum number of bytes the I3C host controller is able to send
  /// on the I3C  bus.
  ///
  UINT32 MaximumTransmitBytes;

  ///
  /// The maximum number of bytes in the I3C bus transaction.
  ///
  UINT32 MaximumTotalBytes;
} EFI_I3C_CONTROLLER_CAPABILITIES;

/// I3C device operation
///
/// The EFI_I3C_OPERATION describes a subset of an I3C transaction in which
/// the I3C controller is either sending or receiving bytes from the bus.
/// Some transactions will consist of a single operation while others will
/// be two or more.
///
/// Note: Some I3C controllers do not support read or write ping (address
/// only) operation and will return EFI_UNSUPPORTED status when these
/// operations are requested.
///
/// Note: I3C controllers which do not support complex transactions requiring
/// multiple repeated start bits return EFI_UNSUPPORTED without processing
/// any of the transaction.
///
typedef struct {
  ///
  /// Flags to qualify the I3C operation.
  ///
  UINT32 Flags;

  ///
  /// Number of bytes to send to or receive from the I3C device.  A ping
  /// (address only byte/bytes)  is indicated by setting the LengthInBytes
  /// to zero.
  ///
  UINT32 LengthInBytes;

  ///
  /// Pointer to a buffer containing the data to send or to receive from
  /// the I3C device.  The Buffer must be at least LengthInBytes in size.
  ///
  UINT8 *Buffer;
} EFI_I3C_OPERATION;

///
/// I3C device request
///
/// The EFI_I3C_REQUEST_PACKET describes a single I3C transaction.  The
/// transaction starts with a start bit followed by the first operation
/// in the operation array.  Subsequent operations are separated with
/// repeated start bits and the last operation is followed by a stop bit
/// which concludes the transaction.  Each operation is described by one
/// of the elements in the Operation array.
///
typedef struct {
  ///
  /// Number of elements in the operation array
  ///
  UINTN OperationCount;

  ///
  /// Description of the I3C operation
  ///
  EFI_I3C_OPERATION Operation [1];
} EFI_I3C_REQUEST_PACKET;

#endif  //  __PI_I3C_H__
