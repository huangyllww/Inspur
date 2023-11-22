/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __VERB_TABLE_HDR_H__
#define __VERB_TABLE_HDR_H__

#include <Base.h>

typedef struct {
  UINT32  Vdid;
  UINT32  Ssid;
  UINT8   RevId;
  UINT8   FpSupport;
  UINT16  RearJacks;
  UINT16  FrontJacks;
} OEM_VERB_TABLE_HEADER;

typedef struct {
  OEM_VERB_TABLE_HEADER  Hdr;
  UINTN                  VerbDataSize;
  UINT32                 *VerbData;
} OEM_VERB_TABLE;

#endif
