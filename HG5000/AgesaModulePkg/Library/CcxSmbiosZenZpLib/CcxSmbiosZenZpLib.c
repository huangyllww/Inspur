/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD Ccx SMBIOS ZEN Library
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  Lib
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 *
 */
/*
 ******************************************************************************
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
 ******************************************************************************
 */


/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include <Library/BaseLib.h>
#include "AGESA.h"
#include "cpuRegisters.h"
#include "CcxRegistersZp.h"
#include "Filecode.h"
#include "CcxSmbiosZenZpCommonLib.h"
#include "Library/AmdBaseLib.h"
#include "Library/CcxSmbiosLib.h"

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */
#define FILECODE LIBRARY_CCXSMBIOSZENZPLIB_CCXSMBIOSZENZPLIB_FILECODE

/*----------------------------------------------------------------------------------------
 *                  T Y P E D E F S     A N D     S T R U C T U R E S
 *----------------------------------------------------------------------------------------
 */

/*----------------------------------------------------------------------------------------
 *           P R O T O T Y P E S     O F     L O C A L     F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */


/*----------------------------------------------------------------------------------------
 *                          E X P O R T E D    F U N C T I O N S
 *----------------------------------------------------------------------------------------
 */

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxGetProcessorFamilyForSmbios
 *
 *    Get processor family information for SMBIOS
 *
 *    @param[in]      Socket         Zero-based socket number to check
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
UINT16
CcxGetProcessorFamilyForSmbios (
  IN       UINTN               Socket,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  return P_FAMILY_OTHER; //"Other" 
}

/* -----------------------------------------------------------------------------*/
/**
 *
 *  CcxGetProcessorUpgradeForSmbios
 *
 *    Get processor upgrade information for SMBIOS
 *
 *    @param[in]      Socket         Zero-based socket number to check
 *    @param[in]      StdHeader      Standard Head Pointer
 *
 */
UINT8
CcxGetProcessorUpgradeForSmbios (
  IN       UINTN               Socket,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT8       PackageType;
  UINT8       ProcUpgrade;
  CPUID_DATA  CpuId;

  AsmCpuid (
      AMD_CPUID_FMF,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  PackageType = (UINT8) (CpuId.EBX_Reg >> 28) & 0xF; // bit 31:28

  switch (PackageType) {
  case ZP_SOCKET_SP3:
    ProcUpgrade = P_UPGRADE_SP3;
    break;
  case ZP_SOCKET_AM4:
    ProcUpgrade = P_UPGRADE_AM4;
    break;
  case ZP_SOCKET_SP4:
    ProcUpgrade = P_UPGRADE_NONE;
    break;
  case ZP_SOCKET_SP3r2:
    ProcUpgrade = P_UPGRADE_SP3r2;
    break;
  case ZP_SOCKET_SP4r2:
    ProcUpgrade = P_UPGRADE_NONE;
    break;
  default:
    ProcUpgrade = P_UPGRADE_UNKNOWN;
    break;
  }
  return ProcUpgrade;
}

VOID
CcxGetCacheInfo (
  IN       UINTN               Socket,
  IN       GET_CACHE_INFO     *CacheInfo,
  IN       AMD_CONFIG_PARAMS  *StdHeader
  )
{
  UINT32                CacheSize;
  UINT32                NumThreads;
  UINT32                NumSharing;
  CPUID_DATA            CpuId;

  AsmCpuidEx (
      0x80000008,
      0,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  NumThreads = ((CpuId.ECX_Reg & 0xFF) + 1);

  // L1 Size & Associativity
  AsmCpuidEx (
      0x8000001D,
      0,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  NumSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
  AsmCpuid (
      AMD_CPUID_TLB_L1Cache,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize = (CpuId.ECX_Reg >> 24) * (NumThreads / NumSharing);

  AsmCpuidEx (
      0x8000001D,
      1,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  NumSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
  AsmCpuid (
      AMD_CPUID_TLB_L1Cache,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize += (CpuId.EDX_Reg >> 24) * (NumThreads / NumSharing);
  CacheInfo->CacheEachLevelInfo[CpuL1Cache].CacheSize = AdjustGranularity (&CacheSize);
  CacheInfo->CacheEachLevelInfo[CpuL1Cache].Associativity = DMI_ASSOCIATIVE_8_WAY;

  // L2 Size & Associativity
  AsmCpuidEx (
      0x8000001D,
      2,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  NumSharing = (((CpuId.EAX_Reg >> 14) & 0xFFF) + 1);
  AsmCpuid (
      AMD_CPUID_L2L3Cache_L2TLB,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize = (CpuId.ECX_Reg >> 16) * (NumThreads / NumSharing);
  CacheInfo->CacheEachLevelInfo[CpuL2Cache].CacheSize = AdjustGranularity (&CacheSize);
  CacheInfo->CacheEachLevelInfo[CpuL2Cache].Associativity = DMI_ASSOCIATIVE_8_WAY;

  // L3 Size & Associativity
  AsmCpuid (
      AMD_CPUID_L2L3Cache_L2TLB,
      &(CpuId.EAX_Reg),
      &(CpuId.EBX_Reg),
      &(CpuId.ECX_Reg),
      &(CpuId.EDX_Reg)
      );
  CacheSize = (CpuId.EDX_Reg >> 18) * 512;
  CacheInfo->CacheEachLevelInfo[CpuL3Cache].CacheSize = AdjustGranularity (&CacheSize);
  switch (((CpuId.EDX_Reg >> 12) & 0xF)) {
  case 8:
    CacheInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = DMI_ASSOCIATIVE_16_WAY;
    break;
  case 0xA:
    CacheInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = DMI_ASSOCIATIVE_32_WAY;
    break;
  case 0xC:
    CacheInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = DMI_ASSOCIATIVE_64_WAY;
    break;
  default:
    CacheInfo->CacheEachLevelInfo[CpuL3Cache].Associativity = DMI_ASSOCIATIVE_UNKNOWN;
    break;
  }
}

/*---------------------------------------------------------------------------------------
 *                           L O C A L    F U N C T I O N S
 *---------------------------------------------------------------------------------------
 */

