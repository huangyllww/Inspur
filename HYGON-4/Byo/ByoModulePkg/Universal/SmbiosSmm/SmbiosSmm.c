/** @file

Copyright (c) 2006 - 2012, Byosoft Corporation.<BR>
All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

File Name:
  SmbiosSmm.c

Abstract:
  Source file for the Smm Smbios driver.

Revision History:

**/

#include <SmbiosSmm.h>
#include <Protocol/PnpSmbios.h>
#include <ByoSmiFlashInfo.h>
#include <ByoSmbiosTable.h>

extern SMBIOS_TABLE_ENTRY_POINT  *mSmbiosEfiEntry;

NV_MEDIA_ACCESS_PROTOCOL    *pMediaAccessProtocol;
EFI_SMM_CPU_PROTOCOL        *mSmmCpu;
UINT32                      RegEbx;
BOOLEAN                     mEntryValid = FALSE;
SMI_INFO                    *gSmiFlashInfo = NULL;
extern UINT32               mSmbiosRegionSize;

EFI_STATUS
PnpGetSystemConfigurationTable (
  IN EFI_GUID *TableGuid,
  IN OUT VOID **Table
);

EFI_STATUS
EFIAPI
DmiPnP50_57Callback (
  IN     EFI_HANDLE               DispatchHandle,
  IN     CONST VOID               *Context,        OPTIONAL
  IN OUT VOID                     *CommBuffer,     OPTIONAL
  IN OUT UINTN                    *CommBufferSize  OPTIONAL
)
/*++

Routine Description:
  It is DMI_PNP_50_57 SMM function defined in CSM spec.
  This notification function is called when an SMM Mode is invoked through SMI.
  This may happen during RT, so it must be RT safe.

Arguments:
  DispatchHandle  - EFI Handle
  DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

Returns:
  None

--*/
{
  EFI_STATUS                  Status;
  UINTN                       Index;
  UINTN                       CpuIndex;
  EFI_SMM_SAVE_STATE_IO_INFO  IoState;
  UINT16                      *ParameterBuffer;
  UINT16                      FunctionId;
  UINT16                      ReturnCode;

  //
  // Check parameter condition
  //
  CpuIndex = 0;
  for (Index = 0; Index < gSmst->NumberOfCpus; Index++) {
    Status = mSmmCpu->ReadSaveState (
                        mSmmCpu,
                        sizeof (EFI_SMM_SAVE_STATE_IO_INFO),
                        EFI_SMM_SAVE_STATE_REGISTER_IO,
                        Index,
                        &IoState
                        );
    if (!EFI_ERROR (Status) && (IoState.IoData == SMM_PnP_BIOS_CALL)) {
      CpuIndex = Index;
      break;
    }
  }
  if(Index >= gSmst->NumberOfCpus) {
    DEBUG((EFI_D_INFO, "Index larger than CPU numbers\n"));
    CpuDeadLoop ();
  }

  //
  // Check parameter condition
  //
  if (gSmiFlashInfo->Tag != SIGNATURE_32 ('$', 'B', 'S', 'B')) {
    ReturnCode = PNP_BIOS_DMI_RETURN_FUNCTION_NOT_SUPPORTED;
    goto ProcExit;
  }

  //
  // Get the first PnP Function call parameter - Function ID
  //
  ParameterBuffer = (UINT16 *)(UINTN)gSmiFlashInfo->Buffer;
  FunctionId = *ParameterBuffer;

  if(!mEntryValid){
    ReturnCode = PNP_BIOS_DMI_RETURN_FUNCTION_NOT_SUPPORTED;
    goto ProcExit;
  }

  // DEBUG((EFI_D_INFO, "FunctionId:%X\n", FunctionId));

  //
  // Check FunctionId and call the function
  // PnP BIOS DMI Function Call
  // They are defined in CSM spec, need to be handled.
  //
  switch (FunctionId) {
  case PNPBIOS_FUNCID_GET_SMBIOS_INFORMATION:
      ReturnCode = PnpGetSmbiosInformation(ParameterBuffer);
      break;

  case PNPBIOS_FUNCID_GET_SMBIOS_STRUCTURE:
      ReturnCode = PnpGetSmbiosStructure(ParameterBuffer);
      break;

  case PNPBIOS_FUNCID_SET_SMBIOS_STRUCTURE:
      GetFromFlash ();
      ReturnCode = PnpSetSmbiosStructure(ParameterBuffer);
      break;

  case PNPBIOS_FUNCID_TEST_SMIPORT:
    ReturnCode = PnpTestSmiPort(ParameterBuffer);
    break;

  case PNPBIOS_FUNCID_GET_STRUCTURE_CHANGE_INFORMATION:
  case PNPBIOS_FUNCID_SMBIOS_CONTROL:
  case PNPBIOS_FUNCID_GET_GENERAL_PURPOSE_NV_INFORMATION:
  case PNPBIOS_FUNCID_READ_GENERAL_PURPOSE_NV_DATA:
  case PNPBIOS_FUNCID_WRITE_GENERAL_PURPOSE_NV_DATA:
      ReturnCode = PNP_BIOS_DMI_RETURN_FUNCTION_NOT_SUPPORTED;
      break;

  default:
      ReturnCode = PNP_BIOS_DMI_RETURN_FUNCTION_NOT_SUPPORTED;
      break;
  }

  // DEBUG((EFI_D_INFO, "RC:%X\n", ReturnCode));

ProcExit:
  gSmiFlashInfo->StatusCode = ReturnCode;

  return EFI_SUCCESS;
}



