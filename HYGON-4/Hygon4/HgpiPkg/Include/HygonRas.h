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

#ifndef _HYGON_RAS_H_
#define _HYGON_RAS_H_

#undef BIT0
#undef BIT1
#undef BIT2
#undef BIT3
#undef BIT4
#undef BIT5
#undef BIT6
#undef BIT7
#undef BIT8
#undef BIT9
#undef BIT10
#undef BIT10
#undef BIT11
#undef BIT12
#undef BIT13
#undef BIT14
#undef BIT15
#undef BIT16
#undef BIT17
#undef BIT18
#undef BIT19
#undef BIT20
#undef BIT21
#undef BIT22
#undef BIT23
#undef BIT24
#undef BIT25
#undef BIT26
#undef BIT27
#undef BIT28
#undef BIT29
#undef BIT30
#undef BIT31
#undef BIT32
#undef BIT33
#undef BIT34
#undef BIT35
#undef BIT36
#undef BIT37
#undef BIT38
#undef BIT39
#undef BIT40
#undef BIT41
#undef BIT42
#undef BIT43
#undef BIT44
#undef BIT45
#undef BIT46
#undef BIT47
#undef BIT48
#undef BIT49
#undef BIT40
#undef BIT41
#undef BIT42
#undef BIT43
#undef BIT44
#undef BIT45
#undef BIT46
#undef BIT47
#undef BIT48
#undef BIT49
#undef BIT50
#undef BIT51
#undef BIT52
#undef BIT53
#undef BIT54
#undef BIT55
#undef BIT56
#undef BIT57
#undef BIT58
#undef BIT59
#undef BIT60
#undef BIT61
#undef BIT62
#undef BIT63

#define BIT0   (0x0000000000000001ull)
#define BIT1   (0x0000000000000002ull)
#define BIT2   (0x0000000000000004ull)
#define BIT3   (0x0000000000000008ull)
#define BIT4   (0x0000000000000010ull)
#define BIT5   (0x0000000000000020ull)
#define BIT6   (0x0000000000000040ull)
#define BIT7   (0x0000000000000080ull)
#define BIT8   (0x0000000000000100ull)
#define BIT9   (0x0000000000000200ull)
#define BIT10  (0x0000000000000400ull)
#define BIT11  (0x0000000000000800ull)
#define BIT12  (0x0000000000001000ull)
#define BIT13  (0x0000000000002000ull)
#define BIT14  (0x0000000000004000ull)
#define BIT15  (0x0000000000008000ull)
#define BIT16  (0x0000000000010000ull)
#define BIT17  (0x0000000000020000ull)
#define BIT18  (0x0000000000040000ull)
#define BIT19  (0x0000000000080000ull)
#define BIT20  (0x0000000000100000ull)
#define BIT21  (0x0000000000200000ull)
#define BIT22  (0x0000000000400000ull)
#define BIT23  (0x0000000000800000ull)
#define BIT24  (0x0000000001000000ull)
#define BIT25  (0x0000000002000000ull)
#define BIT26  (0x0000000004000000ull)
#define BIT27  (0x0000000008000000ull)
#define BIT28  (0x0000000010000000ull)
#define BIT29  (0x0000000020000000ull)
#define BIT30  (0x0000000040000000ull)
#define BIT31  (0x0000000080000000ull)
#define BIT32  (0x0000000100000000ull)
#define BIT33  (0x0000000200000000ull)
#define BIT34  (0x0000000400000000ull)
#define BIT35  (0x0000000800000000ull)
#define BIT36  (0x0000001000000000ull)
#define BIT37  (0x0000002000000000ull)
#define BIT38  (0x0000004000000000ull)
#define BIT39  (0x0000008000000000ull)
#define BIT40  (0x0000010000000000ull)
#define BIT41  (0x0000020000000000ull)
#define BIT42  (0x0000040000000000ull)
#define BIT43  (0x0000080000000000ull)
#define BIT44  (0x0000100000000000ull)
#define BIT45  (0x0000200000000000ull)
#define BIT46  (0x0000400000000000ull)
#define BIT47  (0x0000800000000000ull)
#define BIT48  (0x0001000000000000ull)
#define BIT49  (0x0002000000000000ull)
#define BIT50  (0x0004000000000000ull)
#define BIT51  (0x0008000000000000ull)
#define BIT52  (0x0010000000000000ull)
#define BIT53  (0x0020000000000000ull)
#define BIT54  (0x0040000000000000ull)
#define BIT55  (0x0080000000000000ull)
#define BIT56  (0x0100000000000000ull)
#define BIT57  (0x0200000000000000ull)
#define BIT58  (0x0400000000000000ull)
#define BIT59  (0x0800000000000000ull)
#define BIT60  (0x1000000000000000ull)
#define BIT61  (0x2000000000000000ull)
#define BIT62  (0x4000000000000000ull)
#define BIT63  (0x8000000000000000ull)

#define MSR_APIC_BAR          (0x0000001BUL)
#define LAPIC_BASE_ADDR_MASK  (0x0000FFFFFFFFF000ULL)

#define APIC_ID_REG           (0x20)
#define APIC20_ApicId_Offset  (24)

#define MSR_SYSENTER_EIP               (0x00000176UL)
#define MSR_MCG_CAP                    (0x00000179UL)
#define MSR_MCG_STAT                   (0x0000017AUL)
#define MSR_MCG_RIPV                   BIT0
#define MSR_MCG_EIPV                   BIT1
#define MSR_HWCR                       (0xC0010015UL)
#define MSR_MCEXCEPREDIR               (0xC0010022UL)
#define MSR_SMITRIGIOCYCLE             (0xC0010056UL)
#define MSR_HWCR                       (0xC0010015UL)
#define MCA_STS_WREN_BIT               BIT18
#define MSR_MMIO_CFG_BASE              (0xC0010058UL)
#define MSR_SMM_BASE                   (0xC0010111UL)
#define MSR_LOCAL_SMI_STATUS           (0xC001011AUL)
#define MSR_PFEH_CFG                   (0xC0010120UL)
#define GENERATE_DEFERREDLVT_ON_EXIT   BIT1
#define GENERATE_THRESHOLDLVT_ON_EXIT  BIT2
#define GENERATE_MCE_ON_EXIT           BIT3
  #define SMI_EXITTYPE_MASK            (GENERATE_THRESHOLDLVT_ON_EXIT + GENERATE_DEFERREDLVT_ON_EXIT + \
                                        GENERATE_MCE_ON_EXIT)

#define MSR_PFEH_CLOAK_CFG     (0xC0010121UL)
#define MSR_PFEH_DEF_INT_MASK  (0xC0010122UL)

#define MCA_LEGACY_BASE          (0x00000400UL)
#define MCA_LEGACY_MAX_BANK      (32)
#define MCA_LEGACY_REG_PER_BANK  (4)
#define MCA_LEGACY_TOP_ADDR      (MCA_LEGACY_BASE + (MCA_LEGACY_MAX_BANK * MCA_LEGACY_REG_PER_BANK))
#define MCA_REG_OFFSET_MASK      (0x00000003)

#define MCA_EXTENSION_BASE          (0xC0002000UL)
#define MCA_EXTENSION_REG_PER_BANK  (0x10)
#define MCA_CTL_MASK_BASE           (0xC0010400UL)

#define SMCA_REG_OFFSET_MASK  (0x0000000F)
#define MCA_CTL_OFFSET        (0x00)
#define MCA_STATUS_OFFSET     (0x01)
#define MCA_ADDR_OFFSET       (0x02)
#define MCA_MISC0_OFFSET      (0x03)
#define MCA_CONFIG_OFFSET     (0x04)
#define MCA_IPID_OFFSET       (0x05)
#define MCA_SYND_OFFSET       (0x06)
#define MCA_DESTAT_OFFSET     (0x08)
#define MCA_DEADDR_OFFSET     (0x09)
#define MCA_MISC1_OFFSET      (0x0A)

// This is old x86 MCA address
#define LMCA_ADDR_REG    (0x00)
#define LMCA_STATUS_REG  (0x01)

