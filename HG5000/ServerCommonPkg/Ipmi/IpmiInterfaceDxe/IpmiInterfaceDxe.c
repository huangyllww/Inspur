/** @file
 Byosoft IPMI Interface DXE module entry.
*/


#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <IpmiInterfaceCommon.h>
#include <SetupVariable.h>

#include <AmdPcieComplex.h>
#include <AmdCpmDxe.h>
#include <AmdCpmDefine.h>

#include <Library/HobLib.h>

#define MIO_BASE                  0x80028000000
#define GPIO_BASE                 (MIO_BASE + 0x6000)
#define GPIO_PORTA_BASE           (GPIO_BASE + 0x0 )
#define GPIO_PORTB_BASE           (GPIO_BASE + 0xC )
#define GPIO_PORTC_BASE           (GPIO_BASE + 0x18 )
#define GPIO_PORTD_BASE           (GPIO_BASE + 0x24 )
#define GPIO_DR                   (0x0)
#define GPIO_DDR                  (0x4)
#define GPIO_CTL                  (0x8)
#define GPIO_EXT_PORT_A           (0x50)
#define GPIO_EXT_PORT_B           (0x54)
#define GPIO_EXT_PORT_C           (0x58)

IPMI_PRIVATE_DATA                     *IpmiData;

VOID
InstallSmpiTable (
  IN EFI_EVENT    Event,
  IN VOID*        Context
);


VOID
BmcSmbiosCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS             Status;
  EFI_SMBIOS_PROTOCOL    *Smbios;
  SMBIOS_TABLE_TYPE38    *Type38 = NULL;
  EFI_SMBIOS_HANDLE      SmbiosHandle;
  
  if (Event != NULL) {
    gBS->CloseEvent(Event);
  }
  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    return;
  }

  Type38 = AllocateZeroPool(sizeof(SMBIOS_TABLE_TYPE38) + 2);
  if (Type38 == NULL) {
    DEBUG((EFI_D_ERROR, "AllocatePool Fail !\n" ));
    return;
  }

  Type38->Hdr.Type = EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION;
  Type38->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE38);
  Type38->Hdr.Handle = 0;

  Type38->InterfaceType = IPMIDeviceInfoInterfaceTypeKCS;
  Type38->IPMISpecificationRevision = 0x20;
  Type38->I2CSlaveAddress = BMC_SLAVE_ADDRESS;
  Type38->NVStorageDeviceAddress = 0xFF;
  Type38->BaseAddress = IpmiData->IpmiBaseAddress | BIT0;
  Type38->BaseAddressModifier_InterruptInfo = 0;
  Type38->InterruptNumber = 0;

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add (
                     Smbios,
                     NULL,
                     &SmbiosHandle,
                     (EFI_SMBIOS_TABLE_HEADER*)Type38
                     );  
  DEBUG((EFI_D_INFO, "Add Type38 : %r\n", Status));

  FreePool(Type38);
}

UINT32
GetGpioPortcState (UINT8 BitMap)
{
  UINT32 value = 0;

  value = MmioRead32(GPIO_PORTC_BASE + GPIO_DDR);
  value &= (~BitMap);      //write [BitMap]=0 to set gpio_c[BitMap] to input
  MmioWrite32((GPIO_PORTC_BASE + GPIO_DDR), value);

  value = MmioRead32(GPIO_BASE + GPIO_EXT_PORT_C);
 
  return value;
}

#define GPIO_PIN(socket, die, gpio)     ((socket << 14) + (die << 10) + gpio)

