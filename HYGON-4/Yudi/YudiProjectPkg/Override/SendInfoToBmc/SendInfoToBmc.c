/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SendInfoToBmc.c

Abstract:
  Send bios info to BMC.

Revision History:

**/

#include "SendInfoToBMC.h"
#include <IndustryStandard/IpmiNetFnStorage.h>
#include <Protocol/ReportRasUceProtocol.h>
#include <Library/TimeStampLib.h>


#define _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED   0x0400
#define ATAPI_DEVICE                            0x8000


BOOLEAN
IsMyDevicePathEnd (
  IN EFI_DEVICE_PATH_PROTOCOL    *Dp
  );


VOID*
LibGetPciIoFromDp (
  IN EFI_BOOT_SERVICES         *BS,
  IN EFI_DEVICE_PATH_PROTOCOL  *Dp
  )
{
  EFI_STATUS                Status;
  EFI_DEVICE_PATH_PROTOCOL  *TempDp = Dp;
  EFI_HANDLE                Device;
  EFI_PCI_IO_PROTOCOL       *PciIo = NULL;
  Status = BS->LocateDevicePath(&gEfiPciIoProtocolGuid, &TempDp, &Device);

  if(!EFI_ERROR(Status) && IsMyDevicePathEnd(TempDp)) {
    Status = BS->HandleProtocol(Device, &gEfiPciIoProtocolGuid, (VOID**)&PciIo);
  }
  return PciIo;
}

UINT32
GetCpuCacheKBSize (
  EFI_SMBIOS_PROTOCOL       *Smbios,
  EFI_SMBIOS_HANDLE         CacheHandle
  )
{
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER  p;
  EFI_STATUS                Status = EFI_SUCCESS;
  UINT32                    CacheSize = 0;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_CACHE_INFORMATION;
  while(1){
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status) || SmbiosHandle == CacheHandle){
      p.Hdr = SmbiosHdr;
      break;
    }
  }
  if (EFI_ERROR(Status)) {
    goto ProcExit;
  }

  CacheSize = p.Type7->InstalledSize2;
  if(CacheSize & BIT31){
    CacheSize = CacheSize * 64;
  }

//  if(CacheSize >= 1024 ){
//    CacheSize = CacheSize / 1024;
//  }

ProcExit:
  return CacheSize;
}

UINT8
GetAtaMajorVersion(UINT16 Major)
{
  UINT8   Ver = 0;
  UINT8   i;

  if(Major == 0x0000 || Major == 0xFFFF){
    return 0;
  }

  for(i = 14; i > 0; i--){
    if((Major >> i) & 1){
      Ver = i;
      break;
    }
  }

  return Ver;
}

UINTN
GetAtaMediaRotationRate(ATA_IDENTIFY_DATA *IdentifyData)
{
  UINT16  Rate;

  if(GetAtaMajorVersion(IdentifyData->major_version_no) < 8){
    return 0;
  }

  Rate = IdentifyData->nominal_media_rotation_rate;
  if(Rate == 1){
    return 1;
  } else if(Rate >= 0x401 && Rate <= 0xFFFE){
    return Rate;
  } else {
    return 0;
  }
}

VOID
ConvertData (
  IN  UINT8   *Data,
  IN  UINTN   DataSize
  )
{
  UINTN  Index;
  UINT8  Data8;

  ASSERT(DataSize!=0 && Data!=NULL);

  DataSize &= ~BIT0;
  for(Index = 0; Index < DataSize; Index += 2){
    Data8         = Data[Index];
    Data[Index]   = Data[Index+1];
    Data[Index+1] = Data8;
  }

}


VOID
SendBiosInfo2Bmc (
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS                   Status;
  UINT8                        ResponseData[8];
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TYPE              SmbiosType;
  EFI_SMBIOS_TABLE_HEADER      *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER     p;
  UINT8                        ResponseSize;
  BIOS_VERSION_2_BMC           *BiosVer2Bmc;
  CHAR8                        *BiosDate;
  CHAR8                        *BiosVer;
  
  DEBUG((EFI_D_INFO, "SendBiosInfo2Bmc()\n"));

  BiosVer2Bmc = (BIOS_VERSION_2_BMC*)AllocateZeroPool(sizeof(BIOS_VERSION_2_BMC));
  ASSERT (BiosVer2Bmc != NULL);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr    = (SMBIOS_STRUCTURE*)SmbiosHdr;
  BiosVer  = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosVersion);
  BiosDate = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type0->BiosReleaseDate);

  BiosVer2Bmc->Function = SET_BIOS_VER;
  BiosVer2Bmc->BiosVerOffset = OFFSET_OF(BIOS_VERSION_2_BMC, BiosVer);
  BiosVer2Bmc->BiosDateOffset = OFFSET_OF(BIOS_VERSION_2_BMC, BiosDate);
  AsciiStrCpyS(BiosVer2Bmc->BiosVer, sizeof(BiosVer2Bmc->BiosVer), BiosVer);
  AsciiStrCpyS(BiosVer2Bmc->BiosDate, sizeof(BiosVer2Bmc->BiosDate), BiosDate);

  DEBUG ((DEBUG_INFO, "sizeof BiosVer2Bmc = %d\n", sizeof (*BiosVer2Bmc)));
  ResponseSize = sizeof(ResponseData);
  Status = EfiSendCommandToBMC (
             SM_BYOSOFT_NETFN_APP,
             SM_BYOSOFT_NETFN_SUB_FUN,
             (UINT8 *)BiosVer2Bmc,
             sizeof(BIOS_VERSION_2_BMC),
             ResponseData,
             &ResponseSize
             );
  DEBUG ((DEBUG_INFO, "EfiSendCommandToBMC return %r\n", Status));
  FreePool (BiosVer2Bmc);
}

