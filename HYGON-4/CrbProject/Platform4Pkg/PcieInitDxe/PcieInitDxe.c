

#include <Library/DebugLib.h>
#include <NbioRegisterTypes.h>
#include <HYGON.h>
#include <GnbHsio.h>
#include <Guid/GnbPcieInfoHob.h>
#include <PiDxe.h>
#include <Library/HygonBaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HygonNbioBaseServicesProtocol.h>
#include <Protocol/HygonNbioPcieServicesProtocol.h>
#include <Guid/GnbPcieInfoHob.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Protocol/PciIo.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/DevicePath.h>
#include <Library/ByoCommLib.h>
#include <Library/MemoryAllocationLib.h>
#include <SysMiscCfg.h>
#include <HygonRas.h>
#include <PlatHostDevicePath.h>
#include <HygonHsioInfo.h>
#include <Protocol/Smbios.h>
#include <Library/PlatformCommLib.h>
#include <GnbRegisters.h>


EFI_DEVICE_PATH_PROTOCOL gEndDp = DP_END;

typedef struct {
  UINT8  Present;
  UINT8  Socket;
  UINT8  StartLane;
  UINT8  EndLane;
  UINT8  Bus;
  UINT8  Dev;
  UINT8  Fun;
  UINT8  InUse;
  EFI_PCI_IO_PROTOCOL      *PciIo;
  EFI_DEVICE_PATH_PROTOCOL *Dp;
  UINT16                   DpSize;
} PCIE_BDF_INFO;

typedef struct {
  PLATFORM_HOST_INFO       *HostInfo; 
  UINTN                    HostInfoCount;
  PLATFORM_COMM_INFO       *PlatCommInfo;
  HSIO_PCIE_CTX            *Current;
  PCIE_BDF_INFO            *BdfInfo;  
} PCIE_UPDATE_HOST_CTX;

PCIE_BDF_INFO *gPcieBdfInfo     = NULL;
PCIE_BDF_INFO *gPcieIgdBdfInfo  = NULL;
UINTN         gPcieBdfInfoCount = 0;
UINTN         gPcieBdfInfoIndex = 0;
UINTN         gPciHandleCount   = 0;
EFI_HANDLE    *gPciHandles      = NULL;
EFI_DEVICE_PATH_PROTOCOL **gPciDps = NULL;
EFI_PCI_IO_PROTOCOL      **gPciIos = NULL;
UINTN                    *gPciDpSizes = NULL;
UINT32                   *gPciIds = NULL;
UINT32                   gGfxSlotBDF = 0xFFFFFFFF;
PLATFORM_COMM_INFO       *gPlatformCommInfo = NULL;

STATIC BOARDCARD_PATH2 gPcieSampleDp   = {DP_ROOT(0), DP_PCI(0, 0), DP_PCI(0, 0), DP_END};
STATIC BOARDCARD_PATH1 gPcieRpSampleDp = {DP_ROOT(0), DP_PCI(0, 0), DP_END};


VOID
STATIC
PlatPcieSlotInitCallback1 (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  gPcieBdfInfoCount++;
}

PCIE_BDF_INFO *GetPcieBdf(HSIO_PCIE_CTX *PcieCtx)
{
  UINTN  Index;

  for(Index=0;Index<gPcieBdfInfoCount;Index++){
    if(PcieCtx->Socket    == gPcieBdfInfo[Index].Socket &&
       PcieCtx->StartLane == gPcieBdfInfo[Index].StartLane &&
       PcieCtx->EndLane   == gPcieBdfInfo[Index].EndLane){
      return &gPcieBdfInfo[Index];
    }
  }

  return NULL;
}



VOID*
LibGetPciIoFromDp (
  IN EFI_BOOT_SERVICES         *BS,
  IN EFI_DEVICE_PATH_PROTOCOL  *Dp
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *TempDp = Dp;
  EFI_HANDLE                Device;
  EFI_PCI_IO_PROTOCOL       *PciIo = NULL;


  Status = BS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TempDp, &Device);
  if(!EFI_ERROR(Status) && IsDevicePathEnd(TempDp)){
    Status = BS->HandleProtocol(Device, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
  }
  return PciIo;
}


VOID
STATIC
PlatPcieSlotInitCallback2 (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GNB_HANDLE            *NbioHandle;
  PCIE_BDF_INFO         *Bdf;
  BOARDCARD_PATH1       *RpDp;
  UINTN                 Index;
  UINT32                Address;

  Bdf = &gPcieBdfInfo[gPcieBdfInfoIndex++];

  NbioHandle = (GNB_HANDLE*)PcieConfigGetParentSilicon(Engine);
  Bdf->Present   = Engine->Type.Port.PortData.PortPresent;
  Bdf->Socket    = NbioHandle->SocketId;
  Bdf->StartLane = (UINT8)Engine->EngineData.StartLane;
  Bdf->EndLane   = (UINT8)Engine->EngineData.EndLane;
  Bdf->Bus       = (UINT8)NbioHandle->Address.Address.Bus;
  Bdf->Dev       = Engine->Type.Port.PortData.DeviceNumber;
  Bdf->Fun       = Engine->Type.Port.PortData.FunctionNumber;
  Bdf->InUse     = !!(Engine->InitStatus & INIT_STATUS_PCIE_TRAINING_SUCCESS);

  RpDp = (BOARDCARD_PATH1*)AllocatePool(sizeof(BOARDCARD_PATH1));
  ASSERT(RpDp != NULL);
  CopyMem(RpDp, &gPcieRpSampleDp, sizeof(gPcieRpSampleDp));
  for(Index=0; Index<gPlatformCommInfo->PciHostCount; Index++){
    if(Bdf->Bus >= gPlatformCommInfo->BusBase[Index] &&
       Bdf->Bus <= gPlatformCommInfo->BusLimit[Index]){
      RpDp->PciRootBridgeDevicePath.UID = (UINT32)Index;
      break;
    }
  }
  RpDp->PciBridge1Path.Device   = Bdf->Dev;
  RpDp->PciBridge1Path.Function = Bdf->Fun; 
  Bdf->Dp = (EFI_DEVICE_PATH_PROTOCOL*)RpDp;
  Bdf->DpSize = sizeof(BOARDCARD_PATH1);

  DEBUG((EFI_D_INFO, "P:%d [%d %03d-%03d] (%X,%X,%X) InUse:%d InitStatus:%x\n", \
    Bdf->Present, Bdf->Socket, Bdf->StartLane, Bdf->EndLane, Bdf->Bus, Bdf->Dev, Bdf->Fun, Bdf->InUse, Engine->InitStatus));
    
  if (Engine->Type.Port.PortData.PortPresent){
    Address = MAKE_SBDFO(0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CAP_PCI_OFFSET);
    NbioRegisterRMW (
       NbioHandle,
       TYPE_PCI,
       Address,
       (UINT32) ~(SLOT_CAP_PHYSICAL_SLOT_NUM_MASK),
       (UINT32) (Engine->Type.Port.PortData.SlotNum << SLOT_CAP_PHYSICAL_SLOT_NUM_OFFSET),
       0
       );
  }
}




