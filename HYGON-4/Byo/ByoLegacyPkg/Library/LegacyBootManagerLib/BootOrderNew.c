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
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Guid/LegacyDevOrder.h>
#include <Guid/GlobalVariable.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/PciIo.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ByoBootManagerLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/PerformanceLib.h>
#include <Library/HobLib.h>
#include <IndustryStandard/Atapi.h>
#include <Guid/MemoryTypeInformation.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Library/ByoCommLib.h>
#include <Library/LegacyBootOptionalDataLib.h>
#include <Protocol/EnumLegacyBootOptionsHook.h>
#include <ByoBootGroup.h>
#include <SysMiscCfg.h>
#include <Protocol/BdsBootManagerProtocol.h>
#include <Protocol/ByoPlatformBootManager.h>
#include <Protocol/UefiBbsRelationInfo.h>
#include <Protocol/ByoLegacyOptionCheckProtocol.h>
#include <Library/UefiLib.h>
#include <ByoCustomizedSortPolicy.h>

OPTION_SORT_POLICY                *mSortPolicy = NULL;

BYO_PLATFORM_BOOT_MANAGER_PROTOCOL    *mByoPlatformBootManagerProtocol;
BYO_LEGACY_OPTION_CHECK_PROTOCOL  *mLegacyOptionCheck = NULL;

VOID
PrintBbsTable (
  IN BBS_TABLE  *LocalBbsTable,
  IN UINT16     BbsCount
  );

BOOLEAN
ValidBbsEntry (
  IN BBS_TABLE   *BbsEntry
  );

UINT16
GetDevcieTypeInBBS (
  IN CONST BBS_TABLE *BbsEntry
  );

EFI_STATUS
CreateLegacyBootOption (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  IN BBS_TABLE                         *BbsEntry,
  IN UINT16                            BbsIndex
  );

UINT8 gStorageTypeArray[] = {MSG_NVME_NAMESPACE_DP, MSG_SATA_DP, MSG_ATAPI_DP, 0xFF};

/*
Nvme:
class:	  PCI_CLASS_MASS_STORAGE:1 
subclass:  PCI_CLASS_MASS_STORAGE_NVM:8 
  
HDD:
class:	  PCI_CLASS_MASS_STORAGE:1
subclass:  PCI_CLASS_MASS_STORAGE_IDE:0x01
               PCI_CLASS_MASS_STORAGE_SATADPA:0x06
*/

UINT8
GetBbsStorageType(UINT8 BbsClass, UINT8 BbsSubClass)
{
  UINT8 Type;
  
  if(BbsClass == 0x01){
    switch(BbsSubClass){
      case 0x08:
        Type = MSG_NVME_NAMESPACE_DP; 
        break;
        
      case 0x01:
        Type = MSG_ATAPI_DP;
        break;
        
      case 0x06:
        Type = MSG_SATA_DP;
        break;
        
      default:
        Type = 0xFF;
        break;
    }
  }else{
    Type = 0xFF;
  }
  
  return Type;
}


