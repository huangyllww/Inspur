/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  WheaElog.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#ifndef _SMM_GENELOG_H_
#define _SMM_GENELOG_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/SmmLib.h>
#include <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include "EfiServerManagement.h"

#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/GenericElog.h>

#define EFI_ELOG_PHYSICAL     0
#define EFI_ELOG_VIRTUAL      1
#define MAX_REDIR_DESCRIPTOR  10

///
/// A pointer to a function in IPF points to a plabel.
///
typedef struct {
  UINT64  EntryPoint;
  UINT64  GP;
} EFI_PLABEL;

typedef struct {
  EFI_PLABEL  *Function;
  EFI_PLABEL  Plabel;
} FUNCTION_PTR;

typedef struct {
  EFI_SM_ELOG_REDIR_PROTOCOL  *This;
  FUNCTION_PTR                SetEventLogData;
  FUNCTION_PTR                GetEventLogData;
  FUNCTION_PTR                EraseEventLogData;
  FUNCTION_PTR                ActivateEventLog;
} REDIR_MODULE_PROC;

typedef struct {
  BOOLEAN           Valid;
  REDIR_MODULE_PROC Command[2];
} REDIR_MODULES;

typedef struct {
  REDIR_MODULES Redir[MAX_REDIR_DESCRIPTOR];
  UINTN         MaxDescriptors;
} ELOG_MODULE_GLOBAL;

EFI_STATUS
EfiConvertFunction (
  IN  FUNCTION_PTR                  *Function
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Function  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiSetFunctionEntry (
  IN  FUNCTION_PTR                  *FunctionPointer,
  IN  VOID                          *Function
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  FunctionPointer - GC_TODO: add argument description
  Function        - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

VOID
SmRedirAddressChangeEvent (
  IN EFI_EVENT                      Event,
  IN VOID                           *Context
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  Event   - GC_TODO: add argument description
  Context - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiLibSetElogData (
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId,
  ELOG_MODULE_GLOBAL                    *Global,
  BOOLEAN                               Virtual
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ElogData    - GC_TODO: add argument description
  DataType    - GC_TODO: add argument description
  AlertEvent  - GC_TODO: add argument description
  DataSize    - GC_TODO: add argument description
  RecordId    - GC_TODO: add argument description
  Global      - GC_TODO: add argument description
  Virtual     - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiLibGetElogData (
  IN UINT8                              *ElogData,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINTN                          *DataSize,
  IN OUT UINT64                         *RecordId,
  ELOG_MODULE_GLOBAL                    *Global,
  BOOLEAN                               Virtual
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ElogData  - GC_TODO: add argument description
  DataType  - GC_TODO: add argument description
  DataSize  - GC_TODO: add argument description
  RecordId  - GC_TODO: add argument description
  Global    - GC_TODO: add argument description
  Virtual   - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiLibEraseElogData (
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINT64                         *RecordId,
  ELOG_MODULE_GLOBAL                    *Global,
  BOOLEAN                               Virtual
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  DataType  - GC_TODO: add argument description
  RecordId  - GC_TODO: add argument description
  Global    - GC_TODO: add argument description
  Virtual   - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

EFI_STATUS
EfiLibActivateElog (
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN BOOLEAN                            *EnableElog,
  OUT BOOLEAN                           *ElogStatus,
  ELOG_MODULE_GLOBAL                    *Global,
  BOOLEAN                               Virtual
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  DataType    - GC_TODO: add argument description
  EnableElog  - GC_TODO: add argument description
  ElogStatus  - GC_TODO: add argument description
  Global      - GC_TODO: add argument description
  Virtual     - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

#endif //_SMM_GENELOG_H_
