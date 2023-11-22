

#include <Uefi.h>
#include <Guid/MtcVendor.h>
#include <Library/IoLib.h>
#include <Library/SerialPortLib.h>
#include <Protocol/VariableWrite.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/MtrrLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <SetupVariable.h>
#include <Library/PlatformCommLib.h>
#include <Protocol/FchInitProtocol.h>
#include <SysMiscCfg.h>
#include <Guid/SystemMiscSetting.h>
#include <Library/DxeServicesLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/ByoBootManagerLib.h>
#include <Library/PrintLib.h>
#include <Library/DevicePathLib.h>
#include <Guid/GlobalVariable.h>


extern EFI_GUID     gIgdGopDepexProtocolGuid;
extern EFI_GUID     gObLanUndiDxeDepexProtocolGuid;
CONST SETUP_DATA    *gSetupHob;
EFI_BOOT_MODE       gBootMode;

VOID ProjectReadyToBootHook();
VOID DoCbsHpcbSync();
EFI_STATUS PlatHygonRasPolicyEarlyDxe(PLATFORM_COMM_INFO *Info);
EFI_STATUS GetHwInfo(VOID);

EFI_STATUS
PlatHostInfoInstall (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

typedef struct {
  UINT64  StartAddr;
  UINT64  RangeSize;
}  EFI_RESERVED_MEMORY_LIST;


STATIC EFI_RESERVED_MEMORY_LIST gRsvdMemList[] = {
#if FixedPcdGetBool(PcdReportPcieBaseAddressAreaReservedInMemMap)
  {FixedPcdGet64(PcdPciExpressBaseAddress), FixedPcdGet64(PcdPciExpressBaseSize)},
#endif  
  {0, 0},
};


VOID
EFIAPI
UpdateLockNumState (
  VOID
  )
{
  EFI_STATUS                         Status;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL	 *TextInEx;
  EFI_KEY_TOGGLE_STATE               State;

  Status = gBS->HandleProtocol(gST->ConsoleInHandle, &gEfiSimpleTextInputExProtocolGuid, (VOID**)&TextInEx);
  if(EFI_ERROR(Status)){
    return;
  }

  if (gSetupHob->Numlock) {
    State = EFI_NUM_LOCK_ACTIVE | EFI_TOGGLE_STATE_VALID;
  } else {
    State = EFI_TOGGLE_STATE_VALID;
  }
  TextInEx->SetState(TextInEx, &State);

  return;
}


VOID SetReservedMmio()
{
  UINTN       Index;
  EFI_STATUS  Status;

  for(Index = 0; Index < sizeof(gRsvdMemList)/sizeof(gRsvdMemList[0]); Index++){
    if(gRsvdMemList[Index].RangeSize == 0){
      continue;
    }
    Status = gDS->AddMemorySpace (
                    EfiGcdMemoryTypeReserved,
                    gRsvdMemList[Index].StartAddr,
                    gRsvdMemList[Index].RangeSize,
                    EFI_MEMORY_UC | EFI_MEMORY_RUNTIME
                    );
    ASSERT_EFI_ERROR(Status);

    Status = gDS->SetMemorySpaceAttributes(
                    gRsvdMemList[Index].StartAddr,
                    gRsvdMemList[Index].RangeSize,
                    EFI_MEMORY_RUNTIME
                    );
    DEBUG((EFI_D_INFO, "[%d] %08X %08X %r\n", Index, \
      gRsvdMemList[Index].StartAddr, gRsvdMemList[Index].RangeSize, Status));
    ASSERT_EFI_ERROR(Status);
  }
}



VOID
EFIAPI
PlatformEarlyDxeVarWriteCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID                       *Interface;
  EFI_STATUS                 Status;

  Status = gBS->LocateProtocol(&gEfiVariableWriteArchProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;    
  }
  gBS->CloseEvent(Event);

  DoCbsHpcbSync();
}



