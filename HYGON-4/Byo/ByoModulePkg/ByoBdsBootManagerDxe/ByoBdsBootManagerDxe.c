/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoBdsBootManagerDxe.c

Abstract:
  The platform installs gByoPlatformBootManagerProtocolGuid.

Revision History:

**/

#include "ByoBdsBootManagerDxe.h"
#include <Library/BaseMemoryLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <Protocol/ByoPlatformBootManager.h>

#ifndef BYO_ONLY_UEFI
extern EFI_BOOT_MANAGER_REFRESH_LEGACY_BOOT_OPTION  mEfiBootManagerRefreshLegacyBootOption;
extern EFI_BOOT_MANAGER_LEGACY_BOOT                 mEfiBootManagerLegacyBoot;

EDKII_LEGACY_BOOT_MANAGER_PROTOCOL gEdkiiLegacyBootManagerProtocol = {
  EDKII_LEGACY_BOOT_MANAGER_PROTOCOL_REVISION,
  NULL,
};
#endif
#ifndef BYO_ONLY_UEFI
UINT8
GetLegacyBootGroupType(
  EFI_DEVICE_PATH_PROTOCOL *FilePath,
  UINT8                    *OptionalData
  );
#endif

EFI_BDS_BOOT_MANAGER_PROTOCOL gBdsBootManagerProtocol = {
  NULL,
  NULL,
  ByoEfiBootManagerGetLoadOptions2,
  ByoEfiBootManagerFreeLoadOptions,
  NULL,
  NULL,  
  ByoEfiBootManagerBoot,
  EfiBootManagerGetVariableAndSize,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  0,
  NULL,
  0,
  NULL,
  0
};

BOOLEAN
GetFsDpFromOsShortDp (
  EFI_DEVICE_PATH_PROTOCOL **DevPath
  )
{
  EFI_DEVICE_PATH_PROTOCOL      *FsDp = NULL;
  EFI_DEVICE_PATH_PROTOCOL      *TempDp;
  EFI_DEVICE_PATH_PROTOCOL      *Dp;
  EFI_STATUS                    Status;
  UINTN                         HandleCount;
  EFI_HANDLE                    *Handles;
  UINTN                         Index;
  BOOLEAN                       rc = FALSE;


  Dp = *DevPath;
  
  if (DevicePathType(Dp) == MEDIA_DEVICE_PATH && DevicePathSubType(Dp) == MEDIA_HARDDRIVE_DP) {

    Status = gBS->LocateHandleBuffer (
                     ByProtocol,
                     &gEfiSimpleFileSystemProtocolGuid,
                     NULL,
                     &HandleCount,
                     &Handles
                     );
    if(EFI_ERROR(Status)){
      HandleCount = 0;
    }
    for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      Handles[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID**)&FsDp
                      );
      if(EFI_ERROR(Status)){
        continue;
      }

      TempDp = FsDp;
      while(!IsDevicePathEnd(TempDp)){
        if(DevicePathType(TempDp) == MEDIA_DEVICE_PATH && DevicePathSubType(TempDp) == MEDIA_HARDDRIVE_DP){
          break;
        }
        TempDp = NextDevicePathNode(TempDp);
      }
      if(IsDevicePathEnd(TempDp)){
        continue;
      }
      
      if(CompareMem(Dp, TempDp, DevicePathNodeLength(Dp)) != 0){
        continue;
      }

      DEBUG((EFI_D_INFO, "target HD found\n"));
      *DevPath = FsDp;
      rc = TRUE;
      break;
    }

  }
  
  return rc;
}

