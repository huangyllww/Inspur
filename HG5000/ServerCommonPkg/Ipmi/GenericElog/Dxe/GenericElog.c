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

#include "GenericElog.h"

ELOG_MODULE_GLOBAL  *mElogModuleGlobal;

//
// Define module globals used to register for notification of when
// the ELOG REDIR protocol has been produced.
//
EFI_EVENT           mEfiElogRedirProtocolEvent;

EFI_STATUS
EfiLibSetElogData (
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId,
  IN  ELOG_MODULE_GLOBAL                *Global,
  IN  BOOLEAN                           Virtual
  )
/*++

Routine Description:

    Sends the Event-Log data to the destination.

Arguments:
   
  ElogData              - Pointer to the Event-Log data that needs to be recorded.
  DataSize              - Size of the data to be logged.  
  DataType              - Type of Elog Data that is being recorded.
  AlertEvent            - This is an indication that the input data type is an Alert. 
  RecordId              - Rrecord ID sent by the target.
  Global                - The module global variable pointer.
  Virtual               - If this funciton is called in virtual mode or physical mode

Returns:

  EFI_SUCCESS            - Event-Log was recorded successfully.          
  EFI_OUT_OF_RESOURCES   - Not enough resources to record data. 
  EFI_UNSUPPORTED        - The Data Type is unsupported.  
  
--*/
{
  UINT8       Index;
  VOID        *ElogRedirCommand;
  EFI_STATUS  Status;
  EFI_STATUS  RetStatus;

  RetStatus = EFI_UNSUPPORTED;

  if (DataType >= EfiSmElogMax) {
	  RetStatus = EFI_INVALID_PARAMETER;
	  return RetStatus;
  }
  
  for (Index = 0; Index < Global->MaxDescriptors; Index++) {
    if (Global->Redir[Index].Valid) {
      ElogRedirCommand = Global->Redir[Index].Command[Virtual].SetEventLogData.Function;
      Status = (*((EFI_SET_ELOG_DATA *) &ElogRedirCommand)) (Global->Redir[Index].Command[Virtual].This, ElogData, DataType, AlertEvent, DataSize, RecordId);

      if (!EFI_ERROR (Status)) {
        RetStatus = EFI_SUCCESS;
        break;
      } else if (Status != EFI_UNSUPPORTED) {
        RetStatus = Status;
        break;
      }
    }
  }

  return RetStatus;
}

EFI_STATUS
EfiSetElogData (
  IN  EFI_SM_ELOG_PROTOCOL              *This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  This        - GC_TODO: add argument description
  ElogData    - GC_TODO: add argument description
  DataType    - GC_TODO: add argument description
  AlertEvent  - GC_TODO: add argument description
  DataSize    - GC_TODO: add argument description
  RecordId    - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (DataType >= EfiSmElogMax) {
    return EFI_INVALID_PARAMETER;
  }

  return EfiLibSetElogData (
          ElogData,
          DataType,
          AlertEvent,
          DataSize,
          RecordId,
          mElogModuleGlobal,
          FALSE
          );
}

EFI_STATUS
EfiLibGetElogData (
  IN UINT8                              *ElogData,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINTN                          *DataSize,
  IN OUT UINT64                         *RecordId,
  IN ELOG_MODULE_GLOBAL                 *Global,
  IN BOOLEAN                            Virtual
  )
/*++

Routine Description:
    Gets the Event-Log data from the destination.

Arguments:
   
  ElogData              - Pointer to the Event-Log data buffer that will contain the data to be retrieved.
  DataSize              - Size of the data to be retrieved. .   
  DataType              - Type of Elog Data that is being recorded.  
  RecordId              - This is the RecordId of the next record. If ElogData is NULL, 
                          this gives the RecordId of the first record available in the database with the correct DataSize.
                          A value of 0 on return indicates the last record if the EFI_STATUS indicates a success
  Global                - The module global variable pointer.
  Virtual               - If this funciton is called in virtual mode or physical mode

Returns:

  EFI_SUCCESS           - Event-Log was retrieved successfully.           
  EFI_NOT_FOUND         - Event-Log target not found.                   
  EFI_BUFFER_TOO_SMALL  - Target buffer is too small to retrieve the data.
  EFI_UNSUPPORTED       - The Data Type is unsupported  
  
--*/
{
  UINT8       Index;
  VOID        *ElogRedirCommand;
  EFI_STATUS  Status;
  EFI_STATUS  RetStatus;

  RetStatus = EFI_UNSUPPORTED;

  if (DataType >= EfiSmElogMax) {
	RetStatus = EFI_INVALID_PARAMETER;
	return RetStatus;
  }

  for (Index = 0; Index < Global->MaxDescriptors; Index++) {
    if (Global->Redir[Index].Valid) {
      ElogRedirCommand = Global->Redir[Index].Command[Virtual].GetEventLogData.Function;
      Status = (*((EFI_GET_ELOG_DATA *) &ElogRedirCommand)) (Global->Redir[Index].Command[Virtual].This, ElogData, DataType, DataSize, RecordId);

      if (!EFI_ERROR (Status)) {
        RetStatus = EFI_SUCCESS;
        break;
      } else if (Status != EFI_UNSUPPORTED) {
        RetStatus = Status;
        break;
      }
    }
  }

  return RetStatus;
}

