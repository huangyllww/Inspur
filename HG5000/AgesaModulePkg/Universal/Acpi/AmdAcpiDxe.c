/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate SMBIOS type 4 7 16 17 19 20
 *
 * Contains code that generate SMBIOS type 4 7 16 17 19 20
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Universal
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
#include "AGESA.h"
#include "Filecode.h"
#include "AmdAcpiDxe.h"
#include "AmdAcpiCpuSsdt.h"
#include "AmdAcpiSrat.h"
#include "AmdAcpiSlit.h"
#include "AmdAcpiCrat.h"
#include "AmdAcpiCdit.h"
#include "AmdAcpiMsct.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Protocol/AcpiTable.h"
#include "Protocol/AmdAcpiCompleteProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_ACPI_AMDACPIDXE_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Calculate an ACPI style checksum
 *
 * Computes the checksum and stores the value to the checksum
 * field of the passed in ACPI table's header.
 *
 * @param[in]  Table             ACPI table to checksum
 * @param[in]  StdHeader         Config handle for library and services
 *
 */
VOID
ChecksumAcpiTable (
  IN OUT   ACPI_TABLE_HEADER *Table,
  IN       AMD_CONFIG_PARAMS *StdHeader
  )
{
  UINT8  *BuffTempPtr;
  UINT8  Checksum;
  UINT32 BufferOffset;

  Table->Checksum = 0;
  Checksum = 0;
  BuffTempPtr = (UINT8 *) Table;
  for (BufferOffset = 0; BufferOffset < Table->TableLength; BufferOffset++) {
    Checksum = Checksum - *(BuffTempPtr + BufferOffset);
  }

  Table->Checksum = Checksum;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  Entry for AmdAcpiDxe
 *  Generate ACPI tables
 *
 *  Description:
 *    This function will populate ACPI tables with
 *      SLIT:
 *      SRAT:
 *
 *    @retval          EFI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
EFI_STATUS
EFIAPI
AmdGenerateAcpiTables (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                 Status;
  EFI_STATUS                 CalledStatus;
  EFI_HANDLE                 Handle;
  AMD_CONFIG_PARAMS          StdHeader;
  EFI_ACPI_TABLE_PROTOCOL   *AcpiTableProtocol;
  DXE_AMD_ACPI_INIT_COMPLETE_PROTOCOL AmdAcpiInitCompleteProtocol;

  AGESA_TESTPOINT (TpUniversalAcpiEntry, NULL);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdGenerateAcpiTables Entry\n");

  // Avoid re-entry by locating gAmdAcpiDxeInitCompleteProtocolGuid.
  CalledStatus = gBS->LocateProtocol (&gAmdAcpiDxeInitCompleteProtocolGuid, NULL, (VOID **)&AmdAcpiInitCompleteProtocol);
  if (CalledStatus == EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "    re-entry, just return EFI_ALREADY_STARTED\n");
    AGESA_TESTPOINT (TpUniversalAcpiAbort, NULL);
    return EFI_ALREADY_STARTED;
  }

  Status = EFI_SUCCESS;

  // Locate ACPISupport table.
  CalledStatus = gBS->LocateProtocol (
                       &gEfiAcpiTableProtocolGuid,
                       NULL,
                       &AcpiTableProtocol
                       );
  if (EFI_ERROR (CalledStatus)) {
    AGESA_TESTPOINT (TpUniversalAcpiAbort, NULL);
    return CalledStatus;
  }

  // Processor SSDT
  if (PcdGetBool (PcdAmdAcpiCpuSsdt)) {
    CalledStatus = AmdAcpiCpuSSdt (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // System Resource Affinity Table (SRAT)
  if (PcdGetBool (PcdAmdAcpiSrat)) {
    CalledStatus = AmdAcpiSrat (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Maximum System Characteristics Table (MSCT), provided only when SRAT exists
    if ((PcdGetBool (PcdAmdAcpiMsct)) && (CalledStatus == AGESA_SUCCESS)) {
      CalledStatus = AmdAcpiMsct (AcpiTableProtocol, &StdHeader);
      Status = (CalledStatus > Status) ? CalledStatus : Status;
    }
  }

  // System Locality Distance Information Table (SLIT)
  if (PcdGetBool (PcdAmdAcpiSlit)) {
    CalledStatus = AmdAcpiSlit (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // Component Resource Affinity Table (CRAT)
  if (PcdGetBool (PcdAmdAcpiCrat)) {
    CalledStatus = AmdAcpiCrat (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // Component Locality Distance Information Table (CDIT)
  if (PcdGetBool (PcdAmdAcpiCdit)) {
    CalledStatus = AmdAcpiCdit (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // End
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Install Protocol gAmdAcpiDxeInitCompleteProtocolGuid to avoid re-entry\n");
  AmdAcpiInitCompleteProtocol.Revision = 0;
  Handle = NULL;
  gBS->InstallProtocolInterface (
                          &Handle,
                          &gAmdAcpiDxeInitCompleteProtocolGuid,
                          EFI_NATIVE_INTERFACE,
                          &AmdAcpiInitCompleteProtocol
                          );

  IDS_HDT_CONSOLE (MAIN_FLOW, "  AmdGenerateAcpiTables Exit\n");

  AGESA_TESTPOINT (TpUniversalAcpiExit, NULL);

  return Status;

}

