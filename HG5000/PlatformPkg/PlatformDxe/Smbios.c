
#include "PlatformDxe.h"
#include <SetupVariable.h>
#include <Library/BiosIdLib.h>
#include <Protocol/PciIo.h>
#include <Protocol/BiosIdInfoProtocol.h>
#include <Protocol/PlatHostInfoProtocol.h>


#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiTransportProtocol.h>
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
} SMBIOS_TYPE10_CONTENT;

typedef struct {
  CHAR8   *Name;
  UINT8   DeviceType;
  UINT8   DeviceTypeInstance;
  UINT16  SegmentGroupNum;
  UINT8   BusNum;
  UINT8   DevFuncNum;
} SMBIOS_TYPE41_CONTENT;


#define DMI_TYPE41_DEVTYPE_DEV_EN    BIT7
#define BSC_DEV_LOC_FMT              "Processor%u Ch%u DIMM %c%u"
#define BSC_BANK_LOC_FMT             "A%u_Node%u_Channel%u_Dimm%u"
#define DEVICE_LOCATOR_STRING_NUM    1
#define BANK_LOCATOR_STRING_NUM      2
#define BMC_16BYTE                   16


STATIC CHAR8 gDMiNotSetStr[] = {"TBD"};
// {A7199A69-E522-46bf-BE96-3F49BC97F755}  
STATIC EFI_GUID gMyTestSmbiosType1Guid = { 0xa7199a69, 0xe522, 0x46bf, { 0xbe, 0x96, 0x3f, 0x49, 0xbc, 0x97, 0xf7, 0x55}};
//STATIC EFI_GUID gByoIpmiInterfaceProtocolGuid = { 0xB47769D3, 0x12A3, 0x4F78, { 0x9A, 0xFC, 0x12, 0x2, 0xB3, 0x80, 0xA1, 0x5D } };
//---------------------------------------------------------------------------------------
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
  {"J63",      PortConnectorTypeNone,       "USB+MLAN",       PortConnectorTypeUsb,        PortTypeUsb},
  {"J63",      PortConnectorTypeNone,       "USB+MLAN",       PortConnectorTypeUsb,        PortTypeUsb},
  {"J164",      PortConnectorTypeNone,       "VGA",        PortConnectorTypeDB15Female, PortTypeVideoPort},
  {"J63",       PortConnectorTypeNone,       "USB+MLAN",       PortConnectorTypeRJ45,       PortTypeNetworkPort},
  {"J52",       PortConnectorTypeNone,       "LAN 0",      PortConnectorTypeRJ45,       PortTypeNetworkPort},
  {"J52",       PortConnectorTypeNone,       "LAN 1",      PortConnectorTypeRJ45,       PortTypeNetworkPort},
  {"J96",        PortConnectorTypeNone,      "M.2_0",           PortConnectorTypeNone,       PortTypeOther},
  {"J15",       PortConnectorTypeNone,       "M.2_1",           PortConnectorTypeNone,       PortTypeOther}, 
  {"J25",       PortConnectorTypeNone,       "SATA0",           PortConnectorTypeNone,       PortTypeSata},
  {"J26",       PortConnectorTypeNone,       "SATA1",            PortConnectorTypeNone,       PortTypeSata},
  /*
  {"HDD0-3(J13)",    PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata}, 
  {"HDD4-7(J14)",    PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata},
  {"HDD8-11(J15)",   PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata},  
  {"HDD12-15(J16)",  PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata},
  {"HDD16-19(J17)",  PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata},
  {"HDD20-23(J18)",  PortConnectorTypeSasSata,    "",            PortConnectorTypeNone,       PortTypeSata}, 
  {"TypeA_USB(J23)", PortConnectorTypeUsb,        "",            PortConnectorTypeNone,       PortTypeUsb},  
  */
};
  
#define TYPE8_DEV_COUNT  (sizeof(gType8ContentList)/sizeof(gType8ContentList[0]))
//---------------------------------------------------------------------------------------


SMBIOS_TYPE41_CONTENT  gSmbiosType41ContentList[] = {
  {"VGA",   OnBoardDeviceExtendedTypeVideo,          1, 0, 4, 0}, 
  {"SATA",  OnBoardDeviceExtendedTypeSATAController, 1, 0, 5, 0},
  {"Lan0",  OnBoardDeviceExtendedTypeEthernet,        1, 0, 1, 0}, 
  {"Lan1",  OnBoardDeviceExtendedTypeEthernet,        2, 0, 1, 1}, 
//  {"Lan2",  OnBoardDeviceExtendedTypeEthernet,        2, 0, 3, 2}, 
//  {"Lan3", OnBoardDeviceExtendedTypeEthernet,        3, 0, 3, 3},   
};

SMBIOS_TYPE41_CONTENT  gSmbiosType41ContentList1[] = {
  {"VGA",   OnBoardDeviceExtendedTypeVideo,          1, 0, 6, 0}, 
  {"SATA",  OnBoardDeviceExtendedTypeSATAController, 1, 0, 7, 0},
  {"Lan0",  OnBoardDeviceExtendedTypeEthernet,        1, 0, 3, 0}, 
  {"Lan1",  OnBoardDeviceExtendedTypeEthernet,        2, 0, 3, 1}, 
//  {"Lan2",  OnBoardDeviceExtendedTypeEthernet,        2, 0, 3, 2}, 
//  {"Lan3", OnBoardDeviceExtendedTypeEthernet,        3, 0, 3, 3},   
};

SMBIOS_TYPE41_CONTENT  gSmbiosType41ContentList2[] = {
  {"VGA",   OnBoardDeviceExtendedTypeVideo,           1, 0, 6, 0}, 
  {"SATA",  OnBoardDeviceExtendedTypeSATAController,  1, 0, 7, 0},
  {"Lan0",  OnBoardDeviceExtendedTypeEthernet,        1, 0, 3, 0}, 
  {"Lan1",  OnBoardDeviceExtendedTypeEthernet,        2, 0, 3, 1},  
};



