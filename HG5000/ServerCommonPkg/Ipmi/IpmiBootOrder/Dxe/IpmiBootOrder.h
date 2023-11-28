/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

//
// Statements that include other files
//

#ifndef _SM_BOOT_ORDER_
#define _SM_BOOT_ORDER_

#include "IpmiBoot.h"

#define EFI_SMBO_BLOCK_SIZE_TABLE_SIGN    SIGNATURE_32('_','P','B','_')
#define EFI_SMBO_BOOT_ORDER_TABLE_SIGN    SIGNATURE_32('_','B','O','_')
#define SM_OEM_PARAM_MAJOR_VER            0x00
#define SM_OEM_PARAM_MINOR_VER            0x95

#define OEM_PARAM_SELECTOR_BLOCK_SIZE     120
#define OEM_PARAM_SELECTOR_BOOT_ORDER     125

#define SYSTEM_BOOT_ORDER_TYPE            0x00
#define FDD_ORDER_TYPE                    0x01
#define CD_DVD_ORDER_TYPE                 0x03
#define USB_ORDER_TYPE                    0x05
#define NETWORK_ORDER_TYPE                0x06
#define LOCAL_HDD_ORDER_TYPE              0x08
#define EXT_HDD_ORDER_TYPE                0x09
#define BEV_ORDER_TYPE                    0x80
#define EFI_DEVICE_ORDER_TYPE             0x10
#define END_OF_BOOT_ORDER_TYPE            0xFF

#define BIOS_UPDATED_BOOT_ORDER           0x01
#define SM_UPDATED_BOOT_ORDER             0x02    // SMS or CMM

#pragma pack(1)
typedef struct {
  UINT32    ParamSign;
  UINT8     Checksum;
  UINT8     Major;
  UINT8     Minor;
  UINT16    Length;
  UINT8     Resev;  
} OEM_PARAM_HEADER;

typedef struct {
  OEM_PARAM_HEADER  Header;
  UINT8             SiBlockSize;
  UINT8             Reserved1;
  UINT8             IpmbBlockSize;
  UINT8             Reserved2;
  UINT8             LanBlockSize;
  UINT8             Reserved3;
} OEM_PARAM_BLOCK_SIZE;


typedef struct {
  OEM_PARAM_HEADER  Header;
  UINT8             BootUpdateFlag;
//UINT8             BoData[];           // Variable size BO data
} OEM_BOOT_ORDER_TABLE;

typedef struct {
  UINT8           OrderType;
  UINT8           OrderLength;
//UINT8           DeviceList[];         // Ordered device list
} BOT_LEGACY_ORDER_LIST;

typedef BOT_LEGACY_ORDER_LIST SYSTEM_BOOT_ORDER_DATA;

typedef struct {
  UINT8           OrderType;
  UINT8           OrderLength;
//UINT16          DeviceList[];         // Ordered device list
} BOT_EFI_ORDER_LIST;

typedef struct {
  UINT8           DeviceType;
  UINT16          DeviceNum;
//UINT8           DeviceName[];         // Variable size name data
} BOT_LEGACY_DEVICE;

typedef struct {
  UINT8           DeviceType;
  UINT16          DeviceNum;
  UINT16          PathLength;
//CHAR16          DevName[];            // Variable size name data
//EFI_DEVICE_PATH DevicePath[];         // Variable size path data
} BOT_EFI_DEVICE;

typedef struct {
  UINT8           DeviceType;
  UINT16          DeviceNum;
  UINT16          DataStart;   // offset to whole BOT device name data 
  UINT16          DataLen;   
} BOT_DEVICE_DATA;

#define MAX_BOT_DEVICE_DATA_NUM 0x20

#pragma pack()

EFI_STATUS
IpmiSmSetBootOptions (
  IN  UINT8                           ParameterType,
  IN  UINT8                           Valid,
  IN  UINT8                           *ParamData,
  IN  UINT8                           *ParamSize
  );

EFI_STATUS
IpmiSmGetBootOptions (
  IN  UINT8                           ParameterType,
  IN OUT UINT8                        *ParamData,
  IN OUT UINT8                        *ParamSize
  );

#endif  //_SM_BOOT_ORDER_
