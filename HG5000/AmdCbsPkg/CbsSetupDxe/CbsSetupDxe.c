/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 * ***************************************************************************
 */

#include "CbsSetupDxe.h"
#include <Library/AmdPspApobLib.h>
#include <Library/AmdCbsVariable.h>


CHAR16 VariableName[] = CBS_SYSTEM_CONFIGURATION_NAME;


//
// Global Variables.
//

EFI_GUID mFormSetGuid = CBS_SYSTEM_CONFIGURATION_GUID;

HII_VENDOR_DEVICE_PATH  mCbsHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    CBS_SYSTEM_CONFIGURATION_GUID
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

/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.

  @param  This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Request                A null-terminated Unicode string in
                                 <ConfigRequest> format.
  @param  Progress               On return, points to a character in the Request
                                 string. Points to the string's null terminator if
                                 request was successful. Points to the most recent
                                 '&' before the first failing name/value pair (or
                                 the beginning of the string if the failure is in
                                 the first name/value pair) if the request was not
                                 successful.
  @param  Results                A null-terminated Unicode string in
                                 <ConfigAltResp> format which has all values filled
                                 in for the names in the Request string. String to
                                 be allocated by the called function.

  @retval EFI_SUCCESS            The Results is filled with the requested values.
  @retval EFI_OUT_OF_RESOURCES   Not enough memory to store the results.
  @retval EFI_INVALID_PARAMETER  Request is illegal syntax, or unknown name.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.

**/
EFI_STATUS
EFIAPI
ExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  CBS_DXE_PRIVATE_DATA             *Private;
  EFI_HII_CONFIG_ROUTING_PROTOCOL  *HiiConfigRouting;
  EFI_STRING                       ConfigRequestHdr;
  EFI_STRING                       ConfigRequest;
  BOOLEAN                          AllocatedRequest;
  UINTN                            Size;
  CHAR16                           *StrPointer;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Initialize the local variables.
  //
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  Size             = 0;
  *Progress = Request;
  AllocatedRequest = FALSE;

  Private = CBS_DXE_PRIVATE_FROM_THIS (This);
  HiiConfigRouting = Private->HiiConfigRouting;

  //
  // Get Buffer Storage data from EFI variable.
  // Try to get the current setting from variable.
  //
  BufferSize = 0;
  Status = gRT->GetVariable (
            VariableName,
            &gCbsSystemConfigurationGuid,
            NULL,
            &BufferSize,
            Private->IfrData
            );
  if ((Status == EFI_BUFFER_TOO_SMALL) && (BufferSize == CbsVariableSize)) {
    Status = gRT->GetVariable (
              VariableName,
              &gCbsSystemConfigurationGuid,
              NULL,
              &BufferSize,
              Private->IfrData
              );
    if (EFI_ERROR (Status)) {
      // set all default value to &Private->IfrData
      CbsWriteDefalutValue(Private->IfrData);
    }
  } else {
    // set all default value to &Private->IfrData
    CbsWriteDefalutValue(Private->IfrData);
  }
  CbsComboIdentify(Private->IfrData);
  CbsOptionControlDependOnStepping(Private->IfrData);
  BufferSize = CbsVariableSize;
  Status = gRT->SetVariable(
                        VariableName,
                        &gCbsSystemConfigurationGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        BufferSize,
                        Private->IfrData
                        );

  if (Request == NULL) {
    //
    // Request is set to NULL, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (&gCbsSystemConfigurationGuid, VariableName, Private->DriverHandle);
    if (ConfigRequestHdr != NULL) {
      Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
      ConfigRequest = AllocateZeroPool (Size);
      if (ConfigRequest == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      AllocatedRequest = TRUE;
      UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
      FreePool (ConfigRequestHdr);
      ConfigRequestHdr = NULL;
    }
  } else {
    //
    // Check routing data in <ConfigHdr>.
    // Note: if only one Storage is used, then this checking could be skipped.
    //
    if (!HiiIsConfigHdrMatch (Request, &mFormSetGuid, VariableName)) {
      return EFI_NOT_FOUND;
    }
    //
    // Set Request to the unified request string.
    //
    ConfigRequest = Request;
    //
    // Check whether Request includes Request Element.
    //
    if (StrStr (Request, L"OFFSET") == NULL) {
      //
      // Check Request Element does exist in Reques String
      //
      StrPointer = StrStr (Request, L"PATH");
      if (StrPointer == NULL) {
        return EFI_INVALID_PARAMETER;
      }
      if (StrStr (StrPointer, L"&") == NULL) {
        Size = (StrLen (Request) + 32 + 1) * sizeof (CHAR16);
        ConfigRequest = AllocateZeroPool (Size);
        ASSERT (ConfigRequest != NULL);
        AllocatedRequest = TRUE;
        UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", Request, (UINT64) BufferSize);
      }
    }
  }

  Status = HiiConfigRouting->BlockToConfig (
                                HiiConfigRouting,
                                ConfigRequest,
                                Private->IfrData,
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
    *Progress = Request + StrLen (Request);
  }

  return Status;
}

