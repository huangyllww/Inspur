/**
 * @file
 *
 * GNB init tables
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
#include  <Library/GnbCommonLib.h>
#include  <Library/NbioTable.h>
#include  <GnbRegistersZP.h>
#include <Filecode.h>

#define FILECODE        NBIO_NBIOBASE_AMDNBIOBASEZPPEI_NBIOTABLESZP_FILECODE

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

GNB_TABLE ROMDATA GnbEarlyInitTableZP [] = {
  // IOHC::NB_IOHC_DEBUG0[NB_IOHC_DEBUG_RW]=0x10 - Terminate p2p atomics
  GNB_ENTRY_RMW (
    NB_IOC_DEBUG_TYPE,
    NB_IOC_DEBUG_ADDRESS,
    NBMISC_0004_NB_IOC_DEBUG_RW_MASK,
    (0x10 << NBMISC_0004_NB_IOC_DEBUG_RW_OFFSET)
    ),
  // To enable peer-to-peer operation, set IOHC::IOHC_FEATURE_CNTL[P2P_mode]=1 and PCIE_VDM_CNTL2[VdmP2pMode]=1.
  GNB_ENTRY_RMW (
    IOC_FEATURE_CNTL_TYPE,
    IOC_FEATURE_CNTL_ADDRESS,
    NBMISC_0118_P2P_mode_MASK,
    (0x1 << NBMISC_0118_P2P_mode_OFFSET)
    ),
  GNB_ENTRY_RMW (
    PCIE_VDM_CNTL2_TYPE,
    PCIE_VDM_CNTL2_ADDRESS,
    NBMISC_0230_VdmP2pMode_MASK,
    (0x1 << NBMISC_0230_VdmP2pMode_OFFSET)
    ),
  //IOHC Initialization
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client0_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client0_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client0_Req_TimeSlot_Lower_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client0_Req_TimeSlot_Upper_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client1_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client1_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client1_Req_TimeSlot_Lower_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client1_Req_TimeSlot_Upper_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client2_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client2_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client2_Req_TimeSlot_Lower_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client2_Req_TimeSlot_Upper_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client3_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client3_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client3_Req_TimeSlot_Lower_ADDRESS,
    0x84218421
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client3_Req_TimeSlot_Upper_ADDRESS,
    0x84218421
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client4_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client4_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client4_Req_TimeSlot_Lower_ADDRESS,
    0x84218421
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client4_Req_TimeSlot_Upper_ADDRESS,
    0x84218421
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client5_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client5_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S0_Client5_Req_TimeSlot_Lower_ADDRESS,
    0x85218521
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S0_Client5_Req_TimeSlot_Upper_ADDRESS,
    0x85218521
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client0_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client0_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client0_Req_TimeSlot_Lower_ADDRESS,
    0x42185421
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client0_Req_TimeSlot_Upper_ADDRESS,
    0x21542185
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client1_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client1_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client1_Req_TimeSlot_Lower_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client1_Req_TimeSlot_Upper_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client2_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client2_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client2_Req_TimeSlot_Lower_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client2_Req_TimeSlot_Upper_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client3_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client3_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client3_Req_TimeSlot_Lower_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client3_Req_TimeSlot_Upper_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_Req_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client4_Req_BurstTarget_Lower_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_Req_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client4_Req_BurstTarget_Upper_ADDRESS,
    0x08080808
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_Req_TimeSlot_Lower_TYPE,
    IOHC_SION_S1_Client4_Req_TimeSlot_Lower_ADDRESS,
    0x21212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_Req_TimeSlot_Upper_TYPE,
    IOHC_SION_S1_Client4_Req_TimeSlot_Upper_ADDRESS,
    0x00212121
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_TYPE,
    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Lower_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client0_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client1_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client2_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client3_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client4_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S0_Client5_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client0_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client1_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client2_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client3_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client4_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),
  GNB_ENTRY_WR (
    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_TYPE,
    IOHC_SION_S1_Client5_RdRsp_BurstTarget_Upper_ADDRESS,
    0x02020202
    ),

  //IOAPIC Initialization
  GNB_ENTRY_RMW (
    IOAPIC_BR0_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR0_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_040_Br0_ext_Intr_grp_MASK |
    NBIOAPICCFG_040_Br0_ext_Intr_swz_MASK |
    NBIOAPICCFG_040_Br0_int_Intr_map_MASK,
    (0x0 << NBIOAPICCFG_040_Br0_ext_Intr_grp_OFFSET) |
    (0x0 << NBIOAPICCFG_040_Br0_ext_Intr_swz_OFFSET) |
    (0x10 << NBIOAPICCFG_040_Br0_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR1_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR1_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_044_Br1_ext_Intr_grp_MASK |
    NBIOAPICCFG_044_Br1_ext_Intr_swz_MASK |
    NBIOAPICCFG_044_Br1_int_Intr_map_MASK,
    (0x1 << NBIOAPICCFG_044_Br1_ext_Intr_grp_OFFSET) |
    (0x0 << NBIOAPICCFG_044_Br1_ext_Intr_swz_OFFSET) |
    (0x11 << NBIOAPICCFG_044_Br1_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR2_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR2_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_048_Br2_ext_Intr_grp_MASK |
    NBIOAPICCFG_048_Br2_ext_Intr_swz_MASK |
    NBIOAPICCFG_048_Br2_int_Intr_map_MASK,
    (0x2 << NBIOAPICCFG_048_Br2_ext_Intr_grp_OFFSET) |
    (0x0 << NBIOAPICCFG_048_Br2_ext_Intr_swz_OFFSET) |
    (0x12 << NBIOAPICCFG_048_Br2_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR3_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR3_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_04C_Br3_ext_Intr_grp_MASK |
    NBIOAPICCFG_04C_Br3_ext_Intr_swz_MASK |
    NBIOAPICCFG_04C_Br3_int_Intr_map_MASK,
    (0x3 << NBIOAPICCFG_04C_Br3_ext_Intr_grp_OFFSET) |
    (0x0 << NBIOAPICCFG_04C_Br3_ext_Intr_swz_OFFSET) |
    (0x13 << NBIOAPICCFG_04C_Br3_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR4_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR4_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_050_Br4_ext_Intr_grp_MASK |
    NBIOAPICCFG_050_Br4_ext_Intr_swz_MASK |
    NBIOAPICCFG_050_Br4_int_Intr_map_MASK,
    (0x4 << NBIOAPICCFG_050_Br4_ext_Intr_grp_OFFSET) |
    (0x0 << NBIOAPICCFG_050_Br4_ext_Intr_swz_OFFSET) |
    (0x10 << NBIOAPICCFG_050_Br4_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR5_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR5_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_054_Br5_ext_Intr_grp_MASK |
    NBIOAPICCFG_054_Br5_ext_Intr_swz_MASK |
    NBIOAPICCFG_054_Br5_int_Intr_map_MASK,
    (0x5 << NBIOAPICCFG_054_Br5_ext_Intr_grp_OFFSET) |
    (0x0 << NBIOAPICCFG_054_Br5_ext_Intr_swz_OFFSET) |
    (0x11 << NBIOAPICCFG_054_Br5_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR6_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR6_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_058_Br6_ext_Intr_grp_MASK |
    NBIOAPICCFG_058_Br6_ext_Intr_swz_MASK |
    NBIOAPICCFG_058_Br6_int_Intr_map_MASK,
    (0x6 << NBIOAPICCFG_058_Br6_ext_Intr_grp_OFFSET) |
    (0x0 << NBIOAPICCFG_058_Br6_ext_Intr_swz_OFFSET) |
    (0x12 << NBIOAPICCFG_058_Br6_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR7_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR7_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_05C_Br7_ext_Intr_grp_MASK |
    NBIOAPICCFG_05C_Br7_ext_Intr_swz_MASK |
    NBIOAPICCFG_05C_Br7_int_Intr_map_MASK,
    (0x7 << NBIOAPICCFG_05C_Br7_ext_Intr_grp_OFFSET) |
    (0x0 << NBIOAPICCFG_05C_Br7_ext_Intr_swz_OFFSET) |
    (0x13 << NBIOAPICCFG_05C_Br7_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR8_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR8_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_060_Br8_ext_Intr_grp_MASK |
    NBIOAPICCFG_060_Br8_ext_Intr_swz_MASK |
    NBIOAPICCFG_060_Br8_int_Intr_map_MASK,
    (0x7 << NBIOAPICCFG_060_Br8_ext_Intr_grp_OFFSET) |
    (0x2 << NBIOAPICCFG_060_Br8_ext_Intr_swz_OFFSET) |
    (0xC << NBIOAPICCFG_060_Br8_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR9_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR9_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_064_Br9_ext_Intr_grp_MASK |
    NBIOAPICCFG_064_Br9_ext_Intr_swz_MASK |
    NBIOAPICCFG_064_Br9_int_Intr_map_MASK,
    (0x6 << NBIOAPICCFG_064_Br9_ext_Intr_grp_OFFSET) |
    (0x2 << NBIOAPICCFG_064_Br9_ext_Intr_swz_OFFSET) |
    (0xD << NBIOAPICCFG_064_Br9_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR10_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR10_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_068_Br10_ext_Intr_grp_MASK |
    NBIOAPICCFG_068_Br10_ext_Intr_swz_MASK |
    NBIOAPICCFG_068_Br10_int_Intr_map_MASK,
    (0x5 << NBIOAPICCFG_068_Br10_ext_Intr_grp_OFFSET) |
    (0x2 << NBIOAPICCFG_068_Br10_ext_Intr_swz_OFFSET) |
    (0xE << NBIOAPICCFG_068_Br10_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR11_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR11_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_06C_Br11_ext_Intr_grp_MASK |
    NBIOAPICCFG_06C_Br11_ext_Intr_swz_MASK |
    NBIOAPICCFG_06C_Br11_int_Intr_map_MASK,
    (0x4 << NBIOAPICCFG_06C_Br11_ext_Intr_grp_OFFSET) |
    (0x2 << NBIOAPICCFG_06C_Br11_ext_Intr_swz_OFFSET) |
    (0xF << NBIOAPICCFG_06C_Br11_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR12_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR12_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_070_Br12_ext_Intr_grp_MASK |
    NBIOAPICCFG_070_Br12_ext_Intr_swz_MASK |
    NBIOAPICCFG_070_Br12_int_Intr_map_MASK,
    (0x3 << NBIOAPICCFG_070_Br12_ext_Intr_grp_OFFSET) |
    (0x2 << NBIOAPICCFG_070_Br12_ext_Intr_swz_OFFSET) |
    (0xC << NBIOAPICCFG_070_Br12_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR13_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR13_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_074_Br13_ext_Intr_grp_MASK |
    NBIOAPICCFG_074_Br13_ext_Intr_swz_MASK |
    NBIOAPICCFG_074_Br13_int_Intr_map_MASK,
    (0x2 << NBIOAPICCFG_074_Br13_ext_Intr_grp_OFFSET) |
    (0x2 << NBIOAPICCFG_074_Br13_ext_Intr_swz_OFFSET) |
    (0xD << NBIOAPICCFG_074_Br13_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR14_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR14_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_078_Br14_ext_Intr_grp_MASK |
    NBIOAPICCFG_078_Br14_ext_Intr_swz_MASK |
    NBIOAPICCFG_078_Br14_int_Intr_map_MASK,
    (0x1 << NBIOAPICCFG_078_Br14_ext_Intr_grp_OFFSET) |
    (0x2 << NBIOAPICCFG_078_Br14_ext_Intr_swz_OFFSET) |
    (0xE << NBIOAPICCFG_078_Br14_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR15_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR15_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_07C_Br15_ext_Intr_grp_MASK |
    NBIOAPICCFG_07C_Br15_ext_Intr_swz_MASK |
    NBIOAPICCFG_07C_Br15_int_Intr_map_MASK,
    (0x0 << NBIOAPICCFG_07C_Br15_ext_Intr_grp_OFFSET) |
    (0x2 << NBIOAPICCFG_07C_Br15_ext_Intr_swz_OFFSET) |
    (0xF << NBIOAPICCFG_07C_Br15_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR16_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR16_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_080_Br16_ext_Intr_grp_MASK |
    NBIOAPICCFG_080_Br16_ext_Intr_swz_MASK |
    NBIOAPICCFG_080_Br16_int_Intr_map_MASK,
    (0x3 << NBIOAPICCFG_080_Br16_ext_Intr_grp_OFFSET) |
    (0x1 << NBIOAPICCFG_080_Br16_ext_Intr_swz_OFFSET) |
    (0x14 << NBIOAPICCFG_080_Br16_int_Intr_map_OFFSET)
    ),
  GNB_ENTRY_RMW (
    IOAPIC_BR17_INTERRUPT_ROUTING_TYPE,
    IOAPIC_BR17_INTERRUPT_ROUTING_ADDRESS,
    NBIOAPICCFG_084_Br17_ext_Intr_grp_MASK |
    NBIOAPICCFG_084_Br17_ext_Intr_swz_MASK |
    NBIOAPICCFG_084_Br17_int_Intr_map_MASK,
    (0x4 << NBIOAPICCFG_084_Br17_ext_Intr_grp_OFFSET) |
    (0x3 << NBIOAPICCFG_084_Br17_ext_Intr_swz_OFFSET) |
    (0x8 << NBIOAPICCFG_084_Br17_int_Intr_map_OFFSET)
    ),
  //NBIF DEVICE INITIALIZATION
  //PSPCCP enabled on all dies
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF0_PSPCCP_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  //NTBCCP disaabled
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF1_NTBCCP_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  //NTBCCP enabled on all SP3 dies
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_PACKAGE_SP3,
    TYPE_SMN,
    NBIF1_NTBCCP_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (1 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  // Disable optional devices - SATA, XGBE, HDAudio
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF1_HDAUDIO_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF1_XGBE0_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF1_XGBE1_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF1_XGBE2_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF1_XGBE3_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  //NTB disabled on all dies
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF0_NTB_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIF2_NTB_STRAP0_ADDRESS,
    NBIFSTRAP0_FUNC_EN_DEV0_F0_MASK,
    (0 << NBIFSTRAP0_FUNC_EN_DEV0_F0_OFFSET)
    ),
  // Enable interrupts for USB and PSPCCP, all others disabled
  GNB_ENTRY_WR (
    NBIFMISC0_INTR_LINE_POLARITY_TYPE,
    NBIFMISC0_INTR_LINE_POLARITY_ADDRESS,
    0x00000000
    ),
  GNB_ENTRY_WR (
    NBIFMISC0_INTR_LINE_ENABLE_TYPE,
    NBIFMISC0_INTR_LINE_ENABLE_ADDRESS,
    0x0000000C
    ),
  GNB_ENTRY_WR (
    NBIFMISC1_INTR_LINE_POLARITY_TYPE,
    NBIFMISC1_INTR_LINE_POLARITY_ADDRESS,
    0x00000000
    ),
  GNB_ENTRY_WR (
    NBIFMISC1_INTR_LINE_ENABLE_TYPE,
    NBIFMISC1_INTR_LINE_ENABLE_ADDRESS,
    0x00000000
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    SMN_0x13B10030,
    SMN_0x13B10030_DlDownResetEn_MASK,
    (0x0 << SMN_0x13B10030_DlDownResetEn_OFFSET)
    ),
  //GMI_WRR_WEIGHT
  GNB_ENTRY_RMW (
    TYPE_SMN,
    SMN_0x10123990,
    SMN_GMI_REQ_REALTIME_WEIGHT_MASK |
    SMN_GMI_REQ_NORM_P_WEIGHT_MASK |
    SMN_GMI_REQ_NORM_NP_WEIGHT_MASK,
    (0x8 << SMN_GMI_REQ_REALTIME_WEIGHT_OFFSET) |
    (0x8 << SMN_GMI_REQ_NORM_P_WEIGHT_OFFSET) |
    (0x8 << SMN_GMI_REQ_NORM_NP_WEIGHT_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    SMN_0x10223990,
    SMN_GMI_REQ_REALTIME_WEIGHT_MASK |
    SMN_GMI_REQ_NORM_P_WEIGHT_MASK |
    SMN_GMI_REQ_NORM_NP_WEIGHT_MASK,
    (0x8 << SMN_GMI_REQ_REALTIME_WEIGHT_OFFSET) |
    (0x8 << SMN_GMI_REQ_NORM_P_WEIGHT_OFFSET) |
    (0x8 << SMN_GMI_REQ_NORM_NP_WEIGHT_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    SMN_0x10323990,
    SMN_GMI_REQ_REALTIME_WEIGHT_MASK |
    SMN_GMI_REQ_NORM_P_WEIGHT_MASK |
    SMN_GMI_REQ_NORM_NP_WEIGHT_MASK,
    (0x8 << SMN_GMI_REQ_REALTIME_WEIGHT_OFFSET) |
    (0x8 << SMN_GMI_REQ_NORM_P_WEIGHT_OFFSET) |
    (0x8 << SMN_GMI_REQ_NORM_NP_WEIGHT_OFFSET)
    ),
  //NTBCCP enabled on SP3
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_PACKAGE_SP3,
    NBIFMISC1_INTR_LINE_ENABLE_TYPE,
    NBIFMISC1_INTR_LINE_ENABLE_ADDRESS,
    (1 << 1),
    (1 << 1)
    ),
  GNB_ENTRY_WR (
    NBIFMISC2_INTR_LINE_POLARITY_TYPE,
    NBIFMISC2_INTR_LINE_POLARITY_ADDRESS,
    0x00000000
    ),
  GNB_ENTRY_WR (
    NBIFMISC2_INTR_LINE_ENABLE_TYPE,
    NBIFMISC2_INTR_LINE_ENABLE_ADDRESS,
    0x00000000
    ),
  // NBIFMISC[0-2] SATA RO overide
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIFMISC0_00000024,
    NBIFMISC0_00000024_TX_RO_OVERIDE_P_F2_MASK,
    (1 << NBIFMISC0_00000024_TX_RO_OVERIDE_P_F2_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIFMISC1_00000024,
    NBIFMISC0_00000024_TX_RO_OVERIDE_P_F2_MASK,
    (1 << NBIFMISC0_00000024_TX_RO_OVERIDE_P_F2_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    NBIFMISC2_00000024,
    NBIFMISC0_00000024_TX_RO_OVERIDE_P_F2_MASK,
    (1 << NBIFMISC0_00000024_TX_RO_OVERIDE_P_F2_OFFSET)
    ),
  // To enable dynamic clock gating:. IOHC::IOHC_GLUE_CG_LCLK_CTRL_0[SOFT_OVERRIDE_CLK9~0]=0h
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOHC_Clock_GATING,
    IOHC_GLUE_CG_LCLK_CTRL_0_TYPE,
    IOHC_GLUE_CG_LCLK_CTRL_0_ADDRESS,
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
    IOHC_GLUE_CG_LCLK_CTRL_1_ADDRESS,
//    NBMISC_008C_SOFT_OVERRIDE_CLK9_MASK |
//    NBMISC_008C_SOFT_OVERRIDE_CLK8_MASK |
//    NBMISC_008C_SOFT_OVERRIDE_CLK7_MASK |
//    NBMISC_008C_SOFT_OVERRIDE_CLK6_MASK |
//    NBMISC_008C_SOFT_OVERRIDE_CLK5_MASK |
//    NBMISC_008C_SOFT_OVERRIDE_CLK4_MASK |
//    NBMISC_008C_SOFT_OVERRIDE_CLK3_MASK |
//    NBMISC_008C_SOFT_OVERRIDE_CLK2_MASK |
    NBMISC_008C_SOFT_OVERRIDE_CLK1_MASK |
    NBMISC_008C_SOFT_OVERRIDE_CLK0_MASK,
//    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK9_OFFSET) |
//    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK8_OFFSET) |
//    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK7_OFFSET) |
//    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK6_OFFSET) |
//    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK5_OFFSET) |
//    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK4_OFFSET) |
//    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK3_OFFSET) |
//    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK2_OFFSET) |
    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK1_OFFSET) |
    (0x0 << NBMISC_008C_SOFT_OVERRIDE_CLK0_OFFSET)
    ),

  GNB_ENTRY_RMW (
    SION_WRAPPER_404_SST0_TYPE,
    SION_WRAPPER_404_SST0_ADDRESS,
    SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_MASK,
    (0x1 << SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SION_WRAPPER_404_SST1_TYPE,
    SION_WRAPPER_404_SST1_ADDRESS,
    SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_MASK,
    (0x1 << SION_WRAPPER_404_CFG_SSTSION_GLUE_CG_LCLK_CTRL_SOFT_OVERRIDE_CLK4_OFFSET)
    ),

  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_SST_CLOCK_GATING_DISABLED,
    TYPE_SST0,
    SST_CLOCK_CTRL_SST0_ADDRESS,
    SST_CLOCK_CTRL_RXCLKGATEEn_MASK |
    SST_CLOCK_CTRL_TXCLKGATEEn_MASK,
    (0x0 << SST_CLOCK_CTRL_RXCLKGATEEn_OFFSET) |
    (0x0 << SST_CLOCK_CTRL_TXCLKGATEEn_OFFSET)
    ),

  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_SST_CLOCK_GATING_DISABLED,
    TYPE_SST1,
    SST_CLOCK_CTRL_SST1_ADDRESS,
    SST_CLOCK_CTRL_RXCLKGATEEn_MASK |
    SST_CLOCK_CTRL_TXCLKGATEEn_MASK,
    (0x0 << SST_CLOCK_CTRL_RXCLKGATEEn_OFFSET) |
    (0x0 << SST_CLOCK_CTRL_TXCLKGATEEn_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SST0,
    SST_CLOCK_CTRL_SST0_ADDRESS,
    SST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK,
    (0xF0 << SST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SST1,
    SST_CLOCK_CTRL_SST1_ADDRESS,
    SST_CLOCK_CTRL_PCTRL_IDLE_TIME_MASK,
    (0xF0 << SST_CLOCK_CTRL_PCTRL_IDLE_TIME_OFFSET)
    ),

  GNB_ENTRY_RMW (
    SMN_0x13B3C004_TYPE,
    SMN_0x13B3C004_ADDRESS,
    SMN_0x13B3C004_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B3C004_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B38004_TYPE,
    SMN_0x13B38004_ADDRESS,
    SMN_0x13B38004_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B38004_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B39004_TYPE,
    SMN_0x13B39004_ADDRESS,
    SMN_0x13B39004_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B39004_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B31004_TYPE,
    SMN_0x13B31004_ADDRESS,
    SMN_0x13B31004_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B31004_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B31404_TYPE,
    SMN_0x13B31404_ADDRESS,
    SMN_0x13B31404_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B31404_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B31804_TYPE,
    SMN_0x13B31804_ADDRESS,
    SMN_0x13B31804_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B31804_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B31C04_TYPE,
    SMN_0x13B31C04_ADDRESS,
    SMN_0x13B31C04_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B31C04_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B32004_TYPE,
    SMN_0x13B32004_ADDRESS,
    SMN_0x13B32004_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B32004_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B32404_TYPE,
    SMN_0x13B32404_ADDRESS,
    SMN_0x13B32404_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B32404_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B32804_TYPE,
    SMN_0x13B32804_ADDRESS,
    SMN_0x13B32804_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B32804_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B32C04_TYPE,
    SMN_0x13B32C04_ADDRESS,
    SMN_0x13B32C04_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B32C04_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B33004_TYPE,
    SMN_0x13B33004_ADDRESS,
    SMN_0x13B33004_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B33004_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B33404_TYPE,
    SMN_0x13B33404_ADDRESS,
    SMN_0x13B33404_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B33404_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B33804_TYPE,
    SMN_0x13B33804_ADDRESS,
    SMN_0x13B33804_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B33804_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B33C04_TYPE,
    SMN_0x13B33C04_ADDRESS,
    SMN_0x13B33C04_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B33C04_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B34004_TYPE,
    SMN_0x13B34004_ADDRESS,
    SMN_0x13B34004_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B34004_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B34404_TYPE,
    SMN_0x13B34404_ADDRESS,
    SMN_0x13B34404_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B34404_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B34804_TYPE,
    SMN_0x13B34804_ADDRESS,
    SMN_0x13B34804_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B34804_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x13B34C04_TYPE,
    SMN_0x13B34C04_ADDRESS,
    SMN_0x13B34C04_NoSnoopDis_MASK,
    (0x1 << SMN_0x13B34C04_NoSnoopDis_OFFSET)
    ),
  GNB_ENTRY_WR (
    TYPE_SMN,
    0x4A3C8,
    0xFFFE006F
    ),
  GNB_ENTRY_WR (
    TYPE_SMN,
    0x4A3CC,
    0x3
    ),
  GNB_ENTRY_WR (
    TYPE_SMN,
    0x1128018C,
    0x3
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    0x1128018C,
    0x3,
    0x3
    ),
  // To Enable/Disable NBIO Poison Consumption
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_POISON_ACTION_CONTROL,
    POISON_ACTION_CONTROL_TYPE,
    POISON_ACTION_CONTROL_ADDRESS,
    NBRASCFG_0400_IntPoisonAPMLErrEn_MASK |
    NBRASCFG_0400_IntPoisonLinkDisEn_MASK |
    NBRASCFG_0400_IntPoisonSyncFloodEn_MASK |
    NBRASCFG_0400_EgressPoisonLSAPMLErrEn_MASK |
    NBRASCFG_0400_EgressPoisonLSLinkDisEn_MASK |
    NBRASCFG_0400_EgressPoisonLSSyncFloodEn_MASK |
    NBRASCFG_0400_EgressPoisonHSAPMLErrEn_MASK |
    NBRASCFG_0400_EgressPoisonHSLinkDisEn_MASK |
    NBRASCFG_0400_EgressPoisonHSSyncFloodEn_MASK,
    (0x1 << NBRASCFG_0400_IntPoisonAPMLErrEn_OFFSET) |
    (0x1 << NBRASCFG_0400_IntPoisonLinkDisEn_OFFSET) |
    (0x1 << NBRASCFG_0400_IntPoisonSyncFloodEn_OFFSET) |
    (0x1 << NBRASCFG_0400_EgressPoisonLSAPMLErrEn_OFFSET) |
    (0x1 << NBRASCFG_0400_EgressPoisonLSLinkDisEn_OFFSET) |
    (0x1 << NBRASCFG_0400_EgressPoisonLSSyncFloodEn_OFFSET) |
    (0x1 << NBRASCFG_0400_EgressPoisonHSAPMLErrEn_OFFSET) |
    (0x1 << NBRASCFG_0400_EgressPoisonHSLinkDisEn_OFFSET) |
    (0x1 << NBRASCFG_0400_EgressPoisonHSSyncFloodEn_OFFSET)
    ),
  // set SyncFlood_En/LinkDis_En
  GNB_ENTRY_RMW (
    ErrEvent_ACTION_CONTROL_TYPE,
    ErrEvent_ACTION_CONTROL_ADDRESS,
    ErrEvent_ACTION_CONTROL_LinkDis_En_MASK |
    ErrEvent_ACTION_CONTROL_SyncFlood_En_MASK,
    (0x1 << ErrEvent_ACTION_CONTROL_LinkDis_En_OFFSET) |
    (0x1 << ErrEvent_ACTION_CONTROL_SyncFlood_En_OFFSET)
    ),
  // Set NBIF multi-function enable
  GNB_ENTRY_RMW (
    SMN_0x101235CC_TYPE,
    SMN_0x101235CC_ADDRESS,
    SMN_0x101235CC_STRAP_MULTI_FUNC_EN_MASK,
    (0x1 << SMN_0x101235CC_STRAP_MULTI_FUNC_EN_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x102235CC_TYPE,
    SMN_0x102235CC_ADDRESS,
    SMN_0x102235CC_STRAP_MULTI_FUNC_EN_MASK,
    (0x1 << SMN_0x102235CC_STRAP_MULTI_FUNC_EN_OFFSET)
    ),
  GNB_ENTRY_RMW (
    SMN_0x103235CC_TYPE,
    SMN_0x103235CC_ADDRESS,
    SMN_0x103235CC_STRAP_MULTI_FUNC_EN_MASK,
    (0x1 << SMN_0x103235CC_STRAP_MULTI_FUNC_EN_OFFSET)
    ),
  GNB_ENTRY_RMW (
    RCC_BIF_STRAP1_TYPE,
    RCC_BIF_STRAP1_ADDRESS,
    RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_MASK,
    (0x1 << RCC_BIF_STRAP1_STRAP_TRUE_PM_STATUS_EN_OFFSET)
    ),
  // MGCG enable
  GNB_ENTRY_RMW (
    MGCG_CTRL_TYPE,
    NBIF0_MGCG_CTRL_ADDRESS,
    MGCG_EN_MASK,
    (0x1 << MGCG_EN_OFFSET)
    ),
  GNB_ENTRY_RMW (
    MGCG_CTRL_TYPE,
    NBIF1_MGCG_CTRL_ADDRESS,
    MGCG_EN_MASK,
    (0x1 << MGCG_EN_OFFSET)
    ),
  GNB_ENTRY_RMW (
    MGCG_CTRL_TYPE,
    NBIF2_MGCG_CTRL_ADDRESS,
    MGCG_EN_MASK,
    (0x1 << MGCG_EN_OFFSET)
    ),
  GNB_ENTRY_RMW (
    MGCG_CTRL_TYPE,
    SYSHUBMM0_NGDC_MGCG_CTRL_ADDRESS,
    MGCG_EN_MASK,
    (0x1 << MGCG_EN_OFFSET)
    ),
  GNB_ENTRY_RMW (
    MGCG_CTRL_TYPE,
    SYSHUBMM1_NGDC_MGCG_CTRL_ADDRESS,
    MGCG_EN_MASK,
    (0x1 << MGCG_EN_OFFSET)
    ),
// Nbif B/D/F
  GNB_ENTRY_RMW (
    RCC_BIF_STRAP0_TYPE,
    RCC_NBIF0_STRAP0_ADDRESS,
    RCC_BIF_STRAP0_DN_DEVNUM_MASK |
    RCC_BIF_STRAP0_DN_FUNCID_MASK,
    (0x7 << RCC_BIF_STRAP0_DN_DEVNUM_OFFSET) |
    (0x1 << RCC_BIF_STRAP0_DN_FUNCID_OFFSET)
    ),
  GNB_ENTRY_RMW (
    RCC_BIF_STRAP0_TYPE,
    RCC_NBIF1_STRAP0_ADDRESS,
    RCC_BIF_STRAP0_DN_DEVNUM_MASK |
    RCC_BIF_STRAP0_DN_FUNCID_MASK,
    (0x8 << RCC_BIF_STRAP0_DN_DEVNUM_OFFSET) |
    (0x1 << RCC_BIF_STRAP0_DN_FUNCID_OFFSET)
    ),
  //GNB_ENTRY_RMW (
  //  RCC_BIF_STRAP0_TYPE,
  //  RCC_NBIF2_STRAP0_ADDRESS,
  //  RCC_BIF_STRAP0_DN_DEVNUM_MASK |
  //  RCC_BIF_STRAP0_DN_FUNCID_MASK,
  //  (0x9 << RCC_BIF_STRAP0_DN_DEVNUM_OFFSET) |
  //  (0x1 << RCC_BIF_STRAP0_DN_FUNCID_OFFSET)
  //  ),

  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GnbEnvInitTableZP [] = {

  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GnbMidInitTableZP [] = {

  GNB_ENTRY_TERMINATE
};
