/* $NoKeywords:$ */
/**
 * @file
 *
 * HsioInit - Initialize HSIO subsystem
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

// From HgpiPkg
#include <HygonPcieComplex.h>
#include <Ppi/NbioPcieComplexPpi.h>
#include <Ppi/HygonMemoryInfoHobPpi.h>

#include "HygonNbioPciePei.h"

#include <Guid/GnbPcieInfoHob.h>
#include <Ppi/NbioPcieServicesPpi.h>
#include <Ppi/NbioPcieTrainingPpi.h>
#include <Ppi/NbioSmuServicesPpi.h>
#include <Ppi/SocLogicalIdPpi.h>
#include <Ppi/FabricTopologyServicesPpi.h>
#include <Library/PeiServicesLib.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/PcieMiscCommLib.h>
#include <Library/PciLib.h>
#include <Library/GnbInternalPciLib.h>
// Dependent on HgpiPkg
#include <Hsio.h>
#include <IdsHookId.h>
#include <Library/HygonIdsHookLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/NbioAzaliaLib.h>
#include <Library/FabricResourceManagerLib.h>
#include <Library/GnbPciAccLib.h>
#include <Library/GnbLib.h>
#include <Library/SmnAccessLib.h>
#include <Library/IoLib.h>
#include <CcxRegistersDm.h>
#include <Ppi/Stall.h>
#include <Library/TimerLib.h>
#include <Library/HygonSmnAddressLib.h>

#define FILECODE        NBIO_PCIE_HYEX_HYGONNBIOPCIESTPEI_HSIOINITST_FILECODE

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

#define BRIDGE_DEVICE_TYPE       6
#define DISPLAY_CONTROLLER_TYPE  3
#define PCIeConfig_X1            1
#define PCIeConfig_X2            2
#define PCIeConfig_X4            4
#define PCIeConfig_X8            8
#define PCIeConfig_X16           16

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

UINT32    MaxPayloadStrapList[8] = {
  STRAP_BIF_MAX_PAYLOAD_SUPPORT_A_ADDR,
  STRAP_BIF_MAX_PAYLOAD_SUPPORT_B_ADDR,
  STRAP_BIF_MAX_PAYLOAD_SUPPORT_C_ADDR,
  STRAP_BIF_MAX_PAYLOAD_SUPPORT_D_ADDR,
  STRAP_BIF_MAX_PAYLOAD_SUPPORT_E_ADDR,
  STRAP_BIF_MAX_PAYLOAD_SUPPORT_F_ADDR,
  STRAP_BIF_MAX_PAYLOAD_SUPPORT_G_ADDR,
  STRAP_BIF_MAX_PAYLOAD_SUPPORT_H_ADDR
};

UINT32  EnPcie4StrapList[] = {
  STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_A_ADDR,
  STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_A_ADDR,
  STRAP_BIF_LOCAL_DLF_SUPPORTED_A_ADDR,
  STRAP_BIF_DLF_EXCHANGE_EN_A_ADDR
};

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                    P P I   N O T I F Y   D E S C R I P T O R S
 *----------------------------------------------------------------------------------------
 */

STATIC PEI_HYGON_NBIO_PCIE_TRAINING_START_PPI  mNbioPcieTrainingStartPpi = {
  HYGON_TRAINING_START_STATUS          /// Start Status
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mNbioPcieTrainingStartPpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonNbioPcieTrainingStartPpiGuid,
  &mNbioPcieTrainingStartPpi
};

STATIC PEI_HYGON_NBIO_PCIE_TRAINING_DONE_PPI  mNbioPcieTrainingDonePpi = {
  HYGON_TRAINING_DONE_STATUS          /// Done Status
};

STATIC EFI_PEI_PPI_DESCRIPTOR  mNbioPcieTrainingDonePpiList =
{
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gHygonNbioPcieTrainingDonePpiGuid,
  &mNbioPcieTrainingDonePpi
};

#define HsioInputParsetGetNextDescriptor(Descriptor)  (Descriptor != NULL ? ((((Descriptor->Flags & DESCRIPTOR_TERMINATE_LIST) != 0) ? NULL : (++Descriptor))) : NULL)

extern
HGPI_STATUS
HsioEarlyInit (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       HSIO_COMPLEX_DESCRIPTOR         *UserConfig
  );

extern
EFI_STATUS
SetPcieInitialPreset (
  IN     GNB_HANDLE           *GnbHandle,
  IN     PCIe_WRAPPER_CONFIG  *Wrapper,
  IN     UINT8                Speed,
  IN     UINT8                Stream,
  IN     UINT8                Preset
  );

EFI_STATUS
PcieEqCfgInitPhase (
  IN     GNB_HANDLE                 *GnbHandle,
  IN     PCIe_WRAPPER_CONFIG        *Wrapper
  );

/*----------------------------------------------------------------------------------------*/
/**
 * Callback function for clear all PCIe errors that occur during PCIe training
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  NotifyDescriptor  NotifyDescriptor pointer
 * @param[in]  Ppi               Ppi pointer
 * @retval     EFI_STATUS
 */
EFI_STATUS
EFIAPI
ClearPCIeTrainingErrorsCallback (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                        Status;
  GNB_HANDLE                        *GnbHandle;
  PCIe_ENGINE_CONFIG                *PcieEngineList;
  PCIe_WRAPPER_CONFIG               *PcieWrapper;
  PCIe_PLATFORM_CONFIG              *Pcie;
  PEI_HYGON_NBIO_PCIE_SERVICES_PPI    *PcieServicesPpi;
  GNB_PCIE_INFORMATION_DATA_HOB     *PciePlatformConfigHob;

  Status = EFI_SUCCESS;
  IDS_HDT_CONSOLE (PCIE_MISC, "ClearPCIeTrainingErrorsCallback Entry\n");
  if (PcdGetBool(PcdClearPCIeTrainingErrors)) {
    Status = (*PeiServices)->LocatePpi (
                                       PeiServices,
                                       &gHygonNbioPcieServicesPpiGuid,
                                       0,
                                       NULL,
                                       (VOID **)&PcieServicesPpi
                                       );
    PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
    Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);
    //wait 200ms for PCIe link to stabilize
    MicroSecondDelay(200000);
   
    for (GnbHandle = NbioGetHandle (Pcie); GnbHandle != NULL; GnbHandle = GnbGetNextHandle (GnbHandle)) {
      for (PcieWrapper = PcieConfigGetChildWrapper (GnbHandle); PcieWrapper != NULL; PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper)) {
        for (PcieEngineList = PcieConfigGetChildEngine (PcieWrapper); PcieEngineList != NULL; PcieEngineList = PcieLibGetNextDescriptor (PcieEngineList)) {
          if (PcieEngineList->InitStatus == INIT_STATUS_PCIE_TRAINING_SUCCESS) {
            PcieClearAllErrors (GnbHandle, PcieEngineList);
          }  
        }
      }  
    }
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "ClearPCIeTrainingErrorsCallback Exit\n");
  return Status;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Clean up bridge
 *
 *
 *
 * @param[in]     Endpoint       PCI Address of the bridge
 */
