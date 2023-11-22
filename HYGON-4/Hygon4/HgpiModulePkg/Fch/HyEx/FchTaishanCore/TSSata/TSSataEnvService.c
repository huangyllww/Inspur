/* $NoKeywords:$ */

/**
 * @file
 *
 * SATA Controller family specific service procedure
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
* ***************************************************************************
*/

/*----------------------------------------------------------------------------------------
 *                             M O D U L E S    U S E D
 *----------------------------------------------------------------------------------------
 */
#include "FchPlatform.h"
#include "FchCommonCfg.h"
#include "Filecode.h"
#define FILECODE  PROC_FCH_TAISHAN_TSSATA_TSSATAENVSERVICE_FILECODE

SATA_PHY_SETTING  SataPhyTable[] =
{
  0x0030, 0x0040F407,
  0x0120, 0x00403204,
  0x0110, 0x00403103,

  0x0031, 0x0040F407,
  0x0121, 0x00403204,
  0x0111, 0x00403103,
};

/*----------------------------------------------------------------------------------------
 *                   D E F I N I T I O N S    A N D    M A C R O S
 *----------------------------------------------------------------------------------------
 */

/**
 * FchInitEnvProgramSataPciRegs - Sata Pci Configuration Space
 * register setting
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum -    Bus Number of current Die
 * @param[in] FchDataPtr -   Fch configuration structure pointer
 *
 */
VOID
FchInitEnvProgramSata (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                 PortNum;
  FCH_DATA_BLOCK         *LocalCfgPtr;
  HYGON_CONFIG_PARAMS    *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  //
  // All ports are put in offline mode in PEI. Enable ports here
  //
  for (PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++) {
    if (((LocalCfgPtr->Sata.SataPortPower.SataPortReg[SataIndex] >> PortNum) & BIT0) == 0) {
      FchTSSataInitPortActive (PhysicalDieId, SataIndex, DieBusNum, PortNum, FchDataPtr);
    }
  }

  // Do Sata init
  FchTSSataInitRsmuCtrl (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);

  FchTSSataInitCtrlReg (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);

  FchTSSataInitEsata (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);
  
  if(SataIndex == 0) {
    FchTSSataInitDevSlp (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);
  }

  FchTSSataInitMpssMap (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);

  FchTSSataInitEnableErr (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);

  FchTSSataSetPortGenMode (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);

  if (LocalCfgPtr->Sata.SataSgpio & BIT (SataIndex)) {
    FchTSSataGpioInitial (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);
  }
}

/**
 * FchInitEnvSataRaidProgram - Configuration SATA Raid
 * controller (No longer exist)
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSataRaidProgram (
  IN  VOID     *FchDataPtr
  )
{
}
