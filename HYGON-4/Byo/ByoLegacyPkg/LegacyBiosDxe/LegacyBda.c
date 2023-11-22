/** @file
  This code fills in BDA (0x400) and EBDA (pointed to by 0x4xx)
  information. There is support for doing initialization before
  Legacy16 is loaded and before a legacy boot is attempted.

Copyright (c) 2006 - 2022, Byosoft Corporation.<BR>

All rights reserved.This software and associated documentation (if any)
is furnished under a license and may only be used or copied in
accordance with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be reproduced,
stored in a retrieval system, or transmitted in any form or by any
means without the express written consent of Byosoft Corporation.

**/

#include "LegacyBiosInterface.h"
#include <Guid/PxeControlVariable.h>
#include <Guid/BootRetryPolicyVariable.h>

/**
  Fill in the standard BDA and EBDA stuff before Legacy16 load

  @param  Private     Legacy BIOS Instance data

  @retval EFI_SUCCESS It should always work.

**/
EFI_STATUS
LegacyBiosInitBda (
  IN   LEGACY_BIOS_INSTANCE    *Private,
  OUT  EFI_PHYSICAL_ADDRESS    *LegacyMemTop                
  )
{
  BDA_STRUC             *Bda;
  UINT8                 *Ebda;
  UINT16                *Int13Stack;
  UINTN                 HoleSize;	
  EFI_STATUS            Status;	
  EFI_PHYSICAL_ADDRESS  PhyAddr;
  UINT32                Address;	
  EFI_PHYSICAL_ADDRESS  MemoryAddress;
  UINT32                *ClearPtr;
  UINT16                *SmmPort;
  UINT16                BbsSize;
  UINT16                BbsRtCount;
  CONST UINT16          MinBbsRtCount = 0x30 + 8 + 5;
  BOOLEAN               Legacy2Uefi = PcdGetBool(PcdCsm32Legacy2UefiSupport);
  PXE_CONTROL_VARIABLE        PxeControl;
  UINTN                       Size;
  BOOT_RETRY_POLICY_VARIABLE  BootRetryPolicy;
  BOOLEAN                     NeedBackupBbsTable = FALSE;

// Allocate 0 - 4K for real mode interrupt vectors and BDA.
  Status = AllocateLegacyMemory (
             AllocateAddress,
             0,
             1,
             &MemoryAddress
             );
  ASSERT_EFI_ERROR (Status);

  ClearPtr = (VOID *) ((UINTN) 0x0000);
  gBS->SetMem ((VOID *) ClearPtr, 0x400, INITIAL_VALUE_BELOW_1K);
  ZeroMem ((VOID *) ((UINTN)ClearPtr + 0x400), 0xC00);

  // 640k-4k (0x9F000) to 640k   (0x9FFFF) is reserved for S3 resume (CpuMp)
  // 640k-(n+4)k                 RT bbs table
  // 640k-(n+5)k                 int13 stack
  // 640k-(n+6)k                 EBDA header

  BbsRtCount = PcdGet16(PcdBbsTableEntryRtMaxCount);
  Size = sizeof(BOOT_RETRY_POLICY_VARIABLE);
  Status = gRT->GetVariable (
                  BOOT_RETRY_POLICY_VAR_NAME,
                  &gBootRetryPolicyVariableGuid,
                  NULL,
                  &Size,
                  &BootRetryPolicy
                  );
  if (!EFI_ERROR(Status)) {
    if ((BootRetryPolicy.GroupRetryTime != 0) || ((BootRetryPolicy.GroupRetryTime == 0) && (BootRetryPolicy.AllRetryTime > 1))) {
      DEBUG((EFI_D_INFO, "Boot Group retry enabled, need backup BbsTable %a line=%d\n", __FUNCTION__, __LINE__));
      NeedBackupBbsTable = TRUE;
    }
  }


  if(!PcdGet8(PcdRetryPxeBoot) && !Legacy2Uefi && !NeedBackupBbsTable){
    BbsRtCount = 0;
  }
  DEBUG((EFI_D_INFO, "BbsRtCount:%d, MinBbsRtCount:%d, Legacy2Uefi:%d\n", BbsRtCount, MinBbsRtCount, Legacy2Uefi));	  
  if(BbsRtCount && BbsRtCount < MinBbsRtCount){
    BbsRtCount = MinBbsRtCount;
    PcdSet16S(PcdBbsTableEntryRtMaxCount, BbsRtCount);
  }
  BbsSize  = BbsRtCount * sizeof(BBS_TABLE);
  BbsSize  = ALIGN_VALUE(BbsSize, SIZE_1KB);
  HoleSize = SIZE_2KB + SIZE_1KB + PcdGet16(PcdCpuS3ApVectorMaxSize) + BbsSize;
  Ebda     = (UINT8*)(UINTN)(CONVENTIONAL_MEMORY_TOP - HoleSize);
  PhyAddr  = ((UINTN)Ebda)&(~0xFFF);
  DEBUG((EFI_D_INFO, "EBDA:%X,%X PhyAddr:%lX\n", Ebda, HoleSize, PhyAddr));	
  Status = gBS->AllocatePages (
                  AllocateAddress,
                  EfiReservedMemoryType,
                  EFI_SIZE_TO_PAGES(HoleSize),
                  &PhyAddr
                  );
  ASSERT(!EFI_ERROR(Status));
  if (LegacyMemTop != NULL) {
    *LegacyMemTop = PhyAddr;
  }

  Address = CONVENTIONAL_MEMORY_TOP;
  if(PcdGet16(PcdCpuS3ApVectorMaxSize)){  
    Address -= PcdGet16(PcdCpuS3ApVectorMaxSize);
    ASSERT(PcdGet32(PcdCpuS3ApVectorAddress) == Address);
  }
  if(BbsSize){
    Address -= BbsSize;
    PcdSet32S(PcdBbsTableEntryRtAddress, Address);
    SetMem((VOID*)(UINTN)Address, BbsSize, 0xFF);
    DEBUG((EFI_D_INFO, "BBS RT:%x count:%x, size:%x\n", Address, BbsRtCount, BbsSize));
  }

  Bda = (BDA_STRUC *) ((UINTN) 0x400);
  ZeroMem (Bda, 0x100);
  ZeroMem (Ebda, 0x800);

  Bda->MemSize        = (UINT16)(((UINTN)Ebda)/SIZE_1KB);
  Bda->KeyHead        = 0x1e;
  Bda->KeyTail        = 0x1e;
  Bda->FloppyData     = 0x00;
  Bda->FloppyTimeout  = 0xff;

  Bda->KeyStart       = 0x001E;
  Bda->KeyEnd         = 0x003E;
  Bda->KeyboardStatus = 0x10;
  Bda->Ebda           = (UINT16)((UINTN)Ebda >> 4);

  //
  // Move LPT time out here and zero out LPT4 since some SCSI OPROMS
  // use this as scratch pad (LPT4 is Reserved)
  //
  Bda->Lpt1_2Timeout  = 0x1414;
  Bda->Lpt3_4Timeout  = 0x1400;

  *Ebda               = 0x02;
  Int13Stack          = (UINT16 *) ((UINTN) Ebda + 0x1ca); // Offset 0x1CA EBDA
  *Int13Stack         = Bda->Ebda + 0x80;                  // 0x80 skip 2K EBDA segment

  SmmPort             = (UINT16 *) ((UINTN) Ebda + 0x1D0); // Offset 0x1D0 EBDA
  *SmmPort            = PcdGet16(PcdSwSmiCmdPort);

  //
  // Set Pxe waiting time and detecting media times
  //
  Size = sizeof(PXE_CONTROL_VARIABLE);
  Status = gRT->GetVariable (
                  PXE_CONTROL_VAR_NAME,
                  &gPxeControlVariableGuid,
                  NULL,
                  &Size,
                  &PxeControl
                  );
  if (EFI_ERROR(Status)) {
    PxeControl.WaitSeconds = 0;
    PxeControl.DetectMediaTimes = 1;
  }

  *(Ebda + 0x1D7) = PxeControl.WaitSeconds;
  *(Ebda + 0x1D8) = PxeControl.DetectMediaTimes;

  return EFI_SUCCESS;
}
