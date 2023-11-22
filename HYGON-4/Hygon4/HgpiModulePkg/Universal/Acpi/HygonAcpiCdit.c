/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate Component Locality Distance Information Table (CDIT)
 *
 * Contains code that generate Component Locality Distance Information Table (CDIT)
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
#include "HygonAcpiCdit.h"
#include "Library/HygonBaseLib.h"
#include "Library/HygonHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/BaseLib.h"
#include <Protocol/HygonAcpiCditServicesProtocol.h>
#include "PiDxe.h"

#define FILECODE  UNIVERSAL_ACPI_HYGONACPICDIT_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *  All of the DATA should be defined in _CODE segment.
 *  Use ROMDATA to specify that it belongs to _CODE.
 *----------------------------------------------------------------------------
 */
STATIC CDIT_HEADER  ROMDATA  CditHeaderStruct =
{
  { 'C', 'D', 'I', 'T' },
  0,
  1,
  0,
  { 0 },
  { 0 },
  1,
  { 'H', 'Y', 'G', 'N' },
  1,
  1
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate CDIT
 *
 *  Description:
 *    This function will generate a complete Component Locality Distance Information Table
 *    i.e. CDIT into a memory buffer.
 *
 *  Parameters:
 *    @param[in]       AcpiTableProtocol          Pointer to gEfiAcpiTableProtocolGuid protocol
 *    @param[in]       StdHeader                  Header for library and services
 *
 *    @retval          HGPI_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
HGPI_STATUS
HygonAcpiCdit (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       HYGON_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8                                     *Distance;
  UINT8                                     NumberOfDomains;
  UINTN                                     TableSize;
  UINTN                                     TableKey;
  CDIT_HEADER                               *CditHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS                      AllocParams;
  HYGON_FABRIC_ACPI_CDIT_SERVICES_PROTOCOL  *FabricCditServices;

  // Locate protocol
  if (gBS->LocateProtocol (&gHygonFabricAcpiCditServicesProtocolGuid, NULL, (VOID **)&FabricCditServices) != EFI_SUCCESS) {
    return HGPI_ERROR;
  }

  // Allocate a buffer
  AllocParams.RequestedBufferSize = ACPI_TABLE_MAX_LENGTH;
  AllocParams.BufferHandle = HYGON_ACPI_TABLE_BUFFER_HANDLE;
  AllocParams.Persist = HEAP_SYSTEM_MEM;

  if (HeapAllocateBuffer (&AllocParams, StdHeader) != HGPI_SUCCESS) {
    return HGPI_ERROR;
  }

  CditHeaderStructPtr = (CDIT_HEADER *)AllocParams.BufferPtr;
  Distance = (UINT8 *)CditHeaderStructPtr + sizeof (CDIT_HEADER);

  // Copy CditHeaderStruct -> data buffer
  LibHygonMemCopy ((VOID *)CditHeaderStructPtr, (VOID *)&CditHeaderStruct, (UINTN)(sizeof (CDIT_HEADER)), StdHeader);
  // Update table OEM fields.
  ASSERT (AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiTableHeaderOemId), 100) <= 6);
  ASSERT (AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiCditTableHeaderOemTableId), 100) <= 8);

  LibHygonMemCopy (
    (VOID *)&CditHeaderStructPtr->OemId,
    (VOID *)PcdGetPtr (PcdHygonAcpiTableHeaderOemId),
    AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiTableHeaderOemId), 6),
    StdHeader
    );
  LibHygonMemCopy (
    (VOID *)&CditHeaderStructPtr->OemTableId,
    (VOID *)PcdGetPtr (PcdHygonAcpiCditTableHeaderOemTableId),
    AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiCditTableHeaderOemTableId), 8),
    StdHeader
    );

  // Get CDID from gHygonFabricAcpiCditServicesProtocolGuid
  FabricCditServices->GetCditDistanceInfo (FabricCditServices, &NumberOfDomains, Distance);

  // Update number of system localities
  CditHeaderStructPtr->NumDomains = (UINT64)NumberOfDomains;

  // Store size in table (current buffer offset - buffer start offset)
  CditHeaderStructPtr->Length = sizeof (CDIT_HEADER) + (UINT32)(NumberOfDomains * NumberOfDomains);

  // Boundary check
  ASSERT (CditHeaderStructPtr->Length <= ACPI_TABLE_MAX_LENGTH);
  if (CditHeaderStructPtr->Length > ACPI_TABLE_MAX_LENGTH) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: ACPI table buffer is overflow\n");
    IDS_DEADLOOP ()
  }

  // Update SSDT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *)CditHeaderStructPtr, StdHeader);

  // Publish SSDT
  TableSize = CditHeaderStructPtr->Length;
  TableKey  = 0;
  AcpiTableProtocol->InstallAcpiTable (
                       AcpiTableProtocol,
                       CditHeaderStructPtr,
                       TableSize,
                       &TableKey
                       );

  // Deallocate heap
  HeapDeallocateBuffer (HYGON_ACPI_TABLE_BUFFER_HANDLE, StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  CDIT is created\n");
  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
