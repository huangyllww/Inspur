
//------------------------------------------------------------------------------
#include "PlatformPei.h"
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <SetupVariable.h>
#include <ByoStatusCode.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/MultiPlatSupportLib.h>
#include <Library/DebugPrintErrorLevelLib.h>
#include <DhcpGetNetworldCfg.h>
#include <Library/ByoHygonCommLib.h>
#include <Library/GnbPciLib.h>


//------------------------------------------------------------------------------
extern EFI_GUID gByoSetupDataReadyPpiGuid;
extern EFI_GUID gByoSetupDataHobReadyPpiGuid;

EFI_STATUS 
GetDimmMapInfo (
    IN EFI_PEI_SERVICES           **PeiServices
  );

EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
PcieTrainingDoneCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
PlatformPeiEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gPpiNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiEndOfPeiSignalPpiGuid,
    PlatformPeiEndOfPeiCallback  
  },  
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gHygonNbioPcieTrainingDonePpiGuid,
    PcieTrainingDoneCallback
  },  
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    MemoryDiscoveredPpiNotifyCallback
  }
};

CONST EFI_PEI_PPI_DESCRIPTOR gSetupDataReadyPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gByoSetupDataReadyPpiGuid,
  NULL
};

CONST EFI_PEI_PPI_DESCRIPTOR gSetupDataHobReadyPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gByoSetupDataHobReadyPpiGuid,
  NULL
};


//----------------------------------------------------------------------------




