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
#include <Library/HygonSmnAddressLib.h>
#include "HsioPcieEq.h"
#include <Library/HygonSocBaseLib.h>

#define FILECODE  NBIO_PCIE_HYGX_HYGONNBIOPCIESTPEI_HSIOINITST_FILECODE

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
  STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_B_ADDR,
  STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_B_ADDR,
  STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_C_ADDR,
  STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_C_ADDR,
  STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_D_ADDR,
  STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_D_ADDR,
  STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_E_ADDR,
  STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_E_ADDR,
  STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_F_ADDR,
  STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_F_ADDR,
  STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_G_ADDR,
  STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_G_ADDR,
  STRAP_BIF_TEN_BIT_TAG_COMPLETER_SUPPORTED_H_ADDR,
  STRAP_BIF_TEN_BIT_TAG_REQUESTER_SUPPORTED_H_ADDR,
  STRAP_BIF_LOCAL_DLF_SUPPORTED_A_ADDR,
  STRAP_BIF_DLF_EXCHANGE_EN_A_ADDR,
  STRAP_BIF_DLF_EXCHANGE_EN_B_ADDR,
  STRAP_BIF_DLF_EXCHANGE_EN_C_ADDR,
  STRAP_BIF_DLF_EXCHANGE_EN_D_ADDR,
  STRAP_BIF_DLF_EXCHANGE_EN_E_ADDR,
  STRAP_BIF_DLF_EXCHANGE_EN_F_ADDR,
  STRAP_BIF_DLF_EXCHANGE_EN_G_ADDR,
  STRAP_BIF_DLF_EXCHANGE_EN_H_ADDR
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

VOID
Pcie_Core2_Ports_Training (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN GNB_HANDLE                *GnbHandle
  );

VOID
Pcie_Core0_Ports_Training (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN GNB_HANDLE                 *GnbHandle
  );
