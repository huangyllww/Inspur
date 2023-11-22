/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.


File Name:
  BootMaint.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include <PiDxe.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/GlobalVariable.h>
#include <Guid/PxeControlVariable.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/BaseLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PcdLib.h>
#include <Library/HiiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/SetupSaveNotify.h>
#include <Protocol/ByoFormSetManager.h>
#include <Protocol/DevicePath.h>
#include <Protocol/HiiConfigRouting.h>
#include <Protocol/FormBrowser2.h>
#include <Protocol/ByoFormBrowserEx.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/HiiDatabase.h>
#include <Protocol/HiiString.h>
#include <Protocol/ByoPlatformSetupProtocol.h>
#include <Library/ByoCommLib.h>
#include "FormGuid.h"
#include <Protocol/SetupItemUpdateNotify.h>
#include <ByoBootGroup.h>
#include <Library/SetupUiLib.h>
#include <Protocol/Rng.h>
#include <Library/ByoCommLib.h>
#include <Library/FileExplorerLib.h>
#include <SysMiscCfg.h>
#include <ByoDisabledGroupType.h>
#include <Library/ByoBootManagerLib.h>
#include <IndustryStandard/Pci22.h>
#include <Protocol/DevicePathToText.h>
#include <ByoCustomizedSortPolicy.h>

#ifndef BYO_ONLY_UEFI
#include <Guid/LegacyDevorder.h>
#include <Library/LegacyBootOptionalDataLib.h>
#include <Protocol/LegacyBios.h>
#endif

#define OPTION_NAME_LEN  sizeof ("PlatformRecovery####")
#define TKN_LANG_CHS_NAME              "zh-Hans"
#define USER_BOOT_ORDER_VAR_NAME       L"UserBootOrder"

#define MAX_BOOT_DESCRIPTION_SIZE    (MAX_BOOT_DESCRIPTION_LEN ) * sizeof(CHAR16)
#define MAX_BOOT_OPTIONALDATA_SIZE   (MAX_BOOT_OPTIONAL_DATA_LEN) * sizeof(CHAR16)

#define POSITION_OF_UNUSED_MENU_CTX  0xFFFF
OPTION_SORT_POLICY mDefaultOptionSortOption = {
  {
    HDD_SHORT_FORM_TYPE,
    NVME_SSD_TYPE,
    SATA_HDD_TYPE,
    ATAPI_HDD_TYPE,
    RAID_HDD_TYPE,
    SCSI_HDD_TYPE,
    MMC_HDD_TYPE,
    SD_HDD_TYPE
  }, // HddGroupOrder
  {
    PXE_IPV4_TYPE,
    PXE_IPV6_TYPE,
    HTTP_IPV4_TYPE,
    HTTP_IPV6_TYPE
  }, // NetIpGroupType
  INSERT_NEW_OPTION_BY_PRIORITY
};

EFI_GUID mBootMaintenanceManagerGuid = BOOT_MAINT_MANAGER_FORMSET_GUID;
DISABLED_GROUP_TYPE    mDisabledGroupType;
BOOLEAN                mGotDisableGroupVar = TRUE;
BOOLEAN                mFirstEnterBootOrderPage = TRUE;
UINT8                  mBootMode;
EFI_DEVICE_PATH_PROTOCOL  *mFilePath = NULL;

VOID RestoreBootOrder(BOOLEAN IsUserDefault);

EFI_BOOT_MANAGER_LOAD_OPTION              *mUefiOptions = NULL;
UINTN                                     mUefiOptionsCount = 0;
EFI_BOOT_MANAGER_LOAD_OPTION              *mLegacyOptions = NULL;
UINTN                                     mLegacyOptionsCount = 0;
UINT16                                    mItemsCount;

#ifndef BYO_ONLY_UEFI
  BBS_TABLE                               *mBbsTable = NULL;
  UINT16                                  mBbsCount = 0;
#endif

EFI_STATUS
EFIAPI
BootMaintExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  );

EFI_STATUS
EFIAPI
BootMaintRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  );

EFI_STATUS
EFIAPI
BootMaintCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL         *This,
  IN        EFI_BROWSER_ACTION                     Action,
  IN        EFI_QUESTION_ID                        QuestionId,
  IN        UINT8                                  Type,
  IN        EFI_IFR_TYPE_VALUE                     *Value,
  OUT       EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  );

EFI_STATUS
BootConfigureSaveValue(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS
BootConfigureDiscardValue(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS
BootConfigureLoadDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS
BootConfigureSaveUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS
BootConfigureLoadUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  );

EFI_STATUS
IsBootOptionChanged(
  IN  SETUP_SAVE_NOTIFY_PROTOCOL *This,
  OUT BOOLEAN                    *IsDataChanged
  );

EFI_STATUS
SetupItemBootNotify(
  SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL    *This,
  UINTN                                Item,
  UINTN                                NewStatus
  );

VOID
SetSecureBootInBmmData (
    BMM_FAKE_NV_DATA    *CfgData
  );

VOID
UpdatePages (
  UINT16        StartOpCodeCtx,
  UINT16        EndOpCodeCtx
  );

VOID
UpdateBootOrderItems (
  UINT16              CtxIndex
  );

VOID
UpdateDisablePages (
  UINT16              CtxIndex
  );

CHAR16 *
BmUiDevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  );

extern UINT8           BootMaintManagerBin[];
EFI_HII_HANDLE  gHiiHandle;

#define VAR_OFFSET(Field)              ((UINT16) ((UINTN) &(((BMM_FAKE_NV_DATA *) 0)->Field)))

//
// Question Id of Zero is invalid, so add an offset to it
//
#define QUESTION_ID(Field)             (VAR_OFFSET (Field) + CONFIG_OPTION_OFFSET)

#define LEGACY_HDD_VAR_OFFSET           VAR_OFFSET (LegacyHDD)
#define LEGACY_ODD_VAR_OFFSET           VAR_OFFSET (LegacyODD)
#define LEGACY_USB_DISK_VAR_OFFSET      VAR_OFFSET (LegacyUskDisk)
#define LEGACY_USB_ODD_VAR_OFFSET       VAR_OFFSET (LegacyUsbOdd)
#define LEGACY_PXE_VAR_OFFSET           VAR_OFFSET (LegacyPxe)
#define LEGACY_OTHERS_VAR_OFFSET        VAR_OFFSET (LegacyOthers)

#define UEFI_HDD_VAR_OFFSET             VAR_OFFSET (UefiHDD)
#define UEFI_ODD_VAR_OFFSET             VAR_OFFSET (UefiODD)
#define UEFI_USB_DISK_VAR_OFFSET        VAR_OFFSET (UefiUskDisk)
#define UEFI_USB_ODD_VAR_OFFSET         VAR_OFFSET (UefiUsbOdd)
#define UEFI_PXE_VAR_OFFSET             VAR_OFFSET (UefiPxe)
#define UEFI_OTHERS_VAR_OFFSET          VAR_OFFSET (UefiOthers)

#define STATUS_LEGACY_HDD_VAR_OFFSET           VAR_OFFSET (StatusLegacyHDD)
#define STATUS_LEGACY_ODD_VAR_OFFSET           VAR_OFFSET (StatusLegacyODD)
#define STATUS_LEGACY_USB_DISK_VAR_OFFSET      VAR_OFFSET (StatusLegacyUskDisk)
#define STATUS_LEGACY_USB_ODD_VAR_OFFSET       VAR_OFFSET (StatusLegacyUsbOdd)
#define STATUS_LEGACY_PXE_VAR_OFFSET           VAR_OFFSET (StatusLegacyPxe)
#define STATUS_LEGACY_OTHERS_VAR_OFFSET        VAR_OFFSET (StatusLegacyOthers)

#define STATUS_UEFI_HDD_VAR_OFFSET             VAR_OFFSET (StatusUefiHDD)
#define STATUS_UEFI_ODD_VAR_OFFSET             VAR_OFFSET (StatusUefiODD)
#define STATUS_UEFI_USB_DISK_VAR_OFFSET        VAR_OFFSET (StatusUefiUskDisk)
#define STATUS_UEFI_USB_ODD_VAR_OFFSET         VAR_OFFSET (StatusUefiUsbOdd)
#define STATUS_UEFI_PXE_VAR_OFFSET             VAR_OFFSET (StatusUefiPxe)
#define STATUS_UEFI_OTHERS_VAR_OFFSET          VAR_OFFSET (StatusUefiOthers)

#define STATUS_UEFI_BOOT_GROUP_VAR_OFFSET      VAR_OFFSET (StatusUefiBootGroup)
#define STATUS_LEGACY_BOOT_GROUP_VAR_OFFSET    VAR_OFFSET (StatusLegacyBootGroup)

#define NV_VAR_FLAG    (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)

typedef struct {
  UINT32                         Signature;
  EFI_HII_HANDLE                 BmmHiiHandle;
  EFI_HANDLE                     BmmDriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL BmmConfigAccess;
  SETUP_SAVE_NOTIFY_PROTOCOL     BmmSaveNotify;
  BMM_FAKE_NV_DATA               BmmFakeNvData;
  SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL  Notify;
  EFI_FORM_ID                    BmmCurrentPageId;
  EFI_FORM_ID                    BmmPreviousPageId;
} BMM_CALLBACK_DATA;

#define BMM_CALLBACK_DATA_SIGNATURE          SIGNATURE_32 ('C', 'b', 'c', 'k')
#define BMM_CALLBACK_DATA_FROM_THIS(a)       CR(a, BMM_CALLBACK_DATA, BmmConfigAccess, BMM_CALLBACK_DATA_SIGNATURE)
#define BMM_SAVE_CALLBACK_DATA_FROM_THIS(a)  CR(a, BMM_CALLBACK_DATA, BmmSaveNotify, BMM_CALLBACK_DATA_SIGNATURE)


HII_VENDOR_DEVICE_PATH  mBmmHiiVendorDevicePath = {
  {
    {
      {
        HARDWARE_DEVICE_PATH,
        HW_VENDOR_DP,
        {
          (UINT8) (sizeof (HII_VENDOR_DEVICE_PATH_NODE)),
          (UINT8) ((sizeof (HII_VENDOR_DEVICE_PATH_NODE)) >> 8)
        }
      },
      EFI_BYO_IFR_GUID,
    },
    0,
    (UINTN)&mBmmHiiVendorDevicePath
  },

  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};


STATIC BMM_CALLBACK_DATA gBmmCallbackInfo = {
  BMM_CALLBACK_DATA_SIGNATURE,
  NULL,
  NULL,
  {
    BootMaintExtractConfig,
    BootMaintRouteConfig,
    BootMaintCallback
  },
  {
    NULL,
    BootConfigureSaveValue,
    BootConfigureDiscardValue,
    BootConfigureLoadDefault,
    BootConfigureSaveUserDefault,
    BootConfigureLoadUserDefault,
    IsBootOptionChanged,
  },
  {{0,},},
  {
    SetupItemBootNotify
  },
};

BMM_CALLBACK_DATA   *BmmCallbackInfo = &gBmmCallbackInfo;


#define BYO_BOOT_MAINT_VAR_NAME       L"BmmData"

typedef enum {
  BM_BOOT_TYPE_MIN = 0,
  BM_BOOT_TYPE_UEFI = BM_BOOT_TYPE_MIN,
  BM_BOOT_TYPE_LEGACY,
  BM_BOOT_TYPE_MAX
} BM_BOOT_TYPE;



typedef struct {
  UINT16                        *DisplayString;
  UINT16                        *HelpString;
  EFI_STRING_ID                 DisplayStringToken;
  EFI_STRING_ID                 HelpStringToken;
  EFI_BOOT_MANAGER_LOAD_OPTION  *LoadOption;
  UINT16                        Value;
  BOOLEAN                       Flag;
  BOOLEAN                       Enable;
} BM_BOOT_DEV_ITEM;


typedef struct {
  UINT16             Position;
  UINT16             DevCount;
  EFI_STRING_ID      PromptWithTriangle;
  EFI_STRING_ID      Prompt;
  EFI_STRING_ID      Help;
  UINT16             OrderVarOffset;
  UINT16             OptionVarOffset;
  UINT16             GroupVarOffset;
  UINT16             QuestionId;
  UINT16             StatusGroupQuestionId;
  BM_BOOT_TYPE       BootType;
  BM_MENU_TYPE       MenuType;
  BM_BOOT_DEV_ITEM   Item[MAX_MENU_NUMBER];
  BOOLEAN            Enable;
} BM_MENU_CTX;

typedef struct {
  UINTN       OpCodeHandleCount;
  UINT16      OpCodeCtxId;
  UINT16      StartOpLabelNumber;
  UINT16      EndOpLabelNumber;
  UINT16      QuestionIdBase;
  VOID        *StartOpCodeHandle;
  VOID        *EndOpCodeHandle;
} BM_OPCODE_CTX;

typedef struct {
  BM_BOOT_TYPE  BootType;
  BOOLEAN       Enable;
} BM_BOOT_TYPE_CTX;

BM_BOOT_TYPE_CTX mBmBootTypeCtx[] = {
  {BM_BOOT_TYPE_UEFI, TRUE},
  {BM_BOOT_TYPE_LEGACY, TRUE},
};

// KEEP UEFI FIRST!
BM_MENU_CTX gBmMenuCtx[] = {
  {0, 0, STRING_TOKEN(STR_UEFI_HDD_WITH_TRIANGLE),      STRING_TOKEN(STR_UEFI_HDD),      STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_HDD_VAR_OFFSET,      STATUS_UEFI_HDD_VAR_OFFSET,      DISABLE_HDD_VAR_OFFSET,      0, 0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_HDD,      {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_UEFI_PXE_WITH_TRIANGLE),      STRING_TOKEN(STR_UEFI_PXE),      STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_PXE_VAR_OFFSET,      STATUS_UEFI_PXE_VAR_OFFSET,      DISABLE_PXE_VAR_OFFSET,      0, 0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_PXE,      {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_UEFI_ODD_WITH_TRIANGLE),      STRING_TOKEN(STR_UEFI_ODD),      STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_ODD_VAR_OFFSET,      STATUS_UEFI_ODD_VAR_OFFSET,      DISABLE_ODD_VAR_OFFSET,      0, 0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_ODD,      {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_UEFI_USB_DISK_WITH_TRIANGLE), STRING_TOKEN(STR_UEFI_USB_DISK), STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_USB_DISK_VAR_OFFSET, STATUS_UEFI_USB_DISK_VAR_OFFSET, DISABLE_USB_DISK_VAR_OFFSET, 0, 0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_USB_DISK, {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_UEFI_USB_ODD_WITH_TRIANGLE),  STRING_TOKEN(STR_UEFI_USB_ODD),  STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_USB_ODD_VAR_OFFSET,  STATUS_UEFI_USB_ODD_VAR_OFFSET,  DISABLE_USB_ODD_VAR_OFFSET,  0, 0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_USB_ODD,  {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_UEFI_OTHERS_WITH_TRIANGLE),   STRING_TOKEN(STR_UEFI_OTHERS),   STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_OTHERS_VAR_OFFSET,   STATUS_UEFI_OTHERS_VAR_OFFSET,   DISABLE_OTHERS_VAR_OFFSET,   0, 0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_OTHERS,   {{0,},}, TRUE},

  {0, 0, STRING_TOKEN(STR_LEGACY_HDD_WITH_TRIANGLE),      STRING_TOKEN(STR_LEGACY_HDD),      STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_HDD_VAR_OFFSET,      STATUS_LEGACY_HDD_VAR_OFFSET,      DISABLE_LEGACY_HDD_VAR_OFFSET,      0, 0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_HDD,      {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_LEGACY_PXE_WITH_TRIANGLE),      STRING_TOKEN(STR_LEGACY_PXE),      STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_PXE_VAR_OFFSET,      STATUS_LEGACY_PXE_VAR_OFFSET,      DISABLE_LEGACY_PXE_VAR_OFFSET,      0, 0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_PXE,      {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_LEGACY_ODD_WITH_TRIANGLE),      STRING_TOKEN(STR_LEGACY_ODD),      STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_ODD_VAR_OFFSET,      STATUS_LEGACY_ODD_VAR_OFFSET,      DISABLE_LEGACY_ODD_VAR_OFFSET,      0, 0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_ODD,      {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_LEGACY_USB_DISK_WITH_TRIANGLE), STRING_TOKEN(STR_LEGACY_USB_DISK), STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_USB_DISK_VAR_OFFSET, STATUS_LEGACY_USB_DISK_VAR_OFFSET, DISABLE_LEGACY_USB_DISK_VAR_OFFSET, 0, 0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_USB_DISK, {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_LEGACY_USB_ODD_WITH_TRIANGLE),  STRING_TOKEN(STR_LEGACY_USB_ODD),  STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_USB_ODD_VAR_OFFSET,  STATUS_LEGACY_USB_ODD_VAR_OFFSET,  DISABLE_LEGACY_USB_ODD_VAR_OFFSET,  0, 0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_USB_ODD,  {{0,},}, TRUE},
  {0, 0, STRING_TOKEN(STR_LEGACY_OTHERS),                 STRING_TOKEN(STR_LEGACY_OTHERS),   STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_OTHERS_VAR_OFFSET,   STATUS_LEGACY_OTHERS_VAR_OFFSET,   DISABLE_LEGACY_OTHERS_VAR_OFFSET,   0, 0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_OTHERS,   {{0,},}, TRUE},
};

