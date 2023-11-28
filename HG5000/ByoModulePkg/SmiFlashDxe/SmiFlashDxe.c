/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
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

#include <Guid/CapsuleVendor.h>
#include <Guid/CapsuleRecord.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <ByoBiosInfo2.h>
#include <ByoSmiFlashInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <Library/ByoCommLib.h>
#include <Guid/RecoveryDevice.h>
#include <Pi/PiBootMode.h>
#include <Library/HobLib.h>
#include <ByoStatusCode.h>
#include <Library/ReportStatusCodeLib.h>


//---------------------------------------------------------------------------
UINT32
SMI_CALL (
    UINT8      SwVal,
    UINT8      Function,
    UINT64     ParamAddr
);

typedef struct {
  UINT32                 Signature;
  BYO_SMIFLASH_PROTOCOL  SmiFlash;
  UINT8                  *Paramter;
  UINT8                  *Buffer;
  UINT32                 BufferSize;
} SMI_FLASH_CTX;



#define FVMAIN_OFFSET              (PcdGet32(PcdFlashFvMainBase) - PcdGet32(PcdFlashAreaBaseAddress))
#define SMIFLASH_CTX_SIGN          SIGNATURE_32('_', 'S', 'F', '_')
#define SMIFLASH_CTX_FROM_THIS(a)  CR(a, SMI_FLASH_CTX, SmiFlash, SMIFLASH_CTX_SIGN)

extern EFI_GUID gByoFvMainGuid;

UINT16 gSmmPort = _PCD_VALUE_PcdSwSmiCmdPort;

#pragma pack(1)

typedef enum {
  VERIFY_INIT = 1,
  VERIFY_UPDATE,
  VERIFY_FINAL
} BIOS_VERIFY_SUBFUN;

typedef struct {
  UINT32  Offset;
  UINT32  Length;
} SIGN_RANGE;


typedef struct {
  UINT8                SubFun;
  EFI_PHYSICAL_ADDRESS Buffer;
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



EFI_STATUS
VerifySignBios (
  IN BYO_SMIFLASH_PROTOCOL		   *This,
  IN UINT8   *Buffer,
  IN UINTN   CapsuleSize
)
{
  EFI_STATUS                  Status;
  EFI_PHYSICAL_ADDRESS        BiosBuffer;
  BIOS_VERIFY_PARAMETER       *BiosVerifyPtr;
  BIOS_VERIFY_CAPSULE_BUFFER  *CapsuleBuffer;
  BIOS_VERIFY_CAPSULE_BUFFER  TmpCapsuleBuffer;
  SIGN_RANGE                  SignRange;
  UINT32                      RangeOffset;
  UINT32                      RangeIndex;
  UINT32                      Index;

  Status        = EFI_SUCCESS;
  CapsuleBuffer = (BIOS_VERIFY_CAPSULE_BUFFER *)(UINTN)(Buffer + 0x50); // skip first fv header & align 16
  CopyMem(&TmpCapsuleBuffer, CapsuleBuffer, sizeof(BIOS_VERIFY_CAPSULE_BUFFER));
  
  BiosBuffer = (EFI_PHYSICAL_ADDRESS)Buffer;
  BiosVerifyPtr = (BIOS_VERIFY_PARAMETER*)AllocateZeroPool(sizeof(BIOS_VERIFY_PARAMETER));  
  BiosVerifyPtr->SubFun = VERIFY_INIT;
  BiosVerifyPtr->Buffer = (EFI_PHYSICAL_ADDRESS)CapsuleBuffer;
  

  Status = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_BIOS_VERIFY, (UINTN)BiosVerifyPtr);
  
  Status = (Status >> 8) & 0xFF;

  if (Status == EFI_SUCCESS) {
    BiosVerifyPtr->SubFun = VERIFY_UPDATE;
    RangeOffset = ALIGN_VALUE(TmpCapsuleBuffer.PubkeySize, 4) + ALIGN_VALUE(TmpCapsuleBuffer.SignSize, 4);
    RangeIndex = TmpCapsuleBuffer.RangeArraySize / sizeof(SIGN_RANGE);
    for (Index = 0; Index < RangeIndex; Index ++) {
		
		CopyMem(&SignRange, &CapsuleBuffer->Data[RangeOffset], sizeof(SIGN_RANGE));
        BiosVerifyPtr->Buffer = (EFI_PHYSICAL_ADDRESS)(BiosBuffer + SignRange.Offset);
        BiosVerifyPtr->Size = SignRange.Length;
        Status = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_BIOS_VERIFY, (UINTN)BiosVerifyPtr);
      	RangeOffset += sizeof(SIGN_RANGE);
    }
	Status = (Status >> 8) & 0xFF;
    if (Status == EFI_SUCCESS) {
      	BiosVerifyPtr->SubFun = VERIFY_FINAL;
        BiosVerifyPtr->Buffer = (EFI_PHYSICAL_ADDRESS)CapsuleBuffer;
      Status = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_BIOS_VERIFY, (UINTN)BiosVerifyPtr);
	  Status = (Status >> 8) & 0xFF;
    }
  } else if ((Status & 0xFF) == (EFI_INVALID_PARAMETER & 0xFF)){
    Status = EFI_SUCCESS;
  }

  return Status;
}


