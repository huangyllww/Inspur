/* $NoKeywords:$ */
/*****************************************************************************
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
 ******************************************************************************
 */
#ifndef _CXLREGISTERSTRUCT_H_
#define _CXLREGISTERSTRUCT_H_

#pragma pack(1)
 
typedef union { 
	struct {
		UINT32                    CXL_BACKDOOR : 32;
	} Field;
	UINT32	Value;
} CXL_BACKDOOR_REGISTER;


typedef union { 
	struct {
		UINT32                LCLK_GATE_ENABLE : 1;
		UINT32                PCLK_GATE_ENABLE : 1;
		UINT32                FCLK_GATE_ENABLE : 1;
		UINT32                                 : 27;
		UINT32         CXL_LINKDOWN_PCS_RST_EN : 1;
		UINT32                  MAXPCLK_SEL_EN : 1;
	} Field;
	UINT32	Value;
} CXL_CLK_CNTL_REGISTER;


typedef union { 
	struct {
		UINT32                  RESET_COMPLETE : 1;
		UINT32                                 : 31;
	} Field;
	UINT32	Value;
} CXL_CLK_STATUS_REGISTER;


typedef union { 
	struct {
		UINT32                LC_WAKE_FROM_L23 : 1;
		UINT32                 APP_REQ_EXIT_L1 : 1;
		UINT32                                 : 30;
	} Field;
	UINT32	Value;
} CXL_PM_CNTL_REGISTER;


typedef union { 
	struct {
		UINT32                                 : 32;
	} Field;
	UINT32	Value;
} CXL_PM_STATUS_REGISTER;


typedef union { 
	struct {
		UINT32           APP_DBI_RO_WR_DISABLE : 1;
		UINT32                APP_HOLD_PHY_RST : 1;
		UINT32            APP_CXL_PM_INIT_CMPL : 1;
		UINT32                                 : 1;
		UINT32            APP_RAS_DES_TBA_CTRL : 2;
		UINT32       APP_RAS_DES_SD_HOLD_LTSSM : 1;
		UINT32         LINK_DOWN_CAUSE_RST_DIS : 1;
		UINT32     LC_CAUSE_PRESENCE_DETECT_EN : 1;
		UINT32                  PM_VDM_DROP_EP : 1;
		UINT32                                 : 5;
		UINT32                     SEL_BDF_NUM : 1;
		UINT32                     APP_BDF_NUM : 16;
	} Field;
	UINT32	Value;
} CXL_MISC_CNTL0_REGISTER;


typedef union { 
	struct {
		UINT32                       RESET_PCS : 1;
		UINT32                   CXL_FLUSH_ACK : 1;
		UINT32                CXL_DIAG_CTRLBUS : 3;
		UINT32                                 : 27;
	} Field;
	UINT32	Value;
} CXL_MISC_CNTL1_REGISTER;


typedef union { 
	struct {
		UINT32                                 : 32;
	} Field;
	UINT32	Value;
} CXL_MISC_STATUS0_REGISTER;


typedef union { 
	struct {
		UINT32                                 : 32;
	} Field;
	UINT32	Value;
} CXL_MISC_STATUS1_REGISTER;


typedef union { 
	struct {
		UINT32                APP_LTSSM_ENABLE : 1;
		UINT32                   APP_SRIS_MODE : 1;
		UINT32                                 : 29;
		UINT32                APP_LANE_FLIP_EN : 1;
	} Field;
	UINT32	Value;
} CXL_LC_CNTL_REGISTER;


typedef union { 
	struct {
		UINT32                LC_CURRENT_STATE : 6;
		UINT32                                 : 2;
		UINT32                  LC_PREV_STATE1 : 6;
		UINT32                                 : 2;
		UINT32                  LC_PREV_STATE2 : 6;
		UINT32                                 : 2;
		UINT32                  LC_PREV_STATE3 : 6;
		UINT32                                 : 2;
	} Field;
	UINT32	Value;
} CXL_LC_STATUS0_REGISTER;


typedef union { 
	struct {
		UINT32                  LC_PREV_STATE4 : 6;
		UINT32                                 : 2;
		UINT32                  LC_PREV_STATE5 : 6;
		UINT32                                 : 2;
		UINT32                  LC_PREV_STATE6 : 6;
		UINT32                                 : 2;
		UINT32                  LC_PREV_STATE7 : 6;
		UINT32                                 : 2;
	} Field;
	UINT32	Value;
} CXL_LC_STATUS1_REGISTER;


typedef union { 
	struct {
		UINT32                  LC_PREV_STATE8 : 6;
		UINT32                                 : 2;
		UINT32                  LC_PREV_STATE9 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE10 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE11 : 6;
		UINT32                                 : 2;
	} Field;
	UINT32	Value;
} CXL_LC_STATUS2_REGISTER;


typedef union { 
	struct {
		UINT32                 LC_PREV_STATE12 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE13 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE14 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE15 : 6;
		UINT32                                 : 2;
	} Field;
	UINT32	Value;
} CXL_LC_STATUS3_REGISTER;


typedef union { 
	struct {
		UINT32                 LC_PREV_STATE16 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE17 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE18 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE19 : 6;
		UINT32                                 : 2;
	} Field;
	UINT32	Value;
} CXL_LC_STATUS4_REGISTER;


typedef union { 
	struct {
		UINT32                 LC_PREV_STATE20 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE21 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE22 : 6;
		UINT32                                 : 2;
		UINT32                 LC_PREV_STATE23 : 6;
		UINT32                                 : 2;
	} Field;
	UINT32	Value;
} CXL_LC_STATUS5_REGISTER;


typedef union { 
	struct {
		UINT32                    RDLH_LINK_UP : 1;
		UINT32                    SMLH_LINK_UP : 1;
		UINT32                                 : 30;
	} Field;
	UINT32	Value;
} CXL_LC_STATUS6_REGISTER;


typedef union { 
	struct {
		UINT32                 CI_MSTSPLIT_DIS : 1;
		UINT32       CI_MSTSPLIT_REQ_CHAIN_DIS : 1;
		UINT32          CI_MST_CMPL_DUMMY_DATA : 1;
		UINT32        CI_MST_TAG_BORROWING_DIS : 1;
		UINT32 CI_MSTSDP_CLKGATE_ONESIDED_ENABLE : 1;
		UINT32 CI_MSTSDP_DISCONNECT_RSP_ON_PARTIAL : 1;
		UINT32    CI_MST_MEMR_RD_NONCONT_BE_EN : 1;
		UINT32                                 : 25;
	} Field;
	UINT32	Value;
} CXL_MST_CNTL0_REGISTER;


typedef union { 
	struct {
		UINT32        CI_PRIV_MAX_PAYLOAD_SIZE : 3;
		UINT32        CI_MAX_PAYLOAD_SIZE_MODE : 1;
		UINT32   CI_PRIV_MAX_READ_REQUEST_SIZE : 3;
		UINT32   CI_MAX_READ_REQUEST_SIZE_MODE : 1;
		UINT32                                 : 24;
	} Field;
	UINT32	Value;
} CXL_MST_CNTL1_REGISTER;


typedef union { 
	struct {
		UINT32                TX_MST_MEM_READY : 1;
		UINT32           CI_NO_PENDING_MST_MRD : 1;
		UINT32               CI_MST_WRRSP_IDLE : 1;
		UINT32                  CI_MST_TX_IDLE : 1;
		UINT32              CI_MST_CLKREQ_IDLE : 1;
		UINT32                  TX_P_HDR_EMPTY : 1;
		UINT32                 TX_NP_HDR_EMPTY : 1;
		UINT32                  TX_P_DAT_EMPTY : 1;
		UINT32                 TX_NP_DAT_EMPTY : 1;
		UINT32        CI_P_HDR_NO_FREE_CREDITS : 1;
		UINT32       CI_NP_HDR_NO_FREE_CREDITS : 1;
		UINT32        CI_P_DAT_NO_FREE_CREDITS : 1;
		UINT32       CI_NP_DAT_NO_FREE_CREDITS : 1;
		UINT32                                 : 19;
	} Field;
	UINT32	Value;
} CXL_MST_STATUS0_REGISTER;


typedef union { 
	struct {
		UINT32 CI_SLV_SDP_ERR_DATA_ON_POISONED_DIS : 1;
		UINT32            CI_SLV_SDP_CHAIN_DIS : 1;
		UINT32        CI_SLV_EP_BIT_TO_SDPUSER : 1;
		UINT32             CI_SLV_ORDERING_DIS : 1;
		UINT32               CI_MEM_CACHE_SIZE : 2;
		UINT32                                 : 2;
		UINT32     CI_SLAVE_MEM_WRSIZEDFULL_EN : 1;
		UINT32                                 : 3;
		UINT32  CI_SLV_REQR_FULL_DISCONNECT_EN : 1;
		UINT32  CI_SLV_REQR_PART_DISCONNECT_EN : 1;
		UINT32       CI_SLAVE_TAG_STEALING_DIS : 1;
		UINT32                                 : 1;
		UINT32                     SDP_UNIT_ID : 4;
		UINT32                                 : 4;
		UINT32    CI_PRIV_MAX_CPL_PAYLOAD_SIZE : 3;
		UINT32                                 : 3;
		UINT32    CI_MAX_CPL_PAYLOAD_SIZE_MODE : 2;
	} Field;
	UINT32	Value;
} CXL_SLV_CNTL0_REGISTER;


typedef union { 
	struct {
		UINT32     CI_SLV_RSP_POISONED_UR_MODE : 2;
		UINT32            CI_SLV_CPL_ALLOC_DIS : 1;
		UINT32            CI_SLV_CPL_ALLOC_SOR : 1;
		UINT32                                 : 28;
	} Field;
	UINT32	Value;
} CXL_SLV_CNTL1_REGISTER;


typedef union { 
	struct {
		UINT32                    SLV_REQ_IDLE : 1;
		UINT32                 SLV_REQ_IDLE_RX : 1;
		UINT32                  SLV_S_CPL_IDLE : 1;
		UINT32                  CHIP_SLV_EMPTY : 1;
		UINT32              CI_SLV_CLKREQ_IDLE : 1;
		UINT32              CI_SLV_NPTAG_EMPTY : 1;
		UINT32               CI_SLV_PTAG_EMPTY : 1;
		UINT32              CI_SLV_ARB_BLOCK_P : 1;
		UINT32              CI_SLV_ARB_BLOCK_N : 1;
		UINT32                                 : 23;
	} Field;
	UINT32	Value;
} CXL_SLV_STATUS0_REGISTER;


typedef union { 
	struct {
		UINT32           TX_ARB_ROUND_ROBIN_EN : 1;
		UINT32                TX_ARB_SLV_LIMIT : 5;
		UINT32                TX_ARB_MST_LIMIT : 5;
		UINT32                                 : 1;
		UINT32                   TX_CPL_PASS_P : 1;
		UINT32                    TX_NP_PASS_P : 1;
		UINT32        INNER_CREDIT_CONSUME_DIS : 1;
		UINT32                                 : 17;
	} Field;
	UINT32	Value;
} CXL_TX_CNTL0_REGISTER;


typedef union { 
	struct {
		UINT32                                 : 1;
		UINT32     RX_RCB_MAX_CPL_TIMEOUT_MODE : 1;
		UINT32          RX_RCB_RC_CTO_TO_UR_EN : 1;
		UINT32 RX_RCB_RC_CTO_TO_SC_IN_LINK_DOWN_EN : 1;
		UINT32             HALT_THRESHOLD_TYPE : 1;
		UINT32          HALT_THRESHOLD_PERCENT : 2;
		UINT32     HALT_THRESHOLD_SLV_PH_VALUE : 5;
		UINT32     HALT_THRESHOLD_SLV_PD_VALUE : 5;
		UINT32    HALT_THRESHOLD_SLV_NPH_VALUE : 5;
		UINT32    HALT_THRESHOLD_SLV_NPD_VALUE : 5;
		UINT32            HALT_THRESHOLD_VALUE : 5;
	} Field;
	UINT32	Value;
} CXL_RX_CNTL0_REGISTER;


typedef union { 
	struct {
		UINT32            RX_RCB_UNEXP_CPL_DIS : 1;
		UINT32         RX_RCB_INVALID_SIZE_DIS : 1;
		UINT32           RX_RCB_WRONG_ATTR_DIS : 1;
		UINT32        RX_RCB_WRONG_FUNCNUM_DIS : 1;
		UINT32         RX_RCB_WRONG_PREFIX_DIS : 1;
		UINT32                                 : 27;
	} Field;
	UINT32	Value;
} CXL_RX_CNTL1_REGISTER;


typedef union { 
	struct {
		UINT32     ERR_RECEIVER_OVERFLOW_HDR_P : 1;
		UINT32    ERR_RECEIVER_OVERFLOW_HDR_NP : 1;
		UINT32                                 : 1;
		UINT32     ERR_RECEIVER_OVERFLOW_DAT_P : 1;
		UINT32    ERR_RECEIVER_OVERFLOW_DAT_NP : 1;
		UINT32                                 : 27;
	} Field;
	UINT32	Value;
} CXL_RX_STATUS0_REGISTER;


typedef union { 
	struct {
		UINT32            CFG_TMR_MICROSECONDS : 32;
	} Field;
	UINT32	Value;
} CXL_RSMU_BIOS_TIMER_CMD_REGISTER;


typedef union { 
	struct {
		UINT32               CFG_TMR_CLOCKRATE : 8;
		UINT32                                 : 24;
	} Field;
	UINT32	Value;
} CXL_RSMU_BIOS_TIMER_CNTL_REGISTER;


typedef union { 
	struct {
		UINT32    CFG_TMR_MICROSECONDS_COMPARE : 32;
	} Field;
	UINT32	Value;
} CXL_RSMU_BIOS_TIMER_DEBUG_REGISTER;


typedef union { 
	struct {
		UINT32                                 : 3;
		UINT32 PRESENCE_DETECT_CHANGED_PRIVATE : 1;
		UINT32                                 : 2;
		UINT32   PRESENCE_DETECT_STATE_PRIVATE : 1;
		UINT32                                 : 25;
	} Field;
	UINT32	Value;
} CXL_PCIEP_HPGI_PRIVATE_REGISTER;


typedef union { 
	struct {
		UINT32       REG_HPGI_ASSERT_TO_SMI_EN : 1;
		UINT32       REG_HPGI_ASSERT_TO_SCI_EN : 1;
		UINT32     REG_HPGI_DEASSERT_TO_SMI_EN : 1;
		UINT32     REG_HPGI_DEASSERT_TO_SCI_EN : 1;
		UINT32                                 : 3;
		UINT32                   REG_HPGI_HOOK : 1;
		UINT32   HPGI_REG_ASSERT_TO_SMI_STATUS : 1;
		UINT32   HPGI_REG_ASSERT_TO_SCI_STATUS : 1;
		UINT32 HPGI_REG_DEASSERT_TO_SMI_STATUS : 1;
		UINT32 HPGI_REG_DEASSERT_TO_SCI_STATUS : 1;
		UINT32                                 : 3;
		UINT32 HPGI_REG_PRESENCE_DETECT_STATE_CHANGE_STATUS : 1;
		UINT32 REG_HPGI_PRESENCE_DETECT_STATE_CHANGE_EN : 1;
		UINT32                                 : 15;
	} Field;
	UINT32	Value;
} CXL_PCIEP_HPGI_REGISTER;


typedef union { 
	struct {
		UINT32  HTPLG_CNTL_DESCRIPTOR_SLOT_NUM : 6;
		UINT32                                 : 25;
		UINT32 HTPLG_CNTL_DESCRIPTOR_PORT_ACTIVE : 1;
	} Field;
	UINT32	Value;
} CXL_PCIEP_HCNT_DESCRIPTOR_REGISTER;


typedef union { 
	struct {
		UINT32                   SMU_HP_STATUS : 32;
	} Field;
	UINT32	Value;
} CXL_SMU_HP_STATUS_UPDATE_REGISTER;


typedef union { 
	struct {
		UINT32                  HP_SMU_COMMAND : 32;
	} Field;
	UINT32	Value;
} CXL_HP_SMU_COMMAND_UPDATE_REGISTER;


typedef union { 
	struct {
		UINT32                      SMU_HP_EOI : 1;
		UINT32                                 : 31;
	} Field;
	UINT32	Value;
} CXL_SMU_HP_END_OF_INTERRUPT_REGISTER;


typedef union { 
	struct {
		UINT32      LINK_MANAGEMENT_INT_STATUS : 1;
		UINT32                  LTR_INT_STATUS : 1;
		UINT32                  DPC_INT_STATUS : 1;
		UINT32         RX_LTR_RSMU_INTR_STATUS : 1;
		UINT32       CXL_PM_VDM_RCV_INT_STATUS : 1;
		UINT32      CXL_ERR_VDM_RCV_INT_STATUS : 1;
		UINT32        CXL_DL_LINKUP_INT_STATUS : 1;
		UINT32          CXL_MEM_RAS_INT_STATUS : 1;
		UINT32  CXL_DL_LINKUP_RSMU_INTR_STATUS : 1;
		UINT32                                 : 20;
		UINT32    CXL_DL_LINKUP_RSMU_INTR_MASK : 1;
		UINT32           RX_LTR_RSMU_INTR_MASK : 1;
		UINT32              DPC_RSMU_INTR_MASK : 1;
	} Field;
	UINT32	Value;
} CXL_SMU_INT_PIN_SHARING_PORT_INDICATOR_REGISTER;


typedef union { 
	struct {
		UINT32               LINK_SPEED_UPDATE : 1;
		UINT32                                 : 2;
		UINT32               LINK_WIDTH_UPDATE : 1;
		UINT32                                 : 2;
		UINT32     POWER_DOWN_COMMAND_COMPLETE : 1;
		UINT32                BANDWIDTH_UPDATE : 1;
		UINT32         LINK_POWER_STATE_CHANGE : 1;
		UINT32                                 : 1;
		UINT32            EQUALIZATION_REQUEST : 1;
		UINT32                                 : 19;
		UINT32              DL_BECOME_INACTIVE : 1;
		UINT32                DL_BECOME_ACTIVE : 1;
	} Field;
	UINT32	Value;
} CXL_LINK_MANAGEMENT_STATUS_REGISTER;


typedef union { 
	struct {
		UINT32          LINK_SPEED_UPDATE_MASK : 1;
		UINT32                                 : 2;
		UINT32          LINK_WIDTH_UPDATE_MASK : 1;
		UINT32                                 : 2;
		UINT32 POWER_DOWN_COMMAND_COMPLETE_MASK : 1;
		UINT32           BANDWIDTH_UPDATE_MASK : 1;
		UINT32    LINK_POWER_STATE_CHANGE_MASK : 1;
		UINT32                                 : 1;
		UINT32       EQUALIZATION_REQUEST_MASK : 1;
		UINT32                                 : 19;
		UINT32         DL_BECOME_INACTIVE_MASK : 1;
		UINT32           DL_BECOME_ACTIVE_MASK : 1;
	} Field;
	UINT32	Value;
} CXL_LINK_MANAGEMENT_MASK_REGISTER;


typedef union { 
	struct {
		UINT32               ERR_VDM_RCV_REQID : 16;
		UINT32       ERR_VDM_RCV_FW_INT_VECTOR : 4;
		UINT32                                 : 11;
		UINT32              ERR_VDM_RCV_STATUS : 1;
	} Field;
	UINT32	Value;
} CXL_ERR_VDM_RCV_DATA_REGISTER;


typedef union { 
	struct {
		UINT32             PM_VDM_RCV_PAYLOAD0 : 32;
	} Field;
	UINT32	Value;
} CXL_PM_VDM_RCV_DATA0_REGISTER;


typedef union { 
	struct {
		UINT32             PM_VDM_RCV_PAYLOAD1 : 32;
	} Field;
	UINT32	Value;
} CXL_PM_VDM_RCV_DATA1_REGISTER;


typedef union { 
	struct {
		UINT32                PM_VDM_RCV_REQID : 16;
		UINT32                                 : 14;
		UINT32         PM_VDM_RCV_EP_INDICATOR : 1;
		UINT32               PM_VDM_RCV_STATUS : 1;
	} Field;
	UINT32	Value;
} CXL_PM_VDM_RCV_DATA2_REGISTER;


typedef union { 
	struct {
		UINT32             PM_VDM_SND_PAYLOAD0 : 32;
	} Field;
	UINT32	Value;
} CXL_PM_VDM_SND_DATA0_REGISTER;


