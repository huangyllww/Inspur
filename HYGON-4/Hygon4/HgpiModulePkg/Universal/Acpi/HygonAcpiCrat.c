/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate Component Resource Affinity Table (CRAT)
 *
 * Contains code that generate Component Resource Affinity Table (CRAT)
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
#include "HygonAcpiCrat.h"
#include "Library/HygonBaseLib.h"
#include "Library/HygonHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/BaseLib.h"
#include "Protocol/HygonCoreTopologyProtocol.h"
#include "Protocol/FabricTopologyServices.h"
#include "Protocol/HygonAcpiCratServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE  UNIVERSAL_ACPI_HYGONACPICRAT_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
MakeHSAProcUnitEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
MakeMemoryEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
MakeCacheEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
MakeTLBEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
MakeFPUEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
MakeIOEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

CRAT_HSA_PROCESSING_UNIT *
AddOneHsaEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
/*----------------------------------------------------------------------------
 *  All of the DATA should be defined in _CODE segment.
 *  Use ROMDATA to specify that it belongs to _CODE.
 *----------------------------------------------------------------------------
 */
STATIC CRAT_HEADER  ROMDATA  CratHeaderStruct =
{
  { 'C', 'R', 'A', 'T' },
  0,
  1,
  0,
  { 0 },
  { 0 },
  1,
  { 'H', 'Y', 'G', 'N' },
  1,
  0,
  0,
  { 0,   0,   0,   0, 0, 0}
};

