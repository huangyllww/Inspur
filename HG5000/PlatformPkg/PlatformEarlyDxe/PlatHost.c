
#include "PlatHost.h"
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>


extern ADDITIONAL_ROM_TABLE    gLegacyOpRomTable[];
extern UINTN                   gLegacyOpRomTableSize;


// DP:PciRoot(0x0)/Pci(0x1,0x4/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gPlatformAhci0Dp = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    4,
    1
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x0
  },
  gEndEntire
};

// DP:PciRoot(0x0)/Pci(0x1,0x5)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gPlatformAhci1Dp = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    5,
    1
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x0
  },
  gEndEntire
};

// DP:PciRoot(0x1)/Pci(0x8,0x1)/Pci(0x0,0x2)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gPlatformAhci2Dp = {
  gPciRootBridge1,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    8
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x2,
    0x0
  },
  gEndEntire
};

// DP:PciRoot(0x2)/Pci(0x8,0x1)/Pci(0x0,0x2)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gPlatformAhci3Dp = {
  gPciRootBridge2,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    8
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x2,
    0x0
  },
  gEndEntire
};

// DP:PciRoot(0x3)/Pci(0x8,0x1)/Pci(0x0,0x2)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gPlatformAhci4Dp = {
  gPciRootBridge3,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    8
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x2,
    0x0
  },
  gEndEntire
};

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gObLan1DpData = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x02,
    0x01
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x0
  },
  gEndEntire
};

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gObLan2DpData = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x02,
    0x01
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x1,
    0x0
  },
  gEndEntire
};


//DP:PciRoot(0x2)/Pci(0x3,0x1)/Pci(0x0,0x0)
//DP:PciRoot(0x2)/Pci(0x3,0x2)/Pci(0x0,0x0)
//DP:PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gPlatRiserA0_DpData = {
  gPciRootBridge2,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    3
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x0
  },
  gEndEntire
};

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gPlatRiserA1_DpData = {
  gPciRootBridge2,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    2,
    3
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x0
  },
  gEndEntire
};

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gPlatRiserA2_DpData = {
  gPciRootBridge3,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    2,
    3
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x0
  },
  gEndEntire
};

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH gObPcieSlotJ7DpData = {
  gPciRootBridge2,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x01,
    0x03
  },
  gEndEntire
};

//DP:PciRoot(0x6)/Pci(0x3,0x2)/Pci(0x0,0x0)
//DP:PciRoot(0x6)/Pci(0x3,0x1)/Pci(0x0,0x0)
//DP:PciRoot(0x5)/Pci(0x1,0x2)/Pci(0x0,0x0)
static PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gPlatRiserB0_DpData = {
  gPciRootBridge6,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    2,
    3
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x0
  },
  gEndEntire
};

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gPlatRiserB1_DpData = {
  gPciRootBridge6,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    3
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x0
  },
  gEndEntire
};

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gPlatRiserB2_DpData = {
  gPciRootBridge5,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    2,
    1
  },
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x0,
    0x0
  },
  gEndEntire
};



STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH gObPcieSlotJ8DpData = {
  gPciRootBridge6,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x02,
    0x03
  },
  gEndEntire
};

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH gObPcieSlotJ9DpData = {
  gPciRootBridge7,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x01,
    0x03
  },
  gEndEntire
};

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH gObPcieSlotJ10DpData = {
  gPciRootBridge1,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x01,
    0x01
  },
  gEndEntire
};


STATIC PLATFORM_HOST_INFO_SATA_CTX gPlatHostSata0Ctx = {sizeof(gPlatformAhci0Dp), 0, 8, {0}};
STATIC PLATFORM_HOST_INFO_SATA_CTX gPlatHostSata1Ctx = {sizeof(gPlatformAhci1Dp), 1, 8, {0}};
STATIC PLATFORM_HOST_INFO_SATA_CTX gPlatHostSata2Ctx = {sizeof(gPlatformAhci2Dp), 2, 8, {0}};
STATIC PLATFORM_HOST_INFO_SATA_CTX gPlatHostSata3Ctx = {sizeof(gPlatformAhci3Dp), 3, 8, {0}};

STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ71Ctx = {"J7_1",  SlotTypePciExpress, SlotDataBusWidth8X,    SlotUsageAvailable, SlotLengthLong, 0 };
STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ72Ctx = {"J7_2",  SlotTypePciExpress, SlotDataBusWidth8X,    SlotUsageAvailable, SlotLengthLong, 0 };
STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ73Ctx = {"J7_3",  SlotTypePciExpress, SlotDataBusWidth8X,    SlotUsageAvailable, SlotLengthLong, 0 };
STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ81Ctx = {"J8_1",  SlotTypePciExpress, SlotDataBusWidth8X,    SlotUsageAvailable, SlotLengthLong, 0 };
STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ82Ctx = {"J8_2",  SlotTypePciExpress, SlotDataBusWidth8X,    SlotUsageAvailable, SlotLengthLong, 0 };
STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ83Ctx = {"J8_3",  SlotTypePciExpress, SlotDataBusWidth8X,    SlotUsageAvailable, SlotLengthLong, 0 };
STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ9Ctx  = {"J9",    SlotTypePciExpress, SlotDataBusWidthOther, SlotUsageAvailable, SlotLengthLong, 0 };
STATIC PLATFORM_HOST_INFO_PCIE_CTX gPlatHostPcieJ10Ctx = {"J10",   SlotTypePciExpress, SlotDataBusWidthOther, SlotUsageAvailable, SlotLengthLong, 0 };

PLATFORM_HOST_INFO gPlatHostInfo[] = {
  {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gPlatformAhci0Dp, &gPlatHostSata0Ctx},
  {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gPlatformAhci1Dp, &gPlatHostSata1Ctx},
 //PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gPlatformAhci2Dp, &gPlatHostSata2Ctx},
//{PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gPlatformAhci3Dp, &gPlatHostSata3Ctx},
  
  {PLATFORM_HOST_LAN,  (EFI_DEVICE_PATH_PROTOCOL*)&gObLan1DpData, NULL},
  {PLATFORM_HOST_LAN,  (EFI_DEVICE_PATH_PROTOCOL*)&gObLan2DpData, NULL}, 

  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gPlatRiserA0_DpData, &gPlatHostPcieJ71Ctx}, 
  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gPlatRiserA1_DpData, &gPlatHostPcieJ72Ctx}, 
  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gPlatRiserA2_DpData, &gPlatHostPcieJ73Ctx},
  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gPlatRiserB0_DpData, &gPlatHostPcieJ81Ctx}, 
  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gPlatRiserB1_DpData, &gPlatHostPcieJ82Ctx}, 
  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gPlatRiserB2_DpData, &gPlatHostPcieJ83Ctx}, 
  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gObPcieSlotJ9DpData, &gPlatHostPcieJ9Ctx}, 
  {PLATFORM_HOST_PCIE,  (EFI_DEVICE_PATH_PROTOCOL*)&gObPcieSlotJ10DpData, &gPlatHostPcieJ10Ctx},   
};  

#define PLAT_HOST_INFO_COUNT   (sizeof(gPlatHostInfo)/sizeof(gPlatHostInfo[0]))



#define CLASS_HID           3
#define SUBCLASS_BOOT       1
#define PROTOCOL_KEYBOARD   1

typedef struct {
  USB_CLASS_DEVICE_PATH           UsbClass;
  EFI_DEVICE_PATH_PROTOCOL        End;
} USB_CLASS_FORMAT_DEVICE_PATH;


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

STATIC EFI_DEVICE_PATH_PROTOCOL *gConInDpList[] = {
  (EFI_DEVICE_PATH_PROTOCOL*)&gUsbClassKeyboardDevicePath
};



EFI_STATUS
EFIAPI
DummyStopWatchDog (
    VOID
  )
{
  return EFI_UNSUPPORTED;
}



PLAT_HOST_INFO_PROTOCOL gPlatHostInfoProtocol = {
  gPlatHostInfo,
  PLAT_HOST_INFO_COUNT,
  0,
  NULL,                             // IgdDp
  0,                                // IgdDpSize
  GetPlatSataHostIndex,
  GetPlatSataPortIndex,
  GetPlatNvmeIndex,                 // GetNvmeIndex
  NULL,                             // OptionRomTable
  0,                                // OptionRomTableSize
  NULL,                             // GetPlatUcrDp
  gConInDpList,
  sizeof(gConInDpList)/sizeof(gConInDpList[0]), 
  PlatUpdateBootOption,             // UpdateBootOption
  NULL,                             // UpdateTolumVar
  0,                                // CpuMcVer
  NULL,                             // HotKey
  0,                                // HotKeyCount
  NULL,                             // SetupAddOnHandle
  0,                                // SetupTitleId
  DummyStopWatchDog,                // StopWdg
  0xFFFFFFFF,                       // AddOnGfxId
};



VOID
EFIAPI
PlatHostEnterSetupCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID        *Interface;
  EFI_STATUS  Status;


  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

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
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    PlatHostEnterSetupCallBack,
    NULL,
    &Registration
    ); 

  gPlatHostInfoProtocol.OptionRomTable = gLegacyOpRomTable;
  gPlatHostInfoProtocol.OptionRomTableSize = gLegacyOpRomTableSize;
  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gPlatHostInfoProtocolGuid, &gPlatHostInfoProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  return Status;
}


