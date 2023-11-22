/** @file
  Library functions which relates with booting.

Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
Copyright (c) 2011 - 2021, Intel Corporation. All rights reserved.<BR>
(C) Copyright 2015-2021 Hewlett Packard Enterprise Development LP<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "InternalBm.h"
#include <Protocol/EfiPostLoginProtocol.h>
#include <Protocol/PlatformBootManager.h>
#include <Protocol/DisplayBackupRecovery.h>
#include <Protocol/UefiBootManager.h>

#include <Guid/BootRetryPolicyVariable.h>

#define TYPE_UEFI_PXE  5

#define MAX_DESCRIPTION_STR_LEN  48
EFI_RAM_DISK_PROTOCOL  *mRamDisk = NULL;

EFI_BOOT_MANAGER_REFRESH_LEGACY_BOOT_OPTION  mBmRefreshLegacyBootOption = NULL;
EFI_BOOT_MANAGER_LEGACY_BOOT                 mBmLegacyBoot              = NULL;
EDKII_UEFI_BOOT_MANAGER_PROTOCOL             *mBootMgrProtocol          = NULL;

BOOLEAN       gRetryOptionClassEntered = FALSE;
STATIC UINTN  *gBootOptionOrder        = NULL;


///
/// This GUID is used for an EFI Variable that stores the front device pathes
/// for a partial device path that starts with the HD node.
///
EFI_GUID  mBmHardDriveBootVariableGuid = {
  0xfab7e9e1, 0x39dd, 0x4f2b, { 0x84, 0x08, 0xe2, 0x0e, 0x90, 0x6c, 0xb6, 0xde }
};
EFI_GUID  mBmAutoCreateBootOptionGuid = {
  0x8108ac4e, 0x9f11, 0x4d59, { 0x85, 0x0e, 0xe2, 0x1a, 0x52, 0x2c, 0x59, 0xb2 }
};

/**

  End Perf entry of BDS

  @param  Event                 The triggered event.
  @param  Context               Context for this event.

**/
VOID
EFIAPI
BmEndOfBdsPerfCode (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  //
  // Record the performance data for End of BDS
  //
  PERF_CROSSMODULE_END ("BDS");

  return;
}

/**
  The function registers the legacy boot support capabilities.

  @param RefreshLegacyBootOption The function pointer to create all the legacy boot options.
  @param LegacyBoot              The function pointer to boot the legacy boot option.
**/
VOID
EFIAPI
EfiBootManagerRegisterLegacyBootSupport (
  EFI_BOOT_MANAGER_REFRESH_LEGACY_BOOT_OPTION  RefreshLegacyBootOption,
  EFI_BOOT_MANAGER_LEGACY_BOOT                 LegacyBoot
  )
{
  mBmRefreshLegacyBootOption = RefreshLegacyBootOption;
  mBmLegacyBoot              = LegacyBoot;
}

/**
  Return TRUE when the boot option is auto-created instead of manually added.

  @param BootOption Pointer to the boot option to check.

  @retval TRUE  The boot option is auto-created.
  @retval FALSE The boot option is manually added.
**/
BOOLEAN
EFIAPI
BmIsAutoCreateBootOption (
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  )
{
  if ((BootOption->OptionalDataSize == sizeof (EFI_GUID)) &&
      CompareGuid ((EFI_GUID *)BootOption->OptionalData, &mBmAutoCreateBootOptionGuid)
      )
  {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Find the boot option in the NV storage and return the option number.

  @param OptionToFind  Boot option to be checked.

  @return   The option number of the found boot option.

**/
UINTN
BmFindBootOptionInVariable (
  IN  EFI_BOOT_MANAGER_LOAD_OPTION  *OptionToFind
  )
{
  EFI_STATUS                    Status;
  EFI_BOOT_MANAGER_LOAD_OPTION  BootOption;
  UINTN                         OptionNumber;
  CHAR16                        OptionName[BM_OPTION_NAME_LEN];
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  UINTN                         BootOptionCount;
  UINTN                         Index;

  OptionNumber = LoadOptionNumberUnassigned;

  //
  // Try to match the variable exactly if the option number is assigned
  //
  if (OptionToFind->OptionNumber != LoadOptionNumberUnassigned) {
    UnicodeSPrint (
      OptionName,
      sizeof (OptionName),
      L"%s%04x",
      mBmLoadOptionName[OptionToFind->OptionType],
      OptionToFind->OptionNumber
      );
    Status = EfiBootManagerVariableToLoadOption (OptionName, &BootOption);

    if (!EFI_ERROR (Status)) {
      ASSERT (OptionToFind->OptionNumber == BootOption.OptionNumber);
      if ((OptionToFind->Attributes == BootOption.Attributes) &&
          (StrCmp (OptionToFind->Description, BootOption.Description) == 0) &&
          (CompareMem (OptionToFind->FilePath, BootOption.FilePath, GetDevicePathSize (OptionToFind->FilePath)) == 0) &&
          (OptionToFind->OptionalDataSize == BootOption.OptionalDataSize) &&
          (CompareMem (OptionToFind->OptionalData, BootOption.OptionalData, OptionToFind->OptionalDataSize) == 0)
          )
      {
        OptionNumber = OptionToFind->OptionNumber;
      }

      EfiBootManagerFreeLoadOption (&BootOption);
    }
  }

  //
  // The option number assigned is either incorrect or unassigned.
  //
  if (OptionNumber == LoadOptionNumberUnassigned) {
    BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);

    Index = EfiBootManagerFindLoadOption (OptionToFind, BootOptions, BootOptionCount);
    if (Index != -1) {
      OptionNumber = BootOptions[Index].OptionNumber;
    }

    EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
  }

  return OptionNumber;
}

/**
  Return the correct FV file path.
  FV address may change across reboot. This routine promises the FV file device path is right.

  @param  FilePath     The Memory Mapped Device Path to get the file buffer.

  @return  The updated FV Device Path pointint to the file.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmAdjustFvFilePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *FilePath
  )
{
  EFI_STATUS                 Status;
  UINTN                      Index;
  EFI_DEVICE_PATH_PROTOCOL   *FvFileNode;
  EFI_HANDLE                 FvHandle;
  EFI_LOADED_IMAGE_PROTOCOL  *LoadedImage;
  UINTN                      FvHandleCount;
  EFI_HANDLE                 *FvHandles;
  EFI_DEVICE_PATH_PROTOCOL   *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL   *FullPath;

  //
  // Get the file buffer by using the exactly FilePath.
  //
  FvFileNode = FilePath;
  Status     = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &FvFileNode, &FvHandle);
  if (!EFI_ERROR (Status)) {
    return DuplicateDevicePath (FilePath);
  }

  //
  // Only wide match other FVs if it's a memory mapped FV file path.
  //
  if ((DevicePathType (FilePath) != HARDWARE_DEVICE_PATH) || (DevicePathSubType (FilePath) != HW_MEMMAP_DP)) {
    return NULL;
  }

  FvFileNode = NextDevicePathNode (FilePath);

  //
  // Firstly find the FV file in current FV
  //
  gBS->HandleProtocol (
         gImageHandle,
         &gEfiLoadedImageProtocolGuid,
         (VOID **)&LoadedImage
         );
  NewDevicePath = AppendDevicePathNode (DevicePathFromHandle (LoadedImage->DeviceHandle), FvFileNode);
  FullPath      = BmAdjustFvFilePath (NewDevicePath);
  FreePool (NewDevicePath);
  if (FullPath != NULL) {
    return FullPath;
  }

  //
  // Secondly find the FV file in all other FVs
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiFirmwareVolume2ProtocolGuid,
         NULL,
         &FvHandleCount,
         &FvHandles
         );
  for (Index = 0; Index < FvHandleCount; Index++) {
    if (FvHandles[Index] == LoadedImage->DeviceHandle) {
      //
      // Skip current FV, it was handed in first step.
      //
      continue;
    }

    NewDevicePath = AppendDevicePathNode (DevicePathFromHandle (FvHandles[Index]), FvFileNode);
    FullPath      = BmAdjustFvFilePath (NewDevicePath);
    FreePool (NewDevicePath);
    if (FullPath != NULL) {
      break;
    }
  }

  if (FvHandles != NULL) {
    FreePool (FvHandles);
  }

  return FullPath;
}

/**
  Check if it's a Device Path pointing to FV file.

  The function doesn't garentee the device path points to existing FV file.

  @param  DevicePath     Input device path.

  @retval TRUE   The device path is a FV File Device Path.
  @retval FALSE  The device path is NOT a FV File Device Path.
**/
BOOLEAN
BmIsFvFilePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *Node;

  Node   = DevicePath;
  Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &Node, &Handle);
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  if ((DevicePathType (DevicePath) == HARDWARE_DEVICE_PATH) && (DevicePathSubType (DevicePath) == HW_MEMMAP_DP)) {
    DevicePath = NextDevicePathNode (DevicePath);
    if ((DevicePathType (DevicePath) == MEDIA_DEVICE_PATH) && (DevicePathSubType (DevicePath) == MEDIA_PIWG_FW_FILE_DP)) {
      return IsDevicePathEnd (NextDevicePathNode (DevicePath));
    }
  }

  return FALSE;
}

/**
  Check whether a USB device match the specified USB Class device path. This
  function follows "Load Option Processing" behavior in UEFI specification.

  @param UsbIo       USB I/O protocol associated with the USB device.
  @param UsbClass    The USB Class device path to match.

  @retval TRUE       The USB device match the USB Class device path.
  @retval FALSE      The USB device does not match the USB Class device path.

**/
BOOLEAN
BmMatchUsbClass (
  IN EFI_USB_IO_PROTOCOL    *UsbIo,
  IN USB_CLASS_DEVICE_PATH  *UsbClass
  )
{
  EFI_STATUS                    Status;
  EFI_USB_DEVICE_DESCRIPTOR     DevDesc;
  EFI_USB_INTERFACE_DESCRIPTOR  IfDesc;
  UINT8                         DeviceClass;
  UINT8                         DeviceSubClass;
  UINT8                         DeviceProtocol;

  if ((DevicePathType (UsbClass) != MESSAGING_DEVICE_PATH) ||
      (DevicePathSubType (UsbClass) != MSG_USB_CLASS_DP))
  {
    return FALSE;
  }

  //
  // Check Vendor Id and Product Id.
  //
  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if ((UsbClass->VendorId != 0xffff) &&
      (UsbClass->VendorId != DevDesc.IdVendor))
  {
    return FALSE;
  }

  if ((UsbClass->ProductId != 0xffff) &&
      (UsbClass->ProductId != DevDesc.IdProduct))
  {
    return FALSE;
  }

  DeviceClass    = DevDesc.DeviceClass;
  DeviceSubClass = DevDesc.DeviceSubClass;
  DeviceProtocol = DevDesc.DeviceProtocol;
  if (DeviceClass == 0) {
    //
    // If Class in Device Descriptor is set to 0, use the Class, SubClass and
    // Protocol in Interface Descriptor instead.
    //
    Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &IfDesc);
    if (EFI_ERROR (Status)) {
      return FALSE;
    }

    DeviceClass    = IfDesc.InterfaceClass;
    DeviceSubClass = IfDesc.InterfaceSubClass;
    DeviceProtocol = IfDesc.InterfaceProtocol;
  }

  //
  // Check Class, SubClass and Protocol.
  //
  if ((UsbClass->DeviceClass != 0xff) &&
      (UsbClass->DeviceClass != DeviceClass))
  {
    return FALSE;
  }

  if ((UsbClass->DeviceSubClass != 0xff) &&
      (UsbClass->DeviceSubClass != DeviceSubClass))
  {
    return FALSE;
  }

  if ((UsbClass->DeviceProtocol != 0xff) &&
      (UsbClass->DeviceProtocol != DeviceProtocol))
  {
    return FALSE;
  }

  return TRUE;
}

/**
  Check whether a USB device match the specified USB WWID device path. This
  function follows "Load Option Processing" behavior in UEFI specification.

  @param UsbIo       USB I/O protocol associated with the USB device.
  @param UsbWwid     The USB WWID device path to match.

  @retval TRUE       The USB device match the USB WWID device path.
  @retval FALSE      The USB device does not match the USB WWID device path.

**/
BOOLEAN
BmMatchUsbWwid (
  IN EFI_USB_IO_PROTOCOL   *UsbIo,
  IN USB_WWID_DEVICE_PATH  *UsbWwid
  )
{
  EFI_STATUS                    Status;
  EFI_USB_DEVICE_DESCRIPTOR     DevDesc;
  EFI_USB_INTERFACE_DESCRIPTOR  IfDesc;
  UINT16                        *LangIdTable;
  UINT16                        TableSize;
  UINT16                        Index;
  CHAR16                        *CompareStr;
  UINTN                         CompareLen;
  CHAR16                        *SerialNumberStr;
  UINTN                         Length;

  if ((DevicePathType (UsbWwid) != MESSAGING_DEVICE_PATH) ||
      (DevicePathSubType (UsbWwid) != MSG_USB_WWID_DP))
  {
    return FALSE;
  }

  //
  // Check Vendor Id and Product Id.
  //
  Status = UsbIo->UsbGetDeviceDescriptor (UsbIo, &DevDesc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if ((DevDesc.IdVendor != UsbWwid->VendorId) ||
      (DevDesc.IdProduct != UsbWwid->ProductId))
  {
    return FALSE;
  }

  //
  // Check Interface Number.
  //
  Status = UsbIo->UsbGetInterfaceDescriptor (UsbIo, &IfDesc);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  if (IfDesc.InterfaceNumber != UsbWwid->InterfaceNumber) {
    return FALSE;
  }

  //
  // Check Serial Number.
  //
  if (DevDesc.StrSerialNumber == 0) {
    return FALSE;
  }

  //
  // Get all supported languages.
  //
  TableSize   = 0;
  LangIdTable = NULL;
  Status      = UsbIo->UsbGetSupportedLanguages (UsbIo, &LangIdTable, &TableSize);
  if (EFI_ERROR (Status) || (TableSize == 0) || (LangIdTable == NULL)) {
    return FALSE;
  }

  //
  // Serial number in USB WWID device path is the last 64-or-less UTF-16 characters.
  //
  CompareStr = (CHAR16 *)(UINTN)(UsbWwid + 1);
  CompareLen = (DevicePathNodeLength (UsbWwid) - sizeof (USB_WWID_DEVICE_PATH)) / sizeof (CHAR16);
  if (CompareStr[CompareLen - 1] == L'\0') {
    CompareLen--;
  }

  //
  // Compare serial number in each supported language.
  //
  for (Index = 0; Index < TableSize / sizeof (UINT16); Index++) {
    SerialNumberStr = NULL;
    Status          = UsbIo->UsbGetStringDescriptor (
                               UsbIo,
                               LangIdTable[Index],
                               DevDesc.StrSerialNumber,
                               &SerialNumberStr
                               );
    if (EFI_ERROR (Status) || (SerialNumberStr == NULL)) {
      continue;
    }

    Length = StrLen (SerialNumberStr);
    if ((Length >= CompareLen) &&
        (CompareMem (SerialNumberStr + Length - CompareLen, CompareStr, CompareLen * sizeof (CHAR16)) == 0))
    {
      FreePool (SerialNumberStr);
      return TRUE;
    }

    FreePool (SerialNumberStr);
  }

  return FALSE;
}

/**
  Find a USB device which match the specified short-form device path start with
  USB Class or USB WWID device path. If ParentDevicePath is NULL, this function
  will search in all USB devices of the platform. If ParentDevicePath is not NULL,
  this function will only search in its child devices.

  @param DevicePath           The device path that contains USB Class or USB WWID device path.
  @param ParentDevicePathSize The length of the device path before the USB Class or
                              USB WWID device path.
  @param UsbIoHandleCount     A pointer to the count of the returned USB IO handles.

  @retval NULL       The matched USB IO handles cannot be found.
  @retval other      The matched USB IO handles.

**/
EFI_HANDLE *
BmFindUsbDevice (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN  UINTN                     ParentDevicePathSize,
  OUT UINTN                     *UsbIoHandleCount
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                *UsbIoHandles;
  EFI_DEVICE_PATH_PROTOCOL  *UsbIoDevicePath;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  UINTN                     Index;
  BOOLEAN                   Matched;

  ASSERT (UsbIoHandleCount != NULL);

  //
  // Get all UsbIo Handles.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiUsbIoProtocolGuid,
                  NULL,
                  UsbIoHandleCount,
                  &UsbIoHandles
                  );
  if (EFI_ERROR (Status)) {
    *UsbIoHandleCount = 0;
    UsbIoHandles      = NULL;
  }

  for (Index = 0; Index < *UsbIoHandleCount; ) {
    //
    // Get the Usb IO interface.
    //
    Status = gBS->HandleProtocol (
                    UsbIoHandles[Index],
                    &gEfiUsbIoProtocolGuid,
                    (VOID **)&UsbIo
                    );
    UsbIoDevicePath = DevicePathFromHandle (UsbIoHandles[Index]);
    Matched         = FALSE;
    if (!EFI_ERROR (Status) && (UsbIoDevicePath != NULL)) {
      //
      // Compare starting part of UsbIoHandle's device path with ParentDevicePath.
      //
      if (CompareMem (UsbIoDevicePath, DevicePath, ParentDevicePathSize) == 0) {
        if (BmMatchUsbClass (UsbIo, (USB_CLASS_DEVICE_PATH *)((UINTN)DevicePath + ParentDevicePathSize)) ||
            BmMatchUsbWwid (UsbIo, (USB_WWID_DEVICE_PATH *)((UINTN)DevicePath + ParentDevicePathSize)))
        {
          Matched = TRUE;
        }
      }
    }

    if (!Matched) {
      (*UsbIoHandleCount)--;
      CopyMem (&UsbIoHandles[Index], &UsbIoHandles[Index + 1], (*UsbIoHandleCount - Index) * sizeof (EFI_HANDLE));
    } else {
      Index++;
    }
  }

  return UsbIoHandles;
}

