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

CHAR8  mDummyBuf;

HPCB_TYPE_DATA_START_SIGNATURE ();
HPCB_TYPE_HEADER  HpcbTypeHeader = {
  HPCB_GROUP_MEMORY,
  HPCB_MEM_TYPE_ERROR_OUT_EVENT_CONTROL,
  (sizeof (HPCB_TYPE_HEADER) + sizeof (PSP_ERROR_OUT_CONTROL_STRUCT)),
  0,
  0,
  0
};

///< Platform Specific Overrides
PSP_ERROR_OUT_CONTROL_STRUCT  PspEventOutControl = {
  FALSE,
  FALSE,
  FALSE,
  FALSE,
  0x84,
  15000,
  0x80,
  FALSE,
  4,
  4,
  6,
  6,
  FALSE,
  {
    85,
    0x01,
    0xC0
  },
  {
    {BEEP_ERROR_TYPE_GENERAL|BEEP_ERROR_OPERATION_MASK,     1,   1000},
    {BEEP_ERROR_TYPE_MEMORY|BEEP_ERROR_OPERATION_MASK,      2,    500},
    {BEEP_ERROR_TYPE_DF|BEEP_ERROR_OPERATION_MASK,          3,    500},
    {BEEP_ERROR_TYPE_CCX|BEEP_ERROR_OPERATION_MASK,         4,    500},
    {BEEP_ERROR_TYPE_GNB|BEEP_ERROR_OPERATION_MASK,         5,    500},
    {BEEP_ERROR_TYPE_PSP|BEEP_ERROR_OPERATION_MASK,         6,    500},
    {BEEP_ERROR_TYPE_SMU|BEEP_ERROR_OPERATION_MASK,         7,    500},
    {BEEP_ERROR_TYPE_UNKNOW|BEEP_ERROR_OPERATION_MASK,      2,    100}
  },
  FALSE
};

HPCB_TYPE_DATA_END_SIGNATURE ();

int
main (
  IN int argc, IN char *argv[]
  )
{
  return 0;
}
