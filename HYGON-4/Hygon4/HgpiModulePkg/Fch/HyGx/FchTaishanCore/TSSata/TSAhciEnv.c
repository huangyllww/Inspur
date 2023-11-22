/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch SATA controller (AHCI mode)
 *
 * Init SATA AHCI features.
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
#define FILECODE  PROC_FCH_TAISHAN_TSSATA_TSAHCIENV_FILECODE

/**
 * FchInitEnvSataAhci - Config SATA Ahci controller before PCI
 * emulation
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitEnvSataAhci (
  IN  VOID     *FchDataPtr
  )
{
}

VOID
FchInitEnvSataAhciTS (
  IN  UINT8    PhysicalDieId,
  IN  UINT8    SataIndex,
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  FCH_DATA_BLOCK       *LocalCfgPtr;
  HYGON_CONFIG_PARAMS  *StdHeader;

  LocalCfgPtr = (FCH_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  //
  // Class code
  //
  // PciAndThenOr32 ((((SataBusNum + TAISHAN_SATA_DEV_FUN) << 12) + FCH_SATA_REG08), 0, 0x01060151);
  FchSmnRW (DieBusNum, NBIO_SPACE2 (PhysicalDieId, 0, (SataIndex == 0 ? NBIF2_SATA_STRAP13_ADDRESS_HYGX : NBIF3_SATA_STRAP13_ADDRESS_HYGX)), 0x00, 0x00010601, StdHeader);
  //
  // Device ID
  //
  if ( LocalCfgPtr->Sata.SataClass == SataAhci7804 ) {
    // PciAndThenOr16 ((((SataBusNum + TAISHAN_SATA_DEV_FUN) << 12) + FCH_SATA_REG02), 0, TAISHAN_FCH_SATA_HYGONAHCI_DID);
    FchSmnRW (
      DieBusNum,
      NBIO_SPACE2 (PhysicalDieId, 0, (SataIndex == 0 ? NBIF2_SATA_STRAP0_ADDRESS_HYGX : NBIF3_SATA_STRAP0_ADDRESS_HYGX)),
      0xFFFF0000,
      TAISHAN_FCH_SATA_HYGONAHCI_DID,
      StdHeader
      );
  } else {
    // PciAndThenOr16 ((((SataBusNum + TAISHAN_SATA_DEV_FUN) << 12) + FCH_SATA_REG02), 0, TAISHAN_FCH_SATA_AHCI_DID);
    FchSmnRW (
      DieBusNum,
      NBIO_SPACE2 (PhysicalDieId, 0, (SataIndex == 0 ? NBIF2_SATA_STRAP0_ADDRESS_HYGX : NBIF3_SATA_STRAP0_ADDRESS_HYGX)),
      0xFFFF0000,
      TAISHAN_FCH_SATA_AHCI_DID,
      StdHeader
      );
  }

  //
  // SSID
  //
  if (LocalCfgPtr->Sata.SataAhciSsid != NULL ) {
    FchSmnRW (
      DieBusNum,
      NBIO_SPACE2 (PhysicalDieId, 0, (SataIndex == 0 ? PCICFG_NBIF2_SATA_ADDRESS_HYGX : PCICFG_NBIF3_SATA_ADDRESS_HYGX)) + 0x4C,
      0x00,
      LocalCfgPtr->Sata.SataAhciSsid,
      StdHeader
      );
  }
}
