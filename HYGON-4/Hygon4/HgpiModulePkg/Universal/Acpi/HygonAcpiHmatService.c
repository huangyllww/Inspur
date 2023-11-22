/* $NoKeywords:$ */

/**
 * @file
 *
 * Provide the service to generate Heterogeneous Memory Attribute Table (HMAT)
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
 
#include <Uefi.h>
#include <Filecode.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/HygonAcpiHmatServicesProtocol.h>

#define FILECODE UNIVERSAL_ACPI_HYGONACPIHMATSERVICE_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
CreateStructure (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL      *This,
  IN     VOID                                   *TableHeaderPtr,
  IN OUT UINT8                                 **TableEnd
  );

EFI_STATUS
EFIAPI
FreeBuffer (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL        *This
  );

EFI_STATUS
EFIAPI
AddMemProximityDomainAttributes (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL              *This,
  IN     HMAT_MEM_PROXIMITY_DOMAIN_ATTRIBUTES_FLAGS      Flags,
  IN     UINT32                                          InitiatorProximityDomain,
  IN     UINT32                                          MemoryProximityDomain
  );

EFI_STATUS
EFIAPI
AddSysLocalityLatencyBandwidthInfo (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL              *This,
  IN     HMAT_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO_FLAGS  Flags,
  IN     UINT8                                           DataType,
  IN     UINT8                                           MinTransferSize,
  IN     UINT32                                          NumberOfInitiatorProximityDomains,
  IN     UINT32                                          NumberOfTargetProximityDomains,
  IN     UINT64                                          EntryBaseUnit,
  IN     UINT32                                         *InitiatorProximityDomains,
  IN     UINT32                                         *TargetProximityDomains,
  IN     UINT16                                         *LatencyBandwidthEntries
  );

EFI_STATUS
EFIAPI
AddMemSideCacheInfo (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL         *This,
  IN     UINT32                                     MemoryProximityDomain,
  IN     UINT64                                     MemorySideCacheSize,
  IN     HMAT_MEM_SIDE_CACHE_ATTRIBUTES             CacheAttributes,
  IN     UINT16                                     NumberOfSmbiosHandles,
  IN     UINT16                                    *SmbiosHandles
  );

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

typedef struct {
  UINTN                                Signature;
  LIST_ENTRY                           Link;
  HMAT_STRUCTURE_HEADER                StructureHeader;
} HMAT_STRUCTURE_RAW;

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define HMAT_STRUCTURE_PRIVATE_DATA_SIGNATURE   SIGNATURE_32 ('$', 'H', 'M', 'A')

LIST_ENTRY mHmatEntries;

HYGON_ACPI_HMAT_SERVICES_PROTOCOL mHygonAcpiHmatServices = {
  HYGON_ACPI_HMAT_SERVICES_PROTOCOL_REVISION,
  CreateStructure,
  FreeBuffer,
  AddMemProximityDomainAttributes,
  AddSysLocalityLatencyBandwidthInfo,
  AddMemSideCacheInfo
};

/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
HygonAcpiHmatServiceEntryPoint (
  IN       EFI_HANDLE         ImageHandle,
  IN       EFI_SYSTEM_TABLE   *SystemTable
  )
{
  InitializeListHead (&mHmatEntries);

  return gBS->InstallProtocolInterface (
                &ImageHandle,
                &gHygonAcpiHmatServicesProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &mHygonAcpiHmatServices
                );
}

/*----------------------------------------------------------------------------------------
 *                          L O C A L    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

EFI_STATUS
EFIAPI
CreateStructure (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL      *This,
  IN     VOID                                   *TableHeaderPtr,
  IN OUT UINT8                                 **TableEnd
  )
{
  HMAT_STRUCTURE_RAW *StructBuffer;
  LIST_ENTRY         *Node;
  UINTN              Count;

  if (IsListEmpty (&mHmatEntries)) {
    DEBUG ((DEBUG_INFO, "mHmatEntries is empty \n"));
    return EFI_ABORTED;
  }

  Count = 0;
  for (Node = GetFirstNode (&mHmatEntries); !IsNull (&mHmatEntries, Node); Node = GetNextNode (&mHmatEntries, Node)) {
    StructBuffer = CR (Node, HMAT_STRUCTURE_RAW, Link, HMAT_STRUCTURE_PRIVATE_DATA_SIGNATURE);
    CopyMem (*TableEnd, &StructBuffer->StructureHeader, StructBuffer->StructureHeader.Length);
    *TableEnd += StructBuffer->StructureHeader.Length;
    Count++;
  }
  DEBUG ((DEBUG_INFO, "HMAT structure count = %d\n", Count));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
FreeBuffer (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL        *This
  )
{
  HMAT_STRUCTURE_RAW *StructBuffer;
  while (!IsListEmpty (&mHmatEntries)) {
    StructBuffer = CR (mHmatEntries.ForwardLink, HMAT_STRUCTURE_RAW, Link, HMAT_STRUCTURE_PRIVATE_DATA_SIGNATURE);
    RemoveEntryList (mHmatEntries.ForwardLink);
    FreePool (StructBuffer);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AddMemProximityDomainAttributes (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL              *This,
  IN     HMAT_MEM_PROXIMITY_DOMAIN_ATTRIBUTES_FLAGS      Flags,
  IN     UINT32                                          InitiatorProximityDomain,
  IN     UINT32                                          MemoryProximityDomain
  )
{
  HMAT_STRUCTURE_RAW                                *StructBuffer;
  HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE *StructBodyPtr;

  StructBuffer = AllocateZeroPool (OFFSET_OF (HMAT_STRUCTURE_RAW, StructureHeader) + sizeof (HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE));
  if (StructBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StructBuffer->Signature = HMAT_STRUCTURE_PRIVATE_DATA_SIGNATURE;

  StructBodyPtr = (HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE *) (((UINT8 *) StructBuffer) + OFFSET_OF (HMAT_STRUCTURE_RAW, StructureHeader));
  StructBodyPtr->Type = HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_TYPE;
  StructBodyPtr->Length = sizeof (HMAT_MEMORY_PROXIMITY_DOMAIN_ATTRIBUTES_STRUCTURE);
  StructBodyPtr->Flags.Value = Flags.Value;
  StructBodyPtr->InitiatorProximityDomain = InitiatorProximityDomain;
  StructBodyPtr->MemoryProximityDomain = MemoryProximityDomain;

  InsertTailList (&mHmatEntries, &StructBuffer->Link);
  DEBUG ((DEBUG_INFO, "Add HMAT Memory Proximity Domain Attributes Structure. Size is 0x%x\n", StructBodyPtr->Length));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AddSysLocalityLatencyBandwidthInfo (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL              *This,
  IN     HMAT_SYS_LOCALITY_LATENCY_BANDWIDTH_INFO_FLAGS  Flags,
  IN     UINT8                                           DataType,
  IN     UINT8                                           MinTransferSize,
  IN     UINT32                                          NumberOfInitiatorProximityDomains,
  IN     UINT32                                          NumberOfTargetProximityDomains,
  IN     UINT64                                          EntryBaseUnit,
  IN     UINT32                                         *InitiatorProximityDomains,
  IN     UINT32                                         *TargetProximityDomains,
  IN     UINT16                                         *LatencyBandwidthEntries
  )
{
  HMAT_STRUCTURE_RAW                                        *StructBuffer;
  HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *StructBodyPtr;
  UINTN                                                      BodySize;
  UINT8                                                     *BufferPtr;
  UINTN                                                      CopySize;

  if (NumberOfInitiatorProximityDomains != 0 && InitiatorProximityDomains == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfTargetProximityDomains != 0 && TargetProximityDomains == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (NumberOfInitiatorProximityDomains * NumberOfTargetProximityDomains != 0 && LatencyBandwidthEntries == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (EntryBaseUnit == 0) {
    return EFI_INVALID_PARAMETER;
  }

  BodySize = sizeof (HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE) +
               NumberOfInitiatorProximityDomains * sizeof (UINT32) +
               NumberOfTargetProximityDomains * sizeof (UINT32) +
               NumberOfInitiatorProximityDomains * NumberOfTargetProximityDomains * sizeof (UINT16);


  StructBuffer = AllocateZeroPool (OFFSET_OF (HMAT_STRUCTURE_RAW, StructureHeader) + BodySize);
  if (StructBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StructBuffer->Signature = HMAT_STRUCTURE_PRIVATE_DATA_SIGNATURE;

  StructBodyPtr = (HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_STRUCTURE *) (((UINT8 *) StructBuffer) + OFFSET_OF (HMAT_STRUCTURE_RAW, StructureHeader));
  StructBodyPtr->Type = HMAT_SYSTEM_LOCALITY_LATENCY_AND_BANDWIDTH_INFO_TYPE;
  StructBodyPtr->Length = (UINT32) BodySize;
  StructBodyPtr->Flags.Value = Flags.Value;
  StructBodyPtr->DataType = DataType;
  StructBodyPtr->MinTransferSize = MinTransferSize;
  StructBodyPtr->NumberOfInitiatorProximityDomains = NumberOfInitiatorProximityDomains;
  StructBodyPtr->NumberOfTargetProximityDomains = NumberOfTargetProximityDomains;
  StructBodyPtr->EntryBaseUnit = EntryBaseUnit;

  BufferPtr = (UINT8 *) (StructBodyPtr + 1);
  CopySize = sizeof (UINT32) * NumberOfInitiatorProximityDomains;
  if (InitiatorProximityDomains != NULL) {
    CopyMem (BufferPtr, InitiatorProximityDomains, CopySize);
  }

  BufferPtr = BufferPtr + CopySize;
  CopySize = sizeof (UINT32) * NumberOfTargetProximityDomains;
  if (TargetProximityDomains != NULL) {
    CopyMem (BufferPtr, TargetProximityDomains, CopySize);
  }

  BufferPtr = BufferPtr + CopySize;
  CopySize = sizeof (UINT16) * NumberOfInitiatorProximityDomains * NumberOfTargetProximityDomains;
  if (LatencyBandwidthEntries != NULL) {
    CopyMem (BufferPtr, LatencyBandwidthEntries, CopySize);
  }

  InsertTailList (&mHmatEntries, &StructBuffer->Link);
  DEBUG ((DEBUG_INFO, "Add HMAT System Locality Latency and Bandwidth Information Structure. Size is 0x%x\n", StructBodyPtr->Length));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
AddMemSideCacheInfo (
  IN     HYGON_ACPI_HMAT_SERVICES_PROTOCOL          *This,
  IN     UINT32                                     MemoryProximityDomain,
  IN     UINT64                                     MemorySideCacheSize,
  IN     HMAT_MEM_SIDE_CACHE_ATTRIBUTES             CacheAttributes,
  IN     UINT16                                     NumberOfSmbiosHandles,
  IN     UINT16                                    *SmbiosHandles
  )
{
  HMAT_STRUCTURE_RAW                    *StructBuffer;
  HMAT_MEMORY_SIDE_CACHE_INFO_STRUCTURE *StructBodyPtr;
  UINTN                                  BodySize;
  UINT8                                 *BufferPtr;
  UINTN                                  CopySize;

  if (NumberOfSmbiosHandles != 0 && SmbiosHandles == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BodySize = sizeof (HMAT_MEMORY_SIDE_CACHE_INFO_STRUCTURE) + NumberOfSmbiosHandles * sizeof (UINT16);

  StructBuffer = AllocateZeroPool (OFFSET_OF (HMAT_STRUCTURE_RAW, StructureHeader) + BodySize);
  if (StructBuffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  StructBuffer->Signature = HMAT_STRUCTURE_PRIVATE_DATA_SIGNATURE;

  StructBodyPtr = (HMAT_MEMORY_SIDE_CACHE_INFO_STRUCTURE *) (((UINT8 *) StructBuffer) + OFFSET_OF (HMAT_STRUCTURE_RAW, StructureHeader));
  StructBodyPtr->Type = HMAT_MEMORY_SIDE_CACHE_INFO_TYPE;
  StructBodyPtr->Length = (UINT32) BodySize;
  StructBodyPtr->MemoryProximityDomain = MemoryProximityDomain;
  StructBodyPtr->MemorySideCacheSize = MemorySideCacheSize;
  StructBodyPtr->CacheAttributes.Value = CacheAttributes.Value;
  StructBodyPtr->NumberOfSmbiosHandles = NumberOfSmbiosHandles;

  BufferPtr = (UINT8 *) (StructBodyPtr + 1);
  CopySize = sizeof (UINT16) * NumberOfSmbiosHandles;
  if (SmbiosHandles != NULL) {
    CopyMem (BufferPtr, SmbiosHandles, CopySize);
  }

  InsertTailList (&mHmatEntries, &StructBuffer->Link);
  DEBUG ((DEBUG_INFO, "Add HMAT Memory Side Cache Information Structure. Size is 0x%x\n", StructBodyPtr->Length));

  return EFI_SUCCESS;
}
