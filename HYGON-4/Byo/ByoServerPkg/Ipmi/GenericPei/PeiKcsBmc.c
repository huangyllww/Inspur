/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PeiKcsBmc.c

Abstract:
  Source file for the IPMI PEIM.

Revision History:

**/

#include "PeiKcsBmc.h"

EFI_STATUS
KcsErrorExit (
  UINT64                            KcsTimeoutPeriod,
  UINT16                            KcsPort,
  VOID                              *Context
  )
/*++

Routine Description:

  Check the KCS error status

Arguments:
     
  IpmiInstance     - The pointer of EFI_IPMI_BMC_INSTANCE_DATA
  KcsPort          - The base port of KCS
  Context          - The Context for this operation

Returns:

  EFI_DEVICE_ERROR - The device error happened
  EFI_SUCCESS      - Successfully check the KCS error status

--*/
{
  EFI_STATUS      Status;
  UINT8           KcsData;
  EFI_KCS_STATUS  KcsStatus;
  UINT8           BmcStatus;
  UINT8           RetryCount;
  UINT64          TimeOut;

  TimeOut     = 0;
  RetryCount  = 0;
  while (RetryCount < KCS_ABORT_RETRY_COUNT) {

    TimeOut = AsmReadTsc () + KcsTimeoutPeriod;
    do {
      KcsStatus.RawData = IoRead8 (KcsPort + 1);
      if (KcsStatus.RawData == 0xFF || (AsmReadTsc () >= TimeOut)) {
        RetryCount = KCS_ABORT_RETRY_COUNT;
        break;
      }
    } while (KcsStatus.Status.Ibf);

    if (RetryCount >= KCS_ABORT_RETRY_COUNT) {
      break;
    }

    KcsData = KCS_ABORT;
    IoWrite8 ((KcsPort + 1), KcsData);

    TimeOut = AsmReadTsc () + KcsTimeoutPeriod;
    do {
      KcsStatus.RawData = IoRead8 (KcsPort + 1);
      if (KcsStatus.RawData == 0xFF || (AsmReadTsc () >= TimeOut)) {
        Status = EFI_DEVICE_ERROR;
        goto LabelError;
      }
    } while (KcsStatus.Status.Ibf);

    KcsData = IoRead8 (KcsPort);

    KcsData = 0x0;
    IoWrite8 (KcsPort, KcsData);

    TimeOut = AsmReadTsc () + KcsTimeoutPeriod;
    do {
      KcsStatus.RawData = IoRead8 (KcsPort + 1);
      if (KcsStatus.RawData == 0xFF || (AsmReadTsc () >= TimeOut)) {
        Status = EFI_DEVICE_ERROR;
        goto LabelError;
      }
    } while (KcsStatus.Status.Ibf);

    if (KcsStatus.Status.State == KcsReadState) {
      TimeOut = AsmReadTsc () + KcsTimeoutPeriod;
      do {
        KcsStatus.RawData = IoRead8 (KcsPort + 1);
        if (KcsStatus.RawData == 0xFF || (AsmReadTsc () >= TimeOut)) {
          Status = EFI_DEVICE_ERROR;
          goto LabelError;
        }
      } while (!KcsStatus.Status.Obf);

      BmcStatus = IoRead8 (KcsPort);

      KcsData = KCS_READ;
      IoWrite8 (KcsPort, KcsData);

      TimeOut = AsmReadTsc () + KcsTimeoutPeriod;
      do {
        KcsStatus.RawData = IoRead8 (KcsPort + 1);
        if (KcsStatus.RawData == 0xFF || (AsmReadTsc () >= TimeOut)) {
          Status = EFI_DEVICE_ERROR;
          goto LabelError;
        }
      } while (KcsStatus.Status.Ibf);

      if (KcsStatus.Status.State == KcsIdleState) {
        TimeOut = AsmReadTsc () + KcsTimeoutPeriod;
        do {
          KcsStatus.RawData = IoRead8 (KcsPort + 1);
          if (KcsStatus.RawData == 0xFF || (AsmReadTsc () >= TimeOut)) {
            Status = EFI_DEVICE_ERROR;
            goto LabelError;
          }
        } while (!KcsStatus.Status.Obf);

        KcsData = IoRead8 (KcsPort);
        break;

      } else {
        RetryCount++;
        continue;
      }

    } else {
      RetryCount++;
      continue;
    }
  }

  if (RetryCount >= KCS_ABORT_RETRY_COUNT) {
    Status = EFI_DEVICE_ERROR;
    goto LabelError;
  }

  return EFI_SUCCESS;

LabelError:
  if (AsmReadTsc () >= TimeOut) {
    //
  }
  return Status;
}

