/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Byosoft or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/** @file
  This code decode the JFIF image and get the useful Datamation of JFIF image.

Copyright (c) 1999 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.



**/

#include "JfifDecode.h"

EFI_JPEG_JFIF_DATA      mJfifData;
EFI_JPEG_DECODER_DATA   mDecoderData;


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
  )
{
  UINT8       Index;
  UINT8       Blocks;
  //
  // ImagePtr[4]: sample precision, baseline sequential just support 8 bits.
  // ImagePtr[9]: number of image components in frame.
  //
  if (ImagePtr + 9 >= mJfifData.ImageEnd || ImagePtr[4] != 8 || ImagePtr[9] == 0 || ImagePtr[9] > 3) {
    return EFI_JPEG_SOF_ERROR;
  }

  mJfifData.Sof0Data.Height = (ImagePtr[5] << 8) + ImagePtr[6];
  mJfifData.Sof0Data.Width = (ImagePtr[7] << 8) + ImagePtr[8];
  mJfifData.Sof0Data.Components = ImagePtr[9];

  Blocks = 0;
  for (Index = 0; Index < ImagePtr[9]; Index++) {
    if ((ImagePtr + 11 + Index * 3 + 1) >= mJfifData.ImageEnd) {
      //
      // Data access out of resource.
      //
      return EFI_JPEG_SOF_ERROR;
    }
    mJfifData.Sof0Data.Samples[Index].Hi = ImagePtr[11 + Index * 3] >> 4;
    mJfifData.Sof0Data.Samples[Index].Vi = ImagePtr[11 + Index * 3] & 0x0F;
    mJfifData.Sof0Data.Samples[Index].Blocks =
      mJfifData.Sof0Data.Samples[Index].Hi * mJfifData.Sof0Data.Samples[Index].Vi;
    mJfifData.Sof0Data.Samples[Index].QuanTable = ImagePtr[11 + Index * 3 + 1];

    //
    // Just support 4:1:1,4:2:2, 4:2:1, 1:1:1, 2:1:1
    //
    if (mJfifData.Sof0Data.Samples[Index].Blocks != 1
         && mJfifData.Sof0Data.Samples[Index].Blocks != 2
         && mJfifData.Sof0Data.Samples[Index].Blocks != 4) {
      return EFI_JPEG_SOF_ERROR;
    }
    Blocks = Blocks + mJfifData.Sof0Data.Samples[Index].Blocks;
  }
  //
  // In a MCU, the blocks' number must not more than 10.
  //
  if (Blocks > 10) {
    return EFI_JPEG_SOF_ERROR;
  }
  return EFI_JPEG_DECODE_SUCCESS;
}

