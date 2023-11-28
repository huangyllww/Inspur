/** @file

Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _DIRECT_BOOT_H_
#define _DIRECT_BOOT_H_

/**
  Create direct boot item according to BootOrder Variable.

**/
EFI_STATUS
CreateDirectBootItem (
  IN  EFI_HII_HANDLE    HiiHandle,
  IN  EFI_GUID    *FormsetGuid,
  IN  EFI_FORM_ID    FormId,
  IN  EFI_STRING_ID  HelpToken
  );

/**
  The Callback Function of Direct Boot.
  
**/
EFI_STATUS
DirectBootCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );
  
#endif
