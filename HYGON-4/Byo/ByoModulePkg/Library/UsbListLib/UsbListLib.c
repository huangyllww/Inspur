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
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ByoCommLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Protocol/BlockIo.h>
#include <Protocol/UsbIo.h>
#include <Protocol/DiskInfo.h>
#include <ByoPlatformSetupConfig.h>
#include <Library/HiiLib.h>
#include <Guid/MdeModuleHii.h>
#include <Uefi/UefiSpec.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UsbListLib.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Library/PlatformLanguageLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePath.h>

EFI_GUID          gUsbListLibGuid = { 0x5bbda327, 0xadd8, 0x777e, { 0x94, 0x0b, 0x2d, 0x65, 0xe4, 0x2c, 0x4f, 0x88 } };
EFI_HII_HANDLE    mUsbListLibStringHandle;

LIST_ENTRY mUsbDeviceList = INITIALIZE_LIST_HEAD_VARIABLE (mUsbDeviceList);

EFI_STRING_ID gUsbListInfo[] = {
  STRING_TOKEN(STR_USB_STORAGE1),
  STRING_TOKEN(STR_USB_KEYBOARD1),
  STRING_TOKEN(STR_USB_MOUSE1),
  STRING_TOKEN(STR_USB_OTHERS1),
  STRING_TOKEN(STR_USB_HUB1)
};

#define CLASS_HID           3
#define SUBCLASS_BOOT       1
#define PROTOCOL_KEYBOARD   1
#define PROTOCOL_MOUSE      2

typedef struct {
  BOOLEAN               Init;
  VOID                  *EndOpCodeHandle;
  EFI_HII_HANDLE        HiiHandle;
  CHAR16                Buffer[256];
} USB_INFO_REFRESH_CTX;

typedef struct {
  LIST_ENTRY                    Link;
  UINTN                         Num;
  UINTN                         NoBus;
  UINTN                         Bus;
  UINTN                         Dev;
  UINTN                         Func;
  UINTN                         Speed;
  UINTN                         UsbType;
  UINTN                         ParentPortNumber;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  UINTN                         IsDebugged;
  UINT16                        IdProduct;
  UINT16                        IdVendor;
  CHAR16                        *UsbTypeStr;
  CHAR8                         *Description;
} DEBUG_USB_LIST_INFO;

USB_INFO_REFRESH_CTX gUsbListInfoRefreshCtx = {
  FALSE,
  NULL,
  NULL,
  {0, },
};

DEBUG_USB_LIST_INFO            mUsbListInfo;


