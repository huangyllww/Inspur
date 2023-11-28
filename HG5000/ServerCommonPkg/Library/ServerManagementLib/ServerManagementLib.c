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

#include <PiDxe.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/GenericElog.h>

#include <Library/EfiServerMgmtRtLib.h>

//#include EFI_PROTOCOL_DEPENDENCY (CpuIo)

#define PCAT_RTC_ADDRESS_REGISTER                   0x70
#define PCAT_RTC_DATA_REGISTER                      0x71

#define RTC_ADDRESS_SECONDS           0   // R/W  Range 0..59
#define RTC_ADDRESS_MINUTES           2   // R/W  Range 0..59
#define RTC_ADDRESS_HOURS             4   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define RTC_ADDRESS_DAY_OF_THE_MONTH  7   // R/W  Range 1..31
#define RTC_ADDRESS_MONTH             8   // R/W  Range 1..12
#define RTC_ADDRESS_YEAR              9   // R/W  Range 0..99
#define RTC_ADDRESS_REGISTER_A        10  // R/W[0..6]  R0[7]
#define RTC_ADDRESS_REGISTER_B        11  // R/W
#define RTC_ADDRESS_REGISTER_C        12  // RO
#define RTC_ADDRESS_REGISTER_D        13  // RO
#define RTC_ADDRESS_CENTURY           50  // R/W  Range 19..20 Bit 8 is R/W


//
// Register A
//
typedef struct {
  UINT8 RS : 4;   // Rate Selection Bits
  UINT8 DV : 3;   // Divisor
  UINT8 UIP : 1;  // Update in progress
} RTC_REGISTER_A_BITS;

