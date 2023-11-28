/** @file

Copyright (c) 2006 - 2016, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  KcsBmc.h

Abstract:
  Source file for the IPMI driver.

Revision History:

**/

#include "SetupBmcCfg.h"

//EFI_IPMI_TRANSPORT     *mIpmiTransport = NULL;

CHAR16      mBmcLanVariableName[] = L"BmcLanConfig";
CHAR16      mBmcLanVarStoreName[] = L"BmcLanConfig";
BMC_LAN_CONFIG                          mBmcLanConfig;
EFI_HANDLE                               DriverHandle;
EFI_HII_HANDLE                               HiiHandle = NULL;
EFI_HII_CONFIG_ROUTING_PROTOCOL     *mHiiConfigRouting = NULL;
EFI_GUID                      mSystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
EFI_GUID                       gSetupBmcCfgPkgListGuid = SETUP_BMC_CFG_GUID;
extern IPMI_INTERFACE_PROTOCOL  *mIpmiInterface;

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
extern EFI_GUID gUpdateBmcLanConfigNotifyGuid;


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

  DEBUG((EFI_D_ERROR, "%a()\n", __FUNCTION__));

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
  DEBUG((EFI_D_ERROR, "%a()\n", __FUNCTION__));

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

  Status = HhmCallBack (This, Action, QuestionId, Type, Value, ActionRequest);
  if (Status == EFI_INVALID_PARAMETER) {
    Status = ServerMgmtFormCallback(This, Action, QuestionId, Type, Value, ActionRequest);
    if (Status == EFI_INVALID_PARAMETER) {
      return ValidateSetupInput(This, Action, QuestionId, Type, Value, ActionRequest);
    }
  }
  return Status;
}
/*
EFI_STATUS
GetBmcFwVersion (
  IN EFI_IPMI_TRANSPORT  *pIpmiTransport,
  IN UINT8   *Data
  )
{
  EFI_STATUS                     Status;
  UINT8       ResponseBuff[20];
  UINT8       ResponseSize;

  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);

  Status = pIpmiTransport->SendIpmiCommand (
                            pIpmiTransport,
                            EFI_SM_NETFN_APP,
                            0,
                            EFI_APP_GET_DEVICE_ID,
                            NULL,
                            0,
                            (UINT8 *)&ResponseBuff,
                            (UINT8 *)&ResponseSize
                            );

  DEBUG ((EFI_D_INFO, "Get device ID:%r\n", Status));
  if (!EFI_ERROR (Status) && (NULL != Data)) {
    Data[2] = ResponseBuff[2];
    Data[3] = ResponseBuff[3];
    Data[4] = ResponseBuff[4];    
  }
  return Status;
}

VOID
InitBMCStrings (
  IN EFI_HII_HANDLE     HiiHandle
  )
{
  EFI_STATUS                Status;
  UINT8                    BmcFwVersion[20];
  UINT8                     Data[2];
  CHAR16                    TmpSting[32];

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
  if (NULL != mIpmiTransport) {
    SetMem (BmcFwVersion, sizeof (BmcFwVersion), 0);
    SetMem(TmpSting, sizeof (TmpSting), 0);
    Status = GetBmcFwVersion (mIpmiTransport, Data);
    if (!EFI_ERROR (Status)) {
      UnicodeSPrint (TmpSting, sizeof (TmpSting), L"%1x.%2x", Data[2],Data[3]);
      HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_FIRMWARE_REVISION_VALUE), TmpSting, NULL);
      if (Data[4]==0x02) {
      HiiSetString(HiiHandle, STRING_TOKEN(STR_IPMI_REVISION_VALUE), L"2.0", NULL); 
      }     
    }
    Status = IpmiGetSelfTest (mIpmiTransport, Data);
    if (EFI_ERROR(Status)) {
      HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_SELF_TEST_STATUS_VALUE), L"ERROR", NULL);
    } else {
       switch (Data[0]) {
         case 0x55:
           HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_SELF_TEST_STATUS_VALUE), L"PASSED", NULL);
         break;
         case 0x56:
           HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_SELF_TEST_STATUS_VALUE), L"Not implemented", NULL);
         break;
         case 0x57:
           UnicodeSPrint (TmpSting, sizeof (TmpSting), L"Inaccessible data or device[%2xh]", Data[1]);
           HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_SELF_TEST_STATUS_VALUE), TmpSting, NULL);
         break;
         case 0x58:
           HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_SELF_TEST_STATUS_VALUE), L"Fatal HW error", NULL);
         break;
         default:
           UnicodeSPrint (TmpSting, sizeof (TmpSting), L"Device Special failure[%2xh]", Data[0]);
           HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_SELF_TEST_STATUS_VALUE), TmpSting, NULL);
         break;
       }
    }
  }
  if (NULL == mIpmiTransport) {
    HiiSetString(HiiHandle, STRING_TOKEN(STR_BMC_SELF_TEST_STATUS_VALUE), L"Ipmi transport error", NULL);
  }

}

*/


