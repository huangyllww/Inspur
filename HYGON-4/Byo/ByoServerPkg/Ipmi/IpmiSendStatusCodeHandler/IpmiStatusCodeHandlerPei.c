/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiStatusCodeHandlerPei.c

Abstract:


Revision History:

**/

#include "IpmiSendStatusCodeHandler.h"
#include <PiPei.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Ppi/ReportStatusCodeHandler.h>
#include <Ppi/IpmiTransportPpi.h>
#include <Ppi/ReadOnlyVariable2.h>


EFI_STATUS
EFIAPI
PrjPeiRscHandlerCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
PrjEndOfPeiSignalCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );


STATIC EFI_PEI_NOTIFY_DESCRIPTOR  gPpiNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiRscHandlerPpiGuid,
    PrjPeiRscHandlerCallBack
  },

  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    PrjEndOfPeiSignalCallBack
  },

};

EFI_STATUS
EFIAPI
PrjStatusCodeListenerPei (
  IN CONST  EFI_PEI_SERVICES       **PeiServices,
  IN        EFI_STATUS_CODE_TYPE   CodeType,
  IN        EFI_STATUS_CODE_VALUE  Value,
  IN        UINT32                 Instance,
  IN CONST  EFI_GUID               *CallerId,
  IN CONST  EFI_STATUS_CODE_DATA   *Data
  )
{
  UINT8                       ProgCode  = 0xFF;
  EFI_PEI_HOB_POINTERS        GuidHob;
  IPMI_SEL_PC_INFO            *SelInfo;
  UINT8                       StatusCodeType = 0;
  UINT32                      TimeStamp = 0;
  UINTN                       Index = 0;
  STATIC UINT32               ReportMapBit = 0;

  //
  //  Store the record Status Code into the hob space.
  //
  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    for ( Index = 0; Index < ARRAY_SIZE (gIpmiProgressCode); Index++) {
      if (gIpmiProgressCode[Index].StatusCodeType == Value) {
        if (!(ReportMapBit & gIpmiProgressCode[Index].Flagbit)) {
          ProgCode = gIpmiProgressCode[Index].IpmiEventData;
          StatusCodeType = (UINT8)PROGRESS_CODE;
          ReportMapBit |= gIpmiProgressCode[Index].Flagbit;
          EfiSmGetTimeStamp(&TimeStamp);
        }
      }
    }
  }
  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    for ( Index = 0; Index < ARRAY_SIZE (gIpmiErrorCode); Index++) {
      if (gIpmiErrorCode[Index].StatusCodeType == Value) {
        if (!(ReportMapBit & gIpmiErrorCode[Index].Flagbit)) {
          ProgCode = gIpmiErrorCode[Index].IpmiEventData;
          ReportMapBit |= gIpmiErrorCode[Index].Flagbit;
          StatusCodeType = (UINT8)ERROR_CODE;
          EfiSmGetTimeStamp(&TimeStamp);
        }
      }
    }
  }

  if(ProgCode != 0xFF){
    GuidHob.Raw = GetFirstGuidHob(&gIpmiSelPcInfoHobGuid);
    ASSERT(GuidHob.Raw != NULL);
    SelInfo = (IPMI_SEL_PC_INFO*)(GuidHob.Guid+1);
    if(SelInfo->Count >= ARRAY_SIZE(SelInfo->IpmiEventData)) {
      return EFI_OUT_OF_RESOURCES;
    }
    //
    // Records the reported Sel information.
    //
    SelInfo->IpmiEventData[SelInfo->Count] = ProgCode;
    SelInfo->StatusCodeType[SelInfo->Count] = (UINT8)StatusCodeType;
    SelInfo->TimeStamp[SelInfo->Count] = TimeStamp;
    SelInfo->Count++;

    return EFI_SUCCESS;
  }
  return EFI_UNSUPPORTED;  
}


EFI_STATUS
EFIAPI
PrjPeiRscHandlerCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_PEI_RSC_HANDLER_PPI           *RscHandler;
  IPMI_SEL_PC_INFO                  *SelInfo;
  EFI_STATUS                        Status = EFI_SUCCESS;

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **)&RscHandler
             );
  ASSERT_EFI_ERROR (Status);

  //
  //  Build for a Hob space to store the Status Code of the Pei phase
  //
  SelInfo = BuildGuidHob(&gIpmiSelPcInfoHobGuid, sizeof(IPMI_SEL_PC_INFO));
  ASSERT(SelInfo != NULL);
  ZeroMem(SelInfo, sizeof(IPMI_SEL_PC_INFO));
  Status = RscHandler->Register(PrjStatusCodeListenerPei);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
PrjEndOfPeiSignalCallBack (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                       Status = EFI_SUCCESS;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI  *VariablePpi;
  UINTN                            SelVarSize;
  BMC_SETUP_SEL_EVENT_DATA         SelSetupData = {0};
  EFI_GUID                         SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  EFI_PEI_HOB_POINTERS             GuidHob;
  IPMI_SEL_PC_INFO                 *SelInfo;


  Status = PeiServicesLocatePpi (&gEfiPeiReadOnlyVariable2PpiGuid, 0, NULL, (VOID **)&VariablePpi);
  ASSERT_EFI_ERROR (Status);
  SelVarSize = sizeof (BMC_SETUP_SEL_EVENT_DATA);
  ZeroMem (&SelSetupData, sizeof (SelSetupData));
  Status = VariablePpi->GetVariable (
                          VariablePpi,
                          SEL_EVENT_SETUP_VARIABLE_NAME,
                          &SystemConfigurationGuid,
                          NULL,
                          &SelVarSize,
                          &SelSetupData
                          );
  if (Status == EFI_SUCCESS) {
    GuidHob.Raw = GetFirstGuidHob(&gIpmiSelPcInfoHobGuid);
    ASSERT(GuidHob.Raw != NULL);
    SelInfo = (IPMI_SEL_PC_INFO*)(GuidHob.Guid+1);
    SelInfo->StatusCodeLog = SelSetupData.StatusCodeLog;
  }
  return Status;
}

EFI_STATUS
EFIAPI
StatusCodeHandlerPeiEntry (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;

  Status = PeiServicesNotifyPpi(gPpiNotifyList);
  ASSERT_EFI_ERROR(Status);

  return EFI_SUCCESS;
}