/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SetupVariable.h

Abstract:
  The declaration header file for SYSTEM_CONFIGURATION_GUID.

Revision History:

$END--------------------------------------------------------------------

**/

#ifndef _SETUP_VARIABLE_H
#define _SETUP_VARIABLE_H

#include <Token.h>


#define TSE_SETUP_ENTER_GUID \
  { 0x71202EEE, 0x5F53, 0x40d9, 0xAB, 0x3D, 0x9E, 0x0C, 0x26, 0xD9, 0x66, 0x57 }

#define EFI_IFR_FRONT_PAGE_GUID \
  { 0xe58809f8, 0xfbc1, 0x48e2, { 0x88, 0x3a, 0xa3, 0xf, 0xdc, 0x4b, 0x44, 0x1e } }

#define MAIN_FORM_SET_GUID  \
  { \
    0x985eee91, 0xbcac, 0x4238, { 0x87, 0x78, 0x57, 0xef, 0xdc, 0x93, 0xf2, 0x4e } \
  }

#define ADVANCED_FORM_SET_GUID \
  { \
    0xe14f04fa, 0x8706, 0x4353, { 0x92, 0xf2, 0x9c, 0x24, 0x24, 0x74, 0x6f, 0x9f } \
  }

#define DEVICES_FORM_SET_GUID \
  { \
    0xadfe34c8, 0x9ae1, 0x4f8f, { 0xbe, 0x13, 0xcf, 0x96, 0xa2, 0xcb, 0x2c, 0x5b } \
  }

#define POWER_FORM_SET_GUID \
  { \
    0x5b5eb989, 0x4702, 0x47c5, { 0xbb, 0xe0, 0x4, 0xb9, 0x99, 0xf6, 0x2, 0x1e } \
  }

#define BOOT_FORM_SET_GUID \
  { \
    0x8b33ffe0, 0xd71c, 0x4f82, { 0x9c, 0xeb, 0xc9, 0x70, 0x58, 0xc1, 0x3f, 0x8e } \
  }

#define SECURITY_FORM_SET_GUID \
  { \
    0x981ceaee, 0x931c, 0x4a17, { 0xb9, 0xc8, 0x66, 0xc7, 0xbc, 0xfd, 0x77, 0xe1 } \
  }

#define EXIT_FORM_SET_GUID \
  { \
    0xa43b03dc, 0xc18a, 0x41b1, { 0x91, 0xc8, 0x3f, 0xf9, 0xaa, 0xa2, 0x57, 0x13 } \
  }

#define HDD_PASSWORD_CONFIG_GUID \
  { \
    0xd5fd1546, 0x22c5, 0x4c2e, { 0x96, 0x9f, 0x27, 0x3c, 0x0, 0x77, 0x10, 0x80 } \
  }

#define SECUREBOOT_CONFIG_FORM_SET_GUID \
  { \
    0x5daf50a5, 0xea81, 0x4de2, {0x8f, 0x9b, 0xca, 0xbd, 0xa9, 0xcf, 0x5c, 0x14} \
  }

#define SETUP_PCI_LIST_FORMSET_GUID \
  { 0x357f4a0c, 0xadea, 0x4e07, { 0x86, 0x48, 0x73, 0x5a, 0x55, 0x1d, 0x82, 0xd4 } }

#define FORMSET_ID_GUID_AMD_CBS \
  { \
    0xB04535E3, 0x3004, 0x4946, {0x9E, 0xB7, 0x14, 0x94, 0x28, 0x98, 0x30, 0x53} \
  }

#define SETUP_BMC_CFG_GUID \
  { \
    0x8236697e, 0xf0f6, 0x405f, 0x99, 0x13, 0xac, 0xbc, 0x50, 0xaa, 0x45, 0xd1 \
  }
 

#define MAIN_FORM_SET_CLASS             0x01
#define ADVANCED_FORM_SET_CLASS         0x02
#define DEVICES_FORM_SET_CLASS          0x03
#define BOOT_FORM_SET_CLASS             0x04
#define SECURITY_FORM_SET_CLASS         0x05
#define EXIT_FORM_SET_CLASS             0x06
//#define POWER_FORM_SET_CLASS            0x08

#define NON_FRONT_PAGE_SUBCLASS         0x00
#define FRONT_PAGE_SUBCLASS             0x02

#define SETUP_DATA_ID                   0x1
#define TSESETUP_DATA_ID                0x2
#define SETUP_VOLATILE_DATA_ID          0x3


#define PLATFORM_SETUP_VARIABLE_FLAG    (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)





