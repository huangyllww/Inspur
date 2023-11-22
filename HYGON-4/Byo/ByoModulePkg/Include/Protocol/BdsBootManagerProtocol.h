/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __BDS_BOOT_MANAGER_PROTOCOL_H__
#define __BDS_BOOT_MANAGER_PROTOCOL_H__

#include <Library/UefiBootManagerLib.h>

typedef struct _EFI_BDS_BOOT_MANAGER_PROTOCOL EFI_BDS_BOOT_MANAGER_PROTOCOL;


typedef  
VOID
(EFIAPI *EFI_BOOT_MANAGER_CONNECT_ALL) (
    VOID
  );

typedef  
VOID
(EFIAPI *EFI_BOOT_MANAGER_REFRESH_ALL_BOOT_OPTION) (
    VOID
  );  

typedef   
EFI_STATUS
(EFIAPI *EFI_BOOT_MANAGER_GET_LOAD_OPTIONS) (
  OUT EFI_BOOT_MANAGER_LOAD_OPTION   **Option,
  OUT UINTN                          *OptionCount
  );

typedef   
EFI_STATUS
(EFIAPI *EFI_BOOT_MANAGER_FREE_LOAD_OPTIONS) (
  IN EFI_BOOT_MANAGER_LOAD_OPTION   *Option,
  IN UINTN                          OptionCount
  );

typedef
EFI_STATUS
(EFIAPI *EFI_BOOT_MANAGER_CREATE_SETUP_BOOT_OPTION) (
  OUT EFI_BOOT_MANAGER_LOAD_OPTION   *SetupOption
);

typedef
EFI_STATUS
(EFIAPI *EFI_BOOT_MANAGER_CREATE_SHELL_BOOT_OPTION) (
  OUT EFI_BOOT_MANAGER_LOAD_OPTION   *ShellOption
);

typedef  
VOID
(EFIAPI *EFI_BOOT_MANAGER_BOOT)(
  IN  EFI_BOOT_MANAGER_LOAD_OPTION   *BootOption
  );

typedef
VOID *
(EFIAPI *EFI_BOOT_MANAGER_GET_VARIABLE_AND_SIZE) (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize
  );

typedef
EFI_STATUS
(EFIAPI *EFI_BOOT_MANAGER_BOOT_APP) (
  IN  EFI_GUID            *AppGuid
  );

typedef
EFI_STATUS
(EFIAPI *EFI_BOOT_MANAGER_BOOT_SETUP) (
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *This
  );

typedef
EFI_STATUS
(EFIAPI *EFI_BOOT_MANAGER_BOOT_APP_PARAM) (
  IN  EFI_GUID            *AppGuid,
  IN  VOID                *Parameter,
  IN  UINTN               ParameterSize,
  IN  BOOLEAN             IsBootCategory
  );

typedef
EFI_STATUS
(EFIAPI *EFI_BOOT_MANAGER_BOOT_FILE_PARAM) (
  IN  EFI_DEVICE_PATH_PROTOCOL   *FileDp,
  IN  VOID                       *Parameter,
  IN  UINTN                      ParameterSize,
  IN  BOOLEAN                    IsBootCategory
  );

typedef
VOID *
(EFIAPI *EFI_BOOT_MANAGER_GET_VAR_SIZE_ATTRIBUTE) (
  IN  CHAR16              *Name,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableSize,
  OUT UINT32              *Attribute
  );

typedef
UINTN
(EFIAPI *EFI_BOOT_MANAGER_GET_CURRENT_HOTKEY) (
  VOID
  );

typedef
VOID
(EFIAPI *EFI_BOOT_MANAGER_SET_CURRENT_HOTKEY) (
  UINTN   HotKey
  );


typedef
VOID
(EFIAPI *BYO_PRINT_POST_STRING) (
  IN UINTN     Row,
  IN UINTN     Column,
  IN CHAR16    *String,
  IN UINTN     Attribute           // color, 0:keep
  );

typedef 
CHAR16 * 
(EFIAPI *BYO_GET_EXT_BM_BOOT_NAME)(
  IN  CHAR16                   *BootName, 
  IN  EFI_DEVICE_PATH_PROTOCOL *Dp,
  IN  UINT8                    *OptionalData, 
  IN  UINT32                   OptionalDataSize,
  OUT BOOLEAN                  *Updated
  );


struct _EFI_BDS_BOOT_MANAGER_PROTOCOL {
  EFI_BOOT_MANAGER_CONNECT_ALL              ConnectAll;
  EFI_BOOT_MANAGER_REFRESH_ALL_BOOT_OPTION  RefreshOptions;
  EFI_BOOT_MANAGER_GET_LOAD_OPTIONS         GetOptions;
  EFI_BOOT_MANAGER_FREE_LOAD_OPTIONS        FreeOptions;
  EFI_BOOT_MANAGER_CREATE_SETUP_BOOT_OPTION CreateSetupOption;
  EFI_BOOT_MANAGER_CREATE_SHELL_BOOT_OPTION CreateShellOption;  
  EFI_BOOT_MANAGER_BOOT                     Boot;
  EFI_BOOT_MANAGER_GET_VARIABLE_AND_SIZE    GetVarAndSize;
  EFI_BOOT_MANAGER_BOOT_APP                 BootApp;
  EFI_BOOT_MANAGER_BOOT_SETUP               BootSetup;
  EFI_BOOT_MANAGER_BOOT_APP_PARAM           BootAppParam;
  EFI_BOOT_MANAGER_BOOT_FILE_PARAM          BootFileParam;
  EFI_BOOT_MANAGER_GET_VAR_SIZE_ATTRIBUTE   GetVarSizeAttrib;
  EFI_BOOT_MANAGER_GET_CURRENT_HOTKEY       GetCurHotKey;
  EFI_BOOT_MANAGER_SET_CURRENT_HOTKEY       SetCurHotKey;
  BYO_PRINT_POST_STRING                     PrintPostString;
  BYO_GET_EXT_BM_BOOT_NAME                  GetExtBmBootName;

  EFI_BOOT_MANAGER_LOAD_OPTION              *UefiOptions;
  UINTN                                     UefiOptionsCount;
  EFI_BOOT_MANAGER_LOAD_OPTION              *LegacyOptions;
  UINTN                                     LegacyOptionsCount; 
  EFI_BOOT_MANAGER_LOAD_OPTION              *BootOptions;
  UINTN                                     BootOptionsCount;    
};  
  
extern EFI_GUID gEfiBootManagerProtocolGuid;

#endif