#define MAX_DIE_SUPPORT            (8)
#define MAX_PCIE_PORT_SUPPORT_HYEX  (18)  
#define MAX_PCIE_PORT_SUPPORT_HYGX  (24)                     // Max PCI-E port support per Logical IOD.

#define DF_CFGADDRESSCNTL_FUNC    (0x00)
#define DF_CFGADDRESSCNTL_OFFSET  (0x84)

#define SMM_SAVE_STATE_OFFSET  (0xFE00)

#define DEVICE_WIDTH_x4                (4)
#define DEVICE_WIDTH_x8                (8)
#define ECC_SYMBOL_SIZE_x4             (0)
#define ECC_SYMBOL_SIZE_x8             (1)
#define ECC_BIT_INTERLEAVING_DISABLED  (0)
#define ECC_BIT_INTERLEAVING_ENABLED   (1)

/// ACPI define
#define BERT_SIG  SIGNATURE_32 ('B', 'E', 'R', 'T')

#define ERROR_TYPE_RAW      (1)
#define ERROR_TYPE_GENERIC  (2)

/// BERT Block Status Bits
#define ERROR_UNCORR_VALID       (1 << 0)
#define ERROR_CORR_VALID         (1 << 1)
#define MULT_UNCORR_ERROR_VALID  (1 << 2)
#define MULT_CORR_ERROR_VALID    (1 << 3)

/// ACPI 6.1 Table 18-343 Generic Error Data Entry
#define ERROR_RECOVERABLE         (0)
#define ERROR_SEVERITY_FATAL      (1)
#define ERROR_SEVERITY_CORRECTED  (2)
#define ERROR_NONE                (3)
/// ACPI 6.0
#define GENERIC_ERROR_REVISION  (0x0201)

/// Validation bits UEFI spec2.6
#define FRU_ID_VALID      (1 << 0)
#define FRU_STRING_VALID  (1 << 1)

#define RAS_PARITY_CONTROL_0_HYEX                (0x13B20000)
#define RAS_PARITY_CONTROL_1_HYEX                (0x13B20004)
#define RAS_GLOBAL_STATUS_LO_HYEX                (0x13B20020)
#define RAS_GLOBAL_STATUS_HI_HYEX                (0x13B20024)
#define RAS_GLOBAL_STATUS_LO_MASK_HYEX           (0x000000FF)
#define RAS_GLOBAL_STATUS_HI_MASK_HYEX           (0x000FFFFF)
#define PCIE0_UNCORR_ERR_STATUS_HYEX             (0x11100154)
#define PCIE1_UNCORR_ERR_STATUS_HYEX             (0x11500154)

#define MISC_RAS_CONTROL_HYEX                    (0x13B2015C)

#define PARITY_ERROR_STATUS_UNCORR_GRP_HYEX      (0x13B20600)
#define PARITY_ERROR_STATUS_CORR_GRP_HYEX        (0x13B2064C)
#define PARITY_COUNTER_CORR_GRP_HYEX             (0x13B20698)
#define PARITY_ERROR_STATUS_UCP_GRP_HYEX         (0x13B206E4)
#define PARITY_COUNTER_UCP_GRP_HYEX              (0x13B20718)
#define PCIE_PORT_ACTION_CONTROL_BASE_HYEX       (0x13B2017C)
#define PCIE_ACTION_CONTROL_OFFSET              (0x20)
#define NBIF0_ACTION_CONTROL_BASE_HYEX           (0x13B2037C)
#define NBIF1_ACTION_CONTROL_BASE_HYEX           (0x13B2039C)
#define PCIE2_PORTA_ACTION_CONTROL_BASE_HYEX     (0x13B203BC)
#define PCIE2_PORTB_ACTION_CONTROL_BASE_HYEX     (0x13B203DC)

#define NBIF_GDC_RAS_LEAF0_CTRL_HYEX             (0x0141F800)
#define NBIF_BIF_RAS_LEAF0_CTRL_HYEX             (0x10139000)


#define MAX_UNCORR_GRP_NUM_HYEX                  (19)
#define MAX_CORR_GRP_NUM_HYEX                    (19)
#define MAX_UCP_GRP_NUM_HYEX                     (13)

// PMIO BreakEvent Register for SMN errors
#define PMIO_SMN_BREAK_EVENT_HYEX                (0x02D01380)

#define RAS_PARITY_CONTROL_0_HYGX       (0x08020000)
#define RAS_PARITY_CONTROL_1_HYGX       (0x08020004)
#define RAS_GLOBAL_STATUS_LO_HYGX       (0x08020020)
#define RAS_GLOBAL_STATUS_HI_HYGX       (0x08020024)
#define RAS_GLOBAL_STATUS_LO_MASK_HYGX  (0x000000FF)
#define RAS_GLOBAL_STATUS_HI_MASK_HYGX  (0x3FFFFFFF)

#define MISC_RAS_CONTROL_HYGX                (0x0802015C)

#define PARITY_ERROR_STATUS_UNCORR_GRP_HYGX  (0x08020600)
#define PARITY_ERROR_STATUS_CORR_GRP_HYGX    (0x08020654)
#define PARITY_COUNTER_CORR_GRP_HYGX         (0x080206A8)
#define PARITY_ERROR_STATUS_UCP_GRP_HYGX     (0x080206FC)
#define PARITY_COUNTER_UCP_GRP_HYGX          (0x08020738)
#define PCIE0_PORT_ACTION_CONTROL_BASE_HYGX  (0x0802017C)
#define PCIE1_PORT_ACTION_CONTROL_BASE_HYGX  (0x0802027C)

#define NBIF0_ACTION_CONTROL_BASE_HYGX       (0x0802037C)
#define NBIF2_ACTION_CONTROL_BASE_HYGX       (0x0802039C)
#define NBIF3_ACTION_CONTROL_BASE_HYGX       (0x080203BC)
#define PCIE2_PORT_ACTION_CONTROL_BASE_HYGX  (0x080203DC)

#define FST_ACTION_CONTROL_BASE_HYGX         (0x0802002C)

#define NBIF0_GDC_RAS_LEAF0_CTRL_HYGX  (0x04C1F800)
#define NBIF2_GDC_RAS_LEAF0_CTRL_HYGX  (0x0501F800)
#define NBIF3_GDC_RAS_LEAF0_CTRL_HYGX  (0x0541F800)

#define NBIF0_BIF_RAS_LEAF0_CTRL_HYGX  (0x06439000)
#define NBIF2_BIF_RAS_LEAF0_CTRL_HYGX  (0x06C39000)
#define NBIF3_BIF_RAS_LEAF0_CTRL_HYGX  (0x07039000)

#define MAX_UNCORR_GRP_NUM_HYGX  (21)
#define MAX_CORR_GRP_NUM_HYGX    (21)
#define MAX_UCP_GRP_NUM_HYGX     (15)

// PMIO BreakEvent Register for SMN errors
#define PMIO_SMN_BREAK_EVENT_HYGX  (0x03101380)

#define SMN_CATEGORY_SMN_PARITY_TIMEOUT_PSP_SMU_PARITY_ECC  (1 << 0)
#define SMN_CATEGORY_PSP_PARITY_ECC                         (1 << 2)
#define SMN_CATEGORY_SMN_TIMEOUT_SMU                        (1 << 3)
#define SMN_CATEGORY_SMN_LINK_PACKET_CRC_WITH_RETRY         (1 << 4)
#define SMN_CATEGORY_MASK                                   (SMN_CATEGORY_SMN_PARITY_TIMEOUT_PSP_SMU_PARITY_ECC | \
                                                             SMN_CATEGORY_PSP_PARITY_ECC | \
                                                             SMN_CATEGORY_SMN_TIMEOUT_SMU | \
                                                             SMN_CATEGORY_SMN_LINK_PACKET_CRC_WITH_RETRY)
//FST
#define FST_LINK_CONTROL_HYGX   0xC000030
#define FST_LINK_STATUS_HYGX    0xC000034

