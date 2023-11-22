/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Dharma CPU Register definition
 *
 * Contains the definition of the CPU CPUID MSRs and PCI registers with BKDG recommended values
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Ccx/Sat
 *
 */
/*
 ******************************************************************************
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

#ifndef _CDD_REGISTERS_DHARMA_H_
#define _CDD_REGISTERS_DHARMA_H_
#pragma pack (push, 1)

#ifndef CDD_SPACE
  #define  CDD_SPACE(CDDINDEX, ADDRESS)  (0x80000000 + (CDDINDEX << 28) + ADDRESS)
#endif

#ifndef UMC_SPACE
  #define  UMC_SPACE(CDDINDEX, CHANNEL, ADDRESS)  (0x80000000 + (CDDINDEX << 28) + (CHANNEL << 20) + ADDRESS)
#endif

#ifndef CCX_SPACE_HYEX
  #define  CCX_SPACE_HYEX(CDDINDEX, CCXINDEX, COREINDEX, ADDRESS)   (0x80000000 + (CDDINDEX << 28) + (CCXINDEX << 22) + (COREINDEX << 17) + ADDRESS)
#endif

#ifndef CCX_SPACE_HYGX
  #define  CCX_SPACE_HYGX(CDDINDEX, CCXINDEX, COREINDEX, ADDRESS)  ((COREINDEX < 4) ? \
                                                               (0x80000000 + (CDDINDEX << 28) + (CCXINDEX << 23) + (COREINDEX << 17) + ADDRESS) : \
                                                               (0x80000000 + (CDDINDEX << 28) + (CCXINDEX << 23) + 0x400000 + ((COREINDEX - 4) << 17) + ADDRESS))
#endif

#ifndef CCX_L3_SPACE_HYEX
  #define  CCX_L3_SPACE_HYEX(CDDINDEX, CCXINDEX, ADDRESS)   (0x80000000 + (CDDINDEX << 28) + (CCXINDEX << 22) + ADDRESS)
#endif

#ifndef CCX_L3_SPACE_HYGX
  #define  CCX_L3_SPACE_HYGX(CDDINDEX, CCXINDEX, ADDRESS)  (0x80000000 + (CDDINDEX << 28) + (CCXINDEX << 23) + ADDRESS)
#endif

// PWR Register Define

// Address
#define THREAD_ENABLE_ADDRESS_HYEX   0x5a818

#define THREAD_ENABLE_ADDRESS_HYGX   0x5a878
#define THREAD_ENABLE1_ADDRESS_HYGX  0x5a87C

// Type
#define THREAD_ENABLE_TYPE  TYPE_SMN

#define THREAD_ENABLE_ThreadEn_OFFSET  0
#define THREAD_ENABLE_ThreadEn_WIDTH   32
#define THREAD_ENABLE_ThreadEn_MASK    0xffffffff

/// THREAD_ENABLE
typedef union {
  struct {
    UINT32    ThreadEn;                                                ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} THREAD_ENABLE_STRUCT;

// P state registers
#define PWR_PSATE0_LOW   0x5a820
#define PWR_PSATE0_HIGH  0x5a824
#define PWR_PSATE1_LOW   0x5a828
#define PWR_PSATE1_HIGH  0x5a82C
#define PWR_PSATE2_LOW   0x5a830
#define PWR_PSATE2_HIGH  0x5a834
#define PWR_PSATE3_LOW   0x5a838
#define PWR_PSATE3_HIGH  0x5a83C
#define PWR_PSATE4_LOW   0x5a840
#define PWR_PSATE4_HIGH  0x5a844
#define PWR_PSATE5_LOW   0x5a848
#define PWR_PSATE5_HIGH  0x5a84C
#define PWR_PSATE6_LOW   0x5a850
#define PWR_PSATE6_HIGH  0x5a854
#define PWR_PSATE7_LOW   0x5a858
#define PWR_PSATE7_HIGH  0x5a85C

// Address
#define THREAD_CONFIGURATION_ADDRESS_HYEX  0x5a81c

#define THREAD_CONFIGURATION_ADDRESS_HYGX  0x5a874

// Type
#define THREAD_CONFIGURATION_TYPE  TYPE_SMN

/// THREAD_CONFIGURATION
typedef union {
  struct {
    UINT32    CoreCount    : 4;                                        ///<
    UINT32    ComplexCount : 4;                                        ///<
    UINT32    SMTMode      : 1;                                        ///<
    UINT32    SMTDisable   : 1;                                        ///< 1-Disable SMT; 0-Enable SMT
    UINT32    Reserved1    :22;                                        ///<
  } HyExField;                                                         ///<
  
  struct {
    UINT32    CoreCount    : 4;                                        ///<
    UINT32    ComplexCount : 4;                                        ///<
    UINT32    SMTMode      : 2;                                        ///<
    UINT32    SMTDisable   : 2;                                        ///< 0-Disable SMT; 1-SMT2
    UINT32    Reserved1    : 20;                                       ///<
  } HyGxField;  
  
  UINT32 Value;                                                        ///<
} THREAD_CONFIGURATION_STRUCT;

// Downcore Control Register Address (MSRC001_0290)
#define PWR_DOWNCORE_CNTL_ADDRESS  0x5a860
#define PWR_SOCID_ADDRESS          0x5a86C
#define PWR_CORE_ENABLE_ADDRESS    0x5a870

// Register Name RSMU_PWRMGT_INTR_STATUS_PWR

// Address
#define RSMU_PWRMGT_INTR_STATUS_PWR_ADDRESS  0x100A170

// Type
#define RSMU_PWRMGT_INTR_STATUS_PWR_TYPE  TYPE_SMN

#define RSMU_PWRMGT_INTR_STATUS_PWR_RSMU_PWRMGT_INTR_STATUS_OFFSET  0
#define RSMU_PWRMGT_INTR_STATUS_PWR_RSMU_PWRMGT_INTR_STATUS_WIDTH   16
#define RSMU_PWRMGT_INTR_STATUS_PWR_RSMU_PWRMGT_INTR_STATUS_MASK    0xffff
#define RSMU_PWRMGT_INTR_STATUS_PWR_Reserved_31_16_OFFSET           16
#define RSMU_PWRMGT_INTR_STATUS_PWR_Reserved_31_16_WIDTH            16
#define RSMU_PWRMGT_INTR_STATUS_PWR_Reserved_31_16_MASK             0xffff0000L

/// RSMUPWRMMIO_00000170
typedef union {
  struct {
    UINT32    RSMU_PWRMGT_INTR_STATUS : 16;                            ///<
    UINT32    Reserved_31_16          : 16;                            ///<
  } Field;                                                             ///<
  UINT32    Value;                                                     ///<
} RSMU_PWRMGT_INTR_STATUS_PWR_STRUCT;

// UMC Register Define
#define UMC0_CH_REG_BASE      0x00050000
#define UMC0_SUBCH1_REG_BASE  0x00052000
#define UMC1_CH_REG_BASE      0x00150000
#define UMC1_SUBCH1_REG_BASE  0x00152000
#define UMC2_CH_REG_BASE      0x00250000
#define UMC2_SUBCH1_REG_BASE  0x00252000

#define UMC_CONFIG            0x00000100
#define UMC_SDP_CTRL          0x00000104
#define SDP_CTRL_SDPINIT      BIT31
#define UMC_ECC_CTRL          0x0000014C
#define UMC_MISC_CFG          0x000001E0
#define UMC_ECC_ERR_INJ_CTRL  0x00000D88
#define ECC_ERR_ADDR_EN       BIT2
#define UMC_ECC_ERR_INJ       0x00000DD0

// CCX Register Define
#define CCX0_PMC_PLR_ADDRESS  0x080010BC

#define CCX0_PMREG_INITPKG1_ADDRESS  0x08002FD4
#define COLD_RESET_MBIST_EN_OFFSET   25
#define COLD_RESET_MBIST_EN          BIT25

#define CCX0_CHL3_INITPKG1_ADDRESS  0x08080020
#define BIST_EN_OFFSET              4
#define BIST_EN                     BIT4

#define CCX0_CHL3_FREQSTAT_ADDRESS  0x08080064
#define BIST_DONE_HYEX               BIT9
#define BIST_PF_HYEX                 BIT8

#define BIST_DONE_HYGX               BIT13
#define BIST_PF_HYGX                 BIT12

// PSP & SMU Registers
#define MP5_PUBLIC_REG_HIGH_ADDRESS  0xFFFFFFFF00000000ULL
#define MP6_PUBLIC_REG_HIGH_ADDRESS  0xFFFFFFFF00100000ULL

#pragma pack (pop)
#endif // _CDD_REGISTERS_DHARMA_H_
