/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By: 
  winddy_zhang

File Name:
  PostErrorDxe.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include <Protocol/PlatHostInfoProtocol.h>
#include <Guid/FileInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ByoCommLib.h>
#include <SysMiscCfg.h>
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>
#include <Protocol/SimpleFileSystem.h>
#include <Library/SetupUiLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <ByoStatusCode.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/MemoryAllocationLib.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/Smbios.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <Protocol/PciIo.h>
#include <Protocol/PlatHostInfoProtocol.h>


#define TXT_SCREEN_START_X   2
#define TXT_SCREEN_START_Y   2

typedef struct {
  EFI_HANDLE  Host;
  UINTN       Port;
  UINTN       HddErrCount;
  UINTN       SmartErrCount;
} SATA_PORT_ERR_CTX;

#define MAX_SATA_PORTS_SUPPORT   32

EFI_HII_HANDLE    gHiiHandle;
UINTN             gScreenX = TXT_SCREEN_START_X;
UINTN             gScreenY = TXT_SCREEN_START_Y;


EFI_RSC_HANDLER_PROTOCOL  *gRscHandle = NULL;
SATA_PORT_ERR_CTX         gSataPortErrCtx[MAX_SATA_PORTS_SUPPORT];
UINTN                     gSataPortErrCtxCount = 0;



BOOLEAN IsThisSataPortErr(SATA_PORT_ERR_CTX *Ctx)
{
  if(Ctx == NULL){
    return FALSE;
  }

  if(Ctx->HddErrCount >= 3 || Ctx->SmartErrCount){
    return TRUE;
  }

  return FALSE;
}

BOOLEAN IsAnySataPortErr()
{
  UINTN               Index;
  SATA_PORT_ERR_CTX   *Ctx;

  Ctx = gSataPortErrCtx;
  for(Index=0;Index<gSataPortErrCtxCount;Index++){
    if(IsThisSataPortErr(&Ctx[Index])){
      return TRUE;
    }
  }

  return FALSE;
}



VOID ShowBiosBaseInfo()
{
  EFI_STATUS                Status;
  EFI_SMBIOS_PROTOCOL       *Smbios;
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER  p;
  CHAR8                     *BiosVer;
  CHAR8                     *BiosDate;
  CHAR8                     *BoardId;
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  CHAR8                             *CpuName;
  UINT16                            CpuSpeed; 
  UINTN                             SingleSize;
  UINTN                             TotalMemSize = 0;
  UINT16                            MemSpeed = 0;
  CHAR16                            *s;
  CHAR16                            Buffer[128+1];


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if(EFI_ERROR(Status)){
    return;
  } 

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  BiosVer = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosVersion);
  BiosDate = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosReleaseDate);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;
  BoardId = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type2->ProductName);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr    = SmbiosHdr;
  CpuName  = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->ProcessorVersion);
  CpuSpeed = p.Type4->CurrentSpeed;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr = SmbiosHdr;

    if(MemSpeed == 0 && p.Type17->ConfiguredMemoryClockSpeed != 0){
      MemSpeed = p.Type17->ConfiguredMemoryClockSpeed;
    }

    SingleSize = p.Type17->Size;
    if(SingleSize == 0 || SingleSize == 0xFFFF){
      SingleSize = 0;
    } else if(SingleSize == 0x7FFF){
      SingleSize = p.Type17->ExtendedSize & (~BIT31);   // MB
    } else {
      if(SingleSize & BIT15){                           // unit in KB.
        SingleSize = (SingleSize&(~BIT15)) >> 10;       // to MB
      }else{                                            // unit in MB.
      }
    }

    TotalMemSize += SingleSize;
  }



  ConOut = gST->ConOut;
  
  ConOut->SetCursorPosition(ConOut, TXT_SCREEN_START_X, gScreenY);
  s = HiiGetString(gHiiHandle, STRING_TOKEN(STR_BIOS_VER), NULL);
  UnicodeSPrint(Buffer, sizeof(Buffer), s, BiosVer);
  ConOut->OutputString(ConOut, Buffer);

  gScreenY++;
  ConOut->SetCursorPosition(ConOut, TXT_SCREEN_START_X, gScreenY);
  UnicodeSPrint(Buffer, sizeof(Buffer), L"%a %a", BoardId, BiosDate);
  FreePool(s);  
  ConOut->OutputString(ConOut, Buffer);  

  gScreenY++;
  ConOut->SetCursorPosition(ConOut, TXT_SCREEN_START_X, gScreenY);
  s = HiiGetString(gHiiHandle, STRING_TOKEN(STR_CPU_TYPE), NULL);
  UnicodeSPrint(Buffer, sizeof(Buffer), s, CpuName);
  FreePool(s);
  ConOut->OutputString(ConOut, Buffer);

  gScreenY++;
  ConOut->SetCursorPosition(ConOut, TXT_SCREEN_START_X, gScreenY);
  s = HiiGetString(gHiiHandle, STRING_TOKEN(STR_CPU_SPEED), NULL);
  UnicodeSPrint(Buffer, sizeof(Buffer), s, CpuSpeed);
  FreePool(s);  
  ConOut->OutputString(ConOut, Buffer); 

  gScreenY++;
  ConOut->SetCursorPosition(ConOut, TXT_SCREEN_START_X, gScreenY);
  s = HiiGetString(gHiiHandle, STRING_TOKEN(STR_MEM_SIZE), NULL);
  UnicodeSPrint(Buffer, sizeof(Buffer), s, TotalMemSize, MemSpeed);
  FreePool(s);
  ConOut->OutputString(ConOut, Buffer);  

  gScreenY++;  
}



