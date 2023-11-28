
#ifndef __LEGACY_BOOT_OPTIONAL_DATA_LIB_H__
#define __LEGACY_BOOT_OPTIONAL_DATA_LIB_H__

#include <Base.h>

UINT32 LegacyBootOptionalDataGetSize();
VOID   LegacyBootOptionalDataSetBbsIndex(VOID *OptionalData, UINT16 Index);
UINT16 LegacyBootOptionalDataGetBbsIndex(VOID *OptionalData);

VOID LegacyBootOptionalDataSetPciClass(VOID *OptionalData, UINT8 PciClass, UINT8 PciSubClass);
UINT8 LegacyBootOptionalDataGetPciClass(VOID *OptionalData, UINT8 *PciSubClass OPTIONAL);


#endif