CHAR16* 
EFIAPI
ByoGetExtBmBootName (
  CHAR16                   *Name, 
  EFI_DEVICE_PATH_PROTOCOL *Dp, 
  UINT8                    *OptionalData, 
  UINT32                   OptionalDataSize,
  BOOLEAN                  *Updated
  )
{
  EFI_STATUS               Status;
  CHAR8                    *ModelName;
  CHAR16                   *p;
  UINTN                    Size;
  CONST VOID               *OptData;
  UINTN                    OptDataSize;  
  EFI_HANDLE               Handle;  
  BYO_DISKINFO_PROTOCOL    *ByoDiskInfo;
  EFI_DEVICE_PATH_PROTOCOL *TempDp;
  

  DEBUG((EFI_D_INFO, "%a(%s)\n", __FUNCTION__, Name));

  *Updated = FALSE;
  if(DevicePathType(Dp) == BBS_DEVICE_PATH || PcdGet8(PcdBdsBootOrderUpdateMethod) == 1){
    return Name;
  }

  OptData = PcdGetPtr(PcdEfiBootOptionFlag);
  OptDataSize = PcdGetSize(PcdEfiBootOptionFlag);
  if(OptionalDataSize == OptDataSize && CompareMem(OptionalData, OptData, OptDataSize) == 0){
    return Name;
  }

  TempDp = Dp;
  GetFsDpFromOsShortDp(&TempDp);
  Status = gBS->LocateDevicePath(&gByoDiskInfoProtocolGuid, &TempDp, &Handle); 
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "No ByoDiskInfo(%r)\n", Status));
    return Name;    
  }
  
  Status = gBS->HandleProtocol(
                  Handle,
                  &gByoDiskInfoProtocolGuid,
                  (VOID **)&ByoDiskInfo
                  );
  ASSERT(!EFI_ERROR(Status));
  ModelName = NULL;
  Size = 0;
  Status = ByoDiskInfo->GetMn(ByoDiskInfo, ModelName, &Size);
  if(Status != EFI_BUFFER_TOO_SMALL){
    DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));   
    return Name;
  }
  
  ModelName = AllocatePool(Size);
  ASSERT(ModelName != NULL); 
  Status = ByoDiskInfo->GetMn(ByoDiskInfo, ModelName, &Size);    
  if(EFI_ERROR(Status)){
    FreePool(ModelName);
    DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
    return Name;    
  }

  Size = StrSize(Name) + (4 + Size) * sizeof(CHAR16);
  p = (CHAR16*)AllocatePool(Size);
  ASSERT(p != NULL);
  UnicodeSPrint(p, Size, L"%s (%a)", Name, ModelName);
  FreePool(ModelName);

  *Updated = TRUE;  
  return p;
}

VOID
EFIAPI
ByoPrintPostString (
  IN UINTN     Row,
  IN UINTN     Column,
  IN CHAR16    *String,
  IN UINTN     Attribute
  )
{
  if(String == NULL){
    return;
  }

  if(Attribute != 0){
    gST->ConOut->SetAttribute (gST->ConOut, Attribute);
  }
  gST->ConOut->SetCursorPosition (gST->ConOut, Column, Row);
  gST->ConOut->OutputString (gST->ConOut, String);
}

