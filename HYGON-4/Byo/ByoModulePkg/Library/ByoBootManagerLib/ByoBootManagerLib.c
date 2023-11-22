/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoBootManagerLib.c

Abstract:

Revision History:

**/
#include <Guid/FileInfo.h>
#include <IndustryStandard/Pci22.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ByoBootManagerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/ByoCommLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesLib.h>

#include <Protocol/FirmwareVolume2.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <Protocol/OpRomPnpName.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/UsbIo.h>
#include <Protocol/NetworkInterfaceIdentifier.h>
#include <Protocol/PlatHostInfoProtocol.h>

typedef struct {
  UINT16        DeviceId;
  UINT16        VendorId;
  CHAR16        *NetcardTypeString;
} PCI_NETCARD_TYPE_STRING;

PCI_NETCARD_TYPE_STRING mPciNetcardStringTable[] = {
  { 0x0103, 0x8088, L"WangXun Gigabit Server Adapter WX1860A4"},
  { 0xFFFF, 0xFFFF, L"Customized Netcard"}
};

/**
  Intel X710 network card function 0 device name has "-X" postfix,
  this function will remove it.
 **/
VOID
RemoveSomeNetCardsPostfix (
  CHAR16   *DevName
  )
{
  UINTN   StrLength;

  //
  // Check if current DevName is from Intel X710 netcard.
  //
  if ((StrStr(DevName, L"Intel") == NULL) || (StrStr(DevName, L"X710") == NULL)) {
    return;
  }

  StrLength = StrLen (DevName);
  if ((StrLength > 2) &&
      (DevName[StrLength - 1] >= L'0' ) &&
      (DevName[StrLength - 1] <= L'9') &&
      (DevName[StrLength - 2] == L'-')) {
    DevName[StrLength - 2] = L'\0';
  }

  return;
}

/**
  Get the NameString of netcard with SlotString and non-zero PCI Function number.
  The caller has responsibility to free it when the reval is not NULL.
  @param DevicePath                DevicePath of netcard, and it can't be NULL.
  @param NeedSlotString            If the NameString returned with SlotString.

  @return  The description string, and it's non-NULL.
 **/
CHAR16 *
EFIAPI
GetNameStringOfNetcard (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN BOOLEAN                   NeedSlotString
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  EFI_HANDLE                    PciHandle = NULL;
  EFI_HANDLE                    NiiHandle = NULL;
  CHAR16                        *String = NULL;
  CHAR16                        *TempString = NULL;
  EFI_PCI_IO_PROTOCOL           *PciIo = NULL;
  UINT16                        DeviceId;
  UINT16                        VendorId;
  UINTN                         Seg, Bus, Dev;
  UINTN                         Func = 0;
  CHAR16                        *FunctionString = NULL;
  EFI_DEVICE_PATH_PROTOCOL      *TempDevicePath = NULL;
  UINTN                         Size;
  EFI_HANDLE                    Handle;
  EFI_USB_IO_PROTOCOL           *UsbIo;
  CHAR16                        NullChar;
  CHAR16                        *Manufacturer;
  CHAR16                        *Product;
  EFI_USB_DEVICE_DESCRIPTOR     DevDesc;
  CONST UINT16                  UsbLangId = 0x0409; // English
  PLAT_HOST_INFO_PROTOCOL       *PlatHostInfo = NULL;
  PLATFORM_HOST_INFO            *HostInfo = NULL;
  CHAR8                         *SlotName;
  CHAR16                        *SlotString = NULL;
  EFI_DEVICE_PATH_PROTOCOL      *Node, *NextNode;
  CHAR16                        *NewString = NULL;

  ASSERT(DevicePath != NULL);

  TempDevicePath = DevicePath;
  gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TempDevicePath, &PciHandle);
  TempDevicePath = DevicePath;
  gBS->LocateDevicePath(&gEfiNetworkInterfaceIdentifierProtocolGuid_31, &TempDevicePath, &NiiHandle);

  if (PciHandle != NULL) {
    Status = gBS->HandleProtocol(PciHandle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
    if (!EFI_ERROR(Status)) {
      Status = PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
      if (Func != 0) {
        FunctionString = (CHAR16 *)AllocateZeroPool(StrSize(L"-x"));
        ASSERT(FunctionString != NULL);
        UnicodeSPrint(FunctionString, StrSize(L"-x"), L"-%d", Func&0xF);
      }

      PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0, 1, &VendorId);
      PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x2, 1, &DeviceId);
    }
  }

  //
  // Try to get the NameString of netcards by gEfiNetworkInterfaceIdentifierProtocolGuid_31 handle
  //
  if (NiiHandle != NULL) {
    Status = EfiGetControllerName(NiiHandle, &String);
    if (!EFI_ERROR(Status)) {
      goto APPEND_STRING;
    }
  }

  if (PciHandle != NULL) {
    //
    // Try to get the NameString of netcards by PCI handle.If can't get the NameString by EfiGetControllerName(),
    // use VendorId and DeviceId to match the fixed netcard NameString in mPciNetcardStringTable.
    //
    Status = EfiGetControllerName(PciHandle, &String);
    if (!EFI_ERROR (Status)) {
      goto APPEND_STRING;
    } else {
      for (Index = 0; Index < ARRAY_SIZE(mPciNetcardStringTable); Index++) {
        if ((VendorId == mPciNetcardStringTable[Index].VendorId)
          && (DeviceId == mPciNetcardStringTable[Index].DeviceId)) {
          break;
        }
      }
      if (Index < ARRAY_SIZE(mPciNetcardStringTable)) { // found
        String = (CHAR16 *)AllocateCopyPool(StrSize(mPciNetcardStringTable[Index].NetcardTypeString), mPciNetcardStringTable[Index].NetcardTypeString);
        ASSERT(String != NULL);
        goto APPEND_STRING;
      }
    }
  } else {
    //
    // Try to get the NameString of USB net options which don't support NII and PCI
    //
    TempDevicePath = DevicePath;
    Status = gBS->LocateDevicePath(&gEfiUsbIoProtocolGuid, &TempDevicePath, &Handle);
    if (EFI_ERROR(Status)) {
      goto APPEND_STRING;
    }

    Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiUsbIoProtocolGuid,
                  (VOID **) &UsbIo
                  );
    if (EFI_ERROR(Status)) {
      goto APPEND_STRING;
    }

    NullChar = L'\0';
    Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
    if (EFI_ERROR (Status)) {
      goto APPEND_STRING;
    }

    Status = UsbIo->UsbGetStringDescriptor (
                    UsbIo,
                    UsbLangId,
                    DevDesc.StrManufacturer,
                    &Manufacturer
                    );
    if (EFI_ERROR (Status)) {
      Manufacturer = &NullChar;
    }

    Status = UsbIo->UsbGetStringDescriptor (
                      UsbIo,
                      UsbLangId,
                      DevDesc.StrProduct,
                      &Product
                      );
    if (EFI_ERROR (Status)) {
      Product = &NullChar;
    }

    if ((Manufacturer == &NullChar) && (Product == &NullChar)) {
      goto APPEND_STRING;
    }

    Size = StrSize(Manufacturer) + StrSize(Product);
    String = (CHAR16 *)AllocateZeroPool(Size);
    ASSERT(String != NULL);
    StrCatS (String, Size/sizeof(CHAR16), Manufacturer);
    StrCatS (String, Size/sizeof(CHAR16), L" ");

    StrCatS (String, Size/sizeof(CHAR16), Product);
    if (Manufacturer != &NullChar) {
      FreePool (Manufacturer);
    }
    if (Product != &NullChar) {
      FreePool (Product);
    }
    DeleteExtraSpaces(String);
    return String;
  }

