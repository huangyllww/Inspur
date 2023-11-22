/*++

  Copyright (c) 2006 - 2019 Byosoft Corporation. All rights reserved.
  This program and associated documentation (if any) is furnished
  under a license. Except as permitted by such license,no part of this
  program or documentation may be reproduced, stored divulged or used
  in a public system, or transmitted in any form or by any means
  without the express written consent of Byosoft Corporation.

Module Name:
  PostError.c

Abstract:
  Implements the programming of Post Error.

Revision History:

--*/

#include <Guid/EventGroup.h>
#include <Guid/MdeModuleHii.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <IndustryStandard/SmBios.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/DevicePathLib.h>
#include <Library/HiiLib.h>
#include <Library/PrintLib.h>
#include <Library/SetupUiLib.h>
#include <Library/UefiLib.h>
#include <Protocol/EventLog.h>
#include <Protocol/Smbios.h>
//#include <Protocol/PlatformPostError.h>
#include <Protocol/ByoSmiFlashProtocol.h>
#include <Protocol/HiiConfigAccess.h>
#include <Protocol/DevicePath.h>


#define RECALAM_COUNT              20
#define TypeDesc_number            0x01
#define EVENT_LOG_SIGNATURE        SIGNATURE_32('E', 'V', 'L', 'G')


//
// Access Method.
//  0x00~0x04:  as following definition
//  0x05~0x7f:  Available for future assignment.
//  0x80~0xff:  BIOS Vendor/OEM-specific.
// 
#define ACCESS_INDEXIO_1INDEX8BIT_DATA8BIT    0x00
#define ACCESS_INDEXIO_2INDEX8BIT_DATA8BIT    0X01
#define ACCESS_INDEXIO_1INDEX16BIT_DATA8BIT   0X02
#define ACCESS_MEMORY_MAPPED                  0x03
#define ACCESS_GPNV                           0x04


typedef struct {
  UINTN                            Signature;
  EFI_HANDLE                       DriverHandle;
  EFI_HII_CONFIG_ACCESS_PROTOCOL   ConfigAccess;
  EFI_EVENT_LOG_PROTOCOL           EventLog;
  EFI_HII_HANDLE                   HiiHandle;
  UINT32                           NvAreaOffset;
  UINT32                           NvAreaAddress;
  UINT32                           NvAreaSize;
  EVENT_LOG_RECORD                 *LastEvent;
  EVENT_LOG_RECORD                 *NextEvent;
  EVENT_LOG_RECORD                 Last20Rec[20];
} EVENT_LOG_PRIVATE_DATA;


extern EVENT_LOG_PRIVATE_DATA gPrivate;
extern unsigned char EventLogBin[];


EventLog_TypeDescriptors mTypeDescList[] = {
  {EVENT_LOG_POST_ERROR, EVENT_LOG_DATA_POST_RESULT_BITMAP}
};

UINT8                     *pMemMappedBuffer = NULL;
BYO_SMIFLASH_PROTOCOL     *mByoSmiFlash;

LOG_HEADER_TYPE_1 mDefaultEventLogRecordHeader = {
  {0x00, 0x00, 0x00, 0x00, 0x00},       // header Offset 0h - 04h is OEM reserved.
  0x60,                                 // multiple Event Time Window.
  0x01,                                 // multiple Event Count Increment.
  0x00,                                 // header Offset 07h=Pre-Boot Clear Event.
                                        // log Byte 1 (CMOS RAM Addr 0-255).
  0x00,                                 // header Offset 08h=Pre-Boot Clear Event.
                                        // log Byte 2 (Bit Index).
  0x00,                                 // header Offset 09h= starting loc of checksummed region.
  0x00,                                 // header Offset 0Ah= length of checksummed region.
  0x00,                                 // header Offset 0Bh= starting loc of word checksum.
  {0x00, 0x00, 0x00},                   // header Offset 0Ch-0Eh= RSVD.
  0x01                                  // SMBIOS BIOS Event Log Header Revision 1.
};

//EFI_GUID  gEventLogFormSetGuid = EFI_EVENT_LOG_PROTOCOL_GUID;