//----------------------------------------------------------------
EFI_STATUS
AddType39Callback(  IN EFI_EVENT    Event,
  IN VOID         *Context)
{
  UINT8                 Index;
  UINT8                 i = 0;
  EFI_STATUS            Status = 0;
  UINTN                 TableSize = 0;
  UINT8                 ResponseData[7*BMC_16BYTE];
  UINT8                 ResponseDataSize=0;
  UINTN                 CommandDatasize = 2;
  UINT8                 CommandData[2];
  SMBIOS_TABLE_TYPE39   *Type39Record = NULL;
  EFI_STRING            AssetTagNumStr = NULL;
  EFI_STRING            ModelPartNumStr = NULL;
  UINTN                 AssetTagNumStrLen = 0;
  UINTN                 ModelPartNumStrLen = 0;
  UINTN                 MaxPower = 0;
  UINT8                 *Response = NULL;
  CHAR8                 *OptionalStrStart = NULL;
  EFI_STRING            ManufacturerStr = NULL;
  UINTN                 ManufaturerStrLen = 0;
  EFI_STRING            SerialNumStr = NULL;
  UINTN                 SerialNumStrLen = 0;
  EFI_STRING            RevisionLevelStr = NULL;
  UINTN                 RevisionLevelStrLen = 0;
  EFI_STRING            LocationStr = NULL;
  UINTN                 LocationStrLen = 0;
  EFI_STRING            DeviceNameStr = NULL;
  UINTN                 DeviceNameStrLen = 0;
  EFI_STRING            MaxPowerCapacityStr = NULL;
  UINTN                 MaxPowerCapacityStrLen = 0;
  UINT16                PowerSupplyPresentFlag = 0;
  EFI_STRING            PowerSupplyPresentTemp = NULL;
  EFI_SMBIOS_PROTOCOL   *Smbios;
  EFI_IPMI_TRANSPORT    *gIpmiTransport = NULL;
  EFI_SMBIOS_HANDLE     PowerSupplyHandle;
  
  ResponseDataSize = sizeof (ResponseData);
  DEBUG((DEBUG_INFO,"entry type39 callback\n"));
  Status = gBS->LocateProtocol (
              &gEfiIpmiTransportProtocolGuid,
              NULL,
              &gIpmiTransport
              );
  if (!EFI_ERROR (Status))
  {
   for(Index=0;Index<2;Index++)
   {
     CommandData[0]=0x08;
     CommandData[1]=Index;
     Status = gIpmiTransport->SendIpmiCommand(
                             gIpmiTransport,
                             0x3E,
                             0,
                             0x80,
                             (UINT8 *)&CommandData, 
                             CommandDatasize,
                             (UINT8*)&ResponseData,
                             &ResponseDataSize
                            );
     if (EFI_ERROR (Status))
     {
       DEBUG((DEBUG_INFO,"Get PUS%d info failed: %r\n",Index,Status));
       return Status;
     }else{
       DEBUG((DEBUG_INFO,"ResponseDataSize:0x%02x \n", ResponseDataSize));
       DEBUG((DEBUG_INFO,"ResponseData:\n"));     
      for(i=0;i<ResponseDataSize;i++)
      {
        DEBUG((DEBUG_INFO,"0x%02x ", ResponseData[i]));
      }
     DEBUG ((DEBUG_INFO,"\n")); 
     }


   //get Manufacturer String
    ManufacturerStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(ManufacturerStr,2*BMC_16BYTE);
    ASSERT (ManufacturerStr != NULL);
    for(i=0;(i<16)&&(ResponseData[i] != 0);i++)
    {
      ManufacturerStr[i] = ResponseData[i];
    }
     ManufaturerStrLen = StrLen(ManufacturerStr);
     DEBUG((DEBUG_INFO,"ManufacturerStr:%s \n", ManufacturerStr));


   //get ModelPart Num String
    ModelPartNumStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(ModelPartNumStr,2*BMC_16BYTE);
    ASSERT (ModelPartNumStr != NULL);
     for(i=16;(i<32)&&(ResponseData[i] != 0);i++)
     {
       ModelPartNumStr[i-16] = ResponseData[i];
     }
    ModelPartNumStrLen = StrLen(ModelPartNumStr);
    DEBUG((DEBUG_INFO,"ModelPartNumStr:%s \n", ModelPartNumStr));


   //get MaxPowerCapacity String
    MaxPowerCapacityStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(MaxPowerCapacityStr,2*BMC_16BYTE);
    ASSERT (MaxPowerCapacityStr != NULL);
     for(i=32;(i<48)&&(ResponseData[i] != 0);i++)
     {
       MaxPowerCapacityStr[i-32] = ResponseData[i];
     }
    MaxPowerCapacityStrLen = StrLen(MaxPowerCapacityStr);
    MaxPower = StrDecimalToUintn(MaxPowerCapacityStr);
    DEBUG((DEBUG_INFO,"MaxPowerCapacityStr:%s \n", MaxPowerCapacityStr));


   //get Serial Nummber String
    SerialNumStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(SerialNumStr,2*BMC_16BYTE);
    ASSERT (SerialNumStr != NULL);
    for(i=48;(i<64)&&(ResponseData[i] != 0);i++)
    {
      SerialNumStr[i-48] = ResponseData[i];
    }
    SerialNumStrLen = StrLen(SerialNumStr);
    DEBUG((DEBUG_INFO,"SerialNumStr:%s \n", SerialNumStr));


   //get Revision Level String
    RevisionLevelStr = AllocatePool(2*BMC_16BYTE);
    ZeroMem(RevisionLevelStr,2*BMC_16BYTE);
    ASSERT (RevisionLevelStr != NULL);
     for(i=64;(i<80)&&(ResponseData[i] != 0);i++)
     {
      RevisionLevelStr[i-64] = ResponseData[i];
     }
    RevisionLevelStrLen = StrLen(RevisionLevelStr);
    DEBUG((DEBUG_INFO,"RevisionLevelStr:%s \n", RevisionLevelStr));


   //get  PowerSupplyPresent
    PowerSupplyPresentTemp = AllocatePool(2*2*BMC_16BYTE);
    ZeroMem(PowerSupplyPresentTemp,2*2*BMC_16BYTE);
    ASSERT (PowerSupplyPresentTemp != NULL);
     for(i=80;(i<112)&&(ResponseData[i] != 0);i++)
     {
       PowerSupplyPresentTemp[i-80] = ResponseData[i];
     }
    PowerSupplyPresentFlag = (UINT16)StrDecimalToUintn(PowerSupplyPresentTemp);
     if(PowerSupplyPresentFlag ==  0)
     {
       DEBUG ((DEBUG_INFO,"PowerSupply not Present\n"));
     }else{
       DEBUG ((DEBUG_INFO,"PowerSupplyPresent Flag is %d\n",PowerSupplyPresentFlag));
       PowerSupplyPresentFlag = 1;
     }


   //Psu Location,DeviceName,AssetTag
     if(Index == 0)
     {
       LocationStr     = L"in the back, on the left-hand side";
       ASSERT (LocationStr != NULL);
       LocationStrLen     = StrLen(LocationStr);
       
       DeviceNameStr     = L"PSU0";
       ASSERT (DeviceNameStr != NULL);
       DeviceNameStrLen   = StrLen(DeviceNameStr);
     }
     else {
       LocationStr     = L"in the back, on the left-hand side";
       ASSERT (LocationStr != NULL);
       LocationStrLen     = StrLen(LocationStr);
       
       DeviceNameStr     = L"PSU1";
       ASSERT (DeviceNameStr != NULL);
       DeviceNameStrLen   = StrLen(DeviceNameStr);
     }

    AssetTagNumStr   = L"N/A";
    ASSERT (AssetTagNumStr != NULL);
    AssetTagNumStrLen     = StrLen(AssetTagNumStr);
    
    TableSize = sizeof(SMBIOS_TABLE_TYPE39) + 1 + LocationStrLen + 1 + DeviceNameStrLen + 1 + RevisionLevelStrLen + 1+ SerialNumStrLen + 1 + \
    AssetTagNumStrLen + 1 + ModelPartNumStrLen + 1 + ManufaturerStrLen  + 1 + 1;

    Type39Record = AllocatePool(TableSize);
    ZeroMem(Type39Record, TableSize);
    ASSERT (Type39Record != NULL);

    Type39Record->Hdr.Type           = EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY;
    Type39Record->Hdr.Handle         = SMBIOS_HANDLE_PI_RESERVED;
    Type39Record->Hdr.Length         = sizeof(SMBIOS_TABLE_TYPE39);
    Type39Record->PowerUnitGroup     = Index+1;
    Type39Record->Location           = 0x01;
    Type39Record->DeviceName         = 0x02;
    Type39Record->Manufacturer       = 0x03;
    Type39Record->SerialNumber       = 0x04;
    Type39Record->AssetTagNumber     = 0x05;
    Type39Record->ModelPartNumber    = 0x06;
    Type39Record->RevisionLevel      = 0x07;
    Type39Record->MaxPowerCapacity   = (UINT16)MaxPower;
    
    DEBUG ((DEBUG_INFO,"MaxPower is %d\n",MaxPower)); 
    
    Type39Record->PowerSupplyCharacteristics.PowerSupplyHotReplaceable = (UINT16)(0);
    Type39Record->PowerSupplyCharacteristics.PowerSupplyPresent        = PowerSupplyPresentFlag;
    Type39Record->PowerSupplyCharacteristics.PowerSupplyUnplugged      = (UINT16)(0);
    Type39Record->PowerSupplyCharacteristics.InputVoltageRangeSwitch   = (UINT16)(0x01);
    Type39Record->PowerSupplyCharacteristics.PowerSupplyStatus         = (UINT16)(0x03);
    Type39Record->PowerSupplyCharacteristics.PowerSupplyType           = (UINT16)(0x04);
    Type39Record->PowerSupplyCharacteristics.Reserved                  = (UINT16)(0);

    Type39Record->InputCurrentProbeHandle  = 0xFFFF;
    Type39Record->InputVoltageProbeHandle  = 0xFFFF;
    Type39Record->CoolingDeviceHandle      = 0xFFFF;

    OptionalStrStart = (CHAR8 *) (Type39Record + 1);
    UnicodeStrToAsciiStr (LocationStr,     OptionalStrStart);
    UnicodeStrToAsciiStr (DeviceNameStr,   OptionalStrStart + LocationStrLen + 1);
    UnicodeStrToAsciiStr (ManufacturerStr,   OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1);
    UnicodeStrToAsciiStr (SerialNumStr,   OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1 + ManufaturerStrLen + 1);
    UnicodeStrToAsciiStr (AssetTagNumStr,   OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1 + \
                          ManufaturerStrLen + 1 + SerialNumStrLen+ 1);
    UnicodeStrToAsciiStr (ModelPartNumStr,   OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1 + \
                          ManufaturerStrLen + 1 + SerialNumStrLen+ 1 + AssetTagNumStrLen + 1);
    UnicodeStrToAsciiStr (RevisionLevelStr, OptionalStrStart + LocationStrLen + 1 + DeviceNameStrLen + 1 + \
                          ManufaturerStrLen + 1 + SerialNumStrLen + 1 +AssetTagNumStrLen +1+ModelPartNumStrLen + 1);
     

    Status = gBS->LocateProtocol ( &gEfiSmbiosProtocolGuid, NULL, (VOID **) &Smbios);
     if (EFI_ERROR(Status))
     {
       DEBUG((DEBUG_INFO,"Locate EfiSmbiosProtocol fail!\n"));
       return Status;
     }
     
    PowerSupplyHandle = SMBIOS_HANDLE_PI_RESERVED;
    Status = Smbios->Add (Smbios, NULL, &PowerSupplyHandle, (EFI_SMBIOS_TABLE_HEADER*)Type39Record);
    DEBUG((DEBUG_INFO,"add PSU%d status is %r\n",Index+1,Status));
   
    
   // free pointer
    FreePool(SerialNumStr);
    FreePool(ModelPartNumStr);
    FreePool(ManufacturerStr);
    FreePool(RevisionLevelStr);
    FreePool(MaxPowerCapacityStr);
    FreePool(Type39Record);
    FreePool(PowerSupplyPresentTemp);
       }
  }

  gBS->CloseEvent(Event);
  return Status;
}




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
  UINT8                 RomSize;

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
  UnicodeStrToAsciiStr(FirmwareVendor, BiosVerdorStr);

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
  RomSize = (UINT8)((PcdGet32(PcdFlashAreaSize)>>16)-1);
  if(RomSize>=255){
    SmbiosRecord->BiosSize = 0xFF;
  }else{
    SmbiosRecord->BiosSize = RomSize;
  }