EFI_STATUS
EFIAPI
UsbListLibConstructor (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{

  InitializeLanguage(TRUE);
  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  mUsbListLibStringHandle = HiiAddPackages (&gUsbListLibGuid, ImageHandle, UsbListLibStrings, NULL);
  ASSERT (mUsbListLibStringHandle != NULL);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UsbListLibDestructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{  
  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  HiiRemovePackages(mUsbListLibStringHandle);


  return EFI_SUCCESS;
}


VOID UsbListInfoCtxInit(
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_HII_HANDLE    HiiHandle
  )
{
  USB_INFO_REFRESH_CTX          *Ctx = &gUsbListInfoRefreshCtx;
  EFI_IFR_GUID_LABEL            *EndLabel;
  if(HiiHandle == NULL){
    SETUP_FORMSET_INFO            *SetupFormSet;
    SetupFormSet = BYO_FORMSET_INFO_FROM_THIS(This);
    Ctx->HiiHandle = SetupFormSet->HiiHandle;
  }else{
    Ctx->HiiHandle = HiiHandle;
  }

  Ctx->EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (Ctx->EndOpCodeHandle != NULL);  

  EndLabel = (EFI_IFR_GUID_LABEL*)HiiCreateGuidOpCode(Ctx->EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof(EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = USB_DEV_LIST_LABEL_END;

  Ctx->Init = TRUE;
}


EFI_STATUS
GetUsbListNameByDiskInfo (
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
                    UsbIoHandle,
                    &gEfiDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );

    Status = gBS->HandleProtocol (
                    UsbIoHandle,
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

UINTN
FindUsbPort (
  IN VOID            *DevPath
  )
{

  USB_DEVICE_PATH  *Usb;

  Usb = DevPath;
  return  Usb->ParentPortNumber;
}

CHAR8*
GetUsbListDescription (
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
UpdateUsbListInfo (
  IN UINT16           USBFORMID
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
  USB_INFO_REFRESH_CTX            *Ctx = &gUsbListInfoRefreshCtx;
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
      Status = GetUsbListNameByDiskInfo(Handles[Index], &Description);
      if (EFI_ERROR (Status)) {
        continue;
      }
    } else {
      Description = GetUsbListDescription(UsbIo);
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

    UsbTypeStr = HiiGetString(mUsbListLibStringHandle, gUsbListInfo[UsbType], NULL);
    UnicodeSPrint(Ctx->Buffer, sizeof(Ctx->Buffer), L"%s : %a", UsbTypeStr, Description);
    StrRef = HiiSetString(Ctx->HiiHandle, 0, Ctx->Buffer, NULL);
    FreePool(UsbTypeStr);

    HiiCreateTextOpCode (
      StartOpCodeHandle,
      StrRef,
      0,
      0
      );

    if (Description != NULL) {
      FreePool(Description);
    }
  }

  Status = HiiUpdateForm(Ctx->HiiHandle, NULL, USBFORMID, StartOpCodeHandle, Ctx->EndOpCodeHandle);

  HiiFreeOpCodeHandle(StartOpCodeHandle);
  if (HandleCount != 0) {
    FreePool(Handles);
  }
}

VOID
EFIAPI
UpdateUsbList (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL *This,
  IN EFI_HII_HANDLE    HiiHandle,
  IN UINT16            USBFORMID
  )
{
  // used for usb.sd
  if(!gUsbListInfoRefreshCtx.Init){
    UsbListInfoCtxInit(This,HiiHandle);
  }
  UpdateUsbListInfo(USBFORMID);
}


VOID
DebugUsbList (
  IN UINTN            Index
  )
{
  UINTN                                   j = 0;
  UINTN                                   k = 0;
  LIST_ENTRY                              *Entry;
  DEBUG_USB_LIST_INFO                     *UsbDeviceList;

  Entry = GetFirstNode (&mUsbDeviceList);
  for(k = 0; k < Index; k++){
    Entry = Entry->ForwardLink;
  }
  UsbDeviceList = (DEBUG_USB_LIST_INFO *) Entry;
  
  if(UsbDeviceList->IsDebugged == 0 && UsbDeviceList->Num > 0){
    j = UsbDeviceList->Num;
    while (j > 1) {  
      j = j - 1;
      DEBUG((EFI_D_INFO, "----"));
    }
    if(UsbDeviceList->Num == 1 && UsbDeviceList->NoBus == 0){
      DEBUG((EFI_D_INFO, "BDF[%02d:%02d:%02d]  ",UsbDeviceList->Bus,UsbDeviceList->Dev,UsbDeviceList->Func));
    }
    DEBUG((EFI_D_INFO, "Port:%d   ID:0x%04X:0x%04X  Speed:%04d M  ",UsbDeviceList->ParentPortNumber,UsbDeviceList->IdVendor,UsbDeviceList->IdProduct,UsbDeviceList->Speed));
    DEBUG((EFI_D_INFO, "%s[%a]\n",UsbDeviceList->UsbTypeStr,UsbDeviceList->Description));
    UsbDeviceList->IsDebugged = 1;
  } 
}
  

VOID
AdjustingUsbListSorting(
  IN UINTN            Index,
  IN UINTN            HandleCount
  )
{
  LIST_ENTRY                      *LastEntry;
  LIST_ENTRY                      *CurrentEntry;
  UINTN                           i = 0;
  UINTN                           j = 0;
  UINTN                           k = 0;
  UINTN                           count = 0;
  DEBUG_USB_LIST_INFO             *CurrentUsbDeviceList;
  DEBUG_USB_LIST_INFO             *LastUsbDeviceList;
  count = 0;
  for(i = 0; i < HandleCount; i++){
    count = 0;
    for( j= 0; j < HandleCount - i; j++){
      CurrentEntry = GetFirstNode (&mUsbDeviceList);
      for(k = 0; k < j; k++){
        CurrentEntry = CurrentEntry->ForwardLink;
      }
      CurrentUsbDeviceList = (DEBUG_USB_LIST_INFO *) CurrentEntry;
      if(CurrentUsbDeviceList->Num == Index){
        if(count == 0){
          count = 1;
          LastEntry = CurrentEntry;
          continue;
        }else{
           LastUsbDeviceList = (DEBUG_USB_LIST_INFO *) LastEntry;
          if(LastUsbDeviceList->ParentPortNumber > CurrentUsbDeviceList->ParentPortNumber){
            SwapListEntries(LastEntry,CurrentEntry);
          }
          LastEntry = CurrentEntry;
        }
      }
    }
  }


}

VOID
FindSamePortDevice (
  IN UINTN            Index,
  IN UINTN            HandleCount
  )
{
  UINTN                           j = 0;
  UINTN                           i = 0;
  UINTN                           k = 0;
  UINTN                           IsMatched = 0;
  UINTN                           Num1 = 0;
  UINTN                           Num2 = 0;
  EFI_DEVICE_PATH_PROTOCOL        *Node1;
  EFI_DEVICE_PATH_PROTOCOL        *Node2;
  LIST_ENTRY                      *LastEntry;
  LIST_ENTRY                      *CurrentEntry;
  DEBUG_USB_LIST_INFO             *CurrentUsbDeviceList;
  DEBUG_USB_LIST_INFO             *LastUsbDeviceList;

  LastEntry = GetFirstNode (&mUsbDeviceList);
  for(k = 0; k < Index; k++){
    LastEntry = LastEntry->ForwardLink;
  }
  LastUsbDeviceList = (DEBUG_USB_LIST_INFO *) LastEntry;

   if((LastUsbDeviceList->UsbType == 4)&&(Index < HandleCount - 1)){
     for(i = Index + 1; i < HandleCount; i++){
       CurrentEntry = GetFirstNode (&mUsbDeviceList);
       for(k = 0; k < i; k++){
         CurrentEntry = CurrentEntry->ForwardLink;
       }
       CurrentUsbDeviceList = (DEBUG_USB_LIST_INFO *) CurrentEntry;
       Node1 = (EFI_DEVICE_PATH_PROTOCOL *)LastUsbDeviceList->DevicePath;
       Node2 = (EFI_DEVICE_PATH_PROTOCOL *)CurrentUsbDeviceList->DevicePath;
       if((CurrentUsbDeviceList->Num == LastUsbDeviceList->Num + 1) || (CurrentUsbDeviceList->Num == LastUsbDeviceList->Num)){      
         IsMatched = 0;
         j = 0;
         Num1 = 0;
         Num2 = 0;
         for(j = 0; j < LastUsbDeviceList->Num; j++){
           Num1 = FindUsbPort(Node1);
           Num2 = FindUsbPort(Node2);
           if(Num1 != Num2){
             break;
           }
           Node1 = NextDevicePathNode (Node1);
           Node2 = NextDevicePathNode (Node2);
         }
         if(j == LastUsbDeviceList->Num){
           IsMatched = 1;
         }
         if(IsMatched == 1){
           if(CurrentUsbDeviceList->Num == LastUsbDeviceList->Num + 1){
             DebugUsbList(i);
             if((CurrentUsbDeviceList->UsbType == 4)&&(i < HandleCount - 1)){
               FindSamePortDevice(i,HandleCount);
             }
           }else if((CurrentUsbDeviceList->Num == LastUsbDeviceList->Num) && (CurrentUsbDeviceList->UsbType == 4)){
             CurrentUsbDeviceList->IsDebugged = 1;//Skip duplicate hub device
             continue;
           }
         }
       }
     }
   }





}


VOID
InsertUsbDevice (
  )
{

  DEBUG_USB_LIST_INFO             *UsbDeviceList;
  LIST_ENTRY                      *Entry;

  
  UsbDeviceList = AllocatePool (sizeof (DEBUG_USB_LIST_INFO));
  UsbDeviceList->Num                = mUsbListInfo.Num;
  UsbDeviceList->NoBus              = mUsbListInfo.NoBus;
  UsbDeviceList->Bus                = mUsbListInfo.Bus;
  UsbDeviceList->Dev                = mUsbListInfo.Dev;
  UsbDeviceList->Func               = mUsbListInfo.Func;
  UsbDeviceList->Speed              = mUsbListInfo.Speed;
  UsbDeviceList->UsbType            = mUsbListInfo.UsbType;
  UsbDeviceList->ParentPortNumber   = mUsbListInfo.ParentPortNumber;
  UsbDeviceList->DevicePath         = (EFI_DEVICE_PATH_PROTOCOL *)mUsbListInfo.DevicePath;
  UsbDeviceList->IsDebugged         = 0;
  UsbDeviceList->IdVendor           = mUsbListInfo.IdVendor;
  UsbDeviceList->IdProduct          = mUsbListInfo.IdProduct;
  UsbDeviceList->UsbTypeStr         = AllocateZeroPool(StrSize(mUsbListInfo.UsbTypeStr));
  UsbDeviceList->Description        = AllocateZeroPool(AsciiStrLen(mUsbListInfo.Description)+1);
  StrCpyS (UsbDeviceList->UsbTypeStr,(StrSize(mUsbListInfo.UsbTypeStr)/sizeof(CHAR16)),mUsbListInfo.UsbTypeStr);
  CopyMem(UsbDeviceList->Description,mUsbListInfo.Description,AsciiStrLen(mUsbListInfo.Description));

  if (IsListEmpty (&mUsbDeviceList)) {
    InsertTailList (&mUsbDeviceList, &UsbDeviceList->Link);
  } else {
    Entry = GetFirstNode (&mUsbDeviceList);
    while (!IsNull (&mUsbDeviceList, Entry)) {
      Entry = Entry->ForwardLink;
    }
    if (IsNull (&mUsbDeviceList, Entry)) {
      InsertTailList (Entry, &UsbDeviceList->Link);
    }
  }
}
  
VOID
EFIAPI
DebugUSBdeviceInformation (  
  )
{
#if !defined(MDEPKG_NDEBUG)
  EFI_STATUS                      Status;
  UINTN                           HandleCount;
  UINTN                           Count = 0;
  EFI_HANDLE                      *Handles;
  EFI_USB_IO_PROTOCOL             *UsbIo;
  UINTN                           j = 0;
  UINTN                           Seg = 0;
  UINTN                           Bus = 0;
  UINTN                           Dev = 0;
  UINTN                           Func = 0;
  EFI_HANDLE                      PciHandle;
  EFI_PCI_IO_PROTOCOL             *PciIo;
  UINTN                           Index;
  EFI_DEVICE_PATH_PROTOCOL        *Dp;
  EFI_USB_DEVICE_DESCRIPTOR       DevDesc;
  CHAR16                          *UsbTypeStr;
  UINT8                           UsbType;
  CHAR8                           *Description;
  EFI_USB_INTERFACE_DESCRIPTOR    Interface;
  EFI_DEVICE_PATH_PROTOCOL        *Node;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiUsbIoProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  Count = 0;
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiUsbIoProtocolGuid,
                    (VOID **) &UsbIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);

    Status = gBS->HandleProtocol (
                  Handles[Index],
                  &gEfiDevicePathProtocolGuid,
                  (VOID**)&Dp
                  );
    if (EFI_ERROR (Status)) {
      continue;
    }
    mUsbListInfo.NoBus = 0;
    Status = gBS->LocateDevicePath (
                    &gEfiPciIoProtocolGuid,
                    &Dp,
                    &PciHandle
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }
    Status = gBS->HandleProtocol(
  		                PciHandle,
  		                &gEfiPciIoProtocolGuid, 
                      (void**)&PciIo
  		                );
    if (EFI_ERROR(Status)) {
      continue;
    }

    Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
    if (EFI_ERROR(Status)) {
      mUsbListInfo.NoBus = 1;
    }

    mUsbListInfo.Bus = Bus;
    mUsbListInfo.Dev = Dev;
    mUsbListInfo.Func = Func;
    mUsbListInfo.IdVendor = DevDesc.IdVendor;
    mUsbListInfo.Num = 0;
    mUsbListInfo.IdProduct = DevDesc.IdProduct;
    Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &Interface);

    if(Interface.InterfaceClass == USB_MASS_STORE_CLASS){
      Status = GetUsbListNameByDiskInfo(Handles[Index], &Description);
    } else {
      Description = GetUsbListDescription(UsbIo);
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
      UsbType = 4;
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
      if (Description != NULL) {
        FreePool (Description);
      }
      continue;
    }

    UsbTypeStr = HiiGetString(mUsbListLibStringHandle, gUsbListInfo[UsbType], "en-US");
    mUsbListInfo.DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)Dp;
    Node = (EFI_DEVICE_PATH_PROTOCOL *)Dp;
    mUsbListInfo.UsbType = UsbType;
    mUsbListInfo.IsDebugged = 0;
    mUsbListInfo.Speed = 12;
    j = 0;
    
    while (!IsDevicePathEnd (Node)) {
      mUsbListInfo.ParentPortNumber = FindUsbPort(Node);
      mUsbListInfo.Num = mUsbListInfo.Num + 1; 
      j = j + 1;
      Node = NextDevicePathNode (Node);
      if(j >= 10000){
        break;
      }
    }

    if((DevDesc.MaxPacketSize0 == 9)&&(DevDesc.BcdUSB >= 0x0300)){
      mUsbListInfo.Speed = 5000;//USB3.0
    }else if((DevDesc.MaxPacketSize0 == 16) || (DevDesc.MaxPacketSize0 == 64)){
      mUsbListInfo.Speed = 480;//USB2.0
    }else if(DevDesc.MaxPacketSize0 == 8){
      mUsbListInfo.Speed = 12;//USB1.1
    }
    mUsbListInfo.UsbTypeStr = AllocateZeroPool(StrSize(UsbTypeStr));
    mUsbListInfo.Description = AllocateZeroPool(AsciiStrLen(Description)+1);
    StrCpyS (mUsbListInfo.UsbTypeStr,(StrSize(UsbTypeStr)/sizeof(CHAR16)),UsbTypeStr);
    CopyMem(mUsbListInfo.Description,Description,AsciiStrLen(Description));
    FreePool(UsbTypeStr);
    if (Description != NULL) {
      FreePool(Description);
    }

    InsertUsbDevice();
    Count = Count + 1;  

  }
  if(Count > 1){
    for (Index = 1;  Count >= Index; Index++) {
      AdjustingUsbListSorting(Index,Count);
    }
  }

  if(Count > 0){
    DEBUG((EFI_D_INFO, "usblist:\n"));
    for (Index = 0; Index < Count; Index++) {
      DebugUsbList(Index);
      FindSamePortDevice(Index,Count);
    }
  }else{
    DEBUG((EFI_D_INFO, "UsbListLib : No Usb Device:\n"));
  }
  
  if (HandleCount != 0) {
    FreePool(Handles);
  }
#endif

}