EFI_STATUS
ByoSmiFlashVerifySign(
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT8					 		 *NewBios,
  IN  UINTN					 		 NewBiosSize
 )
{
	EFI_STATUS			  Status;
	Status = EFI_SUCCESS;
	Status = VerifySignBios (This, NewBios, NewBiosSize);
	return Status;
}



EFI_STATUS
ByoSmiFlashSaveSetupPassword(
  IN  BYO_SMIFLASH_PROTOCOL          *This
 )
{
	EFI_STATUS			  Status;
	
	Status = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_CHECK_FOR_PASSWORD, 0);
	return Status;
}

VOID
ByoSmiFlashFirstPowerOnAfterFlashSpi(
  IN  BYO_SMIFLASH_PROTOCOL          *This
 )
{
	EFI_STATUS			  Status;
	Status = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_SET_FOR_FIRST_POWERON, 0);
}


EFI_STATUS
ByoSmiFlashProgramFlash (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize
  )
{
  BIOS_UPDATE_BLOCK_PARAMETER  *p;
  UINT32                       Rc;
  SMI_FLASH_CTX                *Private;  

  
  Private = SMIFLASH_CTX_FROM_THIS(This);
  
  if(DataSize > Private->BufferSize || DataSize == 0){
    return EFI_INVALID_PARAMETER;
  }
  
  p = (BIOS_UPDATE_BLOCK_PARAMETER*)Private->Paramter;
  p->Buffer = (UINTN)Private->Buffer;
  p->Offset = Offset;
  p->Size   = DataSize;
  CopyMem(Private->Buffer, Data, DataSize);

  Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_PROGRAM_FLASH, (UINTN)p);
  Rc = (Rc >> 8) & 0xFF;

  if(Rc == 0){
    return EFI_SUCCESS;
  } else {
    return ENCODE_ERROR(Rc);
  }
}


EFI_STATUS 
ByoSmiFlashUpdateStep (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Step
  )
{
  BIOS_UPDATE_BLOCK_PARAMETER  *p;
  UINT32                       Rc;
  SMI_FLASH_CTX                *Private;


  Private = SMIFLASH_CTX_FROM_THIS(This);
  
  p = (BIOS_UPDATE_BLOCK_PARAMETER*)Private->Paramter;
  p->Buffer = (UINTN)Private->Buffer;
  p->Offset = 0;
  p->Size   = Step;

  Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_WRITECMOS, (UINTN)p);
  Rc = (Rc >> 8) & 0xFF;

  if(Rc == 0){
    return EFI_SUCCESS;
  } else {
    return ENCODE_ERROR(Rc);
  }
}


