#include <Uefi.h>


typedef struct{
  UINT8 ReadFlag;
  //byte1
  UINT8 CpuSmtMode:1;
  UINT8 SriovDis:1;
  UINT8 CpuSvm:1;
  UINT8 CbsCmnGnbNbIOMMU:2;
  UINT8 CbsCmnCpuRdseedRdrandCtrl:1;
  UINT8 PXEBootPriority:2;
  //byte2
  UINT8 CbsDfCmnMemIntlv:3;
  UINT8 CbsCmnCpuCpb:1;
  UINT8 LanBoot:1;
  UINT8 BIOSLock:1;
  UINT8 HideBrandLogo:1;
  UINT8 PcieSlotNamingMethod:1;
  //byte3
  UINT8 BootModeType:1;
  UINT8 Ipv4Support:1;
  UINT8 Ipv6Support:1;
  UINT8 PerfMode2:3;
  UINT8 FRB2WatchDog:1;
  UINT8 AesSupportDis:1;
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
  UINT8 CbsCmnSmeeCtrl:1;
  UINT8 TerminalType:2;
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
  UINT8 PxeNicPortSelectBit0:1;
  //byte10 
  UINT8 PerfMode:1;
  UINT8 CpuPstateEn:2;
  UINT8 CpuCstate:1;
  UINT8 CpuSpeed:2;
  UINT8 PxeNicPortSelectBit1Bit2:2;
  //byte11/12
  UINT8 BootTimeout[2];
  //byte13  
  UINT8 QuietBoot:1;
  UINT8 Numlock:1;
  UINT8 LegacyOpromMessage:1;
  UINT8 ShellEn:1;
  UINT8 UefiNetworkStack:1;
  UINT8 RetryPxeBoot:1;
  UINT8 Reserved8:2;
}BMC_CONFIG_SETUP;


#pragma pack(1)

typedef struct{
  //byte0
  UINT8 Configuration;
  //byte1
  UINT8 Numa;
  //byte2
  UINT8 SNC;
  //byte3
  UINT8 HT;
  //byte4
  UINT8 Turbo;
  //byte5
  UINT8 EIST;
  //byte6
  UINT8 CPUC6report;
  //byte7
  UINT8 EnHanceHaltStat;
  //byte8
  UINT8 IPv6PXESupport;
  //byte9
  UINT8 DMIVendorChange;
  //byte10
  UINT8 POSTHotkeySupport;
  //byte11
  UINT8 LocalHDDBoot;
  //byte12
  UINT8 ActiveVideo;
  //byte13
  UINT8 UmaBasedClustering;
  //byte14
  UINT8 TotalMemoryEncryption;
  //byte15
  UINT8 SWGurdExtensions;
  //byte16
  UINT8 SGXfactoryReset;
  //byte17
  UINT8 Patrolscrub;
  //byte18
  UINT8 PRMRRSize;
  //byte19
  UINT8 ActiveCoreslow;
  //byte20
  UINT8 ActiveCoreshigh;
  //byte21
  UINT8 SGXAutoMPRegistrationAgent;
}BYTEDANCE_BMC_CONFIG;

typedef struct{
  //byte0
  UINT8 Configuration;
  //byte1
  UINT8 Turbo;
  //byte2
  UINT8 HT;
  //byte3
  UINT8 VT;
  //byte4
  UINT8 EIST;
  //byte5
  UINT8 Numa;
  //byte6
  UINT8 VenderChange;
  //byte7
  UINT8 VTd;
  //byte8 
  UINT8 SRIOV;
  //byte9
  UINT8 ActiveVideo;
  //byte10
  UINT8 LocalHddBoot;
  //byte11  
  UINT8 HotKeySupport;
  //byte12
  UINT8 InterSpeedSelect;
  //byte13
  UINT8 IMS;
  //byte14
  UINT8 TPM;
  //byte15
  UINT8 PowerOffMove;
  //byte16
  UINT8 BootModeType;
  //byte17
  UINT8 ActiveCore;
}ALIMOC_BMC_CONFIG;

typedef struct{
  //byte0
  UINT8 Turbo;
  //byte1
  UINT8 HT;
  //byte2
  UINT8 VT;
  //byte3
  UINT8 EIST;
  //byte4
  UINT8 Numa;
  //byte5
  UINT8 VenderChange;
  //byte6
  UINT8 VTd;
  //byte7 
  UINT8 SRIOV;
  //byte8
  UINT8 ActiveVideo;
  //byte9
  UINT8 LocalHddBoot;
  //byte10  
  UINT8 HotKeySupport;
  //byte11
  UINT8 InterSpeedSelect;
  //byte12
  UINT8 IMS;
  //byte13
  UINT8 TPM;
  //byte14
  UINT8 PowerOffMove;
  //byte15
  UINT8 BootModeType;
  //byte16
  UINT8 ActiveCore;
} ALIMOC_BMC_CONFIG_NO_CONFIGURATION;

typedef struct{
  UINT8 Guid[16];
}EFI_IPMI_GET_SYSTEM_GUID;

typedef struct{
  UINT8 Offest;
  UINT8 DataCont;
}READ_BIOS_REQUEST_DATA;

typedef struct{
  UINT8 Length;
  EFI_IPMI_GET_SYSTEM_GUID Data;
}GET_BMC_UUID_RES_DATA;

typedef struct{
  UINT8 Offset;
  UINT8 DataCont;
  BYTEDANCE_BMC_CONFIG Data;
}SET_BIOS_REQUEST_DATA2;

typedef struct{
  UINT8 Offset;
  UINT8 DataCont;
  ALIMOC_BMC_CONFIG Data;
}SET_BIOS_REQUEST_DATA3;

typedef struct{
  UINT8 Offset;
  UINT8 DataCont;
  ALIMOC_BMC_CONFIG_NO_CONFIGURATION Data;
}SET_BIOS_REQUEST_DATA_NO_CONFIGURATION;

typedef struct{
  UINT8                DataCont;
  BYTEDANCE_BMC_CONFIG Cfg;
} READ_BIOS_REQUEST_DATA_RES;

typedef struct{
  UINT8                DataCount;
  ALIMOC_BMC_CONFIG    Cfg;
} READ_BIOS_REQUEST_DATA_RES2;
#pragma pack()

