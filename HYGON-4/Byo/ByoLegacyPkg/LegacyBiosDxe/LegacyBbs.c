/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>

All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "LegacyBiosInterface.h"
#include <IndustryStandard/Pci.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/UefiBbsRelationInfo.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <Protocol/UsbIo.h>
#include <IndustryStandard/Mbr.h>
#include <Protocol/DiskIo.h>
#include <IndustryStandard/Atapi.h>
#include <Library/ByoCommLib.h>

// Give floppy 3 states
// FLOPPY_PRESENT_WITH_MEDIA  = Floppy controller present and media is inserted
// FLOPPY_NOT_PRESENT = No floppy controller present
// FLOPPY_PRESENT_NO_MEDIA = Floppy controller present but no media inserted
//
#define FLOPPY_NOT_PRESENT           0
#define FLOPPY_PRESENT_WITH_MEDIA    1
#define FLOPPY_PRESENT_NO_MEDIA      2

#define PRIORITY_MASK                0x2

typedef enum {
  NOT_IDE_DEVICE = 0,
  IDE_MASTER = 1,
  IDE_SLAVE,
  ASSIGNED_DEVICE
} IDE_STATUS;

#pragma pack(1)

typedef struct _CSM16_SMMCALL_COMM {
    UINT8  CallId;
    UINT32 StatusCode;
    UINT32 Arguments[1];
} CSM16_SMMCALL_COMM;

typedef struct {
  UINTN       Index;
  UINT8       Status;
} IDE_DEVICES_INDEX;

typedef struct _UMS_INFO_ENTRY {
    UINT8       PDT;
    UINT8       Bus;
    UINT8       Device;
    UINT8       Function;
    UINT32      BlockSize;
    UINT32      Head;
    UINT32      Sector;
    UINT32      Cylinder;
    UINT32      LastBlock[2];
    UINT8       ManufacturerDescAStr[0x18];
    UINT64      UsbDp;
    UINT8       ProductDescAStr[0x20];
    UINT32      Handle[2];
    UINT8       USBHDDOriginalType;
    UINT8       FileType;
    UINT32      HiddenSector;
    UINT8       UsbDevParentAddr;
    UINT8       UsbDevParentPort;
    UINT8       Reserved[0x01];
} UMS_INFO_ENTRY;

typedef struct _UMS_INFO {
    UINT8             NumOfUms;
    UMS_INFO_ENTRY    UmsInfoEntry[8];
    UINT8             Reserved[0x16];
} UMS_INFO;

#pragma pack()

BBS_TABLE           *mBbsTable;
BOOLEAN             mBbsTableDoneFlag   = FALSE;
BOOLEAN             IsHaveMediaInFloppy = TRUE;

#define MAXIMUM_OF_HDD  20
#define MAXIMUM_OF_USB  40
UEFI_BBS_RELATION_INFO_PROTOCOL  gHBtable;

#define INCREMENT_OF_COUNT 10

BOOLEAN
IsBbsEntryValid (
  IN BBS_TABLE   *BbsEntry
  );

/**
  Checks the state of the floppy and if media is inserted.

  This routine checks the state of the floppy and if media is inserted.
  There are 3 cases:
  No floppy present         - Set BBS entry to ignore
  Floppy present & no media - Set BBS entry to lowest priority. We cannot
  set it to ignore since 16-bit CSM will
  indicate no floppy and thus drive A: is
  unusable. CSM-16 will not try floppy since
  lowest priority and thus not incur boot
  time penalty.
  Floppy present & media    - Set BBS entry to some priority.

  @return  State of floppy media

**/
UINT8
HasMediaInFloppy (
  VOID
  )
{
  EFI_STATUS                            Status;
  UINTN                                 HandleCount;
  EFI_HANDLE                            *HandleBuffer;
  UINTN                                 Index;
  EFI_ISA_IO_PROTOCOL                   *IsaIo;
  EFI_BLOCK_IO_PROTOCOL                 *BlkIo;

  HandleBuffer  = NULL;
  HandleCount   = 0;

  gBS->LocateHandleBuffer (
        ByProtocol,
        &gEfiIsaIoProtocolGuid,
        NULL,
        &HandleCount,
        &HandleBuffer
        );

  //
  // If don't find any ISA/IO protocol assume no floppy. Need for floppy
  // free system
  //
  if (HandleCount == 0) {
    return FLOPPY_NOT_PRESENT;
  }

  ASSERT (HandleBuffer != NULL);

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiIsaIoProtocolGuid,
                    (VOID **) &IsaIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    if (IsaIo->ResourceList->Device.HID != EISA_PNP_ID (0x604)) {
      continue;
    }
    //
    // Update blockio in case the floppy is inserted in during BdsTimeout
    //
    Status = gBS->DisconnectController (HandleBuffer[Index], NULL, NULL);

    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->ConnectController (HandleBuffer[Index], NULL, NULL, TRUE);

    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlkIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    if (BlkIo->Media->MediaPresent) {
      FreePool (HandleBuffer);
      return FLOPPY_PRESENT_WITH_MEDIA;
    } else {
      FreePool (HandleBuffer);
      return FLOPPY_PRESENT_NO_MEDIA;
    }
  }

  FreePool (HandleBuffer);

  return FLOPPY_NOT_PRESENT;

}


