/* $NoKeywords:$ */

/**
 * @file
 *
 * Configure Max Read Request Size
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
#include  <Library/HygonBaseLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/PcieMiscCommLib.h>
#include  <Library/PcdLib.h>
#include  <OptionGnb.h>

#define FILECODE  LIBRARY_PCIEMISCCOMMLIB_PCIEMAXREADREQLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  GNB_PCI_SCAN_DATA    ScanData;
  UINT8                MaxReadRequestSize;
} PCIE_MAX_READ_REQUEST_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/

/**
 * Configure the Max read request szie setting to all devices in the PCIe Segment
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */
SCAN_STATUS
PcieSetMaxReadRequestSizeCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_MAX_READ_REQUEST_DATA  *PcieMaxReadRequestData;
  PCIE_DEVICE_TYPE            DeviceType;
  UINT8                       PcieCapPtr;
  UINT32                      Value;

  PcieMaxReadRequestData = (PCIE_MAX_READ_REQUEST_DATA *)ScanData;
  IDS_HDT_CONSOLE (
    GNB_TRACE,
    "  PcieSetMaxReadRequestSizeCallback for Device = %d:%d:%d to %d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function,
    PcieMaxReadRequestData->MaxReadRequestSize
    );
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
      );
    Value = (Value & 0xFFFF8FFF) | (PcieMaxReadRequestData->MaxReadRequestSize << 12);
    GnbLibPciWrite (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
      AccessS3SaveWidth32,
      &Value,
      ScanData->StdHeader
      );
  }

  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
    case PcieDeviceRootComplex:
    case PcieDeviceDownstreamPort:
    case PcieDeviceUpstreamPort:
      GnbLibPciScanSecondaryBus (Device, &PcieMaxReadRequestData->ScanData);
      break;
    case PcieDeviceEndPoint:
    case PcieDeviceLegacyEndPoint:
      break;
    default:
      break;
  }

  return SCAN_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/

/**
 * Determine maximum Read Request size for PCIe segment
 *
 *  Scan through all link in segment to determine maximum read request size by EPs.
 *
 * @param[in]   DownstreamPort        PCI address of PCIe port
 * @param[in]   MaxReadRequestSize    MaxReadRequestSize supported by the engine
 * @param[in]   StdHeader             Standard configuration header
 *
 */
VOID
PcieSetMaxReadRequestSize (
  IN       PCI_ADDR                DownstreamPort,
  IN       UINT8                   MaxReadRequestSize,
  IN       HYGON_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_MAX_READ_REQUEST_DATA  PcieMaxReadRequestData;

  PcieMaxReadRequestData.MaxReadRequestSize = MaxReadRequestSize;
  PcieMaxReadRequestData.ScanData.StdHeader = StdHeader;
  PcieMaxReadRequestData.ScanData.GnbScanCallback = PcieSetMaxReadRequestSizeCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieMaxReadRequestData.ScanData);
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
PcieMaxReadRequestInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  UINT8  EngineMaxReadRequest;

  if ((PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS))) {
    EngineMaxReadRequest =  PcdGet8 (PcdCfgMaxReadRequestSize);
    if (EngineMaxReadRequest <= MAX_READREQUESTSIZE_4096) {
      IDS_HDT_CONSOLE (
        GNB_TRACE,
        "Device = %d:%d:%d MaxReadRequestSize = %x\n",
        Engine->Type.Port.Address.Address.Bus,
        Engine->Type.Port.Address.Address.Device,
        Engine->Type.Port.Address.Address.Function,
        EngineMaxReadRequest
        );
      PcieSetMaxReadRequestSize (Engine->Type.Port.Address, EngineMaxReadRequest, NULL);
    }
  }
}

/**----------------------------------------------------------------------------------------*/

/**
 * Interface to configure MaxReadRequestSize on PCIE interface
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
/*----------------------------------------------------------------------------------------*/

VOID
PcieMaxReadRequestInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMaxReadRequestInterface Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        PcieMaxReadRequestInitCallback (PcieEngine, NULL);
      }

      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }

    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMaxReadRequestInterface Exit\n");
}
