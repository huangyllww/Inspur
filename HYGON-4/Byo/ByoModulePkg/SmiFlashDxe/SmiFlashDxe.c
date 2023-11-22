/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
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

#include <Pi/PiFirmwareFile.h>
#include <Pi/PiFirmwareVolume.h>
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
#include <Protocol/SetupItemUpdate.h>
#include <Library/ByoCommLib.h>
#include <Guid/RecoveryDevice.h>
#include <Pi/PiBootMode.h>
#include <Library/HobLib.h>
#include <ByoStatusCode.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/BiosIdLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/ByoBiosSignInfoLib.h>
#include <Protocol/Smbios.h>
#include <ByoSmbiosTable.h>
#include <Protocol/SmmCommunication.h>
#include "SecurityUpdate.h"
#include <Guid/ByoSetupFormsetGuid.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/HiiConfigAccess.h>


//---------------------------------------------------------------------------
UINT32
SMI_CALL (
    UINT8      SwVal,
    UINT64     ParamAddr
);

typedef struct {
  UINT32                 Signature;
  BYO_SMIFLASH_PROTOCOL  SmiFlash;
  PROGRESS_CALLBACK      ProgressUpdate; 
  SHOW_PROGRESS_CALLBACK ShowProgress;
  VOID                   *ShowContext;
} SMI_FLASH_CTX;

#define SMIFLASH_CTX_SIGN          SIGNATURE_32('_', 'S', 'F', '_')
#define SMIFLASH_CTX_FROM_THIS(a)  CR(a, SMI_FLASH_CTX, SmiFlash, SMIFLASH_CTX_SIGN)

#define BIOS_FILE_MAX_SIZE         SIZE_128MB

extern SMI_FLASH_CTX gPrivate;

UINT16         gSmmPort = _PCD_VALUE_PcdSwSmiCmdPort;
SMI_INFO       *gSmiFlashInfo = NULL;
EFI_HII_HANDLE  gHiiHandle;
EFI_GUID gByoSetupSecurityUpdateGuid = SETUP_SECURITY_UPDATE_GUID;
extern unsigned char SecurityUpdateBin[];


typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;

HII_VENDOR_DEVICE_PATH  mSecurityupdateHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    FORMSET_GUID_SECURITY
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    { 
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

EFI_STATUS
EFIAPI
ByoSmiFlashProgramFlash (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize
  )
{
  SMI_FLASH_CTX                      *Private;  

  Private = SMIFLASH_CTX_FROM_THIS(This);
  
  if(DataSize > sizeof(gSmiFlashInfo->Buffer) || DataSize == 0){
    return EFI_INVALID_PARAMETER;
  }
  
  gSmiFlashInfo->SubFunction = SUBFUNCTION_PROGRAM_FLASH;
  gSmiFlashInfo->Offset = Offset;
  gSmiFlashInfo->Size   = DataSize;
  CopyMem(gSmiFlashInfo->Buffer, Data, DataSize);
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINTN)gSmiFlashInfo);
  
  return gSmiFlashInfo->StatusCode;
}


EFI_STATUS 
EFIAPI
ByoSmiFlashUpdateStep (
  IN  BYO_SMIFLASH_PROTOCOL    *This,
  IN  UINT32                   Step
  )
{

  gSmiFlashInfo->SubFunction = SUBFUNCTION_WRITECMOS;
  gSmiFlashInfo->Offset = 0;
  gSmiFlashInfo->Size   = Step;
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
  
  return gSmiFlashInfo->StatusCode;
}


EFI_STATUS
GetBiosInfoFromFile (
  OUT VOID           **BiosInfo,
  IN  UINT8          *BiosData,
  IN  UINTN          BiosDataSize
  )
{
  UINTN                           Index = 0;
  UINTN                           SearchEnd;
  EFI_FIRMWARE_VOLUME_HEADER      *FvHdr;
  UINTN                           i;
  MY_BYO_BIOS_INFO2_TMP           *Info;


  if(BiosData == NULL || BiosInfo == NULL || 
     BiosDataSize < PcdGet32(PcdFlashAreaSize) || BiosDataSize > BIOS_FILE_MAX_SIZE){
    return EFI_INVALID_PARAMETER;
  } 

  SearchEnd = BiosDataSize - sizeof(EFI_FIRMWARE_VOLUME_HEADER);
  while(Index < SearchEnd){
    FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)(BiosData + Index);
    if(FvHdr->Signature == EFI_FVH_SIGNATURE && CalculateSum16((UINT16*)FvHdr, FvHdr->HeaderLength) == 0){
      for(i=0;i<SIZE_4KB;i+=16){
        Info = (MY_BYO_BIOS_INFO2_TMP*)(BiosData + Index + i);
        if(Info->Header.Signature == BYO_BIOS_INFO2_SIGNATURE){
          DEBUG((DEBUG_VERBOSE, "biosinfo +%x\n", Index + i));
          *BiosInfo = Info;
          return EFI_SUCCESS;
        }      
      }

      Index += (UINTN)FvHdr->FvLength;
    } else {
      Index += SIZE_4KB;
    }
  }

  return EFI_NOT_FOUND;
}



EFI_STATUS
EFIAPI
ByoSmiFlashGetBiosInfo2 (
  IN  BYO_SMIFLASH_PROTOCOL   *This,
  IN  UINT8                   *NewBios,
  IN  UINTN                   NewBiosSize,
  OUT VOID                    **BiosInfo
  )
{
  return GetBiosInfoFromFile(BiosInfo, NewBios, NewBiosSize);
}