EFI_BOOT_MANAGER_LOAD_OPTION *
EnumerateLegacyBootOptions2 (
  UINTN                         *BootOptionCount,
  UINT8                         *LegacyGroupOrder, 
  UINTN                         GroupOrderSize
  )
{
  EFI_STATUS                    Status;
  UINT16                        HddCount;
  UINT16                        BbsCount;
  HDD_INFO                      *HddInfo;
  BBS_TABLE                     *BbsTable;
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  UINT16                        Index;
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOptions = NULL;
  UINT16                        DevCount = 0;
  UINT16                        *IndexBuffer = NULL;
  ENUM_LEGACY_BOOT_OPTIONS_HOOK_PARAMETER   Param;
  UINTN                                     TypeIndex;
  UINTN                                     GroupIndex;
  UINT8                                     GroupType;
  BOOLEAN                                   *RecArray = NULL;


  ASSERT (BootOptionCount != NULL);
  *BootOptionCount = 0;
  DEBUG((EFI_D_INFO, "%a(G:%X,L:%X)\n", __FUNCTION__, LegacyGroupOrder, GroupOrderSize));
  
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }

  Status = LegacyBios->GetBbsInfo (
                         LegacyBios,
                         &HddCount,
                         &HddInfo,
                         &BbsCount,
                         &BbsTable
                         );
  if (EFI_ERROR (Status)) {
    goto ProcExit;
  }

  DEBUG((EFI_D_INFO, "BbsCount:%d\n", BbsCount));

  PrintBbsTable(BbsTable, BbsCount);

  IndexBuffer = AllocateZeroPool(BbsCount * sizeof(UINT16));
  if(IndexBuffer == NULL){
    goto ProcExit;
  }

  RecArray = AllocateZeroPool(BbsCount * sizeof(BOOLEAN));
  if(RecArray == NULL){
    goto ProcExit;
  }

  DumpMem8(LegacyGroupOrder, GroupOrderSize);

  DevCount = 0;
  for(GroupIndex=0; GroupIndex<GroupOrderSize; GroupIndex++){
    if(LegacyGroupOrder[GroupIndex] == BM_MENU_TYPE_LEGACY_HDD){
      for (TypeIndex = 0; TypeIndex < ARRAY_SIZE(gStorageTypeArray); TypeIndex++){
        for (Index = 0; Index < BbsCount; Index++) {

          if(RecArray[Index]){
            continue;
          }
          
          if (!ValidBbsEntry(&BbsTable[Index])) {
            RecArray[Index] = 1;
            continue;
          }
          GroupType = GetBbsGroupType(BbsTable[Index].Class, (UINT8)GetDevcieTypeInBBS(&BbsTable[Index]));
          if(LegacyGroupOrder[GroupIndex] != GroupType){
            continue;
          }
          if(gStorageTypeArray[TypeIndex] != GetBbsStorageType(BbsTable[Index].Class, BbsTable[Index].SubClass)){
            continue;
          }
          IndexBuffer[DevCount++] = Index;
          RecArray[Index] = 1;
        }
      }
    } else {
      for (Index = 0; Index < BbsCount; Index++) {

        if(RecArray[Index]){
          continue;
        }
        
        if (!ValidBbsEntry(&BbsTable[Index])) {
          RecArray[Index] = 1;
          continue;
        }
        GroupType = GetBbsGroupType(BbsTable[Index].Class, (UINT8)GetDevcieTypeInBBS(&BbsTable[Index]));
        if(LegacyGroupOrder[GroupIndex] != GroupType){
          continue;
        }
        IndexBuffer[DevCount++] = Index;
        RecArray[Index] = 1;
      }
    }
  }

  DumpMem8(IndexBuffer, sizeof(UINT16)*DevCount);

  Param.Sign = ENUM_LEGACY_BOOT_OPTIONS_HOOK_SIGN;
  Param.BbsTable = BbsTable;
  Param.BbsCount = BbsCount;
  Param.BbsIndexList  = IndexBuffer;
  Param.BbsIndexCount = &DevCount;
  InvokeHookProtocol2(gBS, &gEnumLegacyBootOptionsHookGuid, &Param);

  DEBUG((EFI_D_INFO, "find legacy dev count:%X\n", DevCount));

  if(DevCount != 0){
    BootOptions = AllocatePool(sizeof(EFI_BOOT_MANAGER_LOAD_OPTION) * DevCount);
    ASSERT (BootOptions != NULL);
    for(Index=0;Index<DevCount;Index++){
      DEBUG((EFI_D_INFO, "I:%X\n", IndexBuffer[Index]));
      Status = CreateLegacyBootOption(&BootOptions[Index], &BbsTable[IndexBuffer[Index]], IndexBuffer[Index]);
      ASSERT_EFI_ERROR (Status);
    }
  }

ProcExit:
  if(IndexBuffer != NULL){FreePool(IndexBuffer);}
  if(RecArray != NULL){FreePool(RecArray);}
  *BootOptionCount = DevCount;
  DEBUG((EFI_D_INFO, "Exit P:%X,L:%d\n", BootOptions, DevCount));
  return BootOptions;
}




UINT8 gDefaultLegacyBootGroupOrder[] = {
  BM_MENU_TYPE_LEGACY_HDD, 
  BM_MENU_TYPE_LEGACY_ODD, 
  BM_MENU_TYPE_LEGACY_USB_DISK,
  BM_MENU_TYPE_LEGACY_USB_ODD,
  BM_MENU_TYPE_LEGACY_PXE  
  };

UINT16
FuzzyMatch (
  EFI_BOOT_MANAGER_LOAD_OPTION  *BootOption,
  BBS_TABLE                     *BbsTable,
  UINT16                         BbsCount,
  BOOLEAN                        *BbsIndexUsed
  );

INTN
LegacyBootManagerFindBootOption (
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION *Key,
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION *Array,
  IN UINTN                              Count
  );


