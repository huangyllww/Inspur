/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiBmcInitialize.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#include "IpmiBmcCommon.h"
#include "IpmiBmc.h"
#include <BaseIpmi.h>


STATIC EFI_IPMI_BMC_INSTANCE_DATA  gPrivate;
       EFI_IPMI_BMC_INSTANCE_DATA  *mIpmiInstance = &gPrivate;


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
  

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  if (EFI_ERROR(Status)) {
    return;
  }

  gBS->CloseEvent(Event);

  
  Type38 = AllocateZeroPool(sizeof(SMBIOS_TABLE_TYPE38) + 2);
  ASSERT(Type38 != NULL);

  Type38->Hdr.Type = EFI_SMBIOS_TYPE_IPMI_DEVICE_INFORMATION;
  Type38->Hdr.Length = sizeof(SMBIOS_TABLE_TYPE38);
  Type38->InterfaceType = FixedPcdGet8(PcdIpmiInterfaceType);
  Type38->IPMISpecificationRevision = 0x20;
  Type38->I2CSlaveAddress = BMC_SLAVE_ADDRESS;
  Type38->NVStorageDeviceAddress = 0xFF;
  Type38->BaseAddress = mIpmiInstance->IpmiIoBase | BIT0;
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




EFI_STATUS
GenericIpmiInit (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS             Status = EFI_SUCCESS;
  VOID                   *Registration;
  UINT8                  ErrorCount = 0;
  UINT8                  Index;
  EFI_STATUS_CODE_VALUE  StatusCodeValue[MAX_SOFT_COUNT];


  DEBUG((EFI_D_INFO, "GenericIpmiInit\n"));

  switch(FixedPcdGet8(PcdIpmiInterfaceType)){
    
    case IPMIDeviceInfoInterfaceTypeKCS:
      //
      // Calibrate TSC Counter.  Stall for 10ms, then multiply the resulting number of
      // ticks in that period by 100 to get the number of ticks in a 1 second timeout
      //
      mIpmiInstance->KcsTimeoutPeriod = AsmReadTsc();
      gBS->Stall(10 * 1000);
      mIpmiInstance->KcsTimeoutPeriod = MultU64x32 ((AsmReadTsc() - mIpmiInstance->KcsTimeoutPeriod), BMC_KCS_TIMEOUT * 100);
      DEBUG((EFI_D_INFO, "KcsTimeoutPeriod:%lX\n", mIpmiInstance->KcsTimeoutPeriod));

      mIpmiInstance->Signature                      = SM_IPMI_BMC_SIGNATURE;
      mIpmiInstance->IpmiIoBase                     = PcdGet16(IpmiIoBase);
      mIpmiInstance->SlaveAddress                   = BMC_SLAVE_ADDRESS;
      mIpmiInstance->BmcStatus                      = EFI_BMC_OK;
      mIpmiInstance->IpmiTransport.SendIpmiCommand  = EfiIpmiSendCommand;
      mIpmiInstance->IpmiTransport.GetBmcStatus     = EfiIpmiBmcStatus;
      mIpmiInstance->IpmiTransport.LastCompleteCode = 0;
      break;

    case IPMIDeviceInfoInterfaceTypeBT:
      mIpmiInstance->Signature                      = SM_IPMI_BMC_SIGNATURE;
      mIpmiInstance->IpmiIoBase                     = PcdGet16(IpmiIoBase);
      mIpmiInstance->SlaveAddress                   = BMC_SLAVE_ADDRESS;
      mIpmiInstance->BmcStatus                      = EFI_BMC_OK;
      mIpmiInstance->IpmiTransport.SendIpmiCommand  = EfiIpmiBtSendCommand;
      mIpmiInstance->IpmiTransport.GetBmcStatus     = EfiIpmiBmcStatus;
      mIpmiInstance->IpmiTransport.LastCompleteCode = 0;
      mIpmiInstance->RetryCounter                   = IPMI_BT_CMD_RETRY_TIME;
      break;

    default:
      DEBUG((EFI_D_ERROR, "Unsupported Ipmi iF.\n"));
      return EFI_UNSUPPORTED;
  }
  

// Get the Device ID and check if the system is in Force Update mode.
// Do not continue initialization if the BMC is in Force Update Mode.
// If the BMC is in an OK or Softfail state, then continue with initialization.
  Status = GetDeviceId(mIpmiInstance, StatusCodeValue, &ErrorCount);
  if (mIpmiInstance->BmcStatus != EFI_BMC_UPDATE_IN_PROGRESS){
    Status = GetSelfTest(mIpmiInstance, StatusCodeValue, &ErrorCount);
//-    if (mIpmiInstance->BmcStatus == EFI_BMC_OK || mIpmiInstance->BmcStatus == EFI_BMC_SOFTFAIL) {
//-      SetTimeStamp();
//-    }
  }

  for (Index = 0; Index < ErrorCount; Index++){
    ReportStatusCode(EFI_ERROR_CODE | EFI_ERROR_MAJOR, StatusCodeValue[Index]);
  }

// Now install the Protocol if the BMC is not in a HardFail State and not in Force Update mode
  if (mIpmiInstance->BmcStatus != EFI_BMC_HARDFAIL && mIpmiInstance->BmcStatus != EFI_BMC_UPDATE_IN_PROGRESS){
    Status = gBS->InstallProtocolInterface(
                      &gImageHandle, 
                      &gEfiIpmiTransportProtocolGuid, 
                      EFI_NATIVE_INTERFACE,
                      &mIpmiInstance->IpmiTransport
                      );
    ASSERT_EFI_ERROR(Status);
  }
  
  if(!EFI_ERROR(Status)){
    EfiCreateProtocolNotifyEvent (
      &gEfiSmbiosProtocolGuid,
      TPL_CALLBACK,
      BmcSmbiosCallback,
      NULL,
      &Registration
      );  

    EfiCreateProtocolNotifyEvent (
      &gEfiAcpiTableProtocolGuid,
      TPL_CALLBACK,
      InstallSmpiTable,
      NULL,
      &Registration
      );  
  }

  return EFI_SUCCESS;
}