EFI_STATUS
ByoSmiFlashGetBiosInfo2 (
  IN  BYO_SMIFLASH_PROTOCOL   *This,
  IN  UINT8                   *NewBios,
  IN  UINTN                   NewBiosSize,
  OUT VOID                    **BiosInfo
  )
{
  MY_BYO_BIOS_INFO2_TMP           *Info;
  EFI_FIRMWARE_VOLUME_HEADER      *FvHdr;
  EFI_FIRMWARE_VOLUME_EXT_HEADER  *FvExtHdr;
  UINTN          Index;
  VOID           *p;
  EFI_STATUS     Status;
  UINT8          *FvMain;
  UINTN          Size;
  

  FvMain = NewBios + FVMAIN_OFFSET;
  FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)FvMain;
  FvExtHdr = (EFI_FIRMWARE_VOLUME_EXT_HEADER*)(FvMain + FvHdr->ExtHeaderOffset);
  
  if(FvHdr->Signature != EFI_FVH_SIGNATURE || 
     FvHdr->FvLength != PcdGet32(PcdFlashFvMainSize) ||
     FvHdr->ExtHeaderOffset == 0 ||
     !CompareGuid(&FvExtHdr->FvName, &gByoFvMainGuid)){
    DEBUG((EFI_D_INFO, "FvMain changed\n"));
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  Status = EFI_NOT_FOUND;
  for(Index=0;Index<SIZE_1KB;Index+=16){
    Info = (MY_BYO_BIOS_INFO2_TMP*)&FvMain[Index];
    if(Info->Header.Signature == BYO_BIOS_INFO2_SIGNATURE){
      Size = Info->Header.HeaderLength + Info->Header.Step_NUM * sizeof(FLASH_STEP);
      p = AllocatePool(Size);
      ASSERT(p!=NULL);
      CopyMem(p, Info, Size);
      *BiosInfo = (MY_BYO_BIOS_INFO2_TMP*)p;
      Status = EFI_SUCCESS;
      break;
    }      
  }

ProcExit:
  return Status;
}


EFI_STATUS
ByoSmiFlashPrepareEnv (
  IN  BYO_SMIFLASH_PROTOCOL   *This
  )
{
  UINT32      Rc;

  Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_DISABLE_USB_POWERBUTTON, 0);
  Rc = (Rc >> 8) & 0xFF;

  if(Rc == 0){
    return EFI_SUCCESS;
  } else {
    return ENCODE_ERROR(Rc);
  }
}


EFI_STATUS
ByoSmiFlashClearEnv (
  IN  BYO_SMIFLASH_PROTOCOL   *This
  )
{
  UINT32      Rc;

  DEBUG((EFI_D_INFO, "ClearEnv\n"));

  Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_ENABLE_USB_POWERBUTTON, 0);
  Rc = (Rc >> 8) & 0xFF;

  if(Rc == 0){
    return EFI_SUCCESS;
  } else {
    return ENCODE_ERROR(Rc);
  }
}





EFI_STATUS
ShowCopyRightsAndWarning (
  VOID
)
{
  gST->ConOut->SetAttribute(gST->ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE | EFI_BRIGHT);
  gST->ConOut->ClearScreen (gST->ConOut);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  
  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);

  Print(L"              **************************************************************\n");
  Print(L"              *                   Byosoft Flash Update                     *\n");
  Print(L"              *         Copyright(C) 2006-2021, Byosoft Co.,Ltd.           *\n");
  Print(L"              *                   All rights reserved                      *\n");
  Print(L"              **************************************************************\n\n");
  Print(L"Warning: System is updating flash. Please don't shutdown system during erasing/programming flash!\n");

  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE | EFI_BRIGHT);
  
  return EFI_SUCCESS;
}




EFI_STATUS
ByoSmiFlashBiosCheck (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize,
  OUT VOID                           **pBiosInfo2  OPTIONAL 
  )
{
  EFI_STATUS                   Status;
  BIOS_ID_IMAGE                BiosIdImage;
  UINT32                       Rc;
  SMI_FLASH_CTX                *Private;
  UINT8                        *p;
  MY_BYO_BIOS_INFO2_TMP        *BiosInfo2;   


  if(NewBiosDataSize != PcdGet32(PcdFlashAreaSize)){
    return EFI_UNSUPPORTED;
  }

  Status = GetImageBiosId((UINTN)NewBiosData, NewBiosDataSize, &BiosIdImage);
  if(EFI_ERROR(Status)){
    return Status;
  }

  Private = SMIFLASH_CTX_FROM_THIS(This);

  p = Private->Paramter;
  CopyMem(p, &BiosIdImage, sizeof(BIOS_ID_IMAGE));
  Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_CHECK_BIOS_ID, (UINTN)p);
  Rc = (Rc >> 8) & 0xFF;  
  if(Rc != 0){
    return ENCODE_ERROR(Rc);
  }  

  Status = This->GetBiosInfo2(This, NewBiosData, NewBiosDataSize, &BiosInfo2);
  if(EFI_ERROR(Status)){
    return EFI_UNSUPPORTED;
  }

  if(pBiosInfo2 != NULL){
    *pBiosInfo2 = (VOID*)BiosInfo2;
  }
  
  return EFI_SUCCESS;  
}



