/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef _CHANGE_LANGUAGE_H_
#define _CHANGE_LANGUAGE_H_

/**
  Create language select item on a Formset.

  @param HiiHandle         The Hii Handle of Formset
  @param FormSetGuid    The Guid of Formset
  
**/
EFI_STATUS
UpdateLanguageSettingItem (
  EFI_HII_HANDLE    HiiHandle,
  EFI_GUID    *FormSetGuid,
  EFI_FORM_ID     FormId,
  EFI_STRING_ID    ItemPrompt,
  EFI_STRING_ID    ItemHelp
  );


/**
  The Callback Function of Language Select.
  
**/
EFI_STATUS
SetLanguageCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );



/**
  Get current language and store to gLastLanguage.
  Return TRUE if language is changed .

**/
BOOLEAN
BeLanguageChanged (
  VOID
  );

#endif // _CHANGE_LANGUAGE_H_
