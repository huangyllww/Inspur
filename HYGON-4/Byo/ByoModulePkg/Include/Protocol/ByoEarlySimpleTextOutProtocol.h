/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL_H__
#define __BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL_H__

#include <Base.h>

typedef struct _BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_OUT_STRING)(
  IN  UINT16                          Column,
  IN  UINT16                          Row,
  IN  CHAR8                           *String
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_GET_MODE)(
  OUT  UINT32  *Column,
  OUT  UINT32  *Row
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_SET_FRAMEBUFFER)(
  IN UINT64      FrameBuffer
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_BYO_SET_ATTRIBUTE)(
  IN UINT32      Attribute
  );

typedef
EFI_STATUS
(EFIAPI *EFI_TEXT_BYO_SET_MODE)(
  IN UINT32      ModeNumber
  );

struct _BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL{
  EFI_TEXT_OUT_STRING           OutputString;
  EFI_TEXT_BYO_SET_ATTRIBUTE    SetAttribute;
  EFI_TEXT_BYO_SET_MODE         SetMode;
  EFI_TEXT_GET_MODE             GetMode;
  EFI_TEXT_SET_FRAMEBUFFER      SetFrameBuffer;
};


extern EFI_GUID gByoEarlySimpleTextOutProtocolGuid;


#endif
