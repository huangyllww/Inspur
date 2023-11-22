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

#ifndef _HYGON_CPM_RAS_H_
#define _HYGON_CPM_RAS_H_

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <IndustryStandard/Acpi.h>

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

#define HYGON_CREATOR_ID  SIGNATURE_32 ('H', 'G', 'N', ' ')

#define HYGON_HEST_TABLE_ID  SIGNATURE_64 ('H', 'G', 'N', ' ', 'H', 'E', 'S', 'T')
#define HYGON_BERT_TABLE_ID  SIGNATURE_64 ('H', 'G', 'N', ' ', 'B', 'E', 'R', 'T')
#define HYGON_EINJ_TABLE_ID  SIGNATURE_64 ('H', 'G', 'N', ' ', 'E', 'I', 'N', 'J')

#define ACPI_REV1             (0x01)
#define ACPI_REV2             (0x02)
#define ACPI_REV3             (0x03)
#define ACPI_REV4             (0x04)
#define MAX_ERROR_BLOCK_SIZE  (0x1000)

/// HEST
#define MCE_SOURCE_ID0                  (1)
#define CMC_SOURCE_ID0                  (2)
#define NBIO_SOURCE_ID                  (3)
#define PCIE_SOURCE_ID                  (4)
#define RECORDS_PREALLOCATE             (0x01)
#define SECTIONS_PER_RECORD             (0x01)
#define PCIE_ROOT_PORT_SRC_ID           (0x100)
#define PCIE_DEVICE_SOURCE_ID           (0x101)
#define PCIE_BRIDGE_SOURCE_ID           (0x102)
#define PCIE_RP_GHES_CORR_SRC_ID        (0x200)
#define PCIE_RP_GHES_UNCORR_SRC_ID      (0x201)
#define PCIE_DEV_GHES_CORR_SRC_ID       (0x202)
#define PCIE_DEV_GHES_UNCORR_SRC_ID     (0x203)
#define PCIE_BRIDGE_GHES_CORR_SRC_ID    (0x204)
#define PCIE_BRIDGE_GHES_UNCORR_SRC_ID  (0x205)

///
/// Hardware Error Notification types. All other values are reserved
///
#define HARDWARE_ERROR_NOTIFICATION_POLLED              0x00
#define HARDWARE_ERROR_NOTIFICATION_EXTERNAL_INTERRUPT  0x01
#define HARDWARE_ERROR_NOTIFICATION_LOCAL_INTERRUPT     0x02
#define HARDWARE_ERROR_NOTIFICATION_SCI                 0x03
#define HARDWARE_ERROR_NOTIFICATION_NMI                 0x04
#define HARDWARE_ERROR_NOTIFICATION_CMCI                0x05
#define HARDWARE_ERROR_NOTIFICATION_MCE                 0x06
#define HARDWARE_ERROR_NOTIFICATION_GPIO                0x07

/// Memory Error Types UEFI 2.6 Table 275
typedef enum _MEM_ERROR_TYPE_NUM {
  UNKNOWN = 0,                    ///< 0
  NO_ERROR,                       ///< 1
  SINGLE_BIT_ECC,                 ///< 2
  MULTI_BIT_ECC,                  ///< 3
  SINGLE_SYM_CHIP_KILL_ECC,       ///< 4
  MULTI_SYM_CHIP_KILL_ECC,        ///< 5
  MASTER_ABORT,                   ///< 6
  TARGET_ABORT,                   ///< 7
  PARITY_ERROR,                   ///< 8
  WATCHDOG_TIMEOUT,               ///< 9
  INVALID_ADDRESS,                ///< 10
  MIRROR_BROKEN,                  ///< 11
  MEMORY_SPARING,                 ///< 12
  SCRUB_CORR_ERROR,               ///< 13
  SCRUB_UNCORR_ERROR,             ///< 14
  PHYSICAL_MEM_MAPOUT             ///< 15
} MEM_ERROR_TYPE_NUM;

/// UEFI 2.6 Section N Table 275
/// Bit[15]-Rank Num Valid, Bit[14]-Memory Error Type Valid, Bit[9]-Column Valid, Bit[8]-Row Valid, Bit[7]-Device Valid, Bit[6]-Bank Valid, Bit[5]-Module Valid, Bit[3]-Node Valid, Bit[2]-Error Address Valid, Bit[0]-ErrSts Valid
#define MEM_VALID_BIT_MAP         ((1 << 15) | (1 << 9) | (1 << 8) | (1 << 7) | (1 << 6) | (1 << 5) | (1 << 3) | (1 << 1) | (1 << 0))
#define MEM_PARITY_VALID_BIT_MAP  ((1 << 14) | (1 << 3) | (1 << 0))
#define NBIO_VALID_BIT_MAP        ((1 << 7) | (1 << 4) | (1 << 3) | (1 << 2) | (1 << 1) | (1 << 0))
#define NBIF_VALID_BIT_MAP        ((1 << 6) | (1 << 5) | (1 << 2) | (1 << 1) | (1 << 0))
#define SMN_VALID_BIT_MAP         ((1 << 2) | (1 << 1) | (1 << 0))

/// UEFI 2.6 Section N Table 249 Error Section Descriptor GUID Definitions
/// Processor Generic Error Section GUID
#define PROCESSOR_GENERIC_SECT_GUID \
  { 0x9876CCAD, 0x47B4, 0x4BDB, 0xB6, 0x5E, 0x16, 0xF1, 0x93, 0xC4, 0xF3, 0xDB }
// 9876CCAD-47B4-4bdb-B65E16F193C4F3DB

// Processor Specific (IA32/X64) Error Section GUID
#define PROCESSOR_SPECIFIC_X86_SECT_GUID \
  { 0xDC3EA0B0, 0xA144, 0x4797, 0xB9, 0x5B, 0x53, 0xFA, 0x24, 0x2B, 0x6E, 0x1D }
