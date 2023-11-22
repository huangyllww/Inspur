/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Byosoft or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/** @file
  This code decode the JFIF image and get the useful information.

Copyright (c) 1999 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.



**/

#ifndef _JFIF_DECODE_H_
#define _JFIF_DECODE_H_

#include <PiDxe.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/JpegDecoder.h>
#include <Protocol/HiiImageDecoder.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>

typedef struct _EFI_JPEG_SAMPLE_DATA {
  UINT8                       Hi;
  UINT8                       Vi;
  UINT8                       Blocks;
  UINT8                       QuanTable;
} EFI_JPEG_SAMPLE_DATA;

typedef struct _EFI_JPEG_SOF0_DATA {
  UINT8                       *Ptr;
  UINT8                       Components;
  UINT16                      Width;
  UINT16                      Height;
  EFI_JPEG_SAMPLE_DATA        Samples[3];
} EFI_JPEG_SOF0_DATA;

typedef struct _EFI_JPEG_HUFFMAN_TABLE {
  UINT8   *Ptr;
  UINT8   Number;           //Huffman table number, [0,3]
  UINT8   Type;             //Table class: 0 = DC table, 1 = AC table
  UINT16  MinCode[17];      //Table with minimum value of Huffman code for each layer of the Huffman tree
  UINT16  MaxCode[17];      //Table with maximum value of Huffman code for each layer of the Huffman tree (-1, if none)
  UINT8   Codes[17];        //Table with the number of codes in each layer
  UINT8   FirstCode[17];    //Table with index of 1st symbol of each layer in Huffmanval[]
  UINT8   HuffmanVal[256];  //the length of coding value, or the layers of the Huffman tree
} EFI_JPEG_HUFFMAN_TABLE;

typedef struct {
  UINT8                       DcAcHTIndex;
  UINT8                       QTIndex;
  UINT8                       HiViIndex;
  INT16                       *DcValPtr;
} EFI_JPEG_DECODER_BLOCK_DATA;

typedef struct {
  UINT8                       *ImagePtr;
  UINT8                       *ImageEnd;
  UINT8                       CurByte;
  UINT8                       BitPos;
  UINT8                       Blocks;
  INT16                       DcVal[3];
  EFI_JPEG_DECODER_BLOCK_DATA BlocksInMcu[10];
} EFI_JPEG_DECODER_DATA;


typedef struct {
  UINT8                       *ImageEnd;
  UINT8                       *SoiPtr;
  UINT8                       *App0Ptr;
  UINT8                       *SosPtr;
  UINT8                       *ComPtr;
  UINT8                       *DnlPtr;
  UINT16                      McuRestart;
  UINT8                       *DqtPtr[4];
  UINT8                       HTMaxIndex;
  UINT8                       QTMaxIndex;
  EFI_JPEG_SOF0_DATA          Sof0Data;
  EFI_JPEG_HUFFMAN_TABLE      HuffTable[8];
} EFI_JPEG_JFIF_DATA;

/**
  Get the SOF0(Start Of Frame, Baseline sequential DCT mode) marker's data.
  And initialize the relative global parameters.


  @param ImagePtr        The pointer of the SOF0 marker

  @retval EFI_JPEG_DECODE_SUCCESS    Get the markers data successfully.
  @retval EFI_JPEG_SOF_ERROR         Not have the three components info,
                                     The Sample precision is not 8 bits,
                                     Does not have a image components in the frame,
                                     The sample rate Hi, Vi is neither 1 nor 2,
                                     The blocks number in a MCU is more than 10.

**/
EFI_JPEG_DECODER_STATUS
GetSof0Data (
  IN     UINT8                        *ImagePtr
  );

/**
  Get the HuffmanTable data. And initialize the relative global parameters.


  @param ImagePtr        The pointer of the DHT (Definition of Huffman Table) marker

  @retval EFI_JPEG_DECODE_SUCCESS           Get the Huffman table data successfully.
  @retval EFI_JPEG_HUFFMANTABLE_ERROR       The data of Huffman table is error.

**/
EFI_JPEG_DECODER_STATUS
GetHuffmanTable (
  IN     UINT8                        *ImagePtr
  );

/**
  Get the important marker data, and store them into the global parameters,
  which will be used in the decoding.


  @param ImageData       Pointer to JFIF image buffer.
  @param ImageDataSize   The length in byte of the JFIF image

  @retval EFI_JPEG_DECODE_SUCCESS           Get the markers data successfully.
  @retval EFI_JPEG_INVALID_PARAMETER        Not have the start marker of SOI,
  @retval EFI_JPEG_SOF_ERROR                The data of SOF is error,
  @retval EFI_JPEG_SOS_ERROR                The data of SOS is error,
  @retval EFI_JPEG_QUANTIZATIONTABLE_ERROR  The data of Quantization table is error,
  @retval EFI_JPEG_HUFFMANTABLE_ERROR       The data of Huffman table is error.

**/
EFI_JPEG_DECODER_STATUS
InitJfifData (
  IN     UINT8                         *ImageData,
  IN     UINTN                         ImageDataSize
  );

/**
  Using the global parameter mJfifData to initialize the global parameter of mDecoderData.
  Check whether mJfifData is valid or not.

  @retval EFI_JPEG_DECODE_SUCCESS           Initialize mDecoderData successfully.
  @retval EFI_JPEG_INVALID_PARAMETER        The data is invalid.
  @retval EFI_JPEG_SOF_ERROR                The data of SOF is error,
  @retval EFI_JPEG_SOS_ERROR                The data of SOS is error,
  @retval EFI_JPEG_QUANTIZATIONTABLE_ERROR  The data of Quantization table is error,
  @retval EFI_JPEG_HUFFMANTABLE_ERROR       The data of Huffman table is error.
**/
EFI_JPEG_DECODER_STATUS
InitDecoderData (
  );

/**
  Read a byte from the mDecoderData.ImagePtr to the mDecoderData.CurByte,

  @retval TRUE       The mDecoderData.CurByte is image end.
  @retval FALSE      The mDecoderData.CurByte is not image end.

**/
BOOLEAN
ReadByte (
  );

extern EFI_JPEG_JFIF_DATA      mJfifData;
extern EFI_JPEG_DECODER_DATA   mDecoderData;
#endif
