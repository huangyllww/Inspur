
#include "BdsPlatform.h"
#include <Guid/SetupPassword.h>
#include <Protocol/ExitPmAuth.h>
#include <Protocol/DxeSmmReadyToLock.h>
#include <Protocol/AcpiS3Save.h>
#include <Protocol/FirmwareVolume2.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/UpdateFlash.h>
#include <Library/ReportStatusCodeLib.h>
#include <Protocol/EsrtManagement.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <library/ByoCommLib.h>
#include <library/PerformanceLib.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <Library/ByoUefiBootManagerLib.h>
#include <Library/LogoLib.h>
#include <ByoStatusCode.h>
#include <SysMiscCfg.h>
#include <Protocol/BmcWdtProtocol.h>
#include <BmcConfig.h>
#include <Library/TcgPhysicalPresenceLib.h> 
#include <Library/Tcg2PhysicalPresenceLib.h>

extern EFI_BDS_BOOT_MANAGER_PROTOCOL gBdsBootManagerProtocol;

VOID
InvokeGetBbsInfo (
  VOID
  );

VOID
InstallAdditionalOpRom (
  VOID
  );

EFI_STATUS ByoEfiBootManagerBootApp(IN EFI_GUID *AppGuid);

EFI_STATUS 
ByoEfiBootManagerBootAppParam (
  IN  EFI_GUID            *AppGuid,
  IN  VOID                *Parameter,
  IN  UINTN               ParameterSize,
  IN  BOOLEAN             IsBootCategory  
  );

EFI_STATUS 
ByoEfiBootManagerBootFileParam (
  IN  EFI_DEVICE_PATH_PROTOCOL *FileDp,
  IN  VOID                     *Parameter,
  IN  UINTN                    ParameterSize,
  IN  BOOLEAN                  IsBootCategory
  );

/**
  This function converts an input device structure to a Unicode string.

  @param DevPath                  A pointer to the device path structure.

  @return A new allocated Unicode string that represents the device path.

**/
CHAR16 *
DevicePathToStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  EFI_STATUS                       Status;
  CHAR16                           *ToText;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;

  if (DevPath == NULL) {
    return NULL;
  }

  Status = gBS->LocateProtocol (
                  &gEfiDevicePathToTextProtocolGuid,
                  NULL,
                  (VOID **) &DevPathToText
                  );
  ASSERT_EFI_ERROR (Status);
  ToText = DevPathToText->ConvertDevicePathToText (
                            DevPath,
                            FALSE,
                            TRUE
                            );
  ASSERT (ToText != NULL);
  return ToText;
}



VOID
UpdateEfiGlobalVariable (
  CHAR16           *VariableName,
  EFI_GUID         *AgentGuid,
  PROCESS_VARIABLE ProcessVariable
  )
