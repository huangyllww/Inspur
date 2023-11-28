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


#include "BmcWdt.h"

EFI_BMC_WATCHDOG_INSTANCE mWdtProtPrivate;


EFI_STATUS
ResetWatchdogTimer (
  IN  EFI_BMC_WDT_PROTOCOL  *This
)
{
  EFI_STATUS                Status;
  UINT8                     ResponseData;
  UINT8                     ResponseSize;
  EFI_BMC_WATCHDOG_INSTANCE *Instance;

  Instance = INSTANCE_FROM_EFI_BMC_WATCHDOG_THIS(This);
  ResponseSize    = 0;

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            EFI_APP_RESET_WATCHDOG_TIMER,
            NULL,
            0,
            &ResponseData,
            &ResponseSize
            );
  
  DEBUG ((EFI_D_INFO, "ResetWatchdogTimer : %r\n", Status));
  return EFI_SUCCESS;
}

VOID
EFIAPI
BiosWdtNotification (
  EFI_EVENT                               Event,
  VOID                                    *Context
  ) 
{
  EFI_STATUS                Status;
  UINT8                     DataSize;
  UINT8                     ResponseData;
  UINT8                     ResponseSize;

  DataSize      = sizeof (EFI_BMC_WDT_DATA) - sizeof (UINT16);
  ResponseSize  = 0;
  ZeroMem(&mWdtProtPrivate.Data, DataSize);
  mWdtProtPrivate.Data.TimerUse.Bit.Timer   = 1;
  mWdtProtPrivate.Data.TimerUse.Bit.DontLog = 1;
  mWdtProtPrivate.Data.ExpirationFlag.Bit.BiosFrb2 = 1;
  mWdtProtPrivate.Data.InitialCountdown = 0 ;
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_APP,
             EFI_APP_SET_WATCHDOG_TIMER,
             (UINT8 *)&mWdtProtPrivate.Data,
             DataSize,
             &ResponseData,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "Disable BIOS Watchdog: %r\n", Status));

  gBS->CloseEvent (Event);
}

VOID
EFIAPI
OsWdtNotification (
  EFI_EVENT                               Event,
  VOID                                    *Context
  )
{
  EFI_STATUS                Status;
  EFI_BMC_WDT_CFG           *WdtCfg;
  UINT8                     DataSize;
  UINT8                     ResponseData;
  UINT8                     ResponseSize;
  UINTN                     i;

  WdtCfg        = (EFI_BMC_WDT_CFG *) Context;
  DataSize      = sizeof (EFI_BMC_WDT_DATA) - sizeof (UINT16);
  ResponseSize  = 0;
  ZeroMem(&mWdtProtPrivate.Data, DataSize);
  //
  //If frb2 enable ,close it first.
  //
  if (WdtCfg[1].Enable == TRUE) { 
    mWdtProtPrivate.Data.TimerUse.Bit.Timer   = 1;
    mWdtProtPrivate.Data.TimerUse.Bit.DontLog = 1;
    mWdtProtPrivate.Data.ExpirationFlag.Bit.BiosFrb2 = 1;
    mWdtProtPrivate.Data.InitialCountdown = 0 ;

    Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_APP,
             EFI_APP_SET_WATCHDOG_TIMER,
             (UINT8 *)&mWdtProtPrivate.Data,
             DataSize,
             &ResponseData,
             &ResponseSize
             );
    DEBUG ((EFI_D_INFO, "Disable OS Watchdog: %r\n", Status));  
  }


  ZeroMem(&mWdtProtPrivate.Data, DataSize);
  if (WdtCfg[3].Enable == TRUE) { 
    mWdtProtPrivate.Data.TimerUse.Bit.Timer             = 3;
    mWdtProtPrivate.Data.TimerUse.Bit.DontLog           = 0;
    mWdtProtPrivate.Data.TimerActions.Bit.TimeoutAction = WdtCfg[3].Action;
    mWdtProtPrivate.Data.InitialCountdown               = WdtCfg[3].Timeout * 1000 / 100;
    mWdtProtPrivate.Data.ExpirationFlag.Bit.OsLoad      = 1;

    Status = EFI_SUCCESS;
    for(i = 0; i < 3; i++) {
      Status = EfiSendCommandToBMC (
                EFI_SM_NETFN_APP,
                EFI_APP_SET_WATCHDOG_TIMER,
                (UINT8 *)&mWdtProtPrivate.Data,
                DataSize,
                &ResponseData,
                &ResponseSize
                );
      if(Status == EFI_SUCCESS) break;
    }
    DEBUG ((EFI_D_INFO, "Enable OS Watchdog : %r\n", Status));
    ResetWatchdogTimer(&mWdtProtPrivate.Protocol);
  }
 
  gBS->CloseEvent (Event);

  return ;
}

