/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

By: 
  winddy_zhang

File Name:
  HpDxe.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include "FrontPage.h"
#include "FormGuid.h"
#include <Protocol/SetupItemUpdateNotify.h>
#include <Guid/ByoSetupFormsetGuid.h>
#include "../../PlatformPkg/Include/SetupVariable.h"
#include <Library/ByoUefiBootManagerLib.h>
#include <ByoBootGroup.h>
#include <Library/SetupUiLib.h>
#include <Library/UefiLib.h>
#include <Protocol/Rng.h>
#include <Library/ByoCommLib.h>
#include <Library/PlatformCommLib.h>
#include <SysMiscCfg2.h>
#include <Library/IpmiBaseLib.h>
#include <Ipmi/IpmiNetFnAppOemDefinition.h>
#include <Library/AmdCbsVariable.H>
#include <Protocol/PciIo.h>
#include <Library/AmdCbsVariable.h>

VOID SavePlatformLanguage();
VOID PlatformLanguageLoadDefault();
VOID RestoreBootOrder(BOOLEAN IsUserDefault);

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

extern UINT8           BmBin[];
extern EFI_HII_HANDLE  gHiiHandle;

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

#define UEFI_HDD_VAR_OFFSET             VAR_OFFSET (UefiHDD)
#define UEFI_ODD_VAR_OFFSET             VAR_OFFSET (UefiODD)
#define UEFI_USB_DISK_VAR_OFFSET        VAR_OFFSET (UefiUskDisk)
#define UEFI_USB_ODD_VAR_OFFSET         VAR_OFFSET (UefiUsbOdd)
#define UEFI_PXE_VAR_OFFSET             VAR_OFFSET (UefiPxe)
#define UEFI_OTHERS_VAR_OFFSET          VAR_OFFSET (UefiOthers)


#define NV_VAR_FLAG    (EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE)

typedef struct {
  UINT32                         Signature;
  EFI_HII_HANDLE                 BmmHiiHandle;
  EFI_HANDLE                     BmmDriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL BmmConfigAccess;
  SETUP_SAVE_NOTIFY_PROTOCOL     BmmSaveNotify;
  BMM_FAKE_NV_DATA               BmmFakeNvData;
  SETUP_ITEM_UPDATE_NOTIFY_PROTOCOL  Notify;
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
      FORMSET_GUID_BOOT,
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
  }
};

BMM_CALLBACK_DATA   *BmmCallbackInfo = &gBmmCallbackInfo;


#define BYO_BOOT_MAINT_VAR_NAME       L"BmmData"


EFI_BOOT_MANAGER_LOAD_OPTION   *gNvBootOptions = NULL;
UINTN                          gNvBootOptionCount = 0;
SETUP_DATA                     gSetupDataTemp;


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
    if(EFI_ERROR(Status)){
      continue;
    }

    Status = ItemNotify->Notify(ItemNotify, Item, NewStatus);
  }

  if(Handles != NULL){gBS->FreePool(Handles);}
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

  PlatformLanguageLoadDefault();
  RestoreBootOrder(FALSE);
  NotifySetupItemChanged(ITEM_CSM, CSM_DEFAULT_VALUE?STATUS_ENABLE:STATUS_DISABLE);
  
  return EFI_SUCCESS;
}


EFI_STATUS
BootConfigureLoadUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  EFI_STATUS   Status;
  UINTN        VariableSize;

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  VariableSize = sizeof(SETUP_DATA);
  Status = gRT->GetVariable (
                  L"SetupUserDefault",
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &gSetupDataTemp
                  );
  if(!EFI_ERROR(Status)){
    NotifySetupItemChanged(ITEM_CSM, gSetupDataTemp.BootModeType == BIOS_BOOT_UEFI_OS ? STATUS_DISABLE : STATUS_ENABLE);
  }
  
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
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;

  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &gEfiFormsetGuidBoot, BYO_BOOT_MAINT_VAR_NAME)) {
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
    ConfigRequestHdr = HiiConstructConfigHdr (&gEfiFormsetGuidBoot, BYO_BOOT_MAINT_VAR_NAME, Private->BmmDriverHandle);
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

EFI_STATUS
EFIAPI
BootMaintRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }
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
  EFI_STRING    RequestString = NULL;

  if(Item == ITEM_SECUREBOOT && NewStatus == STATUS_ENABLE){
    DEBUG((EFI_D_INFO, "secure enable, try to disable csm\n"));
     HiiGetBrowserData(
      &gPlatformSetupVariableGuid, 
      PLATFORM_SETUP_VARIABLE_NAME, 
      sizeof(SETUP_DATA), 
      (UINT8*)&gSetupDataTemp
      );
    gSetupDataTemp.BootModeType = BIOS_BOOT_UEFI_OS;
    RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, BootModeType), sizeof(gSetupDataTemp.BootModeType));
    HiiSetBrowserData(
      &gPlatformSetupVariableGuid,
      PLATFORM_SETUP_VARIABLE_NAME,
      sizeof(SETUP_DATA), 
      (UINT8*)&gSetupDataTemp, 
      RequestString
      );
    FreePool (RequestString);
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
									  &gEfiFormsetGuidBoot,
									  DeviceHandle,
									  BmBin,
									  UiAppStrings,
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
} BM_BOOT_DEV_ITEM;


typedef struct {
  UINT16             Position;
  UINT16             DevCount;
  EFI_STRING_ID      Prompt;
  EFI_STRING_ID      Help;
  UINT16             VarOffset;
  UINT16             QuestionId;
  BM_BOOT_TYPE       BootType;
  BM_MENU_TYPE       MenuType;
  BM_BOOT_DEV_ITEM   Item[MAX_MENU_NUMBER];
} BM_MENU_CTX;


// KEEP UEFI FIRST!
#if BC_TENCENT
BM_MENU_CTX gBmMenuCtx[] = {
  {0, 0, STRING_TOKEN(STR_UEFI_HDD),        STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_HDD_VAR_OFFSET,        0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_HDD,          {{0,},}},
//{0, 0, STRING_TOKEN(STR_UEFI_ODD),        STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_ODD_VAR_OFFSET,        0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_ODD,          {{0,},}},
  {0, 0, STRING_TOKEN(STR_UEFI_PXE),        STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_PXE_VAR_OFFSET,        0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_PXE,          {{0,},}},
  {0, 0, STRING_TOKEN(STR_UEFI_USB_ODD),    STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_USB_ODD_VAR_OFFSET,    0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_USB_ODD,      {{0,},}},
  {0, 0, STRING_TOKEN(STR_UEFI_USB_DISK),   STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_USB_DISK_VAR_OFFSET,   0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_USK_DISK,     {{0,},}},
  {0, 0, STRING_TOKEN(STR_UEFI_OTHERS),     STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_OTHERS_VAR_OFFSET,     0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_OTHERS,       {{0,},}},
  
  {0, 0, STRING_TOKEN(STR_LEGACY_HDD),      STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_HDD_VAR_OFFSET,      0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_HDD,      {{0,},}},
//{0, 0, STRING_TOKEN(STR_LEGACY_ODD),      STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_ODD_VAR_OFFSET,      0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_ODD,      {{0,},}},
  {0, 0, STRING_TOKEN(STR_LEGACY_PXE),      STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_PXE_VAR_OFFSET,      0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_PXE,      {{0,},}},
  {0, 0, STRING_TOKEN(STR_LEGACY_USB_ODD),  STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_USB_ODD_VAR_OFFSET,  0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_USB_ODD,  {{0,},}},
  {0, 0, STRING_TOKEN(STR_LEGACY_USB_DISK), STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_USB_DISK_VAR_OFFSET, 0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_USK_DISK, {{0,},}},
};
#else
BM_MENU_CTX gBmMenuCtx[] = {
  {0, 0, STRING_TOKEN(STR_UEFI_HDD),        STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_HDD_VAR_OFFSET,        0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_HDD,          {{0,},}},
  //{0, 0, STRING_TOKEN(STR_UEFI_ODD),        STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_ODD_VAR_OFFSET,        0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_ODD,          {{0,},}},
  {0, 0, STRING_TOKEN(STR_UEFI_USB_DISK),   STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_USB_DISK_VAR_OFFSET,   0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_USK_DISK,     {{0,},}},
   {0, 0, STRING_TOKEN(STR_UEFI_PXE),        STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_PXE_VAR_OFFSET,        0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_PXE,          {{0,},}},
  {0, 0, STRING_TOKEN(STR_UEFI_USB_ODD),    STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_USB_ODD_VAR_OFFSET,    0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_USB_ODD,      {{0,},}},
  {0, 0, STRING_TOKEN(STR_UEFI_OTHERS),     STRING_TOKEN(STR_BOOT_ORDER_HELP), UEFI_OTHERS_VAR_OFFSET,     0, BM_BOOT_TYPE_UEFI, BM_MENU_TYPE_UEFI_OTHERS,       {{0,},}},
  
  {0, 0, STRING_TOKEN(STR_LEGACY_HDD),      STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_HDD_VAR_OFFSET,      0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_HDD,      {{0,},}},
  //{0, 0, STRING_TOKEN(STR_LEGACY_ODD),      STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_ODD_VAR_OFFSET,      0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_ODD,      {{0,},}},
  {0, 0, STRING_TOKEN(STR_LEGACY_USB_DISK), STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_USB_DISK_VAR_OFFSET, 0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_USK_DISK, {{0,},}},
  {0, 0, STRING_TOKEN(STR_LEGACY_PXE),      STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_PXE_VAR_OFFSET,      0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_PXE,      {{0,},}},
  {0, 0, STRING_TOKEN(STR_LEGACY_USB_ODD),  STRING_TOKEN(STR_BOOT_ORDER_HELP), LEGACY_USB_ODD_VAR_OFFSET,  0, BM_BOOT_TYPE_LEGACY, BM_MENU_TYPE_LEGACY_USB_ODD,  {{0,},}},
};
#endif


