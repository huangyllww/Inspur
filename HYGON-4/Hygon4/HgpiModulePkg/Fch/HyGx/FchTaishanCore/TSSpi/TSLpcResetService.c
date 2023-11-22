/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch LPC controller
 *
 * Init LPC Controller features.
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

#define FILECODE  PROC_FCH_TAISHAN_TSSPI_TSLPCRESETSERVICE_FILECODE
#define SPI_BASE  0xFEC10000ul

VOID
FchSetQualModePei (
  IN       UINT32                    SpiQualMode,
  IN       HYGON_CONFIG_PARAMS         *StdHeader
  );

/**
 * FchInitTaishanResetLpcPciTable - Lpc (Spi) device registers
 * initial during the power on stage.
 *
 *
 *
 *
 */
REG8_MASK  FchInitTaishanResetLpcPciTable[] =
{
  //
  // LPC Device (Bus 0, Dev 20, Func 3)
  //
  { 0x00,          LPC_BUS_DEV_FUN, 0                  },

  { FCH_LPC_REG48, 0x00,            BIT0 + BIT1 + BIT2 },
  { FCH_LPC_REG7C, 0x00,            BIT0 + BIT2        },
  //
  // Set 0xBA [6:5] = 11 improve SPI timing margin. (SPI Prefetch enhancement)
  //
  { FCH_LPC_REGBA, 0x9F,            BIT5 + BIT6        },
  // Force EC_PortActive to 1 to fix possible IR non function issue when NO_EC_SUPPORT is defined
  { FCH_LPC_REGA4, 0xFE,            BIT0               },
  { 0xFF,          0xFF,            0xFF               },
};

/**
 * FchInitResetLpcProgram - Config Lpc controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetLpcProgram (
  IN       VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;
  HYGON_CONFIG_PARAMS   *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  PciAndThenOr32 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REG6C, 0xFFFFFF00, 0);

  ProgramPciByteTable (
    (REG8_MASK *)(&FchInitTaishanResetLpcPciTable[0]),
    sizeof (FchInitTaishanResetLpcPciTable) / sizeof (REG8_MASK),
    StdHeader
    );

  if ( LocalCfgPtr->Spi.LpcClk0 ) {
    PciAndThenOr8 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REGD0 + 1, 0xDF, 0x20);
  } else {
    PciAndThenOr8 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REGD0 + 1, 0xDF, 0);
  }

  if ( LocalCfgPtr->Spi.LpcClk1 ) {
    PciAndThenOr8 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REGD0 + 1, 0xBF, 0x40);
  } else {
    PciAndThenOr8 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REGD0 + 1, 0xBF, 0);
  }

  if ( LocalCfgPtr->Misc2.LegacyFree ) {
    PciAndThenOr32 (((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REG44), 00, 0x0003C000);
  } else {
    PciAndThenOr32 (((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REG44), 00, 0xFF03FFD5);
  }
}

/**
 * FchPlatformSpiQeCheck - Platform SPI Qual Enable
 *
 *
 *
 * @param[in] FchDataPtr  - FchData Pointer.
 *
 */