EFI_STATUS
EfiGetElogData (
  IN EFI_SM_ELOG_PROTOCOL               *This,
  IN UINT8                              *ElogData,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINTN                          *DataSize,
  IN OUT UINT64                         *RecordId
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  This      - GC_TODO: add argument description
  ElogData  - GC_TODO: add argument description
  DataType  - GC_TODO: add argument description
  DataSize  - GC_TODO: add argument description
  RecordId  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (DataType >= EfiSmElogMax) {
    return EFI_INVALID_PARAMETER;
  }

  return EfiLibGetElogData (ElogData, DataType, DataSize, RecordId, mElogModuleGlobal, FALSE);
}

EFI_STATUS
EfiLibEraseElogData (
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINT64                         *RecordId,
  IN ELOG_MODULE_GLOBAL                 *Global,
  IN BOOLEAN                            Virtual
  )
/*++

Routine Description:
    Erases the Event-Log data from the destination.

Arguments:
   
  DataType              - Type of Elog Data that is being Erased.   
  RecordId              - This is the RecordId of the data to be erased. If RecordId is NULL, all 
                          the records on the database are erased if permitted by the target. 
                          Contains the deleted RecordId on return
  Global                - The module global variable pointer.
  Virtual               - If this funciton is called in virtual mode or physical mode

Returns:

  EFI_SUCCESS           - Event-Log was erased successfully           
  EFI_UNSUPPORTED       - The Data Type is unsupported
  EFI_NOT_FOUND         - Event-Log target not found
  
--*/
{
  UINT8       Index;
  VOID        *ElogRedirCommand;
  EFI_STATUS  Status;
  EFI_STATUS  RetStatus;

  RetStatus = EFI_UNSUPPORTED;

  if (DataType >= EfiSmElogMax) {
	RetStatus = EFI_INVALID_PARAMETER;
	return RetStatus;
  }

  for (Index = 0; Index < Global->MaxDescriptors; Index++) {
    if (Global->Redir[Index].Valid) {
      ElogRedirCommand = Global->Redir[Index].Command[Virtual].EraseEventLogData.Function;
      Status = (*((EFI_ERASE_ELOG_DATA *) &ElogRedirCommand)) (Global->Redir[Index].Command[Virtual].This, DataType, RecordId);

      if (!EFI_ERROR (Status)) {
        RetStatus = EFI_SUCCESS;
        break;
      } else if (Status != EFI_UNSUPPORTED) {
        RetStatus = Status;
        break;
      }
    }
  }

  return RetStatus;
}

EFI_STATUS
EfiEraseElogData (
  IN EFI_SM_ELOG_PROTOCOL               *This,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINT64                         *RecordId
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  This      - GC_TODO: add argument description
  DataType  - GC_TODO: add argument description
  RecordId  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  return EfiLibEraseElogData (DataType, RecordId, mElogModuleGlobal, FALSE);
}

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
    This API enables/Disables Event Log

Arguments:
   
  DataType              - Type of Elog Data that is being Activated.
  EnableElog            - Enables (TRUE)/Disables(FALSE) Event Log. If NULL just returns the 
                          Current ElogStatus. 
  ElogStatus            - Current (New) Status of Event Log. Enabled (TRUE), Disabled (FALSE).
  Global                - The module global variable pointer.
  Virtual               - If this funciton is called in virtual mode or physical mode

