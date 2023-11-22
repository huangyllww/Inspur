/** @file
  The DisplayBackupRecoverProtocol Protocol defines the interface to backup and recover of seccen.

Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#ifndef __DISPLAY_BACKUP_RECOVER_H__
#define __DISPLAY_BACKUP_RECOVER_H__

#include <Protocol/GraphicsOutput.h>

//
// GUID for DisplayBackupRecoverProtocolProtocol
//
#define BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL_GUID \
  { 0xc2c32ed2, 0x5346, 0x473f, { 0xbf, 0xe2, 0x46, 0x45, 0x89, 0x48, 0xe2, 0x2a } }

typedef struct _BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL;

typedef struct  {
  //
  // ConOut Data
  //
  EFI_SIMPLE_TEXT_OUTPUT_MODE             ConOutMode;
  UINTN                                   ConOutColumn;
  UINTN                                   ConOutRow;

  //
  // Gop Data
  //
  EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE       GopMode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION    GopInfo;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL           *GopData;
  CHAR16                                  *SerialPortStringData;
  UINTN                                   *SerialPortAttributeData;

} BACKUP_RECOVER_DATA;

/**
  Display Backup.

  @param Data              Return Display Data

  @retval EFI_SUCCESS      The backup successfully.
  @retval Other            The backup fail.
**/
typedef
EFI_STATUS
(EFIAPI *BYO_DISPLAY_BACKUP)(
  OUT BACKUP_RECOVER_DATA          **Data
  );

/**
  Display Recover.

  @param Data              Input Display Data

  @retval EFI_SUCCESS      The recover successfully.
  @retval Other            The recover fail.
**/
typedef
EFI_STATUS
(EFIAPI *BYO_DISPLAY_RECOVER)(
  IN BACKUP_RECOVER_DATA          *Data
  );

#define BYO_DISPLAY_RECOVER_ATTRIBUTE_VIDEO            (BIT1)
#define BYO_DISPLAY_RECOVER_ATTRIBUTE_SERIAL_PORT      (BIT2)
#define BYO_DISPLAY_RECOVER_ATTRIBUTE_ALL              (0xFFFFFFFF)
#define BYO_DISPLAY_RECOVER_ATTRIBUTE_NO_VIDEO         (0xFFFFFFFF & (~BIT1))

/**
  Display Recover.

  @param Data              Input Display Data

  @retval EFI_SUCCESS      The recover successfully.
  @retval Other            The recover fail.
**/
typedef
EFI_STATUS
(EFIAPI *BYO_DISPLAY_RECOVER_EX)(
  IN BACKUP_RECOVER_DATA          *Data,
  IN UINT32                       Attribute
  );

struct _BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL {
  BYO_DISPLAY_BACKUP        DispalyBackup;
  BYO_DISPLAY_RECOVER       DispalyRecover;
  BYO_DISPLAY_RECOVER_EX    DispalyRecoverEx;
};

extern EFI_GUID  gDisplayBackupRecoverProtocolGuid;

#endif
