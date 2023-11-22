/* $NoKeywords:$ */

/**
 * @file
 *
 * Configure Max Payload
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
#include <Library/HygonSocBaseLib.h>

#define FILECODE  LIBRARY_PCIEMISCCOMMLIB_PCIEMAXPAYLOADLIB_FILECODE

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
  UINT8                MaxPayload;
  BOOLEAN              Extend_Tag;
} PCIE_MAX_PAYLOAD_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

SCAN_STATUS
PcieGetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

SCAN_STATUS
PcieSetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

//
// HGPI_STATUS
// PciePayloadBlackListFeature (
// IN       PCI_ADDR               Device,
// IN       UINT8                  *MaxPayload,
// IN       HYGON_CONFIG_PARAMS      *StdHeader
// );

/*----------------------------------------------------------------------------------------*/

/**
 * Determine maximum payload size for PCIe segment
 *
 *  Scan through all link in segment to determine maximum payload by EPs.
 *
 * @param[in]   DownstreamPort    PCI address of PCIe port
 * @param[in]   EngineMaxPayload  MaxPayload supported by the engine
 * @param[in]   StdHeader         Standard configuration header
 *
 */
VOID
PcieSetMaxPayload (
  IN       PCI_ADDR                DownstreamPort,
  IN       UINT8                   EngineMaxPayload,
  IN       HYGON_CONFIG_PARAMS       *StdHeader
  )
{
  PCIE_MAX_PAYLOAD_DATA  PcieMaxPayloadData;

  IDS_HDT_CONSOLE (
    GNB_TRACE,
    "  PcieSetMaxPayload for Device = %d:%d:%d\n",
    DownstreamPort.Address.Bus,
    DownstreamPort.Address.Device,
    DownstreamPort.Address.Function
    );
  PcieMaxPayloadData.MaxPayload = EngineMaxPayload;
  PcieMaxPayloadData.Extend_Tag = FALSE;
  PcieMaxPayloadData.ScanData.StdHeader = StdHeader;
  PcieMaxPayloadData.ScanData.GnbScanCallback = PcieGetMaxPayloadCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieMaxPayloadData.ScanData);
  PcieMaxPayloadData.ScanData.GnbScanCallback = PcieSetMaxPayloadCallback;
  GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieMaxPayloadData.ScanData);
  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSetMaxPayloadExit\n");
}

/*----------------------------------------------------------------------------------------*/

/**
 * Evaluate device Max Payload - save SMALLEST Max Payload for PCIe Segment
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */
SCAN_STATUS
PcieGetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_MAX_PAYLOAD_DATA  *PcieMaxPayloadData;
  PCIE_DEVICE_TYPE       DeviceType;
  UINT32                 Value;
  UINT8                  PcieCapPtr;
  UINT8                  DeviceMaxPayload;

  PcieMaxPayloadData = (PCIE_MAX_PAYLOAD_DATA *)ScanData;
  IDS_HDT_CONSOLE (
    GNB_TRACE,
    "  PcieGetMaxPayloadCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CAP_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
      );
    DeviceMaxPayload = (UINT8)(Value & 0x7);
    if (Value & BIT5) {
      PcieMaxPayloadData->Extend_Tag = TRUE;
    }

    PciePayloadBlackListFeature (Device, &DeviceMaxPayload, ScanData->StdHeader);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Found DeviceMaxPayload as %d (Value = %x\n", DeviceMaxPayload, Value);
    if (DeviceMaxPayload < PcieMaxPayloadData->MaxPayload) {
      PcieMaxPayloadData->MaxPayload = DeviceMaxPayload;
    }
  }

  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
    case PcieDeviceRootComplex:
    case PcieDeviceDownstreamPort:
    case PcieDeviceUpstreamPort:
      GnbLibPciScanSecondaryBus (Device, &PcieMaxPayloadData->ScanData);
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
 * Configure the Max Payload setting to all devices in the PCIe Segment
 *
 *
 *
 * @param[in]     Device          PCI Address
 * @param[in,out] ScanData        Scan configuration data
 * @retval                        Scan Status of 0
 */
