/** @file

Copyright (c) 2023, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  IpmiStatusCodeHandlerRuntimeDxe.c

Abstract:


Revision History:

**/

#include "IpmiSendStatusCodeHandler.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/IpmiTransportProtocol.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Pi/PiHob.h>
#include <Guid/MemoryStatusCodeRecord.h>
#include <Library/MemoryAllocationLib.h>


RUNTIME_MEMORY_STATUSCODE_HEADER  *mRtMemoryStatusCodeTable;

UINT8                      mStatusCodeLog = 0;
BOOLEAN                    mStatusReady = FALSE;


typedef struct {
  UINT32           StatusCodeType;
  UINT32           TimeStamp;
  UINT8            IpmiEventData;
} MEMORY_STATUSCODE_TYPE;


EFI_RSC_HANDLER_PROTOCOL  *gRscHandlerProtocol = NULL;

EFI_STATUS
SendStatusCodeSelToBmc(
  UINT32           StatusCodeType,
  UINT8            ProgCode,
  UINT32           TimeStamp
  )
{
	IPMI_SEL_EVENT_RECORD_DATA	       SelRecord;
  UINT8                              ResponseData[2];
  UINT8                              ResponseDataSize;
  EFI_STATUS                         Status = EFI_SUCCESS;

  ZeroMem(&SelRecord, sizeof(IPMI_SEL_EVENT_RECORD_DATA));
  SelRecord.RecordType   = IPMI_SEL_SYSTEM_RECORD;
  SelRecord.TimeStamp    = TimeStamp;
  SelRecord.GeneratorId  = EFI_SOFTWARE_ID;
  SelRecord.EvMRevision  = IPMI_EVM_REVISION;
  SelRecord.SensorType   = 0x0F;
  SelRecord.SensorNumber = 0x00;
  SelRecord.EventDirType = IPMI_SENSOR_TYPE_EVENT_CODE_DISCRETE;

  if (StatusCodeType == EFI_PROGRESS_CODE) {
    SelRecord.OEMEvData1 = OEM_DATA_PROGRESS_CODE;
  } else {
    SelRecord.OEMEvData1 = OEM_DATA_ERROR_CODE;
  }
  SelRecord.OEMEvData2 = (UINT8)ProgCode;
  SelRecord.OEMEvData3 = 0xFF;
  ResponseDataSize = sizeof(ResponseData);

  //
  //  Send Sel to BMC.
  //
  Status = EfiSendCommandToBMC (
            EFI_SM_NETFN_STORAGE,
            EFI_STORAGE_ADD_SEL_ENTRY,
            (UINT8*)&SelRecord,
            sizeof(SelRecord),
            ResponseData,
            &ResponseDataSize
            );
  return Status;
}