EFI_STATUS
IpmiGetSelfTest (
  IN UINT8   *Data
  )
{
  EFI_STATUS  Status;
  UINT8       ResponseBuff[34];
  UINT8       ResponseSize;

  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);

  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_APP,
            EFI_APP_GET_SELFTEST_RESULTS,
            NULL,
            0,
            ResponseBuff,
            &ResponseSize
            ); 

  DEBUG ((EFI_D_INFO, "Get Self Test cmd:%r\n", Status));
  if (!EFI_ERROR (Status) && (NULL != Data)) {
    Data[0] = ResponseBuff[0];
    Data[1] = ResponseBuff[1];
  }
  return Status;
}



void
UpdateBmcFirmwareVersionValue (  
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_STRING_ID                     TokenToUpdate
  )
{
  CHAR16                                TempString[100];
  UINT16                                FwVersion;

  if (mIpmiInterface == NULL) {
    return;
  }

  ZeroMem (TempString, sizeof (TempString));
  FwVersion = mIpmiInterface->GetBmcFirmwareVersion (mIpmiInterface);

#if BC_TENCENT
	UnicodeSPrint (
	  TempString,
	  sizeof (TempString),
	  L"%d.%02x.%02d",
	  FwVersion >> 12, 
	  (FwVersion >> 4) & 0xff, 
	  FwVersion & 0xf
	  );
#else
	UnicodeSPrint (
	  TempString,
	  sizeof (TempString),
	  L"%d.%x.%d",
	  FwVersion >> 12, 
	  (FwVersion >> 4) & 0xff, 
	  FwVersion & 0xf
	  );
#endif
    
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);
  return;
}

/**
 Update BMC Status string in BIOS Setup.
*/
void
UpdateBmcStatusValue (
  IN  EFI_HII_HANDLE            HiiHandle,
  IN  EFI_STRING_ID             TokenToUpdate
  )
{
  CHAR16                          TempString[30];
  IPMI_BMC_STATUS                 BmcStatus;

  if (mIpmiInterface == NULL) {
    return;
  }
  BmcStatus = mIpmiInterface->GetBmcStatus(mIpmiInterface);
  ZeroMem (TempString, sizeof (TempString));

  switch (BmcStatus) {
    case BMC_STATUS_OK:
      UnicodeSPrint (TempString, sizeof (TempString), L"OK");
      break;

    case BMC_STATUS_ERROR:
      UnicodeSPrint (TempString, sizeof (TempString), L"Error");
      break;

    case BMC_STATUS_NOT_READY:
      UnicodeSPrint (TempString, sizeof (TempString), L"Not Ready");
      break;

    case BMC_STATUS_NOT_FOUND:
      UnicodeSPrint (TempString, sizeof (TempString), L"Not Found");
      break;

    case BMC_STATUS_UNKNOWN:
      UnicodeSPrint (TempString, sizeof (TempString), L"Unknown");
      break;
  }

  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

  return;
}



