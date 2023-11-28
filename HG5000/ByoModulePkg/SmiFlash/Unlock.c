
#include "SmiFlash.h"
#include <Protocol/ByoSmiFlashLockProtocol.h>


EFI_STATUS HandleIfUnlockRequest(ROM_HOLE_PARAMETER *Param)
{
  EFI_STATUS                   Status;
  BYO_SMIFLASH_LOCK_PROTOCOL   *SmiFlashLock;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = gSmst->SmmLocateProtocol (
                    &gByoSmiFlashLockProtocolGuid,
                    NULL,
                    (VOID**)&SmiFlashLock
                    );
  if(!EFI_ERROR(Status)){ 
    Status = SmiFlashLock->Unlock(SmiFlashLock, (VOID*)(UINTN)Param->Buffer, Param->BufferSize);
  }

  return Status;
}


EFI_STATUS HandleIfLockRequest()
{
  EFI_STATUS                   Status;
  BYO_SMIFLASH_LOCK_PROTOCOL   *SmiFlashLock;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = gSmst->SmmLocateProtocol (
                    &gByoSmiFlashLockProtocolGuid,
                    NULL,
                    (VOID**)&SmiFlashLock
                    );
  if(!EFI_ERROR(Status)){ 
    Status = SmiFlashLock->Lock(SmiFlashLock);
  }

  return Status;
}