#define BM_MENU_CTX_COUNT         (sizeof(gBmMenuCtx)/sizeof(gBmMenuCtx[0]))
#define BM_MENU_CTX_UEFI_COUNT    5
#define BM_MENU_CTX_LEGACY_COUNT  4


typedef struct {
  UINT16             Position;
  UINT16             DevCount;
  UINT16             *DevName[MAX_MENU_NUMBER];
} USER_BM_MENU_INFO;

USER_BM_MENU_INFO gUserBmMenuInfo[BM_MENU_CTX_COUNT];

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

  for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
    Info[Index].Position = *p++;
    Info[Index].DevCount = *p++;
    DEBUG((EFI_D_INFO, "P:%d L:%d\n", Info[Index].Position, Info[Index].DevCount));
    for(i=0;i<Info[Index].DevCount;i++){
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
  EFI_STATUS    Status;
  UINT16        *ItemOrder;
  

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
    MenuCtx = &gBmMenuCtx[Index];
    BufferSize += sizeof(MenuCtx->Position) + sizeof(MenuCtx->DevCount);
    for(i=0;i<MenuCtx->DevCount;i++){
      BufferSize += StrSize(MenuCtx->Item[i].DisplayString);
    }
  }

  Buffer = AllocatePool(BufferSize);
  ASSERT(Buffer != NULL);
  p = Buffer;
  
  for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
    MenuCtx = &gBmMenuCtx[Index];
    *(p++) = MenuCtx->Position;
    *(p++) = MenuCtx->DevCount;

    ItemOrder = (UINT16*)(((UINTN)&BmmCallbackInfo->BmmFakeNvData) + MenuCtx->VarOffset);
    for(i=0;i<MenuCtx->DevCount;i++){
      for(j=0;j<MenuCtx->DevCount;j++){
        if(MenuCtx->Item[j].Value == ItemOrder[i]){
          Size = StrSize(MenuCtx->Item[j].DisplayString);
          CopyMem(p, MenuCtx->Item[j].DisplayString, Size);
          p += Size/sizeof(CHAR16);
        }
      }
    }    
  }  

  Status = gRT->SetVariable (
                  USER_BOOT_ORDER_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NV_VAR_FLAG,
                  BufferSize,
                  Buffer
                  );

  FreePool(Buffer);
 
  return EFI_SUCCESS;
}








VOID   *gStartOpCodeHandle[BM_BOOT_TYPE_MAX];
VOID   *gEndOpCodeHandle[BM_BOOT_TYPE_MAX];
UINT16 gMaxPosition[BM_BOOT_TYPE_MAX];
UINT16 gOpLabelNumber[BM_BOOT_TYPE_MAX];

BM_MENU_CTX *FindBmMenuCtxEntry(BM_MENU_TYPE MenuType)
{
  UINTN  Index;

  for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
    if(gBmMenuCtx[Index].MenuType == MenuType){
      return &gBmMenuCtx[Index];
    }
  }

  return NULL;
}


BM_MENU_CTX *FindBmMenuCtxEntryByQuestionId(UINT16 QuestionId)
{
  UINTN  Index;

  for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
    if(gBmMenuCtx[Index].QuestionId == QuestionId){
      return &gBmMenuCtx[Index];
    }
  }

  return NULL;
}


BM_MENU_CTX *FindBmMenuCtxEntryByPosition(UINT16 Position, BM_BOOT_TYPE BootType)
{
  UINTN  Index;

  for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
    if(gBmMenuCtx[Index].Position == Position && gBmMenuCtx[Index].BootType == BootType){
      return &gBmMenuCtx[Index];
    }
  }

  return NULL;
}

BM_BOOT_DEV_ITEM *FindBootDevItemEntryByValue(BM_BOOT_DEV_ITEM *List, UINT16 Value)
{
  UINTN  Index;

  for(Index=0;Index<MAX_MENU_NUMBER;Index++){
    if(List[Index].Value == 0){
      DEBUG((EFI_D_ERROR, "%a (L%d) Vin:%d\n", __FUNCTION__, __LINE__, Value));
      return NULL;
    }
    if(List[Index].Value == Value){
      return List + Index;
    }
  }

  DEBUG((EFI_D_ERROR, "%a (L%d) Vin:%d\n", __FUNCTION__, __LINE__, Value));
  return NULL;
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
  EFI_BDS_BOOT_MANAGER_PROTOCOL *BdsBootMgr;
  EFI_STATUS                    Status;
  BOOLEAN                       Updated;
  

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  if(gNvBootOptionCount == 0){
    return EFI_NOT_FOUND;
  }

  VarBase = (UINTN)&BmmCallbackInfo->BmmFakeNvData;
  Option = gNvBootOptions;

  Status = gBS->LocateProtocol (&gEfiBootManagerProtocolGuid, NULL, (VOID**)&BdsBootMgr);
  if(EFI_ERROR(Status)){
    return Status;
  }

  for (Index = 0; Index < gNvBootOptionCount; Index++) {
    
    if (Option[Index].Ignore) {
      continue;
    }

    GroupOrder = (BM_MENU_TYPE)Option[Index].GroupType;
    MenuCtx = FindBmMenuCtxEntry(GroupOrder);
    DEBUG((EFI_D_INFO, "XXXX:%X GroupOrder:%X\n", Option[Index].OptionNumber, GroupOrder));
    if(MenuCtx == NULL){
      continue;
    }

    if(MenuCtx->DevCount < MAX_MENU_NUMBER){
      Item = &MenuCtx->Item[MenuCtx->DevCount];
      Item->LoadOption = &Option[Index];
//-   Item->DisplayString = Item->LoadOption->Description;
      Item->DisplayString = BdsBootMgr->GetExtBmBootName (
                                          Item->LoadOption->Description, 
                                          Item->LoadOption->FilePath,
                                          Item->LoadOption->OptionalData,
                                          Item->LoadOption->OptionalDataSize,
                                          &Updated
                                          );

      Item->HelpString    = NULL; 
      Item->DisplayStringToken = HiiSetString(BmmCallbackInfo->BmmHiiHandle, 0, Item->DisplayString, NULL);      
      Item->HelpStringToken = 0;
      p  = (UINT16*)(VarBase + MenuCtx->VarOffset) + MenuCtx->DevCount;
      *p = (UINT16)Item->LoadOption->OptionNumber + 1;      // 0 is end flag
//-   DEBUG((EFI_D_INFO, "V:%d C:%d O:%X\n", *p, MenuCtx->DevCount, MenuCtx->VarOffset));
      Item->Value = *p;
      MenuCtx->DevCount++;
    }
    
  }

  return EFI_SUCCESS;
}


