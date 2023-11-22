/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_LEGACY_OPTION_CHECK_PROTOCOL__
#define __BYO_LEGACY_OPTION_CHECK_PROTOCOL__

//
// Byo legacy Option Check Protocol GUID value
//
#define BYO_LEGACY_OPTION_CHECK_PROTOCOL_GUID \
    { \
      0x838a2da3, 0xc605, 0x4d21, { 0xb7, 0x1c, 0xe0, 0xa2, 0xc1, 0x3d, 0xd, 0x5c } \
    }

//
// Protocol interface structure
//
typedef struct _BYO_LEGACY_OPTION_CHECK_PROTOCOL BYO_LEGACY_OPTION_CHECK_PROTOCOL;

//
// Revision The revision to which the protocol interface adheres.
//          All future revisions must be backwards compatible.
//          If a future version is not back wards compatible it is not the same GUID.
//
#define BYO_LEGACY_OPTION_CHECK_PROTOCOL_REVISION 0x00000001

#define INVALID_BBSINDEX_VALUE 0xFFFF

/*
  This function allows platform to customize the FuzzyMatch.The platform can match the option
  to the BbsEntry of BbsTable according to the customized description or other information.
  @param[in const] Option                     need to match it to the BbsEntry of BbsTable.
  @param[in] CustomizedType                   only legacy options with CustomizedType type can be matched by
                                              this PLATFORM_FUZZY_MATCH.Other types will be matched by default
                                              FuzzyMatch in LegacyBootManagerLib.
  @param[in] BbsCount                         count of BbsTable.
  @retval INVALID_BBSINDEX_VALUE              this option need to be match by default FuzzyMatch in
                                              LegacyBootManagerLib not PLATFORM_FUZZY_MATCH.
  @retval BbsCount                            this option cannot be matched to any BbsEntry of BbsTable,and this
                                              option will be deleted in LegacyBootManagerLib later.
  @retval Valid BbsIndex value of BbsTable    this option matched its BbsEntry of BbsTable.
*/
typedef
UINT16
(EFIAPI *PLATFORM_FUZZY_MATCH) (
  IN CONST VOID                          *Option,
  IN BOOLEAN                             *BbsIndexUsed,
  IN BBS_TABLE                           *BbsTable,
  IN UINTN                               BbsCount
  );




struct _BYO_LEGACY_OPTION_CHECK_PROTOCOL {
  UINT64                                         Revision;
  PLATFORM_FUZZY_MATCH                           BbsInexFuzzyMatch;
};

extern EFI_GUID gByoLegacyOptionCheckProtocolGuid;

#endif /* __BYO_LEGACY_OPTION_CHECK_PROTOCOL__ */