APPEND_STRING:
  //
  // Append L"-X"(X is PCI Function Number) to String except USB net options which don't support NII and PCI
  //
  if (String == NULL) {
    String = (CHAR16 *)AllocateCopyPool(StrSize(L"Netcard"), L"Netcard");
    ASSERT(String != NULL);
  }
  RemoveSomeNetCardsPostfix(String);
  if (FunctionString != NULL) {
    Size = StrLen(String) * sizeof(CHAR16) + StrSize(FunctionString);
    TempString = (CHAR16 *)AllocateZeroPool(Size);
    ASSERT(TempString != NULL);
    UnicodeSPrint(TempString, Size, L"%s%s", String, FunctionString);
    FreePool(String);
    String = TempString;
    FreePool(FunctionString);
  }

  //
  // Add PCIE slot string to name string of PCI netcards
  //
  if (NeedSlotString) {
    Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&PlatHostInfo);
    if (!EFI_ERROR(Status) && (PlatHostInfo != NULL) && (PlatHostInfo->HostList != NULL)) {
      //
      // Build DevicePath(Node) with Fuction 0 to match the SlotName provided by PlatformHostInfo
      //
      Node = (EFI_DEVICE_PATH_PROTOCOL *)AllocateCopyPool(GetDevicePathSize(DevicePath), DevicePath);
      TempDevicePath = Node;
      while (!IsDevicePathEndType(TempDevicePath)) {
        NextNode = NextDevicePathNode(TempDevicePath);
        if ((NextNode->Type == MESSAGING_DEVICE_PATH) && (NextNode->SubType == MSG_MAC_ADDR_DP)) {
          break;
        }
        TempDevicePath = NextDevicePathNode(TempDevicePath);
      }
      if (!IsDevicePathEndType(TempDevicePath)) {
        ((PCI_DEVICE_PATH *)TempDevicePath)->Function = 0;
      }

      for (Index = 0; Index < PlatHostInfo->HostCount; Index++) { // match HostInfo by DevicePath
        HostInfo = PlatHostInfo->HostList + Index;
        if (HostInfo->Dp != NULL) {
          if (CompareMem(HostInfo->Dp, Node, GetDevicePathSize(HostInfo->Dp) - END_DEVICE_PATH_LENGTH) == 0) {
            break;
          }
        }
      }
      if (Node != NULL) {
        FreePool(Node);
      }

      if ((Index < PlatHostInfo->HostCount) && (HostInfo->HostCtx != NULL)) { // found PCIE slot and SlotName exists
        SlotName = ((PLATFORM_HOST_INFO_PCIE_CTX *)HostInfo->HostCtx)->SlotName;
        if (SlotName != NULL) {
          Size = (AsciiStrLen(SlotName) + 2) * sizeof(CHAR16);
          SlotString = (CHAR16 *)AllocateZeroPool(Size);
          ASSERT(SlotString != NULL);
          AsciiStrToUnicodeStrS (SlotName, SlotString, Size/sizeof(CHAR16));

          Size = StrSize(SlotString) + StrSize(String) + sizeof(CHAR16);
          NewString = (CHAR16 *)AllocatePool(Size);
          ASSERT(NewString != NULL);
          UnicodeSPrint (
            NewString,
            Size,
            L"%s: %s",
            SlotString,
            String
            );
          FreePool(String);
          FreePool(SlotString);
          String = NewString;
        }
      }
    }
  }

  return String;
}

/**
  Append non-zero IfNum to the end of the current net NameString in the form of L"-x".
  If the PCI Function Number is zero, must append it in the same form before appending IfNum.
 **/
VOID
EFIAPI
AppendIfNumToNameString (
  IN OUT CHAR16              **String,
  IN UINTN                   FunctionNumber,
  IN UINT16                  IfNum
  )
{
  CHAR16                        *NewString = NULL;
  UINTN                         Size;

  if ((String == NULL) || (*String == NULL) || (StrLen(*String) == 0) || (IfNum == 0)) {
    return;
  }

  Size = StrSize(*String) + sizeof(CHAR16) * (StrLen(L"-x") + StrLen(L"-xx"));
  NewString = (CHAR16 *)AllocateZeroPool(Size);
  ASSERT(NewString != NULL);

  if (FunctionNumber == 0) {
    UnicodeSPrint(NewString, Size, L"%s-%d-%d", *String, FunctionNumber&0xF, IfNum);
  } else {
    UnicodeSPrint(NewString, Size, L"%s-%d", *String, IfNum);
  }
  FreePool(*String);
  *String = NewString;
}

/**
  Get vendor string and type string of PCIE device.
 **/
CHAR16 *
EFIAPI
GetPciVendorString (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN UINT32                Attribute
  )
{
  EFI_STATUS                    Status;
  CHAR16                        *String = NULL;
  UINTN                         Index;
  UINTN                         HandleCount;
  EFI_HANDLE                    *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  EFI_HANDLE                    PciHandle;
  EFI_PCI_IO_PROTOCOL           *TempPciIo;
  UINTN                         Seg, Bus, Dev, Func;
  EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL  *Nii = NULL;
  UINT16                                     IfNum = 0;

  if ((PciIo == NULL)) {
    return NULL;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    return NULL;
  }
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID**)&DevicePath
                      );            
    if (EFI_ERROR(Status)) {
      continue;
    }

    Nii = NULL;
    IfNum = 0;
    gBS->HandleProtocol(HandleBuffer[Index], &gEfiNetworkInterfaceIdentifierProtocolGuid_31, (VOID**)&Nii);
    if (Nii != NULL) {
      IfNum = Nii->IfNum;
    }

    Status = gBS->LocateDevicePath (
                    &gEfiPciIoProtocolGuid,
                    &DevicePath,
                    &PciHandle
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }
    
    Status = gBS->HandleProtocol (
                    PciHandle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &TempPciIo
                    );                   
    if (EFI_ERROR(Status)) {                          
      continue;
    }
    if (TempPciIo == PciIo) {
      break;
    }
  }

  if (Index < HandleCount) { // found
    DevicePath = DevicePathFromHandle(HandleBuffer[Index]);
    if (DevicePath != NULL) {
      String = GetNameStringOfNetcard(DevicePath, FALSE);
    } else {
      String = (CHAR16 *)AllocateCopyPool(StrSize(L"Netcard"), L"Netcard");
      ASSERT(String != NULL);
    }
    if (IfNum != 0) {
      PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
      AppendIfNumToNameString(&String, Func, IfNum);
    }
  }
  
  return String;
} 

