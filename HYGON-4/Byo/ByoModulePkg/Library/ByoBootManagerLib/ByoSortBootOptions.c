/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoSortBootOptions.c

Abstract:

Revision History:

**/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/ByoCommLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/ByoBootManagerLib.h>

#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/PciIo.h>
#include <Protocol/NetworkInterfaceIdentifier.h>
#include <Protocol/DiskInfo.h>

#include <Guid/FileInfo.h>

#include <ByoCustomizedSortPolicy.h>
#include <SysMiscCfg.h>

#define MASK_OF_BOOT_PRIORITY        0x1000000000000
#define INVALID_BOOT_OPTION_PRIORITY 0xFFFFFFFFFFFFF

#define MASK_OF_HDD_PRIORITY         0x10000000000
#define INVALID_HDD_PRIORITY         0xFFFFFFFFFFF

#define INCREMENT_OF_COUNT 10
#define MAX_VALUE_OF_COUNT 20

#define ITEM_ORDER_OF_SETUP 0x10000

typedef struct {
  UINTN        DevicePriority;
  UINTN        ItemOrder;
  UINTN        Function;
  //
  // 1) When sorting net options of same netcard by SlotNumber, NetcardId is SlotNumber;
  // 2) When sorting net options of same netcard by Pci BusNumber and Pci DeviceNumber,
  // NetcardId is combined by BusNumber and DeviceNumber:(Bus&0xFFFF)<<16 + (Device&0xFFFF)
  //
  UINT32       NetcardId;
  UINT8        GroupType;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  CHAR16       *Description;
  UINT8        NetIpGroupType;
  BOOLEAN      IsNew;
  BOOLEAN      IsPciNet;
  BOOLEAN      IsiSCSI;
  BOOLEAN      IsBdsCreate;
} OPTION_INFO;

OPTION_SORT_POLICY  *mSortPolicy = NULL;
BOOLEAN      IsLoadDefault = FALSE;

typedef struct {
  UINT8                     *DevicePath;
  UINTN                     Size;
  UINT8                     Value;
} NETCARD_ID_INFO;
NETCARD_ID_INFO          *mNetInfo = NULL;
UINTN                    mNetInfoCount = MAX_VALUE_OF_COUNT;
UINTN                    mNetInfoIndex = 0;

//
// The default sorting policy
//
UINT8 mDefaultUefiBootGroupOrder[] = {
  BM_MENU_TYPE_UEFI_HDD,
  BM_MENU_TYPE_UEFI_PXE,
  BM_MENU_TYPE_UEFI_ODD, 
  BM_MENU_TYPE_UEFI_USB_DISK,
  BM_MENU_TYPE_UEFI_USB_ODD,  
  BM_MENU_TYPE_UEFI_OTHERS
  };

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

//
// Support sorting boot options
//
STATIC OPTION_INFO  *mOptionInfo = NULL;
STATIC UINTN        mOptionInfoCount = 0;

UINT8               *mUefiGroupOrder = NULL;
UINTN               mGroupOrderSize = 0;

STATIC
EFI_DEVICE_PATH_PROTOCOL *
CheckHdShortDp (
  IN  EFI_DEVICE_PATH_PROTOCOL        *DevicePath
  )
{
  EFI_STATUS                          Status;
  EFI_DEVICE_PATH_PROTOCOL            *TempDp;
  UINTN                               HandleCount;
  EFI_HANDLE                          *Handles;
  UINTN                               Index;
  EFI_DEVICE_PATH_PROTOCOL            *FsDp = NULL;

  //
  // Check if it's HD short-form DevicePath
  //
  TempDp = DevicePath;
  if ((DevicePathType(TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType(TempDp) == MEDIA_HARDDRIVE_DP)) {
    TempDp = NextDevicePathNode(TempDp);
    while (!IsDevicePathEnd(TempDp)) {
      if ((DevicePathType(TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType(TempDp) == MEDIA_FILEPATH_DP)) {
        break;
      }
      TempDp = NextDevicePathNode(TempDp);
    }

    if (IsDevicePathEnd(TempDp)) {
      return NULL;
    }
  }

  //
  // Get full DevicePath of this HD short-form option
  //    
  Status = gBS->LocateHandleBuffer (
                   ByProtocol,
                   &gEfiSimpleFileSystemProtocolGuid,
                   NULL,
                   &HandleCount,
                   &Handles
                   );                  
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID**)&FsDp
                    );               
    if (EFI_ERROR(Status)) {
      continue;
    }
    
    TempDp = FsDp;
    while (!IsDevicePathEnd(TempDp)) {
      if (DevicePathType(TempDp) == MEDIA_DEVICE_PATH && DevicePathSubType(TempDp) == MEDIA_HARDDRIVE_DP) {
        break;
      }
      TempDp = NextDevicePathNode(TempDp);
    }
    if (IsDevicePathEnd(TempDp)) {
      continue;
    }

    if (CompareMem(DevicePath, TempDp, DevicePathNodeLength(DevicePath)) == 0) {        // node matched
      return DuplicateDevicePath(FsDp);
    }
  }

  return NULL;
}