/// All entries that CRAT needs
// It's better to add entries in the order of CRAT_ENTRY_TYPE
CONST S_MAKE_CRAT_ENTRY ROMDATA  MakeCratEntryTable[] =
{
  MakeHSAProcUnitEntry,
  MakeMemoryEntry,
  MakeCacheEntry,
  MakeTLBEntry,
  MakeFPUEntry,
  MakeIOEntry,
  NULL
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate CRAT
 *
 *  Description:
 *    This function will generate a complete Component Resource Affinity Table
 *    i.e. CRAT into a memory buffer.
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
HygonAcpiCrat (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       HYGON_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8                 i;
  UINT8                 *TableEnd;
  UINTN                 TableSize;
  UINTN                 TableKey;
  CRAT_HEADER           *CratHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS  AllocParams;

  // Allocate a buffer
  AllocParams.RequestedBufferSize = ACPI_TABLE_MAX_LENGTH;
  AllocParams.BufferHandle = HYGON_ACPI_TABLE_BUFFER_HANDLE;
  AllocParams.Persist = HEAP_SYSTEM_MEM;

  if (HeapAllocateBuffer (&AllocParams, StdHeader) != HGPI_SUCCESS) {
    return HGPI_ERROR;
  }

  CratHeaderStructPtr = (CRAT_HEADER *)AllocParams.BufferPtr;
  TableEnd = (UINT8 *)CratHeaderStructPtr + sizeof (CRAT_HEADER);

  // Copy CratHeaderStruct -> data buffer
  LibHygonMemCopy ((VOID *)CratHeaderStructPtr, (VOID *)&CratHeaderStruct, (UINTN)(sizeof (CRAT_HEADER)), StdHeader);
  // Update table OEM fields.
  ASSERT (AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiTableHeaderOemId), 100) <= 6);
  ASSERT (AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiCratTableHeaderOemTableId), 100) <= 8);

  LibHygonMemCopy (
    (VOID *)&CratHeaderStructPtr->OemId,
    (VOID *)PcdGetPtr (PcdHygonAcpiTableHeaderOemId),
    AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiTableHeaderOemId), 6),
    StdHeader
    );
  LibHygonMemCopy (
    (VOID *)&CratHeaderStructPtr->OemTableId,
    (VOID *)PcdGetPtr (PcdHygonAcpiCratTableHeaderOemTableId),
    AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiCratTableHeaderOemTableId), 8),
    StdHeader
    );

  // Add all CRAT entries.
  // It's better to add entries in the order of CRAT_ENTRY_TYPE
  for (i = 0; MakeCratEntryTable[i].MakeCratEntry != NULL; i++) {
    MakeCratEntryTable[i].MakeCratEntry (CratHeaderStructPtr, &TableEnd, StdHeader);
  }

  // Store size in table (current buffer offset - buffer start offset)
  CratHeaderStructPtr->Length = (UINT32)(TableEnd - (UINT8 *)CratHeaderStructPtr);

  // Boundary check
  ASSERT (CratHeaderStructPtr->Length <= ACPI_TABLE_MAX_LENGTH);
  if (CratHeaderStructPtr->Length > ACPI_TABLE_MAX_LENGTH) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: ACPI table buffer is overflow\n");
    IDS_DEADLOOP ()
  }

  // Update CRAT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *)CratHeaderStructPtr, StdHeader);

  // Publish CRAT
  TableSize = CratHeaderStructPtr->Length;
  TableKey  = 0;
  AcpiTableProtocol->InstallAcpiTable (
                       AcpiTableProtocol,
                       CratHeaderStructPtr,
                       TableSize,
                       &TableKey
                       );

  // Deallocate heap
  HeapDeallocateBuffer (HYGON_ACPI_TABLE_BUFFER_HANDLE, StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  CRAT is created\n");
  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/*---------------------------------------------------------------------------------------*/

/**
 * This function will add HSA Processing Unit entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeHSAProcUnitEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  EFI_STATUS                              CalledStatus;
  CRAT_HSA_PROCESSING_UNIT                *CratHsaEntry;
  HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL   *CcxAcpiCrat;
  HYGON_IGPU_ACPI_CRAT_SERVICES_PROTOCOL  *IGpuAcpiCrat;

  // Locate gHygonCcxAcpiCratServicesProtocol
  CalledStatus = gBS->LocateProtocol (&gHygonCcxAcpiCratServicesProtocolGuid, NULL, (VOID **)&CcxAcpiCrat);
  ASSERT (!EFI_ERROR (CalledStatus));
  if (CalledStatus != EFI_SUCCESS) {
    return;
  }

  CcxAcpiCrat->GetCratHsaProcCcxEntry (CcxAcpiCrat, CratHeaderStructPtr, TableEnd);

  // Locate gHygonIGpuAcpiCratServicesProtocol
  CalledStatus = gBS->LocateProtocol (&gHygonIGpuAcpiCratServicesProtocolGuid, NULL, (VOID **)&IGpuAcpiCrat);
  if (CalledStatus == EFI_SUCCESS) {
    CratHsaEntry = (CRAT_HSA_PROCESSING_UNIT *)AddOneHsaEntry (CratHeaderStructPtr, TableEnd);
    IGpuAcpiCrat->GetCratHsaProcIGpuEntry (IGpuAcpiCrat, CratHsaEntry);
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * This function will add memory entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeMemoryEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  HYGON_FABRIC_ACPI_CRAT_SERVICES_PROTOCOL  *CratMemory;

  if (gBS->LocateProtocol (&gHygonFabricAcpiCratServicesProtocolGuid, NULL, (VOID **)&CratMemory) == EFI_SUCCESS) {
    CratMemory->CreateMemory (CratMemory, CratHeaderStructPtr, TableEnd);
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * This function will add cache entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeCacheEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  EFI_STATUS                             CalledStatus;
  HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL  *CcxAcpiCrat;

  // Locate gHygonCcxAcpiCratServicesProtocol
  CalledStatus = gBS->LocateProtocol (&gHygonCcxAcpiCratServicesProtocolGuid, NULL, (VOID **)&CcxAcpiCrat);
  ASSERT (!EFI_ERROR (CalledStatus));
  if (CalledStatus != EFI_SUCCESS) {
    return;
  }

  CcxAcpiCrat->GetCratCacheEntry (CcxAcpiCrat, CratHeaderStructPtr, TableEnd);
}

/*---------------------------------------------------------------------------------------*/

/**
 * This function will add TLB entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeTLBEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
  EFI_STATUS                             CalledStatus;
  HYGON_CCX_ACPI_CRAT_SERVICES_PROTOCOL  *CcxAcpiCrat;

  // Locate gHygonCcxAcpiCratServicesProtocol
  CalledStatus = gBS->LocateProtocol (&gHygonCcxAcpiCratServicesProtocolGuid, NULL, (VOID **)&CcxAcpiCrat);
  ASSERT (!EFI_ERROR (CalledStatus));
  if (CalledStatus != EFI_SUCCESS) {
    return;
  }

  CcxAcpiCrat->GetCratTlbEntry (CcxAcpiCrat, CratHeaderStructPtr, TableEnd);
}

/*---------------------------------------------------------------------------------------*/

/**
 * This function will add FPU entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeFPUEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
}

/*---------------------------------------------------------------------------------------*/

/**
 * This function will add IO entry.
 *
 *    @param[in]      CratHeaderStructPtr  CRAT header pointer
 *    @param[in, out] TableEnd             The end of CRAT
 *    @param[in, out] StdHeader            Standard Head Pointer
 *
 */
VOID
MakeIOEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  )
{
}

/*---------------------------------------------------------------------------------------*/

/**
* This function will add one HSA entry.
*
*    @param[in]      CratHeaderStructPtr  CRAT header pointer
*    @param[in, out] TableEnd             The end of CRAT
*
*/
CRAT_HSA_PROCESSING_UNIT *
AddOneHsaEntry (
  IN       CRAT_HEADER       *CratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  CRAT_HSA_PROCESSING_UNIT  *CurrentEntry;

  CurrentEntry = (CRAT_HSA_PROCESSING_UNIT *)*TableEnd;
  // Update NumNodes & Total entries
  CratHeaderStructPtr->TotalEntries++;
  CratHeaderStructPtr->NumNodes++;
  // Update TableEnd
  *TableEnd += sizeof (CRAT_HSA_PROCESSING_UNIT);
  ASSERT ((*TableEnd - (UINT8 *)CratHeaderStructPtr) <= ACPI_TABLE_MAX_LENGTH);
  // Init Type and Length
  CurrentEntry->Type   = CRAT_HSA_PROC_UNIT_TYPE;
  CurrentEntry->Length = sizeof (CRAT_HSA_PROCESSING_UNIT);

  return CurrentEntry;
}