extern
HGPI_STATUS
HsioEarlyInit (
  IN       CONST EFI_PEI_SERVICES          **PeiServices,
  IN       PCIe_PLATFORM_CONFIG            *Pcie,
  IN       HSIO_COMPLEX_DESCRIPTOR         *UserConfig
  );

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
    IDS_HDT_CONSOLE (GNB_TRACE, "  Found Physical Die Number %d\n", NbioHandle->PhysicalDieId);
    if (NbioHandle->PhysicalDieId == DieNumber) {
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

  Address = ConvertPciePortAddress (PCIE0_GPP0_TX_CNTL_ADDRESS_HYGX, GnbHandle, Wrapper, MappingPortID);

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

  GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieAfterTrainingDoneCallback Enter\n"));

  Address = Engine->Type.Port.Address.AddressValue;
  if (Address != 0) {
    // Set completion timeout
    GnbLibPciRead (Address | 0x80, AccessWidth32, &Value, NULL);
    Value = (Value & 0xfffffff0) | 0x6;
    GnbLibPciWrite (Address | 0x80, AccessWidth32, &Value, NULL);

    // Set slot_implemented
    GnbLibPciRMW (
      Engine->Type.Port.Address.AddressValue | DxF0x58_PCI_OFFSET,
      AccessWidth32,
      0xffffffff,
      1 << DxF0x58_SlotImplemented_OFFSET,
      (HYGON_CONFIG_PARAMS *)NULL
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
      Value = 0;
      break;
    default:
      break;
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PowerOffPhyLanesPllL1 for Wrapper %d Port %d\n", Wrapper->WrapId, MappingPortID);
  //Config LC_CNTL6.LC_L1_PowerDown BIT21
  Address = ConvertPciePortAddress(PCIE0_GPP0_LC_CNTL6_ADDRESS_HYGX, GnbHandle, Wrapper, MappingPortID);
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
  GNB_HANDLE                  *GnbHandle;
  PCIE_VDM_NODE_CTRL4_STRUCT  VdmNode;
  UINTN                       SystemFabricID;
  UINTN                       BusNumberBase;
  UINTN                       BusNumberLimit;
  UINT32                      VdmNodeAddress;
  UINTN                       NumberOfProcessors;
  UINTN                       TotalNumberOfPhysicalDie;
  UINTN                       TotalNumberOfLogicalDie;
  UINTN                       TotalNumberOfRootBridges;
  UINTN                       SocketId, LogicalDieId, RbId;
  UINT32                      VdmNodeIndex;

  VdmNodeIndex = 0;

  FabricTopologyServicesPpi->GetSystemInfo (&NumberOfProcessors, NULL, NULL);
  for (SocketId = 0; SocketId < NumberOfProcessors; SocketId++) {
    FabricTopologyServicesPpi->GetProcessorInfo (SocketId, &TotalNumberOfPhysicalDie, &TotalNumberOfLogicalDie, NULL);
    for (LogicalDieId = 0; LogicalDieId < TotalNumberOfLogicalDie; LogicalDieId++) {
      FabricTopologyServicesPpi->GetDieInfo (SocketId, LogicalDieId, &TotalNumberOfRootBridges, NULL, NULL);
      for (RbId = 0; RbId < TotalNumberOfRootBridges; RbId++) {
        FabricTopologyServicesPpi->GetRootBridgeInfo (SocketId, LogicalDieId, RbId, &SystemFabricID, &BusNumberBase, &BusNumberLimit);
        VdmNode.Field.BUS_RANGE_BASE  = BusNumberBase;
        VdmNode.Field.BUS_RANGE_LIMIT = BusNumberLimit;
        VdmNode.Field.NODE_PRESENT    = 1;
        VdmNodeAddress = PCIE_VDM_NODE0_CTRL4_ADDRESS_HYGX + VdmNodeIndex * 0x4;
        VdmNodeIndex++;

        IDS_HDT_CONSOLE (GNB_TRACE, "Socket %d die %d Rb %d bus 0x%02X-0x%02X, VdmNodeIndex %d \n", SocketId, LogicalDieId, RbId, BusNumberBase, BusNumberLimit, VdmNodeIndex);

        GnbHandle = NbioGetHandle (Pcie);
        while (GnbHandle != NULL) {
          if (GnbHandle->IohubPresent) {
            if ((GnbHandle->SocketId == SocketId) && (GnbHandle->LogicalDieId == LogicalDieId) && (GnbHandle->RbId == RbId)) {
              VdmNode.Field.NODE_PRESENT = 0;
            }

            NbioRegisterWrite (
              GnbHandle,
              TYPE_SMN,
              NBIO_SPACE (GnbHandle, VdmNodeAddress),
              &VdmNode.Value,
              0
              );
          }

          GnbHandle = GnbGetNextHandle (GnbHandle);
        }
      }
    }
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
  UINT32                 StrapIndexAddr;
  UINT32                 StrapDataAddr;
  UINT8                  PkgType;

  IDS_HDT_CONSOLE (GNB_TRACE, "    PcieInterfaceInitSetting Enter \n");

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    IDS_HDT_CONSOLE (GNB_TRACE, "    Init PcieWrapper %d \n", PcieWrapper->WrapId);
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      PcieIntegratedInterfaceCallback (GnbHandle, PcieEngine, NULL);
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }

    if (PcieWrapper->WrapId == 0) {
      StrapIndexAddr = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS);
      StrapDataAddr  = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS);
    } else if (PcieWrapper->WrapId == 1) {
      IDS_HDT_CONSOLE (GNB_TRACE, "   PcieWrapper->WrapId == 1 \n");
      StrapIndexAddr = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS);
      StrapDataAddr  = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS);
    } else {
      StrapIndexAddr = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE2_STRAP_INDEX_ADDRESS);
      StrapDataAddr  = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE2_STRAP_DATA_ADDRESS);
    }

    for(EnPcie4Index = 0; EnPcie4Index < sizeof (EnPcie4StrapList) / sizeof (UINT32); EnPcie4Index++) {
      Index = EnPcie4StrapList[EnPcie4Index];
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      Value = 1;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );
    }

    if (PcdGetBool (PcdCfgACSEnable)) {
      // enable AER
      Index = STRAP_BIF_AER_EN_ADDR;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      Value = 1;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );

      IDS_HDT_CONSOLE (GNB_TRACE, "    Enable PCIE AER Capability for PCIE core %d \n", PcieWrapper->WrapId);
      // enable ACS
      Index = STRAP_BIF_ACS_EN_ADDR;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      Value = 1;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );

      for (Index = STRAP_BIF_ACS_SOURCE_VALIDATION_ADDR; Index <= STRAP_BIF_ACS_UPSTREAM_FORWARDING_ADDR; Index++) {
        NbioRegisterWrite (
          GnbHandle,
          TYPE_SMN,
          StrapIndexAddr,
          &Index,
          0
          );
        Value = 1;
        NbioRegisterWrite (
          GnbHandle,
          TYPE_SMN,
          StrapDataAddr,
          &Value,
          0
          );
      }

      // debug tx_preset rx_preset
      Index = STRAP_BIF_PCIE_LANE_EQ_DN_PORT_RX_PRESET_ADDR;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      Value = 2;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );

      Index = STRAP_BIF_PCIE_LANE_EQ_UP_PORT_RX_PRESET_ADDR;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      Value = 2;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );

      Index = STRAP_BIF_PCIE_LANE_EQ_DN_PORT_TX_PRESET_ADDR;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      Value = 7;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );

      Index = STRAP_BIF_PCIE_LANE_EQ_UP_PORT_TX_PRESET_ADDR;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      Value = 7;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );
      // STRAP_BIF_ALWAYS_USE_FAST_TXCLK = 0
      Index = STRAP_BIF_ALWAYS_USE_FAST_TXCLK_ADDR;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      Value = 0;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );
      // end
    }

    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      ConvertPcieCoreAddress (PCIE0_CNTL2_ADDRESS_HYGX, GnbHandle, PcieWrapper),
      (UINT32) ~(PCIE_CNTL2_TX_ATOMIC_OPS_DISABLE_MASK),
      1 << PCIE_CNTL2_TX_ATOMIC_OPS_DISABLE_OFFSET,
      0
      );

    // Set PCIE_SELECT_BDF_ID as 0xff to solve Iommu issue(Completion-Wait timeout)
    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      ConvertPcieCoreAddress (PCIE0_SELECT_BDF_ID_HYGX, GnbHandle, PcieWrapper),
      0x0,
      0xff,        
      0
      );

    // Enable tag10 for PCIE Core 0/1/2 ports
    IDS_HDT_CONSOLE (GNB_TRACE, "    Enable 10 bits tag for PCIE core %d ports \n", PcieWrapper->WrapId);
    for( Index = 0; Index < 8; Index++) {
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIE0_DEVICE_CNTL2_ADDRESS_HYGX, GnbHandle, PcieWrapper, (UINT8)Index),
        (UINT32) ~(DEVICE_CNTL2_TEN_BIT_TAG_REQUESTER_ENABLE_MASK),
        1 << DEVICE_CNTL2_TEN_BIT_TAG_REQUESTER_ENABLE_OFFSET,
        0
        );
    }
    SetPcieInitialPreset (GnbHandle, PcieWrapper, 3, 0, PcdGet8(PcdPcieDsInitPresetGen3));
    SetPcieInitialPreset (GnbHandle, PcieWrapper, 3, 1, PcdGet8(PcdPcieUsInitPresetGen3));

    PkgType = GetSocPkgType ();
    if ((PkgType == SP6_421)) {
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 4, 0, PcdGet8(PcdPcieDsInitPresetGen4SP6421));
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 4, 1, PcdGet8(PcdPcieUsInitPresetGen4SP6421));
    } else {
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 4, 0, PcdGet8(PcdPcieDsInitPresetGen4));
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 4, 1, PcdGet8(PcdPcieUsInitPresetGen4));
    }
    if ((PkgType == DM1_102)) {
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 5, 0, PcdGet8(PcdPcieDsInitPresetGen5DM1102));
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 5, 1, PcdGet8(PcdPcieUsInitPresetGen5DM1102));
    } else {
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 5, 0, PcdGet8(PcdPcieDsInitPresetGen5));
      SetPcieInitialPreset (GnbHandle, PcieWrapper, 5, 1, PcdGet8(PcdPcieUsInitPresetGen5));
    }
    
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  if (PcdGetBool (PcdCfgACSEnable)) {
    if ((GnbHandle->DieType == IOD_EMEI) && ((GnbHandle->RbId % 2) == 0)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "    Configure ACS for EMEI Socket %d die p%d Rb %d NBIF \n", GnbHandle->SocketId, GnbHandle->PhysicalDieId, GnbHandle->RbId);
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        NBIO_SPACE (GnbHandle, NBIF0_RCC_DEV0_PORT_STRAP0_ADDRESS_HYGX),
        (UINT32) ~(RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK |
                   RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK |
                   RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK),
        ((0x1 << RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET) |
         (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET) |
         (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET)),
        0
        );

      for (Index = 0; Index < 8; Index++) {
        NbioRegisterRMW (
          GnbHandle,
          TYPE_SMN,
          NBIO_SPACE (GnbHandle, NBIF0_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYGX | (Index * 0x200)),
          (UINT32) ~(RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK_HYGX |
                     RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK_HYGX |
                     RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK),
          ((0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET_HYGX) |
           (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET_HYGX) |
           (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET)),
          0
          );
      }
    }

    if (GnbHandle->DieType == IOD_DUJIANG) {
      IDS_HDT_CONSOLE (GNB_TRACE, "    Configure ACS for DJ Socket %d die p%d Rb %d NBIF \n", GnbHandle->SocketId, GnbHandle->PhysicalDieId, GnbHandle->RbId);
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        NBIO_SPACE (GnbHandle, NBIF2_RCC_DEV0_PORT_STRAP0_ADDRESS_HYGX),
        (UINT32) ~(RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK |
                   RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK |
                   RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK),
        ((0x1 << RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET) |
         (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET) |
         (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET)),
        0
        );
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        NBIO_SPACE (GnbHandle, NBIF3_RCC_DEV0_PORT_STRAP0_ADDRESS_HYGX),
        (UINT32) ~(RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_MASK |
                   RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_MASK |
                   RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_MASK),
        ((0x1 << RCC_DEV0_PORT_STRAP0_STRAP_ACS_EN_DN_DEV0_OFFSET) |
         (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_AER_EN_DN_DEV0_OFFSET) |
         (0x1 << RCC_DEV0_PORT_STRAP0_STRAP_CPL_ABORT_ERR_EN_DN_DEV0_OFFSET)),
        0
        );

      for (Index = 0; Index < 8; Index++) {
        NbioRegisterRMW (
          GnbHandle,
          TYPE_SMN,
          NBIO_SPACE (GnbHandle, NBIF2_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYGX | (Index * 0x200)),
          (UINT32) ~(RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK_HYGX |
                     RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK_HYGX |
                     RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK),
          ((0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET_HYGX) |
           (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET_HYGX) |
           (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET)),
          0
          );
        NbioRegisterRMW (
          GnbHandle,
          TYPE_SMN,
          NBIO_SPACE (GnbHandle, NBIF3_RCC_DEV0_EPF0_STRAP2_ADDRESS_HYGX | (Index * 0x200)),
          (UINT32) ~(RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_MASK_HYGX |
                     RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_MASK_HYGX |
                     RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_MASK),
          ((0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_AER_EN_DEV0_F0_OFFSET_HYGX) |
           (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_ACS_EN_DEV0_F0_OFFSET_HYGX) |
           (0x1 << RCC_DEV0_EPFx_STRAP2_STRAP_CPL_ABORT_ERR_EN_DEV0_F0_OFFSET)),
          0
          );
      }
    }
  }

  // GNB_DEBUG_CODE (IDS_HDT_CONSOLE (GNB_TRACE, "PcieInterfaceInitSetting Exit\n"););
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
HsioCfgAfterTraining (
  IN       GNB_HANDLE       *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "HsioCfgAfterTraining Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        if (PcdGetBool (PcdCfgDisableRcvrResetCycle)) {
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
      PcieAfterTrainingDoneCallback (GnbHandle, PcieEngine, NULL);
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }

    Address = ConvertPcieCoreAddress (PCIE0_STRAP_F0_ADDRESS_HYGX, GnbHandle, PcieWrapper);
    Value   = (PcieWrapper->WrapId == 0) ? (UINT32)PcdGet8 (PcdGppAtomicOps) : (UINT32)PcdGet8 (PcdGfxAtomicOps);
    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32) ~(PCIE_STRAP_F0_STRAP_F0_ATOMIC_EN_MASK),
      (1 << PCIE_STRAP_F0_STRAP_F0_ATOMIC_EN_OFFSET),
      0
      );

    if (PcdGetBool (PcdPcieEcrcEnablement)) {
      IDS_HDT_CONSOLE (GNB_TRACE, "    Enable ECRC fearture \n");
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        Address,
        (UINT32) ~(PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_MASK | PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_MASK),
        (1 << PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_OFFSET) | (1 << PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_OFFSET),
        0
        );
    } else {
      IDS_HDT_CONSOLE (GNB_TRACE, "    Disable ECRC fearture \n");
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        Address,
        (UINT32) ~(PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_MASK | PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_MASK),
        (0 << PCIE_STRAP_F0_STRAP_F0_ECRC_CHECK_EN_OFFSET) | (0 << PCIE_STRAP_F0_STRAP_F0_ECRC_GEN_EN_OFFSET),
        0
        );
    }

    if (PcdGet8 (PcdRunEnvironment) >= 0) {
      // Enable PCIE ports AER capability for EMU and FPGA ENV
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        Address,
        (UINT32) ~(PCIE_STRAP_F0_STRAP_F0_AER_EN_MASK),
        (1 << PCIE_STRAP_F0_STRAP_F0_AER_EN_OFFSET),
        0
        );
      IDS_HDT_CONSOLE (GNB_TRACE, "    Enable AER Capability for PCIE Wrapper %d \n", PcieWrapper->WrapId);

      if (PcieWrapper->WrapId == 0) {
        StrapIndexAddr = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS);
        StrapDataAddr  = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS);
      } else if (PcieWrapper->WrapId == 1) {
        IDS_HDT_CONSOLE (GNB_TRACE, "   PcieWrapper->WrapId == 1 \n");
        StrapIndexAddr = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS);
        StrapDataAddr  = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS);
      } else {
        StrapIndexAddr = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE2_STRAP_INDEX_ADDRESS);
        StrapDataAddr  = PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE2_STRAP_DATA_ADDRESS);
      }

      // Enable PCIE ports DPC capability
      Value = 1;
      Index = STRAP_BIF_DPC_EN_ADDR;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );
      // Enable PCIE ports DLF capability
      Index = STRAP_BIF_DLF_EN_ADDR;
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapIndexAddr,
        &Index,
        0
        );
      NbioRegisterWrite (
        GnbHandle,
        TYPE_SMN,
        StrapDataAddr,
        &Value,
        0
        );
      IDS_HDT_CONSOLE (GNB_TRACE, "    Enable DPC & DLF Capability for PCIE Wrapper %d \n", PcieWrapper->WrapId);
    }

    for (PortIndex = 0; PortIndex < 8; PortIndex++) {

      //Set TX_ACK_LATENCY_ACCELERATE BIT31 as 1 to solve RxOF error
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIE_TX_ACK_LATENCY_LIMIT_HYGX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
        (UINT32) ~(TX_ACK_LATENCY_ACCELERATE_MASK),
        (UINT32)(0x1 << TX_ACK_LATENCY_ACCELERATE_OFFSET),
        0
        );

      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIEP_STRAP_MISC_instPCIE0_link_ADDRESS_HYGX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
        (UINT32) ~(PCIEP_STRAP_MISC_STRAP_EXTENDED_FMT_SUPPORTED_MASK | PCIEP_STRAP_MISC_STRAP_E2E_PREFIX_EN_MASK),
        (1 << PCIEP_STRAP_MISC_STRAP_EXTENDED_FMT_SUPPORTED_OFFSET) | (1 << PCIEP_STRAP_MISC_STRAP_E2E_PREFIX_EN_OFFSET),
        0
        );
    }

    for (PortIndex = 0; PortIndex < 8; PortIndex++) {
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIE0_DEVICE_CNTL2_ADDRESS_HYGX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
        (UINT32) ~(DEVICE_CNTL2_CPL_TIMEOUT_VALUE_MASK),
        (0x6 << DEVICE_CNTL2_CPL_TIMEOUT_VALUE_OFFSET),
        0
        );

      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIE0_LC_CNTL_ADDRESS_HYGX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
        (UINT32) ~(PCIE_LC_CNTL_LC_L1_IMMEDIATE_ACK_MASK),
        (0x0 << PCIE_LC_CNTL_LC_L1_IMMEDIATE_ACK_OFFSET),
        0
        );

      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIE0_LC_LINK_WIDTH_CNTL_ADDRESS_HYGX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
        (UINT32) ~(PCIE_LC_LINK_WIDTH_CNTL_LC_DUAL_END_RECONFIG_EN_MASK),
        (0x1 << PCIE_LC_LINK_WIDTH_CNTL_LC_DUAL_END_RECONFIG_EN_OFFSET),
        0
        );

      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIE0_PCIE_LC_CNTL2_ADDRESS_HYGX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
        (UINT32) ~(PCIE_LC_CNTL2_LC_ELEC_IDLE_MODE_MASK),
        (0x1 << PCIE_LC_CNTL2_LC_ELEC_IDLE_MODE_OFFSET),
        0
        );

      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIE0_PCIEP_HW_DEBUG_ADDRESS_HYGX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
        (UINT32) ~(PCIEP_HW_DEBUG_HW_15_DEBUG_MASK),
        (0x1 << PCIEP_HW_DEBUG_HW_15_DEBUG_OFFSET),
        0
        );
      // ECRC_ERR_SEVERITY
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        ConvertPciePortAddress (PCIE0_GPP0_UNCORR_ERR_SEVERITY_HYGX, GnbHandle, PcieWrapper, (UINT8)PortIndex),
        (UINT32) ~(0x80000),
        (0x1 << 19),
        0
        );
    }

    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      ConvertPcieCoreAddress (PCIE0_CNTL_ADDRESS_HYGX, GnbHandle, PcieWrapper),
      (UINT32) ~(PCIE_CNTL_RX_RCB_INVALID_SIZE_DIS_MASK),
      0x0 << PCIE_CNTL_RX_RCB_INVALID_SIZE_DIS_OFFSET,
      0
      );

    Address = ConvertPcieCoreAddress (PCIE0_P_CNTL_ADDRESS_HYGX, GnbHandle, PcieWrapper);
    if ( PcdGetBool (PcdHygonNbioReportEdbErrors)) {
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        Address,
        (UINT32) ~(PCIE_P_CNTL_P_IGNORE_EDB_ERR_MASK),
        0x0 << PCIE_P_CNTL_P_IGNORE_EDB_ERR_OFFSET,
        0
        );
    } else {
      NbioRegisterRMW (
        GnbHandle,
        TYPE_SMN,
        Address,
        (UINT32) ~(PCIE_P_CNTL_P_IGNORE_EDB_ERR_MASK),
        0x1 << PCIE_P_CNTL_P_IGNORE_EDB_ERR_OFFSET,
        0
        );
    }

    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      Address,
      (UINT32) ~(PCIE_P_CNTL_P_ELEC_IDLE_MODE_MASK),
      0x1 << PCIE_P_CNTL_P_ELEC_IDLE_MODE_OFFSET,               // change PCIE_P_CNTL from 3 to 0 math PCIE Spec
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
  UINT32  Value;
  UINT32  Index;
  UINT32  SubsystemDeviceId;
  UINT32  SubsystemVendorId;

  IDS_HDT_CONSOLE (GNB_TRACE, "SubsystemIdSetting Entry\n");

  if (GnbHandle->IohubPresent) {
    // NB ADAPTER D0F0
    Value = PcdGet32 (PcdCfgNbioSsid);
    IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbioSsid = %x\n", Value);
    if (Value != 0) {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBADAPTER_SSID_ADDRESS_HYGX), &Value, 0);
    }

    // IOMMU
    Value = PcdGet32 (PcdCfgIommuSsid);
    IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgIommuSsid = %x\n", Value);
    if (Value != 0) {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_IOMMU_SSID_ADDRESS_HYGX), &Value, 0);
    }
  }

  if (GnbHandle->DieType == IOD_EMEI) {
    if ((GnbHandle->RbId % 2) == 0) {
      // PSPCCP(NBIO0 NBIF0 Func2)
      Value = PcdGet32 (PcdCfgPspccpSsid);
      IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgPspccpSsid = %x\n", Value);
      if ((Value != 0) && (GnbHandle->RbId == 0)) {
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF0_FUNC2_SSID_ADDRESS_HYGX), &Value, 0);
      }

      // NTBCCP(NBIO2 NBIF0 Func2, NBIO0/2 NBIF0 Func3)
      Value = PcdGet32 (PcdCfgNtbccpSsid);
      IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNtbccpSsid = %x\n", Value);
      if (Value != 0) {
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF0_FUNC3_SSID_ADDRESS_HYGX), &Value, 0);
        if (GnbHandle->RbId != 0) {
          NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF0_FUNC2_SSID_ADDRESS_HYGX), &Value, 0);
        }
      }

      // NBIF Dummy Functions
      Value = PcdGet32 (PcdCfgNbifF0Ssid);
      IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbifF0Ssid = %x\n", Value);
      if (Value != 0) {
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF0_SSID_ADDRESS_HYGX), &Value, 0);
      }

      // NBIF Root Bridge Functions
      Value = PcdGet32 (PcdCfgNbifRCSsid);
      IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbifRCSsid = %x\n", Value);
      if (Value != 0) {
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF0RC_SSID_ADDRESS_HYGX), &Value, 0);
      }

      // NTB
      Value = PcdGet32 (PcdCfgNtbSsid);
      IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNtbSsid = %x\n", Value);
      if (Value != 0) {
        NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF0_NTB_SSID_ADDRESS_HYGX), &Value, 0);
      }
    }

    // PCIE
    SubsystemDeviceId = (UINT32)PcdGet16 (PcdHygonPcieSubsystemDeviceID);
    if (SubsystemDeviceId != 0) {
      Index = STRAP_BIF_SUBSYS_ID_ADDR;
      NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS), &Index, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS), &SubsystemDeviceId, 0);
      if ((GnbHandle->RbId % 2) == 0) {
        // NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS), &Index, 0);
        // NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS), &SubsystemDeviceId, 0);
      }
    }

    SubsystemVendorId = (UINT32)PcdGet16 (PcdHygonPcieSubsystemVendorID);
    if (SubsystemVendorId != 0) {
      Index = STRAP_BIF_SUBSYS_VEN_ID_ADDR;
      NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_INDEX_ADDRESS), &Index, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE0_STRAP_DATA_ADDRESS), &SubsystemVendorId, 0);
      if ((GnbHandle->RbId % 2) == 0) {
        // NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_INDEX_ADDRESS), &Index, 0);
        // NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE(GnbHandle, NBIO0_PCIE1_STRAP_DATA_ADDRESS), &SubsystemVendorId, 0);
      }
    }
  }

  if (GnbHandle->DieType == IOD_DUJIANG) {
    // PSPCCP(NBIO0 NBIF3 Func4)
    Value = PcdGet32 (PcdCfgPspccpSsid);
    IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgPspccpSsid = %x\n", Value);
    if (Value != 0) {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF3_FUNC4_SSID_ADDRESS_HYGX), &Value, 0);
    }

    // XGBE
    Value = PcdGet32 (PcdCfgXgbeSsid);
    IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgXgbeSsid = %x\n", Value);
    if ((Value != 0) && (GnbHandle->RbId == 0)) {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF2_XGBE0_SSID_ADDRESS_HYGX), &Value, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF2_XGBE2_SSID_ADDRESS_HYGX), &Value, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF2_XGBE3_SSID_ADDRESS_HYGX), &Value, 0);
    }

    // NBIF Dummy Functions
    Value = PcdGet32 (PcdCfgNbifF0Ssid);
    IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbifF0Ssid = %x\n", Value);
    if (Value != 0) {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF2_SSID_ADDRESS_HYGX), &Value, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF3_SSID_ADDRESS_HYGX), &Value, 0);
    }

    // NBIF Root Bridge Functions
    Value = PcdGet32 (PcdCfgNbifRCSsid);
    IDS_HDT_CONSOLE (GNB_TRACE, "PcdCfgNbifRCSsid = %x\n", Value);
    if (Value != 0) {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF2RC_SSID_ADDRESS_HYGX), &Value, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, PCICFG_NBIF3RC_SSID_ADDRESS_HYGX), &Value, 0);
    }

    // Audio controller
    Value = PcdGet32 (PcdCfgAzaliaSsid);
    if (Value != 0) {
      NbioRegisterWrite (GnbHandle, TYPE_SMN, NBIO_SPACE (GnbHandle, NBIF3EPF3CFG_ADAPTER_ID_ADDRESS_HYGX), &Value, 0);
      IDS_HDT_CONSOLE (GNB_TRACE, "NBIO HDAudio SSID = %x\n", Value);
    }

    // PCIE
    SubsystemDeviceId = (UINT32)PcdGet16 (PcdHygonPcieSubsystemDeviceID);
    if (SubsystemDeviceId != 0) {
      Index = STRAP_BIF_SUBSYS_ID_ADDR;
      NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE2_STRAP_INDEX_ADDRESS), &Index, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE2_STRAP_DATA_ADDRESS), &SubsystemDeviceId, 0);
    }

    SubsystemVendorId = (UINT32)PcdGet16 (PcdHygonPcieSubsystemVendorID);
    if (SubsystemVendorId != 0) {
      Index = STRAP_BIF_SUBSYS_VEN_ID_ADDR;
      NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE2_STRAP_INDEX_ADDRESS), &Index, 0);
      NbioRegisterWrite (GnbHandle, TYPE_SMN, PCIE_STRAP_SPACE (GnbHandle, NBIO0_PCIE2_STRAP_DATA_ADDRESS), &SubsystemVendorId, 0);
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
  EFI_STATUS                              Status;
  EFI_STATUS                              ReturnStatus;
  HGPI_STATUS                             HsioStatus;
  GNB_HANDLE                              *GnbHandle;
  UINT32                                  SmuArg[6];
  PCIe_PLATFORM_CONFIG                    *Pcie;
  PEI_HYGON_NBIO_PCIE_SERVICES_PPI        *PcieServicesPpi;
  PEI_HYGON_NBIO_SMU_SERVICES_PPI         *SmuServicesPpi;
  HYGON_MEMORY_INFO_HOB_PPI               *HygonMemoryInfoHob;
  GNB_PCIE_INFORMATION_DATA_HOB           *PciePlatformConfigHob;
  HYGON_PEI_FABRIC_TOPOLOGY_SERVICES_PPI  *FabricTopologyServicesPpi;
  HSIO_COMPLEX_DESCRIPTOR                 *PcieTopologyData;
  PEI_HYGON_NBIO_PCIE_COMPLEX_PPI         *NbioPcieComplexPpi;
  HYGON_PEI_SOC_LOGICAL_ID_PPI            *SocLogicalIdPpi;
  SOC_LOGICAL_ID                          LogicalId;

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
    PcieTopologyData->BmcLinkLocation = (PcdGet8 (PcdEarlyBmcLinkSocket) << 2) + PcdGet8 (PcdEarlyBmcLinkDie);
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

  LibHygonMemFill (SmuArg, 0x00, 24, (HYGON_CONFIG_PARAMS *)NULL);
  SmuArg[0] = 0xAA55AA55;
  SmuServicesPpi->SmuServiceRequest (SmuServicesPpi, 0, 0, 1, SmuArg, SmuArg);

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Configure Socket %d Die P%d L%d RB %d :\n", GnbHandle->SocketId, GnbHandle->PhysicalDieId, GnbHandle->LogicalDieId, GnbHandle->RbId);

    IDS_HOOK (IDS_HOOK_NBIO_PCIE_USER_CONFIG, (VOID *)GnbHandle, (VOID *)PcieTopologyData);
    // Call KPNP workaround
    PcieInterfaceInitSetting (GnbHandle, LogicalId);

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  SataChannelTypeSetting (PcieTopologyData);
  if (PcdGet8 (PcdRunEnvironment) == 0) {
    InitNTBFeature (PeiServices, PcieTopologyData, Pcie);
  }

  // At this point we know the NBIO topology and SMU services are available
  // We can now use SMU to initialize HSIO

  if (PcdGet8 (PcdRunEnvironment) == 0) {
    // Hsio Training
    HsioStatus = HsioEarlyInit (PeiServices, Pcie, PcieTopologyData);
    if (HsioStatus != HGPI_SUCCESS) {
      IDS_HDT_CONSOLE (GNB_TRACE, "HSIO ERROR!!\n");
      ReturnStatus = EFI_INVALID_PARAMETER;
      ASSERT (FALSE);
    }
  } else {
    // Simnow, EMU, FPGA env, bypass HSIO training
    GnbHandle = NbioGetHandle (Pcie);
    while (GnbHandle != NULL) {
      Pcie_Core0_Ports_Training (PeiServices, GnbHandle);
      Pcie_Core2_Ports_Training (PeiServices, GnbHandle);
      GnbHandle = GnbGetNextHandle (GnbHandle);
    }
  }

  PcieConfigDebugDump (Pcie);

  GnbHandle = NbioGetHandle (Pcie);
  while (GnbHandle != NULL) {
    // IDS_HOOK (IDS_HOOK_NBIO_PCIE_TOPOLOGY, (VOID *)GnbHandle, (VOID *)Pcie);
    if (PcdGet8 (PcdRunEnvironment) == 0) {
      HsioCfgAfterTraining (GnbHandle);
    }

    PciePortsVisibilityControlHyGx (HidePorts, GnbHandle);
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
    if (GnbHandle->IohubPresent) {
      if (TRUE == PcdGetBool (PcdCfgPcieAriSupport)) {
        NbioRegisterRMW (
          GnbHandle,
          TYPE_SMN,
          NBIO_SPACE (GnbHandle, IOC_FEATURE_CNTL_ADDRESS_HYGX),
          (UINT32) ~(NBMISC_0118_IOC_ARI_SUPPORTED_MASK),
          1 << NBMISC_0118_IOC_ARI_SUPPORTED_OFFSET,
          0
          );
      }
    }

    GnbHandle = GnbGetNextHandle (GnbHandle);
  }

  // Install PPI to notify other drivers that PCIe training is complete
  (**PeiServices).InstallPpi (PeiServices, &mNbioPcieTrainingDonePpiList);  // byo230825 -

  IDS_HDT_CONSOLE (GNB_TRACE, "HsioInitializationCallbackPpi Exit Status = 0x%x\n", ReturnStatus);
  HGPI_TESTPOINT (TpHsioInitializationCallbackPpiExit, NULL);
  return ReturnStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Wait for SMN Register xxx Bit x set
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  StallPpi          EFI_PEI_STALL_PPI pointer
 * @param[in]  SmnAddr           SMN register address
 * @param[in]  BitOffset         Bit offset
 */
