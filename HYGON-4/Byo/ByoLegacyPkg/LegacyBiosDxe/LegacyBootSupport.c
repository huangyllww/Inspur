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
#include <Library/TimerLib.h>
#include <ByoStatusCode.h>
#include <Library/UefiBootManagerLib.h>
#include <Guid/BootRetryPolicyVariable.h>
#include <Library/ByoCommLib.h>
#include <ByoBootGroup.h>
#include <Library/LegacyBootOptionalDataLib.h>

#define BOOT_LEGACY_OS              0
#define BOOT_EFI_OS                 1
#define BOOT_UNCONVENTIONAL_DEVICE  2

UINT32              mLoadOptionsSize    = 0;
UINTN               mBootMode           = BOOT_LEGACY_OS;
VOID                *mLoadOptions       = NULL;
BBS_BBS_DEVICE_PATH *mBbsDevicePathPtr  = NULL;
BBS_BBS_DEVICE_PATH mBbsDevicePathNode;
UDC_ATTRIBUTES      mAttributes         = { 0, 0, 0, 0 };
UINTN               mBbsEntry           = 0;
VOID                *mBeerData          = NULL;
VOID                *mServiceAreaData   = NULL;

extern BBS_TABLE           *mBbsTable;

extern VOID                  *mRuntimeSmbiosEntryPoint;
extern EFI_PHYSICAL_ADDRESS  mReserveSmbiosEntryPoint;
extern EFI_PHYSICAL_ADDRESS  mStructureTableAddress;

BOOLEAN                      mLegacyAllAndGroupRetied;
BOOLEAN                      mLegacyPxeRetriedFlag = FALSE;

typedef struct {
    UINT8  BootStart:1;
    UINT8  Reserved:7;
} BOOT_CFG;
typedef struct {
  UINT8    GroupType;
  UINT16   Count;
} LEGACY_BOOT_GROUP_INFO;

BBS_TABLE                         *mBbsRtTable = NULL;
UINT16                            mRetryBbsCount = 0;
UINT16                            mOldBootPriority[MAX_BBS_ENTRIES];
LEGACY_BOOT_GROUP_INFO            *mGroupInfo;
UINTN                             mGroupCount = 0;
UINT8                             mOldPxeRetryValue = 0;

BOOT_RETRY_POLICY_VARIABLE        mRetryPolicy;

/**
  Print the BBS Table.

  @param BbsTable   The BBS table.


**/
VOID
PrintBbsTable (
  IN BBS_TABLE *BbsTable,
  IN UINT16    BbsCount
  )
{
  UINT16 Index;
  UINT16 SubIndex;
  CHAR8  *String;

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, " NO  Prio bb/dd/ff cl/sc Type Stat segm:offs mfgs:mfgo dess:deso\n"));
  DEBUG ((EFI_D_INFO, "=================================================================\n"));
  for (Index = 0; Index < BbsCount; Index++) {
    //
    // Filter
    //
    if (BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY) {
      continue;
    }

    DEBUG ((
      EFI_D_INFO,
      " %02x: %04x %02x/%02x/%02x %02x/%02x %04x %04x",
      (UINTN) Index,
      (UINTN) BbsTable[Index].BootPriority,
      (UINTN) BbsTable[Index].Bus,
      (UINTN) BbsTable[Index].Device,
      (UINTN) BbsTable[Index].Function,
      (UINTN) BbsTable[Index].Class,
      (UINTN) BbsTable[Index].SubClass,
      (UINTN) BbsTable[Index].DeviceType,
      (UINTN) * (UINT16 *) &BbsTable[Index].StatusFlags
      ));
    DEBUG ((
      EFI_D_INFO,
      " %04x:%04x %04x:%04x %04x:%04x",
      (UINTN) BbsTable[Index].BootHandlerSegment,
      (UINTN) BbsTable[Index].BootHandlerOffset,
      (UINTN) BbsTable[Index].MfgStringSegment,
      (UINTN) BbsTable[Index].MfgStringOffset,
      (UINTN) BbsTable[Index].DescStringSegment,
      (UINTN) BbsTable[Index].DescStringOffset
      ));

    //
    // Print DescString
    //
    String = (CHAR8 *)(UINTN)((BbsTable[Index].DescStringSegment << 4) + BbsTable[Index].DescStringOffset);
    if (String != NULL) {
      DEBUG ((EFI_D_INFO," ("));
      for (SubIndex = 0; String[SubIndex] != 0; SubIndex++) {
        DEBUG ((EFI_D_INFO, "%c", String[SubIndex]));
      }
      DEBUG ((EFI_D_INFO,")"));
    }
    DEBUG ((EFI_D_INFO,"\n"));
  }

  DEBUG ((EFI_D_INFO, "\n"));

  return ;
}

/**
  Print the BBS Table.

  @param HddInfo   The HddInfo table.


**/
VOID
PrintHddInfo (
  IN HDD_INFO *HddInfo
  )
{
  UINTN Index;

  DEBUG ((EFI_D_INFO, "\n"));
  for (Index = 0; Index < MAX_IDE_CONTROLLER; Index++) {
    DEBUG ((EFI_D_INFO, "Index - %04x\n", Index));
    DEBUG ((EFI_D_INFO, "  Status    - %04x\n", (UINTN)HddInfo[Index].Status));
    DEBUG ((EFI_D_INFO, "  B/D/F     - %02x/%02x/%02x\n", (UINTN)HddInfo[Index].Bus, (UINTN)HddInfo[Index].Device, (UINTN)HddInfo[Index].Function));
    DEBUG ((EFI_D_INFO, "  Command   - %04x\n", HddInfo[Index].CommandBaseAddress));
    DEBUG ((EFI_D_INFO, "  Control   - %04x\n", HddInfo[Index].ControlBaseAddress));
    DEBUG ((EFI_D_INFO, "  BusMaster - %04x\n", HddInfo[Index].BusMasterAddress));
    DEBUG ((EFI_D_INFO, "  HddIrq    - %02x\n", HddInfo[Index].HddIrq));
    DEBUG ((EFI_D_INFO, "  IdentifyDrive[0].Raw[0] - %x\n", HddInfo[Index].IdentifyDrive[0].Raw[0]));
    DEBUG ((EFI_D_INFO, "  IdentifyDrive[1].Raw[0] - %x\n", HddInfo[Index].IdentifyDrive[1].Raw[0]));
  }

  DEBUG ((EFI_D_INFO, "\n"));

  return ;
}

/**
  Print the PCI Interrupt Line and Interrupt Pin registers.
**/
VOID
PrintPciInterruptRegister (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       Index;
  EFI_HANDLE                  *Handles;
  UINTN                       HandleNum;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  UINT8                       Interrupt[2];
  UINTN                       Segment;
  UINTN                       Bus;
  UINTN                       Device;
  UINTN                       Function;

  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiPciIoProtocolGuid,
         NULL,
         &HandleNum,
         &Handles
         );

  Bus      = 0;
  Device   = 0;
  Function = 0;

  DEBUG ((EFI_D_INFO, "\n"));
  DEBUG ((EFI_D_INFO, " bb/dd/ff interrupt line interrupt pin\n"));
  DEBUG ((EFI_D_INFO, "======================================\n"));
  for (Index = 0; Index < HandleNum; Index++) {
    Status = gBS->HandleProtocol (Handles[Index], &gEfiPciIoProtocolGuid, (VOID **) &PciIo);
    if (!EFI_ERROR (Status)) {
      Status = PciIo->Pci.Read (
                            PciIo,
                            EfiPciIoWidthUint8,
                            PCI_INT_LINE_OFFSET,
                            2,
                            Interrupt
                            );
    }
    if (!EFI_ERROR (Status)) {
      Status = PciIo->GetLocation (
                        PciIo,
                        &Segment,
                        &Bus,
                        &Device,
                        &Function
                        );
    }
    if (!EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INFO, " %02x/%02x/%02x 0x%02x           0x%02x\n",
              Bus, Device, Function, Interrupt[0], Interrupt[1]));
    }
  }
  DEBUG ((EFI_D_INFO, "\n"));

  if (Handles != NULL) {
    FreePool (Handles);
  }
}

/**
  Identify drive data must be updated to actual parameters before boot.

  @param  IdentifyDriveData       ATA Identify Data

**/
VOID
UpdateIdentifyDriveData (
  IN  UINT8     *IdentifyDriveData
  );

/**
  Update SIO data.

  @param  Private                 Legacy BIOS Instance data

  @retval EFI_SUCCESS             Removable media not present

**/
EFI_STATUS
UpdateSioData (
  IN  LEGACY_BIOS_INSTANCE      *Private
  )
{
  EFI_STATUS                          Status;
  UINTN                               Index;
  UINTN                               Index1;
  UINT8                               LegacyInterrupts[16];
  EFI_LEGACY_IRQ_ROUTING_ENTRY        *RoutingTable;
  UINTN                               RoutingTableEntries;
  EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY *IrqPriorityTable;
  UINTN                               NumberPriorityEntries;
  EFI_TO_COMPATIBILITY16_BOOT_TABLE   *EfiToLegacy16BootTable;
  UINT8                               HddIrq;
  UINT16                              LegacyInt;
  UINT16                              LegMask;
  UINT16                              UartIrqs = 0;
  UINT16                              LegacyEdgeLevel;
  UINT32                              Register;
  UINTN                               HandleCount;
  EFI_HANDLE                          *HandleBuffer;
  EFI_ISA_IO_PROTOCOL                 *IsaIo;


  DEBUG((EFI_D_INFO, __FUNCTION__"()\n"));

  LegacyInt               = 0;
  HandleBuffer            = NULL;

  EfiToLegacy16BootTable  = &Private->IntThunk->EfiToLegacy16BootTable;
  LegacyBiosBuildSioData (Private);
  SetMem (LegacyInterrupts, sizeof (LegacyInterrupts), 0);

  //
  // Create list of legacy interrupts.
  //
  for (Index = 0; Index < 4; Index++) {
    LegacyInterrupts[Index] = EfiToLegacy16BootTable->SioData.Serial[Index].Irq;
    if(LegacyInterrupts[Index]){
      UartIrqs |= (UINT16)(1 << LegacyInterrupts[Index]);
    }
    DEBUG((EFI_D_INFO, "Uart%d Irq:%d\n", Index, LegacyInterrupts[Index]));
  }

  for (Index = 4; Index < 7; Index++) {
    LegacyInterrupts[Index] = EfiToLegacy16BootTable->SioData.Parallel[Index - 4].Irq;
  }

  LegacyInterrupts[7] = EfiToLegacy16BootTable->SioData.Floppy.Irq;

  //
  // Get Legacy Hdd IRQs. If native mode treat as PCI
  //
  for (Index = 0; Index < 2; Index++) {
    HddIrq = EfiToLegacy16BootTable->HddInfo[Index].HddIrq;
    if ((HddIrq != 0) && ((HddIrq == 15) || (HddIrq == 14))) {
      LegacyInterrupts[Index + 8] = HddIrq;
    }
  }

  Private->LegacyBiosPlatform->GetRoutingTable (
                                Private->LegacyBiosPlatform,
                                (VOID *) &RoutingTable,
                                &RoutingTableEntries,
                                NULL,
                                NULL,
                                (VOID **) &IrqPriorityTable,
                                &NumberPriorityEntries
                                );
  //
  // Remove legacy interrupts from the list of PCI interrupts available.
  //
  for (Index = 0; Index <= 0x0b; Index++) {
    for (Index1 = 0; Index1 <= NumberPriorityEntries; Index1++) {
      if (LegacyInterrupts[Index] != 0) {
        LegacyInt = (UINT16) (LegacyInt | (1 << LegacyInterrupts[Index]));
        if (LegacyInterrupts[Index] == IrqPriorityTable[Index1].Irq) {
          IrqPriorityTable[Index1].Used = LEGACY_USED;
        }
      }
    }
  }

  DEBUG((EFI_D_INFO, "LegacyInt:%X\n", LegacyInt)); //HYL-2016101802

  Private->Legacy8259->GetMask (
                        Private->Legacy8259,
                        &LegMask,
                        &LegacyEdgeLevel,
                        NULL,
                        NULL
                        );
  //
  // Set SIO interrupts and disable mouse. Let mouse driver
  // re-enable it.
  //
  LegMask = (UINT16) ((LegMask &~LegacyInt) | 0x1000);
  LegacyEdgeLevel |= UartIrqs;

  Private->Legacy8259->SetMask (
                        Private->Legacy8259,
                        &LegMask,
                        &LegacyEdgeLevel,
                        NULL,
                        NULL
                        );

  //
  // Disable mouse in keyboard controller
  //
  Register = 0xA7;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiIsaIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiIsaIoProtocolGuid,
                    (VOID **) &IsaIo
                    );
    ASSERT_EFI_ERROR (Status);
    IsaIo->Io.Write (IsaIo, EfiIsaIoWidthUint8, 0x64, 1, &Register);

  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  return EFI_SUCCESS;

}

/**
  Identify drive data must be updated to actual parameters before boot.
  This requires updating the checksum, if it exists.

  @param  IdentifyDriveData       ATA Identify Data
  @param  Checksum                checksum of the ATA Identify Data

  @retval EFI_SUCCESS             checksum calculated
  @retval EFI_SECURITY_VIOLATION  IdentifyData invalid

**/
EFI_STATUS
CalculateIdentifyDriveChecksum (
  IN  UINT8     *IdentifyDriveData,
  OUT UINT8     *Checksum
  )
{
  UINTN Index;
  UINT8 LocalChecksum;
  LocalChecksum = 0;
  *Checksum     = 0;
  if (IdentifyDriveData[510] != 0xA5) {
    return EFI_SECURITY_VIOLATION;
  }

  for (Index = 0; Index < 512; Index++) {
    LocalChecksum = (UINT8) (LocalChecksum + IdentifyDriveData[Index]);
  }

  *Checksum = LocalChecksum;
  return EFI_SUCCESS;
}


