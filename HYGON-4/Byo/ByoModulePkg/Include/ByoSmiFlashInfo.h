/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_SMIFLASH_INFO_H__
#define __BYO_SMIFLASH_INFO_H__


#define SW_SMI_FLASH_SERVICES	                0xec

#define SUBFUNCTION_GET_FLASH_INFO            0x01
#define SUBFUNCTION_GET_FD_AREA               0x02
#define SUBFUNCTION_READ_FLASH_BLOCK          0x03
#define SUBFUNCTION_COMPARE_BIOS_ID           0x04
#define SUBFUNCTION_UPDATE_SMBIOS_DATA        0x05
#define SUBFUNCTION_BIOS_VERIFY               0X06
#define SUBFUNCTION_CLEAR_EVENT_LOG           0X07
#define SUBFUNCTION_CHECK_MFG                 0x08
#define SUBFUNCTION_PROGRAM_FLASH             0x09
#define SUBFUNCTION_MODE_CHECK_BIOS_VER       0x0A
#define SUBFUNCTION_TOPSWAP_ON                0x0B
#define SUBFUNCTION_TOPSWAP_OFF               0x0C
#define SUBFUNCTION_OA3_UPDATE                0x0D
#define SUBFUNCTION_CLEAR                     0x0E
#define SUBFUNCTION_WRITECMOS                 0x0F
#define SUBFUNCTION_DISABLE_USB_POWERBUTTON   0x10
#define SUBFUNCTION_ENABLE_USB_POWERBUTTON    0x11
#define SUBFUNCTION_CHECK_BIOS_LOCK           0x12
#define SUBFUNCTION_PATCH_FOR_USB             0x13
#define SUBFUNCTION_CHECK_BIOS_ID             0x14
#define SUBFUNCTION_QUERY_PASSWORD            0x15
#define SUBFUNCTION_VERIFY_PASSWORD           0x16
#define SUBFUNCTION_QUERY_PASSWORD_SET        0x17
#define SUBFUNCTION_CHECK_BIOS_INFO           0x18

#define SUBFUNCTION_SET_ADMIN_PASSWORD        0x19
#define SUBFUNCTION_SET_POWERON_PASSWORD      0x20
#define ADMIN_PASSWORD 0
#define POWERON_PASSWORD 1

#define SUBFUNCTION_SET_VARIABLE_INFO         0x21
#define SUBFUNCTION_CHECK_AC                  0x22

#define SUBFUNCTION_WRITE_FLASH               0x80
#define SUBFUNCTION_ERASE_FLASH               0x81
#define SUBFUNCTION_IF_UNLOCK                 0x82
#define SUBFUNCTION_IF_LOCK                   0x83
#define SUBFUNCTION_IF_REMOVE_LOGO            0x84
#define SUBFUNCTION_IF_RST_PASSWD             0x85
#define SUBFUNCTION_IF_SIGN_UPDATE            0x87
#define SUBFUNCTION_IF_SIGN_VERIFY            0x88
#define SUBFUNCTION_IF_SIGN_PREPARE           0x89
#define SUBFUNCTION_IF_UPDATE_UCODE           0x8A       // wz191108 +

#define SUBFUNCTION_SET_BIOS_VALUE            0x31
#define SUBFUNCTION_GET_BIOS_VALUE            0x32
#define SUBFUNCTION_SET_BIOS_VALUE_GROUP      0x33
#define SUBFUNCTION_GET_BIOS_VALUE_GROUP      0x34
#define GET_FD_AREA_SUBFUNCTION               0x42
#define SUBFUNCTION_GET_SETUP_ITEMS_NAME      0x43
#define SUBFUNCTION_GET_SETUP_OPTIONS         0x44
#define SUBFUNCTION_PLATFORM_SUPPORTED        0x45
#define SUBFUNCTION_GET_SETUP_ITEMS_STRING    0x46
#define SUBFUNCTION_GET_VARIABLE_DATA         0x47
#define SUBFUNCTION_SET_VARIABLE_DATA         0x48
#define SUBFUNCTION_GET_SETUP_ONEOF_OPTIONS   0x49
#define SUBFUNCTION_CHECK_VARIABLE_VALID      0x50

#define SUBFUNCTION_LENOVO_OPERTAION          0x10000

#pragma pack(1)

#define SMI_FLASH_E0000_TAG   SIGNATURE_32('$',  'F',  'U',  '$')

typedef struct {
  UINT32   Tag;
  UINT32   Version;
  UINT32   SubFunction;
  UINT32   Offset;
  UINT32   Size;
  UINT64   StatusCode;
  UINT8    Buffer[SIZE_4KB];
} SMI_INFO;

#pragma pack()

#endif        // __BYO_SMIFLASH_INFO_H__