VOID
STATIC
MyPlatPcieCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GNB_HANDLE    *NbioHandle;
  UINT32        PcieCapBase;
  UINT32        LinkStatus;
  UINT32        LinkCap;
  UINT32        LinkCap2;
  UINT32        DevCtrl;
  UINT32        LinkCtrl;
  UINTN         CurLinkSpeed;
  UINTN         CurLinkWidth;
  UINTN         MaxLinkSpeed;
  UINTN         MaxLinkWidth;
  CHAR8         *LinkSpeedStr;
  CHAR8         *MaxLinkSpeedStr;
  UINTN         MaxPayLoadSize;
  UINTN         MaxReadReqSize;
  UINT8         Aspm;
#if !defined(MDEPKG_NDEBUG)
  CHAR8         *AspmStr[4] = {"Disabled", "L0S", "L1", "L0S+L1"};
#endif


  NbioHandle = (GNB_HANDLE*)PcieConfigGetParentSilicon(Engine);

  DEBUG((EFI_D_INFO, "P:%d I:%X B:%X D:%X F:%X\n", \
    Engine->Type.Port.PortData.PortPresent, \
    Engine->InitStatus, \
    NbioHandle->Address.Address.Bus, \
    Engine->Type.Port.PortData.DeviceNumber, \
    Engine->Type.Port.PortData.FunctionNumber \
    ));

  if (Engine->Type.Port.PortData.PortPresent){

    if(!(Engine->InitStatus & INIT_STATUS_PCIE_TRAINING_SUCCESS)){
      DEBUG((EFI_D_INFO, "PCIE(%d-%d) Slot Empty\n", Engine->EngineData.StartLane, Engine->EngineData.EndLane));

    } else {
      PcieCapBase = MAKE_SBDFO(0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, 0x58);
      NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x10, &LinkStatus, 0);
      LinkStatus = LinkStatus >> 16;
      NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x0C, &LinkCap, 0);
      NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x2C, &LinkCap2, 0);
      NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x08, &DevCtrl, 0);
      NbioRegisterRead (NbioHandle, TYPE_PCI, PcieCapBase+0x10, &LinkCtrl, 0);

    // Supported Link Speeds Vector
    //   Bit 0  2.5 GT/s
    //   Bit 1  5.0 GT/s
    //   Bit 2  8.0 GT/s
      CurLinkSpeed = LinkStatus & 0xF;
      CurLinkWidth = (LinkStatus >> 4) & 0x3F;
      MaxLinkSpeed = LinkCap & 0xF;
      MaxLinkWidth = (LinkCap >> 4) & 0x3F;
      MaxPayLoadSize = 128 << ((DevCtrl >> 5) & 7);
      MaxReadReqSize = 128 << ((DevCtrl >> 12) & 7);
      Aspm = (UINT8)(LinkCtrl & 0x3);

      CurLinkSpeed = (UINTN)1 << (CurLinkSpeed - 1);
      MaxLinkSpeed = (UINTN)1 << (MaxLinkSpeed - 1);
      LinkSpeedStr    = LibGetPcieLinkSpeedStr((UINT8)CurLinkSpeed);
      MaxLinkSpeedStr = LibGetPcieLinkSpeedStr((UINT8)MaxLinkSpeed);

      DEBUG((EFI_D_INFO, "PCIE(%d-%d) Width:%dX/%dX Speed:%a/%a MaxPayLoad:%d MaxReadReq:%d Aspm:%a\n", \
        Engine->EngineData.StartLane, Engine->EngineData.EndLane, \
        CurLinkWidth, MaxLinkWidth, LinkSpeedStr, MaxLinkSpeedStr, MaxPayLoadSize, MaxReadReqSize, AspmStr[Aspm] \
        ));
    }

  }

  if(Engine->InitStatus & INIT_STATUS_PCIE_TRAINING_SUCCESS){
    DEBUG((EFI_D_OEM, "S[0x%02X]:B[0x%02X]:D[%d]:F[%d] Link up as x%02d %a\n", \
                          0, \
                          NbioHandle->Address.Address.Bus, \
                          Engine->Type.Port.PortData.DeviceNumber, \
                          Engine->Type.Port.PortData.FunctionNumber, \
                          CurLinkWidth, \
                          LinkSpeedStr \
                          ));

  } else {
    DEBUG((EFI_D_OEM, "S[0x%02X]:B[0x%02X]:D[%d]:F[%d] Link Down!\n", \
                          0, \
                          NbioHandle->Address.Address.Bus, \
                          Engine->Type.Port.PortData.DeviceNumber, \
                          Engine->Type.Port.PortData.FunctionNumber \
                          ));
  }

}




STATIC
UINT8
GetPciMultiFunctionNumber (
    EFI_DEVICE_PATH_PROTOCOL  *Dp
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *TempDp;
  PCI_DEVICE_PATH           *PciDp = NULL;
 
  TempDp = Dp;
  while(!IsDevicePathEnd(TempDp)){
    if(DevicePathType(TempDp) == HARDWARE_DEVICE_PATH && DevicePathSubType(TempDp) == HW_PCI_DP){
      PciDp = (PCI_DEVICE_PATH*)TempDp;
    }
    TempDp = NextDevicePathNode(TempDp);
  }

  if(PciDp == NULL){
    return FALSE;
  } else {
    return PciDp->Function;
  }
}