BM_MENU_TYPE
EFIAPI
GetEfiBootGroupType(
  EFI_DEVICE_PATH_PROTOCOL *FilePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *Dp;
  EFI_HANDLE                    DevHandle;
  EFI_STATUS                    Status;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT8                         ClassCode[3];
  BOOLEAN                       IsODD;
  BOOLEAN                       IsUSB = FALSE;
  BOOLEAN                       IsBlock = FALSE;
  EFI_BLOCK_IO_PROTOCOL         *BlockIo;
  EFI_DEVICE_PATH_PROTOCOL      *Next;
  UINTN                         TempSize;
  BM_MENU_TYPE                  GroupType = BM_MENU_TYPE_MAX;

  IsODD = FALSE;
  Dp = FilePath;

  if (DevicePathType(Dp) == MEDIA_DEVICE_PATH && DevicePathSubType(Dp) == MEDIA_HARDDRIVE_DP) {
    return BM_MENU_TYPE_UEFI_HDD;
  }

  Next = GetNextDevicePathInstance (&Dp, &TempSize);
  while (!IsDevicePathEndType (Next)) {
    //
    // GMAC or PCIE LAN
    //
    if ((Next->Type == MESSAGING_DEVICE_PATH) && (Next->SubType == MSG_MAC_ADDR_DP)) {
      return BM_MENU_TYPE_UEFI_PXE;
    }

    //
    // Classify SD and MMC devices to Harddisk
    //
    if ((Next->Type == MESSAGING_DEVICE_PATH) && ((Next->SubType == MSG_SD_DP) || (Next->SubType == MSG_EMMC_DP))) {
      return BM_MENU_TYPE_UEFI_HDD;
    }
    Next = NextDevicePathNode (Next);
  }
  
  Dp = FilePath;  
  Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &Dp, &DevHandle);
  if(!EFI_ERROR(Status)) {
    IsBlock = TRUE;
    gBS->HandleProtocol(DevHandle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
    if(BlockIo->Media->BlockSize == 2048) {
      IsODD = TRUE;
    }
  }

  Dp = FilePath;  
  Status = gBS->LocateDevicePath(&gEfiUsbIoProtocolGuid, &Dp, &DevHandle);
  if (!EFI_ERROR (Status)) {
    IsUSB = TRUE;
  }

  Dp = FilePath;
  Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DevHandle);
  if(EFI_ERROR(Status)){
    if (IsBlock){
      if (IsUSB) {
        if (IsODD) {
          return BM_MENU_TYPE_UEFI_USB_ODD;
        } else {
          return BM_MENU_TYPE_UEFI_USB_DISK;
        }
      }
      if (IsODD) {
        return BM_MENU_TYPE_UEFI_ODD;
      } else {
        return BM_MENU_TYPE_UEFI_HDD;
      }
    } else {
      return BM_MENU_TYPE_UEFI_OTHERS;
    }
  }
  gBS->HandleProtocol(DevHandle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 9, 3, ClassCode);

  switch (ClassCode[2]) {
    
    case 0x01:
      if(IsODD){
        GroupType = BM_MENU_TYPE_UEFI_ODD;
      } else {
        GroupType = BM_MENU_TYPE_UEFI_HDD;
      }
      break;
      
    case 0x0C:
      if(ClassCode[1] == 3){
        if(IsODD){
          GroupType = BM_MENU_TYPE_UEFI_USB_ODD;
        } else {
          GroupType = BM_MENU_TYPE_UEFI_USB_DISK;
        }
      } else if(ClassCode[1] == 4){
        GroupType = BM_MENU_TYPE_UEFI_HDD;
      } else {
        GroupType = BM_MENU_TYPE_UEFI_OTHERS;
      }
      break;

    case 0x02:
      GroupType = BM_MENU_TYPE_UEFI_PXE;
      break;
  }

  return GroupType;
}

UINT8
EFIAPI
GetEfiNetWorkType (
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath,
  OUT CHAR16                     **TypeStr
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *Node;
  EFI_DEVICE_PATH_PROTOCOL      *NextNode;
  UINT8                         Type;
  UINT8                         IpType;

  ASSERT (DevicePath != NULL);

  IpType = 0xFF;
  
  for (Node = DevicePath; !IsDevicePathEndType(Node); Node = NextDevicePathNode(Node)) {
    if(DevicePathType(Node) != MESSAGING_DEVICE_PATH){
      continue;
    }  

    Type = DevicePathSubType(Node);
    if (Type == MSG_IPv4_DP) {
      IpType = MSG_IPv4_DP;
    } else if (Type == MSG_IPv6_DP) {
      IpType = MSG_IPv6_DP;
    }
    
    NextNode = NextDevicePathNode(Node);    
    if (!IsDevicePathEndType(NextNode)) {
      continue;
    }

    Type = DevicePathSubType(Node);
    switch(Type){
     case MSG_IPv4_DP:
     case MSG_IPv6_DP:
       if (TypeStr != NULL) {
         *TypeStr = L"PXE";
       }
       return IpType;

     case MSG_URI_DP:
       if (TypeStr != NULL) {
         *TypeStr = L"HTTP";
       }
       return IpType;          
    }
  }

  if (TypeStr != NULL) {
    *TypeStr = L"";
  }
  return 0xFF;
}

