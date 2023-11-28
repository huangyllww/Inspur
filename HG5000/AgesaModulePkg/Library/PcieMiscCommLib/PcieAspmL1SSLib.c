/* $NoKeywords:$ */
/**
 * @file
 *
 * PCIe link ASPM L1 SS
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
#include  <OptionGnb.h>
#include  <Filecode.h>
#include  <Library/IdsLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/PcieMiscCommLib.h>
#include  <Library/NbioRegisterAccLib.h>

#define FILECODE LIBRARY_PCIEMISCCOMMLIB_PCIEASPML1SSLIB_FILECODE
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
  BOOLEAN                 AspmL1_1;
  BOOLEAN                 AspmL1_2;
  PCI_ADDR                DownstreamPort;
} PCIE_ASPM_L1SS_DATA;

typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  BOOLEAN                 LTREn;
  PCI_ADDR                DownstreamPort;
} PCIE_LTR_DATA;
/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/**
 * Enable PCIE Advance state power management
 *
 *
 *
 * @param[in] Device              PCI Address of the downstream port
 * @param[in] AspmL1_1            ASPM L1 SS
 * @param[in] AspmL1_2            ASPM L1 SS
 * @param[in] StdHeader           Standard configuration header
 */

VOID
PcieLinkAspmL1SSEnable (
  IN       PCI_ADDR                Device,
  IN       BOOLEAN                 AspmL1_1,
  IN       BOOLEAN                 AspmL1_2,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT16  PcieCapPtr;
  UINT32  CapValue;
  UINT32  Value;

  PcieCapPtr = GnbLibFindPcieExtendedCapability (Device.AddressValue, PCIE_EXT_CAP_ID, StdHeader);
  if (PcieCapPtr == 0) {
    return;
  }
  GnbLibPciRead (
    Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CAP_REGISTER),
    AccessWidth32,
    &CapValue,
    StdHeader
    );
  if (CapValue & BIT4) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
      AccessWidth32,
      &Value,
      StdHeader
      );
    if (AspmL1_1) {
      if (CapValue & BIT3) {
        Value |= BIT3;
      }
      if (CapValue & BIT1) {
        Value |= BIT1;
      }
    }
    if (AspmL1_2) {
      if (CapValue & BIT2) {
        Value |= BIT2;
      }
      if (CapValue & BIT0) {
        Value |= BIT0;
      }
    }
    GnbLibPciWrite (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
      AccessS3SaveWidth32,
      &Value,
      StdHeader
      );
    IDS_HDT_CONSOLE (GNB_TRACE, "  Set Aspm L1 SS for Device = %d:%d:%d Value = %08x\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function,
      Value
      );
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
GetPcieAspmL1SSCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS           ScanStatus;
  PCIE_ASPM_L1SS_DATA   *PcieAspmL1SSData;
  PCIE_DEVICE_TYPE      DeviceType;
  UINT16  PcieCapPtr;
  UINT32  CapValue;
  UINT32  Value;

  ScanStatus = SCAN_SUCCESS;
  PcieAspmL1SSData = (PCIE_ASPM_L1SS_DATA*) ScanData;
  PcieCapPtr = GnbLibFindPcieExtendedCapability (Device.AddressValue, PCIE_EXT_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CAP_REGISTER),
      AccessWidth32,
      &CapValue,
      ScanData->StdHeader
      );
    if (CapValue & BIT4) {
      GnbLibPciRead (
        Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
        AccessWidth32,
        &Value,
        ScanData->StdHeader
        );
        if (!((CapValue & BIT3) || (CapValue & BIT1)))  {
          PcieAspmL1SSData->AspmL1_1 = FALSE;
        }
        if (!((CapValue & BIT2) || (CapValue & BIT0)))  {
          PcieAspmL1SSData->AspmL1_2 = FALSE;
        }
      DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
      switch (DeviceType) {
      case  PcieDeviceRootComplex:
      case  PcieDeviceDownstreamPort:
      case  PcieDeviceUpstreamPort:
        GnbLibPciScanSecondaryBus (Device, &PcieAspmL1SSData->ScanData);
        break;
      case  PcieDeviceEndPoint:
      case  PcieDeviceLegacyEndPoint:
        ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
        break;
      default:
        break;
      }
    } else {
      PcieAspmL1SSData->AspmL1_1 = FALSE;
      PcieAspmL1SSData->AspmL1_2 = FALSE;
    }
  } else {
    PcieAspmL1SSData->AspmL1_1 = FALSE;
    PcieAspmL1SSData->AspmL1_2 = FALSE;
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "  GetPcieAspmL1SSCallback for Device = %d:%d:%d (AspmL1_1=%d, AspmL1_2=%d)\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function,
    PcieAspmL1SSData->AspmL1_1,
    PcieAspmL1SSData->AspmL1_2
    );
  return ScanStatus;
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
SetPcieAspmL1SSCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS             ScanStatus;
  PCIE_ASPM_L1SS_DATA     *PcieAspmL1SSData;
  PCIE_DEVICE_TYPE        DeviceType;

  ScanStatus = SCAN_SUCCESS;
  PcieAspmL1SSData = (PCIE_ASPM_L1SS_DATA *) ScanData;
  IDS_HDT_CONSOLE (GNB_TRACE, "  SetPcieAspmL1SSCallback for Device = %d:%d:%d (AspmL1_1=%d, AspmL1_2=%d)\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function,
    PcieAspmL1SSData->AspmL1_1,
    PcieAspmL1SSData->AspmL1_2
    );
  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    PcieLinkAspmL1SSEnable (
      Device,
      PcieAspmL1SSData->AspmL1_1,
      PcieAspmL1SSData->AspmL1_2,
      ScanData->StdHeader
      );
    GnbLibPciScanSecondaryBus (Device, &PcieAspmL1SSData->ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    PcieLinkAspmL1SSEnable (
      Device,
      PcieAspmL1SSData->AspmL1_1,
      PcieAspmL1SSData->AspmL1_2,
      ScanData->StdHeader
      );
    ScanStatus = SCAN_SKIP_FUNCTIONS | SCAN_SKIP_DEVICES | SCAN_SKIP_BUSES;
    break;
  default:
    IDS_HDT_CONSOLE (GNB_TRACE, "  Unknow DeviceType %d\n", DeviceType);
    break;
  }
  return ScanStatus;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init PM L1 SS on all active ports
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 */

