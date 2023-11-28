

#include "PlatformDxe.h"
#include <Guid/Acpi.h>
#include <Guid/AcpiS3Context.h>
#include <IndustryStandard/Pci.h>
#include <Library/SerialPortLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/LockBoxLib.h>
#include <Library/MtrrLib.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/VariableWrite.h>
#include <Protocol/SimpleTextIn.h>
#include <Protocol/SmmCommunication.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/NvMediaAccess.h>
#include <Protocol/PlatHostInfoProtocol.h>
//#include <Library/TcgPhysicalPresenceLib.h>
//#include <Library/Tcg2PhysicalPresenceLib.h>
#include <Guid/EventLegacyBios.h>
#include <Token.h>
#include <AmdCpu.h>
#include "Library/IoLib.h"
#include <Library/ByoCommLib.h>
#include <ByoStatusCode.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/LoadedImage.h>
#include <ByoBootGroup.h>
#include <Protocol/PciPreprocessHookParameter.h>
#include <Library/IpmiBaseLib.H>
#include <SysMiscCfg2.h>
#include <Library/LogoLib.h>
#include <Library/AmdCbsVariable.h>



#define SIZE_16M_ALIGN              0xFFFFFFFFFF000000ULL

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
  PLATFORM_COMM_INFO  *Info,
  UINT8               Pci64Limit
  );

EFI_STATUS AddSmbiosTypeLate();

extern EFI_GUID gBdsAllDriversConnectedProtocolGuid;
extern EFI_GUID gEfiSetupEnterGuid;

CONST SETUP_DATA      *gSetupData;
BOOLEAN               gAfterReadyToBoot = FALSE;



VOID
ExitPmAuthCallBack (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  VOID              *Interface;
  EFI_STATUS        Status;
  

  Status = gBS->LocateProtocol(&gExitPmAuthProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  
}






UINT32 GetAcpiTableSmmCommAddr(VOID)
{
  EFI_ACPI_4_0_ROOT_SYSTEM_DESCRIPTION_POINTER  *Rsdp;
  EFI_ACPI_DESCRIPTION_HEADER                   *Xsdt;
  UINTN                                         Index;
  UINT64                                        *XTableAddress;
  UINTN                                         TableCount;
  EFI_STATUS                                    Status;	
  EFI_SMM_COMMUNICATION_ACPI_TABLE              *SmmAcpi;
	

  Status = EfiGetSystemConfigurationTable(&gEfiAcpiTableGuid, &Rsdp);
  ASSERT(!EFI_ERROR(Status));	
  if (EFI_ERROR(Status)) {
    return 0;
  }

  Xsdt = (EFI_ACPI_DESCRIPTION_HEADER*)(UINTN)Rsdp->XsdtAddress;
  TableCount = (Xsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT64);
  XTableAddress = (UINT64 *)(Xsdt + 1);
  for (Index = 0; Index < TableCount; Index++) {
    SmmAcpi = (EFI_SMM_COMMUNICATION_ACPI_TABLE *)(UINTN)XTableAddress[Index];
    if ((SmmAcpi->UefiAcpiDataTable.Header.Signature == EFI_ACPI_4_0_UEFI_ACPI_DATA_TABLE_SIGNATURE) &&
        (SmmAcpi->UefiAcpiDataTable.Header.Length == sizeof (EFI_SMM_COMMUNICATION_ACPI_TABLE)) &&
        CompareGuid (&(SmmAcpi->UefiAcpiDataTable.Identifier), &gEfiSmmCommunicationProtocolGuid) ) {
      return (UINT32)(UINTN)SmmAcpi;
    }
  }

  ASSERT(FALSE);
  return 0;
}





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


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

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
}




VOID
EFIAPI
PlatOnReadyToBoot (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__)); 
  gBS->CloseEvent(Event);

  MemoryAbove4GBRecovery();

  gAfterReadyToBoot = TRUE;
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

  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
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



VOID RemoveBootOrder()
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
  if(Status != EFI_BUFFER_TOO_SMALL){
    return;
  }
  BootOrder = AllocatePool(VarSize);
  if(BootOrder == NULL){
    return;
  }  
  Status = gRT->GetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &VarSize,
                  BootOrder
                  );
  DEBUG((EFI_D_INFO, "Get BootOrder %r (%X %X)\n", Status, BootOrder, VarSize));
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  Count = VarSize/sizeof(UINT16);
  for(Index=0;Index<Count;Index++){
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

ProcExit:
  if(BootOrder != NULL){
    FreePool(BootOrder);
  }
}



