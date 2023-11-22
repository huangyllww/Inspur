/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   SMM Control DXE Driver
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

#include "SmmControl.h"
#define FILECODE  UEFI_DXE_SMMCONTROL_SMMCONTROL_FILECODE

#define MemRead32(x)      MmioRead32 (x)
#define MemWrite32(x, y)  MmioWrite32 (x, y)

UINT32                           mAcpiMmioBase;
UINT16                            mAcpiPmBase;
UINT8                             mAcpiSmiCmd;
EFI_PHYSICAL_ADDRESS              mAcpiMmioBase64;
STATIC EFI_SMM_CONTROL2_PROTOCOL  mSmmControl;
EFI_EVENT                         mVirtualAddressChangeEvent = NULL;

EFI_STATUS
SmmControlDxePreInit (
  IN       EFI_HANDLE              ImageHandle
  )
{
  UINT16                           SmmControlData16;
  UINT16                           SmmControlMask16;
  UINT32                           SmmControlData32;
  UINT8                            SmmControlData8;
  UINT8                            i;
  EFI_HANDLE                       mDriverImageHandle;
  EFI_STATUS                       Status;
  UINT64                           Length;
  UINT64                           Attributes;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR  GcdMemorySpaceDescriptor;

  //
  // Enable ACPI MMIO space
  //
  LibFchIndirectIoRead (EfiPciWidthUint8, FCH_IOMAP_REGCD6, FCH_PMIOA_REG04, &SmmControlData8);
  SmmControlData8 |= BIT1;
  LibFchIndirectIoWrite (EfiPciWidthUint8, FCH_IOMAP_REGCD6, FCH_PMIOA_REG04, &SmmControlData8);

  //
  // Get ACPI MMIO base and AcpiPm1EvtBlk address
  //
  LibFchGetAcpiMmioBase (&mAcpiMmioBase);
  LibFchGetAcpiPmBase (&mAcpiPmBase);
  LibFchIndirectIoRead (EfiPciWidthUint8, FCH_IOMAP_REGCD6, FCH_PMIOA_REG6A, &mAcpiSmiCmd);

  if ((0 == mAcpiMmioBase) || (0 == mAcpiPmBase)) {
    return EFI_LOAD_ERROR;
  }

  mAcpiMmioBase64 = (EFI_PHYSICAL_ADDRESS)mAcpiMmioBase;
  Length = 0x1000;
  mDriverImageHandle = ImageHandle;

  // This might need to be done in a separate driver I think the IBVs currently do this in a separate driver.
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  mAcpiMmioBase64,
                  Length,
                  EFI_MEMORY_RUNTIME | EFI_MEMORY_UC
                  );
  if (!EFI_ERROR (Status)) {
    Status = gDS->AllocateMemorySpace (
                    EfiGcdAllocateAddress,
                    EfiGcdMemoryTypeMemoryMappedIo,
                    12,
                    Length,
                    &mAcpiMmioBase64,
                    mDriverImageHandle,
                    NULL
                    );
    ASSERT (!EFI_ERROR (Status));

    Status = gDS->GetMemorySpaceDescriptor (mAcpiMmioBase, &GcdMemorySpaceDescriptor);
    ASSERT (!EFI_ERROR (Status));

    Attributes = GcdMemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME | EFI_MEMORY_UC;

    Status = gDS->SetMemorySpaceAttributes (
                    mAcpiMmioBase,
                    Length,
                    Attributes
                    );
    ASSERT (!EFI_ERROR (Status));
  }

  //
  // Clean up all SMI status and enable bits
  //
  // Clear all SmiControl registers
  SmmControlData32 = 0;
  for (i = FCH_SMI_REGA0; i <= FCH_SMI_REGC4; i += 4) {
    LibFchMemWrite (EfiPciWidthUint32, mAcpiMmioBase + SMI_BASE + i, &SmmControlData32);
  }

  LibFchMemWrite (EfiPciWidthUint32, mAcpiMmioBase + SMI_BASE + FCH_SMI_REG14, &SmmControlData32);

  // Clear all SmiStatus registers (SmiStatus0-4)
  SmmControlData32 = 0xFFFFFFFF;
  LibFchMemWrite (EfiPciWidthUint32, mAcpiMmioBase + SMI_BASE + FCH_SMI_REG80, &SmmControlData32);
  LibFchMemWrite (EfiPciWidthUint32, mAcpiMmioBase + SMI_BASE + FCH_SMI_REG84, &SmmControlData32);
  LibFchMemWrite (EfiPciWidthUint32, mAcpiMmioBase + SMI_BASE + FCH_SMI_REG88, &SmmControlData32);
  LibFchMemWrite (EfiPciWidthUint32, mAcpiMmioBase + SMI_BASE + FCH_SMI_REG8C, &SmmControlData32);
  LibFchMemWrite (EfiPciWidthUint32, mAcpiMmioBase + SMI_BASE + FCH_SMI_REG90, &SmmControlData32);
  LibFchMemWrite (EfiPciWidthUint32, mAcpiMmioBase + SMI_BASE + FCH_SMI_REG10, &SmmControlData32);

  //
  // If SCI is not enabled, clean up all ACPI PM status/enable registers
  //
  LibFchIoRead (EfiPciWidthUint16, mAcpiPmBase + R_FCH_ACPI_PM_CONTROL, &SmmControlData16);
  if (!(SmmControlData16 & BIT0)) {
    // Clear WAKE_EN, RTC_EN, SLPBTN_EN, PWRBTN_EN, GBL_EN and TMR_EN
    SmmControlData16 = 0;
    SmmControlMask16 = (UINT16) ~(BIT15 + BIT10 + BIT9 + BIT8 + BIT5 + BIT0);
    LibFchIoRw (EfiPciWidthUint16, mAcpiPmBase + R_FCH_ACPI_PM1_ENABLE, &SmmControlMask16, &SmmControlData16);

    // Clear WAKE_STS, RTC_STS, SLPBTN_STS, PWRBTN_STS, GBL_STS and TMR_STS
    SmmControlData16 = BIT15 + BIT10 + BIT9 + BIT8 + BIT5 + BIT0;
    LibFchIoWrite (EfiPciWidthUint16, mAcpiPmBase + R_FCH_ACPI_PM1_STATUS, &SmmControlData16);

    // Clear SLP_TYPx
    SmmControlData16 = 0;
    SmmControlMask16 = (UINT16) ~(BIT12 + BIT11 + BIT10);
    LibFchIoRw (EfiPciWidthUint16, mAcpiPmBase + R_FCH_ACPI_PM_CONTROL, &SmmControlMask16, &SmmControlData16);

    // Clear GPE0 Enable Register
    SmmControlData32 = 0;
    LibFchIoWrite (EfiPciWidthUint32, mAcpiPmBase + R_FCH_ACPI_EVENT_ENABLE, &SmmControlData32);

    // Clear GPE0 Status Register
    SmmControlData32 = 0xFFFFFFFF;
    LibFchIoWrite (EfiPciWidthUint32, mAcpiPmBase + R_FCH_ACPI_EVENT_STATUS, &SmmControlData32);
  }

  //
  // Set the EOS Bit
  // Clear SmiEnB to enable SMI function
  //
  SmmControlData32  = MemRead32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98);
  SmmControlData32 |= BIT28;
  SmmControlData32 &= ~BIT31;
  MemWrite32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98, SmmControlData32);

  return EFI_SUCCESS;
}

