/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH PEI driver.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  FCH PEIM
 *
 */
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

#ifndef _FCH_SMBUS_H_
#define _FCH_SMBUS_H_

#include <Ppi/Smbus2.h>
#include "FchPlatform.h"
//
// EQUate Definitions
//
#define SMB_IOREG00  0x00               // SMBusStatus
#define SMB_IOREG01  0x01               // SMBusSlaveStatus
#define SMB_IOREG02  0x02               // SMBusControl
#define SMB_IOREG03  0x03               // SMBusHostCmd
#define SMB_IOREG04  0x04               // SMBusAddress
#define SMB_IOREG05  0x05               // SMBusData0
#define SMB_IOREG06  0x06               // SMBusData1
#define SMB_IOREG07  0x07               // SMBusBlockData
#define SMB_IOREG08  0x08               // SMBusSlaveControl
#define SMB_IOREG0E  0x0E               // SMBusTiming
#define SMB_IOREG18  0x18               // SMBusSdaDebounceCount
#define SMB_IOREG19  0x19               // SMBusSclDebounceCount
#define SMB_IOREG1A  0x1A               // SMBusSlaveTxHoldCount

#define SMB_CMD_QUICK        0x00 << 2  // Quick Read or Write
#define SMB_CMD_BYTE         0x01 << 2  // Byte Read or Write
#define SMB_CMD_BYTE_DATA    0x02 << 2  // Byte Data Read or Write
#define SMB_CMD_WORD_DATA    0x03 << 2  // Word Data Read or Write
#define SMB_CMD_BLOCK        0x05 << 2  // Block Read or Write
#define SMB_ALL_HOST_STATUS  0x1f       // HostBusy+SMBInterrupt+DeviceErr+BusCollision+Failed

#define TRY_TIMES          0x03
#define SMBUS_READ_ENABLE  BIT0

#define SMBUS_TIMING_1M_DIVISOR  16

typedef struct  _HYGON_SMBUS_PPI_PRIVATE HYGON_SMBUS_PPI_PRIVATE;

//
typedef
UINT8
(EFIAPI *SMBUS_IO_READ)(
  IN       HYGON_SMBUS_PPI_PRIVATE  *Private,
  IN       UINTN                  Address
  );

typedef
VOID
(EFIAPI *SMBUS_IO_WRITE)(
  IN       HYGON_SMBUS_PPI_PRIVATE  *Private,
  IN       UINTN                  Address,
  IN       UINT8                  Data
  );

typedef
EFI_STATUS
(EFIAPI *SMBUS_STALL)(
  IN       EFI_PEI_SERVICES   **PeiServices,
  IN       EFI_PEI_STALL_PPI      *This,
  IN       UINTN              Microseconds
  );

//
// Module data structure
//
/// Private SMBUS PPI Data Structures
typedef struct _HYGON_SMBUS_PPI_PRIVATE {
  EFI_PEI_SMBUS2_PPI    SmbusPpi;             ///< SMBUS PPI
  EFI_PEI_SERVICES      **PeiServices;        ///< Pointer to PeiServices
  UINTN                 BaseAddress;          ///< SMBUS Base Address
  UINT8                 RegisterIndex;        ///< SMBUS Base Register Index
  SMBUS_IO_READ         IoRead8;              ///< Local Function Pointer
  SMBUS_IO_WRITE        IoWrite8;             ///< Local Function Pointer
} HYGON_SMBUS_PPI_PRIVATE;

/// Private SMBUS Data Block Structure
typedef struct _HYGON_PEI_FCH_SMBUS_PRIVATE {
  HYGON_SMBUS_PPI_PRIVATE    SmbusPpi0;         ///< SMBUS0 Data Block
  HYGON_SMBUS_PPI_PRIVATE    SmbusPpi1;         ///< SMBUS1 Data Block
} HYGON_PEI_FCH_SMBUS_PRIVATE;

#endif // _FCH_SMBUS_H_