SCAN_STATUS
PcieSetMaxPayloadCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_MAX_PAYLOAD_DATA  *PcieMaxPayloadData;
  PCIE_DEVICE_TYPE       DeviceType;
  UINT8                  PcieCapPtr;
  UINT32                 Value;

  PcieMaxPayloadData = (PCIE_MAX_PAYLOAD_DATA *)ScanData;
  IDS_HDT_CONSOLE (
    GNB_TRACE,
    "  PcieSetMaxPayloadCallback for Device = %d:%d:%d to %d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function,
    PcieMaxPayloadData->MaxPayload
    );
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
      );
    Value = (Value & 0xFFFFFF1F) | (PcieMaxPayloadData->MaxPayload << 5);
    if (PcieMaxPayloadData->Extend_Tag) {
      Value |= BIT8;
      IDS_HDT_CONSOLE (GNB_TRACE, "EXTEND_TAG_EN PCIE_DEVICE_CTRL=%08x\n", Value);
    }

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
      GnbLibPciScanSecondaryBus (Device, &PcieMaxPayloadData->ScanData);
      break;
    case PcieDeviceEndPoint:
    case PcieDeviceLegacyEndPoint:
      break;
    default:
      break;
  }

  return SCAN_SUCCESS;
}

UINT16  PayloadBlacklistDeviceTable[] = {
  0x1969, 0x1083, (UINT16)MAX_PAYLOAD_128
};

/*----------------------------------------------------------------------------------------*/

/**
 * Pcie Max_Payload_Size Black List
 *
 *
 *
 * @param[in] Device              PCI_ADDR of PCIe Device to evaluate
 * @param[in] MaxPayload          Pointer to Max_Payload_Size value
 * @param[in] StdHeader           Standard configuration header
 * @retval     HGPI_STATUS
 */
HGPI_STATUS
PciePayloadBlackListFeature (
  IN       PCI_ADDR               Device,
  IN       UINT8                  *MaxPayload,
  IN       HYGON_CONFIG_PARAMS      *StdHeader
  )
{
  UINT32  TargetDeviceId;
  UINTN   i;
  UINT32  DeviceId;
  UINT32  VendorId;

  GnbLibPciRead (Device.AddressValue, AccessWidth32, &TargetDeviceId, StdHeader);
  for (i = 0; i < (sizeof (PayloadBlacklistDeviceTable) / sizeof (UINT16)); i = i + 3) {
    VendorId = PayloadBlacklistDeviceTable[i];
    DeviceId = PayloadBlacklistDeviceTable[i + 1];
    if (VendorId == (UINT16)TargetDeviceId) {
      if (DeviceId == 0xFFFF || DeviceId == (TargetDeviceId >> 16)) {
        *MaxPayload = (UINT8)PayloadBlacklistDeviceTable[i + 2];
      }
    }
  }

  return HGPI_SUCCESS;
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
PcieMaxPayloadInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  UINT8    EngineMaxPayload;
  BOOLEAN  MaxPayloadEnable;
  UINT16   linkwidth;

  MaxPayloadEnable = PcdGetBool (PcdCfgMaxPayloadEnable);

  if ((MaxPayloadEnable != 0) &&
      (!PcieConfigIsSbPcieEngine (Engine)) &&
      (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS))) {
    EngineMaxPayload =  Engine->Type.Port.MaxPayloadSize;
    if (Engine->EngineData.EndLane >= Engine->EngineData.StartLane) {
      linkwidth = Engine->EngineData.EndLane - Engine->EngineData.StartLane + 1;
    } else {
      linkwidth = Engine->EngineData.StartLane - Engine->EngineData.EndLane + 1;
    }

    if (linkwidth >= 4) {
      EngineMaxPayload =  MAX_PAYLOAD_512;
    } else if (linkwidth == 2) {
      EngineMaxPayload =  MAX_PAYLOAD_256;
    } else if (linkwidth == 1) {
      EngineMaxPayload =  MAX_PAYLOAD_128;
    }
    if (GetHygonSocModel() == HYGON_GX_CPU) {
      //Default set max payload to 256 for GX CPU
      EngineMaxPayload = MAX_PAYLOAD_256;
    }
    if (PcdGet8 (PcdCfgMaxPayloadSize) != 0xFF) {
      EngineMaxPayload = PcdGet8 (PcdCfgMaxPayloadSize);
    }
    IDS_HDT_CONSOLE (GNB_TRACE, "%dX Device = %d:%d:%d MaxPayload = %x\n", linkwidth, Engine->Type.Port.Address.Address.Bus, Engine->Type.Port.Address.Address.Device, Engine->Type.Port.Address.Address.Function, EngineMaxPayload);
    PcieSetMaxPayload (Engine->Type.Port.Address, EngineMaxPayload, NULL);
  }
}

/**----------------------------------------------------------------------------------------*/

/**
 * Interface to configure MaxPayloadSize on PCIE interface
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
/*----------------------------------------------------------------------------------------*/

VOID
PcieMaxPayloadInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG   *PcieEngine;
  PCIe_WRAPPER_CONFIG  *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMaxPayloadInterface Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        PcieMaxPayloadInitCallback (PcieEngine, NULL);
      }

      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }

    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieMaxPayloadInterface Exit\n");
}