VOID
HsioCleanUpEarlyBridge (
  IN       PCI_ADDR              Endpoint
  )
{
  UINT32      Index;
  UINT32      Value;
  PCI_ADDR    Downstream;

  GnbLibPciRead (Endpoint.AddressValue | 0x08, AccessWidth32, &Value, NULL);
  if ((Value >> 24) == DISPLAY_CONTROLLER_TYPE) {
    Value = 0;
    GnbLibPciWrite (Endpoint.AddressValue | 0x04, AccessWidth32, &Value, NULL);
    for (Index = 0x10; Index < 0x28; Index += 4) {
      GnbLibPciWrite (Endpoint.AddressValue | Index, AccessWidth32, &Value, NULL);
    }
  } else if ((Value >> 24) == BRIDGE_DEVICE_TYPE) {
    GnbLibPciRead (Endpoint.AddressValue | 0x18, AccessWidth32, &Value, NULL);
    if ((Value & 0x00FFFF00) != 0) {
      Downstream.AddressValue = MAKE_SBDFO (0, (Value & 0x0000FF00) >> 8, 0, 0, 0);
      HsioCleanUpEarlyBridge (Downstream);
      Value = 0;
      GnbLibPciWrite (Endpoint.AddressValue | 0x1C, AccessWidth16, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x20, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x24, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x28, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x2C, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x30, AccessWidth32, &Value, NULL);
      GnbLibPciWrite (Endpoint.AddressValue | 0x04, AccessWidth32, &Value, NULL);
      GnbLibPciRead (Endpoint.AddressValue | 0x18, AccessWidth32, &Value, NULL);
      Value &= 0xFF0000FF;
      GnbLibPciWrite (Endpoint.AddressValue | 0x18, AccessWidth32, &Value, NULL);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Clean up programming from early BMC initialization
 *
 *
 *
 * @param[in]     GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]     DieNumber      InstanceId of the Die that contains the BMC link
 */
VOID
HsioCleanUpEarlyInit (
  IN       GNB_HANDLE            *GnbHandle,
  IN       UINT32                DieNumber
  )
{
  GNB_HANDLE            *NbioHandle;
  PCIe_ENGINE_CONFIG    *Engine;
  UINT32                Value;
  PCI_ADDR              Endpoint;

  IDS_HDT_CONSOLE (GNB_TRACE, "HsioCleanUpEarlyInit Enter\n");
  NbioHandle = GnbHandle;
  IDS_HDT_CONSOLE (GNB_TRACE, "Looking for Die Number %d\n", DieNumber);
  while (NbioHandle != NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  Found Die Number %d\n", NbioHandle->InstanceId);
    if (NbioHandle->InstanceId == DieNumber) {
      Engine = PcieConfigGetChildEngine (NbioHandle);
      while (Engine != NULL) {
        GnbLibPciRead (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x18), AccessWidth32, &Value, NULL);
        IDS_HDT_CONSOLE (GNB_TRACE, "  Read 0x%x from Device %d Function %d\n", Value, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber);
        if ((Value & 0x00FFFF00) != 0) {
          Endpoint.AddressValue = MAKE_SBDFO (0, (Value & 0x0000FF00) >> 8, 0, 0, 0);
          HsioCleanUpEarlyBridge(Endpoint);
          Value = 0;
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x1C), AccessWidth16, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x20), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x24), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x28), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x2C), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x30), AccessWidth32, &Value, NULL);
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x04), AccessWidth32, &Value, NULL);
          GnbLibPciRead (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x18), AccessWidth32, &Value, NULL);
          Value &= 0xFF0000FF;
          GnbLibPciWrite (MAKE_SBDFO (0, NbioHandle->Address.Address.Bus, Engine->Type.Port.NativeDevNumber, Engine->Type.Port.NativeFunNumber, 0x18), AccessWidth32, &Value, NULL);
        }
        Engine = (PCIe_ENGINE_CONFIG *) PcieConfigGetNextTopologyDescriptor (Engine, DESCRIPTOR_TERMINATE_GNB);
      }
    }
    NbioHandle = GnbGetNextHandle(NbioHandle);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "HsioCleanUpEarlyInit Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enumerate all Pcie connectors for register setting.
 *
 *
 * @param[in]       GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]       Engine         Pointer to engine config descriptor
 * @param[in, out]  Buffer         Not used
 */
VOID
STATIC
PcieIntegratedInterfaceCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIe_WRAPPER_CONFIG               *Wrapper;
  UINT32                            Address;
  UINT8                             MappingPortID;

  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieIntegratedInterfaceCallback Enter\n"));

  Wrapper = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;

  Address = ConvertPciePortAddress(PCIE0_GPP0_TX_CNTL_ADDRESS_HYEX, GnbHandle, Wrapper, MappingPortID);

  NbioRegisterRMW (
    GnbHandle,
    TYPE_SMN,
    Address,
    (UINT32) ~(PCIE_TX_CNTL_TX_FLUSH_TLP_DIS_MASK),
    0 << PCIE_TX_CNTL_TX_FLUSH_TLP_DIS_OFFSET,
    0
    );

  if ((Engine->Type.Port.PortData.LinkSpeedCapability == PcieGen3) ||
      (Engine->Type.Port.PortData.LinkSpeedCapability == PcieGenMaxSupported)) {
    // Gen3 cap enabled
    Engine->Type.Port.LaneEqualizationCntl.DsTxPreset     = 7;
    Engine->Type.Port.LaneEqualizationCntl.DsRxPresetHint = 2;
    Engine->Type.Port.LaneEqualizationCntl.UsTxPreset     = 7;
    Engine->Type.Port.LaneEqualizationCntl.UsRxPresetHint = 2;
  }

  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieIntegratedInterfaceCallback Exit\n"));
}

