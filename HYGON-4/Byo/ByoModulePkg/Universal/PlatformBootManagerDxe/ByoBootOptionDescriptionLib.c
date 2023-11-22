/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ByoBootManagerLib.c

Abstract:

Revision History:

**/
#include "ByoBootOptionDescriptionLib.h"

#include <Guid/FileInfo.h>
#include <IndustryStandard/Pci22.h>

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ByoBootManagerLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/DebugLib.h>
#include <Library/ByoCommLib.h>
#include <Library/UefiBootManagerLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/DiskInfo.h>
#include <Protocol/IdeControllerInit.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/OpRomPnpName.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <Protocol/ScsiPassThru.h>
#include <Protocol/BlockIo.h>
#include <Protocol/NetworkInterfaceIdentifier.h>

#include <IndustryStandard/PciCodeId.h>
#include <IndustryStandard/Scsi.h>
#include <SysMiscCfg.h>

#define VENDOR_IDENTIFICATION_OFFSET     3
#define VENDOR_IDENTIFICATION_LENGTH     8
#define PRODUCT_IDENTIFICATION_OFFSET    11
#define PRODUCT_IDENTIFICATION_LENGTH    16

typedef
CHAR16 *
(* GET_BOOT_DESCRIPTION) (
  IN EFI_HANDLE          Handle
  );


/**
  Try to get the controller's USB description.

  @param Handle                Controller handle.

  @return  The description string.
**/
CHAR16 *
GetUsbDescription (
  IN EFI_HANDLE                Handle
  )
{
  EFI_STATUS                   Status;
  CHAR16                       *Description;
  CHAR8                        ModelNumber[50];
  USB_BOOT_INQUIRY_DATA        UsbInquiryData;  
  UINT32                       BufferSize = 0;
  EFI_DISK_INFO_PROTOCOL       *DiskInfo;
  UINTN                        Index;
  

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDiskInfoProtocolGuid,
                  (VOID **)&DiskInfo
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }             

  if (!CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoUsbInterfaceGuid)) {
    return NULL;
  }		
	
  BufferSize = sizeof(USB_BOOT_INQUIRY_DATA);  
  Status = DiskInfo->Inquiry(
                     DiskInfo,
                     &UsbInquiryData,
                     &BufferSize
                     );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  
  ZeroMem (ModelNumber, sizeof (ModelNumber));
  CopyMem (ModelNumber, UsbInquiryData.VendorID, 8);
  ModelNumber[8] = 0;
  Index = AsciiStrLen(ModelNumber);
  ModelNumber[Index++] = ' ';

  CopyMem (&ModelNumber[Index], UsbInquiryData.ProductID, 16);
  ModelNumber[Index+16] = 0;

  Description = AllocatePool(64);
  UnicodeSPrint (Description, 64, L"USB: %a", ModelNumber);
  DeleteExtraSpaces (Description);

  return Description;
}

