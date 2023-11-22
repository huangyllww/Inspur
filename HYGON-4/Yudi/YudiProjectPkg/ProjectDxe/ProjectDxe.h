#ifndef __PROJECT_DXE_H__
#define __PROJECT_DXE_H__

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ByoCommLib.h>
#include <Library/MemoryAllocationLib.h>
#include <ByoStatusCode.h>
#include <Library/ReportStatusCodeLib.h>
#include <Pi/PiBootMode.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/HiiLib.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <SetupVariable.h>
#include <Library/PlatformCommLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoSharedSmmData.h>

extern POST_HOT_KEY_CTX gPlatPostHotKeyCtx[];
extern UINTN gPlatPostHotKeyCtxCount;
extern EFI_HII_HANDLE gHiiHandle;
extern CONST SETUP_DATA  *gSetupHob;

EFI_STATUS IsaAcpiDevListDxe();

#endif
