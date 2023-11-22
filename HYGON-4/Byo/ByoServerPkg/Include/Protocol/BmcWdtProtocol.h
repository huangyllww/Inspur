/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _EFI_BMC_WDT_H_
#define _EFI_BMC_WDT_H_

#define EFI_BMC_WATCHDOG_PROTOCOL_GUID \
  { \
    0xcdf19dc9, 0x4944, 0x49d3, { 0xb3, 0x2e, 0xb1, 0x61, 0x98, 0x8a, 0x19, 0xce } \
  }

typedef struct _EFI_BMC_WDT_PROTOCOL  EFI_BMC_WDT_PROTOCOL;

#pragma pack(1)
typedef struct {
  union {
    struct {
      UINT8 Timer     : 3;
      UINT8 Reserved  : 3;
      UINT8 DontStop  : 1;
      UINT8 DontLog   : 1;
    } Bit;
    UINT8 RawData;
  } TimerUse;  

  union {
    struct {
      UINT8 TimeoutAction : 3;
      UINT8 Reserved1     : 1;
      UINT8 PreTimeoutInt : 3;
      UINT8 Reserved2     : 1;
    } Bit;
    UINT8 RawData;
  } TimerActions;

  UINT8 PreTimeoutInterval;

  union {
    struct {
      UINT8 Reserved1 : 1;
      UINT8 BiosFrb2  : 1;
      UINT8 BiosPost  : 1;
      UINT8 OsLoad    : 1;
      UINT8 SmsOs     : 1;
      UINT8 Oem       : 1;
      UINT8 Reserved2 : 1;
      UINT8 Reserved3 : 1;
    } Bit;
    UINT8 RawData;
  } ExpirationFlag;

  UINT16 InitialCountdown;
  UINT16 PresentCountdown;
} EFI_BMC_WDT_DATA;
#pragma pack()

typedef struct {
  BOOLEAN                     Enable;
  UINT8                       Action;
  UINT16                      Timeout;
} EFI_BMC_WDT_CFG;


typedef
EFI_STATUS
(EFIAPI *EFI_BMC_WDT_SET_POLICY) (
  IN  EFI_BMC_WDT_PROTOCOL  *This,
  IN  EFI_BMC_WDT_CFG       *Data
  );

typedef
EFI_STATUS
(EFIAPI *EFI_BMC_WDT_GET_POLICY) (
  IN  EFI_BMC_WDT_PROTOCOL  *This,
  OUT EFI_BMC_WDT_CFG       *Data
  );

typedef
EFI_STATUS
(EFIAPI *EFI_BMC_WDT_RESET_POLICY) (
  IN  EFI_BMC_WDT_PROTOCOL  *This
  );


struct _EFI_BMC_WDT_PROTOCOL {
  EFI_BMC_WDT_SET_POLICY         SetWatchdog;
  EFI_BMC_WDT_GET_POLICY         GetWatchdog;
  EFI_BMC_WDT_RESET_POLICY       ResetWatchdog;
};

extern EFI_GUID   gEfiBmcWdtProtocolGuid;

#endif
