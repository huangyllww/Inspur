/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By:
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include "PlatformDxe.h"
#include <Guid/Acpi.h>
#include <Guid/AcpiS3Context.h>
#include <IndustryStandard/Pci.h>
#include <Library/SerialPortLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/LockBoxLib.h>
#include <Library/MtrrLib.h>
#include <Library/ByoRtcLib.h>
#include <Protocol/VariableWrite.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SmmCommunication.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/NvMediaAccess.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Library/TcgPhysicalPresenceLib.h>
#include <Library/Tcg2PhysicalPresenceLib.h>
#include <Guid/EventLegacyBios.h>
#include <HygonCpu.h>
#include "Library/IoLib.h"
#include <Library/ByoCommLib.h>
#include <ByoStatusCode.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/LoadedImage.h>
#include <ByoBootGroup.h>
#include <Protocol/PciPreprocessHookParameter.h>
#include <Library/DxeServicesLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/ByoBootManagerLib.h>
#include <Library/PrintLib.h>
#include <Guid/GlobalVariable.h>
#include <Library/DevicePathLib.h>
#include <Protocol/ReportRasUceProtocol.h>



VOID
SmbiosCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

EFI_STATUS
SmmAccessInit (
    IN EFI_HANDLE           ImageHandle,
    IN EFI_SYSTEM_TABLE     *SystemTable
    );

VOID
ClearRootBridgeResource (
  PLATFORM_COMM_INFO  *Info
  );

VOID
SetRootBridgeMmio64Range (
  PLATFORM_COMM_INFO  *Info
  );

EFI_STATUS AddSmbiosTypeLate();

extern EFI_GUID gBdsAllDriversConnectedProtocolGuid;
extern EFI_GUID gEfiSetupEnterGuid;

CONST SETUP_DATA      *gSetupData;
BOOLEAN               gAfterReadyToBoot = FALSE;
EFI_HII_HANDLE        gHiiHandle = NULL;
BOOLEAN               gIsCmosBad;
BOOLEAN               gCsmOn;
PLATFORM_COMM_INFO    *gPlatCommInfo;


#define EFI_MEMORY_PRESENT      0x0100000000000000ULL
#define EFI_MEMORY_INITIALIZED  0x0200000000000000ULL
#define EFI_MEMORY_TESTED       0x0400000000000000ULL


VOID
MemoryAbove4GBRecovery (
  VOID
  )
{
  UINTN                            NumberOfDescriptors;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR  *MemorySpaceMap = NULL;
  UINTN                            Index;
  EFI_STATUS                       Status;


  DEBUG((EFI_D_INFO, "MemoryAbove4GBRecovery\n"));

  Status = gDS->GetMemorySpaceMap (&NumberOfDescriptors, &MemorySpaceMap);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, "L%d %r\n", __LINE__, Status));
    return;
  }

  for (Index = 0; Index < NumberOfDescriptors; Index++) {
    if(MemorySpaceMap[Index].BaseAddress < SIZE_4GB){
      continue;
    }

    DEBUG((EFI_D_INFO, "(%lX,%lX) T:%X C:%lX\n", MemorySpaceMap[Index].BaseAddress, \
      MemorySpaceMap[Index].Length, MemorySpaceMap[Index].GcdMemoryType, \
      MemorySpaceMap[Index].Capabilities));

    if (MemorySpaceMap[Index].GcdMemoryType != EfiGcdMemoryTypeReserved) {
      continue;
    }
    if ((MemorySpaceMap[Index].Capabilities & (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED))
         != (EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED)) {
         continue;
    }

    DEBUG((EFI_D_INFO, "(%lX,%lX)\n", MemorySpaceMap[Index].BaseAddress, MemorySpaceMap[Index].Length));

    Status = gDS->RemoveMemorySpace (
                    MemorySpaceMap[Index].BaseAddress,
                    MemorySpaceMap[Index].Length
                    );
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "RemoveMemorySpace:%r\n", Status));
      continue;
    }
    Status = gDS->AddMemorySpace (
                    EfiGcdMemoryTypeSystemMemory,
                    MemorySpaceMap[Index].BaseAddress,
                    MemorySpaceMap[Index].Length,
                    MemorySpaceMap[Index].Capabilities & ~(EFI_MEMORY_PRESENT | EFI_MEMORY_INITIALIZED | EFI_MEMORY_TESTED | EFI_MEMORY_RUNTIME)
                    );
    if(EFI_ERROR(Status)){
      DEBUG((EFI_D_INFO, "AddMemorySpace:%r\n", Status));
      continue;
    }
  }

  if(MemorySpaceMap == NULL){
    FreePool(MemorySpaceMap);
  }

  InvokeHookProtocol(gBS, &gByoMemoryAbove4GBReportedHookGuid);
  
}