// FCH
#define SATA_PAR_ERR_CNXT_STS   (BIT10)
#define SATA_PAR_ERR_H2D_STS    (BIT9)
#define SATA_PAR_ERR_D2H2D_STS  (BIT8)
#define SATA_PAR_ERR_STS        (SATA_PAR_ERR_CNXT_STS + SATA_PAR_ERR_H2D_STS + SATA_PAR_ERR_D2H2D_STS)

// DF
#define DF_DFF6_REG_BASE            (0x0001D800)
#define DRAM_SCRUBBER_ERRORADDR_LO  (0x00000040)
#define ERR_INJ_EN                  (BIT0)
#define DRAM_SCRUBBER_ERRORADDR_HI  (0x00000044)
#define DRAM_SCRUBBER_BASEADDR_REG  (0x00000048)
#define DRAM_SCRUBBER_LIMIT_ADDR    (0x0000004C)          // DramScrubLimitAddr
#define DRAM_SCRUBBER_ADDR_LO       (0x00000050)          // DramScrubAddrLo
#define DRAM_SCRUBBER_ADDR_HI       (0x00000054)          // DramScrubAddrHi

// NBIO

// Structure

typedef struct _SMM_SAVE_STATE {
  UINT64    ES[2];                           // FE00
  UINT64    CS[2];                           // FE10
  UINT64    SS[2];                           // FE20
  UINT64    DS[2];                           // FE30
  UINT64    FS[2];                           // FE40
  UINT64    GS[2];                           // FE50
  UINT64    GDTR[2];                         // FE60
  UINT64    LDTR[2];                         // FE70
  UINT64    IDTR[2];                         // FE80
  UINT64    TR[2];                           // FE90
  UINT64    IO_RESTAERT_RIP;                 // FEA0
  UINT64    IO_RESTAERT_RCX;                 // FEA8
  UINT64    IO_RESTAERT_RSI;                 // FEB0
  UINT64    IO_RESTAERT_RDI;                 // FEB8
  UINT32    TrapoFFSET;                      // FEC0
  UINT32    LocalSmiStatus;                  // FEC4
  UINT8     IoRestart;                       // FEC8
  UINT8     AutoHalt;                        // FEC9
  UINT8     NmiMask;                         // FECA
  UINT8     Reserved1[5];                    // FECB
  UINT64    EFER;                            // FED0
  UINT64    SvmState;                        // FED8
  UINT64    GuestVMCBPyysicalAddress;        // FEE0
  UINT64    SVMVirtualInterruptControl;      // FEE8
  UINT8     Reserved2[12];                   // FEF0
  UINT32    SMMRevId;                        // FEFC
  UINT32    SMBASE;                          // FF00
  UINT8     Reserved3[28];                   // FF04
  UINT64    GuestPAT;                        // FF20
  UINT64    HostEFER;                        // FF28
  UINT64    HostCR4;                         // FF30
  UINT64    NestedCR3;                       // FF38
  UINT64    HostCR0;                         // FF40
  UINT64    CR4;                             // FF48
  UINT64    CR3;                             // FF50
  UINT64    CR0;                             // FF58
  UINT64    DR7;                             // FF60
  UINT64    DR6;                             // FF68
  UINT64    RFLAGS;                          // FF70
  UINT64    RIP;                             // FF78
  UINT64    R15;                             // FF80
  UINT64    R14;                             // FF88
  UINT64    R13;                             // FF90
  UINT64    R12;                             // FF98
  UINT64    R11;                             // FFA0
  UINT64    R10;                             // FFA8
  UINT64    R9;                              // FFB0
  UINT64    R8;                              // FFB8
  UINT64    RDI;                             // FFC0
  UINT64    RSI;                             // FFC8
  UINT64    RBP;                             // FFD0
  UINT64    RSP;                             // FFD8
  UINT64    RBX;                             // FFE0
  UINT64    RDX;                             // FFE8
  UINT64    RCX;                             // FFF0
  UINT64    RAX;                             // FFF8
} SMM_SAVE_STATE;

/// RAS MCA Bank enumerate
typedef enum _HYGON_MCA_BANK_NUM {
  // CORE MCA Banks, access through each thread.
  MCA_LS_BANK = 0,        ///</ 0  = MCA LS
  MCA_IF_BANK,            ///</ 1  = MCA IF
  MCA_L2_BANK,            ///</ 2  = MCA L2
  MCA_DE_BANK,            ///</ 3  = MCA DE
  MCA_EMPTY0_BANK,        ///</ 4  = EMPTY BANK
  MCA_EX_BANK,            ///</ 5  = MCA EX
  MCA_FP_BANK,            ///</ 6  = MCA FP

  // NON-CORE MCA banks, access from logic core 0 from each Die

  // MCA L3 CCXx SLIDEx Bank number define
  MCA_L3_C0_S0_BANK,      ///</ 7  = MCA L3 CCX 0 SLIDE 0
  MCA_L3_C0_S1_BANK,      ///</ 8  = MCA L3 CCX 0 SLIDE 1
  MCA_L3_C0_S2_BANK,      ///</ 9  = MCA L3 CCX 0 SLIDE 2
  MCA_L3_C0_S3_BANK,      ///</ 10  = MCA L3 CCX 0 SLIDE 3
  MCA_L3_C1_S0_BANK,      ///</ 11  = MCA L3 CCX 1 SLIDE 0
  MCA_L3_C1_S1_BANK,      ///</ 12  = MCA L3 CCX 1 SLIDE 1
  MCA_L3_C1_S2_BANK,      ///</ 13  = MCA L3 CCX 1 SLIDE 2
  MCA_L3_C1_S3_BANK,      ///</ 14  = MCA L3 CCX 1 SLIDE 3
  MCA_L3_C2_S0_BANK,      ///</ 15  = MCA L3 CCX 2 SLIDE 0
  MCA_L3_C2_S1_BANK,      ///</ 16  = MCA L3 CCX 2 SLIDE 1
  MCA_L3_C2_S2_BANK,      ///</ 17  = MCA L3 CCX 2 SLIDE 2
  MCA_L3_C2_S3_BANK,      ///</ 18  = MCA L3 CCX 2 SLIDE 3
  MCA_L3_C3_S0_BANK,      ///</ 19  = MCA L3 CCX 3 SLIDE 0
  MCA_L3_C3_S1_BANK,      ///</ 20  = MCA L3 CCX 3 SLIDE 1
  MCA_L3_C3_S2_BANK,      ///</ 21  = MCA L3 CCX 3 SLIDE 2
  MCA_L3_C3_S3_BANK,      ///</ 22  = MCA L3 CCX 3 SLIDE 3
  MCA_UMC0_SubChnl0_BANK, ///</ 23  = MCA UMC0 CHANNEL 0
  MCA_UMC0_SubChnl1_BANK, ///</ 24  = MCA UMC0 CHANNEL 1
  MCA_UMC1_SubChnl0_BANK, ///</ 25  = MCA UMC1 CHANNEL 0
  MCA_UMC1_SubChnl1_BANK, ///</ 26  = MCA UMC1 CHANNEL 1
  MCA_UMC2_SubChnl0_BANK, ///</ 27  = MCA UMC2 CHANNEL 0
  MCA_UMC2_SubChnl1_BANK, ///</ 28  = MCA UMC2 CHANNEL 1
  MCA_SMU_CDD_BANK,       ///</ 29  = MCA SMU
  MCA_PSP_CDD_BANK,       ///</ 30  = MCA PSP
  MCA_PB_CDD_BANK,        ///</ 31  = MCA
  MCA_CS0_BANK,           ///</ 32  = MCA
  MCA_CS1_BANK,           ///</ 33  = MCA
  MCA_CS2_BANK,           ///</ 34  = MCA
  MCA_PIE_CDD_BANK,       ///</ 35  = MCA
  MCA_SMU_IOD_BANK,       ///</ 36  = MCA
  MCA_PSP_IOD_BANK,       ///</ 37  = MCA
  MCA_PB_IOD_BANK,        ///</ 38  = MCA
  MCA_PIE_IOD_BANK        ///</ 39  = MCA
} HYGON_MCA_BANK_NUM;

