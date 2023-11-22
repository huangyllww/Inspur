
#include "PlatHost.h"
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>


extern ADDITIONAL_ROM_TABLE    gLegacyOpRomTable[];
extern UINTN                   gLegacyOpRomTableSize;


// DP:PciRoot(0x0)/Pci(0x8,0x1)/Pci(0x0,0x2)
STATIC BOARDCARD_PATH2  gPlatformAhciDp     = {DP_ROOT(0x00), DP_PCI(0x08, 0x01), DP_PCI(0x00, 0x02), DP_END};

// DP:PciRoot(0x0)/Pci(0x1,0x3)/Pci(0x0,0x0)
STATIC BOARDCARD_PATH2 gObLan1DpData        = {DP_ROOT(0x00), DP_PCI(0x01, 0x03), DP_PCI(0x00, 0x00), DP_END};

// DP:PciRoot(0x0)/Pci(0x1,0x4)/Pci(0x0,0x0)
STATIC BOARDCARD_PATH2 gObLan2DpData        = {DP_ROOT(0x00), DP_PCI(0x01, 0x04), DP_PCI(0x00, 0x00), DP_END};

// DP:PciRoot(0x0)/Pci(0x1,0x1)
STATIC BOARDCARD_PATH1 gObPcieSlotJ30DpData = {DP_ROOT(0x00), DP_PCI(0x01, 0x01), DP_END};

// DP:PciRoot(0x0)/Pci(0x1,0x6)
STATIC BOARDCARD_PATH1 gObPcieSlotJ31DpData = {DP_ROOT(0x00), DP_PCI(0x01, 0x06), DP_END};  

// DP:PciRoot(0x0)/Pci(0x3,0x1)
STATIC BOARDCARD_PATH1 gObPcieSlotJ84DpData = {DP_ROOT(0x00), DP_PCI(0x03, 0x01), DP_END};


STATIC PLATFORM_HOST_INFO_SATA_CTX gPlatHostSataCtx = {sizeof(gPlatformAhciDp), 0, 8};

STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ30Ctx = {"J30",  SlotTypePciExpress, SlotDataBusWidth16X, SlotUsageAvailable, SlotLengthLong,  1 };
STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ31Ctx = {"J31",  SlotTypePciExpress, SlotDataBusWidth4X,  SlotUsageAvailable, SlotLengthShort, 2 };
STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ84Ctx = {"J84",  SlotTypePciExpress, SlotDataBusWidth16X, SlotUsageAvailable, SlotLengthLong,  3 };

PLATFORM_HOST_INFO gPlatHostInfo[] = {
  {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gPlatformAhciDp, &gPlatHostSataCtx},

  {PLATFORM_HOST_LAN,  (EFI_DEVICE_PATH_PROTOCOL*)&gObLan1DpData, NULL},
  {PLATFORM_HOST_LAN,  (EFI_DEVICE_PATH_PROTOCOL*)&gObLan2DpData, NULL},

  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gObPcieSlotJ30DpData, &gPlatHostPcieJ30Ctx},
  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gObPcieSlotJ31DpData, &gPlatHostPcieJ31Ctx},
  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gObPcieSlotJ84DpData, &gPlatHostPcieJ84Ctx},
};



#define CLASS_HID           3
#define SUBCLASS_BOOT       1
#define PROTOCOL_KEYBOARD   1

typedef struct {
  USB_CLASS_DEVICE_PATH           UsbClass;
  EFI_DEVICE_PATH_PROTOCOL        End;
} USB_CLASS_FORMAT_DEVICE_PATH;

typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           IsaBridge;
  ACPI_HID_DEVICE_PATH      Keyboard;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_KEYBOARD_DEVICE_PATH;

STATIC USB_CLASS_FORMAT_DEVICE_PATH gUsbClassKeyboardDevicePath = {
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_USB_CLASS_DP,
      (UINT8) (sizeof (USB_CLASS_DEVICE_PATH)),
      (UINT8) ((sizeof (USB_CLASS_DEVICE_PATH)) >> 8)
    },
    0xffff,           // VendorId
    0xffff,           // ProductId
    CLASS_HID,        // DeviceClass
    SUBCLASS_BOOT,    // DeviceSubClass
    PROTOCOL_KEYBOARD // DeviceProtocol
  },

  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    END_DEVICE_PATH_LENGTH,
    0
  }
};