STATIC 
EFI_STATUS 
PcieAddSmbiosType9 (
    PLAT_HOST_INFO_PROTOCOL  *PlatHostInfo
  )
{
  UINTN                              SlotStrLen;
  EFI_STATUS                         Status = EFI_SUCCESS;
  SMBIOS_TABLE_TYPE9                 *SmbiosRecord;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  UINTN                              Index;
  PLATFORM_HOST_INFO_PCIE_CTX        *PcieCtx;
  EFI_DEVICE_PATH_PROTOCOL           *Dp;
  EFI_DEVICE_PATH_PROTOCOL           *ParentDp;
  EFI_HANDLE                         DeviceHandle;
  EFI_PCI_IO_PROTOCOL                *PciIo;
  UINTN                              Seg, Bus, Dev, Fun;
  BOOLEAN                            DevPresent;
  EFI_SMBIOS_PROTOCOL                *Smbios;
 

  DEBUG((EFI_D_INFO, "PcieAddSmbiosType9\n"));

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for(Index=0;Index<PlatHostInfo->HostCount;Index++) {
    if(PlatHostInfo->HostList[Index].HostType == PLATFORM_HOST_PCIE  ||
       PlatHostInfo->HostList[Index].HostType == PLATFORM_HOST_NVME  ||
       PlatHostInfo->HostList[Index].HostType == PLATFORM_HOST_EX_SATA){
      if(PlatHostInfo->HostList[Index].HostCtx == NULL){
        continue;
      }

      if(GetPciMultiFunctionNumber(PlatHostInfo->HostList[Index].Dp) != 0){
        DEBUG((EFI_D_INFO, "Ignore mutli-function.\n"));
        continue;
      }

      PcieCtx = (PLATFORM_HOST_INFO_PCIE_CTX*)PlatHostInfo->HostList[Index].HostCtx;

      SlotStrLen = AsciiStrLen(PcieCtx->SlotName);

      if (SlotStrLen == 0) {
        DEBUG((EFI_D_INFO, "Ignore no slot name pcie slot.\n"));
        continue;
      }

      SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE9) + SlotStrLen + 1 + 1);
      SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_SLOTS;
      SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE9);
      SmbiosRecord->Hdr.Handle = 0;
      SmbiosRecord->SlotDesignation  = 1;
      SmbiosRecord->SlotType         = PcieCtx->SlotType;
      SmbiosRecord->SlotDataBusWidth = PcieCtx->SlotbusWidth;
      SmbiosRecord->CurrentUsage     = PcieCtx->SlotUsage;
      SmbiosRecord->SlotLength       = PcieCtx->SlotLen;
      SmbiosRecord->SlotID           = PcieCtx->SlotId;

      *(UINT8*)&SmbiosRecord->SlotCharacteristics1 = 0x0c;
      *(UINT8*)&SmbiosRecord->SlotCharacteristics2 = 0x01;

      SmbiosRecord->SegmentGroupNum = 0xFF;
      SmbiosRecord->BusNum = 0xFF;
      SmbiosRecord->DevFuncNum = 0xFF;
      ParentDp = GetPciParentDp(gBS, PlatHostInfo->HostList[Index].Dp);
      if(ParentDp == NULL){
        FreePool(SmbiosRecord);
        continue;
      }

      Dp = PlatHostInfo->HostList[Index].Dp;
      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DeviceHandle);
      if(!EFI_ERROR(Status) && IsDevicePathEnd(Dp)){
        DevPresent = TRUE;
      } else {
        DevPresent = FALSE;
      }

      Dp = ParentDp;
      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DeviceHandle);
      if(!EFI_ERROR(Status) && IsDevicePathEnd(Dp)){
        Status = gBS->HandleProtocol(DeviceHandle, &gEfiPciIoProtocolGuid, &PciIo);
        if(!EFI_ERROR(Status)){
          PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
          SmbiosRecord->SegmentGroupNum = (UINT16)Seg;
          SmbiosRecord->BusNum     = (UINT8)Bus;
          SmbiosRecord->DevFuncNum = ((UINT8)Dev << 3) | (UINT8)Fun;
          DEBUG((EFI_D_INFO, "PCI(%X,%X,%X)\n", Bus, Dev, Fun));

          if(DevPresent){
            SmbiosRecord->CurrentUsage = SlotUsageInUse;
          }

        }
      }
      FreePool(ParentDp);

      AsciiStrCpyS((CHAR8*)(SmbiosRecord + 1), SlotStrLen + 1, PcieCtx->SlotName);

      SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
      Status = Smbios->Add (
                         Smbios,
                         NULL,
                         &SmbiosHandle,
                         (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord
                         );

      FreePool(SmbiosRecord);

    }
  }

  return Status;
}





VOID GetAllPciHandles()
{
  EFI_STATUS                            Status;
  UINTN                                 Index;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &gPciHandleCount,
                  &gPciHandles
                  );
  if(EFI_ERROR(Status) || gPciHandleCount == 0){
    goto ProcExit;
  }

  gPciDps = (EFI_DEVICE_PATH_PROTOCOL**)AllocateZeroPool(gPciHandleCount * sizeof(EFI_DEVICE_PATH_PROTOCOL*));
  ASSERT(gPciDps != NULL);

  gPciIos = (EFI_PCI_IO_PROTOCOL**)AllocateZeroPool(gPciHandleCount * sizeof(EFI_PCI_IO_PROTOCOL*));
  ASSERT(gPciIos != NULL);  

  gPciDpSizes = (UINTN*)AllocateZeroPool(gPciHandleCount * sizeof(UINTN));
  ASSERT(gPciDpSizes != NULL);  

  gPciIds = (UINT32*)AllocateZeroPool(gPciHandleCount * sizeof(UINT32));
  ASSERT(gPciIds != NULL);

  for (Index = 0; Index < gPciHandleCount; Index++) {
    Status = gBS->HandleProtocol(gPciHandles[Index], &gEfiDevicePathProtocolGuid, (VOID**)&gPciDps[Index]);
    Status = gBS->HandleProtocol(gPciHandles[Index], &gEfiPciIoProtocolGuid, (VOID**)&gPciIos[Index]);
    gPciDpSizes[Index] = GetDevicePathSize(gPciDps[Index]);
    Status = gPciIos[Index]->Pci.Read(gPciIos[Index], EfiPciIoWidthUint32, 0, 1, &gPciIds[Index]);
  }