/**
  Expand USB Class or USB WWID device path node to be full device path of a USB
  device in platform.

  This function support following 4 cases:
  1) Boot Option device path starts with a USB Class or USB WWID device path,
     and there is no Media FilePath device path in the end.
     In this case, it will follow Removable Media Boot Behavior.
  2) Boot Option device path starts with a USB Class or USB WWID device path,
     and ended with Media FilePath device path.
  3) Boot Option device path starts with a full device path to a USB Host Controller,
     contains a USB Class or USB WWID device path node, while not ended with Media
     FilePath device path. In this case, it will follow Removable Media Boot Behavior.
  4) Boot Option device path starts with a full device path to a USB Host Controller,
     contains a USB Class or USB WWID device path node, and ended with Media
     FilePath device path.

  @param FilePath      The device path pointing to a load option.
                       It could be a short-form device path.
  @param FullPath      The full path returned by the routine in last call.
                       Set to NULL in first call.
  @param ShortformNode Pointer to the USB short-form device path node in the FilePath buffer.

  @return The next possible full path pointing to the load option.
          Caller is responsible to free the memory.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmExpandUsbDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *FullPath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ShortformNode
  )
{
  UINTN                     ParentDevicePathSize;
  EFI_DEVICE_PATH_PROTOCOL  *RemainingDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *NextFullPath;
  EFI_HANDLE                *Handles;
  UINTN                     HandleCount;
  UINTN                     Index;
  BOOLEAN                   GetNext;

  NextFullPath         = NULL;
  GetNext              = (BOOLEAN)(FullPath == NULL);
  ParentDevicePathSize = (UINTN)ShortformNode - (UINTN)FilePath;
  RemainingDevicePath  = NextDevicePathNode (ShortformNode);
  Handles              = BmFindUsbDevice (FilePath, ParentDevicePathSize, &HandleCount);

  for (Index = 0; Index < HandleCount; Index++) {
    FilePath = AppendDevicePath (DevicePathFromHandle (Handles[Index]), RemainingDevicePath);
    if (FilePath == NULL) {
      //
      // Out of memory.
      //
      continue;
    }

    NextFullPath = BmGetNextLoadOptionDevicePath (FilePath, NULL);
    FreePool (FilePath);
    if (NextFullPath == NULL) {
      //
      // No BlockIo or SimpleFileSystem under FilePath.
      //
      continue;
    }

    if (GetNext) {
      break;
    } else {
      GetNext = (BOOLEAN)(CompareMem (NextFullPath, FullPath, GetDevicePathSize (NextFullPath)) == 0);
      FreePool (NextFullPath);
      NextFullPath = NULL;
    }
  }

  if (Handles != NULL) {
    FreePool (Handles);
  }

  return NextFullPath;
}

/**
  Expand File-path device path node to be full device path in platform.

  @param FilePath      The device path pointing to a load option.
                       It could be a short-form device path.
  @param FullPath      The full path returned by the routine in last call.
                       Set to NULL in first call.

  @return The next possible full path pointing to the load option.
          Caller is responsible to free the memory.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmExpandFileDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *FullPath
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     HandleCount;
  EFI_HANDLE                *Handles;
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;
  UINTN                     MediaType;
  EFI_DEVICE_PATH_PROTOCOL  *NextFullPath;
  BOOLEAN                   GetNext;

  EfiBootManagerConnectAll ();
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiSimpleFileSystemProtocolGuid, NULL, &HandleCount, &Handles);
  if (EFI_ERROR (Status)) {
    HandleCount = 0;
    Handles     = NULL;
  }

  GetNext      = (BOOLEAN)(FullPath == NULL);
  NextFullPath = NULL;
  //
  // Enumerate all removable media devices followed by all fixed media devices,
  //   followed by media devices which don't layer on block io.
  //
  for (MediaType = 0; MediaType < 3; MediaType++) {
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (Handles[Index], &gEfiBlockIoProtocolGuid, (VOID *)&BlockIo);
      if (EFI_ERROR (Status)) {
        BlockIo = NULL;
      }

      if (((MediaType == 0) && (BlockIo != NULL) && BlockIo->Media->RemovableMedia) ||
          ((MediaType == 1) && (BlockIo != NULL) && !BlockIo->Media->RemovableMedia) ||
          ((MediaType == 2) && (BlockIo == NULL))
          )
      {
        NextFullPath = AppendDevicePath (DevicePathFromHandle (Handles[Index]), FilePath);
        if (GetNext) {
          break;
        } else {
          GetNext = (BOOLEAN)(CompareMem (NextFullPath, FullPath, GetDevicePathSize (NextFullPath)) == 0);
          FreePool (NextFullPath);
          NextFullPath = NULL;
        }
      }
    }

    if (NextFullPath != NULL) {
      break;
    }
  }

  if (Handles != NULL) {
    FreePool (Handles);
  }

  return NextFullPath;
}

/**
  Expand URI device path node to be full device path in platform.

  @param FilePath      The device path pointing to a load option.
                       It could be a short-form device path.
  @param FullPath      The full path returned by the routine in last call.
                       Set to NULL in first call.

  @return The next possible full path pointing to the load option.
          Caller is responsible to free the memory.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmExpandUriDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *FullPath
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     HandleCount;
  EFI_HANDLE                *Handles;
  EFI_DEVICE_PATH_PROTOCOL  *NextFullPath;
  EFI_DEVICE_PATH_PROTOCOL  *RamDiskDevicePath;
  BOOLEAN                   GetNext;

  EfiBootManagerConnectAll ();
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiLoadFileProtocolGuid, NULL, &HandleCount, &Handles);
  if (EFI_ERROR (Status)) {
    HandleCount = 0;
    Handles     = NULL;
  }

  NextFullPath = NULL;
  GetNext      = (BOOLEAN)(FullPath == NULL);
  for (Index = 0; Index < HandleCount; Index++) {
    NextFullPath = BmExpandLoadFile (Handles[Index], FilePath);

    if (NextFullPath == NULL) {
      continue;
    }

    if (GetNext) {
      break;
    } else {
      GetNext = (BOOLEAN)(CompareMem (NextFullPath, FullPath, GetDevicePathSize (NextFullPath)) == 0);
      //
      // Free the resource occupied by the RAM disk.
      //
      RamDiskDevicePath = BmGetRamDiskDevicePath (NextFullPath);
      if (RamDiskDevicePath != NULL) {
        BmDestroyRamDisk (RamDiskDevicePath);
        FreePool (RamDiskDevicePath);
      }

      FreePool (NextFullPath);
      NextFullPath = NULL;
    }
  }

  if (Handles != NULL) {
    FreePool (Handles);
  }

  return NextFullPath;
}

/**
  Save the partition DevicePath to the CachedDevicePath as the first instance.

  @param CachedDevicePath  The device path cache.
  @param DevicePath        The partition device path to be cached.
**/
VOID
BmCachePartitionDevicePath (
  IN OUT EFI_DEVICE_PATH_PROTOCOL  **CachedDevicePath,
  IN EFI_DEVICE_PATH_PROTOCOL      *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  UINTN                     Count;

  if (BmMatchDevicePaths (*CachedDevicePath, DevicePath)) {
    TempDevicePath    = *CachedDevicePath;
    *CachedDevicePath = BmDelPartMatchInstance (*CachedDevicePath, DevicePath);
    FreePool (TempDevicePath);
  }

  if (*CachedDevicePath == NULL) {
    *CachedDevicePath = DuplicateDevicePath (DevicePath);
    return;
  }

  TempDevicePath    = *CachedDevicePath;
  *CachedDevicePath = AppendDevicePathInstance (DevicePath, *CachedDevicePath);
  if (TempDevicePath != NULL) {
    FreePool (TempDevicePath);
  }

  //
  // Here limit the device path instance number to 12, which is max number for a system support 3 IDE controller
  // If the user try to boot many OS in different HDs or partitions, in theory, the 'HDDP' variable maybe become larger and larger.
  //
  Count          = 0;
  TempDevicePath = *CachedDevicePath;
  while (!IsDevicePathEnd (TempDevicePath)) {
    TempDevicePath = NextDevicePathNode (TempDevicePath);
    //
    // Parse one instance
    //
    while (!IsDevicePathEndType (TempDevicePath)) {
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }

    Count++;
    //
    // If the CachedDevicePath variable contain too much instance, only remain 12 instances.
    //
    if (Count == 12) {
      SetDevicePathEndNode (TempDevicePath);
      break;
    }
  }
}

