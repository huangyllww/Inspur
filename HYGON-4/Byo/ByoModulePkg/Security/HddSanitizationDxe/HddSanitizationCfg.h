/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HddSanitizationDxe.c

Abstract:
  Hdd HddSanitization DXE driver.

Revision History:

**/



#ifndef _HDD_SANITIZE_NVDATASTRUC_H_
#define _HDD_SANITIZE_NVDATASTRUC_H_

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/SetupGuiCustom.h>

#define HDD_SANITIZE_METHOD_CONFIG_GUID \
  { \
    0xf8605bb2, 0x17ec, 0x4e3a, { 0xb3, 0x8c, 0x3c, 0xbb, 0xb2, 0x76, 0xc3, 0xae } \
  }


#define HDD_SANITIZE_METHOD_CONFIG_VAR_NAME      L"HddSanization"

#define FORMID_HDD_MAIN_FORM                     1
#define FORMID_HDD_PASSWORD_ENABLED_FORM         2
#define FORMID_ATA_DEVICE_FORM                   3
#define FORMID_NVME_DEVICE_FORM                  4

#define HDD_DEVICE_ENTRY_LABEL                   0x1234
#define HDD_DEVICE_LABEL_END                     0xffff
          
#define KEY_HDD_DEVICE_ENTRY_BASE                0x1000

#define KEY_HDD_LOCK_NAME                        0x200
#define KEY_HDD_ATA_NAME                         0x201
#define KEY_HDD_NVME_NAME                        0x202

#define NVME_SANITIZATION_METHOD_TYPE            0x300
#define NVME_SANITIZATION_CMD_TYPE               0x301
#define NVME_SANITIZATION_ERASE_SETTING          0x302
#define NVME_SANITIZATION_SANITIZE_METHOD        0x303
#define NVME_SANITIZE_START                      0x304
#define NVME_FORMAT_NVM_START                    0x305

#define SATA_SANITIZATION_METHOD_TYPE            0x320
#define SATA_SANITIZATION_CMD_TYPE               0x311
#define SATA_SANITIZATION_ERASE_SETTING          0x322
#define SATA_SANITIZATION_SANITIZE_METHOD        0x323
#define SATA_SANITIZE_START                      0x325

#pragma pack(1)

typedef struct {
  UINT8      Supported;
  UINT8      Enabled;
  UINT8      Locked;
  UINT8      Frozen;
  UINT16     HddName[64];
  UINT8      SanitizeCap;
} HDD_SECURITY_INFO;


typedef struct {
  UINT8              MethodType;
  UINT8              CmdType;
  UINT8              EraseSet;
  UINT8              SanitizeMethod;
} HDD_SANITIZE_METHOD_CONFIG;

#pragma pack()

#endif
