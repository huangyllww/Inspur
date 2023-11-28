
#ifndef __ACPI_RAM_H__
#define __ACPI_RAM_H__

#include <Base.h>
#include <Library/PlatformCommLib.h>

#pragma pack(1)

typedef struct {
  UINT64      Signature;
  UINT32      RpMmioBase[PCI_HOST_MAX_COUNT];
  UINT32      RpMmioSize[PCI_HOST_MAX_COUNT];
  UINT16      RpIoBase[PCI_HOST_MAX_COUNT];
  UINT16      RpIoSize[PCI_HOST_MAX_COUNT];
  UINT64      RpMmio64Base[PCI_HOST_MAX_COUNT];
  UINT64      RpMmio64Size[PCI_HOST_MAX_COUNT];
  UINT8       RpCount;
  UINT8       RpBusBase[PCI_HOST_MAX_COUNT];
  UINT8       RpBusLimit[PCI_HOST_MAX_COUNT];
  UINT8       VgaIoEn[PCI_HOST_MAX_COUNT];
  UINT8       fTpmPresent;  
} EFI_ACPI_RAM_DATA;

#pragma pack()


#define ACPI_RAM_DATA_SIGNATURE  SIGNATURE_64('$', 'A', 'C', 'P', 'I', 'R', 'A', 'M')

#endif

