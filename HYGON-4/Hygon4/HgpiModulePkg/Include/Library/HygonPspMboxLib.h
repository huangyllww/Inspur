/* $NoKeywords:$ */

/**
 * @file
 *
 * PSP Mailbox related functions Prototype definition
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
 */
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
 *
 ***************************************************************************/

#ifndef _PSP_MBOX_H_
#define _PSP_MBOX_H_
#pragma pack (push, 1)

typedef union _MBOX_BUFFER MBOX_BUFFER;

#define PSP_NOTIFY_SMM  0xdd          ///< @todo Move it out to other file

#define PSP_WAITSTS_MAX_COUNT  10000 ///< @todo Need check this value's correctness

#define UCODE_ALLOCATION_SIZE  (4 * 1024)
// ======================================================================================
//
// Define Mailbox Command
//
// ======================================================================================
//
/// Mbox command list. Only one command can be send till target process it; the only exception is Abort command that BIOS may send in case of timeout etc.
///
typedef enum {
  MboxCmdRsvd = 0x00,                 ///< Unused

  MboxBiosCmdDramInfo       = 0x01,   ///< Bios -> PSP: Memory DRAM information (ie. PspBuffer address etc)
  MboxBiosCmdSmmInfo        = 0x02,   ///< Bios -> PSP: Bios will provide SMM inf - SmmBase, PspSmmDataRegion,PspSmmDataRegionLength, SoftSmiValue, SoftSmiPort
  MboxBiosCmdSxInfo         = 0x03,   ///< Bios -> PSP: Sx transition info (S3, S5)
  MboxBiosCmdRsmInfo        = 0x04,   ///< Bios -> PSP: Resume transition info (BiosResume Vector, Size of resume code)
  MboxBiosCmdPspQuery       = 0x05,   ///< Bios -> PSP: Bios Find supported feature
  MboxBiosCmdBootDone       = 0x06,   ///< Bios -> PSP: Bios is done with BIOS POST
  MboxBiosCmdClearS3Sts     = 0x07,   ///< Bios -> PSP: Inform PSP clear S3ExitReset
  MboxBiosS3DataInfo        = 0x08,   ///< Bios -> PSP: Bios will send this command to inform PSP to save the data needed to restore memory during resume from S3
  MboxBiosCmdNop            = 0x09,   ///< Bios -> PSP: Bios will send this NOP command to indicate to PSP that is is done servicing PSP SMM request
  MboxBiosCmdGetUcodePatch  = 0x0A,   ///< Bios -> PSP: BIOS assign 4KB memory buffer to PSP, PSP decode ucode patch to here and return status to BIOS
  MboxBiosCmdCcpVqOperation = 0x0B,   ///< Bios -> PSP: Bios will send this command to inform PSP to change X86 ccp VQ count <--add new available ccp vq count item 2019/12/05
  MboxBiosCmdHsioRequest    = 0x0F,   ///< Bios -> PSP: Hsio Request Msg
  MboxBiosCmdHSTIQuery      = 0x14,   ///< Bios -> PSP: Command to get HSTI bit field representing the security state of the SoC from the PSP
  MboxBiosCmdGetVersion     = 0x19,   ///< Bios -> PSP: Bios will send this command to get PSP FW image version
  MboxBiosCmdLockDFReg      = 0x1B,   ///< Bios -> PSP: Bios will send this command to lock DF registers
  MboxBiosCmdClrSmmLock     = 0x1C,   ///< Bios -> PSP: Command to clear SMMLock Register in C6 private memory region
  MboxBiosCmdSetApCsBase    = 0x1D,   ///< Bios -> PSP: Bios will send the CS BASE value for AP threads
  MboxBiosCmdUnLockSpi      = 0x1F,   ///< BIOS -> PSP: BIOS will send this command to lock SPI, X86 need in SMM mode when send this command
  MboxBiosCmdTa             = 0x40,   ///< Bios -> PSP: Send command to TA

  MboxPspCmdSpiGetAttrib    = 0x81,   ///< PSP -> BIOS: Get location of PSP NVRam region
  MboxPspCmdSpiSetAttrib    = 0x82,   ///< PSP -> BIOS: Get location of PSP NVRam region
  MboxPspCmdSpiGetBlockSize = 0x83,   ///< PSP -> BIOS: Get Block size info
  MboxPspCmdSpiReadFV       = 0x84,   ///< PSP -> BIOS: Read PSP NVRAM firmware volume
  MboxPspCmdSpiWriteFV      = 0x85,   ///< PSP -> BIOS: Write PSP NVRAM firmware volume
  MboxPspCmdSpiEraseFV      = 0x86,   ///< PSP -> BIOS: Erase PSP NVRAM firmware volume

  MboxCmdAbort = 0xfe,                ///< Abort the last command (BIOS to PSP in case of timeout etc)
} MBOX_COMMAND;