/// RAS NBIO Group Type enumerate
typedef enum _NBIO_GRP_TYPE_NUM {
  NBIO_GRP_UNCORR = 0,      ///</ 0  = Uncorrectable Parity Group
  NBIO_GRP_CORR   = 1,      ///</ 1  = Correctable Parity Group
  NBIO_GRP_UCP    = 3,      ///</ 3  = Uncorrectable converts to poison data parity group
} NBIO_GRP_TYPE_NUM;

/// RAS MCA Bank enumerate
typedef enum _MCA_INT_TYPE_NUM {
  // CORE MCA Banks, access through each thread.
  MCA_NO_INTERRUPT = 0,      ///</ 0  = No Interrupt
  MCA_APIC,                  ///</ 1  = APIC based interrupt (LVT)
  MCA_SMI,                   ///</ 2  = SMI trigger event
  MCA_RESERVED,              ///</ 3  = Reserved
} MCA_INT_TYPE_NUM;

/// RAS SMN Category enumerate
typedef enum _SMN_CATEGORY_NUM {
  SMN_CATEGORY_UNCORR = 0,         ///</ 0  = Uncorrectable Category
  SMN_CATEGORY_CORR   = 1,         ///</ 1  = Correctable Category
  SMN_CATEGORY_FATAL  = 2,         ///</ 2  = Fatal Category
  SMN_CATEGORY_UCP    = 3,         ///</ 3  = Uncorrectable converts to poison data Category
} SMN_CATEGORY_NUM;

/// DRAM Limit Address Register
typedef union {
  struct {
    UINT64    IoTrapSts     : 4;            ///< [3:0] IO Trap Status
    UINT64                  : 4;            ///< [7:4] Reserved
    UINT64    MceRedirSts   : 1;            ///< [8] Machine check exception redirection status
    UINT64                  : 2;            ///< [10:9] Reserved
    UINT64    WrMsr         : 1;            ///< [11] SMM due to a WRMSR of an MCE_STATUS
    UINT64                  : 4;            ///< [15:12] Reserved
    UINT64    SmiSrcLvtLgcy : 1;            ///< [16] SMI source is legacy LVT APIC entry
    UINT64    SmiSrcLvtExt  : 1;            ///< [17] SMI source is APIC[530:500] LVT
    UINT64    SmiSrcMca     : 1;            ///< [18] SMI source is MCA
    UINT64                  : 54;           ///< [63:19] Reserved
  } Field;
  UINT64    Value;
} LOCAL_SMI_STATUS;

/// UMC CH registers
typedef union {
  struct {
    UINT32    RegchClkGateEn         : 1;   ///< Enable configuration register clock gating
    UINT32    DisDebugBusSel         : 1;   ///< Specific UMC debug bus configuration writes.
    UINT32    DisErrInj              : 1;   ///< Specific UMC error injection configuration writes.
    UINT32    AutoRefReorderArb      : 1;   ///<
    UINT32    AutoRefReorderRec      : 1;   ///<
    UINT32    AutoRefNoTimingQualRec : 1;   ///<
    UINT32    MPCBCEn                : 1;   ///<
    UINT32                           : 25;  ///< Reserved
  } Field;
  UINT32    Value;
} UMC_MISCCFG_REG;

typedef union {
  struct {
    UINT32    WrEccEn              : 1;     ///< Enables ECC generation for DRAM data.
    UINT32    EccReplayEn          : 1;     ///<
    UINT32    RplyFirstBeat        : 1;     ///<
    UINT32                         : 1;     ///< Reserved
    UINT32    EccBadDramSymEn      : 1;     ///< Enables the software managed ECC history mechanism for x8 symbol size.
    UINT32    EccHardwareHistoryEn : 1;     ///< Enables the hardware managed ECC history mechanism for x8 symbol size.
    UINT32    EccBitInterleaving   : 1;     ///<
    UINT32    EccSymbolSize        : 2;     ///< 0=x4 symbol. 1=x8 symbol.
    UINT32    UCFatalEn            : 1;     ///< Promote uncorrectable errors from deferred to fatal.
    UINT32    RdEccEn              : 1;     ///< Enable DRAM data ECC checking and correction.
    UINT32    EccByteEnOvrRide     : 1;     ///<
    UINT32    PoisonFatalDis       : 1;     ///<
    UINT32    PoisonInhibit        : 1;     ///<
    UINT32    LegacyEccCodeSel     : 1;     ///<
    UINT32    RdCrcRptSdp          : 1;     ///<
    UINT32                         : 16;    ///< Reserved
  } Field;
  UINT32    Value;
} UMC_ECCCTRL_REG;

typedef union {
  struct {
    UINT32    EccErrPersistentEn : 1;       ///< Enable continuous error injection.
    UINT32    EccErrOneShotEn    : 1;       ///< Enable a single error injection.
    UINT32    EccErrAddrEn       : 1;       ///< Allow address-based injection from the DF CS.
    UINT32                       : 29;      ///< Reserved
  } Field;
  UINT32    Value;
} UMC_ECCERRINJCTRL_REG;

typedef union {
  struct {
    UINT8    DramEccErr         : 1;        ///< DRAM ECC error.
    UINT8    WriteDataPoisonErr : 1;        ///< Data poison error.
    UINT8    SdpParityErr       : 1;        ///< SDP parity error.
    UINT8    ApbErr             : 1;        ///< Advanced peripheral bus error.
    UINT8    AddrCmdParityErr   : 1;        ///< Address/command parity error.
    UINT8    WriteDataCrcErr    : 1;        ///< Write data CRC error.
    UINT8                       : 2;        ///< Reserved
  } Field;
  UINT8    Value;
} UMC_EXT_ERROR_CODE;

typedef union {
  struct {
    UINT32    EccInjVector : 16;            ///<
    UINT32    EccInjEn     : 10;            ///<
    UINT32                 : 6;             ///< Reserved
  } Field;
  UINT32    Value;
} UMC_ECCERRINJ_REG;

/// DF
/// DRAM Scrubber Base Address Register
typedef union {
  /// Bitfields of DRAM Scrubber Base Address Register
  struct {
    UINT32    DramScrubEn       : 1;   ///< DRAM Scrub Enable
    UINT32                      : 3;   ///< Reserved
    UINT32    DramScrubBaseAddr : 28;  ///< DRAM Scrub Base Address
  } Fields;
  UINT32    Value;
} DRAM_SCRUB_BASEADDR_REG;

/// DRAM Scrubber Error Address Lo Register
typedef union {
  /// Bitfields of DRAM Scrubber Error Address Lo Register
  struct {
    UINT32    ErrInjEn         : 1;         ///< A new command to inject error is triggered by writing a 1
    UINT32    ErrInjDone       : 1;         ///< Set by hardware once the write is sent to CS
    UINT32    LockAndDisErrInj : 1;         ///< Lock error injection widget
    UINT32                     : 3;         ///< Reserved
    UINT32    ErrInjAddrLo     : 26;        ///< Error injection address low
  } Fields;
  UINT32    Value;
} DRAM_SCRUB_ERRORADDR_LO_REG;

/// DRAM Scrubber Error Address Hi Register
typedef union {
  /// Bitfields of DRAM Scrubber Error Address Hi Register
  struct {
    UINT32    ErrInjAddrHi : 16;            ///< Error injection address high
    UINT32                 : 16;            ///< Reserved
  } Fields;
  UINT32    Value;
} DRAM_SCRUB_ERRORADDR_HI_REG;

/// DRAM Scrubber Address Lo Register
typedef union {
  /// Bitfields of DRAM Scrubber Address Lo Register
  struct {
    UINT32    MemInitEnable  : 1; ///< Enables memory initialization
    UINT32    MemInitDone    : 1; ///< This bit is set by hardware once it has cleared the memory
    UINT32    DisMemInit     : 1; ///< BIOS or PSP should set the bit when it wants to protect any other malicious software from clearing memory
    UINT32                   : 3; ///< Reserved
    UINT32    SeqScrubAddrLo : 26; ///< Scrubber Address Low
  } Fields;
  UINT32    Value;
} DRAM_SCRUB_ADDR_LO_REG;

