/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseCryptLib.h>
#include <Protocol/CryptoLibDxeProtocol.h>


  
  
BOOLEAN 
EFIAPI
Sha256 (
  IN      VOID     *DataIn, 
  IN      UINTN    DataSize, 
  IN OUT  UINT8    *HashValue
)
{
  UINTN       CtxSize;
  VOID        *Sha256Ctx = NULL;
  BOOLEAN     Rc = FALSE;

  
  CtxSize = Sha256GetContextSize();
  Sha256Ctx = AllocatePool(CtxSize);
  if(Sha256Ctx == NULL){
    goto ProcExit;
  }

  if (!Sha256Init(Sha256Ctx)) {
    goto ProcExit;
  }
  if (!Sha256Update(Sha256Ctx, DataIn, DataSize)) {
    goto ProcExit;
  }
  if (!Sha256Final (Sha256Ctx, HashValue)) {
    goto ProcExit;
  }

  Rc = TRUE;

ProcExit:
  if(Sha256Ctx != NULL){
    FreePool(Sha256Ctx);
  }
  return Rc;
}  
    
BOOLEAN
EFIAPI
LibRsaSetKey (
  IN OUT  VOID             *RsaContext,
  IN      LIB_RSA_KEY_TAG  LibKeyTag,
  IN      CONST UINT8      *BigNumber,
  IN      UINTN            BnSize
  )
{
  RSA_KEY_TAG  KeyTag;
  
  switch(LibKeyTag){
    case LibRsaKeyN:
      KeyTag = RsaKeyN;
      break;

    case LibRsaKeyE:
      KeyTag = RsaKeyE;
      break;

    case LibRsaKeyD:
      KeyTag = RsaKeyD;
      break; 

    default:
      return FALSE;    
  }
  
  return RsaSetKey(RsaContext, KeyTag, BigNumber, BnSize);
}


BOOLEAN
EFIAPI
DataRsaPkcs1Verify (
  IN VOID     *Data,
  IN UINTN    DataSize,
  IN VOID     *SignData,
  IN UINTN    SignDataSize,
  IN VOID     *KeyN,
  IN UINTN    KeyNSize,
  IN VOID     *KeyE,
  IN UINTN    KeyESize
  )
{
  VOID          *Rsa = NULL;
  UINT8         HashValue[SHA256_DIGEST_SIZE];
  BOOLEAN       Rc;

  
  Rc = Sha256(Data, DataSize, HashValue);
  if (!Rc) {
    goto ProcExit; 
  }

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

  if (!RsaPkcs1Verify (Rsa, HashValue, SHA256_DIGEST_SIZE, SignData, SignDataSize)) {
    Rc = FALSE;
    goto ProcExit;
  } 

  Rc = TRUE;

ProcExit:
  if(Rsa != NULL){RsaFree(Rsa);}
  return Rc;
}








CRYPTO_LIB_PROTOCOL gCryptoLibProtocol = {
  CRYPTO_LIB_VERSION,
  Sha256GetContextSize,
  Sha256Init,
  Sha256Update,
  Sha256Final,
  Sha256,
  RsaNew,
  LibRsaSetKey,
  RsaPkcs1Verify,
  RsaFree,
  DataRsaPkcs1Verify
};
  




