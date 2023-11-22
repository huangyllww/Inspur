/** @file

Copyright (c) 2006 - 2022 Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  AdvancedFormCallback.c

Abstract:
  AdvancedFormCallback Setup Rountines

Revision History:


**/

#include <PlatformSetupDxe.h>
#include <Library/ByoCommLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/UsbIo.h>
#include <Protocol/DiskInfo.h>


typedef struct {
  BOOLEAN               Init;
  VOID                  *EndOpCodeHandle;
  EFI_HII_HANDLE        HiiHandle;
  CHAR16                Buffer[256];
} USB_INFO_REFRESH_CTX;

USB_INFO_REFRESH_CTX gUsbInfoRefreshCtx = {
  FALSE,
  NULL,
  NULL,
  {0, },
};

VOID UsbInfoCtxInit(IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This)
{
  SETUP_FORMSET_INFO            *SetupFormSet;
  USB_INFO_REFRESH_CTX          *Ctx = &gUsbInfoRefreshCtx;
  EFI_IFR_GUID_LABEL            *EndLabel;
  
  SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
  Ctx->HiiHandle = SetupFormSet->HiiHandle;

  Ctx->EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (Ctx->EndOpCodeHandle != NULL);  

  EndLabel = (EFI_IFR_GUID_LABEL*)HiiCreateGuidOpCode(Ctx->EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = USB_DEV_LIST_LABEL_END;

  Ctx->Init = TRUE;
}




#define CLASS_HID           3
#define SUBCLASS_BOOT       1
#define PROTOCOL_KEYBOARD   1
#define PROTOCOL_MOUSE      2

EFI_STRING_ID gUsbStrList[] = {
  STRING_TOKEN(STR_USB_STORAGE),
  STRING_TOKEN(STR_USB_KEYBOARD),
  STRING_TOKEN(STR_USB_MOUSE),
  STRING_TOKEN(STR_USB_OTHERS)
};


EFI_STATUS
GetUsbNameByDiskInfo (
    EFI_HANDLE       UsbIoHandle,
    CHAR8            **UsbName
  )
{ 
  EFI_DEVICE_PATH_PROTOCOL                *Dp;
  EFI_HANDLE                              UsbHandle;
  USB_BOOT_INQUIRY_DATA                   UsbInquiryData;
  UINT32                                  BufferSize = 0;
  EFI_STATUS                              Status;
  EFI_DISK_INFO_PROTOCOL                  *DiskInfo;
  EFI_BLOCK_IO_PROTOCOL                   *BlockIo;
  UINT64                                  DiskSize = 0;
  UINT32                                  DiskSize100MB = 0;
  CHAR8                                   VendorID[9];
  CHAR8                                   ProductID[17];
  CHAR8                                   Buffer[48];
  EFI_OPEN_PROTOCOL_INFORMATION_ENTRY     *OpenInfoBuffer;
  UINTN                                   EntryCount;
  UINTN                                   Index;
  EFI_DEVICE_PATH_PROTOCOL                *ChildDevicePath;
  EFI_HANDLE                              TempHandle = UsbIoHandle;

  Status = gBS->HandleProtocol (
                  UsbIoHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**)&Dp
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->LocateDevicePath(&gEfiDiskInfoProtocolGuid, &Dp, &UsbHandle);
  
  if(!EFI_ERROR (Status)) {
    Status = gBS->HandleProtocol (
                    UsbHandle,
                    &gEfiDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );

    Status = gBS->HandleProtocol (
                    UsbHandle,
                    &gEfiBlockIoProtocolGuid,
                    (VOID**)&BlockIo
                    );
  } else {
    Status = gBS->OpenProtocolInformation (
                    UsbIoHandle,
                    &gEfiUsbIoProtocolGuid,
                    &OpenInfoBuffer,
                    &EntryCount
                    );
    if (!EFI_ERROR (Status)) {
      for (Index = 0; Index < EntryCount; Index++) {
        if ((OpenInfoBuffer[Index].Attributes & EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER) != 0) {
          Status = gBS->OpenProtocol (
                          OpenInfoBuffer[Index].ControllerHandle,
                          &gEfiDevicePathProtocolGuid,
                          (VOID **) &ChildDevicePath,
                          NULL,
                          NULL,
                          EFI_OPEN_PROTOCOL_GET_PROTOCOL
                          );
          if (EFI_ERROR (Status)) {
            continue;
          }
          //DEBUG ((DEBUG_INFO, "Child DevicePath is:\n"));
          //ShowDevicePathDxe (gBS, ChildDevicePath);
          Status = gBS->OpenProtocol (
                          OpenInfoBuffer[Index].ControllerHandle,
                          &gEfiDiskInfoProtocolGuid,
                          NULL,
                          NULL,
                          NULL,
                          EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                          );
          if (!EFI_ERROR (Status)) {
            TempHandle = OpenInfoBuffer[Index].ControllerHandle;
            break;
          }
        }
      }
    }
    Status = gBS->HandleProtocol (
                    TempHandle,
                    &gEfiDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );

    Status = gBS->HandleProtocol (
                    TempHandle,
                    &gEfiBlockIoProtocolGuid,
                    (VOID**)&BlockIo
                    );
  }
  
  if(!EFI_ERROR(Status)){
    DiskSize = MultU64x32(BlockIo->Media->LastBlock+1, BlockIo->Media->BlockSize);
    DiskSize100MB = (UINT32)DivU64x64Remainder (DiskSize, 100000000, NULL);
  }

  BufferSize = sizeof(USB_BOOT_INQUIRY_DATA);
  Status = DiskInfo->Inquiry(
                     DiskInfo,
                     &UsbInquiryData,
                     &BufferSize
                     );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "%a (L%d) %r\n", __FUNCTION__, __LINE__, Status));
    return Status;
  }

  CopyMem(VendorID, UsbInquiryData.VendorID, 8);
  VendorID[8] = 0;
  CopyMem(ProductID, UsbInquiryData.ProductID, 16);
  ProductID[16] = 0;
  AsciiSPrint(Buffer, sizeof(Buffer), "%a %a %d.%dGB", VendorID, ProductID, DiskSize100MB/10, DiskSize100MB%10);
  TrimStr8(Buffer);
  *UsbName = AllocateCopyPool(AsciiStrSize(Buffer), Buffer);

  return EFI_SUCCESS;
}



