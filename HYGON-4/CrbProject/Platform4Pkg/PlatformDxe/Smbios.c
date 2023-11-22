

#include "PlatformDxe.h"
#include <SetupVariable.h>
#include <Library/BiosIdLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/SystemPasswordProtocol.h>


typedef struct {
  CHAR8  *IRD_Str;
  UINT8  IntConnType;
  CHAR8  *ERD_Str;
  UINT8  ExtConnType;
  UINT8  PortType;
} SMBIOS_TYPE8_CONTENT;

typedef struct {
  CHAR8   *Name;
  UINT8   DeviceType;
  UINT8   DeviceTypeInstance;
  UINT16  SegmentGroupNum;
  UINT8   BusNum;
  UINT8   DevFuncNum;
} SMBIOS_TYPE41_CONTENT;

#define DMI_TYPE41_DEVTYPE_DEV_EN    BIT7



STATIC CHAR8 gDMiNotSetStr[] = {" "};

//----------------------------------------------------------------
/*
[0] [CON_REAR_USB1](12) [USB_Bottom](12) T:10
[1] [CON_REAR_USB2](12) [USB_Top](12) T:10
[2] [VGA](7) [Video](7) T:1C
[3] [BMC_Mgmf](B) [LAN](B) T:1F
[4] [M.2_0 (J11)](22) [](0) T:FF
[5] [M.2_1 (J12)](22) [](0) T:FF
[6] [HDD0-3 (J13)](22) [](0) T:20
[7] [HDD4-7 (J14)](22) [](0) T:20
[8] [HDD8-11 (J15)](22) [](0) T:20
[9] [HDD12-15 (J16)](22) [](0) T:20
[10] [HDD16-19 (J17)](22) [](0) T:20
[11] [HDD20-23 (J18)](22) [](0) T:20
[12] [TypeA_USB (J23)](12) [](0) T:10
*/

STATIC SMBIOS_TYPE8_CONTENT gType8ContentList[] = {

  {"CON_REAR_USB1",  PortConnectorTypeUsb,        "USB_Bottom",  PortConnectorTypeUsb,        PortTypeUsb},
  {"CON_REAR_USB2",  PortConnectorTypeUsb,        "USB_Top",     PortConnectorTypeUsb,        PortTypeUsb},
  {"VGA",            PortConnectorTypeDB15Female, "Video",       PortConnectorTypeDB15Female, PortTypeVideoPort},
  {"BMC_Mgmf",       PortConnectorTypeRJ45,       "LAN",         PortConnectorTypeRJ45,       PortTypeNetworkPort},
  {"HDD0(J15)",      PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata},
  {"HDD1(J16)",      PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata},
  {"HDD2(J17)",      PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata},
  {"HDD3(J18)",      PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata},
  {"USB0-1(J46)",    PortConnectorTypeUsb,        "",            PortConnectorTypeNone,       PortTypeUsb},

};

#define TYPE8_DEV_COUNT  (sizeof(gType8ContentList)/sizeof(gType8ContentList[0]))


//SMBIOS_TYPE41_CONTENT  gSmbiosType41ContentList[] = {
//  {"Audio", OnBoardDeviceExtendedTypeSound,          1, 0, 0, (0x14 << 3)},
//};






//----------------------------------------------------------------
STATIC
EFI_STATUS
AddSmbiosRecord (
  IN EFI_SMBIOS_PROTOCOL        *Smbios,
  OUT EFI_SMBIOS_HANDLE         *SmbiosHandle,
  IN EFI_SMBIOS_TABLE_HEADER    *Record
  )
{
  *SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  return Smbios->Add (
                   Smbios,
                   NULL,
                   SmbiosHandle,
                   Record
                   );
}