#define PSP_MBOX_SUCCESS                0x00          ///< Mail box success
#define PSP_MBOX_INVALID_PARAMETER      0x01          ///< Invalid Parameter
#define PSP_MBOX_CRC_ERROR              0x02          ///< CRC error
#define PSP_MBOX_COMMAND_PROCESS_ERROR  0x04          ///< Command process error
#define PSP_MBOX_UNSUPPORTED            0x08          ///< Unsupported
///
/// standard header structure for additional parameter
///
typedef struct {
  UINT32    TotalSize;                      ///< Total Size of MBOX_BUFFER (including this field)
  UINT32    Status;                         ///< Status value if any:e
  // UINT8   ReqBuffer[x];                   ///< X byte long Request buffer for additional parameter.
} MBOX_BUFFER_HEADER;

// ======================================================================================
//
// Below define Request buffer for various commands. This structure is based on Command
//
// ======================================================================================
#define SMM_TRIGGER_IO   0
#define SMM_TRIGGER_MEM  1

#define SMM_TRIGGER_BYTE   0
#define SMM_TRIGGER_WORD   1
#define SMM_TRIGGER_DWORD  2

/// Define structure of SMM_TRIGGER_INFO
typedef struct {
  UINT64    Address;                        ///< Memory or IO address (Memory will be qword, IO will be word)
  UINT32    AddressType;                    ///< SMM trigger typr - Perform write to IO/Memory
  UINT32    ValueWidth;                     ///< Width of value to write (byte write, word write,..)
  UINT32    ValueAndMask;                   ///< AND mask of value after reading from the address
  UINT32    ValueOrMask;                    ///< OR Mask of value to write to this address.
} SMM_TRIGGER_INFO;
/// SMM register information
typedef struct {
  UINT64    Address;                        ///< Memory or IO address (Memory will be qword, IO will be word)
  UINT32    AddressType;                    ///< SMM trigger typr - Perform write to IO/Memory
  UINT32    ValueWidth;                     ///< Width of value to write (byte write, word write,..)
  UINT32    RegBitMask;                     ///< AND mask of value after reading from the address
  UINT32    ExpectValue;                    ///< OR Mask of value to write to this address.
} SMM_REGISTER;
/// All SMM register report via SMMINFO command
typedef struct {
  SMM_REGISTER    SmiEnb;                   ///< Register information for SmiEnb
  SMM_REGISTER    Eos;                      ///< Register information for EOS
  SMM_REGISTER    FakeSmiEn;                ///< Register information for FakeSmiEn
  SMM_REGISTER    Reserved[5];              ///< Reserved
} SMM_REGISTER_INFO;
///
/// structure of ReqBuffer for MboxBiosCmdSmmInfo mailbox command
///
typedef struct {
  UINT64               SMMBase;                 ///< SMM TSeg Base
  UINT64               SMMMask;                 ///< Mask of SMM SMM TSeg
  UINT64               PSPSmmDataRegion;        ///< PSP region base in Smm space
  UINT64               PspSmmDataLength;        ///< Psp region length in smm space
  SMM_TRIGGER_INFO     SmmTrigInfo;             ///< Information to generate SMM
  SMM_REGISTER_INFO    SmmRegInfo;              ///< Information describe the SMM register information
  UINT64               PspMboxSmmBufferAddress; ///< Address of MBOX_BUFFER structure used for BIOS-to-PSP commands sent from SMM mode
  UINT64               PspMboxSmmFlagAddress;   ///< Address of 32-bit flag indicating the CPU is in SMM mode
} SMM_REQ_BUFFER;

