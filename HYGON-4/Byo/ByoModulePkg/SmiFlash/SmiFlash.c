/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SmiFlash.c

Abstract:
  Provides Access to flash backup Services through SMI

Revision History:

**/

#include "SmiFlash.h"
#include <Library/IoLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/BiosIdLib.h>
#include <Library/UefiLib.h>
#include <Guid/Acpi.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/PnpSmbios.h>
#include <Protocol/SetupItemUpdate.h>
#include <ByoSmiFlashInfo.h>
#include <Library/ByoCommLib.h>
#include <Guid/SmmVariableCommon.h>
#include <WindowsOA3.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <Protocol/ByoCommSmiSvcProtocol.h>
#include <Protocol/ByoSmiFlashExtProtocol.h>
#include <RsaSignDataHob.h>
#include <Library/BaseCryptLib.h>
#include <Library/HobLib.h>
#include <Library/SystemPasswordLib.h>
#include <Library/ByoBiosSignInfoLib.h>
#include <ByoBiosInfo2.h>
#include <ByoSmbiosTable.h>
#include <SysMiscCfg.h>
#include <Library/ByoRtcLib.h>

//---------------------------------------------------------------------------
NV_MEDIA_ACCESS_PROTOCOL              *mMediaAccess = NULL;
EFI_SMM_CPU_PROTOCOL                  *mSmmCpu = NULL;
UINT8                                 *BlockBuffer;
BOOLEAN                               EnvPrepared  = FALSE;
UINT16                                AcpiPmEnData = 0;
BIOS_ID_IMAGE                         gBiosIdImage;
UINT8                                 *mDisableKBCtrlAltDelReset = NULL;
EFI_PNP_SMBIOS_PROTOCOL               *mPnpSmbiosProtocol = NULL;
UINT8                                 *gResvPage;
BYO_COMM_SMI_SVC_PROTOCOL             *ByoCommSmiSvc = NULL;
SMI_INFO                              *gSmiFlashInfo = NULL;
UINT8                                 mBiosIdInfoHobStatus = 0;
BYO_SMIFLASH_EXT_PROTOCOL             *gByoSmiFlashExt = NULL;
BOOLEAN                               gBiosIdVersionCheck = FALSE;
SYSTEM_PASSWORD                       mSetupPassword;
BOOLEAN                               mIsReserveVariable = FALSE;
UINTN                                 mBiosAreaBaseAddress  = 0;
UINTN                                 mBiosAreaSize         = 0;
UINT32                                mMeAreaBaseAddress    = 0;
UINT32                                mMeAreaSize           = 0;
UINT32                                miRCAreaBaseAddress    = 0;
UINT32                                miRCAreaSize           = 0;
UINT32                                mNISAreaBaseAddress   = 0;
UINT32                                mNISAreaSize          = 0;
UINT32                                mECAreaBaseAddress    = 0;
UINT32                                mECAreaSize           = 0;
UINT32                                mGBEAreaBaseAddress    = 0;
UINT32                                mGBEAreaSize           = 0;
MY_BYO_BIOS_INFO2_TMP                 *gBiosInfo = NULL;
UINT32                                mBootMode = BIOS_BOOT_UEFI_OS;
UINT8                                 mPasswordComplexity = 0;


EFI_ACPI_3_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *gRsdp3;

UINTN
GetMsdmTableAddress(
  VOID
  )
{
  EFI_ACPI_DESCRIPTION_HEADER                   *XsdtHdr;
  EFI_ACPI_DESCRIPTION_HEADER                   *TblHdr;
  UINTN                                         TableCount;
  UINT64                                        *TblAddr64;
  UINTN                                         Index;


  DEBUG((DEBUG_VERBOSE, "%a\n", __FUNCTION__));
  DEBUG((DEBUG_VERBOSE, "Found ACPI Table: %lx\n", gRsdp3));

  XsdtHdr = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)gRsdp3->XsdtAddress;
  TableCount = (XsdtHdr->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) >> 3;
  TblAddr64  = (UINT64*)((UINT8*)XsdtHdr + sizeof(EFI_ACPI_DESCRIPTION_HEADER));
  DEBUG((DEBUG_VERBOSE, "Acpi Count: %x, TblAddress: %lx\n", TableCount, TblAddr64));
  for(Index = 0; Index < TableCount; Index++) {
    TblHdr = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)(TblAddr64[Index]);
    if ((TblHdr->Signature == SIGNATURE_32('S','S','D','M')) || (TblHdr->Signature == SIGNATURE_32('M','S','D','M'))) {
      DEBUG((DEBUG_VERBOSE, "Found MsdmTable! %lx\n", (UINTN)TblHdr));
      return (UINTN)TblHdr;
    }
  }

  return 0;
}


VOID
CheckOa3 (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_ACPI_MSDM_DATA_STRUCTURE          *NvMsdmData;
  EFI_ACPI_TABLE_PROTOCOL               *AcpiTableProtocol;
  EFI_ACPI_MS_DIGITAL_MARKER_TABLE      MsdmTable;
  UINTN                                 TableHandle;


  if(PcdGet32(PcdOa3HoleSize) == 0){
    DEBUG((EFI_D_INFO, "OA3 0 size\n"));
    return;
  }

  NvMsdmData = (EFI_ACPI_MSDM_DATA_STRUCTURE *)(UINTN)PcdGet32(PcdOa3HoleBase);
  DEBUG((EFI_D_INFO, "nvMsdmData: %X\n", NvMsdmData));
  if(NvMsdmData == NULL) {
    return;
  }

  //
  //Update the OA30 table header.
  //
  MsdmTable.Header.Signature = SIGNATURE_32('S','S','D','M');
  MsdmTable.Header.Length = sizeof(EFI_ACPI_MS_DIGITAL_MARKER_TABLE);
  MsdmTable.Header.Revision = 0x01;
  MsdmTable.Header.CreatorRevision = 0x01;
  MsdmTable.Header.OemTableId      = PcdGet64(PcdAcpiDefaultOemTableId);
  MsdmTable.Header.OemRevision     = PcdGet32(PcdAcpiDefaultOemRevision);
  MsdmTable.Header.CreatorId       = PcdGet32(PcdAcpiDefaultCreatorId);
  CopyMem(MsdmTable.Header.OemId, PcdGetPtr(PcdAcpiDefaultOemId), sizeof(MsdmTable.Header.OemId));
  if ((NvMsdmData->MsdmVersion != 0xffffffff) && (NvMsdmData->MsdmVersion != 0x54534554)) {
    MsdmTable.Header.Signature = SIGNATURE_32('M','S','D','M');
  }

  CopyMem (&(MsdmTable.MsdmData), NvMsdmData, sizeof(EFI_ACPI_MSDM_DATA_STRUCTURE));

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTableProtocol);

  TableHandle = 0;
  Status = AcpiTableProtocol->InstallAcpiTable (AcpiTableProtocol, &MsdmTable, sizeof(EFI_ACPI_MS_DIGITAL_MARKER_TABLE), &TableHandle);
  DEBUG((EFI_D_INFO, "%a(L%d):%r\n", __FUNCTION__, __LINE__, Status));

}

EFI_STATUS
EFIAPI
UpdateOA30TableToMem (
    EFI_ACPI_MSDM_DATA_STRUCTURE     *MsdmData
  )
{
  EFI_ACPI_MS_DIGITAL_MARKER_TABLE   *MsdmPointer;

  MsdmPointer = (EFI_ACPI_MS_DIGITAL_MARKER_TABLE*)GetMsdmTableAddress();
  DEBUG((DEBUG_VERBOSE,"MsdmPointer:%x\n",MsdmPointer));
  if (MsdmPointer == 0) {
    return EFI_NOT_FOUND;
  }

  if (CompareMem(&MsdmPointer->MsdmData, MsdmData, sizeof(EFI_ACPI_MSDM_DATA_STRUCTURE)) == 0) {
    return EFI_ALREADY_STARTED;
  }
  
  CopyMem(&MsdmPointer->MsdmData, MsdmData, sizeof(EFI_ACPI_MSDM_DATA_STRUCTURE));
  MsdmPointer->Header.Signature = SIGNATURE_32('M','S','D','M');
  AcpiTableUpdateChksum(MsdmPointer);
  DEBUG((DEBUG_VERBOSE,"OA30 memory update OK\n"));

  return EFI_SUCCESS;
}






EFI_STATUS
ProgramFlash (
  IN  UINT32  Offset,
  IN  UINT32  Size,
  IN  UINT64  Buffer
)
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINTN             BiosAddr;
  VOID              *DataBuffer;

  if (mMediaAccess == NULL) {
    return EFI_UNSUPPORTED;
  }

  if(Offset + Size > PcdGet32(PcdFlashAreaSize)){
    return EFI_INVALID_PARAMETER;
  }

  BiosAddr = (UINTN)PcdGet32(PcdFlashAreaBaseAddress) + Offset;
  DataBuffer = (VOID*)(UINTN)Buffer;

  DEBUG((DEBUG_VERBOSE, "[Update] %X -> %X L:%X\n", DataBuffer, BiosAddr, Size));
  
  Status = mMediaAccess->Erase (
                           mMediaAccess,
                           BiosAddr,
                           Size,
                           SPI_MEDIA_TYPE
                           );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Erase Error:%r\n", Status));
    goto ProcExit;
  }
  
  Status = mMediaAccess->Write (
                           mMediaAccess,
                           BiosAddr,
                           DataBuffer,
                           Size,
                           SPI_MEDIA_TYPE
                           );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Write Error:%r\n", Status));
    goto ProcExit;
  }

