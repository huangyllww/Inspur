/** @file

  @copyright
  BYOSOFT CONFIDENTIAL
  Copyright 2006 - 2022 Byosoft Corporation. <BR>

  The source code contained or described herein and all documents related to the
  source code ("Material") are owned by Byosoft Corporation or its suppliers or
  licensors. Title to the Material remains with Byosoft Corporation or its suppliers
  and licensors. The Material may contain trade secrets and proprietary    and
  confidential information of Byosoft Corporation and its suppliers and licensors,
  and is protected by worldwide copyright and trade secret laws and treaty
  provisions. No part of the Material may be used, copied, reproduced, modified,
  published, uploaded, posted, transmitted, distributed, or disclosed in any way
  without Byosoft's prior express written permission.

  No license under any patent, copyright, trade secret or other intellectual
  property right is granted to or conferred upon you by disclosure or delivery
  of the Materials, either expressly, by implication, inducement, estoppel or
  otherwise. Any license under such intellectual property rights must be
  express and approved by Byosoft in writing.

  Unless otherwise agreed by Byosoft in writing, you may not remove or alter
  this notice or any other notice embedded in Materials by Byosoft or
  Byosoft's suppliers or licensors in any way.
**/

#ifndef _FORM_GUID_H_
#define _FORM_GUID_H_

#define BOOT_MAINT_FORMSET_GUID \
  { \
  0x642237c7, 0x35d4, 0x472d, {0x83, 0x65, 0x12, 0xe0, 0xcc, 0xf2, 0x7a, 0x22} \
  }

#define FILE_EXPLORE_FORMSET_GUID \
  { \
  0xfe561596, 0xe6bf, 0x41a6, {0x83, 0x76, 0xc7, 0x2b, 0x71, 0x98, 0x74, 0xd0} \
  }

#define FORM_MAIN_ID                         0x1001
#define FORM_BOOT_ADD_ID                     0x1002
#define FORM_BOOT_DEL_ID                     0x1003
#define FORM_BOOT_CHG_ID                     0x1004
#define FORM_DRV_ADD_ID                      0x1005
#define FORM_DRV_DEL_ID                      0x1006
#define FORM_DRV_CHG_ID                      0x1007
#define FORM_CON_MAIN_ID                     0x1008
#define FORM_CON_IN_ID                       0x1009
#define FORM_CON_OUT_ID                      0x100A
#define FORM_CON_ERR_ID                      0x100B
#define FORM_FILE_SEEK_ID                    0x100C
#define FORM_FILE_NEW_SEEK_ID                0x100D
#define FORM_DRV_ADD_FILE_ID                 0x100E
#define FORM_DRV_ADD_HANDLE_ID               0x100F
#define FORM_DRV_ADD_HANDLE_DESC_ID          0x1010
#define FORM_BOOT_NEXT_ID                    0x1011
#define FORM_TIME_OUT_ID                     0x1012
#define FORM_BOOT_SETUP_ID                   0x1014
#define FORM_DRIVER_SETUP_ID                 0x1015
#define FORM_BOOT_LEGACY_DEVICE_ID           0x1016
#define FORM_CON_COM_ID                      0x1017
#define FORM_CON_COM_SETUP_ID                0x1018
#define FORM_SET_FD_ORDER_ID                 0x1019
#define FORM_SET_HD_ORDER_ID                 0x101A
#define FORM_SET_CD_ORDER_ID                 0x101B
#define FORM_SET_NET_ORDER_ID                0x101C
#define FORM_SET_BEV_ORDER_ID                0x101D
#define FORM_FILE_EXPLORER_ID                0x101E
#define FORM_BOOT_ADD_DESCRIPTION_ID         0x101F
#define FORM_DRIVER_ADD_FILE_DESCRIPTION_ID  0x1020
#define FORM_CON_MODE_ID                     0x1021
#define FORM_MEMORY_CHECK_ID                 0x1022
#define FORM_UEFI_OPTIMIZED_BOOT_ID          0x1023
#define FORM_BOOT_SEQUENCE_ID                0x1033
#define FORM_SPECIFIC_BOOT_ID                0x1043
#define FORM_SPECIFIC_UEFI_BOOT_ID           0x1044
#define FORM_SPECIFIC_LEGACY_BOOT_ID         0x1045
#define IBV_BOOT_MANAGER_FORM_ID             0x1046
#define FORM_ADD_NEW_FILE_ID                 0x1047
#define FORM_ADD_NEW_FOLDER_ID               0x1048
#define NEW_FILE_NAME_ID                     0x1049
#define NEW_FOLDER_NAME_ID                   0x104A

