
#include <IpmiCommon.h>
#include <Uefi.h>
#include <Base.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiBaseLib.h>
#include <Library/ResetSystemLib.h>
#include <Guid/HygonCbsConfig.h>
#include <Library/HygonCbsVariable.h>
#include <SetupVariable.h>
#include <ByoBootGroup.h>
#include <BmcConfig.h>
#include <Library/ByoCommLib.h>
#include <Library/PlatformCommLib.h>
#include <SystemPasswordVariable.h>
#include <BoardIdType.h>
#include <ProjectToken.h>
#include <Protocol/HygonVerProtocol.h>
#include <Library/ByoHygonCommLib.h>
#include <Library/ByoSharedSmmData.h>

BOOLEAN        gDoResetForApcbSync = FALSE;

EFI_STATUS
EFIAPI
SetOtherValueFromIpmi(
  BMC_CONFIG_SETUP         *ConfigOptions
  )
{
  EFI_STATUS      Status;
  UINT32          BIOSLock;
  BMC_SETUP_DATA  BmcSetupData;
  UINTN           BmcVarSize;

  DEBUG ((DEBUG_INFO, "SetOtherValueFromIpmi()\n"));

  BIOSLock = (UINT32)ConfigOptions->BIOSLock;

  Status = gRT->SetVariable (
                  FLASH_LOCK_STATUS_NAME,
                  &gEfiFlashLockStatusVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(UINT32),
                  (VOID *)&BIOSLock
                  );

  //
  // Set BMC Watch Dog
  //
  BmcVarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &BmcVarSize,
                  &BmcSetupData
                  );
  if (!EFI_ERROR (Status)) {
    BmcSetupData.WdtEnable[0] = ConfigOptions->FRB2WatchDog;
    Status = gRT->SetVariable (
                    BMC_SETUP_VARIABLE_NAME,
                    &gPlatformSetupVariableGuid,
                    BMC_SETUP_VARIABLE_FLAG,
                    BmcVarSize,
                    (VOID *)&BmcSetupData
                    );
    DEBUG ((DEBUG_INFO, "Set FRB2WatchDog:%x %r\n", ConfigOptions->FRB2WatchDog, Status));
  }

  return Status;
}