VOID
EFIAPI
PlatOnReadyToBoot (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  UINT8  Cmos;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  gBS->CloseEvent(Event);

  MemoryAbove4GBRecovery();

  gAfterReadyToBoot = TRUE;

  Cmos = PcdGet8(PcdNvVarSwSetCmosOffset);
  if(PcdGetBool(PcdPlatformNvVariableSwUpdated)){
    if(Cmos != 0xFF){
      WriteCheckedCmosByte(Cmos, 0);
    }
  }
}



VOID
EFIAPI
AllDriversConnectedCallBack (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  VOID        *Interface;
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }

  if ((PcdGet8 (PcdBiosBootModeType) != BIOS_BOOT_UEFI_OS) && PcdGet8 (PcdDisplayOptionRomMessage) == 0) {
    //
    // Stop "Hide loading legacy driver information ..."
    //
    CleanHideOpromMsg ();
  }

  gBS->CloseEvent(Event);
}



VOID SetDefaultLanguage()
{
  EFI_STATUS     Status;
  CHAR8          *DefLang;


  DefLang = (CHAR8*)PcdGetPtr(PcdUefiVariableDefaultPlatformLang);
  Status = gRT->SetVariable (
                  L"PlatformLang",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  AsciiStrSize(DefLang),
                  DefLang
                  );
  DEBUG((EFI_D_INFO, "set PlatformLang:%r\n", Status));
}

VOID HandleCmosBad()
{
  UINT8  CmosBadReason;

  if(gIsCmosBad){
    CmosBadReason = PcdGet8(PcdCmosBadReason);
    if(CmosBadReason == CMOS_BAD_REASON_JUMPER){
      ByoSetDefaultTime();
    }
    SetDefaultLanguage();
    ByoRemoveBootOrder();
    DEBUG((EFI_D_INFO, "report cmos bad\n"));
    REPORT_STATUS_CODE(EFI_ERROR_CODE, BSC_CMOS_RECOVERY_OCCURRED);
  }
}

VOID
SynchronizeVariable()
{
  EFI_STATUS                    Status;
  SETUP_DATA                    SetupData;
  UINTN                         Size;
  BOOLEAN                       Changed = FALSE;

  Size = sizeof(SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );

  if (PcdGet8(PcdBiosBootModeType) != SetupData.BootModeType) {
    SetupData.BootModeType = PcdGet8(PcdBiosBootModeType);
    Changed = TRUE;
  }
  if(SetupData.ClearUceIsoData){
    SetupData.ClearUceIsoData = 0;
    Changed = TRUE;
  }

  if(Changed){
    PcdSetBoolS(PcdPlatformNvVariableSwUpdated, TRUE);
    Status = gRT->SetVariable (
                    PLATFORM_SETUP_VARIABLE_NAME,
                    &gPlatformSetupVariableGuid,
                    PLATFORM_SETUP_VARIABLE_FLAG,
                    Size,
                    &SetupData
                    );
  }
}

VOID
EFIAPI
VariableWriteArchCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID        *Interface;
  EFI_STATUS  Status;


  Status = gBS->LocateProtocol(&gEfiVariableWriteArchProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  HandleCmosBad();
  SynchronizeVariable();
}



VOID PlatformAfterConsoleStartHook()
{
  DEBUG((EFI_D_INFO, "PlatformAfterConsoleStartHook\n"));
}



VOID PlatformAfterConsoleEndHook()
{
  DEBUG((EFI_D_INFO, "PlatformAfterConsoleEndHook\n"));
  AddSmbiosTypeLate();
  
  if(gIsCmosBad){
    DEBUG((EFI_D_INFO, "Clear Cmos Bad\n"));
    LibClearCmosBad();
  }
}


