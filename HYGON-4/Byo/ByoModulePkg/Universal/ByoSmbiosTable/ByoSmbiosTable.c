/** @file

  Copyright (c) 2021, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

  File Name:
    ByoSmbiosTable.c

  Abstract:
    Update SMBIOS ByoTool Type.

  Revision History:

  TIME:
  $AUTHOR:
  $REVIEWERS:
  $SCOPE:
  $TECHNICAL:


  T.O.D.O

  $END

**/
#include <PiDxe.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/PrintLib.h>
#include <Protocol/Smbios.h>
#include <Protocol/PciIo.h>
#include <Protocol/PlatformUsbDeviceSupport.h>
#include <IndustryStandard/SmBios.h>
#include <ByoSmbiosTable.h>
#include <Protocol/ByoPcdAccess.h>
#include <ByoDevicePatch.h>
#include <Library/ByoCommLib.h>
#include <Guid/SmmVariableCommon.h>
#include <Protocol/SetupItemUpdate.h>


EFI_STATUS
ParseAllFfs (
 );

EFI_STATUS
AddDefaultVariableTable(
  EFI_SMBIOS_PROTOCOL                  *SmbiosProtocol,
  EFI_HANDLE                           ImageHandle
);

extern BYO_PCD_ACCESS_PROTOCOL mPcdAccessProtocol;
EFI_PHYSICAL_ADDRESS           IfrDataAddress;
extern UINTN                   TempIfrTotalDataSize;