VOID HandleCmosBad()
{
  UINT8  IsCmosBad;


  IsCmosBad = PcdGet8(PcdIsPlatformCmosBad);

  if(IsCmosBad){
    SetDefaultLanguage();
    RemoveBootOrder();    
    REPORT_STATUS_CODE (EFI_ERROR_CODE, BSC_CMOS_RECOVERY_OCCURRED);
    DEBUG((EFI_D_ERROR, "Clear Cmos Bad\n"));
    LibClearCmosBad();
  }

  if(PcdGet8(PcdVersionCheck)){
    SetDefaultLanguage();
    RemoveBootOrder();  
  }

}


VOID 
GetBMCSetupConfigValue(
  SETUP_DATA  *SetupValue
  )
{
  EFI_STATUS Status;
  CHAR8    *Language;
  BMC_CONFIG_SETUP              *Value;
  UINTN    Size;
  CBS_CONFIG    CbsVariable;

  DEBUG((EFI_D_INFO, "%a() Setupdata platformdxe\n", __FUNCTION__));
  
  if(!PcdGet8(PcdBmcBiosConfigFlag)){
  	return;
  }
  
  Value = GetBMCConfigValueHob();
  TranslateBmcConfig (Value, SetupValue, 1);
  Size = sizeof(CBS_CONFIG);  
  Status = gRT->GetVariable(L"AmdSetup", &gCbsSystemConfigurationGuid, NULL, &Size, &CbsVariable);
  DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), Get CBS :%r, Size :%d.\n", __LINE__, Status, Size));
  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), CbsDfCmnMemIntlv :%d.\n", __LINE__, Value->CbsDfCmnMemIntlv));
    DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), CbsCmnCpuCpb :%d.\n", __LINE__, Value->CbsCmnCpuCpb));

    if (Value->CbsDfCmnMemIntlv == 4) {
      CbsVariable.CbsDfCmnMemIntlv = 7;
    } else {
      CbsVariable.CbsDfCmnMemIntlv = Value->CbsDfCmnMemIntlv;
    }
    CbsVariable.CbsCmnCpuCpb = Value->CbsCmnCpuCpb;

    Size = sizeof(CBS_CONFIG);
    Status = gRT->SetVariable(
                                L"AmdSetup",
                                &gCbsSystemConfigurationGuid,
                                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                                Size,
                                &CbsVariable
                                );
    DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), Set CBS :%r.\n", __LINE__, Status));
  }

 
  if(Value->Language == 0){
  	Language = "zh-Hans";
  }else{
  	Language = "en-US";
  }        
  Status = gRT->SetVariable (
                   L"PlatformLang",
                   &gEfiGlobalVariableGuid,
                   EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                   AsciiStrSize (Language),
                   Language
                   );
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
  SETUP_DATA                    SetupData;
  UINTN                         Size;
  UINT8                         BootModeType;

  Status = gBS->LocateProtocol(&gEfiVariableWriteArchProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  Size = sizeof(SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );
  
  GetBMCSetupConfigValue(&SetupData);
  if(SetupData.LastBootModeType != 0){
  	SetupData.BootModeType = SetupData.LastBootModeType;
	SetupData.LastBootModeType = 0;
  }
  
  if (SetupData.BootModeType == BIOS_BOOT_UEFI_OS) {
    SetupData.VideoRomPolicy = ROM_POLICY_UEFI_FIRST;
    SetupData.PxeRomPolicy = ROM_POLICY_UEFI_FIRST;
    SetupData.StorageRomPolicy = ROM_POLICY_UEFI_FIRST;
    SetupData.OtherRomPolicy = ROM_POLICY_UEFI_FIRST;
  } else if (SetupData.BootModeType == BIOS_BOOT_LEGACY_OS) {
    SetupData.VideoRomPolicy = ROM_POLICY_LEGACY_FIRST;
    SetupData.PxeRomPolicy = ROM_POLICY_LEGACY_FIRST;
    SetupData.StorageRomPolicy = ROM_POLICY_LEGACY_FIRST;
    SetupData.OtherRomPolicy = ROM_POLICY_LEGACY_FIRST;
  }

  SetupData.FtpmSupport = (UINT8)PcdGetBool(PcdFtpmSupport);
  SetupData.DtpmSupport = (UINT8)PcdGetBool(PcdDtpmSupport);
  if (SetupData.TpmSelect == 0 && SetupData.DtpmSupport != TRUE) {
    SetupData.TpmSelect = 2;
  }
  if (SetupData.TpmSelect == 1 && SetupData.FtpmSupport != TRUE) {
    SetupData.TpmSelect = 2;
  }
  if (SetupData.FtpmSupport) {
    SetupData.DtpmSupport = FALSE;
  }

  DEBUG((EFI_D_ERROR, __FUNCTION__"(), FtpmSupport :%d.\n", SetupData.FtpmSupport));
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), DtpmSupport :%d.\n", SetupData.DtpmSupport));
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), TpmSelect :%d.\n", SetupData.TpmSelect));

  
  Status = gRT->SetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  PLATFORM_SETUP_VARIABLE_FLAG,
                  Size,
                  &SetupData
                  );  

  if(PcdGetBool(PcdIpmiBootflag)){
    BootModeType = PcdGet8(PcdIpmiBootMode);
  
    Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );
    if(PcdGetBool(PcdIpmiBootOnce)){
      SetupData.LastBootModeType = SetupData.BootModeType;
	}
    SetupData.BootModeType = BootModeType;

    if (BootModeType == BIOS_BOOT_UEFI_OS) {
      SetupData.VideoRomPolicy = ROM_POLICY_UEFI_FIRST;
      SetupData.PxeRomPolicy = ROM_POLICY_UEFI_FIRST;
      SetupData.StorageRomPolicy = ROM_POLICY_UEFI_FIRST;
      SetupData.OtherRomPolicy = ROM_POLICY_UEFI_FIRST;
    } else if (BootModeType == BIOS_BOOT_LEGACY_OS) {
      SetupData.VideoRomPolicy = ROM_POLICY_LEGACY_FIRST;
      SetupData.PxeRomPolicy = ROM_POLICY_LEGACY_FIRST;
      SetupData.StorageRomPolicy = ROM_POLICY_LEGACY_FIRST;
      SetupData.OtherRomPolicy = ROM_POLICY_LEGACY_FIRST;
    }
  
    gRT->SetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  PLATFORM_SETUP_VARIABLE_FLAG,
                  Size,
                  &SetupData
                  );
  }

  HandleCmosBad();  
  
}