VOID
SmbiosGetStatus (
  OUT UINT32 *Count,
  OUT UINT8 *Status
)
{
  EVENT_LOG_RECORD *pRecord;
  UINT32 counter = 0;

  *Count  = 0;
  *Status = 0;
  if (pMemMappedBuffer == NULL)
    return;

  pRecord = (EVENT_LOG_RECORD *)(pMemMappedBuffer + sizeof (LOG_HEADER_TYPE_1));
  *Status = ELOG_SPACE_VALID;
  while (pRecord->ErrorCode != 0xffff) {
    pRecord ++;
    counter ++;
    if (counter >= ((gPrivate.NvAreaSize / sizeof(EVENT_LOG_RECORD)) - 1)) {
      *Status |= ELOG_SPACE_FULL;
      break;
    }
  }
  *Count = counter;

  return;
}

EFI_STATUS
SmbiosType15Publish (
  IN EFI_SMBIOS_PROTOCOL        *Smbios
)
{
  EFI_STATUS                    Status;
  SMBIOS_TABLE_TYPE15           *Type15Record;
  EFI_SMBIOS_HANDLE             SmbiosHandle;
  UINT32                        EventLogCount;
  UINT8                         ELogStatus;

  Type15Record = AllocateZeroPool(sizeof(SMBIOS_TABLE_TYPE15) + 2);
  Type15Record->Hdr.Type      = EFI_SMBIOS_TYPE_SYSTEM_EVENT_LOG;
  Type15Record->Hdr.Length    = sizeof(SMBIOS_TABLE_TYPE15);
  Type15Record->Hdr.Handle    = 0;
  Type15Record->LogAreaLength = (UINT16)gPrivate.NvAreaSize;
  Type15Record->LogHeaderStartOffset = 0;
  Type15Record->LogDataStartOffset   = sizeof (LOG_HEADER_TYPE_1);
  Type15Record->AccessMethod         = ACCESS_MEMORY_MAPPED;

  SmbiosGetStatus (&EventLogCount, &ELogStatus);
  // This bit-field describes the current status of the system event-log:
  // Bits 7:2 Reserved, set to 0¡¯s
  // Bit 1 Log area full, if 1
  // Bit 0 Log area valid, if 1
  Type15Record->LogStatus            = ELogStatus;

  // Unique token that is reassigned every time the event log changes.
  // Can be used to determine if additional events have occurred since the last time the log was read.
  Type15Record->LogChangeToken       = EventLogCount;

  // The address associated with the access method;
  // the data present depends on the Access Method field value.
  Type15Record->AccessMethodAddress  = gPrivate.NvAreaAddress;

  // Identifies the format of the log header area.
  Type15Record->LogHeaderFormat      = FORMAT_TYPE_TYPE_1_LOG_HEADER;  //LOG_HEADER_FORMAT_TYPE;

  // Number of supported event log type descriptors that follow.
  // If the value is 0, the list that starts at offset 17h is not present.
  Type15Record->NumberOfSupportedLogTypeDescriptors = TypeDesc_number;
  Type15Record->LengthOfLogTypeDescriptor  = sizeof (EventLog_TypeDescriptors);

  CopyMem(
    Type15Record->EventLogTypeDescriptors,
    &mTypeDescList,
    sizeof (mTypeDescList)
    );

  SmbiosHandle = SMBIOS_HANDLE_PI_RESERVED;
  Status = Smbios->Add(Smbios, NULL, &SmbiosHandle, (EFI_SMBIOS_TABLE_HEADER*)Type15Record);
  FreePool (Type15Record);
  return Status;
}


void *
EventGet ()
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));
  return (void *)(pMemMappedBuffer + sizeof (LOG_HEADER_TYPE_1));
}


