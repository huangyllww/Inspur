///** @file
//  Copyright (c) 2023, Byosoft Corporation.<BR>
//  All rights reserved.This software and associated documentation (if any)
//  is furnished under a license and may only be used or copied in
//  accordance with the terms of the license. Except as permitted by such
//  license, no part of this software or documentation may be reproduced,
//  stored in a retrieval system, or transmitted in any form or by any
//  means without the express written consent of Byosoft Corporation.
//
//**/

#ifndef _FORM_GUID_H_
#define _FORM_GUID_H_

#define BOOT_MAINT_MANAGER_FORMSET_GUID \
  { \
  0x2be183d5, 0xc7ff, 0x46a5, { 0xb7, 0x18, 0xe9, 0x69, 0x3a, 0xbc, 0xbf, 0x4 } \
  }

#define FORM_OPROM_PRIORITY                  0x1000
#define FORM_MAIN_ID                         0x1001
#define FORM_DISABLE_GROUP_TYPE              0x2000
#define FORM_DISABLE_BOOT_OPTIONS            0x3000
#define FORM_ADD_BOOT_OPTIONS                0x4000
#define FORM_DELETE_BOOT_OPTIONS             0x5000

#define VARSTORE_ID_BOOT_MAINT               0x1000
#define UEFI_BOOT_ITEM_ID                    0x8000
#define UEFI_BOOT_ITEM_END_ID                0x8400
#define BMM_QUESTION_ID_BASE                 0xA000

#define LEGACY_BOOT_ITEM_ID                  0x8800
#define LEGACY_BOOT_ITEM_END_ID              0x8C00

#define BOOT_ORDER_PAGE_QUESTION_ID          0xB000
#define DISABLE_GROUP_TYPE_PAGE_QUESTION_ID  0xB001
#define DISABLE_OPTIONS_PAGE_QUESTION_ID     0xB002
#define KEY_VALUE_SAVE_AND_EXIT_BOOT         0xB003
#define KEY_VALUE_NO_SAVE_AND_EXIT_BOOT      0xB004

#define DISABLE_UEFI_BOOT_GROUP_ID           0x9000
#define DISABLE_UEFI_BOOT_GROUP_END_ID       0x9400
#define DISABLE_GROUP_QUESTION_ID_BASE       0xC000

#define DISABLE_LEGACY_BOOT_GROUP_ID         0x9800
#define DISABLE_LEGACY_BOOT_GROUP_END_ID     0x9C00
#define DISABLE_LEGACY_QUESTION_ID_BASE      0xC500

#define DISABLE_BOOT_OPTIONS_ID              0x5000
#define DISABLE_BOOT_OPTIONS_END_ID          0x5400
#define DISABLE_OPTIONS_QUESTION_ID_BASE     0xD000

#define FORM_ADD_OPTIONS_ID                  0xE000
#define FORM_DELETEL_OPTIONS_ID              0xE001
#define DELETEL_OPTIONS_ID                   0xE002
#define DELETEL_OPTIONS_END_ID               0xE003
#define FORM_ADD_OPTIONS_QUESTION_ID         0xE004

#define BASE_DELETE_OPTION_QUESTION_ID      0xD001
#define MIN_DELETE_OPTION_QUESTION_ID       BASE_DELETE_OPTION_QUESTION_ID
#define MAX_DELETE_OPTION_QUESTION_ID       0xDFFE

#define MAX_MENU_NUMBER                      32
#define MAX_GROUP_COUNT                      6


#define MIN_BOOT_DESCRIPTION_LEN             6
#define MAX_BOOT_DESCRIPTION_LEN             75
#define MAX_BOOT_DESCRIPTION_STRING_LEN      76

typedef struct {
  //
  // Boot Order
  //
  UINT16  LegacyHDD[MAX_MENU_NUMBER];
  UINT16  LegacyODD[MAX_MENU_NUMBER];
  UINT16  LegacyUskDisk[MAX_MENU_NUMBER];
  UINT16  LegacyUsbOdd[MAX_MENU_NUMBER];
  UINT16  LegacyPxe[MAX_MENU_NUMBER];
  UINT16  LegacyOthers[MAX_MENU_NUMBER];
  UINT16  UefiHDD[MAX_MENU_NUMBER];
  UINT16  UefiODD[MAX_MENU_NUMBER];
  UINT16  UefiUskDisk[MAX_MENU_NUMBER];
  UINT16  UefiUsbOdd[MAX_MENU_NUMBER];
  UINT16  UefiPxe[MAX_MENU_NUMBER];
  UINT16  UefiOthers[MAX_MENU_NUMBER];
  //
  // status of boot options:disabled or enabled
  //
  BOOLEAN  StatusLegacyHDD[MAX_MENU_NUMBER];
  BOOLEAN  StatusLegacyODD[MAX_MENU_NUMBER];
  BOOLEAN  StatusLegacyUskDisk[MAX_MENU_NUMBER];
  BOOLEAN  StatusLegacyUsbOdd[MAX_MENU_NUMBER];
  BOOLEAN  StatusLegacyPxe[MAX_MENU_NUMBER];
  BOOLEAN  StatusLegacyOthers[MAX_MENU_NUMBER];
  BOOLEAN  StatusUefiHDD[MAX_MENU_NUMBER];
  BOOLEAN  StatusUefiODD[MAX_MENU_NUMBER];
  BOOLEAN  StatusUefiUskDisk[MAX_MENU_NUMBER];
  BOOLEAN  StatusUefiUsbOdd[MAX_MENU_NUMBER];
  BOOLEAN  StatusUefiPxe[MAX_MENU_NUMBER];
  BOOLEAN  StatusUefiOthers[MAX_MENU_NUMBER];
  //
  // status of boot groups:disabled or enabled
  //
  BOOLEAN  StatusUefiBootGroup[MAX_GROUP_COUNT];
  BOOLEAN  StatusLegacyBootGroup[MAX_GROUP_COUNT];
  UINT16   BootDescriptionData[MAX_BOOT_DESCRIPTION_STRING_LEN];
} BMM_FAKE_NV_DATA;


#endif