typedef union { 
	struct {
		UINT32             PM_VDM_SND_PAYLOAD1 : 32;
	} Field;
	UINT32	Value;
} CXL_PM_VDM_SND_DATA1_REGISTER;


typedef union { 
	struct {
		UINT32                                 : 31;
		UINT32               PM_VDM_SND_STATUS : 1;
	} Field;
	UINT32	Value;
} CXL_PM_VDM_SND_DATA2_REGISTER;


typedef union { 
	struct {
		UINT32               CFG_LNC_WINDOW_EN : 1;
		UINT32               CFG_LNC_BW_CNT_EN : 1;
		UINT32              CFG_LNC_CMN_CNT_EN : 1;
		UINT32                 CFG_LNC_OVRD_EN : 1;
		UINT32                CFG_LNC_OVRD_VAL : 1;
		UINT32                                 : 27;
	} Field;
	UINT32	Value;
} CXL_LCCNT_CONTROL_REGISTER;


typedef union { 
	struct {
		UINT32                  CFG_LNC_WINDOW : 24;
		UINT32                                 : 8;
	} Field;
	UINT32	Value;
} CXL_CFG_LNC_WINDOW_REGISTER_REGISTER;


typedef union { 
	struct {
		UINT32            CFG_LNC_BW_QUAN_THRD : 3;
		UINT32                                 : 1;
		UINT32           CFG_LNC_CMN_QUAN_THRD : 3;
		UINT32                                 : 25;
	} Field;
	UINT32	Value;
} CXL_LCCNT_QUAN_THRD_REGISTER;


typedef union { 
	struct {
		UINT32               CFG_LNC_BW_WEIGHT : 16;
		UINT32              CFG_LNC_CMN_WEIGHT : 16;
	} Field;
	UINT32	Value;
} CXL_LCCNT_WEIGHT_REGISTER;


typedef union { 
	struct {
		UINT32                  LNC_TOTAL_WACC : 32;
	} Field;
	UINT32	Value;
} CXL_LNC_TOTAL_WACC_REGISTER_REGISTER;


typedef union { 
	struct {
		UINT32                     LNC_BW_WACC : 32;
	} Field;
	UINT32	Value;
} CXL_LNC_BW_WACC_REGISTER_REGISTER;


typedef union { 
	struct {
		UINT32                    LNC_CMN_WACC : 32;
	} Field;
	UINT32	Value;
} CXL_LNC_CMN_WACC_REGISTER_REGISTER;


typedef union { 
	struct {
		UINT32       CXL_PIPE_RXSTANDBY_STATUS : 1;
		UINT32         CXL_PIPE_RXREADY_STATUS : 1;
		UINT32           CXL_PIPE_PERST_STATUS : 1;
		UINT32          CXL_PIPE_PHYRST_STATUS : 1;
		UINT32      CXL_PIPE_RXELECIDLE_STATUS : 1;
		UINT32       CXL_PIPE_PHYSTATUS_STATUS : 1;
		UINT32                                 : 10;
		UINT32               REG_DEBUG_LANE_EN : 16;
	} Field;
	UINT32	Value;
} CXL_PIPE_STATUS_DEBUG_REGISTER;


typedef union { 
	struct {
		UINT32                     CXL_SCRATCH : 32;
	} Field;
	UINT32	Value;
} CXL_SCRATCH_REGISTER;


typedef union { 
	struct {
		UINT32                     HW_00_DEBUG : 1;
		UINT32                     HW_01_DEBUG : 1;
		UINT32                     HW_02_DEBUG : 1;
		UINT32                     HW_03_DEBUG : 1;
		UINT32                     HW_04_DEBUG : 1;
		UINT32                     HW_05_DEBUG : 1;
		UINT32                     HW_06_DEBUG : 1;
		UINT32                     HW_07_DEBUG : 1;
		UINT32                     HW_08_DEBUG : 1;
		UINT32                     HW_09_DEBUG : 1;
		UINT32                     HW_10_DEBUG : 1;
		UINT32                     HW_11_DEBUG : 1;
		UINT32                     HW_12_DEBUG : 1;
		UINT32                     HW_13_DEBUG : 1;
		UINT32                     HW_14_DEBUG : 1;
		UINT32                     HW_15_DEBUG : 1;
		UINT32                                 : 16;
	} Field;
	UINT32	Value;
} CXL_HW_DEBUG_REGISTER;


typedef union { 
	struct {
		UINT32              CTS_Reg_Ocnt_Empty : 1;
		UINT32            CTS_Reg_Mem_Flush_En : 1;
		UINT32           CTS_Reg_Mem_Cancel_En : 1;
		UINT32                                 : 13;
		UINT32            CTS_Reg_Cxl_Rsp_Dslv : 1;
		UINT32                                 : 15;
	} Field;
	UINT32	Value;
} CXL_BRIDGE_CONTROL_REGISTER;


typedef union { 
	struct {
		UINT32                CTS_Reg_Ras_Info : 32;
	} Field;
	UINT32	Value;
} REG_CTS_Reg_Ras_Info_REGISTER;


typedef union { 
	struct {
		UINT32                       VENDOR_ID : 16;
		UINT32                       DEVICE_ID : 16;
	} Field;
	UINT32	Value;
} TYPE1_DEV_ID_VEND_ID_REG_REGISTER;


typedef union { 
	struct {
		UINT32                           IO_EN : 1;
		UINT32                             MSE : 1;
		UINT32                             BME : 1;
		UINT32                             SCO : 1;
		UINT32                          MWI_EN : 1;
		UINT32                           VGAPS : 1;
		UINT32                          PERREN : 1;
		UINT32                           IDSEL : 1;
		UINT32                          SERREN : 1;
		UINT32                         RSVDP_9 : 1;
		UINT32                          INT_EN : 1;
		UINT32                          RESERV : 5;
		UINT32                                 : 1;
		UINT32                        RSVDP_17 : 2;
		UINT32                      INT_STATUS : 1;
		UINT32                        CAP_LIST : 1;
		UINT32                  FAST_66MHZ_CAP : 1;
		UINT32                        RSVDP_22 : 1;
		UINT32                    FAST_B2B_CAP : 1;
		UINT32                      MASTER_DPE : 1;
		UINT32                  DEV_SEL_TIMING : 2;
		UINT32           SIGNALED_TARGET_ABORT : 1;
		UINT32               RCVD_TARGET_ABORT : 1;
		UINT32               RCVD_MASTER_ABORT : 1;
		UINT32              SIGNALED_SYS_ERROR : 1;
		UINT32           DETECTED_PARITY_ERROR : 1;
	} Field;
	UINT32	Value;
} TYPE1_STATUS_COMMAND_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     REVISION_ID : 8;
		UINT32               PROGRAM_INTERFACE : 8;
		UINT32                   SUBCLASS_CODE : 8;
		UINT32                 BASE_CLASS_CODE : 8;
	} Field;
	UINT32	Value;
} TYPE1_CLASS_CODE_REV_ID_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 CACHE_LINE_SIZE : 8;
		UINT32            LATENCY_MASTER_TIMER : 8;
		UINT32                     HEADER_TYPE : 7;
		UINT32                      MULTI_FUNC : 1;
		UINT32                            BIST : 8;
	} Field;
	UINT32	Value;
} TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     BAR0_MEM_IO : 1;
		UINT32                       BAR0_TYPE : 2;
		UINT32                   BAR0_PREFETCH : 1;
		UINT32                      BAR0_START : 28;
	} Field;
	UINT32	Value;
} BAR0_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     BAR1_MEM_IO : 1;
		UINT32                       BAR1_TYPE : 2;
		UINT32                   BAR1_PREFETCH : 1;
		UINT32                      BAR1_START : 28;
	} Field;
	UINT32	Value;
} BAR1_REG_REGISTER;


typedef union { 
	struct {
		UINT32                        PRIM_BUS : 8;
		UINT32                         SEC_BUS : 8;
		UINT32                         SUB_BUS : 8;
		UINT32                   SEC_LAT_TIMER : 8;
	} Field;
	UINT32	Value;
} SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32                       IO_DECODE : 1;
		UINT32                       IO_RESERV : 3;
		UINT32                         IO_BASE : 4;
		UINT32                  IO_DECODE_BIT8 : 1;
		UINT32                      IO_RESERV1 : 3;
		UINT32                        IO_LIMIT : 4;
		UINT32                 SEC_STAT_RESERV : 7;
		UINT32                        RSVDP_23 : 1;
		UINT32                   SEC_STAT_MDPE : 1;
		UINT32                        RSVDP_25 : 2;
		UINT32          SEC_STAT_SIG_TRGT_ABRT : 1;
		UINT32         SEC_STAT_RCVD_TRGT_ABRT : 1;
		UINT32         SEC_STAT_RCVD_MSTR_ABRT : 1;
		UINT32           SEC_STAT_RCVD_SYS_ERR : 1;
		UINT32                    SEC_STAT_DPE : 1;
	} Field;
	UINT32	Value;
} SEC_STAT_IO_LIMIT_IO_BASE_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 MEM_BASE_RESERV : 4;
		UINT32                        MEM_BASE : 12;
		UINT32                MEM_LIMIT_RESERV : 4;
		UINT32                       MEM_LIMIT : 12;
	} Field;
	UINT32	Value;
} MEM_LIMIT_MEM_BASE_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 PREF_MEM_DECODE : 1;
		UINT32                     PREF_RESERV : 3;
		UINT32                   PREF_MEM_BASE : 12;
		UINT32           PREF_MEM_LIMIT_DECODE : 1;
		UINT32                    PREF_RESERV1 : 3;
		UINT32                  PREF_MEM_LIMIT : 12;
	} Field;
	UINT32	Value;
} PREF_MEM_LIMIT_PREF_MEM_BASE_REG_REGISTER;


typedef union { 
	struct {
		UINT32             PREF_MEM_BASE_UPPER : 32;
	} Field;
	UINT32	Value;
} PREF_BASE_UPPER_REG_REGISTER;


typedef union { 
	struct {
		UINT32            PREF_MEM_LIMIT_UPPER : 32;
	} Field;
	UINT32	Value;
} PREF_LIMIT_UPPER_REG_REGISTER;


typedef union { 
	struct {
		UINT32                   IO_BASE_UPPER : 16;
		UINT32                  IO_LIMIT_UPPER : 16;
	} Field;
	UINT32	Value;
} IO_LIMIT_UPPER_IO_BASE_UPPER_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     CAP_POINTER : 8;
		UINT32                         RSVDP_8 : 24;
	} Field;
	UINT32	Value;
} TYPE1_CAP_PTR_REG_REGISTER;


typedef union { 
	struct {
		UINT32                  ROM_BAR_ENABLE : 1;
		UINT32       ROM_BAR_VALIDATION_STATUS : 3;
		UINT32      ROM_BAR_VALIDATION_DETAILS : 4;
		UINT32                         RSVDP_8 : 3;
		UINT32            EXP_ROM_BASE_ADDRESS : 21;
	} Field;
	UINT32	Value;
} TYPE1_EXP_ROM_BASE_REG_REGISTER;


typedef union { 
	struct {
		UINT32                        INT_LINE : 8;
		UINT32                         INT_PIN : 8;
		UINT32                            PERE : 1;
		UINT32                         SERR_EN : 1;
		UINT32                          ISA_EN : 1;
		UINT32                          VGA_EN : 1;
		UINT32                     VGA_16B_DEC : 1;
		UINT32                 MSTR_ABORT_MODE : 1;
		UINT32                             SBR : 1;
		UINT32              BRIDGE_CTRL_RESERV : 9;
	} Field;
	UINT32	Value;
} BRIDGE_CTRL_INT_PIN_INT_LINE_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 EXTENDED_CAP_ID : 16;
		UINT32                     CAP_VERSION : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} SPCIE_CAP_HEADER_REG_REGISTER;


typedef union { 
	struct {
		UINT32                      PERFORM_EQ : 1;
		UINT32                   EQ_REQ_INT_EN : 1;
		UINT32                         RSVDP_2 : 30;
	} Field;
	UINT32	Value;
} LINK_CONTROL3_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 LANE_ERR_STATUS : 16;
		UINT32           RSVDP_LANE_ERR_STATUS : 16;
	} Field;
	UINT32	Value;
} LANE_ERR_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32                  DSP_TX_PRESET0 : 4;
		UINT32             DSP_RX_PRESET_HINT0 : 3;
		UINT32                         RSVDP_7 : 1;
		UINT32                  USP_TX_PRESET0 : 4;
		UINT32             USP_RX_PRESET_HINT0 : 3;
		UINT32                        RSVDP_15 : 1;
		UINT32                  DSP_TX_PRESET1 : 4;
		UINT32             DSP_RX_PRESET_HINT1 : 3;
		UINT32                        RSVDP_23 : 1;
		UINT32                  USP_TX_PRESET1 : 4;
		UINT32             USP_RX_PRESET_HINT1 : 3;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} SPCIE_CAP_OFF_0CH_REG_REGISTER;


typedef union { 
	struct {
		UINT32                  DSP_TX_PRESET2 : 4;
		UINT32             DSP_RX_PRESET_HINT2 : 3;
		UINT32                         RSVDP_7 : 1;
		UINT32                  USP_TX_PRESET2 : 4;
		UINT32             USP_RX_PRESET_HINT2 : 3;
		UINT32                        RSVDP_15 : 1;
		UINT32                  DSP_TX_PRESET3 : 4;
		UINT32             DSP_RX_PRESET_HINT3 : 3;
		UINT32                        RSVDP_23 : 1;
		UINT32                  USP_TX_PRESET3 : 4;
		UINT32             USP_RX_PRESET_HINT3 : 3;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} SPCIE_CAP_OFF_10H_REG_REGISTER;


typedef union { 
	struct {
		UINT32                  DSP_TX_PRESET4 : 4;
		UINT32             DSP_RX_PRESET_HINT4 : 3;
		UINT32                         RSVDP_7 : 1;
		UINT32                  USP_TX_PRESET4 : 4;
		UINT32             USP_RX_PRESET_HINT4 : 3;
		UINT32                        RSVDP_15 : 1;
		UINT32                  DSP_TX_PRESET5 : 4;
		UINT32             DSP_RX_PRESET_HINT5 : 3;
		UINT32                        RSVDP_23 : 1;
		UINT32                  USP_TX_PRESET5 : 4;
		UINT32             USP_RX_PRESET_HINT5 : 3;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} SPCIE_CAP_OFF_14H_REG_REGISTER;


typedef union { 
	struct {
		UINT32                  DSP_TX_PRESET6 : 4;
		UINT32             DSP_RX_PRESET_HINT6 : 3;
		UINT32                         RSVDP_7 : 1;
		UINT32                  USP_TX_PRESET6 : 4;
		UINT32             USP_RX_PRESET_HINT6 : 3;
		UINT32                        RSVDP_15 : 1;
		UINT32                  DSP_TX_PRESET7 : 4;
		UINT32             DSP_RX_PRESET_HINT7 : 3;
		UINT32                        RSVDP_23 : 1;
		UINT32                  USP_TX_PRESET7 : 4;
		UINT32             USP_RX_PRESET_HINT7 : 3;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} SPCIE_CAP_OFF_18H_REG_REGISTER;


typedef union { 
	struct {
		UINT32                  DSP_TX_PRESET8 : 4;
		UINT32             DSP_RX_PRESET_HINT8 : 3;
		UINT32                         RSVDP_7 : 1;
		UINT32                  USP_TX_PRESET8 : 4;
		UINT32             USP_RX_PRESET_HINT8 : 3;
		UINT32                        RSVDP_15 : 1;
		UINT32                  DSP_TX_PRESET9 : 4;
		UINT32             DSP_RX_PRESET_HINT9 : 3;
		UINT32                        RSVDP_23 : 1;
		UINT32                  USP_TX_PRESET9 : 4;
		UINT32             USP_RX_PRESET_HINT9 : 3;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} SPCIE_CAP_OFF_1CH_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 DSP_TX_PRESET10 : 4;
		UINT32            DSP_RX_PRESET_HINT10 : 3;
		UINT32                         RSVDP_7 : 1;
		UINT32                 USP_TX_PRESET10 : 4;
		UINT32            USP_RX_PRESET_HINT10 : 3;
		UINT32                        RSVDP_15 : 1;
		UINT32                 DSP_TX_PRESET11 : 4;
		UINT32            DSP_RX_PRESET_HINT11 : 3;
		UINT32                        RSVDP_23 : 1;
		UINT32                 USP_TX_PRESET11 : 4;
		UINT32            USP_RX_PRESET_HINT11 : 3;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} SPCIE_CAP_OFF_20H_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 DSP_TX_PRESET12 : 4;
		UINT32            DSP_RX_PRESET_HINT12 : 3;
		UINT32                         RSVDP_7 : 1;
		UINT32                 USP_TX_PRESET12 : 4;
		UINT32            USP_RX_PRESET_HINT12 : 3;
		UINT32                        RSVDP_15 : 1;
		UINT32                 DSP_TX_PRESET13 : 4;
		UINT32            DSP_RX_PRESET_HINT13 : 3;
		UINT32                        RSVDP_23 : 1;
		UINT32                 USP_TX_PRESET13 : 4;
		UINT32            USP_RX_PRESET_HINT13 : 3;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} SPCIE_CAP_OFF_24H_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 DSP_TX_PRESET14 : 4;
		UINT32            DSP_RX_PRESET_HINT14 : 3;
		UINT32                         RSVDP_7 : 1;
		UINT32                 USP_TX_PRESET14 : 4;
		UINT32            USP_RX_PRESET_HINT14 : 3;
		UINT32                        RSVDP_15 : 1;
		UINT32                 DSP_TX_PRESET15 : 4;
		UINT32            DSP_RX_PRESET_HINT15 : 3;
		UINT32                        RSVDP_23 : 1;
		UINT32                 USP_TX_PRESET15 : 4;
		UINT32            USP_RX_PRESET_HINT15 : 3;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} SPCIE_CAP_OFF_28H_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 EXTENDED_CAP_ID : 16;
		UINT32                     CAP_VERSION : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} PL16G_EXT_CAP_HDR_REG_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 32;
	} Field;
	UINT32	Value;
} PL16G_CAPABILITY_REG_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 32;
	} Field;
	UINT32	Value;
} PL16G_CONTROL_REG_REGISTER;


typedef union { 
	struct {
		UINT32                      EQ_16G_CPL : 1;
		UINT32                   EQ_16G_CPL_P1 : 1;
		UINT32                   EQ_16G_CPL_P2 : 1;
		UINT32                   EQ_16G_CPL_P3 : 1;
		UINT32                 LINK_EQ_16G_REQ : 1;
		UINT32                         RSVDP_5 : 27;
	} Field;
	UINT32	Value;
} PL16G_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32                  LC_DPAR_STATUS : 16;
		UINT32            RSVDP_LC_DPAR_STATUS : 16;
	} Field;
	UINT32	Value;
} PL16G_LC_DPAR_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32       FIRST_RETIMER_DPAR_STATUS : 16;
		UINT32 RSVDP_FIRST_RETIMER_DPAR_STATUS : 16;
	} Field;
	UINT32	Value;
} PL16G_FIRST_RETIMER_DPAR_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32      SECOND_RETIMER_DPAR_STATUS : 16;
		UINT32 RSVDP_SECOND_RETIMER_DPAR_STATUS : 16;
	} Field;
	UINT32	Value;
} PL16G_SECOND_RETIMER_DPAR_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32              DSP_16G_TX_PRESET0 : 4;
		UINT32              USP_16G_TX_PRESET0 : 4;
		UINT32              DSP_16G_TX_PRESET1 : 4;
		UINT32              USP_16G_TX_PRESET1 : 4;
		UINT32              DSP_16G_TX_PRESET2 : 4;
		UINT32              USP_16G_TX_PRESET2 : 4;
		UINT32              DSP_16G_TX_PRESET3 : 4;
		UINT32              USP_16G_TX_PRESET3 : 4;
	} Field;
	UINT32	Value;
} PL16G_CAP_OFF_20H_REG_REGISTER;


typedef union { 
	struct {
		UINT32              DSP_16G_TX_PRESET4 : 4;
		UINT32              USP_16G_TX_PRESET4 : 4;
		UINT32              DSP_16G_TX_PRESET5 : 4;
		UINT32              USP_16G_TX_PRESET5 : 4;
		UINT32              DSP_16G_TX_PRESET6 : 4;
		UINT32              USP_16G_TX_PRESET6 : 4;
		UINT32              DSP_16G_TX_PRESET7 : 4;
		UINT32              USP_16G_TX_PRESET7 : 4;
	} Field;
	UINT32	Value;
} PL16G_CAP_OFF_24H_REG_REGISTER;


