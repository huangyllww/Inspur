
#include <Uefi.h>
#include <IndustryStandard/Pci.h>
#include <Protocol/PciIo.h>
#include <Protocol/NetworkInterfaceIdentifier.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/ByoCommLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/ProjectCommonLib.h>

/*
#include <AmdCpmDxe.h>
#include <AmdCpmDefine.h>
#include <ProjectConfiguration.h>
#include <ProjectSystemPassword.h>
*/

typedef struct {
  CHAR16            *FilePathString;
  CHAR16            *Description;
} UEFI_OS_BOOT_FILE;

UEFI_OS_BOOT_FILE  mUefiOsBootFiles[] = {
  {L"\\EFI\\ctyunos\\shimx64.efi",               L"CTyunOS"},
  {L"\\EFI\\ctyunos\\grubx64.efi",               L"CTyunOS"},
  {L"\\EFI\\kos\\shimx64-kos.efi",               L"kos"},
  {L"\\EFI\\kos\\grubx64.efi",                   L"kos"},
  {L"\\EFI\\euleros\\shimx64.efi",               L"EulerOS"},
  {L"\\EFI\\openEuler\\shimx64.efi",             L"EulerOS"},
  {L"\\EFI\\onie\\grubx64.efi",                  L"Onie"},
  {L"\\EFI\\Microsoft\\Boot\\bootmgfw.efi",      L"Windows Boot Manager"},
  {L"\\EFI\\centos\\shimx64-centos.efi",         L"CentOS Linux"},
  {L"\\EFI\\centos\\shimx64.efi",                L"CentOS Linux"},
  {L"\\EFI\\centos\\shim.efi",                   L"CentOS Linux"},
  {L"\\EFI\\centos\\grubx64.efi",                L"CentOS Linux"},
  {L"\\EFI\\centos\\grubx64.efi",                L"CentOS Linux"},
  {L"\\EFI\\uos\\shimx64.efi",                   L"UOS"},
  {L"\\EFI\\UnionTech\\shimx64.efi",             L"UOS"},
  {L"\\EFI\\ubuntu\\shimx64.efi",                L"Ubuntu Linux"},
  {L"\\EFI\\ubuntu\\grubx64.efi",                L"Ubuntu Linux"},
  {L"\\EFI\\redhat\\shimx64-redhat.efi",         L"Red Hat Linux"},
  {L"\\EFI\\redhat\\shimx64.efi",                L"Red Hat Linux"},
  {L"\\EFI\\redhat\\shim.efi",                   L"Red Hat Linux"},
  {L"\\EFI\\redhat\\grubx64.efi",                L"Red Hat Linux"},
  {L"\\EFI\\redhat\\grub.efi",                   L"Red Hat Linux"},
  {L"\\EFI\\neokylin\\shimx64.efi",              L"Kylin Linux"},
  {L"\\EFI\\kylin\\shimx64.efi",                 L"Kylin Linux"},
  {L"\\EFI\\kylin\\grubx64.efi",                 L"Kylin Linux"},
  {L"\\EFI\\tencent\\grubx64.efi",               L"Tencent OS tlinux"},
  {L"\\EFI\\tencent\\grub.efi",                  L"Tencent OS tlinux 1.2"},
  {L"\\EFI\\alios\\shimx64-alios.efi",           L"AliOS"},
  {L"\\EFI\\alios\\grubx64.efi",                 L"AliOS"},
  {L"\\EFI\\alios\\shimx64.efi",                 L"Alibaba Cloud Linux"},
  {L"\\EFI\\alinux\\shimx64.efi",                L"Alibaba Cloud Linux"},
  {L"\\EFI\\alinux\\shimx64-alinux.efi",         L"Alibaba Cloud Linux"},
  {L"\\EFI\\alinux\\shim.efi",                   L"Alibaba Cloud Linux"},
  {L"\\EFI\\Alibaba-UEFI\\grubx64.efi",          L"Alibaba Cloud Linux"},
  {L"\\EFI\\anolis\\shim.efi",                   L"AnoLis OS"},
  {L"\\EFI\\anolis\\shimx64.efi",                L"AnoLis OS"},
  {L"\\EFI\\sles12\\shim.efi",                   L"SLES 12"},
  {L"\\EFI\\sles12\\grubx64.efi",                L"SLES 12"},
  {L"\\EFI\\sles\\shim.efi",                     L"SLES 11sp3"},
  {L"\\EFI\\opensuse\\shim.efi",                 L"OpenSUSE"},
  {L"\\EFI\\opensuse\\grubx64.efi",              L"OpenSUSE"},
  {L"\\EFI\\debian\\shimx64.efi",                L"Debian"},
  {L"\\EFI\\debian\\grubx64.efi",                L"Debian"},
  {L"\\EFI\\VMware\\mboot32.efi",                L"Vmware"},
  {L"\\EFI\\fedora\\shim.efi",                   L"Fedora"},
  {L"\\EFI\\fedora\\shimx64.efi",                L"Fedora"},
  {L"\\EFI\\fedora\\shimx64-fedora.efi",         L"Fedora"},
  {L"\\EFI\\linx\\grubx64.efi" ,                 L"linx"},
  {L"\\boot\\efi\\EFI\\BOOT\\BOOTX64.EFI",       L"linx"},
  {L"\\EFI\\anolis\\shimx64-anolios.efi",        L"Anolis"},
  {L"\\EFI\\anolis\\grubx64.efi",                L"Anolis"},
  {L"\\EFI\\bclinux\\grubx64.efi",               L"BigCloud Enterprise Linux"},
  {L"\\EFI\\GRUB2\\grubx64.efi ",                L"GRUB2"},  
  {L"\\EFI\\velinx\\grubx64.efi" ,               L"veLinx"},
  {L"\\efi\\boot\\bootx64.efi",                  L"Rocky OS"},
  
  {EFI_REMOVABLE_MEDIA_FILE_NAME,                NULL}, // Put it in this place, the new OS needs to be on top of it
  {NULL,                                         NULL}
  };