STATIC
EFI_STATUS
AddSmbiosType2(
  EFI_SMBIOS_PROTOCOL *Smbios,
  BIOS_ID_INFO        *BiosIdInfo,
  EFI_SMBIOS_HANDLE   Type3Handle
  )
{
  CHAR8                           *Str8;
  UINTN                           ManuStrLen;
  UINTN                           ProductStrLen;
  UINTN                           VerStrLen;
  UINTN                           AssertTagStrLen;
  UINTN                           SerialNumStrLen;
  UINTN                           ChassisStrLen;
  EFI_STATUS                      Status;
  CHAR8                           *Manufacturer;
  CHAR8                           *Product;
  CHAR8                           *Version;
  CHAR8                           *SerialNumber;
  CHAR8                           *AssertTag;
  CHAR8                           *Chassis;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  SMBIOS_TABLE_TYPE2              *SmbiosRecord;
  UINTN                           TypeSize;


  Manufacturer = (CHAR8*)PcdGetPtr(PcdSmbiosManufacturer);
  ManuStrLen = AsciiStrLen(Manufacturer);
  ASSERT (ManuStrLen <= SMBIOS_STRING_MAX_LENGTH);

  Product = BiosIdInfo->BoardId;
  ProductStrLen = AsciiStrLen(Product);
  ASSERT (ProductStrLen <= SMBIOS_STRING_MAX_LENGTH);

  Version = (CHAR8*)PcdGetPtr(PcdSmbiosBaseBoardVersion);
  VerStrLen = AsciiStrLen(Version);
  ASSERT (VerStrLen <= SMBIOS_STRING_MAX_LENGTH);

  SerialNumber = gDMiNotSetStr;
  SerialNumStrLen = AsciiStrLen(SerialNumber);
  ASSERT (SerialNumStrLen <= SMBIOS_STRING_MAX_LENGTH);

  AssertTag = gDMiNotSetStr;
  AssertTagStrLen = AsciiStrLen(AssertTag);
  ASSERT (AssertTagStrLen <= SMBIOS_STRING_MAX_LENGTH);

  Chassis = "Chassis Location";
  ChassisStrLen = AsciiStrLen(Chassis);
  ASSERT(ChassisStrLen <= SMBIOS_STRING_MAX_LENGTH);

  TypeSize = sizeof(SMBIOS_TABLE_TYPE2) + ManuStrLen + 1
                                        + ProductStrLen + 1
                                        + VerStrLen + 1
                                        + SerialNumStrLen + 1
                                        + AssertTagStrLen + 1
                                        + ChassisStrLen + 1
                                        + 1;
  SmbiosRecord = AllocatePool(TypeSize);
  ZeroMem(SmbiosRecord, TypeSize);

  SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_BASEBOARD_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE2) - sizeof(SmbiosRecord->ContainedObjectHandles);
  //
  // Make handle chosen by smbios protocol.add automatically.
  //
  SmbiosRecord->Hdr.Handle = 0;
  SmbiosRecord->Manufacturer      = 1;
  SmbiosRecord->ProductName       = 2;
  SmbiosRecord->Version           = 3;
  SmbiosRecord->SerialNumber      = 4;
  SmbiosRecord->AssetTag          = 5;
  SmbiosRecord->LocationInChassis = 6;
  SmbiosRecord->FeatureFlag.Motherboard        = 1;
  SmbiosRecord->FeatureFlag.Replaceable        = 1;
  SmbiosRecord->ChassisHandle                  = Type3Handle;
  SmbiosRecord->BoardType                      = BaseBoardTypeMotherBoard;
  SmbiosRecord->NumberOfContainedObjectHandles = 0;

  Str8  = (CHAR8*)(SmbiosRecord + 1) - 2;   // "ContainedObjectHandles" not existed.
  AsciiStrCpyS(Str8, ManuStrLen+1, Manufacturer);
  Str8 += ManuStrLen + 1;
  AsciiStrCpyS(Str8, ProductStrLen+1, Product);
  Str8 += ProductStrLen + 1;
  AsciiStrCpyS(Str8, VerStrLen+1, Version);
  Str8 += VerStrLen  + 1;
  AsciiStrCpyS(Str8, SerialNumStrLen+1, SerialNumber);
  Str8 += SerialNumStrLen  + 1;
  AsciiStrCpyS(Str8, AssertTagStrLen+1, AssertTag);
  Str8 += AssertTagStrLen  + 1;
  AsciiStrCpyS(Str8, ChassisStrLen+1, Chassis);

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);
  FreePool(SmbiosRecord);
  return Status;
}



