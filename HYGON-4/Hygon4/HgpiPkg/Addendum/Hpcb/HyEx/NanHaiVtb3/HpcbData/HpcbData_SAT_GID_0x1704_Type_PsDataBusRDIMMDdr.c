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

// RDIMM DATABUS DDR5
HPCB_TYPE_DATA_START_SIGNATURE ();
HPCB_TYPE_HEADER        HpcbTypeHeader = {
  HPCB_GROUP_MEMORY,
  HPCB_MEM_TYPE_PS_RDIMM_DDR5_DATA_BUS,
  (sizeof (HPCB_TYPE_HEADER) + sizeof (PSCFG_DATABUS_ENTRY_D5) * 30),
  0,
  0,
  0
};

PSCFG_DATABUS_ENTRY_D5 DataBusRdimmDdr5[] = {
// DimmPerCh  DDRrate  VDDIO  ManuFacturer   Dimm0   Dimm1  DeviceWidth    DqRttWr      DqRttNomRd        DqRttNomWr       DqVrefDram  PhyDrvStrenDqDqs PhyOdtStrenDqDqs RxDqVref     RttCkGa        RttCkGb      RttCsGa        RttCsGb      RttCaGa         RttCaGb       DqsRttPark              DqRttPark        VrefCs VrefCa
  {1,       ALL_SPEED, V1_1,  SAMSUNG,      DIMM_SR,   NP, DEVICEWIDTH_4, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {1,       ALL_SPEED, V1_1,  SAMSUNG,      DIMM_SR,   NP, DEVICEWIDTH_8, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {1,       ALL_SPEED, V1_1,  SAMSUNG,      DIMM_DR,   NP, DEVICEWIDTH_4, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {1,       ALL_SPEED, V1_1,  SAMSUNG,      DIMM_DR,   NP, DEVICEWIDTH_8, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {1,       ALL_SPEED, V1_1,  SKHYNIX,      DIMM_SR,   NP, DEVICEWIDTH_4, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {1,       ALL_SPEED, V1_1,  SKHYNIX,      DIMM_SR,   NP, DEVICEWIDTH_8, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x48,  0x48},
  {1,       ALL_SPEED, V1_1,  SKHYNIX,      DIMM_DR,   NP, DEVICEWIDTH_4, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {1,       ALL_SPEED, V1_1,  SKHYNIX,      DIMM_DR,   NP, DEVICEWIDTH_8, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  
  {2,       ALL_SPEED, V1_1,  SAMSUNG,      NP,   DIMM_SR, DEVICEWIDTH_4, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SAMSUNG,      NP,   DIMM_SR, DEVICEWIDTH_8, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SAMSUNG,      NP,   DIMM_DR, DEVICEWIDTH_4, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SAMSUNG,      NP,   DIMM_DR, DEVICEWIDTH_8, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SKHYNIX,      NP,   DIMM_SR, DEVICEWIDTH_4, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SKHYNIX,      NP,   DIMM_SR, DEVICEWIDTH_8, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x48,  0x48},
  {2,       ALL_SPEED, V1_1,  SKHYNIX,      NP,   DIMM_DR, DEVICEWIDTH_4, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SKHYNIX,      NP,   DIMM_DR, DEVICEWIDTH_8, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SAMSUNG,   DIMM_SR, DIMM_SR, DEVICEWIDTH_4, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SAMSUNG,   DIMM_SR, DIMM_SR, DEVICEWIDTH_8, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SAMSUNG,   DIMM_DR, DIMM_DR, DEVICEWIDTH_4, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SAMSUNG,   DIMM_DR, DIMM_DR, DEVICEWIDTH_8, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SKHYNIX,   DIMM_SR, DIMM_SR, DEVICEWIDTH_4, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SKHYNIX,   DIMM_SR, DIMM_SR, DEVICEWIDTH_8, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x48,  0x48},
  {2,       ALL_SPEED, V1_1,  SKHYNIX,   DIMM_DR, DIMM_DR, DEVICEWIDTH_4, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {2,       ALL_SPEED, V1_1,  SKHYNIX,   DIMM_DR, DIMM_DR, DEVICEWIDTH_8, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  ///default:if the appropriate configuration is not queried,the  following default configuration is used
  {0xFF,    ALL_SPEED, V1_1,     0xFF,      NP,   DIMM_SR, DEVICEWIDTH_4, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {0xFF,    ALL_SPEED, V1_1,     0xFF,      NP,   DIMM_SR, DEVICEWIDTH_8, DQ_RTT_WR_34, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x2a,      0x1F,           0x13,             0x17E,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_40,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x48,  0x48},
  {0xFF,    ALL_SPEED, V1_1,     0xFF,      NP,   DIMM_DR, DEVICEWIDTH_4, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},
  {0xFF,    ALL_SPEED, V1_1,     0xFF,      NP,   DIMM_DR, DEVICEWIDTH_8, DQ_RTT_WR_80, DQ_RTT_NOM_RD_80, DQ_RTT_NOM_WR_80,  0x21,      0x1F,           0x6,              0x1a4,    RTT_CK_OFF,    RTT_CK_40,    RTT_CS_OFF,    RTT_CS_40,   RTT_CA_OFF,    RTT_CA_60,    DQS_RTT_PARK_40,         DQ_RTT_PARK_80,     0x3a,  0x3a},

};

HPCB_TYPE_DATA_END_SIGNATURE ();

int
main (
  IN int argc, IN char *argv[]
  )
{
  return 0;
}
