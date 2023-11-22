

#ifndef __PLATFORM_PEI_H__
#define __PLATFORM_PEI_H__
//-----------------------------------------------------------------------------


#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Guid/MemoryTypeInformation.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/Capsule.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoCommLib.h>

EFI_STATUS
EFIAPI
PeimInitializeRecovery (
  IN EFI_PEI_SERVICES     **PeiServices
  );

EFI_STATUS
SmmAccessPpiInstall (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  );

EFI_STATUS
SmmControlPpiInstall (
  IN CONST EFI_PEI_SERVICES  **PeiServices
  );




//------------------------------------------------------------------------------
#endif


