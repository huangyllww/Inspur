/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiSendStatusCodeHandler.h

Abstract:
  Source file for the IPMI PEIM.

Revision History:

**/
#ifndef _EFI_IPMI_SEND_STATUSCODE_HANDLE_H_
#define _EFI_IPMI_SEND_STATUSCODE_HANDLE_H_


#include <Uefi.h>
#include <Library/HobLib.h>
#include <Library/IpmiBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SerialPortLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <ByoStatusCode.h>
#include <BmcConfig.h>
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <Library/ServerManagementLib.h>
#include <Library/UefiLib.h>
#include <Library/IpmiBaseLib.h>


#define  RESERVED       0xFFFFFFFA
#define  DISABLED_STATUS_CODE      0
#define  PROGRESS_AND_ERROR_CODE   1
#define  ERROR_CODE                2
#define  PROGRESS_CODE             3
#define  MAX_STATUS_CODE_NUMBER    35
#define  OEM_DATA_PROGRESS_CODE    0xC2
#define  OEM_DATA_ERROR_CODE       0xC0

STATIC EFI_GUID gIpmiSelPcInfoHobGuid = { 0x136b544a, 0x1ff7, 0x4a3d, { 0xbc, 0x78, 0x14, 0x91, 0x56, 0xaa, 0x44, 0x5d } };

#pragma pack(1)
typedef struct {
  UINT8            Count;
  UINT8            StatusCodeLog;
  UINT8            StatusCodeType[6];
  UINT8            IpmiEventData[6];
  UINT32           TimeStamp[6];
} IPMI_SEL_PC_INFO;
#pragma pack()


EFI_STATUS
EfiSmGetTimeStamp (
  OUT UINT32 *NumOfSeconds
  );


typedef struct {
  UINT32           StatusCodeType;
  UINT32           Flagbit;
  UINT8            IpmiEventData;
} IPMI_STATUS_CODE;


IPMI_STATUS_CODE gIpmiProgressCode[] = {
  { PEI_MEMORY_INIT ,                                   BIT0  ,   0x01 },
  { DXE_ATA_BEGIN  ,                                    BIT1  ,   0x02 },
  { DXE_CPU_AP_INIT  ,                                  BIT2  ,   0x03 },
  { RESERVED ,                                          BIT3  ,   0x04 },
  { DXE_SETUP_START  ,                                  BIT4  ,   0x05 },
  { DXE_USB_BEGIN  ,                                    BIT5  ,   0x06 },
  { DXE_PCI_BUS_REQUEST_RESOURCES ,                     BIT6  ,   0x07 },
  { DXE_LEGACY_OPROM_INIT  ,                            BIT7  ,   0x08 },
  { DXE_CON_OUT_CONNECT  ,                              BIT8  ,   0x09 },
  { DXE_CPU_CACHE_INIT ,                                BIT9  ,   0x0A },
  { EFI_IO_BUS_SMBUS  ,                                 BIT10 ,   0x0B },
  { DXE_KEYBOARD_INIT  ,                                BIT11 ,   0x0C },
  { RESERVED  ,                                         BIT12 ,   0x0D },
  { EFI_PERIPHERAL_DOCKING | EFI_P_PC_PRESENCE_DETECT , BIT13 ,   0x0E },
  { EFI_PERIPHERAL_DOCKING | EFI_P_PC_ENABLE ,          BIT14 ,   0x0F },
  { EFI_PERIPHERAL_DOCKING | EFI_P_PC_REMOVED ,         BIT15 ,   0x10 },
  { EFI_PERIPHERAL_DOCKING | EFI_P_PC_DISABLE ,         BIT16 ,   0x11 },
  { RESERVED  ,                                         BIT17 ,   0x12 },
  { RESERVED ,                                          BIT18 ,   0x13 },
  { RESERVED  ,                                         BIT19 ,   0x14 },
  { RESERVED  ,                                         BIT20 ,   0x15 },
  { RESERVED ,                                          BIT21 ,   0x16 },
  { DXE_KEYBOARD_SELF_TEST  ,                           BIT22 ,   0x17 },
  { RESERVED  ,                                         BIT23 ,   0x18 },
  { PEI_CAR_CPU_INIT ,                                  BIT24 ,   0x19 }
};


IPMI_STATUS_CODE gIpmiErrorCode[] = {
  { PEI_MEMORY_NOT_DETECTED ,                BIT0  ,  0x01 },
  { PEI_MEMORY_NONE_USEFUL  ,                BIT1  ,  0x02 },
  { DXE_ATA_DEVICE_FAILURE  ,                BIT2  ,  0x03 },
  { RESERVED ,                               BIT3  ,  0x04 },
  { RESERVED  ,                              BIT4  ,  0x05 },
  { DXE_ATA_CONTROLLER_ERROR  ,              BIT5  ,  0x06 },
  { EFI_IO_BUS_USB | EFI_P_EC_NOT_DETECTED , BIT6  ,  0x07 },
  { RESERVED  ,                              BIT7  ,  0x08 },
  { RESERVED  ,                              BIT8  ,  0x09 },
  { RESERVED ,                               BIT10 ,  0x0a },
  { RESERVED  ,                              BIT11 ,  0x0b },
  { RESERVED  ,                              BIT12 ,  0x0c },
  { DXE_CPU_INVALID_SPEED ,                  BIT13 ,  0x0d }
};

#endif