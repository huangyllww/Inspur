/**
 * @file
 *
 * IOMMU init tables
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
// #include  <GnbHsio.h>
// #include  <Library/GnbCommonLib.h>
#include  <Library/NbioTable.h>
#include  <GnbRegisters.h>
// #include  <Filecode.h>

GNB_TABLE ROMDATA  IommuInitSequenceHyGx[] = {
  // Disable INTx
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOMMU_CONTROL_W_ADDRESS_HYGX,
    IOMMU_CONTROL_W_INTERRUPT_PIN_W_MASK,
    0x0 << IOMMU_CONTROL_W_INTERRUPT_PIN_W_OFFSET
    ),
  // IOMMU::L2::IOMMU_CAP_MISC[IOMMU_HT_ATS_RESV] = 0h
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOMMU_CAP_MISC_ADDRESS_HYGX,
    IOMMU_CAP_MISC_IOMMU_HT_ATS_RESV_MASK,
    (0 << IOMMU_CAP_MISC_IOMMU_HT_ATS_RESV_OFFSET)
    ),
  // IOMMUL2::IOMMU_CAP_BASE_LO[IOMMU_ENABLE] = 1h
  // no mmio space reserved in PEI, so mask the following codes
  // GNB_ENTRY_RMW (
  // TYPE_SMN,
  // IOMMU_CAP_BASE_LO_ADDRESS,
  // IOMMU_CAP_BASE_LO_IOMMU_ENABLE_MASK,
  // (1 << IOMMU_CAP_BASE_LO_IOMMU_ENABLE_OFFSET)
  // ),

  /* Program the location of the SB into IOMMUL2::L2_SB_LOCATION. The program
     value is required to match the value programmed in IOHC::SB_LOCATION    */
  GNB_ENTRY_COPY (
    TYPE_SMN,
    L2_SB_LOCATION_ADDRESS_HYGX,
    0,
    32,
    SB_LOCATION_TYPE,
    SB_LOCATION_ADDRESS_HYGX,
    0,
    32
    ),

  /* Program the location of the SB into IOMMUL1::L1_SB_LOCATION for the IOMMU L1
   * corresponding to the iFCH or the PCIe core which FCH is located. The program
   * value is required to match the value programmed in IOHC::SB_LOCATION. Leave
   * register at default value for all IOMMU L1s corresponding to other PCIe cores.
   */
  GNB_ENTRY_COPY (
    TYPE_SMN,
    L1_SB_LOCATION_IOAGR_ADDRESS_HYGX,
    0,
    32,
    SB_LOCATION_TYPE,
    SB_LOCATION_ADDRESS_HYGX,
    0,
    32
    ),

  /*
   *If all ports on a PCIe core are disabled, program IOMMUL2::L2_CP_CONTROL_1[CPL1Off] = 1h
   *for the IOMMU L1 corresponding to the PCIE core.
   */
  // TO BE CHECKED

  // Program IOMMUL2::IOMMU_MMIO_CONTROL1_W[MARCnum_SUP_W] = 0h
  GNB_ENTRY_RMW (
    TYPE_SMN,
    IOMMU_MMIO_CONTROL1_W_ADDRESS_HYGX,
    IOMMU_MMIO_CONTROL1_W_MARCnum_SUP_W_MASK,
    (0 << IOMMU_MMIO_CONTROL1_W_MARCnum_SUP_W_OFFSET)
    ),

  // IOMMU L1 Initialization
  // 1. Program IOMMUL1::L1_CNTRL_4[ATS_multiple_resp_en] = 1h
  // 2. Program IOMMUL1::L1_CNTRL_4[Timeout_pulse_ext_En] = 1h
  // PCIE0
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_CNTRL_4_PCIE0_ADDRESS_HYGX,
    L1_CNTRL_4_ATS_multiple_resp_en_MASK |
    L1_CNTRL_4_Timeout_pulse_ext_En_MASK,
    (1 << L1_CNTRL_4_ATS_multiple_resp_en_OFFSET) |
    (0x1 << L1_CNTRL_4_Timeout_pulse_ext_En_OFFSET)
    ),
  //NBIF0
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_CNTRL_4_NBIF0_ADDRESS_HYGX,
    L1_CNTRL_4_ATS_multiple_resp_en_MASK |
    L1_CNTRL_4_Timeout_pulse_ext_En_MASK,
    (1 << L1_CNTRL_4_ATS_multiple_resp_en_OFFSET) |
    (0x1 << L1_CNTRL_4_Timeout_pulse_ext_En_OFFSET)
    ),
  // IOAGR
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_CNTRL_4_IOAGR_ADDRESS_HYGX,
    L1_CNTRL_4_ATS_multiple_resp_en_MASK |
    L1_CNTRL_4_Timeout_pulse_ext_En_MASK,
    (1 << L1_CNTRL_4_ATS_multiple_resp_en_OFFSET) |
    (0x1 << L1_CNTRL_4_Timeout_pulse_ext_En_OFFSET)
    ),
    // This reg is list in satori iommu spec
    // IOMMUL1::L1_MISC_CNTRL_1[REG_enable_nw] = 1h.
    // IOMMUL1::L1_MISC_CNTRL_1[L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_OFFSET] = 1h.
    // IOMMUL1::L1_MISC_CNTRL_1[REG_Block_On_4K_wait_dis] = 1h.
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_MISC_CNTRL_1_PCIE0_ADDRESS_HYGX,
    L1_MISC_CNTRL_1_REG_enable_nw_MASK |
    L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_MASK |
    L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_MASK,
    (0x1 << L1_MISC_CNTRL_1_REG_enable_nw_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_OFFSET)
    ),

  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_MISC_CNTRL_1_PCIE1_ADDRESS_HYGX,
    L1_MISC_CNTRL_1_REG_enable_nw_MASK |
    L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_MASK |
    L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_MASK,
    (0x1 << L1_MISC_CNTRL_1_REG_enable_nw_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_MISC_CNTRL_1_NBIF0_ADDRESS_HYGX,
    L1_MISC_CNTRL_1_REG_enable_nw_MASK |
    L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_MASK |
    L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_MASK,
    (0x1 << L1_MISC_CNTRL_1_REG_enable_nw_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_MISC_CNTRL_1_IOAGR_ADDRESS_HYGX,
    L1_MISC_CNTRL_1_REG_enable_nw_MASK |
    L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_MASK |
    L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_MASK,
    (0x1 << L1_MISC_CNTRL_1_REG_enable_nw_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_disble_eco_invaldation_0_OFFSET) |
    (0x1 << L1_MISC_CNTRL_1_REG_Block_On_4K_wait_dis_OFFSET)
    ),
  // 3. Program IOMMUL1::L1_FEATURE_CNTRL[EXE_lock_bit] = 1h
  // 4. Program IOMMUL1::L1_FEATURE_CNTRL[PMR_lock_bit] = 1h
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_FEATURE_CNTRL_PCIE0_ADDRESS_HYGX,
    L1_FEATURE_CNTRL_EXE_lock_bit_MASK |
    L1_FEATURE_CNTRL_PMR_lock_bit_MASK,
    1 << L1_FEATURE_CNTRL_EXE_lock_bit_OFFSET |
    1 << L1_FEATURE_CNTRL_PMR_lock_bit_OFFSET
    ),

  /*
 GNB_ENTRY_RMW (
   TYPE_SMN,
   L1_FEATURE_CNTRL_PCIE1_ADDRESS_HYGX,
   L1_FEATURE_CNTRL_EXE_lock_bit_MASK |
   L1_FEATURE_CNTRL_PMR_lock_bit_MASK,
   1 << L1_FEATURE_CNTRL_EXE_lock_bit_OFFSET |
   1 << L1_FEATURE_CNTRL_PMR_lock_bit_OFFSET
   ),
   */
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_FEATURE_CNTRL_NBIF0_ADDRESS_HYGX,
    L1_FEATURE_CNTRL_EXE_lock_bit_MASK |
    L1_FEATURE_CNTRL_PMR_lock_bit_MASK,
    1 << L1_FEATURE_CNTRL_EXE_lock_bit_OFFSET |
    1 << L1_FEATURE_CNTRL_PMR_lock_bit_OFFSET
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_FEATURE_CNTRL_IOAGR_ADDRESS_HYGX,
    L1_FEATURE_CNTRL_EXE_lock_bit_MASK |
    L1_FEATURE_CNTRL_PMR_lock_bit_MASK,
    1 << L1_FEATURE_CNTRL_EXE_lock_bit_OFFSET |
    1 << L1_FEATURE_CNTRL_PMR_lock_bit_OFFSET
    ),
  // 5.   Program IOMMUL1::L1_CNTRL_0[Unfilter_dis] = 1h
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_CNTRL_0_PCIE0_ADDRESS_HYGX,
    L1_CNTRL_0_Unfilter_dis_MASK,
    1 << L1_CNTRL_0_Unfilter_dis_OFFSET
    ),

  /*
GNB_ENTRY_RMW (
  TYPE_SMN,
  L1_CNTRL_0_PCIE1_ADDRESS_HYGX,
  L1_CNTRL_0_Unfilter_dis_MASK,
  1 << L1_CNTRL_0_Unfilter_dis_OFFSET
  ),
  */