/**
  This function processes the results of changes in configuration.

  @param  This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Configuration          A null-terminated Unicode string in <ConfigResp>
                                 format.
  @param  Progress               A pointer to a string filled in with the offset of
                                 the most recent '&' before the first failing
                                 name/value pair (or the beginning of the string if
                                 the failure is in the first name/value pair) or
                                 the terminating NULL if all was successful.

  @retval EFI_SUCCESS            The Results is processed successfully.
  @retval EFI_INVALID_PARAMETER  Configuration is NULL.
  @retval EFI_NOT_FOUND          Routing data doesn't match any storage in this
                                 driver.

**/
EFI_STATUS
EFIAPI
RouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  EFI_STATUS                       Status;
  UINTN                            BufferSize;
  CBS_DXE_PRIVATE_DATA             *Private;
  EFI_HII_CONFIG_ROUTING_PROTOCOL  *HiiConfigRouting;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  Private = CBS_DXE_PRIVATE_FROM_THIS (This);
  HiiConfigRouting = Private->HiiConfigRouting;
  *Progress = Configuration;

  //
  // Check routing data in <ConfigHdr>.
  // Note: if only one Storage is used, then this checking could be skipped.
  //
  if (!HiiIsConfigHdrMatch (Configuration, &gCbsSystemConfigurationGuid, VariableName)) {
    return EFI_NOT_FOUND;
  }

  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  BufferSize = CbsVariableSize;
  Status = HiiConfigRouting->ConfigToBlock (
                               HiiConfigRouting,
                               Configuration,
                               Private->IfrData,
                               &BufferSize,
                               Progress
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Store Buffer Storage back to EFI variable
  //
  BufferSize = CbsVariableSize;
  Status = gRT->SetVariable(
                  VariableName,
                  &gCbsSystemConfigurationGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  BufferSize,
                  Private->IfrData
                  );
  if (!EFI_ERROR (Status)) {
    AmdSaveCbsConfigData(Private->IfrData);
  }

  return Status;
}

/**
  This function processes the results of changes in configuration.

  @param  This                   Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Action                 Specifies the type of action taken by the browser.
  @param  QuestionId             A unique value which is sent to the original
                                 exporting driver so that it can identify the type
                                 of data to expect.
  @param  Type                   The type of value for the question.
  @param  Value                  A pointer to the data being sent to the original
                                 exporting driver.
  @param  ActionRequest          On return, points to the action requested by the
                                 callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the
                                 variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the
                                 callback.

**/
EFI_STATUS
EFIAPI
FormCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  EFI_STATUS                       Status;
  CBS_DXE_PRIVATE_DATA             *Private;
  UINTN                            BufferSize;

  //Byo porting >> Sync LCFC SVN 42195 
  if ((Action == EFI_BROWSER_ACTION_FORM_OPEN) || (Action == EFI_BROWSER_ACTION_FORM_CLOSE) ||
      (Action == EFI_BROWSER_ACTION_SUBMITTED)) {
  //Byo Porting  <<
    return EFI_SUCCESS;
  }

  if ((Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Type == EFI_IFR_TYPE_STRING) && (Value->string == 0)) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_SUCCESS;
  Private = CBS_DXE_PRIVATE_FROM_THIS (This);

  //
  // Retrive data from Browser
  //
  BufferSize = CbsVariableSize;
  if (!HiiGetBrowserData(&gCbsSystemConfigurationGuid,
                         VariableName,
                         BufferSize,
                         (UINT8*)Private->IfrData)){
    return EFI_NOT_FOUND;
  }

  Status = AmdCbsSetupCallbackRoutine(This, Action, QuestionId, Type, Value, ActionRequest, (UINT8*)Private->IfrData, Private->HiiHandle);
  CbsComboIdentify(Private->IfrData);
  CbsOptionControlDependOnStepping(Private->IfrData);
  if (!EFI_ERROR (Status)) {
    //
    // Pass changed uncommitted data back to Form Browser
    //
    HiiSetBrowserData(&gCbsSystemConfigurationGuid,
                      VariableName,
                      BufferSize,
                      (UINT8 *)Private->IfrData,
                      NULL);
  }

  return Status;
}