BOOLEAN
IsOsDiskByDescription (
  IN  CHAR16    *OsDescription
  )
{
  UINTN    Index;

  if(OsDescription == NULL){
    return FALSE;
  }

  Index = 0;
  while (mUefiOsBootFiles[Index].Description != NULL) {
    if (StrStr(OsDescription, mUefiOsBootFiles[Index].Description) != NULL) {
      return TRUE;
    }
    Index++;
  }
  return FALSE;
}


BOOLEAN
IsMyDevicePathEnd (
  IN EFI_DEVICE_PATH_PROTOCOL    *Dp
  );


/**
  Check PciIo and input boot option description, if sub-vendor-id is INSPUR_VENDOR_ID(0x1BD4) and
  description has vendor-id's vendor name, then replace it with "Inspur".

  @param  PciIo           PciIo of PCI network device.
  @param  DescriptionStr  Original boot option description.

  @return The string modified by this function or original description.

**/
CHAR16 *
EFIAPI
GetInspurNetworkCardName (
  IN EFI_PCI_IO_PROTOCOL  *PciIo,
  IN CHAR16               *DescriptionStr
)
{
  EFI_STATUS  Status = EFI_SUCCESS;
  UINT32      DidVid, SubDidVid;
  UINTN       Size;
  CHAR16      *VendorName;
  CHAR16      *VendorNameSearch;
  UINTN       VendorNameLength;
  CHAR16      *NewDescriptionStr;
  UINT16      VendorId;
  UINTN       SearchIndex;
  CHAR16      *BeforeVendorNameStr;

  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0, 1, &DidVid);
  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, 0x2C, 1, &SubDidVid);

  if ((SubDidVid & 0xFFFF) != INSPUR_VENDOR_ID) {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  VendorId = DidVid & 0xFFFF;

  if (VendorId == INTEL_VENDOR_ID) {
    VendorName = L"Intel";
  } else if (VendorId == WANGXUN_VENDOR_ID) {
    VendorName = L"WangXun";
  } else {
    Status = EFI_UNSUPPORTED;
    goto Exit;
  }

  VendorNameLength = StrLen (VendorName);
  VendorNameSearch = StrStr (DescriptionStr, VendorName);
  if (VendorNameSearch != NULL) {
    Size = StrSize (DescriptionStr) - StrSize (VendorName) + StrSize (L"Inspur") + sizeof (CHAR16);
    NewDescriptionStr = (CHAR16 *) AllocateZeroPool (Size);
    SearchIndex =  StrLen (DescriptionStr) - StrLen (VendorNameSearch);
    if (SearchIndex > 0) {
      BeforeVendorNameStr = AllocateZeroPool (SearchIndex + sizeof (CHAR16));
      StrnCpyS (BeforeVendorNameStr, SearchIndex + 1, DescriptionStr, SearchIndex);
      UnicodeSPrint (NewDescriptionStr, Size, L"%s%s%s", BeforeVendorNameStr, L"Inspur", VendorNameSearch + VendorNameLength);
      FreePool (BeforeVendorNameStr);
    } else {
      UnicodeSPrint (NewDescriptionStr, Size, L"%s%s", L"Inspur", VendorNameSearch + VendorNameLength);
    }
  } else {
    Status = EFI_UNSUPPORTED;
  }

Exit:
  if (Status == EFI_UNSUPPORTED) {
    NewDescriptionStr = (CHAR16 *) AllocateCopyPool (StrSize (DescriptionStr), DescriptionStr);
  }

  return NewDescriptionStr;
}


/**
  Get startlane from pci device slot number.

  @param  SlotNumber           Slot number of PCI device.

  @return The startlane number of the input pci device slot number.

**/
/*
UINT16
EFIAPI
GetStartLanBySlotNumber (
  IN UINT16 SlotNumber
)
{
  EFI_STATUS                      Status;
  AMD_CPM_TABLE_PROTOCOL          *CpmTableProtocolPtr;
  AMD_CPM_DXIO_TOPOLOGY_TABLE     *DxioTopologyTablePtr;
  UINTN                           Index, CpuCount;
  DXIO_PORT_DESCRIPTOR            *Port;
  UINT16                          StartLane;
  PLATFORM_COMM_INFO              *PlatformCommInfo;
  UINT32                          Socket1LaneMask;

  StartLane = SlotNumber;

  PlatformCommInfo = GetPlatformCommInfo();

  if (PlatformCommInfo->Dies == 4) {
    Socket1LaneMask = BIT7;
  } else {
    Socket1LaneMask = BIT6;
  }

  Status = gBS->LocateProtocol (
                  &gAmdCpmTableProtocolGuid,
                  NULL,
                  (VOID**)&CpmTableProtocolPtr
                  );
  if (EFI_ERROR (Status)) {
    return StartLane;
  }

  CpuCount = PcdGet8 (PcdAmdNumberOfPhysicalSocket);
  DxioTopologyTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr2 (CpmTableProtocolPtr, CPM_SIGNATURE_DXIO_TOPOLOGY);
  for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++) {
    Port = &DxioTopologyTablePtr->Port[Index];
    if (Port->Port.SlotNum == SlotNumber) {
      StartLane = (UINT16)Port->EngineData.StartLane;
      DEBUG ((DEBUG_INFO, "GetStartLanBySlotNumber(S0) Slot number is %d, StartLane is %d\n", Port->Port.SlotNum, StartLane));
    }
    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }
  }
  if (CpuCount == 2) {
    DxioTopologyTablePtr = CpmTableProtocolPtr->CommonFunction.GetTablePtr2 (CpmTableProtocolPtr, CPM_SIGNATURE_DXIO_TOPOLOGY_S1);
    for (Index = 0; Index < AMD_DXIO_PORT_DESCRIPTOR_SIZE; Index ++) {
      Port = &DxioTopologyTablePtr->Port[Index];
      if (Port->Port.SlotNum == SlotNumber) {
        StartLane = (UINT16)Port->EngineData.StartLane;
        StartLane |= Socket1LaneMask;
        DEBUG ((DEBUG_INFO, "GetStartLanBySlotNumber(S1) Slot number is %d, StartLane is %d\n", Port->Port.SlotNum, StartLane));
      }
      if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
        break;
      }
    }
  }

  return StartLane;
}
*/

