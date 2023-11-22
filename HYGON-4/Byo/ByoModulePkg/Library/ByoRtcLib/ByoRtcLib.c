/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoRtcLib.c

Abstract:
  Source file for the ByoRtcLib.

Revision History:

**/
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ByoRtcLib.h>

//
// Days of month.
//
UINTN mDayOfMonth[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

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
  IoWrite8 (PCAT_BYO_RTC_ADDRESS_REGISTER, (UINT8) (Address | (UINT8) (IoRead8 (PCAT_BYO_RTC_ADDRESS_REGISTER) & 0x80)));
  return IoRead8 (PCAT_BYO_RTC_DATA_REGISTER);
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
  IoWrite8 (PCAT_BYO_RTC_ADDRESS_REGISTER, (UINT8) (Address | (UINT8) (IoRead8 (PCAT_BYO_RTC_ADDRESS_REGISTER) & 0x80)));
  IoWrite8 (PCAT_BYO_RTC_DATA_REGISTER, Data);
}

/**
  Checks an 8-bit BCD value, and converts to an 8-bit value if valid.

  This function checks the 8-bit BCD value specified by Value.
  If valid, the function converts it to an 8-bit value and returns it.
  Otherwise, return 0xff.

  @param   Value The 8-bit BCD value to check and convert

  @return  The 8-bit value converted. Or 0xff if Value is invalid.

**/
UINT8
EFIAPI
ByoCheckAndConvertBcd8ToDecimal8 (
  IN  UINT8  Value
  )
{
  if ((Value < 0xa0) && ((Value & 0xf) < 0xa)) {
    return BcdToDecimal8 (Value);
  }

  return 0xff;
}
/**
  Converts time read from RTC to EFI_TIME format defined by UEFI spec.

  This function converts raw time data read from RTC to the EFI_TIME format
  defined by UEFI spec.
  If data mode of RTC is BCD, then converts it to decimal,
  If RTC is in 12-hour format, then converts it to 24-hour format.

  @param   Time       On input, the time data read from RTC to convert
                      On output, the time converted to UEFI format
  @param   RegisterB  Value of Register B of RTC, indicating data mode
                      and hour format.

  @retval  EFI_INVALID_PARAMETER  Parameters passed in are invalid.
  @retval  EFI_SUCCESS            Convert RTC time to EFI time successfully.

**/
EFI_STATUS
ConvertRtcTimeToEfiTime (
  IN OUT EFI_TIME        *Time,
  IN     BYO_RTC_REGISTER_B  RegisterB
  )
{
  BOOLEAN IsPM;

  if ((Time->Hour & 0x80) != 0) {
    IsPM = TRUE;
  } else {
    IsPM = FALSE;
  }

  Time->Hour = (UINT8) (Time->Hour & 0x7f);

  if (RegisterB.Bits.Dm == 0) {
    Time->Year    = ByoCheckAndConvertBcd8ToDecimal8 ((UINT8) Time->Year);
    Time->Month   = ByoCheckAndConvertBcd8ToDecimal8 (Time->Month);
    Time->Day     = ByoCheckAndConvertBcd8ToDecimal8 (Time->Day);
    Time->Hour    = ByoCheckAndConvertBcd8ToDecimal8 (Time->Hour);
    Time->Minute  = ByoCheckAndConvertBcd8ToDecimal8 (Time->Minute);
    Time->Second  = ByoCheckAndConvertBcd8ToDecimal8 (Time->Second);
  }

  if (Time->Year == 0xff || Time->Month == 0xff || Time->Day == 0xff ||
      Time->Hour == 0xff || Time->Minute == 0xff || Time->Second == 0xff) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // If time is in 12 hour format, convert it to 24 hour format
  //
  if (RegisterB.Bits.Mil == 0) {
    if (IsPM && Time->Hour < 12) {
      Time->Hour = (UINT8) (Time->Hour + 12);
    }

    if (!IsPM && Time->Hour == 12) {
      Time->Hour = 0;
    }
  }

  Time->Nanosecond  = 0;

  return EFI_SUCCESS;
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

  Century = RtcRead (BYO_RTC_ADDRESS_CENTURY);

  //
  // Always sync-up the Bit7 "semaphore"...this maintains
  // consistency across the different chips/implementations of
  // the RTC...
  //
  RtcWrite (BYO_RTC_ADDRESS_CENTURY, 0x00);
  Temp = (UINT8) (RtcRead (BYO_RTC_ADDRESS_CENTURY) & 0x7f);
  RtcWrite (BYO_RTC_ADDRESS_CENTURY, Century);
  if (Temp == 0x19 || Temp == 0x20) {
    return EFI_SUCCESS;
  }

  return EFI_DEVICE_ERROR;
}

