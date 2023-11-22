/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SetupBmcCfg.c

Revision History:

**/

#include "SetupBmcCfg.h"
#include <Protocol/IpmiTransportProtocol.h>
#include <SysMiscCfg.h>
#include <Guid/MdeModuleHii.h>
#include <Guid/MemoryStatusCodeRecord.h>
#include <Library/SmmServicesTableLib.h>


EFI_IPMI_TRANSPORT     *mIpmiTransport = NULL;
UINT8                  mBmcVendorValue;

CHAR16      mBmcLanVariableName[] = L"BmcLanConfig";
CHAR16      mBmcLanVarStoreName[] = L"BmcLanConfig";
BMC_LAN_CONFIG                          mBmcLanConfig;
EFI_HANDLE                               DriverHandle;
EFI_HII_HANDLE                               mHiiHandle = NULL;
EFI_HII_CONFIG_ROUTING_PROTOCOL     *mHiiConfigRouting = NULL;
EFI_GUID                      mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
EFI_GUID                       gSetupBmcCfgPkgListGuid = SETUP_BMC_CFG_GUID;

HII_VENDOR_DEVICE_PATH  mSetupBmcCfgHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    //
    // {56536848-E1FD-4cc3-BE85-CA76B8BA353B}
    //
    //{ 0x56536848, 0xe1fd, 0x4cc3, { 0xbe, 0x85, 0xca, 0x76, 0xb8, 0xba, 0x35, 0x3b } }
    SETUP_BMC_CFG_GUID
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

EFI_HII_CONFIG_ACCESS_PROTOCOL  ConfigAccess;

/**
Check Bmc Vendor.

@return 0,    BYO Bmc.
@return 0xff, NO-BYO Bmc.
**/
UINT8
CheckBmcVendor ( 
  VOID
  )	
{
  EFI_STATUS    Status = EFI_SUCCESS;
  UINT8         Response[16]={0};
  UINTN         Responsesize;
  UINT8         Vendor = 0xff;
  
  Responsesize    = sizeof(Response);
  
  Status = EfiSendCommandToBMC (
            0x6,
            0x1,
            NULL,
            0,
            Response,
            (UINT8 *) &Responsesize
            );  
  if(Status == EFI_SUCCESS){
    if((Response[6]==0xcc)&&(Response[7]==0xcb)&&(Response[8]==0x0)){
       Vendor=0;
    }
  }

  DEBUG((DEBUG_INFO, "%a, Vendor :0x%x-%r.\n", __FUNCTION__, Vendor, Status));
  return Vendor;
}

EFI_STATUS
SendConfigInfoToBmc (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  EFI_STATUS      Status;
  UINT8           CmdDataBuff[10];
  UINT8           CmdDataSize;
  UINTN           VarSize;
  BMC_SETUP_DATA  BmcSetupData = {{0}};
  EFI_GUID        SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_SOL_STATUS_PROTOCOL         *IpmiSolStatus;
  DEBUG((EFI_D_INFO, "%a (L %d), \n\n", __FUNCTION__, __LINE__));
  
  Status = gBS->LocateProtocol (
             &gEfiIpmiSolStatusProtocolGuid,
             NULL,
             (VOID **)&IpmiSolStatus
             );


  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &BmcSetupData
                  );

  if(mBmcVendorValue==0){
    CmdDataBuff[0] = 0;
    CmdDataBuff[1] = BmcSetupData.NCSI;
    CmdDataSize = 2;
    Status = EfiSendCommandToBMC (
               0x3c,
               0x04,
               (UINT8 *) &CmdDataBuff,
               CmdDataSize,
               NULL,
               NULL
               );
    DEBUG((EFI_D_INFO,"Set NCSI : %d - %r\n",BmcSetupData.NCSI, Status));
  }

  SetBmcAfterPowerLoss(BmcSetupData.PowerRestorePolicy);
  //set error
  if (BmcSetupData.SerialOverLanEnable) {
    IpmiSolStatus->SetSolEnable(IpmiSolStatus,TRUE);
  } else {
    IpmiSolStatus->SetSolEnable(IpmiSolStatus,FALSE);
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetupBmcCfgExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  UINTN                            Size;
  BOOLEAN                          AllocatedRequest;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress        = Request;
  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &mSystemConfigurationGuid, mBmcLanVarStoreName)) {
    return EFI_NOT_FOUND;
  }



  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  Size             = 0;
  AllocatedRequest = FALSE;
  ConfigRequest    = Request;

  //
  // Get Buffer Storage data from EFI variable
  //
  BufferSize = sizeof (BMC_LAN_CONFIG);
  
  if (Request == NULL || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request is set to NULL, construct full request string.
    //

    //
    // First Set ConfigRequestHdr string.
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&mSystemConfigurationGuid, mBmcLanVarStoreName, DriverHandle);
    ASSERT (ConfigRequestHdr != NULL);

    //
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template 
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    FreePool (ConfigRequestHdr);
  }

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  Status = mHiiConfigRouting->BlockToConfig (
                                mHiiConfigRouting,
                                ConfigRequest,
                                (UINT8 *) &mBmcLanConfig,
                                BufferSize,
                                Results,
                                Progress
                                );

  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = (EFI_STRING)(Request + StrLen (Request));
  }

  return Status;
}

