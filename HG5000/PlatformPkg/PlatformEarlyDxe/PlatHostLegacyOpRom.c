
#include "PlatHost.h"
#include <Protocol/AtaPassThru.h>
#include <Protocol/ScsiPassThruExt.h>
#include <PlatformDefinition.h>
#include <SysMiscCfg2.h>


BOOLEAN 
NvmeOpRomCheck (
  EFI_HANDLE               Handle,
  EFI_PCI_IO_PROTOCOL      *PciIo,
  PLAT_HOST_INFO_PROTOCOL  *HostInfo
  )
{
  UINT8                       ClassCode[3];
  EFI_STATUS                  Status;
  BOOLEAN                     HasImage = FALSE;
  
  
  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CC_PI_REG, 3, ClassCode);
   
  //
  // Examine Nvm Express controller PCI Configuration table fields
  //
  if ((ClassCode[0] != PCI_IF_NVMHCI) || (ClassCode[1] != PCI_CLASS_MASS_STORAGE_NVM) || (ClassCode[2] != PCI_CLASS_MASS_STORAGE)) {
    return FALSE;
  }

  if (PciIo->RomImage != NULL && PciIo->RomSize !=0){
    HasImage = TRUE;
	  DEBUG ((EFI_D_INFO, "NvmeOpRomCheck:RomImage=%x RonSize=%x\n",PciIo->RomImage,PciIo->RomSize));
  }
  
  if (gSetupHob->NvmeOpRomPriority == 1 && HasImage) {
  	return FALSE; // Addon OPROM exists and first
  }	
  
  return TRUE;  
}



BOOLEAN 
OnBoardLanOpRomCheck (
  EFI_HANDLE               Handle,
  EFI_PCI_IO_PROTOCOL      *PciIo,
  PLAT_HOST_INFO_PROTOCOL  *HostInfo
  )
{
  UINT8                       ClassCode[3];
  EFI_STATUS                  Status;
  EFI_DEVICE_PATH_PROTOCOL    *Dp;
  BOOLEAN                     Rc = FALSE;
  UINTN                       Index;
  BOOLEAN                     DpMatch = FALSE;


  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CC_PI_REG, 3, ClassCode);
  if(ClassCode[2] != PCI_CLASS_NETWORK){
    goto ProcExit;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  &Dp
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  

  if(HostInfo == NULL){
    goto ProcExit;
  } 

  for(Index=0;Index<HostInfo->HostCount;Index++){
    if(HostInfo->HostList[Index].HostType != PLATFORM_HOST_LAN){
      continue;
    }
    if(CompareMem(HostInfo->HostList[Index].Dp, Dp, GetDevicePathSize(Dp))==0){
      DpMatch = TRUE;
      break;
    } 
  }
  if(!DpMatch){
    goto ProcExit;
  }

// legacy : 0
// uefi   : 1
// no     : 2
  if(!gSetupHob->LanBoot || PcdGet8(PcdPxeOpRomLaunchPolicy) != ROM_POLICY_LEGACY_FIRST){
    goto ProcExit;  
  }

  Rc = TRUE;

ProcExit:
  return Rc;  
}



BOOLEAN 
OnBoardAhciOpRomCheck(
  EFI_HANDLE               Handle,
  EFI_PCI_IO_PROTOCOL      *PciIo,
  PLAT_HOST_INFO_PROTOCOL  *HostInfo
  )
{
  UINT8                           ClassCode[3];
  EFI_STATUS                      Status;
  UINTN                           Index;
  EFI_DEVICE_PATH_PROTOCOL        *Dp;
  EFI_ATA_PASS_THRU_PROTOCOL      *AtaPassThru;
  EFI_EXT_SCSI_PASS_THRU_PROTOCOL *ExtScsiPT;  
  UINT16                          SataPort;
  UINT16                          SataPortMp;
  UINT64                          Lun;  
  UINT8                           Target[TARGET_MAX_BYTES];
  UINT8                           *TargetId;  
  BOOLEAN                         Rc = FALSE;
  UINT32                          PciId;

  Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CC_PI_REG, 3, ClassCode);
  if(ClassCode[2] != PCI_BCC_STORAGE || ClassCode[1] != PCI_SCC_AHCI){
    goto ProcExit;
  }
  if(HostInfo == NULL){
    goto ProcExit;
  } 

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &PciId);
  if (PciId == 0x06221B21) { 
    goto ProcExit;
  }

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  &Dp
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  


  for(Index = 0; Index < HostInfo->HostCount; Index++){
    if(HostInfo->HostList[Index].HostType != PLATFORM_HOST_SATA){
      continue;
    }
    if(CompareMem(Dp, HostInfo->HostList[Index].Dp, GetDevicePathSize(Dp))){
      continue;
    }  
    
    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiAtaPassThruProtocolGuid,
                    &AtaPassThru
                    );
    if(EFI_ERROR(Status)){
      goto TryScsi;
    }
    SataPort = 0xFFFF;
    while (TRUE) {
      Status = AtaPassThru->GetNextPort(AtaPassThru, &SataPort);
      if (EFI_ERROR (Status)) {
        break;
      } 
      SataPortMp = 0xFFFF;
      while (TRUE) {
        Status = AtaPassThru->GetNextDevice(AtaPassThru, SataPort, &SataPortMp);
        if (EFI_ERROR (Status)) {
          break;
        }
        Rc = TRUE;
      }
    }

    if(Rc){
      break;
    }
    
  }

  if(Rc){
    goto ProcExit;
  }

TryScsi:
  Status = gBS->HandleProtocol(
                  Handle,
                  &gEfiExtScsiPassThruProtocolGuid,
                  &ExtScsiPT
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  Lun = 0;
  TargetId = &Target[0];
  SetMem(Target, sizeof(Target), 0xFF);
  Status = ExtScsiPT->GetNextTargetLun(ExtScsiPT, &TargetId, &Lun);
  if(!EFI_ERROR(Status)){
    Rc = TRUE;
  }
  
ProcExit:
  return Rc;
}



ADDITIONAL_ROM_TABLE  gLegacyOpRomTable[] = {
  {TRUE,  0, 0, (EFI_GUID*)PcdGetPtr(PcdObLanOpRomFile), OnBoardLanOpRomCheck},
  {TRUE,  0, 0, (EFI_GUID*)PcdGetPtr(PcdAhciOpRomFile),  OnBoardAhciOpRomCheck},
  {TRUE,  0, 0, (EFI_GUID*)PcdGetPtr(PcdNvmeOpRomFile),  NvmeOpRomCheck},
};

UINTN gLegacyOpRomTableSize = sizeof(gLegacyOpRomTable);


