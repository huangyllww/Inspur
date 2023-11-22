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
#ifndef __HYGON_SMN_ADDRESS_LIB_H__
#define __HYGON_SMN_ADDRESS_LIB_H__

#ifndef NBIO_SPACE
  #define  NBIO_SPACE(HANDLE, ADDRESS) \
             (UINT32)((HANDLE->DieType == IOD_SATORI) ? \
                      ((HANDLE->RbId << 20) + ADDRESS) : \
                      (IOD0_SMN_BASE_HYGX + (HANDLE->PhysicalDieId << 28) + (HANDLE->RbId << 20) + ADDRESS))
#endif

#ifndef PCIE_STRAP_SPACE
  #define  PCIE_STRAP_SPACE(HANDLE, ADDRESS) \
             (UINT32)((HANDLE->DieType == IOD_SATORI) ? \
                      ((HANDLE->RbId << 8) + ADDRESS) : \
                      (IOD0_SMN_BASE_HYGX + (HANDLE->PhysicalDieId << 28) + (HANDLE->RbId << 8) + ADDRESS))
#endif

#ifndef NBIO_SPACE2
  #define  NBIO_SPACE2(PHYSICAL_DIE_ID, RB_ID, ADDRESS)  (UINT32)(LibNbioSmnAddr2 ((UINT8)(PHYSICAL_DIE_ID), (UINT8)(RB_ID), (UINT32)(ADDRESS)))
#endif

#ifndef NBIO_SPACE3
  #define  NBIO_SPACE3(PHYSICAL_DIE_ID, RB_ID, REG_TYPE, REG_OFT)  (UINT32)(LibNbioSmnAddr3 ((UINT8)(PHYSICAL_DIE_ID), (UINT8)(RB_ID), (UINT32)(REG_TYPE), (UINT32)(REG_OFT)))
#endif

//to remove
#ifndef IOD_SPACE
  #define  IOD_SPACE(IODINDEX, ADDRESS)  (UINT32)(LibIodSmnAddr ((UINT8)(IODINDEX), (UINT32)(ADDRESS)))
#endif

UINT32
LibNbioSmnAddr2 (UINT8 PhysicalDieId, UINT8 RbId, UINT32 Address);

UINT32
LibNbioSmnAddr3 (UINT8 PhysicalDieId, UINT8 RbId, UINT32 RegType, UINT32 RegOft);

UINT32
LibIodSmnAddr (UINT8 PhysicalDieId, UINT32 Address);

#endif