VOID
WaitForBitSet (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_STALL_PPI         *StallPpi,
  IN GNB_HANDLE                *GnbHandle,
  IN UINT32                    SmnAddr,
  IN UINT32                    BitOffset
  )
{
  UINT32  Data32;
  UINT32  Cycle;

  for (Cycle = 0; Cycle < 50; Cycle++) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddr, &Data32, 0);
    Data32 &= (1 << BitOffset);
    if (Data32 != 0) {
      break;
    } else {
      StallPpi->Stall (PeiServices, StallPpi, 1000);
    }
  }

  if (Cycle == 50) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Wait for SMN Addr 0x%X bit %d set timeout !!! \n", SmnAddr, BitOffset);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Wait for SMN Register xxx Bit x clear
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 * @param[in]  StallPpi          EFI_PEI_STALL_PPI pointer
 * @param[in]  SmnAddr           SMN register address
 * @param[in]  BitOffset         Bit offset
 */
VOID
WaitForBitClear (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_STALL_PPI         *StallPpi,
  IN GNB_HANDLE                *GnbHandle,
  IN UINT32                    SmnAddr,
  IN UINT32                    BitOffset
  )
{
  UINT32  Data32;
  UINT32  Cycle;

  // Wait for BIT16 RESET_COMPLETE to set
  for (Cycle = 0; Cycle < 50; Cycle++) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddr, &Data32, 0);
    Data32 &= (1 << BitOffset);
    if (Data32 == 0) {
      break;
    } else {
      StallPpi->Stall (PeiServices, StallPpi, 1000);
    }
  }

  if (Cycle == 50) {
    IDS_HDT_CONSOLE (MAIN_FLOW, "Wait for SMN Addr 0x%X bit %d clear timeout !!! \n", SmnAddr, BitOffset);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Configure NBIOx PCIE core 2 ports
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 *
 */
VOID
Pcie_Core2_Ports_Training (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN GNB_HANDLE                *GnbHandle
  )
{
  UINT32             Data;
  EFI_PEI_STALL_PPI  *StallPpi;
  EFI_STATUS         Status;
  UINT32             Link_Config;
  UINT32             PortNumber;
  UINT32             PortIndex;
  UINT32             Cycle;
  UINT32             PhysicalDieId;

  if (GnbHandle->DieType != IOD_DUJIANG) {
    return;
  }

  PhysicalDieId = GnbHandle->PhysicalDieId;
  IDS_HDT_CONSOLE (MAIN_FLOW, "Physical die %d Pcie_Core2_Ports_Training entry \n", PhysicalDieId);

  PortNumber  = 1;
  Link_Config = 0x00;   // 0x00 - x16
                        // 0x0F - x4444
                        // 0x1F - x22222222

  Status = (**PeiServices).LocatePpi (PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, &StallPpi);

  // Bypass wait for Tx Phystatus hold
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYGX), &Data, 0);
  Data |= 1 << SWRST_GENERAL_CONTROL_BYPASS_PCS_HOLD_OFFSET;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYGX), &Data, 0);

  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, 0x10ed004), &Data, 0); // PCS22 COLD RESET
  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, 0x10ed008), &Data, 0); // PCS22 RSMU HARD RESET
  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, 0x10ae004), &Data, 0); // PCIe2 RSMU COLD RESET
  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, 0x10ae008), &Data, 0); // PCIe2 RSMU HARD RESET

  WaitForBitSet (
    PeiServices,
    StallPpi,
    GnbHandle,
    IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE2_ADDRESS_HYGX),
    SWRST_COMMAND_STATUS_RESET_COMPLETE_OFFSET
    );

  // PCIE core 2 BIT12:CONFIG_XFER_MODE=0, forward only when PCI-E core is in reset
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYGX), &Data, 0);
  Data &= (UINT32) ~(SWRST_GENERAL_CONTROL_CONFIG_XFER_MODE_MASK);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYGX), &Data, 0);

  // Set PCIE core 2 link config
  Data = STRAP_BIF_LINK_CONFIG_ADDR;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE2_STRAP_INDEX_ADDRESS), &Data, 0);
  Data = Link_Config;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE2_STRAP_DATA_ADDRESS), &Data, 0);

  // PCIE core 2 port orderen = 1
  Data = STRAP_BIF_LC_PORT_ORDER_EN_ADDR;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE2_STRAP_INDEX_ADDRESS), &Data, 0);
  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE2_STRAP_DATA_ADDRESS), &Data, 0);

  // Configure port x LANE number
  for (PortIndex = 0; PortIndex < PortNumber; PortIndex++) {
    Data = STRAP_BIF_PORT_OFFSET_A_ADDR + PortIndex;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE2_STRAP_INDEX_ADDRESS), &Data, 0);
    Data = PortIndex * (16 / PortNumber);       // Lane number
    NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE2_STRAP_DATA_ADDRESS), &Data, 0);
  }

  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE2_LC_SPEED_CNTL_ADDRESS_HYGX + (UINT32)0x0000), &Data, 0);
  Data = Data | 0x0800000f; // lc_gen5_en
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE2_LC_SPEED_CNTL_ADDRESS_HYGX + (UINT32)0x0000), &Data, 0);

  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE2_LINK_CNTL2_ADDRESS_HYGX + (UINT32)0x0000), &Data, 0);
  Data = Data & 0xfffffd8;
  Data = Data | 0x5;  // Target link speed
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE2_LINK_CNTL2_ADDRESS_HYGX + (UINT32)0x0000), &Data, 0);
  StallPpi->Stall (PeiServices, StallPpi, 100);

  // Enable reconfigure
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYGX), &Data, 0);
  Data |= 1 << SWRST_GENERAL_CONTROL_RECONFIGURE_EN_OFFSET;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYGX), &Data, 0);

  // Trigger reconfigure
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE2_ADDRESS_HYGX), &Data, 0);
  Data |= 1 << SWRST_COMMAND_STATUS_RECONFIGURE_OFFSET;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE2_ADDRESS_HYGX), &Data, 0);

  WaitForBitClear (
    PeiServices,
    StallPpi,
    GnbHandle,
    IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE2_ADDRESS_HYGX),
    SWRST_COMMAND_STATUS_RECONFIGURE_OFFSET
    );

  WaitForBitSet (
    PeiServices,
    StallPpi,
    GnbHandle,
    IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE2_ADDRESS_HYGX),
    SWRST_COMMAND_STATUS_RESET_COMPLETE_OFFSET
    );

  // PCIE core 2 BIT12:CONFIG_XFER_MODE=1, forward immediately
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYGX), &Data, 0);
  Data |= (1 << SWRST_GENERAL_CONTROL_CONFIG_XFER_MODE_OFFSET);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE2_ADDRESS_HYGX), &Data, 0);

  StallPpi->Stall (PeiServices, StallPpi, 20*1000);

  IoWrite32 (0x80, 0x8899E403);

  // Read PCIE2_P_CNTL BIT13 P_ALWAYS_USE_FAST_TXCLK
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE2_P_CNTL_ADDRESS_HYGX), &Data, 0);
  IDS_HDT_CONSOLE (MAIN_FLOW, "  PCIE0_P_CNTL is %x \n", Data);

  // PCIE core 2 hold training
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_CONTROL_6_PCIECORE2_ADDRESS_HYGX), &Data, 0);
  for (PortIndex = 0; PortIndex < PortNumber; PortIndex++) {
    Data = Data & (UINT32)(~(1 << PortIndex));
  }

  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_CONTROL_6_PCIECORE2_ADDRESS_HYGX), &Data, 0);

  // Read PCIE core 2 link config
  Data = STRAP_BIF_LINK_CONFIG_ADDR;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE2_STRAP_INDEX_ADDRESS), &Data, 0);
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE2_STRAP_DATA_ADDRESS), &Link_Config, 0);
  IDS_HDT_CONSOLE (MAIN_FLOW, "  PCIE Core 2 link_config %x \n", Link_Config);

  // Read PCIE core 2 ports PCIE_LC_STATE0
  Data  = 0;
  Cycle = 0;
  while ((Data & 0xFF) < 0x10) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE2_LC_STATE0_ADDRESS_HYGX), &Data, 0);
    StallPpi->Stall (PeiServices, StallPpi, 1000);
    Cycle++;
    if(Cycle == 20) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Wait for port 0 current LC state change timeout ! \n");
      break;
    }
  }

  for (PortIndex = 0; PortIndex < PortNumber; PortIndex++) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE2_LC_STATE0_ADDRESS_HYGX + (PortIndex << 12)), &Data, 0);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Port %d PCIE_LC_STATE0 %x \n", PortIndex, Data);

    NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE2_GPP0_LINK_CTL_STS_ADDRESS_HYGX + (PortIndex << 12)), &Data, 0);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Port %d Link CNTL STATUS %x \n", PortIndex, Data);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "Pcie_Core2_Ports_Training exit \n");
}

