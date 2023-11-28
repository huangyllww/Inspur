
#include <Uefi.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Uefi/UefiSpec.h>
#include <IndustryStandard/Pci.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/Atapi.h>
#include <IndustryStandard/Smbios.h>
#include <Uefi/UefiAcpiDataTable.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/TimerLib.h>
#include <Library/BaseLib.h>
#include <Library/PciLib.h>
#include <Library/PlatformCommLib.h>
#include <Library/DevicePathLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/ScsiPassThruExt.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/PciIo.h>
#include <Protocol/DevicePathToText.h>
#include <RtcDef.h>
#include <PlatformDefinition.h>
#include <Protocol/ScsiIo.h>
#include <PlatS3Record.h>


extern EFI_GUID gPlatformSetupVariableGuid;
extern EFI_GUID gEfiConsoleOutDeviceGuid;




UINT8 CmosRead(UINT8 Address)
{
  IoWrite8(PCAT_RTC_ADDRESS_REGISTER, (UINT8)(Address|(UINT8)(IoRead8(PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  return IoRead8(PCAT_RTC_DATA_REGISTER);
}

VOID CmosWrite(UINT8 Address, UINT8 Data)
{
  IoWrite8(PCAT_RTC_ADDRESS_REGISTER, (UINT8)(Address|(UINT8)(IoRead8(PCAT_RTC_ADDRESS_REGISTER) & 0x80)));
  IoWrite8(PCAT_RTC_DATA_REGISTER, Data);
}


VOID *GetCpuInfoHobData(VOID)
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *CpuInfo;

  GuidHob.Raw = GetFirstGuidHob(&gEfiCpuInfoHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  CpuInfo = (VOID*)GET_GUID_HOB_DATA(GuidHob);

  return CpuInfo;
}


VOID *GetSetupDataHobData(VOID)
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *SetupData;

  GuidHob.Raw = GetFirstGuidHob(&gPlatformSetupVariableGuid);
  ASSERT(GuidHob.Raw != NULL);
  SetupData = (VOID*)(GuidHob.Guid+1);

  return SetupData;
}

VOID *GetBMCConfigValueHob(VOID)
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *Info;

  GuidHob.Raw = GetFirstGuidHob(&gBMCConfigSetupVariableGuid);
  ASSERT(GuidHob.Raw != NULL);
  Info = (VOID*)(GuidHob.Guid+1);

  return Info;
}


VOID *GetPlatformCommInfo(VOID)
{
  EFI_PEI_HOB_POINTERS  GuidHob;
  VOID                  *Info;

  GuidHob.Raw = GetFirstGuidHob(&gEfiPlatformCommInfoGuid);
  ASSERT(GuidHob.Raw != NULL);
  Info = (VOID*)(GuidHob.Guid+1);
  ASSERT((*(UINT32*)Info) == PLAT_COMM_INFO_SIGN);

  return Info;
}


VOID *GetS3RecordTable()
{
  PLATFORM_S3_RECORD               *S3Record;
  
  S3Record = (PLATFORM_S3_RECORD*)(UINTN)PcdGet32(PcdS3RecordAddr);
  ASSERT(S3Record->Signature == PLAT_S3_RECORD_SIGNATURE);

  return (VOID*)S3Record;
}

VOID SetS3RecordTable(UINT32 Address)
{
  PcdSet32(PcdS3RecordAddr, Address);
}


VOID *GetCarTopData()
{
  EFI_PEI_HOB_POINTERS   GuidHob;
  CAR_TOP_DATA           *CarTopData;

  GuidHob.Raw = GetFirstGuidHob(&gCarTopDataHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  CarTopData  = (CAR_TOP_DATA*)(GuidHob.Guid+1);

  return (VOID*)CarTopData;
}




VOID
ConvertIdentifyDataFormat (
  IN  UINT8   *Data,
  IN  UINTN   DataSize
  )
{
  UINTN  Index;
  UINT8  Data8;

  ASSERT(DataSize!=0 && Data!=NULL);

// here has a hidden bug:
// Data size may be odd number, so it will destroy data out of range.
  DataSize &= ~BIT0;          // Fix: make it be even.
  for(Index = 0; Index < DataSize; Index += 2){
    Data8         = Data[Index];
    Data[Index]   = Data[Index+1];
    Data[Index+1] = Data8;
  }

}



EFI_STATUS RtcWaitToUpdate()
{
  RTC_REGISTER_A  RegisterA;
//RTC_REGISTER_D  RegisterD;
	UINTN           Timeout;


  Timeout = PcdGet32(PcdRealTimeClockUpdateTimeout);

//RegisterD.Data = CmosRead (RTC_ADDRESS_REGISTER_D);
//if (RegisterD.Bits.Vrt == 0) {
//  return EFI_DEVICE_ERROR;
//}

  Timeout         = (Timeout / 10) + 1;
  RegisterA.Data  = CmosRead (RTC_ADDRESS_REGISTER_A);
  while (RegisterA.Bits.Uip == 1 && Timeout > 0) {
    MicroSecondDelay (10);
    RegisterA.Data = CmosRead (RTC_ADDRESS_REGISTER_A);
    Timeout--;
  }
  if (Timeout == 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


UINT8 CheckAndConvertBcd8ToDecimal8(UINT8 Value)
{
  if ((Value < 0xa0) && ((Value & 0xf) < 0xa)) {
    return BcdToDecimal8(Value);
  }
  return 0xff;
}



// after CMOS battery lost, data in cmos will be changed to 00.
BOOLEAN
LibCheckCmosBad (
  VOID
  )
{
  UINT8    Data1, Data2;    
  
  Data1 = CmosRead(0x2e);
  Data2 = CmosRead(0x2f);
  
  if(Data1 == 0 && Data2 == 0){
    return TRUE;        
  }else{
    return FALSE;
  }
}


VOID
LibClearCmosBad (
  VOID
  )
{
  CmosWrite(0x2e, 0x55);
  CmosWrite(0x2f, 0xAA);
}
  

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
  )
{
  if (CmdData == NULL || SetupData == NULL) {
    return EFI_INVALID_PARAMETER;
  }  
  DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), Action :%d.\n", __LINE__, Action));

  if (Action ==0 ) {
  //
  //Copy from BIOS.
  //
  CmdData->ReadFlag = 1;//Flag: send command success
  CmdData->Reserved1 = 0;
  CmdData->Reserved2 = 0;
  CmdData->Reserved3 = 0;
  CmdData->Reserved4 = 0;
  CmdData->Reserved5 = 0;
  CmdData->Reserved6 = 0;
  CmdData->Reserved7 = 0;
  CmdData->Reserved8 = 0;
  	
  CmdData->AmdSmtMode = SetupData->AmdSmtMode;
  CmdData->SriovDis = SetupData->SriovDis;
  CmdData->CbsCmnSVMCtrl = SetupData->CbsCmnSVMCtrl;
  if(SetupData->CbsCmnGnbNbIOMMU == 0xf){
    CmdData->CbsCmnGnbNbIOMMU = 2;
  }else {
    CmdData->CbsCmnGnbNbIOMMU = SetupData->CbsCmnGnbNbIOMMU;
  }

  CmdData->LanBoot = SetupData->LanBoot;
  CmdData->BIOSLock = SetupData->BIOSLock;

  CmdData->BootModeType = SetupData->BootModeType - 1;
  if(SetupData->LanBoot == 0){
  	CmdData->Ipv4Support = 0;
	CmdData->Ipv6Support = 0;
  } else {
  	if(SetupData->NetBootIpVer == 0){ // all support
      CmdData->Ipv4Support = 1;
	  CmdData->Ipv6Support = 1;
	} else if(SetupData->NetBootIpVer == 1){
      CmdData->Ipv4Support = 1;
	  CmdData->Ipv6Support = 0;
	} else {
      CmdData->Ipv4Support = 0;
	  CmdData->Ipv6Support = 1;
	}
  }

  CmdData->UCREnable = SetupData->UCREnable;
  CmdData->SerialBaudrate = SetupData->SerialBaudrate;
  CmdData->Pfeh = SetupData->Pfeh;
  
  switch(SetupData->McaErrThreshCount){
  	case 0:
	  CmdData->McaErrThreshCount = 0;
      break;
  	case 0xFFE:
	  CmdData->McaErrThreshCount = 1;
      break;
  	case 0xFFA:
	  CmdData->McaErrThreshCount = 2;
      break;
  	case 0xFF5:
	  CmdData->McaErrThreshCount = 3;
      break;	
  	case 0xF9B:
	  CmdData->McaErrThreshCount = 4;
      break;
  	case 0xC17:
	  CmdData->McaErrThreshCount = 5;
	  break;
  	case 0x82f:
	  CmdData->McaErrThreshCount = 6;
      break;
	default:
	  CmdData->McaErrThreshCount = 1;
      break;
		
  }
  DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d),CmdData->McaErrThreshCount :0%x.\n", __LINE__, CmdData->McaErrThreshCount));
  
  CmdData->ObLanEn = SetupData->ObLanEn;
  CmdData->WakeOnLan = SetupData->WakeOnLan;
  CmdData->VideoPrimaryAdapter = SetupData->VideoPrimaryAdapter;
  CmdData->USBRearCtrl = SetupData->USBRearCtrl;
  CmdData->USBFronCtrl = SetupData->USBFronCtrl;
  CmdData->UsbMassSupport = SetupData->UsbMassSupport;
  CmdData->PcieMaxPayloadSize = SetupData->PcieMaxPayloadSize;
  CmdData->PcieAspm = SetupData->PcieAspm;
  CmdData->PerfMode = SetupData->PerfMode;
  CmdData->AMDPstate = SetupData->AMDPstate;
  CmdData->AMDCstate = SetupData->AMDCstate;
  CmdData->CpuSpeed = SetupData->CpuSpeed;
  CmdData->BootTimeout[0] = (UINT8)SetupData->BootTimeout;
  CmdData->BootTimeout[1] = (UINT8)(SetupData->BootTimeout>>8);
  CmdData->QuiteBoot = SetupData->QuiteBoot;
  CmdData->Numlock = SetupData->Numlock;
  CmdData->OpromMessage = SetupData->OpromMessage;
  CmdData->ShellEn = SetupData->ShellEn;
  CmdData->UefiNetworkStack = SetupData->UefiNetworkStack;
  CmdData->RetryPxeBoot = SetupData->RetryPxeBoot;
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), AMDPstate :%d, AMDCstate :%d.\n", CmdData->AMDPstate, CmdData->AMDCstate));
   	
  }  //End, Copy from BIOS.


  if (Action ==1 ) {
  //
  //Set to BMC CmdData.
  //  	
  SetupData->AmdSmtMode = CmdData->AmdSmtMode;
  SetupData->SriovDis = CmdData->SriovDis;
  SetupData->CbsCmnSVMCtrl = CmdData->CbsCmnSVMCtrl;
  if(CmdData->CbsCmnGnbNbIOMMU  == 2){
    SetupData->CbsCmnGnbNbIOMMU = 0xf;
  }else{
    SetupData->CbsCmnGnbNbIOMMU = CmdData->CbsCmnGnbNbIOMMU;
  }

  SetupData->LanBoot = CmdData->LanBoot;
  SetupData->BIOSLock = CmdData->BIOSLock;

  SetupData->BootModeType = CmdData->BootModeType + 1;  
  if(CmdData->Ipv4Support == 0 && CmdData->Ipv6Support == 0 ){
  	//SetupData->LanBoot = 0;
  	SetupData->NetBootIpVer = 1;
  }else if (CmdData->Ipv4Support == 1 && CmdData->Ipv6Support == 0){
  	//SetupData->LanBoot = 1;
  	SetupData->NetBootIpVer = 1;
  }else if (CmdData->Ipv4Support == 0 && CmdData->Ipv6Support == 1){
  	//SetupData->LanBoot = 1;
  	SetupData->NetBootIpVer = 2;
  }else {
  	//SetupData->LanBoot = 1;
  	SetupData->NetBootIpVer = 0;
  }

  
  SetupData->UCREnable = CmdData->UCREnable;
  SetupData->SerialBaudrate = CmdData->SerialBaudrate;
  SetupData->Pfeh = CmdData->Pfeh;

  switch(CmdData->McaErrThreshCount){
  	case 0:
	  SetupData->McaErrThreshCount = 0;
      break;
  	case 1:
	  SetupData->McaErrThreshCount = 0xFFE;
      break;
  	case 2:
	  SetupData->McaErrThreshCount = 0xFFA;
      break;
  	case 3:
	  SetupData->McaErrThreshCount = 0xFF5;
      break;	
  	case 4:
	  SetupData->McaErrThreshCount = 0xF9B;
      break;
  	case 5:
	  SetupData->McaErrThreshCount = 0xC17;
      break;
  	case 6:
	  SetupData->McaErrThreshCount = 0x82f;
      break;
	default:
	  SetupData->McaErrThreshCount = 0xFFE;
	  break;
  }
  DEBUG((EFI_D_ERROR, __FUNCTION__"(L %d), SetupData->McaErrThreshCount :0x%x.\n", __LINE__, SetupData->McaErrThreshCount));

  SetupData->ObLanEn = CmdData->ObLanEn;
  SetupData->WakeOnLan = CmdData->WakeOnLan;
  SetupData->VideoPrimaryAdapter = CmdData->VideoPrimaryAdapter;
  SetupData->USBRearCtrl = CmdData->USBRearCtrl;
  SetupData->USBFronCtrl = CmdData->USBFronCtrl;
  SetupData->UsbMassSupport = CmdData->UsbMassSupport;
  SetupData->PcieMaxPayloadSize = CmdData->PcieMaxPayloadSize;
  SetupData->PcieAspm = CmdData->PcieAspm;
  SetupData->PerfMode = CmdData->PerfMode;  	
  if(CmdData->PerfMode == 0){
    SetupData->AMDPstate = CmdData->AMDPstate;
    SetupData->AMDCstate = CmdData->AMDCstate;
  } else {
    SetupData->AMDPstate = 2;
    SetupData->AMDCstate = 0;
  }
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), AMDPstate :%d, AMDCstate :%d.\n", SetupData->AMDPstate, SetupData->AMDCstate));
  
  SetupData->CpuSpeed = CmdData->CpuSpeed;
  SetupData->BootTimeout = (CmdData->BootTimeout[1] << 8) + (CmdData->BootTimeout[0]) ;
  SetupData->QuiteBoot = CmdData->QuiteBoot;
  SetupData->Numlock = CmdData->Numlock;
  SetupData->OpromMessage = CmdData->OpromMessage;
  SetupData->ShellEn = CmdData->ShellEn;
  SetupData->UefiNetworkStack = CmdData->UefiNetworkStack;
  SetupData->RetryPxeBoot = CmdData->RetryPxeBoot;

  	
  }  //End, Set to BMC CmdData.


  return EFI_SUCCESS;
}




