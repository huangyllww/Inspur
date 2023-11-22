/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPasswordLib.h

Abstract:
  System password library.

Revision History:

**/
#ifndef _SYSTEM_PASSWORD_LIB_H_
#define _SYSTEM_PASSWORD_LIB_H_

#include <SystemPasswordVariable.h>
#include <Protocol/SystemPasswordProtocol.h>

#define DEFAULT_PASSWORD_ITERATION_COUNT              1000
#define MIN_PASSWORD_COMPLEXITY                       3
#define SYSTEM_PASSWORD_RECORDER_NAME                 L"SR"

#define RECORDER_TIME_MAX                             FixedPcdGet8(PcdPasswordMaxRecorderTime)

#pragma pack(1)
typedef struct _BYO_SYSTEM_PASSWORD_RECORDER {
  UINT8    AdmCount;
  UINT8    PopCount;
  UINT8    Admin[RECORDER_TIME_MAX][SYSTEM_PASSWORD_HASH_LENGTH];
  UINT8    PowerOn[RECORDER_TIME_MAX][SYSTEM_PASSWORD_HASH_LENGTH];
} BYO_SYSTEM_PASSWORD_RECORDER;
#pragma pack()

UINTN
Unicode2Ascii (
  IN  CHAR16    *String16,
  OUT  UINT8    **String8
  );

UINTN
EncodePassword (
  IN  UINT8    *Password,
  IN  UINTN    Length,
  OUT  UINT8    **Hash,
  IN  UINTN    Type    //Type :Ascii or Scancode
  );

BOOLEAN 
IsValidPdKey (
  EFI_INPUT_KEY *Key
  ); 

/**
  Check the complexity of the password (the password must contain characters).

  @param  Password  Input string

  @retval TRUE  The password meets the complexity requirement.
  @retval FALSE The password does not meet the complexity requirement.
**/
BOOLEAN 
ByoVerifyPasswordComplexity (
  IN  CHAR16 *Password
  );

/**
  Verify that the password matches the one you set before.

  @param  Type  password Type,admin and power on
  @param  Password  Input password string

  @return 1   Password is invalid,The password has been set before (less than five times).
  @retval 0   Password is valid, The password is not set before (less than five times).
**/
UINT8
ByoCheckPasswordRecord (
  PASSWORD_TYPE    Type,
  CHAR16           *Password
  );

/**
  Verify that passwords are the same. Checks whether the passed "String" matches the password for that "Type".

  @param  Type  password Type,admin and power on
  @param  String  Input password string

  @return TRUE   Same password.
  @retval FALSE  Different passwords.
**/
BOOLEAN
ByoCheckRepeatPsd(
  IN  PASSWORD_TYPE    Type,
  IN  CHAR16           *String
  );
#endif

