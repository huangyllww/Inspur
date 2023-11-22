/** @file
  Provides a platform-specific method to enable Secure Boot Custom Mode setup.

  Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/
#include <PiMm.h>
#include <Library/PcdLib.h>
#include <Library/MmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Protocol/GetSecurePPFlagProtocol.h>


#define PP_VALID_FLAG  SIGNATURE_32('_', 'W', 'Z', '_')


UINT32  *gSecurePPFlag = NULL;

// Ensure gSecurePPFlag is initialized
EFI_STATUS InitializeSecurePPFlag() {
  EFI_STATUS Status;
  GETSECUREPPFLAG_PROTOCOL *SecureFlagProtocol;

  if (gSecurePPFlag == NULL) {
    Status = gMmst->MmLocateProtocol(
               &gGetSecurePPFlagProtocolGuid,
               NULL,
               (VOID**)&SecureFlagProtocol);
    ASSERT_EFI_ERROR(Status);

    Status = SecureFlagProtocol->GetSecurePPFlag(&gSecurePPFlag);
    ASSERT_EFI_ERROR(Status);
  }
  return EFI_SUCCESS;
}



EFI_STATUS LibAuthVarSetPhysicalPresent(BOOLEAN Present)
{
  InitializeSecurePPFlag();
  if(Present){
    *gSecurePPFlag = PP_VALID_FLAG;
  } else {
    *gSecurePPFlag = 0;
  }

  return EFI_SUCCESS;  
} 


/**

  This function provides a platform-specific method to detect whether the platform
  is operating by a physically present user. 

  Programmatic changing of platform security policy (such as disable Secure Boot,
  or switch between Standard/Custom Secure Boot mode) MUST NOT be possible during
  Boot Services or after exiting EFI Boot Services. Only a physically present user
  is allowed to perform these operations.

  NOTE THAT: This function cannot depend on any EFI Variable Service since they are
  not available when this function is called in AuthenticateVariable driver.
  
  @retval  TRUE       The platform is operated by a physically present user.
  @retval  FALSE      The platform is NOT operated by a physically present user.

**/
BOOLEAN
EFIAPI
UserPhysicalPresent (
  VOID
  )
{
  InitializeSecurePPFlag(); 
  
  return (BOOLEAN)(*gSecurePPFlag == PP_VALID_FLAG);  
}