typedef union { 
	struct {
		UINT32              DSP_16G_TX_PRESET8 : 4;
		UINT32              USP_16G_TX_PRESET8 : 4;
		UINT32              DSP_16G_TX_PRESET9 : 4;
		UINT32              USP_16G_TX_PRESET9 : 4;
		UINT32             DSP_16G_TX_PRESET10 : 4;
		UINT32             USP_16G_TX_PRESET10 : 4;
		UINT32             DSP_16G_TX_PRESET11 : 4;
		UINT32             USP_16G_TX_PRESET11 : 4;
	} Field;
	UINT32	Value;
} PL16G_CAP_OFF_28H_REG_REGISTER;


typedef union { 
	struct {
		UINT32             DSP_16G_TX_PRESET12 : 4;
		UINT32             USP_16G_TX_PRESET12 : 4;
		UINT32             DSP_16G_TX_PRESET13 : 4;
		UINT32             USP_16G_TX_PRESET13 : 4;
		UINT32             DSP_16G_TX_PRESET14 : 4;
		UINT32             USP_16G_TX_PRESET14 : 4;
		UINT32             DSP_16G_TX_PRESET15 : 4;
		UINT32             USP_16G_TX_PRESET15 : 4;
	} Field;
	UINT32	Value;
} PL16G_CAP_OFF_2CH_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 EXTENDED_CAP_ID : 16;
		UINT32                     CAP_VERSION : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} PL32G_EXT_CAP_HDR_REG_REGISTER;


typedef union { 
	struct {
		UINT32  EQ_BYPASS_HIGHEST_RATE_SUPPORT : 1;
		UINT32            NO_EQ_NEEDED_SUPPORT : 1;
		UINT32                         RSVDP_2 : 6;
		UINT32             MOD_TS_PCIE_SUPPORT : 1;
		UINT32   MOD_TS_TRAING_SET_MSG_SUPPORT : 1;
		UINT32     MOD_TS_ALT_PROTOCOL_SUPPORT : 1;
		UINT32          MOD_TS_RSVD_USAGE_MODE : 5;
		UINT32                        RSVDP_16 : 16;
	} Field;
	UINT32	Value;
} PL32G_CAPABILITY_REG_REGISTER;


typedef union { 
	struct {
		UINT32  EQ_BYPASS_HIGHEST_RATE_DISABLE : 1;
		UINT32            NO_EQ_NEEDED_DISABLE : 1;
		UINT32                         RSVDP_2 : 6;
		UINT32        MOD_TS_USAGE_MODE_SELECT : 3;
		UINT32                        RSVDP_11 : 21;
	} Field;
	UINT32	Value;
} PL32G_CONTROL_REG_REGISTER;


typedef union { 
	struct {
		UINT32                      EQ_32G_CPL : 1;
		UINT32                   EQ_32G_CPL_P1 : 1;
		UINT32                   EQ_32G_CPL_P2 : 1;
		UINT32                   EQ_32G_CPL_P3 : 1;
		UINT32                 LINK_EQ_32G_REQ : 1;
		UINT32                     MOD_TS_RCVD : 1;
		UINT32       RX_ENH_LINK_BEHAVIOR_CTRL : 2;
		UINT32                 TX_PRECODING_ON : 1;
		UINT32                  TX_PRECODE_REQ : 1;
		UINT32               NO_EQ_NEEDED_RCVD : 1;
		UINT32                        RSVDP_11 : 21;
	} Field;
	UINT32	Value;
} PL32G_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32          RCVD_MOD_TS_USAGE_MODE : 3;
		UINT32               RCVD_MOD_TS_INFO1 : 13;
		UINT32           RCVD_MOD_TS_VENDER_ID : 16;
	} Field;
	UINT32	Value;
} PL32G_RCVD_MOD_TS_DATA1_REG_REGISTER;


typedef union { 
	struct {
		UINT32               RCVD_MOD_TS_INFO2 : 24;
		UINT32   RCVD_ALT_PROTOCOL_NEGO_STATUS : 2;
		UINT32                        RSVDP_26 : 6;
	} Field;
	UINT32	Value;
} PL32G_RCVD_MOD_TS_DATA2_REG_REGISTER;


typedef union { 
	struct {
		UINT32            TX_MOD_TS_USAGE_MODE : 3;
		UINT32                 TX_MOD_TS_INFO1 : 13;
		UINT32             TX_MOD_TS_VENDER_ID : 16;
	} Field;
	UINT32	Value;
} PL32G_TX_MOD_TS_DATA1_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 TX_MOD_TS_INFO2 : 24;
		UINT32     TX_ALT_PROTOCOL_NEGO_STATUS : 2;
		UINT32                        RSVDP_26 : 6;
	} Field;
	UINT32	Value;
} PL32G_TX_MOD_TS_DATA2_REG_REGISTER;


typedef union { 
	struct {
		UINT32              DSP_32G_TX_PRESET0 : 4;
		UINT32              USP_32G_TX_PRESET0 : 4;
		UINT32              DSP_32G_TX_PRESET1 : 4;
		UINT32              USP_32G_TX_PRESET1 : 4;
		UINT32              DSP_32G_TX_PRESET2 : 4;
		UINT32              USP_32G_TX_PRESET2 : 4;
		UINT32              DSP_32G_TX_PRESET3 : 4;
		UINT32              USP_32G_TX_PRESET3 : 4;
	} Field;
	UINT32	Value;
} PL32G_CAP_OFF_20H_REG_REGISTER;


typedef union { 
	struct {
		UINT32              DSP_32G_TX_PRESET4 : 4;
		UINT32              USP_32G_TX_PRESET4 : 4;
		UINT32              DSP_32G_TX_PRESET5 : 4;
		UINT32              USP_32G_TX_PRESET5 : 4;
		UINT32              DSP_32G_TX_PRESET6 : 4;
		UINT32              USP_32G_TX_PRESET6 : 4;
		UINT32              DSP_32G_TX_PRESET7 : 4;
		UINT32              USP_32G_TX_PRESET7 : 4;
	} Field;
	UINT32	Value;
} PL32G_CAP_OFF_24H_REG_REGISTER;


typedef union { 
	struct {
		UINT32              DSP_32G_TX_PRESET8 : 4;
		UINT32              USP_32G_TX_PRESET8 : 4;
		UINT32              DSP_32G_TX_PRESET9 : 4;
		UINT32              USP_32G_TX_PRESET9 : 4;
		UINT32             DSP_32G_TX_PRESET10 : 4;
		UINT32             USP_32G_TX_PRESET10 : 4;
		UINT32             DSP_32G_TX_PRESET11 : 4;
		UINT32             USP_32G_TX_PRESET11 : 4;
	} Field;
	UINT32	Value;
} PL32G_CAP_OFF_28H_REG_REGISTER;


typedef union { 
	struct {
		UINT32             DSP_32G_TX_PRESET12 : 4;
		UINT32             USP_32G_TX_PRESET12 : 4;
		UINT32             DSP_32G_TX_PRESET13 : 4;
		UINT32             USP_32G_TX_PRESET13 : 4;
		UINT32             DSP_32G_TX_PRESET14 : 4;
		UINT32             USP_32G_TX_PRESET14 : 4;
		UINT32             DSP_32G_TX_PRESET15 : 4;
		UINT32             USP_32G_TX_PRESET15 : 4;
	} Field;
	UINT32	Value;
} PL32G_CAP_OFF_2CH_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 EXTENDED_CAP_ID : 16;
		UINT32                     CAP_VERSION : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} MARGIN_EXT_CAP_HDR_REG_REGISTER;


typedef union { 
	struct {
		UINT32  MARGINING_USES_DRIVER_SOFTWARE : 1;
		UINT32                         RSVDP_1 : 15;
		UINT32                 MARGINING_READY : 1;
		UINT32        MARGINING_SOFTWARE_READY : 1;
		UINT32                        RSVDP_18 : 14;
	} Field;
	UINT32	Value;
} MARGIN_PORT_CAPABILITIES_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS0_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS1_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS2_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS3_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS4_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS5_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS6_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS7_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS8_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS9_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS10_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS11_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS12_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS13_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS14_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 RECEIVER_NUMBER : 3;
		UINT32                     MARGIN_TYPE : 3;
		UINT32                     USAGE_MODEL : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                  MARGIN_PAYLOAD : 8;
		UINT32          RECEIVER_NUMBER_STATUS : 3;
		UINT32              MARGIN_TYPE_STATUS : 3;
		UINT32              USAGE_MODEL_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32           MARGIN_PAYLOAD_STATUS : 8;
	} Field;
	UINT32	Value;
} MARGIN_LANE_CNTRL_STATUS15_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 EXTENDED_CAP_ID : 16;
		UINT32                     CAP_VERSION : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} RAS_DES_CAP_HEADER_REG_REGISTER;


typedef union { 
	struct {
		UINT32                         VSEC_ID : 16;
		UINT32                        VSEC_REV : 4;
		UINT32                     VSEC_LENGTH : 12;
	} Field;
	UINT32	Value;
} VENDOR_SPECIFIC_HEADER_REG_REGISTER;


typedef union { 
	struct {
		UINT32             EVENT_COUNTER_CLEAR : 2;
		UINT32            EVENT_COUNTER_ENABLE : 3;
		UINT32                         RSVDP_5 : 2;
		UINT32            EVENT_COUNTER_STATUS : 1;
		UINT32       EVENT_COUNTER_LANE_SELECT : 4;
		UINT32                        RSVDP_12 : 4;
		UINT32      EVENT_COUNTER_EVENT_SELECT : 12;
		UINT32                        RSVDP_28 : 4;
	} Field;
	UINT32	Value;
} EVENT_COUNTER_CONTROL_REG_REGISTER;


typedef union { 
	struct {
		UINT32              EVENT_COUNTER_DATA : 32;
	} Field;
	UINT32	Value;
} EVENT_COUNTER_DATA_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     TIMER_START : 1;
		UINT32                         RSVDP_1 : 7;
		UINT32      TIME_BASED_DURATION_SELECT : 8;
		UINT32                        RSVDP_16 : 8;
		UINT32        TIME_BASED_REPORT_SELECT : 8;
	} Field;
	UINT32	Value;
} TIME_BASED_ANALYSIS_CONTROL_REG_REGISTER;


typedef union { 
	struct {
		UINT32        TIME_BASED_ANALYSIS_DATA : 32;
	} Field;
	UINT32	Value;
} TIME_BASED_ANALYSIS_DATA_REG_REGISTER;


typedef union { 
	struct {
		UINT32  TIME_BASED_ANALYSIS_DATA_63_32 : 32;
	} Field;
	UINT32	Value;
} TIME_BASED_ANALYSIS_DATA_63_32_REG_REGISTER;


typedef union { 
	struct {
		UINT32         ERROR_INJECTION0_ENABLE : 1;
		UINT32         ERROR_INJECTION1_ENABLE : 1;
		UINT32         ERROR_INJECTION2_ENABLE : 1;
		UINT32         ERROR_INJECTION3_ENABLE : 1;
		UINT32         ERROR_INJECTION4_ENABLE : 1;
		UINT32         ERROR_INJECTION5_ENABLE : 1;
		UINT32         ERROR_INJECTION6_ENABLE : 1;
		UINT32                         RSVDP_7 : 25;
	} Field;
	UINT32	Value;
} EINJ_ENABLE_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     EINJ0_COUNT : 8;
		UINT32                  EINJ0_CRC_TYPE : 4;
		UINT32                        RSVDP_12 : 20;
	} Field;
	UINT32	Value;
} EINJ0_CRC_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     EINJ1_COUNT : 8;
		UINT32               EINJ1_SEQNUM_TYPE : 1;
		UINT32                         RSVDP_9 : 7;
		UINT32                EINJ1_BAD_SEQNUM : 13;
		UINT32                        RSVDP_29 : 3;
	} Field;
	UINT32	Value;
} EINJ1_SEQNUM_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     EINJ2_COUNT : 8;
		UINT32                 EINJ2_DLLP_TYPE : 2;
		UINT32                        RSVDP_10 : 22;
	} Field;
	UINT32	Value;
} EINJ2_DLLP_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     EINJ3_COUNT : 8;
		UINT32               EINJ3_SYMBOL_TYPE : 3;
		UINT32                        RSVDP_11 : 21;
	} Field;
	UINT32	Value;
} EINJ3_SYMBOL_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     EINJ4_COUNT : 8;
		UINT32                EINJ4_UPDFC_TYPE : 3;
		UINT32                        RSVDP_11 : 1;
		UINT32                 EINJ4_VC_NUMBER : 3;
		UINT32                        RSVDP_15 : 1;
		UINT32           EINJ4_BAD_UPDFC_VALUE : 13;
		UINT32                        RSVDP_29 : 3;
	} Field;
	UINT32	Value;
} EINJ4_FC_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     EINJ5_COUNT : 8;
		UINT32             EINJ5_SPECIFIED_TLP : 1;
		UINT32                         RSVDP_9 : 23;
	} Field;
	UINT32	Value;
} EINJ5_SP_TLP_REG_REGISTER;


typedef union { 
	struct {
		UINT32          EINJ6_COMPARE_POINT_H0 : 32;
	} Field;
	UINT32	Value;
} EINJ6_COMPARE_POINT_H0_REG_REGISTER;


typedef union { 
	struct {
		UINT32          EINJ6_COMPARE_POINT_H1 : 32;
	} Field;
	UINT32	Value;
} EINJ6_COMPARE_POINT_H1_REG_REGISTER;


typedef union { 
	struct {
		UINT32          EINJ6_COMPARE_POINT_H2 : 32;
	} Field;
	UINT32	Value;
} EINJ6_COMPARE_POINT_H2_REG_REGISTER;


typedef union { 
	struct {
		UINT32          EINJ6_COMPARE_POINT_H3 : 32;
	} Field;
	UINT32	Value;
} EINJ6_COMPARE_POINT_H3_REG_REGISTER;


typedef union { 
	struct {
		UINT32          EINJ6_COMPARE_VALUE_H0 : 32;
	} Field;
	UINT32	Value;
} EINJ6_COMPARE_VALUE_H0_REG_REGISTER;


typedef union { 
	struct {
		UINT32          EINJ6_COMPARE_VALUE_H1 : 32;
	} Field;
	UINT32	Value;
} EINJ6_COMPARE_VALUE_H1_REG_REGISTER;


typedef union { 
	struct {
		UINT32          EINJ6_COMPARE_VALUE_H2 : 32;
	} Field;
	UINT32	Value;
} EINJ6_COMPARE_VALUE_H2_REG_REGISTER;


typedef union { 
	struct {
		UINT32          EINJ6_COMPARE_VALUE_H3 : 32;
	} Field;
	UINT32	Value;
} EINJ6_COMPARE_VALUE_H3_REG_REGISTER;


typedef union { 
	struct {
		UINT32           EINJ6_CHANGE_POINT_H0 : 32;
	} Field;
	UINT32	Value;
} EINJ6_CHANGE_POINT_H0_REG_REGISTER;


typedef union { 
	struct {
		UINT32           EINJ6_CHANGE_POINT_H1 : 32;
	} Field;
	UINT32	Value;
} EINJ6_CHANGE_POINT_H1_REG_REGISTER;


typedef union { 
	struct {
		UINT32           EINJ6_CHANGE_POINT_H2 : 32;
	} Field;
	UINT32	Value;
} EINJ6_CHANGE_POINT_H2_REG_REGISTER;


typedef union { 
	struct {
		UINT32           EINJ6_CHANGE_POINT_H3 : 32;
	} Field;
	UINT32	Value;
} EINJ6_CHANGE_POINT_H3_REG_REGISTER;


typedef union { 
	struct {
		UINT32           EINJ6_CHANGE_VALUE_H0 : 32;
	} Field;
	UINT32	Value;
} EINJ6_CHANGE_VALUE_H0_REG_REGISTER;


typedef union { 
	struct {
		UINT32           EINJ6_CHANGE_VALUE_H1 : 32;
	} Field;
	UINT32	Value;
} EINJ6_CHANGE_VALUE_H1_REG_REGISTER;


typedef union { 
	struct {
		UINT32           EINJ6_CHANGE_VALUE_H2 : 32;
	} Field;
	UINT32	Value;
} EINJ6_CHANGE_VALUE_H2_REG_REGISTER;


typedef union { 
	struct {
		UINT32           EINJ6_CHANGE_VALUE_H3 : 32;
	} Field;
	UINT32	Value;
} EINJ6_CHANGE_VALUE_H3_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     EINJ6_COUNT : 8;
		UINT32          EINJ6_INVERTED_CONTROL : 1;
		UINT32               EINJ6_PACKET_TYPE : 3;
		UINT32                        RSVDP_12 : 20;
	} Field;
	UINT32	Value;
} EINJ6_TLP_REG_REGISTER;


typedef union { 
	struct {
		UINT32               FORCE_DETECT_LANE : 16;
		UINT32            FORCE_DETECT_LANE_EN : 1;
		UINT32                        RSVDP_17 : 3;
		UINT32                     TX_EIOS_NUM : 2;
		UINT32              LOW_POWER_INTERVAL : 2;
		UINT32                        RSVDP_24 : 8;
	} Field;
	UINT32	Value;
} SD_CONTROL1_REG_REGISTER;


typedef union { 
	struct {
		UINT32                      HOLD_LTSSM : 1;
		UINT32                RECOVERY_REQUEST : 1;
		UINT32            NOACK_FORCE_LINKDOWN : 1;
		UINT32                         RSVDP_3 : 5;
		UINT32        DIRECT_RECIDLE_TO_CONFIG : 1;
		UINT32        DIRECT_POLCOMP_TO_DETECT : 1;
		UINT32          DIRECT_LPBKSLV_TO_EXIT : 1;
		UINT32                        RSVDP_11 : 5;
		UINT32    FRAMING_ERR_RECOVERY_DISABLE : 1;
		UINT32                        RSVDP_17 : 15;
	} Field;
	UINT32	Value;
} SD_CONTROL2_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     LANE_SELECT : 4;
		UINT32                         RSVDP_4 : 12;
		UINT32                 PIPE_RXPOLARITY : 1;
		UINT32                PIPE_DETECT_LANE : 1;
		UINT32                    PIPE_RXVALID : 1;
		UINT32                 PIPE_RXELECIDLE : 1;
		UINT32                 PIPE_TXELECIDLE : 1;
		UINT32                        RSVDP_21 : 3;
		UINT32                  DESKEW_POINTER : 8;
	} Field;
	UINT32	Value;
} SD_STATUS_L1LANE_REG_REGISTER;


typedef union { 
	struct {
		UINT32                 FRAMING_ERR_PTR : 7;
		UINT32                     FRAMING_ERR : 1;
		UINT32                 PIPE_POWER_DOWN : 3;
		UINT32                        RSVDP_11 : 4;
		UINT32                   LANE_REVERSAL : 1;
		UINT32                  LTSSM_VARIABLE : 16;
	} Field;
	UINT32	Value;
} SD_STATUS_L1LTSSM_REG_REGISTER;


typedef union { 
	struct {
		UINT32              INTERNAL_PM_MSTATE : 5;
		UINT32                         RSVDP_5 : 3;
		UINT32              INTERNAL_PM_SSTATE : 4;
		UINT32                 PME_RESEND_FLAG : 1;
		UINT32                                 : 3;
		UINT32                    LATCHED_NFTS : 8;
		UINT32                        RSVDP_24 : 8;
	} Field;
	UINT32	Value;
} SD_STATUS_PM_REG_REGISTER;


typedef union { 
	struct {
		UINT32                   TX_TLP_SEQ_NO : 12;
		UINT32                   RX_ACK_SEQ_NO : 12;
		UINT32                          DLCMSM : 2;
		UINT32                        FC_INIT1 : 1;
		UINT32                        FC_INIT2 : 1;
		UINT32                        RSVDP_28 : 4;
	} Field;
	UINT32	Value;
} SD_STATUS_L2_REG_REGISTER;


typedef union { 
	struct {
		UINT32                   CREDIT_SEL_VC : 3;
		UINT32          CREDIT_SEL_CREDIT_TYPE : 1;
		UINT32             CREDIT_SEL_TLP_TYPE : 2;
		UINT32                   CREDIT_SEL_HD : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32                    CREDIT_DATA0 : 12;
		UINT32                    CREDIT_DATA1 : 12;
	} Field;
	UINT32	Value;
} SD_STATUS_L3FC_REG_REGISTER;


