/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BootOptionManager.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#include "IpmiBoot.h"
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Protocol/LoadedImage.h>
#include <ByoBootGroup.h>
#include <Protocol/PciIo.h>
#include <Protocol/BlockIo.h>
#include <Library/ByoBootManagerLib.h>

#ifndef NOT_SUPPORT_LEGACY
#include <Library/LegacyBootOptionalDataLib.h>
#endif


EFI_STATUS 
EFIAPI
EfiBootManagerBootApp(
  IN EFI_GUID *AppGuid
  )
{
  EFI_STATUS                     Status;
  EFI_BOOT_MANAGER_LOAD_OPTION   BootOption;

  //
  // Create FV Boot Option
  //
  ZeroMem(&BootOption, sizeof(BootOption));
  BootOption.Status = EFI_INVALID_PARAMETER;

  Status = CreateFvFileBootOption(AppGuid, L"", &BootOption, FALSE, NULL, 0);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  EfiBootManagerBoot(&BootOption);
  EfiBootManagerFreeLoadOption(&BootOption);

  return BootOption.Status;
}

/*
  Check if given DevicePath describes a network device (PXE, HTTP)

*/
BOOLEAN
IsNetworkBootOption (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL *TempDevicePath;

  //
  // Check Device Path is PXE where MSG_IPv4_DP or MSG_IPv6_DP
  //
  TempDevicePath = DevicePath;
  while (!IsDevicePathEndType (TempDevicePath)) {
    if (TempDevicePath->Type == MESSAGING_DEVICE_PATH
        && (TempDevicePath->SubType == MSG_IPv4_DP || TempDevicePath->SubType == MSG_IPv6_DP || TempDevicePath->SubType == MSG_URI_DP)){
      return TRUE;
    }
    TempDevicePath = NextDevicePathNode (TempDevicePath);
  }

  return FALSE;
}


BOOLEAN
EFIAPI
IsHddDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
)
{
  EFI_DEVICE_PATH_PROTOCOL  *CurFullPath;
  EFI_DEVICE_PATH_PROTOCOL  *PreFullPath;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;

  CurFullPath = NULL;
  PreFullPath = NULL;

  //
  // Skip PXE/HTTP BootOption, expanding these boot path could lead to real network communication process (LoadFile)
  // in BmExpandLoadFiles()
  //
  if (IsNetworkBootOption(DevicePath)) {
    return FALSE;
  }

  //
  // Try every Full Device Path generated from bootoption
  //
  CurFullPath = EfiBootManagerGetNextLoadOptionDevicePath (DevicePath, CurFullPath);

  while (CurFullPath != NULL) {
    DEBUG_CODE (
      CHAR16 *DevicePathStr;

      DevicePathStr = ConvertDevicePathToText (CurFullPath, TRUE, TRUE);
      if (DevicePathStr != NULL){
        DEBUG ((DEBUG_INFO, "Full device path %s\n", DevicePathStr));
        FreePool (DevicePathStr);
      }
    );

    //
    // Check if Full Device Path contains SATA HDD Drive Device Node
    //
    TempDevicePath = CurFullPath;
    while (!IsDevicePathEndType (TempDevicePath)) {
      if (TempDevicePath->Type == MESSAGING_DEVICE_PATH && TempDevicePath->SubType == MSG_SATA_DP) {
        if (CurFullPath != NULL) {
          FreePool (CurFullPath);
        }
        return TRUE;
      }
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }


    PreFullPath = CurFullPath;
    CurFullPath = EfiBootManagerGetNextLoadOptionDevicePath (DevicePath, CurFullPath);
    //
    // Free previous full path, move to next full device path
    //
    if (PreFullPath != NULL) {
      FreePool (PreFullPath);
    }
  }

  return FALSE;
}