/**
  For a bootable Device path, return its boot type.

  @param  DevicePath                   The bootable device Path to check

  @retval AcpiFloppyBoot               If given device path contains ACPI_DEVICE_PATH type device path node
                                       which HID is floppy device.
  @retval MessageAtapiBoot             If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_ATAPI_DP.
  @retval MessageSataBoot              If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_SATA_DP.
  @retval MessageScsiBoot              If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_SCSI_DP.
  @retval MessageUsbBoot               If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_USB_DP.
  @retval MessageNetworkBoot           If given device path contains MESSAGING_DEVICE_PATH type device path node
                                       and its last device path node's subtype is MSG_MAC_ADDR_DP, MSG_VLAN_DP,
                                       MSG_IPv4_DP or MSG_IPv6_DP.
  @retval UnsupportedBoot              If given device path doesn't match the above condition, it's not supported.

**/
BOOT_TYPE
EFIAPI
BootTypeFromDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *Node;
  EFI_DEVICE_PATH_PROTOCOL      *LastDeviceNode;

  ASSERT (DevicePath != NULL);

  for (Node = DevicePath; !IsDevicePathEndType (Node); Node = NextDevicePathNode (Node)) {
    switch (DevicePathType (Node)) {

      case ACPI_DEVICE_PATH:
        if (EISA_ID_TO_NUM (((ACPI_HID_DEVICE_PATH *) Node)->HID) == 0x0604) {
          return AcpiFloppyBoot;
        }
        break;

      case MESSAGING_DEVICE_PATH:
        //
        // Skip LUN device node
        //
        LastDeviceNode=Node;
        do {
          LastDeviceNode = NextDevicePathNode (LastDeviceNode);
        } while (
            (DevicePathType (LastDeviceNode) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType(LastDeviceNode) == MSG_DEVICE_LOGICAL_UNIT_DP)
            );

        //
        // if the device path not only point to driver device, it is not a messaging device path,
        //
        if (!IsDevicePathEndType (LastDeviceNode)) {
          continue;
        }

        switch (DevicePathSubType (Node)) {
        case MSG_ATAPI_DP:
          return MessageAtapiBoot;
          break;

        case MSG_SATA_DP:
          return MessageSataBoot;
          break;

        case MSG_USB_DP:
          return MessageUsbBoot;
          break;

        case MSG_SCSI_DP:
          return MessageScsiBoot;
          break;

        case MSG_MAC_ADDR_DP:
        case MSG_VLAN_DP:
        case MSG_IPv4_DP:
        case MSG_IPv6_DP:
          return MessageNetworkBoot;
          break;
        }
    }
  }

  return UnsupportedBoot;
}

CHAR16 *
EFIAPI
GetDescFromPci (
  EFI_HANDLE Handle,
  EFI_DEVICE_PATH_PROTOCOL *ptDevPath
  )
{
  EFI_PCI_IO_PROTOCOL            *ptPciIo;
  EFI_HANDLE                     PciHandle;
  UINT8                          ClassCode[3];
  EFI_DEVICE_PATH_PROTOCOL       *ptDP;
  CHAR16                         *DevName;
  EFI_STATUS                     Status;
  CHAR16                         *Name = NULL;  
  OPROM_PNP_DEV_NAME             *PnpName;
  UINTN                          MyStrSize;


  ptDP = ptDevPath;
  Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &ptDP,
                  &PciHandle
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
      
  Status = gBS->HandleProtocol(
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&ptPciIo
                  );
  ASSERT(!EFI_ERROR(Status));
      
  Status = ptPciIo->Pci.Read(
                          ptPciIo, 
                          EfiPciIoWidthUint8,  
                          PCI_CLASSCODE_OFFSET, 
                          3, 
                          ClassCode
                          );
  if(ClassCode[2] == 8 && 
     ClassCode[1] == 5 && 
    (ClassCode[0] == 0 || ClassCode[0] == 1)){      // SD
    Name = L"SD";
    Name = AllocateCopyPool(StrSize(Name), Name);
  } else if(ClassCode[2] == 1 && ClassCode[1] == 8 && ClassCode[0] == 2){ // NVMe
    DevName = LibOpalDriverGetDriverDeviceName(gBS, Handle);
    if(DevName == NULL){
      Name = L"NVME";
      Name = AllocateCopyPool(StrSize(Name), Name);
    } else {
      MyStrSize = StrSize(DevName) + 32;
      Name = AllocateZeroPool(MyStrSize);
      UnicodeSPrint(Name, MyStrSize, L"NVME: %s", DevName);
      FreePool(DevName);
    }
  } else {
    Status = gBS->HandleProtocol(
                    Handle,
                    &gOpromPnpNameProtocolGuid,
                    (VOID **)&PnpName
                    );
    if(!EFI_ERROR(Status)){
      Name = AllocateCopyPool(StrSize(PnpName->Name), PnpName->Name);
    } else {
      Name = NULL;
    }
  }

ProcExit:
  return Name;
}