PLATFORM_KEYBOARD_DEVICE_PATH  gPs2KbDevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    3,
    11
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0303),
    0
  },
  gEndEntire
};

STATIC EFI_DEVICE_PATH_PROTOCOL *gConInDpList[] = {
  (EFI_DEVICE_PATH_PROTOCOL*)&gUsbClassKeyboardDevicePath,
#if FixedPcdGetBool(PcdHygonLpcDevicePs2KeyboardPresent)
  (EFI_DEVICE_PATH_PROTOCOL*)&gPs2KbDevicePath,
#endif
};



EFI_STATUS
EFIAPI
DummyStopWatchDog (
    VOID
  )
{
  return EFI_UNSUPPORTED;
}
  
EFI_STATUS
EFIAPI
DummyResetWatchDog (
    VOID
  )
{
  return EFI_UNSUPPORTED;
}



typedef struct {
  ACPI_HID_DEVICE_PATH               PciRootBridge;
  PCI_DEVICE_PATH                    IsaBridge;
  ACPI_HID_DEVICE_PATH               IsaSerial;
  UART_DEVICE_PATH                   Uart;
  UART_FLOW_CONTROL_DEVICE_PATH      FlowControl;
  VENDOR_DEVICE_PATH                 TerminalType;
  EFI_DEVICE_PATH_PROTOCOL           End;
} PLATFORM_ISA_SERIAL_DEVICE_PATH;

STATIC PLATFORM_ISA_SERIAL_DEVICE_PATH gSerialDevicePath = {
  DP_ROOT(0),
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    3,
    11
  },
  {
    ACPI_DEVICE_PATH,
    ACPI_DP,
    (UINT8) (sizeof (ACPI_HID_DEVICE_PATH)),
    (UINT8) ((sizeof (ACPI_HID_DEVICE_PATH)) >> 8),
    EISA_PNP_ID(0x0501),
    0
  },
  {
    MESSAGING_DEVICE_PATH,
    MSG_UART_DP,
    (UINT8) (sizeof (UART_DEVICE_PATH)),
    (UINT8) ((sizeof (UART_DEVICE_PATH)) >> 8),
    0,
    115200,
    8,
    1,
    1
  },
  {
    {
      MESSAGING_DEVICE_PATH,
      MSG_VENDOR_DP,
      {
        (UINT8)(sizeof(UART_FLOW_CONTROL_DEVICE_PATH)),
        (UINT8)((sizeof(UART_FLOW_CONTROL_DEVICE_PATH)) >> 8)
      }
    },
    DEVICE_PATH_MESSAGING_UART_FLOW_CONTROL,
    UART_FLOW_CONTROL_XON_XOFF
  },  
  {
    MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
    (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8),
    DEVICE_PATH_MESSAGING_VT_UTF8
  },
  DP_END
};


