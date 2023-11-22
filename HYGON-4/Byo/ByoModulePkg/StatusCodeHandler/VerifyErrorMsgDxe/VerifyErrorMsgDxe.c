/** @file

Copyright (c) 2006 - 2021, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  VerifyErrorMsgDxe.c

Abstract:
  Register a ReportStatusCode callback function to show error messages
when verifying boot image failed.

Revision History:

**/

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/SetupUiLib.h>
#include <Library/HiiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

#include <Protocol/ReportStatusCodeHandler.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Pi/PiStatusCode.h>
#include <Protocol/BootLogo.h>

EFI_HII_HANDLE gStringHandle;

VOID
ShowWarningMsg (
  VOID
  )
{
  CHAR16                      *Str1 = NULL;
  CHAR16                      *Str2 = NULL;
  EFI_STATUS                  Status;
  EFI_BOOT_LOGO_PROTOCOL      *BootLogo;

  ASSERT (gStringHandle!= NULL);

  gST->ConOut->ClearScreen(gST->ConOut);
  
  Status = gBS->LocateProtocol(&gEfiBootLogoProtocolGuid, NULL, (VOID**)&BootLogo);
  if (!EFI_ERROR(Status)) {
    BootLogo->SetBootLogo (BootLogo, NULL, 0, 0, 0, 0);
  }

  Str1 = HiiGetString (gStringHandle, STRING_TOKEN(STR_OS_MISTYPED), NULL);
  Str2 = HiiGetString (gStringHandle, STRING_TOKEN(STR_OS_ENTER_STRING), NULL);
  gST->ConOut->EnableCursor (gST->ConOut, FALSE);
  while (TRUE) {
    if (SELECTION_YES == UiConfirmDialog(DIALOG_WARNING, NULL, NULL, TEXT_ALIGIN_CENTER, Str1,Str2, NULL)) {
      FreePool (Str1);
      FreePool (Str2);
      break;
    }
  }

  gST->ConOut->ClearScreen(gST->ConOut);
  
}

/**
  Handle status codes reported through ReportStatusCodeLib /
  EFI_STATUS_CODE_PROTOCOL.ReportStatusCode().Showing error messages
  when verifying boot image failed.
  @param[in] Event    Event whose notification function is being invoked.

  @param[in] Context  Pointer to EFI_RSC_HANDLER_PROTOCOL, originally looked up
                      when HandleStatusCode() was registered.
**/
STATIC
EFI_STATUS
EFIAPI
HandleStatusCode (
  IN EFI_STATUS_CODE_TYPE  CodeType,
  IN EFI_STATUS_CODE_VALUE Value,
  IN UINT32                Instance,
  IN EFI_GUID              *CallerId,
  IN EFI_STATUS_CODE_DATA  *Data
  )
{
  EFI_STATUS ReturnStatus;

  if ((CodeType == (EFI_ERROR_CODE | EFI_ERROR_MINOR)) && (Value == (EFI_SOFTWARE_DXE_BS_DRIVER |EFI_SW_DXE_BS_EC_BOOT_OPTION_LOAD_ERROR))
    && (Data != NULL)) {
    //
    // Indicate that the failure to load boot option
    //
    ReturnStatus = ((EFI_RETURN_STATUS_EXTENDED_DATA *)Data)->ReturnStatus;
    if (ReturnStatus == EFI_ACCESS_DENIED || ReturnStatus == EFI_SECURITY_VIOLATION) {
      ShowWarningMsg ();
    }
  }

  return EFI_SUCCESS;
}


/**
  Unregister HandleStatusCode() at ExitBootServices().

  (See EFI_RSC_HANDLER_PROTOCOL in Volume 3 of the Platform Init spec.)

  @param[in] Event    Event whose notification function is being invoked.

  @param[in] Context  Pointer to EFI_RSC_HANDLER_PROTOCOL, originally looked up
                      when HandleStatusCode() was registered.
**/
STATIC
VOID
EFIAPI
UnregisterAtExitBootServices (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
{
  EFI_RSC_HANDLER_PROTOCOL *StatusCodeRouter;

  StatusCodeRouter = Context;
  StatusCodeRouter->Unregister (HandleStatusCode);
}

/**
  The module Entry Point of the Firmware Performance Data Table DXE driver.
  Register a status code handler for showing error messages when verifying
  boot image failed.

  @param[in]  ImageHandle    The firmware allocated handle for the EFI image.
  @param[in]  SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS    The entry point is executed successfully.
  @retval Other          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
VerifyErrorMsgDxeEntryPoint (
  IN EFI_HANDLE          ImageHandle,
  IN EFI_SYSTEM_TABLE    *SystemTable
  )
{
  EFI_STATUS               Status;
  EFI_RSC_HANDLER_PROTOCOL *StatusCodeRouter;
  EFI_EVENT                ExitBootEvent;

  Status = gBS->LocateProtocol (&gEfiRscHandlerProtocolGuid, NULL, (VOID **)&StatusCodeRouter);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Register the handler callback.
  //
  Status = StatusCodeRouter->Register (HandleStatusCode, TPL_HIGH_LEVEL);
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "%a:%a: failed to register status code handler: %r\n",
      gEfiCallerBaseName, __FUNCTION__, Status));
    return Status;
  }

  //
  // Unregister the handler callback before exiting from the Boot Time
  //
  Status = gBS->CreateEvent (
                  EVT_SIGNAL_EXIT_BOOT_SERVICES, // Type
                  TPL_CALLBACK,                  // NotifyTpl
                  UnregisterAtExitBootServices,  // NotifyFunction
                  StatusCodeRouter,              // NotifyContext
                  &ExitBootEvent                 // Event
                  );
  if (EFI_ERROR (Status)) {
    //
    // We have to unregister the callback right now, and fail the function.
    //
    DEBUG ((DEBUG_ERROR, "%a:%a: failed to create ExitBootServices() event: "
      "%r\n", gEfiCallerBaseName, __FUNCTION__, Status));
    StatusCodeRouter->Unregister (HandleStatusCode);
    return Status;
  }

  //
  // Add error message strings
  //
  gStringHandle = HiiAddPackages (
                            &gEfiCallerIdGuid,
                            ImageHandle,
                            STRING_ARRAY_NAME,
                            NULL
                            );
  ASSERT (gStringHandle!= NULL);

  return EFI_SUCCESS;
}
