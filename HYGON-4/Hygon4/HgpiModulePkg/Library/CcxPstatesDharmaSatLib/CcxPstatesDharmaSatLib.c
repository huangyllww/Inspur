/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Ccx Pstates Library
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
#include <Library/BaseLib.h>
#include "HGPI.h"
#include "cpuRegisters.h"
#include "CcxRegistersDm.h"
#include "Filecode.h"
#include "Library/HygonBaseLib.h"
#include "Library/CcxPstatesLib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE  LIBRARY_CCXPSTATESDHARMASATLIB_CCXPSTATESDHARMASATLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxGetPstateInfo
 *
 *    Get Pstate information
 *
 *    @param[in]      Socket         Zero-based socket number to check
 *    @param[in]      Pstate         Which P-state to analyze
 *    @param[in]      Frequency      The P-State's frequency in MegaHertz
 *    @param[in]      VoltageInuV    The P-State's voltage in uV
 *    @param[in]      PowerInmW      The Power in milliWatts of that P-State
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 *    @retval         TRUE           Get Pstate information successful.
 *    @retval         FALSE          Pstate is disabled
 *
 */
BOOLEAN
CcxGetPstateInfo (
  IN       UINTN               Socket,
  IN       CCX_PSTATE          Pstate,
  OUT   UINTN              *Frequency,
  OUT   UINTN              *VoltageInuV,
  OUT   UINTN              *PowerInmW,
  IN       HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32      CpuFid;
  UINT32      CpuDfsId;
  UINT32      CpuVid;
  UINT32      IddValue;
  UINT32      IddDiv;
  UINT32      PstateNumber;
  PSTATE_MSR  PstateMsr;

  PstateNumber    = Pstate < HwPstate0 ? (UINT32)Pstate : (UINT32)Pstate - HwPstate0;
  PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + PstateNumber);

  if (PstateMsr.Field.PstateEn == 0) {
    return FALSE;
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "  F18GetPstateFrequency - P%d\n", PstateNumber);

  CpuFid   = (UINT32)PstateMsr.Field.CpuFid_7_0;
  CpuDfsId = (UINT32)PstateMsr.Field.CpuDfsId;
  CpuVid   = (UINT32)PstateMsr.Field.CpuVid;
  IddValue = (UINT32)PstateMsr.Field.IddValue;
  IddDiv   = (UINT32)PstateMsr.Field.IddDiv;

  ASSERT ((CpuDfsId <= 0x1A) || ((CpuDfsId & BIT0) == 0));

  if (CpuDfsId == 0) {
    *Frequency = 0;
  } else if ((CpuDfsId >= 8) && (CpuDfsId <= 0x3C)) {
    *Frequency = (UINTN)((200 * CpuFid) / CpuDfsId);
  } else {
    IDS_HDT_CONSOLE (CPU_TRACE, "    Undefined CpuDfsId %d used. Force it to div 1.\n", CpuDfsId);
    ASSERT (FALSE);
    *Frequency = (UINTN)(25 * CpuFid);
  }

  IDS_HDT_CONSOLE (CPU_TRACE, "    FrequencyInMHz=%d, CpuFid=%d, CpuDfsId=%d\n", *Frequency, CpuFid, CpuDfsId);

  // Voltage
  if ((CpuVid >= 0xF8) && (CpuVid <= 0xFF)) {
    *VoltageInuV = 0;
  } else {
    *VoltageInuV = 1550000L - (6250 * CpuVid);
  }

  // PowerInmW
  *PowerInmW = (*VoltageInuV) / 10 * IddValue;

  switch (IddDiv) {
    case 0:
      *PowerInmW = *PowerInmW / 100L;
      break;
    case 1:
      *PowerInmW = *PowerInmW / 1000L;
      break;
    case 2:
      *PowerInmW = *PowerInmW / 10000L;
      break;
    default:
      // IddDiv is set to an undefined value.  This is due to either a misfused CPU, or
      // an invalid P-state MSR write.
      ASSERT (FALSE);
      *PowerInmW = 0;
      break;
  }

  return TRUE;
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxGetPstateNumber
 *
 *    Get number of Pstate
 *
 *    @param[in]      Socket               Zero-based socket number to check
 *    @param[in]      NumberOfBoostPstate  Number of boost Pstate
 *    @param[in]      NumberOfHwPstate     Number of hardware Pstate
 *    @param[in]      StdHeader            Standard Head Pointer
 *
 */
VOID
CcxGetPstateNumber (
  IN       UINTN               Socket,
  OUT   UINTN              *NumberOfBoostPstate,
  OUT   UINTN              *NumberOfHwPstate,
  IN       HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  PSTATE_CURLIM_MSR  CurLim;

  *NumberOfBoostPstate = 0;

  CurLim.Value = AsmReadMsr64 (MSR_PSTATE_CURRENT_LIMIT);
  *NumberOfHwPstate = (UINTN)CurLim.Field.PstateMaxVal;
}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxSwitchPstate
 *
 *    Switch to a certain Pstate
 *
 *    @param[in]      SwitchPstate            Target Pstate & wait for completion
 *
 */
VOID
CcxSwitchPstate (
  IN       CCX_SWITCH_PSTATE *SwitchPstate
  )
{
  PSTATE_MSR            PstateMsr;

  PstateMsr.Value = AsmReadMsr64 (MSR_PSTATE_0 + (UINT32)(SwitchPstate->Pstate));
  if (PstateMsr.Field.PstateEn == 0) {
    ASSERT (FALSE);
    return;
  }

  AsmWriteMsr64 (MSR_PSTATE_CTL, SwitchPstate->Pstate);

}

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxGetCurrentPstate
 *
 *    Get current Pstate
 *
 *    @retval  UINT8    current Pstate
 *
 */
UINT8
CcxGetCurrentPstate (
  )
{
  PSTATE_STS_MSR  PstateSts;

  PstateSts.Value = AsmReadMsr64 (MSR_PSTATE_STS);
  return (UINT8)PstateSts.Field.CurPstate;
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */
