#ifndef __BYO_EARLY_GOP_INFO_PPI_H__
#define __BYO_EARLY_GOP_INFO_PPI_H__

#include <Base.h>


typedef
VOID
(EFIAPI *PEI_STALL)(
  IN  UINTN  Microseconds
  );

#define EARLY_GOP_INFO_VERSION1        1

typedef struct {
  UINT32    Version;
  UINT8     Bus;
  UINT8     Dev;
  UINT8     Fun;
  UINT64    LogoDataAddr;
  UINT64    LogoDataSize;
  UINT64    PcieConfigBase;
  UINT64    PciIoTranslation;
  UINT64    PciMmio32Translation;
  PEI_STALL Stall;
} BYO_EARLY_GOP_INFO;

extern EFI_GUID gByoEarlyGopInfoPpiGuid;


#endif