UINT16
EFIAPI
InspurHygonGetPciSlotNum (
  IN  EFI_BOOT_SERVICES          *BS,
  IN  EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS                Status;
  EFI_HANDLE                DriverHandle;
  EFI_PCI_IO_PROTOCOL       *PciIo;
  EFI_DEVICE_PATH_PROTOCOL  *BridgeDp = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *DpTemp;
  UINT8                     ClassCode[3];
  UINT16                    VendorId;
  UINT16                    SlotNum = 0;
  UINT8                     CapabilityPtr;
  UINT16                    CapabilityEntry;
  UINT32                    SlotCap;
  BOOLEAN                   BridgeDevice = FALSE;
  UINTN                     DpSize;
  

  if(BS == NULL || DevicePath == NULL){
    return 0;
  }

  DpSize = MyGetDevicePathSize(DevicePath);
  if(DpSize == 0){
    return 0;
  }
  Status = BS->AllocatePool(EfiBootServicesData, DpSize, (VOID**)&BridgeDp);
  if(EFI_ERROR(Status)){
    return 0;
  }
  CopyMem(BridgeDp, DevicePath, DpSize);

  while(!IsMyDevicePathEnd(BridgeDp)) {
    DpTemp = BridgeDp;
    Status = BS->LocateDevicePath (
                   &gEfiPciIoProtocolGuid,
                   &DpTemp,
                   &DriverHandle
                   );
    if (!EFI_ERROR (Status)) {
      Status = BS->HandleProtocol(
                    DriverHandle,
                    &gEfiPciIoProtocolGuid,
                    (VOID**)&PciIo
                    );
      if (!EFI_ERROR (Status)) {
        PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, ClassCode);
        PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, PCI_VENDOR_ID_OFFSET, 1, &VendorId);
        if(ClassCode[2] == 6 && ClassCode[1] == 4 && ClassCode[0] == 0 && VendorId == 0x1D94) {
          BridgeDevice = TRUE;
          break;
        }
      }
    }

    DpTemp = BridgeDp;
    BridgeDp = GetPciParentDp(BS, BridgeDp);
    BS->FreePool(DpTemp);
    if(BridgeDp == NULL){
      break;
    }
  }

  if(BridgeDp != NULL){
    BS->FreePool(BridgeDp);
  }

  if(!BridgeDevice){
    return 0;
  }

  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);

  while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
    PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapabilityPtr, 1, &CapabilityEntry);
    if ((UINT8)CapabilityEntry == EFI_PCI_CAPABILITY_ID_PCIEXP) {
      PciIo->Pci.Read (PciIo, EfiPciIoWidthUint32, CapabilityPtr + 0x14, 1, &SlotCap);
      SlotNum = (UINT16)((SlotCap & 0xfff80000) >> 19);
      break;
    }
    CapabilityPtr = (UINT8)(CapabilityEntry >> 8);
  }

  return SlotNum;
}


CHAR8*
EFIAPI
SmbiosGetDimmDeviceTypeStringInFormFactor (
  UINT8                      FormFactor,
  MEMORY_DEVICE_TYPE_DETAIL  TypeDetail
  )
{
  CHAR8  *BaseModuleType;
  CHAR8  *MemDeviceType;
  UINTN  StrSize;

  if(TypeDetail.Registered == 1 && TypeDetail.LrDimm == 0 && FormFactor == 9) {
    BaseModuleType = "RDIMM";
  } else if (TypeDetail.Unbuffered == 1 && FormFactor == 9) {
    BaseModuleType = "UDIMM";
  } else if (TypeDetail.Registered == 1 && TypeDetail.LrDimm == 1 && FormFactor == 9) {
    BaseModuleType = "LDIMM";
  } else if (TypeDetail.Unbuffered == 1 && FormFactor == 13) {
    BaseModuleType = "SO_DIMM";
  } else {
    BaseModuleType = "other";
  }

  StrSize = AsciiStrSize (BaseModuleType);
  MemDeviceType = AllocateZeroPool (StrSize);
  AsciiStrCpyS (MemDeviceType, StrSize, BaseModuleType);

  return MemDeviceType;
}


BOOLEAN 
InspurLibGetMacFromNiiDp (  
  IN  EFI_BOOT_SERVICES         *pBS,
  IN  VOID                      *Dp,
  OUT UINT8                     MacAddr[6]  
  )
{
  EFI_STATUS                Status;
  UINTN                     Index;
  UINTN                     HandleCount;
  EFI_HANDLE                *HandleBuffer = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *NiiDp;
  UINTN                     DpSize;
  MAC_ADDR_DEVICE_PATH      *MacDevPath;


  if(pBS == NULL || Dp == NULL){
    return FALSE;
  }

  DpSize = MyGetDevicePathSize(Dp);
  if(DpSize <= sizeof(EFI_DEVICE_PATH_PROTOCOL)){
    return FALSE;
  }

  Status = pBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = pBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&NiiDp
                    );
    if(EFI_ERROR(Status)){
      continue;
    }
    
    if(CompareMem(Dp, NiiDp, DpSize-sizeof(EFI_DEVICE_PATH_PROTOCOL)) != 0){
      continue;
    }

    while (!IsMyDevicePathEnd(NiiDp)) {
      if (((EFI_DEVICE_PATH_PROTOCOL*)NiiDp)->Type == MESSAGING_DEVICE_PATH &&
          ((EFI_DEVICE_PATH_PROTOCOL*)NiiDp)->SubType == MSG_MAC_ADDR_DP) {
        MacDevPath = (MAC_ADDR_DEVICE_PATH*)NiiDp;
        CopyMem(MacAddr, MacDevPath->MacAddress.Addr, 6);
        FreePool(HandleBuffer);
        return TRUE;
      }

      NiiDp = NextDevicePathNode(NiiDp);
    }    
  }

  if(HandleBuffer != NULL){
    FreePool(HandleBuffer);
  }
  return FALSE;
}


