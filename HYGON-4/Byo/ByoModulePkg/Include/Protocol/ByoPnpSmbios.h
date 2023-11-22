/** @file

 Copyright (c) 2010 - 2019, Byosoft Corporation.<BR>
 All rights reserved.This software and associated documentation (if any)
 is furnished under a license and may only be used or copied in
 accordance with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be reproduced,
 stored in a retrieval system, or transmitted in any form or by any
 means without the express written consent of Byosoft Corporation.

 File Name:

 Abstract:

 Revision History:

**/

#ifndef _BYO_SMBIOS_DXE_H_
#define _BYO_SMBIOS_DXE_H_
#include <Protocol/Smbios.h>
//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_BYO_PNP_SMBIOS_PROTOCOL  EFI_BYO_PNP_SMBIOS_PROTOCOL;

typedef 
EFI_STATUS
(EFIAPI *BYO_PNP_UPDATE_SMBIOS) (
  IN  EFI_BYO_PNP_SMBIOS_PROTOCOL *This,
  IN  UINT8                       SmbiosType,
  IN  UINTN                       StringNo,
  IN  CHAR8                       *Buffer,
  IN  BOOLEAN                     IsUpdateUuid
  );

typedef 
EFI_STATUS
(EFIAPI *BYO_PNP_UPDATE_SMBIOS_EX) (
  IN  EFI_BYO_PNP_SMBIOS_PROTOCOL *This,
  IN  EFI_SMBIOS_HANDLE           SmbiosHandle,
  IN  UINT8                       SmbiosType,
  IN  UINTN                       StringNo,
  IN  CHAR8                       *Buffer,
  IN  BOOLEAN                     IsUpdateUuid
  );

struct _EFI_BYO_PNP_SMBIOS_PROTOCOL {
  BYO_PNP_UPDATE_SMBIOS    UpdateSmbios;
  BYO_PNP_UPDATE_SMBIOS_EX  UpdateSmbiosEx;
};


extern EFI_GUID  gEfiByoPnpSmbiosProtocolGuid;


#endif