EFI_STATUS
EFIAPI
SetupBmcCfgRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS                                 Status;
  UINTN                                      BufferSize;
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  if (!HiiIsConfigHdrMatch (Configuration, &mSystemConfigurationGuid, mBmcLanVarStoreName)) {
    return EFI_NOT_FOUND;
  }
  
  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  BufferSize = sizeof (BMC_LAN_CONFIG);
  Status = mHiiConfigRouting->ConfigToBlock (
                               mHiiConfigRouting,
                               Configuration,
                               (UINT8 *) &mBmcLanConfig,
                               &BufferSize,
                               Progress
                               );
  
  if(PcdGet8(PcdBmcIsNeedSendConfigInfoToBmc)){
    SendConfigInfoToBmc(NULL); 
  }

  BmcCfgSetVariable(mBmcLanVarStoreName, &mSystemConfigurationGuid, 0, BufferSize, &mBmcLanConfig);
  
  return Status; 
}

EFI_STATUS
EFIAPI
SetupBmcCfgCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest  OPTIONAL
  )
{
  EFI_STATUS        Status;

//Status = HhmCallBack (This, Action, QuestionId, Type, Value, ActionRequest);
  Status = EFI_INVALID_PARAMETER;
  if (Status == EFI_INVALID_PARAMETER) {
    Status = ServerMgmtFormCallback(This, Action, QuestionId, Type, Value, ActionRequest);
    if (Status == EFI_INVALID_PARAMETER) {
      return ValidateSetupInput(This, Action, QuestionId, Type, Value, ActionRequest);
    }
  }
  return Status;
}

EFI_STATUS
GetBmcFwVersion (
  IN EFI_IPMI_TRANSPORT  *pIpmiTransport,
  IN UINT8               *Version
  )
{
  EFI_SM_CTRL_INFO  ControllerInfo;  
  UINT8             ResponseDataSize;
  EFI_STATUS        Status;


  ResponseDataSize = sizeof(ControllerInfo);
  Status = EfiSendCommandToBMC (
              EFI_SM_NETFN_APP,
              EFI_APP_GET_DEVICE_ID,
              NULL,
              0,
              (UINT8*)&ControllerInfo,
              &ResponseDataSize
              );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "GetBmcFwVersion:%r\n", Status));
    return Status;
  }

  Version[2] = ControllerInfo.MajorFirmwareRev;
  Version[1] = ControllerInfo.MinorFirmwareRev;             // BCD
  Version[1] = ((Version[1] >> 4) & 0xF) * 10 + (Version[1] & 0xF);
  Version[0] = (UINT8)(ControllerInfo.AuxFirmwareRevInfo & 0xFF);

  Version[3] = ControllerInfo.SpecificationVersion;

  Version[4] = ControllerInfo.DeviceId;
  Version[5] = ControllerInfo.DeviceRevision;

  DEBUG((EFI_D_INFO, "BMC Ver:%d.%d.%d\n", Version[2], Version[1], Version[0]));

  return EFI_SUCCESS;  
}

