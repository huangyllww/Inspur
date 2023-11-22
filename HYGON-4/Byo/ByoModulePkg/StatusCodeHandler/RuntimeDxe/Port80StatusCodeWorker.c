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


#include "StatusCodeHandlerRuntimeDxe.h"

/**
  Initialization port80 status code worker.

  @retval EFI_SUCCESS  Initialization is successfully.

**/
EFI_STATUS
Port80StatusCodeInitialize (
  VOID
  )
{
  return EFI_SUCCESS;
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
  IN EFI_GUID                       *CallerId,
  IN EFI_STATUS_CODE_DATA           *Data OPTIONAL
  )
{
  UINT8                             Port80Code;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  UINTN                             Columns;
  UINTN                             Rows;
  UINTN                             Px, Py;
  UINTN                             Attribute; 
  CHAR16                            StrBuffer[3];
  UINT32                            EarlyConOutColumn;
  UINT32                            EarlyConOutRows;


  if(gEarlyConOut != NULL && 
     (CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE && 
     Value == (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BEGIN_CONNECTING_DRIVERS)){
    gEarlyConOut = NULL;
  }
  
  Port80Code = (UINT8) GetPostCodeFromStatusCode (CodeType, Value);
  if (Port80Code != 0) {
    DEBUG ((EFI_D_ERROR, "POSTCODE=<%02x>\n", Port80Code));
    IoWrite8 (0x80, Port80Code);
    if(FixedPcdGetBool(PcdShowPostCodeToScreen) && gAllowShowPostCodeToScreen){
      if(gST->ConOut != NULL){
        ConOut = gST->ConOut;
        ConOut->QueryMode(ConOut, ConOut->Mode->Mode, &Columns, &Rows);
        Px = ConOut->Mode->CursorColumn;
        Py = ConOut->Mode->CursorRow;
        Attribute = ConOut->Mode->Attribute;
        UnicodeSPrint(StrBuffer, sizeof(StrBuffer), L"%02X", Port80Code);
        ConOut->SetAttribute(ConOut, EFI_LIGHTGRAY | EFI_BACKGROUND_BLACK);
        ConOut->SetCursorPosition(ConOut, Columns - 4, Rows-2);
        ConOut->OutputString(ConOut, StrBuffer); 
        ConOut->SetAttribute(ConOut, Attribute);
        ConOut->SetCursorPosition(ConOut, Px, Py);  
      }
      if(gEarlyConOut != NULL){
        AsciiSPrint((CHAR8*)StrBuffer, sizeof(StrBuffer), "%02X", Port80Code);
        gEarlyConOut->GetMode(&EarlyConOutColumn, &EarlyConOutRows);
        gEarlyConOut->OutputString((UINT16)(EarlyConOutColumn - 4), (UINT16)(EarlyConOutRows - 2), (CHAR8*)StrBuffer);
      }      
    }
  }

  return EFI_SUCCESS;
}

