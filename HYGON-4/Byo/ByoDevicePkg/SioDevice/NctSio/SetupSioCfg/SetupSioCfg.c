/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SetupSioCfg.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#include "SetupSioCfg.h"

#define ACPI_MMIO_BASE  0xFED80000
#define PMIO2_BASE      0x400

HII_VENDOR_DEVICE_PATH  mSetupSioCfgHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    //{140FCBC6-E8D1-4b0a-AA5F-F117E00A1885}
    SETUP_SIO_CFG_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};


STATIC SIO_SETUP_CONFIG_PRIVATE_DATA gSioSetupData = {
  SIO_SETUP_CONFIG_PRIVATE_SIGNATURE,
  NULL,      //DRIVER HANDLE
  NULL,      //HII HANDLE
  {                                           // EFI_HII_CONFIG_ACCESS_PROTOCOL
    SetupSioFormExtractConfig,
    SetupSioFormRouteConfig,
    SetupSioFormCallback
  },
  {
    NULL,
    SioNvSaveValue,
    SioNvDiscardValue,
    SioNvLoadDefault,
    SioNvSaveUserDefault,
    SioNvLoadUserDefault,
    SioIsNvDataChanged  
  },
  {0}                                        // SIO_SETUP_CONFIG
};



VOID 
DumpEcSpacePage(
  IN UINT8     Page
)
{
   UINTN  Index;
   UINT8  Data8,Count;
   Count = 1;
   DEBUG((EFI_D_INFO,"DumpEcSpacePage:\n"));
   DEBUG((EFI_D_INFO,"   00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n00 "));
   for(Index=0;Index<0x100;Index++){
      Data8 = ReadHwmRegEx(Page, (UINT8)Index);
      DEBUG((EFI_D_INFO, "%02X ", Data8));      
      if(((Index+1)%16)==0){
        DEBUG((EFI_D_INFO, "\n"));
        if(Count<0xF0){
          DEBUG((EFI_D_INFO, "%02x ",Count*0x10));
          Count++;
        }
      }      
   }
   DEBUG((EFI_D_INFO, "\n"));
}


EFI_STATUS
EFIAPI
SetupSioFormExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  SIO_SETUP_CONFIG                 *IfrData;
  SIO_SETUP_CONFIG_PRIVATE_DATA             *Private;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;

  IfrData          = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Status           = EFI_SUCCESS;

  if (Progress == NULL || Results == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  *Progress = Request;
  if((Request != NULL) && !HiiIsConfigHdrMatch(Request, &gSioSetupConfigGuid, SIO_SETUP_VARIABLE_NAME)){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  ConfigRequestHdr = NULL;
  Size             = 0;

  Private = SIO_SETUP_CONFIG_PRIVATE_FROM_THIS_HII(This);
  IfrData = AllocateZeroPool(sizeof(SIO_SETUP_CONFIG));
  if(IfrData == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }
  CopyMem(IfrData, &Private->ConfigData, sizeof(SIO_SETUP_CONFIG));

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  BufferSize = sizeof(SIO_SETUP_CONFIG);
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr(&gSioSetupConfigGuid, 
                                             SIO_SETUP_VARIABLE_NAME, 
                                             Private->DriverHandle);
    Size = (StrLen(ConfigRequestHdr) + 32 + 1) * sizeof(CHAR16);
    ConfigRequest = AllocateZeroPool(Size);
    if(ConfigRequest == NULL){
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;
    }
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
  }

  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                (UINT8*)IfrData,
                                BufferSize,
                                Results,
                                Progress
                                );

  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

ProcExit:
  if(IfrData != NULL){
    FreePool(IfrData);
  }
  if(AllocatedRequest){
    FreePool(ConfigRequest);
  }
  return Status;
}


EFI_STATUS
EFIAPI
SetupSioFormRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if (!HiiIsConfigHdrMatch (Configuration, &gSioSetupConfigGuid, SIO_SETUP_VARIABLE_NAME)) {
    *Progress = Configuration;
    return EFI_NOT_FOUND;
  }

  *Progress = Configuration + StrLen (Configuration);
  return EFI_SUCCESS;
}


VOID
InitString (
  EFI_HII_HANDLE    InitStringHiiHandle,
  EFI_STRING_ID     StrRef,
  CHAR16            *sFormat, ...
  )
{
  STATIC CHAR16 s[1024];
  VA_LIST  Marker;

  VA_START (Marker, sFormat);
  UnicodeVSPrint (s, sizeof (s),  sFormat, Marker);
  VA_END (Marker);

  HiiSetString (InitStringHiiHandle, StrRef, s, NULL);
}

