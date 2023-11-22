/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Byosoft or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/** @file

  This code supports a the private implementation
  of the JPEG Decoder protocol

Copyright (c) 1999 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.



**/

#include "JfifDecode.h"
#include "McuDecode.h"

#define MAX_X 1024 // default max X supported resolution
#define MAX_Y 768  // default max Y supported resolution

/**
  Find the max supported resolution by GOP protocol.

  @param MaxHorizontalResolution   Max supported Horizontal Resolution.
  @param MaxVerticalResolution     Max supported Vertical Resolution.

**/
VOID
GetGraphicConsoleResolution (
  OUT UINT32 *MaxHorizontalResolution,
  OUT UINT32 *MaxVerticalResolution
  )
{
  UINT32     ModeNumber;
  EFI_STATUS Status;
  UINTN      SizeOfInfo;
  UINT32     MaxMode;
  EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *Info;
  EFI_GRAPHICS_OUTPUT_PROTOCOL         *GraphicsOutput;

  *MaxHorizontalResolution = 0;
  *MaxVerticalResolution   = 0;
  Status = gBS->HandleProtocol (gST->ConsoleOutHandle, &gEfiGraphicsOutputProtocolGuid, (VOID **) &GraphicsOutput);
  if (EFI_ERROR (Status)) {
    return ;
  }

  MaxMode = GraphicsOutput->Mode->MaxMode;

  for (ModeNumber = 0; ModeNumber < MaxMode; ModeNumber++) {
    Status = GraphicsOutput->QueryMode (
                       GraphicsOutput,
                       ModeNumber,
                       &SizeOfInfo,
                       &Info
                       );
    if (!EFI_ERROR (Status)) {
      if (Info->HorizontalResolution > *MaxHorizontalResolution) {
        *MaxHorizontalResolution = Info->HorizontalResolution;
        *MaxVerticalResolution   = Info->VerticalResolution;
      }
    }
  }
}