// Bit7	  PCI is supported - 1 (Yes)	
// Bit11	BIOS is Upgradeable (Flash) - 1 (Yes)
// Bit12	BIOS shadowing is allowed - 1 (Yes)
// Bit15	Boot from CD is supported - 1 (Yes)
// Bit16	Selectable Boot is supported - 1 (Yes)
// Bit17	BIOS ROM is socketed - 1 (Yes)
// Bit19	EDD (Enhanced Disk Drive) Specification is supported - 1 (Yes)
// Bit26	Int 5h, Print Screen Service is supported - 1 (Yes)
// Bit27	Int 9h, 8042 Keyboard services are supported - 1 (Yes)
// Bit28	Int 14h, Serial Services are supported - 1 (Yes)
// Bit29  Int 17h, printer services are supported.
// Bit30  Int 10h, CGA/Mono Video Services are supported.
  SmbiosRecord->BiosCharacteristics.PciIsSupported              = 1;
  SmbiosRecord->BiosCharacteristics.BiosIsUpgradable            = 1;
  SmbiosRecord->BiosCharacteristics.BiosShadowingAllowed        = 1;
  SmbiosRecord->BiosCharacteristics.BootFromCdIsSupported       = 1;
  SmbiosRecord->BiosCharacteristics.SelectableBootIsSupported   = 1;
  SmbiosRecord->BiosCharacteristics.RomBiosIsSocketed           = 1;
  SmbiosRecord->BiosCharacteristics.EDDSpecificationIsSupported = 1;
  SmbiosRecord->BiosCharacteristics.PrintScreenIsSupported      = 0;
  SmbiosRecord->BiosCharacteristics.Keyboard8042IsSupported     = 0;
  SmbiosRecord->BiosCharacteristics.SerialIsSupported           = 0;

