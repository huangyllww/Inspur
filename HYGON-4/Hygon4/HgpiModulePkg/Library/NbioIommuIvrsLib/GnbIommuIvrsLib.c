/* $NoKeywords:$ */

/**
 * @file
 *
 * IOMMU IVRS Table Creation
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <HGPI.h>
#include  <Gnb.h>
#include  <PiDxe.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/HygonBaseLib.h>
#include  <Library/HygonSocBaseLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/NbioIommuIvrsLib.h>
#include  <Library/PcdLib.h>
#include  <Library/BaseLib.h>
#include  <OptionGnb.h>
#include  <GnbRegisters.h>
#include  <GnbRegistersCommon.h>
#include  <Library/GnbHeapLib.h>
#include  <Protocol/HygonNbioPcieServicesProtocol.h>
#include  <Guid/GnbPcieInfoHob.h>
#include  <Guid/HobList.h>
#include  <Include/GnbIommu.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Protocol/AcpiTable.h>
#include  <Library/MemoryAllocationLib.h>
#include  <Library/BaseMemoryLib.h>
#include  <Library/NbioRegisterAccLib.h>
#include  <Library/GnbLib.h>
#define FILECODE  LIBRARY_NBIOIOMMUIVRSLIB_GNBIOMMUIVRSLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

GNB_BUILD_OPTIONS ROMDATA  GnbBuildOptions;

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

#define IVRS_TABLE_LENGTH  16 * 1024

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

HGPI_STATUS
GnbCreateIvrsEntry (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       UINT64                     EfrAndMask,
  IN       UINT64                     EfrOrMask,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  );

PCI_ADDR
GnbGetIommuPciAddress (
  IN       GNB_HANDLE               *GnbHandle,
  IN       HYGON_CONFIG_PARAMS        *StdHeader
  );

VOID
GnbCreateIvhdHeader10h (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  OUT   IVRS_IVHD_ENTRY_10H        *Ivhd,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  );

VOID
GnbCreateIvhdHeader11h (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  OUT   IVRS_IVHD_ENTRY_11H        *Ivhd,
  IN       UINT64                     EfrAndMask,
  IN       UINT64                     EfrOrMask,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  );

PCI_ADDR
GnbGetHostPciAddress (
  IN      GNB_HANDLE                  *Handle
  );

HGPI_STATUS
GnbBuildIvmdList (
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  );

VOID
GnbIommuIvrsTableDump (
  IN       VOID                       *Ivrs,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  );

VOID
GnbCreateIvhd (
  IN       GNB_HANDLE                 *GnbHandle,
  OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  );

HGPI_STATUS
GnbFmGetBusDecodeRange (
  IN       GNB_HANDLE                 *GnbHandle,
  OUT   UINT8                      *StartBusNumber,
  OUT   UINT8                      *EndBusNumber,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  );

IOMMU_IVRS_HEADER  IvrsHeader = {
  { 'I', 'V', 'R', 'S' },
  sizeof (IOMMU_IVRS_HEADER),
  2,
  0,
  { 'H', 'Y', 'G', 'O', 'N', 0}, // Change ACPI table id to "HYGON"
  { 'H', 'Y', 'G', 'I', 'O', 'M', 'M', 'U'},// Change ACPI table id to "HYGON"
  1,
  { 'H', 'Y', 'G', 'N' },// Change ACPI table id to "HYGON"
  0,
  0,
  0
};

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

/*----------------------------------------------------------------------------------------*/

/**
 * Create gnb ioapic IVHD entry
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Ivhd            IVHD header pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbIvhdAddApicEntry (
  IN       GNB_HANDLE                 *GnbHandle,
  OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32    AddressLow;
  UINT32    AddressHigh;
  UINT64    IoapicAddress;
  PCI_ADDR  GnbPciAddress;
  PCI_ADDR  GnbIoapicPciId;
  UINT32    HygonCpuModel;

  HygonCpuModel = GetHygonSocModel();
  // Get the PCI address of the GNB
  GnbPciAddress = GnbGetHostPciAddress (GnbHandle);

  if (HygonCpuModel == HYGON_EX_CPU) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_BASE_ADDR_HI_REG_HYEX), &AddressHigh, 0);
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_BASE_ADDR_LOW_REG_HYEX), &AddressLow, 0);
  } else {
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_BASE_ADDR_HI_REG_HYGX), &AddressHigh, 0);
    NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_BASE_ADDR_LOW_REG_HYGX), &AddressLow, 0);
  }

  IoapicAddress  = ((UINT64)AddressHigh) << 32;
  IoapicAddress |= ((UINT64)AddressLow) & 0xffffff00;

  if ((IoapicAddress != 0) && ((AddressLow & 0x01) == 1) && (PcdGet8 (PcdCfgGnbIoapicId) != 0xff)) {
    GnbIoapicPciId.AddressValue     = GnbPciAddress.AddressValue;
    GnbIoapicPciId.Address.Function = 1;
    GnbIvhdAddSpecialDeviceEntry (
      IvhdSpecialDeviceIoapic,
      GnbIoapicPciId,
      PcdGet8 (PcdCfgGnbIoapicId),
      0,
      Ivhd,
      StdHeader
      );
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Build IVRS table
 *
 *
 *
 * @param[in]  EfrAndMask       Mask to clear EFR bits
 * @param[in]  EfrOrMask        Mask to set EFR bits
 * @retval        HGPI_SUCCESS
 * @retval        HGPI_ERROR
 */