STATIC EFI_STATUS AddSmbiosType0(EFI_SMBIOS_PROTOCOL *Smbios, BIOS_ID_INFO *BiosIdInfo)
{
  CHAR8                 *Str8;
  UINTN                 VendorStrLen;
  UINTN                 VerStrLen;
  UINTN                 DateStrLen;
  EFI_STATUS            Status;
  CHAR8                 *BiosVerdorStr = NULL;
  SMBIOS_TABLE_TYPE0    *SmbiosRecord = NULL;
  EFI_SMBIOS_HANDLE     SmbiosHandle;
  CHAR16                *FirmwareVendor;
  UINTN                 FwVdrLen;
  CHAR8                 *Version;
  CHAR8                 *ReleaseDate;
  UINT32                BiosSize;


  FirmwareVendor = (CHAR16*)PcdGetPtr(PcdFirmwareVendor);
  FwVdrLen = StrLen(FirmwareVendor);
  if(FwVdrLen == 0){
    FirmwareVendor = L"ByoSoft";
    FwVdrLen = StrLen(FirmwareVendor);
  }
  BiosVerdorStr = AllocatePool(FwVdrLen+1);
  if(BiosVerdorStr == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }
  UnicodeStrToAsciiStrS(FirmwareVendor, BiosVerdorStr, FwVdrLen+1);

  VendorStrLen = AsciiStrLen(BiosVerdorStr);
  if (VendorStrLen > SMBIOS_STRING_MAX_LENGTH) {
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  Version = BiosIdInfo->BiosVer;
  VerStrLen = AsciiStrLen(Version);
  if (VerStrLen > SMBIOS_STRING_MAX_LENGTH) {
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  ReleaseDate = BiosIdInfo->BiosDate;
  DateStrLen = AsciiStrLen(ReleaseDate);
  if (DateStrLen > SMBIOS_STRING_MAX_LENGTH) {
    Status = EFI_UNSUPPORTED;
    goto ProcExit;
  }

  SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE0) + VendorStrLen + 1 + VerStrLen + 1 + DateStrLen + 1 + 1);
  if(SmbiosRecord == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE0);

  SmbiosRecord->Hdr.Handle  = 0;
  SmbiosRecord->Vendor      = 1;
  SmbiosRecord->BiosVersion = 2;
  SmbiosRecord->BiosSegment = 0xF000;
  SmbiosRecord->BiosReleaseDate = 3;

  BiosSize = PcdGet32(PcdFlashAreaSize);
  if(BiosSize >= SIZE_16MB){
    SmbiosRecord->BiosSize = 0xFF;
    SmbiosRecord->ExtendedBiosSize.Size = (UINT16)(BiosSize >> 20);
    SmbiosRecord->ExtendedBiosSize.Unit = 0;                 // MB
  } else {
    SmbiosRecord->BiosSize = (UINT8)((BiosSize>>16)-1);      //  64K * (n+1)
  }



// Bit7    PCI is supported - 1 (Yes)
// Bit11  BIOS is Upgradeable (Flash) - 1 (Yes)
// Bit12  BIOS shadowing is allowed - 1 (Yes)
// Bit15  Boot from CD is supported - 1 (Yes)
// Bit16  Selectable Boot is supported - 1 (Yes)
// Bit17  BIOS ROM is socketed - 1 (Yes)
// Bit19  EDD (Enhanced Disk Drive) Specification is supported - 1 (Yes)

// Bit26  Int 5h, Print Screen Service is supported - 1 (Yes)
// Bit27  Int 9h, 8042 Keyboard services are supported - 1 (Yes)
// Bit28  Int 14h, Serial Services are supported - 1 (Yes)
// Bit29  Int 17h, printer services are supported.
// Bit30  Int 10h, CGA/Mono Video Services are supported.
  SmbiosRecord->BiosCharacteristics.PciIsSupported              = 1;
  SmbiosRecord->BiosCharacteristics.BiosIsUpgradable            = 1;
  SmbiosRecord->BiosCharacteristics.BiosShadowingAllowed        = 1;
  SmbiosRecord->BiosCharacteristics.BootFromCdIsSupported       = 1;
  SmbiosRecord->BiosCharacteristics.SelectableBootIsSupported   = 1;
  SmbiosRecord->BiosCharacteristics.RomBiosIsSocketed           = 0;
  SmbiosRecord->BiosCharacteristics.EDDSpecificationIsSupported = 1;
  SmbiosRecord->BiosCharacteristics.PrintScreenIsSupported      = 0;
  SmbiosRecord->BiosCharacteristics.Keyboard8042IsSupported     = 0;
  SmbiosRecord->BiosCharacteristics.SerialIsSupported           = 0;

//Bit0  ACPI supported - 1 (Yes)
//Bit1  USB Legacy is supported - 1 (Yes)
  SmbiosRecord->BIOSCharacteristicsExtensionBytes[0] = 3;

//Bit0  BIOS Boot Specification supported - 1 (Yes)
//Bit1  Function key-initiated Network Service boot supported - 0 (No)
//Bit2  Enable Targeted Content Distribution - 0 (No)
//Bit3  UEFI Specification is supported - 0 (No)
//Bit4  SMBIOS table describes a virtual machine - 0 (No)
  SmbiosRecord->BIOSCharacteristicsExtensionBytes[1] = 0xD;

  SmbiosRecord->SystemBiosMajorRelease = BiosIdInfo->VerMajor;
  SmbiosRecord->SystemBiosMinorRelease = BiosIdInfo->VerMinor;
  SmbiosRecord->EmbeddedControllerFirmwareMajorRelease = 0xFF;
  SmbiosRecord->EmbeddedControllerFirmwareMinorRelease = 0xFF;

  Str8 = (CHAR8 *)(SmbiosRecord + 1);
  CopyMem(Str8, BiosVerdorStr, VendorStrLen);
  Str8 += VendorStrLen + 1;
  CopyMem(Str8, Version, VerStrLen);
  Str8 += VerStrLen + 1;
  CopyMem(Str8, ReleaseDate, DateStrLen);

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);

ProcExit:
  if(SmbiosRecord!=NULL){FreePool(SmbiosRecord);}
  if(BiosVerdorStr!=NULL){FreePool(BiosVerdorStr);}
  return Status;
}



STATIC EFI_STATUS AddSmbiosType32(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                         Status;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  SMBIOS_TABLE_TYPE32                *SmbiosRecord;

  SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE32) + 2);
  ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE32) + 2);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_BOOT_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE32);

  SmbiosRecord->Hdr.Handle = 0;
  SmbiosRecord->BootStatus = 0;

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;
}