VOID
EFIAPI
FchInitDonePolicyCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID                *Interface;
  EFI_STATUS          Status;
  PLATFORM_COMM_INFO  *Info;


  Status = gBS->LocateProtocol(&gFchInitDonePolicyProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  MmioWrite8(SB_IOAPIC_BASE_ADDRESS, 0);
  MmioWrite32(SB_IOAPIC_BASE_ADDRESS + 0x10, NCPU << 24);  

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo(); 
  Info->SbApicBase = SB_IOAPIC_BASE_ADDRESS;
  Info->SbApicId   = NCPU;

  DEBUG((EFI_D_INFO, "sbapic[0]:%X\n", MmioRead32(SB_IOAPIC_BASE_ADDRESS + 0x10)));
}


VOID PlatformAfterConsoleStartHook()
{
  BOOLEAN    BootState;
  UINT8      TpmClear = 0;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  BootState = PcdGetBool(PcdBootState);
  if (BootState) {
    PcdSetBool(PcdBootState, FALSE);
  }
/*  
  TcgPhysicalPresenceLibProcessRequest();
  Tcg2PhysicalPresenceLibProcessRequest(NULL,&TpmClear);
  if(TpmClear){
    StopWarningPrompt((VOID*)PcdGet64(PcdWaringPrintEvent));
    gST->ConOut->ClearScreen(gST->ConOut);
    ShowPostLogo();
  }
*/
}



VOID PlatformAfterConsoleEndHook()
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  AddSmbiosTypeLate();
}


VOID PlatformBeforeBiosUpdateHook()
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
}