EFI_STATUS
ClearSmi (
  VOID
  )
{
  UINT32  SmmControlData32;

  //
  // Clear SmiCmdPort Status Bit
  //
  SmmControlData32 = BIT11;
  MemWrite32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG88, SmmControlData32);

  //
  // Set the EOS Bit if it is currently cleared so we can get an SMI otherwise
  // leave the register alone
  //
  SmmControlData32 = MemRead32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98);
  if ((SmmControlData32 & BIT28) == 0) {
    SmmControlData32 |= BIT28;
    MemWrite32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98, SmmControlData32);
  }

  return EFI_SUCCESS;
}

// Invoke SMI activation from either preboot or runtime environment
EFI_STATUS
EFIAPI
HygonTrigger (
  IN       CONST EFI_SMM_CONTROL2_PROTOCOL                *This,
  IN OUT   UINT8                      *CommandPort        OPTIONAL,
  IN OUT   UINT8                      *DataPort           OPTIONAL,
  IN       BOOLEAN                    Periodic            OPTIONAL,
  IN       UINTN                      ActivationInterval  OPTIONAL
  )
{
  UINT8   bIndex;
  UINT8   bData;
  UINT32  SmmControlData32;
  UINT8   bIrqMask;
  UINT8   bIrqMask1;

  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  if (NULL == CommandPort) {
    bIndex = 0xff;
  } else {
    bIndex = *CommandPort;
  }

  if (NULL == DataPort) {
    bData = 0xff;
  } else {
    bData = *DataPort;
  }

  //
  // Enable CmdPort SMI
  //
  SmmControlData32  = MemRead32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REGB0);
  SmmControlData32 &= ~(BIT22 + BIT23);
  SmmControlData32 |= BIT22;
  MemWrite32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REGB0, SmmControlData32);

  // Enable Global SMI
  SmmControlData32  = MemRead32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98);
  SmmControlData32 &= 0x7FFFFFFF;
  MemWrite32 (mAcpiMmioBase64 + SMI_BASE + FCH_SMI_REG98, SmmControlData32);

  // Temporal WA-Ensure IRQ0 is masked off when entering software SMI
  bIrqMask = IoRead8 (FCH_8259_MASK_REG_MASTER);
  if ((bIrqMask & BIT0) == 0) {
    bIrqMask1 = bIrqMask | BIT0;
    IoWrite8 (FCH_8259_MASK_REG_MASTER, bIrqMask1);
  }

  // -WA
  ClearSmi ();
  //
  // Issue command port SMI
  //
  IoWrite16 (mAcpiSmiCmd, (bData << 8) + bIndex);
  //
  // Rearm SMM to ensure if SMM handled didn't get a chance to set EOS bit
  // and OS etc write to Soft SMM port it can still generate soft SMI
  //
  ClearSmi ();
  // WA-Restore IRQ0 mask if needed
  if ((bIrqMask & BIT0) == 0) {
    IoWrite8 (FCH_8259_MASK_REG_MASTER, bIrqMask);
  }

  // -WA
  return EFI_SUCCESS;
}

