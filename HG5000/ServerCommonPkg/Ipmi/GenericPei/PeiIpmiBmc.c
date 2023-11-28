/** @file

Copyright (c) 2006 - 2019, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  PeiIpmiBmc.c

Abstract:
  Source file for the IPMI PEIM.

Revision History:

**/

#include "PeiIpmiBmc.h"

EFI_STATUS
UpdateErrorStatus (
  IN UINT8                      BmcError,
  PEI_IPMI_BMC_INSTANCE_DATA    *IpmiInstance
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
PeiIpmiSendCommandToBMC (
  IN      PEI_IPMI_TRANSPORT_PPI        *This,
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
  PEI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance;
  UINT8                       DataSize;
  EFI_STATUS                  Status;
  EFI_IPMI_COMMAND            *IpmiCommand;
  EFI_IPMI_RESPONSE           *IpmiResponse;

  IpmiInstance = INSTANCE_FROM_PEI_SM_IPMI_BMC_THIS (This);
 
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
  	IpmiInstance->KcsTimeoutPeriod,
    IpmiInstance->IpmiIoBase,
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
  	IpmiInstance->KcsTimeoutPeriod,
    IpmiInstance->IpmiIoBase,
    Context,
    (UINT8 *)IpmiResponse,
    &DataSize
    );

  if (Status != EFI_SUCCESS) {
    IpmiInstance->BmcStatus = EFI_BMC_SOFTFAIL;
    IpmiInstance->SoftErrorCount++;
    return Status;
  }

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
  IpmiInstance->BmcStatus = EFI_BMC_OK;
  return EFI_SUCCESS;
}


EFI_STATUS
PeiIpmiBmcStatus (
  IN  PEI_IPMI_TRANSPORT_PPI          *This,
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
  PEI_IPMI_BMC_INSTANCE_DATA  *IpmiInstance;

  IpmiInstance = INSTANCE_FROM_PEI_SM_IPMI_BMC_THIS (This);

  if ((IpmiInstance->BmcStatus == EFI_BMC_SOFTFAIL) && (IpmiInstance->SoftErrorCount >= MAX_SOFT_COUNT)) {
    IpmiInstance->BmcStatus = EFI_BMC_HARDFAIL;
  }

  *BmcStatus = IpmiInstance->BmcStatus;
  ComAddress->ChannelType = EfiSmBmc;
  ComAddress->Address.BmcAddress.LunAddress = 0x0;
  ComAddress->Address.BmcAddress.SlaveAddress = IpmiInstance->SlaveAddress;
  ComAddress->Address.BmcAddress.ChannelAddress = 0x0;

  return EFI_SUCCESS;
}