/**
  Expand a device path that starts with a hard drive media device path node to be a
  full device path that includes the full hardware path to the device. We need
  to do this so it can be booted. As an optimization the front match (the part point
  to the partition node. E.g. ACPI() /PCI()/ATA()/Partition() ) is saved in a variable
  so a connect all is not required on every boot. All successful history device path
  which point to partition node (the front part) will be saved.

  @param FilePath      The device path pointing to a load option.
                       It could be a short-form device path.

  @return The full device path pointing to the load option.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmExpandPartitionDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath
  )
{
  EFI_STATUS                Status;
  UINTN                     BlockIoHandleCount;
  EFI_HANDLE                *BlockIoBuffer;
  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *CachedDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempNewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FullPath;
  UINTN                     CachedDevicePathSize;
  BOOLEAN                   NeedAdjust;
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  UINTN                     Size;

  //
  // Check if there is prestore 'HDDP' variable.
  // If exist, search the front path which point to partition node in the variable instants.
  // If fail to find or 'HDDP' not exist, reconnect all and search in all system
  //
  GetVariable2 (L"HDDP", &mBmHardDriveBootVariableGuid, (VOID **)&CachedDevicePath, &CachedDevicePathSize);

  //
  // Delete the invalid 'HDDP' variable.
  //
  if ((CachedDevicePath != NULL) && !IsDevicePathValid (CachedDevicePath, CachedDevicePathSize)) {
    FreePool (CachedDevicePath);
    CachedDevicePath = NULL;
    Status           = gRT->SetVariable (
                              L"HDDP",
                              &mBmHardDriveBootVariableGuid,
                              0,
                              0,
                              NULL
                              );
    ASSERT_EFI_ERROR (Status);
  }

  FullPath = NULL;
  if (CachedDevicePath != NULL) {
    TempNewDevicePath = CachedDevicePath;
    NeedAdjust        = FALSE;
    do {
      //
      // Check every instance of the variable
      // First, check whether the instance contain the partition node, which is needed for distinguishing  multi
      // partial partition boot option. Second, check whether the instance could be connected.
      //
      Instance = GetNextDevicePathInstance (&TempNewDevicePath, &Size);
      if (BmMatchPartitionDevicePathNode (Instance, (HARDDRIVE_DEVICE_PATH *)FilePath)) {
        //
        // Connect the device path instance, the device path point to hard drive media device path node
        // e.g. ACPI() /PCI()/ATA()/Partition()
        //
        Status = EfiBootManagerConnectDevicePath (Instance, NULL);
        if (!EFI_ERROR (Status)) {
          TempDevicePath = AppendDevicePath (Instance, NextDevicePathNode (FilePath));
          //
          // TempDevicePath = ACPI()/PCI()/ATA()/Partition()
          // or             = ACPI()/PCI()/ATA()/Partition()/.../A.EFI
          //
          // When TempDevicePath = ACPI()/PCI()/ATA()/Partition(),
          // it may expand to two potienal full paths (nested partition, rarely happen):
          //   1. ACPI()/PCI()/ATA()/Partition()/Partition(A1)/EFI/BootX64.EFI
          //   2. ACPI()/PCI()/ATA()/Partition()/Partition(A2)/EFI/BootX64.EFI
          // For simplicity, only #1 is returned.
          //
          FullPath = BmGetNextLoadOptionDevicePath (TempDevicePath, NULL);
          FreePool (TempDevicePath);

          if (FullPath != NULL) {
            //
            // Adjust the 'HDDP' instances sequence if the matched one is not first one.
            //
            if (NeedAdjust) {
              BmCachePartitionDevicePath (&CachedDevicePath, Instance);
              //
              // Save the matching Device Path so we don't need to do a connect all next time
              // Failing to save only impacts performance next time expanding the short-form device path
              //
              Status = gRT->SetVariable (
                              L"HDDP",
                              &mBmHardDriveBootVariableGuid,
                              EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                              GetDevicePathSize (CachedDevicePath),
                              CachedDevicePath
                              );
            }

            FreePool (Instance);
            FreePool (CachedDevicePath);
            return FullPath;
          }
        }
      }

      //
      // Come here means the first instance is not matched
      //
      NeedAdjust = TRUE;
      FreePool (Instance);
    } while (TempNewDevicePath != NULL);
  }

  //
  // If we get here we fail to find or 'HDDP' not exist, and now we need
  // to search all devices in the system for a matched partition
  //
  EfiBootManagerConnectAll ();
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiBlockIoProtocolGuid, NULL, &BlockIoHandleCount, &BlockIoBuffer);
  if (EFI_ERROR (Status)) {
    BlockIoHandleCount = 0;
    BlockIoBuffer      = NULL;
  }

  //
  // Loop through all the device handles that support the BLOCK_IO Protocol
  //
  for (Index = 0; Index < BlockIoHandleCount; Index++) {
    BlockIoDevicePath = DevicePathFromHandle (BlockIoBuffer[Index]);
    if (BlockIoDevicePath == NULL) {
      continue;
    }

    if (BmMatchPartitionDevicePathNode (BlockIoDevicePath, (HARDDRIVE_DEVICE_PATH *)FilePath)) {
      //
      // Find the matched partition device path
      //
      TempDevicePath = AppendDevicePath (BlockIoDevicePath, NextDevicePathNode (FilePath));
      FullPath       = BmGetNextLoadOptionDevicePath (TempDevicePath, NULL);
      FreePool (TempDevicePath);

      if (FullPath != NULL) {
        BmCachePartitionDevicePath (&CachedDevicePath, BlockIoDevicePath);

        //
        // Save the matching Device Path so we don't need to do a connect all next time
        // Failing to save only impacts performance next time expanding the short-form device path
        //
        Status = gRT->SetVariable (
                        L"HDDP",
                        &mBmHardDriveBootVariableGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        GetDevicePathSize (CachedDevicePath),
                        CachedDevicePath
                        );

        break;
      }
    }
  }

  if (CachedDevicePath != NULL) {
    FreePool (CachedDevicePath);
  }

  if (BlockIoBuffer != NULL) {
    FreePool (BlockIoBuffer);
  }

  return FullPath;
}

/**
  Expand the media device path which points to a BlockIo or SimpleFileSystem instance
  by appending EFI_REMOVABLE_MEDIA_FILE_NAME.

  @param DevicePath  The media device path pointing to a BlockIo or SimpleFileSystem instance.
  @param FullPath    The full path returned by the routine in last call.
                     Set to NULL in first call.

  @return The next possible full path pointing to the load option.
          Caller is responsible to free the memory.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmExpandMediaDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *FullPath
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  EFI_BLOCK_IO_PROTOCOL                 *BlockIo;
  VOID                                  *Buffer;
  EFI_DEVICE_PATH_PROTOCOL              *TempDevicePath;
  EFI_DEVICE_PATH_PROTOCOL              *NextFullPath;
  UINTN                                 Size;
  UINTN                                 TempSize;
  EFI_HANDLE                            *SimpleFileSystemHandles;
  UINTN                                 NumberSimpleFileSystemHandles;
  UINTN                                 Index;
  BOOLEAN                               GetNext;
  EDKII_PLATFORM_BOOT_MANAGER_PROTOCOL  *PlatformOsBootFile = NULL;
  CHAR16                                *BootFile           = EFI_REMOVABLE_MEDIA_FILE_NAME;

  GetNext = (BOOLEAN)(FullPath == NULL);
  //
  // Locate gPlatformOsBootFileProtocolGuid to find OS boot filename defined by the platform later
  //
  gBS->LocateProtocol (&gEdkiiPlatformBootManagerProtocolGuid, NULL, (VOID *)&PlatformOsBootFile);

  //
  // Check whether the device is connected
  //
  TempDevicePath = DevicePath;
  Status         = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &TempDevicePath, &Handle);
  if (!EFI_ERROR (Status)) {
    ASSERT (IsDevicePathEnd (TempDevicePath));
    if ((PlatformOsBootFile != NULL) && (PlatformOsBootFile->FindOsBootFile != NULL)) {
      BootFile = PlatformOsBootFile->FindOsBootFile (Handle);
      if (BootFile == NULL) {
        BootFile = EFI_REMOVABLE_MEDIA_FILE_NAME;
      }
    }

    NextFullPath = FileDevicePath (Handle, BootFile);
    //
    // For device path pointing to simple file system, it only expands to one full path.
    //
    if (GetNext) {
      return NextFullPath;
    } else {
      FreePool (NextFullPath);
      return NULL;
    }
  }

  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &TempDevicePath, &Handle);
  ASSERT_EFI_ERROR (Status);

  //
  // For device boot option only pointing to the removable device handle,
  // should make sure all its children handles (its child partion or media handles)
  // are created and connected.
  //
  gBS->ConnectController (Handle, NULL, NULL, TRUE);

  //
  // Issue a dummy read to the device to check for media change.
  // When the removable media is changed, any Block IO read/write will
  // cause the BlockIo protocol be reinstalled and EFI_MEDIA_CHANGED is
  // returned. After the Block IO protocol is reinstalled, subsequent
  // Block IO read/write will success.
  //
  Status = gBS->HandleProtocol (Handle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  Buffer = AllocatePool (BlockIo->Media->BlockSize);
  if (Buffer != NULL) {
    BlockIo->ReadBlocks (
               BlockIo,
               BlockIo->Media->MediaId,
               0,
               BlockIo->Media->BlockSize,
               Buffer
               );
    FreePool (Buffer);
  }

  //
  // Detect the the default boot file from removable Media
  //
  NextFullPath = NULL;
  Size         = GetDevicePathSize (DevicePath) - END_DEVICE_PATH_LENGTH;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &NumberSimpleFileSystemHandles,
         &SimpleFileSystemHandles
         );
  for (Index = 0; Index < NumberSimpleFileSystemHandles; Index++) {
    //
    // Get the device path size of SimpleFileSystem handle
    //
    TempDevicePath = DevicePathFromHandle (SimpleFileSystemHandles[Index]);
    TempSize       = GetDevicePathSize (TempDevicePath) - END_DEVICE_PATH_LENGTH;
    //
    // Check whether the device path of boot option is part of the SimpleFileSystem handle's device path
    //
    if ((Size <= TempSize) && (CompareMem (TempDevicePath, DevicePath, Size) == 0)) {
      if ((PlatformOsBootFile != NULL) && (PlatformOsBootFile->FindOsBootFile != NULL)) {
        BootFile = PlatformOsBootFile->FindOsBootFile (SimpleFileSystemHandles[Index]);
        if (BootFile == NULL) {
          BootFile = EFI_REMOVABLE_MEDIA_FILE_NAME;
        }
      }

      NextFullPath = FileDevicePath (SimpleFileSystemHandles[Index], BootFile);
      if (GetNext) {
        break;
      } else {
        GetNext = (BOOLEAN)(CompareMem (NextFullPath, FullPath, GetDevicePathSize (NextFullPath)) == 0);
        FreePool (NextFullPath);
        NextFullPath = NULL;
      }
    }
  }

  if (SimpleFileSystemHandles != NULL) {
    FreePool (SimpleFileSystemHandles);
  }

  return NextFullPath;
}

/**
  Check whether Left and Right are the same without matching the specific
  device path data in IP device path and URI device path node.

  @retval TRUE  Left and Right are the same.
  @retval FALSE Left and Right are the different.
**/
BOOLEAN
BmMatchHttpBootDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *Left,
  IN EFI_DEVICE_PATH_PROTOCOL  *Right
  )
{
  for ( ; !IsDevicePathEnd (Left) && !IsDevicePathEnd (Right)
        ; Left = NextDevicePathNode (Left), Right = NextDevicePathNode (Right)
        )
  {
    if (CompareMem (Left, Right, DevicePathNodeLength (Left)) != 0) {
      if ((DevicePathType (Left) != MESSAGING_DEVICE_PATH) || (DevicePathType (Right) != MESSAGING_DEVICE_PATH)) {
        return FALSE;
      }

      if (DevicePathSubType (Left) == MSG_DNS_DP) {
        Left = NextDevicePathNode (Left);
      }

      if (DevicePathSubType (Right) == MSG_DNS_DP) {
        Right = NextDevicePathNode (Right);
      }

      if (((DevicePathSubType (Left) != MSG_IPv4_DP) || (DevicePathSubType (Right) != MSG_IPv4_DP)) &&
          ((DevicePathSubType (Left) != MSG_IPv6_DP) || (DevicePathSubType (Right) != MSG_IPv6_DP)) &&
          ((DevicePathSubType (Left) != MSG_URI_DP)  || (DevicePathSubType (Right) != MSG_URI_DP))
          )
      {
        return FALSE;
      }
    }
  }

  return (BOOLEAN)(IsDevicePathEnd (Left) && IsDevicePathEnd (Right));
}

/**
  Get the file buffer from the file system produced by Load File instance.

  @param LoadFileHandle The handle of LoadFile instance.
  @param RamDiskHandle  Return the RAM Disk handle.

  @return The next possible full path pointing to the load option.
          Caller is responsible to free the memory.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmExpandNetworkFileSystem (
  IN  EFI_HANDLE  LoadFileHandle,
  OUT EFI_HANDLE  *RamDiskHandle
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_HANDLE                *Handles;
  UINTN                     HandleCount;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *Node;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiBlockIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if (EFI_ERROR (Status)) {
    Handles     = NULL;
    HandleCount = 0;
  }

  Handle = NULL;
  for (Index = 0; Index < HandleCount; Index++) {
    Node   = DevicePathFromHandle (Handles[Index]);
    Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &Node, &Handle);
    if (!EFI_ERROR (Status) &&
        (Handle == LoadFileHandle) &&
        (DevicePathType (Node) == MEDIA_DEVICE_PATH) && (DevicePathSubType (Node) == MEDIA_RAM_DISK_DP))
    {
      //
      // Find the BlockIo instance populated from the LoadFile.
      //
      Handle = Handles[Index];
      break;
    }
  }

  if (Handles != NULL) {
    FreePool (Handles);
  }

  if (Index == HandleCount) {
    Handle = NULL;
  }

  *RamDiskHandle = Handle;

  if (Handle != NULL) {
    //
    // Re-use BmExpandMediaDevicePath() to get the full device path of load option.
    // But assume only one SimpleFileSystem can be found under the BlockIo.
    //
    return BmExpandMediaDevicePath (DevicePathFromHandle (Handle), NULL);
  } else {
    return NULL;
  }
}

/**
  Return the RAM Disk device path created by LoadFile.

  @param FilePath  The source file path.

  @return Callee-to-free RAM Disk device path
**/
EFI_DEVICE_PATH_PROTOCOL *
BmGetRamDiskDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *FilePath
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *RamDiskDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  EFI_HANDLE                Handle;

  Node   = FilePath;
  Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &Node, &Handle);
  if (!EFI_ERROR (Status) &&
      (DevicePathType (Node) == MEDIA_DEVICE_PATH) &&
      (DevicePathSubType (Node) == MEDIA_RAM_DISK_DP)
      )
  {
    //
    // Construct the device path pointing to RAM Disk
    //
    Node              = NextDevicePathNode (Node);
    RamDiskDevicePath = DuplicateDevicePath (FilePath);
    ASSERT (RamDiskDevicePath != NULL);
    SetDevicePathEndNode ((VOID *)((UINTN)RamDiskDevicePath + ((UINTN)Node - (UINTN)FilePath)));
    return RamDiskDevicePath;
  }

  return NULL;
}

/**
  Return the buffer and buffer size occupied by the RAM Disk.

  @param RamDiskDevicePath  RAM Disk device path.
  @param RamDiskSizeInPages Return RAM Disk size in pages.

  @retval RAM Disk buffer.
**/
VOID *
BmGetRamDiskMemoryInfo (
  IN EFI_DEVICE_PATH_PROTOCOL  *RamDiskDevicePath,
  OUT UINTN                    *RamDiskSizeInPages
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;
  UINT64      StartingAddr;
  UINT64      EndingAddr;

  ASSERT (RamDiskDevicePath != NULL);

  *RamDiskSizeInPages = 0;

  //
  // Get the buffer occupied by RAM Disk.
  //
  Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &RamDiskDevicePath, &Handle);
  ASSERT_EFI_ERROR (Status);
  ASSERT (
    (DevicePathType (RamDiskDevicePath) == MEDIA_DEVICE_PATH) &&
    (DevicePathSubType (RamDiskDevicePath) == MEDIA_RAM_DISK_DP)
    );
  StartingAddr        = ReadUnaligned64 ((UINT64 *)((MEDIA_RAM_DISK_DEVICE_PATH *)RamDiskDevicePath)->StartingAddr);
  EndingAddr          = ReadUnaligned64 ((UINT64 *)((MEDIA_RAM_DISK_DEVICE_PATH *)RamDiskDevicePath)->EndingAddr);
  *RamDiskSizeInPages = EFI_SIZE_TO_PAGES ((UINTN)(EndingAddr - StartingAddr + 1));
  return (VOID *)(UINTN)StartingAddr;
}

/**
  Destroy the RAM Disk.

  The destroy operation includes to call RamDisk.Unregister to
  unregister the RAM DISK from RAM DISK driver, free the memory
  allocated for the RAM Disk.

  @param RamDiskDevicePath    RAM Disk device path.
**/
VOID
BmDestroyRamDisk (
  IN EFI_DEVICE_PATH_PROTOCOL  *RamDiskDevicePath
  )
{
  EFI_STATUS  Status;
  VOID        *RamDiskBuffer;
  UINTN       RamDiskSizeInPages;

  ASSERT (RamDiskDevicePath != NULL);

  RamDiskBuffer = BmGetRamDiskMemoryInfo (RamDiskDevicePath, &RamDiskSizeInPages);

  //
  // Destroy RAM Disk.
  //
  if (mRamDisk == NULL) {
    Status = gBS->LocateProtocol (&gEfiRamDiskProtocolGuid, NULL, (VOID *)&mRamDisk);
    ASSERT_EFI_ERROR (Status);
  }

  Status = mRamDisk->Unregister (RamDiskDevicePath);
  ASSERT_EFI_ERROR (Status);
  FreePages (RamDiskBuffer, RamDiskSizeInPages);
}

