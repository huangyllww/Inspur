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

#ifndef _RASPCILIB_H_
#define _RASPCILIB_H_
#include  <AMD.h>

#define PCIE_EXT_AER_CAP_ID         (0x01)
#define DPC_EXT_CAP_ID              (0x1D)
#define PCIE_EXT_CAP_ID             (0x1E)
#define SEC_PCIE_EXT_CAP_ID         (0x19)
#define PCIE_LTR_EXT_CAP_ID         (0x18)
#define PCIE_CAP_ID                 (0x10)
#define IOMMU_CAP_ID                (0x0F)
#define ARI_CAP_ID                  (0x0E)

/// PCIe device type
typedef enum {
  PcieDeviceEndPoint,                       ///< Endpoint
  PcieDeviceLegacyEndPoint,                 ///< Legacy endpoint
  PcieDeviceRootComplex = 4,                ///< Root complex
  PcieDeviceUpstreamPort,                   ///< Upstream port
  PcieDeviceDownstreamPort,                 ///< Downstream Port
  PcieDevicePcieToPcix,                     ///< PCIe to PCI/PCIx bridge
  PcieDevicePcixToPcie,                     ///< PCI/PCIx to PCIe bridge
  PcieNotPcieDevice = 0xff                  ///< unknown device
} PCIE_DEVICE_TYPE;

typedef  UINT32 SCAN_STATUS;

#define SCAN_SKIP_FUNCTIONS         (0x1)
#define SCAN_SKIP_DEVICES           (0x2)
#define SCAN_SKIP_BUSES             (0x4)
#define SCAN_SUCCESS                (0x0)

#define PCIE_UNCORR_STATUS_PTR      (0x04)
#define PCIE_UNCORR_MASK_PTR        (0x08)
#define PCIE_CORR_STATUS_PTR        (0x10)
#define PCIE_CORR_MASK_PTR          (0x14)
#define PCIE_ROOT_ERR_CMD_PTR       (0x2C)
#define PCIE_ROOT_STATUS_PTR        (0x30)
#define PCIE_ROOT_STATUS_FATAL_ERROR_MASK (BIT4 | BIT6)
#define PCIE_DEVICE_CONTROL_PTR     (0x08)
#define PCIE_DEVICE_STATUS_PTR      (0x0A)

#define PCIE_DEV_CORR_ERR           (0x1)
#define PCIE_DEV_NON_FATAL_ERR      (0x2)
#define PCIE_DEV_FATAL_ERR          (0x4)
#define PCIE_DEV_USR_DETECTED       (0x8)

#define PCIE_ROOT_CORR_ERR           (0x1)
#define PCIE_ROOT_NON_FATAL_ERR      (0x2)
#define PCIE_ROOT_FATAL_ERR          (0x4)

#define PCIE_LINK_CONTROL3_PTR      (0x04)
#define PCIE_LANE_ERR_STATUS_PTR    (0x08)

//  Forward declaration needed for multi-structure mutual references
typedef struct _RAS_PCI_SCAN_DATA RAS_PCI_SCAN_DATA;

typedef SCAN_STATUS (*RAS_SCAN_CALLBACK) (
  IN       PCI_ADDR             Device,
  IN OUT   RAS_PCI_SCAN_DATA    *ScanData
  );

///Scan supporting data
struct _RAS_PCI_SCAN_DATA {
  RAS_SCAN_CALLBACK     RasScanCallback;  ///< Callback for each found device
  VOID                  *Buffer;          ///< Input Buffer

};

#define   PCIE_CAP_ID                (0x10)
#define   PCIE_LINK_CAP_REGISTER     (0x0C)
#define   PCIE_LINK_CTRL_REGISTER    (0x10)
#define   PCIE_DEVICE_CAP_REGISTER   (0x04)
#define   PCIE_DEVICE_CTRL_REGISTER  (0x08)
#define   PCIE_DEVICE_CAP2_REGISTER  (0x1C)
#define   PCIE_DEVICE_CNTL2_REGISTER (0x20)
#define   PCIE_ASPM_L1_SUPPORT_CAP   (BIT11)
#define   PCIE_SLOT_CAP_REGISTER     (0x14)

BOOLEAN
RasPciIsDevicePresent (
  IN      UINT32              Address
 );

BOOLEAN
RasPciIsBridgeDevice (
  IN      UINT32              Address
 );

BOOLEAN
RasPciIsMultiFunctionDevice (
  IN      UINT32              Address
 );

BOOLEAN
RasPciIsPcieDevice (
  IN      UINT32              Address
 );

UINT8
RasFindPciCapability (
  IN      UINT32              Address,
  IN      UINT8               CapabilityId
  );

UINT16
RasFindPcieExtendedCapability (
  IN      UINT32              Address,
  IN      UINT16              ExtendedCapabilityId
  );

VOID
RasPciScan (
  IN      PCI_ADDR            Start,
  IN      PCI_ADDR            End,
  IN      RAS_PCI_SCAN_DATA   *ScanData
 );

VOID
RasPciScanSecondaryBus (
  IN       PCI_ADDR             Bridge,
  IN OUT   RAS_PCI_SCAN_DATA    *ScanData
  );

PCIE_DEVICE_TYPE
RasGetPcieDeviceType (
  IN      PCI_ADDR            Device
  );

#endif // _RASPCILIB_H_