/// MBOX buffer for SMM info
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  SMM_REQ_BUFFER        Req;                ///< Reques buffer
} MBOX_SMM_BUFFER;

///
/// structure of ReqBuffer for MboxBiosCmdSxInfo mailbox command
///
typedef struct {
  UINT8    SleepType;                       ///< Inform which sleep state the system is going to
} SX_REQ_BUFFER;

/// MBOX buffer for Sx info
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  SX_REQ_BUFFER         Req;                ///< Request buffer
} MBOX_SX_BUFFER;

///
/// structure of ReqBuffer for MboxBiosCmdRsmInfo mailbox command
///
typedef struct {
  UINT64    ResumeVecorAddress;             ///< Address of BIOS resume vector
  UINT64    ResumeVecorLength;              ///< Length of BIOS resume vector
} RSM_REQ_BUFFER;

/// MBOX buffer for RSM info
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  RSM_REQ_BUFFER        Req;                ///< Req
} MBOX_RSM_BUFFER;

/// CAPS_REQ_BUFFER structure
typedef struct {
  UINT32    Capabilities;                   ///< PSP Writes capabilities into this field when it returns.
} CAPS_REQ_BUFFER;

// Bitmap defining capabilities
#ifndef PSP_CAP_TPM_SUPPORTED
  #define PSP_CAP_TPM_SUPPORTED  (1 << 0)     ///< TPM supported
#endif

#ifndef PSP_CAP_TPM_REQ_FACTORY_RESET
  #define PSP_CAP_TPM_REQ_FACTORY_RESET  (1 << 1)  ///< TPM requires a factory reset
#endif

/// MBOX buffer for Capabilities Query
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  CAPS_REQ_BUFFER       Req;                ///< Req
} MBOX_CAPS_BUFFER;

// Bitmap defining HSTI bitfield
#define HSTISTATE_PSP_SECURE_EN             BIT0
#define HSTISTATE_PSP_PLATFORM_SECURE_EN    BIT1
#define HSTISTATE_PSP_DEBUG_LOCK_ON         BIT2
#define HSTISTATE_PSP_CUSTOMER_KEY_LOCK_ON  BIT3

/// MBOX buffer for HSTI
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  UINT32                HSTIState;          ///< HSTIState
} MBOX_HSTI_STATE;

/// MBOX buffer for Exit BIOS info
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
} MBOX_DEFAULT_BUFFER;

/// CS Base Buffer
typedef struct {
  UINT32    Value;
} AP_CS_BASE_BUFFER;

/// MBOX buffer for ApCsBase
typedef struct {
  MBOX_BUFFER_HEADER    Header;                   ///< Header
  AP_CS_BASE_BUFFER     Req;                      ///< Req
} MBOX_AP_CS_BASE_BUFFER;

/// Ucode Patch Base Buffer
typedef struct {
  UINT64    Address;
  UINT32    Size;
} UCODE_PATCH_BUFFER;

/// MBOX buffer for ApCsBase
typedef struct {
  MBOX_BUFFER_HEADER    Header;                   ///< Header
  UCODE_PATCH_BUFFER    UcodePatchBuffer;         ///< Ucode Patch Buffer
} MBOX_UCODE_PATCH_BUFFER;

/// Version Buffer
typedef struct {
  UINT32    PspFwVersion;                // PSP FW Version
  UINT32    HgpiFwVersion;               // HGPI FW Version
  UINT32    HPPBVersion;                 // HGPI HPPB FW Version
  UINT32    HPCBVersion;                 // HGPI HPCB FW Version
  UINT32    HPOBVersion;                 // HGPI HPOB FW Version
  UINT32    SmuVersion;                  // SMU FW Version
} VERSION_BUFFER;