/*++

Routine Description:

  Generic function to update the console variable.
  Please refer to FastBootSupport.c for how to use it.

Arguments:

  VariableName    - The name of the variable to be updated
  AgentGuid       - The Agent GUID
  ProcessVariable - The function pointer to update the variable
                    NULL means to restore to the original value

--*/
{
  EFI_STATUS  Status;
  CHAR16      BackupVariableName[20];
  CHAR16      FlagVariableName[20];
  VOID        *Variable;
  VOID        *BackupVariable;
  VOID        *NewVariable;
  UINTN       VariableSize;
  UINTN       BackupVariableSize;
  UINTN       NewVariableSize;
  BOOLEAN     Flag;
  BOOLEAN     *FlagVariable;
  UINTN       FlagSize;
  CHAR16      *Str;

  ASSERT (StrLen (VariableName) <= 13);
  UnicodeSPrint (BackupVariableName, sizeof (BackupVariableName), L"%sBackup", VariableName);
  UnicodeSPrint (FlagVariableName, sizeof (FlagVariableName), L"%sModify", VariableName);

  Variable       = EfiBootManagerGetVariableAndSize (VariableName, &gEfiGlobalVariableGuid, &VariableSize);
  BackupVariable = EfiBootManagerGetVariableAndSize (BackupVariableName, AgentGuid, &BackupVariableSize);
  FlagVariable   = EfiBootManagerGetVariableAndSize (FlagVariableName, AgentGuid, &FlagSize);
  if (ProcessVariable != NULL) {
    if (FlagVariable == NULL) {
      //
      // Last boot is normal boot
      // Set flag
      // BackupVariable <- Variable
      // Variable       <- ProcessVariable (Variable)
      //
      Flag   = TRUE;
      Status = gRT->SetVariable (
                      FlagVariableName,
                      AgentGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      sizeof (Flag),
                      &Flag
                      );
      ASSERT_EFI_ERROR (Status);

      Status = gRT->SetVariable (
                      BackupVariableName,
                      AgentGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      VariableSize,
                      Variable
                      );
      ASSERT ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND));


      NewVariable     = Variable;
      NewVariableSize = VariableSize;
      ProcessVariable (&NewVariable, &NewVariableSize);
      DEBUG ((EFI_D_ERROR, "============================%s============================\n", VariableName));
      Str = DevicePathToStr ((EFI_DEVICE_PATH_PROTOCOL *) Variable);    DEBUG ((EFI_D_ERROR, "O:%s\n", Str)); gBS->FreePool (Str);
      Str = DevicePathToStr ((EFI_DEVICE_PATH_PROTOCOL *) NewVariable); DEBUG ((EFI_D_ERROR, "N:%s\n", Str)); gBS->FreePool (Str);
      
      Status = gRT->SetVariable (
                      VariableName,
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      NewVariableSize,
                      NewVariable
                      );
      ASSERT ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND));

      if (NewVariable != NULL) {
        FreePool (NewVariable);
      }
    } else { // LastBootIsModifiedPtr != NULL
      //
      // Last Boot is modified boot
      //
    }
  } else {
    if (FlagVariable != NULL) {
      //
      // Last boot is modified boot
      // Clear LastBootIsModified flag
      // Variable       <- BackupVariable
      // BackupVariable <- NULL
      //
      Status = gRT->SetVariable (
                      FlagVariableName,
                      AgentGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );
      ASSERT_EFI_ERROR (Status);

      Status = gRT->SetVariable (
                      VariableName,
                      &gEfiGlobalVariableGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      BackupVariableSize,
                      BackupVariable
                      );
      ASSERT ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND));

      Status = gRT->SetVariable (
                      BackupVariableName,
                      AgentGuid,
                      EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                      0,
                      NULL
                      );
      ASSERT ((Status == EFI_SUCCESS) || (Status == EFI_NOT_FOUND));
    } else { // LastBootIsModifiedPtr == NULL
      //
      // Last boot is normal boot
      //
    }
  }

  if (Variable != NULL) {
    FreePool (Variable);
  }

  if (BackupVariable != NULL) {
    FreePool (BackupVariable);
  }

  if (FlagVariable != NULL) {
    FreePool (FlagVariable);
  }

}



#define BMC_SETUP_VARIABLE_NAME    L"BmcSetup"


VOID
EnableWdg(){
  EFI_STATUS			  Status;
  BMC_SETUP_DATA		  SetupData = {0};
  UINTN				      VarSize;
  EFI_BMC_WDT_PROTOCOL    *WdtProt;
  EFI_GUID			      SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_BMC_WDT_CFG         mWdtCfg[4];

 // EfiInitializeIpmiBase ();
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
				BMC_SETUP_VARIABLE_NAME,
				&SystemConfigurationGuid,
				NULL,
				&VarSize,
				&SetupData
				);
  if (Status == EFI_NOT_FOUND) {
    SetupData.WdtEnable[1]  = 1;
    SetupData.WdtPolicy[1]  = 1;
    SetupData.WdtTimeout[1] = 150;
    SetupData.WdtEnable[3]  = 0;
    SetupData.WdtPolicy[3]  = 1;
    SetupData.WdtTimeout[3] = 300;
    Status = gRT->SetVariable (
				  BMC_SETUP_VARIABLE_NAME,
				  &SystemConfigurationGuid,
				  BMC_SETUP_VARIABLE_FLAG,
				  VarSize,
				  &SetupData
				  );
  }



  Status = gBS->LocateProtocol (
			 &gEfiBmcWdtProtocolGuid,
			 NULL,
			 &WdtProt
			 );
  if (!EFI_ERROR (Status)) {
    mWdtCfg[0].Enable   = FALSE;
    mWdtCfg[0].Action   = 0;
    mWdtCfg[0].Timeout  = 0;
    mWdtCfg[1].Enable   = SetupData.WdtEnable[1];
    mWdtCfg[1].Action   = SetupData.WdtPolicy[1];
    mWdtCfg[1].Timeout  = SetupData.WdtTimeout[1];
    mWdtCfg[2].Enable   = FALSE;
    mWdtCfg[2].Action   = 0;
    mWdtCfg[2].Timeout  = 0;
    mWdtCfg[3].Enable   = SetupData.WdtEnable[3];
    mWdtCfg[3].Action   = SetupData.WdtPolicy[3];
    mWdtCfg[3].Timeout  = SetupData.WdtTimeout[3];
    WdtProt->SetWatchdog (WdtProt, mWdtCfg);
  }
}

