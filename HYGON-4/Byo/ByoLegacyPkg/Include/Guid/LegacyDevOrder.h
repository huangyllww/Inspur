/** @file
  Guid of a NV Variable which store the information about the
  FD/HD/CD/NET/BEV order.

Copyright (c) 2011 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __LEGACY_DEV_ORDER_VARIABLE_GUID_H__
#define __LEGACY_DEV_ORDER_VARIABLE_GUID_H__

///
/// Name and Guid of a NV Variable which stores the information about the
/// FD/HD/CD/NET/BEV order
///
#define EFI_LEGACY_DEV_ORDER_VARIABLE_GUID \
  { \
  0xa56074db, 0x65fe, 0x45f7, {0xbd, 0x21, 0x2d, 0x2b, 0xdd, 0x8e, 0x96, 0x52} \
  }

typedef UINT8 BBS_TYPE;

#pragma pack(1)
typedef struct {
  BBS_TYPE  BbsType;
  ///
  /// Length = sizeof (UINT16) + sizeof (Data)
  ///
  UINT16    Length;
  UINT16    Data[1];
} LEGACY_DEV_ORDER_ENTRY;
#pragma pack()

#define VAR_LEGACY_DEV_ORDER L"LegacyDevOrder"

extern EFI_GUID gEfiLegacyDevOrderVariableGuid;

#endif