#define DEFAULT_BOOT_TIMEOUT                          5
#define PASSWORD_MAX_SIZE                             20
#define PLATFORM_SETUP_VARIABLE_NAME                  L"Setup"
#define SETUP_VOLATILE_VARIABLE_NAME                  L"SetupVolatileData"
#define SETUP_TEMP_VARIABLE_NAME                      L"SetupTemporary"

#define PLATFORM_SETUP_VARIABLE_GUID \
  { \
     0xEC87D643, 0xEBA4, 0x4BB5, 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 \
  }

//
// warning !!!
// after power loss value now is register value,
// if you want to change it to new platform, PLEASE check register manual first!!!
//
#define AFTER_POWER_LOSS_OFF         0
#define AFTER_POWER_LOSS_ON          1
#define AFTER_POWER_LOSS_PREVIOUS    3

#define DISPLAY_PRIMARY_PCIE         0
#define DISPLAY_PRIMARY_IGD          1

#define NET_BOOT_IP_ALL              0
#define NET_BOOT_IP_V4               1
#define NET_BOOT_IP_V6               2

#define USER_LOGIN_TYPE_ADMIN        0
#define USER_LOGIN_TYPE_POP          1



#pragma pack(1)


typedef struct {

  UINT8  Version;

  UINT8  Numlock;
  UINT16 BootTimeout;
  UINT8  OpromMessage;

  UINT8  UCREnable;
  UINT8  TerminalType;
  UINT8  SerialPortSelect;
  UINT8  SerialBaudrate;

  UINT8  Csm;
  UINT8  BootModeType;
  UINT8  NetBootIpVer;


  UINT8  VideoDualVga;  
  UINT8  VideoPrimaryAdapter;  
  UINT8  ObLanEn;
  UINT8  ObLanBoot;
  UINT8  WakeOnLan;
  UINT8  AcpiSleepStatus;
  UINT8  ShellEn;
  
  UINT8  VideoRomPolicy;
  UINT8  PxeRomPolicy;
  UINT8  StorageRomPolicy;
  UINT8  OtherRomPolicy;
  UINT8  UmaAbove4G;
  UINT8  AmdSmtMode;
  UINT8  QuiteBoot;
  UINT8  NvmeDeviceSupport;
  UINT8  Pfeh;
  UINT16 McaErrThreshCount;

  UINT8  SATAPort[32];
  UINT8  SATAPortSpeed[32];
  UINT8  Die0XHCIPort[4];
  UINT8  Die1XHCIPort[4];
  UINT8  Die4XHCIPort[4];
  UINT8  Die5XHCIPort[4];
  UINT8  AMDPstate;
  UINT8  AMDCstate;
  UINT8  PcieAri;

  UINT8  UefiNetworkStack;
  UINT8  LanBoot;
  UINT8  NvmeOpRomPriority;
 // UINT8  AfterPowerLoss;

  EFI_HII_TIME  RTCWakeupTime;
  UINT8  UserDefMon;
  UINT8  UserDefTue;
  UINT8  UserDefWed;
  UINT8  UserDefThu;
  UINT8  UserDefFri;
  UINT8  UserDefSat;
  UINT8  UserDefSun;

  UINT8  UsbMassSupport;

  UINT8  PcieWake;
  UINT8  SriovDis;
  UINT8  RetryPxeBoot;
  UINT8  CpuSpeed;
  UINT8  CbsCmnSVMCtrl;                                    ///Enable/Disable SMEE
  UINT8  CbsCmnGnbNbIOMMU;                                 ///< IOMMU
  UINT8  LoginUserType;

  UINT8  ScreenPrintNum;
  UINT8  TpmSelect;

  UINT8  AesSupport;

  UINT8  LastBootModeType;
  UINT8  CommReserved[12];/*Do not modify the order of variables. If you need to add variables, please add them in the line before CommReserved!*/

#ifdef ADD_ON_SETUP_DATA
  ADD_ON_SETUP_DATA
#endif  
}SETUP_DATA;


// warning: below data should auto align, DO NOT leave hole between members.
// suggest use UINT16 all.
typedef struct {
  UINT8   PlatId;
  UINT8   CpuDies;
  UINT8   SataHosts;
  UINT8   BmcPresent;
  UINT8  AdvanceControl;

#ifdef ADD_ON_SETUP_VOLATILE_DATA
  ADD_ON_SETUP_VOLATILE_DATA
#endif  
  
} SETUP_VOLATILE_DATA;

#pragma pack()


#endif  // #ifndef _SETUP_VARIABLE_H