EFI_STATUS
InitBootGroup (
  VOID
  )
{
  EFI_STATUS   Status;
  UINTN        Index;
  BM_MENU_CTX  *MenuCtx;
  BM_BOOT_TYPE TypeIndex;
  UINT8        LegacyGroupOrder[BM_MENU_CTX_COUNT];
  UINT8        UefiGroupOrder[BM_MENU_CTX_COUNT];
  UINTN        LegacyGroupOrderSize = 0;
  UINTN        UefiGroupOrderSize = 0;
 

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
  ZeroMem(gMaxPosition, sizeof(gMaxPosition));

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
    for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
      gBmMenuCtx[Index].Position = gMaxPosition[gBmMenuCtx[Index].BootType]++;
    }
  } else {
    ASSERT(UefiGroupOrderSize == BM_MENU_CTX_UEFI_COUNT);
    ASSERT(LegacyGroupOrderSize == BM_MENU_CTX_LEGACY_COUNT);
    for(Index=0;Index<UefiGroupOrderSize;Index++){
      MenuCtx = FindBmMenuCtxEntry(UefiGroupOrder[Index]);
      ASSERT(MenuCtx != NULL);
      MenuCtx->Position = gMaxPosition[MenuCtx->BootType]++;
    }
    for(Index=0;Index<LegacyGroupOrderSize;Index++){
      MenuCtx = FindBmMenuCtxEntry(LegacyGroupOrder[Index]);
      ASSERT(MenuCtx != NULL);      
      MenuCtx->Position = gMaxPosition[MenuCtx->BootType]++;
    }    
  }

  for(TypeIndex=BM_BOOT_TYPE_MIN;TypeIndex<BM_BOOT_TYPE_MAX;TypeIndex++){
    if(gMaxPosition[TypeIndex]){
      gMaxPosition[TypeIndex]--;
    }  
  }

  for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
    DEBUG((EFI_D_INFO, "P:%d\n", gBmMenuCtx[Index].Position));
  }  
  DEBUG((EFI_D_INFO, "Max:%d,%d\n", gMaxPosition[0], gMaxPosition[1]));

  return Status;
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
    StrCpy(NewStr+1, Str+1);
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
    StrCpy(NewStr+1, Str+1);
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

  for(Index=0; Index<BM_MENU_CTX_COUNT; Index++){
    if(MenuCtx[Index].DevCount != 0){
      UpdateStringPreFix(MenuCtx[Index].Prompt, GEOMETRICSHAPE_RIGHT_TRIANGLE);
    }else{
      UpdateStringPreFix(MenuCtx[Index].Prompt, L' ');
    }
  }
}


#define PXE_BOOT_ONEOF_ITEM_ID  0x2732
static UINT32    gPxeVidDid[32]={0};
static UINT32    gPxeMacCrc[32]={0};

CHAR16 *
MyDevicePathToStr (
  IN EFI_BOOT_SERVICES		   *BS,
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  );

typedef struct {
  UINT32                    DidVid;
  CHAR16                    *Str;
  EFI_PCI_IO_PROTOCOL       *PciIo[8];
  UINTN                     Index[8];
  UINTN                     Count;
} NetWorkTypeStruct;


NetWorkTypeStruct mNetWorkTypeList[] = {
  { 0x15028086, L"Intel 82579LM Eth",                {NULL,}, {0,}, 0 },
  { 0x15218086, L"Intel I350 Eth",                   {NULL,}, {0,}, 0 },
  { 0x15338086, L"Intel I210 Eth",                   {NULL,}, {0,}, 0 },
  { 0x10D38086, L"Intel 82574L Eth",                 {NULL,}, {0,}, 0 },
  { 0x15638086, L"Intel X550 Eth",                   {NULL,}, {0,}, 0 },
  { 0x816810EC, L"Realtek 8111G Eth",                {NULL,}, {0,}, 0 },
  { 0x16D814E4, L"Broadcom BCM57416 Eth",            {NULL,}, {0,}, 0 },  
  { 0x168E14E4, L"Broadcom BCM57810 Eth",            {NULL,}, {0,}, 0 },
  { 0x16C914E4, L"Broadcom BCM57302 Eth",            {NULL,}, {0,}, 0 },
  { 0x101515B3, L"Mellanox ConnectX-4 LX Eth",       {NULL,}, {0,}, 0 }, 
  { 0x101715B3, L"Mellanox ConnectX-5 LX Eth",       {NULL,}, {0,}, 0 },
  { 0x101915B3, L"Mellanox ConnectX-5 100G Eth",     {NULL,}, {0,}, 0 },
  { 0x01058088, L"WangXun(R)",       				 {NULL,}, {0,}, 0 },
  { 0x01018088, L"WangXun(R)",       				 {NULL,}, {0,}, 0 },
  { 0x01098088, L"WangXun(R)",       				 {NULL,}, {0,}, 0 },
  { 0x20018088, L"WangXun(R)",       				 {NULL,}, {0,}, 0 },
  { 0x10018088, L"WangXun(R) OCP",       		     {NULL,}, {0,}, 0 },
  { 0x15728086, L"Intel(R) X710 10GbE",       	     {NULL,}, {0,}, 0 },
  { 0x15848086, L"Intel(R) XL710 Q1 40GbE",       	 {NULL,}, {0,}, 0 },
  { 0x15838086, L"Intel(R) XL710 Q2 40GbE",       	 {NULL,}, {0,}, 0 },
  { 0x10FB8086, L"Intel(R) 82599 10GbE",       	     {NULL,}, {0,}, 0 },
//{ 0x91801D17, L"ZX-200 GNIC",   {NULL,}, {0,}, 0 },
};



VOID 
InitString (
  EFI_HII_HANDLE    HiiHandle, 
  EFI_STRING_ID     StrRef, 
  CHAR16            *sFormat, ...
  )
{
  STATIC CHAR16 s[1024];
  VA_LIST  Marker;

  VA_START (Marker, sFormat);
  UnicodeVSPrint (s, sizeof (s),  sFormat, Marker);
  VA_END (Marker);
    
  HiiSetString (HiiHandle, StrRef, s, NULL);
}


EFI_STATUS
PxeBootInit (
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBm
  )
{
  VOID                  *StartOpCodeHandle = NULL;
  VOID                  *EndOpCodeHandle   = NULL;
  EFI_IFR_GUID_LABEL    *StartLabel;
  EFI_IFR_GUID_LABEL    *EndLabel;
  UINTN                 Index,Index1,Index2=0;
  VOID                  *OptionsOpCodeHandle = NULL;
  UINT16                StringToken[32];
  CHAR16                TmpString [256];
  EFI_STATUS            Status;
  UINTN                 HandleCount;
  EFI_HANDLE            *HandleBuffer;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINT32                    VidDid;
  UINTN                Seg, Bus, Dev, Fun;
  UINT8    MacAddr[6];
  
  StartOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_PXE_START;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = LABEL_PXE_END;

  OptionsOpCodeHandle = HiiAllocateOpCodeHandle();
  HiiCreateOneOfOptionOpCode (
    OptionsOpCodeHandle,
    STRING_TOKEN(STR_NOPXE),
    0,
    EFI_IFR_TYPE_NUM_SIZE_16,
    0
    );

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  for(Index=0;Index<HandleCount;Index++){
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    if(EFI_ERROR(Status)) {
      continue;
    }
	
    Bus = 0;
    Dev = 0;
    Fun = 0;
    PciIo->GetLocation (PciIo, &Seg, &Bus, &Dev, &Fun);
    VidDid = 0;
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &VidDid);
    for(Index1=0;Index1<ARRAY_SIZE(mNetWorkTypeList);Index1++){
      if(Fun!=0){
        continue;
      }
      if(VidDid == mNetWorkTypeList[Index1].DidVid){
        ZeroMem(TmpString, sizeof(TmpString));
        ZeroMem(MacAddr, sizeof(MacAddr));
        Status = GetPciNicMac (PciIo, MacAddr);
        if (!EFI_ERROR(Status)) {
          UnicodeSPrint(TmpString, sizeof(TmpString), L"%s (%02X-%02X-%02X-%02X-%02X-%02X)",mNetWorkTypeList[Index1].Str, MacAddr[0],MacAddr[1],MacAddr[2],MacAddr[3],MacAddr[4],MacAddr[5]);
        } else {
          UnicodeSPrint(TmpString, sizeof(TmpString), L"%s",mNetWorkTypeList[Index1].Str);
        }
		
        gPxeMacCrc[Index2] = CalculateCrc32(MacAddr, 6);	
        DEBUG((EFI_D_ERROR, __FUNCTION__"(), gPxeMacCrc[%d] :0x%x.\n", Index2, gPxeMacCrc[Index2]));
        DEBUG((EFI_D_ERROR, __FUNCTION__"(), TmpString :%s, Index2 :%d.\n", TmpString, Index2+1));
        StringToken[Index2] = HiiSetString(BmmCallbackInfo->BmmHiiHandle, 0, TmpString, NULL); 
        HiiCreateOneOfOptionOpCode (
            OptionsOpCodeHandle,
            StringToken[Index2],
            0,
            EFI_IFR_TYPE_NUM_SIZE_16,
            Index2+1
            );
        gPxeVidDid[Index2]=VidDid;	
        Index2++;
      }
    }   
  }

  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    PXE_BOOT_ONEOF_ITEM_ID,
    SETUP_DATA_ID,
    OFFSET_OF(SETUP_DATA, SelectPxeNic),
    STRING_TOKEN(STR_PXE_FORM),
    STRING_TOKEN(STR_PXE_FORM_HELP),
    EFI_IFR_FLAG_CALLBACK,
    EFI_IFR_NUMERIC_SIZE_1,                        // <--- !!!
    OptionsOpCodeHandle,
    NULL
    );

  HiiUpdateForm (
    BmmCallbackInfo->BmmHiiHandle,
    &gEfiFormsetGuidBoot,       // Formset GUID
    ROOT_FORM_ID,               // Form ID
    StartOpCodeHandle,              // Label for where to insert opcodes
    EndOpCodeHandle                 // Replace data
    );

  if(StartOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(StartOpCodeHandle);
  }
  if(EndOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(EndOpCodeHandle);
  }
  if(OptionsOpCodeHandle != NULL){
    HiiFreeOpCodeHandle(OptionsOpCodeHandle);
  }  
 
  return EFI_SUCCESS;
}


