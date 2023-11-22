/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  SerialStatusCodeWorker.c

Abstract: 
  Serial I/O status code reporting worker.

Revision History:

Bug 2517:   Create the Module StatusCodeHandler to report status code to 
            all supported device in ByoModule
TIME:       2011-7-22
$AUTHOR:    Liu Chunling
$REVIEWERS:  
$SCOPE:     All Platforms
$TECHNICAL:  
  1. Create the module StatusCodeHandler to support Serial Port, Memory, Port80,
     Beep and OEM devices to report status code.
  2. Create the Port80 map table and the Beep map table to convert status code 
     to code byte and beep times.
  3. Create new libraries to support status code when StatusCodePpi,
     StatusCodeRuntimeProtocol, SmmStatusCodeProtocol has not been installed yet.
$END--------------------------------------------------------------------

**/

#include "StatusCodeHandlerSmm.h"
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
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
  CHAR8           *Filename;
  CHAR8           *Description;
  CHAR8           *Format;
  CHAR8           Buffer[EFI_STATUS_CODE_DATA_MAX_SIZE];
  UINT32          ErrorLevel;
  UINT32          LineNumber;
  UINTN           CharCount;
  BASE_LIST       Marker;


  if(gByoStatusCodeLevel.MessageDisabled){
    return EFI_SUCCESS;
  }

  Buffer[0] = '\0';

  if (Data != NULL &&
      ReportStatusCodeExtractAssertInfo (CodeType, Value, Data, &Filename, &Description, &LineNumber)) {
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
  } else if (Data != NULL &&
             ReportStatusCodeExtractDebugInfo (Data, &ErrorLevel, &Marker, &Format)) {
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
                  "ERROR: C%x:V%x I%x", 
                  CodeType, 
                  Value, 
                  Instance
                  );
    ASSERT (CharCount > 0);
   
    if (CallerId != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount - 1],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %g",
                     CallerId
                     );
    }

    if (Data != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount - 1],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %x",
                     Data
                     );
    }

    CharCount += AsciiSPrint (
                   &Buffer[CharCount - 1],
                   (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                   "\n\r"
                   );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    //
    // Print PROGRESS information into output buffer.
    //
    if(FeaturePcdGet(PcdStatusCodePrintProgressCode)){
      CharCount = AsciiSPrint (
                    Buffer, 
                    sizeof (Buffer), 
                    "PROGRESS CODE: V%x I%x\n\r", 
                    Value, 
                    Instance
                    );
    } else {
      CharCount = 0;
    }
  } else if (Data != NULL &&
             CompareGuid (&Data->Type, &gEfiStatusCodeDataTypeStringGuid) &&
             ((EFI_STATUS_CODE_STRING_DATA *) Data)->StringType == EfiStringAscii) {
    //
    // EFI_STATUS_CODE_STRING_DATA
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "%a\n\r",
                  ((EFI_STATUS_CODE_STRING_DATA *) Data)->String.Ascii
                  );
  } else {
    //
    // Code type is not defined.
    //
    CharCount = AsciiSPrint (
                  Buffer, 
                  sizeof (Buffer), 
                  "Undefined: C%x:V%x I%x\n\r", 
                  CodeType, 
                  Value, 
                  Instance
                  );
  }

  //
  // Call SerialPort Lib function to do print.
  //
  if(CharCount){
     if ((Value&EDKII_BOOT_HOOK_POINT)!=EDKII_BOOT_HOOK_POINT){
       if(gAfterReadyToBoot && gByoStatusCodeLevel.IsUartValid != NULL && !gByoStatusCodeLevel.IsUartValid()){
         return EFI_SUCCESS;
       }
       if (GetPerformanceCounterProperties(NULL, NULL) != 0) {
         SerialPortWriteWithTimeStamp (Buffer, CharCount, EFI_STATUS_CODE_DATA_MAX_SIZE);
       } else {
         SerialPortWrite ((UINT8 *)Buffer, CharCount);
       }
     }
  }
  return EFI_SUCCESS;
}

