/* $NoKeywords:$ */

/**
 * @file
 *
 * Smn register access service procedures
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: GNB
 *
 */
/*
*****************************************************************************
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
* ***************************************************************************
*
*/

#ifndef _SMNACCESSLIB_H_
#define _SMNACCESSLIB_H_
#include  <HGPI.h>

#ifndef D0F0xB8_ADDRESS
  // **** D0F0xB8 Register Definition ****
  // Address
  #define D0F0xB8_ADDRESS  0xB8
#endif

#ifndef D0F0xBC_ADDRESS
  // **** D0F0xB8 Register Definition ****
  // Address
  #define D0F0xBC_ADDRESS  0xBC
#endif

#ifndef GNB_REG_ACC_FLAG_S3SAVE
  #define GNB_REG_ACC_FLAG_S3SAVE  0x00000001ul
#endif

VOID
SmnRegisterRead (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  OUT      VOID                *Value
  );

VOID
SmnRegisterWrite (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  );

VOID
SmnRegisterRMW (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue,
  IN       UINT32              Flags
  );

VOID
SmnRegisterReadBySocketRb (
  IN       UINT8               Socket,
  IN       UINT8               RbNumber,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  OUT      VOID                *Value
  );

VOID
SmnRegisterWriteBySocketRb (
  IN       UINT8               Socket,
  IN       UINT8               RbNumber,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  IN       VOID                *Value
  );

VOID
SmnRegisterRMWBySocketRb (
  IN       UINT8               Socket,
  IN       UINT8               RbNumber,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue
  );

VOID
SmnRegisterRead8 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  OUT      VOID                *Value
  );

VOID
SmnRegisterRead16 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  OUT      VOID                *Value
  );

VOID
SmnRegisterRead32 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  OUT      VOID                *Value
  );

VOID
SmnRegisterWrite8 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  );

VOID
SmnRegisterWrite16 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  );

VOID
SmnRegisterWrite32 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       VOID                *Value,
  IN       UINT32              Flags
  );

VOID
SmnRegisterRMW8 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT8               AndMask,
  IN       UINT8               OrValue,
  IN       UINT32              Flags
  );

VOID
SmnRegisterRMW16 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT16              AndMask,
  IN       UINT16              OrValue,
  IN       UINT32              Flags
  );

VOID
SmnRegisterRMW32 (
  IN       UINT32              BusNumber,
  IN       UINT32              Address,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue,
  IN       UINT32              Flags
  );

VOID
SmnRegisterReadByRbIndex (
  IN       UINT32              SysRbIndex,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value
  );

VOID
SmnRegisterWriteByRbIndex (
  IN       UINT32              SysRbIndex,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value,
  IN       UINT32              Flags
  );

VOID
SmnRegisterRMWByRbIndex (
  IN       UINT32              SysRbIndex,
  IN       UINT32              SmnAddress,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue,
  IN       UINT32              Flags
  );

VOID
SmnRegisterReadBySocket (
  IN       UINT8               Socket,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  OUT      VOID                *Value
  );

VOID
SmnRegisterWriteBySocket (
  IN       UINT8               Socket,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  IN       VOID                *Value
  );

VOID
SmnRegisterRMWBySocket (
  IN       UINT8               Socket,
  IN       UINT32              SmnAddress,
  IN       ACCESS_WIDTH        Width,
  IN       UINT32              AndMask,
  IN       UINT32              OrValue
  );

#endif