EFI_STATUS
BmInit (
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBm
  )
{
  EFI_STATUS                  Status = EFI_SUCCESS;
  UINT16                      Index;
  UINTN                       i;
  VOID                        *OptionsOpCodeHandle;
  BM_BOOT_TYPE                TypeIndex;
  EFI_IFR_GUID_LABEL          *GuidLabel;
  BM_BOOT_DEV_ITEM            *Item;
  BM_MENU_CTX                 *MenuCtx;
  
  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  ASSERT(BM_MENU_CTX_COUNT == BM_MENU_CTX_UEFI_COUNT + BM_MENU_CTX_LEGACY_COUNT);
  
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

  AddBootHiiPackages(BmmCallbackInfo->BmmDriverHandle);
  ASSERT (BmmCallbackInfo->BmmHiiHandle != NULL);

  gStartOpCodeHandle[BM_BOOT_TYPE_UEFI]   = HiiAllocateOpCodeHandle();
  gStartOpCodeHandle[BM_BOOT_TYPE_LEGACY] = HiiAllocateOpCodeHandle();
  gEndOpCodeHandle[BM_BOOT_TYPE_UEFI]     = HiiAllocateOpCodeHandle();
  gEndOpCodeHandle[BM_BOOT_TYPE_LEGACY]   = HiiAllocateOpCodeHandle();
  gOpLabelNumber[BM_BOOT_TYPE_UEFI]       = UEFI_BOOT_ITEM_ID;
  gOpLabelNumber[BM_BOOT_TYPE_LEGACY]     = LEGACY_BOOT_ITEM_ID;  

  GuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (gStartOpCodeHandle[BM_BOOT_TYPE_UEFI], &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  GuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  GuidLabel->Number = gOpLabelNumber[BM_BOOT_TYPE_UEFI];
  GuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (gEndOpCodeHandle[BM_BOOT_TYPE_UEFI], &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  GuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  GuidLabel->Number = UEFI_BOOT_ITEM_END_ID;

  GuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (gStartOpCodeHandle[BM_BOOT_TYPE_LEGACY], &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  GuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  GuidLabel->Number = gOpLabelNumber[BM_BOOT_TYPE_LEGACY];
  GuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (gEndOpCodeHandle[BM_BOOT_TYPE_LEGACY], &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  GuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  GuidLabel->Number = LEGACY_BOOT_ITEM_END_ID;

  BdsBm->GetOptions(&gNvBootOptions, &gNvBootOptionCount);

  MenuCtx = gBmMenuCtx;
  for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
    MenuCtx[Index].QuestionId = BMM_QUESTION_ID_BASE + (UINT16)Index;
    MenuCtx[Index].DevCount   = 0;
    MenuCtx[Index].Position   = 0xFFFF;
  }
  InitBootItems();
  InitBootGroup();
  UpdateGroupString();  

  for(TypeIndex=BM_BOOT_TYPE_MIN;TypeIndex<BM_BOOT_TYPE_MAX;TypeIndex++){
    for(Index=0;Index<=gMaxPosition[TypeIndex];Index++){

      MenuCtx = FindBmMenuCtxEntryByPosition(Index, TypeIndex);
      ASSERT(MenuCtx != NULL);
      
      OptionsOpCodeHandle = HiiAllocateOpCodeHandle();

      if (MenuCtx->DevCount > 0) {

        for(i=0;i<MenuCtx->DevCount;i++){
          Item = &MenuCtx->Item[i];
          HiiCreateOneOfOptionOpCode (
            OptionsOpCodeHandle,
            Item->DisplayStringToken,
            0,
            EFI_IFR_TYPE_NUM_SIZE_16,
            Item->Value
            );
        }
        
        HiiCreateOrderedListOpCode (                   
          gStartOpCodeHandle[TypeIndex],               // Container for dynamic created opcodes     
          MenuCtx->QuestionId,                   // Question ID                               
          VARSTORE_ID_BOOT_MAINT,                      // VarStore ID                               
          MenuCtx->VarOffset,                    // Offset in Buffer Storage                  
          MenuCtx->Prompt,                       // Question prompt text                      
          MenuCtx->Help,                         // Question help text                        
          EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_OPTIONS_ONLY,  // Question flag                             
          0,                                           // Ordered list flag, e.g. EFI_IFR_UNIQUE_SET
          EFI_IFR_TYPE_NUM_SIZE_16,                    // Data type of Question value               
          MAX_MENU_NUMBER,                             // Maximum container                         
          OptionsOpCodeHandle,                         // Option Opcode list                        
          NULL                                         // Default Opcode is NULL                    
          );
      } else {
        DEBUG((EFI_D_INFO, "ActionOp\n"));
        HiiCreateActionOpCode (
          gStartOpCodeHandle[TypeIndex],
          MenuCtx->QuestionId,
          MenuCtx->Prompt,
          MenuCtx->Help,
          EFI_IFR_FLAG_CALLBACK,
          0
          );
      }
      
      HiiFreeOpCodeHandle(OptionsOpCodeHandle);
    }
  
  }

  Status = HiiUpdateForm (
             BmmCallbackInfo->BmmHiiHandle,
             &gEfiFormsetGuidBoot,
             ROOT_FORM_ID,
             gStartOpCodeHandle[BM_BOOT_TYPE_UEFI],
             gEndOpCodeHandle[BM_BOOT_TYPE_UEFI]
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));    

  Status = HiiUpdateForm (
             BmmCallbackInfo->BmmHiiHandle,
             &gEfiFormsetGuidBoot,
             ROOT_FORM_ID,
             gStartOpCodeHandle[BM_BOOT_TYPE_LEGACY],
             gEndOpCodeHandle[BM_BOOT_TYPE_LEGACY]
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status)); 

  return EFI_SUCCESS;
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
  UINT16                      Position;
  UINTN                       i;
  VOID                        *OptionsOpCodeHandle;
  BM_BOOT_TYPE                BootType;
  EFI_IFR_GUID_LABEL          *GuidLabel;
  BM_BOOT_DEV_ITEM            *Item;
  BM_MENU_CTX                 *MenuCtx;
  BM_MENU_CTX                 *MyMenuCtx;  
  UINT16                      OldPosition;
  UINT16                      NewPosition;
  UINT16                      *p;
  BMM_CALLBACK_DATA           *Private;
  BOOLEAN                     Rc;
  EFI_STRING    RequestString = NULL;  
  
  DEBUG((EFI_D_INFO, "BMCB A:%X Q:%X\n", Action, QuestionId));

  if (Action == EFI_BROWSER_ACTION_FORM_OPEN || Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    Status = EFI_SUCCESS;
    goto ProcExit;
  }

  if (Value == NULL || ActionRequest == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  if(Action == EFI_BROWSER_ACTION_CHANGED){
    if(QuestionId == PXE_BOOT_ONEOF_ITEM_ID){		
      Rc = HiiGetBrowserData(
                 &gPlatformSetupVariableGuid, 
                 PLATFORM_SETUP_VARIABLE_NAME, 
                 sizeof(SETUP_DATA), 
                 (UINT8*)&gSetupDataTemp
                 );	  
      if(Rc){			
        DEBUG((EFI_D_ERROR, __FUNCTION__"(), SelectPxeNic :%d.\n", gSetupDataTemp.SelectPxeNic));
        gSetupDataTemp.SelectPxeNic = Value->u8;
        if (Value->u8 ==0) {
          gSetupDataTemp.PxeNicVid = 0;
          gSetupDataTemp.PxeNicDid = 0;
          gSetupDataTemp.PxeNicMacCrc = 0;
        } else {				
          gSetupDataTemp.PxeNicVid = gPxeVidDid[(Value->u8)-1]&0xFFFF;
          gSetupDataTemp.PxeNicDid = (gPxeVidDid[(Value->u8)-1]>>16)&0xFFFF;
          gSetupDataTemp.PxeNicMacCrc = gPxeMacCrc[(Value->u8)-1]&0xFFFF;
        }
        DEBUG((EFI_D_ERROR,__FUNCTION__"(), PxeNicVid :%x, PxeNicDid :%x.\n",gSetupDataTemp.PxeNicVid,gSetupDataTemp.PxeNicDid));
        DEBUG((EFI_D_ERROR,__FUNCTION__"(), PxeNicMacCrc :%x.\n",gSetupDataTemp.PxeNicMacCrc));
		
        RequestString = NULL;	  
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, SelectPxeNic), sizeof(gSetupDataTemp.SelectPxeNic));
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, PxeNicVid), sizeof(gSetupDataTemp.PxeNicVid));
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, PxeNicDid), sizeof(gSetupDataTemp.PxeNicDid));
        RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, PxeNicMacCrc), sizeof(gSetupDataTemp.PxeNicMacCrc));

        Rc = HiiSetBrowserData(
                    &gPlatformSetupVariableGuid,
                    PLATFORM_SETUP_VARIABLE_NAME,
                    sizeof(SETUP_DATA), 
                    (UINT8*)&gSetupDataTemp, 
                    RequestString
                    );
        FreePool (RequestString);
      }		
      return EFI_SUCCESS;      
    }
  } 

  if (QuestionId == KEY_VALUE_BOOTTYPE_CONFIGURATION) {
    if(Action == EFI_BROWSER_ACTION_CHANGED){
      DEBUG((EFI_D_INFO, "BootModeType-> %d\n", Value->u8));

     Rc = HiiGetBrowserData(
      &gPlatformSetupVariableGuid, 
      PLATFORM_SETUP_VARIABLE_NAME, 
      sizeof(SETUP_DATA), 
      (UINT8*)&gSetupDataTemp
      );
     DEBUG((EFI_D_ERROR, __FUNCTION__"(), HiiGetBrowserData Setup :%d.\n", Rc));
     if (Rc) {
       if (Value->u8 == BIOS_BOOT_UEFI_OS) {
         gSetupDataTemp.VideoRomPolicy = ROM_POLICY_UEFI_FIRST;
         gSetupDataTemp.PxeRomPolicy = ROM_POLICY_UEFI_FIRST;
         gSetupDataTemp.StorageRomPolicy = ROM_POLICY_UEFI_FIRST;
         gSetupDataTemp.OtherRomPolicy = ROM_POLICY_UEFI_FIRST;

       } else if (Value->u8 == BIOS_BOOT_LEGACY_OS) {
         gSetupDataTemp.VideoRomPolicy = ROM_POLICY_LEGACY_FIRST;
         gSetupDataTemp.PxeRomPolicy = ROM_POLICY_LEGACY_FIRST;
         gSetupDataTemp.StorageRomPolicy = ROM_POLICY_LEGACY_FIRST;
         gSetupDataTemp.OtherRomPolicy = ROM_POLICY_LEGACY_FIRST;
       }
       RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, VideoRomPolicy), sizeof(gSetupDataTemp.VideoRomPolicy));
       RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, PxeRomPolicy), sizeof(gSetupDataTemp.PxeRomPolicy));
       RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, StorageRomPolicy), sizeof(gSetupDataTemp.StorageRomPolicy));
       RequestString = HiiConstructRequestString (RequestString, OFFSET_OF(SETUP_DATA, OtherRomPolicy), sizeof(gSetupDataTemp.OtherRomPolicy));
		   
        HiiSetBrowserData(
          &gPlatformSetupVariableGuid,
          PLATFORM_SETUP_VARIABLE_NAME,
          sizeof(SETUP_DATA), 
          (UINT8*)&gSetupDataTemp, 
          RequestString
          );
       FreePool (RequestString);
     }
	  
      if (Value->u8 != BIOS_BOOT_UEFI_OS) {
        NotifySetupItemChanged(ITEM_CSM, STATUS_ENABLE);
      }
    }else if(Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD){
      Value->u8 = BIOS_BOOT_UEFI_OS;
      return EFI_SUCCESS;
    }
    return EFI_SUCCESS;
  }

  if(Action == EFI_BROWSER_ACTION_CHANGED){
    MenuCtx = FindBmMenuCtxEntryByQuestionId(QuestionId);
    if(MenuCtx == NULL){
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }

    Private = BMM_CALLBACK_DATA_FROM_THIS(This);
    Rc = HiiGetBrowserData(&gEfiFormsetGuidBoot, BYO_BOOT_MAINT_VAR_NAME, sizeof(BMM_FAKE_NV_DATA), (UINT8*)&Private->BmmFakeNvData);
    DEBUG((EFI_D_INFO, "HiiGetBrowserData:%d\n", Rc));
  }

  if (Action == BROWSER_ACTION_MOVE_UP || Action == BROWSER_ACTION_MOVE_DOWN) {
    MenuCtx = FindBmMenuCtxEntryByQuestionId(QuestionId);
    if(MenuCtx == NULL){
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }

    OldPosition = MenuCtx->Position;
    BootType = MenuCtx->BootType;

    if(Action == BROWSER_ACTION_MOVE_UP){
      if(MenuCtx->Position == 0){
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

    HiiFreeOpCodeHandle(gStartOpCodeHandle[BootType]);
    gStartOpCodeHandle[BootType] = HiiAllocateOpCodeHandle();    
    GuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(gStartOpCodeHandle[BootType], &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    GuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    GuidLabel->Number = gOpLabelNumber[BootType]; 

    for(Position=0;Position<=gMaxPosition[BootType];Position++){

      MyMenuCtx = FindBmMenuCtxEntryByPosition(Position, BootType);
     
      OptionsOpCodeHandle = HiiAllocateOpCodeHandle();

      if (MyMenuCtx->DevCount > 0) {

        p = (UINT16*)((UINTN)&BmmCallbackInfo->BmmFakeNvData + MyMenuCtx->VarOffset);
        for(i=0;i<MyMenuCtx->DevCount;i++){
          Item = FindBootDevItemEntryByValue(MyMenuCtx->Item, p[i]);
          HiiCreateOneOfOptionOpCode (
            OptionsOpCodeHandle,
            Item->DisplayStringToken,
            0,
            EFI_IFR_TYPE_NUM_SIZE_16,
            Item->Value
            );
        }
        
        HiiCreateOrderedListOpCode (                   
          gStartOpCodeHandle[BootType],                // Container for dynamic created opcodes     
          MyMenuCtx->QuestionId,                   // Question ID                               
          VARSTORE_ID_BOOT_MAINT,                      // VarStore ID                               
          MyMenuCtx->VarOffset,                    // Offset in Buffer Storage                  
          MyMenuCtx->Prompt,                       // Question prompt text                      
          MyMenuCtx->Help,                         // Question help text                        
          EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_OPTIONS_ONLY,  // Question flag                             
          0,                                           // Ordered list flag, e.g. EFI_IFR_UNIQUE_SET
          EFI_IFR_TYPE_NUM_SIZE_16,                    // Data type of Question value               
          MAX_MENU_NUMBER,                             // Maximum container                         
          OptionsOpCodeHandle,                         // Option Opcode list                        
          NULL                                         // Default Opcode is NULL                    
          );
      } else {
        HiiCreateActionOpCode (
          gStartOpCodeHandle[BootType],
          MyMenuCtx->QuestionId,
          MyMenuCtx->Prompt,
          MyMenuCtx->Help,
          EFI_IFR_FLAG_CALLBACK,
          0
          );
      }
      
      HiiFreeOpCodeHandle(OptionsOpCodeHandle);
    }

    Status = HiiUpdateForm (
               BmmCallbackInfo->BmmHiiHandle,
               &gEfiFormsetGuidBoot,
               ROOT_FORM_ID,
               gStartOpCodeHandle[BootType],
               gEndOpCodeHandle[BootType]
               );
    DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));    
    
  }else if(Action == EFI_BROWSER_ACTION_DEFAULT_STANDARD){
    MenuCtx = FindBmMenuCtxEntryByQuestionId(QuestionId);
    if(MenuCtx == NULL){
      DEBUG((EFI_D_INFO, "(L%d)\n", __LINE__));
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }
    if(MenuCtx->DevCount == 0){
      DEBUG((EFI_D_INFO, "(L%d)\n", __LINE__));
      Status = EFI_UNSUPPORTED;
      goto ProcExit;
    }
  }

ProcExit:
  return Status;
}