EFI_STATUS
EFIAPI
EventInsert (
  IN UINT16                 ErrorCode,
  IN EFI_TIME               *EventTime   //  OPTIONAL
  )
{
  EFI_TIME         EfiTime;
  EFI_STATUS       Status;
  UINT32           Offset;
  

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (pMemMappedBuffer == NULL){
    return EFI_OUT_OF_RESOURCES;
  }

  if(gPrivate.NextEvent > gPrivate.LastEvent){
    CopyMem(
      gPrivate.Last20Rec, 
      gPrivate.LastEvent - 19,
      20 * sizeof(EVENT_LOG_RECORD)
      );
    SetMem(pMemMappedBuffer, gPrivate.NvAreaSize, 0xFF);
    CopyMem(pMemMappedBuffer, &mDefaultEventLogRecordHeader, sizeof(LOG_HEADER_TYPE_1));
    CopyMem(pMemMappedBuffer + sizeof(LOG_HEADER_TYPE_1), gPrivate.Last20Rec, 20 * sizeof(EVENT_LOG_RECORD));
    
    Status = mByoSmiFlash->Erase(mByoSmiFlash, gPrivate.NvAreaOffset, gPrivate.NvAreaSize);
    ASSERT_EFI_ERROR (Status);

    Status = mByoSmiFlash->Write(mByoSmiFlash, gPrivate.NvAreaOffset, pMemMappedBuffer, sizeof(LOG_HEADER_TYPE_1) + 20 * sizeof(EVENT_LOG_RECORD));
    ASSERT_EFI_ERROR (Status);  

    gPrivate.NextEvent = (EVENT_LOG_RECORD*)(pMemMappedBuffer + sizeof(LOG_HEADER_TYPE_1) + 20 * sizeof(EVENT_LOG_RECORD));
  }

  gPrivate.NextEvent->ErrorCode = ErrorCode;
  if (EventTime == NULL) {
    gRT->GetTime(&EfiTime, NULL);
    CopyMem(&gPrivate.NextEvent->DateTime, &EfiTime,  sizeof(EVENT_LOG_DATE_TIME));
  } else {
    CopyMem(&gPrivate.NextEvent->DateTime, EventTime, sizeof(EVENT_LOG_DATE_TIME));
  }
  Offset = (UINT32)((UINTN)gPrivate.NextEvent - (UINTN)pMemMappedBuffer);
  Status = mByoSmiFlash->Write(mByoSmiFlash, gPrivate.NvAreaOffset + Offset, pMemMappedBuffer + Offset, sizeof(EVENT_LOG_RECORD));
  ASSERT_EFI_ERROR (Status);

  gPrivate.NextEvent++;

  return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
EventClear ()
{
  EFI_STATUS Status;
  
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  // FFh End-of-log. When an application searches through the event-log records,
  // the end of the log is identified when a log record with this type is found.
  if (pMemMappedBuffer == NULL)
    return EFI_OUT_OF_RESOURCES;

  SetMem(pMemMappedBuffer, gPrivate.NvAreaSize, 0xff);
  CopyMem (
    pMemMappedBuffer,
    &mDefaultEventLogRecordHeader,
    sizeof (LOG_HEADER_TYPE_1)
    );

  Status = mByoSmiFlash->Erase(mByoSmiFlash, gPrivate.NvAreaOffset, gPrivate.NvAreaSize);
  ASSERT_EFI_ERROR (Status);

  Status = mByoSmiFlash->Write(mByoSmiFlash, gPrivate.NvAreaOffset, pMemMappedBuffer, sizeof(LOG_HEADER_TYPE_1));
  ASSERT_EFI_ERROR (Status);

  gPrivate.NextEvent = (EVENT_LOG_RECORD*)(pMemMappedBuffer + sizeof(LOG_HEADER_TYPE_1));

  return Status;
}



EFI_STATUS
InitializeEventLogRecords (UINT8 *pBuffer)
{
  EFI_STATUS         Status;
  UINTN              EventMaxCount;
  

  Status = mByoSmiFlash->Read (mByoSmiFlash, gPrivate.NvAreaOffset, pBuffer, gPrivate.NvAreaSize);
  ASSERT_EFI_ERROR (Status);

  if (*(UINT64*)pBuffer == 0xFFFFFFFF) {
    DEBUG((EFI_D_INFO, "Need Init Header\n"));
    CopyMem(pBuffer, &mDefaultEventLogRecordHeader, sizeof (LOG_HEADER_TYPE_1));
    Status = mByoSmiFlash->Write(mByoSmiFlash, gPrivate.NvAreaOffset, pMemMappedBuffer, sizeof (LOG_HEADER_TYPE_1));
    ASSERT_EFI_ERROR (Status);    
  }

  EventMaxCount = (gPrivate.NvAreaSize - sizeof(LOG_HEADER_TYPE_1))/sizeof(EVENT_LOG_RECORD);
  DEBUG((EFI_D_INFO, "EventMaxCount:%d\n", EventMaxCount));
  gPrivate.NextEvent = (EVENT_LOG_RECORD*)(pMemMappedBuffer + sizeof(LOG_HEADER_TYPE_1));
  gPrivate.LastEvent = gPrivate.NextEvent + EventMaxCount - 1;

  while(gPrivate.NextEvent <= gPrivate.LastEvent) {
    if(gPrivate.NextEvent->ErrorCode == 0xFFFF){
      break;
    }
    gPrivate.NextEvent++;
  }
  
  return Status;
}





EFI_STATUS
EFIAPI
EventLogFormExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  return EFI_UNSUPPORTED;
}