/**
  Complete build of BBS TABLE.

  @param  Private                 Legacy BIOS Instance data
  @param  BbsTable                BBS Table passed to 16-bit code

  @retval EFI_SUCCESS             Removable media not present

**/
EFI_STATUS
LegacyBiosBuildBbs (
  IN  LEGACY_BIOS_INSTANCE      *Private,
  IN  BBS_TABLE                 *BbsTable
  )
{
  UINTN     BbsIndex;
  HDD_INFO  *HddInfo;
  UINTN     HddIndex;
  UINTN     Index;

  //
  // First entry is floppy.
  // Next 2*MAX_IDE_CONTROLLER entries are for onboard IDE.
  // Next n entries are filled in after each ROM is dispatched.
  //   Entry filled in if follow BBS spec. See LegacyPci.c
  // Next entries are for non-BBS compliant ROMS. They are filled in by
  //   16-bit code during Legacy16UpdateBbs invocation. Final BootPriority
  //   occurs after that invocation.
  //
  // Floppy
  // Set default state.
  //
  IsHaveMediaInFloppy = HasMediaInFloppy ();
  if (IsHaveMediaInFloppy == FLOPPY_PRESENT_WITH_MEDIA) {
    BbsTable[0].BootPriority = BBS_UNPRIORITIZED_ENTRY;
  } else {
    if (IsHaveMediaInFloppy == FLOPPY_PRESENT_NO_MEDIA) {
      BbsTable[0].BootPriority = BBS_LOWEST_PRIORITY;
    } else {
      BbsTable[0].BootPriority = BBS_IGNORE_ENTRY;
    }
  }

  BbsTable[0].Bus                       = 0xff;
  BbsTable[0].Device                    = 0xff;
  BbsTable[0].Function                  = 0xff;
  BbsTable[0].DeviceType                = BBS_FLOPPY;
  BbsTable[0].Class                     = 01;
  BbsTable[0].SubClass                  = 02;
  BbsTable[0].StatusFlags.OldPosition   = 0;
  BbsTable[0].StatusFlags.Reserved1     = 0;
  BbsTable[0].StatusFlags.Enabled       = 0;
  BbsTable[0].StatusFlags.Failed        = 0;
  BbsTable[0].StatusFlags.MediaPresent  = 0;
  BbsTable[0].StatusFlags.Reserved2     = 0;

  //
  // Onboard HDD - Note Each HDD controller controls 2 drives
  //               Master & Slave
  //
  HddInfo = &Private->IntThunk->EfiToLegacy16BootTable.HddInfo[0];
  //
  // Get IDE Drive Info
  //
  LegacyBiosBuildIdeData (Private, &HddInfo, 0);

  for (HddIndex = 0; HddIndex < MAX_IDE_CONTROLLER; HddIndex++) {

    BbsIndex = HddIndex * 2 + 1;
    for (Index = 0; Index < 2; ++Index) {

      BbsTable[BbsIndex + Index].Bus                      = HddInfo[HddIndex].Bus;
      BbsTable[BbsIndex + Index].Device                   = HddInfo[HddIndex].Device;
      BbsTable[BbsIndex + Index].Function                 = HddInfo[HddIndex].Function;
      BbsTable[BbsIndex + Index].Class                    = 01;
      BbsTable[BbsIndex + Index].SubClass                 = 01;
      BbsTable[BbsIndex + Index].StatusFlags.OldPosition  = 0;
      BbsTable[BbsIndex + Index].StatusFlags.Reserved1    = 0;
      BbsTable[BbsIndex + Index].StatusFlags.Enabled      = 0;
      BbsTable[BbsIndex + Index].StatusFlags.Failed       = 0;
      BbsTable[BbsIndex + Index].StatusFlags.MediaPresent = 0;
      BbsTable[BbsIndex + Index].StatusFlags.Reserved2    = 0;

      //
      // If no controller found or no device found set to ignore
      // else set to unprioritized and set device type
      //
      if (HddInfo[HddIndex].CommandBaseAddress == 0) {
        BbsTable[BbsIndex + Index].BootPriority = BBS_IGNORE_ENTRY;
      } else {
        if (Index == 0) {
          if ((HddInfo[HddIndex].Status & (HDD_MASTER_IDE | HDD_MASTER_ATAPI_CDROM | HDD_MASTER_ATAPI_ZIPDISK)) != 0) {
            BbsTable[BbsIndex + Index].BootPriority = BBS_UNPRIORITIZED_ENTRY;
            if ((HddInfo[HddIndex].Status & HDD_MASTER_IDE) != 0) {
              BbsTable[BbsIndex + Index].DeviceType = BBS_HARDDISK;
            } else if ((HddInfo[HddIndex].Status & HDD_MASTER_ATAPI_CDROM) != 0) {
              BbsTable[BbsIndex + Index].DeviceType = BBS_CDROM;
            } else {
              //
              // for ZIPDISK
              //
              BbsTable[BbsIndex + Index].DeviceType = BBS_HARDDISK;
            }
          } else {
            BbsTable[BbsIndex + Index].BootPriority = BBS_IGNORE_ENTRY;
          }
        } else {
          if ((HddInfo[HddIndex].Status & (HDD_SLAVE_IDE | HDD_SLAVE_ATAPI_CDROM | HDD_SLAVE_ATAPI_ZIPDISK)) != 0) {
            BbsTable[BbsIndex + Index].BootPriority = BBS_UNPRIORITIZED_ENTRY;
            if ((HddInfo[HddIndex].Status & HDD_SLAVE_IDE) != 0) {
              BbsTable[BbsIndex + Index].DeviceType = BBS_HARDDISK;
            } else if ((HddInfo[HddIndex].Status & HDD_SLAVE_ATAPI_CDROM) != 0) {
              BbsTable[BbsIndex + Index].DeviceType = BBS_CDROM;
            } else {
              //
              // for ZIPDISK
              //
              BbsTable[BbsIndex + Index].DeviceType = BBS_HARDDISK;
            }
          } else {
            BbsTable[BbsIndex + Index].BootPriority = BBS_IGNORE_ENTRY;
          }
        }
      }
    }
  }

  return EFI_SUCCESS;

}

BOOLEAN
LibGetSataPortInfo (
  IN  EFI_BOOT_SERVICES         *BS,
  IN  EFI_HANDLE                Handle,
  OUT UINTN                     *HostIndex,
  OUT UINTN                     *PortIndex,
  OUT UINTN                     *PhysicPortIndex OPTIONAL
  )
{
  EFI_STATUS                    Status;
  UINTN                         AtaIndex;
  UINT16                        PlatSataHostIndex = 0;
  PLAT_HOST_INFO_PROTOCOL       *PlatHostInfo;
  BOOLEAN                       rc = FALSE;


  Status = BS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "PlatHostInfo not found\n"));
    goto ProcExit;
  }

  AtaIndex = PlatHostInfo->GetSataPortIndex(Handle);
  if (AtaIndex == 0xFFFF) {
    DEBUG((EFI_D_INFO, "GetSataPortIndex Error\n"));
    goto ProcExit;
  }

  PlatSataHostIndex = PlatHostInfo->GetSataHostIndex(Handle);
  *HostIndex = PlatSataHostIndex;

  if (!PcdGetBool(PcdAhciOrPortNameIndexBase0)) {
    AtaIndex++;
  }
  *PortIndex = AtaIndex;
  *PhysicPortIndex = AtaIndex;
  rc = TRUE;