/**
  Get the HuffmanTable data. And initialize the relative global parameters.


  @param ImagePtr        The pointer of the DHT (Definition of Huffman Table) marker

  @retval EFI_JPEG_DECODE_SUCCESS           Get the Huffman table data successfully.
  @retval EFI_JPEG_HUFFMANTABLE_ERROR       The data of Huffman table is error.

**/
EFI_JPEG_DECODER_STATUS
GetHuffmanTable (
  IN     UINT8                        *ImagePtr
  )
{
  UINT8    Index1;
  UINT8    Index2;
  UINT8    TableIndex;
  UINT8    ValIndex;
  UINT8    *CodesPtr;
  UINT8    *ValPtr;
  UINT8    *TablePtr;
  BOOLEAN  IsZero;
  UINT16   HuffmanCode;

  TablePtr = ImagePtr + 4;
  while(TablePtr < mJfifData.ImageEnd && TablePtr < (ImagePtr + (ImagePtr[2] << 8) + ImagePtr[3])) {
    TableIndex = (TablePtr[0] >> 3) + (TablePtr[0] & 0x0F);
    if (TableIndex > 3) {
      return EFI_JPEG_HUFFMANTABLE_ERROR;
    }

    mJfifData.HuffTable[TableIndex].Ptr = ImagePtr;
    mJfifData.HuffTable[TableIndex].Number = TablePtr[0] & 0x0F;
    mJfifData.HuffTable[TableIndex].Type = TablePtr[0] >> 4;

    //
    //Initial the layer's ptr of the Huffman tree, CodesPtr[1] is the first Huffman tree's layer.
    //  then the index of the layer is just the bits' number of the layer's huffman code.
    //Initial  the value's ptr of Huffman tree, ValPtr[0] is the first Huffman tree's value
    //
    IsZero = TRUE;
    ValIndex = 0;
    HuffmanCode = 0;
    CodesPtr = TablePtr;
    ValPtr = TablePtr + 17;
    for (Index1 = 1; Index1 < 17; Index1++) {
      mJfifData.HuffTable[TableIndex].Codes[Index1] = CodesPtr[Index1];
    }
    for (Index1 = 1; Index1 < 17; Index1++) {
      if (IsZero && CodesPtr[Index1] == 0) {
        continue;
      } else if (IsZero) {
        IsZero = FALSE;
      }
      HuffmanCode = HuffmanCode << 1;
      mJfifData.HuffTable[TableIndex].MinCode[Index1] = HuffmanCode;
      for ( Index2 = 0; Index2 < CodesPtr[Index1]; Index2++, HuffmanCode++) {
        mJfifData.HuffTable[TableIndex].HuffmanVal[ValIndex] = ValPtr[ValIndex];
        ValIndex++;
      }
      mJfifData.HuffTable[TableIndex].MaxCode[Index1] =
        mJfifData.HuffTable[TableIndex].MinCode[Index1] + CodesPtr[Index1] - 1;
      mJfifData.HuffTable[TableIndex].FirstCode[Index1] = ValIndex - CodesPtr[Index1];
    }
    TablePtr = ValPtr + ValIndex;
  }
  return EFI_JPEG_DECODE_SUCCESS;
}

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
  )
{
  EFI_JPEG_DECODER_STATUS    DecodeStatus;
  UINT32                     Index;
  UINT8                      DqtCount;
  UINT8                      *TempPtr;
  UINT8                      *ImagePtr;

  Index = 0;
  ImagePtr  = ImageData;

  SetMem (&mJfifData, sizeof(mJfifData), 0);
  if (ImagePtr[0] != 0xFF || ImagePtr[1] != (UINT8) JPEG_SOI) {
    return EFI_JPEG_INVALID_PARAMETER;
  } else {
    mJfifData.SoiPtr = ImagePtr;
    ImagePtr += 2;
  }
  mJfifData.ImageEnd = ImageData + ImageDataSize;

  //
  // Initialize the value of mJfifData
  //

  //
  // ImagePtr[0], ImagePtr[1], ImagePtr[2], ImagePtr[3] is used in each loop.
  // So, check ImagePtr + 3 < mJfifData.ImageEnd
  //
  while (ImagePtr + 3 < mJfifData.ImageEnd) {
    if (ImagePtr[0] != 0xFF ||(ImagePtr[0] == 0xFF && ImagePtr[1] == 0x00)) {
      ImagePtr++;
      continue;
    }
    //
    // Check this marker is the supporting SOFn (n>0) or not
    //
    if (ImagePtr[1] > 0xc0 && ImagePtr[1] != 0xc4 && ImagePtr[1] <= 0xcf) {
      return EFI_JPEG_SOF_ERROR;
    }

    switch (ImagePtr[1]) {
    case JPEG_SOF0:
      mJfifData.Sof0Data.Ptr = ImagePtr;
      DecodeStatus = GetSof0Data (ImagePtr);
      if (DecodeStatus != EFI_JPEG_DECODE_SUCCESS) {
        return DecodeStatus;
      }
      break;

    case JPEG_SOS:
      mJfifData.SosPtr = ImagePtr;
      //
      // ImagePtr[4]: interleaving, have three component(Y,Cb,Cr) in the scan or grayscale
      // ImagePtr[Index - 1]: Ss(Start of spectral or predictive selection)
      // ImagePtr[Index]: Se(End of spectral selection)
      // ImagePtr[Index + 1]: Ah:Al(high and low of Successive approximation bit position)
      //
      Index = ImagePtr[2];
      Index = (Index << 8) + ImagePtr[3];
      if (ImagePtr + 4 >= mJfifData.ImageEnd
            || ImagePtr + Index + 1 >= mJfifData.ImageEnd
            || (ImagePtr[4] != 3 && ImagePtr[4] != 1)
            || Index < 1
            || ImagePtr[Index - 1] != 0
            || ImagePtr[Index] != 63
            || ImagePtr[Index + 1] != 0) {
        return EFI_JPEG_SOS_ERROR;
      }
      //
      // For the checking of the components of this scan.
      // ImagePtr[4 + Index * 2]: the index of the three component
      // ImagePtr[4 + Index * 2 + 1]: Td:Ta(the HuffmanTable index of DC and AC )
      //
      for (Index = 0; Index < ImagePtr[4]; Index++) {
        if (ImagePtr + 4 + Index * 2 + 2 >= mJfifData.ImageEnd
             || ImagePtr[4 + Index * 2 + 1] > 3
             || (ImagePtr[4 + Index * 2 + 2] >> 4) > 1
             || (ImagePtr[4 + Index * 2 + 2] & 0x0F) > 1) {
          return EFI_JPEG_SOS_ERROR;
        }
      }
      break;

    case JPEG_DHT:
      DecodeStatus = GetHuffmanTable (ImagePtr);
      if (DecodeStatus != EFI_JPEG_DECODE_SUCCESS) {
        return DecodeStatus;
      }
      break;

    case JPEG_DQT:
      //
      // There will be several Quantization tables in one DQT segment
      // About the first Quantization table in the DQT:
      // ImagePtr[4]: the bits of [0:3], Pq(Quantization table element precision)
      // ImagePtr[4]: the bits of [4:7], Tq(Quantization table destination identifier)
      //
      if ((ImagePtr[2] << 8) + ImagePtr[3] < 2) {
        return EFI_JPEG_QUANTIZATIONTABLE_ERROR;
      }
      DqtCount = ((ImagePtr[2] << 8) + ImagePtr[3] - 2)/65;
      TempPtr = ImagePtr + 4;
      for (Index = 0; Index < DqtCount; Index++) {
        if (TempPtr >= mJfifData.ImageEnd || (TempPtr[0] & 0x0F) > 3 || (TempPtr[0] >> 4) != 0) {
          return EFI_JPEG_QUANTIZATIONTABLE_ERROR;
        }
        mJfifData.DqtPtr[TempPtr[0] & 0x0F] = TempPtr + 1;
        TempPtr += 65;
        if (TempPtr > mJfifData.ImageEnd) {
          //
          // Make sure DqtPtr Buffer is in ImageBuffer. DqtPtr requires 64 bytes.
          //
          return EFI_JPEG_QUANTIZATIONTABLE_ERROR;
        }
      }
      break;

    case JPEG_APP0:
      mJfifData.App0Ptr = ImagePtr;
      break;

    case JPEG_APP1:
    case JPEG_APP2:
    case JPEG_APP3:
    case JPEG_APP4:
    case JPEG_APP5:
    case JPEG_APP6:
    case JPEG_APP7:
    case JPEG_APP8:
    case JPEG_APP9:
    case JPEG_APP10:
    case JPEG_APP11:
    case JPEG_APP12:
    case JPEG_APP13:
    case JPEG_APP14:
    case JPEG_APP15:
      break;

    case JPEG_COM:
      mJfifData.ComPtr = ImagePtr;
      break;

    case JPEG_DRI:
      if (ImagePtr + 5 >= mJfifData.ImageEnd) {
        //
        // Data access out of resource.
        //
        return EFI_JPEG_INVALID_PARAMETER;
      }
      mJfifData.McuRestart = (ImagePtr[4] << 8) + ImagePtr[5];
      break;

    case JPEG_RST0:
    case JPEG_RST1:
    case JPEG_RST2:
    case JPEG_RST3:
    case JPEG_RST4:
    case JPEG_RST5:
    case JPEG_RST6:
    case JPEG_RST7:
      break;

    default:
      break;
    }
    if (*(ImagePtr + 2) == 0xFF) {
      ImagePtr += 2;
    } else {
      ImagePtr += 2 + (ImagePtr[2] << 8) + ImagePtr[3];
    }
  }

  return EFI_JPEG_DECODE_SUCCESS;
}

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
  )
{
  UINT8        Index1;
  UINT8        Index2;
  UINT8        Index3;
  UINT8        Blocks;
  UINT8        HTIndex;

  //
  // Check Sos and Sof0Data pointer.
  //
  if (mJfifData.SosPtr == NULL) {
    return EFI_JPEG_SOS_ERROR;
  }
  if (mJfifData.Sof0Data.Ptr == NULL) {
    return EFI_JPEG_SOF_ERROR;
  }
  if (mJfifData.ImageEnd == NULL) {
    return EFI_JPEG_INVALID_PARAMETER;
  }

  //
  // Initialize the value of mDecoderData
  //
  mDecoderData.ImageEnd = mJfifData.ImageEnd;
  mDecoderData.ImagePtr =
    mJfifData.SosPtr + 2 + (mJfifData.SosPtr[2] << 8) + mJfifData.SosPtr[3];
  //
  // Read the first byte.
  //
  if (ReadByte ()) {
    return EFI_JPEG_INVALID_PARAMETER;
  }
  mDecoderData.BitPos = 7;

  //
  //Initialize the blocks infomation in a MCU
  //
  Blocks = 0;
  for (Index3 = 0; Index3 < mJfifData.Sof0Data.Components; Index3++) {
    for (Index1 = 0; Index1 < mJfifData.Sof0Data.Samples[Index3].Vi; Index1++) {
      for (Index2 = 0; Index2 < mJfifData.Sof0Data.Samples[Index3].Hi; Index2++) {
        //
        // Check Huff table to make sure it is valid.
        //
        mDecoderData.BlocksInMcu[Blocks].DcAcHTIndex = *(mJfifData.SosPtr + 6 + Index3 * 2);
        HTIndex = mDecoderData.BlocksInMcu[Blocks].DcAcHTIndex >> 4;
        if (HTIndex > 3 || mJfifData.HuffTable[HTIndex].Ptr == NULL) {
          return EFI_JPEG_HUFFMANTABLE_ERROR;
        }
        HTIndex = (mDecoderData.BlocksInMcu[Blocks].DcAcHTIndex & 0x0F) + 2;
        if (HTIndex > 3 || mJfifData.HuffTable[HTIndex].Ptr == NULL) {
          return EFI_JPEG_HUFFMANTABLE_ERROR;
        }
        //
        // Check Dqt table to make sure it is valid.
        //
        mDecoderData.BlocksInMcu[Blocks].QTIndex = mJfifData.Sof0Data.Samples[Index3].QuanTable;
        if (mDecoderData.BlocksInMcu[Blocks].QTIndex > 3 ||
            mJfifData.DqtPtr[mDecoderData.BlocksInMcu[Blocks].QTIndex] == NULL) {
          return EFI_JPEG_QUANTIZATIONTABLE_ERROR;
        }
        mDecoderData.BlocksInMcu[Blocks].HiViIndex = (Index2 << 4) + Index1;
        mDecoderData.DcVal[Index3] = 0;
        mDecoderData.BlocksInMcu[Blocks].DcValPtr = &(mDecoderData.DcVal[Index3]);
        Blocks++;
      }
    }
  }
  mDecoderData.Blocks = Blocks;

  return EFI_JPEG_DECODE_SUCCESS;
}