/*----------------------------------------------------------------------------------------*/

/**
 * Configure NBIOx PCIE core 0 ports
 *
 * @param[in]  PeiServices       Pointer to EFI_PEI_SERVICES pointer
 *
 */
VOID
Pcie_Core0_Ports_Training (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN GNB_HANDLE                 *GnbHandle
  )
{
  UINT32             Data;
  EFI_PEI_STALL_PPI  *StallPpi;
  EFI_STATUS         Status;
  UINT32             Link_Config;
  UINT32             PortNumber;
  UINT32             PortIndex;
  UINT32             Cycle;
  UINT32             PhysicalDieId;

  if (GnbHandle->DieType != IOD_EMEI) {
    return;
  }

  PhysicalDieId = GnbHandle->PhysicalDieId;
  IDS_HDT_CONSOLE (MAIN_FLOW, "Physical die %d Pcie_Core0_Ports_Training entry \n", PhysicalDieId);

  PortNumber  = 1;
  Link_Config = 0x00;   // 0x00 - x16
                        // 0x0F - x4444
                        // 0x1F - x22222222

  Status = (**PeiServices).LocatePpi (PeiServices, &gEfiPeiStallPpiGuid, 0, NULL, &StallPpi);

  // Bypass wait for Tx Phystatus hold
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYGX), &Data, 0);
  Data |= 1 << SWRST_GENERAL_CONTROL_BYPASS_PCS_HOLD_OFFSET;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYGX), &Data, 0);

  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, 0x10e7004), &Data, 0); // PCS16 COLD RESET
  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, 0x10e7008), &Data, 0); // PCS16 RSMU HARD RESET
  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, 0x1046004), &Data, 0); // PCIe0 RSMU COLD RESET
  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, 0x1046008), &Data, 0); // PCIe0 RSMU HARD RESET

  WaitForBitSet (
    PeiServices,
    StallPpi,
    GnbHandle,
    IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE0_ADDRESS_HYGX),
    SWRST_COMMAND_STATUS_RESET_COMPLETE_OFFSET
    );

  // PCIE core 0 BIT12:CONFIG_XFER_MODE=0, forward only when PCI-E core is in reset
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYGX), &Data, 0);
  Data &= (UINT32) ~(SWRST_GENERAL_CONTROL_CONFIG_XFER_MODE_MASK);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYGX), &Data, 0);

  // Set PCIE core 0 link config
  Data = STRAP_BIF_LINK_CONFIG_ADDR;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE0_STRAP_INDEX_ADDRESS), &Data, 0);
  Data = Link_Config;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE0_STRAP_DATA_ADDRESS), &Data, 0);

  // PCIE core 0 port orderen = 1
  Data = STRAP_BIF_LC_PORT_ORDER_EN_ADDR;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE0_STRAP_INDEX_ADDRESS), &Data, 0);
  Data = 1;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE0_STRAP_DATA_ADDRESS), &Data, 0);

  // Configure port x LANE number
  for (PortIndex = 0; PortIndex < PortNumber; PortIndex++) {
    Data = STRAP_BIF_PORT_OFFSET_A_ADDR + PortIndex;
    NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE0_STRAP_INDEX_ADDRESS), &Data, 0);
    Data = PortIndex * (16 / PortNumber);       // Lane number
    NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE0_STRAP_DATA_ADDRESS), &Data, 0);
  }

  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE0_LC_SPEED_CNTL_ADDRESS_HYGX + (UINT32)0x0000), &Data, 0);
  Data = Data | 0x0800000f; // lc_gen5_en
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE0_LC_SPEED_CNTL_ADDRESS_HYGX + (UINT32)0x0000), &Data, 0);

  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE0_LINK_CNTL2_ADDRESS_HYGX + (UINT32)0x0000), &Data, 0);
  Data = Data & 0xfffffd8;
  Data = Data | 0x5;  // Target link speed
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE0_LINK_CNTL2_ADDRESS_HYGX + (UINT32)0x0000), &Data, 0);
  StallPpi->Stall (PeiServices, StallPpi, 100);

  // Enable reconfigure
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYGX), &Data, 0);
  Data |= 1 << SWRST_GENERAL_CONTROL_RECONFIGURE_EN_OFFSET;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYGX), &Data, 0);

  // Trigger reconfigure
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE0_ADDRESS_HYGX), &Data, 0);
  Data |= 1 << SWRST_COMMAND_STATUS_RECONFIGURE_OFFSET;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE0_ADDRESS_HYGX), &Data, 0);

  WaitForBitClear (
    PeiServices,
    StallPpi,
    GnbHandle,
    IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE0_ADDRESS_HYGX),
    SWRST_COMMAND_STATUS_RECONFIGURE_OFFSET
    );

  WaitForBitSet (
    PeiServices,
    StallPpi,
    GnbHandle,
    IOD_SPACE (PhysicalDieId, SWRST_COMMAND_STATUS_PCIECORE0_ADDRESS_HYGX),
    SWRST_COMMAND_STATUS_RESET_COMPLETE_OFFSET
    );

  // PCIE core 0 BIT12:CONFIG_XFER_MODE=1, forward immediately
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYGX), &Data, 0);
  Data |= (1 << SWRST_GENERAL_CONTROL_CONFIG_XFER_MODE_OFFSET);
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_GENERAL_CONTROL_PCIECORE0_ADDRESS_HYGX), &Data, 0);

  StallPpi->Stall (PeiServices, StallPpi, 20*1000);

  IoWrite32 (0x80, 0x8899E403);

  // Read PCIE0_P_CNTL BIT13 P_ALWAYS_USE_FAST_TXCLK
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE0_P_CNTL_ADDRESS_HYGX), &Data, 0);
  IDS_HDT_CONSOLE (MAIN_FLOW, "  PCIE0_P_CNTL is %x \n", Data);

  // PCIE core 0 hold training
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_CONTROL_6_PCIECORE0_ADDRESS_HYGX), &Data, 0);
  for (PortIndex = 0; PortIndex < PortNumber; PortIndex++) {
    Data = Data & (UINT32)(~(1 << PortIndex));
  }

  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, SWRST_CONTROL_6_PCIECORE0_ADDRESS_HYGX), &Data, 0);

  // Read PCIE core 0 link config
  Data = STRAP_BIF_LINK_CONFIG_ADDR;
  NbioRegisterWrite (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE0_STRAP_INDEX_ADDRESS), &Data, 0);
  NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, NBIO0_PCIE0_STRAP_DATA_ADDRESS), &Link_Config, 0);
  IDS_HDT_CONSOLE (MAIN_FLOW, "  PCIE Core 0 link_config %x \n", Link_Config);

  // Read PCIE core 0 ports PCIE_LC_STATE0
  Data  = 0;
  Cycle = 0;
  while ((Data & 0xFF) < 0x10) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE0_LC_STATE0_ADDRESS_HYGX), &Data, 0);
    StallPpi->Stall (PeiServices, StallPpi, 1000);
    Cycle++;
    if(Cycle == 20) {
      IDS_HDT_CONSOLE (MAIN_FLOW, "  Wait for port 0 current LC state change timeout ! \n");
      break;
    }
  }

  for (PortIndex = 0; PortIndex < PortNumber; PortIndex++) {
    NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE0_LC_STATE0_ADDRESS_HYGX + (PortIndex << 12)), &Data, 0);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Port %d PCIE_LC_STATE0 %x \n", PortIndex, Data);

    NbioRegisterRead (GnbHandle, TYPE_SMN, IOD_SPACE (PhysicalDieId, PCIE0_GPP0_LINK_CTL_STS_ADDRESS_HYGX + (PortIndex << 12)), &Data, 0);
    IDS_HDT_CONSOLE (MAIN_FLOW, "  Port %d Link CNTL STATUS %x \n", PortIndex, Data);
  }

  IDS_HDT_CONSOLE (MAIN_FLOW, "Pcie_Core0_Ports_Training exit \n");
}
