
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
// UDIMM ODTPAT DDR4
APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_UDIMM_DDR4_ODT_PAT, (sizeof(APCB_TYPE_HEADER) + sizeof (PSCFG_2D_ODTPAT_ENTRY) * 5), 0, 0, 0 };  // SizeOfType will be fixed up by tool

PSCFG_2D_ODTPAT_ENTRY OdtPatEntry [] = {
	{ NP,      DIMM_SR, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
	{ NP,      DIMM_DR, 0x00000000, 0x00000000, 0x00000000, 0x00000000 },
	{ DIMM_SR, NP,      0x00000000, 0x00000000, 0x00000000, 0x00000000 },
	{ DIMM_DR, NP,      0x00000000, 0x00000000, 0x00000000, 0x00000000 },
	{ DIMM_SR, DIMM_SR, 0x00000202, 0x00000000, 0x00000101, 0x00000000 },
	{ DIMM_SR, DIMM_DR, 0x00000202, 0x00000000, 0x00000101, 0x00000101 },
	{ DIMM_DR, DIMM_SR, 0x00000202, 0x00000202, 0x00000101, 0x00000000 },
	{ DIMM_DR, DIMM_DR, 0x00000202, 0x00000202, 0x00000101, 0x00000101 },
};

APCB_TYPE_DATA_END_SIGNATURE();
#if 0
//UDIMM CADBUS DDR4
APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_UDIMM_DDR4_CAD_BUS_PAT, sizeof(APCB_TYPE_HEADER) + 30, 0x0000 };  // SizeOfType will be fixed up by tool

PSCFG_CADBUS_ENTRY CadBusUDimmDdr4[] = {
	{1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, NP, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F},
	{ 1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 0, 0x00360000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 1, DDR1866, V1_35 + V1_5        , DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 1, DDR1866, V1_35 + V1_5        , DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 1, DDR2133, V1_5                , DIMM_SR, NP     , 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 1, DDR2133, V1_5                , DIMM_DR, NP     , 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 1, DDR2400, V1_5                , DIMM_SR     , NP, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 1, DDR2400, V1_5                , DIMM_DR     , NP, 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 0, 0x00360000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, 1, 0x00000035, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, 0, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, 1, 0x00000000, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_SR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_DR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_SR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_DR, 1, 0x0000002B, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1866, V1_35 + V1_5        , NP     , DIMM_SR, 0, 0x003C3C3C, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 2, DDR1866, V1_35 + V1_5        , NP     , DIMM_DR, 1, 0x00003C3C, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 2, DDR1866, V1_5                , DIMM_SR, DIMM_SR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1866, V1_5                , DIMM_DR, DIMM_DR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1866, V1_5                , DIMM_DR, DIMM_SR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR1866, V1_5                , DIMM_SR, DIMM_DR, 1, 0x00000031, 0x1F, 0x1F, 0x3F, 0x1F },
	{ 2, DDR2133, V1_5                , NP     , DIMM_SR, 0, 0x003B3B3B, 0x1F, 0x1F, 0x1F, 0x1F },
	{ 2, DDR2133, V1_5                , NP     , DIMM_DR, 1, 0x00003B3B, 0x1F, 0x1F, 0x1F, 0x1F },
};

APCB_TYPE_DATA_END_SIGNATURE();

//UDIMM DATABUS DDR4
APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_UDIMM_DDR4_DATA_BUS_PAT, sizeof(APCB_TYPE_HEADER) + 30, 0x0000 };  // SizeOfType will be fixed up by tool
PSCFG_DATABUS_ENTRY DataBusUdimmDdr4[] = {
	{1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, NP, ODT_60, ODT_OFF, 0x70, 0x70, 0x05},
	{ 1, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x05 },
	{ 1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_SR, NP     , ODT_60 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 1, DDR1600, V1_25 + V1_35 + V1_5, DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 1, DDR2133, V1_5                , DIMM_SR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 1, DDR2133, V1_5                , DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 1, DDR2400, V1_5                , DIMM_SR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 1, DDR2400, V1_5                , DIMM_DR, NP     , ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_60 , ODT_OFF, 0x70, 0x70, 0x05 },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_60 , ODT_OFF, 0x70, 0x70, 0x05 },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_SR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_DR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_DR, DIMM_SR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C },
	{ 2, DDR1333, V1_25 + V1_35 + V1_5, DIMM_SR, DIMM_DR, ODT_30 , ODT_120, 0x75, 0x75, 0x0C },
	{ 2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_SR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 2, DDR1600, V1_25 + V1_35 + V1_5, NP     , DIMM_DR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C },
	{ 2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C },
	{ 2, DDR1600, V1_35 + V1_5        , DIMM_DR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C },
	{ 2, DDR1600, V1_35 + V1_5        , DIMM_SR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C },
	{ 2, DDR1866, V1_35 + V1_5        , NP     , DIMM_SR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 2, DDR1866, V1_35 + V1_5        , NP     , DIMM_DR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 2, DDR1866, V1_5                , DIMM_SR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C },
	{ 2, DDR1866, V1_5                , DIMM_DR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C },
	{ 2, DDR1866, V1_5                , DIMM_DR, DIMM_SR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C },
	{ 2, DDR1866, V1_5                , DIMM_SR, DIMM_DR, ODT_20 , ODT_60 , 0x75, 0x75, 0x0C },
	{ 2, DDR2133, V1_5                , NP     , DIMM_SR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
	{ 2, DDR2133, V1_5                , NP     , DIMM_DR, ODT_40 , ODT_OFF, 0x70, 0x70, 0x0C },
};

APCB_TYPE_DATA_END_SIGNATURE();

//UDIMM MAX FREQ DDR4
APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = { APCB_GROUP_MEMORY, APCB_MEM_TYPE_PS_UDIMM_DDR4_MAX_FREQ_, sizeof(APCB_TYPE_HEADER) + 7, 0x0000 };  // SizeOfType will be fixed up by tool
PSCFG_MAXFREQ_ENTRY MaxFeqUDIMMDdr4 [] = {
	// Max Freq.
	// Format :
	// DimmPerCh,   Dimms,   SR,   DR,   QR,   Speed1_2V,   Speed_TBD1,   Speed_TBD2
	//
	{_1DIMM, 1, 1, 0, 0, DDR2933_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY},
	{ _1DIMM, 1, 0, 1, 0, DDR2667_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY },
	{ _2DIMM, 1, 1, 0, 0, DDR2667_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY },
	{ _2DIMM, 1, 0, 1, 0, DDR2400_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY },
	{ _2DIMM, 2, 2, 0, 0, DDR2133_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY },
	{ _2DIMM, 2, 1, 1, 0, DDR1866_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY },
	{ _2DIMM, 2, 0, 2, 0, DDR1866_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY, UNSUPPORTED_DDR_FREQUENCY }
};

APCB_TYPE_DATA_END_SIGNATURE();
#endif

int main (IN int argc, IN char * argv [ ])
{
  return 0;
}



