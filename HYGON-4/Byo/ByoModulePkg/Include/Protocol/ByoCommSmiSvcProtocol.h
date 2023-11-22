/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BYO_COMM_SMI_SVR_PROTOCOL_H__
#define __BYO_COMM_SMI_SVR_PROTOCOL_H__

typedef struct _BYO_COMM_SMI_SVC_PROTOCOL BYO_COMM_SMI_SVC_PROTOCOL;


enum {
    UDT_UNKNOW   = 0,
    UDT_KEYBOARD = 1,
    UDT_MOUSE    = 2,
    UDT_HUB      = 3,
    UDT_MASS_STORAGE = 4,
};


typedef
EFI_STATUS
(EFIAPI *GET_CURRENT_TIME)(
    IN EFI_TIME   *Time
  );

typedef
EFI_STATUS
(EFIAPI *RESET_SYSTEM_PASSWORD)(
    VOID
  );

#define SET_TIME_FLAG_ALL     0
#define SET_TIME_FLAG_TIME    1
#define SET_TIME_FLAG_DATE    2

typedef
EFI_STATUS
(EFIAPI *SET_CURRENT_TIME)(
    IN EFI_TIME   *Time,
    IN UINTN      SetFlag
  );

typedef
EFI_STATUS
(EFIAPI *GENERATE_UUID)(
    IN OUT EFI_GUID *Guid
  );

typedef
VOID
(EFIAPI *ACPI_ON_SWSMI_HOOK)(
    VOID
  );

typedef
VOID
(EFIAPI *S3_RESUME_SWSMI_HOOK)(
    VOID
  );

typedef
EFI_STATUS
(EFIAPI *GENERATE_UUID_BY_SN)(
    IN     CHAR8    *Sn,
    IN OUT EFI_GUID *Guid
  );

typedef
VOID
(EFIAPI *SMI_SLEEP_CALLBACK)(
    IN     UINT8 SleepType
  );

typedef
EFI_STATUS
(EFIAPI *UPDATE_SETUP_DATA)(
    IN     VOID   *SetupData,
    IN     UINTN  SetupDataSize
  );

typedef
EFI_STATUS
(EFIAPI *GET_BIOS_FEATURE_CODE)(
    OUT UINT8    *FeatrueCode,
        VOID     *FvStart
  );

typedef
EFI_STATUS
(EFIAPI *BYO_USB_FILTER)(
  IN UINTN                             DevType,
  IN EFI_DEVICE_PATH_PROTOCOL          *DevPath
  );

typedef
EFI_STATUS
(EFIAPI *BYO_USB_FILTER2)(
  IN EFI_DEVICE_PATH_PROTOCOL          *DevPath
  );

typedef
EFI_STATUS
(EFIAPI *BYO_DISABLE_POWER_BUTTON)(
  IN BOOLEAN                           Disable
  );


struct _BYO_COMM_SMI_SVC_PROTOCOL {
  GET_CURRENT_TIME         GetCurrentTime;
  RESET_SYSTEM_PASSWORD    ResetSysPassword;
  SET_CURRENT_TIME         SetCurrentTime;
  GENERATE_UUID            GenUuid;
  ACPI_ON_SWSMI_HOOK       AcpiOnHook;
  S3_RESUME_SWSMI_HOOK     S3ResumeHook;
  GENERATE_UUID_BY_SN      GenUuidBySn;
  SMI_SLEEP_CALLBACK       SleepCallback;
  UPDATE_SETUP_DATA        UpdateSetupData;
  GET_BIOS_FEATURE_CODE    GetBiosFeatureCode;
  BYO_USB_FILTER           UsbFilter;
  BYO_USB_FILTER2          UsbFilter2; 
  BYO_DISABLE_POWER_BUTTON DisablePowerButton;
};


extern EFI_GUID gByoCommSmiSvcProtocolGuid;

#endif

