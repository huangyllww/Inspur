
/** @file
Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  BmcCfg.c

Abstract:
  This module is used to configure BMC according to SetupBMC Configuration.

Revision History:

**/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DevicePathLib.h>
#include <BmcConfig.h>
#include <IpmiDefinitions.h>
#include <Library/IpmiBaseLib.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/BmcWdtProtocol.h>
#include <Protocol/Smbios.h>
#include <Library/HobLib.h>
#include <Library/ByoCommLib.h>
#include <Library/PcdLib.h>
//#include <Library/AliOemIpmiCommandLib.h>
//#include <AliMoc/AliOemIpmi.h>

#define BIOS_2_BMC_DATA_SIZE      255
#define EFI_STORAGE_GET_SEL_TIME_UTC_OFFSET   0x5C
#define EFI_STORAGE_SET_SEL_TIME_UTC_OFFSET   0x5D

typedef union{
  UINT32 time;
  UINT8  ch[4];
}TIME_INFO_2_BMC;

typedef union{
  UINT16 UctOffset;
  UINT8  Utc[2];
}UTC_INFO_2_BMC;


STATIC UTC_INFO_2_BMC    gUtcData;
STATIC EFI_GUID          gSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;


UINTN
CalculateNumOfDayPassedThisYear (
  IN    EFI_TIME    *Time
  )
{
  UINTN Index;
  UINTN NumOfDays;
  INTN DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  
  NumOfDays = 0;
  for (Index = 1; Index < Time->Month; Index++) {
     NumOfDays += DaysOfMonth[Index - 1];
  }
  NumOfDays += Time->Day;
  return NumOfDays;
}



UINTN
CountNumOfLeapDays (
   IN EFI_TIME  *Time
   )
{
  UINT16    NumOfYear;
  UINT16    BaseYear;
  UINT16    Index;
  UINTN     Count;
  
  Count      = 0;
  BaseYear  = 1970;
  NumOfYear = Time->Year - 1970;
  
  for (Index = 0; Index <= NumOfYear; Index++) {
    if (IsLeapYear (BaseYear + Index)) {
          Count++;
    }
  }
    
  if ( IsLeapYear( Time->Year ) ) {
    if ( (Count > 0) && (Time->Month < 3) ) {
       Count--;
    }
   }
  return Count;
}



UINT32
EfiSmGetTimeStamp (VOID)
{
  UINT16                  NumOfYears;
  UINTN                   NumOfLeapDays;
  UINTN                   NumOfDays;
  EFI_TIME                Time;
  UINT32                  NumOfSeconds;
  UINT16                  TempUtc=0;
  
  gRT->GetTime (&Time, NULL);
  
  NumOfYears      = Time.Year - 1970;
  NumOfLeapDays   = CountNumOfLeapDays (&Time);
  NumOfDays       = CalculateNumOfDayPassedThisYear (&Time);
  
  NumOfDays += (NumOfLeapDays + (365 * NumOfYears) - 1);
  if((gUtcData.Utc[1] & BIT7) == 0){
    NumOfSeconds = (UINT32) (3600 * 24 * NumOfDays + (Time.Hour * 3600) + (60 * Time.Minute) + Time.Second - gUtcData.UctOffset * 60 );
  }else{
    TempUtc = (UINT16)(~gUtcData.UctOffset + 1);
    NumOfSeconds = (UINT32) (3600 * 24 * NumOfDays + (Time.Hour * 3600) + (60 * Time.Minute) + Time.Second + TempUtc * 60 );
  }

  return NumOfSeconds;
}



VOID
SendTimeInfo2BMC (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINT8                                 ResponseData[4];
  UINT8                                 ResponseSize;
  EFI_STATUS                            Status;
  TIME_INFO_2_BMC                       TIME;

  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  
  TIME.time = EfiSmGetTimeStamp();
  DEBUG((EFI_D_INFO, "TIME:%x\n",TIME.time));

  ResponseSize = sizeof(ResponseData);  
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_SET_SEL_TIME,
             TIME.ch,
             sizeof(TIME),
             ResponseData,
             &ResponseSize
             );
  gBS->CloseEvent (Event);
}



VOID
GetUtcOffsetFromBMC (VOID)
{
  UINT8                                 ResponseData[2];
  UINT8                                 ResponseSize;
  EFI_STATUS                            Status;

  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  ResponseSize = sizeof(ResponseData);
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_GET_SEL_TIME_UTC_OFFSET,
             NULL,
             0,
             ResponseData,
             &ResponseSize
             );
  
  DEBUG((EFI_D_INFO, "GetUtc:%r\n", Status));
  if(!EFI_ERROR(Status)){
    CopyMem(&gUtcData, ResponseData, 2);
  }
  DEBUG((EFI_D_INFO, "UctOffset:%X\n", gUtcData.UctOffset));


// winddy + >> 200923 #2961791 UTC 0x7FF should treat as 0.

// 16-bit, 2s-complement signed integer for the offset in minutes from  
// UTC to SEL Time. LS-byte first. (ranges from -1440 to 1440) 
// 07FFh = "unspecified". Interpret SEL time as local time. 

  if(gUtcData.UctOffset == 0x7FF){
    gUtcData.UctOffset = 0;
  }
