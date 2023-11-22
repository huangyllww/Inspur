/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BiosIdLib.c

Abstract:
  BIOS ID library implementation.

Revision History:

**/

#include <Uefi.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Pi/PiFirmwareFile.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PrintLib.h>
#include <Library/BiosIdLib.h>
#include <Pi/PiFirmwareVolume.h>


STATIC CONST UINT8 gBiosidSign[] = BIOS_ID_SIGN_DATA;

VOID *
EFIAPI
GetBiosIdFormAnyFv(
  VOID
);

EFI_STATUS
EFIAPI
GetPlatBiosIdInfo (
  IN OUT BIOS_ID_INFO  *BiosIdInfo,
  IN VOID              *FvStart,
  IN BIOS_ID_BOARD_ID  *BoardIdArray,
  IN UINTN             BoardIdCount
  )
{
  EFI_STATUS                 Status;
  BIOS_ID_IMAGE              BiosIdImage;
  UINTN                      Index;
  CHAR16                     *p;
  CHAR8                      *s;
  UINT16                     VerValue;
  UINT8                      RevisionVer = 0xFF;
  CHAR8                      *BiosVersion;
  EFI_COMMON_SECTION_HEADER  *SecHdr;
  UINT32                     SecDataSize;
  UINT32                     ExtDataSize;
  CHAR16                     *ExtDataStr;
  CHAR16                     Ver[5];


  DEBUG((EFI_D_INFO, "GetPlatBiosIdInfo\n"));

  ZeroMem(BiosIdInfo, sizeof(BIOS_ID_INFO));
  DEBUG((EFI_D_INFO, "FvStart %x\n", FvStart));

  Status = GetBiosIdSection(FvStart, (VOID**)&SecHdr);
  ASSERT(!EFI_ERROR(Status));
  
  CopyMem(&BiosIdInfo->BiosId, SecHdr+1, sizeof(BIOS_ID_IMAGE));
  CopyMem(&BiosIdImage, &BiosIdInfo->BiosId, sizeof(BIOS_ID_IMAGE));  

  SecDataSize = SECTION_SIZE(SecHdr) - sizeof(EFI_COMMON_SECTION_HEADER);
  if(SecDataSize > sizeof(BIOS_ID_IMAGE)){
    ExtDataSize = SecDataSize - sizeof(BIOS_ID_IMAGE);
    ExtDataStr  = (CHAR16*)((UINT8*)(SecHdr+1) + sizeof(BIOS_ID_IMAGE));
    DEBUG((EFI_D_INFO, "ExtStr(%x,%x)\n", ExtDataStr, ExtDataSize));
    if(ExtDataSize >= sizeof(CHAR16)*2){
      CopyMem(Ver, ExtDataStr, sizeof(CHAR16)*2);
      Ver[2] = 0;
      RevisionVer = (UINT8)StrDecimalToUintn(Ver);
    }
  }

  CopyMem(Ver, BiosIdImage.BiosIdString.VersionMajor, sizeof(BiosIdImage.BiosIdString.VersionMajor));
  Ver[ARRAY_SIZE(BiosIdImage.BiosIdString.VersionMajor)] = 0;
  VerValue = (UINT16)StrDecimalToUintn(Ver);
  BiosIdInfo->VerMajor = (UINT8)(VerValue / 100);
  BiosIdInfo->VerMinor = (UINT8)(VerValue % 100);

  BiosVersion = (CHAR8*)PcdGetPtr(PcdSmbiosBiosVersion);
  if(BiosVersion[0] == 0){
    if(RevisionVer != 0xFF){
      AsciiSPrint (
        BiosIdInfo->BiosVer,
        sizeof(BiosIdInfo->BiosVer),
        "%02d.%02d.%02d",
        BiosIdInfo->VerMajor,
        BiosIdInfo->VerMinor,
        RevisionVer
        );
    } else {
      AsciiSPrint (
        BiosIdInfo->BiosVer,
        sizeof(BiosIdInfo->BiosVer),
        "%02d.%02d",
        BiosIdInfo->VerMajor,
        BiosIdInfo->VerMinor
        );
    }
  } else {
    AsciiSPrint (
      BiosIdInfo->BiosVer,
      sizeof(BiosIdInfo->BiosVer),
      "%a",
      BiosVersion
      );
  }

  p = BiosIdImage.BiosIdString.TimeStamp;     // YYMMDDHHMM -> "05/02/2018"
  s = BiosIdInfo->BiosDate;
  s[0] = (CHAR8)p[2];
  s[1] = (CHAR8)p[3];
  s[2] = '/';
  s[3] = (CHAR8)p[4];
  s[4] = (CHAR8)p[5];
  s[5] = '/';
  s[6] = '2';
  s[7] = '0';
  s[8] = (CHAR8)p[0];
  s[9] = (CHAR8)p[1];
  s[10] = 0;

  for(Index = 0; Index < BoardIdCount; Index++){
    if(CompareMem(BiosIdImage.BiosIdString.BoardId, BoardIdArray[Index].BoardId, sizeof(BiosIdImage.BiosIdString.BoardId)) == 0 &&
       CompareMem(BiosIdImage.BiosIdString.OemId, BoardIdArray[Index].ExtId, sizeof(BiosIdImage.BiosIdString.OemId)) == 0){
      AsciiStrCpyS(BiosIdInfo->BoardId, sizeof(BiosIdInfo->BoardId), BoardIdArray[Index].BoardName);
      break;
    }
  }
  if(Index >= BoardIdCount){
    p = BiosIdImage.BiosIdString.BoardId;
    s = BiosIdInfo->BoardId;
    for(Index=0;Index<7;Index++){
      if(p[Index] == L'_'){
        break;
      }
      s[Index] = (CHAR8)p[Index];
    }
    s[Index] = 0;
  }

  BiosIdInfo->Signature = BIOS_ID_INFO_SIGN;

  // dos 8.3 format
  s = (CHAR8*)PcdGetPtr(PcdBiosFileBaseName);
  DEBUG((EFI_D_INFO, "PcdBiosFileBaseName %a\n",s));

  for(Index = 0; Index < 8; Index++){
    if(s[Index] == 0){
      break;
    }
    BiosIdInfo->RecoveryFileName[Index] = s[Index];
  }
  BiosIdInfo->RecoveryFileName[Index] = 0;

  DEBUG((EFI_D_INFO, "%a() %a %a %a %d.%d %s\n", __FUNCTION__, BiosIdInfo->BoardId, \
                        BiosIdInfo->BiosVer, BiosIdInfo->BiosDate, \
                        BiosIdInfo->VerMajor, BiosIdInfo->VerMinor, BiosIdInfo->RecoveryFileName));
  return Status;
}