EFI_STATUS
EFIAPI
PlatformEarlyDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_HANDLE                    Handle = NULL;
  UINT32                        SystemMiscCfg;
  UINT8                         VideoRomPolicy;
  UINT8                         PxeRomPolicy;
  UINT8                         StorageRomPolicy;
  UINT8                         OtherRomPolicy;
  UINT8                         Csm;
  EFI_STATUS                    Status;
  SYSTEM_MISC_SETTING           *SystemMiscSetting;
  UINTN                         PcdSize;
  PLATFORM_COMM_INFO            *Info;
  UINT8                         BootModeType;	  
  BOOLEAN                       PwdNetworkAuthEn;
  VOID                          *Registration;


  DEBUG((EFI_D_INFO, "PlatEarlyDxe\n"));
  
  SetReservedMmio();

  gBootMode = GetBootModeHob();
  DEBUG((EFI_D_INFO, "BootMode:%X\n", gBootMode));

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  ASSERT(Info->ThisSize == sizeof(PLATFORM_COMM_INFO));
  Info->CpuSmtMode = PcdGet8(PcdHygonSmtMode);

  gSetupHob = GetSetupDataHobData();

  PcdSetBoolS(PcdNetBootHttpEnable, !gSetupHob->HttpBootDis);

  BootModeType = gSetupHob->BootModeType;
  Csm = PcdGet8(PcdLegacyBiosSupport);
  if(!Csm){BootModeType = BIOS_BOOT_UEFI_OS;}
  DEBUG((EFI_D_INFO, "Csm:%d, BootModeType:%d\n", Csm, BootModeType));

  SystemMiscSetting = (SYSTEM_MISC_SETTING *) PcdGetPtr (PcdSystemMiscSetting);
  SystemMiscSetting->SysSetting.Bits.LegacyBiosSupport = Csm;

  PcdSet8S(PcdDisplayOptionRomMessage, !gSetupHob->LegacyOpromMessageDis);

  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);

  if(gSetupHob->RomPolicy == 0){
    if(Csm){
      ((SETUP_DATA*)gSetupHob)->VideoRomPolicy   = ROM_POLICY_LEGACY_FIRST;
      ((SETUP_DATA*)gSetupHob)->PxeRomPolicy     = ROM_POLICY_LEGACY_FIRST;
      ((SETUP_DATA*)gSetupHob)->StorageRomPolicy = ROM_POLICY_LEGACY_FIRST;
      ((SETUP_DATA*)gSetupHob)->OtherRomPolicy   = ROM_POLICY_LEGACY_FIRST; 
    } else {
      ((SETUP_DATA*)gSetupHob)->VideoRomPolicy   = ROM_POLICY_UEFI_FIRST;
      ((SETUP_DATA*)gSetupHob)->PxeRomPolicy     = ROM_POLICY_UEFI_FIRST;
      ((SETUP_DATA*)gSetupHob)->StorageRomPolicy = ROM_POLICY_UEFI_FIRST;
      ((SETUP_DATA*)gSetupHob)->OtherRomPolicy   = ROM_POLICY_UEFI_FIRST;      
    }
  }

  if(gSetupHob->VideoPrimaryAdapter == DISPLAY_PRIMARY_IGD){
    SystemMiscCfg |= SYS_MISC_CFG_PRI_VGA_IGD;
    SystemMiscSetting->PciSetting.Bits.ConfigPriVgaIgd = 1;
  } else if(gSetupHob->VideoPrimaryAdapter == DISPLAY_PRIMARY_ALL){
    SystemMiscCfg |= SYS_MISC_CFG_DUAL_VGA;
  }
  if(gSetupHob->Above4GDecode){
    SystemMiscCfg |= SYS_MISC_CFG_PCI64;
  }
  if(gSetupHob->ShellEn && gSetupHob->BootModeType != BIOS_BOOT_LEGACY_OS){
    SystemMiscCfg |= SYS_MISC_CFG_SHELL_EN;
  }
  if(gSetupHob->NvmeOpRomPriority){
    SystemMiscCfg |= SYS_MISC_NVME_ADDON_OPROM;
    SystemMiscSetting->PciSetting.Bits.NvmeAddOnOprom = 1;
  }
  if(!gSetupHob->HddSmartCheckDis){
    SystemMiscCfg |= SYS_MISC_CFG_HDD_SMART_CHECK;
  }
  PcdSet32S(PcdSystemMiscConfig, SystemMiscCfg);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gBdsAfterConnectAllDefConsoleProtocolGuid,
                  UpdateLockNumState,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);  

  PcdSet8S(PcdNetBootIp4Ip6, gSetupHob->NetBootIpVer);
  PcdSet16S(PcdPostPromptTimeOut, gSetupHob->BootTimeout);
  PcdSet16S(PcdPlatformBootTimeOut, gSetupHob->BootTimeout);  
  PcdSet8S(PcdBiosBootModeType, BootModeType);

