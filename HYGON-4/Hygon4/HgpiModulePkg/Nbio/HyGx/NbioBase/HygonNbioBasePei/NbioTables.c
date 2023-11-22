/**
 * @file
 *
 * GNB init tables
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
#include  <Library/GnbCommonLib.h>
#include  <Library/NbioTable.h>
#include  <GnbRegisters.h>
#include  <Filecode.h>

#define FILECODE  NBIO_NBIOBASE_HYGX_HYGONNBIOBASEPEI_NBIOTABLES_FILECODE

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

/*----------------------------------------------------------------------------------------
 *           T A B L E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */

GNB_TABLE ROMDATA  GnbEarlyInitTableIohub[] = {
  // IOHC::NB_CNTL[DISABLE_NBIO_1_3_FOR_DEINTERLEAVING]=1
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_DISABLE_NBIO23,
    TYPE_SMN,
    NB_IOC_CNTL_ADDRESS_HYGX,
    NB_CNTL_DISABLE_HALF_NBIO_FOR_DEINTERLEAVING_MASK,
    (1 << NB_CNTL_DISABLE_HALF_NBIO_FOR_DEINTERLEAVING_OFFSET)
    ),
  // IOHC::NB_IOHC_DEBUG0[NB_IOHC_DEBUG_RW]=0x10 - Terminate p2p atomics
  GNB_ENTRY_RMW (
    NB_IOC_DEBUG_TYPE,
    NB_IOC_DEBUG_ADDRESS_HYGX,
    NBMISC_0004_NB_IOC_DEBUG_RW_MASK,
    (0x10 << NBMISC_0004_NB_IOC_DEBUG_RW_OFFSET)
    ),
  // To enable peer-to-peer operation, set IOHC::IOHC_FEATURE_CNTL[P2P_mode]=1 and PCIE_VDM_CNTL2[VdmP2pMode]=1.
  GNB_ENTRY_WR (
    IOC_FEATURE_CNTL_TYPE,
    IOC_FEATURE_CNTL_ADDRESS_HYGX,
    0xf0400003
    ),
  //set IOHC::IOHC_SDP_PORT_CNTL
  GNB_ENTRY_WR (
    IOC_FEATURE_CNTL_TYPE,
    IOC_SDP_PORT_CNTL_ADDRESS_HYGX,
    0xfcf
    ),
  GNB_ENTRY_RMW (
    PCIE_VDM_CNTL2_TYPE,
    PCIE_VDM_CNTL2_ADDRESS_HYGX,
    NBMISC_0230_VdmP2pMode_MASK,
    (0x1 << NBMISC_0230_VdmP2pMode_OFFSET)
    ),
  // IOHC Initialization
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client0_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client0_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client0_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client0_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client1_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client1_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client1_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client1_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client2_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client2_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client2_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client2_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client3_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client3_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client3_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0x84218421
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client3_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0x84218421
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client4_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client4_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client4_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0x84218421
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client4_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0x84218421
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client5_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client5_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client5_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0x85218521
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client5_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0x85218521
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client0_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client0_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client0_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client0_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client1_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client1_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client1_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client1_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client2_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client2_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client2_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client2_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client3_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client3_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client3_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client3_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client4_Req_BurstTarget_Lower_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client4_Req_BurstTarget_Upper_ADDRESS_HYGX,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client4_Req_TimeSlot_Lower_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client4_Req_TimeSlot_Upper_ADDRESS_HYGX,
    0
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_ADDRESS_HYGX,
    0x02020202
    ),

  // IOAPIC Initialization
  // RCEC
  GNB_ENTRY_RMW (
    FEATURES_ENABLE_TYPE,
    IOAPIC_FEATURES_ENABLE_ADDRESS_HYGX,
    NBIOAPICCFG_000_RCEC_Intr_map_MASK,
    (0x10 << NBIOAPICCFG_000_RCEC_Intr_map_OFFSET)
  ),

  // PCIE0 Port A~H
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR0_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x10 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR1_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x1 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x11 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR2_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x2 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x12 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR3_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x13 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR4_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x4 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x10 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR5_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x5 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x11 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR6_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x6 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x12 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR7_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x7 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x13 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  // PCIE1 Port A
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR8_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x7 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x2 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0xC << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  // EMEI NBIF0
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR9_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x1 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x14 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  // DUJIANG NBIF2
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR10_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x1 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x1 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x15 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  // DUJIANG NBIF3
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR11_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x2 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x1 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x16 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  // PCIE2 Port A~H
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR12_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x0 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x8 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR13_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x1 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x9 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR14_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x2 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0xA << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR15_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0xB << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR16_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x4 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x8 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR17_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x5 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0x9 << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR18_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x6 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0xA << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR19_INTERRUPT_ROUTING_ADDRESS_HYGX,
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_MASK |
    IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_MASK |
    IOAPIC_BR_INTERRUPT_Br_int_Intr_map_MASK,
    (0x7 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_grp_OFFSET) |
    (0x3 << IOAPIC_BR_INTERRUPT_Br_ext_Intr_swz_OFFSET) |
    (0xB << IOAPIC_BR_INTERRUPT_Br_int_Intr_map_OFFSET)
    ),

  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOHC_P2P_CNTL_ADDRESS_HYGX,
    IOHC_P2P_CNTL_DlDownResetEn_MASK,
    (0x0 << IOHC_P2P_CNTL_DlDownResetEn_OFFSET)
    ),

  // To enable dynamic clock gating:. IOHC::IOHC_GLUE_CG_LCLK_CTRL_0[SOFT_OVERRIDE_CLK9~0]=0h
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOHC_Clock_GATING,
    IOHC_GLUE_CG_LCLK_CTRL_0_TYPE,
    IOHC_GLUE_CG_LCLK_CTRL_0_ADDRESS_HYGX,
    NBMISC_0088_SOFT_OVERRIDE_CLK9_MASK |
    NBMISC_0088_SOFT_OVERRIDE_CLK8_MASK |
    NBMISC_0088_SOFT_OVERRIDE_CLK7_MASK |
    NBMISC_0088_SOFT_OVERRIDE_CLK6_MASK |
    NBMISC_0088_SOFT_OVERRIDE_CLK5_MASK |
    NBMISC_0088_SOFT_OVERRIDE_CLK4_MASK |
    NBMISC_0088_SOFT_OVERRIDE_CLK3_MASK |
    NBMISC_0088_SOFT_OVERRIDE_CLK2_MASK |
    NBMISC_0088_SOFT_OVERRIDE_CLK1_MASK |
    NBMISC_0088_SOFT_OVERRIDE_CLK0_MASK,
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK9_OFFSET) |
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK8_OFFSET) |
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK7_OFFSET) |
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK6_OFFSET) |
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK5_OFFSET) |
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK4_OFFSET) |
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK3_OFFSET) |
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK2_OFFSET) |
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK1_OFFSET) |
    (0x0 << NBMISC_0088_SOFT_OVERRIDE_CLK0_OFFSET)
    ),
  // To enable dynamic clock gating:. IOHC::IOHC_GLUE_CG_LCLK_CTRL_1[SOFT_OVERRIDE_CLK9~0]=0h
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOHC_Clock_GATING,
    IOHC_GLUE_CG_LCLK_CTRL_1_TYPE,
    IOHC_GLUE_CG_LCLK_CTRL_1_ADDRESS_HYGX,
    // NBMISC_008C_SOFT_OVERRIDE_CLK9_MASK |
    // NBMISC_008C_SOFT_OVERRIDE_CLK8_MASK |
    // NBMISC_008C_SOFT_OVERRIDE_CLK7_MASK |
    // NBMISC_008C_SOFT_OVERRIDE_CLK6_MASK |
    // NBMISC_008C_SOFT_OVERRIDE_CLK5_MASK |
    // NBMISC_008C_SOFT_OVERRIDE_CLK4_MASK |
    // NBMISC_008C_SOFT_OVERRIDE_CLK3_MASK |
    // NBMISC_008C_SOFT_OVERRIDE_CLK2_MASK |
    NBMISC_008C_SOFT_OVERRIDE_CLK1_MASK |
    NBMISC_008C_SOFT_OVERRIDE_CLK0_MASK,
    // (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK9_OFFSET) |
    // (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK8_OFFSET) |
    // (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK7_OFFSET) |
    // (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK6_OFFSET) |
    // (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK5_OFFSET) |
    // (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK4_OFFSET) |
    // (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK3_OFFSET) |
    // (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK2_OFFSET) |
    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK1_OFFSET) |
    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK0_OFFSET)
    ),
  // Configure IOHC_Bridge_CNTL registers for NBIF0,2,3
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG24_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG25_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG26_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  // Configure IOHC_Bridge_CNTL registers for PCIE ports
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG0_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG1_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG2_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG3_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG4_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG5_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG6_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG7_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
#if 1
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG8_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
#else
    // disable CXL bridge
    GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG8_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    (IOHC_BRIDGE_CNTL_BridgeDis_MASK | IOHC_BRIDGE_CNTL_CfgDis_MASK | IOHC_BRIDGE_CNTL_CrsEnable_MASK),
    (0x1 << IOHC_BRIDGE_CNTL_BridgeDis_OFFSET) | (1 << IOHC_BRIDGE_CNTL_CfgDis_OFFSET) | (1 << IOHC_BRIDGE_CNTL_CrsEnable_OFFSET)
    ),
