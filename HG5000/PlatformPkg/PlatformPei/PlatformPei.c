
//------------------------------------------------------------------------------
#include "PlatformPei.h"
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <SetupVariable.h>
#include <ByoStatusCode.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/MultiPlatSupportLib.h>
#include <AmdCpu.h>
#include <token.h>
#include <Fch.h>
#include <Ppi/IpmiTransportPpi.h>




BMC_CONFIG_SETUP *gTransData;

//------------------------------------------------------------------------------
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );  

EFI_STATUS
EFIAPI
EndOfPeiCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

// For Port 0x61, program timer 1 as refresh timer
  IoWrite8(0x43, 0x54);
  IoWrite8(0x41, 0x12); 

// [7]   ForcePwrOn
// [5:4] PwrFailOption
//         11b Resume to the same setting before power fails

  MmioWrite8(ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG59, 0xFF);

  return EFI_SUCCESS;
}
  
STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gPpiNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiEndOfPeiSignalPpiGuid,
    EndOfPeiCallback  
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiPeiMemoryDiscoveredPpiGuid,
    MemoryDiscoveredPpiNotifyCallback 
  }
};








//----------------------------------------------------------------------------
CONST EFI_PEI_PPI_DESCRIPTOR gSetupDataReadyPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gByoSetupDataReadyPpiGuid,
  NULL
};



void PlatformUsbSetting(SETUP_DATA *SetupData, PLATFORM_COMM_INFO *Info)
{
  UINT8          XhciPortDisMap[8];
  UINTN          SizeofBuffer = sizeof(XhciPortDisMap);
//  UINT8          Port;


//Die 0 1 4 5, has XHCI controller, 1 mean disable
//
//DieX Bit0 1 -- USB2.0 
//     Bit4 5 -- USB3.0 port
//
// [2.0]  BIT0 ~ BIT3
// [3.0]  BIT4 ~ BIT7

  ZeroMem(XhciPortDisMap, sizeof(XhciPortDisMap));


  if(SetupData->USBRearCtrl){
    XhciPortDisMap[0] = 0;
  } else {
    XhciPortDisMap[0] = 255;
  }
  if(SetupData->USBFronCtrl){
    XhciPortDisMap[1] = 0;
  } else {
    XhciPortDisMap[1] = 255;
  }




/*
  for (Port = 0; Port < 4; Port ++) {
    XhciPortDisMap[0] |= (!SetupData->Die0XHCIPort[Port]) << Port;
  }
  XhciPortDisMap[0] |= XhciPortDisMap[0] << 4;

  for (Port = 0; Port < 4; Port ++) {
    XhciPortDisMap[1] |= (!SetupData->Die1XHCIPort[Port]) << Port;
  }
  XhciPortDisMap[1] |= XhciPortDisMap[1] << 4;

  for (Port = 0; Port < 4; Port ++) {
    XhciPortDisMap[4] |= (!SetupData->Die4XHCIPort[Port]) << Port;
  }
  XhciPortDisMap[4] |= XhciPortDisMap[4] << 4;

  for (Port = 0; Port < 4; Port ++) {
    XhciPortDisMap[5] |= (!SetupData->Die5XHCIPort[Port]) << Port;
  }
  XhciPortDisMap[5] |= XhciPortDisMap[5] << 4;
*/
  DumpMem8(XhciPortDisMap, sizeof(XhciPortDisMap));
  
  PcdSetPtr (PcdXhciPlatformPortDisableMap, &SizeofBuffer, (VOID*)XhciPortDisMap);  
}