EFI_STATUS
EFIAPI
BuildBiosIdInfoHob (
  IN VOID              *FvStart,
  IN BIOS_ID_BOARD_ID  *BoardIdArray,
  IN UINTN              BoardIdCount
  )
{
  BIOS_ID_INFO           *BiosIdInfoHob;
  EFI_STATUS             Status;
  BIOS_ID_INFO           BiosIdInfo;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
  Status = GetBiosIdInfoFromHob (&BiosIdInfo);
  if(Status == EFI_SUCCESS){
    return Status;
  }

  if(FvStart == NULL){
    FvStart =(VOID*)(UINTN)GetBiosIdFormAnyFv();
  }

  if(FvStart == NULL){
    return EFI_NOT_FOUND;
  }

  BiosIdInfoHob = (BIOS_ID_INFO*)BuildGuidHob(&gBiosIdHobGuid, sizeof(BIOS_ID_INFO));
  ASSERT(BiosIdInfoHob!=NULL);

  Status = GetPlatBiosIdInfo(BiosIdInfoHob, FvStart, BoardIdArray, BoardIdCount);

  return Status;
}

EFI_STATUS
EFIAPI
GetBiosIdInfoFromHob (
  IN OUT BIOS_ID_INFO  *BiosIdInfo
  )
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *Info;

  GuidHob.Raw = GetFirstGuidHob(&gBiosIdHobGuid);
  if (GuidHob.Raw != NULL) {
    Info = (VOID*)(GuidHob.Guid+1);
    if (((BIOS_ID_INFO *)Info)->Signature == BIOS_ID_INFO_SIGN) {
      CopyMem(BiosIdInfo, Info, sizeof(BIOS_ID_INFO));
      return EFI_SUCCESS;
    } else {
      return EFI_NOT_FOUND;
    }
  } else {
    return EFI_NOT_FOUND;
  }
}