ProcExit:
  return;
}


VOID FreeAllPciHandles()
{
  if(gPcieBdfInfo != NULL){
    FreePool(gPcieBdfInfo);
    gPcieBdfInfo = NULL;
  }

  if(gPciHandleCount){
    
    FreePool(gPciDps);
    FreePool(gPciIos);
    FreePool(gPciDpSizes);
    FreePool(gPciHandles);
    FreePool(gPciIds);

    gPciDps = NULL;
    gPciIos = NULL;
    gPciDpSizes = NULL;
    gPciHandles = NULL;
    gPciIds = NULL;
    gPciHandleCount = 0;
  }
}




EFI_DEVICE_PATH_PROTOCOL *GetIgdDp(HSIO_PCIE_CTX *p, UINTN *DpSize)
{
  PCIE_BDF_INFO                         *BdfInfo;  
  UINTN                                 Index;
  EFI_DEVICE_PATH_PROTOCOL              *IgdPciDp = NULL;
  UINTN                                 IgdPciDpSize = 0;
  UINT8                                 ClassCode[3];
  EFI_STATUS                            Status;
  UINTN                                 S, B, D, F;


  DEBUG((EFI_D_INFO, "GetIgdDp\n"));

  BdfInfo = GetPcieBdf(p);
  if(BdfInfo == NULL){
    DEBUG((EFI_D_ERROR, "GetPcieBdf -> NULL\n"));
    return NULL;
  }

  for(Index=0;Index<gPciHandleCount;Index++){
    if(gPciIos[Index] == NULL){
      continue;
    }
    gPciIos[Index]->GetLocation(gPciIos[Index], &S, &B, &D, &F);
    if(BdfInfo->Bus == B && BdfInfo->Dev == D && BdfInfo->Fun == F){
      IgdPciDp = gPciDps[Index];
      IgdPciDpSize = gPciDpSizes[Index];
      break;
    }
  }
  if(IgdPciDp == NULL){
    DEBUG((EFI_D_ERROR, "IgdPciDp -> NULL\n"));    
    return NULL;
  }

  for (Index = 0; Index < gPciHandleCount; Index++) {
    if(IgdPciDpSize < gPciDpSizes[Index] && CompareMem(IgdPciDp, gPciDps[Index], IgdPciDpSize - 4) == 0){
      Status = gPciIos[Index]->Pci.Read (
                                    gPciIos[Index],
                                    EfiPciIoWidthUint8,
                                    0x9,
                                    3,
                                    ClassCode
                                    );
      if (EFI_ERROR (Status)) {
        continue;
      }
      DEBUG((EFI_D_INFO, "CC(%X,%X,%X)\n", ClassCode[2], ClassCode[1], ClassCode[0]));        
      if(ClassCode[2] == 3 && ClassCode[1] == 0 && ClassCode[0] == 0){
        ShowDevicePathDxe(gBS, gPciDps[Index]);
        *DpSize = gPciDpSizes[Index];
        return gPciDps[Index];
      }
    } 
  }

  return NULL;
}



VOID 
AddSataToPlatHostInfo (
  PCIE_UPDATE_HOST_CTX     *UpdateCtx
  )
{
  PLATFORM_HOST_INFO          *InfoIn;
  UINTN                       InfoCount;
  PLATFORM_HOST_INFO          *Info;
  UINTN                       Index;
  UINTN                       HygonAhciCount;
  PLATFORM_HOST_INFO_SATA_CTX *AhciCtx;
  
  
  InfoIn    = UpdateCtx->HostInfo;
  InfoCount = UpdateCtx->HostInfoCount;

  HygonAhciCount = 0;
  for(Index=0;Index<gPciHandleCount;Index++){
    if(gPciIds[Index] == HG_AHCI_PCIID){
      HygonAhciCount++;
    }
  }

  if(HygonAhciCount){
    InfoIn = ReallocatePool (
                sizeof(PLATFORM_HOST_INFO) * InfoCount,
                sizeof(PLATFORM_HOST_INFO) * (InfoCount + HygonAhciCount),
                InfoIn
                );
    for(Index=0;Index<gPciHandleCount;Index++){
      if(gPciIds[Index] == HG_AHCI_PCIID){
        Info = &InfoIn[InfoCount++];
        Info->HostType = PLATFORM_HOST_SATA;
        Info->Dp = DuplicateDevicePath(gPciDps[Index]);
        AhciCtx = (PLATFORM_HOST_INFO_SATA_CTX*)AllocateZeroPool(sizeof(PLATFORM_HOST_INFO_SATA_CTX));
        AhciCtx->DpSize = gPciDpSizes[Index];
        Info->HostCtx = AhciCtx;
      }
    }
  }

  UpdateCtx->HostInfo = InfoIn;
  UpdateCtx->HostInfoCount = InfoCount;
}






