/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD PSP SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspSmm Driver
 * @e \$Revision$   @e \$Date$
 *
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
 ******************************************************************************
 */
#include "AmdPspSmmV2.h"
#include "AmdPspResumeServicesV2.h"
#include <Library/BaseLib.h>

#define FILECODE PSP_AMDPSPSMMV2_AMDPSPSMMV2_FILECODE

///@todo move below assert definition to Ids header file
#define ASSERT_EFI_ERROR(x) ASSERT (!EFI_ERROR (x))

SMM_SAVE_AREA_CONTEXT                         *mSmmSaveAreaCopy;
PSP_PLATFORM_PROTOCOL                         mPspPlatformProtocolCopy;
PSP_SMM_RSM_MEM_INFO                          *mPspSmmRsmMemInfoPtr;
BOOLEAN                                       mPspSwSmiEntered = FALSE;
UINT8                                         *mPspMboxSmmBuffer = NULL;
BOOLEAN                                       *mPspMboxSmmFlagAddr = NULL;

extern volatile BOOLEAN  mCapsuleUpdateFlag;
//
// Soft SMI handle to repond fot the SW SMI call coming from PSP
//
EFI_STATUS
EFIAPI
PspSxCallback (
  IN       EFI_HANDLE                    DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT   *DispatchContext,
  IN OUT   VOID                     *CommBuffer OPTIONAL,
  IN OUT   UINTN                    *CommBufferSize  OPTIONAL
  )
{
  IDS_HDT_CONSOLE_PSP_TRACE ("PSP.PspSmm.SleepCallBack\n");
  if (PspMboxBiosCmdSxInfo (DispatchContext->Type, mPspMboxSmmBuffer, mPspMboxSmmFlagAddr)) {
    return (EFI_UNSUPPORTED);
  }

  return (EFI_SUCCESS);
}

EFI_STATUS
EFIAPI
PspSmmSwSmiCallBack (
  IN       EFI_HANDLE                       DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT         *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT          *SwContext,
  IN OUT   UINTN                       *SizeOfSwContext
  )
{
  PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL *PspMboxSmmBufferAddressProtocol;
  EFI_STATUS Status;

  AGESA_TESTPOINT (TpPspSmmV2SwSmiCallbackEntry, NULL);
  IDS_HDT_CONSOLE_PSP_TRACE ("PSP.PspSmm.SwSmiCallBack\n");

  if (mPspSwSmiEntered == FALSE) {
    //Save PspMboxSmmBuffer to globol variable
    Status = gSmst->SmmLocateProtocol (&gPspMboxSmmBufferAddressProtocolGuid, NULL, &PspMboxSmmBufferAddressProtocol);
    if (Status == EFI_SUCCESS) {
      mPspMboxSmmBuffer = PspMboxSmmBufferAddressProtocol->PspMboxSmmBuffer;
      mPspMboxSmmFlagAddr = PspMboxSmmBufferAddressProtocol->PspMboxSmmFlagAddr;
      IDS_HDT_CONSOLE_PSP_TRACE ("\tmPspMboxSmmBuffer 0x%x\n", mPspMboxSmmBuffer);
      IDS_HDT_CONSOLE_PSP_TRACE ("\tmPspMboxSmmFlagAddr 0x%x\n", mPspMboxSmmFlagAddr);
    }

    if (EFI_ERROR (CopySmmSaveAreaToLocal (&mSmmSaveAreaCopy))) {
      return EFI_NOT_FOUND;
    }

    //
    // Setup MSR to instruct microcode to build the S3 save area for resume
    //
    UpdateS3SaveAreaMapV2 ();
    AGESA_TESTPOINT (TpPspSmmV2SwSmiCallbackExit, NULL);
    mPspSwSmiEntered = TRUE;
  }
  return EFI_SUCCESS;
}



