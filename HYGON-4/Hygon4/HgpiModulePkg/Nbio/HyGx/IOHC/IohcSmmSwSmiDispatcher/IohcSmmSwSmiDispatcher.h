/* $NoKeywords:$ */

/**
 * @file
 *
 * IOHC SW SMI Dispatcher Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      Hgpi
 * @e sub-project:  IOHC
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
* ***************************************************************************
*
*/

#ifndef _IOHC_SMM_SW_SMI_DISPATCHER_H_
#define _IOHC_SMM_SW_SMI_DISPATCHER_H_

#include <Protocol/FchSmmApuRasDispatch.h>
#include "Fch.h"
#include <RtcIntrDetBase.h>
#include <Library/RtcIntrDetLib.h>

#ifndef LOCAL_IOD_NBIO_SPACE
  #define  LOCAL_IOD_NBIO_SPACE(RBINDEX, ADDRESS)  (UINT32)((RBINDEX << 20) + ADDRESS)
#endif

#define   NB_RAS_CONTROL_HYGX           0x0802015C
  #define SW_SMI_EN                    BIT16

#define   SW_SMI_CNTL_HYGX              0x0801010C
  #define SMU_HOTPLUG_SMI_FLAG         BIT0
  #define SMU_GET_RTC_RAM_FLAG         BIT1

typedef struct {
  UINTN     SocketId;
  UINTN     LogicalDieId;
  UINTN     RbId;
  UINT32    BusBase;
} NBIO_INFO;

typedef struct {
  UINTN     SwSmiCpuIndex;                   ///< 0-based CPU ID
  UINT8     CommandPort;                     ///< SMI Command port
  UINT8     DataPort;                        ///< SMI Data Port
} IOHC_SMM_SW_CONTEXT;

typedef struct {
  UINT32    SmiStatusBit;                  ///< Status Bit
  UINTN     SocketId;
  UINTN     LogicalDieId;
  UINTN     RbId;
  UINT32    BusBase;
} IOHC_SMM_COMMUNICATION_BUFFER;

typedef EFI_STATUS (EFIAPI *IOHC_SMM_CHILD_DISPATCHER_HANDLER)(
  IN      EFI_HANDLE     DispatchHandle,
  IN OUT  VOID           *CommunicationBuffer OPTIONAL,
  IN OUT  UINTN          *SourceSize OPTIONAL
  );

///
/// IOHC SMM Dispatcher Structure
///
typedef struct {
  UINT32                               SmiStatusBit;    ///< Status Bit
  IOHC_SMM_CHILD_DISPATCHER_HANDLER    SmiDispatcher;   ///< Dispatcher Address
} IOHC_SMM_DISPATCHER_TABLE;

typedef struct {
  UINTN    IohcSwSmiValue;                  ///< SW SMI value
  UINT8    Order;                           ///< Priority 0-Highest (reserved), 0xFF-Lowest (reserved)
} IOHC_SMM_SW_REGISTER_CONTEXT;

typedef
EFI_STATUS
(EFIAPI *IOHC_SMM_SW_HANDLER_ENTRY_POINT)(
  IN       EFI_HANDLE                          DispatchHandle,
  IN CONST IOHC_SMM_SW_REGISTER_CONTEXT        *DispatchContext,
  IN OUT   IOHC_SMM_SW_CONTEXT                 *SwContext,
  IN OUT   UINTN                               *SizeOfSwContext
  );

EFI_STATUS
EFIAPI
IohcSmmNVMeEqDispatchHandler (
  IN       EFI_HANDLE   SmmImageHandle,
  IN OUT   VOID         *CommunicationBuffer OPTIONAL,
  IN OUT   UINTN        *SourceSize OPTIONAL
  );

EFI_STATUS
EFIAPI
SmuGetRtcRamDispatchHandler (
  IN       EFI_HANDLE   DispatchHandle,
  IN OUT   VOID         *CommBuffer OPTIONAL,
  IN OUT   UINTN        *CommBufferSize OPTIONAL
  );

EFI_STATUS
EFIAPI
IohcSwSmiDispatchHandler (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       FCH_SMM_APURAS_REGISTER_CONTEXT   *MiscRegisterContext
  );

VOID
NotifySmuSmiHandlerReady (
  void
  );

#endif