/*----------------------------------------------------------------------------------------*/
/**
 * After Pcie Training Enumerate all Pcie connectors for register setting.
 *
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
PcieAfterTrainingDoneCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIe_WRAPPER_CONFIG               *Wrapper;
  UINT8                             MappingPortID;
  UINT32                            Address;
  UINT32                            Value;
  UINT16                            SlotNum;
  UINT8                             SlotFlag;

  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieAfterTrainingDoneCallback Enter\n"));

  Address = Engine->Type.Port.Address.AddressValue;
  if (Address != 0) {
    // Set completion timeout
    GnbLibPciRead (Address | 0x80, AccessWidth32, &Value, NULL);
    Value = (Value & 0xfffffff0) | 0x6;
    GnbLibPciWrite (Address | 0x80, AccessWidth32, &Value, NULL);

    // Set slot_implemented
    SlotNum  = Engine->Type.Port.PortData.SlotNum;
    SlotFlag = (SlotNum == 0) ? 0 : 1;

    GnbLibPciRMW (
      Engine->Type.Port.Address.AddressValue | DxF0x58_PCI_OFFSET,
      AccessWidth32,
      0xffffffff,
      SlotFlag << DxF0x58_SlotImplemented_OFFSET,
      (HYGON_CONFIG_PARAMS *) NULL
      );
  }

  Wrapper = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  switch (Engine->EngineData.EndLane - Engine->EngineData.StartLane + 1) {
    // PCIe LC_L1_POWERDOWN Config as 0 when X1/X2
    case PCIeConfig_X1:
    case PCIeConfig_X2:
      Value = 0;
      break;
    // PCIe LC_L1_POWERDOWN Config as 1 when X4/X8/X16
    case PCIeConfig_X4:
    case PCIeConfig_X8:
    case PCIeConfig_X16:
      Value = 1;
      break;
    default:
      break;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PowerOffPhyLanesPllL1 for Wrapper %d Port %d\n", Wrapper->WrapId, MappingPortID);
  //Config LC_CNTL6.LC_L1_PowerDown BIT21
  Address = ConvertPciePortAddress(PCIE0_GPP0_LC_CNTL6_ADDRESS_HYEX, GnbHandle, Wrapper, MappingPortID);
  NbioRegisterRMW (
    GnbHandle,
    TYPE_SMN,
    Address,
    (UINT32)~(PCIE_LC_CNTL6_LC_L1_POWERDOWN_MASK),
    (Value << PCIE_LC_CNTL6_LC_L1_POWERDOWN_OFFSET),
    0
    );
  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieAfterTrainingDoneCallback Exit\n"));
}

/*----------------------------------------------------------------------------------------*/
/**
 * After Pcie Training Enumerate all Pcie connectors for register setting.
 *
 *
 *
 * @param[in]     Engine  Engine configuration info
 * @param[in,out] Buffer  Buffer pointer
 * @param[in]     Pcie    PCIe configuration info
 */
