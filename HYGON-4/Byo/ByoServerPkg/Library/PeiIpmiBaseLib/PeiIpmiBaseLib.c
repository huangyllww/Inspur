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
  A Library to support all BMC access via IPMI command during PEI Phase.

Revision History:

**/

#include <PiPei.h>
#include <Library/IpmiBaseLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/DebugLib.h>
#include <Ppi/IpmiTransportPpi.h>


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
  EFI_STATUS             Status;
  PEI_IPMI_TRANSPORT_PPI *IpmiTransport; 

  Status = PeiServicesLocatePpi (
              &gPeiIpmiTransportPpiGuid,
              0,
              NULL,
              (VOID **) &IpmiTransport
              );

  if (EFI_ERROR(Status) || IpmiTransport == NULL) {
    return 0;
  }

  return IpmiTransport->LastCompleteCode;
}


/**
  Routine to send commands to BMC.

  @param NetFunction       - Net function of the command
  @param Command           - IPMI Command
  @param CommandData       - Command Data
  @param CommandDataSize   - Size of CommandData
  @param ResponseData      - Response Data
  @param ResponseDataSize  - Response Data Size

  @return EFI_NOT_AVAILABLE_YET - IpmiTransport Protocol is not installed yet

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
  EFI_STATUS             Status;
  PEI_IPMI_TRANSPORT_PPI *IpmiTransport; 

  Status = PeiServicesLocatePpi (
              &gPeiIpmiTransportPpiGuid,
              0,
              NULL,
              (VOID **) &IpmiTransport
              );

  if (EFI_ERROR(Status) || IpmiTransport == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  Status = IpmiTransport->SendIpmiCommand (
                             IpmiTransport,
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
  Routine to get BMC Status
  @param[out]  BmcStatus    A pointer to the EFI_BMC_STATUS.
  @param[out]  ComAddress   Pointer to the EFI_SM_COM_ADDRESS.

  @retval EFI_SUCCESS            BMC status received successfully.
  @retval EFI_NOT_AVAILABLE_YET  IpmiTransport Protocol is not installed yet.
  @retval Other                  Failure.

**/
EFI_STATUS
EfiGetBmcStatus (
  OUT EFI_BMC_STATUS                         *BmcStatus,
  OUT EFI_SM_COM_ADDRESS                     *ComAddress
  )
{
  EFI_STATUS             Status;
  PEI_IPMI_TRANSPORT_PPI *IpmiTransport; 

  Status = PeiServicesLocatePpi (
              &gPeiIpmiTransportPpiGuid,
              0,
              NULL,
              (VOID **) &IpmiTransport
              );

  if (EFI_ERROR(Status) || IpmiTransport == NULL) {
    return EFI_NOT_AVAILABLE_YET;
  }

  Status = IpmiTransport->GetBmcStatus (
                             IpmiTransport,
                             BmcStatus,
                             ComAddress
                             );
  return Status;
}

