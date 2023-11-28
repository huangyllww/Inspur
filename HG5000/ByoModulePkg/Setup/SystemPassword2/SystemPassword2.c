/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPassword.c

Abstract:
  Implementation of basic setup password function.

Revision History:

**/
#include <Pi/PiBootMode.h>
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/PrintLib.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SimpleTextInEx.h>
#include <SystemPassword2Variable.h>
#include <Protocol/SystemPassword2Protocol.h>
#include <Protocol/CryptoLibDxeProtocol.h>



STATIC SYSTEM_PASSWORD2         gSetupPassword2;
//STATIC SYSTEM_PASSWORD2         gSetupPassword2Default;
STATIC CRYPTO_LIB_PROTOCOL      *gCryptoDxe;
STATIC BOOLEAN                  gPasswordChanged = FALSE;
STATIC EFI_BOOT_MODE            gBootMode;


BOOLEAN
BeHavePassword (
  IN  PASSWORD_TYPE    Type
  )
{
  SYSTEM_PASSWORD2   *SetupPassword2 = &gSetupPassword2;

  if(gBootMode == BOOT_IN_RECOVERY_MODE){
    return FALSE;
  }

  switch (Type) {
    case PD_ADMIN:
      return SetupPassword2->bHaveAdmin;
      
    case PD_POWER_ON:
      return SetupPassword2->bHavePowerOn;

    default:
      return FALSE;
  }  
}


// follow tcm_hmac()
BOOLEAN HashDataSalt(VOID *Password, UINTN PasswordSize, UINT8 *Hash)
{
  VOID               *Salt;  
  UINTN              SaltSize;
  UINTN              Index;
  UINTN              CtxSize;
  VOID               *Sha256Ctx = NULL;  
  UINT8              MyHash[32];
  UINT8              Temp[64];
  UINT8              Temp2[64];
  

  if(PasswordSize == 0){
    return FALSE;
  }

  Salt = (VOID*)PcdGetPtr(PcdSysPasswordSalt);
  SaltSize = PcdGetSize(PcdSysPasswordSalt);
  ASSERT(SaltSize != 0);

  ZeroMem(Temp, sizeof(Temp));
  if(SaltSize > sizeof(Temp)){
    gCryptoDxe->Sha256(Salt, SaltSize, MyHash);
    CopyMem(Temp, MyHash, 32);
  } else {
    CopyMem(Temp, Salt, SaltSize);
  }
  CopyMem(Temp2, Temp, sizeof(Temp));

	for (Index=0; Index<sizeof(Temp); Index++){
		Temp[Index]  ^= 0x36;
		Temp2[Index] ^= 0x5C;
	}

  CtxSize = gCryptoDxe->Sha256GetContextSize();
  Sha256Ctx = AllocatePool(CtxSize);
  ASSERT(Sha256Ctx != NULL);
  
  gCryptoDxe->Sha256Init(Sha256Ctx);
  gCryptoDxe->Sha256Update(Sha256Ctx, Temp, sizeof(Temp));
  gCryptoDxe->Sha256Update(Sha256Ctx, Password, PasswordSize);
  gCryptoDxe->Sha256Final (Sha256Ctx, MyHash);

  gCryptoDxe->Sha256Init(Sha256Ctx);
  gCryptoDxe->Sha256Update(Sha256Ctx, Temp2, sizeof(Temp2));
  gCryptoDxe->Sha256Update(Sha256Ctx, MyHash, 32);
  gCryptoDxe->Sha256Final (Sha256Ctx, Hash);
  
  FreePool(Sha256Ctx);

  return TRUE;

}



EFI_STATUS
WritePassword (
  IN  PASSWORD_TYPE    Type,
  IN  CHAR16           *Password
  )
{
  SYSTEM_PASSWORD2   *SetupPassword2 = &gSetupPassword2;
  UINTN              PwLen;
  UINT8              Hash[32];


  ASSERT(SYSTEM_PASSWORD_HASH_LENGTH <= 32);

  PwLen = StrLen(Password);
  
  switch (Type) {
    case PD_ADMIN:
      if (PwLen) {
        HashDataSalt(Password, StrSize(Password), Hash);
        CopyMem(SetupPassword2->AdminHash, Hash, SYSTEM_PASSWORD_HASH_LENGTH);
        SetupPassword2->bHaveAdmin = TRUE;
      } else {
        ZeroMem(SetupPassword2->AdminHash, SYSTEM_PASSWORD_HASH_LENGTH);
        SetupPassword2->bHaveAdmin = FALSE;
      }
      gPasswordChanged = TRUE;
      return EFI_SUCCESS;

    case PD_POWER_ON:
      if (PwLen) {
        HashDataSalt(Password, StrSize(Password), Hash);
        CopyMem(SetupPassword2->PowerOnHash, Hash, SYSTEM_PASSWORD_HASH_LENGTH);
        SetupPassword2->bHavePowerOn = TRUE;
      } else {
        ZeroMem(SetupPassword2->PowerOnHash, SYSTEM_PASSWORD_HASH_LENGTH);
        SetupPassword2->bHavePowerOn = FALSE;
      }
      gPasswordChanged = TRUE;
      return EFI_SUCCESS;

    default:
      return EFI_INVALID_PARAMETER;
  }

}