VOID
PcieAspmL1SSPortInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIE_ASPM_L1SS_DATA PcieAspmL1SSData;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmL1SSPortInitCallback Enter\n");
  PcieAspmL1SSData.AspmL1_1 = (BOOLEAN) Engine->Type.Port.LinkAspmL1_1;
  PcieAspmL1SSData.AspmL1_2 = (BOOLEAN) Engine->Type.Port.LinkAspmL1_2;
  PcieAspmL1SSData.ScanData.StdHeader = NULL;

  PcieAspmL1SSData.ScanData.GnbScanCallback = GetPcieAspmL1SSCallback;
  GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieAspmL1SSData.ScanData);
  Engine->Type.Port.LinkAspmL1_1 = PcieAspmL1SSData.AspmL1_1;
  Engine->Type.Port.LinkAspmL1_2 = PcieAspmL1SSData.AspmL1_2;

  if ((PcieAspmL1SSData.AspmL1_1) || (PcieAspmL1SSData.AspmL1_2)) {
    PcieAspmL1SSData.ScanData.GnbScanCallback = SetPcieAspmL1SSCallback;
    GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieAspmL1SSData.ScanData);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmL1SSPortInitCallback Exit\n");
}

/*----------------------------------------------------------------------------------------*/
/**
 * set Snoop/non-snoop latency on EP
 *
 *
 *
 * @param[in] Device              PCI Address of the downstream port
 * @param[in] StdHeader           Standard configuration header
 */

