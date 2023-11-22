/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate SMBIOS type 4 7 16 17 19 20
 *
 * Contains code that generate SMBIOS type 4 7 16 17 19 20
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Universal
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
#include "HGPI.h"
#include "Filecode.h"
#include "HygonAcpiDxe.h"
#include "HygonAcpiCpuSsdt.h"
#include "HygonAcpiSrat.h"
#include "HygonAcpiSlit.h"
#include "HygonAcpiCrat.h"
#include "HygonAcpiCdit.h"
#include "HygonAcpiMsct.h"
#include "HygonAcpiHmat.h"
#include "Library/HygonBaseLib.h"
#include "Library/HygonHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Protocol/AcpiTable.h"
#include "Protocol/HygonAcpiCompleteProtocol.h"
#include "PiDxe.h"
#include <Library/UefiLib.h>
#include <Library/HygonSocBaseLib.h>

#define FILECODE  UNIVERSAL_ACPI_HYGONACPIDXE_FILECODE

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
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  UINT8   *BuffTempPtr;
  UINT8   Checksum;
  UINT32  BufferOffset;

  Table->Checksum = 0;
  Checksum    = 0;
  BuffTempPtr = (UINT8 *)Table;
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
 *  AcpiReadyToBoot
 *
 *  Description:
 *     notification event handler when on ReadyToBoot
 *  Parameters:
 *    @param[in]     Event      Event whose notification function is being invoked.
 *    @param[in]     *Context   Pointer to the notification function's context.
 *
 *---------------------------------------------------------------------------------------
 **/
VOID
EFIAPI
AcpiReadyToBoot (
  IN      EFI_EVENT  Event,
  IN      VOID       *Context
  )
{
  EFI_ACPI_TABLE_PROTOCOL                *AcpiTableProtocol;
  HYGON_CONFIG_PARAMS                     StdHeader;
  EFI_STATUS                              CalledStatus;
  UINT32                                  CpuModel;
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "AcpiReadyToBoot Entry\n");
  
  CalledStatus = gBS->LocateProtocol (
                      &gEfiAcpiTableProtocolGuid,
                      NULL,
                      &AcpiTableProtocol
                      );
  if (CalledStatus == EFI_SUCCESS) {
    //  Heterogeneous Memory Attribute Table (HMAT)
    CpuModel = GetHygonSocModel();
    if ((CpuModel == HYGON_GX_CPU) && (PcdGetBool (PcdHygonAcpiHmat) == TRUE)) {
      HygonAcpiHmat (AcpiTableProtocol, &StdHeader);
    }
  }
  
  IDS_HDT_CONSOLE (MAIN_FLOW, "AcpiReadyToBoot Exit\n");
  gBS->CloseEvent (Event);
}

/**
 *---------------------------------------------------------------------------------------
 *
 *  Entry for HygonAcpiDxe
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
HygonGenerateAcpiTables (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  EFI_STATUS                             Status;
  EFI_STATUS                             CalledStatus;
  EFI_HANDLE                             Handle;
  HYGON_CONFIG_PARAMS                    StdHeader;
  EFI_ACPI_TABLE_PROTOCOL                *AcpiTableProtocol;
  DXE_HYGON_ACPI_INIT_COMPLETE_PROTOCOL  HygonAcpiInitCompleteProtocol;
  EFI_EVENT                              ReadyToBootEvent;

  HGPI_TESTPOINT (TpUniversalAcpiEntry, NULL);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  HygonGenerateAcpiTables Entry\n");

  // Avoid re-entry by locating gHygonAcpiDxeInitCompleteProtocolGuid.
  CalledStatus = gBS->LocateProtocol (&gHygonAcpiDxeInitCompleteProtocolGuid, NULL, (VOID **)&HygonAcpiInitCompleteProtocol);
  if (CalledStatus == EFI_SUCCESS) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "    re-entry, just return EFI_ALREADY_STARTED\n");
    HGPI_TESTPOINT (TpUniversalAcpiAbort, NULL);
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
    HGPI_TESTPOINT (TpUniversalAcpiAbort, NULL);
    return CalledStatus;
  }

  // Processor SSDT
  if (PcdGetBool (PcdHygonAcpiCpuSsdt)) {
    CalledStatus = HygonAcpiCpuSSdt (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // System Resource Affinity Table (SRAT)
  if (PcdGetBool (PcdHygonAcpiSrat)) {
    CalledStatus = HygonAcpiSrat (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;

    // Maximum System Characteristics Table (MSCT), provided only when SRAT exists
    if ((PcdGetBool (PcdHygonAcpiMsct)) && (CalledStatus == HGPI_SUCCESS)) {
      CalledStatus = HygonAcpiMsct (AcpiTableProtocol, &StdHeader);
      Status = (CalledStatus > Status) ? CalledStatus : Status;
    }
  }

  // System Locality Distance Information Table (SLIT)
  if (PcdGetBool (PcdHygonAcpiSlit)) {
    CalledStatus = HygonAcpiSlit (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // Component Resource Affinity Table (CRAT)
  if (PcdGetBool (PcdHygonAcpiCrat)) {
    CalledStatus = HygonAcpiCrat (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  // Component Locality Distance Information Table (CDIT)
  if (PcdGetBool (PcdHygonAcpiCdit)) {
    CalledStatus = HygonAcpiCdit (AcpiTableProtocol, &StdHeader);
    Status = (CalledStatus > Status) ? CalledStatus : Status;
  }

  Status = EfiCreateEventReadyToBootEx (TPL_CALLBACK, AcpiReadyToBoot, NULL, &ReadyToBootEvent);

  // End
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Install Protocol gHygonAcpiDxeInitCompleteProtocolGuid to avoid re-entry\n");
  HygonAcpiInitCompleteProtocol.Revision = 0;
  Handle = NULL;
  gBS->InstallProtocolInterface (
         &Handle,
         &gHygonAcpiDxeInitCompleteProtocolGuid,
         EFI_NATIVE_INTERFACE,
         &HygonAcpiInitCompleteProtocol
         );

  IDS_HDT_CONSOLE (MAIN_FLOW, "  HygonGenerateAcpiTables Exit\n");

  HGPI_TESTPOINT (TpUniversalAcpiExit, NULL);

  return Status;
}