STATIC VOID AddOne(PCIE_UPDATE_HOST_CTX *UpdateCtx, EFI_DEVICE_PATH_PROTOCOL *Dp)
{
  PLATFORM_HOST_INFO_PCIE_CTX *PlatHostPcieCtx;
  PLATFORM_HOST_INFO_NVME_CTX *PlatHostNvmeCtx;
  PLATFORM_HOST_INFO          *Info;
  HSIO_PCIE_CTX               *p = UpdateCtx->Current;
  BOARDCARD_PATH2             *PciDp;
  UINTN                       Index;
  PLATFORM_HOST_INFO_SATA_CTX *SataCtx;
  

  UpdateCtx->HostInfo = ReallocatePool (
                          sizeof(PLATFORM_HOST_INFO) * UpdateCtx->HostInfoCount,
                          sizeof(PLATFORM_HOST_INFO) * (UpdateCtx->HostInfoCount + 1),
                          UpdateCtx->HostInfo
                          );
  Info = &UpdateCtx->HostInfo[UpdateCtx->HostInfoCount++];

  if(Dp != NULL){
    Info->Dp = DuplicateDevicePath(Dp);
  } else {
    PciDp = (BOARDCARD_PATH2*)AllocatePool(sizeof(BOARDCARD_PATH2));
    ASSERT(PciDp != NULL);
    CopyMem(PciDp, &gPcieSampleDp, sizeof(BOARDCARD_PATH2));
    for(Index=0;Index<UpdateCtx->PlatCommInfo->PciHostCount;Index++){
      if(UpdateCtx->BdfInfo->Bus >= UpdateCtx->PlatCommInfo->BusBase[Index] &&
         UpdateCtx->BdfInfo->Bus <= UpdateCtx->PlatCommInfo->BusLimit[Index]){
        PciDp->PciRootBridgeDevicePath.UID = (UINT32)Index;
        break;
      }
    }
    PciDp->PciBridge1Path.Device   = UpdateCtx->BdfInfo->Dev;
    PciDp->PciBridge1Path.Function = UpdateCtx->BdfInfo->Fun; 
    Info->Dp = (EFI_DEVICE_PATH_PROTOCOL*)PciDp;
  }
  
  if(p->BuildIn == PCIE_BUILD_IN_SATA){
    Info->HostType = PLATFORM_HOST_SATA;
    SataCtx = AllocateZeroPool(sizeof(PLATFORM_HOST_INFO_SATA_CTX));
    ASSERT(SataCtx != NULL);
    Info->HostCtx = SataCtx;
    SataCtx->DpSize = (UINT16)GetDevicePathSize(Info->Dp);
    
  } else {
    if(p->BuildIn == PCIE_BUILD_IN_NVME){
      Info->HostType = PLATFORM_HOST_NVME;
      PlatHostNvmeCtx = (PLATFORM_HOST_INFO_NVME_CTX*)AllocateZeroPool(sizeof(PLATFORM_HOST_INFO_NVME_CTX));
      ASSERT(PlatHostNvmeCtx != NULL);
      PlatHostPcieCtx = &PlatHostNvmeCtx->PcieCtx;
      PlatHostNvmeCtx->DpSize = (UINT16)GetDevicePathSize(Info->Dp);
      
    } else if(p->BuildIn == PCIE_BUILD_IN_LAN){
      Info->HostType = PLATFORM_HOST_LAN;
      PlatHostPcieCtx = (PLATFORM_HOST_INFO_PCIE_CTX*)AllocateZeroPool(sizeof(PLATFORM_HOST_INFO_PCIE_CTX));
      ASSERT(PlatHostPcieCtx != NULL);
      
    } else {
      Info->HostType = PLATFORM_HOST_PCIE;
      PlatHostPcieCtx = (PLATFORM_HOST_INFO_PCIE_CTX*)AllocateZeroPool(sizeof(PLATFORM_HOST_INFO_PCIE_CTX));
      ASSERT(PlatHostPcieCtx != NULL);
    }

    ASSERT(PlatHostPcieCtx != NULL);
    Info->HostCtx = PlatHostPcieCtx;
    PlatHostPcieCtx->SlotName = p->Name;
    PlatHostPcieCtx->SlotType = SlotTypePciExpress;
    PlatHostPcieCtx->SlotUsage = UpdateCtx->BdfInfo->InUse ? SlotUsageInUse : SlotUsageAvailable;
    PlatHostPcieCtx->SlotLen   = p->SlotLength;
    PlatHostPcieCtx->SlotId    = p->SlotId;
    switch(p->EndLane - p->StartLane + 1){
      case 1:
        PlatHostPcieCtx->SlotbusWidth = SlotDataBusWidth1X;
        PlatHostPcieCtx->SlotType     = SlotTypePciExpressX1;
        break;
      case 2:
        PlatHostPcieCtx->SlotbusWidth = SlotDataBusWidth2X;
        PlatHostPcieCtx->SlotType     = SlotTypePciExpressX2;
        break;    
      case 4:
        PlatHostPcieCtx->SlotbusWidth = SlotDataBusWidth4X;
        PlatHostPcieCtx->SlotType     = SlotTypePciExpressX4;
        break; 
      case 8:
        PlatHostPcieCtx->SlotbusWidth = SlotDataBusWidth8X;
        PlatHostPcieCtx->SlotType     = SlotTypePciExpressX8;
        break; 
      case 12:
        PlatHostPcieCtx->SlotbusWidth = SlotDataBusWidth12X;
        break;
      case 16:
        PlatHostPcieCtx->SlotbusWidth = SlotDataBusWidth16X;
        PlatHostPcieCtx->SlotType     = SlotTypePciExpressX16;
        break;
      case 32:
        PlatHostPcieCtx->SlotbusWidth = SlotDataBusWidth32X;
        break; 
      default:
        PlatHostPcieCtx->SlotbusWidth = SlotDataBusWidthOther;
        break;       
    }          
  }

}