ProcExit:
  return Status;
}



EFI_STATUS
WriteFlash (
  IN  UINT32  Offset,
  IN  UINT32  Size,
  IN  UINT64  Buffer
)
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINTN             BiosAddr;
  VOID              *DataBuffer;

  if (mMediaAccess == NULL) {
    return EFI_UNSUPPORTED;
  }

  if(Offset + Size > PcdGet32(PcdFlashAreaSize)){
    return EFI_INVALID_PARAMETER;
  }

  BiosAddr = (UINTN)PcdGet32(PcdFlashAreaBaseAddress) + Offset;
  DataBuffer = (VOID*)(UINTN)Buffer;

  DEBUG((DEBUG_VERBOSE, "[Write] %X -> %X L:%X\n", DataBuffer, BiosAddr, Size));
  
  Status = mMediaAccess->Write (
                           mMediaAccess,
                           BiosAddr,
                           DataBuffer,
                           Size,
                           SPI_MEDIA_TYPE
                           );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Write Error:%r\n", Status));
    goto ProcExit;
  }

ProcExit:
  return Status;
}



EFI_STATUS
EraseFlash (
  IN  UINT32  Offset,
  IN  UINT32  Size
)
{
  EFI_STATUS        Status = EFI_SUCCESS;
  UINTN             BiosAddr;

  if (mMediaAccess == NULL) {
    return EFI_UNSUPPORTED;
  }

  if(Offset + Size > PcdGet32(PcdFlashAreaSize) || Size == 0){
    return EFI_INVALID_PARAMETER;
  }

  BiosAddr = (UINTN)PcdGet32(PcdFlashAreaBaseAddress) + Offset;

  DEBUG((DEBUG_VERBOSE, "[Erase] %X L:%X\n", BiosAddr, Size));

  Status = mMediaAccess->Erase (
                           mMediaAccess,
                           BiosAddr,
                           Size,
                           SPI_MEDIA_TYPE
                           );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "Erase Error:%r\n", Status));
    goto ProcExit;
  }

ProcExit:
  return Status;
}

EFI_STATUS CheckBiosInfo(MY_BYO_BIOS_INFO2_TMP *BiosInfo2)
{
  EFI_STATUS Status = EFI_SUCCESS;
  UINTN Size;
  if (gBiosInfo) {
    Size = gBiosInfo->Header.HeaderLength + gBiosInfo->Header.Step_NUM * sizeof(FLASH_STEP);
    if(CompareMem(gBiosInfo, BiosInfo2, Size) != 0){
      DEBUG((EFI_D_ERROR, "BiosInfo changed\n"));
      Status = EFI_UNSUPPORTED;
    }
  } else {
    DEBUG((EFI_D_ERROR, "BiosInfo not found\n"));
    Status = EFI_UNSUPPORTED;
  }
  return Status;
}

EFI_STATUS CheckBiosId(BIOS_ID_IMAGE *Id)
{
  BIOS_ID_IMAGE  *OldBiosId;
  BIOS_ID_STRING *IdStr;
  EFI_STATUS     Status = EFI_OUT_OF_RESOURCES;      // Not match

  if (mBiosIdInfoHobStatus == 1) {
    return Status;
  }

  OldBiosId = &gBiosIdImage;
  if(CompareMem(OldBiosId->Signature, Id->Signature, sizeof(OldBiosId->Signature))){
    goto ProcExit;
  }

  IdStr = &gBiosIdImage.BiosIdString;
  if(CompareMem(IdStr->BoardId, Id->BiosIdString.BoardId, sizeof(IdStr->BoardId)) ||
     CompareMem(IdStr->OemId, Id->BiosIdString.OemId, sizeof(IdStr->OemId))){
    goto ProcExit;
  }

  //
  // Check BIOS VersionMajor
  //
  if (gBiosIdVersionCheck) {
    if (CompareMem(IdStr->VersionMajor, Id->BiosIdString.VersionMajor, sizeof(IdStr->VersionMajor)) > 0) {
      goto ProcExit;
    }
  }
  Status = EFI_SUCCESS;

ProcExit:
  CopyMem(Id, &gBiosIdImage, sizeof(BIOS_ID_IMAGE));
  return Status;
}



VOID 
PatchForUsb ( 
    IN UINT8 PatchFlag 
  )
{
  if(PatchFlag == 1) {
    if(mDisableKBCtrlAltDelReset != NULL){
      *mDisableKBCtrlAltDelReset = 1;
    }
  } else if(PatchFlag == 0) {
    if(mDisableKBCtrlAltDelReset != NULL){
      *mDisableKBCtrlAltDelReset = 0;
    }
  }
}



EFI_STATUS SyncNvram()
{
  SMM_VARIABLE_COMMUNICATE_HEADER  *VarCommHdr;
  UINTN                            BufferSize;
  EFI_STATUS                       Status;
  
  VarCommHdr = (SMM_VARIABLE_COMMUNICATE_HEADER*)gResvPage;
  BufferSize = sizeof(SMM_VARIABLE_COMMUNICATE_HEADER);
  ZeroMem(VarCommHdr, BufferSize);
  VarCommHdr->Function = SMM_VARIABLE_FUNCTION_SYNC_NVRAM;
  Status = gSmst->SmiManage (
                    &gEfiSmmVariableProtocolGuid, 
                    NULL, 
                    VarCommHdr, 
                    &BufferSize
                    );
  if(!EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "(L%d) sync nvram: %r\n", __LINE__, VarCommHdr->ReturnStatus));
  }

  return Status;
}



EFI_STATUS
QuerySetupPassWordHash(
  OUT UINT8 *PassWordHash
)
{
  EFI_STATUS        Status;
  UINTN             VariableSize;
  SYSTEM_PASSWORD   SetupPassword;


  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = mSmmVariable->SmmGetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VariableSize,
                  (VOID **)&SetupPassword
                  );
  if(EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }
  if(PassWordHash != NULL){
    if (SetupPassword.AdminHash[0] != 0) {
      CopyMem(PassWordHash, SetupPassword.AdminHash, SYSTEM_PASSWORD_HASH_LENGTH);
    } else if (SetupPassword.PowerOnHash[0] != 0){
      CopyMem(PassWordHash, SetupPassword.PowerOnHash, SYSTEM_PASSWORD_HASH_LENGTH);
    } else {
      return EFI_NOT_FOUND;
    }
    DEBUG((DEBUG_VERBOSE, "Query Hash: %x\n", *(UINT32 *)PassWordHash));
  }
  if((SetupPassword.AdminHash[0] != 0) || (SetupPassword.PowerOnHash[0] != 0)){
    return EFI_SUCCESS;
  } else {
    return EFI_ABORTED;
  }  
}


EFI_STATUS
VerifySetupPassword(
  IN UINT8   *PassWord
)
{
  UINTN      Len;
  UINT8      *InputHash = NULL;
  UINT8      PasswordHash[SYSTEM_PASSWORD_HASH_LENGTH];
  UINT8      Index;
  UINTN      InputLen;

  Len = AsciiStrLen(PassWord);
  DEBUG((DEBUG_VERBOSE, "String Len: %x\n Input String: %a\n", Len, PassWord));
  if (PassWord[0] != 0) {
    InputLen = EncodePassword(PassWord, Len, &InputHash, 0);
    QuerySetupPassWordHash(PasswordHash);
    for (Index = 0; Index < InputLen; Index++) { 
      if (PasswordHash[Index] == InputHash[Index]) {
        continue;
      } else {
        FreePool(InputHash);
        return EFI_ABORTED;
      }
    }
    FreePool(InputHash);
    return EFI_SUCCESS;
  }
  return EFI_ABORTED;
}

