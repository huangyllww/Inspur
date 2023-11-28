/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR> 
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

**/

#include "StatusCodeHandlerRuntimeDxe.h"

EFI_EVENT                 mVirtualAddressChangeEvent = NULL;
EFI_EVENT                 mExitBootServicesEvent     = NULL;
EFI_RSC_HANDLER_PROTOCOL  *mRscHandlerProtocol       = NULL;
extern BYO_EARLY_SIMPLE_TEXT_OUTPUT_PROTOCOL *mByoEarlySimpleTextOutProtocol;
/**
  Unregister status code callback functions only available at boot time from
  report status code router when exiting boot services.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
VOID
EFIAPI
StatusCodeHadlerRuntimeDxeCallback (
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


  mRscHandlerProtocol->Register (Port80StatusCodeReportWorker, TPL_HIGH_LEVEL);

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

  mRscHandlerProtocol->Unregister (Port80StatusCodeReportWorker);

}

VOID ProjectAfterConnectPciRootBridgeHook()
{
  EFI_STATUS                   Status;
  UINTN                        HandleCount;
  EFI_HANDLE                   *HandleBuffer = NULL;
  UINTN                        Index;
  EFI_PCI_IO_PROTOCOL          *ptPciIo;
  UINT16                       PciId;
  UINT8                        ClassCode[3]; 
  UINT64                       MemBar = 0;

  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  Status = gBS->LocateHandleBuffer(
                  ByProtocol, 
                  &gEfiPciIoProtocolGuid, 
                  NULL, 
                  &HandleCount, 
                  &HandleBuffer
                  );
  if(EFI_ERROR(Status) || HandleCount == 0){
    DEBUG((EFI_D_ERROR, "(L%d) %r %d\n", __LINE__, Status, HandleCount));
    goto ProcExit;
  }

  for(Index = 0; Index < HandleCount; Index ++) {
    Status = gBS->HandleProtocol(
                    HandleBuffer[Index],
                    &gEfiPciIoProtocolGuid, 
                    &ptPciIo
                    );
    ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint8, PCI_CLASSCODE_OFFSET, 3, &ClassCode[0]);
    if(ClassCode[2] != 3){
      continue;
    }
    ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint16, PCI_VENDOR_ID_OFFSET, 1, &PciId);
    if(PciId != 0x1A03){
      continue;
    }

    ptPciIo->Pci.Read(ptPciIo, EfiPciIoWidthUint32, PCI_BASE_ADDRESSREG_OFFSET, 1, &MemBar);
    if (MemBar && (mByoEarlySimpleTextOutProtocol != NULL)) {
      mByoEarlySimpleTextOutProtocol->SetFrameBuffer (MemBar);
    }
    break;
  }

ProcExit:
  if(HandleBuffer!=NULL){gBS->FreePool(HandleBuffer);}
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
EarlyVideoStatusCodeHandlerRuntimeDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status;
  VOID                      *Registration;

  EfiCreateProtocolNotifyEvent (
        &gEfiRscHandlerProtocolGuid,
        TPL_CALLBACK,
        StatusCodeHadlerRuntimeDxeCallback,
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

  Status = gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gEfiAfterConnectPciRootBridgeGuid, ProjectAfterConnectPciRootBridgeHook,
                  NULL
                  );

  return Status;
}
