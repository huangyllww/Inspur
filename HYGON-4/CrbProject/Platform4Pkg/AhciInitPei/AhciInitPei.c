/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/



#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/TimerLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <PciDevicePath.h>
#include <Ppi/AtaAhciController.h>
#include <Ppi/ByoPciConfigRestore.h>
#include <Library/ByoHygonCommLib.h>



#define MAX_AHCI_HOST_SUPPORT         4

typedef struct {
  UINTN  AhciCount;
  UINTN  AhcrBar[MAX_AHCI_HOST_SUPPORT];
  UINT8  AhciIndex[MAX_AHCI_HOST_SUPPORT];
  UINT8  HostBusNo[MAX_AHCI_HOST_SUPPORT];  
} AHCI_PEI_CTX;

STATIC AHCI_PEI_CTX gAhciPeiCtx = {
  0,
  {0,},
  {0,},  
  {0,}, 
};



STATIC
EFI_STATUS
AllocateMmio32 (
  IN  UINT8       Bus,
  IN  UINT32      Length,
  IN  UINT32      Align,
  OUT UINTN       *pMmio
  )
{
  FABRIC_MMIO_ATTRIBUTE                 MmioAttr;
  FABRIC_TARGET                         Target;
  UINT64                                BaseAddress;
  UINT64                                AddrLen = Length;
  EFI_STATUS                            Status;

  MmioAttr.MmioType = MMIO_BELOW_4G;
  Target.TgtType    = TARGET_PCI_BUS;
  Target.PciBusNum  = Bus;
  Status = FabricAllocateMmio (
             &BaseAddress, 
             &AddrLen, 
             Align, 
             Target, 
             &MmioAttr
             );
  if(!EFI_ERROR(Status)){
    *pMmio = (UINT32)BaseAddress;
  } else {
    DEBUG((EFI_D_INFO, "AllocateMmio32:%r\n", Status));
  }
  return Status;
}



STATIC BOARDCARD_PATH2  gAhciDp = {DP_ROOT(0x00), DP_PCI(0x08, 0x01), DP_PCI(0x00, 0x02), DP_END};

EFI_STATUS
AhciHcGetMmioBar (
  IN  EDKII_ATA_AHCI_HOST_CONTROLLER_PPI    *This,
  IN  UINT8                                 ControllerId,
  OUT UINTN                                 *MmioBar
  )
{
  if(This == NULL || MmioBar == NULL){
    return EFI_INVALID_PARAMETER;
  }

  if(gAhciPeiCtx.AhciCount <= ControllerId){
    return EFI_INVALID_PARAMETER;
  }

  *MmioBar = gAhciPeiCtx.AhcrBar[ControllerId];
  DEBUG((EFI_D_INFO, "AhciHcGetMmioBar:%X\n", *MmioBar));    
  return EFI_SUCCESS;
}


EFI_STATUS
AhciHcGetDevicePath (
  IN  EDKII_ATA_AHCI_HOST_CONTROLLER_PPI    *This,
  IN  UINT8                                 ControllerId,
  OUT UINTN                                 *DevicePathLength,
  OUT EFI_DEVICE_PATH_PROTOCOL              **DevicePath
  )
{
  if(This == NULL || DevicePathLength == NULL || DevicePath == NULL){
    return EFI_INVALID_PARAMETER;
  }

  if(gAhciPeiCtx.AhciCount <= ControllerId){
    return EFI_INVALID_PARAMETER;
  }

  gAhciDp.PciRootBridgeDevicePath.UID = gAhciPeiCtx.AhciIndex[ControllerId];
  *DevicePath = (EFI_DEVICE_PATH_PROTOCOL*)&gAhciDp;  
  *DevicePathLength = sizeof(BOARDCARD_PATH2);
  return EFI_SUCCESS;

}


EDKII_ATA_AHCI_HOST_CONTROLLER_PPI gAhciHostPpi = {
  AhciHcGetMmioBar,
  AhciHcGetDevicePath
};





