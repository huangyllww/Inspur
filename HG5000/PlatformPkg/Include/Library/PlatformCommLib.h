
#ifndef __PLAT_COMM_LIB_H__
#define __PLAT_COMM_LIB_H__

#include <Uefi.h>
#include <SetupVariable.h>


UINT8 CmosRead(UINT8 Address);
VOID  CmosWrite(UINT8 Address, UINT8 Data);
VOID *GetSetupDataHobData(VOID);
VOID *GetBMCConfigValueHob(VOID);

VOID *GetCarTopData();

VOID
ConvertIdentifyDataFormat (
  IN  UINT8   *Data,
  IN  UINTN   DataSize
  );


#define PCI_HOST_MAX_COUNT       8

#define PLAT_COMM_INFO_SIGN        SIGNATURE_32('P', 'C', 'I', 'F')

typedef struct {
  UINT32  Signature;
  UINT32  Tolum;
  UINT64  Tom2;
  UINT64  UmaBase;
  UINT64  UmaSize;
  UINT32  TsegBase;
  UINT32  TsegSize;
  UINT32  S3DataRecord;
  UINT32  S3DataRecordSize;
  UINT32  S3MemoryAddr;
  UINT32  S3MemorySize;
  UINT32  LowMemSize;
  UINT32  PciHostCount;
  UINT32  Dies;
  UINT32  Sockets;
  UINT32  NbApicId[PCI_HOST_MAX_COUNT];
  UINT32  SbApicId;
  UINT32  NbApicBase[PCI_HOST_MAX_COUNT];
  UINT32  SbApicBase;
  UINT32  MmioBase[PCI_HOST_MAX_COUNT];
  UINT32  MmioSize[PCI_HOST_MAX_COUNT];
  UINT16  IoBase[PCI_HOST_MAX_COUNT];
  UINT16  IoSize[PCI_HOST_MAX_COUNT];
  UINT16  PlatPciIoBase;
  UINT16  PlatPciIoLimit;
  UINT16  PlatPciIoCur;  
  UINT8   FabricId[PCI_HOST_MAX_COUNT];
  UINT64  Mmio64Base[PCI_HOST_MAX_COUNT];
  UINT64  Mmio64Size[PCI_HOST_MAX_COUNT];
  UINT8   BusBase[PCI_HOST_MAX_COUNT];
  UINT8   BusLimit[PCI_HOST_MAX_COUNT];
  UINT8   VgaDieId;
} PLATFORM_COMM_INFO;

typedef struct{
  UINT8                ReadFlag;
  //byte1
  UINT8 AmdSmtMode:1;
  UINT8 SriovDis:1;
  UINT8 CbsCmnSVMCtrl:1;
  UINT8 CbsCmnGnbNbIOMMU:2;
  UINT8 Reserved1:3;
  //byte2
  UINT8 CbsDfCmnMemIntlv:3;  //CBS -> NUMA
  UINT8 CbsCmnCpuCpb:1;  //CBS -> CPB
  UINT8 LanBoot:1; //PXE Boot
  UINT8 BIOSLock:1;
  UINT8 HideBrandLogo:1;
  UINT8 Reserved2:1;
  //byte3
  UINT8 BootModeType:1; 
  UINT8 Ipv4Support:1; 
  UINT8 Ipv6Support:1; 
  UINT8 Reserved3:5;
  //byte4/5/6
  UINT8 FirstBoot:4;
  UINT8 SecondBoot:4;
  UINT8 ThirdBoot:4;
  UINT8 FourthBoot:4;
  UINT8 FifthBoot:4;
  UINT8 SixthBoot:4;
  //byte7
  UINT8 Language:1;
  UINT8 UCREnable:1;
  UINT8 SerialBaudrate:3;  
  UINT8 Reserved4:3;
  //byte8
  UINT8 Pfeh:1; 
  UINT8 McaErrThreshCount:3; 
  UINT8 ObLanEn:1;
  UINT8 WakeOnLan:1;
  UINT8 VideoPrimaryAdapter:1;
  UINT8 Reserved5:1;
  //byte9
  UINT8 USBRearCtrl:1;
  UINT8 USBFronCtrl:1;
  UINT8 UsbMassSupport:1;
  UINT8 PcieMaxPayloadSize:2;
  UINT8 PcieAspm:2;
  UINT8 Reserved6:1;
  //byte10
  UINT8 PerfMode:2;
  UINT8 AMDPstate:2;
  UINT8 AMDCstate:1;
  UINT8 CpuSpeed:2;
  UINT8 Reserved7:1;
  //byte11/12
  UINT8 BootTimeout[2];
  //byte13
  UINT8 QuiteBoot:1;
  UINT8 Numlock:1;
  UINT8 OpromMessage:1;
  UINT8 ShellEn:1;
  UINT8 UefiNetworkStack:1;
  UINT8 RetryPxeBoot:1;
  UINT8 Reserved8:1;
}BMC_CONFIG_SETUP;

extern EFI_GUID gEfiPlatformCommInfoGuid;

VOID *GetPlatformCommInfo(VOID);

VOID *GetCpuInfoHobData(VOID);

EFI_STATUS RtcWaitToUpdate();

UINT8 CheckAndConvertBcd8ToDecimal8(UINT8 Value);

BOOLEAN
LibCheckCmosBad (
  VOID
  );

VOID
LibClearCmosBad (
  VOID
  );

VOID *GetS3RecordTable();
VOID SetS3RecordTable(UINT32 Address);

/**
  Translate Data Structure between BMC_CONFIG_SETUP and SETUP_DATA.
  
  @param CmdData       BMC_CONFIG_SETUP.
  @param SetupData     SETUP_DATA.
  @param Action           0, Copy from BIOS; 1, Set to BMC CmdData.

  @retval  EFI_SUCCESS
  @retval  EFI_INVALID_PARAMETER
**/
EFI_STATUS 
TranslateBmcConfig (
  BMC_CONFIG_SETUP    *CmdData,
  SETUP_DATA    *SetupData,
  UINT8    Action
  );


#endif


