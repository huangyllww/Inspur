
#include "Bds.h"
#include <Protocol/ByoFormSetManager.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Library/HobLib.h>
#include <Library/PciLib.h>
#include "BdsPlatform.h"
#include <Library/ReportStatusCodeLib.h>
#include <ByoStatusCode.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <Token.h>
#include <SystemPasswordVariable.h>
#include <Library/PcdLib.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/Spi.h>
#include <Library/IpmiBaseLib.h>
#include <Library/LogoLib.h>
#include <SetupVariable.h>
#include <Library/SetupUiLib.h>
#include <Library/PlatformLanguageLib.h>
#include <AmdCpmDxe.h>



extern EFI_BDS_BOOT_MANAGER_PROTOCOL gBdsBootManagerProtocol;
EFI_SPI_PROTOCOL  *pSpiProtocol=NULL;
#define BMC_16BYTE                   16

EFI_STATUS
CreateFvBootOption (
  EFI_GUID                     *FileGuid,
  CHAR16                       *Description,
  EFI_BOOT_MANAGER_LOAD_OPTION *BootOption,
  BOOLEAN                      IsBootCategory,
  UINT8                        *OptionalData,    OPTIONAL
  UINT32                       OptionalDataSize
  );

UINTN ByoEfiBootManagerGetCurHotKey(VOID);
VOID  ByoEfiBootManagerSetCurHotKey(UINTN HotKey);

EFI_STATUS ByoEfiBootManagerBootApp(IN EFI_GUID *AppGuid);
VOID UpdateSmbiosType11();

EFI_STATUS AddSmbiosType24();
EFI_STATUS AddSmbiosType39();

VOID SendToBmcConfig();
VOID BootBmcNextBoot();


EFI_BDS_ARCH_PROTOCOL  gBds = {
  BdsEntry
};


EFI_HII_HANDLE gHiiHandle;


/**

  Install Boot Device Selection Protocol

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval  EFI_SUCEESS  BDS has finished initializing.
                        Return the dispatcher and recall BDS.Entry
  @retval  Other        Return status from AllocatePool() or gBS->InstallProtocolInterface

**/
EFI_STATUS
EFIAPI
BdsInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;

  gHiiHandle = HiiAddPackages (
                 &gEfiCallerIdGuid,
                 gImageHandle,
                 BdsDxeStrings,
                 NULL
                 );
  ASSERT (gHiiHandle != NULL);

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiBdsArchProtocolGuid, &gBds,
                  &gEfiBootManagerProtocolGuid, &gBdsBootManagerProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}




/**
  Generic function to update the EFI  variables.

  @param VariableName    The name of the variable to be updated.
  @param ProcessVariable The function pointer to update the variable.
                         NULL means to restore to the original value.
**/
VOID
BdsUpdateVariable (
  CHAR16               *VariableName,
  BDS_PROCESS_VARIABLE ProcessVariable,
  EFI_GUID             *VendorGuid
  )
{
  EFI_STATUS  Status;
  CHAR16      BackupVariableName[20];
  CHAR16      FlagVariableName[20];
  VOID        *Variable;
  VOID        *BackupVariable;
  VOID        *NewVariable;
  UINTN       VariableSize;
  UINTN       BackupVariableSize;
  UINTN       NewVariableSize;
  BOOLEAN     Flag;
  BOOLEAN     *FlagVariable;
  UINTN       FlagSize;

  ASSERT (StrLen (VariableName) <= 13);
  UnicodeSPrint (BackupVariableName, sizeof (BackupVariableName), L"%sBackup", VariableName);
  UnicodeSPrint (FlagVariableName, sizeof (FlagVariableName), L"%sModify", VariableName);

  Variable       = EfiBootManagerGetVariableAndSize (VariableName, &gEfiGlobalVariableGuid, &VariableSize);
  BackupVariable = EfiBootManagerGetVariableAndSize (BackupVariableName, &gEfiCallerIdGuid, &BackupVariableSize);
  FlagVariable   = EfiBootManagerGetVariableAndSize (FlagVariableName, &gEfiCallerIdGuid, &FlagSize);
  if ((ProcessVariable != NULL) && (FlagVariable == NULL)) {
    //
    // Current boot is a modified boot and last boot is a normal boot
    // Set flag to indicate it's a modified boot
    // BackupVariable <- Variable
    // Variable       <- ProcessVariable (Variable)
    //
    Flag   = TRUE;
    Status = gRT->SetVariable (
                    FlagVariableName,
                    &gEfiCallerIdGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    sizeof (Flag),
                    &Flag
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gRT->SetVariable (
                    BackupVariableName,
                    &gEfiCallerIdGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VariableSize,
                    Variable
                    );
    ASSERT ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND));


    NewVariable     = Variable;
    NewVariableSize = VariableSize;
    ProcessVariable (&NewVariable, &NewVariableSize);

    Status = gRT->SetVariable (
                    VariableName,
                    VendorGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    NewVariableSize,
                    NewVariable
                    );
    ASSERT ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND));

    if (NewVariable != NULL) {
      FreePool (NewVariable);
    }
  } else if ((ProcessVariable == NULL) && (FlagVariable != NULL)) {
    //
    // Current boot is a normal boot and last boot is a modified boot
    // Clear flag to indicate it's a normal boot
    // Variable       <- BackupVariable
    // BackupVariable <- NULL
    //
    Status = gRT->SetVariable (
                    FlagVariableName,
                    &gEfiCallerIdGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gRT->SetVariable (
                    VariableName,
                    VendorGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    BackupVariableSize,
                    BackupVariable
                    );
    ASSERT ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND));

    Status = gRT->SetVariable (
                    BackupVariableName,
                    &gEfiCallerIdGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    0,
                    NULL
                    );
    ASSERT ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND));
  }

  if (Variable != NULL) {
    FreePool (Variable);
  }

  if (BackupVariable != NULL) {
    FreePool (BackupVariable);
  }

  if (FlagVariable != NULL) {
    FreePool (FlagVariable);
  }
}





