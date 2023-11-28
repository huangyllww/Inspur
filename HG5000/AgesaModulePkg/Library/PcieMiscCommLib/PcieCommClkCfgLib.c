/* $NoKeywords:$ */
/**
 * @file
 *
 * Program Common Clock Configuration
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 313706 $   @e \$Date: 2015-02-25 21:00:43 -0600 (Wed, 25 Feb 2015) $
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
#include  <Library/GnbCommonLib.h>
#include  <Library/PcieMiscCommLib.h>

#define FILECODE LIBRARY_PCIEMISCCOMMLIB_PCIECOMMCLKCFGLIB_FILECODE
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
  PCI_ADDR                DownstreamPort;
} PCIE_COMM_CLK_DATA;

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Retrain Pcie
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
PcieRetrain (
  IN       PCI_ADDR             Function,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8       PcieCapPtr;
  UINT32      Value;

  PcieCapPtr = GnbLibFindPciCapability (Function.AddressValue, PCIE_CAP_ID, StdHeader);
  Value = BIT27;

  if (PcieCapPtr != 0) {
    GnbLibPciRMW (
      Function.AddressValue | (PcieCapPtr + PCIE_LINK_CTRL_REGISTER),
      AccessS3SaveWidth32,
      ~(UINT32) (BIT5),
      BIT5,
      StdHeader
      );
    IDS_HDT_CONSOLE (GNB_TRACE, "      PcieRetrain link on Device = %d:%d:%d\n", Function.Address.Bus, Function.Address.Device, Function.Address.Function);
    do {
      GnbLibPciRead (
        Function.AddressValue | (PcieCapPtr + PCIE_LINK_CTRL_REGISTER),
        AccessS3SaveWidth32,
        &Value,
        StdHeader);
    } while ((Value & BIT27) != 0);
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Enable common clock configuration on function of the device
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
PcieProgramCommClkCfgOnFunction (
  IN       PCI_ADDR             Function,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{
  UINT8       PcieCapPtr;
  PcieCapPtr = GnbLibFindPciCapability (Function.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr != 0) {
    IDS_HDT_CONSOLE (GNB_TRACE, "      Program Common Clock configuration for Device = %d:%d:%d\n", Function.Address.Bus, Function.Address.Device, Function.Address.Function);
    GnbLibPciRMW (
      Function.AddressValue | (PcieCapPtr + PCIE_LINK_CTRL_REGISTER),
      AccessS3SaveWidth32,
      ~(UINT32) (BIT6),
      BIT6,
      StdHeader
      );
  }
}

/**----------------------------------------------------------------------------------------*/
/**
 * check capability of device
 *
 *
 *
 * @param[in] Device      PCI address of downstream port
 * @param[in] StdHeader   Standard configuration header
 *
 * @retval    TRUE - Device support Common Clock Configuration
 */
 /*----------------------------------------------------------------------------------------*/
