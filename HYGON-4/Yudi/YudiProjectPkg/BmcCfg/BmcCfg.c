
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
  Source file for the IPMI driver.

Revision History:

**/

#include <Uefi.h>
#include <IndustryStandard/Pci30.h>
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
#include <Protocol/BmcCfgProtocol.h>
#include <Protocol/Smbios.h>
#include <Library/HobLib.h>
#include <Library/ByoCommLib.h>
#include <Protocol/PlatHostInfoProtocol.h>
#include <Protocol/ByoDiskInfoProtocol.h>
#include <Protocol/PciIo.h>
#include <ByoBootGroup.h>
#include <Protocol/IpmiBootOrder.h>
#include <Library/PcdLib.h>

#define BIOS_2_BMC_DATA_SIZE      255

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
  IN    EFI_TIME    Time
  )
{
  UINTN Index;
  UINTN NumOfDays;
  INTN DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

  NumOfDays = 0;
  for (Index = 1; Index < Time.Month; Index++) {
     NumOfDays += DaysOfMonth[Index - 1];
  }
  NumOfDays += Time.Day;
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

  if (IsLeapYear( Time->Year ) ) {
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
  NumOfDays       = CalculateNumOfDayPassedThisYear (Time);

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
SendTimeInfo2BMC (VOID)
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
  DEBUG((EFI_D_INFO, "%r\n", Status));
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
             EFI_STORAGE_GET_SEL_UTC_OFFSET,
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
             EFI_STORAGE_SET_SEL_UTC_OFFSET,
             CommandData,
             2,
             ResponseData,
             &ResponseSize
             );
  DEBUG((EFI_D_INFO, "SetUtc:%r\n", Status));

}
VOID
SetGroupTypeTop(UINT8* GroupOrder,UINTN GroupCount,UINT8 Group)
{
  UINT8                    Index,Index1;
  UINT8                    *NewGroupOrder;

  NewGroupOrder = AllocateZeroPool (GroupCount);
  CopyMem(NewGroupOrder, GroupOrder, GroupCount);

  for(Index=0,Index1=1; Index<GroupCount; Index++){
    if(Group != NewGroupOrder[Index]){
      GroupOrder[Index1++] = NewGroupOrder[Index];
    }else{
      GroupOrder[0] = Group;
    }
  }
  FreePool(NewGroupOrder);
}

VOID
ClearBootFlagsValid (
  )
{
  UINT8                                 ResponseSize = 1;
  EFI_STATUS                            Status;
  UINT8                                 CommandData[6] = {0x05,0x00,0x00,0x00,0x00,0x00};
  UINT8                                 ResponseData[1];

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  ZeroMem (ResponseData, 1);
  Status = EfiSendCommandToBMC (
             0x00,
             0x08,
             &CommandData[0],
             6,
             &ResponseData[0],
             &ResponseSize
             );
  DEBUG((EFI_D_INFO, "ClearBootFlagsValid Status:%r\n", Status));
}

//
//Item need change boot group
//
EFI_STATUS
IpmiSetBootGroup()
{
  EFI_STATUS                        Status;
  EFI_HOB_GUID_TYPE                 *GuidHob;
  IPMI_FORCE_BOOT_OPTIONS           *IpmiForceBootOptions;
  UINTN                             GroupOrderSize = 0;
  UINT8                             *GroupOrder = NULL;
  UINT8                             GroupType;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  GuidHob = GetFirstGuidHob(&gIpmiForceBootOptionHobGuid);
  if (GuidHob == NULL || GET_GUID_HOB_DATA_SIZE (GuidHob) != sizeof(IPMI_FORCE_BOOT_OPTIONS)) {
    return EFI_UNSUPPORTED;
  }
  IpmiForceBootOptions = GET_GUID_HOB_DATA (GuidHob);
  if (!IpmiForceBootOptions->BootValid || !IpmiForceBootOptions->PersistentOptions){//don't need to set the order
    DEBUG((EFI_D_INFO,"BootValid false or PersistentOptions apply to next boot only"));
    return EFI_SUCCESS;
  }
  //
  //  Force boot setup,Setup is not a member of the BootGroup
  //
  if (IpmiForceBootOptions->BootDevice == ForceBootBiosSetup){
    ClearBootFlagsValid();
    return EFI_SUCCESS;
  }

  if (IpmiForceBootOptions->BiosBootType == 0) {//BIOS_BOOT_LEGACY_OS
    Status = gRT->GetVariable(
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME, 
                  &gByoGlobalVariableGuid,
                  NULL,
                  &GroupOrderSize,
                  NULL
                  );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      GroupOrder = AllocatePool(GroupOrderSize);
      Status = gRT->GetVariable(
                  BYO_LEGACY_BOOT_GROUP_VAR_NAME, 
                  &gByoGlobalVariableGuid,
                  NULL,
                  &GroupOrderSize,
                  GroupOrder
                  );
    } else {
      DEBUG((EFI_D_INFO,"BootGroup not created.\n"));
      return EFI_UNSUPPORTED;
    }
    DumpMem8(GroupOrder,GroupOrderSize);
    switch(IpmiForceBootOptions->BootDevice){
      case ForceBootPxe:
        GroupType = BM_MENU_TYPE_LEGACY_PXE;
        break;
      case ForceBootHardDrive:
        GroupType = BM_MENU_TYPE_LEGACY_HDD;
        break;
      case ForceBootRemoteCdDvd:
        GroupType = BM_MENU_TYPE_LEGACY_USB_ODD;
        break;
      case ForceBootFloppy:
        GroupType = BM_MENU_TYPE_LEGACY_USB_DISK;
        break;
      default:
        if(GroupOrder != NULL) FreePool(GroupOrder);
        return EFI_UNSUPPORTED;
    }
    SetGroupTypeTop(GroupOrder,GroupOrderSize,GroupType);
    DumpMem8(GroupOrder,GroupOrderSize);  
    Status = gRT->SetVariable (
                    BYO_LEGACY_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    GroupOrderSize,
                    GroupOrder
                    );
  } else {//BIOS_BOOT_UEFI_OS
    Status = gRT->GetVariable(
                  BYO_UEFI_BOOT_GROUP_VAR_NAME, 
                  &gByoGlobalVariableGuid,
                  NULL,
                  &GroupOrderSize,
                  NULL
                  );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      GroupOrder = AllocatePool(GroupOrderSize);
      Status = gRT->GetVariable(
                  BYO_UEFI_BOOT_GROUP_VAR_NAME, 
                  &gByoGlobalVariableGuid,
                  NULL,
                  &GroupOrderSize,
                  GroupOrder
                  );
    } else {
      DEBUG((EFI_D_INFO,"BootGroup not created.\n"));
      return EFI_UNSUPPORTED;
    }
    DEBUG((EFI_D_INFO,"GetVariable Status - %r\n",Status));
    DumpMem8(GroupOrder,GroupOrderSize);
    switch(IpmiForceBootOptions->BootDevice){
      case ForceBootPxe:
        GroupType = BM_MENU_TYPE_UEFI_PXE;
        break;
      case ForceBootHardDrive:
        GroupType = BM_MENU_TYPE_UEFI_HDD;
        break;
      case ForceBootRemoteCdDvd:
        GroupType = BM_MENU_TYPE_UEFI_USB_ODD;
        break;
      case ForceBootFloppy:
        GroupType = BM_MENU_TYPE_UEFI_USB_DISK;
        break;
      default:
        if(GroupOrder != NULL) FreePool(GroupOrder);
        return EFI_UNSUPPORTED;
    }
    SetGroupTypeTop(GroupOrder,GroupOrderSize,GroupType);
    DumpMem8(GroupOrder,GroupOrderSize);  
    Status = gRT->SetVariable (
                    BYO_UEFI_BOOT_GROUP_VAR_NAME,
                    &gByoGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    GroupOrderSize,
                    GroupOrder
                    );
}
  ClearBootFlagsValid();
  
  if(GroupOrder != NULL) FreePool(GroupOrder);

  return EFI_SUCCESS;
}

