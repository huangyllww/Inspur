
#include "SmiFlash.h"
#include <Protocol/ByoCommSmiSvcProtocol.h>


EFI_STATUS HandleIfResetPasswordRequest()
{
  EFI_STATUS                 Status;
  BYO_COMM_SMI_SVC_PROTOCOL  *CommSmiSvc;  
  
  Status = gSmst->SmmLocateProtocol (
                    &gByoCommSmiSvcProtocolGuid,
                    NULL,
                    (VOID**)&CommSmiSvc
                    );
  if(!EFI_ERROR(Status)){
    Status = CommSmiSvc->ResetSysPassword();
  }
  return Status;
}