/**
  Check if DevicePath is HD Short-Form,and get size of DevicePath:
  True:  return FullPath of DevicePath, and size is the size of the FullPath
minus the size of HD(xxxxx) node;
  False: return DevicePath,and size is size of DevicePath minus size of endnode.
**/
STATIC
EFI_DEVICE_PATH_PROTOCOL *
ExpandHdShortForm (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath,
  IN OUT UINTN                *Size             
  )
{
  UINTN                     TempSize1;
  UINTN                     TempSize2;
  EFI_DEVICE_PATH_PROTOCOL  *TempDp;

  TempDp = DevicePath;
  TempSize1 = GetDevicePathSize(TempDp) - END_DEVICE_PATH_LENGTH;
  TempSize2 = 0;

  if ((DevicePathType(TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType(TempDp) == MEDIA_HARDDRIVE_DP)) {
    TempDp = NextDevicePathNode(TempDp);
    while (!IsDevicePathEnd(TempDp)) {
      if ((DevicePathType(TempDp) == MEDIA_DEVICE_PATH) && (DevicePathSubType(TempDp) == MEDIA_FILEPATH_DP)) {
        TempSize2 = GetDevicePathSize(TempDp) - END_DEVICE_PATH_LENGTH;
        break;
      }
      TempDp = NextDevicePathNode(TempDp);
    }

    if (!IsDevicePathEnd(TempDp)) {
      TempDp = CheckHdShortDp (DevicePath); // Try to get FullPath of HD Short-Form
      if (TempDp != NULL) {
        *Size = GetDevicePathSize(TempDp) - END_DEVICE_PATH_LENGTH - (TempSize1 - TempSize2);
        return TempDp;        
      }
    }
  }

  *Size = GetDevicePathSize(DevicePath) - END_DEVICE_PATH_LENGTH;
  return DevicePath;
}

/**
  This function is used to check whether the two HDD DevicePath are uninstalled OS HDD DevicePath
  and installed OS HDD DevicePath pointing to the same HDD.For example, the following two DevicePaths are the same:
  PciRoot(0x0)/Pci(0x3,0x0)/Pci(0x0,0x0)/Sata(0x3,0xFFFF,0x0) and
  PciRoot(0x0)/Pci(0x3,0x0)/Pci(0x0,0x0)/Sata(0x3,0xFFFF,0x0)/HD(1,GPT,ABCC10C4-C111-4B4C-B8D0-D9E64AAFBEAA,0x800,0x32000)/\EFI\Microsoft\Boot\bootmgfw.efi

  @param DevicePath1    A pointer to the first device path data structure.
  @param DevicePath2    A pointer to the second device path data structure.

  @retval TRUE    They are same.
  @retval FALSE   They are not same.

**/
BOOLEAN
CheckAndCompareIgnoreFileNode (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath1,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath2
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *TempDp = NULL;
  BOOLEAN                   HasOs1 = FALSE;
  BOOLEAN                   HasOs2 = FALSE;
  UINTN                     DpSize1 = 0;
  UINTN                     DpSize2 = 0;

  if (IsDevicePathEnd(DevicePath1) || IsDevicePathEnd(DevicePath2)) {
    return FALSE;
  }

  DpSize1 = GetDevicePathSize(DevicePath1);
  DpSize2 = GetDevicePathSize(DevicePath2);

  TempDp = DevicePath1;
  while (!IsDevicePathEnd(TempDp)) {
    if ((TempDp->Type == MEDIA_DEVICE_PATH) && (TempDp->SubType == MEDIA_HARDDRIVE_DP)) {
      break;
    }
    TempDp = NextDevicePathNode(TempDp);
  }
  if (!IsDevicePathEnd(TempDp)) {
    HasOs1 = TRUE;
    DpSize1 = GetDevicePathSize(DevicePath1) - GetDevicePathSize(TempDp);
  }

  TempDp = DevicePath2;
  while (!IsDevicePathEnd(TempDp)) {
    if ((TempDp->Type == MEDIA_DEVICE_PATH) && (TempDp->SubType == MEDIA_HARDDRIVE_DP)) {
      break;
    }
    TempDp = NextDevicePathNode(TempDp);
  }
  if (!IsDevicePathEnd(TempDp)) {
    HasOs2 = TRUE;
    DpSize2 = GetDevicePathSize(DevicePath2) - GetDevicePathSize(TempDp);
  }

  if ((HasOs1 && HasOs2) || !(HasOs1 || HasOs2)) { // no uninstalled OS DevicePath or no installed OS DevicePath
    return FALSE;
  }

  DpSize1 = DpSize1 > DpSize2? DpSize2: DpSize1;
  if (CompareMem(DevicePath1, DevicePath2, DpSize1) == 0) {
    return TRUE;
  }
  return FALSE;
}

/**
  Compare two device paths to check if they are exactly same.

  @param DevicePath1    A pointer to the first device path data structure.
  @param DevicePath2    A pointer to the second device path data structure.

  @retval TRUE    They are same.
  @retval FALSE   They are not same.

**/
STATIC
BOOLEAN
CompareAllDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath1,
  IN EFI_DEVICE_PATH_PROTOCOL *DevicePath2
  )
{
  UINTN                     Size1;
  UINTN                     Size2;
  EFI_DEVICE_PATH_PROTOCOL  *ExpandDp1;
  EFI_DEVICE_PATH_PROTOCOL  *ExpandDp2;
  BOOLEAN                   IsMatched = FALSE;
  
  ExpandDp1 = ExpandHdShortForm(DevicePath1, &Size1);
  ExpandDp2 = ExpandHdShortForm(DevicePath2, &Size2);

  if (Size1 == Size2) {
    if (CompareMem (ExpandDp1, ExpandDp2, Size1) == 0) {
      IsMatched = TRUE;
    } 
  }

  if (!IsMatched && CheckAndCompareIgnoreFileNode(ExpandDp1, ExpandDp2)) {
    IsMatched = TRUE;
  }

  if (ExpandDp1 != NULL) {
    if (CompareMem(ExpandDp1, DevicePath1, GetDevicePathSize(DevicePath1)) != 0) {
      FreePool(ExpandDp1);
    }
  }

  if (ExpandDp2 != NULL) {
    if (CompareMem(ExpandDp2, DevicePath2, GetDevicePathSize(DevicePath2)) != 0) {
      FreePool(ExpandDp2);
    }
  }

  return IsMatched;
}

/**
  Returns the priority of BM_MENU_TYPE_UEFI_OTHERS
  The setup is always behind the shell.
  @param Option
**/
UINTN
ByoGetOptionPriority (
  CONST EFI_BOOT_MANAGER_LOAD_OPTION *Option
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  UINT8                     GroupType;
  OPTION_INFO               *CurrentOptionInfo = NULL;
  EFI_HANDLE                FvHandle;
  VOID                      *NameGuid;  
  BOOLEAN                   IsBdsCreate;
  EFI_BOOT_MANAGER_LOAD_OPTION *TempOption;

  TempOption = (EFI_BOOT_MANAGER_LOAD_OPTION*)Option;
  if ((mOptionInfo == NULL) || (mOptionInfoCount == 0) || (Option == NULL)) { // no network cards or hdd
    return INVALID_PRIORITY;
  }

  GroupType = (UINT8)GetEfiBootGroupType(Option->FilePath);
  if (!IsLoadDefault) {
    if (BmIsAutoCreateBootOption(TempOption)) {
      IsBdsCreate = TRUE;
    } else {
      IsBdsCreate = FALSE;
    }
  } else {
    IsBdsCreate = TRUE;
  }

  //
  // Find out OPTIONS_INFO of the current HDD, net option or legacy options
  //
  for (Index = 0; Index < mOptionInfoCount; Index++) {
    if (mOptionInfo[Index].GroupType != GroupType) {
      continue;
    }
    if ((DevicePathType(mOptionInfo[Index].DevicePath) == BBS_DEVICE_PATH)
      && (DevicePathType(Option->FilePath) == BBS_DEVICE_PATH)) {
      if (StrCmp(mOptionInfo[Index].Description, Option->Description) == 0) {
        CurrentOptionInfo = &mOptionInfo[Index];
        break;
      } else {
        continue;
      }
    }
    if (IsBdsCreate != mOptionInfo[Index].IsBdsCreate) {
      continue;
    }
    if (CompareAllDevicePath(Option->FilePath, mOptionInfo[Index].DevicePath)) {
      CurrentOptionInfo = &mOptionInfo[Index];
      break;      
    }
  }
  
  if (CurrentOptionInfo != NULL) {
    return CurrentOptionInfo->ItemOrder;
  }

  //
  // Return order of BM_MENU_TYPE_UEFI_OTHERS
  //
  if (GroupType == BM_MENU_TYPE_UEFI_OTHERS) {
    DevicePath = Option->FilePath;
    Status = gBS->LocateDevicePath (&gEfiFirmwareVolume2ProtocolGuid, &DevicePath, &FvHandle);
    if (!EFI_ERROR (Status)) {
      NameGuid = EfiGetNameGuidFromFwVolDevicePathNode ((CONST MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *) DevicePath);
      if (NameGuid != NULL) {
        if (CompareGuid (NameGuid, PcdGetPtr (PcdBootManagerMenuFile))) {
          return ITEM_ORDER_OF_SETUP;
        }
      }
    }
    return 0;
  }

  return INVALID_PRIORITY;
}

/**
  Returns the priority number.

  @param BootOption
**/
UINTN
ByoBootOptionPriority (
  CONST EFI_BOOT_MANAGER_LOAD_OPTION *BootOption
  )
{
  UINTN                         Index;
  UINTN                         OptionPriority;
  UINTN                         GroupPriority;
  UINTN                         MemberPriority;

  OptionPriority = INVALID_BOOT_OPTION_PRIORITY;
  
  for (Index = 0; Index < mGroupOrderSize; Index++) {
    if (GetEfiBootGroupType (BootOption->FilePath) == mUefiGroupOrder[Index]) {
      //
      // Set priority of Device Group
      //
      GroupPriority = Index * MASK_OF_BOOT_PRIORITY;

      //
      // Set priority of current option
      //
      MemberPriority = ByoGetOptionPriority(BootOption);
      if (MemberPriority == INVALID_PRIORITY) {
        MemberPriority = 0;
      }
      OptionPriority = GroupPriority + MemberPriority;
    }
  }

  return OptionPriority;
}

INTN
ByoCompareBootOption (
  CONST EFI_BOOT_MANAGER_LOAD_OPTION  *Left,
  CONST EFI_BOOT_MANAGER_LOAD_OPTION  *Right
  )
{
  return ByoBootOptionPriority (Left) - ByoBootOptionPriority (Right);
}

