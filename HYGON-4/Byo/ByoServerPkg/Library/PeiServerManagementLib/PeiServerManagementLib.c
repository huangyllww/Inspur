/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PeiServerManagementLib.c

Abstract:


Revision History:

**/

#include <Uefi.h>
#include <Library/ByoRtcLib.h>
#include <Library/ByoCommLib.h>
#include <Library/BaseMemoryLib.h>
//
// Module Globals
//
INTN DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

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
  EFI_TIME              EfiTime;
  EFI_STATUS            Status;

  Status = ByoRtcGetTime (&EfiTime);
  if (EFI_ERROR (Status)) {
    ZeroMem (&EfiTime, sizeof (EFI_TIME));
  }

  NumOfYears      = EfiTime.Year - 1970;
  NumOfLeapDays   = CountNumOfLeapDays (&EfiTime);
  NumOfDays       = CalculateNumOfDayPassedThisYear (EfiTime);

  //
  // Add 365 days for all years. Add additional days for Leap Years. Subtract off current day.
  //
  NumOfDays += (NumOfLeapDays + (365 * NumOfYears) - 1);

  *NumOfSeconds = (UINT32) (3600 * 24 * NumOfDays + (EfiTime.Hour * 3600) + (60 * EfiTime.Minute) + EfiTime.Second);

  return Status;
}