/**
  Get the file buffer from the specified Load File instance.

  @param LoadFileHandle The specified Load File instance.
  @param FilePath       The file path which will pass to LoadFile().

  @return  The full device path pointing to the load option buffer.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmExpandLoadFile (
  IN  EFI_HANDLE                LoadFileHandle,
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath
  )
{
  EFI_STATUS                Status;
  EFI_LOAD_FILE_PROTOCOL    *LoadFile;
  VOID                      *FileBuffer;
  EFI_HANDLE                RamDiskHandle;
  UINTN                     BufferSize;
  EFI_DEVICE_PATH_PROTOCOL  *FullPath;

  Status = gBS->OpenProtocol (
                  LoadFileHandle,
                  &gEfiLoadFileProtocolGuid,
                  (VOID **)&LoadFile,
                  gImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  ASSERT_EFI_ERROR (Status);

  FileBuffer = NULL;
  BufferSize = 0;
  Status     = LoadFile->LoadFile (LoadFile, FilePath, TRUE, &BufferSize, FileBuffer);
  if ((Status != EFI_WARN_FILE_SYSTEM) && (Status != EFI_BUFFER_TOO_SMALL)) {
    return NULL;
  }

  if (Status == EFI_BUFFER_TOO_SMALL) {
    //
    // The load option buffer is directly returned by LoadFile.
    //
    return DuplicateDevicePath (DevicePathFromHandle (LoadFileHandle));
  }

  //
  // The load option resides in a RAM disk.
  //
  FileBuffer = AllocateReservedPages (EFI_SIZE_TO_PAGES (BufferSize));
  if (FileBuffer == NULL) {
    DEBUG_CODE_BEGIN ();
    EFI_DEVICE_PATH  *LoadFilePath;
    CHAR16           *LoadFileText;
    CHAR16           *FileText;

    LoadFilePath = DevicePathFromHandle (LoadFileHandle);
    if (LoadFilePath == NULL) {
      LoadFileText = NULL;
    } else {
      LoadFileText = ConvertDevicePathToText (LoadFilePath, FALSE, FALSE);
    }

    FileText = ConvertDevicePathToText (FilePath, FALSE, FALSE);

    DEBUG ((
      DEBUG_ERROR,
      "%a:%a: failed to allocate reserved pages: "
      "BufferSize=%Lu LoadFile=\"%s\" FilePath=\"%s\"\n",
      gEfiCallerBaseName,
      __FUNCTION__,
      (UINT64)BufferSize,
      LoadFileText,
      FileText
      ));

    if (FileText != NULL) {
      FreePool (FileText);
    }

    if (LoadFileText != NULL) {
      FreePool (LoadFileText);
    }

    DEBUG_CODE_END ();
    return NULL;
  }

  Status = LoadFile->LoadFile (LoadFile, FilePath, TRUE, &BufferSize, FileBuffer);
  if (EFI_ERROR (Status)) {
    FreePages (FileBuffer, EFI_SIZE_TO_PAGES (BufferSize));
    return NULL;
  }

  FullPath = BmExpandNetworkFileSystem (LoadFileHandle, &RamDiskHandle);
  if (FullPath == NULL) {
    //
    // Free the memory occupied by the RAM disk if there is no BlockIo or SimpleFileSystem instance.
    //
    BmDestroyRamDisk (DevicePathFromHandle (RamDiskHandle));
  }

  return FullPath;
}

/**
  Return the full device path pointing to the load option.

  FilePath may:
  1. Exactly matches to a LoadFile instance.
  2. Cannot match to any LoadFile instance. Wide match is required.
  In either case, the routine may return:
  1. A copy of FilePath when FilePath matches to a LoadFile instance and
     the LoadFile returns a load option buffer.
  2. A new device path with IP and URI information updated when wide match
     happens.
  3. A new device path pointing to a load option in RAM disk.
  In either case, only one full device path is returned for a specified
  FilePath.

  @param FilePath    The media device path pointing to a LoadFile instance.

  @return  The load option buffer.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmExpandLoadFiles (
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                Handle;
  EFI_HANDLE                *Handles;
  UINTN                     HandleCount;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *Node;

  //
  // Get file buffer from load file instance.
  //
  Node   = FilePath;
  Status = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &Node, &Handle);
  if (!EFI_ERROR (Status) && IsDevicePathEnd (Node)) {
    //
    // When wide match happens, pass full device path to LoadFile (),
    // otherwise, pass remaining device path to LoadFile ().
    //
    FilePath = Node;
  } else {
    Handle = NULL;
    //
    // Use wide match algorithm to find one when
    //  cannot find a LoadFile instance to exactly match the FilePath
    //
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiLoadFileProtocolGuid,
                    NULL,
                    &HandleCount,
                    &Handles
                    );
    if (EFI_ERROR (Status)) {
      Handles     = NULL;
      HandleCount = 0;
    }

    for (Index = 0; Index < HandleCount; Index++) {
      if (BmMatchHttpBootDevicePath (DevicePathFromHandle (Handles[Index]), FilePath)) {
        Handle = Handles[Index];
        break;
      }
    }

    if (Handles != NULL) {
      FreePool (Handles);
    }
  }

  if (Handle == NULL) {
    return NULL;
  }

  return BmExpandLoadFile (Handle, FilePath);
}

/**
  Get the load option by its device path.

  @param FilePath  The device path pointing to a load option.
                   It could be a short-form device path.
  @param FullPath  Return the full device path of the load option after
                   short-form device path expanding.
                   Caller is responsible to free it.
  @param FileSize  Return the load option size.

  @return The load option buffer. Caller is responsible to free the memory.
**/
VOID *
EFIAPI
EfiBootManagerGetLoadOptionBuffer (
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath,
  OUT EFI_DEVICE_PATH_PROTOCOL  **FullPath,
  OUT UINTN                     *FileSize
  )
{
  *FullPath = NULL;

  EfiBootManagerConnectDevicePath (FilePath, NULL);
  return BmGetNextLoadOptionBuffer (LoadOptionTypeMax, FilePath, FullPath, FileSize);
}

/**
  Get the next possible full path pointing to the load option.
  The routine doesn't guarantee the returned full path points to an existing
  file, and it also doesn't guarantee the existing file is a valid load option.
  BmGetNextLoadOptionBuffer() guarantees.

  @param FilePath  The device path pointing to a load option.
                   It could be a short-form device path.
  @param FullPath  The full path returned by the routine in last call.
                   Set to NULL in first call.

  @return The next possible full path pointing to the load option.
          Caller is responsible to free the memory.
**/
EFI_DEVICE_PATH_PROTOCOL *
BmGetNextLoadOptionDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *FullPath
  )
{
  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *Node;
  EFI_STATUS                Status;

  ASSERT (FilePath != NULL);

  //
  // Boot from media device by adding a default file name \EFI\BOOT\BOOT{machine type short-name}.EFI
  //
  Node   = FilePath;
  Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &Node, &Handle);
  if (EFI_ERROR (Status)) {
    Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &Node, &Handle);
  }

  if (!EFI_ERROR (Status) && IsDevicePathEnd (Node)) {
    return BmExpandMediaDevicePath (FilePath, FullPath);
  }

  //
  // Expand the short-form device path to full device path
  //
  if ((DevicePathType (FilePath) == MEDIA_DEVICE_PATH) &&
      (DevicePathSubType (FilePath) == MEDIA_HARDDRIVE_DP))
  {
    //
    // Expand the Harddrive device path
    //
    if (FullPath == NULL) {
      return BmExpandPartitionDevicePath (FilePath);
    } else {
      return NULL;
    }
  } else if ((DevicePathType (FilePath) == MEDIA_DEVICE_PATH) &&
             (DevicePathSubType (FilePath) == MEDIA_FILEPATH_DP))
  {
    //
    // Expand the File-path device path
    //
    return BmExpandFileDevicePath (FilePath, FullPath);
  } else if ((DevicePathType (FilePath) == MESSAGING_DEVICE_PATH) &&
             (DevicePathSubType (FilePath) == MSG_URI_DP))
  {
    //
    // Expand the URI device path
    //
    return BmExpandUriDevicePath (FilePath, FullPath);
  } else {
    Node   = FilePath;
    Status = gBS->LocateDevicePath (&gEfiUsbIoProtocolGuid, &Node, &Handle);
    if (EFI_ERROR (Status)) {
      //
      // Only expand the USB WWID/Class device path
      // when FilePath doesn't point to a physical UsbIo controller.
      // Otherwise, infinite recursion will happen.
      //
      for (Node = FilePath; !IsDevicePathEnd (Node); Node = NextDevicePathNode (Node)) {
        if ((DevicePathType (Node) == MESSAGING_DEVICE_PATH) &&
            ((DevicePathSubType (Node) == MSG_USB_CLASS_DP) || (DevicePathSubType (Node) == MSG_USB_WWID_DP)))
        {
          break;
        }
      }

      //
      // Expand the USB WWID/Class device path
      //
      if (!IsDevicePathEnd (Node)) {
        if (FilePath == Node) {
          //
          // Boot Option device path starts with USB Class or USB WWID device path.
          // For Boot Option device path which doesn't begin with the USB Class or
          // USB WWID device path, it's not needed to connect again here.
          //
          BmConnectUsbShortFormDevicePath (FilePath);
        }

        return BmExpandUsbDevicePath (FilePath, FullPath, Node);
      }
    }
  }

  //
  // For the below cases, FilePath only expands to one Full path.
  // So just handle the case when FullPath == NULL.
  //
  if (FullPath != NULL) {
    return NULL;
  }

  //
  // Load option resides in FV.
  //
  if (BmIsFvFilePath (FilePath)) {
    return BmAdjustFvFilePath (FilePath);
  }

  //
  // Load option resides in Simple File System.
  //
  Node   = FilePath;
  Status = gBS->LocateDevicePath (&gEfiSimpleFileSystemProtocolGuid, &Node, &Handle);
  if (!EFI_ERROR (Status)) {
    return DuplicateDevicePath (FilePath);
  }

  //
  // Last chance to try: Load option may be loaded through LoadFile.
  //
  return BmExpandLoadFiles (FilePath);
}

/**
  Check if it's a Device Path pointing to BootManagerMenu.

  @param  DevicePath     Input device path.

  @retval TRUE   The device path is BootManagerMenu File Device Path.
  @retval FALSE  The device path is NOT BootManagerMenu File Device Path.
**/
BOOLEAN
EFIAPI
BmIsBootManagerMenuFilePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_HANDLE  FvHandle;
  VOID        *NameGuid;
  EFI_STATUS  Status;

  Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &DevicePath, &FvHandle);
  if (!EFI_ERROR (Status)) {
    NameGuid = EfiGetNameGuidFromFwVolDevicePathNode ((CONST MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *)DevicePath);
    if (NameGuid != NULL) {
      return CompareGuid (NameGuid, PcdGetPtr (PcdBootManagerMenuFile));
    }
  }

  return FALSE;
}

/**
  Report status code with EFI_RETURN_STATUS_EXTENDED_DATA about LoadImage() or
  StartImage() failure.

  @param[in] ErrorCode      An Error Code in the Software Class, DXE Boot
                            Service Driver Subclass. ErrorCode will be used to
                            compose the Value parameter for status code
                            reporting. Must be one of
                            EFI_SW_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR and
                            EFI_SW_DXE_BS_EC_BOOT_OPTION_FAILED.

  @param[in] FailureStatus  The failure status returned by the boot service
                            that should be reported.
**/
VOID
BmReportLoadFailure (
  IN UINT32      ErrorCode,
  IN EFI_STATUS  FailureStatus
  )
{
  EFI_RETURN_STATUS_EXTENDED_DATA  ExtendedData;

  if (!ReportErrorCodeEnabled ()) {
    return;
  }

  ASSERT (
    (ErrorCode == EFI_SW_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR) ||
    (ErrorCode == EFI_SW_DXE_BS_EC_BOOT_OPTION_FAILED)
    );

  ZeroMem (&ExtendedData, sizeof (ExtendedData));
  ExtendedData.ReturnStatus = FailureStatus;

  REPORT_STATUS_CODE_EX (
    (EFI_ERROR_CODE | EFI_ERROR_MINOR),
    (EFI_SOFTWARE_DXE_BS_DRIVER | ErrorCode),
    0,
    NULL,
    NULL,
    &ExtendedData.DataHeader + 1,
    sizeof (ExtendedData) - sizeof (ExtendedData.DataHeader)
    );
}