STATIC
CHAR16 *
PlatformGetDescFromPci (
  EFI_HANDLE Handle,
  EFI_DEVICE_PATH_PROTOCOL *ptDevPath
  )
{
  EFI_PCI_IO_PROTOCOL            *ptPciIo;
  EFI_HANDLE                     PciHandle;
  UINT8                          ClassCode[3];
  EFI_DEVICE_PATH_PROTOCOL       *ptDP;
  CHAR16                         *DevName;
  EFI_STATUS                     Status;
  CHAR16                         *Name = NULL;
  OPROM_PNP_DEV_NAME             *PnpName;
  UINTN                          MyStrSize;
  BYO_DISKINFO_PROTOCOL          *ByoDiskInfo;
  CHAR8                          Mn[41];
  UINTN                          MnSize;
  UINTN                          Seg,Bus,Dev,Func;

  ZeroMem(Mn,sizeof(Mn));
  ptDP = ptDevPath;
  Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &ptDP,
                  &PciHandle
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }

  Status = gBS->HandleProtocol(
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&ptPciIo
                  );
  ASSERT(!EFI_ERROR(Status));
  ptPciIo->GetLocation(ptPciIo, &Seg, &Bus, &Dev, &Func);
  DEBUG((EFI_D_INFO, "(%X,%X,%X) %X\n", (UINT32) Bus, (UINT32) Dev, (UINT32) Func, (UINT32) Seg));

  Status = ptPciIo->Pci.Read(
                          ptPciIo,
                          EfiPciIoWidthUint8,  
                          PCI_CLASSCODE_OFFSET,
                          3,
                          ClassCode
                          );
  if (ClassCode[2] == 1 && ClassCode[1] == 8 && ClassCode[0] == 2) { // NVMe
    DevName = LibOpalDriverGetDriverDeviceName(gBS, Handle);
    if(DevName == NULL){ // Didn't get the devname from gEfiComponentName2ProtocolGuid
      //
      //get the devname from gByoDiskInfoProtocolGuid
      //
      Status = gBS->HandleProtocol (
                    Handle,
                    &gByoDiskInfoProtocolGuid,
                    (VOID **)&ByoDiskInfo
                    );
      if (EFI_ERROR (Status)) {
        DEBUG((EFI_D_INFO,"locate byodiskinfo protocol is Error\n"));
        Name = L"NVME";
        Name = AllocateCopyPool(StrSize(Name), Name);
        goto ProcExit;
      } else {
        MnSize = sizeof(Mn);
        Status = ByoDiskInfo->GetMn(ByoDiskInfo,Mn,&MnSize);
        if (!EFI_ERROR(Status)) {
          DevName = AllocateZeroPool(MnSize * 2);
          AsciiStrToUnicodeStrS(Mn,DevName,AsciiStrLen(Mn) + 1);
        } else {
          Name = L"NVME";
          Name = AllocateCopyPool(StrSize(Name), Name);
          goto ProcExit;
        }
      }
    }
    MyStrSize = StrSize(DevName) + 50;
    Name = AllocateZeroPool(MyStrSize);
    UnicodeSPrint(Name, MyStrSize, L"NVME(PCI%x-%x-%x-%x): %s", (UINT16) Seg, (UINT8) Bus, (UINT8) Dev, (UINT8) Func, DevName);
    FreePool(DevName);
  } else {
    Status = gBS->HandleProtocol(
                    Handle,
                    &gOpromPnpNameProtocolGuid,
                    (VOID **)&PnpName
                    );
    if(!EFI_ERROR(Status)){
      Name = AllocateCopyPool(StrSize(PnpName->Name), PnpName->Name);
    } else {
      Name = NULL;
    }
  }

ProcExit:
  return Name;
}

