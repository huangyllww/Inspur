/** @file
  This File define BYO_PLATFOMR_SETUP structure.

  Copyright (c) 2023, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_PLATFORM_SETUP_VARIABLE_H__
#define __BYO_PLATFORM_SETUP_VARIABLE_H__
#pragma pack(1)
typedef struct {
  UINT16  PxeRetryEn;
  UINT16 PlatId;
  UINT8  MaxVerifyTimes;
  UINT8  PasswordRepeatTimes;
  UINT16 UnlockTime;
  UINT16 ScreenLockTime;
  UINT8  PasswordReminderDays;
  UINT8  FlashRollBackProtect;
  UINT8  RTVariablesProtect;
  UINT8  ShellEn;
  UINT8  NumLockEn;
  UINT8  PxeOnlyEn;
  UINT8  PasswordComplexity;
  UINT8  DisplayMode;
  UINT8  Csm;
  UINT8  BootModeType;
  UINT8  OptionRomExecution;
  UINT8  VideoRomPolicy;
  UINT8  PxeRomPolicy;
  UINT8  StorageRomPolicy;
  UINT8  OtherRomPolicy;
  UINT8  QuietBoot;
  UINT8  Reserved[32];
} BYO_PLATFOMR_SETUP;
#pragma pack()

#define BYO_PLATFOMR_SETUP_VARSTORE    \
      efivarstore BYO_PLATFOMR_SETUP, attribute = 0x7, name  = ByoSetup, guid  = gPlatformSetupVariableGuid;
#endif /* __BYO_PLATFORM_SETUP_VARIABLE_H__ */