VOID
SendCpuInfo2Bmc(
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS                   Status;
  UINT8                        ResponseSize;
  UINT8                        ResponseData[8];
  CPU_INFO_2_BMC               *CpuInfo;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TYPE              SmbiosType;
  EFI_SMBIOS_TABLE_HEADER      *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER     p;
  UINT8                        DeviceNumId=0;
  CHAR8                        *CpuName;
  UINT8                        ProcessorType;
  CHAR8                        *InstructionSet, *ProcessorArchitecture, *Manufacturer;
  CHAR8                        *Health, *DeviceLocator, *Position, *ProcessorVersion,*SerialNumber;
  UINT32                       CpuId;
  UINT16                       MaxSpeed, CurSpeed;
  UINT8                        CoreCount, ThreadCount;
  UINT32                       L1CacheSize, L2CacheSize, L3CacheSize;
  EFI_SMBIOS_HANDLE            L1CacheHandle, L2CacheHandle, L3CacheHandle;

  DEBUG((EFI_D_INFO, "SendCpuInfo2Bmc()\n"));

  CpuInfo = AllocateZeroPool (sizeof (CPU_INFO_2_BMC));
  ASSERT (CpuInfo != NULL);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_PROCESSOR_INFORMATION;

  while(1){
    Status  = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    p.Hdr   = SmbiosHdr;
    if(p.Type4->Status == 0 && p.Type4->MaxSpeed == 0){
      continue;   //cpu not present that we shouldn't report to bmc for saving boot time.
    }
    CpuName = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->ProcessorVersion);
    ProcessorType = p.Type4->ProcessorType;
    ProcessorArchitecture = "x86";
    InstructionSet =  "x86-64";
    Manufacturer = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->ProcessorManufacturer);

    ProcessorVersion = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->ProcessorVersion);
    TrimStr8(ProcessorVersion);

    CpuId = *(UINT32*)&(p.Type4->ProcessorId.Signature);
    MaxSpeed = p.Type4->MaxSpeed;
    CoreCount = p.Type4->CoreCount;
    ThreadCount = p.Type4->ThreadCount;
    L1CacheHandle = p.Type4->L1CacheHandle;
    L2CacheHandle = p.Type4->L2CacheHandle;
    L3CacheHandle = p.Type4->L3CacheHandle;
    L1CacheSize = GetCpuCacheKBSize(Smbios, L1CacheHandle);
    L2CacheSize = GetCpuCacheKBSize(Smbios, L2CacheHandle)/1024;
    L3CacheSize = GetCpuCacheKBSize(Smbios, L3CacheHandle)/1024;
    CurSpeed = p.Type4->CurrentSpeed;
    Health = "OK";
    DeviceLocator = "CPU0";
    DeviceLocator[3] = DeviceNumId + '0';
    Position =  "MainBoard";
    SerialNumber = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type4->SerialNumber);
    TrimStr8(SerialNumber);

    CpuInfo->Function = SET_CPU_INFO;
    CpuInfo->DeviceNum = DeviceNumId;
    AsciiStrCpyS(CpuInfo->Name, sizeof (CpuInfo->Name), CpuName);
    CpuInfo->DeviceID = DeviceNumId;
    CpuInfo->ProcessorType = ProcessorType;
    AsciiStrCpyS(CpuInfo->ProcessorArchitecture, sizeof(CpuInfo->ProcessorArchitecture), ProcessorArchitecture);
    AsciiStrCpyS(CpuInfo->InstructionSet, sizeof(CpuInfo->InstructionSet), InstructionSet);
    AsciiStrCpyS(CpuInfo->Manufacturer, sizeof(CpuInfo->Manufacturer), Manufacturer);
    AsciiStrCpyS(CpuInfo->Model, sizeof(CpuInfo->Model), ProcessorVersion);
    CpuInfo->ProcessorId = CpuId;
    CpuInfo->MaxSpeed = MaxSpeed;
    CpuInfo->TotalCores = CoreCount;
    CpuInfo->TotalThreads = ThreadCount;
    CpuInfo->Socket = DeviceNumId+1;
    AsciiStrCpyS(CpuInfo->Health, sizeof(CpuInfo->Health), Health);
    CpuInfo->L1CacheSize = (UINT16)L1CacheSize;
    CpuInfo->L2CacheSize = (UINT16)L2CacheSize;
    CpuInfo->L3CacheSize = (UINT16)L3CacheSize;
    AsciiStrCpyS(CpuInfo->DeviceLocator, sizeof(CpuInfo->DeviceLocator), DeviceLocator);
    CpuInfo->Frequency = CurSpeed;
    AsciiStrCpyS(CpuInfo->Position, sizeof(CpuInfo->Position), Position);
    AsciiStrCpyS(CpuInfo->SerialNumbers, sizeof(CpuInfo->SerialNumbers), SerialNumber);

    DEBUG ((DEBUG_INFO, "sizeof CpuInfo = %d\n", sizeof (*CpuInfo)));
    ResponseSize = sizeof (ResponseData);
    Status = EfiSendCommandToBMC (
               SM_BYOSOFT_NETFN_APP,
               SM_BYOSOFT_NETFN_SUB_FUN,
               (UINT8 *)CpuInfo,
               sizeof (CPU_INFO_2_BMC),
               ResponseData,
               &ResponseSize
               );
    DEBUG ((DEBUG_INFO, "EfiSendCommandToBMC return %r\n", Status));
    DeviceNumId++;
  }
  FreePool (CpuInfo);
}