#endif
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG16_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG17_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG18_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG19_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG20_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG21_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG22_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NB_DEVINDCFG_IOHC_Bridge_CNTL_TYPE,
    NB_DEVINDCFG23_IOHC_Bridge_CNTL_ADDRESS_HYGX,
    IOHC_BRIDGE_CNTL_NoSnoopDis_MASK,
    (0x1 << IOHC_BRIDGE_CNTL_NoSnoopDis_OFFSET)
    ),

  // To Enable/Disable NBIO Poison Consumption
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_POISON_ACTION_CONTROL,
    POISON_ACTION_CONTROL_TYPE,
    POISON_ACTION_CONTROL_ADDRESS_HYGX,
    POISON_ACTION_CONTROL_IntPoisonAPMLErrEn_MASK |
    POISON_ACTION_CONTROL_IntPoisonLinkDisEn_MASK |
    POISON_ACTION_CONTROL_IntPoisonSyncFloodEn_MASK |
    POISON_ACTION_CONTROL_EgressPoisonLSAPMLErrEn_MASK |
    POISON_ACTION_CONTROL_EgressPoisonLSLinkDisEn_MASK |
    POISON_ACTION_CONTROL_EgressPoisonLSSyncFloodEn_MASK |
    POISON_ACTION_CONTROL_EgressPoisonHSAPMLErrEn_MASK |
    POISON_ACTION_CONTROL_EgressPoisonHSLinkDisEn_MASK |
    POISON_ACTION_CONTROL_EgressPoisonHSSyncFloodEn_MASK,
    (0x1 << POISON_ACTION_CONTROL_IntPoisonAPMLErrEn_OFFSET) |
    (0x1 << POISON_ACTION_CONTROL_IntPoisonLinkDisEn_OFFSET) |
    (0x1 << POISON_ACTION_CONTROL_IntPoisonSyncFloodEn_OFFSET) |
    (0x1 << POISON_ACTION_CONTROL_EgressPoisonLSAPMLErrEn_OFFSET) |
    (0x1 << POISON_ACTION_CONTROL_EgressPoisonLSLinkDisEn_OFFSET) |
    (0x1 << POISON_ACTION_CONTROL_EgressPoisonLSSyncFloodEn_OFFSET) |
    (0x1 << POISON_ACTION_CONTROL_EgressPoisonHSAPMLErrEn_OFFSET) |
    (0x1 << POISON_ACTION_CONTROL_EgressPoisonHSLinkDisEn_OFFSET) |
    (0x1 << POISON_ACTION_CONTROL_EgressPoisonHSSyncFloodEn_OFFSET)
    ),
  // set SyncFlood_En/LinkDis_En
  GNB_ENTRY_RMW (
    ErrEvent_ACTION_CONTROL_TYPE,
    ErrEvent_ACTION_CONTROL_ADDRESS_HYGX,
    ErrEvent_ACTION_CONTROL_LinkDis_En_MASK |
    ErrEvent_ACTION_CONTROL_SyncFlood_En_MASK,
    (0x1 << ErrEvent_ACTION_CONTROL_LinkDis_En_OFFSET) |
    (0x1 << ErrEvent_ACTION_CONTROL_SyncFlood_En_OFFSET)
    ),

  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA  GnbEarlyInitTableEmei[] = {
  // NBIF DEVICE INITIALIZATION
  // PSPCCP0 enabled on NBIO0/NBIF0 Func2,NTBCCP0 enabled on NBIO2/NBIF0 Func2, NTBCCP1 enabled on NBIO0/2 NBIF0 Func3
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    TYPE_SMN,
    NBIF0_FUNC2_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    TYPE_SMN,
    NBIF0_FUNC3_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),

  // NTB Enable on  NBIO0/2
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    TYPE_SMN,
    NBIF0_NTB_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),

  // Enable interrupts for NTBCCP and PSPCCP on NBIO0/2 NBIF0 Func2,NTBCCP on NBIO0/2 NBIF0 Func3
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    NBIFMISC_INTR_LINE_POLARITY_TYPE,
    NBIFMISC0_INTR_LINE_POLARITY_ADDRESS_HYGX,
    0x00000000
    ),
  // Need check register
  GNB_ENTRY_PROPERTY_WR (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    NBIFMISC_INTR_LINE_ENABLE_TYPE,
    NBIFMISC0_INTR_LINE_ENABLE_ADDRESS_HYGX,
    0x0000000C
    ),

  // HMI_WRR_WEIGHT_HYGX
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    TYPE_SMN,
    NBIF0_HMI_WRR_WEIGHT_HYGX,
    SMN_HMI_REQ_REALTIME_WEIGHT_MASK |
    SMN_HMI_REQ_NORM_P_WEIGHT_MASK |
    SMN_HMI_REQ_NORM_NP_WEIGHT_MASK,
    (0x8 << SMN_HMI_REQ_REALTIME_WEIGHT_OFFSET) |
    (0x8 << SMN_HMI_REQ_NORM_P_WEIGHT_OFFSET) |
    (0x8 << SMN_HMI_REQ_NORM_NP_WEIGHT_OFFSET)
    ),

  /*
          //Set SDP unit ID to 3 for PCIE core 1
          GNB_ENTRY_PROPERTY_WR (
                  TABLE_PROPERTY_EMEI_FULL_NBIO,
                  TYPE_SMN,
                  NBIO0_PCIE1_STRAP_INDEX_ADDRESS,
                  STRAP_BIF_SDP_UNIT_ID_ADDR
          ),
          GNB_ENTRY_PROPERTY_WR (
                  TABLE_PROPERTY_EMEI_FULL_NBIO,
                  TYPE_SMN,
                  NBIO0_PCIE1_STRAP_DATA_ADDRESS,
                  0x3
          ),
          GNB_ENTRY_PROPERTY_WR (
                  TABLE_PROPERTY_EMEI_FULL_NBIO,
                  TYPE_SMN,
                  PCIE_SDP_CTRL_PCIECORE1_ADDRESS,
                  0x3
          ),
          GNB_ENTRY_PROPERTY_RMW (
                  TABLE_PROPERTY_EMEI_FULL_NBIO,
                  TYPE_SMN,
                  PCIE_SDP_CTRL_PCIECORE1_ADDRESS,
                  PCIE_SDP_CTRL_SDP_UNIT_ID_MASK,
                  (0x3 << PCIE_SDP_CTRL_SDP_UNIT_ID_OFFSET)
          ),
  */
  // Set NBIF multi-function enable
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    NBIF_PCIEP_STRAP_MISC_TYPE,
    NBIF0_PCIEP_STRAP_MISC_ADDRESS_HYGX,
    NBIF_PCIEP_STRAP_MISC_STRAP_MULTI_FUNC_EN_MASK,
    (0x1 << NBIF_PCIEP_STRAP_MISC_STRAP_MULTI_FUNC_EN_OFFSET)
    ),

  // notify SMU nbif0 PM Status
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    TYPE_SMN,
    RCC_NBIF0_STRAP1_ADDRESS_HYGX,
    RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_MASK,
    (0x1 << RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    TYPE_SMN,
    RCC_NBIF1_STRAP1_ADDRESS_HYGX,
    RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_MASK,
    (0x1 << RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_OFFSET)
    ),

  // Nbif B/D/F
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_EMEI_FULL_NBIO,
    TYPE_SMN,
    RCC_NBIF0_STRAP0_ADDRESS_HYGX,
    RCC_BIF_STRAP0_DN_DEVNUM_MASK |
    RCC_BIF_STRAP0_DN_FUNCID_MASK,
    (0x7 << RCC_BIF_STRAP0_DN_DEVNUM_OFFSET) |
    (0x1 << RCC_BIF_STRAP0_DN_FUNCID_OFFSET)
    ),
  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA  GnbEarlyInitTableDj[] = {
  // NBIF DEVICE INITIALIZATION
  // ASP enabled
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF2_ASP_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  // PSPCCP enabled on NBIF3 Func4
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF3_FUNC4_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),

  // XHCI enabled on all NBIO
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF2_USB_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF3_USB_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),

  // Disable optional devices - XGBE, HDAudio
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF3_HDAUDIO_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF2_XGBE0_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF2_XGBE2_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF2_XGBE3_STRAP0_ADDRESS_HYGX,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),

  // Enable interrupts for USB/SATA/ASP on NBIO0 NBIF2,USB/SATA/PSPCCP on NBIO0 NBIF3
  GNB_ENTRY_WR (
    NBIFMISC_INTR_LINE_POLARITY_TYPE,
    NBIFMISC2_INTR_LINE_POLARITY_ADDRESS_HYGX,
    0x00000000
    ),
  // Need check register
  GNB_ENTRY_WR (
    NBIFMISC_INTR_LINE_ENABLE_TYPE,
    NBIFMISC2_INTR_LINE_ENABLE_ADDRESS_HYGX,
    0x0000000E
    ),
  GNB_ENTRY_WR (
    NBIFMISC_INTR_LINE_POLARITY_TYPE,
    NBIFMISC3_INTR_LINE_POLARITY_ADDRESS_HYGX,
    0x00000000
    ),
  // Need check register
  GNB_ENTRY_WR (
    NBIFMISC_INTR_LINE_ENABLE_TYPE,
    NBIFMISC3_INTR_LINE_ENABLE_ADDRESS_HYGX,
    0x00000016
    ),

  // HMI_WRR_WEIGHT_HYGX
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF2_HMI_WRR_WEIGHT_HYGX,
    SMN_HMI_REQ_REALTIME_WEIGHT_MASK |
    SMN_HMI_REQ_NORM_P_WEIGHT_MASK |
    SMN_HMI_REQ_NORM_NP_WEIGHT_MASK,
    (0x8 << SMN_HMI_REQ_REALTIME_WEIGHT_OFFSET) |
    (0x8 << SMN_HMI_REQ_NORM_P_WEIGHT_OFFSET) |
    (0x8 << SMN_HMI_REQ_NORM_NP_WEIGHT_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF3_HMI_WRR_WEIGHT_HYGX,
    SMN_HMI_REQ_REALTIME_WEIGHT_MASK |
    SMN_HMI_REQ_NORM_P_WEIGHT_MASK |
    SMN_HMI_REQ_NORM_NP_WEIGHT_MASK,
    (0x8 << SMN_HMI_REQ_REALTIME_WEIGHT_OFFSET) |
    (0x8 << SMN_HMI_REQ_NORM_P_WEIGHT_OFFSET) |
    (0x8 << SMN_HMI_REQ_NORM_NP_WEIGHT_OFFSET)
    ),

  // NBIFMISC[2-3] SATA RO overide
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF2_BIFC_DMA_ATTR_OVERRIDE_F2_F3_HYGX,
    NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F2_MASK,
    (1 << NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F2_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF3_BIFC_DMA_ATTR_OVERRIDE_F2_F3_HYGX,
    NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F2_MASK,
    (1 << NBIF0_BIFC_DMA_ATTR_OVERRIDE_F2_F3_TX_RO_OVERIDE_P_F2_OFFSET)
    ),

  GNB_ENTRY_RMW (
    SION_WRAPPER_404_SST0_TYPE,
    SION_WRAPPER_404_SST0_ADDRESS_HYGX,
    SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_MASK,
    (0x1 << SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SION_WRAPPER_404_SST1_TYPE,
    SION_WRAPPER_404_SST1_ADDRESS_HYGX,
    SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_MASK,
    (0x1 << SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_OFFSET)
    ),

  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_SST_CLOCK_GATING_DISABLED,
    TYPE_SST0,
    SST_CLOCK_CTRL_SST0_ADDRESS_HYGX,
    SST_CLOCK_CTRL_RXCLKGATEEn_MASK |
    SST_CLOCK_CTRL_TXCLKGATEEn_MASK,
    (0x0 << SST_CLOCK_CTRL_RXCLKGATEEn_OFFSET) |
    (0x0 << SST_CLOCK_CTRL_TXCLKGATEEn_OFFSET)
    ),

  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_SST_CLOCK_GATING_DISABLED,
    TYPE_SST1,
    SST_CLOCK_CTRL_SST1_ADDRESS_HYGX,
    SST_CLOCK_CTRL_RXCLKGATEEn_MASK |
    SST_CLOCK_CTRL_TXCLKGATEEn_MASK,
    (0x0 << SST_CLOCK_CTRL_RXCLKGATEEn_OFFSET) |
    (0x0 << SST_CLOCK_CTRL_TXCLKGATEEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SST0,
    SST_CLOCK_CTRL_SST0_ADDRESS_HYGX,
    SST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK,
    (0xF0 << SST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SST1,
    SST_CLOCK_CTRL_SST1_ADDRESS_HYGX,
    SST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK,
    (0xF0 << SST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET)
    ),

  // Set SDP unit ID to 7 for PCIE core 2
  GNB_ENTRY_WR (
    TYPE_SMN,
    NBIO0_PCIE2_STRAP_INDEX_ADDRESS,
    STRAP_BIF_SDP_UNIT_ID_ADDR
    ),
  GNB_ENTRY_WR (
    TYPE_SMN,
    NBIO0_PCIE2_STRAP_DATA_ADDRESS,
    0x7
    ),
  GNB_ENTRY_WR (
    TYPE_SMN,
    PCIE_SDP_CTRL_PCIECORE2_ADDRESS_HYGX,
    0x7
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    PCIE_SDP_CTRL_PCIECORE2_ADDRESS_HYGX,
    PCIE_SDP_CTRL_SDP_UNIT_ID_MASK,
    (0x7 << PCIE_SDP_CTRL_SDP_UNIT_ID_OFFSET)
    ),
  // Set NBIF multi-function enable
  GNB_ENTRY_RMW (
    NBIF_PCIEP_STRAP_MISC_TYPE,
    NBIF2_PCIEP_STRAP_MISC_ADDRESS_HYGX,
    NBIF_PCIEP_STRAP_MISC_STRAP_MULTI_FUNC_EN_MASK,
    (0x1 << NBIF_PCIEP_STRAP_MISC_STRAP_MULTI_FUNC_EN_OFFSET)
    ),
  GNB_ENTRY_RMW (
    NBIF_PCIEP_STRAP_MISC_TYPE,
    NBIF3_PCIEP_STRAP_MISC_ADDRESS_HYGX,
    NBIF_PCIEP_STRAP_MISC_STRAP_MULTI_FUNC_EN_MASK,
    (0x1 << NBIF_PCIEP_STRAP_MISC_STRAP_MULTI_FUNC_EN_OFFSET)
    ),

  // notify SMU nbif2/3 PM Status
  GNB_ENTRY_RMW (
    TYPE_SMN,
    RCC_NBIF2_STRAP1_ADDRESS_HYGX,
    RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_MASK,
    (0x0 << RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    RCC_NBIF3_STRAP1_ADDRESS_HYGX,
    RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_MASK,
    (0x0 << RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_OFFSET)
    ),

  // Nbif B/D/F
  GNB_ENTRY_RMW (
    TYPE_SMN,
    RCC_NBIF2_STRAP0_ADDRESS_HYGX,
    RCC_BIF_STRAP0_DN_DEVNUM_MASK |
    RCC_BIF_STRAP0_DN_FUNCID_MASK,
    (0x8 << RCC_BIF_STRAP0_DN_DEVNUM_OFFSET) |
    (0x1 << RCC_BIF_STRAP0_DN_FUNCID_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    RCC_NBIF3_STRAP0_ADDRESS_HYGX,
    RCC_BIF_STRAP0_DN_DEVNUM_MASK |
    RCC_BIF_STRAP0_DN_FUNCID_MASK,
    (0x9 << RCC_BIF_STRAP0_DN_DEVNUM_OFFSET) |
    (0x1 << RCC_BIF_STRAP0_DN_FUNCID_OFFSET)
    ),

  //Disable IOAGR Chain NBIF2
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF2_A2S_CNTL_SW0_HYGX,
    0,
    0x8080200
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF2_A2S_CNTL_SW1_HYGX,
    0,
    0x8080205
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF2_A2S_CNTL_SW2_HYGX,
    0,
    0x8080200
    ),
 GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF2_A2S_CNTL_SW3_HYGX,
    0,
    0x8080200
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF2_A2S_CNTL_SW4_HYGX,
    0,
    0x8080200
    ),
  
    //Disable IOAGR Chain NBIF3
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF3_A2S_CNTL_SW0_HYGX,
    0,
    0x8080200
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF3_A2S_CNTL_SW1_HYGX,
    0,
    0x8080205
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF3_A2S_CNTL_SW2_HYGX,
    0,
    0x8080200
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF3_A2S_CNTL_SW3_HYGX,
    0,
    0x8080200
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOAGR_NBIF3_A2S_CNTL_SW4_HYGX,
    0,
    0x8080200
    ),
  GNB_ENTRY_TERMINATE
};
