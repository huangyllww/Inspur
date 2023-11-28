/* $NoKeywords:$ */
/**
 * @file
 *
 * Various PCI service routines.
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

#ifndef _GNBLIBPCI_H_
#define _GNBLIBPCI_H_

#include "Porting.h"

#define PCIE_EXT_AER_CAP_ID         0x01
#define PCIE_EXT_CAP_ID             0x1E
#define PCIE_LTR_EXT_CAP_ID         0x18
#define PCIE_CAP_ID                 0x10
#define IOMMU_CAP_ID                0x0F
#define ARI_CAP_ID                  0x0E

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

#define SCAN_SKIP_FUNCTIONS         0x1
#define SCAN_SKIP_DEVICES           0x2
#define SCAN_SKIP_BUSES             0x4
#define SCAN_SUCCESS                0x0

//  Forward declaration needed for multi-structure mutual references
AGESA_FORWARD_DECLARATION (GNB_PCI_SCAN_DATA);

typedef SCAN_STATUS (*GNB_SCAN_CALLBACK) (
  IN       PCI_ADDR             Device,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

///Scan supporting data
struct _GNB_PCI_SCAN_DATA {
  GNB_SCAN_CALLBACK     GnbScanCallback;  ///< Callback for each found device
  AMD_CONFIG_PARAMS     *StdHeader;       ///< Standard configuration header
};

#define   PCIE_CAP_ID                0x10
#define   PCIE_LINK_CAP_REGISTER     0x0C
#define   PCIE_LINK_CTRL_REGISTER    0x10
#define   PCIE_DEVICE_CAP_REGISTER   0x04
#define   PCIE_DEVICE_CTRL_REGISTER  0x08
#define   PCIE_DEVICE_CAP2_REGISTER  0x1C
#define   PCIE_DEVICE_CNTL2_REGISTER 0x20
#define   PCIE_ASPM_L1_SUPPORT_CAP   BIT11
#define   PCIE_SLOT_CAP_REGISTER     0x14

#define MAX_PAYLOAD_128             0x0     ///< Max allowed payload size 128 bytes
#define MAX_PAYLOAD_256             0x1     ///< Max allowed payload size 256 bytes
#define MAX_PAYLOAD_512             0x2     ///< Max allowed payload size 512 bytes
#define MAX_PAYLOAD_1024            0x3     ///< Max allowed payload size 1024 bytes
#define MAX_PAYLOAD_2048            0x4     ///< Max allowed payload size 2048 bytes
#define MAX_PAYLOAD_4096            0x5     ///< Max allowed payload size 4096 bytes
#define MAX_PAYLOAD                 0x5     ///< Max allowed payload size according to spec is 101b (4096 bytes)

#define MAX_READREQUESTSIZE_128     0x0     ///< Max allowed read request size 128 bytes
#define MAX_READREQUESTSIZE_256     0x1     ///< Max allowed read request size 256 bytes
#define MAX_READREQUESTSIZE_512     0x2     ///< Max allowed read request size 512 bytes
#define MAX_READREQUESTSIZE_1024    0x3     ///< Max allowed read request size 1024 bytes
#define MAX_READREQUESTSIZE_2048    0x4     ///< Max allowed read request size 2048 bytes
#define MAX_READREQUESTSIZE_4096    0x5     ///< Max allowed read request size 4096 bytes

BOOLEAN
GnbLibPciIsDevicePresent (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

BOOLEAN
GnbLibPciIsBridgeDevice (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

BOOLEAN
GnbLibPciIsMultiFunctionDevice (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

BOOLEAN
GnbLibPciIsPcieDevice (
  IN      UINT32              Address,
  IN      AMD_CONFIG_PARAMS   *StdHeader
 );

UINT8
GnbLibFindPciCapability (
  IN      UINT32              Address,
  IN      UINT8               CapabilityId,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

UINT16
GnbLibFindPcieExtendedCapability (
  IN      UINT32              Address,
  IN      UINT16              ExtendedCapabilityId,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
GnbLibPciScan (
  IN      PCI_ADDR            Start,
  IN      PCI_ADDR            End,
  IN      GNB_PCI_SCAN_DATA   *ScanData
 );

VOID
GnbLibPciScanSecondaryBus (
  IN       PCI_ADDR             Bridge,
  IN OUT   GNB_PCI_SCAN_DATA    *ScanData
  );

PCIE_DEVICE_TYPE
GnbLibGetPcieDeviceType (
  IN      PCI_ADDR            Device,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

VOID
GnbLibS3SaveConfigSpace (
  IN      UINT32              Address,
  IN      UINT16              StartRegisterAddress,
  IN      UINT16              EndRegisterAddress,
  IN      ACCESS_WIDTH        Width,
  IN      AMD_CONFIG_PARAMS   *StdHeader
  );

#endif