#define MAXIMUM_FORM_ID                      0x10FF

#define KEY_VALUE_COM_SET_BAUD_RATE          0x1101
#define KEY_VALUE_COM_SET_DATA_BITS          0x1102
#define KEY_VALUE_COM_SET_STOP_BITS          0x1103
#define KEY_VALUE_COM_SET_PARITY             0x1104
#define KEY_VALUE_COM_SET_TERMI_TYPE         0x1105
#define KEY_VALUE_MAIN_BOOT_NEXT             0x1106
#define KEY_VALUE_BOOT_ADD_DESC_DATA         0x1107
#define KEY_VALUE_BOOT_ADD_OPT_DATA          0x1108
#define KEY_VALUE_DRIVER_ADD_DESC_DATA       0x1109
#define KEY_VALUE_DRIVER_ADD_OPT_DATA        0x110A
#define KEY_VALUE_SAVE_AND_EXIT              0x110B
#define KEY_VALUE_NO_SAVE_AND_EXIT           0x110C
#define KEY_VALUE_BOOT_FROM_FILE             0x110D
//
//BYO_OVERRIDE_START
//
#define KEY_VALUE_ADD_TO_BOOT_MENU           0x111F
#define KEY_VALUE_CREATE_FILE_AND_EXIT       0x1120
#define KEY_VALUE_NO_CREATE_FILE_AND_EXIT    0x1121
#define KEY_VALUE_CREATE_FOLDER_AND_EXIT     0x1122
#define KEY_VALUE_NO_CREATE_FOLDER_AND_EXIT  0x1123

#define KEY_LABEL_BOOT_NOW                   0x1140
#define KEY_LABEL_BOOT_NOW_MAX               0x11FF


#define FORM_SORT_CLASS_ORDER_ID             0x9000
#define FORM_SORT_CLASS_ORDER_ID_MAX         0x90FF
#define FORM_HDD_CLASS_ORDER_ID              0x9100
#define FORM_DVD_CLASS_ORDER_ID              0x9101
#define FORM_PXE_CLASS_ORDER_ID              0x9102
#define FORM_OTH_CLASS_ORDER_ID              0x9103
#define FORM_HDD_SPECIFIC_ORDER_ID           0x9104
#define FORM_DVD_SPECIFIC_ORDER_ID           0x9105
#define FORM_PXE_SPECIFIC_ORDER_ID           0x9106
#define FORM_OTH_SPECIFIC_ORDER_ID           0x9107
#define FORM_BOOT_MANAGER_QUESTION_ID        0x9200
#define FORM_BOOT_MANAGER_QUESTION_ID_MAX    0x92FF
//
//BYO_OVERRIDE_END
//
#define MAXIMUM_NORMAL_KEY_VALUE             0x11FF

//
// Varstore ID defined for Buffer Storage
//
#define VARSTORE_ID_BOOT_MAINT               0x1000
#define VARSTORE_ID_FILE_EXPLORER            0x1001

//
// End Label
//
#define LABEL_END   0xffff
#define MAX_MENU_NUMBER 100

