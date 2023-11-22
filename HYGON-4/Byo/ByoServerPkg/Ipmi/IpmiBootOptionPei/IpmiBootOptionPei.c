/** @file

Copyright (c) 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiBootOptionPei.c

Abstract:
  Source file for the IPMI PEIM.

Revision History:

**/


#include "IpmiBootOptionPei.h"


EFI_STATUS
IpmiWaitSetInProgressClear (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT8                             Buffer[IPMI_BUFFER_SIZE];
  UINT8                             ResponseSize;
  UINT32                            Retries;
  GET_BOOT_OPTIONS_REQUEST          *GetRequest;
  GET_BOOT_OPTIONS_RESPONSE         *GetResponse;
  BOOT_OPTIONS_RESPONSE_PARAMETER_0 *Parm0;

  Retries = 10;
  ZeroMem (Buffer, IPMI_BUFFER_SIZE);
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

    MicroSecondDelay(IPMI_STALL);
  } while (Retries-- > 0);

  if (Retries == 0) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiGetBootOptions (
  IN  BOOT_OPTION_PARAMETER_TYPE      ParameterType,
  OUT BOOT_OPTIONS_PARAMETERS         *ResponseParameters
  )
{
  EFI_STATUS                Status;
  UINT8                     Buffer[IPMI_BUFFER_SIZE];
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

  ZeroMem (Buffer, IPMI_BUFFER_SIZE);
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

  DEBUG_CODE(
    UINTN Index;
    UINT8 *ResponseBuf;

    ResponseBuf = (UINT8 *)GetResponse;
    DEBUG((DEBUG_INFO, "Boot Option Response: "));
    for (Index = 0; Index < ResponseSize; Index++) {
      DEBUG((DEBUG_INFO, "0x%x ", ResponseBuf[Index]));
    }
    DEBUG((DEBUG_INFO, "\n"));
  );

  CopyMem (ResponseParameters, GetResponse->ParameterData, ResponseSize);

  return EFI_SUCCESS;
}


EFI_STATUS
GetForceBootOptions(
  OUT IPMI_FORCE_BOOT_OPTIONS *IpmiForceBootOptions
)
{
  EFI_STATUS                Status = EFI_SUCCESS;
  BOOT_OPTIONS_PARAMETERS   BootOptParms;

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
    IpmiForceBootOptions->BiosBootType = BootOptParms.Parm5.BiosBootType;

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
    // Persistent Options handling is deferred to IpmiBootOptionDxe
    //
    DEBUG((EFI_D_INFO," PersistentOptions - %x \n",BootOptParms.Parm5.PersistentOptions));
  }
  return Status;
}


EFI_STATUS
ProcessIpmiBootFlags (
  VOID
  )
{
  EFI_STATUS               Status;
  IPMI_FORCE_BOOT_OPTIONS  IpmiForceBootOptions;
  IPMI_FORCE_BOOT_OPTIONS  *IpmiForceBootOptionsHobDataPtr;

  DEBUG((EFI_D_INFO,"%a() \n",__FUNCTION__));

  //
  // Check IPMI Force Boot.
  //
  Status = GetForceBootOptions (&IpmiForceBootOptions);
  DEBUG((EFI_D_INFO,"GetForceBootOptions BootValid:%x  BootDevice:%x\n", \
                    IpmiForceBootOptions.BootValid, IpmiForceBootOptions.BootDevice));
  
  //
  // Save IPMI Boot Option into HOB to avoid unnecessary IPMI Read in IpmiBootOption DXE
  //
  if (!EFI_ERROR(Status)) {
    IpmiForceBootOptionsHobDataPtr = BuildGuidHob(&gIpmiForceBootOptionHobGuid, sizeof(IPMI_FORCE_BOOT_OPTIONS));
    ASSERT (IpmiForceBootOptionsHobDataPtr != NULL);

    CopyMem(IpmiForceBootOptionsHobDataPtr, &IpmiForceBootOptions, sizeof(IPMI_FORCE_BOOT_OPTIONS));
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
IpmiSetBootTypeCallback( 
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  UINT8          IpmiBootMiscCfg;
  UINTN          Size;
  EFI_STATUS     Status;
  IPMI_FORCE_BOOT_OPTIONS *IpmiForceBootOptions;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *Var2Ppi;
  EFI_HOB_GUID_TYPE        *GuidHob;

  DEBUG((EFI_D_INFO,"%a()\n",__FUNCTION__));

  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Var2Ppi
             );
  Size = sizeof(UINT8);
  Status = Var2Ppi->GetVariable (
                      Var2Ppi,
                      L"IpmiBootMiscConfig",
                      &gByoIpmiBootMiscCfgVariableGuid,
                      NULL,
                      &Size,
                      &IpmiBootMiscCfg
                      );
  if(Status != EFI_SUCCESS){
    IpmiBootMiscCfg = 0;
  }
  GuidHob = GetFirstGuidHob(&gIpmiForceBootOptionHobGuid);
  if (GuidHob == NULL || GET_GUID_HOB_DATA_SIZE (GuidHob) != sizeof(IPMI_FORCE_BOOT_OPTIONS)) {
    PcdSet8S(PcdIpmiBootMiscConfig, 0);
    return EFI_UNSUPPORTED;
  } 
  IpmiForceBootOptions = GET_GUID_HOB_DATA (GuidHob);

  if (!IpmiForceBootOptions->BootValid) {
    PcdSet8S(PcdIpmiBootMiscConfig, IpmiBootMiscCfg);
    return EFI_SUCCESS;
  }

  IpmiBootMiscCfg &= ~BIT4;//Need clear bit4 before save data

  if(IpmiForceBootOptions->PersistentOptions == 0){
    IpmiBootMiscCfg |= BIT2;//Enable backup last BiosBootType
  } else {
    IpmiBootMiscCfg &= ~BIT5;//No need for backup ,when set to permanent
    if(IpmiForceBootOptions->BootDevice == 0x6){
      IpmiBootMiscCfg |= BIT4;//Force boot setup persistent
    }
  }

  if (IpmiForceBootOptions->BiosBootType == 1) {
    IpmiBootMiscCfg |= (IpmiBootMiscCfg & ~(BIT0|BIT1)) | 1 | BIT3;//1:BIOS_BOOT_UEFI_OS
  } else if (IpmiForceBootOptions->BiosBootType == 0) {
    IpmiBootMiscCfg |= (IpmiBootMiscCfg & ~(BIT0|BIT1)) | 2 | BIT3;//2:BIOS_BOOT_LEGACY_OS
  }

  PcdSet8S(PcdIpmiBootMiscConfig, IpmiBootMiscCfg);
  return EFI_SUCCESS;
}

STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gPpiNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
    &gEfiPeiReadOnlyVariable2PpiGuid,
    IpmiSetBootTypeCallback
  },
};


EFI_STATUS
EFIAPI
IpmiBootOptionEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = ProcessIpmiBootFlags();
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "ProcessIpmiBootFlags:%r\n", Status));
  }
  Status = PeiServicesNotifyPpi(gPpiNotifyList);

  return Status;
}
