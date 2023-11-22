/**
 * @file
 *
 * HYGON Integrated Debug Debug_library Routines
 *
 * Contains all functions related to HDTOUT
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
#include "HGPI.h"
#include "Library/IdsLib.h"
#include "hygonlib.h"
#include "HYGON.h"
#include "IdsDebugPrint.h"
#include "IdsDpRam.h"
#include "Filecode.h"

#define FILECODE  LIBRARY_HYGONIDSDEBUGPRINTLIB_HYGONIDSDPRAM_FILECODE

/**
 *  Determine whether IDS console is enabled.
 *
 *
 *  @retval       TRUE    Alway return true
 *
 **/
BOOLEAN
HygonIdsDpRamSupport (
  VOID
  )
{
  return TRUE;
}

/**
 * Read Debug Print Memory
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Value         Pointer to data
 *
 */
VOID
STATIC
HygonIdsDpRamRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  OUT   VOID *Value
  )
{
  LibHygonMemRead (AccessWidth, MemAddress, Value, NULL);
}

/**
 * Write Debug Print Memory
 *
 *
 * @param[in] AccessWidth   Access width
 * @param[in] MemAddress    Memory address
 * @param[in] Value         Pointer to data
 *
 */
VOID
STATIC
HygonIdsDpRamWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  OUT   VOID *Value
  )
{
  LibHygonMemWrite (AccessWidth, MemAddress, Value, NULL);
}

/**
 *  get debug print latest Index
 *
 *  @param[in,out] LatestIndex  - LatestIndex to be filled
 *
 *  @retval       HGPI_ERROR    Debug Ram region is read only
 *  @retval       HGPI_SUCCESS  Successfully get the Latest Index

 *
**/
HGPI_STATUS
STATIC
HygonIdsDpRamGetLatestIndex (
  IN OUT   UINT32 *LatestIndex
  )
{
  UINT32     i;
  UINT32     _LatestIndex;
  SIGNATURE  DpRamSig;
  UINT8      DpBufferDftValue;

  ASSERT (LatestIndex != NULL);
  ASSERT (IDS_DPRAM_BASE != 0);
  HygonIdsDpRamRead (AccessWidth32, DEBUG_PRINT_RAM_SIG_ADDR, &DpRamSig);

  // Check Signature, if uninit means it is 1st time comes here
  if (DpRamSig != DEBUG_PRINT_RAM_SIG) {
    // Init Debug Print RAM Header
    DpRamSig = DEBUG_PRINT_RAM_SIG;
    HygonIdsDpRamWrite (AccessWidth32, DEBUG_PRINT_RAM_SIG_ADDR, &DpRamSig);
    // Check if read only memory
    DpRamSig = 0;
    HygonIdsDpRamRead (AccessWidth32, DEBUG_PRINT_RAM_SIG_ADDR, &DpRamSig);
    if (DpRamSig != DEBUG_PRINT_RAM_SIG) {
      return HGPI_ERROR;
    }

    // Init Latest Index with zero
    _LatestIndex = 0;
    HygonIdsDpRamWrite (AccessWidth32, DEBUG_PRINT_RAM_LATESTIDX_ADDR, &_LatestIndex);
    // Init Debug Print Buffer with defalut value
    ASSERT (IDS_DPRAM_SIZE != 0);
    DpBufferDftValue = DEBUG_PRINT_BUFFER_DFT_VALUE;
    for (i = 0; i < DEBUG_PRINT_BUFFER_SIZE; i++) {
      HygonIdsDpRamWrite (AccessWidth8, DEBUG_PRINT_BUFFER_START + i, &DpBufferDftValue);
    }
  }

  HygonIdsDpRamRead (AccessWidth32, DEBUG_PRINT_RAM_LATESTIDX_ADDR, LatestIndex);
  return HGPI_SUCCESS;
}

/**
 *  Get DpRam customize Filter
 *
 *  @param[in,out] Filter    Filter do be filled
 *
 *  @retval       FALSE    Alway return FALSE
 *
 **/
BOOLEAN
HygonIdsDpRamGetFilter (
  IN OUT   UINT64 *Filter
  )
{
  return FALSE;
}

/**
 *  Init local private data
 *
 *  @param[in] Flag    - filter flag
 *  @param[in] debugPrintPrivate  - Point to debugPrintPrivate
 *
**/
VOID
HygonIdsDpRamInitPrivateData (
  IN      UINT64 Flag,
  IN      IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate
  )
{
}

