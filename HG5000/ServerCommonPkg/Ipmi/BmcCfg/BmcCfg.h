/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BmcCfg.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/



//
// Statements that include other files
//
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BiosIdLib.h>

#include <BmcConfig.h>

#include <IpmiDefinitions.h>
#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiInterfaceProtocol.h>
#include <Protocol/BmcWdtProtocol.h>
#include <Protocol/BmcCfgProtocol.h>
#include <Protocol/Smbios.h>
#include <Protocol/PciIo.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/DevicePath.h>
#include <Protocol/NvmExpressPassthru.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/DevicePathToText.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <IndustryStandard/Atapi.h>
#include <IndustryStandard/Nvme.h>
#include <IndustryStandard/Pci30.h>
#include <SetupVariable.h>
#include <Library/HobLib.h>
#include <PlatformDefinition.h>

#include <Library/ByoCommLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/PciLib.h>

#include "JEP106.h"
#include <Guid/GnbPcieInfoHob.h>
#include <NbioRegisterTypes.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <SetupVariable.h>
#include <ByoBootGroup.h>
#include <Protocol/AmdNbioSmuServicesProtocol.h>


#include <Library/NbioHandleLib.h>
#include <Protocol/AmdNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Guid/GnbNbioBaseZPInfoHob.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioSmuV9Lib.h>

#include <Protocol/IpmiBootOptionProtocol.h>
#include <ByoBootGroup.h>

#ifndef _EFI_BMCCONFIG_H_
#define _EFI_BMCCONFIG_H_

//
// BMC CONFIG instance data
//
typedef struct {
  UINTN                       Signature;
  BMC_CONFIG_DATA             Data;
} EFI_BMC_CONFIG_INSTANCE;

#define SM_BMC_CONFIG_SIGNATURE SIGNATURE_32 ('b', 'm', 'v', 'b')

#define INSTANCE_FROM_EFI_BMC_CONFIG_THIS(a) CR (a, EFI_BMC_CONFIG_INSTANCE, Protocol, SM_BMC_CONFIG_SIGNATURE)

#define _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED   0x0400
#define ATAPI_DEVICE                            0x8000

#define BIOS_2_BMC_DATA_SIZE  300
#define MEMORY_SLOT_NUMBER    8

typedef enum {
  SET_BIOS_VER = 0,
  SET_CPU_INFO,
  SET_DIMM_INFO,
  SET_PCI_INFO,
  SET_HDD_INFO,
  //SET_OCP_INFO,
  SET_NVME_INFO,
  SET_LAN_INFO,
} BIOS_2_BMC_INFOR_FUN;

#pragma pack(1)

typedef struct {
  UINT8  Function;
  UINT8  BiosVerOffset;
  UINT8  BiosDateOffset;
  CHAR8  Data[1];
} BIOS_VERSION_2_BMC;

typedef struct {
  UINT8  Function;
  UINT16 L1DataCacheSize;     // KB
  UINT16 L1InstructCacheSize; // KB
  UINT16 L2CacheSize;         // KB
  UINT16 CurSpeed;            // MHZ
  UINT8  CoreCount;
  UINT8  CpuNameOffset;
  CHAR8  Data[1];
} CPU_INFO_2_BMC;

typedef struct {
  UINT8  Function;
  UINT16 MemSpeed;          // MHZ
  UINT16 TotalMemSize;      // GB
  CHAR8  ManuName[20];
  UINT16 SingleSize[MEMORY_SLOT_NUMBER];     // GB
  CHAR8  SerialNum[MEMORY_SLOT_NUMBER][10];
} DIMM_INFO_2_BMC;

typedef struct {
  UINT8  Function;
  UINT8  SataDeviceCount;
  CHAR8  Data[198];     // HDD strings
} HDD_INFO_2_BMC;


typedef struct {
  UINT32 VenderId;
  UINT8  Bus;
  UINT8  Device;
  UINT8  Function;
  UINT8  ClassCode[3];
} PCI_INFO;

typedef struct {
  UINT8     Function;
  UINT8     PciDeviceCount;
  PCI_INFO  PciInfo[1];
} PCI_INFO_2_BMC;

typedef struct {
  UINT32 VenderId;
  UINT8  Mac[6];
} LAN_INFO;

typedef struct {
  UINT8     Function;
  UINT8     LanCount;
  LAN_INFO  LanInfo[1];
} LAN_INFO_2_BMC;

typedef union{
  UINT32 time;
  UINT8  ch[4];
}TIME_INFO_2_BMC;

typedef union{
  UINT16 UctOffset;
  UINT8  Utc[2];
}UTC_INFO_2_BMC;


typedef struct {
  CHAR16  BoardId[7];               // "HX002EG"
  CHAR16  BoardRev;                 // "1"
  CHAR16  Dot1;                     // "."
  CHAR16  OemId[3];                 // "X64"
  CHAR16  Dot2;                     // "."
  CHAR16  VersionMajor[4];          // "_Rxx"
  CHAR16  Dot3;                     // "."
  CHAR16  BuildType;                // "D" or "R"
  CHAR16  VersionMinor[2];          // "00"
  CHAR16  Dot4;                     // "."
  CHAR16  TimeStamp[10];            // "YYMMDDHHMM"
  CHAR16  NullTerminator;           // 0x0000
} SLAVE_BIOS_ID_STRING;

typedef struct {
  UINT8           Signature[8];     // "$IBIOSI$"
  SLAVE_BIOS_ID_STRING  BiosIdString;     // "TNAPCRB1.86C.D0008.1106141018"
} SLAVE_BIOS_ID_IMAGE;

extern EFI_GUID gAsiaCpuCfgHobGuid;

#pragma pack()

#endif