STATIC EFI_GUID gUefiPxeBcDxeFileGuid = {0xB95E9FDA, 0x26DE, 0x48d2, {0x88, 0x07, 0x1F, 0x91, 0x07, 0xAC, 0x5E, 0x3A}};

VOID RemoveUefiPxeDriver()
{
  UINTN                         DriverImageHandleCount;
  EFI_HANDLE                    *DriverImageHandleBuffer = NULL;
  EFI_LOADED_IMAGE_PROTOCOL     *LoadedImage;
  EFI_STATUS                    Status;
  UINTN                         Index;
  MEDIA_FW_VOL_FILEPATH_DEVICE_PATH  *FvFileDp;
  UINT8                         Tmp;
  UINT8 Ipv4;
  UINT8 Ipv6;
  BMC_CONFIG_SETUP              *Value;

  DEBUG((EFI_D_INFO, "%a() Setupdata platformdxe\n", __FUNCTION__));
  
  if(PcdGet8(PcdBmcBiosConfigFlag)){
    Value = GetBMCConfigValueHob();
  	Ipv4 = Value->Ipv4Support;
  	Ipv6 = Value->Ipv6Support;
    if(Ipv4 == 0 && Ipv6 ==0){
      Tmp = 0;
	} else {
	  Tmp = 1;
	}
  } else {
  	Tmp = gSetupData->LanBoot;
  }
  


  //if(!gSetupData->LanBoot){
  if(!Tmp){
 
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
        if(CompareGuid(&FvFileDp->FvFileName, &gUefiPxeBcDxeFileGuid)){
          Status = gBS->UnloadImage(DriverImageHandleBuffer[Index]);
          DEBUG((EFI_D_INFO, "Unload Pxe:%r\n", Status));
          break;
        }
      }

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
  RemoveUefiPxeDriver(); 


  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  ClearRootBridgeResource(Info);
  SetRootBridgeMmio64Range(Info, gSetupData->Pci64Limit);
}



VOID PlatformAfterConnectPciRootBridgeHook()
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
#if !defined(MDEPKG_NDEBUG)
  ListPci(gBS);
#endif

}




//00000003 00003004 00004003 00005024 
//00006003 00007044 00008003 00009064 
//0000A003 0000B084 0000C003 0000D0A4 
//0000E003 0000E0C4 0000F003 00FFF0E4 
STATIC
VOID
SetRootBridgeIoRange (
  PLATFORM_COMM_INFO  *Info
  ) 
{
  UINTN   Index;
  UINT32  HostIndex;
  UINT32  Limit;
  UINTN   HostBase;

  for(HostIndex=0; HostIndex<Info->PciHostCount; HostIndex++){

    DEBUG((EFI_D_INFO, "[%d] %X %X\n", HostIndex, Info->IoBase[HostIndex], Info->IoSize[HostIndex]));
    
    if(HostIndex >= Info->Dies){
      HostBase = PCI_DEV_MMBASE(0, 0x18 + (HostIndex - Info->Dies + 4), 0);  
    }else{
      HostBase = PCI_DEV_MMBASE(0, 0x18 + HostIndex, 0);  
    }

    for(Index=0; Index<Info->PciHostCount; Index++){

      Limit = Info->IoBase[Index] + Info->IoSize[Index] - 1;
      Limit = (Limit & 0xF000) | Info->FabricId[Index];
      if(Index >= Info->Dies){
        MmioWrite32(HostBase+DF0_IOBASE_REG +(Index-Info->Dies+4)*8, Info->IoBase[Index]|3);  // WE, RE
        MmioWrite32(HostBase+DF0_IOLIMIT_REG+(Index-Info->Dies+4)*8, Limit);
      }else{
        MmioWrite32(HostBase+DF0_IOBASE_REG +Index*8, Info->IoBase[Index]|3);  // WE, RE
        MmioWrite32(HostBase+DF0_IOLIMIT_REG+Index*8, Limit); 
      } 
    }
  }
}