//Bit0	ACPI supported - 1 (Yes)
//Bit1	USB Legacy is supported - 1 (Yes)
  SmbiosRecord->BIOSCharacteristicsExtensionBytes[0] = 3;

//Bit0	BIOS Boot Specification supported - 1 (Yes)
//Bit1	Function key-initiated Network Service boot supported - 0 (No)
//Bit2	Enable Targeted Content Distribution - 0 (No)
//Bit3	UEFI Specification is supported - 0 (No)
//Bit4	SMBIOS table describes a virtual machine - 0 (No)
  SmbiosRecord->BIOSCharacteristicsExtensionBytes[1] = 0xD;

  SmbiosRecord->SystemBiosMajorRelease = 0xFF;  //Because the version number of inspur is three digits(xx.xx.xx), this is omitted.
  SmbiosRecord->SystemBiosMinorRelease = 0xFF;
  SmbiosRecord->EmbeddedControllerFirmwareMajorRelease = 0xFF;
  SmbiosRecord->EmbeddedControllerFirmwareMinorRelease = 0xFF;
  if(RomSize>=255){
     SmbiosRecord->ExtendedBiosSize.Size= (RomSize + 1)*64/1024;
     SmbiosRecord->ExtendedBiosSize.Unit = 0; // MB
  }

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


  Manufacturer = (CHAR8*)PcdGetPtr(PcdSmbiosManufacturer);
  ManuStrLen = AsciiStrLen(Manufacturer);
  ASSERT (ManuStrLen <= SMBIOS_STRING_MAX_LENGTH);

  ProductName = (CHAR8*)PcdGetPtr(PcdSmbiosSystemInfoProductName);
  PdNameStrLen = AsciiStrLen(ProductName);
  ASSERT (PdNameStrLen <= SMBIOS_STRING_MAX_LENGTH);

  Version = (CHAR8*)PcdGetPtr(PcdSmbiosSystemInfoVersion);
  VerStrLen = AsciiStrLen(Version);
  ASSERT (VerStrLen <= SMBIOS_STRING_MAX_LENGTH);

  SerialNumber = gDMiNotSetStr;
  SerialNumStrLen = AsciiStrLen(SerialNumber);
  ASSERT (SerialNumStrLen <= SMBIOS_STRING_MAX_LENGTH);

  SkuNumber = gDMiNotSetStr;
  SkuNumStrLen = AsciiStrLen(SkuNumber);
  ASSERT(SkuNumStrLen <= SMBIOS_STRING_MAX_LENGTH);

  Family = "Inspur System";
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

  SmbiosRecord->WakeUpType = SystemWakeupTypePowerSwitch;

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

  Chassis = "Dynamic String";
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
  AsciiStrCpy(Str8, Manufacturer);
  Str8 += ManuStrLen + 1;
  AsciiStrCpy(Str8, Product);
  Str8 += ProductStrLen + 1;
  AsciiStrCpy(Str8, Version);
  Str8 += VerStrLen	+ 1;
  AsciiStrCpy(Str8, SerialNumber);
  Str8 += SerialNumStrLen	+ 1;
  AsciiStrCpy(Str8, AssertTag);
  Str8 += AssertTagStrLen	+ 1;
  AsciiStrCpy(Str8, Chassis);

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);
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
  UINT8                           Response[1];
  UINT8                           Responsesize;

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
  SmbiosRecord->Type         = MiscChassisTypeMainServerChassis;
  SmbiosRecord->Version      = 2;  
  SmbiosRecord->SerialNumber = 3;  
  SmbiosRecord->AssetTag     = 4;
  SmbiosRecord->SKUNumber    = 5;
  
  SmbiosRecord->BootupState      = ChassisStateSafe;
  SmbiosRecord->PowerSupplyState = ChassisStateSafe;
  SmbiosRecord->ThermalState     = ChassisStateSafe;
  SmbiosRecord->SecurityStatus   = ChassisSecurityStatusOther;
  SmbiosRecord->Height           = 2;
  SmbiosRecord->NumberofPowerCords = 2;    

  ZeroMem (Response, 1);
  Responsesize = 1;   
  EfiInitializeIpmiBase();
  Status = EfiSendCommandToBMC (
               0x3a,
               0x04,
               NULL,
               0,
               (UINT8 *) &Response,
               (UINT8 *) &Responsesize
               );
  if(Status == EFI_SUCCESS){
  	if(Response[0] == 0x01){
      SmbiosRecord->Height = 4;
	} else{
      SmbiosRecord->Height = 2;
	}
  }

  Str8 = (CHAR8*)((UINTN)SmbiosRecord + HeaderSize);
  AsciiStrCpy(Str8, Manufacturer);
  Str8 += ManuStrLen + 1;
  AsciiStrCpy(Str8, Version);
  Str8 += VerStrLen + 1;
  AsciiStrCpy(Str8, SerialNumber);
  Str8 += SerialNumStrLen + 1;
  AsciiStrCpy(Str8, AssertTag);
  Str8 += AssertTagStrLen + 1;
  AsciiStrCpy(Str8, SkuNumber);

  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);
  *Type3Handle = SmbiosHandle;

  FreePool(SmbiosRecord);
  return Status;
}


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
      AsciiStrCpy(OptionalStr, Info[Index].IRD_Str);
      OptionalStr += InternalRefStrLen + 1;
    } else {
      SmbiosRecord->InternalReferenceDesignator = 0; 
    }
    
    if(ExternalRefStrLen != 0){
      StrStartIndex++;
      SmbiosRecord->ExternalReferenceDesignator = StrStartIndex;
      AsciiStrCpy(OptionalStr, Info[Index].ERD_Str);
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


STATIC EFI_STATUS AddSmbiosType9(EFI_SMBIOS_PROTOCOL *Smbios)
{
  UINTN                              SlotStrLen;
  EFI_STATUS                         Status = EFI_SUCCESS;
  SMBIOS_TABLE_TYPE9                 *SmbiosRecord;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  PLAT_HOST_INFO_PROTOCOL            *ptHostInfo;  
  UINTN                              Index;
  PLATFORM_HOST_INFO_PCIE_CTX        *PcieCtx;
  EFI_DEVICE_PATH_PROTOCOL           *Dp;
  EFI_DEVICE_PATH_PROTOCOL           *ParentDp;
  EFI_HANDLE                         DeviceHandle;  
  EFI_PCI_IO_PROTOCOL                *PciIo;
  UINTN                              Seg, Bus, Dev, Fun;
  BOOLEAN                            DevPresent;
  
  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  ASSERT(!EFI_ERROR(Status));

  for(Index=0;Index<ptHostInfo->HostCount;Index++){
    if(ptHostInfo->HostList[Index].HostType == PLATFORM_HOST_PCIE  ||
       ptHostInfo->HostList[Index].HostType == PLATFORM_HOST_NVME){
      if(ptHostInfo->HostList[Index].HostCtx == NULL){
        continue;
      }

      PcieCtx = (PLATFORM_HOST_INFO_PCIE_CTX*)ptHostInfo->HostList[Index].HostCtx;

      SlotStrLen = AsciiStrLen(PcieCtx->SlotName);
      SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE9) + SlotStrLen + 1 + 1);
      SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_SLOTS;
      SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE9);
      SmbiosRecord->Hdr.Handle = 0; 
      SmbiosRecord->SlotDesignation  = 1;
      SmbiosRecord->SlotType         = PcieCtx->SlotType;
      SmbiosRecord->SlotDataBusWidth = PcieCtx->SlotbusWidth;
      SmbiosRecord->CurrentUsage     = PcieCtx->SlotUsage;
      SmbiosRecord->SlotLength       = PcieCtx->SlotLen;
      SmbiosRecord->SlotID           = PcieCtx->SlotId;

      *(UINT8*)&SmbiosRecord->SlotCharacteristics1 = 0x0c;
      *(UINT8*)&SmbiosRecord->SlotCharacteristics2 = 0x01;

      SmbiosRecord->SegmentGroupNum = 0xFF;
      SmbiosRecord->BusNum = 0xFF;
      SmbiosRecord->DevFuncNum = 0xFF;
      ParentDp = GetPciParentDp(gBS, ptHostInfo->HostList[Index].Dp);
      if(ParentDp == NULL){
        FreePool(SmbiosRecord);
        continue; 
      }

      Dp = ptHostInfo->HostList[Index].Dp;
      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DeviceHandle);
      if(!EFI_ERROR(Status) && IsDevicePathEnd(Dp)){
        DevPresent = TRUE;
      } else {
        DevPresent = FALSE;
      }
      
      Dp = ParentDp;
      Status = gBS->LocateDevicePath(&gEfiPciIoProtocolGuid, &Dp, &DeviceHandle);
      if(!EFI_ERROR(Status) && IsDevicePathEnd(Dp)){
        Status = gBS->HandleProtocol(DeviceHandle, &gEfiPciIoProtocolGuid, &PciIo);
        if(!EFI_ERROR(Status)){
          PciIo->GetLocation(PciIo, &Seg, &Bus, &Dev, &Fun);
          SmbiosRecord->SegmentGroupNum = (UINT16)Seg;
          SmbiosRecord->BusNum     = (UINT8)Bus;
          SmbiosRecord->DevFuncNum = ((UINT8)Dev << 3) | (UINT8)Fun;
          DEBUG((EFI_D_INFO, "PCI(%X,%X,%X)\n", Bus, Dev, Fun));

          if(DevPresent){
            SmbiosRecord->CurrentUsage = SlotUsageInUse;
          }
          
        }
      }
      FreePool(ParentDp);

      AsciiStrCpy((CHAR8*)(SmbiosRecord + 1), PcieCtx->SlotName);
      Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);
      FreePool(SmbiosRecord);
      
    }
  }

  return Status;
}


