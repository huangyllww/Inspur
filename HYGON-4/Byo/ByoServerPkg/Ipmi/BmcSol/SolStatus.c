/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SolStatus.c

Abstract:
  Source file for the Sol driver.

Revision History:

**/

#include "SolStatus.h"

EFI_SOL_STATUS_PROTOCOL        mIpmiSolStatus;
EFI_GUID gBmcSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

BOOLEAN
EFIAPI
GetSolStatus (
  IN EFI_SOL_STATUS_PROTOCOL                   *This,
  IN UINT8                                     Channel,
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
  IpmiCommand[0] = Channel;     // look up session info according to the session id
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
  IpmiCommand[0] = Channel;     // look up session info according to the session id
  IpmiCommand[1] = 0x01;        // Parameter Selector - SOL Status
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
  if (Status != EFI_SUCCESS) {
    return FALSE;
  }
  DEBUG((EFI_D_INFO, "SOL Status: :%d.\n", SolData[1]));
  if ((SolData[1] & 1) == 1) {
    return TRUE;
  } else {
    return FALSE;
  }

}

EFI_STATUS
EFIAPI
SetSolEnable (
  IN EFI_SOL_STATUS_PROTOCOL                   *This,
  BOOLEAN                                      Enable
  )
{
  EFI_STATUS  Status;
  UINT8       IpmiCommand[18];
  UINT8       SolData[4];
  UINT8       SolDataSize;
  INTN        Index;
  UINT8       BmcChannel[2] = {BASE_BOARD_NIC_CHANNEL_SEL,SHARED_NIC_CHANNEL_SEL};
  UINT8       BmcChannelCount = 1;

  if (PcdGet8(PcdBmcOnboardLanChannelEnable) == FALSE) {
    BmcChannel[0] = SHARED_NIC_CHANNEL_SEL;
  }else{
     if (PcdGet8(PcdBmcShareLanChannelEnable)){
       BmcChannelCount = 2;
     }
  }

  Status = EFI_SUCCESS;
  for (Index = 0; Index < BmcChannelCount; Index ++) {

    // EFI_TRANSPORT_SET_SOL_CONFIG_PARAM to enable/disable sol.
    ZeroMem (&IpmiCommand[0], 18);
    IpmiCommand[0] = BmcChannel[Index];
    IpmiCommand[1] = 0;           //Set in progress
    IpmiCommand[2] = 1;
    SolDataSize    = 0;  
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_TRANSPORT,
              EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
              &IpmiCommand[0],
              3,
              &SolData[0],
              &SolDataSize
              );
    ZeroMem (&IpmiCommand[0], 18);
    IpmiCommand[0] = BmcChannel[Index];
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
    ZeroMem (&IpmiCommand[0], 18);
    IpmiCommand[0] = BmcChannel[Index];
    IpmiCommand[1] = 0;//Set complete
    IpmiCommand[2] = 0;
    SolDataSize    = 0;
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_TRANSPORT,
              EFI_TRANSPORT_SET_SOL_CONFIG_PARAM,
              &IpmiCommand[0],
              3,
              &SolData[0],
              &SolDataSize
              );
  }

  return Status;
}

EFI_STATUS
EFIAPI
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
  INTN        Index;
  UINT8       BmcChannel[2] = {BASE_BOARD_NIC_CHANNEL_SEL,SHARED_NIC_CHANNEL_SEL};
  UINT8       BmcChannelCount = 1;

  if (PcdGet8(PcdBmcOnboardLanChannelEnable) == FALSE) {
    BmcChannel[0] = SHARED_NIC_CHANNEL_SEL;
  }else{
     if (PcdGet8(PcdBmcShareLanChannelEnable)){
       BmcChannelCount = 2;
     }
  }

  BaudRate = BaudRateXlat[BiosBaudRate];
  for (Index = 0; Index < BmcChannelCount; Index ++) {
    if (BaudRate != This->SolBaudRate) {
      ZeroMem (&IpmiCommand[0], 8);
      IpmiCommand[0] = BmcChannel[Index];
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
      IpmiCommand[0] = BmcChannel[Index];       // look up session info according to the session id
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
      IpmiCommand[0] = BmcChannel[Index];       // look up session info according to the session id
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
      IpmiCommand[0] = BmcChannel[Index];
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
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
SolStatusEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++
  
  Routine Description:
    This is the standard EFI driver point. This function initializes
    the private data required for creating SOL Status Driver.
    
  Arguments:
    ImageHandle     - Handle for the image of this driver
    SystemTable     - Pointer to the EFI System Table

  Returns:
    EFI_SUCCESS     - Protocol successfully started and installed
    EFI_UNSUPPORTED - Protocol can't be started

--*/
{
  EFI_STATUS      Status;
  UINT8           SolBaudRate;
  EFI_HANDLE      Handle;
  BOOLEAN         SolStatus = 0 ;
  UINTN           VarSize;
  BMC_SETUP_DATA  SetupData;
  INTN            Index;
  UINT8           BmcChannel[2] = {BASE_BOARD_NIC_CHANNEL_SEL,SHARED_NIC_CHANNEL_SEL};
  UINT8           BmcChannelCount = 1;

  if (PcdGet8(PcdBmcOnboardLanChannelEnable) == FALSE) {
    BmcChannel[0] = SHARED_NIC_CHANNEL_SEL;
  }else{
     if (PcdGet8(PcdBmcShareLanChannelEnable)){
       BmcChannelCount = 2;
     }
  }


  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &gBmcSystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &SetupData
                  );
  if(EFI_ERROR(Status)){
    SetupData.SerialOverLanEnable = 1;
  } else if (PcdGetBool(PcdSolPriority) && !PcdGetBool(PcdFceLoadDefault) && !PcdGetBool (PcdPlatformNvVariableSwUpdated)) {
    SetupData.SerialOverLanEnable = GetSolStatus(&mIpmiSolStatus, BmcChannel[0], &SolBaudRate);
    Status = gRT->SetVariable (
                    BMC_SETUP_VARIABLE_NAME,
                    &gBmcSystemConfigurationGuid,
                    BMC_SETUP_VARIABLE_FLAG,
                    VarSize,
                    &SetupData
                    );
    ASSERT(!EFI_ERROR(Status));
  }

  DEBUG((EFI_D_INFO, "SOL:%d %r\n", SetupData.SerialOverLanEnable, Status));

  SetSolEnable(&mIpmiSolStatus,(BOOLEAN)SetupData.SerialOverLanEnable);
  for (Index = 0; Index < BmcChannelCount; Index ++) {
    SolStatus = GetSolStatus(&mIpmiSolStatus, BmcChannel[Index], &SolBaudRate);
    DEBUG((EFI_D_INFO, "Sol channel number:%d, Status:%d, SolBaudRate:%d\n", BmcChannel[Index],SolStatus, SolBaudRate));
  }
  //
  // Install SOL status protocol
  //
  mIpmiSolStatus.IpmiSolStatus  = SolStatus;
  mIpmiSolStatus.SolBaudRate    = SolBaudRate;
  mIpmiSolStatus.GetSolStatus   = GetSolStatus;
  mIpmiSolStatus.SetSolBaudRate = EfiSetSolBaudRate;
  mIpmiSolStatus.SetSolEnable   = SetSolEnable;
  Handle                        = NULL;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiIpmiSolStatusProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mIpmiSolStatus
                  );

  return EFI_SUCCESS;
}


