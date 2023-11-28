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

#include "SolStatus.h"

EFI_SOL_STATUS_PROTOCOL        mIpmiSolStatus;
#define SOL_CHANNEL 0x1

BOOLEAN
GetSolStatus (
  IN EFI_SOL_STATUS_PROTOCOL                   *This,
  IN OUT UINT8                                 *SolBaudRate
  )
/*++

Routine Description:

    This routine reads the data from BMC and check whether SOL enabled and also find out the proper channel no

Arguments:
    SolBaudRate   - Serial over lan baud rate

Returns:
    TRUE  SOL enabled.
    FALSE SOL disabled.

--*/
{
  EFI_STATUS  Status;
  UINT8       IpmiCommand[8];
  UINT8       SolData[4];
  UINT8       SolDataSize;

  ZeroMem (&IpmiCommand[0], 8);
  IpmiCommand[0] = SOL_CHANNEL;     // look up session info according to the session id
  IpmiCommand[1] = 0x05;            // Parameter Selector - set SOL non-volatile bit rate
  IpmiCommand[2] = 0x00;
  IpmiCommand[3] = 0x00;  
  ZeroMem (SolData, 4);
  SolDataSize = 2;
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_TRANSPORT,
            EFI_TRANSPORT_GET_SOL_CONFIG_PARAM,
            &IpmiCommand[0],
            4,
            &SolData[0],
            &SolDataSize
            );
  if ((EFI_ERROR (Status))) {
    SolData[1] = 0xA;
  } else {
    *SolBaudRate = SolData[1];
  }

  //
  // Check SOL 2.0 status/payload and Get Payload Activation Status
  //
  ZeroMem (&IpmiCommand[0], 8);
  IpmiCommand[0] = 0x01;                // No. of SOL payload type. 
  SolDataSize    = 3;
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_APP,
             EFI_APP_GET_PAYLOAD_ACTIVATION_STATUS,
             &IpmiCommand[0],
             1,
             &SolData[0],
             &SolDataSize
             );
  if (Status != EFI_SUCCESS) {
    return FALSE;
  }
  DEBUG((EFI_D_INFO, "SOL: EFI_APP_GET_PAYLOAD_ACTIVATION_STATUS:%d %d %d.\n", SolData[0], SolData[1], SolData[2]));
  if ((SolData[1] & 1) == 1) {
    return TRUE;
  } else {
    return FALSE;
  }

}

