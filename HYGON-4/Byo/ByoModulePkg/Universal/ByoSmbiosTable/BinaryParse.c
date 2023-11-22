/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <PiDxe.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/FirmwareVolume2.h>
#include <Library/PeCoffLib.h>
#include <Uefi/UefiInternalFormRepresentation.h>

#define MAX_MATCH_GUID_NUM             100

#define EFI_VFR_ATTRACT_GUID \
{ 0xd0bc7cb4, 0x6a47, 0x495f, { 0xaa, 0x11, 0x71, 0x7, 0x46, 0xda, 0x6, 0xa2 } }

extern EFI_PHYSICAL_ADDRESS           IfrDataAddress;

UINT8                 *TempIfrTotalData = NULL;
UINTN                 TempIfrTotalDataSize = 0;

//
// The Guid to sign the position of Vfr and Uni array in FV
//
EFI_GUID  gVfrArrayAttractGuid                         = EFI_VFR_ATTRACT_GUID;

static
BOOLEAN
GetNextOffset (
  IN UINT8 *Data,
  IN EFI_GUID *Guid,
  IN UINTN Len,
  IN OUT UINTN *Offset
  )
{
  UINTN NextOffset;
  if (*Offset >= Len || Len - *Offset <= sizeof (EFI_GUID)) {
    return FALSE;
  }

  for (NextOffset = *Offset; NextOffset < Len - sizeof (EFI_GUID); NextOffset++) {
    if (CompareGuid(Guid, (EFI_GUID*)(Data + NextOffset)) == TRUE) {
      *Offset = NextOffset + sizeof(EFI_GUID);
      return TRUE;
    }
  }
  return FALSE;
}


/**
  Get the address by Guid.

  Parse the FFS image, and find the GUID address.There may be some Guids matching the
  searched Guid.

  @param Fv                     the Pointer to the image.
  @param Guid                   The Guid need to find.
  @param Offset                 The dual Pointer to the offset.
  @param NumOfMatchGuid         The number of matching Guid offset.

  @retval EFI_SUCCESS           The Search was complete successfully
  @return EFI_ABORTED           An error occurred
**/
EFI_STATUS
GetAddressByGuid (
  IN  VOID        *RawData,
  IN  UINTN       RawLength,
  IN  EFI_GUID    *Guid,
  OUT UINTN       **Offset,
  OUT UINT8       *NumOfMatchGuid
  )
{
  UINTN       NextOffset=0;
  UINT8       Flag = 0;

  while (GetNextOffset(RawData, Guid, RawLength, &NextOffset)) {
    Flag = 1;
    if ((NumOfMatchGuid != NULL) && (Offset != NULL)) {
      if (*NumOfMatchGuid == 0) {
        *Offset = AllocatePool (sizeof (UINTN) * MAX_MATCH_GUID_NUM);
        if (*Offset == NULL) {
          return EFI_ABORTED;
        }
        SetMem (*Offset, sizeof (UINTN) * MAX_MATCH_GUID_NUM, 0);
      }
      *(*Offset + *NumOfMatchGuid) = * (UINTN *) ((UINT8 *)RawData + NextOffset);
      (*NumOfMatchGuid)++;
    } else {
      return EFI_SUCCESS;
    }
  }


  if( Flag == 0 ) {
    return EFI_ABORTED;
  }
  return EFI_SUCCESS;
}

