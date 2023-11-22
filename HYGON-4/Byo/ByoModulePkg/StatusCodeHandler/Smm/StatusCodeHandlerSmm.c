/** @file

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR> 
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in 
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced, 
stored in a retrieval system, or transmitted in any form or by any 
means without the express written consent of Byosoft Corporation.

File Name:
  StatusCodeHandlerSmm.c

Abstract: 
  Report Status Code Handler PEIM which produces general handlers and hook them
  onto the SMM status code router.

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

#include "StatusCodeHandlerSmm.h"
#include "Library/BaseLib.h"
EFI_SMM_RSC_HANDLER_PROTOCOL  *mRscHandlerProtocol       = NULL;
BYO_STATUS_CODE_LEVEL         gByoStatusCodeLevel        = {0, NULL};
BOOLEAN                       gAfterReadyToBoot          = FALSE;

/**
  Unregister status code callback functions only available at boot time from
  report status code router when exiting boot services.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.

**/
EFI_STATUS
EFIAPI
StatusCodeHandlerSmmCallback (
  IN CONST EFI_GUID                       *Protocol,
  IN VOID                                 *Interface,
  IN EFI_HANDLE                           Handle
  )
{
  
  EFI_STATUS      Status;
  STATIC BOOLEAN  HasDone = FALSE;     

  if(HasDone){
    goto ProcExit;
  }
  
  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmRscHandlerProtocolGuid,
                    NULL,
                    (VOID**)&mRscHandlerProtocol
                    );
  if(EFI_ERROR(Status)){
    goto ProcExit;
  }
  
  HasDone = TRUE;
                    
  if (FeaturePcdGet (PcdStatusCodeUseSerial)) {
    mRscHandlerProtocol->Register (SerialStatusCodeReportWorker);
  }
  if (FeaturePcdGet (PcdStatusCodeUseMemory)) {
    mRscHandlerProtocol->Register (MemoryStatusCodeReportWorker);
  }
  if (FeaturePcdGet (PcdStatusCodeUsePostCode)) {
    mRscHandlerProtocol->Register (Port80StatusCodeReportWorker);
  }
  if (FeaturePcdGet (PcdStatusCodeUseBeep)) {
    mRscHandlerProtocol->Register (BeepStatusCodeReportWorker);
  }
  if (FeaturePcdGet (PcdStatusCodeUseOem)) {
    mRscHandlerProtocol->Register (OemStatusCodeReportWorker);
  }
	
ProcExit:  
  return EFI_SUCCESS;
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
  EFI_STATUS                        Status;

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
    Status = MemoryStatusCodeInitializeWorker ();
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

}


EFI_STATUS
EFIAPI
SchReadyToBootNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  gAfterReadyToBoot = TRUE;
  return EFI_SUCCESS;  
}

/**
  Entry point of SMM Status Code Driver.

  This function is the entry point of SMM Status Code Driver.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.
  
  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
StatusCodeHandlerSmmEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                Status;
  VOID                      *Registeration;
  EFI_HANDLE                Handle = NULL;
  VOID                      *Registration;
 
  
  //
  // Dispatch initialization request to supported devices
  //
  InitializationDispatcherWorker ();

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmRscHandlerProtocolGuid,
                    StatusCodeHandlerSmmCallback,
                    &Registeration
                    );
  ASSERT_EFI_ERROR (Status);
  StatusCodeHandlerSmmCallback(NULL, NULL, NULL);    // fire it immediately as the protocol maybe already has been installed.

  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gByoStatusCodeLevelOverrideProtocol,
                    EFI_NATIVE_INTERFACE,
                    &gByoStatusCodeLevel
                    );
  ASSERT_EFI_ERROR (Status);  

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEdkiiSmmReadyToBootProtocolGuid,
                    SchReadyToBootNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR(Status);
  
  return EFI_SUCCESS;
}