BOOLEAN 
IsThisLegacyBootDevChanged (
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  );

VOID
BootAllBootOptions (
  IN EFI_BOOT_MANAGER_LOAD_OPTION    *BootOptions,
  IN UINTN                           BootOptionCount
  )
{
  UINTN                              Index;

  for (Index = 0; Index < BootOptionCount; Index++) {
    if ((BootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      DEBUG((EFI_D_INFO, "NotActived\n"));
      continue;
    }

//    if(IsThisLegacyBootDevChanged(&BootOptions[Index])){
//      DEBUG((EFI_D_ERROR, "LegacyBootDevChanged, Need Refresh!\n"));		
//      break;		
//    }	
    ByoEfiBootManagerBoot(&BootOptions[Index]);
//  REPORT_STATUS_CODE(EFI_ERROR_CODE, BSC_BOOT_DEVICE_NOT_FOUND); 
    DEBUG((EFI_D_INFO, "boot:%r\n", BootOptions[Index].Status));
  }
}

/**
  This function attempts to boot per the boot order specified by platform policy.

  If the boot via Boot#### returns with a status of EFI_SUCCESS the boot manager will stop 
  processing the BootOrder variable and present a boot manager menu to the user. If a boot via 
  Boot#### returns a status other than EFI_SUCCESS, the boot has failed and the next Boot####
  in the BootOrder variable will be tried until all possibilities are exhausted.
                                  -- Chapter 3.1.1 Boot Manager Programming, the 4th paragraph
**/
VOID
DefaultBootBehavior (
  VOID
  )
{
  UINTN                         BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  CHAR16                        *Str;
  EFI_STATUS    Status;
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBootMgr;

  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
  BootAllBootOptions(BootOptions, BootOptionCount);
  ByoEfiBootManagerFreeLoadOptions(BootOptions, BootOptionCount);
  
//ByoEfiBootManagerConnectAll();
//ByoEfiBootManagerRefreshAllBootOption();
//BootOptions = EfiBootManagerGetLoadOptions(&BootOptionCount, LoadOptionTypeBoot);	
//BootAllBootOptions (BootOptions, BootOptionCount);

  //
  // Default to Setup.
  //
  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  if(!EFI_ERROR(Status)){      
    BdsBootMgr->BootApp (&gSetupFileGuid);
  } 

  REPORT_STATUS_CODE(EFI_ERROR_CODE, BSC_BOOT_DEVICE_NOT_FOUND); 
  InvokeHookProtocol(gBS, &gByoNoBootDeviceProtocolGuid);

  gST->ConOut->ClearScreen(gST->ConOut);
  gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  Print(L"\n\n");

  Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_NO_BOOT_DEVICE), NULL);
  Print(L"  %s", Str);
  FreePool(Str);
  CpuDeadLoop();
}




