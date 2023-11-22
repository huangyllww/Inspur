/** @file
 Copyright (c) 2010 - 2018, Byosoft Corporation.<BR>
 All rights reserved.This software and associated documentation (if any)
 is furnished under a license and may only be used or copied in
 accordance with the terms of the license. Except as permitted by such
 license, no part of this software or documentation may be reproduced,
 stored in a retrieval system, or transmitted in any form or by any
 means without the express written consent of Byosoft Corporation.

 File Name:

 Abstract:
    Interface of Byo Platform Setup Protocol.

 Revision History:

**/

#ifndef __BYO_PLATFORM_SETUP_CONFIG_H__
#define __BYO_PLATFORM_SETUP_CONFIG_H__

//
// Common ID.
//
#define BYO_FORMSET_CLASS                       0
#define BYO_FORMSET_SUB_CLASS                   0
#define ROOT_FORM_ID                        1

#define KEY_UNASSIGN_GROUP            0xFFFF

#define LABEL_END    0xffff

//
//Dynamic Refresh Label.
//
#define LABEL_DISK_DEVICE_LIST          0xB400
#define LABEL_DYNAMIC_FORMSET          0xB100
#define LABEL_CHANGE_BOOT_ORDER     0xB200
#define LABEL_PCIE_DEVICE_LIST         0xB300
#define LABEL_SATA_DEVICE_LIST         0xB300
#define LABEL_DIRECT_BOOT              LABEL_CHANGE_BOOT_ORDER
//
// Platform Language ID and Definition.
//
#define LABEL_CHANGE_LANGUAGE          0xB000
#define LABEL_CHANGE_LANGUAGE_END      LABEL_END

#define LABEL_CHOOSE_FOLDER            0xB004
#define LABEL_CHOOSE_FOLDER_END        LABEL_END

#define SECURITY_DYNAMIC_LABEL         0xB002
#define SECURITY_DYNAMIC_LABEL_END     LABEL_END

#define ADV_DYNAMIC_FORM_ID            0xF000

#define ADV_DYNAMIC_LABEL              LABEL_DYNAMIC_FORMSET
#define ADV_DYNAMIC_LABEL_END          LABEL_END

#define USB_DEV_LIST_LABEL             0xA002
#define USB_DEV_LIST_LABEL_END         LABEL_END

#define NVME_DEV_LIST_LABEL            0xA004
#define NVME_DEV_LIST_LABEL_END        LABEL_END

#define PCIE_DYNAMIC_LABEL             0xA005
#define PCIE_DYNAMIC_LABEL_END         LABEL_END



#define KEY_CHANGE_LANGUAGE    0x2341
#define KEY_CHOOSE_FOLDER      0x2342

#define LANGUAGE_VALUE_VARIABLE_GUID \
  { \
     0x538f2cf0, 0x1d9c, 0x44a9, { 0x99, 0x14, 0x4, 0xc6, 0xf6, 0x29, 0xd2, 0xce } \
  }

typedef struct _LANGUAGE_VALUE {
  UINT16  Value;
} LANGUAGE_VALUE;

#define LANGUAGE_VALUE_VARSTORE \
    efivarstore LANGUAGE_VALUE, varid = KEY_CHANGE_LANGUAGE, attribute = 2, name  = LangValue, guid  = LANGUAGE_VALUE_VARIABLE_GUID;

//
// This is the VFR compiler generated header file which defines the string identifiers.
//
#define LANGUAGE_NAME_STRING_ID     0x0001

///
/// The size of a 3 character ISO639 language code.
///
#define ISO_639_2_ENTRY_SIZE   3



#endif