// DC3EA0B0-A144-4797-B95B53FA242B6E1D

#define IA32_X64_CACHE_CHECK_STRUC_GUID \
  { 0xA55701F5, 0xE3EF, 0x43de, 0xAC, 0x72, 0x24, 0x9B, 0x57, 0x3F, 0xAD, 0x2C }
// A55701F5-E3EF-43DE-AC72249B573FAD2C

#define IA32_X64_TLB_CHECK_STRUC_GUID \
  { 0xFC06B535, 0x5E1F, 0x4562, 0x9F, 0x25, 0x0A, 0x3B, 0x9A, 0xDB, 0x63, 0xC3 }
// FC06B535-5E1F-4562-9F250A3B9ADB63C3

#define IA32_X64_BUS_CHECK_STRUC_GUID \
  { 0x1CF3F8B3, 0xC5B1, 0x49a2, 0xAA, 0x59, 0x5E, 0xEF, 0x92, 0xFF, 0xA6, 0x3C }
// 1CF3F8B3-C5B1-49a2-AA595EEF92FFA63C

#define IA32_X64_MS_CHECK_FIELD_DESC_GUID \
  { 0x48AB7F57, 0xDC34, 0x4F6C, 0xA7, 0xD3, 0xB0, 0xB5, 0xB0, 0xA7, 0x43, 0x14 }
// 48AB7F57-DC34-4F6C-A7D3B0B5B0A74314

/// Platform Memory Error Section GUID
#define PLATFORM_MEMORY_SECT_GUID \
  { 0xA5BC1114, 0x6F64, 0x4EDE, 0xB8, 0x63, 0x3E, 0x83, 0xED, 0x7C, 0x83, 0xB1 }
// A5BC1114-6F64-4EDE-B8633E83ED7C83B1

/// PCI Express Error Section GUID
#define PCIE_SECT_GUID \
  { 0xD995E954, 0xBBC1, 0x430F, 0xAD, 0x91, 0xB4, 0x4D, 0xCB, 0x3C, 0x6F, 0x35 }
// D995E954-BBC1-430F-AD91B44DCB3C6F35

/// Firmware Error Record Section GUID
#define FIRMWARE_ERROR_RECORD_SECT_GUID \
  { 0x81212A96, 0x09ED, 0x4996, 0x94, 0x71, 0x8D, 0x72, 0x9C, 0x8E, 0x69, 0xED }
// 81212A96-09ED-4996-94718D729C869ED

/// PCI/PCIX Error Section GUID
#define PCI_PCIX_SECT_GUID \
  { 0xC5753963, 0x3B84, 0x4095, 0xBF, 0x78, 0xED, 0xDA, 0xD3, 0xF9, 0xC9, 0xDD }
// C5753963-3B84-4095-BF78EDDAD3F9C9DD

/// PCI Component Device GUID
#define PCI_COMPONENT_DEVICE_SECT_GUID \
  { 0xEB5E4685, 0xCA66, 0x4769, 0xB6, 0xA2, 0x26, 0x06, 0x8B, 0x00, 0x13, 0x26 }
// EB5E4685-CA66-4769-B6A226068B001326

/// DMAr Generic Section GUID
#define DMAR_GENERIC_SECT_GUID \
  { 0x5B51FEF7, 0xC79D, 0x4434, 0x8F, 0x1B, 0xAA, 0x62, 0xDE, 0x3E, 0x2C, 0x64 }
// 5B51FEF7-C79D-4434-8F1BAA62DE3E2C64

/// IOMMU Specific DMAr Section GUID
#define IOMMU_SPECIFIC_DMAR_SECT_GUID \
  { 0x036F84E1, 0x7F37, 0x428c, 0xA7, 0x9E, 0x57, 0x5F, 0xDF, 0xAA, 0x84, 0xEC }
// 036F84E1-7F37-428C-A79E575FDFAA84EC

/// MCA Deferred Error Section GUID
#define MCA_DEFERRED_ERROR_SECT_GUID \
  { 0xAFDA558D, 0x51C9, 0x4256, 0xBB, 0xD0, 0x22, 0x4E, 0x07, 0x84, 0x46, 0xF1 }
// AFDA558D-51C9-4256-BBD0224E078446F1

/// NBIO Error Section GUID
#define NBIO_ERROR_SECT_GUID \
  { 0x911CAC2E, 0xA256, 0x4F84, 0xA3, 0xD3, 0xDD, 0xAE, 0x89, 0x9B, 0x27, 0x28 }
// 911CAC2E-A256-4F84-A3D3DDAE899B2728

/// SMU-SMN Error Section GUID
#define SMN_ERROR_SECT_GUID \
  { 0xA2860CC1, 0x8987, 0x4B7C, 0xB8, 0x6A, 0xD5, 0x08, 0xB1, 0x76, 0xBA, 0x70 }
// A2860CC1-8987-4B7C-B86A-D508B176BA70

/// EINJ Instruction definition
#define EINJ_SET_ERROR_TYPE_WITH_ADDRESS  (0x08)

/// Register bit width
#define REG_BIT_WIDTH_BYTE   (8)
#define REG_BIT_WIDTH_WORD   (16)
#define REG_BIT_WIDTH_DWORD  (32)
#define REG_BIT_WIDTH_QWORD  (64)

/// Register bit offset
#define REG_BIT_OFFSET0  (0)

/// FLAGS
#define EINJ_CLEAR_FLAG  (0)
#define EINJ_SET_FLAG    (1)

/// Reserved
#define EINJ_RESERVED  (0)

/// Mask
#define EINJ_BYTE_MASK          (0xFF)
#define EINJ_WORD_MASK          (0xFFFF)
#define EINJ_DWORD_MASK         (0xFFFFFFFFul)
#define EINJ_QWORD_MASK         (0xFFFFFFFFFFFFFFFFull)
#define EINJ_CHK_BUSY_STS_MASK  (0x01)
#define EINJ_GET_CMD_STS_MASK   (0xFFFE)

