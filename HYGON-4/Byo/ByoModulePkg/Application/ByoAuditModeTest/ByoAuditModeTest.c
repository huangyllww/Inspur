/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <Uefi.h>
#include <Guid/MdeModuleHii.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Library/ByoCommLib.h>
#include <Guid/ImageAuthentication.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>



EFI_STATUS
EFIAPI
ByoAuditModeTestEntry (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{ 
  EFI_STATUS                       Status;
  EFI_IMAGE_EXECUTION_INFO_TABLE  *ImageExeInfoTable;
  UINTN                           NameStringLen;
  EFI_IMAGE_EXECUTION_INFO        *ImageExeInfoEntry;
  EFI_IMAGE_EXECUTION_INFO        *ImageExeInfoEntryNew;
  UINTN                           Num;
  UINTN                           DevicePathSize;
  UINTN                           SignatureSize;
  CHAR16                          *NameStr;
  EFI_DEVICE_PATH_PROTOCOL         *DevicePath;
  EFI_SIGNATURE_LIST               *Signature;
  
  Status                = EFI_SUCCESS;
  ImageExeInfoTable     = NULL;
  NameStringLen         = 0;
  ImageExeInfoEntryNew  = NULL;
  ImageExeInfoEntry     = NULL;
  NameStr               = NULL;
  DevicePath            = NULL;
  Signature             = NULL;
  
  EfiGetSystemConfigurationTable (&gEfiImageSecurityDatabaseGuid, (VOID **) &ImageExeInfoTable);

  ImageExeInfoEntry = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) ImageExeInfoTable + sizeof(UINTN));
  ImageExeInfoEntryNew = ImageExeInfoEntry;
  Print(L"    ImageExeInfoTable->NumberOfImages:%d\n", ImageExeInfoTable->NumberOfImages);
  for(Num = 1;Num <= ImageExeInfoTable->NumberOfImages;Num++)
  { 
    ImageExeInfoEntry = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) ImageExeInfoEntry  + sizeof (EFI_IMAGE_EXECUTION_ACTION) + sizeof(UINT32));
    NameStr = (CHAR16 *)(ImageExeInfoEntry);
    Print(L"    NameStr :%s\n", NameStr);
    NameStringLen = StrSize (NameStr);

    DevicePath = (EFI_DEVICE_PATH_PROTOCOL *)((UINT8 *) NameStr + NameStringLen);
    Print(L"    DevicePath->Type :%x DevicePath->SubType:%x\n", DevicePath->Type,DevicePath->SubType);
    DevicePathSize =  GetDevicePathSize (DevicePath);

    Signature = (EFI_SIGNATURE_LIST *)((UINT8 *) NameStr + NameStringLen + DevicePathSize);
    Print(L"    Signature :%g\n", &Signature->SignatureType);
    SignatureSize = Signature->SignatureListSize;

    if(ImageExeInfoEntryNew->Action == (EFI_IMAGE_EXECUTION_AUTH_SIG_PASSED | EFI_IMAGE_EXECUTION_INITIALIZED)){
      Print(L"    Verify Status : Pass\n");
    }else if(ImageExeInfoEntryNew->Action == (EFI_IMAGE_EXECUTION_AUTH_SIG_FAILED | EFI_IMAGE_EXECUTION_INITIALIZED)){
      Print(L"    Verify Status : Failed\n");
    }else if(ImageExeInfoEntryNew->Action == (EFI_IMAGE_EXECUTION_POLICY_FAILED | EFI_IMAGE_EXECUTION_INITIALIZED)){
      Print(L"    Verify Status : POLICY_FAILED\n");
    }

    Print(L" \n");

    ImageExeInfoEntry = (EFI_IMAGE_EXECUTION_INFO *) ((UINT8 *) ImageExeInfoEntry  + NameStringLen + DevicePathSize +SignatureSize);
    ImageExeInfoEntryNew = ImageExeInfoEntry;

  }

  
  
  return Status;
  
}