VOID
SignalAllDriversConnected (
  VOID
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  SignalProtocolEvent(gBS, &gBdsAllDriversConnectedProtocolGuid, FALSE);
  if(PcdGet8(PcdStopWdg)){
	EnableWdg();
  }
}



VOID
ExitPmAuth (
  VOID
  )
{
  EFI_STATUS                 Status;
  EFI_ACPI_S3_SAVE_PROTOCOL  *AcpiS3Save;

  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));  
  //
  // Prepare S3 information, this MUST be done before ExitPmAuth
  //
  Status = gBS->LocateProtocol (&gEfiAcpiS3SaveProtocolGuid, NULL, (VOID **)&AcpiS3Save);
  if (!EFI_ERROR (Status)) {
    AcpiS3Save->S3Save (AcpiS3Save, NULL);
  }

  //
  // Inform the SMM infrastructure that we're entering BDS and may run 3rd party code hereafter 
  // NOTE: We can NOT put it to PlatformBdsInit, because many boot script touch PCI BAR. :-(
  //       We have to connect PCI root bridge, allocate resource, then ExitPmAuth().
  //
  SignalProtocolEvent(gBS, &gExitPmAuthProtocolGuid, FALSE);

  EfiEventGroupSignal(&gEfiEndOfDxeEventGroupGuid);
  
  //
  // NOTE: We need install DxeSmmReadyToLock directly here because many boot script is added via ExitPmAuth callback.
  // If we install them at same callback, these boot script will be rejected because BootScript Driver runs first to lock them done.
  // So we seperate them to be 2 different events, ExitPmAuth is last chance to let platform add boot script. DxeSmmReadyToLock will
  // make boot script save driver lock down the interface.
  //
  SignalProtocolEvent(gBS, &gEfiDxeSmmReadyToLockProtocolGuid, FALSE);

}








VOID
ConnectRootBridge (
  VOID
  )
{
  UINTN                            RootBridgeHandleCount;
  EFI_HANDLE                       *RootBridgeHandleBuffer = NULL;
  UINTN                            RootBridgeIndex;


  InvokeHookProtocol(gBS, &gEfiBeforeConnectPciRootBridgeGuid);

  RootBridgeHandleCount = 0;
  gBS->LocateHandleBuffer (
         ByProtocol,
         &gEfiPciRootBridgeIoProtocolGuid,
         NULL,
         &RootBridgeHandleCount,
         &RootBridgeHandleBuffer
         );
  for (RootBridgeIndex = 0; RootBridgeIndex < RootBridgeHandleCount; RootBridgeIndex++) {
    gBS->ConnectController(RootBridgeHandleBuffer[RootBridgeIndex], NULL, NULL, FALSE);
  }

  InvokeHookProtocol(gBS, &gEfiAfterAllPciIoGuid); 

  if(RootBridgeHandleBuffer!=NULL){FreePool(RootBridgeHandleBuffer);}	

  InvokeHookProtocol(gBS, &gEfiAfterConnectPciRootBridgeGuid); 

  DEBUG((EFI_D_INFO, "RootBrigdeConnected\n"));
}


