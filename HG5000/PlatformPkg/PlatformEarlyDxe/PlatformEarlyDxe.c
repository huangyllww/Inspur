

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
#include <Library/MtrrLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <PlatformDefinition.h>
#include <SetupVariable.h>
#include <Library/PlatformCommLib.h>
#include "FchPlatform.h"
#include <Protocol/FchInitProtocol.h>
#include <Token.h>
#include <SysMiscCfg.h>
#include <SysMiscCfg2.h>



extern EFI_GUID     gIgdGopDepexProtocolGuid;
extern EFI_GUID     gObLanUndiDxeDepexProtocolGuid;
CONST SETUP_DATA    *gSetupHob;

EFI_STATUS
PlatHostInfoInstall (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  );

typedef struct {
  UINT32  StartAddr;
  UINT32  RangeSize;
}	EFI_RESERVED_MEMORY_LIST;


STATIC EFI_RESERVED_MEMORY_LIST gRsvdMemList[] = {
  {_PCD_VALUE_PcdPciExpressBaseAddress, PCIEX_LENGTH},
  {_PCD_VALUE_PcdFlashAreaBaseAddress,  _PCD_VALUE_PcdFlashAreaSize}, 
};  
  

VOID SetReservedMmio()
{
  UINTN       Index;  
  EFI_STATUS  Status;
  
  for(Index = 0; Index < sizeof(gRsvdMemList)/sizeof(gRsvdMemList[0]); Index++){
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


typedef struct {
  UINT8   Type;
  UINT8   Reserved0:2;
  UINT8   FE:1;
  UINT8   E:1;
  UINT8   Reserved1:4;
  UINT8   Reserved2[6];
} IA32_MTRR_DEF;

VOID
ShowCacheSetting (
  VOID
  )
{
  MTRR_SETTINGS  MtrrSet;
  UINTN          Index;
  UINTN          VariableMtrrCount;
	IA32_MTRR_DEF  *DefMtrr;


  MtrrGetAllMtrrs(&MtrrSet);
  DefMtrr = (IA32_MTRR_DEF*)&MtrrSet.MtrrDefType;
  DEBUG((DEBUG_INFO, "T:%d, FE:%d, E:%d\n",   DefMtrr->Type, DefMtrr->FE, DefMtrr->E));  
  for (Index = 0; Index < MTRR_NUMBER_OF_FIXED_MTRR; Index++) {
    DEBUG((DEBUG_INFO, "%016lX\n", MtrrSet.Fixed.Mtrr[Index]));
  }
  
  VariableMtrrCount = GetVariableMtrrCount();
  for (Index = 0; Index < VariableMtrrCount; Index++) {
    if(!(MtrrSet.Variables.Mtrr[Index].Mask & BIT11)){     // unused, skip
      continue;
    }  
    DEBUG((DEBUG_INFO, "VarMTRR[%02d]: (%016lX,%016lX)\n", 
                       Index, 
                       MtrrSet.Variables.Mtrr[Index].Base, 
                       MtrrSet.Variables.Mtrr[Index].Mask
                       ));
  }  
}


// default [0, A0000) is WB
//
// MSRC001_001D [Top Of Memory 2] 
//   47:23 TOM2[47:23]: second top of memory. Read-write. Reset: Xh. Specifies the address 
//   divides between MMIO and DRAM. This value is normally placed above 4G. 
//   From 4G to TOM2 - 1 is DRAM; TOM2 and above is MMIO.
//
// MSRC001_001A [Top Of Memory] (Core::X86::Msr::TOP_MEM)
//   47:23 TOM[47:23]: top of memory. Read-write. Reset: Xh. Specifies the address that divides 
//   between MMIO and DRAM. This value is normally placed below 4G. From TOM to 4G is MMIO; 
//   below TOM is DRAM.
//
// MSRC001_0010 [System Configuration] (Core::X86::Msr::SYS_CFG)
//   [22] Tom2ForceMemTypeWB: top of memory 2 memory type write back. 
//   The default memory type of memory between 4GB and TOM2 is write back instead of the memory 
//   type defined by Core::X86::Msr::MTRRdefType[MemType]. For this bit to have any effect,
//   Core::X86::Msr::MTRRdefType[MtrrDefTypeEn] must be 1. MTRRs and PAT can be used to override 
//   this memory type.
//
//   [21] MtrrTom2En: MTRR top of memory 2 enable. Read-write. Reset: 0. 
//        0=Core::X86::Msr::TOM2 is disabled. 
//        1=Core::X86::Msr::TOM2 is enabled
//
//   [20] MtrrVarDramEn: MTRR variable DRAM enable. Read-write. Reset: 0. Init: BIOS,1.
//        0=Core::X86::Msr::TOP_MEM and IORRs are disabled. 
//        1=These registers are enabled.
//
//   [19] MtrrFixDramModEn: MTRR fixed RdDram and WrDram modification enable. Read-write. Reset: 0
//        0=Core::X86::Msr::MtrrFix_64K through Core::X86::Msr::MtrrFix_4K_7[RdDram,WrDram] read
//          values is masked 00b; writing does not change the hidden value. 
//        1=Core::X86::Msr::MtrrFix_64K through Core::X86::Msr::MtrrFix_4K_7 [RdDram,WrDram] access 
//          type is Read-write. Not shared between threads.
//   [18] MtrrFixDramEn: MTRR fixed RdDram and WrDram attributes enable. Read-write. Reset: 0
//        1=Enables the RdDram and WrDram attributes in Core::X86::Msr::MtrrFix_64K through 
//          Core::X86::Msr::MtrrFix_4K_7

VOID PlatformEarlyDxeEntryDebug()
{
  UINT64  MsrVal;

  MsrVal = AsmReadMsr64(0xC0010010);

  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0xC001001D, AsmReadMsr64(0xC001001D)));
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0xC001001A, AsmReadMsr64(0xC001001A)));
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0xC0010010, MsrVal));


  AsmWriteMsr64(0xC0010010, MsrVal | BIT18 | BIT19);

  DEBUG((EFI_D_INFO, "FixMtrr:\n"));
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x250, AsmReadMsr64(0x250)));
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x258, AsmReadMsr64(0x258)));
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x259, AsmReadMsr64(0x259)));
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x268, AsmReadMsr64(0x268))); 
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x269, AsmReadMsr64(0x269))); 
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x26A, AsmReadMsr64(0x26A))); 
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x26B, AsmReadMsr64(0x26B)));   
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x26C, AsmReadMsr64(0x26C)));  
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x26D, AsmReadMsr64(0x26D)));   
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x26E, AsmReadMsr64(0x26E))); 
  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0x26F, AsmReadMsr64(0x26F)));   

  DEBUG((EFI_D_INFO, "[%X]:%016lX\n", 0xC0010010, MsrVal)); 

  AsmWriteMsr64(0xC0010010, MsrVal);  

  ShowCacheSetting();
}