Returns:

  EFI_SUCCESS           - Event-Log was recorded successfully           
  EFI_UNSUPPORTED       - The Data Type is unsupported
  
--*/
{
  UINT8       Index;
  VOID        *ElogRedirCommand;
  EFI_STATUS  Status;
  EFI_STATUS  RetStatus;

  RetStatus = EFI_UNSUPPORTED;
  if (DataType >= EfiSmElogMax) {
	RetStatus = EFI_INVALID_PARAMETER;
	return RetStatus;
  }

  for (Index = 0; Index < Global->MaxDescriptors; Index++) {
    if (Global->Redir[Index].Valid) {
      ElogRedirCommand = Global->Redir[Index].Command[Virtual].ActivateEventLog.Function;
      Status = (*((EFI_ACTIVATE_ELOG *) &ElogRedirCommand)) (Global->Redir[Index].Command[Virtual].This, DataType, EnableElog, ElogStatus);

      if (!EFI_ERROR (Status)) {
        RetStatus = EFI_SUCCESS;
        break;
      } else if (Status != EFI_UNSUPPORTED) {
        RetStatus = Status;
        break;
      }
    }
  }

  return RetStatus;
}

EFI_STATUS
EfiActivateElog (
  IN EFI_SM_ELOG_PROTOCOL               *This,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN BOOLEAN                            *EnableElog,
  OUT BOOLEAN                           *ElogStatus
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  This        - GC_TODO: add argument description
  DataType    - GC_TODO: add argument description
  EnableElog  - GC_TODO: add argument description
  ElogStatus  - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  if (DataType >= EfiSmElogMax) {
    return EFI_INVALID_PARAMETER;
  }
  return EfiLibActivateElog (DataType, EnableElog, ElogStatus, mElogModuleGlobal, FALSE);
}


EFI_STATUS
SetElogRedirInstances (
  VOID
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  None

Returns:

  EFI_SUCCESS - GC_TODO: Add description for return value
  EFI_SUCCESS - GC_TODO: Add description for return value

--*/
{
  UINTN                       NumHandles;
  UINTN                       Index;
  UINTN                       Instance;
  UINTN                       EmptyIndex;
  EFI_HANDLE                  *Buffer = NULL;
  EFI_STATUS                  Status;
  EFI_SM_ELOG_REDIR_PROTOCOL  *Redir;
  REDIR_MODULE_PROC           *RedirProc;

  //
  // Check for all IPMI Controllers
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiRedirElogProtocolGuid,
                  NULL,
                  &NumHandles,
                  &Buffer
                  );

  if (EFI_ERROR (Status) || !NumHandles) {
    return EFI_SUCCESS;
  }

  for (Index = 0; ((Index < NumHandles) && (Index < mElogModuleGlobal->MaxDescriptors)); Index++) {
    EmptyIndex = mElogModuleGlobal->MaxDescriptors;
    Status = gBS->HandleProtocol (
                   Buffer[Index],
                   &gEfiRedirElogProtocolGuid,
                   (VOID *) &Redir
                   );
    if (EFI_ERROR (Status) || Redir == NULL) {
      continue;
    }

    for (Instance = 0; Instance < mElogModuleGlobal->MaxDescriptors; Instance++) {
      if (mElogModuleGlobal->Redir[Instance].Valid == FALSE) {
        if (EmptyIndex >= mElogModuleGlobal->MaxDescriptors) {
          EmptyIndex = Instance;
        }
      } else {
        if (Redir == mElogModuleGlobal->Redir[Instance].Command->This) {
          EmptyIndex = mElogModuleGlobal->MaxDescriptors;
          break;
          //
          // FIX: changed continue to break.
          //
        }
      }
    }

    if (EmptyIndex < mElogModuleGlobal->MaxDescriptors) {

      RedirProc = (REDIR_MODULE_PROC *) mElogModuleGlobal->Redir[EmptyIndex].Command;
      mElogModuleGlobal->Redir[EmptyIndex].Valid = TRUE;

      EfiSetFunctionEntry (&RedirProc->ActivateEventLog, *((VOID **) &Redir->ActivateEventLog));
      EfiSetFunctionEntry (&RedirProc->EraseEventLogData, *((VOID **) &Redir->EraseEventlogData));
      EfiSetFunctionEntry (&RedirProc->GetEventLogData, *((VOID **) &Redir->GetEventLogData));
      EfiSetFunctionEntry (&RedirProc->SetEventLogData, *((VOID **) &Redir->SetEventLogData));
      RedirProc->This = Redir;

      CopyMem (&RedirProc[EFI_ELOG_VIRTUAL], &RedirProc[EFI_ELOG_PHYSICAL], sizeof (REDIR_MODULE_PROC));
    }
  }

  if (Buffer != NULL) {
    FreePool (Buffer);
  }                                    

  return EFI_SUCCESS;
}