HGPI_STATUS
GnbIommuIvrsTable (
  IN       UINT64         EfrAndMask,
  IN       UINT64         EfrOrMask
  )
{
  HGPI_STATUS                            Status;
  VOID                                   *Ivrs;
  BOOLEAN                                IvrsSupport;
  GNB_HANDLE                             *GnbHandle;
  BOOLEAN                                IvrsRelativeAddrNamesSupport;
  DXE_HYGON_NBIO_PCIE_SERVICES_PROTOCOL  *PcieServicesProtocol;
  PCIe_PLATFORM_CONFIG                   *Pcie;
  GNB_PCIE_INFORMATION_DATA_HOB          *PciePlatformConfigHobData;
  EFI_ACPI_TABLE_PROTOCOL                *AcpiTable;
  UINTN                                  TableHandle;
  UINT32                                 Value;
  UINT32                                 FchIoapicValue;
  UINT8                                  Counter;
  UINT8                                  InitialGnbIoapicId;
  UINT8                                  InitialFchIoapicId;
  UINT32                                 HygonCpuModel;

  IvrsRelativeAddrNamesSupport = PcdGetBool (PcdIvrsRelativeAddrNamesSupport);

  HygonCpuModel = GetHygonSocModel();

  Status = HGPI_SUCCESS;
  IvrsSupport   = FALSE;
  Counter = 0;
  Ivrs = NULL;

  Ivrs = AllocateZeroPool (IVRS_TABLE_LENGTH);
  ASSERT (Ivrs != NULL);
  CopyMem (Ivrs, &IvrsHeader, sizeof (IvrsHeader));

  // Zero fill table OEM fields.
  LibHygonMemFill (
    (VOID *)&((ACPI_TABLE_HEADER *)Ivrs)->OemId,
    0,
    6,
    (HYGON_CONFIG_PARAMS *)NULL
    );

  LibHygonMemFill (
    (VOID *)&((ACPI_TABLE_HEADER *)Ivrs)->OemTableId,
    0,
    8,
    (HYGON_CONFIG_PARAMS *)NULL
    );

  // Update table OEM fields.
  LibHygonMemCopy (
    (VOID *)&((ACPI_TABLE_HEADER *)Ivrs)->OemId,
    (VOID *)PcdGetPtr (PcdHygonAcpiTableHeaderOemId),
    AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiTableHeaderOemId), 6),
    (HYGON_CONFIG_PARAMS *)NULL
    );

  LibHygonMemCopy (
    (VOID *)&((ACPI_TABLE_HEADER *)Ivrs)->OemTableId,
    (VOID *)PcdGetPtr (PcdHygonAcpiIvrsTableHeaderOemTableId),
    AsciiStrnLenS (PcdGetPtr (PcdHygonAcpiIvrsTableHeaderOemTableId), 8),
    (HYGON_CONFIG_PARAMS *)NULL
    );

  Status = gBS->LocateProtocol (
                  &gHygonNbioPcieServicesProtocolGuid,
                  NULL,
                  &PcieServicesProtocol
                  );

  PcieServicesProtocol->PcieGetTopology (PcieServicesProtocol, (UINT32 **)&PciePlatformConfigHobData);
  Pcie = &(PciePlatformConfigHobData->PciePlatformConfigHob);

  InitialGnbIoapicId = PcdGet8 (PcdCfgGnbIoapicId);
  InitialFchIoapicId = PcdGet8 (PcdCfgFchIoapicId);

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    if (GnbCheckIommuPresent (GnbHandle, (HYGON_CONFIG_PARAMS *)NULL)) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "Build IVRS for Socket %d Silicon %d\n", GnbGetSocketId (GnbHandle), GnbGetDieNumber (GnbHandle));
      IvrsSupport = TRUE;

      if (InitialGnbIoapicId == 0x0) {
        if (HygonCpuModel == HYGON_EX_CPU) {
          NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_ID_REG_HYEX), &Value, 0);
        } else {
          NbioRegisterRead (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, IOAPIC_ID_REG_HYGX), &Value, 0);
        }
        IDS_HDT_CONSOLE (MAIN_FLOW, "PcdCfgGnbIoapicId Value 0x%x\n", Value);
        PcdSet8S (PcdCfgGnbIoapicId, (Value & 0xFF000000) >> 24);
      } else {
        PcdSet8S (PcdCfgGnbIoapicId, InitialGnbIoapicId + Counter);
      }

      if (InitialFchIoapicId == 0x0) {
        FchIoapicValue = 0;
        LibHygonMemWrite (AccessWidth32, 0xFEC00000, &FchIoapicValue, (HYGON_CONFIG_PARAMS *)NULL);
        LibHygonMemRead (AccessWidth32, 0xFEC00010, &FchIoapicValue, (HYGON_CONFIG_PARAMS *)NULL);
        IDS_HDT_CONSOLE (GNB_TRACE, "FchIoapicValue Value 0x%x\n", FchIoapicValue);
        PcdSet8S (PcdCfgFchIoapicId, (FchIoapicValue & 0xFF000000) >> 24);
      }

      IDS_HDT_CONSOLE (MAIN_FLOW, "PcdCfgGnbIoapicId 0x%x\n", PcdGet8 (PcdCfgGnbIoapicId));
      IDS_HDT_CONSOLE (MAIN_FLOW, "PcdCfgFchIoapicId 0x%x\n", PcdGet8 (PcdCfgFchIoapicId));

      GnbCreateIvrsEntry (GnbHandle, IvrsIvhdBlock10h, Ivrs, EfrAndMask, EfrOrMask, (HYGON_CONFIG_PARAMS *)NULL);
      GnbCreateIvrsEntry (GnbHandle, IvrsIvhdBlock11h, Ivrs, EfrAndMask, EfrOrMask, (HYGON_CONFIG_PARAMS *)NULL);
      GnbBuildIvmdList (IvrsIvmdBlock, Ivrs, (HYGON_CONFIG_PARAMS *)NULL);
      if (IvrsRelativeAddrNamesSupport) {
        GnbCreateIvrsEntry (GnbHandle, IvrsIvhdrBlock40h, Ivrs, EfrAndMask, EfrOrMask, (HYGON_CONFIG_PARAMS *)NULL);
        // GnbCreateIvrsEntry (GnbHandle, IvrsIvhdrBlock41h, Ivrs, EfrAndMask, EfrOrMask, (HYGON_CONFIG_PARAMS *)NULL);
        GnbBuildIvmdList (IvrsIvmdrBlock, Ivrs, (HYGON_CONFIG_PARAMS *)NULL);
      }
      Counter++;
    }

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  // Restore original PCD values probably unnecessary
  PcdSet8S (PcdCfgGnbIoapicId, InitialGnbIoapicId);
  PcdSet8S (PcdCfgFchIoapicId, InitialFchIoapicId);

  if (IvrsSupport == TRUE) {
    ChecksumAcpiTable ((ACPI_TABLE_HEADER *)Ivrs, (HYGON_CONFIG_PARAMS *)NULL);
    GnbIommuIvrsTableDump (Ivrs, (HYGON_CONFIG_PARAMS *)NULL);
    Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
    if (!EFI_ERROR (Status)) {
      Status = AcpiTable->InstallAcpiTable (AcpiTable, Ivrs, ((ACPI_TABLE_HEADER *)Ivrs)->TableLength, &TableHandle);
    }
  } else {
    IDS_HDT_CONSOLE (MAIN_FLOW, "  IVRS table not generated\n");
  }

  return Status;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Build IVMD list
 *
 *
 * @param[in]  Type            Entry type
 * @param[in]  Ivrs            IVRS table pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */
HGPI_STATUS
GnbBuildIvmdList (
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  )
{
  IOMMU_EXCLUSION_RANGE_DESCRIPTOR  *IvrsExclusionRangeList;
  IVRS_IVMD_ENTRY                   *Ivmd;
  UINT16                            StartId;
  UINT16                            EndId;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBuildIvmdList Entry\n");
  //
  // todo - Create Pcd or Protocol
  //
  // IvrsExclusionRangeList = ((HYGON_LATE_PARAMS*)StdHeader)->IvrsExclusionRangeList;
  IvrsExclusionRangeList = NULL;

  if (IvrsExclusionRangeList != NULL) {
    // Process the entire IvrsExclusionRangeList here and create an IVMD for eache entry
    IDS_HDT_CONSOLE (GNB_TRACE, "Process Exclusion Range List\n");
    while ((IvrsExclusionRangeList->Flags & DESCRIPTOR_TERMINATE_LIST) == 0) {
      if ((IvrsExclusionRangeList->Flags & DESCRIPTOR_IGNORE) == 0) {
        // Address of IVMD entry
        Ivmd    = (IVRS_IVMD_ENTRY *)((UINT8 *)Ivrs + ((IOMMU_IVRS_HEADER *)Ivrs)->TableLength);
        StartId =
          (IvrsExclusionRangeList->RequestorIdStart.Bus << 8) +
          (IvrsExclusionRangeList->RequestorIdStart.Device << 3) +
          (IvrsExclusionRangeList->RequestorIdStart.Function);
        EndId =
          (IvrsExclusionRangeList->RequestorIdEnd.Bus << 8) +
          (IvrsExclusionRangeList->RequestorIdEnd.Device << 3) +
          (IvrsExclusionRangeList->RequestorIdEnd.Function);
        GnbIvmdAddEntry (
          Type,
          StartId,
          EndId,
          IvrsExclusionRangeList->RangeBaseAddress,
          IvrsExclusionRangeList->RangeLength,
          Ivmd,
          StdHeader
          );
        // Add entry size to existing table length
        ((IOMMU_IVRS_HEADER *)Ivrs)->TableLength += sizeof (IVRS_IVMD_ENTRY);
      }

      // Point to next entry in IvrsExclusionRangeList
      IvrsExclusionRangeList++;
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbBuildIvmdList Exit\n");
  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Dump IVRS table
 *
 *
 * @param[in]     Ivrs            Pointer to IVRS table
 * @param[in]     StdHeader       Standard Configuration Header
 */
VOID
GnbIommuIvrsTableDump (
  IN       VOID                 *Ivrs,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8  *Block;
  UINT8  *Entry;

  Block = (UINT8 *)Ivrs + sizeof (IOMMU_IVRS_HEADER);
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  IVRS Table Start -----------> \n");
  IDS_HDT_CONSOLE (GNB_TRACE, "  IVInfo           = 0x%08x\n", ((IOMMU_IVRS_HEADER *)Ivrs)->IvInfo);
  while (Block < ((UINT8 *)Ivrs + ((IOMMU_IVRS_HEADER *)Ivrs)->TableLength)) {
    if (*Block == IvrsIvhdBlock10h || *Block == IvrsIvhdBlock11h || *Block == IvrsIvhdrBlock40h) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVHD Block Start -------->\n");
      IDS_HDT_CONSOLE (GNB_TRACE, "  Flags            = 0x%02x\n", ((IVRS_IVHD_ENTRY *)Block)->Flags);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Type             = 0x%02x\n", ((IVRS_IVHD_ENTRY *)Block)->Type);
      IDS_HDT_CONSOLE (GNB_TRACE, "  DeviceId         = 0x%04x\n", ((IVRS_IVHD_ENTRY *)Block)->DeviceId);
      IDS_HDT_CONSOLE (GNB_TRACE, "  CapabilityOffset = 0x%02x\n", ((IVRS_IVHD_ENTRY *)Block)->CapabilityOffset);
      IDS_HDT_CONSOLE (GNB_TRACE, "  BaseAddress      = 0x%08x%08x\n", (UINT32)(((IVRS_IVHD_ENTRY *)Block)->BaseAddress >> 32), (UINT32)((IVRS_IVHD_ENTRY *)Block)->BaseAddress);
      IDS_HDT_CONSOLE (GNB_TRACE, "  PCI Segment      = 0x%04x\n", ((IVRS_IVHD_ENTRY *)Block)->PciSegment);
      IDS_HDT_CONSOLE (GNB_TRACE, "  IommuInfo        = 0x%04x\n", ((IVRS_IVHD_ENTRY *)Block)->IommuInfo);
      if (*Block == IvrsIvhdBlock11h) {
        IDS_HDT_CONSOLE (GNB_TRACE, "  IommuAttributes  = 0x%08x\n", ((IVRS_IVHD_ENTRY_11H *)Block)->IommuAttributes);
        IDS_HDT_CONSOLE (GNB_TRACE, "  IommuEfr         = 0x%16lx\n", ((IVRS_IVHD_ENTRY_11H *)Block)->IommuEfr);
        Entry = Block + sizeof (IVRS_IVHD_ENTRY_11H);
      } else if ( *Block == IvrsIvhdrBlock40h) {
        IDS_HDT_CONSOLE (GNB_TRACE, "  IommuAttributes  = 0x%08x\n", ((IVRS_IVHD_ENTRY_40H *)Block)->IommuAttributes);
        IDS_HDT_CONSOLE (GNB_TRACE, "  IommuEfr         = 0x%16lx\n", ((IVRS_IVHD_ENTRY_40H *)Block)->IommuEfr);
        Entry = Block + sizeof (IVRS_IVHD_ENTRY_40H);
      } else {
        IDS_HDT_CONSOLE (GNB_TRACE, "  IommuEfr         = 0x%08x\n", ((IVRS_IVHD_ENTRY_10H *)Block)->IommuEfr);
        Entry = Block + sizeof (IVRS_IVHD_ENTRY_10H);
      }

      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVHD Block Device Entries Start -------->\n");
      while (Entry < (Block + ((IVRS_IVHD_ENTRY *)Block)->Length)) {
        IDS_HDT_CONSOLE (GNB_TRACE, "  ");
        switch (*Entry) {
          case IvhdEntrySelect:
          case IvhdEntryEndRange:
            GnbLibDebugDumpBuffer (Entry, 4, 1, 4);
            Entry = Entry + 4;
            break;
          case IvhdEntryStartRange:
            GnbLibDebugDumpBuffer (Entry, 8, 1, 8);
            Entry = Entry + 8;
            break;
          case IvhdEntryAliasStartRange:
            GnbLibDebugDumpBuffer (Entry, 12, 1, 12);
            Entry = Entry + 12;
            break;
          case IvhdEntryAliasSelect:
          case IvhdEntryExtendedSelect:
          case IvhdEntrySpecialDevice:
            GnbLibDebugDumpBuffer (Entry, 8, 1, 8);
            Entry = Entry + 8;
            break;
          case IvhdEntryPadding:
            Entry = Entry + 4;
            break;
          default:
            IDS_HDT_CONSOLE (GNB_TRACE, "  Unsupported entry type [%d]\n");
            ASSERT (FALSE);
        }
      }

      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVHD Block Device Entries End -------->\n");
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVHD Block End ---------->\n");
    } else if (
               (*Block == IvrsIvmdBlock) ||
               (*Block == IvrsIvmdBlockRange) ||
               (*Block == IvrsIvmdBlockSingle) ||
               (*Block == IvrsIvmdrBlock) ||
               (*Block == IvrsIvmdrBlockSingle)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVMD Block Start -------->\n");
      IDS_HDT_CONSOLE (GNB_TRACE, "  Flags            = 0x%02x\n", ((IVRS_IVMD_ENTRY *)Block)->Flags);
      IDS_HDT_CONSOLE (GNB_TRACE, "  DeviceId         = 0x%04x\n", ((IVRS_IVMD_ENTRY *)Block)->DeviceId);
      switch (*Block) {
        case IvrsIvmdBlock:
        case IvrsIvmdrBlock:
          IDS_HDT_CONSOLE (GNB_TRACE, "  Applies to all devices\n");
          break;
        case IvrsIvmdBlockSingle:
        case IvrsIvmdrBlockSingle:
          IDS_HDT_CONSOLE (GNB_TRACE, "  Applies to a single device\n");
          break;
        default:
          IDS_HDT_CONSOLE (GNB_TRACE, "  DeviceId End   = 0x%04x\n", ((IVRS_IVMD_ENTRY *)Block)->AuxiliaryData);
      }

      IDS_HDT_CONSOLE (GNB_TRACE, "  StartAddress     = 0x%08x%08x\n", (UINT32)(((IVRS_IVMD_ENTRY *)Block)->BlockStart >> 32), (UINT32)((IVRS_IVMD_ENTRY *)Block)->BlockStart);
      IDS_HDT_CONSOLE (GNB_TRACE, "  BockLength       = 0x%08x%08x\n", (UINT32)(((IVRS_IVMD_ENTRY *)Block)->BlockLength >> 32), (UINT32)((IVRS_IVMD_ENTRY *)Block)->BlockLength);
      IDS_HDT_CONSOLE (GNB_TRACE, "  <-------------IVMD Block End ---------->\n");
    }

    Block = Block + ((IVRS_IVHD_ENTRY *)Block)->Length;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  IVRS Table Raw Data --------> \n");
  GnbLibDebugDumpBuffer (Ivrs, ((IOMMU_IVRS_HEADER *)Ivrs)->TableLength, 1, 16);
  IDS_HDT_CONSOLE (GNB_TRACE, "\n");
  IDS_HDT_CONSOLE (GNB_TRACE, "<----------  IVRS Table End -------------> \n");
}

/*----------------------------------------------------------------------------------------*/

/**
 * Create IVRS entry
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Type            Entry type
 * @param[in]  Ivrs            IVRS table pointer
 * @param[in]  EfrAndMask      Mask to clear EFR bits
 * @param[in]  EfrOrMask       Mask to set EFR bits
 * @param[in]  StdHeader       Standard configuration header
 * @retval     HGPI_STATUS
 *
 */
HGPI_STATUS
GnbCreateIvrsEntry (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  IN       VOID                       *Ivrs,
  IN       UINT64                     EfrAndMask,
  IN       UINT64                     EfrOrMask,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  )
{
  IVRS_IVHD_ENTRY  *Ivhd;
  UINT8            IommuCapabilityOffset;
  UINT32           Value;
  PCI_ADDR         GnbIommuPciAddress;

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbFmCreateIvrsEntry Entry\n");
  if (Type == IvrsIvhdBlock10h || Type == IvrsIvhdBlock11h || Type == IvrsIvhdrBlock40h || Type == IvrsIvhdrBlock41h) {
    GnbIommuPciAddress = GnbGetIommuPciAddress (GnbHandle, StdHeader);
    // Update IVINFO
    IommuCapabilityOffset = GnbLibFindPciCapability (GnbIommuPciAddress.AddressValue, IOMMU_CAP_ID, StdHeader);
    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (IommuCapabilityOffset + 0x10), AccessWidth32, &Value, StdHeader);
    ((IOMMU_IVRS_HEADER *)Ivrs)->IvInfo = Value & (IVINFO_HTATSRESV_MASK | IVINFO_VASIZE_MASK | IVINFO_GASIZE_MASK | IVINFO_PASIZE_MASK);
    // EFRSup: IVINFO[0] = bit 27 of Cap+0x10
    GnbLibPciRead (GnbIommuPciAddress.AddressValue | (IommuCapabilityOffset + 0x00), AccessWidth32, &Value, StdHeader);
    if ((Value & BIT27) != 0) {
      ((IOMMU_IVRS_HEADER *)Ivrs)->IvInfo |= IVINFO_EFRSUP_MASK;
    }

    // Address of IVHD entry
    Ivhd = (IVRS_IVHD_ENTRY *)((UINT8 *)Ivrs + ((IOMMU_IVRS_HEADER *)Ivrs)->TableLength);
    if (Type == IvrsIvhdBlock10h) {
      GnbCreateIvhdHeader10h (GnbHandle, Type, (IVRS_IVHD_ENTRY_10H *)Ivhd, StdHeader);
    }

    if (Type == IvrsIvhdBlock11h || Type == IvrsIvhdrBlock40h) {
      GnbCreateIvhdHeader11h (GnbHandle, Type, (IVRS_IVHD_ENTRY_11H *)Ivhd, EfrAndMask, EfrOrMask, StdHeader);
    }

    GnbCreateIvhd (GnbHandle, Ivhd, StdHeader);

    ((IOMMU_IVRS_HEADER *)Ivrs)->TableLength = ((IOMMU_IVRS_HEADER *)Ivrs)->TableLength + Ivhd->Length;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "GnbFmCreateIvrsEntry Exit\n");
  return HGPI_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get IOMMU PCI address
 *
 *
 * @param[in]  GnbHandle       GNB handle
 * @param[in]  StdHeader       Standard configuration header
 */
PCI_ADDR
GnbGetIommuPciAddress (
  IN       GNB_HANDLE               *GnbHandle,
  IN       HYGON_CONFIG_PARAMS        *StdHeader
  )
{
  PCI_ADDR  GnbIommuPciAddress;

  GnbIommuPciAddress = GnbGetHostPciAddress (GnbHandle);
  GnbIommuPciAddress.Address.Function = 0x2;
  return GnbIommuPciAddress;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Check if IOMMU unit present and enabled
 *
 *
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  StdHeader       Standard configuration header
 * @retval     BOOLEAN
 *
 */
BOOLEAN
GnbCheckIommuPresent (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  )
{
  PCI_ADDR  GnbIommuPciAddress;

  if (GnbHandle->IohubPresent) {
    GnbIommuPciAddress = GnbGetIommuPciAddress (GnbHandle, StdHeader);
    if (GnbLibPciIsDevicePresent (GnbIommuPciAddress.AddressValue, StdHeader)) {
      return TRUE;
    }
  }

  return FALSE;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Create IVRS entry
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Type            Block type
 * @param[in]  Ivhd            IVHD header pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbCreateIvhdHeader10h (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  OUT   IVRS_IVHD_ENTRY_10H        *Ivhd,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32          Value;
  PCI_ADDR        GnbIommuPciAddress;
  MMIO_0x30       MMIO_x30_Value;
  MMIO_0x18       MMIO_x18_Value;
  MMIO_0x4000     MMIO_x4000_Value;
  CAPABILITY_REG  CapValue;
  UINT32          MsiNumPPR;

  GnbIommuPciAddress  = GnbGetIommuPciAddress (GnbHandle, StdHeader);
  Ivhd->Ivhd.Type     = (UINT8)Type;
  Ivhd->Ivhd.Length   = sizeof (IVRS_IVHD_ENTRY_10H);
  Ivhd->Ivhd.DeviceId = (UINT16)(((NbioGetHostPciAddress (GnbHandle).AddressValue) >> 12) | 2);
  Ivhd->Ivhd.CapabilityOffset = GnbLibFindPciCapability (GnbIommuPciAddress.AddressValue, IOMMU_CAP_ID, StdHeader);
  Ivhd->Ivhd.PciSegment = 0;
  GnbLibPciRead (GnbIommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x4), AccessWidth32, &Ivhd->Ivhd.BaseAddress, StdHeader);
  GnbLibPciRead (GnbIommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x8), AccessWidth32, (UINT8 *)&Ivhd->Ivhd.BaseAddress + 4, StdHeader);
  Ivhd->Ivhd.BaseAddress = Ivhd->Ivhd.BaseAddress & 0xfffffffffffffffe;
  ASSERT (Ivhd->Ivhd.BaseAddress != 0x0);

  GnbLibMemRead (Ivhd->Ivhd.BaseAddress + 0x30, AccessWidth64, &(MMIO_x30_Value.Value), StdHeader);
  GnbLibMemRead (Ivhd->Ivhd.BaseAddress + 0x18, AccessWidth64, &(MMIO_x18_Value.Value), StdHeader);
  GnbLibPciRead (GnbIommuPciAddress.AddressValue | Ivhd->Ivhd.CapabilityOffset, AccessWidth32, &(CapValue.Value), StdHeader);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.Coherent != 0) ? IVHD_FLAG_COHERENT : 0);
  Ivhd->Ivhd.Flags |= ((CapValue.Field.IommuIoTlbsup != 0) ? IVHD_FLAG_IOTLBSUP : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.Isoc != 0) ? IVHD_FLAG_ISOC : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.ResPassPW != 0) ? IVHD_FLAG_RESPASSPW : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.PassPW != 0) ? IVHD_FLAG_PASSPW : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x30_Value.Field.PPRSup != 0) ? IVHD_FLAG_PPRSUB : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x30_Value.Field.PreFSup != 0) ? IVHD_FLAG_PREFSUP : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.HtTunEn != 0) ? IVHD_FLAG_HTTUNEN : 0);

  GnbLibPciRead (GnbIommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x10), AccessWidth32, &Value, StdHeader);
  Ivhd->Ivhd.IommuInfo = (UINT16)(Value & 0x1f);
  MsiNumPPR = Value >> 27;
  GnbLibPciRead (GnbIommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0xC), AccessWidth32, &Value, StdHeader);
  Ivhd->Ivhd.IommuInfo |= ((Value & 0x1f) << IVHD_INFO_UNITID_OFFSET);

  GnbLibMemRead (Ivhd->Ivhd.BaseAddress + 0x4000, AccessWidth64, &(MMIO_x4000_Value.Value), StdHeader);
  Ivhd->IommuEfr = (UINT32)((MMIO_x30_Value.Field.XTSup << IVHD_EFR_XTSUP_OFFSET) |
                            (MMIO_x30_Value.Field.NXSup << IVHD_EFR_NXSUP_OFFSET) |
                            (MMIO_x30_Value.Field.GTSup << IVHD_EFR_GTSUP_OFFSET) |
                            (MMIO_x30_Value.Field.GLXSup << IVHD_EFR_GLXSUP_OFFSET) |
                            (MMIO_x30_Value.Field.IASup << IVHD_EFR_IASUP_OFFSET) |
                            (MMIO_x30_Value.Field.GASup << IVHD_EFR_GASUP_OFFSET) |
                            (MMIO_x30_Value.Field.HESup << IVHD_EFR_HESUP_OFFSET) |
                            (MMIO_x30_Value.Field.PASmax << IVHD_EFR_PASMAX_OFFSET) |
                            (MMIO_x4000_Value.Field.NCounter << IVHD_EFR_PNCOUNTERS_OFFSET) |
                            (MMIO_x4000_Value.Field.NCounterBanks << IVHD_EFR_PNBANKS_OFFSET) |
                            ((UINTN)(MsiNumPPR) << IVHD_EFR_MSINUMPPR_OFFSET) |
                            (MMIO_x30_Value.Field.GATS << IVHD_EFR_GATS_OFFSET) |
                            (MMIO_x30_Value.Field.HATS << IVHD_EFR_HATS_OFFSET));
}

