/* $NoKeywords:$ */
/**
 * @file
 *
 * Configure SRIS for endpoints
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 312288 $   @e \$Date: 2015-02-04 00:39:01 -0600 (Wed, 04 Feb 2015) $
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include  <GnbDxio.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbPciAccLib.h>
#include  <Library/GnbCommonLib.h>
#include  <GnbRegistersZP.h>
#include  <Library/NbioRegisterAccLib.h>

#define FILECODE NBIO_PCIE_AMDNBIOPCIEZPDXE_AMDNBIOPCIESRISZP_FILECODE
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  UINT32                  EpAddress;
  UINT32                  EpLinkCap2;
} PCIE_SRIS_CAPABILITY_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Evaluate device Sris Capability
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */

SCAN_STATUS
STATIC
PcieGetSrisCapabilityCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS                 ScanStatus;
  PCIE_SRIS_CAPABILITY_DATA   *PcieSrisCapabilityData;
  PCIE_DEVICE_TYPE            DeviceType;

  ScanStatus = SCAN_SUCCESS;
  PcieSrisCapabilityData = (PCIE_SRIS_CAPABILITY_DATA*) ScanData;
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieGetSrisCapabilityCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceEndPoint:
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    GnbLibPciRead (
      Device.AddressValue | 0x84,
      AccessWidth32,
      &PcieSrisCapabilityData->EpLinkCap2,
      ScanData->StdHeader
      );
    PcieSrisCapabilityData->EpAddress = Device.AddressValue;
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  case  PcieDeviceLegacyEndPoint:
    break;
  default:
    break;
  }
  return ScanStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init various features on all active ports
 *
 *
 *
 *
 * @param[in]       GnbHandle      Pointer to the Silicon Descriptor for this node
 * @param[in]       Engine         Pointer to engine config descriptor
 * @param[in, out]  Buffer         Not used
 *
 */

VOID
PcieSrisInitCallback (
  IN       GNB_HANDLE            *GnbHandle,
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIE_SRIS_CAPABILITY_DATA  PcieSrisCapabilityData;
  UINT32                     Address0;
  UINT32                     Address1;
  UINT32                     Value;
  UINT32                     RcLinkCap2;

  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSrisInitCallback for Device = %d:%d:%d\n",
    Engine->Type.Port.Address.Address.Bus,
    Engine->Type.Port.Address.Address.Device,
    Engine->Type.Port.Address.Address.Function
    );
  if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    Address0 = 0x11100274;
    Address1 = 0x111402EC;
    if ((Engine->Type.Port.PortData.DeviceNumber == 1) && (Engine->Type.Port.PortData.FunctionNumber == 1)) {
      Address0 = 0x11100274;
      Address1 = 0x111402EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 1) && (Engine->Type.Port.PortData.FunctionNumber == 2)) {
      Address0 = 0x11101274;
      Address1 = 0x111412EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 1) && (Engine->Type.Port.PortData.FunctionNumber == 3)) {
      Address0 = 0x11102274;
      Address1 = 0x111422EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 1) && (Engine->Type.Port.PortData.FunctionNumber == 4)) {
      Address0 = 0x11103274;
      Address1 = 0x111432EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 1) && (Engine->Type.Port.PortData.FunctionNumber == 5)) {
      Address0 = 0x11104274;
      Address1 = 0x111442EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 1) && (Engine->Type.Port.PortData.FunctionNumber == 6)) {
      Address0 = 0x11105274;
      Address1 = 0x111452EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 1) && (Engine->Type.Port.PortData.FunctionNumber == 7)) {
      Address0 = 0x11106274;
      Address1 = 0x111462EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 2) && (Engine->Type.Port.PortData.FunctionNumber == 1)) {
      Address0 = 0x11107274;
      Address1 = 0x111472EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 3) && (Engine->Type.Port.PortData.FunctionNumber == 1)) {
      Address0 = 0x11200274;
      Address1 = 0x112402EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 3) && (Engine->Type.Port.PortData.FunctionNumber == 2)) {
      Address0 = 0x11201274;
      Address1 = 0x112412EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 3) && (Engine->Type.Port.PortData.FunctionNumber == 3)) {
      Address0 = 0x11202274;
      Address1 = 0x112422EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 3) && (Engine->Type.Port.PortData.FunctionNumber == 4)) {
      Address0 = 0x11203274;
      Address1 = 0x112432EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 3) && (Engine->Type.Port.PortData.FunctionNumber == 5)) {
      Address0 = 0x11204274;
      Address1 = 0x112442EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 3) && (Engine->Type.Port.PortData.FunctionNumber == 6)) {
      Address0 = 0x11205274;
      Address1 = 0x112452EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 3) && (Engine->Type.Port.PortData.FunctionNumber == 7)) {
      Address0 = 0x11206274;
      Address1 = 0x112462EC;
    }
    if ((Engine->Type.Port.PortData.DeviceNumber == 4) && (Engine->Type.Port.PortData.FunctionNumber == 1)) {
      Address0 = 0x11207274;
      Address1 = 0x112472EC;
    }
    NbioRegisterRead ( GnbHandle, TYPE_SMN, Address1, &Value, 0);
    if (Engine->Type.Port.SRIS_SRNS == 1) {
      Value |= BIT8;
      Engine->Type.Port.PortData.LinkAspm = AspmDisabled;
    } else if (Engine->Type.Port.SRIS_SRNS == 1) {
      Value |= BIT13;
      Engine->Type.Port.PortData.LinkAspm = AspmDisabled;
    }
    NbioRegisterWrite (GnbHandle, TYPE_SMN, Address1, &Value, GNB_REG_ACC_FLAG_S3SAVE);

    if (Engine->Type.Port.SRIS_LowerSKPSupport == 1) {
      PcieSrisCapabilityData.EpAddress = 0;
      PcieSrisCapabilityData.EpLinkCap2 = 0;
      PcieSrisCapabilityData.ScanData.StdHeader = NULL;
      PcieSrisCapabilityData.ScanData.GnbScanCallback = PcieGetSrisCapabilityCallback;
      GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieSrisCapabilityData.ScanData);
      GnbLibPciRead (
        Engine->Type.Port.Address.AddressValue | 0x84,
        AccessWidth32,
        &RcLinkCap2,
        NULL
        );
      if (((RcLinkCap2 & 0x70000) && (PcieSrisCapabilityData.EpLinkCap2 & 0xE00)) || ((RcLinkCap2 & 0xE00) && (PcieSrisCapabilityData.EpLinkCap2 & 0x70000))) {
        NbioRegisterRead ( GnbHandle, TYPE_SMN, Address0, &Value, 0);
        Value |= BIT9;
        NbioRegisterWrite (GnbHandle, TYPE_SMN, Address0, &Value, GNB_REG_ACC_FLAG_S3SAVE);
      }
    }
  }
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to configure SRIS on PCIE interface
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 * @retval    AGESA_STATUS
 */
 /*----------------------------------------------------------------------------------------*/

VOID
PcieSrisInit (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSrisInit Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if ((PcieLibIsEngineAllocated (PcieEngine)) && (PcieEngine->Type.Port.SRIS_SRNS == 1)) {
        PcieSrisInitCallback (GnbHandle, PcieEngine, NULL );
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieSrisInit Exit\n");
}