VOID InspurSwapLanMacAddress(UINT8 *MacAddr)
{
  UINT8  d[6];

  CopyMem(d, MacAddr, 6);
  MacAddr[0] = d[5];
  MacAddr[1] = d[4];
  MacAddr[2] = d[3];
  MacAddr[3] = d[2];
  MacAddr[4] = d[1];
  MacAddr[5] = d[0];
}


EFI_STATUS
EFIAPI
InspurGetPciNicMac (
  IN  EFI_PCI_IO_PROTOCOL    *PciIo,
  IN  VOID                   *Dp,
  OUT UINT8                  *MacAddr
  )
{
  EFI_STATUS    Status;
  UINT16        VenderId, DeviceId;
  UINT8         ClassCode[3];
  UINT32        Data32 = 0;
  UINT8         CapabilityID = 0, CapabilityPtr = 0;
  UINT16        CapabilityEntry = 0;
  UINT16        GetMac = 0xf, Init = 0x4;
  UINTN         ExtPciSerialNumberSize;
  UINT64        SerialNumber = 0x0;
  UINTN         Seg, Bus, Dev, Fun;
  PCI_EXPRESS_EXTENDED_CAPABILITIES_SERIAL_NUMBER  ExtPciSerialNumber;
  EFI_DEVICE_PATH_PROTOCOL                         *DevPath;

  if (PciIo == NULL ||MacAddr == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  DEBUG ((DEBUG_INFO, "InspurGetPciNicMac()\n"));

  DevPath = (EFI_DEVICE_PATH_PROTOCOL*)Dp;
  ShowDevicePathDxe(gBS, DevPath);

  if(InspurLibGetMacFromNiiDp(gBS, Dp, MacAddr)){
    DEBUG((EFI_D_INFO, "MacFromNii: %02X-%02X-%02X-%02X-%02X-%02X\n", \
      MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]));
    return EFI_SUCCESS;
  }

  PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 9, 3, &ClassCode);
  if ( ClassCode[2] != PCI_CLASS_NETWORK) {
    return EFI_NOT_FOUND;
  }

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0, 1, &VenderId);
  if (VenderId == 0x1D94) { // skip bridge + Base system peripherals + Encryption/Decryption controllers
    return EFI_NOT_FOUND;
  }
  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0x02, 1, &DeviceId);

  Status = EFI_SUCCESS;
  switch (VenderId) {
    case 0x10EC:
      Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_IO, NULL);
      Status = PciIo->Io.Read(PciIo, EfiPciIoWidthUint8, 0, 0, 6, MacAddr);
      break;

    case 0x8086:
      Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
      if (DeviceId == 0x1557 || DeviceId == 0x10fb || DeviceId == 0x1528 || DeviceId == 0x1563) {
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x5400, 6, MacAddr);
      } else if (DeviceId == 0x1572 || DeviceId == 0x1583 || DeviceId == 0x1584 || DeviceId == 0x15FF) {
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e4440, 4, MacAddr);
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e44c0, 2, &MacAddr[4]);
      } else if (DeviceId == 0x159b || DeviceId == 0x1592) {
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e3b20, 4, MacAddr);
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x001e3ba0, 2, &MacAddr[4]);
      } else {
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x40, 6, MacAddr);
      }
      break;

    case 0x14E4:
      if (DeviceId == 0x168E) {  //NetXtreme II Gigabit Ethernet
        Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x8010, 4, &Data32);
        MacAddr[0] = (UINT8)(Data32>>8);
        MacAddr[1] = (UINT8)Data32;
        Data32 = 0;
        Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x8014, 4, &Data32);
        MacAddr[2] = (UINT8)(Data32 >> 24);
        MacAddr[3] = (UINT8)(Data32>>16);
        MacAddr[4] = (UINT8)(Data32>>8);
        MacAddr[5] = (UINT8)Data32;
      } else if (DeviceId == 0x16D7) { // Inspur_Haydn_BCM_25G_LC
        ExtPciSerialNumberSize = sizeof (PCI_EXPRESS_EXTENDED_CAPABILITIES_SERIAL_NUMBER);
        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, EFI_PCIE_CAPABILITY_BASE_OFFSET, ExtPciSerialNumberSize, &ExtPciSerialNumber);
        while (ExtPciSerialNumber.Header.CapabilityId != 0 && ExtPciSerialNumber.Header.CapabilityVersion != 0 && ExtPciSerialNumber.Header.CapabilityId != 0xFFFF) {
          if (PCI_EXPRESS_EXTENDED_CAPABILITY_SERIAL_NUMBER_ID == ExtPciSerialNumber.Header.CapabilityId) {
            SerialNumber = ExtPciSerialNumber.SerialNumber;
            DEBUG ((DEBUG_INFO, "SerialNumber: 0x%lx\n", SerialNumber));
            break;
          }
          Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, ExtPciSerialNumber.Header.NextCapabilityOffset, ExtPciSerialNumberSize, &ExtPciSerialNumber);
        }

        if (SerialNumber != 0x0) {
          MacAddr[0] = (UINT8)BitFieldRead64 (SerialNumber, 56, 63);
          MacAddr[1] = (UINT8)BitFieldRead64 (SerialNumber, 48, 55);
          MacAddr[2] = (UINT8)BitFieldRead64 (SerialNumber, 40, 47);
          MacAddr[3] = (UINT8)BitFieldRead64 (SerialNumber, 16, 23);
          MacAddr[4] = (UINT8)BitFieldRead64 (SerialNumber, 8,  15);
          MacAddr[5] = (UINT8)BitFieldRead64 (SerialNumber, 0,  7);
          PciIo->GetLocation (PciIo, &Seg, &Bus, &Dev, &Fun);
          if (Fun > 0) {
            MacAddr[5] += (UINT8)Fun;
          }
        }
      }
      break;

    case 0x8088:
      Status = PciIo->Attributes(PciIo, EfiPciIoAttributeOperationEnable, EFI_PCI_IO_ATTRIBUTE_MEMORY, NULL);
      Status = PciIo->Mem.Read(PciIo, EfiPciIoWidthUint8, 0, 0x16200, 6, MacAddr);
      InspurSwapLanMacAddress (MacAddr);
      break;

    case 0x15b3:
      Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, PCI_CAPBILITY_POINTER_OFFSET, 1, &CapabilityPtr);
      while ((CapabilityPtr >= 0x40) && ((CapabilityPtr & 0x03) == 0x00)) {
        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint16, CapabilityPtr, 1, &CapabilityEntry);
        CapabilityID = (UINT8) CapabilityEntry;
        if (0x9 == CapabilityID) {
          Status = PciIo->Pci.Write(PciIo, EfiPciIoWidthUint16, CapabilityPtr + 0x04, 1, &GetMac);
          Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, CapabilityPtr + 0x10, 4, &Data32);
          MacAddr[0] = (UINT8)(Data32>>8);
          MacAddr[1] = (UINT8)Data32;
          Data32 = 0;
          Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, CapabilityPtr + 0x14, 4, &Data32);
          MacAddr[2] = (UINT8)(Data32 >> 24);
          MacAddr[3] = (UINT8)(Data32>>16);
          MacAddr[4] = (UINT8)(Data32>>8);
          MacAddr[5] = (UINT8)Data32;
          Status = PciIo->Pci.Write(PciIo, EfiPciIoWidthUint16, CapabilityPtr + 0x04, 1, &Init);
          break;
        }
        CapabilityPtr = (UINT8)(CapabilityEntry >> 8);
      }
      break;

    case 0x1924: // Solarflare
      ExtPciSerialNumberSize = sizeof (PCI_EXPRESS_EXTENDED_CAPABILITIES_SERIAL_NUMBER);
      Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, EFI_PCIE_CAPABILITY_BASE_OFFSET, ExtPciSerialNumberSize, &ExtPciSerialNumber);
      while (ExtPciSerialNumber.Header.CapabilityId != 0 && ExtPciSerialNumber.Header.CapabilityVersion != 0 && ExtPciSerialNumber.Header.CapabilityId != 0xFFFF) {
        if (PCI_EXPRESS_EXTENDED_CAPABILITY_SERIAL_NUMBER_ID == ExtPciSerialNumber.Header.CapabilityId) {
          SerialNumber = ExtPciSerialNumber.SerialNumber;
          DEBUG ((DEBUG_INFO, "SerialNumber: 0x%lx\n", SerialNumber));
          break;
        }
        Status = PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8, ExtPciSerialNumber.Header.NextCapabilityOffset, ExtPciSerialNumberSize, &ExtPciSerialNumber);
      }

      if (SerialNumber != 0x0) {
        MacAddr[0] = (UINT8)BitFieldRead64 (SerialNumber, 56, 63);
        MacAddr[1] = (UINT8)BitFieldRead64 (SerialNumber, 48, 55);
        MacAddr[2] = (UINT8)BitFieldRead64 (SerialNumber, 40, 47);
        MacAddr[3] = (UINT8)BitFieldRead64 (SerialNumber, 16, 23);
        MacAddr[4] = (UINT8)BitFieldRead64 (SerialNumber, 8,  15);
        MacAddr[5] = (UINT8)BitFieldRead64 (SerialNumber, 0,  7);
        PciIo->GetLocation (PciIo, &Seg, &Bus, &Dev, &Fun);
        if (Fun > 0) {
          MacAddr[5] += (UINT8)Fun;
        }
      }

      break;

    default:
      Status = EFI_NOT_FOUND;
      break;
  }

  DEBUG ((DEBUG_INFO, "MacAddrFromPci:%x-%x-%x-%x-%x-%x.\n", MacAddr[0], MacAddr[1], MacAddr[2], MacAddr[3], MacAddr[4], MacAddr[5]));
  return Status;
}



