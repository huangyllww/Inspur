/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PostCodeMapLib.c

Abstract:
  Source file for the PostCodeMapLib.

Revision History:

**/

#include <Uefi.h>
#include <Pi/PiStatusCode.h>
#include <Library/PostCodeMapLib.h>
#include "Port80MapTable.h"

STATUS_CODE_TO_DATA_MAP* mCheckPointStatusCodes[] = {
  //#define EFI_PROGRESS_CODE 0x00000001
  mProgressPort80MapTable,
  //#define EFI_ERROR_CODE 0x00000002
  mErrorPort80MapTable
  //#define EFI_DEBUG_CODE 0x00000003
};

/**
  Find the post code data from status code value.

  @param  Map              The map used to find in.
  @param  Value            The status code value.

  @return PostCode         0 for not found.

**/
UINT32
FindByteCode (
  IN STATUS_CODE_TO_DATA_MAP  *Map,
  IN EFI_STATUS_CODE_VALUE    Value
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

/**
  Get PostCode from status code type and value.

  @param  CodeType         Indicates the type of status code being reported.
  @param  Value            Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.

  @return PostCode

**/
UINT32
EFIAPI
GetPostCodeFromStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value
  )
{
  UINT32 CodeTypeIndex;

  CodeTypeIndex = STATUS_CODE_TYPE (CodeType) - 1;

  if (CodeTypeIndex >= sizeof (mCheckPointStatusCodes) / sizeof(STATUS_CODE_TO_DATA_MAP*)) {
    return 0;
  }

  return FindByteCode (mCheckPointStatusCodes[CodeTypeIndex], Value);
}