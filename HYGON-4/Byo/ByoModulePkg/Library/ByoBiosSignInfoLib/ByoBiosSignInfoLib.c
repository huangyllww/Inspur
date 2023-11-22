/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
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

#include <Library/ByoBiosSignInfoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <Pi/PiFirmwareVolume.h>
#include <Pi/PiFirmwareFile.h>



// {9FB77E3F-60E1-4256-B3AA-DE2B5331165F}
static EFI_GUID gEfiBiosSignFvGuid =
  { 0x9fb77e3f, 0x60e1, 0x4256, { 0xb3, 0xaa, 0xde, 0x2b, 0x53, 0x31, 0x16, 0x5f }};

EFI_STATUS 
GetSignDataFromBiosFile (
  IN  UINT8                   *BiosData,
  IN  UINTN                   BiosSize,
  OUT BYO_EFI_CAPSULE_DATA    **pCapData,
  OUT UINT32                  *FvOffset
  )
{
  UINTN                           Index;
  UINTN                           End;
  UINT32                          FvSize;
  EFI_FIRMWARE_VOLUME_HEADER      *FvHdr;
  EFI_FIRMWARE_VOLUME_EXT_HEADER *FwVolExHeaderInfo;
  EFI_FFS_FILE_HEADER            *FfsHeader;
  EFI_COMMON_SECTION_HEADER      *Section;
  UINT32                         SignFileOffset;
  BYO_EFI_CAPSULE_DATA           *CapData;


  Index = 0;
  End   = BiosSize - sizeof(EFI_FIRMWARE_VOLUME_HEADER);
  while(Index < End) {
    FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)(BiosData + Index);
    if(FvHdr->Signature == EFI_FVH_SIGNATURE && CalculateSum16((UINT16*)FvHdr, FvHdr->HeaderLength) == 0) {
      FvSize = (UINT32)FvHdr->FvLength;
      if (FvHdr->ExtHeaderOffset != 0) {
        FwVolExHeaderInfo = (EFI_FIRMWARE_VOLUME_EXT_HEADER*)(((UINT8 *)FvHdr) + FvHdr->ExtHeaderOffset);
        if(CompareMem(&gEfiBiosSignFvGuid, &FwVolExHeaderInfo->FvName, sizeof(EFI_GUID)) == 0){
          FfsHeader = (EFI_FFS_FILE_HEADER *) ((UINT8 *) FwVolExHeaderInfo + FwVolExHeaderInfo->ExtHeaderSize);
          FfsHeader = (EFI_FFS_FILE_HEADER *) ALIGN_POINTER (FfsHeader, 8);
          Section   = (EFI_COMMON_SECTION_HEADER *)(FfsHeader + 1);
          SignFileOffset = (UINT32)(((UINTN)(Section + 1) - (UINTN)BiosData));
          if(SignFileOffset % 16){
            SignFileOffset = ALIGN_VALUE(SignFileOffset, 16);
          }
          CapData = (BYO_EFI_CAPSULE_DATA*)(BiosData + SignFileOffset);
          if(CompareMem(&gByoBiosCapsuleGuid, &CapData->Hdr.CapsuleGuid, sizeof(EFI_GUID)) == 0){
            *FvOffset = (UINT32)Index;
            *pCapData = CapData;
            return EFI_SUCCESS;              
          }
        }
      }
      Index += FvSize;
    } else {
      Index += SIZE_4KB;
    }
  }

  return EFI_NOT_FOUND;
}



UINT8
GetBiosSignType (
  IN     UINT8                          *NewBiosData,
  IN     UINTN                          NewBiosDataSize,
  IN OUT IMAGE_DATA_CTX                 *Ctx
  )
{
  UINTN                 SignFileSize;
  BYO_EFI_CAPSULE_DATA  *CapData;
  EFI_STATUS            Status;
  UINT32                FvOffset;
  

  SignFileSize = NewBiosDataSize % SIZE_4KB;
  if(SignFileSize == 256){
    Ctx->SignMethod = SIGN_METHOD_IMAGE_APPEND_SIG;      
    return SIGN_METHOD_IMAGE_APPEND_SIG;
  } else if(SignFileSize == 0){
    CapData = (BYO_EFI_CAPSULE_DATA*)(NewBiosData + 0x50);
    if(CompareMem(&gByoBiosCapsuleGuid, &CapData->Hdr.CapsuleGuid, sizeof(EFI_GUID)) == 0){ 
      Ctx->CapData = CapData;
      Ctx->SignFvOffset = 0;
      Ctx->SignMethod = SIGN_METHOD_FIXED_OFFSET;
      return SIGN_METHOD_FIXED_OFFSET;
    }   
    Status = GetSignDataFromBiosFile(NewBiosData, NewBiosDataSize, &CapData, &FvOffset);
    if(!EFI_ERROR(Status)){
      Ctx->CapData = CapData;
      Ctx->SignFvOffset = FvOffset; 
      Ctx->SignMethod = SIGN_METHOD_INFO_IN_FV;      
      return SIGN_METHOD_INFO_IN_FV;
    }
  }

  return SIGN_METHOD_UNKNOWN;
}