#define UEFI_BOOT_ITEM_OPCODE_CTX_ID       0
#define LEGACY_BOOT_ITEM_OPCODE_CTX_ID     1
#define DISABLE_UEFI_GROUP_OPCODE_CTX_ID   2
#define DISABLE_LEGACY_GROUP_OPCODE_CTX_ID 3
#define DISABLE_BOOT_OPTIONS_OPCODE_CTX_ID 4

BM_OPCODE_CTX mBmOpCodeCtx[] = {
  {0, UEFI_BOOT_ITEM_OPCODE_CTX_ID,       UEFI_BOOT_ITEM_ID,            UEFI_BOOT_ITEM_END_ID,            BMM_QUESTION_ID_BASE,             NULL, NULL},
  {0, LEGACY_BOOT_ITEM_OPCODE_CTX_ID,     LEGACY_BOOT_ITEM_ID,          LEGACY_BOOT_ITEM_END_ID,          BMM_QUESTION_ID_BASE,             NULL, NULL},
  {0, DISABLE_UEFI_GROUP_OPCODE_CTX_ID,   DISABLE_UEFI_BOOT_GROUP_ID,   DISABLE_UEFI_BOOT_GROUP_END_ID,   DISABLE_GROUP_QUESTION_ID_BASE,   NULL, NULL},
  {0, DISABLE_LEGACY_GROUP_OPCODE_CTX_ID, DISABLE_LEGACY_BOOT_GROUP_ID, DISABLE_LEGACY_BOOT_GROUP_END_ID, DISABLE_GROUP_QUESTION_ID_BASE,   NULL, NULL},
  {0, DISABLE_BOOT_OPTIONS_OPCODE_CTX_ID, DISABLE_BOOT_OPTIONS_ID,      DISABLE_BOOT_OPTIONS_END_ID,      DISABLE_OPTIONS_QUESTION_ID_BASE, NULL, NULL},
};

#define BM_MENU_CTX_COUNT         (sizeof(gBmMenuCtx)/sizeof(gBmMenuCtx[0]))
#define BM_MENU_CTX_UEFI_COUNT    6
#define BM_MENU_CTX_LEGACY_COUNT  6

#define BM_OPCODE_CTX_COUNT       (sizeof(mBmOpCodeCtx) / sizeof(mBmOpCodeCtx[0]))
#define BM_BOOT_TYPE_CTX_COUNT    (sizeof(mBmBootTypeCtx) / sizeof(mBmBootTypeCtx[0]))


typedef struct {
  UINT16             Position;
  UINT16             DevCount;
  UINT16             *DevName[MAX_MENU_NUMBER];
} USER_BM_MENU_INFO;

USER_BM_MENU_INFO gUserBmMenuInfo[BM_MENU_CTX_COUNT];

UINT16 gMaxPosition[BM_BOOT_TYPE_MAX];
UINT16 gOpLabelNumber[BM_BOOT_TYPE_MAX];
EFI_BOOT_MANAGER_LOAD_OPTION   *gNvBootOptions = NULL;
UINTN                          gNvBootOptionCount = 0;

BM_MENU_CTX *FindBmMenuCtxEntryByPosition(UINT16 Position, BM_BOOT_TYPE BootType);

STATIC
BM_MENU_TYPE
GetGroupTypeOfAllTypeOptions (
  EFI_BOOT_MANAGER_LOAD_OPTION  *Option
  )
{
  BM_MENU_TYPE                   GroupType;
#ifndef BYO_ONLY_UEFI
  UINT16                         BbsIndex;
  UINT16                         DeviceType;
#endif

  if (Option == NULL) {
    return BM_MENU_TYPE_MAX;
  }

  GroupType = GetEfiBootGroupType(Option->FilePath);
  #ifndef BYO_ONLY_UEFI
    if ((DevicePathType (Option->FilePath) == BBS_DEVICE_PATH) &&
      (DevicePathSubType (Option->FilePath) == BBS_BBS_DP)) { // fix GroupType of Legacy options
      if (mBbsTable == NULL) {
        return BM_MENU_TYPE_MAX;
      }

      BbsIndex = LegacyBootOptionalDataGetBbsIndex(Option->OptionalData);
      DeviceType = mBbsTable[BbsIndex].DeviceType;
      if (DeviceType == BBS_BEV_DEVICE) {
        if (mBbsTable[BbsIndex].Class == PCI_CLASS_MASS_STORAGE) {
          DeviceType = BBS_CDROM;
        } else if (mBbsTable[BbsIndex].Class == PCI_CLASS_NETWORK) {
          DeviceType = BBS_EMBED_NETWORK;
        }
      }
      GroupType = GetBbsGroupType(mBbsTable[BbsIndex].Class, (UINT8)DeviceType);
    }
  #endif

  return GroupType;
}


EFI_STATUS NotifySetupItemChanged(UINTN Item, UINTN NewStatus)
{
  UINTN                                 HandleCount;
  EFI_HANDLE                            *Handles = NULL;
  EFI_STATUS                            Status = EFI_SUCCESS;
  UINTN                                 Index;
  SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL     *ItemNotify;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gSetupItemUpdateNotifyProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gSetupItemUpdateNotifyProtocolGuid,
                    (VOID**)&ItemNotify
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }

    Status = ItemNotify->Notify(ItemNotify, Item, NewStatus);
  }

  if (Handles != NULL) {gBS->FreePool(Handles);}
  return Status;
}



EFI_STATUS
BootConfigureDiscardValue(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
BootConfigureLoadDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
  RestoreBootOrder(FALSE);
  return EFI_SUCCESS;
}


EFI_STATUS
BootConfigureLoadUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  RestoreBootOrder(TRUE);
  return EFI_SUCCESS;
}

EFI_STATUS
IsBootOptionChanged(
  IN  SETUP_SAVE_NOTIFY_PROTOCOL *This,
  OUT BOOLEAN                    *IsDataChanged
  )
{
  *IsDataChanged = FALSE;
  return EFI_SUCCESS;
}



/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.


  @param This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Request         A null-terminated Unicode string in <ConfigRequest> format.
  @param Progress        On return, points to a character in the Request string.
                         Points to the string's null terminator if request was successful.
                         Points to the most recent '&' before the first failing name/value
                         pair (or the beginning of the string if the failure is in the
                         first name/value pair) if the request was not successful.
  @param Results         A null-terminated Unicode string in <ConfigAltResp> format which
                         has all values filled in for the names in the Request string.
                         String to be allocated by the called function.

  @retval  EFI_SUCCESS            The Results is filled with the requested values.
  @retval  EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval  EFI_INVALID_PARAMETER  Request is NULL, illegal syntax, or unknown name.
  @retval  EFI_NOT_FOUND          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
BootMaintExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS         Status;
  UINTN              BufferSize;
  BMM_CALLBACK_DATA  *Private;
  EFI_STRING         ConfigRequestHdr;
  EFI_STRING         ConfigRequest;
  BOOLEAN            AllocatedRequest;
  UINTN              Size;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;

  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &mBootMaintenanceManagerGuid, BYO_BOOT_MAINT_VAR_NAME)) {
    return EFI_NOT_FOUND;
  }

  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0;

  Private = BMM_CALLBACK_DATA_FROM_THIS (This);

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  BufferSize = sizeof (BMM_FAKE_NV_DATA);
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&mBootMaintenanceManagerGuid, BYO_BOOT_MAINT_VAR_NAME, Private->BmmDriverHandle);
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
  }

  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                (UINT8 *) &Private->BmmFakeNvData,
                                BufferSize,
                                Results,
                                Progress
                                );
  DEBUG((EFI_D_INFO, " %a BlockToConfig :%r. line=%d\n", __FUNCTION__, Status, __LINE__));
  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}

VOID
CloseUpdateMenuCtx (
  BMM_CALLBACK_DATA    *Private
  );

EFI_STATUS
EFIAPI
BootMaintRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  BMM_CALLBACK_DATA           *Private;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;

  if (!HiiIsConfigHdrMatch (Configuration, &mBootMaintenanceManagerGuid, BYO_BOOT_MAINT_VAR_NAME)) {
    return EFI_NOT_FOUND;
  }
  
  Private = BMM_CALLBACK_DATA_FROM_THIS(This);
  CloseUpdateMenuCtx(Private);
  *Progress = Configuration + StrLen (Configuration);

  return EFI_SUCCESS;
}



EFI_STATUS
SetupItemBootNotify(
  SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL    *This,
  UINTN                                Item,
  UINTN                                NewStatus
  )
{
  PXE_CONTROL_VARIABLE        PxeControl;
  if(Item == ITEM_NETWORK_BOOT) {
    HiiGetBrowserData(
      &gPxeControlVariableGuid,
      PXE_CONTROL_VAR_NAME,
      sizeof(PXE_CONTROL_VARIABLE),
      (UINT8*)&PxeControl
      );
    if (NewStatus == STATUS_ENABLE) {
      PxeControl.NetworkEnable = 1;
    } else if (NewStatus == STATUS_DISABLE) {
      PxeControl.NetworkEnable = 0;
    }
    HiiSetBrowserData(
          &gPxeControlVariableGuid, 
          PXE_CONTROL_VAR_NAME, 
          sizeof(PXE_CONTROL_VARIABLE), 
          (UINT8*)&PxeControl,
          NULL
          );
  }
  return EFI_SUCCESS;
}


EFI_STATUS AddBootHiiPackages(EFI_HANDLE DeviceHandle)
{
  EFI_STATUS    Status;

  //if(BmmCallbackInfo->BmmHiiHandle != NULL){
    //HiiRemovePackages (BmmCallbackInfo->BmmHiiHandle);
    //BmmCallbackInfo->BmmHiiHandle = NULL;
  //}

  if (BmmCallbackInfo->BmmHiiHandle == NULL) {
  BmmCallbackInfo->BmmHiiHandle = HiiAddPackages (
                    &mBootMaintenanceManagerGuid,
                    DeviceHandle,
                    BootMaintManagerBin,
                    BootMaintenanceManagerDxeStrings,
                    NULL
                    );
  }
  if(BmmCallbackInfo->BmmHiiHandle != NULL){
    gHiiHandle = BmmCallbackInfo->BmmHiiHandle;
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_INVALID_PARAMETER;
  }

  return Status;
}

void UserBmMenuInfoInit(UINT16 *VarData, UINTN VarSize)
{
  UINTN               Index;
  USER_BM_MENU_INFO   *Info;
  UINT16              *p;
  UINTN               i;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  ASSERT(ARRAY_SIZE(gBmMenuCtx) == ARRAY_SIZE(gUserBmMenuInfo));
  ZeroMem(&gUserBmMenuInfo, sizeof(gUserBmMenuInfo));

  if(VarData == NULL || VarSize == 0){
    return;
  }

  Info = gUserBmMenuInfo;
  p = VarData;

  for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
    Info[Index].Position = *p++;
    Info[Index].DevCount = *p++;
    DEBUG((EFI_D_INFO, "P:%d L:%d\n", Info[Index].Position, Info[Index].DevCount));
    for (i = 0; i < Info[Index].DevCount; i++) {
      Info[Index].DevName[i] = p;
      p += StrLen(p) + 1;
      DEBUG((EFI_D_INFO, "[%d] %s\n", i, Info[Index].DevName[i]));
    }
  }
}

EFI_STATUS
BootConfigureSaveUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  UINTN         Index;
  UINTN         i, j;
  BM_MENU_CTX   *MenuCtx;
  UINTN         BufferSize = 0;
  UINT16        *Buffer;
  UINT16        *p;
  UINTN         Size;
  UINT16        *ItemOrder;

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
    MenuCtx = &gBmMenuCtx[Index];
    BufferSize += sizeof(MenuCtx->Position) + sizeof(MenuCtx->DevCount);
    for (i = 0; i < MenuCtx->DevCount; i++) {
      BufferSize += StrSize(MenuCtx->Item[i].DisplayString);
    }
  }

  Buffer = AllocatePool(BufferSize);
  ASSERT(Buffer != NULL);
  p = Buffer;

  for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
    MenuCtx = &gBmMenuCtx[Index];
    *(p++) = MenuCtx->Position;
    *(p++) = MenuCtx->DevCount;

    ItemOrder = (UINT16*)(((UINTN)&BmmCallbackInfo->BmmFakeNvData) + MenuCtx->OrderVarOffset);
    for (i = 0; i < MenuCtx->DevCount; i++) {
      for (j = 0; j < MenuCtx->DevCount; j++) {
        if (MenuCtx->Item[j].Value == ItemOrder[i]) {
          Size = StrSize(MenuCtx->Item[j].DisplayString);
          CopyMem(p, MenuCtx->Item[j].DisplayString, Size);
          p += Size/sizeof(CHAR16);
        }
      }
    }
  }

  gRT->SetVariable (
                  USER_BOOT_ORDER_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NV_VAR_FLAG,
                  BufferSize,
                  Buffer
                  );

  FreePool(Buffer);

  return EFI_SUCCESS;
}

BM_MENU_CTX *FindBmMenuCtxEntry(BM_MENU_TYPE MenuType)
{
  UINTN  Index;

  for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
    if (gBmMenuCtx[Index].MenuType == MenuType) {
      return &gBmMenuCtx[Index];
    }
  }

  return NULL;
}


BM_MENU_CTX *FindBmMenuCtxEntryByQuestionId(UINT16 QuestionId)
{
  UINTN  Index;

  for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
    if ((gBmMenuCtx[Index].QuestionId == QuestionId) || (gBmMenuCtx[Index].StatusGroupQuestionId == QuestionId)) {
      if (gBmMenuCtx[Index].Position == POSITION_OF_UNUSED_MENU_CTX) {
        return NULL;
      }
      return &gBmMenuCtx[Index];
    }
  }

  return NULL;
}


BM_MENU_CTX *FindBmMenuCtxEntryByPosition(UINT16 Position, BM_BOOT_TYPE BootType)
{
  UINTN  Index;
  if (Position == POSITION_OF_UNUSED_MENU_CTX) {
    return NULL;
  }

  for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
    if (gBmMenuCtx[Index].Position == Position && gBmMenuCtx[Index].BootType == BootType) {
      return &gBmMenuCtx[Index];
    }
  }

  return NULL;
}

BM_BOOT_DEV_ITEM *FindBootDevItemEntryByValue(BM_BOOT_DEV_ITEM *List, UINT16 Value)
{
  UINTN  Index;

  for (Index = 0; Index < MAX_MENU_NUMBER; Index++) {
    if (List[Index].Value == 0) {
      DEBUG((EFI_D_ERROR, "%a (L%d) Vin:%d\n", __FUNCTION__, __LINE__, Value));
      return NULL;
    }
    if (List[Index].Value == Value) {
      return List + Index;
    }
  }

  DEBUG((EFI_D_ERROR, "%a (L%d) Vin:%d\n", __FUNCTION__, __LINE__, Value));
  return NULL;
}

/**
  This function converts an input device structure to a Unicode string.

  @param DevPath      A pointer to the device path structure.

  @return             A new allocated Unicode string that represents the device path.

**/
CHAR16 *
BmUiDevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  EFI_STATUS                       Status;
  CHAR16                           *ToText;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

  if (DevPath == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  (VOID **) &DevPathToText
                  );
  ASSERT_EFI_ERROR (Status);
  ToText = DevPathToText->ConvertDevicePathToText (
                            DevPath,
                            FALSE,
                            TRUE
                            );
  ASSERT (ToText != NULL);
  return ToText;
}

