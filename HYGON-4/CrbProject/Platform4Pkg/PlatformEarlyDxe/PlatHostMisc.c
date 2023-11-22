
#include "PlatHost.h"


UINT16
GetPlatSataPortIndex (
  IN  EFI_HANDLE          Handle
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
    if(HostInfo[Index].HostType == PLATFORM_HOST_SATA){
      Ctx = (PLATFORM_HOST_INFO_SATA_CTX*)(HostInfo[Index].HostCtx);
    } else if(HostInfo[Index].HostType == PLATFORM_HOST_EX_SATA){
      Ctx = &((PLATFORM_HOST_INFO_EX_SATA_CTX*)(HostInfo[Index].HostCtx))->SataCtx;
    } else {
      continue;
    }

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