/**
  The function will go through the driver option link list, load and start
  every driver the driver option device path point to.

  @param  BdsDriverLists        The header of the current driver option link list

**/
VOID
LoadDrivers (
  IN EFI_BOOT_MANAGER_LOAD_OPTION       *DriverOption,
  IN UINTN                              DriverOptionCount
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  EFI_HANDLE                ImageHandle;
  EFI_LOADED_IMAGE_PROTOCOL *ImageInfo;
  BOOLEAN                   ReconnectAll;

  ReconnectAll = FALSE;

  //
  // Process the driver option
  //
  for (Index = 0; Index < DriverOptionCount; Index++) {
    //
    // If a load option is not marked as LOAD_OPTION_ACTIVE,
    // the boot manager will not automatically load the option.
    //
    if ((DriverOption[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      continue;
    }

    //
    // If a driver load option is marked as LOAD_OPTION_FORCE_RECONNECT,
    // then all of the EFI drivers in the system will be disconnected and
    // reconnected after the last driver load option is processed.
    //
    if ((DriverOption[Index].Attributes & LOAD_OPTION_FORCE_RECONNECT) != 0) {
      ReconnectAll = TRUE;
    }

    //
    // Make sure the driver path is connected.
    //
    EfiBootManagerConnectDevicePath (DriverOption[Index].FilePath, NULL);

    //
    // Load and start the image that Driver#### describes
    //
    Status = gBS->LoadImage (
                    FALSE,
                    gImageHandle,
                    DriverOption[Index].FilePath,
                    NULL,
                    0,
                    &ImageHandle
                    );

    if (!EFI_ERROR (Status)) {
      gBS->HandleProtocol (ImageHandle, &gEfiLoadedImageProtocolGuid, (VOID **) &ImageInfo);

      //
      // Verify whether this image is a driver, if not,
      // exit it and continue to parse next load option
      //
      if (ImageInfo->ImageCodeType != EfiBootServicesCode && ImageInfo->ImageCodeType != EfiRuntimeServicesCode) {
        gBS->Exit (ImageHandle, EFI_INVALID_PARAMETER, 0, NULL);
        continue;
      }

      ImageInfo->LoadOptionsSize  = DriverOption[Index].OptionalDataSize;
      ImageInfo->LoadOptions      = DriverOption[Index].OptionalData;
      //
      // Before calling the image, enable the Watchdog Timer for
      // the 5 Minute period
      //
      gBS->SetWatchdogTimer (5 * 60, 0x0000, 0x00, NULL);

      DriverOption[Index].Status = gBS->StartImage (ImageHandle, &DriverOption[Index].ExitDataSize, &DriverOption[Index].ExitData);
      DEBUG ((DEBUG_INFO | DEBUG_LOAD, "Driver Return Status = %r\n", DriverOption[Index].Status));

      //
      // Clear the Watchdog Timer after the image returns
      //
      gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
    }
  }

  //
  // Process the LOAD_OPTION_FORCE_RECONNECT driver option
  //
  if (ReconnectAll) {
    EfiBootManagerDisconnectAll ();
    ByoEfiBootManagerConnectAll ();
  }

}





#if !defined(MDEPKG_NDEBUG)
STATIC VOID DumpBootOptions(VOID)
{
  EFI_BOOT_MANAGER_LOAD_OPTION    *Option;
  UINTN                           BootOptionCount;
  UINTN                           i;

  DEBUG ((EFI_D_INFO, "=============Dumping Boot Options==============\n"));

  Option = EfiBootManagerGetLoadOptions(&BootOptionCount, LoadOptionTypeBoot);
  for (i = 0; i < BootOptionCount; i++) {
    DEBUG((EFI_D_INFO, "Boot%04X: A:%08X %s\n", Option[i].OptionNumber, Option[i].Attributes, Option[i].Description));
  }
  ByoEfiBootManagerFreeLoadOptions (Option, BootOptionCount);

  DEBUG ((EFI_D_INFO, "===================== END =====================\n"));
}
#endif



STATIC
EFI_STATUS
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

STATIC
EFI_STATUS
ByoEfiBootManagerCreateSetupBootOption (
  OUT EFI_BOOT_MANAGER_LOAD_OPTION   *SetupOption
)
{
  EFI_STATUS  Status;

  if(SetupOption == NULL){
    return EFI_INVALID_PARAMETER;
  }

  Status = CreateFvBootOption (
             &gSetupFileGuid,
             L"Enter Setup",
             SetupOption,
             FALSE,             // IsBootCategory
             NULL,              // OptionalData
             0                  // OptionalDataSize
             );

  return Status;
}



STATIC
EFI_STATUS
ByoEfiBootManagerCreateShellBootOption (
  OUT EFI_BOOT_MANAGER_LOAD_OPTION   *ShellOption
)
{
  EFI_STATUS  Status;

  if(ShellOption == NULL){
    return EFI_INVALID_PARAMETER;
  }

  Status = CreateFvBootOption (
             (EFI_GUID*)PcdGetPtr(PcdShellFile),
             L"Internal EDK Shell",
             ShellOption,
             FALSE,             // IsBootCategory
             NULL,              // OptionalData
             0                  // OptionalDataSize
             );

  return Status;
}




EFI_STATUS ByoEfiBootManagerBootApp(IN EFI_GUID *AppGuid)
{
  EFI_BOOT_MANAGER_LOAD_OPTION   BootOption;

  ZeroMem(&BootOption, sizeof(BootOption));
  BootOption.Status = EFI_INVALID_PARAMETER;

  CreateFvBootOption(AppGuid, L"", &BootOption, FALSE, NULL, 0);
  ByoEfiBootManagerBoot(&BootOption);
  EfiBootManagerFreeLoadOption(&BootOption);

  return BootOption.Status;
}

EFI_STATUS 
ByoEfiBootManagerBootAppParam (
  IN  EFI_GUID            *AppGuid,
  IN  VOID                *Parameter,
  IN  UINTN               ParameterSize,
  IN  BOOLEAN             IsBootCategory  
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION   BootOption;

  ZeroMem(&BootOption, sizeof(BootOption));
  BootOption.Status = EFI_INVALID_PARAMETER;

  CreateFvBootOption(AppGuid, L"", &BootOption, IsBootCategory, (UINT8*)Parameter, (UINT32)ParameterSize);
  ByoEfiBootManagerBoot(&BootOption);
  EfiBootManagerFreeLoadOption(&BootOption);

  return BootOption.Status;
}

EFI_STATUS 
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




VOID ByoEfiBootManagerConnectAllDummy (VOID)
{
  DEBUG((EFI_D_ERROR, "BootManager Dummy\n"));
}

VOID ByoEfiBootManagerRefreshAllBootOptionDummy (VOID)
{
  DEBUG((EFI_D_ERROR, "BootManager Dummy\n"));
}

VOID ByoEfiBootManagerBootDummy (
  IN  EFI_BOOT_MANAGER_LOAD_OPTION     *BootOption
  )
{
  DEBUG((EFI_D_ERROR, "BootManager Dummy\n"));
}

EFI_STATUS ByoEfiBootManagerBootAppDummy(IN EFI_GUID *AppGuid)
{
  DEBUG((EFI_D_ERROR, "BootManager Dummy\n"));
  return EFI_NOT_READY;
}

EFI_STATUS ByoEfiBootManagerBootSetupDummy(EFI_BDS_BOOT_MANAGER_PROTOCOL *This)
{
  DEBUG((EFI_D_ERROR, "BootManager Dummy\n"));
  return EFI_NOT_READY;
}

EFI_STATUS ByoEfiBootManagerBootAppParameterDummy(EFI_GUID *AppGuid, VOID *Param, UINTN ParamSize, BOOLEAN IsBootCategory)
{
  DEBUG((EFI_D_ERROR, "BootManager Dummy\n"));
  return EFI_NOT_READY;
}

EFI_STATUS ByoEfiBootManagerBootFileParameterDummy(EFI_DEVICE_PATH_PROTOCOL *FileDp, VOID *Param, UINTN ParamSize, BOOLEAN IsBootCategory)
{
  DEBUG((EFI_D_ERROR, "BootManager Dummy\n"));
  return EFI_NOT_READY;
}



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
  UINTN                    SataHostIndex, SataPortIndex, PhysicPortIndex;
  BOOLEAN                  HasSet = FALSE;
  EFI_PCI_IO_PROTOCOL            *ptPciIo;
  EFI_HANDLE                     PciHandle;
  UINTN 						       Seg,Bus,Dev,Func;
  BOOLEAN                    IsOsCreat=FALSE;
  EFI_DEVICE_PATH_PROTOCOL   *OsDp;
  

  DEBUG((EFI_D_INFO, "%a(%s)\n", __FUNCTION__, Name));

  *Updated = FALSE;
  return Name; 
  if(DevicePathType(Dp) == BBS_DEVICE_PATH || PcdGet8(PcdBdsBootOrderUpdateMethod) == 1){
    return Name;
  }

  OptData = PcdGetPtr(PcdEfiBootOptionFlag);
  OptDataSize = PcdGetSize(PcdEfiBootOptionFlag);
  if(OptionalDataSize == OptDataSize && CompareMem(OptionalData, OptData, OptDataSize) == 0){
    return Name;
  }

  TempDp = Dp;
  IsOsCreat=GetFsDpFromOsShortDp(&TempDp);
  Status = gBS->LocateDevicePath(&gByoDiskInfoProtocolGuid, &TempDp, &Handle); 
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "No ByoDiskInfo(%r)\n", Status));
    return Name;    
  }
  
  Status = gBS->HandleProtocol(
                  Handle,
                  &gByoDiskInfoProtocolGuid,
                  &ByoDiskInfo
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

  Size = StrSize(Name) + (36 + Size) * sizeof(CHAR16);
  p = (CHAR16*)AllocatePool(Size);
  ASSERT(p != NULL);
  if(ByoDiskInfo->DevType == BYO_DISK_INFO_DEV_NVME&&IsOsCreat == TRUE){
   ByoDiskInfo->GetDp(ByoDiskInfo,&OsDp);

    ShowDevicePathDxe(gBS,OsDp);
    Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &OsDp,
                  &PciHandle
                  );
    DEBUG((EFI_D_INFO,"LocateDevicePath STATUS IS %r\n",Status));
    Status = gBS->HandleProtocol(
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  &ptPciIo
                  );
    DEBUG((EFI_D_INFO,"HandleProtocol STATUS IS %r\n",Status));
    ptPciIo->GetLocation(ptPciIo, &Seg, &Bus, &Dev, &Func);
    UnicodeSPrint(p, Size, L"%s(NVME(PCI%d-%d-%d): %a)",Name,Bus,Dev, Func, ModelName);
    HasSet = TRUE;
  } else if(ByoDiskInfo->DevType == BYO_DISK_INFO_DEV_SATA){
    if(LibGetSataPortInfo(gBS, Handle, &SataHostIndex, &SataPortIndex, &PhysicPortIndex)){
      if(SataHostIndex & BIT15){
        UnicodeSPrint(p, Size, L"%s(SATA %d: %a)", Name, PhysicPortIndex, ModelName);
      } else {
        UnicodeSPrint(p, Size, L"%s(SATA%d-%d: %a)", Name, SataHostIndex, PhysicPortIndex, ModelName);
      }
      HasSet = TRUE;
    }
  } 

  if(!HasSet) {
    UnicodeSPrint(p, Size, L"%s(%a)", Name, ModelName);
  }
  FreePool(ModelName);

  *Updated = TRUE;  
  return p;
}












