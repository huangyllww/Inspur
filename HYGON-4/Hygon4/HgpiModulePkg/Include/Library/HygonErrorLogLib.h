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

#ifndef _HYGON_ERROR_LOG_LIB_H_
#define _HYGON_ERROR_LOG_LIB_H_

#include <HygonErrorLog.h>
#include <Pi/PiMultiPhase.h>

/**
 * An HYGON Error Log entry.
 */
typedef struct {
  HYGON_STATUS    ErrorClass; ///< The severity of the error, its associated HGPI_STATUS.
  UINT32          ErrorInfo;  ///< Uniquely identifies the error.
  UINT32          DataParam1; ///< Error specific additional data
  UINT32          DataParam2; ///< Error specific additional data
  UINT32          DataParam3; ///< Error specific additional data
  UINT32          DataParam4; ///< Error specific additional data
} HYGON_ERROR_ENTRY;

/**
 * A wrapper for each Error Log entry.
 */
typedef struct {
  UINT16               Count;      ///< Entry number
  HYGON_ERROR_ENTRY    HygonError; ///< The entry itself.
} HYGON_ERROR_STRUCT;

/**
 * The Error Log.
 */
typedef struct {
  UINT16                ReadWriteFlag;                             ///< Read Write flag.
  UINT16                Count;                                     ///< The total number of active entries.
  UINT16                ReadRecordPtr;                             ///< The next entry to read.
  UINT16                WriteRecordPtr;                            ///< The next entry to write.
  HYGON_ERROR_STRUCT    HygonErrorStruct[TOTAL_ERROR_LOG_BUFFERS]; ///< The entries.
} HYGON_ERROR_BUFFER;

typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType;   ///< GUID Hob type structure
  HYGON_ERROR_BUFFER    HygonErrorBuffer; ///< HYGON Error Log buffer structure
} HYGON_ERROR_LOG_INFO_HOB;

extern void
ErrorLogBufferInit (
  HYGON_ERROR_BUFFER *ErrorLogBuffer
  );

extern void
AddErrorLog (
  HYGON_ERROR_BUFFER *ErrorLogBuffer,
  HYGON_ERROR_ENTRY  *HygonErrorEntry
  );

extern void
AquireErrorLog (
  HYGON_ERROR_BUFFER      *ErrorLogBuffer,
  ERROR_LOG_DATA_STRUCT *ErrorLogDataPtr
  );

#endif // _HYGON_ERROR_LOG_LIB_H_