EFI_STATUS 
AddSmbiosType11(EFI_SMBIOS_PROTOCOL *Smbios)
{
  UINTN                    StringSize; 
  CHAR8                    *p;
  EFI_STATUS               Status;
  EFI_SMBIOS_HANDLE        SmbiosHandle;
  SMBIOS_TABLE_TYPE11      *SmbiosRecord;
  UINTN                    TotalSize;
  UINT8                    Index;
  UINT8                    StringCount = 6;
  CHAR8                    *BmcVer = NULL;
  CHAR8                    *Type11Str[] = {
    // BMC information
    "Customer ID:Standard",
    "Fan Type:present",
    "AST2500",
    "BMC version:00.00.00",
    // boot information
    "No Boot device",        // boot order (HDD 1, PXE 2, CDROM 3, USB disk 4)
    "No Hdd Found" // First Hdd string
  };
	
  BmcVer = AllocatePool (20);
  AsciiSPrint(BmcVer, 20, "BMC version:00.00.00");
  
  StringSize = AsciiStrSize(Type11Str[0]) + 
               AsciiStrSize(Type11Str[1]) +
               AsciiStrSize(Type11Str[2]) +
               AsciiStrSize(Type11Str[3]) +
               AsciiStrSize(Type11Str[4]) +
               AsciiStrSize(Type11Str[5]);

  TotalSize = sizeof(SMBIOS_TABLE_TYPE11) + StringSize + 1;
  SmbiosRecord = AllocateZeroPool(TotalSize);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_OEM_STRINGS;
  SmbiosRecord->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE11);

  SmbiosRecord->Hdr.Handle = 0;  
  SmbiosRecord->StringCount = StringCount;
  p = (CHAR8*)(SmbiosRecord + 1);

  for(Index = 0; Index < StringCount; Index ++){
    if (Index == 3 && BmcVer != NULL) {
      AsciiStrCpy(p, BmcVer);
      StringSize = AsciiStrSize(BmcVer);
    } else {
      AsciiStrCpy(p, Type11Str[Index]);
      StringSize = AsciiStrSize(Type11Str[Index]);
    }
    p += StringSize;
  }

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;
}


