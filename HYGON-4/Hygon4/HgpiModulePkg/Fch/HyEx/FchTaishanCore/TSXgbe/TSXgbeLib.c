/* $NoKeywords:$ */

/**
 * @file
 *
 * Config Fch xGbE controller
 *
 * Init Usb Controller features (PEI phase).
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
#define FILECODE  PROC_FCH_TAISHAN_TSXGBE_TSXGBELIB_FILECODE

/**
 * FchTsXgbePortClockGate - Clock Gate xGbE port
 *
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] Port           Port Number in current Die
 *
 */
VOID
FchTsXgbePortClockGate (
  IN  UINT8    PhysicalDieId,
  IN  UINT32   DieBusNum,
  IN  UINT8    Port
  )
{
  FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_CLK_GATE_HYEX + Port * 4, ~(UINT32) BIT0, BIT0, NULL);
}

/**
 * FchTsXgbePortPlatformConfig - Initialize xGbE port
 *
 * @param[in] PhysicalDieId  Physical Die Id of IOD.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] PortNum        Port Number in current Die
 * @param[in] SystemPort     Port Number in the system
 * @param[in] PortData       Port configuration structure pointer.
 *
 */
VOID
FchTsXgbePortPlatformConfig (
  IN  UINT8    PhysicalDieId,
  IN  UINT32   DieBusNum,
  IN  UINT8    PortNum,
  IN  UINT32   SystemPort,
  IN  VOID     *PortData
  )
{
  UINT32         XgbePortProperty;
  FCH_XGBE_PORT  *FchXgbePort;

  FchXgbePort = (FCH_XGBE_PORT *)PortData;

  SystemPort &= 0xFF;
  FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_PROPERTY0_HYEX + PortNum * 0x20, 0xFFFFFF00, SystemPort, NULL);

  XgbePortProperty  = (UINT32)(FchXgbePort->XgbePortPlatformConfig) << 8;
  XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortSupportedSpeed) << 23;
  XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortConnectedType) << 28;
  FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_PROPERTY0_HYEX + PortNum * 0x20, 0x0870F0FF, XgbePortProperty, NULL);

  if (FchXgbePort->XgbePortConnectedType == 1) {
    //
    // SFP+
    //
    XgbePortProperty  = (UINT32)(FchXgbePort->XgbePortSfpI2cAddress) << 8;
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortSfpTxFaultGpio) << 12;
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortSfpRsGpio) << 16;
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortSfpModAbsGpio) << 20;
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortSfpRxLosGpio) << 24;
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortSfpGpioMask) << 28;
    FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_PROPERTY3_HYEX + PortNum * 0x20, 0x000008FF, XgbePortProperty, NULL);

    XgbePortProperty  = (UINT32)(FchXgbePort->XgbePortSfpTwiAddress & 0x7);
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortSfpTwiBus) << 4;
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortSfpTwiAddress & 0xF8) << 5;
    FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_PROPERTY4_HYEX + PortNum * 0x20, 0x000008FF, XgbePortProperty, NULL);

    //
    // I2C PAD MUX
    //
    FchSmnRW (
      DieBusNum,
      FCH_TS_XGBE_PAD_MUX0_HYEX,
      ~(UINT32)(0x0F << (PortNum * 8)),
      (UINT32)(FchXgbePort->XgbaPortPadI2C) << (PortNum * 8),
      NULL
      );
  } else if (FchXgbePort->XgbePortConnectedType == 2) {
    //
    // MDIO
    //
    XgbePortProperty = (UINT32)(FchXgbePort->XgbePortMdioId) << 16;
    FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_PROPERTY0_HYEX + PortNum * 0x20, 0xFFF0FFFF, XgbePortProperty, NULL);

    XgbePortProperty = (UINT32)(FchXgbePort->XgbePortMdioResetType);
    if (FchXgbePort->XgbePortMdioResetType == 1) {
      //
      // I2C GPIO
      //
      XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortMdioResetI2cAddress) << 8;
      XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortResetGpioNum) << 12;
      FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_PROPERTY3_HYEX + PortNum * 0x20, 0xFFFF08FC, XgbePortProperty, NULL);

      //
      // I2C PAD MUX
      //
      FchSmnRW (
        DieBusNum,
        FCH_TS_XGBE_PAD_MUX0_HYEX,
        ~(UINT32)(0x0F << (PortNum * 8)),
        (UINT32)(FchXgbePort->XgbaPortPadI2C) << (PortNum * 8),
        NULL
        );
    } else if (FchXgbePort->XgbePortMdioResetType == 2) {
      //
      // Integrated GPIO
      //
      XgbePortProperty |= (UINT32)(FchXgbePort->XgbePortResetGpioNum) << 4;
      FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_PROPERTY3_HYEX + PortNum * 0x20, 0xFFFFFFCC, XgbePortProperty, NULL);

      // GPIO PAD
      FchSmnRW (
        DieBusNum,
        FCH_TS_XGBE_PAD_MUX1_HYEX,
        ~(UINT32)(0x0F << (PortNum * 8)),
        (UINT32)(FchXgbePort->XgbaPortPadGpio) << (PortNum * 8),
        NULL
        );
    }

    //
    // MDIO PAD MUX
    //
    FchSmnRW (
      DieBusNum,
      FCH_TS_XGBE_PAD_MUX0_HYEX,
      ~(UINT32)(0xF0 << (PortNum * 8)),
      (UINT32)(FchXgbePort->XgbaPortPadMdio) << (PortNum * 8 + 4),
      NULL
      );
  }

  if (FchXgbePort->XgbaPortRedriverPresent) {
    //
    // Redriver
    //
    XgbePortProperty  = (UINT32)(FchXgbePort->XgbaPortRedriverAddress) << 16;
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbaPortRedriverInterface) << 23;
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbaPortRedriverLane) << 24;
    XgbePortProperty |= (UINT32)(FchXgbePort->XgbaPortRedriverModel) << 28;
    XgbePortProperty |= BIT31;
    FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_PROPERTY4_HYEX + PortNum * 0x20, 0x0000FFFF, XgbePortProperty, NULL);
  }
}