EFI_STATUS
ByoSmiFlashDefaultUpdate (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize
  )
{
  MY_BYO_BIOS_INFO2_TMP     *BiosInfo2;  
  EFI_STATUS                Status;
  UINT32                    Index;
  FLASH_STEP                *Step;
  UINT32                    Offset, i;
  UINT32                    LogoAreaSize;
  EFI_BOOT_MODE             BootMode;
 // UINT32                    Size;
  UINT8                     *NVramName=NULL;

  Status = ByoSmiFlashBiosCheck(This, NewBiosData, NewBiosDataSize, &BiosInfo2);
  if(EFI_ERROR(Status)){
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  BootMode = GetBootModeHob();
  ShowCopyRightsAndWarning();
  
  Status = This->VerifySign(This, NewBiosData, NewBiosDataSize);
  if(Status != 0){
	  Print(L"\r BIOS Verify Sign Fail!\n");
	  goto ProcExit;
  }
  
  Print(L"\r BIOS Verify Sign OK!\n");

  This->PrepareEnv(This);

  if (PcdGetBool(PcdUpdateAllBiosEnable)) {
  /*  Size   = PcdGet32(PcdFlashAreaSize);
    for(i = 0; i < Size; i += SIZE_4KB) {
      Print(L"\r%a     (%d%%)", "Update BIOS ........", i*100 / Size);
      This->ProgramFlash (This, i, NewBiosData + i, SIZE_4KB);
    }
    Print(L"\r%a     (%d%%)\n", "Update BIOS ........", 100);*/

    for(Index=0;Index<BiosInfo2->Header.Step_NUM;Index++){
      Step = &BiosInfo2->Step[Index];

      if(Step->Op == OP_CLEAR________){
        Print(L"\r%a     (%d%%)\n", Step->Name, 100);
        This->UpdateStep(This, Step->Step);
        break;
      }
    
      if(Step->Step == FLASH_STEP_BK){
         if(BootMode != BOOT_IN_RECOVERY_MODE){
         continue;
       }
    } 

    if(Step->Op == OP_RESERVE_OEM__ || Step->Op == OP_RESERVE_NVM__ ){
      continue;
    }

    if(Step->Info2.Size == 0){
      continue;
    }


    if(Step->Op == OP_PROGRAM_FLASH){
      for(i=0;i<Step->Info2.Size;i+=SIZE_4KB){
        Offset = Step->Info2.Offset + i;
        Print(L"\r%a     (%d%%)", Step->Name, i*100/Step->Info2.Size);
        This->UpdateStep(This, Step->Step);
        This->ProgramFlash(This, Offset, NewBiosData+Offset, SIZE_4KB);
      }
      Print(L"\r%a     (%d%%)\n", Step->Name, 100);
    } 
  }

    goto UpdateDone;
  }



// program logo.
  LogoAreaSize = PcdGet32(PcdFlashNvLogoSize);
  if(LogoAreaSize && BootMode == BOOT_IN_RECOVERY_MODE){
    Offset = PcdGet32(PcdFlashNvLogoBase) - PcdGet32(PcdFlashAreaBaseAddress);
    for(i=0;i<LogoAreaSize;i+=SIZE_4KB){
      Print(L"\r%a     (%d%%)", "Update Logo ...........", i*100/LogoAreaSize);
      This->ProgramFlash(This, Offset+i, NewBiosData+Offset+i, SIZE_4KB);
    }
    Print(L"\r%a     (%d%%)\n", "Update Logo ...........", 100);
  }

This->SaveSetupPassword(This);

// program others
  for(Index=0;Index<BiosInfo2->Header.Step_NUM;Index++){
    Step = &BiosInfo2->Step[Index];

    if(Step->Op == OP_CLEAR________){
      Print(L"\r%a     (%d%%)\n", Step->Name, 100);
      This->UpdateStep(This, Step->Step);
      break;
    }
    
    if(Step->Step == FLASH_STEP_NV || Step->Step == FLASH_STEP_BK){
       if(BootMode != BOOT_IN_RECOVERY_MODE){
         continue;
       }
    } 

    if(Step->Op == OP_RESERVE_OEM__ || Step->Op == OP_RESERVE_NVM__ ){
      continue;
    }

    if(Step->Info2.Size == 0){
      continue;
    }
  /*  NVramName = "Update FvNvstorage ....";
    if(CompareMem(Step->Name, NVramName, sizeof(Step->Name))){
    Print(L"reserver nvram,skip Nvram\n");
      continue;
    }*/

    if(Step->Op == OP_PROGRAM_FLASH){
      for(i=0;i<Step->Info2.Size;i+=SIZE_4KB){
        Offset = Step->Info2.Offset + i;
        Print(L"\r%a     (%d%%)", Step->Name, i*100/Step->Info2.Size);
        This->UpdateStep(This, Step->Step);
        This->ProgramFlash(This, Offset, NewBiosData+Offset, SIZE_4KB);
      }
      Print(L"\r%a     (%d%%)\n", Step->Name, 100);
    } 
  }
 This->FirstPowerOn(This);
UpdateDone:
  This->ClearEnv(This);

ProcExit:
  return Status;  
}







/**
  Parse device guid then return a media type string for print() function

  @param  DeviceId              Indicate a media type

  @return  CHAR16*              a pointer to media type string

**/
CHAR16*
MediaType (
  IN EFI_GUID           *DeviceId
  )
{
  UINT16           *MediaType;

  MediaType = L"Unkonwn";

  if (CompareGuid (&gRecoveryOnDataCdGuid, DeviceId)) {
    MediaType = L"CDROM  ";
  } else if (CompareGuid (&gRecoveryOnFatFloppyDiskGuid, DeviceId)) {
    MediaType = L"Floppy ";
  } else if (CompareGuid (&gRecoveryOnFatIdeDiskGuid, DeviceId)) {
    MediaType = L"IdeDisk";
  } else if (CompareGuid (&gRecoveryOnFatUsbDiskGuid, DeviceId)) {
    MediaType = L"UsbDisk";
  }

  return MediaType;
}



/**
  Show all bios images recorded in mCapsuleRecord and wait user select one
  of them to finish flash update.

  @return  UINT8                return which one is selected.

**/
UINT8
WaitForSelect(CAPSULE_RECORD *CapRec)
{
  UINT8             Index;
  EFI_INPUT_KEY     Key;
  UINT8             ReturnNumber;
  UINT8             SelectNumber;
  BOOLEAN           LegalKey;
  BOOLEAN           EnterKeyPressed;
  EFI_STATUS        Status;
  

  if (CapRec->CapsuleCount == 1) {
    return CapRec->CapsuleCount;
  }

  Print (L"\rSystem is in Recovery Mode! Total %d BIOS images are found in attached medias\n", CapRec->CapsuleCount);

  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_LIGHTGRAY);
  for (Index = 0; Index < CapRec->CapsuleCount; Index ++) {
    Print(L"[%d] %s   Media: %s   Actived: %s\n", 
            Index+1,
            &CapRec->CapsuleInfo[Index].BiosIdImage.BiosIdString,
            MediaType(&CapRec->CapsuleInfo[Index].DeviceId),
            CapRec->CapsuleInfo[Index].Actived?L"Yes":L"No"
            );
  }
  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE);
  gST->ConOut->EnableCursor (gST->ConOut, TRUE);
  Print (L"\n");
  Print (L"\rPlease enter the number to select one for flash update:");
  //
  // wait legal key pressed
  //
  LegalKey = FALSE;
  EnterKeyPressed = FALSE;
  ReturnNumber = 1;
  ZeroMem (&Key, sizeof(Key));
  do {

    Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key);
    if(EFI_ERROR(Status)){
      gBS->Stall(50000);
      continue;
    }

    SelectNumber = (UINT8)Key.UnicodeChar - 0x30;
    if (SelectNumber > 0 && SelectNumber <= CapRec->CapsuleCount && LegalKey == FALSE) {
      LegalKey = TRUE;
      Print (L"%c", Key.UnicodeChar);
      ReturnNumber = SelectNumber;
    } else if (Key.UnicodeChar == 0x0008 && LegalKey == TRUE) {
      Print (L"%c", Key.UnicodeChar);
      LegalKey = FALSE;
    } else if (Key.UnicodeChar == 0x000D && LegalKey == TRUE) {
      EnterKeyPressed = TRUE;
    }
  } while (!EnterKeyPressed);

  return ReturnNumber;
}