EFI_DEVICE_PATH_PROTOCOL*
GetPlatUcrDp (
    UINTN   *pDpSize,
    UINT16  *pIoBase
  )
{
  PLATFORM_ISA_SERIAL_DEVICE_PATH           *UartDp;
  UINT16                                    IoBase = 0;
  UINT8                                     SerialPortSelect = 1;
  STATIC PLATFORM_ISA_SERIAL_DEVICE_PATH    UcrDpDual[2];
  UINT16                                    DpSize = 0;


  if(PcdGetBool(PcdHygonLpcDeviceUart0Present)){
    SerialPortSelect = 0;
  } else if(PcdGetBool(PcdHygonLpcDeviceUart1Present)){
    SerialPortSelect = 1;
  }

  DEBUG((EFI_D_INFO, "GetPlatUcrDp En:%d br:%d T:%d ucr2:%d\n", \
    gSetupHob->UCREnable, gSetupHob->SerialBaudrate, gSetupHob->TerminalType, \
    gSetupHob->SerialRedirection2));

  if(!gSetupHob->UCREnable && !gSetupHob->SerialRedirection2){
    if(pDpSize != NULL){
      *pDpSize = 0;
    }
    if(pIoBase != NULL){
      *pIoBase = 0;
    }
    return NULL;
  }

  if(gSetupHob->UCREnable){
    CopyMem(&UcrDpDual[0], &gSerialDevicePath, sizeof(PLATFORM_ISA_SERIAL_DEVICE_PATH));
    UartDp = &UcrDpDual[0];
    switch(SerialPortSelect){
      default:
      case 0:
        IoBase = 0x3F8;
        UartDp->IsaSerial.UID = 0;
        break;
      case 1:
        IoBase = 0x2F8;
        UartDp->IsaSerial.UID = 1;
        break;
      case 2:
        IoBase = 0x3E8;
        UartDp->IsaSerial.UID = 2;
        break;
      case 3:
        IoBase = 0x2E8;
        UartDp->IsaSerial.UID = 3;
        break;
    }

    switch (gSetupHob->SerialBaudrate) {
      case 0:
        UartDp->Uart.BaudRate = 9600;
        break;

      case 1:
        UartDp->Uart.BaudRate = 19200;
        break;

      case 2:
        UartDp->Uart.BaudRate = 38400;
        break;

      case 3:
        UartDp->Uart.BaudRate = 57600;
        break;

      case 4:
      default:
        UartDp->Uart.BaudRate = 115200;
        break;
    }

    switch(gSetupHob->TerminalType) {
      case 0:
        CopyMem(&UartDp->TerminalType.Guid, &gEfiPcAnsiGuid, sizeof(EFI_GUID));
        break;
      case 1:
        CopyMem(&UartDp->TerminalType.Guid, &gEfiVT100Guid, sizeof(EFI_GUID));
        break;
      case 2:
        CopyMem(&UartDp->TerminalType.Guid, &gEfiVT100PlusGuid, sizeof(EFI_GUID));
        break;
      default:
      case 3:
        CopyMem(&UartDp->TerminalType.Guid, &gEfiVTUTF8Guid, sizeof(EFI_GUID));
        break;
      case 4:
        CopyMem(&UartDp->TerminalType.Guid, &gEfiTtyTermGuid, sizeof(EFI_GUID));
        break;
      case 5:
        CopyMem(&UartDp->TerminalType.Guid, &gEdkiiLinuxTermGuid, sizeof(EFI_GUID));
        break;
      case 6:
        CopyMem(&UartDp->TerminalType.Guid, &gEdkiiXtermR6Guid, sizeof(EFI_GUID));
        break;
      case 7:
        CopyMem(&UartDp->TerminalType.Guid, &gEdkiiVT400Guid, sizeof(EFI_GUID));
        break;
      case 8:
        CopyMem(&UartDp->TerminalType.Guid, &gEdkiiSCOTermGuid, sizeof(EFI_GUID));
        break;     
      case 9:
        CopyMem(&UartDp->TerminalType.Guid, &gEdkiiTerminalAutoGuid, sizeof(EFI_GUID));
        break;
    }

    switch (gSetupHob->Parity) {
      default:
      case 0:
        UartDp->Uart.Parity = 0x01;
        break;
      case 1:
        UartDp->Uart.Parity = 0x02;
        break;
      case 2:
        UartDp->Uart.Parity = 0x03;
        break;
      case 3:
        UartDp->Uart.Parity = 0x04;
        break;
      case 4:
        UartDp->Uart.Parity = 0x05;
        break;
    }

    switch (gSetupHob->StopBits) {
      default:
      case 0:
        UartDp->Uart.StopBits = 0x01;
        break;
      case 1:
        UartDp->Uart.StopBits = 0x02;
        break;
      case 2:
        UartDp->Uart.StopBits = 0x03;
        break;
    }

    switch (gSetupHob->DataBits) {
      case 0:
        UartDp->Uart.DataBits = 0x07;
        break;
      default:
      case 1:
        UartDp->Uart.DataBits = 0x08;
        break;
    }

    switch (gSetupHob->FlowControl) {
      case 0:
        UartDp->FlowControl.FlowControlMap = 0x01;
        break;
      default:
      case 1:
        UartDp->FlowControl.FlowControlMap = 0x00;
        break;
    }

    UartDp = &UcrDpDual[1];
    DpSize += sizeof(gSerialDevicePath);
    
  }else {
    UartDp = &UcrDpDual[0];
  }

  if(gSetupHob->SerialRedirection2){
    DpSize += sizeof(gSerialDevicePath);
    CopyMem(UartDp, &gSerialDevicePath, sizeof(PLATFORM_ISA_SERIAL_DEVICE_PATH));
    if(SerialPortSelect == 0){
      UartDp->IsaSerial.UID = 1; 
      if(IoBase == 0){
        IoBase = 0x2F8;
      }      
    } else {
      UartDp->IsaSerial.UID = 0; 
      if(IoBase == 0){
        IoBase = 0x3F8;
      }      
    }
    if(UartDp == &UcrDpDual[1]){
      UcrDpDual[0].End.SubType = END_INSTANCE_DEVICE_PATH_SUBTYPE;
    }   
  }

  if(pIoBase != NULL){
    *pIoBase = IoBase;
    DEBUG((EFI_D_INFO, "IoBase:%X\n", *pIoBase));
  }
  if(pDpSize != NULL){
    *pDpSize = DpSize;
  }

  return (EFI_DEVICE_PATH_PROTOCOL*)&UcrDpDual[0];
}




