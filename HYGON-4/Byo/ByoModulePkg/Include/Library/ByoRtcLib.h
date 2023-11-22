/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  RtcLib.h

Abstract:
  Header file for RtcLib.

Revision History:

**/

#ifndef BYO_RTC_LIB_H_
#define BYO_RTC_LIB_H_

#define PCAT_BYO_RTC_ADDRESS_REGISTER     0x70
#define PCAT_BYO_RTC_DATA_REGISTER        0x71
#define BYO_RTC_ADDRESS_SECONDS           0   // R/W  Range 0..59
#define BYO_RTC_ADDRESS_SECONDS_ALARM     1   // R/W  Range 0..59
#define BYO_RTC_ADDRESS_MINUTES           2   // R/W  Range 0..59
#define BYO_RTC_ADDRESS_MINUTES_ALARM     3   // R/W  Range 0..59
#define BYO_RTC_ADDRESS_HOURS             4   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define BYO_RTC_ADDRESS_HOURS_ALARM       5   // R/W  Range 1..12 or 0..23 Bit 7 is AM/PM
#define BYO_RTC_ADDRESS_DAY_OF_THE_WEEK   6   // R/W  Range 1..7
#define BYO_RTC_ADDRESS_DAY_OF_THE_MONTH  7   // R/W  Range 1..31
#define BYO_RTC_ADDRESS_MONTH             8   // R/W  Range 1..12
#define BYO_RTC_ADDRESS_YEAR              9   // R/W  Range 0..99
#define BYO_RTC_ADDRESS_REGISTER_A        10  // R/W[0..6]  R0[7]
#define BYO_RTC_ADDRESS_REGISTER_B        11  // R/W
#define BYO_RTC_ADDRESS_REGISTER_C        12  // RO
#define BYO_RTC_ADDRESS_REGISTER_D        13  // RO
#define BYO_RTC_ADDRESS_REGISTER_E        14
#define BYO_RTC_ADDRESS_CENTURY           50  // R/W  Range 19..20 Bit 8 is R/W

#pragma pack(1)

typedef struct {
  UINT8 Rs : 4;   // Rate Selection Bits
  UINT8 Dv : 3;   // Divisor
  UINT8 Uip : 1;  // Update in progress
} BYO_RTC_REGISTER_A_BITS;

typedef union {
  BYO_RTC_REGISTER_A_BITS Bits;
  UINT8               Data;
} BYO_RTC_REGISTER_A;

typedef struct {
  UINT8 Dse : 1;  // 0 - Daylight saving disabled  1 - Daylight savings enabled
  UINT8 Mil : 1;  // 0 - 12 hour mode              1 - 24 hour mode
  UINT8 Dm  : 1;  // 0 - BCD Format                1 - Binary Format
  UINT8 Sqwe: 1;  // 0 - Disable SQWE output       1 - Enable SQWE output
  UINT8 Uie : 1;  // 0 - Update INT disabled       1 - Update INT enabled
  UINT8 Aie : 1;  // 0 - Alarm INT disabled        1 - Alarm INT Enabled
  UINT8 Pie : 1;  // 0 - Periodic INT disabled     1 - Periodic INT Enabled
  UINT8 Set : 1;  // 0 - Normal operation.         1 - Updates inhibited
} BYO_RTC_REGISTER_B_BITS;

typedef union {
  BYO_RTC_REGISTER_B_BITS Bits;
  UINT8               Data;
} BYO_RTC_REGISTER_B;

typedef struct {
  UINT8 DateAlarm : 6;
  UINT8 ScratchBit : 1;
  UINT8 Vrt : 1;      // Valid RAM and Time
} BYO_RTC_REGISTER_D_BITS;

typedef union {
  BYO_RTC_REGISTER_D_BITS Bits;
  UINT8               Data;
} BYO_RTC_REGISTER_D;

#pragma pack()

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
  );

/**
  Sets the current local time and date information.

  @param  Time                  A pointer to the current time.
  @param  Global                For global use inside this module.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware error.

**/
EFI_STATUS
EFIAPI
ByoRtcSetTime (
  IN EFI_TIME                *Time
  );

/**
  Sets the default time and date information.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval EFI_INVALID_PARAMETER A time field is out of range.
  @retval EFI_DEVICE_ERROR      The time could not be set due due to hardware error.

**/
EFI_STATUS
EFIAPI
ByoSetDefaultTime (
  );

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
  );

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
  );
#endif