VOID 
GetBMCSetupConfig(UINT8 Flag,BOOLEAN IsCmosBad)
{
  EFI_STATUS                Status;
  UINT8                     RespondData[14] = {0};
  UINT8                     ResponseDataSize;
  PEI_IPMI_TRANSPORT_PPI		*PeiImpiPpi = NULL;

  DEBUG((EFI_D_INFO, "%a() Setupdata pei\n", __FUNCTION__));
  
  gTransData = (BMC_CONFIG_SETUP*)BuildGuidHob(&gBMCConfigSetupVariableGuid, sizeof(BMC_CONFIG_SETUP));
  
  ResponseDataSize = sizeof(BMC_CONFIG_SETUP);

  Status = PeiServicesLocatePpi(
			  &gPeiIpmiTransportPpiGuid,
			  0,
			  NULL,
			  (VOID **) &PeiImpiPpi
			  );
  if(PeiImpiPpi != NULL) {
	  Status = PeiImpiPpi->SendIpmiCommand(
					   PeiImpiPpi,
					   0x3E,
					   0,
					   0xC2,
					   NULL,
					   0,
					   (UINT8 *)&RespondData,
					   (UINT8 *)&ResponseDataSize
					   );	
    DEBUG((EFI_D_INFO,"GetBmc command is :%r,respon %02x %02x %02x,flag = %02x\n",Status,RespondData[1],RespondData[2],RespondData[3],RespondData[0]));	
  }
  CopyMem(gTransData,RespondData,sizeof(BMC_CONFIG_SETUP));

  DumpMem8(RespondData, ResponseDataSize);
  if(Status != EFI_SUCCESS || ResponseDataSize < 13 || gTransData->ReadFlag > 1){
  	DEBUG((EFI_D_INFO,"Status Not success\n"));
  	return ;
  }
  if(Flag ==1 && gTransData->ReadFlag==0){
  	DEBUG((EFI_D_INFO,"First power on  \n"));
  	return ;
  }
  if(IsCmosBad){
  	DEBUG((EFI_D_INFO,"CMOS clear \n"));
  	return;
  }

  PcdSet8(PcdBmcBiosConfigFlag, 1 );
}




EFI_STATUS 
SetupDataInit(
  EFI_PEI_READ_ONLY_VARIABLE2_PPI *Var2Ppi,
  EFI_BOOT_MODE                    BootMode,
  PLATFORM_COMM_INFO               *Info
        )
{
  SETUP_DATA     *SetupData;
  EFI_STATUS     Status;
  UINTN          Size;
  BOOLEAN        IsCmosBad;
  SETUP_DATA     *VarHobSetupData = NULL;
  UINT8           Tmp;
  UINT8           AmdSmtMode;
  UINT8           CbsCmnSVMCtrl;
  UINT8           CbsCmnGnbNbIOMMU;
  UINT8           SriovDis;
  UINT8           Flag=0;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  
  SetupData = BuildGuidHob(&gPlatformSetupVariableGuid, sizeof(SETUP_DATA));
  ASSERT(SetupData != NULL);

  IsCmosBad = LibCheckCmosBad();
  DEBUG((EFI_D_INFO, "IsCmosBad:%d\n", IsCmosBad));
  PcdSet8(PcdIsPlatformCmosBad, IsCmosBad);
  if(IsCmosBad || BootMode == BOOT_IN_RECOVERY_MODE){
    DEBUG((EFI_D_ERROR, "Setup Need Load Default\n"));
    goto LoadSetupDefault;
  }	
 /* if(CmosRead(0xD0)){
  	 DEBUG((EFI_D_INFO,"Update in setup Will Load default\n"));
	 PcdSet8(PcdSetUpUpdate,1);
     Flag = 1;//first power on  update in setup
     goto LoadSetupDefault;
  }*/

  Size   = sizeof(SETUP_DATA);  
  Status = Var2Ppi->GetVariable (
                      Var2Ppi,
                      PLATFORM_SETUP_VARIABLE_NAME,
                      &gPlatformSetupVariableGuid,
                      NULL,
                      &Size,
                      SetupData
                      );
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), Get Setup Var :%r.\n", Status));
  
  if(Status == EFI_NOT_FOUND){
  	Flag = 1;//first power on
  }
  if(!EFI_ERROR(Status)){
    if (PcdGetBool (PcdSetupDataVersionCheck)) {
      if (SetupData->Version != SETUP_CURRENT_VERSION) {
	  	PcdSet8(PcdVersionCheck,1);
		Flag = 1;
        goto LoadSetupDefault;			  	
      }
    }
    goto GotSetupVariable;
  }	

LoadSetupDefault:
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), Warning, Load Setup Default,\n"));
  Status = CreateDefaultVariableHob(EFI_HII_DEFAULT_CLASS_STANDARD, 0, (VOID**)&VarHobSetupData);
  ASSERT_EFI_ERROR(Status);
  ASSERT(VarHobSetupData != NULL);
  PcdSetBool(PcdFceLoadDefault, TRUE);
  CmosWrite(0xD1,0);
 // CmosWrite(0xD0,0);//Has Load Default Set 0xD0 to default vaule