VOID
RetryBootOptions (
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  IN UINT16                              PxeRetryTimes
  )
{
  UINTN                         BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  UINTN                         Index;
  BOOLEAN                       FindFirstBootOption = FALSE;
  UINT16                        TriedTimes          = 0;
  UINTN                         FirstOptionIndex = 0;
  UINTN                         LoopTimes           = 0;

  if (BootOption == NULL) {
    return;
  }

  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
  if (BootOptionCount == 0) {
    return;
  }

  gST->ConOut->ClearScreen (gST->ConOut);
  do {
    Index = 0;
    while (TRUE) {
      //
      // First loop must start from selected boot option
      //
      if (Index == BootOptionCount) {
        Index = 0;
      }

      if ((BootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
        Index++;
        continue;
      }

      if (FindFirstBootOption) {
        if (Index == FirstOptionIndex) {
          FindFirstBootOption = FALSE;
          break;
        }
      }

      if (!FindFirstBootOption) {
        if (BootOption->OptionNumber == BootOptions[Index].OptionNumber) {
          FindFirstBootOption = TRUE;
          FirstOptionIndex    = Index;
          EfiBootManagerBoot (&BootOptions[Index]);
        }

        Index++;
      } else {
        if (BootOption->OptionNumber != BootOptions[Index].OptionNumber) {
          EfiBootManagerBoot (&BootOptions[Index]);
          Index++;
        }
      }
    }

    if (PxeRetryTimes != PXE_RETRY_FOREVER_VALUE) {
      //  0xFF:retry forever.
      TriedTimes++;
    }
    LoopTimes++;
    DEBUG ((DEBUG_INFO, "[BDS]Booted UEFI GroupType:0x%x %d times %a\n", TYPE_UEFI_PXE, LoopTimes, __FUNCTION__));
  } while ((TriedTimes < (PxeRetryTimes + 1)) || (PxeRetryTimes == PXE_RETRY_FOREVER_VALUE));

  gRetryOptionClassEntered = FALSE;
  PcdSetBoolS (PcdPxeRetriedFlag, TRUE);
  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
}

typedef
VOID
(EFIAPI *HOOK_FUNCTION)(
  VOID
  );

STATIC EFI_STATUS
InvokeHookProtocol (
  EFI_BOOT_SERVICES  *BS,
  EFI_GUID           *Protocol
  )
{
  UINTN          HandleCount;
  EFI_HANDLE     *Handles = NULL;
  EFI_STATUS     Status;
  UINTN          Index;
  HOOK_FUNCTION  MyHook;

  Status = BS->LocateHandleBuffer (
                 ByProtocol,
                 Protocol,
                 NULL,
                 &HandleCount,
                 &Handles
                 );
  if (EFI_ERROR (Status) || (HandleCount == 0)) {
    goto ProcExit;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = BS->HandleProtocol (
                   Handles[Index],
                   Protocol,
                   (VOID **)&MyHook
                   );
    ASSERT (!EFI_ERROR (Status));
    MyHook ();
  }

ProcExit:
  if (Handles != NULL) {
    BS->FreePool (Handles);
  }

  return Status;
}

VOID
DisableFrb2WatchDog (
  VOID
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiFrb2WatchDogNotifyGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->UninstallProtocolInterface (
                  Handle,
                  &gEfiFrb2WatchDogNotifyGuid,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
}

/**
  Check whether the input file path requires to install LoadFile protocol for network boot option.

  First is to check whether the input file path is network device path.
  Next is to check whether LoadFile protocol has been installed.

  @param  FilePath    Boot option device path

  @retval TRUE        Require to insall LoadFile protocol.
  @retval FALSE       Not network device path or LoadFile protocol has been installed.
**/
BOOLEAN
RequireNetworkLoadFile (
  EFI_DEVICE_PATH_PROTOCOL  *FilePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_HANDLE                Handle;
  EFI_STATUS                Status;

  DevicePath = FilePath;
  while (!IsDevicePathEnd (DevicePath) &&
         ((DevicePathType (DevicePath) != MESSAGING_DEVICE_PATH) ||
          (DevicePathSubType (DevicePath) != MSG_MAC_ADDR_DP))
         )
  {
    DevicePath = NextDevicePathNode (DevicePath);
  }

  if (IsDevicePathEnd (DevicePath)) {
    //
    // It is not network boot option
    //
    return FALSE;
  }

  //
  // Check LoadFile is installed or not
  //
  DevicePath = FilePath;
  Status     = gBS->LocateDevicePath (&gEfiLoadFileProtocolGuid, &DevicePath, &Handle);
  if (EFI_ERROR (Status)) {
    //
    // LoadFile is not installed on the input device path
    //
    return TRUE;
  }

  return FALSE;
}

/**
  Attempt to boot the EFI boot option. This routine sets L"BootCurent" and
  also signals the EFI ready to boot event. If the device path for the option
  starts with a BBS device path a legacy boot is attempted via the registered
  gLegacyBoot function. Short form device paths are also supported via this
  rountine. A device path starting with MEDIA_HARDDRIVE_DP, MSG_USB_WWID_DP,
  MSG_USB_CLASS_DP gets expaned out to find the first device that matches.
  If the BootOption Device Path fails the removable media boot algorithm
  is attempted (\EFI\BOOTIA32.EFI, \EFI\BOOTX64.EFI,... only one file type
  is tried per processor type)

  @param  BootOption    Boot Option to try and boot.
                        On return, BootOption->Status contains the boot status.
                        EFI_SUCCESS     BootOption was booted
                        EFI_UNSUPPORTED A BBS device path was found with no valid callback
                                        registered via EfiBootManagerInitialize().
                        EFI_NOT_FOUND   The BootOption was not found on the system
                        !EFI_SUCCESS    BootOption failed with this error status

**/
VOID
EFIAPI
EfiBootManagerBootFunc (
  IN  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            ImageHandle;
  EFI_LOADED_IMAGE_PROTOCOL             *ImageInfo;
  UINT16                                Uint16;
  UINTN                                 OptionNumber;
  UINTN                                 OriginalOptionNumber;
  EFI_DEVICE_PATH_PROTOCOL              *FilePath;
  EFI_DEVICE_PATH_PROTOCOL              *RamDiskDevicePath;
  VOID                                  *FileBuffer;
  UINTN                                 FileSize;
  EFI_BOOT_LOGO_PROTOCOL                *BootLogo;
  EFI_EVENT                             LegacyBootEvent;
  EFI_POST_LOGIN_PROTOCOL               *PostLogin;
  BOOLEAN                               BootSetup = FALSE;
  EFI_STATUS                            LocateStatus;
  EDKII_PLATFORM_BOOT_MANAGER_PROTOCOL  *PlatformBootManager = NULL;
  EDKII_LEGACY_BOOT_MANAGER_PROTOCOL    *LegacyBootManager   = NULL;
  UINT32                                BootPolicyStatus;
  BOOLEAN                               IsBootManagerMenu         = FALSE;
  BOOLEAN                               NeedReadyToBoot           = FALSE;
  BOOLEAN                               NeedPostLogin             = FALSE;
  BOOLEAN                               PXERetryLimitEnable       = FALSE;
  BOOLEAN                               PxeLoopRetry              = FALSE;
  UINT16                                PxeRetryTimes             = PXE_RETRY_FOREVER_VALUE;
  BYO_DISPLAY_BACKUP_RECOVER_PROTOCOL   *DisplayBackupRecover     = NULL;
  BACKUP_RECOVER_DATA                   *Data                     = NULL;
  BOOLEAN                               IsNetworkLoadFileRequried = FALSE;
  EFI_HANDLE                            Handle;
  BOOT_RETRY_POLICY_VARIABLE            BootRetryPolicy;
  UINTN                                 Size;

  if (BootOption == NULL) {
    return;
  }

  if ((BootOption->FilePath == NULL) || (BootOption->OptionType != LoadOptionTypeBoot)) {
    BootOption->Status = EFI_INVALID_PARAMETER;
    return;
  }

  //
  // 1. Create Boot#### for a temporary boot if there is no match Boot#### (i.e. a boot by selected a EFI Shell using "Boot From File")
  //
  OptionNumber = BmFindBootOptionInVariable (BootOption);
  if (OptionNumber == LoadOptionNumberUnassigned) {
    Status = BmGetFreeOptionNumber (LoadOptionTypeBoot, &Uint16);
    if (!EFI_ERROR (Status)) {
      //
      // Save the BootOption->OptionNumber to restore later
      //
      OptionNumber             = Uint16;
      OriginalOptionNumber     = BootOption->OptionNumber;
      BootOption->OptionNumber = OptionNumber;
      Status                   = EfiBootManagerLoadOptionToVariable (BootOption);
      BootOption->OptionNumber = OriginalOptionNumber;
    }

    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[Bds] Failed to create Boot#### for a temporary boot - %r!\n", Status));
      BootOption->Status = Status;
      return;
    }
  }

  //
  // Locate gEdkiiPlatformBootManagerProtocolGuid to get booting policy
  //
  LocateStatus = gBS->LocateProtocol (
                        &gEdkiiPlatformBootManagerProtocolGuid,
                        NULL,
                        (VOID **)&PlatformBootManager
                        );
  if ((!EFI_ERROR (LocateStatus)) && (PlatformBootManager->BootPolicy != NULL)) {
    LocateStatus = PlatformBootManager->BootPolicy (BootOption, &BootPolicyStatus); // boot policy of Platform
    if (!EFI_ERROR (LocateStatus)) {
      BootSetup           = (BootPolicyStatus & BOOT_POLICY_IS_SETUP) != 0 ? TRUE : FALSE;
      IsBootManagerMenu   = (BootPolicyStatus & BOOT_POLICY_IS_BOOT_MANAGER_MENU) != 0 ? TRUE : FALSE;
      NeedReadyToBoot     = (BootPolicyStatus & BOOT_POLICY_NEED_READY_TO_BOOT) != 0 ? TRUE : FALSE;
      NeedPostLogin       = (BootPolicyStatus & BOOT_POLICY_NEED_POST_LOGIN) != 0 ? TRUE : FALSE;
      PxeLoopRetry        = (BootPolicyStatus & BOOT_POLICY_PXE_RETRY) != 0 ? TRUE : FALSE;
      PXERetryLimitEnable = (BootPolicyStatus & BOOT_POLICY_PXE_RETRY_TIMES_ENABLE) != 0 ? TRUE : FALSE;
      if (PXERetryLimitEnable) {
        PxeRetryTimes = GET_PXE_RETRY_TIMES (BootPolicyStatus);
      }
    }
  } else {
    LocateStatus = EFI_NOT_FOUND;
  }

  if (PcdGetBool(PcdIsAutoBootFromBootBootOptions)) {
    //
    // Adjust PxeRetryTimes when GroupRetryTime is not 0 and GET_PXE_RETRY_TIMES (BootPolicyStatus) is 0xFF.
    //
    Size   = sizeof (BOOT_RETRY_POLICY_VARIABLE);
    Status = gRT->GetVariable (
                    BOOT_RETRY_POLICY_VAR_NAME,
                    &gBootRetryPolicyVariableGuid,
                    NULL,
                    &Size,
                    &BootRetryPolicy
                    );
    if (EFI_ERROR (Status)) {
      BootRetryPolicy.GroupRetryTime = 0;
      BootRetryPolicy.AllRetryTime   = 0;
    }
    if (!PcdGetBool(PcdAllRetryEndFlag) && (BootRetryPolicy.GroupRetryTime != 0) && (PxeRetryTimes == PXE_RETRY_FOREVER_VALUE)) {
      PxeRetryTimes = BootRetryPolicy.GroupRetryTime - 1;
    } else if (!PcdGetBool(PcdAllRetryEndFlag) && (BootRetryPolicy.AllRetryTime > 1) && (PxeRetryTimes == PXE_RETRY_FOREVER_VALUE)) {
      PxeRetryTimes = 0;
    }
  } else if (PcdGetBool(PcdAllRetryEndFlag) && ((PxeRetryTimes != PXE_RETRY_FOREVER_VALUE) || !PxeLoopRetry)) {
    //
    // when it's not PXE retrying forever and all bootoptions have booted.
    //
    FilePath = BootOption->FilePath;
    while(!IsDevicePathEnd(FilePath) && ((DevicePathType (FilePath) != MESSAGING_DEVICE_PATH)
      || (DevicePathSubType (FilePath) != MSG_MAC_ADDR_DP))) {
      FilePath = NextDevicePathNode (FilePath);
    }
    if (!IsDevicePathEnd(FilePath)) { // is Uefi net option
      DEBUG ((DEBUG_ERROR, "[Bds] All boot options have booted, just return!\n"));
      return;
    }
  }


  //
  // Process retrying policy when needed
  //
  if (PxeLoopRetry && !gRetryOptionClassEntered) {
    gRetryOptionClassEntered = TRUE;
    RetryBootOptions (BootOption, PxeRetryTimes);
    BootOption->Status = EFI_NOT_FOUND;
    return;
  }

  if (gRetryOptionClassEntered && !PxeLoopRetry) {
    BootOption->Status = EFI_NOT_FOUND;
    return;
  }

  if (EFI_ERROR (LocateStatus)) {
    //
    // Setting the default boot policy
    //
    IsBootManagerMenu = BmIsBootManagerMenuFilePath (BootOption->FilePath);
    if (IsBootManagerMenu) {
      BootSetup = TRUE;
    }

    NeedReadyToBoot = (BootOption->Attributes & LOAD_OPTION_CATEGORY_APP) == 0 ? TRUE : FALSE;
    NeedPostLogin   = TRUE;
  }

  //
  // 2. Set BootCurrent
  //
  Uint16 = (UINT16)OptionNumber;
  BmSetVariableAndReportStatusCodeOnError (
    L"BootCurrent",
    &gEfiGlobalVariableGuid,
    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
    sizeof (UINT16),
    &Uint16
    );

  if (NeedPostLogin) {
    //
    // Enter password
    //
    Status = gBS->LocateProtocol (&gEfiPostLoginProtocolGuid, NULL, (VOID **)&PostLogin);
    if (!EFI_ERROR (Status)) {
      PostLogin->CheckSysPassword (BootSetup);
    }
  }

  //
  // 3. Signal the EVT_SIGNAL_READY_TO_BOOT event when we are about to load and execute
  //    the boot option.
  //
  if (IsBootManagerMenu) {
    DEBUG ((DEBUG_INFO, "[Bds] Booting Boot Manager Menu.\n"));
    BmStopHotkeyService (NULL, NULL);
  } else {
    if (NeedReadyToBoot) {
      Status = gBS->LocateProtocol (&gDisplayBackupRecoverProtocolGuid, NULL, (VOID **)&DisplayBackupRecover);
      if (!EFI_ERROR (Status)) {
        //
        // Backup Screen
        //
        DisplayBackupRecover->DispalyBackup (&Data);
      }

      gST->ConOut->ClearScreen (gST->ConOut);
      //
      // Set Logo status invalid after clear screen
      //
      BootLogo = NULL;
      Status   = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, (VOID **)&BootLogo);
      if (!EFI_ERROR (Status) && (BootLogo != NULL)) {
        Status = BootLogo->SetBootLogo (BootLogo, NULL, 0, 0, 0, 0);
        ASSERT_EFI_ERROR (Status);
      }

      //
      // Trig ReadyToBoot Hook Function
      //
      InvokeHookProtocol (gBS, &gEfiReadyToBootProtocolGuid);

      EfiSignalEventReadyToBoot ();
      //
      // Report Status Code to indicate ReadyToBoot was signalled
      //
      REPORT_STATUS_CODE (EFI_PROGRESS_CODE, (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_READY_TO_BOOT_EVENT));
    } else {
      Handle = NULL;
      Status = gBS->InstallProtocolInterface (
                      &Handle,
                      &gEfiBootMenuEnterGuid,
                      EFI_NATIVE_INTERFACE,
                      NULL
                      );
    }

    //
    // 4. Repair system through DriverHealth protocol
    //
    BmRepairAllControllers (0);
  }

  PERF_START_EX (gImageHandle, "BdsAttempt", NULL, 0, (UINT32)OptionNumber);

  //
  // 5. Adjust the different type memory page number just before booting
  //    and save the updated info into the variable for next boot to use
  //
  BmSetMemoryTypeInformationVariable (
    (BOOLEAN)((BootOption->Attributes & LOAD_OPTION_CATEGORY) == LOAD_OPTION_CATEGORY_BOOT)
    );

  //
  // 6. Load EFI boot option to ImageHandle
  //
  DEBUG_CODE_BEGIN ();
  if (BootOption->Description == NULL) {
    DEBUG ((DEBUG_INFO | DEBUG_LOAD | FixedPcdGet32 (PcdOemDebugPrintErrorLevel), "[Bds]Booting from unknown device path\n"));
  } else {
    DEBUG ((DEBUG_INFO | DEBUG_LOAD | FixedPcdGet32 (PcdOemDebugPrintErrorLevel), "[Bds]Booting %s\n", BootOption->Description));
  }

  DEBUG_CODE_END ();

  ImageHandle       = NULL;
  RamDiskDevicePath = NULL;
  if (DevicePathType (BootOption->FilePath) != BBS_DEVICE_PATH) {
    REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderLoad));

    Status   = EFI_NOT_FOUND;
    FilePath = NULL;
    //
    // Check whether LoadFile is installed for Network Boot Option (PXE or HTTP)
    //
    IsNetworkLoadFileRequried = RequireNetworkLoadFile (BootOption->FilePath);
    EfiBootManagerConnectDevicePath (BootOption->FilePath, NULL);
    if (IsNetworkLoadFileRequried) {
      //
      // If LoadFile is first installed for Network Boot Option (PXE or HTTP), needs to wait 5s for network service ready
      //
      if (RequireNetworkLoadFile (BootOption->FilePath) == FALSE) {
        gBS->Stall (5000000);
      }
    }

    FileBuffer = BmGetNextLoadOptionBuffer (LoadOptionTypeBoot, BootOption->FilePath, &FilePath, &FileSize);
    if (FileBuffer != NULL) {
      RamDiskDevicePath = BmGetRamDiskDevicePath (FilePath);

      Status = gBS->LoadImage (
                      TRUE,
                      gImageHandle,
                      FilePath,
                      FileBuffer,
                      FileSize,
                      &ImageHandle
                      );
    }

    if (FileBuffer != NULL) {
      FreePool (FileBuffer);
    }

    if (FilePath != NULL) {
      FreePool (FilePath);
    }

    if (EFI_ERROR (Status)) {
      //
      // Disable Frb2 Watch Dog when load image is failed
      //
      DisableFrb2WatchDog ();

      //
      // With EFI_SECURITY_VIOLATION retval, the Image was loaded and an ImageHandle was created
      // with a valid EFI_LOADED_IMAGE_PROTOCOL, but the image can not be started right now.
      // If the caller doesn't have the option to defer the execution of an image, we should
      // unload image for the EFI_SECURITY_VIOLATION to avoid resource leak.
      //
      if (Status == EFI_SECURITY_VIOLATION) {
        gBS->UnloadImage (ImageHandle);
      }

      //
      // Destroy the RAM disk
      //
      if (RamDiskDevicePath != NULL) {
        BmDestroyRamDisk (RamDiskDevicePath);
        FreePool (RamDiskDevicePath);
      }

      //
      // Report Status Code with the failure status to indicate that the failure to load boot option
      //
      BmReportLoadFailure (EFI_SW_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR, Status);
      BootOption->Status = Status;
      if (Data != NULL) {
        FreePool (Data);
      }

      return;
    }
  }

  //
  // Check to see if we should legacy BOOT. If yes then do the legacy boot
  // Write boot to OS performance data for Legacy boot
  //
  if ((DevicePathType (BootOption->FilePath) == BBS_DEVICE_PATH) && (DevicePathSubType (BootOption->FilePath) == BBS_BBS_DP)) {
    if (mBmLegacyBoot == NULL) {
      //
      // Locate Legacy Boot Options Protocol
      //
      Status = gBS->LocateProtocol (
                      &gEdkiiLegacyBootManagerProtocolGuid,
                      NULL,
                      (VOID **)&LegacyBootManager
                      );
      if (!EFI_ERROR (Status)) {
        mBmLegacyBoot = (EFI_BOOT_MANAGER_LEGACY_BOOT)LegacyBootManager->LegacyBoot;
      }
    }

    if (mBmLegacyBoot != NULL) {
      //
      // Write boot to OS performance data for legacy boot.
      //
      PERF_CODE (
        //
        // Create an event to be signalled when Legacy Boot occurs to write performance data.
        //
        Status = EfiCreateEventLegacyBootEx (
                   TPL_NOTIFY,
                   BmEndOfBdsPerfCode,
                   NULL,
                   &LegacyBootEvent
                   );
        ASSERT_EFI_ERROR (Status);
        );

      mBmLegacyBoot (BootOption);
    } else {
      BootOption->Status = EFI_UNSUPPORTED;
    }

    //
    // Disable Frb2 Watch Dog when Legacy Boot exit
    //
    DisableFrb2WatchDog ();

    PERF_END_EX (gImageHandle, "BdsAttempt", NULL, 0, (UINT32)OptionNumber);
    if (Data != NULL) {
      FreePool (Data);
    }

    return;
  }

  //
  // Provide the image with its load options
  //
  Status = gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **)&ImageInfo);
  ASSERT_EFI_ERROR (Status);

  if (!BmIsAutoCreateBootOption (BootOption)) {
    ImageInfo->LoadOptionsSize = BootOption->OptionalDataSize;
    ImageInfo->LoadOptions     = BootOption->OptionalData;
  }

  //
  // Clean to NULL because the image is loaded directly from the firmwares boot manager.
  //
  ImageInfo->ParentHandle = NULL;

  //
  // Before calling the image, enable the Watchdog Timer for 5 minutes period
  //
  gBS->SetWatchdogTimer ((UINTN)PcdGet64 (PcdWatchdogTimeoutValueInSeconds), 0x0000, 0x00, NULL);

  //
  // Write boot to OS performance data for UEFI boot
  //
  PERF_CODE (
    BmEndOfBdsPerfCode (NULL, NULL);
    );

  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderStart));

  Status = gBS->StartImage (ImageHandle, &BootOption->ExitDataSize, &BootOption->ExitData);
  DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Image Return Status = %r\n", Status));
  BootOption->Status = Status;

  //
  // Destroy the RAM disk
  //
  if (RamDiskDevicePath != NULL) {
    BmDestroyRamDisk (RamDiskDevicePath);
    FreePool (RamDiskDevicePath);
  }

  if (EFI_ERROR (Status)) {
    //
    // Report Status Code with the failure status to indicate that boot failure
    //
    BmReportLoadFailure (EFI_SW_DXE_BS_EC_BOOT_OPTION_FAILED, Status);
  }

  PERF_END_EX (gImageHandle, "BdsAttempt", NULL, 0, (UINT32)OptionNumber);

  //
  // Clear the Watchdog Timer after the image returns
  //
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);

  //
  // Disable Frb2 Watch Dog after the image returns
  //
  DisableFrb2WatchDog ();

  //
  // Clear Boot Current
  //
  Status = gRT->SetVariable (
                  L"BootCurrent",
                  &gEfiGlobalVariableGuid,
                  0,
                  0,
                  NULL
                  );
  //
  // Deleting variable with current variable implementation shouldn't fail.
  // When BootXXXX (e.g.: BootManagerMenu) boots BootYYYY, exiting BootYYYY causes BootCurrent deleted,
  // exiting BootXXXX causes deleting BootCurrent returns EFI_NOT_FOUND.
  //
  ASSERT (Status == EFI_SUCCESS || Status == EFI_NOT_FOUND);

  if (Data != NULL) {
    //
    // Restore screen
    //
    DisplayBackupRecover->DispalyRecoverEx (Data, BYO_DISPLAY_RECOVER_ATTRIBUTE_NO_VIDEO);
    FreePool (Data);
  }
}

