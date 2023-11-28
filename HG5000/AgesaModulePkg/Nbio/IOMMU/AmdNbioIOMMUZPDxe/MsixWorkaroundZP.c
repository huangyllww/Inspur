/* $NoKeywords:$ */
/**
 * @file
 *
 * Workaround for MSI-X on USB
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
#include  <Library/AmdBaseLib.h>
#include  <Library/GnbCommonLib.h>
#include  <Library/GnbPcieConfigLib.h>
#include  <Library/PcieMiscCommLib.h>
#include  <Library/PcdLib.h>
#include  <Library/NbioSmuV9Lib.h>
#include  <OptionGnb.h>

#define FILECODE NBIO_IOMMU_AMDNBIOIOMMUZPDXE_MSIXWORKAROUND_FILECODE

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define MSIX_CAP_ID         0x11
#define PT_USB_VEN_DEV_ID   0x43B91022UL

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U  R E S
 *----------------------------------------------------------------------------------------
 */
typedef struct {
  GNB_PCI_SCAN_DATA       ScanData;
  UINT32                  Pi1UsbTrapAddress;
  UINT32                  Pi1UsbTrapValue;
  BOOLEAN                 Pi1UsbTrapFilled;
  UINT32                  Pi2UsbTrapAddress;
  UINT32                  Pi2UsbTrapValue;
  BOOLEAN                 Pi2UsbTrapFilled;
  UINT32                  Pi3UsbTrapAddress;
  UINT32                  Pi3UsbTrapValue;
  BOOLEAN                 Pi3UsbTrapFilled;
  BOOLEAN                 NVCardFound;
} MSIX_WORKAROUND_DATA;


/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------*/
/*
 * Find PCI capability pointer
 *
 *
 *
 * @param[in]      Address         PCI address (as described in PCI_ADDR)
 * @param[in]      CapabilityId    PCI capability ID
 * @param[in][out] PreviousCap     Pointer to location to store the "previous" capability pointer
 * @param[in]      StdHeader       Standard configuration header
 * @retval                         Register address of capability pointer
 *
 */

