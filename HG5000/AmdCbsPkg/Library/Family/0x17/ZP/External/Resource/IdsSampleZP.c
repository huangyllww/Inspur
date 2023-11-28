/*****************************************************************************
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
 *
 ***************************************************************************/


//This file is auto generated, don't edit it manually

//RedirectForReturnDis
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_GEN_W_A05, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_CPU_GEN_W_A05_AUTO:
    break;
  case IDSOPT_CMN_CPU_GEN_W_A05_1:
    break;
  case IDSOPT_CMN_CPU_GEN_W_A05_0:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//L2 TLB Associativity
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_L2_TLB_WAY_ALLOC, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_CPU_L2_TLB_WAY_ALLOC_0:
    break;
  case IDSOPT_CMN_CPU_L2_TLB_WAY_ALLOC_1:
    break;
  case IDSOPT_CMN_CPU_L2_TLB_WAY_ALLOC_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Platform First Error Handling
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_PFEH, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdCcxCfgPFEHEnable)) {
  case IDSOPT_CMN_CPU_PFEH_ENABLED:
    break;
  case IDSOPT_CMN_CPU_PFEH_DISABLED:
    break;
  case IDSOPT_CMN_CPU_PFEH_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Core Performance Boost
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_CPB, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdAmdCpbMode)) {
  case IDSOPT_CMN_CPU_CPB_DISABLED:
    break;
  case IDSOPT_CMN_CPU_CPB_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Enable IBS
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_EN_IBS, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_CPU_EN_IBS_AUTO:
    break;
  case IDSOPT_CMN_CPU_EN_IBS_ENABLED:
    break;
  case IDSOPT_CMN_CPU_EN_IBS_DISABLED:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Global C-state Control
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_GLOBAL_CSTATE_CTRL, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdAmdCStateMode)) {
  case IDSOPT_CMN_CPU_GLOBAL_CSTATE_CTRL_DISABLED:
    break;
  case IDSOPT_CMN_CPU_GLOBAL_CSTATE_CTRL_ENABLED:
    break;
  case IDSOPT_CMN_CPU_GLOBAL_CSTATE_CTRL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Opcache Control
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_OPCACHE_CTRL, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_CPU_OPCACHE_CTRL_DISABLED:
    break;
  case IDSOPT_CMN_CPU_OPCACHE_CTRL_ENABLED:
    break;
  case IDSOPT_CMN_CPU_OPCACHE_CTRL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//OC Mode
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_OC_MODE, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_CPU_OC_MODE_NORMALOPERATION:
    break;
  case IDSOPT_CMN_CPU_OC_MODE_OC1:
    break;
  case IDSOPT_CMN_CPU_OC_MODE_OC2:
    break;
  case IDSOPT_CMN_CPU_OC_MODE_OC3:
    break;
  case IDSOPT_CMN_CPU_OC_MODE_MAXSTRESS:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_SEV_ASID_SPACE_LIMIT, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_CPU_SEV_ASID_SPACE_LIMIT_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_CPU_SEV_ASID_SPACE_LIMIT_MIN);
}

//Streaming Stores Control
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_STREAMING_STORES_CTRL, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_DISABLED:
    break;
  case IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_ENABLED:
    break;
  case IDSOPT_CMN_CPU_STREAMING_STORES_CTRL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Custom Pstate0
IDS_NV_READ_SKIP (IDSNVID_CPU_PST_CUSTOM_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CPU_PST_CUSTOM_P0_CUSTOM:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CPU_COF_P0, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_COF_P0_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_COF_P0_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_VOLTAGE_P0, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_VOLTAGE_P0_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_VOLTAGE_P0_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST0_FID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST0_FID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST0_FID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST0_DID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST0_DID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST0_DID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST0_VID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST0_VID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST0_VID_MIN);
}

//Custom Pstate1
IDS_NV_READ_SKIP (IDSNVID_CPU_PST_CUSTOM_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CPU_PST_CUSTOM_P1_DISABLED:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P1_CUSTOM:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CPU_COF_P1, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_COF_P1_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_COF_P1_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_VOLTAGE_P1, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_VOLTAGE_P1_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_VOLTAGE_P1_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST1_FID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST1_FID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST1_FID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST1_DID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST1_DID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST1_DID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST1_VID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST1_VID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST1_VID_MIN);
}

//Custom Pstate2
IDS_NV_READ_SKIP (IDSNVID_CPU_PST_CUSTOM_P2, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CPU_PST_CUSTOM_P2_DISABLED:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P2_CUSTOM:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P2_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CPU_COF_P2, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_COF_P2_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_COF_P2_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_VOLTAGE_P2, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_VOLTAGE_P2_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_VOLTAGE_P2_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST2_FID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST2_FID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST2_FID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST2_DID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST2_DID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST2_DID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST2_VID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST2_VID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST2_VID_MIN);
}

//Custom Pstate3
IDS_NV_READ_SKIP (IDSNVID_CPU_PST_CUSTOM_P3, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CPU_PST_CUSTOM_P3_DISABLED:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P3_CUSTOM:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P3_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CPU_COF_P3, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_COF_P3_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_COF_P3_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_VOLTAGE_P3, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_VOLTAGE_P3_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_VOLTAGE_P3_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST3_FID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST3_FID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST3_FID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST3_DID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST3_DID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST3_DID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST3_VID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST3_VID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST3_VID_MIN);
}

//Custom Pstate4
IDS_NV_READ_SKIP (IDSNVID_CPU_PST_CUSTOM_P4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CPU_PST_CUSTOM_P4_DISABLED:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P4_CUSTOM:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CPU_COF_P4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_COF_P4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_COF_P4_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_VOLTAGE_P4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_VOLTAGE_P4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_VOLTAGE_P4_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST4_FID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST4_FID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST4_FID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST4_DID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST4_DID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST4_DID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST4_VID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST4_VID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST4_VID_MIN);
}

//Custom Pstate5
IDS_NV_READ_SKIP (IDSNVID_CPU_PST_CUSTOM_P5, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CPU_PST_CUSTOM_P5_DISABLED:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P5_CUSTOM:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P5_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CPU_COF_P5, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_COF_P5_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_COF_P5_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_VOLTAGE_P5, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_VOLTAGE_P5_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_VOLTAGE_P5_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST5_FID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST5_FID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST5_FID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST5_DID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST5_DID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST5_DID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST5_VID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST5_VID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST5_VID_MIN);
}