UINT32
GetNetcardId (
  UINTN          CurrentIndex,
  UINTN          Bus,
  UINTN          Device,
  UINTN          Segment
  )
{
  BOOLEAN                           SortBySlotNum;
  CHAR16                            *OnboardString = L"Onboard";
  CHAR16                            *SlotString = L"Slot ";
  CHAR16                            *ColonString = L":";
  CHAR16                            *String1;
  CHAR16                            *String2;
  CHAR16                            *SlotNumString;
  UINTN                             SlotNumSize;
  UINTN                             Size;
  UINTN                             Count;
  UINTN                             Index = 0;
  UINT32                            NetcardId = 0;
  UINT16                            Number;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  UINTN                             NetDeviceSize = 0;
  UINTN                             MaxSlotNumBits = 3; // SlotNumber cannot have more than three digits

  SortBySlotNum = (mSortPolicy->CustomizedSortPolicy & STACK_NET_OPTIONS_BY_SLOTNUMBER) != 0? TRUE: FALSE;
  if (SortBySlotNum) { // Description with SlotNumber like:UEFI Slot N:Port M - xxxxx PXE IPv4
    //
    // Set SlotNumber of Netcard as NetcardId
    //
    String1 = StrStr(mOptionInfo[CurrentIndex].Description, SlotString);
    if (String1 != NULL) { // String1 like:Slot N:Port M - xxxxx PXE IPv4
      String2 = StrStr(mOptionInfo[CurrentIndex].Description, ColonString);
      if (String2 != NULL) { // String2 like::Port M - xxxxx PXE IPv4
        //
        // size of L"N" from L"Slot N:Port M - xxxxx PXE IPv4"
        //
        Size = StrLen(SlotString) * sizeof(CHAR16);
        SlotNumSize = StrSize(String1) - StrSize(String2) + sizeof(CHAR16) - Size;
        SlotNumString = (CHAR16 *)AllocateZeroPool(SlotNumSize);
        ASSERT(SlotNumString != NULL);
        CopyMem(SlotNumString, String1 + StrLen(SlotString), SlotNumSize - sizeof(CHAR16));

        if (StrLen(SlotNumString) < MaxSlotNumBits) {
          MaxSlotNumBits = StrLen(SlotNumString);
        }
        for (Index = 0; Index < MaxSlotNumBits; Index++) {
          Number = (UINT8)(SlotNumString[Index] - L'0');
          for (Count = 0; Count < (MaxSlotNumBits - Index - 1); Count++) {
            Number = Number * 10;
          }
          NetcardId = NetcardId + Number;
        }

        return (NetcardId + 1); // 0 is Onboard net option rather than Slot 0
      }
    } else { // Description of Onboard option like:UEFI Onboard:Port M - xxxxx PXE IPv4
      if (StrStr(mOptionInfo[CurrentIndex].Description, OnboardString) != NULL) {
        return 0;
      }
    }
  }

  //
  // Combine BusNumber and DeviceNumber as NetcardId of PCI netcard;
  //
  if (mOptionInfo[CurrentIndex].IsPciNet) {
    NetcardId = (((Segment & 0xFFFF) + 1) << 16) + ((Bus & 0xFF) << 8) + (Device & 0xFF);
  } else {
    mOptionInfo[CurrentIndex].Function = 0;
    //
    // For netcards which don't support PCI, assign NetcardId by enumeration order
    // and put old net options in front of the new
    //
    DevicePath = mOptionInfo[CurrentIndex].DevicePath;
    ShowDevicePathDxe(gBS, DevicePath);
    for (Index = 0; Index < mNetInfoIndex; Index++) {
      if (CompareMem(DevicePath, mNetInfo[Index].DevicePath, mNetInfo[Index].Size) == 0) {
        NetcardId = mNetInfo[Index].Value & 0xFF;
        break;
      }
    }

    if (Index == mNetInfoIndex) {
      if (mNetInfoIndex == mNetInfoCount) {
        mNetInfo = (NETCARD_ID_INFO *)ReallocatePool(sizeof(NETCARD_ID_INFO) * mNetInfoCount,
                                                     sizeof(NETCARD_ID_INFO) * (mNetInfoCount + INCREMENT_OF_COUNT),
                                                     mNetInfo);
        ASSERT(mNetInfo != NULL);
        mNetInfoCount = mNetInfoCount + INCREMENT_OF_COUNT;
      }

        while (!IsDevicePathEnd(DevicePath)) {
          if ((DevicePathType(DevicePath) == MESSAGING_DEVICE_PATH)
            && (DevicePathSubType(DevicePath) == MSG_MAC_ADDR_DP)) {
            DevicePath = NextDevicePathNode(DevicePath);
            //
            // note size of DevicePath ended at MAC node, like PciRoot(0x0)/Pci(xx,xx)/.../MAC(xxxx,0x0)
            NetDeviceSize = GetDevicePathSize(mOptionInfo[CurrentIndex].DevicePath) - GetDevicePathSize(DevicePath);
            break;
          }
          DevicePath = NextDevicePathNode(DevicePath);
        }

        mNetInfo[mNetInfoIndex].Size = NetDeviceSize;
        mNetInfo[mNetInfoIndex].DevicePath = AllocateCopyPool(NetDeviceSize, mOptionInfo[CurrentIndex].DevicePath);
        ASSERT(mNetInfo[mNetInfoIndex].DevicePath != NULL);
        mNetInfo[mNetInfoIndex].Value = (UINT8)mNetInfoIndex;
        NetcardId = mNetInfoIndex & 0xFF;
        mNetInfoIndex++;
    }
  }
  return NetcardId;
}

