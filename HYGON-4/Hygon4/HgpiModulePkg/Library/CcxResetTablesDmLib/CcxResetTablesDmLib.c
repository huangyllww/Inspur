/* $NoKeywords:$ */

/**
 * @file
 *
 * CCX Reset Tables Library - Dharm
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
 *
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
 ******************************************************************************
 */
/*++
Module Name:

  CcxResetTablesSatLib.c
  Apply register table settings

Abstract:
--*/

#include <HGPI.h>
#include <Library/HygonTableLib.h>
#include <SocLogicalId.h>
#include <Filecode.h>

#define FILECODE  LIBRARY_CCXRESETTABLESDMLIB_CCXRESETTABLESDMLIB_FILECODE

CONST UINT8 ROMDATA  CcxDmAllCoreRegistersAfterApLaunch[] = {

  // HWCR (0xC0010015)
  // bit[27]     EffFreqReadOnlyLock = 1
  MAKE_MSR_ENTRY (0xC0010015,                    0x0000000008000000, 0x0000000008000000),

  // CPUID_FEATURES (0xC0011004)
  // bit[53]     X2APIC Feature = 0
  MAKE_MSR_PLATFORM_FEAT_ENTRY (HYGON_PF_APIC,   0xC0011004,         0,                  BIT53),

  // CPUIDExt0xC0011005
  // bit [48] FMA4 = 0
  // bit [42] IBS = 0
  MAKE_MSR_ENTRY (0xC0011005,                    0x0000000000000000, 0x0001040000000000),

  // Table Terminator
  MAKE_TABLE_TERMINATOR
};

STATIC CONST UINT8 ROMDATA  CcxDmComputeUnitRegistersAfterApLaunch[] =
{
  // M S R    T a b l e s
  // ----------------------
  // MCA_CTL_MASK_IF (0xC0010401)
  // bit [13]    SystemReadDataError = 1
  MAKE_MSR_ENTRY (0xC0010401,                 0x0000000000002000,   0x0000000000002000),

  // MCA_CTL_MASK_L2 (0xC0010402)
  // bit [3]     Hwa = 1
  MAKE_MSR_ENTRY (0xC0010402,                 0x0000000000000008,   0x0000000000000008),

  // MCA_CTL_MASK_FP (0xC0010406)
  // bit [6]     Hwa = 1
  MAKE_MSR_ENTRY (0xC0010406,                 0x0000000000000040,   0x0000000000000040),

  // Reserved (0xC0011020)
  // bit [57]   Reserved = 1, for all DN Revs
  MAKE_MSR_CPU_REV_ENTRY (HYGON_FAMILY_DN,    HYGON_REV_F18_DN_ALL, 0xC0011020,         0x0200000000000000,   0x0200000000000000),

  // Reserved (0xC001102D)
  // bit [5]    Reserved = 1, for all DN Revs
  MAKE_MSR_CPU_REV_ENTRY (HYGON_FAMILY_DN,    HYGON_REV_F18_DN_ALL, 0xC001102D,         0x0000000400000020,   0x0000000400000020),

  // TW_CFG (0xC0011023)
  // bit[45]     Reserved = 1 if DHM1
  MAKE_MSR_PLATFORM_FEAT_ENTRY (HYGON_PF_DM1, 0xC0011023,           0x0000200000000000, 0x0000200000000000),

  // Reserved (0xC0011028)
  // bit[4]      Reserved = 1
  // bit[2]      Reserved = 1
  MAKE_MSR_ENTRY (0xC0011028,                 0x0000000000000014,   0x0000000000000014),

  // Reserved (0xC0011029)
  // bit[17]      Reserved = 1
  // bit[13]      Reserved = 1
  MAKE_MSR_ENTRY (0xC0011029,                 0x0000000000022000, 0x0000000000022000),

  // Reserved (0xC0011029)
  // bit[28]    Reserved = 1, for all DN Revs
  MAKE_MSR_CPU_REV_ENTRY (HYGON_FAMILY_18_DN, HYGON_REV_F18_DN_ALL, 0xC0011029,         0x0000000010000000,   0x0000000010000000),

  // Reserved (0xC001102A)
  // bit[57]       Reserved = 1, for all DN
  // bit[55]       Reserved = 1, for all DN
  MAKE_MSR_CPU_REV_ENTRY (HYGON_FAMILY_18_DN, HYGON_REV_F18_DN_ALL, 0xC001102A,         0x0280000000000000,   0x0280000000000000),

  // Reserved (0xC001102A)
  // bit[58]       Reserved = 1, for all DN
  MAKE_MSR_CPU_REV_ENTRY (HYGON_FAMILY_18_DN, HYGON_REV_F18_DN_ALL, 0xC001102A,         0x0400000000000000,   0x0400000000000000),

  // Reserved (0xC001102A)
  // bit[15]     Reserved = 1
  // bit[7]      Reserved = 1
  MAKE_MSR_ENTRY (0xC001102A,                 0x0000000000008080,   0x0000000000008080),

  // Reserved (0xC001102C)
  // bit[54]     Reserved = 1
  // bit[53]     Reserved = 1
  // bit[51]     Reserved = 1
  MAKE_MSR_ENTRY (0xC001102C,                 0x0068000000000000,   0x0068000000000000),

  // Reserved (0xC001102B)
  // bit[19]     Reserved = 1
  MAKE_MSR_ENTRY (0xC001102B,                 0x0000000000080000,   0x0000000000080000),

  // Reserved (0xC001102D)
  // bit[60]     Reserved = 1
  // bit[32]     Reserved = 1
  MAKE_MSR_ENTRY (0xC001102D,                 0x1000000100000000,   0x1000000100000000),

  // Reserved (0xC001102E)
  // bit[0]      Reserved = 1, for all DN
  MAKE_MSR_CPU_REV_ENTRY (HYGON_FAMILY_18_DN, HYGON_REV_F18_DN_ALL, 0xC001102E,         0x0000000000000001,   0x0000000000000001),

  // Reserved (0xC0011074)
  // bit[60]     Reserved = 0
  MAKE_MSR_ENTRY (0xC0011074,                 0x0000000000000000,   0x1000000000000000),

  // Table Terminator
  MAKE_TABLE_TERMINATOR
};

