/* $NoKeywords:$ */

/**
 * @file
 *
 * PSP Base Library
 *
 * Contains interface to the PSP library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  PSP
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
 **/

#ifndef _HYGON_PSP_BASELIB_H_
#define _HYGON_PSP_BASELIB_H_

#include <HYGON.h>
#include <HygonPspDirectory.h>

#define PSP_MAILBOX_BASE           0x70               ///< Mailbox base offset on PCIe BAR
#define PSP_MAILBOX_STATUS_OFFSET  0x4                ///< Staus Offset
#define MP0_C2PMSG_37_SMN_OFFSET   (0x10500 + 37 * 4) ///< C2PMSG_37
#define MP0_C2PMSG_38_SMN_OFFSET   (0x10500 + 38 * 4) ///< C2PMSG_38
#pragma pack (push, 1)

///
/// X86 to PSP Buffer which start mapping from C2PMSG_28
///
typedef volatile struct {
  UINT32    Status    : 16;                 ///< Set by the target to indicate the execution status of last command
  UINT32    CommandId : 8;                  ///< Command ID set by host
  UINT32    Reserved  : 5;                  ///< Reserved
  UINT32    Recovery  : 2;                  ///< Set by the target to indicate that the host has to execute FW recovery sequence
  UINT32    Ready     : 1;                  ///< Set by the target to indicate the mailbox interface state.
} PSP_MBOX_CMD_EXT;

typedef volatile union {
  IN  UINT32              Value;                  ///< Cmd register value
  IN  PSP_MBOX_CMD_EXT    Field;                  ///< Extended Cmd register with field definition
} PSP_MBOX_CMD;

typedef volatile struct {
  PSP_MBOX_CMD    Cmd;
  UINT64          Buffer;                  ///< 64 bit Ponter to memory with additional parameter.
} PSP_MBOX;

///
/// X86 to PSP Buffer which start mapping from C2PMSG_32
///
typedef volatile struct {
  UINT32    Status    : 16;                 ///< Set by the target to indicate the execution status of last command
  UINT32    CommandId : 10;                 ///< Command ID set by host
  UINT32    Reserved  : 5;                  ///< Reserved
  UINT32    Response  : 1;                  ///< Set by the target to indicate the mailbox interface state.
} PSP_MBOX_CH2_CMD_EXT;

typedef volatile union {
  IN  UINT32                  Value;              ///< Cmd register value
  IN  PSP_MBOX_CH2_CMD_EXT    Field;              ///< Extended Cmd register with field definition
} PSP_MBOX_CH2_CMD;

#define FIRMWARE_TABLE_SIGNATURE  0x55AA55AAul
/// Define the structure OEM signature table
typedef struct _FIRMWARE_ENTRY_TABLEV2 {
  UINT32    Signature;        ///< Signature should be 0x55AA55AAul
  UINT32    ImcRomBase;       ///< Base Address for Imc Firmware
  UINT32    GecRomBase;       ///< Base Address for Gmc Firmware
  UINT32    XHCRomBase;       ///< Base Address for XHCI Firmware
  UINT32    LegacyPspDirBase; ///< Base Address of PSP directory for legacy program (ML, BP, CZ, BR, ST)
  UINT32    PspDirBase;       ///< Base Address for PSP directory
  UINT32    SatBiosDirBase;   ///< Base Address for SAT BIOS directory
  UINT32    RvBiosDirBase;    ///< Base Address for RV BIOS directory
} FIRMWARE_ENTRY_TABLEV2;

/// Unified Boot BIOS Directory structure
enum _BIOS_DIRECTORY_ENTRY_TYPE {
  BIOS_PUBLIC_KEY       = 0x05,               ///< PSP entry points to BIOS public key stored in SPI space
  BIOS_RTM_FIRMWARE     = 0x06,               ///< PSP entry points to BIOS RTM code (PEI volume) in SPI space
  BIOS_RTM_SIGNATURE    = 0x07,               ///< PSP entry points to signed BIOS RTM hash stored  in SPI space
  BIOS_HPCB_INFO        = 0x60,               ///< Hgpi PSP Customization Block (HPCB)
  BIOS_HPOB_INFO        = 0x61,               ///< Hgpi PSP Output Block (HPOB) target location
  BIOS_FIRMWARE         = 0x62,               ///< BIOS Firmware volumes
  HPOB_NV_COPY          = 0x63,               ///< HPOB data copy on non-volatile storage which will used by ABL during S3 resume
  PMU_INSTRUCTION       = 0x64,               ///< Location field pointing to the instruction portion of PMU firmware
  PMU_DATA              = 0x65,               ///< Location field pointing to the data portion of PMU firmware
  UCODE_PATCH           = 0x66,               ///< Microcode patch
  CORE_MCEDATA          = 0x67,               ///< Core MCE data
  BIOS_HPCB_INFO_BACKUP = 0x68,               ///< Backup Hgpi PSP Customization Block (HPCB)
  BIOS_DIR_LV2          = 0x70,               ///< BIOS entry points to Level 2 BIOS DIR
};

/// Type attribute for BIOS Directory entry
typedef struct {
  UINT32    Type           : 8; ///< Type of BIOS entry
  UINT32    RegionType     : 8; ///< 0 Normal memory, 1 TA1 memory, 2 TA2 memor
  UINT32    BiosResetImage : 1; ///< Set for SEC or EL3 fw, which will be authenticate by PSP FW known as HVB
  UINT32    Copy           : 1; ///< Copy: 1- copy BIOS image image from source to destination 0- Set region attribute based on <ReadOnly, Source, size> attributes
  UINT32    ReadOnly       : 1; ///< 1: Set region to read-only (applicable for ARM- TA1/TA2) 0: Set region to read/write
  UINT32    Compressed     : 1; ///< 1: Compresed
  UINT32    Instance       : 4; ///< Specify the Instance of an entry
  UINT32    Reserved       : 8; ///< Reserve for future use
} TYPE_ATTRIB;