STATIC EFI_STATUS AddSmbiosType12(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                        Status;
  EFI_SMBIOS_HANDLE                 SmbiosHandle;
  SMBIOS_TABLE_TYPE12               *SmbiosRecord;
  CHAR8                             *p;
  UINT8                             Index;
  UINTN                             StringSize; 
  UINT8                             StringCount = 2;
  CHAR8                             *Type12Str[] = {
    "J84: 1-2 Normal, 2-3 RECOVER BIOS",
	"J58: 1-2 Normal, 2-3 CLEAR CMOS",
  };

  StringSize = 0;
  for (Index = 0; Index < StringCount; Index ++) {
    StringSize += AsciiStrSize(Type12Str[Index]);
  }

  SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE12) + StringSize + 1);
  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_CONFIGURATION_OPTIONS;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE12);
  SmbiosRecord->Hdr.Handle = 0;  
  SmbiosRecord->StringCount = StringCount;

  p = (CHAR8*)(SmbiosRecord + 1);
  for (Index = 0; Index < StringCount; Index ++) {
    AsciiStrCpy(p, Type12Str[Index]);
    StringSize = AsciiStrSize(Type12Str[Index]);
    p += StringSize;
  }

  Status = AddSmbiosRecord (Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER *) SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;
}


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

  CurrentLang = GetEfiGlobalVariable(L"PlatformLang");
  DEBUG((EFI_D_INFO, "CurrentLang:%a\n", CurrentLang));
  if (CurrentLang[0] == 'z') {
    SmbiosRecord->CurrentLanguages = 2;
  } else {
    SmbiosRecord->CurrentLanguages = 1;
  }
  if(CurrentLang != NULL){
    FreePool(CurrentLang);
  }

  p = (CHAR8*)(SmbiosRecord + 1);
  AsciiStrCpy(p, LangEnStr);
  p += LangEnStrSize;
  AsciiStrCpy(p, LangCnStr);

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);

  FreePool(SmbiosRecord);
  return Status;
}




STATIC EFI_STATUS AddSmbiosType15(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS				 Status;
  EFI_SMBIOS_HANDLE		 SmbiosHandle;
  SMBIOS_TABLE_TYPE15 	 *SmbiosRecord;
  
  SmbiosRecord = AllocateZeroPool(sizeof(SMBIOS_TABLE_TYPE15)+2);

  SmbiosRecord->Hdr.Type   = EFI_SMBIOS_TYPE_SYSTEM_EVENT_LOG ;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE15);
  SmbiosRecord->Hdr.Handle = 0;
  SmbiosRecord->LogAreaLength = 0;
  SmbiosRecord->LogHeaderStartOffset = 0;
  SmbiosRecord->LogDataStartOffset = 0;
  SmbiosRecord->AccessMethod = 0;
  SmbiosRecord->LogStatus = 0;
  SmbiosRecord->LogChangeToken = 0;
  SmbiosRecord->AccessMethodAddress = 0; 
  SmbiosRecord->LogHeaderFormat = 0; 
  SmbiosRecord->NumberOfSupportedLogTypeDescriptors = 0;
  SmbiosRecord->LengthOfLogTypeDescriptor = 0; 
  SmbiosRecord->EventLogTypeDescriptors[0].DataFormatType = EventLogVariableNone  ; 
  SmbiosRecord->EventLogTypeDescriptors[0].LogType = EventLogTypeReserved ;

  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);
  FreePool(SmbiosRecord);  
  return Status;  
}

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


