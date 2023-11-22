
#include "ProjectSmm.h"
#include <Protocol/ByoSmiFlashExtProtocol.h>


BYO_SMIFLASH_EXT_PROTOCOL *gPlatSmiFlashEx = NULL;

BOOLEAN
EFIAPI
SmiFlashEx (
  IN  SMI_INFO                     *SmiFlashInfo,
  IN  NV_MEDIA_ACCESS_PROTOCOL     *MediaAccess,
  OUT EFI_STATUS                   *RetStatus
  )
{
  BOOLEAN        rc;
  
  rc = gPlatSmiFlashEx->ByoSmiFlashExtFunc(SmiFlashInfo, MediaAccess, RetStatus);

  return rc;
}




BYO_SMIFLASH_EXT_PROTOCOL gByoSmiflashExtProtocol = {
  SmiFlashEx
};

EFI_STATUS SmiFlashExtInit()
{
  EFI_HANDLE     SmmHandle = NULL;  
  EFI_STATUS     Status;

  Status = gSmst->SmmLocateProtocol (
                  &gPlatformSmiFlashExProtocolGuid,
                  NULL,
                  (VOID**)&gPlatSmiFlashEx
                  );
  ASSERT(!EFI_ERROR(Status));

  Status = gSmst->SmmInstallProtocolInterface (
                    &SmmHandle,
                    &gByoSmiFlashExtProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gByoSmiflashExtProtocol
                    );
  ASSERT_EFI_ERROR (Status);
  
  return Status;
}