EFI_STATUS
EFIAPI
EventLogFormRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  return EFI_UNSUPPORTED;
}


EFI_STATUS
EFIAPI
EventLogFormCallback (
  IN     CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN     EFI_BROWSER_ACTION                     Action,
  IN     EFI_QUESTION_ID                        QuestionId,
  IN     UINT8                                  Type,
  IN OUT EFI_IFR_TYPE_VALUE                     *Value,
  OUT    EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  return EFI_SUCCESS;
}


#if 0
VOID
CreateEventLogForm ()
{
  VOID                  *StartOpCodeHandle;
  VOID                  *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL    *StartLabel;
  EFI_IFR_GUID_LABEL    *EndLabel;
  STRING_REF            ErrorDetailToken;
  EVENT_LOG_RECORD      *pRecord;
  CHAR16                LogDate[100];
  UINTN                 LogCount = 0;
  UINT8                 LogShowCount = 0;
  EFI_STATUS            Status;
  PLATFORM_POST_ERROR_PROTOCOL  *PostError = NULL;
  

  Status = gBS->LocateProtocol (
                  &gPlatformPostErrorProtocolGuid,
                  NULL,
                  &PostError
                  );
  ASSERT(!EFI_ERROR(Status));

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  EndOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (EndOpCodeHandle != NULL);

  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = LABEL_EVENT_LOG_LIST;

  EndLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (EndOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = EVENT_LOG_LABEL_END;

  pRecord = (EVENT_LOG_RECORD *)(pMemMappedBuffer + sizeof (LOG_HEADER_TYPE_1));
  while (pRecord->ErrorCode != 0xffff) {
    LogCount++;
    pRecord++;
  }
  DEBUG((EFI_D_INFO, "LogCount:%d\n",LogCount));
  if (LogCount > 0) {
    pRecord --;
    while (LogCount > 0 && LogShowCount < RECALAM_COUNT) {
      UnicodeSPrint(
        LogDate, 
        sizeof(LogDate), 
        L"%04x%a        %02d%02d%02d-%02d%02d%02d", 
        pRecord->ErrorCode, 
        PostError == NULL ? "                " : PostError->GetCodeName(pRecord->ErrorCode),
        pRecord->DateTime.month, 
        pRecord->DateTime.day, 
        pRecord->DateTime.year % 100, 
        pRecord->DateTime.hour, 
        pRecord->DateTime.minute, 
        pRecord->DateTime.second
        );
      ErrorDetailToken = HiiSetString (
                           gPrivate.HiiHandle,
                           0,
                           LogDate,
                           NULL
                           );
      
      HiiCreateTextOpCode (
        StartOpCodeHandle,
        ErrorDetailToken,
        STRING_TOKEN(STR_EMPTY),
        STRING_TOKEN(STR_EMPTY)
        );
      pRecord--;
      LogShowCount++;
      LogCount--;
    }
    
  }
  
  if (LogShowCount < RECALAM_COUNT) {
    for (LogCount = 0; LogCount < (RECALAM_COUNT - LogShowCount); LogCount ++) {
      HiiCreateTextOpCode (
        StartOpCodeHandle,
        STRING_TOKEN(STR_NO_DATA),
        STRING_TOKEN(STR_EMPTY),
        STRING_TOKEN(STR_EMPTY)
        );
    }
  }

  HiiUpdateForm (
    gPrivate.HiiHandle,
    &gEventLogFormSetGuid,      // Formset GUID
    EVENT_LOG_FORM_ID,          // Form ID
    StartOpCodeHandle,          // Label for where to insert opcodes
    EndOpCodeHandle             // Replace data
    );
  
  HiiFreeOpCodeHandle(StartOpCodeHandle);
  HiiFreeOpCodeHandle(EndOpCodeHandle);
}
#endif


VOID
EFIAPI
ErrorLogsEnterSetupCallback (
  IN EFI_EVENT    Event,
  IN VOID*        Context
  )
{
  EFI_STATUS            Status;
  VOID                  *Interface;

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);
  
//CreateEventLogForm();
}