VOID
EFIAPI
EfiBootManagerBoot (
  IN  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  )
{
  EFI_STATUS                    Status;

  if(mBootMgrProtocol == NULL) {
    Status = gBS->LocateProtocol(
                  &gEdkiiUefiBootManagerProtocolGuid,
                  NULL,
                  (VOID**)&mBootMgrProtocol
                  );
    ASSERT_EFI_ERROR(Status);
  }

  mBootMgrProtocol->UefiBoot(BootOption);
  return ;
}

/**
  Check whether there is a instance in BlockIoDevicePath, which contain multi device path
  instances, has the same partition node with HardDriveDevicePath device path

  @param  BlockIoDevicePath      Multi device path instances which need to check
  @param  HardDriveDevicePath    A device path which starts with a hard drive media
                                 device path.

  @retval TRUE                   There is a matched device path instance.
  @retval FALSE                  There is no matched device path instance.

**/
BOOLEAN
BmMatchPartitionDevicePathNode (
  IN  EFI_DEVICE_PATH_PROTOCOL  *BlockIoDevicePath,
  IN  HARDDRIVE_DEVICE_PATH     *HardDriveDevicePath
  )
{
  HARDDRIVE_DEVICE_PATH  *Node;

  if ((BlockIoDevicePath == NULL) || (HardDriveDevicePath == NULL)) {
    return FALSE;
  }

  //
  // Match all the partition device path nodes including the nested partition nodes
  //
  while (!IsDevicePathEnd (BlockIoDevicePath)) {
    if ((DevicePathType (BlockIoDevicePath) == MEDIA_DEVICE_PATH) &&
        (DevicePathSubType (BlockIoDevicePath) == MEDIA_HARDDRIVE_DP)
        )
    {
      //
      // See if the harddrive device path in blockio matches the orig Hard Drive Node
      //
      Node = (HARDDRIVE_DEVICE_PATH *)BlockIoDevicePath;

      //
      // Match Signature and PartitionNumber.
      // Unused bytes in Signature are initiaized with zeros.
      //
      if ((Node->PartitionNumber == HardDriveDevicePath->PartitionNumber) &&
          (Node->MBRType == HardDriveDevicePath->MBRType) &&
          (Node->SignatureType == HardDriveDevicePath->SignatureType) &&
          (CompareMem (Node->Signature, HardDriveDevicePath->Signature, sizeof (Node->Signature)) == 0))
      {
        return TRUE;
      }
    }

    BlockIoDevicePath = NextDevicePathNode (BlockIoDevicePath);
  }

  return FALSE;
}

/**
  Emuerate all possible bootable medias in the following order:
  1. Removable BlockIo            - The boot option only points to the removable media
                                    device, like USB key, DVD, Floppy etc.
  2. Fixed BlockIo                - The boot option only points to a Fixed blockIo device,
                                    like HardDisk.
  3. Non-BlockIo SimpleFileSystem - The boot option points to a device supporting
                                    SimpleFileSystem Protocol, but not supporting BlockIo
                                    protocol.
  4. LoadFile                     - The boot option points to the media supporting
                                    LoadFile protocol.
  Reference: UEFI Spec chapter 3.3 Boot Option Variables Default Boot Behavior

  @param BootOptionCount   Return the boot option count which has been found.

  @retval   Pointer to the boot option array.
**/
EFI_BOOT_MANAGER_LOAD_OPTION *
BmEnumerateBootOptions (
  UINTN  *BootOptionCount
  )
{
  EFI_STATUS                    Status;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  UINTN                         HandleCount;
  EFI_HANDLE                    *Handles;
  EFI_BLOCK_IO_PROTOCOL         *BlkIo;
  UINTN                         Removable;
  UINTN                         Index;
  CHAR16                        *Description;
  EFI_STRING                    TempDescription;
  UINTN                         MaxDescriptionLen;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath = NULL;

  MaxDescriptionLen = MAX_DESCRIPTION_STR_LEN - 3;

  ASSERT (BootOptionCount != NULL);

  *BootOptionCount = 0;
  BootOptions      = NULL;

  //
  // Parse removable block io followed by fixed block io
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiBlockIoProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );

  for (Removable = 0; Removable < 2; Removable++) {
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      Handles[Index],
                      &gEfiBlockIoProtocolGuid,
                      (VOID **)&BlkIo
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }

      //
      // Skip the logical partitions
      //
      if (BlkIo->Media->LogicalPartition) {
        continue;
      }

      //
      // Skip the fixed block io then the removable block io
      //
      if (BlkIo->Media->RemovableMedia == ((Removable == 0) ? FALSE : TRUE)) {
        continue;
      }

      Description = BmGetBootDescription (Handles[Index]);
      if (StrLen (Description) > MaxDescriptionLen) {
        TempDescription = AllocateZeroPool ((MAX_DESCRIPTION_STR_LEN + 1) * sizeof (CHAR16));
        CopyMem (TempDescription, Description, MaxDescriptionLen * sizeof (CHAR16));
        CopyMem (TempDescription + MaxDescriptionLen, L"...", 4 * sizeof (CHAR16));
        FreePool (Description);
        Description = TempDescription;
      }

      BootOptions = ReallocatePool (
                      sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount),
                      sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount + 1),
                      BootOptions
                      );
      ASSERT (BootOptions != NULL);

      Status = EfiBootManagerInitializeLoadOption (
                 &BootOptions[(*BootOptionCount)++],
                 LoadOptionNumberUnassigned,
                 LoadOptionTypeBoot,
                 LOAD_OPTION_ACTIVE,
                 Description,
                 DevicePathFromHandle (Handles[Index]),
                 NULL,
                 0
                 );
      ASSERT_EFI_ERROR (Status);

      FreePool (Description);
    }
  }

  if (HandleCount != 0) {
    FreePool (Handles);
  }

  //
  // Parse simple file system not based on block io
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiSimpleFileSystemProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **)&BlkIo
                    );
    if (!EFI_ERROR (Status)) {
      //
      //  Skip if the file system handle supports a BlkIo protocol, which we've handled in above
      //
      continue;
    }

    Description = BmGetBootDescription (Handles[Index]);
    BootOptions = ReallocatePool (
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount),
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount + 1),
                    BootOptions
                    );
    ASSERT (BootOptions != NULL);

    Status = EfiBootManagerInitializeLoadOption (
               &BootOptions[(*BootOptionCount)++],
               LoadOptionNumberUnassigned,
               LoadOptionTypeBoot,
               LOAD_OPTION_ACTIVE,
               Description,
               DevicePathFromHandle (Handles[Index]),
               NULL,
               0
               );
    ASSERT_EFI_ERROR (Status);
    FreePool (Description);
  }

  if (HandleCount != 0) {
    FreePool (Handles);
  }

  //
  // Parse load file protocol
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiLoadFileProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  for (Index = 0; Index < HandleCount; Index++) {
    //
    // Ignore BootManagerMenu. its boot option will be created by EfiBootManagerGetBootManagerMenu().
    //
    if (BmIsBootManagerMenuFilePath (DevicePathFromHandle (Handles[Index]))) {
      continue;
    }

    //
    // Filter out PXE handles which doesn't have Ip node
    //
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DevicePath
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    while (!IsDevicePathEnd (DevicePath)) {
      if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) &&
          ((DevicePathSubType (DevicePath) == MSG_IPv4_DP) ||
           (DevicePathSubType (DevicePath) == MSG_IPv6_DP) ||
           (DevicePathSubType (DevicePath) == MSG_URI_DP)))
      {
        break;
      }

      DevicePath = NextDevicePathNode (DevicePath);
    }

    if (IsDevicePathEnd (DevicePath)) {
      continue;
    }

    Description = BmGetBootDescription (Handles[Index]);
    BootOptions = ReallocatePool (
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount),
                    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * (*BootOptionCount + 1),
                    BootOptions
                    );
    ASSERT (BootOptions != NULL);

    Status = EfiBootManagerInitializeLoadOption (
               &BootOptions[(*BootOptionCount)++],
               LoadOptionNumberUnassigned,
               LoadOptionTypeBoot,
               LOAD_OPTION_ACTIVE,
               Description,
               DevicePathFromHandle (Handles[Index]),
               NULL,
               0
               );
    ASSERT_EFI_ERROR (Status);
    FreePool (Description);
  }

  if (HandleCount != 0) {
    FreePool (Handles);
  }

  BmMakeBootOptionDescriptionUnique (BootOptions, *BootOptionCount);
  return BootOptions;
}

/**
  1) HD Short-Form: return the fullpath of the DevicePath;
  2) non HD Short-Form: return DevicePath,
**/
EFI_DEVICE_PATH_PROTOCOL *
BmExpandHdShortForm (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath,
  IN OUT UINTN                 *Size
  )
{
  UINTN                     TempSize;
  EFI_DEVICE_PATH_PROTOCOL  *TempDp;

  TempDp   = DevicePath;
  TempSize = GetDevicePathSize (TempDp);
  if ((DevicePathType (TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType (TempDp) == MEDIA_HARDDRIVE_DP)) {
    TempDp = NextDevicePathNode (TempDp);
    while (!IsDevicePathEnd (TempDp)) {
      if ((DevicePathType (TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType (TempDp) == MEDIA_FILEPATH_DP)) {
        break;
      }

      TempDp = NextDevicePathNode (TempDp);
    }

    if (!IsDevicePathEnd (TempDp)) {
      TempDp = BmGetNextLoadOptionDevicePath (DevicePath, NULL); // Try to get FullPath of HD Short-Form
      if (TempDp != NULL) {
        *Size = GetDevicePathSize (TempDp) - TempSize;
        return TempDp;
      }
    }
  }

  *Size = GetDevicePathSize (DevicePath) - END_DEVICE_PATH_LENGTH;
  return DevicePath;
}

/**
  Gets the URI device path when it is the last node.

  @param DevicePath    A pointer to the first device path data structure.

  @retval NULL    No URI device path node or it is not the last node.
  @retval other   The URI device path.

**/
URI_DEVICE_PATH *
GetUriDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *NextDevicePath;

  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == MESSAGING_DEVICE_PATH) && (DevicePathSubType (DevicePath) == MSG_URI_DP)) {
      NextDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)NextDevicePathNode (DevicePath);
      if (IsDevicePathEnd (NextDevicePath)) {
        return (URI_DEVICE_PATH *)DevicePath;
      }
    }

    DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)NextDevicePathNode (DevicePath);
  }

  return NULL;
}