/**
 * FchTsXgbePortMacAddress - Initialize xGbE port
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] PortNum        Port Number in current Die
 * @param[in] MacData        Port configuration structure pointer.
 *
 */
VOID
FchTsXgbePortMacAddress (
  IN  UINT8    PhysicalDieId,
  IN  UINT32   DieBusNum,
  IN  UINT8    PortNum,
  IN  VOID     *MacData
  )
{
  UINT32        FchXgbeMacAddress;
  FCH_XGBE_MAC  *FchXgbePortMac;

  FchXgbePortMac    = (FCH_XGBE_MAC *)MacData;
  FchXgbeMacAddress = FchXgbePortMac->XgbePortMacAddressLow;
  FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_MAC_ADDR_LOW_HYEX + PortNum * 0x8, 0x00, FchXgbeMacAddress, NULL);

  FchXgbeMacAddress  = FchXgbePortMac->XgbePortMacAddressHigh;
  FchXgbeMacAddress &= 0xFFFF;
  FchXgbeMacAddress |= BIT31;
  FchSmnRW (DieBusNum, FCH_TS_XGBE_PORT0_MAC_ADDR_HIGH_HYEX + PortNum * 0x8, 0x00, FchXgbeMacAddress, NULL);
}

/**
 * FchTsXgbePortPtpInit - Initialize xGbE port
 *
 * @param[in] PhysicalDieId  Physical Die Id of lite IOD.
 * @param[in] DieBusNum      Bus Number of current Die
 * @param[in] PortNum        Port Number in current Die
 * @param[in] PortData       Port configuration structure pointer.
 *
 */
VOID
FchTsXgbePortPtpInit (
  IN  UINT8    PhysicalDieId,
  IN  UINT32   DieBusNum,
  IN  UINT8    PortNum,
  IN  VOID     *PortData
  )
{
}

/**
 * FchTsXgbePadEnable - Enable Pad IOMUX
 *
 *
 * @param[in] DieBusNum  Bus Number of current Die
 * @param[in] XgbeData   Pointer to xGbE data structure
 *
 */
VOID
FchTsXgbePadEnable (
  IN  UINT32   DieBusNum,
  IN  VOID     *FchDataPtr
  )
{
  UINT32          FchAcpiMmioBase;
  FCH_DATA_BLOCK  *LocalCfgPtr;

  LocalCfgPtr     = (FCH_DATA_BLOCK *)FchDataPtr;
  FchAcpiMmioBase = LocalCfgPtr->HwAcpi.FchAcpiMmioBase;

  if (LocalCfgPtr->Xgbe.XgbeMdio0Enable) {
    RwMem (FchAcpiMmioBase + IOMUX_BASE + 0x0A, AccessWidth8, 0x00, 0x3);
    RwMem (FchAcpiMmioBase + IOMUX_BASE + 0x28, AccessWidth8, 0x00, 0x2);
  }

  if (LocalCfgPtr->Xgbe.XgbeMdio1Enable) {
    RwMem (FchAcpiMmioBase + IOMUX_BASE + 0x09, AccessWidth8, 0x00, 0x2);
    RwMem (FchAcpiMmioBase + IOMUX_BASE + 0x17, AccessWidth8, 0x00, 0x2);
  }

  if (LocalCfgPtr->Xgbe.XgbeSfpEnable) {
    RwMem (FchAcpiMmioBase + IOMUX_BASE + 0x93, AccessWidth8, 0x00, 0x1);
    RwMem (FchAcpiMmioBase + IOMUX_BASE + 0x94, AccessWidth8, 0x00, 0x1);
  }
}
