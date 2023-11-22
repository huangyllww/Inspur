/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM Table Protocol Declaration
 *
 * Contains code to initialize GUID for HYGON_CPM_TABLE_PROTOCOL
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      CPM
 * @e sub-project:  Library
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

#ifndef _HYGON_CPM_TABLE_PROTOCOL_H_
#define _HYGON_CPM_TABLE_PROTOCOL_H_

#include <HygonCpmBase.h>

//
// GUID definition
//
#define HYGON_CPM_TABLE_PROTOCOL_GUID \
  { 0x3724cf01, 0x00c2, 0x9762, 0x11, 0xb3, 0x0e, 0xa8, 0xaa, 0x89, 0x72, 0x00 }

#define HYGON_CPM_TABLE_SMM_PROTOCOL_GUID \
  { 0xaf6efacf, 0x7a13, 0x45a3, 0xb1, 0xa5, 0xaa, 0xfc, 0x06, 0x1c, 0x4b, 0x79 }

extern EFI_GUID  gHygonCpmTableProtocolGuid;
extern EFI_GUID  gHygonCpmTableSmmProtocolGuid;

/// DXE Protocol Structure
typedef struct _HYGON_CPM_TABLE_PROTOCOL {
  // See the Forward Declaration above
  UINTN                            Revision;                      ///< Protocol Revision
  HYGON_CPM_MAIN_TABLE             *MainTablePtr;                 ///< Pointer to CPM Main Table
  HYGON_CPM_CHIP_ID                ChipId;                        ///< Id of SB Chip
  HYGON_CPM_COMMON_FUNCTION        CommonFunction;                ///< Private Common Functions
  HYGON_CPM_DXE_PUBLIC_FUNCTION    DxePublicFunction;             ///< Public Function of Protocol
} HYGON_CPM_TABLE_PROTOCOL;

// Current Protocol Revision
#define HYGON_CPM_TABLE_PROTOCOL_REV  0x00

#endif
