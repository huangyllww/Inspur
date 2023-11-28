/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  CrbDxe.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include "ProjectDxe.h"
#include <../Fch/Taishan/FchTaishanCore/Fch.h>
#include <IndustryStandard/Smbios.h>
#include <Library/SetupUiLib.h>
#include <SysMiscCfg.h>
#include <GnbRegistersZP.h>
#include <PlatformDefinition.h>
#include <Protocol/Smbios.h>
#include <Token.h>
#include <SystemPasswordVariable.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Protocol/ByoBmBootHookProtocol.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <AmdPcieComplex.h>
#include <AmdCpmDxe.h>
#include <AmdCpmDefine.h>

EFI_HII_HANDLE      gHiiHandle;
CONST SETUP_DATA    *gSetupHob;


#define GPIO_PIN(socket, die, gpio)     ((socket << 14) + (die << 10) + gpio)

VOID BiosPostFinishToBmc()
{
  EFI_STATUS    Status;
  AMD_CPM_TABLE_PROTOCOL    *CpmTableProtocolPtr;

  //UART0_RTS_L/UART2_TXD/EGPIO137	O	EGPIO137	////PU	GPIO£¬BIOS_POST_COMPLT_N£¬BIOS×Ô¼ìÍê³ÉºóÀ­µÍ
  //MmioAndThenOr32((0xfed81500UL + 137 * 4), (UINT32)~BIT22, 0);      // output low
  Status = gBS->LocateProtocol (
                  &gAmdCpmTableProtocolGuid,
                  NULL,
                  (VOID**)&CpmTableProtocolPtr
                  );
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), j_d, locate gAmdCpmTableProtocolGuid :%r.\n", Status));
  if (!EFI_ERROR (Status)) {
    CpmTableProtocolPtr->CommonFunction.SetGpio (CpmTableProtocolPtr, GPIO_PIN(0,0,137), 0);	
  }

  return;
}



VOID
EFIAPI
ProjectOnReadyToBoot (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__)); 
  gBS->CloseEvent(Event);

  BiosPostFinishToBmc();
}



