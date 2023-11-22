/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Byosoft or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/** @file
   JPEG Decoder Protocol that provides services to
   convert JPEG image to GOP BLT buffer.

Copyright (c) 1999 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Byosoft Corporation.

**/

#ifndef __EFI_JPEG_DECODER_H__
#define __EFI_JPEG_DECODER_H__

//
// Global ID for the JPEG Decoder Protocol
//
#define EFI_JPEG_DECODER_PROTOCOL_GUID \
   { 0xa9396a81, 0x6231, 0x4dd7, { 0xbd, 0x9b, 0x2e, 0x6b, 0xf7, 0xec, 0x73, 0xc2 }}

//
// JPEG decoder status for the JPEG Decoder Protocol
//
#define EFI_JPEG_DECODE_SUCCESS          0
#define EFI_JPEG_INVALID_PARAMETER       1
#define EFI_JPEG_IMAGE_UNSUPPORTED       2
#define EFI_JPEG_QUANTIZATIONTABLE_ERROR 3
#define EFI_JPEG_HUFFMANTABLE_ERROR      4
#define EFI_JPEG_SOS_ERROR               5
#define EFI_JPEG_SOF_ERROR               6
#define EFI_JPEG_DECODEDATA_ERROR        7
#define EFI_JPEG_DECODEHT_ERROR          8
#define EFI_JPEG_DECODERST_ERROR         9
#define EFI_JPEG_DECODEAC_ERROR          10

typedef UINTN EFI_JPEG_DECODER_STATUS;

//
// JPEG marker type for the JPEG Decoder Protocol
//
#define JPEG_ANY   0x0
#define JPEG_SOF0  0xc0

#define JPEG_DHT   0xc4

#define JPEG_RST0  0xd0
#define JPEG_RST1  0xd1
#define JPEG_RST2  0xd2
#define JPEG_RST3  0xd3
#define JPEG_RST4  0xd4
#define JPEG_RST5  0xd5
#define JPEG_RST6  0xd6
#define JPEG_RST7  0xd7

#define JPEG_SOI   0xd8
#define JPEG_EOI   0xd9
#define JPEG_SOS   0xda
#define JPEG_DQT   0xdb
#define JPEG_DNL   0xdc
#define JPEG_DRI   0xdd

#define JPEG_APP0  0xe0
#define JPEG_APP1  0xe1
#define JPEG_APP2  0xe2
#define JPEG_APP3  0xe3
#define JPEG_APP4  0xe4
#define JPEG_APP5  0xe5
#define JPEG_APP6  0xe6
#define JPEG_APP7  0xe7
#define JPEG_APP8  0xe8
#define JPEG_APP9  0xe9
#define JPEG_APP10 0xea
#define JPEG_APP11 0xeb
#define JPEG_APP12 0xec
#define JPEG_APP13 0xed
#define JPEG_APP14 0xee
#define JPEG_APP15 0xef

#define JPEG_COM   0xfe

typedef UINTN EFI_JPEG_MARKER_TYPE;

//
// Forward reference for pure ANSI compatability
//
typedef struct _EFI_JPEG_DECODER_PROTOCOL EFI_JPEG_DECODER_PROTOCOL;

/**
  Decodes a JFIF image into a GOP formatted image,
  and returns the decoded image, width and height

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
typedef
EFI_STATUS
(EFIAPI *EFI_JPEG_DECODER_DECODE_IMAGE) (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize,
     OUT UINT8                         **DecodedData,
     OUT UINTN                         *DecodedDataSize,
     OUT UINTN                         *Height,
     OUT UINTN                         *Width,
     OUT EFI_JPEG_DECODER_STATUS       *DecoderStatus
  );

/**
  Get a special Marker info of the JFIF image

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
typedef
EFI_STATUS
(EFIAPI *EFI_JPEG_DECODER_GET_MARKER_DATA) (
  IN     EFI_JPEG_DECODER_PROTOCOL     *This,
  IN     UINT8                         *Start,
  IN     UINT8                         *End,
  IN OUT EFI_JPEG_MARKER_TYPE          *MarkerType,
     OUT UINT8                         **MarkerData,
     OUT UINT32                        *DataSize,
     OUT UINT8                         **Next  OPTIONAL
  );

//
// Interface structure for the JPEG Decoder Protocol
//
struct _EFI_JPEG_DECODER_PROTOCOL {
  EFI_JPEG_DECODER_DECODE_IMAGE         DecodeImage;
  EFI_JPEG_DECODER_GET_MARKER_DATA      GetMarkerData;
};


extern EFI_GUID gEfiJpegDecoderProtocolGuid;

#endif