UINT8
IommuFindPciCapability (
  IN       UINT32              Address,
  IN       UINT8               CapabilityId,
  IN OUT   UINT8               *PreviousCap,
  IN       AMD_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8     CapabilityPtr;
  UINT8     CurrentCapabilityId;

  CapabilityPtr = 0x34;
  *PreviousCap = CapabilityPtr;

  if (!GnbLibPciIsDevicePresent (Address, StdHeader)) {
    return  0;
  }
  while (CapabilityPtr != 0) {
    GnbLibPciRead (Address | CapabilityPtr, AccessWidth8 , &CapabilityPtr, StdHeader);
    if (CapabilityPtr != 0) {
      GnbLibPciRead (Address | CapabilityPtr , AccessWidth8 , &CurrentCapabilityId, StdHeader);
      if (CurrentCapabilityId == CapabilityId) {
        break;
      }
      *PreviousCap = CapabilityPtr;
      CapabilityPtr++;
    }
  }
  return  CapabilityPtr;
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
IommuMsixWorkaroundCallback (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  )
{
  PCIE_DEVICE_TYPE        DeviceType;
  UINT32                  Value;
  UINT32                  VendorId;
  UINT8                   PcieCapPtr;
  UINT8                   PrevCapPtr;
  UINT32                  TrapAddress;
  UINT32                  TrapValue;
  MSIX_WORKAROUND_DATA    *MsixData;

  IDS_HDT_CONSOLE (GNB_TRACE, "  IommuFindUsbDeviceCallback for Device = %d:%d:%d\n",
    Device.Address.Bus,
    Device.Address.Device,
    Device.Address.Function
    );
  MsixData = (MSIX_WORKAROUND_DATA*) ScanData;
  GnbLibPciRead (
    Device.AddressValue,
    AccessWidth32,
    &VendorId,
    ScanData->StdHeader
  );
  VendorId =  VendorId & 0xFFFF;
  if ((VendorId == 0x10DE) || (VendorId == 0x103C) || (VendorId == 0x1590)) {
    MsixData->NVCardFound = TRUE;
  }
  GnbLibPciRead (
    Device.AddressValue | 0x08,
    AccessWidth32,
    &Value,
    ScanData->StdHeader
  );
  if ((Value >> 16) == 0x0C03) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  Found USB Device = %d:%d:%d\n",
                     Device.Address.Bus,
                     Device.Address.Device,
                     Device.Address.Function
                     );
    PcieCapPtr = IommuFindPciCapability (Device.AddressValue, MSIX_CAP_ID, &PrevCapPtr, ScanData->StdHeader);
    if (PcieCapPtr != 0) {
      TrapAddress =  Device.AddressValue | PrevCapPtr;
      GnbLibPciRead (
        Device.AddressValue | PcieCapPtr,
        AccessWidth32,
        &Value,
        ScanData->StdHeader
      );
      GnbLibPciRead (
        Device.AddressValue | PrevCapPtr,
        AccessWidth32,
        &TrapValue,
        ScanData->StdHeader
      );
      TrapValue &= 0xFFFF00FF;
      Value &= 0x0000FF00;
      TrapValue |= Value;
      IDS_HDT_CONSOLE (GNB_TRACE, "  TrapAddress = 0x%x\n", TrapAddress);
      IDS_HDT_CONSOLE (GNB_TRACE, "  TrapValue   = 0x%x\n", TrapValue);
      GnbLibPciRead (
        Device.AddressValue,
        AccessWidth32,
        &Value,
        ScanData->StdHeader
      );
      if (Value != PT_USB_VEN_DEV_ID) {
        if (MsixData->Pi1UsbTrapFilled == FALSE) {
          MsixData->Pi1UsbTrapAddress = TrapAddress;
          MsixData->Pi1UsbTrapValue = TrapValue;
          MsixData->Pi1UsbTrapFilled = TRUE;
        } else if (MsixData->Pi2UsbTrapFilled == FALSE) {
          MsixData->Pi2UsbTrapAddress = TrapAddress;
          MsixData->Pi2UsbTrapValue = TrapValue;
          MsixData->Pi2UsbTrapFilled = TRUE;
        } else if (MsixData->Pi3UsbTrapFilled == FALSE) {
          MsixData->Pi3UsbTrapAddress = TrapAddress;
          MsixData->Pi3UsbTrapValue = TrapValue;
          MsixData->Pi3UsbTrapFilled = TRUE;
        }
      }
    }
  }

  DeviceType = GnbLibGetPcieDeviceType (Device, ScanData->StdHeader);
  switch (DeviceType) {
  case  PcieDeviceRootComplex:
  case  PcieDeviceDownstreamPort:
  case  PcieDeviceUpstreamPort:
    GnbLibPciScanSecondaryBus (Device, ScanData);
    break;
  case  PcieDeviceEndPoint:
  case  PcieDeviceLegacyEndPoint:
    break;
  default:
    break;
  }
  return SCAN_SUCCESS;
}

/**----------------------------------------------------------------------------------------*/
/**
 * Interface to disable MSIx on PCIE USB interface
 *
 *
 *
 * @param[in]  GnbHandle      Pointer to the Silicon Descriptor for this node
 *
 */
 /*----------------------------------------------------------------------------------------*/