EFI_STATUS
EFIAPI
AtaReadIdentifyData (
  IN  EFI_ATA_PASS_THRU_PROTOCOL    *ptAtaPassThru,
  IN  UINT16                        Port,
  IN  UINT16                        PortMP,
  OUT ATA_IDENTIFY_DATA             *IdentifyData
  )
{
  EFI_STATUS                        Status;
  EFI_ATA_COMMAND_BLOCK             Acb;
  EFI_ATA_STATUS_BLOCK              Asb;
  EFI_ATA_PASS_THRU_COMMAND_PACKET  Packet;

  if((ptAtaPassThru == NULL) || (IdentifyData == NULL)){
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem(&Acb, sizeof(Acb));
  ZeroMem(&Asb, sizeof(Asb));
  Acb.AtaCommand    = ATA_CMD_IDENTIFY_DRIVE;
  Acb.AtaDeviceHead = (UINT8) (BIT7 | BIT6 | BIT5 | (PortMP == 0xFFFF ? 0 : (PortMP << 4)));

  ZeroMem(&Packet, sizeof(Packet));
  Packet.Protocol         = EFI_ATA_PASS_THRU_PROTOCOL_PIO_DATA_IN;
  Packet.Length           = EFI_ATA_PASS_THRU_LENGTH_BYTES;
  Packet.Asb              = &Asb;
  Packet.Acb              = &Acb;
  Packet.InDataBuffer     = IdentifyData;
  Packet.InTransferLength = sizeof(ATA_IDENTIFY_DATA);
  Packet.Timeout          = EFI_TIMER_PERIOD_SECONDS (3);

  Status = ptAtaPassThru->PassThru (
                            ptAtaPassThru,
                            Port,
                            PortMP,
                            &Packet,
                            NULL
                            );
  
  return Status;
}

/**
  Initializes a BootOption instance.

  @param FileGuid          Pointer to the File Guid.
  @param Description       The description of the boot option.
  @param BootOption        Pointer to the BootOption.
  @param IsBootCategory    It is BootOption? TURE or FALSE.
  @param OptionalData      Optional data of the load option, else NULL.
  @param OptionalDataSize  Size of the optional data of the load option, else 0.

  @retval EFI_SUCCESS           The load option was initialized successfully.
  @retval other                 The load option was initialized unsuccessfully.
**/
EFI_STATUS
EFIAPI
CreateFvFileBootOption (
  IN  CONST EFI_GUID                    *FileGuid,
  IN CHAR16                             *Description,
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION   *BootOption,
  IN BOOLEAN                            IsBootCategory,
  IN UINT8                              *OptionalData,    OPTIONAL
  IN UINT32                             OptionalDataSize
  )
{
  EFI_STATUS                         Status;
  EFI_DEVICE_PATH_PROTOCOL           *DevicePath = NULL;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH  FileNode;
  UINT32                             Attributes;

  if (FileGuid == NULL || Description ==NULL || BootOption == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  if (OptionalData == NULL && OptionalDataSize != 0) {
    return EFI_INVALID_PARAMETER;
  }
  EfiInitializeFwVolDevicepathNode (&FileNode, FileGuid);
  Status = GetFileDevicePathFromAnyFv (FileGuid,EFI_SECTION_PE32,0,&DevicePath);
  if (EFI_ERROR (Status)) {
    return EFI_INVALID_PARAMETER;
  }
  Attributes = LOAD_OPTION_ACTIVE;
  if(IsBootCategory){
    Attributes |= LOAD_OPTION_CATEGORY_BOOT;
  } else {
    Attributes |= LOAD_OPTION_CATEGORY_APP;
    Attributes |= LOAD_OPTION_HIDDEN;
  }
  Status = EfiBootManagerInitializeLoadOption (
             BootOption,
             LoadOptionNumberUnassigned,
             LoadOptionTypeBoot,
             Attributes,
             Description,
             DevicePath,
             OptionalData,
             OptionalDataSize
             );
  if (DevicePath != NULL) {
    FreePool (DevicePath);
  }

  return Status;
}

/**
  Registers Firmware volume with passed options.

  @param FileGuid          Pointer to the File Guid.
  @param Description       The description of the boot option.
  @param IsBootCategory    It is BootOption? TURE or FALSE.
  @param OptionalData      Optional data of the load option, else NULL.
  @param OptionalDataSize  Size of the optional data of the load option, else 0.
  @param OptionNumber      Entry in BootXXXX if replacing existing, else LoadOptionNumberUnassigned.

  @retval (INTN) NewOption.OptionNumber   The NewBootOption number.
**/
INTN
EFIAPI
RegisterFvFileBootOptionEx (
  IN  CONST EFI_GUID               *FileGuid,
  IN  CHAR16                       *Description,
  IN  BOOLEAN                      IsBootCategory,
  IN  UINT8                        *OptionalData,   OPTIONAL
  IN  UINT32                       OptionalDataSize,
  IN  UINTN                        OptionNumber
  )
{
  EFI_STATUS                        Status;
  INTN                              OptionIndex;
  EFI_BOOT_MANAGER_LOAD_OPTION      NewOption;
  EFI_BOOT_MANAGER_LOAD_OPTION      *BootOptions;
  UINTN                             BootOptionCount;

  if(CompareGuid (FileGuid,PcdGetPtr (PcdBootManagerMenuFile))) {
    Status = EfiBootManagerGetBootManagerMenu (&NewOption);
    if(Status == EFI_SUCCESS) {
      return (INTN)NewOption.OptionNumber;
    }
  }
  Status = CreateFvFileBootOption (FileGuid,Description,&NewOption,IsBootCategory,OptionalData,OptionalDataSize);
  ASSERT_EFI_ERROR (Status);

  BootOptions = EfiBootManagerGetLoadOptions (
                  &BootOptionCount, LoadOptionTypeBoot
                  );

  OptionIndex = EfiBootManagerFindLoadOption (
                  &NewOption, BootOptions, BootOptionCount
                  );

  if (OptionIndex == -1) {
    if (OptionNumber != LoadOptionNumberUnassigned) {
      NewOption.OptionNumber = OptionNumber;
    }
    Status = EfiBootManagerAddLoadOptionVariable (&NewOption, MAX_UINTN);
    ASSERT_EFI_ERROR (Status);
    OptionIndex = (INTN)NewOption.OptionNumber;
  } else {
    OptionIndex = (INTN)BootOptions[OptionIndex].OptionNumber;
  }
  
  EfiBootManagerFreeLoadOption (&NewOption);
  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
  
  return OptionIndex;
}


typedef struct {
  CHAR16   *FilePath;
  CHAR16   *Param;
} BYO_LIB_BOOT_LOADER_INFO;

BYO_LIB_BOOT_LOADER_INFO gByoLibBootLoaderInfo[] = {
  {L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi",    L"/RecoveryBCD"},
};

EFI_STATUS ByoLibFindOsRecoveryLoader(EFI_DEVICE_PATH_PROTOCOL **pFileDp, CHAR16 **LoaderParam)
{
  UINTN                                 HandleCount;
  EFI_HANDLE                            *Handles = NULL;
  EFI_STATUS                            Status;      
  UINTN                                 Index;
  EFI_DEVICE_PATH_PROTOCOL              *Dp = NULL;
  UINT8                                 Type;
  EFI_BLOCK_IO_PROTOCOL                 *BlockIo;
  UINT8                                 FileIndex;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL       *Fs;
  EFI_FILE_PROTOCOL                     *RootFile;
  CHAR16                                *FilePath = NULL;
  BOOLEAN                               HasFound = FALSE;
  FILEPATH_DEVICE_PATH                  *FilePathDp;
  CHAR16                                *Param = NULL;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiSimpleFileSystemProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if(EFI_ERROR(Status) || HandleCount == 0){
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&Dp
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID**)&BlockIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }
    
    if(BlockIo->Media->BlockSize != 512){
      continue;
    }

    Type = GetDeviceTypeFromDp(Dp);
    if(Type == MSG_USB_DP){
      continue;
    }

    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID**)&Fs
                    ); 
    Status = Fs->OpenVolume(Fs, &RootFile);
  	if(EFI_ERROR(Status)){
      continue;
    }      

    for(FileIndex=0;FileIndex<ARRAY_SIZE(gByoLibBootLoaderInfo);FileIndex++){
      HasFound = IsFilePresent(gBS, RootFile, gByoLibBootLoaderInfo[FileIndex].FilePath);
      if(HasFound){
        FilePath = gByoLibBootLoaderInfo[FileIndex].FilePath;
        Param    = gByoLibBootLoaderInfo[FileIndex].Param;
        break;
      }
    }

    RootFile->Close(RootFile);
    if(HasFound){
      break;
    }
  }

  if (Handles != NULL) {
    FreePool(Handles);
  }
  
  if(HasFound){
    FilePathDp = (FILEPATH_DEVICE_PATH *)CreateDeviceNode (
                    MEDIA_DEVICE_PATH,
                    MEDIA_FILEPATH_DP,
                    (UINT16)(sizeof(FILEPATH_DEVICE_PATH) - 2 + StrSize(FilePath))
                    );
    StrCpyS(FilePathDp->PathName, StrSize(FilePath), FilePath);
    *pFileDp = AppendDevicePathNode(Dp, (EFI_DEVICE_PATH_PROTOCOL*)FilePathDp);
    *LoaderParam = Param;
    ShowDevicePathDxe(gBS, *pFileDp);
    FreePool(FilePathDp);
    return EFI_SUCCESS;
  } else {
    return EFI_NOT_FOUND;
  }
}

