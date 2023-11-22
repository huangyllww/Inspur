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
#include <Library/BaseMemoryLib.h>


STATIC
struct{
  UINT8   BaseClass;
  UINT8   SubClass;
  UINT8   Interface;
  CHAR16  *Name;
}gPciClassCodeNameTable[] = {
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


// https://pci-ids.ucw.cz/read/PD/

STATIC CHAR16 *gBaseClassNameTable[] = {
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


STATIC CHAR16 gDummyStr[] = {L""};

CHAR16 *GetClassCodeName(UINT8 ClassCode[3])
{
  UINTN   i;
  UINTN   Count;
  CHAR16  *Name = NULL;
  UINT8   MyClassCode[3];
 
  Count = ARRAY_SIZE(gPciClassCodeNameTable);
  for(i=0;i<Count;i++){
    MyClassCode[2] = gPciClassCodeNameTable[i].BaseClass;
    MyClassCode[1] = gPciClassCodeNameTable[i].SubClass;
    MyClassCode[0] = gPciClassCodeNameTable[i].Interface;
    
    if(MyClassCode[0] == 0xFF){
      MyClassCode[0] = ClassCode[0];
    }
    if(CompareMem(MyClassCode, ClassCode, sizeof(MyClassCode)) == 0){
      Name = gPciClassCodeNameTable[i].Name;
      break;
    }
  }
  
  if((Name == NULL) && (ClassCode[2] < ARRAY_SIZE(gBaseClassNameTable))){
    Name = gBaseClassNameTable[ClassCode[2]];
  }

  if(Name == NULL){
    Name = gDummyStr;
  }
  
  return Name;
}




