/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  HddPasswordDxe.c

Abstract: 
  Hdd password DXE driver.

Revision History:

**/



#ifndef _HDD_PASSWORD_NVDATASTRUC_H_
#define _HDD_PASSWORD_NVDATASTRUC_H_

#include <Guid/HiiPlatformSetupFormset.h>

#define HDD_PASSWORD_CONFIG_GUID \
  { \
    0xd5fd1546, 0x22c5, 0x4c2e, { 0x96, 0x9f, 0x27, 0x3c, 0x0, 0x77, 0x10, 0x80 } \
  }

#define HDD_PASSWORD_VAR_RAW_NAME       HdpCfg
#define HDD_PASSWORD_VAR_NAME           TKN2STR16(HDD_PASSWORD_VAR_RAW_NAME)


#define FORMID_HDD_MAIN_FORM          1  
#define FORMID_HDD_DEVICE_FORM        2
#define FORMIN_HDD_LOCK_FORM          3
#define FORMID_NVME_DEVICE_FORM       4

#define HDD_DEVICE_ENTRY_LABEL        0x1234
#define HDD_DEVICE_LABEL_END          0xffff

#define KEY_HDD_DEVICE_ENTRY_BASE      0x1000

#define KEY_HDD_ENABLE_USER_PASSWORD   0x100
#define KEY_HDD_DISABLE_USER_PASSWORD  0x101
#define KEY_HDD_CHANGE_USER_PASSWORD   0x102
#define KEY_HDD_SET_MASTER_PASSWORD    0x103
#define KEY_HDD_CHANGE_MASTER_PASSWORD 0x104

#define KEY_NVME_ENABLE_ADMIN_PASSWORD  0x180
#define KEY_NVME_CHANGE_ADMIN_PASSWORD  0x181
#define KEY_NVME_CHANGE_USER_PASSWORD    0x182
#define KEY_NVME_DISABLE_ADMIN_PASSWORD 0x183

#define KEY_HDD_NAME                   0x200
#define KEY_HDD_NAME2                  0x201
#define KEY_HDD_NAME3                  0x202




#pragma pack(1)

typedef struct {
  UINT8      Supported;
  UINT8      Enabled;
  UINT8      Locked;
  UINT8      Frozen;
  UINT8      CountExpired;
  UINT8      RetryCount;
  UINT8      UserPasswordStatus;
  UINT8      MasterPasswordStatus;
  UINT8      ChangeMasterSupport;
  UINT8      Reserved;
  UINT16     UserPw[1];
  UINT16     MasterPw[1];
  UINT16     HddName[64];
} HDD_PASSWORD_CONFIG;

#pragma pack()

#endif