STATIC EFI_STATUS AddSmbiosType3(EFI_SMBIOS_PROTOCOL *Smbios, EFI_SMBIOS_HANDLE *Type3Handle)
{
  EFI_STATUS                      Status;
  CHAR8                           *Str8;
  UINTN                           ManuStrLen;
  UINTN                           VerStrLen;
  UINTN                           AssertTagStrLen;
  UINTN                           SerialNumStrLen;
  UINTN                           SkuNumberStrLen;
  CHAR8                           *Manufacturer;
  CHAR8                           *Version;
  CHAR8                           *SerialNumber;
  CHAR8                           *AssertTag;
  CHAR8                           *SkuNumber;
  EFI_SMBIOS_HANDLE               SmbiosHandle;
  SMBIOS_TABLE_TYPE3              *SmbiosRecord;
  UINTN                           HeaderSize;
  SMBIOS_TABLE_STRING             *SkuNumberNo;

  Manufacturer = (CHAR8*)PcdGetPtr(PcdSmbiosManufacturer);
  ManuStrLen = AsciiStrLen(Manufacturer);
  ASSERT(ManuStrLen <= SMBIOS_STRING_MAX_LENGTH);

  Version = (CHAR8*)PcdGetPtr(PcdSmbiosChassisVersion);
  VerStrLen = AsciiStrLen(Version);
  ASSERT(VerStrLen <= SMBIOS_STRING_MAX_LENGTH);

  SerialNumber = " ";
  SerialNumStrLen = AsciiStrLen(SerialNumber);
  ASSERT(SerialNumStrLen <= SMBIOS_STRING_MAX_LENGTH);

  AssertTag = gDMiNotSetStr;
  AssertTagStrLen = AsciiStrLen(AssertTag);
  ASSERT(AssertTagStrLen <= SMBIOS_STRING_MAX_LENGTH);

  SkuNumber = gDMiNotSetStr;
  SkuNumberStrLen = AsciiStrLen(SkuNumber);
  ASSERT(SkuNumberStrLen <= SMBIOS_STRING_MAX_LENGTH);

  HeaderSize = sizeof(SMBIOS_TABLE_TYPE3) - sizeof(CONTAINED_ELEMENT) + sizeof(SMBIOS_TABLE_STRING);
  SmbiosRecord = AllocateZeroPool(
                   HeaderSize +
                   ManuStrLen + 1  +
                   VerStrLen + 1 +
                   SerialNumStrLen + 1 +
                   AssertTagStrLen + 1 +
                   SkuNumberStrLen + 1 +
                   1
                   );

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_ENCLOSURE;
  SmbiosRecord->Hdr.Length = (UINT8)HeaderSize;

  SmbiosRecord->Hdr.Handle   = 0;
  SmbiosRecord->Manufacturer = 1;
  SmbiosRecord->Type         = PcdGet8(PcdSmbiosChassisType);
  SmbiosRecord->Version      = 2;
  SmbiosRecord->SerialNumber = 3;
  SmbiosRecord->AssetTag     = 4;
  SkuNumberNo = (UINT8*)SmbiosRecord + HeaderSize - 1;
  *SkuNumberNo = 5;

  SmbiosRecord->BootupState      = PcdGet8(PcdSmbiosChassisBootupState);
  SmbiosRecord->PowerSupplyState = PcdGet8(PcdSmbiosChassisPowerSupplyState);
  SmbiosRecord->ThermalState     = PcdGet8(PcdSmbiosChassisThermalState);
  SmbiosRecord->SecurityStatus   = PcdGet8(PcdSmbiosChassisSecurityStatus);
  SmbiosRecord->Height           = PcdGet8(PcdSmbiosChassisHeight);
  SmbiosRecord->NumberofPowerCords = PcdGet8(PcdSmbiosChassisPowerCordsCount);

  Str8 = (CHAR8*)((UINTN)SmbiosRecord + HeaderSize);
  AsciiStrCpyS(Str8, ManuStrLen+1, Manufacturer);
  Str8 += ManuStrLen + 1;
  AsciiStrCpyS(Str8, VerStrLen+1, Version);
  Str8 += VerStrLen + 1;
  AsciiStrCpyS(Str8, SerialNumStrLen+1, SerialNumber);
  Str8 += SerialNumStrLen + 1;
  AsciiStrCpyS(Str8, AssertTagStrLen+1, AssertTag);
  Str8 += AssertTagStrLen + 1;
  AsciiStrCpyS(Str8, SkuNumberStrLen+1, SkuNumber);

  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);
  *Type3Handle = SmbiosHandle;

  FreePool(SmbiosRecord);
  return Status;
}