BOOLEAN
IsGopDevicePath (
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  while (!IsDevicePathEndType (DevicePath)) {
    if (DevicePathType (DevicePath) == ACPI_DEVICE_PATH &&
        DevicePathSubType (DevicePath) == ACPI_ADR_DP) {
      return TRUE;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }
  return FALSE;
}


//
// BDS Platform Functions
//

/**
  Connect the USB short form device path.

  @param DevicePath   USB short form device path

  @retval EFI_SUCCESS           Successfully connected the USB device
  @retval EFI_NOT_FOUND         Cannot connect the USB device
  @retval EFI_INVALID_PARAMETER The device path is invalid.
**/
EFI_STATUS
ConnectUsbShortFormDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            *Handles;
  UINTN                                 HandleCount;
  UINTN                                 Index;
  EFI_PCI_IO_PROTOCOL                   *PciIo;
  UINT8                                 Class[3];
  BOOLEAN                               AtLeastOneConnected;

  //
  // Check the passed in parameters
  //
  if (DevicePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((DevicePathType (DevicePath) != MESSAGING_DEVICE_PATH) ||
      ((DevicePathSubType (DevicePath) != MSG_USB_CLASS_DP) && (DevicePathSubType (DevicePath) != MSG_USB_WWID_DP))
     ) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Find the usb host controller firstly, then connect with the remaining device path
  //
  AtLeastOneConnected = FALSE;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  for (Index = 0; Index < HandleCount; Index++) {
    Status = gBS->HandleProtocol (
                    Handles[Index],
                    &gEfiPciIoProtocolGuid,
                    (VOID **) &PciIo
                    );
    if (!EFI_ERROR (Status)) {
      //
      // Check whether the Pci device is the wanted usb host controller
      //
      Status = PciIo->Pci.Read (PciIo, EfiPciIoWidthUint8, 0x09, 3, &Class);
      if (!EFI_ERROR (Status) &&
          ((PCI_CLASS_SERIAL == Class[2]) && (PCI_CLASS_SERIAL_USB == Class[1]))
         ) {
        Status = gBS->ConnectController (
                        Handles[Index],
                        NULL,
                        DevicePath,
                        FALSE
                        );
        if (!EFI_ERROR(Status)) {
          AtLeastOneConnected = TRUE;
        }
      }
    }
  }

  return AtLeastOneConnected ? EFI_SUCCESS : EFI_NOT_FOUND;
}



VOID
UpdateCsm16UCR (
  IN UINT16   UartIoBase
  )
{
  UINTN                      Ebda;

  Ebda = (*(UINT16*)(UINTN)0x40E) << 4;

  *((volatile UINT8*) (Ebda + 0x1C2))  = 1;                  //flag for CSM support
  *((volatile UINT16*)(Ebda + 0x1C3)) = UartIoBase;
}



EFI_STATUS
UpdateSerialConsoleVariable(
  PLAT_HOST_INFO_PROTOCOL *HostInfo
  ) 
{
  EFI_DEVICE_PATH_PROTOCOL  *VarConsole = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *DevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *SerialDevicePath = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *Instance = NULL;
  EFI_DEVICE_PATH_PROTOCOL  *Next;
  UINTN                     Size = 0;
  BOOLEAN                   Found = FALSE;
  UINT16                    UartIoBase = 0;
  BOOLEAN                   UcrEnable = TRUE;
  UINTN                     DpSize = 0;
  

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if(HostInfo->GetPlatUcrDp == NULL){
    UcrEnable = FALSE;
  }

  if(UcrEnable){
    SerialDevicePath = HostInfo->GetPlatUcrDp(&DpSize, &UartIoBase);
    if(SerialDevicePath == NULL){
      UcrEnable = FALSE;
    } else {
      UcrEnable = TRUE;
    }
  }
  
  VarConsole = EfiBootManagerGetVariableAndSize (
                 L"ConOut",
                 &gEfiGlobalVariableGuid,
                 &Size
                 );
  DevicePath = VarConsole;
  
  do {
    if(Instance != NULL){
      FreePool(Instance);
      Instance = NULL;
    }
    Instance = GetNextDevicePathInstance (&DevicePath, &Size);
    if(Instance == NULL){
      break;
    }

    Next = Instance;
    while (!IsDevicePathEndType(Next)) {
      //
      // Early break when it's a serial device path
      // Note: Check EISA_PNP_ID (0x501) instead of UART node because we only want to disable the local serial but not the SOL
      //       SOL device path doesn't contain the EISA_PNP_ID (0x501)
      //
      if ((Next->Type == ACPI_DEVICE_PATH) && (Next->SubType == ACPI_DP) &&
          (((ACPI_HID_DEVICE_PATH *) Next)->HID == EISA_PNP_ID (0x501))
          ) {
        Found = TRUE;
        break;
      }

      if(Next->Type == MESSAGING_DEVICE_PATH && Next->SubType == MSG_UART_DP){
        Found = TRUE;
        break;
      }
      
      Next = NextDevicePathNode(Next);
    }
  } while (DevicePath != NULL && Found == FALSE);

  if (Found) {
    if (!UcrEnable) {
      EfiBootManagerUpdateConsoleVariable (ConIn, NULL, Instance);
      EfiBootManagerUpdateConsoleVariable (ConOut, NULL, Instance);
    }
  }

  if (UcrEnable) {
    UpdateCsm16UCR(UartIoBase);
    if (Found && CompareMem(SerialDevicePath, Instance, DpSize) != 0) {
      EfiBootManagerUpdateConsoleVariable (ConIn, NULL, Instance);
      EfiBootManagerUpdateConsoleVariable (ConOut, NULL, Instance);
    }
    EfiBootManagerUpdateConsoleVariable (ConIn, SerialDevicePath, NULL);
    EfiBootManagerUpdateConsoleVariable (ConOut, SerialDevicePath, NULL);
  }

  if(VarConsole != NULL) {
    FreePool(VarConsole);
  }
  if(Instance != NULL) {
    FreePool(Instance);
  }

  return EFI_SUCCESS;
}



VOID HandleTcgPhysicalPresence()
{
  EFI_GUID  *TpmInstanceGuid;
  BOOLEAN   NeedConfirm = FALSE;
  BOOLEAN   IsTpm12 = FALSE;
  BOOLEAN   IsTpm2  = FALSE;
  UINT8     TpmClear = 0;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  TpmInstanceGuid = (EFI_GUID *)PcdGetPtr(PcdTpmInstanceGuid);
  if (CompareGuid(TpmInstanceGuid, &gEfiTpmDeviceInstanceTpm20DtpmGuid) || 
      CompareGuid(TpmInstanceGuid, &gEfiTpmDeviceInstanceTpm20HgfTpmGuid)){
    DEBUG((EFI_D_INFO, "TPM2.0\n"));
    IsTpm2 = TRUE;
    NeedConfirm = Tcg2PhysicalPresenceLibNeedUserConfirm();
  } else if (CompareGuid(TpmInstanceGuid, &gEfiTpmDeviceInstanceTpm12Guid)){
    DEBUG((EFI_D_INFO, "TPM1.2\n"));
    IsTpm12 = TRUE;
    NeedConfirm = TcgPhysicalPresenceLibNeedUserConfirm();
  }

  if(NeedConfirm){
    ByoEfiBootManagerConnectAllDefaultConsoles();
  }

  if(IsTpm2){
    Tcg2PhysicalPresenceLibProcessRequest(NULL, &TpmClear); 
  }
  if(IsTpm12){
    TcgPhysicalPresenceLibProcessRequest();
  }
}




VOID
EFIAPI
PlatformBootManagerBeforeConsole (
  VOID
  )
/*++

Routine Description:

  Platform Bds init. Incude the platform firmware vendor, revision
  and so crc check.

Arguments:

Returns:

  None.

--*/
{
  EFI_STATUS                          Status = EFI_SUCCESS;
  UINTN                               Index;
  PLAT_HOST_INFO_PROTOCOL             *HostInfo;


  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE,
    (EFI_SOFTWARE_DXE_BS_DRIVER | EFI_SW_DXE_BS_PC_BEGIN_CONNECTING_DRIVERS)
    );

  ConnectRootBridge();

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &HostInfo);
  ASSERT(!EFI_ERROR(Status));

  DEBUG((EFI_D_INFO, "ConInDpCount:%d\n", HostInfo->ConInDpCount));
  for (Index = 0; Index < HostInfo->ConInDpCount; Index++) {
    Status = EfiBootManagerUpdateConsoleVariable (ConIn, HostInfo->ConInDp[Index], NULL);
    DEBUG((EFI_D_INFO, "UpdateVar_ConIn:%r\n", Status));
  }
  
  UpdateSerialConsoleVariable(HostInfo);

  gRT->SetVariable (
         EFI_CAPSULE_VARIABLE_NAME,
         &gEfiCapsuleVendorGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
         0,
         NULL
         );
  HandleTcgPhysicalPresence(); 
  InvokeHookProtocol(gBS, &gPlatBeforeExitPmAuthProtocolGuid);

  PERF_START(NULL, "ExitPmAuth", "BDS", 0); 
  ExitPmAuth();
  PERF_END(NULL, "ExitPmAuth", "BDS", 0);  

}