BOOLEAN
VerifyPassword (
  IN  PASSWORD_TYPE    Type,
  IN  CHAR16           *Password
  )
{
  SYSTEM_PASSWORD2    *SetupPassword2 = &gSetupPassword2;
  UINT8               Hash[32];


  ASSERT(SYSTEM_PASSWORD_HASH_LENGTH <= 32);

  switch (Type) {
    case PD_ADMIN:
      if(!SetupPassword2->bHaveAdmin || StrLen(Password) == 0){
        return FALSE;
      } else {
        HashDataSalt(Password, StrSize(Password), Hash);
        if(CompareMem(Hash, SetupPassword2->AdminHash, SYSTEM_PASSWORD_HASH_LENGTH) == 0){
          return TRUE;
        } else {
          return FALSE;
        }
      }
      break;

    case PD_POWER_ON:
      if(!SetupPassword2->bHavePowerOn || StrLen(Password) == 0){
        return FALSE;
      } else {
        HashDataSalt(Password, StrSize(Password), Hash);
        if(CompareMem(Hash, SetupPassword2->PowerOnHash, SYSTEM_PASSWORD_HASH_LENGTH) == 0){
          return TRUE;
        } else {
          return FALSE;
        }
      }
      break;

    default:
      return FALSE;      
  }

}


BOOLEAN
IsPasswordEqual (
    VOID
  )
{
  SYSTEM_PASSWORD2    *SetupPassword2 = &gSetupPassword2;

  if(!(SetupPassword2->bHaveAdmin && SetupPassword2->bHavePowerOn)){
    return FALSE;
  }

  if(CompareMem(SetupPassword2->AdminHash, SetupPassword2->PowerOnHash, SYSTEM_PASSWORD_HASH_LENGTH) == 0){
    return TRUE;
  } else {
    return FALSE;
  }
}


EFI_STATUS
UpdatePassword (
  VOID
  )
{
  EFI_STATUS       Status = EFI_SUCCESS;

  if(gPasswordChanged){
    Status = gRT->SetVariable (
                    SYSTEM_PASSWORD2_NAME,
                    &gByoSystemPassword2VariableGuid,
                    SYSTEM_PASSWORD2_NV_ATTRIBUTE,
                    sizeof(SYSTEM_PASSWORD2),
                    &gSetupPassword2
                    );
  }
  
  return Status;
}


BYO_SYSTEM_PASSWORD2_PROTOCOL gByoSystemPassword2Protocol = {
  BeHavePassword,
  WritePassword,
  VerifyPassword,
  UpdatePassword,
  IsPasswordEqual
};


EFI_STATUS
EFIAPI
SystemPassword2Init (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS       Status = EFI_SUCCESS;
  EFI_HANDLE       Handle = NULL;
  UINTN            VariableSize = sizeof(SYSTEM_PASSWORD2);
  SYSTEM_PASSWORD2 *SetupPassword2 = &gSetupPassword2;
  BOOLEAN          PassowrdReset;

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  PassowrdReset = PcdGetBool(PcdPasswordResetGpioActive);
  gBootMode = GetBootModeHob();
  
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD2_NAME,
                  &gByoSystemPassword2VariableGuid,
                  NULL,
                  &VariableSize,
                  SetupPassword2
                  );
  DEBUG((EFI_D_INFO, "(L%d) %r PassowrdReset:%d\n", __LINE__, Status, PassowrdReset));	
  if(Status == EFI_NOT_FOUND || PassowrdReset) {
    VariableSize = sizeof(SYSTEM_PASSWORD2);
    ZeroMem(SetupPassword2, VariableSize);
    Status = gRT->SetVariable (
                    SYSTEM_PASSWORD2_NAME,
                    &gByoSystemPassword2VariableGuid,
                    SYSTEM_PASSWORD2_NV_ATTRIBUTE,
                    VariableSize,
                    SetupPassword2
                    );
  }

  Status = gBS->LocateProtocol(&gCryptoLibDxeProtocolGuid, NULL, (VOID**)&gCryptoDxe);
  ASSERT(!EFI_ERROR(Status));

  Status = gBS->InstallProtocolInterface (
                 &Handle,
                 &gByoSystemPassword2ProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &gByoSystemPassword2Protocol
                 );
  ASSERT_EFI_ERROR(Status);

  return Status;
}


