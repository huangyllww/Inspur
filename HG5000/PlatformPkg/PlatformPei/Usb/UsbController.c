/*++

Module Name:
  
  MultiUsbController.c

Abstract:

  Usb Controller PPI Init

--*/
#include "UsbController.h"
#include <PlatformDefinition.h>

//
// PPI interface function
//
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

//
// Globals
//
STATIC PEI_USB_CONTROLLER_PPI  mUsbControllerPpi = { GetUsbController };

STATIC EFI_PEI_PPI_DESCRIPTOR   mPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiUsbControllerPpiGuid,
  NULL
};

//
// PPI interface implementation
//
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
  USB_CONTROLLER_INFO *PeiUsbControllerInfo;
  UINTN XhciIdStart,UhciIdStart,OhciIdStart;

  PeiUsbControllerInfo = USB_CONTROLLER_INFO_FROM_THIS(This);

  if (UsbControllerId >= PeiUsbControllerInfo->TotalUsbControllers) {
    return EFI_INVALID_PARAMETER;
  }

// EHCI
// XHCI
// UHCI
// OHCI

  XhciIdStart = PeiUsbControllerInfo->EhciControllersNum;
  UhciIdStart = XhciIdStart + PeiUsbControllerInfo->XhciControllersNum;
  OhciIdStart = UhciIdStart + PeiUsbControllerInfo->UhciControllersNum;
  
  PeiUsbControllerInfo = USB_CONTROLLER_INFO_FROM_THIS (This);

  if (UsbControllerId < XhciIdStart){
    *ControllerType = PEI_EHCI_CONTROLLER;
    *MemBaseAddress = PeiUsbControllerInfo->EhciMemBase[UsbControllerId];
  } else if (UsbControllerId < UhciIdStart){
    *ControllerType = PEI_XHCI_CONTROLLER;
    *MemBaseAddress = PeiUsbControllerInfo->XhciMemBase[UsbControllerId - XhciIdStart];
  } else if (UsbControllerId < OhciIdStart){
    *ControllerType = PEI_UHCI_CONTROLLER;
  } else {
    *ControllerType = PEI_OHCI_CONTROLLER;
  }

  DEBUG((EFI_D_INFO, "GetUsbHost %d T:%d A:%X\n", UsbControllerId, *ControllerType, *MemBaseAddress));

  return EFI_SUCCESS;
}



#define XHCI_PCI_REG1(Reg)         (PCI_DEV_MMBASE(0x01, 0, 3) + Reg)
#define XHCI_PCI_REG2(Reg)         (PCI_DEV_MMBASE(0x21, 0, 3) + Reg) 
#define XHCI_PCI_REG3(Reg)         (PCI_DEV_MMBASE(0x51, 0, 3) + Reg)

#define XHCI_BAR_TMP_BASE1         0xEDB00000
#define XHCI_BAR_TMP_BASE2         0xEB300000
#define XHCI_BAR_TMP_BASE3         0xF4000000

VOID
DumpPcilist()
{
  UINT8 bus;
  UINT8 dev;
  UINT8 fun;
  for(bus=0;bus<0x80;bus++){
    for(dev=0;dev<32;dev++){
       for(fun=0;fun<8;fun++){
         if((UINT16)MmioRead32(PCI_DEV_MMBASE(bus,dev,fun)+00) != 0xFFFF){
          DEBUG ((EFI_D_ERROR, "bus:%x,dev:%x,fun:%x VendorID:%x  ", bus, dev, fun, (UINT16)MmioRead32(PCI_DEV_MMBASE(bus,dev,fun)+00)));
          DEBUG ((EFI_D_ERROR, "DeviceID:%x\n",(UINT16)(MmioRead32(PCI_DEV_MMBASE(bus,dev,fun)+00)>>16))); 
         }
       }
    }
  }
}

