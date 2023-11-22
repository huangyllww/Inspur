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
  HPCB_MEM_TYPE_DIMM_INFO_SMBUS,
  (sizeof (HPCB_TYPE_HEADER) + sizeof (DIMM_INFO_SMBUS)),
  0,
  0,
  0
};                        

DIMM_INFO_SMBUS  DimmInfoSmbus[] = {
//Present, Socket, Channel, Dimm, SmbusAddress
  TRUE, 0, 0, 0, BLDCF_SPD_CH_A_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x01,
  TRUE, 0, 0, 1, BLDCF_SPD_CH_A_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x01,
  
  TRUE, 0, 1, 0, BLDCF_SPD_CH_B_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x01,
  TRUE, 0, 1, 1, BLDCF_SPD_CH_B_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x01,
  
  TRUE, 0, 2, 0, BLDCF_SPD_CH_C_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x01,
  TRUE, 0, 2, 1, BLDCF_SPD_CH_C_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x01,
  
  TRUE, 0, 3, 0, BLDCF_SPD_CH_D_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x01,
  TRUE, 0, 3, 1, BLDCF_SPD_CH_D_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x01,

  TRUE, 0, 4, 0, BLDCF_SPD_CH_E_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,
  TRUE, 0, 4, 1, BLDCF_SPD_CH_E_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,

  TRUE, 0, 5, 0, BLDCF_SPD_CH_F_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,
  TRUE, 0, 5, 1, BLDCF_SPD_CH_F_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,

  TRUE, 0, 6, 0, BLDCF_SPD_CH_G_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,
  TRUE, 0, 6, 1, BLDCF_SPD_CH_G_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,

  TRUE, 0, 7, 0, BLDCF_SPD_CH_H_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,
  TRUE, 0, 7, 1, BLDCF_SPD_CH_H_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,

  TRUE, 0, 8, 0, BLDCF_SPD_CH_I_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,
  TRUE, 0, 8, 1, BLDCF_SPD_CH_I_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,

  TRUE, 0, 9, 0, BLDCF_SPD_CH_J_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,
  TRUE, 0, 9, 1, BLDCF_SPD_CH_J_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,

  TRUE, 0, 10, 0, BLDCF_SPD_CH_K_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,
  TRUE, 0, 10, 1, BLDCF_SPD_CH_K_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,

  TRUE, 0, 11, 0, BLDCF_SPD_CH_L_DIMM0_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,
  TRUE, 0, 11, 1, BLDCF_SPD_CH_L_DIMM1_ADDRESS, BLDCFG_I2C_MUX_ADDRESS, 0xff, 0x02,
};

HPCB_TYPE_DATA_END_SIGNATURE ();

int
main (
  IN int argc, IN char *argv[]
  )
{
  return 0;
}
