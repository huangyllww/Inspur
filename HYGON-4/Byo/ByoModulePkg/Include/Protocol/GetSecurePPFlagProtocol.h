/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/
#ifndef __GETSECUREPPFLAG_PROTOCOL_H__
#define __GETSECUREPPFLAG_PROTOCOL_H__

#define GETSECUREPPFLAG_PROTOCOL_GUID \
  { \
    0x2E33BE13, 0xB7AC, 0x4E0F, {0x80, 0xA8, 0x9D, 0xAF, 0xDA, 0x9B, 0x9D, 0x7A } \
  }

typedef struct _GETSECUREPPFLAG_PROTOCOL GETSECUREPPFLAG_PROTOCOL;

/**
  Get PcdSecurePPMemAddress value

  @param[in, out] Data32              Return PcdSecurePPMemAddress value

  @retval EFI_SUCCESS                 Always return EFI_SUCCESS
**/
typedef
EFI_STATUS
(EFIAPI *GETSECUREPPFLAG_PROTOCOL_GET_SECURE_PP_FLAG)(
  IN UINT32       **Data32
);

struct _GETSECUREPPFLAG_PROTOCOL {
  GETSECUREPPFLAG_PROTOCOL_GET_SECURE_PP_FLAG      GetSecurePPFlag;
};

extern EFI_GUID  gGetSecurePPFlagProtocolGuid;
#endif