/**
  Registers Windows Recovery Boot Option

  @param Description       The description of the boot option.

  @retval (INTN) NewOption.OptionNumber   The NewBootOption number.

**/
INTN
EFIAPI
RegisterWindowsRecoveryBootOption (
  IN  CHAR16                       *Description
  )
{
  EFI_STATUS                        Status;
  INTN                              OptionIndex;
  EFI_BOOT_MANAGER_LOAD_OPTION      NewOption;
  EFI_BOOT_MANAGER_LOAD_OPTION      *BootOptions;
  UINTN                             BootOptionCount;
  UINT32                            Attributes;
  EFI_DEVICE_PATH_PROTOCOL          *FilePathDp;
  CHAR16                            *OptionalData;
  UINTN                             OptionalDataSize;

  Attributes = LOAD_OPTION_CATEGORY_BOOT | LOAD_OPTION_HIDDEN;
  OptionIndex      = -1;
  FilePathDp       = NULL;
  OptionalData     = NULL;
  OptionalDataSize = 0;

  Status = ByoLibFindOsRecoveryLoader(&FilePathDp, &OptionalData);
  if (EFI_ERROR (Status)) {
    return OptionIndex;
  }
  if (OptionalData != NULL) {
    OptionalDataSize = StrSize (OptionalData);
  }

  Status = EfiBootManagerInitializeLoadOption (
             &NewOption,
             LoadOptionNumberUnassigned,
             LoadOptionTypeBoot,
             Attributes,
             Description,
             FilePathDp,
             (UINT8 *) OptionalData,
             (UINT32) OptionalDataSize
             );
  ASSERT_EFI_ERROR (Status);

  BootOptions = EfiBootManagerGetLoadOptions (
                  &BootOptionCount, LoadOptionTypeBoot
                  );

  OptionIndex = EfiBootManagerFindLoadOption (
                  &NewOption, BootOptions, BootOptionCount
                  );

  if (OptionIndex == -1) {
    Status = EfiBootManagerAddLoadOptionVariable (&NewOption, MAX_UINTN);
    ASSERT_EFI_ERROR (Status);
    OptionIndex = (INTN)NewOption.OptionNumber;
  } else {
    OptionIndex = (INTN)BootOptions[OptionIndex].OptionNumber;
  }

  EfiBootManagerFreeLoadOption (&NewOption);
  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
  
  return OptionIndex;
}

/**
  Registers Firmware volume with passed options.

  @param FileGuid          Pointer to the File Guid.
  @param Description       The description of the boot option.
  @param IsBootCategory    It is BootOption? TURE or FALSE.
  @param OptionalData      Optional data of the load option, else NULL.
  @param OptionalDataSize  Size of the optional data of the load option, else 0.

  @retval (INTN) NewOption.OptionNumber   The NewBootOption number.

**/
INTN
EFIAPI
RegisterFvFileBootOption (
  IN  CONST EFI_GUID               *FileGuid,
  IN  CHAR16                       *Description,
  IN  BOOLEAN                      IsBootCategory,
  IN  UINT8                        *OptionalData,   OPTIONAL
  IN  UINT32                       OptionalDataSize
  )
{
  return RegisterFvFileBootOptionEx(FileGuid,Description,IsBootCategory,OptionalData,OptionalDataSize,LoadOptionNumberUnassigned);
}

/**
  Read the EFI variable (VendorGuid/Name) and return a dynamically allocated
  buffer, and the size of the buffer. If failure return NULL.

  @param  Name                  String part of EFI variable name
  @param  VendorGuid            GUID part of EFI variable name
  @param  VariableSize          Returns the size of the EFI variable that was read

  @return                       Dynamically allocated memory that contains a copy of the EFI variable
                                Caller is responsible freeing the buffer.
  @retval NULL                  Variable was not read

**/
VOID *
EFIAPI
BootManagerGetVariableAndSize (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  )
{
  EFI_STATUS  Status;
  UINTN       BufferSize;
  VOID        *Buffer;
  UINTN       Check;

  Buffer     = NULL;
  BufferSize = 0;

  //
  // Pass in a zero size buffer to find the required buffer size.
  //
  Status = gRT->GetVariable(Name, VendorGuid, NULL, &BufferSize, Buffer);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // Allocate the buffer to return
    //
    Buffer = AllocatePool (BufferSize);
    ASSERT (Buffer != NULL);
    //
    // Read variable into the allocated buffer.
    //
    Status = gRT->GetVariable (Name, VendorGuid, NULL, &BufferSize, Buffer);
    if (EFI_ERROR (Status)) {
      FreePool (Buffer);
      BufferSize = 0;
      Buffer     = NULL;
    }
  }

  Check = ((Buffer == NULL && BufferSize == 0) || (Buffer != NULL && BufferSize != 0));
  if(!Check){
    DEBUG((EFI_D_ERROR, "[ERROR] %r %lX %lX %s %g\n", Status, (UINT64)(UINTN)Buffer, (UINT64)BufferSize, Name, VendorGuid));
  }
  
  ASSERT (((Buffer == NULL) && (BufferSize == 0)) ||
          ((Buffer != NULL) && (BufferSize != 0))
          );
  *VariableSize = BufferSize;

  return Buffer;
}