/// MBOX buffer for GetVersion
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  VERSION_BUFFER        VersionBuffer;      ///< Buffer composed of multiple FW version
} MBOX_GET_VERSION_BUFFER;

typedef UINT32 TEE_STATUS;                  ///< Status of TEE command
#define TEE_OK                               0x000        ///< TEE command success
#define TEE_ERR_SOC_ADDRESS_MAPPING          0x10C        ///< TEE command error of SOC address mapping
#define TEE_ERR_NWD_INVALID_SESSION          0x10E        ///< TEE command error of invalid session
#define TEE_ERR_NWD_TOO_MANY_MAPPED_BUFFERS  0x10F        ///< TEE command error of too many mapped buffers
#define TEE_ERR_NWD_INVALID_WSM_LEN          0x110        ///< TEE command error of invalid length

/// MBOX buffer for MboxBiosCmdTa
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  UINT64                TaCmdBufferAddress; ///< Address of TA command buffer
  UINT64                TaCmdBufferSize;    ///< Size of TA command
} MBOX_TA;

//
// Define Malbox buffer comming from PSP->BIOS
//

///
/// structure of ReqBuffer for MboxPspCmdSpiGetAddress/MboxPspCmdSpiGetAddress  mailbox command
///
typedef struct {
  UINT64    Attribute;                      ///< Inform attribute of SPI part
} SPI_ATTRIB_REQ;

/// MBOX buffer for Spi Get/Set attribute info
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  SPI_ATTRIB_REQ        Req;                ///< Req
} MBOX_SPI_ATTRIB_BUFFER;

///
/// structure of ReqBuffer for MboxPspCmdSpiGetBlockSize mailbox command
///
typedef struct {
  UINT64    TargetNvId;                     ///< Target NV ID
  UINT64    Lba;                            ///< starting LBA
  UINT64    BlockSize;                      ///< Block size of each Lba
  UINT64    NumberOfBlocks;                 ///< Total number of blocks
} SPI_INFO_REQ;

/// MBOX buffer for Spi read block attribute
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  SPI_INFO_REQ          Req;                ///< Req
} MBOX_SPI_INFO_BUFFER;

///
/// structure of ReqBuffer for MboxPspCmdSpiRead/Write mailbox command
///
typedef struct {
  UINT64    TargetNvId;                     ///< Target NV ID
  UINT64    Lba;                            ///< starting LBA
  UINT64    Offset;                         ///< Offset in LBA
  UINT64    NumByte;                        ///< Total byte to read
  UINT8     Buffer[1];                      ///< Buffer to read the data
} SPI_RW_REQ;

/// MBOX buffer for Spi read block attribute
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  SPI_RW_REQ            Req;                ///< Req
} MBOX_SPI_RW_BUFFER;

///
/// structure of ReqBuffer for MboxPspCmdSpiErase mailbox command
///
typedef struct {
  UINT64    TargetNvId;                     ///< Target NV ID
  UINT64    Lba;                            ///< starting LBA
  UINT64    NumberOfBlocks;                 ///< Total number of blocks
} SPI_ERASE_REQ;

/// MBOX buffer for Spi read block attribute
typedef struct {
  MBOX_BUFFER_HEADER    Header;             ///< Header
  SPI_ERASE_REQ         Req;                ///< Req
} MBOX_SPI_ERASE_BUFFER;

/// Union of various structure
typedef union _MBOX_BUFFER {
  MBOX_DEFAULT_BUFFER        Dflt;          ///< Default

  MBOX_SMM_BUFFER            Smm;           ///< Smm
  MBOX_SX_BUFFER             Sx;            ///< Sx
  MBOX_RSM_BUFFER            Rsm;           ///< Rsm
  MBOX_CAPS_BUFFER           Cap;           ///< Cap
  MBOX_GET_VERSION_BUFFER    Ver;           ///< Version
  MBOX_AP_CS_BASE_BUFFER     CsBase;        ///< CS base
  MBOX_SPI_ATTRIB_BUFFER     SpiAttrib;     ///< SpiAttrib
  MBOX_SPI_INFO_BUFFER       SpiInfo;       ///< SpiInfo
  MBOX_SPI_RW_BUFFER         SpiRw;         ///< SpiRw
  MBOX_SPI_ERASE_BUFFER      SpiErase;      ///< SpiErase
  UINT8                      Rsvd[32];      ///< To Keep the minimal size 32 bytes
} MBOX_BUFFER;