EFI_STATUS
SendPeiStatusCodeToBmc(
  void
  )
{
  EFI_PEI_HOB_POINTERS        GuidHob;
  IPMI_SEL_PC_INFO            *SelInfo = NULL;
  UINT32                      Index;
  EFI_STATUS                  Status = EFI_SUCCESS;

  GuidHob.Raw = GetFirstGuidHob(&gIpmiSelPcInfoHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  SelInfo = (IPMI_SEL_PC_INFO *)(GuidHob.Guid + 1);
  for(Index = 0;Index < SelInfo->Count; Index ++) {
    //
    //If the Setup option is PROGRESS_AND_ERROR_CODE, all Status codes are reported.
    //
    if (mStatusCodeLog == PROGRESS_AND_ERROR_CODE) {
      if (SelInfo->StatusCodeType[Index] == PROGRESS_CODE) {
        SendStatusCodeSelToBmc(EFI_PROGRESS_CODE,SelInfo->IpmiEventData[Index],SelInfo->TimeStamp[Index]);
      } else {
        SendStatusCodeSelToBmc(EFI_ERROR_CODE,SelInfo->IpmiEventData[Index],SelInfo->TimeStamp[Index]);
      }
    }
    //
    //If the Setup option is ERROR_CODE, Error Status codes are reported.
    //
    if ((mStatusCodeLog == ERROR_CODE) && (SelInfo->StatusCodeType[Index] == ERROR_CODE)) {
      SendStatusCodeSelToBmc(EFI_ERROR_CODE,SelInfo->IpmiEventData[Index],SelInfo->TimeStamp[Index]);
    }
    //
    //If the Setup option is PROGRESS_CODE, Progress Status codes are reported.
    //
    if ((mStatusCodeLog == PROGRESS_CODE) && (SelInfo->StatusCodeType[Index] == PROGRESS_CODE)) {
      SendStatusCodeSelToBmc(EFI_PROGRESS_CODE,SelInfo->IpmiEventData[Index],SelInfo->TimeStamp[Index]);
    }
  }
  return Status;
}

EFI_STATUS
SendDxeStatusCodeToBmc(
  VOID
  )
{
  MEMORY_STATUSCODE_TYPE            *MemStatusCodeType;
  RUNTIME_MEMORY_STATUSCODE_HEADER  *RtMemoryStatusHeader;
  UINTN                              Index;
  EFI_STATUS                         Status = EFI_SUCCESS;

  //
  //  Reports the Status Code stored in the Dxe phase.
  //
  RtMemoryStatusHeader = mRtMemoryStatusCodeTable;
  MemStatusCodeType =  (MEMORY_STATUSCODE_TYPE *)(mRtMemoryStatusCodeTable + 1);

  for ( Index = 0; Index < RtMemoryStatusHeader->RecordIndex; Index++) {
    SendStatusCodeSelToBmc(MemStatusCodeType->StatusCodeType,MemStatusCodeType->IpmiEventData,MemStatusCodeType->TimeStamp);
    MemStatusCodeType ++;
  }
  return Status;
}

EFI_STATUS
InitStatusCode(
  IN EFI_STATUS_CODE_TYPE    CodeType,
  IN EFI_STATUS_CODE_VALUE   Value,
  IN OUT  UINT8              *IpmiEventData
  )
{
  UINT32                      Index;
  STATIC UINT32               ReportMap = 0;
  EFI_STATUS                  Status = EFI_NOT_FOUND;

  //
  // Check whether it is a Progress Code
  //
  if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    for (Index = 0; Index < ARRAY_SIZE (gIpmiProgressCode); Index++) {
      if(gIpmiProgressCode[Index].StatusCodeType == Value) {
        if (!(ReportMap & gIpmiProgressCode[Index].Flagbit)) {
          *IpmiEventData = gIpmiProgressCode[Index].IpmiEventData;
          ReportMap |= gIpmiProgressCode[Index].Flagbit;
          Status = EFI_SUCCESS;
          return Status;
        }
      }
    }
    return Status;
  //
  // Check whether it is a Error Code
  //
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    for (Index = 0; Index < ARRAY_SIZE (gIpmiErrorCode); Index++) {
      if(gIpmiErrorCode[Index].StatusCodeType == Value) {
        if (!(ReportMap & gIpmiErrorCode[Index].Flagbit)) {
          *IpmiEventData = gIpmiErrorCode[Index].IpmiEventData;
          ReportMap |= gIpmiErrorCode[Index].Flagbit;
          Status = EFI_SUCCESS;
          return Status;
        }
      }
    }
    return Status;
  }
  return Status;
}

EFI_STATUS
EFIAPI
PrjStatusCodeListenerDxe (
  IN EFI_STATUS_CODE_TYPE   CodeType,
  IN EFI_STATUS_CODE_VALUE  Value,
  IN UINT32                 Instance,
  IN EFI_GUID               *CallerId,
  IN EFI_STATUS_CODE_DATA   *Data
  )
{
  MEMORY_STATUSCODE_TYPE      *Record;
  EFI_STATUS                  Status;
  UINT32                      TimeStamp = 0;
  UINT8                       IpmiEventData = 0;

  if (mStatusCodeLog == ERROR_CODE && (CodeType & EFI_STATUS_CODE_TYPE_MASK) != EFI_ERROR_CODE) {
    return EFI_SUCCESS;
  }
  if (mStatusCodeLog == PROGRESS_CODE && (CodeType & EFI_STATUS_CODE_TYPE_MASK) != EFI_PROGRESS_CODE) {
    return EFI_SUCCESS;
  }
  if (((CodeType & EFI_STATUS_CODE_TYPE_MASK) != EFI_PROGRESS_CODE) && ((CodeType & EFI_STATUS_CODE_TYPE_MASK) != EFI_ERROR_CODE)) {
    return EFI_SUCCESS;
  }
  //
  //  Storage gEfiIpmiTransportProtocolGuid and gEfiVariableArchProtocolGuid
  //  prior to installation of the two Protocol Status Code
  //
  Record = (MEMORY_STATUSCODE_TYPE *) (mRtMemoryStatusCodeTable + 1);
  Record = &Record[mRtMemoryStatusCodeTable->RecordIndex];

  Status = InitStatusCode(CodeType,Value,&IpmiEventData);
  if (!EFI_ERROR(Status)) {
    EfiSmGetTimeStamp(&TimeStamp);
    if (mStatusReady) {
      SendStatusCodeSelToBmc(CodeType,IpmiEventData,TimeStamp);
    } else {
      //
      // Save status code.
      //
      Record->StatusCodeType = CodeType;
      Record->TimeStamp = TimeStamp;
      Record->IpmiEventData = IpmiEventData;
      mRtMemoryStatusCodeTable->RecordIndex ++;
    }
  }
  return EFI_SUCCESS;
}

