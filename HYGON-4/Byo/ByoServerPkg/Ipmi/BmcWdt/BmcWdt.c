/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BmcWdt.c

Abstract:
  Source file for the  BMC watchdog timer.

Revision History:

**/


#include "BmcWdt.h"

EFI_BMC_WATCHDOG_INSTANCE mWdtProtPrivate;
EFI_BMC_WDT_CFG           mWdtCfg[2];
BOOLEAN                   mWdtRun[2] = {FALSE, FALSE};

EFI_STATUS
EFIAPI
ResetWatchdogTimer (
  IN  EFI_BMC_WDT_PROTOCOL  *This
)
{
  EFI_STATUS                Status;
  UINT8                     ResponseData;
  UINT8                     ResponseSize;


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

EFI_STATUS
EFIAPI
GetWatchdogTimer (
  IN   EFI_BMC_WDT_PROTOCOL  *This,
  OUT  EFI_BMC_WDT_CFG       *WdtCfg
  )
{
  EFI_STATUS         Status;
  EFI_BMC_WDT_DATA   WdtData;
  UINT8              WdtDataSize = sizeof(EFI_BMC_WDT_DATA);


  if(This == NULL || WdtCfg == NULL){
    return EFI_INVALID_PARAMETER;
  }

  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_APP,
             EFI_APP_GET_WATCHDOG_TIMER,
             NULL,
             0,
             (UINT8*)&WdtData,
             &WdtDataSize
             );
  DEBUG ((EFI_D_INFO, "GetWatchdogTimer:%r %d\n", Status, WdtDataSize));
  if(!EFI_ERROR(Status)){
    WdtCfg->Enable  = WdtData.TimerUse.Bit.DontStop;
    WdtCfg->Action  = WdtData.TimerActions.Bit.TimeoutAction;
    WdtCfg->Timeout = WdtData.PresentCountdown / 10;
    DEBUG((EFI_D_INFO, "[Frb2Wdt] %d %d %d %d %d\n", WdtCfg->Enable, WdtCfg->Action, WdtCfg->Timeout, \
      WdtData.InitialCountdown, WdtData.PresentCountdown));    
  }
  
  return Status;
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


  DEBUG((EFI_D_INFO, "BiosWdtNotification Run %d %d\n", mWdtRun[0], mWdtRun[1]));

  if(mWdtRun[0] || mWdtRun[1]){
    DataSize      = sizeof(EFI_BMC_WDT_DATA) - sizeof(UINT16);
    ResponseSize  = 0;
    ZeroMem(&mWdtProtPrivate.Data, DataSize);
    if(mWdtRun[0]){
      mWdtProtPrivate.Data.TimerUse.Bit.Timer = 1;
      mWdtProtPrivate.Data.ExpirationFlag.Bit.BiosFrb2 = 1;
    } else {
      mWdtProtPrivate.Data.TimerUse.Bit.Timer = 3;
      mWdtProtPrivate.Data.ExpirationFlag.Bit.OsLoad = 1;
    }
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
    mWdtRun[0] = FALSE;
    mWdtRun[1] = FALSE;    
  }
}