GNB_ENTRY_RMW (
  TYPE_SMN,
  L1_CNTRL_0_NBIF0_ADDRESS_HYGX,
  L1_CNTRL_0_Unfilter_dis_MASK,
  1 << L1_CNTRL_0_Unfilter_dis_OFFSET
  ),

  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_CNTRL_0_IOAGR_ADDRESS_HYGX,
    L1_CNTRL_0_Unfilter_dis_MASK,
    1 << L1_CNTRL_0_Unfilter_dis_OFFSET
    ),

  /*
   * 6.    Program IOMMUL1::L1_CNTRL_2[CPD_RESP_MODE],
   * IOMMUL1::L1_CNTRL_2[L1NonConsumedDataErrorSignalEn]
   * and IOMMUL1::L1_CNTRL_2[L1ConsumedDataErrorSignalEn]
   * based on the data parity error handling in the system.
   */
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_CNTRL_2_PCIE0_ADDRESS_HYGX,
    L1_CNTRL_2_CPD_RESP_MODE_MASK |
    L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_MASK |
    L1_CNTRL_2_L1ConsumedDataErrorSignalEn_MASK,
    1 << L1_CNTRL_2_CPD_RESP_MODE_OFFSET |
    1 << L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_OFFSET |
    1 << L1_CNTRL_2_L1ConsumedDataErrorSignalEn_OFFSET
    ),

  /*
GNB_ENTRY_RMW (
  TYPE_SMN,
  L1_CNTRL_2_PCIE1_ADDRESS_HYGX,
  L1_CNTRL_2_CPD_RESP_MODE_MASK |
  L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_MASK |
  L1_CNTRL_2_L1ConsumedDataErrorSignalEn_MASK,
  1 << L1_CNTRL_2_CPD_RESP_MODE_OFFSET |
  1 << L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_OFFSET |
  1 << L1_CNTRL_2_L1ConsumedDataErrorSignalEn_OFFSET
  ),
  */
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_CNTRL_2_NBIF0_ADDRESS_HYGX,
    L1_CNTRL_2_CPD_RESP_MODE_MASK |
    L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_MASK |
    L1_CNTRL_2_L1ConsumedDataErrorSignalEn_MASK,
    1 << L1_CNTRL_2_CPD_RESP_MODE_OFFSET |
    1 << L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_OFFSET |
    1 << L1_CNTRL_2_L1ConsumedDataErrorSignalEn_OFFSET
    ),
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L1_CNTRL_2_IOAGR_ADDRESS_HYGX,
    L1_CNTRL_2_CPD_RESP_MODE_MASK |
    L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_MASK |
    L1_CNTRL_2_L1ConsumedDataErrorSignalEn_MASK,
    1 << L1_CNTRL_2_CPD_RESP_MODE_OFFSET |
    1 << L1_CNTRL_2_L1NonConsumedDataErrorSignalEn_OFFSET |
    1 << L1_CNTRL_2_L1ConsumedDataErrorSignalEn_OFFSET
    ),

  // 1.1.2.3    IOMMU L2 Initialization
  // BIOS should program the following registers for IOMMU L2:
  // 1. Program IOMMUL2::L2_ERR_RULE_CONTROL_0[ERRRuleLock0] = 1h
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2_ERR_RULE_CONTROL_0_ADDRESS_HYGX,
    L2_ERR_RULE_CONTROL_0_ERRRuleLock0_MASK,
    1 << L2_ERR_RULE_CONTROL_0_ERRRuleLock0_OFFSET
    ),
  // 2. Program IOMMUL2::L2_ERR_RULE_CONTROL_3[ERRRuleLock1] = 1h
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2_ERR_RULE_CONTROL_3_ADDRESS_HYGX,
    L2_ERR_RULE_CONTROL_3_ERRRuleLock1_MASK,
    1 << L2_ERR_RULE_CONTROL_3_ERRRuleLock1_OFFSET
    ),
  // 3. Program IOMMUL2::L2_L2A_PGSIZE_CONTROL[L2AREG_HOST_PGSIZE] = 49h
  // 4. Program IOMMUL2::L2_L2A_PGSIZE_CONTROL[L2AREG_GST_PGSIZE] = 49h.
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2_L2A_PGSIZE_CONTROL_ADDRESS_HYGX,
    L2_L2A_PGSIZE_CONTROL_L2AREG_HOST_PGSIZE_MASK |
    L2_L2A_PGSIZE_CONTROL_L2AREG_GST_PGSIZE_OFFSET,
    (0x49 << L2_L2A_PGSIZE_CONTROL_L2AREG_HOST_PGSIZE_OFFSET) |
    (0x49 << L2_L2A_PGSIZE_CONTROL_L2AREG_GST_PGSIZE_OFFSET)
    ),
  // IOMMUL2::L2_L2B_PGSIZE_CONTROL[L2BREG_HOST_PGSIZE] = 49h.
  // IOMMUL2::L2_L2B_PGSIZE_CONTROL[L2BREG_GST_PGSIZE] = 49h.
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2_L2B_PGSIZE_CONTROL_ADDRESS_HYGX,
    L2_L2B_PGSIZE_CONTROL_L2BREG_GST_PGSIZE_MASK |
    L2_L2B_PGSIZE_CONTROL_L2BREG_HOST_PGSIZE_MASK,
    (0x49 << L2_L2B_PGSIZE_CONTROL_L2BREG_GST_PGSIZE_OFFSET) |
    (0x49 << L2_L2B_PGSIZE_CONTROL_L2BREG_HOST_PGSIZE_OFFSET)
    ),
  // 7. Program IOMMUL2::L2_CONTROL_5[GST_partial_ptc_cntrl] = 49h
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2_CONTROL_5_ADDRESS_HYGX,
    L2_CONTROL_5_GST_partial_ptc_cntrl_MASK,
    (0x49 << L2_CONTROL_5_GST_partial_ptc_cntrl_OFFSET)
    ),

  /*
   * 8.  Program IOMMUL2::L2_MISC_CNTRL_3[REG_atomic_filter_en] = 1h.
   * 9.  Program IOMMUL2::L2_MISC_CNTRL_3[REG_mask_l2_nw] = 1h.
   * 10. Program IOMMUL2::L2_MISC_CNTRL_3[REG_ats_iw] = 1h.
   */
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2_DEBUG_3_ADDRESS_HYGX,
    L2_DEBUG_3_REG_atomic_filter_en_MASK |
    L2_DEBUG_3_REG_mask_l2_nw_MASK |
    L2_DEBUG_3_REG_ats_iw_MASK,
    (0x1 << L2_DEBUG_3_REG_atomic_filter_en_OFFSET) |
    (0x1 << L2_DEBUG_3_REG_mask_l2_nw_OFFSET) |
    (0x1 << L2_DEBUG_3_REG_ats_iw_OFFSET)
    ),

  /*
   * 11.   Program IOMMUL2::L2_CP_CONTROL[CPFlushOnWait] = 1h.
   * 12.   Program IOMMUL2::L2_CP_CONTROL[CPFlushOnInv] = 0h.
   */
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2_CP_CONTROL_ADDRESS_HYGX,
    L2_CP_CONTROL_CPFlushOnWait_MASK |
    L2_CP_CONTROL_CPFlushOnInv_MASK,
    (0x1 << L2_CP_CONTROL_CPFlushOnWait_OFFSET) |
    (0x0 << L2_CP_CONTROL_CPFlushOnInv_OFFSET)
    ),
  // 13.    Program IOMMUL2::L2B_UPDATE_FILTER_CNTL[L2b_Update_Filter_Bypass] = 0h
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2B_UPDATE_FILTER_CNTL_ADDRESS_HYGX,
    L2B_UPDATE_FILTER_CNTL_L2b_Update_Filter_Bypass_MASK,
    (0x0 << L2B_UPDATE_FILTER_CNTL_L2b_Update_Filter_Bypass_OFFSET)
    ),
  // 14.    Program IOMMUL2::L2A_UPDATE_FILTER_CNTL[L2a_Update_Filter_Bypass] = 0h
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2A_UPDATE_FILTER_CNTL_ADDRESS_HYGX,
    SMN_15700088_L2a_Update_Filter_Bypass_MASK,
    (0x0 << SMN_15700088_L2a_Update_Filter_Bypass_OFFSET)
    ),

  /*
   * 15.   Program IOMMUL2::L2_TW_CONTROL[TWPrefetchRange] = 7h.
   * 16.   Program IOMMUL2::L2_TW_CONTROL[TWPrefetchEn] = 1h.
   * 17.   Program IOMMUL2::L2_TW_CONTROL[TWGuestPrefetchRange] = 7h.
   * 18.   Program IOMMUL2::L2_TW_CONTROL[TWGuestPrefetchEn] = 1h.
   */
  GNB_ENTRY_RMW (
    TYPE_SMN,
    L2_TW_CONTROL_ADDRESS_HYGX,
    L2_TW_CONTROL_TWPrefetchRange_MASK |
    L2_TW_CONTROL_TWPrefetchEn_MASK |
    L2_TW_CONTROL_TWGuestPrefetchEn_MASK |
    L2_TW_CONTROL_TWGuestPrefetchRange_MASK,
    (0x7 << L2_TW_CONTROL_TWPrefetchRange_OFFSET) |
    (0x1 << L2_TW_CONTROL_TWPrefetchEn_OFFSET) |
    (0x1 << L2_TW_CONTROL_TWGuestPrefetchEn_OFFSET) |
    (0x7 << L2_TW_CONTROL_TWGuestPrefetchRange_OFFSET)
    ),

  // 1.1.2.4   IOMMU Clock Gating

  /*
    1.  Program IOMMUL1::L1_CLKCNTRL_0[L1_CLKGATE_HYSTERESIS] = 20h.
    2.  Program IOMMUL1::L1_CLKCNTRL_0[L1_DMA_CLKGATE_EN] = 1h.
    3.  Program IOMMUL1::L1_CLKCNTRL_0[L1_CACHE_CLKGATE_EN] = 1h.
    4.  Program IOMMUL1::L1_CLKCNTRL_0[L1_CPSLV_CLKGATE_EN] = 0h.
    5.  Program IOMMUL1::L1_CLKCNTRL_0[L1_PERF_CLKGATE_EN] = 1h.
    6.  Program IOMMUL1::L1_CLKCNTRL_0[L1_MEMORY_CLKGATE_EN] = 1h.
    7.  Program IOMMUL1::L1_CLKCNTRL_0[L1_REG_CLKGATE_EN] = 1h.
    8.  Program IOMMUL1::L1_CLKCNTRL_0[L1_HOSTREQ_CLKGATE_EN] = 1h.
    9.  Program IOMMUL1::L1_CLKCNTRL_0[L1_DMARSP_CLKGATE_EN] = 1h.
    10. Program IOMMUL1::L1_CLKCNTRL_0[L1_HOSTRSP_CLKGATE_EN] = 1h.
    11. Program IOMMUL1::L1_CLKCNTRL_0[L1_L2_CLKGATE_EN] = 1h.
  */
  // IOMMU L1 clock gating
  // IOAGR
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    TYPE_SMN,
    L1_CLKCNTRL_0_IOAGR_ADDRESS_HYGX,
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
    TYPE_SMN,
    L1_CLKCNTRL_0_NBIF0_ADDRESS_HYGX,
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
    TYPE_SMN,
    L1_CLKCNTRL_0_PCIE0_ADDRESS_HYGX,
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
  // PCIE1

  /*
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L1_CLOCK_GATING,
    TYPE_SMN,
    L1_CLKCNTRL_0_PCIE1_ADDRESS_HYGX,
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
    */
  /*
   * 12.   Program IOMMUL2::L2_L2A_CK_GATE_CONTROL[CKGateL2ARegsDisable] = 0h.
   * 13.   Program IOMMUL2::L2_L2A_CK_GATE_CONTROL[CKGateL2ADynamicDisable] = 0h.
   * 14.   Program IOMMUL2::L2_L2A_CK_GATE_CONTROL[CKGateL2ACacheDisable] = 0h.
   */
  // IOMMU L2 clock gating
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING,
    TYPE_SMN,
    L2_L2A_CK_GATE_CONTROL_ADDRESS_HYGX,
    IOMMUL2A_0CC_CKGateL2ARegsDisable_MASK | IOMMUL2A_0CC_CKGateL2ADynamicDisable_MASK | IOMMUL2A_0CC_CKGateL2ACacheDisable_MASK,
    (0x0 << IOMMUL2A_0CC_CKGateL2ARegsDisable_OFFSET) |
    (0x0 << IOMMUL2A_0CC_CKGateL2ADynamicDisable_OFFSET) |
    (0x0 << IOMMUL2A_0CC_CKGateL2ACacheDisable_OFFSET)
    ),

  /* 15. Program IOMMUL2::L2_L2B_CK_GATE_CONTROL[CKGateL2BRegsDisable] = 0h.
     16. Program IOMMUL2::L2_L2B_CK_GATE_CONTROL[CKGateL2BDynamicDisable] = 0h.
     17. Program IOMMUL2::L2_L2B_CK_GATE_CONTROL[CKGateL2BMiscDisable] = 0h.
     18. Program IOMMUL2::L2_L2B_CK_GATE_CONTROL[CKGateL2BCacheDisable] = 0h.
   */
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_L2_CLOCK_GATING,
    TYPE_SMN,
    L2_L2B_CK_GATE_CONTROL_ADDRESS_HYGX,
    L2_L2B_CK_GATE_CONTROL_CKGateL2BRegsDisable_MASK | L2_L2B_CK_GATE_CONTROL_CKGateL2BDynamicDisable_MASK |
    L2_L2B_CK_GATE_CONTROL_CKGateL2BMiscDisable_MASK | L2_L2B_CK_GATE_CONTROL_CKGateL2BCacheDisable_MASK,
    (0x0 << L2_L2B_CK_GATE_CONTROL_CKGateL2BRegsDisable_OFFSET) |
    (0x0 << L2_L2B_CK_GATE_CONTROL_CKGateL2BDynamicDisable_OFFSET) |
    (0x0 << L2_L2B_CK_GATE_CONTROL_CKGateL2BMiscDisable_OFFSET) |
    (0x0 << L2_L2B_CK_GATE_CONTROL_CKGateL2BCacheDisable_OFFSET)
    ),
  // Configure IOMMU Power Island
  // Hide IOMMU function if disabled
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_DISABLED,
    TYPE_SMN,
    CFG_IOHC_PCI_ADDRESS_HYGX,
    CFG_IOHC_PCI_CFG_IOHC_PCI_Dev0Fn2RegEn_MASK,
    0x0
    ),

  // Set L1_FEATURE_SUP_CNTRL[L1_XT_SUP_W] to 1 if X2APIC is enabled
  GNB_ENTRY_PROPERTY_RMW (
    TABLE_PROPERTY_IOMMU_X2APIC_SUPPORT,
    TYPE_SMN,
    L1_FEATURE_SUP_CNTRL_ADDRESS_HYGX,
    L1_FEATURE_SUP_CNTRL_L1_XT_SUP_W_MASK,
    (0x1 << L1_FEATURE_SUP_CNTRL_L1_XT_SUP_W_OFFSET)
    ),

  // Table End
  GNB_ENTRY_TERMINATE
};