STATIC EFI_STATUS AddSmbiosType39(EFI_SMBIOS_PROTOCOL *Smbios)
{
   EFI_STATUS            Status ;
   EFI_EVENT             Event;
   VOID                  *Reg;
   EFI_IPMI_TRANSPORT    *gIpmiTransport = NULL;
DEBUG((DEBUG_INFO, "Create type 39 start \n"));

//Register Protocol Notify
  Status = gBS->LocateProtocol (
              &gEfiIpmiTransportProtocolGuid,
              NULL,
              &gIpmiTransport
              );
 if (EFI_ERROR (Status)) {
    DEBUG((DEBUG_INFO, "Not found IpmiTransportProtocol,now Register Protocol Notify  %r \n",Status));
  }
    Status = gBS->CreateEvent (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, AddType39Callback, NULL, &Event);
    ASSERT_EFI_ERROR (Status);
    Status = gBS->RegisterProtocolNotify (&gEfiIpmiTransportProtocolGuid, Event, &Reg);
    ASSERT_EFI_ERROR (Status);
    DEBUG((DEBUG_INFO,"Register IpmiTransportProtocol success\n"));
    gBS->SignalEvent (Event);
  
   
return Status;

  




#if 0
  EFI_STATUS                         Status;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  SMBIOS_TABLE_TYPE39                *SmbiosRecord;
  CHAR8                              *Psu1 = "PSU1";
  CHAR8                              *DummyStr = " ";
  UINTN                              MyStrSize;
  UINTN                              Psu1StrSize;
  CHAR8                              *p;
  UINTN                              Index;


  MyStrSize = AsciiStrSize(DummyStr);
  Psu1StrSize = AsciiStrSize(Psu1);
  SmbiosRecord = AllocateZeroPool(sizeof(SMBIOS_TABLE_TYPE39) + Psu1StrSize + 6 * MyStrSize + 1);
  ASSERT(SmbiosRecord != NULL);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_SYSTEM_POWER_SUPPLY;
  SmbiosRecord->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE39);
  SmbiosRecord->Hdr.Handle = 0;  
  SmbiosRecord->PowerUnitGroup = 1;
  SmbiosRecord->Location = 1;
  SmbiosRecord->DeviceName = 2;
  SmbiosRecord->Manufacturer = 3;
  SmbiosRecord->SerialNumber = 4;
  SmbiosRecord->AssetTagNumber = 5;
  SmbiosRecord->ModelPartNumber = 6;
  SmbiosRecord->RevisionLevel = 7;
  SmbiosRecord->MaxPowerCapacity = 0x8000;            // unknown
  SmbiosRecord->InputVoltageProbeHandle = 0xFFFF;     // none
  SmbiosRecord->CoolingDeviceHandle = 0xFFFF;         // none
  SmbiosRecord->InputCurrentProbeHandle = 0xFFFF;     // none
  SmbiosRecord->PowerSupplyCharacteristics.PowerSupplyPresent = 1;
  SmbiosRecord->PowerSupplyCharacteristics.InputVoltageRangeSwitch = 2;   // Unknown
  SmbiosRecord->PowerSupplyCharacteristics.PowerSupplyStatus = 3;         // OK
  SmbiosRecord->PowerSupplyCharacteristics.PowerSupplyType = 4;           // Switching

  p = (CHAR8*)(SmbiosRecord + 1);
  for(Index=1;Index<=7;Index++){
    if(Index == 2){
      AsciiStrCpy(p, Psu1);
      p += Psu1StrSize;
    } else {
      AsciiStrCpy(p, DummyStr);
      p += MyStrSize;
    }
  }

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord
                     );

  p = LibSmbiosGetStringInTypeByIndex(&SmbiosRecord->Hdr, SmbiosRecord->DeviceName);
  Index = AsciiStrLen(p);
  p[Index-1]++;
  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord
                     );
  
  FreePool(SmbiosRecord);
  return Status;
#endif
}
  
STATIC EFI_STATUS AddSmbiosType41(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                         Status = EFI_SUCCESS;
  SMBIOS_TABLE_TYPE41                *SmbiosRecord;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;
  UINTN                              Index;
  UINTN                              NameLen;
  SMBIOS_TYPE41_CONTENT              *Ctx;
  UINTN                              CtxCount;
  //UINTN                              PciBase;

  if(PcdGet8(PcdSmileline)==0){
	Ctx      = gSmbiosType41ContentList1;
	CtxCount = sizeof(gSmbiosType41ContentList1)/sizeof(gSmbiosType41ContentList1[0]);
  }else if(PcdGet8(PcdSmileline)==3){
    Ctx      = gSmbiosType41ContentList;
    CtxCount = sizeof(gSmbiosType41ContentList)/sizeof(gSmbiosType41ContentList[0]);
  }else{
    Ctx      = gSmbiosType41ContentList2;
    CtxCount = sizeof(gSmbiosType41ContentList2)/sizeof(gSmbiosType41ContentList2[0]);
  }
  
  for(Index=0;Index<CtxCount;Index++){
    //PciBase = PCI_DEV_MMBASE(Ctx[Index].BusNum, (Ctx[Index].DevFuncNum >> 3), (Ctx[Index].DevFuncNum&0x7));
    //if(MmioRead16(PciBase) == 0xFFFF){       //only decode hygon device
      Ctx[Index].DeviceType |= DMI_TYPE41_DEVTYPE_DEV_EN;
    //}  
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
	if(!PcdGet8(PcdObLanEn)  && ((SmbiosRecord->DeviceType&0xF)==5)){
      SmbiosRecord->DeviceType &=~BIT7;
	}

    AsciiStrCpy((CHAR8 *)(SmbiosRecord + 1), Ctx[Index].Name);
    Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);
    FreePool(SmbiosRecord);    
  }  
  
  return Status;  
}

STATIC EFI_STATUS AddSmbiosType42(EFI_SMBIOS_PROTOCOL *Smbios)
{
  EFI_STATUS                         Status = EFI_SUCCESS;
  SMBIOS_TABLE_TYPE42                *SmbiosRecord;
  EFI_SMBIOS_HANDLE                  SmbiosHandle;

  SmbiosRecord = AllocateZeroPool(sizeof (SMBIOS_TABLE_TYPE42)+1+1);

  SmbiosRecord->Hdr.Type = EFI_SMBIOS_TYPE_MANAGEMENT_CONTROLLER_HOST_INTERFACE;
  SmbiosRecord->Hdr.Length = sizeof (SMBIOS_TABLE_TYPE42);
  SmbiosRecord->Hdr.Handle   = 0;  
  SmbiosRecord->InterfaceType = MCHostInterfaceTypeNetworkHostInterface;
  SmbiosRecord->MCHostInterfaceData[0] =MCHostInterfaceProtocolTypeIPMI; 
  
  Status = AddSmbiosRecord(Smbios, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)SmbiosRecord);
  FreePool(SmbiosRecord);    
  return Status;  
}

