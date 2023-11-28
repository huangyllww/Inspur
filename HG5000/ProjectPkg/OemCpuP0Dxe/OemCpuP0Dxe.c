/** @file
==========================================================================================
      NOTICE: Copyright (c) 2006 - 2020 Byosoft Corporation. All rights reserved.
              This program and associated documentation (if any) is furnished
              under a license. Except as permitted by such license,no part of this
              program or documentation may be reproduced, stored divulged or used
              in a public system, or transmitted in any form or by any means
              without the express written consent of Byosoft Corporation.
==========================================================================================

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

#include <Uefi.h>
#include <PiPei.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/PlatformCommLib.h>
#include <SetupVariable.h>
#include <Protocol/ByoCustomPstatesProtocol.h>



typedef union {
  struct {
    UINT64 CpuFid_7_0:8;               ///< CpuFid[7:0]
    UINT64 CpuDid:6;                   ///< CpuDid
    UINT64 CpuVid:8;                   ///< CpuVid
    UINT64 IddValue:8;                 ///< IddValue
    UINT64 IddDiv:2;                   ///< IddDiv
    UINT64 :31;                        ///< Reserved
    UINT64 PstateEn:1;                 ///< Pstate Enable
  } Field;
  UINT64  Value;
} PSTATE_MSR;



EFI_STATUS
EFIAPI
UpdatePstateTable (
  BYO_CUSTOM_PSTATE_PROTOCOL  *This,
  IDS_CUSTOM_CORE_PSTATE      *CustomPstate
  )
{
  SETUP_DATA              *SetupHob;
  UINT8                   Px;
  PSTATE_MSR              PxMsr;
  UINT32                  Freq;

  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  
  SetupHob = (SETUP_DATA*)GetSetupDataHobData();
  if(SetupHob->AMDPstate != 2){
    return EFI_UNSUPPORTED;
  }
  
  Px = SetupHob->CpuSpeed;
  if(Px == 0 || Px > 2){
    return EFI_UNSUPPORTED;
  }
  
  PxMsr.Value = AsmReadMsr64(MSR_PSTATE_0 + Px);

  ZeroMem(CustomPstate, sizeof(IDS_CUSTOM_CORE_PSTATE)*2);
  CustomPstate[0].Custom = CCX_IDS_CORE_PSTATES_CUSTOM;
  CustomPstate[0].CoreFid = (UINT8)PxMsr.Field.CpuFid_7_0;  
  CustomPstate[0].CoreDid = (UINT8)PxMsr.Field.CpuDid;
  CustomPstate[0].CoreVid = (UINT8)PxMsr.Field.CpuVid;
  CustomPstate[1].Custom = CCX_IDS_CORE_PSTATES_DISABLED;

  Freq = (UINT32)(DivU64x32(MultU64x64 (200, CustomPstate[0].CoreFid), CustomPstate[0].CoreDid));
  DEBUG((EFI_D_INFO, "Updated %d %d %d %d\n", CustomPstate[0].CoreFid, CustomPstate[0].CoreDid, \
  CustomPstate[0].CoreVid, Freq));

  return EFI_SUCCESS;
}


UINT16 gCpuSpeedList[3] = {0,};

BYO_CUSTOM_PSTATE_PROTOCOL gByoPStateProtocol = {
  UpdatePstateTable,
  gCpuSpeedList,
  ARRAY_SIZE(gCpuSpeedList)
};


EFI_STATUS
EFIAPI
OemCpuP0DxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINT8       Index;
  PSTATE_MSR  PxMsr;


  DEBUG((EFI_D_INFO, "%a\n", __FUNCTION__));

  for(Index=0;Index<ARRAY_SIZE(gCpuSpeedList);Index++){
    PxMsr.Value = AsmReadMsr64(MSR_PSTATE_0 + Index);
    gCpuSpeedList[Index] = (UINT16)(DivU64x32(MultU64x64 (200, PxMsr.Field.CpuFid_7_0), (UINT32)PxMsr.Field.CpuDid));
    DEBUG((EFI_D_INFO, "[%d] %d\n", Index, gCpuSpeedList[Index]));    
  }

  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gByoCustomPStateProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &gByoPStateProtocol
                  );
  ASSERT(!EFI_ERROR(Status));

  return EFI_SUCCESS;
}