EFI_BDS_BOOT_MANAGER_PROTOCOL gBdsBootManagerProtocol = {
  ByoEfiBootManagerConnectAllDummy,
  ByoEfiBootManagerRefreshAllBootOptionDummy,
  ByoEfiBootManagerGetLoadOptions2,
  ByoEfiBootManagerFreeLoadOptions,
  ByoEfiBootManagerCreateSetupBootOption,
  ByoEfiBootManagerCreateShellBootOption,  
  ByoEfiBootManagerBootDummy,
  EfiBootManagerGetVariableAndSize,
  ByoEfiBootManagerBootAppDummy,
  ByoEfiBootManagerBootSetupDummy,
  ByoEfiBootManagerBootAppParameterDummy,
  ByoEfiBootManagerBootFileParameterDummy,
  EfiBootManagerGetVarSizeAttrib,
  ByoEfiBootManagerGetCurHotKey,
  ByoEfiBootManagerSetCurHotKey,
  ByoDrawPasswordDialog,
  ByoDrawPostPasswordDialog,
  ByoPrintPostString,
  ByoGetExtBmBootName,
  NULL,
  0,
  NULL,
  0
};


typedef  
EFI_STATUS
(EFIAPI *EFI_BDS_BOOT_BEFORE_CONSOLE) (
    VOID
  );