EFI_STATUS
InitBootItems (
  VOID
  )
{
  UINT16                        Index;
  EFI_BOOT_MANAGER_LOAD_OPTION  *Option;
  BM_MENU_CTX                   *MenuCtx;
  BM_BOOT_DEV_ITEM              *Item;
  UINTN                         VarBase;
  UINT16                        *p;
  BM_MENU_TYPE                  GroupOrder;

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  if (gNvBootOptionCount == 0) {
    return EFI_NOT_FOUND;
  }

  VarBase = (UINTN)&BmmCallbackInfo->BmmFakeNvData;
  Option = gNvBootOptions;

  for (Index = 0; Index < gNvBootOptionCount; Index++) {
    if ((Option[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) {
      continue;
    }

    if ((Option[Index].Attributes & LOAD_OPTION_CATEGORY) != LOAD_OPTION_CATEGORY_BOOT) {
      continue;
    }

    GroupOrder = GetGroupTypeOfAllTypeOptions(&Option[Index]);
    MenuCtx = FindBmMenuCtxEntry(GroupOrder);
    DEBUG((EFI_D_INFO, "XXXX:%X GroupOrder:%X\n", Option[Index].OptionNumber, GroupOrder));
    if (MenuCtx == NULL) {
      continue;
    }

    if (MenuCtx->DevCount < MAX_MENU_NUMBER) {
      Item = &MenuCtx->Item[MenuCtx->DevCount];
      Item->LoadOption = &Option[Index];
      Item->DisplayString = Item->LoadOption->Description;
      Item->HelpString    = BmUiDevicePathToStr (Option[Index].FilePath);
      Item->DisplayStringToken = HiiSetString(BmmCallbackInfo->BmmHiiHandle, 0, Item->DisplayString, NULL);
      Item->HelpStringToken = HiiSetString (BmmCallbackInfo->BmmHiiHandle, 0, Item->HelpString, NULL);
      p = (UINT16*)(VarBase + MenuCtx->OrderVarOffset) + MenuCtx->DevCount;
      *p = (UINT16)Item->LoadOption->OptionNumber + 1;      // 0 is end flag
//-   DEBUG((EFI_D_INFO, "V:%d C:%d O:%X\n", *p, MenuCtx->DevCount, MenuCtx->OrderVarOffset));
      Item->Value = *p;
      if ((Option[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
        Item->Enable = FALSE;
      } else {
        Item->Enable = TRUE;
      }
      MenuCtx->DevCount++;
    }
  }

  return EFI_SUCCESS;
}

VOID
InitBootGroup (
  VOID
  )
{
  EFI_STATUS   UefiStatus;
  EFI_STATUS   LegacyStatus;
  EFI_STATUS   Status;
  UINT16       Index;
  BM_MENU_CTX  *MenuCtx;
  BM_BOOT_TYPE TypeIndex;
  UINT8        *LegacyGroupOrder = NULL;
  UINT8        *UefiGroupOrder = NULL;
  UINTN        LegacyGroupOrderSize = 0;
  UINTN        UefiGroupOrderSize = 0;
  UINTN        DataSize;
  UINT8        *GroupOrder;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  ZeroMem(gMaxPosition, sizeof(gMaxPosition));

  UefiStatus = gRT->GetVariable(
                  BYO_UEFI_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &UefiGroupOrderSize,
                  UefiGroupOrder
                  );
  if (UefiStatus == EFI_BUFFER_TOO_SMALL) {
    UefiGroupOrder = AllocatePool (UefiGroupOrderSize);
    ASSERT(UefiGroupOrder != NULL);
    UefiStatus = gRT->GetVariable(
                  BYO_UEFI_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &UefiGroupOrderSize,
                  UefiGroupOrder
                  );
  }

  LegacyStatus = gRT->GetVariable(
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &LegacyGroupOrderSize,
                  LegacyGroupOrder
                  );
  if (LegacyStatus == EFI_BUFFER_TOO_SMALL) {
    LegacyGroupOrder = AllocatePool (LegacyGroupOrderSize);
    ASSERT(LegacyGroupOrder != NULL);
    LegacyStatus = gRT->GetVariable(
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &LegacyGroupOrderSize,
                  LegacyGroupOrder
                  );
  }

  if (!EFI_ERROR(UefiStatus)) {
    //
    // Sync default UefiBootGroupOrder to L"UefiDefaultBootGroupOrder"
    //
    DataSize = 0;
    GroupOrder = NULL;
    Status = gRT->GetVariable (BYO_UEFI_DEFAULT_BOOT_GROUP_VAR_NAME, &gByoGlobalVariableGuid, NULL, &DataSize, GroupOrder);
    if (Status == EFI_NOT_FOUND) {
      Status = gRT->SetVariable (
                    BYO_UEFI_DEFAULT_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    BYO_BG_ORDER_VAR_ATTR,
                    UefiGroupOrderSize,
                    UefiGroupOrder
                    );
      ASSERT(!EFI_ERROR(Status));
    }

    //
    // Init position of UEFI MenuCtx
    //
    for (Index = 0; Index < UefiGroupOrderSize; Index++) {
      MenuCtx = FindBmMenuCtxEntry(UefiGroupOrder[Index]);
      ASSERT(MenuCtx != NULL);
      MenuCtx->Position = gMaxPosition[MenuCtx->BootType]++;
    }
  }

  if (!EFI_ERROR(LegacyStatus)) {
    //
    // Sync default LegacyBootGroupOrder to L"LegacyDefaultBootGroupOrder"
    //
    DataSize = 0;
    GroupOrder = NULL;
    Status = gRT->GetVariable (BYO_LEGACY_DEFAULT_BOOT_GROUP_VAR_NAME, &gByoGlobalVariableGuid, NULL, &DataSize, GroupOrder);
    if (Status == EFI_NOT_FOUND) {
      Status = gRT->SetVariable (
                    BYO_LEGACY_DEFAULT_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    BYO_BG_ORDER_VAR_ATTR,
                    LegacyGroupOrderSize,
                    LegacyGroupOrder
                    );
      ASSERT(!EFI_ERROR(Status));
    }

    //
    // Init position of legacy MenuCtx
    //
    for (Index = 0; Index < LegacyGroupOrderSize; Index++) {
      MenuCtx = FindBmMenuCtxEntry(LegacyGroupOrder[Index]);
      if (MenuCtx != NULL) {
        MenuCtx->Position = gMaxPosition[MenuCtx->BootType]++;
      } else {
        MenuCtx->Position = POSITION_OF_UNUSED_MENU_CTX;
      }
      
    }
  }

  for (TypeIndex = 0; TypeIndex < BM_BOOT_TYPE_CTX_COUNT; TypeIndex++) {
    if (!mBmBootTypeCtx[TypeIndex].Enable) {
      continue;
    }
    if (gMaxPosition[TypeIndex]) {
      gMaxPosition[TypeIndex]--;
    }

    for (Index = 0; Index <= gMaxPosition[TypeIndex]; Index++) {
      MenuCtx = FindBmMenuCtxEntryByPosition(Index, TypeIndex);
      if (MenuCtx == NULL) {
        continue;
      }

      if (mGotDisableGroupVar) {
        MenuCtx->Enable = *((UINT8 *)((UINTN)&mDisabledGroupType + MenuCtx->GroupVarOffset)) == 0? TRUE: FALSE;
      } else {
        MenuCtx->Enable = TRUE;
      }
    }
  }

  for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
    DEBUG((EFI_D_INFO, "P:%d\n", gBmMenuCtx[Index].Position));
  }
  DEBUG((EFI_D_INFO, "Max:%d,%d\n", gMaxPosition[0], gMaxPosition[1]));

  if (LegacyGroupOrder != NULL) {
    FreePool(LegacyGroupOrder);
  }
  if (UefiGroupOrder != NULL) {
    FreePool(UefiGroupOrder);
  }
}



VOID UpdateStringPreFix(EFI_STRING_ID StringId, CHAR16 PreFix)
{
  CHAR16        *Str;
  UINTN         Size;
  CHAR16        *NewStr;


  Str = HiiGetString(BmmCallbackInfo->BmmHiiHandle, StringId, "en-US");
  if(Str != NULL){
    Size = StrSize(Str);
    NewStr = AllocatePool(Size + sizeof(CHAR16));
    ASSERT(NewStr != NULL);
    NewStr[0] = PreFix;
    StrCpyS(NewStr+1, Size/sizeof(CHAR16), Str+1);
    HiiSetString(BmmCallbackInfo->BmmHiiHandle, StringId, NewStr, "en-US");
    FreePool(NewStr);
    FreePool(Str);
  }

  Str = HiiGetString(BmmCallbackInfo->BmmHiiHandle, StringId, TKN_LANG_CHS_NAME);
  if(Str != NULL){
    Size = StrSize(Str);
    NewStr = AllocatePool(Size + sizeof(CHAR16));
    ASSERT(NewStr != NULL);
    NewStr[0] = PreFix;
    StrCpyS(NewStr+1, Size/sizeof(CHAR16), Str+1);
    HiiSetString(BmmCallbackInfo->BmmHiiHandle, StringId, NewStr, TKN_LANG_CHS_NAME);
    FreePool(NewStr);
    FreePool(Str);
  }
}

VOID UpdateGroupString()
{
  UINTN                    Index;
  BM_MENU_CTX              *MenuCtx;


  MenuCtx = gBmMenuCtx;

  for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
    if (MenuCtx[Index].Position == POSITION_OF_UNUSED_MENU_CTX) {
      continue;
    }
    UpdateStringPreFix(MenuCtx[Index].PromptWithTriangle, GEOMETRICSHAPE_RIGHT_TRIANGLE);
    UpdateStringPreFix(MenuCtx[Index].Prompt, L' ');
  }
}

BOOLEAN 
CheckNetworkBootOption()
{
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions;
  UINTN                         BootOptionCount;
  UINTN                         Index;
  BM_MENU_TYPE                  OptionType;

  BootOptions = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
  for (Index = 0; Index < BootOptionCount; Index++) {
    OptionType = GetGroupTypeOfAllTypeOptions(&BootOptions[Index]);
    if (OptionType == BM_MENU_TYPE_UEFI_PXE || OptionType == BM_MENU_TYPE_LEGACY_PXE) {
      break;
    }
  }
  EfiBootManagerFreeLoadOptions (BootOptions, BootOptionCount);
  if (Index < BootOptionCount) {
    return TRUE;
  }
  return FALSE;
}

VOID
SetupEnterCallback (
  IN EFI_EVENT Event,
  IN VOID     *Context
  )
{
  VOID                        *Interface;
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINT16                      Index, Count;
  BM_MENU_CTX                 *MenuCtx;
  UINTN                       Size;
#ifndef BYO_ONLY_UEFI
  EFI_LEGACY_BIOS_PROTOCOL    *LegacyBios;
  HDD_INFO                    *HddInfo;
  UINT16                      HddCount;
#endif
  BMM_FAKE_NV_DATA            *FakeNvData;
  BM_BOOT_TYPE                TypeIndex;
  PXE_CONTROL_VARIABLE        PxeControl;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
  ASSERT(BM_MENU_CTX_COUNT == BM_MENU_CTX_UEFI_COUNT + BM_MENU_CTX_LEGACY_COUNT);

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &BmmCallbackInfo->BmmDriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mBmmHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &BmmCallbackInfo->BmmConfigAccess,
                  &gSetupItemUpdateNotifyProtocolGuid,
                  &BmmCallbackInfo->Notify,
                  NULL
                  );
  ASSERT(!EFI_ERROR(Status));

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &BmmCallbackInfo->BmmSaveNotify.DriverHandle,
                  &gSetupSaveNotifyProtocolGuid,
                  &BmmCallbackInfo->BmmSaveNotify,
                  NULL
                  );
  ASSERT(!EFI_ERROR(Status));

  if (BmmCallbackInfo->BmmHiiHandle != NULL) {
    HiiRemovePackages (BmmCallbackInfo->BmmHiiHandle);
    BmmCallbackInfo->BmmHiiHandle = NULL;
  }
  AddBootHiiPackages(BmmCallbackInfo->BmmDriverHandle);
  ASSERT (BmmCallbackInfo->BmmHiiHandle != NULL);

  gNvBootOptions = EfiBootManagerGetLoadOptions(&gNvBootOptionCount, LoadOptionTypeBoot);
  if (gNvBootOptionCount > 0) {
    mUefiOptions = (EFI_BOOT_MANAGER_LOAD_OPTION *)AllocateZeroPool(gNvBootOptionCount * sizeof(EFI_BOOT_MANAGER_LOAD_OPTION));
    ASSERT(mUefiOptions != NULL);

    mLegacyOptions = (EFI_BOOT_MANAGER_LOAD_OPTION *)AllocateZeroPool(gNvBootOptionCount * sizeof(EFI_BOOT_MANAGER_LOAD_OPTION));
    ASSERT(mLegacyOptions != NULL);
  }
  for (Index = 0; Index < gNvBootOptionCount; Index++) {
    if ((gNvBootOptions[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) {
      continue;
    }

    if ((DevicePathType(gNvBootOptions[Index].FilePath) != BBS_DEVICE_PATH)) {
      ByoCopyBootOption (mUefiOptions + mUefiOptionsCount, gNvBootOptions + Index);
      mUefiOptionsCount++;
    } else {
      ByoCopyBootOption (mLegacyOptions + mLegacyOptionsCount, gNvBootOptions + Index);
      mLegacyOptionsCount++;
    }
  }
  DEBUG((EFI_D_INFO, "mUefiOptionsCount:%d mLegacyOptionsCount=%d %a line=%d\n", mUefiOptionsCount, mLegacyOptionsCount, __FUNCTION__, __LINE__));

  //
  // Update BmmFakeNvData from L"DisabledGroupType"
  //
  Size = sizeof(DISABLED_GROUP_TYPE);
  Status = gRT->GetVariable (BYO_DISABLED_GROUP_TYPE_VAR_NAME, &gByoDisableGroupTypeGuid, NULL, &Size, &mDisabledGroupType);
  if (Status == EFI_NOT_FOUND) {
    mGotDisableGroupVar = FALSE;
    ZeroMem(&mDisabledGroupType, sizeof(DISABLED_GROUP_TYPE));
  }

#ifndef BYO_ONLY_UEFI
  if (mLegacyOptionsCount != 0) {
    Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
    if (!EFI_ERROR(Status)) {
      Status = LegacyBios->GetBbsInfo (
                            LegacyBios,
                            &HddCount,
                            &HddInfo,
                            &mBbsCount,
                            &mBbsTable
                            );
    }
  }
#endif

  mBootMode = PcdGet8(PcdBiosBootModeType);
  for (Index = 0; Index < BM_BOOT_TYPE_CTX_COUNT; Index++) {
    if ((mBootMode == BIOS_BOOT_UEFI_OS) && (mBmBootTypeCtx[Index].BootType == BM_BOOT_TYPE_LEGACY)) {
      mBmBootTypeCtx[Index].Enable = FALSE;
    }
    if ((mBootMode == BIOS_BOOT_LEGACY_OS) && (mBmBootTypeCtx[Index].BootType == BM_BOOT_TYPE_UEFI)) {
      mBmBootTypeCtx[Index].Enable = FALSE;
    }
  }
  MenuCtx = gBmMenuCtx;
  for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
    MenuCtx[Index].DevCount   = 0;
    MenuCtx[Index].Position   = POSITION_OF_UNUSED_MENU_CTX;
    MenuCtx[Index].QuestionId = BMM_QUESTION_ID_BASE + Index;
    MenuCtx[Index].StatusGroupQuestionId = DISABLE_GROUP_QUESTION_ID_BASE + Index;
  }

  InitBootItems();
  InitBootGroup();
  UpdateGroupString();

  UpdatePages(UEFI_BOOT_ITEM_OPCODE_CTX_ID, LEGACY_BOOT_ITEM_OPCODE_CTX_ID);

  Size = sizeof(PXE_CONTROL_VARIABLE);
  Status = gRT->GetVariable (
                  PXE_CONTROL_VAR_NAME,
                  &gPxeControlVariableGuid,
                  NULL,
                  &Size,
                  &PxeControl
                  );
  if (EFI_ERROR(Status)) {
    PxeControl.WaitSeconds = 0;
    PxeControl.DetectMediaTimes = 1;
  }
  //
  // If the NetworkDxe module is already loaded, PXE Boot Wait Time and Detect Media Time under the Boot Policy Manager Form is not displayed.
  //

  Status = gBS->LocateProtocol(&gByoSetupNetworkFormsetGuid, NULL, &Interface);
  if (EFI_ERROR (Status) && CheckNetworkBootOption()){
    PxeControl.NetworkEnable = 1;
  } else {
    PxeControl.NetworkEnable = 0;
  }
  Status = gRT->SetVariable (
                  PXE_CONTROL_VAR_NAME,
                  &gPxeControlVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  Size,
                  &PxeControl
                  );
  ASSERT_EFI_ERROR (Status);
  //
  // Init Enable status of MenuCtxs and Items of the global BMM_FAKE_NV_DATA
  //
  FakeNvData = &BmmCallbackInfo->BmmFakeNvData;
  for (TypeIndex = BM_BOOT_TYPE_MIN; TypeIndex < BM_BOOT_TYPE_MAX; TypeIndex++) {
    for (Index = 0; Index <= gMaxPosition[TypeIndex]; Index++) {
      MenuCtx = FindBmMenuCtxEntryByPosition(Index, TypeIndex);
      if (MenuCtx == NULL) {
        continue;
      }

      if (TypeIndex == BM_BOOT_TYPE_UEFI) {
        *((BOOLEAN *)((UINTN)FakeNvData + STATUS_UEFI_BOOT_GROUP_VAR_OFFSET) + MenuCtx->Position) = !MenuCtx->Enable;
      } else {
        *((BOOLEAN *)((UINTN)FakeNvData + STATUS_LEGACY_BOOT_GROUP_VAR_OFFSET) + MenuCtx->Position) = !MenuCtx->Enable;
      }

      for (Count = 0; Count < MenuCtx->DevCount; Count++) {
        *((BOOLEAN *)((UINTN)FakeNvData + MenuCtx->OptionVarOffset) + Count) = !(MenuCtx->Item[Count].Enable);
      }
    }
  }

  return;
}

EFI_STATUS
EFIAPI
BmInit (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  VOID             *Registration;
  EFI_STATUS       Status;
  OPTION_SORT_POLICY  *SortPolicy = NULL;
  UINTN               VarSize;

  Registration = NULL;
  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    SetupEnterCallback,
    NULL,
    &Registration
    );

  //
  // Set customized sort policy
  Status = GetVariable2 (
                  BYO_CUSTOMIZED_SORT_POLICY_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  (VOID **)&SortPolicy,
                  &VarSize
                  );
  if (EFI_ERROR(Status)) {
    VarSize = sizeof(mDefaultOptionSortOption);
    SortPolicy = (OPTION_SORT_POLICY *)AllocateCopyPool(VarSize, &mDefaultOptionSortOption);
    ASSERT(SortPolicy != NULL);
    Status = gRT->SetVariable (
                      BYO_CUSTOMIZED_SORT_POLICY_VAR_NAME,
                      &gByoGlobalVariableGuid,
                      BYO_SORT_POLICY_VAR_ATTR,
                      VarSize,
                      SortPolicy
                      );
    ASSERT(!EFI_ERROR(Status));
  }
  if (SortPolicy != NULL) {
    FreePool(SortPolicy);
  }
  
  return EFI_SUCCESS;
}