EFI_STATUS
SetSolEnable (
  BOOLEAN               Enable
  )
{
  EFI_STATUS  Status;
  UINT8       IpmiCommand[18];
  UINT8       SolData[4];
  UINT8       SolDataSize;
  INTN        RetryCount = 3;

  Status = EFI_SUCCESS;
  RetryCount = 3;
  while (RetryCount-- > 0) {

    // EFI_TRANSPORT_SET_SOL_CONFIG_PARAM to enable/disable sol.
    ZeroMem (&IpmiCommand[0], 18);
    IpmiCommand[0] = SOL_CHANNEL;
    IpmiCommand[1] = 0;           //Set in progress
    IpmiCommand[2] = 1;
    SolDataSize    = 0;  
    EfiSendCommandToBMC (
      EFI_SM_NETFN_TRANSPORT,
      EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
      &IpmiCommand[0],
      3,
      &SolData[0],
      &SolDataSize
      );
    ZeroMem (&IpmiCommand[0], 18);
    IpmiCommand[0] = SOL_CHANNEL;
    IpmiCommand[1] = 1;
    if (Enable) {
      IpmiCommand[2] = 1 & 0x1;
    } else {
      IpmiCommand[2] = 0 & 0x1;
    }
    SolDataSize = 0;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_TRANSPORT,
              EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
              &IpmiCommand[0],
              3,
              &SolData[0],
              &SolDataSize
              );
    if (Status != EFI_SUCCESS) {
      DEBUG((EFI_D_INFO, "Fail to %sable SOL. %r\n", Enable ? L"en" : L"dis", Status));
    }
//    ZeroMem (&IpmiCommand[0], 18);
//    IpmiCommand[0] = SOL_CHANNEL;
//    IpmiCommand[1] = 0;
//    IpmiCommand[2] = 2;
//    SolDataSize    = 0;
//    EfiSendCommandToBMC (
//      EFI_SM_NETFN_TRANSPORT,
//      EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
//      &IpmiCommand[0],
//      3,
//      &SolData[0],
//      &SolDataSize
//      );
    ZeroMem (&IpmiCommand[0], 18);
    IpmiCommand[0] = SOL_CHANNEL;
    IpmiCommand[1] = 0;//Set complete
    IpmiCommand[2] = 0;
    SolDataSize    = 0;
    EfiSendCommandToBMC (
      EFI_SM_NETFN_TRANSPORT,
      EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
      &IpmiCommand[0],
      3,
      &SolData[0],
      &SolDataSize
      );
    if (Status == EFI_SUCCESS) break;
  }

	RetryCount = 3;

	while (RetryCount-- > 0) {
  
	  // EFI_TRANSPORT_SET_SOL_CONFIG_PARAM to enable/disable sol.
	  ZeroMem (&IpmiCommand[0], 18);
	  IpmiCommand[0] = 8;
	  IpmiCommand[1] = 0;			//Set in progress
	  IpmiCommand[2] = 1;
	  SolDataSize	 = 0;  
	  EfiSendCommandToBMC (
		EFI_SM_NETFN_TRANSPORT,
		EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
		&IpmiCommand[0],
		3,
		&SolData[0],
		&SolDataSize
		);
	  ZeroMem (&IpmiCommand[0], 18);
	  IpmiCommand[0] = 8;
	  IpmiCommand[1] = 1;
	  if (Enable) {
		IpmiCommand[2] = 1 & 0x1;
	  } else {
		IpmiCommand[2] = 0 & 0x1;
	  }
	  SolDataSize = 0;
	  Status = EfiSendCommandToBMC (
				EFI_SM_NETFN_TRANSPORT,
				EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
				&IpmiCommand[0],
				3,
				&SolData[0],
				&SolDataSize
				);
	  if (Status != EFI_SUCCESS) {
		DEBUG((EFI_D_INFO, "Fail to %sable SOL. %r\n", Enable ? L"en" : L"dis", Status));
	  }
  //	ZeroMem (&IpmiCommand[0], 18);
  //	IpmiCommand[0] = SOL_CHANNEL;
  //	IpmiCommand[1] = 0;
  //	IpmiCommand[2] = 2;
  //	SolDataSize    = 0;
  //	EfiSendCommandToBMC (
  //	  EFI_SM_NETFN_TRANSPORT,
  //	  EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
  //	  &IpmiCommand[0],
  //	  3,
  //	  &SolData[0],
  //	  &SolDataSize
  //	  );
	  ZeroMem (&IpmiCommand[0], 18);
	  IpmiCommand[0] = 8;
	  IpmiCommand[1] = 0;//Set complete
	  IpmiCommand[2] = 0;
	  SolDataSize	 = 0;
	  EfiSendCommandToBMC (
		EFI_SM_NETFN_TRANSPORT,
		EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
		&IpmiCommand[0],
		3,
		&SolData[0],
		&SolDataSize
		);
	  if (Status == EFI_SUCCESS) break;
	}

  return Status;
}

EFI_STATUS
EfiSetSolBaudRate (
  IN EFI_SOL_STATUS_PROTOCOL            *This,
  IN UINT8                              BiosBaudRate
  )
/*++

  Routine Description:
    This sets the SOL Baud Rate

  Arguments:
    This         - This pointer
    BiosBaudRate - current BIOS Baud rate setting

  Returns:
    EFI_SUCCESS
--*/
{
  EFI_STATUS  Status;
  UINT8       BaudRate;
  UINT8       IpmiCommand[8];
  UINT8       SolData[4];
  UINT8       SolDataSize;
  UINT8       BaudRateXlat[] = { SOL_BAUD_9600, SOL_BAUD_19200, SOL_BAUD_38400, SOL_BAUD_57600, SOL_BAUD_115200 };

  BaudRate = BaudRateXlat[BiosBaudRate];
  if (BaudRate != This->SolBaudRate) {
    ZeroMem (&IpmiCommand[0], 8);
    IpmiCommand[0] = SOL_CHANNEL;
    IpmiCommand[1] = 0;
    IpmiCommand[2] = 1;
    ZeroMem (SolData, 4);
    SolDataSize = 1;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_TRANSPORT,
              EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
              &IpmiCommand[0],
              3,
              &SolData[0],
              &SolDataSize
              );
    if ((EFI_ERROR (Status))) {
      return Status;
    }
    ZeroMem (&IpmiCommand[0], 8);
    IpmiCommand[0] = SOL_CHANNEL;       // look up session info according to the session id
    IpmiCommand[1] = 0x05;              // Parameter Selector - set SOL non-volatile bit rate
    IpmiCommand[2] = BaudRate;
    ZeroMem (SolData, 4);
    SolDataSize = 1;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_TRANSPORT,
              EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
              &IpmiCommand[0],
              3,
              &SolData[0],
              &SolDataSize
              );
    if ((EFI_ERROR (Status))) {
      return Status;
    }
    ZeroMem (&IpmiCommand[0], 8);
    IpmiCommand[0] = SOL_CHANNEL;       // look up session info according to the session id
    IpmiCommand[1] = 0x06;              // Parameter Selector - set SOL volatile bit rate
    IpmiCommand[2] = BaudRate;
    ZeroMem (SolData, 4);
    SolDataSize = 1;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_TRANSPORT,
              EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
              &IpmiCommand[0],
              3,
              &SolData[0],
              &SolDataSize
              );
    if (!(EFI_ERROR (Status))) {
      This->SolBaudRate = BaudRate;
    }
    ZeroMem (&IpmiCommand[0], 8);
    IpmiCommand[0] = SOL_CHANNEL;
    IpmiCommand[1] = 0;
    IpmiCommand[2] = 0;
    ZeroMem (SolData, 4);
    SolDataSize = 1;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_TRANSPORT,
              EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
              &IpmiCommand[0],
              3,
              &SolData[0],
              &SolDataSize
              );
    if ((EFI_ERROR (Status))) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