VOID
SendDIMMInfo2Bmc( 
  IN EFI_SMBIOS_PROTOCOL  *Smbios
  )
{
  EFI_STATUS                   Status;
  EFI_SMBIOS_HANDLE            SmbiosHandle;
  EFI_SMBIOS_TYPE              SmbiosType;
  EFI_SMBIOS_TABLE_HEADER      *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER     p;
  UINT8                        ResponseSize;
  UINT8                        ResponseData[8];
  DIMM_INFO_2_BMC              *DimmInfo;
  UINT8                        DeviceNumId=0xFF, DataWidth;
  UINT8                        MyDeviceNumId;
  CHAR8                        DimmName[7] = {"DIMM00"};
  UINT32                       SingleSize;
  CHAR8                        *Manufacturer, *PN, *SN,*DeviceLocator,*BaseModuleType, *Health;
  CHAR8                        *DimmTypeString[] = {"DDR2","DDR3","DDR4","DDR5","UnKnown"};
  UINT8                        DimmType;
  UINTN                        Len;
  CHAR8                        *Dw;

  DEBUG((EFI_D_INFO, "SendDIMMInfo2Bmc()\n"));

  DimmInfo = AllocateZeroPool (sizeof(DIMM_INFO_2_BMC));
  ASSERT (DimmInfo != NULL);

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType   = EFI_SMBIOS_TYPE_MEMORY_DEVICE;
  while(1) {
    Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
    if(EFI_ERROR(Status)){
      break;
    }
    DeviceNumId++;
    p.Hdr = SmbiosHdr;
    Dw = LibSmbiosGetTypeEnd(p.Type17) - 4;
    if (!p.Type17->Size) {
      if (CompareMem(Dw, "MNT", 4) == 0) {
        Health = "BAD";
      } else {
        continue;
      }
    } else {
      Health = "OK";
    }

// DeviceLocator : [P0_DIMM_A1]
// DeviceLocator : [P0_DIMM_A0]

    MyDeviceNumId = DeviceNumId;

    DeviceLocator = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->DeviceLocator);

    if(FixedPcdGetBool(PcdSwapDimm0Dimm1InSendBmcInfo)){
      Len = AsciiStrLen(DeviceLocator);
      if(Len){
        if(((MyDeviceNumId & BIT0) == 0) && DeviceLocator[Len-1] == '1'){
          MyDeviceNumId |= BIT0;
        } else if(((MyDeviceNumId & BIT0) == BIT0) && DeviceLocator[Len-1] == '0'){
          MyDeviceNumId &= (UINT8)~BIT0;
        }
      }
    }
    
    DimmName[4] = '0' + MyDeviceNumId/10;
    DimmName[5] = '0' + MyDeviceNumId%10;

    SingleSize = p.Type17->Size;
    if(SingleSize == 0 || SingleSize == 0xFFFF){
      SingleSize = 0;
    } else if(SingleSize == 0x7FFF){
      SingleSize = p.Type17->ExtendedSize & (~BIT31);    // MB
    } else {
      if(SingleSize & BIT15){                            // unit in KB.
        SingleSize = (SingleSize&(~BIT15)) >> 10;        // to MB
      }else{                                             // unit in MB.
      }
    }
    Manufacturer = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->Manufacturer);

    SN = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->SerialNumber);
    if(p.Type17->DataWidth > 0xff){
      DataWidth = (UINT8)(p.Type17->DataWidth & 0xFF);
    }else {
      DataWidth = (UINT8)(p.Type17->DataWidth);
    }
    PN = LibSmbiosGetStringInTypeByIndex(p.Hdr, p.Type17->PartNumber);

    switch(p.Type17->MemoryType){
      case MemoryTypeDdr4:
        DimmType = 2; //DDR4
        break;
      case MemoryTypeDdr3:
        DimmType = 1; //DDR3
        break;
      case MemoryTypeDdr2:
        DimmType = 0; //DDR2
        break;
      default:
        DimmType = 4; //UnKnown
        break;
    }

    if(p.Type17->TypeDetail.Registered == 1 && p.Type17->TypeDetail.LrDimm == 0 && p.Type17->FormFactor == 9) {
      BaseModuleType = "RDIMM";
    } else if (p.Type17->TypeDetail.Unbuffered == 1 && p.Type17->FormFactor == 9) {
      BaseModuleType = "UDIMM";
    } else if (p.Type17->TypeDetail.Registered == 1 && p.Type17->TypeDetail.LrDimm == 1 && p.Type17->FormFactor == 9) {
      BaseModuleType = "LDIMM";
    } else if (p.Type17->TypeDetail.Unbuffered == 1 && p.Type17->FormFactor == 13) {
      BaseModuleType = "SO_DIMM";
    } else {
      BaseModuleType = "other";
    }

    DimmInfo->Function = SET_DIMM_INFO;
    DimmInfo->DeviceNum = MyDeviceNumId;
    AsciiStrCpyS(DimmInfo->Name, sizeof(DimmInfo->Name), DimmName);
    AsciiStrCpyS(DimmInfo->NameId, sizeof(DimmInfo->NameId), DimmName);
    DimmInfo->Capacity = (UINT16)(SingleSize / 1024);
    AsciiStrCpyS(DimmInfo->Manufacturer, sizeof(DimmInfo->Manufacturer), Manufacturer);
    DimmInfo->OperatingSpeed = p.Type17->Speed;
    DimmInfo->CurrentSpeed = p.Type17->ConfiguredMemoryClockSpeed;
    AsciiStrCpyS(DimmInfo->SerialNumber, sizeof(DimmInfo->SerialNumber), SN);
    AsciiStrCpyS(DimmInfo->MemoryDeviceType, sizeof(DimmInfo->MemoryDeviceType), DimmTypeString[DimmType]);
    DimmInfo->DataWidth = DataWidth;
    DimmInfo->RankCount = p.Type17->Attributes & 0xf;
    AsciiStrCpyS(DimmInfo->PartNumber, sizeof(DimmInfo->PartNumber), PN);
    AsciiStrCpyS(DimmInfo->DeviceLocator, sizeof(DimmInfo->DeviceLocator), DeviceLocator);
    AsciiStrCpyS(DimmInfo->BaseModuleType, sizeof(DimmInfo->BaseModuleType), BaseModuleType);
    AsciiStrCpyS(DimmInfo->Health, sizeof(DimmInfo->Health), Health);

    DEBUG ((DEBUG_INFO, "sizeof DimmInfo = %d\n", sizeof (*DimmInfo)));
    ResponseSize = sizeof (ResponseData);
    Status = EfiSendCommandToBMC (
               SM_BYOSOFT_NETFN_APP,
               SM_BYOSOFT_NETFN_SUB_FUN,
               (UINT8 *)DimmInfo,
               sizeof(DIMM_INFO_2_BMC),
               ResponseData,
               &ResponseSize
               );
    DEBUG ((DEBUG_INFO, "EfiSendCommandToBMC return %r\n", Status));
  }
  FreePool (DimmInfo);
}