VOID
CloseUpdateMenuCtx (
  BMM_CALLBACK_DATA    *Private
  )
{
  BM_MENU_CTX                 *MenuCtx;
  BOOLEAN                     Rc;
  BMM_FAKE_NV_DATA            *FakeNvData;
  BM_BOOT_TYPE                TypeIndex;
  UINT16                      Index, Count;
  BOOLEAN                     ItemDisabled;
  BOOLEAN                     NeedEnableAllItems;

  Rc = HiiGetBrowserData(&mBootMaintenanceManagerGuid, BYO_BOOT_MAINT_VAR_NAME, sizeof(BMM_FAKE_NV_DATA), (UINT8*)&Private->BmmFakeNvData);
  if (!Rc) {
    DEBUG((EFI_D_INFO, "HiiGetBrowserData failed\n"));
    return;
  }

  FakeNvData = &Private->BmmFakeNvData;
  for (TypeIndex = 0; TypeIndex < BM_BOOT_TYPE_CTX_COUNT; TypeIndex++) {
    if (!mBmBootTypeCtx[TypeIndex].Enable) {
      continue;
    }
    for (Index = 0; Index <= gMaxPosition[TypeIndex]; Index++) {
      MenuCtx = FindBmMenuCtxEntryByPosition(Index, TypeIndex);
      if (MenuCtx == NULL) {
        continue;
      }

      //
      // Update MenuCtxs
      //
      NeedEnableAllItems = FALSE;
      if (TypeIndex == BM_BOOT_TYPE_UEFI) {
        if (!MenuCtx->Enable && ((!FakeNvData->StatusUefiBootGroup[MenuCtx->Position]) == TRUE)) { // enable current GroupType
          NeedEnableAllItems = TRUE;
        }
        MenuCtx->Enable = (!FakeNvData->StatusUefiBootGroup[MenuCtx->Position]);
      } else {
        if (!MenuCtx->Enable && ((!FakeNvData->StatusLegacyBootGroup[MenuCtx->Position]) == TRUE)) { // enable current GroupType
          NeedEnableAllItems = TRUE;
        }
        MenuCtx->Enable = (!FakeNvData->StatusLegacyBootGroup[MenuCtx->Position]);
      }

      for (Count = 0; Count < MenuCtx->DevCount; Count++) {
        if (NeedEnableAllItems) {
          *((BOOLEAN *)((UINTN)FakeNvData + MenuCtx->OptionVarOffset) + Count) = FALSE; // enable items
        }
        ItemDisabled = *((BOOLEAN *)((UINTN)FakeNvData + MenuCtx->OptionVarOffset) + Count);
        MenuCtx->Item[Count].Enable = (!ItemDisabled);
      }
    }
  }
  HiiSetBrowserData(
      &mBootMaintenanceManagerGuid,
      BYO_BOOT_MAINT_VAR_NAME,
      sizeof(BMM_FAKE_NV_DATA), 
      (UINT8*)&Private->BmmFakeNvData, 
      NULL
      );
}

VOID
AddNewBootItem (
  IN EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption
  )
{
  BM_MENU_TYPE                  GroupType;
  BM_BOOT_DEV_ITEM              *Item;
  UINT16                        *p;
  BM_MENU_CTX                   *MenuCtx;
  BMM_FAKE_NV_DATA              *FakeNvData;
  UINTN                         Index;

  FakeNvData = &BmmCallbackInfo->BmmFakeNvData;
  GroupType = GetGroupTypeOfAllTypeOptions(BootOption);
  for (Index = 0;Index < gNvBootOptionCount;Index++) {
    if (BootOption->OptionNumber == gNvBootOptions[Index].OptionNumber) {
      break;
    }
  }
  ASSERT(Index < gNvBootOptionCount);
  MenuCtx = FindBmMenuCtxEntry(GroupType);
  if (MenuCtx != NULL) {
    if (MenuCtx->DevCount < MAX_MENU_NUMBER) {
      Item = &MenuCtx->Item[MenuCtx->DevCount];
      Item->LoadOption = &gNvBootOptions[Index];
      Item->DisplayString = Item->LoadOption->Description;
      Item->HelpString    = BmUiDevicePathToStr (BootOption->FilePath);
      Item->DisplayStringToken = HiiSetString(BmmCallbackInfo->BmmHiiHandle, 0, Item->DisplayString, NULL);
      Item->HelpStringToken = HiiSetString (BmmCallbackInfo->BmmHiiHandle, 0, Item->HelpString, NULL);
      p = (UINT16*)(((UINTN)FakeNvData) + MenuCtx->OrderVarOffset) + MenuCtx->DevCount;
      *p = (UINT16)Item->LoadOption->OptionNumber + 1;
      Item->Value = *p;
      Item->Enable = TRUE;
      *((BOOLEAN *)((UINTN)FakeNvData + MenuCtx->OptionVarOffset) + MenuCtx->DevCount) = !Item->Enable;
      if (MenuCtx->BootType == BM_BOOT_TYPE_UEFI) {
        *((BOOLEAN *)((UINTN)FakeNvData + STATUS_UEFI_BOOT_GROUP_VAR_OFFSET) + MenuCtx->Position) = !MenuCtx->Enable;
      } else {
        *((BOOLEAN *)((UINTN)FakeNvData + STATUS_LEGACY_BOOT_GROUP_VAR_OFFSET) + MenuCtx->Position) = !MenuCtx->Enable;
      }
      MenuCtx->DevCount++;
    }
    //
    //Save the new boot option information 
    //
    HiiSetBrowserData(
      &mBootMaintenanceManagerGuid, 
      BYO_BOOT_MAINT_VAR_NAME, 
      sizeof(BMM_FAKE_NV_DATA), 
      (UINT8*)FakeNvData,
      NULL
      );
  }
}