EFI_STATUS
EFIAPI
ByoSmiFlashPrepareEnv (
  IN  BYO_SMIFLASH_PROTOCOL   *This
  )
{
  gSmiFlashInfo->SubFunction = SUBFUNCTION_DISABLE_USB_POWERBUTTON;
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
  return gSmiFlashInfo->StatusCode;
}


EFI_STATUS
EFIAPI
ByoSmiFlashClearEnv (
  IN  BYO_SMIFLASH_PROTOCOL   *This
  )
{
  gSmiFlashInfo->SubFunction = SUBFUNCTION_ENABLE_USB_POWERBUTTON;
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
  return gSmiFlashInfo->StatusCode;
}



CHAR16 gCopyrightStr1[] = L"**************************************************************";
CHAR16 gCopyrightStr2[] = L"*                   Byosoft Flash Update                     *";
CHAR16 gCopyrightStr3[] = L"*         Copyright(C) 2006-2021, Byosoft Co.,Ltd.           *";
CHAR16 gCopyrightStr4[] = L"*                   All rights reserved                      *";
CHAR16 gCopyrightStr5[] = L"**************************************************************\n\r\n\r";
CHAR16 gCopyrightStr6[] = L"Warning: System is updating flash. Please don't shutdown system during erasing/programming flash!\n";

EFI_STATUS
ShowCopyRightsAndWarning (
  VOID
)
{
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut = gST->ConOut;
  UINTN                             Columns;
  UINTN                             Rows;
  UINTN                             PrintX, PrintY;
  UINTN                             Len;
  BIOS_ID_INFO                      BiosIdInfo;
  EFI_STATUS                        Status;
  CHAR16                            *Year;

  Status = GetBiosIdInfoFromHob(&BiosIdInfo);
  if (!EFI_ERROR(Status) && (Year = StrStr(gCopyrightStr3, L"2021"))!=NULL) {
    Year[0] = BiosIdInfo.BiosDate[6];
    Year[1] = BiosIdInfo.BiosDate[7];
    Year[2] = BiosIdInfo.BiosDate[8];
    Year[3] = BiosIdInfo.BiosDate[9];
    DEBUG((DEBUG_VERBOSE, "Year:%c%c%c%c\n", Year[0], Year[1], Year[2], Year[3]));
  }

  ConOut->SetAttribute(ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE | EFI_BRIGHT);
  ConOut->EnableCursor(ConOut, FALSE);  
  ConOut->SetAttribute(ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);

  ConOut->QueryMode(ConOut, ConOut->Mode->Mode, &Columns, &Rows);
  Len = StrLen(gCopyrightStr1);
  PrintX = Len >= Columns ? 0 : ((Columns - Len)/2);
  PrintY = 0;

  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr1);
  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr2);
  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr3);
  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr4);
  ConOut->SetCursorPosition(ConOut, PrintX, PrintY++);
  ConOut->OutputString(ConOut, gCopyrightStr5);

  gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_WHITE | EFI_BRIGHT);

  return EFI_SUCCESS;
}




EFI_STATUS
ByoSmiFlashSignVerifyPrepare (
  IN IMAGE_DATA_CTX   *Ctx
  )
{
  SIGN_INIT_DATA  *InitData;
  UINTN           Size;


  if(Ctx == NULL || Ctx->SignSize != 256){
    return EFI_INVALID_PARAMETER;
  }

  if(Ctx->HashType != SIGN_INIT_HASH_TYPE_SHA256 &&  Ctx->HashType != SIGN_INIT_HASH_TYPE_SHA512){
    return EFI_INVALID_PARAMETER;
  }

  Size = sizeof(SIGN_INIT_DATA) + Ctx->SignSize - 1;
  if(Size > sizeof(gSmiFlashInfo->Buffer)){
    return EFI_INVALID_PARAMETER;
  }

  InitData = (SIGN_INIT_DATA*)gSmiFlashInfo->Buffer;

  gSmiFlashInfo->SubFunction = SUBFUNCTION_IF_SIGN_PREPARE; 
  gSmiFlashInfo->Size     = (UINT32)Size;  
  InitData->HashType      = Ctx->HashType;
  InitData->SignSize      = (UINT32)Ctx->SignSize; 
  InitData->SignFvOffset  = Ctx->SignFvOffset;
  InitData->SignMethod    = Ctx->SignMethod;  
  CopyMem(InitData->SignData, Ctx->SignData, Ctx->SignSize);  
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
  return gSmiFlashInfo->StatusCode;

}

CHAR16 *gSigVerifyStr = L"Sign Verify ...........";

VOID 
ShowSignVerifyProgress (
  IN UINTN     Persent
  )
{
  gPrivate.ShowProgress(BIOS_UPDATA_DRAW_TYPE_REFRESH, MultU64x32(Persent, 100), gSigVerifyStr, gPrivate.ShowContext);
}


EFI_STATUS
ByoSmiFlashSignVerifyUpdate (
  IN  IMAGE_DATA_CTX                 *Ctx,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize 
  )
{
  if(Data == NULL || DataSize == 0){
    return EFI_INVALID_PARAMETER;
  }

  if(DataSize > sizeof(gSmiFlashInfo->Buffer)){
    return EFI_INVALID_PARAMETER;
  }

  gSmiFlashInfo->SubFunction = SUBFUNCTION_IF_SIGN_UPDATE; 
  gSmiFlashInfo->Offset   = Offset;
  gSmiFlashInfo->Size     = DataSize; 
  CopyMem(gSmiFlashInfo->Buffer, Data, DataSize);  
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
  return gSmiFlashInfo->StatusCode;

}