//(01,00,00) 01058088 (02,00,00) 00 00000000 ROM(796C0018,24C00) 
//           DP:PciRoot(0x0)/Pci(0x1,0x1)/Pci(0x0,0x0)
//(01,00,01) 01058088 (02,00,00) 00 00000000 ROM(7969B018,24C00) 
//           DP:PciRoot(0x0)/Pci(0x1,0x1)/Pci(0x0,0x1)

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gProjectLan0DpData = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x03,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gProjectLan1DpData = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x03,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gProjectLan2DpData = {
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2 gProjectLan3DpData = {
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


//(07,00,02) 79011D94 (01,06,01) 00 00004000 ROM(00000000,00000) DP:PciRoot(0x0)/Pci(0x8,0x1)/Pci(0x0,0x2)
//(12,00,02) 79011D94 (01,06,01) 00 00004000 ROM(00000000,00000) DP:PciRoot(0x1)/Pci(0x8,0x1)/Pci(0x0,0x2)
//(22,00,02) 79011D94 (01,06,01) 00 00004000 ROM(00000000,00000) DP:PciRoot(0x2)/Pci(0x8,0x1)/Pci(0x0,0x2)
//(32,00,02) 79011D94 (01,06,01) 00 00004000 ROM(00000000,00000) DP:PciRoot(0x3)/Pci(0x8,0x1)/Pci(0x0,0x2)
//(42,00,02) 79011D94 (01,06,01) 00 00004000 ROM(00000000,00000) DP:PciRoot(0x4)/Pci(0x8,0x1)/Pci(0x0,0x2)
//(52,00,02) 79011D94 (01,06,01) 00 00004000 ROM(00000000,00000) DP:PciRoot(0x5)/Pci(0x8,0x1)/Pci(0x0,0x2)
//(62,00,02) 79011D94 (01,06,01) 00 00004000 ROM(00000000,00000) DP:PciRoot(0x6)/Pci(0x8,0x1)/Pci(0x0,0x2)
//(72,00,02) 79011D94 (01,06,01) 00 00004000 ROM(00000000,00000) DP:PciRoot(0x7)/Pci(0x8,0x1)/Pci(0x0,0x2)

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectAhci0Dp = {
  gPciRootBridge0,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectAhci1Dp = {
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

// DP:PciRoot(0x4)/Pci(0x8,0x1)/Pci(0x0,0x2)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectAhci4Dp = {
  gPciRootBridge4,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectAhci5Dp = {
  gPciRootBridge5,
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


// DP:PciRoot(0x5)/Pci(0x1,0x3)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectNvmeJ50Dp = {
  gPciRootBridge5,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    3,
    1,
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



// DP:PciRoot(0x0)/Pci(0x1,0x1)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectNvmeJ49Dp = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    1,
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


// DP:PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectPcieJ48Dp = {
  gPciRootBridge3,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    2,
    3,
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


// (31,00,00) 10FB8086 (02,00,00) 00 00000000 ROM(79331018,12000) DP:PciRoot(0x3)/Pci(0x3,0x1)/Pci(0x0,0x0)
// (31,00,01) 10FB8086 (02,00,00) 00 00000000 ROM(7931D018,12000) DP:PciRoot(0x3)/Pci(0x3,0x1)/Pci(0x0,0x1)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectPcieJ47Dp = {
  gPciRootBridge3,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    3,
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


//(71,00,00) 51B21344 (01,08,02) 00 00000000 ROM(79332018,11600) DP:PciRoot(0x7)/Pci(0x3,0x1)/Pci(0x0,0x0)
//(72,00,00) 51B21344 (01,08,02) 00 00000000 ROM(7931F018,11600) DP:PciRoot(0x7)/Pci(0x3,0x2)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectPcieJ51_1Dp = {
  gPciRootBridge7,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    3,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectPcieJ51_2Dp = {
  gPciRootBridge7,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    2,
    3,
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


//(71,00,00) 51B21344 (01,08,02) 00 00000000 ROM(79332018,11600) DP:PciRoot(0x7)/Pci(0x3,0x3)/Pci(0x0,0x0)
//(72,00,00) 51B21344 (01,08,02) 00 00000000 ROM(7931F018,11600) DP:PciRoot(0x7)/Pci(0x3,0x4)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectPcieJ52_1Dp = {
  gPciRootBridge7,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    3,
    3,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectPcieJ52_2Dp = {
  gPciRootBridge7,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    4,
    3,
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


//(05,00,00) 06221B21 (01,06,01) 00 00000000 ROM(79690018,0C200) 
//   DP:PciRoot(0x0)/Pci(0x1,0x6)/Pci(0x0,0x0)
//(06,00,00) 06221B21 (01,06,01) 00 00000000 ROM(79683018,0C200) 
//   DP:PciRoot(0x0)/Pci(0x1,0x7)/Pci(0x0,0x0)

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gAsm1061R0Dp = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    4,
    1,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gAsm1061R1Dp = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    5,
    1,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gAsm1061R2Dp = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    6,
    1,
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

//
// [ahci]
// die0 [7:4]  J56 - J59     1 x 4
// die1 [7:0]  J53 - J54     4 x 2
// die5 [3:0]  J55           4 x 1
//
STATIC PLATFORM_HOST_INFO_SATA_CTX gProjectAhci0Ctx = {sizeof(PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2), 0, 4};
STATIC PLATFORM_HOST_INFO_SATA_CTX gProjectAhci1Ctx = {sizeof(PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2), 1, 8};
//STATIC PLATFORM_HOST_INFO_SATA_CTX gProjectAhci4Ctx = {sizeof(PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2), 1, 8};
//STATIC PLATFORM_HOST_INFO_SATA_CTX gProjectAhci5Ctx = {sizeof(PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2), 2, 8};

STATIC PLATFORM_HOST_INFO_SATA_CTX gAsm1061R0Ctx = {sizeof(PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2), 3, 1};
STATIC PLATFORM_HOST_INFO_SATA_CTX gAsm1061R1Ctx = {sizeof(PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2), 2, 1};
STATIC PLATFORM_HOST_INFO_SATA_CTX gAsm1061R2Ctx = {sizeof(PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2), 3, 1};

//STATIC PLATFORM_HOST_INFO_PCIE_CTX gAsm1061R0Ctx = {"1", SlotTypeOther, SlotDataBusWidth1X, SlotUsageInUse, SlotLengthOther, 2};
//STATIC PLATFORM_HOST_INFO_PCIE_CTX gAsm1061R1Ctx = {"2", SlotTypeOther, SlotDataBusWidth1X, SlotUsageInUse, SlotLengthOther, 3};


STATIC PLATFORM_HOST_INFO_NVME_CTX gProjectNvmeJ49Ctx = {{"J49", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthShort, 0}, 0, sizeof(gProjectNvmeJ49Dp)};
STATIC PLATFORM_HOST_INFO_NVME_CTX gProjectNvmeJ50Ctx = {{"J50", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthShort, 1}, 1, sizeof(gProjectNvmeJ50Dp)};

STATIC PLATFORM_HOST_INFO_PCIE_CTX gProjectPcieJ48Ctx = {"J48", SlotTypePciExpress, SlotDataBusWidth8X, SlotUsageAvailable, SlotLengthOther, 2};
STATIC PLATFORM_HOST_INFO_PCIE_CTX gProjectPcieJ47Ctx = {"J47", SlotTypePciExpress, SlotDataBusWidth8X, SlotUsageAvailable, SlotLengthOther, 3};

STATIC PLATFORM_HOST_INFO_PCIE_CTX gProjectPcieJ51_1Ctx = {"J51_1", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthOther, 4};
STATIC PLATFORM_HOST_INFO_PCIE_CTX gProjectPcieJ51_2Ctx = {"J51_2", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthOther, 5};
STATIC PLATFORM_HOST_INFO_PCIE_CTX gProjectPcieJ52_1Ctx = {"J52_1", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthOther, 6};
STATIC PLATFORM_HOST_INFO_PCIE_CTX gProjectPcieJ52_2Ctx = {"J52_2", SlotTypePciExpress, SlotDataBusWidth4X, SlotUsageAvailable, SlotLengthOther, 7};



// (05,00,00) 20001A03 (03,00,00) 00 00004000 ROM(799E9018,10800) 
// DP:PciRoot(0x0)/Pci(0x1,0x4)/Pci(0x0,0x0)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH3 gProjectBmcVgaDpData = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x04,
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

// (05,00,00) 20001A03 (03,00,00) 00 00004000 ROM(799E9018,10800) 
// DP:PciRoot(0x0)/Pci(0x1,0x4)/Pci(0x0,0x0)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH3 gProjectBmcVgaDpData1 = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    0x03,
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


typedef struct {
  ACPI_HID_DEVICE_PATH      PciRootBridge;
  PCI_DEVICE_PATH           IsaBridge;
  ACPI_HID_DEVICE_PATH      IsaSerial;
  UART_DEVICE_PATH          Uart;
  VENDOR_DEVICE_PATH        TerminalType;
  EFI_DEVICE_PATH_PROTOCOL  End;
} PLATFORM_ISA_SERIAL_DEVICE_PATH;

PLATFORM_ISA_SERIAL_DEVICE_PATH   gSerialDevicePath = {
  gPciRootBridge,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    3,
    0x14
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
    MESSAGING_DEVICE_PATH,
    MSG_VENDOR_DP,
    (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
    (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8),
    DEVICE_PATH_MESSAGING_PC_ANSI
  },
  gEndEntire
};



/*

riser 0
(41,00,00) 00141000 (01,04,00) 00 00000000 ROM(7911E018,1A400) DP:PciRoot(0x2)/Pci(0x3,0x1)/Pci(0x0,0x0)
(42,00,00) 005D1000 (01,04,00) 00 00000000 ROM(79103018,1AE00) DP:PciRoot(0x2)/Pci(0x3,0x2)/Pci(0x0,0x0)

riser 1
(51,00,00) 15728086 (02,00,00) 00 00000000 ROM(78850018,34400) DP:PciRoot(0x5)/Pci(0x1,0x2)/Pci(0x0,0x0)
(61,00,00) 15638086 (02,00,00) 00 00000000 ROM(787E6018,34200) DP:PciRoot(0x6)/Pci(0x3,0x1)/Pci(0x0,0x0)
(63,00,00) 10FB8086 (02,00,00) 00 00000000 ROM(787A0018,10000) DP:PciRoot(0x6)/Pci(0x3,0x2)/Pci(0x0,0x0)

*/

// DP:PciRoot(0x1)/Pci(0x3,0x1)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectSlot0Dp = {
  gPciRootBridge1,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    3,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectSlot1Dp = {
  gPciRootBridge2,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    1,
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

STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectSlot2Dp = {
  gPciRootBridge3,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    3,
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



//DP:PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectSlot3Dp = {
  gPciRootBridge3,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    2,
    3,
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
//DP:PciRoot(0x1)/Pci(0x3,0x2)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectSlot4Dp = {
  gPciRootBridge1,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    2,
    3,
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
//DP:PciRoot(0x0)/Pci(0x1,0x1)/Pci(0x0,0x0)
STATIC PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH2  gProjectSlot5Dp = {
  gPciRootBridge0,
  {
    HARDWARE_DEVICE_PATH,
    HW_PCI_DP,
    (UINT8) (sizeof (PCI_DEVICE_PATH)),
    (UINT8) ((sizeof (PCI_DEVICE_PATH)) >> 8),
    1,
    1,
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

STATIC PLATFORM_HOST_INFO_PCIE_CTX ProjectSlot0DpCtx = {"PCIE0", SlotTypePciExpress, SlotDataBusWidth8X, SlotUsageAvailable, SlotLengthLong,  0};
STATIC PLATFORM_HOST_INFO_PCIE_CTX ProjectSlot1DpCtx = {"PCIE3", SlotTypePciExpress, SlotDataBusWidth16X, SlotUsageAvailable, SlotLengthLong, 1};
STATIC PLATFORM_HOST_INFO_PCIE_CTX ProjectSlot2DpCtx = {"PCIE2", SlotTypePciExpress, SlotDataBusWidth8X, SlotUsageAvailable, SlotLengthLong, 2};

STATIC PLATFORM_HOST_INFO_PCIE_CTX ProjectSlot3DpCtx = {"PCIE1", SlotTypePciExpress, SlotDataBusWidth8X, SlotUsageAvailable, SlotLengthLong,  3};
STATIC PLATFORM_HOST_INFO_PCIE_CTX ProjectSlot4DpCtx = {"RAID CARD", SlotTypePciExpress, SlotDataBusWidth8X, SlotUsageAvailable, SlotLengthLong, 4};
STATIC PLATFORM_HOST_INFO_PCIE_CTX ProjectSlot5DpCtx = {"Slimline", SlotTypePciExpress, SlotDataBusWidth8X, SlotUsageAvailable, SlotLengthLong, 5};

PLATFORM_HOST_INFO gProjectRiser03SlotInfo[] = {
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot0Dp, &ProjectSlot0DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot1Dp, &ProjectSlot1DpCtx},    
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot2Dp, &ProjectSlot2DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot3Dp, &ProjectSlot3DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot4Dp, &ProjectSlot4DpCtx},    
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot5Dp, &ProjectSlot5DpCtx},
};


PLATFORM_HOST_INFO gProjectRiser13SlotInfo[] = {
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot0Dp, &ProjectSlot0DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot1Dp, &ProjectSlot1DpCtx},    
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot2Dp, &ProjectSlot2DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot3Dp, &ProjectSlot3DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot4Dp, &ProjectSlot4DpCtx},    
};



PLATFORM_HOST_INFO gProjectHostInfo[] = {

  {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectAhci0Dp, &gProjectAhci1Ctx},
      {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectAhci1Dp, &gProjectAhci1Ctx},
      {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectAhci4Dp, &gProjectAhci1Ctx},
        {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectAhci5Dp, &gProjectAhci1Ctx},


  {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gAsm1061R0Dp, &gAsm1061R0Ctx},
  {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gAsm1061R1Dp, &gAsm1061R1Ctx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot0Dp, &ProjectSlot0DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot3Dp, &ProjectSlot3DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot2Dp, &ProjectSlot2DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot1Dp, &ProjectSlot1DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot4Dp, &ProjectSlot4DpCtx}, 
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot5Dp, &ProjectSlot5DpCtx},

  {PLATFORM_HOST_LAN,  (EFI_DEVICE_PATH_PROTOCOL*)&gProjectLan2DpData, NULL},
  {PLATFORM_HOST_LAN,  (EFI_DEVICE_PATH_PROTOCOL*)&gProjectLan3DpData, NULL},

};  

PLATFORM_HOST_INFO gProjectHostInfo1[] = {


  {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectAhci0Dp, &gProjectAhci1Ctx},
    {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectAhci1Dp, &gProjectAhci1Ctx},
      {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectAhci4Dp, &gProjectAhci1Ctx},
        {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectAhci5Dp, &gProjectAhci1Ctx},

  {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gAsm1061R1Dp, &gAsm1061R0Ctx},
  {PLATFORM_HOST_SATA, (EFI_DEVICE_PATH_PROTOCOL*)&gAsm1061R2Dp, &gAsm1061R1Ctx},
  
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot0Dp, &ProjectSlot0DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot3Dp, &ProjectSlot3DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot2Dp, &ProjectSlot2DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot1Dp, &ProjectSlot1DpCtx},
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot4Dp, &ProjectSlot4DpCtx},    
  {PLATFORM_HOST_PCIE, (EFI_DEVICE_PATH_PROTOCOL*)&gProjectSlot5Dp, &ProjectSlot5DpCtx},
  
  {PLATFORM_HOST_LAN,  (EFI_DEVICE_PATH_PROTOCOL*)&gProjectLan0DpData, NULL},
  {PLATFORM_HOST_LAN,  (EFI_DEVICE_PATH_PROTOCOL*)&gProjectLan1DpData, NULL}, 

}; 


EFI_DEVICE_PATH_PROTOCOL*
GetPlatUcrDp (
    UINTN   *pDpSize,
    UINT16  *pIoBase
  )
{
  PLATFORM_ISA_SERIAL_DEVICE_PATH  *UartDp;
  UINT16                           IoBase;
  UINT8                            SerialPortSelect = 0;


  DEBUG((EFI_D_INFO, "%a %d %d %d\n", __FUNCTION__, \
    gSetupHob->UCREnable, gSetupHob->SerialBaudrate, gSetupHob->TerminalType));

  if(!gSetupHob->UCREnable){
    return NULL;
  }
  
  UartDp = &gSerialDevicePath;
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
  }  

  if(pIoBase != NULL){
    *pIoBase = IoBase;
    DEBUG((EFI_D_INFO, "IoBase:%X\n", *pIoBase));
  }

  if(pDpSize != NULL){
    *pDpSize = sizeof(gSerialDevicePath);
  }

  return (EFI_DEVICE_PATH_PROTOCOL*)UartDp;
}  



void SecureBootViolationHook()
{
  CHAR16  *Title, *Content;

  BltSaveAndRetore(gBS, TRUE);
  gST->ConOut->ClearScreen(gST->ConOut);

  Title = HiiGetString(gHiiHandle, STRING_TOKEN(STR_SECURE_BOOT_VIOLATION_TITLE), NULL);
  Content = HiiGetString(gHiiHandle, STRING_TOKEN(STR_SECURE_BOOT_VIOLATION_INFO), NULL);
  UiConfirmDialog(DIALOG_WARNING, Title, NULL, TEXT_ALIGIN_CENTER, Content, NULL);
  FreePool(Title);
  FreePool(Content);

  BltSaveAndRetore(gBS, FALSE);
}



EFI_STATUS
EFIAPI
 ProjectStopBmcWatchDog (
    VOID
  )
{
  SignalProtocolEvent(gBS, &gEfiBootMenuEnterGuid, TRUE);
  return EFI_SUCCESS;
}


void ProjectAfterAllPciIoHook()
{
  UINTN                                 HandleCount;
  EFI_HANDLE                            *Handles = NULL;
  EFI_STATUS                            Status;
  UINTN                                 Index;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  UINT8                                 ClassCode[3];
  EFI_DEVICE_PATH_PROTOCOL              *PciDp;
  PLAT_HOST_INFO_PROTOCOL               *ptHostInfo;
  EFI_DEVICE_PATH_PROTOCOL              *IgdPciDp = NULL;
  UINTN                                 IgdPciDpSize = 0;
  EFI_HANDLE                            IgdHandle = NULL;  
  EFI_HANDLE                            PegHandle = NULL;
  UINT32                                SystemMiscCfg;
  EFI_DEVICE_PATH_PROTOCOL              *ParentPciDp;
  EFI_DEVICE_PATH_PROTOCOL              *Temp;
  EFI_HANDLE                            ParentHandle;
  EFI_HANDLE                            VgaHandle;
  UINT8                                 Data8;
  UINT32                                PciId;


  SystemMiscCfg = PcdGet32(PcdSystemMiscConfig);

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  if(!EFI_ERROR(Status)){
    IgdPciDp = ptHostInfo->IgdDp;
    IgdPciDpSize = ptHostInfo->IgdDpSize;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if(EFI_ERROR(Status) || HandleCount == 0){
    goto ProcExit;
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID**)&PciIo
                    );
    ASSERT(!EFI_ERROR(Status));

    Status = PciIo->Pci.Read (
                      PciIo,
                      EfiPciIoWidthUint8,
                      0x9,
                      3,
                      ClassCode
                      );
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR, "ClassCode Read error:%r\n", Status));
      continue;
    }

    if(ClassCode[2] != 3 || ClassCode[1] != 0 || ClassCode[0] != 0){
      continue;
    }

    Status = gBS->HandleProtocol (Handles[Index], &gEfiDevicePathProtocolGuid, (VOID**)&PciDp);
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_ERROR, "Dp not found\n"));
      continue;
    }
    
    if(IgdHandle == NULL && IgdPciDp != NULL && CompareMem(PciDp, IgdPciDp, IgdPciDpSize) == 0){
      IgdHandle = Handles[Index];
    } else {
      if(PegHandle == NULL){
        PegHandle = Handles[Index];
      }
    }
    
  }


  if(PegHandle == NULL || IgdHandle == NULL){
    goto ProcExit;
  }

  if(SystemMiscCfg & SYS_MISC_CFG_PRI_VGA_IGD){
    VgaHandle = PegHandle;
  } else {
    VgaHandle = IgdHandle;
  }

  Status = gBS->HandleProtocol(VgaHandle, &gEfiDevicePathProtocolGuid, (VOID**)&PciDp);
  Status = gBS->HandleProtocol(VgaHandle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
  if(VgaHandle == PegHandle){
    Status = PciIo->Pci.Read (
                      PciIo,
                      EfiPciIoWidthUint32,
                      0,
                      1,
                      &PciId
                      ); 
    if(!EFI_ERROR(Status) && ptHostInfo != NULL){
      ptHostInfo->AddOnGfxId = PciId;
    }
  }

  ShowDevicePathDxe(gBS, PciDp);
  ParentPciDp = GetPciParentDp(gBS, PciDp);
  Temp = ParentPciDp;
  if(VgaHandle == IgdHandle){                    // bmc has a bridge
    Temp = GetPciParentDp(gBS, ParentPciDp);
    FreePool(ParentPciDp);
    ParentPciDp = Temp; 
  }
  ShowDevicePathDxe(gBS, ParentPciDp); 
  
  Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Temp, &ParentHandle);
  if(!EFI_ERROR(Status) && IsDevicePathEnd(Temp)){
    Status = gBS->HandleProtocol(ParentHandle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, 0x68, 1, &Data8);
    Data8 |= BIT4;
    PciIo->Pci.Write(PciIo, EfiPciIoWidthUint8, 0x68, 1, &Data8);
    DEBUG((EFI_D_INFO, "DisLink\n"));
  }
  FreePool(ParentPciDp);
    
