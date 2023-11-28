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
  * ***************************************************************************
  *
 */

#include <Filecode.h>
#include <PiPei.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdErrorLogLib.h>

#define FILECODE LIBRARY_AMDERRORLOGLIB_AMDERRORLOGLIB_FILECODE

/*----------------------------------------------------------------------------
 *                          DEFINITIONS AND MACROS
 *
 *----------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------
 *                           TYPEDEFS AND STRUCTURES
 *
 *----------------------------------------------------------------------------
 */

VOID
AquireErrorLog (
  IN       AMD_ERROR_BUFFER *ErrorLogBuffer,
  IN OUT   ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  )
{
  UINT32        ErrorLogCounter;
  UINT16        Index;

  ErrorLogCounter = 0;
  LibAmdMemFill (ErrorLogDataPtr, 0, sizeof (ERROR_LOG_DATA_STRUCT), NULL);
  while (!((ErrorLogBuffer->ReadRecordPtr == ErrorLogBuffer->WriteRecordPtr) && (ErrorLogBuffer->ReadWriteFlag == 1))) {
    Index = ErrorLogBuffer->ReadRecordPtr;
    // ErrorLogCounter is start from 0
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].ErrorClass = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.ErrorClass;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].ErrorInfo  = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.ErrorInfo;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].DataParam1 = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam1;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].DataParam2 = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam2;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].DataParam3 = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam3;
    ErrorLogDataPtr->ErrorLog_Param[ErrorLogCounter].DataParam4 = ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam4;
    if (ErrorLogBuffer->ReadRecordPtr == (TOTAL_ERROR_LOG_BUFFERS - 1)) {
      ErrorLogBuffer->ReadRecordPtr = 0;
    } else {
      ErrorLogBuffer->ReadRecordPtr = ErrorLogBuffer->ReadRecordPtr + 1;
    }
    if (ErrorLogBuffer->ReadRecordPtr == ErrorLogBuffer->WriteRecordPtr) {
      ErrorLogBuffer->ReadWriteFlag = 1;
    }
    // Actual Error Log count
    ErrorLogCounter += 1;
  }
  ErrorLogDataPtr->Count = ErrorLogCounter;
}

VOID
AddErrorLog (
  IN OUT   AMD_ERROR_BUFFER *ErrorLogBuffer,
  IN       AMD_ERROR_ENTRY  *AmdErrorEntry
  )
{
  UINT16 Index;

  Index = ErrorLogBuffer->WriteRecordPtr;

  // Add the new Error log data into a circular buffer
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.ErrorClass = AmdErrorEntry->ErrorClass;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.ErrorInfo  = AmdErrorEntry->ErrorInfo;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam1 = AmdErrorEntry->DataParam1;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam2 = AmdErrorEntry->DataParam2;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam3 = AmdErrorEntry->DataParam3;
  ErrorLogBuffer->AmdErrorStruct[Index].AmdError.DataParam4 = AmdErrorEntry->DataParam4;

  if ((ErrorLogBuffer->WriteRecordPtr == ErrorLogBuffer->ReadRecordPtr) &&
     (ErrorLogBuffer->ReadWriteFlag == 0)) {
    ErrorLogBuffer->WriteRecordPtr += 1;
    ErrorLogBuffer->ReadRecordPtr += 1;
    if (ErrorLogBuffer->WriteRecordPtr == TOTAL_ERROR_LOG_BUFFERS) {
      ErrorLogBuffer->WriteRecordPtr = 0;
      ErrorLogBuffer->ReadRecordPtr  = 0;
    }
  } else {
    ErrorLogBuffer->WriteRecordPtr += 1;
    if (ErrorLogBuffer->WriteRecordPtr == TOTAL_ERROR_LOG_BUFFERS) {
      ErrorLogBuffer->WriteRecordPtr = 0;
    }
    ErrorLogBuffer->ReadWriteFlag = 0;
  }
  ErrorLogBuffer->Count = ErrorLogBuffer->Count + 1;

  if (ErrorLogBuffer->Count <= TOTAL_ERROR_LOG_BUFFERS) {
    ErrorLogBuffer->AmdErrorStruct[Index].Count = Index;
  }
}

VOID
ErrorLogBufferInit (
  IN OUT   AMD_ERROR_BUFFER *ErrorLogBuffer
  )
{
  ErrorLogBuffer->Count = 0;
  ErrorLogBuffer->ReadRecordPtr = 0;
  ErrorLogBuffer->WriteRecordPtr = 0;
  ErrorLogBuffer->ReadWriteFlag = 1;
}