EFI_STATUS
SetPassWordHash(
  IN UINT8   PasswordType,    //0:Admin 1:Poweron
  IN UINT8   *PassWord
)
{
  EFI_STATUS Status;
  UINTN      Len;
  UINT8      *InputHash = NULL;
  UINTN      InputLen;
  CHAR16     PasswordStr16[SYSTEM_PASSWORD_LENGTH + 1];

  UINTN      VariableSize = sizeof(SYSTEM_PASSWORD);
  SYSTEM_PASSWORD SetupPassword;
  UINT32     Attribute;
  EFI_TIME   EfiTime;
  SYSTEM_PASSWORD_REQUIREMENTS    SystemPasswordRequirement;
  UINTN      VariableSizeRequirement;
  VariableSize = sizeof(SYSTEM_PASSWORD);
  Status = mSmmVariable->SmmGetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  &Attribute,
                  &VariableSize,
                  &SetupPassword
                  );
  
  Len = AsciiStrLen(PassWord);
  DEBUG((DEBUG_VERBOSE, "String Len: %x\n Input String: %a\n", Len, PassWord));
  if (PassWord[0] != 0) {
    //
    //Check the password length.
    //
    if (Len < SYSTEM_PASSWORD_MIN_LENGTH ||Len > SYSTEM_PASSWORD_LENGTH) {
      return EFI_BAD_BUFFER_SIZE;
    }
    AsciiStrToUnicodeStrS ((CHAR8 *)PassWord, PasswordStr16, ARRAY_SIZE(PasswordStr16));
    //
    //Check password complexity.
    //
    if (mPasswordComplexity && (!ByoVerifyPasswordComplexity(PasswordStr16))) {
      return EFI_UNSUPPORTED;
    }

    VariableSizeRequirement = sizeof(SYSTEM_PASSWORD_REQUIREMENTS);  
    Status = mSmmVariable->SmmGetVariable (
                    PASSWORD_REQUIREMENTS,
                    &gEfiSystemPasswordVariableGuid,
                    NULL,
                    &VariableSizeRequirement,
                    &SystemPasswordRequirement
                    );
    
    if ( !EFI_ERROR (Status) && SystemPasswordRequirement.AdminAndUserNotSame == TRUE){
      if (PasswordType == ADMIN_PASSWORD){
        Status = ByoCheckRepeatPsd(POWERON_PASSWORD, PasswordStr16);
        if (Status == TRUE){
          return EFI_ACCESS_DENIED;
        }
      } else {
        Status = ByoCheckRepeatPsd(ADMIN_PASSWORD, PasswordStr16);
        if (Status == TRUE){
          return EFI_ACCESS_DENIED;
        }
      }
    }

    //
    //Verify that the password is duplicate.
    //
    if (ByoCheckPasswordRecord(PasswordType, PasswordStr16)) {
      return EFI_ALREADY_STARTED;
    }

    InputLen = EncodePassword(PassWord, Len, &InputHash, 0);

    Status = ByoRtcGetTime(&EfiTime);
    if (EFI_ERROR (Status)) {
      ZeroMem (&EfiTime, sizeof (EFI_TIME));
    }
    
    if (PasswordType == ADMIN_PASSWORD){
      SetupPassword.bHaveAdmin = TRUE;
      CopyMem( SetupPassword.AdminHash, InputHash, SYSTEM_PASSWORD_HASH_LENGTH);
      SetupPassword.VerifyTimes = 0;
      CopyMem(&SetupPassword.AdmPwdTime, &EfiTime, sizeof (PASSWOR_TIME));
    } else if (PasswordType == POWERON_PASSWORD){
      SetupPassword.bHavePowerOn = TRUE;
      CopyMem( SetupPassword.PowerOnHash, InputHash, SYSTEM_PASSWORD_HASH_LENGTH);
      SetupPassword.VerifyTimes = 0;
      CopyMem(&SetupPassword.PopPwdTime, &EfiTime, sizeof (PASSWOR_TIME));
    }
    Status = mSmmVariable->SmmSetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  Attribute,
                  VariableSize,
                  &SetupPassword
                  );
    FreePool(InputHash);
  } else {
    if (PasswordType == ADMIN_PASSWORD){
      SetupPassword.bHaveAdmin = FALSE;
      SetMem(SetupPassword.AdminHash, SYSTEM_PASSWORD_HASH_LENGTH, 0);
      SetupPassword.VerifyTimes = 0;
    } else if (PasswordType == POWERON_PASSWORD){
      SetupPassword.bHavePowerOn = FALSE;
      SetMem(SetupPassword.PowerOnHash, SYSTEM_PASSWORD_HASH_LENGTH, 0);
      SetupPassword.VerifyTimes = 0;
    }
    if (!SetupPassword.bHaveAdmin && !SetupPassword.bHavePowerOn) {
      SetupPassword.EnteredType = LOGIN_USER_ADMIN;
    }
    Status = mSmmVariable->SmmSetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  Attribute,
                  VariableSize,
                  &SetupPassword
                  );
  }
  return Status;
}


typedef struct {
  UINT32               HashType;
  VOID                 *HashCtx;
  UINTN                CtxSize;
  BOOLEAN              Init;
  UINT8                *HashArray;
  BOOLEAN              VerifyOK;
  UINT8                SignData[256];
  UINT8                KeyN[256];
  UINT8                KeyE[3];
  UINT8                SignMethod;
  UINT32               SignFvOffset;
} BIOS_SIGN_CTX;


BIOS_SIGN_CTX gBiosSignCtx = {
  0,                                // HashType
  NULL,                             // HashCtx
  0,                                // CtxSize
  FALSE,                            // Init
  NULL,                             // HashArray
  FALSE,                            // VerifyOK
  {0,},                             // SignData
  {0,},                             // KeyN
  {0,},                             // KeyE
  0,
  0,
};





EFI_STATUS
SignInfoVerify (
  VOID
)
{
  UINT8         Hash[32];
  VOID          *Rsa = NULL; 
  BOOLEAN       Rc;
  EFI_STATUS    Status = EFI_ABORTED;

  if(gBiosSignCtx.SignFvOffset + SIZE_4KB >= PcdGet32(PcdFlashAreaSize)){
    return EFI_ABORTED;
  }
  
  Sha256HashAll(gSmiFlashInfo->Buffer, SIZE_4KB - 256, Hash);
  
  Rsa = RsaNew();
  if (Rsa == NULL) {
    DEBUG((EFI_D_ERROR, "%a (L%d)\n", __FUNCTION__, __LINE__));
    goto ProcExit; 
  }
  if (!RsaSetKey(Rsa, RsaKeyN, gBiosSignCtx.KeyN, sizeof(gBiosSignCtx.KeyN))) {
    DEBUG((EFI_D_ERROR, "%a (L%d)\n", __FUNCTION__, __LINE__));
    goto ProcExit; 
  }
  if (!RsaSetKey(Rsa, RsaKeyE, gBiosSignCtx.KeyE, sizeof(gBiosSignCtx.KeyE))) {
    DEBUG((EFI_D_ERROR, "%a (L%d)\n", __FUNCTION__, __LINE__));
    goto ProcExit; 
  }

  Rc = RsaPkcs1Verify (
          Rsa, 
          Hash, 
          sizeof(Hash), 
          gSmiFlashInfo->Buffer + SIZE_4KB - 256, 
          256
          );
  if (!Rc) {
    DEBUG((EFI_D_ERROR, "SignInfoVerify RsaPkcs1Verify ERROR\n"));
    goto ProcExit;
  }

  Status = EFI_SUCCESS;
  Sha256HashAll(gSmiFlashInfo->Buffer, SIZE_4KB, Hash);
  CopyMem(gBiosSignCtx.HashArray + gBiosSignCtx.SignFvOffset/SIZE_4KB*32, Hash, 32);

ProcExit:
  if(Rsa != NULL){RsaFree(Rsa);}  
  return Status;
}






EFI_STATUS
BiosFileHashUpdate (
  IN  UINT32  Offset,
  IN  UINT32  Size,
  IN  UINT8   *Buffer
)
{
  EFI_STATUS            Status = EFI_SUCCESS;
  UINT8                 *Hash;
  UINT8                 *BiosData;
  BOOLEAN               OutBios = FALSE;


  if(!gBiosSignCtx.Init){
    return EFI_NOT_READY;
  }

  if(Offset + Size > PcdGet32(PcdFlashAreaSize)){
    OutBios = TRUE;
  } else {
    if((Offset & 0xFFF) || (Size != SIZE_4KB)){
      return EFI_INVALID_PARAMETER;
    }
  }

  if(Offset == gBiosSignCtx.SignFvOffset && Size == SIZE_4KB && 
    (gBiosSignCtx.SignMethod == SIGN_METHOD_INFO_IN_FV || gBiosSignCtx.SignMethod == SIGN_METHOD_FIXED_OFFSET)){
    Status = SignInfoVerify();
    DEBUG((DEBUG_VERBOSE, "START SIGN INFO\n"));
    goto ProcExit;
  }

  BiosData = Buffer;
  if(!OutBios){
    Hash = gBiosSignCtx.HashArray + Offset/SIZE_4KB*32;
    Sha256HashAll(BiosData, Size, Hash);
  }

  if(gBiosSignCtx.HashType == SIGN_INIT_HASH_TYPE_SHA512){
    Sha512Update(gBiosSignCtx.HashCtx, BiosData, Size);    
  } else {
    Sha256Update(gBiosSignCtx.HashCtx, BiosData, Size);
  }
  
  if(gBiosSignCtx.VerifyOK){
    gBiosSignCtx.VerifyOK = FALSE;
  }

  DEBUG((DEBUG_VERBOSE, "+%X\n", Offset));

ProcExit:  
  return Status;
}



