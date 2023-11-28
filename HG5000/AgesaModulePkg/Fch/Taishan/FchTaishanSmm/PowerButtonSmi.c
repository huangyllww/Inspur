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
#define FILECODE UEFI_SMM_FCHSMM_POWERBUTTONSMI_FILECODE

EFI_STATUS
FchSmmRegisterPowerButtonSmi (
  VOID
  )
{
  EFI_STATUS                              Status;
  FCH_SMM_PWRBTN_DISPATCH2_PROTOCOL       *AmdPwrBtnDispatch;
  FCH_SMM_PWRBTN_REGISTER_CONTEXT         AmdPowerButtonContext;
  EFI_HANDLE                              PowerButtonHandle;

  //
  // Register power button SMM event
  //
  Status = gSmst->SmmLocateProtocol (
                  &gFchSmmPwrBtnDispatch2ProtocolGuid,
                  NULL,
                  &AmdPwrBtnDispatch
                  );
  ASSERT_EFI_ERROR (Status);

  AmdPowerButtonContext.Phase = EfiPowerButtonExit;
  AmdPowerButtonContext.Order = 0xFF;
  Status = AmdPwrBtnDispatch->Register (
                                AmdPwrBtnDispatch,
                                AmdSmiPowerButtonCallback,
                                &AmdPowerButtonContext,
                                &PowerButtonHandle
                                );

  AmdPowerButtonContext.Phase = EfiPowerButtonEntry;
  AmdPowerButtonContext.Order = 0x89;
  Status = AmdPwrBtnDispatch->Register (
                                AmdPwrBtnDispatch,
                                AmdSmiPowerButtonCallbackTest,
                                &AmdPowerButtonContext,
                                &PowerButtonHandle
                                );
  return Status;
}


//
// Enable sleep state transition
//
VOID
FchEnableSleepTransition ( VOID )
{
  // Set SLP_TYPEn to enable SLP_TYP to do S1/S3/S5
  RwPmio (FCH_PMIOA_REGBE, AccessWidth8, 0xFF, BIT5, NULL);

  // Enable S state transition
  RwPmio (FCH_PMIOA_REG08 + 3, AccessWidth8, (UINT32)~(BIT1 + BIT0), (UINT32) BIT0, NULL);
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
AmdSmiPowerButtonCallbackTest (
  IN       EFI_HANDLE                            DispatchHandle,
  IN       CONST FCH_SMM_PWRBTN_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                                  *CommBuffer OPTIONAL,
  IN OUT   UINTN                                 *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS     Status;
  Status = EFI_SUCCESS;
  return Status;
}


EFI_STATUS
EFIAPI
AmdSmiPowerButtonCallback (
  IN       EFI_HANDLE                            DispatchHandle,
  IN       CONST FCH_SMM_PWRBTN_REGISTER_CONTEXT *DispatchContext,
  IN OUT   VOID                                  *CommBuffer OPTIONAL,
  IN OUT   UINTN                                 *CommBufferSize  OPTIONAL
  )
{
  EFI_STATUS     Status;
  UINT32         AcpiMmioBase;
  UINT16         AcpiPmbase;
  UINT16         Value16;

  Status = EFI_SUCCESS;

  FchEnableSleepTransition ();
//  GetFchAcpiMmioBase (&AcpiMmioBase, NULL);
  AcpiMmioBase = 0xFED80000;
  GetFchAcpiPmBase (&AcpiPmbase, NULL);
  BackUpCG2 ();

  // Turn off SLP_TYP SMI
  RwMem (AcpiMmioBase + SMI_BASE + FCH_SMI_REGB0, AccessWidth32, (UINT32)~(BIT3 + BIT2), 0);

  // Clear power button status
  Value16 = BIT8;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmbase + R_FCH_ACPI_PM1_STATUS, &Value16);

  // power the system off now
  LibFchSmmIoRead (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &Value16);
  Value16 &= (UINT32)~(BIT12 + BIT11 + BIT10);
  Value16 |= 5 << 10;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &Value16);
  Value16 |= BIT13;
  LibFchSmmIoWrite (&gSmst->SmmIo, SMM_IO_UINT16, AcpiPmbase + R_FCH_ACPI_PM_CONTROL, &Value16);

  return Status;
}

