/* $NoKeywords:$ */

/**
 * @file
 *
 * Generate Heterogeneous Memory Attribute Table (HMAT)
 *
 * Contains code that generate Heterogeneous Memory Attribute Table (HMAT)
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

#include <HGPI.h>
#include <Filecode.h>
#include "HygonAcpiDxe.h"
#include "HygonAcpiHmat.h"
#include <Library/HygonBaseLib.h>
#include <Library/HygonHeapLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Protocol/HygonAcpiHmatServicesProtocol.h>

#define FILECODE UNIVERSAL_ACPI_HYGONACPIHMAT_FILECODE

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

STATIC HMAT_HEADER ROMDATA HmatHeaderStruct =
{
  SIGNATURE_32 ('H', 'M', 'A', 'T'),                              // Signature
  0,                                                              // Length
  HMAT_REVISION,                                                  // Revision
  0,                                                              // Checksum
  {0},                                                            // OemId[6]
  0,                                                              // OemTableId
  1,                                                              // OemRevision
  SIGNATURE_32 ('H', 'G', 'N', ' '),                              // CreatorId
  1,                                                              // CreatorRevision
  {0}                                                             // Reserved
};

STATIC
VOID
HmatDump (
  IN       VOID                 *Hmat
  );
/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate HMAT
 *
 *  Description:
 *    This function will generate a complete Heterogeneous Memory Attribute Table
 *    i.e. HMAT into a memory buffer.
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
HygonAcpiHmat (
  IN       EFI_ACPI_TABLE_PROTOCOL    *AcpiTableProtocol,
  IN       HYGON_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8                                                    *TableEnd;
  UINTN                                                     TableSize;
  UINTN                                                     TableKey;
  EFI_STATUS                                                Status;
  HMAT_HEADER                                              *HmatHeaderPtr;
  ALLOCATE_HEAP_PARAMS                                      AllocParams;
  HYGON_ACPI_HMAT_SERVICES_PROTOCOL                        *HmatServices;

  IDS_HDT_CONSOLE (MAIN_FLOW, "  HygonAcpiHmat entry \n");

  Status = gBS->LocateProtocol (&gHygonAcpiHmatServicesProtocolGuid, NULL, (VOID **) &HmatServices);
  if (EFI_ERROR (Status)) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Not found gHygonAcpiHmatServicesProtocolGuid \n");
    return HGPI_ERROR;
  }
  // Allocate a buffer
  AllocParams.RequestedBufferSize = ACPI_TABLE_MAX_LENGTH;
  AllocParams.BufferHandle = HYGON_ACPI_TABLE_BUFFER_HANDLE;
  AllocParams.Persist = HEAP_SYSTEM_MEM;

  if (HeapAllocateBuffer (&AllocParams, StdHeader) != HGPI_SUCCESS) {
    return HGPI_ERROR;
  }

  HmatHeaderPtr = (HMAT_HEADER *) AllocParams.BufferPtr;
  TableEnd = (UINT8 *) HmatHeaderPtr + sizeof (HMAT_HEADER);

  // Copy HmatHeaderStruct -> data buffer
  LibHygonMemCopy ((VOID *) HmatHeaderPtr, (VOID *) &HmatHeaderStruct, sizeof (HmatHeaderStruct), StdHeader);
  // Update table OEM fields.
  ASSERT (AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdHygonAcpiTableHeaderOemId), 100) <= 6);
  ASSERT (AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdHygonAcpiHmatTableHeaderOemTableId), 100) <= 8);

  LibHygonMemCopy ((VOID *) &HmatHeaderPtr->OemId,
                 (VOID *) PcdGetPtr (PcdHygonAcpiTableHeaderOemId),
                 AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdHygonAcpiTableHeaderOemId), 6),
                 StdHeader);
  LibHygonMemCopy ((VOID *) &HmatHeaderPtr->OemTableId,
                 (VOID *) PcdGetPtr (PcdHygonAcpiHmatTableHeaderOemTableId),
                 AsciiStrnLenS ((CHAR8 *)PcdGetPtr (PcdHygonAcpiHmatTableHeaderOemTableId), 8),
                 StdHeader);

  // Add all HMAT entries.
  Status = HmatServices->CreateStructure (HmatServices, HmatHeaderPtr, &TableEnd);

  if (EFI_ERROR (Status)) {
    // Deallocate heap
    IDS_HDT_CONSOLE (MAIN_FLOW, "  HMAT is NOT created since Status is %r\n", Status);
    HeapDeallocateBuffer (HYGON_ACPI_TABLE_BUFFER_HANDLE, StdHeader);
    return HGPI_UNSUPPORTED;
  }

  // Store size in table (current buffer offset - buffer start offset)
  HmatHeaderPtr->Length = (UINT32) (TableEnd - (UINT8 *) HmatHeaderPtr);

  // Boundary check
  ASSERT (HmatHeaderPtr->Length <= ACPI_TABLE_MAX_LENGTH);
  if (HmatHeaderPtr->Length > ACPI_TABLE_MAX_LENGTH) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: ACPI table buffer is overflow\n");
    IDS_DEADLOOP ()
  }

  // Update HMAT header Checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) HmatHeaderPtr, StdHeader);

  // Publish HMAT
  TableSize = HmatHeaderPtr->Length;
  TableKey = 0;
  AcpiTableProtocol->InstallAcpiTable (
                  AcpiTableProtocol,
                  HmatHeaderPtr,
                  TableSize,
                  &TableKey
                  );

  DEBUG_CODE (
    HmatDump (HmatHeaderPtr);
  )

  // Deallocate heap
  HeapDeallocateBuffer (HYGON_ACPI_TABLE_BUFFER_HANDLE, StdHeader);

  // Free the HMAT Structure Buffer
  HmatServices->FreeBuffer (HmatServices);

  IDS_HDT_CONSOLE (MAIN_FLOW, "  HMAT is created\n");
  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
/**
 * Dump HMAT
 *
 * @param[in]     Hmat            Pointer to HMAT
 */