EFI_STATUS
EFIAPI
GetBiosIdSection (
  IN  VOID           *FvStart,
  OUT VOID           **SectionHdr  
  )
{
  UINT8                  *Data8;
  UINTN                  Index;
  
  Data8 = (UINT8*)FvStart;
  for(Index=0;Index<SIZE_4KB;Index+=16){
    if(CompareMem(&Data8[Index], gBiosidSign, sizeof(gBiosidSign)) == 0){
      *SectionHdr = Data8 + Index - 4;
      return EFI_SUCCESS;
    }      
  }

  return EFI_NOT_FOUND;
}




EFI_STATUS
EFIAPI
GetBiosIdInFv (
  OUT BIOS_ID_IMAGE  *BiosIdImage,
  IN  VOID           *FvStart   
  )
{
  UINT8                  *Data8;
  UINTN                  Index;
  
  Data8 = (UINT8*)FvStart;
  for(Index=0;Index<SIZE_4KB;Index+=16){
    if(CompareMem(&Data8[Index], gBiosidSign, sizeof(gBiosidSign)) == 0){
      CopyMem(BiosIdImage, &Data8[Index], sizeof(BIOS_ID_IMAGE));
      return EFI_SUCCESS;
    }      
  }

  return EFI_NOT_FOUND;
}



EFI_STATUS
GetBiosIdInAnyFv (
  OUT BIOS_ID_IMAGE  *BiosIdImage,
  IN  UINT8          *BiosData,
  IN  UINTN          BiosDataSize
)
{
  UINTN                           Index = 0;
  UINTN                           SearchEnd;
  EFI_FIRMWARE_VOLUME_HEADER      *FvHdr;
  EFI_STATUS                      Status;


  SearchEnd = BiosDataSize - sizeof(EFI_FIRMWARE_VOLUME_HEADER);
  while(Index < SearchEnd){
    FvHdr = (EFI_FIRMWARE_VOLUME_HEADER*)(BiosData + Index);
    if(FvHdr->Signature == EFI_FVH_SIGNATURE && CalculateSum16((UINT16*)FvHdr, FvHdr->HeaderLength) == 0){
      Status = GetBiosIdInFv(BiosIdImage, FvHdr);
      if(!EFI_ERROR(Status)){
        return Status;
      }

      Index += (UINTN)FvHdr->FvLength;
    } else {
      Index += SIZE_4KB;
    }
  }

  return EFI_NOT_FOUND;
}



EFI_STATUS
EFIAPI
GetBiosBuildTimeHHMM (
  OUT UINT8     *HH,
  OUT UINT8     *MM
  )
{
  EFI_STATUS     Status;
  BIOS_ID_IMAGE  BiosIdImage;
  UINT16         HhMm;
  BIOS_ID_INFO   BiosIdInfo;

  Status = GetBiosIdInfoFromHob (&BiosIdInfo);
  if (!EFI_ERROR (Status)) {
    CopyMem (&BiosIdImage, &BiosIdInfo.BiosId, sizeof (BIOS_ID_IMAGE));
  } else {
    return Status;
  }

  HhMm = (UINT16)StrDecimalToUintn(BiosIdImage.BiosIdString.TimeStamp + 6);
  *HH = (UINT8)(HhMm / 100);
  *MM = (UINT8)(HhMm % 100);

  return Status;
}