EFI_STATUS
ReadAllIfrFromEfiImage (
  UINT8                    *EfiImageBase,
  UINTN                    EfiImageSize
  )
{
  PE_COFF_LOADER_IMAGE_CONTEXT   ImageContext;
  UINT8                          *MemoryImagePointer;
  EFI_STATUS                     Status;
  EFI_HII_PACKAGE_LIST_HEADER    *PackageList;
  EFI_HII_PACKAGE_HEADER         *PackageHeader;

  //
  // Get HII resource data from EFI image
  //
  ZeroMem (&ImageContext, sizeof (ImageContext));
  ImageContext.Handle    = EfiImageBase;
  ImageContext.ImageRead = PeCoffLoaderImageReadFromMemory;
  Status                 = PeCoffLoaderGetImageInfo(&ImageContext);
  if (Status == EFI_SUCCESS && (!ImageContext.IsTeImage)) {
    //
    // Load Image Data
    //
    MemoryImagePointer = (UINT8 *) AllocatePool ((UINTN) ImageContext.ImageSize + ImageContext.SectionAlignment);
    ASSERT (MemoryImagePointer != NULL);
    ZeroMem ((VOID *) MemoryImagePointer, (UINTN) ImageContext.ImageSize + ImageContext.SectionAlignment);
    ImageContext.ImageAddress = ((UINTN) MemoryImagePointer + ImageContext.SectionAlignment - 1) & (~((INT64)ImageContext.SectionAlignment - 1));

    Status = PeCoffLoaderLoadImage (&ImageContext);
    if (Status == EFI_SUCCESS && (ImageContext.HiiResourceData != 0)) {
      PackageList = (EFI_HII_PACKAGE_LIST_HEADER *) (UINTN) ImageContext.HiiResourceData;
      PackageHeader = (EFI_HII_PACKAGE_HEADER *) (PackageList + 1);
      while (PackageHeader->Type != EFI_HII_PACKAGE_END) {
        if (PackageHeader->Type == EFI_HII_PACKAGE_FORMS) {
          CopyMem(TempIfrTotalData + TempIfrTotalDataSize, PackageHeader, PackageHeader->Length);
          TempIfrTotalDataSize = TempIfrTotalDataSize + PackageHeader->Length;
        }

        PackageHeader = (EFI_HII_PACKAGE_HEADER *) ((UINT8 *) PackageHeader + PackageHeader->Length);
      }
    }
    FreePool (MemoryImagePointer);
  }

  if (ImageContext.HiiResourceData != 0) {
    return EFI_SUCCESS;
  }

  return EFI_NOT_FOUND;
}

static
EFI_STATUS
ReadAllIfrToFromset (
  UINT8                    *EfiImageBase,
  UINTN                    EfiImageSize,
  UINTN                    *VfrBinBaseAddress,
  UINT8                    NumberofMachingVfrBin
  )
{
  UINTN      Index;
  EFI_STATUS Status = EFI_SUCCESS;
  UINT8      *IfrBinaryData;
  UINTN      IfrBinarySize;

  //
  // Read all Ifr information into Formset and Form structure
  //
  for (Index = 0; Index < NumberofMachingVfrBin; Index++) {
    if (EfiImageSize < *(VfrBinBaseAddress + Index)) {
      DEBUG ((DEBUG_ERROR, "Failed to locate Ifr data from efi by the offset.\n"));
      Status = EFI_ABORTED;
      goto Done;
    }

    IfrBinaryData = (UINT8 *)(EfiImageBase + *(VfrBinBaseAddress + Index));
    IfrBinarySize = ((EFI_HII_PACKAGE_HEADER *) IfrBinaryData)->Length;
    if (((EFI_HII_PACKAGE_HEADER *) IfrBinaryData)->Type == 0x0) {
      IfrBinarySize = *(UINT32 *)IfrBinaryData;
    }
    CopyMem(TempIfrTotalData + TempIfrTotalDataSize, IfrBinaryData, IfrBinarySize);
    TempIfrTotalDataSize = TempIfrTotalDataSize + IfrBinarySize;
    // DEBUG ((DEBUG_ERROR, "PcdSmbiosTableAddSuppressIf IfrBinarySize:%llx;TempIfrTotalDataSize:%llx\n", IfrBinarySize,TempIfrTotalDataSize));
  }

Done:
  //
  // Free the memory which stores the offset
  //
  if (VfrBinBaseAddress != NULL) {
    FreePool (VfrBinBaseAddress);
  }
  return Status;
}