DumpPciConfig(
  UINT8 Bus,
  UINT8 Dev,
  UINT8 Fun,
  UINT16 Start,
  UINT16 Length
){
  UINT16 offset = 0;
  UINT32 Data32 = 0;
  if((UINT16)MmioRead32(PCI_DEV_MMBASE(Bus, Dev, Fun)+00) != 0xFFFF){
     DEBUG ((EFI_D_ERROR, "\nbus:%02x,dev:%02x,fun:%02x \n", Bus, Dev, Fun));   
     for(offset = 0; offset<Length ; offset+=4){
       Data32 = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+Start+offset);
       if(offset%0x10 == 0 && offset !=0){
          DEBUG((EFI_D_ERROR, "\noffset%0x:", Start+offset));
       }else if(offset ==0){
           DEBUG((EFI_D_ERROR, "offset%0x:", Start+offset));
       } 
       DEBUG((EFI_D_ERROR, " %02x %02x %02x %02x", (UINT8)(Data32), (UINT8)(Data32>>8), (UINT8)(Data32>>16), (UINT8)(Data32>>24) ));
     }
     DEBUG((EFI_D_ERROR,"\n"));
  }
}

VOID
InitPciBridgeTempResource(
  UINT8            Bus,
  UINT8            Dev,
  UINT8            Fun,
  UINT16          MemoryBase,
  UINT16          MemoryLimit
)
{
  UINT8                     pribus;
  UINT8                     secbus;
  UINT8                     subbus;
  UINT32                    Data32;
  
  pribus = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x18) & 0xff;
  secbus = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x18)>>8 & 0xff;
  subbus = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x18)>>16 & 0xff;
  DEBUG ((EFI_D_ERROR, "Before Resource Assignment \n"));
  DEBUG ((EFI_D_ERROR, "bus:0x%02x,dev:0x%02x,fun:0x%02x pribus=0x%02x secbus=0x%02x subbus=0x%02x \n", Bus, Dev, Fun,pribus, secbus, subbus));
  DEBUG ((EFI_D_ERROR, "Bridge Mem base:0x%0x \n",  (UINT32)(MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x20)<<16)));
  DEBUG ((EFI_D_ERROR, "Bridge Mem Limit:0x%0x  \n",  (UINT32)(MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x20 ) |0xFFFFF)));

  Data32 = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun) + 0x18);  //Temp Bus assignment
  Data32 |= (UINT32)((Bus+1)<<16 | (Bus+1)<<8 | Bus);
  MmioWrite32(PCI_DEV_MMBASE(Bus,Dev,Fun) + 0x18, Data32); 
  Data32 = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x20);    //Temp  Mmio base/Limit assignment
  Data32 |= (UINT32)(MemoryLimit<<16 | MemoryBase);  
  MmioWrite32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x20, Data32); 
  Data32 = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun) + 0x04);  // Memory Decode Enable
  Data32 |= (UINT32)(BIT1 | BIT2);
  MmioWrite32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x04, Data32);
  pribus = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x18) & 0xff;
  secbus = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x18)>>8 & 0xff;
  subbus = MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x18)>>16 & 0xff;
  DEBUG ((EFI_D_ERROR, "After Resource Assignment \n"));
  DEBUG ((EFI_D_ERROR, "bus:0x%02x,dev:0x%02x,fun:0x%02x pribus=0x%02x secbus=0x%02x subbus=0x%02x \n", Bus, Dev, Fun, pribus, secbus, subbus));
  DEBUG ((EFI_D_ERROR, "Bridge Mem base:%x  \n",  (UINT32)(MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x20)<<16)));
  DEBUG ((EFI_D_ERROR, "Bridge Mem Limit:%x  \n", (UINT32)(MmioRead32(PCI_DEV_MMBASE(Bus,Dev,Fun)+0x20) | 0xFFFFF)));
}

