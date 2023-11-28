/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate System Resource Affinity Table (SRAT)
 *
 * Contains code that generate System Resource Affinity Table (SRAT)
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
#include "AmdAcpiSrat.h"
#include "Library/AmdBaseLib.h"
#include "Library/AmdHeapLib.h"
#include "Library/UefiBootServicesTableLib.h"
#include "Library/BaseLib.h"
#include "Protocol/AmdAcpiSratServicesProtocol.h"
#include "PiDxe.h"

#define FILECODE UNIVERSAL_ACPI_AMDACPISRAT_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
EFI_STATUS
MakeProcLocalApicStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );

EFI_STATUS
MakeMemoryStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );

EFI_STATUS
MakeProcX2LocalApicStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  );

EFI_STATUS
MakeGiccStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
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

// Version 3 (ACPI 6.0)
STATIC SRAT_HEADER  ROMDATA SratHeaderStruct =
{
  {'S','R','A','T'},
  0,
  3,
  0,
  {0},
  {0},
  1,
  {'H','Y','G','N'},
  1,
  1,
  {0}
};

/// All entries that SRAT needs
CONST S_MAKE_SRAT_STRUCT ROMDATA MakeSratStructTable[] =
{
  MakeProcLocalApicStruct,
  MakeMemoryStruct,
  MakeProcX2LocalApicStruct,
  MakeGiccStruct,
  NULL
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate SRAT
 *
 *  Description:
 *    This function will generate a complete System Resource Affinity Table
 *    i.e. SRAT into a memory buffer.
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
AmdAcpiSrat (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8                i;
  UINT8               *TableEnd;
  UINTN                TableSize;
  UINTN                TableKey;
  EFI_STATUS           Status;
  SRAT_HEADER         *SratHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS AllocParams;

  // Allocate a buffer
  AllocParams.RequestedBufferSize = ACPI_TABLE_MAX_LENGTH;
  AllocParams.BufferHandle = AMD_ACPI_TABLE_BUFFER_HANDLE;
  AllocParams.Persist = HEAP_SYSTEM_MEM;


  if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }

  SratHeaderStructPtr = (SRAT_HEADER *) AllocParams.BufferPtr;
  TableEnd = (UINT8 *) SratHeaderStructPtr + sizeof (SRAT_HEADER);

  // Copy SratHeaderStruct -> data buffer
  LibAmdMemCopy ((VOID *) SratHeaderStructPtr, (VOID *) &SratHeaderStruct, (UINTN) (sizeof (SRAT_HEADER)), StdHeader);
  // Update table OEM fields.
  ASSERT (AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 100) <= 6);
  ASSERT (AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiSratTableHeaderOemTableId), 100) <= 8);

  LibAmdMemCopy ((VOID *) &SratHeaderStructPtr->OemId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiTableHeaderOemId),
                 AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 6),
                 StdHeader);
  LibAmdMemCopy ((VOID *) &SratHeaderStructPtr->OemTableId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiSratTableHeaderOemTableId),
                 AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiSratTableHeaderOemTableId), 8),
                 StdHeader);

  // Add all SRAT entries.
  for (i = 0; MakeSratStructTable[i].MakeSratStruct != NULL; i++) {
    Status = MakeSratStructTable[i].MakeSratStruct (SratHeaderStructPtr, &TableEnd);
    if (Status == EFI_ABORTED) {
      // Deallocate heap
      IDS_HDT_CONSOLE (MAIN_FLOW, "  SRAT is NOT created since MakeSratStructTable[%d] returns EFI_ABORTED\n", i);
      HeapDeallocateBuffer (AMD_ACPI_TABLE_BUFFER_HANDLE, StdHeader);
      return AGESA_UNSUPPORTED;
    }
  }

  // Store size in table (current buffer offset - buffer start offset)
  SratHeaderStructPtr->Length = (UINT32) (TableEnd - (UINT8 *) SratHeaderStructPtr);

  // Boundary check
  ASSERT (SratHeaderStructPtr->Length <= ACPI_TABLE_MAX_LENGTH);
  if (SratHeaderStructPtr->Length > ACPI_TABLE_MAX_LENGTH) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: ACPI table buffer is overflow\n");
    IDS_DEADLOOP ()
  }

  // Update SRAT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) SratHeaderStructPtr, StdHeader);

  // Publish SRAT
  TableSize = SratHeaderStructPtr->Length;
  TableKey = 0;
  AcpiTableProtocol->InstallAcpiTable (
                  AcpiTableProtocol,
                  SratHeaderStructPtr,
                  TableSize,
                  &TableKey
                  );

  // Deallocate heap
  HeapDeallocateBuffer (AMD_ACPI_TABLE_BUFFER_HANDLE, StdHeader);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  SRAT is created\n");
  return AGESA_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
MakeProcLocalApicStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  EFI_STATUS Status;
  AMD_CCX_ACPI_SRAT_SERVICES_PROTOCOL *SratLocalApic;

  Status = EFI_ABORTED;
  if (gBS->LocateProtocol (&gAmdCcxAcpiSratServicesProtocolGuid, NULL, (VOID **) &SratLocalApic) == EFI_SUCCESS) {
    Status = SratLocalApic->CreateLapic (SratLocalApic, SratHeaderStructPtr, TableEnd);
  }

  return Status;
}

EFI_STATUS
MakeMemoryStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  EFI_STATUS Status;
  AMD_FABRIC_ACPI_SRAT_SERVICES_PROTOCOL *SratMemory;

  Status = EFI_ABORTED;
  if (gBS->LocateProtocol (&gAmdFabricAcpiSratServicesProtocolGuid, NULL, (VOID **) &SratMemory) == EFI_SUCCESS) {
    Status = SratMemory->CreateMemory (SratMemory, SratHeaderStructPtr, TableEnd);
  }

  return Status;
}

EFI_STATUS
MakeProcX2LocalApicStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  EFI_STATUS Status;
  AMD_CCX_ACPI_SRAT_SERVICES_PROTOCOL *SratX2Apic;

  Status = EFI_ABORTED;
  if (gBS->LocateProtocol (&gAmdCcxAcpiSratServicesProtocolGuid, NULL, (VOID **) &SratX2Apic) == EFI_SUCCESS) {
    Status = SratX2Apic->CreateX2Apic (SratX2Apic, SratHeaderStructPtr, TableEnd);
  }

  return Status;
}

EFI_STATUS
MakeGiccStruct (
  IN       SRAT_HEADER       *SratHeaderStructPtr,
  IN OUT   UINT8            **TableEnd
  )
{
  return EFI_SUCCESS;
}

