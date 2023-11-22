/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BoundHddCheck.c

Abstract:
  Binding the harddisk(SATA or NVME) according to the configuration set by users.
When this function has been opened, it can't boot from the unbound harddisk.

Revision History:

**/

#include "HddBindDxe.h"
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/GlobalVariable.h>
#include <ByoStatusCode.h>
#include <Protocol/BlockIo.h>
extern HDD_BIND_PRIVATE_DATA gPrivateData;

#ifndef BYO_ONLY_UEFI
extern UINT16  mBbsIndex;

VOID
EFIAPI
GetLegacyDevicePath (
  IN EFI_BOOT_MANAGER_LOAD_OPTION            *BootOption,
  IN OUT EFI_DEVICE_PATH_PROTOCOL            **DevicePath
  )
{
	EFI_STATUS                      Status;
  UINTN                           Index;
  UINT16                          BbsIndex;
  HDD_BIND_INFO                   *ValidHddInfo;
  UINT16                          ValidHddCount;
  EXTEND_HDD_DATA                 *AllHddInfo;
  UINT16                          AllHddCount;  
  EXTEND_HDD_DATA                 *ExtendHddData;
  UINTN                           HandleCount;
  EFI_HANDLE                      *Handles = NULL;
  UEFI_BBS_RELATION_INFO_PROTOCOL          *UefiBbsInfo = NULL;
  UefiBbsHddRL                    *UefiBbsHddTable;
  UINTN                           HBTableIndex; 

  if (DevicePath == NULL) {
    return;
  }
  
  BbsIndex = LegacyBootOptionalDataGetBbsIndex(BootOption->OptionalData);
  Status = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &UefiBbsInfo);
  if (!EFI_ERROR(Status)) {
    UefiBbsHddTable = UefiBbsInfo->UefiBbsHddTable;
    for (HBTableIndex = 0; HBTableIndex < UefiBbsInfo->UefiSataHddCount; HBTableIndex++) {
      if (UefiBbsHddTable[HBTableIndex].BbsIndex == BbsIndex) {
        mBbsIndex = BbsIndex;
        *DevicePath = DuplicateDevicePath(UefiBbsHddTable[HBTableIndex].HddDevicePath);
        return;
      }
    }
  }

  //
  // Get valid hard disks
  //
  Status = gBS->LocateHandleBuffer (
                ByProtocol,
                &gEfiDiskInfoProtocolGuid,
                NULL,
                &HandleCount,
                &Handles
                );
  if (EFI_ERROR(Status)) {
    return;
  } else {
    AllHddCount = EnumerateAllHddInfo(&AllHddInfo);
    if ((AllHddCount == 0) || (AllHddInfo == NULL)) {
      return;
    }
    ValidHddInfo = (HDD_BIND_INFO *)AllocateZeroPool(sizeof(HDD_BIND_INFO) * HandleCount);
    ASSERT(ValidHddInfo != NULL);
    ValidHddCount = FilterOutValidHdd(AllHddInfo, AllHddCount, HandleCount, ValidHddInfo);
    FreePool(AllHddInfo);
  }

  if ((ValidHddInfo == NULL) || (ValidHddCount == 0)) {
    return;
  }

  //
  // To find the real DevicePath of current legacy boot options
  //
  for (Index = 0; Index < ValidHddCount; Index++) {
    ExtendHddData = (EXTEND_HDD_DATA *)ValidHddInfo[Index].ExtendData;
    if (BbsIndex == ExtendHddData->BbsIndex) {
      mBbsIndex = BbsIndex;
      *DevicePath = DuplicateDevicePath(ExtendHddData->DevicePath);
      break;
    }
  }

  for (Index = 0; Index < ValidHddCount; Index++) {
    ExtendHddData = (EXTEND_HDD_DATA *)ValidHddInfo[Index].ExtendData;
    if (ExtendHddData != NULL) {
      if (ExtendHddData->DevicePath != NULL) {
        FreePool(ExtendHddData->DevicePath);
      }
      FreePool(ExtendHddData);
    }
  }
  FreePool(ValidHddInfo);
}
#endif

