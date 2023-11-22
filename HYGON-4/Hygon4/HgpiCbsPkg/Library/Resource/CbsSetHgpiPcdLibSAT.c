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

// This file is auto generated, don't edit it manually

#include <Base.h>
#include <Library/DebugLib.h>
#include "HygonCbsVariable.h"
#include <Library/PcdLib.h>
#include <Library/HygonSocBaseLib.h>

VOID
CbsSetHgpiPcds (
  IN       VOID *CbsVariable
  )
{
  CBS_CONFIG  *Setup_Config;
  UINT32      SataEnable;
  UINT64      XhciEnable;
  UINT32      CpuModel;

  CpuModel = GetHygonSocModel();

  if (CbsVariable == NULL) {
    ASSERT (FALSE);
    return;
  }

  Setup_Config = (CBS_CONFIG *)CbsVariable;
  if (Setup_Config->CbsCmnCpuPfeh != 3) {
    PcdSetBoolS (PcdHygonCcxCfgPFEHEnable, Setup_Config->CbsCmnCpuPfeh);
  }

  if (Setup_Config->CbsCmnCpuCpb != 1) {
    PcdSet8S (PcdHygonCpbMode, Setup_Config->CbsCmnCpuCpb);
  }

  if (Setup_Config->CbsCmnCpuCstateCtrl != 0xFF) {
    PcdSet8S (PcdHygonCStateMode, Setup_Config->CbsCmnCpuCstateCtrl);
  }

  if (Setup_Config->CbsCmnSmtMode != 0x1) {
    PcdSet8S (PcdHygonSmtMode, Setup_Config->CbsCmnSmtMode);
  }

  if (Setup_Config->CbsDfCmnCstateCtrl != 0xFF) {
    PcdSet8S (PcdHygonDfCStateMode, Setup_Config->CbsDfCmnCstateCtrl);
  }

  if (Setup_Config->CbsCmnCpuLApicMode != 0xFF) {
    PcdSetBoolS (PcdX2ApicMode, Setup_Config->CbsCmnCpuLApicMode);
  }

  if (Setup_Config->CbsCmnCoreWatchdogCtrl != 0x3) {
    PcdSetBoolS (PcdHygonCpuWdtEn, Setup_Config->CbsCmnCoreWatchdogCtrl);
  }

  if (Setup_Config->CbsCmnCpuGenDowncoreCtrl != 0) {
    PcdSet8S (PcdHygonDownCoreMode, Setup_Config->CbsCmnCpuGenDowncoreCtrl);
  }

  if (Setup_Config->CbsCmnCpuL1StreamHwPrefetcher != 3) {
    PcdSetBoolS (PcdHygonL1StreamPrefetcher, Setup_Config->CbsCmnCpuL1StreamHwPrefetcher);
  }

  if (Setup_Config->CbsCmnCpuL2StreamHwPrefetcher != 3) {
    PcdSetBoolS (PcdHygonL2StreamPrefetcher, Setup_Config->CbsCmnCpuL2StreamHwPrefetcher);
  }

  if (Setup_Config->CbsDbgPoisonConsumption != 0xF) {
    PcdSetBoolS (PcdHygonNbioPoisonConsumption, Setup_Config->CbsDbgPoisonConsumption);
  }

  if (Setup_Config->CbsNbioRASControl != 0xF) {
    PcdSetBoolS (PcdHygonNbioRASControl, Setup_Config->CbsNbioRASControl);
  }

  if (Setup_Config->CbsCmnNbioEfficiencyOptimizedMode != 0xF) {
    PcdSetBoolS (PcdEfficiencyOptimizedMode, Setup_Config->CbsCmnNbioEfficiencyOptimizedMode);
  }

  if (Setup_Config->CbsCmnNbioPSIDisable != 0xf) {
    PcdSetBoolS (PcdHygonPSIDisable, Setup_Config->CbsCmnNbioPSIDisable);
  }

  if (Setup_Config->CbsDbgGnbDbgACSEnable != 0xF) {
    PcdSetBoolS (PcdCfgACSEnable, Setup_Config->CbsDbgGnbDbgACSEnable);
  }

  if (Setup_Config->CbsGnbDbgPcieAriSupport != 0xf) {
    PcdSetBoolS (PcdCfgPcieAriSupport, Setup_Config->CbsGnbDbgPcieAriSupport);
  }

  if (Setup_Config->CbsCfgPcieLoopbackMode != 2) {
    PcdSetBoolS (PcdCfgPcieLoopbackMode, Setup_Config->CbsCfgPcieLoopbackMode);
  }

  PcdSet16S (PcdCfgPcieCrsDelay, Setup_Config->CbsCfgPcieCrsDelay);
  PcdSet16S (PcdCfgPcieCrsLimit, Setup_Config->CbsCfgPcieCrsLimit);
  if (Setup_Config->CbsCmnGnbNbIOMMU != 0xf) {
    PcdSetBoolS (PcdCfgIommuSupport, Setup_Config->CbsCmnGnbNbIOMMU);
  }

  if (Setup_Config->CbsPcieAspmControl != 0xf) {
    PcdSet8S (PcdPcieAspmControl, Setup_Config->CbsPcieAspmControl);
  }

  if (Setup_Config->CbsPcieEqMode != 0xff) {
    PcdSet8S (PcdPcie32GTBypassEQMode, Setup_Config->CbsPcieEqMode);
  }

  if (Setup_Config->CbsPcie4Symbol != 0xff) {
    PcdSet8S (PcdHygonNbioSpcMode16GT, Setup_Config->CbsPcie4Symbol);
  }

  if (Setup_Config->CbsPcieGen5Precoding != 0xff) {
    PcdSet8S (PcdPcie32GTPrecoding, Setup_Config->CbsPcieGen5Precoding);
  }

  if (Setup_Config->CbsPcieMaxpayloadSize != 0xff) {
    PcdSet8S (PcdCfgMaxPayloadSize, Setup_Config->CbsPcieMaxpayloadSize);
  }

  if (Setup_Config->CbsCmnDisableSideband != 0xF) {
    PcdSetBoolS (PcdHygonDisableSideband, Setup_Config->CbsCmnDisableSideband);
  }

  if (Setup_Config->CbsCmnDisableL1wa != 0xF) {
    PcdSetBoolS (PcdHygonDisableL1wa, Setup_Config->CbsCmnDisableL1wa);
  }

  if (Setup_Config->CbsCmnDisableBridgeDis != 0xF) {
    PcdSetBoolS (PcdHygonDisableBridgeDis, Setup_Config->CbsCmnDisableBridgeDis);
  }

  if (Setup_Config->CbsCmnDisableIrqPoll != 0xF) {
    PcdSetBoolS (PcdHygonDisableIrqPoll, Setup_Config->CbsCmnDisableIrqPoll);
  }

  if (Setup_Config->CbsCmnIrqSetsBridgeDis != 0xF) {
    PcdSetBoolS (PcdHygonIrqSetBridgeDis, Setup_Config->CbsCmnIrqSetsBridgeDis);
  }

  if (Setup_Config->CbsPcieHotplugSupport != 0xF) {
    PcdSetBoolS (PcdCfgPcieHotplugSupport, Setup_Config->CbsPcieHotplugSupport);
  }

  SataEnable = PcdGet32 (PcdSataEnableHyGx);
  if (Setup_Config->CbsCmnFchSataEnable0 == 0) {
    SataEnable &= (~BIT0);
  }

  if (CpuModel == HYGON_EX_CPU) {
    if (Setup_Config->CbsCmnFchSataEnable1 == 0) {
      SataEnable &= (~BIT1);
    }

    if (Setup_Config->CbsCmnFchSataEnable2 == 0) {
      SataEnable &= (~BIT2);
    }

    if (Setup_Config->CbsCmnFchSataEnable3 == 0) {
      SataEnable &= (~BIT3);
    }
  }

  PcdSet32S (PcdSataEnableHyGx, SataEnable);
  if (Setup_Config->CbsCmnFchSataClass != 0xf) {
    PcdSet8S (PcdSataClass, Setup_Config->CbsCmnFchSataClass);
  }

  if (Setup_Config->CbsCmnFchSataRasSupport != 0xf) {
    PcdSetBoolS (PcdSataRasSupport, Setup_Config->CbsCmnFchSataRasSupport);
  }

  if (Setup_Config->CbsCmnFchSataAhciDisPrefetchFunction != 0xf) {
    PcdSetBoolS (PcdSataAhciDisPrefetchFunction, Setup_Config->CbsCmnFchSataAhciDisPrefetchFunction);
  }

  if (Setup_Config->CbsDbgFchSataAggresiveDevSlpP0 != 0xF) {
    PcdSetBoolS (PcdSataDevSlpPort0, Setup_Config->CbsDbgFchSataAggresiveDevSlpP0);
  }

  PcdSet8S (PcdSataDevSlpPort0Num, Setup_Config->CbsDbgFchSataDevSlpPort0Num);
  if (Setup_Config->CbsDbgFchSataAggresiveDevSlpP1 != 0xF) {
    PcdSetBoolS (PcdSataDevSlpPort1, Setup_Config->CbsDbgFchSataAggresiveDevSlpP1);
  }

  PcdSet8S (PcdSataDevSlpPort1Num, Setup_Config->CbsDbgFchSataDevSlpPort1Num);
  
  if (CpuModel == HYGON_EX_CPU) {
    if (Setup_Config->CbsCmnFchUsbXHCI0Enable != 0xf) {
      PcdSetBoolS (PcdXhci0EnableHyEx, Setup_Config->CbsCmnFchUsbXHCI0Enable);
    }

    if (Setup_Config->CbsCmnFchUsbXHCI1Enable != 0xf) {
      PcdSetBoolS (PcdXhci1EnableHyEx, Setup_Config->CbsCmnFchUsbXHCI1Enable);
    }

    if (Setup_Config->CbsCmnFchUsbXHCI2Enable != 0xf) {
      PcdSetBoolS (PcdXhci2EnableHyEx, Setup_Config->CbsCmnFchUsbXHCI2Enable);
    }

    if (Setup_Config->CbsCmnFchUsbXHCI3Enable != 0xf) {
      PcdSetBoolS (PcdXhci3EnableHyEx, Setup_Config->CbsCmnFchUsbXHCI3Enable);
    }

    if (Setup_Config->CbsCmnFchUsbXHCI4Enable != 0xf) {
      PcdSetBoolS (PcdXhci4EnableHyEx, Setup_Config->CbsCmnFchUsbXHCI4Enable);
    }

    if (Setup_Config->CbsCmnFchUsbXHCI5Enable != 0xf) {
      PcdSetBoolS (PcdXhci5EnableHyEx, Setup_Config->CbsCmnFchUsbXHCI5Enable);
    }

    if (Setup_Config->CbsCmnFchUsbXHCI6Enable != 0xf) {
      PcdSetBoolS (PcdXhci6EnableHyEx, Setup_Config->CbsCmnFchUsbXHCI6Enable);
    }

    if (Setup_Config->CbsCmnFchUsbXHCI7Enable != 0xf) {
      PcdSetBoolS (PcdXhci7EnableHyEx, Setup_Config->CbsCmnFchUsbXHCI7Enable);
    }
  }
  else if (CpuModel == HYGON_GX_CPU) {
    XhciEnable = PcdGet64 (PcdXhciEnableHyGx);
    if (Setup_Config->CbsCmnFchUsbXHCI0Enable == 0) {
      XhciEnable &= (~BIT0);
    }

    if (Setup_Config->CbsCmnFchUsbXHCI1Enable == 0) {
      XhciEnable &= (~BIT1);
    }
    PcdSet64S (PcdXhciEnableHyGx, XhciEnable);
  }
  
  PcdSet8S (PcdPwrFailShadow, Setup_Config->CbsCmnFchSystemPwrFailShadow);
  PcdSet8S (PcdFchUart0LegacyEnable, Setup_Config->CbsCmnFchUart0LegacyConfig);
  PcdSet8S (PcdFchUart1LegacyEnable, Setup_Config->CbsCmnFchUart1LegacyConfig);
  PcdSet8S (PcdFchUart2LegacyEnable, Setup_Config->CbsCmnFchUart2LegacyConfig);
  PcdSet8S (PcdFchUart3LegacyEnable, Setup_Config->CbsCmnFchUart3LegacyConfig);

  if (Setup_Config->CbsCmnNtbEnable != 0) {
    PcdSetBoolS (PcdCfgNTBEnable, Setup_Config->CbsCmnNtbEnable);
  }

  if (Setup_Config->CbsCmnNtbLocation != 0xFF) {
    PcdSet8S (PcdCfgNTBLocation, Setup_Config->CbsCmnNtbLocation);
  }

  if (Setup_Config->CbsCmnNtbPCIeCore != 0xf) {
    PcdSet8S (PcdCfgNTBPcieCoreSel, Setup_Config->CbsCmnNtbPCIeCore);
  }

  if (Setup_Config->CbsCmnNtbMode != 0xf) {
    PcdSet8S (PcdCfgNTBMode, Setup_Config->CbsCmnNtbMode);
  }

  if (Setup_Config->CbsCmnNtbLinkSpeed != 0xf) {
    PcdSet8S (PcdCfgNTBLinkSpeed, Setup_Config->CbsCmnNtbLinkSpeed);
  }

  if (Setup_Config->CbsCmnNtbBAR1Window != 0xff) {
    PcdSet8S (PcdCfgNTBBAR1Size, Setup_Config->CbsCmnNtbBAR1Window);
  }

  if (Setup_Config->CbsCmnNtbBAR23Window != 0xff) {
    PcdSet8S (PcdCfgNTBBAR23Size, Setup_Config->CbsCmnNtbBAR23Window);
  }

  if (Setup_Config->CbsCmnNtbBAR45Window != 0xff) {
    PcdSet8S (PcdCfgNTBBAR45Size, Setup_Config->CbsCmnNtbBAR45Window);
  }

  if (Setup_Config->CbsDfNumaNodePerSocket != 0xFF) {
    PcdSet8S (PcdNumaNodePerSocket, Setup_Config->CbsDfNumaNodePerSocket);
  }

  PcdSetBoolS (PcdHygonHgpiSmee, Setup_Config->CbsCmnSmeeCtrl);

  PcdSet8S (PcdHygonAbove4GMmioLimitBit, Setup_Config->CbsAbove4GMmioLimitBit);

  if(PcdGetBool(PcdDldoPsmMarginSupport)){
    if (Setup_Config->CbsDldoPsmMarginCtl != 0x0) {
      PcdSet8S (PcdDldoPsmMargin, Setup_Config->CbsDldoPsmMargin);
    }
  }

  if (Setup_Config->CbsCmnCpuMcaErrThreshEn != 0xFF) {
    PcdSetBoolS (PcdMcaErrThreshEn, Setup_Config->CbsCmnCpuMcaErrThreshEn);
  }
//  if ((Setup_Config->CbsCmnCpuMcaErrThreshEn == 0x1)) {                           byo231109 -
//    PcdSet16S (PcdMcaErrThreshCount, Setup_Config->CbsCmnCpuMcaErrThreshCount);   byo231109 -
//  }                                                                               byo231109 -

  if (Setup_Config->CbsCmnCxlControl != 0xFF) {
    PcdSetBoolS (PcdCfgCxlEnable, Setup_Config->CbsCmnCxlControl);
  }
  PcdSet8S (PcdCxlMemAttr, Setup_Config->CbsCmnCxlMemAttr);
}