VOID
EFIAPI
IpmiTransportCallback (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS                 Status;
  VOID                       *Interface;

  Status = gBS->LocateProtocol(&gEfiIpmiTransportProtocolGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);
  mStatusReady = TRUE;
  //
  //  The Status Code reported in the Pei phase was sent
  //
  Status = SendPeiStatusCodeToBmc();
  if (Status == EFI_SUCCESS) {
    //
    //  The Status Code reported in the Dxe phase was sent
    //
    SendDxeStatusCodeToBmc();
    if (mRtMemoryStatusCodeTable != NULL) {
      FreePool(mRtMemoryStatusCodeTable);
    }
  }
}


VOID
EFIAPI
PrjExitBootServicesEventNotify (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  if(gRscHandlerProtocol != NULL){
    gRscHandlerProtocol->Unregister(PrjStatusCodeListenerDxe);
  }
}



VOID
EFIAPI
ProjectRscHandlerCallBack (
  IN  EFI_EVENT  Event,
  IN  VOID       *Context
  )
{
  EFI_STATUS                 Status;

  Status = gBS->LocateProtocol(&gEfiRscHandlerProtocolGuid, NULL, (VOID**)&gRscHandlerProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  //
  // Register PrjStatusCodeListenerDxe.
  //
  Status = gRscHandlerProtocol->Register(PrjStatusCodeListenerDxe, TPL_HIGH_LEVEL);
}

/**
  Initialize Status Code driver.

  @param ImageHandle     The image handle.
  @param SystemTable     The system table.

  @retval EFI_SUCCESS    The Setup Browser module is initialized correctly..
  @return Other value if failed to initialize the Setup Browser module.

**/

EFI_STATUS
StatusCodeHandleDxeRunTimeEntry(
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_EVENT                   Event;
  VOID                        *Registration;
  EFI_STATUS                  Status = EFI_SUCCESS;
  EFI_PEI_HOB_POINTERS        GuidHob;
  IPMI_SEL_PC_INFO            *SelInfo;

  GuidHob.Raw = GetFirstGuidHob(&gIpmiSelPcInfoHobGuid);
  ASSERT(GuidHob.Raw != NULL);
  SelInfo = (IPMI_SEL_PC_INFO *)(GuidHob.Guid+1);
  mStatusCodeLog = SelInfo->StatusCodeLog;

  if (mStatusCodeLog == DISABLED_STATUS_CODE) {
    return Status;
  }
  //
  // Allocate runtime memory status code pool.
  //
  mRtMemoryStatusCodeTable = AllocateRuntimePool (
                              sizeof (RUNTIME_MEMORY_STATUSCODE_HEADER) +
                              sizeof (MEMORY_STATUSCODE_TYPE) * MAX_STATUS_CODE_NUMBER
                              );
  ASSERT (mRtMemoryStatusCodeTable != NULL);

  mRtMemoryStatusCodeTable->RecordIndex      = 0;
  mRtMemoryStatusCodeTable->MaxRecordsNumber = MAX_STATUS_CODE_NUMBER;

  EfiCreateProtocolNotifyEvent (
    &gEfiRscHandlerProtocolGuid,
    TPL_CALLBACK,
    ProjectRscHandlerCallBack,
    NULL,
    &Registration
    );

  EfiCreateProtocolNotifyEvent (
    &gEfiIpmiTransportProtocolGuid,
    TPL_CALLBACK,
    IpmiTransportCallback,
    NULL,
    &Registration
    );

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  PrjExitBootServicesEventNotify,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  return Status;
}