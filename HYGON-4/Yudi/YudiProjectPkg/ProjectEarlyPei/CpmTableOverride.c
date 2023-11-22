#include <HygonCpmPei.h>
#include <Library/PlatformCommLib.h>
#include <HygonHsioInfo.h>
#include <Library/ByoHygonCommLib.h>
#include <BoardIdType.h>
#include <Library/ByoHygonGpioLib.h>

VOID
UpdatePcieAspm (
  HYGON_CPM_HSIO_TOPOLOGY_TABLE     *HsioTable,
  VOID                              *SetupData
  );

BOOLEAN 
GetSetupDataPcieSlotEnable (
  VOID *SetupData, 
  UINTN Index
  );


VOID
UpdatePcieSlotId (
  IN HYGON_CPM_HSIO_TOPOLOGY_TABLE  *HsioTable,
  IN UINT16                         *SlotId
  )
{
  UINT8                               Index;
  HSIO_PORT_DESCRIPTOR                *Port;
  UINT16                              Id = *SlotId;

  DEBUG((EFI_D_INFO, "UpdatePcieSlotId\n"));
  if (HsioTable == NULL) {
    return;
  }

  for (Index = 0; Index < HYGON_HSIO_PORT_DESCRIPTOR_SIZE; Index++) {
    Port = &HsioTable->Port[Index];
    if(Port->EngineData.EngineType == HsioPcieEngine && Port->Port.SlotNum == 0){
      Port->Port.SlotNum = Id++;
      DEBUG((EFI_D_INFO, "(%d,%d) slot %d\n", Port->EngineData.StartLane, Port->EngineData.EndLane, Port->Port.SlotNum));
    }
    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }
  }

  *SlotId = Id;
}


STATIC HSIO_PORT_DESCRIPTOR  gHsioPcieSample = {
  0,
  HSIO_ENGINE_DATA_INITIALIZER (HsioPcieEngine, 0, 0, HsioHotplugDisabled, 1),
  HSIO_PORT_DATA_INITIALIZER_PCIE (
  HsioPortEnabled,                      // Port Present
  0,                                    // Requested Device
  0,                                    // Requested Function
  HsioHotplugDisabled,                  // Hotplug
  HsioGenMaxSupported,                  // Max Link Speed
  HsioGenMaxSupported,                  // Max Link Capability
  HsioAspmL1,                           // ASPM
  HsioAspmL1,                           // ASPM L1.1 disabled
  HsioAspmL1,                           // ASPM L1.2 disabled
  HsioClkPmSupportDisabled,             // Clock PM
  0
  )
};

STATIC HSIO_PORT_DESCRIPTOR  gHsioSataSample = {
  0,
  HSIO_ENGINE_DATA_INITIALIZER (HsioSATAEngine, 0, 0, HsioHotplugDisabled, 1),
  HSIO_PORT_DATA_INITIALIZER_SATA (
    HsioPortEnabled                       // Port Present
  )
};

typedef struct {
  HSIO_PORT_DESCRIPTOR         *Port;
  HSIO_PORT_DESCRIPTOR         *LastPort;  
  HYGON_CPM_HSIO_TOPOLOGY_TABLE  *s0;
  HYGON_CPM_HSIO_TOPOLOGY_TABLE  *s1;  
  HYGON_CPM_GPIO_INIT_TABLE    *Gpio;  
  VOID                         *SetupHob;
  PLATFORM_COMM_INFO           *Info;
  HSIO_PCIE_CTX_INFO           *CtxInfo;
  HSIO_PCIE_CTX                *PcieCtx;
  UINT8                        Socket;  
  UINT16                       SlotId;
  UINT8                        IsLast;
  UINT8                        PcieCtxIndex;
  UINT8                        LanIndex;
  UINT8                        SetupSlotIndex;
  UINT64                       SlotIdMap;
  UINT8    Cpu0G4Sata;  //BYO_GPIO_DEF_PIN(0,  0, 258)
  UINT8    Cpu0G5Sata;  //BYO_GPIO_DEF_PIN(0,  0, 259)
  UINT8    Cpu1G4Sata;  //BYO_GPIO_DEF_PIN(0,  0, 256)
  UINT8    Cpu1G5Sata;  //BYO_GPIO_DEF_PIN(0,  0, 257)
} DXIO_TABLE_CTX;