VOID
UpdateItem(
  VOID
  )
{
  UINT16                        Index;
  UINT16                        Index1;
  EFI_BOOT_MANAGER_LOAD_OPTION  *Option;
  BM_MENU_CTX                   *MenuCtx;
  BM_BOOT_DEV_ITEM              *Item;
  BM_MENU_TYPE                  GroupOrder;

  if (gNvBootOptionCount == 0) {
    return;
  }

  Option = gNvBootOptions;
  for (Index = 0; Index < gNvBootOptionCount; Index++) {
    if ((Option[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) {
      continue;
    }

    if ((Option[Index].Attributes & LOAD_OPTION_CATEGORY) != LOAD_OPTION_CATEGORY_BOOT) {
      continue;
    }
    GroupOrder = GetGroupTypeOfAllTypeOptions(&Option[Index]);
    MenuCtx = FindBmMenuCtxEntry(GroupOrder);
    if (MenuCtx == NULL) {
      continue;
    }
    for (Index1=0;Index1<MenuCtx->DevCount;Index1++) {
      if (Option[Index].OptionNumber == MenuCtx->Item[Index1].Value -1) {
        Item = &MenuCtx->Item[Index1];
        Item->LoadOption = &Option[Index];
        Item->DisplayString = Option[Index].Description;
        if (Item->HelpString != NULL) {
          FreePool(Item->HelpString);
        }
        Item->HelpString = BmUiDevicePathToStr (Option[Index].FilePath);
        Item->DisplayStringToken = HiiSetString(BmmCallbackInfo->BmmHiiHandle, 0, Item->DisplayString, NULL);
        Item->HelpStringToken = HiiSetString (BmmCallbackInfo->BmmHiiHandle, 0, Item->HelpString, NULL);
        break;
      }
    }
  }

  return ;
}

VOID
UpdateDeletePage(
  VOID
  )
{
  UINT16                          MaxPosition = 0;
  BM_MENU_CTX                     *MenuCtx;
  EFI_BOOT_MANAGER_LOAD_OPTION    *BootOption;
  UINT16                          Index;
  UINTN                           Count;
  EFI_QUESTION_ID                 QuestionId;
  EFI_IFR_GUID_LABEL              *StartLabel;
  EFI_IFR_GUID_LABEL              *EndLabel;
  VOID                            *StarOpCodeHandle;
  VOID                            *EndOpCodeHandle;
  EFI_STATUS                      Status;
  EFI_HANDLE                      FvHandle;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;

  StarOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StarOpCodeHandle != NULL);
  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (StarOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number = DELETEL_OPTIONS_ID;
  EndLabel = (EFI_IFR_GUID_LABEL *)HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number = DELETEL_OPTIONS_END_ID;

  MaxPosition = gMaxPosition[BM_BOOT_TYPE_UEFI];
  for (Index = 0; Index <= MaxPosition; Index++) {
    MenuCtx = FindBmMenuCtxEntryByPosition(Index, BM_BOOT_TYPE_UEFI);
    for (Count = 0; Count < MenuCtx->DevCount; Count++) {
      BootOption = MenuCtx->Item[Count].LoadOption;
      if (BmIsAutoCreateBootOption(BootOption)) {
        continue;
      }
      //
      //Skip Internal Shell
      //
      TempDevicePath = BootOption->FilePath;
      Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &TempDevicePath, &FvHandle);
      if (!EFI_ERROR (Status)) {
        continue;
      }
      QuestionId = (EFI_QUESTION_ID)(BASE_DELETE_OPTION_QUESTION_ID + BootOption->OptionNumber);
      ASSERT(!(QuestionId < MIN_DELETE_OPTION_QUESTION_ID) && !(QuestionId >= MAX_DELETE_OPTION_QUESTION_ID));
      HiiCreateActionOpCode (
        StarOpCodeHandle,
        QuestionId,
        HiiSetString(BmmCallbackInfo->BmmHiiHandle,0,BootOption->Description,NULL),
        HiiSetString(BmmCallbackInfo->BmmHiiHandle,0,BmUiDevicePathToStr(BootOption->FilePath),NULL),
        EFI_IFR_FLAG_CALLBACK,
        0
        );
    }
  }
  HiiUpdateForm (
    BmmCallbackInfo->BmmHiiHandle,
    &mBootMaintenanceManagerGuid,
    FORM_DELETE_BOOT_OPTIONS,
    StarOpCodeHandle,
    EndOpCodeHandle
    );
  HiiFreeOpCodeHandle(StarOpCodeHandle);
  HiiFreeOpCodeHandle(EndOpCodeHandle);
}

/**
  This function processes the results of changes in configuration.


  @param This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param Action             Specifies the type of action taken by the browser.
  @param QuestionId         A unique value which is sent to the original exporting driver
                            so that it can identify the type of data to expect.
  @param Type               The type of value for the question.
  @param Value              A pointer to the data being sent to the original exporting driver.
  @param ActionRequest      On return, points to the action requested by the callback function.

  @retval EFI_SUCCESS           The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES  Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR      The variable could not be saved.
  @retval EFI_UNSUPPORTED       The specified Action is not supported by the callback.
  @retval EFI_INVALID_PARAMETER The parameter of Value or ActionRequest is invalid.
**/
EFI_STATUS
EFIAPI
BootMaintCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL         *This,
  IN        EFI_BROWSER_ACTION                     Action,
  IN        EFI_QUESTION_ID                        QuestionId,
  IN        UINT8                                  Type,
  IN        EFI_IFR_TYPE_VALUE                     *Value,
  OUT       EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  BM_BOOT_TYPE                BootType;
  BM_MENU_CTX                 *MenuCtx;
  BM_MENU_CTX                 *MyMenuCtx;
  UINT16                      OldPosition;
  UINT16                      NewPosition;
  BMM_CALLBACK_DATA           *Private;
  BOOLEAN                     Rc;
  UINT16                      UpdateStartOpCodeCtx, UpdateEndOpCodeCtx;
  BMM_FAKE_NV_DATA            InputData;
  EFI_BOOT_MANAGER_LOAD_OPTION  NewOption;
  EFI_BOOT_MANAGER_LOAD_OPTION  *OldOptions = NULL;
  UINTN                         OldOptionsCount = 0;
  UINT16                        EmptyBootDescriptionData[MAX_BOOT_DESCRIPTION_LEN];
  EFI_DEVICE_PATH_PROTOCOL      *FilePath = NULL;
  CHAR16                        *Str = NULL;
  UINTN                         Index;
  UINTN                         DeleteIndex;
  SELECTION_TYPE                UserSelection;
  BM_MENU_TYPE                  GroupType;
  UINTN                         OptionNumber;
  CHAR16                        *Name = NULL;
  BMM_FAKE_NV_DATA              *FakeNvData;
  UINT16                        *Position;
  EFI_BOOT_MANAGER_LOAD_OPTION  BootOption;

  DEBUG((EFI_D_INFO, "BMCB A:%X Q:%X\n", Action, QuestionId));

  if (Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    Status = EFI_SUCCESS;
    goto ProcExit;
  }

  if (Action == EFI_BROWSER_ACTION_SUBMITTED) {
    Status = EFI_SUCCESS;
    goto ProcExit;
  }

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN) {
    if ((QuestionId != BOOT_ORDER_PAGE_QUESTION_ID) && (QuestionId != DISABLE_GROUP_TYPE_PAGE_QUESTION_ID)
      && (QuestionId != DISABLE_OPTIONS_PAGE_QUESTION_ID)) {
      goto ProcExit;
    }

    switch (QuestionId) {
      case BOOT_ORDER_PAGE_QUESTION_ID:
        if (mFirstEnterBootOrderPage) {
          mFirstEnterBootOrderPage = FALSE;
        }

        if (mFirstEnterBootOrderPage) {
          //
          // Update all BootOrder items and forms
          //
          UpdateStartOpCodeCtx = UEFI_BOOT_ITEM_OPCODE_CTX_ID;
          UpdateEndOpCodeCtx = DISABLE_BOOT_OPTIONS_OPCODE_CTX_ID;
        } else {
          Private = BMM_CALLBACK_DATA_FROM_THIS(This);
          CloseUpdateMenuCtx(Private);
          UpdateStartOpCodeCtx = UEFI_BOOT_ITEM_OPCODE_CTX_ID;
          UpdateEndOpCodeCtx = LEGACY_BOOT_ITEM_OPCODE_CTX_ID;
        }
        break;
      case DISABLE_GROUP_TYPE_PAGE_QUESTION_ID:
        UpdateStartOpCodeCtx = DISABLE_UEFI_GROUP_OPCODE_CTX_ID;
        UpdateEndOpCodeCtx = DISABLE_LEGACY_GROUP_OPCODE_CTX_ID;
        break;
      case DISABLE_OPTIONS_PAGE_QUESTION_ID:
        UpdateStartOpCodeCtx = DISABLE_BOOT_OPTIONS_OPCODE_CTX_ID;
        UpdateEndOpCodeCtx = UpdateStartOpCodeCtx;
        break;
      default:
        break;
    }

    UpdatePages(UpdateStartOpCodeCtx, UpdateEndOpCodeCtx);
    goto ProcExit;
  }

  if (Action == EFI_BROWSER_ACTION_CHANGED) {
    if ((QuestionId > MIN_DELETE_OPTION_QUESTION_ID) && (QuestionId < MAX_DELETE_OPTION_QUESTION_ID)) {
      FakeNvData = &BmmCallbackInfo->BmmFakeNvData;
      OptionNumber = QuestionId - BASE_DELETE_OPTION_QUESTION_ID;
      Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_DELETE_BOOT_OPTION1), NULL);
      UserSelection = UiConfirmDialog(DIALOG_YESNO, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
      FreePool(Str);
      if (UserSelection == SELECTION_YES) {
        Name = AllocatePool(sizeof(L"Boot####"));
        UnicodeSPrint(Name,sizeof(L"Boot####"),L"Boot%04X",OptionNumber);
        Status = EfiBootManagerVariableToLoadOption(Name,&BootOption);
        if (!EFI_ERROR(Status)) {
          DEBUG((DEBUG_INFO,"Found %s\n",Name));
        }
        FreePool(Name);
        ASSERT_EFI_ERROR(Status);
        GroupType = GetGroupTypeOfAllTypeOptions(&BootOption);
        MenuCtx = FindBmMenuCtxEntry(GroupType);
        for (Index = 0;Index < MenuCtx->DevCount;Index++) {
          if (MenuCtx->Item[Index].Value - 1 == OptionNumber) {
            DeleteIndex = Index;
            DEBUG((DEBUG_INFO,"Delete %s Item\n",MenuCtx->Item[Index].LoadOption->Description));
            ZeroMem(&MenuCtx->Item[Index],sizeof(BM_BOOT_DEV_ITEM));
            while(DeleteIndex < MenuCtx->DevCount - 1) {
              CopyMem(&MenuCtx->Item[DeleteIndex],&MenuCtx->Item[DeleteIndex + 1],sizeof(BM_BOOT_DEV_ITEM));
              Position = (UINT16*)(((UINTN)FakeNvData) + MenuCtx->OrderVarOffset) + DeleteIndex;
              *Position = MenuCtx->Item[DeleteIndex].Value;
              DeleteIndex ++;
            }
            MenuCtx->DevCount--;
            HiiSetBrowserData(
              &mBootMaintenanceManagerGuid, 
              BYO_BOOT_MAINT_VAR_NAME, 
              sizeof(BMM_FAKE_NV_DATA), 
              (UINT8*)FakeNvData,
              NULL
              );
            break;
          }
        }
        EfiBootManagerDeleteLoadOptionVariable (OptionNumber, LoadOptionTypeBoot);
        DEBUG((DEBUG_INFO,"Delete Boot%04x Boot Option Success\n",OptionNumber));
        if (gNvBootOptions != NULL) {
          EfiBootManagerFreeLoadOptions(gNvBootOptions,gNvBootOptionCount);
        }
        gNvBootOptions = EfiBootManagerGetLoadOptions(&gNvBootOptionCount, LoadOptionTypeBoot);
        UpdateItem();
        UpdateDeletePage();
      }
      goto ProcExit;
    }
    switch (QuestionId) {
    case FORM_ADD_OPTIONS_ID:
      ChooseFile (NULL, L".efi", NULL, &FilePath);
      if (FilePath != NULL) {
        mFilePath = FilePath;
        HiiSetString(gBmmCallbackInfo.BmmHiiHandle,STRING_TOKEN(STR_SELECT_BOOT_FILE_HELP),BmUiDevicePathToStr(FilePath),NULL);
      }
      break;

    case KEY_VALUE_NO_SAVE_AND_EXIT_BOOT:
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD_EXIT;
      break;

    case KEY_VALUE_SAVE_AND_EXIT_BOOT:
      //
      //The user did not select the boot file
      //
      if (mFilePath == NULL) {
        Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_USER_INPUT_FILE_NULL), NULL);
        UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
        FreePool(Str);
        goto ProcExit;
      }
      HiiGetBrowserData(&mBootMaintenanceManagerGuid, BYO_BOOT_MAINT_VAR_NAME, sizeof(BMM_FAKE_NV_DATA), (UINT8*)&InputData);
      ZeroMem(EmptyBootDescriptionData,MAX_BOOT_DESCRIPTION_SIZE);
      //
      //The user input BootDescription is empty
      //
      if (CompareMem(EmptyBootDescriptionData,InputData.BootDescriptionData,MAX_BOOT_DESCRIPTION_SIZE) == 0) {
        Str = HiiGetString(gHiiHandle, STRING_TOKEN(STR_USER_INPUT_DESCRIPTION_NULL), NULL);
        UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
        FreePool(Str);
        goto ProcExit;
      }
      InputData.BootDescriptionData[MAX_BOOT_DESCRIPTION_LEN] = L'\0';
      //
      //Check to see if it matches an already existing boot option
      //
      OldOptions = EfiBootManagerGetLoadOptions(&OldOptionsCount, LoadOptionTypeBoot);
      for (Index = 0;Index < OldOptionsCount;Index++) {
        if ((OldOptions[Index].Attributes & LOAD_OPTION_CATEGORY_APP) != 0) {
          continue;
        }
        if (StrCmp(OldOptions[Index].Description,(CHAR16*)InputData.BootDescriptionData) == 0) {
          Str =HiiGetString(gHiiHandle, STRING_TOKEN(STR_BOOT_DESCRIPTION_SAME), NULL);
          UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
          FreePool(Str);
          goto ProcExit;
        }
        if (UefiCompareDevicePath(OldOptions[Index].FilePath,mFilePath)) {
          Str =HiiGetString(gHiiHandle, STRING_TOKEN(STR_BOOT_FILE_PATH_SAME), NULL);
          UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str, NULL);
          FreePool(Str);
          goto ProcExit;
        }
      }
      //
      //Initialize the boot option
      //
      Status = EfiBootManagerInitializeLoadOption (
              &NewOption,
              LoadOptionNumberUnassigned,
              LoadOptionTypeBoot,
              LOAD_OPTION_ACTIVE,
              (CHAR16*)InputData.BootDescriptionData,
              mFilePath,
              NULL,
              0
              );
      Status = EfiBootManagerAddLoadOptionVariable (&NewOption, MAX_UINTN);
      ASSERT_EFI_ERROR (Status);
      if (gNvBootOptions != NULL) {
        EfiBootManagerFreeLoadOptions(gNvBootOptions,gNvBootOptionCount);
      }
      gNvBootOptions = EfiBootManagerGetLoadOptions(&gNvBootOptionCount, LoadOptionTypeBoot);
      UpdateItem();
      AddNewBootItem(&NewOption);
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_FORM_DISCARD_EXIT;
      break;
    default:
      break;
    }
    //
    //Clears the display of selected file names,and Change it to option help information
    //
    if ((QuestionId == KEY_VALUE_NO_SAVE_AND_EXIT_BOOT) || (QuestionId == KEY_VALUE_SAVE_AND_EXIT_BOOT)) {
      Str = HiiGetString(gBmmCallbackInfo.BmmHiiHandle,STRING_TOKEN(STR_SELECT_BOOT_FILE_HELP_BACKUP),"zh-Hans");
      HiiSetString(gBmmCallbackInfo.BmmHiiHandle,STRING_TOKEN(STR_SELECT_BOOT_FILE_HELP),Str,"zh-Hans");
      FreePool(Str);
      Str = HiiGetString(gBmmCallbackInfo.BmmHiiHandle,STRING_TOKEN(STR_SELECT_BOOT_FILE_HELP_BACKUP),"en-US");
      HiiSetString(gBmmCallbackInfo.BmmHiiHandle,STRING_TOKEN(STR_SELECT_BOOT_FILE_HELP),Str,"en-US");
      FreePool(Str);
      if (mFilePath != NULL) {
        FreePool(mFilePath);
        mFilePath= NULL;
      }
    }
    if ((QuestionId == FORM_ADD_OPTIONS_ID) || (QuestionId == KEY_VALUE_NO_SAVE_AND_EXIT_BOOT) || (QuestionId == KEY_VALUE_SAVE_AND_EXIT_BOOT)) {
      Status = EFI_SUCCESS;
      goto ProcExit;
    }
  }

  if (Value == NULL || ActionRequest == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  if (Action == EFI_BROWSER_ACTION_CHANGED) {
    MenuCtx = FindBmMenuCtxEntryByQuestionId(QuestionId);
    if(MenuCtx == NULL){
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }

    Private = BMM_CALLBACK_DATA_FROM_THIS(This);
    Rc = HiiGetBrowserData(&mBootMaintenanceManagerGuid, BYO_BOOT_MAINT_VAR_NAME, sizeof(BMM_FAKE_NV_DATA), (UINT8*)&Private->BmmFakeNvData);
    DEBUG((EFI_D_INFO, "HiiGetBrowserData:%d\n", Rc));
  }

  if (Action == BROWSER_ACTION_MOVE_UP || Action == BROWSER_ACTION_MOVE_DOWN) {
    Private = BMM_CALLBACK_DATA_FROM_THIS(This);
    Rc = HiiGetBrowserData(&mBootMaintenanceManagerGuid, BYO_BOOT_MAINT_VAR_NAME, sizeof(BMM_FAKE_NV_DATA), (UINT8*)&Private->BmmFakeNvData);
    DEBUG((EFI_D_INFO, "HiiGetBrowserData:%d\n", Rc));

    MenuCtx = FindBmMenuCtxEntryByQuestionId(QuestionId);
    if(MenuCtx == NULL){
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }
    BootType = MenuCtx->BootType;

    do {
      OldPosition = MenuCtx->Position;

      if(Action == BROWSER_ACTION_MOVE_UP){
        if(MenuCtx->Position == 0){
        HiiSetBrowserData(
          &mBootMaintenanceManagerGuid,
          BYO_BOOT_MAINT_VAR_NAME,
          sizeof(BMM_FAKE_NV_DATA),
          (UINT8*)&Private->BmmFakeNvData,
          NULL
          );

        UpdatePages(BootType, BootType);
          Status = EFI_UNSUPPORTED;
          goto ProcExit;
        }
        NewPosition = MenuCtx->Position - 1;
      } else {
        if(MenuCtx->Position == gMaxPosition[BootType]){
          Status = EFI_UNSUPPORTED;
          goto ProcExit;
        }
        NewPosition = MenuCtx->Position + 1;
      }
  
      MyMenuCtx = FindBmMenuCtxEntryByPosition(NewPosition, BootType);
      MenuCtx = FindBmMenuCtxEntryByPosition(OldPosition, BootType);
      MyMenuCtx->Position = OldPosition;
      MenuCtx->Position   = NewPosition;
  
      if (BootType == BM_BOOT_TYPE_UEFI) {
        *((BOOLEAN *)((UINTN)&Private->BmmFakeNvData + STATUS_UEFI_BOOT_GROUP_VAR_OFFSET) + OldPosition) = !MyMenuCtx->Enable;
        *((BOOLEAN *)((UINTN)&Private->BmmFakeNvData + STATUS_UEFI_BOOT_GROUP_VAR_OFFSET) + NewPosition) = !MenuCtx->Enable;
      } else {
        *((BOOLEAN *)((UINTN)&Private->BmmFakeNvData + STATUS_LEGACY_BOOT_GROUP_VAR_OFFSET) + OldPosition) = !MyMenuCtx->Enable;
        *((BOOLEAN *)((UINTN)&Private->BmmFakeNvData + STATUS_LEGACY_BOOT_GROUP_VAR_OFFSET) + NewPosition) = !MenuCtx->Enable;
      }
    } while (!MyMenuCtx->Enable);

    HiiSetBrowserData(
      &mBootMaintenanceManagerGuid,
      BYO_BOOT_MAINT_VAR_NAME,
      sizeof(BMM_FAKE_NV_DATA), 
      (UINT8*)&Private->BmmFakeNvData, 
      NULL
      );

    UpdatePages(BootType, BootType);
  } else if (Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD) {
    MenuCtx = FindBmMenuCtxEntryByQuestionId(QuestionId);
    if (MenuCtx == NULL) {
      DEBUG((EFI_D_INFO, "(L%d)\n", __LINE__));
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }
    if (MenuCtx->DevCount == 0) {
      DEBUG((EFI_D_INFO, "(L%d)\n", __LINE__));
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }
  }

ProcExit:
  if (OldOptions != NULL) {
    EfiBootManagerFreeLoadOptions(OldOptions,OldOptionsCount);
  }
  return Status;
}


STATIC
VOID
RestoreItemOrder (
  BM_MENU_CTX                    *MenuCtx
  )
{
  UINTN                          i;
  UINTN                          Index;
  BM_BOOT_DEV_ITEM               *Item;
  EFI_BOOT_MANAGER_LOAD_OPTION   *Option;
  UINTN                          OptionCount;
  UINTN                          Count;
  UINTN                          VarBase;
  UINT16                         *p;
  EFI_STATUS                     Status;
  UINTN                          BufferSize = 0;
  UINTN                          *BootOptionOrder = NULL;
  BOOLEAN                        FreeOption = FALSE;

  DEBUG((EFI_D_INFO, "%a %X %X\n", __FUNCTION__, MenuCtx->DevCount, MenuCtx->MenuType));

  for (Index = 0; Index < MenuCtx->DevCount; Index++) {
    MenuCtx->Item[Index].Enable = TRUE;
  }

  if(MenuCtx->DevCount < 2){
    return;
  }

  if (MenuCtx->BootType == BM_BOOT_TYPE_UEFI) {
    Status = gRT->GetVariable (L"BootOptionOrder", &gEdkiiBdsVariableGuid, NULL, &BufferSize, BootOptionOrder);
    if (Status == EFI_BUFFER_TOO_SMALL) {
      BootOptionOrder = AllocatePool (BufferSize);
      Status = gRT->GetVariable (L"BootOptionOrder", &gEdkiiBdsVariableGuid, NULL, &BufferSize, BootOptionOrder);
    }
    if (EFI_ERROR(Status)) {
      Option = mUefiOptions;
      OptionCount = mUefiOptionsCount;
    } else {
      Option = (EFI_BOOT_MANAGER_LOAD_OPTION *)AllocateZeroPool(sizeof(EFI_BOOT_MANAGER_LOAD_OPTION) * mUefiOptionsCount);
      ASSERT(Option != NULL);
      FreeOption = TRUE;
      OptionCount = 0;
      for (Index = 0; Index < BufferSize / sizeof(UINTN); Index++) {
        for (i = 0; i < mUefiOptionsCount; i++) {
          if (mUefiOptions[i].OptionNumber == BootOptionOrder[Index]) {
            ByoCopyBootOption (Option + OptionCount, mUefiOptions + i);
            OptionCount++;
          }
        }
      }
    }
  } else if (MenuCtx->BootType == BM_BOOT_TYPE_LEGACY) {
    Option = mLegacyOptions;
    OptionCount = mLegacyOptionsCount;
  } else {
    Option = NULL;
    OptionCount = 0;
  }
  if (BootOptionOrder != NULL) {
    FreePool(BootOptionOrder);
  }
  DEBUG((EFI_D_INFO, "Option(%X,%X)\n", Option, OptionCount));


  for (i = 0; i < MenuCtx->DevCount; i++) {
    Item = &MenuCtx->Item[i];
    Item->Flag = 0;
    DEBUG((EFI_D_INFO, "Item->Value:%X\n", Item->Value));
  }

  p = AllocateZeroPool(MenuCtx->DevCount * sizeof(UINT16));
  ASSERT(p != NULL);

  VarBase = (UINTN)&BmmCallbackInfo->BmmFakeNvData;
  DumpMem8((UINT16*)(VarBase + MenuCtx->OrderVarOffset), MenuCtx->DevCount * sizeof(UINT16));

  Count = 0;
  for (Index = 0; Index < OptionCount; Index++) {

    if (GetGroupTypeOfAllTypeOptions(&Option[Index]) != MenuCtx->MenuType) {
      continue;
    }

    for (i = 0; i < MenuCtx->DevCount; i++) {
      Item = &MenuCtx->Item[i];
      if (Item->Flag) {
        continue;
      }
      if (CompareMem(Option[Index].FilePath, Item->LoadOption->FilePath, GetDevicePathSize(Option[Index].FilePath)) == 0) {
        p[Count++] = (UINT16)(Item->LoadOption->OptionNumber + 1);
        Item->Flag = 1;
      }
    }
  }

  DumpMem8(p, MenuCtx->DevCount * sizeof(UINT16));

  for (i = 0; i < MenuCtx->DevCount; i++) {
    Item = &MenuCtx->Item[i];
    if (Item->Flag) {
      continue;
    }
    p[Count++] = (UINT16)(Item->LoadOption->OptionNumber + 1);
    Item->Flag = 1;
  }

  //
  // Restore status of Items:enabled
  //
  for (Index = 0; Index < MenuCtx->DevCount; Index++) {
    *((BOOLEAN*)(VarBase + MenuCtx->OptionVarOffset) + Index) = FALSE;
  }

  CopyMem((UINT16*)(VarBase + MenuCtx->OrderVarOffset), p, MenuCtx->DevCount * sizeof(UINT16));
  FreePool(p);

  DumpMem8((UINT16*)(VarBase + MenuCtx->OrderVarOffset), MenuCtx->DevCount * sizeof(UINT16));

  if (FreeOption) {
    FreePool(Option);
  }
}