/**
  Try to get the description of controller with gEfiDiskInfoProtocolGuid

  @param Handle                Controller handle.

  @return  The description string.
**/
CHAR16 *
GetDiskInfoDescription (
  IN EFI_HANDLE                Handle
  )
{
  UINTN                        Index;
  EFI_STATUS                   Status;
  EFI_DISK_INFO_PROTOCOL       *DiskInfo;
  UINT32                       BufferSize;
  EFI_ATAPI_IDENTIFY_DATA      IdentifyData;
  CHAR16                       *Description = NULL;
  UINTN                        Length;
  CONST UINTN                  ModelNameLength = 40;
  CHAR16                       *NewString = NULL;
  CHAR8                        *StrPtr;
  UINT8                        Temp;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;
  EFI_SCSI_INQUIRY_DATA        InquiryData;
  CHAR16                       *SataPrefix = NULL;
  UINTN                        Size;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDiskInfoProtocolGuid,
                  (VOID **) &DiskInfo
                  );
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  //
  // customize NVME and SD
  //
  DevicePath = DevicePathFromHandle(Handle);
  if (BootTypeFromDevicePath(DevicePath) == UnsupportedBoot) {
    NewString = PlatformGetDescFromPci(Handle, DevicePath);
    if (NewString != NULL) {
      Description = AllocateCopyPool(StrSize(NewString), NewString);
      FreePool(NewString);
      return Description;
    }
  }

  if (CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid)
    || CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid)) {
    BufferSize   = sizeof (EFI_ATAPI_IDENTIFY_DATA);
    Status = DiskInfo->Identify (
                         DiskInfo,
                         &IdentifyData,
                         &BufferSize
                         );
    if (!EFI_ERROR (Status)) {
      Description = AllocateZeroPool ((ModelNameLength + 1) * sizeof (CHAR16));
      ASSERT (Description != NULL);
      for (Index = 0; Index + 1 < ModelNameLength; Index += 2) {
        Description[Index]     = (CHAR16) IdentifyData.ModelName[Index + 1];
        Description[Index + 1] = (CHAR16) IdentifyData.ModelName[Index];
      }

      Length = Index;
      Description[Length++] = L'\0';
      ASSERT (Length == ModelNameLength + 1);
    }
  } else if (CompareGuid (&DiskInfo->Interface, &gEfiDiskInfoScsiInterfaceGuid)) {
    BufferSize   = sizeof (EFI_SCSI_INQUIRY_DATA);
    Status = DiskInfo->Inquiry (
                         DiskInfo,
                         &InquiryData,
                         &BufferSize
                         );
    if (!EFI_ERROR (Status)) {
      Description = AllocateZeroPool ((VENDOR_IDENTIFICATION_LENGTH + PRODUCT_IDENTIFICATION_LENGTH + 2) * sizeof (CHAR16));
      ASSERT (Description != NULL);

      //
      // Per SCSI spec, EFI_SCSI_INQUIRY_DATA.Reserved_5_95[3 - 10] save the Vendor identification
      // EFI_SCSI_INQUIRY_DATA.Reserved_5_95[11 - 26] save the product identification,
      // Here combine the vendor identification and product identification to the description.
      //
      StrPtr = (CHAR8 *) (&InquiryData.Reserved_5_95[VENDOR_IDENTIFICATION_OFFSET]);
      Temp = StrPtr[VENDOR_IDENTIFICATION_LENGTH];
      StrPtr[VENDOR_IDENTIFICATION_LENGTH] = '\0';
      AsciiStrToUnicodeStrS (StrPtr, Description, VENDOR_IDENTIFICATION_LENGTH + 1);
      StrPtr[VENDOR_IDENTIFICATION_LENGTH] = Temp;

      //
      // Add one space at the middle of vendor information and product information.
      //
      Description[VENDOR_IDENTIFICATION_LENGTH] = L' ';

      StrPtr = (CHAR8 *) (&InquiryData.Reserved_5_95[PRODUCT_IDENTIFICATION_OFFSET]);
      StrPtr[PRODUCT_IDENTIFICATION_LENGTH] = '\0';
      AsciiStrToUnicodeStrS (StrPtr, Description + VENDOR_IDENTIFICATION_LENGTH + 1, PRODUCT_IDENTIFICATION_LENGTH + 1);
    }
  }

  if (Description != NULL) {
    DeleteExtraSpaces (Description);
    SataPrefix = LibGetSataPrefix(gBS, DevicePathFromHandle(Handle));
    if (SataPrefix == NULL) {
      return NULL;
    }

    Size = (StrLen(SataPrefix) + StrLen(Description) + 3) * sizeof(CHAR16); // sata xxxxx: xxxx
    NewString = (CHAR16 *)AllocateZeroPool(Size);
    ASSERT(NewString != NULL);
    UnicodeSPrint(NewString, Size, L"%s: %s", SataPrefix, Description);
    FreePool(Description);
    FreePool(SataPrefix);
    Description = NewString;
  }

  return Description;
}

