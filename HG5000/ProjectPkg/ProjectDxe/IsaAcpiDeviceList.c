/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IsaAcpiDeviceList.c
Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/


#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Protocol/IsaAcpi.h>
#include <Protocol/IsaIo.h>
#include <SetupVariable.h>



#define MAX_ISA_ACPI_DEVICE_COUNT               4

extern CONST SETUP_DATA  *gSetupData;


GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE mPcatIsaAcpiLpt1DeviceResources[] = {
  {EfiIsaAcpiResourceIo,        0, 0x378, 0x37f},
  {EfiIsaAcpiResourceInterrupt, 0, 7,     0},
  {EfiIsaAcpiResourceEndOfList, 0, 0,     0}
};

//
// COM 1 UART Controller
//
GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE mPcatIsaAcpiCom1DeviceResources[] = {
  {EfiIsaAcpiResourceIo,        0, 0x3f8, 0x3ff},
  {EfiIsaAcpiResourceInterrupt, 0, 4,     0},
  {EfiIsaAcpiResourceEndOfList, 0, 0,     0}
};

//
// COM 2 UART Controller
//
GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE mPcatIsaAcpiCom2DeviceResources[] = {
  {EfiIsaAcpiResourceIo,        0, 0x2f8, 0x2ff},
  {EfiIsaAcpiResourceInterrupt, 0, 3,     0},
  {EfiIsaAcpiResourceEndOfList, 0, 0,     0}
};

//
// COM 3 UART Controller
//
GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE mPcatIsaAcpiCom3DeviceResources[] = {
  {EfiIsaAcpiResourceIo,        0, 0x3e8, 0x2ef},
  {EfiIsaAcpiResourceInterrupt, 0, 3,     0},
  {EfiIsaAcpiResourceEndOfList, 0, 0,     0}
};

//
// COM 4 UART Controller
//
GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE mPcatIsaAcpiCom4DeviceResources[] = {
  {EfiIsaAcpiResourceIo,        0, 0x2e8, 0x2ef},
  {EfiIsaAcpiResourceInterrupt, 0, 3,     0},
  {EfiIsaAcpiResourceEndOfList, 0, 0,     0}
};

//
// PS/2 Keyboard Controller
//
GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE  mPcatIsaAcpiPs2KeyboardDeviceResources[] = {
  {EfiIsaAcpiResourceIo,        0, 0x60, 0x64},
  {EfiIsaAcpiResourceInterrupt, 0, 1,     0},
  {EfiIsaAcpiResourceEndOfList, 0, 0,     0}
};

//
// PS/2 Mouse Controller
//
GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE  mPcatIsaAcpiPs2MouseDeviceResources[] = {
  {EfiIsaAcpiResourceIo,        0, 0x60, 0x64},
  {EfiIsaAcpiResourceInterrupt, 0, 12,     0},
  {EfiIsaAcpiResourceEndOfList, 0, 0,     0}
};

//
// Floppy Disk Controller
//
GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE mPcatIsaAcpiFloppyResources[] = {
  {EfiIsaAcpiResourceIo,        0, 0x3f0, 0x3f7},
  {EfiIsaAcpiResourceInterrupt, 0, 6,     0},
  {EfiIsaAcpiResourceDma,       EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SPEED_COMPATIBLE | EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_WIDTH_8 | EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SINGLE_MODE, 2,     0},
  {EfiIsaAcpiResourceEndOfList, 0, 0,     0}
};

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE_LIST mPcatIsaAcpiLpt1Device = {
  {EISA_PNP_ID(0x401), 0}, mPcatIsaAcpiLpt1DeviceResources
}; // COM 1 UART Controller

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE_LIST mPcatIsaAcpiCom1Device = {
  {EISA_PNP_ID(0x501), 0}, mPcatIsaAcpiCom1DeviceResources
}; // COM 1 UART Controller

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE_LIST mPcatIsaAcpiCom2Device = {
  {EISA_PNP_ID(0x501), 1}, mPcatIsaAcpiCom2DeviceResources
}; // COM 2 UART Controller

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE_LIST mPcatIsaAcpiCom3Device = {
  {EISA_PNP_ID(0x501), 2}, mPcatIsaAcpiCom3DeviceResources
}; // COM 1 UART Controller

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE_LIST mPcatIsaAcpiCom4Device = {
  {EISA_PNP_ID(0x501), 3}, mPcatIsaAcpiCom4DeviceResources
}; // COM 2 UART Controller

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE_LIST mPcatIsaAcpiPs2KeyboardDevice = {
  {EISA_PNP_ID(0x303), 0}, mPcatIsaAcpiPs2KeyboardDeviceResources
}; // PS/2 Keyboard Controller

GLOBAL_REMOVE_IF_UNREFERENCED
EFI_ISA_ACPI_RESOURCE_LIST mPcatIsaAcpiPs2MouseDevice = {
  {EISA_PNP_ID(0x303), 1}, mPcatIsaAcpiPs2MouseDeviceResources
}; // PS/2 Mouse Controller

EFI_ISA_ACPI_RESOURCE_LIST gIsaAcpiDevList[MAX_ISA_ACPI_DEVICE_COUNT+1]; // should big enough.
CONST UINT16 gComBaseList[] = {0x3F8, 0x2F8, 0x3E8, 0x2E8};



EFI_STATUS IsaAcpiDevListDxe()
{
  UINTN                       Index = 0;
  EFI_ISA_ACPI_RESOURCE_LIST  *List;
  EFI_ISA_ACPI_RESOURCE_LIST  *TargetList;
  EFI_STATUS                  Status;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  ZeroMem(gIsaAcpiDevList, sizeof(gIsaAcpiDevList));
  TargetList = gIsaAcpiDevList;
  List = TargetList;

  CopyMem(&TargetList[Index++], &mPcatIsaAcpiCom1Device, sizeof(EFI_ISA_ACPI_RESOURCE_LIST));

  ASSERT(Index <= MAX_ISA_ACPI_DEVICE_COUNT);

  if(Index != 0){
    Status = gBS->InstallProtocolInterface (
                    &gImageHandle,
                    &gIsaAcpiDeviceListGuid,
                    EFI_NATIVE_INTERFACE,
                    TargetList
                    );
  } else {
    Status = EFI_UNSUPPORTED;
  }

  return Status;
}


