/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch Sata controller
 *
 * Init Sata Controller features (PEI phase).
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
#define FILECODE  PROC_FCH_TAISHAN_TSSATA_TSSATARESETSERVICE_FILECODE

/**
 * FchInitResetSataProgram - Config Sata controller during
 * Power-On
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] SataIndex      Sata index 0 or 1.
 * @param[in] DieBusNum      IOHC bus number on current Die.
 * @param[in] FchDataPtr     Fch configuration structure pointer.
 *
 */
VOID
FchInitResetSataProgram (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32                PortNum;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  //
  // staggered spin-up step 2 Optional
  //
  for (PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++) {
    FchSmnRW (
      DieBusNum,
      SATA_SPACE_HYEX (SataIndex, FCH_TS_SATA_AOAC_CONTROL_HYEX),
      ~(UINT32)(BIT0 << PortNum),
      0x00,
      LocalCfgPtr->StdHeader
      );
  }

  //
  // Enable the SATA controller.
  //
  if (LocalCfgPtr->FchReset.SataEnable & BIT(SataIndex)) {
    IdsLibDebugPrint (MAIN_FLOW, "Enable SATA controller \n");
    FchSmnRW (DieBusNum, SATA_SPACE_HYEX(SataIndex, FCH_TS_SATA_MISC_CONTROL_HYEX), 0xfffffffe, 0, LocalCfgPtr->StdHeader);
  } else {
    IdsLibDebugPrint (MAIN_FLOW, "Disable SATA controller \n");
    FchSmnRW (DieBusNum, SATA_SPACE_HYEX(SataIndex, FCH_TS_SATA_MISC_CONTROL_HYEX), 0xfffffffe, BIT0, LocalCfgPtr->StdHeader);
    FchSmnRW (DieBusNum, SATA_SPACE_HYEX(SataIndex, FCH_TS_SATA_MISC_CONTROL_HYEX), ~(UINT32) BIT11, BIT11, LocalCfgPtr->StdHeader);
    return;
  }

  //
  // Put all ports to offline mode
  //
  for (PortNum = 0; PortNum < TAISHAN_SATA_PORT_NUM; PortNum++) {
    IdsLibDebugPrint(MAIN_FLOW, "Put Rb %d SATA port %d to offline mode \n", SataIndex, PortNum);
    FchTSSataInitPortOffline (PhysicalDieId, SataIndex, DieBusNum, PortNum, FchDataPtr);
  }
}