VOID PlatformBeforeBiosUpdateHook()
{

}



STATIC EFI_GUID gUefiPxeBcDxeFileGuid    = {0xB95E9FDA, 0x26DE, 0x48d2, {0x88, 0x07, 0x1F, 0x91, 0x07, 0xAC, 0x5E, 0x3A}};
STATIC EFI_GUID gUefiHttpBootDxeFileGuid = {0xecebcb00, 0xd9c8, 0x11e4, {0xaf, 0x3d, 0x8c, 0xdc, 0xd4, 0x26, 0xc9, 0x73}};
STATIC EFI_GUID gDriverHealthManagerDxeGuid = {0xEBF8ED7C, 0x0DD1, 0x4787, {0x84, 0xF1, 0xF4, 0x8D, 0x53, 0x7D, 0xCA, 0xCF}};
STATIC EFI_GUID gUsbMassStorageDxeGuid      = {0x9FB4B4A7, 0x42C0, 0x4bcd, {0x85, 0x40, 0x9B, 0xCC, 0x67, 0x11, 0xF8, 0x3E}};

VOID RemoveUnusedDriver()
{
  UINTN                         DriverImageHandleCount;
  EFI_HANDLE                    *DriverImageHandleBuffer = NULL;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
  EFI_STATUS                    Status;
  UINTN                         Index;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH  *FvFileDp;
  BOOLEAN                            UnloadPxe  = FALSE;
  BOOLEAN                            UnloadHttp = FALSE;
  BOOLEAN                            UnloadDriverHealth = FALSE;
  BOOLEAN                            UnloadUsbMass;
  VOID                               *Interface;


  DEBUG((EFI_D_INFO, "RemoveUnusedDriver\n"));

  UnloadUsbMass = !gSetupData->UsbMassSupport;

  if(!gSetupData->LanBoot){
    UnloadPxe  = TRUE;
    UnloadHttp = TRUE;
  }
  if(gSetupData->HttpBootDis){
    UnloadHttp = TRUE;
    PcdSetBoolS(PcdNetBootHttpEnable, FALSE);
  } else {
    PcdSetBoolS(PcdNetBootHttpEnable, TRUE);
  }

  if (gSetupData->BootModeType == BIOS_BOOT_LEGACY_OS) {
    UnloadDriverHealth = TRUE;
  }

  if(!UnloadPxe && !UnloadHttp && !UnloadDriverHealth && !UnloadUsbMass){
    return;
  }

  DriverImageHandleCount = 0;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiLoadedImageProtocolGuid,
                  NULL,
                  &DriverImageHandleCount,
                  &DriverImageHandleBuffer
                  );
  if(EFI_ERROR(Status)){
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

    if(DevicePathType(LoadedImage->FilePath) == MEDIA_DEVICE_PATH &&
       DevicePathSubType(LoadedImage->FilePath) == MEDIA_PIWG_FW_FILE_DP) {
      FvFileDp = (MEDIA_FW_VOL_FILEPATH_DEVICE_PATH*)LoadedImage->FilePath;
      if(UnloadPxe && CompareGuid(&FvFileDp->FvFileName, &gUefiPxeBcDxeFileGuid)){
        Status = gBS->UnloadImage(DriverImageHandleBuffer[Index]);
        DEBUG((EFI_D_INFO, "Unload Pxe:%r\n", Status));
        UnloadPxe = FALSE;
        continue;
      }
      if(UnloadHttp && CompareGuid(&FvFileDp->FvFileName, &gUefiHttpBootDxeFileGuid)){
        Status = gBS->UnloadImage(DriverImageHandleBuffer[Index]);
        DEBUG((EFI_D_INFO, "Unload Http:%r\n", Status));
        UnloadHttp = FALSE;
        continue;
      }
      if (UnloadDriverHealth && CompareGuid(&FvFileDp->FvFileName, &gDriverHealthManagerDxeGuid)) {
        Status = gBS->UnloadImage(DriverImageHandleBuffer[Index]);
        DEBUG((EFI_D_INFO, "Unload DriverHealthManagerDxe:%r\n", Status));
        UnloadDriverHealth = FALSE;
        continue;
      }
      if (UnloadUsbMass && CompareGuid(&FvFileDp->FvFileName, &gUsbMassStorageDxeGuid)) {
//      Status = gBS->UnloadImage(DriverImageHandleBuffer[Index]);    // this driver does not have a unload function.
        Status = gBS->HandleProtocol(
                        DriverImageHandleBuffer[Index],
                        &gEfiDriverBindingProtocolGuid,
                        &Interface
                        );
        if(!EFI_ERROR(Status)){
          Status = gBS->UninstallProtocolInterface(
                          DriverImageHandleBuffer[Index],
                          &gEfiDriverBindingProtocolGuid,
                          Interface
                          );
          DEBUG((EFI_D_INFO, "UnIns DriverBinding:%r\n", Status));
        }
        Status = gBS->HandleProtocol(
                        DriverImageHandleBuffer[Index],
                        &gEfiComponentNameProtocolGuid,
                        &Interface
                        );
        if(!EFI_ERROR(Status)){
          Status = gBS->UninstallProtocolInterface(
                          DriverImageHandleBuffer[Index],
                          &gEfiComponentNameProtocolGuid,
                          Interface
                          );
          DEBUG((EFI_D_INFO, "UnIns Cn:%r\n", Status));
        }
        Status = gBS->HandleProtocol(
                        DriverImageHandleBuffer[Index],
                        &gEfiComponentName2ProtocolGuid,
                        &Interface
                        );
        if(!EFI_ERROR(Status)){
          Status = gBS->UninstallProtocolInterface(
                          DriverImageHandleBuffer[Index],
                          &gEfiComponentName2ProtocolGuid,
                          Interface
                          );
          DEBUG((EFI_D_INFO, "UnIns Cn2:%r\n", Status));
        }
        UnloadUsbMass = FALSE;
        continue;
      }
    }

    if(!UnloadPxe && !UnloadHttp && !UnloadDriverHealth && !UnloadUsbMass){
      break;
    }

  }