#if _PCD_VALUE_PcdPlatformSmbiosType11Support
STATIC EFI_STATUS AddSmbiosType11(EFI_SMBIOS_PROTOCOL *Smbios)
{
  UINTN                    OemStrLen;
  CHAR8                    *OptionalStrStart;
  EFI_STATUS               Status;
  CHAR8                    *OemStr;
  EFI_SMBIOS_HANDLE        SmbiosHandle;
  SMBIOS_TABLE_TYPE11      *SmbiosRecord;


  OemStr    = (CHAR8*)PcdGetPtr(PcdPlatformSmbiosType11String);
  OemStrLen = AsciiStrLen(OemStr);
  ASSERT (OemStrLen <= SMBIOS_STRING_MAX_LENGTH);

  DEBUG((EFI_D_INFO, "%a OemStr:[%a]\n", __FUNCTION__, OemStr));

  SmbiosRecord = AllocatePool(sizeof (SMBIOS_TABLE_TYPE11) + OemStrLen + 1 + 1);
  ZeroMem(SmbiosRecord, sizeof (SMBIOS_TABLE_TYPE11) + OemStrLen + 1 + 1);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_OEM_STRINGS;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE11);

  SmbiosRecord->Hdr.Handle = 0;
  SmbiosRecord->StringCount = 1;
  OptionalStrStart = (CHAR8 *)(SmbiosRecord + 1);
  AsciiStrCpyS(OptionalStrStart, OemStrLen+1, OemStr);

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;
}
#endif






EFI_STATUS AddSmbiosType8(EFI_SMBIOS_PROTOCOL *Smbios)
{
  CHAR8                  *OptionalStr;
  UINTN                  InternalRefStrLen;
  UINTN                  ExternalRefStrLen;
  EFI_STATUS             Status = EFI_SUCCESS;
  SMBIOS_TABLE_TYPE8     *SmbiosRecord;
  EFI_SMBIOS_HANDLE      SmbiosHandle;
  UINTN                  Index;
  SMBIOS_TYPE8_CONTENT   *Info;
  UINT8                  StrStartIndex;


  Info = gType8ContentList;
  for(Index=0;Index<TYPE8_DEV_COUNT;Index++){
    InternalRefStrLen = AsciiStrLen(Info[Index].IRD_Str);
    ASSERT (InternalRefStrLen <= SMBIOS_STRING_MAX_LENGTH);

    ExternalRefStrLen = AsciiStrLen(Info[Index].ERD_Str);
    ASSERT (InternalRefStrLen <= SMBIOS_STRING_MAX_LENGTH);

    SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE8) + InternalRefStrLen + 1 + ExternalRefStrLen + 1 + 1);

    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_PORT_CONNECTOR_INFORMATION;
    SmbiosRecord->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE8);
    SmbiosRecord->Hdr.Handle = 0;

    StrStartIndex = 0;
    OptionalStr = (CHAR8*)(SmbiosRecord + 1);

    if(InternalRefStrLen != 0){
      StrStartIndex++;
      SmbiosRecord->InternalReferenceDesignator = StrStartIndex;
      AsciiStrCpyS(OptionalStr, InternalRefStrLen+1, Info[Index].IRD_Str);
      OptionalStr += InternalRefStrLen + 1;
    } else {
      SmbiosRecord->InternalReferenceDesignator = 0;
    }

    if(ExternalRefStrLen != 0){
      StrStartIndex++;
      SmbiosRecord->ExternalReferenceDesignator = StrStartIndex;
      AsciiStrCpyS(OptionalStr, ExternalRefStrLen + 1, Info[Index].ERD_Str);
      OptionalStr += ExternalRefStrLen + 1;
    } else {
      SmbiosRecord->ExternalReferenceDesignator = 0;
    }

    SmbiosRecord->InternalConnectorType = Info[Index].IntConnType;
    SmbiosRecord->ExternalConnectorType = Info[Index].ExtConnType;
    SmbiosRecord->PortType = Info[Index].PortType;

    Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);
    FreePool(SmbiosRecord);
  }

  return Status;
}



#if _PCD_VALUE_PcdPlatformSmbiosType23Support
STATIC EFI_STATUS AddSmbiosType23(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS               Status;
  EFI_SMBIOS_HANDLE        SmbiosHandle;
  SMBIOS_TABLE_TYPE23      *SmbiosRecord;

  SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE23) + 1 + 1);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_RESET;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE23);

  SmbiosRecord->Hdr.Handle    = 0;
  SmbiosRecord->Capabilities  = 0;
  SmbiosRecord->ResetCount    = 0;
  SmbiosRecord->ResetLimit    = 0;
  SmbiosRecord->TimerInterval = 0;
  SmbiosRecord->Timeout       = 0;

  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;
}
#endif