typedef union { 
	struct {
		UINT32                   MFTLP_POINTER : 7;
		UINT32                    MFTLP_STATUS : 1;
		UINT32                         RSVDP_8 : 24;
	} Field;
	UINT32	Value;
} SD_STATUS_L3_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     EQ_LANE_SEL : 4;
		UINT32                     EQ_RATE_SEL : 2;
		UINT32                         RSVDP_6 : 2;
		UINT32                  EXT_EQ_TIMEOUT : 2;
		UINT32                        RSVDP_10 : 6;
		UINT32              EVAL_INTERVAL_TIME : 2;
		UINT32                        RSVDP_18 : 5;
		UINT32               FOM_TARGET_ENABLE : 1;
		UINT32                      FOM_TARGET : 8;
	} Field;
	UINT32	Value;
} SD_EQ_CONTROL1_REG_REGISTER;


typedef union { 
	struct {
		UINT32       FORCE_LOCAL_TX_PRE_CURSOR : 6;
		UINT32           FORCE_LOCAL_TX_CURSOR : 6;
		UINT32      FORCE_LOCAL_TX_POST_CURSOR : 6;
		UINT32 FORCE_LOCAL_RX_HINT_OR_FORCE_LOCAL_TX_2ND_PRE_CURSOR : 6;
		UINT32           FORCE_LOCAL_TX_PRESET : 4;
		UINT32      FORCE_LOCAL_TX_COEF_ENABLE : 1;
		UINT32      FORCE_LOCAL_RX_HINT_ENABLE : 1;
		UINT32    FORCE_LOCAL_TX_PRESET_ENABLE : 1;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} SD_EQ_CONTROL2_REG_REGISTER;


typedef union { 
	struct {
		UINT32      FORCE_REMOTE_TX_PRE_CURSOR : 6;
		UINT32          FORCE_REMOTE_TX_CURSOR : 6;
		UINT32     FORCE_REMOTE_TX_POST_CURSOR : 6;
		UINT32  FORCE_REMOTE_TX_2ND_PRE_CURSOR : 6;
		UINT32                        RSVDP_24 : 4;
		UINT32     FORCE_REMOTE_TX_COEF_ENABLE : 1;
		UINT32                        RSVDP_29 : 3;
	} Field;
	UINT32	Value;
} SD_EQ_CONTROL3_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     EQ_SEQUENCE : 1;
		UINT32             EQ_CONVERGENCE_INFO : 2;
		UINT32              EQ_RULED_VIOLATION : 1;
		UINT32              EQ_RULEA_VIOLATION : 1;
		UINT32              EQ_RULEB_VIOLATION : 1;
		UINT32              EQ_RULEC_VIOLATION : 1;
		UINT32                 EQ_REJECT_EVENT : 1;
		UINT32                         RSVDP_8 : 18;
		UINT32        EQ_REMOTE_2ND_PRE_CURSOR : 6;
	} Field;
	UINT32	Value;
} SD_EQ_STATUS1_REG_REGISTER;


typedef union { 
	struct {
		UINT32             EQ_LOCAL_PRE_CURSOR : 6;
		UINT32                 EQ_LOCAL_CURSOR : 6;
		UINT32            EQ_LOCAL_POST_CURSOR : 6;
		UINT32 EQ_LOCAL_RX_HINT_OR_EQ_LOCAL_2ND_PRE_CURSOR : 6;
		UINT32              EQ_LOCAL_FOM_VALUE : 8;
	} Field;
	UINT32	Value;
} SD_EQ_STATUS2_REG_REGISTER;


typedef union { 
	struct {
		UINT32            EQ_REMOTE_PRE_CURSOR : 6;
		UINT32                EQ_REMOTE_CURSOR : 6;
		UINT32           EQ_REMOTE_POST_CURSOR : 6;
		UINT32                    EQ_REMOTE_LF : 6;
		UINT32                    EQ_REMOTE_FS : 6;
		UINT32                        RSVDP_30 : 2;
	} Field;
	UINT32	Value;
} SD_EQ_STATUS3_REG_REGISTER;


typedef union { 
	struct {
		UINT32         SN_PCIE_EXTENDED_CAP_ID : 16;
		UINT32                  SN_CAP_VERSION : 4;
		UINT32                  SN_NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} SN_BASE_REGISTER;


typedef union { 
	struct {
		UINT32             SN_SER_NUM_REG_1_DW : 32;
	} Field;
	UINT32	Value;
} SER_NUM_REG_DW_1_REGISTER;


typedef union { 
	struct {
		UINT32             SN_SER_NUM_REG_2_DW : 32;
	} Field;
	UINT32	Value;
} SER_NUM_REG_DW_2_REGISTER;


typedef union { 
	struct {
		UINT32                              ID : 16;
		UINT32                             CAP : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} RASDP_EXT_CAP_HDR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         VSEC_ID : 16;
		UINT32                        VSEC_REV : 4;
		UINT32                     VSEC_LENGTH : 12;
	} Field;
	UINT32	Value;
} RASDP_VENDOR_SPECIFIC_HDR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           ERROR_PROT_DISABLE_TX : 1;
		UINT32 ERROR_PROT_DISABLE_AXI_BRIDGE_MASTER : 1;
		UINT32 ERROR_PROT_DISABLE_AXI_BRIDGE_OUTBOUND : 1;
		UINT32    ERROR_PROT_DISABLE_DMA_WRITE : 1;
		UINT32    ERROR_PROT_DISABLE_LAYER2_TX : 1;
		UINT32    ERROR_PROT_DISABLE_LAYER3_TX : 1;
		UINT32       ERROR_PROT_DISABLE_ADM_TX : 1;
		UINT32       ERROR_PROT_DISABLE_CXS_TX : 1;
		UINT32      ERROR_PROT_DISABLE_DTIM_TX : 1;
		UINT32       ERROR_PROT_DISABLE_CXL_TX : 1;
		UINT32                        RSVDP_10 : 6;
		UINT32           ERROR_PROT_DISABLE_RX : 1;
		UINT32 ERROR_PROT_DISABLE_AXI_BRIDGE_INBOUND_COMPLETION : 1;
		UINT32 ERROR_PROT_DISABLE_AXI_BRIDGE_INBOUND_REQUEST : 1;
		UINT32     ERROR_PROT_DISABLE_DMA_READ : 1;
		UINT32    ERROR_PROT_DISABLE_LAYER2_RX : 1;
		UINT32    ERROR_PROT_DISABLE_LAYER3_RX : 1;
		UINT32       ERROR_PROT_DISABLE_ADM_RX : 1;
		UINT32       ERROR_PROT_DISABLE_CXS_RX : 1;
		UINT32         ERROR_PROT_DISABLE_LTIM : 1;
		UINT32                        RSVDP_25 : 7;
	} Field;
	UINT32	Value;
} RASDP_ERROR_PROT_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32             CORR_CLEAR_COUNTERS : 1;
		UINT32                         RSVDP_1 : 3;
		UINT32                CORR_EN_COUNTERS : 1;
		UINT32                         RSVDP_5 : 15;
		UINT32   CORR_COUNTER_SELECTION_REGION : 4;
		UINT32          CORR_COUNTER_SELECTION : 8;
	} Field;
	UINT32	Value;
} RASDP_CORR_COUNTER_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                    CORR_COUNTER : 8;
		UINT32                         RSVDP_8 : 12;
		UINT32    CORR_COUNTER_SELECTED_REGION : 4;
		UINT32           CORR_COUNTER_SELECTED : 8;
	} Field;
	UINT32	Value;
} RASDP_CORR_COUNT_REPORT_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           UNCORR_CLEAR_COUNTERS : 1;
		UINT32                         RSVDP_1 : 3;
		UINT32              UNCORR_EN_COUNTERS : 1;
		UINT32                         RSVDP_5 : 15;
		UINT32 UNCORR_COUNTER_SELECTION_REGION : 4;
		UINT32        UNCORR_COUNTER_SELECTION : 8;
	} Field;
	UINT32	Value;
} RASDP_UNCORR_COUNTER_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                  UNCORR_COUNTER : 8;
		UINT32                         RSVDP_8 : 12;
		UINT32  UNCORR_COUNTER_SELECTED_REGION : 4;
		UINT32         UNCORR_COUNTER_SELECTED : 8;
	} Field;
	UINT32	Value;
} RASDP_UNCORR_COUNT_REPORT_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                    ERROR_INJ_EN : 1;
		UINT32                         RSVDP_1 : 3;
		UINT32                  ERROR_INJ_TYPE : 2;
		UINT32                         RSVDP_6 : 2;
		UINT32                 ERROR_INJ_COUNT : 8;
		UINT32                   ERROR_INJ_LOC : 8;
		UINT32                        RSVDP_24 : 8;
	} Field;
	UINT32	Value;
} RASDP_ERROR_INJ_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 4;
		UINT32            REG_FIRST_CORR_ERROR : 4;
		UINT32            LOC_FIRST_CORR_ERROR : 8;
		UINT32                        RSVDP_16 : 4;
		UINT32             REG_LAST_CORR_ERROR : 4;
		UINT32             LOC_LAST_CORR_ERROR : 8;
	} Field;
	UINT32	Value;
} RASDP_CORR_ERROR_LOCATION_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 4;
		UINT32          REG_FIRST_UNCORR_ERROR : 4;
		UINT32          LOC_FIRST_UNCORR_ERROR : 8;
		UINT32                        RSVDP_16 : 4;
		UINT32           REG_LAST_UNCORR_ERROR : 4;
		UINT32           LOC_LAST_UNCORR_ERROR : 8;
	} Field;
	UINT32	Value;
} RASDP_UNCORR_ERROR_LOCATION_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                   ERROR_MODE_EN : 1;
		UINT32               AUTO_LINK_DOWN_EN : 1;
		UINT32                         RSVDP_2 : 30;
	} Field;
	UINT32	Value;
} RASDP_ERROR_MODE_EN_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                ERROR_MODE_CLEAR : 1;
		UINT32                         RSVDP_1 : 31;
	} Field;
	UINT32	Value;
} RASDP_ERROR_MODE_CLEAR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32             RAM_ADDR_CORR_ERROR : 27;
		UINT32                        RSVDP_27 : 1;
		UINT32            RAM_INDEX_CORR_ERROR : 4;
	} Field;
	UINT32	Value;
} RASDP_RAM_ADDR_CORR_ERROR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           RAM_ADDR_UNCORR_ERROR : 27;
		UINT32                        RSVDP_27 : 1;
		UINT32          RAM_INDEX_UNCORR_ERROR : 4;
	} Field;
	UINT32	Value;
} RASDP_RAM_ADDR_UNCORR_ERROR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32            FLEX_BUS_PORT_CAP_ID : 16;
		UINT32       FLEX_BUS_PORT_CAP_VERSION : 4;
		UINT32        FLEX_BUS_PORT_NXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} CXL_RCRB_PCIE_EXT_CAP_HDR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32          CXL_RCRB_DVSEC_VNDR_ID : 16;
		UINT32              CXL_RCRB_DVSEC_REV : 4;
		UINT32              CXL_RCRB_DVSEC_LEN : 12;
	} Field;
	UINT32	Value;
} CXL_RCRB_DVSEC_PORT_HDR_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32               CXL_RCRB_DVSEC_ID : 16;
		UINT32                       CACHE_CAP : 1;
		UINT32                          IO_CAP : 1;
		UINT32                         MEM_CAP : 1;
		UINT32                        RSVDP_19 : 2;
		UINT32                      CXL2P0_CAP : 1;
		UINT32             CXL_MUL_LOG_DEV_CAP : 1;
		UINT32                        RSVDP_23 : 6;
		UINT32          CXL_128B_LOPT_FLIT_CAP : 1;
		UINT32            CXL_68B_ENH_FLIT_CAP : 1;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} CXL_RCRB_DVSEC_HDR_2_FLEXBUS_CAP_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                        CACHE_EN : 1;
		UINT32                           IO_EN : 1;
		UINT32                          MEM_EN : 1;
		UINT32          CXL_SYNC_HDR_BYPASS_EN : 1;
		UINT32                    DRIFT_BUF_EN : 1;
		UINT32                   CXL2P0_ENABLE : 1;
		UINT32                      CXL_MLD_EN : 1;
		UINT32       DISABLE_CXL_L1P1_TRAINING : 1;
		UINT32               RETIMER_1_PRESENT : 1;
		UINT32               RETIMER_2_PRESENT : 1;
		UINT32                        RSVDP_10 : 3;
		UINT32           CXL_128B_LOPT_FLIT_EN : 1;
		UINT32             CXL_68B_ENH_FLIT_EN : 1;
		UINT32                        RSVDP_15 : 1;
		UINT32                   CACHE_ENABLED : 1;
		UINT32                      IO_ENABLED : 1;
		UINT32                     MEM_ENABLED : 1;
		UINT32     CXL_SYNC_HDR_BYPASS_ENABLED : 1;
		UINT32               DRIFT_BUF_ENABLED : 1;
		UINT32                  CXL2P0_ENABLED : 1;
		UINT32                 CXL_MLD_ENABLED : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32  CXL_CORR_PROT_ID_FRAMING_ERROR : 1;
		UINT32 CXL_UNCORR_PROT_ID_FRAMING_ERROR : 1;
		UINT32          CXL_UNEXP_PROT_ID_DROP : 1;
		UINT32       CXL_RETIMER_PSNT_MISMATCH : 1;
		UINT32      CXL_FB_EN_BITS_P2_MISMATCH : 1;
		UINT32          CXL_128B_LOPT_FLIT_END : 1;
		UINT32            CXL_68B_ENH_FLIT_END : 1;
		UINT32   CXL_IO_THROTTLE_REQD_AT_64GTS : 1;
	} Field;
	UINT32	Value;
} CXL_RCRB_FLEXBUS_CNTRL_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32               RCVD_FB_DATA_PH_1 : 24;
		UINT32                        RSVDP_24 : 8;
	} Field;
	UINT32	Value;
} CXL_RCRB_FLEXBUS_RCVD_MOD_TS_PH1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32              CXL_2_0_EXT_CAP_ID : 16;
		UINT32         CXL_2_0_EXT_CAP_VERSION : 4;
		UINT32         CXL_2_0_EXT_NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} CXL_2_0_EXT_DVSEC_CAP_HDR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32       CXL_2_0_EXT_DVSEC_VNDR_ID : 16;
		UINT32           CXL_2_0_EXT_DVSEC_REV : 4;
		UINT32           CXL_2_0_EXT_DVSEC_LEN : 12;
	} Field;
	UINT32	Value;
} CXL_2_0_EXT_DVSEC_HDR_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32            CXL_2_0_EXT_DVSEC_ID : 16;
		UINT32            CXL_2_0_PM_INIT_CMPL : 1;
		UINT32                        RSVDP_17 : 13;
		UINT32                                 : 1;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} CXL_2_0_EXT_DVSEC_HDR_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                      UNMASK_SBR : 1;
		UINT32             UNMASK_LINK_DISABLE : 1;
		UINT32             ALT_MEM_ID_SPACE_EN : 1;
		UINT32                         ALT_BME : 1;
		UINT32                         RSVDP_4 : 10;
		UINT32                                 : 1;
		UINT32                        RSVDP_15 : 1;
		UINT32                    ALT_BUS_BASE : 8;
		UINT32                   ALT_BUS_LIMIT : 8;
	} Field;
	UINT32	Value;
} CXL_2_0_CTRL_ALT_BUS_BASE_LIMIT_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 4;
		UINT32                    ALT_MEM_BASE : 12;
		UINT32                        RSVDP_16 : 4;
		UINT32                   ALT_MEM_LIMIT : 12;
	} Field;
	UINT32	Value;
} CXL_2_0_ALT_MEM_BASE_LIMIT_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 4;
		UINT32               ALT_PREF_MEM_BASE : 12;
		UINT32                        RSVDP_16 : 4;
		UINT32              ALT_PREF_MEM_LIMIT : 12;
	} Field;
	UINT32	Value;
} CXL_2_0_ALT_PREFETCH_MEM_BASE_LIMIT_OFF_REGISTER;


typedef union { 
	struct {
		UINT32          ALT_PREF_MEM_BASE_HIGH : 32;
	} Field;
	UINT32	Value;
} CXL_2_0_ALT_PREFETCH_MEM_BASE_HIGH_OFF_REGISTER;


typedef union { 
	struct {
		UINT32         ALT_PREF_MEM_LIMIT_HIGH : 32;
	} Field;
	UINT32	Value;
} CXL_2_0_ALT_PREFETCH_MEM_LIMIT_HIGH_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                     CXL_RCRB_EN : 1;
		UINT32                         RSVDP_1 : 12;
		UINT32          CXL_RCRB_BASE_ADDR_LOW : 19;
	} Field;
	UINT32	Value;
} CXL_2_0_CXL_RCRB_BASE_OFF_REGISTER;


typedef union { 
	struct {
		UINT32         CXL_RCRB_BASE_ADDR_HIGH : 32;
	} Field;
	UINT32	Value;
} CXL_2_0_CXL_RCRB_BASE_HIGH_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         ECAP_ID : 16;
		UINT32                        ECAP_VER : 4;
		UINT32                        NEXT_PTR : 12;
	} Field;
	UINT32	Value;
} CXL_GPF_PORT_DVSEC_HDR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                       VENDOR_ID : 16;
		UINT32                         VERSION : 4;
		UINT32                            SIZE : 12;
	} Field;
	UINT32	Value;
} CXL_GPF_PORT_DVSEC_HDR_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                        DVSEC_ID : 16;
		UINT32                        RSVDP_16 : 16;
	} Field;
	UINT32	Value;
} CXL_GPF_PORT_DVSEC_HDR_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                     PH1_TM_BASE : 4;
		UINT32                         RSVDP_4 : 4;
		UINT32                    PH1_TM_SCALE : 4;
		UINT32                        RSVDP_12 : 4;
		UINT32                     PH2_TM_BASE : 4;
		UINT32                        RSVDP_20 : 4;
		UINT32                    PH2_TM_SCALE : 4;
		UINT32                        RSVDP_28 : 4;
	} Field;
	UINT32	Value;
} CXL_GPF_PORT_PH1_CTRL_PH2_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         ECAP_ID : 16;
		UINT32                        ECAP_VER : 4;
		UINT32                        NEXT_PTR : 12;
	} Field;
	UINT32	Value;
} CXL_REG_LOC_DVSEC_HDR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                       VENDOR_ID : 16;
		UINT32                         VERSION : 4;
		UINT32                            SIZE : 12;
	} Field;
	UINT32	Value;
} CXL_REG_LOC_DVSEC_HDR_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                        DVSEC_ID : 16;
		UINT32                        RSVDP_16 : 16;
	} Field;
	UINT32	Value;
} CXL_REG_LOC_DVSEC_HDR_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                       REG_BIR_1 : 3;
		UINT32                         RSVDP_3 : 5;
		UINT32            REG_BLK_IDENTIFIER_1 : 8;
		UINT32            REG_BLK_OFFSET_LOW_1 : 16;
	} Field;
	UINT32	Value;
} CXL_REG_LOC_BLOCK_1_LOW_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           REG_BLK_OFFSET_HIGH_1 : 32;
	} Field;
	UINT32	Value;
} CXL_REG_LOC_BLOCK_1_HIGH_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                       REG_BIR_2 : 3;
		UINT32                         RSVDP_3 : 5;
		UINT32            REG_BLK_IDENTIFIER_2 : 8;
		UINT32            REG_BLK_OFFSET_LOW_2 : 16;
	} Field;
	UINT32	Value;
} CXL_REG_LOC_BLOCK_2_LOW_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           REG_BLK_OFFSET_HIGH_2 : 32;
	} Field;
	UINT32	Value;
} CXL_REG_LOC_BLOCK_2_HIGH_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                       REG_BIR_3 : 3;
		UINT32                         RSVDP_3 : 5;
		UINT32            REG_BLK_IDENTIFIER_3 : 8;
		UINT32            REG_BLK_OFFSET_LOW_3 : 16;
	} Field;
	UINT32	Value;
} CXL_REG_LOC_BLOCK_3_LOW_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           REG_BLK_OFFSET_HIGH_3 : 32;
	} Field;
	UINT32	Value;
} CXL_REG_LOC_BLOCK_3_HIGH_OFF_REGISTER;


