/* $NoKeywords:$ */

/**
 * @file
 *
 * HPCB DXE Driver Header
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      HGPI
 * @e sub-project:  HPCB
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
#include <Protocol/HygonHpcbProtocol.h>

#ifndef _HYGON_HPCB_SMM_H_
  #define _HYGON_HPCB_SMM_H_

  VOID
  mHpcbSetActiveInstance (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT8             Instance
  );

  EFI_STATUS
  mHpcbFlushData (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  );

  EFI_STATUS
  mHpcbUpdateCbsData (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT8             *HpcbTypeData,
  IN       UINT32            SizeInByte,
  IN       BOOLEAN           Internal
  );

  EFI_STATUS
  mHpcbGetConfigParameter (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

  EFI_STATUS
  mHpcbSetConfigParameter (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

  EFI_STATUS
  mHpcbGetConfigParameter (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

  EFI_STATUS
  mHpcbGetCbsParameter (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

  EFI_STATUS
  mHpcbSetCbsParameter (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       UINT16            TokenId,
  IN OUT   UINT32            *SizeInByte,
  IN OUT   UINT64            *Value
  );

  EFI_STATUS
  mHpcbGetDramPostPkgRepairEntry (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN OUT   DRRP_REPAIR_ENTRY *Entry,
  IN OUT   UINT32            *NumOfEntries
  );

  EFI_STATUS
  mHpcbClearDramPostPkgRepairEntry (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  );

  EFI_STATUS
  mHpcbAddDramPostPkgRepairEntry (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  );

  EFI_STATUS
  mHpcbRemoveDramPostPkgRepairEntry (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This,
  IN       DRRP_REPAIR_ENTRY *Entry
  );

// byo230831 + >>
EFI_STATUS
mHpcbReload (
  IN       HYGON_HPCB_SERVICE_PROTOCOL    *This
  );
// byo230831 + <<

#endif //_HYGON_HPCB_SMM_H_
