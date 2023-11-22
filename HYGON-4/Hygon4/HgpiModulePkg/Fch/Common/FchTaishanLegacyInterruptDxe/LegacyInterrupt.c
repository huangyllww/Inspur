/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH DXE Driver
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project   Legacy Interrupt DXE Driver
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

#include "LegacyInterrupt.h"
#define FILECODE  UEFI_DXE_LEGACYINTERRUPT_LEGACYINTERRUPT_FILECODE

// Get the number of PIRQs supported.
EFI_STATUS
HygonGetNumberPirqs (
  IN       EFI_LEGACY_INTERRUPT_PROTOCOL *This,
  OUT   UINT8                         *NumberPirqs
  )
{
  if (NULL == NumberPirqs) {
    return EFI_INVALID_PARAMETER;
  }

  *NumberPirqs = MAX_NUMBER_PIRQS;
  return EFI_SUCCESS;
}

// Get the PCI bus#, device# and function# that associated with this protocol.
EFI_STATUS
HygonGetLocation (
  IN    EFI_LEGACY_INTERRUPT_PROTOCOL *This,
  OUT   UINT8                         *Bus,
  OUT   UINT8                         *Device,
  OUT   UINT8                         *Function
  )
{
  if ((NULL == Bus) || (NULL == Device) || (NULL == Function)) {
    return EFI_INVALID_PARAMETER;
  }

  *Bus      = FCH_PCI_BUS;
  *Device   = FCH_PCI_DEV;
  *Function = FCH_PCI_FUNC;

  return EFI_SUCCESS;
}

// Read the given PIRQ register and return the IRQ value that is assigned to it.
EFI_STATUS
HygonReadPirq (
  IN       EFI_LEGACY_INTERRUPT_PROTOCOL *This,
  IN       UINT8                         PirqNumber,
  OUT      UINT8                         *PirqData
  )
{
  if (NULL == PirqData) {
    return EFI_INVALID_PARAMETER;
  }

  LibFchIndirectIoRead (EfiPciWidthUint8, FCH_IOMAP_REGC00, PirqNumber, PirqData);

  return EFI_SUCCESS;
}

// Write an IRQ value to the specified PIRQ register.
EFI_STATUS
HygonWritePirq (
  IN       EFI_LEGACY_INTERRUPT_PROTOCOL *This,
  IN       UINT8                         PirqNumber,
  IN       UINT8                         PirqData
  )
{
  // if ((PirqNumber >= MAX_NUMBER_PIRQS) || (PirqData > 15)) {
  // return EFI_INVALID_PARAMETER;
  // }
  LibFchIndirectIoWrite (EfiPciWidthUint8, FCH_IOMAP_REGC00, PirqNumber, &PirqData);

  return EFI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Name: HygonInstallLegacyInterrupt
 *
 * This DXE driver produces the legacy interrupt protocol as part of the EfiCompatibility
 * component in CSM. This protocol will manage the programming of PCI IRQ assignment.
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
HygonInstallLegacyInterrupt (
  IN       EFI_HANDLE              ImageHandle,
  IN       EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_LEGACY_INTERRUPT_PROTOCOL  *LegacyInterrupt;
  VOID                           *Instance;
  UINT8                          i;
  UINT8                          bValue;
  EFI_STATUS                     Status;

  //
  // Initialize PCI IRQ routing registers for INTA#-INTH#
  //
  for (i = 0; i < 8; i++) {
    bValue = i | BIT7;                                 // Select IRQ routing to APIC
    LibFchIoWrite (EfiPciWidthUint8, FCH_IOMAP_REGC00, &bValue);
    bValue = i | BIT4;
    LibFchIoWrite (EfiPciWidthUint8, FCH_IOMAP_REGC01, &bValue);
  }

  //
  // Check and see if the protocol has been installed already
  //
  Status = gBS->LocateProtocol (
                  &gEfiLegacyInterruptProtocolGuid,
                  NULL,
                  &Instance
                  );
  ASSERT (EFI_ERROR (Status));

  //
  // Allocate memory and initialize the data structure
  //
  Status = gBS->AllocatePool (
                  EfiBootServicesData,
                  sizeof (EFI_LEGACY_INTERRUPT_PROTOCOL),
                  &LegacyInterrupt
                  );
  ASSERT (!EFI_ERROR (Status));

  LegacyInterrupt->GetNumberPirqs = HygonGetNumberPirqs;
  LegacyInterrupt->GetLocation    = HygonGetLocation;
  LegacyInterrupt->ReadPirq  = HygonReadPirq;
  LegacyInterrupt->WritePirq = HygonWritePirq;

  //
  // Finally install the protocol
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gEfiLegacyInterruptProtocolGuid,
                  LegacyInterrupt,
                  NULL
                  );
  ASSERT (!EFI_ERROR (Status));

  return (Status);
}