EFI_STATUS
SetSetupValueFromIpmi(
  BMC_CONFIG_SETUP         *ConfigOptions,
  SETUP_DATA               *SetupData
  )
{
  UINT16          BootTimeOutIpmiValue;
  
  DEBUG((EFI_D_INFO,"%a()\n",__FUNCTION__));

  //byte 1
  SetupData->CpuSmtMode = ConfigOptions->CpuSmtMode;
  SetupData->SriovDis = ConfigOptions->SriovDis;
  SetupData->CpuSvm = ConfigOptions->CpuSvm;
  if (ConfigOptions->CbsCmnGnbNbIOMMU == 2) {
    SetupData->CbsCmnGnbNbIOMMU = 0xf;
  } else {
    SetupData->CbsCmnGnbNbIOMMU = ConfigOptions->CbsCmnGnbNbIOMMU;
  }
  SetupData->PXEBootPriority = ConfigOptions->PXEBootPriority;
  //byte 2
  SetupData->LanBoot = ConfigOptions->LanBoot;
  // SetupData->BIOSLock = ConfigOptions->BIOSLock;  // this value need use FLASH_LOCK_STATUS_NAME variable
  SetupData->HideBrandLogo = ConfigOptions->HideBrandLogo;
  //byte 3
  SetupData->BootModeType = ConfigOptions->BootModeType + 1;
  if (ConfigOptions->Ipv4Support && ConfigOptions->Ipv6Support) {
    SetupData->NetBootIpVer = NET_BOOT_IP_ALL;
  } else if (ConfigOptions->Ipv4Support && !ConfigOptions->Ipv6Support) {
    SetupData->NetBootIpVer = NET_BOOT_IP_V4;
  } else if (!ConfigOptions->Ipv4Support && ConfigOptions->Ipv6Support) {
    SetupData->NetBootIpVer = NET_BOOT_IP_V6;
  } else {
    SetupData->NetBootIpVer = NET_BOOT_IP_NULL;
  }
  SetupData->AesSupportDis = ConfigOptions->AesSupportDis;
  //byte 7
  SetupData->UCREnable = ConfigOptions->UCREnable;
  SetupData->SerialBaudrate = ConfigOptions->SerialBaudrate;
  //byte 8
  SetupData->Pfeh = ConfigOptions->Pfeh;
  switch(ConfigOptions->McaErrThreshCount){
    case 0:
      SetupData->McaErrThreshCount = 0xFFF;
      break;
    case 1:
      SetupData->McaErrThreshCount = 0xFFE;
      break;
    case 2:
      SetupData->McaErrThreshCount = 0xFF5;
      break;
    case 3:
      SetupData->McaErrThreshCount = 0x0;
      break;  
    case 4:
      SetupData->McaErrThreshCount = 0xF9B;
      break;
    case 5:
      SetupData->McaErrThreshCount = 0xC17;
      break;
    case 6:
      SetupData->McaErrThreshCount = 0x82F;
      break;
    case 7:
      SetupData->McaErrThreshCount = 8190;
      break;
    default:
      SetupData->McaErrThreshCount = 8190;
      break;
  }
  SetupData->ObLanEn = ConfigOptions->ObLanEn;
  SetupData->PcieWake = ConfigOptions->WakeOnLan;
  SetupData->VideoPrimaryAdapter = ConfigOptions->VideoPrimaryAdapter;
  //byte 9
  SetupData->USBRearCtrl = ConfigOptions->USBRearCtrl;
  SetupData->USBFronCtrl = ConfigOptions->USBFronCtrl;
  SetupData->UsbMassSupport = ConfigOptions->UsbMassSupport;
  SetupData->PcieMaxPayloadSize = ConfigOptions->PcieMaxPayloadSize;
  if(ConfigOptions->PcieAspm == PCIE_ASPM_DISABLE || ConfigOptions->PcieAspm == PCIE_ASPM_L1){
    SetupData->PcieAspm = ConfigOptions->PcieAspm;
  } else {
    SetupData->PcieAspm = PCIE_ASPM_DISABLE;
  }
  //byte 10
  SetupData->PerfMode = ConfigOptions->PerfMode2;
  switch (ConfigOptions->PerfMode2) {
    case PERF_MODE_HIGH_PERF:
      SetupData->CpuPstateEn = CPU_P0_STATE;
      SetupData->CpuCstate   = 0;
      break;

    case PERF_MODE_HIGH_BALANCE:
      SetupData->CpuPstateEn = CPU_P0_STATE;
      SetupData->CpuCstate   = 0;
      break;

    case PERF_MODE_ENERGY_BALANCE:
      SetupData->CpuPstateEn = CPU_P0P1_STATE;
      SetupData->CpuCstate   = 0;
      break;

    case PERF_MODE_ENERGY:
      SetupData->CpuPstateEn = CPU_P0P1P2_STATE;
      SetupData->CpuCstate   = 0;
      break;

    case PERF_MODE_OEM:
    default:
      SetupData->CpuPstateEn = ConfigOptions->CpuPstateEn + 0x80;
      SetupData->CpuCstate   = ConfigOptions->CpuCstate;
      SetupData->CpuSpeed    = ConfigOptions->CpuSpeed;
      break;
  }

  //byte 11 12
  BootTimeOutIpmiValue = (ConfigOptions->BootTimeout[1] << 8) + (ConfigOptions->BootTimeout[0]);
  if (BootTimeOutIpmiValue >= 1 && BootTimeOutIpmiValue <= 65535) {
    SetupData->BootTimeout = BootTimeOutIpmiValue;
  }
  //byte 13
  if (ConfigOptions->QuietBoot) {
    SetupData->QuietBootMode = 0;
  } else {
    SetupData->QuietBootMode = 3;
  }
  SetupData->Numlock = ConfigOptions->Numlock;
  SetupData->LegacyOpromMessage = ConfigOptions->LegacyOpromMessage;
  SetupData->ShellEn = ConfigOptions->ShellEn;
  SetupData->UefiNetworkStack = ConfigOptions->UefiNetworkStack;
  SetupData->PxeRetryEn = ConfigOptions->RetryPxeBoot;

  switch(ConfigOptions->TerminalType){
    case 0: // VT100
      SetupData->TerminalType = 1;
      break;
    case 1: // VT100+
      SetupData->TerminalType = 2;
      break;
    case 2: // UTF-8
      SetupData->TerminalType = 3;
      break;
    case 3: // Linux
      SetupData->TerminalType = 5;
      break;  
    default: // VT100
      SetupData->TerminalType = 1;
      break;
  }

  return EFI_SUCCESS;
}
  