/**
  Handle status codes reported through ReportStatusCodeLib /
  EFI_STATUS_CODE_PROTOCOL.ReportStatusCode().Getting DevicePath from L"BootCurrent",
  then checking the bound  harddisk.

  @param[in] Event    Event whose notification function is being invoked.

  @param[in] Context  Pointer to EFI_RSC_HANDLER_PROTOCOL, originally looked up
                      when HandleStatusCode() was registered.
**/
EFI_STATUS
EFIAPI
HandleStatusCode (
  IN EFI_STATUS_CODE_TYPE                    CodeType,
  IN EFI_STATUS_CODE_VALUE                   Value,
  IN UINT32                                  Instance,
  IN EFI_GUID                                *CallerId,
  IN EFI_STATUS_CODE_DATA                    *Data
  )
{
  EFI_STATUS                Status;
  UINT16                    BootCurrent;
  UINTN                     VariableSize;
  EFI_DEVICE_PATH_PROTOCOL  *TempDevicePath;
  CHAR16                    BootOptionName[ARRAY_SIZE (L"Boot####")];
  EFI_BOOT_MANAGER_LOAD_OPTION             BootOption;
  BOOLEAN                   FoundMassStorage = FALSE;
  EFI_DEVICE_PATH_PROTOCOL  *OsLoaderFullPath = NULL;
  EFI_HANDLE                Handle;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath = NULL;
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;
  
  //
  // Ignore all status codes that are not reported by 
  // REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32 (PcdProgressCodeOsLoaderLoad));
  //
  if ((CodeType != EFI_PROGRESS_CODE) || ((Value != PcdGet32 (PcdProgressCodeOsLoaderLoad)) && (Value != BYO_LEAGCY_BOOT_START))) {
    return EFI_SUCCESS;
  }

  ZeroMem(&BootOption, sizeof(EFI_BOOT_MANAGER_LOAD_OPTION));
  #ifndef BYO_ONLY_UEFI
    mBbsIndex = FIRST_USB_BBS_INDEX;
  #endif
  
  //
  // Get DevicePath of current boot option from the variable L"BootCurrent"
  //
  VariableSize = sizeof(BootCurrent);
  Status = gRT->GetVariable (EFI_BOOT_CURRENT_VARIABLE_NAME,
                            &gEfiGlobalVariableGuid,
                            NULL,
                            &VariableSize,
                            &BootCurrent);                        
  if (EFI_ERROR(Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%a: failed to get %g:\"%s\": %r\n",
      gEfiCallerBaseName, __FUNCTION__, &gEfiGlobalVariableGuid,
      EFI_BOOT_CURRENT_VARIABLE_NAME, Status));
    return Status;  
  }
  if (VariableSize != sizeof(BootCurrent)) {
    DEBUG ((DEBUG_ERROR, "%a:%a: got %Lu bytes for %g:\"%s\", expected %Lu\n",
      gEfiCallerBaseName, __FUNCTION__, (UINT64)VariableSize,
      &gEfiGlobalVariableGuid, EFI_BOOT_CURRENT_VARIABLE_NAME,
      (UINT64)sizeof BootCurrent));
    return EFI_INCOMPATIBLE_VERSION;
  }
  
  UnicodeSPrint (BootOptionName, sizeof BootOptionName, L"Boot%04x",BootCurrent);
  Status = EfiBootManagerVariableToLoadOption (BootOptionName, &BootOption);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR,
      "%a:%a: EfiBootManagerVariableToLoadOption(\"%s\"): %r\n",
      gEfiCallerBaseName, __FUNCTION__, BootOptionName, Status));
    goto CHECK_EXIT;
  } 

  #ifndef BYO_ONLY_UEFI
    if ((BootOption.FilePath->Type == BBS_DEVICE_PATH) && (BootOption.FilePath->SubType == BBS_BBS_DP)) {
      GetLegacyDevicePath(&BootOption, &BootOption.FilePath);
    }
  #endif
  TempDevicePath = BootOption.FilePath;

  //
  // Get NvData of bound hdd
  //
  GetHddBindNvData();
  if (gPrivateData.TargetSn == NULL) {
    Status = EFI_SUCCESS;
    goto CHECK_EXIT;
  }
  
  //
  // Check if it's HD Short-Form DevicePath
  //
  OsLoaderFullPath = CheckHdShortDp(BootOption.FilePath);
  if (OsLoaderFullPath != NULL) {
    TempDevicePath = OsLoaderFullPath;
  }

  if ((StrStr(BootOption.Description, L"Lun") != NULL)
    || (StrStr(BootOption.Description, L"Pun") != NULL)
    || (StrStr(BootOption.Description, L"ASMT106") != NULL)
    || (StrStr(BootOption.Description, L" RAID") != NULL)
    || (StrStr(BootOption.Description, L"SAS:") != NULL)) {
    DEBUG ((DEBUG_ERROR, "Target Sn Not Match, Deny!\n"));
    ShowWarningMsg ();
    Status = EFI_ACCESS_DENIED;
    goto CHECK_EXIT;
  }

  //
  // Don't check SATA DVD or USB
  //
  DevicePath = TempDevicePath;
  Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &DevicePath, &Handle);
  if (!EFI_ERROR(Status)) {
    Status = gBS->HandleProtocol (
                        Handle,
                        &gEfiBlockIoProtocolGuid,
                        (VOID **)&BlockIo
                        );
    if (!EFI_ERROR(Status)) {
      if (BlockIo->Media->RemovableMedia == 1 || BlockIo->Media->BlockSize == SIZE_2KB) {
        Status = EFI_SUCCESS;
        goto CHECK_EXIT;
      }
    }
  }

  while (!IsDevicePathEndType (TempDevicePath) ) { // check if it's nvme or sata
		DEBUG((EFI_D_ERROR," DevicePath->Type %x DevicePath->SubType %x\n",TempDevicePath->Type,TempDevicePath->SubType));
    if ((TempDevicePath->Type == MESSAGING_DEVICE_PATH) && ((TempDevicePath->SubType == MSG_SATA_DP) || (TempDevicePath->SubType == MSG_NVME_NAMESPACE_DP) || (TempDevicePath->SubType == MSG_ATAPI_DP))) {
      FoundMassStorage = TRUE;
      break;          
    }
		TempDevicePath = NextDevicePathNode (TempDevicePath);
  }
  if (!FoundMassStorage) {
    Status = EFI_SUCCESS;
    goto CHECK_EXIT;
  }

  //
  // pass DevicePath of the boot option to CheckBindHdd() function for checking.
  //
  if (OsLoaderFullPath != NULL) {
    CheckBindHdd(TempDevicePath, OsLoaderFullPath); 
  } else {
    CheckBindHdd(TempDevicePath, BootOption.FilePath);
  }
  Status = EFI_SUCCESS;

CHECK_EXIT:
  if (OsLoaderFullPath != NULL) {
    FreePool(OsLoaderFullPath);
  }
  EfiBootManagerFreeLoadOption(&BootOption);

  return Status;
}


/**
  Unregister HandleStatusCode() at ExitBootServices().

  (See EFI_RSC_HANDLER_PROTOCOL in Volume 3 of the Platform Init spec.)

  @param[in] Event    Event whose notification function is being invoked.

  @param[in] Context  Pointer to EFI_RSC_HANDLER_PROTOCOL, originally looked up
                      when HandleStatusCode() was registered.
**/
VOID
EFIAPI
UnregisterAtExitBootServices (
  IN EFI_EVENT                               Event,
  IN VOID                                    *Context
  )
{
  EFI_RSC_HANDLER_PROTOCOL  *StatusCodeRouter;

  StatusCodeRouter = Context;
  StatusCodeRouter->Unregister (HandleStatusCode);
}