EFI_STATUS
KcsCheckStatus (
  UINT64                            KcsTimeoutPeriod,
  UINT16                            KcsPort,
  KCS_STATE                         KcsState,
  BOOLEAN                           *Idle,
  VOID                              *Context
  )
/*++

Routine Description:

  Check KCS status

Arguments:

  IpmiInstance  - The pointer of EFI_IPMI_BMC_INSTANCE_DATA
  KcsPort       - The base port of KCS
  KcsState      - The state of KCS to be checked
  Idle          - If the KCS is idle
  Context       - The context for this operation

Returns:

  EFI_SUCCESS   - Checked the KCS status successfully

--*/
{
  EFI_STATUS      Status;
  EFI_KCS_STATUS  KcsStatus;
  UINT8           KcsData;
  UINT64          TimeOut;
  
  if(Idle == NULL ){ 
       return EFI_INVALID_PARAMETER;
  }
	
   *Idle   = FALSE;
   
  TimeOut = AsmReadTsc () + KcsTimeoutPeriod;
  do {
    KcsStatus.RawData = IoRead8 (KcsPort + 1);
    if (KcsStatus.RawData == 0xFF || (AsmReadTsc () >= TimeOut)) {
      Status = EFI_DEVICE_ERROR;
      goto LabelError;
    }
  } while (KcsStatus.Status.Ibf);

  if (KcsState == KcsWriteState) {
    KcsData = IoRead8 (KcsPort);
  }

  if (KcsStatus.Status.State != KcsState) {
    if ((KcsStatus.Status.State == KcsIdleState) && (KcsState == KcsReadState)) {
      *Idle = TRUE;
    } else {
      Status = KcsErrorExit (KcsTimeoutPeriod, KcsPort, Context);
      goto LabelError;
    }
  }

  if (KcsState == KcsReadState) {
    TimeOut = AsmReadTsc () + KcsTimeoutPeriod;
    do {
      KcsStatus.RawData = IoRead8 (KcsPort + 1);
      if (KcsStatus.RawData == 0xFF || (AsmReadTsc () >= TimeOut)) {
        Status = EFI_DEVICE_ERROR;
        goto LabelError;
      }
    } while (!KcsStatus.Status.Obf);
  }

  if (KcsState == KcsWriteState || Idle) {
    KcsData = IoRead8 (KcsPort);
  }

  return EFI_SUCCESS;

LabelError:
  if (AsmReadTsc () >= TimeOut) {
    //
  }
  return Status;
}

EFI_STATUS
SendDataToBmc (
  UINT64                          KcsTimeoutPeriod,
  UINT16                          KcsPort,
  VOID                            *Context,
  UINT8                           *Data,
  UINT8                           DataSize
  )