EFI_STATUS HygonAhciMmioDecodeAndReport(UINT8 Index, PLATFORM_COMM_INFO *PlatCommInfo, EFI_BOOT_MODE BootMode)
{
  UINTN               AhciBar;
  UINTN               BridgeBase;
  UINT16              MmioStart;
  UINT16              MmioEnd;
  UINT32              MmioRange;
  EFI_STATUS          Status;
  UINT8               PciBusNo;
  UINT8               SubBus;
  UINT8               AhciBus;
  UINTN               AhciPciBase;
  

  DEBUG((EFI_D_INFO, "HygonAhciMmioDecode(%d)\n", Index));  

  PciBusNo = PlatCommInfo->BusBase[Index];

  BridgeBase = PCI_DEV_MMBASE(PciBusNo, 8, 1);
  if((UINT16)MmioRead32(BridgeBase) != HG_VENDOR_ID){
    return EFI_NOT_FOUND;
  }

  if(BootMode == BOOT_IN_RECOVERY_MODE){

    AhciBus = 5 | PciBusNo;
    AhciPciBase = PCI_DEV_MMBASE(AhciBus, 0, 2);

    MmioWrite32(BridgeBase+0x18, PciBusNo | (AhciBus << 8) | (AhciBus << 16));
    if((UINT16)MmioRead32(AhciPciBase) != HG_VENDOR_ID){
      MmioWrite32(BridgeBase+0x18, 0);
      DEBUG((EFI_D_INFO, "Ahci[%d] not present\n", Index));
      return EFI_NOT_FOUND;      
    }
    
    Status = AllocateMmio32(PciBusNo, 0x1000, 0xFFF, &AhciBar);
    if(EFI_ERROR(Status)){
      return Status;
    } 
    
    MmioStart = (UINT16)(AhciBar >> 20) << 4;
    MmioEnd   = (UINT16)((AhciBar + 0x1000 - 1) >> 20) << 4;
    MmioRange = MmioStart | (MmioEnd << 16);
    DEBUG((EFI_D_INFO, "AhciBar:%X Mmio(%X,%X) %X\n", AhciBar, MmioStart, MmioEnd, MmioRange));
    MmioWrite32(BridgeBase+0x20, MmioRange);

    MmioWrite32(AhciPciBase+0x24, AhciBar);
    MmioWrite8(AhciPciBase+4, 6);
    MmioWrite8(BridgeBase+4, 6); 

    if(MmioRead32(AhciBar+0x10) == 0xFFFFFFFF){
      MmioWrite32(AhciPciBase+0x24, 0);
      MmioWrite8(AhciPciBase+4, 0);
      MmioWrite8(BridgeBase+4, 0);       
      MmioWrite32(BridgeBase+0x20, 0);
      MmioWrite32(BridgeBase+0x18, 0);
      DEBUG((EFI_D_INFO, "Ahci[%d] not decode\n", Index));
      return EFI_NOT_FOUND;       
    }

    DumpMem32((VOID*)BridgeBase, 256);
    DumpMem32((VOID*)AhciPciBase, 256);  
    DumpMem32((VOID*)(UINTN)AhciBar, 0x40);
    
  }else if(BootMode == BOOT_ON_S3_RESUME){
    BridgeBase = PCI_DEV_MMBASE(PciBusNo, 8, 1);
    SubBus = MmioRead8(BridgeBase+0x19);
    AhciPciBase = PCI_DEV_MMBASE(SubBus, 0, 2);
    AhciBar = MmioRead32(AhciPciBase+0x24) & 0xFFFFFFF0;
    DEBUG((EFI_D_INFO, "AhciBar:%X\n", AhciBar));

    DumpMem32((VOID*)BridgeBase, 256);
    DumpMem32((VOID*)AhciPciBase, 256);  
    DumpMem32((VOID*)(UINTN)AhciBar, 0x40);

  } else {
    return EFI_UNSUPPORTED;
  }

  if(gAhciPeiCtx.AhciCount < MAX_AHCI_HOST_SUPPORT){
    gAhciPeiCtx.AhcrBar[gAhciPeiCtx.AhciCount]   = AhciBar;
    gAhciPeiCtx.AhciIndex[gAhciPeiCtx.AhciCount] = Index;
    gAhciPeiCtx.HostBusNo[gAhciPeiCtx.AhciCount] = PciBusNo;
    gAhciPeiCtx.AhciCount++;
  }

  return EFI_SUCCESS;
}




