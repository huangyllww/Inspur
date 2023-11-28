/**
 * @file
 *
 * IOMMU init tables
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     AGESA
 * @e sub-project: GNB
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-09 12:28:05 -0600 (Tue, 09 Dec 2014) $
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

#define FILECODE        NBIO_NBIOBASE_AMDNBIOBASEZPPEI_GNBIOMMUTABLESZP_FILECODE

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

GNB_TABLE ROMDATA GnbIommuEarlyInitTableZP [] = {
//---------------------------------------------------------------------------

  GNB_ENTRY_TERMINATE
};

GNB_TABLE ROMDATA GnbIommuEnvInitTableZP [] = {
//---------------------------------------------------------------------------
// Set SB location for IOMMU
  GNB_ENTRY_COPY (
    L1_SB_LOCATION_TYPE,
    L1_SB_LOCATION_IOAGR_ADDRESS,
    0,
    32,
    SB_LOCATION_TYPE,
    SB_LOCATION_ADDRESS,
    0,
    32
    ),
  GNB_ENTRY_COPY (
    L2_SB_LOCATION_TYPE,
    L2_SB_LOCATION_ADDRESS,
    0,
    32,
    SB_LOCATION_TYPE,
    SB_LOCATION_ADDRESS,
    0,
    32
    ),

//---------------------------------------------------------------------------
// Disable INTx in IOMMU
  GNB_ENTRY_RMW (
    IOMMU_CONTROL_W_TYPE,
    IOMMU_CONTROL_W_ADDRESS,
    IOMMU_CONTROL_W_INTERRUPT_PIN_W_MASK,
    0x0 << IOMMU_CONTROL_W_INTERRUPT_PIN_W_OFFSET
    ),

  //---------------------------------------------------------------------------
//programs IOHC::CFG_IOHC_PCI[CFG_IOHC_PCI_Dev0Fn2RegEn] = 1h
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_DISABLED,
    CFG_IOHC_PCI_TYPE,
    CFG_IOHC_PCI_ADDRESS,
    SMN_00000034_CFG_IOHC_PCI_Dev0Fn2RegEn_MASK,
    (0x1 << SMN_00000034_CFG_IOHC_PCI_Dev0Fn2RegEn_OFFSET)
    ),
//Program IOMMU::L2::IOMMU_CAP_MISC[IOMMU_HT_ATS_RESV] = 0h
  GNB_ENTRY_RMW (
    IOMMU_CAP_MISC_TYPE,
    IOMMU_CAP_MISC_ADDRESS,
    SMN_00000050_IOMMU_HT_ATS_RESV_MASK,
    (0 << SMN_00000050_IOMMU_HT_ATS_RESV_OFFSET)
    ),

//---------------------------------------------------------------------------
//IOMMU L1 Initialization
//// IOAGR
  GNB_ENTRY_RMW (
    L1_CNTRL_4_TYPE,
    L1_CNTRL_4_IOAGR_ADDRESS,
    L1_CNTRL_4_ATS_multiple_resp_en_MASK |
    L1_CNTRL_4_Timeout_pulse_ext_En_MASK,
    (0x1 << L1_CNTRL_4_ATS_multiple_resp_en_OFFSET) |
    (0x1 << L1_CNTRL_4_Timeout_pulse_ext_En_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_MISC_CNTRL_1_TYPE,
    L1_MISC_CNTRL_1_IOAGR_ADDRESS,
    L1_MISC_CNTRL_1_REG_enable_nw_MASK |
    L1_MISC_CNTRL_1_REG_force_invalid_filter_MASK |
    L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_MASK,
    (0x1 << L1_MISC_CNTRL_1_REG_enable_nw_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_force_invalid_filter_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_FEATURE_CNTRL_TYPE,
    L1_FEATURE_CNTRL_IOAGR_ADDRESS,
    L1_FEATURE_CNTRL_EXE_lock_bit_MASK |
    L1_FEATURE_CNTRL_PMR_lock_bit_MASK,
    1 << L1_FEATURE_CNTRL_EXE_lock_bit_OFFSET |
    1 << L1_FEATURE_CNTRL_PMR_lock_bit_OFFSET
    ),
  GNB_ENTRY_RMW (
    L1_CNTRL_0_TYPE,
    L1_CNTRL_0_IOAGR_ADDRESS,
    L1_CNTRL_0_Unfilter_dis_MASK,
    (0x1 << L1_CNTRL_0_Unfilter_dis_OFFSET)
    ),
///@todo this should be based on data parity error handling
//  GNB_ENTRY_RMW (
//    L1_CNTRL_2_TYPE,
//    L1_CNTRL_2_IOAGR_ADDRESS,
//    L1_CNTRL_2_CPD_RESP_MODE_MASK |
//    L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_MASK |
//    L1_CNTRL_2_L1ConsumedDataErrorSignalEn_MASK,
//    1 << L1_CNTRL_2_CPD_RESP_MODE_OFFSET |
//    1 << L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_OFFSET |
//    1 << L1_CNTRL_2_L1ConsumedDataErrorSignalEn_OFFSET
//    ),

  //// NBIF
  GNB_ENTRY_RMW (
    L1_CNTRL_4_TYPE,
    L1_CNTRL_4_NBIF0_ADDRESS,
    L1_CNTRL_4_ATS_multiple_resp_en_MASK |
    L1_CNTRL_4_Timeout_pulse_ext_En_MASK,
    (0x1 << L1_CNTRL_4_ATS_multiple_resp_en_OFFSET) |
    (0x1 << L1_CNTRL_4_Timeout_pulse_ext_En_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_MISC_CNTRL_1_TYPE,
    L1_MISC_CNTRL_1_NBIF0_ADDRESS,
    L1_MISC_CNTRL_1_REG_enable_nw_MASK |
    L1_MISC_CNTRL_1_REG_force_invalid_filter_MASK |
    L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_MASK,
    (0x1 << L1_MISC_CNTRL_1_REG_enable_nw_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_force_invalid_filter_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_FEATURE_CNTRL_TYPE,
    L1_FEATURE_CNTRL_NBIF0_ADDRESS,
    L1_FEATURE_CNTRL_EXE_lock_bit_MASK |
    L1_FEATURE_CNTRL_PMR_lock_bit_MASK,
    1 << L1_FEATURE_CNTRL_EXE_lock_bit_OFFSET |
    1 << L1_FEATURE_CNTRL_PMR_lock_bit_OFFSET
    ),
  GNB_ENTRY_RMW (
    L1_CNTRL_0_TYPE,
    L1_CNTRL_0_NBIF0_ADDRESS,
    L1_CNTRL_0_Unfilter_dis_MASK,
    (0x1 << L1_CNTRL_0_Unfilter_dis_OFFSET)
    ),
///@todo this should be based on data parity error handling
//  GNB_ENTRY_RMW (
//    L1_CNTRL_2_TYPE,
//    L1_CNTRL_2_NBIF0_ADDRESS,
//    L1_CNTRL_2_CPD_RESP_MODE_MASK |
//    L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_MASK |
//    L1_CNTRL_2_L1ConsumedDataErrorSignalEn_MASK,
//    1 << L1_CNTRL_2_CPD_RESP_MODE_OFFSET |
//    1 << L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_OFFSET |
//    1 << L1_CNTRL_2_L1ConsumedDataErrorSignalEn_OFFSET
//    ),

  //// PCIE0
  GNB_ENTRY_RMW (
    L1_CNTRL_4_TYPE,
    L1_CNTRL_4_PCIE0_ADDRESS,
    L1_CNTRL_4_ATS_multiple_resp_en_MASK |
    L1_CNTRL_4_Timeout_pulse_ext_En_MASK,
    (0x1 << L1_CNTRL_4_ATS_multiple_resp_en_OFFSET) |
    (0x1 << L1_CNTRL_4_Timeout_pulse_ext_En_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_MISC_CNTRL_1_TYPE,
    L1_MISC_CNTRL_1_PCIE0_ADDRESS,
    L1_MISC_CNTRL_1_REG_enable_nw_MASK |
    L1_MISC_CNTRL_1_REG_force_invalid_filter_MASK |
    L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_MASK,
    (0x1 << L1_MISC_CNTRL_1_REG_enable_nw_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_force_invalid_filter_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_FEATURE_CNTRL_TYPE,
    L1_FEATURE_CNTRL_PCIE0_ADDRESS,
    L1_FEATURE_CNTRL_EXE_lock_bit_MASK |
    L1_FEATURE_CNTRL_PMR_lock_bit_MASK,
    1 << L1_FEATURE_CNTRL_EXE_lock_bit_OFFSET |
    1 << L1_FEATURE_CNTRL_PMR_lock_bit_OFFSET
    ),
  GNB_ENTRY_RMW (
    L1_CNTRL_0_TYPE,
    L1_CNTRL_0_PCIE0_ADDRESS,
    L1_CNTRL_0_Unfilter_dis_MASK,
    (0x1 << L1_CNTRL_0_Unfilter_dis_OFFSET)
    ),
///@todo this should be based on data parity error handling
//  GNB_ENTRY_RMW (
//    L1_CNTRL_2_TYPE,
//    L1_CNTRL_2_PCIE0_ADDRESS,
//    L1_CNTRL_2_CPD_RESP_MODE_MASK |
//    L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_MASK |
//    L1_CNTRL_2_L1ConsumedDataErrorSignalEn_MASK,
//    1 << L1_CNTRL_2_CPD_RESP_MODE_OFFSET |
//    1 << L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_OFFSET |
//    1 << L1_CNTRL_2_L1ConsumedDataErrorSignalEn_OFFSET
//    ),

  //// PCIE1
  GNB_ENTRY_RMW (
    L1_CNTRL_4_TYPE,
    L1_CNTRL_4_PCIE1_ADDRESS,
    L1_CNTRL_4_ATS_multiple_resp_en_MASK |
    L1_CNTRL_4_Timeout_pulse_ext_En_MASK,
    (0x1 << L1_CNTRL_4_ATS_multiple_resp_en_OFFSET) |
    (0x1 << L1_CNTRL_4_Timeout_pulse_ext_En_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_MISC_CNTRL_1_TYPE,
    L1_MISC_CNTRL_1_PCIE1_ADDRESS,
    L1_MISC_CNTRL_1_REG_enable_nw_MASK |
    L1_MISC_CNTRL_1_REG_force_invalid_filter_MASK |
    L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_MASK,
    (0x1 << L1_MISC_CNTRL_1_REG_enable_nw_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_force_invalid_filter_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L1_FEATURE_CNTRL_TYPE,
    L1_FEATURE_CNTRL_PCIE1_ADDRESS,
    L1_FEATURE_CNTRL_EXE_lock_bit_MASK |
    L1_FEATURE_CNTRL_PMR_lock_bit_MASK,
    1 << L1_FEATURE_CNTRL_EXE_lock_bit_OFFSET |
    1 << L1_FEATURE_CNTRL_PMR_lock_bit_OFFSET
    ),
  GNB_ENTRY_RMW (
    L1_CNTRL_0_TYPE,
    L1_CNTRL_0_PCIE1_ADDRESS,
    L1_CNTRL_0_Unfilter_dis_MASK,
    (0x1 << L1_CNTRL_0_Unfilter_dis_OFFSET)
    ),
///@todo this should be based on data parity error handling
//  GNB_ENTRY_RMW (
//    L1_CNTRL_2_TYPE,
//    L1_CNTRL_2_PCIE1_ADDRESS,
//    L1_CNTRL_2_CPD_RESP_MODE_MASK |
//    L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_MASK |
//    L1_CNTRL_2_L1ConsumedDataErrorSignalEn_MASK,
//    1 << L1_CNTRL_2_CPD_RESP_MODE_OFFSET |
//    1 << L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_OFFSET |
//    1 << L1_CNTRL_2_L1ConsumedDataErrorSignalEn_OFFSET
//    ),

  //---------------------------------------------------------------------------
  // IOMMU L2 Initialization
  GNB_ENTRY_RMW (
    L2_ERR_RULE_CONTROL_0_TYPE,
    L2_ERR_RULE_CONTROL_0_ADDRESS,
    SMN_00000200_ERRRuleLock0_MASK,
    0x1 << SMN_00000200_ERRRuleLock0_OFFSET
    ),
  GNB_ENTRY_RMW (
    L2_ERR_RULE_CONTROL_3_TYPE,
    L2_ERR_RULE_CONTROL_3_ADDRESS,
    SMN_157000C0_ERRRuleLock1_MASK,
    0x1 << SMN_157000C0_ERRRuleLock1_OFFSET
    ),
  GNB_ENTRY_RMW (
    L2_L2A_PGSIZE_CONTROL_TYPE,
    L2_L2A_PGSIZE_CONTROL_ADDRESS,
    SMN_157000D0_L2AREG_HOST_PGSIZE_MASK |
    SMN_157000D0_L2AREG_GST_PGSIZE_MASK,
    (0x49 << SMN_157000D0_L2AREG_HOST_PGSIZE_OFFSET) |
    (0x49 << SMN_157000D0_L2AREG_GST_PGSIZE_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_L2B_PGSIZE_CONTROL_TYPE,
    L2_L2B_PGSIZE_CONTROL_ADDRESS,
    SMN_00000250_L2BREG_HOST_PGSIZE_MASK |
    SMN_00000250_L2BREG_GST_PGSIZE_MASK,
    (0x49 << SMN_00000250_L2BREG_HOST_PGSIZE_OFFSET) |
    (0x49 << SMN_00000250_L2BREG_GST_PGSIZE_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_CONTROL_5_TYPE,
    L2_CONTROL_5_ADDRESS,
    SMN_00000130_GST_partial_ptc_cntrl_MASK,
    (0x49 << SMN_00000130_GST_partial_ptc_cntrl_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_DEBUG_3_TYPE,
    L2_DEBUG_3_ADDRESS,
    SMN_0000011C_REG_atomic_filter_en_MASK |
    SMN_0000011C_REG_mask_l2_nw_MASK |
    SMN_0000011C_REG_ats_iw_MASK,
    (0x1 << SMN_0000011C_REG_atomic_filter_en_OFFSET) |
    (0x1 << SMN_0000011C_REG_mask_l2_nw_OFFSET) |
    (0x1 << SMN_0000011C_REG_ats_iw_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_CP_CONTROL_TYPE,
    L2_CP_CONTROL_ADDRESS,
    SMN_00000158_CPFlushOnWait_MASK |
    SMN_00000158_CPFlushOnInv_MASK,
    (0x1 << SMN_00000158_CPFlushOnWait_OFFSET) |
    (0x0 << SMN_00000158_CPFlushOnInv_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2B_UPDATE_FILTER_CNTL_TYPE,
    L2B_UPDATE_FILTER_CNTL_ADDRESS,
    SMN_0000014C_L2b_Update_Filter_Bypass_MASK,
    (0x0 << SMN_0000014C_L2b_Update_Filter_Bypass_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2A_UPDATE_FILTER_CNTL_TYPE,
    L2A_UPDATE_FILTER_CNTL_ADDRESS,
    SMN_15700088_L2a_Update_Filter_Bypass_MASK,
    (0x0 << SMN_15700088_L2a_Update_Filter_Bypass_OFFSET)
    ),
  GNB_ENTRY_RMW (
    L2_TW_CONTROL_TYPE,
    L2_TW_CONTROL_ADDRESS,
    SMN_00000150_TWPrefetchRange_MASK |
    SMN_00000150_TWPrefetchEn_MASK,
    (0x7 << SMN_00000150_TWPrefetchRange_OFFSET) |
    (0x1 << SMN_00000150_TWPrefetchEn_OFFSET)
    ),
//---------------------------------------------------------------------------
// IOMMU Initialization
// IOMMU L2 clock gating
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING,
    L2_L2A_CK_GATE_CONTROL_TYPE,
    L2_L2A_CK_GATE_CONTROL_ADDRESS,
    IOMMUL2A_0CC_CKGateL2ARegsDisable_MASK | IOMMUL2A_0CC_CKGateL2ADynamicDisable_MASK | IOMMUL2A_0CC_CKGateL2ACacheDisable_MASK,
    (0x0 << IOMMUL2A_0CC_CKGateL2ARegsDisable_OFFSET) |
    (0x0 << IOMMUL2A_0CC_CKGateL2ADynamicDisable_OFFSET) |
    (0x0 << IOMMUL2A_0CC_CKGateL2ACacheDisable_OFFSET)
    ),
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING,
    L2_L2B_CK_GATE_CONTROL_TYPE,
    L2_L2B_CK_GATE_CONTROL_ADDRESS,
    SMN_00000240_CKGateL2BRegsDisable_MASK | SMN_00000240_CKGateL2BDynamicDisable_MASK |
    SMN_00000240_CKGateL2BMiscDisable_MASK | SMN_00000240_CKGateL2BCacheDisable_MASK,
    (0x0 << SMN_00000240_CKGateL2BRegsDisable_OFFSET) |
    (0x0 << SMN_00000240_CKGateL2BDynamicDisable_OFFSET) |
    (0x0 << SMN_00000240_CKGateL2BMiscDisable_OFFSET) |
    (0x0 << SMN_00000240_CKGateL2BCacheDisable_OFFSET)
    ),
// IOMMU L1 clock gating
  // IOAGR
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    L1_CLKCNTRL_0_TYPE,
    L1_CLKCNTRL_0_IOAGR_ADDRESS,
    L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK |
    L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK,
    (0x20 << L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET) |
    (0x0 << L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET)
    ),
  // NBIF
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    L1_CLKCNTRL_0_TYPE,
    L1_CLKCNTRL_0_NBIF0_ADDRESS,
    L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK |
    L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK,
    (0x20 << L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET) |
    (0x0 << L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET)
    ),
  // PCIE0
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    L1_CLKCNTRL_0_TYPE,
    L1_CLKCNTRL_0_PCIE0_ADDRESS,
    L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK |
    L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK,
    (0x20 << L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET) |
    (0x0 << L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET)
    ),
  //PCIE1
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    L1_CLKCNTRL_0_TYPE,
    L1_CLKCNTRL_0_PCIE1_ADDRESS,
    L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_MASK |
    L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_MASK | L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_MASK |
    L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_MASK,
    (0x20 << L1_CLKCNTRL_0_L1_CLKGATE_HYSTERESIS_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_DMA_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_CACHE_CLKGATE_EN_OFFSET) |
    (0x0 << L1_CLKCNTRL_0_L1_CPSLV_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_PERF_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_MEMORY_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_REG_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_HOSTREQ_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_DMARSP_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_HOSTRSP_CLKGATE_EN_OFFSET) |
    (0x1 << L1_CLKCNTRL_0_L1_L2_CLKGATE_EN_OFFSET)
    ),
//---------------------------------------------------------------------------
// Configure IOMMU Power Island
// Hide IOMMU function if disabled
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_DISABLED,
    CFG_IOHC_PCI_TYPE,
    CFG_IOHC_PCI_ADDRESS,
    SMN_00000034_CFG_IOHC_PCI_Dev0Fn2RegEn_MASK,
    0x0
    ),
  GNB_ENTRY_TERMINATE
};