VOID
SetPcieEPSnooplatency (
  IN       PCI_ADDR                Device,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT16  PcieCapPtr;
  UINT32  Value;

  PcieCapPtr = GnbLibFindPcieExtendedCapability (Device.AddressValue, PCIE_LTR_EXT_CAP_ID, StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + 0x04),
      AccessWidth32,
      &Value,
      StdHeader
      );
    Value = (Value & 0xE000E000) | 0x10011001;
    GnbLibPciWrite (
      Device.AddressValue | (PcieCapPtr + 0x04),
      AccessS3SaveWidth32,
      &Value,
      StdHeader
      );
    IDS_HDT_CONSOLE (GNB_TRACE, "  Set Snoop/non-snoop latency for Device = %d:%d:%d Value = %08x\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function,
      Value
      );
  }
}

/*----------------------------------------------------------------------------------------*/
/**
 * Set LC_LTR_L1_2_thresholds on RC
 *
 *
 *
 * @param[in] Device              PCI Address of the downstream port
 * @param[in] AspmL1_1            ASPM L1 SS
 * @param[in] AspmL1_2            ASPM L1 SS
 * @param[in] StdHeader           Standard configuration header
 */

VOID
SetPcieLC_LTR_L1_2_thresholds (
  IN       PCI_ADDR                Device,
  IN       AMD_CONFIG_PARAMS       *StdHeader
  )
{
  UINT16  PcieCapPtr;
  UINT32  Value;

  PcieCapPtr = GnbLibFindPcieExtendedCapability (Device.AddressValue, PCIE_EXT_CAP_ID, StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
      AccessWidth32,
      &Value,
      StdHeader
      );
    Value = (Value & 0x1C00FFFF) | 0x60010000;
    GnbLibPciWrite (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CTRL_REGISTER),
      AccessS3SaveWidth32,
      &Value,
      StdHeader
      );
    IDS_HDT_CONSOLE (GNB_TRACE, "  Set LC_LTR_L1_2_thresholds for Device = %d:%d:%d Value = %08x\n",
      Device.Address.Bus,
      Device.Address.Device,
      Device.Address.Function,
      Value
      );
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
GetPcieLTRCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  SCAN_STATUS           ScanStatus;
  PCIE_LTR_DATA         *PcieLTRData;
  PCIE_DEVICE_TYPE      DeviceType;
  UINT8                 PcieCapPtr;
  UINT32                Value;

  ScanStatus = SCAN_SUCCESS;
  PcieLTRData = (PCIE_LTR_DATA*) ScanData;
  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CAP2_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
    );
    if (Value & BIT11) {
      PcieLTRData->LTREn = TRUE;
    } else {
      PcieLTRData->LTREn = FALSE;
    }
    DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
    switch (DeviceType) {
    case  PcieDeviceRootComplex:
    case  PcieDeviceDownstreamPort:
    case  PcieDeviceUpstreamPort:
      GnbLibPciScanSecondaryBus (Device, &PcieLTRData->ScanData);
      break;
    case  PcieDeviceEndPoint:
    case  PcieDeviceLegacyEndPoint:
      break;
    default:
      break;
    }
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "  GetPcieLTRCallback for Device = %d:%d:%d (LTR_SUPPORTED=%x)\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function,
    PcieLTRData->LTREn
    );
  return ScanStatus;
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
SetPcieLTRCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_LTR_DATA         *PcieLTRData;
  PCIE_DEVICE_TYPE      DeviceType;
  UINT8                 PcieCapPtr;
  UINT32                Value;

  PcieLTRData = (PCIE_LTR_DATA*) ScanData;
  IDS_HDT_CONSOLE (GNB_TRACE, "  SetPcieLTRCallback for Device = %d:%d:%d to %d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function,
    PcieLTRData->LTREn
    );

  PcieCapPtr = GnbLibFindPciCapability (Device.AddressValue, PCIE_CAP_ID, ScanData->StdHeader);
  if (PcieCapPtr != 0) {
    GnbLibPciRead (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CNTL2_REGISTER),
      AccessWidth32,
      &Value,
      ScanData->StdHeader
    );
    Value |= BIT10;
    GnbLibPciWrite (
      Device.AddressValue | (PcieCapPtr + PCIE_DEVICE_CNTL2_REGISTER),
      AccessS3SaveWidth32,
      &Value,
      ScanData->StdHeader
      );

    DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
    switch (DeviceType) {
    case  PcieDeviceRootComplex:
    case  PcieDeviceDownstreamPort:
    case  PcieDeviceUpstreamPort:
      SetPcieLC_LTR_L1_2_thresholds (Device, ScanData->StdHeader);
      GnbLibPciScanSecondaryBus (Device, &PcieLTRData->ScanData);
      break;
    case  PcieDeviceEndPoint:
    case  PcieDeviceLegacyEndPoint:
      SetPcieEPSnooplatency (Device, ScanData->StdHeader);
      break;
    default:
      break;
    }
  }
  return SCAN_SUCCESS;
}