/**
  Identify drive data must be updated to actual parameters before boot.

  @param  IdentifyDriveData       ATA Identify Data


**/
VOID
UpdateIdentifyDriveData (
  IN  UINT8     *IdentifyDriveData
  )
{
  UINT16          NumberCylinders;
  UINT16          NumberHeads;
  UINT16          NumberSectorsTrack;
  UINT32          CapacityInSectors;
  UINT8           OriginalChecksum;
  UINT8           FinalChecksum;
  EFI_STATUS      Status;
  ATAPI_IDENTIFY  *ReadInfo;

  //
  // Status indicates if Integrity byte is correct. Checksum should be
  // 0 if valid.
  //
  ReadInfo  = (ATAPI_IDENTIFY *) IdentifyDriveData;
  Status    = CalculateIdentifyDriveChecksum (IdentifyDriveData, &OriginalChecksum);
  if (OriginalChecksum != 0) {
    Status = EFI_SECURITY_VIOLATION;
  }
  //
  // If NumberCylinders = 0 then do data(Controller present but don drive attached).
  //
  NumberCylinders = ReadInfo->Raw[1];
  if (NumberCylinders != 0) {
    ReadInfo->Raw[54]   = NumberCylinders;

    NumberHeads         = ReadInfo->Raw[3];
    ReadInfo->Raw[55]   = NumberHeads;

    NumberSectorsTrack  = ReadInfo->Raw[6];
    ReadInfo->Raw[56]   = NumberSectorsTrack;

    //
    // Copy Multisector info and set valid bit.
    //
    ReadInfo->Raw[59] = (UINT16) (ReadInfo->Raw[47] + 0x100);
    CapacityInSectors = (UINT32) ((UINT32) (NumberCylinders) * (UINT32) (NumberHeads) * (UINT32) (NumberSectorsTrack));
    ReadInfo->Raw[57] = (UINT16) (CapacityInSectors >> 16);
    ReadInfo->Raw[58] = (UINT16) (CapacityInSectors & 0xffff);
    if (Status == EFI_SUCCESS) {
      //
      // Force checksum byte to 0 and get new checksum.
      //
      ReadInfo->Raw[255] &= 0xff;
      CalculateIdentifyDriveChecksum (IdentifyDriveData, &FinalChecksum);

      //
      // Force new checksum such that sum is 0.
      //
      FinalChecksum = (UINT8) ((UINT8)0 - FinalChecksum);
      ReadInfo->Raw[255] = (UINT16) (ReadInfo->Raw[255] | (FinalChecksum << 8));
    }
  }
}

/**
  Identify drive data must be updated to actual parameters before boot.
  Do for all drives.

  @param  Private                 Legacy BIOS Instance data


**/
VOID
UpdateAllIdentifyDriveData (
  IN LEGACY_BIOS_INSTANCE                 *Private
  )
{
  UINTN     Index;
  HDD_INFO  *HddInfo;

  HddInfo = &Private->IntThunk->EfiToLegacy16BootTable.HddInfo[0];

  for (Index = 0; Index < MAX_IDE_CONTROLLER; Index++) {
    //
    // Each controller can have 2 devices. Update for each device
    //
    if ((HddInfo[Index].Status & HDD_MASTER_IDE) != 0) {
      UpdateIdentifyDriveData ((UINT8 *) (&HddInfo[Index].IdentifyDrive[0].Raw[0]));
    }

    if ((HddInfo[Index].Status & HDD_SLAVE_IDE) != 0) {
      UpdateIdentifyDriveData ((UINT8 *) (&HddInfo[Index].IdentifyDrive[1].Raw[0]));
    }
  }
}

/**
  Enable ide controller.  This gets disabled when LegacyBoot.c is about
  to run the Option ROMs.

  @param  Private        Legacy BIOS Instance data


**/
VOID
EnableIdeController (
  IN LEGACY_BIOS_INSTANCE              *Private
  )
{
  EFI_PCI_IO_PROTOCOL *PciIo;
  EFI_STATUS          Status;
  EFI_HANDLE          IdeController;
  UINT8               ByteBuffer;
  UINTN               HandleCount;
  EFI_HANDLE          *HandleBuffer;

  Status = Private->LegacyBiosPlatform->GetPlatformHandle (
                                          Private->LegacyBiosPlatform,
                                          EfiGetPlatformIdeHandle,
                                          0,
                                          &HandleBuffer,
                                          &HandleCount,
                                          NULL
                                          );
  if (!EFI_ERROR (Status)) {
    IdeController = HandleBuffer[0];
    Status = gBS->HandleProtocol (
                    IdeController,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    ByteBuffer = 0x1f;
    if (!EFI_ERROR (Status)) {
      PciIo->Pci.Write (PciIo, EfiPciIoWidthUint8, 0x04, 1, &ByteBuffer);
    }
  }
}


/**
  Enable ide controller.  This gets disabled when LegacyBoot.c is about
  to run the Option ROMs.

  @param  Private                 Legacy BIOS Instance data


**/
VOID
EnableAllControllers (
  IN LEGACY_BIOS_INSTANCE              *Private
  )
{
  UINTN               HandleCount;
  EFI_HANDLE          *HandleBuffer;
  UINTN               Index;
  EFI_PCI_IO_PROTOCOL *PciIo;
  PCI_TYPE01          PciConfigHeader;
  EFI_STATUS          Status;

  //
  //
  //
  EnableIdeController (Private);

  //
  // Assumption is table is built from low bus to high bus numbers.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  ASSERT_EFI_ERROR (Status);

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    ASSERT_EFI_ERROR (Status);

    PciIo->Pci.Read (
                PciIo,
                EfiPciIoWidthUint32,
                0,
                sizeof (PciConfigHeader) / sizeof (UINT32),
                &PciConfigHeader
                );

    //
    // We do not enable PPB here. This is for HotPlug Consideration.
    // The Platform HotPlug Driver is responsible for Padding enough hot plug
    // resources. It is also responsible for enable this bridge. If it
    // does not pad it. It will cause some early Windows fail to installation.
    // If the platform driver does not pad resource for PPB, PPB should be in
    // un-enabled state to let Windows know that this PPB is not configured by
    // BIOS. So Windows will allocate default resource for PPB.
    //
    // The reason for why we enable the command register is:
    // The CSM will use the IO bar to detect some IRQ status, if the command
    // is disabled, the IO resource will be out of scope.
    // For example:
    // We installed a legacy IRQ handle for a PCI IDE controller. When IRQ
    // comes up, the handle will check the IO space to identify is the
    // controller generated the IRQ source.
    // If the IO command is not enabled, the IRQ handler will has wrong
    // information. It will cause IRQ storm when the correctly IRQ handler fails
    // to run.
    //
    if (!(IS_PCI_VGA (&PciConfigHeader)     ||
          IS_PCI_OLD_VGA (&PciConfigHeader) ||
          IS_PCI_IDE (&PciConfigHeader)     ||
          IS_PCI_P2P (&PciConfigHeader)     ||
          IS_PCI_P2P_SUB (&PciConfigHeader) ||
          IS_PCI_LPC (&PciConfigHeader)     )) {

      PciConfigHeader.Hdr.Command |= 0x1f;

      PciIo->Pci.Write (PciIo, EfiPciIoWidthUint32, 4, 1, &PciConfigHeader.Hdr.Command);
    }
  }
}

