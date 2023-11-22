/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON Ccx set MMIO configuration space base address LIB
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  Lib
 *
 */
/*****************************************************************************
 *
 * Copyright 2016 - 2023 CHENGDU HAIGUANG IC DESIGN CO., LTD.  All Rights Reserved.
 *
 * HYGON is granting you permission to use this software and documentation (if
 * any) (collectively, the "Materials") pursuant to the terms and conditions of
 * the Software License Agreement included with the Materials.  If you do not
 * have a copy of the Software License Agreement, contact your HYGON
 * representative for a copy.
 *
 * You agree that you will not reverse engineer or decompile the Materials, in
 * whole or in part, except as allowed by applicable law.
 *
 * WARRANTY DISCLAIMER:  THE MATERIALS ARE PROVIDED "AS IS" WITHOUT WARRANTY OF
 * ANY KIND.  HYGON DISCLAIMS ALL WARRANTIES, EXPRESS, IMPLIED, OR STATUTORY,
 * INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE, NON-INFRINGEMENT, THAT THE
 * MATERIALS WILL RUN UNINTERRUPTED OR ERROR-FREE OR WARRANTIES ARISING FROM
 * CUSTOM OF TRADE OR COURSE OF USAGE.  THE ENTIRE RISK ASSOCIATED WITH THE USE
 * OF THE MATERIAL IS ASSUMED BY YOU.  Some jurisdictions do not allow the
 * exclusion of implied warranties, so the above exclusion may not apply to
 * You.
 *
 * LIMITATION OF LIABILITY AND INDEMNIFICATION:  HYGON AND ITS LICENSORS WILL
 * NOT, UNDER ANY CIRCUMSTANCES BE LIABLE TO YOU FOR ANY PUNITIVE, DIRECT,
 * INCIDENTAL, INDIRECT, SPECIAL OR CONSEQUENTIAL DAMAGES ARISING FROM USE OF
 * THE MATERIALS OR THIS AGREEMENT EVEN IF HYGON AND ITS LICENSORS HAVE BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.  In no event shall HYGON's total
 * liability to You for all damages, losses, and causes of action (whether in
 * contract, tort (including negligence) or otherwise) exceed the amount of
 * $100 USD. You agree to defend, indemnify and hold harmless HYGON and its
 * licensors, and any of their directors, officers, employees, affiliates or
 * agents from and against any and all loss, damage, liability and other
 * expenses (including reasonable attorneys' fees), resulting from Your use of
 * the Materials or violation of the terms and conditions of this Agreement.
 *
 * U.S. GOVERNMENT RESTRICTED RIGHTS:  The Materials are provided with
 * "RESTRICTED RIGHTS." Use, duplication, or disclosure by the Government is
 * subject to the restrictions as set forth in FAR 52.227-14 and
 * DFAR252.227-7013, et seq., or its successor.  Use of the Materials by the
 * Government constitutes acknowledgment of HYGON's proprietary rights in them.
 *
 * EXPORT RESTRICTIONS: The Materials may be subject to export restrictions as
 * stated in the Software License Agreement.
 *******************************************************************************
 */
/*++
Module Name:

  CcxSetMmioCfgBaseLib.c
  Set MMIO configuration space base address

Abstract:
--*/

#include <HGPI.h>
#include <Library/BaseLib.h>
#include <CpuRegisters.h>
#include <Filecode.h>
#include <Library/CcxRolesLib.h>

#define FILECODE  LIBRARY_CCXSETMMIOCFGBASELIB_CCXSETMMIOCFGBASELIB_FILECODE

/* -----------------------------------------------------------------------------*/

/**
 *
 *  CcxSetMmioCfgBaseLib
 *
 *  Description:
 *    This routine sets MSR_MMIO_Cfg_Base register
 *
 */
VOID
CcxSetMmioCfgBaseLib (
  )
{
  MMIO_CFG_BASE_MSR  MmioCfgBase;

  if (CcxIsBsp (NULL)) {
    MmioCfgBase.Value = AsmReadMsr64 (MSR_MMIO_Cfg_Base);
    if (MmioCfgBase.Field.Enable == 0) {
      // Set [47:20][MmioCfgBaseAddr]
      // Set [0][Enable]
      // [5:2][BusRange] has been set by ABL
      MmioCfgBase.Field.MmioCfgBaseAddr = RShiftU64 (PcdGet64 (PcdPciExpressBaseAddress), 20);

      if (MmioCfgBase.Field.BusRange == 0) {
        MmioCfgBase.Field.BusRange = 8;   //Default 256 bus
      }

      MmioCfgBase.Field.Enable = 1;
      AsmWriteMsr64 (MSR_MMIO_Cfg_Base, MmioCfgBase.Value);
    }
  }
}
