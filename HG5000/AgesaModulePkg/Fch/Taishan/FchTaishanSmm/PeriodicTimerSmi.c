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
#include "FchSmm.h"
#define FILECODE UEFI_SMM_FCHSMM_PERIODICTIMERSMI_FILECODE

extern FCH_DATA_BLOCK                      gFchDataInSmm;

UINT32 mShortTimerCounter;
UINT32 mLongTimerCounter;

/*++

Routine Description:
  Handler for LongTimer SMI

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  RegisterContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdSmiLongTimerCallback (
  IN       EFI_HANDLE                                DispatchHandle,
  IN       CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT *RegisterContext,
  IN OUT   EFI_SMM_PERIODIC_TIMER_CONTEXT            *PeriodicTimerContext,
  IN OUT   UINTN                                     *SizeOfContext
  )
{
  //LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, 0x80, &mLongTimerCounter);
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &mLongTimerCounter);
  mLongTimerCounter++;
  return EFI_SUCCESS;
}


/*++

Routine Description:
  Handler for ShortTimer SMI

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  RegisterContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdSmiShortTimerCallback (
  IN       EFI_HANDLE                                DispatchHandle,
  IN       CONST FCH_SMM_PERIODICAL_REGISTER_CONTEXT *RegisterContext,
  IN OUT   EFI_SMM_PERIODIC_TIMER_CONTEXT            *PeriodicTimerContext,
  IN OUT   UINTN                                     *SizeOfContext
  )
{
  //LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, 0x80, &mShortTimerCounter);
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &mShortTimerCounter);
  mShortTimerCounter++;
  return EFI_SUCCESS;
}


EFI_STATUS
FchSmmRegisterPeriodicTimerSmi (
  VOID
  )
{
  EFI_STATUS                              Status;
  FCH_SMM_PERIODICAL_DISPATCH2_PROTOCOL   *AmdPeriodicalDispatch;
  FCH_SMM_PERIODICAL_REGISTER_CONTEXT     PeriodicalRegisterContext;
  EFI_HANDLE                              PtHandle;
  FCH_MISC                                *FchMisc;

  FchMisc = &gFchDataInSmm.Misc;

  //
  // Periodic Timer SMI Registration
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmPeriodicalDispatch2ProtocolGuid,
                  NULL,
                  &AmdPeriodicalDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Due to the post code limitation, we choose to turn on LongTimer and ShortTimer SMIs exclusively.
  // If both are set to enable in setup, we only register the LongTimer SMI.
  //
  if (FchMisc->LongTimer.Enable) {
    PeriodicalRegisterContext.SmiTickInterval  = LONG_TIMER_SMI_INTERVAL;
    PeriodicalRegisterContext.Period           = FchMisc->LongTimer.CycleDuration * LONG_TIMER_SMI_INTERVAL;
    PeriodicalRegisterContext.StartNow         = FchMisc->LongTimer.StartNow;
    mLongTimerCounter = 0;
    Status = AmdPeriodicalDispatch->Register (
                                      AmdPeriodicalDispatch,
                                      AmdSmiLongTimerCallback,
                                      &PeriodicalRegisterContext,
                                      &PtHandle
                                      );
  } else if (FchMisc->ShortTimer.Enable) {
    PeriodicalRegisterContext.SmiTickInterval  = SHORT_TIMER_SMI_INTERVAL;
    PeriodicalRegisterContext.Period           = (FchMisc->ShortTimer.CycleDuration >> 1) * SHORT_TIMER_SMI_INTERVAL;
    PeriodicalRegisterContext.StartNow         = FchMisc->ShortTimer.StartNow;
    mShortTimerCounter = 0;
    Status = AmdPeriodicalDispatch->Register (
                                      AmdPeriodicalDispatch,
                                      AmdSmiShortTimerCallback,
                                      &PeriodicalRegisterContext,
                                      &PtHandle
                                      );
  }
  return Status;
}