typedef union { 
	struct {
		UINT32         CXL_RCRB_MEMBAR0_CAP_ID : 16;
		UINT32        CXL_RCRB_MEMBAR0_CAP_VER : 4;
		UINT32  CXL_RCRB_MEMBAR0_CACHE_MEM_VER : 4;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_MEM_SIZE : 8;
	} Field;
	UINT32	Value;
} MEMBAR0_HEADER_CAP_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_RAS_CAP_ID : 16;
		UINT32    CXL_RCRB_MEMBAR0_RAS_CAP_VER : 4;
		UINT32    CXL_RCRB_MEMBAR0_RAS_CAP_PTR : 12;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_CAP_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_SEC_CAP_ID : 16;
		UINT32    CXL_RCRB_MEMBAR0_SEC_CAP_VER : 4;
		UINT32    CXL_RCRB_MEMBAR0_SEC_CAP_PTR : 12;
	} Field;
	UINT32	Value;
} MEMBAR0_SECURITY_CAP_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32    CXL_RCRB_MEMBAR0_LINK_CAP_ID : 16;
		UINT32   CXL_RCRB_MEMBAR0_LINK_CAP_VER : 4;
		UINT32   CXL_RCRB_MEMBAR0_LINK_CAP_PTR : 12;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_CAP_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_CACHE_DATA_PAR : 1;
		UINT32  CXL_RCRB_MEMBAR0_CACHE_ADD_PAR : 1;
		UINT32   CXL_RCRB_MEMBAR0_CACHE_BE_PAR : 1;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_DATA_ECC : 1;
		UINT32   CXL_RCRB_MEMBAR0_MEM_DATA_PAR : 1;
		UINT32    CXL_RCRB_MEMBAR0_MEM_ADD_PAR : 1;
		UINT32     CXL_RCRB_MEMBAR0_MEM_BE_PAR : 1;
		UINT32   CXL_RCRB_MEMBAR0_MEM_DATA_ECC : 1;
		UINT32 CXL_RCRB_MEMBAR0_REINIT_THRESHOLD : 1;
		UINT32 CXL_RCRB_MEMBAR0_ENCOD_VIOLATION : 1;
		UINT32     CXL_RCRB_MEMBAR0_POISON_RVD : 1;
		UINT32  CXL_RCRB_MEMBAR0_RCVR_OVERFLOW : 1;
		UINT32                        RSVDP_12 : 2;
		UINT32 CXL_RCRB_MEMBAR0_CXL_INTERNAL_ERR : 1;
		UINT32  CXL_RCRB_MEMBAR0_CXL_IDE_TX_ER : 1;
		UINT32  CXL_RCRB_MEMBAR0_CXL_IDE_RX_ER : 1;
		UINT32                        RSVDP_17 : 15;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_UNCOR_ERROR_STATUS_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_CACHE_DATA_PAR_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_ADD_PAR_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_BE_PAR_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_DATA_ECC_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_DATA_PAR_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_ADD_PAR_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_BE_PAR_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_DATA_ECC_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_REINIT_THRESHOLD_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_ENCOD_VIOLATION_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_POISON_RVD_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_RCVR_OVERFLOW_MASK : 1;
		UINT32                        RSVDP_12 : 2;
		UINT32 CXL_RCRB_MEMBAR0_CXL_INTERNAL_ER_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_CXL_IDE_TX_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_CXL_IDE_RX_MASK : 1;
		UINT32                        RSVDP_17 : 15;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_UNCOR_ERROR_MASK_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_CACHE_DATA_PAR_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_ADD_PAR_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_BE_PAR_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_DATA_ECC_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_DATA_PAR_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_ADD_PAR_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_BE_PAR_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_DATA_ECC_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_REINIT_THRESHOLD_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_ENCOD_VIOLATION_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_POISON_RVD_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_RCVR_OVERFLOW_SEV : 1;
		UINT32                        RSVDP_12 : 2;
		UINT32 CXL_RCRB_MEMBAR0_CXL_INTERNAL_ER_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_CXL_IDE_TX_SEV : 1;
		UINT32 CXL_RCRB_MEMBAR0_CXL_IDE_RX_SEV : 1;
		UINT32                        RSVDP_17 : 15;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_UNCOR_ERROR_SEVERITY_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_CORR_CACHE_DATA_ECC : 1;
		UINT32 CXL_RCRB_MEMBAR0_CORR_MEM_DATA_ECC : 1;
		UINT32 CXL_RCRB_MEMBAR0_CORR_CRC_THRESHOLD : 1;
		UINT32 CXL_RCRB_MEMBAR0_CORR_RETRY_THRESHOLD : 1;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_POISON_RVD : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_POISON_RVD : 1;
		UINT32  CXL_RCRB_MEMBAR0_PHY_LAYER_ERR : 1;
		UINT32                         RSVDP_7 : 25;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_CORR_ERROR_STATUS_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_CORR_CACHE_DATA_ECC_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_CORR_MEM_DATA_ECC_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_CORR_CRC_THRESHOLD_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_CORR_RETRY_THRESHOLD_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_CACHE_POISON_RVD_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_MEM_POISON_RVD_MASK : 1;
		UINT32 CXL_RCRB_MEMBAR0_PHY_LAYER_ERR_MASK : 1;
		UINT32                         RSVDP_7 : 25;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_CORR_ERROR_MASK_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32        CXL_RCRB_MEMBAR0_ERR_PTR : 6;
		UINT32                         RSVDP_6 : 3;
		UINT32 CXL_RCRB_MEMBAR0_MUL_HDR_RECORDING_CAP : 1;
		UINT32                        RSVDP_10 : 3;
		UINT32     CXL_RCRB_MEMBAR0_POISON_END : 1;
		UINT32                        RSVDP_14 : 18;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_ERR_CAP_CNTRL_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_00 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_00_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_01 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_01_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_02 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_02_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_03 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_03_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_04 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_04_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_05 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_05_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_06 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_06_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_07 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_07_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_08 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_08_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_09 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_09_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_10 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_10_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_11 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_11_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_12 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_12_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_13 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_13_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_14 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_14_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     CXL_RCRB_MEMBAR0_HDR_LOG_15 : 32;
	} Field;
	UINT32	Value;
} MEMBAR0_RAS_HEADER_LOG_REG_15_OFF_REGISTER;


typedef union { 
	struct {
		UINT32   CXL_RCRB_MEMBAR0_LNK_VER_SUPP : 4;
		UINT32   CXL_RCRB_MEMBAR0_LNK_VER_RCVD : 4;
		UINT32   CXL_RCRB_MEMBAR0_LLR_VAL_SUPP : 8;
		UINT32   CXL_RCRB_MEMBAR0_LLR_VAL_RCVD : 8;
		UINT32 CXL_RCRB_MEMBAR0_NUM_RETRY_RCVD : 5;
		UINT32 CXL_RCRB_MEMBAR0_NUM_PHY_REINIT_RCVD : 3;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_LAYER_CAP_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_NUM_PHY_REINIT_RCVD_2 : 2;
		UINT32    CXL_RCRB_MEMBAR0_WR_PTR_RCVD : 8;
		UINT32 CXL_RCRB_MEMBAR0_ECHO_ESEQ_RCVD : 8;
		UINT32 CXL_RCRB_MEMBAR0_NUM_FREE_BUF_RCVD : 8;
		UINT32 CXL_RCRB_MEMBAR0_NO_LL_RESET_SUPPORT : 1;
		UINT32                        RSVDP_27 : 5;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_LAYER_CAP_2_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32    CXL_RCRB_MEMBAR0_LINK_LL_RST : 1;
		UINT32 CXL_RCRB_MEMBAR0_LINK_LL_INIT_STALL : 1;
		UINT32 CXL_RCRB_MEMBAR0_LINK_LL_CRD_STALL : 1;
		UINT32 CXL_RCRB_MEMBAR0_LINK_INIT_STATE : 2;
		UINT32 CXL_RCRB_MEMBAR0_LINK_LL_RETRY_BUF_CONSUME : 8;
		UINT32                        RSVDP_13 : 19;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_CNTRL_STATUS_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_LINK_CTL_CACHE_REQ_CRD : 10;
		UINT32 CXL_RCRB_MEMBAR0_LINK_CTL_CACHE_RSP_CRD : 10;
		UINT32 CXL_RCRB_MEMBAR0_LINK_CTL_CACHE_DATA_CRD : 10;
		UINT32 CXL_RCRB_MEMBAR0_LINK_CTL_MEM_REQ_RSP_CRD : 2;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_RX_CR_CNTRL_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_LINK_CTL_MEM_REQ_RSP_CRD_2 : 8;
		UINT32 CXL_RCRB_MEMBAR0_LINK_CTL_MEM_DATA_CRD : 10;
		UINT32                        RSVDP_18 : 14;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_RX_CR_CNTRL_2_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_LINK_ST_CACHE_REQ_CRD : 10;
		UINT32 CXL_RCRB_MEMBAR0_LINK_ST_CACHE_RSP_CRD : 10;
		UINT32 CXL_RCRB_MEMBAR0_LINK_ST_CACHE_DATA_CRD : 10;
		UINT32 CXL_RCRB_MEMBAR0_LINK_ST_MEM_REQ_RSP_CRD : 2;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_RX_CR_RTN_STATUS_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_LINK_ST_MEM_REQ_RSP_CRD_2 : 8;
		UINT32 CXL_RCRB_MEMBAR0_LINK_ST_MEM_DATA_CRD : 10;
		UINT32                        RSVDP_18 : 14;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_RX_CR_RTN_STATUS_2_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_LINK_TX_CACHE_REQ_CRD : 10;
		UINT32 CXL_RCRB_MEMBAR0_LINK_TX_CACHE_RSP_CRD : 10;
		UINT32 CXL_RCRB_MEMBAR0_LINK_TX_CACHE_DATA_CRD : 10;
		UINT32 CXL_RCRB_MEMBAR0_LINK_TX_MEM_REQ_RSP_CRD : 2;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_TX_CR_STATUS_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_LINK_TX_MEM_REQ_RSP_CRD_2 : 8;
		UINT32 CXL_RCRB_MEMBAR0_LINK_TX_MEM_DATA_CRD : 10;
		UINT32                        RSVDP_18 : 14;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_TX_CR_STATUS_2_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_LINK_ACK_FRC_THRESHOLD : 8;
		UINT32 CXL_RCRB_MEMBAR0_LINK_ACK_FLUSH_RETIMER : 10;
		UINT32                        RSVDP_18 : 14;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_ACK_TMR_CNTRL_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 CXL_RCRB_MEMBAR0_LINK_MDH_DISABLE : 1;
		UINT32                         RSVDP_1 : 31;
	} Field;
	UINT32	Value;
} MEMBAR0_LINK_DEFEATURE_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 4;
		UINT32 CXL_RCRB_MEMBAR0_ARB_MUX_CXL_IO_RND_ROBIN : 4;
		UINT32                         RSVDP_8 : 24;
	} Field;
	UINT32	Value;
} MEMBAR0_ARB_MUX_CXL_IO_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 4;
		UINT32 CXL_RCRB_MEMBAR0_ARB_MUX_CXL_CACHE_MEM_RND_ROBIN : 4;
		UINT32                         RSVDP_8 : 24;
	} Field;
	UINT32	Value;
} MEMBAR0_ARB_MUX_CXL_CACHE_MEM_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                      EXT_CAP_ID : 16;
		UINT32                     CAP_VERSION : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} DPC_EXT_CAP_HEADER_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                 DPC_INT_MSG_NUM : 5;
		UINT32                  DPC_RP_EXT_DPC : 1;
		UINT32  DPC_POISON_TLP_EGR_BLK_SUPPORT : 1;
		UINT32         DPC_DPC_SW_TRIG_SUPPORT : 1;
		UINT32             DPC_RP_PIO_LOG_SIZE : 4;
		UINT32   DPC_DL_ACTIVE_ERR_COR_SUPPORT : 1;
		UINT32                        RSVDP_13 : 3;
		UINT32                 DPC_DPC_TRIG_EN : 2;
		UINT32               DPC_DPC_CPL_CNTRL : 1;
		UINT32                  DPC_DPC_INT_EN : 1;
		UINT32              DPC_DPC_ERR_COR_EN : 1;
		UINT32     DPC_POISON_TLP_EGR_BLOCK_EN : 1;
		UINT32              DPC_DPC_SW_TRIGGER : 1;
		UINT32           DPC_DL_ACTIVE_ERR_COR : 1;
		UINT32                        RSVDP_24 : 8;
	} Field;
	UINT32	Value;
} DPC_CAP_CONTROL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32             DPC_DPC_TRIG_STATUS : 1;
		UINT32             DPC_DPC_TRIG_REASON : 2;
		UINT32              DPC_DPC_INT_STATUS : 1;
		UINT32                 DPC_DPC_RP_BUSY : 1;
		UINT32        DPC_DPC_TRIG_REASON_EXTN : 2;
		UINT32                         RSVDP_7 : 1;
		UINT32      DPC_RP_PIO_FE_PTR_POSITION : 5;
		UINT32                        RSVDP_13 : 3;
		UINT32         DPC_DPC_ERROR_SOURCE_ID : 16;
	} Field;
	UINT32	Value;
} DPC_STATUS_ERR_SOURCE_ID_OFF_REGISTER;


typedef union { 
	struct {
		UINT32              DPC_STS_CFG_UR_CPL : 1;
		UINT32              DPC_STS_CFG_CA_CPL : 1;
		UINT32                 DPC_STS_CFG_CTO : 1;
		UINT32                         RSVDP_3 : 5;
		UINT32               DPC_STS_IO_UR_CPL : 1;
		UINT32               DPC_STS_IO_CA_CPL : 1;
		UINT32                  DPC_STS_IO_CTO : 1;
		UINT32                        RSVDP_11 : 5;
		UINT32              DPC_STS_MEM_UR_CPL : 1;
		UINT32              DPC_STS_MEM_CA_CPL : 1;
		UINT32                 DPC_STS_MEM_CTO : 1;
		UINT32                        RSVDP_19 : 13;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32              DPC_MSK_CFG_UR_CPL : 1;
		UINT32              DPC_MSK_CFG_CA_CPL : 1;
		UINT32                 DPC_MSK_CFG_CTO : 1;
		UINT32                         RSVDP_3 : 5;
		UINT32               DPC_MSK_IO_UR_CPL : 1;
		UINT32               DPC_MSK_IO_CA_CPL : 1;
		UINT32                  DPC_MSK_IO_CTO : 1;
		UINT32                        RSVDP_11 : 5;
		UINT32              DPC_MSK_MEM_UR_CPL : 1;
		UINT32              DPC_MSK_MEM_CA_CPL : 1;
		UINT32                 DPC_MSK_MEM_CTO : 1;
		UINT32                        RSVDP_19 : 13;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_MASK_OFF_REGISTER;


typedef union { 
	struct {
		UINT32              DPC_SEV_CFG_UR_CPL : 1;
		UINT32              DPC_SEV_CFG_CA_CPL : 1;
		UINT32                 DPC_SEV_CFG_CTO : 1;
		UINT32                         RSVDP_3 : 5;
		UINT32               DPC_SEV_IO_UR_CPL : 1;
		UINT32               DPC_SEV_IO_CA_CPL : 1;
		UINT32                  DPC_SEV_IO_CTO : 1;
		UINT32                        RSVDP_11 : 5;
		UINT32              DPC_SEV_MEM_UR_CPL : 1;
		UINT32              DPC_SEV_MEM_CA_CPL : 1;
		UINT32                 DPC_SEV_MEM_CTO : 1;
		UINT32                        RSVDP_19 : 13;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_SEVERITY_OFF_REGISTER;


typedef union { 
	struct {
		UINT32              DPC_SYS_CFG_UR_CPL : 1;
		UINT32              DPC_SYS_CFG_CA_CPL : 1;
		UINT32                 DPC_SYS_CFG_CTO : 1;
		UINT32                         RSVDP_3 : 5;
		UINT32               DPC_SYS_IO_UR_CPL : 1;
		UINT32               DPC_SYS_IO_CA_CPL : 1;
		UINT32                  DPC_SYS_IO_CTO : 1;
		UINT32                        RSVDP_11 : 5;
		UINT32              DPC_SYS_MEM_UR_CPL : 1;
		UINT32              DPC_SYS_MEM_CA_CPL : 1;
		UINT32                 DPC_SYS_MEM_CTO : 1;
		UINT32                        RSVDP_19 : 13;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_SYSERROR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32              DPC_EXC_CFG_UR_CPL : 1;
		UINT32              DPC_EXC_CFG_CA_CPL : 1;
		UINT32                 DPC_EXC_CFG_CTO : 1;
		UINT32                         RSVDP_3 : 5;
		UINT32               DPC_EXC_IO_UR_CPL : 1;
		UINT32               DPC_EXC_IO_CA_CPL : 1;
		UINT32                  DPC_EXC_IO_CTO : 1;
		UINT32                        RSVDP_11 : 5;
		UINT32              DPC_EXC_MEM_UR_CPL : 1;
		UINT32              DPC_EXC_MEM_CA_CPL : 1;
		UINT32                 DPC_EXC_MEM_CTO : 1;
		UINT32                        RSVDP_19 : 13;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_EXCEPTION_OFF_REGISTER;


typedef union { 
	struct {
		UINT32      DPC_FIRST_DWORD_FIRST_BYTE : 8;
		UINT32     DPC_FIRST_DWORD_SECOND_BYTE : 8;
		UINT32      DPC_FIRST_DWORD_THIRD_BYTE : 8;
		UINT32     DPC_FIRST_DWORD_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_HEADER_LOG0_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     DPC_SECOND_DWORD_FIRST_BYTE : 8;
		UINT32    DPC_SECOND_DWORD_SECOND_BYTE : 8;
		UINT32     DPC_SECOND_DWORD_THIRD_BYTE : 8;
		UINT32    DPC_SECOND_DWORD_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_HEADER_LOG1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32      DPC_THIRD_DWORD_FIRST_BYTE : 8;
		UINT32     DPC_THIRD_DWORD_SECOND_BYTE : 8;
		UINT32      DPC_THIRD_DWORD_THIRD_BYTE : 8;
		UINT32     DPC_THIRD_DWORD_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_HEADER_LOG2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     DPC_FOURTH_DWORD_FIRST_BYTE : 8;
		UINT32    DPC_FOURTH_DWORD_SECOND_BYTE : 8;
		UINT32     DPC_FOURTH_DWORD_THIRD_BYTE : 8;
		UINT32    DPC_FOURTH_DWORD_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_HEADER_LOG3_OFF_REGISTER;


typedef union { 
	struct {
		UINT32          DPC_IMPSPEC_FIRST_BYTE : 8;
		UINT32         DPC_IMPSPEC_SECOND_BYTE : 8;
		UINT32          DPC_IMPSPEC_THIRD_BYTE : 8;
		UINT32         DPC_IMPSPEC_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_IMPSPEC_LOG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           DPC_RP_PIO_PRFX_BYTE0 : 8;
		UINT32           DPC_RP_PIO_PRFX_BYTE1 : 8;
		UINT32           DPC_RP_PIO_PRFX_BYTE2 : 8;
		UINT32           DPC_RP_PIO_PRFX_BYTE3 : 8;
	} Field;
	UINT32	Value;
} DPC_RP_PIO_PREFIX_LOG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                       PM_CAP_ID : 8;
		UINT32                 PM_NEXT_POINTER : 8;
		UINT32                     PM_SPEC_VER : 3;
		UINT32                         PME_CLK : 1;
		UINT32                                 : 1;
		UINT32                             DSI : 1;
		UINT32                        AUX_CURR : 3;
		UINT32                      D1_SUPPORT : 1;
		UINT32                      D2_SUPPORT : 1;
		UINT32                     PME_SUPPORT : 5;
	} Field;
	UINT32	Value;
} CAP_ID_NXT_PTR_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     POWER_STATE : 2;
		UINT32                         RSVDP_2 : 1;
		UINT32                     NO_SOFT_RST : 1;
		UINT32                         RSVDP_4 : 4;
		UINT32                      PME_ENABLE : 1;
		UINT32                     DATA_SELECT : 4;
		UINT32                      DATA_SCALE : 2;
		UINT32                      PME_STATUS : 1;
		UINT32                        RSVDP_16 : 6;
		UINT32                   B2_B3_SUPPORT : 1;
		UINT32              BUS_PWR_CLK_CON_EN : 1;
		UINT32               DATA_REG_ADD_INFO : 8;
	} Field;
	UINT32	Value;
} CON_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32                     PCIE_CAP_ID : 8;
		UINT32               PCIE_CAP_NEXT_PTR : 8;
		UINT32                    PCIE_CAP_REG : 4;
		UINT32              PCIE_DEV_PORT_TYPE : 4;
		UINT32                   PCIE_SLOT_IMP : 1;
		UINT32                PCIE_INT_MSG_NUM : 5;
		UINT32                            RSVD : 1;
		UINT32                                 : 1;
	} Field;
	UINT32	Value;
} PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_REGISTER;


typedef union { 
	struct {
		UINT32       PCIE_CAP_MAX_PAYLOAD_SIZE : 3;
		UINT32   PCIE_CAP_PHANTOM_FUNC_SUPPORT : 2;
		UINT32           PCIE_CAP_EXT_TAG_SUPP : 1;
		UINT32                         RSVDP_6 : 9;
		UINT32  PCIE_CAP_ROLE_BASED_ERR_REPORT : 1;
		UINT32                        RSVDP_16 : 16;
	} Field;
	UINT32	Value;
} DEVICE_CAPABILITIES_REG_REGISTER;


typedef union { 
	struct {
		UINT32     PCIE_CAP_CORR_ERR_REPORT_EN : 1;
		UINT32 PCIE_CAP_NON_FATAL_ERR_REPORT_EN : 1;
		UINT32    PCIE_CAP_FATAL_ERR_REPORT_EN : 1;
		UINT32   PCIE_CAP_UNSUPPORT_REQ_REP_EN : 1;
		UINT32           PCIE_CAP_EN_REL_ORDER : 1;
		UINT32    PCIE_CAP_MAX_PAYLOAD_SIZE_CS : 3;
		UINT32             PCIE_CAP_EXT_TAG_EN : 1;
		UINT32        PCIE_CAP_PHANTOM_FUNC_EN : 1;
		UINT32        PCIE_CAP_AUX_POWER_PM_EN : 1;
		UINT32            PCIE_CAP_EN_NO_SNOOP : 1;
		UINT32      PCIE_CAP_MAX_READ_REQ_SIZE : 3;
		UINT32                                 : 1;
		UINT32      PCIE_CAP_CORR_ERR_DETECTED : 1;
		UINT32 PCIE_CAP_NON_FATAL_ERR_DETECTED : 1;
		UINT32     PCIE_CAP_FATAL_ERR_DETECTED : 1;
		UINT32 PCIE_CAP_UNSUPPORTED_REQ_DETECTED : 1;
		UINT32     PCIE_CAP_AUX_POWER_DETECTED : 1;
		UINT32          PCIE_CAP_TRANS_PENDING : 1;
		UINT32                                 : 1;
		UINT32                        RSVDP_23 : 9;
	} Field;
	UINT32	Value;
} DEVICE_CONTROL_DEVICE_STATUS_REGISTER;


typedef union { 
	struct {
		UINT32         PCIE_CAP_MAX_LINK_SPEED : 4;
		UINT32         PCIE_CAP_MAX_LINK_WIDTH : 6;
		UINT32 PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT : 2;
		UINT32       PCIE_CAP_L0S_EXIT_LATENCY : 3;
		UINT32        PCIE_CAP_L1_EXIT_LATENCY : 3;
		UINT32        PCIE_CAP_CLOCK_POWER_MAN : 1;
		UINT32 PCIE_CAP_SURPRISE_DOWN_ERR_REP_CAP : 1;
		UINT32     PCIE_CAP_DLL_ACTIVE_REP_CAP : 1;
		UINT32        PCIE_CAP_LINK_BW_NOT_CAP : 1;
		UINT32    PCIE_CAP_ASPM_OPT_COMPLIANCE : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32               PCIE_CAP_PORT_NUM : 8;
	} Field;
	UINT32	Value;
} LINK_CAPABILITIES_REG_REGISTER;


typedef union { 
	struct {
		UINT32 PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL : 2;
		UINT32                         RSVDP_2 : 1;
		UINT32                    PCIE_CAP_RCB : 1;
		UINT32           PCIE_CAP_LINK_DISABLE : 1;
		UINT32           PCIE_CAP_RETRAIN_LINK : 1;
		UINT32      PCIE_CAP_COMMON_CLK_CONFIG : 1;
		UINT32         PCIE_CAP_EXTENDED_SYNCH : 1;
		UINT32       PCIE_CAP_EN_CLK_POWER_MAN : 1;
		UINT32  PCIE_CAP_HW_AUTO_WIDTH_DISABLE : 1;
		UINT32     PCIE_CAP_LINK_BW_MAN_INT_EN : 1;
		UINT32    PCIE_CAP_LINK_AUTO_BW_INT_EN : 1;
		UINT32                                 : 2;
		UINT32  PCIE_CAP_DRS_SIGNALING_CONTROL : 2;
		UINT32             PCIE_CAP_LINK_SPEED : 4;
		UINT32        PCIE_CAP_NEGO_LINK_WIDTH : 6;
		UINT32                        RSVDP_26 : 1;
		UINT32          PCIE_CAP_LINK_TRAINING : 1;
		UINT32        PCIE_CAP_SLOT_CLK_CONFIG : 1;
		UINT32             PCIE_CAP_DLL_ACTIVE : 1;
		UINT32     PCIE_CAP_LINK_BW_MAN_STATUS : 1;
		UINT32    PCIE_CAP_LINK_AUTO_BW_STATUS : 1;
	} Field;
	UINT32	Value;
} LINK_CONTROL_LINK_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32 PCIE_CAP_ATTENTION_INDICATOR_BUTTON : 1;
		UINT32       PCIE_CAP_POWER_CONTROLLER : 1;
		UINT32             PCIE_CAP_MRL_SENSOR : 1;
		UINT32    PCIE_CAP_ATTENTION_INDICATOR : 1;
		UINT32        PCIE_CAP_POWER_INDICATOR : 1;
		UINT32      PCIE_CAP_HOT_PLUG_SURPRISE : 1;
		UINT32       PCIE_CAP_HOT_PLUG_CAPABLE : 1;
		UINT32 PCIE_CAP_SLOT_POWER_LIMIT_VALUE : 8;
		UINT32 PCIE_CAP_SLOT_POWER_LIMIT_SCALE : 2;
		UINT32  PCIE_CAP_ELECTROMECH_INTERLOCK : 1;
		UINT32     PCIE_CAP_NO_CMD_CPL_SUPPORT : 1;
		UINT32           PCIE_CAP_PHY_SLOT_NUM : 13;
	} Field;
	UINT32	Value;
} SLOT_CAPABILITIES_REG_REGISTER;


