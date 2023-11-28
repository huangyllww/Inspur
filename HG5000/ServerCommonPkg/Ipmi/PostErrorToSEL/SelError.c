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


#include <Protocol/DataHub.h>
#include <Protocol/GenericElog.h>
#include <Protocol/IpmiInterfaceProtocol.h>
#include <EfiServerManagement.h>
#include <ipmi/IpmiNetFnAppDefinitions.h>
#include <library/EfiServerMgmtRtLib.h>
#include <guid/DataHubStatusCodeRecord.h>
#include <Library/UefiBootServicesTableLib.h>
#include <library/BaseMemoryLib.h>
#include <library/UefiLib.h>
#include <library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <library/PostErrorTableLib.h>




EFI_DATA_HUB_PROTOCOL     *mDataHub = NULL;
EFI_EVENT                 mDataHubSelEvent;

VOID                      *mGenericElogReg;
EFI_SM_ELOG_PROTOCOL      *mGenericElog;
UINT64                    LastMtc = 0;

#if 0
#pragma pack(1)
typedef struct {
  UINT16  RecordId;
  UINT8   RecordType;
  UINT32  TimeStamp;
  UINT16  GeneratorId;
  UINT8   EvMRevision;
  UINT8   SensorType;
  UINT8   SensorNumber;
  UINT8   EventDirType;
  UINT8   OEMEvData1;
  UINT8   OEMEvData2;
  UINT8   OEMEvData3;
} EFI_SEL_RECORD_DATA;
#pragma pack()
 

//
// General Info related to SEL Record
//
#define EFI_EVM_REVISION    0x04
#define EFI_BIOS_ID         0x18
#define EFI_FORMAT_REV      0x00
#define EFI_FORMAT_REV1     0x01
#define EFI_SOFTWARE_ID     0x01
#define EFI_GENERATOR_ID(a) ((EFI_BIOS_ID << 1) | (a << 1) | EFI_SOFTWARE_ID)
#endif

//
// This is a discrete sensor
//
#define EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE 0x6F

#define EFI_OEM_SPECIFIC_DATA                     0x02

//#define UNIT_TEST_SEL_ERROR 1

EFI_STATUS
UpdateOemErrorStatusCode (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN OUT  EFI_SEL_RECORD_DATA *SelRecord
  )
{
  UINT32    TimeStamp = 0;
  //
  // General IPMI Information.
  //
  SelRecord->RecordId     = 0;
  SelRecord->RecordType   = 0x02;               // OEM Record
  EfiSmGetTimeStamp (&TimeStamp);
  SelRecord->TimeStamp = TimeStamp;

  SelRecord->GeneratorId  = EFI_SOFTWARE_ID;    // BIOS POST Generator ID = 0x01

  SelRecord->EvMRevision  = EFI_EVM_REVISION;
  SelRecord->EventDirType = EFI_SENSOR_TYPE_EVENT_CODE_DISCRETE;

  // 
  // As per IPMI Spec 1.5, EvData1 field act a qualifier byte for 
  // EvData2 and EvData3. 
  //
  SelRecord->SensorType   = 0x0F;  
  SelRecord->SensorNumber = 0x06;
  SelRecord->OEMEvData1   = ((EFI_OEM_SPECIFIC_DATA << 6) | (EFI_OEM_SPECIFIC_DATA << 4));
  SelRecord->OEMEvData2   = (UINT8) (Value);
  SelRecord->OEMEvData3   = (UINT8) (Value >> 8);
  return EFI_SUCCESS;

}