ProcExit:
  if(DriverImageHandleBuffer != NULL){
    FreePool(DriverImageHandleBuffer);
  }
  return;
}


VOID PlatformBeforeConnectPciRootBridgeHook()
{
  PLATFORM_COMM_INFO  *Info;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  RemoveUnusedDriver();

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
}


VOID LegacyBiosConnectPciRootBridgeHook();

VOID PlatformAfterConnectPciRootBridgeHook()
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

#if !defined(MDEPKG_NDEBUG)
  ListPci(gBS);
#endif
  if(gCsmOn){
    LegacyBiosConnectPciRootBridgeHook();
  }
}


VOID PlatformAfterAllPciIoHook()
{
  PLATFORM_COMM_INFO  *Info;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

//PrepareIoRes(Info);
//SetRootBridgeIoRange(Info);
}



BOOLEAN
IsNetworkFvDispatch (
  VOID
  )
{
  UINTN                     Index;
  UINTN                     HandleCount;
  EFI_HANDLE                *Handles;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *FvDevicePath;
  EFI_STRING                Str;
  EFI_GUID                  NetworkFvNameGuid = {0x85E1ECA2, 0x8309, 0x4286, {0x93, 0x3F, 0x98, 0xC5, 0x61, 0x1C, 0x42, 0xFD}};
  EFI_STATUS                Status;

  Status = gBS->LocateHandleBuffer (
    ByProtocol,
    &gEfiFirmwareVolume2ProtocolGuid,
    NULL,
    &HandleCount,
    &Handles
  );

  if (EFI_ERROR(Status)) {
    return FALSE;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    DevicePath = DevicePathFromHandle(Handles[Index]);
    Str = ConvertDevicePathToText (
            DevicePath,
            FALSE,
            TRUE
          );
    if(Str != NULL){
      DEBUG((EFI_D_INFO, "FV DevicePath  %s\n", Str));
      FreePool(Str);
    }

    FvDevicePath = DevicePath;
    while (!IsDevicePathEndType (FvDevicePath)) {
      if (DevicePathType (FvDevicePath) == MEDIA_DEVICE_PATH &&
          DevicePathSubType (FvDevicePath) == MEDIA_PIWG_FW_VOL_DP) {
        if (CompareMem (&(((MEDIA_FW_VOL_DEVICE_PATH *) FvDevicePath)->FvName), &NetworkFvNameGuid, sizeof (EFI_GUID)) == 0) {
          break;
        }
      }

      FvDevicePath = NextDevicePathNode (FvDevicePath);
    }

    if (!IsDevicePathEndType (FvDevicePath)) {
      break;
    }
  }

  if (Handles != NULL) {
    FreePool(Handles);
  }

  //
  // Find the network FV that has been dispatched
  //
  if (Index < HandleCount) {
    return TRUE;
  }

  return FALSE;
}


