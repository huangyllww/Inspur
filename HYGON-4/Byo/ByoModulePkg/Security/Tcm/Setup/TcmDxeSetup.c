/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  TcmDxe.c

Abstract:
  Dxe part of TCM Module.

Revision History:

Bug 3341 - Add retry mechanism for more tcm command to improve stability.
TIME: 2012-02-03
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. Max retry count is 3.
  2. remove all ASSERT() in TcmDxe.c
$END------------------------------------------------------------

bug 3312 - update tcm settings.
TIME: 2012-01-16
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. Update Tcm.BsCap.HashAlgorithmBitmap to 0x0D.
  2. Always allocate Acpi EventLog memory even if tcm is deactivated.
$END------------------------------------------------------------

Bug 3282 - improve TCM action stability.
TIME: 2012-01-06
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. update command flow as porting guide request.
  2. use retry mechanism.
$END------------------------------------------------------------

Bug 3223 - package ZTE SM3 hash source to .efi for ZTE's copyrights.
TIME: 2011-12-16
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. use ppi or protocol to let hash be independent.
$END------------------------------------------------------------

Bug 3216 - add Tcm SW SM3 hash support.
TIME: 2011-12-13
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. Use ZTE lib to do sm3 hash.
$END------------------------------------------------------------

Bug 3144 - Add Tcm Measurement Architecture.
TIME: 2011-11-24
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. PEI: Measure CRTM Version.
          Measure Main Bios.
  2. DXE: add 'TCPA' acpi table.
          add event log.
          Measure Handoff Tables.
          Measure All Boot Variables.
          Measure Action.
  Note: As software of SM3's hash has not been implemented, so hash
        function is invalid.
$END------------------------------------------------------------

Bug 3075 - Add TCM support.
TIME: 2011-11-14
$AUTHOR: Zhang Lin
$REVIEWERS:
$SCOPE: SugarBay
$TECHNICAL:
  1. Tcm module init version.
     Only support setup function.
$END------------------------------------------------------------

**/



#include "TcmSetup.h"
#include <Library/SafePrintLib.h>
#include <Library/SafeMemLib.h>
#include <Library/PcdLib.h>

#define TCM_MEM_LOG(x)

//--------------------------------------------------------------
HII_VENDOR_DEVICE_PATH  mTcmHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    TCM_SETUP_CONFIG_GUID
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

STATIC CHAR16 *gTcmSetupConfigName  = L"TcmSetup";
STATIC CHAR16 *gTcmStateConfigName  = L"TcmState";

//STATIC CHAR16 *gBootVarName = L"BootOrder";

TCM_SETUP_PRIVATE_DATA  gTcmSetupData = {
  TCM_SETUP_CTX_SIGNATURE,
  NULL,                                       // TcmHandle
  NULL,                                       // DriverHandle
  NULL,                                       // HiiHandle
  {                                           // EFI_HII_CONFIG_ACCESS_PROTOCOL
    TcmFormExtractConfig,
    TcmFormRouteConfig,
    TcmFormCallback
  },
  {0,}                                        // TCM_SETUP_CONFIG
};


//--------------------------------------------------------------














/**
  Set Tcm State option into variable space.

  @param[in] VarValue              The option of Tcm state.

  @retval    EFI_SUCCESS           The operation is finished successfully.
  @retval    Others                Other errors as indicated.

**/
EFI_STATUS
SaveTcmSetupVariable (
  IN TCM_STATE_CONFIG        *TcmState
  )
{
  EFI_STATUS       Status;
  UINTN            VarSize; 
  TCM_SETUP_CONFIG *TcmSetup;

  DEBUG((EFI_D_INFO,"SaveTcmSetupVariable\n"));

  VarSize = sizeof(TCM_SETUP_CONFIG);
  TcmSetup = AllocateZeroPool(VarSize);
  Status = gRT->GetVariable (
                  gTcmSetupConfigName,
                  &gTcmSetupConfigGuid,
                  NULL,
                  &VarSize,
                  TcmSetup
                  );
  if (!EFI_ERROR(Status)) {
    TcmSetup->TcmEnableCmd = TcmState->TcmEnableStatus;
  }

  //Clear TCM pending operation 
  TcmSetup->TcmPendingOperationCmd = KEY_PENDING_NONE;

  Status = gRT->SetVariable (
                  gTcmSetupConfigName,
                  &gTcmSetupConfigGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  VarSize,
                  TcmSetup
                  );
  if(EFI_ERROR(Status)) {
    DEBUG((EFI_D_ERROR,"TCM Setup SetVariable Status = %x\n",Status));
  }

  if (TcmSetup != NULL) {
    FreePool (TcmSetup);
  }
  return Status;
}