typedef  
EFI_STATUS
(EFIAPI *EFI_BDS_BOOT_AFTER_CONSOLE) (
    VOID
  );

EFI_BDS_BOOT_AFTER_CONSOLE  gBdsBootAfterConsole = NULL;
EFI_BDS_BOOT_BEFORE_CONSOLE gBdsBootBeforeConsole = NULL;
BOOLEAN                     gHotKeyHandleByLib    = FALSE;

VOID BdsPrepareHotKey();
VOID BdsHandleHotKey();
VOID BdsStopHotKey();
VOID BdsHotKeyBoot();

VOID BdsFormalizeOSIndicationVariable(VOID);
EFI_STATUS LockReadOnlyVariable();
BOOLEAN CheckOsIndication();
VOID BootOsIndication();

VOID
ClearBootFlagsValid (
  )
{
  UINT8                                 ResponseSize = 1;
  EFI_STATUS                            Status;
  UINT8                                 CommandData[6] = {0x05,0x00,0x00,0x00,0x00,0x00};
  UINT8                                 ResponseData[1];

  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  
  ZeroMem (ResponseData, 1);

  EfiInitializeIpmiBase();
  Status = EfiSendCommandToBMC (
             0x00,
             0x08,
             &CommandData[0],
             6,
             &ResponseData[0],
             &ResponseSize
             );
  DEBUG((EFI_D_INFO, "ClearBootFlagsValid Status:%r\n", Status));
}

BOOLEAN
IfAfterFlash()
{
  SETUP_DATA            SetupData;	
  EFI_STATUS            Status;
  UINTN                 Size;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  Size = sizeof (SETUP_DATA);	
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );  

  if (SetupData.AfterFlash == 0) {
    SetupData.AfterFlash = 1;
     Status = gRT->SetVariable (
                     PLATFORM_SETUP_VARIABLE_NAME,
                     &gPlatformSetupVariableGuid,
                     PLATFORM_SETUP_VARIABLE_FLAG,
                     Size,
                     &SetupData
                     );
   DEBUG((EFI_D_INFO, "SetVariable %r\n", Status)); 
     return TRUE;
  } else {
     return FALSE;
  }
}

#define FVREC_RAEA_SIZE  (_PCD_VALUE_PcdFlashFvRecoveryBackUpSize + _PCD_VALUE_PcdFlashFvRecoverySize + _PCD_VALUE_PcdFlashFvSecSize) 
#define FVMAIN_RAEA_SIZE (_PCD_VALUE_PcdFlashFvMainSize + _PCD_VALUE_PcdFlashFvMain2Size) 


VOID
AmdSPILockTest(
	IN EFI_EVENT		Event,
	IN VOID 			*Context
)
{
    EFI_STATUS             Status;
    SPI_LOCK_PROTOCOL      *FchSpiLockProtocol=NULL;
    
    if (Event != NULL) {
        gBS->CloseEvent (Event);
    }
    Status = gBS->LocateProtocol(&gFchSpiLockGuid,NULL,(VOID **)&FchSpiLockProtocol);
    if (EFI_ERROR (Status)) {
        return;
    }
	//Status = FchSpiLockProtocol->SpiLock(0xFFCF0000,0xC40,1,0);	  //FvRecovery
	//Status = FchSpiLockProtocol->SpiLock(0xFF9F0000,0xC00,1,1);	  //Fvmain
	Status = FchSpiLockProtocol->SpiLock(_PCD_VALUE_PcdFlashFvRecoveryBackUpBase,FVREC_RAEA_SIZE/0x400+0x20,1,0);	  //FvRecovery
	Status = FchSpiLockProtocol->SpiLock(_PCD_VALUE_PcdFlashFvMain2Base,FVMAIN_RAEA_SIZE/0x400,1,1);	 //Fvmain
	Status = FchSpiLockProtocol->SpiLock(_PCD_VALUE_PcdFlashNvStorageVariableBase,0x680,1,2);	  //	0xFF850000 NVRam
	Status = FchSpiLockProtocol->SpiLock(_PCD_VALUE_PcdFlashAreaBaseAddress,0x2140,1,3);	//	0xFF000000 FvBin
	//DEBUG((-1,"AmdSPILockTest FvRecovery %lX	 \n",_PCD_VALUE_PcdFlashFvRecoveryBackUpBase));
	//DEBUG((-1,"AmdSPILockTest FvRecoverySize %lX	 \n",FVREC_RAEA_SIZE/0x400+0x20));
	
	//DEBUG((-1,"AmdSPILockTest FvMain2 %lX   \n",_PCD_VALUE_PcdFlashFvMain2Base));
	//DEBUG((-1,"AmdSPILockTest FvMain2Size %lX   \n",FVMAIN_RAEA_SIZE/0x400));


}