ProcExit:  
  if(Handles != NULL){
    FreePool(Handles);
  }
}





EFI_STATUS AddRiserSmbiosType9(EFI_SMBIOS_PROTOCOL *Smbios, PLATFORM_HOST_INFO *Info, UINTN InfoCount)
{
  UINTN                              SlotStrLen;
  EFI_STATUS                         Status = EFI_SUCCESS;
  SMBIOS_TABLE_TYPE9                 *SmbiosRecord;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  UINTN                              Index;
  PLATFORM_HOST_INFO_PCIE_CTX        *PcieCtx;
  EFI_DEVICE_PATH_PROTOCOL           *Dp;
  EFI_DEVICE_PATH_PROTOCOL           *ParentDp;
  EFI_HANDLE                         DeviceHandle;  
  EFI_PCI_IO_PROTOCOL                *PciIo;
  UINTN                              Seg, Bus, Dev, Fun;
  BOOLEAN                            DevPresent;
  
  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  DEBUG((EFI_D_INFO, "InfoCount is %d\n",InfoCount));

  for(Index=0;Index<InfoCount;Index++){

 	 DEBUG((EFI_D_INFO, "Index is %d\n",Index));

    PcieCtx = (PLATFORM_HOST_INFO_PCIE_CTX*)Info[Index].HostCtx;

    SlotStrLen = AsciiStrLen(PcieCtx->SlotName);
    SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE9) + SlotStrLen + 1 + 1);
    SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_SLOTS;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE9);
    SmbiosRecord->Hdr.Handle = 0; 
    SmbiosRecord->SlotDesignation  = 1;
    SmbiosRecord->SlotType         = PcieCtx->SlotType;
    SmbiosRecord->SlotDataBusWidth = PcieCtx->SlotbusWidth;
    SmbiosRecord->CurrentUsage     = PcieCtx->SlotUsage;
    SmbiosRecord->SlotLength       = PcieCtx->SlotLen;
    SmbiosRecord->SlotID           = PcieCtx->SlotId;

    *(UINT8*)&SmbiosRecord->SlotCharacteristics1 = 0x0c;
    *(UINT8*)&SmbiosRecord->SlotCharacteristics2 = 0x01;

    SmbiosRecord->SegmentGroupNum = 0xFF;
    SmbiosRecord->BusNum = 0xFF;
    SmbiosRecord->DevFuncNum = 0xFF;
    ParentDp = GetPciParentDp(gBS, Info[Index].Dp);
    if(ParentDp == NULL){
      FreePool(SmbiosRecord);
      continue; 
    }

    Dp = Info[Index].Dp;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DeviceHandle);
    if(!EFI_ERROR(Status) && IsDevicePathEnd(Dp)){
      DevPresent = TRUE;
    } else {
      DevPresent = FALSE;
    }
    
    Dp = ParentDp;
    Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DeviceHandle);
    if(!EFI_ERROR(Status) && IsDevicePathEnd(Dp)){
      Status = gBS->HandleProtocol(DeviceHandle, &gEfiPciIoProtocolGuid, &PciIo);
      if(!EFI_ERROR(Status)){
        PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
        SmbiosRecord->SegmentGroupNum = (UINT16)Seg;
        SmbiosRecord->BusNum     = (UINT8)Bus;
        SmbiosRecord->DevFuncNum = ((UINT8)Dev << 3) | (UINT8)Fun;
        DEBUG((EFI_D_INFO, "PCI(%X,%X,%X)\n", Bus, Dev, Fun));

        if(DevPresent){
          SmbiosRecord->CurrentUsage = SlotUsageInUse;
        }
        
      }
    }
    FreePool(ParentDp);

    AsciiStrCpy((CHAR8*)(SmbiosRecord + 1), PcieCtx->SlotName);

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = Smbios->Add (
                       Smbios,
                       NULL,
                       &SmbiosHandle,
                       (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord
                       );
    FreePool(SmbiosRecord);
      
  }

  return Status;
}




