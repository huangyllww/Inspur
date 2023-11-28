/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PeiGenericIpmi.c

Abstract:
  Source file for the IPMI PEIM.

Revision History:

**/

#include "PeiGenericIpmi.h"
#include <Library/TimerLib.h>
#include <Library/PcdLib.h>
#include <SysMiscCfg2.h>

union {
     struct {
      UINT8    Resv : 5;
      UINT8    BiosBootType : 1;
      UINT8    Persistent : 1;
      UINT8    BootFlagValid : 1;
    } Bits;
    UINT8    Data;
  } Data1;

EFI_STATUS
GetIpmiBootType (
  IN      PEI_IPMI_BMC_INSTANCE_DATA   *mIpmiInstance
  )
{
    EFI_STATUS  Status;
    UINT8       CmdDataBuff[10];
    UINT8       CmdDataSize;
    UINT8       ResponseBuff[0xCF];
    UINT8       ResponseSize;
    UINT8       Retry = 3;
    //UINT8       Data1Size;

    CmdDataBuff[0] = 0x5;
    CmdDataBuff[1] = 0;
    CmdDataBuff[2] = 0;
    CmdDataSize = 3;

    ResponseSize = sizeof (ResponseBuff);
    SetMem(ResponseBuff, ResponseSize, 0);

    do {
        Status = PeiEfiIpmiSendCommand(&mIpmiInstance->IpmiTransportPpi,
                                                   0x00,
                                                   0,
                                                   0x09,
                                                   (UINT8 *)CmdDataBuff, 
                                                   CmdDataSize,
                                                   ResponseBuff, 
                                                   &ResponseSize
                                                   );
    if (!EFI_ERROR (Status)) {
      break;
    }
    MicroSecondDelay(10000);
    Retry--;
  } while (0 != Retry);

  DEBUG((EFI_D_INFO,"XCL ResponseBuff IS %x",ResponseBuff[2]));
  if(ResponseBuff[2]&0x80){ // || ResponseBuff[2]&0x40){
     PcdSetBool(PcdIpmiBootflag, TRUE);
    if(ResponseBuff[2] & 0x20){
      PcdSet8S (PcdIpmiBootMode, BIOS_BOOT_UEFI_OS);
    } else {
      PcdSet8S (PcdIpmiBootMode, BIOS_BOOT_LEGACY_OS);
    }
    if(!(ResponseBuff[2] & 0x40)){
	  PcdSetBoolS (PcdIpmiBootOnce, TRUE);
	}
  }
  	 DEBUG((EFI_D_INFO,"ResponseBuff 2 = %0x,3 = %0x\n",ResponseBuff[2],ResponseBuff[3]));
  if((!(ResponseBuff[2] & 0x80))&& 
  	 (ResponseBuff[2] & 0x40)== 0x40 &&
  	 (ResponseBuff[3] & 0x18)== 0x18 ) {
  	 DEBUG((EFI_D_INFO,"Pcd set\n"));
      PcdSetBoolS (PcdIpmiBootToSetup, TRUE);
  }  
  return EFI_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
// Function Implementations
//

/*****************************************************************************
 @brief
  Internal function

 @param[in] PeiServices          General purpose services available to every PEIM.

 @retval EFI_SUCCESS             Always return EFI_SUCCESS
**/
EFI_STATUS
EFIAPI
PeiInitializeIpmiKCSPhysicalLayer (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS             Status;
  PEI_IPMI_BMC_INSTANCE_DATA       *mIpmiInstance;
  mIpmiInstance = NULL;

  //
  // Pilot3 part is not detected then exit this init routine
  //
  //move outside of ipmi stack in order to make ipmi stack no HW dependency
  //Status = CheckForPilot3();

  //if (!EFI_ERROR(Status))
  //{
  mIpmiInstance = AllocateZeroPool(sizeof(PEI_IPMI_BMC_INSTANCE_DATA));
  if (mIpmiInstance == NULL)
  {
	  DEBUG((EFI_D_ERROR,"IPMI Peim:EFI_OUT_OF_RESOURCES of memory allocation\n"));
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Calibrate TSC Counter.  Stall for 10ms, then multiply the resulting number of
  // ticks in that period by 100 to get the number of ticks in a 1 second timeout
  //
  DEBUG((EFI_D_ERROR,"IPMI Peim:IPMI STACK Initialization\n"));
  mIpmiInstance->KcsTimeoutPeriod = AsmReadTsc ();
  //DEBUG((EFI_D_ERROR,"IPMI Peim:Calc the delay for kcs timeout\n"));
  // Status =  PeiServicesLocatePpi(
  //               &gEfiPeiStallPpiGuid,
  //               0,
  //               NULL,
  //               &StallPpi
  //             );
  // ASSERT_EFI_ERROR (Status);
  // StallPpi->Stall (PeiServices, StallPpi, 10*1000);
  //
  // Use MicroSecondDelay to instead of Stall() in PeiServices
  // to remove the PPI dependency
  //
  MicroSecondDelay(10*1000);
  //
  // Initialize the KCS transaction timeout.
  //
  mIpmiInstance->KcsTimeoutPeriod = MultU64x32 ((AsmReadTsc () - mIpmiInstance->KcsTimeoutPeriod), BMC_KCS_TIMEOUT * 100);
  DEBUG((EFI_D_ERROR,"IPMI Peim:KcsTimeoutPeriod=0x%x\n", mIpmiInstance->KcsTimeoutPeriod));
  //
  // Initialize IPMI IO Base.
  //
  mIpmiInstance->IpmiIoBase                     = PcdGet16(IpmiIoBase);
  DEBUG((EFI_D_ERROR,"IPMI Peim:IpmiIoBase=0x%x\n",mIpmiInstance->IpmiIoBase));
  mIpmiInstance->Signature                      = SM_IPMI_BMC_SIGNATURE;
  mIpmiInstance->SlaveAddress                   = BMC_SLAVE_ADDRESS;
  mIpmiInstance->BmcStatus                      = EFI_BMC_NOTREADY;
  mIpmiInstance->IpmiTransportPpi.SendIpmiCommand = PeiEfiIpmiSendCommand;
  mIpmiInstance->IpmiTransportPpi.GetBmcStatus    = PeiEfiIpmiBmcStatus;

  mIpmiInstance->PeiIpmiBmcDataDesc.Flags         = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  mIpmiInstance->PeiIpmiBmcDataDesc.Guid          = &gPeiIpmiTransportPpiGuid;
  mIpmiInstance->PeiIpmiBmcDataDesc.Ppi           = &mIpmiInstance->IpmiTransportPpi;

  //
  // Get the Device ID and check if the system is in Force Update mode.
  //
  DEBUG((EFI_D_ERROR,"IPMI Peim:Get BMC Device Id\n"));
  Status = GetDeviceId (mIpmiInstance);
  //
  // Do not continue initialization if the BMC is in Force Update Mode.
  //
  if (mIpmiInstance->BmcStatus == EFI_BMC_UPDATE_IN_PROGRESS || mIpmiInstance->BmcStatus == EFI_BMC_HARDFAIL) {
    return EFI_UNSUPPORTED;
  }

  Status = GetIpmiBootType (mIpmiInstance);
  if (mIpmiInstance->BmcStatus == EFI_BMC_UPDATE_IN_PROGRESS || mIpmiInstance->BmcStatus == EFI_BMC_HARDFAIL) {
    DEBUG((EFI_D_INFO,"Get ipmiboottype error\n"));
    return EFI_UNSUPPORTED;
  }
  //}

  //
  // Just produce PPI
  //
  Status = PeiServicesInstallPpi (&mIpmiInstance->PeiIpmiBmcDataDesc);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}


/*****************************************************************************
 @bref
  The entry point of the Ipmi PEIM. Instals Ipmi PPI interface.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services. 
  
  @retval EFI_SUCCESS   Indicates that Ipmi initialization completed successfully.
**/
EFI_STATUS
PeimIpmiInterfaceInit (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  //
  // Performing Ipmi KCS physical layer initialization
  //
  Status = PeiInitializeIpmiKCSPhysicalLayer (PeiServices);

  return EFI_SUCCESS;
} // PeimIpmiInterfaceInit()


EFI_STATUS
PeiEfiIpmiSendCommand (
  IN      PEI_IPMI_TRANSPORT_PPI       *This,
  IN      UINT8                        NetFunction,
  IN      UINT8                        Lun,
  IN      UINT8                        Command,
  IN      UINT8                        *CommandData,
  IN      UINTN                        CommandDataSize,
  IN OUT  UINT8                        *ResponseData,
  IN OUT  UINT8                        *ResponseDataSize
  )
/*++

Routine Description:

  Send Ipmi Command in the right mode: HECI or KCS,  to the
  appropiate device, ME or BMC.

Arguments:
  
  This              - Pointer to IPMI protocol instance
  NetFunction       - Net Function of command to send
  Lun               - LUN of command to send
  Command           - IPMI command to send
  CommandData       - Pointer to command data buffer, if needed
  CommandDataSize   - Size of command data buffer
  ResponseData      - Pointer to response data buffer
  ResponseDataSize  - Pointer to response data buffer size

Returns:
  
  EFI_INVALID_PARAMETER - One of the input values is bad
  EFI_DEVICE_ERROR      - IPMI command failed
  EFI_BUFFER_TOO_SMALL  - Response buffer is too small
  EFI_UNSUPPORTED       - Command is not supported by BMC
  EFI_SUCCESS           - Command completed successfully

--*/
{
  //
  // This Will be unchanged ( BMC/KCS style )
  //
  return PeiIpmiSendCommandToBMC(
                                This,
                                NetFunction,
                                Lun,
                                Command,
                                CommandData,
                                CommandDataSize,
                                ResponseData,
                                ResponseDataSize,
                                NULL
                                );
} // EfiIpmiSendCommand()

EFI_STATUS
PeiEfiIpmiBmcStatus (
  IN      PEI_IPMI_TRANSPORT_PPI       *This,
  OUT EFI_BMC_STATUS                   *BmcStatus,
  OUT EFI_SM_COM_ADDRESS               *ComAddress
  )
/*++

Routine Description:

  Updates the BMC status and returns the Com Address

Arguments:

  This        - Pointer to IPMI protocol instance
  BmcStatus   - BMC status
  ComAddress  - Com Address

Returns:

  EFI_SUCCESS - Success

--*/
{
  return PeiIpmiBmcStatus (
                        This,
                        BmcStatus,
                        ComAddress,
                        NULL
                        );
}


EFI_STATUS
GetDeviceId (
  IN      PEI_IPMI_BMC_INSTANCE_DATA   *mIpmiInstance
  )
/*++

Routine Description:
  Execute the Get Device ID command to determine whether or not the BMC is in Force Update
  Mode.  If it is, then report it to the error manager.

Arguments:
  mIpmiInstance   - Data structure describing BMC variables and used for sending commands
  StatusCodeValue - An array used to accumulate error codes for later reporting.
  ErrorCount      - Counter used to keep track of error codes in StatusCodeValue

Returns: 
  Status

--*/
{
  EFI_STATUS          Status;
  UINT8               DataSize;
  EFI_SM_CTRL_INFO    *pBmcInfo;
  UINT32              Retries;
  
  //
  // Set up a loop to retry for up to 30 seconds. Calculate retries not timeout
  // so that in case KCS is not enabled and EfiIpmiSendCommand() returns
  // immediately we will not wait all the 30 seconds.
  //
  Retries = BMC_TIMEOUT_PEI/ BMC_KCS_TIMEOUT + 1;
  //
  // Get the device ID information for the BMC.
  //
  DataSize = sizeof(mIpmiInstance->TempData);
  while (EFI_ERROR(Status = PeiEfiIpmiSendCommand(&mIpmiInstance->IpmiTransportPpi,
                                                   EFI_SM_NETFN_APP,
                                                   0,
                                                   EFI_APP_GET_DEVICE_ID,
                                                   NULL, 
                                                   0,
                                                   mIpmiInstance->TempData, 
                                                   &DataSize
                                                   )))
  {
    DEBUG((EFI_D_ERROR, "[IPMI] BMC does not respond (status: %r), %d retries left\n",
           Status, Retries));
    if (Retries-- == 0)
    {
      mIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
      return Status;
    }
  }
  pBmcInfo = (EFI_SM_CTRL_INFO*)&mIpmiInstance->TempData[0];
  DEBUG((EFI_D_ERROR, "[IPMI] BMC Device ID: 0x%02X, firmware version: %d.%02X\n",
         pBmcInfo->DeviceId, pBmcInfo->MajorFirmwareRev, pBmcInfo->MinorFirmwareRev));
  
  return Status;
} // GetDeviceId()
