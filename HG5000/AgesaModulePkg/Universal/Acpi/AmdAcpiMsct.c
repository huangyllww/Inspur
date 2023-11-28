/* $NoKeywords:$ */
/**
 * @file
 *
 * Generate Maximum System Characteristic Tables (MSCT)
 *
 * Contains code that generate Maximum System Characteristic Table (MSCT)
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
#include "AmdAcpiMsct.h"
#include "AmdAcpiDxe.h"
#include <Library/BaseLib.h>
#include <Library/CcxBaseX86Lib.h>
#include <Library/AmdBaseLib.h>
#include <Library/AmdHeapLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/FabricNumaServicesProtocol.h>
#include <Protocol/CcxBaseServicesProtocol.h>
#include <Protocol/AmdAcpiMsctServicesProtocol.h>

#define FILECODE UNIVERSAL_ACPI_AMDACPIMSCT_FILECODE

STATIC MSCT_HEADER ROMDATA MsctHeaderStruct =
{
  {'M', 'S', 'C', 'T'},       // Signature
  0,                          // Length
  1,                          // Revision
  0,                          // Checksum
  {0},                        // OemId
  {0},                        // OemTableId
  0,                          // OemRevision
  {'H', 'G', 'N', ' '},       // CreatorId
  1,                          // CreatorRev
  0,                          // OffsetProxDomInfo
  0,                          // MaxProxDom
  0,                          // MaxClkDom
  0                           // MaxPhysAddr
};


VOID
STATIC
CreateMsctEntry (
  IN       UINT32 ProxDomainLo,
  IN       UINT32 ProxDomainHi,
  IN       UINT32 MaxProcCap,
  IN       UINT64 MaxMemCap,
  IN OUT   UINT8  **EndOfTable
  );

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/**
 *---------------------------------------------------------------------------------------
 *
 *  Generate MSCT
 *
 *  Description:
 *    This function will generate a complete Maximum System Characteristic Table
 *    i.e. MSCT into a memory buffer.
 *
 *  Parameters:
 *    @param[in]       AcpiTableProtocol          Pointer to gEfiAcpiTableProtocolGuid protocol
 *    @param[in]       StdHeader                  Header for library and services
 *
 *    @retval          AGESA_ERROR                Heap allocation failed, or NUMA services protocol not
 *                                                available
 *                     AGESA_SUCCESS              MSCT successfully generated
 *
 *---------------------------------------------------------------------------------------
 */
