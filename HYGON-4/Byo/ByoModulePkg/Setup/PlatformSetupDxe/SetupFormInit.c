/*++

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:
  Platform configuration setup.

Revision History:


--*/

#include "PlatformSetupDxe.h"
#include <Library/PrintLib.h>


VOID
EFIAPI
UpdatePasswordStringWithLang (
  EFI_HII_HANDLE    HiiHandle,
  EFI_STRING_ID     StrTokenName,
  CHAR8             *Language
  )
{
  UINTN                             Size;
  CHAR16                            *StrBuffer;
  EFI_STRING                        PasswordLengthString;

  PasswordLengthString = HiiGetString(HiiHandle, StrTokenName, Language);
  Size = StrSize (PasswordLengthString) + sizeof (UINT8) * 2;
  StrBuffer = (CHAR16 *)AllocateZeroPool(Size);
  ASSERT(StrBuffer != NULL);
  switch (StrTokenName) {
    case STRING_TOKEN(STR_ADMIN_PASSWORD_HELP):
    case STRING_TOKEN(STR_USER_PASSWORD_HELP):
      UnicodeSPrint(StrBuffer, Size, PasswordLengthString, SYSTEM_PASSWORD_MIN_LENGTH, SYSTEM_PASSWORD_LENGTH);
      break;
    case STRING_TOKEN(STR_PASSWORD_TIMES):
      UnicodeSPrint(StrBuffer, Size, PasswordLengthString, PcdGet8(PcdRecorderPasswordTime));
      break;
    case STRING_TOKEN(STR_PASSWORD_REPEAT_TITLE_HELP):
      UnicodeSPrint(StrBuffer, Size, PasswordLengthString, FixedPcdGet8(PcdPasswordMaxRecorderTime));
      break;
    default:
      break;
  }
  HiiSetString (HiiHandle, StrTokenName, (EFI_STRING)StrBuffer, Language);
  FreePool(PasswordLengthString);
  FreePool (StrBuffer);
}

EFI_STATUS
SecurityFormInit (
  IN EFI_HII_HANDLE    HiiHandle
  )
{
  UpdatePasswordStringWithLang (HiiHandle, STRING_TOKEN(STR_ADMIN_PASSWORD_HELP), "en-US");
  UpdatePasswordStringWithLang (HiiHandle, STRING_TOKEN(STR_ADMIN_PASSWORD_HELP), "zh-Hans");

  UpdatePasswordStringWithLang (HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_HELP), "en-US");
  UpdatePasswordStringWithLang (HiiHandle, STRING_TOKEN(STR_USER_PASSWORD_HELP), "zh-Hans");

  UpdatePasswordStringWithLang (HiiHandle, STRING_TOKEN(STR_PASSWORD_TIMES), "en-US");
  UpdatePasswordStringWithLang (HiiHandle, STRING_TOKEN(STR_PASSWORD_TIMES), "zh-Hans");

  UpdatePasswordStringWithLang (HiiHandle, STRING_TOKEN(STR_PASSWORD_REPEAT_TITLE_HELP), "en-US");
  UpdatePasswordStringWithLang (HiiHandle, STRING_TOKEN(STR_PASSWORD_REPEAT_TITLE_HELP), "zh-Hans");

  return EFI_SUCCESS;
}