EFI_STATUS
SetWatchdogTimer (
  IN  EFI_BMC_WDT_PROTOCOL  *This,
  IN  EFI_BMC_WDT_CFG       *WdtCfg
  )
{
  EFI_STATUS                Status;
  UINT8                     ResponseData;
  UINT8                     ResponseSize;
  UINT8                     DataSize;
  EFI_BMC_WATCHDOG_INSTANCE *Instance;
  EFI_EVENT                 BiosWdtEvent;
  VOID                      *BiosWdtEventReg;
  EFI_EVENT                 OsWdtEvent;
  UINTN                     i;

  Instance      = INSTANCE_FROM_EFI_BMC_WATCHDOG_THIS(This);
  ResponseSize  = 0;
  DataSize      = sizeof (EFI_BMC_WDT_DATA) - sizeof (UINT16);
  ZeroMem(&Instance->Data, DataSize);

  if (WdtCfg[1].Enable == TRUE) {
    Instance->Data.TimerUse.Bit.Timer             = 1;
    Instance->Data.TimerUse.Bit.DontLog           = 0;
    Instance->Data.TimerActions.Bit.TimeoutAction = WdtCfg[1].Action;
    Instance->Data.InitialCountdown               = WdtCfg[1].Timeout * 1000 / 100;
    Instance->Data.ExpirationFlag.Bit.BiosFrb2    = 1;
  } else {
    Instance->Data.TimerUse.Bit.Timer             = 1;
    Instance->Data.TimerUse.Bit.DontLog           = 1;
    Instance->Data.ExpirationFlag.Bit.BiosFrb2    = 1;
  }
  Status = EFI_SUCCESS;
  for(i = 0; i < 3; i++) {
    Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              EFI_APP_SET_WATCHDOG_TIMER,
              (UINT8 *)&Instance->Data,
              DataSize,
              &ResponseData,
              &ResponseSize
              );
    if(Status == EFI_SUCCESS) break;
  }
  DEBUG ((EFI_D_INFO, "Enable SetWatchdogTimer : %r\n", Status));
  ResetWatchdogTimer(This);

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  BiosWdtNotification,
                  WdtCfg,
                  &BiosWdtEvent
                  );
  ASSERT_EFI_ERROR (Status);
  Status = gBS->RegisterProtocolNotify (
                  &gEfiSetupEnterGuid,
                  BiosWdtEvent,
                  &BiosWdtEventReg
                  );
  Status = gBS->RegisterProtocolNotify (
                  &gEfiBootMenuEnterGuid,
                  BiosWdtEvent,
                  &BiosWdtEventReg
                  );
  Status = gBS->RegisterProtocolNotify (
                  &gEfiFrb2WatchDogNotifyGuid,
                  BiosWdtEvent,
                  &BiosWdtEventReg
                  );

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  OsWdtNotification,
                  WdtCfg,
                  &gEfiEventReadyToBootGuid,
                  &OsWdtEvent
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


EFI_STATUS
BmcWdtDriverEntryPoint (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
)
{
  EFI_STATUS            Status;
  EFI_HANDLE            NewHandle;

  Status  = EFI_SUCCESS;

  EfiInitializeIpmiBase ();

  mWdtProtPrivate.Signature               = SM_BMC_WATCHDOG_SIGNATURE;
  mWdtProtPrivate.Protocol.SetWatchdog    = SetWatchdogTimer;
  mWdtProtPrivate.Protocol.ResetWatchdog  = ResetWatchdogTimer;

  //
  // Now install the Protocol
  //
  NewHandle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &NewHandle,
                  &gEfiBmcWdtProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mWdtProtPrivate.Protocol
                  );
  // ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "Install gEfiBmcWdtProtocolGuid : %r\n", Status));

  return EFI_SUCCESS;
}