//
// DP:PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)/SAS(0xC5FAEC803954B66,0x0,0x1,SAS,External,Expanded,0,0x0)
//
// DP:PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)/SAS(0x500E004AAAAAAA03,0x0,0x1,SATA,External,Expanded,0,0x0)
//                                        DP:SAS(0x500E004AAAAAAA03,0x0,0x1,SATA,External,Expanded,0,0x0)
// DP:PciRoot(0x3)/Pci(0x3,0x2)/Pci(0x0,0x0)/SAS(0x500E004AAAAAAA02,0x0,0x1,SATA,External,Expanded,0,0x0)
//                                        DP:SAS(0x500E004AAAAAAA02,0x0,0x1,SATA,External,Expanded,0,0x0)
CHAR16 *
GetSasDescription (
  IN EFI_HANDLE                Handle
  )
{
  EFI_DEVICE_PATH_PROTOCOL       *Dp;
  EFI_DEVICE_PATH_PROTOCOL       *Node;
  EFI_DEVICE_PATH_PROTOCOL       *Temp;
  EFI_DEVICE_PATH_PROTOCOL       *PortDp;
  SAS_DEVICE_PATH                *SasDp;
  UINT16                         Info;
  CHAR16                         *p;
  EFI_STATUS                     Status;
  EFI_HANDLE                     SasHandle;
  EFI_ATA_PASS_THRU_PROTOCOL     *AtaPassThru;
  UINT16                         Port;
  UINT16                         PortMp; 
  UINT16                         SavedPort = 0xFFFF;
  UINT16                         SavedPortMp = 0xFFFF;   
  BOOLEAN                        PortFound;
  ATA_IDENTIFY_DATA              *IdentifyData;
  CHAR8                          ModelName[40 + 1];
  CHAR16                         Buffer[64];

  Dp = DevicePathFromHandle(Handle);

  for (Node = Dp; !IsDevicePathEndType(Node); Node = NextDevicePathNode(Node)) {
    if(DevicePathType(Node) == MESSAGING_DEVICE_PATH && DevicePathSubType(Node) == MSG_VENDOR_DP) {  
      SasDp = (SAS_DEVICE_PATH*)Node;
      if(!CompareGuid(&SasDp->Guid, &gEfiSasDevicePathGuid)){
        continue;
      }
      Info = SasDp->DeviceTopology;
      if ((Info & 0x0f) <= 2 && (Info & BIT7) == 0 && (Info & BIT4)){
        Temp = Dp;
        Status = gBS->LocateDevicePath(&gEfiAtaPassThruProtocolGuid, &Temp, &SasHandle);
        if(!EFI_ERROR(Status)){
          Status = gBS->HandleProtocol (
                          SasHandle,
                          &gEfiAtaPassThruProtocolGuid,
                          (VOID*)&AtaPassThru
                          );
          
          PortFound = FALSE;
          Port = 0xFFFF;
          while (TRUE) {
            Status = AtaPassThru->GetNextPort (AtaPassThru, &Port);
            if (EFI_ERROR (Status)) {
              break;
            }

            PortMp = 0xFFFF;
            while (TRUE) {
              Status = AtaPassThru->GetNextDevice(AtaPassThru, Port, &PortMp);
              if (EFI_ERROR (Status)) {
                break;
              }
            }

            if(!PortFound){
              Status = AtaPassThru->BuildDevicePath(AtaPassThru, Port, PortMp, &PortDp);
              if(!EFI_ERROR (Status)){
                if(CompareMem(PortDp, Temp, GetDevicePathSize(PortDp)) == 0){
                  SavedPort = Port;
                  SavedPortMp = PortMp;
                  PortFound = TRUE;
                }
                FreePool(PortDp);
              }
            }
          }
          
          if(PortFound){

            Port   = SavedPort;
            PortMp = SavedPortMp;
            DEBUG((EFI_D_INFO, "SAS Found Port(%d,%d)\n", Port, PortMp));
            
            IdentifyData = AllocateBsZeroMemoryBelow4G(gBS, sizeof(ATA_IDENTIFY_DATA));
            ASSERT(IdentifyData != NULL);
            Status = AtaReadIdentifyData(AtaPassThru, Port, PortMp, IdentifyData);
            if(!EFI_ERROR(Status)){
              CopyMem(ModelName, IdentifyData->ModelName, sizeof(IdentifyData->ModelName));
              SwapWordArray((UINT8 *)ModelName, 40);
              ModelName[40] = 0;
              TrimStr8(ModelName);
              UnicodeSPrint(Buffer, sizeof(Buffer), L"SAS: %a", ModelName);
              p = AllocateCopyPool(StrSize(Buffer), Buffer);
              FreePages(IdentifyData, EFI_SIZE_TO_PAGES(sizeof(ATA_IDENTIFY_DATA)));              
              return p;              
            }
            FreePages(IdentifyData, EFI_SIZE_TO_PAGES(sizeof(ATA_IDENTIFY_DATA)));
          }
        }
      }
      
      UnicodeSPrint(Buffer, sizeof(Buffer), L"SAS(0x%lx,0x%lx,0x%x)", SasDp->SasAddress, SasDp->Lun, SasDp->RelativeTargetPort);
      p = AllocateCopyPool(StrSize(Buffer), Buffer);
      return p;
    }
  }

  return NULL;
}

