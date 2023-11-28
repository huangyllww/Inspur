/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPassword2.h

Abstract:
  Implementation of basic setup password function.

Revision History:

**/

#ifndef __SYSTEM_PASSWORD2_VARIABLE___H__
#define __SYSTEM_PASSWORD2_VARIABLE___H__


#define SYSTEM_PASSWORD2_HASH_LENGTH    32

#define SYSTEM_PASSWORD2_GUID   { 0x5f7501ff, 0x2ab2, 0x4a0d, { 0xa1, 0xb3, 0x77, 0xbb, 0x78, 0x3e, 0xfb, 0xda } }

#pragma pack(1)
typedef struct {
  UINT8     bHaveAdmin;
  UINT8     bHavePowerOn;  
  UINT8     AdminHash[SYSTEM_PASSWORD2_HASH_LENGTH];
  UINT8     PowerOnHash[SYSTEM_PASSWORD2_HASH_LENGTH];
} SYSTEM_PASSWORD2;
#pragma pack()

#define SYSTEM_PASSWORD2_NAME   L"SysPd2"

#define SYSTEM_PASSWORD2_NV_ATTRIBUTE (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)

#define SYSTEM_PASSWORD2_VARSTORE    \
     efivarstore SYSTEM_PASSWORD2, attribute = 0x7, name = SysPd2, guid = SYSTEM_PASSWORD2_GUID;

#define SYS_PD2               SYSTEM_PASSWORD2
#define LOGIN_USER_ADMIN      0
#define LOGIN_USER_POP        1
#define LOGIN_USER_MAX        2

extern EFI_GUID gByoSystemPassword2VariableGuid;

#endif
