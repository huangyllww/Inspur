/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoPcdAccessProtocol.h

Abstract:
  Head file for the PCD Access protocol.
**/

#ifndef __BYO_PCD_ACCESS_PROTOCOL_DEF_H__
#define __BYO_PCD_ACCESS_PROTOCOL_DEF_H__

#define BYO_PCD_ACCESS_PROTOCOL_GUID  \
  { 0x813116d2, 0xc9e1, 0x4ce9, {0x9b, 0x85, 0xf5, 0xff, 0xad, 0xd7, 0x75, 0xf } }

typedef struct _BYO_PCD_ACCESS_PROTOCOL BYO_PCD_ACCESS_PROTOCOL;

typedef
BOOLEAN
(EFIAPI *BYO_OPAL_UPDATE_TABLE_PCD_GET) (
  );

typedef
BOOLEAN
(EFIAPI *BYO_AHCI_PORT_NAME_INDEX_PCD_GET) (
  );

typedef
UINT8
(EFIAPI *BYO_HDD_PASSWORD_MAX_LENGTH_PCD_GET) (
  );

typedef
UINT8
(EFIAPI *BYO_GRAPHICS_DELTAX_PCD_GET) (
  );

typedef
UINT8
(EFIAPI *BYO_PLATFORM_CMOS_BAD_PCD_GET) (
  );

#define BYO_PCD_ACCESS_PROTOCOL_SIGNATURE SIGNATURE_32 ('B', 'P', 'A', 'P')

struct _BYO_PCD_ACCESS_PROTOCOL {
  BYO_OPAL_UPDATE_TABLE_PCD_GET        PcdGetOpalUpdateDataStoreTable;
  BYO_AHCI_PORT_NAME_INDEX_PCD_GET     PcdGetAhciOrPortNameIndexBase0;
  BYO_HDD_PASSWORD_MAX_LENGTH_PCD_GET  PcdGetHddPasswordMaxLength;
  BYO_GRAPHICS_DELTAX_PCD_GET          PcdGetGraphicsConsoleDeltaXPersent;
  UINT32                               Signature;
  UINT32                               ApiNumber;
  BYO_PLATFORM_CMOS_BAD_PCD_GET        PcdGetIsPlatformCmosBad;
};

extern EFI_GUID gByoPcdAccessProtocolGuid;
#endif