EFI_STATUS
GetBmcMacAddress (
  IN EFI_IPMI_TRANSPORT  *pIpmiTransport,
  IN UINT8               *MacAddress,
  IN UINT8               Channel
  )
{
  UINT8       commanddata[4];
  UINT8       commanddatasize;
  UINT8       Response[20];
  UINT8       Responsesize;
  EFI_STATUS  Status;

  ZeroMem (commanddata, sizeof(commanddata));
  ZeroMem (Response, sizeof(Response)); 

  //
  // Get MAC Address
  //
  commanddata[0] = Channel;           // Channel number
  commanddata[1] = MAC_ADDRESS_PARM;  // Parameter selector
  commanddata[2] = 0;                 // Set Selector.
  commanddata[3] = 0;                 // Block Selector 
  commanddatasize = sizeof(commanddata);
  Responsesize    = sizeof(Response);

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_LAN,           // NetFunction
            EFI_GET_LAN_CONFIG_CMD,     // Command
            (UINT8 *) &commanddata[0],  // *CommandData
            commanddatasize,            // CommandDataSize
            (UINT8 *) &Response,        // *ResponseData
            (UINT8 *) &Responsesize     // *ResponseDataSize
            );
  CopyMem(MacAddress, &Response[1], 6);
  DEBUG((EFI_D_INFO, "BMC MAC:%02x-%02x-%02x-%02x-%02x-%02x\n", Response[1], Response[2], Response[3], Response[4], Response[5], Response[6]));

  return Status;  
}

EFI_STATUS
IpmiGetSelfTest (
  IN EFI_IPMI_TRANSPORT *pIpmiTransport,
  IN UINT8   *Data
  )
{
  EFI_STATUS  Status;
  UINT8       ResponseBuff[34];
  UINT8       ResponseSize;

  ResponseSize = sizeof (ResponseBuff);
  ZeroMem (ResponseBuff, ResponseSize);

  Status = pIpmiTransport->SendIpmiCommand (
                            pIpmiTransport,
                            EFI_SM_NETFN_APP,
                            0,
                            EFI_APP_GET_SELFTEST_RESULTS,
                            NULL,
                            0,
                            (UINT8 *)&ResponseBuff,
                            (UINT8 *)&ResponseSize
                            );

  DEBUG ((EFI_D_INFO, "Get Self Test cmd:%r\n", Status));
  if (!EFI_ERROR (Status) && (NULL != Data)) {
    Data[0] = ResponseBuff[0];
    Data[1] = ResponseBuff[1];
  }
  return Status;
}

EFI_STATUS
UpdateBmcStatusString (
  IN EFI_HII_HANDLE    HiiHandle,
  IN EFI_STRING_ID     String1Id,
  IN EFI_STRING_ID     String2Id,
  IN UINT8             control,
  IN UINT8             Data
  )
{

  EFI_STRING                Tmpting;
  CHAR16                    TmpSting[40];

  SetMem(TmpSting, sizeof (TmpSting), 0);
  Tmpting= NULL;
  if(control == 0) {
    Tmpting = HiiGetString (HiiHandle, String1Id, "en-US");
    HiiSetString(HiiHandle, String2Id, Tmpting, "en-US");
    if(Tmpting != NULL){
      FreePool(Tmpting);
    }

    Tmpting = HiiGetString (HiiHandle, String1Id, "zh-Hans");
    HiiSetString(HiiHandle, String2Id, Tmpting, "zh-Hans");
    if(Tmpting != NULL){
      FreePool(Tmpting);
    }
  }else {
    Tmpting = HiiGetString (HiiHandle, String1Id, "en-US");
    UnicodeSPrint (TmpSting, sizeof (TmpSting), L"%s[0x%2X]", Tmpting, Data);
    HiiSetString(HiiHandle, String2Id, TmpSting, "en-US");
    if(Tmpting != NULL){
      FreePool(Tmpting);
    }
    SetMem(TmpSting, sizeof (TmpSting), 0);
    Tmpting = HiiGetString (HiiHandle, String1Id, "zh-Hans");
    UnicodeSPrint (TmpSting, sizeof (TmpSting), L"%s[0x%2X]", Tmpting, Data);
    HiiSetString(HiiHandle, String2Id, TmpSting, "zh-Hans");
    if(Tmpting != NULL){
      FreePool(Tmpting);
    }
  }
  return EFI_SUCCESS;
}