/**
  The following routines are identical in operation, so combine
  for code compaction:
  EfiGetPlatformBinaryGetMpTable
  EfiGetPlatformBinaryGetOemIntData
  EfiGetPlatformBinaryGetOem32Data
  EfiGetPlatformBinaryGetOem16Data

  @param  This                    Protocol instance pointer.
  @param  Id                      Table/Data identifier

  @retval EFI_SUCCESS             Success
  @retval EFI_INVALID_PARAMETER   Invalid ID
  @retval EFI_OUT_OF_RESOURCES    no resource to get data or table

**/
EFI_STATUS
LegacyGetDataOrTable (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  IN EFI_GET_PLATFORM_INFO_MODE       Id
  )
{
  VOID                              *Table;
  UINT32                            TablePtr;
  UINTN                             TableSize;
  UINTN                             Alignment;
  UINTN                             Location;
  EFI_STATUS                        Status;
  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL *LegacyBiosPlatform;
  EFI_COMPATIBILITY16_TABLE         *Legacy16Table;
  EFI_IA32_REGISTER_SET             Regs;
  LEGACY_BIOS_INSTANCE              *Private;

  Private             = LEGACY_BIOS_INSTANCE_FROM_THIS (This);

  LegacyBiosPlatform  = Private->LegacyBiosPlatform;
  Legacy16Table       = Private->Legacy16Table;

  //
  // Phase 1 - get an address allocated in 16-bit code
  //
  while (TRUE) {
    switch (Id) {
    case EfiGetPlatformBinaryMpTable:
    case EfiGetPlatformBinaryOemIntData:
    case EfiGetPlatformBinaryOem32Data:
    case EfiGetPlatformBinaryOem16Data:
      {
        Status = LegacyBiosPlatform->GetPlatformInfo (
                                      LegacyBiosPlatform,
                                      Id,
                                      (VOID *) &Table,
                                      &TableSize,
                                      &Location,
                                      &Alignment,
                                      0,
                                      0
                                      );
        DEBUG ((EFI_D_INFO, "LegacyGetDataOrTable - ID: %x, %r\n", (UINTN)Id, Status));
        DEBUG ((EFI_D_INFO, "  Table - %x, Size - %x, Location - %x, Alignment - %x\n", (UINTN)Table, (UINTN)TableSize, (UINTN)Location, (UINTN)Alignment));
        break;
      }

    default:
      {
        return EFI_INVALID_PARAMETER;
      }
    }

    if (EFI_ERROR (Status)) {
      return Status;
    }

    ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
    Regs.X.AX = Legacy16GetTableAddress;
    Regs.X.CX = (UINT16) TableSize;
    Regs.X.BX = (UINT16) Location;
    Regs.X.DX = (UINT16) Alignment;
    Private->LegacyBios.FarCall86 (
      This,
      Private->Legacy16CallSegment,
      Private->Legacy16CallOffset,
      &Regs,
      NULL,
      0
      );

    if (Regs.X.AX != 0) {
      DEBUG ((EFI_D_ERROR, "Table ID %x length insufficient\n", Id));
      return EFI_OUT_OF_RESOURCES;
    } else {
      break;
    }
  }
  //
  // Phase 2 Call routine second time with address to allow address adjustment
  //
  Status = LegacyBiosPlatform->GetPlatformInfo (
                                LegacyBiosPlatform,
                                Id,
                                (VOID *) &Table,
                                &TableSize,
                                &Location,
                                &Alignment,
                                Regs.X.DS,
                                Regs.X.BX
                                );
  switch (Id) {
  case EfiGetPlatformBinaryMpTable:
    {
      Legacy16Table->MpTablePtr     = (UINT32) (Regs.X.DS * 16 + Regs.X.BX);
      Legacy16Table->MpTableLength  = (UINT32)TableSize;
      DEBUG ((EFI_D_INFO, "MP table in legacy region - %x\n", (UINTN)Legacy16Table->MpTablePtr));
      break;
    }

  case EfiGetPlatformBinaryOemIntData:
    {

      Legacy16Table->OemIntSegment  = Regs.X.DS;
      Legacy16Table->OemIntOffset   = Regs.X.BX;
      DEBUG ((EFI_D_INFO, "OemInt table in legacy region - %04x:%04x\n", (UINTN)Legacy16Table->OemIntSegment, (UINTN)Legacy16Table->OemIntOffset));
      break;
    }

  case EfiGetPlatformBinaryOem32Data:
    {
      Legacy16Table->Oem32Segment = Regs.X.DS;
      Legacy16Table->Oem32Offset  = Regs.X.BX;
      DEBUG ((EFI_D_INFO, "Oem32 table in legacy region - %04x:%04x\n", (UINTN)Legacy16Table->Oem32Segment, (UINTN)Legacy16Table->Oem32Offset));
      break;
    }

  case EfiGetPlatformBinaryOem16Data:
    {
      //
      //          Legacy16Table->Oem16Segment = Regs.X.DS;
      //          Legacy16Table->Oem16Offset  = Regs.X.BX;
      DEBUG ((EFI_D_INFO, "Oem16 table in legacy region - %04x:%04x\n", (UINTN)Legacy16Table->Oem16Segment, (UINTN)Legacy16Table->Oem16Offset));
      break;
    }

  default:
    {
      return EFI_INVALID_PARAMETER;
    }
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Phase 3 Copy table to final location
  //
  TablePtr = (UINT32) (Regs.X.DS * 16 + Regs.X.BX);

  CopyMem (
    (VOID *) (UINTN)TablePtr,
    Table,
    TableSize
    );

  return EFI_SUCCESS;
}

/**
  Copy SMBIOS table to EfiReservedMemoryType of memory for legacy boot.

**/
VOID
CreateSmbiosTableInReservedMemory (
  VOID
  )
{
  SMBIOS_TABLE_ENTRY_POINT    *EntryPointStructure;
  
  if ((mRuntimeSmbiosEntryPoint == NULL) || 
      (mReserveSmbiosEntryPoint == 0) || 
      (mStructureTableAddress == 0)) {
    return;
  }
  
  EntryPointStructure = (SMBIOS_TABLE_ENTRY_POINT *) mRuntimeSmbiosEntryPoint;
  
  //
  // Copy SMBIOS Entry Point Structure
  //
  CopyMem (
    (VOID *)(UINTN) mReserveSmbiosEntryPoint,
    EntryPointStructure,
    EntryPointStructure->EntryPointLength
  );
  
  //
  // Copy SMBIOS Structure Table into EfiReservedMemoryType memory
  //
  CopyMem (
    (VOID *)(UINTN) mStructureTableAddress,
    (VOID *)(UINTN) EntryPointStructure->TableAddress,
    EntryPointStructure->TableLength
  );
  
  //
  // Update TableAddress in Entry Point Structure
  //
  EntryPointStructure = (SMBIOS_TABLE_ENTRY_POINT *)(UINTN) mReserveSmbiosEntryPoint;
  EntryPointStructure->TableAddress = (UINT32)(UINTN) mStructureTableAddress;
  
  //
  // Fixup checksums in the Entry Point Structure
  //
  EntryPointStructure->IntermediateChecksum = 0;
  EntryPointStructure->EntryPointStructureChecksum = 0;

  EntryPointStructure->IntermediateChecksum = 
    CalculateCheckSum8 (
      (UINT8 *) EntryPointStructure + OFFSET_OF (SMBIOS_TABLE_ENTRY_POINT, IntermediateAnchorString), 
      EntryPointStructure->EntryPointLength - OFFSET_OF (SMBIOS_TABLE_ENTRY_POINT, IntermediateAnchorString)
      );
  EntryPointStructure->EntryPointStructureChecksum =
    CalculateCheckSum8 ((UINT8 *) EntryPointStructure, EntryPointStructure->EntryPointLength);
}




typedef struct {
  BOOLEAN              Ready;
  UINT16               OldPriority[MAX_BBS_ENTRIES];
  BBS_TABLE            *BbsTable;
  UINT16               BbsCount;
  UINT16               LanStartIndex;
  UINT16               LanEndIndex;
  UINT16               PxeRetrylimites; 
  UINT16               LanStartPriority; 
  UINT16               LanEndPriority;
} PXE_RETRY_CTX;

STATIC PXE_RETRY_CTX gPxeRetryCtx = {
  FALSE,
  {0,},
  NULL,
  0,
  0xFFFF,
  0xFFFF,
  0,
  0,
  0
};


typedef struct {
  BOOLEAN              Enable;
  BOOLEAN              Ready;
  UINT16               VideoMode;
  UINT64               TimerPeriod;
  UINT16               OldPriority[MAX_BBS_ENTRIES];
  BBS_TABLE            *BbsTable;
  UINT16               BbsCount;  
} LEGACY_2_UEFI_CTX;

STATIC LEGACY_2_UEFI_CTX gLegacy2UefiCtx = {
  FixedPcdGetBool(PcdCsm32Legacy2UefiSupport),
  FALSE,
  0,
  0,
  {0,},
  NULL,
  0
};


BOOLEAN 
PrepareLegacyPxeRetry (
    LEGACY_BIOS_INSTANCE  *Private
  )
{
  UINT16                            BbsCount = 0;
  UINTN                             CopySize;
  BBS_TABLE                         *BbsTable   = (BBS_TABLE*)Private->BbsTablePtr;
  BBS_TABLE                         *BbsRtTable = (BBS_TABLE*)(UINTN)PcdGet32(PcdBbsTableEntryRtAddress);
  UINT16                            BbsRtTableCount = PcdGet16(PcdBbsTableEntryRtMaxCount);
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;  
  UINT16                            Index;
  UINT16                            BootPriority;
  UINT8                             RetryPxeBoot    = PcdGet8(PcdRetryPxeBoot);
  UINT16                            PxeRetrylimites = PcdGet16(PcdPxeRetrylimites); 
  UINTN                             LanCount;
  UINT16                            LanEndPriority;
  UINT16                            LanStartPriority;
  

  DEBUG((EFI_D_INFO, "PrepareLegacyPxeRetry\n"));

  if(!RetryPxeBoot || BbsRtTableCount == 0 || PxeRetrylimites == 0){
    return FALSE;
  }

  BbsCount = 0;
  LanCount = 0;
  LanEndPriority = 0;
  LanStartPriority = 0xFFFF;
  for(Index=0;Index<MAX_BBS_ENTRIES;Index++){
    if(BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY ||
       BbsTable[Index].BootPriority == BBS_DO_NOT_BOOT_FROM ||
       BbsTable[Index].BootPriority == BBS_LOWEST_PRIORITY){
      continue;
    }

    if(BbsTable[Index].Class == PCI_CLASS_NETWORK){
      LanCount++;

      if(LanEndPriority < BbsTable[Index].BootPriority){
        LanEndPriority = BbsTable[Index].BootPriority;
      }
      if(LanStartPriority > BbsTable[Index].BootPriority){
        LanStartPriority = BbsTable[Index].BootPriority;
      }      
      
      gPxeRetryCtx.LanEndIndex = Index;
      if(gPxeRetryCtx.LanStartIndex == 0xFFFF){
        gPxeRetryCtx.LanStartIndex = Index;
      }      
    }
    
    BbsCount = (UINT16)(Index+1);
  }
  
  DEBUG((EFI_D_INFO, "BbsCount:%d, LanCount:%d %x - %x, %x - %x\n", \
    BbsCount, LanCount ,gPxeRetryCtx.LanStartIndex, gPxeRetryCtx.LanEndIndex, \
    LanStartPriority, LanEndPriority));
  if(BbsCount == 0 || BbsCount > BbsRtTableCount || LanCount == 0){
    return FALSE;
  }

  ASSERT(gPxeRetryCtx.LanStartIndex != 0xFFFF);
  ASSERT(gPxeRetryCtx.LanEndIndex   != 0xFFFF);
    
  CopySize = sizeof(BBS_TABLE) * BbsCount;
  DEBUG((EFI_D_INFO, "%X -> %X L:%X\n", Private->BbsTablePtr, BbsRtTable, CopySize));

  CopyMem(BbsRtTable, BbsTable, CopySize);
  Private->BbsTablePtr = (VOID*)BbsRtTable;
  EfiToLegacy16BootTable = &Private->IntThunk->EfiToLegacy16BootTable;
  EfiToLegacy16BootTable->BbsTable = (UINT32)(UINTN)BbsRtTable;
  EfiToLegacy16BootTable->NumberBbsEntries = BbsCount;


// save old Priority.
  for(Index=0;Index<BbsCount;Index++){
    gPxeRetryCtx.OldPriority[Index] = BbsRtTable[Index].BootPriority;
  }
  
  gPxeRetryCtx.BbsCount = BbsCount;
  gPxeRetryCtx.PxeRetrylimites = PxeRetrylimites;
  gPxeRetryCtx.BbsTable = BbsRtTable;
  gPxeRetryCtx.LanStartPriority = LanStartPriority;
  gPxeRetryCtx.LanEndPriority   = LanEndPriority;


// temp ignore entry Priority after lan.
  for(Index=0;Index<BbsCount;Index++){
    BootPriority = BbsRtTable[Index].BootPriority;
    if(BootPriority == BBS_IGNORE_ENTRY || BootPriority == BBS_DO_NOT_BOOT_FROM){
      continue;
    }
    if(BootPriority > LanEndPriority){
      BbsRtTable[Index].BootPriority = BBS_IGNORE_ENTRY;
      DEBUG((EFI_D_INFO, "ignore I %x\n", Index));
    }
  }

  PrintBbsTable(gPxeRetryCtx.BbsTable, gPxeRetryCtx.BbsCount);

  gPxeRetryCtx.Ready = TRUE;
  return TRUE;
}

VOID
IgnoreAllNetEntries (
  VOID
  )
{
  UINTN                             Index;
  BBS_TABLE                         *BbsTable = gPxeRetryCtx.BbsTable;
  UINT16                            BootPriority;

  DEBUG((EFI_D_INFO, "%a line=%d\n", __FUNCTION__, __LINE__));
  for (Index = 0; Index < gPxeRetryCtx.BbsCount; Index++) {
    BootPriority = BbsTable[Index].BootPriority;
    if(BootPriority == BBS_IGNORE_ENTRY || BootPriority == BBS_DO_NOT_BOOT_FROM){
      continue;
    }
    if ((BootPriority >= gPxeRetryCtx.LanStartPriority) && (BootPriority <= gPxeRetryCtx.LanEndPriority)) {
      BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
      DEBUG((EFI_D_INFO, "ignore I %x\n", Index));
    }
  }

  PrintBbsTable(gPxeRetryCtx.BbsTable, gPxeRetryCtx.BbsCount);
}



VOID
HandlePxeRetry (
    LEGACY_BIOS_INSTANCE  *Private
  )
{
  UINT16                            PxeRetrylimites = gPxeRetryCtx.PxeRetrylimites;
  EFI_IA32_REGISTER_SET             Regs;
  UINTN                             Index;
  BBS_TABLE                         *BbsTable = gPxeRetryCtx.BbsTable;
  UINT16                            BootPriority;


  DEBUG((EFI_D_INFO, "HandlePxeRetry\n"));

  ASSERT(gPxeRetryCtx.LanStartIndex != 0xFFFF);

// temp ignore entry Priority before lan.
  for(Index=0;Index<gPxeRetryCtx.BbsCount;Index++){
    BootPriority = BbsTable[Index].BootPriority;
    if(BootPriority == BBS_IGNORE_ENTRY || BootPriority == BBS_DO_NOT_BOOT_FROM){
      continue;
    }      
    if(BootPriority < gPxeRetryCtx.LanStartPriority){
      BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
      DEBUG((EFI_D_INFO, "ignore I %x\n", Index));
    }
  }

  PrintBbsTable(gPxeRetryCtx.BbsTable, gPxeRetryCtx.BbsCount);
  if (mLegacyPxeRetriedFlag && PcdGetBool(PcdIsAutoBootFromBootBootOptions)) {
    return;
  }

  Index = 1;
  DEBUG((EFI_D_INFO, "[BDS]Booted legacy GroupType:0x%x %d times %a\n", BM_MENU_TYPE_LEGACY_PXE, Index, __FUNCTION__));
  do {
    DEBUG((EFI_D_INFO, "PxeRetrylimites:%d\n", PxeRetrylimites));

    *(UINT16*)((((UINTN)*(UINT16*)(UINTN)0x40E) << 4) + 0x193) = 0;
    ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
    Regs.X.AX = Legacy16Boot;
    Private->LegacyBios.FarCall86 (
      &Private->LegacyBios,
      Private->Legacy16CallSegment,
      Private->Legacy16CallOffset,
      &Regs,
      NULL,
      0
      );

    MicroSecondDelay(2000000);
    
    if((PxeRetrylimites != PXE_RETRY_FOREVER_VALUE) && (PxeRetrylimites != 0)){
      PxeRetrylimites--;
    }
    Index++;
    DEBUG((EFI_D_INFO, "[BDS]Booted legacy GroupType:0x%x %d times %a\n", BM_MENU_TYPE_LEGACY_PXE, Index, __FUNCTION__));
  } while(PxeRetrylimites);
  mLegacyPxeRetriedFlag = TRUE;
}


VOID
HandleRemainEntries (
    LEGACY_BIOS_INSTANCE  *Private
  )
{
  UINT16                            Index;
  UINT16                            BootPriority;
  BBS_TABLE                         *BbsTable = gPxeRetryCtx.BbsTable;
  EFI_IA32_REGISTER_SET             Regs;
  UINTN                             Count = 0;
  

  DEBUG((EFI_D_INFO, "HandleRemainEntries\n"));

  ASSERT(gPxeRetryCtx.LanEndIndex != 0xFFFF);

  for(Index=0;Index<gPxeRetryCtx.BbsCount;Index++){
    BootPriority = gPxeRetryCtx.OldPriority[Index];
    if(BootPriority == BBS_IGNORE_ENTRY || BootPriority == BBS_DO_NOT_BOOT_FROM){
      continue;
    }      
    if(BootPriority > gPxeRetryCtx.LanEndPriority){
      Count++;
      BbsTable[Index].BootPriority = BootPriority;
      DEBUG((EFI_D_INFO, "restore [%x] %x\n", Index, BootPriority));

    } else {
      BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
    }
  }

  PrintBbsTable(gPxeRetryCtx.BbsTable, gPxeRetryCtx.BbsCount);

  if(Count){
    *(UINT16*)((((UINTN)*(UINT16*)(UINTN)0x40E) << 4) + 0x193) = 0;
    ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
    Regs.X.AX = Legacy16Boot;
    Private->LegacyBios.FarCall86 (
      &Private->LegacyBios,
      Private->Legacy16CallSegment,
      Private->Legacy16CallOffset,
      &Regs,
      NULL,
      0
      );
  }
}


VOID
RestoreBbstablePriority (
    VOID
  )
{
  UINT16                            Index;
  UINT16                            BootPriority;
  BBS_TABLE                         *BbsTable = gPxeRetryCtx.BbsTable;


  DEBUG((EFI_D_INFO, "RestoreBbstablePriority\n"));

  if(!gLegacy2UefiCtx.Ready || (BbsTable == NULL)){
    return;
  }

  for(Index=0;Index<gLegacy2UefiCtx.BbsCount;Index++){
    BootPriority = gLegacy2UefiCtx.OldPriority[Index];
    BbsTable[Index].BootPriority = BootPriority;
  }
  for(Index=gLegacy2UefiCtx.BbsCount; Index<MAX_BBS_ENTRIES; Index++){
    BbsTable[Index].BootPriority = BBS_IGNORE_ENTRY;
  }

  PrintBbsTable(gLegacy2UefiCtx.BbsTable, gLegacy2UefiCtx.BbsCount);  
}




VOID ShowLegacyNoBootDev(LEGACY_BIOS_INSTANCE *Private)
{
  EFI_IA32_REGISTER_SET   Regs;
  CHAR8                   *Str = "No Bootable Device.";
  UINTN                   Count = AsciiStrLen(Str);
  UINTN                   Index;


// clear screen
// AX - 0600h
// BX - 0700h
// CX - 0000h
// DX - 184Fh
    ZeroMem (&Regs, sizeof(EFI_IA32_REGISTER_SET));
    Regs.X.AX = 0x0600;
    Regs.X.BX = 0x0700;
    Regs.X.DX = 0x184F;    
    Private->LegacyBios.Int86(&Private->LegacyBios, 0x10, &Regs);  

// reset cursor position
// AX - 0200h
// BX - 0000h
// CX - 0000h
// DX - 0000h
    ZeroMem (&Regs, sizeof(EFI_IA32_REGISTER_SET));
    Regs.X.AX = 0x0200;
    Private->LegacyBios.Int86(&Private->LegacyBios, 0x10, &Regs);

// print char
// AH - 0Eh
// AL - char
// BH - page    // 0
// BL - color   // 3
  for(Index=0;Index<Count;Index++){
    ZeroMem (&Regs, sizeof(EFI_IA32_REGISTER_SET));
    Regs.X.AX = (0xE << 8) | Str[Index];
    Regs.X.BX = 0x0003;
    Private->LegacyBios.Int86(&Private->LegacyBios, 0x10, &Regs);    
  }
}



EFI_STATUS
GetVesaVideoMode (
  IN EFI_LEGACY_BIOS_PROTOCOL *This,
  IN UINT16                   *VideoMode
  )
{
  EFI_STATUS              Status;
  EFI_IA32_REGISTER_SET   Regs;
  LEGACY_BIOS_INSTANCE    *Private;


  Private = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  if (Private->VgaInstalled == FALSE) {
    return EFI_ABORTED;
  }

  //
  // Call VESA Function 0x4F03 to get the current VIDEO mode.
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = 0x4f03;
  Status = This->Int86 (This, 0x10, &Regs);
  *VideoMode = Regs.X.BX;

  //
  // Check to see if the call succeeded.
  //
  if (Regs.X.AX != 0x004f) {
    //
    // Using INT10h 0x0f to get the current VIDEO mode.
    //
    ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
    Regs.H.AH = 0x0f;
    Status = This->Int86 (This, 0x10, &Regs);
    *VideoMode = (UINT16)Regs.H.AL;
  }

  return Status;
}

EFI_STATUS
SetVesaVideoMode (
  IN EFI_LEGACY_BIOS_PROTOCOL *This,
  IN UINT16                   VideoMode
  )
{
  EFI_STATUS              Status;
  EFI_IA32_REGISTER_SET   Regs;
  LEGACY_BIOS_INSTANCE    *Private;

  Private = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  if (Private->VgaInstalled == FALSE) {
    return EFI_ABORTED;
  }

  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = 0x4f02;
  Regs.X.BX = VideoMode;
  Status = This->Int86 (This, 0x10, &Regs);

  //
  // Check to see if the call succeeded.
  //
  if (Regs.X.AX != 0x004f) {
    //
    // Using original INT10h 0x00 to set the current VIDEO mode.
    //
    ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
    Regs.H.AH = 0x00;
    Regs.H.AL = (UINT8)VideoMode;
    Status = This->Int86 (This, 0x10, &Regs);
  }

  return Status;
}


BOOLEAN 
PrepareLegacy2Uefi (
    LEGACY_BIOS_INSTANCE  *Private
  )
{
  UINT16                            BbsCount = 0;
  UINTN                             CopySize;
  BBS_TABLE                         *BbsTable   = (BBS_TABLE*)Private->BbsTablePtr;
  BBS_TABLE                         *BbsRtTable = (BBS_TABLE*)(UINTN)PcdGet32(PcdBbsTableEntryRtAddress);
  UINT16                            BbsRtTableCount = PcdGet16(PcdBbsTableEntryRtMaxCount);
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;  
  UINT16                            Index;
  

  DEBUG((EFI_D_INFO, "PrepareLegacy2Uefi\n"));

  if(BbsRtTableCount == 0){
    return FALSE;
  }

  BbsCount = 0;
  for(Index=0;Index<MAX_BBS_ENTRIES;Index++){
    if(BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY ||
       BbsTable[Index].BootPriority == BBS_DO_NOT_BOOT_FROM ||
       BbsTable[Index].BootPriority == BBS_LOWEST_PRIORITY){
      continue;
    }
    BbsCount = (UINT16)(Index+1);
  }
  
  DEBUG((EFI_D_INFO, "BbsCount:%d\n", BbsCount));
  if(BbsCount == 0 || BbsCount > BbsRtTableCount){
    return FALSE;
  }

  CopySize = sizeof(BBS_TABLE) * BbsCount;
  DEBUG((EFI_D_INFO, "%X -> %X L:%X\n", Private->BbsTablePtr, BbsRtTable, CopySize));

  CopyMem(BbsRtTable, BbsTable, CopySize);
  Private->BbsTablePtr = (VOID*)BbsRtTable;
  EfiToLegacy16BootTable = &Private->IntThunk->EfiToLegacy16BootTable;
  EfiToLegacy16BootTable->BbsTable = (UINT32)(UINTN)BbsRtTable;
  EfiToLegacy16BootTable->NumberBbsEntries = BbsCount;

// save old Priority.
  for(Index=0;Index<BbsCount;Index++){
    gLegacy2UefiCtx.OldPriority[Index] = BbsRtTable[Index].BootPriority;
  }
  
  gLegacy2UefiCtx.BbsCount = BbsCount;
  gLegacy2UefiCtx.BbsTable = BbsRtTable;
  gLegacy2UefiCtx.Ready = TRUE;
  return TRUE;
}


VOID
SyncPxeRetry2Legacy2Uefi (
    VOID
  )
{
  CopyMem(gLegacy2UefiCtx.OldPriority, gPxeRetryCtx.OldPriority, gPxeRetryCtx.BbsCount * sizeof(gPxeRetryCtx.OldPriority[0]));
  gLegacy2UefiCtx.BbsCount = gPxeRetryCtx.BbsCount;
  gLegacy2UefiCtx.BbsTable = gPxeRetryCtx.BbsTable;
  gLegacy2UefiCtx.Ready = TRUE;
}



VOID
SignalEventLegacyBootExit (
    VOID
  )
{
  EFI_STATUS    Status;
  EFI_EVENT     Event;

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  EfiEventEmptyFunction,
                  NULL,
                  &gEfiEventLegacyBootPreExitGuid,
                  &Event
                  );
  if(!EFI_ERROR (Status)){
    gBS->SignalEvent(Event);
    gBS->CloseEvent(Event); 
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  EfiEventEmptyFunction,
                  NULL,
                  &gEfiEventLegacyBootExitGuid,
                  &Event
                  );
  if (!EFI_ERROR (Status)) {
    gBS->SignalEvent(Event);
    gBS->CloseEvent(Event);
  }
}



