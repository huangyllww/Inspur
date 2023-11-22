/** @file
#
#Copyright (c) 2017, Byosoft Corporation.<BR>
#All rights reserved.This software and associated documentation (if any)
#is furnished under a license and may only be used or copied in
#accordance with the terms of the license. Except as permitted by such
#license, no part of this software or documentation may be reproduced,
#stored in a retrieval system, or transmitted in any form or by any
#means without the express written consent of Byosoft Corporation.
#
#File Name:
#  IpmiBootOption.h
#
#Abstract:
#  ServerMgmt features via IPMI commands (IPMI2.0 Spc rev1.1 2013)
#
#Revision History:
#
#TIME:       2017-7-5
#$AUTHOR:    Phinux Qin
#$REVIEWERS:
#$SCOPE:     All BMC Platforms
#$TECHNICAL:
#
#
#$END--------------------------------------------------------------------
#
--*/
#ifndef _IPMI_BOOT_OPTIONS_P_H_
#define _IPMI_BOOT_OPTIONS_P_H_

//
//IPMI BOOT OPTIONS PROTOCOL GUID.
//
  // {49b21dac-d7a8-4b80-ac22-573dc2aabffd} 
#define IPMI_BOOT_OPTIONS_PROTOCOL_GUID \
  { \
    0x49b21dac, 0xd7a8, 0x4b80, {0xac, 0x22, 0x57, 0x3d, 0xc2, 0xaa, 0xbf, 0xfd} \
  }

typedef struct _IPMI_BOOT_OPTIONS_PROTOCOL    IPMI_BOOT_OPTIONS_PROTOCOL;

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiIpmiBootOptionGuid;


typedef
EFI_STATUS
(EFIAPI *IPMI_SYSTEM_BOOT_OPTION5) (
  IN IPMI_BOOT_OPTIONS_PROTOCOL *This
 );

//
// IPMI BOOT OPTION Protocol.
//

typedef struct _IPMI_BOOT_OPTIONS_PROTOCOL {
  IPMI_SYSTEM_BOOT_OPTION5     ProcessIpmiSystemBootOptionParameter5;
} IPMI_BOOT_OPTIONS_PROTOCOL;

typedef enum {
  FORCE_PXE = 0x20, //To avoid to conflict with BBS Boot Device type
  LOCAL_HDD,
  LOCAL_DVD,
  FORCE_SETUP,
  REMOTE_FLOPPY,
  REMOTE_IMAGE_FILE,
  REMOTE_DVD,
  REMOTE_HDD,
  LOCAL_FLOPPY
}IPMI_BOOT_DEVICE;

#pragma pack(1)

typedef struct {
  UINT8    ParameterVersion;
  UINT8    ParameterValid;
} SYSTEM_BOOT_OPTION_RESPONDE_COMMON_HEAD;

typedef union {
  struct {
    UINT8    Status : 2;
    UINT8    Resv : 6;
  } Bits;

  UINT8    Data;
} SYSTEM_BOOT_OPTION_PARAMETER1;

typedef struct {
  UINT8    WriteMask;
  UINT8    BootInitiatorAck;
} SYSTEM_BOOT_OPTION_PARAMETER4;

typedef struct {
   union {
     struct {
      UINT8    Resv : 5;
      UINT8    BiosBootType : 1;
      UINT8    Persistent : 1;
      UINT8    BootFlagValid : 1;
    } Bits;
    UINT8    Data;
  } Data1;

  union {
     struct {
      UINT8    LockOutResetButton : 1;
      UINT8    ScreenBlank : 1;
      UINT8    BootDeviceSelector : 4;
      UINT8    LockKeyboard : 1;
      UINT8    CmosClear : 1;
    } Bits;
    UINT8    Data;
  } Data2;

  union {
     struct {
      UINT8    ConsoleRedictionControl : 2;
      UINT8    LockOutSleepButton : 1;
      UINT8    UserPasswordBypass : 1;
      UINT8    ForceProgressEventTrap : 1;
      UINT8    BiosVerbosity : 2;
      UINT8    LockOutPowerButton : 1;
    } Bits;
    UINT8    Data;
  } Data3;

  union {
     struct {
      UINT8    BiosMuxControlOverride : 3;
      UINT8    BiosShareModeOverride : 1;
      UINT8    Resv : 4;
    } Bits;
    UINT8    Data;
  } Data4;

  union {
     struct {
      UINT8    DeviceInstanceSelector : 5;
      UINT8    Resv : 3;
    } Bits;
    UINT8    Data;
  } Data5;
} SYSTEM_BOOT_OPTION_PARAMETER5;
#pragma pack()

#endif
