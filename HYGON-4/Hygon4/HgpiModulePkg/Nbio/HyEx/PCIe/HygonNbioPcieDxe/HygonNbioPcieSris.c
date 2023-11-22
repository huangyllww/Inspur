/* $NoKeywords:$ */
/**
 * @file
 *
 * Configure SRIS for endpoints
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
#include  <Library/GnbPciAccLib.h>
#include  <Library/GnbCommonLib.h>
#include  <GnbRegisters.h>
#include  <Library/NbioRegisterAccLib.h>

#define FILECODE NBIO_PCIE_HYGONNBIOPCIE_HYEX_DXE_HYGONNBIOPCIESRISST_FILECODE
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

#define DEVFUNC(d, f) ((((UINT8) d) << 3) | ((UINT8) f))

UINT8 ROMDATA DefaultPortDevMap [] = {
  DEVFUNC (1, 1),
  DEVFUNC (1, 2),
  DEVFUNC (1, 3),
  DEVFUNC (1, 4),
  DEVFUNC (1, 5),
  DEVFUNC (1, 6),
  DEVFUNC (1, 7),
  DEVFUNC (2, 1),
  DEVFUNC (3, 1),
  DEVFUNC (3, 2),
  DEVFUNC (3, 3),
  DEVFUNC (3, 4),
  DEVFUNC (3, 5),
  DEVFUNC (3, 6),
  DEVFUNC (3, 7),
  DEVFUNC (4, 1),
  DEVFUNC (5, 1),
  DEVFUNC (5, 2)
};
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Find PCIE port logical bridge ID by device and function number
 * defined engine descriptor
 *
 *   Function only called if requested device/function does not much native device/function
 *
 * @param[in]  DeviceNumber        Gpp Device number
 *             FunctionNumber      Gpp Function number
 * @retval     UINT8               Gpp Logical bridge ID
 */

UINT8
PcieGetPortLogicalBridgeId (
  IN      UINT8         DeviceNumber,
  IN      UINT8         FunctionNumber
  )
{
  UINT8                 Index;
  UINT8                 DevFunc;
  
  DevFunc = (DeviceNumber << 3) +  FunctionNumber;
  
  for (Index = 0; Index < (sizeof (DefaultPortDevMap) / sizeof (DefaultPortDevMap[0])); Index++) {
    if (DefaultPortDevMap[Index] == DevFunc) {
      return Index;
    }
  }
  return 0;
}

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
  PCIe_WRAPPER_CONFIG        *PcieWrapper;
  UINT8                      LogicalGppId;

  IDS_HDT_CONSOLE (GNB_TRACE, "  PcieSrisInitCallback for Device = %d:%d:%d\n",
    Engine->Type.Port.Address.Address.Bus,
    Engine->Type.Port.Address.Address.Device,
    Engine->Type.Port.Address.Address.Function
    );
  
  
  if (PcieConfigCheckPortStatus (Engine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
    PcieWrapper = PcieConfigGetParentWrapper (Engine);   
    
    LogicalGppId = PcieGetPortLogicalBridgeId(Engine->Type.Port.PortData.DeviceNumber, Engine->Type.Port.PortData.FunctionNumber);      
    Address0 = ConvertPciePortAddress(PCIE0_GPP0_LINK_CTL3_ADDRESS_HYEX, GnbHandle, PcieWrapper, (LogicalGppId % 8));
    Address1 = ConvertPciePortAddress(PCIE0_GPP0_LC_CNTL6_ADDRESS_HYEX, GnbHandle, PcieWrapper, (LogicalGppId % 8));

    NbioRegisterRead ( GnbHandle, TYPE_SMN, Address1, &Value, 0);
    //may need debug >>>
    if (Engine->Type.Port.SRIS_SRNS == 1) {
      Value |= BIT8;
      Engine->Type.Port.PortData.LinkAspm = AspmDisabled;
    } else if (Engine->Type.Port.SRIS_SRNS == 1) {
      Value |= BIT13;
      Engine->Type.Port.PortData.LinkAspm = AspmDisabled;
    }
    //may need debug <<<
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
 * @retval    HGPI_STATUS
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
