/** @file

Cxl CxlDevice Driver.

**/
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 *
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 *
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 *
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 *
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 *
 * You agree that you will not reverse engineer or decompile the Materials.
 *
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 *
 *
 ******************************************************************************
 */

#include "CxlDeviceDriver.h"

//
// Consumed Protocols
//
HYGON_NBIO_CXL11_SERVICES_PROTOCOL   *gCxl11ServicesProtocol = NULL;
HYGON_CXL_MANAGER_PROTOCOL           *gCxlMgrProtocol = NULL;

CXL_DEVICE                            gCxlDeviceList[MAX_CXL_DEVICES];
UINTN                                 gCxlDeviceCount = 0;      // 0..MAX_CXL_DEVICES

//
// Produced Protocol
//
EFI_DRIVER_BINDING_PROTOCOL gCxlDeviceDriverBinding = {
  CxlDeviceSupported,
  CxlDeviceStart,
  CxlDeviceStop,
  0xA,
  NULL,
  NULL
};


//
// Driver binding functions declaration
//
/**
  Supported function of Driver Binding protocol for this driver.
  Test to see if this driver supports ControllerHandle.

  @param This                   Protocol instance pointer.
  @param Controller             Handle of CxlDevice to test.
  @param RemainingDevicePath    A pointer to the CxlDevice path. Should be ignored by
                                CxlDevice driver.

  @retval EFI_SUCCESS           This driver supports this CxlDevice.
  @retval EFI_ALREADY_STARTED   This driver is already running on this CxlDevice.
  @retval other                 This driver does not support this CxlDevice.

**/
EFI_STATUS
EFIAPI
CxlDeviceSupported (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_PCI_IO_PROTOCOL  *PciIo;
  EFI_STATUS           Status;
  UINTN                PciSegment;
  UINTN                PciBus;
  UINTN                PciDevice;
  UINTN                PciFunction;
  PCI_TYPE00           PciConfig;
  CXL_DEVICE          *CxlDevice;

  //
  // PCI I/O Protocol attached on this Controller?
  //
  Status = gBS->OpenProtocol (
    Controller,
    &gEfiPciIoProtocolGuid,
    (VOID **)&PciIo,
    This->DriverBindingHandle,
    Controller,
    EFI_OPEN_PROTOCOL_BY_DRIVER
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Get PCI Location
  Status = PciIo->GetLocation (
    PciIo,
    &PciSegment,
    &PciBus,
    &PciDevice,
    &PciFunction
    );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
  
  Status = PciIo->Pci.Read ( PciIo,
                             EfiPciIoWidthUint8,
                             0,
                             sizeof (PCI_TYPE00),
                             &PciConfig
                             );
  
  if (EFI_ERROR(Status)) {
    goto ON_EXIT;
  }

  if (IS_CLASS2 (&PciConfig, PCI_CLASS_MEMORY_CONTROLLER, PCI_CLASS_MEMORY_CXL)) {
    Status = EFI_SUCCESS;
  } else {
    Status = EFI_UNSUPPORTED;
    goto ON_EXIT;
  }
  
  DEBUG ((DEBUG_INFO, "%a: Add [B%02X|D%02X|F%02X] to gCxlDeviceList \n",
    __FUNCTION__, PciBus, PciDevice, PciFunction));
  
  //Add this CXL Device to gCxlDeviceList
  CxlDevice = &gCxlDeviceList[gCxlDeviceCount];
  CxlDevice->Controller = Controller;
  CxlDevice->StartPciIo = PciIo;
  
  CxlDevice->PciAddr.AddressValue = 0;
  CxlDevice->PciAddr.Address.Segment = (UINT32) PciSegment;
  CxlDevice->PciAddr.Address.Bus = (UINT32) PciBus;
  CxlDevice->PciAddr.Address.Device = (UINT32) PciDevice;
  CxlDevice->PciAddr.Address.Function = (UINT32) PciFunction;
  
  CxlDevice->VendorId = PciConfig.Hdr.VendorId;
  CxlDevice->DeviceId = PciConfig.Hdr.DeviceId;
  
  if (PciConfig.Hdr.ClassCode[0] == PCI_IF_CXL_MEM_DEVICE) {
    CxlDevice->CxlVersion = CXL_VERSION_11;
    ProbeCxl11Device (CxlDevice);
  } else if (PciConfig.Hdr.ClassCode[0] == PCI_IF_CXL_20_MEM_DEVICE) {
    if (gCxl11ServicesProtocol != NULL) {
      Status = gCxl11ServicesProtocol->Cxl11GetRootPortInfoByBus (gCxl11ServicesProtocol, (UINT8) CxlDevice->PciAddr.Address.Bus, NULL);
      if (!EFI_ERROR (Status)) {
        CxlDevice->CxlVersion = CXL_VERSION_11;
        ProbeCxl11Device (CxlDevice);
      } else {
        CxlDevice->CxlVersion = CXL_VERSION_20;
        ProbeCxl20Device (CxlDevice);
      }
    } else {
      CxlDevice->CxlVersion = CXL_VERSION_20;
      ProbeCxl20Device (CxlDevice);
    }
  } else {
    CxlDevice->CxlVersion = UNKNOWN_VERSION;
  }
  ++gCxlDeviceCount;
  
ON_EXIT:
  //
  // Close PCI I/O Protocol
  //
  gBS->CloseProtocol (
    Controller,
    &gEfiPciIoProtocolGuid,
    This->DriverBindingHandle,
    Controller
    );

  return Status;
}

/**
  This routine is called right after the .Supported() called and
  Start this driver on ControllerHandle.

  @param This                   Protocol instance pointer.
  @param Controller             Handle of CxlDevice to bind driver to.
  @param RemainingDevicePath    A pointer to the CxlDevice path. Should be ignored by
                                CxlDevice driver.

  @retval EFI_SUCCESS           This driver is added to this CxlDevice.
  @retval EFI_ALREADY_STARTED   This driver is already running on this CxlDevice.
  @retval other                 Some error occurs when binding this driver to this CxlDevice.

**/
EFI_STATUS
EFIAPI
CxlDeviceStart (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  EFI_PCI_IO_PROTOCOL         *PciIo;
  EFI_STATUS                   Status;
  CXL_DEVICE                  *CxlDevice;
  UINTN                        Index;

  DEBUG ((DEBUG_INFO, "\n%a - ENTRY (Handle = %p)\n", __FUNCTION__, Controller));

  //
  // Open PCI I/O Protocol
  //
  Status = gBS->OpenProtocol (
    Controller,
    &gEfiPciIoProtocolGuid,
    (VOID **)&PciIo,
    This->DriverBindingHandle,
    Controller,
    EFI_OPEN_PROTOCOL_BY_DRIVER
    );
  if (EFI_ERROR (Status)) {
    goto ON_EXIT;
  }
    
  for (Index = 0; Index < gCxlDeviceCount; ++Index) {
    CxlDevice = &gCxlDeviceList[Index];
    if (CxlDevice->Controller == Controller) {
      CxlDevice->StartPciIo = PciIo;
      
      if (CxlDevice->CxlVersion == CXL_VERSION_11) {
        Status = InitCxl11Device (CxlDevice);
        
      } else if (CxlDevice->CxlVersion == CXL_VERSION_20) {
        Status = InitCxl20Device (CxlDevice);
      }
      goto ON_EXIT;
    }
  }
  
  //
  // If Controller instance not found: Can't support this CxlDevice!
  //
  DEBUG ((DEBUG_ERROR, "ERROR: Could not find Controller handle!\n"));
  Status = EFI_UNSUPPORTED;
  gBS->CloseProtocol (
    Controller,
    &gEfiPciIoProtocolGuid,
    This->DriverBindingHandle,
    Controller
    );
  
ON_EXIT:
  DEBUG ((DEBUG_INFO, "%a - EXIT (status = %r)\n", __FUNCTION__, Status));
  return Status;
}

/**
  Stop this driver on ControllerHandle.

  @param This               A pointer to the EFI_DRIVER_BINDING_PROTOCOL instance.
  @param Controller         A handle to the CxlDevice being stopped.
  @param NumberOfChildren   The number of child CxlDevice handles in ChildHandleBuffer.
  @param ChildHandleBuffer  An array of child handles to be freed.

  @retval EFI_SUCCESS       This driver is removed from this CxlDevice.
  @retval other             Some error occurs when removing this driver from this CxlDevice.

**/
EFI_STATUS
EFIAPI
CxlDeviceStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN  EFI_HANDLE                   Controller,
  IN  UINTN                        NumberOfChildren,
  IN  EFI_HANDLE                   *ChildHandleBuffer
  )
{
  EFI_STATUS  Status;

  DEBUG ((DEBUG_INFO, "%a - ENTRY (Handle = %p)\n", __FUNCTION__, Controller));

  //
  // Close protocol opened at Start-time
  //
  Status = gBS->CloseProtocol (
    Controller,
    &gEfiPciIoProtocolGuid,
    This->DriverBindingHandle,
    Controller
    );

  DEBUG ((DEBUG_INFO, "%a - EXIT (status = %r)\n", __FUNCTION__, Status));
  return Status;
}