// legacy : 0
// uefi   : 1
// no     : 2
  DEBUG((EFI_D_INFO, "OpRomPolicy(%d,%d,%d,%d)\n", \
    gSetupHob->VideoRomPolicy, gSetupHob->PxeRomPolicy, gSetupHob->StorageRomPolicy, gSetupHob->OtherRomPolicy));

  VideoRomPolicy = gSetupHob->VideoRomPolicy;
  if(VideoRomPolicy == ROM_POLICY_LEGACY_FIRST){
    if(!Csm){
      VideoRomPolicy = ROM_POLICY_UEFI_FIRST;
    }
  } else if(VideoRomPolicy == ROM_POLICY_BY_CSM){
    if(Csm){
      VideoRomPolicy = ROM_POLICY_LEGACY_FIRST;
    } else {
      VideoRomPolicy = ROM_POLICY_UEFI_FIRST;
    }
  }

  PxeRomPolicy = gSetupHob->PxeRomPolicy;
  if(PxeRomPolicy == ROM_POLICY_LEGACY_FIRST){
    if(!Csm){
      PxeRomPolicy = ROM_POLICY_UEFI_FIRST;
    }
  } else if(PxeRomPolicy == ROM_POLICY_BY_CSM){
    if(Csm){
      PxeRomPolicy = ROM_POLICY_LEGACY_FIRST;
    } else {
      PxeRomPolicy = ROM_POLICY_UEFI_FIRST;
    }
  }

  StorageRomPolicy = gSetupHob->StorageRomPolicy;
  if(StorageRomPolicy == ROM_POLICY_LEGACY_FIRST){
    if(!Csm){
      StorageRomPolicy = ROM_POLICY_UEFI_FIRST;
    }
  } else if(StorageRomPolicy == ROM_POLICY_BY_CSM){
    if(Csm){
      StorageRomPolicy = ROM_POLICY_LEGACY_FIRST;
    } else {
      StorageRomPolicy = ROM_POLICY_UEFI_FIRST;
    }
  }

  OtherRomPolicy = gSetupHob->OtherRomPolicy;
  if(OtherRomPolicy == ROM_POLICY_LEGACY_FIRST){
    if(!Csm){
      OtherRomPolicy = ROM_POLICY_UEFI_FIRST;
    }
  } else if(OtherRomPolicy == ROM_POLICY_BY_CSM){
    if(Csm){
      OtherRomPolicy = ROM_POLICY_LEGACY_FIRST;
    } else {
      OtherRomPolicy = ROM_POLICY_UEFI_FIRST;
    }
  }

  DEBUG((EFI_D_INFO, "OpRomPolicy(%d,%d,%d,%d)\n", \
    VideoRomPolicy, PxeRomPolicy, StorageRomPolicy, OtherRomPolicy));

  PcdSet8S(PcdVideoOpRomLaunchPolicy, VideoRomPolicy);
  PcdSet8S(PcdPxeOpRomLaunchPolicy, PxeRomPolicy);
  PcdSet8S(PcdStorageOpRomLaunchPolicy, StorageRomPolicy);
  PcdSet8S(PcdOtherOpRomLaunchPolicy, OtherRomPolicy);

  SystemMiscSetting->PciSetting.Bits.VideoOpRomLaunchPolicy   = VideoRomPolicy;
  SystemMiscSetting->PciSetting.Bits.PxeOpRomLaunchPolicy     = PxeRomPolicy;
  SystemMiscSetting->PciSetting.Bits.StorageOpRomLaunchPolicy = StorageRomPolicy;
  SystemMiscSetting->PciSetting.Bits.OtherOpRomLaunchPolicy   = OtherRomPolicy;

  PwdNetworkAuthEn = PcdGetBool(PcdPwdNetworkAuthEnable);
  if(PwdNetworkAuthEn){
    SystemMiscSetting->PciSetting.Bits.PxeOpRomLaunchPolicy = ROM_POLICY_UEFI_FIRST;
  }

  PcdSize = PcdGetSize (PcdSystemMiscSetting);
  PcdSetPtrS (PcdSystemMiscSetting, &PcdSize, (VOID *)SystemMiscSetting);

  if(PwdNetworkAuthEn || (gSetupHob->ObLanEn && PxeRomPolicy == ROM_POLICY_UEFI_FIRST)){
    gBS->InstallProtocolInterface (
           &Handle,
           &gObLanUndiDxeDepexProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  }
  if (VideoRomPolicy == ROM_POLICY_UEFI_FIRST) {
    gBS->InstallProtocolInterface (
           &Handle,
           &gIgdGopDepexProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
    DEBUG((EFI_D_INFO, "InstallIgdGopDpx\n"));
  }


  gBS->InstallProtocolInterface (
         &Handle,
         Csm ? (&gByoCsmOnDpxGuid) : (&gByoCsmOffDpxGuid),
         EFI_NATIVE_INTERFACE,
         NULL
         );
  
  gBS->InstallProtocolInterface (
         &Handle,
         (EFI_GUID*)PcdGetPtr(PcdTpmTcmModuleDpxGuid),
         EFI_NATIVE_INTERFACE,
         NULL
         );

  if(gSetupHob->PxeRetryEn){
    PcdSet8S(PcdRetryPxeBoot, TRUE);
    PcdSet16S(PcdPxeRetrylimites, gSetupHob->PxeRetryEn);    
  } else {
    PcdSet8S(PcdRetryPxeBoot, FALSE);
  }

  PlatHostInfoInstall(ImageHandle, SystemTable);

  EfiCreateProtocolNotifyEvent (
    &gEfiVariableWriteArchProtocolGuid,
    TPL_CALLBACK,
    PlatformEarlyDxeVarWriteCallBack,
    NULL,
    &Registration
    );

  PlatHygonRasPolicyEarlyDxe(Info);
  GetHwInfo();
  
  DEBUG((EFI_D_INFO, "CpuSmtMode:%d\n", Info->CpuSmtMode));
  DEBUG((EFI_D_INFO, "PlatEarlyDxeExit\n"));

  return EFI_SUCCESS;
}