EFI_STATUS
EFIAPI
PlatformEarlyDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_BOOT_MODE                 BootMode;
  EFI_HANDLE                    Handle = NULL;
  UINT8                         BootModeType;	
  UINT32                        SystemMiscCfg;
  UINT8                         VideoRomPolicy;
  UINT8                         PxeRomPolicy;
  UINT8                         StorageRomPolicy;
  UINT8                         OtherRomPolicy;
  UINT64                        SataMultiDiePortShutDown, i;
  UINT64                        SataMultiDiePortMode;
  UINT8                         Csm;
  UINT8                         CpuP, CpuC;
  UINT32                        PxeBootVidDid =0;

  DEBUG((EFI_D_INFO, "PlatEarlyDxe\n"));

 // SetReservedMmio();   >>follow puti fix Tencent linux cannot Pxe install
//PlatformEarlyDxeEntryDebug();

  BootMode = GetBootModeHob();
  DEBUG((EFI_D_INFO, "BootMode:%X\n", BootMode));

  gSetupHob = GetSetupDataHobData();  
  
  BootModeType = gSetupHob->BootModeType;

  if(gSetupHob->AesSupport){
   AsmMsrOr64(0xC0011004,BIT57);  
  }else{
    AsmMsrAnd64(0xC0011004,(~BIT57));
  }

  DEBUG((EFI_D_INFO,"BootModeType Is1111 %d\n",BootModeType));
  if(PcdGetBool(PcdIpmiBootflag)){
  	BootModeType = PcdGet8(PcdIpmiBootMode);
  }else{
  	if(gSetupHob->LastBootModeType != 0){
	  BootModeType = gSetupHob->LastBootModeType;
	}
    }
  
  if(BootModeType == BIOS_BOOT_UEFI_OS){
    Csm = 0;
  } else {
    Csm = 1;
  }

  PcdSet8(PcdDealDimmError,gSetupHob->DealErrorDimm);
  PcdSet8(PcdLegacyBiosSupport, Csm);
  PcdSet8(PcdDisplayOptionRomMessage, gSetupHob->OpromMessage);
  PcdSet8(PcdUCREnable, gSetupHob->UCREnable);

  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);
  if(gSetupHob->VideoDualVga){
    SystemMiscCfg |= SYS_MISC_CFG_DUAL_VGA;
  }  
  if(gSetupHob->VideoPrimaryAdapter == DISPLAY_PRIMARY_IGD){
    SystemMiscCfg |= SYS_MISC_CFG_PRI_VGA_IGD;
  }
  if(gSetupHob->UmaAbove4G){
    SystemMiscCfg |= SYS_MISC_CFG_PCI64;
  }  
  if(gSetupHob->ShellEn){
    SystemMiscCfg |= SYS_MISC_CFG_SHELL_EN;
  }  
  if(gSetupHob->NvmeOpRomPriority){
    SystemMiscCfg |= SYS_MISC_NVME_ADDON_OPROM;
  }  
  PcdSet32(PcdSystemMiscConfig, SystemMiscCfg);

  if(gSetupHob->Numlock){
    PcdSetBool(PcdKeyboardNumberLockInitState, TRUE);  
  }

  PcdSet8(PcdNetBootIp4Ip6, gSetupHob->NetBootIpVer);  
  if (gSetupHob->SelectPxeNic) {
    PxeBootVidDid = gSetupHob->PxeNicVid|((gSetupHob->PxeNicDid)<<16);
    PcdSet32(PcdPxeNicVidDid, PxeBootVidDid); 
    PcdSet16(PcdPxeNicMacCrc, gSetupHob->PxeNicMacCrc); 
  }		
  DEBUG((EFI_D_INFO, __FUNCTION__"(), PcdPxeNicVidDid :0x%x, PcdPxeNicMacCrc :0x%x.\n",PxeBootVidDid, gSetupHob->PxeNicMacCrc));
  
  PcdSet16(PcdPostPromptTimeOut, gSetupHob->BootTimeout);
  PcdSet8(PcdBiosBootModeType, BootModeType);

  if (BootModeType == BIOS_BOOT_UEFI_OS) {
    VideoRomPolicy = ROM_POLICY_UEFI_FIRST;
    PxeRomPolicy = ROM_POLICY_UEFI_FIRST;
    StorageRomPolicy = ROM_POLICY_UEFI_FIRST;
    OtherRomPolicy = ROM_POLICY_UEFI_FIRST;
  } else if (BootModeType == BIOS_BOOT_LEGACY_OS) {
    VideoRomPolicy = ROM_POLICY_LEGACY_FIRST;
    PxeRomPolicy = ROM_POLICY_LEGACY_FIRST;
    StorageRomPolicy = ROM_POLICY_LEGACY_FIRST;
    OtherRomPolicy = ROM_POLICY_LEGACY_FIRST;
  }