EFI_GUID gNetworkFvGuid = {0x771F5863, 0xCE95, 0x459C, {0x8D, 0xEA, 0xF5, 0xB2, 0x6B, 0xAB, 0x8D, 0xA3}};

VOID PlatformDxeReadyToBootHook()
{
  EFI_STATUS Status;
  VOID       *Buffer;
  VOID       *AlignedBuffer;
  UINTN      Size;
  EFI_HANDLE Handle;
  EFI_FIRMWARE_VOLUME_HEADER   *FwVolHeader;
  UINT32                       FvAlignment;
  STATIC BOOLEAN               IsFvDispatched = FALSE;
  UINTN                        VariableSize;
  UINT16                       BootCurrent;
  CHAR16                       BootOptionName[ARRAY_SIZE (L"Boot####")];
  EFI_BOOT_MANAGER_LOAD_OPTION BmBootOption;


  if (IsFvDispatched) {
    //
    // Network FV has been triged before.
    //
    return;
  }

  if (IsNetworkFvDispatch ()) {
    IsFvDispatched = TRUE;
    return;
  }

  VariableSize = sizeof BootCurrent;
  Status = gRT->GetVariable (EFI_BOOT_CURRENT_VARIABLE_NAME,
                  &gEfiGlobalVariableGuid, NULL /* Attributes */,
                  &VariableSize, &BootCurrent);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%a: failed to get %g:\"%s\": %r\n",
      gEfiCallerBaseName, __FUNCTION__, &gEfiGlobalVariableGuid,
      EFI_BOOT_CURRENT_VARIABLE_NAME, Status));
    return;
  }
  if (VariableSize != sizeof BootCurrent) {
    DEBUG ((DEBUG_ERROR, "%a:%a: got %Lu bytes for %g:\"%s\", expected %Lu\n",
      gEfiCallerBaseName, __FUNCTION__, (UINT64)VariableSize,
      &gEfiGlobalVariableGuid, EFI_BOOT_CURRENT_VARIABLE_NAME,
      (UINT64)sizeof BootCurrent));
    return;
  }

  //
  // Get the Boot#### option that the status code applies to.
  //
  UnicodeSPrint (BootOptionName, sizeof BootOptionName, L"Boot%04x",
    BootCurrent);
  Status = EfiBootManagerVariableToLoadOption (BootOptionName, &BmBootOption);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR,
      "%a:%a: EfiBootManagerVariableToLoadOption(\"%s\"): %r\n",
      gEfiCallerBaseName, __FUNCTION__, BootOptionName, Status));
    return;
  }

  //
  // Check whether it is network boot option
  //
  if (GetEfiBootGroupType (BmBootOption.FilePath) != BM_MENU_TYPE_UEFI_PXE) {
    EfiBootManagerFreeLoadOption (&BmBootOption);
    return;
  }
  EfiBootManagerFreeLoadOption (&BmBootOption);

  //
  // Dispatch Network Stack Drivers
  //
  IsFvDispatched = TRUE;

  Status = GetSectionFromAnyFv (
    &gNetworkFvGuid,
    EFI_SECTION_FIRMWARE_VOLUME_IMAGE,
    0,
    &Buffer,
    &Size
  );

  if (EFI_ERROR (Status)) {
    return;
  }

  FwVolHeader = (EFI_FIRMWARE_VOLUME_HEADER *) Buffer;
  FvAlignment = 1 << ((FwVolHeader->Attributes & EFI_FVB2_ALIGNMENT) >> 16);
  if (FvAlignment < 8) {
    FvAlignment = 8;
  }
  AlignedBuffer = AllocateAlignedPages (EFI_SIZE_TO_PAGES (Size), (UINTN) FvAlignment);
  if (AlignedBuffer == NULL) {
    FreePool (Buffer);
    return;
  }
  CopyMem (AlignedBuffer, Buffer, Size);
  FreePool (Buffer);

  Status = gDS->ProcessFirmwareVolume (AlignedBuffer, Size, &Handle);
  if (!EFI_ERROR (Status)) {
    Status = gDS->Dispatch ();
  }

  return;
}





