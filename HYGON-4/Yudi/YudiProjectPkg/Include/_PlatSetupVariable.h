#ifndef __PLAT_SETUP_VARIABLE_H__
#define __PLAT_SETUP_VARIABLE_H__

// This file is defined at CrbProject\ProjectPkg\_PlatSetupVariable.h, 
// and copy it to CrbProject\Platform4Pkg\Include\PlatSetupVariable.h.
// So if you want to update it, please modify "_PlatSetupVariable.h".

#define ADD_ON_SETUP_DATA  \
  UINT8 PcieDis[32]; \
  UINT8 IpmiSelProgCodeEn; \
  UINT8 SataPortEn[8]; \
  UINT8 UsbPortDis[8]; \
  UINT8 HideBrandLogo;  \
  UINT8 RasMemCeLogPolicy;  \
  UINT8 RasCpuCeLogPolicy; \
  UINT8 PowerOffRemove; \
  UINT8 PcieSlotNamingMethod; \
  UINT8 SataVolatileWriteCacheDis; \
  UINT8 PxeNicPortSelect; \
  UINT8 ReservedBusesForSriovDev; \
  UINT8 PcieAspmL1SS; \
  UINT8 PcieRetrainingCount; \
  UINT16 PcieLinkTrainingTimeout; \
  UINT8 FlashRollBackProtect; \
  UINT16 ScreenLockTime; \
  UINT8 PasswordReminderDays; \
  UINT8 ForceToSetup; \
  UINT16 WaitToShowPostInfo; \
  UINT8 SataEn1; \
  UINT8 SataEn2; \
  UINT8 SataEn3; \
  UINT8 Pfeh;  \
  UINT8 Pci64Limit;  \
  UINT8 BIOSLock;  \
  UINT8 PerfMode;  \
  UINT8 CpuCstate;  \
  UINT8 CpuSpeed;  \
  UINT8 AfterPowerLoss;  \
  UINT16 McaErrThreshCount;  \
  UINT8 ScreenPrintNum; \
  UINT8 WakeOnLan; \
  UINT8 SlimlineSata1En; \
  UINT8 SlimlineSata2En; \
  UINT8 USBRearCtrl; \
  UINT8 USBFronCtrl; \
  UINT8 CbsCmnGnbNbIOMMU; \
  UINT8 DealErrorDimm; \
  UINT8 BiosUpdateParm; \
  UINT8 FtpmSupport; \
  UINT8 DtpmSupport; \
  UINT8  SelectPxeNic;\
  UINT16  PxeNicVid;\
  UINT16  PxeNicDid;\
  UINT16  PxeNicMacCrc;\
  UINT8  PXEBootPriority;\
  UINT8  AsmediaController1;\
  UINT8  AsmediaController2;\
  UINT8 LegacyOpromMessage;\
  UINT16  RebootTime;\
  UINT8 CustomPdCkeck; \
  UINT8 PdComplexity; \
  UINT8 PdLength; \
  UINT8 PdTryCount; \
  UINT8  RasPcieCe; \
  UINT8  PciePassThroughCfg; \
  UINT8  IsMcioAutoSwitchToSata; \
  UINT8  FchSpreadSpectrum; \
  UINT8  MemCeStorm; \
  UINT8  MemCeAccumulation; \
  UINT8  HighestPriorityDevice; \
  UINT8  LocalHddBootDis; \
  UINT8  PostHotKeyDis; \
  UINT8  DmiVendorChange; \
  UINT8  CpuSmtMode; \
  UINT8  CpuSvm; \
  UINT8 PlatReserved[64]; \


#define ADD_ON_SETUP_DATA_AT_HEADER \


#define ADD_ON_SETUP_VOLATILE_DATA \
  UINT16 PcieSlotCount; \
  UINT16 Dummy0[2]; \
  UINT8  HygonSata0HwPresent[8]; \
  UINT8 AdvanceControl; \
  UINT8  DynamicText;\
  UINT8  HideBjTime;\

#define DYNAMIC_TEXT(CaptionToken, HelpToken, ValueToken, Key)\
  oneof varid        = SETUP_VOLATILE_DATA.DynamicText,\
    questionid       = Key,\
    prompt           = CaptionToken,\
    help             = HelpToken,\
    option text      = ValueToken, value = 0, flags = INTERACTIVE | DEFAULT;\
    refresh interval = 1 \
  endoneof

#define DYNAMIC_TEXT_GRAY(CaptionToken, HelpToken, ValueToken, Key)\
  grayoutif TRUE;\
    oneof varid        = SETUP_VOLATILE_DATA.DynamicText,\
      questionid       = Key,\
      prompt           = CaptionToken,\
      help             = HelpToken,\
      option text      = ValueToken, value = 0, flags = INTERACTIVE | DEFAULT;\
      refresh interval = 1 \
    endoneof;\
  endif

#endif

