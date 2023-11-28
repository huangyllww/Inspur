/**
 * @file
 *
 * AMD Integrated Debug Hook Routines
 *
 * Contains all functions related to IDS Hook
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  IDS
 * @e \$Revision: 309090 $   @e \$Date: 2014-12-10 02:28:05 +0800 (Wed, 10 Dec 2014) $
 */
/*****************************************************************************
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
#ifndef _AMD_IDS_HOOKLIB_H_
#define _AMD_IDS_HOOKLIB_H_
#include <IdsHookId.h>
#include <IdsNvTable.h>
#include <Library/AmdIdsHookExtLib.h>

#define IDS_HOOK_TABLE_SIGNATURE    0x53444924ul      ///< $IDS
#define IDS_HOOK_TABLE_REV_1        0x00000001ul
/// Structure of IDS HOOK table header
typedef struct _IDS_HOOK_TABLE_HEADER {
  UINT32 Signature;       ///< Signature
  UINT32 Revision;        ///< Revision
  UINT8  Reserved[24];        ///< Reserved
} IDS_HOOK_TABLE_HEADER;

#define IDS_HOOK_TABLE_HEADER_REV1_DATA \
{\
    IDS_HOOK_TABLE_SIGNATURE, \
    IDS_HOOK_TABLE_REV_1, \
}

/// Declare the function type
typedef IDS_HOOK_STATUS
(IDS_HOOK_FUNC) (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
);

/// Declare the function pointer type
typedef IDS_HOOK_STATUS
(*PIDS_HOOK_FUNC) (
  HOOK_ID HookId,
  VOID *Handle,
  VOID *Data
);

#define IDS_HOOK_ID_END       0xfffffffful      ///< Reserved hook ID used to identify end of IDS_HOOK arrary
#define IDS_HOOK_FUNC_END     0xfffffffful      ///< Reserved FUNC address used to identify end of IDS_HOOK arrary

#define IDS_HOOKS_END \
{\
    IDS_HOOK_ID_END, \
    (PIDS_HOOK_FUNC) (UINTN) IDS_HOOK_FUNC_END, \
}

/// A basic element of a IDS HOOK
typedef struct _IDS_HOOK_ELEMENT {
  HOOK_ID HookId;                   ///< Hook ID used to specified call-in function
  PIDS_HOOK_FUNC IdsHookFunc;        ///< Function will be called for specific HookId
} IDS_HOOK_ELEMENT;

/// IDS HOOK Table will be process by IdsHookDispatcher
typedef struct _IDS_HOOK_TABLE {
  IDS_HOOK_TABLE_HEADER Header;     ///< Header
  IDS_HOOK_ELEMENT      *pIdsHook;          ///< Point to Array of IDS_HOOK
} IDS_HOOK_TABLE;

 /*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */

/// IDS HOOK Macro, Macro used to overriden local data value or set according register for debug purpose
/// HookId: Used to identify specific call-in function
/// Handle: A pointer useed to pass service
/// Data: A pointer used to pass data, can be used for both input & output
#ifndef REMOVE_IDS_HOOK
  #define IDS_HOOK(HookId, Handle, Data)\
  if (IsIdsHookEnabled ()) {\
       IdsHookEntry (HookId, Handle, Data); \
  }
#else
  #define IDS_HOOK(HookId, Handle, Data)
#endif

/// IDS HOOK Macro, Macro used to skip a block of code, depend on the result of IDS Hook
#ifndef REMOVE_IDS_HOOK
  #define IDS_SKIP_HOOK(HookId, Handle, Data)\
     if (IdsHookEntry (HookId, Handle, Data) != IDS_HOOK_SKIP)
#else
  #define IDS_SKIP_HOOK(HookId, Handle, Data)
#endif

/// Control IDS HOOK supported or not. TRUE: IDS HOOK supported. FALSE: IDS HOOK unsupported
BOOLEAN IsIdsHookEnabled ();


/**
 * Entry function of IDS HOOK
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
IdsHookEntry (
  IN       HOOK_ID HookId,
  IN OUT   VOID *Handle,
  IN OUT   VOID *Data
);

/**
 * Return IDS HOOK function table, this function is required to implemented by IdsHookLib consumer
 *
 *
 * @param[in,out] IdsHookFunctionTable Pointer to be filled with IDS_HOOK_TABLE structure
 *
 * @retval    AGESA_SUCCESS     Successfully get the IDS_HOOK_TABLE
 * @retval    AGESA_ERROR       Error happens in get the IDS_HOOK_TABLE
 *
 */
AGESA_STATUS
GetIdsHookTable (
  IN OUT   IDS_HOOK_TABLE **IdsHookFunctionTable
);

/**
 * IDS HOOK dispatcher, it will loop and process the table report by GetIdsHookTable
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
IdsHookDispatcher (
  IN       HOOK_ID HookId,
  IN OUT   VOID *Handle,
  IN OUT   VOID *Data
);

/**
 * Used to get the value for a specific IdsNvId
 *
 *
 * @param[in] IdsNvId            ID used to identify the IDS HOOK
 * @param[in,out] IdsNvValue     Pointer to be filled with corresponding IdsNvId value
 *
 * @retval    AGESA_SUCCESS     Find the according IDSNVID value successfully
 * @retval    AGESA_ERROR       Error occurs during finding the according IDSNVID value
 *
 */
AGESA_STATUS
AmdIdsNvReader (
  IN       IDS_NV_ID IdsNvId,
  IN OUT   UINT64 *IdsNvValue
  );

#define IDS_NV_READ_SKIP(Nvid, NvValue)\
  if ((AmdIdsNvReader ((Nvid), (NvValue)) == AGESA_SUCCESS))

#endif //#define _AMD_IDS_HOOKLIB_H_
