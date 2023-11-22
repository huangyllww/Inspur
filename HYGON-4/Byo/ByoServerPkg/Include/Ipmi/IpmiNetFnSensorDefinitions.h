/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiNetFnSensorDefinitions.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _IPMINETFNSENSORDEFINITIONS_H_
#define _IPMINETFNSENSORDEFINITIONS_H_

//
// Net function definition for Sensor command
//
#define EFI_SM_NETFN_SENSOR 0x04

//
// All Sensor commands and their structure definitions to follow here
//

/*----------------------------------------------------------------------------------------
    Definitions for Send Platform Event Message command     
----------------------------------------------------------------------------------------*/
#define EFI_SENSOR_PLATFORM_EVENT_MESSAGE   0x02

#define EFI_GET_SENSOR_READING              0x2d

#define EFI_GET_SENSOR_FACTORS_READING      0x23


#pragma pack(1)
typedef struct {
  UINT8   GeneratorId;
  UINT8   EvMRevision;
  UINT8   SensorType;
  UINT8   SensorNumber;
  UINT8   EventDirType;
  UINT8   OEMEvData1;
  UINT8   OEMEvData2;
  UINT8   OEMEvData3;
} EFI_PLATFORM_EVENT_MESSAGE_DATA;
#pragma pack()
#endif