typedef union { 
	struct {
		UINT32 PCIE_CAP_ATTENTION_BUTTON_PRESSED_EN : 1;
		UINT32 PCIE_CAP_POWER_FAULT_DETECTED_EN : 1;
		UINT32  PCIE_CAP_MRL_SENSOR_CHANGED_EN : 1;
		UINT32 PCIE_CAP_PRESENCE_DETECT_CHANGE_EN : 1;
		UINT32         PCIE_CAP_CMD_CPL_INT_EN : 1;
		UINT32        PCIE_CAP_HOT_PLUG_INT_EN : 1;
		UINT32 PCIE_CAP_ATTENTION_INDICATOR_CTRL : 2;
		UINT32   PCIE_CAP_POWER_INDICATOR_CTRL : 2;
		UINT32  PCIE_CAP_POWER_CONTROLLER_CTRL : 1;
		UINT32 PCIE_CAP_ELECTROMECH_INTERLOCK_CTRL : 1;
		UINT32   PCIE_CAP_DLL_STATE_CHANGED_EN : 1;
		UINT32  PCIE_CAP_AUTO_SLOT_PWR_LMT_DIS : 1;
		UINT32                        RSVDP_14 : 2;
		UINT32 PCIE_CAP_ATTENTION_BUTTON_PRESSED : 1;
		UINT32   PCIE_CAP_POWER_FAULT_DETECTED : 1;
		UINT32     PCIE_CAP_MRL_SENSOR_CHANGED : 1;
		UINT32 PCIE_CAP_PRESENCE_DETECTED_CHANGED : 1;
		UINT32               PCIE_CAP_CMD_CPLD : 1;
		UINT32       PCIE_CAP_MRL_SENSOR_STATE : 1;
		UINT32  PCIE_CAP_PRESENCE_DETECT_STATE : 1;
		UINT32 PCIE_CAP_ELECTROMECH_INTERLOCK_STATUS : 1;
		UINT32      PCIE_CAP_DLL_STATE_CHANGED : 1;
		UINT32                        RSVDP_25 : 7;
	} Field;
	UINT32	Value;
} SLOT_CONTROL_SLOT_STATUS_REGISTER;


typedef union { 
	struct {
		UINT32 PCIE_CAP_SYS_ERR_ON_CORR_ERR_EN : 1;
		UINT32 PCIE_CAP_SYS_ERR_ON_NON_FATAL_ERR_EN : 1;
		UINT32 PCIE_CAP_SYS_ERR_ON_FATAL_ERR_EN : 1;
		UINT32             PCIE_CAP_PME_INT_EN : 1;
		UINT32   PCIE_CAP_CRS_SW_VISIBILITY_EN : 1;
		UINT32                         RSVDP_5 : 11;
		UINT32      PCIE_CAP_CRS_SW_VISIBILITY : 1;
		UINT32                        RSVDP_17 : 15;
	} Field;
	UINT32	Value;
} ROOT_CONTROL_ROOT_CAPABILITIES_REG_REGISTER;


typedef union { 
	struct {
		UINT32             PCIE_CAP_PME_REQ_ID : 16;
		UINT32             PCIE_CAP_PME_STATUS : 1;
		UINT32            PCIE_CAP_PME_PENDING : 1;
		UINT32                        RSVDP_18 : 14;
	} Field;
	UINT32	Value;
} ROOT_STATUS_REG_REGISTER;


typedef union { 
	struct {
		UINT32      PCIE_CAP_CPL_TIMEOUT_RANGE : 4;
		UINT32 PCIE_CAP_CPL_TIMEOUT_DISABLE_SUPPORT : 1;
		UINT32    PCIE_CAP_ARI_FORWARD_SUPPORT : 1;
		UINT32    PCIE_CAP_ATOMIC_ROUTING_SUPP : 1;
		UINT32     PCIE_CAP_32_ATOMIC_CPL_SUPP : 1;
		UINT32     PCIE_CAP_64_ATOMIC_CPL_SUPP : 1;
		UINT32       PCIE_CAP_128_CAS_CPL_SUPP : 1;
		UINT32     PCIE_CAP_NO_RO_EN_PR2PR_PAR : 1;
		UINT32               PCIE_CAP_LTR_SUPP : 1;
		UINT32    PCIE_CAP_TPH_CMPLT_SUPPORT_0 : 1;
		UINT32    PCIE_CAP_TPH_CMPLT_SUPPORT_1 : 1;
		UINT32                                 : 2;
		UINT32 PCIE_CAP2_10_BIT_TAG_COMP_SUPPORT : 1;
		UINT32 PCIE_CAP2_10_BIT_TAG_REQ_SUPPORT : 1;
		UINT32           PCIE_CAP_OBFF_SUPPORT : 2;
		UINT32 PCIE_CAP2_CFG_EXTND_FMT_SUPPORT : 1;
		UINT32 PCIE_CAP2_CFG_END2END_TLP_PRFX_SUPPORT : 1;
		UINT32 PCIE_CAP2_CFG_MAX_END2END_TLP_PRFXS : 2;
		UINT32                                 : 3;
		UINT32                        RSVDP_27 : 1;
		UINT32          PCIE_CAP_DMWR_CPL_SUPP : 1;
		UINT32          PCIE_CAP_DMWR_LEN_SUPP : 2;
		UINT32                                 : 1;
	} Field;
	UINT32	Value;
} DEVICE_CAPABILITIES2_REG_REGISTER;


typedef union { 
	struct {
		UINT32      PCIE_CAP_CPL_TIMEOUT_VALUE : 4;
		UINT32    PCIE_CAP_CPL_TIMEOUT_DISABLE : 1;
		UINT32 PCIE_CAP_ARI_FORWARD_SUPPORT_CS : 1;
		UINT32          PCIE_CAP_ATOMIC_REQ_EN : 1;
		UINT32      PCIE_CAP_ATOMIC_EGRESS_BLK : 1;
		UINT32             PCIE_CAP_IDO_REQ_EN : 1;
		UINT32             PCIE_CAP_IDO_CPL_EN : 1;
		UINT32                 PCIE_CAP_LTR_EN : 1;
		UINT32                                 : 2;
		UINT32                PCIE_CAP_OBFF_EN : 2;
		UINT32 PCIE_CTRL2_CFG_END2END_TLP_PFX_BLCK : 1;
		UINT32                        RSVDP_16 : 16;
	} Field;
	UINT32	Value;
} DEVICE_CONTROL2_DEVICE_STATUS2_REG_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 1;
		UINT32 PCIE_CAP_SUPPORT_LINK_SPEED_VECTOR : 7;
		UINT32     PCIE_CAP_CROSS_LINK_SUPPORT : 1;
		UINT32     PCIE_CAP_LWR_SKP_OS_GEN_SUP : 7;
		UINT32     PCIE_CAP_LWR_SKP_OS_RCV_SUP : 7;
		UINT32 PCIE_CAP_RETIMER_PRE_DET_SUPPORT : 1;
		UINT32 PCIE_CAP_TWO_RETIMERS_PRE_DET_SUPPORT : 1;
		UINT32                        RSVDP_25 : 6;
		UINT32                                 : 1;
	} Field;
	UINT32	Value;
} LINK_CAPABILITIES2_REG_REGISTER;


typedef union { 
	struct {
		UINT32      PCIE_CAP_TARGET_LINK_SPEED : 4;
		UINT32       PCIE_CAP_ENTER_COMPLIANCE : 1;
		UINT32  PCIE_CAP_HW_AUTO_SPEED_DISABLE : 1;
		UINT32         PCIE_CAP_SEL_DEEMPHASIS : 1;
		UINT32              PCIE_CAP_TX_MARGIN : 3;
		UINT32 PCIE_CAP_ENTER_MODIFIED_COMPLIANCE : 1;
		UINT32         PCIE_CAP_COMPLIANCE_SOS : 1;
		UINT32      PCIE_CAP_COMPLIANCE_PRESET : 4;
		UINT32        PCIE_CAP_CURR_DEEMPHASIS : 1;
		UINT32                 PCIE_CAP_EQ_CPL : 1;
		UINT32              PCIE_CAP_EQ_CPL_P1 : 1;
		UINT32              PCIE_CAP_EQ_CPL_P2 : 1;
		UINT32              PCIE_CAP_EQ_CPL_P3 : 1;
		UINT32            PCIE_CAP_LINK_EQ_REQ : 1;
		UINT32        PCIE_CAP_RETIMER_PRE_DET : 1;
		UINT32   PCIE_CAP_TWO_RETIMERS_PRE_DET : 1;
		UINT32   PCIE_CAP_CROSSLINK_RESOLUTION : 2;
		UINT32                                 : 1;
		UINT32                        RSVDP_27 : 1;
		UINT32                                 : 4;
	} Field;
	UINT32	Value;
} LINK_CONTROL2_LINK_STATUS2_REG_REGISTER;


typedef union { 
	struct {
		UINT32                          CAP_ID : 16;
		UINT32                     CAP_VERSION : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} AER_EXT_CAP_HDR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 4;
		UINT32          DL_PROTOCOL_ERR_STATUS : 1;
		UINT32        SURPRISE_DOWN_ERR_STATUS : 1;
		UINT32                         RSVDP_6 : 6;
		UINT32             POIS_TLP_ERR_STATUS : 1;
		UINT32          FC_PROTOCOL_ERR_STATUS : 1;
		UINT32        CMPLT_TIMEOUT_ERR_STATUS : 1;
		UINT32          CMPLT_ABORT_ERR_STATUS : 1;
		UINT32          UNEXP_CMPLT_ERR_STATUS : 1;
		UINT32         REC_OVERFLOW_ERR_STATUS : 1;
		UINT32             MALF_TLP_ERR_STATUS : 1;
		UINT32                 ECRC_ERR_STATUS : 1;
		UINT32      UNSUPPORTED_REQ_ERR_STATUS : 1;
		UINT32            ACS_VIOLATION_STATUS : 1;
		UINT32             INTERNAL_ERR_STATUS : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32 ATOMIC_EGRESS_BLOCKED_ERR_STATUS : 1;
		UINT32     TLP_PRFX_BLOCKED_ERR_STATUS : 1;
		UINT32 PSND_TLP_EGRSS_BLOCKED_ERR_STATUS : 1;
		UINT32                                 : 4;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} UNCORR_ERR_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 4;
		UINT32            DL_PROTOCOL_ERR_MASK : 1;
		UINT32          SURPRISE_DOWN_ERR_MASK : 1;
		UINT32                         RSVDP_6 : 6;
		UINT32               POIS_TLP_ERR_MASK : 1;
		UINT32            FC_PROTOCOL_ERR_MASK : 1;
		UINT32          CMPLT_TIMEOUT_ERR_MASK : 1;
		UINT32            CMPLT_ABORT_ERR_MASK : 1;
		UINT32            UNEXP_CMPLT_ERR_MASK : 1;
		UINT32           REC_OVERFLOW_ERR_MASK : 1;
		UINT32               MALF_TLP_ERR_MASK : 1;
		UINT32                   ECRC_ERR_MASK : 1;
		UINT32        UNSUPPORTED_REQ_ERR_MASK : 1;
		UINT32              ACS_VIOLATION_MASK : 1;
		UINT32               INTERNAL_ERR_MASK : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32  ATOMIC_EGRESS_BLOCKED_ERR_MASK : 1;
		UINT32       TLP_PRFX_BLOCKED_ERR_MASK : 1;
		UINT32 PSND_TLP_EGRSS_BLOCKED_ERR_MASK : 1;
		UINT32                                 : 4;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} UNCORR_ERR_MASK_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 4;
		UINT32        DL_PROTOCOL_ERR_SEVERITY : 1;
		UINT32        SURPRISE_DOWN_ERR_SVRITY : 1;
		UINT32                         RSVDP_6 : 6;
		UINT32           POIS_TLP_ERR_SEVERITY : 1;
		UINT32        FC_PROTOCOL_ERR_SEVERITY : 1;
		UINT32      CMPLT_TIMEOUT_ERR_SEVERITY : 1;
		UINT32        CMPLT_ABORT_ERR_SEVERITY : 1;
		UINT32        UNEXP_CMPLT_ERR_SEVERITY : 1;
		UINT32       REC_OVERFLOW_ERR_SEVERITY : 1;
		UINT32           MALF_TLP_ERR_SEVERITY : 1;
		UINT32               ECRC_ERR_SEVERITY : 1;
		UINT32    UNSUPPORTED_REQ_ERR_SEVERITY : 1;
		UINT32          ACS_VIOLATION_SEVERITY : 1;
		UINT32           INTERNAL_ERR_SEVERITY : 1;
		UINT32                        RSVDP_23 : 1;
		UINT32 ATOMIC_EGRESS_BLOCKED_ERR_SEVERITY : 1;
		UINT32   TLP_PRFX_BLOCKED_ERR_SEVERITY : 1;
		UINT32 PSND_TLP_EGRSS_BLOCKED_ERR_SEVERITY : 1;
		UINT32                                 : 4;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} UNCORR_ERR_SEV_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                   RX_ERR_STATUS : 1;
		UINT32                         RSVDP_1 : 5;
		UINT32                  BAD_TLP_STATUS : 1;
		UINT32                 BAD_DLLP_STATUS : 1;
		UINT32       REPLAY_NO_ROLEOVER_STATUS : 1;
		UINT32                         RSVDP_9 : 3;
		UINT32        RPL_TIMER_TIMEOUT_STATUS : 1;
		UINT32   ADVISORY_NON_FATAL_ERR_STATUS : 1;
		UINT32        CORRECTED_INT_ERR_STATUS : 1;
		UINT32      HEADER_LOG_OVERFLOW_STATUS : 1;
		UINT32                        RSVDP_16 : 16;
	} Field;
	UINT32	Value;
} CORR_ERR_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                     RX_ERR_MASK : 1;
		UINT32                         RSVDP_1 : 5;
		UINT32                    BAD_TLP_MASK : 1;
		UINT32                   BAD_DLLP_MASK : 1;
		UINT32         REPLAY_NO_ROLEOVER_MASK : 1;
		UINT32                         RSVDP_9 : 3;
		UINT32          RPL_TIMER_TIMEOUT_MASK : 1;
		UINT32     ADVISORY_NON_FATAL_ERR_MASK : 1;
		UINT32          CORRECTED_INT_ERR_MASK : 1;
		UINT32        HEADER_LOG_OVERFLOW_MASK : 1;
		UINT32                        RSVDP_16 : 16;
	} Field;
	UINT32	Value;
} CORR_ERR_MASK_OFF_REGISTER;


