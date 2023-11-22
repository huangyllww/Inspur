/** @file

Copyright (c) 2006 - 2011, Byosoft Corporation.<BR> 
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
            all supported devide in ByoModule
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

#include <PiDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Protocol/ReportStatusCodeHandler.h>
#include <Guid/EventGroup.h>
#include <Library/DebugLib.h>

EFI_EVENT                 mVirtualAddressChangeEvent = NULL;
EFI_EVENT                 mExitBootServicesEvent     = NULL;
EFI_RSC_HANDLER_PROTOCOL  *mRscHandlerProtocol       = NULL;
BOOLEAN                   gHotKeyBoot;

EFI_STATUS
EFIAPI
EarlyVideoStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN EFI_GUID                       *CallerId,
  IN EFI_STATUS_CODE_DATA           *Data OPTIONAL
  );

VOID AfterConnectPciRootBridgeHook();

VOID BeforeHotKeyBoot()
{
  gHotKeyBoot = TRUE;
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

  mRscHandlerProtocol->Register (EarlyVideoStatusCodeReportWorker, TPL_HIGH_LEVEL);

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
  mRscHandlerProtocol->Unregister (EarlyVideoStatusCodeReportWorker);

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
  VOID                      *Registration;

  EfiCreateProtocolNotifyEvent (
		&gEfiRscHandlerProtocolGuid,
		TPL_CALLBACK,
		StatusCodeHadlerRuntimeDxeCallback,
		NULL,
		&Registration
		);

  gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  UnregisterBootTimeHandlers,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServicesEvent
                  );

  gBS->InstallMultipleProtocolInterfaces(
                  &ImageHandle,
                  &gEfiAfterConnectPciRootBridgeGuid, AfterConnectPciRootBridgeHook,
                  &gByoBeforeHotKeyBootProtocolGuid, BeforeHotKeyBoot,
                  NULL
                  );

  return EFI_SUCCESS;
}