BOOLEAN
FchPlatformSpiQeCheck (
  IN       VOID     *FchDataPtr
  )
{
  UINTN                 RomSigStartingAddr;
  UINT8                 Value8;
  UINT8                 DummyCycle;
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;
  HYGON_CONFIG_PARAMS   *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  if (PcdGet8 (PcdRunEnvironment) > 0) {
    if (LocalCfgPtr->Spi.SpiMode == FCH_SPI_MODE_QUAL_144) {
      IdsLibDebugPrint (MAIN_FLOW, "Set QPR Dummy cycle to 6 for 1-4-4 mode\n");
      RwMem (SPI_BASE + FCH_SPI_MMIO_REG32, AccessWidth16, ~((UINT32)(0xF << 8)), ((6) << 8));
      return TRUE;
    }
  }

  if (LocalCfgPtr->Misc2.QeEnabled) {
    return TRUE;
  }

  GetRomSigPtr (&RomSigStartingAddr, StdHeader);
  Value8     = *(UINT8 *)(UINTN)(RomSigStartingAddr + 0x43);
  DummyCycle = *(UINT8 *)(UINTN)(RomSigStartingAddr + 0x45);
  if ((ACPIMMIO8 (0xFED8060C) == 0x20) && (Value8 > 1) && (Value8 < 8)) {
    if (LocalCfgPtr->Spi.SpiMode == 0) {
      LocalCfgPtr->Spi.SpiMode = FCH_SPI_MODE_QUAL_144;
    }

    if (LocalCfgPtr->Spi.SpiMode == FCH_SPI_MODE_QUAL_144) {
      if (DummyCycle != 0xff) {
        RwMem (SPI_BASE + FCH_SPI_MMIO_REG32, AccessWidth16, ~((UINT32)(0xF << 8)), ((6) << 8));
      }
    } else {
      FchConfigureSpiDeviceDummyCycle ((UINT32)(*(UINT8 *)(UINTN)(0xFED8060C)), LocalCfgPtr->Spi.SpiMode);
    }

    return TRUE;
  }

  if ((Value8 > 1) && (Value8 < 8) && (LocalCfgPtr->Spi.SpiMode == 0)) {
    LocalCfgPtr->Spi.SpiMode = Value8;
    Value8 = *(UINT8 *)(UINTN)(RomSigStartingAddr + 0x44);
    LocalCfgPtr->Spi.SpiFastSpeed = Value8 + 1;
    return TRUE;
  }

  return FALSE;
}

/**
 * FchSetQualModePei - Set SPI Qual Mode
 *
 *
 *
 * @param[in] SpiQualMode- Spi Qual Mode.
 * @param[in] StdHeader  - Standard Header.
 *
 */
VOID
FchSetQualModePei (
  IN       UINT32                    SpiQualMode,
  IN       HYGON_CONFIG_PARAMS         *StdHeader
  )
{
  UINT32  SpiMode;

  SpiMode = ACPIMMIO32 (SPI_BASE + FCH_SPI_MMIO_REG00);
  if ((SpiMode & (BIT18 + BIT29 + BIT30)) != ((SpiQualMode & 1) << 18) + ((SpiQualMode & 6) << 28)) {
    RwMem (
      SPI_BASE + FCH_SPI_MMIO_REG00,
      AccessWidth32,
      ~(UINT32)(BIT18 + BIT29 + BIT30),
      ((SpiQualMode & 1) << 18) + ((SpiQualMode & 6) << 28)
      );
  }
}

/**
 * FchInitResetSpi - Config Spi controller during Power-On
 *
 *
 *
 * @param[in] FchDataPtr Fch configuration structure pointer.
 *
 */
VOID
FchInitResetSpi (
  IN VOID     *FchDataPtr
  )
{
  FCH_RESET_DATA_BLOCK  *LocalCfgPtr;
  HYGON_CONFIG_PARAMS   *StdHeader;

  LocalCfgPtr = (FCH_RESET_DATA_BLOCK *)FchDataPtr;
  StdHeader   = LocalCfgPtr->StdHeader;

  HGPI_TESTPOINT (TpFchInitResetSpi, NULL);
  //
  // Set Spi ROM Base Address
  //
  PciAndThenOr32 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REGA0, 0x001F, SPI_BASE);

  RwMem (SPI_BASE + FCH_SPI_MMIO_REG00, AccessWidth32, 0xFFFFFFFF, (BIT19 + BIT24 + BIT25 + BIT26));
  RwMem (SPI_BASE + FCH_SPI_MMIO_REG0C, AccessWidth32, 0xFFC0FFFF, 0);

  // Enable SPI Prefetch for USB, set LPC cfg 0xBA bit 7 to 1.
  PciAndThenOr16 ((LPC_BUS_DEV_FUN << 12) + FCH_LPC_REGBA, 0xFFFF, BIT7);
}
