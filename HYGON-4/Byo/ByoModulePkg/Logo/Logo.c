/** @file
  Logo DXE Driver, install Edkii Platform Logo protocol.

Copyright (c) 2016 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


**/
#include <Uefi.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/HiiImageEx.h>
#include <Protocol/PlatformLogo.h>
#include <Protocol/HiiPackageList.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/HiiLib.h>
#include <Library/BmpSupportLib.h>
#include <IndustryStandard/Bmp.h>
#include <Protocol/HiiImageDecoder.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

typedef struct {
  EFI_IMAGE_ID                          ImageId;
  EDKII_PLATFORM_LOGO_DISPLAY_ATTRIBUTE Attribute;
  INTN                                  OffsetX;
  INTN                                  OffsetY;
} LOGO_ENTRY;

EFI_HII_IMAGE_EX_PROTOCOL *mHiiImageEx;
EFI_HII_HANDLE            mHiiHandle = NULL;
LOGO_ENTRY                mLogos[] = {
  {
    IMAGE_TOKEN (IMG_LOGO),
    EdkiiPlatformLogoDisplayAttributeCenter,
    0,
    0
  }
};

/**
  Load a platform logo image and return its data and attributes.

  @param This              The pointer to this protocol instance.
  @param Instance          The visible image instance is found.
  @param Image             Points to the image.
  @param Attribute         The display attributes of the image returned.
  @param OffsetX           The X offset of the image regarding the Attribute.
  @param OffsetY           The Y offset of the image regarding the Attribute.

  @retval EFI_SUCCESS      The image was fetched successfully.
  @retval EFI_NOT_FOUND    The specified image could not be found.
**/
EFI_STATUS
EFIAPI
GetImage (
  IN     EDKII_PLATFORM_LOGO_PROTOCOL          *This,
  IN OUT UINT32                                *Instance,
     OUT EFI_IMAGE_INPUT                       *Image,
     OUT EDKII_PLATFORM_LOGO_DISPLAY_ATTRIBUTE *Attribute,
     OUT INTN                                  *OffsetX,
     OUT INTN                                  *OffsetY
  )
{
  UINT32 Current;
  if (Instance == NULL || Image == NULL ||
      Attribute == NULL || OffsetX == NULL || OffsetY == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Current = *Instance;
  if (Current >= ARRAY_SIZE (mLogos)) {
    return EFI_NOT_FOUND;
  }

  (*Instance)++;
  *Attribute = mLogos[Current].Attribute;
  *OffsetX   = mLogos[Current].OffsetX;
  *OffsetY   = mLogos[Current].OffsetY;
  return mHiiImageEx->GetImageEx (mHiiImageEx, mHiiHandle, mLogos[Current].ImageId, Image);
}

EFI_HII_IMAGE_DECODER_PROTOCOL *
FindDecoder (
  EFI_GUID      *DecoderGuid
  )
{
  EFI_STATUS          Status;
  UINTN               Index;
  UINTN               HandleNum;
  EFI_HANDLE          *Handles;
  EFI_HII_IMAGE_DECODER_PROTOCOL *Decoder = NULL;
  EFI_GUID                       *DecoderNames;
  UINT16                         NumberOfDecoderName;
  UINTN                          DecoderNameIndex;
  
  Status = gBS->LocateHandleBuffer (ByProtocol, &gEfiHiiImageDecoderProtocolGuid, NULL, &HandleNum, &Handles);
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  
  for (Index = 0; Index < HandleNum; Index++) {
    Status = gBS->HandleProtocol (Handles[Index], &gEfiHiiImageDecoderProtocolGuid, (VOID **) &Decoder);
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = Decoder->GetImageDecoderName (Decoder, &DecoderNames, &NumberOfDecoderName);
    if (EFI_ERROR (Status)) {
      continue;
    }
    for (DecoderNameIndex = 0; DecoderNameIndex < NumberOfDecoderName; DecoderNameIndex++) {
      if (CompareGuid (DecoderGuid, &DecoderNames[DecoderNameIndex])) {
        return Decoder;
      }
    }
  }

 return NULL;
}

VOID
EFIAPI
LogoUpdateOnEndOfDxe(
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
) {

  EFI_IMAGE_INPUT     HiiImageInput;
  VOID               *Image;     // PcdLogoDataAddress
  VOID               *ImageFlash;
  UINTN               ImageSize; // PcdLogoDataSize
  BMP_IMAGE_HEADER    *BmpHeader;
  EFI_STATUS          Status;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL  *BltMap;
  UINTN                          GopBltSize;
  UINTN                          PixelHeight;
  UINTN                          PixelWidth;
  EFI_HII_IMAGE_DECODER_PROTOCOL *Decoder;
  EFI_IMAGE_OUTPUT               *ImageOut;
  UINT8                          *PngHead;
  EFI_GRAPHICS_OUTPUT_PROTOCOL             *GraphicsOutput;
  UINTN                                    SizeOfInfo;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION     *Info;
  BOOLEAN                                  IsBmp = FALSE;
  BOOLEAN                                  IsPng = FALSE;
  UINTN                                    Index;
  BOOLEAN                                  IsSetLogo = FALSE;

  Status = gBS->HandleProtocol (
                  gST->ConsoleOutHandle,
                  &gEfiGraphicsOutputProtocolGuid,
                  (VOID**)&GraphicsOutput
                  );
  if (EFI_ERROR (Status)) {
    GraphicsOutput = NULL;
  }

  Image = (VOID *)(UINTN)PcdGet32(PcdLogoDataAddress);
  ImageSize = (UINTN)PcdGet32(PcdLogoDataSize);

  if (ImageSize == 0) {
    //
    // Update Logo from Flash
    //
    ImageFlash = (VOID *)(UINTN)PcdGet32(PcdFlashNvLogoBase);
    ImageSize = (UINTN)PcdGet32(PcdFlashNvLogoSize);
    if (ImageSize == 0) {
      //
      // No Logo exist.
      //
      return;
    }
    Image = AllocatePool(ImageSize);
    CopyMem(Image,ImageFlash,ImageSize);
  }

  BmpHeader = (BMP_IMAGE_HEADER *)Image;
  BltMap     = NULL;
  GopBltSize = 0;
  ImageOut   = NULL;
  PngHead = (UINT8*)Image;

  if (BmpHeader->CharB == 'B' && BmpHeader->CharM == 'M') {
    IsBmp = TRUE;
    //
    // The input image is BMP image, and convert it to GopBlt
    //
    ImageSize = BmpHeader->Size;
    Status = TranslateBmpToGopBlt(BmpHeader, ImageSize, &BltMap, &GopBltSize, &PixelHeight, &PixelWidth);
    DEBUG((EFI_D_INFO, "TranslateBmpToGopBlt %r\n",Status));
    if (!EFI_ERROR (Status)) {
      //
      // prepare HiiImageInput
      //
      HiiImageInput.Flags  = 0;
      HiiImageInput.Width  = (UINT16)PixelWidth;
      HiiImageInput.Height = (UINT16)PixelHeight;
      HiiImageInput.Bitmap = BltMap;
    }
  } else {
    if ((PngHead[0] == 0x89 && PngHead[1] == 0x50 
        && PngHead[2] == 0x4e && PngHead[3] == 0x47
        && PngHead[4] == 0x0d && PngHead[5] == 0x0a
        && PngHead[6] == 0x1a && PngHead[7] == 0x0a)) {
      Decoder = FindDecoder(&gEfiHiiImageDecoderNamePngGuid);
      IsPng = TRUE;
    } else {
      Decoder = FindDecoder(&gEfiHiiImageDecoderNameJpegGuid);
    }
    if (Decoder == NULL){
      Status = EFI_NOT_FOUND;
      DEBUG((EFI_D_INFO, "Decoder not found\n"));
    } else {    
      Status = Decoder->DecodeImage (
        Decoder,
        Image,
        ImageSize,
        &ImageOut,
        FALSE
      );
      DEBUG((EFI_D_INFO, "Decode image %r\n",Status));
    }

    if (!EFI_ERROR (Status)) {
      //
      // Convert ImageOut to HiiImageInput
      //
      HiiImageInput.Flags  = 0;
      HiiImageInput.Width  = ImageOut->Width;
      HiiImageInput.Height = ImageOut->Height;
      HiiImageInput.Bitmap = ImageOut->Image.Bitmap;
    }
  }

  //
  // Set this image
  //
  if (!EFI_ERROR (Status)) {
    if (GraphicsOutput != NULL) {
      Status = GraphicsOutput->QueryMode (
                        GraphicsOutput,
                        GraphicsOutput->Mode->Mode,
                        &SizeOfInfo,
                        &Info
                        );
      if (!EFI_ERROR (Status)) {
        if(((UINT32)HiiImageInput.Width > Info->HorizontalResolution)
        ||((UINT32)HiiImageInput.Height > Info->VerticalResolution)) {
          //
          //The resolution of the image exceeds the resolution of the screen
          //
          PcdSet8S (PcdRecordUpdateLogoState, 4);
        }
      }
    }
    if (PcdGet8(PcdRecordUpdateLogoState) == 0) {
      Status = mHiiImageEx->SetImageEx (mHiiImageEx, mHiiHandle, IMAGE_TOKEN (IMG_LOGO), &HiiImageInput);
      DEBUG((EFI_D_INFO, "SetImage %r\n",Status));
      if(!EFI_ERROR(Status)){
        PcdSet8S (PcdRecordUpdateLogoState, 0x80);
      }
    }
  } else {
    for (Index = 0; Index < 16;Index += 8) {
      if (*(UINT64*)((UINT8*)Image + Index) != 0xFFFFFFFFFFFFFFFF) {
        IsSetLogo = TRUE;
        break;
      }
    }
    if (IsSetLogo) {
      if (IsBmp) {
        //
        //Illegal bmp image
        //
        PcdSet8S (PcdRecordUpdateLogoState, 1);
      } else if (IsPng) {
        //
        //Unsupported PNG image
        //
        PcdSet8S (PcdRecordUpdateLogoState, 2);
      } else {
        //
        //Unsupported Jpg image
        //
        PcdSet8S (PcdRecordUpdateLogoState, 3);
      }
    }
  }

  if (BltMap != NULL) {
    FreePool (BltMap);
  }

  if (ImageOut != NULL) {
    if (ImageOut->Image.Bitmap != NULL) {
      FreePool (ImageOut->Image.Bitmap);
    }
    FreePool (ImageOut);
  }
}
  
EDKII_PLATFORM_LOGO_PROTOCOL mPlatformLogo = {
  GetImage
};

/**
  Entrypoint of this module.

  This function is the entrypoint of this module. It installs the Edkii
  Platform Logo protocol.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
InitializeLogo (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HII_PACKAGE_LIST_HEADER *PackageList;
  EFI_HII_DATABASE_PROTOCOL   *HiiDatabase;
  EFI_HANDLE                  Handle;
  EFI_EVENT                   EndOfDxeEvent;

  Status = gBS->LocateProtocol (
                  &gEfiHiiDatabaseProtocolGuid,
                  NULL,
                  (VOID **) &HiiDatabase
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
                  &gEfiHiiImageExProtocolGuid,
                  NULL,
                  (VOID **) &mHiiImageEx
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Retrieve HII package list from ImageHandle
  //
  Status = gBS->OpenProtocol (
                  ImageHandle,
                  &gEfiHiiPackageListProtocolGuid,
                  (VOID **) &PackageList,
                  ImageHandle,
                  NULL,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Publish HII package list to HII Database.
    //
    Status = HiiDatabase->NewPackageList (
                            HiiDatabase,
                            PackageList,
                            NULL,
                            &mHiiHandle
                            );
    if (EFI_ERROR (Status)) {
      mHiiHandle = NULL;
    }
  } else {
    mHiiHandle = HiiAddPackages (
                    &gEfiCallerIdGuid,
                    ImageHandle,
                    IMAGE_ARRAY_NAME,
                    NULL
                    );
  }

  Status = EFI_NOT_FOUND;
  if (mHiiHandle != NULL) {
    Handle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &Handle,
                    &gEdkiiPlatformLogoProtocolGuid, &mPlatformLogo,
                    NULL
                    );
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    LogoUpdateOnEndOfDxe,
                    NULL,
                    &gEventAfterConnectConsoleDeviceGuid,
                    &EndOfDxeEvent
                    );
    ASSERT_EFI_ERROR (Status);
    PcdSet8S(PcdRecordUpdateLogoState, 0);
  }

  return Status;
}
