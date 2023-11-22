/* $NoKeywords:$ */

/**
 * @file
 *
 * HSIO Ancillary Data Definitions
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

#ifndef __HSIO_TRAINING_H__
#define __HSIO_TRAINING_H__
#include <Hsio.h>
/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define   SAT_PLATFORM_TYPE  0
#define   ST_PLATFORM_TYPE   1
#define   MAX_INSTANCE_ID    8


#define PCIE_VIDDID_OFT                  0
#define PCIE_VIDDID_SIZE                 4
#define PCIE_SSVIDDID_OFT                0x2C
#define PCIE_SSVIDDID_SIZE               4

#define PCS_NBIO0_PCIE0_BASE_HYGX         0x0D700000
#define PCS_NBIO1_PCIE0_BASE_HYGX         0x0D800000
#define PCS_NBIO2_PCIE0_BASE_HYGX         0x0D900000
#define PCS_NBIO3_PCIE0_BASE_HYGX         0x0DA00000
#define PCS_NBIO0_PCIE1_BASE_HYGX         0x0DB00000
#define PCS_NBIO2_PCIE1_BASE_HYGX         0x0DC00000
#define PCS_NBIO0_PCIE2_BASE_HYGX         0x0DD00000

#define SUP_DIG_SUP_OVRD_IN_1_ADDRESS_HYEX    0x20
#define SUP_DIG_SUP_OVRD_IN_1_ADDRESS_HYGX    0x20
#define TXUP_TERM_OFFSET_OFFSET              0
#define TXUP_TERM_OFFSET_WIDTH               9
#define TXUP_TERM_OFFSET_MASK                0x1FF
#define TXUP_TERM_OFFSET_OVRD_EN_OFFSET      9
#define TXUP_TERM_OFFSET_OVRD_EN_WIDTH       1
#define TXUP_TERM_OFFSET_OVRD_EN_MASK        0x200
#define RX_TERM_OFFSET_OFFSET                10
#define RX_TERM_OFFSET_WIDTH                 5
#define RX_TERM_OFFSET_MASK                  0x7C00
#define RX_TERM_OFFSET_OVRD_EN_OFFSET        15
#define RX_TERM_OFFSET_OVRD_EN_WIDTH         1
#define RX_TERM_OFFSET_OVRD_EN_MASK          0x8000

typedef union {
  struct {
    UINT16 TXUP_TERM_OFFSET:9;
    UINT16 TXUP_TERM_OFFSET_OVRD_EN:1;
    UINT16 RX_TERM_OFFSET:5;
    UINT16 RX_TERM_OFFSET_OVRD_EN:1;
  } Field;
  UINT16 Value;
} SUP_DIG_SUP_OVRD_IN_1;

#define SUP_DIG_SUP_OVRD_IN_2_ADDRESS_HYEX    0x21
#define SUP_DIG_SUP_OVRD_IN_2_ADDRESS_HYGX    0x21
#define TXDN_TERM_OFFSET_OFFSET              0
#define TXDN_TERM_OFFSET_WIDTH               9
#define TXDN_TERM_OFFSET_MASK                0x1FF
#define TXDN_TERM_OFFSET_OVRD_EN_OFFSET      9
#define TXDN_TERM_OFFSET_OVRD_EN_WIDTH       1
#define TXDN_TERM_OFFSET_OVRD_EN_MASK        0x200

typedef union {
  struct {
    UINT16 TXDN_TERM_OFFSET:9;
    UINT16 TXDN_TERM_OFFSET_OVRD_EN:1;
    UINT16 RESERVED_15_0:6;
  } Field;
  UINT16 Value;
} SUP_DIG_SUP_OVRD_IN_2;

#define LANE_DIG_RX_CDR_CNTL_3_HYEX                                 0x109A
#define LANE_DIG_RX_CDR_CNTL_4_HYEX                                 0x109B
#define LANE_DIG_RX_CDR_CNTL_3_HYGX                                 0x109A
#define LANE_DIG_RX_CDR_CNTL_4_HYGX                                 0x109B

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */
HGPI_STATUS
PcieMapPortPciAddress (
  IN      PCIe_ENGINE_CONFIG     *Engine,
  IN      UINT8 *PortDevMap
  );

VOID
PcieSetPortPciAddressMap (
  IN      GNB_HANDLE    *GnbHandle,
  IN      UINT8         *PortDevMap
  );

VOID
NbioEnableNbifDevice (
  IN        GNB_HANDLE         *GnbHandle,
  IN        UINT8              DeviceType,
  IN        UINT8              StartLane
  );

VOID
NbioDisableNbifDevice (
  IN        GNB_HANDLE         *GnbHandle,
  IN        UINT8              DeviceType,
  IN        UINT8              StartLane
  );

UINT32
HsioAncDataCalcSize (
  IN       HSIO_PORT_DESCRIPTOR    *PortPointer
  );

UINT32
HsioAncDataPopulate (
  IN       HSIO_ENGINE_DESCRIPTOR  *EnginePointer,
  IN       HSIO_PORT_DESCRIPTOR    *PortPointer,
  IN       UINT8                   *AncData
  );

VOID
HsioSetOperatingSpeed (
  IN        GNB_HANDLE                      *GnbHandle,
  IN        PCIe_ENGINE_CONFIG              *PcieEngineList,
  IN        HSIO_ENGINE_DESCRIPTOR          *HsioEnginePointer,
  IN        HSIO_PORT_DESCRIPTOR            *UserCfgPortPointer
  );

HGPI_STATUS
HsioGetUserCfgForSocket (
  IN      UINT8                     SocketNumber,
  IN      HSIO_COMPLEX_DESCRIPTOR   *UserComplex,
  IN OUT  HSIO_PORT_DESCRIPTOR      **PortList
  );

#endif
