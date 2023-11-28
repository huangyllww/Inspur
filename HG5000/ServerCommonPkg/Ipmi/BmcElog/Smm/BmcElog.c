/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BmcElog.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#include "BmcElog.h"

//
// Define module globals used to register for notification of when
// the ELOG REDIR protocol has been produced.
//
VOID                        *mEfiBmcTransNotifyReg;
EFI_EVENT                   mEfiBmcTransEvent;
EFI_BMC_ELOG_INSTANCE_DATA  *mRedirProtoPrivate;

/**
    This function verifies if the BMC SEL is full.

    @param VOID

    @retval TRUE If SEL is full
    @retval False If SEL is not full

**/
BOOLEAN
CheckIfSelIsFull (
  VOID
  )
{
  EFI_STATUS    Status;
  UINT32        ResponseDataSize;
  UINT8         OperationSupportByte;
  UINT8         SelIsFull;
  EFI_SEL_INFO  EfiSelInfo;

  OperationSupportByte  = 0;
  SelIsFull             = 0;
  ResponseDataSize      = sizeof (EFI_SEL_INFO);

  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_GET_SEL_INFO,
             NULL,
             0,
             (UINT8 *) &EfiSelInfo,
             (UINT8 *) &ResponseDataSize
             );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  return  (EfiSelInfo.OperationSupport & BIT7);
}

/*++

  Routine Description:
    This function verifies the BMC SEL is full and When it is reports the error to the Error Manager.

  Arguments:
    None

  Returns:
    EFI_SUCCESS
    EFI_DEVICE_ERROR

--*/
EFI_STATUS
WaitTillErased (
  EFI_BMC_ELOG_INSTANCE_DATA            *BmcElogPrivateData,
  UINT8                                 *ResvId
  )
/*++

Routine Description:

Arguments:

  BmcElogPrivateData  - Bmc event log instance
  ResvId              - Reserved ID

Returns:

  EFI_SUCCESS
  EFI_NO_RESPONSE

--*/
{
  UINT32        DataSize;
  INTN          Counter;
  UINT8         ResponseData[10];
  UINT8         ResponseDataSize;
  EFI_CLEAR_SEL EfiClearSel;

  DataSize  = 7;
  Counter   = 0x200;

  while (TRUE) {
    EfiClearSel.Reserve[0]  = ResvId[0];
    EfiClearSel.Reserve[1]  = ResvId[1];
    EfiClearSel.AscC        = 0x43;
    EfiClearSel.AscL        = 0x4C;
    EfiClearSel.AscR        = 0x52;
    EfiClearSel.Erase       = 0x00;
    ResponseDataSize        = 10;

    EfiSendCommandToBMC (
      EFI_SM_NETFN_STORAGE,
      EFI_STORAGE_CLEAR_SEL,
      (UINT8 *) &EfiClearSel,
      sizeof (EfiClearSel),
      (UINT8 *) &ResponseData,
      (UINT8 *) &ResponseDataSize
      );

    if ((ResponseData[0] & 0xf) == 1) {
      return EFI_SUCCESS;
    }
    //
    //  If there is not a response from the BMC controller we need to return and not hang.
    //
    --Counter;
    if (Counter == 0x0) {
      return EFI_NO_RESPONSE;
    }
  }
}

EFI_STATUS
EfiSetBmcElogData (
  IN  EFI_SM_ELOG_REDIR_PROTOCOL        *This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             Size,
  OUT UINT64                            *RecordId
  )