UINT16
GetSlotId(
  UINT8                     Socket,
  UINT8                     StartLane,
  UINT8                     EndLane,
  DXIO_TABLE_CTX            *Ctx  
  )
{
  UINT8                               Index;
  HSIO_PORT_DESCRIPTOR                *Port;

  if (Socket == 0) {
    if(Ctx->s0 == NULL){
      return 0;
    }
    Port = &Ctx->s0->Port[0];
  } else {
    if(Ctx->s1 == NULL){
      return 0;
    }  
    Port = &Ctx->s1->Port[0];
  }

  for (Index = 0; Index < HYGON_HSIO_PORT_DESCRIPTOR_SIZE; Index++) {
    if(Port[Index].EngineData.EngineType != HsioPcieEngine){
      continue;
    }
    if(StartLane == Port[Index].EngineData.StartLane && EndLane == Port[Index].EngineData.EndLane){
      return Port[Index].Port.SlotNum;
    }
    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }
  }

  return 0;
}


VOID
SyncSlotId2PcieCtx (
    DXIO_TABLE_CTX           *Ctx
  )
{
  HSIO_PCIE_CTX  *PcieCtx;
  UINTN          Index;
  
  for(Index=0;Index<Ctx->CtxInfo->CtxCount;Index++){
    PcieCtx = &Ctx->CtxInfo->Ctx[Index];
    if(PcieCtx->EngineType == DXIO_DEV_TYPE_PCIE){
      PcieCtx->SlotId = GetSlotId(PcieCtx->Socket, PcieCtx->StartLane, PcieCtx->EndLane, Ctx);
    }
  }
}


VOID DoOemPorting(HSIO_PCIE_CTX *PcieCtx, DXIO_TABLE_CTX *Ctx)
{

}


STATIC
VOID 
ByoAddHsioLane (
    DXIO_TABLE_CTX        *Ctx,
    UINT8                 StartLane,
    UINT8                 EndLane,
    UINT8                 EngineType,
    CHAR8                 *PcieName,
    UINT8                 BuildIn,
    UINT8                 SlotLength
  )
{
  HSIO_PCIE_CTX    *PcieCtx;


  DEBUG((EFI_D_INFO, "AddDxioPcieLane(%d-%d T:%d E:%d %a BI:%d)\n", \
    StartLane, EndLane, EngineType, Ctx->IsLast, \
    PcieName == NULL ? "" : PcieName, BuildIn));

  PcieCtx = &Ctx->PcieCtx[Ctx->PcieCtxIndex++];

  if(EngineType == HsioPcieEngine){
    PcieCtx->EngineType = DXIO_DEV_TYPE_PCIE;
    CopyMem(Ctx->Port, &gHsioPcieSample, sizeof(HSIO_PORT_DESCRIPTOR));

    if(BuildIn != PCIE_BUILD_IN_IGD){
      Ctx->Port->Port.PortPresent = GetSetupDataPcieSlotEnable(Ctx->SetupHob, Ctx->SetupSlotIndex++);
    } else {
      Ctx->Info->IgdSocket    = Ctx->Socket;
      Ctx->Info->IgdStartLane = StartLane;
    }
    
  } else if(EngineType == HsioSATAEngine){
    PcieCtx->EngineType = DXIO_DEV_TYPE_SATA;
    CopyMem(Ctx->Port, &gHsioSataSample, sizeof(HSIO_PORT_DESCRIPTOR));
    
  } else if(EngineType == HsioEthernetEngine){
    PcieCtx->EngineType = DXIO_DEV_TYPE_ILAN;
  }

  Ctx->Port->EngineData.StartLane = StartLane;
  Ctx->Port->EngineData.EndLane   = EndLane;
  if(Ctx->IsLast){
    Ctx->Port->Flags |= DESCRIPTOR_TERMINATE_LIST;
    Ctx->IsLast = 0;
  }

  PcieCtx->Present    = 1;
  PcieCtx->Socket     = Ctx->Socket;
  PcieCtx->StartLane  = StartLane;
  PcieCtx->EndLane    = EndLane;
  PcieCtx->BuildIn    = BuildIn;
  PcieCtx->SlotLength = SlotLength;

  if(PcieName != NULL){
    AsciiStrCpyS(PcieCtx->Name, sizeof(PcieCtx->Name), PcieName);
  }

  DoOemPorting(PcieCtx, Ctx);

  Ctx->LastPort = Ctx->Port;
  Ctx->Port++;

}