STATIC
VOID
PrepareIoRes (
  PLATFORM_COMM_INFO  *Info
  ) 
{
  UINTN   Index;  

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  for(Index=0; Index<Info->PciHostCount; Index++){
    if(Info->IoSize[Index] != 0){
      continue;
    }

    DEBUG((EFI_D_INFO, "B:%X C:%X\n", Info->PlatPciIoBase, Info->PlatPciIoCur));
 
    if(Info->PlatPciIoCur + SIZE_4KB <= Info->PlatPciIoLimit + 1){
      Info->IoBase[Index]  = Info->PlatPciIoCur;
      Info->IoSize[Index]  = SIZE_4KB;
      Info->PlatPciIoCur  += SIZE_4KB; 
    }
  }

  if(Info->IoSize[0]){
    Info->IoSize[0] += Info->IoBase[0];
    Info->IoBase[0]  = 0;
  }  
}


VOID
DumpRootBridgeResource (
  PLATFORM_COMM_INFO  *Info
  )
{
  UINTN                HostIndex;
  UINTN                HostBase;
 

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  for(HostIndex=0; HostIndex<Info->PciHostCount; HostIndex++){ 
    HostBase = PCI_DEV_MMBASE(0, 0x18 + HostIndex, 0);  
    DumpMem32((VOID*)(HostBase+0x200), 256);
    DumpMem32((VOID*)(HostBase+0xC0), 64);
  }
}  

 
VOID
ClearRootBridgeResource (
  PLATFORM_COMM_INFO  *Info
  )
{
  UINTN                Index;
  UINTN                HostIndex;
  UINTN                HostBase;
 

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  for(HostIndex=0; HostIndex<8; HostIndex++){ 
    HostBase = PCI_DEV_MMBASE(0, 0x18 + HostIndex, 0);  

    if(MmioRead32(HostBase+0x200) == 0xFFFFFFFF && MmioRead32(HostBase+0x204) == 0xFFFFFFFF &&
       MmioRead32(HostBase+0x208) == 0xFFFFFFFF && MmioRead32(HostBase+0x20C) == 0xFFFFFFFF){
      continue;
    }
    
    for(Index=0; Index<8; Index++) {

      MmioWrite32(HostBase+0x208+Index*0x10, 0);
      MmioWrite32(HostBase+0x200+Index*0x10, 0);
      MmioWrite32(HostBase+0x204+Index*0x10, 0);
    
      MmioWrite32(HostBase+0x288+Index*0x10, 0);
      MmioWrite32(HostBase+0x280+Index*0x10, 0);
      MmioWrite32(HostBase+0x284+Index*0x10, 0);

      MmioWrite32(HostBase+DF0_IOBASE_REG +Index*8, 0);
      MmioWrite32(HostBase+DF0_IOLIMIT_REG+Index*8, 0);         
    }
  }
}  


