/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _QR_ENCODER_PROTOCOL_H_
#define _QR_ENCODER_PROTOCOL_H_

#define _QR_ENCODER_PROTOCOL_GUID \
  { \
    0x4eeff0a1, 0xc0c, 0x49f6, { 0xa7, 0xde, 0xa5, 0xa7, 0xbd, 0x32, 0x66, 0x32 } \
  };

typedef struct _QR_ENCODER_PROTOCOL QR_ENCODER_PROTOCOL;

/**
  The QR code is generated

  @param  This                   The pointer to the QR_ENCODER protocol interface.
  @param  Data                   Character string for the QR Code.
  @param  DataLen                Length of the data.
  @param  Bitmap                 Where to store the pointer to a Gop->Blt ready bitmap that will fit into RegionSize.
                                 The caller needs to free the memory.

  @return EFI_SUCCESS if the QR code is generated successfully.
          others if fail.

**/
typedef
EFI_STATUS
(EFIAPI *GENERATE_QR) (
  IN  QR_ENCODER_PROTOCOL              *This,
  IN  UINT8                            *Data,
  IN  UINT16                           DataLen,
  IN  INTN                             RegionSize,
  OUT EFI_GRAPHICS_OUTPUT_BLT_PIXEL    **Bitmap
  );


struct _QR_ENCODER_PROTOCOL {
  GENERATE_QR                 GenerateQrCode;
};

extern EFI_GUID gQrEncoderProtocolGuid;
#endif