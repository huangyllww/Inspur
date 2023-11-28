
/* $NoKeywords:$ */
/**
 * @file
 *
 * @e \$Revision:$   @e \$Date:$
 */
/*****************************************************************************
 *
 * 
 * Copyright 2016 - 2019 CHENGDU HAIGUANG IC DESIGN CO., LTD. All Rights Reserved.
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
 * AMD GRANT HYGON DECLARATION: ADVANCED MICRO DEVICES, INC.(AMD) granted HYGON has
 * the right to redistribute HYGON's Agesa version to BIOS Vendors and HYGON has
 * the right to make the modified version available for use with HYGON's PRODUCT.
 *
 ***************************************************************************/

#include "MyPorting.h"
#include "ApcbV2.h"

#include <MiscMemDefines.h>
#include <APCB.h>
#include <ApcbCustomizedDefinitions.h>
#include <ApcbDefaults.h>


CHAR8 mDummyBuf;
//RDIMM CADBUS DDR4
APCB_TYPE_DATA_START_SIGNATURE();
//APCB_TYPE_HEADER  ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_LRDIMM_DDR4_CAD_BUS, (sizeof(APCB_TYPE_HEADER) + sizeof (PSCFG_CADBUS_ENTRY) * 30), 0, 0, 0 };  // SizeOfType will be fixed up by tool
APCB_TYPE_HEADER  ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_LRDIMM_DDR4_CAD_BUS, (sizeof(APCB_TYPE_HEADER)), 0, 0, 0 };  // SizeOfType will be fixed up by tool
/*
P8SCFG_CADBUS_ENTRY CadBusLRDimmDdr4[] = {
  {1, DDR1333, V1_2, DIMM_LR,      NP, 0, 0, 0x003A3A3A, 0x07, 0x07, 0x07, 0x07},
  {1, DDR1600, V1_2, DIMM_LR,      NP, 0, 0, 0x00393939, 0x07, 0x07, 0x07, 0x07},
  {1, DDR1866, V1_2, DIMM_LR,      NP, 0, 0, 0x00373737, 0x07, 0x07, 0x07, 0x07},
  {1, DDR2133, V1_2, DIMM_LR,      NP, 0, 0, 0x00353535, 0x07, 0x07, 0x07, 0x07},
  {1, DDR2400, V1_2, DIMM_LR,      NP, 0, 0, 0x00333333, 0x07, 0x07, 0x07, 0x07},
  {1, DDR2667, V1_2, DIMM_LR,      NP, 0, 0, 0x00313131, 0x07, 0x07, 0x07, 0x07},
  {1, DDR2933, V1_2, DIMM_LR,      NP, 0, 0, 0x002F2F2F, 0x07, 0x07, 0x07, 0x07},
  {1, DDR3200, V1_2, DIMM_LR,      NP, 0, 0, 0x002D2D2D, 0x07, 0x07, 0x07, 0x07},

  {2, DDR1333, V1_2, NP,      DIMM_LR, 0, 0, 0x003A3A3A, 0x07, 0x07, 0x07, 0x07},
  {2, DDR1333, V1_2, DIMM_LR, DIMM_LR, 0, 0, 0x00383A3A, 0x07, 0x07, 0x07, 0x07},
  {2, DDR1600, V1_2, NP,      DIMM_LR, 0, 0, 0x00393939, 0x07, 0x07, 0x07, 0x07},
  {2, DDR1600, V1_2, DIMM_LR, DIMM_LR, 0, 0, 0x00353939, 0x07, 0x07, 0x07, 0x07},
  {2, DDR1866, V1_2, NP,      DIMM_LR, 0, 0, 0x00373737, 0x07, 0x07, 0x07, 0x07},
  {2, DDR1866, V1_2, DIMM_LR, DIMM_LR, 0, 0, 0x00333737, 0x07, 0x07, 0x07, 0x07},
  {2, DDR2133, V1_2, NP,      DIMM_LR, 0, 0, 0x00353535, 0x07, 0x07, 0x07, 0x07},
  {2, DDR2133, V1_2, DIMM_LR, DIMM_LR, 0, 0, 0x00313535, 0x07, 0x07, 0x07, 0x07},
  {2, DDR2400, V1_2, NP,      DIMM_LR, 0, 0, 0x00333333, 0x07, 0x07, 0x07, 0x07},
  {2, DDR2400, V1_2, DIMM_LR, DIMM_LR, 0, 0, 0x002F3333, 0x07, 0x07, 0x07, 0x07},
  {2, DDR2667, V1_2, NP,      DIMM_LR, 0, 0, 0x00313131, 0x07, 0x07, 0x07, 0x07},
  {2, DDR2933, V1_2, NP,      DIMM_LR, 0, 0, 0x002F2F2F, 0x07, 0x07, 0x07, 0x07},
};
*/
APCB_TYPE_DATA_END_SIGNATURE();


int main (IN int argc, IN char * argv [ ])
{
  return 0;
}