UINTN
GetOptionPriority (
  UINTN          CurrentIndex
  )
{
  EFI_STATUS                        Status;
  UINTN                             Index = INVALID_HDD_PRIORITY;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL          *TempDevice;
  EFI_HANDLE                        Handle;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  UINTN                             Seg, Dev,Func;
  UINTN                             Bus = 0;
  PLAT_HOST_INFO_PROTOCOL           *PlatHostInfo;
  UINT16                            AtaIndex = 0xFFFF;
  UINT16                            PlatSataHostIndex = 0;
  UINT16                            Lun = 0xFF;
  ATAPI_DEVICE_PATH                 *SataIdeDp;
  SCSI_DEVICE_PATH                  *ScsiDp;
  UINT8                             HddGroupType;
  UINTN                             Count;
  UINT8                             SlotNumber;
  EFI_DISK_INFO_PROTOCOL            *DiskInfo;
  UINT32                            IdeChannel;
  UINT32                            IdeDevice;

  if (mOptionInfo[CurrentIndex].GroupType == BM_MENU_TYPE_UEFI_PXE) {
    Index = (mOptionInfo[CurrentIndex].NetcardId << 16) + ((mOptionInfo[CurrentIndex].Function & 0xFF) << 8)
            + mOptionInfo[CurrentIndex].NetIpGroupType;
    return Index;
  }
  
  if (mOptionInfo[CurrentIndex].GroupType == BM_MENU_TYPE_UEFI_HDD) {
    //
    // Check if it's HDD Short-Form DevicePath
    //
    DevicePath = mOptionInfo[CurrentIndex].DevicePath;
    if ((DevicePathType(DevicePath) == MEDIA_DEVICE_PATH) && (DevicePathSubType(DevicePath) == MEDIA_HARDDRIVE_DP)) {
      DevicePath = NextDevicePathNode(DevicePath);
      while (!IsDevicePathEnd(DevicePath)) {
        if ((DevicePathType(DevicePath) == MEDIA_DEVICE_PATH) && (DevicePathSubType(DevicePath) == MEDIA_FILEPATH_DP)) {
          break;
        }
        DevicePath = NextDevicePathNode(DevicePath);
      }
      if (!IsDevicePathEnd(DevicePath)) { // is HD short-form DevicePath
        Index = HDD_SHORT_FORM_TYPE * MASK_OF_HDD_PRIORITY;
        goto RETURN_DEVICE_PRIORITY;
      }
    }

    //
    // Check if it's Messaging DevicePath(including SATA, NVME and so on)
    //
    DevicePath = mOptionInfo[CurrentIndex].DevicePath;
    while (!IsDevicePathEnd(DevicePath) && (DevicePathType(DevicePath) != MESSAGING_DEVICE_PATH)) {
      DevicePath = NextDevicePathNode(DevicePath);
    }
    if (IsDevicePathEnd(DevicePath)) { // It's not HDD options with Messaging DevicePath
      return INVALID_HDD_PRIORITY;
    }

    //
    // Get DevicePriority of Messaging devices
    //
    TempDevice = mOptionInfo[CurrentIndex].DevicePath;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TempDevice, &Handle);
    if (!EFI_ERROR(Status)) {
      Status = gBS->HandleProtocol(
                  Handle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo
                  );
      ASSERT(!EFI_ERROR(Status));
      PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
    }

    switch (DevicePath->SubType) {
      case MSG_NVME_NAMESPACE_DP:
        Index = NVME_SSD_TYPE * MASK_OF_HDD_PRIORITY + ((Seg & 0xFFFF) << 24) + ((Bus & 0xFF) << 16) + ((Dev & 0xFF) << 8) + (Func & 0xFF);
        break;
        
      case MSG_SATA_DP:
        TempDevice = mOptionInfo[CurrentIndex].DevicePath;
        Status = gBS->LocateDevicePath(&gEfiDiskInfoProtocolGuid, &TempDevice, &Handle);
        if (EFI_ERROR(Status)) {
          Index = SATA_HDD_TYPE * MASK_OF_HDD_PRIORITY;
          goto RETURN_DEVICE_PRIORITY;
        }
        Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&PlatHostInfo);
        if (!EFI_ERROR(Status)) {
          if (PlatHostInfo->GetSataPortIndex != NULL) {
            AtaIndex = PlatHostInfo->GetSataPortIndex(Handle);
          }
  
          if (PlatHostInfo->GetSataHostIndex != NULL) {
            PlatSataHostIndex = PlatHostInfo->GetSataHostIndex(Handle);
          }
        }
  
        if (AtaIndex == 0xFFFF) {
          //
          // Get Sata port number from DiskInfo first
          //
          Status = gBS->HandleProtocol (Handle, &gEfiDiskInfoProtocolGuid, (VOID **) &DiskInfo);
          if (!EFI_ERROR(Status)) {
            if (CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid)) {
              Status = DiskInfo->WhichIde (
                           DiskInfo,
                           &IdeChannel,
                           &IdeDevice
                           );
              if (!EFI_ERROR(Status)) {
                AtaIndex = (UINT16)IdeChannel;
              }
            }
          }

          //
          // Get Sata port number from SATA DevicePath Node
          //
          if (AtaIndex == 0xFFFF) {
            AtaIndex = ((SATA_DEVICE_PATH*)DevicePath)->HBAPortNumber;
          }
        }

        if ((PlatSataHostIndex != 0xFFFF) && (PlatSataHostIndex != 0x8000)) {
          Index = SATA_HDD_TYPE * MASK_OF_HDD_PRIORITY + ((UINTN)PlatSataHostIndex << 8) + (UINT8)AtaIndex;
        } else {
          Index = SATA_HDD_TYPE * MASK_OF_HDD_PRIORITY + (UINT8)AtaIndex; // set PlatSataHostIndex to 0
        }
        if (Index >= INVALID_HDD_PRIORITY) {
          Index = SATA_HDD_TYPE * MASK_OF_HDD_PRIORITY;
        }
        Lun = ((SATA_DEVICE_PATH *)DevicePath)->Lun;
	      break;
        
      case MSG_ATAPI_DP:
        SataIdeDp = (ATAPI_DEVICE_PATH*)DevicePath;
	      Index = ATAPI_HDD_TYPE * MASK_OF_HDD_PRIORITY + SataIdeDp->PrimarySecondary*2 + SataIdeDp->SlaveMaster;
        if (Index >= INVALID_HDD_PRIORITY) {
          Index = ATAPI_HDD_TYPE * MASK_OF_HDD_PRIORITY;
        }
	      break;
  
      case MSG_SCSI_DP:
        ScsiDp = (SCSI_DEVICE_PATH *)DevicePath;
        Index = SCSI_HDD_TYPE * MASK_OF_HDD_PRIORITY + (UINT8)Bus * 0x10000 + ScsiDp->Pun;
        Lun = ((SCSI_DEVICE_PATH *)DevicePath)->Lun;
        break;

      case MSG_EMMC_DP:
        SlotNumber = ((EMMC_DEVICE_PATH *)DevicePath)->SlotNumber;
        Index = MMC_HDD_TYPE * MASK_OF_HDD_PRIORITY + SlotNumber;
        break;

      case MSG_SD_DP:
        SlotNumber = ((SD_DEVICE_PATH *)DevicePath)->SlotNumber;
        Index = SD_HDD_TYPE * MASK_OF_HDD_PRIORITY + SlotNumber;
        break;

      default:
        Index = INVALID_HDD_PRIORITY;
        break;
    }
  
    if (StrStr(mOptionInfo[CurrentIndex].Description, L"Lun") != NULL) { // is raid
      Index = RAID_HDD_TYPE * MASK_OF_HDD_PRIORITY + (UINT8)Bus * 0x10000 + Lun;
    }
  }

RETURN_DEVICE_PRIORITY:
  HddGroupType = (UINT8)(Index >> 40);
  for (Count = 0; Count < COUNT_OF_HDD_GROUP_TYPE; Count++) {
    if (HddGroupType == mSortPolicy->HddGroupOrder[Count]) {
      break;
    }
  }
  if (Count < COUNT_OF_HDD_GROUP_TYPE) {
    Index = (Count + 1) * MASK_OF_HDD_PRIORITY + (Index & 0xFFFFFFFFFF);
  } else {
    Index = INVALID_HDD_PRIORITY;
  }

  return Index;
}

UINTN
ByoUpdateOrderOfOldOptions (
  IN BOOLEAN                        FoundSmallerPriority,
  IN UINTN                          MaxItemOrder,
  IN UINTN                          MintItemOrder,
  IN UINTN                          CurrentIndex
  )
{
  UINTN                             Index;

  for (Index = 0; Index < mOptionInfoCount; Index++) {
    if (!mOptionInfo[Index].IsNew && (mOptionInfo[Index].GroupType == mOptionInfo[CurrentIndex].GroupType)) {
      if (FoundSmallerPriority) {
        if (mOptionInfo[Index].ItemOrder > MaxItemOrder) {
          mOptionInfo[Index].ItemOrder = mOptionInfo[Index].ItemOrder + 1;
        }
      } else {
        if (mOptionInfo[Index].ItemOrder >= MintItemOrder) {
          mOptionInfo[Index].ItemOrder = mOptionInfo[Index].ItemOrder + 1;
        }
      }
    }
  }

  if (FoundSmallerPriority) {
    return (MaxItemOrder + 1);
  } else {
    return MintItemOrder;
  }
}

