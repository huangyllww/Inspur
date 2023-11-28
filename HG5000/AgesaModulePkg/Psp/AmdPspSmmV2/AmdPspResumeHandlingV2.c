/* $NoKeywords:$ */
/**
 * @file
 *
 * PSP related functions in SMM
 *
 * Contains PSP SMM Resume handler
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  PSP
 * @e \$Revision$   @e \$Date$
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 *
 ***************************************************************************/

#include "AmdPspSmmV2.h"
#include "AmdPspResumeServicesV2.h"
#include <Library/FchBaseLib.h>
#include <Library/PciLib.h>
#include <Library/CcxPspLib.h>
#include <Library/OemAgesaCcxPlatformLib.h>

#define FILECODE PSP_AMDPSPSMMV2_AMDPSPRESUMEHANDLINGV2_FILECODE

///@todo remove below FCH definition when it has been declared by FCH module
#define ACPIMMIO16(x) (*(volatile UINT16*)(UINTN)(x))
#define ACPI_MMIO_BASE  0xFED80000ul
#define PMIO_BASE       0x300   // DWORD
#define FCH_PMIOA_REG62          0x62         // AcpiPm1CntBlk
#define CAPSULE_UPDATE_SIGNATURE 0xCA9501EFul

volatile BOOLEAN              WaitForBspToFinishRestore = FALSE;
volatile UINTN                CoreInSmm = 0;

STATIC UINT8 ApExitS3Count = 0;
volatile BOOLEAN  mCapsuleUpdateFlag = 0;

VOID
GetSmmSaveArea (
  IN OUT   volatile UINT64  *CoreSaveAreaOffset
  )
{
  *CoreSaveAreaOffset  = AsmReadMsr64 (0xC0010111) + 0xFE00;
}

EFI_STATUS
CopySmmSaveAreaToLocal (
  IN OUT   VOID **SmmSaveStart
  )
{
  UINTN   i;
  volatile UINT64         CoreSaveAreaOffset;
  SMM_SAVE_AREA_CONTEXT   *Dest;
  EFI_STATUS              Status;

  IDS_HDT_CONSOLE_PSP_TRACE ("CopySmmSaveAreaToLocal\n");
  // Allocate SMM buffer to save SMM save area for each core
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    (gSmst->NumberOfCpus)* (SMM_SAVE_SIZE + sizeof (UINT64)),
                    SmmSaveStart
                    );

  IDS_HDT_CONSOLE_PSP_TRACE ("SmmSaveArea at %08x\n", *SmmSaveStart);

  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Smm Pool Allocate fail\n");
    return (Status);
  }
  Dest = (SMM_SAVE_AREA_CONTEXT *)*SmmSaveStart;

  for (i = 0; i < gSmst->NumberOfCpus; i++) {
    CoreSaveAreaOffset = 0xFFFFFFFFFFFFFFFFull;
    if (i == 0) {
      GetSmmSaveArea (&CoreSaveAreaOffset);
    } else {
      gSmst->SmmStartupThisAp (
             GetSmmSaveArea,
             i,
             (void *)&CoreSaveAreaOffset
             );
    }
    //wait fill by AP
    while (CoreSaveAreaOffset == 0xFFFFFFFFFFFFFFFFull) {
        CpuPause();
    };
    
    IDS_HDT_CONSOLE_PSP_TRACE ("Core%d SMM SaveState0x%08x\n", i, CoreSaveAreaOffset);

    Dest->SaveAreaOffset = (VOID *)CoreSaveAreaOffset;
    IDS_HDT_CONSOLE_PSP_TRACE ("Copy SMM SaveState from 0x%08x to 0x%08x\n", Dest->SaveAreaOffset, Dest->SaveAreaBuffer);

    CopyMem (Dest->SaveAreaBuffer,
      Dest->SaveAreaOffset,
      SMM_SAVE_SIZE
      );
    Dest++;
  }
  return Status;
}