/*----------------------------------------------------------------------------------------*/

/**
 * Create IVRS entry
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Type            Block type
 * @param[in]  Ivhd            IVHD header pointer
 * @param[in]  EfrAndMask      Mask to clear EFR bits
 * @param[in]  EfrOrMask       Mask to set EFR bits
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbCreateIvhdHeader11h (
  IN       GNB_HANDLE                 *GnbHandle,
  IN       IVRS_BLOCK_TYPE            Type,
  OUT   IVRS_IVHD_ENTRY_11H        *Ivhd,
  IN       UINT64                     EfrAndMask,
  IN       UINT64                     EfrOrMask,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  )
{
  UINT32          Value;
  PCI_ADDR        GnbIommuPciAddress;
  UINT64          BaseAddress;
  MMIO_0x30       MMIO_x30_Value;
  MMIO_0x18       MMIO_x18_Value;
  CAPABILITY_REG  CapValue;

  GnbIommuPciAddress  = GnbGetIommuPciAddress (GnbHandle, StdHeader);
  Ivhd->Ivhd.Type     = (UINT8)Type;
  Ivhd->Ivhd.Length   = sizeof (IVRS_IVHD_ENTRY_11H);
  Ivhd->Ivhd.DeviceId = (UINT16)(((NbioGetHostPciAddress (GnbHandle).AddressValue) >> 12) | 2);
  Ivhd->Ivhd.CapabilityOffset = GnbLibFindPciCapability (GnbIommuPciAddress.AddressValue, IOMMU_CAP_ID, StdHeader);
  Ivhd->Ivhd.PciSegment = 0;
  GnbLibPciRead (GnbIommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x4), AccessWidth32, &Ivhd->Ivhd.BaseAddress, StdHeader);
  GnbLibPciRead (GnbIommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x8), AccessWidth32, (UINT8 *)&Ivhd->Ivhd.BaseAddress + 4, StdHeader);
  Ivhd->Ivhd.BaseAddress = Ivhd->Ivhd.BaseAddress & 0xfffffffffffffffe;
  ASSERT (Ivhd->Ivhd.BaseAddress != 0x0);

  GnbLibMemRead (Ivhd->Ivhd.BaseAddress + 0x30, AccessWidth64, &(MMIO_x30_Value.Value), StdHeader);
  GnbLibMemRead (Ivhd->Ivhd.BaseAddress + 0x18, AccessWidth64, &(MMIO_x18_Value.Value), StdHeader);
  GnbLibPciRead (GnbIommuPciAddress.AddressValue | Ivhd->Ivhd.CapabilityOffset, AccessWidth32, &(CapValue.Value), StdHeader);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.Coherent != 0) ? IVHD_FLAG_COHERENT : 0);
  Ivhd->Ivhd.Flags |= ((CapValue.Field.IommuIoTlbsup != 0) ? IVHD_FLAG_IOTLBSUP : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.Isoc != 0) ? IVHD_FLAG_ISOC : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.ResPassPW != 0) ? IVHD_FLAG_RESPASSPW : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.PassPW != 0) ? IVHD_FLAG_PASSPW : 0);
  Ivhd->Ivhd.Flags |= ((MMIO_x18_Value.Field.HtTunEn != 0) ? IVHD_FLAG_HTTUNEN : 0);

  GnbLibPciRead (GnbIommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x10), AccessWidth32, &Value, StdHeader);
  Ivhd->Ivhd.IommuInfo = (UINT16)(Value & 0x1f);
  GnbLibPciRead (GnbIommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0xC), AccessWidth32, &Value, StdHeader);
  Ivhd->Ivhd.IommuInfo |= ((Value & 0x1f) << IVHD_INFO_UNITID_OFFSET);

  // Assign attributes
  GnbLibPciRead (GnbIommuPciAddress.AddressValue | (Ivhd->Ivhd.CapabilityOffset + 0x10), AccessWidth32, &Value, StdHeader);
  Ivhd->IommuAttributes = ((Value & 0xf8000000) >> 27) << 23;
  IDS_HDT_CONSOLE (GNB_TRACE, "Attribute cap offset 0x10 = %x\n", Value);
  BaseAddress = Ivhd->Ivhd.BaseAddress;
  GnbLibMemRead (BaseAddress + 0x4000, AccessWidth32, &Value, StdHeader);

  Ivhd->IommuAttributes |= (((Value & 0x3f000) >> 12) << 17) | (((Value & 0x780) >> 7) << 13);
  IDS_HDT_CONSOLE (GNB_TRACE, "Attribute MMIO 0x4000 = %x\n", Value);

  // Assign 64bits EFR for type 11, 40h
  GnbLibMemRead (BaseAddress + 0x30, AccessWidth32, &Ivhd->IommuEfr, StdHeader);
  GnbLibMemRead (BaseAddress + 0x34, AccessWidth32, (UINT8 *)&Ivhd->IommuEfr + 4, StdHeader);
  Ivhd->IommuEfr |= BIT46;
  Ivhd->IommuEfr &= EfrAndMask;
  Ivhd->IommuEfr |= EfrOrMask;

  IDS_HDT_CONSOLE (GNB_TRACE, "IommuEfr = %lx\n", Ivhd->IommuEfr);
}

/*----------------------------------------------------------------------------------------*/

