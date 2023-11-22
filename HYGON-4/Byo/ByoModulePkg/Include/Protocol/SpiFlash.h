/** @file

Copyright (c) 2010 - 2020, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  SpiFlash.h

Abstract: 

Revision History:

**/
/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/

#ifndef _EFI_SPI_FLASH_H_
#define _EFI_SPI_FLASH_H_

//
// Define the SPI protocol GUID
//
#define EFI_SPI_FLASH_PROTOCOL_GUID \
  { \
    0x1156efc6, 0xea32, 0x4396, 0xb5, 0xd5, 0x26, 0x93, 0x2e, 0x83, 0xc3, 0x13 \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID                   gEfiSpiFlashProtocolGuid;

typedef enum {
  VERIFY_INIT = 1,
  VERIFY_UPDATE,
  VERIFY_FINAL
} BIOS_VERIFY_SUBFUN;

#pragma pack(1)

typedef struct {
  UINT32  Offset;
  UINT32  Length;
} SIGN_RANGE;

typedef struct {
  UINT8                SubFun;
  UINTN                Buffer;
  UINT32               Size;
} BIOS_VERIFY_PARAMETER;

/*
EFI_CAPSULE_HEADER
  CapsuleGuid         // 16
  HeaderSize          // 4
  Flags               // 4
  CapsuleImageSize    // 4
-------------------------------------------------------------
  (+) PubkeySize      // 4            +28
  (+) SignSize        // 4            +32
  (+) RangeArraySize  // 4            +36

  (+) Pubkey          //              +40
  (+) Sign            //              +40+PubkeySize            align 4
  (+) Range[]         //              +40+PubkeySize+SignSize   align 4
-------------------------------------------------------------
FD                    // 16 align
*/
typedef struct {
  EFI_CAPSULE_HEADER   Header;
  UINT32               PubkeySize;
  UINT32               SignSize;
  UINT32               RangeArraySize;
  UINT8                Data[1];
} BIOS_VERIFY_CAPSULE_BUFFER;
#pragma pack()

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_SPI_FLASH_PROTOCOL  EFI_SPI_FLASH_PROTOCOL;

typedef 
EFI_STATUS
(EFIAPI *SPI_FLASH_ERASE_INTERFACE) (
  IN EFI_SPI_FLASH_PROTOCOL  *This,
  IN UINT64                  Offset,
  IN UINT64                  Length
  );

typedef 
EFI_STATUS
(EFIAPI *SPI_FLASH_WRITE_INTERFACE) (
  IN  EFI_SPI_FLASH_PROTOCOL *This,
  IN  UINT64                 Offset,
  IN  UINT8                  *Buffer,
  IN  UINT64                 Length
  );

typedef 
EFI_STATUS
(EFIAPI *SPI_FLASH_READ_INTERFACE) (
  IN EFI_SPI_FLASH_PROTOCOL  *This,
  IN UINT64                  Offset,
  IN OUT UINT8               *Buffer,
  IN UINT64                  Length
  );

typedef 
EFI_STATUS
(EFIAPI *SPI_FLASH_ERASE_WRITE_INTERFACE) (
  IN  EFI_SPI_FLASH_PROTOCOL  *This,
  IN  UINT64                  Offset,
  IN  UINT8                   *Buffer,
  IN  UINT64                  Length
  );

typedef
EFI_STATUS
(EFIAPI *SPI_FLASH_DEFAULT_UPDATE)(
  IN  EFI_SPI_FLASH_PROTOCOL         *This,
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize
  ); 

typedef
EFI_STATUS
(EFIAPI *SPI_FLASH_VERIFY_SIGN)(
  IN  EFI_SPI_FLASH_PROTOCOL         *This,
  IN  BIOS_VERIFY_PARAMETER          *BiosVerifyPtr
  );

typedef
EFI_STATUS
(EFIAPI *SPI_FLASH_BIOS_CHECK)(
  IN  EFI_SPI_FLASH_PROTOCOL          *This,
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize
  );


struct _EFI_SPI_FLASH_PROTOCOL {
  SPI_FLASH_ERASE_INTERFACE             Erase;
  SPI_FLASH_WRITE_INTERFACE             Write;
  SPI_FLASH_READ_INTERFACE              Read;
  SPI_FLASH_ERASE_WRITE_INTERFACE       EraseWrite;
  SPI_FLASH_DEFAULT_UPDATE              DefaultUpdate;
  SPI_FLASH_VERIFY_SIGN                 VerifySign;
  SPI_FLASH_BIOS_CHECK                  BiosCheck;
};


#endif
