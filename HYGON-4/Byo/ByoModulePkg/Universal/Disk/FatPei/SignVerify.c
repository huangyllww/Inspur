/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <Guid/CapsuleRecord.h>
#include <Pi/PiHob.h>
#include <Pi/PiPeiCis.h>
#include <Pi/PiFirmwareFile.h>
#include <Ppi/FirmwareVolume.h>
#include <Library/BaseMemoryLib.h>
#include <Ppi/RecoveryModule.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Uefi/UefiBaseType.h>
#include <Guid/BiosIdInfo.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PcdLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Guid/FirmwareFileSystem2.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <RsaSignDataHob.h>
#include <Library/ByoCommLib.h>
#include <Library/BaseCryptLib.h>
#include <Library/ByoBiosSignInfoLib.h>
#include <Protocol/ByoSmiFlashProtocol.h>


BOOLEAN 
HashInit (
  IMAGE_DATA_CTX  *Ctx
  )
{
  UINTN  CtxSize;

  if(Ctx->HashType == SIGN_INIT_HASH_TYPE_SHA256){
    CtxSize = Sha256GetContextSize();
  } else if(Ctx->HashType == SIGN_INIT_HASH_TYPE_SHA512){
    CtxSize = Sha512GetContextSize();
  } else {
    DEBUG((EFI_D_INFO, "Unsupported hash type\n"));
    return FALSE;
  }
  
  Ctx->HashCtx = AllocatePool(CtxSize);
  if(Ctx->HashCtx == NULL){
    return FALSE;
  }

  if(Ctx->HashType == SIGN_INIT_HASH_TYPE_SHA256){
    return Sha256Init(Ctx->HashCtx);
  } else if(Ctx->HashType == SIGN_INIT_HASH_TYPE_SHA512){
    return Sha512Init(Ctx->HashCtx);
  } 

  return FALSE;
}


BOOLEAN 
HashFinal (
  IMAGE_DATA_CTX  *Ctx,
  UINT8           *HashData,
  UINT32          *HashDataSize
  )
{
  if(Ctx->HashType == SIGN_INIT_HASH_TYPE_SHA256){
    *HashDataSize = 32;
    return Sha256Final(Ctx->HashCtx, HashData);
  } else if(Ctx->HashType == SIGN_INIT_HASH_TYPE_SHA512){
    *HashDataSize = 64;  
    return Sha512Final(Ctx->HashCtx, HashData);
  } 

  return FALSE;
}


BOOLEAN
DoRsaPkcs1Verify (
  IN VOID     *HashData,
  IN UINTN    HashDataSize,
  IN VOID     *SignData,
  IN UINTN    SignDataSize,
  IN VOID     *KeyN,
  IN UINTN    KeyNSize,
  IN VOID     *KeyE,
  IN UINTN    KeyESize
  )
{
  VOID          *Rsa = NULL;
  BOOLEAN       Rc;
  

  Rsa = RsaNew();
  if (Rsa == NULL) {
    Rc = FALSE;
    goto ProcExit; 
  }
  if (!RsaSetKey (Rsa, RsaKeyN, KeyN, KeyNSize)) {
    Rc = FALSE;
    goto ProcExit; 
  }
  if (!RsaSetKey (Rsa, RsaKeyE, KeyE, KeyESize)) {
    Rc = FALSE;
    goto ProcExit; 
  }

  if (!RsaPkcs1Verify (Rsa, HashData, HashDataSize, SignData, SignDataSize)) {
    Rc = FALSE;
    goto ProcExit;
  } 

  Rc = TRUE;

ProcExit:
  if(Rsa != NULL){RsaFree(Rsa);}
  return Rc;
}


EFI_STATUS
EFIAPI
ByoPeiSignVerifyUpdate (
  IN  IMAGE_DATA_CTX                 *Ctx,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize 
  )
{
  BOOLEAN  rc;

  if(Ctx->SignMethod == SIGN_METHOD_FIXED_OFFSET || Ctx->SignMethod == SIGN_METHOD_INFO_IN_FV){
    if(Offset == Ctx->SignFvOffset && DataSize == SIZE_4KB){
      return EFI_SUCCESS;
    }
  }

  if(Ctx->HashType == SIGN_INIT_HASH_TYPE_SHA256){
    rc = Sha256Update(Ctx->HashCtx, Data, DataSize);
  } else if(Ctx->HashType == SIGN_INIT_HASH_TYPE_SHA512){
    rc = Sha512Update(Ctx->HashCtx, Data, DataSize);
  } else {
    rc = FALSE;
  }

  if(rc){
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  }
}


BOOLEAN 
EFIAPI
VerifyBiosSign(
  VOID      *ImageData, 
  UINTN     ImageSize
  )
{
  EFI_PEI_HOB_POINTERS          GuidHob;
  PK_RSA2048_HOB_DATA           *PubKey;
  UINT32                        Crc32;
  BOOLEAN                       Rc;  
  EFI_STATUS                    Status;
  IMAGE_DATA_CTX                CtxData;
  IMAGE_DATA_CTX                *Ctx = &CtxData; 
  UINT32                        HashSize;
  UINT8                         Hash[64];
  

  DEBUG((EFI_D_INFO, "VerifyBiosSign\n"));

  Status = ParseByoSignInfo((UINT8*)ImageData, ImageSize, Ctx);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "ParseByoSignInfo error:%r\n", Status));
    return FALSE;
  }

  if(!HashInit(Ctx)){
    DEBUG((EFI_D_ERROR, "HashInit error\n"));
    return FALSE;
  }

  Status = DoByoSignVerifyUpdate(Ctx, ByoPeiSignVerifyUpdate, NULL);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "DoByoSignVerifyUpdate error:%r\n", Status));
    return FALSE;
  }

  if(!HashFinal(Ctx, Hash, &HashSize)){
    DEBUG((EFI_D_ERROR, "HashFinal error\n"));
    return FALSE;
  }
 
  GuidHob.Raw = GetFirstGuidHob(&gByoBiosSignPubKeyFileGuid);
  if(GuidHob.Raw == NULL){
    DEBUG((EFI_D_ERROR, "PubKey not Found\n"));     
    return FALSE;   
  }
  PubKey = GET_GUID_HOB_DATA(GuidHob.Guid);
  Status = LibCalcCrc32(PubKey->KeyN, PubKey->Hdr.KeySize, &Crc32);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  if(Crc32 != PubKey->Hdr.Crc32){
    DEBUG((EFI_D_ERROR, "PubKey Crc Err\n"));    
    return FALSE; 
  }

  Rc = DoRsaPkcs1Verify (
         Hash, 
         HashSize,
         Ctx->SignData, 
         Ctx->SignSize,
         PubKey->KeyN, 
         sizeof(PubKey->KeyN),
         PubKey->KeyE, 
         sizeof(PubKey->KeyE)
         );
  DEBUG((EFI_D_INFO, "DataRsaPkcs1Verify %d\n", Rc));  
  if (Rc) {
    return TRUE;
  } 

  return FALSE;
}