VOID AfterConsoleEndHook()
{
  UINT8                             IsCmosBad; 
  UINT8                             IsNoKB = FALSE;
  UINT8                             IsSataErr;
  UINT32                            MiscConfig;
  UINT8                             NeedHalt = 0;
  CHAR16                            Buffer[128+1];
  EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL   *ConOut;
  UINTN                             EventIndex;
  EFI_STATUS                        Status;
  EFI_INPUT_KEY                     Key;  
  SATA_PORT_ERR_CTX                 *SataErrCtx;
  UINTN                             Index;
  UINTN                             s, b, d, f;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  CHAR16                            *SataCmdErr, *SataSmartErr;
  CHAR16                            *ErrCmd, *ErrSmart;
  CHAR16                            *Str;
  PLAT_HOST_INFO_PROTOCOL           *PlatHostInfo;

  
  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  IsCmosBad = PcdGet8(PcdIsPlatformCmosBad); 

  MiscConfig = PcdGet32(PcdSystemMiscConfig);
  DEBUG((EFI_D_INFO, "MiscConfig:%X\n", MiscConfig));
  if((MiscConfig & SYS_MISC_CFG_STS_ONLY_VIRTUAL_USB_KB) || (MiscConfig & SYS_MISC_CFG_STS_NO_KB)){
    IsNoKB = TRUE;
  }

  IsSataErr = IsAnySataPortErr();

  DEBUG((EFI_D_INFO, "IsCmosBad:%d, IsNoKB:%d, IsSataErr:%d\n", IsCmosBad, IsNoKB, IsSataErr));

  if(!IsCmosBad && !IsNoKB && !IsSataErr){
    return;
  }

  BltSaveAndRetore(gBS, TRUE);

  ShowBiosBaseInfo();

  ConOut = gST->ConOut;

  if(IsNoKB){
    gScreenY++;
    ConOut->SetCursorPosition(ConOut, TXT_SCREEN_START_X, gScreenY);
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_NO_USB_KEYBOARD), NULL);
    ConOut->OutputString(ConOut, Str); 
    FreePool(Str);
  }

  if(IsCmosBad){
    gScreenY++;
    ConOut->SetCursorPosition(ConOut, TXT_SCREEN_START_X, gScreenY);
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_CMOS_BAD), NULL);
    ConOut->OutputString(ConOut, Str); 
    FreePool(Str);    
    NeedHalt = 1;
  }

  if(IsSataErr){
    SataErrCtx = gSataPortErrCtx;

    ErrCmd = HiiGetString(gHiiHandle, STRING_TOKEN(STR_HDD_CMD_ERR), NULL);
    ErrSmart = HiiGetString(gHiiHandle, STRING_TOKEN(STR_HDD_SMART_ERR), NULL);
    
    for(Index=0;Index<gSataPortErrCtxCount;Index++){
      if(!IsThisSataPortErr(&SataErrCtx[Index])){
        continue;
      }

      Status = gBS->HandleProtocol(SataErrCtx[Index].Host, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
      PciIo->GetLocation(PciIo, &s, &b, &d, &f);

      if(SataErrCtx[Index].HddErrCount){
        SataCmdErr = ErrCmd;
      } else {
        SataCmdErr = L"";
      }
      if(SataErrCtx[Index].SmartErrCount){
        SataSmartErr = ErrSmart;
      } else {
        SataSmartErr = L"";
      }      
      
      gScreenY++;
      ConOut->SetCursorPosition(ConOut, TXT_SCREEN_START_X, gScreenY);
      Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_SATA_ERR), NULL);
      UnicodeSPrint(Buffer, sizeof(Buffer), Str, b, d, f, SataErrCtx[Index].Port+1, SataCmdErr, SataSmartErr);
      ConOut->OutputString(ConOut, Buffer);
      FreePool(Str);
      NeedHalt = 1;
    }

    FreePool(ErrCmd);
    FreePool(ErrSmart);
  }

  if(NeedHalt){

    gScreenY += 2;
    ConOut->SetCursorPosition(ConOut, TXT_SCREEN_START_X, gScreenY);
    Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_KEY_CONTINUE), NULL);
    ConOut->OutputString(ConOut, Str); 
    FreePool(Str);

    Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&PlatHostInfo);
    if(!EFI_ERROR(Status)){
      if((PlatHostInfo !=NULL) &&(PlatHostInfo->StopWdg !=NULL)){
         PlatHostInfo->StopWdg();
      }
    }
    
    while (TRUE) {
    	Status = gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
      if(EFI_ERROR(Status)){
        break;
      }
      Status = gST->ConIn->ReadKeyStroke(gST->ConIn, &Key); 
      if(Key.ScanCode == SCAN_NULL && Key.UnicodeChar == CHAR_CARRIAGE_RETURN){
        break;
      }    
    }
  } else {
    gBS->Stall(1000000);
  }

  BltSaveAndRetore(gBS, FALSE);
}