VOID
BIOSLockFucntion(
VOID
)
{
    EFI_EVENT           ReadyToBootEvent;
    SETUP_DATA          SetupData;
    UINTN               Size;
    EFI_STATUS             Status;
    Status = EFI_SUCCESS;
    
  Size = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  ); 

  	DEBUG((EFI_D_INFO,"GetVariable SetupData.BIOSLock %x\n",SetupData.BIOSLock));

  if(SetupData.BIOSLock == 1){

    Status = gBS->CreateEventEx (
            EVT_NOTIFY_SIGNAL,
            TPL_NOTIFY,
            AmdSPILockTest,
            NULL,
            &gEfiEventReadyToBootGuid,
            &ReadyToBootEvent
        );
    ASSERT_EFI_ERROR (Status);   
    }
}

VOID
CheckBiosPassword(
VOID
)
{
	EFI_STATUS			Status;
	UINT8				*DataBuffer;
	UINT32				Size;
	SYSTEM_PASSWORD 	SetupSpi;
	EFI_GUID  	    	gTseSetupGuid = SYSTEM_PASSWORD_GUID;
	UINTN				FvPswAddress;
	UINTN	Index;
	Size = 0x1000;
	//FvPswAddress = PcdGet32(PcdFvPswBase)- PcdGet32(PcdFlashAreaBaseAddress) + SIZE_4KB;
	FvPswAddress = PcdGet32(PcdFlashAreaSize) + PcdGet32(PcdFvPswSize) + SIZE_4KB;
	DEBUG((EFI_D_INFO,"PcdFvPswBase = %x PcdFlashAreaSize %x \n",PcdGet32(PcdFvPswBase),PcdGet32(PcdFlashAreaSize)));

	DEBUG((EFI_D_INFO,"%a(%d) FvPswAddress = %x  \n",__FUNCTION__,__LINE__, FvPswAddress));
	DataBuffer = AllocateReservedZeroMemoryBelow4G(gBS, SIZE_4KB);
	ASSERT(DataBuffer != NULL);
	Status = gBS->LocateProtocol (
					  &gEfiSpiProtocolGuid,
					  NULL,
					  (VOID **)&pSpiProtocol
					  );
	ASSERT_EFI_ERROR (Status);
	Status = pSpiProtocol->Execute(
					pSpiProtocol,
					0x01,//SPI_READ
					0x00 ,//SPI_WREN
					TRUE,
					FALSE,
					FALSE,
					FvPswAddress,
					Size,
					DataBuffer,
					EnumSpiRegionAll
				);
  	DEBUG((EFI_D_INFO,"%a(%d) Status = %r  \n",__FUNCTION__,__LINE__, Status));
	 
	for(Index = 1;Index<200;Index++)
	{
		DEBUG((EFI_D_INFO, " %x", DataBuffer[Index]));
		if(Index%10 == 0)
			DEBUG((EFI_D_INFO, "\n"));
		}

    SetupSpi.bHavePowerOn = DataBuffer[0];
    SetupSpi.bHaveAdmin = DataBuffer[1];
    CopyMem (SetupSpi.AdminHash, DataBuffer+2, sizeof (SetupSpi.AdminHash));
    CopyMem (SetupSpi.PowerOnHash, DataBuffer+14, sizeof (SetupSpi.PowerOnHash));
    CopyMem (SetupSpi.PowerOn, DataBuffer+26, sizeof (SetupSpi.PowerOn));
    CopyMem (SetupSpi.Admin, DataBuffer+66, sizeof (SetupSpi.Admin));
    SetupSpi.EnteredType = DataBuffer[106];
    SetupSpi.VerifyTimes = DataBuffer[107];
    SetupSpi.VerifyTimesAdmin = DataBuffer[108];
    SetupSpi.VerifyTimesPop = DataBuffer[109];
    SetupSpi.RequirePopOnRestart = DataBuffer[110];
    SetupSpi.ChangePopByUser = DataBuffer[111];
    SetupSpi.SimplePassword = DataBuffer[112];
    SetupSpi.PasswordValidDays = DataBuffer[113];
	CopyMem(&SetupSpi.AdmPwdTime, DataBuffer+114, sizeof (PASSWOR_TIME));
	CopyMem(&SetupSpi.PopPwdTime, DataBuffer+121, sizeof (PASSWOR_TIME));
    SetupSpi.HiddenEnteredType = DataBuffer[128];
    SetupSpi.RebootTime = DataBuffer[129];
    CopyMem(&SetupSpi.AdminRecords, DataBuffer+130, sizeof (PASSWORD_RECORD));
    CopyMem(&SetupSpi.PowOnRecords, DataBuffer+190, sizeof (PASSWORD_RECORD));

	DEBUG((EFI_D_INFO, "\nAdminPasswdHash:"));
	for(Index = 0;Index<12;Index++)
	{
		DEBUG((EFI_D_INFO, " %x", SetupSpi.AdminHash[Index]));
		}

	
	DEBUG((EFI_D_INFO, "\nPowerOnHash:"));
	for(Index = 0;Index<12;Index++)
	{
		DEBUG((EFI_D_INFO, " %x", SetupSpi.PowerOnHash[Index]));
		}

	DEBUG((EFI_D_INFO, "\nPowerOnPasswd:"));
	for(Index = 0;Index<20;Index++)
	{
		DEBUG((EFI_D_INFO, " %x", SetupSpi.PowerOn[Index]));
		}

	DEBUG((EFI_D_INFO, "\nAdminPasswd:"));
	for(Index = 0;Index<20;Index++)
	{
		DEBUG((EFI_D_INFO, " %x", SetupSpi.Admin[Index]));
		}

	
  	DEBUG((EFI_D_INFO,"EnteredType %x VerifyTimes %x VerifyTimesAdmin %x \n",SetupSpi.EnteredType,SetupSpi.VerifyTimes,SetupSpi.VerifyTimesAdmin));

	DEBUG((EFI_D_INFO,"VerifyTimesPop %x RequirePopOnRestart %x ChangePopByUser %x \n",SetupSpi.VerifyTimesPop,SetupSpi.RequirePopOnRestart,SetupSpi.ChangePopByUser));
	
	DEBUG((EFI_D_INFO,"SimplePassword %x PasswordValidDays %x HiddenEnteredType %x \n",SetupSpi.SimplePassword,SetupSpi.PasswordValidDays,SetupSpi.HiddenEnteredType));

	DEBUG((EFI_D_INFO,"AdmPwdTime Y %x M %x D %x \n",SetupSpi.AdmPwdTime.Year,SetupSpi.AdmPwdTime.Month,SetupSpi.AdmPwdTime.Day));

	DEBUG((EFI_D_INFO,"PopPwdTime Y %x M %x D %x \n",SetupSpi.PopPwdTime.Year,SetupSpi.PopPwdTime.Month,SetupSpi.PopPwdTime.Day));

	if((SetupSpi.bHaveAdmin ==1)||(SetupSpi.bHavePowerOn ==1)){
	Status = gRT->SetVariable (
         SYSTEM_PASSWORD_NAME,
         &gEfiSystemPasswordVariableGuid,      	 	
		 EFI_VARIABLE_NON_VOLATILE|EFI_VARIABLE_BOOTSERVICE_ACCESS|EFI_VARIABLE_RUNTIME_ACCESS, 
         sizeof(SYSTEM_PASSWORD),
         &SetupSpi
         );
		}
}