BOOLEAN
EFIAPI
IsUsbDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath
)
{
  EFI_DEVICE_PATH_PROTOCOL  *CurFullPath;
  EFI_DEVICE_PATH_PROTOCOL  *PreFullPath;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;

  CurFullPath = NULL;
  PreFullPath = NULL;

  //
  // Skip PXE/HTTP BootOption, expanding these boot path could lead to real network communication process (LoadFile)
  // in BmExpandLoadFiles()
  //
  if (IsNetworkBootOption(DevicePath)) {
    return FALSE;
  }

  //
  // Try every Full Device Path generated from bootoption
  //
  CurFullPath = EfiBootManagerGetNextLoadOptionDevicePath (DevicePath, CurFullPath);

  while (CurFullPath != NULL) {
    DEBUG_CODE (
      CHAR16 *DevicePathStr;

      DevicePathStr = ConvertDevicePathToText (CurFullPath, TRUE, TRUE);
      if (DevicePathStr != NULL){
        DEBUG ((DEBUG_INFO, "Full device path %s\n", DevicePathStr));
        FreePool (DevicePathStr);
      }
    );

    //
    // Check if Full Device Path contains USB Drive Device Node
    //
    TempDevicePath = CurFullPath;
    while (!IsDevicePathEndType (TempDevicePath)) {
      if (TempDevicePath->Type == MESSAGING_DEVICE_PATH && TempDevicePath->SubType == MSG_USB_DP) {
        if (CurFullPath != NULL) {
          FreePool (CurFullPath);
        }
        return TRUE;
      }
      TempDevicePath = NextDevicePathNode (TempDevicePath);
    }


    PreFullPath = CurFullPath;
    CurFullPath = EfiBootManagerGetNextLoadOptionDevicePath (DevicePath, CurFullPath);
    //
    // Free previous full path, move to next full device path
    //
    if (PreFullPath != NULL) {
      FreePool (PreFullPath);
    }
  }

  return FALSE;
}