EFI_STATUS
SendSetupValueToIpmi(
  BMC_CONFIG_SETUP         *CommandData,
  SETUP_DATA               SetupData
  )
{
  DEBUG((EFI_D_INFO,"%a()\n",__FUNCTION__));

  //byte 1
  CommandData->CpuSmtMode = SetupData.CpuSmtMode;
  CommandData->SriovDis = SetupData.SriovDis;
  CommandData->CpuSvm = SetupData.CpuSvm;

  if (SetupData.CbsCmnGnbNbIOMMU == 0xf) {
    CommandData->CbsCmnGnbNbIOMMU = 2;
  } else {
    CommandData->CbsCmnGnbNbIOMMU = SetupData.CbsCmnGnbNbIOMMU;
  }
  CommandData->PXEBootPriority = SetupData.PXEBootPriority;
  //byte 2
  CommandData->LanBoot = SetupData.LanBoot;
  // CommandData->BIOSLock = SetupData.BIOSLock;  // this value need use FLASH_LOCK_STATUS_NAME variable
  CommandData->HideBrandLogo = SetupData.HideBrandLogo;
  //byte 3
  if(SetupData.BootModeType != 0){
    CommandData->BootModeType = SetupData.BootModeType - 1;
  }

  switch (SetupData.NetBootIpVer) {
    case NET_BOOT_IP_ALL:
      CommandData->Ipv4Support = 1;
      CommandData->Ipv6Support = 1;
      break;
    case NET_BOOT_IP_V4:
      CommandData->Ipv4Support = 1;
      CommandData->Ipv6Support = 0;
      break;
    case NET_BOOT_IP_V6:
      CommandData->Ipv4Support = 0;
      CommandData->Ipv6Support = 1;
      break;
    case NET_BOOT_IP_NULL:
      CommandData->Ipv4Support = 0;
      CommandData->Ipv6Support = 0;
      break;
    default:
      CommandData->Ipv4Support = 1;
      CommandData->Ipv6Support = 0;
      break;
  }

  CommandData->AesSupportDis = SetupData.AesSupportDis;

  //byte 7
  CommandData->UCREnable = SetupData.UCREnable;
  CommandData->SerialBaudrate = SetupData.SerialBaudrate;
  //byte 8
  CommandData->Pfeh = SetupData.Pfeh;
  switch(SetupData.McaErrThreshCount){
    case 0xFFF:
      CommandData->McaErrThreshCount = 0;
      break;
    case 0xFFE:
      CommandData->McaErrThreshCount = 1;
      break;
    case 0xFF5:
      CommandData->McaErrThreshCount = 2;
      break;
    case 0xF9B:
      CommandData->McaErrThreshCount = 4;
      break;  
    case 0xC17:
      CommandData->McaErrThreshCount = 5;
      break;
    case 0x82f:
      CommandData->McaErrThreshCount = 6;
      break;
    case 0:
      CommandData->McaErrThreshCount = 3;
      break;
    case 8190:
      CommandData->McaErrThreshCount = 7;
      break;
    default:
      CommandData->McaErrThreshCount = 7;
      break;
  }
  CommandData->ObLanEn = SetupData.ObLanEn;
  CommandData->WakeOnLan = SetupData.PcieWake;
  CommandData->VideoPrimaryAdapter = SetupData.VideoPrimaryAdapter;
  //byte 9
  CommandData->USBRearCtrl = SetupData.USBRearCtrl;
  CommandData->USBFronCtrl = SetupData.USBFronCtrl;
  CommandData->UsbMassSupport = SetupData.UsbMassSupport;
  CommandData->PcieMaxPayloadSize = SetupData.PcieMaxPayloadSize;
  CommandData->PcieAspm = SetupData.PcieAspm;
  //byte 10
  CommandData->PerfMode2 = SetupData.PerfMode;

  switch (SetupData.PerfMode) {
    case PERF_MODE_HIGH_PERF:
      CommandData->CpuPstateEn = CPU_P0_STATE - 0x80;
      CommandData->CpuCstate   = 0;
      break;

    case PERF_MODE_HIGH_BALANCE:
      CommandData->CpuPstateEn = CPU_P0_STATE - 0x80;
      CommandData->CpuCstate   = 0;
      break;

    case PERF_MODE_ENERGY_BALANCE:
      CommandData->CpuPstateEn = CPU_P0P1_STATE - 0x80;
      CommandData->CpuCstate   = SetupData.CpuCstate;
      break;

    case PERF_MODE_ENERGY:
      CommandData->CpuPstateEn = CPU_P0P1P2_STATE - 0x80;
      CommandData->CpuCstate   = 0;
      break;

    case PERF_MODE_OEM:
    default:
      CommandData->CpuPstateEn = SetupData.CpuPstateEn - 0x80;
      CommandData->CpuCstate = SetupData.CpuCstate;
      CommandData->CpuSpeed = SetupData.CpuSpeed;
      break;
  }

  //byte 11 12
  CommandData->BootTimeout[0] = (UINT8)SetupData.BootTimeout;
  CommandData->BootTimeout[1] = (UINT8)(SetupData.BootTimeout>>8);
  //byte 13
  if(SetupData.QuietBootMode == 3) {
    CommandData->QuietBoot = 0;
  } else {
    CommandData->QuietBoot = 1;
  }
  CommandData->Numlock = SetupData.Numlock;
  CommandData->LegacyOpromMessage = SetupData.LegacyOpromMessage;
  CommandData->ShellEn = SetupData.ShellEn;
  CommandData->UefiNetworkStack = SetupData.UefiNetworkStack;
  CommandData->RetryPxeBoot = SetupData.PxeRetryEn;

  switch(SetupData.TerminalType){
    case 1: // VT100
      CommandData->TerminalType = 0;
      break;
    case 2: // VT100+
      CommandData->TerminalType = 1;
      break;
    case 3: // UTF-8
      CommandData->TerminalType = 2;
      break;
    case 5: // Linux
      CommandData->TerminalType = 3;
      break;  
    default: // VT100
      CommandData->TerminalType = 0;
      break;
  }

  return EFI_SUCCESS;
}


