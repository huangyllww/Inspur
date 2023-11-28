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
#include <Protocol/IpmiInterfaceProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

IPMI_INTERFACE_PROTOCOL               *mIpmiInterface = NULL;
VOID                                  *mEfiIpmiProtocolNotifyReg;
EFI_EVENT                             mEfiIpmiProtocolEvent;

EFI_STATUS
NotifyIpmiTransportCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
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
  if (mIpmiInterface == NULL) {
    Status = gBS->LocateProtocol (
              &gByoIpmiInterfaceProtocolGuid,
              NULL,
              (VOID **) &mIpmiInterface
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
  if (mIpmiInterface == NULL) {
    Status = gBS->CreateEvent (EVT_NOTIFY_SIGNAL, TPL_CALLBACK, NotifyIpmiTransportCallback, NULL, &mEfiIpmiProtocolEvent);
    ASSERT_EFI_ERROR (Status);

    Status = gBS->RegisterProtocolNotify (&gByoIpmiInterfaceProtocolGuid, mEfiIpmiProtocolEvent, &mEfiIpmiProtocolNotifyReg);
    ASSERT_EFI_ERROR (Status);

    gBS->SignalEvent (mEfiIpmiProtocolEvent);
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
  EFI_STATUS        Status;
  IPMI_CMD_HEADER   Request;

  if (mIpmiInterface == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  Request.NetFn = NetFunction;
  Request.Cmd   = Command;
  Request.Lun   = IPMI_BMC_LUN;
  Status = mIpmiInterface->ExecuteIpmiCmd (
                             mIpmiInterface,
                             Request,
                             CommandData,
                             CommandDataSize,
                             ResponseData,
                             ResponseDataSize,
                             NULL
                             );
  return Status;


}


UINT8
EfiIGetLastIpmiCmdCompleteCode (
VOID
  )
{
  
  if (mIpmiInterface == NULL) {
    return 0;
  }

  return mIpmiInterface->GetLastCompleteCode(mIpmiInterface);
}