typedef union { 
	struct {
		UINT32               FIRST_ERR_POINTER : 5;
		UINT32                    ECRC_GEN_CAP : 1;
		UINT32                     ECRC_GEN_EN : 1;
		UINT32                  ECRC_CHECK_CAP : 1;
		UINT32                   ECRC_CHECK_EN : 1;
		UINT32             MULTIPLE_HEADER_CAP : 1;
		UINT32              MULTIPLE_HEADER_EN : 1;
		UINT32            TLP_PRFX_LOG_PRESENT : 1;
		UINT32            CTO_PRFX_HDR_LOG_CAP : 1;
		UINT32                        RSVDP_13 : 19;
	} Field;
	UINT32	Value;
} ADV_ERR_CAP_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32          FIRST_DWORD_FIRST_BYTE : 8;
		UINT32         FIRST_DWORD_SECOND_BYTE : 8;
		UINT32          FIRST_DWORD_THIRD_BYTE : 8;
		UINT32         FIRST_DWORD_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} HDR_LOG_0_OFF_REGISTER;


typedef union { 
	struct {
		UINT32         SECOND_DWORD_FIRST_BYTE : 8;
		UINT32        SECOND_DWORD_SECOND_BYTE : 8;
		UINT32         SECOND_DWORD_THIRD_BYTE : 8;
		UINT32        SECOND_DWORD_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} HDR_LOG_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32          THIRD_DWORD_FIRST_BYTE : 8;
		UINT32         THIRD_DWORD_SECOND_BYTE : 8;
		UINT32          THIRD_DWORD_THIRD_BYTE : 8;
		UINT32         THIRD_DWORD_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} HDR_LOG_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32         FOURTH_DWORD_FIRST_BYTE : 8;
		UINT32        FOURTH_DWORD_SECOND_BYTE : 8;
		UINT32         FOURTH_DWORD_THIRD_BYTE : 8;
		UINT32        FOURTH_DWORD_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} HDR_LOG_3_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           CORR_ERR_REPORTING_EN : 1;
		UINT32      NON_FATAL_ERR_REPORTING_EN : 1;
		UINT32          FATAL_ERR_REPORTING_EN : 1;
		UINT32                         RSVDP_3 : 29;
	} Field;
	UINT32	Value;
} ROOT_ERR_CMD_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                      ERR_COR_RX : 1;
		UINT32                  MUL_ERR_COR_RX : 1;
		UINT32          ERR_FATAL_NON_FATAL_RX : 1;
		UINT32      MUL_ERR_FATAL_NON_FATAL_RX : 1;
		UINT32              FIRST_UNCORR_FATAL : 1;
		UINT32            NON_FATAL_ERR_MSG_RX : 1;
		UINT32                FATAL_ERR_MSG_RX : 1;
		UINT32                         RSVDP_7 : 20;
		UINT32             ADV_ERR_INT_MSG_NUM : 5;
	} Field;
	UINT32	Value;
} ROOT_ERR_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32               ERR_COR_SOURCE_ID : 16;
		UINT32   ERR_FATAL_NON_FATAL_SOURCE_ID : 16;
	} Field;
	UINT32	Value;
} ERR_SRC_ID_OFF_REGISTER;


typedef union { 
	struct {
		UINT32    CFG_TLP_PFX_LOG_1_FIRST_BYTE : 8;
		UINT32   CFG_TLP_PFX_LOG_1_SECOND_BYTE : 8;
		UINT32    CFG_TLP_PFX_LOG_1_THIRD_BYTE : 8;
		UINT32   CFG_TLP_PFX_LOG_1_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} TLP_PREFIX_LOG_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32    CFG_TLP_PFX_LOG_2_FIRST_BYTE : 8;
		UINT32   CFG_TLP_PFX_LOG_2_SECOND_BYTE : 8;
		UINT32    CFG_TLP_PFX_LOG_2_THIRD_BYTE : 8;
		UINT32   CFG_TLP_PFX_LOG_2_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} TLP_PREFIX_LOG_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32    CFG_TLP_PFX_LOG_3_FIRST_BYTE : 8;
		UINT32   CFG_TLP_PFX_LOG_3_SECOND_BYTE : 8;
		UINT32    CFG_TLP_PFX_LOG_3_THIRD_BYTE : 8;
		UINT32   CFG_TLP_PFX_LOG_3_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} TLP_PREFIX_LOG_3_OFF_REGISTER;


typedef union { 
	struct {
		UINT32    CFG_TLP_PFX_LOG_4_FIRST_BYTE : 8;
		UINT32   CFG_TLP_PFX_LOG_4_SECOND_BYTE : 8;
		UINT32    CFG_TLP_PFX_LOG_4_THIRD_BYTE : 8;
		UINT32   CFG_TLP_PFX_LOG_4_FOURTH_BYTE : 8;
	} Field;
	UINT32	Value;
} TLP_PREFIX_LOG_4_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                 PCIE_EXT_CAP_ID : 16;
		UINT32                 TPH_REQ_CAP_VER : 4;
		UINT32                TPH_REQ_NEXT_PTR : 12;
	} Field;
	UINT32	Value;
} TPH_EXT_CAP_HDR_REG_REGISTER;


typedef union { 
	struct {
		UINT32              TPH_REQ_NO_ST_MODE : 1;
		UINT32             TPH_REQ_CAP_INT_VEC : 1;
		UINT32             TPH_REQ_DEVICE_SPEC : 1;
		UINT32                         RSVDP_3 : 5;
		UINT32            TPH_REQ_EXTENDED_TPH : 1;
		UINT32      TPH_REQ_CAP_ST_TABLE_LOC_0 : 1;
		UINT32      TPH_REQ_CAP_ST_TABLE_LOC_1 : 1;
		UINT32                        RSVDP_11 : 5;
		UINT32       TPH_REQ_CAP_ST_TABLE_SIZE : 11;
		UINT32                        RSVDP_27 : 5;
	} Field;
	UINT32	Value;
} TPH_REQ_CAP_REG_REG_REGISTER;


typedef union { 
	struct {
		UINT32          TPH_REQ_ST_MODE_SELECT : 3;
		UINT32                         RSVDP_3 : 5;
		UINT32             TPH_REQ_CTRL_REQ_EN : 2;
		UINT32                        RSVDP_10 : 22;
	} Field;
	UINT32	Value;
} TPH_REQ_CONTROL_REG_REG_REGISTER;


typedef union { 
	struct {
		UINT32                  PCI_MSI_CAP_ID : 8;
		UINT32         PCI_MSI_CAP_NEXT_OFFSET : 8;
		UINT32                  PCI_MSI_ENABLE : 1;
		UINT32        PCI_MSI_MULTIPLE_MSG_CAP : 3;
		UINT32         PCI_MSI_MULTIPLE_MSG_EN : 3;
		UINT32         PCI_MSI_64_BIT_ADDR_CAP : 1;
		UINT32                 PCI_PVM_SUPPORT : 1;
		UINT32            PCI_MSI_EXT_DATA_CAP : 1;
		UINT32             PCI_MSI_EXT_DATA_EN : 1;
		UINT32                        RSVDP_27 : 5;
	} Field;
	UINT32	Value;
} PCI_MSI_CAP_ID_NEXT_CTRL_REG_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 2;
		UINT32             PCI_MSI_CAP_OFF_04H : 30;
	} Field;
	UINT32	Value;
} MSI_CAP_OFF_04H_REG_REGISTER;


typedef union { 
	struct {
		UINT32             PCI_MSI_CAP_OFF_08H : 16;
		UINT32             PCI_MSI_CAP_OFF_0AH : 16;
	} Field;
	UINT32	Value;
} MSI_CAP_OFF_08H_REG_REGISTER;


typedef union { 
	struct {
		UINT32             PCI_MSI_CAP_OFF_0CH : 16;
		UINT32             PCI_MSI_CAP_OFF_0EH : 16;
	} Field;
	UINT32	Value;
} MSI_CAP_OFF_0CH_REG_REGISTER;


typedef union { 
	struct {
		UINT32                          CAP_ID : 16;
		UINT32                     CAP_VERSION : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} ATS_CAP_HDR_REG_REGISTER;


typedef union { 
	struct {
		UINT32              INVALIDATE_Q_DEPTH : 5;
		UINT32                PAGE_ALIGNED_REQ : 1;
		UINT32             GLOBAL_INVAL_SPPRTD : 1;
		UINT32                         RSVDP_7 : 9;
		UINT32                             STU : 5;
		UINT32                        RSVDP_21 : 10;
		UINT32                          ENABLE : 1;
	} Field;
	UINT32	Value;
} ATS_CAPABILITIES_CTRL_REG_REGISTER;


typedef union { 
	struct {
		UINT32                          CAP_ID : 16;
		UINT32                     CAP_VERSION : 4;
		UINT32                     NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} ACS_CAP_HDR_REG_REGISTER;


typedef union { 
	struct {
		UINT32                   ACS_SRC_VALID : 1;
		UINT32                    ACS_AT_BLOCK : 1;
		UINT32            ACS_P2P_REQ_REDIRECT : 1;
		UINT32            ACS_P2P_CPL_REDIRECT : 1;
		UINT32              ACS_USP_FORWARDING : 1;
		UINT32          ACS_P2P_EGRESS_CONTROL : 1;
		UINT32       ACS_DIRECT_TRANSLATED_P2P : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32            ACS_EGRESS_CTRL_SIZE : 8;
		UINT32                ACS_SRC_VALID_EN : 1;
		UINT32                 ACS_AT_BLOCK_EN : 1;
		UINT32         ACS_P2P_REQ_REDIRECT_EN : 1;
		UINT32         ACS_P2P_CPL_REDIRECT_EN : 1;
		UINT32           ACS_USP_FORWARDING_EN : 1;
		UINT32       ACS_P2P_EGRESS_CONTROL_EN : 1;
		UINT32    ACS_DIRECT_TRANSLATED_P2P_EN : 1;
		UINT32                        RSVDP_23 : 9;
	} Field;
	UINT32	Value;
} ACS_CAPABILITIES_CTRL_REG_REGISTER;


typedef union { 
	struct {
		UINT32                DLINK_EXT_CAP_ID : 16;
		UINT32               DLINK_CAP_VERSION : 4;
		UINT32               DLINK_NEXT_OFFSET : 12;
	} Field;
	UINT32	Value;
} DATA_LINK_FEATURE_EXT_HDR_OFF_REGISTER;


typedef union { 
	struct {
		UINT32      SCALED_FLOW_CNTL_SUPPORTED : 1;
		UINT32      FUTURE_FEATURE_SUPPORTED_1 : 4;
		UINT32              LCL_L0P_EXIT_LTNCY : 3;
		UINT32        FUTURE_FEATURE_SUPPORTED : 15;
		UINT32                        RSVDP_23 : 8;
		UINT32          DL_FEATURE_EXCHANGE_EN : 1;
	} Field;
	UINT32	Value;
} DATA_LINK_FEATURE_CAP_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 REMOTE_DATA_LINK_FEATURE_SUPPORTED : 5;
		UINT32           REMOTE_L0P_EXIT_LTNCY : 3;
		UINT32                 REMOTE_RESERVED : 15;
		UINT32                        RSVDP_23 : 8;
		UINT32  DATA_LINK_FEATURE_STATUS_VALID : 1;
	} Field;
	UINT32	Value;
} DATA_LINK_FEATURE_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32   ROUND_TRIP_LATENCY_TIME_LIMIT : 16;
		UINT32               REPLAY_TIME_LIMIT : 16;
	} Field;
	UINT32	Value;
} ACK_LATENCY_TIMER_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                VENDOR_SPEC_DLLP : 32;
	} Field;
	UINT32	Value;
} VENDOR_SPEC_DLLP_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                        LINK_NUM : 8;
		UINT32                    FORCED_LTSSM : 4;
		UINT32                        RSVDP_12 : 3;
		UINT32                        FORCE_EN : 1;
		UINT32                      LINK_STATE : 6;
		UINT32    SUPPORT_PART_LANES_RXEI_EXIT : 1;
		UINT32              DO_DESKEW_FOR_SRIS : 1;
		UINT32                        RSVDP_24 : 8;
	} Field;
	UINT32	Value;
} PORT_FORCE_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                        ACK_FREQ : 8;
		UINT32                       ACK_N_FTS : 8;
		UINT32                COMMON_CLK_N_FTS : 8;
		UINT32            L0S_ENTRANCE_LATENCY : 3;
		UINT32             L1_ENTRANCE_LATENCY : 3;
		UINT32                      ENTER_ASPM : 1;
		UINT32            ASPM_L1_TIMER_ENABLE : 1;
	} Field;
	UINT32	Value;
} ACK_F_ASPM_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32        VENDOR_SPECIFIC_DLLP_REQ : 1;
		UINT32                SCRAMBLE_DISABLE : 1;
		UINT32                 LOOPBACK_ENABLE : 1;
		UINT32                    RESET_ASSERT : 1;
		UINT32                         RSVDP_4 : 1;
		UINT32                     DLL_LINK_EN : 1;
		UINT32                    LINK_DISABLE : 1;
		UINT32                  FAST_LINK_MODE : 1;
		UINT32                       LINK_RATE : 4;
		UINT32                        RSVDP_12 : 4;
		UINT32                    LINK_CAPABLE : 6;
		UINT32                                 : 2;
		UINT32                   BEACON_ENABLE : 1;
		UINT32             CORRUPT_LCRC_ENABLE : 1;
		UINT32                  EXTENDED_SYNCH : 1;
		UINT32  TRANSMIT_LANE_REVERSALE_ENABLE : 1;
		UINT32                        RSVDP_28 : 4;
	} Field;
	UINT32	Value;
} PORT_LINK_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                INSERT_LANE_SKEW : 24;
		UINT32               FLOW_CTRL_DISABLE : 1;
		UINT32                 ACK_NAK_DISABLE : 1;
		UINT32             ELASTIC_BUFFER_MODE : 1;
		UINT32             IMPLEMENT_NUM_LANES : 4;
		UINT32     DISABLE_LANE_TO_LANE_DESKEW : 1;
	} Field;
	UINT32	Value;
} LANE_SKEW_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                    MAX_FUNC_NUM : 8;
		UINT32                         RSVDP_8 : 6;
		UINT32          TIMER_MOD_REPLAY_TIMER : 5;
		UINT32               TIMER_MOD_ACK_NAK : 5;
		UINT32               UPDATE_FREQ_TIMER : 5;
		UINT32        FAST_LINK_SCALING_FACTOR : 2;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} TIMER_CTRL_MAX_FUNC_NUM_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                     SKP_INT_VAL : 11;
		UINT32                     EIDLE_TIMER : 4;
		UINT32             DISABLE_FC_WD_TIMER : 1;
		UINT32                     MASK_RADM_1 : 16;
	} Field;
	UINT32	Value;
} SYMBOL_TIMER_FILTER_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                     MASK_RADM_2 : 32;
	} Field;
	UINT32	Value;
} FILTER_MASK_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                       DEB_REG_0 : 32;
	} Field;
	UINT32	Value;
} PL_DEBUG0_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                       DEB_REG_1 : 32;
	} Field;
	UINT32	Value;
} PL_DEBUG1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32             TX_P_DATA_FC_CREDIT : 16;
		UINT32           TX_P_HEADER_FC_CREDIT : 12;
		UINT32     RSVDP_TX_P_FC_CREDIT_STATUS : 4;
	} Field;
	UINT32	Value;
} TX_P_FC_CREDIT_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32            TX_NP_DATA_FC_CREDIT : 16;
		UINT32          TX_NP_HEADER_FC_CREDIT : 12;
		UINT32    RSVDP_TX_NP_FC_CREDIT_STATUS : 4;
	} Field;
	UINT32	Value;
} TX_NP_FC_CREDIT_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           TX_CPL_DATA_FC_CREDIT : 16;
		UINT32         TX_CPL_HEADER_FC_CREDIT : 12;
		UINT32   RSVDP_TX_CPL_FC_CREDIT_STATUS : 4;
	} Field;
	UINT32	Value;
} TX_CPL_FC_CREDIT_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32     RX_TLP_FC_CREDIT_NON_RETURN : 1;
		UINT32              TX_RETRY_BUFFER_NE : 1;
		UINT32              RX_QUEUE_NON_EMPTY : 1;
		UINT32               RX_QUEUE_OVERFLOW : 1;
		UINT32                         RSVDP_4 : 9;
		UINT32    RX_SERIALIZATION_Q_NON_EMPTY : 1;
		UINT32    RX_SERIALIZATION_Q_WRITE_ERR : 1;
		UINT32     RX_SERIALIZATION_Q_READ_ERR : 1;
		UINT32          TIMER_MOD_FLOW_CONTROL : 13;
		UINT32                        RSVDP_29 : 2;
		UINT32       TIMER_MOD_FLOW_CONTROL_EN : 1;
	} Field;
	UINT32	Value;
} QUEUE_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                 WRR_WEIGHT_VC_0 : 8;
		UINT32                 WRR_WEIGHT_VC_1 : 8;
		UINT32                 WRR_WEIGHT_VC_2 : 8;
		UINT32                 WRR_WEIGHT_VC_3 : 8;
	} Field;
	UINT32	Value;
} VC_TX_ARBI_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                 WRR_WEIGHT_VC_4 : 8;
		UINT32                 WRR_WEIGHT_VC_5 : 8;
		UINT32                 WRR_WEIGHT_VC_6 : 8;
		UINT32                 WRR_WEIGHT_VC_7 : 8;
	} Field;
	UINT32	Value;
} VC_TX_ARBI_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32               VC0_P_DATA_CREDIT : 12;
		UINT32             VC0_P_HEADER_CREDIT : 8;
		UINT32                       RESERVED4 : 1;
		UINT32                VC0_P_TLP_Q_MODE : 3;
		UINT32                 VC0_P_HDR_SCALE : 2;
		UINT32                VC0_P_DATA_SCALE : 2;
		UINT32                       RESERVED5 : 2;
		UINT32           TLP_TYPE_ORDERING_VC0 : 1;
		UINT32                VC_ORDERING_RX_Q : 1;
	} Field;
	UINT32	Value;
} VC0_P_RX_Q_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32              VC0_NP_DATA_CREDIT : 12;
		UINT32            VC0_NP_HEADER_CREDIT : 8;
		UINT32                       RESERVED6 : 1;
		UINT32               VC0_NP_TLP_Q_MODE : 3;
		UINT32                VC0_NP_HDR_SCALE : 2;
		UINT32               VC0_NP_DATA_SCALE : 2;
		UINT32                       RESERVED7 : 4;
	} Field;
	UINT32	Value;
} VC0_NP_RX_Q_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32             VC0_CPL_DATA_CREDIT : 12;
		UINT32           VC0_CPL_HEADER_CREDIT : 8;
		UINT32                       RESERVED8 : 1;
		UINT32              VC0_CPL_TLP_Q_MODE : 3;
		UINT32               VC0_CPL_HDR_SCALE : 2;
		UINT32              VC0_CPL_DATA_SCALE : 2;
		UINT32                       RESERVED9 : 4;
	} Field;
	UINT32	Value;
} VC0_CPL_RX_Q_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32               FAST_TRAINING_SEQ : 8;
		UINT32                    NUM_OF_LANES : 5;
		UINT32                    PRE_DET_LANE : 3;
		UINT32          AUTO_LANE_FLIP_CTRL_EN : 1;
		UINT32             DIRECT_SPEED_CHANGE : 1;
		UINT32            CONFIG_PHY_TX_CHANGE : 1;
		UINT32               CONFIG_TX_COMP_RX : 1;
		UINT32                  SEL_DEEMPHASIS : 1;
		UINT32               GEN1_EI_INFERENCE : 1;
		UINT32           SELECT_DEEMPH_VAR_MUX : 1;
		UINT32       SELECTABLE_DEEMPH_BIT_MUX : 1;
		UINT32                 LANE_UNDER_TEST : 4;
		UINT32                 EQ_FOR_LOOPBACK : 1;
		UINT32 TX_MOD_CMPL_PATTERN_FOR_LOOPBACK : 1;
		UINT32                 FORCE_LANE_FLIP : 1;
		UINT32                  SUPPORT_MOD_TS : 1;
	} Field;
	UINT32	Value;
} GEN2_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                      PHY_STATUS : 32;
	} Field;
	UINT32	Value;
} PHY_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                     PHY_CONTROL : 32;
	} Field;
	UINT32	Value;
} PHY_CONTROL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                   TARGET_MAP_PF : 6;
		UINT32                  TARGET_MAP_ROM : 1;
		UINT32                                 : 6;
		UINT32       TARGET_MAP_RESERVED_13_15 : 3;
		UINT32                TARGET_MAP_INDEX : 5;
		UINT32       TARGET_MAP_RESERVED_21_31 : 11;
	} Field;
	UINT32	Value;
} TRGT_MAP_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32              RADM_CLK_GATING_EN : 1;
		UINT32                                 : 1;
		UINT32                         RSVDP_2 : 30;
	} Field;
	UINT32	Value;
} CLOCK_GATING_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32             GEN3_ZRXDC_NONCOMPL : 1;
		UINT32            NO_SEED_VALUE_CHANGE : 1;
		UINT32                         RSVDP_2 : 6;
		UINT32         DISABLE_SCRAMBLER_GEN_3 : 1;
		UINT32                    EQ_PHASE_2_3 : 1;
		UINT32                    EQ_EIEOS_CNT : 1;
		UINT32                         EQ_REDO : 1;
		UINT32                    RXEQ_PH01_EN : 1;
		UINT32              RXEQ_RGRDLESS_RXTS : 1;
		UINT32                        RSVDP_14 : 2;
		UINT32       GEN3_EQUALIZATION_DISABLE : 1;
		UINT32     GEN3_DLLP_XMT_DELAY_DISABLE : 1;
		UINT32         GEN3_DC_BALANCE_DISABLE : 1;
		UINT32                        RSVDP_19 : 2;
		UINT32                 AUTO_EQ_DISABLE : 1;
		UINT32     USP_SEND_8GT_EQ_TS2_DISABLE : 1;
		UINT32 GEN3_EQ_INVREQ_EVAL_DIFF_DISABLE : 1;
		UINT32                 RATE_SHADOW_SEL : 2;
		UINT32                        RSVDP_26 : 6;
	} Field;
	UINT32	Value;
} GEN3_RELATED_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                 GEN3_EQ_FB_MODE : 4;
		UINT32       GEN3_EQ_PHASE23_EXIT_MODE : 1;
		UINT32        GEN3_EQ_EVAL_2MS_DISABLE : 1;
		UINT32  GEN3_LOWER_RATE_EQ_REDO_ENABLE : 1;
		UINT32                         RSVDP_7 : 1;
		UINT32            GEN3_EQ_PSET_REQ_VEC : 16;
		UINT32    GEN3_EQ_FOM_INC_INITIAL_EVAL : 1;
		UINT32        GEN3_EQ_PSET_REQ_AS_COEF : 1;
		UINT32 GEN3_REQ_SEND_CONSEC_EIEOS_FOR_PSET_MAP : 1;
		UINT32                 GEN3_EQ_REQ_NUM : 3;
		UINT32  GEN3_SUPPORT_FINITE_EQ_REQUEST : 1;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} GEN3_EQ_CONTROL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32      GEN3_EQ_FMDC_T_MIN_PHASE23 : 5;
		UINT32            GEN3_EQ_FMDC_N_EVALS : 5;
		UINT32 GEN3_EQ_FMDC_MAX_PRE_CUSROR_DELTA : 4;
		UINT32 GEN3_EQ_FMDC_MAX_POST_CUSROR_DELTA : 4;
		UINT32                                 : 4;
		UINT32                        RSVDP_22 : 10;
	} Field;
	UINT32	Value;
} GEN3_EQ_FB_MODE_DIR_CHANGE_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                       NP_PASS_P : 8;
		UINT32                      CPL_PASS_P : 8;
		UINT32                        RSVDP_16 : 16;
	} Field;
	UINT32	Value;
} ORDER_RULE_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                    LPBK_RXVALID : 16;
		UINT32                   RXSTATUS_LANE : 6;
		UINT32                        RSVDP_22 : 2;
		UINT32                  RXSTATUS_VALUE : 3;
		UINT32                        RSVDP_27 : 4;
		UINT32                   PIPE_LOOPBACK : 1;
	} Field;
	UINT32	Value;
} PIPE_LOOPBACK_CONTROL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                    DBI_RO_WR_EN : 1;
		UINT32                  DEFAULT_TARGET : 1;
		UINT32              UR_CA_MASK_4_TRGT1 : 1;
		UINT32         SIMPLIFIED_REPLAY_TIMER : 1;
		UINT32        DISABLE_AUTO_LTR_CLR_MSG : 1;
		UINT32               ARI_DEVICE_NUMBER : 1;
		UINT32                     CPLQ_MNG_EN : 1;
		UINT32           CFG_TLP_BYPASS_EN_REG : 1;
		UINT32                CONFIG_LIMIT_REG : 10;
		UINT32   TARGET_ABOVE_CONFIG_LIMIT_REG : 2;
		UINT32            P2P_TRACK_CPL_TO_REG : 1;
		UINT32                P2P_ERR_RPT_CTRL : 1;
		UINT32           PORT_LOGIC_WR_DISABLE : 1;
		UINT32                RAS_REG_PF0_ONLY : 1;
		UINT32             RASDES_REG_PF0_ONLY : 1;
		UINT32              ERR_INJ_WR_DISABLE : 1;
		UINT32                                 : 3;
		UINT32                        RSVDP_29 : 3;
	} Field;
	UINT32	Value;
} MISC_CONTROL_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32               TARGET_LINK_WIDTH : 6;
		UINT32        DIRECT_LINK_WIDTH_CHANGE : 1;
		UINT32             UPCONFIGURE_SUPPORT : 1;
		UINT32 RELIABILITY_LINK_WIDTH_CHANGE_ENABLE : 1;
		UINT32                         RSVDP_9 : 23;
	} Field;
	UINT32	Value;
} MULTI_LANE_CONTROL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32               RXSTANDBY_CONTROL : 7;
		UINT32                         RSVDP_7 : 1;
		UINT32                                 : 1;
		UINT32                    L1_NOWAIT_P1 : 1;
		UINT32                      L1_CLK_SEL : 1;
		UINT32               P2NOBEACON_ENABLE : 1;
		UINT32                   PHY_RST_TIMER : 18;
		UINT32         PHY_PERST_ON_WARM_RESET : 1;
		UINT32                                 : 1;
	} Field;
	UINT32	Value;
} PHY_INTEROP_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                      LOOK_UP_ID : 31;
		UINT32                       DELETE_EN : 1;
	} Field;
	UINT32	Value;
} TRGT_CPL_LUT_DELETE_ENTRY_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                  VERSION_NUMBER : 32;
	} Field;
	UINT32	Value;
} PCIE_VERSION_NUMBER_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                    VERSION_TYPE : 32;
	} Field;
	UINT32	Value;
} PCIE_VERSION_TYPE_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         RSVDP_0 : 3;
		UINT32                  RC_DSW_DEV_NUM : 5;
		UINT32                  RC_DSW_BUS_NUM : 8;
		UINT32                        RSVDP_16 : 16;
	} Field;
	UINT32	Value;
} PL_APP_BUS_DEV_NUM_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32       PCIPM_VDM_TRAFFIC_BLOCKED : 1;
		UINT32   PCIPM_NEW_TLP_CLIENT0_BLOCKED : 1;
		UINT32   PCIPM_NEW_TLP_CLIENT1_BLOCKED : 1;
		UINT32   PCIPM_NEW_TLP_CLIENT2_BLOCKED : 1;
		UINT32              PCIPM_RESERVED_4_7 : 4;
		UINT32                         RSVDP_8 : 24;
	} Field;
	UINT32	Value;
} PCIPM_TRAFFIC_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32             SNOOP_LATENCY_VALUE : 10;
		UINT32             SNOOP_LATENCY_SCALE : 3;
		UINT32                        RSVDP_13 : 2;
		UINT32           SNOOP_LATENCY_REQUIRE : 1;
		UINT32          NO_SNOOP_LATENCY_VALUE : 10;
		UINT32          NO_SNOOP_LATENCY_SCALE : 3;
		UINT32                        RSVDP_29 : 2;
		UINT32        NO_SNOOP_LATENCY_REQUIRE : 1;
	} Field;
	UINT32	Value;
} PL_LTR_LATENCY_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                    AUX_CLK_FREQ : 10;
		UINT32                        RSVDP_10 : 22;
	} Field;
	UINT32	Value;
} AUX_CLK_FREQ_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                 POWERDOWN_FORCE : 1;
		UINT32             POWERDOWN_VMAIN_ACK : 1;
		UINT32                         RSVDP_2 : 2;
		UINT32         POWERDOWN_MAC_POWERDOWN : 4;
		UINT32         POWERDOWN_PHY_POWERDOWN : 4;
		UINT32                        RSVDP_12 : 20;
	} Field;
	UINT32	Value;
} POWERDOWN_CTRL_STATUS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32        PMA_PIPE_RST_DELAY_TIMER : 6;
		UINT32                         RSVDP_6 : 2;
		UINT32        DSP_PCIPM_L1_ENTER_DELAY : 4;
		UINT32                        RSVDP_12 : 20;
	} Field;
	UINT32	Value;
} PHY_INTEROP_CTRL_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 GEN4_MARGINING_NUM_TIMING_STEPS : 6;
		UINT32                         RSVDP_6 : 2;
		UINT32 GEN4_MARGINING_MAX_TIMING_OFFSET : 6;
		UINT32                        RSVDP_14 : 2;
		UINT32 GEN4_MARGINING_NUM_VOLTAGE_STEPS : 7;
		UINT32                        RSVDP_23 : 1;
		UINT32 GEN4_MARGINING_MAX_VOLTAGE_OFFSET : 6;
		UINT32                        RSVDP_30 : 2;
	} Field;
	UINT32	Value;
} GEN4_LANE_MARGINING_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 GEN4_MARGINING_SAMPLE_RATE_VOLTAGE : 6;
		UINT32                         RSVDP_6 : 2;
		UINT32 GEN4_MARGINING_SAMPLE_RATE_TIMING : 6;
		UINT32                        RSVDP_14 : 2;
		UINT32         GEN4_MARGINING_MAXLANES : 5;
		UINT32                        RSVDP_21 : 3;
		UINT32 GEN4_MARGINING_VOLTAGE_SUPPORTED : 1;
		UINT32 GEN4_MARGINING_IND_UP_DOWN_VOLTAGE : 1;
		UINT32 GEN4_MARGINING_IND_LEFT_RIGHT_TIMING : 1;
		UINT32 GEN4_MARGINING_SAMPLE_REPORTING_METHOD : 1;
		UINT32 GEN4_MARGINING_IND_ERROR_SAMPLER : 1;
		UINT32                        RSVDP_29 : 2;
		UINT32                                 : 1;
	} Field;
	UINT32	Value;
} GEN4_LANE_MARGINING_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 GEN5_MARGINING_NUM_TIMING_STEPS : 6;
		UINT32                         RSVDP_6 : 2;
		UINT32 GEN5_MARGINING_MAX_TIMING_OFFSET : 6;
		UINT32                        RSVDP_14 : 2;
		UINT32 GEN5_MARGINING_NUM_VOLTAGE_STEPS : 7;
		UINT32                        RSVDP_23 : 1;
		UINT32 GEN5_MARGINING_MAX_VOLTAGE_OFFSET : 6;
		UINT32                        RSVDP_30 : 2;
	} Field;
	UINT32	Value;
} GEN5_LANE_MARGINING_1_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 GEN5_MARGINING_SAMPLE_RATE_VOLTAGE : 6;
		UINT32                         RSVDP_6 : 2;
		UINT32 GEN5_MARGINING_SAMPLE_RATE_TIMING : 6;
		UINT32                        RSVDP_14 : 2;
		UINT32         GEN5_MARGINING_MAXLANES : 5;
		UINT32                        RSVDP_21 : 3;
		UINT32 GEN5_MARGINING_VOLTAGE_SUPPORTED : 1;
		UINT32 GEN5_MARGINING_IND_UP_DOWN_VOLTAGE : 1;
		UINT32 GEN5_MARGINING_IND_LEFT_RIGHT_TIMING : 1;
		UINT32 GEN5_MARGINING_SAMPLE_REPORTING_METHOD : 1;
		UINT32 GEN5_MARGINING_IND_ERROR_SAMPLER : 1;
		UINT32                        RSVDP_29 : 3;
	} Field;
	UINT32	Value;
} GEN5_LANE_MARGINING_2_OFF_REGISTER;