/// Structure for PSP Entry
typedef struct {
  TYPE_ATTRIB    TypeAttrib;                    ///< Type of PSP entry; 32 bit long
  UINT32         Size;                          ///< Size of PSP Entry in bytes
  UINT64         Location;                      ///< Location of PSP Entry (byte offset from start of SPI-ROM)
  UINT64         Destination;                   ///< Destination of PSP Entry copy to
} BIOS_DIRECTORY_ENTRY;

#define BIOS_DIRECTORY_HEADER_SIGNATURE      0x44484224ul ///< $BHD BIOS Directory Signature
#define BIOS_LV2_DIRECTORY_HEADER_SIGNATURE  0x324C4224ul ///< $BL2 BIOS Directory Lv2 Signature
/// Structure for BIOS directory
typedef struct {
  PSP_DIRECTORY_HEADER    Header;         ///< PSP directory header
  BIOS_DIRECTORY_ENTRY    BiosEntry[1];   ///< Array of PSP entries each pointing to a binary in SPI flash
                                          ///< The actual size of this array comes from the
                                          ///< header (PSP_DIRECTORY.Header.TotalEntries)
} BIOS_DIRECTORY;

/// Structure for PSP Combo directory
#define PSP_COMBO_DIRECTORY_HEADER_SIGNATURE  0x50535032ul  ///< 2PSP PSP Combo Directory Signature
typedef struct {
  UINT32    PspCookie;    ///< "2PSP"
  UINT32    Checksum;     ///< 32 bit CRC of header items below and the entire table
  UINT32    TotalEntries; ///< Number of PSP Entries
  UINT32    LookUpMode;   ///< 0 - Dynamic look up through all entries, 1 - PSP/chip ID match.
  UINT8     Reserved[16]; ///< Reserved
} PSP_COMBO_DIRECTORY_HEADER;

/// Structure for PSP Combo directory entry
typedef struct {
  UINT32    IdSelect;     ///< 0 - Compare PSP ID, 1 - Compare chip family ID
  UINT32    Id;           ///< 32-bit Chip/PSP ID
  UINT64    PspDirTableAddr; ///< Point to PSP directory table (level 2)
} PSP_COMBO_DIRECTORY_ENTRY;

#define SAT_PSP_CHIP_ID  0xBC090000  ///< SAT Chip ID in combo structure
#define RV_PSP_CHIP_ID   0xBC0A0000  ///< RV Chip ID in combo structure
typedef struct {
  PSP_COMBO_DIRECTORY_HEADER    Header;        ///< PSP Combo directory header
  PSP_COMBO_DIRECTORY_ENTRY     ComboEntry[1]; ///< Array of PSP combo entries each pointing to level 2 PSP Direcotry header
} PSP_COMBO_DIRECTORY;

#pragma pack (pop)

BOOLEAN
GetDirBase (
  IN OUT   UINT32     *PspDirBase,
  IN OUT   UINT32     *BiosDirBase
  );

BOOLEAN
PSPEntryInfoV2 (
  IN      PSP_DIRECTORY_ENTRY_TYPE    EntryType,
  IN OUT  UINT64                      *EntryAddress,
  IN      UINT32                      *EntrySize
  );

#define INSTANCE_IGNORED  0xFF
BOOLEAN
BIOSEntryInfo (
  IN       UINT8                       EntryType,
  IN       UINT8                       EntryInstance,
  IN OUT   TYPE_ATTRIB                 *TypeAttrib,
  IN OUT   UINT64                      *EntryAddress,
  IN OUT   UINT32                      *EntrySize,
  IN OUT   UINT64                      *EntryDest
  );

BOOLEAN
GetPspEntryScsBinaryV2 (
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  );

BOOLEAN
GetPspEntryPspNvDataV2 (
  IN OUT   UINT64                      *EntryAddress,
  IN       UINT32                      *EntrySize
  );

BOOLEAN
  CheckPspDevicePresentV2 (
                           VOID
                           );

BOOLEAN
  CheckPspPlatformSecureEnable (
                                VOID
                                );

BOOLEAN
  CheckPspRecoveryFlag (
                        VOID
                        );

BOOLEAN
GetPspMmioBase (
  IN OUT   UINT32 *PspMmioBase
  );

BOOLEAN
GetPspMboxLocation (
  IN OUT   PSP_MBOX **PspMbox
  );

VOID
  PspBarInitEarly (
                   VOID
                   );

typedef
BOOLEAN
(*FP_CONDITIONER) (
  IN       VOID        *Context
  );

BOOLEAN
PspLibTimeOutV2 (
  IN       UINT64              uSec,
  IN       FP_CONDITIONER      Conditioner,
  IN       VOID                *Context
  );

BOOLEAN
GetFtpmControlAreaV2 (
  IN OUT   VOID **FtpmControlArea
  );

VOID
SwitchPspMmioDecodeV2 (
  IN       BOOLEAN SwitchFlag,
  IN OUT   UINT32 *RegisterCopy
  );

VOID
  AcquirePspSmiRegMutexV2 (
                           VOID
                           );

VOID
  ReleasePspSmiRegMutexV2 (
                           VOID
                           );

HGPI_STATUS
ReadC2PMsgReg (
  IN       UINTN      Socket,
  IN       UINTN      Die,
  IN       UINT32     C2pMsgOffset,
  IN OUT   UINT32     *RegValue
  );

#endif // _HYGON_LIB_H_
