/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By: 
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/



#ifndef __HDD_BIND_DXE_H__
#define __HDD_BIND_DXE_H__


#include "HddBindDxeVfr.h"
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/SetupSaveNotify.h>

#define HDD_BIND_CTX_SIGNATURE          SIGNATURE_32 ('H', 'D', 'B', 'D')

typedef struct {
  UINTN                     Type;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  UINTN                     Bus;
  UINTN                     Dev;
  UINTN                     Fun;
  UINTN                     Port;
  UINT8                     Sn[21];
  UINT16                    Mn[41];
} HDD_BIND_INFO;

typedef struct {
  UINT32                         Signature;
  EFI_HII_HANDLE                 HiiHandle;
  EFI_HANDLE                     DriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL ConfigAccess;
  HDD_BIND_VAR_DATA              VarData;
  HDD_BIND_INFO                  *HddBindInfo;
  UINTN                          HddBindInfoCount;
  UINTN                          TagetHddIndex;
  VOID                           *HddBindNv;
  UINTN                          HddBindNvSize;
  BOOLEAN                        GetHddBindNv;
  BOOLEAN                        UserChoice;                          
  CHAR8                          *TargetSn;
  CHAR16                         *TargetMn;
} HDD_BIND_CTX;

#define HDD_BIND_DATA_FROM_THIS(a)  CR(a, HDD_BIND_CTX, ConfigAccess, HDD_BIND_CTX_SIGNATURE)


#define HDD_BIND_NV_NAME         L"hbv"
#define HDD_BIND_NV_GUID \
{ 0x2b25602b, 0x7c14, 0x47eb, { 0xbe, 0xe9, 0x90, 0x9f, 0xb8, 0xa1, 0xda, 0x99 } }


extern EFI_GUID gByoHddBindFormsetGuid;

#endif