// winddy + << 200923 #2961791 UTC 0x7FF should treat as 0.

}



VOID
SetUtcOffset2BMC (VOID)
{
  UINT8                                 ResponseData[2];
  UINT8                                 ResponseSize;
  EFI_STATUS                            Status;
  UINT8                                 CommandData[2];

  
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
  
  CopyMem(CommandData, gUtcData.Utc, 2);
  
  DEBUG((EFI_D_INFO, "UctOffset:%d\n", gUtcData.UctOffset));

  ResponseSize = sizeof(ResponseData);
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_STORAGE,
             EFI_STORAGE_SET_SEL_TIME_UTC_OFFSET,
             CommandData,
             2,
             ResponseData,
             &ResponseSize
             );
  DEBUG((EFI_D_INFO, "SetUtc:%r\n", Status));

}

/*
  BYO specific Hook Point for BMC configuration.  AllDriversConnectedProtocol is installed 
  in BdsPlatform PostConsole
*/
VOID 
EFIAPI
BmcCfgConnectAllCallBack (
  IN EFI_EVENT Event, 
  IN VOID      *Context
  )
{
  EFI_STATUS                   Status;
  VOID                         *Interface;
  
  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));
}


EFI_STATUS
GetBMCSetup (
  BMC_SETUP_DATA  *SetupData
  )
{
  EFI_STATUS      Status;
  BMC_SETUP_DATA  TempSetupData;
  UINTN           VarSize;

  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &gSystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &TempSetupData
                  );
  if (EFI_ERROR(Status)) {
    ZeroMem(&TempSetupData, sizeof(BMC_SETUP_DATA));
    //
    // Give Default value, which is the same as VFR default.
    //
    // Boot Watchdog Setting
    //
    TempSetupData.SerialOverLanEnable = 1;    
    TempSetupData.WdtEnable[0]        = FixedPcdGet8(PcdFrb2WdtDefault);    // Enable by default
    TempSetupData.WdtPolicy[0]        = 1;    // Reset by default
    TempSetupData.WdtTimeout[0]       = 600;  // 10 Minutes Timeout
    //
    // OS Watchdog Setting
    //
    TempSetupData.WdtEnable[1]        = FixedPcdGet8(PcdOsWdtDefault);    // Disable by default
    TempSetupData.WdtPolicy[1]        = 1;    // Reset by default
    TempSetupData.WdtTimeout[1]       = 600;  // 10 Minutes Timeout
    TempSetupData.HideBrandLogo       = 0;    

    Status = gRT->SetVariable (
                    BMC_SETUP_VARIABLE_NAME,
                    &gSystemConfigurationGuid,
                    BMC_SETUP_VARIABLE_FLAG,
                    VarSize,
                    &TempSetupData
                    );
    return Status;
  }

  CopyMem(SetupData, &TempSetupData, sizeof(BMC_SETUP_DATA));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
BmcCfgDriverEntryPoint (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
)
{
  EFI_STATUS            Status;
  BMC_SETUP_DATA        SetupData;
  EFI_BMC_WDT_PROTOCOL  *WdtProt;
  EFI_EVENT             Event;
  EFI_BMC_WDT_CFG       WdtCfg[2];
  EFI_BMC_WDT_CFG       CurrentWdtCfg[2];

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  Status = GetBMCSetup(&SetupData);
  if (EFI_ERROR(Status)) {
    DEBUG((DEBUG_ERROR, "GetBMCSetup fails %r\n", Status));
    return Status;
  }

  Status = gBS->LocateProtocol (
               &gEfiBmcWdtProtocolGuid,
               NULL,
               (VOID **)&WdtProt
               );
               
  if (!EFI_ERROR (Status)) {
    //
    // Boot Watchdog config
    //
    WdtCfg[0].Enable   = SetupData.WdtEnable[0];
    WdtCfg[0].Action   = SetupData.WdtPolicy[0];
    WdtCfg[0].Timeout  = SetupData.WdtTimeout[0];
    //
    // OS Bootwatchdog config
    //
    WdtCfg[1].Enable   = SetupData.WdtEnable[1];
    WdtCfg[1].Action   = SetupData.WdtPolicy[1];
    WdtCfg[1].Timeout  = SetupData.WdtTimeout[1];

    Status = WdtProt->GetWatchdog (WdtProt, CurrentWdtCfg);
    
    //
    // Skip setting a disabled BMC Watchdog to disabled again
    //
    if (Status == EFI_SUCCESS && (CurrentWdtCfg[0].Enable || WdtCfg[0].Enable)) {
      WdtProt->SetWatchdog (WdtProt, WdtCfg);
    }
  }

  EfiCreateProtocolNotifyEvent (
    &gBdsAllDriversConnectedProtocolGuid,
    TPL_CALLBACK,
    BmcCfgConnectAllCallBack,
    NULL,
    &Event
    ); 

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gBmcSetupDataReadyGuid, NULL,
                  NULL
                  );
  Status = gBS->CreateEventEx (
                EVT_NOTIFY_SIGNAL,
                TPL_CALLBACK,
                SendTimeInfo2BMC,
                NULL,
                &gEfiEventReadyToBootGuid,
                &Event
                );

  return EFI_SUCCESS;
}



