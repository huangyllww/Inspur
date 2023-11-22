/** @file
  Implementation of Opal password support library.

Copyright (c) 2016 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/

#ifndef _DXE_OPAL_NOTIFY_H_
#define _DXE_OPAL_NOTIFY_H_

#include <PiDxe.h>
#include <PiSmm.h>

#include <Uefi/UefiAcpiDataTable.h>
#include <Guid/PiSmmCommunicationRegionTable.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/OpalPasswordSupportLib.h>
#include <Library/DevicePathLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/SmmCommunication.h>
#include <Protocol/SmmBase2.h>


#pragma pack(1)

typedef struct {
  UINTN       Function;
  EFI_STATUS  ReturnStatus;
  UINT8       Data[1];
} OPAL_SMM_COMMUNICATE_HEADER;

typedef struct {
  UINT8                      Password[32];
  UINT8                      PasswordLength;

  EFI_DEVICE_PATH_PROTOCOL   OpalDevicePath;
} OPAL_COMM_DEVICE_LIST;

#pragma pack()

#define SMM_FUNCTION_SET_OPAL_PASSWORD        1

#define OPAL_PASSWORD_NOTIFY_PROTOCOL_GUID {0x0ff2ddd0, 0xefc9, 0x4f49, { 0x99, 0x7a, 0xcb, 0x59, 0x44, 0xe6, 0x97, 0xd3 } }

#endif