EFI_STATUS
ByoSmiFlashSignVerifyVerify (
    VOID
  )
{
  gSmiFlashInfo->SubFunction = SUBFUNCTION_IF_SIGN_VERIFY; 
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
  return gSmiFlashInfo->StatusCode;
}


/**
  Show Bios Update Progress

  @param[in] Type         Draw Type
                          BIOS_UPDATE_DRAW_TYPE_START   The first drawing
                          BIOS_UPDATE_DRAW_TYPE_RUN     The updating drawing
                          BIOS_UPDATE_DRAW_TYPE_END     The end drawing
  @param[in] Rate         0-10000 Two decimal places
  @param[in] Info         Bios Update Info String
  @param[in] Context      Bios Update Progress Bar Draw Function Context

  @retval EFI_SUCCESS     Draw Success
  @retval Other           Draw Error

**/
EFI_STATUS
BiosUpdataShowProgressCallback (
  IN UINTN     Type,
  IN UINT64    Rate,
  IN CHAR16    *Info,
  IN VOID      *Context
  )
{
  UINTN                     Attribute;

  switch (Type) {
    case BIOS_UPDATA_DRAW_TYPE_INFO:
      if (Info != NULL) {
        Print(Info);
      }
      break;
    case BIOS_UPDATA_DRAW_TYPE_REFRESH:
      if (Info != NULL) {
        Print(L"\r%s     (%d%%)", Info, Rate/100);
      }
      break;
    case BIOS_UPDATA_DRAW_TYPE_COPYRIGHT_INFO:
      Attribute = (UINTN) gST->ConOut->Mode->Attribute;
      gST->ConOut->SetAttribute(gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
      Print(Info);
      gST->ConOut->SetAttribute (gST->ConOut, Attribute);
      break;
    case BIOS_UPDATA_DRAW_TYPE_ERROR:
      Print(L"\r%s Error", Info);
      break;
    case BIOS_UPDATA_DRAW_TYPE_EXIT:
      Print(L"\n");
      break;
    default:
      break;
  }
  return EFI_SUCCESS;
}

void PrintBiosId(BIOS_ID_IMAGE * BiosIdinfo)
{
  int i;
  // BoardId
  Print (L"BoardId: ");
  for  (i = 0; i < 7; i++) {
    Print (L"%c", BiosIdinfo->BiosIdString.BoardId[i]);
  }
  Print (L"\n");
  Print (L"OemId: ");
  for (i = 0; i < 3; i++) {
    Print (L"%c", BiosIdinfo->BiosIdString.OemId[i]);
  }
  Print (L"\n");
  Print (L"VersionMajor:");
  for (i = 0; i < 4; i++) {
    Print (L"%c", BiosIdinfo->BiosIdString.VersionMajor[i]);
  }
  Print (L"\n");

}


EFI_STATUS
EFIAPI
ByoSmiFlashBiosCheck (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize,
  OUT VOID                           **pBiosInfo2  OPTIONAL 
  )
{
  EFI_STATUS                         Status;
  BIOS_ID_IMAGE                      BiosIdImage;
  IMAGE_DATA_CTX                     CtxData;
  IMAGE_DATA_CTX                     *Ctx = &CtxData;
  EFI_TPL                            OldTpl;//TPL
  SMI_FLASH_CTX                      *Private;
  UINT8                              Str = 0;
  BIOS_UPDATE_OPTIONS                UpdateOptions;
  UINT32                             Attributes;
  UINTN                              VariableSize;

  if(This == NULL || NewBiosData == NULL){
    return EFI_INVALID_PARAMETER;
  }
  
  Private = SMIFLASH_CTX_FROM_THIS(This);

  if (Private->ShowProgress == NULL) {
    Private->ShowProgress = BiosUpdataShowProgressCallback;
  }

  if(NewBiosDataSize < PcdGet32(PcdFlashAreaSize) || NewBiosDataSize > BIOS_FILE_MAX_SIZE){
    return EFI_UNSUPPORTED;
  }

  OldTpl = gBS->RaiseTPL(TPL_NOTIFY); //raiseTPL

  SetMem(&UpdateOptions, sizeof(BIOS_UPDATE_OPTIONS), 0);
  VariableSize = sizeof(BIOS_UPDATE_OPTIONS);
  Status = gRT->GetVariable(
      BIOS_UPDATE_OPTIONS_NAME,
      &gByoGlobalVariableGuid,
      &Attributes,
      &VariableSize,
      &UpdateOptions
    );
  if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_VERBOSE, "GetVariable %s failed\n",BIOS_UPDATE_OPTIONS_NAME));
  }

  if ((UpdateOptions.SkipFlag & SKIP_BIOS_ID) == 0){
    DEBUG((DEBUG_VERBOSE, "Check BiosId\n"));
    Status = GetBiosIdInFv (&BiosIdImage, NewBiosData + PcdGet32(PcdFlashFvRecoveryBase) - PcdGet32(PcdFlashAreaBaseAddress));
    if (EFI_ERROR (Status)) {
      Status = GetBiosIdInAnyFv (&BiosIdImage, NewBiosData, NewBiosDataSize);
    }  
    if (EFI_ERROR (Status)) {
      goto ProcExit;
    }

    gSmiFlashInfo->SubFunction = SUBFUNCTION_CHECK_BIOS_ID;
    CopyMem(gSmiFlashInfo->Buffer, &BiosIdImage, sizeof(BIOS_ID_IMAGE));
    SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
    if(EFI_ERROR (gSmiFlashInfo->StatusCode)){
      Status = gSmiFlashInfo->StatusCode;
      if ((UpdateOptions.SkipFlag & ALLOW_PRINT) != 0){
        Print (L"Input image\n");
        PrintBiosId(&BiosIdImage);
        Print (L"Board image\n");
        PrintBiosId((BIOS_ID_IMAGE *)(gSmiFlashInfo->Buffer));
      }
      goto ProcExit;
    }
  }
  
  if ((UpdateOptions.SkipFlag & SKIP_AC) == 0){
    DEBUG((DEBUG_VERBOSE, "Check Ac\n"));
    gSmiFlashInfo->SubFunction = SUBFUNCTION_CHECK_AC;
    SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
    if (gSmiFlashInfo->StatusCode == EFI_SUCCESS || gSmiFlashInfo->StatusCode == EFI_INVALID_PARAMETER){
    } else {
      if ((UpdateOptions.SkipFlag & ALLOW_PRINT) != 0){
        Print (L"Please check AC Adapter be plugged!\n");
      }
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }
  }

  if (PcdGetBool (PcdByoSecureFlashSupport)) {
    Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_ENTRY, 0, L"Bios Verify", Private->ShowContext);

    Status = ParseByoSignInfo(NewBiosData, NewBiosDataSize, &CtxData);
    if(EFI_ERROR(Status)){
      Status = EFI_SECURITY_VIOLATION;
      goto ProcExit;
    }
    Status = ByoSmiFlashSignVerifyPrepare(Ctx);
    if(Private->ProgressUpdate == NULL){
      Str = 1;
    }
    if(EFI_ERROR(Status)){
      Status = EFI_SECURITY_VIOLATION;  
      if(Str == 1){
        Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_ERROR, 0, gSigVerifyStr, Private->ShowContext);
      }
      goto ProcExit;    
    }

    if(Private->ProgressUpdate == NULL){
      Private->ProgressUpdate = ShowSignVerifyProgress;
    }

    Status = DoByoSignVerifyUpdate(
              Ctx, 
              ByoSmiFlashSignVerifyUpdate, 
              Private->ProgressUpdate
              );
    if(EFI_ERROR(Status)){
      Status = EFI_SECURITY_VIOLATION;  
      if(Str == 1){
        Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_ERROR, 0, gSigVerifyStr, Private->ShowContext);
      }
      goto ProcExit;    
    }  

    Private->ProgressUpdate = NULL;

    Status = ByoSmiFlashSignVerifyVerify();
    if(EFI_ERROR(Status)){
      Status = EFI_SECURITY_VIOLATION;  
      if(Str == 1){
        Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_ERROR, 0, gSigVerifyStr, Private->ShowContext);
      }
      goto ProcExit;    
    }
  } else {
    if(NewBiosDataSize != PcdGet32(PcdFlashAreaSize)){
      Status = EFI_UNSUPPORTED;
      goto ProcExit; 
    }
  }

  Status = EFI_SUCCESS;

  if(pBiosInfo2 != NULL){
    Status = This->GetBiosInfo2(This, NewBiosData, NewBiosDataSize, pBiosInfo2);
    if(EFI_ERROR(Status)){
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }
  }
  