EFI_STATUS
ParseByoSignInfo (
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize,
  OUT IMAGE_DATA_CTX                 *Ctx
  )
{
  BYO_EFI_CAPSULE_DATA  *CapData;
  UINT8                 SignMethod;
  UINTN                 Index;
  

  SignMethod = GetBiosSignType(NewBiosData, NewBiosDataSize, Ctx);
  if(SignMethod == SIGN_METHOD_UNKNOWN){
    return EFI_INVALID_PARAMETER;
  }

  Ctx->ImageData = NewBiosData;
  Ctx->ImageSize = NewBiosDataSize;
  Ctx->HashType  = SIGN_INIT_HASH_TYPE_SHA256;

  switch(SignMethod){
    case SIGN_METHOD_FIXED_OFFSET:
    case SIGN_METHOD_INFO_IN_FV:
      CapData       = Ctx->CapData;
      Ctx->SignData = CapData->Pubkey + ALIGN_VALUE(CapData->PubDerKeySize, 4);
      Ctx->SignSize = CapData->SignSize;
      Ctx->Range    = (SIGN_AREA_RANGE*)(Ctx->SignData + ALIGN_VALUE(CapData->SignSize, 4));
      Ctx->RangeCount = CapData->RangeArraySize / sizeof(SIGN_AREA_RANGE);
      Ctx->TotalSignedSize = 0;
      for(Index=0;Index<Ctx->RangeCount;Index++){
        Ctx->TotalSignedSize += Ctx->Range[Index].Length;
      }
      break;
      
    case SIGN_METHOD_IMAGE_APPEND_SIG:
      Ctx->SignSize = Ctx->ImageSize % SIZE_4KB;
      Ctx->Range = &Ctx->Temp;
      Ctx->Range[0].Offset = 0;
      Ctx->Range[0].Length = (UINT32)(Ctx->ImageSize - Ctx->SignSize);
      Ctx->RangeCount = 1;
      Ctx->TotalSignedSize = Ctx->Range[0].Length;
      Ctx->SignData = Ctx->ImageData + Ctx->Range[0].Length;
      break;

    default:
      return EFI_INVALID_PARAMETER;
  } 

  if(Ctx->TotalSignedSize == 0){
    DEBUG((EFI_D_ERROR, "bad Total Signed Size 0\n"));
    return EFI_INVALID_PARAMETER;
  } 
  return EFI_SUCCESS;
}



EFI_STATUS
DoByoSignVerifyUpdate (
  IN  IMAGE_DATA_CTX                      *Ctx,
  IN  BIOS_SIGN_VERIFY_ACTION             Action,
  IN  BIOS_SIGN_VERIFY_PROGRESS_CALLBACK  CallBack
  )
{
  UINT32       i, j, k;
  EFI_STATUS   Status  = EFI_SUCCESS;
  UINTN        CurSize = 0;


  if(Ctx == NULL || Action == NULL){
    return EFI_INVALID_PARAMETER;
  }

  switch(Ctx->SignMethod){
    case SIGN_METHOD_FIXED_OFFSET:
      Status = Action(Ctx, 0, Ctx->ImageData, SIZE_4KB);
      break;
      
    case SIGN_METHOD_INFO_IN_FV:
      Status = Action(Ctx, Ctx->SignFvOffset, Ctx->ImageData + Ctx->SignFvOffset, SIZE_4KB);      
      break;

    default:
      break;
  }  
  if(EFI_ERROR(Status)){
    return Status;
  }

  for(j=0;j<Ctx->RangeCount;j++){
    k = Ctx->Range[j].Offset + Ctx->Range[j].Length;
    for(i=Ctx->Range[j].Offset; i<k; i+=SIZE_4KB){
      Status = Action (
                 Ctx,
                 i, 
                 Ctx->ImageData + i, 
                 SIZE_4KB
                 );
      if(!EFI_ERROR(Status)){
        CurSize += SIZE_4KB;
        if(CallBack != NULL){
          CallBack(CurSize*100/Ctx->TotalSignedSize);
        }
      } else {
        return Status;
      }
    }
  }

  return Status;
}