typedef union { 
	struct {
		UINT32 RX_MESSAGE_BUS_WRITE_BUFFER_DEPTH : 4;
		UINT32 TX_MESSAGE_BUS_MIN_WRITE_BUFFER_DEPTH : 4;
		UINT32          PIPE_GARBAGE_DATA_MODE : 1;
		UINT32                         RSVDP_9 : 23;
	} Field;
	UINT32	Value;
} PIPE_RELATED_OFF_REGISTER;


typedef union { 
	struct {
		UINT32           ALMOST_FULL_THRESHOLD : 16;
		UINT32   ALMOST_FULL_THRESHOLD_ADJ_VAL : 12;
		UINT32                        RSVDP_28 : 2;
		UINT32  ALMOST_FULL_THRESHOLD_ADJ_SIGN : 1;
		UINT32 RX_SERIALIZATION_OVFLW_PRVNTN_EN : 1;
	} Field;
	UINT32	Value;
} RX_SERIALIZATION_Q_CTRL_OFF_REGISTER;


typedef union { 
	struct {
		UINT32               CXL_IO_VLSM_STATE : 4;
		UINT32        CXL_CACHE_MEM_VLSM_STATE : 4;
		UINT32              CXL_VLSM_CSR_STATE : 3;
		UINT32              CXL_VLSM_TO_ACTIVE : 1;
		UINT32         CXL_VLSM_LP_ALMP_ENABLE : 1;
		UINT32                        RSVDP_13 : 11;
		UINT32           CXL_APN_BYPASS_ENABLE : 1;
		UINT32                                 : 1;
		UINT32          CXL_VLSM_IO_ALMP_ERROR : 3;
		UINT32          CXL_VLSM_MC_ALMP_ERROR : 3;
	} Field;
	UINT32	Value;
} CXL_VLSM_CSR_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32          CXL_LRSM_MUX_NUM_RETRY : 5;
		UINT32                         RSVDP_5 : 3;
		UINT32         CXL_LRSM_MUX_NUM_REINIT : 5;
		UINT32                        RSVDP_13 : 3;
		UINT32                  CXL_LRSM_STATE : 6;
		UINT32            CXL_LRSM_FATAL_ERROR : 1;
		UINT32                        RSVDP_23 : 9;
	} Field;
	UINT32	Value;
} CXL_LRSM_CSR_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32      CXL_LRSM_TIMEOUT_THRESHOLD : 16;
		UINT32               CXL_CRC_THRESHOLD : 10;
		UINT32                        RSVDP_26 : 2;
		UINT32                                 : 4;
	} Field;
	UINT32	Value;
} CXL_LL_CTRL_CR_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32   CXL_FATAL_ERR_RETRY_ACK_VIRAL : 1;
		UINT32                   CXL_RAS_ERROR : 1;
		UINT32           CXL_LL_STATIC_BIT_ERR : 1;
		UINT32                  CXL_LRSM_ABORT : 1;
		UINT32              CXL_LRSM_ACK_ERROR : 1;
		UINT32         CXL_LL_TC_FLOW_CTRL_ERR : 5;
		UINT32           CXL_LL_INIT_PARAM_ERR : 1;
		UINT32         CXL_LL_MULTI_INIT_PARAM : 1;
		UINT32         CXL_RETRY_ESEQ_OVERFLOW : 1;
		UINT32               CXL_RETRY_CRC_ERR : 1;
		UINT32         CXL_LL_RX_FLOW_CTRL_ERR : 5;
		UINT32                                 : 2;
		UINT32                CXL_AP_VIRAL_REQ : 1;
		UINT32                                 : 4;
		UINT32                     CXL_MDH_ERR : 1;
		UINT32                                 : 4;
		UINT32                        RSVDP_31 : 1;
	} Field;
	UINT32	Value;
} CXL_FATAL_ERR_CR_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32          CACHE_POISON_INJ_START : 1;
		UINT32           CACHE_POISON_INJ_BUSY : 1;
		UINT32            MEM_POISON_INJ_START : 1;
		UINT32             MEM_POISON_INJ_BUSY : 1;
		UINT32             IO_POISON_INJ_START : 1;
		UINT32              IO_POISON_INJ_BUSY : 1;
		UINT32               CACHE_MEM_CRC_INJ : 2;
		UINT32         CACHE_MEM_CRC_INJ_COUNT : 2;
		UINT32    CACHE_MEM_CRC_INJ_COUNT_BUSY : 1;
		UINT32                        RSVDP_11 : 21;
	} Field;
	UINT32	Value;
} CXL_CACHE_MEM_LL_INJ_OFF_REGISTER;


typedef union { 
	struct {
		UINT32         CACHE_MEM_RX_CRC_INJ_EN : 1;
		UINT32      CACHE_MEM_RX_CRC_INJ_COUNT : 2;
		UINT32                         RSVDP_3 : 1;
		UINT32     CACHE_MEM_RX_CRC_INJ_STATUS : 2;
		UINT32        CACHE_MEM_RX_CRC_INJ_NUM : 10;
		UINT32                        RSVDP_15 : 16;
	} Field;
	UINT32	Value;
} CACHE_MEM_RX_CRC_INJ_CTRL_REGISTER;


typedef union { 
	struct {
		UINT32          IO_LL_POISON_INJ_START : 1;
		UINT32           IO_LL_POISON_INJ_BUSY : 1;
		UINT32               FLOW_CTRL_ERR_INJ : 1;
		UINT32          FLOW_CTRL_ERR_INJ_BUSY : 1;
		UINT32                         RSVDP_4 : 28;
	} Field;
	UINT32	Value;
} CXL_IO_LL_INJ_OFF_REGISTER;


typedef union { 
	struct {
		UINT32        FB_CORR_PROTO_ID_ERR_INJ : 1;
		UINT32      FB_UNCORR_PROTO_ID_ERR_INJ : 1;
		UINT32       FB_UNEXP_PROTO_ID_ERR_INJ : 1;
		UINT32        FB_PROTO_ID_ERR_INJ_BUSY : 1;
		UINT32                         RSVDP_4 : 28;
	} Field;
	UINT32	Value;
} FLEXBUS_LOG_PHY_ERR_INJ_OFF_REGISTER;


typedef union { 
	struct {
		UINT32             CXL_PL_VIRAL_STATUS : 1;
		UINT32                                 : 16;
		UINT32                        RSVDP_17 : 15;
	} Field;
	UINT32	Value;
} CXL_PORT_LOGIC_VIRAL_STS_OFF_REGISTER;


typedef union { 
	struct {
		UINT32      DBI_FUNCTION_BANK_CTRL_REG : 1;
		UINT32                         RSVDP_1 : 31;
	} Field;
	UINT32	Value;
} DBI_FUNCTION_BANK_CTRL_REG_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                         UTILITY : 32;
	} Field;
	UINT32	Value;
} UTILITY_OFF_REGISTER;


typedef union { 
	struct {
		UINT32                      PM_UTILITY : 32;
	} Field;
	UINT32	Value;
} PM_UTILITY_OFF_REGISTER;

typedef union {
  struct {
    UINT32                            SYSERR_DISABLE:1;
    UINT32                            HWINIT_WR_LOCK:1;
    UINT32                            Reserved_3_2:2;
    UINT32                            FUNC_NUM:3;
    UINT32                            Reserved_31_7:23;
    UINT32                            ENABLE_SERR_EN_FOR_TRANSMISSION:1;
    UINT32                            ENABLE_SIGNALED_SERR_IN_STATUS:1;
  } Field;
  UINT32 Value;
} RCEC_CFG_REGISTER;

typedef union {
  struct {
    UINT32                            Reserved7_0:8;
    UINT32                            NEXT_BUS:8;
    UINT32                            LAST_BUS:8;
    UINT32                            Reserved31_24:8;
  } Field;
  UINT32 Value;
} PCIE_RCECEPA_ASSOCI_REGISTER;

typedef union {
  struct {
    UINT32                                        : 2;
    UINT32                            CXL_ALT_MSE : 1;
    UINT32                            CXL_ALT_BME : 1;
		UINT32                                        : 12;
    UINT32                       CXL_ALT_BUS_BASE : 8;
		UINT32                      CXL_ALT_BUS_LIMIT : 8;
  } Field;
  UINT32 Value;
} IOHC_SHADOW_CXL_PORT_CNTL_AND_ALT_BUS_RANGE_REGISTER;

#pragma pack()

#endif
