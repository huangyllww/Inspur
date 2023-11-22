/** @file

  Termial Data Backup protocol
  This protocol uses File Device Path to get an EFI image.

Copyright (c) 2006 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __TERMINAL_DATA_BACKUP_PROTOCOL_H__
#define __TERMINAL_DATA_BACKUP_PROTOCOL_H__

#define TERMINAL_DATA_BACKUP_PROTOCOL_GUID  \
  { 0x9fbb6a01, 0x0109, 0xe01c, { 0xa8, 0x2f, 0x0f, 0x5d, 0x26, 0x97, 0x03, 0x7e }}

typedef struct _TERMINAL_DATA_BACKUP_PROTOCOL   TERMINAL_DATA_BACKUP_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *GET_TERMINAL_BACKUP_DATA)(
  IN  TERMINAL_DATA_BACKUP_PROTOCOL          *This,
  IN  UINTN                                  X,
  IN  UINTN                                  Y,
  IN  UINTN                                  Width,
  IN  UINTN                                  Height,
  OUT CHAR16                                 **WString,
  OUT UINTN                                  **Attribute
  );

struct _TERMINAL_DATA_BACKUP_PROTOCOL {
  GET_TERMINAL_BACKUP_DATA     GetData;
};

extern EFI_GUID gTerminalDataBackupProtocolGuid;

#endif