EFI_STATUS
ByoSmiFlashRecoveryUpdate (
  IN  BYO_SMIFLASH_PROTOCOL     *This
  )
{
  EFI_STATUS                    Status;
  UINT8                         *NewBiosData;
  UINTN                         NewBiosDataSize;
  UINT8                         SelectNum;
  EFI_PEI_HOB_POINTERS          GuidHob;
  CAPSULE_RECORD                *CapRec;
  UINTN                         Index;


  GuidHob.Raw = GetFirstGuidHob(&gRecoveryCapsuleRecordGuid);
  if(GuidHob.Raw == NULL){
    return EFI_NOT_FOUND;
  }

  CapRec = (CAPSULE_RECORD*)(GuidHob.Guid+1);

  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK| EFI_WHITE);
  gST->ConOut->ClearScreen (gST->ConOut);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  
  SelectNum = WaitForSelect(CapRec);
  NewBiosData = (UINT8*)(UINTN)CapRec->CapsuleInfo[SelectNum - 1].BaseAddress;
  NewBiosDataSize = (UINTN)CapRec->CapsuleInfo[SelectNum - 1].Length;

  REPORT_STATUS_CODE (EFI_ERROR_CODE, BSC_BIOS_RECOVERY_OCCURRED);  

  Status = This->DefaultUpdate(This, NewBiosData, NewBiosDataSize);
  if (EFI_ERROR (Status)) {
    gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_RED);
    Print(L"Flash Update Error!");
    Print(L"Press power button to Shutdown system...\n");
    CpuDeadLoop();
  } else {
    gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
    Print(L"Flash is updated successfully!\n\n");
    for (Index = 5; Index > 0; Index--) {
      Print (L"\rSystem will reset in %d second(s)", Index);
      gBS->Stall (1000000);
    }
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);    
  }

  return Status;  
}