UINT32
GetLegacyHddTypeIndex (
  EFI_BOOT_MANAGER_LOAD_OPTION *Option
  )
{
  UINT8  DevType;
  UINT32 Index = INVALID_PRIORITY;
  UINT16 *Desc;
  UINT8  PciClass;
  UINT8  PciSubClass;
  EFI_STATUS                        Status;
  BBS_TABLE                         *BbsTable;
  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios;
  HDD_INFO                          *HddInfo;
  UINT16                            HddCount;
  UINT16                            BbsIndex;
  UINT16                            BbsCount;
  UINT32                            Bus = 0;
  UINT32                            Device, Function;
  CHAR16                            *SasCnString = NULL;
  CHAR16                            *LunString = NULL;
  UEFI_BBS_RELATION_INFO_PROTOCOL	  *UefiBbsInfo;
  UefiBbsHddRL                      *UefiBbsHddTable;
  UINTN                             HddIndex;
  UINT16                            AtaIndex = 0xFFFF;
  UINT16                            PlatSataHostIndex = 0;
  UINT8                             HddGroupType;
  UINT32                            Count;

  
//DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
  
  PciClass = LegacyBootOptionalDataGetPciClass(Option->OptionalData, &PciSubClass);
  DevType = GetBbsStorageType(PciClass, PciSubClass);

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (!EFI_ERROR(Status)) {
    Status = LegacyBios->GetBbsInfo (
                         LegacyBios,
                         &HddCount,
                         &HddInfo,
                         &BbsCount,
                         &BbsTable
                         );
    if (!EFI_ERROR(Status)) {
      BbsIndex = LegacyBootOptionalDataGetBbsIndex(Option->OptionalData);
      Bus = BbsTable[BbsIndex].Bus;
      Device = BbsTable[BbsIndex].Device;
      Function = BbsTable[BbsIndex].Function;
    }
  }
  
  Status = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &UefiBbsInfo);
  if (!EFI_ERROR(Status) && (UefiBbsInfo != NULL)) {
    if ((DevType == MSG_SATA_DP) || (DevType == MSG_ATAPI_DP)) {
      BbsIndex = LegacyBootOptionalDataGetBbsIndex(Option->OptionalData);
      UefiBbsHddTable = UefiBbsInfo->UefiBbsHddTable;
      for (HddIndex = 0; HddIndex < UefiBbsInfo->UefiSataHddCount; HddIndex++) {
        if (UefiBbsHddTable[HddIndex].BbsIndex == BbsIndex) {
          AtaIndex = (UINT16)UefiBbsHddTable[HddIndex].HddPortIndex;
          PlatSataHostIndex = (UINT16)UefiBbsHddTable[HddIndex].SataHostIndex;
          if ((AtaIndex != 0xFFFF) && (PlatSataHostIndex != 0x8000)) {
            Index = SATA_HDD_TYPE * MASK_OF_PRIORITY + ((UINT8)PlatSataHostIndex << 8) + (UINT8)AtaIndex;
          } else {
            Index = SATA_HDD_TYPE * MASK_OF_PRIORITY + (UINT8)AtaIndex;
          }
          goto RETURN_DEVICE_PRIORITY;
        }
      }
    }
  }

  Desc = Option->Description;
  switch(DevType){
    case MSG_NVME_NAMESPACE_DP:
      Index = NVME_SSD_TYPE * MASK_OF_PRIORITY + ((Bus & 0xFF) << 16) + ((Device & 0xFF) << 8) + (Function & 0xFF);
      break;
      
    case MSG_SATA_DP:
    case MSG_ATAPI_DP:
	    if(StrnCmp(Desc, L"SATA 0", 0x06) == 0){
        Index = SATA_HDD_TYPE * MASK_OF_PRIORITY + 0;
      }else if(StrnCmp(Desc, L"SATA 1", 0x06) == 0){
        Index = SATA_HDD_TYPE * MASK_OF_PRIORITY + 1;
      }else if(StrnCmp(Desc, L"SATA 2", 0x06) == 0){
        Index = SATA_HDD_TYPE * MASK_OF_PRIORITY + 2;
      }else if(StrnCmp(Desc, L"SATA 3", 0x06) == 0){
        Index = SATA_HDD_TYPE * MASK_OF_PRIORITY + 3;
      } else {
        Index = SATA_HDD_TYPE * MASK_OF_PRIORITY + 0xF;
      }
      break;
      
    default:
      Index = INVALID_PRIORITY;
      break;
  }

  //
  // Assign Priority to SAS and RAID options
  //
  if (Index == INVALID_PRIORITY) {
    if (StrStr(Desc, L"SAS: ") != NULL) {
      if (StrStr(Desc, L"CN") != NULL) {
        SasCnString = StrStr(Desc, L"CN"); // CN0:1:0
        DEBUG((EFI_D_INFO, "%a line=%d SasCnString=%s \n", __FUNCTION__, __LINE__, SasCnString));
        Index = SCSI_HDD_TYPE * MASK_OF_PRIORITY + ((Bus & 0xFF) << 12) + (((SasCnString[2] - '0')&0xF)<<8) + (((SasCnString[4] - '0')&0xF)<<4) + ((SasCnString[6] - '0')&0xF);
      } else if ((StrStr(Desc, L" Lun") != NULL) && (StrStr(Desc, L" RAID") != NULL)) { // SAS RAID
        LunString = StrStr(Desc, L"Lun");
        Index = SCSI_HDD_TYPE * MASK_OF_PRIORITY + ((Bus & 0xFF) << 12) + ((LunString[3] - '0')&0xF);
      } else {
        Index = SCSI_HDD_TYPE * MASK_OF_PRIORITY + ((Bus & 0xFF) << 12);
      }
    } else if ((StrStr(Desc, L" Lun") != NULL) && (StrStr(Desc, L" RAID") != NULL)) { // SCSI RAID
      LunString = StrStr(Desc, L"Lun");
      DEBUG((EFI_D_INFO, "%a line=%d LunString=%s \n", __FUNCTION__, __LINE__, LunString));
      Index = RAID_HDD_TYPE * MASK_OF_PRIORITY + ((Bus & 0xFF) << 4) + ((LunString[3] - '0')&0xF);
    } else if (StrStr(Desc, L" RAID") != NULL) {
      Index = RAID_HDD_TYPE * MASK_OF_PRIORITY + ((Bus & 0xFF) << 4);
    }
  }