VOID
FillUsbDevicePortInfo (
 IN SMBIOS_USB_DEVICE_SUPPORT_TABLE_TYPE251  *UsbDeviceTable251,
 IN EFI_DEVICE_PATH_PROTOCOL                 *DevicePath
 )
{
  EFI_STATUS                Status;
  EFI_HANDLE                DeviceHandle;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  UINTN                     SegmentNumber;
  UINTN                     BusNumber;
  UINTN                     DeviceNumber;
  UINTN                     FunctionNumber;
  EFI_DEVICE_PATH_PROTOCOL  *Next;
  USB_DEVICE_PATH           *UsbDevicepath;
  SOC_MMIO_DEVICE_PATH      *SocMmioDevPath;
  CHAR8                     *UsbPortStr;
  CHAR8                     *CurrStr;
  
  UsbPortStr = (CHAR8 *)&UsbDeviceTable251->PortAddressStr[0];
  ZeroMem (UsbPortStr, 16);

  if (DevicePath == NULL) {
    if (UsbDeviceTable251->SupportProperty.UsbHostControllerType == USB_CONTROLLER_TYPE_MMIO) {
      UsbDeviceTable251->ControllerInfo.MmioBaseAddress = 0xFFFFFFFFFFFFFFFF;
    } else {
      UsbDeviceTable251->ControllerInfo.PciController.Seg = 0xFFFFFFFF;
      UsbDeviceTable251->ControllerInfo.PciController.Bus = 0xFFFF;
      UsbDeviceTable251->ControllerInfo.PciController.Dev = 0xFF;
      UsbDeviceTable251->ControllerInfo.PciController.Func = 0xFF;
    }
  } else if (DevicePath->Type == HARDWARE_DEVICE_PATH) {
    if (PcdGet16(PcdSmbiosType251Version) == SMBIOS_USB_SUPPORT_DEVICE_TABLE_VERISON_0001) {
      DEBUG((EFI_D_INFO,"SmbiosType251 cannot be set under this version\n"));
      ASSERT(FALSE);
    }
    CurrStr = UsbPortStr;
    Next    = DevicePath;
    while (!IsDevicePathEnd (Next)) {
      if (Next->Type == HARDWARE_DEVICE_PATH && Next->SubType == HW_VENDOR_DP) {
        SocMmioDevPath = (SOC_MMIO_DEVICE_PATH *)Next;
        UsbDeviceTable251->ControllerInfo.MmioBaseAddress = SocMmioDevPath->MmioBaseAddress;
        DEBUG((DEBUG_INFO,"SocMmioDevPath->MmioBaseAddress [0x%x]\n",SocMmioDevPath->MmioBaseAddress));
      }
      if (Next->Type == MESSAGING_DEVICE_PATH && Next->SubType == MSG_USB_DP) {
        UsbDevicepath = (USB_DEVICE_PATH *)Next;
        if (CurrStr != (CHAR8 *)&UsbDeviceTable251->PortAddressStr[0]) {
          CurrStr[0] = '.';
          CurrStr++;
        }
        //
        // USB Port Number in device path starts from 0, while USB spec declares it is from 1. 
        // So, +1 for SMBIOS table for OS.
        //
        DEBUG((DEBUG_INFO,"PortNumber %d",UsbDevicepath->ParentPortNumber));
        AsciiSPrint((CHAR8 *)CurrStr, 4, "%02X", (UsbDevicepath->ParentPortNumber + 1));
        CurrStr += 2;
        // Make sure the port string length is less than 15
        ASSERT ((CurrStr - UsbPortStr) < (16 - 3 - 1)); 
      }
      Next = NextDevicePathNode (Next);
    }
    DEBUG ((DEBUG_INFO, "\nUsbPortStr[%a]\n", UsbPortStr));
  } else {
    Status = gBS->LocateDevicePath (&gEfiPciIoProtocolGuid, &DevicePath, &DeviceHandle);
    ASSERT_EFI_ERROR (Status);
    Status = gBS->HandleProtocol (DeviceHandle, &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
    ASSERT_EFI_ERROR (Status);
    Status = PciIo->GetLocation (PciIo, &SegmentNumber, &BusNumber, &DeviceNumber, &FunctionNumber);
    ASSERT_EFI_ERROR (Status);
    DEBUG ((DEBUG_INFO, "SegmentNumber[%x] BusNumber[%x] DeviceNumber[%x] FunctionNumber[%x] \n", SegmentNumber, BusNumber, DeviceNumber, FunctionNumber));
    UsbDeviceTable251->ControllerInfo.PciController.Seg = (UINT32)SegmentNumber;
    UsbDeviceTable251->ControllerInfo.PciController.Bus = (UINT16)BusNumber;
    UsbDeviceTable251->ControllerInfo.PciController.Dev = (UINT8)DeviceNumber;
    UsbDeviceTable251->ControllerInfo.PciController.Func = (UINT8)FunctionNumber;

    CurrStr = UsbPortStr;
    Next    = DevicePath;
    while (!IsDevicePathEnd (Next)) {
      if (Next->Type == MESSAGING_DEVICE_PATH && Next->SubType == MSG_USB_DP) {
        UsbDevicepath = (USB_DEVICE_PATH *)Next;
        if (CurrStr != (CHAR8 *)&UsbDeviceTable251->PortAddressStr[0]) {
          CurrStr[0] = '.';
          CurrStr++;
        }
        //
        // USB Port Number in device path starts from 0, while USB spec declares it is from 1. 
        // So, +1 for SMBIOS table for OS.
        //
        AsciiSPrint((CHAR8 *)CurrStr, 4, "%02X", (UsbDevicepath->ParentPortNumber + 1));
        CurrStr += 2;
        // Make sure the port string length is less than 15
        ASSERT ((CurrStr - UsbPortStr) < (16 - 3 - 1)); 
      }
      Next = NextDevicePathNode (Next);
    }
    DEBUG ((DEBUG_INFO, "UsbPortStr[%a]\n", UsbPortStr));
    //AsciiSPrint(UsbPortStr, 3, "XX");
  }
}

VOID
EFIAPI
AddUsbDeviceSupportSmbiosTable251CallBack (
  VOID
  )
{
  EFI_STATUS                               Status;
  PLATFORM_USB_DEVICE_SUPPORT_TABLE        *PlatformUsbDeviceSupportTable = NULL;
  EFI_SMBIOS_HANDLE                        SmbiosHandle;
  EFI_SMBIOS_PROTOCOL                      *SmbiosProtocol;
  UINT32                                   RuleIndex;
  SMBIOS_USB_DEVICE_SUPPORT_TABLE_TYPE251  UsbDeviceTable251;

  Status = gBS->LocateProtocol (
               &gPlatformUsbDeviceSupportProtocolGuid,
               NULL,
               (VOID **) &PlatformUsbDeviceSupportTable
               );
  if (EFI_ERROR (Status) || PlatformUsbDeviceSupportTable == NULL) {
    return;
  }

  DEBUG ((DEBUG_INFO, "PlatformUsbDeviceSupportTable=%p\n", PlatformUsbDeviceSupportTable));
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }

  ZeroMem(&UsbDeviceTable251, sizeof(SMBIOS_USB_DEVICE_SUPPORT_TABLE_TYPE251));
  UsbDeviceTable251.Hdr.Type    = SMBIOS_USB_SUPPORT_DEVICE_TYPE;
  UsbDeviceTable251.Hdr.Length  = sizeof (SMBIOS_USB_DEVICE_SUPPORT_TABLE_TYPE251) - 2;
  UsbDeviceTable251.Hdr.Handle  = 0;
  UsbDeviceTable251.Signature   = SMBIOS_USB_SUPPORT_DEVICE_TABLE_SIGNATURE;
  if (PcdGet16(PcdSmbiosType251Version) != 0) {
    UsbDeviceTable251.Revision  = PcdGet16(PcdSmbiosType251Version);
  } else {
    UsbDeviceTable251.Revision  = SMBIOS_USB_SUPPORT_DEVICE_TABLE_VERISON_0002;
  }

  for (RuleIndex = 0; RuleIndex < PlatformUsbDeviceSupportTable->Header.RuleCount; RuleIndex++) {
    UsbDeviceTable251.Priority         = PlatformUsbDeviceSupportTable->Rules[RuleIndex].Priority;
    UsbDeviceTable251.IdVendor         = PlatformUsbDeviceSupportTable->Rules[RuleIndex].IdVendor;
    UsbDeviceTable251.IdProduct        = PlatformUsbDeviceSupportTable->Rules[RuleIndex].IdProduct;
    UsbDeviceTable251.DeviceClass      = PlatformUsbDeviceSupportTable->Rules[RuleIndex].DeviceClass;
    UsbDeviceTable251.DeviceSubClass   = PlatformUsbDeviceSupportTable->Rules[RuleIndex].DeviceSubClass;
    UsbDeviceTable251.DeviceProtocol   = PlatformUsbDeviceSupportTable->Rules[RuleIndex].DeviceProtocol;

    UsbDeviceTable251.SupportProperty.Support               = PlatformUsbDeviceSupportTable->Rules[RuleIndex].SupportProperty.Bits.Support;
    UsbDeviceTable251.SupportProperty.StorageReadSupport    = PlatformUsbDeviceSupportTable->Rules[RuleIndex].SupportProperty.Bits.StorageReadSupport;
    UsbDeviceTable251.SupportProperty.StorageWriteSupport   = PlatformUsbDeviceSupportTable->Rules[RuleIndex].SupportProperty.Bits.StorageWriteSupport;
    UsbDeviceTable251.SupportProperty.StorageExecuteSupport = PlatformUsbDeviceSupportTable->Rules[RuleIndex].SupportProperty.Bits.StorageExecuteSupport;
    UsbDeviceTable251.SupportProperty.UsbHostControllerType = PlatformUsbDeviceSupportTable->Rules[RuleIndex].SupportProperty.Bits.UsbHostControllerType;
    UsbDeviceTable251.SupportProperty.Reserved              = 0;

    FillUsbDevicePortInfo (&UsbDeviceTable251, PlatformUsbDeviceSupportTable->Rules[RuleIndex].DevicePath);
  
    DEBUG ((DEBUG_INFO, "UsbDeviceTable251=%p\n", &UsbDeviceTable251));

    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = SmbiosProtocol->Add (SmbiosProtocol, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) &UsbDeviceTable251);
    ASSERT_EFI_ERROR (Status);
  }
}