/// DRAM Scrubber Address Hi Register
typedef union {
  /// Bitfields of DRAM Scrubber Address Hi Register
  struct {
    UINT32    SeqScrubAddrHi : 16; ///< Scrubber Address High
    UINT32                   : 16; ///< Reserved
  } Fields;
  UINT32    Value;
} DRAM_SCRUB_ADDR_HI_REG;

/// NBIO
typedef union {
  /// Bitfields of IOHCRAS Parity Control 0 Register
  struct {
    UINT32    ParityCorrThreshold : 16;     ///< The number of ucp parity errors logged before an action is triggered.
    UINT32    ParityUCPThreshold  : 16;     ///< The number of correctable parity errors logged before an action is triggered.
  } Fields;
  UINT32    Value;
} PARITY_CONTROL_0_REG;

/// NBIO IOHCRAS Parity Control 1 Register
typedef union {
  /// Bitfields of IOHCRAS Parity Control 1 Register
  struct {
    UINT32    ParityErrGenGroupSel     : 8; ///< Controls the parity error group that is the target of error injection or masking.
    UINT32    ParityErrGenGroupTypeSel : 1; ///< 0- Uncorrectable Group Type, 1- - Correctable Group Type.
    UINT32                             : 2; ///< Reserved.
    UINT32    ParityErrGenIdSel        : 5; ///< Selects a structure within the target parity group for error injection or masking.
    UINT32    ParityErrGenCmd          : 4; ///< 0000 - Reserved
                                            ///< 0001 - Error Injection on Write Side
                                            ///< 0010 - Error Injection on Read Side
                                            ///< 0011 - Reserved
                                            ///< 0100 - Disable Error Reporting
                                            ///< 0101 - Enable Error Reporting
                                            ///< 0110 - Reserved
                                            ///< 0111-1111 - Reserved
                                            ///< Encoding for ECC Parity Wrappers:
                                            ///< 0000 - Disable ECC Error Detection and Correction
                                            ///< 0001 - Enable ECC Error Detection and Correction
                                            ///< 0010 - SEC Error Injection
                                            ///< 0011 - Disable SEC Error Reporting
                                            ///< 0100 - Enable SEC Error Reporting
                                            ///< 0101 - DED Error Injection
                                            ///< 0110 - Disable DED Error Reporting
                                            ///< 0111 - Enable DED Error Reporting
                                            ///< 1000 - Disable Conversion of DED Errors to Poisoned Data
                                            ///< 1001 - Enable Conversion of DED Errors to Poisoned Data
                                            ///< 1010 - UCP Error Injection
                                            ///< 1011 - Disable UCP Error Reporting
                                            ///< 1100 - Enable UCP Error Reporting
    UINT32                            : 10; ///< Reserved.
    UINT32    ParityErrGenTrigger     : 1;  ///< Writing this bit to 1 triggers the command defined by the other ParityErrGen* fields in this register.
    UINT32    ParityErrGenInjectAllow : 1;  ///< Software must poll this bit and make sure this bit is asserted before updating the rest of the fields in this register.
  } Fields;
  UINT32    Value;
} PARITY_CONTROL_1_REG;

typedef union {
  /// Bitfields of IOHCRAS PCIE0/1PortA/H*_ACTION_CONTROL Register
  struct {
    UINT32    ApmlErrEn   : 1;              ///< 1 causes an interrupt to SMU/MP1 to be triggered when the associated error is logged.
    UINT32    IntrGenSel  : 2;              ///< A non-zero value causes an interrupt to be sent to the CPU complex when the associated error is logged.
    UINT32    LinkDisEn   : 1;              ///< 1 causes PCIe links to be disabled when the associated error is logged.
    UINT32    SyncFloodEn : 1;              ///< 1 causes an SDP ErrEvent to be triggered when the associated error is logged.
    UINT32                : 27;             ///< Reserved.
  } Fields;
  UINT32    Value;
} ERR_ACT_CTRL_REG;

// byo231031 + >>
#define BYO_RAS_POLICY_DATA_TAG   SIGNATURE_32('B', 'R', 'D', 'A')
#define BYO_RAS_POLICY_DATA_VER   2

typedef struct {
  UINT8             Valid;
  ERR_ACT_CTRL_REG  PciePortActList[8];
  ERR_ACT_CTRL_REG  NbifActConfigList[8];
} ERR_ACT_CTRL_REG_LIST;

typedef struct {
  UINT32                 Tag;
  UINT32                 Version;
  ERR_ACT_CTRL_REG_LIST  Eacr;
  BOOLEAN                ApeiDisable;
  BOOLEAN                ApeiEinjDisable;
  BOOLEAN                ApeiEinjCpuCeDis;
  UINT32                 PcieAerCeMask;
  UINT32                 PcieAerUceMask;
  UINT16                 McaNonMemErrThresh;
  UINT16                 McaErrThreshCount;
  UINT16                 McaErrThreshSwCount;
  UINT16                 LeakybucketMinites;
  UINT16                 LeakybucketOnceCount;
  UINT8                  Leakybucket24HClear;
  UINT8                  MemTinyStormThresholdCount;
  UINT16                 MemTinyStormTimeIntervalSec; 
  UINT16                 PcieCeThreshold;
} BYO_RAS_POLICY_DATA;

// byo231031 + <<



typedef union {
  /// Bitfields of NBIFRAS NBIF_RAS_LEAFx_CTRL, NBIFGDCRAS GDC_RAS_LEAFx_CTRL Register
  struct {
    UINT32    PoisonDetEn      : 1;        ///< Control bit for port poison data detection.
    UINT32    PoisonErrEventEn : 1;        ///< Control for sending error event when dectecting poison data.
    UINT32    PoisonStallEn    : 1;        ///< Control for stalling its egress port when dectecting poison data.
    UINT32                     : 1;        ///< Reserved
    UINT32    ParityDetEn      : 1;        ///< Control for port parity error detection.
    UINT32    ParityErrEventEn : 1;        ///< Control for sending error event when dectecting parity error.
    UINT32    ParityStallEn    : 1;        ///< Control for stalling its egress port when dectecting parity error.
    UINT32                     : 9;        ///< Reserved
    UINT32    ErrEventRecv     : 1;        ///< Status for SDP error event receive recording.
    UINT32    LinkDisRecv      : 1;        ///< Status for Link disable receive recording.
    UINT32    PoisonErrDet     : 1;        ///< Status for self-detected poison data recording.
    UINT32    ParityErrDet     : 1;        ///< Status for self-detected parity error recording.
    UINT32    ErrEventSent     : 1;        ///< Status for SDP error event sent recording.
    UINT32    EgressStalled    : 1;        ///< Status for egress stall recordin.
    UINT32                     : 10;       ///< Reserved
  } Fields;
  UINT32    Value;
} RAS_LEAF_CTRL_REG;

typedef union {
  /// Bitfields of NBIFRAS NBIF_RAS_LEAFx_CTRL, NBIFGDCRAS GDC_RAS_LEAFx_CTRL Register
  struct {
    UINT32 Pcie0PortA:1;                    ///< The status of errors from PCIE0 PortA
    UINT32 Pcie0PortB:1;                    ///< The status of errors from PCIE0 PortB
    UINT32 Pcie0PortC:1;                    ///< The status of errors from PCIE0 PortC
    UINT32 Pcie0PortD:1;                    ///< The status of errors from PCIE0 PortD
    UINT32 Pcie0PortE:1;                    ///< The status of errors from PCIE0 PortE
    UINT32 Pcie0PortF:1;                    ///< The status of errors from PCIE0 PortF
    UINT32 Pcie0PortG:1;                    ///< The status of errors from PCIE0 PortG
    UINT32 Pcie0PortH:1;                    ///< The status of errors from PCIE0 PortH
    UINT32 Pcie1PortA:1;                    ///< The status of errors from PCIE1 PortA
    UINT32 Pcie1PortB:1;                    ///< The status of errors from PCIE1 PortB
    UINT32 Pcie1PortC:1;                    ///< The status of errors from PCIE1 PortC
    UINT32 Pcie1PortD:1;                    ///< The status of errors from PCIE1 PortD
    UINT32 Pcie1PortE:1;                    ///< The status of errors from PCIE1 PortE
    UINT32 Pcie1PortF:1;                    ///< The status of errors from PCIE1 PortF
    UINT32 Pcie1PortG:1;                    ///< The status of errors from PCIE1 PortG
    UINT32 Pcie1PortH:1;                    ///< The status of errors from PCIE1 PortH
    UINT32 Nbif0PortA:1;                    ///< The status of errors from NBIF0 PortA.
    UINT32 Nbif1PortA:1;                    ///< The status of errors from NBIF1 PortA.
    UINT32 Pcie2PortA:1;                    ///< The status of errors from PCIE2 PortA
    UINT32 Pcie2PortB:1;                    ///< The status of errors from PCIE2 PortB
    UINT32 :12;                             ///< Reserved
  } Fields;
  UINT32  Value;
}RAS_GLOBAL_STS_HI_REG_HYEX;

