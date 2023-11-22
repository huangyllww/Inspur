

#include "PlatformPei.h"
#include <GnbHsio.h>
#include <Library/PlatformCommLib.h>
#include <Library/ByoHygonFabricLib.h>
#include <Library/PcieConfigLib.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Guid/GnbPcieInfoHob.h>
#include <SetupVariable.h>
#include <GnbRegisters.h>
#include <Library/NbioRegisterAccLib.h>


typedef struct {
  PLATFORM_COMM_INFO  *Info;
  UINT32              GfxRpCount;
  PCIe_ENGINE_CONFIG  *IgdRpEngine;
  PCIe_ENGINE_CONFIG  *GfxRpEngine[8];
} GFX_DIS_CTX;



STATIC EFI_GUID gByoGfxDisCtxHobGuid = { 0x241b4de1, 0x8104, 0x4b3c, { 0x93, 0x90, 0xa3, 0x54, 0x6a, 0x90, 0x4c, 0xd8 } };



VOID
STATIC
PcieSlotCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  GNB_HANDLE            *NbioHandle;
  GFX_DIS_CTX           *Ctx;  
  BYO_PEI_PCIE_INFO     PcieInfo;

  if(!(Engine->InitStatus & INIT_STATUS_PCIE_TRAINING_SUCCESS)){
    return;
  }
  if(!Engine->Type.Port.PortData.PortPresent){
    return;
  }

  Ctx        = (GFX_DIS_CTX*)Buffer;  
  NbioHandle = (GNB_HANDLE*)PcieConfigGetParentSilicon(Engine);

  LibGetPcieSubDeviceInfo(NbioHandle, Engine, &PcieInfo);
  DEBUG((EFI_D_INFO, "PciId:%x\n", PcieInfo.PciId));

  if(Ctx->Info->IgdSocket != 0xFF){
    if(NbioHandle->SocketId == Ctx->Info->IgdSocket &&
       Ctx->Info->IgdStartLane == Engine->EngineData.StartLane){
      DEBUG((EFI_D_INFO, "IsIGD\n"));
      Ctx->IgdRpEngine = Engine;
      PcdSet16S(PcdIgdRpBDF, (PcieInfo.PortBus << 8) | (PcieInfo.PortDev << 3) | PcieInfo.PortFun);
      return;
    }
  }

  if(IsGfxClassCode(PcieInfo.ClassCode, PcieInfo.PciId)){
    if(Ctx->GfxRpCount < ARRAY_SIZE(Ctx->GfxRpEngine)){
      Ctx->GfxRpEngine[Ctx->GfxRpCount++] = Engine;
    }
    if(Ctx->Info->AddOnGfxPciId == 0){
      Ctx->Info->AddOnGfxPciId = PcieInfo.PciId;
    }
    DEBUG((EFI_D_INFO, "AddOn Gfx Found\n"));
  } 
}


#define PCIE_ROOT_PORT_DIS_MASK   \
  ((UINT32)~(IOHC_BRIDGE_CNTL_BridgeDis_MASK | IOHC_BRIDGE_CNTL_CfgDis_MASK | IOHC_BRIDGE_CNTL_CrsEnable_MASK))

#define PCIE_ROOT_PORT_DIS_VALUE \
  ((1 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET) | (1 << IOHC_BRIDGE_CNTL_CfgDis_OFFSET) | (1 << IOHC_BRIDGE_CNTL_CrsEnable_OFFSET))
  


EFI_STATUS
EFIAPI
PcieTrainingDoneCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                        Status;
  PCIe_PLATFORM_CONFIG              *Pcie;
  PEI_HYGON_NBIO_PCIE_SERVICES_PPI  *PcieServicesPpi;
  GNB_PCIE_INFORMATION_DATA_HOB     *PciePlatformConfigHob;
  GFX_DIS_CTX                       *Ctx;
  CONST SETUP_DATA                  *SetupHob;
  UINTN                             Index;  
  GNB_HANDLE                        *NbioHandle;
  PCIe_ENGINE_CONFIG                *Engine;
  BOOLEAN                           AllowEarlyGop = TRUE;


  Ctx = (GFX_DIS_CTX*)BuildGuidHob(&gByoGfxDisCtxHobGuid, sizeof(GFX_DIS_CTX));
  ASSERT(Ctx != NULL);
  ZeroMem(Ctx, sizeof(GFX_DIS_CTX));
  Ctx->Info = (PLATFORM_COMM_INFO*)GetPlatformCommInfo();

  SetupHob = GetSetupDataHobData();

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );
  PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
  Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);

  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieSlotCallback,
    Ctx,
    Pcie
    );

  if(Ctx->IgdRpEngine == NULL){
    AllowEarlyGop = FALSE;
  }

  if(Ctx->GfxRpCount && Ctx->IgdRpEngine != NULL){
    
    if(SetupHob->VideoPrimaryAdapter == DISPLAY_PRIMARY_IGD){
      for(Index=0;Index<Ctx->GfxRpCount;Index++){
        Engine = Ctx->GfxRpEngine[Index];
        NbioHandle = (GNB_HANDLE*)PcieConfigGetParentSilicon(Engine);
        NbioRegisterRMW (
          NbioHandle,
          TYPE_SMN,
          NBIO_SPACE(NbioHandle, NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYEX) + (Engine->Type.Port.LogicalBridgeId << 10),
          PCIE_ROOT_PORT_DIS_MASK,
          PCIE_ROOT_PORT_DIS_VALUE,
          0   // Change to flags if used in DXE
          );
        Engine->InitStatus &= ~(UINT16)INIT_STATUS_PCIE_TRAINING_SUCCESS;
        DEBUG((EFI_D_INFO, "disable (%x,%x,%x) InitStatus:%d\n", \
          NbioHandle->Address.Address.Bus, Engine->Type.Port.PortData.DeviceNumber, \
          Engine->Type.Port.PortData.FunctionNumber, Engine->InitStatus));
      }
      
    } else if(SetupHob->VideoPrimaryAdapter == DISPLAY_PRIMARY_PCIE){
      Engine = Ctx->IgdRpEngine;
      NbioHandle = (GNB_HANDLE*)PcieConfigGetParentSilicon(Engine);
      NbioRegisterRMW (
        NbioHandle,
        TYPE_SMN,
        NBIO_SPACE(NbioHandle, NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYEX) + (Engine->Type.Port.LogicalBridgeId << 10),
        PCIE_ROOT_PORT_DIS_MASK,
        PCIE_ROOT_PORT_DIS_VALUE,
        0   // Change to flags if used in DXE
        );
      Engine->InitStatus &= ~(UINT16)INIT_STATUS_PCIE_TRAINING_SUCCESS;
      AllowEarlyGop = FALSE;
    } 
    
  }

  if(!AllowEarlyGop){
    PcdSet16S(PcdIgdRpBDF, 0xFFFF);
    DEBUG((EFI_D_INFO, "Dis EarlyGop\n"));
  }

  return EFI_SUCCESS;
}