RETURN_DEVICE_PRIORITY:
  if ((mSortPolicy != NULL) && (mSortPolicy->HddGroupOrder != NULL)) {
    HddGroupType = (UINT8)(Index >> 24);
    for (Count = 0; Count < COUNT_OF_HDD_GROUP_TYPE; Count++) {
      if (HddGroupType == mSortPolicy->HddGroupOrder[Count]) {
        break;
      }
    }
    if (Count < COUNT_OF_HDD_GROUP_TYPE) {
      Index = (Count + 1) * MASK_OF_PRIORITY + (Index & 0xFFFFFF);
    } else {
      Index = INVALID_PRIORITY;
    }
  }

  return Index;
}

VOID
UpdateNewOrderData (
    UINT16 Data16,
    UINTN  Index,
    UINT16 *Array,
    UINTN  CurArrayCount
  )
{
  UINTN   i, j;

  DumpMem8(Array, CurArrayCount * sizeof(UINT16));

  i = CurArrayCount - 1;
  j = i - Index + 1;
  while(j--){
    Array[i+1] = Array[i];
    DEBUG((EFI_D_INFO, "%d -> %d\n", i, i+1));
    i--;
  }
  DEBUG((EFI_D_INFO, "i:%d\n", i));
  Array[Index] = Data16;

  DumpMem8(Array, (CurArrayCount+1) * sizeof(UINT16));  
}

UINTN
GetBootXXXXIndex(  
  EFI_BOOT_MANAGER_LOAD_OPTION  *Option,
  UINTN                         OptionCount,
  UINT16                        XXXX
  )
{
  UINTN  Index;

  for(Index=0;Index<OptionCount;Index++){
    if(Option[Index].OptionNumber == XXXX){
      return Index;
    }
  }

  ASSERT(FALSE);
  return 0;
}

UINTN
SortGetLegacyDefaultOrder (
  UINT16                        *Order,
  UINT8                         *GroupOrder,
  UINTN                         GroupOrderCount,
  EFI_BOOT_MANAGER_LOAD_OPTION  *Options,
  UINTN                         OptionsCount
  )
{
  UINTN                        Index, GroupIndex, i, j;
  UINTN                        OrderCount = 0;
  UINTN                        GroupStart;
  BOOLEAN                      Updated;

  for (Index = 0; Index < OptionsCount; Index++) {
    Options[Index].ItemOrder = GetLegacyHddTypeIndex(&Options[Index]);
  }

  for (GroupIndex = 0; GroupIndex < GroupOrderCount; GroupIndex++) {
    GroupStart = OrderCount;
    for (Index = 0; Index < OptionsCount; Index++) {
      if (Options[Index].GroupType != GroupOrder[GroupIndex]) {
        continue;
      }
      if (Options[Index].GroupType != BM_MENU_TYPE_LEGACY_HDD) {
        Order[OrderCount] = (UINT16)Options[Index].OptionNumber;
        OrderCount++;
      } else {
        Updated = FALSE;
        if (OrderCount - GroupStart) {
          for (i = GroupStart; i < OrderCount; i++) {
            j = GetBootXXXXIndex(Options, OptionsCount, Order[i]);
            if((Options[Index].ItemOrder) <= Options[j].ItemOrder){
              UpdateNewOrderData(
                (UINT16)Options[Index].OptionNumber,
                i,
                Order,
                OrderCount
                );
              OrderCount++;
              Updated = TRUE;
              break;
            }
          }
        } 
        if(!Updated){
          Order[OrderCount] = (UINT16)Options[Index].OptionNumber;
          OrderCount++;
        }
      }
    }
  }

  return OrderCount;
}