void
UpdateBmcSelfTestStatusValue (
  IN  EFI_HII_HANDLE            HiiHandle,
  IN  EFI_STRING_ID             TokenToUpdate
  )
{
  EFI_STATUS               Status;
  CHAR16                   TempString[30];
  UINT8                     Data[2];

 
  Status = IpmiGetSelfTest (Data);
  if (EFI_ERROR(Status)) {
    UnicodeSPrint (TempString, sizeof (TempString), L"Not Ready");    
  } else {
     switch (Data[0]) {
       case 0x55:
         UnicodeSPrint (TempString, sizeof (TempString), L"PASSED");    
       break;
       case 0x56:
         UnicodeSPrint (TempString, sizeof (TempString), L"Not implemented");    
       break;
       case 0x57:
         UnicodeSPrint (TempString, sizeof (TempString), L"Inaccessible data or device[%2xh]", Data[1]);
       break;
       case 0x58:
         UnicodeSPrint (TempString, sizeof (TempString), L"Fatal HW error");         
       break;
       default:
         UnicodeSPrint (TempString, sizeof (TempString), L"Device Special failure[%2xh]", Data[0]);
       break;
     }
  }
  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);

  return;
}


void
UpdateIPMIVersionValue (  
  IN  EFI_HII_HANDLE                    HiiHandle,
  IN  EFI_STRING_ID                     TokenToUpdate
  )
{
  CHAR16                                TempString[100];
  UINT16                                IpmiVersion;

  if (mIpmiInterface == NULL) {
    return;
  }

  ZeroMem (TempString, sizeof (TempString));
  IpmiVersion = mIpmiInterface->GetIpmiVersion (mIpmiInterface);

  UnicodeSPrint (
    TempString,
    sizeof (TempString),
    L"%x.%d",
    (IpmiVersion >> 4) & 0xf, 
    IpmiVersion & 0xf
    );

  HiiSetString (HiiHandle, TokenToUpdate, TempString, NULL);
  return;
}

VOID
EFIAPI
UpdateBmcLanConfig (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{ 
  UINTN               DataSize; 
 
  UpdateBmcFirmwareVersionValue (HiiHandle, STRING_TOKEN(STR_BMC_FIRMWARE_REVISION_VALUE));
  UpdateBmcStatusValue (HiiHandle, STRING_TOKEN(STR_BMC_STATUS_VALUE));
  UpdateBmcSelfTestStatusValue(HiiHandle, STRING_TOKEN(STR_BMC_SELF_TEST_STATUS_VALUE));
  UpdateIPMIVersionValue(HiiHandle, STRING_TOKEN(STR_IPMI_REVISION_VALUE));

  
  InitFruStrings (HiiHandle);
  DataSize = sizeof(BMC_LAN_CONFIG);
  BmcCfgGetVariable(mBmcLanVarStoreName, &mSystemConfigurationGuid, NULL, &DataSize, &mBmcLanConfig);
  gBS->CloseEvent (Event);
}
  



EFI_STATUS
SendConfigInfoToBmc (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  EFI_STATUS      Status;
  UINT8           CmdDataBuff[10];
  UINT8           CmdDataSize;
  UINT8           ResponseBuff[34];
  UINT8           ResponseSize;
  UINTN           VarSize;
  BMC_SETUP_DATA  BmcSetupData = {0};
  EFI_GUID        SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  UINT8           ResponseData;
  UINT8           NCSICommand[2];
  UINT8                 ResponseDataSize;
  
  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &BmcSetupData
                  );

  CmdDataBuff[0] = BmcSetupData.PowerRestorePolicy&0x03;
  CmdDataSize = 1;
  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);
  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_CHASSIS,
             EFI_CHASSIS_SET_POWER_RESTORE_POLICY,
             CmdDataBuff,
             CmdDataSize,
             ResponseBuff,
             &ResponseSize
             );

  DEBUG ((EFI_D_INFO, "Set power restore policy:%r,PowerRestorePolicy:%x\n", Status,BmcSetupData.PowerRestorePolicy));