/*++

Routine Description:

Arguments:

  This        - Protocol pointer
  ElogData    - Buffer for log storage
  DataType    - Event Log type
  AlertEvent  - If it is a alert event
  Size        - Log data size
  RecordId    - Indicate which recorder it is

Returns:

  EFI_STATUS

--*/
{
  EFI_BMC_ELOG_INSTANCE_DATA  *BmcElogPrivateData;
  EFI_STATUS                  Status;
  UINT8                       SelRecordToAdd[MAX_TEMP_DATA];
  UINT8                       ResponseData[10];
  UINT8                       ResponseDataSize;
  UINT32                      DataSize;

  DEBUG((EFI_D_ERROR,"%a SMM \n",__FUNCTION__));

  Status              = EFI_SUCCESS;
  DataSize            = (UINT32) Size + 1;
  BmcElogPrivateData  = INSTANCE_FROM_EFI_SM_ELOG_REDIR_THIS (This);

  if (DataType >= EfiSmElogMax) {
    return EFI_INVALID_PARAMETER;
  }

  if (DataSize > MAX_TEMP_DATA) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Check if SEL is full before logging any entries.
  // If SEL is full, then return EFI_OUT_OF_RESOURCES.
  //
  if (CheckIfSelIsFull()) {
    return EFI_OUT_OF_RESOURCES;
  }

  if (BmcElogPrivateData->DataType == DataType) {
    CopyMem (SelRecordToAdd, ElogData, (DataSize - 1));

    *RecordId = 0;

    if (AlertEvent) {
      ResponseDataSize = 1;
      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_SENSOR,
                EFI_SENSOR_PLATFORM_EVENT_MESSAGE,
                (UINT8 *) SelRecordToAdd,
                (UINT8) Size,
                (UINT8 *) &ResponseData,
                (UINT8 *) &ResponseDataSize
                );
    } else {
      ResponseDataSize = 10;
      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_STORAGE,
                EFI_STORAGE_ADD_SEL_ENTRY,
                (UINT8 *) SelRecordToAdd,
                (UINT8) Size,
                (UINT8 *) &ResponseData,
                (UINT8 *) &ResponseDataSize
                );

      if (Status == EFI_SUCCESS) {
        *RecordId = (UINT16) (*((UINT16 *) &ResponseData[0]));
      }
    }
  }

  return Status;
}

EFI_STATUS
EfiGetBmcElogData (
  IN EFI_SM_ELOG_REDIR_PROTOCOL         *This,
  IN UINT8                              *ElogData,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINTN                          *Size,
  IN OUT UINT64                         *RecordId
  )
/*++

Routine Description:

Arguments:

  This        - Protocol pointer
  ElogData    - Buffer for log data store
  DataType    - Event log type
  Size        - Size of log data
  RecordId    - indicate which recorder it is

Returns:

  EFI_STATUS

--*/
{
  EFI_BMC_ELOG_INSTANCE_DATA  *BmcElogPrivateData;
  UINT64                      ReceiveKey;
  EFI_STATUS                  Status;
  UINT32                      DataSize;
  EFI_GET_SEL_ENTRY           EfiGetSelEntry;
  UINT8                       ResponseData[25];
  UINT8                       ResponseDataSize;

  Status              = EFI_SUCCESS;
  DataSize            = 7;
  DEBUG((EFI_D_ERROR,"%a SMM\n",__FUNCTION__));

  ReceiveKey          = *RecordId;
  BmcElogPrivateData  = INSTANCE_FROM_EFI_SM_ELOG_REDIR_THIS (This);

  if (DataType >= EfiSmElogMax) {
    return EFI_INVALID_PARAMETER;
  }

  if (BmcElogPrivateData->DataType == DataType) {
    EfiGetSelEntry.ReserveId[0] = 0;
    EfiGetSelEntry.ReserveId[1] = 0;
    EfiGetSelEntry.SelRecID[0]  = (UINT8) ReceiveKey;
    ReceiveKey                  = DivU64x32 (ReceiveKey, (UINT32) (1 << 8));
    EfiGetSelEntry.SelRecID[1]  = (UINT8) ReceiveKey;
    ReceiveKey                  = DivU64x32 (ReceiveKey, (UINT32) (1 << 24));
    EfiGetSelEntry.Offset       = 0;
    EfiGetSelEntry.BytesToRead  = EFI_COMPLETE_SEL_RECORD;
    ResponseDataSize            = 25;

    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_STORAGE,
              EFI_STORAGE_GET_SEL_ENTRY,
              (UINT8 *) &EfiGetSelEntry,
              sizeof (EfiGetSelEntry),
              (UINT8 *) &ResponseData,
              (UINT8 *) &ResponseDataSize
              );
    if (Status == EFI_SUCCESS) {
      if (*Size < (DataSize - 2)) {
        return EFI_BUFFER_TOO_SMALL;
      }

      if (ResponseData[0] == 0xFF && ResponseData[1] == 0xFF) {
        return EFI_NOT_FOUND;
      }

      *RecordId = (UINT16) (*((UINT16 *) &ResponseData[0]));
      CopyMem (ElogData, &ResponseData[2], (ResponseDataSize - 2));
      *Size = DataSize - 2;
    }
  }

  return Status;
}

EFI_STATUS
EfiEraseBmcElogData (
  IN EFI_SM_ELOG_REDIR_PROTOCOL         *This,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINT64                         *RecordId
  )