/**
 *  Get a record's size which end of 0x00
 *
 *  @param[in] RecordAddr  - The address of a record
 *
 *  @retval    RecordSize    Size of a record
**/
UINT32
STATIC
HygonIdsDpRamGetRecordSize (
  IN      UINT64 RecordAddr
  )
{
  UINT32  RecordSize;
  UINT8   BufValue8;

  RecordSize = 0;
  do {
    HygonIdsDpRamRead (AccessWidth8, RecordAddr++, &BufValue8);
    RecordSize++;
  } while (BufValue8 != DEBUG_STRING_END_FLAG);

  return RecordSize;
}

/**
 *  Clean up the Debug Print buffer, remove oldest record, do the relocation
 *
 *  @param[in] NewRecordLength  - The Length of new record
 *  @param[in,out] LatestIndex  - LatestIndex to be Updated
 *
**/
VOID
STATIC
HygonIdsDpRamCleanUp (
  IN       UINTN NewRecordLength,
  IN OUT   UINT32 *LatestIndex
  )
{
  UINT32  RemainedSize;
  UINT32  RecordOffset;
  UINT32  RecordSize;
  UINT8   Value8;
  UINT32  i;

  RecordOffset = 0;
  RemainedSize = DEBUG_PRINT_BUFFER_SIZE - *LatestIndex;
  // Calculate How many record need be removed
  while (RemainedSize < NewRecordLength) {
    RecordSize    = HygonIdsDpRamGetRecordSize (DEBUG_PRINT_BUFFER_START + RecordOffset);
    RecordOffset += RecordSize;
    RemainedSize += RecordSize;
  }

  // Move forward (RecordOffset) byte data
  for (i = 0; i < (*LatestIndex - RecordOffset); i++) {
    HygonIdsDpRamRead (AccessWidth8, DEBUG_PRINT_BUFFER_START + RecordOffset + i, &Value8);
    HygonIdsDpRamWrite (AccessWidth8, DEBUG_PRINT_BUFFER_START + i, &Value8);
  }

  *LatestIndex -= RecordOffset;
  // Fill LatestIndex ~ End with default value
  for (i = 0; i < (DEBUG_PRINT_BUFFER_SIZE - *LatestIndex); i++) {
    Value8 = DEBUG_PRINT_BUFFER_DFT_VALUE;
    HygonIdsDpRamWrite (AccessWidth8, DEBUG_PRINT_BUFFER_START + *LatestIndex + i, &Value8);
  }
}

/**
 *  Print formated string
 *
 *  @param[in] Buffer  - Point to input buffer
 *  @param[in] BufferSize  - Buffer size
 *  @param[in] debugPrintPrivate  - Option
 *
**/
VOID
HygonIdsDpRamPrint (
  IN      CHAR8   *Buffer,
  IN      UINTN BufferSize,
  IN      IDS_DEBUG_PRINT_PRIVATE_DATA *debugPrintPrivate
  )
{
  UINT32  LatestIndex;
  UINT32  Counter;
  UINT64  DebugPrintBufferAddr;
  UINT32  _BufferSize;
  UINT32  StopLoggingWhenBufferFull;

  ASSERT (BufferSize <= DEBUG_PRINT_BUFFER_SIZE);
  // Get Latest Index
  if (HygonIdsDpRamGetLatestIndex (&LatestIndex) == HGPI_SUCCESS) {
    // Add the size for '\0'
    _BufferSize = (UINT32)BufferSize + 1;
    StopLoggingWhenBufferFull = IDS_DPRAM_STOP_LOGGING_WHEN_BUFFER_FULL;
    // Check if exceed the limit, if so shift the oldest data, and do the relocation
    if (((LatestIndex + _BufferSize) > DEBUG_PRINT_BUFFER_SIZE) &&
        !StopLoggingWhenBufferFull) {
      HygonIdsDpRamCleanUp (_BufferSize, &LatestIndex);
    }

    if ((LatestIndex + _BufferSize) <= DEBUG_PRINT_BUFFER_SIZE) {
      // Save the data to RAM, Update the Latest Index
      Counter = _BufferSize;
      DebugPrintBufferAddr = (UINT64)(DEBUG_PRINT_BUFFER_START + LatestIndex);
      while (Counter--) {
        HygonIdsDpRamWrite (AccessWidth8, DebugPrintBufferAddr++, Buffer++);
      }

      LatestIndex += _BufferSize;
      HygonIdsDpRamWrite (AccessWidth32, DEBUG_PRINT_RAM_LATESTIDX_ADDR, &LatestIndex);
    }
  }
}

CONST IDS_DEBUG_PRINT ROMDATA  IdsDebugPrintRamInstance =
{
  HygonIdsDpRamSupport,
  HygonIdsDpRamGetFilter,
  HygonIdsDpRamInitPrivateData,
  HygonIdsDpRamPrint
};
