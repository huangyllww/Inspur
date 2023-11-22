/* $NoKeywords:$ */

/**
 * @file
 *
 * GNB function to create/locate PCIe configuration data area
 *
 * Contain code that create/locate and rebase configuration data area.
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <GnbHsio.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/HygonBaseLib.h>
#include  <Library/HygonSocBaseLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/PcieInputParserLib.h>
#include  <HygonPcieComplex.h>
#include  <GnbRegisters.h>

#define FILECODE  LIBRARY_PCIECONFIGLIB_PCIECONFIGLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/**
 * get Master Lane of PCIe port engine
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 Master Engine Lane Number
 */
UINT8
PcieConfigGetPcieEngineMasterLane (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  UINT8                MasterLane;
  PCIe_WRAPPER_CONFIG  *Wrapper;

  ASSERT (PcieConfigIsPcieEngine (Engine));

  Wrapper = PcieConfigGetParentWrapper (Engine);
  if (Engine->EngineData.StartLane <= Engine->EngineData.EndLane) {
    MasterLane = (UINT8)(Engine->EngineData.StartLane - Wrapper->StartPhyLane);
  } else {
    MasterLane = (UINT8)(Engine->EngineData.EndLane - Wrapper->StartPhyLane);
  }

  return MasterLane;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get number of core lanes
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 Number of core lane
 */
UINT8
PcieConfigGetNumberOfCoreLane (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  if (Engine->Type.Port.StartCoreLane >= UNUSED_LANE_ID || Engine->Type.Port.EndCoreLane >= UNUSED_LANE_ID) {
    return 0;
  }

  return (UINT8)(Engine->Type.Port.EndCoreLane - Engine->Type.Port.StartCoreLane + 1);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Disable engine
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 */
VOID
PcieConfigDisableEngine (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  if (PcieConfigIsSbPcieEngine (Engine)) {
    return;
  }

  PcieConfigResetDescriptorFlags (Engine, DESCRIPTOR_ALLOCATED);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Disable all engines on wrapper
 *
 *
 *
 * @param[in]  EngineTypeMask Engine type bitmap.
 * @param[in]  Wrapper        Pointer to wrapper config descriptor
 */
VOID
PcieConfigDisableAllEngines (
  IN      UINTN                          EngineTypeMask,
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;

  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if ((EngineList->EngineData.EngineType & EngineTypeMask) != 0) {
      PcieConfigDisableEngine (EngineList);
    }

    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get engine PHY lanes bitmap
 *
 *
 *
 * @param[in]   Engine              Pointer to engine config descriptor
 */
UINT32
PcieConfigGetEnginePhyLaneBitMap (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  UINT32  LaneBitMap;

  LaneBitMap = 0;
  if (PcieLibIsEngineAllocated (Engine)) {
    LaneBitMap = ((1 << PcieConfigGetNumberOfPhyLane (Engine)) - 1) <<  (PcieLibGetLoPhyLane (Engine) - PcieConfigGetParentWrapper (Engine)->StartPhyLane);
  }

  return LaneBitMap;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get number of phy lanes
 *
 *
 *
 * @param[in]  Engine      Pointer to engine config descriptor
 * @retval                 Number of Phy lane
 */
UINT8
PcieConfigGetNumberOfPhyLane (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  if (Engine->EngineData.StartLane >= UNUSED_LANE_ID || Engine->EngineData.EndLane >= UNUSED_LANE_ID) {
    return 0;
  }

  if (Engine->EngineData.StartLane > Engine->EngineData.EndLane) {
    return (UINT8)(Engine->EngineData.StartLane - Engine->EngineData.EndLane + 1);
  } else {
    return (UINT8)(Engine->EngineData.EndLane - Engine->EngineData.StartLane + 1);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get port configuration signature for given wrapper and core
 *
 * Support for unify register access through index/data pair on GNB
 *
 * @param[in]  Wrapper     Pointer to wrapper config descriptor
 * @param[in]  CoreId      Core ID
 * @retval                 Configuration Signature
 */
UINT64
PcieConfigGetConfigurationSignature (
  IN      PCIe_WRAPPER_CONFIG            *Wrapper,
  IN      UINT8                          CoreId
  )
{
  UINT64              ConfigurationSignature;
  PCIe_ENGINE_CONFIG  *EngineList;

  ConfigurationSignature = 0;
  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieConfigIsPcieEngine (EngineList) && EngineList->Type.Port.CoreId == CoreId) {
      ConfigurationSignature = (ConfigurationSignature << 8) | PcieConfigGetNumberOfCoreLane (EngineList);
    }

    EngineList = PcieLibGetNextDescriptor (EngineList);
  }

  IDS_HDT_CONSOLE (PCIE_MISC, "  CoreId - %x      ConfigurationSignature - %x\n", CoreId, ConfigurationSignature);
  return ConfigurationSignature;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Check Port Status
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  PortStatus      Check if status asserted for port
 * @retval                     TRUE if status asserted
 */
BOOLEAN
PcieConfigCheckPortStatus (
  IN       PCIe_ENGINE_CONFIG            *Engine,
  IN       UINT32                        PortStatus
  )
{
  return (Engine->InitStatus & PortStatus) == 0 ? FALSE : TRUE;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Set/Reset port status
 *
 *
 *
 * @param[in]  Engine          Pointer to engine config descriptor
 * @param[in]  SetStatus       SetStatus
 * @param[in]  ResetStatus     ResetStatus
 *
 */
UINT16
PcieConfigUpdatePortStatus (
  IN       PCIe_ENGINE_CONFIG               *Engine,
  IN       PCIe_ENGINE_INIT_STATUS          SetStatus,
  IN       PCIe_ENGINE_INIT_STATUS          ResetStatus
  )
{
  Engine->InitStatus |= SetStatus;
  Engine->InitStatus &= (~ResetStatus);
  return Engine->InitStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Execute callback on all descriptor of specific type
 *
 *
 * @param[in]       InDescriptorFlags    Include descriptor flags
 * @param[in]       OutDescriptorFlags   Exclude descriptor flags
 * @param[in]       TerminationFlags     Termination flags
 * @param[in]       Callback             Pointer to callback function
 * @param[in, out]  Buffer               Pointer to buffer to pass information to callback
 * @param[in]       Pcie                 Pointer to global PCIe configuration
 */
HGPI_STATUS
PcieConfigRunProcForAllDescriptors (
  IN       UINT32                           InDescriptorFlags,
  IN       UINT32                           OutDescriptorFlags,
  IN       UINT32                           TerminationFlags,
  IN       PCIe_RUN_ON_DESCRIPTOR_CALLBACK  Callback,
  IN OUT   VOID                             *Buffer,
  IN       PCIe_PLATFORM_CONFIG             *Pcie
  )
{
  HGPI_STATUS             HgpiStatus;
  HGPI_STATUS             Status;
  PCIe_DESCRIPTOR_HEADER  *Descriptor;

  HgpiStatus = HGPI_SUCCESS;
  Descriptor = PcieConfigGetChild (InDescriptorFlags & DESCRIPTOR_ALL_TYPES, &Pcie->Header);
  while (Descriptor != NULL) {
    if ((InDescriptorFlags & Descriptor->DescriptorFlags) != 0 && (OutDescriptorFlags && Descriptor->DescriptorFlags) == 0) {
      Status = Callback (Descriptor, Buffer, Pcie);
      HGPI_STATUS_UPDATE (Status, HgpiStatus);
    }

    Descriptor = (PCIe_DESCRIPTOR_HEADER *)PcieConfigGetNextTopologyDescriptor (Descriptor, TerminationFlags);
  }

  return HgpiStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Execute callback on all wrappers in topology
 *
 *
 * @param[in]       DescriptorFlags   Wrapper Flags
 * @param[in]       Callback          Pointer to callback function
 * @param[in, out]  Buffer            Pointer to buffer to pass information to callback
 * @param[in]       Pcie              Pointer to global PCIe configuration
 */
HGPI_STATUS
PcieConfigRunProcForAllWrappers (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_WRAPPER_CALLBACK  Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  HGPI_STATUS          HgpiStatus;
  HGPI_STATUS          Status;
  PCIe_WRAPPER_CONFIG  *Wrapper;

  HgpiStatus = HGPI_SUCCESS;
  Wrapper    = (PCIe_WRAPPER_CONFIG *)PcieConfigGetChild (DESCRIPTOR_ALL_WRAPPERS, &Pcie->Header);
  while (Wrapper != NULL) {
    if ((DescriptorFlags & DESCRIPTOR_ALL_WRAPPERS & Wrapper->Header.DescriptorFlags) != 0) {
      Status = Callback (Wrapper, Buffer, Pcie);
      HGPI_STATUS_UPDATE (Status, HgpiStatus);
    }

    Wrapper = (PCIe_WRAPPER_CONFIG *)PcieConfigGetNextTopologyDescriptor (Wrapper, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }

  return HgpiStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Execute callback on all wrappers in NBIO
 *
 *
 * @param[in]       DescriptorFlags   Wrapper Flags
 * @param[in]       Callback          Pointer to callback function
 * @param[in, out]  Buffer            Pointer to buffer to pass information to callback
 * @param[in]       Pcie              Pointer to global PCIe configuration
 */
VOID
PcieConfigRunProcForAllWrappersInNbio (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_WRAPPER_CALLBACK2 Callback,
  IN OUT   VOID                          *Buffer,
  IN       GNB_HANDLE                    *GnbHandle
  )
{
  PCIe_WRAPPER_CONFIG  *Wrapper;

  Wrapper = PcieConfigGetChildWrapper (GnbHandle);
  while ((Wrapper != NULL) && (GnbHandle == (GNB_HANDLE *)PcieConfigGetParentSilicon (Wrapper))) {
    if ((DescriptorFlags & DESCRIPTOR_ALL_WRAPPERS & Wrapper->Header.DescriptorFlags) != 0) {
      Callback (Wrapper, Buffer, GnbHandle);
    }

    Wrapper = (PCIe_WRAPPER_CONFIG *)PcieConfigGetNextTopologyDescriptor (Wrapper, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }

  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Execute callback on all engine in topology
 *
 *
 * @param[in]       DescriptorFlags Engine flags.
 * @param[in]       Callback        Pointer to callback function
 * @param[in, out]  Buffer          Pointer to buffer to pass information to callback
 * @param[in]       Pcie            Pointer to global PCIe configuration
 */
VOID
PcieConfigRunProcForAllEngines (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_ENGINE_CALLBACK   Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_PLATFORM_CONFIG          *Pcie
  )
{
  PCIe_ENGINE_CONFIG  *Engine;

  Engine = (PCIe_ENGINE_CONFIG *)PcieConfigGetChild (DESCRIPTOR_ALL_ENGINES, &Pcie->Header);
  while (Engine != NULL) {
    if (!((DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0 && !PcieLibIsEngineAllocated (Engine))) {
      if ((Engine->Header.DescriptorFlags & DESCRIPTOR_ALL_ENGINES & DescriptorFlags) != 0) {
        Callback (Engine, Buffer, Pcie);
      }
    }

    Engine = (PCIe_ENGINE_CONFIG *)PcieConfigGetNextTopologyDescriptor (Engine, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Execute callback on all engine in wrapper
 *
 *
 * @param[in]       DescriptorFlags Engine flags.
 * @param[in]       Callback        Pointer to callback function
 * @param[in, out]  Buffer          Pointer to buffer to pass information to callback
 * @param[in]       Pcie            Pointer to global PCIe configuration
 */
VOID
PcieConfigRunProcForAllEnginesInWrapper (
  IN       UINT32                        DescriptorFlags,
  IN       PCIe_RUN_ON_ENGINE_CALLBACK2  Callback,
  IN OUT   VOID                          *Buffer,
  IN       PCIe_WRAPPER_CONFIG           *Wrapper
  )
{
  PCIe_ENGINE_CONFIG  *Engine;

  Engine = PcieConfigGetChildEngine (Wrapper);
  while ((Engine != NULL) && (Wrapper == PcieConfigGetParentWrapper (Engine))) {
    if (!((DescriptorFlags & DESCRIPTOR_ALLOCATED) != 0 && !PcieLibIsEngineAllocated (Engine))) {
      if ((Engine->Header.DescriptorFlags & DESCRIPTOR_ALL_ENGINES & DescriptorFlags) != 0) {
        Callback (Engine, Buffer, Wrapper);
      }
    }

    Engine = (PCIe_ENGINE_CONFIG *)PcieConfigGetNextTopologyDescriptor (Engine, DESCRIPTOR_TERMINATE_TOPOLOGY);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get parent descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetParent (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Parent != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *)((UINT8 *)Descriptor - Descriptor->Parent);
    } else {
      return NULL;
    }
  }

  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get child descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetChild (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Child != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *)((UINT8 *)Descriptor + Descriptor->Child);
    } else {
      return NULL;
    }
  }

  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get peer descriptor of specific type
 *
 *
 * @param[in]       Type            Descriptor type
 * @param[in]       Descriptor      Pointer to buffer to pass information to callback
 */
PCIe_DESCRIPTOR_HEADER *
PcieConfigGetPeer (
  IN       UINT32                        Type,
  IN       PCIe_DESCRIPTOR_HEADER        *Descriptor
  )
{
  ASSERT (Descriptor != NULL);
  while ((Descriptor->DescriptorFlags & Type) == 0) {
    if (Descriptor->Peer != 0) {
      Descriptor = (PCIe_DESCRIPTOR_HEADER *)((UINT8 *)Descriptor + Descriptor->Peer);
    } else {
      return NULL;
    }
  }

  return Descriptor;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Check is engine is active or potentially active
 *
 *
 *
 * @param[in]  Engine      Pointer to engine descriptor
 * @retval                 TRUE  - engine active
 * @retval                 FALSE - engine not active
 */
BOOLEAN
PcieConfigIsActivePcieEngine (
  IN      PCIe_ENGINE_CONFIG             *Engine
  )
{
  BOOLEAN  Result;

  ASSERT (PcieConfigIsPcieEngine (Engine));
  Result = FALSE;
  if (PcieConfigIsEngineAllocated (Engine)) {
    if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS) ||
        (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled && Engine->Type.Port.PortData.LinkHotplug != HotplugInboard)) {
      Result = TRUE;
    }
  }

  return Result;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Locate SB engine on wrapper
 *
 *
 *
 * @param[in]  Wrapper     Pointer to wrapper config descriptor
 * @retval                 SB engine pointer or NULL
 */
PCIe_ENGINE_CONFIG *
PcieConfigLocateSbEngine (
  IN      PCIe_WRAPPER_CONFIG            *Wrapper
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;

  EngineList = PcieConfigGetChildEngine (Wrapper);
  while (EngineList != NULL) {
    if (PcieConfigIsSbPcieEngine (EngineList)) {
      return EngineList;
    }

    EngineList = PcieLibGetNextDescriptor (EngineList);
  }

  return NULL;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Helper function to dump engine configuration
 *
 *
 * @param[in]  EngineList           Engine Configuration
 */
VOID
PcieConfigEngineDebugDump (
  IN      PCIe_ENGINE_CONFIG        *EngineList
  )
{
  IDS_HDT_CONSOLE (PCIE_MISC, "        Descriptor Flags - 0x%08x\n", EngineList->Header.DescriptorFlags);
  IDS_HDT_CONSOLE (PCIE_MISC, "        Engine Type - PCIe Port\n        Start Phy Lane - %d\n        End   Phy Lane - %d\n",
    EngineList->EngineData.StartLane,
    EngineList->EngineData.EndLane
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "        Scratch        - %d\n", EngineList->Scratch);
  IDS_HDT_CONSOLE (PCIE_MISC, "        Init Status    - 0x%08x\n", EngineList->InitStatus);
  IDS_HDT_CONSOLE (PCIE_MISC, "        PCIe port configuration:\n");
  IDS_HDT_CONSOLE (PCIE_MISC, "          Port Training - %a\n",
    (EngineList->Type.Port.PortData.PortPresent == PortDisabled) ? "Disable" : "Enabled"
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "          Requested PCI Dev Number   - %d\n", EngineList->Type.Port.PortData.DeviceNumber);
  IDS_HDT_CONSOLE (PCIE_MISC, "          Requested PCI Func Number  - %d\n", EngineList->Type.Port.PortData.FunctionNumber);
  IDS_HDT_CONSOLE (PCIE_MISC, "          PCI Address - %d:%d:%d\n",
    EngineList->Type.Port.Address.Address.Bus,
    EngineList->Type.Port.Address.Address.Device,
    EngineList->Type.Port.Address.Address.Function
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "          EngineId - %d\n",
    EngineList->Type.Port.PcieBridgeId
    );
  IDS_HDT_CONSOLE (PCIE_MISC, "          Misc Control  - 0x%02x\n", EngineList->Type.Port.PortData.MiscControls);

  IDS_HDT_CONSOLE (PCIE_MISC, "          Hotplug - %a\n",
    (EngineList->Type.Port.PortData.LinkHotplug == HotplugDisabled) ? "Disabled" : \
    (EngineList->Type.Port.PortData.LinkHotplug == HotplugBasic) ? "Basic" : \
    (EngineList->Type.Port.PortData.LinkHotplug == HotplugServer) ? "Server" : \
    (EngineList->Type.Port.PortData.LinkHotplug == HotplugEnhanced) ? "Enhanced" : \
    (EngineList->Type.Port.PortData.LinkHotplug == HotplugServerSsd) ? "ServerSsd" : \
    (EngineList->Type.Port.PortData.LinkHotplug == HotplugInboard) ? "Inboard" : "Unknown"
    );
  ASSERT (EngineList->Type.Port.PortData.LinkHotplug < MaxHotplug);
  IDS_HDT_CONSOLE (PCIE_MISC, "          ASPM    - %a\n",
    (EngineList->Type.Port.PortData.LinkAspm == AspmDisabled) ? "Disabled" : \
    (EngineList->Type.Port.PortData.LinkAspm == AspmL0s) ? "L0s" : \
    (EngineList->Type.Port.PortData.LinkAspm == AspmL1) ? "L1" : \
    (EngineList->Type.Port.PortData.LinkAspm == AspmL0sL1) ? "L0s & L1" : "Unknown"
    );
  ASSERT (EngineList->Type.Port.PortData.LinkAspm < MaxAspm);
  IDS_HDT_CONSOLE (PCIE_MISC, "          Speed   - %d\n", EngineList->Type.Port.PortData.LinkSpeedCapability);

}

/*----------------------------------------------------------------------------------------*/

/**
 * Helper function to dump wrapper configuration
 *
 *
 * @param[in]  WrapperList           Wrapper Configuration
 */
VOID
PcieConfigWrapperDebugDump (
  IN      PCIe_WRAPPER_CONFIG        *WrapperList
  )
{
  PCIe_ENGINE_CONFIG  *EngineList;

  IDS_HDT_CONSOLE (PCIE_MISC, "      <---------Wrapper %d -  Config -------->\n", WrapperList->WrapId);
  // PcieFmDebugGetWrapperNameString (WrapperList)
  // );
  IDS_HDT_CONSOLE (PCIE_MISC, "        Start PHY lane - %02d\n", WrapperList->StartPhyLane);
  IDS_HDT_CONSOLE (PCIE_MISC, "        End   PHY lane - %02d\n", WrapperList->EndPhyLane);
  IDS_HDT_CONSOLE (PCIE_MISC, "        Descriptor Flags - 0x%08x\n", WrapperList->Header.DescriptorFlags);
  IDS_HDT_CONSOLE (PCIE_MISC, "        PowerOffUnusedLanes - %x\n        PowerOffUnusedPlls - %x\n        ClkGating - %x\n",
        WrapperList->Features.PowerOffUnusedLanes,
        WrapperList->Features.PowerOffUnusedPlls,
        WrapperList->Features.ClkGating
        );

  IDS_HDT_CONSOLE (PCIE_MISC, "        LclkGating - %x\n        TxclkGatingPllPowerDown - %x\n        PllOffInL1 - %x\n",
        WrapperList->Features.LclkGating,
        WrapperList->Features.TxclkGatingPllPowerDown,
        WrapperList->Features.PllOffInL1
        );
  IDS_HDT_CONSOLE (PCIE_MISC, "      <---------Wrapper -  Config End----->\n");
  // PcieFmDebugGetWrapperNameString (WrapperList)
  // );
  EngineList = PcieConfigGetChildEngine (WrapperList);
  while (EngineList != NULL) {
    if (PcieLibIsEngineAllocated (EngineList)) {
      PcieConfigEngineDebugDump (EngineList);
    }

    EngineList = PcieLibGetNextDescriptor (EngineList);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Helper function to dump configuration to debug out
 *
 *
 * @param[in]  Pcie                Pointer to global PCIe configuration
 */
VOID
PcieConfigDebugDump (
  IN      PCIe_PLATFORM_CONFIG        *Pcie
  )
{
  PCIe_SILICON_CONFIG  *SiliconList;
  PCIe_WRAPPER_CONFIG  *WrapperList;
  PCIe_COMPLEX_CONFIG  *ComplexList;

  ComplexList = (PCIe_COMPLEX_CONFIG *)PcieConfigGetChild (DESCRIPTOR_COMPLEX, &Pcie->Header);
  IDS_HDT_CONSOLE (PCIE_MISC, "<-------------- PCIe Config Start------------>\n");
  IDS_HDT_CONSOLE (
    PCIE_MISC,
    "  PSPP Policy - %a\n",
    (Pcie->PsppPolicy == PsppPowerSaving) ? "Power Saving" :
    (Pcie->PsppPolicy == PsppBalanceHigh) ? "Balance-High" : (
                                                              (Pcie->PsppPolicy == PsppBalanceLow) ? "Balance-Low" : (
                                                                                                                      (Pcie->PsppPolicy == PsppPerformance) ? "Performance" : (
                                                                                                                                                                               (Pcie->PsppPolicy == PsppDisabled) ? "Disabled" : "Unknown")))
    );
  IDS_HDT_CONSOLE (
    PCIE_MISC,
    "  GFX Workaround - %a\n",
    (Pcie->GfxCardWorkaround == 0) ? "Disabled" : "Enabled"
    );
  IDS_HDT_CONSOLE (
    PCIE_MISC,
    "  LinkL0Pooling - %dus\n",
    Pcie->LinkL0Pooling
    );
  IDS_HDT_CONSOLE (
    PCIE_MISC,
    "  LinkGpioResetAssertionTime - %dus\n",
    Pcie->LinkGpioResetAssertionTime
    );
  IDS_HDT_CONSOLE (
    PCIE_MISC,
    "  LinkReceiverDetectionPooling - %dus\n",
    Pcie->LinkReceiverDetectionPooling
    );
  while (ComplexList != NULL) {
    IDS_HDT_CONSOLE (PCIE_MISC, "  <---------- Complex Config Start ---------->\n");
    IDS_HDT_CONSOLE (PCIE_MISC, "    Descriptor Flags - 0x%08x\n", ComplexList->Header.DescriptorFlags);
    IDS_HDT_CONSOLE (PCIE_MISC, "    Node ID - %d\n", ComplexList->NodeId);
    SiliconList = PcieConfigGetChildSilicon (ComplexList);
    while (SiliconList != NULL) {
      IDS_HDT_CONSOLE (PCIE_MISC, "    <---------- Silicon Config Start -------->\n");
      IDS_HDT_CONSOLE (PCIE_MISC, "      Descriptor Flags - 0x%08x\n", SiliconList->Header.DescriptorFlags);
      IDS_HDT_CONSOLE (PCIE_MISC, "      SocketID - %d\n", SiliconList->SocketId);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Physical Die ID - %d\n", SiliconList->PhysicalDieId);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Logical Die ID - %d\n", SiliconList->LogicalDieId);
      IDS_HDT_CONSOLE (GNB_TRACE, "      Global IodId: %d\n", SiliconList->GlobalIodId);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Die Type - %a\n", (SiliconList->DieType == IOD_EMEI) ? "EMEI" : (SiliconList->DieType == IOD_DUJIANG) ? "DUJIANG" : "Unknown");
      IDS_HDT_CONSOLE (PCIE_MISC, "      Root bridge Id - %d\n", SiliconList->RbId);
      IDS_HDT_CONSOLE (PCIE_MISC, "      IohubPresent - %d\n", SiliconList->IohubPresent);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Address: 0x%08X\n", SiliconList->Address.AddressValue);
      IDS_HDT_CONSOLE (PCIE_MISC, "      Host PCI Address - %X:%X:%X\n",
        SiliconList->Address.Address.Bus,
        SiliconList->Address.Address.Device,
        SiliconList->Address.Address.Function
        );
      IDS_HDT_CONSOLE (PCIE_MISC, "      StartLane: %d\n", SiliconList->StartLane);
      IDS_HDT_CONSOLE (PCIE_MISC, "      EndLane: %d\n", SiliconList->EndLane);
      IDS_HDT_CONSOLE (PCIE_MISC, "      IodStartLane: %d\n", SiliconList->IodStartLane);
      IDS_HDT_CONSOLE (PCIE_MISC, "      IodEndLane: %d\n", SiliconList->IodEndLane);
      WrapperList = PcieConfigGetChildWrapper (SiliconList);
      while (WrapperList != NULL) {
        PcieConfigWrapperDebugDump (WrapperList);
        WrapperList = PcieLibGetNextDescriptor (WrapperList);
      }

      IDS_HDT_CONSOLE (PCIE_MISC, "    <---------- Silicon Config End ---------->\n");
      SiliconList = PcieLibGetNextDescriptor (SiliconList);
    }

    IDS_HDT_CONSOLE (PCIE_MISC, "  <---------- Complex Config End ------------>\n");
    // ComplexList = PcieLibGetNextDescriptor (ComplexList);
    if ((ComplexList->Header.DescriptorFlags & DESCRIPTOR_TERMINATE_TOPOLOGY) == 0) {
      ComplexList++;
    } else {
      ComplexList = NULL;
    }
  }

  IDS_HDT_CONSOLE (PCIE_MISC, "<-------------- PCIe Config End-------------->\n");
}

/*----------------------------------------------------------------------------------------*/

/**
 * Helper function to dump input configuration to user engine descriptor
 *
 *
 * @param[in]  EngineDescriptor   Pointer to engine descriptor
 */
VOID
PcieUserDescriptorConfigDump (
  IN      PCIe_ENGINE_DESCRIPTOR      *EngineDescriptor
  )
{
  IDS_HDT_CONSOLE (
    PCIE_MISC,
    "  Engine Type - %a\n",
    (EngineDescriptor->EngineData.EngineType == PciePortEngine) ? "PCIe Port" : (
                                                                                 (EngineDescriptor->EngineData.EngineType == PcieDdiEngine) ? "DDI Link" : (
                                                                                                                                                            (EngineDescriptor->EngineData.EngineType == PcieUnusedEngine) ? "Unused" : "Invalid"))
    );
  IDS_HDT_CONSOLE (
    PCIE_MISC,
    "    Start Phy Lane - %d\n    End   Phy Lane - %d\n",
    EngineDescriptor->EngineData.StartLane,
    EngineDescriptor->EngineData.EndLane
    );
  if (EngineDescriptor->EngineData.EngineType == PciePortEngine) {
    IDS_HDT_CONSOLE (
      PCIE_MISC,
      "    PortPresent - %d\n    ChannelType  - %d\n    DeviceNumber - %d\n    FunctionNumber - %d\n    LinkSpeedCapability - %d\n    LinkAspm - %d\n    LinkHotplug - %d\n    ResetId - %d\n    SB link - %d\n    MiscControls - 0x%02x\n",
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.PortPresent,
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.ChannelType,
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.DeviceNumber,
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.FunctionNumber,
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.LinkSpeedCapability,
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.LinkAspm,
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.LinkHotplug,
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.ResetId,
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.MiscControls.SbLink,
      ((PCIe_PORT_DESCRIPTOR *)EngineDescriptor)->Port.MiscControls
      );
  }

  if (EngineDescriptor->EngineData.EngineType == PcieDdiEngine) {
    IDS_HDT_CONSOLE (
      PCIE_MISC,
      "    ConnectorType - %d\n    AuxIndex  - %d\n    HdpIndex - %d\n",
      ((PCIe_DDI_DESCRIPTOR *)EngineDescriptor)->Ddi.ConnectorType,
      ((PCIe_DDI_DESCRIPTOR *)EngineDescriptor)->Ddi.AuxIndex,
      ((PCIe_DDI_DESCRIPTOR *)EngineDescriptor)->Ddi.HdpIndex
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
PcieUserConfigConfigDump (
  IN      PCIe_COMPLEX_DESCRIPTOR     *ComplexDescriptor
  )
{
  PCIe_ENGINE_DESCRIPTOR   *EngineDescriptor;
  PCIe_COMPLEX_DESCRIPTOR  *CurrentComplexDescriptor;
  UINTN                    ComplexIndex;
  UINTN                    Index;
  UINTN                    NumberOfEngines;
  UINTN                    NumberOfComplexes;

  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config Start------------->\n");

  NumberOfComplexes = PcieInputParserGetNumberOfComplexes (ComplexDescriptor);
  for (ComplexIndex = 0; ComplexIndex < NumberOfComplexes; ++ComplexIndex) {
    CurrentComplexDescriptor = PcieInputParserGetComplexDescriptor (ComplexDescriptor, ComplexIndex);
    NumberOfEngines = PcieInputParserGetNumberOfEngines (CurrentComplexDescriptor);
    IDS_HDT_CONSOLE (
      PCIE_MISC,
      "  ComplexDescriptor Node - %d\n  NumberOfEngines - %d\n",
      ComplexDescriptor->SocketId,
      NumberOfEngines
      );

    for (Index = 0; Index < NumberOfEngines; Index++) {
      EngineDescriptor = PcieInputParserGetEngineDescriptor (ComplexDescriptor, Index);
      PcieUserDescriptorConfigDump (EngineDescriptor);
    }
  }

  IDS_HDT_CONSOLE (PCIE_MISC, "<---------- PCIe User Config End-------------->\n");
}

/*----------------------------------------------------------------------------------------*/

/**
 * Get IOHC Bridge control address
 *
 *
 * @param[in]
 * GnbHandle      Pointer to the Silicon Descriptor for this NBIO
 * Engine         Pointer to a Pcie Engine
 */
UINT32
GetIohcBridgeCntlAddress (
  IN      GNB_HANDLE            *GnbHandle,
  IN      PCIe_ENGINE_CONFIG    *Engine
  )
{
  UINT32  SmnAddress;

  SmnAddress = (GnbHandle->RbId << 20) + NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYGX;

  SmnAddress += Engine->Type.Port.LogicalBridgeId << 10;

  return SmnAddress;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Convert a PCIE core address to other NBIO's PCIE core address
 *
 *
 * @param[in]
 * Address        A Pcie core register
 * GnbHandle      Pointer to the Silicon Descriptor for this NBIO
 * PcieWrapper    Pointer to a PcieWrapper
 */
UINT32
ConvertPcieCoreAddress (
  IN      UINT32                Address,
  IN      GNB_HANDLE            *GnbHandle,
  IN      PCIe_WRAPPER_CONFIG   *PcieWrapper
  )
{
  UINT32  PcieCoreReg;
  UINT32  HygonCpuModel;

  HygonCpuModel = GetHygonSocModel();
  if (HygonCpuModel == HYGON_EX_CPU) {
    //HyEx
    if (PcieWrapper->WrapId == 0) {
      PcieCoreReg = NBIO0_PCIE0_APERTURE_ID_HYEX << 20;
    } else if (PcieWrapper->WrapId == 1) {
      PcieCoreReg = NBIO0_PCIE1_APERTURE_ID_HYEX << 20;
    } else {
      PcieCoreReg = NBIO0_PCIE2_APERTURE_ID_HYEX << 20;
    }
    PcieCoreReg = (UINT32)(GnbHandle->RbId << 20) + PcieCoreReg + (Address & 0xFFFFF);

  } else {
    //HyGx
    if (PcieWrapper->WrapId == 0) {
      PcieCoreReg = NBIO0_PCIE0_APERTURE_ID_HYGX << 20;
    } else if (PcieWrapper->WrapId == 1) {
      PcieCoreReg = NBIO0_CXL_APERTURE_ID_HYGX << 20;
    } else {
      PcieCoreReg = NBIO0_PCIE2_APERTURE_ID_HYGX << 20;
    }
    PcieCoreReg = IOD0_SMN_BASE_HYGX + (UINT32)(GnbHandle->PhysicalDieId << 28) + (UINT32)(GnbHandle->RbId << 20) + PcieCoreReg + (Address & 0xFFFFF);
  }
  
  return PcieCoreReg;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Convert a PCIE port address to other NBIO or wrapper's PCIE port address
 *
 *
 * @param[in]
 * Address        A Pcie port register
 * GnbHandle      Pointer to the Silicon Descriptor for this NBIO
 * PcieWrapper    Pointer to a Pcie Wrapper
 * PortIndex      A PortIndex
 * 
 * @return UINT32 Converted PCIE port address
 */
UINT32
ConvertPciePortAddress (
  IN      UINT32                Address,
  IN      GNB_HANDLE            *GnbHandle,
  IN      PCIe_WRAPPER_CONFIG   *PcieWrapper,
  IN      UINT8                 PortIndex
  )
{
  UINT32  PciePortReg;

  PciePortReg = ConvertPcieCoreAddress (Address, GnbHandle, PcieWrapper);
  PciePortReg = (PciePortReg & 0xFFFF0FFF) + (PortIndex << 12);
  return PciePortReg;
}

/**
 * Convert a PCIE port address to other NBIO or wrapper's PCIE port address
 *
 * @param[in]     Address    A Pcie port register
 * @param[in]     GnbHandle  Pointer to the Silicon Descriptor for this NBIO
 * @param[in]     Engine     Point to PCIe engine
 * 
 * @return UINT32 Converted PCIE port address
 */

UINT32
ConvertPciePortAddress2 (
  IN  UINT32                Address,
  IN  GNB_HANDLE            *GnbHandle,
  IN  PCIe_ENGINE_CONFIG    *Engine
  )
{
  PCIe_WRAPPER_CONFIG       *PcieWrapper;
  UINT8                     MappingPortID;

  PcieWrapper = PcieConfigGetParentWrapper (Engine);
  MappingPortID = (Engine->Type.Port.PortId) % 8;
  
  return ConvertPciePortAddress (Address, GnbHandle, PcieWrapper, MappingPortID);
}