// Clear any system state that was created in response to the Trigger call
EFI_STATUS
EFIAPI
HygonClear (
  IN       CONST EFI_SMM_CONTROL2_PROTOCOL *This,
  IN       BOOLEAN                         Periodic OPTIONAL
  )
{
  if (Periodic) {
    return EFI_INVALID_PARAMETER;
  }

  return ClearSmi ();
}

//
// FUNCTION NAME.
// VariableVirtualAddressChangeCallBack - Call back function for Virtual Address Change Event.
//
// FUNCTIONAL DESCRIPTION.
// This function convert the virtual addreess to support runtime access.
//
// ENTRY PARAMETERS.
// Event           - virtual Address Change Event.
// Context         - virtual Address Change call back Context.
//
// EXIT PARAMETERS.
// None.
//
// WARNINGS.
// None.
//

VOID
EFIAPI
VariableVirtualAddressChangeCallBack (
  IN EFI_EVENT Event,
  IN VOID *Context
  )
{
  EfiConvertPointer (0, (VOID **)&mAcpiMmioBase64);
  EfiConvertPointer (0, (VOID *)&(mSmmControl.Trigger));
  EfiConvertPointer (0, (VOID *)&(mSmmControl.Clear));

  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Name: HygonInstallSmmControl
 *
 * This DXE driver produces the SMM Control Protocol
 *
 * @param[in]    ImageHandle     Pointer to the image handle
 * @param[in]    SystemTable     Pointer to the EFI system table
 *
 * @retval       EFI_SUCCESS     Driver initialized successfully
 * @retval       EFI_ERROR       Driver initialization failed
 *
 */
/*----------------------------------------------------------------------------------------*/
EFI_STATUS
EFIAPI
HygonInstallSmmControl (
  IN       EFI_HANDLE              ImageHandle,
  IN       EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS  Status;

  //
  // Initialize EFI library
  //
  Status = SmmControlDxePreInit (ImageHandle);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  VariableVirtualAddressChangeCallBack,
                  NULL,
                  &gEfiEventVirtualAddressChangeGuid,
                  &mVirtualAddressChangeEvent
                  );
  ASSERT (!EFI_ERROR (Status));

  mSmmControl.Trigger = HygonTrigger;
  mSmmControl.Clear   = HygonClear;
  mSmmControl.MinimumTriggerPeriod = 0;

  //
  // Finally install the protocol
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiSmmControl2ProtocolGuid,
                  &mSmmControl,
                  NULL
                  );
  ASSERT (!EFI_ERROR (Status));

  return Status;
}
