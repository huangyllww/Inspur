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
#include <IndustryStandard/Pci.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/PciSegmentLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/PciIo.h>

typedef struct{
  UINT8   BaseClass;
  UINT8   SubClass;
  UINT8   Interface;
  CHAR16  *Name;
} PCI_CLASS_INFO;

PCI_CLASS_INFO mPciClassCodeNameTable[] = {
  {0x00, 0x00, 0x00, L"compatible"},
  {0x00, 0x01, 0x00, L"VGA-compatible"},
  
  {0x01, 0x00, 0x00, L"SCSI bus controller"},
  {0x01, 0x01, 0xFF, L"IDE controller"},
  {0x01, 0x02, 0x00, L"Floppy disk controller"},
  {0x01, 0x03, 0x00, L"IPI bus controller"},
  {0x01, 0x04, 0x00, L"RAID controller"},
  {0x01, 0x05, 0x20, L"ATA controller with single DMA"},
  {0x01, 0x05, 0x30, L"ATA controller with chained DMA"},
  {0x01, 0x06, 0x00, L"SATA DPA"},
  {0x01, 0x06, 0x01, L"SATA AHCI"}, 
  {0x01, 0x08, 0x02, L"NVMe"},
  {0x01, 0x80, 0x00, L"Other mass storage controller"},
  
  {0x02, 0x00, 0x00, L"Ethernet controller"},
  {0x02, 0x01, 0x00, L"Token Ring controller"},
  {0x02, 0x02, 0x00, L"FDDI controller"},
  {0x02, 0x03, 0x00, L"ATM controller"},
  {0x02, 0x04, 0x00, L"ISDN controller"},
  {0x02, 0x05, 0x00, L"WorldFip controller"},
  {0x02, 0x06, 0xFF, L"PICMG 2.14 Multi Computing"},
  {0x02, 0x80, 0x00, L"Other network controller"},
  
  {0x03, 0x00, 0x00, L"VGA controller"},
  {0x03, 0x00, 0x01, L"8514-compatible controller"},
  {0x03, 0x01, 0x00, L"XGA controller"},
  {0x03, 0x02, 0x00, L"3D controller"},
  {0x03, 0x80, 0x00, L"Other display controller"},
  
  {0x04, 0x00, 0x00, L"Video device"},
  {0x04, 0x01, 0x00, L"Audio device"},
  {0x04, 0x02, 0x00, L"Computer telephony device"},
  {0x04, 0x80, 0x00, L"Other multimedia device"},
  
  {0x05, 0x00, 0x00, L"RAM"},
  {0x05, 0x01, 0x00, L"Flash"},
  {0x05, 0x80, 0x00, L"Other memory controller"},
  
  {0x06, 0x00, 0x00, L"Host bridge"},
  {0x06, 0x01, 0x00, L"ISA bridge"},
  {0x06, 0x02, 0x00, L"EISA bridge"},
  {0x06, 0x03, 0x00, L"MCA bridge"},
  {0x06, 0x04, 0x00, L"P2P bridge"},
  {0x06, 0x04, 0x01, L"SubDecode P2P bridge"},
  {0x06, 0x05, 0x00, L"PCMCIA bridge"},
  {0x06, 0x06, 0x00, L"NuBus bridge"},
  {0x06, 0x07, 0x00, L"CardBus bridge"},
  {0x06, 0x08, 0xFF, L"RACEway bridge"},
  {0x06, 0x09, 0x40, L"Semi-transparent P2P bridge with the primary PCI bus facing processor"},
  {0x06, 0x09, 0x80, L"Semi-transparent P2P bridge with the secondary PCI bus facing processor"},
  {0x06, 0x0A, 0x00, L"InfiniBand-to-PCI host bridge"},
  {0x06, 0x80, 0x00, L"Other bridge device"},
  
  {0x07, 0x00, 0x00, L"Generic XT-compatible serial controller"},
  {0x07, 0x00, 0x01, L"16450-compatible serial controller"},
  {0x07, 0x00, 0x02, L"16550-compatible serial controller"},
  {0x07, 0x00, 0x03, L"16650-compatible serial controller"},
  {0x07, 0x00, 0x04, L"16750-compatible serial controller"},
  {0x07, 0x00, 0x05, L"16850-compatible serial controller"},
  {0x07, 0x00, 0x06, L"16950-compatible serial controller"},
  {0x07, 0x01, 0x00, L"Parallel port"},
  {0x07, 0x01, 0x01, L"Bi-directional parallel port"},
  {0x07, 0x01, 0x02, L"ECP 1.X compliant parallel port"},
  {0x07, 0x01, 0x03, L"IEEE1284 controller"},
  {0x07, 0x01, 0xFE, L"IEEE1284 target device"},
  {0x07, 0x02, 0x00, L"Multiport serial controller"},
  {0x07, 0x03, 0x00, L"Generic modem"},
  {0x07, 0x03, 0x01, L"Hayes compatible modem, 16450-compatible interface"},
  {0x07, 0x03, 0x02, L"Hayes compatible modem, 16550-compatible interface"},
  {0x07, 0x03, 0x03, L"Hayes compatible modem, 16650-compatible interface"},
  {0x07, 0x03, 0x04, L"Hayes compatible modem, 16750-compatible interface"},
  {0x07, 0x04, 0x00, L"GPIB (IEEE 488.1/2) controller"},
  {0x07, 0x05, 0x00, L"Smart Card"},
  {0x07, 0x80, 0x00, L"Other communications device"},
  
  {0x08, 0x00, 0x00, L"Generic 8259 PIC"}, 
  {0x08, 0x00, 0x01, L"ISA PIC"},
  {0x08, 0x00, 0x02, L"EISA PIC"},
  {0x08, 0x00, 0x10, L"I/O APIC interrupt controller"},
  {0x08, 0x00, 0x20, L"I/O(x) APIC interrupt controller"},
  {0x08, 0x01, 0x00, L"Generic 8237 DMA controller"},
  {0x08, 0x01, 0x01, L"ISA DMA controller"},
  {0x08, 0x01, 0x02, L"EISA DMA controller"},
  {0x08, 0x02, 0x00, L"Generic 8254 system timer"},
  {0x08, 0x02, 0x01, L"ISA system timer"},
  {0x08, 0x02, 0x02, L"EISA system timers"},
  {0x08, 0x03, 0x00, L"Generic RTC controller"},
  {0x08, 0x03, 0x01, L"ISA RTC controller"},
  {0x08, 0x04, 0x00, L"Generic PCI Hot-Plug controller"},
  {0x08, 0x80, 0x00, L"Other system peripheral"},
  
  {0x09, 0x00, 0x00, L"Keyboard controller"},
  {0x09, 0x01, 0x00, L"Digitizer (pen)"},
  {0x09, 0x02, 0x00, L"Mouse controller"},
  {0x09, 0x03, 0x00, L"Scanner controller"},
  {0x09, 0x04, 0x00, L"Gameport controller (generic)"},
  {0x09, 0x04, 0x10, L"Gameport controller"},
  {0x09, 0x80, 0x00, L"Other input controller"},
  
  {0x0A, 0x00, 0x00, L"Generic docking station"},
  {0x0A, 0x80, 0x00, L"Other type of docking station"},
  
  {0x0B, 0x00, 0x00, L"386"},
  {0x0B, 0x01, 0x00, L"486"},
  {0x0B, 0x02, 0x00, L"Pentium"},
  {0x0B, 0x10, 0x00, L"Alpha"},
  {0x0B, 0x20, 0x00, L"PowerPC"},
  {0x0B, 0x30, 0x00, L"MIPS"},
  {0x0B, 0x40, 0x00, L"Co-processor"},
  
  {0x0C, 0x00, 0x00, L"IEEE 1394(FireWire)"},
  {0x0C, 0x00, 0x10, L"IEEE 1394(OHCI)"},
  {0x0C, 0x01, 0x00, L"ACCESS.bus"},
  {0x0C, 0x02, 0x00, L"SSA"},
  {0x0C, 0x03, 0x00, L"USB UHCI"},
  {0x0C, 0x03, 0x10, L"USB OHCI"},
  {0x0C, 0x03, 0x20, L"USB EHCI"},
  {0x0C, 0x03, 0x30, L"USB XHCI"},  
  {0x0C, 0x03, 0x80, L"USB"},
  {0x0C, 0x03, 0xFE, L"USB device"},
  {0x0C, 0x04, 0x00, L"Fibre Channel"},
  {0x0C, 0x05, 0x00, L"SMBus"},
  {0x0C, 0x06, 0x00, L"InfiniBand"},
  {0x0C, 0x07, 0x00, L"IPMI SMIC Interface"},
  {0x0C, 0x07, 0x01, L"IPMI Kybd Controller Style Interface"},
  {0x0C, 0x07, 0x02, L"IPMI Block Transfer Interface"},
  {0x0C, 0x08, 0x00, L"SERCOS Interface Standard"},
  {0x0C, 0x09, 0x00, L"CANbus"},
  
  {0x0D, 0x00, 0x00, L"iRDA compatible controller"},
  {0x0D, 0x01, 0x00, L"Consumer IR controller"},
  {0x0D, 0x10, 0x00, L"RF controller"},
  {0x0D, 0x11, 0x00, L"Bluetooth"},
  {0x0D, 0x12, 0x00, L"Broadband"},
  {0x0D, 0x20, 0x00, L"Ethernet (802.11a - 5 GHz)"},
  {0x0D, 0x21, 0x00, L"Ethernet (802.11b - 2.4 GHz)"},
  {0x0D, 0x80, 0x00, L"Other type of wireless controller"},
  
  {0x0F, 0x01, 0x00, L"TV"},
  {0x0F, 0x02, 0x00, L"Audio"},
  {0x0F, 0x03, 0x00, L"Voice"},
  {0x0F, 0x04, 0x00, L"Data"},
  
  {0x10, 0x00, 0x00, L"Network and computing en/decryption"},
  {0x10, 0x10, 0x00, L"Entertainment en/decryption"},
  {0x10, 0x80, 0x00, L"Other en/decryption"},
  
  {0x11, 0x00, 0x00, L"DPIO modules"},
  {0x11, 0x01, 0x00, L"Performance counters"},
  {0x11, 0x10, 0x00, L"Communications synchronization plus time and frequency test/m measurement"},
  {0x11, 0x20, 0x00, L"Management card"},
  {0x11, 0x80, 0x00, L"Other data acquisition/signal processing controllers"},
};

