/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  WheaElog.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _ERROR_MANAGER_TABLES_H_
#define _ERROR_MANAGER_TABLES_H_

//#include "ErrorManagerSetup.h"
#include <Library/PrintLib.h>


typedef UINT16                    STRING_REF;

//
// Global declarations
//

#define ERROR_CODE_COLUMN      0
#define ERROR_SEVERITY_COLUMN  15
#define ERROR_INSTANCE_COLUMN  35
#define BLANK                  L"          "

//
// Error conversion table structures
//
typedef struct {
  UINT32                  Value;
  STRING_REF              Token;
} STATUS_CODE_LOOKUP_TABLE;

//
// ERROR OPERATION LOOKUP TABLE
//
// Value - (Class/SubClass | Operation) parameter in ReportStatusCode(..)
//
// IsSpecificToken, Token:
// Error message can be generic or specific as specified by IsSpecificToken.
// If IsSpecificToken is TRUE, only the operation string represented by Token is printed.
// If IsSpecificToken is FALSE, Error Manager creates a generic message by concatenating the
// Class/SubClass and the operation Token.  Class/SubClass represents the subject of the message,
// while operation represents the verb of the message.
//
// LegacyErrorCode - A 16 bit error code representing the same error in a legacy system.
//                   If this field is zero, no corresponding legacy error code exists.
//
typedef struct {
  UINT32                  Value;
  BOOLEAN                 IsCustomToken;
  STRING_REF              Token;
  STRING_REF              ActionToken; 
  UINT16                  LegacyErrorCode;
} OPERATION_LOOKUP_TABLE;



//
// Look up table global structures
//
extern STATUS_CODE_LOOKUP_TABLE mSeverityToken[];
extern OPERATION_LOOKUP_TABLE mOperationToken[];


//
// Tiano Instance is 1 based.  0 Instance is non-applicable.
// Memory Instance consists of board for the upper 16 bits and DIMM
//
#define EFI_MEMORY_INSTANCE_DIMM_MASK       0x0000FFFF
#define EFI_MEMORY_INSTANCE_BOARD_MASK      0xFFFF0000
#define EFI_MEMORY_DIMM_INSTANCE(i)         ((UINT16) ((i) & EFI_MEMORY_INSTANCE_DIMM_MASK))
#define EFI_MEMORY_BOARD_INSTANCE(i)        ((UINT16) (((i) & EFI_MEMORY_INSTANCE_BOARD_MASK) >> 16) )

#define EFI_IS_MEMORY_DIMM_INSTANCE(i) ((((i) & EFI_MEMORY_INSTANCE_DIMM_MASK) != 0) && (((i) & EFI_MEMORY_INSTANCE_BOARD_MASK) != 0))
#define EFI_IS_MEMORY_BOARD_INSTANCE(i) ((((i) & EFI_MEMORY_INSTANCE_BOARD_MASK) != 0) && (((i) & EFI_MEMORY_INSTANCE_DIMM_MASK) == 0))
#define EFI_IS_MEMORY_SYSTEM_INSTANCE(i) ((i) == 0)
#define EFI_COMPACT_LEGACY_MEMORY_INSTANCE(i) (((EFI_MEMORY_BOARD_INSTANCE(i) -1) << 3) | (EFI_MEMORY_DIMM_INSTANCE(i)-1))
#define EFI_COMPACT_LEGACY_MEMORY_SYSTEM_OFFSET   0xFC
#define EFI_COMPACT_LEGACY_MEMORY_BOARD_OFFSET    0xE0
#define EFI_COMPACT_LEGACY_MEMORY_DIMM_OFFSET     0x00


EFI_HII_HANDLE                 ErrorTableHiiHandle = NULL;
EFI_GUID                            ErrorTableGuid = {0x2cefbe09, 0x67e6, 0x4f49, {0x84, 0xbd, 0x87, 0x33, 0x15, 0x22, 0x8, 0x7d}};
//
// Compact Class definitions
// Values of 0-3 are used by this specification
// Values in the range 4-7 are reserved for OEM use
//
#define EFI_COMPACT_STATUS_CODE_CLASS_MASK          0x0600
#define EFI_STATUS_CODE_CLASS_OEM_MASK              0x80000000
#define EFI_COMPACT_STATUS_CODE_CLASS_OEM_MASK      0x4000

//
// Compact Subclass definitions
// Values of 0-15 are used by this specification
// Values in the range 16-31 are reserved for OEM use
//
#define EFI_COMPACT_STATUS_CODE_SUBCLASS_MASK       0x01E0
#define EFI_COMPACT_STATUS_CODE_SUBCLASS_OEM_MASK   0x0800
#define EFI_STATUS_CODE_SUBCLASS_OEM_MASK           0x00800000

//
// Compact Error code definition
// General partitioning scheme for compact Error Codes 
// Operation Category (bit 13-12)
// 00b Reserved
// 01b Shared by all sub-classes in a given class
// 10b Subclass Specific
// 11b OEM specific

#define EFI_STATUS_CODE_OPERATION_CATEGORY_MASK     0x00003000
#define EFI_COMPACT_STATUS_CODE_OPERATION_SHARED    0x1000
#define EFI_COMPACT_STATUS_CODE_OPERATION_SUBCLASS  0x2000
#define EFI_COMPACT_STATUS_CODE_OPERATION_OEM       0x3000
#define EFI_COMPACT_STATUS_CODE_OPERATION_MASK      0x001F
#define EFI_STATUS_CODE_OPERATION_CODE_MASK         0x0FFF

UINT16
STATIC
CompactEfiStatusCode(
  UINT32 Efi32BitCode
);

#endif //#define _ERROR_MANAGER_TABLES_H_
