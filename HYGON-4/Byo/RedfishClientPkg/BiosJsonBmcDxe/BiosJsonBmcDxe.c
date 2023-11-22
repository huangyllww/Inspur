/** @file
  This sample application bases on HelloWorld PCD setting
  to print "UEFI Hello World!" to the UEFI Console.

  Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>
  All rights reserved.This software and associated documentation (if any)
  is furnished under a license and may only be used or copied in
  accordance with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be reproduced,
  stored in a retrieval system, or transmitted in any form or by any
  means without the express written consent of Byosoft Corporation.

**/

#include <PiDxe.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IpmiBaseLib.h>
#include <Ipmi/IpmiNetFnAppOemDefinition.h>

#include "FileSupport.h"
#include <RedfishJsonStructure/Bios/v1_0_0/EfiBiosV1_0_0.h>
#include <RedfishJsonStructure/AttributeRegistry/v1_0_0/EfiAttributeRegistryV1_0_0.h>
#include <Protocol/RestJsonStructure.h>
#include <Protocol/ByoFormBrowserEx.h>
#include <Protocol/SetupSaveNotify.h>

#define BIOS_JSON_FILE L"BiosCurrentSetting.json"
#define ATTRIBUTE_REGISTRY_JSON_FILE L"BiosAttributeRegistry.json"
#define REQUEST_USB_OPEN_BUFFER_SIZE   1
#define REQUEST_BUFFER_SIZE       2
#define IPMI_USB_DEVICE_STALL    100000  // 100ms
#define IPMI_USB_DEVICE_TIME_OUT 5000000 // 5s

BOOLEAN mIsReportRedfishData = FALSE;
BOOLEAN mIsEnterReportRedfishData = FALSE;
BOOLEAN mIsResetSystem = FALSE;
BOOLEAN mFirstReportEnterUiApp = FALSE;
BOOLEAN mUsbKeyOpen = FALSE;

SETUP_SAVE_NOTIFY_PROTOCOL  gSetupSaveNotify;

EFI_STATUS
IpmiOpenUsbKey (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT8                             RequestData[REQUEST_BUFFER_SIZE];
  UINT8                             ResponseSize;
  UINT8                             ResponseData;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  ZeroMem (RequestData, REQUEST_BUFFER_SIZE);
  RequestData[0] = SM_BYOSOFT_NETFN_SUB_REDFISH_OPEN_USB_KEY;
  ResponseSize   = 1;

  //
  // Open USB Key
  //
  Status = EfiSendCommandToBMC (
            SM_BYOSOFT_NETFN_APP,
            SM_BYOSOFT_NETFN_SUB_REDFISH,
            (UINT8 *) RequestData,
            REQUEST_USB_OPEN_BUFFER_SIZE,
            &ResponseData,
            &ResponseSize
            );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mUsbKeyOpen = TRUE;
  return EFI_SUCCESS;
}

