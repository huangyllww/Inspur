

#include "PlatformPei.h"
#include <HygonCpmCommon.h>
#include <Library/Ppi/HygonCpmTablePpi/HygonCpmTablePpi.h>


EFI_STATUS GetDimmMapInfo(IN EFI_PEI_SERVICES **PeiServices)
{
  HYGON_CPM_TABLE_PPI                 *CpmTablePpiPtr;
  EFI_STATUS                          Status;
  HYGON_CPM_DIMM_MAP_TABLE            *DimmMapPtr; 
  UINTN                               Count;
  BYO_HYGON_DIMM_MAP_INFO             *MapInfo;
  UINTN                               Size;
  UINTN                               Index;
  
  
  DEBUG((EFI_D_INFO, "GetDimmMapInfo\n"));

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonCpmTablePpiGuid,
                             0,
                             NULL,
                             (VOID **)&CpmTablePpiPtr
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  DimmMapPtr = CpmTablePpiPtr->CommonFunction.GetTablePtr2(CpmTablePpiPtr, HYEX_CPM_SIGNATURE_DIMM_MAP);
  if (DimmMapPtr != NULL) {
    return EFI_NOT_FOUND;
  }

  for(Count=0; Count<ARRAY_SIZE(DimmMapPtr->DIMM); Count++){
    if(DimmMapPtr->DIMM[Count].ChannelName == 0){
      break;
    }
  }
  DEBUG((EFI_D_INFO, "Count:%d\n", Count));
  if(Count == 0){
    return EFI_NOT_FOUND;    
  }

  Size = sizeof(BYO_HYGON_DIMM_MAP_INFO) + sizeof(BYO_HYGON_DIMM_MAP_DATA) * (Count - 1);
  MapInfo = (BYO_HYGON_DIMM_MAP_INFO*)BuildGuidHob(&gHygonDimmMapInfoHobGuid, Size);
  ASSERT(MapInfo != NULL);

  MapInfo->MapDataCount = (UINT8)Count;
  for(Index=0;Index<Count;Index++){
    MapInfo->MapData[Index].Socket = DimmMapPtr->DIMM[Index].Socket;
    MapInfo->MapData[Index].Cdd    = DimmMapPtr->DIMM[Index].Cdd;
    MapInfo->MapData[Index].UMCchannel = DimmMapPtr->DIMM[Index].UMCchannel;
    MapInfo->MapData[Index].Dimm = DimmMapPtr->DIMM[Index].Dimm;
    MapInfo->MapData[Index].LogicChannel = DimmMapPtr->DIMM[Index].LogicChannel;
    MapInfo->MapData[Index].Channel = DimmMapPtr->DIMM[Index].ChannelName - 'A';
  }

  return Status;
}

