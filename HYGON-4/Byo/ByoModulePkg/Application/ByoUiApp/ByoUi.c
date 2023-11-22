/** @file
  Byosoft Ui App module is driver for BDS phase.

 Copyright (c) 2010 - 2018, Byosoft Corporation.<BR>
 All rights reserved.This software and associated documentation (if any)
 is furnished under a license and may only be used or copied in
 accordance with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be reproduced,
 stored in a retrieval system, or transmitted in any form or by any
 means without the express written consent of Byosoft Corporation.

 File Name:

 Abstract:
    Byosoft Ui App module.

 Revision History:

**/

#include "ByoUi.h"
#include <SysMiscCfg.h>
#include <Library/PcdLib.h>

#define INIT_SETUP               0x00
#define RUN_SETUP                0x01
#define EXIT_SETUP               0x02

VOID ClearPendingKeys();

EFI_BYO_PLATFORM_SETUP_PROTOCOL    *gByoSetup = NULL;
EFI_GUID                           mUiAppVariableGuid = {0xcd4c9763, 0xa8c1, 0x4e5e, {0xb5, 0xf2, 0x46, 0x5b, 0x53, 0x89, 0x3d, 0xfc}};
EFI_HII_HANDLE                     gHandle = NULL;

VOID 
DumpHiiPackages (
  VOID
)
{
  EFI_STATUS                  Status;
  UINT8                       ClassGuidNum;
  EFI_GUID                    *ClassGuid;
  EFI_IFR_FORM_SET            *Buffer;
  UINTN                       BufferSize;
  UINT8                       *Ptr;
  UINTN                       TempSize;
  UINTN             Index;
  EFI_HII_HANDLE    *HiiHandles;
  EFI_STRING        TempString;

  HiiHandles = HiiGetHiiHandles (NULL);
  ASSERT (HiiHandles != NULL);

  //
  // Find HiiHandle to match the input FormSetGuid
  //
  for (Index = 0; HiiHandles[Index] != NULL; Index++) {

    Status = HiiGetFormSetFromHiiHandle(HiiHandles[Index], &Buffer,&BufferSize);
    if (EFI_ERROR (Status)) {
      continue;
    }

    TempSize = 0;
    Ptr = (UINT8 *) Buffer;
    while(TempSize < BufferSize)  {
      TempSize += ((EFI_IFR_OP_HEADER *) Ptr)->Length;

      if (((EFI_IFR_OP_HEADER *) Ptr)->Length <= OFFSET_OF (EFI_IFR_FORM_SET, Flags)){
        Ptr += ((EFI_IFR_OP_HEADER *) Ptr)->Length;
        continue;
      }

      //
      // Check formset opcode
      //
      if (((EFI_IFR_OP_HEADER *) Ptr)->OpCode != EFI_IFR_FORM_SET_OP) {
        Ptr += ((EFI_IFR_OP_HEADER *) Ptr)->Length;
        continue;
      }

      DEBUG ((DEBUG_INFO, "FormSet Guid is %g\n", &(((EFI_IFR_FORM_SET *)Ptr)->Guid)));
      TempString = HiiGetString (HiiHandles[Index], ((EFI_IFR_FORM_SET *)Ptr)->FormSetTitle, "en-US");
      if (TempString != NULL) {
        DEBUG ((DEBUG_INFO, "FormSet Tile is %s\n", TempString));
        FreePool (TempString);
      }
      TempString = ByoUiAppExtractDevicePathFromHiiHandle (HiiHandles[Index]);
      if (TempString != NULL) {
        DEBUG ((DEBUG_INFO, "Device Path is %s\n", TempString));
        FreePool (TempString);
      }

      ClassGuidNum = (UINT8) (((EFI_IFR_FORM_SET *)Ptr)->Flags & 0x3);
      ClassGuid = (EFI_GUID *) (VOID *)(Ptr + sizeof (EFI_IFR_FORM_SET));
      while (ClassGuidNum-- > 0) {
        DEBUG ((DEBUG_INFO, "  Class Guid is %g\n", ClassGuid));
        ClassGuid ++;
      }

      Ptr += ((EFI_IFR_OP_HEADER *) Ptr)->Length;
    }

    FreePool (Buffer);
  }

  FreePool (HiiHandles);

}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the image goes into a library that calls this
  function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeByoUI (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;
  EFI_HANDLE    Handle;
  EFI_BOOT_LOGO_PROTOCOL    *BootLogo;
  EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
  UINT32                    SystemMiscCfg;
  EFI_HANDLE                DeviceHandle = NULL;

  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);
  if(!(SystemMiscCfg & SYS_MISC_CFG_CONNECT_ALL)){
    //
    // Indicate if the connect all has been performed before.
    // If has not been performed before, do here.
    //
    SystemMiscCfg = SystemMiscCfg | SYS_MISC_CFG_CONNECT_ALL;
    PcdSet32S(PcdSystemMiscConfig, SystemMiscCfg);
    EfiBootManagerConnectAll ();
  }
  if(!(SystemMiscCfg & SYS_MISC_CFG_UPD_BOOTORDER)){
    //
    // The boot option enumeration time is acceptable in Ui driver
    //
    SystemMiscCfg = SystemMiscCfg | SYS_MISC_CFG_UPD_BOOTORDER;
    PcdSet32S(PcdSystemMiscConfig, SystemMiscCfg);
    EfiBootManagerRefreshAllBootOption ();
  }

  DEBUG ((EFI_D_INFO, "InitializeByoUI, \n"));
  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);

  //
  // Get our image information
  //
  Status = gBS->OpenProtocol (
                  ImageHandle,
                  &gEfiLoadedImageProtocolGuid,
                  (VOID **) &LoadedImage,
                  ImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // If the option data is the guid, then signal it. 
  //
  if (LoadedImage->LoadOptionsSize == sizeof (EFI_GUID)) {
    EfiEventGroupSignal ((CONST EFI_GUID *)LoadedImage->LoadOptions);
  }

  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiFrb2WatchDogNotifyGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  Status = gBS->UninstallProtocolInterface (
                  Handle,
                  &gEfiFrb2WatchDogNotifyGuid,
                  NULL
                  );  

  //
  //Signal EnterSetupEvent to load all resource.
  //
  DEBUG((EFI_D_INFO, "InitializeByoUI, Signal Enter Setup Event, \n"));
  Handle = NULL;
  if (PcdGet8 (PcdUiAppState) == INIT_SETUP) {
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiSetupEnterGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );
    Status = gBS->UninstallProtocolInterface (
                    Handle,
                    &gEfiSetupEnterGuid,
                    NULL
                    );
    REPORT_STATUS_CODE (EFI_PROGRESS_CODE, EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_PC_USER_SETUP);

    //
    // Dump Hii Package
    //
    DEBUG_CODE (
      DumpHiiPackages();
    );
  }

  //
  //Enter Setup.
  //
  UiEntry (FALSE);

  gHandle = HiiAddPackages (
              &gEfiCallerIdGuid,
              DeviceHandle,
              STRING_ARRAY_NAME,
              NULL
              );
  ASSERT (gHandle != NULL);
 
  Status = AddDynamicFormset (
      NULL, 
      &gEfiFormsetGuidBoot, 
      &gEfiIfrByoBootUiPageGuid,
      ROOT_FORM_ID,
      LABEL_CHANGE_BOOT_ORDER
      );
  DMSG((EFI_D_ERROR, "InstallFormset, Add Boot Dynamic FormSet :0x%x-%r.\n", LABEL_CHANGE_BOOT_ORDER, Status));

  //
  // Clear Background.
  //
  if (!PcdGetBool (PcdEarlyVideoSupport)) {
    gST->ConOut->ClearScreen (gST->ConOut);
  }

  //
  // Boot Logo is corrupted, report it using Boot Logo protocol.
  //
  Status = gBS->LocateProtocol (&gEfiBootLogoProtocolGuid, NULL, (VOID **) &BootLogo);
  if (!EFI_ERROR (Status) && (BootLogo != NULL)) {
    BootLogo->SetBootLogo (BootLogo, NULL, 0, 0, 0, 0);
  }

  ClearPendingKeys();

  if (gByoSetup == NULL) {
    //
    //Locate Byo Platform Setup Protocol.
    //
    Status = gBS->LocateProtocol (
                    &gEfiByoPlatformSetupGuid,
                    NULL,
                    (VOID**)&gByoSetup
                    );
    DMSG((EFI_D_INFO, "UiEntry, locate Byo Setup :%r.\n", Status));
    ASSERT_EFI_ERROR (Status);
  }

  gByoSetup->Run (gByoSetup);

  //
  // Remove install HII packages
  //
  RemoveHiiResource ();

  PcdSet8S (PcdUiAppState, EXIT_SETUP);

  return EFI_SUCCESS;
}

