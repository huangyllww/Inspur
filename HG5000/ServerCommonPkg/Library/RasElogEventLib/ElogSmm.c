/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ElogSmm

Abstract:
  Source file for the RasElogEventLib.

Revision History:

**/
#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/IpmiBaseLib.h>
#include <Protocol/GenericElog.h>

EFI_SM_ELOG_PROTOCOL  *mGenericElogProtocol     = NULL;

EFI_STATUS
SetEventLogData (
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId
  )
/*++

  Routine Description:
    This function sends event log data to the destination such as LAN, ICMB, BMC etc.  

  Arguments:
    ElogData - is a pointer to the event log data that needs to be recorded
    DataType - type of elog data that is being recorded.  The elog is redirected based on this
               parameter.
    AlertEvent - is an indication that the input data type is an alert.  The underlying
                 drivers need to decide if they need to listen to the DataType and send it on
                 an appropriate channel as an alert use of the information.
    DataSize - is the size of the data to be logged
    RecordId -is the array of record IDs sent by the target.  This can be used to retieve the
              records or erase the records.

  Returns:
    EFI_SUCCESS - if the data was logged.
    EFI_INVALID_PARAMETER - if the DataType is >= EfiSmElogMax
    EFI_OUT_OF_RESOURCES - if the DataSize is larger than the elog temp buffer and we cannot log the record
    EFI_NOT_FOUND - the event log target was not found
    EFI_PROTOCOL_ERROR - there was a data formatting error

--*/
{

  return mGenericElogProtocol->SetEventLogData (
                                mGenericElogProtocol,
                                ElogData,
                                DataType,
                                AlertEvent,
                                DataSize,
                                RecordId
                                );
}

EFI_STATUS
GetGenericElogProtocol (
)
{
    EFI_STATUS Status = EFI_SUCCESS;
    //
    // If the protocol is not found return EFI_NOT_FOUND
    //
    if (mGenericElogProtocol == NULL) {
        Status = gSmst->SmmLocateProtocol(&gSmmGenericElogProtocolGuid, NULL, &mGenericElogProtocol);
        DEBUG((EFI_D_ERROR, "zzzzzzzzz SMM  %a L%d Status %r\n", __FUNCTION__, __LINE__, Status));
        if (EFI_ERROR(Status)) {
            mGenericElogProtocol = NULL;
        }
    }
    return Status;
}

EFI_STATUS
SendErrorLogDataToBmc (
  IN  UINT8                         *ElogData,
  IN  UINTN                         DataSize
  )
{
  EFI_STATUS                  Status;
  UINT8                       ResponseData[10];
  UINT8                       ResponseDataSize = 4;
  DEBUG((EFI_D_ERROR, "SendErrorLogDataToBmc \n"));
  Status = EfiInitializeIpmiBase ();
  DEBUG((EFI_D_ERROR, "EfiInitializeIpmiBase: Ipmi-%r\n", Status));


  Status = EfiSendCommandToBMC (
            0x0A,
            0x44,
            (UINT8 *) ElogData,
            (UINT8) DataSize,
            (UINT8 *) &ResponseData,
            (UINT8 *) &ResponseDataSize
            );

  return Status;
}