ProcExit:
  return rc;
}

VOID
UpdateHddRelation (
  UefiBbsHddRL   *HddRelation,
  UINTN          HddIndex,
  EFI_PCI_IO_PROTOCOL  **AddedPciIo
  )
{
  UINTN                     Index, Count, FirstIndex;
  UINTN                     LagerMinIndex;
  UINT32                    LagerMin;
  BOOLEAN                   LagerMinInit = FALSE;
  UefiBbsHddRL              TempHddRl;

  for (Index = 0; Index < HddIndex; Index++) {
    if (AddedPciIo[Index] == AddedPciIo[HddIndex]) {
      FirstIndex = Index;
      break;
    }
  }
  if (FirstIndex == HddIndex) {
    DEBUG((EFI_D_INFO, "No need to update the order of SATA devices in HddRelation %a\n", __FUNCTION__));
    return;
  }

  for (Count = FirstIndex; Count < HddIndex; Count++) {
    if (HddRelation[Count].Priority > HddRelation[HddIndex].Priority) {
      if (!LagerMinInit) {
        LagerMinInit = TRUE;
        LagerMin = HddRelation[Count].Priority;
        LagerMinIndex = Count;
        continue;
      }

      if (HddRelation[Count].Priority < LagerMin) {
        LagerMin = HddRelation[Count].Priority;
        LagerMinIndex = Count;
      }
    }
  }
  if (!LagerMinInit) {
    return;
  }

  CopyMem(&TempHddRl, &HddRelation[HddIndex], sizeof(UefiBbsHddRL));
  for (Index = HddIndex - 1; Index >= LagerMinIndex; Index--) {
    CopyMem(&HddRelation[Index + 1], &HddRelation[Index], sizeof(UefiBbsHddRL));
    if (Index == 0) {
      break;
    }
  }
  CopyMem(&HddRelation[LagerMinIndex], &TempHddRl, sizeof(UefiBbsHddRL));
}