SATA_PORT_ERR_CTX *GetSataPortErr(EFI_HANDLE Host, UINTN Port)
{
  UINTN               Index;
  SATA_PORT_ERR_CTX   *Ctx;

  Ctx = gSataPortErrCtx;
  for(Index=0;Index<gSataPortErrCtxCount;Index++){
    if(Ctx[Index].Host == Host && Ctx[Index].Port == Port){
      return &Ctx[Index];
    }
  }

  return NULL;
}

VOID IncSataPortErr(SATA_PORT_ERR_CTX *Ctx, EFI_HANDLE Host, UINTN Port, BOOLEAN IsSmart)
{
  if(Ctx == NULL){
    if(gSataPortErrCtxCount >= MAX_SATA_PORTS_SUPPORT){
      return;
    }
    Ctx = &gSataPortErrCtx[gSataPortErrCtxCount];
    Ctx->Host = Host;
    Ctx->Port = Port;
    if(IsSmart){
      Ctx->SmartErrCount++;
    } else {
      Ctx->HddErrCount++;
    }
    gSataPortErrCtxCount++;

  } else {
    if(IsSmart){
      Ctx->SmartErrCount++;
    } else {
      Ctx->HddErrCount++;
    }
  }
}



STATIC
EFI_STATUS
EFIAPI
StatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
  ATA_DEVICE_ERROR_INFO   *AtaErrInfo;
  UINT16                  PortNo;
  SATA_PORT_ERR_CTX       *Ctx;  


  if((CodeType & EFI_STATUS_CODE_TYPE_MASK) != EFI_ERROR_CODE){
    return EFI_UNSUPPORTED;
  }

  switch (Value) {

    case DXE_ATA_DEVICE_FAILURE:
      AtaErrInfo = (ATA_DEVICE_ERROR_INFO*)((UINT8*)Data + Data->HeaderSize);
      if(AtaErrInfo->Signature == ATA_DEVICE_ERROR_INFO_TAG){
        if(AtaErrInfo->IsIdeMode){
          PortNo = AtaErrInfo->Port * 2 + AtaErrInfo->PortMp;
        } else {
          PortNo = AtaErrInfo->Port;
        }
        Ctx = GetSataPortErr(AtaErrInfo->Host, PortNo);
        IncSataPortErr(Ctx, AtaErrInfo->Host, PortNo, 0);
      }
      break;

    case (EFI_IO_BUS_ATA_ATAPI | EFI_IOB_ATA_BUS_SMART_OVERTHRESHOLD):
      AtaErrInfo = (ATA_DEVICE_ERROR_INFO*)((UINT8*)Data + Data->HeaderSize);
      if(AtaErrInfo->Signature == ATA_DEVICE_ERROR_INFO_TAG){
        if(AtaErrInfo->IsIdeMode){
          PortNo = AtaErrInfo->Port * 2 + AtaErrInfo->PortMp;
        } else {
          PortNo = AtaErrInfo->Port;
        }
        Ctx = GetSataPortErr(AtaErrInfo->Host, PortNo);
        IncSataPortErr(Ctx, AtaErrInfo->Host, PortNo, 1);
      }
      break;      
      
  }
  
  return EFI_SUCCESS;
}