ProcExit:
  //
  // Clear Bios Update Setting
  //
  gRT->SetVariable(
      BIOS_UPDATE_OPTIONS_NAME,
      &gByoGlobalVariableGuid,
      0,
      0,
      NULL
    );
  if (PcdGetBool (PcdByoSecureFlashSupport)) {
    Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_EXIT, 0, NULL, Private->ShowContext);
  }
  gBS->RestoreTPL (OldTpl);
  return Status;  
}

// SUBFUNCTION_BIOS_VERIFY may have some patches, so invoke it.
EFI_STATUS
EFIAPI
ByoSmiFlashVerify (
  BYO_SMIFLASH_PROTOCOL          *This
  )
{
  gSmiFlashInfo->SubFunction = SUBFUNCTION_BIOS_VERIFY; 
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
  return gSmiFlashInfo->StatusCode;
}




EFI_STATUS
EFIAPI
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
  UINT32                    LastStep = 0xFFFFFFFF;
  UINT8                     *OldBiosData = NULL;
  VOID                      *BiosMmio;  
  UINT32                    BiosSize;
  EFI_TPL                   OldTpl;//TPL
  SMI_FLASH_CTX             *Private;
  CHAR16                    *StepName = NULL;

  Private = SMIFLASH_CTX_FROM_THIS(This);
  if (Private->ShowProgress == NULL) {
    Private->ShowProgress = BiosUpdataShowProgressCallback;
  }

  Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_ENTRY, 0, L"Bios Update", Private->ShowContext);

  OldTpl = gBS->RaiseTPL(TPL_NOTIFY); //raiseTPL

  //
  // Print warning message when updating BIOS image.
  //
  Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_COPYRIGHT_INFO, 0, gCopyrightStr6, Private->ShowContext);

  Status = This->GetBiosInfo2(This, NewBiosData, NewBiosDataSize, (VOID **)&BiosInfo2);
  if(EFI_ERROR(Status)){
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  This->PrepareEnv(This);

  if(PcdGetBool(PcdSmiFlashDxeDirectRead)){
    BiosSize = PcdGet32(PcdFlashAreaSize);
    OldBiosData = AllocatePool(BiosSize);
    if(OldBiosData == NULL){
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;      
    }
    BiosMmio = (VOID*)(UINTN)(0xFFFFFFFF - BiosSize + 1);
    WriteBackInvalidateDataCacheRange(BiosMmio, BiosSize);

    Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_INFO, 0, L"Read Current BIOS\n", Private->ShowContext);
    CopyMem(OldBiosData, BiosMmio, BiosSize);    
  }

  BootMode = GetBootModeHob();