STATIC
VOID
EFIAPI
WriteStatusRecordsToSel (
  )
{
  EFI_STATUS                        Status;
  EFI_DATA_HUB_PROTOCOL             *DataHub;
  EFI_DATA_RECORD_HEADER            *Record;
  DATA_HUB_STATUS_CODE_DATA_RECORD  *DataRecord;
  UINT64                            Mtc = 0, LastMtc = 0;
  static UINT64                     SavedMonotonicCount = 0;
  UINT32                            CodeType = 0;
  UINT32                            Value = 0;
  UINT32                            Instance = 0;
  EFI_SEL_RECORD_DATA               SelRecord;  
  UINT64                            RecordId = 0;
  BOOLEAN                           IsCustom = FALSE;
  UINT16                            LegacyErrorCode = 0;
  UINT16                            OperationToken = 0;
  BOOLEAN                           LogToSel = FALSE;

  Status = gBS->LocateProtocol (
                  &gEfiDataHubProtocolGuid, 
                  NULL, 
                  (VOID **)&DataHub
                  );
  if(EFI_ERROR(Status)){
    return ;
  }
  //
  // To prevent duplication of SEL records, the last record ID
  // is stored in the static SavedMonotonicCount.
  // It is extracted again in order to first acquire the next "Mtc" and then acquire the next record. 
  // Before this change, when the new record happened to the last record, it would never be extracted.
  // Consequently, the error would not reach BMC although it correctly appeared in Setup. 
  // Assume there is always a SEL record when signalled. 
  // If it is the 1st and the only record, then harmless to call twice. In reality, there are always many other records, such as EFI_PROGRESS_CODE, ahead of the accumulated error codes or the signalled error code.
  //
  Mtc = SavedMonotonicCount;
  Status = DataHub->GetNextRecord (
                      DataHub, 
                      &Mtc, 
                      NULL, 
                      &Record 
                      );
 
  while (!EFI_ERROR (Status)) {
    LastMtc = Mtc;
    Status = DataHub->GetNextRecord (
                      DataHub, 
                      &Mtc, 
                      NULL, 
                      &Record 
                      );                     
    if (!EFI_ERROR (Status)) {
      //
      // Save the previous record ID
      //
      SavedMonotonicCount = LastMtc;

      if (CompareGuid (&Record->DataRecordGuid, &gEfiDataHubStatusCodeRecordGuid)) {
        DataRecord = (DATA_HUB_STATUS_CODE_DATA_RECORD *)(((CHAR8 *)Record) + Record->HeaderSize);
        CodeType  = DataRecord->CodeType;

#ifdef UNIT_TEST_SEL_ERROR
        DEBUG(( EFI_D_ERROR, "DataHubStatus Codetype:%x\n",CodeType));
#endif

        if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
          LogToSel = TRUE;
          if ((CodeType & (EFI_STATUS_CODE_CLASS_MASK + EFI_STATUS_CODE_SUBCLASS_MASK)) == EFI_COMPUTING_UNIT_MEMORY) {
            Value = GenerateLegacyMemoryCode(DataRecord->Value, DataRecord->Instance);
            if (Value == 0) {
              LogToSel = FALSE;
            }
          } else {
            Status = MatchOperation (
                        DataRecord->Value,
                        &IsCustom, 
                        &OperationToken,
                        &LegacyErrorCode 
                        );

            if (EFI_ERROR(Status)){
              LogToSel = FALSE;
            }   
            Value = CompactStatusCode(DataRecord->Value, DataRecord->Instance,LegacyErrorCode, TRUE);

            if (Value == 0) {
              LogToSel = FALSE;
            }

          } 
         
          if (LogToSel) {
            Instance  = DataRecord->Instance;
            Status    = UpdateOemErrorStatusCode (
                          CodeType, 
                          Value, 
                          Instance, 
                          &SelRecord
                          );

            if (!EFI_ERROR (Status)) {
#ifdef UNIT_TEST_SEL_ERROR
              DEBUG(( EFI_D_ERROR, "SEL structure---sensor type:%x  OEM1:%x   OEM2:%X   OEM3:%X\n",SelRecord.SensorType,SelRecord.OEMEvData1,SelRecord.OEMEvData2,SelRecord.OEMEvData3));				
#endif
              Status = mGenericElog->SetEventLogData (
                                      mGenericElog,
                                      (UINT8*) &SelRecord,
                                      EfiElogSmIPMI,
                                      FALSE,
                                      sizeof (EFI_SEL_RECORD_DATA),
                                      &RecordId
                                      );
            }
          }
        }
      }

      //
      // break the while loop at the last record.
      //
      if (Mtc == 0)
          break;
    }
  }
}

VOID
EFIAPI
NotifyGenericElog (
  IN  EFI_EVENT   Event,
  IN  VOID        *Context
  )
{
  EFI_STATUS  Status;

  Status = gBS->LocateProtocol (
                  &gEfiGenericElogProtocolGuid,
                  mGenericElogReg,
                  &mGenericElog
                  );
  if (EFI_ERROR (Status)) {
    return;
  }
  WriteStatusRecordsToSel();

  return;
}



EFI_STATUS PostErrorToSelEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description:

  Entry point of the Error Manager screen setup driver.

Arguments:

  ImageHandle   EFI_HANDLE: A handle for the image that is initializing this driver
  SystemTable   EFI_SYSTEM_TABLE: A pointer to the EFI system table

Returns:

  EFI_SUCCESS:              Driver initialized successfully

--*/
{
  EFI_EVENT           mEvent;
  EFI_STATUS          Status;



  Status = EfiCreateEventReadyToBootEx (  
              TPL_NOTIFY, 
              NotifyGenericElog , 
              NULL, 
              &mEvent 
              );
  

  ASSERT_EFI_ERROR (Status);  


  return Status;
}