/// 2x Sized Mailbox Buffer for alignment
typedef struct {
  MBOX_BUFFER    Chunk[2];                  ///< Add additional one for alignment
} UNALIGNED_MBOX_BUFFER;

///
/// Buffer is 2X size of aligned structure of size at least 32 bytes long
/// To align in go to next offset past the buffer size & from there back
/// to 32 byte aligned address
#define BALIGN32(p)  ((VOID *)(((UINTN)(VOID *)(p) + 32) & ~0x1F))

///
/// PSP to X86 Buffer exist in SMMRAM
///
typedef struct {
  UINT32    CheckSumValue : 8;  ///< 8bits Checksum Value of MBOX_BUFFER
  UINT32    CheckSumEn    : 1;  ///< Switch for Enable/Disable Checksum
  UINT32    Reserved      : 22; ///< Reserved
  UINT32    CommandReady  : 1; ///< Flag used specify P2C SMI command is ready to handle a new command
} SMI_MBOX_STATUS;

/// PSP to BIOS SMI mailbox structure
typedef struct {
  volatile MBOX_COMMAND       MboxCmd;      ///< Mbox Command 32 bit wide
  volatile SMI_MBOX_STATUS    MboxSts;      ///< Mbox status  32 bit wide
  MBOX_BUFFER                 Buffer;       ///< Mailbox buffer right after
} BIOS_MBOX;
// add new available ccp vq count item 2019/12/05 --->
typedef struct  {
  UINT32    ExportVqNr; ///< Number of exported VQ
} EXPORT_PSPCCPVQ_BUFFER;

typedef struct {
  MBOX_BUFFER_HEADER        Header;               ///< Header
  EXPORT_PSPCCPVQ_BUFFER    ExportPspccpvqBuffer; ///< Export PSPCCP VQ Buffer
} MBOX_EXPORTPSPCCPVQ_BUFFER;

#define VQ_OPERATION_RET_SUCCESS 0x0
#define VQ_OPERATION_RET_WAITING 0xF
#define VQ_OPERATION_MSG_RETRY_TIME 0xF

#define C2P_ARG_APPERTURE_ID_HYEX          0x38
#define C2P_ARG_APPERTURE_ID_HYGX          0x1A

#define C2P_CMD_STATUS_REG_HYEX            ((C2P_ARG_APPERTURE_ID_HYEX << 20)+0x10570)
#define C2P_ARG0_REG_HYEX                  ((C2P_ARG_APPERTURE_ID_HYEX << 20)+0x10574)
#define C2P_ARG1_REG_HYEX                  ((C2P_ARG_APPERTURE_ID_HYEX << 20)+0x10578)
#define C2P_ARG2_REG_HYEX                  ((C2P_ARG_APPERTURE_ID_HYEX << 20)+0x1057C)
#define C2P_ARG3_REG_HYEX                  ((C2P_ARG_APPERTURE_ID_HYEX << 20)+0x1056C)
#define C2P_ARG4_REG_HYEX                  ((C2P_ARG_APPERTURE_ID_HYEX << 20)+0x10584)
#define C2P_ARG5_REG_HYEX                  ((C2P_ARG_APPERTURE_ID_HYEX << 20)+0x10588)