/*
UINT8 ReadHwmRegEx(UINT8 Page,UINT8 Index)
{
  UINT8      Data8;

  IoWrite8(_PCD_VALUE_PcdHwmIoBaseAddress, Page);  
  IoWrite8(_PCD_VALUE_PcdHwmIoBaseAddress + 1, Index);
  Data8 = IoRead8(_PCD_VALUE_PcdHwmIoBaseAddress + 2);
  IoWrite8(_PCD_VALUE_PcdHwmIoBaseAddress, 0xFF);  
  return Data8;
}
*/

EFI_STATUS
EFIAPI
SetupSioFormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  )
{
  EFI_STATUS                     Status;
  UINT8                          Data1;
  UINT8                          Data2;
  //UINT8                          Data3;
  UINTN                          RpmValue;
  UINTN                          Multiple;
  UINTN                          Voltage;
  SIO_SETUP_CONFIG_PRIVATE_DATA  *Private; 
  EFI_STATUS                     Status1;
  UINTN                          VarSize;
  SIO_SETUP_CONFIG               SioConfig;

  //DEBUG((EFI_D_INFO,"%a\n",__FUNCTION__));
  if (Action == EFI_BROWSER_ACTION_FORM_CLOSE) {
    return EFI_SUCCESS;
  }
  //DEBUG((EFI_D_INFO,"Action :%x Type:%x QuestionId = %x\n", Action,Type,QuestionId));
  Private = SIO_SETUP_CONFIG_PRIVATE_FROM_THIS_HII(This);
  VarSize = sizeof(SIO_SETUP_CONFIG);
  ZeroMem(&SioConfig, VarSize);
  Status1 = gRT->GetVariable (
                  SIO_SETUP_VARIABLE_NAME,
                  &gSioSetupConfigGuid,
                  NULL,
                  &VarSize,
                  &SioConfig
                  );

  switch (QuestionId) {
      case SYSTIN_TEMP_KEY:
        Data1 = ReadHwmRegEx(0, 0x27);
        //The data format for sensors SYSTIN, CPUTIN and AUXTIN is 9-bit, two’s-complement. The resolution is 0.5 degrees Celsius.
        Data1 = Data1/2;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_SYSTIN_VALUE), L"%d.0 %s", Data1,L"C");
        break;
      case CPUTIN_TEMP_KEY:
        Data1 = ReadHwmRegEx(1, 0x50);
        Data2 = ReadHwmRegEx(1, 0x51) & 0x80;
        //The data format for sensors SYSTIN, CPUTIN and AUXTIN is 9-bit, two’s-complement. The resolution is 0.5 degrees Celsius.
        Data1 = Data1/2;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_CPUTIN_VALUE), L"%d.%d %s", Data1, Data2 ? 5 : 0, L"C");
        break;
      case AUXTIN_TEMP_KEY:
        Data1 = ReadHwmRegEx(2, 0x50);
        Data2 = ReadHwmRegEx(2, 0x51) & 0x80;
        //The data format for sensors SYSTIN, CPUTIN and AUXTIN is 9-bit, two’s-complement. The resolution is 0.5 degrees Celsius.
        Data1 = Data1/2;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_AUXTIN_VALUE), L"%d.%d %s", Data1 ,Data2 ? 5 : 0, L"C");
        break;
      // case TSI_TEMP_KEY:
      //   Data1 = ReadHwmRegEx(4, 0x9);
      //   Data2 = ReadHwmRegEx(4,0xA) & 0xE0;
      //   DEBUG((EFI_D_INFO,"TSI_TEMP_KEY1:%x,%x\n",Data1,Data2));
      //   InitString (Private->HiiHandle, STRING_TOKEN (STR_TSI_VALUE), L"%d.%d %s", Data1 ,(Data2 >> 5) * 125 / 100, L"C");
      //   break;
      case VOLTAGE_CPUVCORE_KEY:
        Data1 = ReadHwmRegEx(0, 0x20);
        Voltage = Data1 * 8;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_CPUVCORE_VALUE), L"%d.%-1d%-1d%-1d V", Voltage/1000, (Voltage%1000)/100, (Voltage%100)/10, (Voltage%10));
        break;
      case VOLTAGE_VBAT_KEY:
        //WriteHwmRegEx(0, 0x5D, ReadHwmRegEx(0, 0x5D) | BIT0); // EN_VBAT_MNT
        Data2 = ReadHwmRegEx(5, 0x51);
        Voltage = Data2 * 16;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_VBAT_VALUE), L"%d.%-1d%-1d%-1d V", Voltage/1000, (Voltage%1000)/100, (Voltage%100)/10, (Voltage%10));
        break;
      case VOLTAGE_3VSB_KEY:
        Data1 = ReadHwmRegEx(5, 0x50);
        Voltage = Data1 * 16;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_3VSB_VALUE), L"%d.%-1d%-1d%-1d V", Voltage/1000, (Voltage%1000)/100, (Voltage%100)/10, (Voltage%10));
        break;
      case VOLTAGE_3VCC_KEY:
        Data1 = ReadHwmRegEx(0, 0x23);
        Voltage = Data1 * 16;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_3VCC_VALUE), L"%d.%-1d%-1d%-1d V", Voltage/1000, (Voltage%1000)/100, (Voltage%100)/10, (Voltage%10));
        break;
      case VOLTAGE_AVCC_KEY:
        Data1 = ReadHwmRegEx(0, 0x22);
        Voltage = Data1 * 16;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_AVCC_VALUE), L"%d.%-1d%-1d%-1d V", Voltage/1000, (Voltage%1000)/100, (Voltage%100)/10, (Voltage%10));
        break;
      case VOLTAGE_VIN0_KEY:
        Multiple  = 662;  //6.62 Multiple
        Data1 = ReadHwmRegEx(0, 0x21);
        Voltage = Data1 * 8 * Multiple;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_VIN0_VALUE), L"%d.%-1d%-1d%-1d V", Voltage/100000,(Voltage%100000)/10000, (Voltage%10000)/1000, (Voltage%1000)/100);
        break; 
      case VOLTAGE_VIN1_KEY:
        Multiple = 4;
        Data1 = ReadHwmRegEx(0, 0x24);
        Voltage = Data1 * 8 * Multiple;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_VIN1_VALUE), L"%d.%-1d%-1d%-1d V", Voltage/1000,(Voltage%1000)/100, (Voltage%100)/10, (Voltage%10));
        break;
      case VOLTAGE_VIN2_KEY:
        Multiple = 2;
        Data1 = ReadHwmRegEx(0, 0x25);
        Voltage = Data1 * 8 * Multiple;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_VIN2_VALUE), L"%d.%-1d%-1d%-1d V", Voltage/1000,(Voltage%1000)/100, (Voltage%100)/10, (Voltage%10));
        break;
      case VOLTAGE_VIN3_KEY:
        Multiple = 2;
        Data1 = ReadHwmRegEx(0, 0x26);
        Voltage = Data1 * 8 * Multiple;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_VIN3_VALUE), L"%d.%-1d%-1d%-1d V", Voltage/1000,(Voltage%1000)/100, (Voltage%100)/10, (Voltage%10));
        break;
      case CPUFANIN_KEY: // CPUFANIN
        Data1 = ReadHwmRegEx(6, 0x58);
        Data2 =  ReadHwmRegEx(6,0x59);
        /*Data1 = MmioRead8(ACPI_MMIO_BASE+PMIO2_BASE+0x69); //LOW
        Data2 = MmioRead8(ACPI_MMIO_BASE+PMIO2_BASE+0x6A); //HIG
        Data3 = MmioRead8(ACPI_MMIO_BASE+PMIO2_BASE+0x63)&(BIT0|BIT1);
        RpmValue = 22500/(BIT0<<Data3)/((Data2<<8)+Data1)*60;
        InitString (Private->HiiHandle, STRING_TOKEN (STR_SYSFAN4_VALUE), L"%d %s", RpmValue,L"RPM");*/
        RpmValue = ((Data1<<8) | Data2);
        InitString (Private->HiiHandle, STRING_TOKEN (STR_CPUFANIN_VALUE), L"%d %s", (RpmValue == 0xA4) ? 0 : RpmValue,L"RPM");
        break;
      case SYSFANIN_KEY: // SYSFANIN
        Data1 = ReadHwmRegEx(6, 0x56);
        Data2 =  ReadHwmRegEx(6,0x57);
        RpmValue = ((Data1<<8) | Data2);
        InitString (Private->HiiHandle, STRING_TOKEN (STR_SYSFANIN_VALUE), L"%d %s", (RpmValue == 0xA4) ? 0 : RpmValue,L"RPM");
        break;
      case AUXFANIN0_KEY: // AUXFANIN0
        Data1 = ReadHwmRegEx(6, 0x5A);
        Data2 =  ReadHwmRegEx(6,0x5B);
        RpmValue = ((Data1<<8) | Data2);
        InitString (Private->HiiHandle, STRING_TOKEN (STR_AUXFANIN0_VALUE), L"%d %s", (RpmValue == 0xA4) ? 0 : RpmValue,L"RPM");
        break;
      case AUXFANIN1_KEY: // AUXFANIN1
        Data1 = ReadHwmRegEx(6, 0x5C);
        Data2 =  ReadHwmRegEx(6,0x5D);
        RpmValue = ((Data1<<8) | Data2);
        InitString (Private->HiiHandle, STRING_TOKEN (STR_AUXFANIN1_VALUE), L"%d %s", (RpmValue == 0xA4) ? 0 : RpmValue,L"RPM");
        break;
      case AUXFANIN2_KEY: // AUXFANIN2
        Data1 = ReadHwmRegEx(6, 0x5E);
        Data2 = ReadHwmRegEx(6,0x5F);
        RpmValue = ((Data1<<8) | Data2);
        InitString (Private->HiiHandle, STRING_TOKEN (STR_AUXFANIN2_VALUE), L"%d %s", (RpmValue == 0xA4) ? 0 : RpmValue,L"RPM");
        break;
      default:
        Status = EFI_INVALID_PARAMETER;
        break;
  }
  Status1 = gRT->SetVariable (
                SIO_SETUP_VARIABLE_NAME,
                &gSioSetupConfigGuid,
                EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                VarSize,
                &SioConfig
                );

  Status = EFI_SUCCESS;
  return Status;
}

