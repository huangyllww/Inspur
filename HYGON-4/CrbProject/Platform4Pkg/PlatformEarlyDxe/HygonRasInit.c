

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <HygonRas.h>
#include <SetupVariable.h>
#include "PlatHost.h"
#include <HygonRas.h>
#include <Library/PlatformCommLib.h>


STATIC BYO_RAS_POLICY_DATA gByoRasPolicyData;

EFI_STATUS PlatHygonRasPolicyEarlyDxe(PLATFORM_COMM_INFO *Info)
{
  EFI_STATUS               Status;
  ERR_ACT_CTRL_REG_LIST    *p = &gByoRasPolicyData.Eacr;
  UINT32                   PcieAerCeMask;
  UINT16                   McaErrThreshSwCount;
  UINT16                   McaErrThreshCount;
  UINT16                   McaNonMemErrThresh;


  DEBUG((EFI_D_INFO, "PlatHygonRasPolicyEarlyDxe\n"));

  ZeroMem(&gByoRasPolicyData, sizeof(gByoRasPolicyData));
  gByoRasPolicyData.Version = BYO_RAS_POLICY_DATA_VER;
  gByoRasPolicyData.Tag     = BYO_RAS_POLICY_DATA_TAG;

  gByoRasPolicyData.ApeiEinjDisable  = gSetupHob->ApeiEinjDis;
  gByoRasPolicyData.ApeiDisable      = gSetupHob->ApeiSupportDis;
  gByoRasPolicyData.ApeiEinjCpuCeDis = gSetupHob->EinjCpuCeDis;

  if(gSetupHob->ErrorActionCtrlMode){
    p->Valid = TRUE;
    p->PciePortActList[0].Value = gSetupHob->PcieSerrActionCtrl & 0x1F;
    p->PciePortActList[1].Value = gSetupHob->PcieIntFatalActionCtrl & 0x1F;
    p->PciePortActList[2].Value = 0xFFFFFFFF;
    p->PciePortActList[3].Value = gSetupHob->PcieIntCorrActionCtrl & 0x1F;
    p->PciePortActList[4].Value = gSetupHob->PcieExtFatalActionCtrl & 0x1F;
    p->PciePortActList[5].Value = gSetupHob->PcieExtNonFatalActionCtrl & 0x1F;
    p->PciePortActList[6].Value = gSetupHob->PcieExtCorrActionCtrl & 0x1F;
    p->PciePortActList[7].Value = gSetupHob->PcieParityErrActionCtrl & 0x1F;
    
    p->NbifActConfigList[0].Value = gSetupHob->NbifSerrActionCtrl & 0x1F;
    p->NbifActConfigList[1].Value = gSetupHob->NbifIntFatalActionCtrl & 0x1F;
    p->NbifActConfigList[2].Value = gSetupHob->NbifIntNonFatalActionCtrl & 0x1F;
    p->NbifActConfigList[3].Value = gSetupHob->NbifIntCorrActionCtrl & 0x1F;
    p->NbifActConfigList[4].Value = gSetupHob->NbifExtFatalActionCtrl & 0x1F;
    p->NbifActConfigList[5].Value = gSetupHob->NbifExtNonFatalActionCtrl & 0x1F;
    p->NbifActConfigList[6].Value = gSetupHob->NbifExtCorrActionCtrl & 0x1F;
    p->NbifActConfigList[7].Value = gSetupHob->NbifParityErrActionCtrl & 0x1F;
  }

  PcieAerCeMask = 0;
  if(gSetupHob->PcieAerCeMaskSet){
    if(gSetupHob->PcieCeMaskBadTLP){
      PcieAerCeMask |= BIT0;
    }
    if(gSetupHob->PcieCeMaskBadDll){
      PcieAerCeMask |= BIT1;
    }
    if(gSetupHob->PcieCeMaskRollOver){
      PcieAerCeMask |= BIT2;
    } 
    if(gSetupHob->PcieCeMaskTimeOut){
      PcieAerCeMask |= BIT3;
    } 
    if(gSetupHob->PcieCeMaskNonFatal){
      PcieAerCeMask |= BIT4;
    }     
  }
  gByoRasPolicyData.PcieAerCeMask        = PcieAerCeMask;
  gByoRasPolicyData.PcieAerUceMask       = PcdGet32(PcdPcieAerUceDefMask);
  gByoRasPolicyData.LeakybucketMinites   = gSetupHob->LeakybucketMinites;
  gByoRasPolicyData.LeakybucketOnceCount = gSetupHob->LeakybucketOnceCount;
  gByoRasPolicyData.Leakybucket24HClear  = gSetupHob->Leakybucket24HClear;  
  gByoRasPolicyData.MemTinyStormThresholdCount  = gSetupHob->MemCeTinyStormTC;
  gByoRasPolicyData.MemTinyStormTimeIntervalSec = gSetupHob->MemCeTinyStormSec;
  gByoRasPolicyData.PcieCeThreshold      = gSetupHob->PcieCeThreshold;

  McaErrThreshSwCount = gSetupHob->MemCeThresh;
  McaNonMemErrThresh  = gSetupHob->McaNonMemErrThresh;

  DEBUG((EFI_D_INFO, "McaErrThreshSwCount:%d, McaNonMemErrThresh:%d\n", McaErrThreshSwCount, McaNonMemErrThresh));

  if(McaErrThreshSwCount > 8000){
    McaErrThreshSwCount = 8000;
  } else if(McaErrThreshSwCount == 0){
    McaErrThreshSwCount = 4095;
  }
  McaErrThreshCount = McaErrThreshSwCount;
  if(McaErrThreshCount >= 0xFFF){
    McaErrThreshCount = 0;
  } else {
    McaErrThreshCount = 0xFFF - McaErrThreshCount;
  }  
  if(McaNonMemErrThresh >= 4096 || McaNonMemErrThresh == 0){
    McaNonMemErrThresh = McaErrThreshCount;
  } else {
    McaNonMemErrThresh = 0xFFF - McaNonMemErrThresh;
  }

  gByoRasPolicyData.McaErrThreshSwCount = McaErrThreshSwCount;
  gByoRasPolicyData.McaErrThreshCount   = McaErrThreshCount;
  gByoRasPolicyData.McaNonMemErrThresh  = McaNonMemErrThresh;

  DEBUG((EFI_D_INFO, "McaErrThreshSwCount:%d, McaErrThreshCount:%d, McaNonMemErrThresh:%d\n", \
    McaErrThreshSwCount, McaErrThreshCount, McaNonMemErrThresh));
  
  Status = gBS->InstallMultipleProtocolInterfaces(
                  &gImageHandle,
                  &gByoRasPolicyDataProtocolGuid, &gByoRasPolicyData,
                   NULL
                  );
  ASSERT(!EFI_ERROR(Status));

  Info->RasPolicyAddr = (UINTN)&gByoRasPolicyData;

  return EFI_SUCCESS;
}