typedef union {
  /// Bitfields of NBIFRAS NBIF_RAS_LEAFx_CTRL, NBIFGDCRAS GDC_RAS_LEAFx_CTRL Register
  struct {
    UINT32    Pcie0PortA : 1;               ///< The status of errors from PCIE0 PortA
    UINT32    Pcie0PortB : 1;               ///< The status of errors from PCIE0 PortB
    UINT32    Pcie0PortC : 1;               ///< The status of errors from PCIE0 PortC
    UINT32    Pcie0PortD : 1;               ///< The status of errors from PCIE0 PortD
    UINT32    Pcie0PortE : 1;               ///< The status of errors from PCIE0 PortE
    UINT32    Pcie0PortF : 1;               ///< The status of errors from PCIE0 PortF
    UINT32    Pcie0PortG : 1;               ///< The status of errors from PCIE0 PortG
    UINT32    Pcie0PortH : 1;               ///< The status of errors from PCIE0 PortH
    UINT32    Pcie1PortA : 1;               ///< The status of errors from PCIE1 PortA
    UINT32    Nbif0PortA : 1;               ///< The status of errors from NBIF0 PortA.
    UINT32    Nbif2PortA : 1;               ///< The status of errors from NBIF2 PortA.
    UINT32    Nbif3PortA : 1;               ///< The status of errors from NBIF3 PortA.
    UINT32    Pcie2PortA : 1;               ///< The status of errors from PCIE2 PortA
    UINT32    Pcie2PortB : 1;               ///< The status of errors from PCIE2 PortB
    UINT32    Pcie2PortC : 1;               ///< The status of errors from PCIE2 PortC
    UINT32    Pcie2PortD : 1;               ///< The status of errors from PCIE2 PortD
    UINT32    Pcie2PortE : 1;               ///< The status of errors from PCIE2 PortE
    UINT32    Pcie2PortF : 1;               ///< The status of errors from PCIE2 PortF
    UINT32    Pcie2PortG : 1;               ///< The status of errors from PCIE2 PortG
    UINT32    Pcie2PortH : 1;               ///< The status of errors from PCIE2 PortH
    UINT32               : 8;               ///< Reserved
    UINT32    Fstus      : 1;               ///< The status of errors from FSTus
    UINT32    Fstds      : 1;               ///< The status of errors from FSTds
    UINT32               : 2;               ///< Reserved
  } Fields;
  UINT32    Value;
} RAS_GLOBAL_STS_HI_REG_HYGX;

/// MCA
typedef union {
  struct {
    UINT64    ErrorCode    : 16;            ///<
    UINT64    ErrorCodeExt : 6;             ///<
    UINT64                 : 10;            ///< Reserved
    UINT64    ErrorCodeId  : 6;             ///<
    UINT64                 : 5;             ///< Reserved
    UINT64    Poison       : 1;             ///< 1 = The error was the result of attempting to consume poisoned data.
    UINT64    Deferred     : 1;             ///< 1 = A deferred error was created.
    UINT64    UECC         : 1;             ///< 1 = The error was an uncorrectable ECC error
    UINT64    CECC         : 1;             ///< 1 = The error was a correctable ECC error
    UINT64                 : 5;             ///< Reserved
    UINT64    Transparent  : 1;             ///< 1 = This error is a transparent error.
    UINT64    SyndV        : 1;             ///< 1 = This error logged information in MCA_SYND
    UINT64                 : 1;             ///< Reserved
    UINT64    TCC          : 1;             ///< 1 = The thread which consumed the error is not restartable and must be terminated.
    UINT64    ErrCoreIdVal : 1;             ///< 1 = The ErrCoreId field is valid
    UINT64    PCC          : 1;             ///< 1 = Hardware context held by the processor may have been corrupted
    UINT64    AddrV        : 1;             ///< 1 = MCA_ADDR contains adderss information
    UINT64    MiscV        : 1;             ///< 1 = Valid thresholding in MCA_MISC0
    UINT64    En           : 1;             ///< 1 = MCA error reporting is enabled for this error.
    UINT64    UC           : 1;             ///< 1 = The error was not corrected by hardware
    UINT64    Overflow     : 1;             ///< 1 = An error was detected while the valid bit was set
    UINT64    Val          : 1;             ///< 1 = A valid error has been detected
  } Field;
  UINT64    Value;
} MCA_STATUS_MSR;

typedef union {
  struct {
    UINT64             : 44;                ///< Reserved
    UINT64    Deferred : 1;                 ///< 1 = A deferred error was created.
    UINT64             : 8;                 ///< Reserved
    UINT64    SyndV    : 1;                 ///< 1 = This error logged information in MCA_SYND
    UINT64             : 4;                 ///< Reserved
    UINT64    AddrV    : 1;                 ///< 1 = MCA_ADDR contains adderss information
    UINT64             : 3;                 ///< Reserved
    UINT64    Overflow : 1;                 ///< 1 = An error was detected while the valid bit was set
    UINT64    Val      : 1;                 ///< 1 = A valid error has been detected
  } Field;
  UINT64    Value;
} MCA_DESTAT_MSR;

typedef union {
  struct {
    UINT64    ErrorAddr : 56;               ///< Error Address
    UINT64    LSB       : 6;                ///< Least significant valid bit
    UINT64              : 2;                ///< Reserved
  } Field;
  UINT64    Value;
} MCA_ADDR_MSR;

typedef union {
  struct {
    UINT64                     : 24;         ///< Reserved
    UINT64    BlkPtr           : 8;          ///< 01h=Extended MSR block is valid.
    UINT64    ErrCnt           : 12;         ///< Error Counter
    UINT64                     : 4;          ///< Reserved
    UINT64    Ovrflw           : 1;          ///< Set by hardware when ErrCnt transitions from FFEh to FFFh.
    UINT64    ThresholdIntType : 2;          ///< Type of interrupt signal
    UINT64    CntEn            : 1;          ///< 1=Count thresholding errors.
    UINT64    LvtOffset        : 4;          ///< Error thresholding interrupt LVT address
    UINT64                     : 4;          ///< Reserved
    UINT64    IntP             : 1;          ///< 1=ThresholdIntType can be used to generate interrupts.
    UINT64    Locked           : 1;          ///< 1=Writes to this register are ignored
    UINT64    CntP             : 1;          ///< 1=A valid threshold counter is present
    UINT64    Valid            : 1;          ///< 1=A valid CntP field is present in this register.
  } Field;
  UINT64    Value;
} MCA_MISC0_MSR;

typedef union {
  struct {
    UINT64                     : 24;         ///< Reserved
    UINT64    BlkPtr           : 8;          ///< 01h=Extended MSR block is valid.
    UINT64    ErrCnt           : 12;         ///< Error Counter
    UINT64                     : 4;          ///< Reserved
    UINT64    Ovrflw           : 1;          ///< Set by hardware when ErrCnt transitions from FFEh to FFFh.
    UINT64    ThresholdIntType : 2;          ///< Type of interrupt signal
    UINT64    CntEn            : 1;          ///< 1=Count thresholding errors.
    UINT64                     : 8;          ///< Reserved
    UINT64    IntP             : 1;          ///< 1=ThresholdIntType can be used to generate interrupts.
    UINT64    Locked           : 1;          ///< 1=Writes to this register are ignored
    UINT64    CntP             : 1;          ///< 1=A valid threshold counter is present
    UINT64    Valid            : 1;          ///< 1=A valid CntP field is present in this register.
  } Field;
  UINT64    Value;
} MCA_MISC1_MSR;