/// Injection Instructions
#define EINJ_READ_REGISTER         (0x00)     // Returns a register value
#define EINJ_READ_REGISTER_VALUE   (0x01)     // Reads register value and compares results to value in VALUE field
#define EINJ_WRITE_REGISTER        (0x02)     // Write register (ignores value in register field)
#define EINJ_WRITE_REGISTER_VALUE  (0x03)     // Write register using data in value field
#define EINJ_NOOP                  (0x04)     // No operation

/// Busy bits
#define APEI_RESET_BUSY_BIT  (0X00)
#define APEI_SET_BUSY_BIT    (0x01)

#define SMIPORT             (0xB2)
#define EINJ_BEGIN_INJ_CMD  (0x81)

#define EINJ_INSTR_ENTRIES       (9)
#define EINJ_TRIG_INSTR_ENTRIES  (1)

#define EINJ_ERROR_VECTOR_CORR    (0x0001)
#define EINJ_ERROR_VECTOR_UNCORR  (0x0801)

#define EINJ_DFLT_INJECT_ADDRESS  (0x100000000)
// #define EINJ_DFLT_INJECT_ADDRESS      (0xC41FE71C0)
// #define EINJ_DFLT_INJECT_ADDRESS      (0x2041E9F4C0)
#define DRAM_SCRUBBER_ERROR_ADDRESS_LOW_MASK  (0xFFFFFFFFFFFFFFC0)

#pragma pack (push, 1)

/// MSR179 Global Machine Check Capabilities data struct
typedef struct _MSR_MCG_CAP_STRUCT {
  UINT64    Count   : 8;          ///< Indicates the number of
                                  ///< error-reporting banks visible to each core
  UINT64    McgCtlP : 1;          ///< 1=The machine check control registers
  UINT64    Rsvd    : 55;         ///< reserved
} MSR_MCG_CAP_STRUCT;

/// DPC Control Register
/// BXXD01F[7:1]x386;
/// BXXD02F1x386
/// BXXD03F[7:1]x386
/// BXXD04F1x386
typedef union _PCIE_DPC_CNTL_REG {
  UINT16    Raw;                           ///< Raw data
  struct {
    ///< PCIE_DPC_CNTL_REG
    UINT16    DpcTriggerEnable       :  2; ///< bit 1:0  DPC Trigger Enable
    UINT16    DpcCompletionControl   :  1; ///< bit 2    DPC Completion Control
    UINT16    DpcInterruptEnable     :  1; ///< bit 3    DPC Interrupt Enable
    UINT16    DpcErrCorEnable        :  1; ///< bit 4    DPC ERR_COR Enable
    UINT16    PoisonedTLPEgressBlkEn :  1; ///< bit 5    Poisoned TLP Egress Blocking Enable
    UINT16    DpcSoftwareTrigger     :  1; ///< bit 6    DPC Software Trigger
    UINT16    DlActiveErrCorEnable   :  1; ///< bit 7    DL_Active ERR_COR Enable
    UINT16    Reserved               :  8; ///< bit 15:8 Reserved.
  } DpcRegField;
} PCIE_DPC_CNTL_REG;

/// BERT

/// UEFI 2.6 Appendix N Table 253. Cache Check Structure

#define CACHE_CHECK_TRANSTYPE            (BIT0)
#define CACHE_CHECK_OPERATION            (BIT1)
#define CACHE_CHECK_LEVEL                (BIT2)
#define CACHE_CHECK_PROC_CONEXT_CORRUPT  (BIT3)
#define CACHE_CHECK_UNCORRECTED          (BIT4)
#define CACHE_CHECK_PRECISE_IP           (BIT5)
#define CACHE_CHECK_RESTARTABLE          (BIT6)
#define CACHE_CHECK_OVERFLOW             (BIT7)

typedef union {
  struct {
    ///< CACHE_CHECK_STRUC
    UINT64    ValidationBits          : 16;    ///< [15:0] Validation Bits
    UINT64    TranscationType         : 2;     ///< [17:16] Transcation Type
    UINT64    Operation               : 4;     ///< [21:18] Operation
    UINT64    Level                   : 3;     ///< [24:22] Cache Level
    UINT64    ProcessorContextCorrupt : 1;     ///< [25] Processor Context Corrupt
    UINT64    Uncorrected             : 1;     ///< [26] Uncorrected
    UINT64    PreciseIp               : 1;     ///< [27] Precise IP
    UINT64    RestartableIp           : 1;     ///< [28] Restartable IP
    UINT64    Overflow                : 1;     ///< [29] Overflow
    UINT64                            : 34;    ///< [63:30] Reserved
  } Field;
  UINT64    Value;
} CACHE_CHECK_STRUC;

/// UEFI 2.6 Appendix N Table 254. TLB Check Structure

#define TLB_CHECK_TRANSTYPE            (BIT0)
#define TLB_CHECK_OPERATION            (BIT1)
#define TLB_CHECK_LEVEL                (BIT2)
#define TLB_CHECK_PROC_CONEXT_CORRUPT  (BIT3)
#define TLB_CHECK_UNCORRECTED          (BIT4)
#define TLB_CHECK_PRECISE_IP           (BIT5)
#define TLB_CHECK_RESTARTABLE          (BIT6)
#define TLB_CHECK_OVERFLOW             (BIT7)

typedef union {
  struct {
    ///< TLB_CHECK_STRUC
    UINT64    ValidationBits          : 16;    ///< [15:0] Validation Bits
    UINT64    TranscationType         : 2;     ///< [17:16] Transcation Type
    UINT64    Operation               : 4;     ///< [21:18] Operation
    UINT64    Level                   : 3;     ///< [24:22] Cache Level
    UINT64    ProcessorContextCorrupt : 1;     ///< [25] Processor Context Corrupt
    UINT64    Uncorrected             : 1;     ///< [26] Uncorrected
    UINT64    PreciseIp               : 1;     ///< [27] Precise IP
    UINT64    RestartableIp           : 1;     ///< [28] Restartable IP
    UINT64    Overflow                : 1;     ///< [29] Overflow
    UINT64                            : 34;    ///< [63:30] Reserved
  } Field;
  UINT64    Value;
} TLB_CHECK_STRUC;