EFI_STATUS
BiosFileSignVerify (
  VOID
  )
{
  VOID          *Rsa = NULL;
  UINT8         HashValue[64];
  UINTN         HashSize;
  BOOLEAN       Rc;
  EFI_STATUS    Status = EFI_ABORTED;
  UINT8         *n;
  UINT8         *e;


  if(gBiosSignCtx.HashType == SIGN_INIT_HASH_TYPE_SHA512){
    Rc = Sha512Final(gBiosSignCtx.HashCtx, HashValue);
    HashSize = 64;
  } else {
    Rc = Sha256Final(gBiosSignCtx.HashCtx, HashValue);
    HashSize = 32;
  }
  if(!Rc){
    DEBUG((EFI_D_ERROR, "%a (L%d)\n", __FUNCTION__, __LINE__));
    goto ProcExit;
  }

  n = gBiosSignCtx.KeyN;
  e = gBiosSignCtx.KeyE;
  
  Rsa = RsaNew();
  if (Rsa == NULL) {
    DEBUG((EFI_D_ERROR, "%a (L%d)\n", __FUNCTION__, __LINE__));
    goto ProcExit; 
  }
  if (!RsaSetKey(Rsa, RsaKeyN, n, sizeof(gBiosSignCtx.KeyN))) {
    DEBUG((EFI_D_ERROR, "%a (L%d)\n", __FUNCTION__, __LINE__));
    goto ProcExit; 
  }
  if (!RsaSetKey(Rsa, RsaKeyE, e, sizeof(gBiosSignCtx.KeyE))) {
    DEBUG((EFI_D_ERROR, "%a (L%d)\n", __FUNCTION__, __LINE__));
    goto ProcExit; 
  }

  Rc = RsaPkcs1Verify (
          Rsa, 
          HashValue, 
          HashSize, 
          gBiosSignCtx.SignData, 
          sizeof(gBiosSignCtx.SignData)
          );
  if (!Rc) {
    DEBUG((EFI_D_ERROR, "RsaPkcs1Verify ERROR\n"));
    DumpMem8(gBiosSignCtx.SignData, sizeof(gBiosSignCtx.SignData));
    DumpMem8(HashValue, HashSize);
    DumpMem8(gBiosSignCtx.KeyN, sizeof(gBiosSignCtx.KeyN));
    DumpMem8(gBiosSignCtx.KeyE, sizeof(gBiosSignCtx.KeyE));    
    goto ProcExit;
  }

  Status = EFI_SUCCESS;
  gBiosSignCtx.VerifyOK = TRUE;

ProcExit:
  if(Rsa != NULL){RsaFree(Rsa);}
  DEBUG((DEBUG_VERBOSE, "SignVerify:%r\n", Status));
  return Status;
}




typedef struct {
  UINT32  Offset;
  UINT32  Size;
} AREA_INFO;
STATIC AREA_INFO gOpenAreaInfo[] = {
  {FixedPcdGet32(PcdEventLogBase)    - FixedPcdGet32(PcdFlashAreaBaseAddress), FixedPcdGet32(PcdEventLogSize)},
  {FixedPcdGet32(PcdFlashNvLogoBase) - FixedPcdGet32(PcdFlashAreaBaseAddress), FixedPcdGet32(PcdFlashNvLogoSize)},
  {FixedPcdGet32(PcdRomHoleBase)     - FixedPcdGet32(PcdFlashAreaBaseAddress), FixedPcdGet32(PcdRomHoleSize)},  
  {FixedPcdGet32(PcdFlashNvStorageMicrocodeBase)     - FixedPcdGet32(PcdFlashAreaBaseAddress), FixedPcdGet32(PcdFlashNvStorageMicrocodeSize)},  
  {FixedPcdGet32(PcdOa3HoleBase)     - FixedPcdGet32(PcdFlashAreaBaseAddress), FixedPcdGet32(PcdOa3HoleSize)},   
  {FixedPcdGet32(PcdFlashNvStorageVariableBase) - FixedPcdGet32(PcdFlashAreaBaseAddress), FixedPcdGet32(PcdFlashNvStorageVariableSize)},
  {FixedPcdGet32(PcdFlashNvStorageSmbiosBase) - FixedPcdGet32(PcdFlashAreaBaseAddress), FixedPcdGet32(PcdFlashNvStorageSmbiosSize)},
  {(UINT32)FixedPcdGet64(PcdFvSignRegionBase) - FixedPcdGet32(PcdFlashAreaBaseAddress), (UINT32)FixedPcdGet64(PcdFvSignRegionSize)}
};

BOOLEAN
IsSectorDataVerified (
  IN  UINT32  Offset,
  IN  UINT32  Size,
  IN  UINT8   *Buffer
)  
{
  UINT8                 *SavedHash;
  UINT8                 Hash[32];
  UINTN                 Index;

  if (PcdGetBool(PcdByoSecureFlashSupport) == FALSE) {
    return TRUE;
  }

  for(Index=0;Index<ARRAY_SIZE(gOpenAreaInfo);Index++){
    if(gOpenAreaInfo[Index].Size == 0){
      continue;
    }
    if(Offset >= gOpenAreaInfo[Index].Offset && 
       Size <= gOpenAreaInfo[Index].Size &&
       Offset + Size <= gOpenAreaInfo[Index].Offset + gOpenAreaInfo[Index].Size){
      DEBUG((DEBUG_VERBOSE, "in area %d\n", Index));
      return TRUE;
    }
  }

  if(!gBiosSignCtx.VerifyOK){
    DEBUG((EFI_D_ERROR, "not verify\n"));
    return FALSE;
  }
  if((Offset & 0xFFF) || (Size != SIZE_4KB)){
    DEBUG((EFI_D_ERROR, "bad Offset or Size\n"));
    return FALSE;
  }
  if(Offset + Size > PcdGet32(PcdFlashAreaSize)){
    DEBUG((EFI_D_ERROR, "out of bios\n"));
    return FALSE;
  }

  SavedHash = gBiosSignCtx.HashArray + Offset/SIZE_4KB*32;

  Sha256HashAll(Buffer, Size, Hash);
  if(CompareMem(SavedHash, Hash, sizeof(Hash)) == 0){
    DEBUG((DEBUG_VERBOSE, "Hash match\n"));
    return TRUE;
  } else {
    DEBUG((EFI_D_ERROR, "Hash not match(%X,%X)\n", Offset, Size));
    DumpMem8(SavedHash, 32);
    DumpMem8(Hash, 32);
    return FALSE;
  }
}


EFI_STATUS
BiosSignPubKeyInit (
  VOID
)
{
  EFI_PEI_HOB_POINTERS          GuidHob;
  PK_RSA2048_HOB_DATA           *PubKey;
  UINT32                        Crc32;
  EFI_STATUS                    Status;

  GuidHob.Raw = GetFirstGuidHob(&gByoBiosSignPubKeyFileGuid);
  if(GuidHob.Raw == NULL){
    return EFI_NOT_FOUND;
  }
  PubKey = GET_GUID_HOB_DATA(GuidHob.Guid);
  Status = LibCalcCrc32(PubKey->KeyN, PubKey->Hdr.KeySize, &Crc32);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if(Crc32 != PubKey->Hdr.Crc32){
    return EFI_CRC_ERROR;
  }
  CopyMem(gBiosSignCtx.KeyN, PubKey->KeyN, sizeof(gBiosSignCtx.KeyN));
  CopyMem(gBiosSignCtx.KeyE, PubKey->KeyE, sizeof(gBiosSignCtx.KeyE));

  return EFI_SUCCESS;
}

EFI_STATUS
GetVariableByBuffer (
  UINT8    *Buffer
)
{
  EFI_STATUS   Status;
  UINTN VariableLength = (UINTN)(((VARIABLE_STRUCT *)Buffer)->VariableLen);
  Status = mSmmVariable->SmmGetVariable (
    ((VARIABLE_STRUCT *)Buffer)->VariableName,
    &(((VARIABLE_STRUCT *)Buffer)->VariableGuid),
    &(((VARIABLE_STRUCT *)Buffer)->Attribute),
    &VariableLength,
    ((VARIABLE_STRUCT *)Buffer)->Variabledata
    );
  ((VARIABLE_STRUCT *)Buffer)->VariableLen = (UINT64)VariableLength;
  return Status;
}

EFI_STATUS
SetVariableByBuffer (
  UINT8    *Buffer
)
{
  EFI_STATUS                      Status;
  Status = mSmmVariable->SmmSetVariable (
    ((VARIABLE_STRUCT *)Buffer)->VariableName,
    &(((VARIABLE_STRUCT *)Buffer)->VariableGuid),
    ((VARIABLE_STRUCT *)Buffer)->Attribute,
    ((VARIABLE_STRUCT *)Buffer)->VariableLen,
    ((VARIABLE_STRUCT *)Buffer)->Variabledata
    );
  return Status;
}