VOID
SetRootBridgeMmio64Range (
  PLATFORM_COMM_INFO  *Info,
  UINT8               Pci64Limit
  )
{
  UINTN                Index;
  UINT32               HostIndex;
  UINT64               Tom2;
  UINT64               MmioBaseAddr;
  UINT64               MmioEndAddr;
  UINT64               MmioSize;
  UINT64               MmioSize16MAlign;
  UINT64               Current;
  UINTN                HostBase;
  UINT32               Base;
  UINT32               Limit;
  EFI_STATUS           Status;
  //UINT64               Base;


 /* DEBUG((EFI_D_INFO, "Pci64Base:%d\n", Pci64Limit));

  switch(Pci64Limit){
    case 0:
    default:
      MmioEndAddr = 0x3FFFFFFFFFFFULL;              // 64T
      break;

    case 1:
      MmioEndAddr = 0x1FFFFFFFFFFFULL;              // 32T
      break;

    case 2:
      MmioEndAddr = 0x0FFFFFFFFFFFULL;              // 16
      break;      
  }*/

  MmioEndAddr  = 0x3FFFFFFFFFFULL;           // 4T
  Tom2 = AsmReadMsr64 (0xC001001D);
  MmioBaseAddr = (Tom2 > SIZE_4GB) ? Tom2 : SIZE_4GB;
//  if(MmioBaseAddr < Base){
//    MmioBaseAddr = Base;
//  }
  
  MmioSize     = MmioEndAddr - MmioBaseAddr + 1;
  MmioSize16MAlign = DivU64x32(MmioSize, Info->PciHostCount) & SIZE_16M_ALIGN;

  Current = (MmioEndAddr - MmioSize16MAlign + 1) & SIZE_16M_ALIGN;
  for(Index=0; Index<Info->PciHostCount; Index++){
    Info->Mmio64Base[Index] = Current;
    Info->Mmio64Size[Index] = MmioSize16MAlign;
    Current -= MmioSize16MAlign;
  }
  if(Index > 1){
    Info->Mmio64Size[Index-1] = Info->Mmio64Base[Index-2] - MmioBaseAddr;
    Info->Mmio64Base[Index-1] = MmioBaseAddr;
  }

  for(Index=0;Index<Info->PciHostCount;Index++){
    DEBUG((EFI_D_INFO, "PCI64[%d] %lX %lX\n", Index, Info->Mmio64Base[Index], Info->Mmio64Size[Index]));
  }

  for(HostIndex=0; HostIndex<Info->PciHostCount; HostIndex++){ 
    if(HostIndex >= Info->Dies) {
      HostBase = PCI_DEV_MMBASE(0, 0x18 + (HostIndex - Info->Dies +4), 0);  
    } else {
      HostBase = PCI_DEV_MMBASE(0, 0x18 + HostIndex, 0);  
    }
    
    for(Index=0; Index<Info->PciHostCount; Index++) {
      Base  = (UINT32)RShiftU64(Info->Mmio64Base[Index], 16);
      Limit = (UINT32)RShiftU64(Info->Mmio64Base[Index] + Info->Mmio64Size[Index] - 1, 16);
      if(Index >= Info->Dies){
        MmioWrite32(HostBase+0x280+(Index-Info->Dies+4)*0x10, Base);
        MmioWrite32(HostBase+0x284+(Index-Info->Dies+4)*0x10, Limit);
        MmioWrite32(HostBase+0x288+(Index-Info->Dies+4)*0x10, (Info->FabricId[Index] << 4)|3);      // WE, RE
        DEBUG((EFI_D_INFO, "MMIO Pro HostBase=%x Index = %x FabricId= %x Base=%x Limit=%x \n", HostBase, Index, Info->FabricId[Index], Base, Limit));       
      } else {
        MmioWrite32(HostBase+0x280+Index*0x10, Base);
        MmioWrite32(HostBase+0x284+Index*0x10, Limit);
        MmioWrite32(HostBase+0x288+Index*0x10, (Info->FabricId[Index] << 4)|3);      // WE, RE
        DEBUG((EFI_D_INFO, "MMIO Pro HostBase=%x Index = %x FabricId= %x  Base=%x Limit=%x\n", HostBase, Index, Info->FabricId[Index], Base, Limit));  
      }
    }
  }

  for(Index=0; Index<Info->PciHostCount; Index++) {
    Status = gDS->AddMemorySpace (
                    EfiGcdMemoryTypeMemoryMappedIo,
                    Info->Mmio64Base[Index],
                    Info->Mmio64Size[Index],
                    EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                    );
    DEBUG((EFI_D_INFO, "+MMIO64 (%lX,%lX) %r\n", Info->Mmio64Base[Index], Info->Mmio64Size[Index], Status));
  }
}



VOID PlatformAfterAllPciIoHook()
{
  PLATFORM_COMM_INFO  *Info;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  
  PrepareIoRes(Info);
  SetRootBridgeIoRange(Info);
}









EFI_STATUS 
PciFindCapabilityOffset(
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *Io,
  UINT64                           Address,
  UINT8                            *pPciCapId
  )
{
	EFI_STATUS  Status;
	UINT16			PciRegSts;
	UINT16			Data16;
	UINT8			  Offset;
	BOOLEAN			HasFound;


  HasFound = FALSE;
  
	Status = Io->Pci.Read(Io, EfiPciWidthUint16, Address | PCI_PRIMARY_STATUS_OFFSET, 1, &PciRegSts);
	if(PciRegSts & BIT4){		// have Capabilities linked list
		Status = Io->Pci.Read(Io, EfiPciWidthUint8, Address | PCI_CAPBILITY_POINTER_OFFSET, 1, &Offset);
		while(1){
			Status = Io->Pci.Read(Io, EfiPciWidthUint16, Address | Offset, 1, &Data16);
			if((UINT8)(Data16&0xFF) == *pPciCapId){
				HasFound = TRUE;
				*pPciCapId = Offset;
				break;						
			}else if((UINT8)(Data16>>8)==0x00){	// link list end.
				break;
			}
			Offset = (UINT8)(Data16>>8);
		}
	}
	
	if(HasFound){
		Status = EFI_SUCCESS;
	}else{
		Status = EFI_NOT_FOUND;
	}

	return Status;		
}