VOID
SendPcieInfo2Bmc()
{
  EFI_STATUS                               Status;
  UINT8                                    ResponseSize;
  UINT8                                    ResponseData[8];
  PCIE_INFO_2_BMC                          *PcieInfo;
  UINTN                                    HandleCount, Index;
  EFI_HANDLE                               *HandleBuffer;
  EFI_PCI_IO_PROTOCOL                      *PciIo;
  EFI_PCI_IO_PROTOCOL                      *BridgePciIo;
  EFI_DEVICE_PATH_PROTOCOL                 *PciDp;
  PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH  SlotDp;
  UINT8                                    DeviceNumId=0;
  UINTN                                    Segment, Bus, Device, Function;
  UINTN                                    BridgeSegment, BridgeBus, BridgeDevice, BridgeFunction;
  UINT32                                   VidDid;
  UINT8                                    ClassCode[3];
  UINT16                                   VenderId, DeviceId, SubsystemId, SubsystemVendorId;
  UINT8                                    RevisionId;
  UINT32                                   LinkStatus, LinkCap, LinkCap2;
  UINTN                                    CurLinkWidth, MaxLinkWidth, CurLinkSpeed, MaxLinkSpeed;
  UINT8                                    CapabilityPtr = 0, CapabilityID = 0, PcieCapabilityPtr = 0;
  UINT16                                   CapabilityEntry = 0;
  EFI_DEVICE_PATH_PROTOCOL                 EndDp = gEndEntire;

  DEBUG((EFI_D_INFO, "SendPcieInfo2Bmc()\n"));

  PcieInfo = (PCIE_INFO_2_BMC *)AllocateZeroPool (sizeof(PCIE_INFO_2_BMC));
  ASSERT (PcieInfo != NULL);

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 9, 3, &ClassCode);
    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);

    //
    // skip bridge + Base system peripherals + Encryption/Decryption controllers + vga control
    //
    if (ClassCode[2] == PCI_CLASS_BRIDGE || ClassCode[2] == PCI_CLASS_SYSTEM_PERIPHERAL || Function != 0) {
      continue;
    }

    PciIo->GetLocation (PciIo, &Segment, &Bus, &Device, &Function);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &VidDid);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0, 1, &VenderId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x02, 1, &DeviceId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8,  0x08, 1, &RevisionId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x2C, 1, &SubsystemVendorId);
    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x2E, 1, &SubsystemId);

    if ((VidDid&0xFFFF) == 0x1D94) { // skip bridge + Base system peripherals + Encryption/Decryption controllers
      continue;
    }
    if (VidDid == 0x20001A03 || VidDid == 0x06221B21 ) { 
      continue;
    }

    PcieCapabilityPtr = 0;
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);
    while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
      PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapabilityPtr, 1, &CapabilityEntry);
      CapabilityID = (UINT8) CapabilityEntry;

      if ((UINT8)EFI_PCI_CAPABILITY_ID_PCIEXP == CapabilityID) {
        PcieCapabilityPtr = CapabilityPtr;
        break;
      }
      CapabilityPtr = (UINT8)(CapabilityEntry >> 8);
    }
    if (PcieCapabilityPtr) {

        PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, PcieCapabilityPtr + 0x10, 1, &LinkStatus);
        LinkStatus = LinkStatus >> 16;

        PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, PcieCapabilityPtr + 0x0c, 1, &LinkCap);
        PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, PcieCapabilityPtr + 0x2c, 1, &LinkCap2);

        CurLinkWidth = (LinkStatus >> 4) & 0x3F;
        MaxLinkWidth = (LinkCap >> 4) & 0x3F;
        CurLinkSpeed = LinkStatus & 0xF;
        MaxLinkSpeed = LinkCap & 0xF;
//      SupportLinkSpeedVector = (LinkCap2 >> 1) & 0x7F;

        if(CurLinkSpeed >= 1 && CurLinkSpeed <= 7){
          CurLinkSpeed = (UINTN)1 << (CurLinkSpeed - 1);
          if (CurLinkSpeed == 4) {
            CurLinkSpeed = 3;
          } else if (CurLinkSpeed == 8){
            CurLinkSpeed = 4;
          }
        }
        if(MaxLinkSpeed >= 1 && MaxLinkSpeed <= 7){
          MaxLinkSpeed = (UINTN)1 << (MaxLinkSpeed - 1);
          if (MaxLinkSpeed == 4) {
            MaxLinkSpeed = 3;
          } else if (MaxLinkSpeed == 8){
            MaxLinkSpeed = 4;
          }
        }

    }
    Status = gBS->HandleProtocol (HandleBuffer[Index], &gEfiDevicePathProtocolGuid, (VOID**)&PciDp);
    CopyMem(&SlotDp, PciDp, sizeof(PLATFORM_ONBOARD_CONTROLLER_DEVICE_PATH));
    CopyMem(&SlotDp.End, &EndDp, sizeof(EFI_DEVICE_PATH_PROTOCOL));
    ShowDevicePathDxe(gBS, (EFI_DEVICE_PATH_PROTOCOL*)&SlotDp);
 
    BridgePciIo = NULL;
    BridgePciIo = (EFI_PCI_IO_PROTOCOL*)LibGetPciIoFromDp(gBS, (EFI_DEVICE_PATH_PROTOCOL*)&SlotDp);
    if(BridgePciIo != NULL){
      BridgePciIo->GetLocation (BridgePciIo, &BridgeSegment, &BridgeBus, &BridgeDevice, &BridgeFunction);
    }

    PcieInfo->Function = SET_PCI_INFO;
    PcieInfo->DeviceNum = DeviceNumId;
    PcieInfo->VenderID = VenderId;
    PcieInfo->DeviceID = DeviceId;
    PcieInfo->Bus = (UINT8)Bus;
    PcieInfo->Device = (UINT8)Device;
    PcieInfo->Fun = (UINT8)Function;
    CopyMem (&PcieInfo->ClassCode, (UINT8*)ClassCode, 3);
    PcieInfo->DeviceClass = (UINT8)ClassCode[2];
    PcieInfo->FunctionID = (UINT8)Function;
    PcieInfo->FunctionType = 0;//Physical
    PcieInfo->RevisionID = (UINT8)RevisionId;
    PcieInfo->SubSystemVenderID = SubsystemVendorId;
    PcieInfo->SubSystemID = SubsystemId;
    PcieInfo->MaxLinkWidth = (UINT8) MaxLinkWidth;
    PcieInfo->MaxLinkSpeed = (UINT8) MaxLinkSpeed;
    PcieInfo->CurrentLinkWidth = (UINT8) CurLinkWidth;
    PcieInfo->CurrentLinkSpeed = (UINT8) CurLinkSpeed;
    PcieInfo->Slot = (UINT8)LibGetPciSlotNum(gBS, PciDp);
    PcieInfo->RiserType = 0;
    PcieInfo->PCIELocationOnRiser = 0;
    PcieInfo->RootBridgeBus = (UINT8)BridgeBus;
    PcieInfo->RootBridgeDev = (UINT8)BridgeDevice;
    PcieInfo->RootBridgeFun = (UINT8)BridgeFunction;

    DEBUG ((DEBUG_INFO, "sizeof PcieInfo = %d\n", sizeof (*PcieInfo)));
    ResponseSize = sizeof(ResponseData);
    Status = EfiSendCommandToBMC (
               SM_BYOSOFT_NETFN_APP,
               SM_BYOSOFT_NETFN_SUB_FUN,
               (UINT8 *)PcieInfo,
               sizeof(PCIE_INFO_2_BMC),
               ResponseData,
               &ResponseSize
               );
    DEBUG ((DEBUG_INFO, "EfiSendCommandToBMC return %r\n", Status));
    DeviceNumId++;
  }
  FreePool (PcieInfo);
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
}

