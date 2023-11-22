/* $NoKeywords */

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
* * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
*
* HYGON is granting you permission to use this software (the Materials)
* pursuant to the terms and conditions of your Software License Agreement
* with HYGON. This header does *NOT* give you permission to use the Materials
* or any rights under HYGON's intellectual property. Your use of any portion
* of these Materials shall constitute your acceptance of those terms and
* conditions. If you do not agree to the terms and conditions of the Software
* License Agreement, please do not use any portion of these Materials.
*
* CONFIDENTIALITY: The Materials and all other information, identified as
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
* THE POSSIBILITY OF SUCH DAMAGES. BECAUSE SOME JURISDICTIONS PROHIBIT THE
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
* available to you. Additionally, HYGON retains the right to modify the
* Materials at any time, without notice, and is not obligated to provide such
* modified Materials to you.
*
******************************************************************************
*/

#include <Library/UefiLib.h>
#include <Protocol/PciIo.h>
#include <Library/PciLib.h>
#include <IndustryStandard/Pci22.h>
#include "SnpDxe.h"
#include <Library/PrintLib.h>

#define HYGON_PCI_VENDOR_ID  0x1d94
#define XGBE_PCI_DEVICE_ID1  0x1458
#define XGBE_PCI_DEVICE_ID2  0x1459

EFI_STATUS
STATIC
EFIAPI
HygonXgbeDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

EFI_STATUS
STATIC
EFIAPI
HygonXgbeDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  );

EFI_STATUS
STATIC
EFIAPI
HygonXgbeDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  );

EFI_STATUS
STATIC
EFIAPI
HygonXgbeGetDriverName (
  IN  EFI_COMPONENT_NAME_PROTOCOL *This,
  IN  CHAR8                       *Language,
  OUT CHAR16                      **DriverName
  );

EFI_STATUS
STATIC
EFIAPI
HygonXgbeGetControllerName (
  IN  EFI_COMPONENT_NAME_PROTOCOL *This,
  IN  EFI_HANDLE                  ControllerHandle,
  IN  EFI_HANDLE                  ChildHandle,
  IN  CHAR8                       *Language,
  OUT CHAR16                      **ControllerName
  );

extern EFI_STATUS
EFIAPI
HygonXgbeInit (
  EFI_HANDLE                Controller,
  EFI_DEVICE_PATH_PROTOCOL  *PciDevicePath,
  ENET_DXE_PRIVATE_DATA     **PrivatePtr
  );

extern EFI_STATUS
EFIAPI
EthStop (
  IN EFI_SIMPLE_NETWORK_PROTOCOL *This
  );

EFI_DRIVER_BINDING_PROTOCOL  gHygonXgbeDriverBinding = {
  HygonXgbeDriverBindingSupported,
  HygonXgbeDriverBindingStart,
  HygonXgbeDriverBindingStop,
  0x10,
  NULL,
  NULL
};

STATIC
EFI_UNICODE_STRING_TABLE  mHygonXgbeDriverNameTable[] = {
  { "eng; en", L"HygonXgbe Network Driver" },
  { NULL,      NULL                        }
};

STATIC
EFI_UNICODE_STRING_TABLE  mHygonXgbeControllerNameTable[] = {
  { "eng; en", (L"HYGON XGBE B:XX D:XX F:XX") },
  { NULL,      NULL                           }
};

EFI_COMPONENT_NAME_PROTOCOL  gHygonXgbeComponentName = {
  &HygonXgbeGetDriverName,
  &HygonXgbeGetControllerName,
  "eng" // SupportedLanguages, ISO 639-2 language codes
};

EFI_COMPONENT_NAME2_PROTOCOL  gHygonXgbeComponentName2 = {
  (EFI_COMPONENT_NAME2_GET_DRIVER_NAME)&HygonXgbeGetDriverName,
  (EFI_COMPONENT_NAME2_GET_CONTROLLER_NAME)&HygonXgbeGetControllerName,
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
HygonXgbeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS  Status;

  Status =  EfiLibInstallDriverBindingComponentName2 (
              ImageHandle,
              SystemTable,
              &gHygonXgbeDriverBinding,
              ImageHandle,
              &gHygonXgbeComponentName,
              &gHygonXgbeComponentName2
              );
  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Install HygonXgbe Success\n"));
  } else {
    DEBUG ((EFI_D_ERROR, "Install HygonXgbe failed\n"));
  }

  return Status;
}

