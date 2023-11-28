

#include <Uefi.h>
#include <PiDxe.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/FileInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePath.h>
#include <Protocol/SimpleFileSystem.h>
#include <Protocol/SimpleTextOut.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/ScsiIo.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/BlockIo.h>
#include <Library/ByoCommLib.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <Library/SetupUiLib.h>
#include <Protocol/FileExplorer.h>
#include <Library/PlatformCommLib.h>
#include <SetupVariable.h>



EFI_HII_HANDLE gHiiHandle;

#if 0
EFI_STATUS 
ReadFileFromUsb (
  CHAR16  *Name,
  UINT8   **FileData,
  UINTN   *FileSize
  )
{
  EFI_STATUS                       Status;
  UINTN                            Index;
  UINTN                            HandleCount;
  EFI_HANDLE                       *Handles = NULL;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FS;  
  EFI_FILE_PROTOCOL                *RootFile = NULL;
  EFI_DEVICE_PATH_PROTOCOL         *DevPath;
  BOOLEAN                          IsUsb;

  
  Status = gBS->LocateHandleBuffer (
                 ByProtocol,
                 &gEfiSimpleFileSystemProtocolGuid,
                 NULL,
                 &HandleCount,
                 &Handles
                 );
  if(EFI_ERROR(Status) || HandleCount == 0){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }  
  
  for (Index = 0; Index < HandleCount; Index++) {
    IsUsb = FALSE;
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&DevPath
                    );    
    if(!EFI_ERROR(Status)){
      while (!IsDevicePathEnd(DevPath)) {
        if ((DevicePathType(DevPath) == MESSAGING_DEVICE_PATH) &&
            (DevicePathSubType(DevPath) == MSG_USB_DP)) {
          IsUsb = TRUE;    
          break;
        }
        DevPath = NextDevicePathNode(DevPath);
      }    
    }

    DEBUG((EFI_D_INFO, "FS[%d]: IsUsb:%d\n", Index, IsUsb));
    if(!IsUsb){
      continue;
    }
    
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiSimpleFileSystemProtocolGuid,
                    (VOID**)&FS
                    );
    ASSERT(!EFI_ERROR (Status));

    if(RootFile != NULL){
      RootFile->Close(RootFile);
      RootFile = NULL;
    }  
  	Status = FS->OpenVolume(FS, &RootFile);
  	if(EFI_ERROR(Status)){
      DEBUG((EFI_D_ERROR, "OpenRootFile Error:%r\n", Status));
      continue;
    }  

    Status = RootFileReadFile(gBS, RootFile, Name, FileData, FileSize);
  	if(!EFI_ERROR(Status)){
      break;
    } 
  }

  if(Index >= HandleCount){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  } 

ProcExit:
  if(Handles != NULL){
    FreePool(Handles);
  }  
  if(RootFile != NULL){
    RootFile->Close(RootFile);
  } 
  return Status;
}
#endif


VOID ShowFileNotFound()
{
  CHAR16  *Str;

  Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_FILE_NOT_FOUND), NULL);
  UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
  FreePool(Str);
}


VOID ShowFileNotMatch()
{
  CHAR16  *Str;

  Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_FILE_NOT_MATCH), NULL);
  UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
  FreePool(Str);
}


EFI_STATUS 
LoadFileByDp (
  EFI_DEVICE_PATH_PROTOCOL *Dp,
  UINT8                    **FileData,
  UINTN                    *FileSize  
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL  *FS; 
  EFI_FILE_PROTOCOL                *RootFile = NULL;
  CHAR16                           *FileName;


  ShowDevicePathDxe(gBS, Dp);

  Status = gBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid, &Dp, &Handle);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  
  
  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiSimpleFileSystemProtocolGuid,
                  (VOID**)&FS
                  );
  ASSERT(!EFI_ERROR(Status));
  
	Status = FS->OpenVolume(FS, &RootFile);
	if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "OpenRootFile Error:%r\n", Status));
    goto ProcExit;
  }  

  if(DevicePathType(Dp) == MEDIA_DEVICE_PATH && DevicePathSubType(Dp) == MEDIA_FILEPATH_DP){
    FileName = ((FILEPATH_DEVICE_PATH*)Dp)->PathName;
  } else {
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  Status = RootFileReadFile(gBS, RootFile, FileName, FileData, FileSize);

ProcExit:
  if(RootFile != NULL){
    RootFile->Close(RootFile);
  }   
  return Status;
}



