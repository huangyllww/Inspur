

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ByoUefiBootManagerLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>

#include <Protocol/DevicePath.h>
#include <Protocol/LoadFile.h>

#include <UefiBootManagerLoadOption.h>
#include <Protocol/BdsBootManagerProtocol.h>

extern EFI_BOOT_MANAGER_LEGACY_BOOT mEfiBootManagerLegacyBoot;

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
  UINTN                         HandleCount;
  EFI_HANDLE                    *Handles;
  UINTN                         Index;
  UINTN                         BootIndex;
  EFI_DEVICE_PATH_PROTOCOL      *DevicePath;
  UINTN                         BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  BBS_BBS_DEVICE_PATH           *Bbs;
  UINT8    PxeRetry;


  gBS->SetWatchdogTimer (0x0000, 0x0000, 0x0000, NULL);
  SignalProtocolEvent(gBS, &gEfiBootMenuEnterGuid, TRUE);
  
  BootOptions = EfiBootManagerGetLoadOptions(&BootOptionCount, LoadOptionTypeBoot);
  DEBUG_CODE (
    for (Index = 0; Index < BootOptionCount; Index++) {
      if ((BootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
        continue;
      }
      DEBUG ((EFI_D_ERROR,"BootOptions:%d ",Index));
      ShowDevicePathDxe (gBS, BootOptions[Index].FilePath);
    }
  );
  //
  // Parse load file, assuming UEFI Network boot option
  //
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiLoadFileProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );
  if (HandleCount) {
    gST->ConOut->ClearScreen(gST->ConOut);
    gST->ConOut->EnableCursor(gST->ConOut, FALSE);
  }

  PxeRetry = PcdGet8 (PcdRetryPxeBoot);
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), PxeRetry :%d, HandleCount :%d.\n", PxeRetry, HandleCount));
  do {
    for (BootIndex = 0; BootIndex < BootOptionCount; BootIndex ++) {
      if ((BootOptions[BootIndex].Attributes & LOAD_OPTION_ACTIVE) == 0) {
        continue;
      }
      DEBUG ((EFI_D_ERROR,"BootIndex:%d,\n",BootIndex));
      for (Index = 0; Index < HandleCount; Index ++) {
        Status = gBS->HandleProtocol(
                        Handles[Index],
                        &gEfiDevicePathProtocolGuid,
                        &DevicePath
                        );
        ShowDevicePathDxe (gBS, DevicePath);
        if (CompareDevicePath (DevicePath, BootOptions[BootIndex].FilePath)) {
          DEBUG ((EFI_D_ERROR,"boot from native LAN BootOptions:%d,\n",BootIndex));
          ByoEfiBootManagerBoot (&BootOptions[BootIndex]);
        }
      }
      // Check to see if we should legacy BOOT. If yes then do the legacy boot
      // Write boot to OS performance data for Legacy boot
      //
      Bbs = (BBS_BBS_DEVICE_PATH *)(BootOptions[BootIndex].FilePath);
      DEBUG ((EFI_D_ERROR,"Bbs->DeviceType:%x\n",Bbs->DeviceType));
      if ((Bbs->Header.Type == BBS_DEVICE_PATH) && 
          (Bbs->Header.SubType == BBS_BBS_DP) && 
          (Bbs->DeviceType == BBS_TYPE_EMBEDDED_NETWORK)) {
        if (mEfiBootManagerLegacyBoot != NULL) {
          DEBUG ((EFI_D_ERROR,"boot from legacy LAN device!\n"));
          mEfiBootManagerLegacyBoot (&BootOptions[BootIndex]);
        } else {
          BootOptions[BootIndex].Status = EFI_UNSUPPORTED;
        }
      }
  
    }

    if (PcdGet8 (PcdRetryPxeBoot) > 1) {
      PxeRetry --;
    } 
  } while (PxeRetry && HandleCount);
  
  if (HandleCount != 0) {
    FreePool (Handles);
  }
  ByoEfiBootManagerFreeLoadOptions(BootOptions, BootOptionCount);

  return EFI_SUCCESS;
}


