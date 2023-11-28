/*++

  Copyright (c) 2006 - 2019 Byosoft Corporation. All rights reserved.
  This program and associated documentation (if any) is furnished
  under a license. Except as permitted by such license,no part of this
  program or documentation may be reproduced, stored divulged or used
  in a public system, or transmitted in any form or by any means
  without the express written consent of Byosoft Corporation.

Module Name:
  PostError.c

Abstract:
  Implements the programming of Post Error.

Revision History:

--*/

#include <Guid/MdeModuleHii.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>
#include <IndustryStandard/Pci.h>
#include "PciListDxe.h"


EFI_HII_HANDLE  gHiiHandle;
EFI_GUID gByoSetupPciListFormsetGuid = SETUP_PCI_LIST_FORMSET_GUID;
extern unsigned char PciListBin[];

typedef struct {
  UINT16   VendorId;
  UINT16   DeviceId;
  CHAR16   *DeviceName;
} PCI_DEV_ID_NAME;

typedef struct {
  UINT8  Bus;
  UINT8  Dev;
  UINT8  Func;
  UINT8  ClassCode[3];  
  UINT32 Id;
} PCI_IO_CTX;

CHAR16 *GetClassCodeName(UINT8 ClassCode[3]);

extern PCI_DEV_ID_NAME gPciDevIdNameList[];
extern UINTN gPciDevIdNameListCount;

/*
CHAR16 *GetDevName(UINT32 PciId, UINT8 ClassCode[3])
{
  UINTN   Index;
  UINT16  *Id;
  CHAR16  *s;

  
  Id = (UINT16*)&PciId;
  for(Index=0;Index<gPciDevIdNameListCount;Index++){
    if(Id[0] == gPciDevIdNameList[Index].VendorId && Id[1] == gPciDevIdNameList[Index].DeviceId){
      return gPciDevIdNameList[Index].DeviceName;
    }
  }

  s = GetDevNameByClassCode(ClassCode);
  if(s == NULL){
    return L"Unknown";
  } else {
    return s;
  }
}
*/


VOID
CreateForm ()
{
  VOID                  *StartOpCodeHandle = NULL;
  VOID                  *EndOpCodeHandle   = NULL;
  EFI_IFR_GUID_LABEL    *StartLabel;
  EFI_IFR_GUID_LABEL    *EndLabel;
  EFI_STATUS            Status;
  UINTN                 HandleCount;
  EFI_HANDLE            *Handles = NULL;
  UINTN                 Index;
  EFI_STRING_ID         StrId;
  UINT8                 ClassCode[3];
  CHAR16                StrBuffer[128];
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINT32                PciId;
  UINTN                 s, b, d, f;
  PCI_IO_CTX            *PciIoCtx = NULL;
  UINTN                 PciIoCtxCount = 0;
  PCI_IO_CTX            *p;
  PCI_IO_CTX            *PciIoCtxTemp = NULL;
  UINTN                 i, j;
  EFI_STRING_ID         HelpToken;
  

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  PciIoCtx = AllocatePool(sizeof(PCI_IO_CTX) * HandleCount);
  if(PciIoCtx == NULL){
    goto ProcExit;
  }

  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_PCI_LIST_START;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_PCI_LIST_END;

  for(Index=0;Index<HandleCount;Index++){

    Status = gBS->HandleProtocol(
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    &PciIo
                    );

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &PciId);
    if((UINT16)PciId == 0xFFFF){          // ignore unexisted
      continue;
    }
    
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, ClassCode);
    if(ClassCode[2] == 6){          // ignore bridge
      continue;
    }
    if(ClassCode[2] == 0x13 && ClassCode[1] == 0 && ClassCode[0] == 0){         // Non-Essential Instrumentation
      continue;
    }
    if(ClassCode[2] == 0x10 && ClassCode[1] == 0x80 && ClassCode[0] == 0){      // Other en/decryption
      continue;
    }    
 
    PciIo->GetLocation(PciIo, &s, &b, &d, &f);

    p = &PciIoCtx[PciIoCtxCount];
    p->Bus  = (UINT8)b;
    p->Dev  = (UINT8)d;
    p->Func = (UINT8)f;
    p->Id   = PciId;
    CopyMem(p->ClassCode, ClassCode, sizeof(p->ClassCode));
    PciIoCtxCount++;

  }

  if(PciIoCtxCount == 0){
    goto ProcExit;
  }

  PciIoCtxTemp = AllocatePool(sizeof(PCI_IO_CTX) * PciIoCtxCount);
  if(PciIoCtxTemp == NULL){
    goto ProcExit;;
  }

  i = 0;  
  for(Index=0;Index<256;Index++){          // bus no
    for(j=0;j<PciIoCtxCount;j++){
      if(PciIoCtx[j].Bus == Index){
        CopyMem(&PciIoCtxTemp[i++], &PciIoCtx[j], sizeof(PCI_IO_CTX));
        if(i == PciIoCtxCount){
          break;
        }
      }
    }
  }
  ASSERT(i == PciIoCtxCount);

  FreePool(PciIoCtx);
  PciIoCtx = PciIoCtxTemp;
  PciIoCtxTemp = NULL;

  HelpToken = HiiSetString(gHiiHandle, 0, L"", NULL);

//BUS DEV FUN   VendorID  DeviceID  DeviceClass
//00  00  00    0000      0000      abcd
  for(Index=0;Index<PciIoCtxCount;Index++){
    p = &PciIoCtx[Index];
    UnicodeSPrint(
      StrBuffer, 
      sizeof(StrBuffer), 
      L"%02X  %02X  %02X    %04X      %04X      %s", 
      p->Bus, p->Dev, p->Func,
      p->Id & 0xFFFF,      
      (p->Id >> 16) & 0xFFFF,
      GetClassCodeName(p->ClassCode)
      );
    StrId = HiiSetString (
              gHiiHandle,
              0,
              StrBuffer,
              NULL
              );
    HiiCreateActionOpCode (
      StartOpCodeHandle,
      (UINT16)(0xF000 + Index),
      StrId,
      HelpToken,
      EFI_IFR_FLAG_CALLBACK,
      0
      );
    
  }

  HiiUpdateForm (
    gHiiHandle,
    &gByoSetupPciListFormsetGuid,   // Formset GUID
    PCI_LIST_FORM_ID,               // Form ID
    StartOpCodeHandle,              // Label for where to insert opcodes
    EndOpCodeHandle                 // Replace data
    );

ProcExit:
  if(StartOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(StartOpCodeHandle);
  }
  if(EndOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(EndOpCodeHandle);
  }
  if(PciIoCtx != NULL){
    FreePool(PciIoCtx);
  }
  if(PciIoCtxTemp != NULL){
    FreePool(PciIoCtxTemp);
  }  
}



VOID
EFIAPI
EnterSetupCallback (
  IN EFI_EVENT    Event,
  IN VOID*        Context
  )
{
  EFI_STATUS            Status;
  VOID                  *Interface;

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);
  
  CreateForm();
}







EFI_STATUS
PciListDxeEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  VOID      *Registration;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  gHiiHandle = HiiAddPackages (
                 &gByoSetupPciListFormsetGuid,
                 NULL,
                 STRING_ARRAY_NAME,
                 PciListBin,
                 NULL
                 );
  ASSERT(gHiiHandle != NULL);

  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    EnterSetupCallback,
    NULL,
    &Registration
    );

  return EFI_SUCCESS;
}


