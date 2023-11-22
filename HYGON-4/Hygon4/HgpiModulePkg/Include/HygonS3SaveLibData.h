/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Library
 *
 * Contains interface to the HYGON S3 save library
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
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

#ifndef _HYGON_S3SAVE_LIB_DATA_H_
#define _HYGON_S3SAVE_LIB_DATA_H_

/// Define the Private Data structure used by HygonS3SaveDxeLib
typedef struct _HYGON_S3LIB_PRIVATE_DATA {
  BOOLEAN    S3ScriptLock;    ///< S3Script Lock or Not for Non SMM driver
  BOOLEAN    CloseTable;      ///< TRUE HygonS3SaveLib is collecting the S3 script data, FALSE stop collect the data
  UINT8      *TableBase;      ///< Table used to collect the S3 Script DATA after SmmLock
} HYGON_S3_LIB_PRIVATE_DATA;

#define HYGON_S3LIB_BOOT_SCRIPT_TABLE_SIG    SIGNATURE_32 ('A', 'S', '3', 'T')
#define HYGON_S3LIB_BOOT_SCRIPT_TABLE_VER_1  0x1ul
#define HYGON_S3LIB_BUFFER_INIT_SIZE         (1024 * 16)
#define HYGON_S3LIB_BUFFER_INC_SIZE          (1024 * 1)
#define HYGON_S3LIB_TABLE_END                (0xFFFF)
#define HYGON_S3LIB_TABLE_END_SIZE           sizeof (UINT16)
#define HYGON_S3LIB_TABLE_WATCHER            (0x01558844ul)
#define HYGON_S3LIB_TABLE_WATCHER_SIZE       sizeof (UINT32)

/// S3 Boot Script table Header
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_TABLE_HEADER {
  UINT32    Sinature;       ///< Sinature
  UINT32    Version;        ///< Version
  UINT8     Reserved[24];   ///< Reserved
  UINTN     TableLength;    ///< Allocated memory length
  UINTN     NextDataOffset; ///< Offset to next data
  UINT8     Data;           ///< Data start
} HYGON_S3_LIB_BOOT_SCRIPT_TABLE_HEADER;

/// S3 Lib script operator enumeration
typedef enum {
  HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE_OP = 0x0,    ///< IO write operator
  HYGON_S3LIB_BOOT_SCRIPT_IO_RMW_OP,            ///< IO Read Write operator
  HYGON_S3LIB_BOOT_SCRIPT_IO_POLL_OP,           ///< IO poll operator
  HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE_OP,         ///< Mem write operator
  HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW_OP,           ///< Mem Read Write operator
  HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL_OP,          ///< Mem poll operator
  HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE_OP,         ///< PCI write operator
  HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW_OP,           ///< PCI Read Write operator
  HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL_OP,          ///< PCI poll operator
  HYGON_S3LIB_BOOT_SCRIPT_DISPATCH_OP,          ///< Dispatch operator
}  HYGON_S3LIB_BOOT_SCRIPT_OP;

/// Hygon S3 Lib Boot script common structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_COMMON {
  UINT16    OpCode;                           ///< operator
} HYGON_S3LIB_BOOT_SCRIPT_COMMON;

/// Hygon S3 Lib Boot script IO write structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE {
  UINT16                      OpCode;         ///< operator
  S3_BOOT_SCRIPT_LIB_WIDTH    Width;          ///< Width
  UINT64                      Address;        ///< Address
  UINT64                      Value;          ///< Value
} HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE;

/// Hygon S3 Lib Boot script IO read write structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_IO_RMW {
  UINT16                      OpCode;         ///< operator
  S3_BOOT_SCRIPT_LIB_WIDTH    Width;          ///< Width
  UINT64                      Address;        ///< Address
  UINT64                      Data;           ///< Data
  UINT64                      DataMask;       ///< DataMask
} HYGON_S3LIB_BOOT_SCRIPT_IO_RMW;

/// Hygon S3 Lib Boot script IO poll structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_IO_POLL {
  UINT16                      OpCode;         ///< operator
  S3_BOOT_SCRIPT_LIB_WIDTH    Width;          ///< Width
  UINT64                      Address;        ///< Address
  UINT64                      Data;           ///< Data
  UINT64                      DataMask;       ///< DataMask
  UINT64                      Delay;          ///< Delay
} HYGON_S3LIB_BOOT_SCRIPT_IO_POLL;

