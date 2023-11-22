/** @file
  System configuration setting Structure PCD.

Copyright (c) 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __SYSTEM_MISC_SETTING_H__
#define __SYSTEM_MISC_SETTING_H__

#define ROM_POLICY_LEGACY_FIRST  0
#define ROM_POLICY_UEFI_FIRST    1
#define ROM_POLICY_NO_LAUNCH     2
#define ROM_POLICY_BY_CSM        3
#define ROM_POLICY_ALL           4

typedef struct {
  union {
    struct {
      UINT32    ConfigDualVga                : 1;
      UINT32    ConfigPriVgaIgd              : 1;
      UINT32    NvmeAddOnOprom               : 1;
      UINT32    PciVgaPaletteSnoopingSupport : 1;
      UINT32    Reserved                     : 16;
      UINT32    VideoOpRomLaunchPolicy       : 3;
      UINT32    PxeOpRomLaunchPolicy         : 3;
      UINT32    StorageOpRomLaunchPolicy     : 3;
      UINT32    OtherOpRomLaunchPolicy       : 3;
    } Bits;
    UINT32    Uint32;
  } PciSetting;
  union {
    struct {
      UINT32    LegacyBiosSupport : 1;
      UINT32    Reserved          : 31;
    } Bits;
    UINT32    Uint32;
  } SysSetting;
} SYSTEM_MISC_SETTING;

#endif