EFI_STATUS
SendCbsValueToIpmi(
  BMC_CONFIG_SETUP         *CommandData,
  CBS_CONFIG CbsSetupData
  )
{
  DEBUG((EFI_D_INFO,"%a()\n",__FUNCTION__));


  if(CbsSetupData.CbsDfCmnMemIntlv == 7){
    CommandData->CbsDfCmnMemIntlv = 4;
  } else {
    CommandData->CbsDfCmnMemIntlv = CbsSetupData.CbsDfCmnMemIntlv;
  }
  CommandData->CbsCmnCpuRdseedRdrandCtrl = CbsSetupData.CbsCmnCpuRdseedRdrandCtrl;
  CommandData->CbsCmnCpuCpb = CbsSetupData.CbsCmnCpuCpb;
  CommandData->CbsCmnSmeeCtrl = CbsSetupData.CbsCmnSmeeCtrl;

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
SendOtherVariableToIpmi (
  BMC_CONFIG_SETUP         *CommandData
  )
{
  EFI_STATUS      Status;
  UINT32          SetupLock;
  UINTN           VariableSize;
  BMC_SETUP_DATA  BmcSetupData;
  UINTN           BmcVarSize;

  DEBUG((EFI_D_INFO,"SendOtherVariableToIpmi()\n"));

  VariableSize = sizeof(SetupLock);
  Status = gRT->GetVariable (
                  FLASH_LOCK_STATUS_NAME,
                  &gEfiFlashLockStatusVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupLock
                  );
  if (!EFI_ERROR (Status)) {
    CommandData->BIOSLock = (UINT8)SetupLock;
  } else {
    //
    // If get variable failed then set it to default(0)
    //
    CommandData->BIOSLock = 0;
  }


  //
  // Send BMC FRB2 Watch Dog
  //
  BmcVarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &BmcVarSize,
                  &BmcSetupData
                  );
  if (!EFI_ERROR (Status)) {
    CommandData->FRB2WatchDog = BmcSetupData.WdtEnable[0];
  } else {
    CommandData->FRB2WatchDog = PcdGet8(PcdFrb2WdtDefault);
  }

  return EFI_SUCCESS;
}