UINT8 GetEmptyHpSlotId(DXIO_TABLE_CTX *Ctx)
{
  UINT8  Index;

  for(Index=1;Index<64;Index++){
    if((Ctx->SlotIdMap & LShiftU64(1, Index)) == 0){
      Ctx->SlotIdMap |= LShiftU64(1, Index);
      return Index;
    }
  }

  DEBUG((EFI_D_ERROR, "No valid HP slot id\n"));
  return 0;
}



// *MUST* invoke after UpdateHsioTable().
VOID 
ByoSetHsioLaneSlotIdAndHP (
    DXIO_TABLE_CTX        *Ctx,
    UINT16                SlotId,                  // 0: auto
    BOOLEAN               IsHotPlug
  )
{
  HSIO_PORT_DESCRIPTOR  *Port;
  

  if(Ctx->LastPort == NULL){
    return;
  }

  Port = Ctx->LastPort;
  if(Port->EngineData.EngineType != HsioPcieEngine){
    return;
  }

  if(IsHotPlug){
    Port->Port.LinkHotplug = HsioHotplugServerExpress;
    //- Port->EngineData.HotPluggable = 1;
    if(SlotId > 63 || SlotId == 0 || (Ctx->SlotIdMap & LShiftU64(1, SlotId))!=0){
      SlotId = GetEmptyHpSlotId(Ctx);
      DEBUG((EFI_D_INFO, "re-assign HP slot id to %d\n", SlotId));
    } else {
      Ctx->SlotIdMap |= LShiftU64(1, SlotId);
    }
  } else {
    if(SlotId == 0 || SlotId <= 63){
      SlotId = Ctx->SlotId++;
    }
  }

  Port->Port.SlotNum = SlotId;

}


VOID 
GetHsioGpioStatus (
    DXIO_TABLE_CTX  *Ctx
  )
{
  Ctx->Cpu0G4Sata = LibCpmGetGpio(BYO_GPIO_DEF_PIN(0,  0, 258));
  Ctx->Cpu0G5Sata = LibCpmGetGpio(BYO_GPIO_DEF_PIN(0,  0, 259));
  Ctx->Cpu1G4Sata = LibCpmGetGpio(BYO_GPIO_DEF_PIN(0,  0, 256));
  Ctx->Cpu1G5Sata = LibCpmGetGpio(BYO_GPIO_DEF_PIN(0,  0, 257));
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), Cpu0G4Sata :%d, Cpu0G5Sata :%d.\n", __LINE__, Ctx->Cpu0G4Sata, Ctx->Cpu0G5Sata));
  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), Cpu1G4Sata :%d, Cpu0G5Sata :%d.\n", __LINE__, Ctx->Cpu1G4Sata, Ctx->Cpu1G5Sata));
}