UINTN mPciClassCodeNameTableNum = sizeof (mPciClassCodeNameTable) / sizeof (PCI_CLASS_INFO);

// https://pci-ids.ucw.cz/read/PD/

STATIC CHAR16 *mBaseClassNameTable[] = {
  L"compatible",
  L"Mass storage controller",
  L"Network controller",
  L"Display controller",
  L"Multimedia device",
  L"Memory controller",
  L"Bridge device",
  L"Simple communication controllers",
  L"Base system peripherals",
  L"Input devices",
  L"Docking stations",
  L"Processors",
  L"Serial bus controllers",
  L"Wireless controller",
  L"Intelligent I/O controllers",
  L"Satellite communication controllers",
  L"Encryption/Decryption controllers",
  L"Data acquisition and signal processing controllers",
  L"Processing accelerators", 	
  L"Non-Essential Instrumentation",
};

UINTN mPciBaseClassCodeNameTableNum = sizeof (mBaseClassNameTable) / sizeof (CHAR16*);

LIST_ENTRY mPciDeviceList = INITIALIZE_LIST_HEAD_VARIABLE (mPciDeviceList);

typedef struct {
  LIST_ENTRY            Link;
  EFI_PCI_IO_PROTOCOL   *PciIo;
  UINTN                 SegmentNum;
  UINTN                 BusNum;
  UINTN                 DeviceNum;
  UINTN                 FunctionNum;
  BOOLEAN               IsBridge;
  UINTN                 StartBus;
  UINTN                 EndBus;
  BOOLEAN               Finished;
} LIST_PCI_DEVCIE_INFO;