EFI_STATUS
InitUsbController (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                Status;
  USB_CONTROLLER_INFO       *PeiUsbControllerInfo;
  UINTN                     XhciNum = 0;
	
  DEBUG ((EFI_D_INFO, "InitUsbController() Start\n"));

  PeiUsbControllerInfo = (USB_CONTROLLER_INFO*) AllocateZeroPool(sizeof(USB_CONTROLLER_INFO));
  if (PeiUsbControllerInfo == NULL) {
    DEBUG ((EFI_D_ERROR, "Failed to allocate memory for PeiEhciDev!\n"));
    return EFI_OUT_OF_RESOURCES;
  }
   InitPciBridgeTempResource(0X00, 0x07, 0x01, 0xEDB0, 0xEDD0);   //Init Bus=0, Dev=7, Fun=01 USB Bridge Resource.
   InitPciBridgeTempResource(0X20, 0x07, 0x01, 0xEB30, 0xEB50);   //Init Bus=20, Dev=7, Fun=01 USB Bridge Resource. 
   MmioWrite32(PCI_DEV_MMBASE(0x00, 0x18, 0x00)+0x200, 0x00ec00);
   MmioWrite32(PCI_DEV_MMBASE(0x00, 0x18, 0x00)+0x204, 0x00fec0);
   MmioWrite32(PCI_DEV_MMBASE(0x00, 0x18, 0x00)+0x208, 0x000043);
   MmioWrite32(PCI_DEV_MMBASE(0x00, 0x18, 0x00)+0x210, 0x00e980);
   MmioWrite32(PCI_DEV_MMBASE(0x00, 0x18, 0x00)+0x214, 0x00ebff);
   MmioWrite32(PCI_DEV_MMBASE(0x00, 0x18, 0x00)+0x218, 0x000243);
   
  // Need allocate resource and decode it frist
  if((UINT16)MmioRead32(XHCI_PCI_REG1(0)) != 0xFFFF){
    PeiUsbControllerInfo->XhciPciAddr[XhciNum] = XHCI_PCI_REG1(0);
    PeiUsbControllerInfo->XhciMemBase[XhciNum] = XHCI_BAR_TMP_BASE1;
    XhciNum ++;
  }
  if((UINT16)MmioRead32(XHCI_PCI_REG2(0)) != 0xFFFF){
    PeiUsbControllerInfo->XhciPciAddr[XhciNum] = XHCI_PCI_REG2(0);
    PeiUsbControllerInfo->XhciMemBase[XhciNum] = XHCI_BAR_TMP_BASE2;
    XhciNum ++;    
  } 
  if((UINT16)MmioRead32(XHCI_PCI_REG3(0)) != 0xFFFF){
    PeiUsbControllerInfo->XhciPciAddr[XhciNum] = XHCI_PCI_REG3(0);
    PeiUsbControllerInfo->XhciMemBase[XhciNum] = XHCI_BAR_TMP_BASE3;
    XhciNum ++;    
  } 
  DEBUG ((EFI_D_INFO, "InitUsbController() XhciNum =%x \n", XhciNum));
  ASSERT(XhciNum <= USB_XHCI_HOST_MAX);
  
  PeiUsbControllerInfo->Signature           = PEI_USB_CONTROLLER_SIGNATURE;
  PeiUsbControllerInfo->UsbControllerPpi    = mUsbControllerPpi;
  PeiUsbControllerInfo->PpiList             = mPpiList;
  PeiUsbControllerInfo->PpiList.Ppi         = &PeiUsbControllerInfo->UsbControllerPpi;
  PeiUsbControllerInfo->EhciControllersNum  = 0;
  PeiUsbControllerInfo->XhciControllersNum  = XhciNum;
  PeiUsbControllerInfo->UhciControllersNum  = 0;
  PeiUsbControllerInfo->TotalUsbControllers = XhciNum;
  DEBUG ((EFI_D_INFO, "USB HostCount:%d\n", PeiUsbControllerInfo->TotalUsbControllers));

  InitUsbControl (PeiServices, PeiUsbControllerInfo);  

  Status = PeiServicesInstallPpi (&PeiUsbControllerInfo->PpiList);
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_INFO, "InitUsbController() End\n"));

  return Status;

}