/*++

Routine Description:

Arguments:

  This        - Protocol pointer
  DataType    - Event log type
  RecordId    - return which recorder it is

Returns:

  EFI_STATUS

--*/
{
  EFI_BMC_ELOG_INSTANCE_DATA  *BmcElogPrivateData;
  EFI_STATUS                  Status;
  UINT32                      DataSize;
  UINT64                      ReceiveKey;
  UINT8                       ResvId[2];
  UINT8                       GetSelInfoOutput[14];
  UINT8                       SelReserveIdIsSupported;
  UINT8                       OperationSupport;
  UINT8                       SelReserveIdvalue;
  EFI_DELETE_SEL              EfiDeleteSel;
  UINT8                       ResponseData[15];
  UINT8                       ResponseDataSize;
  EFI_CLEAR_SEL               EfiClearSel;

  Status              = EFI_SUCCESS;
  DEBUG((EFI_D_ERROR,"%a SMM\n",__FUNCTION__));
  
  BmcElogPrivateData  = INSTANCE_FROM_EFI_SM_ELOG_REDIR_THIS (This);
  if (DataType >= EfiSmElogMax) {
    return EFI_INVALID_PARAMETER;
  }

  if (BmcElogPrivateData->DataType == DataType) {
    //
    // Before issueing this SEL reservation ID, Check whether this command is supported or not by issuing the
    // GetSelInfoCommand. If it does not support ResvId should be 0000h
    //
    ResponseDataSize = 15;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_STORAGE,
              EFI_STORAGE_GET_SEL_INFO,
              NULL,
              0,
              (UINT8 *) &ResponseData,
              (UINT8 *) &ResponseDataSize
              );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    CopyMem (GetSelInfoOutput, &ResponseData[0], 14);
    OperationSupport  = GetSelInfoOutput[13];
    SelReserveIdvalue = (OperationSupport & 0x02);
    if (SelReserveIdvalue == SEL_RESERVE_ID_SUPPORT) {
      SelReserveIdIsSupported = 1;
    } else {
      SelReserveIdIsSupported = 0;
    }
    //
    // if SelReserveIdcommand not supported do not issue the RESERVE_SEL_ENTRY command, and set the ResvId value to 0000h
    //
    //
    // Get the SEL reservation ID
    //
    if (SelReserveIdIsSupported) {
      ResponseDataSize = 15;
      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_STORAGE,
                EFI_STORAGE_RESERVE_SEL,
                NULL,
                0,
                (UINT8 *) &ResponseData,
                (UINT8 *) &ResponseDataSize
                );
      if (EFI_ERROR (Status)) {
        return Status;
      }

      CopyMem (ResvId, &ResponseData[0], 2);
    } else {
      ResvId[0] = 0x00;
      ResvId[1] = 0x00;
    }
    //
    // Clear the SEL
    //
    if (RecordId != NULL) {
      ReceiveKey                      = *RecordId;
      EfiDeleteSel.ReserveId[0]       = ResvId[0];
      EfiDeleteSel.ReserveId[1]       = ResvId[1];
      EfiDeleteSel.RecordToDelete[0]  = (UINT8) ReceiveKey;
      DivU64x32 (ReceiveKey, (UINT32) (1 >> 8));
      EfiDeleteSel.RecordToDelete[1] = (UINT8) ReceiveKey;
      DivU64x32 (ReceiveKey, (UINT32) (1 >> 24));
      DataSize          = sizeof (EfiDeleteSel);

      ResponseDataSize  = 15;
      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_STORAGE,
                EFI_STORAGE_DELETE_SEL_ENTRY,
                (UINT8 *) &EfiDeleteSel,
                (UINT8) DataSize,
                (UINT8 *) &ResponseData,
                (UINT8 *) &ResponseDataSize
                );

    } else {
      EfiClearSel.Reserve[0]  = ResvId[0];
      EfiClearSel.Reserve[1]  = ResvId[1];
      EfiClearSel.AscC        = 0x43;
      EfiClearSel.AscL        = 0x4C;
      EfiClearSel.AscR        = 0x52;
      EfiClearSel.Erase       = 0xAA;
      DataSize                = sizeof (EfiClearSel);

      ResponseDataSize        = 15;
      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_STORAGE,
                EFI_STORAGE_CLEAR_SEL,
                (UINT8 *) &EfiClearSel,
                (UINT8) DataSize,
                (UINT8 *) &ResponseData,
                (UINT8 *) &ResponseDataSize
                );

    }

    if (Status == EFI_SUCCESS) {
      if (RecordId != NULL) {
        *RecordId = (UINT16) (*((UINT16 *) &BmcElogPrivateData->TempData[0]));
      } else {
        WaitTillErased (BmcElogPrivateData, ResvId);
      }
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EfiActivateBmcElog (
  IN EFI_SM_ELOG_REDIR_PROTOCOL         *This,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN BOOLEAN                            *EnableElog,
  OUT BOOLEAN                           *ElogStatus
  )
/*++

Routine Description:

Arguments:

  This        - Protocol pointer
  DataType    - indicate event log type
  EnableElog  - Enable/Disable event log
  ElogStatus  - return log status

Returns:

  EFI_STATUS

--*/
{
  EFI_BMC_ELOG_INSTANCE_DATA  *BmcElogPrivateData;
  EFI_STATUS                  Status;
  UINT8                       ElogStat;
  UINT8                       ResponseData[10];
  UINT8                       ResponseDataSize;

  Status              = EFI_SUCCESS;
  ElogStat            = 0;

  BmcElogPrivateData  = INSTANCE_FROM_EFI_SM_ELOG_REDIR_THIS (This);

  if (DataType >= EfiSmElogMax) {
    return EFI_INVALID_PARAMETER;
  }
  DEBUG((EFI_D_ERROR,"%a SMM\n",__FUNCTION__));

  ElogStat = 0;
  if (BmcElogPrivateData->DataType == DataType) {
    ResponseDataSize = 10;
    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_APP,
               EFI_APP_GET_BMC_GLOBAL_ENABLES,
               NULL,
               0,
               (UINT8 *) &ResponseData,
               (UINT8 *) &ResponseDataSize
               );

    if (EnableElog == NULL) {
      *ElogStatus = (UINT8) (0x1 & (ResponseData[0] >> 3));
    } else {
      if (Status == EFI_SUCCESS) {
        if (*EnableElog) {
          ElogStat = 0x8;
        }

        ElogStat          = (ElogStat | (0xF7 & ResponseData[0]));
        ResponseDataSize  = 10;
        Status = EfiSendCommandToBMC (
                   EFI_SM_NETFN_APP,
                   EFI_APP_SET_BMC_GLOBAL_ENABLES,
                   (UINT8 *) &ElogStat,
                   1,
                   (UINT8 *) &ResponseData,
                   (UINT8 *) &ResponseDataSize
                   );
      }
    }
  }

  return Status;
}