EFI_STATUS
EFIAPI
ByoSmbiosTableEntryPoint (
  IN EFI_HANDLE            ImageHandle,
  IN EFI_SYSTEM_TABLE*      SystemTable
  )
{
  EFI_SMBIOS_PROTOCOL           *SmbiosProtocol;
  EFI_SMBIOS_HANDLE             SmbiosHandle;
  SMBIOS_BYOTOOL_TABLE          TypeRecord;
  EFI_STATUS                    Status;
  SMBIOS_IFRDATA_TABLE         IfrDataTable;

  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &SmbiosProtocol);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (&TypeRecord, sizeof (TypeRecord));
  TypeRecord.Hdr.Type    = SMBIOS_BYOTOOL_TYPE;
  TypeRecord.Hdr.Length  = sizeof (SMBIOS_BYOTOOL_TABLE) - 2;
  TypeRecord.Hdr.Handle  = 0;
  TypeRecord.Signature   = SMBIOS_BYOTOOL_TABLE_SIGNATURE;
  TypeRecord.SwSmiPort   = (UINT32) PcdGet16 (PcdSwSmiCmdPort);
  TypeRecord.MemorySize  = RESERVED_BYOTOOL_MEMORY_SIZE;
  TypeRecord.Version.TableVersion = SMBIOS_BYOTOOL_TABLE_VERISON;
  TypeRecord.Version.VendorType   = PcdGet8 (PcdSmbiosByoToolVendorType);
  TypeRecord.Version.ByoFlashVersion = BYOFLASH_VERSION;
  TypeRecord.Version.ByoDmiVersion   = BYODMI_VERSION;
  TypeRecord.Version.ByoCfgVersion   = BYOCFG_VERSION;

  TypeRecord.MemoryAddress = (UINT32)LibGetSmiInfoBufferAddr(gBS);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosProtocol->Add (SmbiosProtocol, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) &TypeRecord);

  if (PcdGetBool(PcdSmbiosTableAddSuppressIf) == TRUE){
    DEBUG ((DEBUG_INFO, "PcdSmbiosTableAddSuppressIf add begin\n"));
    IfrDataTable.Hdr.Type = SMBIOS_IFRDATA_TYPE;
    IfrDataTable.Hdr.Length  = sizeof(SMBIOS_IFRDATA_TABLE);
    IfrDataTable.Hdr.Handle  = 0;
    ParseAllFfs ();
    IfrDataTable.Signature   = SMBIOS_IFRDATA_TABLE_SIGNATURE;
    IfrDataTable.IfrAddress = IfrDataAddress;
    IfrDataTable.IfrDataLength = TempIfrTotalDataSize;
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = SmbiosProtocol->Add (SmbiosProtocol, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*) &IfrDataTable);
    DEBUG ((DEBUG_INFO, "PcdSmbiosTableAddSuppressIf result:%llx;IfrDataLength:%llx;IfrDataAddress:%p\n", Status,TempIfrTotalDataSize,IfrDataAddress));
  }

  Status = AddDefaultVariableTable(SmbiosProtocol, ImageHandle);
  if (EFI_ERROR (Status)){
    DEBUG ((DEBUG_INFO, "DefaultVariableDataTable add skip\n"));
  }
  
  //
  // Create one callback function to Add SMBIOS 251 table
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiAfterConnectPciRootBridgeGuid, AddUsbDeviceSupportSmbiosTable251CallBack,
                  &gByoPcdAccessProtocolGuid, &mPcdAccessProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}