#define C2P_CMD_STATUS_REG_HYGX            ((C2P_ARG_APPERTURE_ID_HYGX << 20)+0x10570)
#define C2P_ARG0_REG_HYGX                  ((C2P_ARG_APPERTURE_ID_HYGX << 20)+0x10574)
#define C2P_ARG1_REG_HYGX                  ((C2P_ARG_APPERTURE_ID_HYGX << 20)+0x10578)
#define C2P_ARG2_REG_HYGX                  ((C2P_ARG_APPERTURE_ID_HYGX << 20)+0x1057C)
#define C2P_ARG3_REG_HYGX                  ((C2P_ARG_APPERTURE_ID_HYGX << 20)+0x1056C)
#define C2P_ARG4_REG_HYGX                  ((C2P_ARG_APPERTURE_ID_HYGX << 20)+0x10584)
#define C2P_ARG5_REG_HYGX                  ((C2P_ARG_APPERTURE_ID_HYGX << 20)+0x10588)
#define PSP_SERVICE_RESULT_OK         1
#define PSP_SERVICE_RESULT_FATAL      ((UINT8)0xFC)  
typedef volatile struct {
  UINT32    Status    : 16;                 ///< Set by the target to indicate the execution status of last command
  UINT32    CommandId : 8;                  ///< CmdID
  UINT32    Reserved  : 5;                  ///< Reserved
  UINT32    Recovery  : 2;                  ///< Set by the target to indicate that the host has to execute FW recovery sequence
  UINT32    Ready     : 1;                  ///< Set by the target to indicate the mailbox interface state.
} PSP_MBOX_CMD_STATUS_EXT;

typedef volatile union {
  IN  UINT32                     Value;               ///< Cmd register value
  IN  PSP_MBOX_CMD_STATUS_EXT    Field;               ///< Extended Cmd register with field definition
} PSP_MBOX_CMD_STATUS;

#pragma pack (pop)
// ======================================================================================
//
// Define Mailbox function prototype
//
// ======================================================================================
//
//

EFI_STATUS
CheckFtpmCapsV2 (
  IN OUT   UINT32 *Caps
  );

EFI_STATUS
PspMboxBiosCmdS3Info (
  IN UINT64 S3RestoreBufferBase,
  IN UINT64 S3RestoreBufferSize,
  IN OUT   UINT8 *Hmac
  );

EFI_STATUS
PspMboxBiosCmdSmmInfo (
  IN SMM_REQ_BUFFER      *SmmInfoReq
  );

EFI_STATUS
PspMboxBiosCmdSxInfo (
  IN UINT8     SleepType,
  IN UINT8     *SmmBuffer,
  IN BOOLEAN   *SmmFlag
  );

EFI_STATUS
PspMboxBiosCmdResumeInfo (
  IN UINT64 S3ResumeAddress,
  IN UINT64 S3ResumeCodeSize
  );

EFI_STATUS
PspMboxBiosCmdExitBootServices (
  );

EFI_STATUS
PspMboxBiosQueryHSTIState (
  IN OUT   UINT32 *HSTIState
  );

EFI_STATUS
PspMboxBiosSendApCsBase (
  IN       UINT32 ApCsBase
  );

EFI_STATUS
PspMboxBiosGetFwVersions (
  IN OUT   VERSION_BUFFER *FwVersions
  );

EFI_STATUS
PspMboxBiosClearSmmLock (
  IN UINT8     *SmmBuffer,
  IN BOOLEAN   *SmmFlag
  );

EFI_STATUS
PspMboxBiosLockDFReg (
  );

EFI_STATUS
PspMboxBiosUnLockSpi (
  );

EFI_STATUS
PspMboxBiosGetUcodePatch (
  IN UINT64    *UcodePatchBuffer,
  IN UINT32    BufferSize
  );

EFI_STATUS
PspMboxOperationVQ (
  IN OUT UINT8 *VqCount
  );

BOOLEAN
SendPspCommandCh2 (
  IN       VOID       *MboxBuffer,
  IN       UINT32   Cmd
  );

EFI_STATUS
fTpmExecuteCommand (
  IN       VOID                 *CommandBuffer,
  IN       UINT32               CommandSize,
  IN OUT   VOID                 *ResponseBuffer,
  IN OUT   UINT32               *ResponseSize
  );

UINT32
NbioPspServiceRequest (
  IN       UINT32                   IohcBus,
  IN       UINT32                   PhysicalDieId,
  IN       UINT32                   RequestId,
  IN OUT   UINT32                   *RequestArgument,
  IN       UINT32                   AccessFlags
  );

#endif //_PSP_MBOX_H_