EFI_STATUS ReConnectUefiDriver()
{
  EfiBootManagerConnectConsoleVariable(ConOut);
  EfiBootManagerConnectConsoleVariable(ConIn);
  return EFI_SUCCESS;
}

STATIC
UINT16
GetBbsDevcieType (
  IN CONST BBS_TABLE *BbsEntry
)
{
  UINT16  DevType;

  DevType = BbsEntry->DeviceType;
  if(BbsEntry->DeviceType == BBS_BEV_DEVICE){
    if(BbsEntry->Class == PCI_CLASS_MASS_STORAGE){
      DevType = BBS_CDROM;
    }else if(BbsEntry->Class == PCI_CLASS_NETWORK){
      DevType = BBS_EMBED_NETWORK;
    }
  }
  return DevType;
}
VOID
PrepareLegacyGroupInfo (
  BBS_TABLE        *BbsTable
  )
{
  EFI_BOOT_MANAGER_LOAD_OPTION      *BootOptions;
  UINTN                             OptionsCount;
  UINTN                             Count;
  UINTN                             Index;
  UINT16                            BbsIndex;
  BOOLEAN                           Inited = FALSE;
  UINT8                             GroupType = BM_MENU_TYPE_MAX;
  if ((mGroupInfo == NULL) || (BbsTable == NULL)) {
    return;
  }

  BootOptions = EfiBootManagerGetLoadOptions (&OptionsCount, LoadOptionTypeBoot);
  for (Index = 0; Index < OptionsCount; Index++) {
    if ((BootOptions[Index].Attributes & LOAD_OPTION_ACTIVE) == 0) {
      continue;
    }
    if ((DevicePathType (BootOptions[Index].FilePath) != BBS_DEVICE_PATH) ||
      (DevicePathSubType (BootOptions[Index].FilePath) != BBS_BBS_DP)) {
      continue;
    }
    BbsIndex = (UINT16)LegacyBootOptionalDataGetBbsIndex(BootOptions[Index].OptionalData);
    if ((BbsIndex >= MAX_BBS_ENTRIES) || (BbsTable[BbsIndex].BootPriority == BBS_IGNORE_ENTRY)
      || (BbsTable[BbsIndex].BootPriority == BBS_DO_NOT_BOOT_FROM)
      || (BbsTable[Index].BootPriority == BBS_LOWEST_PRIORITY)) {
      continue;
    }
    GroupType = GetBbsGroupType(BbsTable[BbsIndex].Class, (UINT8)GetBbsDevcieType(&BbsTable[BbsIndex]));
    if (GroupType == BM_MENU_TYPE_MAX) {
      continue;
    }
    if (!Inited) {
      Inited = TRUE;
      mGroupInfo[mGroupCount].GroupType = GroupType;
      mGroupInfo[mGroupCount].Count++;
      mGroupCount++;
      continue;
    }
    for (Count = 0; Count < mGroupCount; Count++) {
      if (mGroupInfo[Count].GroupType == GroupType) { // update current GroupType info
        mGroupInfo[Count].Count++;
        break;
      }
    }

    if (Count == mGroupCount) { // not added in mGroupInfo
      mGroupInfo[Count].GroupType = GroupType;
      mGroupInfo[Count].Count++;
      mGroupCount++;
    }
  }
  EfiBootManagerFreeLoadOptions (BootOptions, OptionsCount);
}

VOID
AdjustBbsTable (
  UINTN      GroupIndex
  )
{
  UINT16                 Index;
  UINT8                  GroupType;

  if ((mGroupInfo == NULL) || (mBbsRtTable == NULL) || (mRetryBbsCount == 0)) {
    return;
  }

  for (Index = 0; Index < mRetryBbsCount; Index++) {
    if ((mBbsRtTable[Index].BootPriority == BBS_IGNORE_ENTRY)
    || (mBbsRtTable[Index].BootPriority == BBS_DO_NOT_BOOT_FROM)
    || (mBbsRtTable[Index].BootPriority == BBS_LOWEST_PRIORITY)) {
      continue;
    }

    GroupType = GetBbsGroupType(mBbsRtTable[Index].Class, (UINT8)GetBbsDevcieType(&mBbsRtTable[Index]));
    if (mGroupInfo[GroupIndex].GroupType == GroupType) {
      continue;
    }

    mBbsRtTable[Index].BootPriority = BBS_IGNORE_ENTRY;
    DEBUG((EFI_D_INFO, "Boot retry ignore I %x %a\n", Index, __FUNCTION__));
  }
}
VOID
RestoreBbsTable (
  VOID
  )
{
  UINTN                           Size;
  UINT16                          Index;

  if ((mGroupInfo == NULL) || (mBbsRtTable == NULL) || (mRetryBbsCount == 0)) {
    return;
  }

  Size = sizeof(BBS_TABLE) * mRetryBbsCount;
  DEBUG((EFI_D_INFO, "Restore BootPriority of BbsTable from %x %a\n", mBbsRtTable, __FUNCTION__));
  for (Index = 0; Index < mRetryBbsCount; Index++) {
    mBbsRtTable[Index].BootPriority = mOldBootPriority[Index];
  }
  PrintBbsTable(mBbsRtTable, mRetryBbsCount);
}

