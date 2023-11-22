/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe ports visibility control.
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
#include <Filecode.h>
#include <GnbRegisters.h>
#include <GnbHsio.h>
#include <Library/HygonBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/NbioHandleLib.h>
#include <Library/NbioRegisterAccLib.h>
#include <Library/TimerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcieConfigLib.h>

#define FILECODE NBIO_PCIE_HYEX_HYGONNBIOPCIESTPEI_HIDEPORTSST_FILECODE

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

VOID
STATIC
PcieControlPortsHyEx (
  IN      PCIE_PORT_VISIBILITY    Control,
  IN      GNB_HANDLE              *GnbHandle
  );

VOID
STATIC
PcieEnablePortsHyEx (
  IN      GNB_HANDLE              *GnbHandle
  );

/*----------------------------------------------------------------------------------------*/

/**
 * Control port visibility in PCI config space
 *
 *
 * @param[in]  Control         Control Hide/Unhide ports
 * @param[in]  GnbHandle       Pointer to GNB_HANDLE
 */
VOID
PciePortsVisibilityControlHyEx (
  IN      PCIE_PORT_VISIBILITY    Control,
  IN      GNB_HANDLE              *GnbHandle
  )
{
  switch (Control) {
  case UnhidePorts:
    IDS_HDT_CONSOLE (GNB_TRACE, "Unhide Ports\n");
    PcieControlPortsHyEx (UnhidePorts, GnbHandle);
    break;
  case HidePorts:
    IDS_HDT_CONSOLE (GNB_TRACE, "Hide Ports\n");
    PcieControlPortsHyEx (HidePorts, GnbHandle);
    PcieEnablePortsHyEx (GnbHandle);
    break;
  default:
    ASSERT (FALSE);
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Hide/Unhide all ports
 *
 *
 * @param[in]  Control             Control Hide/Unhide ports
 * @param[in]  GnbHandle           Pointer to the Silicon Descriptor for this node
 */
VOID
STATIC
PcieControlPortsHyEx (
  IN      PCIE_PORT_VISIBILITY    Control,
  IN      GNB_HANDLE              *GnbHandle
  )
{
  UINT32              Value;
  UINT32              Index;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieControlPortsHyEx Entry \n");
  Value = (Control == HidePorts) ? ((1 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET) | (1 << IOHC_BRIDGE_CNTL_CfgDis_OFFSET)) : 0;
  Value |= (1 << IOHC_BRIDGE_CNTL_CrsEnable_OFFSET);

  for (Index = 0; Index < 18; Index++) {    ///@todo magic number 16
   IDS_HDT_CONSOLE (GNB_TRACE, "  Hide Port %d \n", Index);
   NbioRegisterRMW (GnbHandle,
                    TYPE_SMN,
                    NBIO_SPACE(GnbHandle, NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYEX) + (Index << 10),
                    (UINT32)~(IOHC_BRIDGE_CNTL_BridgeDis_MASK | IOHC_BRIDGE_CNTL_CfgDis_MASK | IOHC_BRIDGE_CNTL_CrsEnable_MASK),
                    Value,
                    0   // Change to flags if used in DXE
                    );
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieControlPortsHyEx Exit\n");
  return;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Hide unused ports
 *
 *
 *
 * @param[in]  GnbHandle           Pointer to the Silicon Descriptor for this node
 */
VOID
STATIC
PcieEnablePortsHyEx (
  IN      GNB_HANDLE              *GnbHandle
  )
{
  PCIe_WRAPPER_CONFIG               *PcieWrapper;
  PCIe_ENGINE_CONFIG                *PcieEngine;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEnablePortsHyEx Entry \n");

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      IDS_HDT_CONSOLE (GNB_TRACE, "PcieEnablePortsHyEx Wrapper %d LogicalBridgeId %d, InitStatus %d, LinkHotplug %d \n", 
         PcieWrapper->WrapId, 
         PcieEngine->Type.Port.LogicalBridgeId,
         PcieEngine->InitStatus,
         PcieEngine->Type.Port.PortData.LinkHotplug
         );

      if ((PcieEngine->InitStatus == INIT_STATUS_PCIE_TRAINING_SUCCESS) ||
          ((PcieEngine->Type.Port.PortData.LinkHotplug != HotplugDisabled) &&
          (PcieEngine->Type.Port.PortData.LinkHotplug != HotplugInboard))) {
        IDS_HDT_CONSOLE (GNB_TRACE, "  Enable port %d device %d function %d \n", 
          PcieEngine->Type.Port.PortId, 
          PcieEngine->Type.Port.NativeDevNumber,
          PcieEngine->Type.Port.NativeFunNumber
          );
        
        NbioRegisterRMW (
          GnbHandle,
          TYPE_SMN,
          IOHC_BRIDGE_SPACE(GnbHandle, PcieEngine, NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYEX),
                    (UINT32)~(IOHC_BRIDGE_CNTL_BridgeDis_MASK | IOHC_BRIDGE_CNTL_CfgDis_MASK | IOHC_BRIDGE_CNTL_CrsEnable_MASK),
                    (1 << IOHC_BRIDGE_CNTL_CrsEnable_OFFSET),
                    0   // Change to flags if used in DXE
        );
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieEnablePortsHyEx Exit \n");
  return;
}

