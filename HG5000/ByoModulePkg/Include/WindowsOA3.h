
#include <IndustryStandard/Acpi10.h>

#pragma pack (1)
typedef struct {
  UINT32                           MsdmVersion;
  UINT32                           MsdmReserved;
  UINT32                           MdsmDataType;
  UINT32                           MsdmDataReserved;
  UINT32                           MsdmDataLength;
  UINT8                            MsdmData[29];      //5*5 Product Key, including "-"
} EFI_ACPI_MSDM_DATA_STRUCTURE;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER        Header;
  EFI_ACPI_MSDM_DATA_STRUCTURE       MsdmData;
} EFI_ACPI_MS_DIGITAL_MARKER_TABLE;
#pragma pack ()