// program logo.
  LogoAreaSize = PcdGet32(PcdFlashNvLogoSize);
  if(LogoAreaSize && BootMode == BOOT_IN_RECOVERY_MODE){
    Offset = PcdGet32(PcdFlashNvLogoBase) - PcdGet32(PcdFlashAreaBaseAddress);
    for(i=0;i<LogoAreaSize;i+=SIZE_4KB){
      Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_REFRESH, MultU64x32(i, 10000)/LogoAreaSize, L"Update Logo ...........", Private->ShowContext);
      Status = This->ProgramFlash(This, Offset+i, NewBiosData+Offset+i, SIZE_4KB);
      if(EFI_ERROR(Status)){
        Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_ERROR, MultU64x32(i, 10000)/LogoAreaSize, L"Update Logo ...........", Private->ShowContext);
        goto ProcExit;
      }
    }
    Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_REFRESH, 10000, L"Update Logo ...........", Private->ShowContext);
    Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_INFO, 0, L"\n", Private->ShowContext);
  }


// program others
  for(Index=0;Index<BiosInfo2->Header.Step_NUM;Index++){
    Step = &BiosInfo2->Step[Index];
    if (StepName != NULL) {
      FreePool (StepName);
    }
    StepName = AllocateZeroPool ((AsciiStrLen(Step->Name) + 1) * sizeof (CHAR16));
    AsciiStrToUnicodeStrS ((CHAR8 *)&Step->Name[0], StepName, AsciiStrLen(Step->Name) + 1);

    if(Step->Op == OP_BIOS_VERIFY__){
      Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_EXIT, 10000, StepName, Private->ShowContext);
      This->UpdateStep(This, Step->Step);
      break;
    }
    
    if(Step->Op == OP_RESERVE_OEM__ || Step->Op == OP_RESERVE_NVM__){
      continue;
    }
    
    if((Step->Step == FLASH_STEP_NV || Step->Step == FLASH_STEP_LOGO || Step->Step == FLASH_STEP_SMBIOS) && !PcdGetBool(PcdUpdateNvVariableEnable)){
      continue;
    }

    if(Step->Info2.Size == 0){
      continue;
    }

    if(BootMode == BOOT_IN_RECOVERY_MODE && Step->Step == FLASH_STEP_BK){
      continue;
    }

    if(Step->Op == OP_PROGRAM_FLASH){
      for(i=0;i<Step->Info2.Size;i+=SIZE_4KB){
        Offset = Step->Info2.Offset + i;
        Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_REFRESH, MultU64x32(i, 10000)/Step->Info2.Size, StepName, Private->ShowContext);
        if(LastStep != Step->Step){
          This->UpdateStep(This, Step->Step);
          LastStep = Step->Step;
        }
        if(PcdGetBool(PcdSmiFlashDxeDirectRead)){
          if(CompareMem(OldBiosData+Offset, NewBiosData+Offset, SIZE_4KB) != 0){
            Status = This->ProgramFlash(This, Offset, NewBiosData+Offset, SIZE_4KB);
          }
        } else {
          Status = This->ProgramFlash(This, Offset, NewBiosData+Offset, SIZE_4KB);
        }
        if(EFI_ERROR(Status)){
          Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_ERROR, MultU64x32(i, 10000)/Step->Info2.Size, StepName, Private->ShowContext);
          goto ProcExit;
        }
      }
      Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_REFRESH, 10000, StepName, Private->ShowContext);
      Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_INFO, 0, L"\n", Private->ShowContext);
    } 
  }

  This->FlashVerify(This);
  This->ClearEnv(This);

ProcExit:
  if (StepName != NULL) {
    FreePool (StepName);
  }
  if(OldBiosData != NULL){
    FreePool(OldBiosData);
  }
  gBS->RestoreTPL (OldTpl);
  return Status;  
}