/// UEFI 2.6 Appendix N Table 255. Bus Check Structure

#define BUS_CHECK_TRANSTYPE            (BIT0)
#define BUS_CHECK_OPERATION            (BIT1)
#define BUS_CHECK_LEVEL                (BIT2)
#define BUS_CHECK_PROC_CONEXT_CORRUPT  (BIT3)
#define BUS_CHECK_UNCORRECTED          (BIT4)
#define BUS_CHECK_PRECISE_IP           (BIT5)
#define BUS_CHECK_RESTARTABLE          (BIT6)
#define BUS_CHECK_OVERFLOW             (BIT7)
#define BUS_CHECK_PARTICIPTION         (BIT8)
#define BUS_CHECK_TIME_OUT             (BIT9)
#define BUS_CHECK_ADDRESS_SPACE        (BIT10)

typedef union {
  struct {
    ///< BUS_CHECK_STRUC
    UINT64    ValidationBits          : 16;    ///< [15:0] Validation Bits
    UINT64    TranscationType         : 2;     ///< [17:16] Transcation Type
    UINT64    Operation               : 4;     ///< [21:18] Operation
    UINT64    Level                   : 3;     ///< [24:22] Cache Level
    UINT64    ProcessorContextCorrupt : 1;     ///< [25] Processor Context Corrupt
    UINT64    Uncorrected             : 1;     ///< [26] Uncorrected
    UINT64    PreciseIp               : 1;     ///< [27] Precise IP
    UINT64    RestartableIp           : 1;     ///< [28] Restartable IP
    UINT64    Overflow                : 1;     ///< [29] Overflow
    UINT64    ParticitionType         : 2;     ///< [31:30] Particition Type
    UINT64    TimeOut                 : 1;     ///< [32] Time Out
    UINT64    AddressSpace            : 2;     ///< [43:33] Address Space
    UINT64                            : 29;    ///< [63:35] Reserved
  } Field;
  UINT64    Value;
} BUS_CHECK_STRUC;

/// UEFI 2.6 Appendix N Table 256. MS Check Field Description
#define MS_CHECK_ERRORTYPE            (BIT0)
#define MS_CHECK_PROC_CONEXT_CORRUPT  (BIT1)
#define MS_CHECK_UNCORRECTED          (BIT2)
#define MS_CHECK_PRECISE_IP           (BIT3)
#define MS_CHECK_RESTARTABLE          (BIT4)
#define MS_CHECK_OVERFLOW             (BIT5)

#define MSCHK_ERRTYPE_NO_ERROR                  (BIT0)
#define MSCHK_ERRTYPE_UNCLASSIFIED              (BIT1)
#define MSCHK_ERRTYPE_MICROCODE_ROM_PARITY_ERR  (BIT2)
#define MSCHK_ERRTYPE_EXTERNAL_ERROR            (BIT3)
#define MSCHK_ERRTYPE_FRC_ERROR                 (BIT4)
#define MSCHK_ERRTYPE_INTERNAL_UNCLASSIFIED     (BIT5)

typedef union {
  struct {
    ///< MS_CHECK_FIELD
    UINT64    ValidationBits          : 16;    ///< [15:0] Validation Bits
    UINT64    ErrorType               : 3;     ///< [18:16] Error Type
    UINT64    ProcessorContextCorrupt : 1;     ///< [19] Processor Context Corrupt
    UINT64    Uncorrected             : 1;     ///< [20] Uncorrected
    UINT64    PreciseIp               : 1;     ///< [21] PreciseIp
    UINT64    RestartableIp           : 1;     ///< [22] RestartableIp
    UINT64    Overflow                : 1;     ///< [23] Overflow
    UINT64                            : 40;    ///< [63:24] Reserved
  } Field;
  UINT64    Value;
} MS_CHECK_FIELD;

/// UEFI 2.6 Appendix N Table 252. IA32/X64 Processor Error Information Structure
typedef union {
  struct {
    ///< PROC_ERR_INFO_VALID_BIT
    UINT64    CheckInfoValid            : 1;   ///< [0] Check Info Valid
    UINT64    TargetAddrIdentifierValid : 1;   ///< [1] Target Address Identifier Valid
    UINT64    RequestorIdentifierValid  : 1;   ///< [2] Requestor Identifier Valid
    UINT64    ResponderIdentifierValid  : 1;   ///< [3] Responder Identifier Valid
    UINT64    InstructionPointerValid   : 1;   ///< [4] Instruction Pointer Valid
    UINT64                              : 59;  ///< [63:5] Reserved
  } Field;
  UINT64    Value;
} PROC_ERR_INFO_VALID_BIT;

typedef struct _PROC_ERR_INFO_STRUC {
  UINT8                      ErrorStructureType[16]; ///< Error Structure Type
  PROC_ERR_INFO_VALID_BIT    ValidBits;              ///< Validation Bits
  UINT64                     CheckInformation;       ///< Check Information
  UINT64                     TargetIdentifier;       ///< Target Identifier
  UINT64                     RequestorIdentifier;    ///< Requestor Identifier
  UINT64                     ResponderIdentifier;    ///< Responder Identifier
  UINT64                     InstructionPointer;     ///< Instruction Pointer
} PROC_ERR_INFO_STRUC;