VOID
BmcCfgConnectAllCallBack (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_STATUS                   Status;
  VOID                         *Interface;
  UINT8                        *ResponseData;
  UINT8                        *CommandData;
  EFI_SMBIOS_PROTOCOL          *Smbios;


  Status = gBS->LocateProtocol(&gBdsAllDriversConnectedProtocolGuid, NULL, (VOID**)&Interface);
  if (EFI_ERROR (Status)) {
    return;
  }

  gBS->CloseEvent(Event);

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  CommandData  = AllocatePool(BIOS_2_BMC_DATA_SIZE);
  ResponseData = AllocatePool(BIOS_2_BMC_DATA_SIZE);

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  ASSERT_EFI_ERROR (Status);

  ZeroMem(CommandData, BIOS_2_BMC_DATA_SIZE);
  GetUtcOffsetFromBMC();
  SendTimeInfo2BMC();
  IpmiSetBootGroup();

  InvokeHookProtocol(gBS, &gPlatformIpmiConfigSetupProtocolGuid);

  FreePool(ResponseData);
  FreePool(CommandData);

}




EFI_STATUS
BmcCfgDriverEntryPoint (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
)
{
  EFI_STATUS            Status;
  BMC_SETUP_DATA        SetupData;
  UINTN                 VarSize;
  EFI_BMC_WDT_PROTOCOL  *WdtProt;

  EFI_EVENT             Event;
  EFI_BMC_WDT_CFG       WdtCfg[2];               // wz200914 -  clean BMC_SETUP_DATA


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  EfiInitializeIpmiBase ();

  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &gSystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &SetupData
                  );
  if (EFI_ERROR(Status)) {
    ZeroMem(&SetupData, sizeof(SetupData));
// wz200914 - >>  clean BMC_SETUP_DATA, keep default value be same with VFR.
    SetupData.SerialOverLanEnable = 1;
    SetupData.WdtEnable[0]  = 1;
    SetupData.WdtPolicy[0]  = 1;
    SetupData.WdtTimeout[0] = 600;
    SetupData.WdtEnable[1]  = 0;
    SetupData.WdtPolicy[1]  = 2;
    SetupData.WdtTimeout[1] = 600;
// wz200914 - << clean BMC_SETUP_DATA

    Status = gRT->SetVariable (
                    BMC_SETUP_VARIABLE_NAME,
                    &gSystemConfigurationGuid,
                    BMC_SETUP_VARIABLE_FLAG,
                    VarSize,
                    &SetupData
                    );
    ASSERT(!EFI_ERROR(Status));
  }

  Status = gBS->LocateProtocol (
               &gEfiBmcWdtProtocolGuid,
               NULL,
               &WdtProt
               );
  if (!EFI_ERROR (Status)) {
// wz200914 - >>  clean BMC_SETUP_DATA
    WdtCfg[0].Enable   = SetupData.WdtEnable[0];
    WdtCfg[0].Action   = SetupData.WdtPolicy[0];
    WdtCfg[0].Timeout  = SetupData.WdtTimeout[0];
    WdtCfg[1].Enable   = SetupData.WdtEnable[1];
    WdtCfg[1].Action   = SetupData.WdtPolicy[1];
    WdtCfg[1].Timeout  = SetupData.WdtTimeout[1];
// wz200914 - <<  clean BMC_SETUP_DATA,
    WdtProt->SetWatchdog (WdtProt, WdtCfg);
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

  return EFI_SUCCESS;
}