void ProjectAfterConnectSequenceHook()
{
//  BYO_SYS_CFG_STS_DATA   *SysCfg;
  EFI_STATUS             Status;
  EFI_SMBIOS_PROTOCOL    *Smbios;  


  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    return;
  }

  if(PcdGet8(PcdSmileline)==3){
     //AddRiserSmbiosType9(Smbios, gProjectRiser13SlotInfo,5);
  }else{
    //AddRiserSmbiosType9(Smbios, gProjectRiser03SlotInfo,6);
  }

}

/*
STATIC EFI_STATUS AddSmbiosType39(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                         Status;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  SMBIOS_TABLE_TYPE39                *SmbiosRecord;
  CHAR8                              *Psu1 = "PSU1";
  CHAR8                              *DummyStr = " ";
  UINTN                              MyStrSize;
  UINTN                              Psu1StrSize;
  CHAR8                              *p;
  UINTN                              Index;


  MyStrSize = AsciiStrSize(DummyStr);
  Psu1StrSize = AsciiStrSize(Psu1);
  SmbiosRecord = AllocateZeroPool(sizeof(SMBIOS_TABLE_TYPE39) + Psu1StrSize + 6 * MyStrSize + 1);
  ASSERT(SmbiosRecord != NULL);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY;
  SmbiosRecord->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE39);
  SmbiosRecord->Hdr.Handle = 0;  
  SmbiosRecord->PowerUnitGroup = 1;
  SmbiosRecord->Location = 1;
  SmbiosRecord->DeviceName = 2;
  SmbiosRecord->Manufacturer = 3;
  SmbiosRecord->SerialNumber = 4;
  SmbiosRecord->AssetTagNumber = 5;
  SmbiosRecord->ModelPartNumber = 6;
  SmbiosRecord->RevisionLevel = 7;
  SmbiosRecord->MaxPowerCapacity = 0x8000;            // unknown
  SmbiosRecord->InputVoltageProbeHandle = 0xFFFF;     // none
  SmbiosRecord->CoolingDeviceHandle = 0xFFFF;         // none
  SmbiosRecord->InputCurrentProbeHandle = 0xFFFF;     // none
  SmbiosRecord->PowerSupplyCharacteristics.PowerSupplyPresent = 1;
  SmbiosRecord->PowerSupplyCharacteristics.InputVoltageRangeSwitch = 2;   // Unknown
  SmbiosRecord->PowerSupplyCharacteristics.PowerSupplyStatus = 3;         // OK
  SmbiosRecord->PowerSupplyCharacteristics.PowerSupplyType = 4;           // Switching

  p = (CHAR8*)(SmbiosRecord + 1);
  for(Index=1;Index<=7;Index++){
    if(Index == 2){
      AsciiStrCpy(p, Psu1);
      p += Psu1StrSize;
    } else {
      AsciiStrCpy(p, DummyStr);
      p += MyStrSize;
    }
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord
                     );

  p = LibSmbiosGetStringInTypeByIndex(&SmbiosRecord->Hdr, SmbiosRecord->DeviceName);
  Index = AsciiStrLen(p);
  p[Index-1]++;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord
                     );
  
  FreePool(SmbiosRecord);
  return Status;
}
*/


