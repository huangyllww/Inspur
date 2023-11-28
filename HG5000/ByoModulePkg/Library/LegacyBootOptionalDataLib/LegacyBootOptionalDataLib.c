
#include <PiDxe.h>
#include <Library/DebugLib.h>

#pragma pack(1)
typedef struct {
  UINT32     Tag;
  UINT16     BbsIndex;
  UINT8      PciClass;
  UINT8      PciSubClass;
} LEGACY_BOOT_OPTION_BBS_DATA;
#pragma pack()


#define LEGACY_BOOT_OPTION_BBS_DATA_TAG    SIGNATURE_32('B', 'Y', 'L', 'B')



UINT32 LegacyBootOptionalDataGetSize()
{
  return sizeof(LEGACY_BOOT_OPTION_BBS_DATA);
}

VOID LegacyBootOptionalDataSetBbsIndex(VOID *OptionalData, UINT16 Index)
{
  LEGACY_BOOT_OPTION_BBS_DATA  *LegacyData;
  
  ASSERT(OptionalData != NULL);
  
  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA*)OptionalData;
  LegacyData->BbsIndex = Index;
  LegacyData->Tag = LEGACY_BOOT_OPTION_BBS_DATA_TAG;
}

UINT16 LegacyBootOptionalDataGetBbsIndex(VOID *OptionalData)
{
  LEGACY_BOOT_OPTION_BBS_DATA  *LegacyData;
  
  ASSERT(OptionalData != NULL);
  
  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA*)OptionalData;
  ASSERT(LegacyData->Tag == LEGACY_BOOT_OPTION_BBS_DATA_TAG);
  
  return LegacyData->BbsIndex;  
}


VOID LegacyBootOptionalDataSetPciClass(VOID *OptionalData, UINT8 PciClass, UINT8 PciSubClass)
{
  LEGACY_BOOT_OPTION_BBS_DATA  *LegacyData;
  
  ASSERT(OptionalData != NULL);
  
  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA*)OptionalData;
  LegacyData->PciClass = PciClass;
  LegacyData->PciSubClass = PciSubClass; 
}

UINT8 LegacyBootOptionalDataGetPciClass(VOID *OptionalData, UINT8 *PciSubClass OPTIONAL)
{
  LEGACY_BOOT_OPTION_BBS_DATA  *LegacyData;
  
  ASSERT(OptionalData != NULL);
  
  LegacyData = (LEGACY_BOOT_OPTION_BBS_DATA*)OptionalData;
  ASSERT(LegacyData->Tag == LEGACY_BOOT_OPTION_BBS_DATA_TAG);

  if(PciSubClass != NULL){
    *PciSubClass = LegacyData->PciSubClass;
  }
  
  return LegacyData->PciClass; 
}


