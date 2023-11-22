/* $NoKeywords:$ */
/**
 * @file
 *
 * HsioCfgPoints - Configuration entry points for the HSIO subsystem
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HygonNbioBasePei
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
 * 
 * HYGON is granting you permission to use this software (the Materials)
 * pursuant to the terms and conditions of your Software License Agreement
 * with HYGON.  This header does *NOT* give you permission to use the Materials
 * or any rights under HYGON's intellectual property.  Your use of any portion
 * of these Materials shall constitute your acceptance of those terms and
 * conditions.  If you do not agree to the terms and conditions of the Software
 * License Agreement, please do not use any portion of these Materials.
 * 
 * CONFIDENTIALITY:  The Materials and all other information, identified as
 * confidential and provided to you by HYGON shall be kept confidential in
 * accordance with the terms and conditions of the Software License Agreement.
 * 
 * LIMITATION OF LIABILITY: THE MATERIALS AND ANY OTHER RELATED INFORMATION
 * PROVIDED TO YOU BY HYGON ARE PROVIDED "AS IS" WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTY OF ANY KIND, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, TITLE, FITNESS FOR ANY PARTICULAR PURPOSE,
 * OR WARRANTIES ARISING FROM CONDUCT, COURSE OF DEALING, OR USAGE OF TRADE.
 * IN NO EVENT SHALL HYGON OR ITS LICENSORS BE LIABLE FOR ANY DAMAGES WHATSOEVER
 * (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF PROFITS, BUSINESS
 * INTERRUPTION, OR LOSS OF INFORMATION) ARISING OUT OF HYGON'S NEGLIGENCE,
 * GROSS NEGLIGENCE, THE USE OF OR INABILITY TO USE THE MATERIALS OR ANY OTHER
 * RELATED INFORMATION PROVIDED TO YOU BY HYGON, EVEN IF HYGON HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES.  BECAUSE SOME JURISDICTIONS PROHIBIT THE
 * EXCLUSION OR LIMITATION OF LIABILITY FOR CONSEQUENTIAL OR INCIDENTAL DAMAGES,
 * THE ABOVE LIMITATION MAY NOT APPLY TO YOU.
 * 
 * HYGON does not assume any responsibility for any errors which may appear in
 * the Materials or any other related information provided to you by HYGON, or
 * result from use of the Materials or any related information.
 * 
 * You agree that you will not reverse engineer or decompile the Materials.
 * 
 * NO SUPPORT OBLIGATION: HYGON is not obligated to furnish, support, or make any
 * further information, software, technical information, know-how, or show-how
 * available to you.  Additionally, HYGON retains the right to modify the
 * Materials at any time, without notice, and is not obligated to provide such
 * modified Materials to you.
 * 
 * 
 ******************************************************************************
 */
#include <HYGON.h>
#include <Gnb.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbHsio.h>
#include <GnbRegisters.h>

//From HgpiPkg
#include <HygonPcieComplex.h>
#include <Ppi/NbioPcieComplexPpi.h>


#include "HygonNbioPciePei.h"

#include <Guid/GnbPcieInfoHob.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/NbioPcieTrainingPpi.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/SocLogicalIdPpi.h>
#include <Ppi/NbioPcieDpcStatusPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioHandleLib.h>
//Dependent on HgpiPkg
#include <Library/HsioLib.h>
#include <IdsHookId.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioAzaliaLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/GnbLib.h>
#include <CcxRegistersDm.h>

#define FILECODE        NBIO_PCIE_HYEX_HYGONNBIOPCIESTPEI_HSIOCFGPOINTSST_FILECODE

/*----------------------------------------------------------------------------------------
 *                         E X T E R N   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

// Comment this line to enable extended debug output if tracing is enabled
#undef GNB_TRACE_ENABLE
#define FULL_EQ_MODE             0
#define BYPASS_EQ_MODE           1
#define NO_NEEDED_EQ_MODE        2
/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */


//=========================================================================================
// Timepoint after port mapping and before reconfig
//=========================================================================================