VOID
LegacyBootAndRetryPolicies (
  LEGACY_BIOS_INSTANCE  *Private
  )
{
  UINTN                             Size;
  UINTN                             Index, GroupIndex;
  UINTN                             Count;
  BBS_TABLE                         *BbsTable;
  UINT16                            BbsRtTableCount = PcdGet16(PcdBbsTableEntryRtMaxCount);
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;
  EFI_IA32_REGISTER_SET             Regs;
  UINTN                             GroupTimes;
  UINT16                            PxeRetrylimits = PcdGet16(PcdPxeRetrylimites);
  UINTN                             PxeTypeIndex = 0xFFFF;

  DEBUG((EFI_D_INFO, "%a line=%d\n", __FUNCTION__, __LINE__));

  if (Private == NULL) {
    return;
  }

  BbsTable = (BBS_TABLE*)Private->BbsTablePtr;
  mBbsRtTable = (BBS_TABLE*)(UINTN)PcdGet32(PcdBbsTableEntryRtAddress);

  if (mBbsRtTable == NULL) {
    mRetryPolicy.GroupRetryTime = 0;
    mRetryPolicy.AllRetryTime = 1; // at least boot once
  }

  if (mRetryPolicy.AllRetryTime == 0) {
    mRetryPolicy.AllRetryTime = 1; // at least boot once
  }
  DEBUG((EFI_D_INFO, "GroupRetryTime=%d AllRetrytime=%d\n", mRetryPolicy.GroupRetryTime, mRetryPolicy.AllRetryTime));

  if ((mRetryPolicy.GroupRetryTime != 0) || ((mRetryPolicy.AllRetryTime > 1) && (mRetryPolicy.GroupRetryTime == 0))) {
    if (mRetryPolicy.GroupRetryTime == 0) {
      mRetryPolicy.GroupRetryTime = 1;
    }
    //
    // Prepare LEGACY_BOOT_GROUP_INFO as legacy options retrying order
    //
    Count = BM_MENU_TYPE_LEGACY_MAX - BM_MENU_TYPE_LEGACY_MIN;
    mGroupInfo = (LEGACY_BOOT_GROUP_INFO *)AllocateZeroPool(sizeof(LEGACY_BOOT_GROUP_INFO) * Count);
    ASSERT(mGroupInfo != NULL);
    PrepareLegacyGroupInfo(BbsTable);

    //
    // Backup the raw BbsTable
    //
    for (Index = 0; Index < BbsRtTableCount; Index++) {
      if ((BbsTable[Index].BootPriority == BBS_IGNORE_ENTRY)
        || (BbsTable[Index].BootPriority == BBS_DO_NOT_BOOT_FROM)
        || (BbsTable[Index].BootPriority == BBS_LOWEST_PRIORITY)) {
        continue;
      }
      mRetryBbsCount = (UINT16)(Index + 1);
    }
    if ((mRetryBbsCount == 0) || (mRetryBbsCount > BbsRtTableCount)) {
      return;
    }

    //
    // Back up BootPriority of BbsTable and copy BbsTable to PcdBbsTableEntryRtAddress
    //
    DEBUG((EFI_D_INFO, "mRetryBbsCount=%d %a\n", mRetryBbsCount, __FUNCTION__));
    for (Index = 0; Index < mRetryBbsCount; Index++) {
      mOldBootPriority[Index] = BbsTable[Index].BootPriority;
    }
    Size = sizeof(BBS_TABLE) * mRetryBbsCount;
    DEBUG((EFI_D_INFO, "backup BbsTable%X -> %X L:%X %a\n", BbsTable, mBbsRtTable, Size, __FUNCTION__));
    CopyMem(mBbsRtTable, BbsTable, Size);
    Private->BbsTablePtr = (VOID*)mBbsRtTable;
    EfiToLegacy16BootTable = &Private->IntThunk->EfiToLegacy16BootTable;
    EfiToLegacy16BootTable->BbsTable = (UINT32)(UINTN)mBbsRtTable;
    EfiToLegacy16BootTable->NumberBbsEntries = mRetryBbsCount;

    //
    // process legacy options retrying booting policy
    //
    GroupTimes = mRetryPolicy.GroupRetryTime;
    Index = 0;
    while (Index < mRetryPolicy.AllRetryTime) {
      DEBUG((EFI_D_INFO, "*** Start booting all legacy boot options: ***\n"));
      for (Count = 0; Count < mGroupCount; Count++) {
        AdjustBbsTable(Count);
        PrintBbsTable(mBbsRtTable, mRetryBbsCount);
        if ((mGroupInfo[Count].GroupType == BM_MENU_TYPE_LEGACY_PXE) && (mOldPxeRetryValue != 0)) {
          PxeTypeIndex = Count;
          DEBUG((EFI_D_INFO, "Pxe GroupTypeIndex=%d %a line=%d\n", PxeTypeIndex, __FUNCTION__, __LINE__));
          if ((mOldPxeRetryValue != 0) && (PxeRetrylimits != PXE_RETRY_FOREVER_VALUE)
            && (mGroupInfo[Count].GroupType == BM_MENU_TYPE_LEGACY_PXE)) {
            GroupTimes = PxeRetrylimits + 1;
          }
        }
        for (GroupIndex = 0; GroupIndex < GroupTimes; GroupIndex++) {
          DEBUG((EFI_D_INFO, "[F0000]:%X\n", *(UINT32*)(UINTN)0xF0000));
          ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
          Regs.X.AX = Legacy16Boot;
          Private->LegacyBios.FarCall86 (
            &Private->LegacyBios,
            Private->Legacy16CallSegment,
            Private->Legacy16CallOffset,
            &Regs,
            NULL,
            0
            );
          DEBUG((EFI_D_INFO, "Legacy16Boot ret\n"));
    
          if ((mOldPxeRetryValue != 0) && (PxeRetrylimits != PXE_RETRY_FOREVER_VALUE)
            && (mGroupInfo[Count].GroupType == BM_MENU_TYPE_LEGACY_PXE)) {
              MicroSecondDelay(2000000);
          }
          DEBUG((EFI_D_INFO, "[BDS]Booted legacy GroupType:0x%x %d times %a\n", mGroupInfo[Count].GroupType, GroupIndex + 1, __FUNCTION__));

          *(UINT16*)((((UINTN)*(UINT16*)(UINTN)0x40E) << 4) + 0x193) = 0;
        }
        if ((mOldPxeRetryValue != 0) && (PxeRetrylimits != PXE_RETRY_FOREVER_VALUE)
          && (mGroupInfo[Count].GroupType == BM_MENU_TYPE_LEGACY_PXE)) {
          GroupTimes = mRetryPolicy.GroupRetryTime;
        }
        RestoreBbsTable();
      }
      if ((PcdGet16(PcdUnlimitedAllRetry) == 0) || (mRetryPolicy.AllRetryTime != PcdGet16(PcdUnlimitedAllRetry))) {
        Index++;
      }
    }

    //
    // if PXE retrying is enabled and PxeRetrylimits is 0xFF, retry pxe options forever.
    //
    if ((mOldPxeRetryValue != 0) && (PxeRetrylimits == PXE_RETRY_FOREVER_VALUE) && (PxeTypeIndex != 0xFFFF)) {
      AdjustBbsTable(PxeTypeIndex);
      PrintBbsTable(mBbsRtTable, mRetryBbsCount);
      Index = 0;
      while (TRUE) {
        DEBUG((EFI_D_INFO, "PxeRetrylimites:%d\n", PxeRetrylimits));
  
        *(UINT16*)((((UINTN)*(UINT16*)(UINTN)0x40E) << 4) + 0x193) = 0;
        ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
        Regs.X.AX = Legacy16Boot;
        Private->LegacyBios.FarCall86 (
          &Private->LegacyBios,
          Private->Legacy16CallSegment,
          Private->Legacy16CallOffset,
          &Regs,
          NULL,
          0
          );
  
        MicroSecondDelay(2000000);
        DEBUG((EFI_D_INFO, "Legacy16Boot ret\n"));
        Index++;
        DEBUG((EFI_D_INFO, "[BDS]Booted legacy GroupType:0x%x %d times %a\n", BM_MENU_TYPE_LEGACY_PXE, Index, __FUNCTION__));
      }
    }
    mLegacyAllAndGroupRetied = TRUE;
  } else {
    //
    // no boot Group retrying
    //
    Index = 0;
    while (Index < mRetryPolicy.AllRetryTime) {
      DEBUG((EFI_D_INFO, "*** Start booting all legacy boot options: ***\n"));
      DEBUG((EFI_D_INFO, "[F0000]:%X\n", *(UINT32*)(UINTN)0xF0000));
      ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
      Regs.X.AX = Legacy16Boot;
      Private->LegacyBios.FarCall86 (
        &Private->LegacyBios,
        Private->Legacy16CallSegment,
        Private->Legacy16CallOffset,
        &Regs,
        NULL,
        0
        );

      DEBUG((EFI_D_INFO, "Legacy16Boot ret\n"));
      if ((PcdGet16(PcdUnlimitedAllRetry) == 0) || (mRetryPolicy.AllRetryTime != PcdGet16(PcdUnlimitedAllRetry))) {
        Index++;
      }
    }
  }
}