/*----------------------------------------------------------------------------------------*/
/**
 * Callback to init LTR on all active ports
 *
 *
 *
 * @param[in]       Engine          Pointer to engine config descriptor
 * @param[in, out]  Buffer          Not used
 */

VOID
PcieLTRPortInitCallback (
  IN       PCIe_ENGINE_CONFIG    *Engine,
  IN OUT   VOID                  *Buffer
  )
{
  PCIE_LTR_DATA PcieLTRData;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieLTRPortInitCallback Enter\n");
  PcieLTRData.LTREn = FALSE;
  PcieLTRData.ScanData.StdHeader = NULL;

  PcieLTRData.ScanData.GnbScanCallback = GetPcieLTRCallback;
  GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieLTRData.ScanData);

  if (PcieLTRData.LTREn) {
    PcieLTRData.ScanData.GnbScanCallback = SetPcieLTRCallback;
    GnbLibPciScan (Engine->Type.Port.Address, Engine->Type.Port.Address, &PcieLTRData.ScanData);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieLTRPortInitCallback Exit\n");
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to enable PM L1 SS
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
 /*----------------------------------------------------------------------------------------*/
VOID
PcieAspmL1SSInterface (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;

  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmL1SSInterface Enter\n");
  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        if (((PcieEngine->Type.Port.LinkAspmL1_1 == 1) || (PcieEngine->Type.Port.LinkAspmL1_2 == 1)) &&
            PcieConfigCheckPortStatus (PcieEngine, INIT_STATUS_PCIE_TRAINING_SUCCESS)) {
          PcieAspmL1SSPortInitCallback (PcieEngine, NULL);
          if ((PcieEngine->Type.Port.LinkAspmL1_1 == 1) || (PcieEngine->Type.Port.LinkAspmL1_2 == 1)) {
            // after enable L1 SS, then re-enable ASPM L1
            PcieEngine->Type.Port.PortData.LinkAspm = AspmL1;
          }
          if (PcieEngine->Type.Port.LinkAspmL1_2 == 1) {
            // if L1.2 is enabled, LTR has to be enabled
            if (PcieWrapper->WrapId == 0) {
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11140304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11141304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11142304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11143304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11144304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11145304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11146304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11147304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
            } else {
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11240304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11241304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11242304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11243304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11244304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11245304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11246304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
              NbioRegisterRMW (GnbHandle, TYPE_SMN, 0x11247304, (UINT32) ~(BIT5), BIT5, GNB_REG_ACC_FLAG_S3SAVE);
            }
            PcieLTRPortInitCallback (PcieEngine, NULL);
          }
        }
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }
  IDS_HDT_CONSOLE (GNB_TRACE, "PcieAspmL1SSInterface Exit\n");
}