STATIC
VOID
RestoreUserItemOrder (
  BM_MENU_CTX                    *MenuCtx
  )
{
  UINTN                          i;
  UINTN                          Index;
  BM_BOOT_DEV_ITEM               *Item;
  UINTN                          Count;
  UINTN                          VarBase;
  UINT16                         *p;
  USER_BM_MENU_INFO              *Info;
  UINTN                          MenuIndex;


  DEBUG((EFI_D_INFO, "%a %X %X\n", __FUNCTION__, MenuCtx->DevCount, MenuCtx->MenuType));

  for (Index = 0; Index < MenuCtx->DevCount; Index++) {
    MenuCtx->Item[Index].Enable = TRUE;
  }
  if (MenuCtx->DevCount < 2) {
    return;
  }

  MenuIndex = MenuCtx - gBmMenuCtx;
  DEBUG((EFI_D_INFO, "MenuIndex:%d\n", MenuIndex));
  Info = &gUserBmMenuInfo[MenuIndex];

  for (i = 0; i < MenuCtx->DevCount; i++) {
    Item = &MenuCtx->Item[i];
    Item->Flag = 0;
    DEBUG((EFI_D_INFO, "Item->Value:%X\n", Item->Value));
  }
//for(i=0;i<Info->DevCount;i++){
//  Info->Flag[i] = 0;
//}

  p = AllocateZeroPool(MenuCtx->DevCount * sizeof(UINT16));
  ASSERT(p != NULL);

  VarBase = (UINTN)&BmmCallbackInfo->BmmFakeNvData;
  DumpMem8((UINT16*)(VarBase + MenuCtx->OrderVarOffset), MenuCtx->DevCount * sizeof(UINT16));

  Count = 0;
  for (Index = 0; Index < Info->DevCount; Index++) {
//  if(Info->Flag[Index]){
//    continue;
//  }
    for (i = 0; i < MenuCtx->DevCount; i++) {
      Item = &MenuCtx->Item[i];
      if (Item->Flag) {
        continue;
      }
      if (StrCmp(Info->DevName[Index], Item->DisplayString) == 0) {
        p[Count++] = Item->Value;
        Item->Flag = 1;
        break;
      }
    }
  }

  DumpMem8(p, MenuCtx->DevCount * sizeof(UINT16));

  for (i = 0; i < MenuCtx->DevCount; i++) {
    Item = &MenuCtx->Item[i];
    if (Item->Flag) {
      continue;
    }
    p[Count++] = Item->Value;;
    Item->Flag = 1;
  }

  for (Index = 0; Index < MenuCtx->DevCount; Index++) {
    *((BOOLEAN*)(VarBase + MenuCtx->OptionVarOffset) + Index) = FALSE;
  }
  CopyMem((UINT16*)(VarBase + MenuCtx->OrderVarOffset), p, MenuCtx->DevCount * sizeof(UINT16));
  FreePool(p);

  DumpMem8((UINT16*)(VarBase + MenuCtx->OrderVarOffset), MenuCtx->DevCount * sizeof(UINT16));
}







VOID RestoreBootOrder(BOOLEAN IsUserDefault)
{
  BM_MENU_CTX                    *MenuCtx;
  UINT16                         Index, Count;
  UINTN                          TypeIndex;
  BOOLEAN                        Rc;
  UINTN                          VarSize;
  UINT16                         *VarBuffer = NULL;
  BMM_FAKE_NV_DATA               *FakeNvData;
  EFI_STATUS                     UefiStatus, LegacyStatus;
  UINT16                         PlatformIndex;
  UINT8                          *LegacyDefaultGroupOrder = NULL;
  UINT8                          *UefiDefaultGroupOrder = NULL;
  UINTN                          LegacyDefaultGroupOrderSize = 0;
  UINTN                          UefiDefaultGroupOrderSize = 0;

  DEBUG((EFI_D_INFO, "%a(%d)\n", __FUNCTION__, IsUserDefault));
  MenuCtx = gBmMenuCtx;
  if(IsUserDefault){
    VarBuffer = BootManagerGetVariableAndSize(USER_BOOT_ORDER_VAR_NAME, &gByoGlobalVariableGuid, &VarSize);
    UserBmMenuInfoInit(VarBuffer, VarSize);
    if (VarBuffer == NULL) {
      return;
    }
  }

  //
  // Get default GroupOrder
  //
  UefiStatus = gRT->GetVariable(
                  BYO_UEFI_DEFAULT_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &UefiDefaultGroupOrderSize,
                  UefiDefaultGroupOrder
                  );
  if (UefiStatus == EFI_BUFFER_TOO_SMALL) {
    UefiDefaultGroupOrder = AllocatePool (UefiDefaultGroupOrderSize);
    ASSERT(UefiDefaultGroupOrder != NULL);
    UefiStatus = gRT->GetVariable(
                  BYO_UEFI_DEFAULT_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &UefiDefaultGroupOrderSize,
                  UefiDefaultGroupOrder
                  );
  }

  LegacyStatus = gRT->GetVariable(
                  BYO_LEGACY_DEFAULT_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &LegacyDefaultGroupOrderSize,
                  LegacyDefaultGroupOrder
                  );
  if (LegacyStatus == EFI_BUFFER_TOO_SMALL) {
    LegacyDefaultGroupOrder = AllocatePool (LegacyDefaultGroupOrderSize);
    ASSERT(LegacyDefaultGroupOrder != NULL);
    LegacyStatus = gRT->GetVariable(
                  BYO_LEGACY_DEFAULT_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &LegacyDefaultGroupOrderSize,
                  LegacyDefaultGroupOrder
                  );
  }
  if (!EFI_ERROR(UefiStatus) || !EFI_ERROR (LegacyStatus)) {
    if (!EFI_ERROR(UefiStatus)) {
      for (PlatformIndex = 0;PlatformIndex < BM_MENU_CTX_UEFI_COUNT;PlatformIndex++) {
        for (Index = 0;Index < BM_MENU_CTX_UEFI_COUNT;Index++) {
          if (gBmMenuCtx[Index].MenuType == UefiDefaultGroupOrder[PlatformIndex]) {
            gBmMenuCtx[Index].Position = PlatformIndex;
            break;
          }
        }
      }
    }

    if (!EFI_ERROR(LegacyStatus)) {
      for (PlatformIndex = 0;PlatformIndex < BM_MENU_CTX_LEGACY_COUNT;PlatformIndex++) {
        for (Index = 0;Index < BM_MENU_CTX_LEGACY_COUNT;Index++) {
          if (gBmMenuCtx[Index + BM_MENU_CTX_UEFI_COUNT].MenuType == LegacyDefaultGroupOrder[PlatformIndex]) {
            gBmMenuCtx[Index + BM_MENU_CTX_UEFI_COUNT].Position = PlatformIndex;
            break;
          }
        }
      }
    }
  } else {
    if (VarBuffer == NULL) {
      ZeroMem(gMaxPosition, sizeof(gMaxPosition));
      for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
        if (gBmMenuCtx[Index].Position != POSITION_OF_UNUSED_MENU_CTX) {
          gBmMenuCtx[Index].Position = gMaxPosition[gBmMenuCtx[Index].BootType]++;
        }
      }
      for (TypeIndex = 0; TypeIndex < BM_BOOT_TYPE_CTX_COUNT; TypeIndex++) {
        if (!mBmBootTypeCtx[TypeIndex].Enable) {
          continue;
        }
        if (gMaxPosition[TypeIndex]) {
          gMaxPosition[TypeIndex]--;
        }
      }
    } else {
      for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
        if (gBmMenuCtx[Index].Position != POSITION_OF_UNUSED_MENU_CTX) {
          gBmMenuCtx[Index].Position = gUserBmMenuInfo[Index].Position;
          }
      }
    }
  }

  Rc = HiiGetBrowserData(&mBootMaintenanceManagerGuid, BYO_BOOT_MAINT_VAR_NAME, sizeof(BMM_FAKE_NV_DATA), (UINT8*)&BmmCallbackInfo->BmmFakeNvData);
  DEBUG((EFI_D_INFO, "HiiGetBrowserData:%d, VarBuffer:%X\n", Rc, VarBuffer));

  FakeNvData = &BmmCallbackInfo->BmmFakeNvData;
  for (TypeIndex = 0; TypeIndex < BM_BOOT_TYPE_CTX_COUNT; TypeIndex++) {
    if (!mBmBootTypeCtx[TypeIndex].Enable) {
      continue;
    }
    for (Index = 0; Index <= gMaxPosition[TypeIndex]; Index++) {
      MenuCtx = FindBmMenuCtxEntryByPosition(Index, TypeIndex);
      if (MenuCtx == NULL) {
        continue;
      }
      MenuCtx->Enable = TRUE;

      if (TypeIndex == BM_BOOT_TYPE_UEFI) {
        FakeNvData->StatusUefiBootGroup[MenuCtx->Position] = FALSE;
      } else {
        FakeNvData->StatusLegacyBootGroup[MenuCtx->Position] = FALSE;
      }
      if (MenuCtx->DevCount > 0) {
        for (Count = 0; Count < MenuCtx->DevCount; Count++) {
          *((BOOLEAN *)((UINTN)FakeNvData + MenuCtx->OptionVarOffset) + Count) = FALSE;
        }
        if (VarBuffer == NULL) {
          RestoreItemOrder(MenuCtx);
        } else {
          RestoreUserItemOrder(MenuCtx);
        }
      }
    }
  }

  Rc = HiiSetBrowserData(&mBootMaintenanceManagerGuid, BYO_BOOT_MAINT_VAR_NAME, sizeof(BMM_FAKE_NV_DATA), (UINT8*)&BmmCallbackInfo->BmmFakeNvData, NULL);
  DEBUG((EFI_D_INFO, "HiiSetBrowserData:%d\n", Rc));

  if (VarBuffer != NULL) {
    FreePool(VarBuffer);
  }

  UpdatePages(0, BM_OPCODE_CTX_COUNT - 1);

  if (LegacyDefaultGroupOrder != NULL) {
    FreePool(LegacyDefaultGroupOrder);
  }
  if (UefiDefaultGroupOrder != NULL) {
    FreePool(UefiDefaultGroupOrder);
  }
}




UINT8 gLegacyBootGroupOrder[] = {
  BM_MENU_TYPE_LEGACY_HDD,
  BM_MENU_TYPE_LEGACY_ODD,
  BM_MENU_TYPE_LEGACY_USB_DISK,
  BM_MENU_TYPE_LEGACY_USB_ODD,
  BM_MENU_TYPE_LEGACY_PXE,
  };

UINT8 gUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD,
  BM_MENU_TYPE_UEFI_ODD,
  BM_MENU_TYPE_UEFI_USB_DISK,
  BM_MENU_TYPE_UEFI_USB_ODD,
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_OTHERS
  };




CHAR16 *GetUserBootOrderData(UINT16 *VarData, UINTN VarSize, UINT8 Position, BOOLEAN IsUefi, UINT16 *DevCount)
{
  UINT16  *p;
  UINT16  *e;
  UINT16  Pos;
  UINT16  Count;
  UINT16  i;
  UINTN   t, c;


  p = VarData;
  e = (UINT16*)((UINTN)VarData + VarSize);
  *DevCount = 0;

  if (IsUefi) {
    t = 0;
  } else {
    t = 1;
  }

  c = 0;
  while (p < e) {
    Pos = *p++;
    Count = *p++;

    if (Pos == Position) {
      if (c == t) {
        *DevCount = Count;
        return p;
      }
      c++;
    }

    for (i = 0; i < Count; i++) {
      p += StrLen(p) + 1;
    }
  }

  ASSERT(FALSE);
  return NULL;
}


BOOLEAN
IsThisNvDevPresent (
  CHAR16                         *DevName,
  EFI_BOOT_MANAGER_LOAD_OPTION   *Options,
  UINTN                          OptionCount,
  UINT16                         *OptionNumber
  )
{
  UINTN  Index;

  for(Index=0;Index<OptionCount;Index++){
    if(StrCmp(DevName, Options[Index].Description) == 0){
      *OptionNumber = (UINT16)Options[Index].OptionNumber;
      return TRUE;
    }
  }

  return FALSE;
}

BOOLEAN
IsThisOptionNumberPresent (
    UINT16     OptionNumber,
    UINT16     *Array,
    UINTN      ArrayCount
  )
{
  UINTN  Index;

  for (Index = 0; Index < ArrayCount; Index++) {
    if (OptionNumber == Array[Index]) {
      return TRUE;
    }
  }

  return FALSE;
}



VOID BootOrderLoadDefault()
{
  EFI_STATUS                     Status;
  UINTN                          VarSize;
  UINT16                         *VarBuffer = NULL;
  UINT16                         *BootOrder;
  UINTN                          BootOrderSize;
  UINTN                          Index, Count;
  CHAR16                         OptionName[sizeof("Boot####")];
  UINTN                          i, j;
  BM_MENU_CTX                    *MenuCtx;
  EFI_BOOT_MANAGER_LOAD_OPTION   *Options;
  UINTN                          OptionCount;
  UINT8                          GroupOrder[sizeof(gLegacyBootGroupOrder)];
  UINTN                          UefiGroupCount;
  USER_BM_MENU_INFO              *Info;
  UINT16                         *NewBootOrder = NULL;
  CHAR16                         *DevName;
  UINT16                          DevCount;
  UINT16                          OptionNumber;

  DEBUG((EFI_D_INFO, " %a line=%d\n", __FUNCTION__, __LINE__));

  UefiGroupCount = BM_MENU_CTX_UEFI_COUNT;

  VarBuffer = BootManagerGetVariableAndSize(USER_BOOT_ORDER_VAR_NAME, &gByoGlobalVariableGuid, &VarSize);
  if (VarBuffer == NULL) {
    Status = gRT->SetVariable (
                    BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    BYO_BG_ORDER_VAR_ATTR,
                    sizeof(gLegacyBootGroupOrder),
                    gLegacyBootGroupOrder
                    );
    ASSERT(!EFI_ERROR(Status));

    Status = gRT->SetVariable (
                    BYO_UEFI_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    BYO_BG_ORDER_VAR_ATTR,
                    sizeof(gUefiBootGroupOrder),
                    gUefiBootGroupOrder
                    );
    ASSERT(!EFI_ERROR(Status));

    BootOrder = BootManagerGetVariableAndSize(L"BootOrder", &gEfiGlobalVariableGuid, &BootOrderSize);
    if (BootOrder != NULL) {
      Count = BootOrderSize/sizeof(UINT16);
      for (Index = 0; Index < Count; Index++) {
        UnicodeSPrint(OptionName, sizeof(OptionName), L"Boot%04X", BootOrder[Index]);
        Status = gRT->SetVariable (
                        OptionName,
                        &gEfiGlobalVariableGuid,
                        EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                        0,
                        NULL
                        );
      }
      Status = gRT->SetVariable (
                      L"BootOrder",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );
      FreePool(BootOrder);
    }
  } else {
    UserBmMenuInfoInit(VarBuffer, VarSize);
    Info    = gUserBmMenuInfo;
    MenuCtx = gBmMenuCtx;
    for(Index=0;Index<UefiGroupCount;Index++){
      for(i=0;i<UefiGroupCount;i++){
        if(Info[i].Position == Index){
          GroupOrder[Index] = (UINT8)MenuCtx[i].MenuType;
          break;
        }
      }
    }
    DumpMem8(GroupOrder, sizeof(GroupOrder));
    Status = gRT->SetVariable (
                    BYO_UEFI_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    BYO_BG_ORDER_VAR_ATTR,
                    sizeof(GroupOrder),
                    GroupOrder
                    );
    ASSERT(!EFI_ERROR(Status));

    Info    = gUserBmMenuInfo + UefiGroupCount;
    MenuCtx = gBmMenuCtx      + UefiGroupCount;
    for (Index = 0; Index < UefiGroupCount; Index++) {
      for (i = 0; i < UefiGroupCount; i++) {
        if (Info[i].Position == Index) {
          GroupOrder[Index] = (UINT8)MenuCtx[i].MenuType;
          break;
        }
      }
    }
    DumpMem8(GroupOrder, sizeof(GroupOrder));
    Status = gRT->SetVariable (
                    BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    BYO_BG_ORDER_VAR_ATTR,
                    sizeof(GroupOrder),
                    GroupOrder
                    );
    ASSERT(!EFI_ERROR(Status));

    Options = EfiBootManagerGetLoadOptions(&OptionCount, LoadOptionTypeBoot);
    if (OptionCount != 0) {
      NewBootOrder = AllocatePool(OptionCount * sizeof(UINT16));
      i = 0;
      for (Index = 0; Index < BM_MENU_CTX_COUNT; Index++) {
        DevName = GetUserBootOrderData(VarBuffer, VarSize, (UINT8)(Index%UefiGroupCount), Index < UefiGroupCount, &DevCount);
        for (j = 0; j < DevCount; j++) {
          DEBUG((EFI_D_INFO, "DevName:%s\n", DevName));
          if (IsThisNvDevPresent(DevName, Options, OptionCount, &OptionNumber) &&
             !IsThisOptionNumberPresent(OptionNumber, NewBootOrder, i)) {
            NewBootOrder[i++] = OptionNumber;
          }
          DevName += StrLen(DevName) + 1;
        }
      }

      for (Index = 0; Index < OptionCount; Index++) {
        if (!IsThisOptionNumberPresent((UINT16)Options[Index].OptionNumber, NewBootOrder, i)) {
          UnicodeSPrint(OptionName, sizeof(OptionName), L"Boot%04X", Options[Index].OptionNumber);
          Status = gRT->SetVariable (
                          OptionName,
                          &gEfiGlobalVariableGuid,
                          EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                          0,
                          NULL
                          );
        }
      }

      DEBUG((EFI_D_INFO, "NewBootOrder:\n"));
      DumpMem8(NewBootOrder, i * sizeof(UINT16));
      Status = gRT->SetVariable (
                      L"BootOrder",
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      i * sizeof(UINT16),
                      NewBootOrder
                      );

      EfiBootManagerFreeLoadOptions(Options, OptionCount);
      FreePool(NewBootOrder);
    }
  }

  if (VarBuffer != NULL) {
    FreePool(VarBuffer);
  }
}