/**
  The CxlDevice Driver entry point.

  @param  ImageHandle    The firmware allocated handle for the EFI image.
  @param  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
CxlDeviceDriverEntryPoint (
  IN  EFI_HANDLE        ImageHandle,
  IN  EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS                      Status;
  CXL_DEVICE                     *CxlDevice;
  FABRIC_CXL_AVAIL_RESOURCE       CxlResourceSize;
  UINT8                           Index;

  DEBUG ((DEBUG_INFO, "%a - ENTRY\n", __FUNCTION__));

  for (Index = 0; Index < MAX_CXL_DEVICES; ++Index) {
    CxlDevice = &gCxlDeviceList[Index];
    CxlDevice->StartPciIo = NULL;
    CxlDevice->DeviceProbed = FALSE;
  }
  //
  // Locate CxlManager Protocol
  //
  Status = gBS->LocateProtocol (
    &gHygonCxlManagerProtocolGuid,
    NULL,
    (VOID **)&gCxlMgrProtocol
    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR: Found gHygonCxlManagerProtocolGuid failed!\n"));
    return Status;
  }
  Status = gCxlMgrProtocol->FabricCxlGetAvailableResource (gCxlMgrProtocol, &CxlResourceSize);
  
  Status = gBS->LocateProtocol (
    &gHygonNbioCxl11ServicesProtocolGuid,
    NULL,
    (VOID **)&gCxl11ServicesProtocol
    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR: Found gHygonNbioCxl11ServicesProtocolGuid failed!\n"));
    gCxl11ServicesProtocol = NULL;
  }
  //
  // Install driver model protocol
  //
  Status = EfiLibInstallDriverBindingComponentName2 (
    ImageHandle,
    SystemTable,
    &gCxlDeviceDriverBinding,
    ImageHandle,
    &gCxlDeviceComponentName,
    &gCxlDeviceComponentName2
    );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "ERROR: EfiLibInstallDriverBindingComponentName2 () failed!\n"));
  }

  DEBUG ((DEBUG_INFO, "%a - EXIT (status = %r)\n", __FUNCTION__, Status));
  return Status;
}