VOID
EFIAPI
ErrorLogsReadyToBootCallback (
  IN EFI_EVENT    Event,
  IN VOID*        Context
  )
{
  EFI_SMBIOS_PROTOCOL   *Smbios;
  EFI_STATUS            Status;
  

  gBS->CloseEvent (Event);

  Status = gBS->LocateProtocol(&gEfiSmbiosProtocolGuid, NULL, (VOID**)&Smbios);
  ASSERT_EFI_ERROR (Status);
  SmbiosType15Publish (Smbios);
}



EVENT_LOG_PRIVATE_DATA gPrivate = {
  EVENT_LOG_SIGNATURE,
  NULL,
  {
    EventLogFormExtractConfig,
    EventLogFormRouteConfig,
    EventLogFormCallback
  },
  {
    EventGet,
    EventInsert,
    EventClear
  },
  NULL,
  0,
  0,
  0,
  0,
  0,
  {{0,},},
};



EFI_STATUS
EventLogEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                Status;
  EFI_EVENT                 ReadyToBootEvent;
  VOID                      *Registration;


  DEBUG((EFI_D_INFO, "EventLogEntry\n"));

  gPrivate.NvAreaAddress = PcdGet32(PcdEventLogBase);
  gPrivate.NvAreaSize    = PcdGet32(PcdEventLogSize);
  gPrivate.NvAreaOffset  = gPrivate.NvAreaAddress - PcdGet32(PcdFlashAreaBaseAddress);
  DEBUG((EFI_D_INFO, "(L%d) %X %X %X\n", __LINE__, gPrivate.NvAreaAddress, gPrivate.NvAreaSize, gPrivate.NvAreaOffset));
  if(gPrivate.NvAreaAddress == 0 || gPrivate.NvAreaSize == 0){
    return EFI_UNSUPPORTED;
  }

  pMemMappedBuffer = AllocatePool(gPrivate.NvAreaSize);
  if (pMemMappedBuffer == NULL){
    return EFI_OUT_OF_RESOURCES;
  }
  SetMem(pMemMappedBuffer, gPrivate.NvAreaSize, 0xff);

  Status = gBS->LocateProtocol(&gByoSmiFlashProtocolGuid, NULL, (VOID**)&mByoSmiFlash);
  ASSERT_EFI_ERROR (Status);  
  InitializeEventLogRecords(pMemMappedBuffer);

#if 0
  gPrivate.HiiHandle = HiiAddPackages (
                         &gEfiEventLogProtocolGuid,
                         NULL,
                         STRING_ARRAY_NAME,
                         EventLogBin,
                         NULL
                         );
  ASSERT(gPrivate.HiiHandle != NULL);
#endif

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiEventLogProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gPrivate.EventLog
                  );

  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    ErrorLogsEnterSetupCallback,
    NULL,
    &Registration
    );

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             ErrorLogsReadyToBootCallback,
             NULL,
             &ReadyToBootEvent
             ); 
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}