EFI_STATUS
GetFreeOptionNumber2 (
  IN EFI_BOOT_MANAGER_LOAD_OPTION  *NvBootOption,
  IN UINTN                         NvBootOptionCount,
  OUT UINT16                       *FreeOptionNumber
  )
{
  
  UINTN         OptionNumber;
  UINTN         Index;
  BOOLEAN       Same;

  if (FreeOptionNumber == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  for (OptionNumber = 0; OptionNumber < 0x10000; OptionNumber++) {
    Same = FALSE;
    for (Index = 0; Index < NvBootOptionCount; Index++) {
      if (OptionNumber == NvBootOption[Index].OptionNumber) {
        Same = TRUE;
        break;
      }
    }
    if(!Same){
      break;

    }
  }

  DEBUG((EFI_D_INFO, "%a : %X\n", __FUNCTION__, OptionNumber));

  if (OptionNumber == 0x10000) {
    return EFI_OUT_OF_RESOURCES;
  } else {
    *FreeOptionNumber = (UINT16)OptionNumber;
    return EFI_SUCCESS;
  }
}

EFI_STATUS
BmAddLoadOption (
  IN EFI_BOOT_MANAGER_LOAD_OPTION  *Option,
  IN EFI_BOOT_MANAGER_LOAD_OPTION  **NvBootOption,
  IN UINTN                         *NvBootOptionCount
  )
{
  EFI_STATUS                      Status;
  UINT16                          OptionNumber;
  EFI_BOOT_MANAGER_LOAD_OPTION    *NewOption;
  UINTN                           Count;
  

  if (Option == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (Option->OptionNumber == LoadOptionNumberUnassigned) {
    Status = GetFreeOptionNumber2 (
               *NvBootOption,
               *NvBootOptionCount,
               &OptionNumber
               );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    Option->OptionNumber = OptionNumber;
  }

  if (Option->OptionNumber >= LoadOptionNumberMax) {
    return EFI_INVALID_PARAMETER;
  }

  Count = *NvBootOptionCount;
  NewOption = AllocatePool((Count+1) * sizeof(EFI_BOOT_MANAGER_LOAD_OPTION));
  ASSERT(NewOption != NULL);

  if(Count){
    CopyMem(NewOption, *NvBootOption, sizeof(EFI_BOOT_MANAGER_LOAD_OPTION) * Count);
  }
  ByoCopyBootOption(NewOption + Count, Option);
  NewOption[Count].New = TRUE;

  if(*NvBootOption != NULL){FreePool(*NvBootOption);}
  *NvBootOption = NewOption;
  *NvBootOptionCount = Count + 1;
  
  return EFI_SUCCESS;
}

VOID
EFIAPI
LegacyBmRefresh2 (
    VOID
  )
{
  EFI_STATUS                        Status;
  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios;
  EFI_BOOT_MANAGER_LOAD_OPTION      *BootOption;
  UINTN                             BootOptionCount;
  EFI_BOOT_MANAGER_LOAD_OPTION      *NvBootOption;
  UINTN                             NvBootOptionCount;  
  UINT16                            HddCount;
  UINT16                            BbsCount;
  HDD_INFO                          *HddInfo;
  BBS_TABLE                         *BbsTable;
  UINT16                            BbsIndex;
  UINT16                            OldBbsIndex;
  BOOLEAN                           *BbsIndexUsed = NULL;
  UINTN                             Index;
  UINT8                             *LegacyGroupOrder = NULL;
  UINTN                             GroupOrderSize;
  UINTN                             GroupIndex;
  UINTN                             GroupStart;
  UINT16                            *NewOrder = NULL;
  UINT16                            NewIndex;
  BOOLEAN                           Updated;
  UINTN                             i, j;
  CHAR16                            OptionName[sizeof("Boot####")];  
  EFI_BDS_BOOT_MANAGER_PROTOCOL     *BdsBootMgr;
  EFI_BOOT_MANAGER_LOAD_OPTION               *UpdatedBootOptions;
  UINTN                                      UpdatedBootOptionCount;
  UINT32                            MaxHddOrder = 0;
  UINTN                             OptionIndex;
  UINTN                             OptionNumber;
  UINTN                             Size;
  BOOLEAN                           InsertNewByPriority = FALSE;
  UINT16                            *LegacyDefaultOrder = NULL;
  UINTN                             LegacyDefaultCount = 0;
  INTN                              NvIndex;
  UINT32                            NvAtrributes;
  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID**)&LegacyBios);
  if (EFI_ERROR (Status) || PcdGet8(PcdBiosBootModeType) == BIOS_BOOT_UEFI_OS) {
    BootOption = EfiBootManagerGetLoadOptions (&BootOptionCount, LoadOptionTypeBoot);
    for (Index = 0; Index < BootOptionCount; Index++) {
      if ((DevicePathType (BootOption[Index].FilePath) == BBS_DEVICE_PATH) &&
          (DevicePathSubType (BootOption[Index].FilePath) == BBS_BBS_DP)) {
        EfiBootManagerDeleteLoadOptionVariable (BootOption[Index].OptionNumber, BootOption[Index].OptionType);
      }
    }
    return;
  }
  PERF_START (NULL, "LegacyBootOptionEnum", "BDS", 0);


  Status = LegacyBios->GetBbsInfo (
                         LegacyBios,
                         &HddCount,
                         &HddInfo,
                         &BbsCount,
                         &BbsTable
                         );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "(%d)GetBbsInfo:%r\n", __LINE__, Status));
    return;
  }

  //
  // Get customized sort policy
  //
  Status = GetVariable2 (
                  BYO_CUSTOMIZED_SORT_POLICY_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  (VOID **)&mSortPolicy,
                  &Size
                  );
  if (!EFI_ERROR(Status) && ((mSortPolicy->CustomizedSortPolicy & INSERT_NEW_OPTION_BY_PRIORITY) != 0)) {
    InsertNewByPriority = TRUE;
  }

  LegacyGroupOrder = EfiBootManagerGetVariableAndSize (
                       BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                       &gByoGlobalVariableGuid,
                       &GroupOrderSize
                       );
  if(LegacyGroupOrder == NULL){
    GroupOrderSize = sizeof(gDefaultLegacyBootGroupOrder);
    LegacyGroupOrder = AllocateCopyPool(GroupOrderSize, gDefaultLegacyBootGroupOrder);
    Status = gRT->SetVariable (
                    BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    GroupOrderSize,
                    LegacyGroupOrder
                    );  
    ASSERT(!EFI_ERROR(Status));
  }

  NvBootOption = EfiBootManagerGetLoadOptions(&NvBootOptionCount, LoadOptionTypeBoot);
  BootOption = EnumerateLegacyBootOptions2(&BootOptionCount, LegacyGroupOrder, GroupOrderSize);
  if (mByoPlatformBootManagerProtocol != NULL && mByoPlatformBootManagerProtocol->RefreshAllBootOptions != NULL) {
    //
    // If found, call platform specific refresh to all auto enumerated and NV boot options.
    //
    Status = mByoPlatformBootManagerProtocol->RefreshAllBootOptions ((CONST EFI_BOOT_MANAGER_LOAD_OPTION *)BootOption,
                                                         (CONST UINTN)BootOptionCount,
                                                         &UpdatedBootOptions,
                                                         &UpdatedBootOptionCount);
    if (!EFI_ERROR (Status)) {
      ByoEfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);
      BootOption = UpdatedBootOptions;
      BootOptionCount = UpdatedBootOptionCount;
    }
  }

  for (Index = 0; Index < BootOptionCount; Index++) {
    DEBUG((EFI_D_INFO, "[%04X] %s\n", BootOption[Index].OptionNumber, BootOption[Index].Description));
    BootOption[Index].ItemOrder = GetLegacyHddTypeIndex(&BootOption[Index]);
  }  
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    DEBUG((EFI_D_INFO, "NV[%04X] %s\n", NvBootOption[Index].OptionNumber, NvBootOption[Index].Description));
  }

  BbsIndexUsed = AllocateZeroPool(BbsCount * sizeof(BOOLEAN));
  ASSERT (BbsIndexUsed != NULL);

  if (mLegacyOptionCheck == NULL) {
    gBS->LocateProtocol (&gByoLegacyOptionCheckProtocolGuid,
                                  NULL,
                                  (VOID **)&mLegacyOptionCheck);
  }