//Custom Pstate6
IDS_NV_READ_SKIP (IDSNVID_CPU_PST_CUSTOM_P6, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CPU_PST_CUSTOM_P6_DISABLED:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P6_CUSTOM:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P6_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CPU_COF_P6, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_COF_P6_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_COF_P6_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_VOLTAGE_P6, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_VOLTAGE_P6_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_VOLTAGE_P6_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST6_FID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST6_FID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST6_FID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST6_DID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST6_DID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST6_DID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST6_VID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST6_VID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST6_VID_MIN);
}

//Custom Pstate7
IDS_NV_READ_SKIP (IDSNVID_CPU_PST_CUSTOM_P7, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CPU_PST_CUSTOM_P7_DISABLED:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P7_CUSTOM:
    break;
  case IDSOPT_CPU_PST_CUSTOM_P7_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CPU_COF_P7, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_COF_P7_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_COF_P7_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_VOLTAGE_P7, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_VOLTAGE_P7_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_VOLTAGE_P7_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST7_FID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST7_FID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST7_FID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST7_DID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST7_DID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST7_DID_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CPU_PST7_VID, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CPU_PST7_VID_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CPU_PST7_VID_MIN);
}

//Downcore control
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_GEN_DOWNCORE_CTRL, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdAmdDownCoreMode)) {
  case IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_TWO11:
    break;
  case IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_TWO20:
    break;
  case IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_THREE30:
    break;
  case IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_FOUR22:
    break;
  case IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_FOUR40:
    break;
  case IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_SIX33:
    break;
  case IDSOPT_CMN_CPU_GEN_DOWNCORE_CTRL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//L1 Stream HW Prefetcher
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_L1_STREAM_HW_PREFETCHER, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdL1StreamPrefetcher)) {
  case IDSOPT_CMN_CPU_L1_STREAM_HW_PREFETCHER_DISABLE:
    break;
  case IDSOPT_CMN_CPU_L1_STREAM_HW_PREFETCHER_ENABLE:
    break;
  case IDSOPT_CMN_CPU_L1_STREAM_HW_PREFETCHER_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//L2 Stream HW Prefetcher
IDS_NV_READ_SKIP (IDSNVID_CMN_CPU_L2_STREAM_HW_PREFETCHER, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdL2StreamPrefetcher)) {
  case IDSOPT_CMN_CPU_L2_STREAM_HW_PREFETCHER_DISABLE:
    break;
  case IDSOPT_CMN_CPU_L2_STREAM_HW_PREFETCHER_ENABLE:
    break;
  case IDSOPT_CMN_CPU_L2_STREAM_HW_PREFETCHER_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//DRAM scrub time
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_DRAM_SCRUB_TIME, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_DISABLED:
    break;
  case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_1HOUR:
    break;
  case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_4HOURS:
    break;
  case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_8HOURS:
    break;
  case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_16HOURS:
    break;
  case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_24HOURS:
    break;
  case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_48HOURS:
    break;
  case IDSOPT_DF_CMN_DRAM_SCRUB_TIME_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Redirect scrubber control
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_REDIR_SCRUB_CTRL, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_DISABLED:
    break;
  case IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_ENABLED:
    break;
  case IDSOPT_DF_CMN_REDIR_SCRUB_CTRL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Disable DF sync flood propagation
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_SYNC_FLOOD_PROP, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_SYNC_FLOOD_PROP_SYNCFLOODDISABLED:
    break;
  case IDSOPT_DF_CMN_SYNC_FLOOD_PROP_SYNCFLOODENABLED:
    break;
  case IDSOPT_DF_CMN_SYNC_FLOOD_PROP_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Freeze DF module queues on error
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_FREEZE_QUEUE_ERROR, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_DISABLED:
    break;
  case IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_ENABLED:
    break;
  case IDSOPT_DF_CMN_FREEZE_QUEUE_ERROR_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//GMI encryption control
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_GMI_ENCRYPTION, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_GMI_ENCRYPTION_DISABLED:
    break;
  case IDSOPT_DF_CMN_GMI_ENCRYPTION_ENABLED:
    break;
  case IDSOPT_DF_CMN_GMI_ENCRYPTION_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//xGMI encryption control
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_X_GMI_ENCRYPTION, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_X_GMI_ENCRYPTION_DISABLED:
    break;
  case IDSOPT_DF_CMN_X_GMI_ENCRYPTION_ENABLED:
    break;
  case IDSOPT_DF_CMN_X_GMI_ENCRYPTION_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//CC6 memory region encryption
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_CC6_MEM_ENCRYPTION, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_CC6_MEM_ENCRYPTION_DISABLED:
    break;
  case IDSOPT_DF_CMN_CC6_MEM_ENCRYPTION_ENABLED:
    break;
  case IDSOPT_DF_CMN_CC6_MEM_ENCRYPTION_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Location of private memory regions
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_CC6_ALLOCATION_SCHEME, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_CC6_ALLOCATION_SCHEME_DISTRIBUTED:
    break;
  case IDSOPT_DF_CMN_CC6_ALLOCATION_SCHEME_CONSOLIDATED:
    break;
  case IDSOPT_DF_CMN_CC6_ALLOCATION_SCHEME_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//System probe filter
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_SYS_PROBE_FILTER, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_SYS_PROBE_FILTER_DISABLED:
    break;
  case IDSOPT_DF_CMN_SYS_PROBE_FILTER_ENABLED:
    break;
  case IDSOPT_DF_CMN_SYS_PROBE_FILTER_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Memory interleaving
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_MEM_INTLV, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_MEM_INTLV_NONE:
    break;
  case IDSOPT_DF_CMN_MEM_INTLV_CHANNEL:
    break;
  case IDSOPT_DF_CMN_MEM_INTLV_DIE:
    break;
  case IDSOPT_DF_CMN_MEM_INTLV_SOCKET:
    break;
  case IDSOPT_DF_CMN_MEM_INTLV_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Memory interleaving size
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_MEM_INTLV_SIZE, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_MEM_INTLV_SIZE_256BYTES:
    break;
  case IDSOPT_DF_CMN_MEM_INTLV_SIZE_512BYTES:
    break;
  case IDSOPT_DF_CMN_MEM_INTLV_SIZE_1KB:
    break;
  case IDSOPT_DF_CMN_MEM_INTLV_SIZE_2KB:
    break;
  case IDSOPT_DF_CMN_MEM_INTLV_SIZE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Channel interleaving hash
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_CHNL_INTLV_HASH, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_CHNL_INTLV_HASH_DISABLED:
    break;
  case IDSOPT_DF_CMN_CHNL_INTLV_HASH_ENABLED:
    break;
  case IDSOPT_DF_CMN_CHNL_INTLV_HASH_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Memory Clear
IDS_NV_READ_SKIP (IDSNVID_DF_CMN_MEM_CLEAR, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DF_CMN_MEM_CLEAR_ENABLED:
    break;
  case IDSOPT_DF_CMN_MEM_CLEAR_DISABLED:
    break;
  case IDSOPT_DF_CMN_MEM_CLEAR_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Overclock
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_OVERCLOCK_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_OVERCLOCK_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_OVERCLOCK_DDR4_ENABLED:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Memory Clock Speed
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_SPEED_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_SPEED_DDR4_667MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_800MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_933MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_1067MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_1200MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_1333MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_1467MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_1600MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_333MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_400MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_533MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_1050MHZ:
    break;
  case IDSOPT_CMN_MEM_SPEED_DDR4_1066MHZ:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Tcl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TCL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_0FHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_10HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_11HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_12HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_13HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_14HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_15HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_16HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_17HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_18HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_19HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_1FHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_20HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCL_DDR4_21HCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Trcdrd
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRCDRD_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_0FHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_10HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_14HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_15HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_16HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_17HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_18HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_19HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_1AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_11HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_12HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_13HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDRD_DDR4_1BHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Trcdwr
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRCDWR_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XACLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XBCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XCCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XDCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XECLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_0XFCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_10HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_11HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_12HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_13HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_14HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_15HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_16HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_17HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_18HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_19HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_1AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCDWR_DDR4_1BHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Trp
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRP_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_0FHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_10HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_11HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_12HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_13HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_14HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_15HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_16HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_17HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_18HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_19HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_1AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRP_DDR4_1BHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Tras
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRAS_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_15HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_16HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_17HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_18HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_19HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_1FHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_20HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_21HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_22HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_23HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_24HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_25HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_26HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_27HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_28HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_29HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_2FHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_30HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_31HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_32HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_33HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_34HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_35HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_36HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_37HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_38HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_39HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRAS_DDR4_3AHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Trc Ctrl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRC_CTRL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRC_CTRL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRC_CTRL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRC_DDR4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_MEM_TIMING_TRC_DDR4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_MEM_TIMING_TRC_DDR4_MIN);
}