VOID
IommuMsixWorkaround (
  IN       GNB_HANDLE  *GnbHandle
  )
{
  PCIe_ENGINE_CONFIG             *PcieEngine;
  PCIe_WRAPPER_CONFIG            *PcieWrapper;
  MSIX_WORKAROUND_DATA           MsixData;
  GNB_HANDLE                     *ThisGnb;
  UINT32                         SmuArg[6];

  IDS_HDT_CONSOLE (GNB_TRACE, "IommuMsixWorkaround Enter\n");

  MsixData.ScanData.StdHeader = NULL;
  MsixData.ScanData.GnbScanCallback = IommuMsixWorkaroundCallback;
  MsixData.Pi1UsbTrapAddress = 0;
  MsixData.Pi1UsbTrapValue = 0;
  MsixData.Pi1UsbTrapFilled = FALSE;
  MsixData.Pi2UsbTrapAddress = 0;
  MsixData.Pi2UsbTrapValue = 0;
  MsixData.Pi2UsbTrapFilled = FALSE;
  MsixData.Pi3UsbTrapAddress = 0;
  MsixData.Pi3UsbTrapValue = 0;
  MsixData.Pi3UsbTrapFilled = FALSE;
  MsixData.NVCardFound = FALSE;

  PcieWrapper = PcieConfigGetChildWrapper (GnbHandle);
  while (PcieWrapper != NULL) {
    PcieEngine = PcieConfigGetChildEngine (PcieWrapper);
    while (PcieEngine != NULL) {
      if (PcieLibIsEngineAllocated (PcieEngine)) {
        GnbLibPciScan (PcieEngine->Type.Port.Address, PcieEngine->Type.Port.Address, &MsixData.ScanData);
      }
      PcieEngine = PcieLibGetNextDescriptor (PcieEngine);
    }
    PcieWrapper = PcieLibGetNextDescriptor (PcieWrapper);
  }

  NbioSmuServiceCommonInitArgumentsV9 (SmuArg);
  if (MsixData.Pi1UsbTrapFilled == TRUE) {
    IDS_HDT_CONSOLE (GNB_TRACE, "  Pi1UsbTrapAddress = 0x%x\n", MsixData.Pi1UsbTrapAddress);
    IDS_HDT_CONSOLE (GNB_TRACE, "  Pi1UsbTrapValue   = 0x%x\n", MsixData.Pi1UsbTrapValue);
    SmuArg[0] = MsixData.Pi1UsbTrapAddress;
    SmuArg[1] = MsixData.Pi1UsbTrapValue;
    if (MsixData.Pi2UsbTrapFilled == TRUE) {
      IDS_HDT_CONSOLE (GNB_TRACE, "  Pi2UsbTrapAddress = 0x%x\n", MsixData.Pi2UsbTrapAddress);
      IDS_HDT_CONSOLE (GNB_TRACE, "  Pi2UsbTrapValue   = 0x%x\n", MsixData.Pi2UsbTrapValue);
      SmuArg[2] = MsixData.Pi2UsbTrapAddress;
      SmuArg[3] = MsixData.Pi2UsbTrapValue;
      if (MsixData.Pi3UsbTrapFilled == TRUE) {
        IDS_HDT_CONSOLE (GNB_TRACE, "  Pi3UsbTrapAddress = 0x%x\n", MsixData.Pi3UsbTrapAddress);
        IDS_HDT_CONSOLE (GNB_TRACE, "  Pi3UsbTrapValue   = 0x%x\n", MsixData.Pi3UsbTrapValue);
        SmuArg[4] = MsixData.Pi3UsbTrapAddress;
        SmuArg[5] = MsixData.Pi3UsbTrapValue;
      }
    }
  }
  if (MsixData.NVCardFound == TRUE) {
    SmuArg[0] |= 1 << 31;
  }
  ThisGnb = GnbHandle;
  while (ThisGnb != NULL) {
    NbioSmuServiceRequestV9 (NbioGetHostPciAddress(ThisGnb), SMC_MSG_IommuInterruptRemap, SmuArg, GNB_REG_ACC_FLAG_S3SAVE);
    ThisGnb = GnbGetNextHandle (ThisGnb);
  }

  IDS_HDT_CONSOLE (GNB_TRACE, "IommuMsixWorkaround Exit\n");
}

