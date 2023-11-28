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

#ifndef _AMD_ERROR_LOG_LIB_H_
#define _AMD_ERROR_LOG_LIB_H_

#include <AmdErrorLog.h>
#include <Pi/PiMultiPhase.h>

/**
 * An AMD Error Log entry.
 */
typedef struct {
  AMD_STATUS ErrorClass;   ///< The severity of the error, its associated AGESA_STATUS.
  UINT32   ErrorInfo;        ///< Uniquely identifies the error.
  UINT32   DataParam1;       ///< Error specific additional data
  UINT32   DataParam2;       ///< Error specific additional data
  UINT32   DataParam3;       ///< Error specific additional data
  UINT32   DataParam4;       ///< Error specific additional data
} AMD_ERROR_ENTRY;

/**
 * A wrapper for each Error Log entry.
 */
typedef struct {
  UINT16            Count;         ///< Entry number
  AMD_ERROR_ENTRY   AmdError;    ///< The entry itself.
} AMD_ERROR_STRUCT;

/**
 * The Error Log.
 */
typedef struct {
  UINT16        ReadWriteFlag;   ///< Read Write flag.
  UINT16        Count;           ///< The total number of active entries.
  UINT16        ReadRecordPtr;   ///< The next entry to read.
  UINT16        WriteRecordPtr;  ///< The next entry to write.
  AMD_ERROR_STRUCT AmdErrorStruct[TOTAL_ERROR_LOG_BUFFERS];   ///< The entries.
} AMD_ERROR_BUFFER;

typedef struct {
  EFI_HOB_GUID_TYPE    EfiHobGuidType;   ///< GUID Hob type structure
  AMD_ERROR_BUFFER     AmdErrorBuffer;   ///< AMD Error Log buffer structure
} AMD_ERROR_LOG_INFO_HOB;

extern void ErrorLogBufferInit (AMD_ERROR_BUFFER *ErrorLogBuffer);
extern void AddErrorLog (AMD_ERROR_BUFFER *ErrorLogBuffer, AMD_ERROR_ENTRY  *AmdErrorEntry);
extern void AquireErrorLog (AMD_ERROR_BUFFER *ErrorLogBuffer, ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr);


#endif  // _AMD_ERROR_LOG_LIB_H_