VOID ProjectReadyToBootHook()
{
  EFI_HANDLE Handle;

  if (PcdGet8 (PcdUiAppState) == INIT_SETUP) {
    //
    // Signal EnterSetupEvent to load all resource.
    //
    Handle = NULL;
    gBS->InstallProtocolInterface (
                    &Handle,
                    &gEfiSetupEnterGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );

    gBS->UninstallProtocolInterface (
                    Handle,
                    &gEfiSetupEnterGuid,
                    NULL
                    );
  }

  UiEntry (FALSE);
}

/**
  The user Entry Point for Application. The user code starts with this function
  as the real entry point for the image goes into a library that calls this
  function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
InitializeByoDxe (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gEfiReadyToBootProtocolGuid, ProjectReadyToBootHook,
                  NULL
                  );

  return Status;
}

VOID ClearPendingKeys()
{
  EFI_INPUT_KEY  Key;
  UINTN          Count = 0;

  while(1){
    Count = 0;
    if(gST->ConIn->ReadKeyStroke(gST->ConIn, &Key) != EFI_SUCCESS){
      break;
    }
    Count++;
    if(Count >= 100){
      break;
    }
  };
}

/**
  Platform Setup to trig exit UiApp
**/
VOID
EFIAPI
UiAppPlatformReset (VOID)
{
  UINTN         OemPlatformResetFuncAddr;
  UINTN         VarSize;
  EFI_STATUS    Status;

  RemoveHiiResource ();

  VarSize = sizeof (OemPlatformResetFuncAddr);
  Status  = gRT->GetVariable (
                  L"OemPlatformReset",
                  &mUiAppVariableGuid,
                  NULL,
                  &VarSize,
                  &OemPlatformResetFuncAddr
                  );

  if (!EFI_ERROR (Status) && OemPlatformResetFuncAddr != 0) {
    ((OEM_PLATFORM_RESET) OemPlatformResetFuncAddr) ();
  }

  // reset system after exit setup
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
}

