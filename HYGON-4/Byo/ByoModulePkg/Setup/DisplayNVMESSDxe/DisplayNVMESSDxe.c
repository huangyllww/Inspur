/*++

  Copyright (c) 2023 Byosoft Corporation. All rights reserved.
  This program and associated documentation (if any) is furnished
  under a license. Except as permitted by such license,no part of this
  program or documentation may be reproduced, stored divulged or used
  in a public system, or transmitted in any form or by any means
  without the express written consent of Byosoft Corporation.

Module Name:

Abstract:

Revision History:

--*/
#include <Uefi.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiLib.h>

#include <Protocol/DevicePath.h>
#include <Guid/ByoSetupFormsetGuid.h>
#include "DisplayNVMESSConf.h"
#include <Protocol/ByoDiskInfoProtocol.h>

#define NEWSTRING_SIZE 0x200
typedef UINT16 STRING_REF;

EFI_GUID gByoSetupDisplayNVMeSSDGuid = SETUP_DISPLAY_NVMESSD_GUID;
extern unsigned char DisplayNVMESSDBin[];

VOID
EFIAPI
EnterSetupCallback (
  IN EFI_EVENT    Event,
  IN VOID*        Context
  )
{
  EFI_STATUS            Status;
  BYO_DISKINFO_PROTOCOL           *ByoDiskInfo;
  UINTN                           Index;
  UINTN                           HandleCount;
  UINTN                           Size;
  EFI_HANDLE                      *Handles;
  CHAR8                           Mn[41];
  CHAR8                           Fr[9];
  UINT64                          DiskSize;
  UINT32                          DriveSizeInGB;
  UINT64                          RemainderInBytes;
  CHAR16                          *NewString;
  EFI_STRING_ID                   TokenToUpdate;
  VOID                            *Interface;
  EFI_HII_HANDLE                  HiiHandle;

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->CloseEvent(Event);

  NewString   = AllocateZeroPool (NEWSTRING_SIZE);

  Status = gBS->LocateHandleBuffer (
         ByProtocol,
         &gByoDiskInfoProtocolGuid,
         NULL,
         &HandleCount,
         &Handles
         );

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gByoDiskInfoProtocolGuid,
                    (VOID **)&ByoDiskInfo
                    );

    if (EFI_ERROR (Status)) {
      continue;
    }

    if(ByoDiskInfo->DevType == BYO_DISK_INFO_DEV_NVME){

      HiiHandle = HiiAddPackages (
                    &gByoSetupDisplayNVMeSSDGuid,
                    Handles[Index],
                    STRING_ARRAY_NAME,
                    DisplayNVMESSDBin,
                    NULL
                    );
      ASSERT(HiiHandle != NULL);

      Size = sizeof(Mn);
      Status = ByoDiskInfo->GetMn(ByoDiskInfo, Mn, &Size);
      Size = sizeof(Fr);
      Status = ByoDiskInfo->GetFr(ByoDiskInfo, Fr, &Size);
      Status = ByoDiskInfo->GetDiskSize(ByoDiskInfo, &DiskSize);

      DriveSizeInGB = (UINT32) DivU64x64Remainder(DiskSize, 1000000000, &RemainderInBytes);
      DriveSizeInGB &=~1;

      UnicodeSPrint (NewString, NEWSTRING_SIZE, L"%a", Mn);
      TokenToUpdate = (STRING_REF) STRING_TOKEN (STR_SSD_MODEL_NUMBER_VALUE);
      HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);

      UnicodeSPrint (NewString, NEWSTRING_SIZE, L"%a", Fr);
      TokenToUpdate = (STRING_REF) STRING_TOKEN (STR_SSD_FIRMWARE_REVISION_VALUE);
      HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);

      UnicodeSPrint (NewString, NEWSTRING_SIZE, L"%d", DriveSizeInGB);
      TokenToUpdate = (STRING_REF) STRING_TOKEN (STR_SSD_CAPACITY_VALUE);
      HiiSetString (HiiHandle, TokenToUpdate, NewString, NULL);
    }
  }
  gBS->FreePool (NewString);
}

EFI_STATUS
EFIAPI
DisplayNVMESSDxeEntryPoint(
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  VOID                  *Registration;

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    EnterSetupCallback,
    NULL,
    &Registration
  );

  return EFI_SUCCESS;
}