VOID
STATIC
InitBusRanges (
  IN     PCIe_PLATFORM_CONFIG                 *Pcie,
  IN     HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI *FabricTopologyServicesPpi

  )
{
  GNB_HANDLE                      *GnbHandle;
  GNB_HANDLE                      *InnerHandle;
  PCIE_VDM_NODE_CTRL4_STRUCT      VdmNode;
  UINTN                           SystemFabricID;
  UINTN                           BusNumberBase;
  UINTN                           BusNumberLimit;
  UINT32                          VdmNodeAddress;

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    InnerHandle = NbioGetHandle (Pcie);
    while (InnerHandle != NULL) {
      FabricTopologyServicesPpi->GetRootBridgeInfo (InnerHandle->SocketId,
                                            InnerHandle->LogicalDieId,
                                            InnerHandle->RbId,
                                            &SystemFabricID,
                                            &BusNumberBase,
                                            &BusNumberLimit
                                            );
      VdmNode.Field.BUS_RANGE_BASE = BusNumberBase;
      VdmNode.Field.BUS_RANGE_LIMIT = BusNumberLimit;
      if (InnerHandle->InstanceId == GnbHandle->InstanceId) {
        VdmNode.Field.NODE_PRESENT = 0;
      } else {
        VdmNode.Field.NODE_PRESENT = 1;
      }
      VdmNodeAddress = PCIE_VDM_NODE0_CTRL4_ADDRESS_HYEX + (InnerHandle->InstanceId * 0x4);
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         NBIO_SPACE(GnbHandle, VdmNodeAddress),
                         &VdmNode.Value,
                         0
                         );
      InnerHandle = GnbGetNextHandle (InnerHandle);
    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to user engine descriptor
 *
 *
 * @param[in]  EngineDescriptor   Pointer to engine descriptor
 */
VOID
HsioUserDescriptorConfigDump (
  IN      HSIO_PORT_DESCRIPTOR      *EngineDescriptor
  )
{

  IDS_HDT_CONSOLE (PCIE_MISC, "  Engine Type - %a\n",
    (EngineDescriptor->EngineData.EngineType == HsioPcieEngine) ? "PCIe Port" : (
    (EngineDescriptor->EngineData.EngineType == HsioSATAEngine) ? "SATA Port" : (
    (EngineDescriptor->EngineData.EngineType == HsioEthernetEngine) ? "Ethernet Port" : (
    (EngineDescriptor->EngineData.EngineType == HsioUnusedEngine) ? "Unused" : "Invalid")))
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Start Phy Lane - %d\n    End   Phy Lane - %d\n",
    EngineDescriptor->EngineData.StartLane,
    EngineDescriptor->EngineData.EndLane
  );
  IDS_HDT_CONSOLE (PCIE_MISC, "    Hotplug - %d\n", EngineDescriptor->EngineData.HotPluggable);
  IDS_HDT_CONSOLE (PCIE_MISC, "    GpioGroupId - %d\n", EngineDescriptor->EngineData.GpioGroupId);
  if (EngineDescriptor->EngineData.EngineType == HsioPcieEngine){
    IDS_HDT_CONSOLE (PCIE_MISC, "    PortPresent - %d\n    DeviceNumber - %d\n    FunctionNumber - %d\n    LinkSpeedCapability - %d\n    LinkAspm - %d\n    LinkHotplug - %d\n    SB link - %d\n    MiscControls - 0x%02x\n" ,
      ((HSIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.PortPresent,
      ((HSIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.DeviceNumber,
      ((HSIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.FunctionNumber,
      ((HSIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkSpeedCapability,
      ((HSIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkAspm,
      ((HSIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.LinkHotplug,
      ((HSIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.MiscControls.SbLink,
      ((HSIO_PORT_DESCRIPTOR *) EngineDescriptor)->Port.MiscControls
    );
  }
}


/*----------------------------------------------------------------------------------------*/
/**
 * Helper function to dump input configuration to debug out
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to user defined complex descriptor
 */
VOID
HsioUserConfigConfigDump (
  IN      HSIO_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  HSIO_PORT_DESCRIPTOR      *EngineDescriptor;
  UINTN                     Index;
  HSIO_COMPLEX_DESCRIPTOR   *LocalDescriptor;

  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config Start------------->\n");
  IDS_HDT_CONSOLE(PCIE_MISC, " BMC Link Location is %x\n", ComplexDescriptor->BmcLinkLocation);
  LocalDescriptor = ComplexDescriptor;
  Index = 0;
  while (LocalDescriptor != NULL) {
    IDS_HDT_CONSOLE (PCIE_MISC, "<---------------- Socket %d ----------------->\n", Index);
    EngineDescriptor = LocalDescriptor->PciePortList;
    while (EngineDescriptor != NULL) {
      HsioUserDescriptorConfigDump (EngineDescriptor);
      EngineDescriptor = HsioInputParsetGetNextDescriptor (EngineDescriptor);
    }
    LocalDescriptor = HsioInputParsetGetNextDescriptor (LocalDescriptor);
    Index++;
  }
  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config End-------------->\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * Force all ComplexDescriptor entries to enable TurnOffUnusedLanes
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to user defined complex descriptor
 */
VOID
PcieForceTurnOffUnusedLanes (
  IN      HSIO_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  HSIO_PORT_DESCRIPTOR      *EngineDescriptor;
  HSIO_COMPLEX_DESCRIPTOR   *LocalDescriptor;

  LocalDescriptor = ComplexDescriptor;
  while (LocalDescriptor != NULL) {
    EngineDescriptor = LocalDescriptor->PciePortList;
    while (EngineDescriptor != NULL) {
      EngineDescriptor->Port.MiscControls.TurnOffUnusedLanes = 1;
      IDS_HDT_CONSOLE (PCIE_MISC, "PcieForceTurnOffUnusedLanes(TurnOffUnusedLanes=%x)\n", EngineDescriptor->Port.MiscControls.TurnOffUnusedLanes);
      EngineDescriptor = HsioInputParsetGetNextDescriptor (EngineDescriptor);
    }
    LocalDescriptor = HsioInputParsetGetNextDescriptor (LocalDescriptor);
  }
}

/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting
 *
 *
 *
 * @param[in]  ComplexDescriptor   Pointer to user defined complex descriptor
 *
 * @retval    HGPI_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
SataChannelTypeSetting (
  IN      HSIO_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  UINT8                   SataChannelType;
  HSIO_PORT_DESCRIPTOR    *EngineDescriptor;

  SataChannelType = PcdGet8 (PcdCfgSataPhyTuning);
  if (SataChannelType != 0) {
    while (ComplexDescriptor != NULL) {
      EngineDescriptor = ComplexDescriptor->PciePortList;
      while (EngineDescriptor != NULL) {
        if (EngineDescriptor->EngineData.EngineType == HsioSATAEngine) {
          EngineDescriptor->Port.MiscControls.ChannelType = SataChannelType;
          IDS_HDT_CONSOLE (GNB_TRACE, "SataChannelTypeSetting(ChannelType=%x)\n", SataChannelType);
        }
        EngineDescriptor = HsioInputParsetGetNextDescriptor (EngineDescriptor);
      }
      ComplexDescriptor = HsioInputParsetGetNextDescriptor (ComplexDescriptor);
    }
  }
  return;
}

/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    HGPI_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
PcieInterfaceInitSetting (
  IN       GNB_HANDLE       *GnbHandle,
  IN       SOC_LOGICAL_ID   LogicalCpuId
  )
{
  PCIe_ENGINE_CONFIG     *PcieEngine;
  PCIe_WRAPPER_CONFIG    *PcieWrapper;
  UINT32                 Index;
  UINT32                 Value;
  UINT8                  EnPcie4Index;
  PCIE_LC_CNTL5_STRUCT   LcCntl5;
  UINT32                 StrapIndexAddr;
  UINT32                 StrapDataAddr;
  UINT32                 SmnAddress;
  UINT8                  MappingPortID;
  PCIE_LC_32GT_EQUALIZATION_CNTL_STRUCT  LcCntl32GT;

  IDS_HDT_CONSOLE (GNB_TRACE, "    PcieInterfaceInitSetting Enter \n");

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "    Init PcieWrapper %d \n", PcieWrapper->WrapId);
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      PcieIntegratedInterfaceCallback (GnbHandle, PcieEngine, NULL);
      MappingPortID = (PcieEngine->Type.Port.PortId) % 8;
      
      //Do not power down lanes when in hold training
      SmnAddress = ConvertPciePortAddress (PCIE0_LC_CNTL5_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
      NbioRegisterRead(GnbHandle, TYPE_SMN, SmnAddress, &LcCntl5.Value, 0);
      LcCntl5.Field.LC_HOLD_TRAINING_MODE = 0;
      NbioRegisterWrite(GnbHandle, TYPE_SMN, SmnAddress, &LcCntl5.Value, 0);
      //PCIe Gen5 WA,Force Gen5 DSP Tx Init Preset
      SmnAddress = ConvertPciePortAddress(PCIE0_LC_32GT_EQUALIZATION_CNTL_ADDRESS_HYEX, GnbHandle, PcieWrapper, MappingPortID);
      NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &LcCntl32GT.Value, 0);
      LcCntl32GT.Field.LC_USE_EQTS2_PRESET_32GT = 0;
      NbioRegisterWrite (GnbHandle, TYPE_SMN, SmnAddress, &LcCntl32GT.Value, 0);
      
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }

    if (PcieWrapper->WrapId == 0) {
      StrapIndexAddr = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS);
      StrapDataAddr  = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS);
    } else if (PcieWrapper->WrapId == 1) {
      StrapIndexAddr = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS);
      StrapDataAddr  = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS);
    } else {
      StrapIndexAddr = NBIO0_PCIE2_STRAP_INDEX_ADDRESS;
      StrapDataAddr = NBIO0_PCIE2_STRAP_DATA_ADDRESS;
    }
   for(EnPcie4Index = 0 ; EnPcie4Index < 4 ;EnPcie4Index++){
    Index = EnPcie4StrapList[EnPcie4Index];
    NbioRegisterWrite (GnbHandle,
                       TYPE_SMN,
                       StrapIndexAddr,
                       &Index, 
                       0
                       );
    Value = 1;
    NbioRegisterWrite (GnbHandle,
                       TYPE_SMN,
                       StrapDataAddr,
                       &Value,
                       0
                       ); 
   }	

    if (PcdGetBool (PcdCfgACSEnable)) {
    //enable AER
    Index = STRAP_BIF_AER_EN_ADDR;
    NbioRegisterWrite (GnbHandle,
                       TYPE_SMN,
                       StrapIndexAddr,
                       &Index,
                       0
                       );
    Value = 1;
    NbioRegisterWrite (GnbHandle,
                       TYPE_SMN,
                       StrapDataAddr,
                       &Value,
                       0
                       );

    IDS_HDT_CONSOLE (GNB_TRACE, "    Enable PCIE AER Capability for PCIE core %d \n", PcieWrapper->WrapId);
	  //enable ACS
      Index = STRAP_BIF_ACS_EN_ADDR;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapIndexAddr,
                         &Index,
                         0
                         );
      Value = 1;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapDataAddr,
                         &Value,
                         0
                         );

      for (Index = STRAP_BIF_ACS_SOURCE_VALIDATION_ADDR; Index <= STRAP_BIF_ACS_UPSTREAM_FORWARDING_ADDR; Index++) {
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           StrapIndexAddr,
                           &Index,
                           0
                           );
        Value = 1;
        NbioRegisterWrite (GnbHandle,
                           TYPE_SMN,
                           StrapDataAddr,
                           &Value,
                           0
                           );
      }
      //debug tx_preset rx_preset
      Index = STRAP_BIF_PCIE_LANE_EQ_DN_PORT_RX_PRESET_ADDR;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapIndexAddr,
                         &Index,
                         0
                         );
      Value = 2;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapDataAddr,
                         &Value,
                         0
                         );
      
      Index = STRAP_BIF_PCIE_LANE_EQ_UP_PORT_RX_PRESET_ADDR;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapIndexAddr,
                         &Index,
                         0
                         );
      Value = 2;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapDataAddr,
                         &Value,
                         0
                         );
      
      Index = STRAP_BIF_PCIE_LANE_EQ_DN_PORT_TX_PRESET_ADDR;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapIndexAddr,
                         &Index,
                         0
                         );
      Value = 7;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapDataAddr,
                         &Value,
                         0
                         );
      
      Index = STRAP_BIF_PCIE_LANE_EQ_UP_PORT_TX_PRESET_ADDR;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapIndexAddr,
                         &Index,
                         0
                         );
      Value = 7;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapDataAddr,
                         &Value,
                         0
                         );
      // STRAP_BIF_ALWAYS_USE_FAST_TXCLK = 1 to solve X1 pcie issue
      Index = STRAP_BIF_ALWAYS_USE_FAST_TXCLK_ADDR;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapIndexAddr,
                         &Index,
                         0
                         );
      Value = 1;
      NbioRegisterWrite (GnbHandle,
                         TYPE_SMN,
                         StrapDataAddr,
                         &Value,
                         0
                         );	  
      //end
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 3, 0, PcdGet8 (PcdPcieDsInitialPresetGen3));
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 3, 1, PcdGet8 (PcdPcieUsInitialPresetGen3));
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 4, 0, PcdGet8 (PcdPcieDsInitialPresetGen4));
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 4, 1, PcdGet8 (PcdPcieUsInitialPresetGen4));
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 5, 0, PcdGet8 (PcdPcieDsInitialPresetGen5));
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 5, 1, PcdGet8 (PcdPcieUsInitialPresetGen5));
    }
    
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     ConvertPcieCoreAddress(PCIE0_CNTL2_ADDRESS_HYEX, GnbHandle, PcieWrapper),
                     (UINT32)~(PCIE_CNTL2_TX_ATOMIC_OPS_DISABLE_MASK),
                     1 << PCIE_CNTL2_TX_ATOMIC_OPS_DISABLE_OFFSET,
                     0
                     );

    // Set PCIE_SELECT_BDF_ID as 0xff to solve Iommu issue(Completion-Wait timeout)
    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      ConvertPcieCoreAddress (PCIE0_SELECT_BDF_ID_HYEX, GnbHandle, PcieWrapper),
      0x0,
      0xff,        
      0
      );
    PcieEqCfgInitPhase (GnbHandle, PcieWrapper);
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  
  if (PcdGetBool (PcdCfgACSEnable)) {
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE(GnbHandle, NBIF0_RCC_DEV0_PORT_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK),
                     ((0x1 << RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET)),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE(GnbHandle, NBIF1_RCC_DEV0_PORT_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK),
                     ((0x1 << RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET)),
                     0
                     );
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     NBIO_SPACE(GnbHandle, NBIF2_RCC_DEV0_PORT_STRAP0_ADDRESS_HYEX),
                     (UINT32) ~(RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK |
                                RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK),
                     ((0x1 << RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET) |
                      (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET)),
                     0
                     );
    for (Index = 0; Index < 8; Index++) {
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE(GnbHandle, (NBIF0_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYEX + (Index * 0x200))),
                       (UINT32) ~(RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK_HYEX |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK_HYEX |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK),
                       ((0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET_HYEX) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET_HYEX) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET)),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE(GnbHandle, (NBIF1_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYEX + (Index * 0x200))),
                       (UINT32) ~(RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK_HYEX |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK_HYEX |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK),
                       ((0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET_HYEX) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET_HYEX) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET)),
                       0
                       );
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       NBIO_SPACE(GnbHandle, (NBIF2_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYEX + (Index * 0x200))),
                       (UINT32) ~(RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK_HYEX |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK_HYEX |
                                  RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK),
                       ((0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET_HYEX) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET_HYEX) |
                        (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET)),
                       0
                       );
    }
  }

  //GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieInterfaceInitSetting Exit\n"););
  IDS_HDT_CONSOLE (GNB_TRACE, "    PcieInterfaceInitSetting Exit \n");
}