VOID
ProjectSmbiosCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS             Status;
  EFI_SMBIOS_PROTOCOL    *Smbios;  
  

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

// AddSmbiosType39(Smbios);
}

EFI_STATUS CheckHiddenItem (VOID)
{
  SYSTEM_PASSWORD                   PasswordVariable;
  UINTN                             VarSize;
  EFI_STATUS                        Status = EFI_SUCCESS;
  BYO_SMIFLASH_PROTOCOL             *ByoSmiFlash;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  VarSize = sizeof (SYSTEM_PASSWORD);
  Status = gRT->GetVariable (
                  SYSTEM_PASSWORD_NAME,
                  &gEfiSystemPasswordVariableGuid,
                  NULL,
                  &VarSize,
                  &PasswordVariable
                  );
  if (!EFI_ERROR(Status)&&(PasswordVariable.HiddenEnteredType == 0x5A)) {    
    PasswordVariable.HiddenEnteredType = 0xA5;
    Status = gRT->SetVariable (
                    SYSTEM_PASSWORD_NAME,
                    &gEfiSystemPasswordVariableGuid,
                     EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    VarSize,
                    &PasswordVariable
                    );

       gBS->LocateProtocol (
              &gByoSmiFlashProtocolGuid,
              NULL,
              (VOID**)&ByoSmiFlash
              );
       ByoSmiFlash->SaveSetupPassword(ByoSmiFlash);
  }
  return Status;
}