EFI_STATUS
SetupDataInit (
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Var2Ppi,
  EFI_BOOT_MODE                    BootMode,
  PLATFORM_COMM_INFO               *Info,
  SETUP_DATA                       **pSetupDataOut
  )
{
  SETUP_DATA     *SetupData;
  EFI_STATUS     Status;
  EFI_STATUS     Status1;  
  UINTN          Size;
  BOOLEAN        IsCmosBad;
  SETUP_DATA     *VarHobSetupData = NULL;
  UINT8          Sriov;
  BOOLEAN        NeedDefault;
  UINT8          CmosBadReason = CMOS_BAD_REASON_UNKNOWN;
  UINT32         CmosBadPolicy = PcdGet32(PcdCmosBadPolicy);
  UINT32         DebugLevel;
  UINT8          Cmos;
  UINT8          CmosValue;
  UINT8          Csm;
  PASSWORD_NETWORK_AUTH_CONFIG  Pnac;
  BOOLEAN                       TextScreenEn;
  BOOLEAN                       LogoEn;  
  UINT8                         MaxReadRequestSize;


  DEBUG((EFI_D_INFO, "SetupDataInit, CmosBadPolicy:%x\n", CmosBadPolicy));

  SetupData = BuildGuidHob(&gPlatformSetupVariableGuid, sizeof(SETUP_DATA));
  ASSERT(SetupData != NULL);
  ZeroMem(SetupData, sizeof(SETUP_DATA));

  if(CmosBadPolicy & TKN_CMOS_BAD_POLICY_IGNORE){
    IsCmosBad = FALSE;
  } else {
    IsCmosBad = LibCheckCmosBad();
  }
  if(IsCmosBad){
    CmosBadReason = CMOS_BAD_REASON_JUMPER;
  }
  NeedDefault = PcdGetBool(PcdBiosSettingNeedLoadDefault);
  if(NeedDefault){
    CmosBadReason = CMOS_BAD_REASON_SW_SET;
  }
  DEBUG((EFI_D_INFO, "IsCmosBad:%d NeedDefault:%d BootMode:0x%X\n", IsCmosBad, NeedDefault, BootMode));
  if(NeedDefault || IsCmosBad || BootMode == BOOT_IN_RECOVERY_MODE){
    DEBUG((EFI_D_ERROR, "Setup Need Load Default\n"));
    goto LoadSetupDefault;
  }

  Size   = sizeof(SETUP_DATA);
  Status = Var2Ppi->GetVariable (
                      Var2Ppi,
                      PLATFORM_SETUP_VARIABLE_NAME,
                      &gPlatformSetupVariableGuid,
                      NULL,
                      &Size,
                      SetupData
                      );
  if(!EFI_ERROR(Status)){
    if (PcdGetBool (PcdSetupDataVersionCheck) && SetupData->Version != FixedPcdGet8(PcdSetupDataVersion)) {
      DEBUG((EFI_D_INFO, "setup data version changed(%d %d)\n", SetupData->Version, FixedPcdGet8(PcdSetupDataVersion)));
      CmosBadReason = CMOS_BAD_REASON_SW_UPDATE;
      goto LoadSetupDefault;
    }
    if(Size != sizeof(SETUP_DATA)){
      DEBUG((EFI_D_ERROR, "setup data size changed(%d %d)\n", Size, sizeof(SETUP_DATA)));
      CmosBadReason = CMOS_BAD_REASON_SW_UPDATE;      
      goto LoadSetupDefault;
    }
    goto GotSetupVariable;
  } else {
    Size   = sizeof(SETUP_DATA);
    Status = Var2Ppi->GetVariable (
                        Var2Ppi,
                        EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                        &gEfiMemoryTypeInformationGuid,
                        NULL,
                        &Size,
                        SetupData
                        );
    if(Status == EFI_NOT_FOUND){
      CmosBadReason = CMOS_BAD_REASON_NO_VARIABLE; 
    }
  }

LoadSetupDefault:
  PcdSet8S(PcdIsPlatformCmosBad, TRUE);
  PcdSet8S(PcdCmosBadReason, CmosBadReason);
  DEBUG((EFI_D_ERROR, "Load FCE default\n"));
  Status = CreateDefaultVariableHob(EFI_HII_DEFAULT_CLASS_STANDARD, 0, (VOID**)&VarHobSetupData);
  ASSERT_EFI_ERROR(Status);
  ASSERT(VarHobSetupData != NULL);
  PcdSetBoolS(PcdFceLoadDefault, TRUE);

// FCE tool has some limitation for gathering default value, so do override here.
//FCE_DEFAULT_VALUE_OVERRIDE_DATA;

  Size = sizeof(SETUP_DATA);
  Status = Var2Ppi->GetVariable (
                      Var2Ppi,
                      PLATFORM_SETUP_VARIABLE_NAME,
                      &gPlatformSetupVariableGuid,
                      NULL,
                      &Size,
                      SetupData
                      );
  ASSERT_EFI_ERROR(Status);




//------------------------------------------------------------------------------
GotSetupVariable:

  Status = PeiServicesInstallPpi(&gSetupDataHobReadyPpiList);
  ASSERT_EFI_ERROR(Status);

  switch (SetupData->SysDebugMode) {
    default:
    case 0:
      DebugLevel = FixedPcdGet32(PcdDebugLevelOem0);
      break;
    case 1:
      DebugLevel = FixedPcdGet32(PcdDebugLevelOem1);
      break;
    case 2:
      DebugLevel = FixedPcdGet32(PcdDebugLevelOem2);
      break;
    case 3:
      DebugLevel = FixedPcdGet32(PcdDebugLevelOem3);
      PcdSetBoolS(PcdHygonIdsDebugPrintEnable, TRUE);
      break;
  }
  SetDebugPrintErrorLevel(DebugLevel);
  DEBUG((DEBUG_INFO, "ErrorLevel: 0x%X\n", GetDebugPrintErrorLevel()));

  if (SetupData->AtaPhyDetectDelay >= 16) {
    PcdSet8S(PcdAtaPhyDetectDelay, SetupData->AtaPhyDetectDelay);
  }
  if (SetupData->UsbWaitPortStableStall >= 100) {
    PcdSet32S(PcdUsbWaitPortStableStall, SetupData->UsbWaitPortStableStall);
  }

  Info->LegacyPxeEn = SetupData->LanBoot;
  if(PcdGetBool(PcdSetupHasCsmItem)){
    Csm = SetupData->Csm;
  } else {
    if(SetupData->BootModeType == BIOS_BOOT_UEFI_OS){
      Csm = 0;
    } else {
      Csm = 1;
    }
  }
  PcdSet8S(PcdLegacyBiosSupport, Csm);
  Info->Csm = Csm;


  Sriov = !SetupData->SriovDis;
  PcdSetBoolS(PcdSrIovSupport, Sriov);
  PcdSetBoolS(PcdCfgPcieAriSupport, Sriov);
  
  PcdSet8S(PcdHygonCpuPstateMode, SetupData->CpuPstateEn);
//PcdSet8S(PcdHygonSmtMode, SetupData->CpuSmtMode);                    // Setup_Config->CbsCmnSmtMode
//PcdSet8S(PcdCfgMaxPayloadValue, SetupData->PcieMaxPayloadSize);      // CBS_CONFIG.CbsPcieMaxpayloadSize
  PcdSetBoolS(PcdHygonCpuAesInsSupport, !SetupData->AesSupportDis);

  switch(SetupData->PcieMaxReadReqSize){
    case PCIE_MAX_READ_REQ_AUTO:
    default:
      MaxReadRequestSize = 0xFF;
      break;

    case PCIE_MAX_READ_REQ_128:
      MaxReadRequestSize = MAX_READREQUESTSIZE_128;
      break;

    case PCIE_MAX_READ_REQ_256:
      MaxReadRequestSize = MAX_READREQUESTSIZE_256;
      break;

    case PCIE_MAX_READ_REQ_512:
      MaxReadRequestSize = MAX_READREQUESTSIZE_512;
      break;
  }
  if(MaxReadRequestSize != 0xFF){
    PcdSet8S(PcdCfgMaxReadRequestSize, MaxReadRequestSize);
  }
 
  if(SetupData->NumaDisable){
    PcdSetBoolS(PcdHygonAcpiSrat, FALSE);
    PcdSetBoolS(PcdHygonAcpiSlit, FALSE);    
  } else {
    PcdSetBoolS(PcdHygonAcpiSrat, TRUE);
    PcdSetBoolS(PcdHygonAcpiSlit, TRUE);  
  }

  DEBUG((EFI_D_INFO, "CpuPstateEn        : %d\n", SetupData->CpuPstateEn));
  DEBUG((EFI_D_INFO, "SriovDis           : %d\n", SetupData->SriovDis));
  DEBUG((EFI_D_INFO, "NumaDisable        : %d\n", SetupData->NumaDisable));  
  DEBUG((EFI_D_INFO, "PcieMaxReadReqSize : %d\n", SetupData->PcieMaxReadReqSize));  
//DEBUG((EFI_D_INFO, "PcieMaxPayloadSize : %d\n", SetupData->PcieMaxPayloadSize));  
  DEBUG((EFI_D_INFO, "AesSupportDis      : %d\n", SetupData->AesSupportDis));
  DEBUG((EFI_D_INFO, "UceResetIso        : %d\n", SetupData->UceResetIso));

  //
  // TPM   SetupHob->TpmSelect  PcdTpmSelect
  // Dis   0                    0xff
  // fTPM  1                    0x01
  // SPI   2                    0x00
  // LPC   3                    0x00
  //
  switch (SetupData->TpmSelect) {
    default:
    case 0:
      PcdSet8S (PcdTpmSelect, 0xFF);
      break;

    case 1:
      PcdSet8S (PcdTpmSelect, 0x01);
      PcdSetBoolS (PcdFtpmSupport, TRUE);
      break;

    case 2:
    case 3:
      PcdSet8S (PcdTpmSelect, 0x00);
      break; 
  }
  DEBUG((EFI_D_INFO, "TpmSelect :%d\n", SetupData->TpmSelect));  
  LibTpmDecode(SetupData->TpmSelect);

  switch(SetupData->QuietBootMode){
    default:
    case 0:
      TextScreenEn = TRUE;
      LogoEn       = TRUE;
      break;

    case 1:
      TextScreenEn = TRUE;
      LogoEn       = FALSE;
      break;

    case 2:
      TextScreenEn = FALSE;
      LogoEn       = TRUE;
      break;

    case 3:
      TextScreenEn = FALSE;
      LogoEn       = FALSE;
      break;      
  }

  PcdSetBoolS(PcdPostTextScreenEnable, TextScreenEn);
  if(!LogoEn){
    PcdSet32S(PcdSystemMiscConfig, PcdGet32(PcdSystemMiscConfig)|SYS_MISC_CFG_DIS_SHOW_LOGO);
  }

  Size = sizeof(PASSWORD_NETWORK_AUTH_CONFIG);
  Status1 = Var2Ppi->GetVariable (
                       Var2Ppi,
                       PASSWORD_NETWORK_AUTH_VAR_NAME,
                       &gByoPasswordNetworkAuthConfigGuid,
                       NULL,
                       &Size,
                       &Pnac
                       );
  DEBUG((EFI_D_INFO, "GetVar(pnac):%r, %d\n", Status1, Pnac.PwdNetworkAuthEnable));
  if(!EFI_ERROR(Status1)){
    PcdSetBoolS(PcdPwdNetworkAuthEnable, Pnac.PwdNetworkAuthEnable);
  }

  PcdSetBoolS(PcdResetOnMemoryTypeInformationChange, SetupData->S4Support);

  PcdSetBoolS(PcdResetCpuOnSyncFlood, !SetupData->ResetCpuOnSyncFloodDis);

  PcdSet32S(PcdCpmSmiThreshold, SetupData->RasCeSmiThreshold);

  Cmos      = PcdGet8(PcdNvVarSwSetCmosOffset);
  CmosValue = 0;
  if((Cmos != 0xFF && ReadCheckedCmosByte(Cmos, &CmosValue) && CmosValue) || PcdGet8(PcdIsPlatformCmosBad)){
    DEBUG((EFI_D_INFO, "NvVarSwSet\n"));
    PcdSetBoolS(PcdPlatformNvVariableSwUpdated, TRUE);
  }

  Status = PeiServicesInstallPpi(&gSetupDataReadyPpiList);
  ASSERT_EFI_ERROR(Status);

  *pSetupDataOut = SetupData;
  return Status;
}



EFI_STATUS
EFIAPI
PlatformPeiEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS  Status;
  
  Status = GetDimmMapInfo(PeiServices);

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
PlatformPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                       Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Var2Ppi;
  EFI_BOOT_MODE                    BootMode;
  PLATFORM_COMM_INFO               *Info;
  SETUP_DATA                       *SetupData;
  

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID**)&Var2Ppi
             );
  ASSERT_EFI_ERROR(Status);

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR(Status);

  SetupDataInit(Var2Ppi, BootMode, Info, &SetupData);

  Status = PeiServicesNotifyPpi(&gPpiNotifyList[0]);
  ASSERT_EFI_ERROR(Status);

  return Status;
}