EFI_STATUS
ParseAllFfs(
 )
{
  EFI_STATUS                Status;
  UINTN                     FvIndex;
  UINTN                     NumberHandles;
  EFI_HANDLE                *FirmWareProtocolHandle = NULL;
  EFI_FIRMWARE_VOLUME2_PROTOCOL   *FirmWareProtocol;
  UINTN                     Key;
  EFI_FV_FILETYPE           FileType;
  EFI_GUID                  FileGuid;
  EFI_FV_FILE_ATTRIBUTES    Attributes;
  UINT32                    AuthenticationStatus;
  VOID                      *EfiImageBase;
  UINTN                     EfiImageSize;
  VOID                      *RawSection;
  UINTN                     RawSectionSize;
  UINTN                     Index;
  UINT8                     NumberofMachingVfrBin;
  UINTN                     *VfrBinBaseAddress;
  UINTN                     FfsSize;

  Status = gBS->LocateHandleBuffer(
                ByProtocol,
                &gEfiFirmwareVolume2ProtocolGuid,
                NULL,
                &NumberHandles,
                &FirmWareProtocolHandle
                );
  if (EFI_ERROR(Status)){
    DEBUG ((DEBUG_ERROR, "LocateHandleBuffer failed\n"));
    return Status;
  }
  //Get ffs
  
  TempIfrTotalData = AllocateZeroPool(SIZE_1MB);
  TempIfrTotalDataSize = 0;

  for (FvIndex = 0; FvIndex < NumberHandles; FvIndex++){
    Status = gBS->HandleProtocol(
                FirmWareProtocolHandle[FvIndex],
                &gEfiFirmwareVolume2ProtocolGuid,
                (VOID **) &FirmWareProtocol
                );
    if (EFI_ERROR(Status)){
      DEBUG ((DEBUG_ERROR, "HandleProtocol failed\n"));
      break;
    }

    Key      = 0;
    FileType = EFI_FV_FILETYPE_DRIVER;

    while (1) {
      Status = FirmWareProtocol->GetNextFile (
        FirmWareProtocol,
        &Key,
        &FileType,
        &FileGuid,
        &Attributes,
        &FfsSize
      );
      if (EFI_ERROR (Status)) {
        break;
      }

      EfiImageBase = NULL;
      EfiImageSize = 0;
      Status = FirmWareProtocol->ReadSection (
        FirmWareProtocol,
        &FileGuid,
        EFI_SECTION_PE32,
        0,
        &EfiImageBase,
        &EfiImageSize,
        &AuthenticationStatus
      );
      if (EFI_ERROR (Status)) {
        continue;
      }
      //
      // Read HII data from HiiResource Section
      //
      Status = ReadAllIfrFromEfiImage (EfiImageBase, EfiImageSize);
      if (!EFI_ERROR (Status)) {
        FreePool (EfiImageBase);
        continue;
      }
      //
      // Read HII data from EFI image
      //
      Index = 0;
      while (1) {
        RawSection     = NULL;
        RawSectionSize = 0;

        Status = FirmWareProtocol->ReadSection (
          FirmWareProtocol,
          &FileGuid,
          EFI_SECTION_RAW,
          Index++,
          &RawSection,
          &RawSectionSize,
          &AuthenticationStatus
        );

        if (EFI_ERROR (Status)) {
          break;
        }

        NumberofMachingVfrBin = 0;
        VfrBinBaseAddress     = NULL;
        Status = GetAddressByGuid (
                  RawSection,
                  RawSectionSize,
                  &gVfrArrayAttractGuid,
                  &VfrBinBaseAddress,
                  &NumberofMachingVfrBin
                  );
        if (!EFI_ERROR (Status)) {
          ReadAllIfrToFromset (
            EfiImageBase,
            EfiImageSize,
            VfrBinBaseAddress,
            NumberofMachingVfrBin
          );
          FreePool (EfiImageBase);
          FreePool (RawSection);
          break;
        }
        FreePool (RawSection);
      }
    }
  }

  if (TempIfrTotalDataSize != 0){

    IfrDataAddress = SIZE_4GB - 1;
    Status  = gBS->AllocatePages (
                    AllocateMaxAddress,
                    EfiReservedMemoryType,
                    EFI_SIZE_TO_PAGES (TempIfrTotalDataSize),
                    &IfrDataAddress
                    );
    if (EFI_ERROR (Status)) {
      Status  = gBS->AllocatePages (
                    AllocateAnyPages,
                    EfiReservedMemoryType,
                    EFI_SIZE_TO_PAGES (TempIfrTotalDataSize),
                    &IfrDataAddress
                    );
    }
    ASSERT_EFI_ERROR (Status);
    CopyMem((VOID *)IfrDataAddress, TempIfrTotalData, TempIfrTotalDataSize);
    FreePool(TempIfrTotalData);
  }

  FreePool (FirmWareProtocolHandle);

  return Status;
}


