/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __OPAL_SP_STORAGE_H__
#define __OPAL_SP_STORAGE_H__


#pragma pack(1)

typedef struct{
  EFI_GUID Signature;          // GUID of this data structure
                               // {AD0A29F0-E60C-4DBB-868D-EDAFA2C4A8D9}
  UINT16   StructureVersion;   // Structure version
                               // Start from 0x0000. Next version is 0x0001
  UINT16   Length;             // Structure length in byte
  UINT16   OwnerCategory;      // Owner Category who installed the password
                               // 0x0000 = Notebook
                               // 0x0001 = Desktop
                               // 0x0002 = Workstation
                               // 0x0003-0xFFFF = Reserved
  UINT16   OwerID;                         
  UINT32   PasswordStatus;     // Password installation status (0=Not installed, 1=Installed)
                               //    Bit0: Admin password status
                               //    Bit1: User password status
                               //    Bit2-31: Reserved for future expansion

} OPAL_DATASTORE_OEM_INFO;

#pragma pack()

#define ADMIN_PASSWORD BIT0
#define USER_PASSWORD  BIT1

extern EFI_GUID gOpalDataStoreSignatureGuid;


#endif