///
/// This is the structure that will be used to store the
/// question's current value. Use it at initialize time to
/// set default value for each question. When using at run
/// time, this map is returned by the callback function,
/// so dynamically changing the question's value will be
/// possible through this mechanism
///
typedef struct {
  //
  // Three questions displayed at the main page
  // for Timeout, BootNext, MemoryCheck Variables respectively
  //
  UINT16  BootTimeOut;
  UINT16  BootNext;
  UINT8   MemoryCheck;
  BOOLEAN UefiOptimizedBoot;

  //
  // This is the COM1 Attributes value storage
  //
  UINT8   COM1BaudRate;
  UINT8   COM1DataRate;
  UINT8   COM1StopBits;
  UINT8   COM1Parity;
  UINT8   COM1TerminalType;

  //
  // This is the COM2 Attributes value storage
  //
  UINT8   COM2BaudRate;
  UINT8   COM2DataRate;
  UINT8   COM2StopBits;
  UINT8   COM2Parity;
  UINT8   COM2TerminalType;

  //
  // Driver Option Add Handle page storage
  //
  UINT16  DriverAddHandleDesc[MAX_MENU_NUMBER];
  UINT16  DriverAddHandleOptionalData[MAX_MENU_NUMBER];
  UINT8   DriverAddActive;
  UINT8   DriverAddForceReconnect;

  //
  // Console Input/Output/Errorout using COM port check storage
  //
  UINT8   ConsoleInputCOM1;
  UINT8   ConsoleInputCOM2;
  UINT8   ConsoleOutputCOM1;
  UINT8   ConsoleOutputCOM2;
  UINT8   ConsoleErrorCOM1;
  UINT8   ConsoleErrorCOM2;

  //
  // Console port for Out-Of-Band use.
  //
  UINT16   COMOutOfBandPort;

  //
  // At most 100 input/output/errorout device for console storage
  //
  UINT8   ConsoleCheck[MAX_MENU_NUMBER];

  //
  // Boot or Driver Option Order storage
  // The value is the OptionNumber+1 because the order list value cannot be 0
  // Use UINT32 to hold the potential value 0xFFFF+1=0x10000
  //
  UINT32  OptionOrder[MAX_MENU_NUMBER];

  //
  // Boot or Driver Option Delete storage
  //
  BOOLEAN OptionDel[MAX_MENU_NUMBER];

  //
  // This is the Terminal Attributes value storage
  //
  UINT8   COMBaudRate;
  UINT8   COMDataRate;
  UINT8   COMStopBits;
  UINT8   COMParity;
  UINT8   COMTerminalType;

  //
  // Legacy Device Order Selection Storage
  //
  UINT8   LegacyFD[MAX_MENU_NUMBER];
  UINT8   LegacyHD[MAX_MENU_NUMBER];
  UINT8   LegacyCD[MAX_MENU_NUMBER];
  UINT8   LegacyNET[MAX_MENU_NUMBER];
  UINT8   LegacyBEV[MAX_MENU_NUMBER];

  //
  // We use DisableMap array to record the enable/disable state of each boot device
  // It should be taken as a bit array, from left to right there are totally 256 bits
  // the most left one stands for BBS table item 0, and the most right one stands for item 256
  // If the bit is 1, it means the boot device has been disabled.
  //
  UINT8   DisableMap[32];

  //
  // Console Output Text Mode
  //
  UINT16  ConsoleOutMode;

  //
  //  UINT16                    PadArea[10];
  //
} BMM_FAKE_NV_DATA;

//
// Key used by File Explorer forms
//
#define KEY_VALUE_SAVE_AND_EXIT_BOOT           0x1000
#define KEY_VALUE_NO_SAVE_AND_EXIT_BOOT        0x1001
#define KEY_VALUE_SAVE_AND_EXIT_DRIVER         0x1002
#define KEY_VALUE_NO_SAVE_AND_EXIT_DRIVER      0x1003

///
/// This is the data structure used by File Explorer formset
///
typedef struct {
  UINT16  DescriptionData[75];
  UINT16  OptionalData[127];
  UINT8   Active;
  UINT8   ForceReconnect;
} FILE_EXPLORER_NV_DATA;

#endif