/**
  This function returns BIOS ID by searching HOB or FV.
  It also debug print the BIOS ID found.

  @param[out] BiosIdImage   The BIOS ID got from HOB or FV. It is optional,
                            no BIOS ID will be returned if it is NULL as input.

  @retval EFI_SUCCESS               BIOS ID has been got successfully.
  @retval EFI_NOT_FOUND             BIOS ID image is not found, and no parameter will be modified.

**/
EFI_STATUS
EFIAPI
GetBiosId (
  OUT BIOS_ID_IMAGE     *BiosIdImage OPTIONAL
  )
{
  EFI_STATUS            Status;
  BIOS_ID_IMAGE         TempBiosIdImage;
  BIOS_ID_INFO           BiosIdInfo;

  if (BiosIdImage == NULL) {
    //
    // It is NULL as input, so no BIOS ID will be returned.
    // Use temp buffer to hold the BIOS ID.
    //
    BiosIdImage = &TempBiosIdImage;
  }

  Status = BuildBiosIdInfoHob(NULL,NULL,0);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_FOUND;
  }

  Status = GetBiosIdInfoFromHob(&BiosIdInfo);
  if (!EFI_ERROR (Status)) {
    CopyMem (BiosIdImage, &BiosIdInfo.BiosId, sizeof (BIOS_ID_IMAGE));
  }

  return Status;
}

EFI_STATUS
EFIAPI
GetBiosVersionDateTime (
  OUT CHAR16    *BiosVersion, OPTIONAL
  OUT CHAR16    *BiosReleaseDate, OPTIONAL
  OUT CHAR16    *BiosReleaseTime OPTIONAL
  )
{
  EFI_STATUS        Status;
  BIOS_ID_IMAGE     BiosIdImage;
  BIOS_ID_INFO           BiosIdInfo;
  UINTN                 Size;

  Size    = 0;
  if ((BiosVersion == NULL) && (BiosReleaseDate == NULL) && (BiosReleaseTime == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = GetBiosIdInfoFromHob(&BiosIdInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  } else {
    CopyMem (&BiosIdImage, &BiosIdInfo.BiosId, sizeof (BIOS_ID_IMAGE));
  }

  if (BiosVersion != NULL) {
    //
    // Fill the BiosVersion data from the BIOS ID.
    //
    CopyMem (BiosVersion, &(BiosIdImage.BiosIdString), sizeof (BIOS_ID_STRING)-sizeof(CHAR16)*12);
    Size = sizeof (BIOS_ID_STRING)-sizeof(CHAR16)*12;
    ASSERT ((Size % 2) == 0);
    BiosVersion[Size/2] = (CHAR16) ((UINT8) ('\0'));
  }
  
  if (BiosReleaseDate != NULL) {
    //
    // Fill the build timestamp date from the BIOS ID in the "MM/DD/YY" format.
    //
    BiosReleaseDate[0] = BiosIdImage.BiosIdString.TimeStamp[2];
    BiosReleaseDate[1] = BiosIdImage.BiosIdString.TimeStamp[3];
    BiosReleaseDate[2] = (CHAR16) ((UINT8) ('/'));

    BiosReleaseDate[3] = BiosIdImage.BiosIdString.TimeStamp[4];
    BiosReleaseDate[4] = BiosIdImage.BiosIdString.TimeStamp[5];
    BiosReleaseDate[5] = (CHAR16) ((UINT8) ('/'));

    //
    // Add 20 for SMBIOS table
    // Current Linux kernel will misjudge 09 as year 0, so using 2009 for SMBIOS table
    //
    BiosReleaseDate[6] = '2';
    BiosReleaseDate[7] = '0';
    BiosReleaseDate[8] = BiosIdImage.BiosIdString.TimeStamp[0];
    BiosReleaseDate[9] = BiosIdImage.BiosIdString.TimeStamp[1];

    BiosReleaseDate[10] = (CHAR16) ((UINT8) ('\0'));
  }

  if (BiosReleaseTime != NULL) {

    //
    // Fill the build timestamp time from the BIOS ID in the "HH:MM" format.
    //
    BiosReleaseTime[0] = BiosIdImage.BiosIdString.TimeStamp[6];
    BiosReleaseTime[1] = BiosIdImage.BiosIdString.TimeStamp[7];
    BiosReleaseTime[2] = (CHAR16) ((UINT8) (':'));

    BiosReleaseTime[3] = BiosIdImage.BiosIdString.TimeStamp[8];
    BiosReleaseTime[4] = BiosIdImage.BiosIdString.TimeStamp[9];

    BiosReleaseTime[5] = (CHAR16) ((UINT8) ('\0'));
  }

  return  EFI_SUCCESS;
}