STATIC
UINTN
GetUefiSataIndex(
  OUT UefiBbsHddRL   *HddRelation,
  IN  UINTN          HddCount
  )
{
  EFI_STATUS                Status;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer;
  UINTN                     Index;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *OriginDevicePath;
  EFI_HANDLE                DeviceHandle;
  UINTN                     Seg, Bus, Dev, Func;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  BYO_DISKINFO_PROTOCOL     *ByoDiskInfo;
  CHAR8                     Sn[MAXSIZE_OF_SN];
  UINTN                     Size;
  UINTN                     HddIndex = 0;
  UINTN                     PlatSataHostIndex = 0xFFFF;
  UINTN                     AtaIndex, PhysicPortIndex;
  UINT8                     ClassCode[3];
  EFI_PCI_IO_PROTOCOL       **HddPciIo = NULL;
  UINTN                     Maximum = 20;
  UINTN                     HddPciIoCount = 0;
  UINTN                     PciIndex;
  EFI_BLOCK_IO_PROTOCOL     *BlockIo;
  EFI_DISK_INFO_PROTOCOL    *DiskInfo;
  ATA_IDENTIFY_DATA         IdentifyData;
  UINT32                    BufferSize;
  IDE_STATUS                IdeStatus;
  ATAPI_DEVICE_PATH         *IdeDp = NULL;
  SATA_DEVICE_PATH          *SataDp;
  EFI_PCI_IO_PROTOCOL       **AddedPciIo = NULL;
  UINT32                    Priority;

  if (HddRelation == NULL) {
    return 0;
  }

  //
  // Find out all PciIo of PCI_CLASS_MASS_STORAGE in order.
  //
  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiPciIoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
  if (EFI_ERROR(Status)) {
    return 0;
  }
  DEBUG((EFI_D_INFO,"GetPciIo %r HandleCount %x\n", Status, HandleCount));

  HddPciIo = (EFI_PCI_IO_PROTOCOL **)AllocateZeroPool(Maximum * sizeof(EFI_PCI_IO_PROTOCOL *));
  ASSERT(HddPciIo != NULL);

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                        HandleBuffer[Index],
                        &gEfiPciIoProtocolGuid,
                        &PciIo
                        );
    if (EFI_ERROR(Status)) {
      continue;
    }

    PciIo->Pci.Read (
               PciIo,
               EfiPciIoWidthUint8,
               PCI_CLASSCODE_OFFSET,
               sizeof(ClassCode) / sizeof(ClassCode[0]),
               ClassCode
               );
    if (ClassCode[2] == PCI_CLASS_MASS_STORAGE) {
      if (HddPciIoCount == Maximum) {
        HddPciIo = (EFI_PCI_IO_PROTOCOL **)ReallocatePool(Maximum, Maximum + INCREMENT_OF_COUNT, HddPciIo);
        ASSERT(HddPciIo != NULL);
        Maximum = Maximum + INCREMENT_OF_COUNT;
      }
      HddPciIo[HddPciIoCount] = PciIo;
      HddPciIoCount++;
    }
  }
  if (HandleBuffer != NULL) {
    FreePool(HandleBuffer);
  }

  //
  // Save information of HDD according to the order of PciIo.
  //
  AddedPciIo = (EFI_PCI_IO_PROTOCOL **)AllocateZeroPool(HddCount * sizeof(EFI_PCI_IO_PROTOCOL *));
  ASSERT(AddedPciIo != NULL);
  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiDiskInfoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &HandleBuffer
                    );
  if (EFI_ERROR(Status)) {
    return 0;
  }
  DEBUG((EFI_D_INFO,"GetDiskInfo %r HandleCount %x\n",Status,HandleCount));
  for (PciIndex = 0; PciIndex < HddPciIoCount; PciIndex++) {
    for (Index = 0; Index < HandleCount; Index++) {
      if (HddIndex > HddCount) {
        DEBUG((EFI_D_ERROR," Too many storage devices cannot be processed!"));
        break;
      }

      IdeStatus = NOT_IDE_DEVICE;
      SataDp = NULL;
      Priority = 0xFFFF;
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID *) &DevicePath
                      );
      ASSERT_EFI_ERROR(Status);
      OriginDevicePath = DevicePath;

      Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &DevicePath, &DeviceHandle);
      if (EFI_ERROR(Status)) {
        continue;
      }

      Status = gBS->HandleProtocol (
                          DeviceHandle,
                          &gEfiPciIoProtocolGuid,
                          &PciIo
                          );
      if (EFI_ERROR(Status)) {
        continue;
      }

      if (PciIo != HddPciIo[PciIndex]) {
        continue;
      }

      Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiBlockIoProtocolGuid, (VOID**)&BlockIo);
      if (EFI_ERROR(Status)) {
        continue;
      }

      DEBUG((EFI_D_ERROR," EfiPciIoProtocolGuid get bus dev"));
      PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
      DEBUG((EFI_D_INFO, "(%X,%X,%X) %X\n", Bus, Dev, Func, HandleBuffer[Index]));

      while (!IsDevicePathEndType (DevicePath)) {
        if ((DevicePath->Type == MESSAGING_DEVICE_PATH) && ((DevicePath->SubType == MSG_SATA_DP)
        || (DevicePath->SubType == MSG_NVME_NAMESPACE_DP) || (DevicePath->SubType == MSG_ATAPI_DP))) {
          break;
        }
        DevicePath = NextDevicePathNode (DevicePath);
      }
      if (IsDevicePathEnd(DevicePath)) {
        continue;
      } else {
        if (DevicePath->SubType == MSG_ATAPI_DP) { // is IDE mode
          IdeDp = (ATAPI_DEVICE_PATH *)DevicePath;
          Priority = ((UINT16)(IdeDp->PrimarySecondary + 1) << 8) + (IdeDp->SlaveMaster);
          if (IdeDp->SlaveMaster == 0) {
            IdeStatus = IDE_MASTER;
          } else {
            IdeStatus = IDE_SLAVE;
          }
        } else if (DevicePath->SubType == MSG_SATA_DP) { // is AHCI mode
          SataDp = (SATA_DEVICE_PATH *)DevicePath;
          Priority = ((UINT32)PRIORITY_MASK << 16) + SataDp->HBAPortNumber;
        }
      }

      DEBUG((EFI_D_INFO,"This are Nvme or M2 or HDD devices!\n"));
      Status = gBS->HandleProtocol(
                        HandleBuffer[Index],
                        &gByoDiskInfoProtocolGuid,
                        (VOID**)&ByoDiskInfo
                        );
      if (!EFI_ERROR(Status)) {
        Size = sizeof(Sn);
        Status = ByoDiskInfo->GetSn(ByoDiskInfo, Sn, &Size);
        CopyMem(HddRelation[HddIndex].SerialNo, Sn, MAXSIZE_OF_SN);
        DEBUG((EFI_D_INFO,"IdentifyData %r Sn %a\n",Status,Sn));
      }else {
        DevicePath = OriginDevicePath;
        Status = gBS->LocateDevicePath(
              &gEfiDiskInfoProtocolGuid,
              &DevicePath,
              &DeviceHandle
              );
        if (EFI_ERROR(Status)) {
          return Status;
        }
        Status = gBS->HandleProtocol(
                    DeviceHandle,
                    &gEfiDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );
        if (EFI_ERROR(Status)) {
          return Status;
        }
        BufferSize = sizeof(ATA_IDENTIFY_DATA);
        Status = DiskInfo->Identify (
                        DiskInfo,
                        &IdentifyData,
                        &BufferSize
                        );
        if (EFI_ERROR(Status)) {
          return Status;
        }
        if ((!(IdentifyData.config & ATAPI_DEVICE)) || (IdentifyData.config == 0x848A)) {
          if (Sn != NULL) {
            ZeroMem(Sn, MAXSIZE_OF_SN);
            CopyMem(Sn, IdentifyData.SerialNo, sizeof(IdentifyData.SerialNo));
            SwapWordArray((UINT8*)Sn, MAXSIZE_OF_SN - 1);
            Sn[MAXSIZE_OF_SN - 1] = '\0';
            AsciiDeleteExtraSpaces(Sn);
            CopyMem(HddRelation[HddIndex].SerialNo, Sn, MAXSIZE_OF_SN);
            DEBUG((EFI_D_INFO,"IdentifyData %r Sn %a\n",Status,Sn));
          }
        }
      }

      if (!LibGetSataPortInfo(gBS, HandleBuffer[Index], &PlatSataHostIndex, &AtaIndex, &PhysicPortIndex)) {
        DEBUG((EFI_D_INFO,"LibGetSataPortInfo false!\n"));
        PhysicPortIndex = 0;
      }

      DEBUG((EFI_D_INFO,"PlatSataHostIndex %x\n",PlatSataHostIndex));
      if ((PlatSataHostIndex != 0xFFFF) && (PlatSataHostIndex != 0x8000)) {
        PlatSataHostIndex = PlatSataHostIndex + 1;
      }

      HddRelation[HddIndex].HddPortIndex = PhysicPortIndex;
      DEBUG((EFI_D_INFO,"This is %x PhysicPortIndex.\n",PhysicPortIndex));

      AddedPciIo[HddIndex] = HddPciIo[PciIndex];
      HddRelation[HddIndex].UefiHddIndex = HddIndex;
      HddRelation[HddIndex].SataHostIndex = PlatSataHostIndex;
      HddRelation[HddIndex].Bus = Bus;
      HddRelation[HddIndex].Device = Dev;
      HddRelation[HddIndex].Function = Func;
      HddRelation[HddIndex].HddDevicePath = OriginDevicePath;
      HddRelation[HddIndex].Priority = Priority;
      HddRelation[HddIndex].Status = IdeStatus;

      //
      // Update the order of AHCI SATA devices in HddRelation according to HBAPortNumber
      //
      if (((SataDp != NULL) || (IdeStatus == IDE_MASTER) || (IdeStatus == IDE_SLAVE)) && (HddIndex > 0)) {
        UpdateHddRelation(HddRelation, HddIndex, AddedPciIo);
      }
      HddIndex++;
    }
  }

  if (HandleBuffer != NULL) {
    FreePool(HandleBuffer);
  }

  if (AddedPciIo != NULL) {
    FreePool(AddedPciIo);
  }

  if (HddPciIo != NULL) {
    FreePool(HddPciIo);
  }

  return HddIndex;
}

