/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH SMM Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
 *
 */
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

#ifndef _FCH_SMM_DRIVER_H_
#define _FCH_SMM_DRIVER_H_

#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Protocol/SmmBase2.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/FchSmmSxDispatch2.h>
#include <Protocol/FchSmmPwrBtnDispatch2.h>
#include <Protocol/FchSmmPeriodicalDispatch2.h>
#include <Protocol/FchSmmGpiDispatch2.h>
#include <Protocol/FchSmmMiscDispatch.h>
#include <Protocol/FchSmmIoTrapDispatch2.h>
#include <Protocol/FchSmmUsbDispatch2.h>
#include <Protocol/FchInitProtocol.h>
#include <Protocol/FchSmmInitProtocol.h>

#include <Library/FchSmmLib.h>
#include "FchPlatform.h"

EFI_STATUS
EFIAPI
AmdSmiAcpiOnCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT       *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  );

EFI_STATUS
EFIAPI
AmdSmiAcpiOnCallback2 (
  IN  EFI_HANDLE                       DispatchHandle,
  IN  FCH_SMM_SW_REGISTER_CONTEXT      *DispatchContext,
  IN  FCH_SMM_SW_CONTEXT               *EfiSmmSwContext,
  IN  UINTN                            *SizeOfEfiSmmSwContext
  );

EFI_STATUS
EFIAPI
AmdSmiAcpiOffCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT       *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  );

#ifdef FCH_SPI_PROTECT_SMI
  EFI_STATUS
  EFIAPI
  AmdSmiSpiUnlockCallback (
    IN       EFI_HANDLE                        DispatchHandle,
    IN CONST FCH_SMM_SW_REGISTER_CONTEXT       *DispatchContext,
    IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
    IN OUT   UINTN                             *SizeOfSwContext
  );

  EFI_STATUS
  EFIAPI
  AmdSmiSpiLockCallback (
    IN       EFI_HANDLE                        DispatchHandle,
    IN CONST FCH_SMM_SW_REGISTER_CONTEXT       *DispatchContext,
    IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
    IN OUT   UINTN                             *SizeOfSwContext
  );
#endif

VOID
FchSmmAcpiOn (
  IN      FCH_DATA_BLOCK   *FchDataPtr
  );

VOID
FchInitS3EarlyRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  );

VOID
FchInitS3LateRestore (
  IN      FCH_DATA_BLOCK     *FchDataPtr
  );

EFI_STATUS
EFIAPI
AmdSmiBeforePciS3RestoreCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT       *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
);

EFI_STATUS
EFIAPI
AmdSmiAfterPciS3RestoreCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN CONST FCH_SMM_SW_REGISTER_CONTEXT       *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
);

EFI_STATUS
EFIAPI
AmdSmiPowerButtonCallback (
  IN       EFI_HANDLE                       DispatchHandle,
  IN CONST FCH_SMM_PWRBTN_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                             *CommBuffer OPTIONAL,
  IN OUT   UINTN                            *CommBufferSize  OPTIONAL
  );

EFI_STATUS
EFIAPI
AmdSmiPowerButtonCallbackTest (
  IN       EFI_HANDLE                       DispatchHandle,
  IN CONST FCH_SMM_PWRBTN_REGISTER_CONTEXT  *DispatchContext,
  IN OUT   VOID                             *CommBuffer OPTIONAL,
  IN OUT   UINTN                            *CommBufferSize  OPTIONAL
  );

EFI_STATUS
EFIAPI
AmdSmiS3SleepEntryCallback (
  IN       EFI_HANDLE                       DispatchHandle,
  IN CONST FCH_SMM_SX_REGISTER_CONTEXT      *DispatchContext,
  IN OUT   VOID                             *CommBuffer OPTIONAL,
  IN OUT   UINTN                            *CommBufferSize  OPTIONAL
  );

EFI_STATUS
EFIAPI
AmdSmiS4SleepEntryCallback (
  IN       EFI_HANDLE                       DispatchHandle,
  IN CONST FCH_SMM_SX_REGISTER_CONTEXT      *DispatchContext,
  IN OUT   VOID                             *CommBuffer OPTIONAL,
  IN OUT   UINTN                            *CommBufferSize  OPTIONAL
  );

EFI_STATUS
EFIAPI
AmdSmiS4SleepEntryCallback2 (
  IN  EFI_HANDLE                       DispatchHandle,
  IN  FCH_SMM_SX_REGISTER_CONTEXT      *DispatchContext,
  IN OUT   VOID                        *CommBuffer,
  IN OUT   UINTN                       *CommBufferSize
  );

EFI_STATUS
EFIAPI
AmdSmiS5SleepEntryCallback (
  IN       EFI_HANDLE                       DispatchHandle,
  IN CONST FCH_SMM_SX_REGISTER_CONTEXT      *DispatchContext,
  IN OUT   VOID                             *CommBuffer OPTIONAL,
  IN OUT   UINTN                            *CommBufferSize  OPTIONAL
  );


EFI_STATUS
EFIAPI
AmdMiscFchGppHpSmiCallback (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  FCH_SMM_MISC_REGISTER_CONTEXT     *DispatchContext
  );

#ifdef FCH_TIMER_TICK_INTERVAL_WA
  EFI_STATUS
  EFIAPI
  AmdMiscFchHpetIntervalCallback (
    IN  EFI_HANDLE                        DispatchHandle,
    IN  FCH_SMM_MISC_REGISTER_CONTEXT     *DispatchContext
  );

  EFI_STATUS
  EFIAPI
  AmdMiscFchIrq2TrapCallback (
    IN  EFI_HANDLE                        DispatchHandle,
    IN  FCH_SMM_MISC_REGISTER_CONTEXT     *DispatchContext
  );
#endif


EFI_STATUS
EFIAPI
FchUsbBeforeOCcallback (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  FCH_SMM_USB_REGISTER_CONTEXT      *DispatchContext
  );

EFI_STATUS
FchSmmRegisterSwSmi (
  VOID
  );

EFI_STATUS
FchSmmRegisterPeriodicTimerSmi (
  VOID
  );

EFI_STATUS
FchSmmRegisterGpiSmi (
  VOID
  );

EFI_STATUS
FchSmmRegisterPowerButtonSmi (
  VOID
  );

EFI_STATUS
FchSmmRegisterSxSmi (
  VOID
  );

VOID
FchEnableSleepTransition (
  VOID
  );

EFI_STATUS
FchSmmRegisterMiscSmi (
  VOID
  );

EFI_STATUS
FchSmmRegisterIoTrapSmi (
  VOID
  );

EFI_STATUS
RegisterUsbPatchSmi (
  VOID
  );

EFI_STATUS
FchSmmRegisterEhciDebugport (
  VOID
  );

typedef struct _FCH_MEM_BACKUP{
    UINT64          Address;
    UINT32          Data;
} FCH_MEM_BACKUP;

#endif
