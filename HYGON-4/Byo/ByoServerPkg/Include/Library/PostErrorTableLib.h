/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#ifndef __POST_ERROR_TABLE_LIB_H__
#define __POST_ERROR_TABLE_LIB_H__ 


typedef UINT16                    STRING_REF;

//
// Error conversion table structures
//
typedef struct {
  UINT32                  Value;
  STRING_REF              Token;
} STATUS_CODE_LOOKUP_TABLE;


VOID
InitializeErrorTableHiiHandle();



EFI_STATUS
GetSubClassString(
  IN  UINT32                    Value,
  OUT STRING_REF           **String
);


EFI_STATUS
GetSeverityString(
  IN  UINT32                    Value,
  OUT UINT16		     **String
);


void
GetOperationString(
  IN   STRING_REF             Token,
  OUT UINT16		     **String
);

EFI_STATUS
MatchOperation (
  IN  UINT32              Value,
  OUT BOOLEAN             *IsCustom,
  OUT STRING_REF          *Token,
  OUT UINT16              *LegacyErrorCode
  );


EFI_STATUS
MatchOperationAction (
  IN  UINT32              Value,
  OUT STRING_REF          *Token
  );

UINT16
CompactStatusCode(
  IN UINT32  EfiErrorCode,
  IN UINT32  ErrorInstance,
  IN UINT16  LegacyErrorCode,
  IN BOOLEAN ErrorCodeZeroFlag
);

UINT16
GenerateLegacyMemoryCode(
  IN  UINT32            ErrorCodeValue ,
  IN  UINT32            ErrorInstance
  );


#endif //__POST_ERROR_TABLE_LIB_H__