STATIC
VOID 
RestoreItemOrder (
  BM_MENU_CTX                    *MenuCtx,
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBootMgr
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


  DEBUG((EFI_D_INFO, "%a %X %X\n", __FUNCTION__, MenuCtx->DevCount, MenuCtx->MenuType));

  if(MenuCtx->DevCount < 2){
    return;
  }

  if(MenuCtx->BootType == BM_BOOT_TYPE_UEFI){
    Option = BdsBootMgr->UefiOptions;
    OptionCount = BdsBootMgr->UefiOptionsCount;
  } else if(MenuCtx->BootType == BM_BOOT_TYPE_LEGACY){
    Option = BdsBootMgr->LegacyOptions;
    OptionCount = BdsBootMgr->LegacyOptionsCount;
  } else {
    Option = NULL;
    OptionCount = 0;
  }
  DEBUG((EFI_D_INFO, "Option(%X,%X)\n", Option, OptionCount));
  

  for(i=0;i<MenuCtx->DevCount;i++){
    Item = &MenuCtx->Item[i];
    Item->Flag = 0;
    DEBUG((EFI_D_INFO, "Item->Value:%X\n", Item->Value));
  }

  p = AllocateZeroPool(MenuCtx->DevCount * sizeof(UINT16));
  ASSERT(p != NULL);

  VarBase = (UINTN)&BmmCallbackInfo->BmmFakeNvData;
  DumpMem8((UINT16*)(VarBase + MenuCtx->VarOffset), MenuCtx->DevCount * sizeof(UINT16));
  
  Count = 0;
  for(Index=0;Index<OptionCount;Index++){
    if(Option[Index].GroupType != MenuCtx->MenuType){
      continue;
    }

    for(i=0;i<MenuCtx->DevCount;i++){
      Item = &MenuCtx->Item[i];
      if(Item->Flag){
        continue;
      }

    if(BM_MENU_TYPE_UEFI_HDD == MenuCtx->MenuType || BM_MENU_TYPE_LEGACY_HDD == MenuCtx->MenuType){
      continue;
    }
      if(CompareMem(Option[Index].FilePath, Item->LoadOption->FilePath, GetDevicePathSize(Option[Index].FilePath)) == 0 ||
         (Option[Index].DupDp != NULL && CompareMem(Option[Index].DupDp, Item->LoadOption->FilePath, GetDevicePathSize(Option[Index].FilePath)) == 0)){
        p[Count++] = (UINT16)(Item->LoadOption->OptionNumber + 1);
        Item->Flag = 1;
      }
    }
  }

Count = 0;
  for(Index=0;Index<OptionCount;Index++){
    if(Option[Index].GroupType != MenuCtx->MenuType){
      continue;
    }

    for(i=0;i<MenuCtx->DevCount;i++){
      Item = &MenuCtx->Item[i];
      if(Item->Flag){
        continue;
      }
      if(StrStr(Item->LoadOption->Description,L"SATA2-0")!=NULL||StrStr(Item->LoadOption->Description,L"SATA2-1")!=NULL)
      {
        p[Count++] = (UINT16)(Item->LoadOption->OptionNumber + 1);
        Item->Flag = 1;
      }
    }

    for(i=0;i<MenuCtx->DevCount;i++){
      Item = &MenuCtx->Item[i];
      if(Item->Flag){
        continue;
      }
      if(StrStr(Item->LoadOption->Description,L"SATA2-0")==NULL&&StrStr(Item->LoadOption->Description,L"SATA2-1")==NULL&&StrStr(Item->LoadOption->Description,L"SATA")!=NULL)
      {
        p[Count++] = (UINT16)(Item->LoadOption->OptionNumber + 1);
        Item->Flag = 1;
      }
    }

    for(i=0;i<MenuCtx->DevCount;i++){
      Item = &MenuCtx->Item[i];
      if(Item->Flag){
        continue;
      }
      if(StrStr(Item->LoadOption->Description,L"NVME")!=NULL)
      {
        p[Count++] = (UINT16)(Item->LoadOption->OptionNumber + 1);
        Item->Flag = 1;
      }
    }
  }

  DumpMem8(p, MenuCtx->DevCount * sizeof(UINT16));
  
  for(i=0;i<MenuCtx->DevCount;i++){
    Item = &MenuCtx->Item[i];
    if(Item->Flag){
      continue;
    }
    p[Count++] = (UINT16)(Item->LoadOption->OptionNumber + 1);
    Item->Flag = 1;    
  }

  CopyMem((UINT16*)(VarBase + MenuCtx->VarOffset), p, MenuCtx->DevCount * sizeof(UINT16));
  FreePool(p);

  DumpMem8((UINT16*)(VarBase + MenuCtx->VarOffset), MenuCtx->DevCount * sizeof(UINT16));  
}