// mark invalid
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if((DevicePathType (NvBootOption[Index].FilePath) != BBS_DEVICE_PATH) || (DevicePathSubType(NvBootOption[Index].FilePath) != BBS_BBS_DP)) {
      DEBUG((EFI_D_INFO, "!NV[%d]:%X\n", Index, NvBootOption[Index].OptionNumber));
      continue;
    }

    DEBUG((EFI_D_INFO, "NV[%d]:%X\n", Index, NvBootOption[Index].OptionNumber)); 
    BbsIndex = INVALID_BBSINDEX_VALUE;
    if ((mLegacyOptionCheck != NULL) && (mLegacyOptionCheck->BbsInexFuzzyMatch != NULL)) {
        BbsIndex = mLegacyOptionCheck->BbsInexFuzzyMatch(&NvBootOption[Index],
                                                        BbsIndexUsed,
                                                        BbsTable,
                                                        BbsCount
                                                        );
    }
    if (BbsIndex == INVALID_BBSINDEX_VALUE) {
      BbsIndex = FuzzyMatch(&NvBootOption[Index], BbsTable, BbsCount, BbsIndexUsed);
    }

    OldBbsIndex = LegacyBootOptionalDataGetBbsIndex(NvBootOption[Index].OptionalData);
    if (BbsIndex == BbsCount) {
      DEBUG((EFI_D_INFO, "[LegacyBds]BbsEntry not found, delete BOOT%04X: %s\n", NvBootOption[Index].OptionNumber, NvBootOption[Index].Description));
      NvBootOption[Index].Deleted = TRUE;
    } else {
      DEBUG((EFI_D_INFO, "[BBS]Found BbsEntry:0x%X -> 0x%X\n", OldBbsIndex, BbsIndex));

      for (OptionIndex = 0; OptionIndex < BootOptionCount; OptionIndex++) {
        if (LegacyBootOptionalDataGetBbsIndex(BootOption[OptionIndex].OptionalData) == BbsIndex) {
          break;
        }
      }
      if (OptionIndex < BootOptionCount) {
        //
        // Check if it need to be deleted by the platform
        //
        if (BootOption[OptionIndex].OptionNumber == DELETE_FLAG) {
          DEBUG((EFI_D_INFO, "Delate NvOption by the platform:%s %a\n", NvBootOption[Index].Description, __FUNCTION__));
          NvBootOption[Index].Deleted = TRUE;
          continue;
        }

        //
        // Check and update the option
        //
        if (LegacyBootManagerFindBootOption(&BootOption[OptionIndex], &NvBootOption[Index], 1) == -1) {
          OptionNumber = NvBootOption[Index].OptionNumber;
          NvAtrributes = NvBootOption[Index].Attributes;
          EfiBootManagerFreeLoadOption(&NvBootOption[Index]);
          Status = EfiBootManagerInitializeLoadOption (
                     &NvBootOption[Index],
                     OptionNumber,
                     LoadOptionTypeBoot,
                     NvAtrributes,
                     BootOption[OptionIndex].Description,
                     BootOption[OptionIndex].FilePath,
                     BootOption[OptionIndex].OptionalData,
                     BootOption[OptionIndex].OptionalDataSize
                     );
          if (!EFI_ERROR(Status)) {
            EfiBootManagerLoadOptionToVariable(&NvBootOption[Index]);
            DEBUG((EFI_D_INFO, "Updated NvOption:%s %a\n", BootOption[OptionIndex].Description, __FUNCTION__));
          }
        }
      }
    }

    NvBootOption[Index].ItemOrder = GetLegacyHddTypeIndex(&NvBootOption[Index]);
  }

  if(NvBootOptionCount + BootOptionCount){
    NewOrder = AllocatePool((NvBootOptionCount + BootOptionCount) * sizeof(UINT16));
    ASSERT(NewOrder != NULL);
  }
  DEBUG((EFI_D_INFO, "NewOrder P:%X\n", NewOrder));