/**
  Assign drive number to legacy HDD drives prior to booting an EFI
  aware OS so the OS can access drives without an EFI driver.
  Note: BBS compliant drives ARE NOT available until this call by
  either shell or EFI.

  @param  This                    Protocol instance pointer.

  @retval EFI_SUCCESS             Drive numbers assigned

**/
EFI_STATUS
GenericLegacyBoot (
  IN EFI_LEGACY_BIOS_PROTOCOL           *This
  )
{
  EFI_STATUS                        Status;
  LEGACY_BIOS_INSTANCE              *Private;
  EFI_IA32_REGISTER_SET             Regs;
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;
  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL *LegacyBiosPlatform;
  UINTN                             CopySize;
  VOID                              *AcpiPtr;
  HDD_INFO                          *HddInfo;
  HDD_INFO                          *LocalHddInfo;
  UINTN                             Index;
  EFI_COMPATIBILITY16_TABLE         *Legacy16Table;
  UINT32                            *BdaPtr;
  UINT16                            HddCount;
  UINT16                            BbsCount;
  BBS_TABLE                         *LocalBbsTable;
  UINT32                            *BaseVectorMaster;
  EFI_TIME                          BootTime;
  UINT32                            LocalTime;
  EFI_HANDLE                        IdeController;
  UINTN                             HandleCount;
  EFI_HANDLE                        *HandleBuffer;
  VOID                              *AcpiTable;
  UINTN                             ShadowAddress;
  UINT32                            Granularity;
  BOOT_CFG                          *BootCfg; 
  UINT16                            EBdaSeg;
  UINTN                             Size;

  mLegacyAllAndGroupRetied = FALSE;
  LocalHddInfo  = NULL;
  HddCount      = 0;
  BbsCount      = 0;
  LocalBbsTable = NULL;

  //
  // Invoke to check bound Hdd
  //
  REPORT_STATUS_CODE (EFI_PROGRESS_CODE, BYO_LEAGCY_BOOT_START);

  Private       = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  DEBUG_CODE (
    DEBUG ((EFI_D_ERROR, "Start of legacy boot\n"));
  );

  //
  // Only when booting from BootBootOptions, the GroupRetry and AllRetry are supported
  //
  Status = EFI_NOT_FOUND;
  if (PcdGetBool(PcdIsAutoBootFromBootBootOptions)) {
    Size = sizeof(BOOT_RETRY_POLICY_VARIABLE);
    Status = gRT->GetVariable (
                    BOOT_RETRY_POLICY_VAR_NAME,
                    &gBootRetryPolicyVariableGuid,
                    NULL,
                    &Size,
                    &mRetryPolicy
                    );
  }
  if (EFI_ERROR(Status)) {
    mRetryPolicy.GroupRetryTime = 0;
    mRetryPolicy.AllRetryTime = 1; // at least boot once
  }

  if ((mRetryPolicy.GroupRetryTime != 0) || (mRetryPolicy.AllRetryTime > 1)) {
    mOldPxeRetryValue = PcdGet8(PcdRetryPxeBoot);
    PcdSet8S(PcdRetryPxeBoot, 0);
  }

  if(gLegacy2UefiCtx.Enable){
    GetVesaVideoMode(This, &gLegacy2UefiCtx.VideoMode);
    DEBUG ((EFI_D_INFO, "VideoMode:0x%x BBS(%x,%x)\n", gLegacy2UefiCtx.VideoMode, Private->BbsTablePtr, gLegacy2UefiCtx.BbsCount));
    if (Private->LegacyBootEntered) {
      PrintBbsTable((BBS_TABLE*)Private->BbsTablePtr, gLegacy2UefiCtx.BbsCount);
      *(UINT16*)((((UINTN)*(UINT16*)(UINTN)0x40E) << 4) + 0x193) = 0;
      goto _LegacyBoot;
    }
  }

  Legacy16Table                         = Private->Legacy16Table;
  EfiToLegacy16BootTable                = &Private->IntThunk->EfiToLegacy16BootTable;
  HddInfo = &EfiToLegacy16BootTable->HddInfo[0];

  LegacyBiosPlatform = Private->LegacyBiosPlatform;

  EfiToLegacy16BootTable->MajorVersion = EFI_TO_LEGACY_MAJOR_VERSION;
  EfiToLegacy16BootTable->MinorVersion = EFI_TO_LEGACY_MINOR_VERSION;
  
  //
  // If booting to a legacy OS then force HDD drives to the appropriate
  // boot mode by calling GetIdeHandle.
  // A reconnect -r can force all HDDs back to native mode.
  //
  IdeController = NULL;
  if ((mBootMode == BOOT_LEGACY_OS) || (mBootMode == BOOT_UNCONVENTIONAL_DEVICE)) {
    Status = LegacyBiosPlatform->GetPlatformHandle (
                                  Private->LegacyBiosPlatform,
                                  EfiGetPlatformIdeHandle,
                                  0,
                                  &HandleBuffer,
                                  &HandleCount,
                                  NULL
                                  );
    if (!EFI_ERROR (Status)) {
      IdeController = HandleBuffer[0];
    }   
  }
  //
  // Unlock the Legacy BIOS region
  //
  //Private->LegacyRegion->UnLock (
  //                         Private->LegacyRegion,
  //                         0xE0000,
  //                         0x20000,
  //                         &Granularity
  //                         );

  //
  // Reconstruct the Legacy16 boot memory map
  //
  LegacyBiosBuildE820 (Private, &CopySize);
  if (CopySize > Private->Legacy16Table->E820Length) {
    ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
    Regs.X.AX = Legacy16GetTableAddress;
    Regs.X.CX = (UINT16) CopySize;
    Regs.X.DX = 1;	
    Private->LegacyBios.FarCall86 (
      &Private->LegacyBios,
      Private->Legacy16Table->Compatibility16CallSegment,
      Private->Legacy16Table->Compatibility16CallOffset,
      &Regs,
      NULL,
      0
      );

    Private->Legacy16Table->E820Pointer = (UINT32) (Regs.X.DS * 16 + Regs.X.BX);
    Private->Legacy16Table->E820Length  = (UINT32) CopySize;
    if (Regs.X.AX != 0) {
      DEBUG ((EFI_D_ERROR, "Legacy16 E820 length insufficient\n"));
    } else {
      CopyMem (
        (VOID *)(UINTN) Private->Legacy16Table->E820Pointer,
        Private->E820Table,
        CopySize
        );
    }
  } else {
    CopyMem (
      (VOID *)(UINTN) Private->Legacy16Table->E820Pointer,
      Private->E820Table,
      CopySize
      );
    Private->Legacy16Table->E820Length = (UINT32) CopySize;
  }

  //
  // We do not ASSERT if SmbiosTable not found. It is possible that a platform does not produce SmbiosTable.
  //
  if (mReserveSmbiosEntryPoint == 0) {
    DEBUG ((EFI_D_INFO, "Smbios table is not found!\n"));
  }
  CreateSmbiosTableInReservedMemory ();
  EfiToLegacy16BootTable->SmbiosTable = (UINT32)(UINTN)mReserveSmbiosEntryPoint;

  AcpiTable = NULL;
  Status = EfiGetSystemConfigurationTable (
             &gEfiAcpi20TableGuid,
             &AcpiTable
             );
  if (EFI_ERROR (Status)) {
    Status = EfiGetSystemConfigurationTable (
               &gEfiAcpi10TableGuid,
               &AcpiTable
               );
  }
  //
  // We do not ASSERT if AcpiTable not found. It is possible that a platform does not produce AcpiTable.
  //
  if (AcpiTable == NULL) {
    DEBUG ((EFI_D_INFO, "ACPI table is not found!\n"));
  }
  EfiToLegacy16BootTable->AcpiTable = (UINT32)(UINTN)AcpiTable;

  //
  // Get RSD Ptr table rev at offset 15 decimal
  // Rev = 0 Length is 20 decimal
  // Rev != 0 Length is UINT32 at offset 20 decimal
  //
  if (AcpiTable != NULL) {

    AcpiPtr = AcpiTable;
    if (*((UINT8 *) AcpiPtr + 15) == 0) {
      CopySize = 20;
    } else {
      AcpiPtr   = ((UINT8 *) AcpiPtr + 20);
      CopySize  = (*(UINT32 *) AcpiPtr);
    }

    CopyMem (
      (VOID *)(UINTN) Private->Legacy16Table->AcpiRsdPtrPointer,
      AcpiTable,
      CopySize
      );
  }
  //
  // Make sure all PCI Interrupt Line register are programmed to match 8259
  //
  PciProgramAllInterruptLineRegisters (Private);

  //
  // Unlock the Legacy BIOS region as PciProgramAllInterruptLineRegisters
  // can lock it.
  //
  //Private->LegacyRegion->UnLock (
  //                         Private->LegacyRegion,
  //                         Private->BiosStart,
  //                         Private->LegacyBiosImageSize,
  //                         &Granularity
  //                         );

  //
  // Configure Legacy Device Magic
  //
  // Only do this code if booting legacy OS
  //
  if ((mBootMode == BOOT_LEGACY_OS) || (mBootMode == BOOT_UNCONVENTIONAL_DEVICE)) {
    UpdateSioData (Private);
  }
  //
  // Setup BDA and EBDA standard areas before Legacy Boot
  //
  LegacyBiosCompleteBdaBeforeBoot (Private);
  LegacyBiosCompleteStandardCmosBeforeBoot (Private);

  //
  // We must build IDE data, if it hasn't been done, before PciShadowRoms
  // to insure EFI drivers are connected.
  //
  LegacyBiosBuildIdeData (Private, &HddInfo, 1);
  UpdateAllIdentifyDriveData (Private);

  //
  // Clear IO BAR, if IDE controller in legacy mode.
  //
  InitLegacyIdeController (IdeController);

  //
  // Generate number of ticks since midnight for BDA. DOS requires this
  // for its time. We have to make assumptions as to how long following
  // code takes since after PciShadowRoms PciIo is gone. Place result in
  // 40:6C-6F
  //
  // Adjust value by 1 second.
  //
  gRT->GetTime (&BootTime, NULL);
  LocalTime = BootTime.Hour * 3600 + BootTime.Minute * 60 + BootTime.Second;
  LocalTime += 1;

  //
  // Multiply result by 18.2 for number of ticks since midnight.
  // Use 182/10 to avoid floating point math.
  //
  LocalTime = (LocalTime * 182) / 10;
  BdaPtr    = (UINT32 *) (UINTN)0x46C;
  *BdaPtr   = LocalTime;

  //
  // Shadow PCI ROMs. We must do this near the end since this will kick
  // of Native EFI drivers that may be needed to collect info for Legacy16
  //
  //  WARNING: PciIo is gone after this call.
  //
  PciShadowRoms (Private);

  //
  // Shadow PXE base code, BIS etc.
  //
  //Private->LegacyRegion->UnLock (Private->LegacyRegion, 0xc0000, 0x40000, &Granularity);
  ShadowAddress = Private->OptionRom;
  Private->LegacyBiosPlatform->PlatformHooks (
                                 Private->LegacyBiosPlatform,
                                 EfiPlatformHookShadowServiceRoms,
                                 0,
                                 0,
                                 &ShadowAddress,
                                 Legacy16Table,
                                 NULL
                                 );
  Private->OptionRom = (UINT32)ShadowAddress;
  //
  // Register Legacy SMI Handler
  //
  LegacyBiosPlatform->SmmInit (
                        LegacyBiosPlatform,
                        EfiToLegacy16BootTable
                        );

  //
  // Let platform code know the boot options
  //
  LegacyBiosGetBbsInfo (
    This,
    &HddCount,
    &LocalHddInfo,
    &BbsCount,
    &LocalBbsTable
    );

  DEBUG_CODE (
    PrintPciInterruptRegister ();
    PrintBbsTable (LocalBbsTable, MAX_BBS_ENTRIES);
//- PrintHddInfo (LocalHddInfo);
    );
  //
  // If drive wasn't spun up then BuildIdeData may have found new drives.
  // Need to update BBS boot priority.
  //
  for (Index = 0; Index < MAX_IDE_CONTROLLER; Index++) {
    if ((LocalHddInfo[Index].IdentifyDrive[0].Raw[0] != 0) &&
        (LocalBbsTable[2 * Index + 1].BootPriority == BBS_IGNORE_ENTRY)
        ) {
      LocalBbsTable[2 * Index + 1].BootPriority = BBS_UNPRIORITIZED_ENTRY;
    }

    if ((LocalHddInfo[Index].IdentifyDrive[1].Raw[0] != 0) &&
        (LocalBbsTable[2 * Index + 2].BootPriority == BBS_IGNORE_ENTRY)
        ) {
      LocalBbsTable[2 * Index + 2].BootPriority = BBS_UNPRIORITIZED_ENTRY;
    }
  }

  //Private->LegacyRegion->UnLock (
  //                         Private->LegacyRegion,
  //                         0xc0000,
  //                         0x40000,
  //                         &Granularity
  //                         );

  LegacyBiosPlatform->PrepareToBoot (
                        LegacyBiosPlatform,
                        mBbsDevicePathPtr,
                        mBbsTable,
                        mLoadOptionsSize,
                        mLoadOptions,
                        (VOID *) &Private->IntThunk->EfiToLegacy16BootTable
                        );

  //
  // If no boot device return to BDS
  //
  if ((mBootMode == BOOT_LEGACY_OS) || (mBootMode == BOOT_UNCONVENTIONAL_DEVICE)) {
    for (Index = 0; Index < BbsCount; Index++){
      if ((LocalBbsTable[Index].BootPriority != BBS_DO_NOT_BOOT_FROM) &&
          (LocalBbsTable[Index].BootPriority != BBS_UNPRIORITIZED_ENTRY) &&
          (LocalBbsTable[Index].BootPriority != BBS_IGNORE_ENTRY)) {
        break;
      }
    }
    if (Index == BbsCount) {
      return EFI_DEVICE_ERROR;
    }
  }
  //
  // Let the Legacy16 code know the device path type for legacy boot
  //
  EfiToLegacy16BootTable->DevicePathType = mBbsDevicePathPtr->DeviceType;

  //
  // Copy MP table, if it exists.
  //
  LegacyGetDataOrTable (This, EfiGetPlatformBinaryMpTable);

  if (!Private->LegacyBootEntered) {
    //
    // Copy OEM INT Data, if it exists. Note: This code treats any data
    // as a bag of bits and knows nothing of the contents nor cares.
    // Contents are IBV specific.
    //
    LegacyGetDataOrTable (This, EfiGetPlatformBinaryOemIntData);

    //
    // Copy OEM16 Data, if it exists.Note: This code treats any data
    // as a bag of bits and knows nothing of the contents nor cares.
    // Contents are IBV specific.
    //
    LegacyGetDataOrTable (This, EfiGetPlatformBinaryOem16Data);

    //
    // Copy OEM32 Data, if it exists.Note: This code treats any data
    // as a bag of bits and knows nothing of the contents nor cares.
    // Contents are IBV specific.
    //
    LegacyGetDataOrTable (This, EfiGetPlatformBinaryOem32Data);

    PrepareLegacyPxeRetry(Private);
    if(gLegacy2UefiCtx.Enable){
      if(!gPxeRetryCtx.Ready){
        PrepareLegacy2Uefi(Private);
      } else {
        SyncPxeRetry2Legacy2Uefi();
      }
    }
  }

  //
  // Call into Legacy16 code to prepare for INT 19h
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16PrepareToBoot;

  //
  // Pass in handoff data
  //
  Regs.X.ES = NORMALIZE_EFI_SEGMENT ((UINTN)EfiToLegacy16BootTable);
  Regs.X.BX = NORMALIZE_EFI_OFFSET ((UINTN)EfiToLegacy16BootTable);

  Private->LegacyBios.FarCall86 (
    This,
    Private->Legacy16CallSegment,
    Private->Legacy16CallOffset,
    &Regs,
    NULL,
    0
    );

  if (Regs.X.AX != 0) {
    return EFI_DEVICE_ERROR;
  }
  //
  // Lock the Legacy BIOS region
  //
  //Private->LegacyRegion->Lock (
  //                        Private->LegacyRegion,
  //                         0xc0000,
  //                         0x40000,
  //                         &Granularity
  //                         );
  //
  // Lock attributes of the Legacy Region if chipset supports
  //
  Private->LegacyRegion->BootLock (
                           Private->LegacyRegion,
                           0xc0000,
                           0x40000,
                           &Granularity
                           );

  //
  // Call into Legacy16 code to do the INT 19h
  //
  EnableAllControllers (Private);

_LegacyBoot:  
  if ((mBootMode == BOOT_LEGACY_OS) || (mBootMode == BOOT_UNCONVENTIONAL_DEVICE)) {
    //
    // Report Status Code to indicate legacy boot event will be signalled
    //
    REPORT_STATUS_CODE (
      EFI_PROGRESS_CODE,
      (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_LEGACY_BOOT_EVENT)
      );

    //
    // Signal all the events that are waiting on EVT_SIGNAL_LEGACY_BOOT
    //
    EfiSignalEventLegacyBoot ();

    DEBUG ((EFI_D_INFO, "Legacy INT19 Boot...\n"));

    //
    // Disable DXE Timer while executing in real mode
    //
    Private->Timer->GetTimerPeriod (Private->Timer, &gLegacy2UefiCtx.TimerPeriod);
    Private->Timer->SetTimerPeriod (Private->Timer, 0);
    
    //
    // Save and disable interrupt of debug timer
    //
    SaveAndSetDebugTimerInterrupt (FALSE);


    //
    // Put the 8259 into its legacy mode by reprogramming the vector bases
    //
    Private->Legacy8259->SetVectorBase (Private->Legacy8259, LEGACY_MODE_BASE_VECTOR_MASTER, LEGACY_MODE_BASE_VECTOR_SLAVE);
    //
    // PC History
    //   The original PC used INT8-F for master PIC. Since these mapped over
    //   processor exceptions TIANO moved the master PIC to INT68-6F.
    // We need to set these back to the Legacy16 unexpected interrupt(saved
    // in LegacyBios.c) since some OS see that these have values different from
    // what is expected and invoke them. Since the legacy OS corrupts EFI
    // memory, there is no handler for these interrupts and OS blows up.
    //
    // We need to save the TIANO values for the rare case that the Legacy16
    // code cannot boot but knows memory hasn't been destroyed.
    //
    // To compound the problem, video takes over one of these INTS and must be
    // be left.
    // @bug - determine if video hooks INT(in which case we must find new
    //          set of TIANO vectors) or takes it over.
    //
    //
    BaseVectorMaster = (UINT32 *) (sizeof (UINT32) * PcdGet8(Pcd8259ProtectedModeBaseVectorMaster));
    for (Index = 0; Index < 8; Index++) {
      Private->ThunkSavedInt[Index] = BaseVectorMaster[Index];
      if (Private->ThunkSeg == (UINT16) (BaseVectorMaster[Index] >> 16)) {
        BaseVectorMaster[Index] = (UINT32) (Private->BiosUnexpectedInt);
      }
    }
    
    ((BDA_STRUC*)(UINTN)0x400)->Com1 = 0;   
    ((BDA_STRUC*)(UINTN)0x400)->Com2 = 0; 

    if (mLegacyPxeRetriedFlag && PcdGetBool(PcdIsAutoBootFromBootBootOptions)) {
      //
      // Ignore lan entries of BbsTable
      //
      IgnoreAllNetEntries();
    }
    LegacyBootAndRetryPolicies(Private);
    if (mRetryPolicy.GroupRetryTime != 0) {
      PcdSet8S(PcdRetryPxeBoot, mOldPxeRetryValue);
    }

    if(gPxeRetryCtx.Ready && !mLegacyAllAndGroupRetied){
      HandlePxeRetry(Private);
      HandleRemainEntries(Private);
    }

    if(!gLegacy2UefiCtx.Ready){
      ShowLegacyNoBootDev(Private);
      *gSystemInLegacyPtr = 1;
      CpuDeadLoop();
      
    } else {
      Private->Legacy8259->SetVectorBase (Private->Legacy8259, PcdGet8(Pcd8259ProtectedModeBaseVectorMaster), PROTECTED_MODE_BASE_VECTOR_SLAVE);
      Private->Timer->SetTimerPeriod (Private->Timer, gLegacy2UefiCtx.TimerPeriod);
      SetVesaVideoMode(This, gLegacy2UefiCtx.VideoMode);
      RestoreBbstablePriority();
    }

    BaseVectorMaster = (UINT32 *) (sizeof (UINT32) * PcdGet8(Pcd8259ProtectedModeBaseVectorMaster));
    for (Index = 0; Index < 8; Index++) {
      BaseVectorMaster[Index] = Private->ThunkSavedInt[Index];
    }
  }
  Private->LegacyBootEntered = TRUE;
  SignalEventLegacyBootExit();
  ReConnectUefiDriver();  

  //
  // Clear the BootStart bit to enable mouse in the setup after exiting legacy environment.
  //
  EBdaSeg = *(UINT16 *)(UINTN)0x40E;
  BootCfg  = (BOOT_CFG *)(UINTN)(((UINT32)EBdaSeg << 4) + 0x105);
  BootCfg->BootStart = 0;
  
  if ((mBootMode == BOOT_LEGACY_OS) || (mBootMode == BOOT_UNCONVENTIONAL_DEVICE)) {
    //
    // Should never return unless never passed control to 0:7c00(first stage
    // OS loader) and only then if no bootable device found.
    //
    return EFI_DEVICE_ERROR;
  } else {
    //
    // If boot to EFI then expect to return to caller
    //
    return EFI_SUCCESS;
  }
}


/**
  Assign drive number to legacy HDD drives prior to booting an EFI
  aware OS so the OS can access drives without an EFI driver.
  Note: BBS compliant drives ARE NOT available until this call by
  either shell or EFI.

  @param  This                    Protocol instance pointer.
  @param  BbsCount                Number of BBS_TABLE structures
  @param  BbsTable                List BBS entries

  @retval EFI_SUCCESS             Drive numbers assigned

**/
EFI_STATUS
EFIAPI
LegacyBiosPrepareToBootEfi (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  OUT UINT16                          *BbsCount,
  OUT BBS_TABLE                       **BbsTable
  )
{
  EFI_STATUS                        Status;
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;
  LEGACY_BIOS_INSTANCE              *Private;

  Private                 = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  EfiToLegacy16BootTable  = &Private->IntThunk->EfiToLegacy16BootTable;
  mBootMode               = BOOT_EFI_OS;
  mBbsDevicePathPtr       = NULL;
  Status                  = GenericLegacyBoot (This);
  *BbsTable               = (BBS_TABLE*)(UINTN)EfiToLegacy16BootTable->BbsTable;
  *BbsCount               = (UINT16) (sizeof (Private->IntThunk->BbsTable) / sizeof (BBS_TABLE));
  return Status;
}

