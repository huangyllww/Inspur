
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

APCB_TYPE_DATA_START_SIGNATURE();
APCB_TYPE_HEADER       ApcbTypeHeader = {
  APCB_GROUP_DF,
  APCB_DF_TYPE_XGMI_TX_EQ,
  (
    sizeof (APCB_TYPE_HEADER)
  ),
  1, // Instance 1
  0,
  0
};  // SizeOfType will be fixed up by tool

UINT32 xGmiTxEqSetting[] = {

  MAKE_TX_EQ_FREQ_TABLE(
    (XGMI_TX_EQ_SPEED_ALL & ~XGMI_TX_EQ_SPEED_128),
    MAKE_TX_EQ_PAIR(MAKE_TX_EQ_ALL_SYS_LANES(), MAKE_TX_EQ_DATA(40,  0, 0))
    ),

#if AGT_DATA_FILE_INSTANCE1_EXIST
  // Auto-generated file combining AGT generated XGMI TXEQ files under AGT_Data\Instance1 folder
  #include "AGT_xGMITxEq_Inst_1.h"
#else
  //
  // Setting for each lane for each speeds
  //
  MAKE_TX_EQ_FREQ_TABLE_COMPACT(XGMI_TX_EQ_SPEED_85,
    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE2_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(38, 8, 0), MAKE_TX_EQ_LANE_DATA(37, 12, 0),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(36, 16, 0), MAKE_TX_EQ_LANE_DATA(33, 16, 12), MAKE_TX_EQ_LANE_DATA(35, 12, 8),
                            MAKE_TX_EQ_LANE_DATA(38, 8, 0), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(35, 12, 8), MAKE_TX_EQ_LANE_DATA(38, 8, 0),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(37, 4, 8), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(37, 4, 8)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE3_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(35, 20, 0), MAKE_TX_EQ_LANE_DATA(35, 20, 0), MAKE_TX_EQ_LANE_DATA(35, 8, 12), MAKE_TX_EQ_LANE_DATA(36, 8, 8),
                            MAKE_TX_EQ_LANE_DATA(35, 16, 4), MAKE_TX_EQ_LANE_DATA(38, 0, 8), MAKE_TX_EQ_LANE_DATA(38, 8, 0), MAKE_TX_EQ_LANE_DATA(37, 12, 0),
                            MAKE_TX_EQ_LANE_DATA(38, 8, 0), MAKE_TX_EQ_LANE_DATA(38, 4, 4), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(39, 4, 0),
                            MAKE_TX_EQ_LANE_DATA(37, 4, 8), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(38, 0, 8), MAKE_TX_EQ_LANE_DATA(34, 16, 8)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE0_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(38, 8, 0), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(35, 20, 0), MAKE_TX_EQ_LANE_DATA(36, 16, 0),
                            MAKE_TX_EQ_LANE_DATA(35, 20, 0), MAKE_TX_EQ_LANE_DATA(38, 4, 4), MAKE_TX_EQ_LANE_DATA(36, 8, 8), MAKE_TX_EQ_LANE_DATA(36, 12, 4),
                            MAKE_TX_EQ_LANE_DATA(34, 16, 8), MAKE_TX_EQ_LANE_DATA(38, 4, 4), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(39, 4, 0),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(37, 12, 0)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE1_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(34, 24, 0), MAKE_TX_EQ_LANE_DATA(31, 36, 0),
                            MAKE_TX_EQ_LANE_DATA(36, 16, 0), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(37, 0, 12), MAKE_TX_EQ_LANE_DATA(37, 12, 0),
                            MAKE_TX_EQ_LANE_DATA(38, 8, 0), MAKE_TX_EQ_LANE_DATA(37, 8, 4), MAKE_TX_EQ_LANE_DATA(37, 4, 8), MAKE_TX_EQ_LANE_DATA(38, 0, 8),
                            MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(37, 8, 4)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE2_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(33, 20, 8), MAKE_TX_EQ_LANE_DATA(34, 24, 0),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(33, 8, 20), MAKE_TX_EQ_LANE_DATA(31, 16, 20), MAKE_TX_EQ_LANE_DATA(38, 0, 8),
                            MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(40, 0, 0),
                            MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(35, 8, 12), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(37, 0, 12)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE3_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(31, 24, 12), MAKE_TX_EQ_LANE_DATA(32, 24, 8), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(36, 8, 8),
                            MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(37, 8, 4),
                            MAKE_TX_EQ_LANE_DATA(37, 12, 0), MAKE_TX_EQ_LANE_DATA(37, 8, 4), MAKE_TX_EQ_LANE_DATA(37, 12, 0), MAKE_TX_EQ_LANE_DATA(38, 8, 0),
                            MAKE_TX_EQ_LANE_DATA(37, 8, 4), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(33, 16, 12), MAKE_TX_EQ_LANE_DATA(34, 16, 8)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE0_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(40, 0, 0),
                            MAKE_TX_EQ_LANE_DATA(32, 24, 8), MAKE_TX_EQ_LANE_DATA(34, 20, 4), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(39, 0, 4),
                            MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(36, 16, 0), MAKE_TX_EQ_LANE_DATA(40, 0, 0),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(33, 24, 4), MAKE_TX_EQ_LANE_DATA(37, 0, 12)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE1_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(36, 0, 16), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(33, 24, 4), MAKE_TX_EQ_LANE_DATA(38, 4, 4),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(35, 16, 4), MAKE_TX_EQ_LANE_DATA(38, 4, 4), MAKE_TX_EQ_LANE_DATA(31, 24, 12),
                            MAKE_TX_EQ_LANE_DATA(38, 0, 8), MAKE_TX_EQ_LANE_DATA(38, 0, 8), MAKE_TX_EQ_LANE_DATA(37, 12, 0), MAKE_TX_EQ_LANE_DATA(37, 0, 12),
                            MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(37, 8, 4), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(37, 4, 8))
    ),

  MAKE_TX_EQ_FREQ_TABLE_COMPACT(XGMI_TX_EQ_SPEED_96,
    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE2_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(27, 2, 0), MAKE_TX_EQ_LANE_DATA(24, 8, 5), MAKE_TX_EQ_LANE_DATA(26, 0, 5), MAKE_TX_EQ_LANE_DATA(23, 8, 8),
                            MAKE_TX_EQ_LANE_DATA(23, 16, 0), MAKE_TX_EQ_LANE_DATA(23, 14, 2), MAKE_TX_EQ_LANE_DATA(23, 19, 0), MAKE_TX_EQ_LANE_DATA(23, 16, 0),
                            MAKE_TX_EQ_LANE_DATA(27, 2, 0), MAKE_TX_EQ_LANE_DATA(25, 8, 2), MAKE_TX_EQ_LANE_DATA(28, 0, 0), MAKE_TX_EQ_LANE_DATA(26, 2, 2),
                            MAKE_TX_EQ_LANE_DATA(28, 0, 0), MAKE_TX_EQ_LANE_DATA(25, 2, 5), MAKE_TX_EQ_LANE_DATA(27, 0, 2), MAKE_TX_EQ_LANE_DATA(25, 8, 2)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE3_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(33, 16, 12), MAKE_TX_EQ_LANE_DATA(35, 12, 8), MAKE_TX_EQ_LANE_DATA(34, 24, 0), MAKE_TX_EQ_LANE_DATA(31, 32, 4),
                            MAKE_TX_EQ_LANE_DATA(31, 24, 12), MAKE_TX_EQ_LANE_DATA(34, 16, 8), MAKE_TX_EQ_LANE_DATA(33, 28, 0), MAKE_TX_EQ_LANE_DATA(34, 16, 8),
                            MAKE_TX_EQ_LANE_DATA(37, 12, 0), MAKE_TX_EQ_LANE_DATA(36, 12, 4), MAKE_TX_EQ_LANE_DATA(31, 24, 12), MAKE_TX_EQ_LANE_DATA(37, 8, 4),
                            MAKE_TX_EQ_LANE_DATA(38, 0, 8), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(32, 16, 16), MAKE_TX_EQ_LANE_DATA(30, 24, 16)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE0_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(37, 8, 4), MAKE_TX_EQ_LANE_DATA(34, 16, 8), MAKE_TX_EQ_LANE_DATA(33, 20, 8), MAKE_TX_EQ_LANE_DATA(33, 20, 8),
                            MAKE_TX_EQ_LANE_DATA(35, 20, 0), MAKE_TX_EQ_LANE_DATA(35, 20, 0), MAKE_TX_EQ_LANE_DATA(35, 20, 0), MAKE_TX_EQ_LANE_DATA(36, 16, 0),
                            MAKE_TX_EQ_LANE_DATA(34, 20, 4), MAKE_TX_EQ_LANE_DATA(38, 0, 8), MAKE_TX_EQ_LANE_DATA(37, 8, 4), MAKE_TX_EQ_LANE_DATA(39, 0, 4),
                            MAKE_TX_EQ_LANE_DATA(38, 8, 0), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(34, 4, 20), MAKE_TX_EQ_LANE_DATA(39, 4, 0)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE1_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(25, 5, 2), MAKE_TX_EQ_LANE_DATA(23, 14, 5), MAKE_TX_EQ_LANE_DATA(21, 19, 5), MAKE_TX_EQ_LANE_DATA(21, 19, 8),
                            MAKE_TX_EQ_LANE_DATA(23, 14, 5), MAKE_TX_EQ_LANE_DATA(23, 11, 5), MAKE_TX_EQ_LANE_DATA(22, 16, 5), MAKE_TX_EQ_LANE_DATA(27, 2, 0),
                            MAKE_TX_EQ_LANE_DATA(26, 5, 0), MAKE_TX_EQ_LANE_DATA(26, 5, 0), MAKE_TX_EQ_LANE_DATA(27, 0, 2), MAKE_TX_EQ_LANE_DATA(25, 8, 2),
                            MAKE_TX_EQ_LANE_DATA(26, 5, 0), MAKE_TX_EQ_LANE_DATA(27, 2, 0), MAKE_TX_EQ_LANE_DATA(25, 5, 2), MAKE_TX_EQ_LANE_DATA(26, 2, 2)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE2_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(25, 2, 5), MAKE_TX_EQ_LANE_DATA(24, 8, 5), MAKE_TX_EQ_LANE_DATA(23, 8, 8), MAKE_TX_EQ_LANE_DATA(23, 8, 11),
                            MAKE_TX_EQ_LANE_DATA(23, 8, 8), MAKE_TX_EQ_LANE_DATA(24, 5, 8), MAKE_TX_EQ_LANE_DATA(23, 8, 11), MAKE_TX_EQ_LANE_DATA(27, 2, 0),
                            MAKE_TX_EQ_LANE_DATA(28, 0, 0), MAKE_TX_EQ_LANE_DATA(28, 0, 0), MAKE_TX_EQ_LANE_DATA(26, 5, 0), MAKE_TX_EQ_LANE_DATA(26, 0, 5),
                            MAKE_TX_EQ_LANE_DATA(28, 0, 0), MAKE_TX_EQ_LANE_DATA(23, 8, 11), MAKE_TX_EQ_LANE_DATA(28, 0, 0), MAKE_TX_EQ_LANE_DATA(27, 0, 2)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE3_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(31, 24, 12), MAKE_TX_EQ_LANE_DATA(34, 12, 12), MAKE_TX_EQ_LANE_DATA(35, 8, 12), MAKE_TX_EQ_LANE_DATA(35, 12, 8),
                            MAKE_TX_EQ_LANE_DATA(31, 28, 8), MAKE_TX_EQ_LANE_DATA(35, 4, 16), MAKE_TX_EQ_LANE_DATA(33, 24, 4), MAKE_TX_EQ_LANE_DATA(31, 24, 12),
                            MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(30, 28, 12), MAKE_TX_EQ_LANE_DATA(31, 24, 12), MAKE_TX_EQ_LANE_DATA(38, 8, 0),
                            MAKE_TX_EQ_LANE_DATA(37, 8, 4), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(35, 20, 0), MAKE_TX_EQ_LANE_DATA(33, 16, 12)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE0_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(40, 0, 0),
                            MAKE_TX_EQ_LANE_DATA(36, 8, 8), MAKE_TX_EQ_LANE_DATA(34, 16, 8), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(36, 0, 16),
                            MAKE_TX_EQ_LANE_DATA(34, 8, 16), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(37, 8, 4), MAKE_TX_EQ_LANE_DATA(35, 12, 8),
                            MAKE_TX_EQ_LANE_DATA(38, 8, 0), MAKE_TX_EQ_LANE_DATA(39, 4, 0), MAKE_TX_EQ_LANE_DATA(34, 12, 12), MAKE_TX_EQ_LANE_DATA(40, 0, 0)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE1_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(24, 5, 8), MAKE_TX_EQ_LANE_DATA(27, 2, 0), MAKE_TX_EQ_LANE_DATA(22, 14, 8), MAKE_TX_EQ_LANE_DATA(25, 8, 0),
                            MAKE_TX_EQ_LANE_DATA(26, 0, 5), MAKE_TX_EQ_LANE_DATA(24, 5, 8), MAKE_TX_EQ_LANE_DATA(23, 14, 2), MAKE_TX_EQ_LANE_DATA(23, 8, 11),
                            MAKE_TX_EQ_LANE_DATA(27, 2, 0), MAKE_TX_EQ_LANE_DATA(28, 0, 0), MAKE_TX_EQ_LANE_DATA(26, 2, 2), MAKE_TX_EQ_LANE_DATA(26, 0, 5),
                            MAKE_TX_EQ_LANE_DATA(27, 0, 2), MAKE_TX_EQ_LANE_DATA(28, 0, 0), MAKE_TX_EQ_LANE_DATA(28, 0, 0), MAKE_TX_EQ_LANE_DATA(27, 2, 0))
    ),

  MAKE_TX_EQ_FREQ_TABLE_COMPACT(XGMI_TX_EQ_SPEED_107,
    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE2_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(25, 11, 0), MAKE_TX_EQ_LANE_DATA(22, 14, 8), MAKE_TX_EQ_LANE_DATA(23, 5, 11), MAKE_TX_EQ_LANE_DATA(23, 8, 8),
                            MAKE_TX_EQ_LANE_DATA(22, 11, 11), MAKE_TX_EQ_LANE_DATA(25, 5, 5), MAKE_TX_EQ_LANE_DATA(21, 14, 11), MAKE_TX_EQ_LANE_DATA(23, 14, 2),
                            MAKE_TX_EQ_LANE_DATA(21, 11, 14), MAKE_TX_EQ_LANE_DATA(23, 11, 5), MAKE_TX_EQ_LANE_DATA(25, 8, 2), MAKE_TX_EQ_LANE_DATA(25, 5, 2),
                            MAKE_TX_EQ_LANE_DATA(26, 2, 2), MAKE_TX_EQ_LANE_DATA(21, 19, 8), MAKE_TX_EQ_LANE_DATA(25, 0, 8), MAKE_TX_EQ_LANE_DATA(25, 2, 8)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE3_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(34, 16, 8), MAKE_TX_EQ_LANE_DATA(33, 20, 8), MAKE_TX_EQ_LANE_DATA(32, 24, 8), MAKE_TX_EQ_LANE_DATA(31, 28, 8),
                            MAKE_TX_EQ_LANE_DATA(31, 28, 8), MAKE_TX_EQ_LANE_DATA(33, 12, 16), MAKE_TX_EQ_LANE_DATA(31, 20, 16), MAKE_TX_EQ_LANE_DATA(33, 20, 8),
                            MAKE_TX_EQ_LANE_DATA(38, 0, 8), MAKE_TX_EQ_LANE_DATA(33, 28, 0), MAKE_TX_EQ_LANE_DATA(29, 32, 12), MAKE_TX_EQ_LANE_DATA(39, 4, 0),
                            MAKE_TX_EQ_LANE_DATA(33, 16, 12), MAKE_TX_EQ_LANE_DATA(34, 8, 16), MAKE_TX_EQ_LANE_DATA(32, 24, 8), MAKE_TX_EQ_LANE_DATA(37, 12, 0)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE0_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(32, 20, 12), MAKE_TX_EQ_LANE_DATA(31, 20, 16), MAKE_TX_EQ_LANE_DATA(32, 16, 16),
                            MAKE_TX_EQ_LANE_DATA(33, 16, 12), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(33, 12, 16), MAKE_TX_EQ_LANE_DATA(36, 16, 0),
                            MAKE_TX_EQ_LANE_DATA(35, 12, 8), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(37, 4, 8),
                            MAKE_TX_EQ_LANE_DATA(37, 8, 4), MAKE_TX_EQ_LANE_DATA(32, 24, 8), MAKE_TX_EQ_LANE_DATA(31, 28, 8), MAKE_TX_EQ_LANE_DATA(31, 32, 4)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET1_DIE1_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(23, 5, 14), MAKE_TX_EQ_LANE_DATA(25, 0, 11), MAKE_TX_EQ_LANE_DATA(21, 19, 8), MAKE_TX_EQ_LANE_DATA(19, 28, 5),
                            MAKE_TX_EQ_LANE_DATA(22, 16, 5), MAKE_TX_EQ_LANE_DATA(22, 14, 8), MAKE_TX_EQ_LANE_DATA(23, 11, 5), MAKE_TX_EQ_LANE_DATA(22, 11, 11),
                            MAKE_TX_EQ_LANE_DATA(24, 14, 0), MAKE_TX_EQ_LANE_DATA(25, 11, 0), MAKE_TX_EQ_LANE_DATA(25, 5, 2), MAKE_TX_EQ_LANE_DATA(25, 0, 8),
                            MAKE_TX_EQ_LANE_DATA(26, 2, 2), MAKE_TX_EQ_LANE_DATA(26, 2, 2), MAKE_TX_EQ_LANE_DATA(24, 8, 5), MAKE_TX_EQ_LANE_DATA(27, 2, 0)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE2_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(25, 5, 5), MAKE_TX_EQ_LANE_DATA(25, 0, 11), MAKE_TX_EQ_LANE_DATA(21, 5, 19), MAKE_TX_EQ_LANE_DATA(23, 5, 14),
                            MAKE_TX_EQ_LANE_DATA(23, 5, 14), MAKE_TX_EQ_LANE_DATA(23, 5, 11), MAKE_TX_EQ_LANE_DATA(23, 2, 14), MAKE_TX_EQ_LANE_DATA(28, 0, 0),
                            MAKE_TX_EQ_LANE_DATA(23, 8, 8), MAKE_TX_EQ_LANE_DATA(26, 0, 5), MAKE_TX_EQ_LANE_DATA(25, 8, 0), MAKE_TX_EQ_LANE_DATA(28, 0, 0),
                            MAKE_TX_EQ_LANE_DATA(26, 0, 5), MAKE_TX_EQ_LANE_DATA(22, 11, 11), MAKE_TX_EQ_LANE_DATA(23, 5, 11), MAKE_TX_EQ_LANE_DATA(28, 0, 0)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE3_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(33, 20, 8), MAKE_TX_EQ_LANE_DATA(31, 24, 12), MAKE_TX_EQ_LANE_DATA(32, 28, 4), MAKE_TX_EQ_LANE_DATA(30, 16, 24),
                            MAKE_TX_EQ_LANE_DATA(35, 4, 16), MAKE_TX_EQ_LANE_DATA(38, 4, 4), MAKE_TX_EQ_LANE_DATA(38, 8, 0), MAKE_TX_EQ_LANE_DATA(30, 20, 20),
                            MAKE_TX_EQ_LANE_DATA(36, 4, 12), MAKE_TX_EQ_LANE_DATA(30, 20, 20), MAKE_TX_EQ_LANE_DATA(31, 20, 16), MAKE_TX_EQ_LANE_DATA(34, 24, 0),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(35, 0, 20), MAKE_TX_EQ_LANE_DATA(32, 20, 12), MAKE_TX_EQ_LANE_DATA(32, 16, 16)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE0_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(35, 8, 12), MAKE_TX_EQ_LANE_DATA(35, 8, 12), MAKE_TX_EQ_LANE_DATA(38, 0, 8), MAKE_TX_EQ_LANE_DATA(33, 4, 24),
                            MAKE_TX_EQ_LANE_DATA(35, 8, 12), MAKE_TX_EQ_LANE_DATA(32, 16, 16), MAKE_TX_EQ_LANE_DATA(38, 0, 8), MAKE_TX_EQ_LANE_DATA(39, 0, 4),
                            MAKE_TX_EQ_LANE_DATA(33, 12, 16), MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(39, 0, 4), MAKE_TX_EQ_LANE_DATA(30, 24, 16),
                            MAKE_TX_EQ_LANE_DATA(40, 0, 0), MAKE_TX_EQ_LANE_DATA(33, 12, 16), MAKE_TX_EQ_LANE_DATA(33, 20, 8), MAKE_TX_EQ_LANE_DATA(33, 16, 12)),

    MAKE_TX_EQ_LIST_COMPACT(MAKE_TX_EQ_SOCKET0_DIE1_EACH_LANES_COMPACT(),
                            MAKE_TX_EQ_LANE_DATA(23, 5, 14), MAKE_TX_EQ_LANE_DATA(24, 0, 14), MAKE_TX_EQ_LANE_DATA(21, 8, 16), MAKE_TX_EQ_LANE_DATA(23, 11, 8),
                            MAKE_TX_EQ_LANE_DATA(26, 0, 5), MAKE_TX_EQ_LANE_DATA(23, 14, 2), MAKE_TX_EQ_LANE_DATA(23, 11, 5), MAKE_TX_EQ_LANE_DATA(23, 8, 8),
                            MAKE_TX_EQ_LANE_DATA(26, 5, 0), MAKE_TX_EQ_LANE_DATA(25, 2, 5), MAKE_TX_EQ_LANE_DATA(27, 0, 2), MAKE_TX_EQ_LANE_DATA(25, 0, 8),
                            MAKE_TX_EQ_LANE_DATA(25, 2, 8), MAKE_TX_EQ_LANE_DATA(25, 0, 8), MAKE_TX_EQ_LANE_DATA(25, 0, 8), MAKE_TX_EQ_LANE_DATA(26, 0, 5))
    ),


#endif

  // Termination
  MAKE_TX_EQ_END()
};


APCB_TYPE_DATA_END_SIGNATURE();


int main(IN int argc, IN char * argv[])
{

  return sizeof(xGmiTxEqSetting);
}