EFI_STATUS
ByoSmiFlashRead (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize
  )
{
  BIOS_UPDATE_BLOCK_PARAMETER  *p;
  UINT32                       Rc;
  SMI_FLASH_CTX                *Private;
  UINT32                       Index;
  UINT32                       Count;
  UINT32                       Remain;
  
  
  Private = SMIFLASH_CTX_FROM_THIS(This);
  
  if(DataSize == 0){
    return EFI_INVALID_PARAMETER;
  }

  Count = DataSize/Private->BufferSize;
  p = (BIOS_UPDATE_BLOCK_PARAMETER*)Private->Paramter;
  
  for(Index = 0; Index < Count; Index++){
    p->Offset = Offset + Index * Private->BufferSize;
    p->Size   = Private->BufferSize;
    p->Buffer = (UINTN)Private->Buffer;
    Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_READ_FLASH_BLOCK, (UINTN)p);
    Rc = (Rc >> 8) & 0xFF;  
    if(Rc != 0){
      return ENCODE_ERROR(Rc);
    }
    CopyMem((UINT8*)Data + Index * Private->BufferSize, Private->Buffer, Private->BufferSize);
  }

  Remain = DataSize % Private->BufferSize;
  if(Remain){
    p->Offset = Offset + Index * Private->BufferSize;
    p->Size   = Remain;
    p->Buffer = (UINTN)Private->Buffer;
    Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_READ_FLASH_BLOCK, (UINTN)p);
    Rc = (Rc >> 8) & 0xFF; 
    if(Rc != 0){
      return ENCODE_ERROR(Rc);
    }
    CopyMem((UINT8*)Data + Index * Private->BufferSize, Private->Buffer, Remain);    
  }

  return EFI_SUCCESS;
}


