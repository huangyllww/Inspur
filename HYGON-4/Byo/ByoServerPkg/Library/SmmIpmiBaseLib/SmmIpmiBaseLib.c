/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
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
#ifdef MM_IPMI_BASE_LIB
#include <Library/MmServicesTableLib.h>
#else
#include <Library/SmmServicesTableLib.h>
#endif
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

/**
  Routine to send commands to BMC.

  @param NetFunction       - Net function of the command
  @param Command           - IPMI Command
  @param CommandData       - Command Data
  @param CommandDataSize   - Size of CommandData
  @param ResponseData      - Response Data
  @param ResponseDataSize  - Response Data Size

  @retval EFI_SUCCESS:               Get successfully
  @retval EFI_NOT_AVAILABLE_YET

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
    #ifdef MM_IPMI_BASE_LIB
      Status = gMmst->MmLocateProtocol (
                          &gMmIpmiTransportProtocolGuid,
                          NULL,
                          (VOID **) &mIpmiTransport
                          );
    #else
      Status = gSmst->SmmLocateProtocol (
                        &gSmmIpmiTransportProtocolGuid,
                        NULL,
                        (VOID **) &mIpmiTransport
                        );
    #endif

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

  @param BmcStatus                   The ConnectAllComplete EFI Event.
  @param ComAddress                  Event context pass to create function

  @retval EFI_SUCCESS:               Get successfully
  @retval EFI_NOT_AVAILABLE_YET

**/
EFI_STATUS
EfiGetBmcStatus (
  OUT EFI_BMC_STATUS                       *BmcStatus,
  OUT EFI_SM_COM_ADDRESS                   *ComAddress
  )
{
  EFI_STATUS  Status;

  if (mIpmiTransport == NULL) {
    #ifdef MM_IPMI_BASE_LIB
      Status = gMmst->MmLocateProtocol (
                          &gMmIpmiTransportProtocolGuid,
                          NULL,
                          (VOID **) &mIpmiTransport
                          );
    #else
      Status = gSmst->SmmLocateProtocol (
                        &gSmmIpmiTransportProtocolGuid,
                        NULL,
                        (VOID **) &mIpmiTransport
                        );
    #endif
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