/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting after Hsio training done
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    HGPI_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
HsioCfgAfterTraining
 (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;
  UINT32               Address;

  IDS_HDT_CONSOLE (GNB_TRACE, "HsioCfgAfterTraining Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated(PcieEngine)) {

        if (PcdGetBool(PcdCfgDisableRcvrResetCycle)) {
          Address = SMN_PCS20_0x12E0b0c8_ADDRESS_HYEX + (UINT32)(GnbHandle->RbId) * 0x100000 + (UINT32)(PcieWrapper->WrapId) * 0x100000;
          if (PcieEngine->Type.Port.PortData.PortPresent) {
            NbioRegisterRMW(
            GnbHandle,
            TYPE_SMN,
            Address,
            (UINT32)~(0x00040000),
            (1 << 18),
             0
            );
          }
        }

      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }

    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "HsioCfgAfterTraining Exit\n");
}


/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting after Hsio init done
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    HGPI_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
PcieInterfaceAfterHsioDoneSetting (
  IN       GNB_HANDLE       *GnbHandle,
  IN       SOC_LOGICAL_ID   LogicalCpuId
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;
  UINT32               Index;
  UINT32               Value;
  UINT32               Address;
  UINT32               StrapIndexAddr;
  UINT32               StrapDataAddr;
  UINT32               PortIndex;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInterfaceAfterHsioDoneSetting Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      PcieAfterTrainingDoneCallback (GnbHandle, PcieEngine, NULL );
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    
    Address = ConvertPcieCoreAddress(PCIE0_STRAP_F0_ADDRESS_HYEX, GnbHandle, PcieWrapper);
    Value = (PcieWrapper->WrapId == 0) ? (UINT32) PcdGet8 (PcdGppAtomicOps) : (UINT32) PcdGet8(PcdGfxAtomicOps);
    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     Address,
                     (UINT32) ~(PCIE_STRAP_F0_STRAP_F0_ATOMIC_EN_MASK),
                     (1 << PCIE_STRAP_F0_STRAP_F0_ATOMIC_EN_OFFSET),
                     0
                     );

    if (PcdGetBool (PcdPcieEcrcEnablement)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "    Enable ECRC fearture \n");  
      NbioRegisterRMW (GnbHandle,
                       TYPE_SMN,
                       Address,
                       (UINT32) ~(PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_MASK | PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_MASK),
                       (1 << PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_OFFSET) | (1 << PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_OFFSET),
                       0
                       );
    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "    Disable ECRC fearture \n");  
      NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         Address,
                         (UINT32) ~(PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_MASK | PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_MASK),
                         (0 << PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_OFFSET) | (0 << PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_OFFSET),
                         0
                         );
    }

    if (PcdGet8 (PcdRunEnvironment) >= 0) {
        //Enable PCIE ports AER capability for EMU and FPGA ENV
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         Address,
                         (UINT32) ~(PCIE_STRAP_F0_STRAP_F0_AER_EN_MASK),
                         (1 << PCIE_STRAP_F0_STRAP_F0_AER_EN_OFFSET),
                         0
                         );
        IDS_HDT_CONSOLE (GNB_TRACE, "    Enable AER Capability for PCIE Wrapper %d \n", PcieWrapper->WrapId);
        
        if (PcieWrapper->WrapId == 0) {
            StrapIndexAddr = PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS);
            StrapDataAddr = PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS);
        } else if (PcieWrapper->WrapId == 1) {
            StrapIndexAddr = PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS);
            StrapDataAddr = PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS);
        } else {
            StrapIndexAddr = NBIO0_PCIE2_STRAP_INDEX_ADDRESS;
            StrapDataAddr = NBIO0_PCIE2_STRAP_DATA_ADDRESS;
        }
        
		//Enable PCIE ports DPC capability for EMU and FPGA ENV
        Value = 1;
        Index = STRAP_BIF_DPC_EN_ADDR;
        NbioRegisterWrite (GnbHandle,
                                   TYPE_SMN,
                                   StrapIndexAddr,
                                   &Index,
                                   0
                                   );
        NbioRegisterWrite (GnbHandle,
                                   TYPE_SMN,
                                   StrapDataAddr,
                                   &Value,
                                   0
                                   );
        IDS_HDT_CONSOLE (GNB_TRACE, "    Enable DPC Capability for PCIE Wrapper %d \n", PcieWrapper->WrapId);
    }
    
    for (PortIndex = 0; PortIndex < 8; PortIndex++) {
      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     ConvertPciePortAddress(PCIEP_STRAP_MISC_instPCIE0_link_ADDRESS_HYEX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
                     (UINT32) ~(PCIEP_STRAP_MISC_STRAP_EXTENDED_FMT_SUPPORTED_MASK | PCIEP_STRAP_MISC_STRAP_E2E_PREFIX_EN_MASK),
                     (1 << PCIEP_STRAP_MISC_STRAP_EXTENDED_FMT_SUPPORTED_OFFSET) | (1 << PCIEP_STRAP_MISC_STRAP_E2E_PREFIX_EN_OFFSET),
                     0
                     );
    }

    for (PortIndex = 0; PortIndex < 8; PortIndex++) {
      //Set TX_ACK_LATENCY_ACCELERATE BIT31 as 1 to solve RxOF error
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIE0_TX_ACK_LATENCY_LIMIT_HYEX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
        (UINT32) ~(TX_ACK_LATENCY_ACCELERATE_MASK),
        (UINT32)(0x1 << TX_ACK_LATENCY_ACCELERATE_OFFSET),
        0
        );
      
      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     ConvertPciePortAddress(PCIE0_DEVICE_CNTL2_ADDRESS_HYEX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
                     (UINT32) ~(DEVICE_CNTL2_CPL_TIMEOUT_VALUE_MASK),
                     (0x6 << DEVICE_CNTL2_CPL_TIMEOUT_VALUE_OFFSET),
                     0
                     );

      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     ConvertPciePortAddress(PCIE0_LC_CNTL_ADDRESS_HYEX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
                     (UINT32) ~(PCIE_LC_CNTL_LC_L1_IMMEDIATE_ACK_MASK),
                     (0 << PCIE_LC_CNTL_LC_L1_IMMEDIATE_ACK_OFFSET),
                     0
                     );

      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     ConvertPciePortAddress(PCIE0_LC_LINK_WIDTH_CNTL_ADDRESS_HYEX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
                     (UINT32) ~(PCIE_LC_LINK_WIDTH_CNTL_LC_DUAL_END_RECONFIG_EN_MASK),
                     (0x1 << PCIE_LC_LINK_WIDTH_CNTL_LC_DUAL_END_RECONFIG_EN_OFFSET),
                     0
                     );

      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     ConvertPciePortAddress(PCIE0_PCIE_LC_CNTL2_ADDRESS_HYEX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
                     (UINT32) ~(PCIE_LC_CNTL2_LC_ELEC_IDLE_MODE_MASK),
                     (0x1 << PCIE_LC_CNTL2_LC_ELEC_IDLE_MODE_OFFSET),
                     0
                     );

	  NbioRegisterRMW (GnbHandle,
				     TYPE_SMN,
				     ConvertPciePortAddress(PCIE0_PCIEP_HW_DEBUG_ADDRESS_HYEX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
				     (UINT32) ~(PCIEP_HW_DEBUG_HW_15_DEBUG_MASK),
				     (0x1 << PCIEP_HW_DEBUG_HW_15_DEBUG_OFFSET),
				     0
				     );					 
      //ECRC_ERR_SEVERITY
      NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     ConvertPciePortAddress(PCIE0_GPP0_UNCORR_ERR_SEVERITY_HYEX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
                     (UINT32) ~(0x80000),
                     (0x1 << 19),
                     0
                     ); 
      
    }

    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     ConvertPcieCoreAddress(PCIE0_CNTL_ADDRESS_HYEX, GnbHandle, PcieWrapper),
                     (UINT32) ~(PCIE_CNTL_RX_RCB_INVALID_SIZE_DIS_MASK),
                     0x0 << PCIE_CNTL_RX_RCB_INVALID_SIZE_DIS_OFFSET,
                     0
                     );

    Address = ConvertPcieCoreAddress(PCIE0_P_CNTL_ADDRESS_HYEX, GnbHandle, PcieWrapper);
    if ( PcdGetBool (PcdHygonNbioReportEdbErrors) ) {
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         Address,
                         (UINT32) ~(PCIE_P_CNTL_P_IGNORE_EDB_ERR_MASK),
                         0x0 << PCIE_P_CNTL_P_IGNORE_EDB_ERR_OFFSET,
                         0
                         );
    } else {
        NbioRegisterRMW (GnbHandle,
                         TYPE_SMN,
                         Address,
                         (UINT32) ~(PCIE_P_CNTL_P_IGNORE_EDB_ERR_MASK),
                         0x1 << PCIE_P_CNTL_P_IGNORE_EDB_ERR_OFFSET,
                         0
                         );
    }

    NbioRegisterRMW (GnbHandle,
                     TYPE_SMN,
                     Address,
                     (UINT32) ~(PCIE_P_CNTL_P_ELEC_IDLE_MODE_MASK),
                     0x1 << PCIE_P_CNTL_P_ELEC_IDLE_MODE_OFFSET,//change PCIE_P_CNTL from 3 to 0 math PCIE Spec
                     0
                     );

    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieInterfaceAfterHsioDoneSetting Exit\n");
}


