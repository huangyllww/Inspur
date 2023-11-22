

#include <HygonCpmPei.h>
#include <HygonCpmBaseIo.h>
#include <SetupVariable.h>


BOOLEAN GetSetupDataPcieSlotEnable(VOID *SetupData, UINTN Index)
{
  SETUP_DATA   *SetupHob = (SETUP_DATA*)SetupData;

  if(Index >= ARRAY_SIZE(SetupHob->PcieDis)){
    return TRUE;
  }

  return !SetupHob->PcieDis[Index];
}


VOID
UpdatePcieAspm (
  HYGON_CPM_HSIO_TOPOLOGY_TABLE     *HsioTable,
  VOID                              *SetupData
  )
{
  UINT8                               Index;
  HSIO_PORT_DESCRIPTOR                *Port;
  SETUP_DATA                          *SetupHob;

  SetupHob = (SETUP_DATA*)SetupData;
  DEBUG((EFI_D_INFO, "UpdatePcieAspm(%d)\n", SetupHob->PcieAspm));

  if (HsioTable == NULL || SetupHob == NULL) {
    return;
  }

  for (Index = 0; Index < HYGON_HSIO_PORT_DESCRIPTOR_SIZE; Index++) {
    Port = &HsioTable->Port[Index];
    if(Port->EngineData.EngineType != HsioPcieEngine){
      continue;
    }

    if(SetupHob->PcieAspm){
      Port->Port.LinkAspm = HsioAspmL1;
    } else {
      Port->Port.LinkAspm = HsioAspmDisabled;
    }

    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }
  }

}



