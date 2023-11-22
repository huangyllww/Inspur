/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   FCH SMM Driver
 *
 */
/*****************************************************************************
 *
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 *
 ******************************************************************************
 */
#include "FchSmm.h"
#define FILECODE  UEFI_SMM_FCHSMM_SXSMI_FILECODE

extern  UINT8           *mFchPciIrqRoutingTable;
extern  FCH_MEM_BACKUP  *mFchMemoryBackup;
UINT64                  gS3MemorySaveTableAddress[] = { 0xFEDC0010, 0xFEDC0020, 0xFEDC0030, 0 };
UINTN                   gS3MemorySaveTableSize = sizeof (gS3MemorySaveTableAddress) / sizeof (UINT64);
extern FCH_DATA_BLOCK   gFchDataInSmm;

EFI_STATUS
FchSmmRegisterSxSmi (
  VOID
  )
{
  EFI_STATUS                     Status;
  FCH_SMM_SX_DISPATCH2_PROTOCOL  *HygonSxDispatch;
  FCH_SMM_SX_REGISTER_CONTEXT    SxRegisterContext;
  EFI_HANDLE                     SxHandle;

  //
  // Register HYGON SX SMM
  //
  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmSxDispatch2ProtocolGuid,
                    NULL,
                    &HygonSxDispatch
                    );
  ASSERT (!EFI_ERROR (Status));

  SxRegisterContext.Type  = SxS3;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 1;

  Status = HygonSxDispatch->Register (
                              HygonSxDispatch,
                              HygonSmiS3SleepEntryCallback,
                              &SxRegisterContext,
                              &SxHandle
                              );

  SxRegisterContext.Type  = SxS4;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 1;

  Status = HygonSxDispatch->Register (
                              HygonSxDispatch,
                              HygonSmiS4SleepEntryCallback,
                              &SxRegisterContext,
                              &SxHandle
                              );

  SxRegisterContext.Type  = SxS5;
  SxRegisterContext.Phase = SxEntry;
  SxRegisterContext.Order = 1;

  Status = HygonSxDispatch->Register (
                              HygonSxDispatch,
                              HygonSmiS5SleepEntryCallback,
                              &SxRegisterContext,
                              &SxHandle
                              );

  return Status;
}

/**
 * FixPsp4Ehang
 *
 *
 * @retval  VOID
 *
 */
VOID
FixPsp4Ehang (
  OUT VOID
  )
{
  UINT8                Value8;
  UINT32               IoApicNumber;
  HYGON_CONFIG_PARAMS  *StdHeader;
  FCH_DATA_BLOCK       *pFchPolicy;

  pFchPolicy = &gFchDataInSmm;
  StdHeader  = pFchPolicy->StdHeader;

  ACPIMMIO32 (FCH_AOACx94S0I3_CONTROL) |= FCH_AOACx94S0I3_CONTROL_ARBITER_DIS + FCH_AOACx94S0I3_CONTROL_INTERRUPT_DIS;
  ACPIMMIO32 (0xFEC00000) = 0x3E;
  ACPIMMIO32 (0xFEC00010) = 0xFF;
  LibHygonIoRead (AccessWidth8, FCH_IOMAP_REGED, &Value8, StdHeader);
  ACPIMMIO32 (0xFEC00020) = 0x17;
  LibHygonIoRead (AccessWidth8, FCH_IOMAP_REGED, &Value8, StdHeader);
  IoApicNumber = ACPIMMIO32 (0xFEC00020);

  for (IoApicNumber = 0; IoApicNumber < 24; IoApicNumber++) {
    ACPIMMIO32 (0xFEC00000) = 0x10 + (IoApicNumber << 1);
    ACPIMMIO32 (0xFEC00010) = BIT16;
  }
}

/*++

Routine Description:


Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
HygonSmiS3SleepEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                              *CommBuffer OPTIONAL,
  IN OUT   UINTN                             *CommBufferSize  OPTIONAL
  )
{
  UINT8           Index;
  UINT32          DieBusNum;
  UINT8           UsbIndex;
  UINTN           RootBridge;
  UINT8           *pData;
  FCH_DATA_BLOCK  *pFchPolicy;
  FCH_MEM_BACKUP  *pMemBuffer;

  pFchPolicy = &gFchDataInSmm;
  // Save entire FCH PCI IRQ routing space (C00/C01)
  pData = mFchPciIrqRoutingTable;
  Index = 0xFF;
  do {
    Index++;
    LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC00, &Index);
    LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC01, pData++);
  } while (Index != 0xFF);

  pMemBuffer = mFchMemoryBackup;

  if(pMemBuffer != NULL) {
    for (Index = 0; gS3MemorySaveTableAddress[Index] != 0 && Index < gS3MemorySaveTableSize; Index++) {
      pMemBuffer[Index].Address = gS3MemorySaveTableAddress[Index];
      pMemBuffer[Index].Data    = MmioRead32 (pMemBuffer[Index].Address);
    }
  }

  // Put Usb3 to S0 power rail
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEE, AccessWidth8, (UINT32) ~(BIT0 + BIT1), (BIT1 + BIT0));
  BackUpCG2 ();
  FixPsp4Ehang ();
  
  RootBridge = FabricTopologyGetNumberOfRootBridgesOnSocket(0);
  for(UsbIndex = 0; UsbIndex < RootBridge; UsbIndex++){
    DieBusNum = ReadSocDieBusNum(0, 0, UsbIndex);
    FchTSXhciInitS3EntryProgram (0, 0, UsbIndex, DieBusNum, pFchPolicy);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HygonSmiS4SleepEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                              *CommBuffer OPTIONAL,
  IN OUT   UINTN                             *CommBufferSize  OPTIONAL
  )
{
  FCH_DATA_BLOCK  *pFchPolicy;
  UINT8           DieBusNum;
  UINT8           UsbIndex;
  UINTN           RootBridge;
  
  pFchPolicy = &gFchDataInSmm;

  // Put Usb3 to S0 power rail
  RwMem (ACPI_MMIO_BASE + PMIO_BASE + FCH_PMIOA_REGEE, AccessWidth8, (UINT32) ~(BIT0 + BIT1), (BIT1 + BIT0));
  BackUpCG2 ();
  FixPsp4Ehang ();

  RootBridge = FabricTopologyGetNumberOfRootBridgesOnSocket(0);
  
  for (UsbIndex = 0; UsbIndex < RootBridge; UsbIndex++) {
    DieBusNum = (UINT8)ReadSocDieBusNum(0, 0, UsbIndex);
    TurnOffUsbPme (0, UsbIndex, DieBusNum, pFchPolicy);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HygonSmiS5SleepEntryCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SX_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                              *CommBuffer OPTIONAL,
  IN OUT   UINTN                             *CommBufferSize  OPTIONAL
  )
{
  BackUpCG2 ();
  // RwMem (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG04 + 2, AccessWidth8, ~BIT3, BIT3);
  FixPsp4Ehang ();
  return EFI_SUCCESS;
}
