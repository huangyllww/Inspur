/** @file

Copyright (c) 2006 - 2020, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  ProjectSmm.c

Abstract:
  Source file for the Smm platform driver.

Revision History:

**/

#include "ProjectSmm.h"
#include <Protocol/SmmVariable.h>
#include <Fch.h>


UINT16                          mAcpiBaseAddr;
SETUP_DATA                      gSetupData;
BYO_COMM_SMI_SVC_PROTOCOL       gByoCommSmiSvc;
BOOLEAN                         gSmmReadyToBoot = FALSE;



VOID ProjectSleepCommonHandler(UINT8 SleepType)
{
  if(SleepType == 4 || SleepType == 5 || SleepType == 0xFF){

    DEBUG((EFI_D_INFO, "%a(),WakeOnLan:%x\n", __FUNCTION__,gSetupData.WakeOnLan));
    DEBUG((EFI_D_INFO, "[800]:%X\n", IoRead32(0x800)));
    IoWrite16(0x800, BIT14);
    if(gSetupData.WakeOnLan){
      IoAnd16(0x802, (UINT16)~BIT14);
    } else {
      IoOr16(0x802, BIT14);
    }
    DEBUG((EFI_D_INFO, "[800]:%X\n", IoRead32(0x800)));
    ProjectEnableS5RtcWake();
  }
}


VOID AcpiOnHook(VOID)
{
  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
}

VOID S3ResumeHook(VOID)
{
  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
}


EFI_STATUS 
ProjectUpdateSetupData (
  IN     VOID   *SetupData,
  IN     UINTN  SetupDataSize
)
{
  if(SetupData == NULL || SetupDataSize != sizeof(SETUP_DATA)){
    return EFI_INVALID_PARAMETER;
  }

  CopyMem(&gSetupData, SetupData, SetupDataSize);
  return EFI_SUCCESS;
}


EFI_STATUS EFIAPI ResetSystemPassword(VOID)
{
/*  
  EFI_STATUS                 Status;
  EFI_SMM_VARIABLE_PROTOCOL  *SmmVariable;
  SYSTEM_PASSWORD2           SysPasswd2;


  Status = gSmst->SmmLocateProtocol(&gEfiSmmVariableProtocolGuid, NULL, (VOID**)&SmmVariable);
  if(EFI_ERROR(Status)){
    return Status;
  }

  ZeroMem(&SysPasswd2, sizeof(SYSTEM_PASSWORD2)); 
  Status = SmmVariable->SmmSetVariable (
                          SYSTEM_PASSWORD2_NAME,
                          &gByoSystemPassword2VariableGuid,
                          SYSTEM_PASSWORD2_NV_ATTRIBUTE,
                          sizeof(SYSTEM_PASSWORD2),
                          (VOID*)&SysPasswd2
                          );  
  ASSERT_EFI_ERROR(Status);
  return Status;
*/

  return EFI_UNSUPPORTED;  
}


EFI_STATUS
EFIAPI
ProjectSmmReadyToBootNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
  gSmmReadyToBoot = TRUE;
  return EFI_SUCCESS;
}


EFI_STATUS
GetFeatureCode (
    OUT UINT8    *FeatrueCode,
        VOID     *FvStart
  )
{
  return EFI_UNSUPPORTED;
}



BOOLEAN   gEnvPrepared  = FALSE;
UINT16    gAcpiPmEnData = 0;
UINT32    gFchSmiCtl3   = 0;

EFI_STATUS
EFIAPI
ProjectDisablePowerButton (
  IN BOOLEAN       Disable
  )
{
  if(Disable){

    gFchSmiCtl3 = MmioRead32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC);
    gAcpiPmEnData = IoRead16(mAcpiBaseAddr+2);
    
    MmioWrite32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC, gFchSmiCtl3 & ((UINT32)~(BIT6 | BIT7)));
    IoWrite16(mAcpiBaseAddr+2, gAcpiPmEnData & (UINT16)~BIT8);
    
    gEnvPrepared = TRUE; 
    return EFI_SUCCESS;
    
  } else {
  
    if(gEnvPrepared){
      IoWrite16(mAcpiBaseAddr, BIT8);           // clear power button status.
      IoWrite16(mAcpiBaseAddr+2, gAcpiPmEnData); // restore
      MmioWrite32(ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC, gFchSmiCtl3);
      gEnvPrepared = FALSE;
      return EFI_SUCCESS;
      
    } else {
      return EFI_NOT_READY;
    }
  }
}



EFI_STATUS
ProjectSmmEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
{
  EFI_STATUS                                Status;
  SETUP_DATA	                              *SetupHob;
  EFI_HANDLE                                SmmHandle;
  VOID                                      *Registration;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));
  
  mAcpiBaseAddr = PcdGet16(AcpiIoPortBaseAddress);

  SetupHob = (SETUP_DATA*)GetSetupDataHobData();
  CopyMem(&gSetupData, SetupHob, sizeof(SETUP_DATA));	

  SmmHandle = NULL;
  gByoCommSmiSvc.GetCurrentTime   = GetCurrentTime;
  gByoCommSmiSvc.ResetSysPassword = ResetSystemPassword;
  gByoCommSmiSvc.SetCurrentTime   = SetCurrentTime;
  gByoCommSmiSvc.GenUuid          = GenerateUUID;
  gByoCommSmiSvc.AcpiOnHook       = AcpiOnHook;
  gByoCommSmiSvc.S3ResumeHook     = S3ResumeHook;
  gByoCommSmiSvc.GenUuidBySn      = GenerateUUIDBySn;
  gByoCommSmiSvc.SleepCallback    = ProjectSleepCommonHandler;
  gByoCommSmiSvc.UpdateSetupData  = ProjectUpdateSetupData;
  gByoCommSmiSvc.GetBiosFeatureCode = GetFeatureCode;  
  gByoCommSmiSvc.UsbFilter          = ByoLegacyUsbFilter;
  gByoCommSmiSvc.UsbFilter2         = ByoLegacyUsbFilter2;
  gByoCommSmiSvc.DisablePowerButton = ProjectDisablePowerButton;
  
  UsbFilterInit();  
  
  Status = gSmst->SmmInstallProtocolInterface (
                    &SmmHandle,
                    &gByoCommSmiSvcProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gByoCommSmiSvc
                    );  
  ASSERT_EFI_ERROR (Status);

  Status = gSmst->SmmRegisterProtocolNotify (
                    &gEdkiiSmmReadyToBootProtocolGuid,
                    ProjectSmmReadyToBootNotify,
                    &Registration
                    );
  ASSERT_EFI_ERROR(Status);
    
  return EFI_SUCCESS;
}