BOOLEAN
IsPciBridge (
  IN LIST_PCI_DEVCIE_INFO     *PciDevice,
  OUT UINTN                   *StartBus,
  OUT UINTN                   *EndBus
  )
{
  UINT8       PciHeaderType;
  UINT32      Value;
	PciDevice->PciIo->Pci.Read(PciDevice->PciIo, EfiPciIoWidthUint8, PCI_HEADER_TYPE_OFFSET, 1, &PciHeaderType);
  if ((PciHeaderType & HEADER_TYPE_PCI_TO_PCI_BRIDGE) != 0) {
    PciDevice->PciIo->Pci.Read(PciDevice->PciIo, EfiPciIoWidthUint32, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET, 1, &Value);
    *StartBus = (Value >> 8) & 0xFF;
    *EndBus = (Value >> 16) & 0xFF;
    return TRUE;
  } else {
    return FALSE;
  }
}

VOID
InsertPciDevice (
  IN EFI_PCI_IO_PROTOCOL   *PciIo,
  IN UINTN                 SegmentNum,
  IN UINTN                 BusNum,
  IN UINTN                 DeviceNum,
  IN UINTN                 FunctionNum
  )
{
  LIST_ENTRY               *Entry;
  LIST_PCI_DEVCIE_INFO     *PciDevice;
  LIST_PCI_DEVCIE_INFO     *PciDeviceInList;

  PciDevice = AllocatePool (sizeof (LIST_PCI_DEVCIE_INFO));
  PciDevice->PciIo       = PciIo;
  PciDevice->SegmentNum  = SegmentNum;
  PciDevice->BusNum      = BusNum;
  PciDevice->DeviceNum   = DeviceNum;
  PciDevice->FunctionNum = FunctionNum;
  PciDevice->Finished    = FALSE;
  PciDevice->IsBridge    = IsPciBridge (PciDevice, &PciDevice->StartBus, &PciDevice->EndBus);

  if (IsListEmpty (&mPciDeviceList)) {
    InsertTailList (&mPciDeviceList, &PciDevice->Link);
  } else {
    Entry = GetFirstNode (&mPciDeviceList);
    while (!IsNull (&mPciDeviceList, Entry)) {
      PciDeviceInList = (LIST_PCI_DEVCIE_INFO *) Entry;
      if (PCI_SEGMENT_LIB_ADDRESS (PciDevice->SegmentNum, PciDevice->BusNum, PciDevice->DeviceNum, PciDevice->FunctionNum, 0) <
          PCI_SEGMENT_LIB_ADDRESS (PciDeviceInList->SegmentNum, PciDeviceInList->BusNum, PciDeviceInList->DeviceNum, PciDeviceInList->FunctionNum, 0)) {
        InsertTailList (Entry, &PciDevice->Link);
        break;
      }
      Entry = Entry->ForwardLink;
    }
    if (IsNull (&mPciDeviceList, Entry)) {
      InsertTailList (Entry, &PciDevice->Link);
    }
  }
}