EFI_STATUS
SetLanguageFromIpmi(
  BMC_CONFIG_SETUP         *ConfigOptions
  )
{
  EFI_STATUS               Status;
  CHAR8                    *Language;

  DEBUG((EFI_D_INFO,"%a()\n",__FUNCTION__));

  if(ConfigOptions->Language == 0){
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
  return Status;
}

EFI_STATUS
SendLanguageToIpmi(
  BMC_CONFIG_SETUP         *ConfigOptions
  )
{
  EFI_STATUS               Status;
  CHAR8                    *CurrentLang;

  DEBUG((EFI_D_INFO,"%a()\n",__FUNCTION__));

  Status = GetEfiGlobalVariable2(L"PlatformLang", &CurrentLang, NULL);
  if (CurrentLang[0] == 'z') {
    ConfigOptions->Language = 0;
  } else {
    ConfigOptions->Language = 1;
  }
  if(CurrentLang != NULL){
    FreePool(CurrentLang);
  }

  return Status;
}
EFI_STATUS
CheckBootGroupUsable(
  UINT8                    BootModeType,
  UINT8                    *GroupOrder,
  UINTN                    GroupOrderSize
  )
{
  UINT8                    Index,Index2;

  if(BootModeType == BIOS_BOOT_UEFI_OS){
    for (Index = 0; Index < GroupOrderSize; Index++) {
      if (GroupOrder[Index] < BM_MENU_TYPE_UEFI_MIN ||
        GroupOrder[Index] > BM_MENU_TYPE_UEFI_MAX ||
        GroupOrder[Index] == BM_MENU_TYPE_UEFI_ODD ) {
        return EFI_UNSUPPORTED;
      }
    }
  } else if (BootModeType == BIOS_BOOT_LEGACY_OS) {
    for (Index = 0; Index < GroupOrderSize; Index++) {
      if (GroupOrder[Index] < BM_MENU_TYPE_LEGACY_MIN ||
        GroupOrder[Index] > BM_MENU_TYPE_LEGACY_MAX ||
        GroupOrder[Index] == BM_MENU_TYPE_LEGACY_ODD ||
        (GroupOrder[Index] == BM_MENU_TYPE_LEGACY_OTHERS && GroupOrderSize == 4)) {
        return EFI_UNSUPPORTED;
      }
    }
  }
  for (Index = 0; Index < GroupOrderSize - 1; Index++) {
    for(Index2 = Index + 1; Index2 < GroupOrderSize; Index2++) {
      if(GroupOrder[Index] == GroupOrder[Index2]) {
        return EFI_UNSUPPORTED;
      }
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
SetBootGroupOrderFromIpmi(
  BMC_CONFIG_SETUP         *ConfigOptions,
  SETUP_DATA               *SetupData
  )
{
  EFI_STATUS               Status;
  CHAR16                   *VariableName;
  UINTN                    GroupOrderSize = 0;
  UINT8                    *GroupOrder = NULL;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  /* j_t if (PcdGetBool(PcdSkipIpmiSetBootGroup)) {
    DEBUG((EFI_D_INFO,"Boot order should not synchronize with oem ipmi command\n"));
    return EFI_SUCCESS;
  }*/
  if (SetupData->BootModeType == BIOS_BOOT_LEGACY_OS) {//BIOS_BOOT_LEGACY_OS
    VariableName = BYO_LEGACY_BOOT_GROUP_VAR_NAME;
  } else if (SetupData->BootModeType == BIOS_BOOT_UEFI_OS){
    VariableName = BYO_UEFI_BOOT_GROUP_VAR_NAME;
  }
  
  Status = gRT->GetVariable(
                VariableName, 
                &gByoGlobalVariableGuid,
                NULL,
                &GroupOrderSize,
                NULL
                );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    GroupOrder = AllocatePool(GroupOrderSize);
    Status = gRT->GetVariable(
                VariableName, 
                &gByoGlobalVariableGuid,
                NULL,
                &GroupOrderSize,
                GroupOrder
                );
  } else {
    return EFI_UNSUPPORTED;
  }
  DumpMem8(GroupOrder,GroupOrderSize);
  if(SetupData->BootModeType == BIOS_BOOT_LEGACY_OS && GroupOrderSize == 4){
    GroupOrder[0] = ConfigOptions->FirstBoot  + 0x80;
    GroupOrder[1] = ConfigOptions->SecondBoot + 0x80;
    GroupOrder[2] = ConfigOptions->ThirdBoot  + 0x80;
    GroupOrder[3] = ConfigOptions->FourthBoot + 0x80;
  } else if (SetupData->BootModeType == BIOS_BOOT_LEGACY_OS && GroupOrderSize == 5) {
    //
    // Tencent BIOS has Other Legacy Group
    //
    GroupOrder[0] = ConfigOptions->FirstBoot  + 0x80;
    GroupOrder[1] = ConfigOptions->SecondBoot + 0x80;
    GroupOrder[2] = ConfigOptions->ThirdBoot  + 0x80;
    GroupOrder[3] = ConfigOptions->FourthBoot + 0x80;
    GroupOrder[4] = ConfigOptions->FifthBoot  + 0x80;
  } else if(SetupData->BootModeType == BIOS_BOOT_UEFI_OS && GroupOrderSize == 5){
    GroupOrder[0] = ConfigOptions->FirstBoot;
    GroupOrder[1] = ConfigOptions->SecondBoot;
    GroupOrder[2] = ConfigOptions->ThirdBoot;
    GroupOrder[3] = ConfigOptions->FourthBoot;
    GroupOrder[4] = ConfigOptions->FifthBoot;
  }
  Status = CheckBootGroupUsable(SetupData->BootModeType,GroupOrder,GroupOrderSize);
  if (Status != EFI_SUCCESS) {
    return Status;
  }
  DumpMem8(GroupOrder,GroupOrderSize);
  Status = gRT->SetVariable (
                  VariableName,
                  &gByoGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  GroupOrderSize,
                  GroupOrder
                  );
  return Status;
}

EFI_STATUS
SendBootGroupOrderToIpmi(
  BMC_CONFIG_SETUP         *ConfigOptions,
  SETUP_DATA               SetupData
  )
{
  EFI_STATUS               Status;
  CHAR16                   *VariableName;
  UINTN                    GroupOrderSize = 0;
  UINT8                    *GroupOrder = NULL;

  DEBUG((EFI_D_INFO,"%a()\n",__FUNCTION__));

  if (SetupData.BootModeType == BIOS_BOOT_LEGACY_OS) {//BIOS_BOOT_LEGACY_OS
    VariableName = BYO_LEGACY_BOOT_GROUP_VAR_NAME;
  } else if (SetupData.BootModeType == BIOS_BOOT_UEFI_OS){
    VariableName = BYO_UEFI_BOOT_GROUP_VAR_NAME;
  }
  
  Status = gRT->GetVariable(
                VariableName, 
                &gByoGlobalVariableGuid,
                NULL,
                &GroupOrderSize,
                NULL
                );
  if (Status == EFI_BUFFER_TOO_SMALL) {
    GroupOrder = AllocatePool(GroupOrderSize);
    Status = gRT->GetVariable(
                VariableName, 
                &gByoGlobalVariableGuid,
                NULL,
                &GroupOrderSize,
                GroupOrder
                );
  } else {
    return EFI_UNSUPPORTED;
  }
  DumpMem8(GroupOrder,GroupOrderSize);
  if (SetupData.BootModeType == BIOS_BOOT_LEGACY_OS) {
    ConfigOptions->FirstBoot  = GroupOrder[0] - 0x80;
    ConfigOptions->SecondBoot = GroupOrder[1] - 0x80;
    ConfigOptions->ThirdBoot  = GroupOrder[2] - 0x80;
    ConfigOptions->FourthBoot = GroupOrder[3] - 0x80;
    if (GroupOrderSize == 5) {
      ConfigOptions->FifthBoot = GroupOrder[4] - 0x80;
    } else {
      ConfigOptions->FifthBoot  = 0x5;
    }
    ConfigOptions->SixthBoot  = 0x1;  // DVDROMDrive
  } else if (SetupData.BootModeType == BIOS_BOOT_UEFI_OS) {
    ConfigOptions->FirstBoot  = GroupOrder[0];
    ConfigOptions->SecondBoot = GroupOrder[1];
    ConfigOptions->ThirdBoot  = GroupOrder[2];
    ConfigOptions->FourthBoot = GroupOrder[3];
    ConfigOptions->FifthBoot  = GroupOrder[4];
    ConfigOptions->SixthBoot  = 0x1;  // DVDROMDrive
  }

  return EFI_SUCCESS;
}



EFI_STATUS
WriteSetupAndCbsVariable(
  BMC_CONFIG_SETUP     *ConfigOptions
  )
{
  EFI_STATUS                    Status;
  SETUP_DATA                    SetupData;
  SETUP_DATA                    *SetupHob;
  UINTN                         Size;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  /* j_t if (PcdGet8(PcdSkipIpmiOemCmdSetVariable)) {
    DEBUG ((DEBUG_INFO, "ByoCfg set variables. Skip set variables.\n"));
    return EFI_SUCCESS;
  }*/

  Size = sizeof(SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );
  Status = SetSetupValueFromIpmi(ConfigOptions,&SetupData);
  DEBUG((EFI_D_INFO,"SetSetupValueFromIpmi Status - %r\n",Status));

  //
  // Sync BootModeType from Hob Data to Setup Variable
  //
  SetupHob = GetSetupDataHobData();
  SetupData.BootModeType = SetupHob->BootModeType;

  Status = gRT->SetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Size,
                  &SetupData
                  );

  Status = SetOtherValueFromIpmi(ConfigOptions);
  DEBUG((EFI_D_INFO,"SetOtherValueFromIpmi Status - %r\n", Status));

  Status = SetBootGroupOrderFromIpmi(ConfigOptions,&SetupData);
  DEBUG((EFI_D_INFO,"SetBootGroupOrderFromIpmi Status - %r\n",Status));

  Status = SetLanguageFromIpmi(ConfigOptions);
  DEBUG((EFI_D_INFO,"SetLanguageFromIpmi Status - %r\n",Status));

  return EFI_SUCCESS;
}

EFI_STATUS
SendBiosVariableToIpmi()
{
  EFI_STATUS                    Status;
  UINTN                         Size;
  BMC_CONFIG_SETUP              CommandData = {0};
  SETUP_DATA                    SetupData;
  CBS_CONFIG      CbsSetupData;
  UINT8                         Cmos;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  CommandData.ReadFlag = 1;
  Size = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );
  Size = sizeof (CBS_CONFIG);
  Status = gRT->GetVariable (
                  L"AmdSetup",
                  &gCbsSystemConfigurationGuid,
                  NULL,
                  &Size,
                  &CbsSetupData
                  );

  SendSetupValueToIpmi(&CommandData,SetupData);
  SendCbsValueToIpmi(&CommandData,CbsSetupData);
  SendOtherVariableToIpmi(&CommandData);
  SendLanguageToIpmi(&CommandData);
  SendBootGroupOrderToIpmi(&CommandData,SetupData);

  Status = EfiSendCommandToBMC (
             SM_BYOSOFT_NETFN_APP,
             0xc3,
             (UINT8 *) &CommandData,
             sizeof(CommandData),
             NULL,
             NULL
             );


  Cmos = PcdGet8(PcdNvVarSwSetCmosOffset);
  ASSERT(Cmos != 0xFF);
  //j_t if (PcdGetBool(PcdPlatformNvVariableSwUpdated) || PcdGet8(PcdSkipIpmiOemCmdSetVariable)){
  if (PcdGetBool(PcdPlatformNvVariableSwUpdated)){
    //
    // Clear CMOS offset 0x60 after send variable data to BMC if needed.
    //
    DEBUG ((DEBUG_INFO, "Clear CMOS offset 0x60.\n"));
    WriteCheckedCmosByte(Cmos, 0);
  }

  return EFI_SUCCESS;
}


EFI_STATUS
ProjectByoBdsEnterHook ()
{
  EFI_STATUS  Status;
  UINT8       RespondData[1] = {0};
  UINT8       ResponseDataSize;
  UINT8       CmdData[1] = {0};
  UINT8       CmdDataSize;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), gDoResetForApcbSync :%d.\n", __LINE__, gDoResetForApcbSync));
  if (PcdGetBool (PcdResetAfterIpmiChangeBiosSetup) || gDoResetForApcbSync) {
    //
    // Send variable to BMC if necessary.
    //
    SendBiosVariableToIpmi();
    DEBUG ((DEBUG_INFO, "BIOS Need reboot that some function in CBS can use correct value.\n"));
    if (PcdGetBool (PcdBiosSettingNeedLoadDefault)) {
      //
      // Clear BMC Load BIOS default value flag
      //
      CmdData[0] = 0;
      RespondData[0] = 0;
      CmdDataSize = sizeof(CmdData);
      Status = EfiSendCommandToBMC (
                 0x3E,
                 0x61,
                 (UINT8 *)&CmdData,
                 CmdDataSize,
                 (UINT8 *)&RespondData,
                 (UINT8 *)&ResponseDataSize
                 );
      DEBUG((EFI_D_INFO, "Set BMC load default CmdData: %r.\n", Status));
    }
    gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
  }

  return EFI_SUCCESS;
}