/// UEFI 2.6 Appendix N Table 259.
typedef struct _X64_REGISTER_STATE {
  UINT64    RAX;
  UINT64    RBX;
  UINT64    RCX;
  UINT64    RDX;
  UINT64    RSI;
  UINT64    RDI;
  UINT64    RBP;
  UINT64    RSP;
  UINT64    R8;
  UINT64    R9;
  UINT64    R10;
  UINT64    R11;
  UINT64    R12;
  UINT64    R13;
  UINT64    R14;
  UINT64    R15;
  UINT16    CS;
  UINT16    DS;
  UINT16    SS;
  UINT16    ES;
  UINT16    FS;
  UINT16    GS;
  UINT32    Reserved;
  UINT64    RFLAGS;
  UINT64    EIP;
  UINT64    CR0;
  UINT64    CR1;
  UINT64    CR2;
  UINT64    CR3;
  UINT64    CR4;
  UINT64    CR8;
  UINT64    GDTR_LO;
  UINT64    GDTR_HI;
  UINT64    IDTR_LO;
  UINT64    IDTR_HI;
  UINT16    LDTR;
  UINT16    TR;
} X64_REGISTER_STATE;

/// UEFI 2.6 Appendix N Table 257. IA32/X64 Processor Context Information
#define UNCLASSIFIED_DATA             (BIT0)
#define MSR_REGISTERS                 (BIT1)
#define MODE_EXECUTION_CONTEXT_32BIT  (BIT2)
#define MODE_EXECUTION_CONTEXT_64BIT  (BIT3)
#define FXSAVE_CONTEXT                (BIT4)
#define DEBUG_REGISTERS_32BIT_MODE    (BIT5)
#define DEBUG_REGISTERS_64BIT_MODE    (BIT6)
#define MEMORY_MAPPED_REGISTERS       (BIT7)

typedef struct _PROC_CONEXT_INFO_STRUC {
  UINT16    RegisterContextType;              ///< Register Context Type
  UINT16    RegisterArraySize;                ///< Register Array Size
  UINT32    MSRAddress;                       ///< MSR Address
  UINT64    MMRegisterAddress;                ///< Memory Map Register Address
  UINT64    RegisterArray[];                  ///< Register Array
} PROC_CONEXT_INFO_STRUC;

/// UEFI 2.6 Appendix N Table 251. IA32/X64 Processor Error Section

/// IA32/X64 Processor Error Section Structure
typedef union {
  struct {
    ///< PROC_ERR_INFO_VALID_BIT
    UINT64    LocalApicIDValid        : 1;      ///< [0] Check Info Valid
    UINT64    CpuIdInfoValid          : 1;      ///< [1] Target Address Identifier Valid
    UINT64    ProcErrInfoStrucNum     : 6;      ///< [7:2] Number of Processor Error Information Structure
    UINT64    ProcContextInfoStrucNum : 6;      ///< [13:8] Number of Processor Context Information Structure
    UINT64                            : 50;     ///< [63:14] Reserved
  } Field;
  UINT64    Value;
} PROC_ERR_VALID_BIT;

typedef struct _PLATFORM_PROC_ERR_SEC {
  PROC_ERR_VALID_BIT    ValidBits;              ///< Validation Bits
  UINT64                LocalApicID;            ///< Processor APIC ID
  UINT64                CpuIdInfo_EAX;          ///< CPUID Information output value from EAX
  UINT64                CpuIdInfo_EBX;          ///< CPUID Information output value from EBX
  UINT64                CpuIdInfo_ECX;          ///< CPUID Information output value from ECX
  UINT64                CpuIdInfo_EDX;          ///< CPUID Information output value from EDX
} PLATFORM_PROC_ERR_SEC;

typedef union {
  struct {
    ///< MCA_ERROR_CODE_TYPES
    UINT16    CacheLevel      : 2;             ///< [1:0] Cache Level
    UINT16    TransactionType : 2;             ///< [3:2] Transaction Type
    UINT16    TlbErr          : 12;            ///< [15:4] TLB error check bit
  } Field;
  UINT16    Value;
} MCA_TLB_ERROR;

typedef union {
  struct {
    ///< MCA_ERROR_CODE_TYPES
    UINT16    CacheLevel         : 2;          ///< [1:0] Cache Level
    UINT16    TransactionType    : 2;          ///< [3:2] Transaction Type
    UINT16    MemTransactionType : 4;          ///< [7:4] Memory Transaction Type
    UINT16    MemErr             : 8;          ///< [8] Memmory type error check bit
  } Field;
  UINT16    Value;
} MCA_MEM_ERROR;

typedef union {
  struct {
    ///< MCA_ERROR_CODE_TYPES
    UINT16    CacheLevel             : 2;      ///< [1:0] Cache Level
    UINT16    TransactionType        : 2;      ///< [3:2] Transaction Type
    UINT16    MemTransactionType     : 4;      ///< [7:4] Memory Transaction Type
    UINT16    Timeout                : 1;      ///< [8] Timeout
    UINT16    ParticipationProcessor : 2;      ///< [10:9] Participation Processor
    UINT16    BusErr                 : 5;      ///< [15:11] Bus Error Type check bit
  } Field;
  UINT16    Value;
} MCA_PP_ERROR;

typedef union {
  struct {
    ///< MCA_ERROR_CODE_TYPES
    UINT16    Zero1             : 8;           ///< [7:0] zero bits
    UINT16    InternalErrorType : 2;           ///< [9:8] Internal Error Type
    UINT16    InternalErr       : 6;           ///< [15:10] Internal Error check bit
  } Field;
  UINT16    Value;
} MCA_INT_ERROR;

#define TLB_ERROR_MASK       (0xFFF0)
#define TLB_ERROR_CHK_SHIFT  (4)
#define MEM_ERROR_MASK       (0xFF00)
#define MEM_ERROR_CHK_SHIFT  (8)
#define PP_ERROR_MASK        (0xF800)
#define PP_ERROR_CHK_SHIFT   (11)
#define INT_ERROR_MASK       (0xFC00)
#define INT_ERROR_CHK_SHIFT  (10)

/// UEFI 2.6 Appendix N Table 275 Spec platform memory error