VOID
SetAndUpdateItemOrder (
  IN UINTN                          MaxOrderOfHdd
  )
{
  UINTN                             Index;
  UINTN                             Count;
  BOOLEAN                           InsertNewOptionByPriority;
  BOOLEAN                           SameTypeNetOptionExist;
  BOOLEAN                           ReferenceInited;
  BOOLEAN                           MinReferenceInited;
  UINTN                             MaxValue;
  UINTN                             MinValue;
  UINTN                             MaxItemOrder = 0;
  UINTN                             MinItemOrder = 0;
  BOOLEAN                           FoundSmallerPriority;

  if (mOptionInfoCount == 0) {
    return;
  }
  InsertNewOptionByPriority = (mSortPolicy->CustomizedSortPolicy & INSERT_NEW_OPTION_BY_PRIORITY) != 0? TRUE: FALSE;
  DEBUG((EFI_D_INFO, "InsertNewOptionByPriority=%d %a\n", InsertNewOptionByPriority, __FUNCTION__));

  //
  // Set ItemOrder of new options and update ItemOrder of other old options in the same group
  //
  for (Index = 0; Index < mOptionInfoCount; Index++) {
    if (!mOptionInfo[Index].IsNew) {
      continue;
    }

    //
    // Insert new HDD options at the end of old HDD options when INSERT_NEW_OPTION_BY_PRIORITY not set
    //
    if (!InsertNewOptionByPriority && (mOptionInfo[Index].GroupType == BM_MENU_TYPE_UEFI_HDD)) {
      mOptionInfo[Index].ItemOrder = MaxOrderOfHdd + mOptionInfo[Index].DevicePriority;
      mOptionInfo[Index].IsNew = FALSE;
      continue;
    }

    //
    // 1. Check if old net options with the same NetcardId exist when current option is net:
    // 1) When no existing and INSERT_NEW_OPTION_BY_PRIORITY set, find out the biggest ItemOrder of
    // old net options whose NetcardId smaller then the current NetcarId as MaxItemOrder;
    // 2) When no existing and INSERT_NEW_OPTION_BY_PRIORITY not set, find out the biggest ItemOrder
    // of old net options as MaxItemOrder.
    // 2. When current option is HDD, find out the biggest ItemOrder of hdd option whose DevicePriority
    // is smaller than the current option's as MaxItemOrder, and find out the smallest ItemOrder of hdd
    // option whose DevicePriority is bigger than the current option's as MinItemOrder; If MaxItemOrder
    // exists, insert current option after the option whose ItemOrder is  MaxItemOrder.Otherwise, insert
    // current before the option whose ItemOrder is MinItemOrder.
    //
    SameTypeNetOptionExist = FALSE;
    MinReferenceInited = FALSE;
    ReferenceInited = FALSE;
    FoundSmallerPriority = FALSE;
    for (Count = 0; Count < mOptionInfoCount; Count++) {
      if (mOptionInfo[Count].IsNew || (mOptionInfo[Count].GroupType != mOptionInfo[Index].GroupType) || (!mOptionInfo[Count].IsBdsCreate)) {
        continue;
      }

      if (mOptionInfo[Index].GroupType == BM_MENU_TYPE_UEFI_PXE) {
        if (InsertNewOptionByPriority) {
          //
          // note the biggest ItemOrder of net option whose NetcardId is smaller than current option
          //
          if (mOptionInfo[Count].NetcardId <= mOptionInfo[Index].NetcardId) {
            FoundSmallerPriority = TRUE;
            if (!ReferenceInited) {
              MaxItemOrder = mOptionInfo[Count].ItemOrder;
              ReferenceInited = TRUE;
            }

            if (mOptionInfo[Count].ItemOrder > MaxItemOrder) {
              MaxItemOrder = mOptionInfo[Count].ItemOrder;
            }
          }

          if (mOptionInfo[Count].NetcardId > mOptionInfo[Index].NetcardId) {
            if (!MinReferenceInited) {
              MinItemOrder = mOptionInfo[Count].ItemOrder;
              MinReferenceInited = TRUE;
            }

            if (mOptionInfo[Count].ItemOrder < MinItemOrder) {
              MinItemOrder = mOptionInfo[Count].ItemOrder;
            }
          }
        } else {
          //
          // note the biggest ItemOrder of net option
          //
          if (!ReferenceInited) {
            MaxItemOrder = mOptionInfo[Count].ItemOrder;
            ReferenceInited = TRUE;
          }

          if (mOptionInfo[Count].ItemOrder > MaxItemOrder) {
            MaxItemOrder = mOptionInfo[Count].ItemOrder;
          }
        }

        if ((mOptionInfo[Count].NetcardId == mOptionInfo[Index].NetcardId)
          && (mOptionInfo[Count].Function == mOptionInfo[Index].Function)) {
          SameTypeNetOptionExist = TRUE; // old net option with the same NetcardId and same port(functionNumber) exist
        }
      } else if (mOptionInfo[Index].GroupType == BM_MENU_TYPE_UEFI_HDD) {
        //
        // Note the smallest ItemOrder of option whose DevicePriority is bigger than the current option.
        //
        if (mOptionInfo[Count].DevicePriority >= mOptionInfo[Index].DevicePriority) {
          if (!MinReferenceInited) {
            MinItemOrder = mOptionInfo[Count].ItemOrder;
            MinReferenceInited = TRUE;
          }
          if (mOptionInfo[Count].ItemOrder < MinItemOrder) {
            MinItemOrder = mOptionInfo[Count].ItemOrder;
          }
        }

        //
        // Note the biggest ItemOrder of option whos DevicePriority is smaller than the current option.
        //
        if (mOptionInfo[Count].DevicePriority <= mOptionInfo[Index].DevicePriority) {
          FoundSmallerPriority = TRUE;
          if (!ReferenceInited) {
            MaxItemOrder = mOptionInfo[Count].ItemOrder;
            ReferenceInited = TRUE;
          }
          if (mOptionInfo[Count].ItemOrder > MaxItemOrder) {
            MaxItemOrder = mOptionInfo[Count].ItemOrder;
          }
        }
      }
    }
    DEBUG((EFI_D_INFO, "FoundSmallerPriority=%d MaxItemOrder=%x %a line=%d\n", FoundSmallerPriority,
      MaxItemOrder, __FUNCTION__, __LINE__));

    //
    // When current option is net and old net options with the same NetcardId exist, find out the biggest
    // DevicePriority of the option whose DevicePriority is smaller than current option's and set ItemOrder
    // of this option to MaxItemOrder, and find out the smallest DevicePriority of the option whose DevicePriority
    // is bigger than current option's, then set ItemOrder of this option to MinItemOrder.If MaxItemOrder
    // exists, insert current net option after the option whose ItemOrder is  MaxItemOrder.Otherwise, insert
    // current before the option whose ItemOrder is MinItemOrder.
    //
    if (SameTypeNetOptionExist) {
      DEBUG((EFI_D_INFO, "Pxe SameTypeNetOptionExist %a line=%d\n", __FUNCTION__, __LINE__));
      ReferenceInited = FALSE;
      MinReferenceInited = FALSE;
      FoundSmallerPriority = FALSE;
      for (Count = 0; Count < mOptionInfoCount; Count++) {
        if (mOptionInfo[Count].IsNew || (mOptionInfo[Count].GroupType != BM_MENU_TYPE_UEFI_PXE)
          || ((mOptionInfo[Count].NetcardId + mOptionInfo[Count].Function)
          != (mOptionInfo[Index].NetcardId + mOptionInfo[Index].Function))) {
          continue;
        }

        if (mOptionInfo[Count].DevicePriority > mOptionInfo[Index].DevicePriority) {
          //
          // Note the ItemOder of option whose DevicePriority is the smallest but bigger than current option's
          //
          if (!MinReferenceInited) {
            MinValue = mOptionInfo[Count].DevicePriority;
            MinItemOrder = mOptionInfo[Count].ItemOrder;
            MinReferenceInited = TRUE;
          }

          if (mOptionInfo[Count].DevicePriority < MinValue) {
            MinValue = mOptionInfo[Count].DevicePriority;
            MinItemOrder = mOptionInfo[Count].ItemOrder;
          }
        } else {
          //
          // Note the ItemOder of option whose DevicePriority is the biggest but smaller than current option's
          //
          FoundSmallerPriority = TRUE;
          if (!ReferenceInited) {
            MaxValue = mOptionInfo[Count].DevicePriority;
            MaxItemOrder = mOptionInfo[Count].ItemOrder;
            ReferenceInited = TRUE;
          }

          if (mOptionInfo[Count].DevicePriority >= MaxValue) {
            MaxValue = mOptionInfo[Count].DevicePriority;
            MaxItemOrder = mOptionInfo[Count].ItemOrder;
          }
        }
      }
    } else if (!SameTypeNetOptionExist && (mOptionInfo[Index].GroupType == BM_MENU_TYPE_UEFI_PXE)) {
      if (!InsertNewOptionByPriority) {
        FoundSmallerPriority = TRUE;
      }
    }

    //
    // Update old options with the same GroupType and the ItemOrder bigger than MaxItemOrder
    //
    DEBUG((EFI_D_INFO, "FoundSmallerPriority=%d MaxItemOrder=%x MinItemOrder=%x Index=%d  %a line=%d\n", 
      FoundSmallerPriority, MaxItemOrder, MinItemOrder, Index, __FUNCTION__, __LINE__));
    mOptionInfo[Index].ItemOrder = ByoUpdateOrderOfOldOptions(FoundSmallerPriority, MaxItemOrder, MinItemOrder, Index);
    mOptionInfo[Index].IsNew = FALSE;
  }
}