/**
  Decodes a JFIF image into a GOP formatted image,
  and returns the decoded image, width and height.

  Caution: This file requires additional review when modified.
  This service will have external input - JPG image.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.

  @param This            Protocol instance structure
  @param ImageData       The data of the JFIF image, which will be decoded
  @param ImageDataSize   The size in bytes of ImageData
  @param DecodedData     The decoded data, this output parameter contains
                         a newly allocated memory space, and it is the
                         caller's responsibility to free this memory buffer.
  @param DecodedDataSize The size in bytes of DecodedData
  @param Height          The height of the image displaying
  @param Width           The width of the image displaying
  @param DecoderStatus   The status of the decoding progress.

  @retval EFI_SUCCESS            The JFIF image is decoded successfully.
  @retval EFI_INVALID_PARAMETER  Either one of ImageData, Width, Height, DecodedDataSize
                                 and DecoderStatus is NULL, or ImageDataSize is zero.
  @retval EFI_OUT_OF_RESOURCES   The memory for DecodedData could not be allocated.
  @retval EFI_UNSUPPORTED        The JFIF image can not be decoded, and the detail error info
                                 will be returned by the output parameter "decoderStatus".
**/
EFI_STATUS
EFIAPI
JpegDecoderDecodeImage (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width,
     OUT EFI_JPEG_DECODER_STATUS       *DecoderStatus
  )
{
  BOOLEAN               IntervalFlag;
  BOOLEAN               IsEnd;
  UINT16                McuCount;
  INT16                 McuSrcBuff[10*64];
  INT16                 McuDstBuff[12*64];
  UINT16                CurHPixel;
  UINT16                CurVLine;
  UINT32                MaxHorizontalResolution;
  UINT32                MaxVerticalResolution;
  EFI_STATUS            Status;

  if (ImageData == NULL
       || ImageDataSize == 0
       || DecodedDataSize == NULL
       || Height == NULL
       || Width == NULL
       || DecoderStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  *DecoderStatus = InitJfifData (ImageData, ImageDataSize);
  if (*DecoderStatus != EFI_JPEG_DECODE_SUCCESS) {
    return EFI_UNSUPPORTED;
  }
  *DecoderStatus = InitDecoderData();
  if (*DecoderStatus != EFI_JPEG_DECODE_SUCCESS) {
    return EFI_UNSUPPORTED;
  }

  *Width = mJfifData.Sof0Data.Width;
  *Height = mJfifData.Sof0Data.Height;
  if ((mJfifData.Sof0Data.Width == 0) || (mJfifData.Sof0Data.Height == 0)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Try to get current Graphics console resolution.
  //
  GetGraphicConsoleResolution (&MaxHorizontalResolution, &MaxVerticalResolution);

  DEBUG ((DEBUG_INFO, "GOP MAX Resolution X is %x and Y is %x\n", MaxHorizontalResolution, MaxVerticalResolution));
  DEBUG ((DEBUG_INFO, "Current Picture X Resolution is %x and Y is %x\n", *Width, *Height));
  //
  // Check the supported height and width size before allocating memory
  //
  if (MaxHorizontalResolution == 0) {
    if ((*Width > MAX_X) || (*Height > MAX_Y)) {
      return EFI_UNSUPPORTED;
    }
  } else {
    if ((*Width > MaxHorizontalResolution) || (*Height > MaxVerticalResolution)) {
      return EFI_UNSUPPORTED;
    }
  }
  //
  // Ensure Width * Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) doesn't overflow.
  //
  if (mJfifData.Sof0Data.Width > DivU64x32 ((UINTN) ~0, mJfifData.Sof0Data.Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
    return EFI_UNSUPPORTED;
  }

  *DecodedDataSize =
     mJfifData.Sof0Data.Width * mJfifData.Sof0Data.Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL);
  *DecodedData = AllocatePool(*DecodedDataSize);
  if (*DecodedData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = EFI_SUCCESS;
  CurHPixel = 0;
  CurVLine = 0;
  IntervalFlag = FALSE;

  IsEnd = FALSE;
  McuCount = 0;
  while (!IsEnd) {
    SetMem(&McuDstBuff, sizeof(McuDstBuff), 0);
    if ((Status = McuRetrieve((INT16 *)&McuSrcBuff, IntervalFlag, &IsEnd, DecoderStatus)) != EFI_SUCCESS) {
      break;
    }
    McuCount++;
    if ((mJfifData.McuRestart)&&(McuCount % mJfifData.McuRestart==0)) {
      IntervalFlag = TRUE;
    } else {
      IntervalFlag = FALSE;
    }
    McuDecode ((INT16 *)&McuSrcBuff, (INT16 *)&McuDstBuff);
    StoreMcuToBltBuffer ((INT16 *)&McuDstBuff, DecodedData, &CurHPixel, &CurVLine);
  }

  //
  // Correct return Status if Image is terminator with 0xD9FF
  //
  if (IsEnd && (*(UINT16 *) mDecoderData.ImagePtr == 0xD9FF)) {
    Status = EFI_SUCCESS;
    *DecoderStatus = EFI_JPEG_DECODE_SUCCESS;
  }

  //
  // Free memory if the image data can't be decoded.
  //
  if (EFI_ERROR (Status)) {
    FreePool (*DecodedData);
    *DecodedData = NULL;
  }

  return Status;
}


/**
  Get a special Marker info of the JFIF image

  Caution: This file requires additional review when modified.
  This service will have external input - JPG image.
  This external input must be validated carefully to avoid security issue like
  buffer overflow, integer overflow.

  @param This            Protocol instance structure
  @param Start           The start of the JFIF image,
                         or the output Next parameter from a previous call.
  @param End             The end of the JFIF image.
  @param MarkerType      The type of the marker in the JFIF image
  @param MarkerData      The pointer of the marker specified by the special MarkerType in the JFIF image.
  @param DataSize        The size in bytes of MarkerData (with the marker bytes and length bytes).
  @param Next            The next pointer following the "MarkerType" marker,
                         it is next marker pointer, or the compressed data pointer after SOS marker.

  @retval EFI_SUCCESS            The marker information is retrieved successfully.
  @retval EFI_INVALID_PARAMETER  Either one of Start, End and DataSize is NULL,
                                 or End is less than Start.
  @retval EFI_NOT_FOUND          The marker can not be found in the JFIF image.

**/
EFI_STATUS
EFIAPI
JpegDecoderGetMarkerData (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *Start,
  IN     UINT8                         *End,
  IN OUT EFI_JPEG_MARKER_TYPE          *MarkerType,
     OUT UINT8                         **MarkerData,
     OUT UINT32                        *DataSize,
     OUT UINT8                         **Next  OPTIONAL
  )
{
  UINT8  *ImagePtr;

  ImagePtr = Start;
  if (Start == NULL
       || End == NULL
       || End < Start
       || DataSize == NULL
       || MarkerType == NULL
       || MarkerData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  while (ImagePtr + 2 < End) {
    //
    // The ImagePtr is not a Marker's pointer
    //
    if (*ImagePtr != 0xFF || (*ImagePtr == 0xFF && *(ImagePtr + 1) == 0x00)) {
      ImagePtr++;
      continue;
    }

    if (*(ImagePtr + 2) == 0xFF) {
      //
      // The ImagePtr is a marker's pointer, and this marker does not have data info
      //
      *DataSize = 2;
    } else {
      if (ImagePtr + 3 >= End) {
        //
        // Data access out of resource
        //
        break;
      }
      *DataSize = *(ImagePtr + 2);
      *DataSize = ((*DataSize) << 8) + *(ImagePtr + 3) + 2;
    }
    if (*MarkerType == JPEG_ANY) {
      //
      //Find the first marker following the Start pointer which has data info.
      //
      *MarkerType = *(ImagePtr + 1);
      *MarkerData = ImagePtr;
      if (Next != NULL) {
        *Next = ImagePtr + *DataSize;
      }
      return EFI_SUCCESS;
    } else if ( *(ImagePtr + 1) == *MarkerType) {
      //
      // The ImagePtr is a marker's pointer, and this marker have info
      //
      *MarkerData = ImagePtr;
      if (Next != NULL) {
        *Next = ImagePtr + *DataSize;
      }
      return EFI_SUCCESS;
    } else {
      ImagePtr += *DataSize;
    }
  }
  *MarkerData = NULL;
  *DataSize = 0;
  if (Next != NULL) {
    *Next = NULL;
  }
  return EFI_NOT_FOUND;
}

EFI_JPEG_DECODER_PROTOCOL JpegDecoderProtocol = {
  JpegDecoderDecodeImage,
  JpegDecoderGetMarkerData
};

EFI_GUID mSupportDecoderList[] = {
  EFI_HII_IMAGE_DECODER_NAME_JPEG_GUID
};

/**
  There could be more than one EFI_HII_IMAGE_DECODER_PROTOCOL instances installed
  in the system for different image formats. This function returns the decoder
  name which callers can use to find the proper image decoder for the image. It
  is possible to support multiple image formats in one EFI_HII_IMAGE_DECODER_PROTOCOL.
  The capability of the supported image formats is returned in DecoderName and
  NumberOfDecoderName.

  @param This                    EFI_HII_IMAGE_DECODER_PROTOCOL instance.
  @param DecoderName             Pointer to a dimension to retrieve the decoder
                                 names in EFI_GUID format. The number of the
                                 decoder names is returned in NumberOfDecoderName.
  @param NumberofDecoderName     Pointer to retrieve the number of decoders which
                                 supported by this decoder driver.

  @retval EFI_SUCCESS            Get decoder name success.
  @retval EFI_UNSUPPORTED        Get decoder name fail.

**/
EFI_STATUS
EFIAPI
JpegGetImageDecoderName (
  IN      EFI_HII_IMAGE_DECODER_PROTOCOL   *This,
  IN OUT  EFI_GUID                         **DecoderName,
  IN OUT  UINT16                           *NumberofDecoderName
  )
{
  *DecoderName = mSupportDecoderList;
  *NumberofDecoderName = sizeof (mSupportDecoderList) / sizeof (EFI_GUID);
  return EFI_SUCCESS;
}

/**
  This function returns the image information of the given image raw data. This
  function first checks whether the image raw data is supported by this decoder
  or not. This function may go through the first few bytes in the image raw data
  for the specific data structure or the image signature. If the image is not supported
  by this image decoder, this function returns EFI_UNSUPPORTED to the caller.
  Otherwise, this function returns the proper image information to the caller.
  It is the caller's responsibility to free the ImageInfo.

  @param This                    EFI_HII_IMAGE_DECODER_PROTOCOL instance.
  @param Image                   Pointer to the image raw data.
  @param SizeOfImage             Size of the entire image raw data.
  @param ImageInfo               Pointer to receive EFI_HII_IMAGE_DECODER_IMAGE_INFO_HEADER.

  @retval EFI_SUCCESS            Get image info success.
  @retval EFI_UNSUPPORTED        Unsupported format of image.
  @retval EFI_INVALID_PARAMETER  Incorrect parameter.
  @retval EFI_BAD_BUFFER_SIZE    Not enough memory.

**/
EFI_STATUS
EFIAPI
JpegGetImageInfo (
  IN      EFI_HII_IMAGE_DECODER_PROTOCOL           *This,
  IN      VOID                                     *Image,
  IN      UINTN                                    SizeOfImage,
  IN OUT  EFI_HII_IMAGE_DECODER_IMAGE_INFO_HEADER  **ImageInfo
  )
{
  EFI_JPEG_DECODER_STATUS           DecoderStatus;
  UINT16                            Width;
  UINT16                            Height;
  EFI_HII_IMAGE_DECODER_JPEG_INFO   *JpegInfo;

  if (Image == NULL
       || SizeOfImage == 0
       || ImageInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DecoderStatus = InitJfifData (Image, SizeOfImage);
  if (DecoderStatus != EFI_JPEG_DECODE_SUCCESS) {
    return EFI_UNSUPPORTED;
  }
  DecoderStatus = InitDecoderData();
  if (DecoderStatus != EFI_JPEG_DECODE_SUCCESS) {
    return EFI_UNSUPPORTED;
  }

  Width = mJfifData.Sof0Data.Width;
  Height = mJfifData.Sof0Data.Height;
  if ((mJfifData.Sof0Data.Width == 0) || (mJfifData.Sof0Data.Height == 0)) {
    return EFI_UNSUPPORTED;
  }

  //
  // Ensure Width * Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL) doesn't overflow.
  //
  if (mJfifData.Sof0Data.Width > DivU64x32 ((UINTN) ~0, mJfifData.Sof0Data.Height * sizeof (EFI_GRAPHICS_OUTPUT_BLT_PIXEL))) {
    return EFI_UNSUPPORTED;
  }

  JpegInfo = AllocatePool (sizeof (EFI_HII_IMAGE_DECODER_JPEG_INFO));
  if (JpegInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyGuid (&JpegInfo->Header.DecoderName, &gEfiHiiImageDecoderNameJpegGuid);
  JpegInfo->Header.ImageInfoSize = sizeof (EFI_HII_IMAGE_DECODER_JPEG_INFO);
  JpegInfo->Header.ImageWidth    = Width;
  JpegInfo->Header.ImageHeight   = Height;
  JpegInfo->Header.ColorType     = EFI_HII_IMAGE_DECODER_COLOR_TYPE_RGB;
  JpegInfo->Header.ColorDepthInBits = 8;
  JpegInfo->ScanType = 0;

  *ImageInfo = (EFI_HII_IMAGE_DECODER_IMAGE_INFO_HEADER *) JpegInfo;
  return EFI_SUCCESS;
}

/**
  This function decodes the image which the image type of this image is supported
  by this EFI_HII_IMAGE_DECODER_PROTOCOL. If **Bitmap is not NULL, the caller intends
  to put the image in the given image buffer. That allows the caller to put an
  image overlap on the original image. The transparency is handled by the image
  decoder because the transparency capability depends on the image format. Callers
  can set Transparent to FALSE to force disabling the transparency process on the
  image. Forcing Transparent to FALSE may also improve the performance of the image
  decoding because the image decoder can skip the transparency processing.  If **Bitmap
  is NULL, the image decoder allocates the memory buffer for the EFI_IMAGE_OUTPUT
  and decodes the image to the image buffer. It is the caller?s responsibility to
  free the memory for EFI_IMAGE_OUTPUT. Image decoder doesn?t have to handle the
  transparency in this case because there is no background image given by the caller.
  The background color in this case is all black (#00000000).

  @param This                    EFI_HII_IMAGE_DECODER_PROTOCOL instance.
  @param Image                   Pointer to the image raw data.
  @param ImageRawDataSize        Size of the entire image raw data.
  @param BltMap                  EFI_IMAGE_OUTPUT to receive the image or overlap
                                 the image on the original buffer.
  @param Transparent             BOOLEAN value indicates whether the image decoder
                                 has to handle the transparent image or not.


  @retval EFI_SUCCESS            Image decode success.
  @retval EFI_UNSUPPORTED        Unsupported format of image.
  @retval EFI_INVALID_PARAMETER  Incorrect parameter.
  @retval EFI_BAD_BUFFER_SIZE    Not enough memory.

**/
EFI_STATUS
EFIAPI
JpegDecodeImage (
  IN      EFI_HII_IMAGE_DECODER_PROTOCOL   *This,
  IN      VOID                              *Image,
  IN      UINTN                             ImageRawDataSize,
  IN OUT  EFI_IMAGE_OUTPUT                  **BltMap OPTIONAL,
  IN      BOOLEAN                           Transparent
  )
{
  EFI_STATUS                        Status;
  EFI_JPEG_DECODER_STATUS           DecoderStatus;
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL     *GopBlt;
  UINTN                             GopBltSize;
  UINTN                             PixelWidth;
  UINTN                             PixelHeight;

  if (BltMap == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  Status = JpegDecoderDecodeImage (
              &JpegDecoderProtocol,
              Image,
              ImageRawDataSize,
              (UINT8 **) &GopBlt,
              &GopBltSize,
              &PixelHeight,
              &PixelWidth,
              &DecoderStatus
              );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (DecoderStatus != EFI_JPEG_DECODE_SUCCESS) {
    return EFI_UNSUPPORTED;
  }

  if (*BltMap == NULL) {
    *BltMap = AllocatePool (sizeof (EFI_IMAGE_OUTPUT));
    if (*BltMap == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
  }
  (*BltMap)->Width  = (UINT16) PixelWidth;
  (*BltMap)->Height = (UINT16) PixelHeight;
  (*BltMap)->Image.Bitmap = GopBlt;

  return EFI_SUCCESS;
}

EFI_HII_IMAGE_DECODER_PROTOCOL HiiJpegDecoderProtocol = {
  JpegGetImageDecoderName,
  JpegGetImageInfo,
  JpegDecodeImage
};

/**
  Install Driver to produce JPEG Decoder protocol.

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval EFI_SUCCESS JPEG Decoder protocol installed
  @retval Other       No protocol installed, unload driver.

**/
EFI_STATUS
EFIAPI
JpegDecoderInstall (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS    Status;
  EFI_HANDLE    Handle;

  Handle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiJpegDecoderProtocolGuid,
                  &JpegDecoderProtocol,
                  &gEfiHiiImageDecoderProtocolGuid,
                  &HiiJpegDecoderProtocol,
                  NULL
                  );
  return Status;
}
