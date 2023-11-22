/** @file

Copyright (c) 2006 - 2021 Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By:
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#ifndef __BYO_BIOS_SIGN_INFO_LIB_H__
#define __BYO_BIOS_SIGN_INFO_LIB_H__

#include <Uefi.h>
#include <ByoSmiFlashInfo.h>



#define SIGN_METHOD_SIG_ONLY            0
#define SIGN_METHOD_IMAGE_APPEND_SIG    1
#define SIGN_METHOD_FIXED_OFFSET        2
#define SIGN_METHOD_INFO_IN_FV          3
#define SIGN_METHOD_UNKNOWN             0xFF

#define BIOS_SIGN_INFO_FIX_OFFSET           0x50


#pragma pack(1)

typedef struct {
  UINT32  Offset;
  UINT32  Length;
} SIGN_AREA_RANGE;

typedef struct {
  EFI_GUID          CapsuleGuid;
  UINT32            HeaderSize;
  UINT32            Flags;
  UINT32            CapsuleImageSize;
} BYO_EFI_CAPSULE_HEADER;

typedef struct {
  BYO_EFI_CAPSULE_HEADER  Hdr;
  UINT32                  PubDerKeySize;
  UINT32                  SignSize;
  UINT32                  RangeArraySize;
  UINT8                   Pubkey[1];
} BYO_EFI_CAPSULE_DATA;

#pragma pack()

extern EFI_GUID gByoBiosCapsuleGuid;


typedef struct {
  UINT8            *ImageData;
  UINTN            ImageSize;
  UINT8            *SignData;
  UINTN            SignSize;
  UINT32           HashType;
  SIGN_AREA_RANGE  Temp;           // for all image data hash
  SIGN_AREA_RANGE  *Range;
  UINT32           RangeCount;
  UINTN            TotalSignedSize;
  VOID             *HashCtx;
  BYO_EFI_CAPSULE_DATA  *CapData;
  UINT8                 SignMethod;
  UINT32                SignFvOffset;
} IMAGE_DATA_CTX;


EFI_STATUS
ParseByoSignInfo (
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize,
  OUT IMAGE_DATA_CTX                 *Ctx
  );


typedef
VOID
(EFIAPI *BIOS_SIGN_VERIFY_PROGRESS_CALLBACK)(
  IN UINTN     Persent
  );

typedef
EFI_STATUS
(EFIAPI *BIOS_SIGN_VERIFY_ACTION) (
  IN  IMAGE_DATA_CTX                 *Ctx,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize 
  );

EFI_STATUS
EFIAPI
DoByoSignVerifyUpdate (
  IN  IMAGE_DATA_CTX                      *Ctx,
  IN  BIOS_SIGN_VERIFY_ACTION             Action,
  IN  BIOS_SIGN_VERIFY_PROGRESS_CALLBACK  CallBack
  );



#endif