/// Memory Error Section Valid bit fields
typedef union {
  struct {
    ///< MEM_ERR_VALID_BIT
    UINT64    ErrStatusValid        : 1;       ///< [0] Error Status Valid
    UINT64    PhysicalAddrValid     : 1;       ///< [1] Physical Address Valid
    UINT64    PhysicalAddrMaskValid : 1;       ///< [2] Physical Address Mask Valid
    UINT64    NodeValid             : 1;       ///< [3] Node Valid
    UINT64    CardValid             : 1;       ///< [4] Card Valid
    UINT64    ModuleValid           : 1;       ///< [5] Module Valid
    UINT64    BankValid             : 1;       ///< [6] Bank Valid
    UINT64    DeviceValid           : 1;       ///< [7] Device Valid
    UINT64    RowValid              : 1;       ///< [8] Row Valid
    UINT64    ColumnValid           : 1;       ///< [9] Column Valid
    UINT64    BitPositionValid      : 1;       ///< [10] Bit Position Valid
    UINT64    PltfrmReqIdValid      : 1;       ///< [11] Platform Requestor Id Valid
    UINT64    PltfrmRespIdValid     : 1;       ///< [12] Platform Responder Id Valid
    UINT64    MemPltfrmTargetValid  : 1;       ///< [13] Memory Platform Target Valid
    UINT64    MemoryErrTypeValid    : 1;       ///< [14] Memory Error Type Valid
    UINT64    RankNumberValid       : 1;       ///< [15] Rank Number Valid
    UINT64    CardHandleValid       : 1;       ///< [16] Card Handle Valid
    UINT64    ModuleHandleValid     : 1;       ///< [17] Module Handle Valid
    UINT64    ExtendRow1617Valid    : 1;       ///< [18] Extend Row bit 16 and 17 Valid
    UINT64    BankGroupValid        : 1;       ///< [19] Bank Group Valid
    UINT64    BankAddressValid      : 1;       ///< [20] Bank Address Valid
    UINT64    ChipIdValid           : 1;       ///< [21] Chip Identification Valid
    UINT64                          : 42;      ///< [63:22] Reserved
  } Field;
  UINT64    Value;
} MEM_ERR_VALID_BIT;

typedef struct _PLATFORM_MEM_ERR_SEC {
  MEM_ERR_VALID_BIT    ValidBits;   ///< Valid bits Bitmp
  UINT64               ErrStatus;   ///< Error Status
  UINT64               PhyAddr;     ///< Physical memory address of detected error
  UINT64               PhyAddrMask; ///< Physical Error Address mask
  UINT16               Node;        ///< Node Number
  UINT16               Card;        ///< Card Number
  UINT16               Module;      ///< Module Number
  UINT16               Bank;        ///< Bank Number
  UINT16               Device;      ///< Device Number
  UINT16               Row;         ///< Row Number
  UINT16               Column;      ///< Column Number
  UINT16               BitPosition; ///< Bit Position
  UINT64               RequestorID; ///< Requestor ID
  UINT64               ResponderID; ///< Responder ID
  UINT64               TargetID;    ///< Target ID
  UINT8                MemErrType;  ///< Memory Error Type
} PLATFORM_MEM_ERR_SEC;

///
/// PCIE Error Section DEVICE_ID
///
typedef struct {
  UINT16    VendorId;                                                ///< Vendor ID
  UINT16    DeviceId;                                                ///< Device ID
  UINT8     ClassCode[3];                                            ///< Class Code
  UINT8     Function;                                                ///< Function
  UINT8     Device;                                                  ///< Device
  UINT16    Segment;                                                 ///< Segment
  UINT8     PrimaryBus;                                              ///< Primary Bus
  UINT8     SecondaryBus;                                            ///< Secondary Bus
  UINT16    Reserved1 : 3;                                           ///< Reserved1
  UINT16    Slot      : 13;                                          ///< Slot
  UINT8     Reserved;                                                ///< Reserved
} DEVICE_ID;

///
/// PCIE Error Section CAP_STRUCTURE
///
typedef struct {
  UINT32    CapabilityData[15];                                      ///< CapabilityData
} CAP_STRUCTURE;

///
/// PCIE Error Section AER_INFO
///
typedef struct {
  UINT32    AerInfoData[24];                                         ///< AerInfoData
} AER_INFO;

/// DRAM Limit Address Register
typedef union {
  struct {
    ///< PCIE_ERR_VALID_BIT
    UINT64    PortTypeValid        : 1;        ///< [0] Port Type Valid
    UINT64    VersionValid         : 1;        ///< [1] Version Valid
    UINT64    CommandStatusValid   : 1;        ///< [2] Command Status Valid
    UINT64    DeviceIdValid        : 1;        ///< [3] Device ID Valid
    UINT64    DeviceSerialNumValid : 1;        ///< [4] Device Serial Number Valid
    UINT64    BridgeCntlStsValid   : 1;        ///< [5] Bridge Control Status Valid
    UINT64    CapStructStsValid    : 1;        ///< [6] Capability Structure Status Valid
    UINT64    AERInfoValid         : 1;        ///< [7] AER Info Valid
    UINT64                         : 42;       ///< [63:8] Reserved
  } Field;
  UINT64    Value;
} PCIE_ERR_VALID_BIT;

///
/// PCIE Error Section
///
typedef struct {
  PCIE_ERR_VALID_BIT    Validation;                                  ///< Validation Bits
  UINT32                PortType;                                    ///< Port Type
  UINT32                Revision;                                    ///< Revision
  UINT32                CommandStatus;                               ///< Command Status
  UINT32                Reserved;                                    ///< Reserved
  DEVICE_ID             DeviceId;                                    ///< Device Id
  UINT8                 SerialNum[8];                                ///< Serial Num
  UINT32                BridgeCtrlStatus;                            ///< Bridge Control Status
  CAP_STRUCTURE         CapabilityStructure;                         ///< Capability Structure
  AER_INFO              AerInfo;                                     ///< AER Info
} PCIE_ERROR_SECTION;

