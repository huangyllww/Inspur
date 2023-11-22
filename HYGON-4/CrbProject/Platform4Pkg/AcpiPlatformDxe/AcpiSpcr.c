
#include "AcpiPlatform.h"
#include <IndustryStandard/SerialPortConsoleRedirectionTable.h>
#include <Library/DevicePathLib.h>

//
// terminal type
//
#define VT100TYPE                      0
#define VT100PLUSTYPE                  1
#define VTUTF8TYPE                     2
#define PCANSITYPE                     3


EFI_ACPI_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE  gAcpiSpcrTable = {
  {
    EFI_ACPI_3_0_SERIAL_PORT_CONSOLE_REDIRECTION_TABLE_SIGNATURE,
    sizeof(gAcpiSpcrTable),
    0x01,                                                 // Revision
    0,                                                    // checksum
    {'B', 'Y', 'O', 'S', 'O','F'},                        // OemId
    SIGNATURE_64('B','Y','O','S','O','F','T','\0'),       // OemTableId
    1,                                                    // OemRevision
    SIGNATURE_32('B','Y','O','\0'),                       // CreatorId
    0x01000001                                            // CreatorRevision
  },
  0,                                                      // InterfaceType
  {0, 0, 0},                                              // Reserved1[3]
  {0x01,                                                  // AddressSpaceId
   0x08,                                                  // RegisterBitWidth
   0,                                                     // RegisterBitOffset
   0,                                                     // Reserved
   0                                                      // Address
  },
  0,                                                      // InterruptType
  0,                                                      // Irq
  0,                                                      // GlobalSystemInterrupt
  0,                                                      // BaudRate
  0,                                                      // Parity
  0,                                                      // StopBits
  0,                                                      // FlowControl
  0,                                                      // TerminalType
  0,                                                      // Language
  0xFFFF,                                                 // PciDeviceId
  0xFFFF,                                                 // PciVendorId
  0,                                                      // PciBusNumber
  0,                                                      // PciDeviceNumber
  0,                                                      // PciFunctionNumber
  0,                                                      // PciFlags
  0,                                                      // PciSegment
  0                                                       // Reserved2
};


VOID
BuildAcpiSpcrTable (
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTable,
  EFI_DEVICE_PATH_PROTOCOL *UcrDp,
  UINT16                   IoBase
  )
{
  EFI_STATUS                Status;
  UINT16                    ComPort;
  UINT8                     Baudrate;
  UINT8                     TerminalType;
  UINT8                     IrqNumber;
  UINTN                     TableKey;
  EFI_DEVICE_PATH_PROTOCOL  *Next = UcrDp;
  UART_DEVICE_PATH          *UartDp;
  VENDOR_DEVICE_PATH        *VendorDp;
  UINT8                     StopBits;
  UINTN                     GetCount = 0;


  IrqNumber    = 4;
  ComPort      = IoBase;
  Baudrate     = 7;
  TerminalType = VTUTF8TYPE;
  StopBits     = 1;

  while(!IsDevicePathEnd(Next)) {
    if (Next->Type == MESSAGING_DEVICE_PATH && Next->SubType == MSG_UART_DP){
      UartDp = (UART_DEVICE_PATH*)Next;
      StopBits = UartDp->StopBits;
      switch (UartDp->BaudRate) {
        case 9600:
          Baudrate = 3;
          break;

        case 19200:
          Baudrate = 4;
          break;

        case 57600:
          Baudrate = 6;
          break;

        default:
        case 115200:
          Baudrate = 7;
          break;
      }
      GetCount++;
    }else if(Next->Type == MESSAGING_DEVICE_PATH && Next->SubType == MSG_VENDOR_DP){
      VendorDp = (VENDOR_DEVICE_PATH*)Next;
      if(CompareGuid(&VendorDp->Guid, &gEfiVT100PlusGuid)){
        TerminalType = VT100PLUSTYPE;
      } else if(CompareGuid(&VendorDp->Guid, &gEfiVT100Guid)){
        TerminalType = VT100TYPE;
      } else if(CompareGuid(&VendorDp->Guid, &gEfiPcAnsiGuid)){
        TerminalType = PCANSITYPE;
      } else if(CompareGuid(&VendorDp->Guid, &gEfiVTUTF8Guid)){
        TerminalType = VTUTF8TYPE;
      }
      GetCount++;
    }

    if(GetCount == 2){
      break;
    }

    Next = NextDevicePathNode(Next);
  }

  switch (ComPort) {
    default:
    case 0x3F8:
      IrqNumber = 4;
      break;

    case 0x2F8:
      IrqNumber = 3;
      break;

    case 0x3E8:
      IrqNumber = 4;
      break;

    case 0x2E8:
      IrqNumber = 3;
      break;
  }

  gAcpiSpcrTable.BaseAddress.Address  = (UINT64)ComPort;
  gAcpiSpcrTable.InterruptType        = 0x05;//bit0:dual-8259 bit1:I/O APIC bit2:I/O SAPIC
  gAcpiSpcrTable.Irq                  = IrqNumber;
  gAcpiSpcrTable.BaudRate              = Baudrate;
  gAcpiSpcrTable.StopBits              = StopBits;
  gAcpiSpcrTable.FlowControl          = 0;
  gAcpiSpcrTable.TerminalType          = TerminalType;

  PlatformUpdateTables((EFI_ACPI_COMMON_HEADER*)&gAcpiSpcrTable);
  AcpiTableUpdateChksum(&gAcpiSpcrTable);

  Status = AcpiTable->InstallAcpiTable(
                        AcpiTable,
                        &gAcpiSpcrTable,
                        sizeof(gAcpiSpcrTable),
                        &TableKey
                        );

}

