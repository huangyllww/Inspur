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
#include <Protocol/SystemPasswordProtocol.h> //XCL -

#define UNIQUE_SYSTEM_NUMBER_LENGTH    8
#define PASSWORD_ADMIN         0x3
#define PASSWORD_POWER_ON      0x4
#define PASSWORD_ALL           0x7
#define PASSWORD_LOCK          0xF

#define LAST_LOCKED_TIME_GUID \
  { \
     0x8473ddf4, 0x3478, 0x2345, { 0x33, 0x13, 0x54, 0x56, 0x32, 0x52, 0x42, 0x32 }\
  }

typedef struct {
  UINT8   Check;
  UINT8   Type;
  UINT8   LockedNum;
  UINT8   Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
} EFI_PSW_RTC;

#define LAST_PSW_LOCK_TIME   L"LastLockTime"

EFI_STATUS
EFIAPI
Sha256Hash (
  IN  CONST VOID        *Data,
  IN        UINTN       DataLen,
  OUT       UINT8       *Digest,
  IN        UINTN       DigestSize
  );

UINTN
Unicode2Ascii (
  IN  CHAR16    *String16,
  OUT  UINT8    **String8
  );


UINTN
GetUniqueNumber (
  UINT8    **Number
  );


UINTN
EncodePassword (
  IN  UINT8    *Password,
  IN  UINTN    Length,
  OUT  UINT8    **Hash,
  IN  UINTN    Type    //Type :Ascii or Scancode
  );

BOOLEAN
CompareInputPassword (
  IN  UINT8    *PasswordHash,
  IN  CHAR16    *InputPassword
  );

/**
  Check Power-On Passwrod.

**/
EFI_STATUS
CheckPopPassword (VOID );

/**
  Check Setup Passwrod.

**/
EFI_STATUS
CheckSetupPassword (VOID );

/**
  Check System Passwrod.

**/
EFI_STATUS
CheckSysPd (
  BOOLEAN   BootSetup
  );

BOOLEAN 
IsValidPdKey (
  EFI_INPUT_KEY *Key
  ); 

#endif

