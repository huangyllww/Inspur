/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By:
  winddy_zhang

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/PcdLib.h>
#include <Guid/TpmInstance.h>
#include <Library/BaseCoreLogicalIdLib.h>


EFI_STATUS
fTpmExecuteCommand (
    IN       VOID                 *CommandBuffer,
    IN       UINT32               CommandSize,
    IN OUT   VOID                 *ResponseBuffer,
    IN OUT   UINT32               *ResponseSize
  );

EFI_STATUS
PspMboxBiosQueryCaps (
  IN OUT   UINT32 *Capabilities
  );


EFI_STATUS
EFIAPI
HgfTpm2SubmitCommand (
  IN UINT32            InputParameterBlockSize,
  IN UINT8             *InputParameterBlock,
  IN OUT UINT32        *OutputParameterBlockSize,
  IN UINT8             *OutputParameterBlock
  )
{
  return fTpmExecuteCommand(
           InputParameterBlock,
           InputParameterBlockSize,
           OutputParameterBlock,
           OutputParameterBlockSize
           );
}

EFI_STATUS
EFIAPI
HgfTpm2RequestUseTpm (
  VOID
  )
{
  if(PcdGetBool(PcdFtpmSupport)){
    return EFI_SUCCESS;
  } else {
    return EFI_UNSUPPORTED;
  }
}


TPM2_DEVICE_INTERFACE  mHgfTpm2InternalTpm2Device = {
  TPM_DEVICE_INTERFACE_TPM20_HGFTPM,
  HgfTpm2SubmitCommand,
  HgfTpm2RequestUseTpm,
};

/**
  The function register DTPM2.0 instance.
  
  @retval EFI_SUCCESS   DTPM2.0 instance is registered, or system dose not surpport registr DTPM2.0 instance
**/
EFI_STATUS
EFIAPI
Tpm2InstanceLibHgfTpmConstructor (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = Tpm2RegisterTpm2DeviceLib (&mHgfTpm2InternalTpm2Device);
  if ((Status == EFI_SUCCESS) || (Status == EFI_UNSUPPORTED)) {
    if(Status == EFI_SUCCESS){
      DEBUG((EFI_D_INFO, "mHgfTpm2InternalTpm2Device OK\n"));
    }
    return EFI_SUCCESS;
  }
  return Status;
}


