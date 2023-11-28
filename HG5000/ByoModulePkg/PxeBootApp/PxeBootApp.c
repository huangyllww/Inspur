

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ByoUefiBootManagerLib.h>
#include <Library/ByoCommLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadFile.h>
#include <Protocol/BdsBootManagerProtocol.h>


/**
  Compare two device pathes to check if they are exactly same.

  @param DevicePath1    A pointer to the first device path data structure.
  @param DevicePath2    A pointer to the second device path data structure.

  @retval TRUE    They are same.
  @retval FALSE   They are not same.

**/
BOOLEAN
CompareDevicePath (
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath1,
    IN EFI_DEVICE_PATH_PROTOCOL *DevicePath2
)
{
    UINTN Size1;
    UINTN Size2;

    Size1 = GetDevicePathSize (DevicePath1);
    Size2 = GetDevicePathSize (DevicePath2);

    if (Size1 != Size2) {
      return FALSE;
    }
    if (CompareMem (DevicePath1, DevicePath2, Size1) != 0) {
      return FALSE;
    }
    return TRUE;
}


EFI_STATUS
EFIAPI
PxeBootAppEntry (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS                    Status;
  UINTN                         HandleCount = 0;
  EFI_HANDLE                    *Handles = NULL;
  UINTN                         Index;
  UINTN                         BootIndex;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  UINTN                         BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  BBS_BBS_DEVICE_PATH           *Bbs;
  EFI_BDS_BOOT_MANAGER_PROTOCOL *BdsBootMgr;
  

  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);

  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }  

  BdsBootMgr->GetOptions(&BootOptions, &BootOptionCount); 

  Status = gBS->LocateHandleBuffer (
                 ByProtocol,
                 &gEfiLoadFileProtocolGuid,
                 NULL,
                 &HandleCount,
                 &Handles
                 );
  for (Index = 0; Index < HandleCount; Index ++) {
    
    for (BootIndex = 0; BootIndex < BootOptionCount; BootIndex ++) {
      if (BootOptions[BootIndex].Ignore) {
        continue;
      }
      Status = gBS->HandleProtocol(
                      Handles[Index],
                      &gEfiDevicePathProtocolGuid,
                      &DevicePath
                      );
      if(EFI_ERROR(Status)){
        continue;
      }
      ShowDevicePathDxe(gBS, DevicePath);
      if (CompareDevicePath(DevicePath, BootOptions[BootIndex].FilePath)) {
        DEBUG ((EFI_D_INFO, "boot from native LAN BootOptions:%d,\n", BootIndex));
        BdsBootMgr->Boot(&BootOptions[BootIndex]);
        break;
      }
    }
  }

  for (BootIndex = 0; BootIndex < BootOptionCount; BootIndex++) {
    Bbs = (BBS_BBS_DEVICE_PATH*)(BootOptions[BootIndex].FilePath);
    if ((Bbs->Header.Type == BBS_DEVICE_PATH) && 
        (Bbs->Header.SubType == BBS_BBS_DP) && 
        (Bbs->DeviceType == BBS_TYPE_EMBEDDED_NETWORK)) {
      BdsBootMgr->Boot(&BootOptions[BootIndex]);  
    }
  }

  if (HandleCount != 0) {
    FreePool(Handles);
  }
  BdsBootMgr->FreeOptions(BootOptions, BootOptionCount);

ProcExit:
  return Status;
}


