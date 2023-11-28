/** @file
  This library is TPM2 DTPM instance.
  It can be registered to Tpm2 Device router, to be active TPM2 engine,
  based on platform setting.

Copyright (c) 2013 - 2016, Intel Corporation. All rights reserved. <BR>
This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/Tpm2DeviceLib.h>
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
  SOC_LOGICAL_ID  LogicalId;
  BOOLEAN         rc;
  

  rc = BaseGetLogicalIdOnExecutingCore(&LogicalId);
  if(!rc){
    return EFI_UNSUPPORTED;
  }

  DEBUG((EFI_D_INFO, "Higon Family %X Rev %X\n", LogicalId.Family, LogicalId.Revision));

  if(LogicalId.Family == HYGON_FAMILY_18_DN && LogicalId.Revision >= HYGON_REV_F18_DN_B0){
    return EFI_SUCCESS;
  }
  
  return EFI_UNSUPPORTED;

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


