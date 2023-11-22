/**
 * @file
 *
 * HYGON Integrated Debug Hook Routines
 *
 * Contains all functions related to IDS Hook
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  IDS
 */
/*****************************************************************************
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

#ifndef _HYGON_IDS_HOOK_EXTLIB_H_
#define _HYGON_IDS_HOOK_EXTLIB_H_

#include <IdsHookId.h>

///
/// The return status for all IDS HOOK services.
///
typedef enum {
  IDS_HOOK_SUCCESS = 0,       ///< The service completed normally
  IDS_HOOK_UNSUPPORTED,       ///< Unsupported IDS HOOK
  IDS_HOOK_BUFFER_TOO_SMALL,  ///< Too small buffer
  IDS_HOOK_NOT_FOUND,         ///< Haven't found accordingly service entry for specific IDS HOOK ID
  IDS_HOOK_ERROR,             ///< Error happens during service IDS HOOK
  IDS_HOOK_SKIP,              ///< Use to notify the IDS HOOK caller to skip a block of codes, used for IDS_HOOK_SKIP
  IDS_HOOK_NO_SKIP,           ///< Use to notify the IDS HOOK caller not skip a block of codes, used for IDS_HOOK_SKIP
  IDS_HOOK_MAX                ///< Not a status, for limit checking.
} IDS_HOOK_STATUS;

/**
 * Entry function of IDS Ext HOOK
 *
 *
 * @param[in] HookId            ID used to identify the IDS HOOK
 * @param[in,out] Handle        Optional, Used to transfer variable information except the Data.
 *                              e.g. It can set with the service table pointer. Or
 *                              It can set with the global data structure
 * @param[in,out] Data          Point to the data used in the Hook
 *
 * @retval    IDS_HOOK_SUCCESS     Hook execute success
 * @retval    other value          refer to IDS_HOOK_STATUS
 *
 */
IDS_HOOK_STATUS
IdsHookExtEntry (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
  );

/**
 * Get IDS NV Table
 *
 * IDS NV table is a data filled multiple IDS_NV_RECORD structure
 *
 * @param[in,out] IdsNvTable        Data pointer used to filled with multiple IDS_NV_RECORD structure
 *                              The memory space need be pre-allocated before call this routine
 *
 * @param[in,out] IdsNvTableSize   Size of IdsNvTable, if size is less than IdsNvTable occupied, it will
 *                                 filled with the required data size on ouput.
 *                                 On input, the size in bytes allocated for the return IdsNvTable.
 *                                 On output, the size of data returned in IdsNvTable.
 *
 * @retval    IDS_HOOK_SUCCESS     Get IDS NV Table successfully
 * @retval    IDS_HOOK_BUFFER_TOO_SMALL       Buffer too small to hold IdsNvTable
 * @retval    Other                Other error happens during get IDS NV Table
 *
 */
IDS_HOOK_STATUS
GetIdsNvTable (
  IN OUT   VOID *IdsNvTable,
  IN OUT   UINT32 *IdsNvTableSize
  );

#endif //#define _HYGON_IDS_HOOK_EXTLIB_H_