STATIC
UINTN
GetUefiUsbIndex(
  OUT UefiBbsUsbRL   *UsbRelation,
  IN  UINTN          UsbCount
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     UsbIndex;
  UINTN                     HandleCount;
  EFI_HANDLE                *Handles;
  EFI_BLOCK_IO_PROTOCOL     *BlkIo;
  EFI_USB_IO_PROTOCOL       *UsbIo;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;

  if (UsbRelation == NULL) {
    return 0;
  }

  Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiBlockIoProtocolGuid,
                    NULL,
                    &HandleCount,
                    &Handles
                    );
  if (EFI_ERROR(Status)) {
    return 0;
  }

  UsbIndex = 0;
  for (Index =0; Index < HandleCount; Index++) {
    if (Index > UsbCount) {
      DEBUG((EFI_D_ERROR," Too many Usb devices cannot be processed!"));
      break;
    }
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiBlockIoProtocolGuid,
                    (VOID **) &BlkIo
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    if (BlkIo->Media->LogicalPartition) { // Skip the logical partitions
      continue;
    }

    if (BlkIo->Media->RemovableMedia == 0) { // Skip the fixed block io
      continue;
    }

    Status = gBS->HandleProtocol (
                  Handles[Index],
                  &gEfiUsbIoProtocolGuid,
                  (VOID **) &UsbIo
                  );
    if (EFI_ERROR(Status)) {
      continue;
    }

    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID *) &DevicePath
                    );
    if (!EFI_ERROR(Status)) {
      UsbRelation[UsbIndex].UsbDevicePath = DevicePath;
      UsbIndex++;
    }
  }

  return UsbIndex;
}

//
// Update BbsIndex of HddBbsRelation and UsbBbsRelation according to
// BbsTable(BBS_FLOPPY > BBS_HARDDISK > BBS_CDROM > BBS_EMBED_NETWORK > BBS_BEV_DEVICE)
//
STATIC
VOID
UpdateBbsIndex(
  IN OUT BBS_TABLE   *BbsTable,
  IN UINT16          BbsCount,
  IN UefiBbsHddRL    *HddBbsRelation,
  IN UINTN           HddCount,
  IN UefiBbsUsbRL    *UsbBbsRelation,
  IN UINTN           *UsbCount
  )
{
  UINT16                Index;
  UINT16                BbsIndex;
  UINTN                 BbsUsbIndex = 0;
  UINTN                 Count;
  UMS_INFO              *mUmsInfo;
  BOOLEAN               IdeDeviceExist = FALSE;

  mUmsInfo = (UMS_INFO *)(UINTN)0xEFC80; // CSM16 places the USB device's info in this address


  if ((BbsTable == NULL) || (HddBbsRelation == NULL) || (UsbBbsRelation == NULL)) {
    return;
  }
  for (Index = 0; Index < HddCount; Index++) {
    if ((HddBbsRelation[Index].Status == IDE_MASTER) || (HddBbsRelation[Index].Status == IDE_SLAVE)) {
      IdeDeviceExist = TRUE;
      break;
    }
  }

  //
  // Update SATA HDD or NVME SSD or USB device first
  //
  for (BbsIndex = 0; BbsIndex < BbsCount; BbsIndex++) {
    // DEBUG ((EFI_D_INFO, " BbsIndex : %02X  BDF :  %X %X %X Class: %X SubClass : %X DType: %X USBIndex : %X\n", \
    //                       BbsIndex, BbsTable[BbsIndex].Bus, BbsTable[BbsIndex].Device, BbsTable[BbsIndex].Function ,BbsTable[BbsIndex].Class, BbsTable[BbsIndex].SubClass, BbsTable[BbsIndex].DeviceType, BbsUsbIndex));
    if ((BbsTable[BbsIndex].BootPriority == BBS_IGNORE_ENTRY) || (BbsTable[BbsIndex].BootPriority == BBS_DO_NOT_BOOT_FROM)
      || (BbsTable[BbsIndex].BootPriority == BBS_LOWEST_PRIORITY)) {
      continue;
    }

    if ((BbsTable[BbsIndex].Class == 0x01) && ((BbsTable[BbsIndex].SubClass == 0x06)
      || (BbsTable[BbsIndex].SubClass == 0x08))) { // AHCI SATA devices and NVME SSD
      for (Count = 0; Count < HddCount; Count++) {
        if (HddBbsRelation[Count].Status == NOT_IDE_DEVICE) {
          HddBbsRelation[Count].BbsIndex = BbsIndex;
          HddBbsRelation[Count].Status = ASSIGNED_DEVICE;
          break;
        }
      }

    } else if ((BbsTable[BbsIndex].Class == 0xC) && (BbsTable[BbsIndex].SubClass == 0x3)) { // is usb
      UsbBbsRelation[BbsUsbIndex].BbsIndex = BbsIndex;
      UsbBbsRelation[BbsUsbIndex].UsbDevicePath =  (EFI_DEVICE_PATH_PROTOCOL *)mUmsInfo->UmsInfoEntry[(UINT16)BbsTable[BbsIndex].IBV1].UsbDp;
      BbsUsbIndex++;
      *UsbCount = BbsUsbIndex;
    }
  }

  //
  // Update IDE devices
  //
  if (IdeDeviceExist) {
    //
    // Update IDE master devices
    //
    for (BbsIndex = 1; BbsIndex < 17; BbsIndex++) { // the position of IDE BbsEntries in BbsTable is from 1 to 16
      if ((BbsTable[BbsIndex].BootPriority == BBS_IGNORE_ENTRY) || (BbsTable[BbsIndex].BootPriority == BBS_DO_NOT_BOOT_FROM)
        || (BbsTable[BbsIndex].BootPriority == BBS_LOWEST_PRIORITY)) {
        continue;
      }
      if ((BbsTable[BbsIndex].Class == 0x01) && (BbsTable[BbsIndex].SubClass == 0x01)) {
        if (BbsIndex%2 == 0) { // is IDE slave
          continue;
        }
        for (Count = 0; Count < HddCount; Count++) {
          if (HddBbsRelation[Count].Status == IDE_MASTER) {
            HddBbsRelation[Count].BbsIndex = BbsIndex;
            HddBbsRelation[Count].Status = ASSIGNED_DEVICE;
            break;
          }
        }
      }
    }

    //
    // Update IDE slave devices
    //
    for (BbsIndex = 1; BbsIndex < 17; BbsIndex++) { // the position of IDE BbsEntries in BbsTable is from 1 to 16
      if ((BbsTable[BbsIndex].BootPriority == BBS_IGNORE_ENTRY) || (BbsTable[BbsIndex].BootPriority == BBS_DO_NOT_BOOT_FROM)
        || (BbsTable[BbsIndex].BootPriority == BBS_LOWEST_PRIORITY)) {
        continue;
      }
      if ((BbsTable[BbsIndex].Class == 0x01) && (BbsTable[BbsIndex].SubClass == 0x01)) {
        if (BbsIndex%2 != 0) { // is IDE master
          continue;
        }
        for (Count = 0; Count < HddCount; Count++) {
          if (HddBbsRelation[Count].Status == IDE_SLAVE) {
            HddBbsRelation[Count].BbsIndex = BbsIndex;
            HddBbsRelation[Count].Status = ASSIGNED_DEVICE;
            break;
          }
        }
      }
    }
  }
}