/**
  Sort the specified boot options, and return the sorted boot options.

  @param  SortOptions           Boot options needed to be sorted.If it's NULL, return.
  @param  SortOptionsCount      Count of SortOptions.
  @param CompareFunction        The comparator
**/
VOID
EFIAPI
ByoSortSpecifiedBootOptions (
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION    *SortOptions,
  IN CONST UINTN                         SortOptionsCount,
  SORT_COMPARE                       CompareFunction
  )
{
  // UINTN    Index;

  if ((SortOptions == NULL) || (SortOptionsCount == 0)) {
    return;
  }

  // DEBUG((EFI_D_INFO, "Dump options before sorting %a line=%d\n", __FUNCTION__, __LINE__));
  // for (Index = 0; Index < SortOptionsCount; Index++) {
  //   DEBUG((EFI_D_INFO, "Index%d:%s %a line=%d\n", Index, SortOptions[Index].Description, __FUNCTION__, __LINE__));
  // }

  //
  // Insertion sort algorithm
  //
  PerformQuickSort (
    SortOptions,
    SortOptionsCount,
    sizeof (EFI_BOOT_MANAGER_LOAD_OPTION),
    CompareFunction
    );

  // DEBUG((EFI_D_INFO, "Dump options after sorting %a line=%d\n", __FUNCTION__, __LINE__));
  // for (Index = 0; Index < SortOptionsCount; Index++) {
  //   DEBUG((EFI_D_INFO, "Index%d:%s %a line=%d\n", Index, SortOptions[Index].Description, __FUNCTION__, __LINE__));
  // }
}