VOID AddDxioDevToPlatHostInfo(PCIE_UPDATE_HOST_CTX *UpdateCtx)
{
  PCIE_BDF_INFO               *BdfInfo;
  UINTN                       Index;
  UINTN                       S, B, D, F;  
  EFI_DEVICE_PATH_PROTOCOL    *Dp = NULL;
  UINTN                       DpSize = 0;
  EFI_STATUS                  Status;
  UINT8                       ClassCode[3];
  HSIO_PCIE_CTX               *p = UpdateCtx->Current;
  UINTN                       DevCount;


  if(p->BuildIn == PCIE_BUILD_IN_IGD){
    return;
  }
  if(p->EngineType == DXIO_DEV_TYPE_ILAN){
    return;
  }

  DEBUG((EFI_D_INFO, "AddDxioDevToPlatHostInfo(%x,%x)\n", UpdateCtx->HostInfo, UpdateCtx->HostInfoCount));
  
  if(p->EngineType == DXIO_DEV_TYPE_PCIE){
    
    BdfInfo = GetPcieBdf(p);
    if(BdfInfo == NULL){
      DEBUG((EFI_D_ERROR, "GetPcieBdf ERROR\n"));
      return;
    }

    UpdateCtx->BdfInfo = BdfInfo;

    for(Index=0;Index<gPciHandleCount;Index++){
      if(gPciIos[Index] == NULL){
        DEBUG((EFI_D_ERROR, "PciIo NULL\n"));
        continue;
      }
      gPciIos[Index]->GetLocation(gPciIos[Index], &S, &B, &D, &F);
      if(BdfInfo->Bus == B && BdfInfo->Dev == D && BdfInfo->Fun == F){
        Dp = gPciDps[Index];
        DpSize = gPciDpSizes[Index];
        BdfInfo->PciIo = gPciIos[Index];
        DEBUG((EFI_D_INFO, "DP(%X,%X) @ %d\n", Dp, DpSize, Index));
        break;
      }
    }

    if(Dp == NULL || DpSize == 0){
      DEBUG((EFI_D_INFO, "Bridge DP not found\n"));
      AddOne(UpdateCtx, NULL);
      return;
    }

    ShowDevicePathDxe(gBS, Dp);

    DevCount = 0;
    for (Index = 0; Index < gPciHandleCount; Index++) {
      if(DpSize < gPciDpSizes[Index] && CompareMem(Dp, gPciDps[Index], DpSize - 4) == 0){
        Status = gPciIos[Index]->Pci.Read (
                                      gPciIos[Index],
                                      EfiPciIoWidthUint8,
                                      0x9,
                                      sizeof(ClassCode),
                                      ClassCode
                                      );
        if (EFI_ERROR (Status)) {
          DEBUG((EFI_D_ERROR, "Read ClassCode Error\n")); 
          continue;
        }
        if(ClassCode[2] == 6 && ClassCode[1] == 4 && ClassCode[0] == 0){
          DEBUG((EFI_D_INFO, "Ignore Bridge @ %d %X\n", Index, gPciIos[Index])); 
          continue;
        }

        DEBUG((EFI_D_INFO, "+1\n")); 
        AddOne(UpdateCtx, gPciDps[Index]);
        DevCount++;
      } 
    }

    if(DevCount == 0){
      AddOne(UpdateCtx, NULL);
    }
    
  }

}


STATIC CHAR8 *PlatHostTypeNameList[] = {"SATA", "LAN", "PCIE", "SDIO", "IGD", "NVME", "ExSATA"};

STATIC CHAR8 *GetHostTypeName(PLATFORM_HOST_TYPE Type)
{
  if(Type >= PLATFORM_HOST_MAX){
    return "MAX";
  }
  if(Type >= ARRAY_SIZE(PlatHostTypeNameList)){
    return "Unknown";
  }
  return PlatHostTypeNameList[Type];
}

STATIC 
VOID 
DumpPlatHostInfo(
  PLATFORM_HOST_INFO        *HostList,
  UINTN                     HostCount
  )
{
  UINTN                        Index;
  PLATFORM_HOST_TYPE           HostType;
  PLATFORM_HOST_INFO_SATA_CTX  *SataCtx;
  PLATFORM_HOST_INFO_NVME_CTX  *NvmeCtx;


  DEBUG((EFI_D_INFO, "DumpPlatHostInfo\n"));

  for(Index=0;Index<HostCount;Index++){
    HostType = HostList[Index].HostType;
    DEBUG((EFI_D_INFO, "[%d] T:%d(%a) ", Index, HostType, GetHostTypeName(HostType)));
    ShowDevicePathDxe(gBS, HostList[Index].Dp);
    if(HostType == PLATFORM_HOST_SATA){
      SataCtx = (PLATFORM_HOST_INFO_SATA_CTX*)HostList[Index].HostCtx;
      DEBUG((EFI_D_INFO, "  SATA L%d I:%d\n", SataCtx->DpSize, SataCtx->HostIndex)); 
    } else if(HostType == PLATFORM_HOST_EX_SATA){
      SataCtx = &((PLATFORM_HOST_INFO_EX_SATA_CTX*)HostList[Index].HostCtx)->SataCtx;
      DEBUG((EFI_D_INFO, "  SATA L%d I:%d\n", SataCtx->DpSize, SataCtx->HostIndex));    
    } else if(HostType == PLATFORM_HOST_NVME){
      NvmeCtx = (PLATFORM_HOST_INFO_NVME_CTX*)HostList[Index].HostCtx;
      DEBUG((EFI_D_INFO, "  NVME L%d I:%d\n", NvmeCtx->DpSize, NvmeCtx->NvmeIndex));       
    }
  }
}




VOID
CollectPcieSlotInfo (
    HSIO_PCIE_CTX            *Ctx,
    UINTN                    CtxCount
  )
{
  UINTN                         Index;
  HSIO_PCIE_CTX                 *p;
  PCIE_BDF_INFO                 *BdfInfo;
  EFI_STATUS                    Status;
  UINTN                         SlotDevCount = 0;
  SETUP_SLOT_INFO_DATA          *SetupSlot = NULL;
  SETUP_SLOT_INFO               *SetupSlotDev = NULL;
  UINTN                         i = 0, j;
  EFI_PCI_IO_PROTOCOL           *PciIo;
//UINT8                         SupportLinkSpeedVector;


  if(Ctx == NULL){
    return;
  }

  for(Index=0; Index<CtxCount; Index++){
    p = &Ctx[Index];
    if(!p->Present){
      break;
    }     
    if(p->EngineType == DXIO_DEV_TYPE_PCIE && p->BuildIn != PCIE_BUILD_IN_IGD){
      BdfInfo = GetPcieBdf(p);
      if(BdfInfo == NULL || !BdfInfo->InUse){
        continue;
      }
      SlotDevCount++;
    }
  }

  DEBUG((EFI_D_INFO, "SlotDevCount:%d\n", SlotDevCount));
  if(SlotDevCount == 0){
    return;
  }

  SetupSlot = AllocateZeroPool(sizeof(SETUP_SLOT_INFO_DATA) + (SlotDevCount-1)*sizeof(SETUP_SLOT_INFO));
  ASSERT(SetupSlot != NULL);
  SetupSlot->DevCount = SlotDevCount;

  for(Index=0; Index<CtxCount; Index++){
    p = &Ctx[Index];    
    if(!p->Present){
      break;
    }     
    if(p->EngineType == DXIO_DEV_TYPE_PCIE && p->BuildIn != PCIE_BUILD_IN_IGD){
      BdfInfo = GetPcieBdf(p);
      if(BdfInfo == NULL || !BdfInfo->InUse || BdfInfo->PciIo == NULL){
        continue;
      }

      SetupSlotDev = &SetupSlot->Info[i++];
      SetupSlotDev->SlotName = Ctx[Index].Name;

      PciIo  = BdfInfo->PciIo;
      Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0x58+0x10, 1, &SetupSlotDev->LinkStatus);
      SetupSlotDev->LinkStatus = SetupSlotDev->LinkStatus >> 16;
      Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0x58+0x0C, 1, &SetupSlotDev->LinkCap);      
      Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0x58+0x2C, 1, &SetupSlotDev->LinkCap2);  
      Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0x58+0x08, 1, &SetupSlotDev->DevCtrl);  
      Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0x58+0x10, 1, &SetupSlotDev->LinkCtrl);  

      SetupSlotDev->CurLinkSpeed = (UINT8)(SetupSlotDev->LinkStatus & 0xF);
      SetupSlotDev->CurLinkWidth = (UINT8)((SetupSlotDev->LinkStatus >> 4) & 0x3F);
