/* $NoKeywords:$ */
/**
 * @file
 *
 * FCH SMM Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project   FCH SMM Driver
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 10:28:05 -0800 (Tue, 09 Dec 2014) $
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
#include "FchSmm.h"
#define FILECODE UEFI_SMM_FCHSMM_MISCSMI_FILECODE

extern FCH_DATA_BLOCK                      gFchDataInSmm;

EFI_STATUS
FchSmmRegisterMiscSmi (
  VOID
  )
{
  EFI_STATUS                                 Status;
  FCH_SMM_MISC_DISPATCH_PROTOCOL             *FchSmmMiscDispatch;
  FCH_SMM_MISC_REGISTER_CONTEXT              MiscRegisterContext;
  EFI_HANDLE                                 MiscHandle;
  FCH_DATA_BLOCK                             *FchDb;
  //UINT8                                      GppHpGeventNum;

  FchDb = &gFchDataInSmm;

  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmMiscDispatchProtocolGuid,
                  NULL,
                  &FchSmmMiscDispatch
                  );
  ASSERT_EFI_ERROR (Status);
  MiscRegisterContext.Order        = 0x80;
  MiscHandle                       = NULL;

  ////
  //// Smi SBTSI test Done
  ////
  //Status = gBS->LocateProtocol (
  //                &gFchSmmMiscDispatchProtocolGuid,
  //                NULL,
  //                &FchSmmMiscDispatch
  //                );
  //ASSERT_EFI_ERROR (Status);

  //MiscRegisterContext.SmiStatusReg = FCH_SMI_REG84;
  //MiscRegisterContext.SmiStatusBit = BIT15;
  //MiscRegisterContext.Order        = 0x80;
  //Status = FchSmmMiscDispatch->Register (
  //                                 FchSmmMiscDispatch,
  //                                 AmdMiscFchTsiSmiCallback,
  //                                 &MiscRegisterContext,
  //                                 &MiscHandle
  //                                 );
  //ACPIMMIO8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAB) &= ~(BIT7);
  //ACPIMMIO8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGAB) |= (BIT6);

  //
  // Initialize GPP hotplug SMI
  //
  //if (FchDb->Gpp.PortCfg[0].PortHotPlug || FchDb->Gpp.PortCfg[1].PortHotPlug ||  \
  //    FchDb->Gpp.PortCfg[2].PortHotPlug || FchDb->Gpp.PortCfg[3].PortHotPlug) {
  //  GppHpGeventNum = FchDb->Gpp.GppHotPlugGeventNum & 31;
  //  MiscRegisterContext.SmiStatusReg = FCH_SMI_REG80;
  //  MiscRegisterContext.SmiStatusBit = 1 << GppHpGeventNum;
  //  MiscRegisterContext.Order        = 0x80;
  //  Status = FchSmmMiscDispatch->Register (
  //                                 FchSmmMiscDispatch,
  //                                 AmdMiscFchGppHpSmiCallback,
  //                                 &MiscRegisterContext,
  //                                 &MiscHandle
  //                                 );
  //  ACPIMMIO8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA0 + ((GppHpGeventNum << 1) / 8)) &= ~(0x03 << ((GppHpGeventNum << 1) % 8));
  //  ACPIMMIO8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGA0 + ((GppHpGeventNum << 1) / 8)) |= (0x01 << ((GppHpGeventNum << 1) % 8));
  //}

#ifdef FCH_TIMER_TICK_INTERVAL_WA
  //
  // Initialize timer tick interval workaround
  //
  MiscRegisterContext.SmiStatusReg = FCH_SMI_REG90;
  MiscRegisterContext.SmiStatusBit = BIT24;
  MiscRegisterContext.Order        = 0x80;
  Status = FchSmmMiscDispatch->Register (
                                 FchSmmMiscDispatch,
                                 AmdMiscFchHpetIntervalCallback,
                                 &MiscRegisterContext,
                                 &MiscHandle
                                 );
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGD0) = 0xFED00109;
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGC4) |= BIT16;

  //
  // Initialize timer tick interval workaround
  //
  MiscRegisterContext.SmiStatusReg = FCH_SMI_REG8C;
  MiscRegisterContext.SmiStatusBit = BIT2;
  MiscRegisterContext.Order        = 0x80;
  Status = FchSmmMiscDispatch->Register (
                                 FchSmmMiscDispatch,
                                 AmdMiscFchIrq2TrapCallback,
                                 &MiscRegisterContext,
                                 &MiscHandle
                                 );
  //Set IRQ2 smi active high
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG9C) |= BIT2;
  //Select IoApic IRQ2 smi trap
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG98) &= ~(BIT24);
#endif

  return Status;
}


/*++

Routine Description:
  SMI handler to restore ACPI mode

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the FCH_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
/*
EFI_STATUS
EFIAPI
AmdMiscFchTsiSmiCallback (
  IN  EFI_HANDLE                        DispatchHandle,
  IN  FCH_SMM_MISC_REGISTER_CONTEXT     *DispatchContext
  )
{
  EFI_STATUS     Status;
  UINT8          bData;
//  UINT8          *pData;
  Status = 0;
  //to be implemented

  bData = 0xff;
  LibAmdIoWrite (AccessWidth8, 0xb00, &bData, NULL);
  LibAmdIoRead (AccessWidth8, 0xb04, &bData, NULL);

  LibAmdIoWrite (AccessWidth8, 0xb04, &bData, NULL);
  //LibAmdIoWrite (AccessWidth8, 0x80, &bData, NULL);
  while (ACPIMMIO8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84 + 1 ) & (BIT7)) ACPIMMIO8 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG84 + 1 ) |= (BIT7);
  bData = 0xff;
  LibAmdIoWrite (AccessWidth8, 0xb00, &bData, NULL);

  return Status;
}
*/

/*++

Routine Description:
  SMI handler to handle GPP hotplug

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the FCH_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
/*
EFI_STATUS
EFIAPI
AmdMiscFchGppHpSmiCallback (
  IN  EFI_HANDLE                         DispatchHandle,
  IN  FCH_SMM_MISC_REGISTER_CONTEXT      *DispatchContext
  )
{
  FCH_DATA_BLOCK     *FchDb;

  FchDb = &gFchInitInSmm.FchPolicy;
  FchGppHotplugSmiCallbackPtr (FchDb);
  return EFI_SUCCESS;
}
*/

#ifdef FCH_TIMER_TICK_INTERVAL_WA
/*++

Routine Description:
  SMI handler to HPET Interval Write Trap

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the FCH_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdMiscFchHpetIntervalCallback (
  IN  EFI_HANDLE                         DispatchHandle,
  IN  FCH_SMM_MISC_REGISTER_CONTEXT      *DispatchContext
  )
{
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG90) |= BIT24;     //
  //Enable IRQ2 trap
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB8) |= BIT4;     //
  return EFI_SUCCESS;
}
/*++

Routine Description:
  SMI handler to Irq2 Trap

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering
  DispatchContext - Pointer to the FCH_SMM_SW_DISPATCH_CONTEXT

Returns:

  None.

--*/
EFI_STATUS
EFIAPI
AmdMiscFchIrq2TrapCallback (
  IN  EFI_HANDLE                         DispatchHandle,
  IN  FCH_SMM_MISC_REGISTER_CONTEXT      *DispatchContext
  )
{
  UINT32        HpetInterval;
  UINT8         RtcIndex;
  UINT8         RtcRegA;
  UINT8         RtcRegB;
  //Disable IRQ2 trap
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB8) &= ~(BIT4 + BIT5);     //
  HpetInterval = ACPIMMIO32 (0xFED00108) - ACPIMMIO32 (0xFED000F0);
  //while (HpetInterval >= 0) {
  //}
  if (HpetInterval > 15000000 / 70) {
    RWPMIO (FCH_PMIOA_REGA0, AccessWidth8, ~BIT1, 0);
    RtcIndex = IoRead8 (0x70);
    IoWrite8 (0x70, 0x0A);
    RtcRegA = IoRead8 (0x71);
    IoWrite8 (0x71, ((RtcRegA & 0xF0) | 0x0E));
    IoWrite8 (0x70, 0x0B);
    RtcRegB = IoRead8 (0x71);
    IoWrite8 (0x71, (RtcRegB | BIT6));
    RWPMIO (FCH_PMIOA_REGA0 + 2, AccessWidth8, ~BIT1, BIT1);
    RWPMIO (FCH_PMIOA_REGA0, AccessWidth8, ~BIT1, BIT1);
    RWPMIO (FCH_PMIOA_REGA0 + 2, AccessWidth8, ~BIT1, 0);
    RWPMIO (FCH_PMIOA_REGA0, AccessWidth8, ~BIT1, 0);
    IoWrite8 (0x70, 0x0A);
    IoWrite8 (0x71, RtcRegA);
    IoWrite8 (0x70, 0x0B);
    IoWrite8 (0x71, RtcRegB);
    IoWrite8 (0x70, RtcIndex);
  } else {
    RWPMIO (FCH_PMIOA_REGA0, AccessWidth8, ~BIT1, BIT1);
  }

  //Disable IRQ2 trap
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REGB8) &= ~(BIT4 + BIT5);     //
  ACPIMMIO32 (ACPI_MMIO_BASE + SMI_BASE + FCH_SMI_REG8C) |= BIT2;     //
  return EFI_SUCCESS;
}
#endif