EFI_STATUS
EFIAPI
Pnp50GetSmbiosInformation(
  IN      EFI_PNP_SMBIOS_PROTOCOL   *This,
  IN OUT  VOID                      *DataPtr
  )
{
  return EFI_UNSUPPORTED;
}

EFI_STATUS
EFIAPI
Pnp51GetSmbiosStructure(
  IN      EFI_PNP_SMBIOS_PROTOCOL   *This,
  IN OUT  VOID                      *DataPtr
  )
{
  UINT16  Rc;

  RegEbx = SIGNATURE_32('$', 'B', 'S', 'B');
  Rc = PnpGetSmbiosStructure(DataPtr);
  if(Rc == PNP_BIOS_DMI_RETURN_SUCCESS){
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  } 
}

EFI_STATUS
EFIAPI
Pnp51GetSmbiosStructureByType (
  IN      EFI_PNP_SMBIOS_PROTOCOL   *This,
  IN OUT  VOID                      *DataPtr
  )
{
  UINT16  Rc;

  RegEbx = SIGNATURE_32('$', 'B', 'S', 'B');
  Rc = PnpGetSmbiosStructureByType(DataPtr);
  if(Rc == PNP_BIOS_DMI_RETURN_SUCCESS){
    return EFI_SUCCESS;
  } else {
    return EFI_INVALID_PARAMETER;
  } 
}

EFI_STATUS
EFIAPI
Pnp52SetSmbiosStructure(
  IN      EFI_PNP_SMBIOS_PROTOCOL   *This,
  IN OUT  VOID                      *DataPtr,
  IN      UINT8                     Control
  )
{
  UINT16  Rc;

  if(!mEntryValid){
    return EFI_NOT_READY;
  }

  GetFromFlash();
  Rc = PnpSetSmbiosStructure32((PNP_52_DATA_BUFFER*)DataPtr, Control, TRUE);
  if(Rc == PNP_BIOS_DMI_RETURN_SUCCESS){
    return EFI_SUCCESS;
  } else if(Rc == PNP_BIOS_DMI_RETURN_CURRENTLY_LOCKED){
    return EFI_ACCESS_DENIED;
  } else {
    return EFI_INVALID_PARAMETER;
  }   
}


EFI_PNP_SMBIOS_PROTOCOL gByoPnpSmbiosProtocol = {
  Pnp50GetSmbiosInformation,
  Pnp51GetSmbiosStructure,
  Pnp52SetSmbiosStructure,
  Pnp51GetSmbiosStructureByType
};



EFI_STATUS
EFIAPI
SmmReadyToLockNotify (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS  Status;

  DEBUG((EFI_D_INFO, "SmbiosSmm.ReadyToLock\n"));

  Status = PnpGetSystemConfigurationTable (
             &gEfiSmbiosTableGuid,
             (VOID**)&mSmbiosEfiEntry
             );
  if(!EFI_ERROR(Status)){
    DEBUG((EFI_D_INFO, "mSmbiosEfiEntry:%X\n", mSmbiosEfiEntry));
    mEntryValid = TRUE;
  }

  return EFI_SUCCESS;

}



EFI_STATUS
EFIAPI
InitSmbiosSmm (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
)
/*++

Routine Description:

  Initializes the SMM Platform Driver

Arguments:

  ImageHandle   - Pointer to the loaded image protocol for this driver
  SystemTable   - Pointer to the EFI System Table

Returns:

  Status        - EFI_SUCCESS

--*/
{
  EFI_STATUS                                Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL             *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT               SwContext;
  EFI_HANDLE                                SwHandle;
  EFI_HANDLE                                PnpSmbiosHandle;
  VOID                                      *Registration;
  

  ASSERT(PcdGet32(PcdFlashNvStorageSmbiosBase) != 0);
  ASSERT(PcdGet32(PcdFlashNvStorageSmbiosSize) >= SIZE_4KB);
  mSmbiosRegionSize = PcdGet32(PcdFlashNvStorageSmbiosSize);

  //
  // Locate the SMM CPU protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmCpuProtocolGuid, NULL, (VOID **) &mSmmCpu);
  ASSERT_EFI_ERROR (Status);
  //
  //  Locate the SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **)&SwDispatch);
  ASSERT_EFI_ERROR (Status);

  //
  // Register the DMI PnP 50_57 handler
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmNvMediaAccessProtocolGuid, NULL, (VOID **)&pMediaAccessProtocol);
  ASSERT_EFI_ERROR (Status);
  Status = AllocDataBuffer();
  ASSERT_EFI_ERROR (Status);

  SwContext.SwSmiInputValue = SMM_PnP_BIOS_CALL;
  Status = SwDispatch->Register (
                         SwDispatch,
                         DmiPnP50_57Callback,
                         &SwContext,
                         &SwHandle
                         );

  PnpSmbiosHandle = NULL;
  Status = gSmst->SmmInstallProtocolInterface (
                    &PnpSmbiosHandle,
                    &gEfiPnpSmbiosProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &gByoPnpSmbiosProtocol
                    );

  Status = gSmst->SmmRegisterProtocolNotify (&gEfiSmmReadyToLockProtocolGuid, SmmReadyToLockNotify, &Registration);
  ASSERT_EFI_ERROR (Status);

  gSmiFlashInfo = (SMI_INFO *)(UINT64)LibGetSmiInfoBufferAddr(gBS);
  DEBUG((EFI_D_INFO, "smbios gSmiFlashInfo:%p\n",gSmiFlashInfo));


  return EFI_SUCCESS;
}



