/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  StatusCodeHandlerRuntimeDxe.c

Abstract: 
  Report Status Code Handler PEIM which produces general handlers and hook them
  onto the DXE status code router.

Revision History:

Bug 2517:   Create the Module StatusCodeHandler to report status code to 
            all supported device in ByoModule
TIME:       2011-7-22
$AUTHOR:    Liu Chunling
$REVIEWERS:  
$SCOPE:     All Platforms
$TECHNICAL:  
  1. Create the module StatusCodeHandler to support Serial Port, Memory, Port80,
     Beep and OEM devices to report status code.
  2. Create the Port80 map table and the Beep map table to convert status code 
     to code byte and beep times.
  3. Create new libraries to support status code when StatusCodePpi,
     StatusCodeRuntimeProtocol, SmmStatusCodeProtocol has not been installed yet.
$END--------------------------------------------------------------------

**/

#include "StatusCodeHandlerRuntimeDxe.h"

EFI_EVENT                 mVirtualAddressChangeEvent = NULL;
EFI_EVENT                 mExitBootServicesEvent     = NULL;
EFI_RSC_HANDLER_PROTOCOL  *mRscHandlerProtocol       = NULL;
BYO_STATUS_CODE_LEVEL     gByoStatusCodeLevel        = {0, NULL};
BOOLEAN                   gAllowShowPostCodeToScreen = TRUE;
BOOLEAN                   gAfterReadyToBoot          = FALSE;
BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL  *gEarlyConOut = NULL;


/**
  Unregister status code callback functions only available at boot time from
  report status code router when exiting boot services.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
StatusCodeHandlerRuntimeDxeCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  
  EFI_STATUS                Status;

  gBS->CloseEvent(Event);

  Status = gBS->LocateProtocol (
                  &gEfiRscHandlerProtocolGuid,
                  NULL,
                  (VOID **) &mRscHandlerProtocol
                  );
  ASSERT_EFI_ERROR (Status);

  if (FeaturePcdGet (PcdStatusCodeUseSerial)) {
    mRscHandlerProtocol->Register (SerialStatusCodeReportWorker, TPL_HIGH_LEVEL);
  }
  if (FeaturePcdGet (PcdStatusCodeUseMemory)) {
    mRscHandlerProtocol->Register (RtMemoryStatusCodeReportWorker, TPL_HIGH_LEVEL);
  }
  if (FeaturePcdGet (PcdStatusCodeUsePostCode)) {
    mRscHandlerProtocol->Register (Port80StatusCodeReportWorker, TPL_HIGH_LEVEL);
  }

  if (FeaturePcdGet (PcdStatusCodeUseBeep)) {
    mRscHandlerProtocol->Register (BeepStatusCodeReportWorker, TPL_HIGH_LEVEL);
  }
  if (FeaturePcdGet (PcdStatusCodeUseOem)) {
    mRscHandlerProtocol->Register (OemStatusCodeReportWorker, TPL_HIGH_LEVEL);
  }

}


/**
  Unregister status code callback functions only available at boot time from
  report status code router when exiting boot services.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
UnregisterBootTimeHandlers (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  if (FeaturePcdGet (PcdStatusCodeUseSerial)) {
    mRscHandlerProtocol->Unregister (SerialStatusCodeReportWorker);
  }

  if (FeaturePcdGet (PcdStatusCodeUsePostCode)) {
    mRscHandlerProtocol->Unregister (Port80StatusCodeReportWorker);
  }

  if (FeaturePcdGet (PcdStatusCodeUseBeep)) {
    mRscHandlerProtocol->Unregister (BeepStatusCodeReportWorker);
  }
  if (FeaturePcdGet (PcdStatusCodeUseOem)) {
    mRscHandlerProtocol->Unregister (OemStatusCodeReportWorker);
  }
}

/**
  Virtual address change notification call back. It converts global pointer
  to virtual address.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
VirtualAddressChangeCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  //
  // Convert memory status code table to virtual address;
  //
  EfiConvertPointer (
    0,
    (VOID **) &mRtMemoryStatusCodeTable
    );
}

/**
  Dispatch initialization request to sub status code devices based on 
  customized feature flags.
 
**/
VOID
InitializationDispatcherWorker (
  VOID
  )
{
  EFI_PEI_HOB_POINTERS              Hob;
  EFI_STATUS                        Status;
  MEMORY_STATUSCODE_PACKET_HEADER   *PacketHeader;
  MEMORY_STATUSCODE_RECORD          *Record;
  UINTN                             Index;
  UINTN                             MaxRecordNumber;

  //
  // If enable UseSerial, then initialize serial port.
  // if enable UseRuntimeMemory, then initialize runtime memory status code worker.
  //
  if (FeaturePcdGet (PcdStatusCodeUseSerial)) {
    //
    // Call Serial Port Lib API to initialize serial port.
    //
    Status = SerialPortInitialize ();
    ASSERT_EFI_ERROR (Status);
  }
  if (FeaturePcdGet (PcdStatusCodeUseMemory)) {
    Status = RtMemoryStatusCodeInitializeWorker ();
    ASSERT_EFI_ERROR (Status);
  }
  if (FeaturePcdGet (PcdStatusCodeUsePostCode)) {
    Status = Port80StatusCodeInitialize ();
    ASSERT_EFI_ERROR (Status);
  }
  if (FeaturePcdGet (PcdStatusCodeUseOem)) {
		Status = OemStatusCodeInitialize ();
    ASSERT_EFI_ERROR (Status);
  }

  //
  // Replay Status code which saved in GUID'ed HOB to all supported devices. 
  //
  if (FeaturePcdGet (PcdStatusCodeReplayIn)) {
    // 
    // Journal GUID'ed HOBs to find all record entry, if found, 
    // then output record to support replay device.
    //
    Hob.Raw   = GetFirstGuidHob (&gMemoryStatusCodeRecordGuid);
    if (Hob.Raw != NULL) {
      PacketHeader = (MEMORY_STATUSCODE_PACKET_HEADER *) GET_GUID_HOB_DATA (Hob.Guid);
      Record = (MEMORY_STATUSCODE_RECORD *) (PacketHeader + 1);
      MaxRecordNumber = (UINTN) PacketHeader->RecordIndex;
      if (PacketHeader->PacketIndex > 0) {
        //
        // Record has been wrapped around. So, record number has arrived at max number.
        //
        MaxRecordNumber = (UINTN) PacketHeader->MaxRecordsNumber;
      }
      for (Index = 0; Index < MaxRecordNumber; Index++) {
        //
        // Dispatch records to devices based on feature flag.
        //
        if (FeaturePcdGet (PcdStatusCodeUseSerial)) {
          SerialStatusCodeReportWorker (
            Record[Index].CodeType,
            Record[Index].Value,
            Record[Index].Instance,
            NULL,
            NULL
            );
        }
        if (FeaturePcdGet (PcdStatusCodeUseMemory)) {
          RtMemoryStatusCodeReportWorker (
            Record[Index].CodeType,
            Record[Index].Value,
            Record[Index].Instance,
            NULL,
            NULL
            );
        }
      }
    }
  }
}


