/** @file
  Serial I/O status code reporting worker.

  Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
  SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "StatusCodeHandlerRuntimeDxe.h"
#include <Library/BaseLib.h>
#include <Library/TimerLib.h>
#define DEBUG_LIB_TIME_STAMP_LENGTH   12

VOID
SerialPortWriteWithTimeStamp (
  IN CHAR8  *Buffer,
  IN UINTN  NumberOfBytes,
  IN UINTN  MaxBufferSize
  )
{
  UINT32 TimeStampInMs;
  CHAR8  *MatchFormat;
  CHAR8  TimeFormatStr[DEBUG_LIB_TIME_STAMP_LENGTH + 1];
  
  MatchFormat = AsciiStrStr (Buffer, "\r\n");
  if (MatchFormat != NULL) {
    MatchFormat += 2;
    if ((MaxBufferSize - (MatchFormat - Buffer) - AsciiStrSize (MatchFormat)) >= DEBUG_LIB_TIME_STAMP_LENGTH) {
      CopyMem (MatchFormat + DEBUG_LIB_TIME_STAMP_LENGTH, MatchFormat, AsciiStrSize (MatchFormat));
      TimeStampInMs = (UINT32) DivU64x32 (GetTimeInNanoSecond (GetPerformanceCounter()), 1000 * 1000);
      AsciiSPrint (TimeFormatStr, sizeof (TimeFormatStr), "[%5d.%03ds]", TimeStampInMs / 1000, TimeStampInMs % 1000);
      CopyMem (MatchFormat, TimeFormatStr, DEBUG_LIB_TIME_STAMP_LENGTH);
    }
  }
  SerialPortWrite ((UINT8*)Buffer, AsciiStrLen (Buffer));
}

/**
  Convert status code value and extended data to readable ASCII string, send string to serial I/O device.

  @param  CodeType         Indicates the type of status code being reported.
  @param  Value            Describes the current status of a hardware or software entity.
                           This included information about the class and subclass that is used to
                           classify the entity as well as an operation.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS      Status code reported to serial I/O successfully.
  @retval EFI_DEVICE_ERROR EFI serial device cannot work after ExitBootService() is called.
  @retval EFI_DEVICE_ERROR EFI serial device cannot work with TPL higher than TPL_CALLBACK.

**/
EFI_STATUS
EFIAPI
SerialStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE   CodeType,
  IN EFI_STATUS_CODE_VALUE  Value,
  IN UINT32                 Instance,
  IN EFI_GUID               *CallerId,
  IN EFI_STATUS_CODE_DATA   *Data OPTIONAL
  )
{
  CHAR8      *Filename;
  CHAR8      *Description;
  CHAR8      *Format;
  CHAR8      Buffer[MAX_DEBUG_MESSAGE_LENGTH];
  UINT32     ErrorLevel;
  UINT32     LineNumber;
  UINTN      CharCount;
  BASE_LIST  Marker;

  Buffer[0] = '\0';

  if ((Data != NULL) &&
      ReportStatusCodeExtractAssertInfo (CodeType, Value, Data, &Filename, &Description, &LineNumber))
  {
    //
    // Print ASSERT() information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "\n\rDXE_ASSERT!: %a (%d): %a\n\r",
                  Filename,
                  LineNumber,
                  Description
                  );
  } else if ((Data != NULL) &&
             ReportStatusCodeExtractDebugInfo (Data, &ErrorLevel, &Marker, &Format))
  {
    //
    // Print DEBUG() information into output buffer.
    //
    CharCount = AsciiBSPrint (
                  Buffer,
                  sizeof (Buffer),
                  Format,
                  Marker
                  );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    //
    // Print ERROR information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "ERROR: C%08x:V%08x I%x",
                  CodeType,
                  Value,
                  Instance
                  );
    ASSERT (CharCount > 0);

    if (CallerId != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %g",
                     CallerId
                     );
    }

    if (Data != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %x",
                     Data
                     );
    }

    CharCount += AsciiSPrint (
                   &Buffer[CharCount],
                   (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                   "\n\r"
                   );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    //
    // Print PROGRESS information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "PROGRESS CODE: V%08x I%x\n\r",
                  Value,
                  Instance
                  );
  } else if ((Data != NULL) &&
             CompareGuid (&Data->Type, &gEfiStatusCodeDataTypeStringGuid) &&
             (((EFI_STATUS_CODE_STRING_DATA *)Data)->StringType == EfiStringAscii))
  {
    //
    // EFI_STATUS_CODE_STRING_DATA
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "%a",
                  ((EFI_STATUS_CODE_STRING_DATA *)Data)->String.Ascii
                  );
  } else {
    //
    // Code type is not defined.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "Undefined: C%08x:V%08x I%x\n\r",
                  CodeType,
                  Value,
                  Instance
                  );
  }

  //
  // Call SerialPort Lib function to do print.
  // BYOSOFT_OVERRIDE: OEM Report Status Code is only used for Process Handler Notify.
  //                   No Serial Output need be generated.
  if ((Value&EDKII_BOOT_HOOK_POINT) != EDKII_BOOT_HOOK_POINT) {
    if (GetPerformanceCounterProperties(NULL, NULL) != 0) {
      SerialPortWriteWithTimeStamp (Buffer, CharCount, MAX_DEBUG_MESSAGE_LENGTH);
    } else {
      SerialPortWrite ((UINT8 *)Buffer, CharCount);
    }
  }

  //
  // If register an unregister function of gEfiEventExitBootServicesGuid,
  // then some log called in ExitBootServices() will be lost,
  // so unregister the handler after receive the value of exit boot service.
  //
  if (((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) &&
      (Value == (EFI_SOFTWARE_EFI_BOOT_SERVICE | EFI_SW_BS_PC_EXIT_BOOT_SERVICES)))
  {
    UnregisterSerialBootTimeHandlers ();
  }

  return EFI_SUCCESS;
}