/**----------------------------------------------------------------------------------------*/
/**
 * PCIE interface to configure register setting after Hsio init done
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    HGPI_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
SubsystemIdSetting (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  UINT32      Value;
  UINT32      Index;
  UINT32      SubsystemDeviceId;
  UINT32      SubsystemVendorId;

  IDS_HDT_CONSOLE (GNB_TRACE, "SubsystemIdSetting Entry\n");

  // NB ADAPTER D0F0
  Value = PcdGet32 (PcdCfgNbioSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbioSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBADAPTER_SSID_ADDRESS_HYEX), &Value, 0);
  }

  // IOMMU
  Value = PcdGet32 (PcdCfgIommuSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgIommuSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_IOMMU_SSID_ADDRESS_HYEX), &Value, 0);
  }

  // PSPCCP(NBIO0/NBIF0)
  Value = PcdGet32 (PcdCfgPspccpSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgPspccpSsid = %x\n", Value);
  if ((Value != 0) && (GnbHandle->RbId == 0)) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF0_FUNC2_SSID_ADDRESS_HYEX), &Value, 0);
  }
    
  // NTBCCP(NBIO1-3/NBIF0, NBIO0-3/NBIF1)
  Value = PcdGet32 (PcdCfgNtbccpSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNtbccpSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF1_FUNC1_SSID_ADDRESS_HYEX), &Value, 0);
    if (GnbHandle->RbId != 0){
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF0_FUNC2_SSID_ADDRESS_HYEX), &Value, 0);
    }
  }
  // XGBE
  Value = PcdGet32 (PcdCfgXgbeSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgXgbeSsid = %x\n", Value);
  if ((Value != 0) && (GnbHandle->RbId == 0)) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_XGBE0_SSID_ADDRESS_HYEX, &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_XGBE1_SSID_ADDRESS_HYEX, &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_XGBE2_SSID_ADDRESS_HYEX, &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCICFG_NBIF1_XGBE3_SSID_ADDRESS_HYEX, &Value, 0);
  }

  // NBIF Dummy Functions
  Value = PcdGet32 (PcdCfgNbifF0Ssid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbifF0Ssid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF0_SSID_ADDRESS_HYEX), &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF1_SSID_ADDRESS_HYEX), &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF2_SSID_ADDRESS_HYEX), &Value, 0);
  }

  // NBIF Root Bridge Functions
  Value = PcdGet32 (PcdCfgNbifRCSsid);
  Value = ((Value >> 16) & 0xFFFF) | ((Value << 16) & 0xFFFF0000);      // byo230905 +
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbifRCSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF0RC_SSID_ADDRESS_HYEX), &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF1RC_SSID_ADDRESS_HYEX), &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF2RC_SSID_ADDRESS_HYEX), &Value, 0);
  }

  // NTB
  Value = PcdGet32 (PcdCfgNtbSsid);
  IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNtbSsid = %x\n", Value);
  if (Value != 0) {
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF0_NTB_SSID_ADDRESS_HYEX), &Value, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE(GnbHandle, PCICFG_NBIF2_NTB_SSID_ADDRESS_HYEX), &Value, 0);
  }

  // PCIE
  SubsystemDeviceId = (UINT32) PcdGet16 (PcdHygonPcieSubsystemDeviceID);
  if (SubsystemDeviceId != 0) {
    Index = STRAP_BIF_SUBSYS_ID_ADDR;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS), &Index, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS), &SubsystemDeviceId, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS), &Index, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS), &SubsystemDeviceId, 0);
    if (GnbHandle->RbId == 0) {
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO0_PCIE2_STRAP_INDEX_ADDRESS, &Index, 0);
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO0_PCIE2_STRAP_DATA_ADDRESS, &SubsystemDeviceId, 0);
    }
  }
  SubsystemVendorId = (UINT32) PcdGet16 (PcdHygonPcieSubsystemVendorID);
  if (SubsystemVendorId != 0) {
    Index = STRAP_BIF_SUBSYS_VEN_ID_ADDR;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS), &Index, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS), &SubsystemVendorId, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS), &Index, 0);
    NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS), &SubsystemVendorId, 0);
    if (GnbHandle->RbId == 0) {
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO0_PCIE2_STRAP_INDEX_ADDRESS, &Index, 0);
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO0_PCIE2_STRAP_DATA_ADDRESS, &SubsystemVendorId, 0);
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "SubsystemIdSetting Exit\n");
  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Callback for NbioSmuServicesPpi installation
 *
 *
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  NotifyDescriptor  NotifyDescriptor pointer
 * @param[in]  Ppi               Ppi pointer
 * @retval     EFI_STATUS
 */