VOID
ShowVersionCheckLoadDefault()
{
  CHAR16		*Result ;
  CHAR16		 *Result_Title;

  StopWarningPrompt((VOID*)PcdGet64(PcdWaringPrintEvent));
  Result = HiiGetString(gHiiHandle, STRING_TOKEN(STR_SETUP_VERSION_ERROR), NULL);
  Result_Title = HiiGetString(gHiiHandle, STRING_TOKEN(STR_SETUP_VERSION_TITLE), NULL);

  UiConfirmDialog(DIALOG_INFO,Result_Title, NULL, TEXT_ALIGIN_CENTER,  Result, NULL);
 
  if(!PcdGet8(PcdLegacyBiosSupport)){
    if(IsEnglishLang()){
      StartWarningPrompt (L"Driver Loading");
    }else{
      StartWarningPrompt (L"驱动加载中");
    }
  }
}


VOID
HandleDimmError()
{
  CHAR16        *Result = L"Memory failure, the system will shut down in %d seconds!";
  CHAR16        *Result_C = L"内存故障，系统将在%d秒内关机!";
  CHAR16        *buffer;
  UINTN         Size;
  UINTN         HaltTime;

    DEBUG((EFI_D_INFO,"PcdDimmError is %d\n",PcdGet8(PcdDimmError)));
  if((PcdGet8(PcdDimmError) & 0x80)!=0){  //Dimm Error Deceted
      HaltTime = 5;
      Size = StrSize(Result)+5;
	  buffer = AllocateZeroPool(Size);
	  DEBUG((EFI_D_INFO,"PcdDimmError deceted\n"));
    while (1){
      if(BeEnglish()){
        UnicodeSPrint(buffer, Size, Result, HaltTime);
        UiConfirmDialog(DIALOG_NO_REPAINT, L"Memory Failure", NULL, TEXT_ALIGIN_CENTER,  buffer, NULL);
      }else{
        UnicodeSPrint(buffer, Size, Result_C, HaltTime);
        UiConfirmDialog(DIALOG_NO_REPAINT, L"内存故障", NULL, TEXT_ALIGIN_CENTER,  buffer, NULL);
	  }
	  gBS->Stall(1000 * 1000);
      HaltTime--;
      if (HaltTime == 0){
        gRT->ResetSystem (EfiResetShutdown, EFI_SUCCESS, 0, NULL);
      }
    }
  }
}