STATIC
VOID
HmatDump (
  IN       VOID     *Hmat
  )
{
  UINT8   *Block;
  UINT8   *BufferPtr;
  UINT32   Index, i, j;
  UINT32   NumberOfInitiatorProximityDomains;
  UINT32   NumberOfTargetProximityDomains;

  Block = (UINT8 *) Hmat + sizeof (HMAT_HEADER);
  IDS_HDT_CONSOLE (MAIN_FLOW, "<----------  HMAT Table Start -----------> \n");
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Table Length         = 0x%08x\n", ((HMAT_HEADER *) Hmat)-> Length);
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Revision             = 0x%04x\n", ((HMAT_HEADER *) Hmat)-> Revision);
  IDS_HDT_CONSOLE (MAIN_FLOW, "  Checksum             = 0x%02x\n", ((HMAT_HEADER *) Hmat)-> Checksum);

  while (Block < ((UINT8 *) Hmat + ((HMAT_HEADER *) Hmat)->Length)) {
    if (*Block == HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_TYPE) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  <-------------HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES Start -------->\n");
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Type          = 0x%02x\n", ((HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE *) Block)->Type);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Length        = 0x%08x\n", ((HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE *) Block)->Length);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Flags         = 0x%04x\n", ((HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE *) Block)->Flags.Value);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  InitiatorProximityDomain = %d \n", ((HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE *) Block)->InitiatorProximityDomain);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  MemoryProximityDomain    = %d \n", ((HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE *) Block)->MemoryProximityDomain);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  <-------------HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES End ---------->\n");
      Block = Block + ((HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE *) Block)->Length;
      
    } else if (*Block == HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_TYPE) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  <-------------HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH Entry Start -------->\n");
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Type            = 0x%02x\n", ((HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) Block)->Type);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Length          = 0x%04x\n", ((HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) Block)->Length);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Flags           = 0x%02x\n", ((HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) Block)->Flags.Value);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  DataType        = 0x%02x\n", ((HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) Block)->DataType);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  MinTransferSize = 0x%02x\n", ((HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) Block)->MinTransferSize);
      NumberOfInitiatorProximityDomains = ((HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) Block)->NumberOfInitiatorProximityDomains;
      NumberOfTargetProximityDomains = ((HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) Block)->NumberOfTargetProximityDomains;
      IDS_HDT_CONSOLE (MAIN_FLOW, "  NumberOfInitiatorProximityDomains = %d \n", NumberOfInitiatorProximityDomains);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  NumberOfTargetProximityDomains = %d \n", NumberOfTargetProximityDomains);
      IDS_HDT_CONSOLE (MAIN_FLOW, "  EntryBaseUnit  = %ld \n", ((HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) Block)->EntryBaseUnit);
      
      BufferPtr = Block + 32;
      IDS_HDT_CONSOLE (MAIN_FLOW, "  InitiatorProximityDomains List : ");
      for (Index = 0; Index < NumberOfInitiatorProximityDomains; Index++) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "%d, " , *(UINT32 *)BufferPtr);
        BufferPtr = BufferPtr + 4;
      }
      IDS_HDT_CONSOLE (MAIN_FLOW, "\n");

      IDS_HDT_CONSOLE (MAIN_FLOW, "  TargetProximityDomains List : ");
      for (Index = 0; Index < NumberOfTargetProximityDomains; Index++) {
        IDS_HDT_CONSOLE (MAIN_FLOW, "%d, " , *(UINT32 *)BufferPtr);
        BufferPtr = BufferPtr + 4;
      }
      IDS_HDT_CONSOLE (MAIN_FLOW, "\n");

      for (i = 0; i < NumberOfInitiatorProximityDomains; i++) {
        for (j = 0; j < NumberOfTargetProximityDomains; j++) {
          IDS_HDT_CONSOLE (MAIN_FLOW, "    Entry[%d][%d] = %d \n", i, j, *(UINT16 *)BufferPtr);
          BufferPtr = BufferPtr + 2;
        }
      }

      IDS_HDT_CONSOLE (MAIN_FLOW, "  <-------------HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH Entry End ---------->\n");
      Block = Block + ((HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) Block)->Length;
    }
  }
}