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


#include <SysMiscCfg.h>
#include "PlatSetupVariable.h"



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




#define MAIN_FORM_SET_CLASS             0x01
#define ADVANCED_FORM_SET_CLASS         0x02
#define DEVICES_FORM_SET_CLASS          0x03
#define BOOT_FORM_SET_CLASS             0x04
#define SECURITY_FORM_SET_CLASS         0x05
#define EXIT_FORM_SET_CLASS             0x06

#define NON_FRONT_PAGE_SUBCLASS         0x00
#define FRONT_PAGE_SUBCLASS             0x02

#define SETUP_DATA_ID                   0x1
#define TSESETUP_DATA_ID                0x2
#define SETUP_VOLATILE_DATA_ID          0x3


#define PLATFORM_SETUP_VARIABLE_FLAG    (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)


#define PCIE_MAX_PAYLOAD_AUTO         0
#define PCIE_MAX_PAYLOAD_128          1
#define PCIE_MAX_PAYLOAD_256          2
#define PCIE_MAX_PAYLOAD_512          3

#define PCIE_ASPM_DISABLE             0
#define PCIE_ASPM_L0S                 1
#define PCIE_ASPM_L1                  2
#define PCIE_ASPM_L0SL1               3

#define PCIE_MAX_READ_REQ_AUTO        0
#define PCIE_MAX_READ_REQ_128         1
#define PCIE_MAX_READ_REQ_256         2
#define PCIE_MAX_READ_REQ_512         3

#define USB_ALL_SUPPORT_MODE                     0
#define USB_HID_SUPPORT_MODE                     1
#define USB_HID_MASS_SUPPORT_MODE                2
#define USB_HID_MASS_READ_ONLY_MODE              3
#define USB_HID_OTHERS_SUPPORT_MODE              4
#define USB_HID_OTHERS_MASS_READ_ONLY_MODE       5
#define USB_MASS_SUPPORT_MODE                    6
#define USB_MASS_READ_ONLY_MODE                  7
#define USB_MASS_OTHERS_SUPPORT_MODE             8
#define USB_MASS_READ_ONLY_OTHERS_SUPPORT_MODE   9
#define USB_OTHERS_SUPPORT_MODE                  10
#define USB_NONE_SUPPORT_MODE                    11

#define TKN_LANG_CHS_NAME              "zh-Hans"

#define CSM_DEFAULT_VALUE                             0
#define USER_BOOT_ORDER_VAR_NAME                      L"UserBootOrder"

#define PASSWORD_MAX_SIZE                             20
#define PLATFORM_SETUP_VARIABLE_NAME                  L"Setup"
#define SETUP_VOLATILE_VARIABLE_NAME                  L"SetupVolatileData"

#define PLATFORM_SETUP_VARIABLE_GUID \
  { \
     0xEC87D643, 0xEBA4, 0x4BB5, 0xA1, 0xE5, 0x3F, 0x3E, 0x36, 0xB2, 0x0D, 0xA9 \
  }


#define DISPLAY_PRIMARY_PCIE         0
#define DISPLAY_PRIMARY_IGD          1
#define DISPLAY_PRIMARY_ALL          2

#define USER_LOGIN_TYPE_ADMIN        0
#define USER_LOGIN_TYPE_POP          1



#pragma pack(1)


