/** @file
 Byosoft IPMI industrial standard define file.

 Copyright (c) 2022, Byosoft Corporation.<BR>

 All rights reserved.This software and associated documentation (if any)
 is furnished under a license and may only be used or copied in
 accordance with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be reproduced,
 stored in a retrieval system, or transmitted in any form or by any
 means without the express written consent of Byosoft Corporation.

*/

#ifndef _IPMI_STANDARD_H_
#define _IPMI_STANDARD_H_

#pragma pack(1)

//
// IPMI Device Global Command
//
#define IPMI_CMD_GET_DEVICE_ID          0x01
//
// Structure to store information read back when executing GetDeviceId command
//
typedef struct {
  UINT8 DeviceId;
  UINT8 DeviceRevision      :4;
  UINT8 Reserved            :3;
  UINT8 ProvideSDRsDevice   :1;
  UINT8 MajorFmRevision   :7;
  UINT8 DeviceAvailable   :1;
  UINT8 MinorFmRevision;
  UINT8 IpmiVersionMostSigBits  :4;
  UINT8 IpmiVersionLeastSigBits :4;
  UINT8 SensorDevice        :1;
  UINT8 SDRRepositoryDevice :1;
  UINT8 SELDevice           :1;
  UINT8 FRUInventoryDevice  :1;
  UINT8 IPMBEventReceiver   :1;
  UINT8 IPMBEventGenerator  :1;
  UINT8 Bridge              :1;
  UINT8 ChassisDevice       :1;
  UINT8 ManufacturerId[3];
  UINT8 ProductId[2];
  UINT8 AuxInfo[4];
} IPMI_BMC_INFO;

//
// BMC Device and Messaging Commands
//
#define IPMI_CMD_CLEAR_MESSAGE_FLAGS                0x30
#define IPMI_CMD_GET_MESSAGE                        0x33
#define IPMI_CMD_SEND_MESSAGE                       0x34

//
// IPMB Message Data for Send Message Command
//
typedef struct {
  UINT8   Channel;
  UINT8   DestAddr;
  UINT8   DestLun  :2;
  UINT8   NetFn    :6;
  UINT8   Check1;
  UINT8   SrcAddr;
  UINT8   SrcLun   :2;
  UINT8   Seq	   :6;
  UINT8   Cmd;
  UINT8   Data[128];
} IPMB_REQUEST_VIA_SEND_MESSAGE;

//
// IPMB Message Data for Get Message Command
//
typedef struct {
  UINT8   Channel;
  UINT8   SrcLun   :2;
  UINT8   NetFn    :6;
  UINT8   Check1;
  UINT8   DestAddr;
  UINT8   DestLun  :2;
  UINT8   Seq      :6;
  UINT8   Cmd;
  UINT8   Data[128];
} IPMB_RESPONSE_VIA_GET_MESSAGE;

//
// LAN Device Commands
//
#define IPMI_CMD_GET_LAN_PARAMETERS                       0x02
//
// LAN Configuration Parameter
//
#define LAN_IP_ADDRESS                                    3


#pragma pack()

#endif
