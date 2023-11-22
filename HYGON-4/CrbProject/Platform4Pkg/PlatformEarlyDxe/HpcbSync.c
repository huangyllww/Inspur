

#include "PlatHost.h"
#include <Library/PlatformCommLib.h>
#include <Library/UefiLib.h>
#include <Library/HygonCbsVariable.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Guid/HygonCbsConfig.h>
#include <Guid/MemoryTypeInformation.h>


VOID DoCbsHpcbSync()
{
  EFI_STATUS                 Status;
  UINTN                      Size;
  CBS_CONFIG                 *CbsConfig;


  DEBUG((EFI_D_INFO, "DoCbsHpcbSync\n"));

  CbsConfig = (CBS_CONFIG*)GetHygonCbsHobData();
  Status = gRT->SetVariable(
                  CBS_SYSTEM_CONFIGURATION_NAME,
                  &gCbsSystemConfigurationGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof(CBS_CONFIG),
                  CbsConfig
                  ); 
  DEBUG ((EFI_D_INFO, "SetVariable(CBS):%r\n", Status));
  

  if(gBootMode == BOOT_IN_RECOVERY_MODE){
    DEBUG((EFI_D_INFO, "ignore at recovery\n"));
    return;
  }

  Size   = 0;
  Status = gRT->GetVariable (
                  EFI_MEMORY_TYPE_INFORMATION_VARIABLE_NAME,
                  &gEfiMemoryTypeInformationGuid,
                  NULL,
                  &Size,
                  NULL
                  );
  DEBUG((EFI_D_INFO, "GetVariable:%r\n", Status));  
  if (Status == EFI_NOT_FOUND) {
    DEBUG((EFI_D_INFO, "ignore at first boot.\n"));
    return;
  }

  DEBUG((EFI_D_INFO, "ApcbWritten:%d\n", gByoSharedSmmData->ApcbWritten));
  if(gByoSharedSmmData->ApcbWritten){
    gByoSharedSmmData->NeedSysReset = 1;
  }
}




