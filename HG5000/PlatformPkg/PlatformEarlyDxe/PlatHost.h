
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
#include <Token.h>


extern PLAT_HOST_INFO_PROTOCOL   gPlatHostInfoProtocol;
extern CONST SETUP_DATA    *gSetupHob;

UINT16
GetPlatSataPortIndex (
IN  EFI_HANDLE          Handle,
UINTN                  *PhysicalPort
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
  
VOID
PlatUpdateBootOption (
  EFI_BOOT_MANAGER_LOAD_OPTION  **BootOptions,
  UINTN                         *BootOptionCount
  );

#endif