/// Hygon S3 Lib Boot script Memory write structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE {
  UINT16                      OpCode;         ///< operator
  S3_BOOT_SCRIPT_LIB_WIDTH    Width;          ///< Width
  UINT64                      Address;        ///< Address
  UINT64                      Value;          ///< Value
} HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE;

/// Hygon S3 Lib Boot script Memory read write structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW {
  UINT16                      OpCode;         ///< operator
  S3_BOOT_SCRIPT_LIB_WIDTH    Width;          ///< Width
  UINT64                      Address;        ///< Address
  UINT64                      Data;           ///< Data
  UINT64                      DataMask;       ///< DataMask
} HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW;

/// Hygon S3 Lib Boot script Memory poll structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL {
  UINT16                      OpCode;         ///< operator
  S3_BOOT_SCRIPT_LIB_WIDTH    Width;          ///< Width
  UINT64                      Address;        ///< Address
  UINT64                      Data;           ///< Data
  UINT64                      DataMask;       ///< DataMask
  UINTN                       LoopTimes;      ///< LoopTimes
} HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL;

/// Hygon S3 Lib Boot script PCI write structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE {
  UINT16                      OpCode;         ///< operator
  S3_BOOT_SCRIPT_LIB_WIDTH    Width;          ///< Width
  UINT16                      Segment;        ///< PCI segment
  UINT64                      Address;        ///< Address
  UINT64                      Value;          ///< Value
} HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE;

/// Hygon S3 Lib Boot script PCI read write structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW {
  UINT16                      OpCode;         ///< operator
  S3_BOOT_SCRIPT_LIB_WIDTH    Width;          ///< Width
  UINT16                      Segment;        ///< PCI segment
  UINT64                      Address;        ///< Address
  UINT64                      Data;           ///< Data
  UINT64                      DataMask;       ///< DataMask
} HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW;

/// Hygon S3 Lib Boot script PCI POLL structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL {
  UINT16                      OpCode;         ///< operator
  S3_BOOT_SCRIPT_LIB_WIDTH    Width;          ///< Width
  UINT16                      Segment;        ///< PCI segment
  UINT64                      Address;        ///< Address
  UINT64                      Data;           ///< Data
  UINT64                      DataMask;       ///< DataMask
  UINT64                      Delay;          ///< Delay
} HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL;

/// Hygon S3 Lib Boot script Dispatch structure
typedef struct  _HYGON_S3LIB_BOOT_SCRIPT_DISPATCH {
  UINT16    OpCode;                           ///< operator
  VOID      *EntryPoint;                      ///< EntryPoint
  VOID      *Context;                         ///< Context
} HYGON_S3LIB_BOOT_SCRIPT_DISPATCH;

/// Union structure of Hygon S3 Lib script data
typedef union _HYGON_S3LIB_BOOT_SCRIPT_DATA {
  HYGON_S3LIB_BOOT_SCRIPT_COMMON       Common;   ///< Common
  HYGON_S3LIB_BOOT_SCRIPT_IO_WRITE     IoWrite;  ///< IoWrite
  HYGON_S3LIB_BOOT_SCRIPT_IO_RMW       IoRMW;    ///< IoRMW
  HYGON_S3LIB_BOOT_SCRIPT_IO_POLL      IoPoll;   ///< IoPoll
  HYGON_S3LIB_BOOT_SCRIPT_MEM_WRITE    MemWrite; ///< MemWrite
  HYGON_S3LIB_BOOT_SCRIPT_MEM_RMW      MemRMW;   ///< MemRMW
  HYGON_S3LIB_BOOT_SCRIPT_MEM_POLL     MemPoll;  ///< MemPoll
  HYGON_S3LIB_BOOT_SCRIPT_PCI_WRITE    PciWrite; ///< PciWrite
  HYGON_S3LIB_BOOT_SCRIPT_PCI_RMW      PciRMW;   ///< PciRMW
  HYGON_S3LIB_BOOT_SCRIPT_PCI_POLL     PciPoll;  ///< PciPoll
  HYGON_S3LIB_BOOT_SCRIPT_DISPATCH     Dispatch; ///< Dispatch
} HYGON_S3LIB_BOOT_SCRIPT_DATA;

#endif //_HYGON_S3SAVE_LIB_DATA_H_
