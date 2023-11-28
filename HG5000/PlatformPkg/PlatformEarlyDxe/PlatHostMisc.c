
#include "PlatHost.h"
#include "../PlatformPkg/Include/SetupVariable.h"
#include <Library/UefiRuntimeServicesTableLib.h>

UINTN
GetPhysicsPortIndex (
  EFI_HANDLE          Handle,
  UINT16              DevIndex
  )
{
  UINTN                              Index;
  EFI_STATUS                         Status = EFI_SUCCESS;
  PLATFORM_HOST_INFO                 *HostInfo;
  PLATFORM_HOST_INFO_SATA_CTX        *Ctx;
  EFI_DEVICE_PATH_PROTOCOL           *SataHostDp;
  UINTN                              PortIndex;
  UINTN                 Size;
  SETUP_DATA            SetupData;


  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**)&SataHostDp
                  );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "%a (L:%d) %r\n", __FUNCTION__, __LINE__, Status));
    goto ProcExit;
  }

  Size = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  ); 
  if(EFI_ERROR(Status)){
    SetupData.ScreenPrintNum = 0;
  }

  if (SetupData.ScreenPrintNum > 1) {
    SetupData.ScreenPrintNum = 0; // only support 0 and 1
  }

  DEBUG((EFI_D_ERROR, "SetupData.ScreenPrintNum = 0x%x\n", SetupData.ScreenPrintNum));
  DEBUG((EFI_D_ERROR, "DevIndex = 0x%x\n", DevIndex));
  
  HostInfo = gPlatHostInfoProtocol.HostList;
  for(Index=0;Index<gPlatHostInfoProtocol.HostCount;Index++){
    if(HostInfo[Index].HostType != PLATFORM_HOST_SATA){
      continue;
    }
    Ctx = (PLATFORM_HOST_INFO_SATA_CTX*)(HostInfo[Index].HostCtx);
    if(CompareMem(HostInfo[Index].Dp, SataHostDp, Ctx->DpSize-4) == 0){
      DEBUG((EFI_D_ERROR, "Ctx->PortCount = 0x%x\n", Ctx->PortCount));
      for (PortIndex = 0; PortIndex < Ctx->PortCount; PortIndex++) {
        if (Ctx->PortLogicToPhysic[SetupData.ScreenPrintNum][PortIndex].LogicPort == DevIndex) {
          DEBUG((EFI_D_ERROR, "return PhysicsPort = 0x%x\n", Ctx->PortLogicToPhysic[SetupData.ScreenPrintNum][PortIndex].PhysicsPort));
          return Ctx->PortLogicToPhysic[SetupData.ScreenPrintNum][PortIndex].PhysicsPort;
        }
      }
    }
  }
ProcExit:
  return DevIndex;
}

UINT16
GetPlatSataPortIndex (
  IN  EFI_HANDLE          Handle,
  UINTN                  *PhysicalPort
  )
{
  EFI_STATUS                    Status;
  UINT16                        DevIndex = 0xFFFF;
  EFI_DISK_INFO_PROTOCOL        *DiskInfo;
  UINT32                        IdeChannel;
  UINT32                        IdeDevice;
  UINT8                         SataMode;


  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDiskInfoProtocolGuid,
                  (VOID**)&DiskInfo
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  if(CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid)){
    SataMode = 1;
  } else if(CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid)){
    SataMode = 6;
  } else {
    goto ProcExit;
  }

  Status = DiskInfo->WhichIde (
                       DiskInfo,
                       &IdeChannel,
                       &IdeDevice
                       );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  if(SataMode == 1){
    DevIndex = (UINT16)(IdeChannel * 2 + IdeDevice);
  } else if(SataMode == 6){                                      // must AHCI
    DevIndex = (UINT16)IdeChannel;
  }

  if (PhysicalPort != NULL) {
    *PhysicalPort = GetPhysicsPortIndex(Handle, DevIndex);
  }

ProcExit:
  return DevIndex;
}



UINT16 
GetPlatSataHostIndex(
  EFI_HANDLE          Handle
  )
{
  UINTN                              Index;
  EFI_STATUS                         Status = EFI_SUCCESS;
  PLATFORM_HOST_INFO                 *HostInfo;
  PLATFORM_HOST_INFO_SATA_CTX        *Ctx;
  EFI_DEVICE_PATH_PROTOCOL           *SataHostDp;


  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID**)&SataHostDp
                  );
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_ERROR, "%a (L:%d) %r\n", __FUNCTION__, __LINE__, Status));
    goto ProcExit;
  }

  HostInfo = gPlatHostInfoProtocol.HostList;
  for(Index=0;Index<gPlatHostInfoProtocol.HostCount;Index++){
    if(HostInfo[Index].HostType != PLATFORM_HOST_SATA){
      continue;
    }
    Ctx = (PLATFORM_HOST_INFO_SATA_CTX*)(HostInfo[Index].HostCtx);
    if(CompareMem(HostInfo[Index].Dp, SataHostDp, Ctx->DpSize-4) == 0){
      if(gPlatHostInfoProtocol.SataHostCount == 1){
        return 0x8000;
      } else {
        return Ctx->HostIndex;
      }
    }
  }

ProcExit:
  return 0xFFFF;
}



VOID
PlatUpdateBootOption (
  EFI_BOOT_MANAGER_LOAD_OPTION  **BootOptions,
  UINTN                         *BootOptionCount
  )
{
  DEBUG((EFI_D_INFO, "%a(%d)\n", __FUNCTION__, *BootOptionCount));
}




EFI_STATUS
GetPlatNvmeIndex (
  EFI_DEVICE_PATH_PROTOCOL *Dp, 
  UINTN                    *NvmeIndex
  )
{
  PLATFORM_HOST_INFO           *HostInfo;
  PLATFORM_HOST_INFO_NVME_CTX  *Ctx;
  UINTN                        Index;


  HostInfo = gPlatHostInfoProtocol.HostList;
  for(Index=0;Index<gPlatHostInfoProtocol.HostCount;Index++){
    if(HostInfo[Index].HostType != PLATFORM_HOST_NVME){
      continue;
    }

    Ctx = (PLATFORM_HOST_INFO_NVME_CTX*)(HostInfo[Index].HostCtx);
    ASSERT(Ctx->DpSize != 0);

    if(CompareMem(HostInfo[Index].Dp, Dp, Ctx->DpSize - 4) == 0){
      *NvmeIndex = Ctx->NvmeIndex;
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}