/// BERT Generic Processor Error Entry
typedef struct _GENERIC_PROC_ERR_ENTRY {
  EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE    GenErrorDataEntry;     ///< Generic Error Data Entry
  PLATFORM_PROC_ERR_SEC                              ProcErrorSection;      ///< Platform Processor Error Section
} GENERIC_PROC_ERR_ENTRY;

/// BERT Generic Memory Error Entry
typedef struct _GENERIC_MEM_ERR_ENTRY {
  EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE    GenErrorDataEntry;     ///< Generic Error Data Entry
  PLATFORM_MEM_ERR_SEC                               MemErrorSection;       ///< Platform Memory Error Section
} GENERIC_MEM_ERR_ENTRY;

/// Generic Nbio Error Entry
typedef struct _GENERIC_NBIO_ERR_ENTRY {
  EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE    GenErrorDataEntry;     ///< Generic Error Data Entry
  HYGON_NBIO_ERROR_RECORD                            NbioErrorSection;      ///< Platform NBIO Error Section
} GENERIC_NBIO_ERR_ENTRY;

/// Generic Smn Error Entry
typedef struct _GENERIC_SMN_ERR_ENTRY {
  EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE    GenErrorDataEntry;     ///< Generic Error Data Entry
  HYGON_SMN_ERROR_RECORD                             SmnErrorSection;       ///< Platform NBIO Error Section
} GENERIC_SMN_ERR_ENTRY;

/// Generic PcieAer Error Entry
typedef struct _GENERIC_PCIE_AER_ERR_ENTRY {
  EFI_ACPI_6_0_GENERIC_ERROR_DATA_ENTRY_STRUCTURE    GenErrorDataEntry;     ///< Generic Error Data Entry
  PCIE_ERROR_SECTION                                 PcieAerErrorSection;   ///< Platform NBIO Error Section
} GENERIC_PCIE_AER_ERR_ENTRY;

/*
 * SET_ERROR_TYPE_WITH_ADDRESS action command structure.
 */

typedef struct _SET_ERR_TYPE_WITH_ADDR {
  UINT32    ErrorType;
  UINT32    VendorExtension;
  UINT32    Flags;
  UINT32    Apicid;
  UINT64    MemoryAddress;
  UINT64    MemoryAddressRange;
  UINT32    PcieSbdf;
} SET_ERR_TYPE_WITH_ADDR;

typedef enum _SET_WITH_ADDR_NUM {
  SETWA_FLAGS_APICID    = 1,
  SETWA_FLAGS_MEM       = 2,
  SETWA_FLAGS_PCIE_SBDF = 4,
} SET_WITH_ADDR_NUM;

/*
 * Vendor extensions for platform specific operations
 */
typedef struct _VENDOR_ERR_TYPE_EXTEN {
  UINT32    Length;
  UINT32    PcieSbdf;
  UINT16    VendorId;
  UINT16    DeviceId;
  UINT8     RevId;
  UINT8     Reserved[3];
} VENDOR_ERR_TYPE_EXTEN;

/// EINJ Correctable Memory Trigger Action Table
typedef struct _TRIG_ERROR_ACTION {
  EFI_ACPI_6_0_EINJ_TRIGGER_ACTION_TABLE           TrigErrActionHeader;                           ///< Trigger Error Action Header
  EFI_ACPI_6_0_EINJ_INJECTION_INSTRUCTION_ENTRY    TrigErrorInstruction[EINJ_TRIG_INSTR_ENTRIES]; ///< Trigger Error Instruction Array
} TRIG_ERROR_ACTION;

/// APEI EINJ Registers
typedef struct _APEI_EINJ_REGISTERS {
  UINT64    EinjBeginInjReg;            ///< EINJ Begin Injection Register
  UINT64    EinjGetTriActTblReg;        ///< EINJ Get Trigger Action Table Register
  UINT64    EinjSetErrTypeReg;          ///< EINJ Set Error Type Register
  UINT64    EinjGetErrTypeReg;          ///< EINJ Get Error Type Register
  UINT64    EinjEndOperReg;             ///< EINJ End Operation Register
  UINT64    EinjChkBusyStsReg;          ///< EINJ Check Busy Status Register
  UINT64    EinjCmdStsReg;              ///< EINJ Command Status Register
  UINT64    EinjSetErrTypeWithAddrReg;  ///< EINJ Set Error Type with Address Register
} APEI_EINJ_REGISTERS;

/// EINJ ACPI Table
typedef struct _APEI_EINJ_ACPI_TABLE {
  EFI_ACPI_6_0_ERROR_INJECTION_TABLE_HEADER        InjectionHeader;                         ///< APEI EINJ Table Header
  EFI_ACPI_6_0_EINJ_INJECTION_INSTRUCTION_ENTRY    InjectionInstEntry[EINJ_INSTR_ENTRIES];  ///< APEI EINJ Injection Intrustion Instance Entry
} APEI_EINJ_ACPI_TABLE;

