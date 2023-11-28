/** @file

Copyright (c) 2006 - 2018, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SmmPlatform.c

Abstract:
  Source file for the Smm platform driver.

Revision History:

**/

#include "SmmPlatform.h"
#include <Protocol/FchSmmSwDispatch2.h>
#include <Protocol/FchSmmSxDispatch2.h>
#include <Fch.h>
#include <Protocol/ByoCommSmiSvcProtocol.h>
#include <SetupVariable.h>
#include <Protocol/SmmPowerButtonDispatch2.h>
#include <PlatS3Record.h>
#include <UefiCpuPkg/PiSmmCommunication/PiSmmCommunicationPrivate.h>


SMMCALL_ENTRY             *mSmmCallTablePtr = NULL;
UINT16                    mAcpiBaseAddr;
SETUP_DATA                gSetupData;
PLATFORM_S3_RECORD        *gS3Record = NULL;

VOID EndLegacyUsb()
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  if (mSmmCallTablePtr) {
    mSmmCallTablePtr[__ID_EndLegacyUsb]();
  }
}


EFI_STATUS
EFIAPI
SmiAcpiOnCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  DEBUG((EFI_D_INFO, "%a()\n", __FUNCTION__));

  REPORT_STATUS_CODE (
    EFI_PROGRESS_CODE, 
    (EFI_SOFTWARE_SMM_DRIVER | BYO_ACPI_ENABLE)
    );
  
  IoWrite16(0x80, 0);

// Guarantee day-of-month alarm is invalid (ACPI 1.0 section 4.7.2.4)  
  CmosWrite(0x0D, 0);

  // Disable PM sources except power button
  IoWrite16(mAcpiBaseAddr + PMIO_PM_EN, PMIO_PM_EN_PWRBTN);
  IoWrite16(mAcpiBaseAddr + PMIO_STS_REG, 0xFFFF);

// [15] TimerEn. Read-write. Reset: 0. 0=Disable. 1=Enable the SMI short Timer or long timer, 
// which is selected by FCH::SMI::SmiTrig0[SmiTimerSel]
  MmioAnd16(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96, (UINT16)~BIT15);     //Disable SMI Timer

//[31:30] Smicontrol143. Read-write. Reset: 0. Control for Long timer.
//[29:28] Smicontrol142. Read-write. Reset: 0. Control for Short timer.
//          00b Disable.
//          01b SMI
  MmioAnd32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC0, (UINT32)~((BIT28 | BIT29 | BIT30 | BIT31))); 

//[7:6] Smicontrol51. Read-write. Reset: 0. Control for Power button event
  MmioAnd32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC, (UINT32)~((BIT6 | BIT7)));

  EndLegacyUsb();

  return EFI_SUCCESS;
}




EFI_STATUS
LegacyUsbCallback (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS                    Status;
  EFI_LEGACY_USB_INF_PROTOCOL   *LegacyUsbInf;

  Status = gSmst->SmmLocateProtocol (
                  &gEfiLegacyUsbInfProtocolGuid,
                  NULL,
                  &LegacyUsbInf
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mSmmCallTablePtr = LegacyUsbInf->SmmCallTablePtr;
  return EFI_SUCCESS;
}



VOID SleepCommonHandler(UINT8 SleepType)
{
  EFI_STATUS                 Status;
  BYO_COMM_SMI_SVC_PROTOCOL  *CommSmiSvc;

  DEBUG((EFI_D_INFO, "SleepCommonHandler(%d)\n", SleepType));

  Status = gSmst->SmmLocateProtocol (
                    &gByoCommSmiSvcProtocolGuid,
                    NULL,
                    (VOID**)&CommSmiSvc
                    );
  if(!EFI_ERROR(Status)){
    CommSmiSvc->SleepCallback(SleepType);
  }  
}


EFI_STATUS
S3SleepEntryCallBack (
  IN     EFI_HANDLE               DispatchHandle,
  IN     CONST VOID               *Context,        OPTIONAL
  IN OUT VOID                     *CommBuffer,     OPTIONAL
  IN OUT UINTN                    *CommBufferSize  OPTIONAL
)
{
  SleepCommonHandler(3);
  return EFI_SUCCESS;
}


EFI_STATUS
S4SleepEntryCallBack (
  IN     EFI_HANDLE               DispatchHandle,
  IN     CONST VOID               *Context,        OPTIONAL
  IN OUT VOID                     *CommBuffer,     OPTIONAL
  IN OUT UINTN                    *CommBufferSize  OPTIONAL
)
{
  SleepCommonHandler(4);
  return EFI_SUCCESS;
}

EFI_STATUS
S5SleepEntryCallBack (
  IN     EFI_HANDLE               DispatchHandle,
  IN     CONST VOID               *Context,        OPTIONAL
  IN OUT VOID                     *CommBuffer,     OPTIONAL
  IN OUT UINTN                    *CommBufferSize  OPTIONAL
)
{
  SleepCommonHandler(5);
  return EFI_SUCCESS;
}



EFI_STATUS
PowerButtonCallback (
  IN     EFI_HANDLE               DispatchHandle,
  IN     CONST VOID               *Context,        OPTIONAL
  IN OUT VOID                     *CommBuffer,     OPTIONAL
  IN OUT UINTN                    *CommBufferSize  OPTIONAL
)
{
  SleepCommonHandler(0xFF);
  return EFI_SUCCESS;
}




VOID SetS3RecordSmmCommCtx()
{
  UINTN                           Index;
  EFI_SMM_COMMUNICATION_CONTEXT   *Ctx;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  if(gS3Record == NULL){
    return;
  }

  for (Index = 0; Index < gSmst->NumberOfTableEntries; Index++) {
    if (CompareGuid(&gSmst->SmmConfigurationTable[Index].VendorGuid, &gEfiPeiSmmCommunicationPpiGuid)) {
      Ctx = (EFI_SMM_COMMUNICATION_CONTEXT*)gSmst->SmmConfigurationTable[Index].VendorTable;
      ASSERT(Ctx->Signature == SMM_COMMUNICATION_SIGNATURE);
      ASSERT(sizeof(gS3Record->SmmCommCtx) >= sizeof(EFI_SMM_COMMUNICATION_CONTEXT));
      CopyMem(gS3Record->SmmCommCtx, Ctx, sizeof(EFI_SMM_COMMUNICATION_CONTEXT));
      gS3Record->SmmCommCtxSize = sizeof(EFI_SMM_COMMUNICATION_CONTEXT);
      DEBUG((EFI_D_INFO, "SmmCommCtxSize:%d\n", gS3Record->SmmCommCtxSize));
      break;
    }
  }
}




EFI_STATUS
EFIAPI
SmmReadyToBootNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  SetS3RecordSmmCommCtx();
  return EFI_SUCCESS;
}