STATIC
VOID
EFIAPI
OnRscHandlerReady (
  IN  EFI_EVENT       Event,
  IN  VOID            *Context
  )
{
  EFI_STATUS                Status;
  EFI_RSC_HANDLER_PROTOCOL  *RscHandle;

  
  Status = gBS->LocateProtocol(
                  &gEfiRscHandlerProtocolGuid,
                  NULL,
                  (VOID **)&RscHandle
                  );
  if(EFI_ERROR(Status)){
    return;
  }
  gBS->CloseEvent(Event);

  gRscHandle = RscHandle;
  gRscHandle->Register(StatusCodeReportWorker, TPL_HIGH_LEVEL);
}


VOID
EFIAPI
OnExitBootServices (
  IN  EFI_EVENT   Event,
  IN  void        *Context
  )
{
  if (gRscHandle != NULL) {
    gRscHandle->Unregister(StatusCodeReportWorker);
  }
}



EFI_STATUS
EFIAPI
PostErrorDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                Status;
  VOID                      *Registration;
  EFI_EVENT                 Event;


  ZeroMem(gSataPortErrCtx, sizeof(gSataPortErrCtx));

  gHiiHandle = HiiAddPackages (
                 &gEfiCallerIdGuid,
                 gImageHandle,
                 STRING_ARRAY_NAME,
                 NULL
                 );
  ASSERT(gHiiHandle != NULL);

  EfiCreateProtocolNotifyEvent (
    &gEfiRscHandlerProtocolGuid,
    TPL_CALLBACK,
    OnRscHandlerReady,
    NULL,
    &Registration
    );

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  OnExitBootServices,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &Event
                  );
  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gPlatAfterConsoleEndProtocolGuid, AfterConsoleEndHook,
                  NULL
                  );

  ASSERT(!EFI_ERROR (Status));
  return EFI_SUCCESS;
}