/**
  Compare two device pathes to check if they are exactly same.

  @param DevicePath1    A pointer to the first device path data structure.
  @param DevicePath2    A pointer to the second device path data structure.

  @retval TRUE    They are same.
  @retval FALSE   They are not same.

**/
BOOLEAN
BmCompareDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath1,
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath2
  )
{
  UINTN                     Size1;
  UINTN                     Size2;
  UINTN                     UriStrLen1;
  UINTN                     UriStrLen2;
  EFI_DEVICE_PATH_PROTOCOL  *ExpandDp1;
  EFI_DEVICE_PATH_PROTOCOL  *ExpandDp2;
  BOOLEAN                   IsMatched = FALSE;
  URI_DEVICE_PATH           *UriDevicePath1;
  URI_DEVICE_PATH           *UriDevicePath2;
  BOOLEAN                   IsSameUri = FALSE;

  ExpandDp1 = BmExpandHdShortForm (DevicePath1, &Size1);
  ExpandDp2 = BmExpandHdShortForm (DevicePath2, &Size2);

  UriDevicePath1 = GetUriDevicePath (DevicePath1);
  UriDevicePath2 = GetUriDevicePath (DevicePath2);

  if ((UriDevicePath1 != NULL) && (UriDevicePath2 != NULL)) {
    if (DevicePathNodeLength (UriDevicePath1) == sizeof (URI_DEVICE_PATH)) {
      UriStrLen1 = 0;
    } else {
      UriStrLen1 = AsciiStrLen (UriDevicePath1->Uri);
    }

    if (DevicePathNodeLength (UriDevicePath2) == sizeof (URI_DEVICE_PATH)) {
      UriStrLen2 = 0;
    } else {
      UriStrLen2 = AsciiStrLen (UriDevicePath2->Uri);
    }

    if ((UriStrLen1 == UriStrLen2) && (CompareMem (UriDevicePath1->Uri, UriDevicePath2->Uri, UriStrLen1) == 0)) {
      IsSameUri = TRUE;
    }
  }

  if (IsSameUri) {
    Size1 = Size1 -(GetDevicePathSize ((EFI_DEVICE_PATH_PROTOCOL *)UriDevicePath1) - END_DEVICE_PATH_LENGTH);
    Size2 = Size2 -(GetDevicePathSize ((EFI_DEVICE_PATH_PROTOCOL *)UriDevicePath2) - END_DEVICE_PATH_LENGTH);
  }

  if (Size1 == Size2) {
    if (CompareMem (ExpandDp1, ExpandDp2, Size1) == 0) {
      IsMatched = TRUE;
    }
  }

  if (GetDevicePathSize (ExpandDp1) == GetDevicePathSize (ExpandDp2)) {
    if (CompareMem (ExpandDp1, ExpandDp2, GetDevicePathSize (ExpandDp1)) == 0) {
      IsMatched = TRUE;
    }
  }

  if (ExpandDp1 != NULL) {
    if (CompareMem (ExpandDp1, DevicePath1, GetDevicePathSize (DevicePath1)) != 0) {
      FreePool (ExpandDp1);
    }
  }

  if (ExpandDp2 != NULL) {
    if (CompareMem (ExpandDp2, DevicePath2, GetDevicePathSize (DevicePath2)) != 0) {
      FreePool (ExpandDp2);
    }
  }

  return IsMatched;
}

/**
   The function checks boot options for HD Short-form Device Path format added by end-user or OS installer:
   like HD(2,GPT,5F343B3C-ADE7-4F05-8FF0-6A2A0AC55B86,0xFA000,0x32000)/\EFI\Microsoft\Boot\bootmgfw.efi;
   For HD short-form and full DevicePath NV boot options,we must check for repeatability.
   1.Feature Description:
     Remove DEAD BootOption registered by OS Loader.
     OS registers BootOption which contains short-formed device path. However, due to operations like SATA device
     uninstallation/replacement, OS Re-installation etc. Such bootOption could become DEAD and should be deleted.
   2.Detection Rule:
     1) Delete DEAD BootOption:Extend short-formed device path to full device path, if extension fails, which means partition
    doesn't exist anymore, so remove it;
     2) Check duplicated boot options with Active&& Unhidden NV options(full && short-form DevicePath), and delete the option
    based on gEfiMdeModulePkgTokenSpaceGuid.PcdSetDescOfOsFirst:
        * When gEfiMdeModulePkgTokenSpaceGuid.PcdSetDescOfOsFirst is TRUE: delete the option created by BDS;
        * When gEfiMdeModulePkgTokenSpaceGuid.PcdSetDescOfOsFirst is FALSE: delete the option created by OS loader;
     3) Update boot description in NV: For options where the description is inconsistent
    with the current boot option, update its description to the current value
     4) Delete boot options whose OptionNumber is marked as DELETE_FLAG:
        When the platform marked unwanted options as DELETE_FLAG, we need to delete relative NV options.
   return TRUE: invalid HD short-form boot option, need to delete.
          FALSE: valid HD short-form boot option or it's not short-form option, no need to delete;
**/
BOOLEAN
CheckInvalidBootOptions (
  IN EFI_BOOT_MANAGER_LOAD_OPTION  *NvOption,
  EFI_BOOT_MANAGER_LOAD_OPTION     *BootOptions,
  UINTN                            BootOptionCount,
  EFI_BOOT_MANAGER_LOAD_OPTION     *NvBootOptions,
  UINTN                            NvBootOptionCount
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *NvDp;
  EFI_DEVICE_PATH_PROTOCOL  *TempDp;
  EFI_DEVICE_PATH_PROTOCOL  *OsLoaderFullPath;
  UINTN                     Index;
  CHAR16                    *NewDescription;
  BOOLEAN                   IsHddShortForm = FALSE;
  UINTN                     Size;
  UINTN                     NvSize;
  //
  // Check if NvOption had been deleted
  //
  if (NvOption->OptionNumber == DELETE_FLAG) {
    return FALSE;
  }

  //
  // Check if NvOption is HD short-form DevicePath
  //
  NvDp             = NvOption->FilePath;
  OsLoaderFullPath = NvOption->FilePath;
  if ((DevicePathType (NvDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType (NvDp) == MEDIA_HARDDRIVE_DP)) {
    TempDp = NextDevicePathNode (NvDp);
    while (!IsDevicePathEnd (TempDp)) {
      if ((DevicePathType (TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType (TempDp) == MEDIA_FILEPATH_DP)) {
        break;
      }

      TempDp = NextDevicePathNode (TempDp);
    }

    if (!IsDevicePathEnd (TempDp)) {
      DEBUG ((DEBUG_INFO, "%s is HD Short-form DevicePath\n", NvOption->Description));
      IsHddShortForm = TRUE;
      //
      // 1) Check if it's DEAD BootOption
      //
      OsLoaderFullPath = BmGetNextLoadOptionDevicePath (NvOption->FilePath, NULL);
      if (OsLoaderFullPath == NULL) {
        DEBUG ((DEBUG_INFO, "%s is DEAD OS Loader Option!Must Delete!\n", NvOption->Description));
        return TRUE;
      }
    }
  }

  //
  // skip inactive and hidden NV boot options
  //
  if (((NvOption->Attributes & LOAD_OPTION_ACTIVE) == 0) || ((NvOption->Attributes & LOAD_OPTION_HIDDEN) != 0)) {
    return FALSE;
  }

  //
  // 2) Loop boot options in NV to check if there are duplicated active&&unhidden boot options, and delete the option created by BDS
  //
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if (  (NvBootOptions[Index].OptionNumber == DELETE_FLAG) || ((NvBootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0)
       || ((NvBootOptions[Index].Attributes & LOAD_OPTION_HIDDEN) != 0))
    {
      continue;
    }

    //
    // Match to skip yourself
    //
    if (EfiBootManagerFindLoadOption (&NvBootOptions[Index], NvOption, 1) != -1) {
      continue;
    }

    if (BmCompareDevicePath (NvOption->FilePath, NvBootOptions[Index].FilePath)) {
      //
      //Two boot option are not the same if their FilePath and OptionalData are not the same
      //
      if(BmIsAutoCreateBootOption(NvOption)) {
        NvSize = 0;
      } else {
        NvSize = NvOption->OptionalDataSize;
      }
      if (BmIsAutoCreateBootOption(&NvBootOptions[Index])) {
        Size = 0;
      } else {
        Size = NvBootOptions[Index].OptionalDataSize;
      }
      if (NvSize != Size) {
        continue;
      }
      if (CompareMem(NvOption->OptionalData,NvBootOptions[Index].OptionalData,Size) != 0) {
        continue;
      }
      if (!BmIsAutoCreateBootOption (&NvBootOptions[Index])) {
        if (PcdGetBool (PcdSetDescOfOsFirst)) {
          continue;
        }
      } else {
        if (!PcdGetBool (PcdSetDescOfOsFirst)) {
          continue;
        }
      }

      Status = EfiBootManagerDeleteLoadOptionVariable (NvBootOptions[Index].OptionNumber, LoadOptionTypeBoot);
      DEBUG ((DEBUG_INFO, "%s is duplicated! Deleted %r!\n", NvBootOptions[Index].Description, Status));
      NvBootOptions[Index].OptionNumber = DELETE_FLAG;
      break;
    }
  }

  //
  // 3) Loop enumerated boot options, delete duplicated options created by BDS and update boot description in NV
  //
  for (Index = 0; Index < BootOptionCount; Index++) {
    if (BmCompareDevicePath (NvOption->FilePath, BootOptions[Index].FilePath)) {
      //
      //Two boot option are not the same if their FilePath and OptionalData are not the same
      //
      if(BmIsAutoCreateBootOption(NvOption)) {
        NvSize = 0;
      } else {
        NvSize = NvOption->OptionalDataSize;
      }
      if (BmIsAutoCreateBootOption(&BootOptions[Index])) {
        Size = 0;
      } else {
        Size = BootOptions[Index].OptionalDataSize;
      }
      if (NvSize != Size) {
        continue;
      }
      if (CompareMem(NvOption->OptionalData,BootOptions[Index].OptionalData,Size) != 0) {
        continue;
      }
      //
      // Platform will tag DELETE_FLAG to boot option to delete same NV boot option
      //
      if (BootOptions[Index].OptionNumber == DELETE_FLAG) {
        DEBUG ((DEBUG_INFO, "%s is asked to delete by the platform!\n", NvOption->Description));
        return TRUE;
      }

      if (!BmIsAutoCreateBootOption (NvOption)) {
        //
        // Back up the OptionNumber to gBootOptionOrder in order
        //
        if (gBootOptionOrder[Index] != NvOption->OptionNumber) {
          gBootOptionOrder[Index] = NvOption->OptionNumber;
        }

        //
        // If gEfiMdeModulePkgTokenSpaceGuid.PcdSetBootOptionOfOsFirst is FALSE, we need
        // to delete boot option build by OS and keep duplicated option build by BDS.
        // If it's TRUE, just "continue" to keep entire boot option build by OS.
        //
        if (PcdGetBool (PcdSetDescOfOsFirst)) {
          BootOptions[Index].OptionNumber = DELETE_FLAG;
          continue;
        } else {
          return TRUE;
        }
      }

      if (StrCmp (NvOption->Description, BootOptions[Index].Description)) {
        // update description of BDS options or Os installer(PcdSetDescOfOsFirst is FALSE).
        if (NvOption->Description != NULL) {
          FreePool (NvOption->Description);
        }

        NewDescription = AllocateZeroPool (StrSize (BootOptions[Index].Description));
        StrCpyS (NewDescription, StrSize (BootOptions[Index].Description) / sizeof (CHAR16), BootOptions[Index].Description);
        NvOption->Description = NewDescription;
        DEBUG ((DEBUG_INFO, "NV[%04X] new string: %s\n", NvOption->OptionNumber, NvOption->Description));
        EfiBootManagerLoadOptionToVariable (NvOption);
      }

      //
      // Back up the OptionNumber to gBootOptionOrder in order
      //
      if (gBootOptionOrder[Index] != NvOption->OptionNumber) {
        gBootOptionOrder[Index] = NvOption->OptionNumber;
      }

      BootOptions[Index].OptionNumber = DELETE_FLAG;
      break;
    }
  }

  if (OsLoaderFullPath != NULL) {
    if (CompareMem (OsLoaderFullPath, NvOption->FilePath, GetDevicePathSize (NvOption->FilePath)) != 0) {
      FreePool (OsLoaderFullPath);
    }
  }

  //
  // When PcdSetDescOfOsFirst is FALSE, always delete NvOption created by OS
  //
  if (IsHddShortForm && !PcdGetBool (PcdSetDescOfOsFirst) && !BmIsAutoCreateBootOption (NvOption)) {
    return TRUE;
  }

  return FALSE;
}

static
BOOLEAN
IsFilePath (
  IN  EFI_DEVICE_PATH_PROTOCOL     *DevicePath
)
{
  EFI_HANDLE                      FvHandle;
  EFI_STATUS                      Status;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;

  TempDevicePath = DevicePath;
  Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &TempDevicePath, &FvHandle);
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  return FALSE;
}

/**
  The function enumerates all boot options, creates them and registers them in the BootOrder variable.
**/
VOID
EFIAPI
EfiBootManagerRefreshAllBootOption (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_BOOT_MANAGER_LOAD_OPTION          *NvBootOptions;
  UINTN                                 NvBootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION          *BootOptions;
  UINTN                                 BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION          *UpdatedBootOptions;
  UINTN                                 UpdatedBootOptionCount;
  UINTN                                 Index;
  EDKII_PLATFORM_BOOT_MANAGER_PROTOCOL  *PlatformBootManager;
  EDKII_LEGACY_BOOT_MANAGER_PROTOCOL    *LegacyBootManager;
  BOOLEAN                               InvalidHDShortDp = FALSE;
  EFI_DEVICE_PATH_PROTOCOL              *DevicePath = NULL;
  EFI_HANDLE                            Handle;
  EFI_BLOCK_IO_PROTOCOL                 *BlkIo;
  BOOLEAN                               IsLockedHdd;
  UINT32                                IsCalled;
  INTN                                  NvIndex;
  BOOLEAN                               IsExistence;
  UINTN                                 HandleIndex;
  EFI_HANDLE                            *SimpleFileSystemHandles;
  UINTN                                 NumberSimpleFileSystemHandles = 0;
  EFI_DEVICE_PATH_PROTOCOL              *TempDevicePath;
  UINTN                                 DevicePathSize;

  //
  //Check whether EfiBootManagerRefreshAllBootOption is executed
  //If it has been executed return
  //
  IsCalled = PcdGet32(PcdBootManagerLibSetting);
  if ((IsCalled & BOOT_MANAGER_BOOT_OPTIONS) > 0) {
    return ;
  }

  //
  // Optionally refresh the legacy boot option
  //
  if (mBmRefreshLegacyBootOption == NULL) {
    //
    // Locate Platform Boot Options Protocol
    //
    Status = gBS->LocateProtocol (
                    &gEdkiiLegacyBootManagerProtocolGuid,
                    NULL,
                    (VOID **)&LegacyBootManager
                    );
    if (!EFI_ERROR (Status)) {
      mBmRefreshLegacyBootOption = LegacyBootManager->RefreshLegacyBootOption;
    }
  }

  if (mBmRefreshLegacyBootOption != NULL) {
    mBmRefreshLegacyBootOption ();
  }

  BootOptions = BmEnumerateBootOptions (&BootOptionCount);

  //
  // Locate Platform Boot Options Protocol
  //
  Status = gBS->LocateProtocol (
                  &gEdkiiPlatformBootManagerProtocolGuid,
                  NULL,
                  (VOID **)&PlatformBootManager
                  );
  if (!EFI_ERROR (Status)) {
    //
    // If found, call platform specific refresh to all auto enumerated and NV
    // boot options.
    //
    Status = PlatformBootManager->RefreshAllBootOptions (
                                    (CONST EFI_BOOT_MANAGER_LOAD_OPTION *)BootOptions,
                                    (CONST UINTN)BootOptionCount,
                                    &UpdatedBootOptions,
                                    &UpdatedBootOptionCount
                                    );
    if (!EFI_ERROR (Status)) {
      EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
      BootOptions     = UpdatedBootOptions;
      BootOptionCount = UpdatedBootOptionCount;
    }
  }

  //
  // Mark the boot option as added by BDS by setting OptionalData to a special GUID
  //
  for (Index = 0; Index < BootOptionCount; Index++) {
    BootOptions[Index].OptionalData     = AllocateCopyPool (sizeof (EFI_GUID), &mBmAutoCreateBootOptionGuid);
    BootOptions[Index].OptionalDataSize = sizeof (EFI_GUID);
  }

  //
  // Init gBootOptionOrder
  //
  if (BootOptionCount > 0) {
    gBootOptionOrder = (UINTN *)AllocatePool (BootOptionCount * sizeof (UINTN));
    ASSERT (gBootOptionOrder != NULL);
    for (Index = 0; Index < BootOptionCount; Index++) {
      gBootOptionOrder[Index] = DELETE_FLAG;
    }
  }

  NvBootOptions = EfiBootManagerGetLoadOptions (&NvBootOptionCount, LoadOptionTypeBoot);

  Status = gBS->LocateHandleBuffer (
              ByProtocol,
              &gEfiSimpleFileSystemProtocolGuid,
              NULL,
              &NumberSimpleFileSystemHandles,
              &SimpleFileSystemHandles
              );
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR,"Locate gEfiSimpleFileSystemProtocolGuid :%r",Status));
  }
  //
  // Remove invalid EFI boot options from NV
  //
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    IsLockedHdd = FALSE;
    if (((DevicePathType (NvBootOptions[Index].FilePath) != BBS_DEVICE_PATH) ||
         (DevicePathSubType (NvBootOptions[Index].FilePath) != BBS_BBS_DP)
         ) && (!BmIsBootManagerMenuFilePath (NvBootOptions[Index].FilePath))
        )
    {
      if (BmIsAutoCreateBootOption (&NvBootOptions[Index]) && (NvBootOptions[Index].OptionNumber != DELETE_FLAG)) {
        //
        // Only check those added by BDS
        // so that the boot options added by end-user or OS installer won't be deleted
        //
        if (EfiBootManagerFindLoadOption (&NvBootOptions[Index], BootOptions, BootOptionCount) == -1) {
          DevicePath = NvBootOptions[Index].FilePath;
          Status = gBS->LocateDevicePath(&gEfiDiskInfoProtocolGuid, &DevicePath, &Handle);
          if (!EFI_ERROR(Status)) {
            Status = gBS->HandleProtocol(Handle, &gEfiBlockIoProtocolGuid, (VOID **)&BlkIo);
            if (EFI_ERROR(Status)) {
              IsLockedHdd = TRUE;
            }
          }

          if (IsLockedHdd) {
            NvBootOptions[Index].Attributes = (NvBootOptions[Index].Attributes & (~LOAD_OPTION_ACTIVE)) | LOAD_OPTION_HIDDEN;
            DEBUG((EFI_D_INFO, "NV[%04X] is locked and set to inactive and hidden \n", NvBootOptions[Index].OptionNumber));
            EfiBootManagerLoadOptionToVariable (&NvBootOptions[Index]);
          } else {
            Status = EfiBootManagerDeleteLoadOptionVariable (NvBootOptions[Index].OptionNumber, LoadOptionTypeBoot);
            //
            // Deleting variable with current variable implementation shouldn't fail.
            //
            ASSERT_EFI_ERROR (Status);
            // if BootOption has been Delete in BootOrder, there is no need to check below
            if (!EFI_ERROR (Status)) {
              NvBootOptions[Index].OptionNumber =  DELETE_FLAG;
              continue;
            }
          }
        }
      }

      //
      // Delete invalid boot options, like DEAD BootOption, duplicated options or those marked as DELETE_FLAG by platform
      //
      InvalidHDShortDp = CheckInvalidBootOptions (&NvBootOptions[Index], BootOptions, BootOptionCount, NvBootOptions, NvBootOptionCount);
      if (InvalidHDShortDp) {
        Status = EfiBootManagerDeleteLoadOptionVariable (NvBootOptions[Index].OptionNumber, LoadOptionTypeBoot);
        DEBUG ((DEBUG_INFO, "Delete invalid OS Loader Option %s,Status:%r\n", NvBootOptions[Index].Description, Status));
        NvBootOptions[Index].OptionNumber = DELETE_FLAG;
        continue;
      }
      //
      //Skip Internal Shell
      //
      if (IsFilePath(NvBootOptions[Index].FilePath)) {
        continue;
      }
      //
      //Delete the boot option added manually by the user 
      //
      if ((!BmIsAutoCreateBootOption (&NvBootOptions[Index])) 
           && (NvBootOptions[Index].OptionNumber != DELETE_FLAG)
           && ((NvBootOptions[Index].Attributes & LOAD_OPTION_CATEGORY_APP) == 0)) {
        IsExistence = FALSE;
        for (HandleIndex = 0; HandleIndex < NumberSimpleFileSystemHandles; HandleIndex++) {
          TempDevicePath = DevicePathFromHandle (SimpleFileSystemHandles[HandleIndex]);
          DevicePathSize = GetDevicePathSize(TempDevicePath) > GetDevicePathSize(NvBootOptions[Index].FilePath) ? GetDevicePathSize(NvBootOptions[Index].FilePath) : GetDevicePathSize(TempDevicePath);
          DevicePathSize = DevicePathSize - sizeof(EFI_DEVICE_PATH_PROTOCOL);//skip End node
          if (CompareMem(NvBootOptions[Index].FilePath,TempDevicePath,DevicePathSize) == 0) {
            IsExistence = TRUE;
            break;
          }
        }
        if (!IsExistence) {
          Status = EfiBootManagerDeleteLoadOptionVariable (NvBootOptions[Index].OptionNumber, LoadOptionTypeBoot);
          DEBUG ((DEBUG_INFO, "Delete added manually by the user Loader Option %s,Status:%r\n", NvBootOptions[Index].Description, Status));
          NvBootOptions[Index].OptionNumber = DELETE_FLAG;
        }
      }
    }
  }

  //
  // Add new EFI boot options to NV
  //
  for (Index = 0; Index < BootOptionCount; Index++) {
    if (BootOptions[Index].OptionNumber == DELETE_FLAG) {
      continue;
    }

    NvIndex = EfiBootManagerFindLoadOption (&BootOptions[Index], NvBootOptions, NvBootOptionCount);
    if (NvIndex == -1) {
      EfiBootManagerAddLoadOptionVariable (&BootOptions[Index], (UINTN)-1);
      //
      // Try best to add the boot options so continue upon failure.
      //
    } else {
      BootOptions[Index].OptionNumber = NvBootOptions[(UINTN)NvIndex].OptionNumber;
    }
  }

  //
  // Store the refresh boot option into EFI variable
  //
  if (BootOptionCount > 0) {
    for (Index = 0; Index < BootOptionCount; Index++) {
      if (BootOptions[Index].OptionNumber != DELETE_FLAG) {
        gBootOptionOrder[Index] = BootOptions[Index].OptionNumber;
      }
    }

    Status = gRT->SetVariable (
                    L"BootOptionOrder",
                    &gEdkiiBdsVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
                    BootOptionCount * sizeof (UINTN),
                    gBootOptionOrder
                    );
  }

  if (gBootOptionOrder != NULL) {
    FreePool (gBootOptionOrder);
  }

  if (NumberSimpleFileSystemHandles > 0) {
    FreePool(SimpleFileSystemHandles );
  }

  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
  EfiBootManagerFreeLoadOptions (NvBootOptions, NvBootOptionCount);

  //
  //Set pcd when the execution is complete
  //
  PcdSet32S(PcdBootManagerLibSetting,IsCalled|BOOT_MANAGER_BOOT_OPTIONS);
}