/**
  Build the BootTemp#### or DriverTemp#### option from the VariableName.

  @param  VariableName          EFI Variable name indicate if it is Boot#### or
                                Driver####

  @retval EFI_SUCCESS     Get the option just been created
  @retval EFI_NOT_FOUND   Failed to get the new option

**/
EFI_STATUS
EFIAPI
TempBootManagerVariableToLoadOption (
  IN  CHAR16                          *VariableName,
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION *Option  
  )
{
  EFI_STATUS                         Status;
  UINT32                             Attribute;
  UINT16                             FilePathSize;
  UINT8                              *Variable;
  UINT8                              *TempPtr;
  UINTN                              VariableSize;
  EFI_DEVICE_PATH_PROTOCOL           *FilePath;
  UINT8                              *OptionalData;
  UINT32                             OptionalDataSize;
  CHAR16                             *Description;
  UINT8                              NumOff;
  EFI_BOOT_MANAGER_LOAD_OPTION_TYPE  OptionType;
  UINT16                             OptionNumber;

  if ((VariableName == NULL) || (Option == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Read the variable
  //
  Variable = BootManagerGetVariableAndSize (
              VariableName,
              &gByoGlobalVariableGuid,
              &VariableSize
              );
  if (Variable == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  // Notes: careful defined the variable of Boot#### or
  // Driver####, consider use some macro to abstract the code
  //
  //
  // Get the option attribute
  //
  TempPtr   =  Variable;
  Attribute =  *(UINT32 *) Variable;
  TempPtr   += sizeof (UINT32);

  //
  // Get the option's device path size
  //
  FilePathSize =  *(UINT16 *) TempPtr;
  TempPtr     += sizeof (UINT16);

  //
  // Get the option's description string
  //
  Description  = (CHAR16 *) TempPtr;

  //
  // Get the option's description string size
  //
  TempPtr     += StrSize ((CHAR16 *) TempPtr);

  //
  // Get the option's device path
  //
  FilePath     =  (EFI_DEVICE_PATH_PROTOCOL *) TempPtr;
  TempPtr     += FilePathSize;

  OptionalDataSize = (UINT32) (VariableSize - (UINTN) (TempPtr - Variable));
  if (OptionalDataSize == 0) {
    OptionalData = NULL;
  } else {
    OptionalData = TempPtr;
  }

  if (*VariableName == L'B') {
    OptionType = LoadOptionTypeBoot;
    NumOff = (UINT8) (sizeof (L"BootTemp") / sizeof (CHAR16) - 1);
  } else {
    OptionType = LoadOptionTypeDriver;
    NumOff = (UINT8) (sizeof (L"DriverTemp") / sizeof (CHAR16) - 1);
  }
  
  //
  // Get the value from VariableName Unicode string
  // since the ISO standard assumes ASCII equivalent abbreviations, we can be safe in converting this
  // Unicode stream to ASCII without any loss in meaning.
  //
  OptionNumber = (UINT16) StrHexToUintn (VariableName + NumOff);

  Status = EfiBootManagerInitializeLoadOption (
             Option,
             OptionNumber,
             OptionType,
             Attribute,
             Description,
             FilePath,
             OptionalData,
             OptionalDataSize
             );
  ASSERT_EFI_ERROR (Status);
  
  FreePool (Variable);
  return Status;
}

/**
  Copy load option

  @param  Dst       Target option of copy
  @param  Src       Place to copy from

  @retval None
**/
VOID
EFIAPI
ByoCopyBootOption(
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION   *Dst,
  IN     EFI_BOOT_MANAGER_LOAD_OPTION   *Src
  )
{
  UINTN                         Size;

  ASSERT(Dst != NULL);
  ASSERT(Src != NULL);
  
  CopyMem(Dst, Src, sizeof(EFI_BOOT_MANAGER_LOAD_OPTION));

  Size = StrSize(Src->Description);
  Dst->Description = AllocatePool(Size);
  ASSERT(Dst->Description != NULL);
  StrCpyS(Dst->Description,Size/sizeof (CHAR16), Src->Description);

  Size = GetDevicePathSize(Src->FilePath);
  Dst->FilePath = AllocatePool(Size);
  ASSERT(Dst->FilePath != NULL); 
  CopyMem(Dst->FilePath, Src->FilePath, Size);

  if(Src->OptionalDataSize != 0){
    Dst->OptionalData = AllocatePool(Src->OptionalDataSize);
    ASSERT(Dst->OptionalData != NULL);
    CopyMem(Dst->OptionalData, Src->OptionalData, Src->OptionalDataSize);
  }
}

/**
  Unload driver Image

  @param  FileGuid              The file Guid

  @retval EFI_NOT_FOUND         Can not find the file guid
  @retval other                 UnloadImage status
**/
EFI_STATUS 
EFIAPI
UnloadDriverWithFileGuid(
  EFI_GUID      *FileGuid
  )
{
  UINTN                              DriverImageHandleCount;
  EFI_HANDLE                         *DriverImageHandleBuffer = NULL;
  EFI_LOADED_IMAGE_PROTOCOL          *LoadedImage;
  EFI_STATUS                         Status;
  UINTN                              Index;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH  *FvFileDp;
  
  DEBUG((EFI_D_INFO, "%a Enter\n", __FUNCTION__));
  DriverImageHandleCount = 0;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadedImageProtocolGuid,
                  NULL,
                  &DriverImageHandleCount,
                  &DriverImageHandleBuffer
                  );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "Locate Loaded Image Protocol Failed\n"));
    goto ProcExit;
  }

  for (Index = 0; Index < DriverImageHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    DriverImageHandleBuffer[Index],
                    &gEfiLoadedImageProtocolGuid,
                    (VOID**)&LoadedImage
                    );
    if (LoadedImage->FilePath == NULL) {
      continue;
    }

    if (DevicePathType(LoadedImage->FilePath) == MEDIA_DEVICE_PATH && 
        DevicePathSubType(LoadedImage->FilePath) == MEDIA_PIWG_FW_FILE_DP) {
          FvFileDp = (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)LoadedImage->FilePath;
          if (CompareGuid(&FvFileDp->FvFileName, FileGuid)) {
            DEBUG((EFI_D_INFO, "Find the Driver, and Unload this driver Image.\n"));
            Status = gBS->UnloadImage(DriverImageHandleBuffer[Index]);
            break;
          }
    }
  }

  if (Index == DriverImageHandleCount) {
    DEBUG((EFI_D_INFO, " Driver image is not found\n"));
    Status = EFI_NOT_FOUND;
  }

ProcExit:
  if(DriverImageHandleBuffer != NULL){
    FreePool(DriverImageHandleBuffer);
  }

  return Status;
}

/**
  Connect network driver

  @param None

  @retval None
**/
VOID
EFIAPI
ConnectNetworkDriverWithPciIo(
  VOID
  )
{
  EFI_STATUS                   Status;
  UINTN                        Index;
  UINTN                        HandleCount;
  EFI_HANDLE                   *HandleBuffer = NULL;
  EFI_PCI_IO_PROTOCOL          *PciIo;
  UINT32                       Data32;
  UINT8                        ClassCode[3];

DEBUG((EFI_D_INFO," Connect Network Driver\n"));
  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiPciIoProtocolGuid,
                NULL,
                &HandleCount,
                &HandleBuffer
                );
  if(EFI_ERROR(Status) || HandleCount == 0){
    goto Exit;
  }
  for(Index=0; Index<HandleCount; Index++){ 
    Status = gBS->HandleProtocol(
                  HandleBuffer[Index],
                  &gEfiPciIoProtocolGuid,
                  (VOID**)&PciIo
                  );

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &Data32);
    if((UINT16)Data32 == 0xFFFF){
      continue;
    }

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, &ClassCode[0]);
    if (ClassCode[2] == 2 && ClassCode[1] == 0 && ClassCode[0] == 0) {
      DEBUG((EFI_D_INFO,"ConnectController\n"));
      gBS->ConnectController (HandleBuffer[Index], NULL, NULL, TRUE);
    }
  }

Exit:
  if(HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }

  return ;
}

