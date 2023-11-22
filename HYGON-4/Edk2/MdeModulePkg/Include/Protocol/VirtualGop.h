/** @file
  This code supports the implementation of the Virtual Gop protocol

Copyright (c) 2009 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _VIRTUAL_GOP_PROTOCOL_H_
#define _VIRTUAL_GOP_PROTOCOL_H_

// Global ID for the draw management Protocol
//
#define VIRTUAL_GOP_PROTOCOL_GUID                                                  \
  {                                                                                \
    0x744d0cdc, 0x11c4, 0x4500, { 0x8e, 0x8a, 0x3a, 0x69, 0xb1, 0x94, 0x6b, 0xd8 } \
  }

typedef struct  _VIRTUAL_GOP_PROTOCOL  VIRTUAL_GOP_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *VIRTUAL_GOP_ENABLE)(
    IN  VIRTUAL_GOP_PROTOCOL           *This,
    IN  BOOLEAN                        Enable
);

typedef enum {
  VGopBltBufferToVideo,
  VGopBltVideoToBuffer,
  VGopAutoBltBufferToVideo,
  VGopAutoBltVideoToBuffer,
  VGopGraphicsOutputBltOperationMax
} VIRTUAL_GOP_SYNC_TYPE;

typedef
EFI_STATUS
(EFIAPI *VIRTUAL_GOP_SYNC)(
    IN  VIRTUAL_GOP_PROTOCOL              *This,
    IN  VIRTUAL_GOP_SYNC_TYPE             Type,
    IN  UINT32                            X,       OPTIONAL
    IN  UINT32                            Y,       OPTIONAL
    IN  UINT32                            Width,   OPTIONAL
    IN  UINT32                            Height   OPTIONAL
);

// Draw management protocol structure
//
struct _VIRTUAL_GOP_PROTOCOL
{
    BOOLEAN                            IsVirtualGop;

    // Set draw border
    //
    VIRTUAL_GOP_SYNC                   VirtualGopSync;
    VIRTUAL_GOP_ENABLE                 EnableVirtualGop;
};

extern EFI_GUID gVirtualGopProtocolGuid;

#endif