// FCE tool has some limitation for gathering default value, so do override here.
  FCE_DEFAULT_VALUE_OVERRIDE_DATA;
  
  Size = sizeof(SETUP_DATA);  
  Status = Var2Ppi->GetVariable (
                      Var2Ppi,
                      PLATFORM_SETUP_VARIABLE_NAME,
                      &gPlatformSetupVariableGuid,
                      NULL,
                      &Size,
                      SetupData
                      );
  ASSERT_EFI_ERROR(Status);



//------------------------------------------------------------------------------
GotSetupVariable:

  DEBUG((EFI_D_INFO, "BootModeType :%d\n", SetupData->BootModeType));
  DEBUG((EFI_D_INFO, "sizeof(Setup):%d\n", sizeof(SETUP_DATA)));
  DEBUG((EFI_D_INFO, "Version:%d\n", SetupData->Version));
  
  GetBMCSetupConfig(Flag,IsCmosBad);
DEBUG((EFI_D_INFO,"Value boot timeout = %x+%x\n",gTransData->BootTimeout[0],gTransData->BootTimeout[1]));
  if(Flag == 1){ // First power init ipmi bootmode
  	CmosWrite(0x50,0);
  	CmosWrite(0x51,0);
  	CmosWrite(0x52,0);
  }
  Tmp = PcdGet8(PcdBmcBiosConfigFlag);
  if(PcdGet8(PcdBmcBiosConfigFlag)){
    TranslateBmcConfig (gTransData, SetupData, 1);
  }
  
  if(!EFI_ERROR(Status)){
    if(Tmp){
      AmdSmtMode = gTransData->AmdSmtMode;
      CbsCmnSVMCtrl = gTransData->CbsCmnSVMCtrl;
      CbsCmnGnbNbIOMMU = gTransData->CbsCmnGnbNbIOMMU;
      PcdSet8 (PcdAmdCpbMode, gTransData->CbsCmnCpuCpb);
      PcdSet8(PcdAmdSmtMode, AmdSmtMode);
      PcdSet8(PcdAmdSVMCtrl, CbsCmnSVMCtrl);
	  PcdSet8(PcdAmdNbIOMMU, CbsCmnGnbNbIOMMU);
    }else{    
      PcdSet8(PcdAmdSmtMode, SetupData->AmdSmtMode);
      PcdSet8(PcdAmdSVMCtrl, SetupData->CbsCmnSVMCtrl);
	  PcdSet8(PcdAmdNbIOMMU, SetupData->CbsCmnGnbNbIOMMU);
    }
  }
  if(Tmp){
    SriovDis = gTransData->SriovDis;
   //if(SriovDis){
   //   PcdSet8(PcdAmdNbIOMMU,!SriovDis);
   // }
    PcdSetBool(PcdCfgPcieAriSupport, !SriovDis);
    PcdSetBool(PcdAmdCcxCfgPFEHEnable, gTransData->Pfeh);
     if (gTransData->McaErrThreshCount == 0){
      PcdSetBool(PcdMcaErrThreshEn, FALSE);
      PcdSet16(PcdMcaErrThreshCount, 0);
     } else {
      PcdSet16(PcdMcaErrThreshCount, SetupData->McaErrThreshCount);
     }
    
  }else{
    /*if(SetupData->SriovDis){
      PcdSet8(PcdAmdNbIOMMU,!SetupData->SriovDis);
    }*/
    PcdSetBool(PcdCfgPcieAriSupport, !SetupData->SriovDis);
    PcdSetBool(PcdAmdCcxCfgPFEHEnable, SetupData->Pfeh);
	if (SetupData->McaErrThreshCount == 0){
      PcdSetBool(PcdMcaErrThreshEn, FALSE);
    }
    PcdSet16(PcdMcaErrThreshCount, SetupData->McaErrThreshCount);
  }

  PcdSet8(PcdTpmSelect, SetupData->TpmSelect);

    if (SetupData->TpmSelect != 1) {
    PcdSetBool (PcdHygonFtpmEnable, FALSE);	
  }
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), TpmSelect :%d.\n", SetupData->TpmSelect));  
  if (SetupData->TpmSelect == 0) {
    // decode SPI TCM.
    //LPC[7C]
    //  [0]  = 1    // Tpm12En
    //  [12] = 1    // TpmPfetchEn. (TPM burst read)
    //  [13] = 1    // TpmBufferEn
    //
    //LPC[A0]
    //  [0] = 1     // AltSpiCSEnable
    //  [3] = 1     // RouteTpm2Spi(TPM cycles are routed to SPI bus with TPM_SPI_CS# asserted.)
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), PcdTpmDecode :%d.\n", PcdGet8(PcdTpmDecode)));  
    switch(PcdGet8(PcdTpmDecode)){
      case TPM_DECODE_NONE:
      default:
        break;
    
      case TPM_DECODE_SPI:
        MmioOr32(LPC_PCI_REG(0x7C), BIT0 | BIT12 | BIT13);
        MmioOr32(LPC_PCI_REG(0xA0), BIT0 | BIT3);
        break;
    
      case TPM_DECODE_LPC:
        MmioOr32(LPC_PCI_REG(0x7C), BIT0 | BIT12 | BIT13);
        break;
    }
  } else {

    DEBUG((EFI_D_INFO, "Disable Tpm Decode.\n"));  
    MmioAnd32(LPC_PCI_REG(0x7C), ~BIT0);
  }
  

  PlatformUsbSetting(SetupData, Info);

  Status = PeiServicesInstallPpi(&gSetupDataReadyPpiList);
  ASSERT_EFI_ERROR(Status);

  #if BC_TENCENT
  PcdSet8S (PcdIpmiBootMode, SetupData->BootModeType);
  #endif 
  

  return Status;
}






