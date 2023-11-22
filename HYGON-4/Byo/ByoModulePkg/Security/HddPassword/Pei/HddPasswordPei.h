/** @file
  HddPassword PEI module which is used to unlock HDD password for S3.

  Copyright (c) 2022, Byosoft Corporation. All rights reserved.<BR>

  This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.


**/

#ifndef _HDD_PASSWORD_PEI_H_
#define _HDD_PASSWORD_PEI_H_

#include <PiPei.h>
#include <IndustryStandard/Atapi.h>

#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/PciLib.h>
#include <Library/LockBoxLib.h>

#include <Ppi/AtaPassThru.h>

#include <HdpS3Info.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Library/ByoCommLib.h>
//
// Time out value for ATA PassThru PPI
// When unlocking the password,the 3-second timeout value of some hard disks is not enough,
// so it is changed to 5 seconds.
//
#define ATA_TIMEOUT                          50000000
//
// According to ATA spec, the max length of hdd password is 32 bytes
//
#define HDD_PASSWORD_MAX_LENGTH         32

//#define HDD_PASSWORD_DEVICE_INFO_GUID { 0xb361d186, 0xc021, 0x4f72, { 0x86, 0x8, 0x16, 0xbf, 0xbd, 0xdb, 0x14, 0x11 } }

typedef struct {
  UINT8             Bus;
  UINT8             Device;
  UINT8             Function;
  UINT8             Reserved;
  UINT16            Port;
  UINT16            PortMultiplierPort;
} HDD_PASSWORD_DEVICE;
//
// It will be used to unlock HDD password for S3.
//
typedef struct {
  HDD_PASSWORD_DEVICE         Device;
  CHAR8                       Password[HDD_PASSWORD_MAX_LENGTH];
  UINT32                      DevicePathLength;
  EFI_DEVICE_PATH_PROTOCOL    DevicePath[];
} HDD_PASSWORD_DEVICE_INFO;

#endif
