/** @file

Copyright (c) 2006 - 2023, Byosoft Corporation.<BR>
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
#include <Protocol/NvMediaAccess.h>
#include <Protocol/NvMediaDevice.h>
#include <Protocol/FchSmmIoTrapDispatch2.h>
#include <PlatS3Record.h>
#include <Library/ByoSharedSmmData.h>
#include <Library/ByoHygonCommLib.h>


SMMCALL_ENTRY             *mSmmCallTablePtr = NULL;
UINT16                    mAcpiBaseAddr;
SETUP_DATA                gSetupData;
PLATFORM_S3_RECORD        *gS3Record = NULL;
UINT8                     gPciHostCount;
UINT8                     *gSystemInLegacyPtr = NULL;
BOOLEAN                   gIsAcpiOn = FALSE;
EFI_HANDLE                gCf9IoTrapHandle = NULL;

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

  gIsAcpiOn = TRUE;

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
                  (VOID**)&LegacyUsbInf
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mSmmCallTablePtr = LegacyUsbInf->SmmCallTablePtr;
  return EFI_SUCCESS;
}



EFI_STATUS
SmmNvMediaDeviceCallback (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS                    Status;
  NV_MEDIA_DEVICE_PROTOCOL      *NvDev;
  MEDIA_BLOCK_MAP               *BlockMap;
  

  if(gByoSharedSmmData->FlashSize){
    return EFI_ALREADY_STARTED;
  }

  Status = gSmst->SmmLocateProtocol (
                  &gEfiSmmNvMediaDeviceProtocolGuid,
                  NULL,
                  (VOID**)&NvDev
                  );
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_INFO, "SmmNvMediaDevice %r\n", Status));
    return Status;
  }

  Status = NvDev->Info(NvDev, &BlockMap);
  if (EFI_ERROR (Status)) {
    DEBUG((EFI_D_ERROR, "NvDev->Info:%r\n", Status));
    return Status;
  }

  gByoSharedSmmData->FlashSize = (1 << BlockMap->Size) * BlockMap->Count;
  
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

// SMIx004 [Event_Enable] (EventEn) 
//   This is the mirror register of the standard ACPI register FCH::PM::EventEnable. 
//   Each bit controls whether ACPI should generate wake up and SCI interrupt.
// PMx0818 [EventEnable] (EventEnable) 
//
// { SMI_BASE >> 8,  FCH_SMI_TS_XhcWake,     0,    11                        },
//

  MmioOr32(0xFED80204, BIT11);  
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


EFI_STATUS PlatformCf9IoTrapUnRegister()
{
  FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL        *AmdIoTrapDispatch;
  EFI_STATUS                                Status;

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmIoTrapDispatch2ProtocolGuid,
                  NULL,
                  &AmdIoTrapDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  IoWrite8(0xcd6, 0xD2);
  IoWrite8(0xcd7, IoRead8(0xcd7) & ((UINT8)~BIT6));

  Status = AmdIoTrapDispatch->UnRegister (
                                AmdIoTrapDispatch,
                                gCf9IoTrapHandle
                                );
  ASSERT_EFI_ERROR (Status);

  return Status;
}

EFI_STATUS
EFIAPI
SmmReadyToBootNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  if(PcdGetBool(PcdPlatformCf9IoTrapSupport)){
    PlatformCf9IoTrapUnRegister();
  }

  return EFI_SUCCESS;
}


EFI_STATUS
EFIAPI
PlatformIoTrapSmiCallback (
  IN       EFI_HANDLE   DispatchHandle,
  IN       CONST VOID   *Context        OPTIONAL,
  IN OUT   VOID         *CommBuffer     OPTIONAL,
  IN OUT   UINTN        *CommBufferSize OPTIONAL
  )
{
  UINT8   Cf9Value;


  DEBUG((EFI_D_INFO, "PlatformIoTrapSmiCallback\n"));

  IoWrite8(0xcd6, 0xD2);
  IoWrite8(0xcd7, IoRead8(0xcd7) & ((UINT8)~BIT6));

  Cf9Value = IoRead8(0xcf9);
  if(FixedPcdGetBool(PcdForceLegacyColdReset) && (*gSystemInLegacyPtr) && !gIsAcpiOn){
    Cf9Value |= 6;  
    IoWrite8(0xcd6, 0x10);
    IoWrite8(0xcd7, IoRead8(0xcd7) | BIT1);
  } else {
    Cf9Value |= 4;
  }
  DEBUG((EFI_D_INFO, "[cf9]:%x\n", Cf9Value));
  DEBUG((EFI_D_INFO, "\n\n\n\n"));  
  IoWrite8(0xcf9, Cf9Value);
  CpuDeadLoop();

  return EFI_SUCCESS;
}



EFI_STATUS PlatformCf9IoTrapInit()
{
  FCH_SMM_IO_TRAP_DISPATCH2_PROTOCOL        *AmdIoTrapDispatch;
  FCH_SMM_IO_TRAP_REGISTER_CONTEXT          IoTrapRegisterContext;
  EFI_STATUS                                Status;

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmIoTrapDispatch2ProtocolGuid,
                  NULL,
                  &AmdIoTrapDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  IoWrite8(0xcd6, 0xD2);
  IoWrite8(0xcd7, IoRead8(0xcd7) | BIT6);

  IoTrapRegisterContext.Address = 0xcf9;
  IoTrapRegisterContext.Length  = 1;
  IoTrapRegisterContext.Type    = WriteTrap;
  Status = AmdIoTrapDispatch->Register (
                                AmdIoTrapDispatch,
                                PlatformIoTrapSmiCallback,
                                &IoTrapRegisterContext,
                                &gCf9IoTrapHandle
                                );
  ASSERT_EFI_ERROR (Status);

  return Status;
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
  PLATFORM_COMM_INFO                        *Info;
  UINT8                                     *Data8;


  SetupHob = GetSetupDataHobData();
  CopyMem(&gSetupData, SetupHob, sizeof(SETUP_DATA));

  mAcpiBaseAddr = PcdGet16(AcpiIoPortBaseAddress);

  if(FixedPcdGetBool(PcdAcpiS3Enable)){
    gS3Record = (PLATFORM_S3_RECORD*)GetS3RecordTable();
    DEBUG((EFI_D_INFO, "gS3Record:%X\n", gS3Record));
  }

  Info = GetPlatformCommInfo();
  gPciHostCount = (UINT8)Info->PciHostCount;

  PlatSmiFlashExInit();

  Status = gSmst->SmmLocateProtocol(&gFchSmmSwDispatch2ProtocolGuid, NULL, &AmdSwDispatch);
  ASSERT_EFI_ERROR (Status);

  SwRegisterContext.HygonSwValue = PcdGet8(PcdFchOemEnableAcpiSwSmi);
  SwRegisterContext.Order        = 0x70;
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

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEfiSmmNvMediaDeviceProtocolGuid,
                    SmmNvMediaDeviceCallback,
                    &Registration
                    );
  SmmNvMediaDeviceCallback(NULL, NULL, NULL);

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSxDispatch2ProtocolGuid,
                  NULL,
                  &AmdSxDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  SxRegisterContext.Type  = SxS3;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 10;
  Status = AmdSxDispatch->Register (
                             AmdSxDispatch,
                             S3SleepEntryCallBack,
                             &SxRegisterContext,
                             &SwHandle
                             );

  SxRegisterContext.Type  = SxS4;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 10;
  Status = AmdSxDispatch->Register (
                             AmdSxDispatch,
                             S4SleepEntryCallBack,
                             &SxRegisterContext,
                             &SwHandle
                             );

  SxRegisterContext.Type  = SxS5;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 10;
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

  gSystemInLegacyPtr = (UINT8*)(UINTN)PcdGet64(PcdSystemInLegacyPtr);
  if(gSystemInLegacyPtr == NULL){
    Status = gBS->AllocatePool(
                    EfiReservedMemoryType,
                    sizeof(UINT8),
                    (VOID**)&Data8
                    );
    ASSERT(!EFI_ERROR(Status));
    *Data8 = 0;
    PcdSet64S(PcdSystemInLegacyPtr, (UINTN)Data8);
    gSystemInLegacyPtr = Data8;
  }
  DEBUG((EFI_D_INFO, "SystemInLegacyPtr:%x\n", gSystemInLegacyPtr));

  if(PcdGetBool(PcdPlatformCf9IoTrapSupport)){
    PlatformCf9IoTrapInit();
  }

  return EFI_SUCCESS;
}