//    SupportLinkSpeedVector     = (UINT8)((SetupSlotDev->LinkCap2 >> 1) & 0x7F);  
      SetupSlotDev->MaxLinkSpeed = (UINT8)(SetupSlotDev->LinkCap & 0xF);
      SetupSlotDev->MaxLinkWidth = (UINT8)((SetupSlotDev->LinkCap >> 4) & 0x3F);

      DEBUG((EFI_D_INFO, "[%d] %d %a PciIo:%X\n", i, Index, SetupSlotDev->SlotName, PciIo));
      DEBUG((EFI_D_INFO, "LinkStatus:%X LinkCap:%X LinkCap2:%X DevCtrl:%X LinkCtrl:%X\n", \
        SetupSlotDev->LinkStatus, SetupSlotDev->LinkCap, \
        SetupSlotDev->LinkCap2, SetupSlotDev->DevCtrl, SetupSlotDev->LinkCtrl));
      DEBUG((EFI_D_INFO, "CurLinkSpeed:%d CurLinkWidth:%d MaxLinkSpeed:%d MaxLinkWidth:%d\n", \
        SetupSlotDev->CurLinkSpeed, SetupSlotDev->CurLinkWidth, SetupSlotDev->MaxLinkSpeed, \
        SetupSlotDev->MaxLinkWidth));      

      if(SetupSlotDev->CurLinkSpeed >= 1 && SetupSlotDev->CurLinkSpeed <= 7){
        SetupSlotDev->CurLinkSpeed = (UINT8)(1 << (SetupSlotDev->CurLinkSpeed - 1));
      }
      if(SetupSlotDev->MaxLinkSpeed >= 1 && SetupSlotDev->MaxLinkSpeed <= 7){
        SetupSlotDev->MaxLinkSpeed = (UINT8)(1 << (SetupSlotDev->MaxLinkSpeed - 1));
      }

      DEBUG((EFI_D_INFO, "DP(%x,%x) gPciHandleCount:%d\n", BdfInfo->Dp, BdfInfo->DpSize, gPciHandleCount));
      ShowDevicePathDxe(gBS, BdfInfo->Dp);

      SetupSlotDev->FuncCount = 0;
      SetupSlotDev->DevPciId  = 0x0000FFFF;
      for (j = 0; j < gPciHandleCount; j++) {
        if(BdfInfo->DpSize < gPciDpSizes[j] && CompareMem(BdfInfo->Dp, gPciDps[j], BdfInfo->DpSize - 4) == 0){
          DEBUG((EFI_D_INFO, "DP match\n"));          
          if(SetupSlotDev->DevPciId == 0x0000FFFF){
            Status = gPciIos[j]->Pci.Read (
                                          gPciIos[j],
                                          EfiPciIoWidthUint32,
                                          0,
                                          1,
                                          &SetupSlotDev->DevPciId
                                          );
            Status = gPciIos[j]->Pci.Read (
                                          gPciIos[j],
                                          EfiPciIoWidthUint8,
                                          9,
                                          3,
                                          SetupSlotDev->DevClassCode
                                          ); 
            SetupSlotDev->FuncCount++;
          }
        } 
      }

    }
  }

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gImageHandle,
                  &gHygonSetupSlotDevInfoProtoclGuid, SetupSlot,
                  NULL
                  );
  
}



