/*++

  Copyright (c) 2023 Byosoft Corporation. All rights reserved.
  This program and associated documentation (if any) is furnished
  under a license. Except as permitted by such license,no part of this
  program or documentation may be reproduced, stored divulged or used
  in a public system, or transmitted in any form or by any means
  without the express written consent of Byosoft Corporation.

Module Name:

Abstract:

Revision History:

--*/

#ifndef __DISPLAY_NVMESS_CONF_H__
#define __DISPLAY_NVMESS_CONF_H__

#include <Guid/SetupGuiCustom.h>

// 28a40cd7-4165-a9fd-47bf-df242d1fdc33
//{ 0x28a40cd7, 0x4165, 0xa9fd, { 0x47, 0xbf, 0xdf, 0x24, 0x2d, 0x1f, 0xdc, 0x33 }}

#define SETUP_DISPLAY_NVMESSD_GUID \
  { 0x28a40cd7, 0x4165, 0xa9fd, { 0x47, 0xbf, 0xdf, 0x24, 0x2d, 0x1f, 0xdc, 0x33 }}

#define VFR_FORMID_DISPLAY_NVMESSD             0x1004

extern EFI_GUID gByoSetupDisplayNVMeSSDGuid;

#endif