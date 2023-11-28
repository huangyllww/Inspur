/* $NoKeywords:$ */
/**
 * @file
 *
 * DxioCfgPointsZP - Configuration entry points for the DXIO subsystem
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  AmdNbioBaseZPPei
 * @e \$Revision: 312065 $   @e \$Date: 2015-01-30 04:23:05 -0600 (Fri, 30 Jan 2015) $
 *
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 ******************************************************************************
 */
#include <AMD.h>
#include <Gnb.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbDxio.h>
#include <GnbRegistersZP.h>

//From AgesaPkg
#include <AmdPcieComplex.h>
#include <Ppi/NbioPcieComplexPpi.h>


#include "AmdNbioPcieZPPei.h"

#include <Guid/GnbPcieInfoHob.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/NbioPcieTrainingPpi.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/SocLogicalIdPpi.h>
#include <Ppi/NbioPcieDpcStatusPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/AmdBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioHandleLib.h>
//Dependent on AgesaPkg
#include <Library/DxioLibV1.h>
#include <IdsHookId.h>
#include <Library/AmdIdsHookLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioAzaliaLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/GnbLib.h>
#include <CcxRegistersZP.h>

#define FILECODE        NBIO_PCIE_AMDNBIOPCIEZPPEI_DXIOCFGPOINTSZP_FILECODE

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
DxioCfgBeforeReconfigCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgBeforeReconfigCallback Enter\n");
  return;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure DXIO/PCIe ports after ports are mapped and before reconfig
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
DxioCfgBeforeReconfig (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG        *PcieEngine;
  PCIe_WRAPPER_CONFIG       *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgBeforeReconfig Enter\n");
  IDS_HOOK (IDS_HOOK_NBIO_PCIE_TOPOLOGY, (VOID *) GnbHandle, (VOID *) NULL);

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      DxioCfgBeforeReconfigCallback (GnbHandle, PcieEngine, NULL );
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

  Address = (Wrapper->WrapId == 0) ? SMN_0x111402ec_ADDRESS : SMN_0x112402ec_ADDRESS;
  Address += (MappingPortID * 0x1000);
  if (PcdGetBool(PcdAmdNbioSpcMode2P5GT)) {
    NbioRegisterRMW(
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32)~(SMN_0x112402ec_LC_SPC_MODE_2P5GT_MASK),
      (1 << SMN_0x111402ec_LC_SPC_MODE_2P5GT_OFFSET),
      0
      );
  } else {
    NbioRegisterRMW(
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32)~(SMN_0x112402ec_LC_SPC_MODE_2P5GT_MASK),
      (0 << SMN_0x111402ec_LC_SPC_MODE_2P5GT_OFFSET),
      0
      );
  }
  if (PcdGetBool(PcdAmdNbioSpcMode5GT)) {
    NbioRegisterRMW(
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32)~(SMN_0x112402ec_LC_SPC_MODE_5GT_MASK),
      (1 << SMN_0x111402ec_LC_SPC_MODE_5GT_OFFSET),
      0
      );
  } else {
    NbioRegisterRMW(
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32)~(SMN_0x112402ec_LC_SPC_MODE_5GT_MASK),
      (0 << SMN_0x111402ec_LC_SPC_MODE_5GT_OFFSET),
      0
    );
  }
  Address = (Wrapper->WrapId == 0) ? SMN_0x11140284_ADDRESS : SMN_0x11240284_ADDRESS;
  Address += (MappingPortID * 0x1000);
  if (PcdGetBool(PcdCfgPcieLoopbackMode)) {
    if (Engine->Type.Port.PortData.LinkHotplug && Engine->Type.Port.PortData.PortPresent) {
      NbioRegisterRMW(
      GnbHandle,
      SMN_0x11X40284_TYPE,
      Address,
      (UINT32)~(SMN_0x11X40284_BIT13_MASK),
      (1 << SMN_0x11X40284_BIT13_OFFSET),
       0
      );
    }
  } else {
    NbioRegisterRMW(
    GnbHandle,
    SMN_0x11X40284_TYPE,
    Address,
    (UINT32)~(SMN_0x11X40284_BIT13_MASK),
    (0 << SMN_0x11X40284_BIT13_OFFSET),
     0
    );
  }

  if (Engine->Type.Port.PortData.LinkHotplug && Engine->Type.Port.PortData.PortPresent) {
    Value = PcdGet16(PcdCfgPcieCrsDelay);
    NbioRegisterRMW(
    GnbHandle,
    SMN_0x13B10028_TYPE,
    SMN_0x13B10028_ADDRESS,
    (UINT32)~(SMN_0x13B10028_CrsDelayCount_MASK),
    (Value << SMN_0x13B10028_CrsDelayCount_OFFSET),
     0
    );
    Value = PcdGet16(PcdCfgPcieCrsLimit);
    NbioRegisterRMW(
    GnbHandle,
    SMN_0x13B10028_TYPE,
    SMN_0x13B10028_ADDRESS,
    (UINT32)~(SMN_0x13B10028_CrsLimitCount_MASK),
    (Value << SMN_0x13B10028_CrsLimitCount_OFFSET),
     0
    );
  }

  if (PcdGetBool(PcdCfgDisableRcvrResetCycle)) {
    Address = (Wrapper->WrapId == 0) ? SMN_0x1220b0c8_ADDRESS : SMN_0x1230b0c8_ADDRESS;
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

  if (PcdGet8(PcdPcieEqSearchMode) == DxioEqPresetSearchPreset) {
    Address = (Wrapper->WrapId == 0) ? SMN_0x111402d8_ADDRESS : SMN_0x112402d8_ADDRESS;
    Address += (MappingPortID * 0x1000);

    Value = PcdGet8(PcdPcieEqSearchPreset);
    if (Value > 9) {
      Value = Engine->Type.Port.EqPreset;
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "EqPreset: %d\n", Value);
    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32)~(SMN_0x111402d8_LC_FORCE_PRESET_IN_EQ_REQ_PHASE_MASK |
          SMN_0x111402d8_LC_FORCE_PRESET_VALUE_MASK),
      (1 << SMN_0x111402d8_LC_FORCE_PRESET_IN_EQ_REQ_PHASE_OFFSET) |
      (Value << SMN_0x111402d8_LC_FORCE_PRESET_VALUE_OFFSET),
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
DxioCfgAfterReconfigCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCI_ADDR                            PciAddress;
  UINT16                              Index;
  PCIE_LANE_EQUALIZATION_CNTL_STRUCT  LaneEq;
  UINT32                              Value32;

  if (PcieLibIsEngineAllocated(Engine)) {
    IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgAfterReconfigCallback Enter\n");
    IDS_HDT_CONSOLE (GNB_TRACE, "DsTxPreset = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.DsTxPreset);
    IDS_HDT_CONSOLE (GNB_TRACE, "DsRxPresetHint = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint);
    IDS_HDT_CONSOLE (GNB_TRACE, "UsTxPreset = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.UsTxPreset);
    IDS_HDT_CONSOLE (GNB_TRACE, "UsRxPresetHint = 0x%x\n", Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint);

    PcieSetSPC (GnbHandle, Engine);

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
  }
  return;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure DXIO/PCIe ports after reconfig and before training
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
DxioCfgAfterReconfig (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG              *PcieEngine;
  PCIe_WRAPPER_CONFIG             *PcieWrapper;
  EFI_STATUS                      Status;
  UINT32                          Index;
  UINT32                          Value;
  PCIe_DPC_STATUS_DATA            *DpcStatusData;
  PEI_AMD_NBIO_PCIE_DPCSTATUS_PPI *DpcStatusPpi;
  EFI_PEI_SERVICES                **PeiServices;

  IDS_HDT_CONSOLE (GNB_TRACE, "DxioCfgAfterReconfig Enter\n");
  DpcStatusData = NULL;
  DpcStatusPpi = NULL;
  Value = 0;
  PeiServices = (EFI_PEI_SERVICES **) GetPeiServicesTablePointer ();

  Status = (*PeiServices)->LocatePpi (PeiServices, &gAmdNbioPcieDpcStatusPpiGuid, 0, NULL, &DpcStatusPpi);
  if (!EFI_ERROR (Status)) {
    DpcStatusPpi->GetDpcStatus (DpcStatusPpi, (PCIe_DPC_STATUS_DATA **)&DpcStatusData);
  }

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      DxioCfgAfterReconfigCallback (GnbHandle, PcieEngine, NULL );
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    for (Index = 0; Index < 8; Index++) {
      if (DpcStatusData != NULL) {
        NbioRegisterRead (GnbHandle,
                          TYPE_SMN,
                          (PcieWrapper->WrapId == 0) ?  (PCIE0_DPC_STATUS_ADDRESS | (Index << 12)) : (PCIE1_DPC_STATUS_ADDRESS | (Index << 12)),
                          &Value,
                          0
                          );
        IDS_HDT_CONSOLE (GNB_TRACE, "PCIE_DPC_STATUS(0x388) = 0x%x\n", Value);
        if (Value & BIT0) {
          if (DpcStatusData->size < MAX_NUMBER_DPCSTATUS) {
            DpcStatusData->DpcStatusArray[DpcStatusData->size].DieID = (UINT8) GnbHandle->InstanceId;
            DpcStatusData->DpcStatusArray[DpcStatusData->size].BusNumber = (UINT8) GnbHandle->Address.Address.Bus;
            DpcStatusData->DpcStatusArray[DpcStatusData->size].PCIeCoreID = (UINT8) PcieWrapper->WrapId;
            DpcStatusData->DpcStatusArray[DpcStatusData->size].PCIePortID = (UINT8) Index;
            DpcStatusData->DpcStatusArray[DpcStatusData->size].DpcStatus = (UINT16) Value;
            IDS_HDT_CONSOLE (GNB_TRACE, "DpcStatus[%d].DieID=%d\n", DpcStatusData->size, DpcStatusData->DpcStatusArray[DpcStatusData->size].DieID);
            IDS_HDT_CONSOLE (GNB_TRACE, "DpcStatus[%d].BusNumber=%d\n", DpcStatusData->size, DpcStatusData->DpcStatusArray[DpcStatusData->size].BusNumber);
            IDS_HDT_CONSOLE (GNB_TRACE, "DpcStatus[%d].PCIeCoreID=%d\n", DpcStatusData->size, DpcStatusData->DpcStatusArray[DpcStatusData->size].PCIeCoreID);
            IDS_HDT_CONSOLE (GNB_TRACE, "DpcStatus[%d].PCIePortID=%d\n", DpcStatusData->size, DpcStatusData->DpcStatusArray[DpcStatusData->size].PCIePortID);
            IDS_HDT_CONSOLE (GNB_TRACE, "DpcStatus[%d].DpcStatus=0x%x\n", DpcStatusData->size, DpcStatusData->DpcStatusArray[DpcStatusData->size].DpcStatus);

            NbioRegisterWrite (GnbHandle,
                               TYPE_SMN,
                               (PcieWrapper->WrapId == 0) ?  (PCIE0_DPC_STATUS_ADDRESS | (Index << 12)) : (PCIE1_DPC_STATUS_ADDRESS | (Index << 12)),
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
  return;
}