/**
  To boot from an unconventional device like parties and/or execute HDD diagnostics.

  @param  This            Protocol instance pointer.
  @param  Attributes      How to interpret the other input parameters
  @param  BbsEntry        The 0-based index into the BbsTable for the parent
                          device.
  @param  BeerData        Pointer to the 128 bytes of ram BEER data.
  @param  ServiceAreaData Pointer to the 64 bytes of raw Service Area data. The
                          caller must provide a pointer to the specific Service
                          Area and not the start all Service Areas.

  @retval EFI_INVALID_PARAMETER if error. Does NOT return if no error.

***/
EFI_STATUS
EFIAPI
LegacyBiosBootUnconventionalDevice (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  IN UDC_ATTRIBUTES                   Attributes,
  IN UINTN                            BbsEntry,
  IN VOID                             *BeerData,
  IN VOID                             *ServiceAreaData
  )
{
  EFI_STATUS                        Status;
  EFI_TO_COMPATIBILITY16_BOOT_TABLE *EfiToLegacy16BootTable;
  LEGACY_BIOS_INSTANCE              *Private;
  UD_TABLE                          *UcdTable;
  UINTN                             Index;
  UINT16                            BootPriority;
  BBS_TABLE                         *BbsTable;

  BootPriority = 0;
  Private = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  mBootMode = BOOT_UNCONVENTIONAL_DEVICE;
  mBbsDevicePathPtr = &mBbsDevicePathNode;
  mAttributes = Attributes;
  mBbsEntry = BbsEntry;
  mBeerData = BeerData, mServiceAreaData = ServiceAreaData;

  EfiToLegacy16BootTable = &Private->IntThunk->EfiToLegacy16BootTable;

  //
  // Do input parameter checking
  //
  if ((Attributes.DirectoryServiceValidity == 0) &&
      (Attributes.RabcaUsedFlag == 0) &&
      (Attributes.ExecuteHddDiagnosticsFlag == 0)
      ) {
    return EFI_INVALID_PARAMETER;
  }

  if (((Attributes.DirectoryServiceValidity != 0) && (ServiceAreaData == NULL)) ||
      (((Attributes.DirectoryServiceValidity | Attributes.RabcaUsedFlag) != 0) && (BeerData == NULL))
      ) {
    return EFI_INVALID_PARAMETER;
  }

  UcdTable = (UD_TABLE *) AllocatePool (
                            sizeof (UD_TABLE)
                            );
  if (NULL == UcdTable) {
    return EFI_OUT_OF_RESOURCES;
  }

  EfiToLegacy16BootTable->UnconventionalDeviceTable = (UINT32)(UINTN)UcdTable;
  UcdTable->Attributes = Attributes;
  UcdTable->BbsTableEntryNumberForParentDevice = (UINT8) BbsEntry;
  //
  // Force all existing BBS entries to DoNotBoot. This allows 16-bit CSM
  // to assign drive numbers but bot boot from. Only newly created entries
  // will be valid.
  //
  BbsTable = (BBS_TABLE*)(UINTN)EfiToLegacy16BootTable->BbsTable;
  for (Index = 0; Index < EfiToLegacy16BootTable->NumberBbsEntries; Index++) {
    BbsTable[Index].BootPriority = BBS_DO_NOT_BOOT_FROM;
  }
  //
  // If parent is onboard IDE then assign controller & device number
  // else they are 0.
  //
  if (BbsEntry < MAX_IDE_CONTROLLER * 2) {
    UcdTable->DeviceNumber = (UINT8) ((BbsEntry - 1) % 2);
  }

  if (BeerData != NULL) {
    CopyMem (
      (VOID *) UcdTable->BeerData,
      BeerData,
      (UINTN) 128
      );
  }

  if (ServiceAreaData != NULL) {
    CopyMem (
      (VOID *) UcdTable->ServiceAreaData,
      ServiceAreaData,
      (UINTN) 64
      );
  }
  //
  // For each new entry do the following:
  //   1. Increment current number of BBS entries
  //   2. Copy parent entry to new entry.
  //   3. Zero out BootHandler Offset & segment
  //   4. Set appropriate device type. BEV(0x80) for HDD diagnostics
  //      and Floppy(0x01) for PARTIES boot.
  //   5. Assign new priority.
  //
  if ((Attributes.ExecuteHddDiagnosticsFlag) != 0) {
    EfiToLegacy16BootTable->NumberBbsEntries += 1;

    CopyMem (
      (VOID *) &BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].BootPriority,
      (VOID *) &BbsTable[BbsEntry].BootPriority,
      sizeof (BBS_TABLE)
      );

    BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].BootHandlerOffset  = 0;
    BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].BootHandlerSegment = 0;
    BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].DeviceType         = 0x80;

    UcdTable->BbsTableEntryNumberForHddDiag = (UINT8) (EfiToLegacy16BootTable->NumberBbsEntries - 1);

    BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].BootPriority = BootPriority;
    BootPriority += 1;

    //
    // Set device type as BBS_TYPE_DEV for PARTIES diagnostic
    //
    mBbsDevicePathNode.DeviceType = BBS_TYPE_BEV;
  }

  if (((Attributes.DirectoryServiceValidity | Attributes.RabcaUsedFlag)) != 0) {
    EfiToLegacy16BootTable->NumberBbsEntries += 1;
    CopyMem (
      (VOID *) &BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].BootPriority,
      (VOID *) &BbsTable[BbsEntry].BootPriority,
      sizeof (BBS_TABLE)
      );

    BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].BootHandlerOffset  = 0;
    BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].BootHandlerSegment = 0;
    BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].DeviceType         = 0x01;
    UcdTable->BbsTableEntryNumberForBoot = (UINT8) (EfiToLegacy16BootTable->NumberBbsEntries - 1);
    BbsTable[EfiToLegacy16BootTable->NumberBbsEntries].BootPriority = BootPriority;

    //
    // Set device type as BBS_TYPE_FLOPPY for PARTIES boot as floppy
    //
    mBbsDevicePathNode.DeviceType = BBS_TYPE_FLOPPY;
  }
  //
  // Build the BBS Device Path for this boot selection
  //
  mBbsDevicePathNode.Header.Type    = BBS_DEVICE_PATH;
  mBbsDevicePathNode.Header.SubType = BBS_BBS_DP;
  SetDevicePathNodeLength (&mBbsDevicePathNode.Header, sizeof (BBS_BBS_DEVICE_PATH));
  mBbsDevicePathNode.StatusFlag = 0;
  mBbsDevicePathNode.String[0]  = 0;

  Status                        = GenericLegacyBoot (This);
  return Status;
}

/**
  Attempt to legacy boot the BootOption. If the EFI context has been
  compromised this function will not return.

  @param  This             Protocol instance pointer.
  @param  BbsDevicePath    EFI Device Path from BootXXXX variable.
  @param  LoadOptionsSize  Size of LoadOption in size.
  @param  LoadOptions      LoadOption from BootXXXX variable

  @retval EFI_SUCCESS      Removable media not present

**/
EFI_STATUS
EFIAPI
LegacyBiosLegacyBoot (
  IN EFI_LEGACY_BIOS_PROTOCOL           *This,
  IN  BBS_BBS_DEVICE_PATH               *BbsDevicePath,
  IN  UINT32                            LoadOptionsSize,
  IN  VOID                              *LoadOptions
  )
{
  EFI_STATUS  Status;

  mBbsDevicePathPtr = BbsDevicePath;
  mLoadOptionsSize  = LoadOptionsSize;
  mLoadOptions      = LoadOptions;
  mBootMode         = BOOT_LEGACY_OS;
  Status            = GenericLegacyBoot (This);

  return Status;
}

/**
  Convert EFI Memory Type to E820 Memory Type.

  @param  Type  EFI Memory Type

  @return ACPI Memory Type for EFI Memory Type

**/
EFI_ACPI_MEMORY_TYPE
EfiMemoryTypeToE820Type (
  IN  UINT32    Type
  )
{
  switch (Type) {
  case EfiLoaderCode:
  case EfiLoaderData:
  case EfiBootServicesCode:
  case EfiBootServicesData:
  case EfiConventionalMemory:
//case EfiRuntimeServicesCode:
//case EfiRuntimeServicesData:
    return EfiAcpiAddressRangeMemory;

  case EfiPersistentMemory:
    return EfiAddressRangePersistentMemory;

  case EfiACPIReclaimMemory:
    return EfiAcpiAddressRangeACPI;

  case EfiACPIMemoryNVS:
    return EfiAcpiAddressRangeNVS;

  //
  // All other types map to reserved.
  // Adding the code just waists FLASH space.
  //
  //  case  EfiReservedMemoryType:
  //  case  EfiUnusableMemory:
  //  case  EfiMemoryMappedIO:
  //  case  EfiMemoryMappedIOPortSpace:
  //  case  EfiPalCode:
  //  case  EfiRuntimeServicesCode:
  //  case  EfiRuntimeServicesData:
  //
  default:
    return EfiAcpiAddressRangeReserved;
  }
}