VOID
SendHddInfo2Bmc()
{
  EFI_STATUS                   Status;
  UINT8                        ResponseSize;
  UINT8                        ResponseData[8];
  HDD_INFO_2_BMC               *HddInfo;
  EFI_DISK_INFO_PROTOCOL       *DiskInfo;
  ATA_IDENTIFY_DATA            *IdentifyData = NULL;
  UINTN                        HandleCount,RotationRate,Index;
  EFI_HANDLE                   *HandleBuffer;
  UINT8                        DeviceNumId=0;
  UINT32                       BufferSize;
  UINT8                        ModelName[40+1];
  UINT8                        FirmwareVer[8+1];
  UINT8                        SerialNo[20+1];
  CHAR8                        *Module, *Health, *IndicatorStatus;
  CHAR8                        *State, *Protocol, *MediaType, *IndicatorLED, *HotspareType;
  UINT64                       DriveSizeInBytes = 0, NumSectors = 0, RemainderInBytes = 0;
  UINT32                       DriveSizeInGB = 0;


  DEBUG((EFI_D_INFO, "SendHddInfo2Bmc()\n"));
  
  HddInfo = (HDD_INFO_2_BMC*)AllocateZeroPool(sizeof(HDD_INFO_2_BMC));
  ASSERT(HddInfo != NULL);
  IdentifyData = (ATA_IDENTIFY_DATA*)AllocateZeroPool(sizeof(ATA_IDENTIFY_DATA));
  ASSERT(IdentifyData != NULL);

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDiskInfoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  for (Index = 0; Index < HandleCount; Index++) {

    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDiskInfoProtocolGuid,
                    (VOID**)&DiskInfo
                    );
    ASSERT_EFI_ERROR(Status); 

    if(!CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoAhciInterfaceGuid) && !CompareGuid(&DiskInfo->Interface, &gEfiDiskInfoIdeInterfaceGuid)){
      continue;
    }
    
    BufferSize = sizeof(ATA_IDENTIFY_DATA);
    Status = DiskInfo->Inquiry (
                         DiskInfo,
                         IdentifyData,
                         &BufferSize
                         ); 
    if(Status != EFI_NOT_FOUND){
      continue;
    }
    
    BufferSize = sizeof(ATA_IDENTIFY_DATA);
    Status = DiskInfo->Identify (
                         DiskInfo,
                         IdentifyData,
                         &BufferSize
                         );
    if (EFI_ERROR(Status)) {
      DEBUG((EFI_D_ERROR, "Identify %r\n", Status));
      continue;
    }
    if ((!(IdentifyData->config & ATAPI_DEVICE)) || (IdentifyData->config == 0x848A)) {
      if (IdentifyData->command_set_supported_83 & _48_BIT_ADDRESS_FEATURE_SET_SUPPORTED) {
        NumSectors = *(UINT64 *)&IdentifyData->maximum_lba_for_48bit_addressing; 
      } else {
        NumSectors = IdentifyData->user_addressable_sectors_lo + (IdentifyData->user_addressable_sectors_hi << 16);
      }

      DriveSizeInBytes = MultU64x32(NumSectors, 512);
      DriveSizeInGB    = (UINT32)DivU64x64Remainder (DriveSizeInBytes, 1000000000, &RemainderInBytes);
      DriveSizeInGB    &= (UINT32)~BIT0;
    }
    RotationRate = GetAtaMediaRotationRate(IdentifyData);

    CopyMem(ModelName, IdentifyData->ModelName, sizeof(IdentifyData->ModelName));
    SwapWordArray(ModelName, 40);
    ModelName[40] = 0;
    TrimStr8(ModelName);

    Module = "SATA";

    CopyMem(FirmwareVer, IdentifyData->FirmwareVer, sizeof(IdentifyData->FirmwareVer));
    SwapWordArray(FirmwareVer, 8);
    FirmwareVer[8] = 0;
    TrimStr8(FirmwareVer);

    Health = "OK";
    State = "Enabled";
    Protocol = "SATA";
    if(RotationRate == 1){
      MediaType = "SSD";
    } else {
      MediaType = "HDD";
    }

    CopyMem(SerialNo, IdentifyData->SerialNo, sizeof(IdentifyData->SerialNo));
    SwapWordArray(SerialNo, 20);
    SerialNo[20] = 0;
    TrimStr8(SerialNo);

    IndicatorLED    = "Blinkling";
    HotspareType    = "None";
    IndicatorStatus = "OK";

    HddInfo->Function = SET_HDD_INFO;
    HddInfo->DeviceNum = DeviceNumId;
    AsciiStrCpyS(HddInfo->Name, sizeof(HddInfo->Name), ModelName);
    AsciiStrCpyS(HddInfo->Revision, sizeof(HddInfo->Revision), FirmwareVer);
    AsciiStrCpyS(HddInfo->Health, sizeof(HddInfo->Health), Health);
    AsciiStrCpyS(HddInfo->State, sizeof(HddInfo->State), State);
    HddInfo->CapacityBytes = DriveSizeInGB;
    HddInfo->FailurePredicted = TRUE;
    AsciiStrCpyS(HddInfo->Protocol ,sizeof(HddInfo->Protocol), Protocol);
    AsciiStrCpyS(HddInfo->MediaType, sizeof(HddInfo->MediaType), MediaType);
    AsciiStrCpyS(HddInfo->SerialNumber, sizeof(HddInfo->SerialNumber), SerialNo);

    HddInfo->CapableSpeed = 0;
    HddInfo->NegotiatedSpeed = 0;
    HddInfo->PredictedMediaLifeLeftPercent = 0;

    AsciiStrCpyS(HddInfo->IndicatorLED, sizeof(HddInfo->IndicatorLED), IndicatorLED);
    AsciiStrCpyS(HddInfo->HotspareType, sizeof(HddInfo->HotspareType), HotspareType);

    DEBUG ((DEBUG_INFO, "sizeof HddInfo = %d\n", sizeof (*HddInfo)));
    ResponseSize = sizeof(ResponseData);
    Status = EfiSendCommandToBMC (
               SM_BYOSOFT_NETFN_APP,
               SM_BYOSOFT_NETFN_SUB_FUN,
               (UINT8*)HddInfo,
               sizeof(HDD_INFO_2_BMC),
               ResponseData,
               &ResponseSize
               );
    DEBUG ((DEBUG_INFO, "EfiSendCommandToBMC return %r\n", Status));
    DeviceNumId++;

  }

  FreePool(HddInfo);
  FreePool(IdentifyData);
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
}


