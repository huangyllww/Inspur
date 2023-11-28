
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/MemoryMappedConfigurationSpaceAccessTable.h>
#include "AcpiPlatform.h"
#include <Token.h>


#define MCFG_SIGN   EFI_ACPI_3_0_PCI_EXPRESS_MEMORY_MAPPED_CONFIGURATION_SPACE_BASE_ADDRESS_DESCRIPTION_TABLE_SIGNATURE
#define MCFG_REV    EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_SPACE_ACCESS_TABLE_REVISION
typedef EFI_ACPI_MEMORY_MAPPED_ENHANCED_CONFIGURATION_SPACE_BASE_ADDRESS_ALLOCATION_STRUCTURE MCFG_ENTRY;
typedef EFI_ACPI_MEMORY_MAPPED_CONFIGURATION_BASE_ADDRESS_TABLE_HEADER                        MCFG_HDR;

#pragma pack (1)

typedef struct {
  MCFG_HDR      Header;
  MCFG_ENTRY    Entry;
} MCFG_TABLE;

#pragma pack ()



MCFG_TABLE gMcfgTable = {
  {
    {
      MCFG_SIGN,
      sizeof(MCFG_TABLE),
      MCFG_REV,
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
    _PCD_VALUE_PcdPciExpressBaseAddress,                          // BaseAddress
    0x0000,                                                       // PciSegmentGroupNumber
    0x00,                                                         // StartBusNumber
    (UINT8)((PCIEX_LENGTH/SIZE_1MB) - 1),                         // EndBusNumber
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

//  ASSERT(PcdGet32(PcdPciExpressLength) != 0);
//  ASSERT(PcdGet32(PcdPciExpressLength) <= SIZE_256MB);

  PlatformUpdateTables((EFI_ACPI_COMMON_HEADER*)&gMcfgTable);
  AcpiTableUpdateChksum(&gMcfgTable);
  Status = AcpiTable->InstallAcpiTable(
                        AcpiTable,
                        &gMcfgTable,
                        sizeof(gMcfgTable),
                        &TableKey
                        );
  DEBUG((EFI_D_INFO, "%a L%d %r\n", __FUNCTION__, __LINE__, Status));  
}



