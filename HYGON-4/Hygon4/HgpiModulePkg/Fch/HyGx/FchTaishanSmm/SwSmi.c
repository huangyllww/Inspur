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
#define FILECODE  UEFI_SMM_FCHSMM_SWSMI_FILECODE

extern FCH_DATA_BLOCK   gFchDataInSmm;
extern  UINT8           *mFchPciIrqRoutingTable;
extern  FCH_MEM_BACKUP  *mFchMemoryBackup;
extern  UINTN           gS3MemorySaveTableSize;

UINT16
FchGetPmBase (
  VOID
  )
{
  UINT8  Lsb;
  UINT8  Msb;

  LibFchSmmPmioRead8 (&gSmst->SmmIo, FCH_PMIOA_REG60, &Lsb);
  LibFchSmmPmioRead8 (&gSmst->SmmIo, FCH_PMIOA_REG60 + 1, &Msb);
  return (UINT16)((Msb << 8) + Lsb);
}

/**
 * FchSmmAcpiOn - Config Fch during ACPI_ON
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchSmmAcpiOn (
  IN  FCH_DATA_BLOCK     *FchDataPtr
  )
{
  //
  // Commented the following code since we need to leave the IRQ1/12 filtering enabled always as per latest
  // recommendation. This is required to fix the keyboard stuck issue when playing games under Windows
  //

  //
  // Disable Power Button SMI
  //
  RwMem (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAC, AccessWidth8, ~(UINT32)(BIT6), 0);
}
/*++

Routine Description:
  SMI handler to restore ACPI mode

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the EFI_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
HygonSmiBeforePciS3RestoreCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  FCH_DATA_BLOCK  *pFchPolicy;
  EFI_STATUS      Status;
  UINT8           Index;
  UINT8           *pData;
  FCH_MEM_BACKUP  *pMemBuffer;

  Status = EFI_SUCCESS;

  // Restore entire FCH PCI IRQ routing space (C00/C01)
  pData = mFchPciIrqRoutingTable;
  Index = 0xFF;
  do {
    Index++;
    LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC00, &Index);
    LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC01, pData++);
  } while (Index != 0xFF);

  pMemBuffer = mFchMemoryBackup;
  if(pMemBuffer != NULL) {
    for(Index = 0; (pMemBuffer[Index].Address != 0) && (Index < gS3MemorySaveTableSize); Index++) {
      MmioWrite32 (pMemBuffer[Index].Address, pMemBuffer[Index].Data);
    }
  }

  pFchPolicy = &gFchDataInSmm;
  FchInitS3EarlyRestore (pFchPolicy);
  return Status;
}

EFI_STATUS
EFIAPI
HygonSmiAfterPciS3RestoreCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  FCH_DATA_BLOCK  *pFchPolicy;
  EFI_STATUS      Status;

  Status     = EFI_SUCCESS;
  pFchPolicy = &gFchDataInSmm;
  FchInitS3LateRestore (pFchPolicy);
  return Status;
}

EFI_STATUS
EFIAPI
HygonSmiAcpiOnCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  FCH_DATA_BLOCK  *pFchPolicy;
  UINT16          AcpiPmbase;
  UINT8           dbIndex;
  UINT8           dbIrq;
  UINT32          ddValue;

  AcpiPmbase = FchGetPmBase ();

  pFchPolicy = &gFchDataInSmm;
  FchSmmAcpiOn (pFchPolicy);

  // Disable all GPE events and clear all GPE status
  ddValue = 0;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT32, AcpiPmbase + R_FCH_ACPI_EVENT_ENABLE, &ddValue);
  LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT32, AcpiPmbase + R_FCH_ACPI_EVENT_STATUS, &ddValue);
  if (ddValue) {
    LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT32, AcpiPmbase + R_FCH_ACPI_EVENT_STATUS, &ddValue);
  }

  // Set ACPI IRQ to IRQ9 for non-APIC OSes
  dbIndex = 0x10;                                          // PIC - SCI
  dbIrq   = 9;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC00, &dbIndex);
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC01, &dbIrq);

  dbIndex |= BIT7;                                         // IOAPIC - SCI
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC00, &dbIndex);
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, FCH_IOMAP_REGC01, &dbIrq);

  // Finally enable SCI
  LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT32, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &ddValue);
  ddValue |= BIT0;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT32, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &ddValue);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HygonSmiAcpiOffCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  UINT16  AcpiPmbase;
  UINT32  ddValue;

  AcpiPmbase = FchGetPmBase ();

  // Turn off SCI
  LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT32, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &ddValue);
  ddValue &= ~BIT0;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT32, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &ddValue);

  return EFI_SUCCESS;
}

#ifdef FCH_SPI_PROTECT_SMI
  EFI_STATUS
  EFIAPI
  HygonSmiSpiUnlockCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
  {
    FCH_DATA_BLOCK  *pFchPolicy;
    EFI_STATUS      Status;

    Status     = EFI_SUCCESS;
    pFchPolicy = &gFchDataInSmm;
    FchSpiUnlock (pFchPolicy);
    return Status;
  }

  EFI_STATUS
  EFIAPI
  HygonSmiSpiLockCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
  {
    FCH_DATA_BLOCK  *pFchPolicy;
    EFI_STATUS      Status;

    Status     = EFI_SUCCESS;
    pFchPolicy = &gFchDataInSmm;
    FchSpiLock (pFchPolicy);
    return Status;
  }

#endif

STATIC UINT8  SavePort80 = 0x00;

EFI_STATUS
EFIAPI
HygonStartTimerSmiCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  SavePort80 = ACPIMMIO8 (ACPI_MMIO_BASE + MISC_BASE + FCH_MISC_REG44);

  ACPIMMIO16 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96) |= SMI_TIMER_ENABLE;
  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
HygonStopTimerSmiCallback (
  IN       EFI_HANDLE                        DispatchHandle,
  IN       CONST FCH_SMM_SW_REGISTER_CONTEXT *DispatchContext,
  IN OUT   FCH_SMM_SW_CONTEXT                *SwContext,
  IN OUT   UINTN                             *SizeOfSwContext
  )
{
  ACPIMMIO16 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG96) &= ~SMI_TIMER_ENABLE;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT8, 0x80, &SavePort80);
  return EFI_SUCCESS;
}

EFI_STATUS
FchSmmRegisterSwSmi (
  VOID
  )
{
  EFI_STATUS                     Status;
  FCH_SMM_SW_DISPATCH2_PROTOCOL  *HygonSwDispatch;
  FCH_SMM_SW_REGISTER_CONTEXT    SwRegisterContext;
  EFI_HANDLE                     SwHandle;
  FCH_MISC                       *FchMisc;

  FchMisc = &gFchDataInSmm.Misc;

  //
  // Locate SMM SW dispatch protocol
  //
  Status = gSmst->SmmLocateProtocol (
                    &gFchSmmSwDispatch2ProtocolGuid,
                    NULL,
                    &HygonSwDispatch
                    );
  ASSERT (!EFI_ERROR (Status));

  SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemBeforePciRestoreSwSmi); // use of PCD in place of FCHOEM_BEFORE_PCI_RESTORE_SWSMI    0xD3
  SwRegisterContext.Order = 0x80;
  Status = HygonSwDispatch->Register (
                              HygonSwDispatch,
                              HygonSmiBeforePciS3RestoreCallback,
                              &SwRegisterContext,
                              &SwHandle
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemAfterPciRestoreSwSmi); // use of PCD in place of FCHOEM_AFTER_PCI_RESTORE_SWSMI    0xD4
  SwRegisterContext.Order = 0x80;
  Status = HygonSwDispatch->Register (
                              HygonSwDispatch,
                              HygonSmiAfterPciS3RestoreCallback,
                              &SwRegisterContext,
                              &SwHandle
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemEnableAcpiSwSmi); // use of PCD in place of FCHOEM_ENABLE_ACPI_SWSMI           0xA0
  SwRegisterContext.Order = 0x80;
  Status = HygonSwDispatch->Register (
                              HygonSwDispatch,
                              HygonSmiAcpiOnCallback,
                              &SwRegisterContext,
                              &SwHandle
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemDisableAcpiSwSmi); // use of PCD in place of FCHOEM_DISABLE_ACPI_SWSMI          0xA1
  SwRegisterContext.Order = 0x80;
  Status = HygonSwDispatch->Register (
                              HygonSwDispatch,
                              HygonSmiAcpiOffCallback,
                              &SwRegisterContext,
                              &SwHandle
                              );
  if (EFI_ERROR (Status)) {
    return Status;
  }

 #ifdef FCH_SPI_PROTECT_SMI
    SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemSpiUnlockSwSmi); // use of PCD in place of FCHOEM_SPI_UNLOCK_SWSMI            0xAA
    SwRegisterContext.Order = 0x80;
    Status = HygonSwDispatch->Register (
                                HygonSwDispatch,
                                HygonSmiSpiUnlockCallback,
                                &SwRegisterContext,
                                &SwHandle
                                );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemSpiLockSwSmi); // use of PCD in place of FCHOEM_SPI_LOCK_SWSMI              0xAB
    SwRegisterContext.Order = 0x80;
    Status = HygonSwDispatch->Register (
                                HygonSwDispatch,
                                HygonSmiSpiLockCallback,
                                &SwRegisterContext,
                                &SwHandle
                                );
    if (EFI_ERROR (Status)) {
      return Status;
    }

 #endif

  if (FchMisc->LongTimer.Enable || FchMisc->ShortTimer.Enable) {
    SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemStartTimerSmi); // use of PCD in place of FCHOEM_START_TIMER_SMI             0xBC
    SwRegisterContext.Order = 0x80;
    Status = HygonSwDispatch->Register (
                                HygonSwDispatch,
                                HygonStartTimerSmiCallback,
                                &SwRegisterContext,
                                &SwHandle
                                );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    SwRegisterContext.HygonSwValue = PcdGet8 (PcdFchOemStopTimerSmi); // use of PCD in place of FCHOEM_STOP_TIMER_SMI              0xBD
    SwRegisterContext.Order = 0x80;
    Status = HygonSwDispatch->Register (
                                HygonSwDispatch,
                                HygonStopTimerSmiCallback,
                                &SwRegisterContext,
                                &SwHandle
                                );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}
