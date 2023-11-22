/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SetupBrowser.h

Abstract:

Revision History:

**/
#ifndef __SETUP_BROWSER_H__
#define __SETUP_BROWSER_H__

#include <Guid/ByoSetupFormsetGuid.h>

#define SEARCH_OPTION_ID                             0x3000

#define SETUP_SEARCH_GUID \
  { 0xcaeb6219, 0xf608, 0xa7f5, { 0xab, 0xb9, 0x15, 0xeb, 0x78, 0x91, 0x16, 0x35 }}


#define EFI_SEARCH_VARIABLE_NAME                  L"ByoSetupSearch"

typedef struct {
  UINT16                   SearchInputData[31];
  UINT16                   MatchingAlgorithm;
} SEARCH_SAVE_DATA;

#define  KEY_SEARCH_OPTION                          0x4000
#define  KEY_SEARCH_ALGORITHM                       0x4001

#define LABEL_UPDATE_SEARCH_FORM                    0x4100
#define LABEL_END                                   0x4101
#define LABEL_UPDATE_UNOPTIONAL_SEARCH_FORM         0x4102
#define LABEL_UNOPTIONAL_END                        0x4103

extern EFI_GUID gEfiSetupSearchGuid;
#endif