/*  CmdDataBuff[0] = 0;
  CmdDataBuff[1] = BmcSetupData.FanProfile;
  ResponseSize = sizeof (ResponseBuff);
  Status = EfiSendCommandToBMC (
             SM_HYGON_NETFN_APP,
             HYGON_SET_FUN_CONTROL_POLICY,
             CmdDataBuff,
             2,
             ResponseBuff,
             &ResponseSize
             );
  DEBUG ((EFI_D_INFO, "Set FAN:%r,FanProfile:%x\n", Status,BmcSetupData.FanProfile));
  if (!EFI_ERROR (Status) && (BmcSetupData.FanProfile == 0)) {// Manual Mode
    CmdDataBuff[0] = 0xff; // All Fans
    CmdDataBuff[1] = BmcSetupData.FanSpeedControl;
    ResponseSize = sizeof (ResponseBuff);
    Status = EfiSendCommandToBMC (
               SM_HYGON_NETFN_APP,
               HYGON_SET_FUN_SPEED,
               CmdDataBuff,
               2,
               ResponseBuff,
               &ResponseSize
               );
    DEBUG ((EFI_D_INFO, "Set FAN:%r,FanSpeedControl:%d%%\n", Status,BmcSetupData.FanSpeedControl));
  }
*/


  if(BmcSetupData.NCSI == 0){  // disable NCSI
  	NCSICommand[0] = 0;
  	NCSICommand[1] = 0;
  	Status = EfiSendCommandToBMC (
                  0x3c,
                  0x04,
                  (UINT8 *) &NCSICommand,
                  sizeof(NCSICommand),
                  (UINT8 *)&ResponseData,
                  (UINT8 *)&ResponseDataSize
                  );
  	}else{ // enable NCSI
  	NCSICommand[0] = 0;
  	NCSICommand[1] = 1;
  	Status = EfiSendCommandToBMC (
                  0x3c,
                  0x04,
                  (UINT8 *) &NCSICommand,
                  sizeof(NCSICommand),
                  (UINT8 *)&ResponseData,
                  (UINT8 *)&ResponseDataSize
                  );
	}
	DEBUG((EFI_D_INFO,"Set NCSI :%r\n",Status));

  /*if(BmcSetupData.SerialOverLanEnable){
  	SetSolEnable(TRUE);
  }else{
  	SetSolEnable(FALSE);
  }*/
  return Status;
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


EFI_STATUS
IpmiGetSetShareLinkStatus (
  IN UINT8    *Data,
  IN BOOLEAN  IsSet
  )
{                                       //+TangYan+
  EFI_STATUS  Status;
  UINT8       ResponseBuff[2];
  UINT8       ResponseSize;
  UINT8       CmdDataBuff[2];
  UINT8       CmdDataSize;

  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);

  CmdDataSize = sizeof (CmdDataBuff);
  SetMem (CmdDataBuff, CmdDataSize, 0);
  if (IsSet) {
    CmdDataBuff[0] = 1;
    CmdDataBuff[1] = *Data;
  }

  Status = EfiSendCommandToBMC (
             SM_BYOSOFT_NETFN_APP,
             SM_BYOSOFT_SHARE_LINK_FUN,
             CmdDataBuff,
             CmdDataSize,
             (UINT8 *)&ResponseBuff,
             (UINT8 *)&ResponseSize
             );
  DEBUG ((EFI_D_INFO, "Get/Set Share Link Status cmd:%r\n", Status));
  if (!EFI_ERROR (Status) && (IsSet == FALSE)) {
    CopyMem (Data, ResponseBuff, 1);
  }
  return Status;
}


