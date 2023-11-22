/** @file

Copyright (c) 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Protocol/Smbios.h>
#include <Protocol/PciIo.h>
#include <Protocol/PlatformUsbDeviceSupport.h>
#include <IndustryStandard/SmBios.h>
#include <ByoSmbiosTable.h>
#include <Protocol/ByoPcdAccess.h>
#include <ByoDevicePatch.h>
#include <Library/ByoCommLib.h>
#include <Guid/SmmVariableCommon.h>
#include <Protocol/SetupItemUpdate.h>
#include <Library/DxeServicesLib.h>


UINT64                          HobDataAddress;
UINT32                          mSetupVariableNumber = 0;
UINT32                          mSetupItemNumber = 0;
SETUP_ITEM_INFO                 *mSetupItem = NULL;
SETUP_VARIABLE_TABLE            *mSetupVariableTable = NULL;


VOID
LocatSetupItemsDb (
  )
{
  VOID  *Buffer;
  UINTN BufferSize;
  SETUP_DATABASE_LAYOUT *SetupDB;
  EFI_STATUS Status;

  //
  // Get Setup Item DataBase
  //
  Buffer = NULL;
  Status = GetSectionFromAnyFv (
            &gByoSetupItemsDBGuid, 
            EFI_SECTION_RAW, 
            0, 
            &Buffer,
            &BufferSize
          );

  if (Status == EFI_SUCCESS) {
    ASSERT(BufferSize >= sizeof(SETUP_DATABASE_LAYOUT));
    SetupDB = (SETUP_DATABASE_LAYOUT *) Buffer;
    mSetupVariableNumber = SetupDB->SetupVariableNumber;
    mSetupItemNumber = SetupDB->SetupItemNumber;
    mSetupVariableTable  = SetupDB->VariableTable;
    mSetupItem           = (SETUP_ITEM_INFO *) (mSetupVariableTable + mSetupVariableNumber);
  }

  return;
}


EFI_STATUS
FindSetupItemOffset(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  OUT UINT32                      *SetupVariableOffset,
  OUT UINT32                      *SetupItemOffset
  )
{
  UINT32                        Index,VariableIndex;
  for ( Index = 0; Index < mSetupVariableNumber; Index++){
    if (StrCmp(VariableName, mSetupVariableTable[Index].VariableName) == 0){
      if (CompareMem(VendorGuid, &mSetupVariableTable[Index].VariableGuid, sizeof(EFI_GUID)) == 0) {
        break;
      }
    }
  }
  if (Index == mSetupVariableNumber){
    return EFI_NOT_FOUND;
  } else {
    *SetupVariableOffset = Index;
  }
  
  VariableIndex = Index;
  for ( Index = 1; Index <= mSetupItemNumber; Index++){
    if (mSetupItem[Index].VariableIndex == VariableIndex && mSetupItem[Index].ItemOffset == Offset){
      *SetupItemOffset = Index;
      return EFI_SUCCESS;
    }
  }
  return EFI_NOT_FOUND;
}

UINT8 *
FindVariableData (
  IN  VARIABLE_STORE_HEADER  *VariableStorage,
  IN  EFI_GUID               *VarGuid,
  IN  UINT32                 VarAttribute,
  IN  CHAR16                 *VarName
  )
{
  VARIABLE_HEADER *VariableHeader;
  VARIABLE_HEADER *VariableEnd;
  UINT8           *VariableData;

  VariableEnd    = (VARIABLE_HEADER *) ((UINT8 *) VariableStorage + VariableStorage->Size);
  VariableHeader = (VARIABLE_HEADER *) (VariableStorage + 1);
  VariableHeader = (VARIABLE_HEADER *) HEADER_ALIGN (VariableHeader);
  while (VariableHeader < VariableEnd) {
    if (CompareGuid (&VariableHeader->VendorGuid, VarGuid) &&
        VariableHeader->Attributes == VarAttribute &&
        StrCmp (VarName, (CHAR16 *) (VariableHeader + 1)) == 0) {
      VariableData = (UINT8 *)VariableHeader + sizeof (VARIABLE_HEADER) + VariableHeader->NameSize;
      return VariableData;
    }
    VariableHeader = (VARIABLE_HEADER *) ((UINT8 *) VariableHeader + sizeof (VARIABLE_HEADER) + VariableHeader->NameSize + VariableHeader->DataSize);
    VariableHeader = (VARIABLE_HEADER *) HEADER_ALIGN (VariableHeader);
  }

  return NULL;
}


EFI_STATUS
EFIAPI
UpdateMinValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  )
{
  return EFI_UNSUPPORTED;
  
}

EFI_STATUS
EFIAPI
UpdateMaxValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
UpdateDefaultValue(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT64                       Value
  )
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINT32                        SetupVariableOffset = 0;
  UINT32                        SetupItemOffset = 0;
  VARIABLE_STORE_HEADER         *gVariableStorage = NULL;
  UINT8                         *VariableHeaderData = NULL;

  if (mSetupItem == NULL){
    LocatSetupItemsDb();
  }
  Status = FindSetupItemOffset(VariableName, VendorGuid, Offset, &SetupVariableOffset, &SetupItemOffset);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "Offset %d to variable:%s not found\n",Offset,VariableName));
    return Status;
  }

  gVariableStorage = (VARIABLE_STORE_HEADER *)HobDataAddress;
  VariableHeaderData = FindVariableData(gVariableStorage, 
                          VendorGuid, 
                          mSetupVariableTable[SetupVariableOffset].Attribute, 
                          VariableName
                        );
  if (VariableHeaderData == NULL){
    return EFI_INVALID_PARAMETER;
  }
  CopyMem(VariableHeaderData + mSetupItem[SetupItemOffset].ItemOffset, &Value, mSetupItem[SetupItemOffset].ItemLength);
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
UpdateAttribute(
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINT16                       Attribute
  )
{
  return EFI_UNSUPPORTED;
}



EFI_STATUS
EFIAPI
UpdateOptionValue (
  IN CHAR16                       *VariableName,
  IN EFI_GUID                     *VendorGuid,
  IN UINT16                       Offset,
  IN UINTN                        Count,
  IN UINT64                       *OptionValueArray
  )
{
  return EFI_UNSUPPORTED;
}

SETUP_ITEM_UPDATE_PROTOCOL gSetupItemUpdateProtocol = {
  UpdateMinValue,
  UpdateMaxValue,
  UpdateDefaultValue,
  UpdateAttribute,
  UpdateOptionValue
};


EFI_STATUS
AddDefaultVariableTable(
  EFI_SMBIOS_PROTOCOL                  *SmbiosProtocol,
  EFI_HANDLE                           ImageHandle
)
{
  EFI_STATUS                    Status;
  VARIABLE_STORE_HEADER         *gVariableStorage;
  EFI_PEI_HOB_POINTERS          GuidHob;
  EFI_SMBIOS_HANDLE             SmbiosHandle;
  SMBIOS_DEFAULT_VARIABLE_HOB_TABLE         DefaultVariableDataTable;

  DEBUG ((DEBUG_INFO, "DefaultVariableDataTable add begin\n"));
  DefaultVariableDataTable.Hdr.Type = SMBIOS_DEFAULT_VARIABLE_HOB_DATA_TYPE;
  DefaultVariableDataTable.Hdr.Length  = sizeof(SMBIOS_DEFAULT_VARIABLE_HOB_TABLE);
  DefaultVariableDataTable.Hdr.Handle  = 0;
  GuidHob.Raw = GetFirstGuidHob (&gEfiVariableGuid);
  if (GuidHob.Raw == NULL) {
    GuidHob.Raw = GetFirstGuidHob (&gVariableDefaultGuid);
  }
  if (GuidHob.Raw == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  gVariableStorage = GET_GUID_HOB_DATA (GuidHob.Guid);

  Status  = gBS->AllocatePages (
                AllocateAnyPages,
                EfiReservedMemoryType,
                EFI_SIZE_TO_PAGES (gVariableStorage->Size),
                &HobDataAddress
                );
  
  ASSERT_EFI_ERROR (Status);
  CopyMem((VOID *)HobDataAddress, gVariableStorage, gVariableStorage->Size);

  DefaultVariableDataTable.Signature   = SMBIOS_DEFAULT_VARIABLE_HOB_DATA_TABLE_SIGNATURE;
  DefaultVariableDataTable.HobDataAddress = (UINT64)HobDataAddress;
  DefaultVariableDataTable.HobDataLength = (UINT64)(gVariableStorage->Size);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = SmbiosProtocol->Add (SmbiosProtocol, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) &DefaultVariableDataTable);
  DEBUG ((DEBUG_INFO, "DefaultVariableDataTable result:%llx;DefaultVariableHobDataAddress:%p\n", Status,DefaultVariableDataTable.HobDataAddress));
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gSetupItemUpdateProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gSetupItemUpdateProtocol
                  );

  return EFI_SUCCESS;
}