// copy UEFI
  NewIndex = 0;
  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if((DevicePathType (NvBootOption[Index].FilePath) != BBS_DEVICE_PATH) || (DevicePathSubType(NvBootOption[Index].FilePath) != BBS_BBS_DP)){
      DEBUG((EFI_D_INFO, "+UEFI [%d]:%d\n", NewIndex, NvBootOption[Index].OptionNumber));
      NewOrder[NewIndex++] = (UINT16)NvBootOption[Index].OptionNumber;
    }
  }

// copy LEGACY
  for(GroupIndex=0;GroupIndex<GroupOrderSize;GroupIndex++){

    GroupStart = NewIndex;

// copy old
    for (Index = 0; Index < NvBootOptionCount; Index++) {
      if((DevicePathType (NvBootOption[Index].FilePath) != BBS_DEVICE_PATH) ||
         (DevicePathSubType(NvBootOption[Index].FilePath) != BBS_BBS_DP) ||
          NvBootOption[Index].Deleted) {
        continue;
      }
      if(NvBootOption[Index].GroupType != LegacyGroupOrder[GroupIndex]){
        continue;
      }
      DEBUG((EFI_D_INFO, "+OLD LEGACY [%d]:%d\n", NewIndex, NvBootOption[Index].OptionNumber));
      NewOrder[NewIndex++] = (UINT16)NvBootOption[Index].OptionNumber;

      if (!InsertNewByPriority) {
        if (LegacyGroupOrder[GroupIndex] == BM_MENU_TYPE_LEGACY_HDD) { // note the max order of legacy HDD options
          NvBootOption[Index].ItemOrder = NewIndex - 1;
          MaxHddOrder = NewIndex;
        }
      }
    }

// add new
    for (Index = 0; Index < BootOptionCount; Index++) {
      if (BootOption[Index].OptionNumber == DELETE_FLAG) { // the platform wants to delete such options
        continue;
      }

      if(BootOption[Index].GroupType != LegacyGroupOrder[GroupIndex]){
        continue;
      }
      
      NvIndex = LegacyBootManagerFindBootOption(&BootOption[Index], NvBootOption, NvBootOptionCount);
      if (NvIndex== -1) {
        BmAddLoadOption(&BootOption[Index], &NvBootOption, &NvBootOptionCount);

        DEBUG((EFI_D_INFO, "GroupStart:%d, NewIndex:%d\n", GroupStart, NewIndex));

        if(LegacyGroupOrder[GroupIndex] == BM_MENU_TYPE_LEGACY_HDD){
          DEBUG((EFI_D_INFO, "NewItemOrder:%X\n", BootOption[Index].ItemOrder));

          Updated = FALSE;
          if(NewIndex - GroupStart){
            for(i=GroupStart;i<NewIndex;i++){
              j = GetBootXXXXIndex(NvBootOption, NvBootOptionCount, NewOrder[i]);
              DEBUG((EFI_D_INFO, "(L%d) %X %X %X %X\n", __LINE__, BootOption[Index].ItemOrder + MaxHddOrder, NvBootOption[j].ItemOrder, NewOrder[i], j));
              if((BootOption[Index].ItemOrder + MaxHddOrder) <= NvBootOption[j].ItemOrder){
                UpdateNewOrderData(
                  (UINT16)BootOption[Index].OptionNumber,
                  i,
                  NewOrder,
                  NewIndex
                  );
                NewIndex++;
                Updated = TRUE;
                break;
              }
            }
          } 

          if(!Updated){
            NewOrder[NewIndex] = (UINT16)BootOption[Index].OptionNumber;
            DEBUG((EFI_D_INFO, "LEGACY +[%d]:%X\n", NewIndex, NewOrder[NewIndex]));
            NewIndex++;
          }
          
        } else {
          NewOrder[NewIndex] = (UINT16)BootOption[Index].OptionNumber;
          DEBUG((EFI_D_INFO, "LEGACY +[%d]:%X\n", NewIndex, NewOrder[NewIndex]));
          NewIndex++;
        } 
      } else {
        BootOption[Index].OptionNumber = NvBootOption[(UINTN)NvIndex].OptionNumber;
      }
    }
  }

  DEBUG((EFI_D_INFO, "NewIndex:%d\n", NewIndex));

  for (Index = 0; Index < NvBootOptionCount; Index++) {
    if(NvBootOption[Index].Deleted){
      UnicodeSPrint(OptionName, sizeof(OptionName), L"Boot%04X", NvBootOption[Index].OptionNumber);
      Status = gRT->SetVariable (
                      OptionName,
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );   
      DEBUG((EFI_D_INFO, "Del %X: %r\n", NvBootOption[Index].OptionNumber, Status));
    } else if(NvBootOption[Index].New){
      Status = EfiBootManagerLoadOptionToVariable(&NvBootOption[Index]);
      NvBootOption[Index].New = FALSE;
      DEBUG((EFI_D_INFO, "add %X: %r\n", NvBootOption[Index].OptionNumber, Status));
    }
  }

  DumpMem8(NewOrder, NewIndex * sizeof(UINT16));
  Status = gRT->SetVariable (
                  L"BootOrder",
                  &gEfiGlobalVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                  NewIndex * sizeof(UINT16),
                  NewOrder
                  );

  ByoEfiBootManagerFreeLoadOptions (NvBootOption, NvBootOptionCount);
//ByoEfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);

  Status = gBS->LocateProtocol(&gEfiBootManagerProtocolGuid, NULL, (VOID **)&BdsBootMgr);
  if(!EFI_ERROR(Status)){
    if (BootOptionCount > 0) {
      LegacyDefaultOrder = (UINT16 *)AllocatePool(BootOptionCount * sizeof(UINT16));
      ASSERT(LegacyDefaultOrder != NULL);
      LegacyDefaultCount = SortGetLegacyDefaultOrder(LegacyDefaultOrder, LegacyGroupOrder, GroupOrderSize, BootOption, BootOptionCount);
    }
    if (LegacyDefaultCount > 0) {
      UpdatedBootOptions = (EFI_BOOT_MANAGER_LOAD_OPTION *)AllocateZeroPool (sizeof (EFI_BOOT_MANAGER_LOAD_OPTION) * BootOptionCount);
      ASSERT(UpdatedBootOptions != NULL);
      UpdatedBootOptionCount = 0;
      for (Index = 0; Index < LegacyDefaultCount; Index++) {
        for (i = 0; i < BootOptionCount; i++) {
          if (BootOption[i].OptionNumber == LegacyDefaultOrder[Index]) {
            DEBUG((EFI_D_INFO, "[%04X] %s\n", BootOption[i].OptionNumber, BootOption[i].Description));
            ByoCopyBootOption (UpdatedBootOptions + UpdatedBootOptionCount, BootOption + i);
            UpdatedBootOptionCount++;
          }
        }
      }
      ByoEfiBootManagerFreeLoadOptions (BootOption, BootOptionCount);
      BootOption = UpdatedBootOptions;
      BootOptionCount = UpdatedBootOptionCount;
    }

    BdsBootMgr->LegacyOptions = BootOption;
    BdsBootMgr->LegacyOptionsCount = BootOptionCount;
    if (LegacyDefaultOrder != NULL) {
      FreePool(LegacyDefaultOrder);
    }
  } 

  FreePool(LegacyGroupOrder);
  FreePool(BbsIndexUsed);
  if(NewOrder != NULL){FreePool(NewOrder);}

  PERF_END(NULL, "LegacyBootOptionEnum", "BDS", 0);
  DEBUG((EFI_D_INFO, "%a() Exit\n", __FUNCTION__));
}



