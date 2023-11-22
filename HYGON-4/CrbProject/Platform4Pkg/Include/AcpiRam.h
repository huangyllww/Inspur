
#ifndef __ACPI_RAM_H__
#define __ACPI_RAM_H__

#include <Base.h>
#include <Library/PlatformCommLib.h>

#pragma pack(1)

typedef struct {
  UINT64      Signature;
  
  UINT64      RpMmio64Base[PCI_HOST_MAX_COUNT];
  UINT64      RpMmio64Size[PCI_HOST_MAX_COUNT]; 
  
  UINT32      RpMmioBase[PCI_HOST_MAX_COUNT];
  UINT32      RpMmioSize[PCI_HOST_MAX_COUNT];

  UINT32      Pxm[PCI_HOST_MAX_COUNT];
  
  UINT32      PcieBaseAddress;
  UINT32      PcieBaseLength;
  UINT32      RomSize;
  
  UINT16      RpIoBase[PCI_HOST_MAX_COUNT];
  UINT16      RpIoSize[PCI_HOST_MAX_COUNT];
  
  UINT8       RpCount;
  UINT8       RpBusBase[PCI_HOST_MAX_COUNT];
  UINT8       RpBusLimit[PCI_HOST_MAX_COUNT];
  UINT8       TomLow;
  UINT8       TomHigh;
  UINT8       VgaUid;
  
  UINT8       fTPMPresent:1;
  UINT8       BmcPresent:1;
  UINT8       UT0E:1;
  UINT8       UT1E:1;
  UINT8       UT2E:1;
  UINT8       UT3E:1; 
  UINT8       I2c0En:1;
  UINT8       I2c1En:1; 
  UINT8       I2c2En:1; 
  UINT8       I2c3En:1; 
  UINT8       I2c4En:1; 
  UINT8       I2c5En:1;
  UINT8       S3En:1;
  UINT8       S4En:1;
  UINT8       Pfeh:1; 
  UINT8       Wkpm:1;
} EFI_ACPI_RAM_DATA;

#pragma pack()


#define ACPI_RAM_DATA_SIGNATURE  SIGNATURE_64('$', 'A', 'C', 'P', 'I', 'R', 'A', 'M')

#endif