VOID
ConnectSequence (
  VOID
  )
{
  UINTN                      Index;
  EFI_STATUS                 Status;
  PLATFORM_HOST_INFO         *HostInfo;
  EFI_DEVICE_PATH_PROTOCOL   *Dp;
  PLAT_HOST_INFO_PROTOCOL    *ptHostInfo;
  EFI_PCI_IO_PROTOCOL        *PciIo;
  UINT32                     PciId;
  UINTN                      HandleCount;
  EFI_HANDLE                 *Handles;  
  EFI_DEVICE_PATH_PROTOCOL   **DpBuffer = NULL;
  UINTN                      *DpSizeBuffer = NULL;
  UINTN                      i;
  UINTN                      DpSize;
  UINTN                      CmpSize;
  BOOLEAN                    IsSlot;
  
  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateProtocol(&gPlatHostInfoProtocolGuid, NULL, &ptHostInfo);
  if(EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "[ERROR] PlatHostInfo not found\n"));
    return;
  }

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiPciIoProtocolGuid,
                  NULL,
                  &HandleCount,
                  &Handles
                  );
  if(EFI_ERROR(Status) || HandleCount == 0){
    goto ProcExit;
  }
  
  DpBuffer     = AllocatePool(HandleCount * sizeof(EFI_DEVICE_PATH_PROTOCOL*));
  DpSizeBuffer = AllocatePool(HandleCount * sizeof(UINTN));
  ASSERT(DpBuffer != NULL);
  ASSERT(DpSizeBuffer != NULL);
  for(Index=0;Index<HandleCount;Index++){
    Status = gBS->HandleProtocol(Handles[Index], &gEfiDevicePathProtocolGuid, &DpBuffer[Index]);
    if(EFI_ERROR(Status)){
      DpBuffer[Index] = NULL;
      DpSizeBuffer[Index] = 0;
    } else {
      DpSizeBuffer[Index] = GetDevicePathSize(DpBuffer[Index]);
    }
  }

  HostInfo = ptHostInfo->HostList;
  for (Index = 0; Index < ptHostInfo->HostCount; Index++) {

// GFX has been connected at ByoEfiBootManagerConnectAllDefaultConsoles()
// so here igonre it.
    if(HostInfo[Index].HostType == PLATFORM_HOST_IGD){
      continue;
    }
    
    Dp = HostInfo[Index].Dp;
    DpSize = GetDevicePathSize(Dp);

    IsSlot = HostInfo[Index].HostType == PLATFORM_HOST_PCIE || HostInfo[Index].HostType == PLATFORM_HOST_NVME;
    if(IsSlot){
      CmpSize = DpSize - sizeof(PCI_DEVICE_PATH) - 4;
    } else {
      CmpSize = DpSize;      
    }

    DEBUG((EFI_D_INFO, "CS[%d]\n", Index));

    for(i=0;i<HandleCount;i++){
      if(DpSizeBuffer[i] == 0){
        continue;
      }
      if(IsSlot){
        if(DpSize > DpSizeBuffer[i]){
          continue;
        }
      } else {
        if(DpSize != DpSizeBuffer[i]){
          continue;
        }
      }

      if(CompareMem(Dp, DpBuffer[i], CmpSize) == 0){
        Status = gBS->HandleProtocol(Handles[i], &gEfiPciIoProtocolGuid, &PciIo);
        PciIo->Pci.Read(PciIo, EfiPciIoWidthUint32, 0, 1, &PciId);
        Status = gBS->ConnectController(Handles[i], NULL, NULL, TRUE);
        DEBUG((EFI_D_INFO, "CS[%d]:%r(%08X)\n", Index, Status, PciId));         
      }
    }
    
  }