VOID
EFIAPI
SchOnReadyToBoot (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  gBS->CloseEvent(Event);
  gAllowShowPostCodeToScreen = FALSE;
  gAfterReadyToBoot = TRUE;
}


VOID
SchSetupEnterCallback (
  IN EFI_EVENT Event,
  IN VOID     *Context
  )
{
  EFI_STATUS      Status;
  VOID            *Interface;

  Status = gBS->LocateProtocol(&gEfiSetupEnterGuid, NULL, &Interface);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);
  gAllowShowPostCodeToScreen = FALSE;  
}



VOID
EarlyConOutCallback (
  IN EFI_EVENT Event,
  IN VOID     *Context
  )
{
  EFI_STATUS                             Status;
  BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL  *EarlyConOut;

  Status = gBS->LocateProtocol(&gByoEarlySimpleTextOutProtocolGuid, NULL, (VOID**)&EarlyConOut);
  if (EFI_ERROR (Status)) {
    return;
  }
  gBS->CloseEvent(Event);

  gEarlyConOut = EarlyConOut;
}




/**
  Entry point of DXE Status Code Driver.

  This function is the entry point of this DXE Status Code Driver.
  It initializes registers status code handlers, and registers event for EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
StatusCodeHandlerRuntimeDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status;
  VOID                      *Registration;
  EFI_EVENT                 Event;
  
  //
  // Dispatch initialization request to supported devices
  //
  InitializationDispatcherWorker ();

  EfiCreateProtocolNotifyEvent (
		&gEfiRscHandlerProtocolGuid,
		TPL_CALLBACK,
		StatusCodeHandlerRuntimeDxeCallback,
		NULL,
		&Registration
		);

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  UnregisterBootTimeHandlers,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServicesEvent
                  );

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  VirtualAddressChangeCallBack,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mVirtualAddressChangeEvent
                  );
  ASSERT_EFI_ERROR (Status);

  Status = EfiCreateEventReadyToBootEx (
             TPL_CALLBACK,
             SchOnReadyToBoot,
             NULL,
             &Event
             );
  ASSERT_EFI_ERROR(Status);

  EfiCreateProtocolNotifyEvent (
    &gEfiSetupEnterGuid,
    TPL_CALLBACK,
    SchSetupEnterCallback,
    NULL,
    &Registration
    );

  EfiCreateProtocolNotifyEvent (
    &gByoEarlySimpleTextOutProtocolGuid,
    TPL_CALLBACK,
    EarlyConOutCallback,
    NULL,
    &Registration
    );

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gByoStatusCodeLevelOverrideProtocol, &gByoStatusCodeLevel,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