VOID
InitBMCStrings (
  IN EFI_HII_HANDLE     HiiHandle
  )
{
  EFI_STATUS                Status;
  UINT8                     BmcFwVersion[20];
  UINT8                     Data[6];
  CHAR16                    TmpSting[40];
  UINT32                    Bmc;
  EFI_BMC_STATUS            *BmcStatus = &Bmc;
  EFI_SM_COM_ADDRESS        ComAddress;
  CHAR8                     *BmcVer;  
  
  if (NULL == mIpmiTransport) {

    Status = gBS->LocateProtocol (
             &gEfiIpmiTransportProtocolGuid,
             NULL,
             (VOID **) &mIpmiTransport
             );
    if (EFI_ERROR (Status)) {
      mIpmiTransport = NULL;
    }
  }
  ZeroMem(Data, 6);
  if (NULL != mIpmiTransport) {
    SetMem (BmcFwVersion, sizeof (BmcFwVersion), 0);
    SetMem(TmpSting, sizeof (TmpSting), 0);
    Status = GetBmcFwVersion (mIpmiTransport, Data);
    if (!EFI_ERROR (Status)) {
      BmcVer = PcdGetPtr(PcdBmcFwVerStr8);
      if(BmcVer[0] == 0){
        UnicodeSPrint(TmpSting, sizeof(TmpSting), L"%d.%02d.%02d", Data[2], Data[1], Data[0]);
      } else {
        UnicodeSPrint(TmpSting, sizeof(TmpSting), L"%a", BmcVer);
      }
      HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_FIRMWARE_VERSION_VALUE), TmpSting, NULL);
      if (Data[3] == 0x02) {
        HiiSetString(HiiHandle, STRING_TOKEN(STR_IPMI_REVISION_VALUE), L"2.0", NULL); 
      }
      SetMem(TmpSting, sizeof (TmpSting), 0);
      UnicodeSPrint (TmpSting, sizeof (TmpSting), L"%d", Data[4]);
      HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_DEVICE_ID_VALUE), TmpSting, NULL);
      SetMem(TmpSting, sizeof (TmpSting), 0);
      UnicodeSPrint (TmpSting, sizeof (TmpSting), L"%d", Data[5]);
      HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_DEVICE_REVISION_VALUE), TmpSting, NULL);
    }
    Status = GetBmcMacAddress (mIpmiTransport, Data, BASE_BOARD_NIC_CHANNEL_SEL);
    if (!EFI_ERROR (Status)) {
      UnicodeSPrint (TmpSting, sizeof (TmpSting), L"%02x-%02x-%02x-%02x-%02x-%02x", Data[0], Data[1], Data[2], Data[3], Data[4], Data[5]);
      HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_MAC_ADDRESS_VALUE), TmpSting, NULL);
    }
    Status = IpmiGetSelfTest (mIpmiTransport, Data);
    if (EFI_ERROR(Status)) {
      UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_SM_BMC_STATUS_ERROR),STRING_TOKEN(STR_SM_BMC_STATUS_VALUE), 0, 0);
    } else {
       switch (Data[0]) {
         case 0x55:
           UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_SM_BMC_STATUS_PASSED),STRING_TOKEN(STR_SM_BMC_STATUS_VALUE), 0, 0);
         break;
         case 0x56:
           UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_SM_BMC_STATUS_IMPLEMENTED),STRING_TOKEN(STR_SM_BMC_STATUS_VALUE), 0, 0);
         break;
         case 0x57:
           UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_SM_BMC_STATUS_INACCESSIBLE),STRING_TOKEN(STR_SM_BMC_STATUS_VALUE), 1, Data[1]);
         break;
         case 0x58:
           UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_SM_BMC_STATUS_FATAL),STRING_TOKEN(STR_SM_BMC_STATUS_VALUE), 0, 0);
         break;
         default:
           UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_SM_BMC_STATUS_DEVICE),STRING_TOKEN(STR_SM_BMC_STATUS_VALUE), 1, Data[0]);
         break;
       }
    }
    Status = mIpmiTransport->GetBmcStatus(
                                        mIpmiTransport,
  	                                    BmcStatus,
  	                                    &ComAddress    
  	                                    );
    if (Bmc == 0 ) {
      UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_BMC_STATUS_OK),STRING_TOKEN(STR_BMC_STATUS_VALUE), 0, 0);
    }else if (Bmc == 1 ) {
      UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_BMC_STATUS_SOFT_FAIL),STRING_TOKEN(STR_BMC_STATUS_VALUE), 0, 0);
    }else if (Bmc == 2 ) {
      UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_BMC_STATUS_HARD_FAIL),STRING_TOKEN(STR_BMC_STATUS_VALUE), 0, 0);
    }else if (Bmc == 3 ) {
      UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_BMC_STATUS_UPDATE),STRING_TOKEN(STR_BMC_STATUS_VALUE), 0, 0);
    }else if (Bmc == 4 ) {
      UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_BMC_STATUS_NOT_READY),STRING_TOKEN(STR_BMC_STATUS_VALUE), 0, 0);
    }else {
      UpdateBmcStatusString(HiiHandle,STRING_TOKEN(STR_BMC_STATUS_UNKNOWN),STRING_TOKEN(STR_BMC_STATUS_VALUE), 0, 0);
    }
  }
  if (NULL == mIpmiTransport) {
    HiiSetString(HiiHandle, STRING_TOKEN(STR_SM_BMC_STATUS_VALUE), L"Ipmi transport error", NULL);
  }
}