STATIC
VOID 
RestoreUserItemOrder (
  BM_MENU_CTX                    *MenuCtx,
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBootMgr
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

  if(MenuCtx->DevCount < 2){
    return;
  }

  MenuIndex = MenuCtx - gBmMenuCtx;
  DEBUG((EFI_D_INFO, "MenuIndex:%d\n", MenuIndex));  
  Info = &gUserBmMenuInfo[MenuIndex];

  for(i=0;i<MenuCtx->DevCount;i++){
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
  DumpMem8((UINT16*)(VarBase + MenuCtx->VarOffset), MenuCtx->DevCount * sizeof(UINT16));
  
  Count = 0;
  for(Index=0;Index<Info->DevCount;Index++){
//  if(Info->Flag[Index]){
//    continue;
//  }
    for(i=0;i<MenuCtx->DevCount;i++){
      Item = &MenuCtx->Item[i];
      if(Item->Flag){
        continue;
      }
      if(StrCmp(Info->DevName[Index], Item->DisplayString) == 0){
        p[Count++] = Item->Value;
        Item->Flag = 1;
        break;
      }
    }
  }

  DumpMem8(p, MenuCtx->DevCount * sizeof(UINT16));
  
  for(i=0;i<MenuCtx->DevCount;i++){
    Item = &MenuCtx->Item[i];
    if(Item->Flag){
      continue;
    }
    p[Count++] = Item->Value;;
    Item->Flag = 1;    
  }

  CopyMem((UINT16*)(VarBase + MenuCtx->VarOffset), p, MenuCtx->DevCount * sizeof(UINT16));
  FreePool(p);

  DumpMem8((UINT16*)(VarBase + MenuCtx->VarOffset), MenuCtx->DevCount * sizeof(UINT16));  
}







VOID RestoreBootOrder(BOOLEAN IsUserDefault)
{
  BM_MENU_CTX                    *MenuCtx;
  UINT16                         Index;
  UINTN                          TypeIndex;
  UINTN                          i;
  VOID                           *OptionsOpCodeHandle;
  BM_BOOT_DEV_ITEM               *Item;
  EFI_STATUS                     Status;
  EFI_IFR_GUID_LABEL             *GuidLabel;
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBootMgr;  
  BOOLEAN                        Rc;
  UINTN                          VarSize;
  UINT16                         *VarBuffer = NULL;


  DEBUG((EFI_D_INFO, "%a(%d)\n", __FUNCTION__, IsUserDefault));

  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  ASSERT(!EFI_ERROR(Status));

  if(IsUserDefault){
    VarBuffer = BdsBootMgr->GetVarAndSize(USER_BOOT_ORDER_VAR_NAME, &gByoGlobalVariableGuid, &VarSize);
    UserBmMenuInfoInit(VarBuffer, VarSize);
    if(VarBuffer == NULL){
      return;
    }
  }
    
  MenuCtx = gBmMenuCtx;

  Rc = HiiGetBrowserData(&gEfiFormsetGuidBoot, BYO_BOOT_MAINT_VAR_NAME, sizeof(BMM_FAKE_NV_DATA), (UINT8*)&BmmCallbackInfo->BmmFakeNvData);
  DEBUG((EFI_D_INFO, "HiiGetBrowserData:%d, VarBuffer:%X\n", Rc, VarBuffer));

  if(VarBuffer == NULL){
    ZeroMem(gMaxPosition, sizeof(gMaxPosition));
    for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
      gBmMenuCtx[Index].Position = gMaxPosition[gBmMenuCtx[Index].BootType]++;
    }
    for(TypeIndex=BM_BOOT_TYPE_MIN;TypeIndex<BM_BOOT_TYPE_MAX;TypeIndex++){
      if(gMaxPosition[TypeIndex]){
        gMaxPosition[TypeIndex]--;
      }  
    }
    
  } else {
    for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
      gBmMenuCtx[Index].Position = gUserBmMenuInfo[Index].Position;
    }  
  }


  for(TypeIndex=BM_BOOT_TYPE_MIN;TypeIndex<BM_BOOT_TYPE_MAX;TypeIndex++){

    HiiFreeOpCodeHandle(gStartOpCodeHandle[TypeIndex]);
    gStartOpCodeHandle[TypeIndex] = HiiAllocateOpCodeHandle();    
    GuidLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode(gStartOpCodeHandle[TypeIndex], &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    GuidLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    GuidLabel->Number = gOpLabelNumber[TypeIndex]; 

    for(Index=0;Index<=gMaxPosition[TypeIndex];Index++){

      MenuCtx = FindBmMenuCtxEntryByPosition(Index, TypeIndex);
      ASSERT(MenuCtx != NULL);
      
      OptionsOpCodeHandle = HiiAllocateOpCodeHandle();

      if (MenuCtx->DevCount > 0) {

        if(VarBuffer == NULL){
          RestoreItemOrder(MenuCtx, BdsBootMgr);
        } else {
          RestoreUserItemOrder(MenuCtx, BdsBootMgr);
        }

        for(i=0;i<MenuCtx->DevCount;i++){
          Item = &MenuCtx->Item[i];
          HiiCreateOneOfOptionOpCode (
            OptionsOpCodeHandle,
            Item->DisplayStringToken,
            0,
            EFI_IFR_TYPE_NUM_SIZE_16,
            Item->Value
            );
        }
        
        HiiCreateOrderedListOpCode (                   
          gStartOpCodeHandle[TypeIndex],               // Container for dynamic created opcodes     
          MenuCtx->QuestionId,                   // Question ID                               
          VARSTORE_ID_BOOT_MAINT,                      // VarStore ID                               
          MenuCtx->VarOffset,                    // Offset in Buffer Storage                  
          MenuCtx->Prompt,                       // Question prompt text                      
          MenuCtx->Help,                         // Question help text                        
          EFI_IFR_FLAG_CALLBACK | EFI_IFR_FLAG_OPTIONS_ONLY,  // Question flag                             
          0,                                           // Ordered list flag, e.g. EFI_IFR_UNIQUE_SET
          EFI_IFR_TYPE_NUM_SIZE_16,                    // Data type of Question value               
          MAX_MENU_NUMBER,                             // Maximum container                         
          OptionsOpCodeHandle,                         // Option Opcode list                        
          NULL                                         // Default Opcode is NULL                    
          );
      } else {
        DEBUG((EFI_D_INFO, "ActionOp\n"));
        HiiCreateActionOpCode (
          gStartOpCodeHandle[TypeIndex],
          MenuCtx->QuestionId,
          MenuCtx->Prompt,
          MenuCtx->Help,
          EFI_IFR_FLAG_CALLBACK,
          0
          );
      }
      
      HiiFreeOpCodeHandle(OptionsOpCodeHandle);
    }
  
  }


  Rc = HiiSetBrowserData(&gEfiFormsetGuidBoot, BYO_BOOT_MAINT_VAR_NAME, sizeof(BMM_FAKE_NV_DATA), (UINT8*)&BmmCallbackInfo->BmmFakeNvData, NULL);
  DEBUG((EFI_D_INFO, "HiiSetBrowserData:%d\n", Rc));

  Status = HiiUpdateForm (
             BmmCallbackInfo->BmmHiiHandle,
             &gEfiFormsetGuidBoot,
             ROOT_FORM_ID,
             gStartOpCodeHandle[BM_BOOT_TYPE_UEFI],
             gEndOpCodeHandle[BM_BOOT_TYPE_UEFI]
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));    

  Status = HiiUpdateForm (
             BmmCallbackInfo->BmmHiiHandle,
             &gEfiFormsetGuidBoot,
             ROOT_FORM_ID,
             gStartOpCodeHandle[BM_BOOT_TYPE_LEGACY],
             gEndOpCodeHandle[BM_BOOT_TYPE_LEGACY]
             );
  DEBUG((EFI_D_INFO, "HiiUpdateForm:%r\n", Status));

  if(VarBuffer != NULL){
    FreePool(VarBuffer);
  }
}