STATIC EFI_STATUS AddSmbiosType13(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS               Status;
  EFI_SMBIOS_HANDLE        SmbiosHandle;
  SMBIOS_TABLE_TYPE13      *SmbiosRecord;
  CHAR8                    *LangEnStr;
  CHAR8                    *LangCnStr;
  UINTN                    LangEnStrSize;
  UINTN                    LangCnStrSize;
  CHAR8                    *CurrentLang;
  CHAR8                    *p;


  LangEnStr = "en|US|iso8859-1";
  LangCnStr = "zh|CN|unicode";

  LangEnStrSize = AsciiStrSize(LangEnStr);
  LangCnStrSize = AsciiStrSize(LangCnStr);

  SmbiosRecord = AllocateZeroPool(sizeof(SMBIOS_TABLE_TYPE13) + LangEnStrSize + LangCnStrSize + 1);
  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_BIOS_LANGUAGE_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE13);

  SmbiosRecord->Hdr.Handle = 0;
  SmbiosRecord->InstallableLanguages = 2;
  SmbiosRecord->Flags = 0;                            // long format

  Status = GetEfiGlobalVariable2(L"PlatformLang", &CurrentLang, NULL);
  if(!EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "CurrentLang:%a\n", CurrentLang));
    if (CurrentLang[0] == 'z') {
      SmbiosRecord->CurrentLanguages = 2;
    } else {
      SmbiosRecord->CurrentLanguages = 1;
    }
    FreePool(CurrentLang);

  } else {
    SmbiosRecord->CurrentLanguages = 1;
  }

  p = (CHAR8*)(SmbiosRecord + 1);
  AsciiStrCpyS(p, LangEnStrSize+1, LangEnStr);
  p += LangEnStrSize;
  AsciiStrCpyS(p, LangCnStrSize+1, LangCnStr);

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;
}


// {A7199A69-E522-46bf-BE96-3F49BC97F755}
STATIC EFI_GUID gMyTestSmbiosType1Guid = { 0xa7199a69, 0xe522, 0x46bf, { 0xbe, 0x96, 0x3f, 0x49, 0xbc, 0x97, 0xf7, 0x55}};

STATIC EFI_STATUS AddSmbiosType1(EFI_SMBIOS_PROTOCOL *Smbios, BIOS_ID_INFO *BiosIdInfo)
{
  CHAR8                             *Str8;
  UINTN                             ManuStrLen;
  UINTN                             VerStrLen;
  UINTN                             PdNameStrLen;
  UINTN                             SerialNumStrLen;
  UINTN                             SkuNumStrLen;
  UINTN                             FamilyStrLen;
  UINTN                             TotalSize;
  EFI_STATUS                        Status;
  CHAR8                             *Manufacturer;
  CHAR8                             *ProductName;
  CHAR8                             *Version;
  CHAR8                             *SerialNumber;
  CHAR8                             *SkuNumber;
  CHAR8                             *Family;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  SMBIOS_TABLE_TYPE1                *SmbiosRecord;
  UINT8                             WakeUpSrc;  


  Manufacturer = (CHAR8*)PcdGetPtr(PcdSmbiosManufacturer);
  ManuStrLen = AsciiStrLen(Manufacturer);
  ASSERT (ManuStrLen <= SMBIOS_STRING_MAX_LENGTH);

  ProductName = (CHAR8*)PcdGetPtr(PcdSmbiosSystemInfoProductName);
  if(ProductName[0] == 0){
    ProductName = BiosIdInfo->BoardId;
  }
  PdNameStrLen = AsciiStrLen(ProductName);
  ASSERT (PdNameStrLen <= SMBIOS_STRING_MAX_LENGTH);

  Version = (CHAR8*)PcdGetPtr(PcdSmbiosSystemInfoVersion);
  VerStrLen = AsciiStrLen(Version);
  ASSERT (VerStrLen <= SMBIOS_STRING_MAX_LENGTH);

  SerialNumber = gDMiNotSetStr;
  SerialNumStrLen = AsciiStrLen(SerialNumber);
  ASSERT (SerialNumStrLen <= SMBIOS_STRING_MAX_LENGTH);

  SkuNumber = (CHAR8*)PcdGetPtr(PcdSmbiosSystemInfoSkuNumber);
  SkuNumStrLen = AsciiStrLen(SkuNumber);
  ASSERT(SkuNumStrLen <= SMBIOS_STRING_MAX_LENGTH);

  Family = (CHAR8*)PcdGetPtr(PcdSmbiosSystemInfoFamily);
  FamilyStrLen = AsciiStrLen(Family);
  ASSERT(FamilyStrLen <= SMBIOS_STRING_MAX_LENGTH);

  TotalSize = sizeof (SMBIOS_TABLE_TYPE1) + ManuStrLen + 1 + PdNameStrLen + 1 + VerStrLen + 1 +
              SerialNumStrLen + 1 + SkuNumStrLen + 1 + FamilyStrLen + 1 + 1;
  SmbiosRecord = AllocateZeroPool(TotalSize);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_INFORMATION;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE1);

  SmbiosRecord->Hdr.Handle   = 0;
  SmbiosRecord->Manufacturer = 1;
  SmbiosRecord->ProductName  = 2;
  SmbiosRecord->Version      = 3;
  SmbiosRecord->SerialNumber = 4;
  SmbiosRecord->SKUNumber    = 5;
  SmbiosRecord->Family       = 6;

#ifndef MDEPKG_NDEBUG
  CopyMem(&SmbiosRecord->Uuid, &gMyTestSmbiosType1Guid, sizeof(SmbiosRecord->Uuid));