VOID
SolReadyToBootCallback (
  IN  EFI_EVENT                Event,
  IN  VOID                     *Context
)
{
  EFI_HANDLE                        *HandleBuffer;
  UINTN                             Index;
  UINTN                             HandleCount;
  EFI_STATUS                        Status;
  DEBUG((EFI_D_ERROR, __FUNCTION__"(), PcdUCREnable :0x%x.\n", PcdGet8 (PcdUCREnable)));

  if (PcdGet8 (PcdUCREnable)) {
     PcdSet32 (PcdConOutColumn, 80);
     PcdSet32 (PcdConOutRow, 25);
     Status = gBS->LocateHandleBuffer (
                     ByProtocol,
                     &gEfiSimpleTextOutProtocolGuid,
                     NULL,
                     &HandleCount,
                     &HandleBuffer
                     );
    if (!EFI_ERROR (Status)) {
       for (Index = 0; Index < HandleCount; Index++) {
          gBS->DisconnectController (HandleBuffer[Index], NULL, NULL);
       }
       for (Index = 0; Index < HandleCount; Index++) {
          gBS->ConnectController (HandleBuffer[Index], NULL, NULL, TRUE);
       }
       if (HandleBuffer != NULL) {
         FreePool (HandleBuffer);
       }
    }
  }

  gBS->CloseEvent (Event);
}


EFI_STATUS
SolStatusEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++
  
  Routine Description:
    This is the standard EFI driver point. This function intitializes
    the private data required for creating SOL Status Driver.
    
  Arguments:
    ImageHandle     - Handle for the image of this driver
    SystemTable     - Pointer to the EFI System Table

  Returns:
    EFI_SUCCESS     - Protocol successfully started and installed
    EFI_UNSUPPORTED - Protocol can't be started

--*/
{
  EFI_STATUS    Status;
  UINT8         SolBaudRate;
  EFI_HANDLE    Handle;
  BOOLEAN       SolStatus;
  UINTN         VarSize;
  BMC_SETUP_DATA  SetupData;
  EFI_GUID        SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_EVENT           Event;

  SolStatus   = FALSE;
  SolBaudRate = IPMI_SOL_BAUD_115200;

  EfiInitializeIpmiBase ();

  //
  // Get the Setup variable for the console redir settings.
  //
  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &SetupData
                  );
  DEBUG((EFI_D_INFO, "SetupData.SerialOverLanEnable = %d, Status = %r.\n", SetupData.SerialOverLanEnable, Status));
  //
  // Make sure SOL enable variable is initialized to disabled.
  //
  SolStatus = GetSolStatus (&mIpmiSolStatus, &SolBaudRate);
  if (SetupData.SerialOverLanEnable) {
    SetSolEnable (TRUE);

    Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  SolReadyToBootCallback,
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &Event
                  );
    DEBUG((EFI_D_ERROR, __FUNCTION__"(), Create SolReadyToBootCallback :%r.\n", Status ));
  } else {
    SetSolEnable (FALSE);   
  }

  //
  // Install SOL status protocol
  //
  mIpmiSolStatus.IpmiSolStatus  = SolStatus;
  mIpmiSolStatus.SolBaudRate    = SolBaudRate;
  mIpmiSolStatus.GetSolStatus   = GetSolStatus;
  mIpmiSolStatus.SetSolBaudRate = EfiSetSolBaudRate;
  Handle                        = NULL;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiIpmiSolStatusProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mIpmiSolStatus
                  );

  return EFI_SUCCESS;
}