/*++

Routine Description:

  Send data to BMC

Arguments:

  IpmiInstance  - The pointer of EFI_IPMI_BMC_INSTANCE_DATA
  Context       - The context of this operation
  Data          - The data pointer to be sent
  DataSize      - The data size

Returns:

  EFI_SUCCESS   - Send out the data successfully

--*/
{
  EFI_KCS_STATUS  KcsStatus;
  UINT8           KcsData;
  UINT16          KcsIoBase;
  EFI_STATUS      Status;
  UINT8           i;
  BOOLEAN         Idle;
  UINT64          TimeOut;

  KcsIoBase = KcsPort;

  TimeOut = AsmReadTsc() + KcsTimeoutPeriod;

  do {
    KcsStatus.RawData = IoRead8 (KcsIoBase + 1);
    if (KcsStatus.RawData == 0xFF || AsmReadTsc() >= TimeOut)
    {
      if ((Status = KcsErrorExit (KcsTimeoutPeriod, KcsIoBase, Context)) != EFI_SUCCESS)
      {
        return Status;
      }
    }
  } while (KcsStatus.Status.Ibf);

  KcsData = KCS_WRITE_START;
  IoWrite8 ((KcsIoBase + 1), KcsData);
  if ((Status = KcsCheckStatus (KcsTimeoutPeriod, KcsIoBase, KcsWriteState, &Idle, Context)) != EFI_SUCCESS) {
    return Status;
  }

  for (i = 0; i < DataSize; i++) {
    if (i == (DataSize - 1)) {
      if ((Status = KcsCheckStatus (KcsTimeoutPeriod, KcsIoBase, KcsWriteState, &Idle, Context)) != EFI_SUCCESS) {
        return Status;
      }

      KcsData = KCS_WRITE_END;
      IoWrite8 ((KcsIoBase + 1), KcsData);
    }

    Status = KcsCheckStatus (KcsTimeoutPeriod, KcsIoBase, KcsWriteState, &Idle, Context);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    IoWrite8 (KcsIoBase, Data[i]);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ReceiveBmcData (
  UINT64                          KcsTimeoutPeriod,
  UINT16                          KcsPort,
  VOID                            *Context,
  UINT8                           *Data,
  UINT8                           *DataSize
  )
/*++

Routine Description:

  Routine Description:

  Receive data from BMC

Arguments:

  IpmiInstance  - The pointer of EFI_IPMI_BMC_INSTANCE_DATA
  Context       - The context of this operation
  Data          - The buffer pointer
  DataSize      - The buffer size

Returns:

  EFI_SUCCESS   - Received data successfully

--*/
{
  UINT8       KcsData;
  UINT16      KcsIoBase;
  EFI_STATUS  Status;
  BOOLEAN     Idle;
  UINT8       Count;

  Count     = 0;
  KcsIoBase = KcsPort;

  while (TRUE) {

    if ((Status = KcsCheckStatus (KcsTimeoutPeriod, KcsIoBase, KcsReadState, &Idle, Context)) != EFI_SUCCESS) {
      return Status;
    }

    if (Idle) {
      *DataSize = Count;
      break;
    }

    if (Count > *DataSize) {
      return EFI_DEVICE_ERROR;
    }

    Data[Count] = IoRead8 (KcsIoBase);

    Count++;

    KcsData = KCS_READ;
    IoWrite8 (KcsIoBase, KcsData);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ReceiveBmcDataFromPort (
  UINT64                          KcsTimeoutPeriod,
  UINT16                          KcsPort,  
  VOID                            *Context,
  UINT8                           *Data,
  UINT8                           *DataSize
  )
/*++

Routine Description:

  Receive data from BMC

Arguments:

  IpmiInstance  - The pointer of EFI_IPMI_BMC_INSTANCE_DATA
  Context       - The context of this operation
  Data          - The buffer pointer to receive data
  DataSize      - The buffer size

Returns:

  EFI_SUCCESS   - Received the data successfully

--*/
{
  EFI_STATUS  Status;
  UINT16      KcsIoBase;
  UINT8       i;
  UINT8       MyDataSize;

  MyDataSize  = *DataSize;

  KcsIoBase   = KcsPort;

  for (i = 0; i < KCS_ABORT_RETRY_COUNT; i++) {
    Status = ReceiveBmcData (KcsTimeoutPeriod, KcsIoBase, Context, Data, DataSize);
    if (EFI_ERROR (Status)) {
      if ((Status = KcsErrorExit (KcsTimeoutPeriod, KcsIoBase, Context)) != EFI_SUCCESS) {
        return Status;
      }

      *DataSize = MyDataSize;
    } else {
      return Status;
    }
  }

  return EFI_DEVICE_ERROR;
}

EFI_STATUS
SendDataToBmcPort (
  UINT64                          KcsTimeoutPeriod,
  UINT16                          KcsPort,  
  VOID                            *Context,
  UINT8                           *Data,
  UINT8                           DataSize
  )
/*++

Routine Description:

  Send data to BMC

Arguments:

  IpmiInstance  - The pointer of EFI_IPMI_BMC_INSTANCE_DATA
  Context       - The context of this operation
  Data          - The data pointer to be sent
  DataSize      - The data size

Returns:

  EFI_SUCCESS   - Send out the data successfully

--*/
{
  EFI_STATUS  Status;
  UINT16      KcsIoBase;
  UINT8       i;

  KcsIoBase = KcsPort;

  for (i = 0; i < KCS_ABORT_RETRY_COUNT; i++) {
    Status = SendDataToBmc (KcsTimeoutPeriod, KcsIoBase, Context, Data, DataSize);
    if (EFI_ERROR (Status)) {
      if ((Status = KcsErrorExit (KcsTimeoutPeriod, KcsIoBase, Context)) != EFI_SUCCESS) {
        return Status;
      }
    } else {
      return Status;
    }
  }

  return EFI_DEVICE_ERROR;
}