CHAR16 *
GetRaidDescription (
  IN EFI_HANDLE                  Handle
  )
{
  EFI_PCI_IO_PROTOCOL            *ptPciIo;
  EFI_HANDLE                     PciHandle;
  EFI_HANDLE                     ScsiHandle;  
  UINT8                          ClassCode[3];
  EFI_DEVICE_PATH_PROTOCOL       *ptDP;
  EFI_STATUS                     Status;
  CHAR16                         *Name = NULL;  
  OPROM_PNP_DEV_NAME             *PnpName;
  UINTN                          MyStrSize;
  EFI_DEVICE_PATH_PROTOCOL       *ptDevPath;
  UINT16                          VendorId;
  EFI_SCSI_PASS_THRU_PROTOCOL     *ScsiPassThru;
  UINT16                          Pun = 0xFF;
  UINT16                          Lun = 0xFF;
  UINT32                         ControllerNum = 0;
  UINTN                           Seg,Bus,Dev,Func;

  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  ptDevPath = DevicePathFromHandle(Handle);
  ptDP = ptDevPath;
  Status = gBS->LocateDevicePath (
                  &gEfiPciIoProtocolGuid,
                  &ptDP,
                  &PciHandle
                  );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
      
  Status = gBS->HandleProtocol(
                  PciHandle,
                  &gEfiPciIoProtocolGuid,
                  (VOID **)&ptPciIo
                  );
  ASSERT(!EFI_ERROR(Status));
      
  Status = ptPciIo->Pci.Read(
                          ptPciIo, 
                          EfiPciIoWidthUint8,  
                          PCI_CLASSCODE_OFFSET, 
                          3, 
                          ClassCode
                          );
  if(ClassCode[2] == PCI_CLASS_MASS_STORAGE && (ClassCode[1] == PCI_CLASS_MASS_STORAGE_RAID
    || ClassCode[1] == PCI_CLASS_MASS_STORAGE_SAS || ClassCode[1] == PCI_CLASS_MASS_STORAGE_OTHER)){
    ptPciIo->Pci.Read (
               ptPciIo,
               EfiPciIoWidthUint32,
               0,
               1,
               &VendorId
               );
      DEBUG((EFI_D_ERROR," EfiPciIoProtocolGuid get bus dev"));
      ptPciIo->GetLocation(ptPciIo, &Seg, &Bus, &Dev, &Func);
      DEBUG((EFI_D_INFO, "(%X,%X,%X) %X\n", Bus, Dev, Func, Seg));

      if(VendorId == 0x1000){  //AVAGO LSI MEAGRAID
           ptDP = ptDevPath;
           ShowDevicePathDxe(gBS, ptDP);
           Status = gBS->LocateDevicePath (
                            &gEfiScsiPassThruProtocolGuid,
                            &ptDP,
                            &ScsiHandle
                            );
           if(!EFI_ERROR(Status)){
              Status = gBS->HandleProtocol(
                ScsiHandle,
                &gEfiScsiPassThruProtocolGuid,
                (VOID **)&ScsiPassThru
                );
              if(!EFI_ERROR(Status)){
                Pun = ((SCSI_DEVICE_PATH *)ptDP)->Pun;
                for (ptDP = ptDevPath; !IsDevicePathEndType (ptDP); ptDP = NextDevicePathNode (ptDP)) {
                     if(DevicePathType (ptDP) == HARDWARE_DEVICE_PATH && DevicePathSubType(ptDP) == HW_CONTROLLER_DP){
                        ControllerNum = ((CONTROLLER_DEVICE_PATH *)ptDP)->ControllerNumber;
                        break;
                     }
                }
                MyStrSize = StrSize(ScsiPassThru->Mode->ControllerName) + StrSize(ScsiPassThru->Mode->ChannelName) + 68;
                Name = AllocateZeroPool(MyStrSize);
                UnicodeSPrint(Name, MyStrSize, L"%s (PCI:%x-%x-%x-%x) Drive %d %s Pun %d", ScsiPassThru->Mode->ControllerName,
                  Seg,Bus,Dev,Func,ControllerNum,ScsiPassThru->Mode->ChannelName,Pun);
                goto ProcExit;
              }
           }
      }

      for (ptDP = ptDevPath; !IsDevicePathEndType (ptDP); ptDP = NextDevicePathNode (ptDP)) {
        if (DevicePathType (ptDP) == MESSAGING_DEVICE_PATH && DevicePathSubType(ptDP) == MSG_SCSI_DP) {
          Pun = ((SCSI_DEVICE_PATH *)ptDP)->Pun;
          Lun = ((SCSI_DEVICE_PATH *)ptDP)->Lun;
          break;
        }
        if (DevicePathType (ptDP) == MESSAGING_DEVICE_PATH && DevicePathSubType(ptDP) == MSG_SATA_DP) {
          Lun = ((SATA_DEVICE_PATH *)ptDP)->Lun;
          break;
        }
      }
      MyStrSize = 68;
      Name = AllocateZeroPool(MyStrSize);

      if (((Pun == 0) && (Lun != 0x4000)) || (Pun == 0xFF)) {
        //The Lun number is counted from 1 in the Raid configuration interface,so we follow this behavior
        Lun++;
        UnicodeSPrint(Name, MyStrSize, L"RAID (PCI:%x-%x-%x-%x) Lun %d", Seg, Bus, Dev, Func, Lun);
      } else {
        UnicodeSPrint(Name, MyStrSize, L"SAS (PCI:%x-%x-%x-%x) Pun %d", Seg, Bus, Dev, Func, Pun);
      }
  } else {
    Status = gBS->HandleProtocol(
                    Handle,
                    &gOpromPnpNameProtocolGuid,
                    (VOID **)&PnpName
                    );
    if(!EFI_ERROR(Status)){
      Name = AllocateCopyPool(StrSize(PnpName->Name), PnpName->Name);
    } else {
      Name = NULL;
    }
  }

ProcExit:
  return Name;
}