/**
  Check if it is a leap year.

  @param    Time   The time to be checked.

  @retval   TRUE   It is a leap year.
  @retval   FALSE  It is NOT a leap year.
**/
STATIC
BOOLEAN
IsLeapYear (
  IN EFI_TIME   *Time
  )
{
  if (Time->Year % 4 == 0) {
    if (Time->Year % 100 == 0) {
      if (Time->Year % 400 == 0) {
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
/**
  See if field Day of an EFI_TIME is correct.

  @param    Time   Its Day field is to be checked.

  @retval   TRUE   Day field of Time is correct.
  @retval   FALSE  Day field of Time is NOT correct.
**/
BOOLEAN
DayValid (
  IN  EFI_TIME  *Time
  )
{
  //
  // The validity of Time->Month field should be checked before
  //
  ASSERT (Time->Month >=1);
  ASSERT (Time->Month <=12);
  if (Time->Day < 1 ||
      Time->Day > mDayOfMonth[Time->Month - 1] ||
      (Time->Month == 2 && (!IsLeapYear (Time) && Time->Day > 28))
      ) {
    return FALSE;
  }

  return TRUE;
}
/**
  See if all fields of a variable of EFI_TIME type is correct.

  @param   Time   The time to be checked.

  @retval  EFI_INVALID_PARAMETER  Some fields of Time are not correct.
  @retval  EFI_SUCCESS            Time is a valid EFI_TIME variable.

**/
EFI_STATUS
RtcTimeFieldsValid (
  IN EFI_TIME *Time
  )
{

  if (Time->Year < PcdGet16 (PcdMinimalValidYear) ||
      Time->Year > PcdGet16 (PcdMaximalValidYear) ||
      Time->Month < 1 ||
      Time->Month > 12 ||
      (!DayValid (Time)) ||
      Time->Hour > 23 ||
      Time->Minute > 59 ||
      Time->Second > 59) {
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

/**
  Wait for a period for the RTC to be ready.

  @param    Timeout  Tell how long it should take to wait.

  @retval   EFI_DEVICE_ERROR   RTC device error.
  @retval   EFI_SUCCESS        RTC is updated and ready.
**/
EFI_STATUS
EFIAPI
ByoRtcWaitToUpdate (
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
  BYO_RTC_REGISTER_A  RegisterA;

  //
  // Wait for up to 0.1 seconds for the RTC to be ready.
  //
  Timeout         = (Timeout / 10) + 1;
  RegisterA.Data  = RtcRead (BYO_RTC_ADDRESS_REGISTER_A);
  while (RegisterA.Bits.Uip == 1 && Timeout > 0) {
    MicroSecondDelay (10);
    RegisterA.Data = RtcRead (BYO_RTC_ADDRESS_REGISTER_A);
    Timeout--;
  }

  if (Timeout == 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Returns the current time and date information, and the time-keeping capabilities
  of the hardware platform.

  @param  Time          A pointer to storage to receive a snapshot of the current time.
  @param  Capabilities  An optional pointer to a buffer to receive the real time clock
                        device's capabilities.
  @param  Global        For global use inside this module.

  @retval EFI_SUCCESS            The operation completed successfully.
  @retval EFI_INVALID_PARAMETER  Time is NULL.
  @retval EFI_DEVICE_ERROR       The time could not be retrieved due to hardware error.

**/
EFI_STATUS
EFIAPI
ByoRtcGetTime (
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
  BYO_RTC_REGISTER_B  RegisterB;
  UINT8           Century;
  EFI_STATUS      Status;

  //
  // Check parameters for null pointer
  //
  if (Time == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Wait for up to 0.1 seconds for the RTC to be updated
  //
  Status = ByoRtcWaitToUpdate (PcdGet32 (PcdRealTimeClockUpdateTimeout));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Read Register B
  //
  RegisterB.Data = RtcRead (BYO_RTC_ADDRESS_REGISTER_B);

  //
  // Get the Time/Date/Daylight Savings values.
  //
  Time->Second  = RtcRead (BYO_RTC_ADDRESS_SECONDS);
  Time->Minute  = RtcRead (BYO_RTC_ADDRESS_MINUTES);
  Time->Hour    = RtcRead (BYO_RTC_ADDRESS_HOURS);
  Time->Day     = RtcRead (BYO_RTC_ADDRESS_DAY_OF_THE_MONTH);
  Time->Month   = RtcRead (BYO_RTC_ADDRESS_MONTH);
  Time->Year    = RtcRead (BYO_RTC_ADDRESS_YEAR);

  Status = ConvertRtcTimeToEfiTime (Time, RegisterB);
  
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (RtcTestCenturyRegister () == EFI_SUCCESS) {
    Century = ByoCheckAndConvertBcd8ToDecimal8 ((UINT8) (RtcRead (BYO_RTC_ADDRESS_CENTURY) & 0x7f));
  } else {
    Century = ByoCheckAndConvertBcd8ToDecimal8 (RtcRead (BYO_RTC_ADDRESS_CENTURY));
  }
  if (Century == 0xff) {
    return EFI_INVALID_PARAMETER;
  }

  Time->Year = (UINT16) (Century * 100 + Time->Year);

  Status = RtcTimeFieldsValid (Time);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Converts time from EFI_TIME format defined by UEFI spec to RTC format.

  This function converts time from EFI_TIME format defined by UEFI spec to RTC format.
  If data mode of RTC is BCD, then converts EFI_TIME to it.
  If RTC is in 12-hour format, then converts EFI_TIME to it.

  @param   Time       On input, the time data read from UEFI to convert
                      On output, the time converted to RTC format
  @param   RegisterB  Value of Register B of RTC, indicating data mode
**/
VOID
ConvertEfiTimeToRtcTime (
  IN OUT EFI_TIME        *Time,
  IN     BYO_RTC_REGISTER_B  RegisterB
  )
{
  BOOLEAN IsPM;

  IsPM = TRUE;
  //
  // Adjust hour field if RTC is in 12 hour mode
  //
  if (RegisterB.Bits.Mil == 0) {
    if (Time->Hour < 12) {
      IsPM = FALSE;
    }

    if (Time->Hour >= 13) {
      Time->Hour = (UINT8) (Time->Hour - 12);
    } else if (Time->Hour == 0) {
      Time->Hour = 12;
    }
  }
  //
  // Set the Time/Date values.
  //
  Time->Year  = (UINT16) (Time->Year % 100);

  if (RegisterB.Bits.Dm == 0) {
    Time->Year    = DecimalToBcd8 ((UINT8) Time->Year);
    Time->Month   = DecimalToBcd8 (Time->Month);
    Time->Day     = DecimalToBcd8 (Time->Day);
    Time->Hour    = DecimalToBcd8 (Time->Hour);
    Time->Minute  = DecimalToBcd8 (Time->Minute);
    Time->Second  = DecimalToBcd8 (Time->Second);
  }
  //
  // If we are in 12 hour mode and PM is set, then set bit 7 of the Hour field.
  //
  if (RegisterB.Bits.Mil == 0 && IsPM) {
    Time->Hour = (UINT8) (Time->Hour | 0x80);
  }
}


/**
  Sets the current local time and date information.

  @param  TimeToSet             A pointer to the current time.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware error.

**/
EFI_STATUS
EFIAPI
ByoRtcSetTime (
  IN EFI_TIME                *TimeToSet
  )
{
  EFI_STATUS      Status;
  EFI_TIME        RtcTime;
  EFI_TIME        *Time;
  BYO_RTC_REGISTER_B  RegisterB;

  if (TimeToSet == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  //
  // Don't modify the input TimeToSet
  // Copy TimeToSet to the local variable RtcTime
  //
  Time = &RtcTime;
  CopyMem (Time, TimeToSet, sizeof (EFI_TIME));

  //
  // Make sure that the time fields are valid
  //
  Status = RtcTimeFieldsValid (Time);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "RtcTimeFieldsValid Status: %r\n", Status));
    return Status;
  }

  //
  // Wait for up to 0.1 seconds for the RTC to be updated
  //
  Status = ByoRtcWaitToUpdate (PcdGet32 (PcdRealTimeClockUpdateTimeout));
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "RtcWaitToUpdate Status: %r\n", Status));
    return Status;
  }

  //
  // Read Register B, and inhibit updates of the RTC
  //
  RegisterB.Data      = RtcRead (BYO_RTC_ADDRESS_REGISTER_B);
  RegisterB.Bits.Set  = 1;
  RtcWrite (BYO_RTC_ADDRESS_REGISTER_B, RegisterB.Data);
  //
  // Set RTC configuration after get original time
  // The value of bit AIE should be reserved.
  //
  RegisterB.Data = FixedPcdGet8 (PcdInitialValueRtcRegisterB) | (RegisterB.Data & BIT5);
  RtcWrite (BYO_RTC_ADDRESS_REGISTER_B, RegisterB.Data);

  //
  // Store the century value to RTC before converting to BCD format.
  //
  RtcWrite (BYO_RTC_ADDRESS_CENTURY, DecimalToBcd8 ((UINT8) (Time->Year / 100)));

  ConvertEfiTimeToRtcTime (Time, RegisterB);

  RtcWrite (BYO_RTC_ADDRESS_SECONDS, Time->Second);
  RtcWrite (BYO_RTC_ADDRESS_MINUTES, Time->Minute);
  RtcWrite (BYO_RTC_ADDRESS_HOURS, Time->Hour);
  RtcWrite (BYO_RTC_ADDRESS_DAY_OF_THE_MONTH, Time->Day);
  RtcWrite (BYO_RTC_ADDRESS_MONTH, Time->Month);
  RtcWrite (BYO_RTC_ADDRESS_YEAR, (UINT8) Time->Year);

  //
  // Allow updates of the RTC registers
  //
  RegisterB.Bits.Set = 0;
  RtcWrite (BYO_RTC_ADDRESS_REGISTER_B, RegisterB.Data);

  return EFI_SUCCESS;
}

/**
  Sets the default time and date information.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware error.

**/
EFI_STATUS
EFIAPI
ByoSetDefaultTime (
  )
{
  EFI_TIME       DefaultTime;
  EFI_STATUS     Status;

  ZeroMem (&DefaultTime, sizeof (DefaultTime));
  DefaultTime.Day      = PcdGet8 (PcdMinimalValidDay);
  DefaultTime.Month    = PcdGet8 (PcdMinimalValidMonth);
  DefaultTime.Year     = PcdGet16 (PcdMinimalValidYear);
  DefaultTime.TimeZone = EFI_UNSPECIFIED_TIMEZONE;

  Status = ByoRtcSetTime((EFI_TIME *)&DefaultTime);

  return Status;
}
