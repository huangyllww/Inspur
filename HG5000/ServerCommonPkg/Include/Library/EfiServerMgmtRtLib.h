/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _EFI_SERVER_MEMT_LIB_H_
#define _EFI_SERVER_MEMT_LIB_H_

#include <Protocol/GenericElog.h>

//
// ELOG Library Initializer.
//
EFI_STATUS
EfiInitializeGenericElog (
  VOID
  )
/*++

  Routine Description:
    The function will set up a notification on the ELOG protocol.  This function is required to be called prior
    to utilizing the ELOG protocol from within this library.

  Arguments:
    None

  Returns:
    EFI_SUCCESS - after the notificication has been setup.

--*/
;

EFI_STATUS
EfiSmSetEventLogData (
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
    RecordId - is the array of record IDs sent by the target.  This can be used to retieve the
               records or erase the records.

  Returns:
    EFI_SUCCESS - if the data was logged.
    EFI_INVALID_PARAMETER - if the DataType is >= EfiSmElogMax
    EFI_OUT_OF_RESOURCES - if the DataSize is larger than the elog temp buffer and we cannot log the record
    EFI_NOT_FOUND - the event log target was not found
    EFI_PROTOCOL_ERROR - there was a data formatting error

--*/
;

EFI_STATUS
EfiSmGetEventLogData (
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  OUT UINTN                         *DataSize,
  IN OUT UINT64                         *RecordId
  )
/*++

  Routine Description:
    This function gets event log data from the destination dependant on the DataType.  The destination
    can be a remote target such as LAN, ICMB, IPMI, or a FV.  The ELOG redir driver will resolve the
    destination.

  Arguments:
    ElogData - a pointer to the an event log data buffer to contain the data to be retrieved.
    DataType - this is the type of elog data to be gotten.  Elog is redirected based upon this
               information.
    DataSize - this is the size of the data to be retrieved.
    RecordId - the RecordId of the next record.  If ElogData is NULL, this gives the RecordId of the first 
               record available in the database with the correct DataSize.  A value of 0 on return indicates
               that it was last record if the Status is EFI_SUCCESS.

  Returns:
    EFI_SUCCESS - if the event log was retrieved successfully.
    EFI_NOT_FOUND - if the event log target was not found.
    EFI_NO_RESPONSE - if the event log target is not responding.  This is done by the redir driver.
    EFI_INVALID_PARAMETER - DataType or another parameter was invalid.
    EFI_BUFFER_TOO_SMALL - the ElogData buffer is too small to be filled with the requested data.

--*/
;

EFI_STATUS
EfiSmEraseEventlogData (
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINT64                         *RecordId
  )
/*++

  Routine Description:
    This function erases the event log data defined by the DataType.  The redir driver associated with
    the DataType resolves the path to the record.

  Arguments:
    DataType - the type of elog data that is to be erased.  
    RecordId - the RecordId of the data to be erased.  If RecordId is NULL,  all records in the
               database are erased if permitted by the target.  RecordId will contain the deleted
               RecordId on return.

  Returns:
    EFI_SUCCESS - the record or collection of records were erased.
    EFI_NOT_FOUND - the event log target was not found.
    EFI_NO_RESPONSE - the event log target was found but did not respond.
    EFI_INVALID_PARAMETER - one of the parameters was invalid.

--*/
;

EFI_STATUS
EfiSmActivateEventLog (
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN BOOLEAN                            *EnableElog,
  OUT BOOLEAN                           *ElogStatus
  )
/*++

  Routine Description:
    This function enables or disables the event log defined by the DataType.

  Arguments:
    DataType - the type of elog data that is being activated.
    EnableElog - enables or disables the event log defined by the DataType.  If it is NULL
                 it returns the current status of the DataType log.
    ElogStatus - is the current status of the Event log defined by the DataType.  Enabled is 
                 TRUE and Disabled is FALSE.

  Returns:
    EFI_SUCCESS - if the event log was successfully enabled or disabled.
    EFI_NOT_FOUND - the event log target was not found.
    EFI_NO_RESPONSE - the event log target was found but did not respond.
    EFI_INVALID_PARAMETER - one of the parameters was invalid.

--*/
;

EFI_STATUS
EfiSmGetTimeStamp (
  OUT UINT32 *NumOfSeconds
  )
/*+++

Routine Description:

  Return Date and Time from RTC in Unix format which fits in 32 bit format

Arguments:

  NumOfSeconds - pointer to return calculated time

Returns:

  EFI_SUCCESS
  EFI status if error occurred

--*/
;

#endif