VOID
EFIAPI
ResetBiosWdt (
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

  DEBUG((EFI_D_INFO, "ResetBiosWdt Run %d %d\n", mWdtRun[0], mWdtRun[1]));

  WdtCfg        = (EFI_BMC_WDT_CFG *)mWdtCfg;
  DataSize      = sizeof (EFI_BMC_WDT_DATA) - sizeof (UINT16);
  ResponseSize  = 0;
  ZeroMem(&mWdtProtPrivate.Data, DataSize);
  if (WdtCfg[0].Enable) {
    mWdtProtPrivate.Data.TimerUse.Bit.Timer             = 1;
    mWdtProtPrivate.Data.TimerActions.Bit.TimeoutAction = WdtCfg[0].Action;
    mWdtProtPrivate.Data.InitialCountdown               = WdtCfg[0].Timeout * 1000 / 100;
    mWdtProtPrivate.Data.ExpirationFlag.Bit.BiosFrb2    = 1;

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
    DEBUG ((EFI_D_INFO, "Reset BIOS Watchdog : %r\n", Status));
    ResetWatchdogTimer(&mWdtProtPrivate.Protocol);
    mWdtRun[0] = TRUE;
  }
  return ;
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


  DEBUG((EFI_D_INFO, "OsWdtNotification Run %d %d\n", mWdtRun[0], mWdtRun[1]));

  WdtCfg        = (EFI_BMC_WDT_CFG *)mWdtCfg;
  DataSize      = sizeof (EFI_BMC_WDT_DATA) - sizeof (UINT16);
  ResponseSize  = 0;
  ZeroMem(&mWdtProtPrivate.Data, DataSize);
  //
  //If frb2 enable ,close it first.
  //
  if (mWdtRun[0]) {
    mWdtProtPrivate.Data.TimerUse.Bit.Timer   = 1;
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
    mWdtRun[0] = FALSE;
  }


  ZeroMem(&mWdtProtPrivate.Data, DataSize);
  if (WdtCfg[1].Enable) { 
    if(!mWdtRun[1]){
      mWdtProtPrivate.Data.TimerUse.Bit.Timer             = 3;
      mWdtProtPrivate.Data.TimerActions.Bit.TimeoutAction = WdtCfg[1].Action;
      mWdtProtPrivate.Data.InitialCountdown               = WdtCfg[1].Timeout * 1000 / 100;
      mWdtProtPrivate.Data.ExpirationFlag.Bit.OsLoad      = 1;
      DEBUG((DEBUG_INFO, "Enable OS Watchdog : Timer %x\n", mWdtProtPrivate.Data.TimerUse));   
      DEBUG((DEBUG_INFO, "Enable OS Watchdog : TimeoutAction %x\n", mWdtProtPrivate.Data.TimerActions));
      DEBUG((DEBUG_INFO, "Enable OS Watchdog : OsLoad %x\n", mWdtProtPrivate.Data.ExpirationFlag));
      DEBUG((DEBUG_INFO, "Enable OS Watchdog : InitialCountdown %x\n", mWdtProtPrivate.Data.InitialCountdown));
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
      mWdtRun[1] = TRUE;
    }
    
    ResetWatchdogTimer(&mWdtProtPrivate.Protocol);
  }
 
//gBS->CloseEvent (Event);

  return;
}

EFI_STATUS
EFIAPI
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

  if (WdtCfg[0].Enable) {
    Instance->Data.TimerUse.Bit.Timer             = 1;
    Instance->Data.TimerActions.Bit.TimeoutAction = WdtCfg[0].Action;
    Instance->Data.InitialCountdown               = WdtCfg[0].Timeout * 1000 / 100;
    Instance->Data.ExpirationFlag.Bit.BiosFrb2    = 1;
  } else {
    Instance->Data.TimerUse.Bit.Timer             = 1;
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
  if (WdtCfg[0].Enable) {
    ResetWatchdogTimer(This);
  }
  CopyMem(mWdtCfg, WdtCfg, sizeof(mWdtCfg));  // wz200914 +
  mWdtRun[0] = WdtCfg[0].Enable;

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  BiosWdtNotification,
                  NULL,
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
                  NULL,
                  &gEfiEventReadyToBootGuid,
                  &OsWdtEvent
                  );

Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  ResetBiosWdt,
                  NULL,
                  &BiosWdtEvent
                  );
  Status = gBS->RegisterProtocolNotify (
                  &gEfiFrb2WatchDogResetNotifyGuid,
                  BiosWdtEvent,
                  &BiosWdtEventReg
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
BmcWdtDriverEntryPoint (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
)
{
  EFI_STATUS            Status;
  EFI_HANDLE            NewHandle;

  Status  = EFI_SUCCESS;

  EfiInitializeIpmiBase ();

  mWdtProtPrivate.Signature               = (UINTN)SM_BMC_WATCHDOG_SIGNATURE;
  mWdtProtPrivate.Protocol.SetWatchdog    = SetWatchdogTimer;
  mWdtProtPrivate.Protocol.GetWatchdog    = GetWatchdogTimer;
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
  DEBUG ((EFI_D_INFO, "Install BmcWdt %r\n", Status));

  return EFI_SUCCESS;
}


