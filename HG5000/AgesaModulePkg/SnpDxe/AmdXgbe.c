/* $NoKeywords:$ */
/**
 * @file
 *
 * SnpDxe.c
 *
 * Contains Ethernet Driver for the DXE phase.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project: FDK
 * @e sub-project: UEFI
 * @e version: $Revision: 300489 $ @e date: $Date: 2014-08-08 14:36:46 -0700 (Fri, 08 Aug 2014) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#include <Library/UefiLib.h>
#include <Protocol/PciIo.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci22.h>
#include "SnpDxe.h"


EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  );

EFI_STATUS
STATIC
EFIAPI
AmdXgbeGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL *This,
  IN  CHAR8                       *Language,
  OUT CHAR16                      **DriverName
  );

EFI_STATUS
STATIC
EFIAPI
AmdXgbeGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL *This,
  IN  EFI_HANDLE                  ControllerHandle,
  IN  EFI_HANDLE                  ChildHandle,
  IN  CHAR8                       *Language,
  OUT CHAR16                      **ControllerName
  );

#if 0
extern EFI_STATUS
EFIAPI
AmdXgbeInit (
  VOID
        );
#else
extern EFI_STATUS
EFIAPI
AmdXgbeInit (
        IN  EFI_HANDLE                  ControllerHandle
        );
#endif

extern EFI_STATUS
EFIAPI
EthStop (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This
  );

extern ENET_DXE_PRIVATE_DATA    *gPrivate;
extern EFI_HANDLE               gControllerHandle;
EFI_DEVICE_PATH_PROTOCOL        *PciDevicePath;

EFI_DRIVER_BINDING_PROTOCOL gAmdXgbeDriverBinding = {
  AmdXgbeDriverBindingSupported,
  AmdXgbeDriverBindingStart,
  AmdXgbeDriverBindingStop,
  0x10,
  NULL,
  NULL
};

STATIC
EFI_UNICODE_STRING_TABLE mAmdXgbeDriverNameTable[] = {
  { "eng; en", L"AmdXgbe Network Driver" },
  { NULL,     NULL                     }
};

STATIC
EFI_UNICODE_STRING_TABLE mAmdXgbeControllerNameTable[] = {
  { "eng; en", L"AmdXgbe Network Device" },
  { NULL,     NULL                     }
};

EFI_COMPONENT_NAME_PROTOCOL gAmdXgbeComponentName = {
  &AmdXgbeGetDriverName,
  &AmdXgbeGetControllerName,
  "eng" // SupportedLanguages, ISO 639-2 language codes
};

EFI_COMPONENT_NAME2_PROTOCOL gAmdXgbeComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME)     &AmdXgbeGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME) &AmdXgbeGetControllerName,
  "en" // SupportedLanguages, RFC 4646 language codes
};

/**
  This is the declaration of an EFI image entry point. This entry point is the
  same for UEFI Applications, UEFI OS Loaders, and UEFI Drivers including both
  device drivers and bus drivers.

  @param  ImageHandle           The firmware allocated handle for the UEFI
                                image.
  @param  SystemTable           A pointer to the EFI System Table.

  @retval EFI_SUCCESS           The operation completed successfully.
  @retval Others                An unexpected error occurred.
**/

EFI_STATUS
EFIAPI
AmdXgbeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  return EfiLibInstallDriverBindingComponentName2 (
           ImageHandle,
           SystemTable,
           &gAmdXgbeDriverBinding,
           ImageHandle,
           &gAmdXgbeComponentName,
           &gAmdXgbeComponentName2
           );
}


EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS          Status;
  EFI_PCI_IO_PROTOCOL *PciIo;
  AMD_FABRIC_TOPOLOGY_SERVICES_PROTOCOL *FabricTopology;
  PCI_ADDR            PciAddress;
  UINT32              VidDid;
  UINTN               SegmentNumber;
  UINTN               BusNumber;
  UINTN               DeviceNumber;
  UINTN               FunctionNumber;
  UINTN               BusNumberBase;
  UINTN               SocketNum;
  UINTN               DieNum;
  UINT8               XgbeBusDie0;

  SocketNum = 0;
  DieNum = 0;

  Status = gBS->LocateProtocol (
                &gAmdFabricTopologyServicesProtocolGuid,
                NULL,
                &FabricTopology
                );
  if (FabricTopology->GetRootBridgeInfo (FabricTopology, SocketNum, DieNum, 0, NULL, &BusNumberBase, NULL) != EFI_SUCCESS) {
    Status = EFI_DEVICE_ERROR;
  }
  PciAddress.AddressValue = (MAKE_SBDFO (0, BusNumberBase, 0x8, FUNC_1, 0x19));
  XgbeBusDie0 = PciRead8 (PciAddress.AddressValue);

  Status = gBS->OpenProtocol (
    DeviceHandle,
    &gEfiPciIoProtocolGuid,
    (VOID **) &PciIo,
    This->DriverBindingHandle,
    DeviceHandle,
    EFI_OPEN_PROTOCOL_BY_DRIVER
    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = PciIo->Pci.Read (
    PciIo,
    EfiPciIoWidthUint32,
    PCI_VENDOR_ID_OFFSET,
    1,
    &VidDid
    );
  if (!EFI_ERROR (Status)) {
    //if (VidDid == 0x14591022) {
      if ((VidDid == 0x14591022) ||(VidDid == 0x14581022) || (VidDid == 0x14581D94) || (VidDid == 0x14591D94)) {
      PciIo->GetLocation (
                        PciIo,
                        &SegmentNumber,
                        &BusNumber,
                        &DeviceNumber,
                        &FunctionNumber
      );
      
      if (((FunctionNumber == 4) || (FunctionNumber == 5) || (FunctionNumber == 6) || (FunctionNumber == 7)) && (BusNumber == XgbeBusDie0)) {
        Status = EFI_SUCCESS;
        DEBUG ((DEBUG_ERROR, "PS-67: AmdXgbe SUPPORTED\n"));
        DEBUG ((DEBUG_ERROR, "PS-67: Segment = %x, BusNumber= %x, DeviceNumber = %x FunctionNumber = %x \n", SegmentNumber, BusNumber, DeviceNumber, FunctionNumber));
      } else {
        Status = EFI_UNSUPPORTED;
      }
    } else {
      Status = EFI_UNSUPPORTED;
    }
  }

  gBS->CloseProtocol (
    DeviceHandle,
    &gEfiPciIoProtocolGuid,
    This->DriverBindingHandle,
    DeviceHandle);

  return Status;
}

EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS          Status;
  VOID                *ChildPciIo;
  gControllerHandle = ControllerHandle;
  DEBUG ((DEBUG_ERROR, "\nPS-67: AmdXgbeDriverBindingStart()\n"));
  // get PCI Device Path
  Status = gBS->OpenProtocol (
          ControllerHandle,
          &gEfiDevicePathProtocolGuid,
          (VOID **) &PciDevicePath,
          This->DriverBindingHandle,
          ControllerHandle,
          EFI_OPEN_PROTOCOL_BY_DRIVER
          );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeStart: cannot open Device Path: Status: %r\n", Status));
    return Status;
  }

  //Status = AmdXgbeInit (); //PS-67----
  Status = AmdXgbeInit (ControllerHandle); //PS-67++++ for multi xgbe ports

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeStart: AmdXgbeInit failed: Status: %r\n", Status));
    goto ErrorExit1;
  }

  Status = gBS->OpenProtocol (ControllerHandle, &gEfiPciIoProtocolGuid,
                    &ChildPciIo, This->DriverBindingHandle,
                    gPrivate->DeviceHandle, EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER);

  if (!EFI_ERROR (Status)) {
    return Status;
  }

  ErrorExit1:
    gBS->CloseProtocol (
      ControllerHandle,
      &gEfiDevicePathProtocolGuid,
      This->DriverBindingHandle,
      ControllerHandle);
  return Status;
}