/*
 * Get PCI_ADDR of GNB
 *
 *
 * @param[in] Handle          Pointer to GNB_HANDLE
 * @retval    PCI_ADDR        PCI_ADDR of device
 */
PCI_ADDR
GnbGetHostPciAddress (
  IN      GNB_HANDLE          *Handle
  )
{
  ASSERT (Handle != NULL);
  return Handle->Address;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Create IVHD entry
 *
 *
 * @param[in]  GnbHandle       Gnb handle
 * @param[in]  Ivhd            IVHD header pointer
 * @param[in]  StdHeader       Standard configuration header
 *
 */
VOID
GnbCreateIvhd (
  IN       GNB_HANDLE                 *GnbHandle,
  OUT   IVRS_IVHD_ENTRY            *Ivhd,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  )
{
  HGPI_STATUS  Status;
  PCI_ADDR     Start;
  PCI_ADDR     End;
  UINT8        StartBusNumber;
  UINT8        EndBusNumber;

  Status = GnbFmGetBusDecodeRange (GnbHandle, &StartBusNumber, &EndBusNumber, StdHeader);
  ASSERT (Status == HGPI_SUCCESS);
  Start.AddressValue = MAKE_SBDFO (0, StartBusNumber, 0, 3, 0);
  End.AddressValue   = MAKE_SBDFO (0, EndBusNumber, 0x1F, 6, 0);
  IDS_HDT_CONSOLE (GNB_TRACE, "StartBusNumber = %x     EndBusNumber = %x  \n", StartBusNumber, EndBusNumber);
  GnbIvhdAddDeviceRangeEntry (Start, End, 0, Ivhd, StdHeader);
  if (GnbGetHostPciAddress (GnbHandle).AddressValue == 0) {
    SbCreateIvhdEntries (Ivhd, StdHeader);
  }

  GnbIvhdAddApicEntry (GnbHandle, Ivhd, StdHeader);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get bus range decoded by GNB
 *
 * Final bus allocation can not be assumed until HygonInitMid
 *
 * @param[in]   GnbHandle       GNB handle
 * @param[out]  StartBusNumber  Beginning of the Bus Range
 * @param[out]  EndBusNumber    End of the Bus Range
 * @param[in]   StdHeader       Standard configuration header
 * @retval                      Status
 */
HGPI_STATUS
GnbFmGetBusDecodeRange (
  IN       GNB_HANDLE                 *GnbHandle,
  OUT   UINT8                      *StartBusNumber,
  OUT   UINT8                      *EndBusNumber,
  IN       HYGON_CONFIG_PARAMS          *StdHeader
  )
{
  PCI_ADDR    GnbPciAddress;
  GNB_HANDLE  *NextHandle;

  GnbPciAddress   = NbioGetHostPciAddress (GnbHandle);
  *StartBusNumber = (UINT8)GnbPciAddress.Address.Bus;
  NextHandle = GnbGetNextHandle (GnbHandle);

  if (NextHandle == NULL) {
    *EndBusNumber = 0xFF;
  } else {
    GnbPciAddress = NbioGetHostPciAddress (NextHandle);
    *EndBusNumber = (UINT8)(GnbPciAddress.Address.Bus - 1);
  }

  return HGPI_SUCCESS;
}
