/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CCX BASE Library - X86
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CCX
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
 */

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

#include "Uefi.h"
#include <Library/BaseLib.h>
#include "HYGON.h"
#include "Filecode.h"
#include <Library/HygonBaseLib.h>
#include <Library/HygonSocBaseLib.h>
#include <Library/IdsLib.h>
#include <Library/CcxBaseX86Lib.h>

#define FILECODE  LIBRARY_CCXBASEX86LIB_CCXBASEX86LIB_FILECODE

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U  N C T I O N S
 *----------------------------------------------------------------------------------------
 */
VOID
STATIC
CcxGetDataFromPtr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       VOID         *Data,
  IN       VOID         *DataMask,
  OUT   UINT32       *TemData,
  OUT   UINT32       *TempDataMask
  );

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the MMIO address used for PCI extended configuration space
 *
 * @param[in]     MmioAddress   Mmio Address
 * @param[in]     MmioSize      Mmio Size
 * @param[in]     StdHeader     Standard configuration header (Optional)
 */
BOOLEAN
CcxGetPciMmioAddress (
  OUT   UINT64            *MmioAddress,
  OUT   UINT32            *MmioSize,
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  BOOLEAN  MmioIsEnabled;
  UINT32   EncodedSize;
  UINT64   LocalMsrRegister;

  MmioIsEnabled    = FALSE;
  LocalMsrRegister = AsmReadMsr64 (0xC0010058);
  if ((LocalMsrRegister & BIT0) != 0) {
    *MmioAddress  = LocalMsrRegister & 0xFFFFFFFFFFF00000;
    EncodedSize   = (UINT32)RShiftU64 ((LocalMsrRegister & 0x3C), 2);
    *MmioSize     = ((1 << EncodedSize) * 0x100000);
    MmioIsEnabled = TRUE;
  }

  return MmioIsEnabled;
}

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the core's APIC ID at reset
 *
 *
 * @param[in]     StdHeader     Standard configuration header (Optional)
 */
UINT32
CcxGetInitialApicID (
  IN       HYGON_CONFIG_PARAMS *StdHeader
  )
{
  CPUID_DATA  CpuId;

  AsmCpuid (
    HYGON_CPUID_APICID_LPC_BID,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );
  return ((CpuId.EBX_Reg >> 24) & 0xFF);
}

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the value of ThreadsPerCore
 *
 *
 * @retval Value of ThreadsPerCore + 1
 */
UINT8
CcxGetThreadsPerCore (
  )
{
  CPUID_DATA  CpuId;

  AsmCpuid (
    0x8000001E,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );

  return (UINT8)(((CpuId.EBX_Reg >> 8) & 0xFF) + 1);
}

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the nodes of processor
 *
 *
 * @retval Value of NodesPerProcessor + 1
 */
UINT8
CcxGetNodesPerProcessor (
  )
{
  CPUID_DATA  CpuId;

  AsmCpuid (
    0x8000001E,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );

  return (UINT8)(((CpuId.ECX_Reg >> 8) & 0x7) + 1);
}

VOID
STATIC
CcxGetDataFromPtr (
  IN       ACCESS_WIDTH AccessWidth,
  IN       VOID         *Data,
  IN       VOID         *DataMask,
  OUT   UINT32       *TemData,
  OUT   UINT32       *TempDataMask
  )
{
  switch (AccessWidth) {
    case AccessWidth8:
    case AccessS3SaveWidth8:
      *TemData = (UINT32)*(UINT8 *)Data;
      *TempDataMask = (UINT32)*(UINT8 *)DataMask;
      break;
    case AccessWidth16:
    case AccessS3SaveWidth16:
      *TemData = (UINT32)*(UINT16 *)Data;
      *TempDataMask = (UINT32)*(UINT16 *)DataMask;
      break;
    case AccessWidth32:
    case AccessS3SaveWidth32:
      *TemData = *(UINT32 *)Data;
      *TempDataMask = *(UINT32 *)DataMask;
      break;
    default:
      IDS_ERROR_TRAP;
  }
}

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the value of PhysAddrSize
 *
 *
 * @retval Value of CPUID_Fn8000_00008_EAX[7:0, PhysAddrSize]
 */
UINT8
CcxGetPhysAddrSize (
  )
{
  CPUID_DATA  CpuId;

  AsmCpuid (
    0x80000008,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );

  return (UINT8)(CpuId.EAX_Reg & 0xFF);
}

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the value of ApicIdCoreIdSize
 *
 *
 * @retval Value of CPUID_Fn8000_00008_ECX[15:12, ApicIdCoreIdSize]
 */
UINT8
CcxGetApicIdCoreIdSize (
  )
{
  CPUID_DATA  CpuId;

  AsmCpuid (
    0x80000008,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );

  return (UINT8)((CpuId.ECX_Reg >> 12) & 0xF);
}

/*---------------------------------------------------------------------------------------*/

/**
 * Returns the value of LFuncExt
 *
 *
 * @retval Value of CPUID_Fn8000_00000_EAX[31:0, LFuncExt]
 */
UINT32
CcxGetLFuncExt (
  )
{
  CPUID_DATA  CpuId;

  AsmCpuid (
    0x80000000,
    &(CpuId.EAX_Reg),
    &(CpuId.EBX_Reg),
    &(CpuId.ECX_Reg),
    &(CpuId.EDX_Reg)
    );

  return (CpuId.EAX_Reg);
}
