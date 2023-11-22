/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#ifndef __PLAT_HOST_INFO_PROTOCOL_H__
#define __PLAT_HOST_INFO_PROTOCOL_H__

#include <Uefi.h>
#include <Protocol/DevicePath.h>
#include <Protocol/PciIo.h>
#include <Protocol/SimpleTextIn.h>
#include <PlatHostDevicePath.h>
#include <PciDevicePath.h>



typedef struct _PLAT_HOST_INFO_PROTOCOL  PLAT_HOST_INFO_PROTOCOL;

typedef
BOOLEAN
(EFIAPI *LEGACY_OPROM_RUN_CHECK)(
  IN EFI_HANDLE               Handle,
  IN EFI_PCI_IO_PROTOCOL      *PciIo,
  IN PLAT_HOST_INFO_PROTOCOL  *HostInfo
  );

typedef struct {
  BOOLEAN                 Enable;
  UINT16                  VendorId;
  UINT16                  DeviceId;
  EFI_GUID                *RomImageGuid;
  LEGACY_OPROM_RUN_CHECK  RunCheck;
} ADDITIONAL_ROM_TABLE;



typedef enum {
  PLATFORM_HOST_SATA,
  PLATFORM_HOST_LAN,
  PLATFORM_HOST_PCIE,
  PLATFORM_HOST_SDIO,
  PLATFORM_HOST_IGD,
  PLATFORM_HOST_AUDIO,
  PLATFORM_HOST_NVME,
  PLATFORM_HOST_EX_SATA,  
  PLATFORM_HOST_USB,
  PLATFORM_HOST_OCP,
  PLATFORM_HOST_MAX
} PLATFORM_HOST_TYPE;


typedef struct {
  UINTN                     DpSize;
  UINT16                    HostIndex;   // 0 base 
  UINT16                    PortCount;
} PLATFORM_HOST_INFO_SATA_CTX;

typedef struct {
  CHAR8                     *SlotName;
  UINT8                     SlotType;
  UINT8                     SlotbusWidth;
  UINT8                     SlotUsage;
  UINT8                     SlotLen;
  UINT16                    SlotId;
} PLATFORM_HOST_INFO_PCIE_CTX;

typedef struct {
  PLATFORM_HOST_INFO_PCIE_CTX PcieCtx;
  UINT16                      NvmeIndex;
  UINT16                      DpSize;
} PLATFORM_HOST_INFO_NVME_CTX;

typedef struct {
  PLATFORM_HOST_INFO_PCIE_CTX PcieCtx;
  PLATFORM_HOST_INFO_SATA_CTX SataCtx;
} PLATFORM_HOST_INFO_EX_SATA_CTX;

typedef struct {
  PLATFORM_HOST_TYPE        HostType;
  EFI_DEVICE_PATH_PROTOCOL  *Dp;
  VOID                      *HostCtx;
} PLATFORM_HOST_INFO;


// return 0xFFFF - ERROR
// return 0x8000 - only one host
// return others - OK
typedef
UINT16
(EFIAPI *GET_PLAT_SATA_HOST_INDEX)(
  EFI_HANDLE          Handle
  );

typedef
UINT16
(EFIAPI *GET_PLAT_SATA_PORT_INDEX)(
  EFI_HANDLE          Handle
  );


typedef
VOID
(EFIAPI *UPDATE_BOOT_OPTION)(
  VOID                          **BootOptions,
  UINTN                         *BootOptionCount
  );

typedef
EFI_STATUS
(EFIAPI *UPDATE_TOLUM_VAR)(
  UINTN     BitsOfAlignment,
  UINT64    AddrLen
  );

typedef
EFI_DEVICE_PATH_PROTOCOL*
(EFIAPI *GET_PLAT_UCR_DP)(
  UINTN   *DpSize,  OPTIONAL
  UINT16  *IoBase   OPTIONAL
  );



#define HOTKEY_BOOT_NONE              0
#define HOTKEY_BOOT_SETUP             1
#define HOTKEY_BOOT_MENU              2
#define HOTKEY_BOOT_PASS              3
#define HOTKEY_BOOT_PXE               4
#define HOTKEY_BOOT_WINDOWS_RECOVERY  5
#define HOTKEY_BOOT_OS_BACKUP_RESTORE 6