VOID
EFIAPI
OverrideSmmSaveArea (
  IN OUT   VOID                *SmmSaveArea,
  IN       REGISTER_OVERRIDE   *RegisterOverride,
  IN       UINTN                CpuIndex
  )
{
  SMM_SAVE_MAP  *SmmSavePtr = (SMM_SAVE_MAP *)SmmSaveArea;


  SmmSavePtr->CS_Selector   = RegisterOverride->CodeSegOverrideValue;
  SmmSavePtr->CS_Attributes = RegisterOverride->CodeSegAttrib;
  SmmSavePtr->DS_Selector   = RegisterOverride->DateSegOverrideValue;
  SmmSavePtr->DS_Attributes = RegisterOverride->DataSegAttrib;
  SmmSavePtr->DS_Attributes = RegisterOverride->DataSegAttrib;
  SmmSavePtr->ES_Selector   = RegisterOverride->DateSegOverrideValue;
  SmmSavePtr->FS_Selector   = RegisterOverride->DateSegOverrideValue;
  SmmSavePtr->FS_Attributes = RegisterOverride->DataSegAttrib;
  SmmSavePtr->DS_Attributes = RegisterOverride->DataSegAttrib;
  SmmSavePtr->GS_Selector   = RegisterOverride->DateSegOverrideValue;
  SmmSavePtr->GS_Attributes = RegisterOverride->DataSegAttrib;
  SmmSavePtr->DS_Attributes = RegisterOverride->DataSegAttrib;
  SmmSavePtr->SS_Selector   = RegisterOverride->DateSegOverrideValue;
  SmmSavePtr->DS_Attributes = RegisterOverride->DataSegAttrib;


  SmmSavePtr->RAX = RegisterOverride->RaxOverrideValue;
  SmmSavePtr->RBX = RegisterOverride->RbxOverrideValue;
  SmmSavePtr->RDX = RegisterOverride->RdxOverrideValue;

  SmmSavePtr->GDTR_Base   = RegisterOverride->GdtOverrideValue;
  SmmSavePtr->GDTR_Limit  = 0x3f;
  SmmSavePtr->IDTR_Base   = 0;
  SmmSavePtr->IDTR_Limit  = 0;

  SmmSavePtr->CR0 = RegisterOverride->Cr0;
  SmmSavePtr->CR3 = RegisterOverride->Cr3;
  SmmSavePtr->CR4 = RegisterOverride->Cr4;
  SmmSavePtr->RFLAGS = 0;

  if (CpuIndex == 0) {
    SmmSavePtr->RIP = RegisterOverride->RipBspOverrideValue;
    SmmSavePtr->RSP = RegisterOverride->StackBase + RegisterOverride->BspStackSize;
  } else {
    SmmSavePtr->RIP = RegisterOverride->RipApOverrideValue;
    SmmSavePtr->RSP = ( RegisterOverride->StackBase +
                        RegisterOverride->BspStackSize +
                        (RegisterOverride->ApStackSize * CpuIndex)
                       );
  }

  //Clear Auto Halt
  SmmSavePtr->Auto_Halt_Restart = 0;
}


EFI_STATUS
EFIAPI
RestoreSaveAreaFromLocal (
  IN OUT   SMM_SAVE_AREA_CONTEXT *SmmSaveStart,
  IN OUT   REGISTER_OVERRIDE     *RegisterOverride
  )
{
  UINTN   i;

  for (i = 0; i < gSmst->NumberOfCpus; i++) {

    IDS_HDT_CONSOLE_PSP_TRACE ("Restore SmmSave from %08x to %08x\n", SmmSaveStart->SaveAreaBuffer, SmmSaveStart->SaveAreaOffset);
    CopyMem (
      SmmSaveStart->SaveAreaOffset,
      SmmSaveStart->SaveAreaBuffer,
      SMM_SAVE_SIZE
      );

    OverrideSmmSaveArea (
      SmmSaveStart->SaveAreaOffset,
      RegisterOverride,
      i
      );
    SmmSaveStart++;
  }
  return EFI_SUCCESS;
}


VOID
PspConnectedStandbyResumeHandling (
  )
{
  // Call Register callback for connected standby
  PspResumeServiceCallBackV2 (ResumeFromConnectedStandby);
}

EFI_STATUS
EFIAPI
CustomizeSmmRsmVector (
  IN OUT   REGISTER_OVERRIDE       *RegisterOverride
  )
{
  RSM_HANDOFF_INFO *ResumeHandoffInfo;

  ResumeHandoffInfo = mPspPlatformProtocolCopy.RsmHandOffInfo;
  RegisterOverride->RipBspOverrideValue   = ResumeHandoffInfo->RsmEntryPoint;
  RegisterOverride->RipApOverrideValue    = ResumeHandoffInfo->RsmEntryPoint;
  //Set Special value to identify CapsuleUpdate
  if (mCapsuleUpdateFlag) {
    RegisterOverride->RbxOverrideValue     = CAPSULE_UPDATE_SIGNATURE;
  } else {
    RegisterOverride->RbxOverrideValue     = 0;
  }

  RegisterOverride->RdxOverrideValue      = ResumeHandoffInfo->EdxResumeSignature;
  RegisterOverride->GdtOverrideValue      = ResumeHandoffInfo->GdtOffset;
  RegisterOverride->CodeSegOverrideValue  = ResumeHandoffInfo->CodeSelector;
  RegisterOverride->DateSegOverrideValue  = ResumeHandoffInfo->DataSelector;

  RegisterOverride->GdtOverrideLimit      = 0x3f;
  RegisterOverride->CodeSegAttrib         = 0xc9b;
  RegisterOverride->DataSegAttrib         = 0xc93;
  RegisterOverride->Cr0                   = 0x00000013;
  RegisterOverride->Cr3                   = 0x00;
  RegisterOverride->Cr4                   = 0x00040608;

  RegisterOverride->RaxOverrideValue      = (UINTN) mPspSmmRsmMemInfoPtr;      //Rax Point to mPspSmmRsmMemInfo structure
  RegisterOverride->StackBase             = mPspSmmRsmMemInfoPtr->StackPtr;
  RegisterOverride->BspStackSize          = mPspSmmRsmMemInfoPtr->BspStackSize; //Reserve Stack for BSP
  RegisterOverride->ApStackSize           = mPspSmmRsmMemInfoPtr->ApStackSize; //Reserve Stack for AP


  return (EFI_SUCCESS);
}