CHAR8*
MyGetUsbDescription (
  IN EFI_USB_IO_PROTOCOL  *UsbIo
  )
{
  EFI_STATUS                   Status;
  CHAR16                       *Manufacturer = NULL;
  CHAR16                       *Product      = NULL;
  CHAR8                        *Description;
  EFI_USB_DEVICE_DESCRIPTOR    DevDesc;
  UINTN                        DescMaxSize;
  UINT16                       UsbLangId = 0x0409; // English
  BOOLEAN                      NeedFree1 = FALSE;
  BOOLEAN                      NeedFree2 = FALSE;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Status = UsbIo->UsbGetStringDescriptor (
                    UsbIo,
                    UsbLangId,
                    DevDesc.StrManufacturer,
                    &Manufacturer
                    );
  if (EFI_ERROR (Status)) {
    Manufacturer = L"";
  } else {
    NeedFree1 = TRUE;
  }

  Status = UsbIo->UsbGetStringDescriptor (
                    UsbIo,
                    UsbLangId,
                    DevDesc.StrProduct,
                    &Product
                    );
  if (EFI_ERROR (Status)) {
    Product = L"";
  } else {
    NeedFree2 = TRUE;
  }

  if (!NeedFree1 && !NeedFree2) {
    DescMaxSize = 128;
    Description = AllocateZeroPool(DescMaxSize);
    ASSERT (Description != NULL);
    AsciiSPrint (
      Description,
      DescMaxSize,
      "VendorId:0x%04X ProductId:0x%04X",
      DevDesc.IdVendor,
      DevDesc.IdProduct
      );
    return Description;
  }

  DescMaxSize = StrLen(Manufacturer) + 1 + StrLen(Product) + 1;
  Description = AllocateZeroPool(DescMaxSize);
  ASSERT (Description != NULL);
  AsciiSPrint(
    Description,
    DescMaxSize,
    "%s %s",
    Manufacturer,
    Product
    );

  TrimStr8(Description);
  if(NeedFree1)FreePool(Manufacturer);
  if(NeedFree2)FreePool(Product);
  return Description;
}