EFI_STATUS
EFIAPI
PlatformDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS              Status;
  VOID                    *Registration;
  EFI_EVENT               Event;
  UINT8                   *DpxData = NULL;
  UINTN                   DpxDataSize;
  BOOLEAN                 NetStackHasDpx;


  DEBUG((DEBUG_INFO, "PlatformDxeEntry\n"));

  gHiiHandle = HiiAddPackages (
                 &gEfiCallerIdGuid,
                 ImageHandle,
                 PlatformDxeStrings,
                 NULL
                 );
  ASSERT (gHiiHandle != NULL);

  gSetupData = (CONST SETUP_DATA*)GetSetupDataHobData();
  gIsCmosBad = PcdGet8(PcdIsPlatformCmosBad);
  gCsmOn     = PcdGet8(PcdLegacyBiosSupport);
  gPlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  Status = LegacyBiosPlatformInstall(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);

  Status = LegacyRegion2Install(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);

  Status = PciPlatformInstall(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);

  Status = SataControllerInstall(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);

  Status = IncompatiblePciDeviceSupportEntryPoint(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);

  Status = SmmAccessInit(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);

  Status = MiscConfigDxe();
  ASSERT_EFI_ERROR(Status);

  EfiCreateProtocolNotifyEvent (
    &gBdsAllDriversConnectedProtocolGuid,
    TPL_CALLBACK,
    AllDriversConnectedCallBack,
    NULL,
    &Registration
    );

  EfiCreateProtocolNotifyEvent (
    &gEfiSmbiosProtocolGuid,
    TPL_CALLBACK,
    SmbiosCallback,
    NULL,
    &Registration
    );

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             PlatOnReadyToBoot,
             NULL,
             &Event
             );
  ASSERT_EFI_ERROR(Status);

  EfiCreateProtocolNotifyEvent (
    &gEfiVariableWriteArchProtocolGuid,
    TPL_CALLBACK,
    VariableWriteArchCallBack,
    NULL,
    &Registration
    );

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gPlatAfterConsoleStartProtocolGuid, PlatformAfterConsoleStartHook,
                  &gPlatAfterConsoleEndProtocolGuid, PlatformAfterConsoleEndHook,
                  &gPlatBeforeBiosUpdateProtocolGuid, PlatformBeforeBiosUpdateHook,
                  &gEfiBeforeConnectPciRootBridgeGuid, PlatformBeforeConnectPciRootBridgeHook,
                  &gEfiAfterConnectPciRootBridgeGuid, PlatformAfterConnectPciRootBridgeHook,
                  &gEfiAfterAllPciIoGuid, PlatformAfterAllPciIoHook,
                  NULL
                  );

  NetStackHasDpx = TRUE;
  Status = GetSectionFromAnyFv (
              &gNetworkFvGuid,
              EFI_SECTION_DXE_DEPEX,
              0,
              &DpxData,
              &DpxDataSize
              );
  if(EFI_ERROR(Status) || (DpxDataSize == 2 && DpxData[0] == EFI_DEP_TRUE && DpxData[1] == EFI_DEP_END)){
    NetStackHasDpx = FALSE;
  }
  if(DpxData != NULL){
    FreePool(DpxData);
  }

  DEBUG((DEBUG_INFO, "NetStackHasDpx:%d\n", NetStackHasDpx));
  if(NetStackHasDpx){
    if((gSetupData->LanBoot && gSetupData->UefiNetworkStack) && (gSetupData->BootModeType != BIOS_BOOT_LEGACY_OS)){
      PcdSetBoolS (PcdAlwaysCreatNetOption, TRUE);

      gBS->InstallProtocolInterface (
             &ImageHandle,
             &gEfiReadyToBootProtocolGuid,
             EFI_NATIVE_INTERFACE,
             (VOID*)(UINTN)PlatformDxeReadyToBootHook
             );
    }
  }

  return EFI_SUCCESS;
}