EFI_STATUS
IpmiQueryUseKey (
  )
{
  EFI_STATUS                        Status;
  UINT8                             RequestData[REQUEST_BUFFER_SIZE];
  UINT8                             ResponseSize;
  UINT8                             ResponseData;
  UINT32                            Retries;

  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  ResponseSize = 1;
  ResponseData = 0xFF;
  Retries      = IPMI_USB_DEVICE_TIME_OUT / IPMI_USB_DEVICE_STALL;

  //
  // Query USB key device until the device is ready
  //
  do {
    RequestData[0] = SM_BYOSOFT_NETFN_SUB_REDFISH_QUERY_USB_KEY;
    Status = EfiSendCommandToBMC (
              SM_BYOSOFT_NETFN_APP,
              SM_BYOSOFT_NETFN_SUB_REDFISH,
              (UINT8 *) RequestData,
              REQUEST_USB_OPEN_BUFFER_SIZE,
              &ResponseData,
              &ResponseSize
              );
    if (!EFI_ERROR (Status)) {
      break;
    }

    gBS->Stall(IPMI_USB_DEVICE_STALL);
  } while (--Retries > 0);

  if (Retries == 0) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
IpmiCloseUseKey (
  UINT8  ReturnStatus
  )
{
  EFI_STATUS                        Status;
  UINT8                             RequestData[REQUEST_BUFFER_SIZE];
  UINT8                             ResponseSize;
  UINT8                             ResponseData;
  UINT32                            Retries;

  Retries = 10;
  ZeroMem (RequestData, REQUEST_BUFFER_SIZE);
  RequestData[0] = SM_BYOSOFT_NETFN_SUB_REDFISH_CLOSE_USB_KEY;
  RequestData[1] = ReturnStatus;
  ResponseSize   = 1;

  Status = EfiSendCommandToBMC (
            SM_BYOSOFT_NETFN_APP,
            SM_BYOSOFT_NETFN_SUB_REDFISH,
            (UINT8 *) RequestData,
            REQUEST_BUFFER_SIZE,
            &ResponseData,
            &ResponseSize
            );

  if(!EFI_ERROR(Status)){
    mUsbKeyOpen = FALSE;
  }
  return Status;
}

VOID
EFIAPI
RedfishReportData (
  IN        CONST GUID        *CallBackGuid, OPTIONAL
  IN        UINTN             CallBackToken,
  IN  OUT   VOID              *TokenData,
  IN        UINTN             TokenDataSize
  )
{
  RedfishBios_V1_0_0_Bios_CS       *BiosReturnedCS;
  RedfishCS_Type_EmptyProp_CS_Data *EmptyProp_CS_Ptr;
  RedfishCS_EmptyProp_KeyValue     *KeyValuePtr;
  RedfishCS_EmptyProp_KeyValue     *CurrentSettings;
  RedfishAttributeRegistry_V1_0_0_AttributeRegistry_CS *AttributeReturnedCS;
  RedfishAttributeRegistry_V1_0_0_Attributes_Array_CS  *CurrentAttributes;
  EFI_BYO_FORM_BROWSER_EXTENSION_PROTOCOL *FormBrowserProtocol;
  EFI_REST_JSON_STRUCTURE_PROTOCOL        *RestJsonStructureProtocol;
  EFI_REST_JSON_STRUCTURE_HEADER          *JsonStructure;
  UINTN      BufferSize;
  VOID       *Buffer;
  UINT32     Index;
  EFI_STATUS Status;
  CHAR8      *JsonText;
  VOID       *OriginPtr;
  BOOLEAN    VarSwUpdated = PcdGetBool(PcdPlatformNvVariableSwUpdated);

  DEBUG ((DEBUG_INFO, "RedfishReportData VarSwUpdated:%d\n", VarSwUpdated));

  if (TokenDataSize > 0) {
    //
    // Report Redfish data only once after Enter UiApp
    //
    if (mFirstReportEnterUiApp) {
      return;
    }
    mFirstReportEnterUiApp = TRUE;
  }

  if (mIsEnterReportRedfishData) {
    //
    // Avoid re-enter this function
    //
    mIsResetSystem = TRUE;
    return;
  }

  mIsEnterReportRedfishData = TRUE;
  DEBUG((EFI_D_INFO, "%a()\n",__FUNCTION__));

  if(!mUsbKeyOpen){
    Status = IpmiOpenUsbKey();
    if (EFI_ERROR (Status)) {
      DEBUG((EFI_D_INFO, "IpmiOpenUsbKey:%r\n", Status));
      goto Exit;
    }
  }

  //
  // Query whether BMC USB Device Is Ready
  //
  Status = IpmiQueryUseKey ();
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  DEBUG ((DEBUG_INFO, "BMC Device is ready to use. \n"));

  Status = gBS->LocateProtocol (
                  &gEfiByoFormBrowserExProtocolGuid,
                  NULL,
                  (VOID **) &FormBrowserProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  Status = gBS->LocateProtocol (
                  &gEfiRestJsonStructureProtocolGuid,
                  NULL,
                  (VOID **) &RestJsonStructureProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Read BIOS JSON from file, if not found, then read if from BIOS image
  //
  if(VarSwUpdated){
    Status = EFI_NOT_FOUND;
  } else {
    DEBUG ((DEBUG_INFO, "GetSectionFromUSB \n"));
    Status = EFI_NOT_FOUND;
    if (mIsReportRedfishData == FALSE) {
      //
      // First Report, need to read BIOS JSON from USB device and align the setting from BMC
      // The following report will save the setting only.
      //
      Status = ReadFileToBuffer (BIOS_JSON_FILE, &BufferSize, &Buffer);
      //
      // After the first report is finished, set this flag as TRUE
      //
      mIsReportRedfishData = TRUE;
    }
  }
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_INFO, "GetSectionFromFv \n"));
    Status = GetSectionFromAnyFv (&gRedfishPlatformJsonFile, EFI_SECTION_RAW, 1, &Buffer, &BufferSize);
  }
  ASSERT_EFI_ERROR (Status);

  //
  // Convert CS to JSON, and save it as BIOS JSON
  //
  DEBUG ((DEBUG_INFO, "Convert Json BIOS \n"));
  JsonText = Buffer;
  Status = RestJsonStructureProtocol->ToStructure (
              RestJsonStructureProtocol,
              NULL,
              JsonText,
              &JsonStructure
            );
  FreePool (JsonText);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  DEBUG ((DEBUG_INFO, "Check Json BIOS \n"));

  BiosReturnedCS = (RedfishBios_V1_0_0_Bios_CS *) JsonStructure->JsonStructurePointer;
  EmptyProp_CS_Ptr = (RedfishCS_Type_EmptyProp_CS_Data *)((&(BiosReturnedCS->Attributes->Prop))->ForwardLink);
  if (EmptyProp_CS_Ptr->Header.ResourceType != RedfishCS_Type_CS_EmptyProp) {
    goto Exit;
  }

  DEBUG ((DEBUG_INFO, "Get BIOS setting from Browser \n"));
  //
  // Input user setting, and Get the setting from Setup Browser
  //
  Status = FormBrowserProtocol->ReterieveSetupOptions (EmptyProp_CS_Ptr->KeyValuePtr, &CurrentSettings, &CurrentAttributes);
  if (EFI_ERROR (Status)) {
    RestJsonStructureProtocol->DestoryStructure (
                RestJsonStructureProtocol,
                JsonStructure
              );
    goto Exit;
  }

  DEBUG ((DEBUG_INFO, "Update BIOS setting from Browser \n"));
  //
  // Update BIOS setting from Setup Browser
  //
  OriginPtr = EmptyProp_CS_Ptr->KeyValuePtr;
  EmptyProp_CS_Ptr->KeyValuePtr = CurrentSettings;
  KeyValuePtr = EmptyProp_CS_Ptr->KeyValuePtr;
  Index = 0;
  for (KeyValuePtr = EmptyProp_CS_Ptr->KeyValuePtr; KeyValuePtr != NULL; KeyValuePtr = KeyValuePtr->NextKeyValuePtr) {
    Index ++;
  }
  EmptyProp_CS_Ptr->NunmOfProperties = Index;

  DEBUG ((DEBUG_INFO, "Begin Convert BIOS setting to JSON and Question number is %d \n", Index));
  //
  // Convert BIOS setting to JSON
  //
  Status = RestJsonStructureProtocol->ToJson (
              RestJsonStructureProtocol,
              JsonStructure,
              &JsonText
            );
  DEBUG ((DEBUG_INFO, "Free Convert BIOS setting to JSON \n"));

  EmptyProp_CS_Ptr->KeyValuePtr = OriginPtr;
  RestJsonStructureProtocol->DestoryStructure (
              RestJsonStructureProtocol,
              JsonStructure
            );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  DEBUG ((DEBUG_INFO, "Convert BIOS setting to JSON \n"));

  FreePool (CurrentSettings);
  Status = WriteFileFromBuffer (BIOS_JSON_FILE, AsciiStrLen (JsonText), JsonText);
  FreePool (JsonText);

  DEBUG ((DEBUG_INFO, "Save BIOS setting to JSON file. Status is %r\n", Status));

  //
  // Read Attribute JSON from BIOS image
  //
  GetSectionFromAnyFv (&gRedfishPlatformJsonFile, EFI_SECTION_RAW, 0, &Buffer, &BufferSize);

  //
  // Covert JSON to Attribute Structure
  //
  JsonText = Buffer;
  Status = RestJsonStructureProtocol->ToStructure (
              RestJsonStructureProtocol,
              NULL,
              JsonText,
              &JsonStructure
            );
  FreePool (JsonText);
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  AttributeReturnedCS = JsonStructure->JsonStructurePointer;

  //
  // Update Attribute Structure based on current BIOS
  //
  OriginPtr = AttributeReturnedCS->RegistryEntries->Attributes;
  AttributeReturnedCS->RegistryEntries->Attributes = CurrentAttributes;

  DEBUG ((DEBUG_INFO, "Convert BIOS Attributes to JSON \n"));
  //
  // Convert Attribute to JSON
  //
  Status = RestJsonStructureProtocol->ToJson (
              RestJsonStructureProtocol,
              JsonStructure,
              &JsonText
            );
  AttributeReturnedCS->RegistryEntries->Attributes = OriginPtr;
  RestJsonStructureProtocol->DestoryStructure (
              RestJsonStructureProtocol,
              JsonStructure
            );
  if (EFI_ERROR (Status)) {
    goto Exit;
  }
  FreePool (CurrentAttributes);

  Status = WriteFileFromBuffer (ATTRIBUTE_REGISTRY_JSON_FILE, AsciiStrLen (JsonText), JsonText);
  FreePool (JsonText);
  DEBUG ((DEBUG_INFO, "Save BIOS Attributes File. Status is %r.\n", Status));

Exit:
  mIsEnterReportRedfishData = FALSE;

  if (mIsResetSystem) {
    //
    // Trig Browser Reset to do the required steps
    //
    FormBrowserProtocol->PlatformReset ();
  }

  return;
}