VOID
SendNvmeInfo2Bmc (
  VOID
  )
{
  EFI_STATUS                   Status;
  UINT8                        ResponseSize;
  UINT8                        ResponseData[8];
  UINTN                        HandleCount,Index;
  EFI_HANDLE                   *HandleBuffer;
  UINT8                        DeviceNumId=0;
  NVME_INFO_2_BMC              *NvmeInfo;
  BYO_DISKINFO_PROTOCOL        *ByoDiskInfo;
  CHAR8                        Mn[41];
  CHAR8                        Sn[21];
  UINTN                        Size;
  UINT64                       DiskSize, RemainderInBytes;
  UINT32                       DriveSizeInGB;
  CHAR8                        *Module, *Health, *State, *Protocol, *MediaType, *IndicatorLED;
  CHAR8                        *HotspareType, *IndicatorStatus, *FirmwareVer;

  DEBUG((EFI_D_INFO, "SendNvmeInfo2Bmc()\n"));

  NvmeInfo = (NVME_INFO_2_BMC *) AllocateZeroPool (sizeof(NVME_INFO_2_BMC));
  ASSERT (NvmeInfo != NULL);

  Status=gBS->LocateHandleBuffer (
                ByProtocol,
                &gByoDiskInfoProtocolGuid,
                NULL,
                &HandleCount,
                &HandleBuffer
                );
  DEBUG((EFI_D_INFO,"LocateHandleBuff gByoDiskInfoProtocolGuid Status = %r\n",Status));

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gByoDiskInfoProtocolGuid,
                    (VOID **)&ByoDiskInfo
                    );
    DEBUG((EFI_D_INFO,"HandleProtocol gByoDiskInfoProtocolGuid Status = %r\n",Status));
    if (EFI_ERROR (Status)) {
      continue;
    }
    DEBUG((EFI_D_INFO,"ByoDiskInfo->DevType = %d\n",ByoDiskInfo->DevType));
    if(ByoDiskInfo->DevType != BYO_DISK_INFO_DEV_NVME){
      continue;
    }

    Size = sizeof(Mn);
    Status = ByoDiskInfo->GetMn(ByoDiskInfo, Mn, &Size);
    Size = sizeof(Sn);
    Status = ByoDiskInfo->GetSn(ByoDiskInfo, Sn, &Size);
    Status = ByoDiskInfo->GetDiskSize(ByoDiskInfo, &DiskSize);

    DriveSizeInGB = (UINT32) DivU64x64Remainder(DiskSize, 1000000000, &RemainderInBytes);
    DriveSizeInGB &=~1;
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), DriveSizeInGB :%d.\n", DriveSizeInGB));

    Module = "NVMe";
    FirmwareVer = " ";
    Health = "OK"; 
    State = "Enabled";
    Protocol = "NVMe";
    MediaType = "SSD";
    IndicatorLED = "Blinkling";
    HotspareType = "None";
    IndicatorStatus = "OK";

    NvmeInfo->Function = SET_NVME_INFO;
    NvmeInfo->DeviceNum = DeviceNumId;
    AsciiStrCpyS(NvmeInfo->Name, sizeof(NvmeInfo->Name), Mn);
    AsciiStrCpyS(NvmeInfo->Revision, sizeof(NvmeInfo->Revision), FirmwareVer);
    AsciiStrCpyS(NvmeInfo->Health, sizeof(NvmeInfo->Health), Health);
    AsciiStrCpyS(NvmeInfo->State, sizeof(NvmeInfo->State), State);
    NvmeInfo->CapacityBytes = DriveSizeInGB;
    NvmeInfo->FailurePredicted = TRUE;

    AsciiStrCpyS(NvmeInfo->Protocol, sizeof(NvmeInfo->Protocol), Protocol);
    AsciiStrCpyS(NvmeInfo->MediaType, sizeof(NvmeInfo->MediaType), MediaType);
    AsciiStrCpyS(NvmeInfo->SerialNumber, sizeof(NvmeInfo->SerialNumber), Sn);
    NvmeInfo->CapableSpeed = 0;
    NvmeInfo->NegotiatedSpeed = 0;
    NvmeInfo->PredictedMediaLifeLeftPercent = 0;
    AsciiStrCpyS(NvmeInfo->IndicatorLED, sizeof(NvmeInfo->IndicatorLED), IndicatorLED);
    AsciiStrCpyS(NvmeInfo->HotspareType, sizeof(NvmeInfo->HotspareType), HotspareType);
    AsciiStrCpyS(NvmeInfo->Status, sizeof(NvmeInfo->Status), IndicatorStatus);

    DEBUG ((DEBUG_INFO, "sizeof NvmeInfo = %d\n", sizeof (*NvmeInfo)));
    ResponseSize = sizeof (ResponseData);
    Status = EfiSendCommandToBMC (
               SM_BYOSOFT_NETFN_APP,
               SM_BYOSOFT_NETFN_SUB_FUN,
               (UINT8 *)NvmeInfo,
               sizeof(NVME_INFO_2_BMC),
               ResponseData,
               &ResponseSize
               );
    DEBUG ((DEBUG_INFO, "EfiSendCommandToBMC return %r\n", Status));
    DeviceNumId++;
  }
  FreePool (NvmeInfo);
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
}

