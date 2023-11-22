/** @file

Copyright (c) 1999 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/


#ifndef _PLAT_CONFIGURATION_H_
#define _PLAT_CONFIGURATION_H_


#define KEY_CHANGE_LANGUAGE             0x2341  //BYOSOFT_OVERRIDE

#define KEY_SAVE_TO_FILE                0x9001
#define KEY_RESTORE_FROM_FILE           0x9002
#define KEY_SAVE_AND_EXIT_VALUE         4099
#define KEY_DISCARD_AND_EXIT_VALUE      4100
#define KEY_RESTORE_DEFAULTS_VALUE      4101
#define KEY_LOAD_CUS_DEFAULTS_VALUE     4110
#define KEY_SCU_CUS_FLAG                4111
#define KEY_SAVE_WITHOUT_EXIT_VALUE     4112
#define KEY_DISCARD_WITHOUT_EXIT_VALUE  4113

#define KEY_UEFI_LEGACY_MODE_SELECT     4102

#define AUTO_ID(x)  x

#define EXIT_FORM_SET


#define EXIT_FORM_SET_GUID \
  { \
    0xa43b03dc, 0xc18a, 0x41b1, { 0x91, 0xc8, 0x3f, 0xf9, 0xaa, 0xa2, 0x57, 0x13 } \
  }

#define MAIN_FORM_SET_GUID  \
  { \
    0x985eee91, 0xbcac, 0x4238, { 0x87, 0x78, 0x57, 0xef, 0xdc, 0x93, 0xf2, 0x4e } \
  }

#define ADVANCED_FORM_SET_GUID \
  { \
    0xe14f04fa, 0x8706, 0x4353, { 0x92, 0xf2, 0x9c, 0x24, 0x24, 0x74, 0x6f, 0x9f } \
  }
#define FRONT_PAGE_GUID        { 0xe58809f8, 0xfbc1, 0x48e2, { 0x88, 0x3a, 0xa3, 0xf, 0xdc, 0x4b, 0x44, 0x1e } }

//#define PLATFORM_VARIABLE_ATTRIBUTES 7

#define SECURITY_FORM_SET_GUID \
  { \
    0x981ceaee, 0x931c, 0x4a17, { 0xb9, 0xc8, 0x66, 0xc7, 0xbc, 0xfd, 0x77, 0xe1 } \
  }

#define SECUREBOOT_CONFIG_FORM_SET_GUID \
  { \
    0x5daf50a5, 0xea81, 0x4de2, {0x8f, 0x9b, 0xca, 0xbd, 0xa9, 0xcf, 0x5c, 0x14 } \
  }  

#define SERVER_MGMT_FORM_SET_GUID \
  { \
    0xbb651963, 0xe710, 0x4006, { 0xb3, 0xef, 0xdc, 0x53, 0x60, 0x0a, 0x17, 0x5c }\
  }

#endif