CONST UINT8 ROMDATA  CcxDmComplexRegistersAfterApLaunch[] = {
  // Reserved (0xC0011093)
  // bit[22]     Reserved = 1
  // bit[20:19]  Reserved = 1
  // bit[18]     Reserved = 0
  // bit[16]     Reserved = 0
  // bit[15]     Reserved = 0
  // bit[12:10]  Reserved = 0
  // bit[9:8]    Reserved = 0
  // bit[6:4]    Reserved = 6
  // bit[3:2]    Reserved = 0
  MAKE_MSR_CPU_REV_ENTRY (HYGON_FAMILY_18_DN, HYGON_REV_F18_DN_ALL, 0xC0011093,          0x0000000000480060, 0x00000000005D9F7C),

  // Reserved (0xC0011097)
  // bit[11:9]   Reserved = 7
  // bit[8:6]    Reserved = 7
  // bit[5:3]    Reserved = 6
  // bit[2:1]    Reserved = 3
  MAKE_MSR_ENTRY (0xC0011097,                 0x0000000000000FF6,   0x0000000000000FFE),

  // Table Terminator
  MAKE_TABLE_TERMINATOR
};

CONST REGISTER_TABLE ROMDATA  CcxDmComplexRegTableAfterApLaunch = {
  ComplexPrimary,
  CcxDmComplexRegistersAfterApLaunch,
};

CONST REGISTER_TABLE ROMDATA  CcxDmCURegTableAfterApLaunch = {
  ComputeUnitPrimary,
  CcxDmComputeUnitRegistersAfterApLaunch,
};

CONST REGISTER_TABLE ROMDATA  CcxDmAllCoreRegTableAfterApLaunch = {
  AllCores,
  CcxDmAllCoreRegistersAfterApLaunch,
};

CONST REGISTER_TABLE ROMDATA  *CcxDmRegisterTablesAfterApLaunch[] = {
  &CcxDmAllCoreRegTableAfterApLaunch,
  &CcxDmCURegTableAfterApLaunch,
  &CcxDmComplexRegTableAfterApLaunch,
  NULL
};

CONST REGISTER_TABLE ROMDATA  *CcxDmRegisterTablesAfterApLaunchSecureS3[] = {
  NULL
};

CONST REGISTER_TABLE_AT_GIVEN_TP ROMDATA  CcxDmRegTableListAtGivenTP[] = {
  { HygonRegisterTableTpAfterApLaunch,         CcxDmRegisterTablesAfterApLaunch         },
  { HygonRegisterTableTpAfterApLaunchSecureS3, CcxDmRegisterTablesAfterApLaunchSecureS3 },
  { MaxHygonRegisterTableTps,                  NULL                                     }
};

/*++

Routine Description:

  Dharma - Satori Register table programming

Arguments:
  SleepType
  HYGON_CONFIG_PARAMS *

Returns:

--*/
VOID
CcxProgramTablesAtReset (
  IN       UINT8               SleepType,
  IN OUT   HYGON_CONFIG_PARAMS  *StdHeader
  )
{
  if (SleepType == 0x03) {
    SetRegistersFromTablesAtGivenTimePoint ((REGISTER_TABLE_AT_GIVEN_TP *)&CcxDmRegTableListAtGivenTP[0], HygonRegisterTableTpAfterApLaunchSecureS3, StdHeader);
  } else {
    SetRegistersFromTablesAtGivenTimePoint ((REGISTER_TABLE_AT_GIVEN_TP *)&CcxDmRegTableListAtGivenTP[0], HygonRegisterTableTpAfterApLaunch, StdHeader);
  }
}