VOID
SendNetCardInfo2Bmc()
{
  EFI_STATUS                   Status;
  UINT8                        ResponseSize;
  UINT8                        ResponseData[8];
  UINTN                        HandleCount,Index,Index2;
  EFI_HANDLE                   *HandleBuffer;
  NETCARD_INFO_2_BMC           *NetCardInfo;
  UINT8                        DeviceNumId=0;
  EFI_PCI_IO_PROTOCOL          *PciIo,*PciIo2;
  EFI_DEVICE_PATH_PROTOCOL     *DevPath,*DevPath2;
  UINTN                        Seg, Bus, Dev, Fun, Seg2,Bus2, Dev2, Fun2;
  UINT8                        ClassCode[3];
  UINT8                        PciIndex=0, PortNum;
  UINT8                        MacAddr[6];
  UINT8                        MacAddr2[6];

  DEBUG((EFI_D_INFO, "SendNetCardInfo2Bmc()\n"));

  NetCardInfo = (NETCARD_INFO_2_BMC *) AllocateZeroPool (sizeof(NETCARD_INFO_2_BMC));
  ASSERT (NetCardInfo != NULL);

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **)&PciIo
                    );
    ASSERT_EFI_ERROR (Status);
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&DevPath
                    );

    PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 9, 3, &ClassCode);
    PciIo->GetLocation (PciIo, &Seg, &Bus, &Dev, &Fun);

    PciIndex ++;//mismatch SendPcieInfo2Bmc() pci devicenumid

    if ( ClassCode[2] != PCI_CLASS_NETWORK || Fun != 0) {
      continue;
    }

    PortNum = 1;
    Status = GetOnboardLanMacAddress(gBS, DevPath, MacAddr);
    if(!EFI_ERROR(Status)){
      CopyMem(&NetCardInfo->Mac[0], MacAddr, 6);
    } else {
      ZeroMem(&NetCardInfo->Mac[0], 6);
    }

    for(Index2 = 0; Index2 < HandleCount; Index2++){
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index2],
                      &gEfiPciIoProtocolGuid,
                      (VOID **)&PciIo2
                      );
      ASSERT_EFI_ERROR (Status);
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index2],
                      &gEfiDevicePathProtocolGuid,
                      (VOID **)&DevPath2
                      );
      PciIo2->GetLocation (PciIo2, &Seg2, &Bus2, &Dev2, &Fun2);
      if(!(Bus2==Bus && Dev2==Dev && Fun2!=Fun)){
        continue;
      }
      PortNum++;
      if(PortNum > ARRAY_SIZE(NetCardInfo->Mac)){
        break;
      }
      Status = GetOnboardLanMacAddress(gBS, DevPath2, MacAddr2);
      if(!EFI_ERROR(Status)){
        CopyMem(&NetCardInfo->Mac[PortNum-1], MacAddr2, 6);
      } else {
        ZeroMem(&NetCardInfo->Mac[PortNum-1], 6);
      }

    }
    NetCardInfo->Function = SET_LAN_INFO;
    NetCardInfo->DeviceNum = DeviceNumId;
    NetCardInfo->PresentStatus = 0;
    NetCardInfo->ControlType = 0;
    NetCardInfo->PCIEIndex = PciIndex-1;
    NetCardInfo->PortNum = PortNum;

    DEBUG ((DEBUG_INFO, "sizeof NetCardInfo = %d\n", sizeof (*NetCardInfo)));
    ResponseSize = sizeof (ResponseData);
    Status = EfiSendCommandToBMC (
               SM_BYOSOFT_NETFN_APP,
               SM_BYOSOFT_NETFN_SUB_FUN,
               (UINT8 *) NetCardInfo,
               sizeof(NETCARD_INFO_2_BMC),
               ResponseData,
               &ResponseSize
               );
    DEBUG ((DEBUG_INFO, "EfiSendCommandToBMC return %r\n", Status));
    DeviceNumId++;
  }
  FreePool (NetCardInfo);
  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }
}


