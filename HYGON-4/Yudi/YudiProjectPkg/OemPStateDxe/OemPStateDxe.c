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
#include <ProjectToken.h>



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
  
  SetupHob = (SETUP_DATA*)GetSetupDataHobData();
  DEBUG((DEBUG_INFO, "UpdatePstateTable(), PerfMode :%d.\n", SetupHob->PerfMode));

  Px = 0xFF;
  if (SetupHob->PerfMode == PERF_MODE_HIGH_PERF) {
    return EFI_UNSUPPORTED;
  }

  if(PcdGet8(PcdHygonCpuPstateMode) == CPU_P0_STATE){
    Px = 0;
  }
  if(PcdGet8(PcdHygonCpuPstateMode) == CPU_P0P1_STATE){
    Px = 1;
  }
  if(PcdGet8(PcdHygonCpuPstateMode) == CPU_P0P1P2_STATE){
    Px = 2;
  }

  if (SetupHob->PerfMode == PERF_MODE_OEM) {
    if (SetupHob->CpuPstateEn != CPU_P0_STATE) {
      return EFI_UNSUPPORTED;
    }
    Px = SetupHob->CpuSpeed;
    if (Px == 0) {
      return EFI_UNSUPPORTED;
    }
    //
    // If CPB is AUTO(means CPU can be OC) and PerfMode is PERF_MODE_OEM(User Defined),
    // we need skip PState configuration
    //
    if (PcdGet8 (PcdHygonCpbMode) == 1) {
      return EFI_UNSUPPORTED;
    }
  }

  if( Px > 2){
    return EFI_UNSUPPORTED;
  }

  DEBUG((DEBUG_INFO, "UpdatePstateTable(), CpuSpeed Px :%d.\n", Px));  
  PxMsr.Value = AsmReadMsr64(MSR_PSTATE_0 + Px);

  ZeroMem(CustomPstate, sizeof(IDS_CUSTOM_CORE_PSTATE)*8);
  CustomPstate[0].Custom = CCX_IDS_CORE_PSTATES_CUSTOM;
  CustomPstate[0].CoreFid = (UINT8)PxMsr.Field.CpuFid_7_0;  
  CustomPstate[0].CoreDid = (UINT8)PxMsr.Field.CpuDid;
  if (CustomPstate[0].CoreDid > 0x1A) {
    CustomPstate[0].CoreDid &= 0xFE;
  }
  CustomPstate[0].CoreVid = (UINT8)PxMsr.Field.CpuVid;


  CustomPstate[1].Custom = CCX_IDS_CORE_PSTATES_CUSTOM;
  CustomPstate[1].CoreFid = (UINT8)PxMsr.Field.CpuFid_7_0;  
  CustomPstate[1].CoreDid = (UINT8)PxMsr.Field.CpuDid;
  if (CustomPstate[1].CoreDid > 0x1A) {
    CustomPstate[1].CoreDid &= 0xFE;
  }
  CustomPstate[1].CoreVid = (UINT8)PxMsr.Field.CpuVid;

  CustomPstate[2].Custom = CCX_IDS_CORE_PSTATES_CUSTOM;
  CustomPstate[2].CoreFid = (UINT8)PxMsr.Field.CpuFid_7_0;  
  CustomPstate[2].CoreDid = (UINT8)PxMsr.Field.CpuDid;
  if (CustomPstate[2].CoreDid > 0x1A) {
    CustomPstate[2].CoreDid &= 0xFE;
  }
  CustomPstate[2].CoreVid = (UINT8)PxMsr.Field.CpuVid;

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
OemPStateDxeEntry (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINT8       Index;
  PSTATE_MSR  PxMsr;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), j_d, \n", __LINE__));
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


