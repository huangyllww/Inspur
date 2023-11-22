/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SendInfoToBmc.h

Abstract:
  Header file for send bios info to BMC.

Revision History:

**/

#ifndef _SEND_INFO_TO_BMC_H_
#define _SEND_INFO_TO_BMC_H_

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>


#include <Library/DevicePathLib.h>

#include <Library/IpmiBaseLib.h>
#include <Library/ByoCommLib.h>
#include <Library/PciLib.h>
#include <BmcConfig.h>

#include <Protocol/PciIo.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/Smbios.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/ByoDiskInfoProtocol.h>

#include <IndustryStandard/Atapi.h>
#include <IndustryStandard/Pci30.h>


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
  CHAR8  BiosVer[32];
  CHAR8  BiosDate[32];
} BIOS_VERSION_2_BMC;

typedef struct {
  UINT8  Function;
  UINT8  DeviceNum;
  CHAR8  Name[16];
  UINT8  DeviceID;
  UINT8  ProcessorType;
  CHAR8  ProcessorArchitecture[8];
  CHAR8  InstructionSet[8];
  CHAR8  Manufacturer[16];
  CHAR8  Model[40];
  UINT32 ProcessorId;
  UINT16 MaxSpeed;
  UINT8  TotalCores;
  UINT8  TotalThreads;
  UINT8  Socket;
  CHAR8  Health[8];
  UINT16 L1CacheSize;
  UINT16 L2CacheSize;
  UINT16 L3CacheSize;
  CHAR8  DeviceLocator[8];
  UINT16 Frequency;
  CHAR8  Position[16];
  CHAR8  SerialNumbers[16];
} CPU_INFO_2_BMC;

typedef struct {
  UINT8  Function;
  UINT8  DeviceNum;
  CHAR8  Name[8];
  CHAR8  NameId[8];
  UINT16 Capacity;//GB
  CHAR8  Manufacturer[20];
  UINT16 OperatingSpeed;
  UINT16 CurrentSpeed;
  CHAR8  SerialNumber[12];
  CHAR8  MemoryDeviceType[8];
  UINT8  DataWidth;
  UINT8  RankCount;
  CHAR8  PartNumber[20];
  CHAR8  DeviceLocator[8];
  CHAR8  BaseModuleType[12];
  CHAR8  FirmwareRevision[8];
  CHAR8  Health[8];
} DIMM_INFO_2_BMC;

typedef struct {
  UINT8  Function;
  UINT8  DeviceNum;
  UINT16 VenderID;
  UINT16 DeviceID;
  UINT8  Bus;
  UINT8  Device;
  UINT8  Fun;
  UINT8  ClassCode[3];
  UINT8  DeviceClass;
  UINT8  FunctionID;
  UINT8  FunctionType;
  UINT8  RevisionID;
  UINT16 SubSystemVenderID;
  UINT16 SubSystemID;
  UINT8  MaxLinkWidth;
  UINT8  MaxLinkSpeed;
  UINT8  CurrentLinkWidth;
  UINT8  CurrentLinkSpeed;
  UINT8  Slot;
  UINT8  RiserType;
  UINT8  PCIELocationOnRiser;
  UINT8  RootBridgeBus;
  UINT8  RootBridgeDev;
  UINT8  RootBridgeFun;
} PCIE_INFO_2_BMC;

typedef struct {
  UINT8  Function;
  UINT8  DeviceNum;
  CHAR8  Name[40];
  CHAR8  Model[16];
  CHAR8  Revision[8];
  CHAR8  Health[8];
  CHAR8  State[8];
  UINT32 CapacityBytes;
  UINT8  FailurePredicted;
  CHAR8  Protocol[16];
  CHAR8  MediaType[8];
  CHAR8  Manufacturer[20];
  CHAR8  SerialNumber[20];
  UINT8  CapableSpeed;
  UINT8  NegotiatedSpeed;
  UINT16 PredictedMediaLifeLeftPercent;
  CHAR8  IndicatorLED[8];
  CHAR8  HotspareType[8];
  CHAR8  Status[16];
} HDD_INFO_2_BMC;

typedef struct {
  UINT8  Function;
  UINT8  DeviceNum;
  CHAR8  Name[40];
  CHAR8  Model[16];
  CHAR8  Revision[8];
  CHAR8  Health[8];
  CHAR8  State[8];
  UINT32 CapacityBytes;
  UINT8  FailurePredicted;
  CHAR8  Protocol[16];
  CHAR8  MediaType[8];
  CHAR8  Manufacturer[20];
  CHAR8  SerialNumber[20];
  UINT8  CapableSpeed;
  UINT8  NegotiatedSpeed;
  UINT16 PredictedMediaLifeLeftPercent;
  CHAR8  IndicatorLED[8];
  CHAR8  HotspareType[8];
  CHAR8  Status[16];
} NVME_INFO_2_BMC;

typedef struct {
  UINT8  Function;
  UINT8  DeviceNum;
  UINT8  PresentStatus;
  UINT8  ControlType;
  UINT8  PCIEIndex;
  UINT8  PortNum;
  UINT8  Mac[4][6];
} NETCARD_INFO_2_BMC;


#define BYO_SUBFUNC_EXTLOG           3

#define UCE_ISOLATION_TYPE_MEM       1
#define UCE_ISOLATION_TYPE_CPU       2


typedef struct {
  UINT8   RecordType;            // 1
  UINT32  TimeStamp;
  UINT64  Address;
  UINT32  Length;
} EXT_LOG_DATA_MEM_UCE;

typedef struct {
  UINT8   RecordType;            // 2
  UINT32  TimeStamp;
  UINT16  CoreId;
} EXT_LOG_DATA_CPU_UCE;

#pragma pack()

#endif