/**

  Service routine for BdsInstance->Entry(). Devices are connected, the
  consoles are initialized, and the boot options are tried.
  @param This             Protocol Instance structure.

**/
VOID
EFIAPI
BdsEntry (
  IN EFI_BDS_ARCH_PROTOCOL  *This
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION    *DriverOption;
  UINTN                           DriverOptionCount;
  CHAR16                          *FirmwareVendor;
  EFI_EVENT                       HotkeyTriggered;
  EFI_BOOT_MODE                   BootMode;
  BOOLEAN                         IsOsIndication;
  BOOLEAN                         HasFlash;
//  EFI_HANDLE                      Handle;


  PERF_END (NULL, "DXE", NULL, 0);
  PERF_START (NULL, "BDS", NULL, 0);

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  InvokeHookProtocol(gBS, &gByoBdsEnterHookGuid);

  BootMode = GetBootModeHob();

//  Handle = NULL;
//  gBS->InstallProtocolInterface(
//		 &Handle,
//		 &gUpdateBmcLanConfigNotifyGuid,
//		 EFI_NATIVE_INTERFACE,
//		 NULL
//		 );

  FirmwareVendor = (CHAR16 *) PcdGetPtr (PcdFirmwareVendor);
  gST->FirmwareVendor = AllocateRuntimeCopyPool (StrSize (FirmwareVendor), FirmwareVendor);
  ASSERT (gST->FirmwareVendor != NULL);
  gST->FirmwareRevision = PcdGet32 (PcdFirmwareRevision);

  gST->Hdr.CRC32 = 0;
  gBS->CalculateCrc32 ((VOID *) gST, sizeof (EFI_SYSTEM_TABLE), &gST->Hdr.CRC32);

  if(FeaturePcdGet(PcdBdsSupportUefiOsIndication)){
    BdsFormalizeOSIndicationVariable();
  }
  LockReadOnlyVariable();
  if(FeaturePcdGet(PcdBdsSupportUefiOsIndication)){
    IsOsIndication = CheckOsIndication();
  } else {
    IsOsIndication = FALSE;
  }

  if(!gHotKeyHandleByLib && !IsOsIndication){
    BdsPrepareHotKey();
  }

  EfiBootManagerStartHotkeyService (&HotkeyTriggered);

  PERF_START (NULL, "BeforeConsole", "BDS", 0);
  PlatformBootManagerBeforeConsole();
  PERF_END   (NULL, "BeforeConsole", "BDS", 0);

  DriverOption = EfiBootManagerGetLoadOptions (&DriverOptionCount, LoadOptionTypeDriver);
  LoadDrivers (DriverOption, DriverOptionCount);
  ByoEfiBootManagerFreeLoadOptions (DriverOption, DriverOptionCount);

  if(gBdsBootBeforeConsole != NULL){
    gBdsBootBeforeConsole();
  }
  
  PERF_START (NULL, "ConnectConsoles", "BDS", 0);
  ByoEfiBootManagerConnectAllDefaultConsoles();
  PERF_END   (NULL, "ConnectConsoles", "BDS", 0);

  if(!PcdGet8(PcdDealDimmError)){
    HandleDimmError();
  }
  if(PcdGet8(PcdVersionCheck)){
    ShowVersionCheckLoadDefault();
  }
  InvokeHookProtocol(gBS, &gBdsAfterConnectAllDefConsoleProtocolGuid);


// -------------------------- afterConsole --------------------------
  PERF_START (NULL, "AfterConsole", "BDS", 0);
  PlatformBootManagerAfterConsole();
  PERF_END   (NULL, "AfterConsole", "BDS", 0);
  

// ----------------------------- HotKey -----------------------------
  if(!gHotKeyHandleByLib && !IsOsIndication){
    BdsHandleHotKey(BootMode, HotkeyTriggered);
  }else{
    StopWarningPrompt((VOID*)PcdGet64(PcdWaringPrintEvent));  
  }

  BdsStopHotKey();

  //
  // Clear/Restore Key####/Timeout per platform request
  // For Timeout, variable change is enough to control the following behavior
  // For Key####, variable change is not enough so we still need to check the *_ENABLE_HOTKEY later
  //
  BdsUpdateVariable (L"KeyOrder", NULL, &gByoGlobalVariableGuid);

#if !defined(MDEPKG_NDEBUG)
  DumpBootOptions();
#endif

  if(gBdsBootAfterConsole != NULL){
    gBdsBootAfterConsole();
  }
  

//--------------------------------------------------------------------
  HasFlash = IfAfterFlash();
  if ((CmosRead(AFTER_FLASH_SPI) == 0xE || HasFlash == TRUE) && !PcdGet8(PcdIsPlatformCmosBad)){
        CheckBiosPassword();
        CmosWrite(AFTER_FLASH_SPI, 0xFF);
  }
  WriteAsmedia(gBS);
 UpdateSmbiosType11();
 AddSmbiosType24();
 AddSmbiosType39();
  BIOSLockFucntion();
 
  ClearBootFlagsValid();
  SendToBmcConfig();
 

  if(!IsOsIndication){
    EfiBootManagerHotkeyBoot();
    if(!gHotKeyHandleByLib){
      BdsHotKeyBoot();
    }
  } else {
    BootOsIndication();
  }
  
 if (PcdGetBool(PcdIpmiBootNext)) {
    BootBmcNextBoot();
  } 

  InvokeHookProtocol(gBS, &gPlatBeforeDefaultBootProtocolGuid);
  
  DefaultBootBehavior();
  CpuDeadLoop();
}