/**
  This function is called to get or create the boot option for the Boot Manager Menu.

  The Boot Manager Menu is shown after successfully booting a boot option.
  This function will first try to search the BootManagerMenuFile is in the same FV as
  the module links to this library. If fails, it will search in all FVs.

  @param  BootOption    Return the boot option of the Boot Manager Menu

  @retval EFI_SUCCESS   Successfully register the Boot Manager Menu.
  @retval EFI_NOT_FOUND The Boot Manager Menu cannot be found.
  @retval others        Return status of gRT->SetVariable (). BootOption still points
                        to the Boot Manager Menu even the Status is not EFI_SUCCESS
                        and EFI_NOT_FOUND.
**/
EFI_STATUS
BmRegisterBootManagerMenu (
  OUT EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  )
{
  EFI_STATUS                Status;
  CHAR16                    *Description;
  UINTN                     DescriptionLength;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINTN                     HandleCount;
  EFI_HANDLE                *Handles;
  UINTN                     Index;

  DevicePath  = NULL;
  Description = NULL;
  //
  // Try to find BootManagerMenu from LoadFile protocol
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiLoadFileProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  for (Index = 0; Index < HandleCount; Index++) {
    if (BmIsBootManagerMenuFilePath (DevicePathFromHandle (Handles[Index]))) {
      DevicePath  = DuplicateDevicePath (DevicePathFromHandle (Handles[Index]));
      Description = BmGetBootDescription (Handles[Index]);
      break;
    }
  }

  if (HandleCount != 0) {
    FreePool (Handles);
  }

  if (DevicePath == NULL) {
    Status = GetFileDevicePathFromAnyFv (
               PcdGetPtr (PcdBootManagerMenuFile),
               EFI_SECTION_PE32,
               0,
               &DevicePath
               );
    if (EFI_ERROR (Status)) {
      DEBUG ((DEBUG_WARN, "[Bds]BootManagerMenu FFS section can not be found, skip its boot option registration\n"));
      return EFI_NOT_FOUND;
    }

    ASSERT (DevicePath != NULL);
    //
    // Get BootManagerMenu application's description from EFI User Interface Section.
    //
    Status = GetSectionFromAnyFv (
               PcdGetPtr (PcdBootManagerMenuFile),
               EFI_SECTION_USER_INTERFACE,
               0,
               (VOID **)&Description,
               &DescriptionLength
               );
    if (EFI_ERROR (Status)) {
      Description = NULL;
    }
  }

  Status = EfiBootManagerInitializeLoadOption (
             BootOption,
             LoadOptionNumberUnassigned,
             LoadOptionTypeBoot,
             LOAD_OPTION_CATEGORY_APP | LOAD_OPTION_ACTIVE | LOAD_OPTION_HIDDEN,
             (Description != NULL) ? Description : L"Boot Manager Menu",
             DevicePath,
             (UINT8 *)&mBmAutoCreateBootOptionGuid,
             sizeof (EFI_GUID)
             );
  ASSERT_EFI_ERROR (Status);
  FreePool (DevicePath);
  if (Description != NULL) {
    FreePool (Description);
  }

  DEBUG_CODE (
    EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions;
    UINTN                           BootOptionCount;

    BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
    ASSERT (EfiBootManagerFindLoadOption (BootOption, BootOptions, BootOptionCount) == -1);
    EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
    );

  return EfiBootManagerAddLoadOptionVariable (BootOption, (UINTN)-1);
}

/**
  Return the boot option corresponding to the Boot Manager Menu.
  It may automatically create one if the boot option hasn't been created yet.

  @param BootOption    Return the Boot Manager Menu.

  @retval EFI_SUCCESS   The Boot Manager Menu is successfully returned.
  @retval EFI_NOT_FOUND The Boot Manager Menu cannot be found.
  @retval others        Return status of gRT->SetVariable (). BootOption still points
                        to the Boot Manager Menu even the Status is not EFI_SUCCESS
                        and EFI_NOT_FOUND.
**/
EFI_STATUS
EFIAPI
EfiBootManagerGetBootManagerMenu (
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  )
{
  EFI_STATUS                    Status;
  UINTN                         BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  UINTN                         Index;
  UINTN                         BestIndex;
  UINTN                         MatchIndex;

  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);

  MatchIndex = BootOptionCount;
  BestIndex  = BootOptionCount;
  for (Index = 0; Index < BootOptionCount; Index++) {
    if (BmIsBootManagerMenuFilePath (BootOptions[Index].FilePath)) {
      if (BmIsAutoCreateBootOption (&BootOptions[Index])) {
        BestIndex = Index;
        break;
      }

      if (MatchIndex == BootOptionCount) {
        MatchIndex = Index;
      }
    }
  }

  if (BestIndex != BootOptionCount) {
    Index = BestIndex;
  } else if (MatchIndex != BootOptionCount) {
    Index = MatchIndex;
  }

  if (Index != BootOptionCount) {
    Status = EfiBootManagerInitializeLoadOption (
               BootOption,
               BootOptions[Index].OptionNumber,
               BootOptions[Index].OptionType,
               BootOptions[Index].Attributes,
               BootOptions[Index].Description,
               BootOptions[Index].FilePath,
               BootOptions[Index].OptionalData,
               BootOptions[Index].OptionalDataSize
               );
    ASSERT_EFI_ERROR (Status);
  }

  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);

  //
  // Automatically create the Boot#### for Boot Manager Menu when not found.
  //
  if (Index == BootOptionCount) {
    return BmRegisterBootManagerMenu (BootOption);
  } else {
    return EFI_SUCCESS;
  }
}

/**
  Get the next possible full path pointing to the load option.
  The routine doesn't guarantee the returned full path points to an existing
  file, and it also doesn't guarantee the existing file is a valid load option.
  BmGetNextLoadOptionBuffer() guarantees.

  @param FilePath  The device path pointing to a load option.
                   It could be a short-form device path.
  @param FullPath  The full path returned by the routine in last call.
                   Set to NULL in first call.

  @return The next possible full path pointing to the load option.
          Caller is responsible to free the memory.
**/
EFI_DEVICE_PATH_PROTOCOL *
EFIAPI
EfiBootManagerGetNextLoadOptionDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *FilePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *FullPath
  )
{
  return BmGetNextLoadOptionDevicePath (FilePath, FullPath);
}