#else
  SetMem(&SmbiosRecord->Uuid, sizeof(SmbiosRecord->Uuid), 0xFF);
#endif

  switch (gPlatCommInfo->AcpiWakeupSrc) {
    case WAK_TYPE_SYS_RESET:
      WakeUpSrc = SystemWakeupTypeAcPowerRestored;
      break;

    case WAK_TYPE_RTC:
      WakeUpSrc = SystemWakeupTypeApmTimer;
      break;

    case WAK_TYPE_PCIE:
      WakeUpSrc = SystemWakeupTypeLanRemote;
      break;

    case WAK_TYPE_POWERBUTTON:
    default:
      WakeUpSrc = SystemWakeupTypePowerSwitch;
      break;
  }
  SmbiosRecord->WakeUpType = WakeUpSrc;

  Str8 = (CHAR8 *)(SmbiosRecord + 1);
  CopyMem(Str8, Manufacturer, ManuStrLen);
  Str8 += ManuStrLen + 1;
  CopyMem(Str8, ProductName, PdNameStrLen);
  Str8 += PdNameStrLen + 1;
  CopyMem(Str8, Version, VerStrLen);
  Str8 += VerStrLen + 1;
  CopyMem(Str8, SerialNumber, SerialNumStrLen);
  Str8 += SerialNumStrLen + 1;
  CopyMem(Str8, SkuNumber, SkuNumStrLen);
  Str8 += SkuNumStrLen + 1;
  CopyMem(Str8, Family, FamilyStrLen);

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;
}




STATIC EFI_STATUS AddSmbiosType12(EFI_SMBIOS_PROTOCOL *Smbios)
{
  UINTN                             OptStrLen;
  CHAR8                             *OptionString;
  EFI_STATUS                        Status;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  SMBIOS_TABLE_TYPE12               *SmbiosRecord;


  OptionString = "System Option String";
  OptStrLen = AsciiStrLen(OptionString);
  ASSERT(OptStrLen <= SMBIOS_STRING_MAX_LENGTH);

  SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE12) + OptStrLen + 1 + 1);
  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_CONFIGURATION_OPTIONS;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE12);
  SmbiosRecord->Hdr.Handle = 0;
  SmbiosRecord->StringCount = 1;
  AsciiStrCpyS((CHAR8*) (SmbiosRecord + 1), OptStrLen + 1, OptionString);

  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;
}



/*
STATIC EFI_STATUS AddSmbiosType41(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                         Status = EFI_SUCCESS;
  SMBIOS_TABLE_TYPE41                *SmbiosRecord;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  UINTN                              Index;
  UINTN                              NameLen;
  SMBIOS_TYPE41_CONTENT              *Ctx;
  UINTN                              CtxCount;
  UINTN                              PciBase;


  Ctx      = gSmbiosType41ContentList;
  CtxCount = sizeof(gSmbiosType41ContentList)/sizeof(gSmbiosType41ContentList[0]);

  for(Index=0;Index<CtxCount;Index++){
    PciBase = PCI_DEV_MMBASE(Ctx[Index].BusNum, (Ctx[Index].DevFuncNum >> 3), (Ctx[Index].DevFuncNum&0x7));
    if(MmioRead16(PciBase) != 0xFFFF){
      Ctx[Index].DeviceType |= DMI_TYPE41_DEVTYPE_DEV_EN;
    }

    NameLen = AsciiStrLen(Ctx[Index].Name);
    SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE41) + NameLen + 1 + 1);
    SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_ONBOARD_DEVICES_EXTENDED_INFORMATION;
    SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE41);
    SmbiosRecord->ReferenceDesignation = 1;
    SmbiosRecord->DeviceType         = Ctx[Index].DeviceType;
    SmbiosRecord->DeviceTypeInstance = Ctx[Index].DeviceTypeInstance;
    SmbiosRecord->SegmentGroupNum    = Ctx[Index].SegmentGroupNum;
    SmbiosRecord->BusNum             = Ctx[Index].BusNum;
    SmbiosRecord->DevFuncNum         = Ctx[Index].DevFuncNum;

    AsciiStrCpy((CHAR8 *)(SmbiosRecord + 1), Ctx[Index].Name);
    Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);
    FreePool(SmbiosRecord);
  }

  return Status;
}
*/


// 00b  Disabled 
// 01b  Enabled 
// 10b  Not Implemented 
// 11b  Unknown 

#define SECURITY_STATUS_DISABLED     0
#define SECURITY_STATUS_ENABLED      1
#define SECURITY_STATUS_NOT_IMP      2
#define SECURITY_STATUS_UNKNOWN      3

typedef struct {
  UINT8   FpResetSts:2;
  UINT8   AdminPwdSts:2;
  UINT8   KbPwdSts:2;
  UINT8   PopSts:2;
} HW_SECURITY_SET;