STATIC
EFI_STATUS
UefiBbsRelationInfoInstall (
  IN UefiBbsHddRL *UefiBbsHddTable,
  IN UefiBbsUsbRL *UsbBbsRelation,
  IN UINTN LocalUefiSataHddIndex,
  IN UINTN LocalUefiUsbCount
  )
{
  EFI_STATUS     Status;
  EFI_HANDLE     Handle = NULL;
  UINTN          Index = 0;
  UEFI_BBS_RELATION_INFO_PROTOCOL	*UefiBbsInfo;
  CHAR16         *DevicePathStr;

  Status = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &UefiBbsInfo);
  if (EFI_ERROR(Status)) {
    gHBtable.UefiBbsHddTable = UefiBbsHddTable;
    gHBtable.UefiBbsUsbTable = UsbBbsRelation;
    gHBtable.UefiSataHddCount = LocalUefiSataHddIndex;
    gHBtable.UefiUsbCount = LocalUefiUsbCount;
    DEBUG((EFI_D_INFO, "HBtable->UefiSataHddCount %x\n", gHBtable.UefiSataHddCount));
    Status = gBS->InstallProtocolInterface (
                    &Handle,
                    &gUefiBbsRelationProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gHBtable
                    );
    ASSERT_EFI_ERROR(Status);
  } else {
    //
    // Update UefiBbsHddTable
    //
    if (UefiBbsInfo->UefiBbsHddTable != NULL) {
      FreePool(UefiBbsInfo->UefiBbsHddTable);
    }

    if (UefiBbsInfo->UefiBbsUsbTable != NULL) {
      FreePool(UefiBbsInfo->UefiBbsUsbTable);
    }

    UefiBbsInfo->UefiBbsHddTable = UefiBbsHddTable;
    UefiBbsInfo->UefiBbsUsbTable = UsbBbsRelation;
    UefiBbsInfo->UefiSataHddCount = LocalUefiSataHddIndex;
    UefiBbsInfo->UefiUsbCount = LocalUefiUsbCount;
  }

  //
  // Dump UEFI BBS Table Info
  //
  for (Index = 0; Index < LocalUefiSataHddIndex; Index ++) {
    DevicePathStr = ConvertDevicePathToText (UefiBbsHddTable[Index].HddDevicePath, FALSE, FALSE);
    DEBUG ((DEBUG_INFO, "UEFI HDD BBS INFO HddIndex %d BbsIndex %d DevicePath %s\n", UefiBbsHddTable[Index].UefiHddIndex, UefiBbsHddTable[Index].BbsIndex, DevicePathStr));
    if (DevicePathStr != NULL) {
      FreePool (DevicePathStr);
    }
  }

  for (Index = 0; Index < LocalUefiUsbCount; Index ++) {
    DevicePathStr = ConvertDevicePathToText (UsbBbsRelation[Index].UsbDevicePath, FALSE, FALSE);
    DEBUG ((DEBUG_INFO, "UEFI USB BBS INFO BbsIndex %d DevicePath %s\n", UsbBbsRelation[Index].BbsIndex, DevicePathStr));
    if (DevicePathStr != NULL) {
      FreePool (DevicePathStr);
    }
  }

  return Status;
}

/*
Nvme:
class:	  PCI_CLASS_MASS_STORAGE:1
subclass:  PCI_CLASS_MASS_STORAGE_NVM:8

HDD:
class:	  PCI_CLASS_MASS_STORAGE:1
subclass:  PCI_CLASS_MASS_STORAGE_IDE:0x01
               PCI_CLASS_MASS_STORAGE_SATADPA:0x06
*/
BOOLEAN
IsBbsEntryValid (
  IN BBS_TABLE   *BbsEntry
  )
{
  switch (BbsEntry->BootPriority) {
    case BBS_IGNORE_ENTRY:
    case BBS_DO_NOT_BOOT_FROM:
    case BBS_LOWEST_PRIORITY:
      return FALSE;
    default:
      break;
  }

  if ((BbsEntry->Class == 0x01) && ((BbsEntry->SubClass == 0x08)
    || (BbsEntry->SubClass != 0x01) || (BbsEntry->SubClass != 0x06))) { // is Hdd
    return TRUE;
  }

  if ((BbsEntry->Class == 0xC) && (BbsEntry->SubClass == 0x3)) { // is USB
    return TRUE;
  }

  return FALSE;
}