EFI_STATUS
BootConfigureSaveValue(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  BM_MENU_CTX    *MenuCtx;
  BM_BOOT_TYPE   BootType;
  UINT16         Position;
  UINT8          LegacyGroupOrder[BM_MENU_CTX_LEGACY_COUNT];
  UINT8          UefiGroupOrder[BM_MENU_CTX_UEFI_COUNT];
  UINT16         *NewBootOrder = NULL;
  UINTN          NewBootOrderIndex = 0;
  UINT16         *p;
  UINT16          i;
  BM_BOOT_DEV_ITEM  *Item;
  EFI_STATUS        Status;
  UINTN             Index;
  UINTN             BootOrderSize;
  UINT8             *GroupOrderList[BM_BOOT_TYPE_MAX];
  UINT8             GroupOrderIndex[BM_BOOT_TYPE_MAX];
  UINT8             IpmiBootMiscCfg;
  UINTN             VarSize;
  UINT16            *OldBootOrder = NULL;
  CHAR16            OptionName[OPTION_NAME_LEN];
  BOOLEAN           ItemDisabled;
  BOOLEAN           GroupDisabled;
  BOOLEAN           OptionActive;
  EFI_BOOT_MANAGER_LOAD_OPTION  Option;
  UINTN             Size;
  UINTN                       OrderIndex, AddedItemCount;

  DEBUG((EFI_D_INFO, "%a C:%d P:%d %d\n", __FUNCTION__, gNvBootOptionCount, gMaxPosition[0], gMaxPosition[1]));

  Size = sizeof(DISABLED_GROUP_TYPE);
  Status = gRT->GetVariable (BYO_DISABLED_GROUP_TYPE_VAR_NAME, &gByoDisableGroupTypeGuid, NULL, &Size, &mDisabledGroupType);

  BootOrderSize = gNvBootOptionCount * sizeof(UINT16);
  if (gNvBootOptionCount != 0) {
    NewBootOrder = AllocatePool(BootOrderSize);
    OldBootOrder = AllocatePool(BootOrderSize);
    ASSERT(NewBootOrder != NULL);
  }

  GroupOrderList[BM_BOOT_TYPE_UEFI] = UefiGroupOrder;
  GroupOrderList[BM_BOOT_TYPE_LEGACY] = LegacyGroupOrder;
  ZeroMem(GroupOrderIndex, sizeof(GroupOrderIndex));

  for (BootType = 0; BootType < BM_BOOT_TYPE_CTX_COUNT; BootType++) {
    if (!mBmBootTypeCtx[BootType].Enable) {
      continue;
    }
    for (Position = 0; Position <= gMaxPosition[BootType]; Position++) {
      MenuCtx = FindBmMenuCtxEntryByPosition(Position, BootType);
      if (MenuCtx == NULL) {
        continue;
      }
//    DEBUG((EFI_D_INFO, "MenuType:%X OrderVarOffset:%X\n", MenuCtx->MenuType, MenuCtx->OrderVarOffset));
      GroupOrderList[BootType][GroupOrderIndex[BootType]++] = MenuCtx->MenuType;
      //
      // Get and update disabled status of GroupType
      //
      if (BootType == BM_BOOT_TYPE_UEFI) {
        GroupDisabled = *((BOOLEAN *)((UINTN)&BmmCallbackInfo->BmmFakeNvData + STATUS_UEFI_BOOT_GROUP_VAR_OFFSET) + MenuCtx->Position);
      } else {
        GroupDisabled = *((BOOLEAN *)((UINTN)&BmmCallbackInfo->BmmFakeNvData + STATUS_LEGACY_BOOT_GROUP_VAR_OFFSET) + MenuCtx->Position);
      }
      if (GroupDisabled) {
        *((UINT8 *)((UINTN)&mDisabledGroupType + MenuCtx->GroupVarOffset)) = 1;
      } else {
        *((UINT8 *)((UINTN)&mDisabledGroupType + MenuCtx->GroupVarOffset)) = 0;
      }

      //
      // Process items of current GroupType
      //
      if (NewBootOrder == NULL) {
        continue;
      }
      p = (UINT16*)((UINTN)&BmmCallbackInfo->BmmFakeNvData + MenuCtx->OrderVarOffset);
      AddedItemCount = 0;
      for (Index = 0; Index < gNvBootOptionCount; Index++) {
        if (GetGroupTypeOfAllTypeOptions(&gNvBootOptions[Index]) != MenuCtx->MenuType) {
          continue;
        }

        for (i = 0; i < MenuCtx->DevCount; i++) {
          Item = &MenuCtx->Item[i];
          ASSERT(Item != NULL);
          if ((Item->Value - 1) == gNvBootOptions[Index].OptionNumber) {
            break;
          }
        }
        if (i == MenuCtx->DevCount) { // not found, is hidden and add it to NewBootOrder directly
          NewBootOrder[NewBootOrderIndex++] = (UINT16)gNvBootOptions[Index].OptionNumber;
          continue;
        } else {
          //
          // Check if current option is in orderlist of BMM_FAKE_NV_DATA
          //
          for (i = 0; i < MenuCtx->DevCount; i++) {
            Item = FindBootDevItemEntryByValue(MenuCtx->Item, p[i]);
            ASSERT(Item != NULL);
            if ((Item->Value - 1) == gNvBootOptions[Index].OptionNumber) {
              break;
            }
          }
          if (i == MenuCtx->DevCount) { // not found
            NewBootOrder[NewBootOrderIndex++] = (UINT16)gNvBootOptions[Index].OptionNumber;

            //
            // Update options
            //
            Item = FindBootDevItemEntryByValue(MenuCtx->Item, (UINT16)(gNvBootOptions[Index].OptionNumber + 1));
            ASSERT(Item != NULL);
            UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x",gNvBootOptions[Index].OptionNumber);
            Status = EfiBootManagerVariableToLoadOption (OptionName, &Option);
            if (!EFI_ERROR(Status)) {
              //
              // When the current GroupType is not disabled, whether the boot option is disabled depends on the 
              // disabled status of the boot option
              //
              ItemDisabled = !Item->Enable;
              OptionActive = (Option.Attributes & LOAD_OPTION_ACTIVE) != 0? TRUE: FALSE;
              if (OptionActive != !ItemDisabled) {
                //
                // Update the Attributes of current option
                //
                if (ItemDisabled) {
                  Option.Attributes &= (~LOAD_OPTION_ACTIVE);
                } else {
                  Option.Attributes |= LOAD_OPTION_ACTIVE;
                }
                EfiBootManagerLoadOptionToVariable(&Option);
                EfiBootManagerFreeLoadOption(&Option);
              }
            }
            continue;
          }
        }

        //
        // Current option is in the orderlist of BMM_FAKE_NV_DATA.
        // Save the ordered options in orderlist(p) to NewBootOrder, and these options are put on the same positions
        // in NewBootOrder as the options of gNvBootOptions which are in the orderlist of BMM_FAKE_NV_DATA.
        //
        Item = FindBootDevItemEntryByValue(MenuCtx->Item, p[AddedItemCount]);
        ASSERT(Item != NULL);
        DEBUG((EFI_D_INFO, "[%d] %d %s\n", AddedItemCount, p[AddedItemCount], Item->DisplayString));
        for (OrderIndex = 0; OrderIndex < NewBootOrderIndex; OrderIndex++) {
          if ((p[AddedItemCount] - 1) == NewBootOrder[OrderIndex]) {
            break;
          }
        }
        if (OrderIndex < NewBootOrderIndex) { // existed in NewBootOrder
          AddedItemCount++;
          continue;
        }
        NewBootOrder[NewBootOrderIndex++] = p[AddedItemCount] - 1;
        //
        // Update options
        //
        Item = FindBootDevItemEntryByValue(MenuCtx->Item, p[AddedItemCount]);
        ASSERT(Item != NULL);
        UnicodeSPrint (OptionName, sizeof (OptionName), L"Boot%04x",p[AddedItemCount] - 1);
        Status = EfiBootManagerVariableToLoadOption (OptionName, &Option);
        if (!EFI_ERROR(Status)) {
          //
          // When the current GroupType is not disabled, whether the boot option is disabled depends on the 
          // disabled status of the boot option
          //
          ItemDisabled = !Item->Enable;
          OptionActive = (Option.Attributes & LOAD_OPTION_ACTIVE) != 0? TRUE: FALSE;
          if (OptionActive != !ItemDisabled) {
            //
            // Update the Attributes of current option
            //
            if (ItemDisabled) {
              Option.Attributes &= (~LOAD_OPTION_ACTIVE);
            } else {
              Option.Attributes |= LOAD_OPTION_ACTIVE;
            }
            EfiBootManagerLoadOptionToVariable(&Option);
            EfiBootManagerFreeLoadOption(&Option);
          }
        }
        AddedItemCount++;
      }
    }
  }

  DEBUG((EFI_D_INFO, "Order Count:%d-%d\n", NewBootOrderIndex, gNvBootOptionCount));

  if (NewBootOrderIndex != gNvBootOptionCount) {
    for (Index = 0; Index < gNvBootOptionCount; Index++) {
      if (((gNvBootOptions[Index].Attributes & LOAD_OPTION_HIDDEN) != 0) ||
          ((gNvBootOptions[Index].Attributes & LOAD_OPTION_CATEGORY) != LOAD_OPTION_CATEGORY_BOOT)) {
        ASSERT(NewBootOrder != NULL);
        NewBootOrder[NewBootOrderIndex++] = (UINT16)gNvBootOptions[Index].OptionNumber;
      }
    }
  }

  ASSERT(NewBootOrderIndex == gNvBootOptionCount);

  DumpMem8(LegacyGroupOrder, sizeof(LegacyGroupOrder));
  DumpMem8(UefiGroupOrder, sizeof(UefiGroupOrder));
  DumpMem8(NewBootOrder, gNvBootOptionCount * sizeof(UINT16));

  Status = gRT->GetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BootOrderSize,
                  OldBootOrder
                  );
  DEBUG((EFI_D_INFO, "Get BootOrder:%r\n", Status));

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NV_VAR_FLAG,
                  BootOrderSize,
                  NewBootOrder
                  );
  DEBUG((EFI_D_INFO, "Set BootOrder:%r\n", Status));

  if (CompareMem(NewBootOrder, OldBootOrder, BootOrderSize)) {
    VarSize = sizeof(UINT8);
    Status = gRT->GetVariable (
                    L"IpmiBootMiscConfig",
                    &gByoIpmiBootMiscCfgVariableGuid,
                    NULL,
                    &VarSize,
                    &IpmiBootMiscCfg
                    );
    if(Status == EFI_SUCCESS) {
      IpmiBootMiscCfg &= ~BIT4;
      Status = gRT->SetVariable (
                      L"IpmiBootMiscConfig",
                      &gByoIpmiBootMiscCfgVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      VarSize,
                      &IpmiBootMiscCfg
                      );
      DEBUG((EFI_D_INFO, "IpmiBootMiscCfg after:%x\n", IpmiBootMiscCfg));
    }
  }

  Status = gRT->SetVariable (
                  BYO_UEFI_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NV_VAR_FLAG,
                  GroupOrderIndex[BM_BOOT_TYPE_UEFI],
                  UefiGroupOrder
                  );
  DEBUG((EFI_D_INFO, "Set UefiGroupBootOrder:%r\n", Status));

  Status = gRT->SetVariable (
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NV_VAR_FLAG,
                  GroupOrderIndex[BM_BOOT_TYPE_LEGACY],
                  LegacyGroupOrder
                  );
  DEBUG((EFI_D_INFO, "Set LegacyGroupBootOrder:%r\n", Status));

  if(NewBootOrder != NULL){
    FreePool(NewBootOrder);
  }

  mDisabledGroupType.IsByoCommonMaint = TRUE;
  Status = gRT->SetVariable (
                  BYO_DISABLED_GROUP_TYPE_VAR_NAME,
                  &gByoDisableGroupTypeGuid,
                  BYO_DISABLED_GROUP_TYPE_VAR_ATTR,
                  sizeof(DISABLED_GROUP_TYPE),
                  &mDisabledGroupType
                  );
  DEBUG((EFI_D_INFO, "Set BootControl:%r\n", Status));

  return EFI_SUCCESS;
}






EFI_STATUS
EFIAPI
ByoFreeBMPackage (
  IN EFI_HANDLE                            ImageHandle,
  IN EFI_SYSTEM_TABLE                      *SystemTable
  )
{
  if(BmmCallbackInfo->BmmHiiHandle != NULL){
    HiiRemovePackages (BmmCallbackInfo->BmmHiiHandle);
  }

  if (BmmCallbackInfo->BmmSaveNotify.DriverHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           BmmCallbackInfo->BmmSaveNotify.DriverHandle,
           &gSetupSaveNotifyProtocolGuid,
           &BmmCallbackInfo->BmmSaveNotify,
           NULL
           );
  }

  if (BmmCallbackInfo->BmmDriverHandle != NULL) {
    gBS->UninstallMultipleProtocolInterfaces (
           BmmCallbackInfo->BmmDriverHandle,
           &gEfiDevicePathProtocolGuid,
           &mBmmHiiVendorDevicePath,
           &gEfiHiiConfigAccessProtocolGuid,
           &BmmCallbackInfo->BmmConfigAccess,
           NULL
           );
  }

  return EFI_SUCCESS;

}



UINTN FindBmMenuPosition(BM_MENU_TYPE MenuType)
{
  UINTN  Index;

  for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
    if(gBmMenuCtx[Index].MenuType == MenuType){
      return Index;
    }
  }

  ASSERT(FALSE);
  return 0;
}

