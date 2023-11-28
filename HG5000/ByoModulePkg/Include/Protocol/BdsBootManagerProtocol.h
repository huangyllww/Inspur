
#ifndef __BDS_BOOT_MANAGER_PROTOCOL_H__
#define __BDS_BOOT_MANAGER_PROTOCOL_H__

#include <UefiBootManagerLoadOption.h>

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
EFI_STATUS
(EFIAPI *BYO_DRAW_PASSWORD_CALLBACK)(
    IN CHAR16  *Password
  );

#define PASSWD_REASON_NONE        0
#define PASSWD_REASON_WRONG_OLD   1
#define PASSWD_REASON_NOT_EQUAL   2
#define PASSWD_REASON_USER_CANCEL 3
#define PASSWD_REASON_EMPTY       4

typedef
EFI_STATUS
(EFIAPI *BYO_DRAW_PASSWORD_DIALOG) (
  IN OUT CHAR16                      *NewPassword,
  IN     UINTN                       PasswordSize,
  IN     BYO_DRAW_PASSWORD_CALLBACK  Callback,
  IN     CHAR16                      *Title,
  OUT    UINTN                       *Reason
  );

typedef
EFI_STATUS
(EFIAPI *BYO_DRAW_POST_PASSWORD_DIALOG) (
  IN  CHAR16                     *TitleStr,
  IN  CHAR16                     *Prompt,
  OUT CHAR16                     *Password,
  IN  UINTN                      PasswordSize
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
  BYO_DRAW_PASSWORD_DIALOG                  DrawPwdDlg;
  BYO_DRAW_POST_PASSWORD_DIALOG             DrawPostPwdDlg;
  BYO_PRINT_POST_STRING                     PrintPostString;
  BYO_GET_EXT_BM_BOOT_NAME                  GetExtBmBootName;

  EFI_BOOT_MANAGER_LOAD_OPTION              *UefiOptions;
  UINTN                                     UefiOptionsCount;
  EFI_BOOT_MANAGER_LOAD_OPTION              *LegacyOptions;
  UINTN                                     LegacyOptionsCount;    
};  
  
extern EFI_GUID gEfiBootManagerProtocolGuid;

#endif