/**
  Sorting UEFI boot options.
  This function can be called after the first calling EfiBootManagerRefreshAllBootOption() to sort UEFI boot options.The
  order of Legacy boot options can not be changed(Legacy boot options are sorted in LegacyBootManagerLib).It will keep
  the order of NV boot options and insert new UEFI boot options according to sorting policy:
  1. Check if the PcdBiosBootModeType is BIOS_BOOT_LEGACY_OS, and return when it's BIOS_BOOT_LEGACY_OS;
  2. Check if boot options exist, and return when no boot options exist;
  3. Check if new boot options exist, and return when no new boot options.All old boot options were sorted at the last
  booting.
  4. Get sorting policy from L"CustomizedSortPolicy" variable defined in ByoCustomizedSortPolicy.h.
  The platform can customize the sorting policy by setting this variable, otherwise the default policy is used.
  5. Assign DevicePriority to all UEFI HDD and net options according to the sorting policy and hardware information,
  such as SATA port number, netcard slot number, PCI bus number and so on.
  6. Assign ItemOrder to all UEFI HDD and net options to make sure that they can be sorted as sorting policy.
  7. Call EfiBootManagerSortLoadOptionVariable() to actually sort all boot options and set to L"BootOrder".This step will
  sort the boot options into groups based on L"UefiBootGroupOrder" variable.

  @param  OldOptions            Boot options saved from the last booting, they are typically get before calling
                                EfiBootManagerRefreshAllBootOption().
  @param  OldOptionsCount       Count of OldOptions.
  @param  SortOptions           Boot options needed to be sorted.If it's NULL this function will sort boot options got from BootOrder in NV.
  @param  SortOptionsCount      Count of SortOptions.
  @param  CompareBootOption     The comparator function pointer.It can point to the function implemented by the platform
                                or NULL(use ByoCompareBootOption()) as CompareFunction parameter of
                                EfiBootManagerSortLoadOptionVariable().
**/
VOID
EFIAPI
ByoSortBootOptions (
  IN CONST EFI_BOOT_MANAGER_LOAD_OPTION  *OldOptions,
  IN UINTN                               OldOptionsCount,
  IN OUT EFI_BOOT_MANAGER_LOAD_OPTION    *SortOptions,
  IN CONST UINTN                         SortOptionsCount,
  IN SORT_COMPARE                        CompareBootOption
  )
{
  EFI_STATUS                        Status;
  UINTN                             Index;
  UINTN                             Count = 0;
  EFI_BOOT_MANAGER_LOAD_OPTION      *Options;
  UINTN                             OptionsCount;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath;
  UINT8                             GroupType;
  BOOLEAN                           FoundNewOption = FALSE;
  BOOLEAN                           IsNew;
  UINTN                             MaxOrderOfHdd = 0;
  UINTN                             VarSize;
  EFI_HANDLE                        Handle;
  EFI_PCI_IO_PROTOCOL               *PciIo = NULL;
  UINTN                             Seg, Bus, Device;
  UINT8                             IpType;
  CHAR16                            *BootType;
  UINTN                             NetDeviceSize = 0;
  BOOLEAN                           SortNvOptions = TRUE;
  PLAT_HOST_INFO_PROTOCOL           *PlatHostInfo = NULL;
  POST_HOT_KEY_CTX                  *PostHotKeyCtx = NULL;
  UINTN                             PostHotKeyCount = 0;
  CHAR16                            *OsRecoveryDesc = NULL;
  UINT32                            MaxNetcardId = 0;
  EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL  *Nii;
  INTN                              MatchedIndex;
  EFI_BOOT_MANAGER_LOAD_OPTION      *TempOldOption;

  if (PcdGet8(PcdBiosBootModeType) == BIOS_BOOT_LEGACY_OS) {
    DEBUG((EFI_D_INFO, "Don't sort Legacy options in %a\n", __FUNCTION__));
    return;
  }
  if ((OldOptions != NULL) && (OldOptionsCount != 0)) {
    IsLoadDefault = FALSE;
  } else {
    IsLoadDefault = TRUE;
  }
  if ((SortOptions != NULL) && (SortOptionsCount != 0)) {
    Options = SortOptions;
    OptionsCount = SortOptionsCount;
    SortNvOptions = FALSE;
  } else {
    Options = EfiBootManagerGetLoadOptions (&OptionsCount, LoadOptionTypeBoot);
  }
  if (OptionsCount == 0) {
    DEBUG((EFI_D_INFO, "No options need sort in %a\n", __FUNCTION__));
    return;
  }

  //
  // Initialize OPTION_INFO of network and hdd options;
  //
  mOptionInfo = (OPTION_INFO *)AllocateZeroPool(sizeof(OPTION_INFO) * OptionsCount);
  ASSERT(mOptionInfo != NULL);

  //
  // Initialize NETCARD_ID_INFO of netcard options
  //
  if (mNetInfo == NULL) {
    mNetInfo = (NETCARD_ID_INFO *)AllocateZeroPool(sizeof(NETCARD_ID_INFO) * mNetInfoCount);
    ASSERT(mNetInfo != NULL);
  }  

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID **)&PlatHostInfo);
  if ((PlatHostInfo != NULL) && PlatHostInfo->HotKey != NULL && PlatHostInfo->HotKeyCount != 0) {
    PostHotKeyCtx  = PlatHostInfo->HotKey;
    PostHotKeyCount = PlatHostInfo->HotKeyCount;

    for (Index = 0; Index < PostHotKeyCount; Index++) {
      if (PostHotKeyCtx[Index].BootType == HOTKEY_BOOT_WINDOWS_RECOVERY) {
        OsRecoveryDesc = PostHotKeyCtx[Index].BootOptionName;
        break;
      }
    }
  }

  //
  // 1.Initialize OPTION_INFO of net and HDD options;
  // 2.Check if new options exist and assign ItemOrder of old options
  //
  for (Index = 0; Index < OptionsCount; Index++) {
    NetDeviceSize = 0;
    IsNew = TRUE;
    FoundNewOption = FALSE;
    MatchedIndex = EfiBootManagerFindLoadOption(&Options[Index], OldOptions, OldOptionsCount);
    if (MatchedIndex == -1) {
      FoundNewOption = TRUE;
    }

    //
    // Now existed legacy options are NV options, and init legacy options
    //
    if (DevicePathType(Options[Index].FilePath) == BBS_DEVICE_PATH) {
      mOptionInfo[mOptionInfoCount].GroupType = BM_MENU_TYPE_UEFI_OTHERS;
      mOptionInfo[mOptionInfoCount].IsNew = FALSE;
      mOptionInfo[mOptionInfoCount].ItemOrder = Index + 1;
      mOptionInfo[mOptionInfoCount].DevicePath = DuplicateDevicePath(Options[Index].FilePath);
      mOptionInfo[mOptionInfoCount].Description = (CHAR16 *)AllocateCopyPool(StrSize(Options[Index].Description),
                                                                             Options[Index].Description);
      ASSERT(mOptionInfo[mOptionInfoCount].Description != NULL);
      if (!IsLoadDefault) {
        if (BmIsAutoCreateBootOption(&Options[Index])) {
          mOptionInfo[mOptionInfoCount].IsBdsCreate = TRUE;
        } else {
          mOptionInfo[mOptionInfoCount].IsBdsCreate = FALSE;
        }
      } else {
        mOptionInfo[mOptionInfoCount].IsBdsCreate = TRUE;
      }
      mOptionInfoCount++;
      continue;
    }

    if (!IsLoadDefault) {
      if ((!BmIsAutoCreateBootOption(&Options[Index])) && (GetEfiBootGroupType(Options[Index].FilePath) == BM_MENU_TYPE_UEFI_HDD)) {
        mOptionInfo[mOptionInfoCount].IsNew = FALSE;
        mOptionInfo[mOptionInfoCount].ItemOrder = MatchedIndex;
        mOptionInfo[mOptionInfoCount].GroupType = BM_MENU_TYPE_UEFI_HDD;
        mOptionInfo[mOptionInfoCount].DevicePath = DuplicateDevicePath(Options[Index].FilePath);
        mOptionInfo[mOptionInfoCount].Description = (CHAR16 *)AllocateCopyPool(StrSize(Options[Index].Description),
                                                                              Options[Index].Description);
        ASSERT(mOptionInfo[mOptionInfoCount].Description != NULL);
        mOptionInfo[mOptionInfoCount].IsBdsCreate = FALSE;
        mOptionInfoCount++;
        continue;
      }
    }

    GroupType = GetEfiBootGroupType(Options[Index].FilePath);
    if ((GroupType != BM_MENU_TYPE_UEFI_HDD) && (GroupType != BM_MENU_TYPE_UEFI_PXE)) {
      continue;
    }
    mOptionInfo[mOptionInfoCount].GroupType = GroupType;

    DevicePath = Options[Index].FilePath;
    while (!IsDevicePathEnd(DevicePath)) {
      if ((DevicePathType(DevicePath) == MESSAGING_DEVICE_PATH) && (DevicePathSubType(DevicePath) == MSG_ISCSI_DP)) {
        break;
      }
      DevicePath = NextDevicePathNode(DevicePath);
    }
    if (!IsDevicePathEnd(DevicePath)) {
      mOptionInfo[mOptionInfoCount].IsiSCSI = TRUE;
    }

    if (GetEfiBootGroupType(Options[Index].FilePath) == BM_MENU_TYPE_UEFI_PXE) {
      IsNew = FoundNewOption;
      if (!IsNew) {
        Count = (UINTN)MatchedIndex;
      }
    } else if (GetEfiBootGroupType(Options[Index].FilePath) == BM_MENU_TYPE_UEFI_HDD) {
      for (Count = 0; Count < OldOptionsCount; Count++) {
        if (GroupType != GetEfiBootGroupType(OldOptions[Count].FilePath)) {
          continue;
        }
        TempOldOption = (EFI_BOOT_MANAGER_LOAD_OPTION*)&OldOptions[Count];
        if (!BmIsAutoCreateBootOption(TempOldOption)) {
          continue;
        }
        //
        // Skip OS Recovery option
        //
        if ((OsRecoveryDesc != NULL) && (StrStr(OldOptions[Count].Description, OsRecoveryDesc) != NULL)
          && ((OldOptions[Count].Attributes & LOAD_OPTION_ACTIVE) == 0)) {
          continue;
        }

        if (CompareAllDevicePath(Options[Index].FilePath, OldOptions[Count].FilePath)) {
          IsNew = FALSE;
          break;
        }
      }

      //
      // If the Description of UEFI HDD option is not changed due to OS installation or HDD formatting, and the FilePath
      // is not changed, this UEFI HDD option need to be reordered.
      // Note:When installing OS or formatting HDD, only some information is added to or removed from the original
      // Description, and the original Description as a whole are not corrupted.The following examples show how
      // Description of UEFI HDD option changes in different cases:
      // 1) Install OS(ubuntu): SATA XXXX ->  ubuntu(SATA XXXX);
      // 2) Formatting HDD: ubuntu(SATA XXXX) -> SATA XXXX;
      // 3) Exchange the sata port of SATA HDD(SATA1: XXXX) and SATA DVD(SATA2: AAAA): SATA1: XXXX -> SATA2: XXXX;
      //
      if (!IsNew && FoundNewOption) {
        if ((StrStr(Options[Index].Description, OldOptions[Count].Description) == NULL)
          && (StrStr(OldOptions[Count].Description, Options[Index].Description) == NULL)) {
            IsNew = TRUE;
        }
      }
    }

    if (!IsNew) {
      //
      // It's old option, and ItemOrder is count.
      // Note the max ItemOrder of old hdd options.
      //
      mOptionInfo[mOptionInfoCount].ItemOrder = Count;
      if (GroupType == BM_MENU_TYPE_UEFI_HDD) {
        if (mOptionInfo[mOptionInfoCount].ItemOrder > MaxOrderOfHdd) {
          MaxOrderOfHdd = mOptionInfo[mOptionInfoCount].ItemOrder;
        }
      } else if ((GroupType == BM_MENU_TYPE_UEFI_PXE) && !mOptionInfo[mOptionInfoCount].IsiSCSI) {
        //
        // Note NETCARD_ID_INFO of netcard for assigning NetcardId of NON PCI netcard option later
        //
        if (mNetInfoIndex == mNetInfoCount) {
          mNetInfo = (NETCARD_ID_INFO *)ReallocatePool(sizeof(NETCARD_ID_INFO) * mNetInfoCount,
                                                       sizeof(NETCARD_ID_INFO) * (mNetInfoCount + INCREMENT_OF_COUNT),
                                                       mNetInfo);
          ASSERT(mNetInfo != NULL);
          mNetInfoCount = mNetInfoCount + INCREMENT_OF_COUNT;
        }
        DevicePath = Options[Index].FilePath;
        while (!IsDevicePathEnd(DevicePath)) {
          if ((DevicePathType(DevicePath) == MESSAGING_DEVICE_PATH) && (DevicePathSubType(DevicePath) == MSG_MAC_ADDR_DP)) {
            DevicePath = NextDevicePathNode(DevicePath);
            //
            // note size of DevicePath ended at MAC node, like PciRoot(0x0)/Pci(xx,xx)/.../MAC(xxxx,0x0)
            //
            NetDeviceSize = GetDevicePathSize(Options[Index].FilePath) - GetDevicePathSize(DevicePath);
            break;
          }
          DevicePath = NextDevicePathNode(DevicePath);
        }
        if (NetDeviceSize == 0) {
          continue;
        }

        for (Count = 0; Count < mNetInfoIndex; Count++) {
          if (CompareMem(Options[Index].FilePath, mNetInfo[Count].DevicePath, mNetInfo[Count].Size) == 0) {
            break;
          }
        }
        if (Count == mNetInfoIndex) { // not found or no one exist
          mNetInfo[mNetInfoIndex].Size = NetDeviceSize;
          mNetInfo[mNetInfoIndex].DevicePath = AllocateCopyPool(NetDeviceSize, Options[Index].FilePath);
          ASSERT(mNetInfo[mNetInfoIndex].DevicePath != NULL);
          mNetInfo[mNetInfoIndex].Value = (UINT8)mNetInfoIndex;
          mNetInfoIndex++;
        }
      }
    }
    mOptionInfo[mOptionInfoCount].IsNew = IsNew;
    mOptionInfo[mOptionInfoCount].DevicePath = DuplicateDevicePath(Options[Index].FilePath);
    mOptionInfo[mOptionInfoCount].Description = (CHAR16 *)AllocateCopyPool(StrSize(Options[Index].Description),
                                                                           Options[Index].Description);
    ASSERT(mOptionInfo[mOptionInfoCount].Description != NULL);
    if (!IsLoadDefault) {
      if (BmIsAutoCreateBootOption(&Options[Index])) {
        mOptionInfo[mOptionInfoCount].IsBdsCreate = TRUE;
      } else {
        mOptionInfo[mOptionInfoCount].IsBdsCreate = FALSE;
      }
    } else {
      mOptionInfo[mOptionInfoCount].IsBdsCreate = TRUE;
    }
    mOptionInfoCount++;
  }

  if (!FoundNewOption) { // when new options exist,we need to update ItemOrder of OptionsInfo.
    goto NO_NEED_TO_ASSIGN_ITEMORDER;
  }
  MaxOrderOfHdd++;
  DEBUG((EFI_D_INFO, "MaxOrderOfHdd=%d %a line=%d\n", MaxOrderOfHdd, __FUNCTION__, __LINE__));

  //
  // Get customized sort policy
  //
  Status = GetVariable2 (
                  BYO_CUSTOMIZED_SORT_POLICY_VAR_NAME,
                  &gByoGlobalVariableGuid,
                  (VOID **)&mSortPolicy,
                  &VarSize
                  );
  if (EFI_ERROR(Status)) {
    VarSize = sizeof(mDefaultOptionSortOption);
    mSortPolicy = (OPTION_SORT_POLICY *)AllocateCopyPool(VarSize, &mDefaultOptionSortOption);
    ASSERT(mSortPolicy != NULL);
  }

  //
  // Initialize the rest OPTION_INFO of network and hdd options for assigning DevicePriority;
  //
  for (Index = 0; Index < mOptionInfoCount; Index++) {
    if (mOptionInfo[Index].GroupType == BM_MENU_TYPE_UEFI_PXE) {
      IpType = GetEfiNetWorkType(mOptionInfo[Index].DevicePath, &BootType);
      if (BootType == NULL) {
        continue;
      }

      //
      // Assign NetIpGroupType to OPTION_INFO of net option
      //
      if ((IpType == MSG_IPv4_DP) && (StrStr(BootType, L"PXE") != NULL)) {
        mOptionInfo[Index].NetIpGroupType = PXE_IPV4_TYPE;
      } else if ((IpType == MSG_IPv6_DP) && (StrStr(BootType, L"PXE") != NULL)) {
        mOptionInfo[Index].NetIpGroupType = PXE_IPV6_TYPE;
      } else if ((IpType == MSG_IPv4_DP) && (StrStr(BootType, L"HTTP") != NULL)) {
        mOptionInfo[Index].NetIpGroupType = HTTP_IPV4_TYPE;
      } else if ((IpType == MSG_IPv6_DP) && (StrStr(BootType, L"HTTP") != NULL)) {
        mOptionInfo[Index].NetIpGroupType = HTTP_IPV6_TYPE;
      } else {
        mOptionInfo[Index].NetIpGroupType = IP_GROUP_TYPE_MAX;
      }

      for (Count = 0; Count < COUNT_OF_IP_GROUP_TYPE; Count++) {
        if (mOptionInfo[Index].NetIpGroupType == mSortPolicy->NetIpGroupType[Count]) {
          break;
        }
      }
      if (Count < COUNT_OF_IP_GROUP_TYPE) {
        mOptionInfo[Index].NetIpGroupType = (UINT8)Count;
      } else {
        mOptionInfo[Index].NetIpGroupType = IP_GROUP_TYPE_MAX;
      }

      DevicePath = mOptionInfo[Index].DevicePath;
      Status = gBS->LocateDevicePath(&gEfiNetworkInterfaceIdentifierProtocolGuid_31, &DevicePath, &Handle);
      if (!EFI_ERROR(Status)) {
        Status = gBS->HandleProtocol(Handle, &gEfiNetworkInterfaceIdentifierProtocolGuid_31, (VOID**)&Nii);
        if (!EFI_ERROR(Status)) {
          mOptionInfo[Index].Function = mOptionInfo[Index].Function + Nii->IfNum;
        }
      }
      DevicePath = mOptionInfo[Index].DevicePath;
      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &DevicePath, &Handle);
      if (!EFI_ERROR(Status)) {
        Status = gBS->HandleProtocol(Handle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
        if (!EFI_ERROR(Status)) {
          PciIo->GetLocation(PciIo,
                             &Seg,
                             &Bus,
                             &Device,
                             &mOptionInfo[Index].Function);
          mOptionInfo[Index].IsPciNet = TRUE;
          mOptionInfo[Index].NetcardId = GetNetcardId(Index, Bus, Device, Seg);
          if (MaxNetcardId < mOptionInfo[Index].NetcardId) {
            MaxNetcardId = mOptionInfo[Index].NetcardId;
          }
        }
      }

      if (EFI_ERROR(Status)) {
        mOptionInfo[Index].IsPciNet = FALSE; // is not PCI netcard.
        mOptionInfo[Index].NetcardId = GetNetcardId(Index, 0, 0, 0);
      }
    }
    mOptionInfo[Index].DevicePriority = GetOptionPriority(Index);
  }

  //
  // Assign NetcardId and DevicePriority for iSCSI net options(Now NetIpGroupType is set)
  //
  MaxNetcardId++;
  for (Index = 0; Index < mOptionInfoCount; Index++) {
    if ((mOptionInfo[Index].GroupType == BM_MENU_TYPE_UEFI_PXE) && (!mOptionInfo[Index].IsNew) && mOptionInfo[Index].IsiSCSI) {
      mOptionInfo[Index].NetcardId = MaxNetcardId;
      mOptionInfo[Index].DevicePriority = GetOptionPriority(Index);
      MaxNetcardId++;
    }
  }
  for (Index = 0; Index < mOptionInfoCount; Index++) {
    if ((mOptionInfo[Index].GroupType == BM_MENU_TYPE_UEFI_PXE) && (mOptionInfo[Index].IsNew) && mOptionInfo[Index].IsiSCSI) {
      mOptionInfo[Index].NetcardId = MaxNetcardId;
      mOptionInfo[Index].DevicePriority = GetOptionPriority(Index);
      MaxNetcardId++;
    }
  }

  //
  // Assign ItemOrder to all UEFI HDD and net options
  //
  SetAndUpdateItemOrder(MaxOrderOfHdd);