EFI_STATUS
ByoSmiFlashWrite (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize
  )
{
  BIOS_UPDATE_BLOCK_PARAMETER  *p;
  UINT32                       Rc;
  SMI_FLASH_CTX                *Private;
  UINT32                       Index;
  UINT32                       Count;
  UINT32                       Remain;
  
  
  Private = SMIFLASH_CTX_FROM_THIS(This);
  
  if(DataSize == 0){
    return EFI_INVALID_PARAMETER;
  }

  Count = DataSize/Private->BufferSize;
  p = (BIOS_UPDATE_BLOCK_PARAMETER*)Private->Paramter;
  
  for(Index = 0; Index < Count; Index++){
    p->Offset = Offset + Index * Private->BufferSize;
    p->Size   = Private->BufferSize;
    p->Buffer = (UINTN)Private->Buffer;
    CopyMem(Private->Buffer, (UINT8*)Data + Index * Private->BufferSize, Private->BufferSize);
    Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_WRITE_FLASH, (UINTN)p);
    Rc = (Rc >> 8) & 0xFF;  
    if(Rc != 0){
      return ENCODE_ERROR(Rc);
    }
  }

  Remain = DataSize % Private->BufferSize;
  if(Remain){
    p->Offset = Offset + Index * Private->BufferSize;
    p->Size   = Remain;
    p->Buffer = (UINTN)Private->Buffer;
    CopyMem(Private->Buffer, (UINT8*)Data + Index * Private->BufferSize, Remain);
    Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_WRITE_FLASH, (UINTN)p);
    Rc = (Rc >> 8) & 0xFF; 
    if(Rc != 0){
      return ENCODE_ERROR(Rc);
    }
  }

  return EFI_SUCCESS;
}



EFI_STATUS
ByoSmiFlashErase (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  UINT32                         DataSize
  )
{
  BIOS_UPDATE_BLOCK_PARAMETER  *p;
  UINT32                       Rc;
  SMI_FLASH_CTX                *Private;
  
  
  Private = SMIFLASH_CTX_FROM_THIS(This);
  Offset = Offset & ~0xFFF;
  DataSize = ALIGN_VALUE(DataSize, SIZE_4KB);
  if(DataSize == 0){
    return EFI_INVALID_PARAMETER;
  }
  
  p = (BIOS_UPDATE_BLOCK_PARAMETER*)Private->Paramter;
  p->Offset = Offset;
  p->Size   = DataSize;
  Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_ERASE_FLASH, (UINTN)p);
  Rc = (Rc >> 8) & 0xFF;  
  if(Rc != 0){
    return ENCODE_ERROR(Rc);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
ByoSmiUpdateSmbios (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UPDATE_SMBIOS_PARAMETER        *SmbiosPtr
  )
{
  VOID             *Ptr;
  UINT32           Rc;
  SMI_FLASH_CTX    *Private;
  
  Private = SMIFLASH_CTX_FROM_THIS(This);
  
  Ptr = Private->Paramter;
  CopyMem(Ptr, SmbiosPtr, sizeof (UPDATE_SMBIOS_PARAMETER) + SmbiosPtr->Parameter.DataLength);
  Rc = SMI_CALL(SW_SMI_FLASH_SERVICES, SUBFUNCTION_UPDATE_SMBIOS_DATA, (UINTN)Ptr);
  Rc = (Rc >> 8) & 0xFF;  
  if(Rc != 0){
    return ENCODE_ERROR(Rc);
  }

  return EFI_SUCCESS;

}







STATIC SMI_FLASH_CTX gPrivate = {
  SMIFLASH_CTX_SIGN,
  {
    ByoSmiFlashVerifySign,
    ByoSmiFlashGetBiosInfo2,
    ByoSmiFlashPrepareEnv,
    ByoSmiFlashClearEnv,
    ByoSmiFlashUpdateStep,
    ByoSmiFlashProgramFlash,
    ByoSmiFlashDefaultUpdate,
    ByoSmiFlashRecoveryUpdate,
    ByoSmiFlashRead,
    ByoSmiFlashErase,    
    ByoSmiFlashWrite,
    ByoSmiUpdateSmbios,
    ByoSmiFlashBiosCheck,
    ByoSmiFlashSaveSetupPassword,
    ByoSmiFlashFirstPowerOnAfterFlashSpi
  },
  NULL,
  NULL
};



EFI_STATUS
SmiFlashDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS            Status;
  UINT8                 *Buffer;


  DEBUG((EFI_D_INFO, "SmiFlashDxe\n"));

  Buffer = (UINT8*)AllocateReservedZeroMemoryBelow4G(gBS, SIZE_4KB + SIZE_4KB);
  if(Buffer == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  gPrivate.Paramter = Buffer;
  gPrivate.Buffer   = Buffer + SIZE_4KB;
  gPrivate.BufferSize = SIZE_4KB;
  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gByoSmiFlashProtocolGuid,
                  &gPrivate.SmiFlash,
                  NULL
                  );
  
ProcExit:  
  return Status;
}

