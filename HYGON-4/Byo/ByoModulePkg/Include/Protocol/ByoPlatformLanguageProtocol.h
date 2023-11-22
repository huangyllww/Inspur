/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_PLATFORM_LANGUAGE_PROTOCOL_H__
#define __BYO_PLATFORM_LANGUAGE_PROTOCOL_H__


typedef struct {
  CHAR8    PlatformLang[32];
  BOOLEAN  LangChanged;
} BYO_PLATFORM_LANGUAGE_PROTOCOL;

extern EFI_GUID gByoPlatformLanguageProtocolGuid;

#endif