EFI_STATUS
EFIAPI
ByoSmiFlashUpdateAll (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT8                          *NewBiosData,
  IN  UINTN                          NewBiosDataSize
  )
{
  EFI_STATUS                Status;
  UINT32                    Offset;	
  UINT8                     *OldBiosData = NULL;
  VOID                      *BiosMmio;  
  UINT32                    BiosSize;
  EFI_TPL                   OldTpl;//TPL
  SMI_FLASH_CTX             *Private;
  CHAR16                    *StepName = NULL;

  Private = SMIFLASH_CTX_FROM_THIS(This);
  if (Private->ShowProgress == NULL) {
    Private->ShowProgress = BiosUpdataShowProgressCallback;
  }

  Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_ENTRY, 0, L"Bios Update All", Private->ShowContext);

  OldTpl = gBS->RaiseTPL(TPL_NOTIFY); //raiseTPL

  //
  // Print warning message when updating BIOS image.
  //
  Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_COPYRIGHT_INFO, 0, gCopyrightStr6, Private->ShowContext);

  This->PrepareEnv(This);

  BiosSize = PcdGet32(PcdFlashAreaSize);
  if(PcdGetBool(PcdSmiFlashDxeDirectRead)){
    OldBiosData = AllocatePool(BiosSize);
    if(OldBiosData == NULL){
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;      
    }
    BiosMmio = (VOID*)(UINTN)(0xFFFFFFFF - BiosSize + 1);
    WriteBackInvalidateDataCacheRange(BiosMmio, BiosSize);

    Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_INFO, 0, L"Read Current BIOS\n", Private->ShowContext);
    CopyMem(OldBiosData, BiosMmio, BiosSize);    
  }

  for(Offset=0; Offset<BiosSize; Offset+=SIZE_4KB){
    Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_REFRESH, MultU64x32(Offset, 10000)/BiosSize, L"Bios Update all ...........", Private->ShowContext);
    if(PcdGetBool(PcdSmiFlashDxeDirectRead)){
      if(CompareMem(OldBiosData+Offset, NewBiosData+Offset, SIZE_4KB) != 0){
        Status = This->ProgramFlash(This, Offset, NewBiosData+Offset, SIZE_4KB);
      }
    } else {
      Status = This->ProgramFlash(This, Offset, NewBiosData+Offset, SIZE_4KB);
    }
    if(EFI_ERROR(Status)){
      Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_ERROR, MultU64x32(Offset, 10000)/BiosSize, L"Bios Update all ...........", Private->ShowContext);
      goto ProcExit;
    }
  }
  Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_REFRESH, 10000, L"Bios Update all ...........", Private->ShowContext);
  Private->ShowProgress(BIOS_UPDATA_DRAW_TYPE_INFO, 0, L"\n", Private->ShowContext);

  This->FlashVerify(This);
  This->ClearEnv(This);