VOID
EFIAPI
UpdateBmcLanConfig (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  UINTN               DataSize; 
  InitBMCStrings(mHiiHandle);
  InitFruStrings(mHiiHandle);
  DataSize = sizeof(BMC_LAN_CONFIG);
  BmcCfgGetVariable(mBmcLanVarStoreName, &mSystemConfigurationGuid, NULL, &DataSize, &mBmcLanConfig);
  gBS->CloseEvent (Event);
}



EFI_STATUS
SetBmcAfterPowerLoss (
  IN UINT8  AfterPowerLoss
  )
{
  EFI_STATUS      Status;
  UINT8           CmdDataBuff[16];
  UINT8           CmdDataSize;
  UINT8           ResponseBuff[34];
  UINT8           ResponseSize;


// USE bmc real value, so do not need to convert value.
/*
// 0: off
// 1: previous
// 2: on  
  switch(AfterPowerLoss){
    case AFTER_POWER_LOSS_OFF:
    default:
      AfterPowerLoss = 0;
      break;

    case AFTER_POWER_LOSS_PREVIOUS:
      AfterPowerLoss = 1;
      break;   

    case AFTER_POWER_LOSS_ON:
      AfterPowerLoss = 2;
      break;       
  }
*/

  CmdDataBuff[0] = AfterPowerLoss & 0x03;
  CmdDataSize    = 1;
  ResponseSize   = sizeof(ResponseBuff);
  ZeroMem(ResponseBuff, ResponseSize);
  
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_CHASSIS,
             EFI_CHASSIS_SET_POWER_RESTORE_POLICY,
             CmdDataBuff,
             CmdDataSize,
             ResponseBuff,
             &ResponseSize
             );

  DEBUG ((EFI_D_INFO, "%a(%d) %r\n", __FUNCTION__, AfterPowerLoss, Status));

  return Status;
}


