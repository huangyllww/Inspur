/* $NoKeywords:$ */
/**
 * @file
 *
 * PspP2Cmbox Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AMD PSP UEFI Driver
 * @e sub-project   PspP2Cmbox Driver
 * @e \$Revision: 309147 $   @e \$Date: 2014-12-10 17:55:49 +0800 (Wed, 10 Dec 2014) $
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
#include "AmdPspP2CmboxV1.h"
#include <Library/S3BootScriptLib.h>
#include <Filecode.h>

#define FILECODE PSP_AMDPSPP2CMBOXV1_AMDPSPP2CMBOXV1_FILECODE

#undef  GET_MASK8
#define GET_MASK8(HighBit, LowBit) ((((UINT8) 1 << (HighBit - LowBit + 1)) - 1) << LowBit)

#undef  GET_MASK16
#define GET_MASK16(HighBit, LowBit) ((((UINT16) 1 << (HighBit - LowBit + 1)) - 1) << LowBit)

#undef  GET_MASK32
#define GET_MASK32(HighBit, LowBit) ((((UINT32) 1 << (HighBit - LowBit + 1)) - 1) << LowBit)

#undef  GET_MASK64
#define GET_MASK64(HighBit, LowBit) ((((UINT64) 1 << (HighBit - LowBit + 1)) - 1) << LowBit)

#define MSR_SMMADDR                 0xC0010112ul
#define MSR_SMMADDR_TSEGBASE_BITS   GET_MASK64 (39, 17)

#define MSR_SMMMASK                 0xC0010113ul
#define MSR_SMMMASK_TSEGMASK_BITS   GET_MASK64 (39, 17)


/// @todo move this out of this file to common place
/// @todo make it customizable via build option etc. Also check the Size of this buffer is
/// sufficient for PSP communication

#define PSP_DATA_BLOCK_SIZE       (8*1024)
#define PSP_P2CMBOX_SW_SMI_CALLBACK    0xAD
//#define PSP2BIOS_USING_SW_SMI

#define PSP_SMM_BUFFER_ALIGNMENT    0x20u // Must be 32-byte aligned
#define PSP_SMM_BUFFER_PAGES        1u
#define PSP_MBOX_SMM_BUFFER_OFFSET  0u
#define PSP_MBOX_SMM_FLAG_OFFSET    0x00000C00 //Start from 3K

///@todo remove below FCH definition when it has been declared by FCH module
#define ACPIMMIO16(x) (*(volatile UINT16*)(UINTN)(x))
#define ACPIMMIO32(x) (*(volatile UINT32*)(UINTN)(x))
#define ACPI_MMIO_BASE  0xFED80000ul
#define SMI_BASE        0x200   // DWORD
#define PMIO_BASE       0x300   // DWORD
#define FCH_PMIOA_REG6A          0x6A         // AcpiSmiCmd
#define FCH_SMI_REG84            0x84         // SmiStatus1
#define FCH_SMI_REG98            0x98         // SmiTrig
#define FCH_SMI_REGA8            0xA8

///@todo move below assert definition to Ids header file
#define ASSERT_EFI_ERROR(x) ASSERT (!EFI_ERROR (x))


UINT64                                mTsegBase = 0;
UINT64                                mTsegSize = 0;
BIOS_MBOX                             *PspToBiosMbox = NULL;
UINT64                                mPspNvRamRegionBase = 0;
UINT32                                mPspNvRamRegionSize = 0;
UINTN                                 mBlockSize = 0;

SMM_TRIGGER_INFO  mSmmTrigInfo =  {
                                      0x00,                     // Address location
                                      0x00,                     // AddressType
                                      0x00,                     // Address width 0: Byte
                                      0x00,                     // And Mask
                                      0x00  // OrMask
                                  };

PSP_MBOX_SMMBUFFER_ADDRESS_PROTOCOL mPspMboxSmmBufferAddressProtocol;

BOOLEAN
IsValidP2CCommand (
  IN  MBOX_COMMAND     MboxCmd
  )
{
  if ((MboxCmd == MboxPspCmdSpiGetAttrib) ||
      (MboxCmd == MboxPspCmdSpiSetAttrib) ||
      (MboxCmd == MboxPspCmdSpiGetBlockSize) ||
      (MboxCmd == MboxPspCmdSpiReadFV) ||
      (MboxCmd == MboxPspCmdSpiWriteFV) ||
      (MboxCmd == MboxPspCmdSpiEraseFV)
      ) {
    return TRUE;
  }
  return FALSE;
}

EFI_STATUS
CheckMboxValidity (
  IN  BIOS_MBOX *P2CMbox
  )
{
  if ( (P2CMbox == NULL) ||
       (mTsegBase == 0)  ||
       ( (UINT64) P2CMbox < mTsegBase) ||
       (UINT64) P2CMbox > (mTsegBase + mTsegSize) ||
       (P2CMbox->Buffer.Dflt.Header.TotalSize > PSP_DATA_BLOCK_SIZE ) ||
       (IsValidP2CCommand (P2CMbox->MboxCmd) == FALSE)
      ) {
    return (EFI_INVALID_PARAMETER);
  }

  return (EFI_SUCCESS);
}

EFI_STATUS
P2CmboxSmmCallBackWorker (
  VOID
  )
{
  UINT32                MboxCmd;
  UINT32                *pMboxSts;
  MBOX_DEFAULT_BUFFER   *MboxBuffer;
  EFI_STATUS            Status;

  Status = EFI_SUCCESS;

  MboxCmd     = PspToBiosMbox->MboxCmd;
  pMboxSts     = (UINT32 *)&(PspToBiosMbox->MboxSts);
  MboxBuffer  = (MBOX_DEFAULT_BUFFER *)&(PspToBiosMbox->Buffer.Dflt);
  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.P2CMbox Cmd:0x%x\n", MboxCmd);

  // basic check. Ignore all the invalid case
  Status = CheckMboxValidity (PspToBiosMbox);
  IDS_HDT_CONSOLE_PSP_TRACE ("\tCheckMboxValidity[%x]\n", Status);

  if (!EFI_ERROR (Status)) {
    Status = ProcessPspCommand (
               MboxCmd,
               MboxBuffer
               );
    *pMboxSts = MBOX_STATUS_INITIALIZED;
    if (EFI_ERROR (Status)) {
      *pMboxSts |= MBOX_STATUS_ERROR;
    }

    PspToBiosMbox->MboxCmd  = 0;     //Clear the command register
    // Trigger interrupt to PSP. Inform command processed
    PspMboxBiosCmdNop ();
    AGESA_TESTPOINT (TpPspP2CmboxCmdExit, NULL);
  } else {
    AGESA_TESTPOINT (TpPspP2CmboxCmdFailExit, NULL);
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("P2CMbox Exit[0x%x] \n", *pMboxSts );
  return (Status);
}
//
// Soft SMI handle to repond to sw smi call coming from PSP
//
EFI_STATUS
EFIAPI
P2CmboxSwSmiCallback (
  IN       EFI_HANDLE DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT *SwContext,
  IN OUT   UINTN *SizeOfSwContext
  )
{
  return P2CmboxSmmCallBackWorker ();
}

EFI_STATUS
EFIAPI
P2CmboxMiscSmiCallback (
  IN  EFI_HANDLE                       DispatchHandle,
  IN  FCH_SMM_MISC_REGISTER_CONTEXT   *MiscRegisterContext
  )
{
  EFI_STATUS Status;

  Status = P2CmboxSmmCallBackWorker ();
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) &= ~BIT25;  // Deassert fakeSts0

  return Status;
}

EFI_STATUS
InitFchFakeStsInfo (
  IN OUT   SMM_TRIGGER_INFO *SmmTriggerInfoPtr
  )
{
  SmmTriggerInfoPtr->Address = ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98;
  SmmTriggerInfoPtr->AddressType = SMM_TRIGGER_MEM;
  SmmTriggerInfoPtr->ValueWidth = SMM_TRIGGER_DWORD;
  SmmTriggerInfoPtr->ValueAndMask = (UINT32) ~BIT25;
  SmmTriggerInfoPtr->ValueOrMask = BIT25;
  return EFI_SUCCESS;
}

EFI_STATUS
EnablePspFakeStsSmi (
  VOID
  )
{
  FCH_SMM_MISC_DISPATCH_PROTOCOL    *AmdFchSmmMiscDispatch;
  EFI_HANDLE                        MiscHandle;
  FCH_SMM_MISC_REGISTER_CONTEXT     MiscRegisterContext;
  EFI_STATUS                        Status;
  UINT32                            OrMask;
  UINT32                            AndMask;

  // enable PSP SMM via Fake Sts0
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84) |= BIT1;  //Clear FakeSts0
  OrMask = BIT1;
  AndMask = 0xFFFFFFFFul;
  S3BootScriptSaveMemReadWrite (S3BootScriptWidthUint32, (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84), &OrMask, &AndMask);

  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) &= ~BIT25;  // Deassert fakeSts0
  OrMask = 0;
  AndMask = (UINT32) ~BIT25;
  S3BootScriptSaveMemReadWrite (S3BootScriptWidthUint32, (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98), &OrMask, &AndMask);

  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA8) &= ~(BIT3 + BIT2);  // Enable SMI [3:2] = 1
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA8) |= BIT2;
  OrMask = BIT2;
  AndMask = (UINT32) ~(BIT3 + BIT2);
  S3BootScriptSaveMemReadWrite (S3BootScriptWidthUint32, (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA8), &OrMask, &AndMask);


  MiscRegisterContext.SmiStatusReg = FCH_SMI_REG84;
  MiscRegisterContext.SmiStatusBit = BIT1;
  MiscRegisterContext.Order        = 0x80;

  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmMiscDispatchProtocolGuid,
                    NULL,
                    &AmdFchSmmMiscDispatch
                    );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = AmdFchSmmMiscDispatch->Register (
                                      AmdFchSmmMiscDispatch,
                                      &P2CmboxMiscSmiCallback,
                                      &MiscRegisterContext,
                                      &MiscHandle
                                      );

  return Status;
}
EFI_STATUS
EnablePspSwSmi (
  VOID
  )
{
  EFI_STATUS                      Status;
  FCH_SMM_SW_REGISTER_CONTEXT     SwContext;
  EFI_HANDLE                      SwHandle;
  FCH_SMM_SW_DISPATCH2_PROTOCOL    *SwDispatch;


  //
  //  Get the Sw dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmSwDispatch2ProtocolGuid,
                  NULL,
                  &SwDispatch
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SwContext.AmdSwValue  = PSP_P2CMBOX_SW_SMI_CALLBACK;
  SwContext.Order       = 0x80;
  Status = SwDispatch->Register (
                          SwDispatch,
                          &P2CmboxSwSmiCallback,
                          &SwContext,
                          &SwHandle
                          );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  mSmmTrigInfo.Address = (UINT64) ACPIMMIO16 (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REG6A);
  mSmmTrigInfo.AddressType = SMM_TRIGGER_IO;
  mSmmTrigInfo.ValueWidth = SMM_TRIGGER_BYTE;
  mSmmTrigInfo.ValueAndMask = 0;
  mSmmTrigInfo.ValueOrMask = PSP_P2CMBOX_SW_SMI_CALLBACK;
  IDS_HDT_CONSOLE_PSP_TRACE ("\tSwSmiTriggerReg:0x%x\n", mSmmTrigInfo.Address);

  return EFI_SUCCESS;
}

EFI_STATUS
PspP2CmboxEntry (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                      Status;
  UINT8                           *LocalPspMboxSmmBuffer;
  UINT8                           *PspMboxSmmBuffer;
  BOOLEAN                         *PspMboxSmmFlagAddr;
  EFI_HANDLE                      Handle;

  LocalPspMboxSmmBuffer = NULL;
  PspMboxSmmBuffer = NULL;
  PspMboxSmmFlagAddr = NULL;

  AGESA_TESTPOINT (TpPspP2CmboxV1Entry, NULL);
  //We are now in SMM
  //If PSP feature turn off, exit the driver
  if ((CheckPspDevicePresentV1 () == FALSE) ||
      (PcdGetBool (PcdAmdPspEnable) == FALSE)) {
    return EFI_SUCCESS;
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("Psp.Drv.P2Cmbox Enter\n");
  //
  // We're now in SMM!

  //Init PSP DIR Entry
  Status = PSPEntryInfoV1 (
                    PSP_NV_DATA,
                    &mPspNvRamRegionBase,
                    &mPspNvRamRegionSize
                    );
  //Init Blocksize
  PspGetFlashBlockSize (&mBlockSize);

  IDS_HDT_CONSOLE_PSP_TRACE ("PSP NV Region[%x] Size:[%x] BlockSize[%x]\n", mPspNvRamRegionBase, mPspNvRamRegionSize, mBlockSize);

  // Allocate SMM buffer for PSP-> BIOS communication
  IDS_HDT_CONSOLE_PSP_TRACE ("\tAllocate SMM buffer for P2C Mbox\n");
  Status = gSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    PSP_DATA_BLOCK_SIZE + 32,
                    &PspToBiosMbox
                    );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  // Allocate SMM buffer to hold C2P message after bootdone
  LocalPspMboxSmmBuffer = AllocateAlignedPages(PSP_SMM_BUFFER_PAGES, PSP_SMM_BUFFER_ALIGNMENT);
  ASSERT (LocalPspMboxSmmBuffer != NULL);
  if (LocalPspMboxSmmBuffer != NULL) {
    //Assign the address for C2P RT buffer, C2P SMM flag
    PspMboxSmmBuffer = LocalPspMboxSmmBuffer + PSP_MBOX_SMM_BUFFER_OFFSET;
    PspMboxSmmFlagAddr = LocalPspMboxSmmBuffer + PSP_MBOX_SMM_FLAG_OFFSET;
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPspMboxSmmBuffer 0x%x\n", PspMboxSmmBuffer);
    IDS_HDT_CONSOLE_PSP_TRACE ("\tPspMboxSmmFlagAddr 0x%x\n", PspMboxSmmFlagAddr);
    //Saving the address through SMM protocol
    Handle = NULL;
    mPspMboxSmmBufferAddressProtocol.PspMboxSmmBuffer = PspMboxSmmBuffer;
    mPspMboxSmmBufferAddressProtocol.PspMboxSmmFlagAddr = PspMboxSmmFlagAddr;
    gSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gPspMboxSmmBufferAddressProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mPspMboxSmmBufferAddressProtocol
                      );
  }
  //
  // Initialize the data structure
  ZeroMem (PspToBiosMbox, PSP_DATA_BLOCK_SIZE + 32);

  //Align on 32 bytes boundary
  PspToBiosMbox = ALIGN_POINTER ((UINTN) PspToBiosMbox, 32);
  PspToBiosMbox->MboxCmd      = 0;
  *(UINT32 *)&(PspToBiosMbox->MboxSts) = MBOX_STATUS_INITIALIZED;

  #ifdef PSP2BIOS_USING_SW_SMI
  IDS_HDT_CONSOLE_PSP_TRACE ("\tP2C using SW SMI\n");
  Status = EnablePspSwSmi ();
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  #else
  IDS_HDT_CONSOLE_PSP_TRACE ("\tP2C using fake SMI\n");
  InitFchFakeStsInfo (&mSmmTrigInfo);
  Status = EnablePspFakeStsSmi ();
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }
  #endif
  //Get TsegBase, TsegSize
  mTsegBase = AsmReadMsr64 (MSR_SMMADDR);
  mTsegBase &= MSR_SMMADDR_TSEGBASE_BITS;

  mTsegSize = AsmReadMsr64 (MSR_SMMMASK);
  mTsegSize = (~(mTsegSize & MSR_SMMMASK_TSEGMASK_BITS) + 1) & MSR_SMMMASK_TSEGMASK_BITS;


  IDS_HDT_CONSOLE_PSP_TRACE ("\tSMMBase:0x%x SMMLength:0x%x\n\tPSPSmmDataRegion:0x%x PspSmmDataLength:0x%x\n", mTsegBase, mTsegSize, PspToBiosMbox, PSP_DATA_BLOCK_SIZE);
  //Dump mSmmTrigInfo
  IDS_HDT_CONSOLE_PSP_TRACE ("\tSmmTrigInfo::Addr:0x%x AddrType:0x%x Width:0x%x AndMask:0x%x OrMask:0x%x\n",
              mSmmTrigInfo.Address,
              mSmmTrigInfo.AddressType,
              mSmmTrigInfo.ValueWidth,
              mSmmTrigInfo.ValueAndMask,
              mSmmTrigInfo.ValueOrMask);

  // After command MBOX_C2P_BOOT_DONE (6), any subsequent commands have to have a command buffer
  // placed in the area started with the BiosSmmCmdBufAddr address provided by the command MBOX_SMI_INFO_BUFFER.
  // And the contents inside the word addressed by the BiosSmmFlagAddr should be non-0, indicating the CPU is in SMM mode

  if (PspMboxBiosCmdSmmInfo (
        mTsegBase,
        mTsegSize,
        (UINT64) PspToBiosMbox,
        PSP_DATA_BLOCK_SIZE,
        &mSmmTrigInfo,
        (UINT64) (UINTN) PspMboxSmmBuffer,
        (UINT64) (UINTN) PspMboxSmmFlagAddr
        )) {
    return (EFI_SUCCESS);
  }

  IDS_HDT_CONSOLE_PSP_TRACE ("P2Cmbox Exit\n");
  AGESA_TESTPOINT (TpPspP2CmboxV1Exit, NULL);
  return EFI_SUCCESS;
}

