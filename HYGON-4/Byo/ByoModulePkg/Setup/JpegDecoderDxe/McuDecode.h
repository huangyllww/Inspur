/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Byosoft or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/** @file
  This code decode the JFIF image.

Copyright (c) 1999 - 2022, Byosoft Corporation. All rights reserved.<BR>
This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.



**/

#ifndef _HUFFMAN_DECODE_H_
#define _HUFFMAN_DECODE_H_

#include "JfifDecode.h"

#define COS0 1448 /* 4096*sqrt(1/8): 12 bits*/
#define COS1 2009 /* 4096*(1/2)*cos(1*pi/16): 12 bits*/
#define COS2 1892 /* 4096*(1/2)*cos(2*pi/16): 12 bits*/
#define COS3 1703 /* 4096*(1/2)*cos(3*pi/16): 12 bits*/
#define COS4 1448 /* 4096*(1/2)*cos(4*pi/16): 12 bits*/
#define COS5 1138 /* 4096*(1/2)*cos(5*pi/16): 12 bits*/
#define COS6 784  /* 4096*(1/2)*cos(6*pi/16): 12 bits*/
#define COS7 500  /* 4096*(1/2)*cos(7*pi/16): 12 bits*/

/**
  Get the bit of mDecoderData.BitPos in the mDecoderData.CurByte,
  mDecoderData.BitPos is between 0 and 7.

  @param IsEnd         When reading a new byte from the image, the image is end or not

  @return 0 or 1       The bit value of the mDecoderData.BitPos in the mDecoderData.CurByte.

**/
UINT8
GetNextBit (
  OUT    BOOLEAN                        *IsEnd
  );

/**
  Decode the block element from the image(Base on the mDecoderData.ImagePtr).


  @param HTIndex         The Huffman table index of this element decoding
  @param ZeroCount       In the AC element decoding, the number of zero before a Non-Zero element
  @param DecodedVal      The value of the Non-Zero element
  @param IsEnd           The image is end or not
  @param DecoderStatus   The detail status of the element decoding

  @retval EFI_SUCCESS      The element decoded successfully
  @retval EFI_UNSUPPORTED  The element decoding is failed, detail status refers "DecoderStatus"

**/
EFI_STATUS
ElementDecode (
  IN        UINT8                        HTIndex,
     OUT    UINT8                        *ZeroCount,
     OUT    INT16                        *DecodedVal,
     OUT    BOOLEAN                      *IsEnd,
     OUT    EFI_JPEG_DECODER_STATUS      *DecoderStatus
  );

/**
  Decode the block data from the image.


  @param BlockBuff       The pointer of the block's decoded data
  @param DcAcHTIndex     The indexes of this block's AC, DC Huffman table
  @param IsEnd           The image is end or not
  @param DecoderStatus   The detail status of the block decoding

  @retval EFI_SUCCESS      The block decoded successfully
  @retval EFI_UNSUPPORTED  The block decoding is failed, detail status refers "DecoderStatus"

**/
EFI_STATUS
BlockDecode (
     OUT    INT16                        *BlockBuff,
  IN        UINT8                        DcAcHTIndex,
     OUT    BOOLEAN                      *IsEnd,
     OUT    EFI_JPEG_DECODER_STATUS      *DecoderStatus
  );

/**
  Dequantize and De-Zigzag the block data.


  @param BlockSrcBuff    The pointer of the source data
  @param BlockDstBuff    The pointer of the destination data
  @param BlockIndex      The index of the block in the MCU

**/
VOID
BlockDequantDezigzag (
  IN        INT16                        *BlockSrcBuff,
     OUT    INT16                        *BlockDstBuff,
  IN        UINT8                         BlockIndex
  );

/**
  Do the IDCT with the block data, and add the offset(128) to Y component.


  @param BlockSrcDstBuff The pointer of the source data, and the destination data
                         will also be stored at the pointer
  @param BlockIndex      The index of the block in the MCU

**/
VOID
BlockIDctAddoffset (
  IN  OUT   INT16                        *BlockSrcDstBuff,
  IN        UINT8                         BlockIndex
  );

/**
  Decode the MCU data from the image, get the MCU's blocks data.


  @param McuDstBuff      The pointer of the MCU's decoded data
  @param IntervalFlag    When image has RST marker, the flag of the interval restart
  @param IsEnd           The image is end or not
  @param DecoderStatus   The detail status of the MCU decoding

  @retval EFI_SUCCESS      The MCU decoded successfully
  @retval EFI_UNSUPPORTED  The MCU decoding is failed, detail status refers "DecoderStatus"

**/
EFI_STATUS
McuRetrieve (
     OUT    INT16                        *McuDstBuff,
  IN        BOOLEAN                      IntervalFlag,
     OUT    BOOLEAN                      *IsEnd,
     OUT    EFI_JPEG_DECODER_STATUS      *DecoderStatus
  );

/**
  Decode the MCU data.


  @param McuSrcBuff      The pointer of the source data
  @param McuDstBuff      The pointer of the destination data

**/
VOID
McuDecode (
  IN        INT16                        *McuSrcBuff,
     OUT    INT16                        *McuDstBuff
  );


/**
  Store the MCU data into the GOP buffer.


  @param McuSrcBuff      The pointer of the source data
  @param DstBuff         The pointer of the destination data
  @param CurHPixel       The current horizontal pixel index of the MCU
  @param CurVLine        The current vertical line index of the MCU

**/
VOID
StoreMcuToBltBuffer (
  IN        INT16                        *McuSrcBuff,
  IN  OUT   UINT8                        **DstBuff,
  IN  OUT   UINT16                       *CurHPixel,
  IN  OUT   UINT16                       *CurVLine
  );

#endif