/**
  This function is the main entry of the UI entry.
  The function will present the main menu of the system UI.

  @param ConnectAllHappened Caller passes the value to UI to avoid unnecessary connect-all.

**/
VOID
EFIAPI
UiEntry (
  IN BOOLEAN                      ConnectAllHappened
  )
{
  EFI_STATUS                    Status;
  UINTN                         OemPlatformResetFuncAddr;
  EFI_BYO_FORM_BROWSER_EXTENSION_PROTOCOL *ByoFormBrowserExtProtocol;

  //
  // Has been in RUN_SETUP state
  //
  if (PcdGet8 (PcdUiAppState) == RUN_SETUP) {
    return;
  }

  DEBUG ((EFI_D_INFO, "UiEntry, \n"));

  //
  //Locate Byo Platform Setup Protocol.
  //
  Status = gBS->LocateProtocol (
                  &gEfiByoPlatformSetupGuid,
                  NULL,
                  (VOID**)&gByoSetup
                  );
  DMSG((EFI_D_INFO, "UiEntry, locate Byo Setup :%r.\n", Status));
  ASSERT_EFI_ERROR (Status);

  //
  //Locate Byo Platform Setup Protocol and Set UiAppPlatformReset
  //
  Status = gBS->LocateProtocol (
                  &gEfiByoFormBrowserExProtocolGuid,
                  NULL,
                  (VOID**)&ByoFormBrowserExtProtocol
                  );
  DMSG((EFI_D_INFO, "UiEntry, locate Byo From Browser Ext :%r.\n", Status));
  ASSERT_EFI_ERROR (Status);

  if (PcdGet8 (PcdUiAppState) == INIT_SETUP) {
    //
    // Save original OemPlatformReset function pointer
    //
    OemPlatformResetFuncAddr = (UINTN) ByoFormBrowserExtProtocol->PlatformReset;
    gRT->SetVariable (
            L"OemPlatformReset",
            &mUiAppVariableGuid,
            EFI_VARIABLE_BOOTSERVICE_ACCESS,
            sizeof (UINTN),
            &OemPlatformResetFuncAddr
            );
  }
  ByoFormBrowserExtProtocol->PlatformReset = UiAppPlatformReset;

  //
  // Prepare HII resources
  //
  AddAllFormset ();

  gByoSetup->InitializeMainFormset (gByoSetup);

  PcdSet8S (PcdUiAppState, RUN_SETUP);
}