UINTN gApcbSyncDpxMatchCount = 0;

VOID CheckAndDoResetForApcbSync()
{
  BYO_SHARED_SMM_DATA     *SharedData;
  
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  if(gApcbSyncDpxMatchCount < 3){
    DEBUG((EFI_D_INFO, "Dpx not match\n"));
    return;
  }

  if(PcdGet8 (PcdIsPlatformCmosBad) && PcdGet8 (PcdCmosBadReason) == CMOS_BAD_REASON_NO_VARIABLE){ // first full flash power on.
    DEBUG((EFI_D_INFO, "ignore at first boot.\n"));
    return;
  }

  SharedData = (BYO_SHARED_SMM_DATA*)(UINTN)PcdGet64(PcdByoSharedSmmDataAddr);
  if (SharedData->Signature != BYO_SHARED_SMM_DATA_SIGNATURE) {
    DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), Bad Signature.\n", __LINE__));
    return;
  }
	
  DEBUG((EFI_D_INFO, "ApcbWritten:%d\n", SharedData->ApcbWritten));
  if(SharedData->ApcbWritten){
    DEBUG((EFI_D_INFO, "Hpcb updated, need do reset...\n"));
    // gRT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    gDoResetForApcbSync = TRUE;
  }
}


VOID
EFIAPI
CbsHobToVariableCallBack (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  VOID                       *Interface;
  EFI_STATUS                 Status;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  Status = gBS->LocateProtocol(&gHygonCbsVariableHobGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  gApcbSyncDpxMatchCount++;
  CheckAndDoResetForApcbSync();
}


VOID
EFIAPI
IpmiConfigSetupResetArchCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID                       *Interface;
  EFI_STATUS                 Status;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  Status = gBS->LocateProtocol(&gEfiResetArchProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  gApcbSyncDpxMatchCount++;
  CheckAndDoResetForApcbSync();
}


VOID
EFIAPI
IpmiConfigSetupAmdVersionStringCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID                       *Interface;
  EFI_STATUS                 Status;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n", __LINE__));
  Status = gBS->LocateProtocol(&gHygonVersionStringProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  gApcbSyncDpxMatchCount++;
  CheckAndDoResetForApcbSync();
}


EFI_STATUS
IpmiConfigSetupEntryPoint (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_PEI_HOB_POINTERS          GuidHob;
  BMC_CONFIG_SETUP             *ConfigOptions;
  VOID                         *Registration;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), j_d, PcdFceLoadDefault :%d.\n", __LINE__, PcdGetBool(PcdFceLoadDefault)));
  GuidHob.Raw = GetFirstGuidHob(&gIpmiConfigSetupInfoHobGuid);
  if (GuidHob.Raw != NULL && !PcdGetBool(PcdFceLoadDefault)) {
    DEBUG((EFI_D_INFO,"Sync data in variable and hob\n"));
    ConfigOptions = (BMC_CONFIG_SETUP*)GET_GUID_HOB_DATA (GuidHob);
    DumpMem8(ConfigOptions, sizeof(BMC_CONFIG_SETUP));
    WriteSetupAndCbsVariable(ConfigOptions);
  }

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gPlatformIpmiConfigSetupProtocolGuid, SendBiosVariableToIpmi,
                  &gByoBdsEnterHookGuid, ProjectByoBdsEnterHook,
                  NULL
                  );

  EfiCreateProtocolNotifyEvent (
    &gHygonCbsVariableHobGuid,
    TPL_CALLBACK,
    CbsHobToVariableCallBack,
    NULL,
    &Registration
    );
  EfiCreateProtocolNotifyEvent (
    &gEfiResetArchProtocolGuid,
    TPL_CALLBACK,
    IpmiConfigSetupResetArchCallBack,
    NULL,
    &Registration
    );
  EfiCreateProtocolNotifyEvent (
    &gHygonVersionStringProtocolGuid,
    TPL_CALLBACK,
    IpmiConfigSetupAmdVersionStringCallBack,
    NULL,
    &Registration
    );

  return EFI_SUCCESS;
}