/**
  Stop BMC USB device on ready to boot or exit UI

  @param    Event          Event pointer related to hotkey service.
  @param    Context        Context pass to this function.
**/
VOID
EFIAPI
StopBmcUsbService (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  gBS->CloseEvent (Event);

  //
  // Close USB key on ready to boot event
  //
  IpmiCloseUseKey (0);

  return;
}

EFI_STATUS
SaveSettingToJson (
  IN SETUP_SAVE_NOTIFY_PROTOCOL *This
  )
{
  RedfishReportData (NULL, 0, NULL, 0);
  return EFI_SUCCESS;
}

/**
  Entry point to communicate with BMC by Redfish data

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.
  @retval other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
BiosJsonBmcDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS Status;
  EFI_EVENT  Event;

  Status = IpmiOpenUsbKey ();

  if (!EFI_ERROR (Status)) {
    LibPcdCallbackOnSet (
      &gEfiByoModulePkgTokenSpaceGuid,
      PcdToken (PcdUiAppState),
      RedfishReportData
      );

    Status = EfiCreateEventReadyToBootEx (
              TPL_CALLBACK,
              StopBmcUsbService,
              NULL,
              &Event
              );
    ASSERT_EFI_ERROR (Status);

    Status = EfiNamedEventListen (
               &gEfiSetupExitGuid,
               TPL_CALLBACK,
               StopBmcUsbService,
               NULL,
               NULL);
    ASSERT_EFI_ERROR (Status);

    //
    // Install setup notify function.
    //
    ZeroMem(&gSetupSaveNotify, sizeof(SETUP_SAVE_NOTIFY_PROTOCOL));
    gSetupSaveNotify.SaveValue = SaveSettingToJson;
    Status = gBS->InstallProtocolInterface (
                    &gSetupSaveNotify.DriverHandle,
                    &gSetupSaveNotifyProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gSetupSaveNotify
                    );
  }

  return Status;
}
