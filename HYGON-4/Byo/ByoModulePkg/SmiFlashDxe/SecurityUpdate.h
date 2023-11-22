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

#ifndef __SECURITY_UPDATE_H__
#define __SECURITY_UPDATE_H__

#include <Guid/SetupGuiCustom.h>

// 5b056aa4-b0e8-0d48-884c-3729b5b3b409
//{ 0x5b056aa4, 0xb0e8, 0x0d48, { 0x88, 0x4c, 0x37, 0x29, 0xb5, 0xb3, 0xb4, 0x09 }}

#define SETUP_SECURITY_UPDATE_GUID \
  { 0x5b056aa4, 0xb0e8, 0x0d48, { 0x88, 0x4c, 0x37, 0x29, 0xb5, 0xb3, 0xb4, 0x09 }}

#define VFR_FORMID_SECURITY_UPDATE             0x1003

extern EFI_GUID gByoSetupSecurityUpdateGuid;

#endif