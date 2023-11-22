/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Library
 *
 * Contains interface to the HYGON HGPI library
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

#ifndef _HYGON_BASE_LIB_H_
#define _HYGON_BASE_LIB_H_
#include "Uefi.h"
#include "HYGON.h"
#include "Library/IdsLib.h"
#include "SocLogicalId.h"

#define IOCF8  0xCF8
#define IOCFC  0xCFC

// Reg Values for ReadCpuReg and WriteCpuReg
#define    CR4_REG  0x04
#define    DR0_REG  0x10
#define    DR1_REG  0x11
#define    DR2_REG  0x12
#define    DR3_REG  0x13
#define    DR7_REG  0x17

// IO
VOID
LibHygonIoRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  OUT      VOID *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
LibHygonIoWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
LibHygonIoRMW (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
LibHygonIoPoll (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT16 IoAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN       UINT64 Delay,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

// Memory or MMIO
VOID
LibHygonMemRead (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  OUT      VOID *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
LibHygonMemWrite (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Value,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
LibHygonMemRMW (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
LibHygonMemPoll (
  IN       ACCESS_WIDTH AccessWidth,
  IN       UINT64 MemAddress,
  IN       VOID *Data,
  IN       VOID *DataMask,
  IN       UINT64 Delay,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

// PCI
BOOLEAN
GetPciMmioAddress (
  OUT      UINT64            *MmioAddress,
  OUT      UINT32            *MmioSize,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
LibHygonPciFindNextCap (
  IN OUT   PCI_ADDR *Address,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

// Utility Functions
EFI_STATUS
EFIAPI
LibHygonInsertSmbiosString (
  IN OUT  CHAR8       **Destination,
  IN      UINTN       *DestMax,
  IN      CONST CHAR8 *Source
  );

VOID
LibHygonMemFill (
  IN       VOID *Destination,
  IN       UINT8 Value,
  IN       UINTN FillLength,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
LibHygonMemCopy (
  IN       VOID *Destination,
  IN       VOID *Source,
  IN       UINTN CopyLength,
  IN OUT   HYGON_CONFIG_PARAMS *StdHeader
  );

UINT16
  LibHygonSimNowEnterDebugger (
                               VOID
                               );

VOID
  LibHygonHDTBreakPoint (
                         VOID
                         );

VOID
LibHygonGetDataFromPtr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       VOID         *Data,
  IN       VOID         *DataMask,
  OUT      UINT32       *TemData,
  OUT      UINT32       *TempDataMask
  );

UINT8
LibHygonAccessWidth (
  IN       ACCESS_WIDTH AccessWidth
  );

VOID
  LibHygonFinit (
                 VOID
                 );

UINT32
  GetPlatformNumberOfSockets (
                              VOID
                              );

BOOLEAN
GetSocketModuleOfNode (
  IN       UINT32    Node,
  OUT      UINT32    *Socket,
  OUT      UINT32    *Module,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
PutEventLog (
  IN       HGPI_STATUS      EventClass,
  IN       UINT32            EventInfo,
  IN       UINT32            DataParam1,
  IN       UINT32            DataParam2,
  IN       UINT32            DataParam3,
  IN       UINT32            DataParam4,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
GetPciAddress (
  IN       HYGON_CONFIG_PARAMS *StdHeader,
  IN       UINT32            Socket,
  IN       UINT32            Module,
  OUT     PCI_ADDR          *PciAddress,
  OUT     HGPI_STATUS      *HgpiStatus
  );

BOOLEAN
IsProcessorPresent (
  IN       UINT32             Socket,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

HGPI_STATUS
GetTscRate (
  OUT      UINT32            *FrequencyInMHz,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

VOID
GetLogicalIdOfSocket (
  IN       UINT32            Socket,
  OUT      SOC_LOGICAL_ID    *LogicalId,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
GetNbPstateInfo (
  IN       PCI_ADDR               *PciAddress,
  IN       UINT32                 NbPstate,
  OUT      UINT32                 *FreqNumeratorInMHz,
  OUT      UINT32                 *FreqDivisor,
  OUT      UINT32                 *VoltageInuV,
  IN       HYGON_CONFIG_PARAMS      *StdHeader
  );

#endif // _HYGON_BASE_LIB_H_