EFI_STATUS SetPcieSlotId(PCI_PREPROCESS_HOOK_PARAMETER *p)
{
  STATIC UINT32      SlotId = 10;
         EFI_STATUS  Status;
         UINT8       Offset;
         UINT32      Data32;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *Io;
  UINT64                           Address; 
//UINT8                            ClassCode[3];


  Io = p->Io;
  Address = *(UINT64*)&p->PciAddress;

  Io->Pci.Read(Io, EfiPciWidthUint32, Address|0, 1, &Data32);
  if(Data32 != 0x14531D94){
    return EFI_UNSUPPORTED;
  }

//  Io->Pci.Read(Io, EfiPciWidthUint8, Address|9, 3, ClassCode);
//  if(!(ClassCode[2] == 6 && ClassCode[1] == 4 && ClassCode[0] == 0)){
//    return EFI_UNSUPPORTED;
//  }

	Offset = EFI_PCI_CAPABILITY_ID_PCIEXP;
  Status = PciFindCapabilityOffset(Io, Address, &Offset);
  if(EFI_ERROR(Status)){
    return Status;
  }

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Address = Address | (Offset+0x14);

  Io->Pci.Read(Io, EfiPciWidthUint32, Address, 1, &Data32);
  DEBUG((EFI_D_INFO, "(%X,%X,%X) [%X] = %X\n", p->PciAddress.Bus, p->PciAddress.Device, p->PciAddress.Function, Offset+0x14, Data32));
  if((Data32 >> 19) == 0){                                      // only fill slot id is 0
    Data32 = (Data32 & 0x7FFFF) | (SlotId << 19);
    DEBUG((EFI_D_INFO, "-> %X %X\n", Data32, SlotId));  
    Io->Pci.Write(Io, EfiPciWidthUint32, Address, 1, &Data32);  
    SlotId++;
  }

//Io->Pci.Read(Io, EfiPciWidthUint32, Address, 1, &Data32);
//DEBUG((EFI_D_INFO, "-> %X\n", Data32));

  return EFI_SUCCESS;
}



VOID
PlatPreprocessPciHook (
  VOID  *Param   
  )
{
  PCI_PREPROCESS_HOOK_PARAMETER   *p;
//EFI_STATUS                      Status;
  
  p = (PCI_PREPROCESS_HOOK_PARAMETER*)Param;
  ASSERT(p->Sign == PCI_PREPROCESS_HOOK_PARAMETER_SIGN);

//if(p->Phase == EfiPciBeforeResourceCollection){
//  SetPcieSlotId(p);
//}
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
   
  
  DEBUG((DEBUG_INFO, "%a()\n", __FUNCTION__));

  gSetupData = GetSetupDataHobData();
  
  Status = LegacyBiosPlatformInstall(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);  
  
  Status = LegacyRegion2Install(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);  

  Status = PciPlatformInstall(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);    

  Status = SataControllerInstall(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);

//Status = IncompatiblePciDeviceSupportEntryPoint(ImageHandle, SystemTable);
//ASSERT_EFI_ERROR(Status);  

  Status = SmmAccessInit(ImageHandle, SystemTable);
  ASSERT_EFI_ERROR(Status);
  
  Status = MiscConfigDxe();
  ASSERT_EFI_ERROR(Status);

//Status = IsaAcpiDevListDxe();
  
//Status = PciHotPlugEntryPoint(ImageHandle, SystemTable);
//ASSERT_EFI_ERROR(Status);  

  EfiCreateProtocolNotifyEvent (
    &gExitPmAuthProtocolGuid,
    TPL_CALLBACK,
    ExitPmAuthCallBack,
    NULL,
    &Registration
    );   

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
    &gFchInitDonePolicyProtocolGuid,
    TPL_CALLBACK,
    FchInitDonePolicyCallBack,
    NULL,
    &Registration
    ); 

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
//-               &gPciPreprocessControllerHookProtocolGuid, PlatPreprocessPciHook,  
                  NULL
                  );
  
  return Status;
}




