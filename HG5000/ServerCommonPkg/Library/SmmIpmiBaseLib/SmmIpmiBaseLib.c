/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  WheaElog.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#include <PiDxe.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Library/IpmiBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>

STATIC EFI_IPMI_TRANSPORT *mIpmiTransport = NULL;
VOID                      *mEfiIpmiProtocolNotifyReg = NULL;
EFI_EVENT                 mEfiIpmiProtocolEvent;

EFI_STATUS
NotifyIpmiTransportCallback (
  IN CONST EFI_GUID                *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                        Handle
  )
/*++

Routine Description:

  Callback function for locating the IpmiTransport protocol

Arguments:

  Event                                       - Event on which the callback is called
  Context                                     - The context of the Callback

Returns:

  The return value from EfiLocateProtocolInterface function or EFI_SUCCESS

--*/
{
  EFI_STATUS  Status;
  Status = EFI_SUCCESS;
  if (mIpmiTransport == NULL) {
    Status = gSmst->SmmLocateProtocol (
                    &gSmmIpmiTransportProtocolGuid,
                    NULL,
                    (VOID **) &mIpmiTransport
                    );
  }

  return Status;
}

EFI_STATUS
EfiInitializeIpmiBase (
  VOID
  )
/*++

Routine Description:

  Initialize the global varible with the pointer of IpmiTransport Protocol

Arguments:

  None

Returns:

  EFI_SUCCESS - Always return success

--*/
{
  EFI_STATUS  Status;
  if (mIpmiTransport == NULL) {
    Status = gSmst->SmmLocateProtocol (
                    &gSmmIpmiTransportProtocolGuid,
                    NULL,
                    (VOID **) &mIpmiTransport
                    );
	  if(EFI_ERROR(Status))
	  {
        Status = gSmst->SmmRegisterProtocolNotify(
                     &gSmmIpmiTransportProtocolGuid, 
                     NotifyIpmiTransportCallback, 
                     &mEfiIpmiProtocolNotifyReg
                     );
	  }
    ASSERT_EFI_ERROR (Status);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EfiSendCommandToBMC (
  IN UINT8        NetFunction,
  IN UINT8        Command,
  IN UINT8        *CommandData,
  IN UINTN        CommandDataSize,
  OUT UINT8       *ResponseData,
  IN OUT UINT8    *ResponseDataSize
  )
/*++

Routine Description:

  Routine to send commands to BMC

Arguments:

  NetFunction       - Net function of the command
  Command           - IPMI Command
  CommandData       - Command Data
  CommandDataSize   - Size of CommandData
  ResponseData      - Response Data
  ResponseDataSize  - Response Data Size

Returns:

  EFI_NOT_AVAILABLE_YET - IpmiTransport Protocol is not installed yet

--*/
{
  EFI_STATUS  Status;

  if (mIpmiTransport == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  Status = mIpmiTransport->SendIpmiCommand (
                            mIpmiTransport,
                            NetFunction,
                            0,
                            Command,
                            CommandData,
                            CommandDataSize,
                            ResponseData,
                            ResponseDataSize
                            );
  return Status;
}

EFI_STATUS
EfiGetBmcStatus(
	OUT EFI_BMC_STATUS										   *BmcStatus,
	OUT EFI_SM_COM_ADDRESS									   *ComAddress
  )
/*++

Routine Description:

  Routine to send commands to BMC

Arguments:

  NetFunction       - Net function of the command
  Command           - IPMI Command
  CommandData       - Command Data
  CommandDataSize   - Size of CommandData
  ResponseData      - Response Data
  ResponseDataSize  - Response Data Size

Returns:

  EFI_NOT_AVAILABLE_YET - IpmiTransport Protocol is not installed yet

--*/
{
  EFI_STATUS  Status;

  if (mIpmiTransport == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  Status = mIpmiTransport->GetBmcStatus(
                                        mIpmiTransport,
  	                                    BmcStatus,
  	                                    ComAddress
  	                                   );
  return Status;
}