EFI_STATUS 
EFIAPI
EfiBootManagerBootHdd(
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_BOOT_MANAGER_LOAD_OPTION   *Option;
  UINTN                          OptionCount;
  UINTN                          Index;

  Status = EFI_NOT_FOUND;

  //
  // Get BootOptions list which is defined by "BootOrder" variable
  //
  Option = EfiBootManagerGetLoadOptions(&OptionCount, LoadOptionTypeBoot);
  if(Option == NULL || OptionCount == 0){
    return Status;
  }

  //
  // Try to boot HDD Device in BootOption list
  //
  for (Index = 0; Index < OptionCount; Index++) {
    if ((Option[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      DEBUG((EFI_D_INFO, "NotActivated\n"));
      continue;
    }

    if (IsHddDevicePath(Option[Index].FilePath)) {
      DEBUG((EFI_D_INFO, "Try Boot HDD BootOption\n"));
      EfiBootManagerBoot(&Option[Index]);
      //
      // Shouldn't reach here if boot succeeds.
      //
      Status = Option[Index].Status;
      DEBUG((EFI_D_INFO, "Boot Status :%r\n", Option[Index].Status));
    }
  }

  EfiBootManagerFreeLoadOptions(Option, OptionCount);

  return Status;
}


EFI_STATUS 
EFIAPI
EfiBootManagerBootUsb(
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_BOOT_MANAGER_LOAD_OPTION   *Option;
  UINTN                          OptionCount;
  UINTN                          Index;

  Status = EFI_NOT_FOUND;

  //
  // Get BootOptions list which is defined by "BootOrder" variable
  //
  Option = EfiBootManagerGetLoadOptions(&OptionCount, LoadOptionTypeBoot);
  if(Option == NULL || OptionCount == 0){
    return Status;
  }

  //
  // Try to boot USB Device in BootOption list
  //
  for (Index = 0; Index < OptionCount; Index++) {
    if ((Option[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      DEBUG((EFI_D_INFO, "NotActivated\n"));
      continue;
    }

    if (IsUsbDevicePath(Option[Index].FilePath)) {
      DEBUG((EFI_D_INFO, "Try Boot USB BootOption\n"));
      EfiBootManagerBoot(&Option[Index]);
      //
      // Shouldn't reach here if boot succeeds.
      //
      Status = Option[Index].Status;
      DEBUG((EFI_D_INFO, "Boot Status :%r\n", Option[Index].Status));
    }
  }

  EfiBootManagerFreeLoadOptions(Option, OptionCount);

  return Status;
}

BM_MENU_TYPE
GetUefiBootGroupType(
  EFI_DEVICE_PATH_PROTOCOL *FilePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *Dp;
  EFI_HANDLE                    DevHandle;
  EFI_STATUS                    Status;
  EFI_PCI_IO_PROTOCOL           *PciIo;
  UINT8                         ClassCode[3];
  BOOLEAN                       IsODD;
  EFI_BLOCK_IO_PROTOCOL         *BlockIo;
  BM_MENU_TYPE                  GroupType = BM_MENU_TYPE_MAX;
  
  
  IsODD = FALSE;
  Dp = FilePath;


  if (DevicePathType(Dp) == MEDIA_DEVICE_PATH && DevicePathSubType(Dp) == MEDIA_HARDDRIVE_DP) {
    return BM_MENU_TYPE_UEFI_HDD;
  }

  Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &Dp, &DevHandle);
  if(!EFI_ERROR(Status) && IsDevicePathEnd(Dp)){
    gBS->HandleProtocol(DevHandle, &gEfiBlockIoProtocolGuid, (VOID **)&BlockIo);
    if(BlockIo->Media->BlockSize == 2048){
      IsODD = TRUE;
    }
  }

  Dp = FilePath;
  Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DevHandle);
  if(EFI_ERROR(Status)){
    return BM_MENU_TYPE_UEFI_OTHERS;
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
      if(IsODD){
        GroupType = BM_MENU_TYPE_UEFI_USB_ODD;
      } else {
        GroupType = BM_MENU_TYPE_UEFI_USB_DISK;
      }
      break;

    case 0x02:
      GroupType = BM_MENU_TYPE_UEFI_PXE;
      break;

    default:
      GroupType = BM_MENU_TYPE_UEFI_OTHERS;
      break;
  }

  return GroupType;
}

EFI_STATUS 
EFIAPI
BootManagerBootUefiGroup(
  BM_MENU_TYPE GroupType
  )
{
  EFI_STATUS                     Status;
  EFI_BOOT_MANAGER_LOAD_OPTION   *Option;
  UINTN                          OptionCount;
  UINTN                          Index;

  Status = EFI_NOT_FOUND;

  //
  // Get BootOptions list which is defined by "BootOrder" variable
  //
  Option = EfiBootManagerGetLoadOptions(&OptionCount, LoadOptionTypeBoot);
  if(Option == NULL || OptionCount == 0){
    return Status;
  }

  for (Index = 0; Index < OptionCount; Index++) {
    if ((Option[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      DEBUG((EFI_D_INFO, "NotActived\n"));
      continue;
    }

    if (GetUefiBootGroupType(Option[Index].FilePath) == GroupType){
      DEBUG((EFI_D_INFO, "Try Boot %s\n",Option[Index].Description));
      EfiBootManagerBoot(&Option[Index]);
      //
      // Shouldn't reach here if boot succeeds.
      //
      Status = Option[Index].Status;
      DEBUG((EFI_D_INFO, "Boot Status :%r\n", Option[Index].Status));
    }else{
      DEBUG((EFI_D_INFO,"Boot %s mismatch ,Grouptype = %0x\n",Option[Index].Description,GetUefiBootGroupType(Option[Index].FilePath)));
    }
  }

  EfiBootManagerFreeLoadOptions(Option, OptionCount);

  return Status;
}

#ifdef NOT_SUPPORT_LEGACY

EFI_STATUS 
EFIAPI
BootManagerBootLegacyGroup(
  BM_MENU_TYPE GroupType
  )
{
  ASSERT (FALSE);
  return EFI_UNSUPPORTED;
}

#else

BM_MENU_TYPE
GetLegacyBootGroupType(
  EFI_DEVICE_PATH_PROTOCOL *FilePath,
  UINT8 *OptionalData
  )
{
  UINT8   DevClass;
  UINT8   BbsType;

  DevClass = LegacyBootOptionalDataGetPciClass(OptionalData, NULL);
  BbsType  = (UINT8)((BBS_BBS_DEVICE_PATH*)FilePath)->DeviceType;
  return GetBbsGroupType(DevClass, BbsType); 
}

EFI_STATUS 
EFIAPI
BootManagerBootLegacyGroup(
  BM_MENU_TYPE GroupType
  )
{
  EFI_STATUS                     Status;
  EFI_BOOT_MANAGER_LOAD_OPTION   *Option;
  UINTN                          OptionCount;
  UINTN                          Index;

  Status = EFI_NOT_FOUND;

  //
  // Get BootOptions list which is defined by "BootOrder" variable
  //
  Option = EfiBootManagerGetLoadOptions(&OptionCount, LoadOptionTypeBoot);
  if(Option == NULL || OptionCount == 0){
    return Status;
  }
  for (Index = 0; Index < OptionCount; Index++) {
    if ((Option[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      DEBUG((EFI_D_INFO, "NotActivated\n"));
      continue;
    }
    if ((DevicePathType (Option[Index].FilePath) != BBS_DEVICE_PATH) || (DevicePathSubType (Option[Index].FilePath) != BBS_BBS_DP)) {
      continue;
    }
    if (GetLegacyBootGroupType(Option[Index].FilePath,Option[Index].OptionalData) == GroupType) {
      DEBUG((EFI_D_INFO, "Try Boot %s\n",Option[Index].Description));
      EfiBootManagerBoot(&Option[Index]);
      //
      // Shouldn't reach here if boot succeeds.
      //
      Status = Option[Index].Status;
      DEBUG((EFI_D_INFO, "Boot Status :%r\n", Option[Index].Status));
    } else {
      DEBUG((EFI_D_INFO,"Boot %s mismatch ,Grouptype = %0x\n",Option[Index].Description,GetLegacyBootGroupType(Option[Index].FilePath,Option[Index].OptionalData)));
    }
  }

  EfiBootManagerFreeLoadOptions(Option, OptionCount);

  return Status;
}
#endif