PLAT_HOST_INFO_PROTOCOL gPlatHostInfoProtocol = {
  gPlatHostInfo,
  ARRAY_SIZE(gPlatHostInfo),
  0,
  NULL,                             // IgdDp
  0,                                // IgdDpSize
  GetPlatSataHostIndex,
  GetPlatSataPortIndex,
  GetPlatNvmeIndex,                 // GetNvmeIndex
  gLegacyOpRomTable,                // OptionRomTable
  0,                                // OptionRomTableSize
  GetPlatUcrDp,                     // GetPlatUcrDp
  gConInDpList,
  ARRAY_SIZE(gConInDpList),
  NULL,                             // UpdateBootOption
  NULL,                             // UpdateTolumVar
  0,                                // CpuMcVer
  NULL,                             // HotKey
  0,                                // HotKeyCount
  NULL,                             // SetupAddOnHandle
  0,                                // SetupTitleId
  DummyStopWatchDog,                // StopWdg
  0xFFFFFFFF,                       // AddOnGfxId
  DummyResetWatchDog,               // ResetWdg
};



VOID
EFIAPI
CcxDxeInitCompleteCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID        *Interface;
  EFI_STATUS  Status;


  Status = gBS->LocateProtocol(&gHygonCcxDxeInitCompleteProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

// MSR0000_008B [Patch Level] (Core::X86::Msr::PATCH_LEVEL)
//   [31:0] PatchLevel. Read,Error-on-write,Volatile. Reset: 0. This returns an identification
//          number for the microcode patch that has been loaded. If no patch has been loaded,
//          this returns 0.
  gPlatHostInfoProtocol.CpuMcVer = (UINT32)AsmReadMsr64(0x8B);
  DEBUG((EFI_D_INFO, "CpuMcVer:%X\n", gPlatHostInfoProtocol.CpuMcVer));
}






EFI_STATUS
PlatHostInfoInstall (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS     Status;
  VOID           *Registration;


  EfiCreateProtocolNotifyEvent (
    &gHygonCcxDxeInitCompleteProtocolGuid,
    TPL_CALLBACK,
    CcxDxeInitCompleteCallBack,
    NULL,
    &Registration
    );

  gPlatHostInfoProtocol.OptionRomTableSize = gLegacyOpRomTableSize;
  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gPlatHostInfoProtocolGuid, &gPlatHostInfoProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  return Status;
}