EFI_STATUS
InitializePlatformSmm (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS                                Status;
  EFI_HANDLE                                SwHandle = NULL;
  FCH_SMM_SW_DISPATCH2_PROTOCOL             *AmdSwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT               SwRegisterContext;
  VOID                                      *Registration;
  FCH_SMM_SX_DISPATCH2_PROTOCOL             *AmdSxDispatch;
  FCH_SMM_SX_REGISTER_CONTEXT               SxRegisterContext;
  EFI_SMM_POWER_BUTTON_DISPATCH2_PROTOCOL   *PowerButtonDispatch; 
  EFI_SMM_POWER_BUTTON_REGISTER_CONTEXT     PowerButtonContext;  
  SETUP_DATA                                *SetupHob;
  EFI_HANDLE                                PowerButtonHandle = NULL;
  

  SetupHob = GetSetupDataHobData();
  CopyMem(&gSetupData, SetupHob, sizeof(SETUP_DATA));

  mAcpiBaseAddr = PcdGet16(AcpiIoPortBaseAddress);

#if _PCD_VALUE_PcdAcpiS3Enable
  gS3Record = (PLATFORM_S3_RECORD*)GetS3RecordTable();
  DEBUG((EFI_D_INFO, "gS3Record:%X\n", gS3Record));
#endif  

  Status = gSmst->SmmLocateProtocol(&gFchSmmSwDispatch2ProtocolGuid, NULL, &AmdSwDispatch);
  ASSERT_EFI_ERROR (Status);

  SwRegisterContext.AmdSwValue  = PcdGet8(PcdFchOemEnableAcpiSwSmi);
  SwRegisterContext.Order       = 0x70;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            SmiAcpiOnCallback,
                            &SwRegisterContext,
                            &SwHandle
                            );
  ASSERT(!EFI_ERROR (Status));

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiLegacyUsbInfProtocolGuid,
                    LegacyUsbCallback,
                    &Registration
                    );
  LegacyUsbCallback (NULL, NULL, NULL);


  //
  // Register AMD SX SMM
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSxDispatch2ProtocolGuid,
                  NULL,
                  &AmdSxDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  SxRegisterContext.Type  = SxS3;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 0;
  Status = AmdSxDispatch->Register (
                             AmdSxDispatch,
                             S3SleepEntryCallBack,
                             &SxRegisterContext,
                             &SwHandle
                             );

  SxRegisterContext.Type  = SxS4;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 0;
  Status = AmdSxDispatch->Register (
                             AmdSxDispatch,
                             S4SleepEntryCallBack,
                             &SxRegisterContext,
                             &SwHandle
                             );

  SxRegisterContext.Type  = SxS5;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 0;
  Status = AmdSxDispatch->Register (
                             AmdSxDispatch,
                             S5SleepEntryCallBack,
                             &SxRegisterContext,
                             &SwHandle
                             );

  Status = gSmst->SmmLocateProtocol (
                    &gEfiSmmPowerButtonDispatch2ProtocolGuid,
                    NULL,
                    &PowerButtonDispatch
                    );
  ASSERT_EFI_ERROR (Status);

  PowerButtonContext.Phase = EfiPowerButtonEntry;
  Status = PowerButtonDispatch->Register (
                                  PowerButtonDispatch,
                                  PowerButtonCallback,
                                  &PowerButtonContext,
                                  &PowerButtonHandle
                                  );
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEdkiiSmmReadyToBootProtocolGuid,
                    SmmReadyToBootNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR(Status);
  
  return EFI_SUCCESS;
}