EFI_STATUS
EFIAPI
AhciEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  UINTN               Base;
  PLATFORM_COMM_INFO  *PlatCommInfo;
  UINTN               Index;
  UINT8               PciBusNo;
  EFI_BOOT_MODE       BootMode;
  UINT8               AhciBus;


  DEBUG((EFI_D_INFO, "AhciEndOfPeiCallback\n"));

  BootMode = GetBootModeHob();
  if(BootMode == BOOT_ON_S3_RESUME){
    return EFI_SUCCESS;
  }

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  for(Index=0;Index<gAhciPeiCtx.AhciCount;Index++){

    PciBusNo = gAhciPeiCtx.HostBusNo[Index];
    AhciBus  = 5 | PciBusNo;

    MmioWrite8(PCI_DEV_MMBASE(AhciBus, 0, 2)+4, 0);
    MmioWrite8(PCI_DEV_MMBASE(PciBusNo, 8, 1)+4, 0);

    Base = PCI_DEV_MMBASE(AhciBus, 0, 2);
    MmioWrite32(Base+0x24, 0);

    Base = PCI_DEV_MMBASE(PciBusNo, 8, 1);
    MmioWrite32(Base+0x20, 0);

    DumpMem32((VOID*)PCI_DEV_MMBASE(PciBusNo, 8, 1), 256);
    DumpMem32((VOID*)PCI_DEV_MMBASE(AhciBus, 0, 2), 256);

    MmioWrite32(Base+0x18, 0);
  }

  return EFI_SUCCESS;
}



static EFI_PEI_PPI_DESCRIPTOR gPpiList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEdkiiPeiAtaAhciHostControllerPpiGuid,
    &gAhciHostPpi
  }
};

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiEndOfPeiSignalPpiGuid,
    AhciEndOfPeiCallback 
  },
};



EFI_STATUS
EFIAPI
AhciInitPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS       Status;
  EFI_BOOT_MODE    BootMode;
  BYO_RESTORE_DEVICE_PCI_CONFIG_PPI  *RestorePciConfig;
  PLATFORM_COMM_INFO  *PlatCommInfo;
  UINT8               Index;
 
  DEBUG ((EFI_D_INFO, "AhciInitPeiEntry\n"));

  BootMode = GetBootModeHob();

  if(BootMode == BOOT_ON_S3_RESUME){  
    //
    // Restore PCI Configuration Space for AHCI Controller and Upstream PCI Bridge
    //
    Status = PeiServicesLocatePpi (
                &gByoRestorePciDeviceConfigPpiGuid,
                0,
                NULL,
                (VOID **) &RestorePciConfig
                );
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "Fail to Allocate ByoRestorePciDeviceConfigPpi\n"));
      return Status;
    }
  
    Status = RestorePciConfig->RestoreDevicePciConfig();
    if (EFI_ERROR(Status)) {
      DEBUG((DEBUG_ERROR, "Fail to Restore Device PCI Configuration Space\n"));
      return Status;
    }
  }

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  for(Index=0;Index<PlatCommInfo->PciHostCount;Index++){
    HygonAhciMmioDecodeAndReport(Index, PlatCommInfo, BootMode);
  }  

  Status = PeiServicesInstallPpi(gPpiList);
  ASSERT_EFI_ERROR(Status);

  Status = PeiServicesNotifyPpi(gNotifyList);
  ASSERT_EFI_ERROR(Status);

  return Status;  
}  