/**
  This function allows the caller to request the current
  configuration for one or more named elements. The resulting
  string is in <ConfigAltResp> format. Any and all alternative
  configuration strings shall also be appended to the end of the
  current configuration string. If they are, they must appear
  after the current configuration. They must contain the same
  routing (GUID, NAME, PATH) as the current configuration string.
  They must have an additional description indicating the type of
  alternative configuration the string represents,
  "ALTCFG=<StringToken>". That <StringToken> (when
  converted from Hex UNICODE to binary) is a reference to a
  string in the associated string pack.

  @param[in] This       Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in] Request    A null-terminated Unicode string in
                        <ConfigRequest> format. Note that this
                        includes the routing information as well as
                        the configurable name / value pairs. It is
                        invalid for this string to be in
                        <MultiConfigRequest> format.
  @param[out] Progress  On return, points to a character in the
                        Request string. Points to the string's null
                        terminator if request was successful. Points
                        to the most recent "&" before the first
                        failing name / value pair (or the beginning
                        of the string if the failure is in the first
                        name / value pair) if the request was not
                        successful.
  @param[out] Results   A null-terminated Unicode string in
                        <ConfigAltResp> format which has all values
                        filled in for the names in the Request string.
                        String to be allocated by the called function.

  @retval EFI_SUCCESS             The Results string is filled with the
                                  values corresponding to all requested
                                  names.
  @retval EFI_OUT_OF_RESOURCES    Not enough memory to store the
                                  parts of the results that must be
                                  stored awaiting possible future
                                  protocols.
  @retval EFI_INVALID_PARAMETER   For example, passing in a NULL
                                  for the Request parameter
                                  would result in this type of
                                  error. In this case, the
                                  Progress parameter would be
                                  set to NULL.
  @retval EFI_NOT_FOUND           Routing data doesn't match any
                                  known driver. Progress set to the
                                  first character in the routing header.
                                  Note: There is no requirement that the
                                  driver validate the routing data. It
                                  must skip the <ConfigHdr> in order to
                                  process the names.
  @retval EFI_INVALID_PARAMETER   Illegal syntax. Progress set
                                  to most recent & before the
                                  error or the beginning of the
                                  string.
  @retval EFI_INVALID_PARAMETER   Unknown name. Progress points
                                  to the & before the name in
                                  question.Currently not implemented.
**/
EFI_STATUS
EFIAPI
TcmFormExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{

  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  TCM_STATE_CONFIG                 *IfrData;
  TCM_SETUP_PRIVATE_DATA           *Private;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;
  UINTN                            NumberofPrinted;

  IfrData          = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Status           = EFI_SUCCESS;
  Private          = NULL;

  if (Progress == NULL || Results == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto ProcExit;
  }

  *Progress = Request;
  if((Request != NULL) && !HiiIsConfigHdrMatch(Request, &gTcmSetupConfigGuid, gTcmStateConfigName)){
    Status = EFI_NOT_FOUND;
    goto ProcExit;
  }

  ConfigRequestHdr = NULL;
  Size             = 0;

  Private = TCM_SETUP_DATA_FROM_THIS_HII_CONFIG(This);
  IfrData = AllocateZeroPool(sizeof(TCM_STATE_CONFIG));
  if(IfrData == NULL){
    Status = EFI_OUT_OF_RESOURCES;
    goto ProcExit;
  }


  Status = CopyMemBS (IfrData, sizeof (TCM_STATE_CONFIG), &Private->ConfigData, sizeof (TCM_STATE_CONFIG));
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "%a line %d CopyMemBS failed.\n", __FUNCTION__, __LINE__));
  }

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  BufferSize = sizeof(TCM_STATE_CONFIG);
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr(&gTcmSetupConfigGuid,
                                             gTcmStateConfigName,
                                             Private->DriverHandle);
    Size = (StrLen(ConfigRequestHdr) + 32 + 1) * sizeof(CHAR16);
    ConfigRequest = AllocateZeroPool(Size);
    if(ConfigRequest == NULL){
      Status = EFI_OUT_OF_RESOURCES;
      goto ProcExit;
    }
    AllocatedRequest = TRUE;
    NumberofPrinted = UnicodeSPrintBS (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    if (NumberofPrinted == 0) {
      DEBUG ((EFI_D_INFO, "%a line %d UnicodeSPrintBS failed.\n", __FUNCTION__, __LINE__));
    }
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


/**
  This function applies changes in a driver's configuration.
  Input is a Configuration, which has the routing data for this
  driver followed by name / value configuration pairs. The driver
  must apply those pairs to its configurable storage. If the
  driver's configuration is stored in a linear block of data
  and the driver's name / value pairs are in <BlockConfig>
  format, it may use the ConfigToBlock helper function (above) to
  simplify the job. Currently not implemented.

  @param[in]  This           Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Configuration  A null-terminated Unicode string in
                             <ConfigString> format.
  @param[out] Progress       A pointer to a string filled in with the
                             offset of the most recent '&' before the
                             first failing name / value pair (or the
                             beginning of the string if the failure
                             is in the first name / value pair) or
                             the terminating NULL if all was
                             successful.

  @retval EFI_SUCCESS             The results have been distributed or are
                                  awaiting distribution.
  @retval EFI_OUT_OF_RESOURCES    Not enough memory to store the
                                  parts of the results that must be
                                  stored awaiting possible future
                                  protocols.
  @retval EFI_INVALID_PARAMETERS  Passing in a NULL for the
                                  Results parameter would result
                                  in this type of error.
  @retval EFI_NOT_FOUND           Target for the specified routing data
                                  was not found.
**/
EFI_STATUS
EFIAPI
TcmFormRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{

  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  TCM_STATE_CONFIG                 *IfrData;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if (!HiiIsConfigHdrMatch (Configuration, &gTcmSetupConfigGuid, gTcmStateConfigName)) {
    *Progress = Configuration;
    return EFI_NOT_FOUND;
  }

  IfrData = AllocateZeroPool(sizeof(TCM_STATE_CONFIG));
  //
  // Map the Configuration to the configuration block.
  //
  BufferSize = sizeof (TCM_STATE_CONFIG);
  Status = gHiiConfigRouting->ConfigToBlock (
                                gHiiConfigRouting,
                                Configuration,
                                (UINT8 *)IfrData,
                                &BufferSize,
                                Progress
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Store Buffer Storage back to EFI variable if needed
  //
  Status = SaveTcmSetupVariable (IfrData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  *Progress = Configuration + StrLen (Configuration);

  if (IfrData != NULL) {
    FreePool (IfrData);
  }

  return EFI_SUCCESS;

}


/**
  This function is called to provide results data to the driver.
  This data consists of a unique key that is used to identify
  which data is either being passed back or being asked for.

  @param[in]  This               Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param[in]  Action             Specifies the type of action taken by the browser.
  @param[in]  QuestionId         A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect. The format of the data tends to
                                 vary based on the opcode that generated the callback.
  @param[in]  Type               The type of value for the question.
  @param[in]  Value              A pointer to the data being sent to the original
                                 exporting driver.
  @param[out]  ActionRequest     On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.Currently not implemented.
  @retval EFI_INVALID_PARAMETERS Passing in wrong parameter.
  @retval Others                 Other errors as indicated.
**/
EFI_STATUS
EFIAPI
TcmFormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS              Status;
  TCM_SETUP_PRIVATE_DATA  *Private;
  TCM_STATE_CONFIG        *ConfigData;
  UINT8                   TcmState;

  BOOLEAN          TcmEnableStatus;
  BOOLEAN          TcmActivated;

  Private    = NULL;
  ConfigData = NULL;
  Status     = EFI_SUCCESS;


  if ((This == NULL) || (Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Private    = TCM_SETUP_DATA_FROM_THIS_HII_CONFIG(This);
  ConfigData = &Private->ConfigData;

  DEBUG((EFI_D_INFO,"TCM_DEBUG : Tcm browser Action = %x\n", Action));

  if(Action == EFI_BROWSER_ACTION_CHANGED) {
    Status = GetTcmState(Private->TcmHandle, &TcmEnableStatus, &TcmActivated, NULL, NULL, NULL);

    switch(QuestionId){
    case KEY_TCM_ENABLE:
      TcmState = Value->u8;
      if (TcmEnableStatus != (BOOLEAN)TcmState) {
        Status = EnableTCMDevice((BOOLEAN)TcmState);
        if(EFI_ERROR(Status)){
          ConfigData->TcmEnableStatus = !TcmState;
          Value->u8 = !TcmState;
          DEBUG((EFI_D_ERROR,"TCM_DEBUG : Tcm Device Enable/Disable set failed = %x\n", Status));
        } else {
          ConfigData->TcmEnableStatus = ((BOOLEAN)TcmState);
        }
      }
      break;

    default:
      break;
    }

    switch(QuestionId){  
    case KEY_TCM_OPERATION:
      switch(Value->u8){
      case KEY_PENDING_NONE:
        break;

      case KEY_PENDING_ACTIVE:
      case KEY_PENDING_DEACTIVE:
        TcmState = (BOOLEAN)(Value->u8 & BIT0);
        ConfigData->TcmActive = TcmState;
        Status = ActivateTCMDevice(TcmState);
        if(EFI_ERROR(Status)){
          if (Value->u8 == KEY_PENDING_ACTIVE) {
            ConfigData->TcmActive = KEY_PENDING_DEACTIVE;
          } else {
            ConfigData->TcmActive = KEY_PENDING_ACTIVE;
          } 
          DEBUG((EFI_D_ERROR,"TCM_DEBUG : Tcm Activated/Deactivated set failed = %x\n", Status));
        }

        break;

      case KEY_PENDING_CLEAR:
        //Need to make sure Tcm state is activated before force clear, otherwise the force clear command would response error 
        if (TcmActivated == FALSE) {
          Status = ActivateTCMDevice(TRUE);
          if(EFI_ERROR(Status)){
            DEBUG((EFI_D_ERROR,"TCM_DEBUG : Tcm Activated/Deactivated set failed = %x\n", Status));
          }
        }
        Status = TcmForceClear();
        if(!EFI_ERROR(Status)){
          if(ConfigData->TcmActive){
            ConfigData->TcmActive = 0; //Deactivate
          }
          if(ConfigData->TcmEnableStatus){
            ConfigData->TcmEnableStatus = 0; //Disable
          }

        } else {
          DEBUG((EFI_D_ERROR,"TCM_DEBUG : Tcm Clear set failed = %x\n", Status));
        }

        break;

      default:
        break;
      }
      break;

    default:
      break;
    }

    HiiSetBrowserData(&gTcmSetupConfigGuid,
                    gTcmStateConfigName,
                    sizeof(TCM_STATE_CONFIG),
                    (UINT8 *)ConfigData,
                    NULL
                    );
  }
  return EFI_SUCCESS;
}

/**
  Collect TCM state for TCM hardware or TcmSetup variable
 
  @param[in/out]       TcmHandle
  @param[in/out]       TcmState           TCM configuration.
  @param[in/out]       TcmSetup           TcmCmd
  @return BOOLEAN      TcmEnable Current  Status
 */
BOOLEAN 
CollectTcmState (
  IN EFI_TCM_HANDLE        TcmHandle,
  IN OUT TCM_STATE_CONFIG  *TcmState,
  IN OUT TCM_SETUP_CONFIG  *TcmSetup
  )
{
  EFI_STATUS                    Status;
  UINTN                         VarSize;
  BOOLEAN                       TcmEnableStatus;
  BOOLEAN                       TcmActivated;

  DEBUG((EFI_D_INFO,"CollectTcmState Entry \n"));
  //Get TCM device status
  TcmState->TcmPresent     = TRUE;
  Status = GetTcmState(gTcmSetupData.TcmHandle, &TcmEnableStatus, &TcmActivated, NULL, NULL, NULL);
  DEBUG((EFI_D_INFO,"GetTcmState TcmEnableStatus - %x GetTcmState TcmActivated - %x\n",TcmEnableStatus,TcmActivated));
  if(EFI_ERROR(Status)){
    gTcmSetupData.ConfigData.TcmPresent      = FALSE;
    gTcmSetupData.ConfigData.TcmEnableStatus = FALSE;
    gTcmSetupData.ConfigData.TcmActive       = FALSE;
  } else {
    gTcmSetupData.ConfigData.TcmActive       = TcmActivated;
    gTcmSetupData.ConfigData.TcmEnableStatus = TcmEnableStatus;
  }
  //Get TCM setup NV variable
  Status  = EFI_SUCCESS;
  VarSize = sizeof(TCM_SETUP_CONFIG);
  Status  = gRT->GetVariable (
                  gTcmSetupConfigName,
                  &gTcmSetupConfigGuid,
                  NULL,
                  &VarSize,
                  TcmSetup
                  );
    if (EFI_ERROR(Status)) {
      TcmSetup->TcmEnableCmd = TcmEnableStatus;
      TcmSetup->TcmPendingOperationCmd = KEY_PENDING_NONE;
      DEBUG((EFI_D_ERROR,"TCM_DEBUG : GetVariable return error %x\n", Status));
      //Setup a default variable according to TPM status when it's not exist
      Status = gRT->SetVariable (
                      gTcmSetupConfigName,
                      &gTcmSetupConfigGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      VarSize,
                      TcmSetup
                      );
      if(EFI_ERROR(Status)){
        DEBUG((EFI_D_ERROR,"TCM_DEBUG : TcmSetup variable set failed %x\n", Status));
      }
    }
  return TcmEnableStatus;
}

/**
  Handle TCM pending operations. 
 
  @param[in/out]   TcmState        TCM configuration.
  @param[in/out]   TcmSetup        TCMCmd
  @return EFI_STATUS 
 */
EFI_STATUS
TcmSetupPendingOperation( 
  IN OUT TCM_STATE_CONFIG  *TcmState,
  IN OUT TCM_SETUP_CONFIG  *TcmSetup
  )
{

  EFI_STATUS                    Status;
  BOOLEAN                       TcmCommandStatus = TRUE; 
  BOOLEAN                       VariableSetRequired = FALSE; 
  UINTN                         VarSize;

  DEBUG((EFI_D_INFO,"TcmSetupPendingOperation Entry \n"));

  Status = EFI_SUCCESS;

  if ((TcmSetup->TcmEnableCmd != TcmState->TcmEnableStatus) || (TcmSetup->TcmPendingOperationCmd != KEY_PENDING_NONE)) {

    if (TcmSetup->TcmEnableCmd != TcmState->TcmEnableStatus) {
      Status = EnableTCMDevice((BOOLEAN)TcmSetup->TcmEnableCmd);
      if(EFI_ERROR(Status)){
        DEBUG((EFI_D_ERROR,"TCM_DEBUG : EnableTCMDevice return error %x\n", Status));
        TcmSetup->TcmEnableCmd = TcmState->TcmEnableStatus;
        VariableSetRequired = TRUE;
      } else {
          //Update TCM enable status when system reset is not required
          TcmState->TcmEnableStatus = TcmSetup->TcmEnableCmd;
        }
      }
    }

    if (TcmSetup->TcmPendingOperationCmd != KEY_PENDING_NONE) {
      switch (TcmSetup->TcmPendingOperationCmd) {
        case KEY_PENDING_ACTIVE:
          if(TcmState->TcmActive != TRUE){
            Status = ActivateTCMDevice(TcmSetup->TcmPendingOperationCmd & BIT0);
            if(EFI_ERROR(Status)){
              DEBUG((EFI_D_ERROR,"TCM_DEBUG : ActivateTCMDevice return error %x\n", Status));
              TcmCommandStatus = FALSE;
            } else {
              //Update TCM activate status when system reset is not required
              TcmState->TcmActive = TRUE;      
              TcmSetup->TcmPendingOperationCmd = KEY_PENDING_NONE;  
            }
          }
          break;

        case KEY_PENDING_DEACTIVE:
          if(TcmState->TcmActive != FALSE) {
            Status = ActivateTCMDevice(TcmSetup->TcmPendingOperationCmd & BIT0);
            if(EFI_ERROR(Status)){
              DEBUG((EFI_D_ERROR,"TCM_DEBUG : ActivateTCMDevice return error %x\n", Status));
              TcmCommandStatus = FALSE;
            } else {
              //Update TCM activate status when system reset is not required
              TcmState->TcmActive = FALSE;
              TcmSetup->TcmPendingOperationCmd = KEY_PENDING_NONE;
          }
          break;
                
        case KEY_PENDING_CLEAR:
          //Skip Tcm clear command when TCM is disabled and deactivated
          if ((TcmState->TcmEnableStatus == FALSE) && (TcmState->TcmActive == FALSE)) {
            TcmSetup->TcmPendingOperationCmd = KEY_PENDING_NONE;
            break;
          }
          if (TcmState->TcmActive == FALSE) {
            Status = ActivateTCMDevice(TRUE);
            if(EFI_ERROR(Status)){
              DEBUG((EFI_D_ERROR,"TCM_DEBUG : Tcm Activated/Deactivated set failed = %x\n", Status));
              TcmCommandStatus = FALSE;
            }
          }
          Status = TcmForceClear();
          if(EFI_ERROR(Status)){
            DEBUG((EFI_D_ERROR,"TCM_DEBUG : TcmForceClear return error %x\n", Status));
            TcmCommandStatus = FALSE;
          } else {
            TcmSetup->TcmEnableCmd = FALSE;
          }
          break;

          default:
            TcmSetup->TcmPendingOperationCmd = KEY_PENDING_NONE;
            break;
      }

      VariableSetRequired = TRUE;

    }

    //Not clear Tcm pending operation command if command responds failure
    if (TcmCommandStatus) {
      TcmSetup->TcmPendingOperationCmd = KEY_PENDING_NONE;
    } else {
      //Do error handling here if requires, would clear the pending operation by default
      TcmSetup->TcmPendingOperationCmd = KEY_PENDING_NONE;
    }

    if (VariableSetRequired) {
      VarSize = sizeof(TCM_SETUP_CONFIG);
      Status = gRT->SetVariable (
                      gTcmSetupConfigName,
                      &gTcmSetupConfigGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      VarSize,
                      TcmSetup
                      );
      if(EFI_ERROR(Status)){
        DEBUG((EFI_D_ERROR,"TCM_DEBUG : TcmSetup variable set failed %x\n", Status));
      }
    }

  }

  return Status;
}

/**
  This function update the Chinese and English strings for TCM device state.

  @param[in]  HiiHandle  A handle that was previously registered in the HII Database.
  @param[in]  String1Id  The identifier of the string to retrieved from the string
                         package associated with HiiHandle.
  @param[in]  String2Id  If zero, then a new string is created in the
                         String Package associated with HiiHandle.  If
                         non-zero, then the string specified by String2Id
                         is updated in the String Package associated
                         with HiiHandle.
**/
EFI_STATUS
UpdateTcmDeviceState (
  IN EFI_HII_HANDLE    HiiHandle,
  IN EFI_STRING_ID     String1Id,
  IN EFI_STRING_ID     String2Id
  )
{
  EFI_STRING           TcmDeviceState;

  TcmDeviceState = NULL;
  TcmDeviceState = HiiGetString (HiiHandle, String1Id, "en-US");
  HiiSetString (HiiHandle, String2Id, TcmDeviceState, "en-US");
  if(TcmDeviceState != NULL){
    FreePool(TcmDeviceState);
  }
  TcmDeviceState = HiiGetString (HiiHandle, String1Id, "zh-Hans");
  HiiSetString (HiiHandle, String2Id, TcmDeviceState, "zh-Hans");
  if(TcmDeviceState != NULL){
    FreePool(TcmDeviceState);
  }
  return EFI_SUCCESS;
}
/**
  The driver's entry point.
 
  It publishes EFI TCM Protocol.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
TcmSetupEntry (
  IN    EFI_HANDLE       ImageHandle,
  IN    EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS              Status;
  TCM_SETUP_CONFIG        *TcmSetup;
  BOOLEAN                 TcmEnableCurrentStatus;

  TcmSetup = NULL;
  TcmEnableCurrentStatus = FALSE;

  if (!PcdGetBool (PcdTcmChipPresent)) {
    //
    //TCM is not Exist.
    //
    gTcmSetupData.ConfigData.TcmPresent = FALSE;
    DEBUG ((DEBUG_ERROR, "TcmSetup: TCM chip not present!\n"));
  } else {
    //VarSize = sizeof(TCM_SETUP_CONFIG);
    gTcmSetupData.TcmHandle = (EFI_TCM_HANDLE)(UINTN)PcdGet64 (PcdTpmBaseAddress);
    Status = TcmPcRequestUseTcm(gTcmSetupData.TcmHandle);
    DEBUG((EFI_D_INFO,"TcmPcRequestUseTcm - %r\n",Status));
    if(!EFI_ERROR(Status)){
      //Collect Tcm state when its Exist
      gTcmSetupData.ConfigData.TcmPresent = TRUE;
      TcmSetup = AllocateZeroPool (sizeof(TCM_SETUP_CONFIG));
      TcmEnableCurrentStatus = CollectTcmState(gTcmSetupData.TcmHandle, &gTcmSetupData.ConfigData,TcmSetup);

      DEBUG((EFI_D_INFO,"TcmEnableStatus %x TcmActive %x\n", gTcmSetupData.ConfigData.TcmEnableStatus, gTcmSetupData.ConfigData.TcmActive));

      TcmSetupPendingOperation(&gTcmSetupData.ConfigData,TcmSetup);

      if (TcmSetup != NULL) {
        FreePool (TcmSetup);
      }
    } else {
      //
      //TCM is not Exist.
      //
      gTcmSetupData.ConfigData.TcmPresent = FALSE;
    }
  }

// setup.
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &gTcmSetupData.DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mTcmHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &gTcmSetupData.ConfigAccess,
                  NULL
                  );
  if(EFI_ERROR(Status)){
    TCM_MEM_LOG(("L%d:%r", __LINE__, Status));
  }

  gTcmSetupData.HiiHandle = HiiAddPackages (
                         &gTcmSetupConfigGuid,
                         gTcmSetupData.DriverHandle,
                         TcmSetupStrings,
                         TcmSetupBin,
                         NULL
                         );
  if(gTcmSetupData.HiiHandle == NULL){
    TCM_MEM_LOG(("L%d:HiiHandle->NULL", __LINE__));
  }

  if (!gTcmSetupData.ConfigData.TcmPresent) {
    UpdateTcmDeviceState (gTcmSetupData.HiiHandle, STRING_TOKEN (STR_TCM_DEVICE_NOT_PRESENT), STRING_TOKEN (STR_TCM_DEVICE_STATE_CONTENT));
  } else {
    ReportTcm12Info (&gTcmSetupData);
    UpdateTcmDeviceState (gTcmSetupData.HiiHandle, STRING_TOKEN (STR_TCM_DEVICE_ENABLED), STRING_TOKEN (STR_TCM_DEVICE_STATE_CONTENT));
    if (!TcmEnableCurrentStatus) {
      UpdateTcmDeviceState (gTcmSetupData.HiiHandle, STRING_TOKEN (STR_TCM_DEVICE_DISABLED), STRING_TOKEN (STR_TCM_DEVICE_STATE_CONTENT));
    }
  }

  //
  //Remove TCM Packages When PcdTpmAndTcmHasSetupWithoutDevice|FALSE & TCM Chip Not Present.
  //
  if ((!FixedPcdGetBool(PcdTpmAndTcmHasSetupWithoutDevice))&&(!PcdGetBool (PcdTcmChipPresent))){
    DEBUG ((DEBUG_ERROR, "%a %d PcdTpm2ChipPresent == FALSE \n", __FUNCTION__, __LINE__));
    HiiRemovePackages(gTcmSetupData.HiiHandle);
    gBS->UninstallMultipleProtocolInterfaces(
          gTcmSetupData.DriverHandle,
          &gEfiDevicePathProtocolGuid,
          &mTcmHiiVendorDevicePath,
          &gEfiHiiConfigAccessProtocolGuid,
          &gTcmSetupData.ConfigAccess,
          NULL
          );
  }

  DEBUG((EFI_D_INFO,"%a -END\n",__FUNCTION__));

  return EFI_SUCCESS;
}





