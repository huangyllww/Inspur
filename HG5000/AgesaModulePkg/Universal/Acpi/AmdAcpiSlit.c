/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate System Locality Distance Information Table (SLIT)
 *
 * Contains code that generate System Locality Distance Information Table (SLIT)
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
#include "AmdAcpiSlit.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/BaseLib.h"
#include <Protocol/AmdAcpiSlitServicesProtocol.h>
#include "PiDxe.h"

#define FILECODE UNIVERSAL_ACPI_AMDACPISLIT_FILECODE

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
STATIC SLIT_HEADER  ROMDATA SlitHeaderStruct =
{
  {'S','L','I','T'},
  0,
  1,
  0,
  {0},
  {0},
  1,
  {'H','Y','G','N'},
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
 *  Generate SLIT
 *
 *  Description:
 *    This function will generate a complete System Locality Distance Information Table
 *    i.e. SLIT into a memory buffer.
 *
 *  Parameters:
 *    @param[in]       AcpiTableProtocol          Pointer to gEfiAcpiTableProtocolGuid protocol
 *    @param[in]       StdHeader                  Header for library and services
 *
 *    @retval          AGESA_STATUS
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdAcpiSlit (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8               *Distance;
  UINT8                NumberOfDomains;
  UINTN                TableSize;
  UINTN                TableKey;
  EFI_STATUS           Status;
  SLIT_HEADER         *SlitHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS AllocParams;
  AMD_FABRIC_ACPI_SLIT_SERVICES_PROTOCOL *FabricSlitServices;

  // Locate protocol
  if (gBS->LocateProtocol (&gAmdFabricAcpiSlitServicesProtocolGuid, NULL, (VOID **) &FabricSlitServices) != EFI_SUCCESS) {
    return AGESA_ERROR;
  }

  // Allocate a buffer
  AllocParams.RequestedBufferSize = ACPI_TABLE_MAX_LENGTH;
  AllocParams.BufferHandle = AMD_ACPI_TABLE_BUFFER_HANDLE;
  AllocParams.Persist = HEAP_SYSTEM_MEM;

  if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }

  SlitHeaderStructPtr = (SLIT_HEADER *) AllocParams.BufferPtr;
  Distance = (UINT8 *) SlitHeaderStructPtr + sizeof (SLIT_HEADER);

  // Get SLIT from gAmdFabricAcpiSlitServicesProtocolGuid
  Status = FabricSlitServices->GetSlitDistanceInfo (FabricSlitServices, &NumberOfDomains, Distance);

  // Only create SLIT when NumberOfDomains > 1
  if ((NumberOfDomains > 1) && (Status != EFI_ABORTED)) {
    // Copy SlitHeaderStruct -> data buffer
    LibAmdMemCopy ((VOID *) SlitHeaderStructPtr, (VOID *) &SlitHeaderStruct, (UINTN) (sizeof (SLIT_HEADER)), StdHeader);
    // Update table OEM fields.
    ASSERT (AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 100) <= 6);
    ASSERT (AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiSlitTableHeaderOemTableId), 100) <= 8);

    LibAmdMemCopy ((VOID *) &SlitHeaderStructPtr->OemId,
                   (VOID *) PcdGetPtr (PcdAmdAcpiTableHeaderOemId),
                   AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 6),
                   StdHeader);
    LibAmdMemCopy ((VOID *) &SlitHeaderStructPtr->OemTableId,
                   (VOID *) PcdGetPtr (PcdAmdAcpiSlitTableHeaderOemTableId),
                   AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiSlitTableHeaderOemTableId), 8),
                   StdHeader);

    // Update number of system localities
    SlitHeaderStructPtr->NumberOfSystemLocalities = (UINT64) NumberOfDomains;

    // Store size in table (current buffer offset - buffer start offset)
    SlitHeaderStructPtr->Length = sizeof (SLIT_HEADER) + (UINT32) (NumberOfDomains * NumberOfDomains);

    // Boundary check
    ASSERT (SlitHeaderStructPtr->Length <= ACPI_TABLE_MAX_LENGTH);
    if (SlitHeaderStructPtr->Length > ACPI_TABLE_MAX_LENGTH) {
      IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: ACPI table buffer is overflow\n");
      IDS_DEADLOOP ()
    }

    // Update SLIT header Checksum
    ChecksumAcpiTable ((ACPI_TABLE_HEADER *) SlitHeaderStructPtr, StdHeader);

    // Publish SLIT
    TableSize = SlitHeaderStructPtr->Length;
    TableKey = 0;
    AcpiTableProtocol->InstallAcpiTable (
                    AcpiTableProtocol,
                    SlitHeaderStructPtr,
                    TableSize,
                    &TableKey
                    );
    IDS_HDT_CONSOLE (MAIN_FLOW, "  SLIT is created\n");
  }

  // Deallocate heap
  HeapDeallocateBuffer (AMD_ACPI_TABLE_BUFFER_HANDLE, StdHeader);

  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