//TrrdS
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRRD_S_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_S_DDR4_0CHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//TrrdL
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRRD_L_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRRD_L_DDR4_0CHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Tfaw Ctrl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TFAW_CTRL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TFAW_CTRL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TFAW_CTRL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TFAW_DDR4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_MEM_TIMING_TFAW_DDR4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_MEM_TIMING_TFAW_DDR4_MIN);
}

//TwtrS
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWTR_S_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_S_DDR4_0EHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//TwtrL
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWTR_L_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWTR_L_DDR4_0EHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Twr Ctrl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWR_CTRL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TWR_CTRL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWR_CTRL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWR_DDR4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_MEM_TIMING_TWR_DDR4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_MEM_TIMING_TWR_DDR4_MIN);
}

//Trcpage Ctrl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRCPAGE_CTRL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRCPAGE_DDR4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_MEM_TIMING_TRCPAGE_DDR4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_MEM_TIMING_TRCPAGE_DDR4_MIN);
}

//TrdrdScL Ctrl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_L_CTRL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRDRD_SC_L_DDR4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_MEM_TIMING_TRDRD_SC_L_DDR4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_MEM_TIMING_TRDRD_SC_L_DDR4_MIN);
}

//TwrwrScL Ctrl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_L_CTRL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWRWR_SC_L_DDR4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_MEM_TIMING_TWRWR_SC_L_DDR4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_MEM_TIMING_TWRWR_SC_L_DDR4_MIN);
}

//Trfc Ctrl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRFC_CTRL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRFC_CTRL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRFC_CTRL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRFC_DDR4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_MEM_TIMING_TRFC_DDR4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_MEM_TIMING_TRFC_DDR4_MIN);
}

//Trfc2 Ctrl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRFC2_CTRL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRFC2_CTRL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRFC2_CTRL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRFC2_DDR4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_MEM_TIMING_TRFC2_DDR4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_MEM_TIMING_TRFC2_DDR4_MIN);
}

//Trfc4 Ctrl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRFC4_CTRL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRFC4_CTRL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRFC4_CTRL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRFC4_DDR4, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_MEM_TIMING_TRFC4_DDR4_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_MEM_TIMING_TRFC4_DDR4_MIN);
}