EFI_STATUS
STATIC
EFIAPI
HygonXgbeDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS           Status;
  EFI_PCI_IO_PROTOCOL  *PciIo;
  UINT32               VidDid;
  UINTN                SegmentNumber;
  UINTN                BusNumber;
  UINTN                DeviceNumber;
  UINTN                FunctionNumber;

  Status = gBS->OpenProtocol (
                  DeviceHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
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
    if (VidDid == ((XGBE_PCI_DEVICE_ID1 << 16) | HYGON_PCI_VENDOR_ID) ||
        VidDid ==  ((XGBE_PCI_DEVICE_ID2 << 16) | HYGON_PCI_VENDOR_ID)) {
      PciIo->GetLocation (
               PciIo,
               &SegmentNumber,
               &BusNumber,
               &DeviceNumber,
               &FunctionNumber
               );
      if ((FunctionNumber == 4) || (FunctionNumber == 5) || (FunctionNumber == 6) || (FunctionNumber == 7)) {
        Status = EFI_SUCCESS;
        DEBUG ((EFI_D_ERROR, "=Xgbe SUPPORTED\n"));
        DEBUG ((
          EFI_D_ERROR,
          "Segment = %x, BusNumber= %x, DeviceNumber = %x FunctionNumber = %x \n",
          SegmentNumber,
          BusNumber,
          DeviceNumber,
          FunctionNumber
          ));
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
         DeviceHandle
         );

  return Status;
}

EFI_STATUS
STATIC
EFIAPI
HygonXgbeDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  ControllerHandle,
  IN EFI_DEVICE_PATH_PROTOCOL    *RemainingDevicePath
  )
{
  EFI_STATUS                Status;
  VOID                      *ChildPciIo;
  ENET_DXE_PRIVATE_DATA     *Private;
  EFI_DEVICE_PATH_PROTOCOL  *PciDevicePath;

  // get PCI Device Path
  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&PciDevicePath,
                  This->DriverBindingHandle,
                  ControllerHandle,
                  EFI_OPEN_PROTOCOL_BY_DRIVER
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "HygonXgbeStart: cannot open Device Path: Status: %r\n", Status));
  }

  Status = HygonXgbeInit (ControllerHandle, PciDevicePath, &Private);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "HygonXgbeStart: HygonXgbeInit failed: Status: %r\n", Status));
    goto ErrorExit1;
  }

  Status = gBS->OpenProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  &ChildPciIo,
                  This->DriverBindingHandle,
                  Private->DeviceHandle,
                  EFI_OPEN_PROTOCOL_BY_CHILD_CONTROLLER
                  );

  if (!EFI_ERROR (Status)) {
    return Status;
  }

ErrorExit1:
  gBS->CloseProtocol (
         ControllerHandle,
         &gEfiDevicePathProtocolGuid,
         This->DriverBindingHandle,
         ControllerHandle
         );

  return Status;
}