ProcExit:
  if (StepName != NULL) {
    FreePool (StepName);
  }
  if(OldBiosData != NULL){
    FreePool(OldBiosData);
  }
  gBS->RestoreTPL (OldTpl);
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

  MediaType = L"Unknown";

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
    Print(L"[%d] %s   Media: %s   Activated: %s\n", 
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
EFIAPI
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

  ShowCopyRightsAndWarning();

  Status = This->BiosCheck(This, NewBiosData, NewBiosDataSize, NULL);
  if (Status == EFI_SUCCESS) {
    Status = This->DefaultUpdate(This, NewBiosData, NewBiosDataSize);
  }
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
EFIAPI
ByoSmiFlashRead (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize
  )
{
  SMI_FLASH_CTX                *Private;
  UINT32                       Index;
  UINT32                       Count;
  UINT32                       Remain;
  
  Private = SMIFLASH_CTX_FROM_THIS(This);
  
  if(DataSize == 0){
    return EFI_INVALID_PARAMETER;
  }

  Count = DataSize/sizeof(gSmiFlashInfo->Buffer);
  
  for(Index = 0; Index < Count; Index++){
    gSmiFlashInfo->SubFunction = SUBFUNCTION_READ_FLASH_BLOCK;
    gSmiFlashInfo->Offset = Offset + Index * sizeof(gSmiFlashInfo->Buffer);
    gSmiFlashInfo->Size = sizeof(gSmiFlashInfo->Buffer);
    SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
    if(EFI_ERROR (gSmiFlashInfo->StatusCode)){
        return gSmiFlashInfo->StatusCode;
    }
    CopyMem((UINT8*)Data + Index * sizeof(gSmiFlashInfo->Buffer), gSmiFlashInfo->Buffer, gSmiFlashInfo->Size);
  }

  Remain = DataSize % sizeof(gSmiFlashInfo->Buffer);
  if(Remain){
    gSmiFlashInfo->SubFunction = SUBFUNCTION_READ_FLASH_BLOCK;
    gSmiFlashInfo->Offset = Offset + Index * sizeof(gSmiFlashInfo->Buffer);
    gSmiFlashInfo->Size = Remain;
    SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
    CopyMem((UINT8*)Data + Index * sizeof(gSmiFlashInfo->Buffer), gSmiFlashInfo->Buffer, gSmiFlashInfo->Size);
    return gSmiFlashInfo->StatusCode;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
ByoSmiFlashWrite (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  VOID                           *Data,
  IN  UINT32                         DataSize
  )
{
  SMI_FLASH_CTX                *Private;
  UINT32                       Index;
  UINT32                       Count;
  UINT32                       Remain;
  
  Private = SMIFLASH_CTX_FROM_THIS(This);
  
  if(DataSize == 0){
    return EFI_INVALID_PARAMETER;
  }

  Count = DataSize/sizeof(gSmiFlashInfo->Buffer);
  
  for(Index = 0; Index < Count; Index++){
    gSmiFlashInfo->SubFunction = SUBFUNCTION_WRITE_FLASH;
    gSmiFlashInfo->Offset = Offset + Index * sizeof(gSmiFlashInfo->Buffer);
    gSmiFlashInfo->Size = sizeof(gSmiFlashInfo->Buffer);
    CopyMem(gSmiFlashInfo->Buffer, (UINT8*)Data + Index * sizeof(gSmiFlashInfo->Buffer), sizeof(gSmiFlashInfo->Buffer));
    SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
    if(EFI_ERROR (gSmiFlashInfo->StatusCode)){
        return gSmiFlashInfo->StatusCode;
    }
  }

  Remain = DataSize % sizeof(gSmiFlashInfo->Buffer);
  if(Remain){
    gSmiFlashInfo->SubFunction = SUBFUNCTION_WRITE_FLASH;
    gSmiFlashInfo->Offset = Offset + Index * sizeof(gSmiFlashInfo->Buffer);
    gSmiFlashInfo->Size = Remain;
    CopyMem(gSmiFlashInfo->Buffer, (UINT8*)Data + Index * sizeof(gSmiFlashInfo->Buffer), Remain);
    SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);
    return gSmiFlashInfo->StatusCode;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
ByoSmiFlashErase (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  UINT32                         DataSize
  )
{

  Offset = Offset & ~0xFFF;
  DataSize = ALIGN_VALUE(DataSize, SIZE_4KB);
  if(DataSize == 0){
    return EFI_INVALID_PARAMETER;
  }
  
  gSmiFlashInfo->SubFunction = SUBFUNCTION_ERASE_FLASH;
  gSmiFlashInfo->Offset = Offset;
  gSmiFlashInfo->Size = DataSize;
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);

  return gSmiFlashInfo->StatusCode;
}


EFI_STATUS
EFIAPI
ByoSmiUpdateSmbios (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UPDATE_SMBIOS_PARAMETER        *SmbiosPtr
  )
{

  gSmiFlashInfo->SubFunction = SUBFUNCTION_UPDATE_SMBIOS_DATA;
  CopyMem(gSmiFlashInfo->Buffer, SmbiosPtr, sizeof (UPDATE_SMBIOS_PARAMETER) + SmbiosPtr->Parameter.DataLength);
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);

  return gSmiFlashInfo->StatusCode;
}



EFI_STATUS
EFIAPI
ByoSetSignVerifyProgressCallBack (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  PROGRESS_CALLBACK              CallBack
  )
{
  SMI_FLASH_CTX                *Private;

  if(This == NULL || CallBack == NULL){
    return EFI_INVALID_PARAMETER;
  }
  
  Private = SMIFLASH_CTX_FROM_THIS(This);
  Private->ProgressUpdate = CallBack;
  return EFI_SUCCESS;
}

/**
  Show Bios Update Progress

  @param[in] Type         Draw Type
                          BIOS_UPDATE_DRAW_TYPE_START   The first drawing
                          BIOS_UPDATE_DRAW_TYPE_RUN     The updating drawing
                          BIOS_UPDATE_DRAW_TYPE_END     The end drawing
  @param[in] Rate         0-10000 Two decimal places
  @param[in] Context      Bios Update Progress Bar Draw Function Context

  @retval EFI_SUCCESS     Draw Success
  @retval Other           Draw Error

**/
EFI_STATUS
EFIAPI 
ByoSetShowProgressCallback (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  SHOW_PROGRESS_CALLBACK         CallBack,
  IN  VOID                           *Context
  )
{
  SMI_FLASH_CTX                *Private;

  if(This == NULL || CallBack == NULL){
    return EFI_INVALID_PARAMETER;
  }

  Private = SMIFLASH_CTX_FROM_THIS(This);
  Private->ShowProgress = CallBack;
  Private->ShowContext = Context;

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
ByoFlashProgramBackupFlash (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  UINT8                          *Data,
  IN  UINT32                         DataSize
  )
{
  return EFI_UNSUPPORTED;
}
    
EFI_STATUS
EFIAPI
ByoFlashBackupRead (
  IN  BYO_SMIFLASH_PROTOCOL          *This,
  IN  UINT32                         Offset,
  IN  UINT8                          *Data,
  IN  UINT32                         DataSize
  )
{
  return EFI_UNSUPPORTED;
}


UINT32
ByoGetFlashBolckSize (
  )
{
  return SIZE_4KB;
}

SMI_FLASH_CTX gPrivate = {
  SMIFLASH_CTX_SIGN,
  {
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
    ByoSetSignVerifyProgressCallBack,
    ByoSetShowProgressCallback,
    ByoFlashProgramBackupFlash,
    ByoFlashBackupRead,
    ByoGetFlashBolckSize,
    ByoSmiFlashUpdateAll,
    ByoSmiFlashVerify
  },
  NULL
};





EFI_STATUS
EFIAPI
UpdateMinValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  )
{
  SETUP_UPDATE_DATA_INFO   *TempDataInfo = (SETUP_UPDATE_DATA_INFO *)gSmiFlashInfo->Buffer;
  StrCpyS(TempDataInfo->VariableName, 64, VariableName);
  CopyMem(&TempDataInfo->VendorGuid, VendorGuid, sizeof(EFI_GUID) );
  TempDataInfo->Offset = Offset;
  TempDataInfo->UpdateType = SETUP_UPDATE_TYPE_MIN_VALUE;
  TempDataInfo->UpdateValue[0] = Value;

  gSmiFlashInfo->SubFunction = SUBFUNCTION_SET_VARIABLE_INFO;
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);

  return gSmiFlashInfo->StatusCode;
  
}

EFI_STATUS
EFIAPI
UpdateMaxValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  )
{
  SETUP_UPDATE_DATA_INFO   *TempDataInfo = (SETUP_UPDATE_DATA_INFO *)gSmiFlashInfo->Buffer;
  StrCpyS(TempDataInfo->VariableName, 64, VariableName);
  CopyMem(&TempDataInfo->VendorGuid, VendorGuid, sizeof(EFI_GUID) );
  TempDataInfo->Offset = Offset;
  TempDataInfo->UpdateType = SETUP_UPDATE_TYPE_MAX_VALUE;
  TempDataInfo->UpdateValue[0] = Value;

  gSmiFlashInfo->SubFunction = SUBFUNCTION_SET_VARIABLE_INFO;
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);

  return gSmiFlashInfo->StatusCode;
}

