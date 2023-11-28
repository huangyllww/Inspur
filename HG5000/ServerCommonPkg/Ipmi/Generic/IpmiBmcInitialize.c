/** @file

Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
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
#include <Library/PcdLib.h>
#include <AmdCpmDxe.h>



STATIC EFI_IPMI_BMC_INSTANCE_DATA  gPrivate;
       EFI_IPMI_BMC_INSTANCE_DATA  *mIpmiInstance = &gPrivate;


/**
  Update KCS status in ACPI tables according to its actuall state
  
  This routine tests if ACPI protocol is installed and if so it updates
  the IPMI device status immediately, otherwise it installs itself as a notify
  function to be called when ACPI protocol is installed.

  @param[in] Event      Event is used to distinguish immediate call
  @param[in] pContext  Pointer to EFI_IPMI_BMC_INSTANCE_DATA

  @return Void.
**/
VOID
IpmiAcpiUpdateAsl(
  IN EFI_EVENT                 Event,
  IN VOID                     *Context)
{
  EFI_STATUS                   Status;
  EFI_ACPI_TABLE_PROTOCOL     *pAcpiProtocol = NULL;
  EFI_ACPI_SDT_PROTOCOL       *pAcpiSdt = NULL;  
  EFI_ACPI_DESCRIPTION_HEADER *pTableHeader = NULL;
  EFI_ACPI_TABLE_VERSION       TableVersion;
  INTN                         TableIndex;
  UINTN                        TableHandle;
  UINT16                       DevLen;
  UINT8                       *pByte = NULL;
  UINT8                       *pDev = NULL;

  DEBUG((EFI_D_ERROR,"%a\n",__FUNCTION__));
  if (Event != NULL){
    gBS->CloseEvent(Event);
  }
  Status = gBS->LocateProtocol(&gEfiAcpiTableProtocolGuid, NULL, &pAcpiProtocol);
  if (!EFI_ERROR(Status)){
    Status = gBS->LocateProtocol(&gEfiAcpiSdtProtocolGuid, NULL, &pAcpiSdt);
  }


   //
  // Locate 'DSDT' table. ACPI protocol does not allow for asking for particular signature.
  // Must walk through the tables and check for signature.
  //
  for (TableIndex = 0; TableIndex < 64; TableIndex++) {
    DEBUG((EFI_D_ERROR,"TableIndex = %d\n",TableIndex));
    Status = pAcpiSdt->GetAcpiTable(TableIndex, (EFI_ACPI_SDT_HEADER **)&pTableHeader,
                                    &TableVersion, &TableHandle);
    DEBUG((EFI_D_ERROR,"pAcpiSdt->GetAcpiTable Status = %r\n",Status));

    if (EFI_ERROR(Status)) { 
      //
      // ACPI DSDT table not found, try again at ready-to-boot.
      // All tables should be ready at that time.
      //
#if 0
      Status = EfiCreateEventReadyToBootEx (TPL_NOTIFY, IpmiAcpiUpdateAsl, NULL, &Event);
      ASSERT_EFI_ERROR(Status);
#endif
      return;
    }

    DEBUG((EFI_D_ERROR,"pTableHeader->Signature = %X\n",pTableHeader->Signature));
    
    if (pTableHeader->Signature == SIGNATURE_32('D','S','D','T')) { 
      //
      // Got DSDT, now find 'IBMC' device and update its _STA object.
      //

      DEBUG((EFI_D_ERROR,"pByte = %X ,  pTableHeader->Length = %x\n",(UINT8*)&pTableHeader[1],pTableHeader->Length));
      
      for (pByte = (UINT8*)&pTableHeader[1]; pByte < (UINT8*)pTableHeader + pTableHeader->Length; pByte++) { 
        //
        // Look for 'IBMC' signature in the AML
        //
        if (*(UINT32*)pByte == SIGNATURE_32('I','B','M','C')){ 
          //
          // Make sure it's AML Device object and find its length
          //
          DevLen = 0;
          if (pByte[-2] == AML_EXT_DEVICE_OP){
            DevLen = pByte[-1];
          } else if (pByte[-3] == AML_EXT_DEVICE_OP) {
            DevLen = *(UINT16*)(pByte - 2);
            DevLen = ((DevLen & 0x0FF00) >> 4) | (DevLen & 0x0F);
          }
          //
          // Conditional match.  Search _CSR in Device (LDRC).
          //
          for (pDev = pByte; pByte < pDev + DevLen; pByte++) { 
            //
            // Look for '_STA' signature
            //
            if (*(UINT32*)pByte == SIGNATURE_32('_', 'S', 'T', 'A')) { 
              //
              // Modify only if byte, as expected. Otherwise do not touch to not break AML.
              //
              if (pByte[4] == AML_BYTE_PREFIX) {
                pByte[5] = 0x0F; // KCS present and functioning
                //
                // Update the modified ACPI table
                //
                Status = pAcpiProtocol->InstallAcpiTable(pAcpiProtocol, pTableHeader,
                                                         pTableHeader->Length, &TableHandle);
                ASSERT_EFI_ERROR(Status);
              }
              break; // for (pDev...)
            }
          }
          break; // for (pTable...)
        } // if (...IBMC...)
      } // for (pByte...)
      //FreePool(pTableHeader);
      break; // for (TableIndex...)
    } // if (DSDT)
    //FreePool(pTableHeader);
  }
  return;
}


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
  Type38->InterfaceType = IPMIDeviceInfoInterfaceTypeKCS;
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
SetTimeStamp(VOID)
/**+

  Send the time to the BMC, in the UNIX 32 bit format.

  @param none

  @retval Status          - result of sending the time stamp

**/
{
  EFI_STATUS            Status;
  UINT32                NumOfSeconds;
  UINT8                 DataSize;
  STATIC EFI_SEL_RECORD_DATA   TimeStampEvtRecord = {
    0,                                  // Record Id
    EFI_SEL_SYSTEM_RECORD,              // Record Type
    0,                                  // Time stamp
    0x0001,                             // GenID:BIOS
    EFI_EVM_REVISION,                   // EVM REV
    0x12,                               // Sensor Type
    0x83,                               // Sensor No
    EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE,// Event Dir
    05,                                 // Sensor specific Offset for Timestamp Clock Synch Event.
    00,                                 // ED2
    0xFF                                // ED3
  };


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  //
  // Log Timestamp Clock Synch Event 1st pair.
  //
  DataSize = MAX_TEMP_DATA;
  Status = EfiIpmiSendCommand (
             &mIpmiInstance->IpmiTransport,
             EFI_SM_NETFN_STORAGE,
             0,
             EFI_STORAGE_ADD_SEL_ENTRY,
             (UINT8 *) &TimeStampEvtRecord,
             sizeof (EFI_SEL_RECORD_DATA),
             mIpmiInstance->TempData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = EfiSmGetTimeStamp (&NumOfSeconds);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DataSize = MAX_TEMP_DATA;
  Status = EfiIpmiSendCommand (
             &mIpmiInstance->IpmiTransport,
             EFI_SM_NETFN_STORAGE,
             0,
             EFI_STORAGE_SET_SEL_TIME,
             (UINT8 *) &NumOfSeconds,
             4,
             mIpmiInstance->TempData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Log Timestamp Clock Synch Event 2nd pair.
  //
  TimeStampEvtRecord.OEMEvData2 = 0x80;
  DataSize                      = MAX_TEMP_DATA;
  Status = EfiIpmiSendCommand (
             &mIpmiInstance->IpmiTransport,
             EFI_SM_NETFN_STORAGE,
             0,
             EFI_STORAGE_ADD_SEL_ENTRY,
             (UINT8 *) &TimeStampEvtRecord,
             sizeof (EFI_SEL_RECORD_DATA),
             mIpmiInstance->TempData,
             &DataSize
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}



EFI_STATUS GetBmcVersion()
{
  EFI_SM_CTRL_INFO  ControllerInfo;  
  UINT8             ResponseDataSize;
  EFI_STATUS        Status;
  UINT8             Version[3];


  ResponseDataSize = sizeof(ControllerInfo);
  Status = EfiIpmiSendCommand (
            &mIpmiInstance->IpmiTransport,
            EFI_SM_NETFN_APP,
            0,
            EFI_APP_GET_DEVICE_ID,
            NULL,
            0,
            (UINT8*)&ControllerInfo,
            (UINT8*)&ResponseDataSize
            );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
    return Status;
  }

  Version[2] = ControllerInfo.MajorFirmwareRev;
  Version[1] = ControllerInfo.MinorFirmwareRev;             // BCD
  Version[1] = ((Version[1] >> 4) & 0xF) * 10 + (Version[1] & 0xF);
  Version[0] = (UINT8)(ControllerInfo.AuxFirmwareRevInfo & 0xFF);
  PcdSet32(PcdBmcVersion, (UINT32)((Version[2] << 16) | (Version[1] << 8) | (Version[0])));

  DEBUG((EFI_D_INFO, "BMC Ver:%d.%d.%d\n", Version[2], Version[1], Version[0]));

  return EFI_SUCCESS;
}


EFI_STATUS GetCpldVersion()
{
  UINT16            RspData; 
  UINT8             RspDataSize;
  EFI_STATUS        Status;
  UINT8             *p;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  RspDataSize = sizeof(RspData);
  Status = EfiIpmiSendCommand (
            &mIpmiInstance->IpmiTransport,
            0x3E,
            0,
            0x84,
            NULL,
            0,
            (UINT8*)&RspData,
            &RspDataSize
            );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_INFO, "(L%d) %r\n", __LINE__, Status));
    return Status;
  }

  p = (UINT8*)&RspData;
  RspData = (p[0] << 8) + p[0];
  PcdSet32(PcdCpldVersion, RspData);

  DEBUG((EFI_D_INFO, "CPLD Ver:%X\n", RspData));

  return EFI_SUCCESS;
}




