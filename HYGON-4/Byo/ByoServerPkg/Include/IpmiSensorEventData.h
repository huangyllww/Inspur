/** @file
  
Copyright (c) 2017, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiSensorEventData.h

Abstract: 
  GUID for ByoModulePkg Server management module. 

Revision History:

Bug Num: Description of this bug.
TIME: 2017-5-16
$AUTHOR:   Phinux Qin  

$END----------------------------------------------------------------------------

**/

#ifndef __IPMI_SENSOR_EVENT_DATA_H__
#define __IPMI_SENSOR_EVENT_DATA_H__

#define BYO_RAS_GID                        0x39
#define BYO_RAS_MCA_GID                    0x37
#define BYO_OEM_SEL_DEF                    0xE9

#define SENSOR_RAS_MEM_ECC_ERROR           0x01
#define SENSOR_RAS_MEM_MULTIBIT_ERROR      0x02
#define SENSOR_RAS_MEM_SCRUB_UNCOR_ERROR   0x03
#define SENSOR_RAS_MEM_MIRROR_ERROR        0x05
#define SENSOR_RAS_MEM_PARITY_ERROR        0x06
#define SENSOR_RAS_MEM_MISC_ERROR          0x07


#define SENSOR_RAS_PCI_LEGACY_ERROR        0x10
#define SENSOR_RAS_PCIBRIDGE_LEGACY_ERROR  0x11
#define SENSOR_RAS_PCIE_AER_UNCORRE_ERROR  0x14
#define SENSOR_RAS_PCIE_AER_CORRE_ERROR    0x15
#define SENSOR_RAS_PCIE_ERROR              0x16

#define PCI_AER_SENSOR_EVENT_TYPE          0x70
#define PCI_MISC_SENSOR_EVENT_TYPE         0x72
#define MEM_MISC_SENSOR_EVENT_TYPE         0x74

#define SEL_VIEWER_DEBUG(x)

#pragma pack(1)

typedef struct {
  UINT32    TimeStamp;
  UINT16    GeneratorId;
  UINT8     EvMRevision;
  UINT8     SensorType;
  UINT8     SensorNumber;
  UINT8     EventType:   7;
  UINT8     EventDir:    1;
  UINT8     EvData1;
  UINT8     EvData2;
  UINT8     EvData3;
} STD_SEL_RECORD_DATA;

typedef struct {
  UINT32    TimeStamp;
  UINT8     ManufactureId[3];
  UINT8     OemData[6];
} OEM_SEL_RECORD_DATA;

typedef struct {
  UINT8     OemData[13];
} OEM_SEL_RECORDT2_DATA;

typedef struct {
  //UINT8     ApciId[3];  ////It looks that 24bits would be ok.
  //UINT16    MsrNum; 
  UINT32     ApicId;
  UINT8      BankNum:       6;
  UINT8      BankReg:       2;
  UINT64     MsrData;
} OEM_SEL_RECORD_MCAINFO;

typedef union {
  STD_SEL_RECORD_DATA            StdSelData;
  OEM_SEL_RECORD_DATA            OemSelData;
  OEM_SEL_RECORDT2_DATA          OemSelDataT2;
  OEM_SEL_RECORD_MCAINFO         Msr64SelData;
} SEL_DATA;

typedef struct {
  UINT16      RecordId;
  UINT8       RecordType;
  SEL_DATA    SelData;
} SEL_RECORD;

#pragma pack()
  
#endif