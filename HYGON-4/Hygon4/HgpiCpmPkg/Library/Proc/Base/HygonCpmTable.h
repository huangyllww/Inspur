/* $NoKeywords:$ */

/**
 * @file
 *
 * HYGON CPM Library for CPM Table Access
 *
 * Contains interface to the HYGON CPM library
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

#ifndef _HYGON_CPM_TABLE_H_
#define _HYGON_CPM_TABLE_H_

VOID *
EFIAPI
CpmAddTable (
  IN       VOID                    *This,
  IN       VOID                    *TablePtr
  );

VOID *
EFIAPI
CpmRemoveTable (
  IN       VOID                    *This,
  IN       VOID                    *TablePtr
  );

VOID *
EFIAPI
CpmGetTablePtr (
  IN       VOID                    *This,
  IN       UINT32                  TableId
  );

VOID *
EFIAPI
CpmGetTablePtr2 (
  IN       VOID                    *This,
  IN       UINT32                  TableId
  );

UINT16
EFIAPI
CpmGetTableSize (
  IN       VOID                    *TablePtr
  );

VOID
EFIAPI
CpmCopyMem (
  IN OUT   VOID                    *destination,
  IN       VOID                    *source,
  IN       UINTN                   size
  );

VOID
EFIAPI
CpmGenerateTableList (
  IN       VOID                    *TableListPtr,
  IN       UINT16                  PlatformId,
  IN OUT   HYGON_CPM_TABLE_LIST      *TableInHobListPtr
  );

VOID *
EFIAPI
CpmCopyTableListToMemory (
  IN       HYGON_CPM_TABLE_LIST      *TableInHobListPtr,
  IN OUT   VOID                    *HobTablePtr
  );

VOID
EFIAPI
CpmRelocateTableList (
  IN OUT   VOID                    *TablePtr
  );

VOID *
EFIAPI
CpmSearchAndUpdateTable (
  IN       VOID                    *This,
  IN       UINT32                  TableId,
  IN       UINT8                   Flag
  );

VOID
EFIAPI
CpmPostCode (
  IN       UINT32                  Postcode
  );

HYGON_CPM_TABLE_LIST *
EFIAPI
CpmAdjustTableList (
  IN       VOID                *HobTablePtr
  );

#endif