VOID
EFIAPI
PspS3ResumeHandling (
  VOID
  )
{
  REGISTER_OVERRIDE     RegisterOverride;

  ZeroMem (&RegisterOverride, sizeof (RegisterOverride));

  //Set OEM customized info
  CustomizeSmmRsmVector (&RegisterOverride);

  // Patch Smm save area
  RestoreSaveAreaFromLocal (
    mSmmSaveAreaCopy,
    &RegisterOverride
  );

  // Now call all the Register callback handling
  PspResumeServiceCallBackV2 (ResumeFromS3);

}
BOOLEAN
IsS0i3Resume (
  )
{
  UINT32    S0i3Ctrl;
  UINTN     PciAddress;

  PciAddress = MAKE_SBDFO (0, 0, 0x18, 5, 0x204);
  S0i3Ctrl = PciRead32 (PciAddress);
  return (((S0i3Ctrl & BIT3) == 0) ? FALSE : TRUE);
}


UINT8
GetResumeType (
  )
{
  if (IsS0i3Resume ()) {
    return ResumeFromConnectedStandby;
  } else if (FchReadSleepType () == 0x03) {
    return ResumeFromS3;
  } else {
    if (!mCapsuleUpdateFlag) {
      IDS_HDT_CONSOLE_PSP_TRACE ("Unsupported Resume type\n");
      ASSERT (FALSE);
      CpuDeadLoop ();  //Should never come here
      return InvalidResumeValue;
    } else {
      return ResumeFromS3;
    }
  }
}



// This is the entry point of AP when we resume from sleep /CS and
// after PSP restore the memory. PSP release BSP and BSP will go through PEI Agesa
// driver to release AP. Here we setup stack etc and AP will rsm to SEC code
// AP from here
//
VOID
ApSmmResumeVector (
  )
{
  UINT8  ResumeType;
  UINT32 ApSyncFlagAddress = 0;
  UINT32 *ApSyncFlag;

  AGESA_TESTPOINT (TpPspSmmV2ApRsmEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSP.ApSmmResumeVector\n");
  ResumeType = GetResumeType (); //Check resume type is S3 or connected standby

  if (ResumeType == ResumeFromS3) {
    // Now call all the Register callback handling
    PspResumeServiceCallBackV2 (ResumeFromS3);

    AGESA_TESTPOINT (TpPspSmmV2ApRsmExit, NULL);
    
    GetApSyncFlagAddress(&ApSyncFlagAddress);
    ApSyncFlag = (UINT32*)((UINTN)ApSyncFlagAddress);
	ASSERT (ApSyncFlag != NULL);
    *ApSyncFlag = *ApSyncFlag +1;
    
    S3ExitToBiosV2 ();
  } else if (ResumeType == ResumeFromConnectedStandby) {
    // Call Register callback for connected standby
    PspConnectedStandbyResumeHandling ();
  }

  CpuDeadLoop ();
}

//
// This is the entry point of BSP when we resume from sleep and
// after PSP restore the memory will release the CPU code to run
// BSP from here
//
VOID
BspSmmResumeVector (
  )
{
  UINT8  ResumeType;

  AGESA_TESTPOINT (TpPspSmmV2BspRsmEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSP.BspSmmResumeVector\n");

  ResumeType = GetResumeType (); //Check resume type is S3 or connected standby
  if (ResumeType == ResumeFromS3) {
    IDS_HDT_CONSOLE_PSP_TRACE ("S3 Resume Handling\n");

    PspS3ResumeHandling ();

    AGESA_TESTPOINT (TpPspSmmV2BspRsmExit, NULL);
    //Clear Sleep Type to force S5 Path, Specail value is set in RDX to identify CapsuleUpdate mode
    if (mCapsuleUpdateFlag) {
      FchWriteSleepType (0);
    }
    S3ExitToBiosV2 ();
  } else if (ResumeType == ResumeFromConnectedStandby) {
    // Call Register callback for connected standby
    IDS_HDT_CONSOLE_PSP_TRACE ("S0i3 resume Handling\n");
    PspConnectedStandbyResumeHandling ();
  }

  // we should never come here
  CpuDeadLoop ();
}

VOID
UpdateS3SaveAreaMapV2 (
  )
{
  UINTN i;
  IDS_HDT_CONSOLE_PSP_TRACE ("UpdateS3SaveAreaMap\n");
  for (i = 1; i < gSmst->NumberOfCpus; i++) {
    gSmst->SmmStartupThisAp (
             SetupReleaseVector,
             i,
             (VOID *)(UINTN)ApSmmResumeVector
             );

  }

  SetupReleaseVector ((VOID *) (UINTN) &BspSmmResumeVector);
}