EFI_STATUS
EFIAPI
ByoEfiBootManagerGetLoadOptions2 (
  OUT EFI_BOOT_MANAGER_LOAD_OPTION   **Option,
  OUT UINTN                          *OptionCount
)
{
  *Option = EfiBootManagerGetLoadOptions(OptionCount, LoadOptionTypeBoot);
  if(*Option == NULL || *OptionCount == 0){
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
EfiBootManagerDeleteAllUefiBootOption(
  VOID
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION  *NvBootOptions;
  UINTN                         NvBootOptionCount;
  UINTN                         Index;
  EFI_DEVICE_PATH_PROTOCOL      *FilePath;  


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  NvBootOptions = EfiBootManagerGetLoadOptions (&NvBootOptionCount, LoadOptionTypeBoot);

  for (Index = 0; Index < NvBootOptionCount; Index++) {
    FilePath = NvBootOptions[Index].FilePath;
    if (DevicePathType(FilePath) == BBS_DEVICE_PATH) {
      continue;
    }  
    if((NvBootOptions[Index].Attributes & LOAD_OPTION_CATEGORY) == LOAD_OPTION_CATEGORY_APP) {
      continue;
    }  
    EfiBootManagerDeleteLoadOptionVariable(NvBootOptions[Index].OptionNumber, LoadOptionTypeBoot);
  }

  ByoEfiBootManagerFreeLoadOptions (NvBootOptions, NvBootOptionCount);
  return EFI_SUCCESS;	
}

VOID
AfterConsoleGetBootOptions (
  VOID
  )
{
  EFI_BDS_BOOT_MANAGER_PROTOCOL   *BdsBootManager;
  EFI_BOOT_MANAGER_LOAD_OPTION    *Options;
  EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions;
  UINTN                           BootOptionCount;
  UINTN                           Index;
  UINTN                           IndexJ;
  BOOLEAN                         IsLegacyBoot;
  UINTN BufferSize;
  UINTN *BootOptionOrder;
  EFI_STATUS Status;
  BYO_PLATFORM_BOOT_MANAGER_PROTOCOL  *ByoPlatformBootManager = NULL;

  BdsBootManager = &gBdsBootManagerProtocol;

  IsLegacyBoot = FALSE;
  #ifndef BYO_ONLY_UEFI
  //
  // Legacy Boot Only
  //
  if (PcdGet8(PcdBiosBootModeType) == BIOS_BOOT_LEGACY_OS) {
    IsLegacyBoot = TRUE;
    //
    // Remove UEFI boot options
    //
    EfiBootManagerDeleteAllUefiBootOption();
    DEBUG ((DEBUG_INFO, "Remove UEFI Boot Option on Legacy Only!\n"));
  }

  if (PcdGet8(PcdBiosBootModeType) != BIOS_BOOT_UEFI_OS) {
    //
    // Install gEdkiiLegacyBootManagerProtocol
    //
    gEdkiiLegacyBootManagerProtocol.LegacyBoot = mEfiBootManagerLegacyBoot;
    gEdkiiLegacyBootManagerProtocol.GetLegacyBootGroupType = GetLegacyBootGroupType;
    Status = gBS->InstallProtocolInterface (
                  &gImageHandle,
                  &gEdkiiLegacyBootManagerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gEdkiiLegacyBootManagerProtocol);
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Prepare Legacy Boot Options
  //
  if (mEfiBootManagerRefreshLegacyBootOption != NULL) {
    mEfiBootManagerRefreshLegacyBootOption();
    DEBUG ((DEBUG_INFO, "Refresh Legacy Boot Option!\n"));
  }
  #endif

  Status = gBS->LocateProtocol (&gByoPlatformBootManagerProtocolGuid,
                            NULL,
                            (VOID **)&ByoPlatformBootManager);
  if (!EFI_ERROR(Status) && (ByoPlatformBootManager != NULL)
    && (ByoPlatformBootManager->EndOfRefreshBootOption != NULL)) {
      ByoPlatformBootManager->EndOfRefreshBootOption();
  }

  //
  // Collect Boot Options
  //
  BdsBootManager->BootOptions = EfiBootManagerGetLoadOptions(&BdsBootManager->BootOptionsCount, LoadOptionTypeBoot);
  DEBUG ((DEBUG_INFO, "BootOptions Count is %d\n", BdsBootManager->BootOptionsCount));
  if (BdsBootManager->BootOptionsCount == 0) {
    return;
  }

  //
  // Legacy Boot Option has been assigned in LegacyBootManagerLib
  //
  if (IsLegacyBoot) {
    return;
  }

  //
  // Find the default boot option
  //
  BufferSize      = 0;
  BootOptionOrder = NULL;
  Status = gRT->GetVariable (L"BootOptionOrder", &gEdkiiBdsVariableGuid, NULL, &BufferSize, BootOptionOrder);
  if (Status == EFI_BUFFER_TOO_SMALL) {
    BootOptionOrder = AllocatePool (BufferSize);
    Status = gRT->GetVariable (L"BootOptionOrder", &gEdkiiBdsVariableGuid, NULL, &BufferSize, BootOptionOrder);
  }
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Find the matched UEFI boot options
  //
  Options = BdsBootManager->BootOptions;
  BootOptions = AllocateZeroPool (sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * BdsBootManager->BootOptionsCount);
  BootOptionCount = 0;
  for (IndexJ = 0; IndexJ < BufferSize / sizeof (UINTN); IndexJ ++) {
    for (Index = 0; Index < BdsBootManager->BootOptionsCount; Index++) {
      if (((Options[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) || Options[Index].Deleted) {
        continue;
      }

      if (Options[Index].OptionNumber == BootOptionOrder[IndexJ]) {
        DEBUG((EFI_D_INFO, "[%04X] %s\n", Options[Index].OptionNumber, Options[Index].Description));
        ByoCopyBootOption (BootOptions + BootOptionCount, Options + Index);
        BootOptionCount++;
      }
    }
  }

  BdsBootManager->UefiOptions = BootOptions;
  BdsBootManager->UefiOptionsCount = BootOptionCount;

  FreePool (BootOptionOrder);
  return;
}

EFI_STATUS
EFIAPI
CreateSetupBootOption (
  OUT EFI_BOOT_MANAGER_LOAD_OPTION   *SetupOption
) {
  UINTN                           Index;
  EFI_BDS_BOOT_MANAGER_PROTOCOL   *BdsBootManager;

  BdsBootManager = &gBdsBootManagerProtocol;

  for (Index = 0; Index < BdsBootManager->BootOptionsCount; Index ++) {
    if (BmIsBootManagerMenuFilePath (BdsBootManager->BootOptions[Index].FilePath)) {
      break;
    }
  }

  if (Index < BdsBootManager->BootOptionsCount) {
    ByoCopyBootOption (SetupOption, BdsBootManager->BootOptions + Index);
    SetupOption->Attributes = LOAD_OPTION_CATEGORY_APP | LOAD_OPTION_ACTIVE;
    return EFI_SUCCESS;
  }

  SetupOption->Attributes = LOAD_OPTION_HIDDEN;

  return EFI_UNSUPPORTED;
}

VOID
EFIAPI
DummyConnectAll (
  VOID
  ) {
  return;
}

VOID
EFIAPI
DummyRefreshOptions (
  VOID
  ) {
  return;
}

EFI_STATUS
CreateDpFileBootOption (
  EFI_DEVICE_PATH_PROTOCOL     *FilePathDp,
  CHAR16                       *Description,
  EFI_BOOT_MANAGER_LOAD_OPTION *BootOption,
  BOOLEAN                      IsBootCategory,
  UINT8                        *OptionalData,    OPTIONAL
  UINT32                       OptionalDataSize       
  )
{
  UINT32        Attributes;
  EFI_STATUS    Status;
  

  if ((BootOption == NULL) || (FilePathDp == NULL) || (Description == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

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
             FilePathDp,
             OptionalData,
             OptionalDataSize
             );

  return Status;
}

EFI_STATUS 
EFIAPI
ByoEfiBootManagerBootFileParam (
  IN  EFI_DEVICE_PATH_PROTOCOL *FileDp,
  IN  VOID                     *Parameter,
  IN  UINTN                    ParameterSize,
  IN  BOOLEAN                  IsBootCategory
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION   BootOption;

  ZeroMem(&BootOption, sizeof(BootOption));
  BootOption.Status = EFI_INVALID_PARAMETER;

  CreateDpFileBootOption(FileDp, L"", &BootOption, IsBootCategory, (UINT8*)Parameter, (UINT32)ParameterSize);
  ByoEfiBootManagerBoot(&BootOption);
  EfiBootManagerFreeLoadOption(&BootOption);

  return BootOption.Status;
}

UINTN
EFIAPI
DummyGetCurHotKey (
  VOID
  )
{
  return 0;
}

VOID
EFIAPI
DummySetCurHotKey (
  UINTN HotKey
  )
{
  return;
}

EFI_STATUS
EFIAPI
ByoEfiBootManagerBootApp(IN EFI_GUID *AppGuid)
{
  EFI_BOOT_MANAGER_LOAD_OPTION   BootOption;

  ZeroMem(&BootOption, sizeof(BootOption));
  BootOption.Status = EFI_INVALID_PARAMETER;

  CreateFvBootOption(AppGuid, L"", &BootOption, FALSE, NULL, 0);
  ByoEfiBootManagerBoot(&BootOption);
  EfiBootManagerFreeLoadOption(&BootOption);

  return BootOption.Status;
}


VOID
EFIAPI
InstallAfterConsoleGetOptions(
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  EFI_STATUS          Status;
  EFI_HANDLE          Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gPlatAfterConsoleEndProtocolGuid, AfterConsoleGetBootOptions,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
}

EFI_STATUS
EFIAPI
ByoBdsBootManagerDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS          Status;
  EFI_EVENT           EndOfDxeEvent;


  gBdsBootManagerProtocol.ConnectAll     = DummyConnectAll;
  gBdsBootManagerProtocol.RefreshOptions = DummyRefreshOptions;
  gBdsBootManagerProtocol.CreateSetupOption = CreateSetupBootOption;
  gBdsBootManagerProtocol.PrintPostString  = ByoPrintPostString;
  gBdsBootManagerProtocol.GetExtBmBootName = ByoGetExtBmBootName;
  gBdsBootManagerProtocol.BootFileParam  = ByoEfiBootManagerBootFileParam;
  gBdsBootManagerProtocol.GetCurHotKey = DummyGetCurHotKey;
  gBdsBootManagerProtocol.SetCurHotKey = DummySetCurHotKey;
  gBdsBootManagerProtocol.BootApp      = ByoEfiBootManagerBootApp;

  //
  // Install gEfiBootManagerProtocolGuid
  //
  Status = gBS->InstallProtocolInterface (
                 &ImageHandle,
                 &gEfiBootManagerProtocolGuid,
                 EFI_NATIVE_INTERFACE,
                 &gBdsBootManagerProtocol);
  ASSERT_EFI_ERROR (Status);  

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  InstallAfterConsoleGetOptions,
                  NULL,
                  &gEfiEndOfDxeEventGroupGuid,
                  &EndOfDxeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}