//ProcODT
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_PROC_ODT_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_HIGHIMPEDANCE:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_480OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_240OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_160OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_120OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_96OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_80OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_686OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_60OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_533OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_48OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_436OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_40OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_369OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_343OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_32OHM:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PROC_ODT_DDR4_30OHM:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Tcwl
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TCWL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_0FHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_10HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_11HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_12HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_13HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_14HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_15HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCWL_DDR4_16HCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Trtp
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRTP_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRTP_DDR4_0EHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Trdwr
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRDWR_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_0FHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_10HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_11HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_12HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_13HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_14HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_15HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_16HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_17HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_18HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_19HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDWR_DDR4_1FHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Twrrd
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWRRD_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_1CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0CH:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0DH:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0EH:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRRD_DDR4_0FH:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//TwrwrSc
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWRWR_SC_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_1CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SC_DDR4_0FHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//TwrwrSd
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWRWR_SD_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_1CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_SD_DDR4_0FHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//TwrwrDd
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TWRWR_DD_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_1CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TWRWR_DD_DDR4_0FHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//TrdrdSc
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRDRD_SC_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_1CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SC_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//TrdrdSd
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRDRD_SD_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_1CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_SD_DDR4_0FHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//TrdrdDd
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TRDRD_DD_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_1CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TRDRD_DD_DDR4_0FHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Tcke
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TIMING_TCKE_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_2CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_3CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_4CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_5CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_6CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_7CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_8CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_9CLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_0FHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_10HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_11HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_12HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_13HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_14HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_15HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_16HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_17HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_18HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_19HCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1AHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1BHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1CHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1DHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1EHCLK:
    break;
  case IDSOPT_CMN_MEM_TIMING_TCKE_DDR4_1FHCLK:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Cmd2T
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER2_T_MODE_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER2_T_MODE_DDR4_1T:
    break;
  case IDSOPT_CMN_MEM_CTRLLER2_T_MODE_DDR4_2T:
    break;
  case IDSOPT_CMN_MEM_CTRLLER2_T_MODE_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Gear Down Mode
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_GEAR_DOWN_MODE_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_GEAR_DOWN_MODE_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_GEAR_DOWN_MODE_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_GEAR_DOWN_MODE_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Power Down Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_PWR_DN_EN_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Data Bus Configuration User Controls
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_DATA_BUS_CONFIG_CTL_DDR4_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//RttNom
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_RTT_NOM_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RTT_NOMDISABLE:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ4:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ2:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ6:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ1:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ5:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ3:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_RZQ7:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_NOM_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//RttWr
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_RTT_WR_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_DYNAMICODTOFF:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_RZQ2:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_RZQ1:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_HIZ:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_RZQ3:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_WR_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//RttPark
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_RTT_PARK_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RTT_PARKDISABLE:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ4:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ2:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ6:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ1:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ5:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ3:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_RZQ7:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_RTT_PARK_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Data Poisoning
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_DATA_POISONING_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_DATA_POISONING_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_DATA_POISONING_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_DATA_POISONING_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//DRAM ECC Symbol Size
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4_X4:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4_X8:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_SYMBOL_SIZE_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//DRAM ECC Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_DRAM_ECC_EN_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//TSME
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_TSME_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_TSME_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_TSME_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_TSME_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Data Scramble
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_DATA_SCRAMBLE_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Chipselect Interleaving
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_MAPPING_BANK_INTERLEAVE_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//BankGroupSwap
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//BankGroupSwapAlt
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_CTRLLER_BANK_GROUP_SWAP_ALT_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Address Hash Bank
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_ADDRESS_HASH_BANK_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_ADDRESS_HASH_BANK_DDR4_DISABLED:
    break;
  case IDSOPT_CMN_MEM_ADDRESS_HASH_BANK_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_ADDRESS_HASH_BANK_DDR4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Address Hash CS
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_ADDRESS_HASH_CS_DDR4, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_ADDRESS_HASH_CS_DDR4_AUTO:
    break;
  case IDSOPT_CMN_MEM_ADDRESS_HASH_CS_DDR4_ENABLED:
    break;
  case IDSOPT_CMN_MEM_ADDRESS_HASH_CS_DDR4_DISABLED:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//MBIST Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_MBIST_EN, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_MBIST_EN_DISABLED:
    break;
  case IDSOPT_CMN_MEM_MBIST_EN_ENABLED:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//MBIST Test Mode
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_MBIST_TESTMODE, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_MBIST_TESTMODE_INTERFACEMODE:
    break;
  case IDSOPT_CMN_MEM_MBIST_TESTMODE_DATAEYEMODE:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//MBIST Aggressors
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_MBIST_AGGRESSORS, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_MBIST_AGGRESSORS_DISABLED:
    break;
  case IDSOPT_CMN_MEM_MBIST_AGGRESSORS_ENABLED:
    break;
  case IDSOPT_CMN_MEM_MBIST_AGGRESSORS_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//MBIST Per Bit Slave Die Reporting
IDS_NV_READ_SKIP (IDSNVID_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT_DISABLED:
    break;
  case IDSOPT_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT_ENABLED:
    break;
  case IDSOPT_CMN_MEM_MBIST_PER_BIT_SLAVE_DIE_REPORT_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//System Configuration
IDS_NV_READ_SKIP (IDSNVID_CMN_GNB_SMU_SYSTEM_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_65WPORCONFIGURATION:
    break;
  case IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_45WPORCONFIGURATION:
    break;
  case IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_35WPORCONFIGURATION:
    break;
  case IDSOPT_CMN_GNB_SMU_SYSTEM_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//NBIO Internal Poison Consumption
IDS_NV_READ_SKIP (IDSNVID_DBG_POISON_CONSUMPTION, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdNbioPoisonConsumption)) {
  case IDSOPT_DBG_POISON_CONSUMPTION_ENABLED:
    break;
  case IDSOPT_DBG_POISON_CONSUMPTION_DISABLED:
    break;
  case IDSOPT_DBG_POISON_CONSUMPTION_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//NBIO RAS Control
IDS_NV_READ_SKIP (IDSNVID_DBG_RAS_CONTROL, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdNbioRASControl)) {
  case IDSOPT_DBG_RAS_CONTROL_ENABLED:
    break;
  case IDSOPT_DBG_RAS_CONTROL_DISABLED:
    break;
  case IDSOPT_DBG_RAS_CONTROL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Determinism Slider
IDS_NV_READ_SKIP (IDSNVID_CMN_DETERMINISM_SLIDER, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdDeterminismControl)) {
  case IDSOPT_CMN_DETERMINISM_SLIDER_AUTO:
    break;
  case IDSOPT_CMN_DETERMINISM_SLIDER_POWER:
    break;
  case IDSOPT_CMN_DETERMINISM_SLIDER_PERFORMANCE:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//cTDP Control
IDS_NV_READ_SKIP (IDSNVID_CMNC_TDP_CTL, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMNC_TDP_CTL_MANUAL:
    break;
  case IDSOPT_CMNC_TDP_CTL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMNC_TDP_LIMIT, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMNC_TDP_LIMIT_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMNC_TDP_LIMIT_MIN);
}

//Efficiency Optimized Mode
IDS_NV_READ_SKIP (IDSNVID_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdEfficiencyOptimizedMode)) {
  case IDSOPT_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE_DISABLED:
    break;
  case IDSOPT_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE_ENABLED:
    break;
  case IDSOPT_CMN_NBIO_EFFICIENCY_OPTIMIZED_MODE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PSI
IDS_NV_READ_SKIP (IDSNVID_CMN_NBIO_PSI_DISABLE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdPSIDisable)) {
  case IDSOPT_CMN_NBIO_PSI_DISABLE_DISABLE:
    break;
  case IDSOPT_CMN_NBIO_PSI_DISABLE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//ACS Enable
IDS_NV_READ_SKIP (IDSNVID_DBG_GNB_DBG_ACS_ENABLE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdCfgACSEnable)) {
  case IDSOPT_DBG_GNB_DBG_ACS_ENABLE_ENABLE:
    break;
  case IDSOPT_DBG_GNB_DBG_ACS_ENABLE_DISABLED:
    break;
  case IDSOPT_DBG_GNB_DBG_ACS_ENABLE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PCIe ARI Support
IDS_NV_READ_SKIP (IDSNVID_GNB_DBG_PCIE_ARI_SUPPORT, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdCfgPcieAriSupport)) {
  case IDSOPT_GNB_DBG_PCIE_ARI_SUPPORT_DISABLE:
    break;
  case IDSOPT_GNB_DBG_PCIE_ARI_SUPPORT_ENABLE:
    break;
  case IDSOPT_GNB_DBG_PCIE_ARI_SUPPORT_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//CLDO_VDDP Control
IDS_NV_READ_SKIP (IDSNVID_CMN_CLDO_VDDP_CTL, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_CLDO_VDDP_CTL_AUTO:
    break;
  case IDSOPT_CMN_CLDO_VDDP_CTL_MANUAL:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CMN_CLDOVDD_PVOLTAGE, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CMN_CLDOVDD_PVOLTAGE_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CMN_CLDOVDD_PVOLTAGE_MIN);
}