#define HOTKEY_BOOT_OEM1              0x80
#define HOTKEY_BOOT_OEM2              0x81
#define HOTKEY_BOOT_OEM3              0x82
#define HOTKEY_BOOT_OEM4              0x83
#define HOTKEY_BOOT_OEM5              0x84
#define HOTKEY_BOOT_OEM6              0x85
#define HOTKEY_BOOT_OEM7              0x86
#define HOTKEY_BOOT_OEM8              0x87
#define HOTKEY_BOOT_OEM9              0x88
#define HOTKEY_BOOT_OEM10             0x89


#define HOTKEY_ATTRIBUTE_NO_PROMPT          BIT0
#define HOTKEY_ATTRIBUTE_OEM_HANDLER        BIT1
#define HOTKEY_ATTRIBUTE_DISABLED           BIT2
#define HOTKEY_ATTRIBUTE_NO_TIMEOUT_PROMPT  BIT3
#define HOTKEY_ATTRIBUTE_NO_PROGRESS_BAR    BIT4
#define HOTKEY_ATTRIBUTE_NO_LOGIN           BIT5
#define HOTKEY_ATTRIBUTE_OEM_DRAW           BIT6

#define HOTKEY_ATTRIBUTE_GLOBAL_MASK        (HOTKEY_ATTRIBUTE_NO_TIMEOUT_PROMPT | HOTKEY_ATTRIBUTE_NO_PROGRESS_BAR)


// sometimes, OEM may want to check CTRL or SHIFT key also, so give a function
// for advanced check.
typedef
BOOLEAN
(EFIAPI *IS_HOT_KEY_MATCH)(
    IN CONST EFI_KEY_DATA  *KeyData
  );

typedef
VOID
(EFIAPI *OEM_HOT_KEY_HANDLER)(
    IN CONST EFI_KEY_DATA  *KeyData
  );

typedef struct {
  EFI_INPUT_KEY        Key;
  IS_HOT_KEY_MATCH     IsKeyMatch;        // advanced check, NULL means no need.
  UINTN                Attribute;
  UINTN                BootType;
  VOID                 *RegHandle;
  EFI_GUID             *FileName;
  OEM_HOT_KEY_HANDLER  Handler;
  CHAR16               *ScrPrompt;
  CHAR16               *BootOptionName;
} POST_HOT_KEY_CTX;


typedef
EFI_STATUS
(EFIAPI *GET_PLAT_NVME_INDEX)(
  EFI_DEVICE_PATH_PROTOCOL *Dp, 
  UINTN                    *Index
  );

typedef
EFI_STATUS
(EFIAPI *BYO_STOP_WATCHDOG)(
    VOID
  );

typedef
EFI_STATUS
(EFIAPI *BYO_RESET_WATCHDOG)(
    VOID
  );

typedef
CHAR16 *
(EFIAPI *GET_PLAT_SATA_PREFIX)(
  EFI_DEVICE_PATH_PROTOCOL *Dp
  );
struct _PLAT_HOST_INFO_PROTOCOL {
  PLATFORM_HOST_INFO        *HostList;
  UINTN                     HostCount;
  UINTN                     SataHostCount;
  EFI_DEVICE_PATH_PROTOCOL  *IgdDp;
  UINTN                     IgdDpSize;
  GET_PLAT_SATA_HOST_INDEX  GetSataHostIndex;
  GET_PLAT_SATA_PORT_INDEX  GetSataPortIndex;
  GET_PLAT_NVME_INDEX       GetNvmeIndex;
  ADDITIONAL_ROM_TABLE      *OptionRomTable;
  UINTN                     OptionRomTableSize;
  GET_PLAT_UCR_DP           GetPlatUcrDp;
  EFI_DEVICE_PATH_PROTOCOL  **ConInDp;
  UINTN                     ConInDpCount;
  UPDATE_BOOT_OPTION        UpdateBootOption;
  UPDATE_TOLUM_VAR          UpdateTolumVar;
  UINT64                    CpuMcVer;
  POST_HOT_KEY_CTX          *HotKey;
  UINTN                     HotKeyCount;
  VOID                      *SetupAddOnHandle;
  UINTN                     SetupTitleId;
  BYO_STOP_WATCHDOG         StopWdg;
  UINT32                    AddOnGfxId;
  BYO_RESET_WATCHDOG        ResetFrb2Wdg;
  GET_PLAT_SATA_PREFIX      GetCustomizedSataPrefix;
};


extern EFI_GUID gPlatHostInfoProtocolGuid;



#endif


