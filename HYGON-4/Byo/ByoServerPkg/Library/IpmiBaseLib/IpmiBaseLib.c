/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:


Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#include <PiDxe.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>

STATIC EFI_IPMI_TRANSPORT *mIpmiTransport = NULL;


EFI_STATUS
EfiInitializeIpmiBase (
  VOID
  )
/*++

Routine Description:

  Initialize the global variable with the pointer of IpmiTransport Protocol

Arguments:

  None

Returns:

  EFI_SUCCESS - Always return success

--*/
{
  return EFI_SUCCESS;
}


UINT8
EfiIGetLastIpmiCmdCompleteCode (
  VOID
  )
{
  EFI_STATUS  Status;

  if (mIpmiTransport == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiIpmiTransportProtocolGuid,
                    NULL,
                    (VOID **) &mIpmiTransport
                    );
        
    if (EFI_ERROR(Status) || mIpmiTransport == NULL) {
      return 0;
    }
  }

  return mIpmiTransport->LastCompleteCode;
}

/**
  Routine to send commands to BMC.
  @param[in]       NetFunction        Net function of the command
  @param[in]       Command            IPMI Command
  @param[in]       CommandData        Command Data
  @param[in]       CommandDataSize    Size of CommandData
  @param[out]      ResponseData       Response Data
  @param[in, out]  ResponseDataSize   Response Data Size

  @retval EFI_SUCCESS            Return Successfully.
  @retval EFI_NOT_AVAILABLE_YET  IpmiTransport Protocol is not installed yet
  @retval Other                  Failure.

**/
EFI_STATUS
EfiSendCommandToBMC (
  IN UINT8        NetFunction,
  IN UINT8        Command,
  IN UINT8        *CommandData,
  IN UINT8        CommandDataSize,
  OUT UINT8       *ResponseData,
  IN OUT UINT8    *ResponseDataSize
  )
{
  EFI_STATUS  Status;

  if (mIpmiTransport == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiIpmiTransportProtocolGuid,
                    NULL,
                    (VOID **) &mIpmiTransport
                    );

    if (EFI_ERROR(Status) || mIpmiTransport == NULL) {
      return EFI_NOT_AVAILABLE_YET;
    }
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

/**
  Routine to send commands to BMC.
  @param[out]  BmcStatus    A pointer to the EFI_BMC_STATUS.
  @param[out]  ComAddress   Pointer to the EFI_SM_COM_ADDRESS.

  @retval EFI_SUCCESS            Restart Successfully.
  @retval EFI_NOT_AVAILABLE_YET  IpmiTransport Protocol is not installed yet.
  @retval Other                  Failure.

**/
EFI_STATUS
EfiGetBmcStatus (
  OUT EFI_BMC_STATUS                         *BmcStatus,
  OUT EFI_SM_COM_ADDRESS                     *ComAddress
  )
{
  EFI_STATUS  Status;

  if (mIpmiTransport == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiIpmiTransportProtocolGuid,
                    NULL,
                    (VOID **) &mIpmiTransport
                    );

    if (EFI_ERROR(Status) || mIpmiTransport == NULL) {
      return EFI_NOT_AVAILABLE_YET;
    }
  }

  Status = mIpmiTransport->GetBmcStatus (
                             mIpmiTransport,
                             BmcStatus,
                             ComAddress
                             );
  return Status;
}