EFI_STATUS
EFIAPI
GetPciNetCardIfNum (
  IN  EFI_PCI_IO_PROTOCOL          *PciIo,
  IN  EFI_DEVICE_PATH_PROTOCOL     *DpIn,
  OUT UINT16                       *MaxIfNumber,
  OUT UINT16                       *IfNumber
  )
{
  EFI_STATUS                                 Status;
  UINTN                                      Index;
  UINTN                                      HandleCount;
  EFI_HANDLE                                 *HandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL                   *DevicePath;
  EFI_HANDLE                                 NiiHandle;
  EFI_HANDLE                                 PciHandle;
  EFI_PCI_IO_PROTOCOL                        *TempPciIo;
  EFI_NETWORK_INTERFACE_IDENTIFIER_PROTOCOL  *Nii = NULL;
  UINT16                                     MaxIfNum = 0;
  UINT16                                     IfNum = 0;
  BOOLEAN                                    FoundIfNum = FALSE;

  if ((PciIo == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiNetworkInterfaceIdentifierProtocolGuid_31,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  Status = gBS->LocateDevicePath (&gEfiNetworkInterfaceIdentifierProtocolGuid_31, &DpIn, &NiiHandle);
  DEBUG ((DEBUG_INFO, "Status of NiiHandle is %r\n", Status));
  if (!EFI_ERROR(Status)) {
    DEBUG ((DEBUG_INFO, "Get Nii IfNumber: "));
    gBS->HandleProtocol(NiiHandle, &gEfiNetworkInterfaceIdentifierProtocolGuid_31, (VOID**)&Nii);
    if (Nii != NULL) {
      DEBUG ((DEBUG_INFO, "Nii->IfNum: %d\n", Nii->IfNum));
      IfNum = Nii->IfNum;
      FoundIfNum = TRUE;
    }
  }

  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID**)&DevicePath
                      );            
    if (EFI_ERROR(Status)) {
      continue;
    }

    Status = gBS->LocateDevicePath (
                    &gEfiPciIoProtocolGuid,
                    &DevicePath,
                    &PciHandle
                    );
    if (EFI_ERROR(Status)) {
      continue;
    }
    
    Status = gBS->HandleProtocol (
                    PciHandle,
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &TempPciIo
                    );                   
    if (EFI_ERROR(Status)) {                          
      continue;
    }
    if (TempPciIo == PciIo) { // Different IfNums have same PciIo
      Nii = NULL;
      Status = gBS->HandleProtocol(HandleBuffer[Index], &gEfiNetworkInterfaceIdentifierProtocolGuid_31, (VOID**)&Nii);
      if (Nii != NULL && Nii->IfNum > MaxIfNum) {
        MaxIfNum = Nii->IfNum;
      }
      continue;
    }
  }

  if (FoundIfNum) {
    *MaxIfNumber = MaxIfNum + 1;
    *IfNumber    = IfNum;
    DEBUG ((DEBUG_INFO, "Found IfNumber, IfNum: %d, MaxIfNum: %d\n", *IfNumber, *MaxIfNumber));
    return EFI_SUCCESS;
  } else {
    *MaxIfNumber = 1;
    *IfNumber    = 0;
    return EFI_NOT_FOUND;
  }
}

