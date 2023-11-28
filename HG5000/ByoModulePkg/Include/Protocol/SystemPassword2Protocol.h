/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SystemPassword2Protocol.h

Abstract:
  Implementation of basic setup password function.

Revision History:

**/

#ifndef __SYSTEM_PASSWORD2_PROTOCOL___H__
#define __SYSTEM_PASSWORD2_PROTOCOL___H__


typedef enum {
  PD_ADMIN     = 0,
  PD_POWER_ON,
  PD_MAX
} PASSWORD_TYPE;



typedef EFI_STATUS
(*WRITE_SYSTEM_PASSWORD) (
  IN  PASSWORD_TYPE    Type,
  OUT CHAR16           *Password
);

typedef BOOLEAN
(*BE_HAVE_SYSTEM_PASSWORD) (
  IN PASSWORD_TYPE    Type
);

typedef BOOLEAN
(*VERIFY_SYSTEM_PASSWORD) (
  IN PASSWORD_TYPE    Type,
  IN CHAR16           *Password
);

typedef EFI_STATUS
(*UPDATE_SYSTEM_PASSWORD) (
  VOID
);

typedef BOOLEAN
(*IS_PASSWORD_EQUAL) (
  VOID
);

typedef struct  { 
  BE_HAVE_SYSTEM_PASSWORD  BeHave;
  WRITE_SYSTEM_PASSWORD    Write;
  VERIFY_SYSTEM_PASSWORD   Verify;
  UPDATE_SYSTEM_PASSWORD   Update;
  IS_PASSWORD_EQUAL        IsEqual;
  
} BYO_SYSTEM_PASSWORD2_PROTOCOL;

extern EFI_GUID gByoSystemPassword2ProtocolGuid;

#endif
