/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiBmc.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#include "IpmiBmc.h"

EFI_STATUS
UpdateErrorStatus (
  IN UINT8                      BmcError,
  EFI_IPMI_BMC_INSTANCE_DATA    *IpmiInstance
  )
/*++

Routine Description:

  Check if the completion code is a Soft Error and increment the count.  The count
  is not updated if the BMC is in Force Update Mode.

Arguments:

  BmcError      - Completion code to check
  IpmiInstance  - BMC instance data

Returns:

  EFI_SUCCESS   - Status

--*/
{
  UINT8   Errors[] = COMPLETION_CODES;
  UINT16  CodeCount;
  UINT8   i;

  CodeCount = sizeof (Errors) / sizeof (Errors[0]);
  for (i = 0; i < CodeCount; i++) {
    if (BmcError == Errors[i]) {
      //
      // Don't change Bmc Status flag if the BMC is in Force Update Mode.
      //
      if (IpmiInstance->BmcStatus != EFI_BMC_UPDATE_IN_PROGRESS) {
        IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
      }

      IpmiInstance->SoftErrorCount++;
      break;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiSendCommandToBMC (
  IN      EFI_IPMI_TRANSPORT            *This,
  IN      UINT8                         NetFunction,
  IN      UINT8                         Lun,
  IN      UINT8                         Command,
  IN      UINT8                         *CommandData,
  IN      UINTN                         CommandDataSize,
  IN OUT  UINT8                         *ResponseData,
  IN OUT  UINT8                         *ResponseDataSize,
  IN      VOID                          *Context
  )
/*++

Routine Description:

  Send IPMI command to BMC

Arguments:
  
  This              - Pointer to IPMI protocol instance
  NetFunction       - Net Function of command to send
  Lun               - LUN of command to send
  Command           - IPMI command to send
  CommandData       - Pointer to command data buffer, if needed
  CommandDataSize   - Size of command data buffer
  ResponseData      - Pointer to response data buffer
  ResponseDataSize  - Pointer to response data buffer size
  Context           - Context

Returns:
  
  EFI_INVALID_PARAMETER - One of the input values is bad
  EFI_DEVICE_ERROR      - IPMI command failed
  EFI_BUFFER_TOO_SMALL  - Response buffer is too small
  EFI_UNSUPPORTED       - Command is not supported by BMC
  EFI_SUCCESS           - Command completed successfully
  
--*/
{
  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance;
  UINT8                       DataSize;
  EFI_STATUS                  Status;
  EFI_IPMI_COMMAND            *IpmiCommand;
  EFI_IPMI_RESPONSE           *IpmiResponse;

  IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

  //
  // The TempData buffer is used for both sending command data and receiving
  // response data.  Since the command format is different from the response
  // format, the buffer is cast to both structure definitions.
  //
  IpmiCommand   = (EFI_IPMI_COMMAND *) IpmiInstance->TempData;
  IpmiResponse  = (EFI_IPMI_RESPONSE *) IpmiInstance->TempData;

  //
  // Send IPMI command to BMC
  //
  IpmiCommand->Lun          = Lun;
  IpmiCommand->NetFunction  = NetFunction;
  IpmiCommand->Command      = Command;

  //
  // Ensure that the buffer is valid before attempting to copy the command data
  // buffer into the IpmiCommand structure.
  //
  if (CommandDataSize > 0) {
    if (CommandData == NULL) {
      return EFI_INVALID_PARAMETER;
    }

    CopyMem (
      IpmiCommand->CommandData,
      CommandData,
      CommandDataSize
      );
  }

  Status = SendDataToBmcPort (
             IpmiInstance,
             Context,
             (UINT8 *)IpmiCommand,
             (CommandDataSize + EFI_IPMI_COMMAND_HEADER_SIZE)
             );
  if (Status != EFI_SUCCESS) {
    IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
    IpmiInstance->SoftErrorCount++;
    return Status;
  }

  //
  // Get Response to IPMI Command from BMC.
  //
  DataSize = MAX_TEMP_DATA;
  Status = ReceiveBmcDataFromPort (
             IpmiInstance,
             Context,
             (UINT8 *)IpmiResponse,
             &DataSize
             );
  if (Status != EFI_SUCCESS) {
    IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
    IpmiInstance->SoftErrorCount++;
    return Status;
  }
  IpmiInstance->IpmiTransport.LastCompleteCode = IpmiResponse->CompletionCode;

  if ((IpmiResponse->CompletionCode != COMP_CODE_NORMAL) &&
      (IpmiInstance->BmcStatus == EFI_BMC_UPDATE_IN_PROGRESS)) {
    //
    // If the completion code is not normal and the BMC is in Force Update
    // mode, then update the error status and return EFI_UNSUPPORTED.
    //
    UpdateErrorStatus (
      IpmiResponse->CompletionCode,
      IpmiInstance
      );
    return EFI_UNSUPPORTED;
  } else if (IpmiResponse->CompletionCode != COMP_CODE_NORMAL) {
    //
    // Otherwise if the BMC is in normal mode, but the completion code
    // is not normal, then update the error status and return device error.
    //
    UpdateErrorStatus (
      IpmiResponse->CompletionCode,
      IpmiInstance
      );
    if(IpmiResponse->CompletionCode == COMP_CODE_TOO_MANY_BYTES){
      return EFI_OUT_OF_RESOURCES;
    }
    return EFI_DEVICE_ERROR;
  }

  //
  // If we got this far without any error codes, but the DataSize is 0 then the 
  // command response failed, so do not continue.  
  //
  if (DataSize == 0) {
    return EFI_DEVICE_ERROR;
  }

  //
  // Verify the response data buffer passed in is big enough.
  //
  if ((DataSize - EFI_IPMI_RESPONSE_HEADER_SIZE) > *ResponseDataSize) {
    return EFI_BUFFER_TOO_SMALL;
  }

  //
  // Copy data over to the response data buffer.
  //
  *ResponseDataSize = DataSize - EFI_IPMI_RESPONSE_HEADER_SIZE;
  CopyMem (
    ResponseData,
    IpmiResponse->ResponseData,
    *ResponseDataSize
    );

  return EFI_SUCCESS;
}


EFI_STATUS
IpmiBmcStatus (
  IN  EFI_IPMI_TRANSPORT              *This,
  OUT EFI_BMC_STATUS                  *BmcStatus,
  OUT EFI_SM_COM_ADDRESS              *ComAddress,
  IN  VOID                            *Context
  )
/*++

Routine Description:

  Updates the BMC status and returns the Com Address

Arguments:

  This        - Pointer to IPMI protocol instance
  BmcStatus   - BMC status
  ComAddress  - Com Address
  Context     - Context

Returns:

  EFI_SUCCESS - Success
  
--*/
{
  EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance;

  IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);

  if ((IpmiInstance->BmcStatus == EFI_BMC_SOFTFAIL) && (IpmiInstance->SoftErrorCount >= MAX_SOFT_COUNT)) {
    IpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
  }

  *BmcStatus = IpmiInstance->BmcStatus;
  ComAddress->ChannelType = EfiSmBmc;
  ComAddress->Address.BmcAddress.LunAddress = 0x0;
  ComAddress->Address.BmcAddress.ChannelAddress = 0x0;

  return EFI_SUCCESS;
}

EFI_STATUS
EfiIpmiBmcStatus (
  IN EFI_IPMI_TRANSPORT                *This,
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
  return IpmiBmcStatus (
           This,
           BmcStatus,
           ComAddress,
           NULL
           );
}



EFI_STATUS
EfiIpmiSendCommand (
  IN      EFI_IPMI_TRANSPORT           *This,
  IN      UINT8                        NetFunction,
  IN      UINT8                        Lun,
  IN      UINT8                        Command,
  IN      UINT8                        *CommandData,
  IN      UINTN                        CommandDataSize,
  IN OUT  UINT8                        *ResponseData,
  IN OUT  UINT8                        *ResponseDataSize
  )
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

  Status = IpmiSendCommandToBMC (
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
    EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance;
    IpmiInstance = INSTANCE_FROM_EFI_SM_IPMI_BMC_THIS (This);
    DEBUG ((EFI_D_INFO, "IPMI_ERR(%X):%r\n", IpmiInstance->IpmiTransport.LastCompleteCode, Status));
  }
  DEBUG_CODE_END ();

  return Status;
}

EFI_STATUS
GetDeviceId (
  IN      EFI_IPMI_BMC_INSTANCE_DATA   *IpmiInstance,
  IN      EFI_STATUS_CODE_VALUE        StatusCodeValue[ ],
  IN OUT  UINT8                        *ErrorCount
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
  EFI_STATUS                Status;
  UINT8                     DataSize;
  EFI_SM_CTRL_INFO          *ControllerInfo;
  UINT64                    Tsc1, Tsc2, Tsc, WaitTsc, Tsc1s;


  Tsc1s   = DivU64x32(mIpmiInstance->KcsTimeoutPeriod, BMC_KCS_TIMEOUT);
  WaitTsc = MultU64x32(Tsc1s, PcdGet16(PcdWaitBmcReadyMaxSeconds));
  Tsc1 = AsmReadTsc();
  
  while(1) {
    //
    // Get the device ID information for the BMC.
    //
    DataSize = MAX_TEMP_DATA;
    Status = mIpmiInstance->IpmiTransport.SendIpmiCommand (
               &mIpmiInstance->IpmiTransport,
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
      gBS->Stall(1000000);

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

  DEBUG((EFI_D_INFO, "wait bmc ready %ld+(s)\n", DivU64x64Remainder(Tsc, Tsc1s, NULL)));

  //
  // If there is no error then proceed to check the data returned by the BMC
  //
  if (!EFI_ERROR (Status)) {
    ControllerInfo = (EFI_SM_CTRL_INFO *) mIpmiInstance->TempData;
    //
    // If the controller is in Update Mode and the maximum number of errors has not been exceeded, then
    // save the error code to the StatusCode array and increment the counter.  Set the BMC Status to indicate
    // the BMC is in force update mode.
    //
    if (ControllerInfo->UpdateMode != 0) {
      mIpmiInstance->BmcStatus = EFI_BMC_UPDATE_IN_PROGRESS;
      if (*ErrorCount < MAX_SOFT_COUNT) {
        StatusCodeValue[*ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_FORCE_UPDATE_MODE;
        (*ErrorCount)++;
      }
    }
  }

  return Status;
}


/**

  Execute the Get Self Test results command to determine whether or not the BMC self tests
  have passed

  @param mIpmiInstance   - Data structure describing BMC variables and used for sending commands
  @param StatusCodeValue - An array used to accumulate error codes for later reporting.
  @param ErrorCount      - Counter used to keep track of error codes in StatusCodeValue

  @retval EFI_SUCCESS       - BMC Self test results are retrieved and saved into BmcStatus
  @retval EFI_DEVICE_ERROR  - BMC failed to return self test results.

**/
EFI_STATUS
GetSelfTest (
  IN      EFI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance,
  IN      EFI_STATUS_CODE_VALUE       StatusCodeValue[  ],
  IN OUT  UINT8                       *ErrorCount
  )
{
  EFI_STATUS  Status;
  UINT8       DataSize;
  UINT8       Index;
  UINT8       *TempPtr;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  //
  // Get the SELF TEST Results.
  //
  DataSize = sizeof(mIpmiInstance->TempData);
  Status = mIpmiInstance->IpmiTransport.SendIpmiCommand (
             &mIpmiInstance->IpmiTransport,
             EFI_SM_NETFN_APP,
             0,
             EFI_APP_GET_SELFTEST_RESULTS,
             NULL,
             0,
             mIpmiInstance->TempData,
             &DataSize
             );
  //
  // If Status indicates a Device error, then the BMC is not responding, so send an error.
  //
  if (EFI_ERROR(Status)){
    DEBUG((DEBUG_ERROR, "\n[IPMI] BMC does not respond (%r)!\n\n", Status));
    if (*ErrorCount < MAX_SOFT_COUNT) {
      StatusCodeValue[*ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_COMM_ERROR;
      (*ErrorCount)++;
    }

    mIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
    return Status;
  } else {
    DEBUG((DEBUG_INFO, "[IPMI] BMC self-test result: %02X-%02X\n",
           mIpmiInstance->TempData[0], mIpmiInstance->TempData[1]));
    //
    // Copy the Self test results to Error Status.  Data will be copied as long as it
    // does not exceed the size of the ErrorStatus variable.
    //
    for (Index = 0, TempPtr = (UINT8 *) &mIpmiInstance->ErrorStatus;
         (Index < DataSize) && (Index < sizeof (mIpmiInstance->ErrorStatus));
         Index++, TempPtr++
        ) {
      *TempPtr = mIpmiInstance->TempData[Index];
    }
    //
    // Check the IPMI defined self test results.
    // Additional Cases are device specific test results.
    //
    switch (mIpmiInstance->TempData[0]) {
      case EFI_APP_SELFTEST_NO_ERROR:
      case EFI_APP_SELFTEST_NOT_IMPLEMENTED:
        mIpmiInstance->BmcStatus = EFI_BMC_OK;
        break;

      case EFI_APP_SELFTEST_ERROR:
        //
        // Three of the possible errors result in BMC hard failure; FRU Corruption,
        // BootBlock Firmware corruption, and Operational Firmware Corruption.  All
        // other errors are BMC soft failures.
        //
        if ((mIpmiInstance->TempData[1] & (EFI_APP_SELFTEST_FRU_CORRUPT | EFI_APP_SELFTEST_FW_BOOTBLOCK_CORRUPT | 
             EFI_APP_SELFTEST_FW_CORRUPT)) != 0) {
          mIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
        } else {
          mIpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
        }
        //
        //Test Only 
        //
        if(mIpmiInstance->TempData[1]==0x04){
          mIpmiInstance->BmcStatus = EFI_BMC_OK;          
        }
        
        //
        // Check if SDR repository is empty and report it if it is.
        //
        if ((mIpmiInstance->TempData[1] & EFI_APP_SELFTEST_SDR_REPOSITORY_EMPTY) != 0) {
          if (*ErrorCount < MAX_SOFT_COUNT) {
            StatusCodeValue[*ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SDR_EMPTY;
            (*ErrorCount)++;
          }
        }
        break;

      case EFI_APP_SELFTEST_FATAL_HW_ERROR:
        mIpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
        break;

      default:
        //
        // Call routine to check device specific failures.
        //
        break;
        // GetDeviceSpecificTestResults (mIpmiInstance);
    }

    if (mIpmiInstance->BmcStatus == EFI_BMC_HARDFAIL) {
      if (*ErrorCount < MAX_SOFT_COUNT) {
        StatusCodeValue[*ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_HARD_FAIL;
        (*ErrorCount)++;
      }
    } else if (mIpmiInstance->BmcStatus == EFI_BMC_SOFTFAIL) {
      if (*ErrorCount < MAX_SOFT_COUNT) {
        StatusCodeValue[*ErrorCount] = EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR | EFI_CU_FP_EC_SOFT_FAIL;
        (*ErrorCount)++;
      }
    }
  }

  return EFI_SUCCESS;
} // GetSelfTest()