typedef union {
  struct {
    UINT64    McaX                             : 1;     ///< Reserved
    UINT64    TransparentErrorLoggingSupported : 1;     ///< 01h=Extended MSR block is valid.
    UINT64    DeferredErrorLoggingSupported    : 1;     ///< Error Counter
    UINT64    FlagMcaCorrErrSupported          : 1;     ///<
    UINT64    ArmLoggingModeSupported          : 1;     ///<
    UINT64    DeferredIntTypeSupported         : 1;     ///< Set by hardware when ErrCnt transitions from FFEh to FFFh.
    UINT64                                     : 26;    ///< Reserved
    UINT64    McaXEnable                       : 1;     ///< Type of interrupt signal
    UINT64    TransparentErrorLoggingEnable    : 1;     ///< 1=Count thresholding errors.
    UINT64    LogDeferredInMcaStat             : 1;     ///< Log Deferred error in MCA_STATUS
    UINT64    FlagMcaCorrErr                   : 1;     ///<
    UINT64    LoggingModeIsArm                 : 1;     ///<
    UINT64    DeferredIntType                  : 2;     ///< Type of Deferred error interrupt.
    UINT64                                     : 25;    ///< Reserved
  } Field;
  UINT64    Value;
} MCA_CONFIG_MSR;

// Syndrome Types
#define MCA_SYND_TYPE_DRAM_ERR      0x00
#define MCA_SYND_TYPE_CACHE_ERR     0x01
#define MCA_SYND_TYPE_INTERNAL_ERR  0x02

typedef union {
  struct {
    UINT64    ErrorInformation : 18;        ///< 01h=Extended MSR block is valid.
    UINT64    Length           : 6;         ///< Error Counter
    UINT64    ErrorPriority    : 3;         ///< Set by hardware when ErrCnt transitions from FFEh to FFFh.
    UINT64                     : 1;         ///< Reserved
    UINT64    Type             : 4;         ///< Type
    UINT64    Syndrome         : 32;        ///< Type of interrupt signal
  } Field;
  UINT64    Value;
} MCA_SYND_MSR;

// MCA Hardware ID
#define MCA_CPU_CORE_ID  (0x0B0)
#define LS_MCA_TYPE      (0x0000)
#define IF_MCA_TYPE      (0x0001)
#define L2_MCA_TYPE      (0x0002)
#define DE_MCA_TYPE      (0x0003)
#define EX_MCA_TYPE      (0x0005)
#define FP_MCA_TYPE      (0x0006)

#define MCA_L3_ID    (0x0B0)
#define L3_MCA_TYPE  (0x0007)

#define MCA_DATA_FABRIC_ID  (0x02E)
#define CS_MCA_TYPE         (0x0000)
#define PIE_MCA_TYPE        (0x0001)

#define MCA_UMC_ID              (0x096)
#define UMC_MCA_TYPE            (0x0000)
#define UMC0_SubCh0_MCA_INS_ID  (0x60050F00)
#define UMC0_SubCh1_MCA_INS_ID  (0x60052F00)
#define UMC1_SubCh0_MCA_INS_ID  (0x60150F00)
#define UMC1_SubCh1_MCA_INS_ID  (0x60152F00)
#define UMC2_SubCh0_MCA_INS_ID  (0x60250F00)          // byo231103 +
#define UMC2_SubCh1_MCA_INS_ID  (0x60252F00)          // byo231103 +

#define MCA_PARAMETER_BLOCK_ID  (0x005)
#define PB_MCA_TYPE             (0x0000)

#define MCA_PSP_ID    (0x0FF)
#define PSP_MCA_TYPE  (0x0000)

#define MCA_SMU_ID    (0x001)
#define SMU_MCA_TYPE  (0x0000)

#define MCA_INSTANCE_ID_END  (0xFFFF)

#define INSTANCEID_NOT_FOUND  (0xFF)

typedef union {
  struct {
    UINT64    InstanceId : 32;              ///< The instance ID of this IP.
    UINT64    HardwareID : 12;              ///< The Hardware ID of the IP associated with this MCA bank.
    UINT64               : 4;               ///< Reserved
    UINT64    McaType    : 16;              ///< The McaType of the MCA bank within this IP.
  } Field;
  UINT64    Value;
} MCA_IPID_MSR;

typedef struct _CPU_INFO {
  UINTN    ProcessorNumber;
  UINT8    SocketId;
  UINT8    CddId;
  UINT8    CcxId;
  UINT8    CoreId;
  UINT8    ThreadID;
} CPU_INFO;

typedef struct _MCA_BANK_ERROR_INFO {
  UINTN             McaBankNumber;
  MCA_STATUS_MSR    McaStatusMsr;
  MCA_ADDR_MSR      McaAddrMsr;
  MCA_DESTAT_MSR    McaDeStatMsr;
  MCA_ADDR_MSR      McaDeAddrMsr;
  MCA_CONFIG_MSR    McaConfigMsr;
  MCA_SYND_MSR      McaSyndMsr;
  MCA_MISC0_MSR     McaMisc0Msr;
  MCA_MISC1_MSR     McaMisc1Msr;
  MCA_IPID_MSR      McaIpidMsr;
} MCA_BANK_ERROR_INFO;

typedef struct _RAS_MCA_ERROR_INFO {
  CPU_INFO               CpuInfo;
  MCA_BANK_ERROR_INFO    McaBankErrorInfo[40];
  UINT8                  BankIndex;
  UINT8                  CeIgnoreBank;
  UINT8                  DimmIndex;
  UINT16                 DimmCeThru;  
} RAS_MCA_ERROR_INFO;

typedef struct _RAS_NBIO_ERROR_INFO {
  UINT8                Socket;
  UINT8                LogicalDie;
  UINT8                IohubPhysicalDie;
  UINT8                Rb;
  UINT8                BusNumber;
  NBIO_GRP_TYPE_NUM    TypeId;
  UINT8                GroupId;
  UINT32               NbioGlobalStatusLo;
  UINT32               NbioGlobalStatusHi;
  UINT32               NbioParityErrorStsAddr;
  UINT32               NbioParityErrorSts;
} RAS_NBIO_ERROR_INFO;

/// Request the identity of dimm from system address
typedef struct {
  UINT64    normalizedAddr;             ///< Error Address that needs to be translated to dimm identification.
  UINT8     normalizedSocketId;         ///< The socket on which the targeted address locates.
  UINT8     normalizedCddId;            ///< The Cdd on which the targeted address locates.
  UINT8     normalizedChannelId;        ///< The channel on which the targeted address locates.
  UINT8     normalizedSubChannelId;     ///< The sub channel on which the targeted address locates.
} NORMALIZED_ADDRESS;

typedef struct {
  UINT8     ChipSelect;                 ///< The chip select on which the targeted address locates.
  UINT8     Bank;                       ///< The Bank for which the error address resides
  UINT32    Row;                        ///< The Row for which the error address resides
  UINT16    Column;                     ///< The Column for which the error address resides
  UINT8     rankmul;                    ///< The Rank Multiplex for which the error address resides
  UINT8     DimmId;                     ///< Reserved
} DIMM_INFO;

typedef struct _RAS_THRESHOLD_CONFIG {
  BOOLEAN    ThresholdControl;
  UINT16     ThresholdCount;
  UINT16     NonMemThreshCount;
  UINT8      ThresholdIntType;
} RAS_THRESHOLD_CONFIG;