EFI_STATUS
EFIAPI
FlashInterface (
  IN     EFI_HANDLE               DispatchHandle,
  IN     CONST VOID               *Context,        OPTIONAL
  IN OUT VOID                     *CommBuffer,     OPTIONAL
  IN OUT UINTN                    *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS                   Status;
  UINTN                        Index;
  UINTN                        CpuIndex;
  UINT32                       SubFunction;
  EFI_SMM_SAVE_STATE_IO_INFO   IoState;
  UINT16                       AcpiIoBase;
  FD_AREA_INFO                 *FdArea;
  UINT8                        CmosData;
  UPDATE_VALUE_PARAMETER       *VariableParam;
  UINTN                        VariableSize;
  
  AcpiIoBase = PcdGet16(AcpiIoPortBaseAddress);   
  CpuIndex = 0;
  for(Index = 0; Index < gSmst->NumberOfCpus; Index++) {
    Status = mSmmCpu->ReadSaveState (
                        mSmmCpu,
                        sizeof (EFI_SMM_SAVE_STATE_IO_INFO),
                        EFI_SMM_SAVE_STATE_REGISTER_IO,
                        Index,
                        &IoState
                        );
    if(!EFI_ERROR (Status) && (IoState.IoData == SW_SMI_FLASH_SERVICES)) {
      CpuIndex = Index;
      break;
    }
  }
  
  if(Index >= gSmst->NumberOfCpus) {
    CpuDeadLoop ();
  }

  SubFunction = gSmiFlashInfo->SubFunction;
  DEBUG((DEBUG_VERBOSE, "SubFunction:%X\n", SubFunction));
  //
  // Use BYO_SMIFLASH_EXT_PROTOCOL to do platform customize subfunction
  //
  if (gByoSmiFlashExt != NULL) {
    if (gByoSmiFlashExt->ByoSmiFlashExtFunc(gSmiFlashInfo, mMediaAccess, &Status)) {
      goto ProcExit;
    }
    Status = EFI_SUCCESS;
  } else {
    Status = gSmst->SmmLocateProtocol (&gByoSmiFlashExtProtocolGuid, NULL, (VOID **)&gByoSmiFlashExt);
    if (!EFI_ERROR (Status)) {
      if (gByoSmiFlashExt->ByoSmiFlashExtFunc(gSmiFlashInfo, mMediaAccess, &Status)) {
        goto ProcExit;
      }
    }
    Status = EFI_SUCCESS;
  }

  switch (SubFunction) {  
    case SUBFUNCTION_GET_FD_AREA:
    case GET_FD_AREA_SUBFUNCTION:
      FdArea = (FD_AREA_INFO*)(UINTN)gSmiFlashInfo->Buffer;

      switch(FdArea->Type) {
        default:
          DEBUG((DEBUG_VERBOSE, "T:%X\n", FdArea->Type));
          Status = RETURN_INVALID_PARAMETER;
          break;
        
        case FD_AREA_TYPE_FD:
          FdArea->Offset = 0;
          FdArea->Size   = PcdGet32(PcdFlashAreaSize);
          Status = EFI_SUCCESS;
          break;

        case FD_AREA_TYPE_LOGO:
          FdArea->Offset = PcdGet32(PcdFlashNvLogoBase) - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = PcdGet32(PcdFlashNvLogoSize);
          Status = EFI_SUCCESS;
          break;

        case FD_AREA_TYPE_SMBIOS:
          FdArea->Offset = PcdGet32(PcdFlashNvStorageSmbiosBase) - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = PcdGet32(PcdFlashNvStorageSmbiosSize);
          Status = EFI_SUCCESS;          
          break;

        case FD_AREA_TYPE_NV_STORAGE:
          FdArea->Offset = PcdGet32(PcdFlashNvStorageVariableBase) - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = PcdGet32(PcdFlashNvStorageVariableSize);
          Status = EFI_SUCCESS;           
          break;
        case FD_AREA_TYPE_SETUP_ITEMS_NAME:
          FdArea->Offset = 0;
          FdArea->Size = mSetupNameSize;
          DEBUG((EFI_D_INFO, "(%X,%X,%X)\n", FdArea->Type, FdArea->Offset, FdArea->Size));
          Status = EFI_SUCCESS;
          break;
        case FD_AREA_TYPE_MICROCODE:
          if (PcdGet32(PcdFlashNvStorageMicrocodeSize) == 0) {
            Status = EFI_UNSUPPORTED;
            break;
          }
          FdArea->Offset = PcdGet32(PcdFlashNvStorageMicrocodeBase) - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = PcdGet32(PcdFlashNvStorageMicrocodeSize);
          Status = EFI_SUCCESS;
          break;
        case FD_AREA_TYPE_OA3:
          if (PcdGet32(PcdOa3HoleSize) == 0) {
            Status = EFI_UNSUPPORTED;
            break;
          }
          FdArea->Offset = PcdGet32(PcdOa3HoleBase) - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = PcdGet32(PcdOa3HoleSize);
          Status = EFI_SUCCESS;
          break;
        case FD_AREA_TYPE_SETUP_ITEMS_OPTION:
          FdArea->Offset = 0;
          FdArea->Size = mSetupOptionsSize;
          DEBUG((DEBUG_VERBOSE, "(%X,%X,%X)\n", FdArea->Type, FdArea->Offset, FdArea->Size));
          Status = EFI_SUCCESS;
          break;
        case FD_AREA_TYPE_SETUP_ITEMS_STRING:
          FdArea->Offset = 0;
          FdArea->Size = mSetupItemStringSize;
          //DEBUG((DEBUG_VERBOSE, "(%X,%X,%X)\n", FdArea->Type, FdArea->Offset, FdArea->Size));
          Status = EFI_SUCCESS;
          break;
        case FD_AREA_TYPE_ME:
          if (mMeAreaSize == 0) {
            Status = EFI_UNSUPPORTED;
            break;
          }
          FdArea->Offset = mMeAreaBaseAddress - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = mMeAreaSize;
          Status = EFI_SUCCESS;
          break;
        case FD_AREA_TYPE_IRC:
          if (miRCAreaSize == 0) {
            Status = EFI_UNSUPPORTED;
            break;
          }
          FdArea->Offset = miRCAreaBaseAddress - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = miRCAreaSize;
          Status = EFI_SUCCESS;
          break;
        case FD_AREA_TYPE_EC:
          if (mECAreaSize == 0) {
            Status = EFI_UNSUPPORTED;
            break;
          }
          FdArea->Offset = mECAreaBaseAddress - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = mECAreaSize;
          Status = EFI_SUCCESS;
          break;
        case FD_AREA_TYPE_GBE:
          if (mGBEAreaSize == 0) {
            Status = EFI_UNSUPPORTED;
            break;
          }
          FdArea->Offset = mGBEAreaBaseAddress - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = mGBEAreaSize;
          Status = EFI_SUCCESS;
          break;
        case FD_AREA_TYPE_NIS:
          if (mNISAreaSize == 0) {
            Status = EFI_UNSUPPORTED;
            break;
          }
          FdArea->Offset = mNISAreaBaseAddress - PcdGet32(PcdFlashAreaBaseAddress);
          FdArea->Size   = mNISAreaSize;
          Status = EFI_SUCCESS;
          break; 
        case FD_AREA_TYPE_SETUP_ITEMS_ONEOF_STRING:
          FdArea->Offset = 0;
          FdArea->Size = mSetupItemOneOfOptionStringSize;
          DEBUG((DEBUG_VERBOSE, "(%X,%X,%X)\n", FdArea->Type, FdArea->Offset, FdArea->Size));
          Status = EFI_SUCCESS;
          break;
      }

      if(!EFI_ERROR(Status)){
        DEBUG((DEBUG_VERBOSE, "(%X,%X,%X)\n", FdArea->Type, FdArea->Offset, FdArea->Size));
      }
      
      break;

    case SUBFUNCTION_BIOS_VERIFY:
      //
      // After full BIOS is updated, reset VerifyOK as FALSE.
      //
      gBiosSignCtx.VerifyOK = FALSE;
      if (mIsReserveVariable) {
        mIsReserveVariable = FALSE;

        SyncNvram();

        if (PcdGetBool (PcdSmiFlashUpdateReservePassword)) {
          //
          // Set password after bios update
          //
          mSetupPassword.PasswordValidDays = 0;
          mSetupPassword.RequirePopOnRestart = 0;
          
          VariableSize = sizeof(SYSTEM_PASSWORD);
          mSmmVariable->SmmSetVariable (
            SYSTEM_PASSWORD_NAME,
            &gEfiSystemPasswordVariableGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
            VariableSize,
            (VOID **)&mSetupPassword
            );
        }
      }
      break;

    case SUBFUNCTION_DISABLE_USB_POWERBUTTON:
      if (ByoCommSmiSvc != NULL && ByoCommSmiSvc->DisablePowerButton != NULL) {
        Status = ByoCommSmiSvc->DisablePowerButton(1);
      } else {
        Status = EFI_UNSUPPORTED;
      }
      if(EFI_ERROR(Status)){
        AcpiPmEnData = IoRead16(AcpiIoBase+2);
        IoWrite16(AcpiIoBase+2, AcpiPmEnData & (UINT16)~BIT8);  // disable power button.
        EnvPrepared = TRUE;
        Status = EFI_SUCCESS;
      }
      PatchForUsb(1);
      break;
    
    case SUBFUNCTION_ENABLE_USB_POWERBUTTON:
      if (ByoCommSmiSvc != NULL && ByoCommSmiSvc->DisablePowerButton != NULL) {
        Status = ByoCommSmiSvc->DisablePowerButton(0);
      } else {
        Status = EFI_UNSUPPORTED;
      }
      if(EFI_ERROR(Status)){
        if(EnvPrepared){
          IoWrite16(AcpiIoBase, BIT8);           // clear power button status.
          IoWrite16(AcpiIoBase+2, AcpiPmEnData); // restore
          EnvPrepared = FALSE;
          Status = EFI_SUCCESS;
        }
      }
      PatchForUsb(0);
      break;

    case SUBFUNCTION_UPDATE_SMBIOS_DATA:
      Status = HandleSmbiosDataRequest((UPDATE_SMBIOS_PARAMETER*)(UINTN) gSmiFlashInfo->Buffer);
      break;

    case SUBFUNCTION_PROGRAM_FLASH:
      if(IsSectorDataVerified(gSmiFlashInfo->Offset, gSmiFlashInfo->Size, gSmiFlashInfo->Buffer)){
        if (gSmiFlashInfo->Offset == (PcdGet32(PcdFlashNvStorageVariableBase) - PcdGet32(PcdFlashAreaBaseAddress))) {
          mIsReserveVariable = TRUE;
          if (PcdGetBool (PcdSmiFlashUpdateReservePassword)) {
            //
            // Get Password before variable region update
            //
            VariableSize = sizeof(SYSTEM_PASSWORD);
            mSmmVariable->SmmGetVariable (
              SYSTEM_PASSWORD_NAME,
              &gEfiSystemPasswordVariableGuid,
              NULL,
              &VariableSize,
              (VOID **)&mSetupPassword
              );
          }
        }
        Status = ProgramFlash (
                   gSmiFlashInfo->Offset,
                   gSmiFlashInfo->Size,
                   (UINTN)gSmiFlashInfo->Buffer
                   );
      } else {
        Status = EFI_ACCESS_DENIED;
      }
      break;

    case SUBFUNCTION_WRITE_FLASH: 
      if(IsSectorDataVerified(gSmiFlashInfo->Offset, gSmiFlashInfo->Size, gSmiFlashInfo->Buffer)){      
        Status = WriteFlash (
                   gSmiFlashInfo->Offset,
                   gSmiFlashInfo->Size,
                   (UINTN)gSmiFlashInfo->Buffer
                   );
      } else {
        Status = EFI_ACCESS_DENIED;
      }   
      break;

    case SUBFUNCTION_ERASE_FLASH: 
      if(IsSectorDataVerified(gSmiFlashInfo->Offset, gSmiFlashInfo->Size, gSmiFlashInfo->Buffer)){    
        if (gSmiFlashInfo->Offset == (PcdGet32(PcdFlashNvStorageVariableBase) - PcdGet32(PcdFlashAreaBaseAddress))) {
          mIsReserveVariable = TRUE;
          if (PcdGetBool (PcdSmiFlashUpdateReservePassword)) {
            //
            // Get Password before variable region update
            //
            VariableSize = sizeof(SYSTEM_PASSWORD);
            mSmmVariable->SmmGetVariable (
              SYSTEM_PASSWORD_NAME,
              &gEfiSystemPasswordVariableGuid,
              NULL,
              &VariableSize,
              (VOID **)&mSetupPassword
              );
          }
        }       
        Status = EraseFlash (
                   gSmiFlashInfo->Offset,
                   gSmiFlashInfo->Size
                   );
      } else {
        Status = EFI_ACCESS_DENIED;
      }      
      break;
    
    case SUBFUNCTION_OA3_UPDATE:
      {
        UINT32                    RomHoleSize;
        UINT32                    RomHoleBase;
        UINT32                    RomHoleBaseAlign;
        VOID                      *Buffer;
        
        RomHoleSize = PcdGet32(PcdOa3HoleSize);
        if(gSmiFlashInfo->Size != sizeof(EFI_ACPI_MSDM_DATA_STRUCTURE)){
          Status = EFI_INVALID_PARAMETER;
          break;
        }

        RomHoleBase = PcdGet32(PcdOa3HoleBase);
        if (RomHoleBase % SIZE_4KB != 0) {
          //
          // if not SIZE_4KB Alignment, we should make sure RomHoleBase + RomHoleSize < RomHoleBaseAlign + SIZE_4KB
          //
          RomHoleBaseAlign = RomHoleBase & ~(SIZE_4KB -1);
          if (RomHoleBase + RomHoleSize > RomHoleBaseAlign + SIZE_4KB) {
            DEBUG((EFI_D_INFO, "RomHoleBase = 0x%x, unsupported\n", RomHoleBase));
            Status = EFI_UNSUPPORTED;
            break;
          }
        } else {
          RomHoleBaseAlign = RomHoleBase;
        }
        
        if(PcdGetBool(PcdSmiFlashOa3AcpiTableSupport)){     
          Status = UpdateOA30TableToMem((EFI_ACPI_MSDM_DATA_STRUCTURE*)(UINTN)gSmiFlashInfo->Buffer);
          DEBUG((DEBUG_VERBOSE, "UpdateOa30ToMem: %r\n", Status));
        } else {
          Status = EFI_NOT_FOUND;
        }
        if(!EFI_ERROR(Status) || Status == EFI_NOT_FOUND) {
          if (mMediaAccess == NULL) {
            Status = EFI_UNSUPPORTED;
            break;
          }

          Buffer = AllocatePool(SIZE_4KB);
          //
          // save original data
          //
          gSmiFlashInfo->Size = SIZE_4KB;
          Status = mMediaAccess->Read(mMediaAccess,
                                    RomHoleBaseAlign,
                                    Buffer,
                                    (UINTN*)&gSmiFlashInfo->Size,
                                    SPI_MEDIA_TYPE
                                    );
          if(EFI_ERROR(Status)){
            DEBUG((EFI_D_INFO, "Read:%r\n", Status));
            break;
          }

          Status = mMediaAccess->Erase(
                                  mMediaAccess,
                                  RomHoleBaseAlign,
                                  SIZE_4KB,
                                  SPI_MEDIA_TYPE
                                  );
          DEBUG((DEBUG_VERBOSE, "Erase:%r\n", Status));
          if(!EFI_ERROR(Status)){
            CopyMem((UINT8*)Buffer + (RomHoleBase - RomHoleBaseAlign), gSmiFlashInfo->Buffer, RomHoleSize);
            Status = mMediaAccess->Write(
                                    mMediaAccess,
                                    RomHoleBaseAlign,
                                    Buffer,
                                    SIZE_4KB,
                                    SPI_MEDIA_TYPE
                                    );
            DEBUG((DEBUG_VERBOSE, " Write:%r\n", Status));
          }
        } else if(Status == EFI_ALREADY_STARTED){
          Status = EFI_SUCCESS;
        }
      }
      break;


    case SUBFUNCTION_WRITECMOS:
      CmosData = (UINT8)(gSmiFlashInfo->Size & 0xF);
      //todo liming check
      WriteCheckedCmosByte(PcdGet8 (PcdRecoveryStepCmosOffset), CmosData);
      DEBUG((DEBUG_VERBOSE, "Cmos:%X\n", CmosData));
      Status = EFI_SUCCESS;
      break;

    case SUBFUNCTION_CHECK_BIOS_LOCK:
      Status = HandleIfGetLockStatus((UINTN*)gSmiFlashInfo->Buffer);
      break; 

    case SUBFUNCTION_PATCH_FOR_USB:
      Status = EFI_SUCCESS;
      break;
      
    case SUBFUNCTION_READ_FLASH_BLOCK:
      if (mMediaAccess == NULL) {
        Status = EFI_UNSUPPORTED;
        break;
      }
      DEBUG((DEBUG_VERBOSE, "Offset: 0x%X, Size:0x%X\n", gSmiFlashInfo->Offset, gSmiFlashInfo->Size));
      if(gSmiFlashInfo->Offset + gSmiFlashInfo->Size > PcdGet32(PcdFlashAreaSize)){
        Status = EFI_INVALID_PARAMETER;
        break;
      }
      Status = mMediaAccess->Read (
                               mMediaAccess,
                               PcdGet32(PcdFlashAreaBaseAddress) + gSmiFlashInfo->Offset,
                               gSmiFlashInfo->Buffer,
                               (UINTN*)&gSmiFlashInfo->Size,
                               SPI_MEDIA_TYPE
                               );
      break;


    case SUBFUNCTION_CHECK_BIOS_ID:
      Status = CheckBiosId((BIOS_ID_IMAGE*)(UINTN) gSmiFlashInfo->Buffer);
      DEBUG ((DEBUG_VERBOSE, "CheckBiosId: %r\n",Status));
      break;
      
    case SUBFUNCTION_CHECK_BIOS_INFO:
      Status = EFI_SUCCESS;   // CheckBiosInfo((MY_BYO_BIOS_INFO2_TMP*)(UINTN) gSmiFlashInfo->Buffer);
      DEBUG ((DEBUG_VERBOSE, "CheckBiosInfo: %r\n",Status));
      break;

// wz191108 + >>
    case SUBFUNCTION_IF_UPDATE_UCODE:
      Status = HandleUcodeRequest((ROM_HOLE_PARAMETER*)(UINTN) gSmiFlashInfo->Buffer);
      break;
// wz191108 + <<

    case SUBFUNCTION_IF_UNLOCK:
      Status = HandleIfUnlockRequest((ROM_HOLE_PARAMETER*)(UINTN) gSmiFlashInfo->Buffer);
      break;      

    case SUBFUNCTION_IF_LOCK:
      Status = HandleIfLockRequest();
      break;

    case SUBFUNCTION_IF_REMOVE_LOGO:
      Status = EraseFlash (
                 PcdGet32(PcdFlashNvLogoBase) - PcdGet32(PcdFlashAreaBaseAddress), 
                 PcdGet32(PcdFlashNvLogoSize)
                 );
      break;

    case SUBFUNCTION_IF_RST_PASSWD:
      Status = HandleIfResetPasswordRequest();
      break;

    case SUBFUNCTION_GET_BIOS_VALUE:
      VariableParam = (UPDATE_VALUE_PARAMETER *)(UINTN) gSmiFlashInfo->Buffer;
      Status = UniToolGetValueOffset (VariableParam);
      break;

    case SUBFUNCTION_SET_BIOS_VALUE:
      VariableParam = (UPDATE_VALUE_PARAMETER *)(UINTN) gSmiFlashInfo->Buffer;
      Status = UniToolSetValueOffset (VariableParam);
      break;
    case SUBFUNCTION_SET_BIOS_VALUE_GROUP:
      Status = SetSetupValueGroup((UPDATE_VALUE_PARAMETER *)(UINTN) gSmiFlashInfo->Buffer, gSmiFlashInfo->Offset, gSmiFlashInfo->Size);
      break;
    case SUBFUNCTION_GET_BIOS_VALUE_GROUP:
      Status = GetSetupValueGroup((UPDATE_VALUE_PARAMETER *)(UINTN) gSmiFlashInfo->Buffer, gSmiFlashInfo->Offset);
      break;
    case SUBFUNCTION_GET_SETUP_ITEMS_NAME:
      Status = GetSetupItemsName(gSmiFlashInfo->Offset, gSmiFlashInfo->Size, gSmiFlashInfo->Buffer);
      break;

    case SUBFUNCTION_GET_SETUP_OPTIONS:
      Status = GetSetupItemsOption(gSmiFlashInfo->Offset, gSmiFlashInfo->Size, gSmiFlashInfo->Buffer);
      break;
    case SUBFUNCTION_GET_SETUP_ITEMS_STRING:
      Status = GetSetupItemsNameString(gSmiFlashInfo->Offset, gSmiFlashInfo->Size, gSmiFlashInfo->Buffer);
      break;
    case SUBFUNCTION_GET_VARIABLE_DATA:
      Status = GetVariableByBuffer(gSmiFlashInfo->Buffer);
      break;
    case SUBFUNCTION_SET_VARIABLE_DATA:
      Status = SetVariableByBuffer(gSmiFlashInfo->Buffer);
      break;
    case SUBFUNCTION_GET_SETUP_ONEOF_OPTIONS:
      Status = GetSetupItemsOneofString(gSmiFlashInfo->Offset, gSmiFlashInfo->Size, gSmiFlashInfo->Buffer);
      break;
    case SUBFUNCTION_QUERY_PASSWORD:
    case SUBFUNCTION_QUERY_PASSWORD_SET:
      Status = QuerySetupPassWordHash(NULL);
      break;
    case SUBFUNCTION_SET_ADMIN_PASSWORD:
      Status = SetPassWordHash(ADMIN_PASSWORD, gSmiFlashInfo->Buffer);
      break;
    case SUBFUNCTION_SET_POWERON_PASSWORD:
      Status = SetPassWordHash(POWERON_PASSWORD, gSmiFlashInfo->Buffer);
      break;
    case SUBFUNCTION_VERIFY_PASSWORD:
      Status = VerifySetupPassword(gSmiFlashInfo->Buffer);
      break;
    case SUBFUNCTION_PLATFORM_SUPPORTED:
      Status = 0;
      if(mMeAreaSize > 0) {
        Status |= SUPPORT_FD_AREA_TYPE_ME;
      }
      if(miRCAreaSize > 0) {
        Status |= SUPPORT_FD_AREA_TYPE_IRC;
      }
      if(mECAreaSize > 0) {
        Status |= SUPPORT_FD_AREA_TYPE_EC;
      }
      if(mGBEAreaSize > 0) {
        Status |= SUPPORT_FD_AREA_TYPE_GBE;
      }
      if(mNISAreaSize > 0) {
        Status |= SUPPORT_FD_AREA_TYPE_NIS;
      }
      if (PcdGet32(PcdFlashNvStorageMicrocodeSize) > 0) {
        Status |= SUPPORT_FD_AREA_TYPE_MICROCODE;
      }
      if (PcdGet32(PcdOa3HoleSize) > 0) {
        Status |= SUPPORT_FD_AREA_TYPE_OA3;
      }
      if (PcdGet32(PcdFlashNvLogoSize) > 0) {
        Status |= SUPPORT_FD_AREA_TYPE_LOGO;
      }
      if (PcdGetBool(PcdCapsuleOnDiskSupport) && (mBootMode == BIOS_BOOT_UEFI_OS)){
        Status |= SUPPORT_FD_AREA_TYPE_CAP;
      }
      
      break;
    case SUBFUNCTION_IF_SIGN_UPDATE:
      if (!PcdGetBool (PcdByoSecureFlashSupport)) {
        Status = EFI_UNSUPPORTED;
        break;
      }
      Status = BiosFileHashUpdate (
                 gSmiFlashInfo->Offset, 
                 gSmiFlashInfo->Size, 
                 gSmiFlashInfo->Buffer
                 );      
      break;

    case SUBFUNCTION_IF_SIGN_VERIFY:
      if (!PcdGetBool (PcdByoSecureFlashSupport)) {
        Status = EFI_UNSUPPORTED;
        break;
      }
      Status = BiosFileSignVerify();
      break;

    case SUBFUNCTION_IF_SIGN_PREPARE:
      if (!PcdGetBool (PcdByoSecureFlashSupport)) {
        Status = EFI_UNSUPPORTED;
        break;
      }
      {
        SIGN_INIT_DATA   *SignInitData;
        Status = EFI_SUCCESS;
        SignInitData = (SIGN_INIT_DATA*)gSmiFlashInfo->Buffer;
        if(SignInitData->SignSize != 256 ||
          gSmiFlashInfo->Size != sizeof(SIGN_INIT_DATA) + SignInitData->SignSize - 1){
          Status = EFI_INVALID_PARAMETER;
        }
        if(!EFI_ERROR(Status)){
          CopyMem(gBiosSignCtx.SignData, SignInitData->SignData, SignInitData->SignSize);
          gBiosSignCtx.HashType = SignInitData->HashType;      

          if(gBiosSignCtx.Init){
            FreePool(gBiosSignCtx.HashCtx);
          }
          if(gBiosSignCtx.HashType == SIGN_INIT_HASH_TYPE_SHA512){
            gBiosSignCtx.CtxSize = Sha512GetContextSize();
          } else {
            gBiosSignCtx.CtxSize = Sha256GetContextSize();
          }
          gBiosSignCtx.HashCtx = AllocatePool(gBiosSignCtx.CtxSize);
          ASSERT(gBiosSignCtx.HashCtx != NULL);
          if(gBiosSignCtx.HashType == SIGN_INIT_HASH_TYPE_SHA512){
            Sha512Init(gBiosSignCtx.HashCtx);
          } else {
            Sha256Init(gBiosSignCtx.HashCtx);
          }
          gBiosSignCtx.SignFvOffset = SignInitData->SignFvOffset;
          gBiosSignCtx.SignMethod   = SignInitData->SignMethod;
          gBiosSignCtx.Init = TRUE;
        }
      }
      break;

    case SUBFUNCTION_SET_VARIABLE_INFO:
      SETUP_UPDATE_DATA_INFO   *TempDataInfo = (SETUP_UPDATE_DATA_INFO *)gSmiFlashInfo->Buffer;
      switch (TempDataInfo->UpdateType){
        case SETUP_UPDATE_TYPE_MIN_VALUE:
          Status = UpdateMinValue(TempDataInfo->VariableName, &TempDataInfo->VendorGuid, TempDataInfo->Offset, TempDataInfo->UpdateValue[0]);
          break;
        
        case SETUP_UPDATE_TYPE_MAX_VALUE:
          Status = UpdateMaxValue(TempDataInfo->VariableName, &TempDataInfo->VendorGuid, TempDataInfo->Offset, TempDataInfo->UpdateValue[0]);
          break;
        
        case SETUP_UPDATE_TYPE_DEAULT_VALUE:
          Status = UpdateDefaultValue(TempDataInfo->VariableName, &TempDataInfo->VendorGuid, TempDataInfo->Offset, TempDataInfo->UpdateValue[0]);
          break;
        
        case SETUP_UPDATE_TYPE_OPTION_VALUE:
          Status = UpdateOptionValue(TempDataInfo->VariableName, &TempDataInfo->VendorGuid, TempDataInfo->Offset, TempDataInfo->UpdateValueCount, TempDataInfo->UpdateValue);
          break;
        
        default:
          break;
      }
      break;
    
    case SUBFUNCTION_CHECK_VARIABLE_VALID:
      Status = CheckVariableValid((CHECK_VALUE_PARAMETER *)gSmiFlashInfo->Buffer);
      break;
    case SUBFUNCTION_CHECK_AC:
      Status = EFI_SUCCESS;
      break;
    default:
      Status = RETURN_INVALID_PARAMETER;
      break;
  }

ProcExit:
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "RC:%r\n", Status));
  }