///
/// Platform RAS configuration data structure
///
typedef struct {
  BOOLEAN    PFEHEnable;                           ///< PFEHEnable;
  BOOLEAN    FchApuRasSmiSupport;                  ///< FchApuRasSmiSupport;
  BOOLEAN    FchSataRasSupport;                    ///< FchSataRasSupport;
  UINT16     SwSmiCmdPortAddr;                     ///< SwSmiCmdPortAddr;
  UINT8      MceSwSmiData;                         ///< MceSwSmiData;
  UINT8      EinjSwSmiData;                        ///< EinjSwSmiData;
  BOOLEAN    McaErrThreshEn;                       ///< McaErrThreshEn;
  UINT16     McaErrThreshCount;                    ///< McaErrThreshCount;
  UINT32     McaPollInterval;                      ///< McaPollInterval;
  UINT32     NbioPollInterval;                     ///< NbioPollInterval;
  UINT32     PciePollInterval;                     ///< PciePollInterval;
  BOOLEAN    NbioCorrectedErrThreshEn;             ///< NbioCorrectedErrThreshEn;
  UINT16     NbioCorrectedErrThreshCount;          ///< NbioCorrectedErrThreshCount;
  BOOLEAN    NbioDeferredErrThreshEn;              ///< NbioDeferredErrThreshEn;
  UINT16     NbioDeferredErrThreshCount;           ///< NbioDeferredErrThreshCount;
  UINT32     RasSmiThreshold;                      ///< RasSmiThreshold
  UINT32     RasSmiScale;                          ///< RasSmiScale
  UINT8      RasThresholdPeriodicSmiEn;            ///< RasThresholdPeriodicSmiCtrl
  UINT32     RasRetryCnt;                          ///< RasRetryCnt;
  UINT8      EinjTrigErrSwSmiId;                   ///< EinjTrigErrSwSmiId;
  BOOLEAN    GhesNotifyType;                       ///< GhesNotifyType
  BOOLEAN    PcieGhesNotifyType;                   ///< PcieGhesNotifyType
  UINT32     PcieRpCorrectedErrorMask;             ///< PcieRpCorrectedErrorMask
  UINT32     PcieRpUnCorrectedErrorMask;           ///< PcieRpUnCorrectedErrorMask
  UINT32     PcieRpUnCorrectedErrorSeverity;       ///< PcieRpUnCorrectedErrorSeverity
  UINT32     PcieDevCorrectedErrorMask;            ///< PcieDevCorrectedErrorMask
  UINT32     PcieDevUnCorrectedErrorMask;          ///< PcieDevUnCorrectedErrorMask
  UINT32     PcieDevUnCorrectedErrorSeverity;      ///< PcieDevUnCorrectedErrorSeverity
// byo231031 + >>  
  UINT32     PcieAerCeMask;
  UINT32     PcieAerUceMask;
  BOOLEAN    ApeiDisable;
  BOOLEAN    ApeiEinjDisable;
  BOOLEAN    ApeiEinjCpuCeDisable;
// byo231031 + <<  
  UINT16     McaErrThreshSwCount;            // byo231101 +
  UINT16     McaNonMemErrThresh;     // byo231109 +
  UINT16     PcieCeThreshold;        // byo231109 +
} PLATFORM_RAS_POLICY;

/// Pcie Port Profile
typedef struct {
  UINT8    NbioSocketNum;               ///< NbioSocketNum
  UINT8    NbioLogicalDieNum;           ///< NbioLogicalDieNum
  UINT8    NbioIohubPhysicalDieNum;     ///< NbioIohubPhysicalDie
  UINT8    NbioRbNum;                   ///< NbioRbNum
  UINT8    NbioBusNum;                  ///< NbioBusNum
  UINT8    PciPortNumber;               ///< Logical port number
  UINT8    PciPhysicalPortNumber;       ///< Physical port number from REMAP
  UINT16   CeCount;                  // byo231109 +
} PCIE_PORT_PROFILE;

/// Pcie Active Port Map
typedef struct {
  UINT16               PortCount;       ///< PortCount
  PCIE_PORT_PROFILE    PciPortNumber[1]; ///< PciPortNumber
} PCIE_ACTIVE_PORT_MAP;

/// Pcie Port List
typedef struct {
  UINT8    Device;                      ///< Device
  UINT8    Function;                    ///< Function
} PCIE_PORT_LIST;

/// Nbio Parity Error Table
typedef struct {
  UINT32    GroupType;           ///< Correctable/Uncorrectable
  UINT32    GroupId;             ///< Group ID
  UINT32    StructId;            ///< Structure ID
  UINT32    ErrGenCmd;           ///< Error command
} NBIO_PARITY_ERROR_TABLE;

#define MAX_PCIEMAP_BLOCK_SIZE  (0x400)

///
/// Platform APEI Private data structure
///
typedef struct _PLATFORM_APEI_PRIVATE_BUFFER {
  BOOLEAN                                     Valid;                          ///< Indicate if data valid
  BOOLEAN                                     PlatformHestValid;              ///< Indicate if Platform Hest supported
  BOOLEAN                                     PlatformBertValid;              ///< Indicate if Platform Bert supported
  BOOLEAN                                     PlatformEinjValid;              ///< Indicate if Platform Einj supported
  UINT64                                      PcieBaseAddress;                ///< Pcie Base Address
  APEI_EINJ_REGISTERS                         *EinjActMemRegAddr;             ///< EINJ Action Instruction Register buffer pointer
  UINT8                                       *TrigErrorAct;                  ///< EINJ Action Table pointer
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE    *HygonNbioErrBlk;               ///< Nbio Generic Error Status Block
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE    *HygonPcieAerErrBlk;            ///< PCI-E AER Generic Error Status Block
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE    *HygonPcieAerUnErrBlk;          ///< PCI-E Root Port AER Generic Uncorrectable Error Status Block
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE    *HygonPcieDevAerErrBlk;         ///< PCI-E Devices AER Generic Correctable Error Status Block
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE    *HygonPcieDevAerUnErrBlk;       ///< PCI-E Devices AER Generic Uncorrectable Error Status Block
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE    *HygonPcieBridgeAerErrBlk;      ///< PCI-E Bridge AER Generic Correctable Error Status Block
  EFI_ACPI_6_0_BOOT_ERROR_REGION_STRUCTURE    *HygonPcieBridgeAerUnErrBlk;    ///< PCI-E Bridge AER Generic Uncorrectable Error Status Block
  PCIE_ACTIVE_PORT_MAP                        *HygonPciePortMap;              ///< Active PCI-E port map
  PLATFORM_RAS_POLICY                         PlatRasPolicy;                  ///< RAS Platform configuration
} PLATFORM_APEI_PRIVATE_BUFFER;

#pragma pack (pop)

#endif //_HYGON_CPM_RAS_H_