typedef union {
  struct {
    UINT32    BusId        : 1;         ///< [0] Bus ID
    UINT32    ErrorSource  : 1;         ///< [1] Error Source
    UINT32    ErrorType    : 1;         ///< [2] Error Type
    UINT32    GroupType    : 1;         ///< [3] Group Type
    UINT32    GroupId      : 1;         ///< [4] Group ID
    UINT32    NbifId       : 1;         ///< [5] Leaf ID
    UINT32    LeafId       : 1;         ///< [6] Leaf ID
    UINT32    ParityErrSts : 1;         ///< [7] Parity Error Status
    UINT32                 : 24;        ///< [31:8] Reserved
  } Field;
  UINT32    Value;
} NBIO_ERR_VALID_BIT;

typedef union {
  struct {
    UINT8    Nbio     : 1;             ///< [0] NBIO
    UINT8    NbifMm   : 1;             ///< [1] NBIFMM
    UINT8    SyshubMm : 1;             ///< [2] SYSHUBMM
    UINT8             : 5;             ///< [7:3] Reserved
  } Field;
  UINT8    Value;
} NBIO_ERR_SRC;

typedef union {
  struct {
    UINT8    ParityErrCorr     : 1;    ///< [0] Correctable Parity Error
    UINT8    ParityErrNonFatal : 1;    ///< [1] Non-Fatal Parity Error
    UINT8    ParityErrFatal    : 1;    ///< [2] Fatal Parity Error
    UINT8    ParityErrSerr     : 1;    ///< [3] Serr Parity Error
    UINT8    PoisonError       : 1;    ///< [4] Poisoned Error
    UINT8                      : 3;    ///< [7:5] Reserved
  } Field;
  UINT8    Value;
} NBIO_ERR_TYPE;

typedef union {
  struct {
    UINT32    BusId       : 1;          ///< [0] Bus ID
    UINT32    ErrorType   : 1;          ///< [1] Error Type
    UINT32    ErrorSource : 1;          ///< [2] Error Source
    UINT32                : 29;         ///< [31:3] Reserved
  } Field;
  UINT32    Value;
} SMN_ERROR_VALID_BIT;

typedef union {
  struct {
    UINT8    SmnParitySmnPspTimeoutSmuParityEcc : 1; ///< [0] SMN Parity
                                                     /// SMN Timeouts:PSP
                                                     /// SMU Parity and ECC
    UINT8                                       : 1; ///< [1] Reserved
    UINT8    PspParityEcc                       : 1; ///< [2] Psp Parity and ECC
    UINT8    SmnTimeoutSmu                      : 1; ///< [3] SMN Timeouts: SMU
    UINT8    SmnLinkPacketCrcWithRetry          : 1; ///< [4] SMN Link Packet CRC with Retry
    UINT8                                       : 4; ///< [7:3] Reserved
  } Field;
  UINT8    Value;
} SMN_ERROR_SOURCE;

#ifndef MAX_SOCKETS_SUPPORTED
#define MAX_SOCKETS_SUPPORTED    4  ///< Max number of sockets in system
#endif

#ifndef MAX_CHANNELS_PER_SOCKET
#define MAX_CHANNELS_PER_SOCKET  12  ///< Max Channels per sockets
#endif

#define TOTAL_SYSTEM_CHANNELS    (MAX_SOCKETS_SUPPORTED * MAX_CHANNELS_PER_SOCKET)

#pragma pack (push, 1)
typedef struct {
  IN       UINT8      TOTAL_NUM_RANKS_PER_UMCCH_ADDR_TRANS[TOTAL_SYSTEM_CHANNELS];
  IN       UINT8      RANK_ENABLE_PER_UMCCH_ADDR_TRANS[TOTAL_SYSTEM_CHANNELS];
  IN       UINT32     ADDRHASHBANK0[TOTAL_SYSTEM_CHANNELS];
  IN       UINT32     ADDRHASHBANK1[TOTAL_SYSTEM_CHANNELS];
  IN       UINT32     ADDRHASHBANK2[TOTAL_SYSTEM_CHANNELS];
  IN       UINT32     ADDRHASHBANK3[TOTAL_SYSTEM_CHANNELS];
  IN       UINT32     ADDRHASHBANK4[TOTAL_SYSTEM_CHANNELS];
  IN       UINT32     ADDRHASHBANKPC[TOTAL_SYSTEM_CHANNELS];
  IN       UINT32     ADDRHASHBANKPC2[TOTAL_SYSTEM_CHANNELS];
  IN       UINT32     ADDRHASHNORMADDR[TOTAL_SYSTEM_CHANNELS][2];
  IN       UINT32     CSBASE[TOTAL_SYSTEM_CHANNELS][4];
  IN       UINT32     CSBASESEC[TOTAL_SYSTEM_CHANNELS][4];
  IN       UINT32     CSMASK[TOTAL_SYSTEM_CHANNELS][2];
  IN       UINT32     CSMASKSEC[TOTAL_SYSTEM_CHANNELS][2];
  IN       UINT32     CONFIGDIMM[TOTAL_SYSTEM_CHANNELS][4]; // [25:24] is the secondary CSEnable. [29:26] can be rmselsec
  IN       UINT32     BANKSELDIMM[TOTAL_SYSTEM_CHANNELS][2];
  IN       UINT32     ROWSELDIMM[TOTAL_SYSTEM_CHANNELS][2];
  IN       UINT32     COL0SELDIMM[TOTAL_SYSTEM_CHANNELS][2];
  IN       UINT32     COL1SELDIMM[TOTAL_SYSTEM_CHANNELS][2];
  IN       UINT32     RMSELDIMM[TOTAL_SYSTEM_CHANNELS][2];
  IN       UINT32     RMSELDIMMSEC[TOTAL_SYSTEM_CHANNELS][2];
  IN       UINT32     CSSIZEDIMM[TOTAL_SYSTEM_CHANNELS][8];
  IN       UINT32     CHANSIZEDIMM[TOTAL_SYSTEM_CHANNELS];
  IN       UINT8      CSPOSHI[TOTAL_SYSTEM_CHANNELS][4];
  IN       UINT8      CSPOSLO[TOTAL_SYSTEM_CHANNELS][4];
  IN       UINT32     CTRLREG[TOTAL_SYSTEM_CHANNELS][4];
  IN       BOOLEAN    addrhash[6];
} ADDR_DATA;

///
/// NBIO Error Record structure
///
typedef struct {
  NBIO_ERR_VALID_BIT    ValidationBits;
  UINT8                 BusId;
  NBIO_ERR_SRC          ErrorSource;
  NBIO_ERR_TYPE         ErrorType;
  UINT8                 GroupType;
  UINT16                GroupId;
  UINT8                 NbifId;
  UINT8                 LeafId;
  UINT32                ParityErrSts;
} HYGON_NBIO_ERROR_RECORD;

///
/// SMU Error Record structure
///
typedef struct {
  SMN_ERROR_VALID_BIT    ValidationBits;
  UINT8                  BusId;
  SMN_CATEGORY_NUM       Category;
  SMN_ERROR_SOURCE       ErrorSource;
} HYGON_SMN_ERROR_RECORD;

#pragma pack (pop)

///
/// Platform RAS configuration data structure
///
typedef struct {
  BOOLEAN      PFEHEnable;
  UINT16       SwSmiCmdPortAddr;
  UINT8        MceSwSmiData;
  BOOLEAN      McaErrThreshEn;
  UINT16       McaErrThreshCount;
  UINT16       McaErrThreshSwCount;          // byo231101 +
  UINT16       McaNonMemErrThresh;
  BOOLEAN      NbioCorrectedErrThreshEn;
  UINT16       NbioCorrectedErrThreshCount;
  BOOLEAN      NbioDeferredErrThreshEn;
  UINT16       NbioDeferredErrThreshCount;
  UINTN        TotalNumberOfProcessors;
  CPU_INFO     *RasCpuMap;
  ADDR_DATA    *AddrData;
} HYGON_RAS_POLICY;

/// MCA Instance ID List
typedef struct {
  UINT32    InstanceID;             ///< MCA IPID Instance ID
  UINT8     InstanceIDIndex;        ///< The Index of block instance
} MCA_INSTANCE_ID_ENTRY;

#endif // _HYGON_RAS_H_