EFI_STATUS
EFIAPI
UpdateDefaultValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  )
{
  SETUP_UPDATE_DATA_INFO   *TempDataInfo = (SETUP_UPDATE_DATA_INFO *)gSmiFlashInfo->Buffer;
  StrCpyS(TempDataInfo->VariableName, 64, VariableName);
  CopyMem(&TempDataInfo->VendorGuid, VendorGuid, sizeof(EFI_GUID) );
  TempDataInfo->Offset = Offset;
  TempDataInfo->UpdateType = SETUP_UPDATE_TYPE_DEAULT_VALUE;
  TempDataInfo->UpdateValue[0] = Value;

  gSmiFlashInfo->SubFunction = SUBFUNCTION_SET_VARIABLE_INFO;
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);

  return gSmiFlashInfo->StatusCode;

}

EFI_STATUS
EFIAPI
UpdateAttribute(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT16                       Attribute
  )
{
  return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
UpdateOptionValue (
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINTN                        Count,
  IN UINT64                       *OptionValueArray
  )
{
  SETUP_UPDATE_DATA_INFO   *TempDataInfo = (SETUP_UPDATE_DATA_INFO *)gSmiFlashInfo->Buffer;
  StrCpyS(TempDataInfo->VariableName, 64, VariableName);
  CopyMem(&TempDataInfo->VendorGuid, VendorGuid, sizeof(EFI_GUID) );
  TempDataInfo->Offset = Offset;
  TempDataInfo->UpdateType = SETUP_UPDATE_TYPE_OPTION_VALUE;
  TempDataInfo->UpdateValueCount = Count;
  CopyMem(TempDataInfo->UpdateValue, OptionValueArray, Count*sizeof(UINT64));

  gSmiFlashInfo->SubFunction = SUBFUNCTION_SET_VARIABLE_INFO;
  SMI_CALL(SW_SMI_FLASH_SERVICES, (UINT64)(UINTN)gSmiFlashInfo);

  return gSmiFlashInfo->StatusCode;
}

SETUP_ITEM_UPDATE_PROTOCOL gSetupItemUpdateProtocol = {
  UpdateMinValue,
  UpdateMaxValue,
  UpdateDefaultValue,
  UpdateAttribute,
  UpdateOptionValue
};

/**
  Callback function for EnterSetup protocol install events

  @param Event           the event that is signaled.
  @param Context         not used here.

**/
VOID
EFIAPI
EnterSetupCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS                                Status;
  UINTN                                     CommSize;
  EFI_SMM_COMMUNICATION_PROTOCOL            *SmmCommunication;
  EFI_SMM_COMMUNICATE_HEADER                *SmmCommunicateHeader = NULL;

  gBS->CloseEvent (Event);

  Status = gBS->LocateProtocol (&gEfiSmmCommunicationProtocolGuid, NULL, (VOID **) &SmmCommunication);
  if (EFI_ERROR (Status)) {
    return;
  }

  CommSize = sizeof (EFI_SMM_COMMUNICATE_HEADER);
  SmmCommunicateHeader = AllocateReservedPool (CommSize);
  CopyGuid (&SmmCommunicateHeader->HeaderGuid, &gEfiSetupEnterGuid);
  SmmCommunicateHeader->MessageLength = 1;
  SmmCommunicateHeader->Data[0] = 0;
  SmmCommunication->Communicate (SmmCommunication, SmmCommunicateHeader, &CommSize);
  FreePool (SmmCommunicateHeader);

  return;
}

EFI_STATUS
EFIAPI
SmiFlashDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS          Status;
  VOID                *Registration;
  EFI_HANDLE          DriverHandle;

  DriverHandle = NULL;
  DEBUG((EFI_D_INFO, "SmiFlashDxeEntryPoint\n"));

  gSmiFlashInfo = (SMI_INFO *)(UINT64)LibGetSmiInfoBufferAddr(gBS);
  DEBUG((EFI_D_INFO, "SmiFlashDxe gSmiFlashInfo:%p\n",gSmiFlashInfo));

  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    EnterSetupCallback,
    NULL,
    &Registration
    );

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gByoSmiFlashProtocolGuid,
                  &gPrivate.SmiFlash,
                  NULL
                  );
  ASSERT(!EFI_ERROR(Status));

  Status = gBS->InstallMultipleProtocolInterfaces (
                &DriverHandle,
                &gEfiDevicePathProtocolGuid,
                &mSecurityupdateHiiVendorDevicePath,
                NULL
                );
  ASSERT(!EFI_ERROR(Status));

  gHiiHandle = HiiAddPackages (
                &gByoSetupSecurityUpdateGuid,
                DriverHandle,
                STRING_ARRAY_NAME,
                SecurityUpdateBin,
                NULL
                );
  ASSERT(gHiiHandle != NULL);

  Status = gBS->InstallProtocolInterface (
                    &ImageHandle,
                    &gSetupItemUpdateProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gSetupItemUpdateProtocol
                    );
  
  return Status;
}