/* Lane scope for Hygon4.
0-15     G0
16-31    G1
32-47    G2
48-63    G3

64-79    G4
80-95    G5
96-111   P6 
112-127  P7 (PCIE 3.0)
128-129  P8 (PCIE 3.0)
*/
VOID UpdateHsioTable (
    DXIO_TABLE_CTX  *Ctx
  )
{
  UINTN                     Size;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n\n", __LINE__));
  if(Ctx->s0 == NULL){
    return;
  }

  Size = sizeof(HSIO_PCIE_CTX_INFO) + sizeof(HSIO_PCIE_CTX) * (HYGON_HSIO_PORT_DESCRIPTOR_SIZE - 1);
  Ctx->PcieCtxIndex = 0;
  Ctx->CtxInfo = (HSIO_PCIE_CTX_INFO*)BuildGuidHob(&gHygonHsioInfoHobGuid, Size);
  Ctx->PcieCtx = Ctx->CtxInfo->Ctx;
  ASSERT(Ctx->CtxInfo != NULL);
  DEBUG((EFI_D_INFO, "CtxInfo:%X L:%d\n", Ctx->CtxInfo, Size));
  ZeroMem(Ctx->CtxInfo, Size);

  //
  //Lanes on socket 0.
  //
  Ctx->Port = &Ctx->s0->Port[0];
  Ctx->Socket = 0;
  switch(FixedPcdGet8(PcdBoardIdType)){
    case TKN_BOARD_ID_TYPE_NHVTB1:
      ByoAddHsioLane(Ctx, 64,  79,  HsioPcieEngine, "G4(J26)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 80,  95,  HsioPcieEngine, "G5(J27)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 96,  111, HsioPcieEngine, "P6(J28)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 112, 115, HsioSATAEngine, NULL,         PCIE_BUILD_IN_SATA,  PCIE_SLOT_LENGTH_NONE);
      ByoAddHsioLane(Ctx, 120, 127, HsioPcieEngine, "P7(J29)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 128, 128, HsioPcieEngine, "IGD",        PCIE_BUILD_IN_IGD,   PCIE_SLOT_LENGTH_OTHER);
      break;

    case TKN_BOARD_ID_TYPE_NHVTB3:
      ByoAddHsioLane(Ctx,  0,   15, HsioPcieEngine, "G0(J39)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 16,   31, HsioPcieEngine, "G1(J40)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 32,   47, HsioPcieEngine, "G2(J41)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 48,   63, HsioPcieEngine, "G3(J42)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 64,   79, HsioPcieEngine, "G4(J14)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 80,   95, HsioPcieEngine, "G5(J15)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 96,  111, HsioPcieEngine, "P6(J16)",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 112, 115, HsioSATAEngine, NULL,         PCIE_BUILD_IN_SATA,  PCIE_SLOT_LENGTH_NONE);
      ByoAddHsioLane(Ctx, 120, 127, HsioPcieEngine, "P7(J208)",   PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 128, 128, HsioPcieEngine, "IGD",        PCIE_BUILD_IN_IGD,   PCIE_SLOT_LENGTH_OTHER);      
      ByoAddHsioLane(Ctx, 129, 129, HsioPcieEngine, "Onboard(JR5)", PCIE_BUILD_IN_LAN, PCIE_SLOT_LENGTH_OTHER);
      break;

    case TKN_BOARD_ID_TYPE_OEM:
      if (Ctx->Cpu0G4Sata) {
        ByoAddHsioLane(Ctx, 64,  71,  HsioPcieEngine, "J62-G4",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      } else {
        ByoAddHsioLane(Ctx, 64,  71,  HsioSATAEngine, NULL,    PCIE_BUILD_IN_SATA,  PCIE_SLOT_LENGTH_NONE);
      }			
      ByoAddHsioLane(Ctx, 72,  79,  HsioPcieEngine, "J61-G4",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);

      if (Ctx->Cpu0G5Sata) {
        ByoAddHsioLane(Ctx, 80,  87,  HsioPcieEngine, "J2-G5",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      } else {
        ByoAddHsioLane(Ctx, 80,  87,  HsioSATAEngine, NULL,    PCIE_BUILD_IN_SATA,  PCIE_SLOT_LENGTH_NONE);
      }		
      ByoAddHsioLane(Ctx, 88,  95,  HsioPcieEngine, "J57-G5",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 96,  103, HsioPcieEngine, "J55-OCP0-P6",    PCIE_BUILD_IN_LAN,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 104,  111, HsioPcieEngine, "J58-P6",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);			
      ByoAddHsioLane(Ctx, 112, 127, HsioPcieEngine, "J116-P7",         PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_NONE);
      ByoAddHsioLane(Ctx, 128, 128, HsioPcieEngine, "IGD",        PCIE_BUILD_IN_IGD,   PCIE_SLOT_LENGTH_OTHER);
      ByoAddHsioLane(Ctx, 129, 129, HsioPcieEngine, "Asmedia",        PCIE_BUILD_IN_SATA,   PCIE_SLOT_LENGTH_OTHER);
      break;

    default:
      ASSERT(FALSE);
      return;
  }
  if(Ctx->LastPort != NULL){
    Ctx->LastPort->Flags |= DESCRIPTOR_TERMINATE_LIST;
  }

  //
  //Lanes on socket 1.
  //
  if(Ctx->s1 == NULL){
    return;
  }
  Ctx->Port = &Ctx->s1->Port[0];
  Ctx->Socket = 1;
  switch(FixedPcdGet8(PcdBoardIdType)){
    case TKN_BOARD_ID_TYPE_NHVTB1:
      ByoAddHsioLane(Ctx, 64,  67,  HsioPcieEngine, "G4(MCIO0,J91)", PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_OTHER);
      ByoAddHsioLane(Ctx, 68,  71,  HsioPcieEngine, "G4(MCIO0,J91)", PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_OTHER);
      ByoAddHsioLane(Ctx, 72,  75,  HsioPcieEngine, "G4(MCIO1,J90)", PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_OTHER);
      ByoAddHsioLane(Ctx, 76,  79,  HsioPcieEngine, "G4(MCIO1,J90)", PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_OTHER);
      ByoAddHsioLane(Ctx, 80,  95,  HsioPcieEngine, "G5(J31)",       PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 96,  111, HsioPcieEngine, "P6(J32)",       PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 112, 127, HsioPcieEngine, "P7(J33)",       PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 129, 129, HsioPcieEngine, "LAN",           PCIE_BUILD_IN_LAN,   PCIE_SLOT_LENGTH_OTHER);
      break;
			
    case TKN_BOARD_ID_TYPE_NHVTB3: //No second Socket for NanHaiVtb3.
      break;

    case TKN_BOARD_ID_TYPE_OEM:
      if (Ctx->Cpu1G4Sata) {
        ByoAddHsioLane(Ctx, 64,  71,  HsioPcieEngine, "J64-G4",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      } else {
        ByoAddHsioLane(Ctx, 64,  71,  HsioSATAEngine, NULL,    PCIE_BUILD_IN_SATA,  PCIE_SLOT_LENGTH_NONE);
      }		
      ByoAddHsioLane(Ctx, 72,  79,  HsioPcieEngine, "J63-G4",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
			
      if (Ctx->Cpu1G5Sata) {
        ByoAddHsioLane(Ctx, 80,  87,  HsioPcieEngine, "J4-G5",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      } else {
        ByoAddHsioLane(Ctx, 80,  87,  HsioSATAEngine, NULL,    PCIE_BUILD_IN_SATA,  PCIE_SLOT_LENGTH_NONE);
      }		
      ByoAddHsioLane(Ctx, 88,  95,  HsioPcieEngine, "J6-G5",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 96,  103, HsioPcieEngine, "J56-OCP1-P6",    PCIE_BUILD_IN_LAN,  PCIE_SLOT_LENGTH_LONG);
      ByoAddHsioLane(Ctx, 104,  111, HsioPcieEngine, "J124-P6",    PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_LONG);			
      ByoAddHsioLane(Ctx, 112, 127, HsioPcieEngine, "J16-P7",         PCIE_BUILD_IN_NONE,  PCIE_SLOT_LENGTH_NONE);
      break;
      
    default:
      ASSERT(FALSE);
      return;
  }	
  if(Ctx->LastPort != NULL){
    Ctx->LastPort->Flags |= DESCRIPTOR_TERMINATE_LIST;
  }
	
  return;
}


VOID DumpDxioSetting(HYGON_CPM_HSIO_TOPOLOGY_TABLE  *HsioTable)
{
  UINT8                               Index;
  HSIO_PORT_DESCRIPTOR                *Port;

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), \n\n", __LINE__));
  if (HsioTable == NULL) {
    return;
  }

  for (Index = 0; Index < HYGON_HSIO_PORT_DESCRIPTOR_SIZE; Index++) {
    Port = &HsioTable->Port[Index];
    DEBUG((EFI_D_INFO, "[%d-%d] P:%d T:%d ", \
      Port->EngineData.StartLane, Port->EngineData.EndLane, Port->Port.PortPresent, Port->EngineData.EngineType));
    if(Port->EngineData.EngineType == HsioPcieEngine){
      DEBUG((EFI_D_INFO, "slot:%d HP:%d", Port->Port.SlotNum, Port->Port.LinkHotplug));
    }
    DEBUG((EFI_D_INFO, "\n"));

    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }
  }
}


HSIO_PORT_DESCRIPTOR *FindLastDxioPort(HYGON_CPM_HSIO_TOPOLOGY_TABLE *HsioTable)
{
  UINTN                  Index;
  HSIO_PORT_DESCRIPTOR   *Port;

  for (Index = 0; Index < HYGON_HSIO_PORT_DESCRIPTOR_SIZE; Index++) {
    Port = &HsioTable->Port[Index];
    if(Port->Flags == DESCRIPTOR_TERMINATE_LIST){
      return Port;
    }
  }

  ASSERT(FALSE);
  return NULL;
}

VOID 
AddDisabledPcieLane(
  HYGON_CPM_HSIO_TOPOLOGY_TABLE *HsioTable, 
  HSIO_PORT_DESCRIPTOR          *Port, 
  UINT8                         StartLane, 
  UINT8                         EndLane
  )
{
  UINTN                         Index;
  HSIO_PORT_DESCRIPTOR          *p;
  BOOLEAN                       IsOverLay = FALSE;


  for (Index = 0; Index < HYGON_HSIO_PORT_DESCRIPTOR_SIZE; Index++) {
    p = &HsioTable->Port[Index];
    if(StartLane >= p->EngineData.StartLane && StartLane <= p->EngineData.EndLane){
      IsOverLay = TRUE;
      break;
    }
    if(EndLane >= p->EngineData.StartLane && EndLane <= p->EngineData.EndLane){
      IsOverLay = TRUE;
      break;
    }
    if(p->Flags == DESCRIPTOR_TERMINATE_LIST){
      break;
    }
  }

  DEBUG((EFI_D_INFO, __FUNCTION__"(L.%d), IsOverLay :%d, Lane :%d-%d.\n", __LINE__, IsOverLay, StartLane, EndLane));	
  if(!IsOverLay){
    Port->Flags &= ~DESCRIPTOR_TERMINATE_LIST;
    Port++;    
    CopyMem(Port, &gHsioPcieSample, sizeof(HSIO_PORT_DESCRIPTOR));
    Port->Port.PortPresent     = HsioPortDisabled;
    Port->EngineData.StartLane = StartLane;
    Port->EngineData.EndLane   = EndLane;
    Port->Flags |= DESCRIPTOR_TERMINATE_LIST;    
  }
}


VOID DisableUnusedLane(DXIO_TABLE_CTX *Ctx)
{
  HSIO_PORT_DESCRIPTOR   *Port;

  switch(FixedPcdGet8(PcdBoardIdType)){
    case TKN_BOARD_ID_TYPE_NHVTB1:
      Port = FindLastDxioPort(Ctx->s0);
      if(Port != NULL){
        AddDisabledPcieLane(Ctx->s0, Port, 116, 119);
        AddDisabledPcieLane(Ctx->s0, Port, 129, 129);    
      }
      Port = FindLastDxioPort(Ctx->s1);
      if(Port != NULL){
        AddDisabledPcieLane(Ctx->s1, Port, 128, 128);
      }  
      break;

    case TKN_BOARD_ID_TYPE_NHVTB3:
      Port = FindLastDxioPort(Ctx->s0);
      if(Port != NULL){
        AddDisabledPcieLane(Ctx->s0, Port, 116, 119);
      }      
      break;

    case TKN_BOARD_ID_TYPE_OEM:
      Port = FindLastDxioPort(Ctx->s0);
      if(Port != NULL){
        AddDisabledPcieLane(Ctx->s0, Port, 129, 129);    
      }
      Port = FindLastDxioPort(Ctx->s1);
      if(Port != NULL){
        AddDisabledPcieLane(Ctx->s1, Port, 128, 129);
      }  
      break;
			
    default:
      break;
  }
}




EFI_STATUS
PrjCpmTableOverride (
  IN       EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS                   Status = EFI_SUCCESS;
  HYGON_CPM_TABLE_PPI          *CpmTablePpi;
  DXIO_TABLE_CTX               Ctx;
  

  DEBUG((EFI_D_INFO, "PrjCpmTableOverride\n"));
  DEBUG((EFI_D_INFO, "sizeof(HYGON_CPM_HSIO_TOPOLOGY_TABLE):%d\n", sizeof(HYGON_CPM_HSIO_TOPOLOGY_TABLE)));

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonCpmTablePpiGuid,
                             0,
                             NULL,
                             (VOID**)&CpmTablePpi
                             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem(&Ctx, sizeof(Ctx));
  Ctx.SlotId = 100;
  Ctx.s0   = (HYGON_CPM_HSIO_TOPOLOGY_TABLE*)CpmTablePpi->CommonFunction.GetTablePtr2(CpmTablePpi, HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY);
  Ctx.s1   = (HYGON_CPM_HSIO_TOPOLOGY_TABLE*)CpmTablePpi->CommonFunction.GetTablePtr2(CpmTablePpi, HYEX_CPM_SIGNATURE_HSIO_TOPOLOGY_S1);
  Ctx.Gpio = (HYGON_CPM_GPIO_INIT_TABLE*)CpmTablePpi->CommonFunction.GetTablePtr2(CpmTablePpi, HYEX_CPM_SIGNATURE_GPIO_INIT);
  Ctx.SetupHob = GetSetupDataHobData();
  Ctx.Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  DEBUG((EFI_D_INFO, "s0:%x s1:%x gpio:%x setup:%x\n", Ctx.s0, Ctx.s1, Ctx.Gpio, Ctx.SetupHob));

  GetHsioGpioStatus(&Ctx);
  UpdateHsioTable(&Ctx);
  Ctx.CtxInfo->CtxCount = Ctx.PcieCtxIndex;
  DEBUG((EFI_D_INFO, "GpioType:%d CtxCount:%d\n", Ctx.CtxInfo->GpioType, Ctx.CtxInfo->CtxCount));  
  

  UpdatePcieSlotId(Ctx.s0, &Ctx.SlotId);
  UpdatePcieSlotId(Ctx.s1, &Ctx.SlotId);

  SyncSlotId2PcieCtx(&Ctx);

  UpdatePcieAspm(Ctx.s0, Ctx.SetupHob);
  UpdatePcieAspm(Ctx.s1, Ctx.SetupHob);

  DisableUnusedLane(&Ctx);

  DumpDxioSetting(Ctx.s0);
  DumpDxioSetting(Ctx.s1);
  
  return EFI_SUCCESS; 
}