// error in AH.
  gSmiFlashInfo->StatusCode = Status;
//  ASSERT_EFI_ERROR (Status);

  return Status;
}


EFI_STATUS
EFIAPI
SmmNvMediaAccessProtocolNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS                    Status;
  VOID                          *pInterface;

  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmNvMediaAccessProtocolGuid,
                  NULL,
                  (VOID**)&pInterface
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mBiosAreaBaseAddress  = PcdGet32(PcdBiosBase);
  mBiosAreaSize         = PcdGet32(PcdBiosSize);
  mMeAreaBaseAddress    = PcdGet32(PcdMeBase);
  mMeAreaSize           = PcdGet32(PcdMeSize);
  miRCAreaBaseAddress    = PcdGet32(PcdiRCBase);
  miRCAreaSize           = PcdGet32(PcdiRCSize);
  mNISAreaBaseAddress    = PcdGet32(PcdNISBase);
  mNISAreaSize           = PcdGet32(PcdNISSize);
  mECAreaBaseAddress    = PcdGet32(PcdECBase);
  mECAreaSize           = PcdGet32(PcdECSize);
  mGBEAreaBaseAddress    = PcdGet32(PcdGBEBase);
  mGBEAreaSize           = PcdGet32(PcdGBESize);  

  mMediaAccess = (NV_MEDIA_ACCESS_PROTOCOL*)pInterface;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PnpSmbiosProtocolNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS                    Status;
  VOID                          *pInterface;

  Status = gSmst->SmmLocateProtocol (
                  &gEfiPnpSmbiosProtocolGuid,
                  NULL,
                  (VOID**)&pInterface
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mPnpSmbiosProtocol = (EFI_PNP_SMBIOS_PROTOCOL*)pInterface;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ByoCommSmiSvcProtocolNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS                    Status;
  VOID                          *pInterface;

  Status = gSmst->SmmLocateProtocol (
                  &gByoCommSmiSvcProtocolGuid,
                  NULL,
                  (VOID**)&pInterface
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ByoCommSmiSvc = (BYO_COMM_SMI_SVC_PROTOCOL*)pInterface;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmExitBootServicesProtocolNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  mBootMode = BIOS_BOOT_UEFI_OS;
  DEBUG((EFI_D_ERROR, "Set BootMode:%x\n",mBootMode));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmLegacyBootProtocolNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  mBootMode = BIOS_BOOT_LEGACY_OS;
  DEBUG((EFI_D_ERROR, "Set BootMode:%x\n",mBootMode));
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmmEndOfDxeEventNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  gBiosIdVersionCheck = PcdGetBool(PcdBiosIdVersionCheck);
  mPasswordComplexity = PcdGet8(PcdPasswordComplexity);
  return EFI_SUCCESS;
}

VOID
GetBiosInfoData()
{
  MY_BYO_BIOS_INFO2_TMP           *Info;
  UINTN                           Index;
  VOID                            *p;
  EFI_STATUS                      Status;
  UINTN                           Size;
  UINT8                           *BiosInfoFile = NULL;
  UINTN                           BiosInfoFileSize;

  Status = LibReadFileFromFv (gBS, &gBiosInfoFileGuid, EFI_SECTION_ALL, 0, (VOID **)&BiosInfoFile, &BiosInfoFileSize);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "BiosInfo not found\n"));
    goto ProcExit;
  }
  Status = EFI_NOT_FOUND;
  for(Index=0;Index<BiosInfoFileSize;Index+=16){
    Info = (MY_BYO_BIOS_INFO2_TMP*)&(BiosInfoFile[Index]);
    if(Info->Header.Signature == BYO_BIOS_INFO2_SIGNATURE){
      Size = Info->Header.HeaderLength + Info->Header.Step_NUM * sizeof(FLASH_STEP);
      p = AllocatePool(Size);
      ASSERT(p!=NULL);
      CopyMem(p, Info, Size);
      gBiosInfo = (MY_BYO_BIOS_INFO2_TMP*)p;
      Status = EFI_SUCCESS;
      DEBUG((EFI_D_INFO, "BiosInfo found\n"));
      break;
    }
  }