VOID
ShowPciDeviceInfo (
  IN LIST_PCI_DEVCIE_INFO     *PciDevice
  )
{
  UINT16      VendorId;
  UINT16      DeviceId;
  UINT8       ClassCode[3];
  UINTN       Index;

  PciDevice->PciIo->Pci.Read(PciDevice->PciIo, EfiPciIoWidthUint16, PCI_VENDOR_ID_OFFSET, 1, &VendorId);
  PciDevice->PciIo->Pci.Read(PciDevice->PciIo, EfiPciIoWidthUint16, PCI_DEVICE_ID_OFFSET, 1, &DeviceId);
  PciDevice->PciIo->Pci.Read(PciDevice->PciIo, EfiPciIoWidthUint8,  PCI_CLASSCODE_OFFSET, 3, &ClassCode[0]);

  DEBUG ((DEBUG_INFO, "  [%04x:%04x] - ", VendorId, DeviceId));
  for (Index = 0; Index < mPciClassCodeNameTableNum; Index++) {
    if (ClassCode[2] == mPciClassCodeNameTable[Index].BaseClass &&
        ClassCode[1] == mPciClassCodeNameTable[Index].SubClass &&
        ClassCode[0] == mPciClassCodeNameTable[Index].Interface) {
      DEBUG ((DEBUG_INFO, "%s", mPciClassCodeNameTable[Index].Name));
      break;
    }
  }

  if (Index >= mPciClassCodeNameTableNum) {
    if (ClassCode[2] < mPciBaseClassCodeNameTableNum - 1) {
      DEBUG ((DEBUG_INFO, "%s", mBaseClassNameTable[ClassCode[2]]));
    } else {
      DEBUG ((DEBUG_INFO, "[%02x:%02x:%02x]", ClassCode[2], ClassCode[1], ClassCode[0]));
    }
  }
}

