/**
 * @file
 *
 * HYGON Integrated Debug Debug_library Routines
 *
 * Contains all functions related to Redirect IO
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  IDS
 */
/*****************************************************************************
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
#include <Uefi.h>
#include <Pi/PiMultiPhase.h>
#include <Library/BaseLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/HygonIdsDebugPrintLib.h>
#include <HygonIdsDebugPrintLocal.h>
#include <Filecode.h>

#define FILECODE  LIBRARY_HYGONIDSDEBUGPRINTLIB_HYGONIDSDPREDIRECTIO_FILECODE

/**
 *  Determine whether IDS console is enabled.
 *
 *
 *  @retval       TRUE    Alway return true
 *
 **/
BOOLEAN
HygonIdsDpRedirectIoSupport (
  VOID
  )
{
  return PcdGetBool (PcdHygonIdsDebugPrintRedirectIOEnable);
}

VOID
HygonIdsDpRedirectIoConstructor (
  VOID
  )
{
  return;
}

/**
 *  Get customize Filter
 *
 *  @param[in,out] Filter    Filter do be filled
 *
 *  @retval       FALSE    Alway return FALSE
 *
 **/
BOOLEAN
HygonIdsDpRedirectIoGetFilter (
  IN OUT   UINT64 *Filter
  )
{
  return FALSE;
}

#define REDIRECT_IO_DATA_BEGIN  0x5f535452ul
#define REDIRECT_IO_DATA_END    0x5f454e44ul
#define BIOS_SCRATCH_MSR_REG    0xC001100Cul

/**
 *  Print formated string with redirect IO
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *
**/
VOID
HygonIdsDpRedirectIoPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize
  )
{
  UINT32  Value;

  if (PcdGet8 (PcdBiosLogPrint) == 2) {
    // Print message via MSR register
    Value = REDIRECT_IO_DATA_BEGIN;
    AsmWriteMsr64 (BIOS_SCRATCH_MSR_REG, Value);

    while (BufferSize--) {
      AsmWriteMsr64 (BIOS_SCRATCH_MSR_REG, *Buffer++);
    }

    Value = REDIRECT_IO_DATA_END;
    AsmWriteMsr64 (BIOS_SCRATCH_MSR_REG, Value);
  } else {
    Value = REDIRECT_IO_DATA_BEGIN;
    LibHygonIoWrite (AccessWidth32, PcdGet16 (PcdIdsDebugPort), &Value, NULL);

    while (BufferSize--) {
      LibHygonIoWrite (AccessWidth8, PcdGet16 (PcdIdsDebugPort), Buffer++, NULL);
    }

    Value = REDIRECT_IO_DATA_END;
    LibHygonIoWrite (AccessWidth32, PcdGet16 (PcdIdsDebugPort), &Value, NULL);
  }
}

CONST HYGON_IDS_DEBUG_PRINT_INSTANCE  HygonIdsDebugPrintRedirectIoInstance =
{
  HygonIdsDpRedirectIoSupport,
  HygonIdsDpRedirectIoConstructor,
  HygonIdsDpRedirectIoGetFilter,
  HygonIdsDpRedirectIoPrint
};
