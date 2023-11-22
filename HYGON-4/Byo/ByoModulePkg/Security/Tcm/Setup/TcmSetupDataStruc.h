/** @file

Copyright (c) 2006 - 2011, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  TcmSetupDataStruc.h

Abstract:
  Dxe part of TCM Module.

Revision History:

Bug 3075 - Add TCM support.
TIME: 2011-11-14
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. Tcm module init version.
     Only support setup function.
$END--------------------------------------------------------------------

**/



#ifndef __TCM_SETUP_DATA_STRUC__
#define __TCM_SETUP_DATA_STRUC__

#include <Guid/HiiPlatformSetupFormset.h>
#include <Guid/TcmSetupCfgGuid.h>
#include <TcmPlatformDataStruc.h>

#define TCM_CONFIG_VARSTORE_ID      0x0011
#define TCM_STATE_VARSTORE_ID       0x0012

#define TCM_CONFIG_FORM_ID          0x0001
//#define KEY_TCM_ENABLE                1101
//#define KEY_TCM_ACTIVATE              1102
//#define KEY_TCM_FORCE_CLEAR           1103


#define KEY_HIDE_TCM                   0x2000
#define KEY_TCM_ACTION                 0x3000
#define KEY_TCM_MOR_ENABLE             0x4000
#define KEY_SECURITY_DEVICE            0x5000
#define KEY_TCM_ENABLE                 0x6000
#define KEY_TCM_OPERATION              0x7000

#define KEY_PENDING_NONE               0x0
#define KEY_PENDING_ACTIVE             0x1
#define KEY_PENDING_DEACTIVE           0x2
#define KEY_PENDING_CLEAR              0x3

#pragma pack(1)
typedef struct {
  UINT8   TcmPresent;
  UINT8   TcmActive;
  UINT8   TcmEnableStatus;
} TCM_STATE_CONFIG;
#pragma pack()

#endif      // __TCM_SETUP_DATA_STRUC__