VOID BiosUpdate(VOID)
{
  EFI_STATUS                  Status;
  UINT8                       *FileData = NULL;
  UINTN                       FileSize;
  BYO_SMIFLASH_PROTOCOL       *ByoSmiFlash;
  UINTN                       Index;
  CHAR16                      *Str; 
  CHAR16                      *Str2;
  CHAR16                      *Str3;
  SELECTION_TYPE              Choice;
  EFI_FILE_EXPLORER_PROTOCOL  *FileExplorer;
  EFI_DEVICE_PATH_PROTOCOL    *FileDp = NULL;
  UINTN                       Dummy;                 // wz200707 +
  UINTN                       *HotkeyDis = 0;        // wz200707 +
  SETUP_DATA                  SetupData;
  UINTN                       Size;
 
#if 0
  Status = ReadFileFromUsb(L"BIOS.BIN", &FileData, &FileSize);
  if(EFI_ERROR(Status)){
    ShowFileNotFound();
    goto ProcExit;
  }
#endif

Size = sizeof(SETUP_DATA);
Status = gRT->GetVariable (
				PLATFORM_SETUP_VARIABLE_NAME,
				&gPlatformSetupVariableGuid,
				NULL,
				&Size,
				&SetupData
				);


// wz200707 + >>  
  Status = gBS->LocateProtocol(&gByoSetupHotKeyDisableProtocolGuid, NULL, (VOID**)&HotkeyDis);
  if(EFI_ERROR(Status)){
    HotkeyDis = &Dummy;
  }

  *HotkeyDis = BIT0;
// wz200707 + <<

  Status = gBS->LocateProtocol(&gEfiFileExplorerProtocolGuid, NULL, &FileExplorer);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  
  Status = FileExplorer->ChooseFile(NULL, L".bin", NULL, &FileDp);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  Status = LoadFileByDp(FileDp, &FileData, &FileSize);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  Status = gBS->LocateProtocol (
                  &gByoSmiFlashProtocolGuid,
                  NULL,
                  (VOID**)&ByoSmiFlash
                  );
  ASSERT(!EFI_ERROR(Status));

  Status = ByoSmiFlash->BiosCheck(ByoSmiFlash, FileData, FileSize, NULL);
  if(EFI_ERROR(Status)){
    ShowFileNotMatch();
    goto ProcExit;    
  }

  Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATE_BIOS_WARN), NULL);
  Str2 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATE_BIOS_WARN2), NULL);
  Str3 = HiiGetString(gHiiHandle, STRING_TOKEN(STR_UPDATE_BIOS_WARN3), NULL);
  Choice = UiConfirmDialog(DIALOG_YESNO, NULL, NULL, TEXT_ALIGIN_CENTER, Str, Str2, Str3, NULL);
  FreePool(Str);
  FreePool(Str2);
  FreePool(Str3);
  if (Choice != SELECTION_YES) {
    goto ProcExit;
  }

  if(SetupData.BiosUpdateParm == 1){
    PcdSetBool(PcdUpdateAllBiosEnable, TRUE);
  }
  Status = ByoSmiFlash->DefaultUpdate(ByoSmiFlash, FileData, FileSize);
  if(Status != EFI_SUCCESS) {
  	if(Status == EFI_UNSUPPORTED) {
		    gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_RED);
			  Print (L"\r NOT Find BIOS!\n");
      	Print (L"\r BIOS Update Exit.\n");
		    gBS->Stall (5000000);
   		  goto ProcExit;
  		}
	  if(Status == EFI_ACCESS_DENIED) {
		    gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_RED);
			  Print (L"\r BIOS Verify Sign Error!\n");
        Print (L"\r BIOS Update Exit.\n");
		    gBS->Stall (5000000);
   		  goto ProcExit;
  		}
    gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_RED);
    Print(L"Flash Update Error!");
    Print(L"Press power button to Shutdown system...\n");
    CpuDeadLoop();
  } else {
    gST->ConOut->SetAttribute (gST->ConOut, EFI_BACKGROUND_BLACK | EFI_YELLOW);
    Print(L"Flash is updated successfully!\n\n");
	//CmosWrite(0xD0,1); //update bios in setup set load default flag  XCL<< - 21 01 16
    for (Index = 5; Index > 0; Index--) {
      Print (L"\rSystem will reset in %d second(s)", Index);
      gBS->Stall (1000000);
    }
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);    
  }

ProcExit:
  *HotkeyDis = 0;                                             // wz200707 +
  if(FileDp != NULL){
    FreePool(FileDp);
  }
  if(FileData != NULL){
    FreePool(FileData);
  }
  DEBUG((EFI_D_INFO, "%a %r\n", __FUNCTION__, Status));
  return;
}





EFI_STATUS
BiosUpdateEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;

  gHiiHandle = HiiAddPackages (
                &gEfiCallerIdGuid,
                ImageHandle,
                STRING_ARRAY_NAME,
                NULL
                );
  ASSERT(gHiiHandle != NULL);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gByoSetupBiosUpdateProtocolGuid, BiosUpdate,
                  NULL
                  );
  ASSERT(!EFI_ERROR(Status));
  
  return Status;
}