typedef union {
  RTC_REGISTER_A_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_A;

//
// Register B
//
typedef struct {
  UINT8 DSE : 1;  // 0 - Daylight saving disabled  1 - Daylight savings enabled
  UINT8 MIL : 1;  // 0 - 12 hour mode              1 - 24 hour mode
  UINT8 DM : 1;   // 0 - BCD Format                1 - Binary Format
  UINT8 SQWE : 1; // 0 - Disable SQWE output       1 - Enable SQWE output
  UINT8 UIE : 1;  // 0 - Update INT disabled       1 - Update INT enabled
  UINT8 AIE : 1;  // 0 - Alarm INT disabled        1 - Alarm INT Enabled
  UINT8 PIE : 1;  // 0 - Periodic INT disabled     1 - Periodic INT Enabled
  UINT8 SET : 1;  // 0 - Normal operation.         1 - Updates inhibited
} RTC_REGISTER_B_BITS;

typedef union {
  RTC_REGISTER_B_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_B;

//
// Register D
//
typedef struct {
  UINT8 Reserved : 7; // Read as zero.  Can not be written.
  UINT8 VRT : 1;      // Valid RAM and Time
} RTC_REGISTER_D_BITS;

typedef union {
  RTC_REGISTER_D_BITS Bits;
  UINT8               Data;
} RTC_REGISTER_D;


//
// Module Globals
//
EFI_SM_ELOG_PROTOCOL  *mGenericElogProtocol     = NULL;
VOID                  *mGenericElogRegistration = NULL;
//EFI_CPU_IO_PROTOCOL   *mCpuIo = NULL;

INTN DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

VOID
EFIAPI
GenericElogNotificationFunction (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
/*++
  
  Routine Description:
    This function is called whenever an instance of ELOG protocol is created.  When the function is notified
    it initializes the module global data.

  Arguments:
    Event - This is used only for EFI compatability
    Context      - This is used only for EFI compatability
  
  Returns:
    EFI_SUCCESS - if the ELOG protocol is found and the module global data is initialized
    EFI_NOT_FOUND - if the ELOG protocol could not be found
  
--*/
{
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol(&gEfiGenericElogProtocolGuid, NULL, &mGenericElogProtocol);
  if (EFI_ERROR (Status)) {
    mGenericElogProtocol = NULL;
  }
}

EFI_STATUS
EfiInitializeGenericElog (
  VOID
  )
/*++

  Routine Description:
    The function will set up a notification on the ELOG protocol.  This function is required to be called prior
    to utilizing the ELOG protocol from within this library.

  Arguments:

  Returns:
    EFI_SUCCESS - after the notificication has been setup.

--*/
{
  EFI_EVENT Event;
  EFI_STATUS  Status;

    Status = gBS->CreateEvent (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, GenericElogNotificationFunction, NULL, &Event);
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (&gEfiGenericElogProtocolGuid, Event, &mGenericElogRegistration);
    ASSERT_EFI_ERROR (Status);

    gBS->SignalEvent (Event);


  return EFI_SUCCESS;
}

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
  //
  // If the protocol is not found return EFI_NOT_FOUND
  //
  if (mGenericElogProtocol == NULL) {
    return EFI_NOT_FOUND;

  }

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
    EFI_BUFFER_TOO_SMALL -the ElogData buffer is too small to be filled with the requested data.

--*/
{
  //
  // If the protocol is not found return EFI_NOT_FOUND
  //
  if (mGenericElogProtocol == NULL) {
    return EFI_NOT_FOUND;

  }

  return mGenericElogProtocol->GetEventLogData (
                                mGenericElogProtocol,
                                ElogData,
                                DataType,
                                DataSize,
                                RecordId
                                );
}

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
{
  //
  // If the protocol is not found return EFI_NOT_FOUND
  //
  if (mGenericElogProtocol == NULL) {
    return EFI_NOT_FOUND;

  }

  return mGenericElogProtocol->EraseEventlogData (
                                mGenericElogProtocol,
                                DataType,
                                RecordId
                                );
}

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
{
  //
  // If the protocol is not found return EFI_NOT_FOUND
  //
  if (mGenericElogProtocol == NULL) {
    return EFI_NOT_FOUND;

  }

  return mGenericElogProtocol->ActivateEventLog (
                                mGenericElogProtocol,
                                DataType,
                                EnableElog,
                                ElogStatus
                                );
}

BOOLEAN
IsLeapYear (
  IN UINT16   Year
  )
/*++

Routine Description:

  This function verifies the leap year

Arguments:

  Year    year in YYYY format.

Returns:

  TRUE if the year is a leap year

--*/
{
  if (Year % 4 == 0) {
    if (Year % 100 == 0) {
      if (Year % 400 == 0) {
        return TRUE;
      } else {
        return FALSE;
      }
    } else {
      return TRUE;
    }
  } else {
    return FALSE;
  }
}

UINTN
CountNumOfLeapDays (
  IN EFI_TIME  *Time
  )
/*++

Routine Description:

  This function calculates the total number leap days from 1970 to the current year

Arguments:

  Time    - Current Time

Returns:

  Returns the number of leap days since the base year, 1970.

--*/
{
  UINT16  NumOfYear;
  UINT16  BaseYear;
  UINT16  Index;
  UINTN   Count;

  Count     = 0;
  BaseYear  = 1970;
  NumOfYear = Time->Year - 1970;

  for (Index = 0; Index <= NumOfYear; Index++) {
    if (IsLeapYear (BaseYear + Index)) {
      Count++;
    }
  }

  //
  // If the current year is a leap year but the month is January or February,
  // then the leap day has not occurred and should not be counted. If it is
  // February 29, the leap day is accounted for in CalculateNumOfDayPassedThisYear( )
  //
  if ( IsLeapYear( Time->Year ) ) {
    if ( (Count > 0) && (Time->Month < 3) ) {
      Count--;
    }
  }

  return Count;
}

UINTN
CalculateNumOfDayPassedThisYear (
  IN  EFI_TIME    Time
  )
/*++

Routine Description:

  This function calculates the total number of days passed till the day in a year.
  If the year is a leap year, an extra day is not added since the number of leap
  days is calculated in CountNumOfLeapDays.

Arguments:

  Time    This structure contains detailed information about date and time..

Returns:

  Returns the number of days passed until the input day.

--*/
{
  UINTN Index;
  UINTN NumOfDays;

  NumOfDays = 0;
  for (Index = 1; Index < Time.Month; Index++) {
    NumOfDays += DaysOfMonth[Index - 1];
  }

  NumOfDays += Time.Day;
  return NumOfDays;
}

UINT8
BcdToDecimal (
  IN  UINT8 BcdValue
  )
/*++

Routine Description:
  Function converts a BCD to a decimal value.

Arguments:
  BcdValue         - An 8 bit BCD value

Returns:

  UINT8            - Returns the decimal value of the BcdValue

--*/
{
  UINTN High;
  UINTN Low;

  High  = BcdValue >> 4;
  Low   = BcdValue - (High << 4);

  return (UINT8) (Low + (High * 10));
}

//
// RTC read functions were copied here since we need to get the time
// in both DXE and runtime code.  The PcRtc driver is not currently a
// dual mode driver, this is more efficient since making PcRtc dual mode
// would unnecessarily bloat the SMM code space.
//

UINT8
RtcRead (
  IN  UINT8 Address
  )
/*++

Routine Description:

  Read data register and return contents.

Arguments:

  Address - Register address to read    

Returns:

  Value of data register contents

--*/
{
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, (UINT8) (Address | (UINT8) (IoRead8 (PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  return IoRead8 (PCAT_RTC_DATA_REGISTER);
}

VOID
RtcWrite (
  IN  UINT8   Address,
  IN  UINT8   Data
  )
/*++

Routine Description:

  Write data to register address.

Arguments:

  Address - Register address to write
  Data    - Data to write to register

Returns:

  None

--*/
{
  IoWrite8 (PCAT_RTC_ADDRESS_REGISTER, (UINT8) (Address | (UINT8) (IoRead8 (PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  IoWrite8 (PCAT_RTC_DATA_REGISTER, Data);
}

VOID
ConvertRtcTimeToEfiTime (
  IN EFI_TIME       *Time,
  IN RTC_REGISTER_B RegisterB
  )
/*++

Routine Description:

Arguments:

  Time
  RegisterB

Returns:

  None

--*/
{
  BOOLEAN PM;

  if ((Time->Hour) & 0x80) {
    PM = TRUE;
  } else {
    PM = FALSE;
  }

  Time->Hour = (UINT8) (Time->Hour & 0x7f);

  if (RegisterB.Bits.DM == 0) {
    Time->Year    = BcdToDecimal ((UINT8) Time->Year);
    Time->Month   = BcdToDecimal (Time->Month);
    Time->Day     = BcdToDecimal (Time->Day);
    Time->Hour    = BcdToDecimal (Time->Hour);
    Time->Minute  = BcdToDecimal (Time->Minute);
    Time->Second  = BcdToDecimal (Time->Second);
  }
  //
  // If time is in 12 hour format, convert it to 24 hour format
  //
  if (RegisterB.Bits.MIL == 0) {
    if (PM && Time->Hour < 12) {
      Time->Hour = (UINT8) (Time->Hour + 12);
    }

    if (!PM && Time->Hour == 12) {
      Time->Hour = 0;
    }
  }

  Time->Nanosecond  = 0;
  Time->TimeZone    = EFI_UNSPECIFIED_TIMEZONE;
  Time->Daylight    = 0;
}

EFI_STATUS
RtcTestCenturyRegister (
  VOID
  )
/*++

Routine Description:

  Test Century Register.

Arguments:

  None

Returns:

  EFI_SUCCESS
  EFI_DEVICE_ERROR

--*/
{
  UINT8 Century;
  UINT8 Temp;

  Century = RtcRead (RTC_ADDRESS_CENTURY);

  //
  // Always sync-up the Bit7 "semaphore"...this maintains
  // consistency across the different chips/implementations of
  // the RTC...
  //
  RtcWrite (RTC_ADDRESS_CENTURY, 0x00);
  Temp = (UINT8) (RtcRead (RTC_ADDRESS_CENTURY) & 0x7f);
  RtcWrite (RTC_ADDRESS_CENTURY, Century);
  if (Temp == 0x19 || Temp == 0x20) {
    return EFI_SUCCESS;
  }

  return EFI_DEVICE_ERROR;
}

EFI_STATUS
RtcWaitToUpdate (
  UINTN Timeout
  )
/*++

Routine Description:

  Waits until RTC register A and D show data is valid

Arguments:

  Timeout - Maximum time to wait

Returns:

  EFI_DEVICE_ERROR
  EFI_SUCCESS

--*/
{
  RTC_REGISTER_A  RegisterA;
  RTC_REGISTER_D  RegisterD;

  //
  // See if the RTC is functioning correctly
  //
  RegisterD.Data = RtcRead (RTC_ADDRESS_REGISTER_D);

  if (RegisterD.Bits.VRT == 0) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Wait for up to 0.1 seconds for the RTC to be ready.
  //
  Timeout         = (Timeout / 10) + 1;
  RegisterA.Data  = RtcRead (RTC_ADDRESS_REGISTER_A);
  while (RegisterA.Bits.UIP == 1 && Timeout > 0) {
    gBS->Stall (10);
    RegisterA.Data = RtcRead (RTC_ADDRESS_REGISTER_A);
    Timeout--;
  }

  RegisterD.Data = RtcRead (RTC_ADDRESS_REGISTER_D);
  if (Timeout == 0 || RegisterD.Bits.VRT == 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RtcGetTime (
  OUT EFI_TIME              *Time
  )
/*++

Routine Description:

  Get time from RTC

Arguments:

  Time - pointer to time structure

Returns:

  EFI_INVALID_PARAMETER
  EFI_SUCCESS

--*/
{
  RTC_REGISTER_B  RegisterB;
  UINT8           Century;
  EFI_STATUS      Status;

/*
  //
  // Need to locate the protocol everytime since we may be in DXE or runtime.
  //
  Status = gBS->LocateProtocol(&gEfiCpuIoProtocolGuid, NULL, &mCpuIo);
  if EFI_ERROR(Status) {
    return (Status);
  }
*/
  //
  // Check parameters for null pointer
  //
  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Wait for up to 0.1 seconds for the RTC to be updated
  //
  Status = RtcWaitToUpdate (100000);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Read Register B
  //
  RegisterB.Data = RtcRead (RTC_ADDRESS_REGISTER_B);

  //
  // Get the Time/Date/Daylight Savings values.
  //
  Time->Second  = RtcRead (RTC_ADDRESS_SECONDS);
  Time->Minute  = RtcRead (RTC_ADDRESS_MINUTES);
  Time->Hour    = RtcRead (RTC_ADDRESS_HOURS);
  Time->Day     = RtcRead (RTC_ADDRESS_DAY_OF_THE_MONTH);
  Time->Month   = RtcRead (RTC_ADDRESS_MONTH);
  Time->Year    = RtcRead (RTC_ADDRESS_YEAR);

  ConvertRtcTimeToEfiTime (Time, RegisterB);

  if (RtcTestCenturyRegister () == EFI_SUCCESS) {
    Century = BcdToDecimal ((UINT8) (RtcRead (RTC_ADDRESS_CENTURY) & 0x7f));
  } else {
    Century = BcdToDecimal (RtcRead (RTC_ADDRESS_CENTURY));
  }

  Time->Year = (UINT16) (Century * 100 + Time->Year);

  return EFI_SUCCESS;
}

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
{
  UINT16                NumOfYears;
  UINTN                 NumOfLeapDays;
  UINTN                 NumOfDays;
  EFI_TIME              Time;
  EFI_STATUS  Status;

  Status = RtcGetTime (&Time);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  NumOfYears      = Time.Year - 1970;
  NumOfLeapDays   = CountNumOfLeapDays (&Time);
  NumOfDays       = CalculateNumOfDayPassedThisYear (Time);

  //
  // Add 365 days for all years. Add additional days for Leap Years. Subtract off current day.
  //
  NumOfDays += (NumOfLeapDays + (365 * NumOfYears) - 1);

  *NumOfSeconds = (UINT32) (3600 * 24 * NumOfDays + (Time.Hour * 3600) + (60 * Time.Minute) + Time.Second);

  return EFI_SUCCESS;
}
