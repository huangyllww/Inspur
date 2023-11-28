// 
// Copyright (c) 2006-2012, Intel Corporation
// All rights reserved. This program and the accompanying materials
// are licensed and made available under the terms and conditions of the BSD License
// which accompanies this distribution.  The full text of the license may be found at
// http://opensource.org/licenses/bsd-license.php
// 
// THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
// WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
// 
// Module Description:
//
// Formset guids, form id and VarStore data structure for Boot Maintenance Manager.

#ifndef _FORM_GUID_H_
#define _FORM_GUID_H_
  

#define FORM_OPROM_PRIORITY                  0x1000

#define KEY_VALUE_BOOTTYPE_CONFIGURATION     0x2001

#define VARSTORE_ID_BOOT_MAINT               0x1000
#define UEFI_BOOT_ITEM_ID                    0x8000
#define UEFI_BOOT_ITEM_END_ID                0x8400
#define LEGACY_BOOT_ITEM_ID                  0x8800
#define LEGACY_BOOT_ITEM_END_ID              0x8C00
#define LABEL_PXE_START                      0x8E00
#define LABEL_PXE_END                        0x8F00

#define BMM_QUESTION_ID_BASE                 0xA000


#define MAX_MENU_NUMBER                      60



typedef struct {
  UINT16   LegacyHDD[MAX_MENU_NUMBER];
  UINT16   LegacyODD[MAX_MENU_NUMBER];  
  UINT16   LegacyUskDisk[MAX_MENU_NUMBER];
  UINT16   LegacyUsbOdd[MAX_MENU_NUMBER];
  UINT16   LegacyPxe[MAX_MENU_NUMBER];
  UINT16   UefiHDD[MAX_MENU_NUMBER];
  UINT16   UefiODD[MAX_MENU_NUMBER];  
  UINT16   UefiUskDisk[MAX_MENU_NUMBER];
  UINT16   UefiUsbOdd[MAX_MENU_NUMBER];
  UINT16   UefiPxe[MAX_MENU_NUMBER];
  UINT16   UefiOthers[MAX_MENU_NUMBER];   
  UINT8    CsmOn;

} BMM_FAKE_NV_DATA;


#endif