VOID SaveCurrentBootOrderInfo()
{
  UINTN                          Index;
  UINTN                          i;
  UINTN                          BufferSize;
  UINT16                         *Buffer;
  UINT16                         *p;
  UINTN                          Size;
  EFI_STATUS                     Status;
  EFI_BOOT_MANAGER_LOAD_OPTION   *NvBootOption = NULL;
  UINTN                           NvBootOptionCount = 0;
  BM_MENU_TYPE                    GroupOrder;
  UINTN                           Position;
  USER_BM_MENU_INFO               *Info;
//BYO_SYS_CFG_STS_DATA            *SysCfg;
  UINT8                           LegacyGroupOrder[BM_MENU_CTX_COUNT];
  UINT8                           UefiGroupOrder[BM_MENU_CTX_COUNT];
  UINTN                           LegacyGroupOrderSize = 0;
  UINTN                           UefiGroupOrderSize = 0;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Info = gUserBmMenuInfo;
  ZeroMem(&gUserBmMenuInfo, sizeof(gUserBmMenuInfo));

  NvBootOption = EfiBootManagerGetLoadOptions(&NvBootOptionCount, LoadOptionTypeBoot);

  UefiGroupOrderSize = sizeof(UefiGroupOrder);
  Status = gRT->GetVariable(
                  BYO_UEFI_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NULL,
                  &UefiGroupOrderSize,
                  UefiGroupOrder
                  );
  if(!EFI_ERROR(Status)){
    LegacyGroupOrderSize = sizeof(LegacyGroupOrder);
    Status = gRT->GetVariable(
                    BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    NULL,
                    &LegacyGroupOrderSize,
                    LegacyGroupOrder
                    );
  }
  if(EFI_ERROR(Status)){
    i = 0;
    for(Index=0;Index<UefiGroupOrderSize;Index++){
      Info[Index].Position = (UINT16)i++;
    }
    i = 0;
    for(Index=0;Index<LegacyGroupOrderSize;Index++){
      Info[Index+BM_MENU_CTX_UEFI_COUNT].Position = (UINT16)i++;
    }
  } else {
    ASSERT(UefiGroupOrderSize == BM_MENU_CTX_UEFI_COUNT);
    ASSERT(LegacyGroupOrderSize == BM_MENU_CTX_LEGACY_COUNT);
    i = 0;
    for(Index=0;Index<UefiGroupOrderSize;Index++){
      Position = FindBmMenuPosition(UefiGroupOrder[Index]);
      Info[Position].Position = (UINT16)i++;
    }
    i = 0;
    for(Index=0;Index<LegacyGroupOrderSize;Index++){
      Position = FindBmMenuPosition(LegacyGroupOrder[Index]);
      Info[Position].Position = (UINT16)i++;
    }
  }

  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if (((NvBootOption[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) ||
        ((NvBootOption[Index].Attributes & LOAD_OPTION_HIDDEN) != 0)) {
      continue;
    }

    GroupOrder = GetGroupTypeOfAllTypeOptions(&NvBootOption[Index]);
    Position = FindBmMenuPosition(GroupOrder);
    if(Info[Position].DevCount < MAX_MENU_NUMBER){
      Info[Position].DevName[Info[Position].DevCount] = NvBootOption[Index].Description;
      Info[Position].DevCount++;
    }
  }

  BufferSize = 0;
  for(Index=0;Index<ARRAY_SIZE(gUserBmMenuInfo);Index++){
    BufferSize += sizeof(Info[Index].Position) + sizeof(Info[Index].DevCount);
    for(i=0;i<Info[Index].DevCount;i++){
      BufferSize += StrSize(Info[Index].DevName[i]);
    }
  }

  Buffer = AllocatePool(BufferSize);
  ASSERT(Buffer != NULL);
  p = Buffer;

  for(Index=0;Index<ARRAY_SIZE(gUserBmMenuInfo);Index++){
    *(p++) = Info[Index].Position;
    *(p++) = Info[Index].DevCount;
    for(i=0;i<Info[Index].DevCount;i++){
      Size = StrSize(Info[Index].DevName[i]);
      CopyMem(p, Info[Index].DevName[i], Size);
      p += Size/sizeof(CHAR16);
    }
  }

  DumpMem8(Buffer, BufferSize);

  FreePool(Buffer);
}

VOID
UpdatePages (
  UINT16        StartOpCodeCtx,
  UINT16        EndOpCodeCtx
  )
{
  UINT16                      Index;
  EFI_IFR_GUID_LABEL          *GuidLabel;

  DEBUG((EFI_D_INFO, "%a:OpCodeCtx Form %d To %d line=%d\n", __FUNCTION__, StartOpCodeCtx, EndOpCodeCtx, __LINE__));

  //
  // Allocate and init labels
  //
  for (Index = StartOpCodeCtx; Index <= EndOpCodeCtx; Index++) {
    mBmOpCodeCtx[Index].StartOpCodeHandle = HiiAllocateOpCodeHandle();
    mBmOpCodeCtx[Index].EndOpCodeHandle = HiiAllocateOpCodeHandle();

    GuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (mBmOpCodeCtx[Index].StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    GuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    GuidLabel->Number = mBmOpCodeCtx[Index].StartOpLabelNumber;
    GuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (mBmOpCodeCtx[Index].EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    GuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    GuidLabel->Number = mBmOpCodeCtx[Index].EndOpLabelNumber;
  }

  //
  // Show and update Boot Order page
  //
  for (Index = StartOpCodeCtx; Index <= EndOpCodeCtx; Index++) {
    if ((mBmOpCodeCtx[Index].StartOpLabelNumber == UEFI_BOOT_ITEM_ID)
      || (mBmOpCodeCtx[Index].StartOpLabelNumber == LEGACY_BOOT_ITEM_ID)) {
      if ((mBmOpCodeCtx[Index].StartOpLabelNumber == UEFI_BOOT_ITEM_ID) && (mBootMode == BIOS_BOOT_LEGACY_OS)) {
        continue;
      }
      if ((mBmOpCodeCtx[Index].StartOpLabelNumber == LEGACY_BOOT_ITEM_ID) && (mBootMode == BIOS_BOOT_UEFI_OS)) {
        continue;
      }
      UpdateBootOrderItems(Index);
    } else if ((mBmOpCodeCtx[Index].StartOpLabelNumber == DISABLE_UEFI_BOOT_GROUP_ID)
      || (mBmOpCodeCtx[Index].StartOpLabelNumber == DISABLE_LEGACY_BOOT_GROUP_ID)
      || (mBmOpCodeCtx[Index].StartOpLabelNumber == DISABLE_BOOT_OPTIONS_ID)) {
      if ((mBmOpCodeCtx[Index].StartOpLabelNumber == DISABLE_UEFI_BOOT_GROUP_ID) && (mBootMode == BIOS_BOOT_LEGACY_OS)) {
        continue;
      }
      if ((mBmOpCodeCtx[Index].StartOpLabelNumber == DISABLE_LEGACY_BOOT_GROUP_ID) && (mBootMode == BIOS_BOOT_UEFI_OS)) {
        continue;
      }
      UpdateDisablePages(Index);
    }
  }

  //
  // Free labels
  //
  for (Index = StartOpCodeCtx; Index <= EndOpCodeCtx; Index++) {
    if (mBmOpCodeCtx[Index].StartOpCodeHandle != NULL) {
      HiiFreeOpCodeHandle(mBmOpCodeCtx[Index].StartOpCodeHandle);
      mBmOpCodeCtx[Index].StartOpCodeHandle = NULL;
    }

    if (mBmOpCodeCtx[Index].EndOpCodeHandle != NULL) {
      HiiFreeOpCodeHandle(mBmOpCodeCtx[Index].EndOpCodeHandle);
      mBmOpCodeCtx[Index].EndOpCodeHandle = NULL;
    }
  }
}

VOID
UpdateBootOrderItems (
  UINT16              CtxIndex
  )
{
  EFI_STATUS                  Status;
  UINT16                      Index;
  UINT16                      MaxPosition = 0;
  UINTN                       Count;
  BM_BOOT_TYPE                BootType = 0;
  BM_MENU_CTX                 *MenuCtx;
  BM_BOOT_DEV_ITEM            *Item;
  VOID                        *OptionsOpCodeHandle;
  BOOLEAN                     ActiveItemsExist;
  UINT8                       ItemCount;
  EFI_STRING_ID               PromptToken = 0;

  if (mBmOpCodeCtx[CtxIndex].StartOpLabelNumber == UEFI_BOOT_ITEM_ID) {
    MaxPosition = gMaxPosition[BM_BOOT_TYPE_UEFI];
    BootType = BM_BOOT_TYPE_UEFI;
    PromptToken = STRING_TOKEN(STR_UEFI_BOOT_ORDER);
  } else if (mBmOpCodeCtx[CtxIndex].StartOpLabelNumber == LEGACY_BOOT_ITEM_ID) {
    MaxPosition = gMaxPosition[BM_BOOT_TYPE_LEGACY];
    BootType = BM_BOOT_TYPE_LEGACY;
    PromptToken = STRING_TOKEN(STR_LEGACY_BOOT_ORDER);
  }
  //
  //When the boot mode is not Legacy, add "Add Boot Option" option
  //
  if (BootType != BM_BOOT_TYPE_LEGACY) {
    HiiCreateGotoOpCode(
      mBmOpCodeCtx[CtxIndex].StartOpCodeHandle,
      FORM_ADD_BOOT_OPTIONS,STRING_TOKEN(STR_ADD_BOOT_OPTION),
      STRING_TOKEN(STR_ADD_BOOT_OPTION_HELP),
      0,
      FORM_ADD_OPTIONS_QUESTION_ID
      );
    HiiCreateGotoOpCode(
      mBmOpCodeCtx[CtxIndex].StartOpCodeHandle,
      FORM_DELETE_BOOT_OPTIONS,
      STRING_TOKEN(STR_DELETE_BOOT_OPTION),
      STRING_TOKEN(STR_DELETE_BOOT_OPTION_HELP),
      0,
      FORM_DELETEL_OPTIONS_ID
      );
    UpdateDeletePage();
    HiiCreateSubTitleOpCode(mBmOpCodeCtx[CtxIndex].StartOpCodeHandle,STRING_TOKEN(STR_NULL_STRING),STRING_TOKEN(STR_NULL_STRING),0,0);
  }
  HiiCreateSubTitleOpCode (mBmOpCodeCtx[CtxIndex].StartOpCodeHandle, PromptToken, 0, 0, 0);
  for (Index = 0; Index <= MaxPosition; Index++) {
    MenuCtx = FindBmMenuCtxEntryByPosition(Index, BootType);
    if ((MenuCtx == NULL) || !MenuCtx->Enable) { // hide the disabled boot group
      continue;
    }

    ActiveItemsExist = FALSE;
    ItemCount = 0;
    OptionsOpCodeHandle = HiiAllocateOpCodeHandle();
    for (Count = 0; Count < MenuCtx->DevCount; Count++) {
      Item = &MenuCtx->Item[Count];

      if (!Item->Enable) { // hide the disabled boot option
        continue;
      }
      ActiveItemsExist = TRUE;
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        Item->DisplayStringToken,
        0,
        EFI_IFR_TYPE_NUM_SIZE_16,
        Item->Value
        );
      ItemCount ++;
    }

    if (ActiveItemsExist) {
      HiiCreateOrderedListOpCode (
        mBmOpCodeCtx[CtxIndex].StartOpCodeHandle,               // Container for dynamic created opcodes
        MenuCtx->QuestionId,                   // Question ID
        VARSTORE_ID_BOOT_MAINT,                      // VarStore ID
        MenuCtx->OrderVarOffset,                    // Offset in Buffer Storage
        MenuCtx->PromptWithTriangle,                       // Question prompt text
        MenuCtx->Help,                         // Question help text
        EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_OPTIONS_ONLY,  // Question flag
        0,                                           // Ordered list flag, e.g. EFI_IFR_UNIQUE_SET
        EFI_IFR_TYPE_NUM_SIZE_16,                    // Data type of Question value
        ItemCount,                             // Maximum container
        OptionsOpCodeHandle,                         // Option Opcode list
        NULL                                         // Default Opcode is NULL
        );
    } else {
      DEBUG((EFI_D_INFO, "ActionOp\n"));
      HiiCreateActionOpCode (
        mBmOpCodeCtx[CtxIndex].StartOpCodeHandle,
        MenuCtx->QuestionId,
        MenuCtx->Prompt,
        MenuCtx->Help,
        EFI_IFR_FLAG_CALLBACK,
        0
        );
    }
    HiiFreeOpCodeHandle(OptionsOpCodeHandle);
  }

  Status = HiiUpdateForm (
             BmmCallbackInfo->BmmHiiHandle,
             &mBootMaintenanceManagerGuid,
             FORM_MAIN_ID,
             mBmOpCodeCtx[CtxIndex].StartOpCodeHandle,
             mBmOpCodeCtx[CtxIndex].EndOpCodeHandle
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));
}

VOID
UpdateDisablePages (
  UINT16              CtxIndex
  )
{
  EFI_STATUS                  Status;
  UINT16                      Index, Count;
  UINT16                      MaxPosition = 0;
  BM_BOOT_TYPE                BootType = 0, TypeIndex;
  BM_MENU_CTX                 *MenuCtx;
  BM_BOOT_DEV_ITEM            *Item;
  EFI_FORM_ID                 FormId = 0;
  UINT16                      VarOffset = 0;
  EFI_STRING_ID               PromptToken = 0;

  if ((mBmOpCodeCtx[CtxIndex].StartOpLabelNumber == DISABLE_UEFI_BOOT_GROUP_ID)
    || (mBmOpCodeCtx[CtxIndex].StartOpLabelNumber == DISABLE_LEGACY_BOOT_GROUP_ID)) {
    FormId = FORM_DISABLE_GROUP_TYPE;
    if (mBmOpCodeCtx[CtxIndex].StartOpLabelNumber == DISABLE_UEFI_BOOT_GROUP_ID) {
      VarOffset = (UINT16)STATUS_UEFI_BOOT_GROUP_VAR_OFFSET;
      MaxPosition = gMaxPosition[BM_BOOT_TYPE_UEFI];
      BootType = BM_BOOT_TYPE_UEFI;
      PromptToken = STRING_TOKEN(STR_DISABLE_UEFI_GROUP_TYPE);
    } else if (mBmOpCodeCtx[CtxIndex].StartOpLabelNumber == DISABLE_LEGACY_BOOT_GROUP_ID) {
      VarOffset = (UINT16)STATUS_LEGACY_BOOT_GROUP_VAR_OFFSET;
      MaxPosition = gMaxPosition[BM_BOOT_TYPE_LEGACY];
      BootType = BM_BOOT_TYPE_LEGACY;
      PromptToken = STRING_TOKEN(STR_DISABLE_LEGACY_GROUP_TYPE);
    }

    HiiCreateSubTitleOpCode (mBmOpCodeCtx[CtxIndex].StartOpCodeHandle, PromptToken, 0, 0, 0);
    for (Index = 0; Index <= MaxPosition; Index++) {
      MenuCtx = FindBmMenuCtxEntryByPosition(Index, BootType);
      if (MenuCtx == NULL) {
        continue;
      }
  
      HiiCreateCheckBoxOpCode (
        mBmOpCodeCtx[CtxIndex].StartOpCodeHandle,
        MenuCtx->StatusGroupQuestionId,
        VARSTORE_ID_BOOT_MAINT,
        (UINT16)(VarOffset + Index),
        MenuCtx->Prompt,
        STRING_TOKEN (STR_DISABLE_GROUP_TYPE_HELP),
        EFI_IFR_FLAG_CALLBACK,
        0,
        NULL
        );
    }
  } else if (mBmOpCodeCtx[CtxIndex].StartOpLabelNumber == DISABLE_BOOT_OPTIONS_ID) {
    mItemsCount = 0;
    FormId = FORM_DISABLE_BOOT_OPTIONS;
    for (TypeIndex = 0; TypeIndex < BM_BOOT_TYPE_CTX_COUNT; TypeIndex++) {
      if (!mBmBootTypeCtx[TypeIndex].Enable) {
        continue;
      }
      if (TypeIndex == BM_BOOT_TYPE_UEFI) {
        if (mUefiOptionsCount == 0) {
          continue;
        }
        PromptToken = STRING_TOKEN(STR_DISABLE_UEFI_OPTIONS);
      } else {
        if (mLegacyOptionsCount == 0) {
          continue;
        }
        PromptToken = STRING_TOKEN(STR_DISABLE_LEGACY_OPTIONS);
      }
      HiiCreateSubTitleOpCode (mBmOpCodeCtx[CtxIndex].StartOpCodeHandle, PromptToken, 0, 0, 0);
      for (Index = 0; Index <= gMaxPosition[TypeIndex]; Index++) {
        MenuCtx = FindBmMenuCtxEntryByPosition(Index, TypeIndex);
        if ((MenuCtx == NULL) || (MenuCtx->DevCount == 0) || !MenuCtx->Enable) {
          continue;
        }

        for (Count = 0; Count < MenuCtx->DevCount; Count++) {
          Item = &MenuCtx->Item[Count];
          HiiCreateCheckBoxOpCode (
            mBmOpCodeCtx[CtxIndex].StartOpCodeHandle,
            mBmOpCodeCtx[CtxIndex].QuestionIdBase + mItemsCount,
            VARSTORE_ID_BOOT_MAINT,
            (UINT16)(MenuCtx->OptionVarOffset + Count),
            Item->DisplayStringToken,
            Item->HelpStringToken,
            0,
            0,
            NULL
            );
            mItemsCount++;
        }
      }
    }
  }

  Status = HiiUpdateForm (
             BmmCallbackInfo->BmmHiiHandle,
             &mBootMaintenanceManagerGuid,
             FormId,
             mBmOpCodeCtx[CtxIndex].StartOpCodeHandle,
             mBmOpCodeCtx[CtxIndex].EndOpCodeHandle
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));
}