EFI_STATUS
STATIC
EFIAPI
AmdXgbeDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  )
{
  EFI_STATUS                        Status;
  EFI_PCI_IO_PROTOCOL               *PciIo;
  EFI_SIMPLE_NETWORK_PROTOCOL       *SimpleNetwork;
  BOOLEAN                           AllChildrenStopped;
  UINTN                             Index;

  Status = gBS->OpenProtocol (
          DeviceHandle,
          &gEfiPciIoProtocolGuid,
          (VOID **) &PciIo,
          This->DriverBindingHandle,
          DeviceHandle,
          EFI_OPEN_PROTOCOL_GET_PROTOCOL
          );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop OpenProtocol error: %r\n", Status));
  }

  if (gPrivate != NULL) {
    Status = EthStop (&gPrivate->Snp);
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop EthStop error: %r\n", Status));
  }

  AllChildrenStopped = TRUE;

  for (Index = 0; Index < NumberOfChildren; Index++) {
    Status = gBS->OpenProtocol (
                      ChildHandleBuffer[Index],
                      &gEfiSimpleNetworkProtocolGuid,
                      (VOID **) &SimpleNetwork,
                      This->DriverBindingHandle,
                      DeviceHandle,
                      EFI_OPEN_PROTOCOL_GET_PROTOCOL
                      );
    DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop Open SNP Protocol Status: %r\n", Status));

    if (!EFI_ERROR (Status)) {
      Status = gBS->UninstallMultipleProtocolInterfaces (
                        ChildHandleBuffer[Index],
                        &gEfiDevicePathProtocolGuid,
                        gPrivate->FullDevicePath,
                        &gEfiSimpleNetworkProtocolGuid,
                        &gPrivate->Snp,
                        NULL
                        );
      DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop UninstallMultipleProtocolInterfaces Status: %r\n", Status));
    }

    if (EFI_ERROR (Status)) {
      AllChildrenStopped = FALSE;
    }
  }

  if (!AllChildrenStopped) {
    return EFI_DEVICE_ERROR;
  }

    Status = gBS->CloseProtocol (
        DeviceHandle,
        &gEfiPciIoProtocolGuid,
        This->DriverBindingHandle,
        DeviceHandle
        );

  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "AmdXgbeDriverBindingStop ClosePciIoProtocol for DeviceHandle error: %r\n", Status));
    return Status;
  }

  if (gPrivate != NULL) {
    FreePool (gPrivate);
    gPrivate = NULL;
  }

  return Status;
}

EFI_STATUS
STATIC
EFIAPI
AmdXgbeGetDriverName (
  IN       EFI_COMPONENT_NAME_PROTOCOL *This,
  IN       CHAR8                       *Language,
  OUT       CHAR16                      **DriverName
  )
{
  return (Language == NULL || DriverName == NULL) ?
         EFI_INVALID_PARAMETER :
         LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mAmdXgbeDriverNameTable,
           DriverName,
           (BOOLEAN) (This == &gAmdXgbeComponentName) // Iso639Language
           );
}

EFI_STATUS
STATIC
EFIAPI
AmdXgbeGetControllerName (
  IN       EFI_COMPONENT_NAME_PROTOCOL *This,
  IN       EFI_HANDLE                  ControllerHandle,
  IN       EFI_HANDLE                  ChildHandle,
  IN       CHAR8                       *Language,
  OUT       CHAR16                     **ControllerName
  )
{

  if (ControllerHandle == NULL || Language == NULL || ControllerName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mAmdXgbeControllerNameTable,
           ControllerName,
           (BOOLEAN) (This == &gAmdXgbeComponentName) // Iso639Language
           );
}
