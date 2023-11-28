/** @file
  Provides a secure platform-specific method to detect physically present user.

Copyright (c) 2020, Byosoft Corporation. All rights reserved.<BR>
This program and the accompanying materials 
are licensed and made available under the terms and conditions of the BSD License 
which accompanies this distribution.  The full text of the license may be found at 
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS, 
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#ifndef __BYO_SECURE_LIB_H__
#define __BYO_SECURE_LIB_H__


#define PP_VALID_FLAG  SIGNATURE_32('_', 'W', 'Z', '_')

// {77fa9abd-0359-4d32-bd60-28f4e78f784b}.
STATIC EFI_GUID gMicrosoftSignatureOwnerGuid = \
{0x77fa9abd, 0x0359, 0x4d32, {0xBD, 0x60, 0x28, 0xF4, 0xE7, 0x8F, 0x78, 0x4b}};

// {7AE32DDF-1623-4904-AC2B-206AD65790D4}
STATIC EFI_GUID gMySignatureOwnerGuid = \
{0x7ae32ddf, 0x1623, 0x4904, {0xac, 0x2b, 0x20, 0x6a, 0xd6, 0x57, 0x90, 0xd4}};

typedef struct{
  EFI_GUID          *VarGuid;
  CHAR16            *VarName;
} AUTHVAR_KEY_NAME;

typedef struct{
  EFI_GUID          *File;
  CHAR16            *UiName;
  AUTHVAR_KEY_NAME  *KeyName;
  UINT8             *Data;
  UINTN             DataSize;
  EFI_GUID          *SignatureOwner;
} RESTORED_KEY_FILE;

EFI_STATUS
AuthRemoveAllCertKeys(
  VOID
  );

EFI_STATUS
AuthRestoreFactoryKeys (
  VOID
  );

#endif