EFI_STATUS
SelComponents ( 
IN BMC_SETUP_SEL_EVENT_DATA  *TempSetupData
  ) 
{
  BOOLEAN         BootClearSEL;

  DEBUG ((DEBUG_ERROR, "%a %d  \n", __FUNCTION__, __LINE__));
  BootClearSEL = FALSE;
  switch (TempSetupData->ClearSysEventLog) {
  case BMC_CLEAR_SYS_EVENT_LOG_CLEAR:
    BootClearSEL = TRUE;
    break;
  case BMC_CLEAR_SYS_EVENT_LOG_NO_CLEAR://no clear
    break;
  case BMC_CLEAR_SYS_EVENT_LOG_NEXT_BOOT_CLEAR://nextboot clear
    ClearSEL();
    BootClearSEL = TRUE;
    break;
  case BMC_CLEAR_SYS_EVENT_LOG_EVERY_BOOT_CLEAR://everyboot clear
    ClearSEL();
    break;
  default:
    break;
  }
  if (BootClearSEL == TRUE) {
    TempSetupData->ClearSysEventLog = BMC_CLEAR_SYS_EVENT_LOG_NO_CLEAR;
  }
  return EFI_SUCCESS;
}





EFI_STATUS
EFIAPI
BmcSaveValue (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  return EFI_SUCCESS;
}

EFI_STATUS
IpmiGetChassisStatus (
  IN UINT8   *Data,
  IN UINTN   Size
  )
{
  EFI_STATUS  Status;
  UINT8       ResponseBuff[34];
  UINT8       ResponseSize;

  if ((NULL == Data) || (Size < 4)) {
     return EFI_INVALID_PARAMETER;
  }

  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);

  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_CHASSIS,
             EFI_CHASSIS_GET_STATUS,
             NULL,
             0,
             (UINT8 *)&ResponseBuff,
             (UINT8 *)&ResponseSize
             );

  DEBUG ((EFI_D_INFO, "Get Chassis Status cmd:%r\n", Status));
  if (!EFI_ERROR (Status)) {
    CopyMem (Data, ResponseBuff, Size);
  }
  return Status;
}

SETUP_SAVE_NOTIFY_PROTOCOL  mSetupSaveNotify;