#if 0
VOID UpdateSmbiosType17(VOID)
{
    
    EFI_STATUS                      Status;
    EFI_SMBIOS_TYPE                 SmbiosType = 0;
    EFI_SMBIOS_TABLE_HEADER         *SmbiosRecord = NULL;
    SMBIOS_TABLE_TYPE17             *Type17Record = NULL;
    EFI_SMBIOS_PROTOCOL             *Smbios = NULL;
    EFI_SMBIOS_HANDLE               SmbiosHandle = 0;
    UINTN                           BufSize = sizeof(CHAR8) * SMBIOS_STRING_MAX_LENGTH;
//    CHAR8                           *NewDeviceLocatorStr = NULL;
    CHAR8                           *NewBankLocatorStr = NULL;
    UINT8                           Socket             = 0;
    UINT8                           Channel            = 0;
    UINT8                           Dimm               = 0;
    UINT8                           Node               = 0;
/* <DESC> : DIMM info in HDM/BMC is inconsistent with BIOS Setup&Smbios_type_17 because of R4950 DIMM0&DIMM1 exchanged.Modify the BIOS Code to fit the exchange of DIMM0&1. */
    UINT8                           tmpDimm            = 0;
    UINTN                           StringNumber;
    UINT64                          TotalMemSizeMB = 0;

    DEBUG((EFI_D_INFO, __FUNCTION__"()\n"));
    Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID **)&Smbios);
    if (EFI_ERROR(Status))
    {
        return;
    }
    SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
    SmbiosType = EFI_SMBIOS_TYPE_MEMORY_DEVICE;

    for (Socket = 0; Socket < 2; Socket++)
    {
        for (Node = 0; Node < 2; Node++) 
        {
            for (Channel = 0; Channel < 6; Channel++)
            {
                for (Dimm = 0; Dimm < 2; Dimm++)
                {
                Status = Smbios->GetNext(Smbios, &SmbiosHandle, &SmbiosType, &SmbiosRecord, NULL);
                if (EFI_ERROR(Status))
                {
                    DEBUG((EFI_D_INFO,"cpu %d node %d channel %d dimm %d smbios not found\n",Socket,Node,Channel,Dimm));
                    continue;
                }
                Type17Record = (SMBIOS_TABLE_TYPE17 *)SmbiosRecord;
                
//                NewDeviceLocatorStr = AllocatePool(BufSize);
//                ASSERT(NewDeviceLocatorStr != NULL);
                NewBankLocatorStr = AllocatePool(BufSize);
                ASSERT(NewBankLocatorStr != NULL);
                
               /* <DESC> : Caculate total memory size through smbios type17 */
                if (0x7FFF == Type17Record->Size) {
                  TotalMemSizeMB += Type17Record->ExtendedSize;
                } else {
                  TotalMemSizeMB += Type17Record->Size;
                }
                
                /* <DESC> : DIMM info in HDM/BMC is inconsistent with BIOS Setup&Smbios type 17 because of R4950 DIMM0&DIMM1 exchanged.Modify the BIOS Code to fit the exchange of DIMM0&1. */
                tmpDimm = (Dimm + 1) % 2;
                
               /*<DESC> some smbios data wasn't updated match with requirements*/
//                AsciiSPrint(NewDeviceLocatorStr, BufSize, BSC_DEV_LOC_FMT, Socket, Channel, Socket + 'A', Channel + tmpDimm * 8);
                AsciiSPrint(NewBankLocatorStr, BufSize, BSC_BANK_LOC_FMT, Socket, Node, Channel, Dimm);
  
//                StringNumber = DEVICE_LOCATOR_STRING_NUM;
//                Status = Smbios->UpdateString(Smbios, &SmbiosHandle, &StringNumber, NewDeviceLocatorStr);
                StringNumber = BANK_LOCATOR_STRING_NUM;
                Status = Smbios->UpdateString(Smbios, &SmbiosHandle, &StringNumber, NewBankLocatorStr);
                
//                FreePool(NewDeviceLocatorStr);
                FreePool(NewBankLocatorStr);
               }
           }
       }
   }
    
    return;
}
#endif


EFI_STATUS AddSmbiosTypeLate()
{
  EFI_STATUS             Status;
  EFI_SMBIOS_PROTOCOL    *Smbios;  

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = AddSmbiosType13(Smbios);
  ASSERT(!EFI_ERROR(Status));

  Status = AddSmbiosType9(Smbios);  
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
  BIOS_ID_INFO           *BiosIdInfo;
  EFI_SMBIOS_HANDLE      Type3Handle;
  
  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    return;
  }

  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gBiosIdInfoProtocolGuid, NULL, (VOID**)&BiosIdInfo);
  ASSERT(!EFI_ERROR(Status));

  Status = AddSmbiosType0(Smbios, BiosIdInfo);  
  ASSERT(!EFI_ERROR(Status));  
  Status = AddSmbiosType1(Smbios, BiosIdInfo); 
  ASSERT(!EFI_ERROR(Status));
  Status = AddSmbiosType3(Smbios, &Type3Handle);
  ASSERT(!EFI_ERROR(Status));  
  Status = AddSmbiosType2(Smbios, BiosIdInfo, Type3Handle);
  ASSERT(!EFI_ERROR(Status));
  Status = AddSmbiosType8(Smbios);  
  ASSERT(!EFI_ERROR(Status));  
//  Status = AddSmbiosType10(Smbios);  
//  ASSERT(!EFI_ERROR(Status)); 
  Status = AddSmbiosType11(Smbios);  
  ASSERT(!EFI_ERROR(Status));  
  Status = AddSmbiosType12(Smbios);  
  ASSERT(!EFI_ERROR(Status));  
  Status = AddSmbiosType15(Smbios);  
  ASSERT(!EFI_ERROR(Status)); 
  Status = AddSmbiosType23(Smbios);  
  ASSERT(!EFI_ERROR(Status));  
  Status = AddSmbiosType32(Smbios);
  ASSERT(!EFI_ERROR(Status));  
  //Status = AddSmbiosType39(Smbios);
  //ASSERT(!EFI_ERROR(Status)); 
  Status = AddSmbiosType41(Smbios);
  ASSERT(!EFI_ERROR(Status));
  Status = AddSmbiosType42(Smbios);  
  ASSERT(!EFI_ERROR(Status)); 


}




