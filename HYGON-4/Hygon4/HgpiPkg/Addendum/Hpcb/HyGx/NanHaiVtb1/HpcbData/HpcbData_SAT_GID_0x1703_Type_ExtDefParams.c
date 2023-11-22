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
  HPCB_GROUP_DF,
  HPCB_DF_TYPE_CONFIG_DEFAULT_PARAMETERS,
  (
   sizeof (HPCB_TYPE_HEADER)
  ),
  0,
  0,
  0
};

HPCB_PARAM_ATTRIBUTE  ParameterListDefaultsAttribute[] = {
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_HMI_ENCRYPT,                 sizeof (UINT8) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_XHMI_ENCRYPT,                sizeof (UINT8) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_SAVE_RESTORE_MEM_ENCRYPT,    sizeof (UINT8) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_SYS_STORAGE_AT_TOP_OF_MEM,   sizeof (UINT8) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_PROBE_FILTER_ENABLE,         sizeof (UINT8) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_BOTTOMIO,                    sizeof (UINT8) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_MEM_INTERLEAVING,            sizeof (UINT8) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_DRAM_INTERLEAVE_SIZE,        sizeof (UINT8) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_ENABLE_CHAN_INTLV_HASH,      sizeof (UINT8) - 1,  0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_PCI_MMIO_SIZE,               sizeof (UINT32) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_CAKE_CRC_THRESH_PERF_BOUNDS, sizeof (UINT32) - 1, 0 },
  { HPCB_TIME_POINT_ANY, HPCB_TOKEN_CONFIG_DF_MEM_CLEAR_DISABLE,           sizeof (UINT8) - 1,  0 },

  { 0,                   HPCB_TOKEN_CONFIG_LIMIT,                          0,                   0 },
};

_1BYTE_ALIGN UINT8  ParameterListDefaultsValue[] = {
  BSU08 (BLDCFG_DF_HMI_ENCRYPT),                  // < HMI encryption
  BSU08 (BLDCFG_DF_XHMI_ENCRYPT),                 // < xHMI encryption
  BSU08 (BLDCFG_DF_SAVE_RESTORE_MEM_ENCRYPT),     // < Save/Restore memory encryption
  BSU08 (BLDCFG_DF_SYS_STORAGE_AT_TOM),           // < Global CC6 enable
  BSU08 (BLDCFG_DF_PROBE_FILTER_ENABLE),          // < SPF
  BSU08 (BLDCFG_DF_BOTTOM_IO),                    // < Bottom IO
  BSU08 (BLDCFG_DF_MEM_INTERLEAVING),             // < Channel, Die, Socket interleaving control
  BSU08 (BLDCFG_DF_DRAM_INTLV_SIZE),              // < Address bit to interleave on
  BSU08 (BLDCFG_DF_CHAN_INTLV_HASH_EN),           // < Enable channel interleaving hash
  BSU32 (BLDCFG_DF_PCI_MMIO_SIZE),                // < Size in bytes of space used for PCI MMIO
  BSU32 (BLDCFG_DF_CAKE_CRC_THRESH_PERF_BOUNDS),  // < Percentage of performance to sacrifice for CAKE CRC in 0.00001% units
  BSU08 (BLDCFG_DF_MEM_CLEAR_DISABLE),            // < Enable/Disable MemClear

  BSU08 (0xFF)
};

HPCB_TYPE_DATA_END_SIGNATURE ();

int
main (
  IN int argc, IN char *argv[]
  )
{
  return 0;
}