VOID
UpdateUsbPortInfo (
    VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           HandleCount;
  EFI_HANDLE                      *Handles;
  EFI_USB_IO_PROTOCOL             *UsbIo;
  UINTN                           Index;
  UINT8                           UsbType;
  CHAR8                           *Description;
  EFI_USB_INTERFACE_DESCRIPTOR    Interface;
  EFI_STRING_ID                   StrRef;
  CHAR16                          *UsbTypeStr;
  USB_INFO_REFRESH_CTX            *Ctx = &gUsbInfoRefreshCtx;
  EFI_IFR_GUID_LABEL              *StartLabel;
  VOID                            *StartOpCodeHandle;


  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);
  
  StartLabel = (EFI_IFR_GUID_LABEL*)HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = USB_DEV_LIST_LABEL;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiUsbIoProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiUsbIoProtocolGuid,
                    (VOID **) &UsbIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &Interface);
    if (EFI_ERROR (Status)) {
      continue;
    }

    if(Interface.InterfaceClass == USB_MASS_STORE_CLASS){
      Status = GetUsbNameByDiskInfo(Handles[Index], &Description);
      if (EFI_ERROR (Status)) {
        continue;
      }
    } else {
      Description = MyGetUsbDescription(UsbIo);
      if (Description == NULL) {
        continue;
      }
    }

    if (Interface.InterfaceClass == USB_MASS_STORE_CLASS) {
      UsbType = 0;
    } else if (Interface.InterfaceClass == CLASS_HID &&
              Interface.InterfaceSubClass == SUBCLASS_BOOT &&
              Interface.InterfaceProtocol == PROTOCOL_KEYBOARD) {
      UsbType = 1;
    } else if (Interface.InterfaceClass == CLASS_HID &&
              Interface.InterfaceSubClass == SUBCLASS_BOOT &&
              Interface.InterfaceProtocol == PROTOCOL_MOUSE) {
      UsbType = 2;
    } else if (Interface.InterfaceClass == 0x09) {
      if (Description != NULL) {
        FreePool (Description);
      }
      continue;  //Skip USB HUB
    } else if (Interface.InterfaceClass == CLASS_HID && 
              Interface.InterfaceSubClass == 0 &&
              Interface.InterfaceProtocol == 0) {
      if (Description != NULL) {
        FreePool (Description);
      }
      continue;  //Skip Duplicate keyboard device
    } else {
      UsbType = 3;
    }

    if (AsciiStrStr(Description, "Virtual") != NULL || AsciiStrStr(Description, "virtual") != NULL) {
      DEBUG((EFI_D_INFO, "skip bmc virtual usb\n"));
      if (Description != NULL) {
        FreePool (Description);
      }
      continue;
    }

    UsbTypeStr = HiiGetString(Ctx->HiiHandle, gUsbStrList[UsbType], NULL);
    UnicodeSPrint(Ctx->Buffer, sizeof(Ctx->Buffer), L"%s : %a", UsbTypeStr, Description);
    StrRef = HiiSetString(Ctx->HiiHandle, 0, Ctx->Buffer, NULL);
    FreePool(UsbTypeStr);

    HiiCreateTextOpCode (
      StartOpCodeHandle,
      StrRef,
      STRING_TOKEN(STR_EMPTY),
      STRING_TOKEN(STR_EMPTY)
      );

    if (Description != NULL) {
      FreePool(Description);
    }
  }

  Status = HiiUpdateForm(Ctx->HiiHandle, NULL, USB_FORM_ID, StartOpCodeHandle, Ctx->EndOpCodeHandle);

  HiiFreeOpCodeHandle(StartOpCodeHandle);
  if (HandleCount != 0) {
    FreePool(Handles);
  }
}




EFI_STATUS
EFIAPI
DevicesFormCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_BROWSER_ACTION                   Action,
  IN EFI_QUESTION_ID                      KeyValue,
  IN UINT8                                Type,
  IN EFI_IFR_TYPE_VALUE                   *Value,
  OUT EFI_BROWSER_ACTION_REQUEST          *ActionRequest
  )
{
  if(Action == EFI_BROWSER_ACTION_RETRIEVE){
    if(KeyValue == KEY_USB_DEV_LIST){
      DEBUG((EFI_D_INFO, "usb refresh\n"));
      if(!gUsbInfoRefreshCtx.Init){
        UsbInfoCtxInit(This);
      }
      UpdateUsbPortInfo();
    }
  }

  return EFI_UNSUPPORTED;
}