VOID
ShowPciBusInfoHeader (
  IN LIST_PCI_DEVCIE_INFO     *PciDevice
  )
{
  if (PciDevice->SegmentNum == 0 && PciDevice->BusNum == 0) {
    DEBUG ((DEBUG_INFO, "\n-"));  
  } else {
    DEBUG ((DEBUG_INFO, "\n "));  
  }
  DEBUG ((DEBUG_INFO, "+-[%04x:%02x]-+", PciDevice->SegmentNum, PciDevice->BusNum));
}

VOID
ShowPciInfo (
  IN LIST_PCI_DEVCIE_INFO     *PciDevice,
  IN UINTN                    BusDepth
  )
{
  UINTN                    Index;
  LIST_PCI_DEVCIE_INFO     *BackPciDevice;
  LIST_PCI_DEVCIE_INFO     *ForwardPciDevice;

  BackPciDevice    = (LIST_PCI_DEVCIE_INFO *)PciDevice->Link.BackLink;
  ForwardPciDevice = (LIST_PCI_DEVCIE_INFO *)PciDevice->Link.ForwardLink;

  if (PciDevice->BusNum != 0 || PciDevice->DeviceNum != 0 || PciDevice->FunctionNum != 0) {
    if (PciDevice->BusNum == BackPciDevice->BusNum) {
      DEBUG ((DEBUG_INFO, "\n |           "));
      for (Index = 0; Index < BusDepth - 1; Index++) {
        DEBUG ((DEBUG_INFO, "                "));
      }
    }

    if (PciDevice->BusNum == BackPciDevice->BusNum && PciDevice->BusNum != ForwardPciDevice->BusNum) {
      DEBUG ((DEBUG_INFO, "\\"));  
    } else {
      DEBUG ((DEBUG_INFO, "+"));  
    }
  }

  DEBUG ((DEBUG_INFO, "-%02x.%01x", PciDevice->DeviceNum, PciDevice->FunctionNum));

  if (PciDevice->IsBridge) {
    DEBUG ((DEBUG_INFO, "-[%02x-%02x]--", PciDevice->StartBus, PciDevice->EndBus));
  } else {
    ShowPciDeviceInfo (PciDevice);
  }
  PciDevice->Finished = TRUE;
}

VOID
ShowPciBus (
  IN UINTN                 SegmentNum,
  IN UINTN                 Bus,
  IN UINTN                 BusDepth
  )
{
  LIST_ENTRY               *Entry;
  LIST_PCI_DEVCIE_INFO     *PciDevice;
  UINTN                    BusIndex;

  Entry = GetFirstNode (&mPciDeviceList);
  while (!IsNull (&mPciDeviceList, Entry)) {
    PciDevice = (LIST_PCI_DEVCIE_INFO *) Entry;
    if (!PciDevice->Finished && PciDevice->SegmentNum == SegmentNum && PciDevice->BusNum == Bus) {
      ShowPciInfo (PciDevice, BusDepth);
      if (PciDevice->IsBridge) {
        for (BusIndex = PciDevice->StartBus; BusIndex < PciDevice->EndBus + 1; BusIndex++) {
          ShowPciBus (SegmentNum, BusIndex, BusDepth + 1);
        }
      }
    }
    Entry = Entry->ForwardLink;
  }
}

VOID
PciShowTree (
  VOID
  )
{
  LIST_ENTRY               *Entry;
  LIST_PCI_DEVCIE_INFO     *PciDeviceInList;
  UINTN                    SegmentNum;

  DEBUG((EFI_D_INFO, "PciTree:\n"));
  DEBUG((EFI_D_INFO, "-----------------------------------------------------------------------------"));
  Entry = GetFirstNode (&mPciDeviceList);
  while (!IsNull (&mPciDeviceList, Entry)) {
    PciDeviceInList = (LIST_PCI_DEVCIE_INFO *) Entry;
    if (!PciDeviceInList->Finished) {
      SegmentNum = PciDeviceInList->SegmentNum;
      ShowPciBusInfoHeader (PciDeviceInList);
      ShowPciBus (SegmentNum, PciDeviceInList->BusNum, 1);
    }
    Entry = Entry->ForwardLink;
  }
  DEBUG((EFI_D_INFO, "\n-----------------------------------------------------------------------------\n"));
}
