/* $NoKeywords:$ */

/**
 * @file
 *
 * PCIe Clock Power Managment
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
#include  <GnbHsio.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/PcieMiscCommLib.h>

#define FILECODE  LIBRARY_PCIEMISCCOMMLIB_PCIECLKPMLIB_FILECODE

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

/*----------------------------------------------------------------------------------------*/

/**
 * Enable Clock Power Managment on function of the device
 *
 *
 *
 * @param[in] Function     PCI address of function.
 * @param[in] StdHeader    Standard configuration header
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
STATIC
PcieClkPmEnableOnFunction (
  IN       PCI_ADDR             Function,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8  PcieCapPtr;

  PcieCapPtr = GnbLibFindPciCapability (Function.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRMW (
      Function.AddressValue | (PcieCapPtr + PCIE_LINK_CTRL_REGISTER),
      AccessS3SaveWidth32,
      ~(UINT32)(BIT8),
      BIT8,
      StdHeader
      );
  }
}

/**----------------------------------------------------------------------------------------*/

/**
 * check capability of entire device including its functions
 *
 *
 *
 * @param[in] Device      PCI address of downstream port
 * @param[in] StdHeader   Standard configuration header
 *
 * @retval    TRUE - Device support Clock Power Managment
 */
/*----------------------------------------------------------------------------------------*/
BOOLEAN
STATIC
PcieClkPmCheckDeviceCapability (
  IN       PCI_ADDR                 Device,
  IN       HYGON_CONFIG_PARAMS        *StdHeader
  )
{
  UINT8   MaxFunc;
  UINT8   CurrentFunc;
  UINT8   PcieCapPtr;
  UINT32  Value;

  MaxFunc = GnbLibPciIsMultiFunctionDevice (Device.AddressValue, StdHeader) ? 7 : 0;

  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    if (GnbLibPciIsDevicePresent (Device.AddressValue, StdHeader)) {
      PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, StdHeader);
      if (PcieCapPtr == 0) {
        return FALSE;
      }

      GnbLibPciRead (
        Device.AddressValue | (PcieCapPtr + PCIE_LINK_CAP_REGISTER),
        AccessWidth32,
        &Value,
        StdHeader
        );
      if ((Value & BIT18) == 0) {
        return FALSE;
      }
    }
  }

  return TRUE;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Set Clock power managment on device
 *
 *
 *
 * @param[in] Device        PCI address of device.
 * @param[in] StdHeader     Standard configuration header
 *
 */
/*----------------------------------------------------------------------------------------*/
VOID
STATIC
PcieClkPmEnableOnDevice (
  IN       PCI_ADDR             Device,
  IN       HYGON_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8        MaxFunc;
  UINT8        CurrentFunc;
  HGPI_STATUS  HgpiStatus;

  HgpiStatus = HGPI_UNSUPPORTED;
  if (PcieClkPmCheckDeviceCapability (Device, StdHeader)) {
    MaxFunc = GnbLibPciIsMultiFunctionDevice (Device.AddressValue, StdHeader) ? 7 : 0;
    for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
      Device.Address.Function = CurrentFunc;
      if (GnbLibPciIsDevicePresent (Device.AddressValue, StdHeader)) {
        // HgpiStatus = HgpiGnbOemCallout (StdHeader, HGPI_GNB_PCIE_CLK_REQ, &Device);
        if (HgpiStatus == HGPI_SUCCESS) {
          IDS_HDT_CONSOLE (
            GNB_TRACE,
            "  Enable Clock Power Managment for Device = %d:%d:%d\n",
            Device.Address.Bus,
            Device.Address.Device,
            Device.Address.Function
            );
          PcieClkPmEnableOnFunction (Device, StdHeader);
        }
      }
    }
  }
}

/*----------------------------------------------------------------------------------------*/

/**
 * Evaluate device
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */
SCAN_STATUS
STATIC
PcieClkPmCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS       ScanStatus;
  PCIE_DEVICE_TYPE  DeviceType;

  ScanStatus = SCAN_SUCCESS;
  IDS_HDT_CONSOLE (
    GNB_TRACE,
    "  PcieClkPmCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  ScanStatus = SCAN_SUCCESS;
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
    case PcieDeviceRootComplex:
    case PcieDeviceDownstreamPort:
      GnbLibPciRMW (Device.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffffull, 0x0, ScanData->StdHeader);
      GnbLibPciScanSecondaryBus (Device, ScanData);
      break;
    case PcieDeviceUpstreamPort:
      PcieClkPmEnableOnDevice (Device, ScanData->StdHeader);
      GnbLibPciRMW (Device.AddressValue | 0x18, AccessS3SaveWidth32, 0xffffffffull, 0x0, ScanData->StdHeader);
      GnbLibPciScanSecondaryBus (Device, ScanData);
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    case PcieDeviceEndPoint:
    case PcieDeviceLegacyEndPoint:
      PcieClkPmEnableOnDevice (Device, ScanData->StdHeader);
      ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
      break;
    default:
      break;
  }

  return ScanStatus;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Confiugure Clock Power Managment
 *
 *
 *
 *
 * @param[in]       DownstreamPort       Downstream port PCI address
 * @param[in]       StdHeader            Standard configuration header
 *
 */
VOID
STATIC
PcieClkPmPortInitConfigure (
  IN       PCI_ADDR               DownstreamPort,
  IN       HYGON_CONFIG_PARAMS      *StdHeader
  )
{
  GNB_PCI_SCAN_DATA  ScanData;

  ScanData.StdHeader = StdHeader;
  ScanData.GnbScanCallback = PcieClkPmCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &ScanData);
}

/*----------------------------------------------------------------------------------------*/

/**
 * Callback to init various features on all active ports
 *
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 *
 */
VOID
STATIC
PcieClkPmPortInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  if (Engine->Type.Port.PortData.MiscControls.ClkPmSupport == 0x1 &&
      !PcieConfigIsSbPcieEngine (Engine) &&
      PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    PcieClkPmPortInitConfigure (
      Engine->Type.Port.Address,
      NULL
      );
  }
}

/**----------------------------------------------------------------------------------------*/

/**
 * Interface to enable Clock Power Managment
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
/*----------------------------------------------------------------------------------------*/

VOID
PcieClkPmInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkPmInterface Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        PcieClkPmPortInitCallback (PcieEngine, NULL);
      }

      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }

    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieClkPmInterface Exit\n");
}
