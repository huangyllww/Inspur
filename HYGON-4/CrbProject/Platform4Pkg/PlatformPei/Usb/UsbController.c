/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:

Abstract:

Revision History:

$END----------------------------------------------------------------------------
**/

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/ByoCommLib.h>
#include <Ppi/UsbController.h>
#include <Library/TimerLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoHygonCommLib.h>



#define USB_XHCI_HOST_MAX   8

#define PEI_USB_CONTROLLER_SIGNATURE  SIGNATURE_32 ('M', 'U', 'S', 'B')

typedef struct {
  UINTN                       Signature;
  PEI_USB_CONTROLLER_PPI      UsbControllerPpi;
  EFI_PEI_PPI_DESCRIPTOR      PpiList;
  UINTN                       XhciControllersNum;
  UINTN                       XhciMemBase[USB_XHCI_HOST_MAX];
  UINT8                       PciHostBusNo[USB_XHCI_HOST_MAX];
} USB_CONTROLLER_INFO;


STATIC
EFI_STATUS
EFIAPI
GetUsbController (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN  PEI_USB_CONTROLLER_PPI        *This,
  IN UINT8                          UsbControllerId,
  OUT UINTN                         *ControllerType,
  OUT UINTN                         *MemBaseAddress
  );


STATIC USB_CONTROLLER_INFO gUsbInfo = {
  PEI_USB_CONTROLLER_SIGNATURE,
  {
    GetUsbController
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gPeiUsbControllerPpiGuid,
    &gUsbInfo.UsbControllerPpi
  },
  0,
  {0,},
  {0,},  
};


VOID StopXhci(UINT32 PciBase)
{
  UINT8   CapLen;
  UINT32  UsbSts;
  UINT32  Bar0;
  UINTN   Index;

  if((UINT16)MmioRead32(PciBase + PCI_VID_REG) == 0xFFFF){
    return;
  }

  Bar0 = MmioRead32(PciBase + PCI_BAR0_REG) & 0xFFFFFFF0;
  CapLen = MmioRead8(Bar0);
  DEBUG((EFI_D_INFO, "XHCI BAR0:%X %X\n", Bar0, CapLen));
  
  MmioAnd32(Bar0 + CapLen, (UINT32)~BIT0);

  Index = 0;
  while(1){
    MicroSecondDelay(1000);
    UsbSts = MmioRead32(Bar0 + CapLen + 4);
    if(UsbSts & BIT0){
      break;
    }

    Index++;
    if(Index >= 10 * 1000){      // 10s
      break;
    }
  }
  
  DEBUG((EFI_D_INFO, "Stop %X %X T:%X\n", MmioRead32(Bar0 + CapLen), MmioRead32(Bar0 + CapLen + 4), Index));  
 
}


EFI_STATUS
EFIAPI
XhciEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  UINTN               Base;
  PLATFORM_COMM_INFO  *PlatCommInfo;
  UINTN               Index;
  UINT8               PciBusNo;
  UINTN               XhciPciBase;
  

  DEBUG((EFI_D_INFO, "XhciEndOfPeiCallback\n"));

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  for (Index=0; Index<gUsbInfo.XhciControllersNum; Index++) {

    PciBusNo = gUsbInfo.PciHostBusNo[Index];

    Base        = PCI_DEV_MMBASE(PciBusNo, 7, 1);
    XhciPciBase = PCI_DEV_MMBASE(PciBusNo | 4, 0, 3);

    StopXhci(XhciPciBase);

    MmioWrite8(XhciPciBase+4, 0);
    MmioWrite8(Base+4, 0);
    MmioWrite32(XhciPciBase+0x10, 0);
    MmioWrite32(Base+0x20, 0);

    DumpMem32((VOID*)Base, 256);
    DumpMem32((VOID*)XhciPciBase, 256);

    MmioWrite32(Base+0x18, 0);
  }

  return EFI_SUCCESS;
}


STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiEndOfPeiSignalPpiGuid,
    XhciEndOfPeiCallback 
  },
};



STATIC
EFI_STATUS
EFIAPI
GetUsbController (
  IN  EFI_PEI_SERVICES                **PeiServices,
  IN  PEI_USB_CONTROLLER_PPI          *This,
  IN  UINT8                           UsbControllerId,
  OUT UINTN                           *ControllerType,
  OUT UINTN                           *MemBaseAddress
  )
{
  if (UsbControllerId >= gUsbInfo.XhciControllersNum) {
    return EFI_INVALID_PARAMETER;
  }

  *ControllerType = PEI_XHCI_CONTROLLER;
  *MemBaseAddress = gUsbInfo.XhciMemBase[UsbControllerId];

  DEBUG((EFI_D_INFO, "GetUsbHost %d T:%d A:%X\n", UsbControllerId, *ControllerType, *MemBaseAddress));

  return EFI_SUCCESS;
}



