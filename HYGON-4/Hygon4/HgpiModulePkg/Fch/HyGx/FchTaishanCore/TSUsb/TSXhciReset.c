/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch Xhci controller
 *
 * Init Xhci Controller features (PEI phase).
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
#include <Library/HygonCapsuleLib.h>
#include <Library/IdsLib.h>
#define FILECODE  FCH_TAISHAN_FCHTAISHANCORE_TAISHAN_TSUSB_TSXHCIRESET_FILECODE

VOID
FchInitRecoveryXhci (
  IN  VOID     *FchDataPtr
  );

/**
 * FchInitResetXhci - Config Xhci controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetXhci (
  IN  VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;
  UINT32                DieBusNum;
  UINT32                PhysicalDieId;
  UINT32                LogicalDieId;
  UINT8                 UsbIndex;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;

  HGPI_TESTPOINT (TpFchInitResetUsb, NULL);

  if (PcdGetBool (PcdHygonUsbSupport) == FALSE) {
    return;
  }

  if (CheckHygonSoc () == FALSE) {
    return;
  }

  PhysicalDieId = 0;
  LogicalDieId  = 0;
  for(UsbIndex = 0; UsbIndex < USB_NUMBER_PER_DUJIANG; UsbIndex++) {
    DieBusNum = ReadSocDieBusNum (0, LogicalDieId, 0);
    
    //Raise USB Axi Clock to 400M
    FchSmnRW (DieBusNum, USB_SPACE_HYGX (PhysicalDieId, UsbIndex, FCH_SMN_MISC_BASE_HYGX + FCH_MISC_REG40), ~ (UINT32) BIT2, 0, LocalCfgPtr->StdHeader);

    // for Boundary Scan
    FchSmnRW (DieBusNum, USB_SPACE_HYGX (PhysicalDieId, UsbIndex, FCH_TS_USB_SPARE2_REG_HYGX), ~(UINT32)BIT31, BIT31, LocalCfgPtr->StdHeader);
    if ((LocalCfgPtr->FchReset.XhciEnable & (UINT64)BIT (UsbIndex)) == 0) {
      // Disable USB
      FchSmnRW (DieBusNum, NBIO_SPACE2 (PhysicalDieId, 0, (UsbIndex == 0 ? NBIF2_USB_STRAP0_ADDRESS_HYGX : NBIF3_USB_STRAP0_ADDRESS_HYGX)), ~(UINT32)BIT28, BIT28, LocalCfgPtr->StdHeader);
      continue;
    }

    if ((ReadFchSleepType (LocalCfgPtr->StdHeader) == ACPI_SLPTYP_S3) || (HygonCapsuleGetStatus ())) {
      FchTSXhciInitS3ExitProgram (0, (UINT8)PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr);
    } else {
      FchTSXhciInitBootProgram (0, (UINT8)PhysicalDieId, UsbIndex, DieBusNum, FchDataPtr);
    }
  }
}

/**
 * FchInitRecoveryLpc - Config Xhci controller during Crisis
 * Recovery
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitRecoveryXhci (
  IN  VOID     *FchDataPtr
  )
{
}
