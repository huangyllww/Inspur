/* $NoKeywords:$ */

/**
 * @file
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
 *
 ***************************************************************************/

#include "MyPorting.h"
#include <MiscMemDefines.h>
#include <HPCB.h>
#include <HpcbCustomizedDefinitions.h>
#include <HpcbAutoGen.h>

// #define HPCB_TYPE_DATA_START_SIGNATURE()    _16BYTE_ALIGN CHAR8 SigStartTypeData[16] = { '$', 'A', 'P', 'C', 'B', '_', 'T', 'Y', 'P', 'E', '_', 'S', 'T', 'A', 'R', 'T' }
// #define HPCB_TYPE_DATA_END_SIGNATURE()      _4BYTE_ALIGN CHAR8 SigEndTypeData[16]   = { '$', 'A', 'P', 'C', 'B', '_', 'T', 'Y', 'P', 'E', '_', 'E', 'N', 'D', '_', '$' }

CHAR8  mDummyBuf;

HPCB_TYPE_DATA_START_SIGNATURE ();
HPCB_TYPE_HEADER  HpcbTypeHeader = {
  HPCB_GROUP_CBS,
  HPCB_TYPE_CBS_COMMON_PARAMETERS,
  (
   sizeof (HPCB_TYPE_HEADER)
  ),
  0,
  0,
  0
};

HPCB_PARAM_ATTRIBUTE  InternalParameterListDefaultsAttribute[] = {
  HPCB_CBS_CMN_DEF_ATRIBS_AUTOGEN
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CBS_CMN_SOCKET_NUMBER, sizeof (UINT8) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CBS_CMN_XHMI_CONNECT_TYPE, sizeof (UINT8) - 1, 0 },
  { 0, HPCB_TOKEN_CBS_CMN_LIMIT, 0, 0 },
};

_1BYTE_ALIGN  UINT8  InternalParameterListDefaultsValue[] = {
  HPCB_CBS_CMN_DEF_VALUES_AUTOGEN
  BSU08 (BLDCFG_SOCKET_NUMBER),
  BSU08 (BLDCFG_SOCKET_XHMI_CONNECT_TYPE),
  BSU08 (0xFF),
};

HPCB_TYPE_DATA_END_SIGNATURE ();

int
main (
  IN int argc, IN char *argv[]
  )
{
  return 0;
}
