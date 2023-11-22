/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch SATA controller
 *
 * Init SATA features.
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
#include "FchCommonCfg.h"
#include "Filecode.h"
#define FILECODE  PROC_FCH_TAISHAN_TSSATA_TSSATAENV_FILECODE

/**
 * FchInitEnvSata - Config SATA controller before PCI emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSata (
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;
  UINT32               DieBusNum;
  UINT8                PhysicalDieId;
  UINT8                LogicalDieId;
  UINT8                SataIndex;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  HGPI_TESTPOINT (TpFchInitEnvSata, NULL);

  if (CheckHygonSoc () == FALSE) {
    return;
  }

  PhysicalDieId = 0;
  LogicalDieId  = 0;
  for(SataIndex = 0; SataIndex < SATA_NUMBER_PER_DUJIANG; SataIndex++) {
    // check if Socket0 DJ0 Sata Enable?
    if ((LocalCfgPtr->Sata.SataMode.SataEnable & (UINT32)BIT (SataIndex)) == 0) {
      if (SataIndex == 0) {
        FchSmnRW (0, NBIO_SPACE2 (PhysicalDieId, 0, NBIF2_SATA_STRAP0_ADDRESS_HYGX), ~(UINT32)BIT28, 0, StdHeader);
      } else {
        FchSmnRW (0, NBIO_SPACE2 (PhysicalDieId, 0, NBIF3_SATA_STRAP0_ADDRESS_HYGX), ~(UINT32)BIT28, 0, StdHeader);
      }

      continue;
    }

    //
    // another FCH initialize at FchTSSecondaryFchInitSataDxe()
    //

    DieBusNum = ReadSocDieBusNum (0, LogicalDieId, 0);

    IdsLibDebugPrint (MAIN_FLOW, "SataEnableWriteAccessTS \n");
    SataEnableWriteAccessTS (PhysicalDieId, SataIndex, DieBusNum, LocalCfgPtr);

    IdsLibDebugPrint (MAIN_FLOW, "FchInitEnvProgramSata \n");
    FchInitEnvProgramSata (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);                    // Init Sata parameters

    //
    // Call Sub-function for each Sata mode
    //
    if ((LocalCfgPtr->Sata.SataClass == SataAhci7804) || (LocalCfgPtr->Sata.SataClass == SataAhci)) {
      IdsLibDebugPrint (MAIN_FLOW, "FchInitEnvSataAhciTS \n");
      FchInitEnvSataAhciTS (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);
    }

    if ( LocalCfgPtr->Sata.SataClass == SataRaid) {
      IdsLibDebugPrint (MAIN_FLOW, "FchInitEnvSataRaidTS \n");
      FchInitEnvSataRaidTS (PhysicalDieId, SataIndex, DieBusNum, FchDataPtr);
    }

    //
    // SATA IRQ Resource
    //
    IdsLibDebugPrint (MAIN_FLOW, "SataSetIrqIntResource \n");
    SataSetIrqIntResource (LocalCfgPtr, StdHeader);

    IdsLibDebugPrint (MAIN_FLOW, "SataDisableWriteAccessTS \n");
    SataDisableWriteAccessTS (PhysicalDieId, SataIndex, DieBusNum, LocalCfgPtr);
  }
}