VOID
EFIAPI
IpmiEnterSetupCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  VOID        *Interface;
  EFI_STATUS  Status;
  

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  GetBmcVersion(); 
  GetCpldVersion();
}

#define GPIO_PIN(socket, die, gpio)     ((socket << 14) + (die << 10) + gpio)


EFI_STATUS
CheckBmcState ()
{
  EFI_STATUS    Status;
  AMD_CPM_TABLE_PROTOCOL    *CpmTableProtocolPtr;
  UINTN    TimeOut = 180;
  UINT32    GpioState;  
    

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
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), GpioState :%d,timeout = %d.\n", GpioState,TimeOut));
    if (GpioState) {
      Status = EFI_SUCCESS;
      break;
    }
	
    gBS->Stall (1000000); // 1s
    TimeOut --;
  }

  return Status;
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

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  CheckBmcState();
  
  //
  // Calibrate TSC Counter.  Stall for 10ms, then multiply the resulting number of
  // ticks in that period by 100 to get the number of ticks in a 1 second timeout
  //
  mIpmiInstance->KcsTimeoutPeriod = AsmReadTsc();
  gBS->Stall(10 * 1000);
  mIpmiInstance->KcsTimeoutPeriod = MultU64x32 ((AsmReadTsc() - mIpmiInstance->KcsTimeoutPeriod), BMC_KCS_TIMEOUT * 100);
  DEBUG((EFI_D_INFO, "%a() KcsTimeoutPeriod:%lX\n",__FUNCTION__, mIpmiInstance->KcsTimeoutPeriod));

  mIpmiInstance->Signature                      = SM_IPMI_BMC_SIGNATURE;
  mIpmiInstance->IpmiIoBase                     = PcdGet16(IpmiIoBase);
  mIpmiInstance->SlaveAddress                   = BMC_SLAVE_ADDRESS;
  mIpmiInstance->BmcStatus                      = EFI_BMC_OK;
  mIpmiInstance->IpmiTransport.SendIpmiCommand  = EfiIpmiSendCommand;
  mIpmiInstance->IpmiTransport.GetBmcStatus     = EfiIpmiBmcStatus;
  mIpmiInstance->IpmiTransport.LastCompleteCode = 0;
  

// Get the Device ID and check if the system is in Force Update mode.
// Do not continue initialization if the BMC is in Force Update Mode.
// If the BMC is in an OK or Softfail state, then continue with initialization.
  Status = GetDeviceId(mIpmiInstance, StatusCodeValue, &ErrorCount);
  if (mIpmiInstance->BmcStatus != EFI_BMC_UPDATE_IN_PROGRESS){
    Status = GetSelfTest(mIpmiInstance, StatusCodeValue, &ErrorCount);
    if (mIpmiInstance->BmcStatus == EFI_BMC_OK || mIpmiInstance->BmcStatus == EFI_BMC_SOFTFAIL) {
      //SetTimeStamp();
    }
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

    EfiCreateProtocolNotifyEvent (
      &gEfiSetupEnterGuid,
      TPL_CALLBACK,
      IpmiEnterSetupCallBack,
      NULL,
      &Registration
      ); 
   
  }
  
  if(!EFI_ERROR(Status)){
    EfiCreateProtocolNotifyEvent (
      &gEfiSmbiosProtocolGuid,
      TPL_CALLBACK,
      BmcSmbiosCallback,
      NULL,
      &Registration
      );  
  }

  return EFI_SUCCESS;
}




