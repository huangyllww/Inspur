/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __RSA_SIGN_DATA_HOB__
#define __RSA_SIGN_DATA_HOB__


#pragma pack(1)

typedef struct {
  UINT32    Crc32;
  UINT16    KeySize;
  BOOLEAN   IsProKey;
  UINT8     Reserved;
} PUBKEY_HEADER;

typedef struct {
  PUBKEY_HEADER     Hdr;
  UINT8             KeyN[256];
  UINT8             KeyE[3];
} PK_RSA2048_HOB_DATA;

#pragma pack()


#define RSA2048_SIGN_SIZE     256

#endif