BOOLEAN
IsMbrExist (
  EFI_DEVICE_PATH_PROTOCOL            *DevicePath
  )
{
  EFI_STATUS                      Status;
  EFI_HANDLE                      Handle;
  EFI_DISK_IO_PROTOCOL            *DiskIo;
  EFI_BLOCK_IO_PROTOCOL           *BlkIo;
  VOID                            *Buffer;
  MASTER_BOOT_RECORD              *Mbr;
  UINTN                           Index;
  EFI_DEVICE_PATH_PROTOCOL        *TempDevicePath;
  UINT32                          MediaId;

  if (DevicePath == NULL) {
    return TRUE;
  }

  TempDevicePath = DevicePath;
  Status = gBS->LocateDevicePath (&gEfiBlockIoProtocolGuid, &TempDevicePath, &Handle);
  if (EFI_ERROR(Status)) {
    return TRUE;
  }

  Status = gBS->HandleProtocol(
                    Handle,
                    &gEfiBlockIoProtocolGuid,
                    (VOID **)&BlkIo
                    );
  if (EFI_ERROR(Status)) {
    return TRUE;
  }

  MediaId = BlkIo->Media->MediaId;
  Status = gBS->LocateDevicePath (&gEfiDiskIoProtocolGuid, &DevicePath, &Handle);
  if (EFI_ERROR(Status)) {
    return TRUE;
  }

  Status = gBS->HandleProtocol(
                    Handle,
                    &gEfiDiskIoProtocolGuid,
                    (VOID **)&DiskIo
                    );
  if (EFI_ERROR(Status)) {
    return TRUE;
  }

  Buffer = AllocateZeroPool(0x200); // 512Byte
  ASSERT(Buffer != NULL);

  Status = DiskIo->ReadDisk (
                    DiskIo,
                    MediaId,
                    0,
                    0x200,  // 512Byte
                    Buffer
                    );
  if (!EFI_ERROR(Status)) {
    Mbr = (MASTER_BOOT_RECORD *)Buffer;
    if ((Mbr->Signature != 0xAA55)) {
      FreePool(Buffer);
      return FALSE;
    }

    for (Index = 0; Index < MAX_MBR_PARTITIONS; Index++) {
      if ((Mbr->Partition[Index].BootIndicator == 0x80)
        || ((Mbr->Partition[Index].BootIndicator == 0) && (Mbr->Partition[Index].OSIndicator == 0x83))) { // create legacy linux boot option
        break;
      }
    }

    if (Index == MAX_MBR_PARTITIONS) {
      FreePool(Buffer);
      return FALSE;
    }
  } else if ((Status == EFI_NO_MEDIA) && (BlkIo->Media->BlockSize == 2048)) { // is only SATA DVD without CD
    FreePool(Buffer);
    return FALSE;
  }

  FreePool(Buffer);
  return TRUE;
}

VOID
CheckAndMarkInvalidBbs (
  UEFI_BBS_RELATION_INFO_PROTOCOL  *UefiBbsInfo,
  BBS_TABLE                        *BbsTable,
  UINTN                            BbsCount
  )
{
  UINTN                             Index;
  UINTN                             Count;
  EFI_DEVICE_PATH_PROTOCOL          *DevicePath = NULL;
  UefiBbsHddRL                      *UefiBbsHddTable;
  UefiBbsUsbRL                      *UefiBbsUsbTable;
  UINTN                             UefiSataHddCount;
  UINTN                             UefiUsbCount;
  BOOLEAN                           MatchedDevice = FALSE;

  if ((UefiBbsInfo == NULL) || (BbsTable == NULL) || (BbsCount == 0)
    || ((UefiBbsInfo->UefiBbsHddTable == NULL) && (UefiBbsInfo->UefiBbsUsbTable == NULL))) {
    return;
  }

  UefiBbsHddTable = UefiBbsInfo->UefiBbsHddTable;
  UefiSataHddCount = UefiBbsInfo->UefiSataHddCount;
  UefiBbsUsbTable = UefiBbsInfo->UefiBbsUsbTable;
  UefiUsbCount = UefiBbsInfo->UefiUsbCount;

  for (Count = 0; Count < BbsCount; Count++) {
    if (!IsBbsEntryValid(&BbsTable[Count])) {
      continue;
    }

    //
    // Skip CD/DVD ROM devices
    //
    if ((BbsTable[Count].DeviceType == BBS_CDROM) || ((BbsTable[Count].DeviceType == BBS_BEV_DEVICE)
      && (BbsTable[Count].Class == PCI_CLASS_MASS_STORAGE))) {
      continue;
    }

    for (Index = 0; Index < UefiSataHddCount; Index++) { // check hdd first
      if (UefiBbsHddTable[Index].BbsIndex == Count) {
        DevicePath = UefiBbsHddTable[Index].HddDevicePath; // found the DevicePath of hdd to which the current BbsEntry points
        MatchedDevice = TRUE;
        break;
      }
    }

    if (!MatchedDevice) { // check usb when no hdd matched
      for (Index = 0; Index < UefiUsbCount; Index++) {
        if (UefiBbsUsbTable[Index].BbsIndex == Count) {
          DevicePath = UefiBbsUsbTable[Index].UsbDevicePath; // found the DevicePath of USB device to which the current BbsEntry points
          MatchedDevice = TRUE;
          break;
        }
      }
    }

    if (MatchedDevice) {
      if (!IsMbrExist(DevicePath)) {
        BbsTable[Count].BootPriority = BBS_DO_NOT_BOOT_FROM;
        DEBUG((EFI_D_INFO, "Disable Bbs:%x %a line=%d\n", Count, __FUNCTION__, __LINE__));
      }
      MatchedDevice = FALSE;
    }
  }
}