VOID
EFIAPI
CloseWatchDog(
){
  DEBUG ((DEBUG_ERROR, "%a %d\n", __FUNCTION__, __LINE__));
  EnterSioCfgMode();
  WriteSioReg(R_SIO_LOGICAL_DEVICE, 0x08);
  DEBUG ((DEBUG_ERROR, "TIME: %x \n", ReadSioReg(R_SIO_WDT1_COUNTER_REG)));
  WriteSioReg(R_SIO_GPIO0_IO_REG, 0xFF);
  WriteSioReg(R_SIO_WDT1_COUNTER_REG, 0x00);
  WriteSioReg(R_SIO_WDT1_CTRL_STATUS_REG, 0x00);
  WriteSioReg(R_SIO_GPIO0_IO_REG, ReadSioReg(R_SIO_GPIO0_IO_REG) &(~BIT3));
  ExitSioCfgMode();
}
VOID
EFIAPI
SetWatchDog(
  IN UINT8 Time
){
  DEBUG((EFI_D_INFO, "WDT TIME:%d\n", Time));
  DEBUG ((DEBUG_ERROR, "%a %d  \n", __FUNCTION__, __LINE__));
  EnterSioCfgMode();
  WriteSioReg(R_SIO_LOGICAL_DEVICE, 0x08);
  WriteSioReg(R_SIO_WDT1_COUNTER_REG, Time);
  WriteSioReg(R_SIO_WDT1_CTRL_STATUS_REG, 0x00);
  ExitSioCfgMode();
  #define R_GPIO3 (0xFED80D00UL + 3)
  #define FCH_GPIO_3_PIN   (0xfed81500UL  + 3* 4)
  MmioWrite8(R_GPIO3, 0x00);           // EGPIO116
  MmioAndThenOr32(FCH_GPIO_3_PIN, (UINT32)~BIT22, BIT23);      // output low
  gBS->Stall(300);
  MmioOr32(FCH_GPIO_3_PIN, (UINT32)BIT22);      // output HIGH
  MmioOr32(FCH_GPIO_3_PIN, (UINT32)BIT23);      // output HIGH
}
VOID
EFIAPI
InitWatchDog(
){
  DEBUG ((DEBUG_ERROR, "%a %d\n", __FUNCTION__, __LINE__));
  EnterSioCfgMode();
  WriteSioReg(R_SIO_LOGICAL_DEVICE, 0x08);
  WriteSioReg(R_SIO_GLOBAL_OPTION, ReadSioReg(R_SIO_GLOBAL_OPTION) & (~BIT6));
  WriteSioReg(R_SIO_GPIO0_MULTI_FUNC, BIT3);
  WriteSioReg(R_SIO_ACTIVATE, 0x3);
  WriteSioReg(R_SIO_GPIO0_IO_REG, ReadSioReg(R_SIO_GPIO0_IO_REG) &(~BIT3));
  ExitSioCfgMode();
}
EFI_STATUS 
SioNvSaveValue(  
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
 //EFI_STATUS            Status = EFI_SUCCESS;
  
  DEBUG((EFI_D_ERROR,"%a\n",__FUNCTION__));
  return EFI_UNSUPPORTED;
}



