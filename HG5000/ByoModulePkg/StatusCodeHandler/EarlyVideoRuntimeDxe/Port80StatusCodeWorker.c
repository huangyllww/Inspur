/** @file

Copyright (c) 2006 - 2011, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  Port80StatusCodeWorker.c

Abstract: 
  Port80 status code implementation.

Revision History:

$END--------------------------------------------------------------------

**/


#include "StatusCodeHandlerRuntimeDxe.h"
#include <Port80MapTable.h>

BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL *mByoEarlySimpleTextOutProtocol = NULL;

STATUS_CODE_TO_DATA_MAP* CheckPointStatusCodes[] = 
{
    //#define EFI_PROGRESS_CODE 0x00000001
    mProgressPort80MapTable,
    //#define EFI_ERROR_CODE 0x00000002
    mErrorPort80MapTable
    //#define EFI_DEBUG_CODE 0x00000003
};

UINT32
FindByteCode (
  STATUS_CODE_TO_DATA_MAP    *Map, 
  EFI_STATUS_CODE_VALUE      Value
  )
{
  while(Map->Value != 0) {
    if (Map->Value == Value) {
      return Map->Data;
    }
    Map++;
  }
  return 0;
}

UINT32
GetCheckPointCode (
  IN EFI_STATUS_CODE_TYPE   Type,
  IN EFI_STATUS_CODE_VALUE  Value
  )
{
  UINT32 CodeTypeIndex;
  
  CodeTypeIndex = STATUS_CODE_TYPE (Type) - 1;
  
  if (CodeTypeIndex >= sizeof (CheckPointStatusCodes) / sizeof(STATUS_CODE_TO_DATA_MAP*)) {
    return 0;
  }
  
  return FindByteCode (CheckPointStatusCodes[CodeTypeIndex], Value);
}

void 
Hex2Char(UINT8 Value, CHAR8 *s)
{
  UINT8 Data;

  Data = (Value >> 4) & 0xf;
  if(Data >= 0 && Data <= 9){
    s[0] = Data + '0';
  } else if(Data >= 0xa && Data <= 0xf){
    s[0] = Data - 0xa + 'A';
  }

  Data = Value & 0xf;
  if(Data >= 0 && Data <= 9){
    s[1] = Data + '0';
  } else if(Data >= 0xa && Data <= 0xf){
    s[1] = Data - 0xa + 'A';
  }
  s[2] = 0;
  
  return;
}

void
ShowPort80Code (UINT8 Port80Code)
{
  EFI_STATUS  Status;
  static UINT32  Column = 0;
  static UINT32  Row = 0;
  CHAR8  Code[3];
  
  if (Column == 0) {
    Status = mByoEarlySimpleTextOutProtocol->GetMode (&Column, &Row);
    if (EFI_ERROR (Status)) {
      return;
    }
  }

  Hex2Char (Port80Code, Code);
  mByoEarlySimpleTextOutProtocol->OutputString (Column - 4, Row - 3, Code);
}

/**
  Convert status code value and write data to port 0x80.

  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  Value            Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS      Status code reported to port 0x80 successfully.

**/
EFI_STATUS
EFIAPI
Port80StatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
  UINT8 Port80Code;

  if(mByoEarlySimpleTextOutProtocol == NULL){
    gBS->LocateProtocol (
           &gByoEarlySimpleTextOutProtocolGuid,
           NULL,
           (VOID **) &mByoEarlySimpleTextOutProtocol
           );
  }
  
  if(mByoEarlySimpleTextOutProtocol != NULL){
    Port80Code = (UINT8) GetCheckPointCode (CodeType, Value);
    if (Port80Code != 0) {
      ShowPort80Code (Port80Code);
    }
  }

  return EFI_SUCCESS;
}