VOID
PcieUpdateHostList (
    PLAT_HOST_INFO_PROTOCOL  *PlatHostInfo
  )
{
  EFI_HOB_GUID_TYPE        *GuidHob;
  HSIO_PCIE_CTX_INFO       *CtxInfo;
  HSIO_PCIE_CTX            *Ctx;
  HSIO_PCIE_CTX            *p;
  UINTN                    DxioPcieCtxCount;
  UINTN                    Index;
  EFI_DEVICE_PATH_PROTOCOL *IgdPciDp;
  UINTN                    IgdPciDpSize;  
  PCIE_UPDATE_HOST_CTX     UpdateCtx;
  PLATFORM_HOST_INFO_SATA_CTX  *SataCtx;
  UINTN                        NvmeIndex;
  PLATFORM_HOST_INFO_NVME_CTX  *NvmeCtx;
  EFI_DEVICE_PATH_PROTOCOL     *Dp;
  EFI_HANDLE                   DeviceHandle;
  EFI_STATUS                   Status;
  EFI_PCI_IO_PROTOCOL          *PciIo;
  UINT8                        ClassCode[3];
  PLATFORM_HOST_INFO_EX_SATA_CTX  *ExSataCtx;
  

  DEBUG((EFI_D_INFO, "PcieUpdateHostList\n"));

  GuidHob = GetFirstGuidHob(&gHygonHsioInfoHobGuid);
  if (GuidHob == NULL) {
    return;
  }

  ZeroMem(&UpdateCtx, sizeof(PCIE_UPDATE_HOST_CTX));

  UpdateCtx.PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  
  CtxInfo = (HSIO_PCIE_CTX_INFO*)GET_GUID_HOB_DATA(GuidHob);
  Ctx     = CtxInfo->Ctx;
  DxioPcieCtxCount = CtxInfo->CtxCount;

  AddSataToPlatHostInfo(&UpdateCtx);

  for(Index=0;Index<DxioPcieCtxCount;Index++){
    p = &Ctx[Index];
    if(!p->Present){
      break;
    }
    DEBUG((EFI_D_INFO, "[%d] S%d [%d-%d] T:%d I:%d L:%d Id:%d N:%a\n", Index, \
      p->Socket, p->StartLane, p->EndLane, p->EngineType, p->BuildIn, p->SlotLength, p->SlotId, p->Name));

    UpdateCtx.Current = p;
    AddDxioDevToPlatHostInfo(&UpdateCtx);

    if(p->EngineType == DXIO_DEV_TYPE_PCIE){
      if(p->BuildIn == PCIE_BUILD_IN_IGD){
        IgdPciDp = GetIgdDp(p, &IgdPciDpSize);
        if(IgdPciDp != NULL){
          PlatHostInfo->IgdDp = IgdPciDp;
          PlatHostInfo->IgdDpSize = IgdPciDpSize;
          DEBUG((EFI_D_INFO, "IGD_DP:"));
          ShowDevicePathDxe(gBS, PlatHostInfo->IgdDp);
        }
      }
    }
  }

  PlatHostInfo->HostList  = UpdateCtx.HostInfo;
  PlatHostInfo->HostCount = UpdateCtx.HostInfoCount;
  PlatHostInfo->SataHostCount = 0;
  NvmeIndex = 0;
  for(Index=0;Index<PlatHostInfo->HostCount;Index++){
    if(PlatHostInfo->HostList[Index].HostType == PLATFORM_HOST_SATA){
      SataCtx = (PLATFORM_HOST_INFO_SATA_CTX*)PlatHostInfo->HostList[Index].HostCtx;
      SataCtx->HostIndex = (UINT16)PlatHostInfo->SataHostCount;
      PlatHostInfo->SataHostCount++;
    } else if(PlatHostInfo->HostList[Index].HostType == PLATFORM_HOST_NVME){
      NvmeCtx = (PLATFORM_HOST_INFO_NVME_CTX*)PlatHostInfo->HostList[Index].HostCtx;
      NvmeCtx->NvmeIndex = (UINT16)NvmeIndex++;
    } else if(PlatHostInfo->HostList[Index].HostType == PLATFORM_HOST_PCIE){
      Dp = PlatHostInfo->HostList[Index].Dp;
      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DeviceHandle);
      if(!EFI_ERROR(Status)){
        Status = gBS->HandleProtocol(DeviceHandle, &gEfiPciIoProtocolGuid, &PciIo);
        PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 9, sizeof(ClassCode), ClassCode);
        if(ClassCode[2] == 1 && (ClassCode[1] == 6 || ClassCode[1] == 1)){
          ExSataCtx = AllocateZeroPool(sizeof(PLATFORM_HOST_INFO_EX_SATA_CTX));
          ASSERT(ExSataCtx != NULL);
          CopyMem(&ExSataCtx->PcieCtx, PlatHostInfo->HostList[Index].HostCtx, sizeof(PLATFORM_HOST_INFO_PCIE_CTX));
          ExSataCtx->SataCtx.HostIndex = (UINT16)PlatHostInfo->SataHostCount++;
          ExSataCtx->SataCtx.DpSize = (UINT16)GetDevicePathSize(PlatHostInfo->HostList[Index].Dp);
          
          PlatHostInfo->HostList[Index].HostCtx = ExSataCtx;
          PlatHostInfo->HostList[Index].HostType = PLATFORM_HOST_EX_SATA;
        }
      }
    }
  }

  DumpPlatHostInfo(PlatHostInfo->HostList, PlatHostInfo->HostCount);
  CollectPcieSlotInfo(Ctx, DxioPcieCtxCount);

  if(UpdateCtx.PlatCommInfo->AddOnGfxPciId){
    PlatHostInfo->AddOnGfxId = UpdateCtx.PlatCommInfo->AddOnGfxPciId;
  }
}



VOID PcieAfterConnectPciRootBridge()
{
  PCIe_PLATFORM_CONFIG           *Pcie;
  PLAT_HOST_INFO_PROTOCOL        *PlatHostInfo;
  EFI_STATUS                     Status;  
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB          *PciePlatformConfigHobData;


  DEBUG((EFI_D_INFO, "PcieAfterConnectPciRootBridge\n"));

  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );
  ASSERT(!EFI_ERROR(Status));

  PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32**)&PciePlatformConfigHobData);
  Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID**)&PlatHostInfo);
  ASSERT(!EFI_ERROR(Status));

  GetAllPciHandles();
  PcieUpdateHostList(PlatHostInfo);
  PcieAddSmbiosType9(PlatHostInfo);

  DEBUG((EFI_D_OEM, "DumpPcieLinkStatus\n"));
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    MyPlatPcieCallback,
    NULL,
    Pcie
    );
  DEBUG((EFI_D_OEM, "\n\n"));

  FreeAllPciHandles();
}





EFI_STATUS
PcieInitDxeEntry (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  PCIe_PLATFORM_CONFIG                   *Pcie;
  EFI_STATUS                             Status;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *PcieServicesProtocol;
  GNB_PCIE_INFORMATION_DATA_HOB          *PciePlatformConfigHobData;


  DEBUG((EFI_D_INFO, "PcieInitDxeEntry\n"));

  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );
  ASSERT(!EFI_ERROR(Status));

  PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32**)&PciePlatformConfigHobData);
  Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);

  gPlatformCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PlatPcieSlotInitCallback1,
    NULL,
    Pcie
    );

  gPcieBdfInfo = AllocateZeroPool(sizeof(PCIE_BDF_INFO) * gPcieBdfInfoCount);
  ASSERT(gPcieBdfInfo != NULL);
  DEBUG((EFI_D_INFO, "gPcieBdfInfo:%X %d\n", gPcieBdfInfo, gPcieBdfInfoCount));  
  
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PlatPcieSlotInitCallback2,
    NULL,
    Pcie
    );

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gEfiAfterConnectPciRootBridgeGuid, PcieAfterConnectPciRootBridge,
                  NULL
                  );

  return EFI_SUCCESS;
}

