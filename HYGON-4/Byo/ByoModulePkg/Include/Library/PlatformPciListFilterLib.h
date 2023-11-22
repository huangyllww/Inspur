/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _PLATFORM_PCI_LIST_FILTER_LIB_H_
#define _PLATFORM_PCI_LIST_FILTER_LIB_H_

typedef struct {
  UINT8  Bus;
  UINT8  Dev;
  UINT8  Func;
  UINT8  ClassCode[3];  
  UINT32 Id;
  UINT8  LinkSpeed;
  UINT8  CurLinkWidth;
  UINT8  MaxLinkWidth;
  UINT8  MaxLinkSpeed;
  UINT8  ASPMSupport;
  CHAR16 *PciDeviceByName;
  CHAR16  RootPortName[20];
  UINT32  Seg;
} PCI_IO_CTX;

typedef struct {
  CHAR16                *LinkSpeedStr;
  CHAR16                *MaxLinkSpeedStr;
  CHAR16                CurLinkWidthStr[4];
  CHAR16                MaxLinkWidthStr[4];
  CHAR16                *ASPMSupportStr;
  CHAR16                *PciDeviceNameStr;
} PCI_IO_DISPLAY_STR;

typedef
BOOLEAN
(EFIAPI *PLATFORM_PCI_LIST_FILTER_HOOK_FUNC) (
  IN  PCI_IO_CTX      *PciIoCtx,
  IN OUT CHAR16       **PciDevName
);

extern PLATFORM_PCI_LIST_FILTER_HOOK_FUNC       gPlatformPciListFilterFunc;

#endif