STATIC
EFI_STATUS
AllocateMmio32 (
  IN  UINT8       Bus,
  IN  UINT32      Length,
  IN  UINT32      Align,
  OUT UINTN       *pMmio
  )
{
  FABRIC_MMIO_ATTRIBUTE                 MmioAttr;
  FABRIC_TARGET                         Target;
  UINT64                                BaseAddress;
  UINT64                                AddrLen = Length;
  EFI_STATUS                            Status;

  MmioAttr.MmioType = MMIO_BELOW_4G;
  Target.TgtType    = TARGET_PCI_BUS;
  Target.PciBusNum  = Bus;
  Status = FabricAllocateMmio (
             &BaseAddress, 
             &AddrLen, 
             Align, 
             Target, 
             &MmioAttr
             );
  if(!EFI_ERROR(Status)){
    *pMmio = (UINT32)BaseAddress;
  } else {
    DEBUG((EFI_D_INFO, "AllocateMmio32:%r\n", Status));
  }
  return Status;
}


EFI_STATUS HygonXhciMmioDecodeAndReport(UINT8 Index, PLATFORM_COMM_INFO *PlatCommInfo)
{
  UINTN               XhciBar;
  UINTN               Base;
  UINTN               XhciPciBase;  
  UINT16              MmioStart;
  UINT16              MmioEnd;
  UINT32              MmioRange;
  EFI_STATUS          Status;
  UINT8               PciBusNo;
  UINT8               XhciBusNo;


  DEBUG((EFI_D_INFO, "HygonXhciMmioDecodeAndReport(%d)\n", Index));

  PciBusNo = PlatCommInfo->BusBase[Index];

  Base = PCI_DEV_MMBASE(PciBusNo, 7, 1);
  if((UINT16)MmioRead32(Base) != HG_VENDOR_ID){
    return EFI_NOT_FOUND;
  }

  XhciBusNo = PciBusNo | 4;
  MmioWrite32(Base+0x18, PciBusNo | (XhciBusNo << 8) | (XhciBusNo << 16));
  XhciPciBase = PCI_DEV_MMBASE(XhciBusNo, 0, 3);
  if((UINT16)MmioRead32(XhciPciBase) != HG_VENDOR_ID){
    MmioWrite32(Base+0x18, 0);
    DEBUG((EFI_D_INFO, "xhci[%d] not present\n", Index));
    return EFI_NOT_FOUND;
  }

  Status = AllocateMmio32(PciBusNo, 0x100000, 0xFFFFF, &XhciBar);
  if(EFI_ERROR(Status)){
    DEBUG ((EFI_D_INFO, "AllocateMmio32:%r\n", Status));
    return Status;
  }
  MmioStart = (UINT16)(XhciBar >> 20) << 4;
  MmioEnd   = (UINT16)((XhciBar + 0x100000 - 1) >> 20) << 4;
  MmioRange = MmioStart | (MmioEnd << 16);
  DEBUG((EFI_D_INFO, "XhciBar:%X Mmio(%X,%X) %X\n", XhciBar, MmioStart, MmioEnd, MmioRange));
  MmioWrite32(Base+0x20, MmioRange);

  DEBUG((EFI_D_INFO, "XhciPciBase:%X %X\n", XhciPciBase, MmioRead32(XhciPciBase)));
  MmioWrite32(XhciPciBase+0x10, XhciBar);

  MmioWrite8(XhciPciBase+4, 6);
  MmioWrite8(Base+4, 6);

  DumpMem32((VOID*)Base, 256);
  DumpMem32((VOID*)XhciPciBase, 256);
  DumpMem32((VOID*)(UINTN)XhciBar, 0x40);

  if (gUsbInfo.XhciControllersNum < USB_XHCI_HOST_MAX) {
    gUsbInfo.XhciMemBase[gUsbInfo.XhciControllersNum]  = XhciBar;
    gUsbInfo.PciHostBusNo[gUsbInfo.XhciControllersNum] = PciBusNo;
    gUsbInfo.XhciControllersNum++;
  }
  DEBUG((EFI_D_INFO, "XhciCount:%d\n", gUsbInfo.XhciControllersNum));

  return EFI_SUCCESS;
}


EFI_STATUS
InitUsbController (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                Status;
  PLATFORM_COMM_INFO        *PlatCommInfo;
  UINT8                     Index;
  EFI_BOOT_MODE             BootMode;


  DEBUG((EFI_D_INFO, "InitUsbController\n"));

  BootMode = GetBootModeHob ();
  if(BootMode != BOOT_IN_RECOVERY_MODE){
    return EFI_UNSUPPORTED;
  }

  PlatCommInfo = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();
  for (Index=0; Index < PlatCommInfo->PciHostCount; Index++) {
    Status = HygonXhciMmioDecodeAndReport(Index, PlatCommInfo);
  }

  Status = PeiServicesInstallPpi(&gUsbInfo.PpiList);
  ASSERT_EFI_ERROR (Status);

  Status = PeiServicesNotifyPpi(gNotifyList);
  ASSERT_EFI_ERROR(Status);

  return Status;
}