NO_NEED_TO_ASSIGN_ITEMORDER:
  Status = GetVariable2 (
              BYO_UEFI_BOOT_GROUP_VAR_NAME,
              &gByoGlobalVariableGuid,
              (void**)&mUefiGroupOrder,
              &mGroupOrderSize
              );
  if(EFI_ERROR(Status)){
    mGroupOrderSize = sizeof(mDefaultUefiBootGroupOrder);
    mUefiGroupOrder = AllocateCopyPool(mGroupOrderSize, mDefaultUefiBootGroupOrder);
    Status = gRT->SetVariable (
                    BYO_UEFI_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    mGroupOrderSize,
                    mUefiGroupOrder
                    );
    ASSERT(!EFI_ERROR(Status));
  }

  //
  // sort all boot options and set to L"BootOrder"
  //
  if (CompareBootOption == NULL) {
    CompareBootOption = (SORT_COMPARE)ByoCompareBootOption;
  }
  if (SortNvOptions) {
    EfiBootManagerSortLoadOptionVariable(LoadOptionTypeBoot, (SORT_COMPARE)CompareBootOption);
  } else {
    ByoSortSpecifiedBootOptions(SortOptions, SortOptionsCount, (SORT_COMPARE)CompareBootOption);
  }

  if (mUefiGroupOrder != NULL) {
    FreePool(mUefiGroupOrder);
  }
  mGroupOrderSize = 0;

  if (mOptionInfo != NULL) {
    for (Index = 0; Index < mOptionInfoCount; Index++) {
      if (mOptionInfo[Index].DevicePath != NULL) {
        FreePool(mOptionInfo[Index].DevicePath);
      }
      if (mOptionInfo[Index].Description != NULL) {
        FreePool(mOptionInfo[Index].Description);
      }
    }
    FreePool(mOptionInfo);
    mOptionInfo = NULL;
  }
  mOptionInfoCount = 0;

  if (mNetInfo != NULL) {
    for (Index = 0; Index < mNetInfoCount; Index++) {
      if (mNetInfo[Index].DevicePath != NULL) {
        FreePool(mNetInfo[Index].DevicePath);
      }
    }
    FreePool(mNetInfo);
    mNetInfo = NULL;
  }
  mNetInfoIndex = 0;
  mNetInfoCount = MAX_VALUE_OF_COUNT;

  if (SortNvOptions) {
    EfiBootManagerFreeLoadOptions(Options, OptionsCount);
  }

  if (mSortPolicy != NULL) {
    FreePool(mSortPolicy);
    mSortPolicy = NULL;
  }
}
