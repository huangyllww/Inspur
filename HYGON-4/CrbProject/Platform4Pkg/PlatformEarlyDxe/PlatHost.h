
#ifndef __PLAT_HOST_H__
#define __PLAT_HOST_H__

#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/DiskInfo.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <IndustryStandard/Smbios.h>
#include <Library/ByoCommLib.h>
#include <IndustryStandard/Pci.h>
#include <SetupVariable.h>
#include <Library/ByoSharedSmmData.h>


extern PLAT_HOST_INFO_PROTOCOL   gPlatHostInfoProtocol;
extern CONST SETUP_DATA    *gSetupHob;
extern EFI_BOOT_MODE       gBootMode;

UINT16
GetPlatSataPortIndex (
  IN  EFI_HANDLE          Handle
  );

UINT16
GetPlatSataHostIndex(
  EFI_HANDLE          Handle
  );

EFI_STATUS
GetPlatNvmeIndex (
  EFI_DEVICE_PATH_PROTOCOL *Dp,
  UINTN                    *NvmeIndex
  );

#endif
