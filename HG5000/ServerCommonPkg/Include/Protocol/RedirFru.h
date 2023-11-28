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

#ifndef _REDIR_FRU_H_
#define _REDIR_FRU_H_

#define EFI_SM_FRU_REDIR_PROTOCOL_GUID \
  { \
    0x28638cfa, 0xea88, 0x456c, 0x92, 0xa5, 0xf2, 0x49, 0xca, 0x48, 0x85, 0x35 \
  }

#define EFI_SM_FRU_REDIR_SIGNATURE  SIGNATURE_32 ('f', 'r', 'r', 'x')

typedef struct _EFI_SM_FRU_REDIR_PROTOCOL EFI_SM_FRU_REDIR_PROTOCOL;


#pragma pack(1)
typedef struct {
  BOOLEAN           Valid;
  EFI_FRU_DATA_INFO FruDevice;
} EFI_FRU_DEVICE_INFO;


typedef struct {
  UINT8 FormatVersionNumber : 4;
  UINT8 Reserved : 4;
  UINT8 InternalUseStartingOffset;
  UINT8 ChassisInfoStartingOffset;
  UINT8 BoardAreaStartingOffset;
  UINT8 ProductInfoStartingOffset;
  UINT8 MultiRecInfoStartingOffset;
  UINT8 Pad;
  UINT8 Checksum;
} IPMI_FRU_COMMON_HEADER;

typedef struct {
  UINT8       RecordTypeId;
  UINT8       RecordFormatVersion:4;
  UINT8       Reserved:3;
  UINT8       EndofList:1;
  UINT8       RecordLength;
  UINT8       RecordChecksum;
  UINT8       HeaderChecksum;
} IPMI_FRU_MULTI_RECORD_HEADER;

typedef struct {
  UINT8       RecordCheckSum;
  UINT8       SubRecordId;
  EFI_GUID    Uuid;
} IPMI_SYSTEM_UUID_SUB_RECORD;

#pragma pack()


//
//  Redir FRU Function Prototypes
//
typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_REDIR_INFO) (
  IN EFI_SM_FRU_REDIR_PROTOCOL            * This,
  IN  UINTN                               FruSlotNumber,
  OUT EFI_GUID                            * FruFormatGuid,
  OUT UINTN                               *DataAccessGranularity,
  OUT CHAR16                              **FruInformationString
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_SLOT_INFO) (
  IN EFI_SM_FRU_REDIR_PROTOCOL            * This,
  OUT EFI_GUID                            * FruTypeGuid,
  OUT UINTN                               *StartFruSlotNumber,
  OUT UINTN                               *NumSlots
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FRU_REDIR_DATA) (
  IN EFI_SM_FRU_REDIR_PROTOCOL            * This,
  IN  UINTN                               FruSlotNumber,
  IN  UINTN                               FruDataOffset,
  IN  UINTN                               FruDataSize,
  IN  UINT8                               *FruData
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_FRU_REDIR_DATA) (
  IN EFI_SM_FRU_REDIR_PROTOCOL            * This,
  IN  UINTN                               FruSlotNumber,
  IN  UINTN                               FruDataOffset,
  IN  UINTN                               FruDataSize,
  IN  UINT8                               *FruData
  );

//
// REDIR FRU PROTOCOL
//
typedef struct _EFI_SM_FRU_REDIR_PROTOCOL {
  EFI_GET_FRU_REDIR_INFO  GetFruRedirInfo;
  EFI_GET_FRU_SLOT_INFO   GetFruSlotInfo;
  EFI_GET_FRU_REDIR_DATA  GetFruRedirData;
  EFI_SET_FRU_REDIR_DATA  SetFruRedirData;
} EFI_SM_FRU_REDIR_PROTOCOL;

extern EFI_GUID gEfiRedirFruProtocolGuid;

#endif