typedef struct {

#ifdef ADD_ON_SETUP_DATA_AT_HEADER
  ADD_ON_SETUP_DATA_AT_HEADER
#endif 

  UINT8  Version;
  UINT8  Numlock;
  UINT16 BootTimeout;
  UINT8  UCREnable;
  UINT8  TerminalType;
  UINT8  SerialPortSelect;
  UINT8  SerialBaudrate;
  UINT8  SerialRedirection2;
  UINT8  BootModeType;
  UINT8  NetBootIpVer;
  UINT8  VideoPrimaryAdapter;
  UINT8  ObLanEn;
  UINT8  ShellEn;
  UINT8  VideoRomPolicy;
  UINT8  PxeRomPolicy;
  UINT8  StorageRomPolicy;
  UINT8  OtherRomPolicy;
  UINT8  Above4GDecode;
  UINT8  CpuPstateEn;
  UINT8  UefiNetworkStack;
  UINT8  LanBoot;
  UINT8  NvmeOpRomPriority;
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
  UINT8  PcieMaxPayloadSize;              // reserved.
  UINT8  PcieMaxReadReqSize;  
  UINT8  PcieAspm;
  UINT8  PxeRetryEn;
  UINT8  SataEn;
  UINT8  SriovDis;
  UINT8  StopBits;
  UINT8  Parity;
  UINT8  DataBits;
  UINT8  FlowControl;
  UINT8  SysDebugMode;
  UINT8  NumaDisable;
  UINT8  HddSmartCheckDis;
  UINT8  LegacyUsbDisable;
  UINT8  AesSupportDis;
  UINT8  TpmSelect;
  UINT8  HttpBootDis;
  UINT8  AtaPhyDetectDelay;
  UINT32 UsbWaitPortStableStall;
  UINT8  HygonTpcmEn;
  UINT8  LegacyOpromMessageDis;
  UINT8  RomPolicy;
  UINT8  Csm;
  UINT8  UceResetIso;
  UINT8  ClearUceIsoData;
  UINT8  PcieAerCeMaskSet;
  UINT8  PcieCeMaskBadTLP;
  UINT8  PcieCeMaskBadDll;
  UINT8  PcieCeMaskRollOver;
  UINT8  PcieCeMaskTimeOut;
  UINT8  PcieCeMaskNonFatal;
  UINT8  ErrorActionCtrlMode;
  UINT8  PcieSerrActionCtrl;
  UINT8  PcieIntFatalActionCtrl;
  UINT8  PcieIntCorrActionCtrl;
  UINT8  PcieExtFatalActionCtrl;
  UINT8  PcieExtNonFatalActionCtrl;
  UINT8  PcieExtCorrActionCtrl;
  UINT8  PcieParityErrActionCtrl;
  UINT8  NbifSerrActionCtrl;
  UINT8  NbifIntFatalActionCtrl;
  UINT8  NbifIntNonFatalActionCtrl;
  UINT8  NbifIntCorrActionCtrl;
  UINT8  NbifExtFatalActionCtrl;
  UINT8  NbifExtNonFatalActionCtrl;
  UINT8  NbifExtCorrActionCtrl;
  UINT8  NbifParityErrActionCtrl;
  UINT8  MemCeTinyStormTC;
  UINT8  NmiSyncFloodPinEn;
  UINT8  ResetCpuOnSyncFloodDis;
  UINT8  ApeiEinjDis;
  UINT8  ApeiSupportDis;
  UINT16 MemCeThresh;
  UINT16 MemCeTinyStormSec;
  UINT8  RasCeSmiThreshold;
  UINT8  EinjCpuCeDis;
  UINT16 LeakybucketMinites;
  UINT16 LeakybucketOnceCount;
  UINT8  Leakybucket24HClear;
  UINT16 PcieCeThreshold;
  UINT16 McaNonMemErrThresh;
  UINT8  QuietBootMode;
  UINT8  S4Support;
  UINT8  HpetDis;
  UINT8  MyReserved[62];
  
#ifdef ADD_ON_SETUP_DATA
  ADD_ON_SETUP_DATA
#endif 
  
}SETUP_DATA;


// warning: below data should auto align, DO NOT leave hole between members.
// suggest use UINT16 all.
typedef struct {
  UINT16  PlatId;
  UINT8   LoginUserType;
  UINT8   CpuSockets;
  UINT8   Cpu0Dimms;
  UINT8   Cpu1Dimms;
  UINT8   BmcPresent;
  UINT8   CsmOn;
  UINT8   TpcmSupport;

#ifdef ADD_ON_SETUP_VOLATILE_DATA
  ADD_ON_SETUP_VOLATILE_DATA
#endif 

} SETUP_VOLATILE_DATA;

#pragma pack()


#endif  // #ifndef _SETUP_VARIABLE_H