//HD Audio Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_GNB_HD_AUDIO_EN, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_GNB_HD_AUDIO_EN_ENABLE:
    break;
  case IDSOPT_CMN_GNB_HD_AUDIO_EN_DISABLED:
    break;
  case IDSOPT_CMN_GNB_HD_AUDIO_EN_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Block PCIe Loopback
IDS_NV_READ_SKIP (IDSNVID_CFG_PCIE_LOOPBACK_MODE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdCfgPcieLoopbackMode)) {
  case IDSOPT_CFG_PCIE_LOOPBACK_MODE_DISABLE:
    break;
  case IDSOPT_CFG_PCIE_LOOPBACK_MODE_ENABLE:
    break;
  case IDSOPT_CFG_PCIE_LOOPBACK_MODE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_CFG_PCIE_CRS_DELAY, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CFG_PCIE_CRS_DELAY_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CFG_PCIE_CRS_DELAY_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_CFG_PCIE_CRS_LIMIT, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_CFG_PCIE_CRS_LIMIT_MAX);
  ASSERT (IdsNvValue >= IDSOPT_CFG_PCIE_CRS_LIMIT_MIN);
}

//IOMMU
IDS_NV_READ_SKIP (IDSNVID_CMN_GNB_NB_IOMMU, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdCfgIommuSupport)) {
  case IDSOPT_CMN_GNB_NB_IOMMU_DISABLED:
    break;
  case IDSOPT_CMN_GNB_NB_IOMMU_ENABLED:
    break;
  case IDSOPT_CMN_GNB_NB_IOMMU_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Concurrent Training
IDS_NV_READ_SKIP (IDSNVID_PCIE_SYNC_RESET, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdPcieSyncReset)) {
  case IDSOPT_PCIE_SYNC_RESET_FALSE:
    break;
  case IDSOPT_PCIE_SYNC_RESET_TRUE:
    break;
  case IDSOPT_PCIE_SYNC_RESET_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Fan Control
IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_CTL, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DBG_FAN_CTL_MANUAL:
    break;
  case IDSOPT_DBG_FAN_CTL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Force PWM Control
IDS_NV_READ_SKIP (IDSNVID_DBG_FORCE_PWM_CTL, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DBG_FORCE_PWM_CTL_FORCE:
    break;
  case IDSOPT_DBG_FORCE_PWM_CTL_UNFORCE:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FORCE_PWM, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FORCE_PWM_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FORCE_PWM_MIN);
}

//Fan Table Control
IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_CTL, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DBG_FAN_TABLE_CTL_MANUAL:
    break;
  case IDSOPT_DBG_FAN_TABLE_CTL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_TEMP_LOW, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FAN_TABLE_TEMP_LOW_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FAN_TABLE_TEMP_LOW_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_TEMP_MED, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FAN_TABLE_TEMP_MED_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FAN_TABLE_TEMP_MED_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_TEMP_HIGH, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FAN_TABLE_TEMP_HIGH_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FAN_TABLE_TEMP_HIGH_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_TEMP_CRITICAL, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FAN_TABLE_TEMP_CRITICAL_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FAN_TABLE_TEMP_CRITICAL_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_T_PWM_LOW, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FAN_TABLE_T_PWM_LOW_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FAN_TABLE_T_PWM_LOW_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_PWM_MED, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FAN_TABLE_PWM_MED_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FAN_TABLE_PWM_MED_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_PWM_HIGH, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FAN_TABLE_PWM_HIGH_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FAN_TABLE_PWM_HIGH_MIN);
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_HYST, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FAN_TABLE_HYST_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FAN_TABLE_HYST_MIN);
}

//Pwm Frequency
IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_TABLE_PWM_FREQ, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DBG_FAN_TABLE_PWM_FREQ_100HZ:
    break;
  case IDSOPT_DBG_FAN_TABLE_PWM_FREQ_25KHZ:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Fan Polarity
IDS_NV_READ_SKIP (IDSNVID_DBG_FAN_POLARITY, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_DBG_FAN_POLARITY_NEGATIVE:
    break;
  case IDSOPT_DBG_FAN_POLARITY_POSITIVE:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Ignore sideband
IDS_NV_READ_SKIP (IDSNVID_CMN_DISABLE_SIDEBAND, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdDisableSideband)) {
  case IDSOPT_CMN_DISABLE_SIDEBAND_DISABLED:
    break;
  case IDSOPT_CMN_DISABLE_SIDEBAND_ENABLED:
    break;
  case IDSOPT_CMN_DISABLE_SIDEBAND_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Disable L1 w/a
IDS_NV_READ_SKIP (IDSNVID_CMN_DISABLE_L1WA, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdDisableL1wa)) {
  case IDSOPT_CMN_DISABLE_L1WA_DISABLED:
    break;
  case IDSOPT_CMN_DISABLE_L1WA_ENABLED:
    break;
  case IDSOPT_CMN_DISABLE_L1WA_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Disable BridgeDis
IDS_NV_READ_SKIP (IDSNVID_CMN_DISABLE_BRIDGE_DIS, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdDisableBridgeDis)) {
  case IDSOPT_CMN_DISABLE_BRIDGE_DIS_DISABLED:
    break;
  case IDSOPT_CMN_DISABLE_BRIDGE_DIS_ENABLED:
    break;
  case IDSOPT_CMN_DISABLE_BRIDGE_DIS_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Disable irq polling
IDS_NV_READ_SKIP (IDSNVID_CMN_DISABLE_IRQ_POLL, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdDisableIrqPoll)) {
  case IDSOPT_CMN_DISABLE_IRQ_POLL_DISABLED:
    break;
  case IDSOPT_CMN_DISABLE_IRQ_POLL_ENABLED:
    break;
  case IDSOPT_CMN_DISABLE_IRQ_POLL_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//IRQ sets BridgeDis
IDS_NV_READ_SKIP (IDSNVID_CMN_IRQ_SETS_BRIDGE_DIS, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdAmdIrqSetBridgeDis)) {
  case IDSOPT_CMN_IRQ_SETS_BRIDGE_DIS_DISABLED:
    break;
  case IDSOPT_CMN_IRQ_SETS_BRIDGE_DIS_ENABLED:
    break;
  case IDSOPT_CMN_IRQ_SETS_BRIDGE_DIS_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}
  