/*EFI_STATUS
IpmiGetVLanStatus (
  IN UINT8   *Data
  )
{
  EFI_STATUS  Status;
  UINT8       ResponseBuff[4];
  UINT8       ResponseSize;
  UINT8       CmdDataBuff[4];
  UINT8       CmdDataSize;

  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);
  
  CmdDataBuff[0] = BASE_BOARD_NIC_CHANNEL_SEL;
  CmdDataBuff[1] = VLAN_PARM;
  CmdDataBuff[2] = 0;
  CmdDataBuff[3] = 0;
  CmdDataSize = 4;

  Status = EfiSendCommandToBMC (
             EFI_SM_NETFN_TRANSPORT,
             EFI_TRANSPORT_GET_LAN_CONFIG_PARAMETERS,
             CmdDataBuff,
             CmdDataSize,
             (UINT8 *)&ResponseBuff,
             (UINT8 *)&ResponseSize
             );
  DEBUG ((EFI_D_INFO, "Get VLAN Status cmd:%r\n", Status));
  if (!EFI_ERROR (Status)) {
    CopyMem (Data, ResponseBuff, 3);
    DEBUG ((EFI_D_INFO, "data:%x %x %x\n", ResponseBuff[0],ResponseBuff[1], ResponseBuff[2]));
  }

  return Status;
}


void
SetVLanState (
  IN BMC_SETUP_DATA  *SetupData
  )
{
  EFI_STATUS      Status;
  UINT8           CmdDataBuff[4];
  UINT8           CmdDataSize;
  UINT8           ResponseBuff[10];
  UINT8           ResponseSize;

  IpmiGetVLanStatus (ResponseBuff);
  DEBUG ((EFI_D_INFO, "SetupData->VLanState:%x ResponseBuff[2]:%x\n", SetupData->VLanState, ResponseBuff[2]));
  if (SetupData->VLanState != (ResponseBuff[2] >> 7)) {
    CmdDataBuff[0] = BASE_BOARD_NIC_CHANNEL_SEL;
    CmdDataBuff[1] = VLAN_PARM;
    CmdDataBuff[2] = (UINT8)(SetupData->VLanID);
    CmdDataBuff[3] = ((SetupData->VLanID >> 8) & 0xF);
    if (SetupData->VLanState) {
      CmdDataBuff[3] |= BIT7;
    }
    CmdDataSize = 4;

    ResponseSize = sizeof (ResponseBuff);
    SetMem (ResponseBuff, ResponseSize, 0);

    Status = EfiSendCommandToBMC (
               EFI_SM_NETFN_TRANSPORT,
               EFI_TRANSPORT_SET_LAN_CONFIG_PARAMETERS,
               CmdDataBuff,
               CmdDataSize,
               (UINT8 *)&ResponseBuff,
               (UINT8 *)&ResponseSize
               );
    DEBUG ((EFI_D_INFO, "Set VLAN Status cmd:%r\n", Status));
  }
}


void
SetShareLinkState (
  IN BMC_SETUP_DATA  *SetupData
  )
{
  UINT8   ShareLinkState;
  
  IpmiGetSetShareLinkStatus (&ShareLinkState, FALSE);
  if (SetupData->ShareLink != ShareLinkState) {
    ShareLinkState = SetupData->ShareLink;
    IpmiGetSetShareLinkStatus (&ShareLinkState, TRUE);
  }
}
*/

EFI_STATUS
NotifySaveCallBack (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
)
{                                       //+TangYan+
  EFI_STATUS      Status;
  UINTN           VarSize;
  BMC_SETUP_DATA  SetupData;
  
  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &mSystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &SetupData
                  );
  ASSERT_EFI_ERROR (Status);

 // SetVLanState (&SetupData);
 // SetShareLinkState (&SetupData);
  
  return EFI_SUCCESS;
}


SETUP_SAVE_NOTIFY_PROTOCOL  mSetupSaveNotify;
EFI_STATUS
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
  UINTN                       VarSize;
  BMC_SETUP_DATA              SetupData = {0};
  EFI_GUID                    SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  UINT8                       ChassisStaus[5];
  UINT8                       PowerPolicy;
 // UINT8                       State;  //+TangYan+
