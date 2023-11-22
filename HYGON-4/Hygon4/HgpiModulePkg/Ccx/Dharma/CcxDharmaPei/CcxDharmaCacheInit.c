/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CCX Dharma - Satori API, and related functions.
 *
 * Contains code that initializes the core complex
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  CCX
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
/*++
Module Name:

  CcxDharmaSatCacheInit.c
  Initialize Cache

Abstract:
--*/

#include <Library/BaseLib.h>
#include <HGPI.h>
#include <Filecode.h>
#include <Library/HygonBaseLib.h>
#include <Library/CcxBaseX86Lib.h>

#define FILECODE  CCX_DHARMA_CCXDHARMAPEI_CCXDHARMACACHEINIT_FILECODE

/* -----------------------------------------------------------------------------*/

/**
 *
 *
 * Program MTRRs to describe given range as given cache type.  Use MTRR pairs
 * starting with the given MTRRphys Base address, and use as many as is
 * required up to (excluding) MSR 020C, which is reserved for OS.
 *
 * "Limit" in the context of this procedure is not the numerically correct
 * limit, but rather the Last address+1, for purposes of coding efficiency
 * and readability.  Size of a region is then Limit-Base.
 *
 * 1. Size of each range must be a power of two
 * 2. Each range must be naturally aligned (Base is same as size)
 *
 * There are two code paths: the ascending path and descending path (analogous
 * to bsf and bsr), where the next limit is a function of the next set bit in
 * a forward or backward sequence of bits (as a function of the Limit).  We
 * start with the ascending path, to ensure that regions are naturally aligned,
 * then we switch to the descending path to maximize MTRR usage efficiency.
 * Base=0 is a special case where we start with the descending path.
 * Correct Mask for region is 2comp(Size-1)-1,
 * which is 2comp(Limit-Base-1)-1 *
 * @param[in]     Base - Base address[47:16] of specified range.
 * @param[in]     *LimitPtr - Limit address[47:16] of specified range.
 * @param[in]     MtrrAddr - address of var MTRR pair to start using.
 * @param[in]     MtrrType - Cache type for the range.
 * @param[in]  StdHeader         Config handle for library and services
 *
 *
 * @retval     HGPI_STATUS
 */
HGPI_STATUS
SetVariableMTRRs (
  IN       UINT32            Base,
  IN OUT   UINT32            *LimitPtr,
  IN       UINT32            MtrrAddr,
  IN       UINT8             MtrrType,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT64  LocalMsrRegister;
  UINT32  CurBase;
  UINT32  CurLimit;
  UINT32  CurSize;
  UINT32  CurAddr;
  UINT32  Value32;

  CurBase  = Base;
  CurLimit = *LimitPtr;
  CurAddr  = MtrrAddr;

  while ((CurAddr >= 0x200) && (CurAddr < 0x20C) && (CurBase < *LimitPtr)) {
    CurSize   = CurLimit = (UINT32)1 << LowBitSet32 (CurBase);
    CurLimit += CurBase;
    if ((CurBase == 0) || (*LimitPtr < CurLimit)) {
      CurLimit  = *LimitPtr - CurBase;
      CurSize   = CurLimit = (UINT32)1 << HighBitSet32 (CurLimit);
      CurLimit += CurBase;
    }

    // prog. MTRR with current region Base
    LocalMsrRegister = (CurBase << 16) | (UINT32)MtrrType;
    AsmWriteMsr64 (CurAddr, LocalMsrRegister);

    // prog. MTRR with current region Mask
    CurAddr++;                              // other half of MSR pair
    Value32 = CurSize - (UINT32)1;
    Value32 = ~Value32;
    LocalMsrRegister = 0xFFFF00000000ull | (Value32 << 16) | ((UINT32)1 << 11);
    AsmWriteMsr64 (CurAddr, LocalMsrRegister);

    CurBase = CurLimit;
    CurAddr++;                              // next MSR pair
  }

  if (CurLimit < *LimitPtr) {
    // Announce failure
    *LimitPtr = CurLimit;
    IDS_ERROR_TRAP;
  }

  LocalMsrRegister = 0;
  while ((CurAddr >= 0x200) && (CurAddr < 0x20C)) {
    AsmWriteMsr64 (CurAddr, LocalMsrRegister);
    CurAddr++;
  }

  return HGPI_SUCCESS;
}

/*++

Routine Description:

  Dharma - Satori Microcode Patch loading

Arguments:
  HYGON_CONFIG_PARAMS *

Returns:

--*/
VOID
CcxDharmaCacheInit (
  IN       HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32       Cache32bTOP;
  UINT64       LocalMsrRegister;
  HGPI_STATUS  Status;

  LocalMsrRegister = AsmReadMsr64 (0xC001001A);

  Cache32bTOP = (UINT32)(RShiftU64 (LocalMsrRegister, 16) & 0xFFFFFFFF);

  //
  // ======================================================================
  // Set default values for CPU registers
  // ======================================================================
  //
  AsmMsrOr64 (0xC0010010, BIT19);

  LocalMsrRegister = 0x1E1E1E1E1E1E1E1Eull;
  AsmWriteMsr64 (0x250, LocalMsrRegister);      // 0 - 512K = WB Mem
  LocalMsrRegister = 0x1E1E1E1E1E1E1E1Eull;
  AsmWriteMsr64 (0x258, LocalMsrRegister);      // 512K - 640K = WB Mem

  //
  // ======================================================================
  // Set variable MTRR values
  // ======================================================================
  //
  Status = SetVariableMTRRs (0, &Cache32bTOP, 0x200, 6, StdHeader);

  AsmMsrOr64 (0xC0010010, (BIT18 | BIT20));

  LocalMsrRegister = AsmReadMsr64 (0xC0010010);
  if ((LocalMsrRegister & BIT21) != 0) {
    LocalMsrRegister |= BIT22;
  }

  LocalMsrRegister &= ~BIT19;
  AsmWriteMsr64 (0xC0010010, LocalMsrRegister);
}
