/** @file

Copyright (c) 2006 - 2011, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PeiBiosIdLib.c

Abstract:
  Boot service DXE BIOS ID library implementation.

Revision History:

**/

#include <PiPei.h>
#include <Pi/PiFirmwareVolume.h>
#include <Library/DebugLib.h>
#include <Library/BiosIdLib.h>
#include <Library/BaseMemoryLib.h>


STATIC CONST UINT8 gBiosidSign[] = BIOS_ID_SIGN_DATA;

EFI_STATUS
GetBiosId (
  OUT BIOS_ID_IMAGE  *BiosIdImage
  )
/*++
Description:

  This function returns BIOS ID by searching HOB or FV.

Arguments:

  BiosIdImage - The BIOS ID got from HOB or FV
  
Returns:

  EFI_SUCCESS - All parameters were valid and BIOS ID has been got.

  EFI_NOT_FOUND - BiosId image is not found, and no parameter will be modified.

  EFI_INVALID_PARAMETER - The parameter is NULL
     
--*/
{
  EFI_STATUS   Status = EFI_NOT_FOUND;
  UINT8        *Data8;
  UINTN        Index;
  UINTN        Count;

  Data8 = (UINT8*)(UINTN)PcdGet32(PcdFlashFvSecBase);
  Count = PcdGet32(PcdFlashFvSecSize);
  
  for(Index=0;Index<Count;Index+=16){
    if(CompareMem(&Data8[Index], gBiosidSign, sizeof(gBiosidSign)) == 0){
      CopyMem(BiosIdImage, &Data8[Index], sizeof(BIOS_ID_IMAGE));
      Status = EFI_SUCCESS;
      break;
    }      
  }

  DEBUG((EFI_D_INFO, __FUNCTION__":%r\n", Status));
  return Status;
}



EFI_STATUS
GetRecoveryCapsuleBiosIdImage (
  IN UINTN                CapsuleBaseAddress,
  IN OUT BIOS_ID_IMAGE    *BiosIdImage
  )
{
  UINT8                        *Data8;
  UINTN                        Index;
  UINTN                        Count;
  EFI_FIRMWARE_VOLUME_HEADER   *FvHdr;
  EFI_STATUS                   Status = EFI_NOT_FOUND;

  Count = SIZE_4KB;
  FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)(CapsuleBaseAddress + PcdGet32(PcdFlashFvMainBase) - PcdGet32(PcdFlashAreaBaseAddress));
  if(FvHdr->Signature != EFI_FVH_SIGNATURE || FvHdr->FvLength != PcdGet32(PcdFlashFvMainSize)){
    goto ProcExit;
  }

  Data8 = (UINT8*)(FvHdr+1);
  for(Index=0;Index<Count;Index+=16){
    if(CompareMem(&Data8[Index], gBiosidSign, sizeof(gBiosidSign)) == 0){
      CopyMem(BiosIdImage, &Data8[Index], sizeof(BIOS_ID_IMAGE));
      Status = EFI_SUCCESS;
      break;
    }      
  }  

ProcExit:
  if(EFI_ERROR(Status)){	
    DEBUG((EFI_D_INFO, __FUNCTION__":%r\n", Status));
  }		
  return Status;
}

UINT8 mPubKeySignature[5] = {"_KH_"};	

EFI_STATUS
GetPubkey (
  IN  UINT8     *StartAddress,
  IN  UINT8     *EndAddress,
  OUT UINT8     **Buffer
)
{
  EFI_STATUS    Status;
  UINT8         *Adress;
  Status = EFI_NOT_FOUND;

  for (Adress = StartAddress; Adress < EndAddress; ) {
    if (*(UINT32 *)Adress == *(UINT32 *)mPubKeySignature) {
      Adress += 4; // skip signature "_KH_"
      *Buffer = Adress;
      DEBUG ((DEBUG_INFO, "Get PubKey Buffer:%x\n", *Buffer));
      Status = EFI_SUCCESS;
      break;
    }
    Adress += 16;
  }

  return Status;
}

