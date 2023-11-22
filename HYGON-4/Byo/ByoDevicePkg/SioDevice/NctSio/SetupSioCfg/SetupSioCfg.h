/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SetupSioCfg.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#ifndef _EFI_SETUP_SIO_CFG_H
#define _EFI_SETUP_SIO_CFG_H
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/IoLib.h>
#include <Library/SuperIoLib.h>

#include <Protocol/HiiString.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/SetupSaveNotify.h>
#include <Guid/SioSetupDataStruct.h>

#include "NctReg.h"


extern EFI_GUID gEfiSetupEnterGuid;
extern EFI_HII_HANDLE HiiHandle;
extern unsigned char SetupSioCfgVfrBin[];
extern unsigned char SetupSioCfgStrings[];

#define SETUP_VOLATILE_VARIABLE_NAME                  L"SetupVolatileData"


#pragma pack(1)
///
/// HII specific Vendor Device Path definition.
///
typedef struct {
  VENDOR_DEVICE_PATH             VendorDevicePath;
  EFI_DEVICE_PATH_PROTOCOL       End;
} HII_VENDOR_DEVICE_PATH;
#pragma pack()

typedef struct {
  UINTN                            Signature;
  EFI_HANDLE                       DriverHandle;
  EFI_HII_HANDLE                   HiiHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;
  SETUP_SAVE_NOTIFY_PROTOCOL       SetupSaveNotify;
  SIO_SETUP_CONFIG                 ConfigData;  
} SIO_SETUP_CONFIG_PRIVATE_DATA;


#define SIO_SETUP_CONFIG_PRIVATE_SIGNATURE SIGNATURE_32 ('B', 'S', 'I', 'O')


#define SIO_SETUP_CONFIG_PRIVATE_FROM_THIS_HII(a)  \
  CR(a, SIO_SETUP_CONFIG_PRIVATE_DATA, ConfigAccess, SIO_SETUP_CONFIG_PRIVATE_SIGNATURE)


EFI_STATUS
EFIAPI
SetupSioFormRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );


EFI_STATUS
EFIAPI
SetupSioFormExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );
  
EFI_STATUS
EFIAPI
SetupSioFormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  );


EFI_STATUS 
SioNvSaveValue(  
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
SioNvDiscardValue(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
SioNvLoadDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
SioNvSaveUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
SioNvLoadUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS 
SioIsNvDataChanged(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This,
  BOOLEAN                       *IsDataChanged
  );


#endif