EFI_STATUS
CheckBmcFirmwareState ()
{
  EFI_STATUS    Status;
  AMD_CPM_TABLE_PROTOCOL    *CpmTableProtocolPtr;
  UINTN    TimeOut = 180;
  UINT32    GpioState;
  EFI_BOOT_MODE    BootMode;
  
  BootMode = GetBootModeHob();
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), BootMode :0x%x.\n", BootMode));  
  if(BootMode == BOOT_IN_RECOVERY_MODE){
    return EFI_SUCCESS;
  }

    Status = gBS->LocateProtocol (
                  &gAmdCpmTableProtocolGuid,
                  NULL,
                  (VOID**)&CpmTableProtocolPtr
                  );
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), locate gAmdCpmTableProtocolGuid :%r.\n", Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Status = EFI_NOT_READY;  
  GpioState = 0;
  while (TimeOut) {
    GpioState = CpmTableProtocolPtr->CommonFunction.GetGpio(CpmTableProtocolPtr, GPIO_PIN(0,0,86));
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), GpioState :%d.\n", GpioState));
    if (GpioState) {
      Status = EFI_SUCCESS;
      break;
    }
	
    gBS->Stall (1000000); // 1s
    TimeOut --;
  }

  return Status;
}


/**
 Entrypoint of dxe module.

 This function is the entry point of this module. It installs IPMI Interface Protocol
 in DXE phase.

 @param[in]         ImageHandle         The firmware allocated handle for the EFI image.
 @param[in]         SystemTable         A pointer to the EFI System Table.

 @retval EFI_SUCCESS                    Install protocol success.
 @retval !EFI_SUCCESS                   Install protocol fail.
*/
EFI_STATUS
EFIAPI
IpmiInterfaceDxeEntry (
  IN  EFI_HANDLE                        ImageHandle,
  IN  EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                            Status;
  SETUP_VOLATILE_DATA                   SetupVData;
  UINTN                                 VariableSize;
  VOID                                  *Registration;
  EFI_EVENT                             ReadyToBootEvent;
  UINTN         TempData;

  //
  // Allocate private data memory
  //
  IpmiData = (IPMI_PRIVATE_DATA*)AllocateZeroPool (sizeof (IPMI_PRIVATE_DATA));
  if (IpmiData == NULL) {
    return EFI_UNSUPPORTED;
  }

  //CheckBmcFirmwareState ();
  //
  // Fill DXE context
  //
  //
  // Calibrate TSC Counter.  Stall for 10ms, then multiply the resulting number of
  // ticks in that period by 100 to get the number of ticks in a 1 second timeout
  //
  TempData = AsmReadTsc();
  gBS->Stall(10 * 1000);
  TempData = MultU64x32 ((AsmReadTsc() - TempData), BMC_KCS_TIMEOUT * 100);
  DEBUG((EFI_D_INFO, "%a() TotalTimeTicks:%lX\n",__FUNCTION__, TempData));
  
  IpmiData->TotalTimeTicks =FixedPcdGet8 (PcdIpmiExecuteTime);// TempData;//FixedPcdGet8 (PcdIpmiExecuteTime);
  InitialIpmiInterfaceContent (IpmiData);
  if(IpmiData->BmcStatus != BMC_STATUS_OK){
    return EFI_UNSUPPORTED;
  }
  //
  // Install the Ipmi interface
  //
  Status = gBS->InstallProtocolInterface (
                  &IpmiData->ProtocolHandle,
                  &gByoIpmiInterfaceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &IpmiData->IpmiProtocol
                  );
  
  if(!EFI_ERROR(Status)){
    EfiCreateProtocolNotifyEvent (
      &gEfiSmbiosProtocolGuid,
      TPL_CALLBACK,
      BmcSmbiosCallback,
      NULL,
      &Registration
      );  
  }

  VariableSize = sizeof(SETUP_VOLATILE_DATA);
  Status = gRT->GetVariable (
                  SETUP_VOLATILE_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  NULL,
                  &VariableSize,
                  &SetupVData
                  );
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_INFO, "Failed to read SetupVolatileData variable: %r.\n", Status));
    VariableSize = sizeof(SETUP_VOLATILE_DATA);
    ZeroMem(&SetupVData, VariableSize); 
  }
  SetupVData.BmcPresent = 1;

  Status = gRT->SetVariable (
                  SETUP_VOLATILE_VARIABLE_NAME,
                  &gPlatformSetupVariableGuid,
                  EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  VariableSize,
                  &SetupVData
                  );

  Status = EfiCreateEventReadyToBootEx (
             TPL_NOTIFY,
             InstallSmpiTable,
             IpmiData,
             &ReadyToBootEvent
             );

  return Status;
}
