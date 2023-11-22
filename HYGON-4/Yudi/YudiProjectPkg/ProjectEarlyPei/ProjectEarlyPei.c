

#include <PiPei.h>
#include <Pi/PiBootMode.h>      // HobLib.h +
#include <Pi/PiHob.h>           // HobLib.h +
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/ByoCommLib.h>
#include <Library/PrintLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PrintLib.h>
#include <Library/BiosIdLib.h>
#include <SetupVariable.h>
#include <ByoStatusCode.h>
#include <Library/PlatformCommLib.h>
#include <BoardIdType.h>


EFI_STATUS
PrjCpmTableOverride (
  IN       EFI_PEI_SERVICES       **PeiServices
  );

EFI_STATUS
EFIAPI
PrjEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS PrjPeiSendBmcProgressCode();

EFI_STATUS
EFIAPI
PrjPeiRscHandlerCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
UpdateBootModeCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
PrjSetupDataReadyCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  CONST SETUP_DATA   *SetupHob;
        UINTN        Index;
        UINT32       PortDis;
        UINT8        *UsbDisableMap;
        UINTN        UsbDisableMapCount;       
        UINT8        XhciPortDisMap[32];
        UINTN        SizeofBuffer;


  DEBUG((EFI_D_INFO, "PrjSetupDataReadyCallBack\n"));

  SetupHob = (CONST SETUP_DATA*)GetSetupDataHobData();

  UsbDisableMap      = PcdGetPtr(PcdXhciPlatformPortDisableMapHyEx);
  UsbDisableMapCount = PcdGetSize(PcdXhciPlatformPortDisableMapHyEx);
  ASSERT(sizeof(XhciPortDisMap) >= UsbDisableMapCount);
  CopyMem(XhciPortDisMap, UsbDisableMap, UsbDisableMapCount);

  if(SetupHob->UsbPortDis[0]){
    XhciPortDisMap[0] |= BIT0 | BIT4;
  } else {
    XhciPortDisMap[0] &= (UINT8)~(BIT0 | BIT4);
  }
  if(SetupHob->UsbPortDis[1]){
    XhciPortDisMap[0] |= BIT1 | BIT5;
  } else {
    XhciPortDisMap[0] &= (UINT8)~(BIT1 | BIT5);
  }
  if(SetupHob->UsbPortDis[2]){
    XhciPortDisMap[0] |= BIT2 | BIT6;
  } else {
    XhciPortDisMap[0] &= (UINT8)~(BIT2 | BIT6);
  }
  if(SetupHob->UsbPortDis[3]){
    XhciPortDisMap[0] |= BIT3 | BIT7;
  } else {
    XhciPortDisMap[0] &= (UINT8)~(BIT3 | BIT7);
  }

  if(SetupHob->UsbPortDis[4]){
    XhciPortDisMap[1] |= BIT0 | BIT4;
  } else {
    XhciPortDisMap[1] &= (UINT8)~(BIT0 | BIT4);
  }
  if(SetupHob->UsbPortDis[5]){
    XhciPortDisMap[1] |= BIT1 | BIT5;
  } else {
    XhciPortDisMap[1] &= (UINT8)~(BIT1 | BIT5);
  }
  if(SetupHob->UsbPortDis[6]){
    XhciPortDisMap[1] |= BIT2 | BIT6;
  } else {
    XhciPortDisMap[1] &= (UINT8)~(BIT2 | BIT6);
  }
  if(SetupHob->UsbPortDis[7]){
    XhciPortDisMap[1] |= BIT3 | BIT7;
  } else {
    XhciPortDisMap[1] &= (UINT8)~(BIT3 | BIT7);
  }

  DumpMem8(XhciPortDisMap, sizeof(XhciPortDisMap));
  SizeofBuffer = sizeof(XhciPortDisMap);
  PcdSetPtrS(PcdXhciPlatformPortDisableMapHyEx, &SizeofBuffer, (VOID*)XhciPortDisMap);


  PcdSet8S(PcdSataPortPowerPort0, !SetupHob->SataPortEn[0]);
  PcdSet8S(PcdSataPortPowerPort1, !SetupHob->SataPortEn[1]);
  PcdSet8S(PcdSataPortPowerPort2, !SetupHob->SataPortEn[2]);
  PcdSet8S(PcdSataPortPowerPort3, !SetupHob->SataPortEn[3]);
  PcdSet8S(PcdSataPortPowerPort4, !SetupHob->SataPortEn[4]);
  PcdSet8S(PcdSataPortPowerPort5, !SetupHob->SataPortEn[5]);
  PcdSet8S(PcdSataPortPowerPort6, !SetupHob->SataPortEn[6]);
  PcdSet8S(PcdSataPortPowerPort7, !SetupHob->SataPortEn[7]);

  PortDis = PcdGet32(PcdSataPortPowerPortHyEx);
  for(Index=0; Index<ARRAY_SIZE(SetupHob->SataPortEn); Index++){
    if(!SetupHob->SataPortEn[Index]){
      PortDis |= (UINT32)(1 << Index);
    }
  }
  PcdSet32S(PcdSataPortPowerPortHyEx, PortDis);
	
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), IpmiSelProgCodeEn :%d.\n", __LINE__, SetupHob->IpmiSelProgCodeEn));
  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
PrjCpmOverrideTableCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  return PrjCpmTableOverride(PeiServices);
}


EFI_STATUS
EFIAPI
PrjEndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  CONST SETUP_DATA   *SetupHob; 
  PLATFORM_COMM_INFO *Info;
  UINTN              BufferSize;
  CHAR8              Buffer[16];

  SetupHob = GetSetupDataHobData();
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), IpmiSelProgCodeEn :%d.\n", __LINE__, SetupHob->IpmiSelProgCodeEn));
  if(SetupHob->IpmiSelProgCodeEn){
    PrjPeiSendBmcProgressCode();
  }

  Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  if(!IsZeroBuffer(Info->BmcVersion, sizeof(Info->BmcVersion))){
    AsciiSPrint(Buffer, sizeof(Buffer), "%d.%d.%d", Info->BmcVersion[2], Info->BmcVersion[1], Info->BmcVersion[0]);
    BufferSize = AsciiStrSize(Buffer);
    DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), PcdBmcFwVerStr8 :%a.\n", __LINE__, Buffer));
    PcdSetPtrS(PcdBmcFwVerStr8, &BufferSize, Buffer);
  }

  return EFI_SUCCESS;
}


VOID
GetClearPWDStatusFromJumper (
  )
{
  BOOLEAN  IsClear = FALSE;

  switch(PcdGet8(PcdBoardIdType)){
    case TKN_BOARD_ID_TYPE_NHVTB1:
//-   LibHygonSetGpio(mGPIO_DEF_PIN(0, 0, 70), mGPIO_DEF_SET(GPIO_FUNCTION_0, GPIO_INPUT, GPIO_PU_EN) );
//-   IsClear = !LibHygonGetGpio(mGPIO_DEF_PIN(0, 0, 70)); 
      break;
  }
  
  if (IsClear) {
    PcdSet8S(PcdIsClearSystemPwd, 1);
  }
}




static BIOS_ID_BOARD_ID gBiosIdBoardIdList[] = {
  {{L"HYG3000"}, {L"CRB"}, {"HYG3000"}},
};

EFI_STATUS
EFIAPI
CreateBiosIdInfoHob (
    VOID
  )
{
  EFI_STATUS             Status;
  EFI_BOOT_MODE          BootMode;
  UINT32                 FvStart;
  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR(Status);
  if(BootMode == BOOT_IN_RECOVERY_MODE){
    FvStart = PcdGet32(PcdFlashFvRecoveryBackUpBase);
  } else {
    FvStart = PcdGet32(PcdFlashFvRecoveryBase);
  }

  Status = BuildBiosIdInfoHob ((VOID*)(UINTN)FvStart, gBiosIdBoardIdList, ARRAY_SIZE(gBiosIdBoardIdList));
  ASSERT_EFI_ERROR(Status);

  return Status;
}


EFI_STATUS
EFIAPI
UpdateBootModeCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  CreateBiosIdInfoHob();
  GetClearPWDStatusFromJumper();
  return EFI_SUCCESS;
}


STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gPrjPpiNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gByoHygonCpmTableOverrideHookPpiGuid,
    PrjCpmOverrideTableCallback,
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiEndOfPeiSignalPpiGuid,
    PrjEndOfPeiCallback
  },  
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiRscHandlerPpiGuid,
    PrjPeiRscHandlerCallBack
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiMasterBootModePpiGuid,
    UpdateBootModeCallBack
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gByoSetupDataReadyPpiGuid,
    PrjSetupDataReadyCallBack
  },

};


EFI_STATUS
EFIAPI
ProjectEarlyPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS   Status;

  ASSERT(PcdGet16(PcdHygonFchCfgSmiCmdPortAddr) == PcdGet16(PcdSwSmiCmdPort));
  ASSERT(PcdGet16(PcdHygonFchCfgAcpiPm1EvtBlkAddr) == PcdGet16(AcpiIoPortBaseAddress));

  switch(PcdGet8(PcdBoardIdType)){
    case TKN_BOARD_ID_TYPE_NHVTB3:
      PcdSet32S(PcdSataEnableHyEx, (PcdGet32(PcdSataEnableHyEx) & 0xFFFFFFF0)|BIT0);
      break;
  }

  Status = PeiServicesNotifyPpi(gPrjPpiNotifyList);
  ASSERT_EFI_ERROR(Status);

  return Status;
}


