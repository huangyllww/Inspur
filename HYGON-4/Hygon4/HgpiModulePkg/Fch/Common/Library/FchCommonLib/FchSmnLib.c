/* $NoKeywords:$ */

/**
 * @file
 *
 * FCH SMN operation lib
 *
 *
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:     HGPI
 * @e sub-project: FCH
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
****************************************************************************
*/
#include "FchPlatform.h"
#define FILECODE  FCH_TAISHAN_FCHTAISHANCORE_COMMON_FCHSMNLIB_FILECODE

/**
 * FchSmnRead - Read SMN register
 *
 * @param[in] IohcBus     - IOHC (Node) bus number
 * @param[in] SmnAddress  - Register SMN address
 * @param[in] Value       - Pointer to register value
 * @param[in] StdHeader   -
 *
 */
VOID
FchSmnRead (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  SmnRegisterRead (IohcBus, SmnAddress, Value);
}

VOID
FchSmnWrite (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              *Value,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  SmnRegisterWrite (IohcBus, SmnAddress, Value, 0);
}

VOID
FchSmnRW (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT32              AndMask,
  IN       UINT32              OrMask,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  UINT32  RegValue;

  FchSmnRead (IohcBus, SmnAddress, &RegValue, StdHeader);
  RegValue &= AndMask;
  RegValue |= OrMask;
  FchSmnWrite (IohcBus, SmnAddress, &RegValue, StdHeader);
}

/**
 * FchSmnRead8 - Read SMN register in Byte
 *
 * @param[in] IohcBus     - IOHC (Node) bus number
 * @param[in] SmnAddress  - Register SMN address
 * @param[in] Value8      - Pointer to register value
 * @param[in] StdHeader   -
 *
 */
VOID
FchSmnRead8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               *Value8,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  SmnRegisterRead8 (IohcBus, SmnAddress, Value8);
}

VOID
FchSmnWrite8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               *Value8,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  SmnRegisterWrite8 (IohcBus, SmnAddress, Value8, 0);
}

VOID
FchSmnRW8 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT8               AndMask,
  IN       UINT8               OrMask,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  UINT8  RegValue;

  FchSmnRead8 (IohcBus, SmnAddress, &RegValue, StdHeader);
  RegValue &= AndMask;
  RegValue |= OrMask;
  FchSmnWrite8 (IohcBus, SmnAddress, &RegValue, StdHeader);
}

VOID
FchSmnRead16 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT16              *Value16,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  SmnRegisterRead16 (IohcBus, SmnAddress, Value16);
}

VOID
FchSmnWrite16 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT16              *Value16,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  SmnRegisterWrite16 (IohcBus, SmnAddress, Value16, 0);
}

VOID
FchSmnRW16 (
  IN       UINT32              IohcBus,
  IN       UINT32              SmnAddress,
  IN       UINT16              AndMask,
  IN       UINT16              OrMask,
  IN       HYGON_CONFIG_PARAMS   *StdHeader
  )
{
  UINT16  RegValue;

  FchSmnRead16 (IohcBus, SmnAddress, &RegValue, StdHeader);
  RegValue &= AndMask;
  RegValue |= OrMask;
  FchSmnWrite16 (IohcBus, SmnAddress, &RegValue, StdHeader);
}