/**
  Main entry for this driver.

  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
CbsSetupRecovery (
  CBS_DXE_PRIVATE_DATA            *Private
  )
{
  EFI_STATUS                       Status;
  BOOLEAN                          ApcbRecoveryFlag;
  UINTN                            BufferSize;

  Status = AmdPspGetApobApcbRecoveryFlag (&ApcbRecoveryFlag);
  ASSERT (Status == EFI_SUCCESS);
  if (ApcbRecoveryFlag) {
    DEBUG ((EFI_D_ERROR, "APCB Recovery enabled\n"));
    //
    // Get Buffer Storage data from EFI variable.
    // Try to get the current setting from variable.
    //
    BufferSize = 0;
    Status = gRT->GetVariable (
              VariableName,
              &gCbsSystemConfigurationGuid,
              NULL,
              &BufferSize,
              Private->IfrData
              );
    if ((Status == EFI_BUFFER_TOO_SMALL) && (BufferSize == CbsVariableSize)) {
      Status = gRT->GetVariable (
                VariableName,
                &gCbsSystemConfigurationGuid,
                NULL,
                &BufferSize,
                Private->IfrData
                );
      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR, "Load CBS defaults\n"));
        CbsWriteDefalutValue(Private->IfrData);
      }
    } else {
      DEBUG ((EFI_D_ERROR, "Load CBS defaults\n"));
      CbsWriteDefalutValue(Private->IfrData);
    }

    //
    // Store Buffer Storage back to EFI variable
    //
    BufferSize = CbsVariableSize;
    Status = gRT->SetVariable(
                    VariableName,
                    &gCbsSystemConfigurationGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    BufferSize,
                    Private->IfrData
                    );
    ASSERT (Status == EFI_SUCCESS);
  }

  return Status;
}

VOID
CbsWriteBMCData(
  UINT8 *IfrData
)
{
   CBS_CONFIG *Setup_Config;

   Setup_Config = (CBS_CONFIG *)IfrData;

   Setup_Config->CbsCmnCpuCpb = PcdGet8(PcdCpbMode);
}

/**
  Main entry for this driver.

  @param ImageHandle     Image handle this driver.
  @param SystemTable     Pointer to SystemTable.

  @retval EFI_SUCESS     This function always complete successfully.

**/
EFI_STATUS
EFIAPI
CbsSetupDxeInit (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS                      Status;
  EFI_HII_DATABASE_PROTOCOL       *HiiDatabase;
  EFI_HII_STRING_PROTOCOL         *HiiString;
  EFI_HII_CONFIG_ROUTING_PROTOCOL *HiiConfigRouting;
  CBS_DXE_PRIVATE_DATA            *Private;
  UINTN                           BufferSize;
  EFI_HII_PACKAGE_LIST_HEADER     *HiiPackageList;

  DEBUG ((EFI_D_ERROR, "CbsSetupDxeInit Driver entry\n"));
  Private = AllocateZeroPool (sizeof (CBS_DXE_PRIVATE_DATA));
  if (Private == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->Signature   = CBS_DXE_PRIVATE_SIGNATURE;

  Private->ConfigAccess.ExtractConfig = ExtractConfig;
  Private->ConfigAccess.RouteConfig   = RouteConfig;
  Private->ConfigAccess.Callback      = FormCallback;

  Private->IfrData = AllocateZeroPool (CbsVariableSize);
  ASSERT (Private->IfrData != NULL);

  BufferSize = 0;
  Status = gRT->GetVariable (
              VariableName,
              &gCbsSystemConfigurationGuid,
              NULL,
              &BufferSize,
              Private->IfrData
              );
  if ((Status == EFI_BUFFER_TOO_SMALL) && (BufferSize == CbsVariableSize)) {
    DEBUG ((EFI_D_ERROR, " Get cbs variable(%x)\n", BufferSize));
    Status = gRT->GetVariable (
                  VariableName,
                  &gCbsSystemConfigurationGuid,
                  NULL,
                  &BufferSize,
                  Private->IfrData
                  );
  if(PcdGet8(PcdBmcBiosConfigFlag)){
	//CbsWriteBMCData(Private->IfrData);
  }
//-    if (EFI_ERROR (Status)) {                                                                    // winddy -
if ((EFI_ERROR (Status))|| PcdGet8(PcdIsPlatformCmosBad) || PcdGetBool(PcdFceLoadDefault)) {        // winddy +
      DEBUG ((EFI_D_ERROR, " Set cbs default variable(0x%x - 0x%x)\n", BufferSize, CbsVariableSize));
      CbsWriteDefalutValue(Private->IfrData);
      CbsComboIdentify(Private->IfrData);
	  CbsOptionControlDependOnStepping(Private->IfrData);
  
      //
      // Store Buffer Storage back to EFI variable
      //
      BufferSize = CbsVariableSize;
      Status = gRT->SetVariable(
                      VariableName,
                      &gCbsSystemConfigurationGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      BufferSize,
                      Private->IfrData
                      );
    }
  } else {
    DEBUG ((EFI_D_ERROR, " Set cbs default variable(0x%x - 0x%x)\n", BufferSize, CbsVariableSize));
    CbsWriteDefalutValue(Private->IfrData);
	if(PcdGet8(PcdBmcBiosConfigFlag)){
      CbsWriteBMCData(Private->IfrData);
    }
    CbsComboIdentify(Private->IfrData);
	CbsOptionControlDependOnStepping(Private->IfrData);

    //
    // Store Buffer Storage back to EFI variable
    //
    BufferSize = CbsVariableSize;
    Status = gRT->SetVariable(
                    VariableName,
                    &gCbsSystemConfigurationGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    BufferSize,
                    Private->IfrData
                    );
  }

  CbsComboIdentify(Private->IfrData);
  CbsOptionControlDependOnStepping(Private->IfrData);

  BufferSize = CbsVariableSize;
  Status = gRT->SetVariable(
	      VariableName,
	      &gCbsSystemConfigurationGuid,
	      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
	      BufferSize,
	      Private->IfrData
	      );

  //
  // Locate Hii Database protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Private->HiiDatabase = HiiDatabase;

  //
  // Locate HiiString protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiStringProtocolGuid, NULL, (VOID **) &HiiString);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Private->HiiString = HiiString;

  //
  // Locate ConfigRouting protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &HiiConfigRouting);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Private->HiiConfigRouting = HiiConfigRouting;
  

  // Get the HiiPackageListProtocol from the Image's Handle
  Status = gBS->HandleProtocol(ImageHandle, &gEfiHiiPackageListProtocolGuid, (VOID**)&HiiPackageList);
  if(EFI_ERROR(Status)){
    return Status;
  }
      

 //
  // Install Device Path Protocol and Config Access protocol to driver handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Private->DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mCbsHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &Private->ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  
  Status = HiiDatabase->NewPackageList( 
                  HiiDatabase,
                  HiiPackageList,
                  Private->DriverHandle,
                  &Private->HiiHandle
                  );
  ASSERT_EFI_ERROR (Status);
                  
                                        

  CbsSetupRecovery (Private);

  //
  // Publish our HII data
  //
//  Private->HiiHandle = HiiAddPackages (
//                         &gCbsSystemConfigurationGuid,
//                         Private->DriverHandle,
//                         CbsSetupLibInstanceStrings,
//                         AmdCbsFormBin,
//                         NULL
//                         );
  


  if (Private->HiiHandle == NULL) {
    FreePool(Private);
    return EFI_OUT_OF_RESOURCES;
  }

  return EFI_SUCCESS;
}
