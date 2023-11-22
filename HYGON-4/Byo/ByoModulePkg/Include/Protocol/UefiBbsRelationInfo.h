/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __UEFI_BBS_RELATION_PROTOCOL_INFO__
#define __UEFI_BBS_RELATION_PROTOCOL_INFO__

#include <Uefi.h>

#define ATAPI_DEVICE              0x8000

#define MAXSIZE_OF_SN   21

typedef struct _UEFI_BBS_RELATION_PROTOCOL  UEFI_BBS_RELATION_INFO_PROTOCOL;

typedef struct {
  UINTN						        UefiHddIndex;
  UINT16            			BbsIndex;
  UINTN							      Bus;
  UINTN             			Device;
  UINTN             			Function;
  EFI_DEVICE_PATH_PROTOCOL		*HddDevicePath;	 
  UINTN 	  					    HddPortIndex;  
  UINTN 						      SataHostIndex;
  CHAR8							      SerialNo[MAXSIZE_OF_SN];
  UINT32                  Priority;
  UINT8                   Status;
} UefiBbsHddRL;

typedef struct {
  UINT16                  BbsIndex;
  EFI_DEVICE_PATH_PROTOCOL  *UsbDevicePath;
} UefiBbsUsbRL;


struct _UEFI_BBS_RELATION_PROTOCOL {
  UefiBbsHddRL            *UefiBbsHddTable;
  UefiBbsUsbRL            *UefiBbsUsbTable;
  UINTN                   UefiSataHddCount;
  UINTN                   UefiUsbCount;
};

extern EFI_GUID gUefiBbsRelationProtocolGuid;


#endif