/**
  Disconnect network driver

  @param None

  @retval None
**/
VOID
EFIAPI
DisconnectNetworkDriverWithPciIo(
  VOID
  )
{
  EFI_STATUS                   Status;
  UINTN                        Index;
  UINTN                        HandleCount;
  EFI_HANDLE                   *HandleBuffer = NULL;
  EFI_PCI_IO_PROTOCOL          *PciIo;
  UINT32                       Data32;
  UINT8                        ClassCode[3];

DEBUG((EFI_D_INFO,"Disconnect Network Driver\n"));
  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiPciIoProtocolGuid,
                NULL,
                &HandleCount,
                &HandleBuffer
                );
  if(EFI_ERROR(Status) || HandleCount == 0){
    goto Exit;
  }
  for(Index=0; Index<HandleCount; Index++){ 
    Status = gBS->HandleProtocol(
                  HandleBuffer[Index],
                  &gEfiPciIoProtocolGuid,
                  (VOID**)&PciIo
                  );

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, PCI_VENDOR_ID_OFFSET, 1, &Data32);
    if((UINT16)Data32 == 0xFFFF){
      continue;
    }

    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, &ClassCode[0]);
    if (ClassCode[2] == 2 && ClassCode[1] == 0 && ClassCode[0] == 0) {
      DEBUG((EFI_D_INFO,"DisconnectController\n"));
      gBS->DisconnectController (HandleBuffer[Index], NULL, NULL);
    }
  }

Exit:
  if(HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }

  return ;
}

/**
  Delete boot group order and boot order variable

  @param None

  @retval None
**/
VOID
EFIAPI
ByoRemoveBootOrder(
  VOID
  )
{
  EFI_STATUS                      Status;
  UINTN                           VarSize = 0;
  UINT16                          *BootOrder = NULL;
  UINTN                           Index;
  UINTN                           Count;
  CHAR16                          OptionName[8+1];

  Status = gRT->GetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &VarSize,
                  BootOrder
                  );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    BootOrder = AllocatePool(VarSize);
    ASSERT(BootOrder != NULL);
    Status = gRT->GetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &VarSize,
                  BootOrder
                  );
  }
  DEBUG((EFI_D_INFO, "Get BootOrder %r (%X %X)\n", Status, BootOrder, VarSize));

  if (!EFI_ERROR(Status)) {
    Count = VarSize/sizeof(UINT16);
    for (Index = 0; Index < Count; Index++) {
      UnicodeSPrint(OptionName, sizeof(OptionName), L"Boot%04X", BootOrder[Index]);
      Status = gRT->SetVariable (
                      OptionName,
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );
      DEBUG((EFI_D_INFO, "delete %s:%r\n", OptionName, Status));
    }
    Status = gRT->SetVariable (
                L"BootOrder",
                &gEfiGlobalVariableGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                0,
                NULL
                );
    DEBUG((EFI_D_INFO, "delete bootorder:%r\n", Status));
  }

  Status = gRT->SetVariable (
                  BYO_UEFI_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  0,
                  NULL
                  );
  DEBUG((EFI_D_INFO, "delete uefi boot group: %r\n", Status));

  Status = gRT->SetVariable (
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  0,
                  NULL
                  );
  DEBUG((EFI_D_INFO, "delete legacy boot group: %r\n", Status));

  if(BootOrder != NULL){
    FreePool(BootOrder);
  }
}

EFI_STATUS 
GetBiosEfiFileFvHandleInAllFv(
  IN  EFI_GUID      *FfsGuid,
  OUT EFI_HANDLE    *FvHandle
  )
{
  UINTN                              FvHandleCount;
  EFI_HANDLE                         *FvHandleBuffer = NULL;  
  EFI_STATUS                         Status;
  UINTN                              Index;
  EFI_FIRMWARE_VOLUME2_PROTOCOL      *Fv2;
  UINT32                             AuthenticationStatus;
  VOID                               *Buffer;
  UINTN                              Size;


  ASSERT(FfsGuid != NULL && FvHandle != NULL);
  *FvHandle = NULL;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &FvHandleCount,
                  &FvHandleBuffer
                  );
  if(EFI_ERROR(Status) || FvHandleCount==0){
    goto ProcExit;
  }
  
  for (Index = 0; Index < FvHandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    FvHandleBuffer[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID**)&Fv2
                    );
    ASSERT(!EFI_ERROR(Status));

    Buffer  = NULL;
    Size    = 0;
    Status  = Fv2->ReadSection (
                    Fv2,
                    FfsGuid,
                    EFI_SECTION_PE32,
                    0,
                    &Buffer,
                    &Size,
                    &AuthenticationStatus
                    );
    if(!EFI_ERROR(Status)){
      if (Buffer != NULL) {
        FreePool(Buffer);
      }
      *FvHandle = FvHandleBuffer[Index];
      break;
    }

  }

ProcExit:

  if (FvHandleBuffer != NULL) {
    gBS->FreePool(FvHandleBuffer);
  }  
  if(*FvHandle == NULL){
    return EFI_NOT_FOUND;
  } else {
    return EFI_SUCCESS;
  }
}

EFI_STATUS
EFIAPI
CreateFvBootOption (
  EFI_GUID                     *FileGuid,
  CHAR16                       *Description,
  EFI_BOOT_MANAGER_LOAD_OPTION *BootOption,
  BOOLEAN                      IsBootCategory,
  UINT8                        *OptionalData,    OPTIONAL
  UINT32                       OptionalDataSize       
  )
{
  EFI_STATUS                         Status;
  EFI_DEVICE_PATH_PROTOCOL           *DevicePath;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH  FileNode;
  UINT32                             Attributes;
  EFI_HANDLE                         FvHandle;  
  

  if ((BootOption == NULL) || (FileGuid == NULL) || (Description == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  EfiInitializeFwVolDevicepathNode (&FileNode, FileGuid);

  Status = GetBiosEfiFileFvHandleInAllFv(FileGuid, &FvHandle);
  if (EFI_ERROR (Status)) {  
    return EFI_NOT_FOUND;
  }

  DevicePath = AppendDevicePathNode (
                 DevicePathFromHandle (FvHandle),
                 (EFI_DEVICE_PATH_PROTOCOL *) &FileNode
                 );

  Attributes = LOAD_OPTION_ACTIVE;
  if(IsBootCategory){
    Attributes |= LOAD_OPTION_CATEGORY_BOOT;
  } else {
    Attributes |= LOAD_OPTION_CATEGORY_APP;
  }
  
  Status = EfiBootManagerInitializeLoadOption (
             BootOption,
             LoadOptionNumberUnassigned,
             LoadOptionTypeBoot,
             Attributes,
             Description,
             DevicePath,
             OptionalData,
             OptionalDataSize
             );
  FreePool (DevicePath);
  return Status;
}