EFI_STATUS
EFIAPI
SetupBmcCfgEntryPoint (
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
  VOID                        *mEventReg;
  EFI_EVENT                   mEvent; 
  UINT8                       ResponseBuff[3];
  UINT8                       ResponseSize;
  UINT8                       ChassisStatus[5];
  BMC_SETUP_DATA              SetupData = {{0}};
  BMC_SETUP_SEL_EVENT_DATA    SelSetupData = {0};
  UINT8                       Commanddata[3]={0,0,0};
  UINTN                       VarSize;
  UINTN                       SelVarSize;
  EFI_GUID                    SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);
  //
  // Initialize COM layer Library as we will use IPMI COM Library functions.
  //
  EfiInitializeIpmiBase ();
  mBmcVendorValue=CheckBmcVendor();

  if ((PcdGetBool (PcdFceLoadDefault) || PcdGetBool (PcdPlatformNvVariableSwUpdated)) && PcdGet8(PcdBmcIsNeedSendConfigInfoToBmc)) {
    DEBUG ((DEBUG_INFO, "Send NCSI, SOL, PowerRestorePolicy to BMC when PcdFceLoadDefault is TRUE or PcdPlatformNvVariableSwUpdated is TRUE.\n"));
    SendConfigInfoToBmc(NULL);
  }

  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &SetupData
                  );
  ASSERT_EFI_ERROR (Status);

  if(mBmcVendorValue==0){
    Status = EfiSendCommandToBMC (
               0x32,
               0x72,
               Commanddata,
               3,
               (UINT8 *)&ResponseBuff,
               (UINT8 *)&ResponseSize
               );
    DEBUG((EFI_D_INFO,"Get Ncsi Command respond data is %d %d \n",ResponseBuff[0],ResponseBuff[1]));
    if(!EFI_ERROR (Status)){
      SetupData.NCSI = ResponseBuff[1];
    }
  }

  // 0: off
  // 1: previous
  // 2: on
  Status = IpmiGetChassisStatus(ChassisStatus, sizeof(ChassisStatus));
  if(!EFI_ERROR (Status)){
    SetupData.PowerRestorePolicy = (ChassisStatus[0] >> 5) & 0x03;
  }

  SelVarSize = sizeof (BMC_SETUP_SEL_EVENT_DATA);
  Status = gRT->GetVariable (
                  SEL_EVENT_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  NULL,
                  &SelVarSize,
                  &SelSetupData
                  );
  if (Status == EFI_NOT_FOUND) {
    //
    //  Reset variable
    //
    DEBUG ((DEBUG_INFO, "BmcSetupSelEventData variable is not found\n"));
    SelSetupData.ClearSysEventLog = 1;
    SelSetupData.SelComponents = 1;
    SelSetupData.StatusCodeLog = 0;
    Status = gRT->SetVariable (
                SEL_EVENT_SETUP_VARIABLE_NAME,
                &SystemConfigurationGuid,
                BMC_SETUP_VARIABLE_FLAG,
                SelVarSize,
                &SelSetupData
                );
  }
  ASSERT_EFI_ERROR (Status);
  SelComponents(&SelSetupData);
  Status = gRT->SetVariable (
                  SEL_EVENT_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  BMC_SETUP_VARIABLE_FLAG,
                  SelVarSize,
                  &SelSetupData
                  );

  Status = gRT->SetVariable (
                BMC_SETUP_VARIABLE_NAME,
                &SystemConfigurationGuid,
                BMC_SETUP_VARIABLE_FLAG,
                VarSize,
                &SetupData
                );

  Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID**)&mHiiConfigRouting);
  ASSERT_EFI_ERROR (Status);

  DriverHandle = NULL;
  ConfigAccess.ExtractConfig = SetupBmcCfgExtractConfig;
  ConfigAccess.RouteConfig   = SetupBmcCfgRouteConfig;
  ConfigAccess.Callback      = SetupBmcCfgCallback;  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DriverHandle,
                  &gEfiDevicePathProtocolGuid, &mSetupBmcCfgHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid, &ConfigAccess,
                  &gEfiCallerIdGuid, NULL,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  mHiiHandle = HiiAddPackages (
               &gSetupBmcCfgPkgListGuid,
               DriverHandle,
               BmcConfigBin,
               SetupBmcCfgStrings,
               NULL
               );
  ASSERT (mHiiHandle != NULL);

  //
  // Register Notification to get BMC Lan config before entering into setup
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL, 
                  TPL_CALLBACK, 
                  UpdateBmcLanConfig,
                  NULL, 
                  &mEvent);
  ASSERT_EFI_ERROR (Status);
 
  Status = gBS->RegisterProtocolNotify (
                   &gEfiSetupEnterGuid,
                   mEvent,
                   &mEventReg
                    );
  ASSERT_EFI_ERROR (Status);

  ZeroMem(&mSetupSaveNotify, sizeof(SETUP_SAVE_NOTIFY_PROTOCOL));
  mSetupSaveNotify.SaveValue = BmcSaveValue;
  Status = gBS->InstallProtocolInterface (
                  &mSetupSaveNotify.DriverHandle,
                  &gSetupSaveNotifyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSetupSaveNotify
                  );

  return EFI_SUCCESS;
}