VOID SendMiscInfo2Bmc()
{
  EFI_STATUS                            Status;
  UINT32                                TimeStamp;
  UINT8                                 ResponseData[2];
  UINT8                                 ResponseDataSize;
  IPMI_SEL_EVENT_RECORD_DATA            RecData;
  IPMI_SEL_EVENT_RECORD_DATA            *SelRecord = &RecData;
  PLAT_HOST_INFO_PROTOCOL               *PlatHost;
  HYGON_REPORT_UCE_PROTOCOL             *ReportUce;
  UINT32                                *MemPage;
  UINT32                                MemPageCount;
  UINT64                                *ApicId;
  UINTN                                 ApicIdSize;
  UINTN                                 Index;
  EXT_LOG_DATA_MEM_UCE                  MemUce;
  EXT_LOG_DATA_CPU_UCE                  CpuUce;
  UINT64                                MemAddress;
  UINT16                                CoreId;
  UINT32                                PageSize = SIZE_4KB;

  
  if(PcdGet64(PcdFirstVideoHostHandle) == 0){

    DEBUG((EFI_D_INFO, "SendElogEventNoVga\n"));
    GetTimeStampLib(&TimeStamp);

    SelRecord->RecordType   = IPMI_SEL_SYSTEM_RECORD;
    SelRecord->TimeStamp    = TimeStamp;
    SelRecord->GeneratorId  = PcdGet16(PcdIpmiSelRecordGeneratorId);
    SelRecord->EvMRevision  = IPMI_EVM_REVISION;
    SelRecord->SensorType   = 0x0F; // System Firmware Progress (formerly POST Error)
    SelRecord->SensorNumber = 0x0;
    SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
    SelRecord->OEMEvData1   = 0x80;
    SelRecord->OEMEvData2   = 0x0A; // No video device detected
    SelRecord->OEMEvData3   = 0x00;

    ResponseDataSize = sizeof(ResponseData);
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_STORAGE,
              EFI_STORAGE_ADD_SEL_ENTRY,
              (UINT8*)SelRecord,
              sizeof(IPMI_SEL_EVENT_RECORD_DATA),
              ResponseData,
              &ResponseDataSize
              );
    DEBUG((EFI_D_INFO, "EfiSendCommandToBMC:%r\n", Status));
    
  }


  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, (VOID**)&PlatHost);
  if(!EFI_ERROR(Status) && PlatHost->CpuMcVer == 0){
    DEBUG((EFI_D_INFO, "SendElogEventLoadMicrocodeError\n"));
    GetTimeStampLib(&TimeStamp);

    SelRecord->RecordType   = IPMI_SEL_SYSTEM_RECORD;
    SelRecord->TimeStamp    = TimeStamp;
    SelRecord->GeneratorId  = PcdGet16(PcdIpmiSelRecordGeneratorId);
    SelRecord->EvMRevision  = IPMI_EVM_REVISION;
    SelRecord->SensorType   = 0x07; // Processor
    SelRecord->SensorNumber = 0x0;
    SelRecord->EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;
    SelRecord->OEMEvData1   = 0x05; // Configuration Error
    SelRecord->OEMEvData2   = 0x00;
    SelRecord->OEMEvData3   = 0x00;

    ResponseDataSize = sizeof(ResponseData);
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_STORAGE,
              EFI_STORAGE_ADD_SEL_ENTRY,
              (UINT8*)SelRecord,
              sizeof(IPMI_SEL_EVENT_RECORD_DATA),
              ResponseData,
              &ResponseDataSize
              );
    DEBUG((EFI_D_INFO, "EfiSendCommandToBMC:%r\n", Status));    
  }

  Status = gBS->LocateProtocol (
                       &gHygonReportRasUceProtocolGuid,
                       NULL,
                       (VOID**)&ReportUce
                       );
  if (!EFI_ERROR (Status)) {
    Status = ReportUce->GetNewErr(ReportUce, &MemPage, &MemPageCount, &ApicId, &ApicIdSize);
    if (!EFI_ERROR (Status)) {
      GetTimeStampLib(&TimeStamp);
      if(MemPageCount){
        for(Index=0;Index<MemPageCount;Index++){
          MemUce.RecordType = UCE_ISOLATION_TYPE_MEM;
          MemAddress = LShiftU64(MemPage[Index], 12);
          CopyMem(&MemUce.TimeStamp, &TimeStamp, sizeof(TimeStamp));
          CopyMem(&MemUce.Address, &MemAddress, sizeof(MemUce.Address));
          CopyMem(&MemUce.Length, &PageSize, sizeof(MemUce.Length));
          ResponseDataSize = sizeof(ResponseData);
          Status = EfiSendCommandToBMC (
                    EFI_SM_NETFN_SENSOR,
                    BYO_SUBFUNC_EXTLOG,
                    (UINT8*)&MemUce,
                    sizeof(MemUce),
                    ResponseData,
                    &ResponseDataSize
                    );  
        }
      }
      for(Index=0;Index<64;Index++){
        if(ApicId[0] & LShiftU64(1, Index)){
          CoreId = (UINT16)Index;
          CpuUce.RecordType = UCE_ISOLATION_TYPE_CPU;
          CopyMem(&CpuUce.TimeStamp, &TimeStamp, sizeof(TimeStamp));
          CopyMem(&CpuUce.CoreId, &CoreId, sizeof(CpuUce.CoreId));
          ResponseDataSize = sizeof(ResponseData);
          Status = EfiSendCommandToBMC (
                    EFI_SM_NETFN_SENSOR,
                    BYO_SUBFUNC_EXTLOG,
                    (UINT8*)&CpuUce,
                    sizeof(CpuUce),
                    ResponseData,
                    &ResponseDataSize
                    );          
        }
      }
      if(ApicIdSize > 8){
        for(Index=0;Index<64;Index++){
          if(ApicId[1] & LShiftU64(1, Index)){
            CoreId = (UINT16)Index + 64;
            CpuUce.RecordType = UCE_ISOLATION_TYPE_CPU;
            CopyMem(&CpuUce.TimeStamp, &TimeStamp, sizeof(TimeStamp));
            CopyMem(&CpuUce.CoreId, &CoreId, sizeof(CpuUce.CoreId));
            ResponseDataSize = sizeof(ResponseData);
            Status = EfiSendCommandToBMC (
                      EFI_SM_NETFN_SENSOR,
                      BYO_SUBFUNC_EXTLOG,
                      (UINT8*)&CpuUce,
                      sizeof(CpuUce),
                      ResponseData,
                      &ResponseDataSize
                      );  
          }
        }
      }
    }
  }  
}



VOID
SendAllInfoToBmc(
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{  
  EFI_STATUS                   Status;
  EFI_SMBIOS_PROTOCOL          *Smbios;
  VOID                         *Interface;
  EFI_SM_CTRL_INFO             ControllerInfo;  
  UINT8                        ResponseDataSize;
  UINT32                       ManufacturerId;
  

  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->CloseEvent(Event);

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  ASSERT_EFI_ERROR (Status);

  ResponseDataSize = sizeof(ControllerInfo);
  Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              EFI_APP_GET_DEVICE_ID,
              NULL,
              0,
              (UINT8*)&ControllerInfo,
              &ResponseDataSize
              );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "GetDevId Err:%r\n", Status));
    return;
  }

  ManufacturerId = (ControllerInfo.ManufacturerId[2] << 16) | 
                   (ControllerInfo.ManufacturerId[1] << 8)  |
                   (ControllerInfo.ManufacturerId[0]);
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), j_d, ManufacturerId :0x%x.\n", __LINE__, ManufacturerId));

  if(ManufacturerId == 0xCBCC){
    SendBiosInfo2Bmc(Smbios);
    SendCpuInfo2Bmc(Smbios);
    SendDIMMInfo2Bmc(Smbios);
    SendHddInfo2Bmc();
    SendNvmeInfo2Bmc();
    SendPcieInfo2Bmc();
    SendNetCardInfo2Bmc();
    SendMiscInfo2Bmc();
  }

}

EFI_STATUS
SendInfoToBmcEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
)
{

  EFI_EVENT   Event;


  EfiCreateProtocolNotifyEvent (
    &gBdsAllDriversConnectedProtocolGuid,
    TPL_CALLBACK,
    SendAllInfoToBmc,
    NULL,
    &Event
    );


  return EFI_SUCCESS;
}


