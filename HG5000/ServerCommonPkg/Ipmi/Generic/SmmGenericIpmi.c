/** @file

Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SmmGenericIpmi.c

Abstract:
  Source file for the IPMI driver.

Revision History:

**/
//
// Statements that include other files
//
#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/SmmLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/IpmiBaseLib.h>
#include <EfiSmStatusCodes.h>
#include "IpmiBmcCommon.h"
#include "IpmiBmc.h"

EFI_IPMI_BMC_INSTANCE_DATA             *mIpmiInstance;
EFI_HANDLE                             mImageHandle;

EFI_STATUS
SmmInitializeIpmiKCSPhysicalLayer (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
/*++

Routine Description:
  Setup and initialize the BMC for the DXE phase.  In order to verify the BMC is functioning
  as expected, the BMC Selftest is performed.  The results are then checked and any errors are
  reported to the error manager.  Errors are collected throughout this routine and reported
  just prior to installing the driver.  If there are more errors than MAX_SOFT_COUNT, then they
  will be ignored.

Arguments:
  ImageHandle - Handle of this driver image
  SystemTable - Table containing standard EFI services

Returns: 
  EFI_SUCCESS - Successful driver initialization

--*/
{
  EFI_STATUS                       Status;
  UINT8                            ErrorCount;
  EFI_HANDLE                       Handle;
  EFI_STATUS_CODE_VALUE            StatusCodeValue[MAX_SOFT_COUNT];

  DEBUG ((EFI_D_ERROR,"SmmInitializeIpmiKCSPhysicalLayer entry \n"));
  ErrorCount = 0;
  mImageHandle = ImageHandle; 

  mIpmiInstance = AllocateZeroPool (sizeof (EFI_IPMI_BMC_INSTANCE_DATA));
  ASSERT (mIpmiInstance != NULL);

  //
  // Calibrate TSC Counter.  Stall for 10ms, then multiply the resulting number of
  // ticks in that period by 100 to get the number of ticks in a 1 second timeout
  //
  mIpmiInstance->KcsTimeoutPeriod = AsmReadTsc();
  gBS->Stall(10 * 1000);
  mIpmiInstance->KcsTimeoutPeriod = MultU64x32 ((AsmReadTsc() - mIpmiInstance->KcsTimeoutPeriod), BMC_KCS_TIMEOUT * 100);
  DEBUG((EFI_D_INFO, "%a() KcsTimeoutPeriod:%lX\n",__FUNCTION__, mIpmiInstance->KcsTimeoutPeriod));
  //
  // Initialize IPMI IO Base, we still use SMS IO base to get device ID and Seltest result since SMM IF may have different cmds supported
  //
  mIpmiInstance->IpmiIoBase                     = PcdGet16(IpmiIoBase);
  mIpmiInstance->Signature                      = SM_IPMI_BMC_SIGNATURE;
  mIpmiInstance->SlaveAddress                   = BMC_SLAVE_ADDRESS;
  mIpmiInstance->BmcStatus                      = EFI_BMC_OK;
  mIpmiInstance->IpmiTransport.SendIpmiCommand  = EfiIpmiSendCommand;
  mIpmiInstance->IpmiTransport.GetBmcStatus     = EfiIpmiBmcStatus;

  DEBUG ((EFI_D_ERROR,"IPMI: Waiting for Getting BMC DID in SMM \n"));
  //
  // Get the Device ID and check if the system is in Force Update mode.
  //
  // Just obey the Spec.. 
  // If we want to improve performance, we're going to comment it.
  //
  Status = GetDeviceId (
             mIpmiInstance,
             StatusCodeValue,
             &ErrorCount
             );
  ASSERT_EFI_ERROR (Status);

  Handle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gSmmIpmiTransportProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mIpmiInstance->IpmiTransport
                    );
  ASSERT_EFI_ERROR (Status);

  DEBUG ((EFI_D_ERROR,"SmmInitializeIpmiKCSPhysicalLayer exit \n"));
  
  return EFI_SUCCESS;
}

EFI_STATUS
InitializeSmmGenericIpmi (
  IN EFI_HANDLE             ImageHandle,
  IN EFI_SYSTEM_TABLE       *SystemTable
  )
{
  SmmInitializeIpmiKCSPhysicalLayer (ImageHandle, SystemTable);
  return EFI_SUCCESS;
}
