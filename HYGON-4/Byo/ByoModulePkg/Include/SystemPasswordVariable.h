/** @file

Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPassword.h

Abstract:
  Implementation of basic setup password function.

Revision History:

**/

#ifndef __SYSTEM_PASSWORD_VARIABLE___H__
#define __SYSTEM_PASSWORD_VARIABLE___H__


#define UNIQUE_SYSTEM_NUMBER_LENGTH    8
#define SYSTEM_PASSWORD_LENGTH         FixedPcdGet8 (PcdSystemPasswordLength)
#define SYSTEM_PASSWORD_MIN_LENGTH     FixedPcdGet8 (PcdSystemPasswordMinLength)
#define SYSTEM_PASSWORD_HASH_LENGTH    FixedPcdGet8 (PcdSystemPasswordHashLength)
#define SYSTEM_VERIFY_TIMES            PcdGet8(PcdMaxVerifyTimes)

#define SYSTEM_PASSWORD_GUID \
  { \
     0x7d925a5d, 0x90c2, 0x4c85, { 0x90, 0x6d, 0x1, 0x3f, 0x56, 0x5e, 0x33, 0xee }\
  }


#define LOCK_STATUS_VARIABLE_GUID \
  { \
     0x1689a7c4, 0x353c, 0x4be2, {0xcc, 0xaf, 0x80, 0x6b, 0xed, 0x27, 0xb0, 0x4d}\
  }


#define PASSWORD_HASH_SHA256 0
#define PASSWORD_HASH_SM3    1
#define PASSWORD_HASH_SHA512 2
#define PASSWORD_HASH_SHA384 3

#pragma pack(1)

typedef struct {
  UINT16  Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;
} PASSWOR_TIME;

typedef struct {
  UINT32  LockStatus;
} FLASH_LOCK_STATUS_DATA;

typedef struct {
  UINT16    Admin[SYSTEM_PASSWORD_LENGTH];
  UINT16    PowerOn[SYSTEM_PASSWORD_LENGTH];
} SYSTEM_PASSWORD_DATA;

typedef struct _SYSTEM_PASSWORD{
  UINT16    Admin[20];
  UINT8     AdminHash[SYSTEM_PASSWORD_HASH_LENGTH];
  UINT8     bHaveAdmin;
  UINT16    PowerOn[20];
  UINT8     PowerOnHash[SYSTEM_PASSWORD_HASH_LENGTH];
  UINT8     bHavePowerOn;
  UINT8     EnteredType;
  UINT8     VerifyTimes;
  UINT8     VerifyTimesAdmin;
  UINT8     VerifyTimesPop;
  UINT8     RequirePopOnRestart;
  UINT8     ChangePopByUser;
  UINT8     PasswordValidDays;
  PASSWOR_TIME AdmPwdTime;
  PASSWOR_TIME PopPwdTime;
  UINT8     HashType;
} SYSTEM_PASSWORD;

typedef struct {
  UINT8  PasswordLengthMin;
  UINT8  PasswordLengthMax;
  UINT8  RecoderTime;
  UINT8  RecoderTimeMax;
  UINT8  PcdSystemPasswordHashIteration;
  BOOLEAN  AdminAndUserNotSame;
  UINT8  Reserved[26];
} SYSTEM_PASSWORD_REQUIREMENTS;

#pragma pack()

#define SYSTEM_PASSWORD_NAME   L"SysPd"
#define USER_DEFEAT_SYSTEM_PASSWORD_NAME   L"UserDefeatSysPd"

#define SYSTEM_PASSWORD_VARSTORE    \
     efivarstore SYSTEM_PASSWORD, attribute = 0x7, name = SysPd, guid = SYSTEM_PASSWORD_GUID;

#define SYSTEM_PASSWORD_DATA_VARSTORE    \
     varstore SYSTEM_PASSWORD_DATA, name = SysPdData, guid = SYSTEM_PASSWORD_GUID;

#define SYS_PD                       SYSTEM_PASSWORD
#define LOGIN_USER_ADMIN      0
#define LOGIN_USER_POP        1
#define LOGIN_USER_MAX        2

#define LOGIN_TYPE                grayoutif ideqval SYSTEM_PASSWORD.EnteredType
#define LOGIN_TYPE_ADMIN     grayoutif NOT ideqval SYSTEM_PASSWORD.EnteredType == LOGIN_USER_ADMIN
#define END_LOGIN_TYPE        endif


#define FLASH_LOCK_STATUS_NAME L"ByoFlashLockStatusString"

#define FLASH_LOCK_STATUS_DATA_VARSTORE    \
     efivarstore FLASH_LOCK_STATUS_DATA, attribute = 0x7, name = ByoFlashLockStatusString, guid = LOCK_STATUS_VARIABLE_GUID;

#define PASSWORD_REQUIREMENTS L"ByoPasswordRequirements"

#endif