// mucse vendor id
#define MUCSE_VENDOR_ID                                  0x8848


UINT8
EFIAPI
InspurGetPciPortNum (
  IN  EFI_PCI_IO_PROTOCOL          *PciIo,
  IN  EFI_DEVICE_PATH_PROTOCOL     *FilePath
  )
{
  EFI_STATUS                Status;
  UINTN                     PciSegment;
  UINTN                     PciBus;
  UINTN                     PciDevice;
  UINTN                     PciFunction;
  UINT8                     PortNum = 0xFF;
  UINT16                    VendorId;
  UINT16                    MaxIfNum = 1;
  UINT16                    IfNum = 0;

  if (PciIo == NULL) {
    return PortNum;
  }

  Status = PciIo->GetLocation(PciIo, &PciSegment, &PciBus, &PciDevice, &PciFunction);
  if(!EFI_ERROR (Status)) {
    PortNum = (UINT8)PciFunction ;
  }

  Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint16, 0, 1, &VendorId);
  if (!EFI_ERROR (Status) && VendorId == MUCSE_VENDOR_ID && PortNum != 0xFF) {
    Status = GetPciNetCardIfNum(PciIo, FilePath, &MaxIfNum, &IfNum);
    DEBUG ((DEBUG_INFO, "Status: %r, MaxIfNum: %d, IfNum: %d\n", Status, MaxIfNum, IfNum));
    if (EFI_ERROR (Status)) {
      MaxIfNum = 1;
      IfNum = 0;
    }
    PortNum = PortNum * (UINT8)MaxIfNum + (UINT8)IfNum;
  }

  return PortNum;
}