VOID
EFIAPI
MaskNonbootableDevice (
  IN      EFI_EVENT                 Event,
  IN      VOID                      *Context
  )
{
  EFI_STATUS                        Status;
  BBS_TABLE                         *BbsTable = NULL;
  UefiBbsHddRL                      *UefiSataHdd = NULL;
  UINTN                             UefiSataHddCount = 0;
  UefiBbsUsbRL                      *UefiUsb = NULL;
  UINTN                             UefiUsbCount = 0;
  UEFI_BBS_RELATION_INFO_PROTOCOL	  *UefiBbsInfo;

  BbsTable = (BBS_TABLE *)Context;
  if (BbsTable == NULL) {
    return;
  }

  //
  // Install gUefiBbsRelationProtocolGuid
  //
  UefiSataHdd = (UefiBbsHddRL *)AllocatePool(sizeof(UefiBbsHddRL) * MAXIMUM_OF_HDD);
  ASSERT(UefiSataHdd != NULL);
  UefiSataHddCount = GetUefiSataIndex(UefiSataHdd, MAXIMUM_OF_HDD);

  UefiUsb = (UefiBbsUsbRL *)AllocatePool(sizeof(UefiBbsUsbRL) * MAXIMUM_OF_USB);
  ASSERT(UefiUsb != NULL);
  // UefiUsbCount = GetUefiUsbIndex(UefiUsb, MAXIMUM_OF_USB);
  // DEBUG ((EFI_D_INFO, "USB Count: %X \n", UefiUsbCount));
  UpdateBbsIndex(BbsTable,(UINT16)MAX_BBS_ENTRIES,UefiSataHdd, UefiSataHddCount, UefiUsb , &UefiUsbCount);
  DEBUG ((EFI_D_INFO, "USB Count: %X \n", UefiUsbCount));
  UefiBbsRelationInfoInstall (UefiSataHdd, UefiUsb, UefiSataHddCount, UefiUsbCount);

  if (PcdGetBool(PcdEnableCheckMbr)) {
    //
    // Check MBR of bbstable
    //
    Status = gBS->LocateProtocol(&gUefiBbsRelationProtocolGuid, NULL, &UefiBbsInfo);
    DEBUG((EFI_D_INFO, "Locate gUefiBbsRelationProtocolGuid:%r %a line=%d\n", Status, __FUNCTION__, __LINE__));
    if (!EFI_ERROR(Status)) {
      CheckAndMarkInvalidBbs(UefiBbsInfo, BbsTable, MAX_BBS_ENTRIES);
    }
  }
}

/**
  Get all BBS info

  @param  This                    Protocol instance pointer.
  @param  HddCount                Number of HDD_INFO structures
  @param  HddInfo                 Onboard IDE controller information
  @param  BbsCount                Number of BBS_TABLE structures
  @param  BbsTable                List BBS entries

  @retval EFI_SUCCESS             Tables returned
  @retval EFI_NOT_FOUND           resource not found
  @retval EFI_DEVICE_ERROR        can not get BBS table

**/
EFI_STATUS
EFIAPI
LegacyBiosGetBbsInfo (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  OUT UINT16                          *HddCount,
  OUT HDD_INFO                        **HddInfo,
  OUT UINT16                          *BbsCount,
  OUT BBS_TABLE                       **BbsTable
  )
{
  LEGACY_BIOS_INSTANCE              *Private;
  EFI_IA32_REGISTER_SET             Regs;
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;
//  HDD_INFO                          *LocalHddInfo;
//  IN BBS_TABLE                      *LocalBbsTable;
  UINTN                             NumHandles;
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             Index;
  UINTN                             TempData;
  //UINT32                            Granularity;
  VOID                              *Registration;

  HandleBuffer            = NULL;

  Private                 = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  EfiToLegacy16BootTable  = &Private->IntThunk->EfiToLegacy16BootTable;
//  LocalHddInfo            = EfiToLegacy16BootTable->HddInfo;
//  LocalBbsTable           = (BBS_TABLE*)(UINTN)EfiToLegacy16BootTable->BbsTable;

  if (!mBbsTableDoneFlag) {
    mBbsTable = Private->BbsTablePtr;

    //
    // Always enable disk controllers so 16-bit CSM code has valid information for all
    // drives.
    //
    //
    // Get PciRootBridgeIO protocol
    //
    gBS->LocateHandleBuffer (
          ByProtocol,
          &gEfiPciRootBridgeIoProtocolGuid,
          NULL,
          &NumHandles,
          &HandleBuffer
          );

    if (NumHandles == 0) {
      return EFI_NOT_FOUND;
    }

    mBbsTableDoneFlag = TRUE;
    for (Index = 0; Index < NumHandles; Index++) {
      //
      // Connect PciRootBridgeIO protocol handle with FALSE parameter to let
      // PCI bus driver enumerate all subsequent handles
      //
      gBS->ConnectController (HandleBuffer[Index], NULL, NULL, FALSE);

    }

    LegacyBiosBuildBbs (Private, mBbsTable);

    //Private->LegacyRegion->UnLock (Private->LegacyRegion, 0xe0000, 0x20000, &Granularity);

    //
    // Call into Legacy16 code to add to BBS table for non BBS compliant OPROMs.
    //
    ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
    Regs.X.AX = Legacy16UpdateBbs;

    //
    // Pass in handoff data
    //
    TempData  = (UINTN) EfiToLegacy16BootTable;
    Regs.X.ES = NORMALIZE_EFI_SEGMENT ((UINT32) TempData);
    Regs.X.BX = NORMALIZE_EFI_OFFSET ((UINT32) TempData);

    Private->LegacyBios.FarCall86 (
      This,
      Private->Legacy16CallSegment,
      Private->Legacy16CallOffset,
      &Regs,
      NULL,
      0
      );

    //Private->Cpu->FlushDataCache (Private->Cpu, 0xE0000, 0x20000, EfiCpuFlushTypeWriteBackInvalidate);
    //Private->LegacyRegion->Lock (Private->LegacyRegion, 0xe0000, 0x20000, &Granularity);

    DEBUG((EFI_D_INFO, "%a AX:%X\n", __FUNCTION__, Regs.X.AX));

    if (Regs.X.AX != 0) {
      return EFI_DEVICE_ERROR;
    }

    //
    // Check MBR of all hdd and usb BbsEntries after gBdsAllDriversConnectedProtocolGuid
    //
    EfiCreateProtocolNotifyEvent (
      &gBdsAllDriversConnectedProtocolGuid,
      TPL_CALLBACK,
      MaskNonbootableDevice,
      (VOID *)(UINTN)EfiToLegacy16BootTable->BbsTable,
      &Registration
      );
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  *HddCount = MAX_IDE_CONTROLLER;
  *HddInfo  = EfiToLegacy16BootTable->HddInfo;
  *BbsTable = (BBS_TABLE*)(UINTN)EfiToLegacy16BootTable->BbsTable;
  if(EfiToLegacy16BootTable->BbsTable > (((UINTN)*(UINT16*)(UINTN)0x40E) << 4)){
    *BbsCount = (UINT16)EfiToLegacy16BootTable->NumberBbsEntries;
  } else {
    *BbsCount = (UINT16) (sizeof (Private->IntThunk->BbsTable) / sizeof (BBS_TABLE));
  }
  return EFI_SUCCESS;
}