// D18F0x0[A0...BC] [Configuration Address Maps] (DF::CfgAddressMap)
//   31:24 BusNumLimit.
//   23:16 BusNumBase
STATIC
VOID
GetRootBridgeBusRange (
  PLATFORM_COMM_INFO  *Info,
  UINTN               Sockets,
  UINTN               dies
  ) 
{
  UINT32               Index;
  UINT32               Data32;
  
  for(Index=0;Index<Info->PciHostCount;Index++){
    if(Index >=dies)
    {
      Data32 = MmioRead32(DF0_PCI_REG(0xA0)+(Index - dies + 4)*4);
    }else{
      Data32 = MmioRead32(DF0_PCI_REG(0xA0)+Index*4);
    }
    Info->BusBase[Index]  = (UINT8)((Data32 >> 16) & 0xFF);
    Info->BusLimit[Index] = (UINT8)((Data32 >> 24) & 0xFF);
    if(Info->BusLimit[Index] == 0xFF){
      Info->BusLimit[Index] = 0x7F;
    }
    DEBUG((EFI_D_INFO, "RB[%d]Bus(%X,%X)\n", Index, Info->BusBase[Index], Info->BusLimit[Index]));
  }
}





// D18F1x200 [System Configuration]
//   [5] OtherSocketPresent. The other socket is present and enabled in the system.
void BuildDefaultPlatformInfoHob(PLATFORM_COMM_INFO **pInfo)
{
  PLATFORM_COMM_INFO  *Info;
  UINTN               Sockets;
  UINTN               dies = 0;
  UINT32              Data32;


  Info = (PLATFORM_COMM_INFO*)BuildGuidHob(&gEfiPlatformCommInfoGuid, sizeof(PLATFORM_COMM_INFO));
  ASSERT(Info != NULL);
  ZeroMem(Info, sizeof(PLATFORM_COMM_INFO));
  Info->Signature = PLAT_COMM_INFO_SIGN;

  Data32 = MmioRead32(DF0F1_PCI_REG(0x200));
  Sockets = !!(Data32 & BIT5) + 1;
  dies = !!(Data32 & BIT0) + !!(Data32 & BIT1) + !!(Data32 & BIT2) + !!(Data32 & BIT3);
  Info->PciHostCount = (UINT32)(Sockets * dies);
  Info->Dies         = (UINT32)dies;
  Info->Sockets      = (UINT32)Sockets;
  DEBUG((EFI_D_INFO, "PciHostCount:%d %X %d %d\n", Info->PciHostCount, Data32, Sockets, dies));

  if(Info->PciHostCount == 8){

    Info->FabricId[0] = 0x04;
    Info->FabricId[1] = 0x24;
    Info->FabricId[2] = 0x44;
    Info->FabricId[3] = 0x64;
    Info->FabricId[4] = 0x84;
    Info->FabricId[5] = 0xA4;
    Info->FabricId[6] = 0xC4;
    Info->FabricId[7] = 0xE4; 

  } else if(Info->PciHostCount == 4){

    if(dies == 4){
      Info->FabricId[0] = 0x04;
      Info->FabricId[1] = 0x24;
      Info->FabricId[2] = 0x44;
      Info->FabricId[3] = 0x64;
    } else if(dies == 2){
      Info->FabricId[0] = 0x04;
      Info->FabricId[1] = 0x24;
      Info->FabricId[2] = 0x84;
      Info->FabricId[3] = 0xA4;
    }
    
  } else if(Info->PciHostCount == 2) {
    if(dies == 2){ 
      Info->FabricId[0] = 0x04;
      Info->FabricId[1] = 0x24;
    }else if(dies ==1){
      Info->FabricId[0] = 0x04;
      Info->FabricId[1] = 0x84; 
    }
  } else if(Info->PciHostCount == 1){
    Info->FabricId[0] = 0x04; 
  }

  Info->PlatPciIoBase  = PLAT_PCI_IO_BASE;
  Info->PlatPciIoLimit = PLAT_PCI_IO_LIMIT;
  Info->PlatPciIoCur   = Info->PlatPciIoBase;

  GetRootBridgeBusRange(Info, Sockets, dies);
//SetRootBridgeIoRange(Info, Sockets, dies);
//SetRootBridgeMmio64Range(Info,Sockets, dies);   // agesa FabricResourceInit() will set it.

  *pInfo = Info;
}