//  UINT16                      VLanID;
//  UINT8                       VLanState[3];
  BOOLEAN                     NeedSetVariable = FALSE;
  UINT8                       ResponseBuff[3];
  UINT8                       ResponseSize;
 // UINT8                       ResponseBuff1[16];
 // UINT8                       ResponseSize1;
  UINT8                       Commanddata[3]={0,0,0};

  DEBUG((EFI_D_ERROR, "%a()\n", __FUNCTION__));

  ResponseSize = sizeof (ResponseBuff);
  SetMem (ResponseBuff, ResponseSize, 0);
  //
  // Initialize COM layer Library as we will use IPMI COM Library functions.
  //
  EfiInitializeIpmiBase ();

  VarSize = sizeof (BMC_SETUP_DATA);
  Status = gRT->GetVariable (
                  BMC_SETUP_VARIABLE_NAME,
                  &SystemConfigurationGuid,
                  NULL,
                  &VarSize,
                  &SetupData
                  );
  IpmiGetChassisStatus (ChassisStaus, sizeof (ChassisStaus));

 /* ResponseSize1 = sizeof (ResponseBuff1);
  SetMem (ResponseBuff1, ResponseSize1, 0);
  Status = EfiSendCommandToBMC (
					  0x6,
					  0x1,
					  (UINT8 *)NULL,
					  0,
					  (UINT8 *)&ResponseBuff1,
					  (UINT8 *)&ResponseSize1
					  );*/
 // if(ResponseBuff1[2]>3){
    Status = EfiSendCommandToBMC (
              0x32,
              0x72,
              Commanddata,
              3,
              (UINT8 *)&ResponseBuff,
              (UINT8 *)&ResponseSize
              );
    DEBUG((EFI_D_INFO,"@@@@@@@@@@@respond data is %d %d \n",ResponseBuff[0],ResponseBuff[1]));
    if(ResponseBuff[1]==0){
      SetupData.NCSI = 0;
      Status = gRT->SetVariable (
                BMC_SETUP_VARIABLE_NAME,
                &SystemConfigurationGuid,
                BMC_SETUP_VARIABLE_FLAG,
                VarSize,
                &SetupData
                );
    
    }else{
      SetupData.NCSI = 1;
      Status = gRT->SetVariable (
                BMC_SETUP_VARIABLE_NAME,
                &SystemConfigurationGuid,
                BMC_SETUP_VARIABLE_FLAG,
                VarSize,
                &SetupData
                );
    }
 // }
  // [6:5] - power restore policy
  PowerPolicy = (ChassisStaus[0] >> 5) & 0x03;
  DEBUG ((EFI_D_INFO, "BMC ChassisStaus[0]:%x, PowerPolicy:%x,SetupData.PowerRestorePolicy:%x\n", ChassisStaus[0], PowerPolicy,SetupData.PowerRestorePolicy));
  if (SetupData.PowerRestorePolicy != PowerPolicy) {
     SetupData.PowerRestorePolicy = PowerPolicy;
     Status = gRT->SetVariable (
                     BMC_SETUP_VARIABLE_NAME,
                     &SystemConfigurationGuid,
                     BMC_SETUP_VARIABLE_FLAG,
                     VarSize,
                     &SetupData
                     );

  }
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

  HiiHandle = HiiAddPackages (
               &gSetupBmcCfgPkgListGuid,
               DriverHandle,
               BmcConfigBin,
               SetupBmcCfgStrings,
               NULL
               );
  ASSERT (HiiHandle != NULL);

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

//  Status = gBS->RegisterProtocolNotify (
//                   &gUpdateBmcLanConfigNotifyGuid,
//                   mEvent,
//                   &mEventReg
//                    );
//  ASSERT_EFI_ERROR (Status);

  ZeroMem(&mSetupSaveNotify, sizeof(SETUP_SAVE_NOTIFY_PROTOCOL));
  mSetupSaveNotify.SaveValue = SendConfigInfoToBmc;
  Status = gBS->InstallProtocolInterface (
                  &mSetupSaveNotify.DriverHandle,
                  &gSetupSaveNotifyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSetupSaveNotify
                  );

/*  Status = IpmiGetSetShareLinkStatus (&State, FALSE);  //+TangYan+
  DEBUG ((EFI_D_INFO, "ShareLinkState:%x,Status:%r\n", State,Status));
  if (!EFI_ERROR (Status)){ 
    if (SetupData.ShareLink != State) {
      SetupData.ShareLink = State;
      NeedSetVariable = TRUE;
    }
  }
  Status = IpmiGetVLanStatus (VLanState);
  if (!EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "VLan:%x %x %x\n", VLanState[0], VLanState[1], VLanState[2]));
    State = VLanState[2] >> 7;
    DEBUG ((EFI_D_INFO, "VLanState:%x\n", State));
    if (SetupData.VLanState != State) {
      SetupData.VLanState = State;
      NeedSetVariable = TRUE;
    }
    VLanID = VLanState[1] | ((VLanState[2] & 0xF) << 8);
    DEBUG ((EFI_D_INFO, "VLanID:%x\n", VLanID));
    if (SetupData.VLanID != VLanID) {
      SetupData.VLanID = VLanID;
      NeedSetVariable = TRUE;
    }
  }*/
  
  return EFI_SUCCESS;
}