/**
  Try to get the boot description of network options.

  @param Handle                Controller handle.

  @return  The description string.
**/
CHAR16 *
GetNetworkDescription (
  IN EFI_HANDLE                Handle
  )
{
  EFI_STATUS                     Status;
  EFI_DEVICE_PATH_PROTOCOL       *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL       *TempDevicePath;
  EFI_PCI_IO_PROTOCOL            *PciIo;
  CHAR16                         *DevStr;
  UINT8                          IpType;
  CHAR16                         *IpTypeStr;
  CHAR16                         *BootTypeStr;
  UINTN                          Size;
  CHAR16                         *Description = NULL;
  EFI_HANDLE                     NiiPciHandle;
  UINTN                          Seg, Bus, Dev, Func;
  EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL  *Nii = NULL;
  UINT16                         IfNum = 0;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->OpenProtocol (
                  Handle,
                  &gEfiLoadFileProtocolGuid,
                  NULL,
                  gImageHandle,
                  Handle,
                  EFI_OPEN_PROTOCOL_TEST_PROTOCOL
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }

  DevicePath = DevicePathFromHandle(Handle);
  if (DevicePath == NULL) {
    return NULL;
  }
  TempDevicePath = DevicePath;
  Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TempDevicePath, &Handle);
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  Status = gBS->HandleProtocol(Handle, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
  if (EFI_ERROR(Status)) {
    return NULL;
  }

  TempDevicePath = DevicePath;
  Status = gBS->LocateDevicePath(&gEfiNetworkInterfaceIdentifierProtocolGuid_31, &TempDevicePath, &NiiPciHandle);
  if (EFI_ERROR(Status)) {
    NiiPciHandle = NULL;
  }
  gBS->HandleProtocol(NiiPciHandle, &gEfiNetworkInterfaceIdentifierProtocolGuid_31, (VOID**)&Nii);
  if (Nii != NULL) {
    IfNum = Nii->IfNum;
  }

  IpType = GetEfiNetWorkType(DevicePath, &BootTypeStr);
  DevStr = GetNameStringOfNetcard(DevicePath, TRUE);
  if (IfNum != 0) {
    PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Func);
    AppendIfNumToNameString(&DevStr, Func, IfNum);
  }
  switch (IpType) {
    case MSG_IPv4_DP:
      IpTypeStr = L"IPv4";
      break;
    case MSG_IPv6_DP:
      IpTypeStr = L"IPv6";
      break;
    default:
      IpTypeStr = L"";
      break;
  }
  
  if (DevStr != NULL) {
    Size = StrSize(DevStr) + StrSize(BootTypeStr) + (StrLen(IpTypeStr) + 1) * sizeof(CHAR16);
    Description = (CHAR16 *)AllocatePool(Size);
    ASSERT(Description != NULL);
    UnicodeSPrint (
      Description, 
      Size, 
      L"%s %s %s", 
      DevStr, 
      BootTypeStr,
      IpTypeStr
      );
    FreePool(DevStr);
  }

  return Description;
}