BOOLEAN
STATIC
IsPcieCommClk (
  IN       PCI_ADDR                 Device,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{

  UINT8   PcieCapPtr;
  UINT32  Value;

  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, StdHeader);
  if (PcieCapPtr == 0) {
    return FALSE;
  }
  GnbLibPciRead (
    Device.AddressValue | (PcieCapPtr + PCIE_LINK_CTRL_REGISTER),
    AccessWidth32,
    &Value,
    StdHeader
    );
  if ((Value & BIT28) != 0) {
    return TRUE;
  }


  return FALSE;
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
PcieProgramCommClkCfgOnDevice (
  IN       PCI_ADDR             Device,
  IN       AMD_CONFIG_PARAMS    *StdHeader
  )
{

  UINT8 MaxFunc;
  UINT8 CurrentFunc;

  MaxFunc = GnbLibPciIsMultiFunctionDevice (Device.AddressValue, StdHeader) ? 7 : 0;
  for (CurrentFunc = 0; CurrentFunc <= MaxFunc; CurrentFunc++) {
    Device.Address.Function = CurrentFunc;
    if (GnbLibPciIsDevicePresent (Device.AddressValue, StdHeader)) {
      PcieProgramCommClkCfgOnFunction (Device, StdHeader);
    }
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 *  Program common clock on link
 *
 *
 *
 * @param[in]       Downstream          PCI Address of downstrteam port
 * @param[in]       Upstream            PCI Address of upstream port
 * @param[in]       StdHeader           Standard configuration header
 */

VOID
STATIC
PcieProgramCommClkCfgOnLink (
  IN       PCI_ADDR                 Downstream,
  IN       PCI_ADDR                 Upstream,
  IN       AMD_CONFIG_PARAMS        *StdHeader
  )
{

  BOOLEAN              UpCommClk;
  BOOLEAN              DownCommClk;

  UpCommClk = IsPcieCommClk (Upstream, StdHeader);


  IDS_HDT_CONSOLE (GNB_TRACE, "  Read Slot Clock Config [%d]  for Device = %d:%d:%d\n",
    UpCommClk,
    Upstream.Address.Bus,
    Upstream.Address.Device,
    Upstream.Address.Function
    );

  DownCommClk = IsPcieCommClk (Downstream, StdHeader);
  IDS_HDT_CONSOLE (GNB_TRACE, "  Read Slot Clock Config [%d]  for Device = %d:%d:%d\n",
    DownCommClk,
    Downstream.Address.Bus,
    Downstream.Address.Device,
    Downstream.Address.Function
    );

  if (UpCommClk && DownCommClk) {
    // Program  Donstream component (upper bus) by function
    PcieProgramCommClkCfgOnFunction (Downstream, StdHeader);
    // Program  Upstream component (current device/bus) by device, need to check multi-func device
    PcieProgramCommClkCfgOnDevice (Upstream, StdHeader);
    // Retrain link on upper
    PcieRetrain (Downstream, StdHeader);
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
PcieCommClkCfgCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_DEVICE_TYPE        DeviceType;
  PCIE_COMM_CLK_DATA      *PcieCommClkData;

  ScanStatus = SCAN_SUCCESS;
  IDS_HDT_CONSOLE (GNB_TRACE, "  Enter PcieCommClkCfgCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );

  PcieCommClkData = (PCIE_COMM_CLK_DATA *) ScanData;
  ScanStatus = SCAN_SUCCESS;
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);

  switch (DeviceType) {
  case  PcieDeviceRootComplex:
    IDS_HDT_CONSOLE (GNB_TRACE, "    PCI Scan Root port..\n");
    PcieCommClkData->DownstreamPort = Device;
    GnbLibPciScanSecondaryBus (Device, &PcieCommClkData->ScanData);
    IDS_HDT_CONSOLE (GNB_TRACE, "    PCI Scan Root port back.\n");
    break;

  case  PcieDeviceDownstreamPort:
    IDS_HDT_CONSOLE (GNB_TRACE, "    PCI Scan Downstream port..\n");
    PcieCommClkData->DownstreamPort = Device;
    GnbLibPciScanSecondaryBus (Device, &PcieCommClkData->ScanData);
    IDS_HDT_CONSOLE (GNB_TRACE, "    PCI Scan Downstream back.\n");
    break;

  case  PcieDeviceUpstreamPort:
    IDS_HDT_CONSOLE (GNB_TRACE, "    PCI Scan Upstream port..\n");
    PcieProgramCommClkCfgOnLink (PcieCommClkData->DownstreamPort, Device, ScanData->StdHeader);
    GnbLibPciScanSecondaryBus (Device, &PcieCommClkData->ScanData);
    IDS_HDT_CONSOLE (GNB_TRACE, "    PCI Scan upstream back.\n");
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;

  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    IDS_HDT_CONSOLE (GNB_TRACE, "    PCI Scan found End-Point device.\n");
    PcieProgramCommClkCfgOnLink (PcieCommClkData->DownstreamPort, Device, ScanData->StdHeader);
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
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
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 *
 */

VOID
STATIC
PcieCommClkCfgPortInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCI_ADDR               DownstreamPort;
  PCIE_COMM_CLK_DATA     PcieCommClkData;

  PcieCommClkData.ScanData.StdHeader = NULL;
  PcieCommClkData.ScanData.GnbScanCallback = PcieCommClkCfgCallback;

  DownstreamPort = Engine->Type.Port.Address;

  if (!PcieConfigIsSbPcieEngine (Engine) && PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    GnbLibPciScan (DownstreamPort, DownstreamPort, &PcieCommClkData.ScanData);
  }
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to program coomon clock configuration
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
 /*----------------------------------------------------------------------------------------*/

VOID
PcieCommClkCfgInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieCommClkCfgInterface Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        PcieCommClkCfgPortInitCallback (PcieEngine, NULL);
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieCommClkCfgInterface Exit\n");
}