STATIC EFI_STATUS AddSmbiosType24(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                        Status;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  SMBIOS_TABLE_TYPE24               Type24;
  HW_SECURITY_SET                   *Set = (HW_SECURITY_SET*)&Type24.HardwareSecuritySettings;
  EFI_SYSTEM_PASSWORD_PROTOCOL      *SysPwd;
  

  Type24.Hdr.Type = EFI_SMBIOS_TYPE_HARDWARE_SECURITY;
  Type24.Hdr.Length = sizeof(SMBIOS_TABLE_TYPE24);
  Type24.Hdr.Handle = 0;

  Status = gBS->LocateProtocol(&gEfiSystemPasswordProtocolGuid, NULL, (VOID**)&SysPwd);
  if (EFI_ERROR(Status)) {
    Set->AdminPwdSts = SECURITY_STATUS_NOT_IMP;
    Set->PopSts      = SECURITY_STATUS_NOT_IMP;    
  } else {
    Set->AdminPwdSts = SysPwd->BeHave(PD_ADMIN) ? SECURITY_STATUS_ENABLED : SECURITY_STATUS_DISABLED;
    Set->PopSts      = SysPwd->BeHave(PD_POWER_ON) ? SECURITY_STATUS_ENABLED : SECURITY_STATUS_DISABLED;
  }

  Set->FpResetSts = PcdGet8(PcdSmbiosType24FrontPanelResetStatusValue);
  Set->KbPwdSts   = PcdGet8(PcdSmbiosType24KeyboardPasswordStatusValue);

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, &Type24.Hdr);

  return Status;
}



EFI_STATUS AddSmbiosTypeLate()
{
  EFI_STATUS                Status;
  EFI_SMBIOS_PROTOCOL       *Smbios;
  EFI_SMBIOS_HANDLE         SmbiosHandle;
  EFI_SMBIOS_TYPE           SmbiosType;
  EFI_SMBIOS_TABLE_HEADER   *SmbiosHdr;
  SMBIOS_STRUCTURE_POINTER  p;


  DEBUG((EFI_D_INFO, "AddSmbiosTypeLate\n"));

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  SmbiosType = EFI_SMBIOS_TYPE_BIOS_INFORMATION;
  Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosHdr, NULL);
  ASSERT(!EFI_ERROR(Status));
  p.Hdr = SmbiosHdr;

  DEBUG((EFI_D_INFO, "FlashSize:%x\n", gByoSharedSmmData->FlashSize));

  if(gByoSharedSmmData->FlashSize){
    if(gByoSharedSmmData->FlashSize >= SIZE_16MB){
      p.Type0->BiosSize = 0xFF;
      p.Type0->ExtendedBiosSize.Size = (UINT16)(gByoSharedSmmData->FlashSize >> 20);
      p.Type0->ExtendedBiosSize.Unit = 0;                 // MB
    } else {
      p.Type0->BiosSize = (UINT8)((gByoSharedSmmData->FlashSize>>16)-1);      //  64K * (n+1)
    }
  }
 

  Status = AddSmbiosType13(Smbios);
  ASSERT(!EFI_ERROR(Status));

  Status = AddSmbiosType24(Smbios);
  ASSERT(!EFI_ERROR(Status));

  return Status;
}



VOID
SmbiosCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS             Status;
  EFI_SMBIOS_PROTOCOL    *Smbios;
  BIOS_ID_INFO           BiosIdInfo;
  EFI_SMBIOS_HANDLE      Type3Handle;


  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    return;
  }

  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = GetBiosIdInfoFromHob (&BiosIdInfo);
  ASSERT(!EFI_ERROR(Status));

  Status = AddSmbiosType0(Smbios, &BiosIdInfo);
  ASSERT(!EFI_ERROR(Status));
  Status = AddSmbiosType1(Smbios, &BiosIdInfo);
  ASSERT(!EFI_ERROR(Status));
  Status = AddSmbiosType3(Smbios, &Type3Handle);
  ASSERT(!EFI_ERROR(Status));
  Status = AddSmbiosType2(Smbios, &BiosIdInfo, Type3Handle);
  ASSERT(!EFI_ERROR(Status));

  if (PcdGetBool (PcdPlatformSmbiosType8Support)) {
    Status = AddSmbiosType8(Smbios);
    ASSERT(!EFI_ERROR(Status));
  }

#if _PCD_VALUE_PcdPlatformSmbiosType11Support
  Status = AddSmbiosType11(Smbios);
  ASSERT(!EFI_ERROR(Status));
#endif

#if _PCD_VALUE_PcdPlatformSmbiosType12Support     // winddy + 200918 #2941884 add t12, t18 support control.
  Status = AddSmbiosType12(Smbios);
  ASSERT(!EFI_ERROR(Status));
#endif                                            // winddy + 200918 #2941884 add t12, t18 support control.

#if _PCD_VALUE_PcdPlatformSmbiosType23Support
  Status = AddSmbiosType23(Smbios);
  ASSERT(!EFI_ERROR(Status));
#endif

  Status = AddSmbiosType32(Smbios);
  ASSERT(!EFI_ERROR(Status));

}