GET_BOOT_DESCRIPTION mByoUEFIBootDescriptionHandlers[] = {
  GetUsbDescription,
  GetDiskInfoDescription,
  GetSasDescription,
  GetRaidDescription,
  GetNetworkDescription
};

/**
  Return the UEFI boot description for the option.

  @param DevicePath                DevicePath of Option.

  @return  The description string.
**/
CHAR16 *
EFIAPI
ByoGetUEFIBootDescription (
  IN EFI_DEVICE_PATH_PROTOCOL        *DevicePath
  )
{
  EFI_STATUS                     Status;
  EFI_HANDLE                     Handle;
  UINTN                          Index;
  UINTN                          Size;
  EFI_BLOCK_IO_PROTOCOL          *BlkIo;
  UINT8                          IpType;
  CHAR16                         *BootTypeStr;  
  EFI_DEVICE_PATH_PROTOCOL       *TempDp = NULL;
  CHAR16                         *Description = NULL;
  CHAR16                         *Temp = NULL;
  CHAR16                         UefiPrefix[] = L"UEFI ";

  if (DevicePath == NULL) {
    return NULL;
  }

  //
  // Check if it's BlockIo
  //
  TempDp = DevicePath;
  Status = gBS->LocateDevicePath(&gEfiBlockIoProtocolGuid, &TempDp, &Handle);
  if (!EFI_ERROR(Status)) {
    Status = gBS->HandleProtocol (
                Handle,
                &gEfiBlockIoProtocolGuid,
                (VOID **)&BlkIo
                );
    if (!EFI_ERROR(Status)) {
      goto GET_DESC;
    }
  }

  //
  // Check if it's Non-BlockIo SimpleFileSystem
  //
  TempDp = DevicePath;
  Status = gBS->LocateDevicePath(&gEfiSimpleFileSystemProtocolGuid, &TempDp, &Handle);
  if (!EFI_ERROR(Status)) {
    Status = gBS->HandleProtocol (
                Handle,
                &gEfiBlockIoProtocolGuid,
                (VOID **)&BlkIo
                );
    if (EFI_ERROR(Status)) {
      goto GET_DESC;
    }
  }

  //
  // Check if it's LoadFile
  //
  TempDp = DevicePath;
  Status = gBS->LocateDevicePath(&gEfiLoadFileProtocolGuid, &TempDp, &Handle);
  if (!EFI_ERROR(Status)) {
    IpType = GetEfiNetWorkType(DevicePath, &BootTypeStr);
    if (IpType == 0xFF) {
      return NULL;
    }
  }

GET_DESC:
  for (Index = 0; Index < ARRAY_SIZE (mByoUEFIBootDescriptionHandlers); Index++) {
    Temp = mByoUEFIBootDescriptionHandlers[Index] (Handle);
    if (Temp != NULL) {
      Size = StrSize(Temp) + StrLen(UefiPrefix) * sizeof(CHAR16);
      Description = (CHAR16 *)AllocateZeroPool(Size);
      ASSERT(Description != NULL);
      UnicodeSPrint (Description, Size, L"%s%s", UefiPrefix, Temp);
      FreePool(Temp);
      break;
    }
  }

  return Description;
}