EFI_STATUS
EFIAPI
HsioInitializationCallbackPpi (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                             Status;
  EFI_STATUS                             ReturnStatus;
  HGPI_STATUS                            HsioStatus;
  GNB_HANDLE                             *GnbHandle;
  UINT32                                 SmuArg[6];
  PCIe_PLATFORM_CONFIG                   *Pcie;
  PEI_HYGON_NBIO_PCIE_SERVICES_PPI       *PcieServicesPpi;
  PEI_HYGON_NBIO_SMU_SERVICES_PPI        *SmuServicesPpi;
  HYGON_MEMORY_INFO_HOB_PPI              *HygonMemoryInfoHob;
  GNB_PCIE_INFORMATION_DATA_HOB          *PciePlatformConfigHob;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI *FabricTopologyServicesPpi;
  HSIO_COMPLEX_DESCRIPTOR                *PcieTopologyData;
  PEI_HYGON_NBIO_PCIE_COMPLEX_PPI        *NbioPcieComplexPpi;
  HYGON_PEI_SOC_LOGICAL_ID_PPI           *SocLogicalIdPpi;
  SOC_LOGICAL_ID                         LogicalId;

// byo230825 - >>
  Status = (*PeiServices)->LocatePpi (PeiServices, &gHygonMemoryInfoHobPpiGuid, 0, NULL, &HygonMemoryInfoHob);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (*PeiServices)->LocatePpi (PeiServices, &gHygonNbioSmuServicesPpiGuid, 0, NULL, &SmuServicesPpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = (*PeiServices)->LocatePpi (PeiServices, &gHygonNbioPcieComplexPpiGuid, 0, NULL, &NbioPcieComplexPpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  HGPI_TESTPOINT (TpHsioInitializationCallbackPpiEntry, NULL);
  IDS_HDT_CONSOLE (GNB_TRACE, "HsioInitializationCallbackPpi Entry\n");
  ReturnStatus = EFI_SUCCESS;
  (**PeiServices).InstallPpi (PeiServices, &mNbioPcieTrainingStartPpiList);
// byo230825 - <<
  
  NbioPcieComplexPpi->PcieGetComplex (NbioPcieComplexPpi, &PcieTopologyData);
  // Need topology structure to get GnbHandle
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonNbioPcieServicesPpiGuid,
                             0,
                             NULL,
                             (VOID **)&PcieServicesPpi
                             );
  PcieServicesPpi->PcieGetTopology (PcieServicesPpi, &PciePlatformConfigHob);
  Pcie = &(PciePlatformConfigHob->PciePlatformConfigHob);

  if (TRUE == PcdGetBool (PcdEarlyBmcLinkTraining)) {
    PcieTopologyData->BmcLinkLocation = (PcdGet8 (PcdEarlyBmcLinkSocket) << 2) + PcdGet8(PcdEarlyBmcLinkDie);
    GnbHandle = NbioGetHandle (Pcie);
    HsioCleanUpEarlyInit (GnbHandle, PcieTopologyData->BmcLinkLocation);
  } else {
    PcieTopologyData->BmcLinkLocation = 0xFF;
  }
  PcieForceTurnOffUnusedLanes (PcieTopologyData);
  HsioUserConfigConfigDump (PcieTopologyData);

  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonSocLogicalIdPpiGuid,
                             0,
                             NULL,
                             &SocLogicalIdPpi
                             );
  Status = SocLogicalIdPpi->GetLogicalIdOnCurrentCore (&LogicalId);

  LibHygonMemFill (SmuArg, 0x00, 24, (HYGON_CONFIG_PARAMS *) NULL);
  SmuArg[0] = 0xAA55AA55;
  SmuServicesPpi->SmuServiceRequest (SmuServicesPpi, 0, 0, 1, SmuArg, SmuArg);

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Configure Socket %d RB %d :\n", GnbHandle->SocketId, GnbHandle->RbId);
    
    IDS_HDT_CONSOLE (MAIN_FLOW, "    Disable HsioHDaudioEngine\n");
    NbioDisableNbifDevice (GnbHandle, HsioHDaudioEngine, 0);
                
    IDS_HOOK (IDS_HOOK_NBIO_PCIE_USER_CONFIG, (VOID *)GnbHandle, (VOID *)PcieTopologyData);
    // Call KPNP workaround
    PcieInterfaceInitSetting (GnbHandle, LogicalId);
    
    GnbHandle = GnbGetNextHandle(GnbHandle);
  }

  SataChannelTypeSetting  (PcieTopologyData);
  if (PcdGet8 (PcdRunEnvironment) == 0) {
      InitNTBFeature (PeiServices, PcieTopologyData, Pcie);
  }
  // At this point we know the NBIO topology and SMU services are available
  // We can now use SMU to initialize HSIO

  if (PcdGet8 (PcdRunEnvironment) < 2) {
      HsioStatus = HsioEarlyInit (PeiServices, Pcie, PcieTopologyData);
      if (HsioStatus != HGPI_SUCCESS) {
        IDS_HDT_CONSOLE (GNB_TRACE, "HSIO ERROR!!\n");
        ReturnStatus = EFI_INVALID_PARAMETER;
        ASSERT (FALSE);
      }
  }
  PcieConfigDebugDump(Pcie);

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
//          IDS_HOOK (IDS_HOOK_NBIO_PCIE_TOPOLOGY, (VOID *)GnbHandle, (VOID *)Pcie);
    if (PcdGet8 (PcdRunEnvironment) < 2) {  
        HsioCfgAfterTraining (GnbHandle);
    }
    PciePortsVisibilityControlHyEx (HidePorts, GnbHandle);
    PcieInterfaceAfterHsioDoneSetting (GnbHandle, LogicalId);
    SubsystemIdSetting (GnbHandle);
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }
  
  if (PcdGet8 (PcdRunEnvironment) == 0) {
      PcieConfigureHotplugPortsST (Pcie);
      InitNTBAfterTraining (PeiServices, PcieTopologyData, Pcie);
  }
  Status = (*PeiServices)->LocatePpi (
                             PeiServices,
                             &gHygonFabricTopologyServicesPpiGuid,
                             0,
                             NULL,
                             &FabricTopologyServicesPpi
                             );
  InitBusRanges (Pcie, FabricTopologyServicesPpi);

  // Initialize ARI
  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    if (TRUE == PcdGetBool (PcdCfgPcieAriSupport)) {
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        NBIO_SPACE(GnbHandle, IOC_FEATURE_CNTL_ADDRESS_HYEX),
        (UINT32)~(NBMISC_0118_IOC_ARI_SUPPORTED_MASK),
        1 << NBMISC_0118_IOC_ARI_SUPPORTED_OFFSET,
        0
        );

//PcieAriInit (GnbHandle);

    }
    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  // Install PPI to notify other drivers that PCIe training is complete
  (**PeiServices).InstallPpi (PeiServices, &mNbioPcieTrainingDonePpiList);   // byo230825 -

  IDS_HDT_CONSOLE (GNB_TRACE, "HsioInitializationCallbackPpi Exit Status = 0x%x\n", ReturnStatus);
  HGPI_TESTPOINT (TpHsioInitializationCallbackPpiExit, NULL);
  return ReturnStatus;
}