/**
  Build the E820 table.

  @param  Private  Legacy BIOS Instance data
  @param  Size     Size of E820 Table

  @retval EFI_SUCCESS  It should always work.

**/
EFI_STATUS
LegacyBiosBuildE820 (
  IN  LEGACY_BIOS_INSTANCE    *Private,
  OUT UINTN                   *Size
  )
{
  EFI_STATUS                  Status;
  EFI_E820_ENTRY64            *E820Table;
  EFI_MEMORY_DESCRIPTOR       *EfiMemoryMap;
  EFI_MEMORY_DESCRIPTOR       *EfiMemoryMapEnd;
  EFI_MEMORY_DESCRIPTOR       *EfiEntry;
  EFI_MEMORY_DESCRIPTOR       *NextEfiEntry;
  EFI_MEMORY_DESCRIPTOR       TempEfiEntry;
  UINTN                       EfiMemoryMapSize;
  UINTN                       EfiMapKey;
  UINTN                       EfiDescriptorSize;
  UINT32                      EfiDescriptorVersion;
  UINTN                       Index;
  EFI_PEI_HOB_POINTERS        Hob;
  EFI_HOB_RESOURCE_DESCRIPTOR *ResourceHob;
  UINTN                       TempIndex;
  UINTN                       IndexSort;
  UINTN                       TempNextIndex;
  EFI_E820_ENTRY64            TempE820;
  EFI_ACPI_MEMORY_TYPE        TempType;
  BOOLEAN                     ChangedFlag;
  UINTN                       Above1MIndex;
  UINT64                      MemoryBlockLength;

  E820Table = (EFI_E820_ENTRY64 *) Private->E820Table;

  //
  // Get the EFI memory map.
  //
  EfiMemoryMapSize  = 0;
  EfiMemoryMap      = NULL;
  Status = gBS->GetMemoryMap (
                  &EfiMemoryMapSize,
                  EfiMemoryMap,
                  &EfiMapKey,
                  &EfiDescriptorSize,
                  &EfiDescriptorVersion
                  );
  ASSERT (Status == EFI_BUFFER_TOO_SMALL);

  do {
    //
    // Use size returned back plus 1 descriptor for the AllocatePool.
    // We don't just multiply by 2 since the "for" loop below terminates on
    // EfiMemoryMapEnd which is dependent upon EfiMemoryMapSize. Otherwise
    // we process bogus entries and create bogus E820 entries.
    //
    EfiMemoryMap = (EFI_MEMORY_DESCRIPTOR *) AllocatePool (EfiMemoryMapSize);
    ASSERT (EfiMemoryMap != NULL);
    Status = gBS->GetMemoryMap (
                    &EfiMemoryMapSize,
                    EfiMemoryMap,
                    &EfiMapKey,
                    &EfiDescriptorSize,
                    &EfiDescriptorVersion
                    );
    if (EFI_ERROR (Status)) {
      FreePool (EfiMemoryMap);
    }
  } while (Status == EFI_BUFFER_TOO_SMALL);

  ASSERT_EFI_ERROR (Status);

  //
  // Punch in the E820 table for memory less than 1 MB.
  // Assume ZeroMem () has been done on data structure.
  //
  //
  // First entry is 0 to (640k - EBDA)
  //
  E820Table[0].BaseAddr  = 0;
  E820Table[0].Length    = (UINT64) ((*(UINT16 *) (UINTN)0x40E) << 4);
  E820Table[0].Type      = EfiAcpiAddressRangeMemory;

  //
  // Second entry is (640k - EBDA) to 640k
  //
  E820Table[1].BaseAddr  = E820Table[0].Length;
  E820Table[1].Length    = (UINT64) ((640 * 1024) - E820Table[0].Length);
  E820Table[1].Type      = EfiAcpiAddressRangeReserved;

  //
  // Third Entry is legacy BIOS
  // DO NOT CLAIM region from 0xA0000-0xDFFFF. OS can use free areas
  // to page in memory under 1MB.
  // Omit region from 0xE0000 to start of BIOS, if any. This can be
  // used for a multiple reasons including OPROMS.
  //

  //
  // The CSM binary image size is not the actually size that CSM binary used,
  // to avoid memory corrupt, we declare the 0E0000 - 0FFFFF is used by CSM binary.
  //
  E820Table[2].BaseAddr  = 0xE0000;
  E820Table[2].Length    = 0x20000;
  E820Table[2].Type      = EfiAcpiAddressRangeReserved;

  Above1MIndex = 2;

  //
  // Process the EFI map to produce E820 map;
  //

  //
  // Sort memory map from low to high
  //
  EfiEntry        = EfiMemoryMap;
  NextEfiEntry    = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  while (EfiEntry < EfiMemoryMapEnd) {
    while (NextEfiEntry < EfiMemoryMapEnd) {
      if (EfiEntry->PhysicalStart > NextEfiEntry->PhysicalStart) {
        CopyMem (&TempEfiEntry, EfiEntry, sizeof (EFI_MEMORY_DESCRIPTOR));
        CopyMem (EfiEntry, NextEfiEntry, sizeof (EFI_MEMORY_DESCRIPTOR));
        CopyMem (NextEfiEntry, &TempEfiEntry, sizeof (EFI_MEMORY_DESCRIPTOR));
      }

      NextEfiEntry = NEXT_MEMORY_DESCRIPTOR (NextEfiEntry, EfiDescriptorSize);
    }

    EfiEntry      = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
    NextEfiEntry  = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }

  EfiEntry        = EfiMemoryMap;
  EfiMemoryMapEnd = (EFI_MEMORY_DESCRIPTOR *) ((UINT8 *) EfiMemoryMap + EfiMemoryMapSize);
  for (Index = Above1MIndex; (EfiEntry < EfiMemoryMapEnd) && (Index < EFI_MAX_E820_ENTRY - 1); ) {
    MemoryBlockLength = (UINT64) (LShiftU64 (EfiEntry->NumberOfPages, 12));
    if ((EfiEntry->PhysicalStart + MemoryBlockLength) < 0x100000) {
      //
      // Skip the memory block is under 1MB
      //
    } else {
      if (EfiEntry->PhysicalStart < 0x100000) {
        //
        // When the memory block spans below 1MB, ensure the memory block start address is at least 1MB
        //
        MemoryBlockLength       -= 0x100000 - EfiEntry->PhysicalStart;
        EfiEntry->PhysicalStart =  0x100000;
      }

      //
      // Convert memory type to E820 type
      //
      TempType = EfiMemoryTypeToE820Type (EfiEntry->Type);

      if ((E820Table[Index].Type == TempType) && (EfiEntry->PhysicalStart == (E820Table[Index].BaseAddr + E820Table[Index].Length))) {
        //
        // Grow an existing entry
        //
        E820Table[Index].Length += MemoryBlockLength;
      } else {
        //
        // Make a new entry
        //
        ++Index;
        E820Table[Index].BaseAddr  = EfiEntry->PhysicalStart;
        E820Table[Index].Length    = MemoryBlockLength;
        E820Table[Index].Type      = TempType;
      }
    }
    EfiEntry = NEXT_MEMORY_DESCRIPTOR (EfiEntry, EfiDescriptorSize);
  }

  FreePool (EfiMemoryMap);

  //
  // Process the reserved memory map to produce E820 map ;
  //
  for (Hob.Raw = GetHobList (); !END_OF_HOB_LIST (Hob); Hob.Raw = GET_NEXT_HOB (Hob)) {
    if (Hob.Raw != NULL && GET_HOB_TYPE (Hob) == EFI_HOB_TYPE_RESOURCE_DESCRIPTOR) {
      ResourceHob = Hob.ResourceDescriptor;
      if (((ResourceHob->ResourceType == EFI_RESOURCE_FIRMWARE_DEVICE)  ||
          (ResourceHob->ResourceType == EFI_RESOURCE_MEMORY_RESERVED)    ) &&
          (ResourceHob->PhysicalStart > 0x100000) &&
          (Index < EFI_MAX_E820_ENTRY - 1)) {
        ++Index;
        E820Table[Index].BaseAddr  = ResourceHob->PhysicalStart;
        E820Table[Index].Length    = ResourceHob->ResourceLength;
        E820Table[Index].Type      = EfiAcpiAddressRangeReserved;
      }
    }
  }

  Index ++;
  Private->IntThunk->EfiToLegacy16InitTable.NumberE820Entries = (UINT32)Index;
  Private->IntThunk->EfiToLegacy16BootTable.NumberE820Entries = (UINT32)Index;
  Private->NumberE820Entries = (UINT32)Index;
  *Size = (UINTN) (Index * sizeof (EFI_E820_ENTRY64));

  //
  // Sort E820Table from low to high
  //
  for (TempIndex = 0; TempIndex < Index; TempIndex++) {
    ChangedFlag = FALSE;
    for (TempNextIndex = 1; TempNextIndex < Index - TempIndex; TempNextIndex++) {
      if (E820Table[TempNextIndex - 1].BaseAddr > E820Table[TempNextIndex].BaseAddr) {
        ChangedFlag                       = TRUE;
        TempE820.BaseAddr                 = E820Table[TempNextIndex - 1].BaseAddr;
        TempE820.Length                   = E820Table[TempNextIndex - 1].Length;
        TempE820.Type                     = E820Table[TempNextIndex - 1].Type;

        E820Table[TempNextIndex - 1].BaseAddr  = E820Table[TempNextIndex].BaseAddr;
        E820Table[TempNextIndex - 1].Length    = E820Table[TempNextIndex].Length;
        E820Table[TempNextIndex - 1].Type      = E820Table[TempNextIndex].Type;

        E820Table[TempNextIndex].BaseAddr      = TempE820.BaseAddr;
        E820Table[TempNextIndex].Length        = TempE820.Length;
        E820Table[TempNextIndex].Type          = TempE820.Type;
      }
    }

    if (!ChangedFlag) {
      break;
    }
  }

  //
  // Remove the overlap range
  //
  for (TempIndex = 1; TempIndex < Index; TempIndex++) {
    if (E820Table[TempIndex - 1].BaseAddr <= E820Table[TempIndex].BaseAddr &&
        ((E820Table[TempIndex - 1].BaseAddr + E820Table[TempIndex - 1].Length) >=
         (E820Table[TempIndex].BaseAddr +E820Table[TempIndex].Length))) {
        //
        //Overlap range is found
        //
        ASSERT (E820Table[TempIndex - 1].Type == E820Table[TempIndex].Type);

        if (TempIndex == Index - 1) {
          E820Table[TempIndex].BaseAddr = 0;
          E820Table[TempIndex].Length   = 0;
          E820Table[TempIndex].Type     = (EFI_ACPI_MEMORY_TYPE) 0;
          Index--;
          break;
        } else {
          for (IndexSort = TempIndex; IndexSort < Index - 1; IndexSort ++) {
            E820Table[IndexSort].BaseAddr = E820Table[IndexSort + 1].BaseAddr;
            E820Table[IndexSort].Length   = E820Table[IndexSort + 1].Length;
            E820Table[IndexSort].Type     = E820Table[IndexSort + 1].Type;
          }
          Index--;
       }
    }
  }



  Private->IntThunk->EfiToLegacy16InitTable.NumberE820Entries = (UINT32)Index;
  Private->IntThunk->EfiToLegacy16BootTable.NumberE820Entries = (UINT32)Index;
  Private->NumberE820Entries = (UINT32)Index;
  *Size = (UINTN) (Index * sizeof (EFI_E820_ENTRY64));

  //
  // Determine OS usable memory above 1Mb
  //
  Private->IntThunk->EfiToLegacy16BootTable.OsMemoryAbove1Mb = 0x0000;
  for (TempIndex = Above1MIndex; TempIndex < Index; TempIndex++) {
    if (E820Table[TempIndex].BaseAddr >= 0x100000 && E820Table[TempIndex].BaseAddr < 0x100000000ULL) { // not include above 4G memory
      //
      // ACPIReclaimMemory is also usable memory for ACPI OS, after OS dumps all ACPI tables.
      //
      if ((E820Table[TempIndex].Type == EfiAcpiAddressRangeMemory) || (E820Table[TempIndex].Type == EfiAcpiAddressRangeACPI)) {
        Private->IntThunk->EfiToLegacy16BootTable.OsMemoryAbove1Mb += (UINT32) (E820Table[TempIndex].Length);
      } else {
        break; // break at first not normal memory, because SMM may use reserved memory.
      }
    }
  }

  Private->IntThunk->EfiToLegacy16InitTable.OsMemoryAbove1Mb = Private->IntThunk->EfiToLegacy16BootTable.OsMemoryAbove1Mb;

  //
  // Print DEBUG information
  //
  for (TempIndex = 0; TempIndex < Index; TempIndex++) {
    DEBUG((EFI_D_INFO, "E820[%2d]: 0x%16lx ---- 0x%16lx, Type = 0x%x \n",
      TempIndex,
      E820Table[TempIndex].BaseAddr,
      (E820Table[TempIndex].BaseAddr + E820Table[TempIndex].Length),
      E820Table[TempIndex].Type
      ));
  }

  return EFI_SUCCESS;
}


/**
  Fill in the standard BDA and EBDA stuff prior to legacy Boot

  @param  Private      Legacy BIOS Instance data

  @retval EFI_SUCCESS  It should always work.

**/
EFI_STATUS
LegacyBiosCompleteBdaBeforeBoot (
  IN  LEGACY_BIOS_INSTANCE    *Private
  )
{
  BDA_STRUC                   *Bda;
  UINT16                      MachineConfig;
  DEVICE_PRODUCER_DATA_HEADER *SioPtr;

  Bda           = (BDA_STRUC *) ((UINTN) 0x400);
  MachineConfig = 0;

  SioPtr        = &(Private->IntThunk->EfiToLegacy16BootTable.SioData);
  Bda->Com1     = SioPtr->Serial[0].Address;
  Bda->Com2     = SioPtr->Serial[1].Address;
  Bda->Com3     = SioPtr->Serial[2].Address;
  Bda->Com4     = SioPtr->Serial[3].Address;

  if (SioPtr->Serial[0].Address != 0x00) {
    MachineConfig += 0x200;
  }

  if (SioPtr->Serial[1].Address != 0x00) {
    MachineConfig += 0x200;
  }

  if (SioPtr->Serial[2].Address != 0x00) {
    MachineConfig += 0x200;
  }

  if (SioPtr->Serial[3].Address != 0x00) {
    MachineConfig += 0x200;
  }

  Bda->Lpt1 = SioPtr->Parallel[0].Address;
  Bda->Lpt2 = SioPtr->Parallel[1].Address;
  Bda->Lpt3 = SioPtr->Parallel[2].Address;

  if (SioPtr->Parallel[0].Address != 0x00) {
    MachineConfig += 0x4000;
  }

  if (SioPtr->Parallel[1].Address != 0x00) {
    MachineConfig += 0x4000;
  }

  if (SioPtr->Parallel[2].Address != 0x00) {
    MachineConfig += 0x4000;
  }

  Bda->NumberOfDrives = (UINT8) (Bda->NumberOfDrives + Private->IdeDriveCount);
  if (SioPtr->Floppy.NumberOfFloppy != 0x00) {
    MachineConfig     = (UINT16) (MachineConfig + 0x01 + (SioPtr->Floppy.NumberOfFloppy - 1) * 0x40);
    Bda->FloppyXRate  = 0x07;
  }

  Bda->Lpt1_2Timeout  = 0x1414;
  Bda->Lpt3_4Timeout  = 0x1414;
  Bda->Com1_2Timeout  = 0x0101;
  Bda->Com3_4Timeout  = 0x0101;

  //
  // Force VGA and Coprocessor, indicate 101/102 keyboard
  //
  MachineConfig       = (UINT16) (MachineConfig + 0x00 + 0x02 + (SioPtr->MousePresent * 0x04));
  Bda->MachineConfig  = MachineConfig;

  return EFI_SUCCESS;
}

/**
  Fill in the standard BDA for Keyboard LEDs

  @param  This         Protocol instance pointer.
  @param  Leds         Current LED status

  @retval EFI_SUCCESS  It should always work.

**/
EFI_STATUS
EFIAPI
LegacyBiosUpdateKeyboardLedStatus (
  IN EFI_LEGACY_BIOS_PROTOCOL           *This,
  IN  UINT8                             Leds
  )
{
  LEGACY_BIOS_INSTANCE  *Private;
  BDA_STRUC             *Bda;
  UINT8                 LocalLeds;
  EFI_IA32_REGISTER_SET Regs;

  Bda                 = (BDA_STRUC *) ((UINTN) 0x400);

  Private             = LEGACY_BIOS_INSTANCE_FROM_THIS (This);
  LocalLeds           = Leds;
  Bda->LedStatus      = (UINT8) ((Bda->LedStatus &~0x07) | LocalLeds);
  LocalLeds           = (UINT8) (LocalLeds << 4);
  Bda->ShiftStatus    = (UINT8) ((Bda->ShiftStatus &~0x70) | LocalLeds);
  LocalLeds           = (UINT8) (Leds & 0x20);
  Bda->KeyboardStatus = (UINT8) ((Bda->KeyboardStatus &~0x20) | LocalLeds);
  //
  // Call into Legacy16 code to allow it to do any processing
  //
  ZeroMem (&Regs, sizeof (EFI_IA32_REGISTER_SET));
  Regs.X.AX = Legacy16SetKeyboardLeds;
  Regs.H.CL = Leds;

  Private->LegacyBios.FarCall86 (
    &Private->LegacyBios,
    Private->Legacy16Table->Compatibility16CallSegment,
    Private->Legacy16Table->Compatibility16CallOffset,
    &Regs,
    NULL,
    0
    );

  return EFI_SUCCESS;
}


/**
  Fill in the standard CMOS stuff prior to legacy Boot

  @param  Private      Legacy BIOS Instance data

  @retval EFI_SUCCESS  It should always work.

**/
EFI_STATUS
LegacyBiosCompleteStandardCmosBeforeBoot (
  IN  LEGACY_BIOS_INSTANCE    *Private
  )
{
  UINT8   Bda;
  UINT8   Floppy;
  UINT32  Size;

  //
  // Update CMOS locations
  // 10 floppy
  // 12,19,1A - ignore as OS don't use them and there is no standard due
  //            to large capacity drives
  // CMOS 14 = BDA 40:10 plus bit 3(display enabled)
  //
  Bda = (UINT8)(*((UINT8 *)((UINTN)0x410)) | BIT3);

  //
  // Force display enabled
  //
  Floppy = 0x00;
  if ((Bda & BIT0) != 0) {
    Floppy = BIT6;
  }

  //
  // Check if 2.88MB floppy set
  //
  if ((Bda & (BIT7 | BIT6)) != 0) {
    Floppy = (UINT8)(Floppy | BIT1);
  }

  LegacyWriteStandardCmos (CMOS_10, Floppy);
  LegacyWriteStandardCmos (CMOS_14, Bda);

  //
  // Force Status Register A to set rate selection bits and divider
  //
//LegacyWriteStandardCmos (CMOS_0A, 0x26);    // already set at PcRtcInit()

  //
  // redo memory size since it can change
  //
  Size = 0xFFFF;
  LegacyWriteStandardCmos (CMOS_17, (UINT8)(Size & 0xFF));
  LegacyWriteStandardCmos (CMOS_30, (UINT8)(Size & 0xFF));
  LegacyWriteStandardCmos (CMOS_18, (UINT8)(Size >> 8));
  LegacyWriteStandardCmos (CMOS_31, (UINT8)(Size >> 8));

  LegacyCalculateWriteStandardCmosChecksum ();

  return EFI_SUCCESS;
}

/**
  Relocate this image under 4G memory for IPF.

  @param  ImageHandle  Handle of driver image.
  @param  SystemTable  Pointer to system table.

  @retval EFI_SUCCESS  Image successfully relocated.
  @retval EFI_ABORTED  Failed to relocate image.

**/
EFI_STATUS
RelocateImageUnder4GIfNeeded (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  return EFI_SUCCESS;
}