VOID
ProjectBmBootHook (
  VOID  *Param   
  )
{
  BYO_BM_BOOT_HOOK_PARAMETER   *p;

  
  p = (BYO_BM_BOOT_HOOK_PARAMETER*)Param;
  ASSERT(p->Sign == BYO_BM_BOOT_HOOK_PARAMETER_SIGN);

  DEBUG((EFI_D_INFO, "%s G:%X\n", p->BootOption->Description, p->BootOption->GroupType));
}



EFI_STATUS
ProjectDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_EVENT                       Event; 
  PLAT_HOST_INFO_PROTOCOL         *PlatHostInfo;  
  UINTN                           Index;
  UINT8                           ChoseDp;


  gSetupHob = GetSetupDataHobData();

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &PlatHostInfo);
  ASSERT(!EFI_ERROR(Status));

  gHiiHandle = HiiAddPackages (
                 &gEfiCallerIdGuid,
                 gImageHandle,
                 STRING_ARRAY_NAME,
                 NULL
                 );
  ASSERT(gHiiHandle != NULL);

  ChoseDp = PcdGet8(PcdSmileline);
  PlatHostInfo->SetupAddOnHandle = gHiiHandle;
  PlatHostInfo->SetupTitleId = STRING_TOKEN(STR_SETUP_TITLE);

  PlatHostInfo->HotKey = gPlatPostHotKeyCtx;
  PlatHostInfo->HotKeyCount = gPlatPostHotKeyCtxCount;

  if(ChoseDp==3||ChoseDp==2){ //SATA config
    PlatHostInfo->HostList  = gProjectHostInfo;
	PlatHostInfo->HostCount = ARRAY_SIZE(gProjectHostInfo);
    PlatHostInfo->IgdDp = (EFI_DEVICE_PATH_PROTOCOL*)&gProjectBmcVgaDpData1;
    PlatHostInfo->IgdDpSize = sizeof(gProjectBmcVgaDpData1);  
  }else{
    PlatHostInfo->HostList  = gProjectHostInfo1;
	PlatHostInfo->HostCount = ARRAY_SIZE(gProjectHostInfo1);
	PlatHostInfo->IgdDp = (EFI_DEVICE_PATH_PROTOCOL*)&gProjectBmcVgaDpData;
    PlatHostInfo->IgdDpSize = sizeof(gProjectBmcVgaDpData);  
  }
  PlatHostInfo->SataHostCount = 0;

  for(Index=0;Index<PlatHostInfo->HostCount;Index++){
    if(PlatHostInfo->HostList[Index].HostType == PLATFORM_HOST_SATA){
      PlatHostInfo->SataHostCount++;
    }
  }

  DEBUG((EFI_D_INFO, "SataHostCount:%d\n", PlatHostInfo->SataHostCount));

  PlatHostInfo->GetPlatUcrDp = GetPlatUcrDp;
  PlatHostInfo->StopWdg      = ProjectStopBmcWatchDog;

  Status = IsaAcpiDevListDxe();
  ASSERT_EFI_ERROR(Status); 
  Status = CheckHiddenItem();
  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             ProjectOnReadyToBoot,
             NULL,
             &Event
             ); 
  ASSERT_EFI_ERROR(Status); 

/*
  EfiCreateProtocolNotifyEvent (
    &gEfiSmbiosProtocolGuid,
    TPL_CALLBACK,
    ProjectSmbiosCallback,
    NULL,
    &Registration
    );       
*/

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gByoSecureBootViolationHookProtocolGuid, SecureBootViolationHook,
                  &gEfiAfterAllPciIoGuid, ProjectAfterAllPciIoHook,
                  &gPlatAfterConnectSequenceProtocolGuid, ProjectAfterConnectSequenceHook,
//-                  &gByoBmBootHookProtocolGuid, ProjectBmBootHook,
                  NULL
                  );

  return EFI_SUCCESS;
}