EFI_STATUS
AmdCapsuleSmmHook (
  IN       UINT32  AmdCapsuleHookFlag
  ) {
  mCapsuleUpdateFlag = 1;
  if (AmdCapsuleHookFlag == PSP_CAPSULE_HOOK_FLAG_SXINFO_NOCLRSMMLCK) {
    //Send PSP about S3 entering
    PspMboxBiosCmdSxInfo (0x3, mPspMboxSmmBuffer, mPspMboxSmmFlagAddr);
  } else if (AmdCapsuleHookFlag == PSP_CAPSULE_HOOK_FLAG_SXINFO_CLRSMMLCK) {
    //Notify PSP about clearing SMM lock
    PspMboxBiosClearSmmLock(mPspMboxSmmBuffer, mPspMboxSmmFlagAddr);
    //Send PSP about S3 entering
    PspMboxBiosCmdSxInfo (0x3, mPspMboxSmmBuffer, mPspMboxSmmFlagAddr);
  }
  return EFI_SUCCESS;
}

AMD_CAPSULE_SMM_HOOK_PROTOCOL mAmdCapsuleSmmHookProtocol = {
    AmdCapsuleSmmHook
};

EFI_STATUS
EFIAPI
AmdPspSmmV2Entry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  FCH_SMM_SX_DISPATCH2_PROTOCOL   *SxDispatch;
  EFI_HANDLE                      SxHandle, SwHandle;
  FCH_SMM_SX_REGISTER_CONTEXT     SxRegisterContext;
  FCH_SMM_SW_DISPATCH2_PROTOCOL    *AmdSwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT     SwRegisterContext;
  PSP_PLATFORM_PROTOCOL           *PspPlatfromProtocol;
  EFI_LOADED_IMAGE_PROTOCOL       *LoadedImage;
  EFI_PHYSICAL_ADDRESS            TempStackPtr;
  EFI_PHYSICAL_ADDRESS            AlignStackPtr;
  UINT32                          StackSize;
  EFI_HANDLE                      Handle;

  AGESA_TESTPOINT (TpPspSmmV2Entry, NULL);
  //If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresentV2 () == FALSE) ||
      (PcdGetBool (PcdAmdPspEnable) == FALSE)) {
    return EFI_SUCCESS;
  }

  //
  // We're now in SMM!
  //

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.AmdPspSmmV2 Enter\n");
  mPspSwSmiEntered = FALSE;

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSxDispatch2ProtocolGuid,
                  NULL,
                  &SxDispatch
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SxRegisterContext.Type  = SxS3;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 1;

  // Register with Smm driver to intercept Sx transition and inform Psp via mailbox in
  // that transition
  IDS_HDT_CONSOLE_PSP_TRACE ("\tRegister Sleep SMI\n");
  Status = SxDispatch->Register (
                          SxDispatch,
                          PspSxCallback,
                          &SxRegisterContext,
                          &SxHandle
                          );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  // Declare the AGESA capsule entry hook
  Handle = NULL;
  IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall AGESA capsule entry hook protocol\n");
  Status = gSmst->SmmInstallProtocolInterface (
                    &Handle,
                    &gAmdCapsuleSmmHookProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    &mAmdCapsuleSmmHookProtocol
                    );
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("\tInstall Fail\n");
  }
  // Below code is only needed if the referecen BIOS intend to use New S3 resume path (Using Dram instead of CAR)
  // Even if PSP initialize the DRAM, the OEM bios still has a choice to use the old resume path
  // i.e. do not write to MSR to perform resume. So add customization token to skip below code
  // for those designs
  //Locate Psp Platform Protocol to get customized information
  if (CheckPspPlatformSecureEnableV2 () == 0) {
    //Secure S3 only required for platform_secure_enable chip which memory will be restore by PSP FW
    return EFI_SUCCESS;
  }
  IDS_HDT_CONSOLE_PSP_TRACE ("\tLocate Psp Platform Protocol\n");
  Status = gBS->LocateProtocol (
                    &gPspPlatformProtocolGuid,
                    NULL,
                    &PspPlatfromProtocol
                    );

  if (EFI_ERROR (Status)) {
    return Status;
  }
  //Dump Platform protocol info
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPSP_PLATFORM_PROTOCOL::\n");
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\tCpuContextResumeEnable:0x%x\n", PspPlatfromProtocol->CpuContextResumeEnable);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\tSwSmiCmdtoBuildContext:0x%x\n", PspPlatfromProtocol->SwSmiCmdtoBuildContext);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\tBspStackSize:0x%x\n", PspPlatfromProtocol->BspStackSize);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\tApStackSize:0x%x\n", PspPlatfromProtocol->ApStackSize);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\tRsmHandOffInfo:&0x%x\n", PspPlatfromProtocol->RsmHandOffInfo);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\t\tGdtOffset:0x%x\n", PspPlatfromProtocol->RsmHandOffInfo->GdtOffset );
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\t\tCodeSelector:0x%x\n", PspPlatfromProtocol->RsmHandOffInfo->CodeSelector);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\t\tDataSelector:0x%x\n", PspPlatfromProtocol->RsmHandOffInfo->DataSelector);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\t\tRsmEntryPoint:0x%x\n", PspPlatfromProtocol->RsmHandOffInfo->RsmEntryPoint);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\t\tEdxResumeSignature:0x%x\n", PspPlatfromProtocol->RsmHandOffInfo->EdxResumeSignature);

  //Copy the data to local copy to make it available at SMM space
  SetMem (&mPspPlatformProtocolCopy,
          sizeof (PSP_PLATFORM_PROTOCOL),
          0
         );

  CopyMem (&mPspPlatformProtocolCopy,
           PspPlatfromProtocol,
           sizeof (PSP_PLATFORM_PROTOCOL)
          );

  if ((mPspPlatformProtocolCopy.CpuContextResumeEnable != TRUE) || (AmdCapsuleGetStatus ())) {
    // We are not supporting CPU context resume. So we can bail from here
    return EFI_SUCCESS;
  }


  //Allocate Memory for hold PSP_SMM_RSM_MEM_INFO
  IDS_HDT_CONSOLE_PSP_TRACE ("\tAllocate Memory for New S3 Resume\n");

  Status = gBS->AllocatePool (
                EfiReservedMemoryType,
                sizeof (PSP_SMM_RSM_MEM_INFO),
                &mPspSmmRsmMemInfoPtr);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem (mPspSmmRsmMemInfoPtr, sizeof (PSP_SMM_RSM_MEM_INFO));

  //Allocate Memory for stack during SMM Resume
  //Allocate Double Stack Size for alignment
  //For ZP, only require to assign Stack for BSP, AP will halt after exit AP SMM Vector which no stack needed
  ASSERT (mPspPlatformProtocolCopy.BspStackSize != 0);
  ASSERT (mPspPlatformProtocolCopy.ApStackSize != 0);
  StackSize = mPspPlatformProtocolCopy.BspStackSize + mPspPlatformProtocolCopy.ApStackSize * ((UINT32) gSmst->NumberOfCpus - 1);
  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiReservedMemoryType,
                  EFI_SIZE_TO_PAGES (2 * StackSize),
                  &TempStackPtr);
  IDS_HDT_CONSOLE_PSP_TRACE ("Allocate TempStack 0x%x Size 0x%x\n", TempStackPtr, 2 * StackSize);

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ZeroMem ((VOID *) TempStackPtr, 2 * StackSize);


  //Align on Stack Size boundary which is a PEI Core requirement
  AlignStackPtr = ((TempStackPtr + mPspPlatformProtocolCopy.BspStackSize) & (~(mPspPlatformProtocolCopy.BspStackSize - 1)));

  //Free Unused Pages
  if (AlignStackPtr - TempStackPtr) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Free 0x%x Size 0x%x\n", TempStackPtr, (AlignStackPtr - TempStackPtr));
    gBS->FreePages (TempStackPtr, EFI_SIZE_TO_PAGES (AlignStackPtr - TempStackPtr));
  }
  if (StackSize - (AlignStackPtr - TempStackPtr)) {
    IDS_HDT_CONSOLE_PSP_TRACE ("Free 0x%x Size 0x%x\n", AlignStackPtr + StackSize, (StackSize - (AlignStackPtr - TempStackPtr)));
    gBS->FreePages (AlignStackPtr + StackSize, EFI_SIZE_TO_PAGES (StackSize - (AlignStackPtr - TempStackPtr)));
  }

  mPspSmmRsmMemInfoPtr->StackPtr = AlignStackPtr;
  IDS_HDT_CONSOLE_PSP_TRACE ("AlignStackPtr 0x%x\n", AlignStackPtr);

  mPspSmmRsmMemInfoPtr->BspStackSize = mPspPlatformProtocolCopy.BspStackSize;
  mPspSmmRsmMemInfoPtr->ApStackSize = mPspPlatformProtocolCopy.ApStackSize;
  //Allocate Memory for temporary storage during SMM Resume
  Status = gBS->AllocatePages (
                  AllocateAnyPages,
                  EfiReservedMemoryType,
                  EFI_SIZE_TO_PAGES (SMM_RESUME_TEMP_REGION_SIZE),
                  &mPspSmmRsmMemInfoPtr->TempRegionPtr);
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mPspSmmRsmMemInfoPtr->TempRegionSize = SMM_RESUME_TEMP_REGION_SIZE;
  //Dump result of memory allocation
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPspSmmRsmMemInfo::\n");
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\tStackPtr:0x%x BspStackSize:0x%x ApStackSize:0x%x\n",
              mPspSmmRsmMemInfoPtr->StackPtr,
              mPspSmmRsmMemInfoPtr->BspStackSize,
              mPspSmmRsmMemInfoPtr->ApStackSize);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\tRsmHeapPtr:0x%x HeapSize:0x%x\n",
              mPspSmmRsmMemInfoPtr->RsmHeapPtr,
              mPspSmmRsmMemInfoPtr->HeapSize);
  IDS_HDT_CONSOLE_PSP_TRACE ("\t\tTempRegionPtr:0x%x TempRegionSize:0x%x\n",
              mPspSmmRsmMemInfoPtr->TempRegionPtr,
              mPspSmmRsmMemInfoPtr->TempRegionSize);

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &AmdSwDispatch
                  );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  // Hook for software SMI to prepare CPU context for Smm resume during S3-resume
  IDS_HDT_CONSOLE_PSP_TRACE ("\tRegister SW SMI for prepare CPU resume context\n");
  SwRegisterContext.AmdSwValue  = mPspPlatformProtocolCopy.SwSmiCmdtoBuildContext;
  SwRegisterContext.Order       = 0x80;
  Status = AmdSwDispatch->Register (
                            AmdSwDispatch,
                            PspSmmSwSmiCallBack,
                            &SwRegisterContext,
                            &SwHandle
                            );

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Publish protocol for others to register for resume time callback
  //
  IDS_HDT_CONSOLE_PSP_TRACE ("\tPublish resume register protocol\n");
  InstallPspResumeCallbackProtocolV2 ();

  //Send Resume info to PSP to prevent resume module be tampered
  Status = gBS->HandleProtocol (
                    ImageHandle,
                    &gEfiLoadedImageProtocolGuid,
                    &LoadedImage
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("\tASRF.Base:0x%x ASRF.Size:0x%x\n", LoadedImage->ImageBase, LoadedImage->ImageSize);
  Status = PspMboxBiosCmdResumeInfo (
                  (UINT64) LoadedImage->ImageBase,
                  LoadedImage->ImageSize);

  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("AmdPspSmmV2 Exit\n");
  AGESA_TESTPOINT (TpPspSmmV2Exit, NULL);
  return Status;
}

