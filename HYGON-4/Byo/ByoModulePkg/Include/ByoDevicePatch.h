/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_DEVICE_PATCH_H__
#define __BYO_DEVICE_PATCH_H__

#define USB_CONTROLLER_GUID { \
    0x70E12AC7, 0x04B9, 0xFFFE, { 0x44, 0x41, 0xCB, 0x5E, 0x99, 0xF6, 0x41, 0x0B } \
}

#pragma pack(1)

typedef struct {
  EFI_DEVICE_PATH_PROTOCOL        Header;
  EFI_GUID                        Guid;
  UINT64                          MmioBaseAddress;
} SOC_MMIO_DEVICE_PATH;

#pragma pack()

extern EFI_GUID gByoUsbControllerGuid;

#endif
