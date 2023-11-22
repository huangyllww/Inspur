/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_CUSTOMSTR_PROTOCOL_H__
#define __BYO_CUSTOMSTR_PROTOCOL_H__

#define BYO_CUSTOM_STR_PROTOCOL_GUID \
{ 0x3b0af8f0, 0x967d, 0x4a40, { 0xab, 0xb5, 0x61, 0xa7, 0x68, 0xef, 0x04, 0xc7 } };

typedef struct {
  CHAR16 *StrSetupDefaultConfirmation;
  CHAR16 *StrSetupConfirmation;
  CHAR16 *StrDiscard;
  CHAR16 *StrDiscardExit;
  CHAR16 *StrLoadDefault;
  CHAR16 *StrLoadFormset;
  CHAR16 *StrSaving;
  CHAR16 *StrSavingExit;
  CHAR16 *StrSaveUserDefault;
  CHAR16 *StrLoadUserDefault;
  CHAR16 *StrSetPasswordSuccess;
  CHAR16 *StrDefaultValueRecovery;
  CHAR16 *StrConfigurationSaving;
  CHAR16 *gStrPromptMsg;
} BYO_CUSTOM_STR_PROTOCOL;

extern EFI_GUID gByoCustomStrProtocolGuid;

#endif