ProcExit:
  if(BiosInfoFile != NULL){
    FreePool (BiosInfoFile);
  }
}

/**
  Software SMI handler that is called when the EnterSetup protocol is installed.
  This function installs the EnterSetup Protocol so SMM Drivers are informed that
  platform code are in Setup state

  @param  DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param  Context         Points to an optional handler context which was specified when the handler was registered.
  @param  CommBuffer      A pointer to a collection of data in memory that will
                          be conveyed from a non-SMM environment into an SMM environment.
  @param  CommBufferSize  The size of the CommBuffer.

  @return Status Code

**/
EFI_STATUS
EFIAPI
SmmEnterSetupHandler (
  IN     EFI_HANDLE  DispatchHandle,
  IN     CONST VOID  *Context,        OPTIONAL
  IN OUT VOID        *CommBuffer,     OPTIONAL
  IN OUT UINTN       *CommBufferSize  OPTIONAL
  )
{
  EFI_HANDLE                    Handle;
  EFI_STATUS                    Status;

  DEBUG((EFI_D_INFO, "SmmEnterSetupHandler\n"));

  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gEfiSetupEnterGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SmiFlashEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL *SwDispatch = NULL;
  EFI_SMM_SW_REGISTER_CONTEXT   SwContext;
  EFI_HANDLE                    Handle;
  UINT8                         *Data8;
  VOID                          *Registration;
  BIOS_ID_INFO                  BiosIdInfo;

  DEBUG((EFI_D_INFO, "SmiFlashEntryPoint\n"));

  Status = GetBiosIdInfoFromHob (&BiosIdInfo);
  if (!EFI_ERROR (Status)) {
    CopyMem (&gBiosIdImage, &BiosIdInfo.BiosId, sizeof (BIOS_ID_IMAGE));
  } else {
    mBiosIdInfoHobStatus = 1;
  }

  if (PcdGetBool(PcdByoSecureFlashSupport)) {
    Status = BiosSignPubKeyInit();
    ASSERT_EFI_ERROR (Status);

    {
      UINTN HashArraySize = PcdGet32(PcdFlashAreaSize)/SIZE_4KB*32;
      gBiosSignCtx.HashArray = AllocatePages(EFI_SIZE_TO_PAGES(HashArraySize));
      ASSERT(gBiosSignCtx.HashArray != NULL);
    }
  }

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmCpuProtocolGuid,
                    NULL,
                    (VOID**)&mSmmCpu
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmSwDispatch2ProtocolGuid,
                    NULL,
                    (VOID**)&SwDispatch
                    );
  ASSERT_EFI_ERROR (Status);
     
  SwContext.SwSmiInputValue = SW_SMI_FLASH_SERVICES;
  Status = SwDispatch->Register (
                         SwDispatch,
                         FlashInterface,
                         &SwContext,
                         &Handle
                         );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmiHandlerRegister (
                    SmmEnterSetupHandler,
                    &gEfiSetupEnterGuid,
                    &Handle
                    );
  ASSERT_EFI_ERROR (Status);

  gResvPage = AllocateReservedZeroMemoryBelow4G(gBS, SIZE_4KB);
  ASSERT(gResvPage != NULL);


  if(PcdGetBool(PcdSmiFlashOa3AcpiTableSupport)){
    CheckOa3();
    Status = EfiGetSystemConfigurationTable (
               &gEfiAcpiTableGuid, 
               (VOID **)&gRsdp3
               );
    ASSERT_EFI_ERROR (Status);
  }
  
  mDisableKBCtrlAltDelReset = (UINT8*)(UINTN)PcdGet64(PcdDisableKBCtrlAltDelResetDataPtr);
  if(mDisableKBCtrlAltDelReset == NULL){
    Status = gBS->AllocatePool(
                    EfiReservedMemoryType,
                    sizeof(UINT8),
                    (VOID**)&Data8
                    );
    ASSERT(!EFI_ERROR(Status));
    *Data8 = 0;
    PcdSet64S(PcdDisableKBCtrlAltDelResetDataPtr, (UINTN)Data8);
    mDisableKBCtrlAltDelReset = Data8;
  }

  //
  // Register SmmReadyToLock notification.
  //
  Registration = NULL;
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmNvMediaAccessProtocolGuid,
                    SmmNvMediaAccessProtocolNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR (Status);
  SmmNvMediaAccessProtocolNotify(NULL, NULL, NULL);

  Registration = NULL;
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiPnpSmbiosProtocolGuid,
                    PnpSmbiosProtocolNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR (Status);
  PnpSmbiosProtocolNotify(NULL, NULL, NULL);

  Registration = NULL;
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gByoCommSmiSvcProtocolGuid,
                    ByoCommSmiSvcProtocolNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR (Status);
  ByoCommSmiSvcProtocolNotify(NULL, NULL, NULL);

  //
  // Register SmmExitBootServices and SmmLegacyBoot notification.
  //
  Registration = NULL;
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEdkiiSmmExitBootServicesProtocolGuid,
                    SmmExitBootServicesProtocolNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR (Status);

  Registration = NULL;
  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEdkiiSmmLegacyBootProtocolGuid,
                    SmmLegacyBootProtocolNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmEndOfDxeProtocolGuid,
                    SmmEndOfDxeEventNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR (Status);

  gSmiFlashInfo = (SMI_INFO *)(UINT64)LibGetSmiInfoBufferAddr(gBS);
  DEBUG((EFI_D_INFO, "smiflash gSmiFlashInfo:%p\n",gSmiFlashInfo));

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID**)&mSmmVariable
                    );
  ASSERT_EFI_ERROR (Status);

  //
  // Locate SETUP Database
  //
  LocatSetupItemsDb ();

  DEBUG((EFI_D_INFO, "UpdateVariableDefault "));
  Status = UpdateVariableDefault();
  if (!EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "success\n"));
  } else {
    DEBUG((EFI_D_INFO, "failed\n"));
  }

  //
  // Get BIOS INFO
  //
  GetBiosInfoData ();

  Handle = NULL;
  Status = gBS->InstallProtocolInterface(&Handle, &gEfiCallerIdGuid, EFI_NATIVE_INTERFACE, NULL);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