ProcExit:
  InvokeHookProtocol(gBS, &gPlatAfterConnectSequenceProtocolGuid);
  if(DpBuffer!=NULL){FreePool(DpBuffer);}
  if(DpSizeBuffer!=NULL){FreePool(DpSizeBuffer);}  
}

extern EFI_GUID  gSignalBeforeEnterSetupGuid;


VOID
EFIAPI
PlatformBootManagerAfterConsole (
  VOID
  )
/*++

Routine Description:

  The function will excute with as the platform policy, current policy
  is driven by boot mode. IBV/OEM can customize this code for their specific
  policy action.
  
Arguments:
  DriverOptionList - The header of the driver option link list
  BootOptionList   - The header of the boot option link list
  ProcessCapsules  - A pointer to ProcessCapsules()
  BaseMemoryTest   - A pointer to BaseMemoryTest()
 
Returns:
  None.
  
--*/
{
  EFI_BOOT_MODE                 BootMode;
  ESRT_MANAGEMENT_PROTOCOL      *EsrtManagement;
  EFI_STATUS                    Status;
  BYO_SMIFLASH_PROTOCOL         *ByoSmiFlash;


  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  gBdsBootManagerProtocol.BootApp        = ByoEfiBootManagerBootApp;
  gBdsBootManagerProtocol.Boot           = ByoEfiBootManagerBoot;
  gBdsBootManagerProtocol.BootAppParam   = ByoEfiBootManagerBootAppParam;
  gBdsBootManagerProtocol.BootFileParam  = ByoEfiBootManagerBootFileParam;
  
  InvokeHookProtocol(gBS, &gPlatAfterConsoleStartProtocolGuid);

  Status = gBS->LocateProtocol(&gEsrtManagementProtocolGuid, NULL, (VOID **)&EsrtManagement);
  if (EFI_ERROR(Status)) {
    EsrtManagement = NULL;
  }

  BootMode = GetBootModeHob();
  
  switch (BootMode) {

    case BOOT_ASSUMING_NO_CONFIGURATION_CHANGES:
    case BOOT_WITH_MINIMAL_CONFIGURATION:
    case BOOT_ON_S4_RESUME:
      PERF_START(NULL, "ConnectSequence", "BDS", 0);      
      ConnectSequence();
      PERF_END(NULL, "ConnectSequence", "BDS", 0);

      PERF_START(NULL, "LegacyOpRom", "BDS", 0);      
      InstallAdditionalOpRom();
      PERF_END(NULL, "LegacyOpRom", "BDS", 0);

      PERF_START(NULL, "AllConnectEvent", "BDS", 0); 
      SignalAllDriversConnected();
      PERF_END(NULL, "AllConnectEvent", "BDS", 0);
      
      InvokeGetBbsInfo();
      ByoEfiBootManagerRefreshAllBootOption();

      if (EsrtManagement != NULL) {
        EsrtManagement->LockEsrtRepository();
      }
      
      break;


//    case BOOT_ON_FLASH_UPDATE:
//      if (EsrtManagement != NULL) {
//        EsrtManagement->SyncEsrtFmp();
//      }
//      InvokeHookProtocol(gBS, &gPlatBeforeBiosUpdateProtocolGuid);
//      ProccessFlashUpdate(BOOT_ON_FLASH_UPDATE);
//      break;

    case BOOT_IN_RECOVERY_MODE:
      ConnectSequence();
      InvokeHookProtocol(gBS, &gPlatBdsRecoveryStartProtocolGuid);
      REPORT_STATUS_CODE (EFI_ERROR_CODE, BSC_BIOS_RECOVERY_BDS_START); 
      Status = gBS->LocateProtocol (
                      &gByoSmiFlashProtocolGuid,
                      NULL,
                      (VOID**)&ByoSmiFlash
                      );
      if(!EFI_ERROR(Status)){
        ByoSmiFlash->Recovery(ByoSmiFlash);
      }
      CpuDeadLoop();
      break;

    case BOOT_WITH_FULL_CONFIGURATION:
    case BOOT_WITH_FULL_CONFIGURATION_PLUS_DIAGNOSTICS:
    case BOOT_WITH_DEFAULT_SETTINGS:
    default:
      ConnectSequence();
      ByoEfiBootManagerConnectAll();
      InstallAdditionalOpRom();
			
			if (PcdGet8 (PcdLegacyBiosSupport) == 1) {
        if(!(PcdGet32(PcdSystemMiscConfig) & SYS_MISC_CFG_DIS_SHOW_LOGO)){
          ShowPostLogo();
          InvokeHookProtocol(gBS, &gByoAfterShowPostLogoProtocolGuid);
        }
      }

      SignalAllDriversConnected();
      ByoEfiBootManagerRefreshAllBootOption();
      if (EsrtManagement != NULL) {
        EsrtManagement->SyncEsrtFmp();
      }
      break;
  }

  gBdsBootManagerProtocol.ConnectAll     = ByoEfiBootManagerConnectAll;
  gBdsBootManagerProtocol.RefreshOptions = ByoEfiBootManagerRefreshAllBootOption;

  InvokeHookProtocol(gBS, &gPlatAfterConsoleEndProtocolGuid);

}