// legacy : 0
// uefi   : 1
// no     : 2
  DEBUG((EFI_D_INFO, "OpRomPolicy(%d,%d,%d,%d)\n", \
    gSetupHob->VideoRomPolicy, gSetupHob->PxeRomPolicy, gSetupHob->StorageRomPolicy, gSetupHob->OtherRomPolicy));

  /*VideoRomPolicy = gSetupHob->VideoRomPolicy;
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
  }*/

  DEBUG((EFI_D_INFO, "OpRomPolicy(%d,%d,%d,%d)\n", \
    VideoRomPolicy, PxeRomPolicy, StorageRomPolicy, OtherRomPolicy));
  
  PcdSet8(PcdVideoOpRomLaunchPolicy, VideoRomPolicy);
  PcdSet8(PcdPxeOpRomLaunchPolicy, PxeRomPolicy);
  PcdSet8(PcdStorageOpRomLaunchPolicy, StorageRomPolicy);
  PcdSet8(PcdOtherOpRomLaunchPolicy, OtherRomPolicy);


  if(PxeRomPolicy == ROM_POLICY_UEFI_FIRST){
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

  
  //SATA {  //FchDxe
  PcdSet8(PcdSataPortPowerPort0, !gSetupHob->SATAPort[0]);
  PcdSet8(PcdSataPortPowerPort1, !gSetupHob->SATAPort[1]);
  PcdSet8(PcdSataPortPowerPort2, !gSetupHob->SATAPort[2]);
  PcdSet8(PcdSataPortPowerPort3, !gSetupHob->SATAPort[3]);
  PcdSet8(PcdSataPortPowerPort4, !gSetupHob->SATAPort[4]);
  PcdSet8(PcdSataPortPowerPort5, !gSetupHob->SATAPort[5]);
  PcdSet8(PcdSataPortPowerPort6, !gSetupHob->SATAPort[6]);
  PcdSet8(PcdSataPortPowerPort7, !gSetupHob->SATAPort[7]);
  SataMultiDiePortShutDown = 0;
  for (i=0; i<32; i++){
      if(i<16) {// die 0/1
          SataMultiDiePortShutDown |= (((UINT64)!(BOOLEAN)(gSetupHob->SATAPort[i]))<<i);
      }else {   // die 4/5
          SataMultiDiePortShutDown |= (((UINT64)!(BOOLEAN)(gSetupHob->SATAPort[i]))<<(i+16));
      }
  }
  PcdSet64(PcdSataMultiDiePortShutDown, SataMultiDiePortShutDown);//FchMultiFchDxe
  
  PcdSet8(PcdSataModePort0, gSetupHob->SATAPortSpeed[0]);  
  PcdSet8(PcdSataModePort1, gSetupHob->SATAPortSpeed[1]);  
  PcdSet8(PcdSataModePort2, gSetupHob->SATAPortSpeed[2]);  
  PcdSet8(PcdSataModePort3, gSetupHob->SATAPortSpeed[3]);  
  PcdSet8(PcdSataModePort4, gSetupHob->SATAPortSpeed[4]);  
  PcdSet8(PcdSataModePort5, gSetupHob->SATAPortSpeed[5]);  
  PcdSet8(PcdSataModePort6, gSetupHob->SATAPortSpeed[6]);  
  PcdSet8(PcdSataModePort7, gSetupHob->SATAPortSpeed[7]);  
  SataMultiDiePortMode = 0;
  for (i=0; i<32; i++){
      if(i<16) {// die 0/1
          SataMultiDiePortMode |= (((UINT64)(gSetupHob->SATAPortSpeed[i]))<<(i*2));
      }else {   // die 4/5
          SataMultiDiePortMode |= (((UINT64)(gSetupHob->SATAPortSpeed[i]))<<((i+16)*2));
      }
  }
  PcdSet64(PcdSataMultiDiePortMode, SataMultiDiePortMode);

  if(gSetupHob->PerfMode == 0){
    CpuP = gSetupHob->AMDPstate;
    CpuC = gSetupHob->AMDCstate;
  } else {
    CpuP = 2;
    CpuC = 0;
  }

  DEBUG((EFI_D_INFO, "CpuP:%d, CpuC:%d\n", CpuP, CpuC));

  PcdSet8(PcdAmdPstateMode, CpuP);
  PcdSet8(PcdAmdCStateMode, CpuC);

  
  PcdSetBool(PcdSrIovSupport, !(BOOLEAN)gSetupHob->SriovDis);
  PcdSet8 (PcdRetryPxeBoot, gSetupHob->RetryPxeBoot);

 /* if(gSetupHob->AfterPowerLoss == AFTER_POWER_LOSS_OFF || 
     gSetupHob->AfterPowerLoss == AFTER_POWER_LOSS_PREVIOUS ||
     gSetupHob->AfterPowerLoss == AFTER_POWER_LOSS_ON){
    PcdSet8(PcdPwrFailShadow, gSetupHob->AfterPowerLoss);
    DEBUG((EFI_D_INFO, "AfterPowerLoss:%d\n", gSetupHob->AfterPowerLoss));
  }*/

  PlatHostInfoInstall(ImageHandle, SystemTable);
  
  //clear SciEn Bit
  IoAnd8 ( PcdGet16 (PcdAmdFchCfgAcpiPm1CntBlkAddr), ~BIT0);
  
  DEBUG((EFI_D_INFO, "PlatEarlyDxeExit\n"));

  return EFI_SUCCESS;
}



