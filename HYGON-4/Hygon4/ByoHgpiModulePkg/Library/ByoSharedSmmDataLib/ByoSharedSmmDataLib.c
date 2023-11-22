/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  ByoSharedSmmDataLib.c

Abstract: 

Revision History:

**/

#include <Base.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/ByoSharedSmmData.h>


BYO_SHARED_SMM_DATA   *gByoSharedSmmData;

EFI_STATUS
EFIAPI
ByoSharedSmmDataConstructor (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS    Status;
  
  gByoSharedSmmData = (BYO_SHARED_SMM_DATA*)(UINTN)PcdGet64(PcdByoSharedSmmDataAddr);
  if(gByoSharedSmmData != NULL){
    ASSERT(gByoSharedSmmData->Signature == BYO_SHARED_SMM_DATA_SIGNATURE);
  } else {
    Status = gBS->AllocatePool(
                    EfiReservedMemoryType,
                    sizeof(BYO_SHARED_SMM_DATA),
                    (VOID**)&gByoSharedSmmData
                    );
    ASSERT(!EFI_ERROR(Status));
    ZeroMem(gByoSharedSmmData, sizeof(BYO_SHARED_SMM_DATA));
    gByoSharedSmmData->Signature = BYO_SHARED_SMM_DATA_SIGNATURE;
    PcdSet64S(PcdByoSharedSmmDataAddr, (UINTN)gByoSharedSmmData);
  }
  return EFI_SUCCESS;
}