EFI_STATUS
STATIC
EFIAPI
HygonXgbeDriverBindingStop (
  IN EFI_DRIVER_BINDING_PROTOCOL *This,
  IN EFI_HANDLE                  DeviceHandle,
  IN UINTN                       NumberOfChildren,
  IN EFI_HANDLE                  *ChildHandleBuffer
  )
{
  EFI_STATUS                   Status;
  EFI_PCI_IO_PROTOCOL          *PciIo;
  EFI_SIMPLE_NETWORK_PROTOCOL  *SimpleNetwork;
  BOOLEAN                      AllChildrenStopped;
  UINTN                        Index;
  ENET_DXE_PRIVATE_DATA        *Private;

  Status = gBS->OpenProtocol (
                  DeviceHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo,
                  This->DriverBindingHandle,
                  DeviceHandle,
                  EFI_OPEN_PROTOCOL_GET_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "XgbeDriverBindingStop OpenProtocol error: %r\n", Status));
  }

  DEBUG ((EFI_D_ERROR, "Xgbe stop, NumberOfChildren: %d\n", NumberOfChildren));

  AllChildrenStopped = TRUE;

  for (Index = 0; Index < NumberOfChildren; Index++) {
    Status = gBS->OpenProtocol (
                    ChildHandleBuffer[Index],
                    &gEfiSimpleNetworkProtocolGuid,
                    (VOID **)&SimpleNetwork,
                    This->DriverBindingHandle,
                    DeviceHandle,
                    EFI_OPEN_PROTOCOL_GET_PROTOCOL
                    );
    DEBUG ((
      EFI_D_ERROR,
      "XgbeDriverBindingStop Open SNP Protocol Status: %r SimpleNetwork:%p\n",
      Status,
      SimpleNetwork
      ));

    if (SimpleNetwork != NULL) {
      Status = EthStop (SimpleNetwork);
    }

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "XgbeDriverBindingStop EthStop error: %r\n", Status));
    }

    if (SimpleNetwork != NULL) {
      Private = ENET_DXE_PRIVATE_DATA_FROM_SNP_THIS (SimpleNetwork);
    }

    if (!EFI_ERROR (Status)) {
      Status = gBS->UninstallMultipleProtocolInterfaces (
                      ChildHandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      Private->FullDevicePath,
                      &gEfiSimpleNetworkProtocolGuid,
                      &Private->Snp,
                      NULL
                      );
      DEBUG ((EFI_D_ERROR, "HygonXgbeDriverBindingStop UninstallMultipleProtocolInterfaces Status: %r\n", Status));
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
    DEBUG ((EFI_D_ERROR, "HygonXgbeDriverBindingStop ClosePciIoProtocol for DeviceHandle error: %r\n", Status));
    return Status;
  }

  if (Private != NULL) {
    FreePool (Private);
    Private = NULL;
  }

  return Status;
}

EFI_STATUS
STATIC
EFIAPI
HygonXgbeGetDriverName (
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
           mHygonXgbeDriverNameTable,
           DriverName,
           (BOOLEAN)(This == &gHygonXgbeComponentName)  // Iso639Language
           );
}

EFI_STATUS
STATIC
EFIAPI
HygonXgbeGetControllerName (
  IN       EFI_COMPONENT_NAME_PROTOCOL *This,
  IN       EFI_HANDLE                  ControllerHandle,
  IN       EFI_HANDLE                  ChildHandle,
  IN       CHAR8                       *Language,
  OUT       CHAR16                     **ControllerName
  )
{
  EFI_STATUS           Status;
  EFI_PCI_IO_PROTOCOL  *PciIo;
  UINT32               VidDid;
  UINTN                SegmentNumber;
  UINTN                BusNumber;
  UINTN                DeviceNumber;
  UINTN                FunctionNumber;

  if (ControllerHandle == NULL || Language == NULL || ControllerName == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->HandleProtocol (
                  ControllerHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&PciIo
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
    if (VidDid == ((XGBE_PCI_DEVICE_ID1 << 16) | HYGON_PCI_VENDOR_ID) ||
        VidDid ==  ((XGBE_PCI_DEVICE_ID2 << 16) | HYGON_PCI_VENDOR_ID)) {
      PciIo->GetLocation (
               PciIo,
               &SegmentNumber,
               &BusNumber,
               &DeviceNumber,
               &FunctionNumber
               );

      UnicodeSPrint (
        mHygonXgbeControllerNameTable[0].UnicodeString,
        sizeof (L"HYGON XGBE B:XX D:XX F:XX"),
        L"HYGON XGBE B:%02d D:%02d F:%02d",
        BusNumber,
        DeviceNumber,
        FunctionNumber
        );
      if ((FunctionNumber == 4) || (FunctionNumber == 5) || (FunctionNumber == 6) || (FunctionNumber == 7)) {
        return LookupUnicodeString2 (
                 Language,
                 This->SupportedLanguages,
                 mHygonXgbeControllerNameTable,
                 ControllerName,
                 (BOOLEAN)(This == &gHygonXgbeComponentName) // Iso639Language
                 );
      }
    }
  }

  return LookupUnicodeString2 (
           Language,
           This->SupportedLanguages,
           mHygonXgbeControllerNameTable,
           ControllerName,
           (BOOLEAN)(This == &gHygonXgbeComponentName)  // Iso639Language
           );
}
