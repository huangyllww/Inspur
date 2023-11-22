/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SmiFlashPei.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/ByoCommLib.h>
#include <RsaSignDataHob.h>

#include <Library/BiosIdLib.h>

EFI_STATUS
EFIAPI
ByoGetBiosSignPubKey (
    VOID    **pPubKeyData,
    UINT32  *pPubKeyDataSize
  )
{
  EFI_STATUS                   Status = EFI_NOT_FOUND;
  UINT32                       PubKeyDataSize;
  EFI_PEI_FV_HANDLE            VolumeHandle;
  EFI_PEI_FILE_HANDLE          FileHandle;
  VOID                         *SignData;
  UINTN                        Instance;

  Instance = 0;
  while (TRUE) {
    Status = PeiServicesFfsFindNextVolume(Instance, &VolumeHandle);
      if (EFI_ERROR (Status)) {
        break;
      }

    Status = PeiServicesFfsFindFileByName(&gByoBiosSignPubKeyFileGuid, VolumeHandle, &FileHandle);
    if(!EFI_ERROR(Status)){
      Status = PeiServicesFfsFindSectionData(EFI_SECTION_RAW, FileHandle, &SignData);
      ASSERT_EFI_ERROR(Status);

      PubKeyDataSize = SECTION_SIZE (((EFI_COMMON_SECTION_HEADER*)((UINT8*)SignData - sizeof(EFI_COMMON_SECTION_HEADER))));
      PubKeyDataSize -= sizeof(EFI_COMMON_SECTION_HEADER);
      *pPubKeyData = SignData;
      *pPubKeyDataSize = PubKeyDataSize;
      break;
    }

    Instance++;
  }

  DEBUG((EFI_D_INFO, "GetSignPK:%r\n", Status));
  return Status;
}


EFI_STATUS
EFIAPI
SmiFlashPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                  Status;
  VOID                        *PubKeyData;
  UINT32                      PubKeyDataSize;
  PK_RSA2048_HOB_DATA         *PubKey;
  

  DEBUG((EFI_D_INFO, "SmiFlashPeiEntry\n"));
  
  Status = BuildBiosIdInfoHob(NULL,NULL,0);
  if(EFI_ERROR(Status)){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  Status = ByoGetBiosSignPubKey(&PubKeyData, &PubKeyDataSize);
  if(EFI_ERROR(Status)){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  DEBUG((EFI_D_INFO, "PubKey(%X,%X)\n", PubKeyData, PubKeyDataSize));

// !!! Hob Data Length is align 8 byte.
  PubKey = (PK_RSA2048_HOB_DATA*)BuildGuidHob(&gByoBiosSignPubKeyFileGuid, sizeof(PK_RSA2048_HOB_DATA));
  ASSERT(PubKey!=NULL);
  CopyMem(PubKey->KeyN, PubKeyData, PubKeyDataSize);
  PubKey->Hdr.KeySize = (UINT16)PubKeyDataSize;
  LibCalcCrc32(PubKey->KeyN, PubKey->Hdr.KeySize, &PubKey->Hdr.Crc32);
  PubKey->Hdr.IsProKey = TRUE;

  DEBUG((EFI_D_INFO, "PubKeyHob OK\n"));

ProcExit:
  return Status; 
}  