/*----------------------------------------------------------------------------------------*/
/**
 * Per-Engine Callback for
 *
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
HsioCfgBeforeReconfigCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "HsioCfgBeforeReconfigCallback Enter\n");
  return;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure HSIO/PCIe ports after ports are mapped and before reconfig
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    HGPI_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
HsioCfgBeforeReconfig (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG        *PcieEngine;
  PCIe_WRAPPER_CONFIG       *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "HsioCfgBeforeReconfig Enter\n");
  IDS_HOOK (IDS_HOOK_NBIO_PCIE_TOPOLOGY, (VOID *) GnbHandle, (VOID *) NULL);

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      HsioCfgBeforeReconfigCallback (GnbHandle, PcieEngine, NULL );
      CommonPcieEqConfigurationBeforeReconfig (GnbHandle, PcieEngine);
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  return;
}

VOID
PcieSetSPC (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine
  )
{
  PCIe_WRAPPER_CONFIG               *Wrapper;
  UINT32                            Address;
  UINT32                            Value;
  UINT8                             MappingPortID;

  Wrapper = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;

  Address = ConvertPciePortAddress(PCIE0_GPP0_LC_CNTL6_ADDRESS_HYEX, GnbHandle, Wrapper, MappingPortID);
  //SPC Mode 2.5GT
  Value = PcdGet8 (PcdHygonNbioSpcMode2P5GT);
  if (Value != 0xff) {
    NbioRegisterRMW(
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32)~(PCIE_LC_CNTL6_LC_SPC_MODE_2P5GT_MASK),
      (UINT32)(Value << PCIE_LC_CNTL6_LC_SPC_MODE_2P5GT_OFFSET),
      0
      );
  }

  //SPC Mode 5GT
  Value = PcdGet8 (PcdHygonNbioSpcMode5GT);
  if (Value != 0xff) {
    NbioRegisterRMW(
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32)~(PCIE_LC_CNTL6_LC_SPC_MODE_5GT_MASK),
      (UINT32)(Value << PCIE_LC_CNTL6_LC_SPC_MODE_5GT_OFFSET),
      0
      );
  }

  Address = ConvertPciePortAddress(PCIE0_GPP0_LC_TRAINING_CNTL_ADDRESS_HYEX, GnbHandle, Wrapper, MappingPortID);
  if (PcdGetBool(PcdCfgPcieLoopbackMode)) {
    if (Engine->Type.Port.PortData.LinkHotplug && Engine->Type.Port.PortData.PortPresent) {
      NbioRegisterRMW(
      GnbHandle,
      PCIE0_GPP0_LC_TRAINING_CNTL_TYPE,
      Address,
      (UINT32)~(PCIE0_GPP0_LC_TRAINING_CNTL_LC_DISABLE_TRAINING_BIT_ARCH_MASK),
      (1 << PCIE0_GPP0_LC_TRAINING_CNTL_LC_DISABLE_TRAINING_BIT_ARCH_OFFSET),
       0
      );
    }
  } else {
    NbioRegisterRMW(
    GnbHandle,
    PCIE0_GPP0_LC_TRAINING_CNTL_TYPE,
    Address,
    (UINT32)~(PCIE0_GPP0_LC_TRAINING_CNTL_LC_DISABLE_TRAINING_BIT_ARCH_MASK),
    (0 << PCIE0_GPP0_LC_TRAINING_CNTL_LC_DISABLE_TRAINING_BIT_ARCH_OFFSET),
     0
    );
  }

  if (PcdGetBool(PcdCfgDisableRcvrResetCycle)) {
    Address = SMN_PCS20_0x12E0b0c8_ADDRESS_HYEX + (UINT32)(GnbHandle->RbId) * 0x100000 + (UINT32)(Wrapper->WrapId) * 0x100000;
    if (Engine->Type.Port.PortData.PortPresent) {
      NbioRegisterRMW(
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32)~(0x00040000),
      (0 << 18),
       0
      );
    }
  }

  if (PcdGet8(PcdPcieEqSearchMode) == HsioEqPresetSearchPreset) {
    Address = ConvertPciePortAddress(PCIE0_GPP0_PCIE_LC_CNTL4_ADDRESS_HYEX, GnbHandle, Wrapper, MappingPortID);

    Value = PcdGet8(PcdPcieEqSearchPreset);
    if (Value > 9) {
      Value = Engine->Type.Port.EqPreset;
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "EqPreset: %d\n", Value);
    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32)~(PCIE_LC_CNTL4_LC_FORCE_PRESET_IN_EQ_REQ_PHASE_MASK |
          PCIE_LC_CNTL4_LC_FORCE_PRESET_VALUE_MASK),
      (1 << PCIE_LC_CNTL4_LC_FORCE_PRESET_IN_EQ_REQ_PHASE_OFFSET) |
      (Value << PCIE_LC_CNTL4_LC_FORCE_PRESET_VALUE_OFFSET),
      0
      );
  }

  return;

}


//=========================================================================================
// Timepoint after reconfig and before port training
//=========================================================================================

/*----------------------------------------------------------------------------------------*/
/**
 * Per-Engine Callback for
 *
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
HsioCfgAfterReconfigCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  //PCI_ADDR                            PciAddress;
  //UINT16                              Index;
  //PCIE_LANE_EQUALIZATION_CNTL_STRUCT  LaneEq;
  //UINT32                              Value32;

  if (PcieLibIsEngineAllocated(Engine)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "HsioCfgAfterReconfigCallback Enter\n");
    IDS_HDT_CONSOLE (GNB_TRACE, "DsTxPreset = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.DsTxPreset);
    IDS_HDT_CONSOLE (GNB_TRACE, "DsRxPresetHint = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint);
    IDS_HDT_CONSOLE (GNB_TRACE, "UsTxPreset = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.UsTxPreset);
    IDS_HDT_CONSOLE (GNB_TRACE, "UsRxPresetHint = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint);

    PcieSetSPC (GnbHandle, Engine);
#if 0
    PciAddress.AddressValue = Engine->Type.Port.Address.AddressValue;
    PciAddress.Address.Register = 0x27C;
    if ((Engine->Type.Port.LaneEqualizationCntl.DsTxPreset != 0) &&
       (Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint != 0) &&
       (Engine->Type.Port.LaneEqualizationCntl.UsTxPreset != 0) &&
       (Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint != 0)) {
      GnbLibPciRead(PciAddress.AddressValue, AccessWidth16, &LaneEq.Value, NULL);
      LaneEq.Field.DOWNSTREAM_PORT_TX_PRESET = Engine->Type.Port.LaneEqualizationCntl.DsTxPreset;
      LaneEq.Field.DOWNSTREAM_PORT_RX_PRESET_HINT = Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint;
      LaneEq.Field.UPSTREAM_PORT_TX_PRESET = Engine->Type.Port.LaneEqualizationCntl.UsTxPreset;
      LaneEq.Field.UPSTREAM_PORT_RX_PRESET_HINT = Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint;
      Value32 = (UINT32) LaneEq.Value + ((UINT32) LaneEq.Value << 16);
      for (Index = 0; Index < 8; Index++) {
        IDS_HDT_CONSOLE (GNB_TRACE, "Writing Address = 0x%x\n", PciAddress.AddressValue);
        GnbLibPciWrite(PciAddress.AddressValue, AccessWidth32, &Value32, NULL);
        PciAddress.Address.Register += 4;
      }
    }
  #endif
  }
  return;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure HSIO/PCIe ports after reconfig and before training
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    HGPI_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
HsioCfgAfterReconfig (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG                 *PcieEngine;
  PCIe_WRAPPER_CONFIG                *PcieWrapper;
  EFI_STATUS                         Status;
  UINT32                             Index;
  UINT32                             Value;
  UINT32                             Address;
  PCIe_DPC_STATUS_DATA               *DpcStatusData;
  PEI_HYGON_NBIO_PCIE_DPCSTATUS_PPI  *DpcStatusPpi;
  EFI_PEI_SERVICES                   **PeiServices;
  GNB_HANDLE                         *LocalHandle;
  UINT32                             SocketId;
  PCIE_LC_CNTL12_STRUCT              PcieLcCntl12;
  LINK_CNTL_32GT_STRUCT              LinkCntl_32GT;
  PCIE_LC_CNTL8_STRUCT               LcCntl8;
  UINT32                             SmnAddress;
  UINT8                              MappingPortID;
  UINT32                             LinkSpeed;
  UINT32                             LinkStatus;

  IDS_HDT_CONSOLE (GNB_TRACE, "%a for Socket %d\n", __FUNCTION__, GnbHandle->SocketId);
  DpcStatusData = NULL;
  DpcStatusPpi  = NULL;
  Value = 0;
  PeiServices = (EFI_PEI_SERVICES **)GetPeiServicesTablePointer ();

  Status = (*PeiServices)->LocatePpi (PeiServices, &gHygonNbioPcieDpcStatusPpiGuid, 0, NULL, &DpcStatusPpi);
  if (!EFI_ERROR (Status)) {
    DpcStatusPpi->GetDpcStatus (DpcStatusPpi, (PCIe_DPC_STATUS_DATA **)&DpcStatusData);
  }

  LocalHandle = GnbHandle;
  SocketId    = GnbHandle->SocketId;
  while (LocalHandle != NULL) {
    if (LocalHandle->SocketId == SocketId) {
      PcieWrapper = PcieConfigGetChildWrapper (LocalHandle);
      while (PcieWrapper != NULL) {
        PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
        while (PcieEngine != NULL) {
          HsioCfgAfterReconfigCallback (LocalHandle, PcieEngine, NULL);
          CommonPcieEqConfigurationBeforeTraining (LocalHandle, PcieEngine);
          CustomerPcieEqConfiguration (LocalHandle, PcieEngine, AFTER_RECONFIG, NULL);
          MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
          if (PcieWrapper->WrapId < 2) {
            // Only set Precoding for PCIE Core 0 and Core 1 ports
            if(PcdGetBool (PcdPcie50PrecodingEn)) {
              //Enable Precoding
              SmnAddress = ConvertPciePortAddress (PCIE0_PCIE_LC_CNTL12_ADDRESS_HYEX, LocalHandle, PcieWrapper, MappingPortID);
              NbioRegisterRead(LocalHandle, TYPE_SMN, SmnAddress, &PcieLcCntl12.Value, 0);
              IDS_HDT_CONSOLE (GNB_TRACE, "Before PrecodingEn Socket%x NBIO%x PCIeCore%x = %x \n",LocalHandle->SocketId, LocalHandle->RbId, PcieWrapper->WrapId, PcieLcCntl12.Value);
              PcieLcCntl12.Field.LC_TRANSMITTER_PRECODE_REQUEST_32GT = 1;
              NbioRegisterWrite(LocalHandle, TYPE_SMN, SmnAddress, &PcieLcCntl12.Value, 0);
              IDS_HDT_CONSOLE (GNB_TRACE, "After PrecodingEn Socket%x NBIO%x PCIeCore%x = %x \n",LocalHandle->SocketId, LocalHandle->RbId, PcieWrapper->WrapId, PcieLcCntl12.Value);
            }
            
            switch(PcdGet8(PcdPcie50ByPassEQ)){
              case FULL_EQ_MODE:
                // Set PCIe full Eq Mode by default   
                SmnAddress = ConvertPciePortAddress (PCIE0_LINK_CNTL_32GT_ADDRESS_HYEX, LocalHandle, PcieWrapper, MappingPortID);
                NbioRegisterRead(LocalHandle, TYPE_SMN, SmnAddress, &LinkCntl_32GT.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before Full_Eq_Mode Socket%x NBIO%x PCIeCore%x =%x \n",LocalHandle->SocketId, LocalHandle->RbId, PcieWrapper->WrapId, LinkCntl_32GT.Value);
                LinkCntl_32GT.Field.EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE = 1;
                LinkCntl_32GT.Field.NO_EQUALIZATION_NEEDED_DISABLE = 1;
                NbioRegisterWrite(LocalHandle, TYPE_SMN, SmnAddress, &LinkCntl_32GT.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "After Full_Eq_Mode Socket%x NBIO%x PCIeCore%x =%x \n",LocalHandle->SocketId, LocalHandle->RbId, PcieWrapper->WrapId, LinkCntl_32GT.Value); 
                break;
                  
              case BYPASS_EQ_MODE:
                //Enable Bypass Eq Mode
                SmnAddress = ConvertPciePortAddress (PCIE0_LINK_CNTL_32GT_ADDRESS_HYEX, LocalHandle, PcieWrapper, MappingPortID);
                NbioRegisterRead(LocalHandle, TYPE_SMN, SmnAddress, &LinkCntl_32GT.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before Bypass_Eq_Mode Socket%x NBIO%x PCIeCore%x =%x \n",LocalHandle->SocketId,LocalHandle->RbId, PcieWrapper->WrapId, LinkCntl_32GT.Value); 
                LinkCntl_32GT.Field.EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE = 0;
                LinkCntl_32GT.Field.NO_EQUALIZATION_NEEDED_DISABLE = 1;
                NbioRegisterWrite(LocalHandle, TYPE_SMN, SmnAddress, &LinkCntl_32GT.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "After Bypass_Eq_Mode Socket%x NBIO%x PCIeCore%x =%x \n",LocalHandle->SocketId,LocalHandle->RbId, PcieWrapper->WrapId, LinkCntl_32GT.Value); 
                break;
                  
              case NO_NEEDED_EQ_MODE:
                //Enable No_Eq_Needed Mode
                SmnAddress = ConvertPciePortAddress (PCIE0_LINK_CNTL_32GT_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
                NbioRegisterRead(LocalHandle, TYPE_SMN, SmnAddress, &LinkCntl_32GT.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before No_Needed_Eq_Mode Socket%x NBIO%x PCIeCore%x =%x \n",LocalHandle->SocketId,LocalHandle->RbId, PcieWrapper->WrapId, LinkCntl_32GT.Value);
                LinkCntl_32GT.Field.EQUALIZATION_BYPASS_TO_HIGHEST_RATE_DISABLE = 0;
                LinkCntl_32GT.Field.NO_EQUALIZATION_NEEDED_DISABLE = 0;
                NbioRegisterWrite(LocalHandle, TYPE_SMN, SmnAddress, &LinkCntl_32GT.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "After No_Needed_Eq_Mode Socket%x NBIO%x PCIeCore%x =%x \n",LocalHandle->SocketId,LocalHandle->RbId, PcieWrapper->WrapId, LinkCntl_32GT.Value); 
                break;
                
              default:
                break;
            } 
            //PCIe 4Symbol Control for Gen4
            if(PcdGet8(PcdPcie4Symbol)){
              SmnAddress = ConvertPciePortAddress (PCIE0_LINK_STATUS_SMN_ADDRESS_HYEX, LocalHandle, PcieWrapper, MappingPortID);
              NbioRegisterRead(LocalHandle,TYPE_SMN,SmnAddress,&LinkStatus,0);
              //read current link speed
              LinkSpeed = LinkStatus&0xF;
            
              //if(LinkSpeed == 4){
                MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                SmnAddress = ConvertPciePortAddress (PCIE0_PCIE_LC_CNTL8_ADDRESS_HYEX, LocalHandle, PcieWrapper, MappingPortID);
                NbioRegisterRead(LocalHandle, TYPE_SMN, SmnAddress, &LcCntl8.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before_LC_SPC_MODE_16GT=%x\n",LcCntl8.Value);
                LcCntl8.Field.LC_SPC_MODE_16GT = 2;
                NbioRegisterWrite(LocalHandle, TYPE_SMN, SmnAddress, &LcCntl8.Value, 0);  
                IDS_HDT_CONSOLE (GNB_TRACE, "After_LC_SPC_MODE_16GT=%x\n",LcCntl8.Value);
             // }
            }else{
                MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
                SmnAddress = ConvertPciePortAddress (PCIE0_PCIE_LC_CNTL8_ADDRESS_HYEX, LocalHandle, PcieWrapper, MappingPortID);
                NbioRegisterRead(LocalHandle, TYPE_SMN, SmnAddress, &LcCntl8.Value, 0);
                IDS_HDT_CONSOLE (GNB_TRACE, "Before_LC_SPC_MODE_16GT=%x\n",LcCntl8.Value);
                LcCntl8.Field.LC_SPC_MODE_16GT = 1;
                NbioRegisterWrite(LocalHandle, TYPE_SMN, SmnAddress, &LcCntl8.Value, 0);  
                IDS_HDT_CONSOLE (GNB_TRACE, "After_LC_SPC_MODE_16GT=%x\n",LcCntl8.Value);
            }
          }
          PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
        }
        
        if (PcieWrapper->WrapId < 2) {        
          // Only enable tag10 for PCIE Core 0 and Core 1 ports
          IDS_HDT_CONSOLE (GNB_TRACE, "    Enable 10 bits tag for PCIE core %d ports \n", PcieWrapper->WrapId);
          for( Index = 0; Index < 8; Index++) {
            NbioRegisterRMW (
              LocalHandle,
              TYPE_SMN,
              ConvertPciePortAddress (PCIE0_DEVICE_CNTL2_ADDRESS_HYEX, GnbHandle, PcieWrapper, (UINT8)Index),
              (UINT32) ~(DEVICE_CNTL2_TEN_BIT_TAG_REQUESTER_ENABLE_MASK),
              0 << DEVICE_CNTL2_TEN_BIT_TAG_REQUESTER_ENABLE_OFFSET,
              0
              );
          }
        }
        
        for (Index = 0; Index < 8; Index++) {
          if (DpcStatusData != NULL) {
            Address = ConvertPciePortAddress (PCIE0_DPC_STATUS_ADDRESS_HYEX, LocalHandle, PcieWrapper, (UINT8)Index);
            NbioRegisterRead (
              LocalHandle,
              TYPE_SMN,
              Address,
              &Value,
              0
              );
            IDS_HDT_CONSOLE (GNB_TRACE, "PCIE_DPC_STATUS(0x388) = 0x%x\n", Value);
            if (Value & BIT0) {
              if (DpcStatusData->size < MAX_NUMBER_DPCSTATUS) {
                DpcStatusData->DpcStatusArray[DpcStatusData->size].SocketId   = (UINT8)LocalHandle->SocketId;
                DpcStatusData->DpcStatusArray[DpcStatusData->size].RBIndex    = (UINT8)LocalHandle->RbId;
                DpcStatusData->DpcStatusArray[DpcStatusData->size].BusNumber  = (UINT8)LocalHandle->Address.Address.Bus;
                DpcStatusData->DpcStatusArray[DpcStatusData->size].PCIeCoreID = (UINT8)PcieWrapper->WrapId;
                DpcStatusData->DpcStatusArray[DpcStatusData->size].PCIePortID = (UINT8)Index;
                DpcStatusData->DpcStatusArray[DpcStatusData->size].DpcStatus  = (UINT16)Value;
                IDS_HDT_CONSOLE (
                  GNB_TRACE,
                  "DpcStatus[%d].SocketId=%d\n",
                  DpcStatusData->size,
                  DpcStatusData->DpcStatusArray[DpcStatusData->size].SocketId
                  );
                IDS_HDT_CONSOLE (
                  GNB_TRACE,
                  "DpcStatus[%d].RBIndex=%d\n",
                  DpcStatusData->size,
                  DpcStatusData->DpcStatusArray[DpcStatusData->size].RBIndex
                  );
                IDS_HDT_CONSOLE (
                  GNB_TRACE,
                  "DpcStatus[%d].BusNumber=%d\n",
                  DpcStatusData->size,
                  DpcStatusData->DpcStatusArray[DpcStatusData->size].BusNumber
                  );
                IDS_HDT_CONSOLE (
                  GNB_TRACE,
                  "DpcStatus[%d].PCIeCoreID=%d\n",
                  DpcStatusData->size,
                  DpcStatusData->DpcStatusArray[DpcStatusData->size].PCIeCoreID
                  );
                IDS_HDT_CONSOLE (
                  GNB_TRACE,
                  "DpcStatus[%d].PCIePortID=%d\n",
                  DpcStatusData->size,
                  DpcStatusData->DpcStatusArray[DpcStatusData->size].PCIePortID
                  );
                IDS_HDT_CONSOLE (
                  GNB_TRACE,
                  "DpcStatus[%d].DpcStatus=0x%x\n",
                  DpcStatusData->size,
                  DpcStatusData->DpcStatusArray[DpcStatusData->size].DpcStatus
                  );

                NbioRegisterWrite (
                  LocalHandle,
                  TYPE_SMN,
                  Address,
                  &Value,
                  0
                  );

                DpcStatusData->size++;
              }
            }
          }
        }

        PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
      }
    }
    //Set CRS Delay/Limit
    NbioRegisterRMW(
      LocalHandle,
      IOHC_PCIE_CRS_Count_TYPE,
      NBIO_SPACE(LocalHandle, IOHC_PCIE_CRS_Count_ADDRESS_HYEX),
      (UINT32)~(IOHC_PCIE_CRS_Count_CrsDelayCount_MASK),
      (UINT32)(PcdGet16(PcdCfgPcieCrsDelay) << IOHC_PCIE_CRS_Count_CrsDelayCount_OFFSET),
      0
      );
    NbioRegisterRMW(
      LocalHandle,
      IOHC_PCIE_CRS_Count_TYPE,
      NBIO_SPACE(LocalHandle, IOHC_PCIE_CRS_Count_ADDRESS_HYEX),
      (UINT32)~(IOHC_PCIE_CRS_Count_CrsLimitCount_MASK),
      (UINT32)(PcdGet16(PcdCfgPcieCrsDelay) << IOHC_PCIE_CRS_Count_CrsLimitCount_OFFSET),
      0
      );
    LocalHandle = GnbGetNextHandle (LocalHandle);
  }

  return;
}