//PCIE hotplug support
IDS_NV_READ_SKIP (IDSNVID_CBS_PCIE_HOTPLUG_SUPPORT, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdCfgPcieHotplugSupport)) {
  case IDSOPT_PCIE_HOTPLUG_SUPPORT_DISABLED:
    break;
  case IDSOPT_PCIE_HOTPLUG_SUPPORT_ENABLED:
    break;
  case IDSOPT_PCIE_HOTPLUG_SUPPORT_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//SATA Controller
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_SATA_ENABLE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdSataEnable)) {
  case IDSOPT_CMN_FCH_SATA_ENABLE_DISABLED:
    break;
  case IDSOPT_CMN_FCH_SATA_ENABLE_ENABLED:
    break;
  case IDSOPT_CMN_FCH_SATA_ENABLE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//SATA Mode
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_SATA_CLASS, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdSataClass)) {
  case IDSOPT_CMN_FCH_SATA_CLASS_AHCI:
    break;
  case IDSOPT_CMN_FCH_SATA_CLASS_AHCIASID0X7904:
    break;
  case IDSOPT_CMN_FCH_SATA_CLASS_AUTO:
    break;
  case IDSOPT_CMN_FCH_SATA_CLASS_RAID:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Sata RAS Support
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_SATA_RAS_SUPPORT, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdSataRasSupport)) {
  case IDSOPT_CMN_FCH_SATA_RAS_SUPPORT_DISABLED:
    break;
  case IDSOPT_CMN_FCH_SATA_RAS_SUPPORT_ENABLED:
    break;
  case IDSOPT_CMN_FCH_SATA_RAS_SUPPORT_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Sata Disabled AHCI Prefetch Function
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdSataAhciDisPrefetchFunction)) {
  case IDSOPT_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION_DISABLED:
    break;
  case IDSOPT_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION_ENABLED:
    break;
  case IDSOPT_CMN_FCH_SATA_AHCI_DIS_PREFETCH_FUNCTION_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Aggresive SATA Device Sleep Port 0
IDS_NV_READ_SKIP (IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdSataDevSlpPort0)) {
  case IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0_DISABLED:
    break;
  case IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0_ENABLED:
    break;
  case IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FCH_SATA_DEV_SLP_PORT0_NUM, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FCH_SATA_DEV_SLP_PORT0_NUM_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FCH_SATA_DEV_SLP_PORT0_NUM_MIN);
}

