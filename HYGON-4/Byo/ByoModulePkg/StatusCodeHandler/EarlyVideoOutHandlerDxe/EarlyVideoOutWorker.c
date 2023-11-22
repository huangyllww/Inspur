/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  EarlyVideoOutWorker.c
**/

#include <PiDxe.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciIo.h>
#include <Protocol/ByoEarlySimpleTextOutProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PostCodeMapLib.h>

BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL *mByoEarlySimpleTextOutProtocol = NULL;
extern BOOLEAN                   gHotKeyBoot;

static
void 
Hex2Char(UINT8 Value, CHAR8 *s)
{
  UINT8 Data;

  Data = (Value >> 4) & 0xf;
  if(Data >= 0 && Data <= 9){
    s[0] = Data + '0';
  } else if(Data >= 0xa && Data <= 0xf){
    s[0] = Data - 0xa + 'A';
  }

  Data = Value & 0xf;
  if(Data >= 0 && Data <= 9){
    s[1] = Data + '0';
  } else if(Data >= 0xa && Data <= 0xf){
    s[1] = Data - 0xa + 'A';
  }
  s[2] = 0;
  
  return;
}

static void ShowPort80Code (UINT8 Port80Code)
{
  EFI_STATUS     Status;
  static UINT32  Column = 0;
  static UINT32  Row = 0;
  CHAR8          Code[3];
  
  if (Column == 0) {
    Status = mByoEarlySimpleTextOutProtocol->GetMode(&Column, &Row);
    DEBUG((EFI_D_INFO, "early %d x %d %r\n", Column, Row, Status));
    if (EFI_ERROR (Status)) {
      return;
    }
  }

  Hex2Char(Port80Code, Code);
  mByoEarlySimpleTextOutProtocol->OutputString((UINT16)(Column - 4), (UINT16)(Row - 2), Code);
}


void GetEarlyVideoProtocol()
{
  if(mByoEarlySimpleTextOutProtocol == NULL){
    gBS->LocateProtocol (
           &gByoEarlySimpleTextOutProtocolGuid,
           NULL,
           (VOID**)&mByoEarlySimpleTextOutProtocol
           );
  }
}


void EarlyVideoOut(UINT8 Port80Code)
{
  if(gHotKeyBoot){
    return;
  }
  
  GetEarlyVideoProtocol();
  if(mByoEarlySimpleTextOutProtocol != NULL){
    ShowPort80Code(Port80Code);
  }  
}

VOID AfterConnectPciRootBridgeHook()
{
  EFI_STATUS                   Status;
  UINTN                        HandleCount;
  EFI_HANDLE                   *HandleBuffer = NULL;
  UINTN                        Index;
  EFI_PCI_IO_PROTOCOL          *ptPciIo;
  UINT16                       PciId;
  UINT8                        ClassCode[3]; 
  UINT64                       MemBar = 0;

  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateHandleBuffer(
                  ByProtocol, 
                  &gEfiPciIoProtocolGuid, 
                  NULL, 
                  &HandleCount, 
                  &HandleBuffer
                  );
  if(EFI_ERROR(Status) || HandleCount == 0){
    DEBUG((EFI_D_ERROR, "(L%d) %r %d\n", __LINE__, Status, HandleCount));
    goto ProcExit;
  }

  for(Index = 0; Index < HandleCount; Index ++) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid, 
                    (VOID **)&ptPciIo
                    );
    ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, &ClassCode[0]);
    if(ClassCode[2] != 3){
      continue;
    }
    ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint16, PCI_VENDOR_ID_OFFSET, 1, &PciId);
    if(PciId != 0x1A03){
      continue;
    }

    ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint32, PCI_BASE_ADDRESSREG_OFFSET, 1, &MemBar);
    GetEarlyVideoProtocol();
    if (MemBar && mByoEarlySimpleTextOutProtocol != NULL) {
      mByoEarlySimpleTextOutProtocol->SetFrameBuffer(MemBar);
    }
    break;
  }

  if (MemBar == 0) { // not find BMC VGA
    if(HandleBuffer != NULL) {
      gBS->FreePool(HandleBuffer);
      HandleBuffer = NULL;
    }
    DEBUG ((EFI_D_INFO,"Unregister early vidoe Port80StatusCodeReportWorker\n"));
    Status = gBS->LocateHandleBuffer(
                    ByProtocol, 
                    &gByoEarlySimpleTextOutProtocolGuid, 
                    NULL, 
                    &HandleCount, 
                    &HandleBuffer
                    );
    if(EFI_ERROR(Status) || HandleCount == 0){
      DEBUG((EFI_D_ERROR, "(L%d) %r %d\n", __LINE__, Status, HandleCount));
      goto ProcExit;
    }

    for(Index = 0; Index < HandleCount; Index ++){
      Status = gBS->UninstallProtocolInterface (
                      HandleBuffer[Index],
                      &gByoEarlySimpleTextOutProtocolGuid,
                      NULL
                      );
      mByoEarlySimpleTextOutProtocol = NULL;
    }
  }
  
ProcExit:
  if(HandleBuffer!=NULL){
    gBS->FreePool(HandleBuffer);
  }
}

EFI_STATUS
EFIAPI
EarlyVideoStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
  UINT8 Port80Code;
  
  Port80Code = (UINT8) GetPostCodeFromStatusCode (CodeType, Value);
  if (Port80Code != 0) {
    EarlyVideoOut(Port80Code);
  }

  return EFI_SUCCESS;
}
