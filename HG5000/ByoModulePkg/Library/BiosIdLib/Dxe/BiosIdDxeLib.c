/** @file
==========================================================================================
      NOTICE: Copyright (c) 2006 - 2017 Byosoft Corporation. All rights reserved.
              This program and associated documentation (if any) is furnished
              under a license. Except as permitted by such license,no part of this
              program or documentation may be reproduced, stored divulged or used
              in a public system, or transmitted in any form or by any means
              without the express written consent of Byosoft Corporation.
==========================================================================================

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Library/BiosIdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>


STATIC CONST UINT8 gBiosidSign[] = BIOS_ID_SIGN_DATA;



EFI_STATUS
GetBiosId (
  OUT BIOS_ID_IMAGE     *BiosIdImage
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
  VOID         *p;

  ASSERT(BiosIdImage != NULL);
  
  p = (VOID*)(UINTN)PcdGet64(PcdBiosIdPtr);
  if(p != NULL){
    CopyMem(BiosIdImage, p, sizeof(BIOS_ID_IMAGE));
    ASSERT(CompareMem(BiosIdImage->Signature, gBiosidSign, sizeof(gBiosidSign))==0);
    return EFI_SUCCESS;
  }

  Data8 = (UINT8*)(UINTN)PcdGet32(PcdFlashFvMainBase);
  Count = PcdGet32(PcdFlashFvMainSize);
  
  for(Index=0;Index<Count;Index+=16){
    if(CompareMem(&Data8[Index], gBiosidSign, sizeof(gBiosidSign)) == 0){
      CopyMem(BiosIdImage, &Data8[Index], sizeof(BIOS_ID_IMAGE));
      Status = gBS->AllocatePool(EfiBootServicesData, sizeof(BIOS_ID_IMAGE), (VOID**)&p);
      ASSERT(!EFI_ERROR(Status));
      CopyMem(p, BiosIdImage, sizeof(BIOS_ID_IMAGE));
      PcdSet64(PcdBiosIdPtr, (UINTN)p);
      break;
    }      
  }

  DEBUG((EFI_D_INFO, "%a():%r P:%X\n", __FUNCTION__, Status, p));
  return Status;
}


EFI_STATUS
GetBiosBuildTimeHHMM (
  OUT UINT8     *HH,
  OUT UINT8     *MM
  )
{
  BIOS_ID_IMAGE  BiosIdImage;
  EFI_STATUS     Status;
  UINT16         HhMm;


  Status = GetBiosId(&BiosIdImage);
  if(EFI_ERROR(Status)){
    return Status;
  }

  HhMm = (UINT16)StrDecimalToUintn(BiosIdImage.BiosIdString.TimeStamp + 6);
  *HH = (UINT8)(HhMm / 100);
  *MM = (UINT8)(HhMm % 100);

  return EFI_SUCCESS;  
}




EFI_STATUS
GetImageBiosId (
  IN UINTN                ImageAddress,
  IN UINTN                ImageSize,
  IN OUT BIOS_ID_IMAGE    *BiosIdImage
  )
{
  UINT8                        *Data8;
  UINTN                        Index;
  UINTN                        Count;
  EFI_FIRMWARE_VOLUME_HEADER   *FvHdr;
  EFI_STATUS                   Status = EFI_NOT_FOUND;
  UINTN                        Offset;
  UINTN                        o;


  Offset = PcdGet32(PcdFlashFvMainBase) - PcdGet32(PcdFlashAreaBaseAddress);
  Count = SIZE_4KB;

  if(Offset + Count > ImageSize){
    DEBUG((EFI_D_ERROR, "out of range %X %X %X\n", Offset, Count, ImageSize));
    goto ProcExit;
  }

  FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)(ImageAddress + Offset);
  if(FvHdr->Signature != EFI_FVH_SIGNATURE){
    DEBUG((EFI_D_INFO, "Sign:%08X\n", FvHdr->Signature));
    goto ProcExit;
  }

  o = sizeof(EFI_FIRMWARE_VOLUME_HEADER);
  o = ALIGN_VALUE(o, 16);
  Data8 = (UINT8*)((UINTN)FvHdr + o);
  Count = Count - o;
  for(Index=0;Index<Count;Index+=16){
    if(CompareMem(&Data8[Index], gBiosidSign, sizeof(gBiosidSign)) == 0){
      CopyMem(BiosIdImage, &Data8[Index], sizeof(BIOS_ID_IMAGE));
      Status = EFI_SUCCESS;
      break;
    }      
  }  

ProcExit:
  if(EFI_ERROR(Status)){	
    DEBUG((EFI_D_ERROR, "%a:%r L:%X O:%X C:%X\n", __FUNCTION__, Status, ImageSize, Offset, Count));
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

