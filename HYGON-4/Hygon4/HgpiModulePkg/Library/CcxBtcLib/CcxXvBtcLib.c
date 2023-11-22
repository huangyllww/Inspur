/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CCX Boot Time Calibration Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
 *
 */
/*
 ******************************************************************************
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

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "PiDxe.h"
#include <Library/BaseLib.h>
#include <Library/SynchronizationLib.h>
#include "Porting.h"
#include "HYGON.h"
#include "Filecode.h"
#include "CcxRegistersXv.h"
#include "UnbRegisters.h"
#include "Library/HygonBaseLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include <Library/PciLib.h>
#include "Protocol/MpService.h"

#define FILECODE  LIBRARY_CCXBTCLIB_CCXXVBTCLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                          G L O B A L     D A T A
 *----------------------------------------------------------------------------------------
 */
STATIC UINT8      OkToStartTest;
STATIC EFI_EVENT  HygonBtcEvent;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
UINT8
CcxXvBtcGetTargetPstate (
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  );

HGPI_STATUS
CcxXvTransitionPstate (
  IN       UINT8              StateNumber,
  IN       BOOLEAN            WaitForTransition,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
CcxXvBtcAps (
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
*/

/*----------------------------------------------------------------------------------------*/

/**
 * Setup for boot time calibration tests.
 *
 * This procedure will perform the following steps :
 * 1..Set Bsp to P7 (Or Pmin)
 *
 * @param[in]  StdHeader            Standard configuration header
 */
HGPI_STATUS
CcxBtcBefore (
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                     TargetPstate;
  EFI_MP_SERVICES_PROTOCOL  *MpServices;
  EFI_STATUS                Status;

  // Initialize the semaphore.
  OkToStartTest = 0;

  TargetPstate = CcxXvBtcGetTargetPstate (StdHeader);
  CcxXvTransitionPstate (TargetPstate, FALSE, StdHeader);

  Status = gBS->LocateProtocol (&gEfiMpServiceProtocolGuid, NULL, &MpServices);
  if (EFI_ERROR (Status)) {
    return HGPI_ERROR;
  }

  // Create a event so StartupAllAPs executes in non-blocking mode.
  Status = gBS->CreateEvent (
                  0x00000000,
                  0,
                  NULL,
                  NULL,
                  &HygonBtcEvent
                  );
  if (EFI_ERROR (Status)) {
    return HGPI_ERROR;
  }

  Status = MpServices->StartupAllAPs (
                         MpServices,
                         CcxXvBtcAps,
                         FALSE,
                         HygonBtcEvent,
                         0,
                         (VOID *)StdHeader,
                         NULL
                         );

  if (EFI_ERROR (Status)) {
    return HGPI_ERROR;
  }

  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Restore pre workload run conditions
 *
 * @param[in]  StdHeader            Standard configuration header
 */
VOID
CcxBtcAfter (
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  InterlockedIncrement ((UINT32 *)&OkToStartTest);

  CcxXvTransitionPstate (0, FALSE, StdHeader);

  while (gBS->CheckEvent (HygonBtcEvent) == EFI_SUCCESS) {
  }
}

/*---------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/**
 * Return a target Pstate which is required for the CalPgmIndex
 *
 * @param[in]  StdHeader        Standard configuration header
 *
 * @return                      Target Pstate which is required for the CalPgmIndex
 */
UINT8
CcxXvBtcGetTargetPstate (
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8                          TargetPstate;
  UINTN                          PciAddress;
  CPB_CTRL_REGISTER              CpbCtrl;
  CLK_PWR_TIMING_CTRL2_REGISTER  Cptc2;

  TargetPstate = 7;
  PciAddress   = CPTC2_PCI_ADDR;
  *((UINT32 *)&Cptc2) = PciRead32 (PciAddress);
  if (TargetPstate > (UINT8)Cptc2.HwPstateMaxVal) {
    TargetPstate = (UINT8)Cptc2.HwPstateMaxVal;
  }

  PciAddress = CPB_CTRL_PCI_ADDR;
  *((UINT32 *)&CpbCtrl) = PciRead32 (PciAddress);
  TargetPstate = TargetPstate - (UINT8)CpbCtrl.NumBoostStates;

  return (TargetPstate);
}

/*---------------------------------------------------------------------------------------*/

/**
 *  Transitions the executing core to the desired P-state.
 *
 *  @param[in]   StateNumber              The new P-State to make effective.
 *  @param[in]   WaitForTransition        True if the caller wants the transition completed upon return.
 *  @param[in]   StdHeader                Header for library and services
 *
 *  @retval      HGPI_SUCCESS       Always Succeeds
 */
HGPI_STATUS
CcxXvTransitionPstate (
  IN       UINT8              StateNumber,
  IN       BOOLEAN            WaitForTransition,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT64  LocalMsrRegister;

  LocalMsrRegister = AsmReadMsr64 (MSR_PSTATE_CTL);
  ((PSTATE_CTRL_MSR *)&LocalMsrRegister)->PstateCmd = (UINT64)StateNumber;
  AsmWriteMsr64 (MSR_PSTATE_CTL, LocalMsrRegister);
  if (WaitForTransition) {
    do {
      LocalMsrRegister = AsmReadMsr64 (MSR_PSTATE_STS);
    } while (((PSTATE_STS_MSR *)&LocalMsrRegister)->CurPstate != (UINT64)StateNumber);
  }

  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Work procedure for the Aps.
 *
 * This procedure will perform the following steps :
 * 1..Set APs to P7 (Or Pmin)
 * 2..Wait for semaphore to flag run tests.
 * 3..Transfer to a target Pstate
 * 4..If tests completed, wait for semaphore to signal transition Pstate and exit, else...
 *    ... wait for semaphore to signal run next workload.
 *
 *  @param[in]   StdHeader                Header for library and services
 */
VOID
CcxXvBtcAps (
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  TargetPstate;

  TargetPstate = CcxXvBtcGetTargetPstate (StdHeader);
  CcxXvTransitionPstate (TargetPstate, TRUE, StdHeader);

  while (OkToStartTest == 0) {
    CpuPause ();
  }

  CcxXvTransitionPstate (0, FALSE, StdHeader);

  return;
}
