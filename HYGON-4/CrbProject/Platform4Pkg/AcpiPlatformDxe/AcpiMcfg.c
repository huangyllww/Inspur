

#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include "AcpiPlatform.h"


#pragma pack (1)

typedef struct {
  EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER      Header;
  EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE  Entry;
} MCFG_TABLE;

#pragma pack ()


MCFG_TABLE gMcfgTable = {
  {
    {
      EFI_ACPI_3_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE,
      sizeof(MCFG_TABLE),
      EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_SPACE_ACCESS_TABLE_REVISION,
      0x00,
      {' ', ' ', ' ', ' ', ' ', ' '}, // OemId
      0,                              // OemTableId
      1,                              // OemRevision
      0,                              // CreatorId
      0,                              // CreatorRevision
    },
    0,                                // Reserved
  },

  {
    FixedPcdGet64(PcdPciExpressBaseAddress),                          // BaseAddress
    0x0000,                                                       // PciSegmentGroupNumber
    0x00,                                                         // StartBusNumber
    (UINT8)((FixedPcdGet64(PcdPciExpressBaseSize)/SIZE_1MB) - 1),                         // EndBusNumber
    0x00000000                                                    // Reserved
  }
};


VOID
BuildAcpiMcfgTable (
  EFI_ACPI_TABLE_PROTOCOL  *AcpiTable
  )
{
  UINTN       TableKey;
  EFI_STATUS  Status;

  PlatformUpdateTables((EFI_ACPI_COMMON_HEADER*)&gMcfgTable);
  AcpiTableUpdateChksum(&gMcfgTable);
  Status = AcpiTable->InstallAcpiTable(
                        AcpiTable,
                        &gMcfgTable,
                        sizeof(gMcfgTable),
                        &TableKey
                        );
  ASSERT(!EFI_ERROR(Status));
}