VOID SendRestart(){
  UINT8                     ResetCommand[16]={0x0,0x0,0x2,0x0,0x0,0x0,0x0,0x21,0x0,0x4,0x1d,0xe8,0x6f,0x7,0xff,0xff};
  EFI_STATUS                Status;
  PEI_IPMI_TRANSPORT_PPI		*PeiImpiPpi = NULL;
  UINTN                     Size;
  
  Status = PeiServicesLocatePpi(
			  &gPeiIpmiTransportPpiGuid,
			  0,
			  NULL,
			  (VOID **) &PeiImpiPpi
			  );
  if(PeiImpiPpi != NULL) {
    Size = sizeof(ResetCommand);
	  Status = PeiImpiPpi->SendIpmiCommand(
					   PeiImpiPpi,
					   0xa,
					   0,
					   0x44,
					   (UINT8*)&ResetCommand,
					   Size,
					   NULL,
					   0
					   );	
    DEBUG((EFI_D_INFO,"SendRestart command is :%r\n",Status));	
  }
}

VOID SendBoot(){
  UINT8                     ResetCommand[16]={0x0,0x0,0x2,0x0,0x0,0x0,0x0,0x21,0x0,0x4,0x1f,0xe9,0x6f,0x6,0x0,0x0};
  EFI_STATUS                Status;
  PEI_IPMI_TRANSPORT_PPI		*PeiImpiPpi = NULL;
  UINTN                     Size;
  
  Status = PeiServicesLocatePpi(
			  &gPeiIpmiTransportPpiGuid,
			  0,
			  NULL,
			  (VOID **) &PeiImpiPpi
			  );
  if(PeiImpiPpi != NULL) {
    Size = sizeof(ResetCommand);
	  Status = PeiImpiPpi->SendIpmiCommand(
					   PeiImpiPpi,
					   0xa,
					   0,
					   0x44,
					   (UINT8*)&ResetCommand,
					   Size,
					   NULL,
					   0
					   );	
    DEBUG((EFI_D_INFO,"SendRestart command is :%r\n",Status));	
  }
}


EFI_STATUS
EFIAPI
PlatformPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                       Status;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Var2Ppi;
  EFI_BOOT_MODE                    BootMode;
  PLATFORM_COMM_INFO               *Info;
  UINT32                           ResetStatus;
  
  ResetStatus = MmioRead32(0xFED803C0UL);
  DEBUG((EFI_D_INFO,"ResetStatus is 0x%x\n ",ResetStatus));
  if(ResetStatus&BIT19){//do reset
    SendRestart();
  }else{
    SendBoot();
  }
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID**)&Var2Ppi
             );
  ASSERT_EFI_ERROR(Status);

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR(Status);  

  BuildDefaultPlatformInfoHob(&Info);
  SetupDataInit(Var2Ppi, BootMode, Info);
 
  Status = PeiServicesNotifyPpi(&gPpiNotifyList[0]);
  ASSERT_EFI_ERROR(Status);



  return Status;
}