VOID
EFIAPI
NotifyElogRedirEventCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  )
/*++

Routine Description:
    This notification function is invoked when an instance of the
    ELOG REDIR protocol is produced.

Arguments:
    Event - The event that occured
    Context - For EFI compatiblity.  Not used.

--*/
{
  SetElogRedirInstances ();
}

EFI_STATUS
EFIAPI
InitializeElogLayer (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
/*++

Routine Description:

  Initialize the generic ELog driver of server management

Arguments:

  ImageHandle  - The image handle of this driver
  SystemTable  - The pointer of EFI_SYSTEM_TABLE

Returns:

  EFI_SUCCESS - The driver initialized successfully

--*/
{
  EFI_HANDLE                NewHandle;
  EFI_STATUS                Status;
  EFI_SM_ELOG_PROTOCOL      *ElogProtocol;
  EFI_EVENT                 Event;

  mElogModuleGlobal = AllocateZeroPool (sizeof (ELOG_MODULE_GLOBAL));
  ASSERT (mElogModuleGlobal != NULL);

  mElogModuleGlobal->MaxDescriptors = MAX_REDIR_DESCRIPTOR;

  //
  // Check for all IPMI Controllers
  //
  SetElogRedirInstances ();

  ElogProtocol = AllocatePool (sizeof (EFI_SM_ELOG_PROTOCOL));
  ASSERT (ElogProtocol != NULL);

  

  ElogProtocol->ActivateEventLog  = EfiActivateElog;
  ElogProtocol->EraseEventlogData = EfiEraseElogData;
  ElogProtocol->GetEventLogData   = EfiGetElogData;
  ElogProtocol->SetEventLogData   = EfiSetElogData;

  NewHandle                       = NULL;
  Status = gBS->InstallProtocolInterface (
                  &NewHandle,
                  &gEfiGenericElogProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  ElogProtocol
                  );

  ASSERT_EFI_ERROR (Status);

  //
  // Register to be notified when the ELOG REDIR protocol has been
  // produced.
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  NotifyElogRedirEventCallback,
                  NULL,
                  &Event
                  );
  if(!EFI_ERROR(Status)){
     Status = gBS->RegisterProtocolNotify (
                    &gEfiRedirElogProtocolGuid,
                    Event,
                    &mEfiElogRedirProtocolEvent
                    );
  }

#ifdef GENERICELOG_DXE_TESTING
   //
   //Design for unit testing
   //
  {
   #pragma pack(1)
    typedef struct {
      UINT8 GID;
      UINT8 ERev;
      UINT8 SType;
      UINT8 SNo;
      UINT8 EDir;
      UINT8 EData1;
      UINT8 EData2;
      UINT8 EData3;
     }ELOG_DATA;
   #pragma pack()

    ELOG_DATA TestEvent;
    UINT64     RId;

    TestEvent.GID     =  0x01;
    TestEvent.ERev    =  0x04;
    TestEvent.EDir    =  0x6F;
    TestEvent.SType   =  0x12;
    TestEvent.SNo     =  0xC0;
    TestEvent.EData1  =  0x01;
    TestEvent.EData2  =  0xAA;
    TestEvent.EData3  =  0x55;

    EfiSetElogData(
                  ElogProtocol,
                  (UINT8 *)&TestEvent,
                  1,
                  TRUE,
                  sizeof(TestEvent),
                  &RId
                  );

  }
#endif

  return Status;
}

EFI_STATUS
EfiSetFunctionEntry (
  IN  FUNCTION_PTR    *FunctionPointer,
  IN  VOID            *Function
  )
/*++

Routine Description:

  Set the function entry

Arguments:

  FunctionPointer - The destination function pointer
  Function        - The source function pointer

Returns:

  EFI_SUCCESS - Set the function pointer successfully

--*/
{
  FunctionPointer->Function = (EFI_PLABEL *) Function;
  return EFI_SUCCESS;
}
