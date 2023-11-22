/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  HpPlatformLib.h
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/
#ifndef __HDP_S3_INFO_H__
#define __HDP_S3_INFO_H__

#pragma pack(1)
typedef struct {
  UINT8 Bus;
  UINT8 Dev;
  UINT8 Func;
  UINT16 Port;              //Used for SATA HDD unlock
  UINT16 PortMp;            //Used for SATA HDD unlock
  UINT8 Scc;                //Used to distinguish NVMe and SATA
  BOOLEAN IsUser;           //Used as Identifier
  BOOLEAN IsAtaSec;
  UINT32  Nsid;             //Used for NVME OPAL unlock
  UINT8   Sn[20];           //NVMe Product serial number
  UINT8 Password[32];
} HDP_PORT_INFO;

typedef struct {
  UINT8          EnCount;
  UINT8          Pad;
  HDP_PORT_INFO  Port[1];
} HDP_S3_DATA;
#pragma pack()

#endif

