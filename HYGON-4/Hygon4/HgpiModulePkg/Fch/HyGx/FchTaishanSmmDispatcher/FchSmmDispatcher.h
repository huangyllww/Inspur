/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Hudson-2 SMI Dispatcher Driver Example
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  UEFI
 *
 */
/******************************************************************************
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
****************************************************************************/

#ifndef _FCH_SMM_DISPATCHER_H_
#define _FCH_SMM_DISPATCHER_H_

#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#include <Protocol/SmmCpu.h>
#include <Protocol/FchSmmSxDispatch2.h>
#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/FchSmmPwrBtnDispatch2.h>
#include <Protocol/FchSmmPeriodicalDispatch2.h>
#include <Protocol/FchSmmIoTrapDispatch2.h>
#include <Protocol/FchSmmGpiDispatch2.h>
#include <Protocol/FchSmmUsbDispatch2.h>

#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmSxDispatch2.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <Protocol/SmmPeriodicTimerDispatch2.h>
#include <Protocol/SmmUsbDispatch2.h>
#include <Protocol/SmmGpiDispatch2.h>
#include <Protocol/SmmIoTrapDispatch2.h>

#include "FchSmmSxDispatcher.h"
#include "FchSmmSwDispatcher.h"
#include "FchSmmPwrBtnDispatcher.h"
#include "FchSmmIoTrapDispatcher.h"
#include "FchSmmPeriodicalDispatcher.h"
#include "FchSmmGpiDispatcher.h"
#include "FchSmmUsbDispatcher.h"
#include "FchSmmMiscDispatcher.h"
#include "FchSmmApuRasDispatcher.h"

#include "FchPlatform.h"
#pragma pack (8)

#define Slp_Type     BIT1
#define SmiCmdPort   BIT11
#define UsbSmi       BIT21
#define PwrBtn       BIT19
#define Eos          BIT28
#define IoTrapping0  BIT20
#define IoTrapping1  BIT21
#define IoTrapping2  BIT22
#define IoTrapping3  BIT23
#define ShortTimer   BIT14
#define LongTimer    BIT15
#define Gpe          0xfffffffful
#define Smbus0       BIT15
#define AllGevents   0xfffffffful
#define FchGppSerr0  BIT21
#define FchGppSerr1  BIT22
#define FchGppSerr2  BIT23
#define FchGppSerr3  BIT24
#define FchFakeSts0  BIT1
#define ApuRasSmi    BIT21

typedef EFI_STATUS (EFIAPI *HYGON_SM_SMM_CHILD_DISPATCHER_HANDLER)(
  IN      EFI_HANDLE     SmmImageHandle,
  IN OUT  VOID           *CommunicationBuffer OPTIONAL,
  IN OUT  UINTN          *SourceSize OPTIONAL
  );

///
/// HYGON FCH SMM Dispatcher Structure
///
typedef struct {
  UINT32    SmiStatusReg;                               ///< Status Register
  UINT32    SmiStatusBit;                               ///< Status Bit
} FCH_SMM_COMMUNICATION_BUFFER;

///
/// HYGON FCH SMM Dispatcher Structure
///
typedef struct {
  UINT32                                   StatusReg;     ///< Status Register
  UINT32                                   SmiStatusBit;  ///< Status Bit
  HYGON_SM_SMM_CHILD_DISPATCHER_HANDLER    SmiDispatcher; ///< Dispatcher Address
} FCH_SMM_DISPATCHER_TABLE;

///
/// HYGON FCH SMM Dispatcher Structure
///
typedef struct {
  UINT16              ioPort;                           ///< I/O port
  EFI_SMM_IO_WIDTH    ioWidth;                          ///< I/O width
  UINT32              ioValue;                          ///< I/O value
} SAVE_B2B_IO;

///
/// HYGON Save/Restore Structure for SMM
///
typedef struct {
  UINT8               Bus;                              ///< PCI Bus
  UINT8               Dev;                              ///< PCI Device
  UINT8               Func;                             ///< PCI Function
  UINT16              Offset;                           ///< Offset registers
  EFI_SMM_IO_WIDTH    DataWidth;                        ///< Data width
  UINT32              DataValue;                        ///< Data value
} SAVE_PCI;

typedef struct {
  UINT8              RbBusNum;                         ///< Bus Number to access SMN MMIO
  BOOLEAN            XhciEnable;
} FCH_USB_SMI_SYSINFO;

#endif
