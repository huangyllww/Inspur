/** @file
  Internal include file for Status Code Handler Driver.

  Copyright (c) 2009 - 2022, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/

#ifndef __STATUS_CODE_HANDLER_SMM_H__
#define __STATUS_CODE_HANDLER_SMM_H__

#include <Protocol/SmmReportStatusCodeHandler.h>

#include <Guid/MemoryStatusCodeRecord.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/StatusCodeDataTypeDebug.h>

#include <Library/SynchronizationLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/SerialPortLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/StatusCodeBeepLib.h>
#include <Library/PostCodeMapLib.h>
#include <Protocol/ByoStatusCodeLevelOverrideProtocol.h>

//
// Runtime memory status code worker definition
//
//typedef struct {
//  UINT32   RecordIndex;
//  UINT32   NumberOfRecords;
//  UINT32   MaxRecordsNumber;
//} RUNTIME_MEMORY_STATUSCODE_HEADER;

extern RUNTIME_MEMORY_STATUSCODE_HEADER  *mSmmMemoryStatusCodeTable;
extern BYO_STATUS_CODE_LEVEL             gByoStatusCodeLevel;
extern BOOLEAN                           gAfterReadyToBoot;

/**
  Locates Serial I/O Protocol as initialization for serial status code worker.
 
  @retval EFI_SUCCESS  Serial I/O Protocol is successfully located.

**/
EFI_STATUS
EfiSerialStatusCodeInitializeWorker (
  VOID
  );


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
  );

/**
  Initialize runtime memory status code table as initialization for runtime memory status code worker
 
  @retval EFI_SUCCESS  Runtime memory status code table successfully initialized.

**/
EFI_STATUS
MemoryStatusCodeInitializeWorker (
  VOID
  );

/**
  Report status code into runtime memory. If the runtime pool is full, roll back to the 
  first record and overwrite it.
 
  @param  CodeType                Indicates the type of status code being reported.
  @param  Value                   Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is used to
                                  classify the entity as well as an operation.
  @param  Instance                The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param  CallerId                This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different rules to
                                  different callers.
  @param  Data                    This optional parameter may be used to pass additional data.
 
  @retval EFI_SUCCESS             Status code successfully recorded in runtime memory status code table.

**/
EFI_STATUS
EFIAPI
MemoryStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE               CodeType,
  IN EFI_STATUS_CODE_VALUE              Value,
  IN UINT32                             Instance,
  IN EFI_GUID                           *CallerId,
  IN EFI_STATUS_CODE_DATA               *Data OPTIONAL
  );

/**
  Create the first memory status code GUID'ed HOB as initialization for memory status code worker.

  @retval EFI_SUCCESS  The GUID'ed HOB is created successfully.

**/
EFI_STATUS
Port80StatusCodeInitialize (
  VOID
  );

/**
  Convert status code value and write data to port 0x80.
 
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

  @retval EFI_SUCCESS      Status code reported to port 0x80 successfully.

**/
EFI_STATUS
EFIAPI
Port80StatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  );

/**
  Convert status code value to the times of beep.
 
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

  @retval EFI_SUCCESS      Status code reported to beep successfully.

**/
EFI_STATUS
EFIAPI
BeepStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN EFI_GUID                       *CallerId,
  IN EFI_STATUS_CODE_DATA           *Data OPTIONAL
  );

/**
  Create the first memory status code GUID'ed HOB as initialization for memory status code worker.

  @retval EFI_SUCCESS  The GUID'ed HOB is created successfully.

**/
EFI_STATUS
OemStatusCodeInitialize (
  VOID
  );

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
OemStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN EFI_GUID                       *CallerId,
  IN EFI_STATUS_CODE_DATA           *Data OPTIONAL
  );

#endif