AGESA_STATUS
AmdAcpiMsct (
  IN       EFI_ACPI_TABLE_PROTOCOL  *AcpiTableProtocol,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8                                   *EndOfTable;
  UINTN                                   TableSize;
  UINTN                                   TableKey;
  UINT32                                  i;
  UINT32                                  NumPopProxDomains;
  UINT32                                  MaxNumProxDomains;
  UINT32                                  PrevDomain;
  UINT32                                  ProxDomainLo;
  UINT32                                  PrevDomainMaxProcCap;
  UINT64                                  PrevDomainMaxMemCap;
  EFI_STATUS                              CalledStatus;
  AGESA_STATUS                            Status;
  MSCT_HEADER                             *MsctHeaderStructPtr;
  ALLOCATE_HEAP_PARAMS                    AllocParams;
  FABRIC_NUMA_SERVICES_PROTOCOL           *FabricNumaServices;
  AMD_FABRIC_ACPI_MSCT_SERVICES_PROTOCOL  *FabricMsctServices;
  CCX_BASE_SERVICES_PROTOCOL              *CcxBaseServices;
  MSCT_PROX_DOMAIN_INFO                   *MsctInfo;

  // Locate Fabric NUMA services protocol
  if (gBS->LocateProtocol (&gAmdFabricNumaServicesProtocolGuid, NULL, (VOID **) &FabricNumaServices) != EFI_SUCCESS) {
    return AGESA_ERROR;
  }

  // Locate Fabric MSCT services protocol
  if (gBS->LocateProtocol (&gAmdFabricAcpiMsctServicesProtocolGuid, NULL, (VOID **) &FabricMsctServices) != EFI_SUCCESS) {
    return AGESA_ERROR;
  }

  // Locate CCX Base services protocol
  if (gBS->LocateProtocol (&gAmdCcxBaseServicesProtocolGuid, NULL, (VOID **) &CcxBaseServices) != EFI_SUCCESS) {
    return AGESA_ERROR;
  }

  // Allocate space for the table
  AllocParams.RequestedBufferSize = ACPI_TABLE_MAX_LENGTH;
  AllocParams.BufferHandle        = AMD_ACPI_TABLE_BUFFER_HANDLE;
  AllocParams.Persist             = HEAP_SYSTEM_MEM;

  if (HeapAllocateBuffer (&AllocParams, StdHeader) != AGESA_SUCCESS) {
    return AGESA_ERROR;
  }

  MsctHeaderStructPtr = (MSCT_HEADER *) AllocParams.BufferPtr;
  EndOfTable = (UINT8 *) MsctHeaderStructPtr + sizeof (MSCT_HEADER);

  LibAmdMemCopy ((VOID *) MsctHeaderStructPtr, (VOID *) &MsctHeaderStruct, (UINTN) (sizeof (MSCT_HEADER)), StdHeader);

  ASSERT (AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiMsctTableHeaderOemTableId), 100) <= 8);

  LibAmdMemCopy ((VOID *) &MsctHeaderStructPtr->OemId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiTableHeaderOemId),
                 AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiTableHeaderOemId), 6),
                 StdHeader);
  LibAmdMemCopy ((VOID *) &MsctHeaderStructPtr->OemTableId,
                 (VOID *) PcdGetPtr (PcdAmdAcpiMsctTableHeaderOemTableId),
                 AsciiStrnLenS (PcdGetPtr (PcdAmdAcpiMsctTableHeaderOemTableId), 8),
                 StdHeader);

  MsctHeaderStructPtr->OffsetProxDomInfo = (UINT32) (EndOfTable - (UINT8 *) MsctHeaderStructPtr);
  MsctHeaderStructPtr->MaxProxDom = (UINT32) (FabricNumaServices->GetMaxDomains (FabricNumaServices) - 1);
  MsctHeaderStructPtr->MaxClkDom = 0;
  MsctHeaderStructPtr->MaxPhysAddr =  ((UINT64) 1 << CcxBaseServices->CcxGetMaxPhysAddr (CcxBaseServices)) - 1;

  CalledStatus = FabricMsctServices->GetMsctInfo (FabricMsctServices, &NumPopProxDomains, &MaxNumProxDomains, &MsctInfo);

  ASSERT (CalledStatus == EFI_SUCCESS);

  PrevDomain = 0;
  ProxDomainLo = 0;
  PrevDomainMaxProcCap = MsctInfo[0].MaxProcCap;
  PrevDomainMaxMemCap = MsctInfo[0].MaxMemCap;

  // If contiguous domains in the MSCT contain the same amount of processors and memory,
  // we can report the domains as a range with a single structure.
  for (i = 0; i < NumPopProxDomains; i++) {
    PrevDomain = i - 1;
    // If MaxProcCap and MaxMemCap of current domain do not match previous domain, we add an entry to the MSCT table
    if ((PrevDomainMaxProcCap != MsctInfo[i].MaxProcCap) || (PrevDomainMaxMemCap != MsctInfo[i].MaxMemCap)) {
      CreateMsctEntry (ProxDomainLo, PrevDomain, MsctInfo[PrevDomain].MaxProcCap, MsctInfo[PrevDomain].MaxMemCap, &EndOfTable);
      ProxDomainLo = i;
      PrevDomainMaxProcCap = MsctInfo[i].MaxProcCap;
      PrevDomainMaxMemCap = MsctInfo[i].MaxMemCap;
    }
  }

  CreateMsctEntry (ProxDomainLo, NumPopProxDomains - 1, MsctInfo[NumPopProxDomains - 1].MaxProcCap, MsctInfo[NumPopProxDomains - 1].MaxMemCap, &EndOfTable);

  // Add range for any remaining unused domains.
  if (NumPopProxDomains < MaxNumProxDomains) {
    CreateMsctEntry (NumPopProxDomains, MaxNumProxDomains - 1, 0, 0, &EndOfTable);
  }

  MsctHeaderStructPtr->Length = (UINT32) (EndOfTable - (UINT8 *) MsctHeaderStructPtr);

  // Boundary check
  ASSERT (MsctHeaderStructPtr->Length <= ACPI_TABLE_MAX_LENGTH);
  if (MsctHeaderStructPtr->Length > ACPI_TABLE_MAX_LENGTH) {
    IDS_HDT_CONSOLE (CPU_TRACE, "ERROR: ACPI table buffer overflowed\n");
    IDS_DEADLOOP ();
  }

  // Update checksum
  ChecksumAcpiTable ((ACPI_TABLE_HEADER *) MsctHeaderStructPtr, StdHeader);

  // Publish MSCT
  TableSize = MsctHeaderStructPtr->Length;
  TableKey = 0;
  CalledStatus = AcpiTableProtocol->InstallAcpiTable (
                             AcpiTableProtocol,
                             MsctHeaderStructPtr,
                             TableSize,
                             &TableKey
                             );
  if (CalledStatus == EFI_SUCCESS) {
    Status = AGESA_SUCCESS;
    IDS_HDT_CONSOLE (MAIN_FLOW, "  MSCT is created\n");
  } else {
    Status = AGESA_ERROR;
    IDS_HDT_CONSOLE (MAIN_FLOW, "  MSCT was not created\n");
  }

  // Deallocate heap
  HeapDeallocateBuffer (AMD_ACPI_TABLE_BUFFER_HANDLE, StdHeader);

  return Status;
}


/**
 *---------------------------------------------------------------------------------------
 *
 *  CreateMsctEntry
 *
 *  Description:
 *    Create the maximum proximity domain structure
 *
 *  Parameters:
 *    @param[in]      ProxDomainLo               The lowest proximity domain number in the range
 *    @param[in]      ProxDomainHi               The highest proximity domain number in the range
 *    @param[in]      MaxProcCap                 Maximum (actual) processor capacity for each proximity domain
 *    @param[in]      MaxMemCap                  Maximum (actual) memory capacity for each proximity domain
 *    @param[in,out]  EndOfTable                 Pointer to the end of the MSCT table
 *
 *
 *---------------------------------------------------------------------------------------
 */
VOID
STATIC
CreateMsctEntry (
  IN       UINT32 ProxDomainLo,
  IN       UINT32 ProxDomainHi,
  IN       UINT32 MaxProcCap,
  IN       UINT64 MaxMemCap,
  IN OUT   UINT8  **EndOfTable
  )
{

  MSCT_PROX_DOMAIN_INFO_STRUCT  *MaxProxDomStruct;

  MaxProxDomStruct = (MSCT_PROX_DOMAIN_INFO_STRUCT *) *EndOfTable;
  MaxProxDomStruct->Revision = 1;
  MaxProxDomStruct->Length = 22;
  MaxProxDomStruct->MaxProcCap = MaxProcCap;
  MaxProxDomStruct->MaxMemCap = MaxMemCap;
  MaxProxDomStruct->ProximityLow = ProxDomainLo;
  MaxProxDomStruct->ProximityHigh = ProxDomainHi;
  *EndOfTable += sizeof (MSCT_PROX_DOMAIN_INFO_STRUCT);
}