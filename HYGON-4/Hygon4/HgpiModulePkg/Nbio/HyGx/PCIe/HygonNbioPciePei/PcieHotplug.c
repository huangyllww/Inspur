/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe port remapping functions.
 *
 *
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
#include <HYGON.h>
#include <Gnb.h>
#include <GnbHsio.h>
#include <PiPei.h>
#include <Filecode.h>
#include <GnbRegisters.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/TimerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcieConfigLib.h>
#include <Library/PcdLib.h>

#include <HygonPcieComplex.h>

#define FILECODE  NBIO_PCIE_HYGX_HYGONNBIOPCIESTPEI_PCIEHOTPLUGST_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                     L O C A L   D A T A   D E F I N I T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/**
 * Callback to init hotplug features on all hotplug ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 * @param[in]       Pcie            Pointer to global PCIe configuration
 *
 */
VOID
STATIC
PcieHotplugInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer,
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  PCIe_WRAPPER_CONFIG  *Wrapper;
  GNB_HANDLE           *GnbHandle;
  UINT32               SmnAddress;
  UINT32               Value;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieHotplugInitCallback Enter\n");
  if (Engine->Type.Port.PortData.LinkHotplug != HotplugDisabled ) {
    IDS_HDT_CONSOLE (GNB_TRACE, "Found Hotplug Engine at:\n");
    IDS_HDT_CONSOLE (GNB_TRACE, "  Port.PortId = %d\n", Engine->Type.Port.PortId);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Port.PcieBridgeId = %d\n", Engine->Type.Port.PcieBridgeId);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Port.LogicalBridgeId = %d\n", Engine->Type.Port.LogicalBridgeId);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Port.Address = %x\n", Engine->Type.Port.Address.AddressValue);
    IDS_HDT_CONSOLE (
      GNB_TRACE,
      "  Device = %x, Function = %x, LinkHotplug=%x\n",
      Engine->Type.Port.PortData.DeviceNumber,
      Engine->Type.Port.PortData.FunctionNumber,
      Engine->Type.Port.PortData.LinkHotplug
      );
    IDS_HDT_CONSOLE (GNB_TRACE, "  Type = ");

    Wrapper   = PcieConfigGetParentWrapper (Engine);
    GnbHandle = (GNB_HANDLE *)PcieConfigGetParentSilicon (Engine);

    // devCfg:NB_PCIE_SLOT_CAP.HOTPLUG_CAPABLE = 1h;
    NbioRegisterRMW (
      GnbHandle,
      TYPE_PCI,
      MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CAP_PCI_OFFSET),
      (UINT32) ~(SLOT_CAP_HOTPLUG_CAPABLE_MASK),
      1 << SLOT_CAP_HOTPLUG_CAPABLE_OFFSET,
      0
      );

    if (Wrapper->WrapId == 0) {
      SmnAddress = NBIO_SPACE (GnbHandle, L1_MISC_CNTRL_1_PCIE0_ADDRESS_HYGX);
    } else if (Wrapper->WrapId == 1) {
      SmnAddress = NBIO_SPACE (GnbHandle, L1_MISC_CNTRL_1_PCIE1_ADDRESS_HYGX);
    } else {
      SmnAddress = NBIO_SPACE (GnbHandle, L1_MISC_CNTRL_1_IOAGR_ADDRESS_HYGX);
    }

    NbioRegisterRMW (
      GnbHandle,
      TYPE_SMN,
      SmnAddress,
      (UINT32) ~(L1_MISC_CNTRL_1_REG_force_OrderStreamID_func_MASK),
      1 << L1_MISC_CNTRL_1_REG_force_OrderStreamID_func_OFFSET,
      0
      );

    SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_TX_CNTL_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
    NbioRegisterRMW (
      GnbHandle,
      PCIE_TX_CNTL_TYPE,
      SmnAddress,
      (UINT32) ~(PCIE_TX_CNTL_TX_FLUSH_TLP_DIS_MASK),
      0 << PCIE_TX_CNTL_TX_FLUSH_TLP_DIS_OFFSET,
      0
      );

    //
    // Type specific hotplug configuration
    //
    switch (Engine->Type.Port.PortData.LinkHotplug) {
      //
      // Basic Hotplug Configuration
      //
      case HsioHotplugBasic:
        IDS_HDT_CONSOLE (GNB_TRACE, "HsioHotplugBasic\n");
        NbioRegisterRMW (
          GnbHandle,
          TYPE_PCI,
          MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
          (UINT32) ~(SLOT_CNTL_HOTPLUG_INTR_EN_MASK),
          1 << SLOT_CNTL_HOTPLUG_INTR_EN_OFFSET,
          0
          );

        // devCfg:PCIEIND_P: PCIEP PCIEP_PORT_CNTL.NATIVE_PME_EN = !legacyHotPlug;
        SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_PORT_CNTL_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
        NbioRegisterRMW (
          GnbHandle,
          PCIE_TX_CNTL_TYPE,
          SmnAddress,
          (UINT32) ~(PCIEP_PORT_CNTL_NATIVE_PME_EN_MASK),
          0 << PCIEP_PORT_CNTL_NATIVE_PME_EN_OFFSET,
          0
          );
        break;

      //
      // Enhanced Hotplug Configuration
      //
      case HsioHotplugEnhanced:
        IDS_HDT_CONSOLE (GNB_TRACE, "HsioHotplugEnhanced\n");

        NbioRegisterRMW (
          GnbHandle,
          TYPE_PCI,
          MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
          (UINT32) ~(SLOT_CNTL_HOTPLUG_INTR_EN_MASK),
          1 << SLOT_CNTL_HOTPLUG_INTR_EN_OFFSET,
          0
          );

        // devCfg:PCIEIND_P: PCIEP PCIEP_PORT_CNTL.NATIVE_PME_EN = !legacyHotPlug;
        SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_PORT_CNTL_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
        NbioRegisterRMW (
          GnbHandle,
          PCIE_TX_CNTL_TYPE,
          SmnAddress,
          (UINT32) ~(PCIEP_PORT_CNTL_NATIVE_PME_EN_MASK),
          1 << PCIEP_PORT_CNTL_NATIVE_PME_EN_OFFSET,
          0
          );

        SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_LC_CNTL3_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
        NbioRegisterRMW (
          GnbHandle,
          PCIE_LC_CNTL3_TYPE,
          SmnAddress,
          (UINT32) ~(PCIE_LC_CNTL3_LC_ENHANCED_HOT_PLUG_EN_MASK |
                     PCIE_LC_CNTL3_LC_RCVR_DET_EN_OVERRIDE_MASK |
                     PCIE_LC_CNTL3_LC_EHP_RX_PHY_CMD_MASK |
                     PCIE_LC_CNTL3_LC_EHP_TX_PHY_CMD_MASK),
          ((1 << PCIE_LC_CNTL3_LC_ENHANCED_HOT_PLUG_EN_OFFSET) |
           (0 << PCIE_LC_CNTL3_LC_RCVR_DET_EN_OVERRIDE_OFFSET) |
           (3 << PCIE_LC_CNTL3_LC_EHP_RX_PHY_CMD_OFFSET) |
           (3 << PCIE_LC_CNTL3_LC_EHP_TX_PHY_CMD_OFFSET)),
          0
          );

        NbioRegisterRead (
          GnbHandle,
          TYPE_PCI,
          MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
          &Value,
          0
          );
        IDS_HDT_CONSOLE (GNB_TRACE, "Value = 0x%x\n", Value);

        SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_LC_CNTL3_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
        NbioRegisterRead (
          GnbHandle,
          PCIE_LC_CNTL3_TYPE,
          SmnAddress,
          &Value,
          0
          );
        IDS_HDT_CONSOLE (GNB_TRACE, "Value = 0x%x\n", Value);

        SmnAddress = ConvertPcieCoreAddress (PCIE0_CNTL_ADDRESS_HYGX, GnbHandle, Wrapper);
        NbioRegisterRMW (
          GnbHandle,
          PCIE_CNTL_TYPE,
          SmnAddress,
          (UINT32) ~(PCIE_CNTL_LC_HOT_PLUG_DELAY_SEL_MASK),
          0x5 << PCIE_CNTL_LC_HOT_PLUG_DELAY_SEL_OFFSET,
          0
          );

        SmnAddress = ConvertPcieCoreAddress (CPM_CONTROL_PCIECORE0_ADDRESS_HYGX, GnbHandle, Wrapper);
        NbioRegisterRMW (
          GnbHandle,
          CPM_CONTROL_TYPE,
          SmnAddress,
          (UINT32) ~(CPM_CONTROL_RCVR_DET_CLK_ENABLE_MASK),
          0x1 << CPM_CONTROL_RCVR_DET_CLK_ENABLE_OFFSET,
          0
          );

        SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_LINK_CTL_STS_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
        NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Value, 0);

        SmnAddress = GetIohcBridgeCntlAddress (GnbHandle, Engine);
        if ((Value & (UINT32)(1 << LINK_CTL_STS_DL_ACTIVE_OFFSET)) == 0) {
          NbioRegisterRMW (
            GnbHandle,
            TYPE_SMN,
            SmnAddress,
            (UINT32) ~(IOHC_BRIDGE_CNTL_BridgeDis_MASK),
            (1 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET),
            0
            );
          IDS_HDT_CONSOLE (GNB_TRACE, "No ep - BridgeDis: %08x\n", SmnAddress);
        } else {
          NbioRegisterRMW (
            GnbHandle,
            TYPE_SMN,
            SmnAddress,
            (UINT32) ~(IOHC_BRIDGE_CNTL_BridgeDis_MASK),
            (0 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET),
            0
            );
          IDS_HDT_CONSOLE (GNB_TRACE, "Found ep - BridgeDis: %08x\n", SmnAddress);
        }

        break;
      //
      // Inboard Hotplug Configuration
      // Inboard Hotplug is similar to Enhanced Hotplug, except that the device must be present at boot
      //
      case HsioHotplugInboard:
        IDS_HDT_CONSOLE (GNB_TRACE, "HsioHotplugInboard\n");
        if (Engine->InitStatus == INIT_STATUS_PCIE_TRAINING_SUCCESS) {
          NbioRegisterRMW (
            GnbHandle,
            TYPE_PCI,
            MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
            (UINT32) ~(SLOT_CNTL_HOTPLUG_INTR_EN_MASK),
            1 << SLOT_CNTL_HOTPLUG_INTR_EN_OFFSET,
            0
            );

          // devCfg:PCIEIND_P: PCIEP PCIEP_PORT_CNTL.NATIVE_PME_EN = !legacyHotPlug;
          SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_PORT_CNTL_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
          NbioRegisterRMW (
            GnbHandle,
            PCIE_TX_CNTL_TYPE,
            SmnAddress,
            (UINT32) ~(PCIEP_PORT_CNTL_NATIVE_PME_EN_MASK),
            1 << PCIEP_PORT_CNTL_NATIVE_PME_EN_OFFSET,
            0
            );

          SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_LC_CNTL3_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
          NbioRegisterRMW (
            GnbHandle,
            PCIE_LC_CNTL3_TYPE,
            SmnAddress,
            (UINT32) ~(PCIE_LC_CNTL3_LC_ENHANCED_HOT_PLUG_EN_MASK |
                       PCIE_LC_CNTL3_LC_RCVR_DET_EN_OVERRIDE_MASK |
                       PCIE_LC_CNTL3_LC_EHP_RX_PHY_CMD_MASK |
                       PCIE_LC_CNTL3_LC_EHP_TX_PHY_CMD_MASK),
            ((1 << PCIE_LC_CNTL3_LC_ENHANCED_HOT_PLUG_EN_OFFSET) |
             (0 << PCIE_LC_CNTL3_LC_RCVR_DET_EN_OVERRIDE_OFFSET) |
             (3 << PCIE_LC_CNTL3_LC_EHP_RX_PHY_CMD_OFFSET) |
             (3 << PCIE_LC_CNTL3_LC_EHP_TX_PHY_CMD_OFFSET)),
            0
            );

          NbioRegisterRead (
            GnbHandle,
            TYPE_PCI,
            MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
            &Value,
            0
            );
          IDS_HDT_CONSOLE (GNB_TRACE, "Value = 0x%x\n", Value);

          SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_LC_CNTL3_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
          NbioRegisterRead (
            GnbHandle,
            PCIE_LC_CNTL3_TYPE,
            SmnAddress,
            &Value,
            0
            );
          IDS_HDT_CONSOLE (GNB_TRACE, "Value = 0x%x\n", Value);

          SmnAddress = ConvertPcieCoreAddress (PCIE0_CNTL_ADDRESS_HYGX, GnbHandle, Wrapper);
          NbioRegisterRMW (
            GnbHandle,
            PCIE_CNTL_TYPE,
            SmnAddress,
            (UINT32) ~(PCIE_CNTL_LC_HOT_PLUG_DELAY_SEL_MASK),
            0x5 << PCIE_CNTL_LC_HOT_PLUG_DELAY_SEL_OFFSET,
            0
            );

          SmnAddress = ConvertPcieCoreAddress (CPM_CONTROL_PCIECORE0_ADDRESS_HYGX, GnbHandle, Wrapper);
          NbioRegisterRMW (
            GnbHandle,
            CPM_CONTROL_TYPE,
            SmnAddress,
            (UINT32) ~(CPM_CONTROL_RCVR_DET_CLK_ENABLE_MASK),
            0x1 << CPM_CONTROL_RCVR_DET_CLK_ENABLE_OFFSET,
            0
            );
        }

        break;
      case HsioHotplugServerExpress:
        IDS_HDT_CONSOLE (GNB_TRACE, "HsioHotplugServerExpress\n");
        // Express Module
        NbioRegisterRMW (
          GnbHandle,
          TYPE_PCI,
          MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CAP_PCI_OFFSET),
          (UINT32) ~(SLOT_CAP_ATTN_BUTTON_PRESENT_MASK |
                     SLOT_CAP_PWR_CONTROLLER_PRESENT_MASK |
                     SLOT_CAP_MRL_SENSOR_PRESENT_MASK |
                     SLOT_CAP_ATTN_INDICATOR_PRESENT_MASK |
                     SLOT_CAP_PWR_INDICATOR_PRESENT_MASK |
                     SLOT_CAP_HOTPLUG_SURPRISE_MASK |
                     SLOT_CAP_ELECTROMECH_INTERLOCK_PRESENT_MASK |
                     SLOT_CAP_NO_COMMAND_COMPLETED_SUPPORTED_MASK),
          ((1 << SLOT_CAP_ATTN_BUTTON_PRESENT_OFFSET) |
           (1 << SLOT_CAP_PWR_CONTROLLER_PRESENT_OFFSET) |
           (0 << SLOT_CAP_MRL_SENSOR_PRESENT_OFFSET) |
           (1 << SLOT_CAP_ATTN_INDICATOR_PRESENT_OFFSET) |
           (1 << SLOT_CAP_PWR_INDICATOR_PRESENT_OFFSET) |
           (1 << SLOT_CAP_HOTPLUG_SURPRISE_OFFSET) |
           (1 << SLOT_CAP_ELECTROMECH_INTERLOCK_PRESENT_OFFSET) |
           (1 << SLOT_CAP_NO_COMMAND_COMPLETED_SUPPORTED_OFFSET)),
          0
          );
        NbioRegisterRMW (
          GnbHandle,
          TYPE_PCI,
          MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
          (UINT32) ~(SLOT_CNTL_ATTN_BUTTON_PRESSED_MASK |
                     SLOT_CNTL_PWR_FAULT_DETECTED_MASK |
                     SLOT_CNTL_MRL_SENSOR_CHANGED_MASK |
                     SLOT_CNTL_PRESENCE_DETECT_CHANGED_MASK |
                     SLOT_CNTL_COMMAND_COMPLETED_MASK |
                     SLOT_CNTL_DL_STATE_CHANGED_MASK),
          ((1 << SLOT_CNTL_ATTN_BUTTON_PRESSED_OFFSET) |
           (1 << SLOT_CNTL_PWR_FAULT_DETECTED_OFFSET) |
           (1 << SLOT_CNTL_MRL_SENSOR_CHANGED_OFFSET) |
           (1 << SLOT_CNTL_PRESENCE_DETECT_CHANGED_OFFSET) |
           (1 << SLOT_CNTL_COMMAND_COMPLETED_OFFSET) |
           (1 << SLOT_CNTL_DL_STATE_CHANGED_OFFSET)),
          0
          );

        SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_PORT_CNTL_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
        NbioRegisterRMW (
          GnbHandle,
          PCIE_TX_CNTL_TYPE,
          SmnAddress,
          (UINT32) ~(PCIEP_PORT_CNTL_PWR_FAULT_EN_MASK),
          1 << PCIEP_PORT_CNTL_PWR_FAULT_EN_OFFSET,
          0
          );

        SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_LINK_CTL_STS_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
        NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Value, 0);
        SmnAddress = GetIohcBridgeCntlAddress (GnbHandle, Engine);
        if ((Value & (UINT32)(1 << LINK_CTL_STS_DL_ACTIVE_OFFSET)) == 0) {
          NbioRegisterRMW (
            GnbHandle,
            TYPE_SMN,
            SmnAddress,
            (UINT32) ~(IOHC_BRIDGE_CNTL_BridgeDis_MASK),
            (1 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET),
            0
            );
          IDS_HDT_CONSOLE (GNB_TRACE, "No ep - BridgeDis: %08x\n", SmnAddress);
        } else {
          NbioRegisterRMW (
            GnbHandle,
            TYPE_SMN,
            SmnAddress,
            (UINT32) ~(IOHC_BRIDGE_CNTL_BridgeDis_MASK),
            (0 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET),
            0
            );
          IDS_HDT_CONSOLE (GNB_TRACE, "Found ep - BridgeDis: %08x\n", SmnAddress);
        }

        break;

      case HsioHotplugServerEntSSD:
        IDS_HDT_CONSOLE (GNB_TRACE, "HsioHotplugServerEntSSD\n");
        // Enterprise SSD
        NbioRegisterRMW (
          GnbHandle,
          TYPE_PCI,
          MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CAP_PCI_OFFSET),
          (UINT32) ~(SLOT_CAP_ATTN_BUTTON_PRESENT_MASK |
                     SLOT_CAP_PWR_CONTROLLER_PRESENT_MASK |
                     SLOT_CAP_MRL_SENSOR_PRESENT_MASK |
                     SLOT_CAP_ATTN_INDICATOR_PRESENT_MASK |
                     SLOT_CAP_PWR_INDICATOR_PRESENT_MASK |
                     SLOT_CAP_HOTPLUG_SURPRISE_MASK |
                     SLOT_CAP_ELECTROMECH_INTERLOCK_PRESENT_MASK |
                     SLOT_CAP_NO_COMMAND_COMPLETED_SUPPORTED_MASK),
          ((0 << SLOT_CAP_ATTN_BUTTON_PRESENT_OFFSET) |
           (0 << SLOT_CAP_PWR_CONTROLLER_PRESENT_OFFSET) |
           (0 << SLOT_CAP_MRL_SENSOR_PRESENT_OFFSET) |
           (0 << SLOT_CAP_ATTN_INDICATOR_PRESENT_OFFSET) |
           (0 << SLOT_CAP_PWR_INDICATOR_PRESENT_OFFSET) |
           (1 << SLOT_CAP_HOTPLUG_SURPRISE_OFFSET) |
           (0 << SLOT_CAP_ELECTROMECH_INTERLOCK_PRESENT_OFFSET) |
           (1 << SLOT_CAP_NO_COMMAND_COMPLETED_SUPPORTED_OFFSET)),
          0
          );
        NbioRegisterRMW (
          GnbHandle,
          TYPE_PCI,
          MAKE_SBDFO (0, 0, Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber, SLOT_CNTL_PCI_OFFSET),
          (UINT32) ~(SLOT_CNTL_ATTN_BUTTON_PRESSED_MASK |
                     SLOT_CNTL_PWR_FAULT_DETECTED_MASK |
                     SLOT_CNTL_MRL_SENSOR_CHANGED_MASK |
                     SLOT_CNTL_PRESENCE_DETECT_CHANGED_MASK |
                     SLOT_CNTL_COMMAND_COMPLETED_MASK |
                     SLOT_CNTL_DL_STATE_CHANGED_MASK),
          ((1 << SLOT_CNTL_ATTN_BUTTON_PRESSED_OFFSET) |
           (1 << SLOT_CNTL_PWR_FAULT_DETECTED_OFFSET) |
           (1 << SLOT_CNTL_MRL_SENSOR_CHANGED_OFFSET) |
           (1 << SLOT_CNTL_PRESENCE_DETECT_CHANGED_OFFSET) |
           (1 << SLOT_CNTL_COMMAND_COMPLETED_OFFSET) |
           (1 << SLOT_CNTL_DL_STATE_CHANGED_OFFSET)),
          0
          );

        SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_PORT_CNTL_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
        NbioRegisterRMW (
          GnbHandle,
          PCIE_TX_CNTL_TYPE,
          SmnAddress,
          (UINT32) ~(PCIEP_PORT_CNTL_PWR_FAULT_EN_MASK),
          1 << PCIEP_PORT_CNTL_PWR_FAULT_EN_OFFSET,
          0
          );

        SmnAddress = ConvertPciePortAddress (PCIE0_GPP0_LINK_CTL_STS_ADDRESS_HYGX, GnbHandle, Wrapper, (Engine->Type.Port.PortId % 8));
        NbioRegisterRead (GnbHandle, TYPE_SMN, SmnAddress, &Value, 0);

        SmnAddress = GetIohcBridgeCntlAddress (GnbHandle, Engine);
        if ((Value & (UINT32)(1 << LINK_CTL_STS_DL_ACTIVE_OFFSET)) == 0) {
          NbioRegisterRMW (
            GnbHandle,
            TYPE_SMN,
            SmnAddress,
            (UINT32) ~(IOHC_BRIDGE_CNTL_BridgeDis_MASK),
            (1 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET),
            0
            );
          IDS_HDT_CONSOLE (GNB_TRACE, "No ep - BridgeDis: %08x\n", SmnAddress);
        } else {
          NbioRegisterRMW (
            GnbHandle,
            TYPE_SMN,
            SmnAddress,
            (UINT32) ~(IOHC_BRIDGE_CNTL_BridgeDis_MASK),
            (0 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET),
            0
            );
          IDS_HDT_CONSOLE (GNB_TRACE, "Found ep - BridgeDis: %08x\n", SmnAddress);
        }

        break;

      default:
        IDS_HDT_CONSOLE (GNB_TRACE, "Invalid Hotplug Type\n");
        ASSERT (FALSE);
        break;
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieHotplugInitCallback Exit\n");
}

/*----------------------------------------------------------------------------------------*/

/**
 * Map engine to specific PCI device address
 *
 *
 * @param[in]  GnbHandle           Pointer to the Silicon Descriptor for this node
 * @param[in]  PortDevMap          Pointer to PortDevMap
 */
VOID
PcieConfigureHotplugPortsST (
  IN       PCIe_PLATFORM_CONFIG  *Pcie
  )
{
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieConfigureHotplugPortsST Enter\n");
  PcieConfigRunProcForAllEngines (
    DESCRIPTOR_ALLOCATED | DESCRIPTOR_PCIE_ENGINE,
    PcieHotplugInitCallback,
    NULL,
    Pcie
    );
}