#if BC_TENCENT
UINT8 gLegacyBootGroupOrder[] = {
  BM_MENU_TYPE_LEGACY_HDD, 
  BM_MENU_TYPE_LEGACY_PXE,
  BM_MENU_TYPE_LEGACY_USB_ODD,
  BM_MENU_TYPE_LEGACY_USK_DISK
  };

UINT8 gUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD, 
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_USB_ODD,
  BM_MENU_TYPE_UEFI_USK_DISK,
  BM_MENU_TYPE_UEFI_OTHERS
  };
#else
UINT8 gLegacyBootGroupOrder[] = {
  BM_MENU_TYPE_LEGACY_HDD, 
  //BM_MENU_TYPE_LEGACY_ODD, 
  BM_MENU_TYPE_LEGACY_USK_DISK,
  BM_MENU_TYPE_LEGACY_PXE,
  BM_MENU_TYPE_LEGACY_USB_ODD,
  };

UINT8 gUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD, 
  //BM_MENU_TYPE_UEFI_ODD, 
  BM_MENU_TYPE_UEFI_USK_DISK,
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_USB_ODD,
  BM_MENU_TYPE_UEFI_OTHERS
  };
#endif



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

  if(IsUefi){
    t = 0;
  } else {
    t = 1;
  }

  c = 0;
  while(p < e){
    Pos = *p++;
    Count = *p++;

    if(Pos == Position){
      if(c == t){
        *DevCount = Count;
        return p;
      }
      c++;
    }
    
    for(i=0;i<Count;i++){
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

  for(Index=0;Index<ArrayCount;Index++){
    if(OptionNumber == Array[Index]){
      return TRUE;
    }
  }

  return FALSE;
}



VOID BootOrderLoadDefault()
{
  EFI_STATUS                     Status;
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBootMgr;  
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
  
 
  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  ASSERT(!EFI_ERROR(Status));

  UefiGroupCount = BM_MENU_CTX_UEFI_COUNT;

  VarBuffer = BdsBootMgr->GetVarAndSize(USER_BOOT_ORDER_VAR_NAME, &gByoGlobalVariableGuid, &VarSize);
  if(VarBuffer == NULL){
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

    BootOrder = BdsBootMgr->GetVarAndSize(L"BootOrder", &gEfiGlobalVariableGuid, &BootOrderSize);
    if(BootOrder != NULL){
      Count = BootOrderSize/sizeof(UINT16);
      for(Index=0;Index<Count;Index++){
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
    
  }else {

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
                    BYO_LEGACY_BOOT_GROUP_VAR_NAME, 
                    &gByoGlobalVariableGuid, 
                    BYO_BG_ORDER_VAR_ATTR, 
                    sizeof(GroupOrder), 
                    GroupOrder
                    );
    ASSERT(!EFI_ERROR(Status));

    BdsBootMgr->GetOptions(&Options, &OptionCount);
    if(OptionCount){
      NewBootOrder = AllocatePool(OptionCount * sizeof(UINT16));
      i = 0;
      for(Index=0;Index<BM_MENU_CTX_COUNT;Index++){
        DevName = GetUserBootOrderData(VarBuffer, VarSize, (UINT8)(Index%UefiGroupCount), Index < UefiGroupCount, &DevCount);
        for(j=0;j<DevCount;j++){
          DEBUG((EFI_D_INFO, "DevName:%s\n", DevName));
          if(IsThisNvDevPresent(DevName, Options, OptionCount, &OptionNumber) &&
             !IsThisOptionNumberPresent(OptionNumber, NewBootOrder, i)){
            NewBootOrder[i++] = OptionNumber;
          }
          DevName += StrLen(DevName) + 1;
        }
      }

      for(Index=0;Index<OptionCount;Index++){
        if(!IsThisOptionNumberPresent((UINT16)Options[Index].OptionNumber, NewBootOrder, i)){
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

      BdsBootMgr->FreeOptions(Options, OptionCount);
      FreePool(NewBootOrder);
    }
  }

  if(VarBuffer != NULL){
    FreePool(VarBuffer);
  }
}

VOID BMCConfigSetup(
  UINT8        *GroupOrder )
{
  EFI_STATUS      Status;
  UINTN           Size;
  BMC_CONFIG_SETUP       commanddata,ResponseData;
  SETUP_DATA            SetupData;
  UINTN        GroupOrderSize = 0;
  UINT8        Index;
  UINT8                 ResponseDataSize;
  CHAR8                    *CurrentLang;
  UINT8        *CMD;
  CBS_CONFIG    CbsVariable;
  UINTN        BufferSize;
  CBS_CONFIG   CBSData;
  
  Size = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );  
  TranslateBmcConfig (&commanddata, &SetupData, 0);
  Size = sizeof (CBS_CONFIG);
  Status = gRT->GetVariable(L"AmdSetup", &gCbsSystemConfigurationGuid, NULL, &Size, &CbsVariable);
  DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), Get CBS :%r, Size :%d.\n", __LINE__, Status, Size));
  if (!EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), CbsDfCmnMemIntlv :%d.\n", __LINE__, CbsVariable.CbsDfCmnMemIntlv));
    DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), CbsCmnCpuCpb :%d.\n", __LINE__, CbsVariable.CbsCmnCpuCpb));

    if (CbsVariable.CbsDfCmnMemIntlv == 7) {
      commanddata.CbsDfCmnMemIntlv = 4;
    } else {
      commanddata.CbsDfCmnMemIntlv = CbsVariable.CbsDfCmnMemIntlv;
    }
    commanddata.CbsCmnCpuCpb = CbsVariable.CbsCmnCpuCpb;
  }

  
  if(SetupData.BootModeType != 1){  
  	for(Index=0;Index<4;Index++){
	  GroupOrder[Index] -= 0x80;
    }
    GroupOrder[4] = 0x05;
  }
  commanddata.FirstBoot = GroupOrder[0];
  commanddata.SecondBoot= GroupOrder[1];
  commanddata.ThirdBoot = GroupOrder[2];
  commanddata.FourthBoot= GroupOrder[3];
  commanddata.FifthBoot = GroupOrder[4]; 
  commanddata.SixthBoot = 0x01;

  CurrentLang = GetEfiGlobalVariable(L"PlatformLang");
  DEBUG((EFI_D_INFO, "CurrentLang:%a\n", CurrentLang));
  if (CurrentLang[0] == 'z') {
    commanddata.Language = 0;
  } else {
    commanddata.Language = 1;
  }
  if(CurrentLang != NULL){
    FreePool(CurrentLang);
  }
  
  BufferSize = sizeof(CBS_CONFIG);
  Status = gRT->GetVariable (
              L"AmdSetup",
              &gCbsSystemConfigurationGuid,
              NULL,
              &BufferSize,
              &CBSData
              );
  commanddata.CbsCmnCpuCpb = CBSData.CbsCmnCpuCpb;
  CMD = (UINT8*)&commanddata;
  DumpMem8(CMD, sizeof(BMC_CONFIG_SETUP));
  
  EfiInitializeIpmiBase();
  Status = EfiSendCommandToBMC (
                  SM_BYOSOFT_NETFN_APP,
                  0xc3,
                  (UINT8 *) &commanddata,
                  sizeof(commanddata),
                  (UINT8 *)&ResponseData,
                  (UINT8 *)&ResponseDataSize
                  );
  DEBUG((EFI_D_INFO,"Send command Status = %r\n",Status));
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
  UINTN             BootOrderSize;
  UINT8             *GroupOrderList[BM_BOOT_TYPE_MAX];
  UINT8             GroupOrderIndex[BM_BOOT_TYPE_MAX];
  SETUP_DATA        SetupData;
  UINTN             Size;


  DEBUG((EFI_D_INFO, "%a C:%d P:%d %d\n", __FUNCTION__, gNvBootOptionCount, gMaxPosition[0], gMaxPosition[1]));

  BootOrderSize = gNvBootOptionCount * sizeof(UINT16);
  if(gNvBootOptionCount != 0){
    NewBootOrder = AllocatePool(BootOrderSize);
    ASSERT(NewBootOrder != NULL);
  }

  GroupOrderList[BM_BOOT_TYPE_UEFI] = UefiGroupOrder;
  GroupOrderList[BM_BOOT_TYPE_LEGACY] = LegacyGroupOrder; 
  ZeroMem(GroupOrderIndex, sizeof(GroupOrderIndex));

  for(BootType = BM_BOOT_TYPE_MIN; BootType < BM_BOOT_TYPE_MAX; BootType++){
    for(Position=0; Position<=gMaxPosition[BootType]; Position++){
      MenuCtx = FindBmMenuCtxEntryByPosition(Position, BootType);
      ASSERT(MenuCtx != NULL);
//    DEBUG((EFI_D_INFO, "MenuType:%X VarOffset:%X\n", MenuCtx->MenuType, MenuCtx->VarOffset));
      GroupOrderList[BootType][GroupOrderIndex[BootType]++] = MenuCtx->MenuType;
      if(NewBootOrder != NULL){
        p = (UINT16*)((UINTN)&BmmCallbackInfo->BmmFakeNvData + MenuCtx->VarOffset);
        for(i=0;i<MenuCtx->DevCount;i++){
          Item = FindBootDevItemEntryByValue(MenuCtx->Item, p[i]);
          ASSERT(Item != NULL);
          DEBUG((EFI_D_INFO, "[%d] %d %s\n", i, p[i], Item->DisplayString));
          NewBootOrder[NewBootOrderIndex++] = p[i] - 1;
        }
      }
    }
  }

  DumpMem8(LegacyGroupOrder, sizeof(LegacyGroupOrder));
  DumpMem8(UefiGroupOrder, sizeof(UefiGroupOrder));
  DumpMem8(NewBootOrder, gNvBootOptionCount * sizeof(UINT16));

  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  NV_VAR_FLAG,
                  BootOrderSize,
                  NewBootOrder
                  );
  DEBUG((EFI_D_INFO, "Set BootOrder:%r\n", Status));

  Status = gRT->SetVariable (
                  BYO_UEFI_BOOT_GROUP_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  NV_VAR_FLAG,
                  GroupOrderIndex[BM_BOOT_TYPE_UEFI],
                  UefiGroupOrder
                  );
  DEBUG((EFI_D_INFO, "Set UefiGroupBootOrder:%r\n", Status));
  DEBUG((EFI_D_INFO,"commanddata %0x %0x %0x %0x %0x %0x \n",UefiGroupOrder[0],UefiGroupOrder[1],UefiGroupOrder[2],UefiGroupOrder[3],UefiGroupOrder[4],UefiGroupOrder[5]));

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
  
  Size = sizeof (SETUP_DATA);
  Status = gRT->GetVariable (
                  PLATFORM_SETUP_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &Size,
                  &SetupData
                  );  
  SavePlatformLanguage();
  if(SetupData.BootModeType == 1){
    BMCConfigSetup(UefiGroupOrder);
  } else {
	BMCConfigSetup(LegacyGroupOrder);
  }
  
  return EFI_SUCCESS;
}