EFI_STATUS 
SioNvDiscardValue(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
 //EFI_STATUS            Status = EFI_SUCCESS;
 // TCM_DXE_PRIVATE_DATA  *Private; 

 // Private = TCM_DXE_DATA_FROM_THIS_SETUP_SAVE(This);
  DEBUG((EFI_D_ERROR,"%a\n",__FUNCTION__));
  
  return EFI_UNSUPPORTED;
}



EFI_STATUS 
SioNvLoadDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{

  DEBUG((EFI_D_ERROR,"%a\n",__FUNCTION__));
  
  return EFI_UNSUPPORTED;
}


EFI_STATUS 
SioNvSaveUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_UNSUPPORTED;  
}


EFI_STATUS 
SioNvLoadUserDefault(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_UNSUPPORTED;  
}

EFI_STATUS 
SioIsNvDataChanged(
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This,
  BOOLEAN                       *IsDataChanged
  )
{
  *IsDataChanged = FALSE;
  return EFI_SUCCESS;
}


EFI_STATUS
SetupSioCfgEntryPoint (
  IN EFI_HANDLE                           ImageHandle,
  IN EFI_SYSTEM_TABLE                     *SystemTable
  )
/*++

Routine Description:

Arguments:

  ImageHandle - Handle of this driver image
  SystemTable - Table containing standard EFI services

Returns:

  EFI_SUCCESS

--*/
{
  EFI_STATUS                  Status;
  UINTN                       VarSize;
  SIO_SETUP_CONFIG            SioConfig;
  UINT16                      SioChipId;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  SioChipId = GetSioChipId();
  DEBUG((EFI_D_INFO, "Check SIO ChipId %X\n", SioChipId));
  if (SioChipId != NCT6776_CHIP_ID && SioChipId != NCT6126_CHIP_ID) {
    return EFI_UNSUPPORTED;
  }
  //enable cpu fan detect for fan2 rpm reading 
  MmioWrite8(ACPI_MMIO_BASE+PMIO2_BASE+0x66, 0x01);
  //
  // Check Sio Setup Variable .
  //
  VarSize = sizeof(SIO_SETUP_CONFIG);  
  Status = gRT->GetVariable (
                  SIO_SETUP_VARIABLE_NAME,
                  &gSioSetupConfigGuid,
                  NULL,
                  &VarSize,
                  &SioConfig
                  );

    if(Status == EFI_NOT_FOUND ) {
      //
      // First in, we should set variable.
      //
      VarSize = sizeof(SIO_SETUP_CONFIG);
      ZeroMem(&SioConfig, VarSize);

      Status = gRT->SetVariable (
                      SIO_SETUP_VARIABLE_NAME,
                      &gSioSetupConfigGuid,
                      SIO_SETUP_VARIABLE_FLAG,
                      VarSize,
                      &SioConfig
                      );
    }


  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gSioSetupData.DriverHandle,
                  &gEfiDevicePathProtocolGuid, &mSetupSioCfgHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid, &gSioSetupData.ConfigAccess,
                  &gSetupSaveNotifyProtocolGuid,&gSioSetupData.SetupSaveNotify,
                  &gEfiCallerIdGuid, NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  gSioSetupData.SetupSaveNotify.DriverHandle = gSioSetupData.DriverHandle;
  gSioSetupData.HiiHandle = HiiAddPackages (
                               &gSioSetupConfigGuid,
                               gSioSetupData.DriverHandle,
                               SetupSioCfgStrings,               
                               SetupSioCfgVfrBin,
                               NULL
                               );
  ASSERT (gSioSetupData.HiiHandle != NULL);
  
  InitWatchDog();
  if (SioConfig.WtdEn) {
    SetWatchDog(SioConfig.WatchDogTime);
    Status = gBS->InstallMultipleProtocolInterfaces(
              &ImageHandle,
              &gPlatAfterConsoleStartProtocolGuid, CloseWatchDog,
              NULL
              );
    ASSERT_EFI_ERROR (Status);
  }
  return EFI_SUCCESS;
}



