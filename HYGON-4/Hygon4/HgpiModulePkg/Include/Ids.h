/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON IDS Routines
 *
 * Contains HYGON HGPI Integrated Debug Macros
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  IDS
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

/* Macros to aid debugging */
/* These definitions expand to zero (0) bytes of code when disabled */

#ifndef _IDS_COMPATIBILITY_H_
#define _IDS_COMPATIBILITY_H_

#define IDS_OPTION_HOOK(IdsOption, DataPtr, StdHeader)
#define IDS_SKIP_HOOK(IdsOption, DataPtr, StdHeader)
#define IDS_PERF_TIMESTAMP(ID, StdHeader)

#define IEM_SKIP_CODE(L)
#define IEM_INSERT_CODE(L, Fn, Parm)
#define IDS_HDT_CONSOLE_DEBUG_CODE(Code)
#define IDS_TIMEOUT_CTL(t)

/// Data Structure for Mem ECC parameter override
typedef struct {
  IN BOOLEAN    CfgEccRedirection;                ///< ECC Redirection
  IN UINT16     CfgScrubDramRate;                 ///< Scrub Dram Rate
  IN UINT16     CfgScrubL2Rate;                   ///< Scrub L2Rate
  IN UINT16     CfgScrubL3Rate;                   ///< Scrub L3Rate
  IN UINT16     CfgScrubIcRate;                   ///< Scrub Ic Rate
  IN UINT16     CfgScrubDcRate;                   ///< Scrub Dc Rate
  IN BOOLEAN    CfgEccSyncFlood;                  ///< ECC Sync Flood
} ECC_OVERRIDE_STRUCT;

#endif // _IDS_COMPATIBILITY_H_