VOID
ByoFreeBMPackage (
  VOID
  )
{
  if(BmmCallbackInfo->BmmHiiHandle != NULL){
    HiiRemovePackages (BmmCallbackInfo->BmmHiiHandle);
  }

  if (gStartOpCodeHandle[BM_BOOT_TYPE_UEFI] != NULL) {
    HiiFreeOpCodeHandle (gStartOpCodeHandle[BM_BOOT_TYPE_UEFI]);
  }
  if (gStartOpCodeHandle[BM_BOOT_TYPE_LEGACY] != NULL) {
    HiiFreeOpCodeHandle (gStartOpCodeHandle[BM_BOOT_TYPE_LEGACY]);
  }  
  if (gEndOpCodeHandle[BM_BOOT_TYPE_UEFI] != NULL) {
    HiiFreeOpCodeHandle (gEndOpCodeHandle[BM_BOOT_TYPE_UEFI]);
  }
  if (gEndOpCodeHandle[BM_BOOT_TYPE_LEGACY] != NULL) {
    HiiFreeOpCodeHandle (gEndOpCodeHandle[BM_BOOT_TYPE_LEGACY]);
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
  EFI_BDS_BOOT_MANAGER_PROTOCOL  *BdsBootMgr;   
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

  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, &BdsBootMgr);
  ASSERT(!EFI_ERROR(Status));

  Info = gUserBmMenuInfo;
  ZeroMem(&gUserBmMenuInfo, sizeof(gUserBmMenuInfo));

  BdsBootMgr->GetOptions(&NvBootOption, &NvBootOptionCount);
//SysCfg = (BYO_SYS_CFG_STS_DATA*)GetByoSysAndStsHobData();

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
    if (NvBootOption[Index].Ignore) {
      continue;
    }

    GroupOrder = (BM_MENU_TYPE)NvBootOption[Index].GroupType;
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
/*  
  DEBUG((EFI_D_INFO, "RSVMEM(%X,%X)\n", SysCfg->BootOrderRsvMemAddr, SysCfg->BootOrderRsvMemSize));
  if(BufferSize <= SysCfg->BootOrderRsvMemSize){
    CopyMem((VOID*)(UINTN)SysCfg->BootOrderRsvMemAddr, Buffer, BufferSize);
    *(UINT32*)(UINTN)SysCfg->BootOrderDataSizeAddr = (UINT32)BufferSize;
  }
*/
  FreePool(Buffer);
}