EFI_STATUS
SetElogRedirInstall (
  VOID
  )
/*++

Routine Description:

Arguments:

  None

Returns:

  EFI_SUCCESS

--*/
{
  EFI_HANDLE  NewHandle;
  EFI_STATUS  Status;
  BOOLEAN     EnableElog;
  BOOLEAN     ElogStatus;
  UINT16      Instance;

  Status        = EFI_SUCCESS;
  EnableElog    = TRUE;
  ElogStatus    = TRUE;
  Instance      = 0;

  mRedirProtoPrivate->Signature                 = SM_ELOG_REDIR_SIGNATURE;
  mRedirProtoPrivate->DataType                  = EfiElogSmIPMI;
  mRedirProtoPrivate->BmcElog.ActivateEventLog  = EfiActivateBmcElog;
  mRedirProtoPrivate->BmcElog.EraseEventlogData = EfiEraseBmcElogData;
  mRedirProtoPrivate->BmcElog.GetEventLogData   = EfiGetBmcElogData;
  mRedirProtoPrivate->BmcElog.SetEventLogData   = EfiSetBmcElogData;
  mRedirProtoPrivate->Instance                  = Instance;
  //
  // Now install the Protocol
  //
  NewHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &NewHandle,
                    &gSmmRedirElogProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mRedirProtoPrivate->BmcElog
                    );

  ASSERT_EFI_ERROR (Status);
  //
  // Activate the Event Log (This should depend upon Setup).
  //
  EfiActivateBmcElog (&mRedirProtoPrivate->BmcElog, EfiElogSmIPMI, &EnableElog, &ElogStatus);
  return EFI_SUCCESS;
}

VOID
EFIAPI
NotifyBmcTransCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
/*++

Routine Description:
    This notification function is invoked when an instance of the
    IPMI Transport Layer is produced.

Arguments:
    Event - The event that occured
    Context - For EFI compatiblity.  Not used.

Returns:
    None

--*/
{
  SetElogRedirInstall ();
}

EFI_STATUS
InitializeSmBmcElogLayer (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
/*++

Routine Description:

Arguments:

  ImageHandle - ImageHandle of the loaded driver
  SystemTable - Pointer to the System Table

Returns:

  EFI_STATUS

--*/
{
  EFI_STATUS  Status;

  Status  = EFI_SUCCESS;

  EfiInitializeIpmiBase ();

  mRedirProtoPrivate = AllocatePool (sizeof (EFI_BMC_ELOG_INSTANCE_DATA));
  ASSERT (mRedirProtoPrivate!=NULL);

  SetElogRedirInstall ();

  return Status;

}
