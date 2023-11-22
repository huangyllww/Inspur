/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
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

union {
     struct {
      UINT8    Resv : 5;
      UINT8    BiosBootType : 1;
      UINT8    Persistent : 1;
      UINT8    BootFlagValid : 1;
    } Bits;
    UINT8    Data;
  } Data1;


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
  EFI_STATUS                    Status;
  PEI_IPMI_BMC_INSTANCE_DATA    *mIpmiInstance;

  mIpmiInstance = NULL;

  //
  // Init LPC decoding to make sure IPMI stack can be earlier than PlatformPreMemInit
  // Note: 0xCA2 decode space need to be initialized before this module
  //
  //Status = PchLpcGenIoRangeSet ((IPMI_DEFAULT_SMM_IO_BASE  & 0xFF0), 0x10);
  //if (Status == EFI_SUCCESS)
  //{
  //  DEBUG ((DEBUG_INFO, "[IPMI_DEBUG]: PeiInitializeIpmiKCSPhysicalLayer PchLpcGenIoRangeSet 0xCA2!\n"));
  //}

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
  DEBUG((EFI_D_INFO,"IPMI Peim:IPMI STACK Initialization\n"));
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
  DEBUG((EFI_D_INFO,"IPMI Peim:KcsTimeoutPeriod=0x%x\n", mIpmiInstance->KcsTimeoutPeriod));
  //
  // Initialize IPMI IO Base.
  //
  mIpmiInstance->IpmiIoBase                     = PcdGet16(IpmiIoBase);
  DEBUG((EFI_D_INFO,"IPMI Peim:IpmiIoBase=0x%x\n",mIpmiInstance->IpmiIoBase));
  mIpmiInstance->Signature                      = SM_IPMI_BMC_SIGNATURE;
  mIpmiInstance->SlaveAddress                   = BMC_SLAVE_ADDRESS;
  mIpmiInstance->BmcStatus                      = EFI_BMC_NOTREADY;
  mIpmiInstance->IpmiTransportPpi.SendIpmiCommand = PeiEfiIpmiSendCommand;
  mIpmiInstance->IpmiTransportPpi.GetBmcStatus    = PeiEfiIpmiBmcStatus;
  mIpmiInstance->IpmiTransportPpi.LastCompleteCode = 0;

  mIpmiInstance->PeiIpmiBmcDataDesc.Flags         = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  mIpmiInstance->PeiIpmiBmcDataDesc.Guid          = &gPeiIpmiTransportPpiGuid;
  mIpmiInstance->PeiIpmiBmcDataDesc.Ppi           = &mIpmiInstance->IpmiTransportPpi;

  //
  // Get the Device ID and check if the system is in Force Update mode.
  //
  DEBUG((EFI_D_INFO,"IPMI Peim:Get BMC Device Id\n"));
  Status = GetDeviceId (mIpmiInstance);
  if (EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR,"IPMI Peim:Get BMC Device Id Failed. Status=%r\n",Status));
  }

  //
  // Do not continue initialization if the BMC is in Force Update Mode.
  //
  if (mIpmiInstance->BmcStatus == EFI_BMC_UPDATE_IN_PROGRESS || mIpmiInstance->BmcStatus == EFI_BMC_HARDFAIL) {
    return EFI_UNSUPPORTED;
  }


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
  IN      UINT8                        CommandDataSize,
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
  EFI_STATUS  Status;

  DEBUG_CODE_BEGIN ();
  UINTN  Index;
  UINT8* Buffer;

  DEBUG ((EFI_D_INFO, "NetFn(%x) Cmd(%x)\n", NetFunction, Command));
  if(CommandData!=NULL){
    DEBUG ((EFI_D_INFO, "->\n"));
    Buffer = (UINT8*)CommandData;
    for(Index=0;Index<CommandDataSize;Index++){
      DEBUG ((EFI_D_INFO, "%02x ",Buffer[Index]));
      if(((Index+1)%16) == 0){
        DEBUG ((EFI_D_INFO, "\n"));
      }
    }
    Buffer = NULL;
    DEBUG ((EFI_D_INFO, "\n"));
  }
  DEBUG_CODE_END ();

  //
  // This Will be unchanged ( BMC/KCS style )
  //
  Status = PeiIpmiSendCommandToBMC(
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

  DEBUG_CODE_BEGIN ();
  UINTN Index;
  UINT8* Buffer;
  if(!EFI_ERROR(Status) && ResponseData!=NULL && ResponseDataSize!=NULL && *ResponseDataSize){
    DEBUG ((EFI_D_INFO, "<-\n"));
    Buffer = (UINT8*)ResponseData;
    for(Index=0;Index<*ResponseDataSize;Index++){
      DEBUG ((EFI_D_INFO, "%02x ",Buffer[Index]));
      if(((Index+1)%16) == 0){
        DEBUG ((EFI_D_INFO, "\n"));
      }
    }
    Buffer = NULL;
    DEBUG ((EFI_D_INFO, "\n"));
  }
  if(EFI_ERROR(Status)){
    PEI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance;
    IpmiInstance = INSTANCE_FROM_PEI_SM_IPMI_BMC_THIS (This);
    DEBUG ((EFI_D_INFO, "IPMI_ERR(%X):%r\n", IpmiInstance->IpmiTransportPpi.LastCompleteCode, Status));
  }
  DEBUG_CODE_END ();

  return Status;

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
  EFI_STATUS                      Status;
  UINT8                           DataSize;
  EFI_SM_CTRL_INFO                *pBmcInfo;
  UINT32                          Retries;
  EFI_IPMI_MSG_GET_BMC_EXEC_RSP   *pBmcExecContext;
  EFI_SM_CTRL_INFO                *pBmcInfoHobPayload;
  UINT64                          Tsc1, Tsc2, Tsc, WaitTsc, Tsc1s;


  Tsc1s   = DivU64x32(mIpmiInstance->KcsTimeoutPeriod, BMC_KCS_TIMEOUT);
  WaitTsc = MultU64x32(Tsc1s, PcdGet16(PcdWaitBmcReadyMaxSeconds));
  Tsc1 = AsmReadTsc();
  DEBUG((EFI_D_INFO,"GetDeviceId Tsc1s = %0x,WaitTsc = %0x,Tsc1 = %0x\n",Tsc1s,WaitTsc,Tsc1));
  while(1) {
    //
    // Get the device ID information for the BMC.
    //
    DataSize = MAX_TEMP_DATA;
    Status = PeiEfiIpmiSendCommand(&mIpmiInstance->IpmiTransportPpi,
               EFI_SM_NETFN_APP,
               0,
               EFI_APP_GET_DEVICE_ID,
               NULL,
               0,
               mIpmiInstance->TempData,
               &DataSize
               );
    if (Status == EFI_SUCCESS) {
      DEBUG ((EFI_D_INFO, "IPMI: SendCommand success!\n"));
      break;
      
    } else {
      Tsc2 = AsmReadTsc();
      if(Tsc2 > Tsc1){
        Tsc = Tsc2 - Tsc1;
      } else {
        Tsc = 0xFFFFFFFFFFFFFFF - Tsc1 + 1 + Tsc2;
      }

      if(WaitTsc && Tsc >= WaitTsc){
        break;
      }
      //
      //Handle the case that BMC FW still not enable KCS channel after AC cycle. just stall 1 second
      //
      MicroSecondDelay(1*1000*1000);

      //
      // Display message and retry.
      //
      DEBUG (
        (EFI_D_ERROR | EFI_D_INFO,
        "IPMI: Waiting for BMC (0x%x)...\n",
        mIpmiInstance->IpmiIoBase)
        );

    }
  }
  if (EFI_ERROR (Status)) {
    mIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
    return Status;
  }
  pBmcInfo = (EFI_SM_CTRL_INFO*)&mIpmiInstance->TempData[0];
  DEBUG((EFI_D_INFO, "[IPMI PEI] BMC Device ID: 0x%02X, firmware version: %d.%02X UpdateMode:%x\n",
         pBmcInfo->DeviceId, pBmcInfo->MajorFirmwareRev, pBmcInfo->MinorFirmwareRev, pBmcInfo->UpdateMode));

  //
  //Build a BMC Info GUIDED HOB to cache EFI_SM_CTRL_INFO. It will be consumed by IPMI in DXE Phase
  //
  pBmcInfoHobPayload = BuildGuidHob(&gBmcInfoHobGuid, sizeof(EFI_SM_CTRL_INFO));
  CopyMem(pBmcInfoHobPayload, pBmcInfo, sizeof(EFI_SM_CTRL_INFO));

  //
  // In OpenBMC, UpdateMode: the bit 7 of byte 4 in get device id command is used for the BMC status:
  // 0 means BMC is ready, 1 means BMC is not ready.
  // At the very beginning of BMC power on, the status is 1 means BMC is in booting process and not ready. It is not the flag for force update mode.
  //
  if (pBmcInfo->UpdateMode == BMC_READY) {
    mIpmiInstance->BmcStatus = EFI_BMC_OK;
    return EFI_SUCCESS;
  } else {
    DataSize = sizeof(mIpmiInstance->TempData);
    Status = PeiEfiIpmiSendCommand(&mIpmiInstance->IpmiTransportPpi,
                                                   EFI_SM_NETFN_FIRMWARE,
                                                   0,
                                                   EFI_FIRMWARE_GET_BMC_EXECUTION_CONTEXT,
                                                   NULL,
                                                   0,
                                                   mIpmiInstance->TempData,
                                                   &DataSize
                                                   );
    pBmcExecContext = (EFI_IPMI_MSG_GET_BMC_EXEC_RSP*)&mIpmiInstance->TempData[0];
    DEBUG ((DEBUG_INFO, "[IPMI] Operational status of BMC: 0x%x\n", pBmcExecContext->CurrentExecutionContext));
    if ((pBmcExecContext->CurrentExecutionContext == EFI_FIRMWARE_BMC_IN_FORCED_UPDATE_MODE) &&
        !EFI_ERROR (Status)) {
      DEBUG ((DEBUG_ERROR, "[IPMI] BMC in Forced Update mode, skip waiting for BMC_READY.\n"));
      mIpmiInstance->BmcStatus = EFI_BMC_UPDATE_IN_PROGRESS;
    } else {
      //UpdateMode=1 mean BMC is not ready, continue waiting.
      while(Retries-- != 0) {
        MicroSecondDelay(1*1000*1000); //delay 1 seconds
        DataSize = sizeof(mIpmiInstance->TempData);
        Status = PeiEfiIpmiSendCommand(&mIpmiInstance->IpmiTransportPpi,
                                                EFI_SM_NETFN_APP,
                                                0,
                                                EFI_APP_GET_DEVICE_ID,
                                                NULL,
                                                0,
                                                mIpmiInstance->TempData,
                                                &DataSize
                                                );
        if(!EFI_ERROR(Status)) {
          pBmcInfo = (EFI_SM_CTRL_INFO*)&mIpmiInstance->TempData[0];
          DEBUG((DEBUG_INFO, "[IPMI] UpdateMode Retries: %d   pBmcInfo->UpdateMode:%x, Status: %r\n",Retries, pBmcInfo->UpdateMode, Status));
          if (pBmcInfo->UpdateMode == BMC_READY) {
            mIpmiInstance->BmcStatus = EFI_BMC_OK;
            return EFI_SUCCESS;
          }
        } else {
          DEBUG((DEBUG_INFO, "[IPMI] UpdateMode Retries: %d Status: %r\n", Retries, Status));
        }
      }
      mIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
    }
  }

  return Status;
} // GetDeviceId()