//Aggresive SATA Device Sleep Port 1
IDS_NV_READ_SKIP (IDSNVID_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdSataDevSlpPort1)) {
  case IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1_DISABLED:
    break;
  case IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1_ENABLED:
    break;
  case IDSOPT_DBG_FCH_SATA_AGGRESIVE_DEV_SLP_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

IDS_NV_READ_SKIP (IDSNVID_DBG_FCH_SATA_DEV_SLP_PORT1_NUM, &IdsNvValue) {
  ASSERT (IdsNvValue <= IDSOPT_DBG_FCH_SATA_DEV_SLP_PORT1_NUM_MAX);
  ASSERT (IdsNvValue >= IDSOPT_DBG_FCH_SATA_DEV_SLP_PORT1_NUM_MIN);
}

//XHCI controller enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_USB_XHC_I0_ENABLE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdXhci0Enable)) {
  case IDSOPT_CMN_FCH_USB_XHC_I0_ENABLE_ENABLED:
    break;
  case IDSOPT_CMN_FCH_USB_XHC_I0_ENABLE_DISABLED:
    break;
  case IDSOPT_CMN_FCH_USB_XHC_I0_ENABLE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//XHCI Controller1 enable (Die1)
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_USB_XHC_I1_ENABLE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdXhci1Enable)) {
  case IDSOPT_CMN_FCH_USB_XHC_I1_ENABLE_ENABLED:
    break;
  case IDSOPT_CMN_FCH_USB_XHC_I1_ENABLE_DISABLED:
    break;
  case IDSOPT_CMN_FCH_USB_XHC_I1_ENABLE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//XHCI2 enable (MCM1/Die0)
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_USB_XHC_I2_ENABLE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdXhci2Enable)) {
  case IDSOPT_CMN_FCH_USB_XHC_I2_ENABLE_ENABLED:
    break;
  case IDSOPT_CMN_FCH_USB_XHC_I2_ENABLE_DISABLED:
    break;
  case IDSOPT_CMN_FCH_USB_XHC_I2_ENABLE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//XHCI3 enable (MCM1/Die1)
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_USB_XHC_I3_ENABLE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdXhci3Enable)) {
  case IDSOPT_CMN_FCH_USB_XHC_I3_ENABLE_ENABLED:
    break;
  case IDSOPT_CMN_FCH_USB_XHC_I3_ENABLE_DISABLED:
    break;
  case IDSOPT_CMN_FCH_USB_XHC_I3_ENABLE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//SD Configuration Mode
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_SD_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdSdConfig)) {
  case IDSOPT_CMN_FCH_SD_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_SD_CONFIG_VER20:
    break;
  case IDSOPT_CMN_FCH_SD_CONFIG_SDDUMP:
    break;
  case IDSOPT_CMN_FCH_SD_CONFIG_AUTOVERSION20LOWSPEED:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Ac Loss Control
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_SYSTEM_PWR_FAIL_SHADOW, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPwrFailShadow)) {
  case IDSOPT_CMN_FCH_SYSTEM_PWR_FAIL_SHADOW_ALWAYSOFF:
    break;
  case IDSOPT_CMN_FCH_SYSTEM_PWR_FAIL_SHADOW_ALWAYSON:
    break;
  case IDSOPT_CMN_FCH_SYSTEM_PWR_FAIL_SHADOW_RESERVED:
    break;
  case IDSOPT_CMN_FCH_SYSTEM_PWR_FAIL_SHADOW_PREVIOUS:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//I2C 0 Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_I2_C0_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_I2_C0_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C0_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C0_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//I2C 1 Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_I2_C1_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_I2_C1_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C1_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C1_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//I2C 2 Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_I2_C2_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_I2_C2_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C2_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C2_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//I2C 3 Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_I2_C3_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_I2_C3_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C3_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C3_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//I2C 4 Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_I2_C4_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_I2_C4_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C4_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C4_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//I2C 5 Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_I2_C5_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_I2_C5_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C5_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_I2_C5_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Uart 0 Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_UART0_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_UART0_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_UART0_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_UART0_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Uart 0 Legacy Options
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_UART0_LEGACY_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (FchUart0LegacyEnable)) {
  case IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_0X2E8:
    break;
  case IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_0X2F8:
    break;
  case IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_0X3E8:
    break;
  case IDSOPT_CMN_FCH_UART0_LEGACY_CONFIG_0X3F8:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Uart 1 Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_UART1_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_UART1_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_UART1_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_UART1_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Uart 1 Legacy Options
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_UART1_LEGACY_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (FchUart1LegacyEnable)) {
  case IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_0X2E8:
    break;
  case IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_0X2F8:
    break;
  case IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_0X3E8:
    break;
  case IDSOPT_CMN_FCH_UART1_LEGACY_CONFIG_0X3F8:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Uart 2 Enable (no HW FC)
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_UART2_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_UART2_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_UART2_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_UART2_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Uart 2 Legacy Options
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_UART2_LEGACY_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (FchUart2LegacyEnable)) {
  case IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_0X2E8:
    break;
  case IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_0X2F8:
    break;
  case IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_0X3E8:
    break;
  case IDSOPT_CMN_FCH_UART2_LEGACY_CONFIG_0X3F8:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Uart 3 Enable (no HW FC)
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_UART3_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_UART3_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_UART3_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_UART3_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//Uart 3 Legacy Options
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_UART3_LEGACY_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (FchUart3LegacyEnable)) {
  case IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_0X2E8:
    break;
  case IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_0X2F8:
    break;
  case IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_0X3E8:
    break;
  case IDSOPT_CMN_FCH_UART3_LEGACY_CONFIG_0X3F8:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//ESPI Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_FCH_ESPI_CONFIG, &IdsNvValue) {
  switch (IdsNvValue) {
  case IDSOPT_CMN_FCH_ESPI_CONFIG_DISABLED:
    break;
  case IDSOPT_CMN_FCH_ESPI_CONFIG_ENABLED:
    break;
  case IDSOPT_CMN_FCH_ESPI_CONFIG_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT SATA Port Enable
IDS_NV_READ_SKIP (IDSNVID_CMN_PT_SATA_PORT_ENABLE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTSataPortEnable)) {
  case IDSOPT_CMN_PT_SATA_PORT_ENABLE_DISABLED:
    break;
  case IDSOPT_CMN_PT_SATA_PORT_ENABLE_ENABLED:
    break;
  case IDSOPT_CMN_PT_SATA_PORT_ENABLE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT SATA Mode
IDS_NV_READ_SKIP (IDSNVID_CMN_PT_SATA_CLASS, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTSataMode)) {
  case IDSOPT_CMN_PT_SATA_CLASS_AHCI:
    break;
  case IDSOPT_CMN_PT_SATA_CLASS_RAID:
    break;
  case IDSOPT_CMN_PT_SATA_CLASS_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT Aggresive SATA Device Sleep Port 0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTSataAggresiveDevSlpP0)) {
  case IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT Aggresive SATA Device Sleep Port 1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTSataAggresiveDevSlpP1)) {
  case IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1_DISABLE:
    break;
  case IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1_ENABLE:
    break;
  case IDSOPT_DBG_PT_SATA_AGGRESIVE_DEV_SLP_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT XHCI GEN1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_XHCI_GEN1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTXhciGen1)) {
  case IDSOPT_DBG_PT_XHCI_GEN1_DISABLE:
    break;
  case IDSOPT_DBG_PT_XHCI_GEN1_ENABLE:
    break;
  case IDSOPT_DBG_PT_XHCI_GEN1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT XHCI GEN2
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_XHCI_GEN2, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTXhciGen2)) {
  case IDSOPT_DBG_PT_XHCI_GEN2_DISABLE:
    break;
  case IDSOPT_DBG_PT_XHCI_GEN2_ENABLE:
    break;
  case IDSOPT_DBG_PT_XHCI_GEN2_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB Equalization4
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB_EQUALIZATION4, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsbEqualization4)) {
  case IDSOPT_DBG_PT_USB_EQUALIZATION4_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB_EQUALIZATION4_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB_EQUALIZATION4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB Redriver
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB_REDRIVER, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsbRedriver)) {
  case IDSOPT_DBG_PT_USB_REDRIVER_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB_REDRIVER_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB_REDRIVER_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB31 PORT0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB31_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb31P0)) {
  case IDSOPT_DBG_PT_USB31_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB31_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB31_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB31 PORT1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB31_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb31P1)) {
  case IDSOPT_DBG_PT_USB31_P1_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB31_P1_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB31_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB30 PORT0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB30_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb30P0)) {
  case IDSOPT_DBG_PT_USB30_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB30 PORT1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB30_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb30P1)) {
  case IDSOPT_DBG_PT_USB30_P1_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P1_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB30 PORT2
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB30_P2, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb30P2)) {
  case IDSOPT_DBG_PT_USB30_P2_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P2_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P2_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB30 PORT3
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB30_P3, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb30P3)) {
  case IDSOPT_DBG_PT_USB30_P3_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P3_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P3_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB30 PORT4
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB30_P4, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb30P4)) {
  case IDSOPT_DBG_PT_USB30_P4_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P4_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB30 PORT5
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB30_P5, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb30P5)) {
  case IDSOPT_DBG_PT_USB30_P5_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P5_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB30_P5_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB20_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb20P0)) {
  case IDSOPT_DBG_PT_USB20_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB20_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb20P1)) {
  case IDSOPT_DBG_PT_USB20_P1_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P1_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT2
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB20_P2, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb20P2)) {
  case IDSOPT_DBG_PT_USB20_P2_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P2_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P2_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT3
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB20_P3, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb20P3)) {
  case IDSOPT_DBG_PT_USB20_P3_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P3_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P3_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT4
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB20_P4, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb20P4)) {
  case IDSOPT_DBG_PT_USB20_P4_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P4_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT5
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_USB20_P5, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTUsb20P5)) {
  case IDSOPT_DBG_PT_USB20_P5_DISABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P5_ENABLE:
    break;
  case IDSOPT_DBG_PT_USB20_P5_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB31 PORT0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB31_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb31P0)) {
  case IDSOPT_DBG_PT_PROM2_USB31_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB31_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB31_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB31 PORT1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB31_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb31P1)) {
  case IDSOPT_DBG_PT_PROM2_USB31_P1_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB31_P1_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB31_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB30 PORT0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB30_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb30P0)) {
  case IDSOPT_DBG_PT_PROM2_USB30_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB30_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB30_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB30 PORT1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB30_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb30P1)) {
  case IDSOPT_DBG_PT_PROM2_USB30_P1_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB30_P1_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB30_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB20_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb20P0)) {
  case IDSOPT_DBG_PT_PROM2_USB20_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB20_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb20P1)) {
  case IDSOPT_DBG_PT_PROM2_USB20_P1_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P1_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT2
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB20_P2, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb20P2)) {
  case IDSOPT_DBG_PT_PROM2_USB20_P2_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P2_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P2_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT3
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB20_P3, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb20P3)) {
  case IDSOPT_DBG_PT_PROM2_USB20_P3_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P3_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P3_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT4
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB20_P4, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb20P4)) {
  case IDSOPT_DBG_PT_PROM2_USB20_P4_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P4_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT5
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM2_USB20_P5, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm2Usb20P5)) {
  case IDSOPT_DBG_PT_PROM2_USB20_P5_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P5_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM2_USB20_P5_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB31 PORT0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM1_USB31_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm1Usb31P0)) {
  case IDSOPT_DBG_PT_PROM1_USB31_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB31_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB31_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB31 PORT1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM1_USB31_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm1Usb31P1)) {
  case IDSOPT_DBG_PT_PROM1_USB31_P1_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB31_P1_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB31_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB30 PORT0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM1_USB30_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm1Usb30P0)) {
  case IDSOPT_DBG_PT_PROM1_USB30_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB30_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB30_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT0
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM1_USB20_P0, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm1Usb20P0)) {
  case IDSOPT_DBG_PT_PROM1_USB20_P0_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P0_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P0_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT1
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM1_USB20_P1, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm1Usb20P1)) {
  case IDSOPT_DBG_PT_PROM1_USB20_P1_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P1_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P1_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT2
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM1_USB20_P2, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm1Usb20P2)) {
  case IDSOPT_DBG_PT_PROM1_USB20_P2_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P2_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P2_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT3
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM1_USB20_P3, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm1Usb20P3)) {
  case IDSOPT_DBG_PT_PROM1_USB20_P3_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P3_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P3_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT4
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM1_USB20_P4, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm1Usb20P4)) {
  case IDSOPT_DBG_PT_PROM1_USB20_P4_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P4_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P4_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//PT USB20 PORT5
IDS_NV_READ_SKIP (IDSNVID_DBG_PT_PROM1_USB20_P5, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdPTProm1Usb20P5)) {
  case IDSOPT_DBG_PT_PROM1_USB20_P5_DISABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P5_ENABLE:
    break;
  case IDSOPT_DBG_PT_PROM1_USB20_P5_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//NTB Enable
IDS_NV_READ_SKIP (IDSNVID_DBG_S_P3_NTB_ENABLE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGetBool (PcdCfgNTBEnable)) {
  case IDSOPT_DBG_S_P3_NTB_ENABLE_AUTO:
    break;
  case IDSOPT_DBG_S_P3_NTB_ENABLE_ENABLE:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//NTB Location
IDS_NV_READ_SKIP (IDSNVID_DBG_S_P3_NTB_LOCATION, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdCfgNTBLocation)) {
  case IDSOPT_DBG_S_P3_NTB_LOCATION_AUTO:
    break;
  case IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET0DIE0:
    break;
  case IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET0DIE1:
    break;
  case IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET0DIE2:
    break;
  case IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET0DIE3:
    break;
  case IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET1DIE0:
    break;
  case IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET1DIE1:
    break;
  case IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET1DIE2:
    break;
  case IDSOPT_DBG_S_P3_NTB_LOCATION_SOCKET1DIE3:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//NTB active on PCIeCore
IDS_NV_READ_SKIP (IDSNVID_DBG_S_P3_NTB_PC_IE_CORE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdCfgNTBPcieCoreSel)) {
  case IDSOPT_DBG_S_P3_NTB_PC_IE_CORE_AUTO:
    break;
  case IDSOPT_DBG_S_P3_NTB_PC_IE_CORE_CORE0:
    break;
  case IDSOPT_DBG_S_P3_NTB_PC_IE_CORE_CORE1:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//NTB Mode
IDS_NV_READ_SKIP (IDSNVID_DBG_S_P3_NTB_MODE, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdCfgNTBMode)) {
  case IDSOPT_DBG_S_P3_NTB_MODE_NTBDISABLED:
    break;
  case IDSOPT_DBG_S_P3_NTB_MODE_NTBPRIMARY:
    break;
  case IDSOPT_DBG_S_P3_NTB_MODE_NTBSECONDARY:
    break;
  case IDSOPT_DBG_S_P3_NTB_MODE_NTBRANDOM:
    break;
  case IDSOPT_DBG_S_P3_NTB_MODE_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

//  Link Speed
IDS_NV_READ_SKIP (IDSNVID_DBG_S_P3_NTB_LINK_SPEED, &IdsNvValue) {
  switch (IdsNvValue) {
  switch (PcdGet8 (PcdCfgNTBLinkSpeed)) {
  case IDSOPT_DBG_S_P3_NTB_LINK_SPEED_MAXSPEED:
    break;
  case IDSOPT_DBG_S_P3_NTB_LINK_SPEED_GEN1:
    break;
  case IDSOPT_DBG_S_P3_NTB_LINK_SPEED_GEN2:
    break;
  case IDSOPT_DBG_S_P3_NTB_LINK_SPEED_GEN3:
    break;
  case IDSOPT_DBG_S_P3_NTB_LINK_SPEED_AUTO:
    break;
  default:
    ASSERT (FALSE);
    break;
  }
}

