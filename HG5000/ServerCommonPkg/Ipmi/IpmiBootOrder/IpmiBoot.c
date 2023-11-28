/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiBoot.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/


#include "IpmiBoot.h"


#define BUFFER_SIZE 50

IPMI_BOOT_PROTOCOL        mIpmiBootProtocol;
IPMI_FORCE_BOOT_OPTIONS   mIpmiForceBootOptions;

EFI_STATUS
IpmiWaitSetInProgressClear (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT8                             Buffer[BUFFER_SIZE];
  UINT8                             ResponseSize;
  UINT32                            Retries;
  GET_BOOT_OPTIONS_REQUEST          *GetRequest;
  GET_BOOT_OPTIONS_RESPONSE         *GetResponse;
  BOOT_OPTIONS_RESPONSE_PARAMETER_0 *Parm0;

  Retries = 10;
  ZeroMem (Buffer, BUFFER_SIZE);
  GetRequest = (GET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  GetRequest->ParameterSelector = 0; // Set in progress parameter
  GetRequest->SetSelector   = 0;
  GetRequest->BlockSelector = 0;
  GetResponse = (GET_BOOT_OPTIONS_RESPONSE *) (Buffer + sizeof (GET_BOOT_OPTIONS_REQUEST));
  ResponseSize = sizeof (GET_BOOT_OPTIONS_RESPONSE);

  //
  // Wait for in-progress bit to clear
  //
  do {
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_CHASSIS,
              EFI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
              (UINT8 *) GetRequest,
              sizeof (GET_BOOT_OPTIONS_REQUEST),
              (UINT8 *) GetResponse,
              &ResponseSize
              );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Parm0 = (BOOT_OPTIONS_RESPONSE_PARAMETER_0 *) GetResponse->ParameterData;
    if (Parm0->SetInProgress == 0) {
      break;
    }

    gBS->Stall (IPMI_STALL);
  } while (Retries-- > 0);

  if (Retries == 0) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiSetBootOptions (
  IN  BOOT_OPTION_PARAMETER_TYPE       ParameterType,
  OUT BOOT_OPTIONS_PARAMETERS          *RequestParameters
  )
{
  EFI_STATUS                Status;
  UINT8                     Buffer[BUFFER_SIZE];
  UINT8                     ResponseSize;
  SET_BOOT_OPTIONS_REQUEST  *SetRequest;
  SET_BOOT_OPTIONS_RESPONSE SetResponse;
  UINT8                     RequestSize;

  //
  // Wait for Set In Progress to clear
  //
  Status = IpmiWaitSetInProgressClear ();
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (Buffer, BUFFER_SIZE);
  SetRequest                        = (SET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  SetRequest->ParameterSelector     = (UINT8) ParameterType;
  SetRequest->MarkParameterInvalid  = 0;

  CopyMem (
    SetRequest->ParameterData,
    RequestParameters,
    sizeof (BOOT_OPTIONS_PARAMETERS)
    );

  ResponseSize = sizeof (SET_BOOT_OPTIONS_RESPONSE);
  if (ParameterType == ParmTypeBootFlags) {
    //
    // BMC FW expects all the 5 bytes of the Boot option param 5 to be passed. If only the Valid bit field
    // is passed, the command will return an Error
    //
    RequestSize = sizeof (SET_BOOT_OPTIONS_REQUEST) + 4;  
  } else {
    RequestSize = sizeof (SET_BOOT_OPTIONS_REQUEST);
  }

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_CHASSIS,
            EFI_CHASSIS_SET_SYSTEM_BOOT_OPTIONS,
            (UINT8 *) SetRequest,
            RequestSize,
            (UINT8 *) &SetResponse,
            &ResponseSize
            );

  return Status;
}

EFI_STATUS
IpmiGetBootOptions (
  IN  BOOT_OPTION_PARAMETER_TYPE      ParameterType,
  OUT BOOT_OPTIONS_PARAMETERS         *ResponseParameters
  )
{
  EFI_STATUS                Status;
  UINT8                     Buffer[BUFFER_SIZE];
  UINT8                     ResponseSize;
  GET_BOOT_OPTIONS_REQUEST  *GetRequest;
  GET_BOOT_OPTIONS_RESPONSE *GetResponse;

  //
  // Wait for Set In Progress to clear
  //
  Status = IpmiWaitSetInProgressClear ();
  DEBUG((EFI_D_INFO,"IpmiWaitSetInProgressClear - %r\n ",Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (Buffer, BUFFER_SIZE);
  GetRequest                    = (GET_BOOT_OPTIONS_REQUEST *) &Buffer[0];
  GetRequest->ParameterSelector = (UINT8) ParameterType;
  GetRequest->SetSelector       = 0;
  GetRequest->BlockSelector     = 0;

  GetResponse = (GET_BOOT_OPTIONS_RESPONSE *) (Buffer + sizeof (GET_BOOT_OPTIONS_REQUEST));
  ResponseSize = sizeof (BOOT_OPTIONS_PARAMETERS) + sizeof (GET_BOOT_OPTIONS_RESPONSE);
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_CHASSIS,
            EFI_CHASSIS_GET_SYSTEM_BOOT_OPTIONS,
            (UINT8 *) GetRequest,
            sizeof (GET_BOOT_OPTIONS_REQUEST),
            (UINT8 *) GetResponse,
            &ResponseSize
            );

  //
  // What error to check for
  //
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CopyMem (ResponseParameters, GetResponse->ParameterData, ResponseSize);

  return EFI_SUCCESS;
}


EFI_STATUS
GetForceBootOptions(
  OUT IPMI_FORCE_BOOT_OPTIONS *IpmiForceBootOptions
)
{
  static int                DoneOnce = FALSE;
  EFI_STATUS                Status = EFI_SUCCESS;
  BOOT_OPTIONS_PARAMETERS   BootOptParms;

  DEBUG((EFI_D_INFO, "DoneOnce =%x \n",DoneOnce));
  if (DoneOnce) {
    IpmiForceBootOptions = &mIpmiForceBootOptions;
    return EFI_SUCCESS;
  }

  //
  // Read Boot Flags and check if  "Boot flag valid" is set
  //
  ZeroMem (IpmiForceBootOptions, sizeof (IPMI_FORCE_BOOT_OPTIONS));
  Status = IpmiGetBootOptions (ParmTypeBootFlags, &BootOptParms);
  DEBUG((EFI_D_INFO," IpmiGetBootOptions - %r \n",Status));
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IpmiForceBootOptions->BootValid = BootOptParms.Parm5.BootFlagValid;
  DEBUG((EFI_D_INFO," BootValid - %x \n",BootOptParms.Parm5.BootFlagValid));
  if (BootOptParms.Parm5.BootFlagValid) {
    //
    // Fill out secure boot,
    //
    IpmiForceBootOptions->SecureMode.s.BlankVideo = BootOptParms.Parm5.ScreenBlank;
    IpmiForceBootOptions->SecureMode.s.WriteProtectDiskette = 0;
    IpmiForceBootOptions->SecureMode.s.LockNmiButton = 0;
    IpmiForceBootOptions->SecureMode.s.LockSleepButton = BootOptParms.Parm5.LockSleep;
    IpmiForceBootOptions->SecureMode.s.LockResetButton = BootOptParms.Parm5.LockReset;
    IpmiForceBootOptions->SecureMode.s.LockPowerButton = BootOptParms.Parm5.LockPower;
    IpmiForceBootOptions->SecureMode.s.LockFrontPanel = 0;

    //
    // CMOS clear, console redir flags ... here
    //
    IpmiForceBootOptions->CmosClear = BootOptParms.Parm5.CmosClear;
    DEBUG((EFI_D_INFO," CmosClear - %x \n",BootOptParms.Parm5.CmosClear));

    //
    // Redir PW Bypass BiosVerbosity done in ConsoleAvailableEntry()
    //
    IpmiForceBootOptions->UserPasswordBypass = BootOptParms.Parm5.UserPasswordBypass;
    IpmiForceBootOptions->BiosVerbosity = BootOptParms.Parm5.BiosVerbosity;

    //
    // TBD: how to do these??
    //
    IpmiForceBootOptions->ConsoleRedirection = BootOptParms.Parm5.ConsoleRedirection;
    IpmiForceBootOptions->LockKeyboard = BootOptParms.Parm5.LockKeyboard;

    //
    // Get the Boot device Selector
    //

    IpmiForceBootOptions->BootDevice = BootOptParms.Parm5.BootDeviceSelector;
    IpmiForceBootOptions->PersistentOptions = BootOptParms.Parm5.PersistentOptions;

    //
    // TBD: Get mail box data as OEM parameters
    //

    //
    // Get service partition Selector
    //
    if (IpmiForceBootOptions->BootDevice == ForceBootServicePartition) {
      Status = IpmiGetBootOptions (ParmTypeServicePartitionSelector, &BootOptParms);
      if (EFI_ERROR (Status)) {
        return Status;
      }

      IpmiForceBootOptions->ServicePartitionSelector = BootOptParms.Parm1.ServicePartitionSelector;
    }

    //
    // TBD: Handle service partition scan request
    //

    //
    // Here we need to check if the option apply for next boot only, and if yes, clear the Valid bit
    //
    DEBUG((EFI_D_INFO," PersistentOptions - %x \n",BootOptParms.Parm5.PersistentOptions));

    if (BootOptParms.Parm5.PersistentOptions == 0) {
      ZeroMem (&BootOptParms, sizeof (BootOptParms));
      BootOptParms.Parm5.BootFlagValid = 0;
      Status = IpmiSetBootOptions(ParmTypeBootFlags, &BootOptParms);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    //
    // Write to Boot Info Acknowlege "BIOS/POST has handled boot info"
    //
    if (BootOptParms.Parm4.WriteMask & BOOT_OPTION_HANDLED_BY_BIOS) {
      ZeroMem (&BootOptParms, sizeof (BootOptParms));
      BootOptParms.Parm4.BootInitiatorAcknowledgeData = BOOT_OPTION_HANDLED_BY_BIOS;
      Status = IpmiSetBootOptions(ParmTypeBootInfoAck, &BootOptParms);
    }
  }
  DoneOnce = TRUE;
  return Status;
}


EFI_STATUS
ProcessIpmiBootFlags (
  IN OUT    UINT8  *BootType
  )
{
  DEBUG((EFI_D_INFO,"%a() \n",__FUNCTION__));
  //
  // Check if IPMI Force Boot.
  //
  GetForceBootOptions (&mIpmiForceBootOptions);
  DEBUG((EFI_D_INFO,"GetForceBootOptions BootValid:%x  BootDevice:%x\n",mIpmiForceBootOptions.BootValid,mIpmiForceBootOptions.BootDevice));

  if (mIpmiForceBootOptions.BootValid == 0) {
    return EFI_NOT_FOUND;
  }

  *BootType = (UINT8)mIpmiForceBootOptions.BootDevice;

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiBootEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++

  Routine Description:
    This is the standard EFI driver entry point. This function installs
    the IpmiBoot protocol.

  Arguments:
    ImageHandle     - Handle for the image of this driver
    SystemTable     - Pointer to the EFI System Table

  Returns:
    EFI_SUCCESS     - Protocol successfully started and installed

--*/
{
  EFI_STATUS  Status;
  EFI_HANDLE  NewHandle;

  NewHandle = NULL;
  EfiInitializeIpmiBase ();

  mIpmiBootProtocol.ProcessBootFlags = ProcessIpmiBootFlags;

  Status = gBS->InstallProtocolInterface (
                &NewHandle,
                &gEfiIpmiBootGuid,
                EFI_NATIVE_INTERFACE,
                &mIpmiBootProtocol
                );
  DEBUG ((EFI_D_INFO, "[IpmiBoot] Install gEfiIpmiBootGuid Protocol\n"));

  return Status;
}

