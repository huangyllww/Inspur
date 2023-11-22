/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_BIOS_INFO_H__
#define __BYO_BIOS_INFO_H__

#include <Uefi.h>

#define BIOS_BLOCK_TYPE_FV_SEC            1
#define BIOS_BLOCK_TYPE_FV_BB             2
#define BIOS_BLOCK_TYPE_FV_MAIN           3
#define BIOS_BLOCK_TYPE_FV_MICROCODE      4
#define BIOS_BLOCK_TYPE_FV_NVRAM          5
#define BIOS_BLOCK_TYPE_FV_LOGO           6
#define BIOS_BLOCK_TYPE_FV_BB_BU          7
#define BIOS_BLOCK_TYPE_FV_SIGN           9
#define BIOS_BLOCK_TYPE_ROM_HOLE          0x80
#define BIOS_BLOCK_TYPE_ROM_HOLE_SMBIOS   0x81
#define BIOS_BLOCK_TYPE_FV_BACKUP_MAIN    7
#define BIOS_BLOCK_TYPE_PBP               0xF0
#define BIOS_BLOCK_TYPE_PBP1              0xF1

#define BYO_FT_PLATFORM_TYPE              0x1001
#define BYO_HW_PLATFORM_TYPE              0x1002


#define BYO_BIOS_INFO_SIGNATURE         SIGNATURE_32('$', 'B', 'I', '$')
#define BYO_BIOS_INFO_SIGNATURE2        SIGNATURE_32('_', 'B', 'Y', 'O')
#define BYO_BIOS_INFO_VERSION           1

#define SMBIOS_REC_SIGNATURE             0x55AA
#define BYO_FLASH_INFO_SIGNATURE         SIGNATURE_32('$', 'F', 'I', '$')
#define BYO_FLASH_INFO_VERSION           1
#define BYO_FLASH_INFO_NAME              L"FlashInfo"
#define BYO_PHYTIUM_PLATFORM             0
#define BYO_LOONGSON_PLATFORM            1

#pragma pack(1)
typedef struct {
  UINT32  Type;
  UINT64  Base;
  UINT64  Size;
  UINT32  Sign;             // for instance
  CHAR16  String[32];
} BIOS_IMAGE_BLOCK_INFO;

typedef struct {
  UINT32                 Signature;
  UINT32                 Length;
  UINT32                 Version;
  UINT32                 Reserved;  
  UINT32                 Signature2;
  UINT64                 BiosBaseAddr;
  UINT64                 BiosSize;
  UINT32                 InfoCount;
  UINT32                 PlatfromType;
  UINT32                 Offset;
  UINT64                 SpiControllerBase;  
} BYO_BIOS_INFO_HEADER;
 
typedef struct {
  BYO_BIOS_INFO_HEADER   Header;
  BIOS_IMAGE_BLOCK_INFO  Info[1];
} BYO_BIOS_INFO;

typedef struct {
  UINT32    Signature;
  UINT16    Length;
  UINT16    Version;
  UINT64    SmbiosNvOffset;
  UINT32    SmbiosNvSize;
  UINT64    FlashBase;
  UINT32    FlashSize;
  UINT64    SpiControllerBase;
  UINT32    SpiControllerSize;
  UINT8     CmdWrite;
  UINT8     CmdErase;
  UINT8     CmdPp;
  UINT8     PlatformType;
  UINT8     Reserved2[3];
  UINT32    Reserved[9];
} FLASH_INFO_TABLE;

#pragma pack()

#endif
