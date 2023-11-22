/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _GENERIC_ELOG_H_
#define _GENERIC_ELOG_H_

#define EFI_SM_ELOG_PROTOCOL_GUID \
  { \
    0x59d02fcd, 0x9233, 0x4d34, 0xbc, 0xfe, 0x87, 0xca, 0x81, 0xd3, 0xdd, 0xa7 \
  }

#define EFI_SM_ELOG_REDIR_PROTOCOL_GUID \
  { \
    0x16d11030, 0x71ba, 0x4e5e, 0xa9, 0xf9, 0xb4, 0x75, 0xa5, 0x49, 0x4, 0x8a \
  }

typedef struct _EFI_SM_ELOG_PROTOCOL EFI_SM_ELOG_PROTOCOL;

typedef struct _EFI_SM_ELOG_PROTOCOL EFI_SM_ELOG_REDIR_PROTOCOL;

//
// Common Defines
//
typedef enum {
  EfiElogSmSMBIOS,
  EfiElogSmIPMI,
  EfiElogSmMachineCritical,
  EfiElogSmASF,
  EfiElogSmOEM,
  EfiSmElogMax
} EFI_SM_ELOG_TYPE;

//
//  Generic ELOG Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_SET_ELOG_DATA) (
  IN EFI_SM_ELOG_PROTOCOL               * This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  BOOLEAN                           AlertEvent,
  IN  UINTN                             DataSize,
  OUT UINT64                            *RecordId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_ELOG_DATA) (
  IN EFI_SM_ELOG_PROTOCOL               * This,
  IN  UINT8                             *ElogData,
  IN  EFI_SM_ELOG_TYPE                  DataType,
  IN  OUT UINTN                         *DataSize,
  IN OUT UINT64                         *RecordId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_ERASE_ELOG_DATA) (
  IN EFI_SM_ELOG_PROTOCOL               * This,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN OUT UINT64                         *RecordId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVATE_ELOG) (
  IN EFI_SM_ELOG_PROTOCOL               * This,
  IN EFI_SM_ELOG_TYPE                   DataType,
  IN BOOLEAN                            *EnableElog,
  OUT BOOLEAN                           *ElogStatus
  );

//
// IPMI TRANSPORT PROTOCOL
//
struct _EFI_SM_ELOG_PROTOCOL {
  EFI_SET_ELOG_DATA   SetEventLogData;
  EFI_GET_ELOG_DATA   GetEventLogData;
  EFI_ERASE_ELOG_DATA EraseEventlogData;
  EFI_ACTIVATE_ELOG   ActivateEventLog;
} ;

extern EFI_GUID gEfiGenericElogProtocolGuid;
extern EFI_GUID gEfiRedirElogProtocolGuid;
extern EFI_GUID gSmmGenericElogProtocolGuid;
extern EFI_GUID gSmmRedirElogProtocolGuid;

#endif