BOOLEAN
EFIAPI
CheckPasswordMatch (
  CHAR16  *Password,
  UINT8   CheckType
  )
{
  UINTN   i;
  UINTN   Length;

  Length = StrLen(Password);

  switch(CheckType) {
    case 0:
      for(i = 0; i < Length; i++) {
        if((Password[i] >= '0') && (Password[i] <= '9')) {
          //DEBUG((EFI_D_INFO, "CheckType0: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    case 1:
      for(i = 0; i < Length; i++) {
        if((Password[i] >= 'A') && (Password[i] <= 'Z')) {
          //DEBUG((EFI_D_INFO, "CheckType1: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    case 2:
      for(i = 0; i < Length; i++) {
        if(((Password[i] >= ' ') && (Password[i] <= '/')) ||
           ((Password[i] >= ':') && (Password[i] <= '@')) ||
           ((Password[i] >= '[') && (Password[i] <= '`')) ||
           ((Password[i] >= '{') && (Password[i] <= '~'))) {
          //DEBUG((EFI_D_INFO, "CheckType2: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    case 3:
      for(i = 0; i < Length; i++) {
        if((Password[i] >= 'a') && (Password[i] <= 'z')) {
          //DEBUG((EFI_D_INFO, "CheckType3: Password[%d] = %c\n",i,Password[i]));
          return TRUE;
        }
      }
      break;
    default:
      return FALSE;
      break;
  }
  return FALSE;
}

#define MIN_PASSWORD_COMPLEXITY    3

BOOLEAN
EFIAPI
VerifyPasswordComplexity (
  CHAR16   *Password,
  BOOLEAN  Check
  )
{
  UINT16 i;
  UINT16 j;
  UINT16 k;
  UINT16 n;

  if (!Check) {
    return TRUE;
  }
  
  i = CheckPasswordMatch(Password,0);
  j = CheckPasswordMatch(Password,1);
  k = CheckPasswordMatch(Password,2);
  n = CheckPasswordMatch(Password,3);
  
  if((i + j + n >= MIN_PASSWORD_COMPLEXITY)&&(k >= 1)) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/*
VOID*
EFIAPI
GetSetupData2HobData (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *SetupData2;

  GuidHob.Raw = GetFirstGuidHob(&gPlatformSetupVariable2Guid);
  ASSERT(GuidHob.Raw != NULL);
  SetupData2 = (VOID*)(GuidHob.Guid+1);

  return SetupData2;
}
*/

typedef struct{
  UINT8   BaseClass;
  UINT8   SubClass;
  UINT8   Interface;
  CHAR16  *Name;
} PROJECT_PCI_CLASS_INFO;

PROJECT_PCI_CLASS_INFO mPciClassCodeNameTable[] = {
  {0x00, 0x00, 0x00, L"compatible"},
  {0x00, 0x01, 0x00, L"VGA-compatible"},
  
  {0x01, 0x00, 0x00, L"SCSI bus controller"},
  {0x01, 0x01, 0xFF, L"IDE controller"},
  {0x01, 0x02, 0x00, L"Floppy disk controller"},
  {0x01, 0x03, 0x00, L"IPI bus controller"},
  {0x01, 0x04, 0x00, L"RAID controller"},
  {0x01, 0x05, 0x20, L"ATA controller with single DMA"},
  {0x01, 0x05, 0x30, L"ATA controller with chained DMA"},
  {0x01, 0x06, 0x00, L"SATA DPA"},
  {0x01, 0x06, 0x01, L"SATA AHCI"}, 
  {0x01, 0x08, 0x02, L"NVMe"},
  {0x01, 0x80, 0x00, L"Other mass storage controller"},
  
  {0x02, 0x00, 0x00, L"Ethernet controller"},
  {0x02, 0x01, 0x00, L"Token Ring controller"},
  {0x02, 0x02, 0x00, L"FDDI controller"},
  {0x02, 0x03, 0x00, L"ATM controller"},
  {0x02, 0x04, 0x00, L"ISDN controller"},
  {0x02, 0x05, 0x00, L"WorldFip controller"},
  {0x02, 0x06, 0xFF, L"PICMG 2.14 Multi Computing"},
  {0x02, 0x80, 0x00, L"Other network controller"},
  
  {0x03, 0x00, 0x00, L"VGA controller"},
  {0x03, 0x00, 0x01, L"8514-compatible controller"},
  {0x03, 0x01, 0x00, L"XGA controller"},
  {0x03, 0x02, 0x00, L"3D controller"},
  {0x03, 0x80, 0x00, L"Other display controller"},
  
  {0x04, 0x00, 0x00, L"Video device"},
  {0x04, 0x01, 0x00, L"Audio device"},
  {0x04, 0x02, 0x00, L"Computer telephony device"},
  {0x04, 0x80, 0x00, L"Other multimedia device"},
  
  {0x05, 0x00, 0x00, L"RAM"},
  {0x05, 0x01, 0x00, L"Flash"},
  {0x05, 0x80, 0x00, L"Other memory controller"},
  
  {0x06, 0x00, 0x00, L"Host bridge"},
  {0x06, 0x01, 0x00, L"ISA bridge"},
  {0x06, 0x02, 0x00, L"EISA bridge"},
  {0x06, 0x03, 0x00, L"MCA bridge"},
  {0x06, 0x04, 0x00, L"P2P bridge"},
  {0x06, 0x04, 0x01, L"SubDecode P2P bridge"},
  {0x06, 0x05, 0x00, L"PCMCIA bridge"},
  {0x06, 0x06, 0x00, L"NuBus bridge"},
  {0x06, 0x07, 0x00, L"CardBus bridge"},
  {0x06, 0x08, 0xFF, L"RACEway bridge"},
  {0x06, 0x09, 0x40, L"Semi-transparent P2P bridge with the primary PCI bus facing processor"},
  {0x06, 0x09, 0x80, L"Semi-transparent P2P bridge with the secondary PCI bus facing processor"},
  {0x06, 0x0A, 0x00, L"InfiniBand-to-PCI host bridge"},
  {0x06, 0x80, 0x00, L"Other bridge device"},
  
  {0x07, 0x00, 0x00, L"Generic XT-compatible serial controller"},
  {0x07, 0x00, 0x01, L"16450-compatible serial controller"},
  {0x07, 0x00, 0x02, L"16550-compatible serial controller"},
  {0x07, 0x00, 0x03, L"16650-compatible serial controller"},
  {0x07, 0x00, 0x04, L"16750-compatible serial controller"},
  {0x07, 0x00, 0x05, L"16850-compatible serial controller"},
  {0x07, 0x00, 0x06, L"16950-compatible serial controller"},
  {0x07, 0x01, 0x00, L"Parallel port"},
  {0x07, 0x01, 0x01, L"Bi-directional parallel port"},
  {0x07, 0x01, 0x02, L"ECP 1.X compliant parallel port"},
  {0x07, 0x01, 0x03, L"IEEE1284 controller"},
  {0x07, 0x01, 0xFE, L"IEEE1284 target device"},
  {0x07, 0x02, 0x00, L"Multiport serial controller"},
  {0x07, 0x03, 0x00, L"Generic modem"},
  {0x07, 0x03, 0x01, L"Hayes compatible modem, 16450-compatible interface"},
  {0x07, 0x03, 0x02, L"Hayes compatible modem, 16550-compatible interface"},
  {0x07, 0x03, 0x03, L"Hayes compatible modem, 16650-compatible interface"},
  {0x07, 0x03, 0x04, L"Hayes compatible modem, 16750-compatible interface"},
  {0x07, 0x04, 0x00, L"GPIB (IEEE 488.1/2) controller"},
  {0x07, 0x05, 0x00, L"Smart Card"},
  {0x07, 0x80, 0x00, L"Other communications device"},
  
  {0x08, 0x00, 0x00, L"Generic 8259 PIC"}, 
  {0x08, 0x00, 0x01, L"ISA PIC"},
  {0x08, 0x00, 0x02, L"EISA PIC"},
  {0x08, 0x00, 0x10, L"I/O APIC interrupt controller"},
  {0x08, 0x00, 0x20, L"I/O(x) APIC interrupt controller"},
  {0x08, 0x01, 0x00, L"Generic 8237 DMA controller"},
  {0x08, 0x01, 0x01, L"ISA DMA controller"},
  {0x08, 0x01, 0x02, L"EISA DMA controller"},
  {0x08, 0x02, 0x00, L"Generic 8254 system timer"},
  {0x08, 0x02, 0x01, L"ISA system timer"},
  {0x08, 0x02, 0x02, L"EISA system timers"},
  {0x08, 0x03, 0x00, L"Generic RTC controller"},
  {0x08, 0x03, 0x01, L"ISA RTC controller"},
  {0x08, 0x04, 0x00, L"Generic PCI Hot-Plug controller"},
  {0x08, 0x80, 0x00, L"Other system peripheral"},
  
  {0x09, 0x00, 0x00, L"Keyboard controller"},
  {0x09, 0x01, 0x00, L"Digitizer (pen)"},
  {0x09, 0x02, 0x00, L"Mouse controller"},
  {0x09, 0x03, 0x00, L"Scanner controller"},
  {0x09, 0x04, 0x00, L"Gameport controller (generic)"},
  {0x09, 0x04, 0x10, L"Gameport controller"},
  {0x09, 0x80, 0x00, L"Other input controller"},
  
  {0x0A, 0x00, 0x00, L"Generic docking station"},
  {0x0A, 0x80, 0x00, L"Other type of docking station"},
  
  {0x0B, 0x00, 0x00, L"386"},
  {0x0B, 0x01, 0x00, L"486"},
  {0x0B, 0x02, 0x00, L"Pentium"},
  {0x0B, 0x10, 0x00, L"Alpha"},
  {0x0B, 0x20, 0x00, L"PowerPC"},
  {0x0B, 0x30, 0x00, L"MIPS"},
  {0x0B, 0x40, 0x00, L"Co-processor"},
  
  {0x0C, 0x00, 0x00, L"IEEE 1394(FireWire)"},
  {0x0C, 0x00, 0x10, L"IEEE 1394(OHCI)"},
  {0x0C, 0x01, 0x00, L"ACCESS.bus"},
  {0x0C, 0x02, 0x00, L"SSA"},
  {0x0C, 0x03, 0x00, L"USB UHCI"},
  {0x0C, 0x03, 0x10, L"USB OHCI"},
  {0x0C, 0x03, 0x20, L"USB EHCI"},
  {0x0C, 0x03, 0x30, L"USB XHCI"},  
  {0x0C, 0x03, 0x80, L"USB"},
  {0x0C, 0x03, 0xFE, L"USB device"},
  {0x0C, 0x04, 0x00, L"Fibre Channel"},
  {0x0C, 0x05, 0x00, L"SMBus"},
  {0x0C, 0x06, 0x00, L"InfiniBand"},
  {0x0C, 0x07, 0x00, L"IPMI SMIC Interface"},
  {0x0C, 0x07, 0x01, L"IPMI Kybd Controller Style Interface"},
  {0x0C, 0x07, 0x02, L"IPMI Block Transfer Interface"},
  {0x0C, 0x08, 0x00, L"SERCOS Interface Standard"},
  {0x0C, 0x09, 0x00, L"CANbus"},
  
  {0x0D, 0x00, 0x00, L"iRDA compatible controller"},
  {0x0D, 0x01, 0x00, L"Consumer IR controller"},
  {0x0D, 0x10, 0x00, L"RF controller"},
  {0x0D, 0x11, 0x00, L"Bluetooth"},
  {0x0D, 0x12, 0x00, L"Broadband"},
  {0x0D, 0x20, 0x00, L"Ethernet (802.11a - 5 GHz)"},
  {0x0D, 0x21, 0x00, L"Ethernet (802.11b - 2.4 GHz)"},
  {0x0D, 0x80, 0x00, L"Other type of wireless controller"},
  
  {0x0F, 0x01, 0x00, L"TV"},
  {0x0F, 0x02, 0x00, L"Audio"},
  {0x0F, 0x03, 0x00, L"Voice"},
  {0x0F, 0x04, 0x00, L"Data"},
  
  {0x10, 0x00, 0x00, L"Network and computing en/decryption"},
  {0x10, 0x10, 0x00, L"Entertainment en/decryption"},
  {0x10, 0x80, 0x00, L"Other en/decryption"},
  
  {0x11, 0x00, 0x00, L"DPIO modules"},
  {0x11, 0x01, 0x00, L"Performance counters"},
  {0x11, 0x10, 0x00, L"Communications synchronization plus time and frequency test/m measurement"},
  {0x11, 0x20, 0x00, L"Management card"},
  {0x11, 0x80, 0x00, L"Other data acquisition/signal processing controllers"},
};

UINTN mPciClassCodeNameTableNum = sizeof (mPciClassCodeNameTable) / sizeof (PROJECT_PCI_CLASS_INFO);

// https://pci-ids.ucw.cz/read/PD/

STATIC CHAR16 *mBaseClassNameTable[] = {
  L"compatible",
  L"Mass storage controller",
  L"Network controller",
  L"Display controller",
  L"Multimedia device",
  L"Memory controller",
  L"Bridge device",
  L"Simple communication controllers",
  L"Base system peripherals",
  L"Input devices",
  L"Docking stations",
  L"Processors",
  L"Serial bus controllers",
  L"Wireless controller",
  L"Intelligent I/O controllers",
  L"Satellite communication controllers",
  L"Encryption/Decryption controllers",
  L"Data acquisition and signal processing controllers",
  L"Processing accelerators", 	
  L"Non-Essential Instrumentation",
};

UINTN mPciBaseClassCodeNameTableNum = sizeof (mBaseClassNameTable) / sizeof (CHAR16*);

CHAR16 *
EFIAPI
GetPciDeviceClassInfo (
  IN EFI_PCI_IO_PROTOCOL   *PciIo
  )
{
  UINT8       ClassCode[3];
  UINTN       Index;
  CHAR16      *Name = NULL;

  PciIo->Pci.Read(PciIo, EfiPciIoWidthUint8,  PCI_CLASSCODE_OFFSET, 3, &ClassCode[0]);

  for (Index = 0; Index < mPciClassCodeNameTableNum; Index++) {
    if (ClassCode[2] == mPciClassCodeNameTable[Index].BaseClass &&
        ClassCode[1] == mPciClassCodeNameTable[Index].SubClass &&
        ClassCode[0] == mPciClassCodeNameTable[Index].Interface) {
      Name = AllocateCopyPool (StrSize (mPciClassCodeNameTable[Index].Name), mPciClassCodeNameTable[Index].Name);
      break;
    }
  }

  if (Index >= mPciClassCodeNameTableNum) {
    if (ClassCode[2] < mPciBaseClassCodeNameTableNum - 1) {
      Name = AllocateCopyPool (StrSize (mBaseClassNameTable[ClassCode[2]]), mBaseClassNameTable[ClassCode[2]]);
    } else {
      Name = AllocateZeroPool (StrSize (L"ClassCode %02x:%02x:%02x") + 1);
      UnicodeSPrint (Name, StrSize (L"ClassCode %02x:%02x:%02x") + 1, L"ClassCode %02x:%02x:%02x", ClassCode[2], ClassCode[1], ClassCode[0]);
    }
  }

  return Name;
}

